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

int MatchGeantToPythia (
			String PpLevelFile = "AjResults/AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root"
			// TString PpLevelFile = "AjResults/Above25_Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root"
			// TString PpLevelFile = "AjResults/CrossCheckForResAj_NoEff_NoCut_Geant.root" // CROSSCHECK ONLY
			
			) {

  cerr << "FIX THE Eta Cut!!" << endl;  return -1;
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);
  TLegend* leg = 0;

  bool PrepClosure=false;
    
  float MinJetPt = 5.0;
  bool UseMiss=true;
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
  bool RejectOutliers=true;
  bool RejectHiweights= true;
  
  float RCut = 0.4;
  float EtaCut = 1.0-RCut;


  // Input
  // -----
 
  TString McLevelFile  = "AjResults/Groom_Aj_TrueMB_NoCut_NoEff_GeantMc.root"; // Reference (particle level) jets
  // CROSSCHECK ONLY
  // TString McLevelFile  = "AjResults/ForResAj_NoEff_NoCut_GeantMc.root"; // Reference (particle level) jets

  // TString McLevelFile  = "AjResults/Above25_Groom_Aj_TrueMB_NoEff_GeantMc.root"; // Reference (particle level) jets
  // TString McLevelFile  = "AjResults/Above9_Groom_Aj_TrueMB_NoCut_NoEff_GeantMc.root"; // Reference (particle level) jets
  // TString McLevelFile  = "AjResults/Groom_Aj_TrueMB_NoCut_NoEff_GeantMc_9_11.root"; // Reference (particle level) jets
  // TString McLevelFile  = "AjResults/Groom_Aj_TrueMB_NoCut_NoEff_GeantMc_7_9.root"; // Reference (particle level) jets

  
  // TString PpLevelFile = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root"; // pp-like events
  // TString McLevelFile  = "HThistos/Groom_Aj_TrueMB_NoEff_GeantMc.root"; // Reference (particle level) jets
  // TString McLevelFile  = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_GeantMc.root"; // Reference (particle level) jets

  // Output
  // ------
  // TString OutFileName = "AjResults/NewTrainedWith_";
  TString OutFileName = "AjResults/";
  if ( PrepClosure )    OutFileName += "ForClosure_";
  OutFileName += gSystem->BaseName(PpLevelFile);
  OutFileName.ReplaceAll(".root","_");
  if ( UseMiss ) OutFileName += "WithMisses_";
  else OutFileName += "NoMisses_";
  if ( RejectOutliers ) OutFileName += "NoOutliers_";
  OutFileName += "TrainedWith_";
  OutFileName += gSystem->BaseName(McLevelFile);
  
  TString PlotBase = "plots/";
  if ( PrepClosure ) PlotBase += "ForClosure_";
  PlotBase += gSystem->BaseName(PpLevelFile);
  PlotBase.ReplaceAll(".root","_");
  if ( UseMiss ) PlotBase += "WithMisses_";
  else PlotBase += "NoMisses_";
  if ( RejectOutliers ) PlotBase += "NoOutliers_";
  PlotBase += "Trained_With";
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

  // The leading jet that triggered this event
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

  int nPtBins = 80;
  float ptmin=0;
  float ptmax=80;

  // // Trigger jet info
  // Old
  // TH1D* McTrigPtMatched = new TH1D( "McTrigPtMatched",";Trigger p_{T}^{Jet} [GeV/c]", nPtBins, ptmin, ptmax );
  // TH1D* DetTrigPt = new TH1D( "DetTrigPt",";Trigger p_{T}^{Jet} [GeV/c]", nPtBins, ptmin, ptmax );
  // TH2D* TriggerJetForLostMc = new TH2D( "TriggerJetForLostMc", ";p_{T};#eta", 120, 10, 70, 100,-1,1 );

  // New
  TH1D* McTriggerPt = new TH1D( "McTriggerPt",";Trigger p_{T}^{Part} [GeV/c]", nPtBins, ptmin, ptmax );
  TH1D* PpTriggerPt = new TH1D( "PpTriggerPt",";Trigger p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax );

  TH2D* McPpTriggerPt = new TH2D( "McPpTriggerPt",";p_{T}^{Part} [GeV/c];p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax, nPtBins, ptmin, ptmax );

  TH2D* DeltaTriggerPt = new TH2D( "DeltaTriggerPt",";p_{T}^{Part};Trigger p_{T}^{Part}-p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax, 100, -40, 60 );
  TH2D* RelDeltaTriggerPt = new TH2D( "RelDeltaTriggerPt",";p_{T}^{Part};Trigger (p_{T}^{Part}-p_{T}^{Det}) / p_{T}^{Part}", nPtBins, ptmin, ptmax, 100, -2, 2 );

  // Debug
  TH1D* hhh = new TH1D( "hhh","",10, 0.05,0.55 );


  // Set up response matrix
  // ----------------------
  // int nZgBinsTrue = 20;
  // float zgminTrue = 0;
  // float zgmaxTrue = 0.5;
  // int nZgBinsMeas = 20;
  // float zgminMeas = 0;
  // float zgmaxMeas = 0.5;
  // int nZgBinsTrue = 40;
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
  TH2D* TrigMeas2D  = new TH2D( "TrigMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  TH2D* TrigTestTruth2D = new TH2D( "TrigTestTruth2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* TrigTestMeas2D  = new TH2D( "TrigTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  TH2D* RecoilTruth2D = new TH2D( "RecoilTruth2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* RecoilMeas2D  = new TH2D( "RecoilMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  TH2D* RecoilTestTruth2D = new TH2D( "RecoilTestTruth2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* RecoilTestMeas2D  = new TH2D( "RecoilTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  TH2D* McPpTriggerZg2030 = new TH2D( "McPpTriggerZg2030",";z_{g}^{Part};z_{g}^{Det}", nZgBinsTrue, zgminTrue, zgmaxTrue, nZgBinsMeas, zgminMeas, zgmaxMeas );
  TH2D* McPpTriggerZg10plus = new TH2D( "McPpTriggerZg10plus",";z_{g}^{Part};z_{g}^{Det}", nZgBinsTrue, zgminTrue, zgmaxTrue, nZgBinsMeas, zgminMeas, zgmaxMeas );
  TH3D* McPpTriggerZg3d = new TH3D( "McPpTriggerZg3d",";z_{g}^{Part};z_{g}^{Det};p_{T}^{truth}", nZgBinsTrue, zgminTrue, zgmaxTrue, nZgBinsMeas, zgminMeas, zgmaxMeas, 12, 0, 60 );

  // ------------------------
  // Loop over particle level
  // ------------------------

  cout << "Loop over particle level" << endl;
  cout << "------------------------" << endl;

  for ( Long64_t mcEvi = 0; mcEvi< McJets->GetEntries() ; ++mcEvi ){
    if ( !(mcEvi%10000) ) cout << "Working on " << mcEvi << " / " << McJets->GetEntries() << endl;
    McJets->GetEntry(mcEvi);
    // TLorentzVector* McT = (TLorentzVector*) McTrigger->At(0);
    TLorentzVector* McT = (TLorentzVector*) McTriggerLo->At(0);
    TLorentzVector* McA = 0;
    //if (McAwayJet->GetEntries()!=0 ) McA = (TLorentzVector*) McAwayJet->At(0);
    if (McAwayJet->GetEntries()!=0 ) McA = (TLorentzVector*) McAwayLo->At(0);
    
    // Require truth in acceptance
    // ---------------------------
    if ( fabs ( McT->Eta() ) > EtaCut ) continue;
    bool HasAway= ( McA!=0 )   &&  ( fabs ( McA->Eta() ) < EtaCut );
        
    // Skip low pT events
    // ------------------
    if ( McT->Pt() < MinJetPt ) continue;
    // if ( HasAway && McA->Pt() < MinJetPt ) continue; // TEST


    // Skip high weight outliers
    // -------------------------
    if ( RejectHiweights && GeantWeightReject ( 0.4, McT->Pt(), mcweight, 0 ) )  {
      cout << "Skipping TRIGGER with pt=" << McT->Pt() << " due to high weight" << endl;
      continue;
    }
    if ( HasAway && RejectHiweights && GeantWeightReject ( 0.4, McA->Pt(), mcweight, 1 ) )  {
      cout << "Skipping RECOIL with pt=" << McA->Pt() << " due to high weight" << endl;
      continue;
    }
    
    
    
    // Get corresponding pp event
    // --------------------------
    Long64_t ppevi=-1;
    ppevi = PpJets->GetEntryNumberWithIndex( mcrunid, mceventid );

    // bool missed = false;
    if ( ppevi < 0 ){      
      // Here is where we for the first time could file for loss
      if ( UseMiss ){
	if ( !PrepClosure || mcEvi%2 == 0){
	  TrigPtResponse.Miss( McT->Pt(), mcweight );
	  TrigPtZgResponse2D.Miss( McT->Pt(), mczgtriglo, mcweight );
	}
	if ( HasAway ) {
	  if ( !PrepClosure || mcEvi%2 == 0) {
	    RecoilPtResponse.Miss( McA->Pt(), mcweight );
	    RecoilPtZgResponse2D.Miss( McA->Pt(), mczgawaylo, mcweight );
	  }
	}
      }
      
      // Skip this event
      continue;
      //missed = true;
    }
    PpJets->GetEntry(ppevi);

    // Record Truth
    // ------------
    if ( !PrepClosure || mcEvi%2 == 0)    TrigTruth2D->Fill( McT->Pt(), mczgtriglo, mcweight );
    if ( !PrepClosure || mcEvi%2 == 1)    TrigTestTruth2D->Fill( McT->Pt(), mczgtriglo, mcweight );
    if ( HasAway ){
      if ( !PrepClosure || mcEvi%2 == 0)    RecoilTruth2D->Fill( McA->Pt(), ppzgawaylo, mcweight );
      if ( !PrepClosure || mcEvi%2 == 1)    RecoilTestTruth2D->Fill( McA->Pt(), ppzgawaylo, mcweight );
    }

    // Get the det level jets
    // ----------------------
    if ( PpTrigger->GetEntries()!=1 ){
      cout << " No trigger jet in data?!" << endl;
      return -1;
    } 
    
    TLorentzVector* PpT = (TLorentzVector*) PpTrigger->At(0);
    TLorentzVector* PpA = 0;
    if (PpAwayJet->GetEntries()!=0 ) PpA = (TLorentzVector*) PpAwayJet->At(0);

    if ( PpT->Pt()<15 && PpT->Pt()>10 && ( !RejectHiweights || !GeantWeightReject ( 0.4, PpT->Pt(), mcweight, 10 ) ) )
	 hhh->Fill( ppzgtriglo, mcweight );

    // TRIGGER: Find best match to MC
    // ------------------------------
    TLorentzVector* MatchT = 0;
    if ( McT->DeltaR( *PpT ) < RCut ) MatchT = PpT;
    // else if ( PpA && McT->DeltaR( *PpA ) < RCut ) MatchT = PpA;

    if ( !MatchT ) {
      // lost trigger jet
      if ( UseMiss ){
	if ( !PrepClosure || mcEvi%2 == 0){
	  TrigPtResponse.Miss( McT->Pt(), mcweight );
	  TrigPtZgResponse2D.Miss( McT->Pt(), mczgtriglo, mcweight );
	}
	if ( HasAway ) {
	  if ( !PrepClosure || mcEvi%2 == 0) {
	    RecoilPtResponse.Miss( McA->Pt(), mcweight );
	    RecoilPtZgResponse2D.Miss( McA->Pt(), mczgawaylo, mcweight );
	  }
	}
      }
      continue;
    }

    if ( RejectHiweights && GeantWeightReject ( 0.4, MatchT->Pt(), mcweight, 10 ) )  {
      cout << "Skipping RECO TRIGGER with pt=" << McT->Pt() << " due to high weight" << endl;
      continue;
    }

    // Skip other outliers
    // -------------------
    if ( RejectOutliers ){
      if ( (McT->Pt() - MatchT->Pt()) / McT->Pt() < -0.4 )
	continue;
    }


    // Fill truth, smeared, and delta pT
    McTriggerPt->Fill( McT->Pt(), mcweight);
    PpTriggerPt->Fill( MatchT->Pt(), mcweight);
    if ( fabs( McT->Pt() - MatchT->Pt() ) > McT->Pt() ) {
      cerr << " ------------------------------------------------------------" << endl;
      cerr << "Trigger match with dR=" << McT->DeltaR( *MatchT)
	   << " pT_Part= " << McT->Pt() << " pT_Det= " << MatchT->Pt() << endl;
      cerr << "mcrunid=" << mcrunid << "  " << "mceventid=" << mceventid << endl ;
      cerr << "pprunid=" << pprunid << "  " << "ppeventid=" << ppeventid << endl ;
      cerr << "mcweight=" << mcweight << "   ppweight=" << ppweight << endl ;
      cerr << " ------------------------------------------------------------" << endl;
    }
    McPpTriggerPt->Fill( McT->Pt(), MatchT->Pt(), mcweight);
 
    DeltaTriggerPt->Fill( McT->Pt(), McT->Pt() - MatchT->Pt(), mcweight);
    RelDeltaTriggerPt->Fill( McT->Pt(), (McT->Pt() - MatchT->Pt()) / McT->Pt(), mcweight);
    
    if ( !PrepClosure || mcEvi%2 == 0) {
      TrigPtResponse.Fill( MatchT->Pt(), McT->Pt(), mcweight );
      TrigPtZgResponse2D.Fill( MatchT->Pt(), ppzgtriglo, McT->Pt(), mczgtriglo, mcweight );
      McPpTriggerZg3d->Fill( ppzgtriglo, mczgtriglo, McT->Pt(), mcweight);
      if ( MatchT->Pt() > 10 ) McPpTriggerZg10plus->Fill( ppzgtriglo, mczgtriglo, mcweight);
      
      if ( MatchT->Pt() > 20 && MatchT->Pt() < 30 ){
	McPpTriggerZg2030->Fill( ppzgtriglo, mczgtriglo, mcweight);
      }
    }
    
    if ( !PrepClosure || mcEvi%2 == 0)    TrigMeas2D->Fill( MatchT->Pt(), ppzgtriglo, mcweight );
    if ( !PrepClosure || mcEvi%2 == 1)    TrigTestMeas2D->Fill( MatchT->Pt(), ppzgtriglo, mcweight );
    
    // RECOIL: Find best match to MC
    // -----------------------------
    if ( !HasAway ) {
      // no recoil jet
      continue;
    }
    
    TLorentzVector* MatchA = 0;
    if ( McA->DeltaR( *PpA ) < RCut ) MatchA = PpA;
    // else if ( McA->DeltaR( *PpT ) < RCut ) MatchA = PpT;
    
    if ( !MatchA ) {
      // lost recoil jet
      if ( UseMiss ){
	if ( !PrepClosure || mcEvi%2 == 0){
	  RecoilPtResponse.Miss( McA->Pt(), mcweight );
	  RecoilPtZgResponse2D.Miss( McA->Pt(), mczgawaylo, mcweight );
	}
      }
      continue;
    }
    
    if ( MatchA && RejectHiweights && GeantWeightReject ( 0.4, MatchA->Pt(), mcweight, 11 ) )  {
      cout << "Skipping RECO RECOIL with pt=" << McA->Pt() << " due to high weight" << endl;
      continue;
    }

    // if ( fabs( McA->Pt() - MatchA->Pt() ) > McA->Pt() ) {
    //   cerr << "Recoil match with dR=" << McA->DeltaR( *MatchA)
    // 	   << " pT_Part= " << McA->Pt() << " pT_Det= " << MatchA->Pt() << "  " << mcrunid << "  " << pprunid << endl ;
    // }

    if ( !PrepClosure || mcEvi%2 == 0){
      RecoilPtResponse.Fill( MatchA->Pt(), McA->Pt(), mcweight );
      RecoilPtZgResponse2D.Fill( MatchA->Pt(), ppzgawaylo, McA->Pt(), mczgawaylo, mcweight );
      RecoilPtZgResponse2D.Fill( MatchA->Pt(), ppzgawaylo, McA->Pt(), mczgawaylo, mcweight );
    }

    if ( !PrepClosure || mcEvi%2 == 0)   RecoilMeas2D->Fill( MatchA->Pt(), ppzgawaylo, mcweight );
    if ( !PrepClosure || mcEvi%2 == 1)   RecoilTestMeas2D->Fill( MatchA->Pt(), ppzgawaylo, mcweight );

  }

  new TCanvas ( "c0","");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  // McTriggerPt->Draw();
  // PpTriggerPt->Draw("same");
  TH1D* TrigTruthPt = (TH1D*) TrigTruth2D->ProjectionX("TrigTruthPt");
  TH1D* TrigMeasPt = (TH1D*) TrigMeas2D->ProjectionX("TrigMeasPt");
  TH1D* TrigTestTruthPt = (TH1D*) TrigTestTruth2D->ProjectionX("TrigTestTruthPt");
  TH1D* TrigTestMeasPt = (TH1D*) TrigTestMeas2D->ProjectionX("TrigTestMeasPt");

  leg = new TLegend( 0.6, 0.6, 0.89, 0.9, "Leading Jet p_{T}" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  TrigTruthPt->Draw("9same");
  TrigMeasPt->Draw("9same");
  
  TrigMeasPt->SetLineColor(kBlue);
  leg->AddEntry(TrigMeasPt, "Training GEANT Result");
  TrigTruthPt->SetLineColor(kOrange);
  leg->AddEntry(TrigTruthPt, "Training Matched Truth");

  TrigTestMeasPt->SetLineColor(kMagenta);
  TrigTestMeasPt->Draw("9same");
  leg->AddEntry(TrigTestMeasPt, "TESTING GEANT Result");
  TrigTestTruthPt->SetLineColor(kRed);
  TrigTestTruthPt->Draw("9same");
  leg->AddEntry(TrigTestTruthPt, "TESTING Matched Truth");
  leg->Draw("same");
  
  // PpTriggerPt->SetLineColor(kBlue);
  // leg->AddEntry(PpTriggerPt, "Training GEANT Result");
  // McTriggerPt->SetLineColor(kOrange);
  // leg->AddEntry(McTriggerPt, "Matched Training Truth");
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

  // new TCanvas ( "c22","",500, 500);
  // // gPad->SetCanvasSize(450,450);
  // RelDeltaRecoilPt->Draw("colz");
  // gPad->SaveAs(PlotBase+"_RelDeltaPt.png");

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

  // new TCanvas ( "c5","",500, 500);
  // gPad->SetCanvasSize(450,450);
  // McPpTriggerZg2030->Draw("colz");
  // l.SetLineStyle(7);
  // l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
  // gPad->SaveAs(PlotBase+"_ZgResponseHist.png");

  new TCanvas ( "c5","",500, 500);
  gPad->SetCanvasSize(450,450);
  McPpTriggerZg10plus->Draw("colz");
  l.SetLineStyle(7);
  l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
  gPad->SaveAs(PlotBase+"_ZgResponseHist.png");
  
  new TCanvas ( "c6","",500, 500);
  gPad->SetCanvasSize(450,450);

  gPad->SaveAs(PlotBase+"_ZgResponses.pdf[");
  for ( int i=1; i<=McPpTriggerZg3d->GetNbinsZ(); ++i ){
    McPpTriggerZg3d->GetZaxis()->SetRange(i,i);
    TH2D* h2=(TH2D*)McPpTriggerZg3d->Project3D("yx");
    TString s=McPpTriggerZg3d->GetName(); s+=i;
    h2->SetName(s);
    s="p_{T}^{truth}=";
    s+= int(McPpTriggerZg3d->GetZaxis()->GetBinLowEdge(i)+0.01);
    s+=" - ";
    s+= int(McPpTriggerZg3d->GetZaxis()->GetBinLowEdge(i+1)+0.01);
    s+=" GeV/c ";
    h2->SetTitle(s);    
    h2->Draw("colz");
    l.SetLineStyle(7);
    l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
    // if (i==1) gPad->SaveAs(PlotBase+"_ZgResponses.pdf[");
    // else if (i==McPpTriggerZg3d->GetNbinsZ()) gPad->SaveAs(PlotBase+"_ZgResponses.pdf]");
    // else
    gPad->SaveAs(PlotBase+"_ZgResponses.pdf");
  }
  gPad->SaveAs(PlotBase+"_ZgResponses.pdf]");
 
  // new TCanvas ( "c5","",500, 500);
  // gPad->SetCanvasSize(450,450);
  // TrigPtZgResponse2D->Hmeasured()->Draw("colz");
  // l.SetLineStyle(7);
  // l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
  // gPad->SaveAs(PlotBase+"_ZgResponseHist.png");

  
    
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


  new TCanvas;     hhh->DrawNormalized();
  hhh->SaveAs("hhh.root");

  return 0;

}
