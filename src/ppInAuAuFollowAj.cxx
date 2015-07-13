/** @file ppInAuAuFollowAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Variation of Aj analysis on pp jets embedded into AuAu.
    @date Mar 30, 2015
*/

#include "FollowAjAnalysis.hh"
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

  // Set up some convenient default
  // ------------------------------
  const char *defaults[5] = {"ppInAuAuFollowAj","AjResults/ppInAuAuFollowAj.root","AjResults/ppFollowAj.root","-1", "-1" };
  if ( argc==1 ) {
    argv=defaults;
    argc=5;
  }
  
  
  // Throw arguments in a vector
  // ---------------------------
  vector<string> arguments(argv + 1, argv + argc);

  // Output file
  // -----------
  TString OutFileName = arguments.at(0);

  // soft constituent cut
  // --------------------
  float PtConsLo=0.2;
  if ( OutFileName.Contains ("Pt1") ){
    PtConsLo=1.0;
  }

  // pp File
  // -------
  TString ppAjName = arguments.at(1);
    
  // trigger cut
  // -----------
  float LeadPtMin=AjParameters::LeadPtMin;
  float SubLeadPtMin=AjParameters::SubLeadPtMin;
  if ( atof(arguments.at(2).c_str()) > 0 ) LeadPtMin    = atof( (arguments.at(2).c_str()) );
  if ( atof(arguments.at(3).c_str()) > 0 ) SubLeadPtMin = atof( (arguments.at(3).c_str()) );

  cout << " Leading Jets above     " <<    LeadPtMin << " GeV/c" << endl;
  cout << " Sub-Leading Jets above " << SubLeadPtMin << " GeV/c" << endl;

  
  // How many times to use every pp jet
  // ----------------------------------
  Int_t nMix=3;

  // Load pp Jets
  // ------------
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
  TString TriggerName = "MB";
  TString InFileName = "Data/AuAuMB_0_20/*.root";

  TChain* chain = new TChain( ChainName );
  chain->Add( InFileName );
  
  double RefMultCut = 0;
  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );

  // Files and histograms
  // --------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  assert ( fout->IsOpen() );
  cout << " ################################################### " << endl;
  cout << "Writing to: " << fout->GetName() << endl;
  cout << " ################################################### " << endl;
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  float SmallR=0.2; // DEBUG
  float LargeR=0.4;

  TH1D* LeadDeltaPtHi    = new TH1D( "LeadDeltaPtHi",   "Matched, hard constituents, p_T^{Lead} difference between small and large R", 200, -30, 10 );
  TH1D* SubLeadDeltaPtHi = new TH1D( "SubLeadDeltaPtHi","Matched, hard constituents, p_T^{Sub} difference between small and large R", 200, -30, 10 );
  TH1D* LeadDeltaPtLo    = new TH1D( "LeadDeltaPtLo",   "Matched, soft constituents, p_T^{Lead} difference between small and large R", 200, -30, 10 );
  TH1D* SubLeadDeltaPtLo = new TH1D( "SubLeadDeltaPtLo","Matched, soft constituents, p_T^{Sub} difference between small and large R", 200, -30, 10 );

  TH2D* SmallUnmatchedhPtHi = new TH2D( "SmallUnmatchedhPtHi","R=0.2, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* SmallhPtHi = new TH2D( "SmallhPtHi","R=0.2, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* SmallhPtLo = new TH2D( "SmallhPtLo","R=0.2, p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );

  TH1D* SmallhdphiHi        = new TH1D( "SmallhdphiHi","R=0.2, #Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* SmallhdphiLo        = new TH1D( "SmallhdphiLo","R=0.2, #Delta#phi for soft constituent jets", 200, -2, 2 );

  TH2D* SmallUnmatchedAJ_hi = new TH2D( "SmallUnmatchedAJ_hi","R=0.2, Unmatched A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5  );
  TH2D* SmallAJ_hi = new TH2D( "SmallAJ_hi","R=0.2, A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5  );
  TH2D* SmallAJ_lo = new TH2D( "SmallAJ_lo","R=0.2, A_{J} for soft constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5  );

  TH2D* LargehPtHi = new TH2D( "LargehPtHi","R=0.4, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* LargehPtLo = new TH2D( "LargehPtLo","R=0.4, p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* LargehdphiHi = new TH1D( "LargehdphiHi","R=0.4, #Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* LargehdphiLo = new TH1D( "LargehdphiLo","R=0.4, #Delta#phi for soft constituent jets", 200, -2, 2 );

  TH2D* LargeAJ_hi = new TH2D( "LargeAJ_hi","R=0.4, A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5  );
  TH2D* LargeAJ_lo = new TH2D( "LargeAJ_lo","R=0.4, A_{J} for soft constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5  );

  TH2D* SmallDeltaAJ_hilo = new TH2D( "SmallDeltaAJ_hilo","Small R, A_J (hard) - A_J (soft);#Delta A_{J};Refmult;fraction", 100, -1, 1, 800, -0.5, 799.5 );
  TH2D* LargeDeltaAJ_hilo = new TH2D( "LargeDeltaAJ_hilo","Large R, A_J (hard) - A_J (soft);#Delta A_{J};Refmult;fraction", 100, -1, 1, 800, -0.5, 799.5 );
  TH2D* DeltaAJ_hi = new TH2D( "DeltaAJ_hi","Matched, hard constituents, A_J (Small R) - A_J (Large R);#Delta A_{J};Refmult;fraction", 100, -1, 1, 800, -0.5, 799.5 );
  TH2D* DeltaAJ_lo = new TH2D( "DeltaAJ_lo","Matched, soft constituents, A_J (Small R) - A_J (Large R);#Delta A_{J};Refmult;fraction", 100, -1, 1, 800, -0.5, 799.5 );
  
  // Helpers
  // -------
  vector<PseudoJet> particles;
  vector<PseudoJet> AuAuparticles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TLorentzVector* lv;
  
  int nHardDijets = 0;
  int nLargeToSmallHiMatched=0;
  int nLargeToSmallLoMatched=0;
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
  FollowAjAnalysis AjA( SmallR, LargeR,
			AjParameters::jet_ptmin, AjParameters::jet_ptmax,
			LeadPtMin, SubLeadPtMin, 
			AjParameters::max_track_rap,
			PtConsLo, AjParameters::PtConsHi,
			AjParameters::dPhiCut
			);  
  
  // Reset the reader
  // ----------------
  reader.ResetEventCounters();
  reader.Init(nEvents);
  
  double refmult; // Really an int, but may change if using refmultcorr
  Long64_t nJetsUsed=0;
  while ( reader.NextEvent() ) {
    reader.PrintStatus(10);      

    // event info
    // ----------
    TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();
    refmult=0;
    refmult=header->GetGReferenceMultiplicity();    
    
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

      // ret =AjA.AnalyzeAndFill( particles, &(vTriggerJet.at(*jit)),, refmult,
      ret =AjA.AnalyzeAndFill( particles, 0, refmult,
			       LeadDeltaPtHi, SubLeadDeltaPtHi, LeadDeltaPtLo, SubLeadDeltaPtLo,
			       SmallUnmatchedhPtHi,  SmallhPtHi, SmallhPtLo,
			       SmallhdphiHi, SmallhdphiLo,
			       SmallUnmatchedAJ_hi, SmallAJ_hi, SmallAJ_lo,
			       LargehPtHi, LargehPtLo,
			       LargehdphiHi, LargehdphiLo,
			       LargeAJ_hi, LargeAJ_lo,
			       SmallDeltaAJ_hilo,  LargeDeltaAJ_hilo,
			       DeltaAJ_hi,  DeltaAJ_lo
			       );

      // Intercept return value for radius matching
      if ( ret == 101 || ret == 102 ){
	nLargeToSmallHiMatched++;
	ret-=100;
      }
      if ( ret == 103 ){
	nLargeToSmallHiMatched++;
	nLargeToSmallLoMatched++;
	ret-=100;
      }

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
       << nHardDijets << " dijets with constituents above 2 GeV and R=" << SmallR << "," << endl
       << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
       << " of which " <<  nMatchedDijets << " could be matched." << endl;
  
  cout << "The " << nHardDijets << " dijets with hard constituents were matched to " 
       << nLargeToSmallHiMatched << " dijets with R=" << LargeR << "," << endl
       << "and the " << nMatchedDijets << " matched corresponding soft constituent dijets " 
       << " were matched to " <<  nLargeToSmallLoMatched << " larger radius ones." << endl;

  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;
  
  return 0;
  
}
