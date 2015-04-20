/** @file PythiaInAuAuAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Similar to ppInAuAuAj, but use pythia events
    @date Mar 04, 2015
*/

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
#include <tBranch.h>
#include <TMath.h>
#include <TRandom.h>

#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <exception>

using namespace std;
using namespace fastjet;

/** 
    - Set up vector of pythia jets
    - Set up Au+Au input tree
    - Set up output histos and tree
    - First cycle over Au+Au - Determine acceptable events
    - To each pythia event, assign at random a fixed amount of Au+Au events 
      that it wishes to be mixed with
    - Second cycle over Au+Au - for each event, perform Aj Analysis
      with every pythia event that asks for it.
    The idea behind this two step approach is that we avoid jumping at random through the AuAu chain,
    the tree gets read sequentially, and exactly twice.
*/
int main () {

  // How many times to use every pythia jet
  // ----------------------------------
  Int_t nMix=3;

  gRandom->SetSeed(1);

  // Load pythia Jets
  // ----------------
  TString PythiaName = "pytest.root";
  TChain* PythiaJets = new TChain("tree");
  PythiaJets->Add(PythiaName);
  assert ( PythiaJets->GetEntries()>0 && "Something went wrong loading the Pythia jets.");
  
  TClonesArray* pFullEvent = new TClonesArray("TLorentzVector");
  PythiaJets->GetBranch("Particles")->SetAutoDelete(kFALSE);
  PythiaJets->SetBranchAddress("Particles", &pFullEvent);

  // Throw them into convenient vectors
  // ----------------------------------
  vector< vector<PseudoJet> > FullPythiaEvent;
  vector<PseudoJet> CurrentPythiaEvent;
  TLorentzVector* lv;
  // Int_t NPythiaEvents=PythiaJets->GetEntries();
  Int_t NPythiaEvents=30000;
  cout << "Filling pythia vector" << endl;
  for ( Long64_t pythiaEv = 0; pythiaEv< NPythiaEvents ; ++ pythiaEv ){
    PythiaJets->GetEntry(pythiaEv);
    CurrentPythiaEvent.clear();
    for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
      lv = (TLorentzVector*) pFullEvent->At(i);
      // Ensure kinematic similarity
      if ( lv->Pt()<0.2 ) continue;
      if ( fabs( lv->Eta()>1) ) continue;
      CurrentPythiaEvent.push_back( MakePseudoJet( lv ) );
    }
    FullPythiaEvent.push_back ( CurrentPythiaEvent );
  }

  // Load and set up AuAu tree
  // --------------------
  TString ChainName  = "JetTree";
  TString TriggerName = "MB";
  TString InFileName = "Data/AuAuMB_0_20/*.root";

  TChain* chain = new TChain( ChainName );
  chain->Add( InFileName );
  
  int RefMultCut=AjParameters::AuAuRefMultCut;
  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );
  // TStarJetPicoDefinitions::SetDebugLevel(10);

  // Files and histograms
  // --------------------
  TString OutFileName = "AjResults/PythiaInAuAuAj.root";
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  TH2D* UnmatchedhPtHi = new TH2D( "UnmatchedhPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* hPtHi = new TH2D( "hPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* hPtLo = new TH2D( "hPtLo","p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* UnmatchedhdPtHi = new TH1D( "UnmatchedhdPtHi","#Delta p_{T} for unmatched hard constituent jets", 120, -10, 50 );
  TH1D* hdPtHi = new TH1D( "hdPtHi","#Delta p_{T} for hard constituent jets", 120, -10, 50 );
  TH1D* hdPtLo = new TH1D( "hdPtLo","#Delta p_{T} for soft constituent jets", 120, -10, 50 );
    
  TH1D* hdphiHi = new TH1D( "hdphiHi","#Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* hdphiLo = new TH1D( "hdphiLo","#Delta#phi for soft constituent jets", 200, -2, 2 );

  TH1D* UnmatchedAJ_hi = new TH1D( "UnmatchedAJ_hi","Unmatched A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* AJ_hi = new TH1D( "AJ_hi","A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* AJ_lo = new TH1D( "AJ_lo","A_{J} for soft constituent jets", 40, -0.3, 0.9 );

  TH3D* UsedEventsHiPhiEtaPt=new TH3D("UsedEventsHiPhiEtaPt","UsedEventsHiPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.0, 10); // QA
  TH3D* UsedEventsLoPhiEtaPt=new TH3D("UsedEventsLoPhiEtaPt","UsedEventsLoPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.0, 10); // QA  
  
  // Helpers
  // -------
  vector<PseudoJet> particles;
  vector<PseudoJet> AuAuparticles;
  TStarJetVectorContainer<TStarJetVector>* container;
  
  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  
  // For pythia jet selection
  // ------------------------
  gRandom->SetSeed(1);

  // Long64_t nEvents=100; // -1 for all
  Long64_t nEvents=-1;
  reader.Init(nEvents);

  // First round - initialize AuAu data
  // ----------------------------------
  cout << "First round - find acceptable AuAu events" << endl;
  // NOTE: This could be a problem if we ever have more than INTMAX events :-/
  // Could solve using http://en.cppreference.com/w/cpp/numeric/random
  vector <Int_t> AcceptedEvents; 
  while ( reader.NextEvent() ) {
    reader.PrintStatus(10);      
    AcceptedEvents.push_back( reader.GetNOfCurrentEvent() );
  }
  
  // Now every pythia jet gets assigned nMix many events to be embedded in
  // This way we can cycle through the potentially large AuAu chain sequentially
  // ---------------------------------------------------------------------------
  cout << "Initializing mixing pools." << endl;
  Int_t random;
  int emergencystop=nMix*100;
  vector< set<Int_t> > MixingPool;
  for ( int i =0; i< FullPythiaEvent.size() ; ++i ){
    set<Int_t> EventNos;
    int tries=0;
    while ( EventNos.size() < nMix ){
      if ( tries++ > emergencystop ) { cerr << "Stuck in endless loop - aborting." << endl; return -1;}
      random = gRandom->Integer (AcceptedEvents.size() );
      EventNos.insert(AcceptedEvents.at(random) );	 
    }
    MixingPool.push_back( EventNos );
  }  

  // Second round - actual analysis
  // ------------------------------
  cout << "Second round - embed" << endl;  

  // Initialize analysis class
  // -------------------------
  AjAnalysis AjA( AjParameters::R, AjParameters::jet_ptmin, AjParameters::jet_ptmax,
		  AjParameters::LeadPtMin, AjParameters::SubLeadPtMin, 
		  AjParameters::max_track_rap, AjParameters::PtConsLo, AjParameters::PtConsHi,
		  AjParameters::dPhiCut,
		  UnmatchedhPtHi,  hPtHi, hPtLo,  
		  UnmatchedhdPtHi, hdPtHi, hdPtLo,
		  hdphiHi, hdphiLo,
		  UnmatchedAJ_hi, AJ_hi, AJ_lo,
		  UsedEventsHiPhiEtaPt, UsedEventsLoPhiEtaPt
		  );

  // Reset the reader
  // ----------------
  reader.ResetEventCounters();
  reader.Init(nEvents);
  
  Long64_t nJetsUsed=0;
  while ( reader.NextEvent() ) {
    reader.PrintStatus(10);      
    
    // Load event
    // ----------
    container = reader.GetOutputContainer();

    // Make AuAu vector
    // ----------------
    AuAuparticles.clear();
    for (int ip = 0; ip<container->GetEntries() ; ++ip ){
      lv = container->Get(ip);
      AuAuparticles.push_back ( MakePseudoJet( lv ) );
    }

    // Find jets that want this event
    // ------------------------------
    vector <int> JetIndices;
    for ( int i =0; i< MixingPool.size() ; ++i ){
      set<Int_t>& EventNos = MixingPool.at(i);
      if ( EventNos.find( reader.GetNOfCurrentEvent() ) != EventNos.end() ) {
	JetIndices.push_back( i );
      }
    }

    // And run analysis individually for each of these pythia events
    // ---------------------------------------------------------- 
    for ( vector<int>::iterator jit=JetIndices.begin(); jit!=JetIndices.end(); ++ jit ){
      // Add pythia jets
      // -----------
      particles = AuAuparticles;
      for ( int i=0; i < FullPythiaEvent.at(*jit).size() ; ++i ){
	particles.push_back( FullPythiaEvent.at(*jit).at(i) );
      }
      
      // Run analysis
      // ------------
      int ret =AjA.AnalyzeAndFill( particles );
      
      switch ( ret ){
      case 3 : nMatchedDijets++;
	// FALLTHROUGH
      case 2 : nCorrespondingLowDijets++;
	// FALLTHROUGH
      case 1 : nHardDijets++;
	// FALLTHROUGH
      case 0 : /* Nothing found */
	break;
      default :
	cerr << "Unrecognized return value!" << endl;
	throw(-1);
	return -1;
	break;      
      } //  switch (ret)
      nJetsUsed++;
    } // jit
  } // reader.NextEvent()
  cout << "##################################################################" << endl;


  // Scale per used pythia event
  // ------------------------
  UsedEventsHiPhiEtaPt->Scale( 1./nJetsUsed ); 

  // Close up shop
  // -------------
  fout->Write();

  cout << "Embedded " << nJetsUsed << " in " << reader.GetNOfEvents() << " events and found " << endl
       << nHardDijets << " dijets with constituents above 2 GeV," << endl
       << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
       << " of which " <<  nMatchedDijets << " could be matched." << endl;


  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;
  
  return 0;
  
}
