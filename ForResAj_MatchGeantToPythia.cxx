#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TProfile.h>
#include <TLine.h>

#include <TROOT.h>
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

// Load helper macro
#include "GeantWeightReject.hh"

int ForResAj_MatchGeantToPythia (
				 // TString PpLevelFile = "AjResults/R0.2_ForResAj_NoEff_HT54_Geant.root"
				 // TString PpLevelFile = "AjResults/ForResAj_NoEff_HT54_Geant.root"
				 // TString PpLevelFile = "AjResults/ForResAj_HT54_Geant.root"
				 TString PpLevelFile = "AjResults/R0.2_ForResAj_HT54_Geant.root"
				 // TString PpLevelFile = "AjResults/ForResAj_Geant.root"
				 // TString PpLevelFile = "AjResults/ForResAj_NoEff_Geant.root"
				 ) {
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);
  TLegend* leg = 0;

  // float R = 0.4;
  float RCut = 0.4;
				   
  // The embedding data has some strange quirks,
  // specifically things like 10 GeV "true" jets reconstructed at 50 GeV
  // My suspicion is that some pythia particles were not properly recorded.
  // This is supported by the fact that these low pT leading jets come
  // from quite high pHat bins.
  // Example:
  // pHat bin 35-45
  // Trigger match with dR=0.334396 pT_Part= 2.88517 pT_Det= 28.9917
  // Visualization:
  // TFile *_file0 = TFile::Open("Data/AddedGeantPythia/picoDst_35_45_4.root");
  // new TCanvas;
  // JetTree->Draw("fPrimaryTracks.GetEta():fPrimaryTracks.GetPhi()","fPrimaryTracks.GetPt()*(Entry$==5570  && abs(fPrimaryTracks.GetEta())<1.2 && fPrimaryTracks.GetPt()<30  && fPrimaryTracks.GetPt()>0.2  )","colz");
  // new TCanvas;
  // JetTree->Draw("fTowers.GetEta():fTowers.GetPhi()","fTowers.GetEnergy()/TMath::CosH(fTowers.GetEta())*( Entry$==5570  )","colz");
  // new TCanvas;
  // JetTreeMc->Draw("fPrimaryTracks.GetEta():fPrimaryTracks.GetPhi()","fPrimaryTracks.GetPt()*(Entry$==5570  && abs(fPrimaryTracks.GetEta())<1.2  && fPrimaryTracks.GetPt()>0.2  )","colz");  
  // Here's an option to reject them based on a simple cut.    
  bool RejectOutliers=false;
  bool RejectHiweights=false;
  
  float EtaCut = 1.0-RCut;


  // Input
  // -----
 
  // TString McLevelFile  = "AjResults/ForResAj_NoEff_NoCut_GeantMc.root"; // Reference (particle level) jets
  // TString McLevelFile  = "AjResults/ForResAj_NoEff_TrueMB_NoCut_GeantMc.root"; // Reference (particle level) jets
  TString McLevelFile  = "AjResults/R0.2_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root"; // Reference (particle level) jets

  // Output
  // ------
  TString OutFileName = "AjResults/";
  OutFileName += gSystem->BaseName(PpLevelFile);
  OutFileName.ReplaceAll(".root","_");
  if ( RejectOutliers ) OutFileName += "NoOutliers_";
  OutFileName += "MatchedTo_";
  OutFileName += gSystem->BaseName(McLevelFile);
  
  TString PlotBase = "plots/";
  PlotBase += gSystem->BaseName(PpLevelFile);
  PlotBase.ReplaceAll(".root","_");
  if ( RejectOutliers ) PlotBase += "NoOutliers_";
  PlotBase += "MatchedTo_";
  PlotBase += gSystem->BaseName(McLevelFile);
  PlotBase.ReplaceAll(".root","");
  
  // Refmult Cut
  // -----------
  int RefMultCut=0;

  // Load MC level tree
  // ------------------
  // Note that we only save two jets, so we may miss some
  // Currently looks like missing 222 out of 70k triggers (0.3%)
  // and 1608 total when also requiring a recoil match (2.3%)
  // and 1636 total when rejecting high weights
  
  // TChain* McJets = new TChain("ResultTree");
  TChain* McJets = new TChain("TriggeredTree");
  McJets->Add ( McLevelFile );
  McJets->BuildIndex("runid","eventid");

  // TLorentzVector* McTrigger = new TLorentzVector();
  // McJets->SetBranchAddress("j1", &McTrigger);

  // TLorentzVector* McTriggerLo = new TLorentzVector();
  // McJets->SetBranchAddress("jm1", &McTriggerLo);

  // TLorentzVector* McAwayJet = new TLorentzVector();
  // McJets->SetBranchAddress("j2", &McAwayJet);

  // TLorentzVector* McAwayLo = new TLorentzVector();
  // McJets->SetBranchAddress("jm2", &McAwayLo);

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

  // Set up pp events
  // ----------------
  TChain* PpJets = new TChain("ResultTree");
  PpJets->Add ( PpLevelFile );
  PpJets->BuildIndex("runid","eventid");

  TLorentzVector* PpTrigger = new TLorentzVector();
  PpJets->SetBranchAddress("j1", &PpTrigger);

  TLorentzVector* PpTriggerLo = new TLorentzVector();
  PpJets->SetBranchAddress("jm1", &PpTriggerLo);

  TLorentzVector* PpAwayJet = new TLorentzVector();
  PpJets->SetBranchAddress("j2", &PpAwayJet);

  TLorentzVector* PpAwayLo = new TLorentzVector();
  PpJets->SetBranchAddress("jm2", &PpAwayLo);

  int ppeventid;
  int pprunid;
  double ppweight;     // Double-check, should be the same as below
  PpJets->SetBranchAddress("eventid", &ppeventid);
  PpJets->SetBranchAddress("runid", &pprunid);
  PpJets->SetBranchAddress("weight",&ppweight );

  // Output and histograms
  // ----------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  int nPtBins = 80;
  float ptmin=0;
  float ptmax=80;

  TH1D* McTriggerPt = new TH1D( "McTriggerPt",";Trigger p_{T}^{Part} [GeV/c]", nPtBins, ptmin, ptmax );
  TH1D* PpTriggerPt = new TH1D( "PpTriggerPt",";Trigger p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax );
  TH1D* PpTriggerPtLo = new TH1D( "PpTriggerPtLo",";Trigger p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax );

  TH2D* PpMcTriggerPt = new TH2D( "PpMcTriggerPt",";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c]", nPtBins, ptmin, ptmax, nPtBins, ptmin, ptmax );
  TH2D* PpMcTriggerPtLo = new TH2D( "PpMcTriggerPtLo",";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c]", nPtBins, ptmin, ptmax, nPtBins, ptmin, ptmax );

  TH2D* DeltaTriggerPt      = new TH2D( "DeltaTriggerPt",";p_{T}^{hardcore};Trigger p_{T}^{Part}-p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax, 120, -60, 60 );
  TH2D* RelDeltaTriggerPt   = new TH2D( "RelDeltaTriggerPt",";p_{T}^{hardcore};Trigger (p_{T}^{Part}-p_{T}^{Det}) / p_{T}^{Part}", nPtBins, ptmin, ptmax, 300, -3, 3 );
  TH2D* DeltaTriggerPtLo    = new TH2D( "DeltaTriggerPtLo",";p_{T}^{matched};Trigger p_{T}^{Part}-p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax, 120, -60, 60 );
  TH2D* RelDeltaTriggerPtLo = new TH2D( "RelDeltaTriggerPtLo",";p_{T}^{matched};Trigger (p_{T}^{Part}-p_{T}^{Det}) / p_{T}^{Part}", nPtBins, ptmin, ptmax, 300, -3, 3 );

  TH1D* McRecoilPt = new TH1D( "McRecoilPt",";Recoil p_{T}^{Part} [GeV/c]", nPtBins, ptmin, ptmax );
  TH1D* PpRecoilPt = new TH1D( "PpRecoilPt",";Recoil p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax );
  TH1D* PpRecoilPtLo = new TH1D( "PpRecoilPtLo",";Recoil p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax );
  
  TH2D* PpMcRecoilPt   = new TH2D( "PpMcRecoilPt",";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c]", nPtBins, ptmin, ptmax, nPtBins, ptmin, ptmax );
  TH2D* PpMcRecoilPtLo = new TH2D( "PpMcRecoilPtLo",";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c]", nPtBins, ptmin, ptmax, nPtBins, ptmin, ptmax );

  TH2D* DeltaRecoilPt      = new TH2D( "DeltaRecoilPt",";p_{T}^{hardcore};Recoil p_{T}^{Part}-p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax, 120, -60, 60 );
  TH2D* RelDeltaRecoilPt   = new TH2D( "RelDeltaRecoilPt",";p_{T}^{hardcore};Recoil (p_{T}^{Part}-p_{T}^{Det}) / p_{T}^{Part}", nPtBins, ptmin, ptmax, 300, -3, 3 );
  TH2D* DeltaRecoilPtLo    = new TH2D( "DeltaRecoilPtLo",";p_{T}^{matched};Recoil p_{T}^{Part}-p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax, 120, -60, 60 );
  TH2D* RelDeltaRecoilPtLo = new TH2D( "RelDeltaRecoilPtLo",";p_{T}^{matched};Recoil (p_{T}^{Part}-p_{T}^{Det}) / p_{T}^{Part}", nPtBins, ptmin, ptmax, 300, -3, 3 );

  // Set up response matrix
  // ----------------------
  // int nPtBinsTrue =  60;
  // float ptminTrue =   0;
  // float ptmaxTrue =  60;
  // int nPtBinsMeas =  50;
  // float ptminMeas =  10;
  // float ptmaxMeas =  60;

  // int nPtBinsTrue =  nPtBins;
  // float ptminTrue =  ptmin;
  // float ptmaxTrue =  ptmax;

  int nPtBinsTrue =  nPtBins;
  float ptminTrue =  ptmin;
  float ptmaxTrue =  ptmax;

  // int nPtBinsMeas =  140;
  // float ptminMeas =  10;
  // float ptmaxMeas =  80;
  int nPtBinsMeas =  60;
  float ptminMeas =  10;
  float ptmaxMeas =  70;

  // ------------------------
  // Loop over detector level
  // ------------------------

  cout << "Loop over detector level" << endl;
  cout << "------------------------" << endl;
  
  int lost=0;
  for ( Long64_t ppEvi = 0; ppEvi< PpJets->GetEntries() ; ++ppEvi ){
    if ( !(ppEvi%10000) ) cout << "Working on " << ppEvi << " / " << PpJets->GetEntries() << endl;
    PpJets->GetEntry(ppEvi);
    TLorentzVector* PpT = PpTrigger;
    TLorentzVector* PpA = PpAwayJet;
    if (PpT->Pt() <20 || PpA->Pt() <10 ) {
      cerr << PpT->Pt() << "  " <<  PpA->Pt() << endl;
      return -1;
    }
            
    TLorentzVector* PpTlo = PpTriggerLo;
    TLorentzVector* PpAlo = PpAwayLo;
    if (PpTlo->Pt() <1 || PpAlo->Pt() <1 ) {
      cerr << PpTlo->Pt() << "  " <<  PpAlo->Pt() << endl;
      return -1;
    }
        
    // Get corresponding mc event
    // --------------------------
    Long64_t mcevi=-1;
    mcevi = McJets->GetEntryNumberWithIndex( pprunid, ppeventid );
    if ( mcevi < 0 ){
      cerr << "Couldn't find this event in MC!" << endl;
      cerr << pprunid << "  " <<  ppeventid << endl;
      // Skip this event
      cout << ++lost << endl;
      continue;
    }

    McJets->GetEntry(mcevi);
    TLorentzVector* McT = (TLorentzVector*) McTriggerLo->At(0);
    TLorentzVector* McA = 0;
    //if (McAwayJet->GetEntries()!=0 ) McA = (TLorentzVector*) McAwayJet->At(0);
    if (McAwayJet->GetEntries()!=0 ) McA = (TLorentzVector*) McAwayLo->At(0);
   
    
    // Find best match to MC
    // ---------------------
    TLorentzVector* MatchT = 0;
    if ( McT->DeltaR( *PpT ) < RCut ) MatchT = McT;
    else if ( McA && McA->DeltaR( *PpT ) < RCut ) MatchT = McA;

    TLorentzVector* MatchA = 0;
    if ( !McA ) {
      // lost recoil jet
      cout << ++lost << endl;
      continue;
    }

    if ( McA->DeltaR( *PpA ) < RCut ) MatchA = McA;
    else if ( McT->DeltaR( *PpA ) < RCut ) MatchA = McT;
    
    if ( !MatchA ) {
      // lost recoil jet
      cout << ++lost << endl;
      continue;
    }


    if ( !MatchT ) {
      // lost trigger jet
      cout << ++lost << endl;
      // cout << "Lost the trigger jet" << endl;
      // cout << "PP: Pt=" << PpT->Pt() << "  Eta=" << PpT->Eta() << "  Phi=" << PpT->Phi() << endl;
      // cout << "MC trig: Pt=" << McT->Pt() << "  Eta=" << McT->Eta() << "  Phi=" << McT->Phi() << endl;
      // if ( McA && McA->Pt()>0)
      // 	cout << "MC away: Pt=" << McA->Pt() << "  Eta=" << McA->Eta() << "  Phi=" << McA->Phi() << endl;
      // cout << endl;
      continue;
    }
    
    
    // Done, we have matched pairs
    // ---------------------------

    
    // Skip high weight outliers
    // -------------------------
    if ( RejectHiweights ) {
      if ( GeantWeightReject ( RCut, PpT->Pt(), ppweight, 10 ) )  {
	cout << "Skipping RECO TRIGGER with pt=" << PpT->Pt() << " due to high weight" << endl;
	cout << ++lost << endl;
	continue;
      }
      
      if ( GeantWeightReject ( RCut, PpA->Pt(), ppweight, 11 ) )  {
	cout << "Skipping RECO RECOIL with pt=" << PpA->Pt() << " due to high weight" << endl;
	cout << ++lost << endl;
	continue;
      }

      if ( GeantWeightReject ( RCut, MatchT->Pt(), mcweight, 0 ) )  {
	cout << "Skipping TRIGGER with pt=" << MatchT->Pt() << " due to high weight" << endl;
	cout << ++lost << endl;
	continue;
      }
      if ( GeantWeightReject ( RCut, MatchA->Pt(), mcweight, 1 ) )  {
	cout << "Skipping RECOIL with pt=" << MatchA->Pt() << " due to high weight" << endl;
	cout << ++lost << endl;
	continue;
      }
    }

    // Skip other outliers
    // -------------------
    if ( RejectOutliers ){
      if ( (PpT->Pt() - MatchT->Pt()) / MatchT->Pt() < -0.4 ) {
    	cout << ++lost << endl;
    	continue;
      }
    }
    

    // Fill histos
    // -----------
    PpTriggerPt->Fill( PpT->Pt(), ppweight );
    PpMcTriggerPt->Fill( PpT->Pt(), MatchT->Pt(), ppweight );
    PpRecoilPt->Fill( PpA->Pt(), ppweight );
    PpMcRecoilPt->Fill( PpA->Pt(), MatchA->Pt(), ppweight );
    
    McTriggerPt->Fill( MatchT->Pt(), mcweight );
    McRecoilPt->Fill( MatchA->Pt(), mcweight );
    
    DeltaTriggerPt->Fill( PpT->Pt(), MatchT->Pt() - PpT->Pt(), mcweight);
    RelDeltaTriggerPt->Fill( PpT->Pt(), (MatchT->Pt() - PpT->Pt()) / MatchT->Pt(), mcweight);

    DeltaRecoilPt->Fill( PpA->Pt(), MatchA->Pt() - PpA->Pt(), mcweight);
    RelDeltaRecoilPt->Fill( PpA->Pt(), (MatchA->Pt() - PpA->Pt()) / MatchA->Pt(), mcweight);
    
    PpTriggerPtLo->Fill( PpTlo->Pt(), ppweight );
    PpMcTriggerPtLo->Fill( PpTlo->Pt(), MatchT->Pt(), ppweight );

    PpRecoilPtLo->Fill( PpAlo->Pt(), ppweight );
    PpMcRecoilPtLo->Fill( PpAlo->Pt(), MatchA->Pt(), ppweight );
    
    DeltaTriggerPtLo->Fill( PpTlo->Pt(), MatchT->Pt() - PpTlo->Pt(), mcweight);
    RelDeltaTriggerPtLo->Fill( PpTlo->Pt(), (MatchT->Pt() - PpTlo->Pt()) / MatchT->Pt(), mcweight);

    DeltaRecoilPtLo->Fill( PpAlo->Pt(), MatchA->Pt() - PpAlo->Pt(), mcweight);
    RelDeltaRecoilPtLo->Fill( PpAlo->Pt(), (MatchA->Pt() - PpAlo->Pt()) / MatchA->Pt(), mcweight);

  } 
    
  // Done
  // ----
  fout->Write();
  cout << " Wrote to" << endl << OutFileName << endl;

  return 0;

}
