/** @file PicoAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Aj analysis and embedding prep in p+p.
    @details Perform Aj analysis in a given TStarPicoJetTree chain. Can also save events with 10 GeV jets for embedding.
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
#include <cmath>
#include <exception>

using namespace std;
using namespace fastjet;

/** 
    - Set up input tree
    - Set up output histos and tree
    - Initialize AjAnalysis object
    - Loop through events
    \arg argv: List of root files. TChain can do some globbing, so you can use
    <BR><tt>% PicoAj '~putschke/Data/ppHT/&lowast;.root'</tt>
    <BR>But ultimately, it's better to let the shell handle the globbing, especially for cases like 
    <BR><tt>% PicoAj ~putschke/Data/&lowast;/&lowast;.root</tt>
    <BR>For a file list, simply use <BR><tt>% cat file.list | PicoAj</tt>

*/
int main ( int argc, const char** argv ) {

  // Set up some convenient default
  // ------------------------------
  // const char *defaults[4] = {"PicoAj","ppAj.root","ppHT","~putschke/Data/ppHT/*.root"};
  // const char *defaults[4] = {"PicoAj","AuAuAj.root","HT","CleanAuAu/Clean809.root"};
  const char *defaults[4] = {"PicoAj","test.root","HT","/Users/putschke/Data/Pico_Eflow/auau_ht_pico_20_R_0.4_ptcut_2.0_bin_36.root"};
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
  bool isAuAu=false;
  for (int i=2; i<arguments.size() ; ++i ) {
    chain->Add( arguments.at(i).data() );
    if (arguments.at(i).find("AuAu") != std::string::npos ) isAuAu=true; // Quick and dirty...
  }
  

  double RefMultCut = 0;
  if ( isAuAu ) RefMultCut=AjParameters::AuAuRefMultCut;
  // WARNING: ~putschke/Data/Pico_Eflow/auau_ht* is cut off at 351!
  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );
  TStarJetPicoDefinitions::SetDebugLevel(0);
    
  // Files and histograms
  // --------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  assert ( fout->IsOpen() );
  cout << " ################################################### " << endl;
  cout << "Writing to: " << fout->GetName() << endl;
  cout << " ################################################### " << endl;
  
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

  // Save results
  // ------------
  TTree* ResultTree=new TTree("ResultTree","Result Jets");
  TLorentzVector j1, j2, jm1, jm2;
  ResultTree->Branch("j1",&j1);
  ResultTree->Branch("j2",&j2);
  ResultTree->Branch("jm1",&jm1);
  ResultTree->Branch("jm2",&jm2);
  int eventid;
  int runid;
  ResultTree->Branch("eventid",&eventid, "eventid/i");
  ResultTree->Branch("runid",&runid, "runid/i");

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
    TriggeredTree->Branch("eventid",&eventid, "eventid/i");
    TriggeredTree->Branch("runid",&runid, "runid/i");

  } 
  

  
	      
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
  
  // Cycle through events
  // --------------------
  vector<PseudoJet> particles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv; // TLorentzVector* would be sufficient. 
  
  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  
  Long64_t nEvents=-1; // -1 for all
  // Long64_t nEvents=10;
  reader.Init(nEvents);

  PseudoJet pj;
  try{
    while ( reader.NextEvent() ) {
      reader.PrintStatus(10);
      TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();
      
      eventid = header->GetEventId();
      runid   = header->GetRunId();

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
      // std::cerr << ret << endl;
    
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
      // ------------------------------------------------------------------
      if (SaveFullEvents){
	FullEvent.Clear();
	TriggerJet.Clear();
	if ( AjA.Has10Gev )  { 
	  for ( int i = 0; i<particles.size() ; ++i ){
	    if ( particles.at(i).pt() >50 ) { 
	      cerr << " =====> " <<particles.at(i).pt() << "  " << particles.at(i).phi() << "  " << particles.at(i).eta() << endl;  
	    } 
	    new (FullEvent[i])   TLorentzVector ( MakeTLorentzVector( particles.at(i) )  );
	  } // for particles
	  // Save trigger jet as well
	  vector<PseudoJet> JAhiResult = AjA.GetJAhiResult();
	  new (TriggerJet[0]) TLorentzVector ( MakeTLorentzVector( JAhiResult.at(0) )  );
	  TriggeredTree->Fill();
	} // has Trigger
      } // SaveFullEvents

      // Save results
      vector<PseudoJet> DiJetsHi = AjA.GetDiJetsHi();
      vector<PseudoJet> DiJetsLo = AjA.GetDiJetsLo();

      // Check for matching? yes, for now.
      // cout << DiJetsHi.size() << "  " << DiJetsLo.size() << endl;
      // cout << " going in"  << endl;

      // cout << reader.GetNOfCurrentEvent() << endl;
      if ( reader.GetNOfCurrentEvent() == 43 && false) {
	cout << "Jet 1:" << endl;
	cout << "pt = " << DiJetsHi.at(0).pt() << endl;
	cout << "eta = " << DiJetsHi.at(0).eta() << endl;
	cout << "phi = " << DiJetsHi.at(0).phi() << endl;
	// cout << "area = " << DiJetsHi.at(0).area() << endl;
	cout << "Jet 2:" << endl;
	cout << "pt = " << DiJetsHi.at(1).pt() << endl;
	cout << "eta = " << DiJetsHi.at(1).eta() << endl;
	cout << "phi = " << DiJetsHi.at(1).phi() << endl;
	// cout << "area = " << DiJetsHi.at(1).area() << endl;
	cout << endl;
	cout << "|dPhi-pi|=" << fabs(DiJetsHi.at(0).phi() - DiJetsHi.at(1).phi()-TMath::Pi()) << endl;
	cout << "|dPhi+pi|=" << fabs(DiJetsHi.at(0).phi() - DiJetsHi.at(1).phi()+TMath::Pi()) << endl;
	
	// for (int i=0; i< container->GetEntries() ; ++i ){
	//   if ( container->Get(i)->Pt()>2 )
	//     container->Get(i)->Print();
	// }
	// return -1;
      }
      
      if ( DiJetsLo.size()==2 ){
	// cout << "Event # = "<< reader.GetNOfCurrentEvent() << endl;
	// cout << "Jet 1:" << endl;
	// cout << "pt = " << DiJetsHi.at(0).pt() << endl;
	// cout << "eta = " << DiJetsHi.at(0).eta() << endl;
	// cout << "phi = " << DiJetsHi.at(0).phi() << endl;
	// // cout << "area = " << DiJetsHi.at(0).area() << endl;
	// cout << "Jet 2:" << endl;
	// cout << "pt = " << DiJetsHi.at(1).pt() << endl;
	// cout << "eta = " << DiJetsHi.at(1).eta() << endl;
	// cout << "phi = " << DiJetsHi.at(1).phi() << endl;
	// // cout << "area = " << DiJetsHi.at(1).area() << endl;
	// cout << endl;
	// if ( fabs( DiJetsHi.at(0).pt() - 21.785988 )< 0.00001 ) { cout << reader.GetNOfCurrentEvent() << endl; return -1;}
  
	// cout << DiJetsHi.size() << "  " << DiJetsLo.size() << endl;
	j1 = MakeTLorentzVector( DiJetsHi.at(0) );
	j2 = MakeTLorentzVector( DiJetsHi.at(1) );
	jm1 = MakeTLorentzVector( DiJetsLo.at(0) );
	jm2 = MakeTLorentzVector( DiJetsLo.at(1) );
	ResultTree->Fill();
      }
      // cout << " Coming out"  << endl;
	
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
       << nHardDijets << " dijets with constituents above 2 GeV," << endl
       << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
       << " of which " <<  nMatchedDijets << " could be matched." << endl;


  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;

  // cout << "Aj->GetEntries() = " << AJ_hi->GetEntries() << endl;
  // cout << "Unmatched GetEntries() = " << UnmatchedAJ_hi->GetEntries() << endl;
  
  cout << "Aj->GetEntries() = " << UnmatchedAJ_hi->GetEntries() << endl;
  return 0;
  
}
