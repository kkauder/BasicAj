/** @file RandomCone.cxx
    @author Kolja Kauder
    @version Revision 0.2
    @brief Embed Au+Au di-jet pairs with a high-pT cut into random circular regions in MB.
    NO jet-finding here! Using fastjet for background subtraction
    @date Apr 28, 2015
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

/** 
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

  // How many times to use every hard di-jet
  // THIS SHOULD MAYBE BE ONE IN THE FINAL VERSION
  // ----------------------------------------------
  Int_t nMix=6;
  if ( nMix !=1 ){
    cerr << " CAREFUL: FAKING BETTER STATISTICS " << endl;
    cout << " CAREFUL: FAKING BETTER STATISTICS " << endl;
  }
  
  const char *defaults[5] = {"RandomCone","AjResults/Fresh_NicksList_HC100_RandomCone.root","AjResults/Fresh_NicksList_HC100_AuAu.root","MB","Data/NewPicoDst_AuAuCentralMB/*.root"};
  if ( argc==1 ) {
    argv=defaults;
    argc=5;
  }
  // Throw arguments in a vector
  // ---------------------------
  vector<string> arguments(argv + 1, argv + argc);

  // Load high cut jets
  // ------------------
  TString OrigAuAuName = arguments.at(1);
    
  TChain* OrigJets = new TChain("ResultTree");
  OrigJets->Add(OrigAuAuName);
  assert ( OrigJets->GetEntries()>0 && "Something went wrong loading the Au+Au jets.");
  
  TLorentzVector *pJ1 = new TLorentzVector();
  OrigJets->SetBranchAddress("j1", &pJ1);
  TLorentzVector *pJ2 = new TLorentzVector();
  OrigJets->SetBranchAddress("j2", &pJ2);
    
  // Throw them into convenient vectors
  // ----------------------------------
  vector< vector<PseudoJet> > vOrigJets;
  vector<PseudoJet> Current;

  for ( Long64_t OrigEv = 0; OrigEv< OrigJets->GetEntries() ; ++ OrigEv ){
    OrigJets->GetEntry(OrigEv);
    Current.clear();
    
    Current.push_back( MakePseudoJet( pJ1 ) );
    Current.push_back( MakePseudoJet( pJ2 ) );
    vOrigJets.push_back ( Current );
  }

  // Load and set up AuAu tree
  // --------------------
  TString ChainName  = "JetTree";
  TString TriggerName = arguments.at(2);
  TChain* chain = new TChain( ChainName );
  for (int i=3; i<arguments.size() ; ++i ) {
    chain->Add( arguments.at(i).data() );
  }

  int RefMultCut=0;
  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );
  reader.SetApplyFractionHadronicCorrection(kTRUE);
  reader.SetFractionHadronicCorrection(0.9999);
  reader.SetRejectTowerElectrons( kFALSE );  

  // Explicitly choose bad tower list here
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  if ( OrigAuAuName.Contains("NPE") || OrigAuAuName.Contains("AuAu11") ){
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/badTowerList_y11.txt");
  // Add the following to y11 as well, once we're embedding!
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
  } else {
    // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/OrigY7MBBadTowers.txt");
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_AuAu_Nick.txt");
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
  }
  // DEBUG ONLY
  // towerCuts->AddBadTowers( "emptyBadTowerList.txt");

  // // DEBUG: KK: Reject bad phi strip  
  // towerCuts->SetPhiCut(0, -1.2);
  // TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  // trackCuts->SetPhiCut(0, -1.2);

  // TStarJetPicoDefinitions::SetDebugLevel(10);

  // Files and histograms
  // --------------------
  TString OutFileName = arguments.at(0);

  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  TH2D* OrigAJ_hi = new TH2D( "OrigAJ_hi","Original A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* RandomAJ_lo = new TH2D( "RandomAJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );

  TH1D* DebugRhoDiff = new TH1D("DebugRhoDiff", "Difference in median pT when excluding two highest jets", 100, -10, 10 );
  
  // Helpers
  // -------
  vector<PseudoJet> particles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TLorentzVector* lv;
  
  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  
  // For Random choosing
  // -------------------
  gRandom->SetSeed(1);
  fastjet::GhostedAreaSpec TmpArea; // for access to static random seed
  vector<int> SeedStatus;

  Long64_t nEvents=-1; // -1 for all
  // Long64_t nEvents=99999;
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
  
  // Now every original jet gets assigned nMix many events to be embedded in
  // This way we can cycle through the potentially large AuAu chain sequentially
  // ---------------------------------------------------------------------------
  cout << "Initializing mixing pools." << endl;
  Int_t random;
  int emergencystop=nMix*100;
  vector< set<Int_t> > MixingPool;
  for ( int i =0; i< vOrigJets.size() ; ++i ){
    set<Int_t> EventNos;
    int tries=0;
    
    while ( EventNos.size() < nMix ){
      if ( tries++ > emergencystop ) { cerr << "Stuck in endless loop - aborting." << endl; return -1;}
      random = gRandom->Integer (AcceptedEvents.size() );
      EventNos.insert(AcceptedEvents.at(random) );	 
    }
    MixingPool.push_back( EventNos );
  }

  cout << MixingPool.size() << endl;
  

  // Second round - actual analysis
  // ------------------------------
  cout << "Second round - embed" << endl;  

  // Jet finding input
  // -----------------
  float R = 0.4;
  float PtConsLo=0.2;
  if ( OutFileName.Contains ("R0.2") ){
    R=0.2;
  }
  if ( OutFileName.Contains ("Pt1") ){
    PtConsLo=1.0;
  }
  float max_track_rap = AjParameters::max_track_rap;
  float max_rap = max_track_rap-R;
  float ghost_maxrap = max_rap + 2.0 * R;


  fastjet::JetDefinition jet_def = JetDefinition(fastjet::antikt_algorithm, R);
  fastjet::GhostedAreaSpec area_spec( ghost_maxrap, AjParameters::ghost_repeat, AjParameters::ghost_area );
  fastjet::AreaDefinition  area_def( fastjet::active_area, area_spec);

  // Constituent selectors
  // ---------------------
  fastjet::Selector select_track_rap = fastjet::SelectorAbsRapMax(max_track_rap);
  fastjet::Selector select_lopt      = fastjet::SelectorPtMin( PtConsLo );
  fastjet::Selector slo = select_track_rap * select_lopt;
  
  // Background selector -
  // keep in mind we could reject the (two) highest jet(s)
  // fastjet::Selector selector_bkgd=fastjet::SelectorAbsRapMax( 0.6 ) * (!fastjet::SelectorNHardest(2));
  fastjet::Selector selector_bkgd=fastjet::SelectorAbsRapMax( 0.6 );    

  // Reset the reader
  // ----------------
  reader.ResetEventCounters();
  reader.Init(nEvents);
  
  Long64_t nJetsUsed=0;
  vector<PseudoJet> AuAuparticles;

  double refmult; // Really an int, but may change if using refmultcorr
  int eventid;
  int runid;

  fastjet::Selector GrabCone = fastjet::SelectorCircle( R );
  while ( reader.NextEvent() ) {
    reader.PrintStatus(10);      
    
    // Load event
    // ----------
    container = reader.GetOutputContainer();

    // event info
    // ----------
    TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();
    //refmult=header->GetGReferenceMultiplicity();
    refmult=header->GetProperReferenceMultiplicity();
    eventid = header->GetEventId();
    runid   = header->GetRunId();

    // NEW 05/07/15: For repeatability across different picoDSTs, set random seed
    // Static member, so we can set it here
    // Annoyingly, the getter and setter isn't static, so we need to instantiate
    // Apparently, the seed is always an int[2], so it's natural to seed it with runid and eventid      
    TmpArea.get_random_status(SeedStatus);
    if ( SeedStatus.size() !=2 ) {
      throw std::string("SeedStatus.size() !=2");
      return -1;
    } 
    SeedStatus.at(0) = runid;
    SeedStatus.at(1) = eventid;
    TmpArea.set_random_status(SeedStatus);

    // Make AuAu vector
    // ----------------
    AuAuparticles.clear();
    for (int ip = 0; ip<container->GetEntries() ; ++ip ){
      lv = container->Get(ip);
      AuAuparticles.push_back ( MakePseudoJet( lv ) );
    }

    // Set up jet finder
    // -----------------
    std::vector<fastjet::PseudoJet> pLo = slo( AuAuparticles );
    JetAnalyzer JAlo( pLo, jet_def, area_def, selector_bkgd );
    // Need to create subtractor to have bkg estimator
    fastjet::Subtractor* BackgroundSubtractor =  JAlo.GetBackgroundSubtractor();

    // // DEBUG
    // JetAnalyzer JAtest( pLo, jet_def, area_def, selector_bkgd * (!fastjet::SelectorNHardest(2)) );
    // // Need to create subractor to have bkg estimator
    // fastjet::Subtractor* TestBackgroundSubtractor =  JAtest.GetBackgroundSubtractor();
    // // cout << JAlo.GetBackgroundEstimator()->rho() << " -  " 
    // // 	 << JAtest.GetBackgroundEstimator()->rho() << " = " 
    // // 	 << JAlo.GetBackgroundEstimator()->rho() - JAtest.GetBackgroundEstimator()->rho() << endl;

    // DebugRhoDiff->Fill ( JAlo.GetBackgroundEstimator()->rho() - JAtest.GetBackgroundEstimator()->rho() );

    // Now fill up original jets with circular background
    // --------------------------------------------------
    // We can either choose them at random or use the original jet phi-eta as center
    // Choosing the latter for now

    // Find jets that want this event
    // ------------------------------
    vector <int> JetIndices;
    for ( int i =0; i< MixingPool.size() ; ++i ){
      set<Int_t>& EventNos = MixingPool.at(i);
      if ( EventNos.find( reader.GetNOfCurrentEvent() ) != EventNos.end() ) {
    	JetIndices.push_back( i );
        }
    }

    // And run analysis individually for each of these
    // -----------------------------------------------
    for ( vector<int>::iterator jit=JetIndices.begin(); jit!=JetIndices.end(); ++ jit ){
      PseudoJet& j1 = vOrigJets.at(*jit).at(0);
      GrabCone.set_reference( j1);
      vector<PseudoJet> C1 = GrabCone( pLo );
      PseudoJet RC1 (0,0,0,0);
      for (vector<PseudoJet>::iterator it = C1.begin() ; it!= C1.end() ; ++ it ) RC1+= *it;
      
      PseudoJet& j2 = vOrigJets.at(*jit).at(1);
      GrabCone.set_reference( j2);
      vector<PseudoJet> C2 = GrabCone( pLo );
      PseudoJet RC2 (0,0,0,0);
      for (vector<PseudoJet>::iterator it = C2.begin() ; it!= C2.end() ; ++ it ) RC2+= *it;

      // cout << RC1.pt() << "  "
      // 	   << RC1.pt() - TMath::Pi()*R*R * JAlo.GetBackgroundEstimator()->rho() << "  "
      // 	   << RC1.pt() - TMath::Pi()*R*R * JAtest.GetBackgroundEstimator()->rho() << endl;
      // cout << " ============================== " << endl;

      // Sanity check - should reproduce AJ_hi with fake statistics
      OrigAJ_hi->Fill( fabs ( (j1.pt() - j2.pt())  / (j1.pt() + j2.pt()) ), refmult );
      
      float pt1 = j1.pt() + RC1.pt() - TMath::Pi()*R*R * JAlo.GetBackgroundEstimator()->rho() ;
      float pt2 = j2.pt() + RC2.pt() - TMath::Pi()*R*R * JAlo.GetBackgroundEstimator()->rho() ;

      RandomAJ_lo->Fill( fabs ( (pt1 - pt2) / (pt1 + pt2) ), refmult );            
    }
            

  } // reader.NextEvent()
  cout << "##################################################################" << endl;

  // Close up shop
  // -------------
  fout->Write();

  // cout << "Embedded " << nJetsUsed << " jets above 10 GeV in " << reader.GetNOfEvents() << " events and found " << endl
  //      << nHardDijets << " dijets with constituents above 2 GeV," << endl
  //      << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
  //      << " of which " <<  nMatchedDijets << " could be matched." << endl;


  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;
  
  if ( nMix !=1 ){
    cerr << " CAREFUL: FAKING BETTER STATISTICS " << endl;
    cout << " CAREFUL: FAKING BETTER STATISTICS " << endl;
  }

  return 0;
  
}
