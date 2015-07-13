#include "AjAnalysis.hh"
#include "AjParameters.hh"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TChain.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>

#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <exception>

using namespace std;
using namespace fastjet;

int main ( int argc, const char** argv ) {

  TString input = "Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root";
  // TString input = "Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB_8177020_DC4BA348C050D5562E7461357C4B341D_0.root";

  TString OutFileName = "LorentzMB_Top20.root";

  // Load and set up AuAu tree
  // --------------------
  TString ChainName  = "JetTree";
  TString TriggerName = "MB";
  TChain* chain = new TChain( ChainName );
  chain->Add( input );

  // int RefMultCut=0;
  int RefMultCut=269;
  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TTree* TriggeredTree=0;
  static const Int_t kmaxT=5000; // max # of particles
  TClonesArray FullEvent("TLorentzVector",kmaxT);

  int eventid;
  int runid;
  double refmult; // Really an int, but may change if using refmultcorr
  TriggeredTree = new TTree("TriggeredTree","Triggered Events");
  // NOTE: Ignore "Warning in <TTree::Bronch>: Using split mode on a class: TLorentzVector with a custom Streamer"
  // See: https://root.cern.ch/phpBB3/viewtopic.php?p=18269
  TriggeredTree->Branch("FullEvent", &FullEvent );
  TriggeredTree->Branch("eventid",&eventid, "eventid/i");
  TriggeredTree->Branch("runid",&runid, "runid/i");
  TriggeredTree->Branch("refmult",&refmult, "refmult/d");
    
  
  // Helpers
  // -------
  TStarJetVectorContainer<TStarJetVector>* container;
  TLorentzVector* lv;
  
  // Get AuAu data
  // -------------
  // Long64_t nEvents=9999; // -1 for all
  Long64_t nEvents=-1;
  reader.Init(nEvents);
  
  Long64_t nJetsUsed=0;
  while ( reader.NextEvent() ) {
    reader.PrintStatus(10);      
    
    // Load event
    // ----------
    container = reader.GetOutputContainer();

    // event info
    // ----------
    TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();
    refmult=header->GetGReferenceMultiplicity();
    eventid = header->GetEventId();
    runid   = header->GetRunId();

    // Make AuAu vector
    // ----------------
    FullEvent.Clear();

    for ( int ip = 0; ip< container->GetEntries() ; ++ip ){
      lv = container->Get(ip);
      new (FullEvent[ip]) TLorentzVector (*lv);
    }
    
    TriggeredTree->Fill();    
    
  } // reader.NextEvent()
  cout << "##################################################################" << endl;

  // Close up shop
  // -------------
  fout->Write();


  return 0;
  
}
