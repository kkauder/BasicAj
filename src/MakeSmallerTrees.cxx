// #include "AjAnalysis.hh"
// #include "AjParameters.hh"

#include "JetAnalyzer.hh"

#include "TStarJetPicoReader.h"
#include "TStarJetPicoEvent.h"
#include "TStarJetPicoEventHeader.h"
#include "TStarJetPicoEventCuts.h"

#include "TStarJetPicoPrimaryTrack.h"
#include "TStarJetPicoTrackCuts.h"
#include "TStarJetPicoTowerCuts.h"

#include "TStarJetVectorContainer.h"
#include "TStarJetVector.h"
#include "TStarJetPicoTriggerInfo.h"
#include "TStarJetPicoUtils.h"

#include "TSystem.h"
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"

#include <iostream>
#include <exception>

#include <stdlib.h>     // for getenv

using namespace std;
using namespace fastjet;


int main ( int argc, const char** argv ) {

  // TString inname="CleanAuAu/Clean809.root";
  // TString outname="SmallAuAu/TEST.root";
  TString inname="/data3/HaddedAuAu11picoNPE18/AuAu11PicoNPE18_Cent8_0.root";
  TString outname="/Users/kkauder/SmallNPE18/TEST.root";
  TString TriggerName="HT";

  switch (argc){
  case 1 :
    // use default values
    break;
  case 2 :
    inname = argv[1];
    outname=gSystem->BaseName(inname);
    outname.Prepend("/Users/kkauder/SmallNPE18/Small_");
    break;
  default :
    cerr << "Incompatible arguments." << endl;
    return -1;
    break;
  }

  cout << "Reading from " << inname << endl;
  cout << "Writing to " << outname << endl;
    
  // Load input
  // ----------
  TChain* chain = new TChain ( "JetTree");
  chain->Add( inname );

  double RefMultCut = 0;
  double MinJetPt=16;
  TStarJetPicoReader reader;
  reader.SetInputChain (chain);
  

  // Event and track selection
  // -------------------------
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  evCuts->SetTriggerSelection( TriggerName ); //All, MB, HT, pp, ppHT, ppJP
  // Additional cuts 
  evCuts->SetVertexZCut ( 30 );
  evCuts->SetRefMultCut ( RefMultCut );
  evCuts->SetVertexZDiffCut( 100 ); // SHOULD be smaller, like 3-6
  evCuts->SetMinEventEtCut ( -1.0 ); // Could go to 5.5, 6?

  // Keep things inclusive! Keep electrons and use MIP correction for jet finding
  reader.SetRejectTowerElectrons( kFALSE );
  reader.SetApplyFractionHadronicCorrection(kFALSE);

  // SHOULD ADD BAD RUN LIST
  
  // Tracks cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  trackCuts->SetDCACut( 2 ); //  SHOULD be 1?
  trackCuts->SetMinNFitPointsCut( 20 );
  trackCuts->SetFitOverMaxPointsCut( 0.52 );
  trackCuts->SetMaxPtCut ( 1000 ); // SHOULD be smaller, like 15-30?

  cout << "Using these track cuts:" << endl;
  cout << " dca : " << trackCuts->GetDCACut(  ) << endl;
  cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << endl;
  cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << endl;
  cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << endl;
  
  // Towers
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  towerCuts->SetMaxEtCut( 1000 ); // SHOULD be smaller, say 45
  towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/badTowerList_y11.txt");
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/OrigY7MBBadTowers.txt");

  cout << "Using these tower cuts:" << endl;
  cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << endl;
  cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << endl;
  
  // V0s: Turn off
  reader.SetProcessV0s(false);
  TStarJetPicoDefinitions::SetDebugLevel(0);

  Long64_t nEvents=-1; // -1 for all
  // nEvents=10000;
  reader.Init(nEvents);

  Int_t nAccepted=0;
  Int_t n10=0; // Just out of curiosity, count how many above 10 GeV
  
  // Set up output
  // -------------
  TFile * fout = new TFile (outname, "RECREATE");
  // TTree::SetMaxTreeSize(40000000LL ); // 10000000LL == 10 MB
  TTree::SetMaxTreeSize(200000000LL ); // 10000000LL == 10 MB
  // TTree::SetMaxTreeSize( 1000000LL);
  TTree* outTree=new TTree("JetTree","Preselected Pico Tree for Jet");
  TStarJetPicoEvent *mEvPico;
  outTree->Branch("PicoJetTree",&mEvPico);

  // Set up jet finding
  // ------------------
  double PtConsHi = 2.0;
  double max_track_rap=1.0;

  // allow two Rs. Smaller R should be a subset of larger R,
  // but it scans out to larger rapidity
  double R1 = 0.4;
  double R2 = 0.2;
  JetDefinition jet_def1 = fastjet::JetDefinition(fastjet::antikt_algorithm, R1);
  JetDefinition jet_def2 = fastjet::JetDefinition(fastjet::antikt_algorithm, R2);
  
  Selector select_track_rap = fastjet::SelectorAbsRapMax(max_track_rap);
  Selector select_hipt      = fastjet::SelectorPtMin( PtConsHi );
  Selector shi     = select_track_rap * select_hipt;
  
  vector<PseudoJet> particles;
  PseudoJet pj;
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv; // TLorentzVector* would be sufficient. 

  
  try{
    while ( reader.NextEvent() ) {
      reader.PrintStatus(10);
      
      // eventid = header->GetEventId();
      // runid   = header->GetRunId();
      // refmult=0;
      // if ( isAuAu ) refmult=header->GetGReferenceMultiplicity();
      
      // Load event
      // ----------
      container = reader.GetOutputContainer();
      
      // Make particle vector
      // --------------------
      particles.clear();
    
      for (int ip = 0; ip<container->GetEntries() ; ++ip ){
	sv = container->Get(ip);  // Note that TStarJetVector  contains more info, such as charge;
	// mostly for demonstration, add charge information
	pj=MakePseudoJet( sv );
	pj.set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge() ) );
	particles.push_back ( pj );
      }    

      // find high constituent pT jets
      // -----------------------------
      vector<PseudoJet> pHi = shi( particles );
      JetAnalyzer JA1 ( pHi, jet_def1 ); // NO background subtraction
      JetAnalyzer JA2 ( pHi, jet_def2 ); // NO background subtraction
      vector<PseudoJet> JAResult1 = fastjet::sorted_by_pt( JA1.inclusive_jets() );     // NO jet selector
      vector<PseudoJet> JAResult2 = fastjet::sorted_by_pt( JA2.inclusive_jets() );     // NO jet selector
      if ( JAResult1.size()<1 && JAResult2.size()<1  )         continue;
	    
      if ( JAResult1.at(0).pt()>10 )         n10++;


      if ( ( JAResult1.size()>0 && JAResult1.at(0).pt()> MinJetPt ) ||
	   ( JAResult2.size()>0 && JAResult2.at(0).pt()> MinJetPt )     ){

	nAccepted++;
	mEvPico=(TStarJetPicoEvent*) reader.GetEvent();
	outTree->Fill();
      } // has trigger
      
    } // while NextEvent
  } catch ( exception& e) {
    cerr << "Caught " << e.what() << endl;
    return -1;
  }
  
  outTree->Write();  

  cout << "Started with " << chain->GetEntries() << " events." << endl;
  cout << "Accepted " << nAccepted << " events." << endl;
  cout << "There were " << n10 << " events with a jet above 10 GeV." << endl;
  
  return 0;
  
}
