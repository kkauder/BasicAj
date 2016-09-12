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

int PrepUnfolding ( ) {
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  TLegend* leg = 0;
  
  float RCut = 0.4;
  float EtaCut = 1.0-RCut;

    
  // Input
  // -----
  TString PpLevelFile = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root"; // pp-like events

  // Output
  // ------
  // TString OutFileName = "PpForUnfolding.root";
  TString OutFileName = "HThistos/ForUnfolding_";
  OutFileName += gSystem->BaseName(PpLevelFile);
  

  // Refmult Cut
  // -----------
  int RefMultCut=0;

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

  TH1D* PpTrigPt = new TH1D( "PpTrigPt",";Trigger p_{T}^{Det} [GeV/c]", nPtBins, ptmin, ptmax );

  // Set up Data
  // ----------------------
  // int nZgBinsMeas = 20;
  // float zgminMeas = 0;
  // float zgmaxMeas = 0.5;
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
  int nPtBinsMeas =  nPtBins;
  float ptminMeas =  ptmin;
  float ptmaxMeas =  ptmax;

  // Only need one of these
  TH2D* Meas2D  = new TH2D( "Meas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia in MC", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  TH2D* TestMeas2D  = new TH2D( "TestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia in MC", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  // ------------------------
  // Loop over measured level
  // ------------------------

  cout << "Loop over measured data" << endl;
  cout << "------------------------" << endl;

  for ( Long64_t ppEvi = 0; ppEvi< PpJets->GetEntries() ; ++ppEvi ){
    if ( !(ppEvi%10000) ) cout << "Working on " << ppEvi << " / " << PpJets->GetEntries() << endl;
    PpJets->GetEntry(ppEvi);
    TLorentzVector* PpT = (TLorentzVector*) PpTrigger->At(0);

    // Require truth in acceptance
    // ---------------------------
    if ( fabs ( PpT->Eta() ) > EtaCut ) continue;
    PpTrigPt->Fill( PpT->Pt(), ppweight );
        
    TLorentzVector* PpA = 0;
    if (PpAwayJet->GetEntries()!=1 ) PpA = (TLorentzVector*) PpAwayJet->At(0);

    Meas2D->Fill( PpT->Pt(), ppzgtriglo, ppweight );
    TestMeas2D->Fill( PpT->Pt(), ppzgtriglo, ppweight );
    

  }

  new TCanvas ( "c0","");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  // leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "Trigger Jet (leading, above 10 GeV)" );
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);
  PpTrigPt->Draw();

  fout->Write();

  cout << " Wrote to" << endl << OutFileName << endl;
  return 0;
}
