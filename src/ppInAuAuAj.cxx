/** @file ppInAuAuAj.cxx
    @author Kolja Kauder
    @version Revision 0.2
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
  
  // const char *defaults[5] = {"ppInAuAuAj","ppInAuAuAjTest.root","AjResults/Tow0_Eff0_ppAj.root","MB","Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root"};
  const char *defaults[5] = {"ppInAuAuAj","ppInAuAuAjTest.root","AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppAj.root","MB","Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB_8177020_DC4BA348C050D5562E7461357C4B341D_0.root"};
  if ( argc==1 ) {
    argv=defaults;
    argc=5;
  }

  float RemoveSoftFromAway = 0.75;

  // Throw arguments in a vector
  // ---------------------------
  vector<string> arguments(argv + 1, argv + argc);

  // Output
  // ------
  TString OutFileName = arguments.at(0);

  // Load pp Jets
  // ------------
  TString ppAjName = arguments.at(1);

  // jet resolution parameter
  // ------------------------
  float R = 0.4;
  // Follow to different R
  float OtherR = 0.2;   // will be set to 0.4 if we trigger on 0.2, i.e., we can follow in either direction

  if ( OutFileName.Contains ("R0.2") ){
    if ( !ppAjName.Contains ("R0.2") ) {
      cerr << "OutFileName and ppAjName not compatible" << endl;
      return -1;
    }
    R=0.2;
    OtherR=0.4;    
  }

  float PtConsLo=0.2;
  if ( OutFileName.Contains ("Pt1") ){
    if ( !ppAjName.Contains ("Pt1") ) {
      cerr << "OutFileName and ppAjName not compatible" << endl;
      return -1;
    }
    PtConsLo=1.0;
  }
  

  // Jet Pt cuts
  // -----------
  float LeadPtMin = AjParameters::LeadPtMin;
  float SubLeadPtMin = AjParameters::SubLeadPtMin;
  // if ( OutFileName.Contains ("R0.2") ){
  //   LeadPtMin=16.0;
  //   SubLeadPtMin=8.0;
  // }

  cout << " ################################################### " << endl;
  cout << "Triggering with R=" << R << endl;
  cout << "Following to R=" << OtherR << endl;
  cout << "Low pT cut =" << PtConsLo << endl;
  cout << " ################################################### " << endl;


  // Refmult Cut
  // -----------
  // int RefMultCut=AjParameters::AuAuRefMultCut;
  int RefMultCut=0;

  // How many times to use every pp jet
  // THIS SHOULD MAYBE BE ONE IN THE FINAL VERSION
  // ----------------------------------------------
  Int_t nMix=6;
  if ( OutFileName.Contains ("MixTest") ){
    nMix=1;
    RefMultCut=269;
  }

  if ( nMix !=1 ){
    cerr << " CAREFUL: FAKING BETTER PP STATISTICS " << endl;
    cout << " CAREFUL: FAKING BETTER PP STATISTICS " << endl;
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

  TClonesArray* pAwayJet = new TClonesArray("TLorentzVector");
  ppJets->GetBranch("AwayJet")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("AwayJet", &pAwayJet);

  // Throw them into convenient vectors
  // ----------------------------------
  vector< vector<PseudoJet> > FullPpEvent;
  vector<PseudoJet> vTriggerJet;
  vector<PseudoJet> vAwayJet;
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

    // if ( ((TLorentzVector*) pTriggerJet->At(0))->Pt() > 20 ){
    //   cout << ((TLorentzVector*) pAwayJet->At(0))->Pt() << endl;
    // }
    vAwayJet.push_back( MakePseudoJet( (TLorentzVector*) pAwayJet->At(0) ) );
  }

  // Load and set up AuAu tree
  // --------------------
  TString ChainName  = "JetTree";
  TString TriggerName = arguments.at(2);
  TChain* chain = new TChain( ChainName );
  TString InPattern=arguments.at(3);
  chain->Add( InPattern );
  // for (int i=3; i<arguments.size() ; ++i ) {
  //   chain->Add( arguments.at(i).data() );
  // }

  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );
  reader.SetApplyFractionHadronicCorrection(kTRUE);
  reader.SetFractionHadronicCorrection(0.9999);
  reader.SetRejectTowerElectrons( kFALSE );
  //   reader.SetApplyFractionHadronicCorrection(kFALSE);

  // Explicitly choose bad tower list here
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  if ( InPattern.Contains("NPE") ){
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/badTowerList_y11.txt");
  } else {
    // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/OrigY7MBBadTowers.txt");
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_AuAu_Nick.txt");
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
    // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
  }
  // Add the following to y11 as well, once we're embedding!
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");

  
  TStarJetPicoDefinitions::SetDebugLevel(0);

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

  TH2D* hdPtLead = new TH2D( "hdPtLead","#Delta p_{T} soft-hard, leading jets;#Delta p_{T};Refmult", 120, -20, 40, 800, -0.5, 799.5 );
  TH2D* hdPtSubLead = new TH2D( "hdPtSubLead","#Delta p_{T} soft-hard, subleading jets;#Delta p_{T};Refmult", 120, -20, 40, 800, -0.5, 799.5 );
  TH2D* SpecialhdPtLead = new TH2D( "SpecialhdPtLead","#Delta p_{T} soft-hard, leading jets, |A_{J}|>0.25 ;#Delta p_{T};Refmult", 120, -20, 40, 800, -0.5, 799.5 );
  TH2D* SpecialhdPtSubLead = new TH2D( "SpecialhdPtSubLead","#Delta p_{T} soft-hard, subleading jets, |A_{J}|>0.25;#Delta p_{T};Refmult", 120, -20, 40, 800, -0.5, 799.5 );

  TH1D* hdphiHi = new TH1D( "hdphiHi","#Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* hdphiLo = new TH1D( "hdphiLo","#Delta#phi for soft constituent jets", 200, -2, 2 );

  TH1D* hJetHadronHiLead = new TH1D( "hJetHadronHiLead","Jet-h for leading hard constituent jets; #Delta#phi", 200, -2, 2 );
  TH1D* hJetHadronHiSubLead = new TH1D( "hJetHadronHiSublead","Jet-h for sub-leading hard constituent jets; #Delta#phi", 200, -2, 2 );
  TH1D* hJetHadronLoLead = new TH1D( "hJetHadronLoLead","Jet-h for leading soft constituent jets; #Delta#phi", 200, -2, 2 );
  TH1D* hJetHadronLoSubLead = new TH1D( "hJetHadronLoSublead","Jet-h for sub-leading soft constituent jets; #Delta#phi", 200, -2, 2 );

  // Follow down to other R
  TH2D* OtherAJ_lo = new TH2D( "OtherAJ_lo","A_{J} for soft constituent jets with other R ;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* OtherLeadPtLoss_lo    = new TH2D( "OtherLeadPtLoss_lo","Leading #Delta p_{T} for soft constituent jets with other R ;A_{J};Refmult;fraction", 120, -10, 50, 800, -0.5, 799.5 );
  TH2D* OtherSubLeadPtLoss_lo = new TH2D( "OtherSubLeadPtLoss_lo","SubLeading #Delta p_{T} for soft constituent jets with other R ;A_{J};Refmult;fraction", 120, -10, 50, 800, -0.5, 799.5 );

  // DEBUG
  TH1D* MBTracks = new TH1D("MBTracks","MB tracks, 0-20%; p_{T}", 200, 0, 50);
  TH1D* MBTowers = new TH1D("MBTowers","MB towers, 0-20%; E_{T}", 200, 0, 50);
  TH1D* RemovedSoft = new TH1D( "RemovedSoft", "", 100, 0, 10 );

  //Scale by AJ_hi->Integral() for number of BG constituents per di-jet
  TH1D* CapturedHiAuAu = new TH1D("CapturedHiAuAu","AuAu constituents above 2 GeV in the dijets; p_{T}", 300, 0, 30);

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
  double refmult; // Really an int, but may change if using refmultcorr
  ResultTree->Branch("eventid",&eventid, "eventid/i");
  ResultTree->Branch("runid",&runid, "runid/i");
  ResultTree->Branch("refmult",&refmult, "refmult/d");

  float rho;
  float rhoerr;

  ResultTree->Branch("rho",&rho, "rho/f");
  ResultTree->Branch("rhoerr",&rhoerr, "rhoerr/f");


  
  // Helpers
  // -------
  vector<PseudoJet> particles;
  vector<PseudoJet> AuAuparticles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv;
  // TLorentzVector* lv;
  
  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  
  // For pp jet selection
  // --------------------
  gRandom->SetSeed(1);
  fastjet::GhostedAreaSpec TmpArea; // for access to static random seed
  vector<int> SeedStatus;

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
  AjAnalysis AjA( R,
		  AjParameters::jet_ptmin, AjParameters::jet_ptmax,
		  LeadPtMin, SubLeadPtMin, 
		  // AjParameters::LeadPtMin, AjParameters::SubLeadPtMin, 
		  AjParameters::max_track_rap,
		  PtConsLo,
		  AjParameters::PtConsHi,
		  AjParameters::dPhiCut
		  );  

  // Reset the reader
  // ----------------
  reader.ResetEventCounters();
  reader.Init(nEvents);
  
  Long64_t nJetsUsed=0;
  PseudoJet pj;
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
    // Using Au+Au ids
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
      sv = container->Get(ip);
      pj = MakePseudoJet( sv );
      pj.set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge(), "auau" ) );
      AuAuparticles.push_back ( pj );

      // DEBUG
      if ( fabs(sv->GetCharge())>1e-4 ) {
	MBTracks->Fill(sv->Pt());
      } else {
	MBTowers->Fill(sv->Et());
      }
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
      // particles = AuAuparticles;
      // for ( int i=0; i < FullPpEvent.at(*jit).size() ; ++i ){
      // 	particles.push_back( FullPpEvent.at(*jit).at(i) );
      // }

      // Two steps for cross check - sort by pT first and remove some soft stuff
      vector<PseudoJet> tmp;
      for ( int i=0; i < FullPpEvent.at(*jit).size() ; ++i ){
       	tmp.push_back( FullPpEvent.at(*jit).at(i) );
      }
      tmp = sorted_by_pt( tmp );
	    
      particles = AuAuparticles;
      float removed=0.0;
      for ( int i = tmp.size()-1; i>=0; --i ){
      	// Remove some soft particles from the near side as a cross check
      	if ( RemoveSoftFromAway > 0 && removed < RemoveSoftFromAway ){
      	  // Only if we have something reasonable there
	  TLorentzVector away =  MakeTLorentzVector( vAwayJet.at(*jit) );
      	  if ( away.Pt() > 10 ){
	    if ( away.DeltaR ( MakeTLorentzVector( tmp.at(i)) )<0.4 ){
	      if ( tmp.at(i).pt() < 2.0 ){
		// cout << "removing particle with pt = " << tmp.at(i).pt() << endl;
		removed += tmp.at(i).pt();
		continue;
	      }  // is soft 
	    } // is in Away Cone
	  } // Away Jet is high enough
      	} // removal goal not reached
	particles.push_back( tmp.at(i) );
      } 	
      
      // Run analysis
      // ------------
      int ret;
      // ret =AjA.AnalyzeAndFill( particles, 0,refmult,
      ret=AjA.AnalyzeAndFill( particles, &(vTriggerJet.at(*jit)), refmult, // Force matching to original trigger
			      UnmatchedAJ_hi, AJ_hi, AJ_lo,
			      
			      UnmatchedhPtHi,  hPtHi, hPtLo,
			      UnmatchedhdPtHi, hdPtHi, hdPtLo,
			      hdphiHi, hdphiLo,
			      
			      OtherAJ_lo, OtherLeadPtLoss_lo, OtherSubLeadPtLoss_lo, OtherR,
			      
			      hdPtLead, hdPtSubLead,
			      SpecialhdPtLead, SpecialhdPtSubLead
			      );
      
      switch ( ret ){
      case 3 : nMatchedDijets++;
	// FALLTHROUGH
      case 2 : nCorrespondingLowDijets++;
	// FALLTHROUGH
      case 1 : nHardDijets++;
	// FALLTHROUGH
	if ( RemoveSoftFromAway > 0 )      RemovedSoft->Fill( removed );
      case 0 : /* Nothing found */
	break;
      default :
	cerr << "Unrecognized return value!" << endl;
	throw(-1);
	return -1;
	break;      
      } //  switch (ret)
      nJetsUsed++;

      // Save results
      vector<PseudoJet> DiJetsHi = AjA.GetDiJetsHi();
      vector<PseudoJet> DiJetsLo = AjA.GetDiJetsLo();
      
      // Check for matching? yes, for now.
      // cout << DiJetsHi.size() << "  " << DiJetsLo.size() << endl;
      // cout << " going in"  << endl;      
      if ( DiJetsLo.size()==2 ){
	j1 = MakeTLorentzVector( DiJetsHi.at(0) );
	j2 = MakeTLorentzVector( DiJetsHi.at(1) );
	jm1 = MakeTLorentzVector( DiJetsLo.at(0) );
	jm2 = MakeTLorentzVector( DiJetsLo.at(1) );

	// DEBUG
	TLorentzVector away =  MakeTLorentzVector( vAwayJet.at(*jit) );
	if ( away.Pt()==0 )   cerr << "Away Jet not previously seen..." << endl;
	else if ( away.DeltaR(j2) > 0.4 )  {
	  cerr << "Away Jet different from previous, DeltaR = " << away.DeltaR(j2)
	       << "  phi found = " << away.Phi() << "  phi now = " << j2.Phi()
	       << "  eta found = " << away.Eta() << "  eta now = " << j2.Eta()
	       << endl;
	}

	// DEBUG
	rho=rhoerr=0;
	fastjet::Selector selector_bkgd = fastjet::SelectorAbsRapMax( 0.6 ) * (!fastjet::SelectorNHardest(2));
	rho=AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	rhoerr=AjA.GetJAlo()->GetBackgroundEstimator()->sigma() ;
	
	ResultTree->Fill();

	// Recover AuAu parts
	vector<PseudoJet> HiCons = DiJetsHi.at(0).constituents();
	for (int i=0; i<HiCons.size(); ++i ){
	  if ( HiCons.at(i).has_user_info() ){
	    if (HiCons.at(i).user_info<JetAnalysisUserInfo>().GetTag() == "auau" ) CapturedHiAuAu->Fill(HiCons.at(i).pt());
	  }
	}

      }      
      
    } // jit
  } // reader.NextEvent()
  cout << "##################################################################" << endl;

  // Close up shop
  // -------------
  fout->Write();

  TFile * f = new TFile("spectra.root","RECREATE");
  MBTracks->Write();
  MBTowers->Write();
  
  
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
