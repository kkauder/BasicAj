#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TProfile.h>
#include <TLine.h>

#include <TLorentzVector.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TChain.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <exception>

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldTUnfold.h"

using namespace std;

int MatchGeantToPythia ( ) {
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);
  TLegend* leg = 0;
  
  float RCut = 0.4;
  float EtaCut = 1.0-RCut;

  // Input
  // -----
  TString PpLevelFile = "AjResults/AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root"; // pp-like events
  TString McLevelFile  = "AjResults/Groom_Aj_TrueMB_NoEff_GeantMc.root"; // Reference (particle level) jets
  // TString PpLevelFile = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root"; // pp-like events
  // TString McLevelFile  = "HThistos/Groom_Aj_TrueMB_NoEff_GeantMc.root"; // Reference (particle level) jets
  //TString McLevelFile  = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_GeantMc.root"; // Reference (particle level) jets

  // Output
  // ------
  TString OutFileName = "AjResults/TrainedWith_";
  OutFileName += gSystem->BaseName(McLevelFile);
    
  TString PlotBase = "plots/TrainedWith_";
  PlotBase += gSystem->BaseName(McLevelFile);
  PlotBase.ReplaceAll(".root","");
  

  // Refmult Cut
  // -----------
  int RefMultCut=0;

  // Load MC level tree
  // ------------------
  TChain* McJets = new TChain("TriggeredTree");
  McJets->Add ( McLevelFile );
  McJets->BuildIndex("runid","eventid");

  // The leading jet above 10 GeV that caused
  // When appropriate, it has the HT!
  // this event to be recorded
  TClonesArray* McTrigger = new TClonesArray("TLorentzVector");
  McJets->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  McJets->SetBranchAddress("TriggerJet", &McTrigger);

  TClonesArray* McTriggerLo = new TClonesArray("TLorentzVector");
  McJets->GetBranch("TriggerJetLo")->SetAutoDelete(kFALSE);
  McJets->SetBranchAddress("TriggerJetLo", &McTriggerLo);

  // On the other side
  TClonesArray* McAwayJet = new TClonesArray("TLorentzVector");
  McJets->GetBranch("AwayJet")->SetAutoDelete(kFALSE);
  McJets->SetBranchAddress("AwayJet", &McAwayJet);

  TClonesArray* McAwayLo = new TClonesArray("TLorentzVector");
  McJets->GetBranch("AwayJetLo")->SetAutoDelete(kFALSE);
  McJets->SetBranchAddress("AwayJetLo", &McAwayLo);

  int mceventid;
  int mcrunid;
  double mcweight;     // Double-check, should be the same as below
  McJets->SetBranchAddress("eventid", &mceventid);
  McJets->SetBranchAddress("runid", &mcrunid);
  McJets->SetBranchAddress("weight",&mcweight );

  float mczgtrighi, mczgtriglo, mczgawayhi, mczgawaylo;
  McJets->SetBranchAddress("zgtrighi",&mczgtrighi );
  McJets->SetBranchAddress("zgtriglo",&mczgtriglo );
  McJets->SetBranchAddress("zgawayhi",&mczgawayhi );
  McJets->SetBranchAddress("zgawaylo",&mczgawaylo );

  
  // Set up pp events
  // ----------------
  TChain* PpJets = new TChain("TriggeredTree");
  PpJets->Add ( PpLevelFile );
  PpJets->BuildIndex("runid","eventid");

  // The leading jet above 10 GeV that caused
  // When appropriate, it has the HT!
  // this event to be recorded
  TClonesArray* PpTrigger = new TClonesArray("TLorentzVector");
  PpJets->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  PpJets->SetBranchAddress("TriggerJet", &PpTrigger);

  TClonesArray* PpTriggerLo = new TClonesArray("TLorentzVector");
  PpJets->GetBranch("TriggerJetLo")->SetAutoDelete(kFALSE);
  PpJets->SetBranchAddress("TriggerJetLo", &PpTriggerLo);

  // On the other side
  TClonesArray* PpAwayJet = new TClonesArray("TLorentzVector");
  PpJets->GetBranch("AwayJet")->SetAutoDelete(kFALSE);
  PpJets->SetBranchAddress("AwayJet", &PpAwayJet);

  TClonesArray* PpAwayLo = new TClonesArray("TLorentzVector");
  PpJets->GetBranch("AwayJetLo")->SetAutoDelete(kFALSE);
  PpJets->SetBranchAddress("AwayJetLo", &PpAwayLo);

  int ppeventid;
  int pprunid;
  double ppweight;     // Double-check, should be the same as below
  PpJets->SetBranchAddress("eventid", &ppeventid);
  PpJets->SetBranchAddress("runid", &pprunid);
  PpJets->SetBranchAddress("weight",&ppweight );

  float ppzgtrighi, ppzgtriglo, ppzgawayhi, ppzgawaylo;
  PpJets->SetBranchAddress("zgtrighi",&ppzgtrighi );
  PpJets->SetBranchAddress("zgtriglo",&ppzgtriglo );
  PpJets->SetBranchAddress("zgawayhi",&ppzgawayhi );
  PpJets->SetBranchAddress("zgawaylo",&ppzgawaylo );

  // Output and histograms
  // ----------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // int nPtBins = 140;
  // float ptmin=10;
  // float ptmax=80;

  int nPtBins = 60;
  float ptmin=10;
  float ptmax=70;

  // // Trigger jet info
  // Old
  TH1D* McTrigPt = new TH1D( "McTrigPt",";Trigger p_{T}^{Jet} [GeV/c]", nPtBins, ptmin, ptmax );
  TH1D* McTrigPtMatched = new TH1D( "McTrigPtMatched",";Trigger p_{T}^{Jet} [GeV/c]", nPtBins, ptmin, ptmax );
  TH1D* DetTrigPt = new TH1D( "DetTrigPt",";Trigger p_{T}^{Jet} [GeV/c]", nPtBins, ptmin, ptmax );
  TH2D* TriggerJetForLostMc = new TH2D( "TriggerJetForLostMc", ";p_{T};#eta", 120, 10, 70, 100,-1,1 );

  // New
  TH1D* McTriggerPt = new TH1D( "McTriggerPt",";Trigger p_{T}^{Part} [GeV/c]", nPtBins, ptmin, ptmax );
  TH1D* PpTriggerPt = new TH1D( "PpTriggerPt",";Trigger p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax );

  TH2D* McPpTriggerPt = new TH2D( "McPpTriggerPt",";p_{T}^{Part} [GeV/c];p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax, nPtBins, ptmin, ptmax );

  TH2D* DeltaTriggerPt = new TH2D( "DeltaTriggerPt",";p_{T}^{Part};Trigger p_{T}^{Part}-p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax, 100, -40, 60 );
  TH2D* RelDeltaTriggerPt = new TH2D( "RelDeltaTriggerPt",";p_{T}^{Part};Trigger (p_{T}^{Part}-p_{T}^{Det}) / p_{T}^{Part}", nPtBins, ptmin, ptmax, 100, -2, 2 );
  

  // Set up response matrix
  // ----------------------
  // int nZgBinsTrue = 20;
  // float zgminTrue = 0;
  // float zgmaxTrue = 0.5;
  // int nZgBinsMeas = 20;
  // float zgminMeas = 0;
  // float zgmaxMeas = 0.5;
  int nZgBinsTrue = 20;
  float zgminTrue = 0.05;
  float zgmaxTrue = 0.55;
  int nZgBinsMeas = 20;
  float zgminMeas = 0.05;
  float zgmaxMeas = 0.55;
  // int nPtBinsTrue =  60;
  // float ptminTrue =   0;
  // float ptmaxTrue =  60;
  // int nPtBinsMeas =  50;
  // float ptminMeas =  10;
  // float ptmaxMeas =  60;

  int nPtBinsTrue =  nPtBins;
  float ptminTrue =  ptmin;
  float ptmaxTrue =  ptmax;
  int nPtBinsMeas =  nPtBins;
  float ptminMeas =  ptmin;
  float ptmaxMeas =  ptmax;

  // RooUnfoldResponse LeadPtResponse ( nPtBinsMeas, ptminMeas, ptmaxMeas, nPtBinsTrue, ptminTrue, ptmaxTrue );
  RooUnfoldResponse TrigPtResponse ( nPtBinsMeas, ptminMeas, ptmaxMeas, nPtBinsTrue, ptminTrue, ptmaxTrue );
  RooUnfoldResponse RecoilPtResponse ( nPtBinsMeas, ptminMeas, ptmaxMeas, nPtBinsTrue, ptminTrue, ptmaxTrue );

  // 2D unfolding:
  TH2D* hTrue= new TH2D ("hTrue", "Truth", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* hMeas= new TH2D ("hMeas", "Measured", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  RooUnfoldResponse TrigPtZgResponse2D;
  TrigPtZgResponse2D.Setup (hMeas, hTrue );

  TH2D* hRecoilTrue= new TH2D ("hRecoilTrue", "Truth", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* hRecoilMeas= new TH2D ("hRecoilMeas", "Measured", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  RooUnfoldResponse RecoilPtZgResponse2D;
  RecoilPtZgResponse2D.Setup (hMeas, hTrue );

  TH2D* TrigTruth2D = new TH2D( "TrigTruth2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* TrigMeas2D  = new TH2D( "TrigMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6#{oplus}GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  TH2D* TrigTestTruth2D = new TH2D( "TrigTestTruth2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* TrigTestMeas2D  = new TH2D( "TrigTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6#{oplus}GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  TH2D* RecoilTruth2D = new TH2D( "RecoilTruth2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* RecoilMeas2D  = new TH2D( "RecoilMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6#{oplus}GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  TH2D* RecoilTestTruth2D = new TH2D( "RecoilTestTruth2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* RecoilTestMeas2D  = new TH2D( "RecoilTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6#{oplus}GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  TH2D* McPpTriggerZg2030 = new TH2D( "McPpTriggerZg2030",";z_{g}^{Part};z_{g}^{Det}", nZgBinsTrue, zgminTrue, zgmaxTrue, nZgBinsMeas, zgminMeas, zgmaxMeas );

  // ------------------------
  // Loop over particle level
  // ------------------------

  cout << "Loop over particle level" << endl;
  cout << "------------------------" << endl;

  for ( Long64_t mcEvi = 0; mcEvi< McJets->GetEntries() ; ++mcEvi ){
    if ( !(mcEvi%10000) ) cout << "Working on " << mcEvi << " / " << McJets->GetEntries() << endl;
    McJets->GetEntry(mcEvi);
    TLorentzVector* McT = (TLorentzVector*) McTrigger->At(0);

    // Require truth in acceptance
    // ---------------------------
    if ( fabs ( McT->Eta() ) > EtaCut ) continue;
    McTrigPt->Fill( McT->Pt(), mcweight );
    
    // Get corresponding pp event
    // --------------------------
    Long64_t ppevi=-1;
    ppevi = PpJets->GetEntryNumberWithIndex( mcrunid, mceventid );
    if ( ppevi < 0 ){
      // TriggerJetForLostMc->Fill( tdet->Pt(),tdet->Eta() );
      // lostpp++;
      // Here is where we for the first time could file for loss
      // Instead, just skip this event
      continue;
    }
    PpJets->GetEntry(ppevi);

    // Get the det level jets
    // ----------------------
    if ( PpTrigger->GetEntries()!=1 ){
      cout << " No trigger jet in data?!" << endl;
      return -1;
    } 
    
    TLorentzVector* PpT = (TLorentzVector*) PpTrigger->At(0);
    TLorentzVector* PpA = 0;
    if (PpAwayJet->GetEntries()!=0 ) PpA = (TLorentzVector*) PpAwayJet->At(0);

    // TRIGGER: Find best match to MC
    // ------------------------------
    TLorentzVector* MatchT = 0;
    if ( McT->DeltaR( *PpT ) < RCut ) MatchT = PpT;
    // else if ( PpA && McT->DeltaR( *PpA ) < RCut ) MatchT = PpA;
    
    if ( !MatchT ) {
      // lost trigger jet
      continue;
    }
    // Fill truth, smeared, and delta pT
    McTriggerPt->Fill( McT->Pt(), mcweight);
    PpTriggerPt->Fill( MatchT->Pt(), mcweight);
    if ( fabs( McT->Pt() - MatchT->Pt() ) > McT->Pt() ) {
      cerr << McT->DeltaR( *MatchT) << "  " << McT->Pt() << "  " << MatchT->Pt() << "  " << mcrunid << "  " << pprunid << endl ;
    }
    McPpTriggerPt->Fill( McT->Pt(), MatchT->Pt(), mcweight);

    DeltaTriggerPt->Fill( McT->Pt(), McT->Pt() - MatchT->Pt(), mcweight);
    RelDeltaTriggerPt->Fill( McT->Pt(), (McT->Pt() - MatchT->Pt()) / McT->Pt(), mcweight);
    
    //    if ( mcEvi %2 ){     // Use half for training, half for testing
    TrigPtResponse.Fill( MatchT->Pt(), McT->Pt(), mcweight );
    TrigPtZgResponse2D.Fill( MatchT->Pt(), ppzgtriglo, McT->Pt(), mczgtriglo, mcweight );
    TrigPtZgResponse2D.Fill( MatchT->Pt(), ppzgtriglo, McT->Pt(), mczgtriglo, mcweight );
    
    if ( MatchT->Pt() > 20 && MatchT->Pt() < 30 ){
      McPpTriggerZg2030->Fill( ppzgtriglo, mczgtriglo, mcweight);
    }
    
    TrigMeas2D->Fill( MatchT->Pt(), ppzgtriglo, mcweight );
    TrigTruth2D->Fill( McT->Pt(), mczgtriglo, mcweight );
    //    } else {
    TrigTestMeas2D->Fill( MatchT->Pt(), ppzgtriglo, mcweight );
    TrigTestTruth2D->Fill( McT->Pt(), mczgtriglo, mcweight );
    //    }    
    
    // RECOIL: Find best match to MC
    // -----------------------------
    TLorentzVector* McA = 0;
    if (McAwayJet->GetEntries()!=0 ) McA = (TLorentzVector*) McAwayJet->At(0);
    if ( !McA ) {
      // no recoil jet
      continue;
    }
    
    // Require truth in acceptance
    // ---------------------------
    if ( fabs ( McA->Eta() ) > EtaCut ) continue;
    // McRecoilPt->Fill( McA->Pt(), mcweight );

    TLorentzVector* MatchA = 0;
    if ( McA->DeltaR( *PpA ) < RCut ) MatchA = PpA;
    // else if ( McA->DeltaR( *PpT ) < RCut ) MatchA = PpT;
    
    if ( !MatchA ) {
      // lost recoil jet
      continue;
    }
    
    //    if ( mcEvi %2 ){     // Use half for training, half for testing
    RecoilPtResponse.Fill( MatchA->Pt(), McA->Pt(), mcweight );
    RecoilPtZgResponse2D.Fill( MatchA->Pt(), ppzgawaylo, McA->Pt(), mczgawaylo, mcweight );
    RecoilPtZgResponse2D.Fill( MatchA->Pt(), ppzgawaylo, McA->Pt(), mczgawaylo, mcweight );
    
    RecoilMeas2D->Fill( MatchA->Pt(), ppzgawaylo, mcweight );
    RecoilTruth2D->Fill( McA->Pt(), ppzgawaylo, mcweight );
    //    } else {
    RecoilTestMeas2D->Fill( MatchA->Pt(), ppzgawaylo, mcweight );
    RecoilTestTruth2D->Fill( McA->Pt(), ppzgawaylo, mcweight );
    //    }    

  }

  new TCanvas ( "c0","");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  McTriggerPt->Draw();
  PpTriggerPt->Draw("same");

  leg = new TLegend( 0.6, 0.7, 0.89, 0.9, "Leading Jet p_{T}" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  PpTriggerPt->SetLineColor(kBlue);
  leg->AddEntry(PpTriggerPt, "GEANT Result");

  McTriggerPt->SetLineColor(kOrange);
  leg->AddEntry(McTriggerPt, "Matched Training Truth");
  leg->Draw();

  gPad->SaveAs(PlotBase+"_TrigSpec.png");

  // new TCanvas ( "c1","",500, 500);
  new TCanvas ( "c1","");
  // gPad->SetCanvasSize(450,450);
  // gPad->DrawFrame();
  DeltaTriggerPt->Draw("colz");
  // capped byt ptdet = ptpart-10
  TF1* f = new TF1("f","x-10", ptmin, ptmax);
  f->SetLineStyle(7);
  f->SetLineWidth(1);
  f->Draw("same");
  gPad->SaveAs(PlotBase+"_DeltaPt.png");


  new TCanvas ( "c2","",500, 500);
  // gPad->SetCanvasSize(450,450);
  RelDeltaTriggerPt->Draw("colz");
  gPad->SaveAs(PlotBase+"_RelDeltaPt.png");

  new TCanvas ( "c3","",500, 500);
  // gPad->SetCanvasSize(450,450);
  DeltaTriggerPt->ProfileX("_pfx",1,-1,"s")->Draw();
  gPad->SaveAs(PlotBase+"_DeltaProfile.png");

  new TCanvas ( "c4","",500, 500);
  gPad->SetCanvasSize(450,450);
  McPpTriggerPt->Draw("colz");
  TLine l;
  l.SetLineStyle(7);
  l.DrawLine( ptmin, ptmin, ptmax, ptmax );
  gPad->SaveAs(PlotBase+"_ResponseHist.png");

  new TCanvas ( "c5","",500, 500);
  gPad->SetCanvasSize(450,450);
  McPpTriggerZg2030->Draw("colz");
  l.SetLineStyle(7);
  l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
  gPad->SaveAs(PlotBase+"_ZgResponseHist.png");

  
  
    
  // Done
  // ----
  fout->Write();
  TrigPtResponse.SetName("TrigPtResponse");
  TrigPtZgResponse2D.SetName("TrigPtZgResponse2D");
  TrigPtResponse.Write();
  TrigPtZgResponse2D.Write();

  RecoilPtResponse.SetName("RecoilPtResponse");
  RecoilPtZgResponse2D.SetName("RecoilPtZgResponse2D");
  RecoilPtResponse.Write();
  RecoilPtZgResponse2D.Write();

  cout << " Wrote to" << endl << OutFileName << endl;
  return 0;

}
