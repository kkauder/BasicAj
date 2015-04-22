/** @file ppInAuAuAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Aj analysis on pp jets embedded into AuAu.
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
#include <TSystem.h>

#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <exception>

using namespace std;
using namespace fastjet;

/** 
    - Set up vector of pp jets
    - Set up Au+Au input tree
    - Set up output histos and tree
    - First cycle over Au+Au - Determine acceptable events
    - To each pp event, assign at random a fixed amount of Au+Au events 
      that it wishes to be mixed with
    - Second cycle over Au+Au - for each event, perform Aj Analysis
      with every pp event that asks for it.
    The idea behind this two step approach is that we avoid jumping at random through the AuAu chain,
    the tree gets read sequentially, and exactly twice.
*/
int main ( int argc, const char** argv ) {

  // How many times to use every pp jet
  // THIS SHOULD BE ONE IN THE FINAL VERSION!!
  // -----------------------------------------
  Int_t nMix=6;
  if ( nMix !=1 ){
    cerr << " CAREFUL: FAKING BETTER PP STATISTICS " << endl;
    cout << " CAREFUL: FAKING BETTER PP STATISTICS " << endl;
  }
  
  // const char *defaults[4] = {"ppInAuAuAj","351ppInAuAuAj.root","MB","Data/AuAuMB_0_20/*.root"}; // CUT OFF at refmult<351
  const char *defaults[4] = {"ppInAuAuAj","ppInAuAuAj.root","MB","Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root"}; // CUT OFF at refmult<351
  if ( argc==1 ) {
    argv=defaults;
    argc=4;
  }
  // Throw arguments in a vector
  // ---------------------------
  vector<string> arguments(argv + 1, argv + argc);

  // Load pp Jets
  // ------------
  TString ppAjName = "AjResults/ppAj.root";
  if ( AjParameters::R==0.2 ){
    ppAjName.ReplaceAll ( gSystem->BaseName(ppAjName), TString ("R0.2_")+ gSystem->BaseName(ppAjName));
  }

    
  TChain* ppJets = new TChain("TriggeredTree");
  ppJets->Add(ppAjName);
  assert ( ppJets->GetEntries()>0 && "Something went wrong loading the pp jets.");

  TClonesArray* pFullEvent = new TClonesArray("TLorentzVector");
  ppJets->GetBranch("FullEvent")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("FullEvent", &pFullEvent);

  TClonesArray* pTriggerJet = new TClonesArray("TLorentzVector");
  ppJets->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("TriggerJet", &pTriggerJet);

  // Throw them into convenient vectors
  // ----------------------------------
  vector< vector<PseudoJet> > FullPpEvent;
  vector<PseudoJet> vTriggerJet;
  vector<PseudoJet> CurrentPpEvent;
  for ( Long64_t ppEv = 0; ppEv< ppJets->GetEntries() ; ++ ppEv ){
    ppJets->GetEntry(ppEv);
    CurrentPpEvent.clear();
    for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
      CurrentPpEvent.push_back( MakePseudoJet( (TLorentzVector*) pFullEvent->At(i) ) );
    }
    FullPpEvent.push_back ( CurrentPpEvent );
    if (pTriggerJet->GetEntries()!=1 ) cout << ppEv<< "  " << pTriggerJet->GetEntries() << endl;
    vTriggerJet.push_back( MakePseudoJet( (TLorentzVector*) pTriggerJet->At(0) ) );
  }

  // Load and set up AuAu tree
  // --------------------
  TString ChainName  = "JetTree";
  TString TriggerName = arguments.at(1);
  TChain* chain = new TChain( ChainName );
  for (int i=2; i<arguments.size() ; ++i ) {
    chain->Add( arguments.at(i).data() );
  }

  // int RefMultCut=AjParameters::AuAuRefMultCut;
  int RefMultCut=0;
  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );
  // TStarJetPicoDefinitions::SetDebugLevel(10);

  // Files and histograms
  // --------------------
  TString OutFileName = arguments.at(0);
  if ( AjParameters::R==0.2 ){
    OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("R0.2_")+ gSystem->BaseName(OutFileName));
  }

  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  TH2D* UnmatchedAJ_hi = new TH2D( "UnmatchedAJ_hi","Unmatched A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_hi = new TH2D( "AJ_hi","A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_lo = new TH2D( "AJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );

  TH2D* UnmatchedhPtHi = new TH2D( "UnmatchedhPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* hPtHi = new TH2D( "hPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* hPtLo = new TH2D( "hPtLo","p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* UnmatchedhdPtHi = new TH1D( "UnmatchedhdPtHi","#Delta p_{T} for unmatched hard constituent jets", 120, -10, 50 );
  TH1D* hdPtHi = new TH1D( "hdPtHi","#Delta p_{T} for hard constituent jets", 120, -10, 50 );
  TH1D* hdPtLo = new TH1D( "hdPtLo","#Delta p_{T} for soft constituent jets", 120, -10, 50 );

  TH1D* hdphiHi = new TH1D( "hdphiHi","#Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* hdphiLo = new TH1D( "hdphiLo","#Delta#phi for soft constituent jets", 200, -2, 2 );
  
  // Helpers
  // -------
  vector<PseudoJet> particles;
  vector<PseudoJet> AuAuparticles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TLorentzVector* lv;
  
  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  
  // For pp jet selection
  // --------------------
  gRandom->SetSeed(1);

  // Long64_t nEvents=99; // -1 for all
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
  
  // Now every pp jet gets assigned nMix many events to be embedded in
  // This way we can cycle through the potentially large AuAu chain sequentially
  // ---------------------------------------------------------------------------
  cout << "Initializing mixing pools." << endl;
  Int_t random;
  int emergencystop=nMix*100;
  vector< set<Int_t> > MixingPool;
  for ( int i =0; i< FullPpEvent.size() ; ++i ){
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
		  AjParameters::dPhiCut
		  );  

  // Reset the reader
  // ----------------
  reader.ResetEventCounters();
  reader.Init(nEvents);
  
  Long64_t nJetsUsed=0;
  double refmult; // Really an int, but may change if using refmultcorr
  while ( reader.NextEvent() ) {
    reader.PrintStatus(10);      
    
    // Load event
    // ----------
    container = reader.GetOutputContainer();

    // event info
    // ----------
    TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();
    refmult=header->GetGReferenceMultiplicity();

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

    
    // And run analysis individually for each of these pp events
    // ---------------------------------------------------------- 
    for ( vector<int>::iterator jit=JetIndices.begin(); jit!=JetIndices.end(); ++ jit ){
      // Add pp jets
      // -----------
      particles = AuAuparticles;
      for ( int i=0; i < FullPpEvent.at(*jit).size() ; ++i ){
	particles.push_back( FullPpEvent.at(*jit).at(i) );
      }
      
      // Run analysis
      // ------------
      int ret;
      ret =AjA.AnalyzeAndFill( particles, 0,refmult,
      // ret=AjA.AnalyzeAndFill( particles, &(vTriggerJet.at(*jit)), refmult, // Force matching to original trigger
			      UnmatchedAJ_hi, AJ_hi, AJ_lo,
			      
			      UnmatchedhPtHi,  hPtHi, hPtLo,
			      UnmatchedhdPtHi, hdPtHi, hdPtLo,
			      hdphiHi, hdphiLo
			      );
      // ret=AjA.AnalyzeAndFill( particles, &(vTriggerJet.at(*jit)) ); // Force matching to original trigger

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

  // Close up shop
  // -------------
  fout->Write();

  cout << "Embedded " << nJetsUsed << " jets above 10 GeV in " << reader.GetNOfEvents() << " events and found " << endl
       << nHardDijets << " dijets with constituents above 2 GeV," << endl
       << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
       << " of which " <<  nMatchedDijets << " could be matched." << endl;


  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;
  
  if ( nMix !=1 ){
    cerr << " CAREFUL: FAKING BETTER PP STATISTICS " << endl;
    cout << " CAREFUL: FAKING BETTER PP STATISTICS " << endl;
  }

  return 0;
  
}
