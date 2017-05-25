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

int ForResAj_MatchGeantToPythia_HCcheck (
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
  // bool RejectOutliers=false;
  // bool RejectHiweights=false;
  
  float EtaCut = 1.0-RCut;


  // Input
  // -----  
  TString File70 = "AjResults/CrossCheckForResAj_HC70_NoEff_HT54_Geant.root";
  TString File100  = "AjResults/ForResAj_NoEff_HT54_Geant.root";
  
  // Output
  // ------
  TString OutFileName = "AjResults/";
  OutFileName += gSystem->BaseName( File100 );
  OutFileName.ReplaceAll(".root","_");
  //  if ( RejectOutliers ) OutFileName += "NoOutliers_";
  OutFileName += "MatchedTo_";
  OutFileName += gSystem->BaseName( File70 );
  
  TString PlotBase = "plots/";
  PlotBase += gSystem->BaseName( File100 );
  PlotBase.ReplaceAll(".root","_");
  // if ( RejectOutliers ) PlotBase += "NoOutliers_";
  PlotBase += "MatchedTo_";
  PlotBase += gSystem->BaseName(File70);
  PlotBase.ReplaceAll(".root","");
  
  // Refmult Cut
  // -----------
  int RefMultCut=0;

  // Load Trees
  // ----------
  TChain* Jets70 = new TChain("TriggeredTree");
  Jets70->Add ( File70 );
  Jets70->BuildIndex("runid","eventid");

  // TLorentzVector* File70Trigger = new TLorentzVector();
  // Jets70->SetBranchAddress("j1", &File70Trigger);

  // TLorentzVector* File70TriggerLo = new TLorentzVector();
  // Jets70->SetBranchAddress("jm1", &File70TriggerLo);

  // TLorentzVector* File70AwayJet = new TLorentzVector();
  // Jets70->SetBranchAddress("j2", &File70AwayJet);

  // TLorentzVector* File70AwayLo = new TLorentzVector();
  // Jets70->SetBranchAddress("jm2", &File70AwayLo);

  TClonesArray* File70Trigger = new TClonesArray("TLorentzVector");
  Jets70->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  Jets70->SetBranchAddress("TriggerJet", &File70Trigger);

  TClonesArray* File70TriggerLo = new TClonesArray("TLorentzVector");
  Jets70->GetBranch("TriggerJetLo")->SetAutoDelete(kFALSE);
  Jets70->SetBranchAddress("TriggerJetLo", &File70TriggerLo);

  // On the other side
  TClonesArray* File70AwayJet = new TClonesArray("TLorentzVector");
  Jets70->GetBranch("AwayJet")->SetAutoDelete(kFALSE);
  Jets70->SetBranchAddress("AwayJet", &File70AwayJet);

  TClonesArray* File70AwayLo = new TClonesArray("TLorentzVector");
  Jets70->GetBranch("AwayJetLo")->SetAutoDelete(kFALSE);
  Jets70->SetBranchAddress("AwayJetLo", &File70AwayLo);

  int eventid70;
  int runid70;
  double weight70;     // Double-check, should be the same as below
  Jets70->SetBranchAddress("eventid", &eventid70);
  Jets70->SetBranchAddress("runid", &runid70);
  Jets70->SetBranchAddress("weight",&weight70 );

  // Set up pp events
  // ----------------
  TChain* Jets100 = new TChain("ResultTree");
  Jets100->Add ( File100 );
  Jets100->BuildIndex("runid","eventid");

  TLorentzVector* File100Trigger = new TLorentzVector();
  Jets100->SetBranchAddress("j1", &File100Trigger);

  TLorentzVector* File100TriggerLo = new TLorentzVector();
  Jets100->SetBranchAddress("jm1", &File100TriggerLo);

  TLorentzVector* File100AwayJet = new TLorentzVector();
  Jets100->SetBranchAddress("j2", &File100AwayJet);

  TLorentzVector* File100AwayLo = new TLorentzVector();
  Jets100->SetBranchAddress("jm2", &File100AwayLo);

  int eventid100;
  int runid100;
  double weight100;     // Double-check, should be the same as below
  Jets100->SetBranchAddress("eventid", &eventid100);
  Jets100->SetBranchAddress("runid", &runid100);
  Jets100->SetBranchAddress("weight",&weight100);

  // Output and histograms
  // ----------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  int nPtBins = 80;
  float ptmin=0;
  float ptmax=80;

  TH2D* DeltaTriggerPt      = new TH2D( "DeltaTriggerPt"   , ";p_{T}^{HC100};Trigger p_{T}^{HC70}-p_{T}^{HC100} [GeV/c]", nPtBins, ptmin, ptmax, 100, -10, 10 );
  TH2D* DeltaTriggerPtLo    = new TH2D( "DeltaTriggerPtLo" , ";p_{T}^{HC100,matched};Trigger p_{T}^{HC70}-p_{T}^{HC100} [GeV/c]", nPtBins, ptmin, ptmax, 100, -10, 10 );
 
  TH2D* DeltaRecoilPt      = new TH2D( "DeltaRecoilPt"   , ";p_{T}^{HC100};Recoil p_{T}^{HC70}-p_{T}^{HC100} [GeV/c]", nPtBins, ptmin, ptmax, 100, -10, 10 );
  TH2D* DeltaRecoilPtLo    = new TH2D( "DeltaRecoilPtLo" , ";p_{T}^{HC100,matched};Recoil p_{T}^{HC70}-p_{T}^{HC100} [GeV/c]", nPtBins, ptmin, ptmax, 100, -10, 10 );

  TH2D* RelDeltaTriggerPt      = new TH2D( "RelDeltaTriggerPt"   , ";p_{T}^{HC100};Rel p_{T} Shift", nPtBins, ptmin, ptmax, 100, 0, 0.5);
  TH2D* RelDeltaTriggerPtLo    = new TH2D( "RelDeltaTriggerPtLo" , ";p_{T}^{HC100,matched};Rel p_{T} Shift", nPtBins, ptmin, ptmax, 100, 0, 0.5);

  TH2D* RelDeltaRecoilPt      = new TH2D( "RelDeltaRecoilPt"   , ";p_{T}^{HC100};Rel p_{T} Shift", nPtBins, ptmin, ptmax, 100, 0, 0.5);
  TH2D* RelDeltaRecoilPtLo    = new TH2D( "RelDeltaRecoilPtLo" , ";p_{T}^{HC100,matched};Rel p_{T} Shift", nPtBins, ptmin, ptmax, 100, 0, 0.5);
  
  // ------------------------
  // Loop over detector level
  // ------------------------

  cout << "Loop over File100" << endl;
  cout << "-----------------" << endl;
  
  int lost=0;
  for ( Long64_t Evi100 = 0; Evi100<Jets100->GetEntries() ; ++Evi100 ){
    if ( !(Evi100%10000) ) cout << "Working on " << Evi100 << " / " << Jets100->GetEntries() << endl;
    Jets100->GetEntry(Evi100);
    TLorentzVector* File100T = File100Trigger;
    TLorentzVector* File100A = File100AwayJet;
    if (File100T->Pt() <20 || File100A->Pt() <10 ) {
      cerr << File100T->Pt() << "  " <<  File100A->Pt() << endl;
      return -1;
    }
            
    TLorentzVector* File100TLo = File100TriggerLo;
    TLorentzVector* File100ALo = File100AwayLo;
    if (File100TLo->Pt() <1 || File100ALo->Pt() <1 ) {
      cerr << File100TLo->Pt() << "  " <<  File100ALo->Pt() << endl;
      return -1;
    }
        
    // Get corresponding HC70 event
    // ----------------------------
    Long64_t Evi70=-1;
    Evi70 = Jets70->GetEntryNumberWithIndex( runid100, eventid100 );
    if ( Evi70 < 0 ){
      cerr << "Couldn't find this event in HC70!" << endl;
      cerr << runid100 << "  " <<  eventid100 << endl;
      // Skip this event
      cout << ++lost << endl;
      continue;
    }

    Jets70->GetEntry(Evi70);
    TLorentzVector* File70TLo = (TLorentzVector*) File70TriggerLo->At(0);
    TLorentzVector* File70T = (TLorentzVector*) File70Trigger->At(0);
    TLorentzVector* File70ALo = 0;
    TLorentzVector* File70A = 0;
    if (File70AwayJet->GetEntries()!=0 ) {
      File70ALo = (TLorentzVector*) File70AwayLo->At(0);
      File70A   = (TLorentzVector*) File70AwayJet->At(0);
    }
    
    // Find best match
    // ---------------
    TLorentzVector* MatchT = 0;
    TLorentzVector* MatchTLo = 0;
    if ( File70T->DeltaR( *File100T ) < RCut ) {
      MatchT = File70T;
      MatchTLo = File70TLo;
    }
    else if ( File70A && File70A->DeltaR( *File100T ) < RCut ) {
      MatchT = File70A;
      MatchTLo = File70ALo;
    }

    TLorentzVector* MatchA = 0;
    TLorentzVector* MatchALo = 0;
    if ( !File70A ) {
      // lost recoil jet
      cout << ++lost << endl;
      continue;
    }

    if ( File70A->DeltaR( *File100A ) < RCut ) {
      MatchA = File70A;
      MatchALo = File70ALo;
    } else if ( File70T->DeltaR( *File100A ) < RCut ) {
      MatchA = File70T;
      MatchALo = File70TLo;
    }
    
    if ( !MatchA ) {
      // lost recoil jet
      cout << ++lost << endl;
      continue;
    }


    if ( !MatchT ) {
      // lost trigger jet
      cout << ++lost << endl;
      // cout << "Lost the trigger jet" << endl;
      // cout << "PP: Pt=" << File100T->Pt() << "  Eta=" << File100T->Eta() << "  Phi=" << File100T->Phi() << endl;
      // cout << "MC trig: Pt=" << File70T->Pt() << "  Eta=" << File70T->Eta() << "  Phi=" << File70T->Phi() << endl;
      // if ( File70A && File70A->Pt()>0)
      // 	cout << "MC away: Pt=" << File70A->Pt() << "  Eta=" << File70A->Eta() << "  Phi=" << File70A->Phi() << endl;
      // cout << endl;
      continue;
    }
    
    // Done, we have matched pairs
    // ---------------------------

    
    // // Skip high weight outliers
    // // -------------------------
    // if ( RejectHiweights ) {
    //   if ( GeantWeightReject ( RCut, File100T->Pt(), ppweight, 10 ) )  {
    // 	cout << "Skipping RECO TRIGGER with pt=" << File100T->Pt() << " due to high weight" << endl;
    // 	cout << ++lost << endl;
    // 	continue;
    //   }
      
    //   if ( GeantWeightReject ( RCut, File100A->Pt(), ppweight, 11 ) )  {
    // 	cout << "Skipping RECO RECOIL with pt=" << File100A->Pt() << " due to high weight" << endl;
    // 	cout << ++lost << endl;
    // 	continue;
    //   }

    //   if ( GeantWeightReject ( RCut, MatchT->Pt(), mcweight, 0 ) )  {
    // 	cout << "Skipping TRIGGER with pt=" << MatchT->Pt() << " due to high weight" << endl;
    // 	cout << ++lost << endl;
    // 	continue;
    //   }
    //   if ( GeantWeightReject ( RCut, MatchA->Pt(), mcweight, 1 ) )  {
    // 	cout << "Skipping RECOIL with pt=" << MatchA->Pt() << " due to high weight" << endl;
    // 	cout << ++lost << endl;
    // 	continue;
    //   }
    // }

    // // Skip other outliers
    // // -------------------
    // if ( RejectOutliers ){
    //   if ( (File100T->Pt() - MatchT->Pt()) / MatchT->Pt() < -0.4 ) {
    // 	cout << ++lost << endl;
    // 	continue;
    //   }
    // }
    
    // cout << File100T->Pt() << " - " <<  MatchT->Pt() << " = " << File100T->Pt() - MatchT->Pt() << endl;
    // cout << File100T->Pt() << " - " <<  MatchT->Pt() << " = " << File100T->Pt() - MatchT->Pt() << endl;
    // cout << File100A->Pt() << " - " <<  MatchA->Pt() << " = " << File100A->Pt() - MatchA->Pt() << endl;
    // cout << File100TLo->Pt() << " - " <<  MatchTLo->Pt() << " = " << File100TLo->Pt() - MatchTLo->Pt() << endl;
    // cout << File100ALo->Pt() << " - " <<  MatchALo->Pt() << " = " << File100ALo->Pt() - MatchALo->Pt() << endl;
    // cout << endl;
    
    // Fill histos
    // -----------
    DeltaTriggerPt->Fill( File100T->Pt(), MatchT->Pt() - File100T->Pt(), weight70);
    RelDeltaTriggerPt->Fill( File100T->Pt(), fabs( (MatchT->Pt() - File100T->Pt()) / MatchT->Pt() ), weight70);

    DeltaTriggerPtLo->Fill( File100TLo->Pt(), MatchTLo->Pt() - File100TLo->Pt(), weight70);
    RelDeltaTriggerPtLo->Fill( File100TLo->Pt(), fabs( (MatchTLo->Pt() - File100TLo->Pt()) / MatchTLo->Pt() ), weight70);

    DeltaRecoilPt->Fill( File100A->Pt(), MatchA->Pt() - File100A->Pt(), weight70);
    RelDeltaRecoilPt->Fill( File100A->Pt(), fabs( (MatchA->Pt() - File100A->Pt()) / MatchA->Pt() ), weight70);
    
    DeltaRecoilPtLo->Fill( File100ALo->Pt(), MatchALo->Pt() - File100ALo->Pt(), weight70);
    RelDeltaRecoilPtLo->Fill( File100ALo->Pt(), fabs( (MatchALo->Pt() - File100ALo->Pt()) / MatchALo->Pt() ), weight70);

  }
    
  // Done
  // ----
  fout->Write();
  cout << " Wrote to" << endl << OutFileName << endl;

  return 0;

}
