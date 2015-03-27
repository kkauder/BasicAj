/** @file PicoFollowAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Aj analysis and embedding prep in p+p.
    @details Perform Aj analysis in a given TStarPicoJetTree chain. Can also save events with 10 GeV jets for embedding.
    @date Mar 04, 2015
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
#include <cmath>
#include <exception>

using namespace std;
using namespace fastjet;

/** 
    - Set up input tree
    - Set up output histos and tree
    - Initialize FollowAjAnalysis object
    - Loop through events
    \arg argv: List of root files. TChain can do some globbing, so you can use
    <BR><tt>% FollowPicoAj '~putschke/Data/ppHT/&lowast;.root'</tt>
    <BR>But ultimately, it's better to let the shell handle the globbing, especially for cases like 
    <BR><tt>% FollowPicoAj ~putschke/Data/&lowast;/&lowast;.root</tt>
    <BR>For a file list, simply use <BR><tt>% cat file.list | PicoAj</tt>

*/
int main ( int argc, const char** argv ) {

  // Set up some convenient default
  // ------------------------------
  const char *defaults[4] = {"FollowPicoAj","ppFollowAj.root","ppHT","~putschke/Data/ppHT/*.root"};
  // const char *defaults[4] = {"FollowPicoAj","AuAuFollowAj.root","HT","CleanAuAu/*.root"};
  if ( argc==1 ) {
    argv=defaults;
    argc=4;
  }
  
  // Throw arguments in a vector
  // ---------------------------
  vector<string> arguments(argv + 1, argv + argc);

  // Load and set up tree
  // --------------------
  TString ChainName  = "JetTree";
  TString OutFileName = arguments.at(0);
  TString TriggerName = arguments.at(1);

  TChain* chain = new TChain( ChainName );
  for (int i=2; i<arguments.size() ; ++i ) {
    chain->Add( arguments.at(i).data() );
  }
  
  TStarJetPicoReader reader = SetupReader( chain, TriggerName );

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
  TH2D* SmallUnmatchedhPtHi = new TH2D( "SmallUnmatchedhPtHi","R=0.2, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* SmallhPtHi = new TH2D( "SmallhPtHi","R=0.2, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* SmallhPtLo = new TH2D( "SmallhPtLo","R=0.2, p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* SmallUnmatchedhdPtHi = new TH1D( "SmallUnmatchedhdPtHi","R=0.2, #Delta p_{T} for unmatched hard constituent jets", 120, -10, 50 );
  TH1D* SmallhdPtHi = new TH1D( "SmallhdPtHi","R=0.2, #Delta p_{T} for hard constituent jets", 120, -10, 50 );
  TH1D* SmallhdPtLo = new TH1D( "SmallhdPtLo","R=0.2, #Delta p_{T} for soft constituent jets", 120, -10, 50 );

  TH1D* SmallhdphiHi        = new TH1D( "SmallhdphiHi","R=0.2, #Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* SmallhdphiLo        = new TH1D( "SmallhdphiLo","R=0.2, #Delta#phi for soft constituent jets", 200, -2, 2 );

  TH1D* SmallUnmatchedAJ_hi = new TH1D( "SmallUnmatchedAJ_hi","R=0.2, Unmatched A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* SmallAJ_hi = new TH1D( "SmallAJ_hi","R=0.2, A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* SmallAJ_lo = new TH1D( "SmallAJ_lo","R=0.2, A_{J} for soft constituent jets", 40, -0.3, 0.9 );

  TH2D* LargeUnmatchedhPtHi = new TH2D( "LargeUnmatchedhPtHi","R=0.4, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* LargehPtHi = new TH2D( "LargehPtHi","R=0.4, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* LargehPtLo = new TH2D( "LargehPtLo","R=0.4, p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* LargehdphiHi = new TH1D( "LargehdphiHi","R=0.4, #Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* LargehdphiLo = new TH1D( "LargehdphiLo","R=0.4, #Delta#phi for soft constituent jets", 200, -2, 2 );

  TH1D* LargeUnmatchedhdPtHi = new TH1D( "LargeUnmatchedhdPtHi","R=0.4, #Delta p_{T} for unmatched hard constituent jets", 120, -10, 50 );
  TH1D* LargehdPtHi = new TH1D( "LargehdPtHi","R=0.4, #Delta p_{T} for hard constituent jets", 120, -10, 50 );
  TH1D* LargehdPtLo = new TH1D( "LargehdPtLo","R=0.4, #Delta p_{T} for soft constituent jets", 120, -10, 50 );
  
  TH1D* LargeUnmatchedAJ_hi = new TH1D( "LargeUnmatchedAJ_hi","R=0.4, Unmatched A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* LargeAJ_hi = new TH1D( "LargeAJ_hi","R=0.4, A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* LargeAJ_lo = new TH1D( "LargeAJ_lo","R=0.4, A_{J} for soft constituent jets", 40, -0.3, 0.9 );

  TH3D* UsedEventsHiPhiEtaPt=new TH3D("UsedEventsHiPhiEtaPt","UsedEventsHiPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.0, 10); // QA
  TH3D* UsedEventsLoPhiEtaPt=new TH3D("UsedEventsLoPhiEtaPt","UsedEventsLoPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.0, 10); // QA  

  // Save the full event and the trigger jet if this is pp and there's at least a 10 GeV jet.
  // ----------------------------------------------------------------------------------------
  // This is somewhat wasteful, we could instead read the original trees.

  bool SaveFullEvents = TriggerName.Contains("ppHT");
  TTree* TriggeredTree=0;

  TClonesArray TriggerJet( "TLorentzVector",1 ); 
  static const Int_t kmaxT=5000; // max # of particles
  TClonesArray FullEvent("TLorentzVector",kmaxT);
  if (SaveFullEvents) {
    TriggeredTree = new TTree("TriggeredTree","Triggered Events");
    // NOTE: Ignore "Warning in <TTree::Bronch>: Using split mode on a class: TLorentzVector with a custom Streamer"
    // See: https://root.cern.ch/phpBB3/viewtopic.php?p=18269
    TriggeredTree->Branch("FullEvent", &FullEvent );
    TriggeredTree->Branch("TriggerJet", &TriggerJet);
  } 
  
  
  // Initialize analysis class
  // -------------------------
  FollowAjAnalysis AjA( SmallR, LargeR,
			AjParameters::jet_ptmin, AjParameters::jet_ptmax,
			AjParameters::LeadPtMin, AjParameters::SubLeadPtMin, 
			AjParameters::max_track_rap, AjParameters::PtConsLo, AjParameters::PtConsHi,
			AjParameters::dPhiCut,
			SmallUnmatchedhPtHi,  SmallhPtHi, SmallhPtLo,
			SmallUnmatchedhdPtHi, SmallhdPtHi, SmallhdPtLo,
			SmallhdphiHi, SmallhdphiLo,
			SmallUnmatchedAJ_hi, SmallAJ_hi, SmallAJ_lo,
			LargeUnmatchedhPtHi,  LargehPtHi, LargehPtLo,
			LargeUnmatchedhdPtHi, LargehdPtHi, LargehdPtLo,  
			LargehdphiHi, LargehdphiLo,
			LargeUnmatchedAJ_hi, LargeAJ_hi, LargeAJ_lo,
			UsedEventsHiPhiEtaPt, UsedEventsLoPhiEtaPt
			);  
  
  // Cycle through events
  // --------------------
  vector<PseudoJet> particles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv; // TLorentzVector* would be sufficient. 
  
  int nHardDijets = 0;
  int nLargeToSmallHiMatched=0;
  int nLargeToSmallLoMatched=0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  
  Long64_t nEvents=-1; // -1 for all
  // Long64_t nEvents=10;
  reader.Init(nEvents);

  PseudoJet pj;
  try{
    while ( reader.NextEvent() ) {
      reader.PrintStatus(10);
      
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

      // Run analysis
      // ------------
      int ret;
      ret =AjA.AnalyzeAndFill( particles );
      
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
      }
    
      // Save the full event for embedding if there's at least one 10 GeV jet
      // --------------------------------------------------------------------
      if (SaveFullEvents){
	FullEvent.Clear();
	TriggerJet.Clear();
	if ( AjA.Has10Gev )  { 
	  for ( int i = 0; i<particles.size() ; ++i ){
	    if ( particles.at(i).pt() >100 ) { 
	      cerr << " =====> " <<particles.at(i).pt() << "  " << particles.at(i).phi() << "  " << particles.at(i).eta() << endl;  
	    } 
	    new (FullEvent[i])   TLorentzVector ( MakeTLorentzVector( particles.at(i) )  );
	  } // for particles
	  // Save trigger jet as well
	  vector<PseudoJet> JAhiResult = AjA.GetSmallJAhiResult();
	  new (TriggerJet[0]) TLorentzVector ( MakeTLorentzVector( JAhiResult.at(0) )  );
	  TriggeredTree->Fill();
	} // has Trigger
      } // SaveFullEvents
    } // while NextEvent
  } catch ( exception& e) {
    cerr << "Caught " << e.what() << endl;
    return -1;
  }
  
  cout << "##################################################################" << endl;
  
  Long64_t nEventsUsed=reader.GetNOfEvents();  
  UsedEventsHiPhiEtaPt->Scale( 1./nEventsUsed );

  // Close up shop
  // -------------
  fout->Write();

  cout << "In " << nEventsUsed << " events, found " << endl
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
