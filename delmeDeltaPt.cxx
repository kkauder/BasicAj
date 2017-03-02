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


int DeltaPt()
{
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  TLegend* leg = 0;
  
  float RCut = 0.4;
  float EtaCut = 1.0-RCut;

  // Input
  // -----
  TString * sTruth =  "AjResults/Groom_Aj_TrueMB_NoEff_GeantMc.root";
  TString * sMeas  =  "AjResults/Groom_Aj_HT54_Geant.root";

  // Refmult Cut
  // -----------
  int RefMultCut=0;

    // Set up pp events
  // ----------------
  TChain* PpJets = new TChain("ResultTree");
  PpJets->Add ( PpLevelFile );
  PpJets->BuildIndex("runid","eventid");

  TClonesArray* j1 = new TClonesArray("TLorentzVector");
  PpJets->GetBranch("j1")->SetAutoDelete(kFALSE);
  PpJets->SetBranchAddress("j1", &j1);

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




}
