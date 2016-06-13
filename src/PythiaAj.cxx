/** @file PicoAj.cxx
    @author Kolja Kauder
    @version Revision 0.2
    @brief Aj analysis and embedding prep in p+p.
    @details Perform Aj analysis in a given TStarPicoJetTree chain. Can also save events with 10 GeV jets for embedding.
    @date Mar 04, 2015
*/

#include "AjAnalysis.hh"
#include "AjParameters.hh"
#include "ktTrackEff.hh"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>

#include <TLorentzVector.h>
#include <TParameter.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TChain.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>

#include "TClonesArray.h"

#include <iostream>
#include <cmath>
#include <exception>
#include <cstdlib>      // std::rand, std::srand
#include <algorithm>    // std::random_shuffle

using namespace std;
using namespace fastjet;

/// Used to randomly shift a jet, for Eta Cone 
bool ShiftEta ( PseudoJet& j, float MinDist, float MaxJetRap );

/** 
    - Set up input tree
    - Set up output histos and tree
    - Initialize AjAnalysis object
    - Loop through events
    \arg argv: 
    - [1] : output file
    - [2] : trigger name
    - [3] : Input file pattern. Let TChain handle the globbing, i.e. use for example
    <BR><tt>% PicoAj '~putschke/Data/ppHT/&lowast;.root'</tt>
    <BR>For cases like 
    <BR><tt>% PicoAj ~putschke/Data/&lowast;/&lowast;.root</tt>
    <BR>change this macro
    - [4] : tower uncertainty switch ( -1/0/1 )
    - [5] : efficiency correction uncertainty switch ( -1/0/1 )
    <BR> Note that we use a vector for argv and shift the index down by one.
*/

int main ( int argc, const char** argv ) {

  // Set up some convenient default
  // ------------------------------
  const char *defaults[] = {"PythiaAj","pythiatest.root","Data/RhicPythia/RhicPythiaOnly_10_ptHat=20_23.root" };

  if ( argc==1 ) {
    argv=defaults;
    argc=sizeof (defaults ) / sizeof (defaults[0] );
  }

  // Throw arguments in a vector
  // ---------------------------
  vector<string> arguments(argv + 1, argv + argc);

  // Load and set up tree
  // --------------------
  TString ChainName  = "tree";
  TString OutFileName = arguments.at(0);
  
  // jet resolution parameter
  // ------------------------
  float R = 0.4;
  // Follow to different R
  float OtherR = 0.2;   // will be set to 0.4 if we trigger on 0.2, i.e., we can follow in either direction

  if ( OutFileName.Contains ("R0.2") ){
    R=0.2;
    OtherR=0.4;
  }

  // soft constituent cut
  // --------------------
  float PtConsLo=0.2;
  if ( OutFileName.Contains ("Pt1") ){
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

  // Also pull a random offset from the file name
  // --------------------------------------------
  // To seed the backgrounder in different ways
  // allows for patterns like output/rndm0/test.root
  // ONLY pulls one digit
  int randomoff = TString( OutFileName( OutFileName.Index("rndm") + 4,  1 )).Atoi(); // defaults to zero
  // eventid: typically < 1M --> shift by 10M
  // runid: typically 8XXYYYY --> shift by 10M
  randomoff *= 10000000;
  cout << " ################################################### " << endl;
  cout << "   FastJet random seeds offset by " << randomoff << endl;
  cout << " ################################################### " << endl;
  

  cout << " ################################################### " << endl;
  cout << "Triggering with R=" << R << endl;
  cout << "Following to R=" << OtherR << endl;
  cout << "Low pT cut =" << PtConsLo << endl;
  cout << " ################################################### " << endl;

  TChain* Events = new TChain( ChainName );
  TString InPattern=arguments.at(1);
  Events->Add( InPattern );
  Long64_t NEvents=-1; // -1 for all
  // Long64_t NEvents=100000; // -1 for all
  if ( NEvents<0 ) NEvents = INT_MAX;
  assert ( Events->GetEntries()>0 && "Something went wrong loading events.");
  NEvents=min(NEvents,Events->GetEntries() );
  cout << "Running over " << Events->GetEntries() << " events." << endl;
  
  double RefMultCut = 0;
  int eventid;
  int runid;
  double weight=1;

  TClonesArray* pFullEvent = new TClonesArray("TStarJetVector");
  Events->GetBranch("PythiaParticles")->SetAutoDelete(kFALSE);
  Events->SetBranchAddress("PythiaParticles", &pFullEvent);
    
  Events->SetBranchAddress("eventid",&eventid);
  Events->SetBranchAddress("runid",&runid);

  // Files and histograms
  // --------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  assert ( fout->IsOpen() );
  cout << " ################################################### " << endl;
  cout << "Writing to: " << fout->GetName() << endl;
  cout << " ################################################### " << endl;
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);
  
  TH2D* UnmatchedAJ_hi = new TH2D( "UnmatchedAJ_hi","Unmatched A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_hi = new TH2D( "AJ_hi","A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_lo = new TH2D( "AJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );

  // Filling these differently, so we need a dummy
  TH2D* dummy = (TH2D*) AJ_hi->Clone("dummy");

  // save in the tree as well 
  double fAJ_hi=-1;
  double fAJ_lo=-1;

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
  TH1D* hJetHadronHiSubLead = new TH1D( "hJetHadronHiSubLead","Jet-h for sub-leading hard constituent jets; #Delta#phi", 200, -2, 2 );
  TH1D* hJetHadronLoLead = new TH1D( "hJetHadronLoLead","Jet-h for leading soft constituent jets; #Delta#phi", 200, -2, 2 );
  TH1D* hJetHadronLoSubLead = new TH1D( "hJetHadronLoSubLead","Jet-h for sub-leading soft constituent jets; #Delta#phi", 200, -2, 2 );

  // Follow to different R
  TH2D* OtherAJ_lo = new TH2D( "OtherAJ_lo","A_{J} for soft constituent jets with other R ;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* OtherLeadPtLoss_lo    = new TH2D( "OtherLeadPtLoss_lo","Leading #Delta p_{T} for soft constituent jets with other R ;A_{J};Refmult;fraction", 120, -10, 50, 800, -0.5, 799.5 );
  TH2D* OtherSubLeadPtLoss_lo = new TH2D( "OtherSubLeadPtLoss_lo","SubLeading #Delta p_{T} for soft constituent jets with other R ;A_{J};Refmult;fraction", 120, -10, 50, 800, -0.5, 799.5 );

  // eta shift destroys "true" soft jet components
  TH2D* EtaShiftAJ_lo = new TH2D( "EtaShiftAJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
	
  // DEBUG histos
  // ------------
  TH3D* ptphieta = new TH3D("ptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 100, -1, 1);
  TH1D* csize = new TH1D("csize","",5000, -0.5, 4999.5 );
  // Find some info on the background
  TH1D* hrho = new TH1D( "hrho","#rho", 240, 0, 120 );
  TH1D* hrhoerr = new TH1D( "hrhoerr","#sigma_{#rho}", 100, 0, 50 );
  TH1D* hrhoHi = new TH1D( "hrhoHi","#rho above 2 GeV", 240, 0, 120 );

  // can't compute area_error(), take rms of this instead
  TH1D* hj1area    = new TH1D( "hj1area","j1 area", 320, 0.0, 0.8 );
  TH1D* hj2area    = new TH1D( "hj2area","j2 area", 320, 0.0, 0.8 );
  TH1D* hrestarea    = new TH1D( "hrestarea","restarea", 160, 0.0, 0.8 );
  fout->cd();

  // Save results
  // ------------
  TTree* ResultTree=new TTree("ResultTree","Result Jets");
  TLorentzVector j1, j2, jm1, jm2;
  ResultTree->Branch("j1",&j1);
  ResultTree->Branch("j2",&j2);
  ResultTree->Branch("jm1",&jm1);
  ResultTree->Branch("jm2",&jm2);
  double refmult; // Really an int, but may change when using refmultcorr
  float rho;
  float rhoerr;
  float rhoHi;
  float j1area;
  float j2area;

  ResultTree->Branch("fAJ_hi",      &fAJ_hi, "fAJ_hi/D" );
  ResultTree->Branch("fAJ_lo",      &fAJ_lo, "fAJ_lo/D" );

  ResultTree->Branch("weight",      &weight, "weight/D" );

  ResultTree->Branch("eventid",&eventid, "eventid/i");
  ResultTree->Branch("runid",&runid, "runid/i");
  ResultTree->Branch("refmult",&refmult, "refmult/d");
  ResultTree->Branch("rho",&rho, "rho/f");
  ResultTree->Branch("rhoerr",&rhoerr, "rhoerr/f");
  ResultTree->Branch("rhoHi",&rhoHi, "rhoHi/f");
  ResultTree->Branch("j1area",&j1area, "j1area/f");
  ResultTree->Branch("j2area",&j2area, "j2area/f");

  // area and pT of all remaining jets (those used for rho)
  static const Int_t kmaxJ=500; // max # of jets
  int nRestJ=0;
  float RestArea[kmaxJ];
  float RestPt[kmaxJ];
  ResultTree->Branch("nRestJ",&nRestJ, "nRestJ/i");
  ResultTree->Branch("RestArea",RestArea, "RestArea[nRestJ]/f");
  ResultTree->Branch("RestPt",  RestPt,   "RestPt[nRestJ]/f");
  
  // For embedding: save the full event and the trigger jet
  // if this is pp and there's at least a 10 GeV jet.
  // -------------------------------------------------------------------
  // This is somewhat wasteful, we could instead read the original trees.
  bool SaveFullEvents = false;
  TTree* TriggeredTree=0;

  TClonesArray TriggerJet( "TLorentzVector",1 ); 
  TClonesArray AwayJet( "TLorentzVector",1 ); 
  static const Int_t kmaxT=5000; // max # of particles
  TClonesArray FullEvent("TLorentzVector",kmaxT);

  // // Split up
  // TClonesArray Tracks("TLorentzVector",kmaxT);
  // TClonesArray Towers("TLorentzVector",kmaxT);

  if (SaveFullEvents) {
    TriggeredTree = new TTree("TriggeredTree","Triggered Events");
    // NOTE: Ignore "Warning in <TTree::Bronch>: Using split mode on a class: TLorentzVector with a custom Streamer"
    TriggeredTree->Branch("FullEvent", &FullEvent );
    // TriggeredTree->Branch("Tracks", &Tracks );
    // TriggeredTree->Branch("Towers", &Towers );
    TriggeredTree->Branch("TriggerJet", &TriggerJet);
    TriggeredTree->Branch("AwayJet", &AwayJet);
    TriggeredTree->Branch("eventid",&eventid, "eventid/i");
    TriggeredTree->Branch("runid",&runid, "runid/i");

  } 
      
  // Initialize analysis class
  // -------------------------
  AjAnalysis AjA( R,
		  AjParameters::jet_ptmin, AjParameters::jet_ptmax,
		  LeadPtMin, SubLeadPtMin, 
		  AjParameters::max_track_rap,
		  PtConsLo,
		  AjParameters::PtConsHi,
		  AjParameters::dPhiCut
		  );  
  
  // Cycle through events
  // --------------------
  vector<PseudoJet> particles;
  TStarJetVector* sv; // TLorentzVector* would be sufficient. 
  
  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
 


  PseudoJet pj;
  float pt1, pt2;
	
  fastjet::GhostedAreaSpec TmpArea; // for access to static random seed
  vector<int> SeedStatus;
  
  fastjet::Selector GrabCone = fastjet::SelectorCircle( R );    

  Long64_t evi =0;
  try{
    while ( true ){    
      if ( evi>= NEvents ) {
	break;
      }
      if ( !(evi%1000) ) cout << "Working on " << evi << " / " << NEvents << endl;
      Events->GetEntry(evi);
      ++evi;

      fAJ_hi=fAJ_lo=-1;

      // Fill particle container
      // -----------------------
      particles.clear();
      for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
	sv = (TStarJetVector*) pFullEvent->At(i);
	// Ensure kinematic similarity
	if ( sv->Pt()< PtConsLo ) continue;
	if ( fabs( sv->Eta() )>AjParameters::max_track_rap ) continue;
	
	pj=MakePseudoJet( sv );
      	pj.set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge() ) );
	particles.push_back ( pj );
      }
      if ( particles.size()==0 ) continue;
      
      // Let's use the eventid as random seed.
      // that way things stay reproducible between different trees
      // but at the same time there's enough randomness
      gRandom->SetSeed( eventid ); 
      
      // NEW 05/07/15: For repeatability across different picoDSTs, set random seed
      // Static member, so we can set it here
      // Annoyingly, the getter and setter isn't static, so we need to instantiate
      // Apparently, the seed is always an int[2], so it's natural to seed it with runid and eventid      
      TmpArea.get_random_status(SeedStatus);
      if ( SeedStatus.size() !=2 ) {
	throw std::string("SeedStatus.size() !=2");
	return -1;
      } 
      SeedStatus.at(0) = runid   + randomoff;
      SeedStatus.at(1) = eventid + randomoff;
      TmpArea.set_random_status(SeedStatus);
      
      refmult=0;
      
      // For pythia, use cross section as weight
      // ---------------------------------------
      weight=1;
      if ( TParameter<double>* sigmaGen=(TParameter<double>*) Events->GetCurrentFile()->Get("sigmaGen") ){
	weight=sigmaGen->GetVal();
      }

      // Run analysis
      // ------------
      int ret;      
      ret =AjA.AnalyzeAndFill( particles, 0, refmult, 
			       dummy, dummy, dummy,
			       dummy, dummy, dummy,
			       // UnmatchedAJ_hi, AJ_hi, AJ_lo,
			       // UnmatchedNoFabsAJ_hi, NoFabsAJ_hi, NoFabsAJ_lo,

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
      case 0 : /* Nothing found */
	break;
      default :
	cerr << "Unrecognized return value!" << endl;
	throw(-1);
	return -1;
	break;      
      }
      

      // save event info
      if ( ret>0 && refmult>268 ) {
	// Selector slo = AjA.GetLoConsSelector();
	// vector<PseudoJet> accepted, notaccepted;
	// slo.sift(particles, accepted, notaccepted);
	// for (int i =0; i< notaccepted.size(); ++i ){
	//   cout << notaccepted.at(i).pt() << "  " << notaccepted.at(i).phi() << "  " << notaccepted.at(i).eta() << "  " << endl;
	// }
	// return 0;	
	csize->Fill(AjA.GetLoConstituents().size());
	for (int i =0; i< AjA.GetLoConstituents().size(); ++i ){
	  ptphieta->Fill( AjA.GetLoConstituents().at(i).pt(), AjA.GetLoConstituents().at(i).phi(), AjA.GetLoConstituents().at(i).eta() );
	}

      }
      
      // Save the full event for embedding if there's at least one 10 GeV jet
      // ------------------------------------------------------------------
      if (SaveFullEvents){
	FullEvent.Clear();
	TriggerJet.Clear();
	AwayJet.Clear();
	if ( AjA.Has10Gev )  { 
	  int j=0;
	  int k=0;
	  for ( int i = 0; i<particles.size() ; ++i ){
	    if ( particles.at(i).pt() >50 ) { 
	      cerr << " =====> " <<particles.at(i).pt()
		   << "  " << particles.at(i).phi()
		   << "  " << particles.at(i).eta()
		   << "  " << particles.at(i).user_info<JetAnalysisUserInfo>().GetQuarkCharge() << endl;
	      if ( ret>0 ) {
		cerr << " AND ret == " << ret << endl;
	      }
	    } 
	    TLorentzVector lv = MakeTLorentzVector( particles.at(i) );
	    new (FullEvent[i])   TLorentzVector ( lv  );
	    // //DEBUG -- quick and dirty differentiation betwen towers and tracks
	    // if ( abs( particles.at(i).user_info<JetAnalysisUserInfo>().GetQuarkCharge())>0 ){
	    //   new (Towers[j++])   TLorentzVector ( lv  );
	    // } else{
	    //   new (Tracks[k++])   TLorentzVector ( lv  );
	    // }	    
	  } // for particles
	  // Save trigger jet as well
	  vector<PseudoJet> JAhiResult = AjA.GetJAhiResult();
	  new (TriggerJet[0]) TLorentzVector ( MakeTLorentzVector( JAhiResult.at(0) )  );	  
	  vector<PseudoJet> DiJetsHi = SelectorDijets( AjParameters::dPhiCut ) ( JAhiResult );
	  if ( DiJetsHi.size()>1 ){
	    if ( DiJetsHi.at(0).pt() < DiJetsHi.at(1).pt() ) {
	      cerr << "Huh??" << endl;
	      return -1;
	    }	    
	    new (AwayJet[0]) TLorentzVector ( MakeTLorentzVector( DiJetsHi.at(1) )  );
	  } else {
	    new (AwayJet[0]) TLorentzVector ();
	  }
	  TriggeredTree->Fill();
	} // has Trigger
      } // SaveFullEvents

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
	rho=rhoerr=j1area=j2area=0;
	nRestJ=0;
	fastjet::Selector selector_bkgd = fastjet::SelectorAbsRapMax( 0.6 ) * (!fastjet::SelectorNHardest(2));
	rho=AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	rhoerr=AjA.GetJAlo()->GetBackgroundEstimator()->sigma() ;
	// // More Debug
	// rhoHi=0;
	// // rhoHi=AjA.GetJAhi()->GetBackgroundEstimator()->rho() ;	
	// // cout << "rhoHi = "  << rhoHi << endl;
	// // cout << AjA.GetJAhi()->GetBackgroundEstimator()->n_jets_used() << " used for HI BG" << endl;
	// // cout << AjA.GetJAhi()->GetBackgroundEstimator()->n_empty_jets() << " of them empty" << endl;

	
	if ( refmult >= 0 ){
	  hrho->Fill(rho) ;
	  hrhoerr->Fill(rhoerr) ;
	  hrhoHi->Fill(rhoHi) ;
	  vector<PseudoJet> lojets = fastjet::sorted_by_pt( selector_bkgd ( AjA.GetJAlo()->inclusive_jets() ) );
	  j1area=DiJetsLo.at(0).area();
	  j2area=DiJetsLo.at(1).area();
	  
	  hj1area->Fill(DiJetsLo.at(0).area());
	  hj2area->Fill(DiJetsLo.at(1).area());
	  for ( int i=2; i <lojets.size() ; ++ i ){
	    hrestarea->Fill(lojets.at(i).area());
	  } 

	  // cout << DiJetsLo.at(0).pt()<< "  "  << jm1.Pt() << "  " << lojets.at(0).pt() << endl;
	  nRestJ=0;
	  for ( int i=0; i<lojets.size()-2 ; ++i ){
	    if ( lojets.at(i+2).pt() < 1e-4 ) continue;
	    RestArea[i] = lojets.at(i+2).area();
	    RestPt[i] = lojets.at(i+2).pt();
	    nRestJ++;
	  }

	  // save q&d jet-hadron correlation
	  for (int i=0; i<particles.size() ; ++i ){
	    PseudoJet& p = particles.at(i);
	    if ( p.user_info<JetAnalysisUserInfo>().GetQuarkCharge() == 0 ) continue;
	    TLorentzVector lv = MakeTLorentzVector( p );

	    if ( p.perp() > 0.2 ){
	      hJetHadronLoLead->Fill( jm1.DeltaPhi( lv ) );
	      hJetHadronLoSubLead->Fill( jm2.DeltaPhi( lv ) );
	    }

	    if ( p.perp() > 2 ){
	      hJetHadronHiLead->Fill( j1.DeltaPhi( lv ) );
	      hJetHadronHiSubLead->Fill( j2.DeltaPhi( lv ) );
	    }

	  }
	}

	// Fill here instead b/c of the weight
	AJ_hi->Fill( AjA.CalcAj( DiJetsHi ), refmult, weight );
	AJ_lo->Fill( AjA.CalcAj( DiJetsLo ), refmult, weight );

	fAJ_hi=AjA.CalcAj( DiJetsHi );
	fAJ_lo=AjA.CalcAj( DiJetsLo );

	ResultTree->Fill();
      }
    } // while NextEvent
  } catch ( exception& e) {
    cerr << "Caught " << e.what() << endl;
    return -1;
  }
  
  cout << "##################################################################" << endl;

  // Close up shop
  // -------------
  fout->cd();
  fout->Write();

  cout << "In " << NEvents << " events, found " << endl
       << nHardDijets << " dijets with constituents above 2 GeV," << endl
       << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
       << " of which " <<  nMatchedDijets << " could be matched." << endl;


  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;

  // cout << "Aj->GetEntries() = " << AJ_hi->GetEntries() << endl;
  // cout << "Unmatched GetEntries() = " << UnmatchedAJ_hi->GetEntries() << endl;
  
  cout << "Aj->GetEntries() = " << UnmatchedAJ_hi->GetEntries() << endl;

  // // DEBUG
  // cout << " No 4 GeV tower found in " << No4Gev << " out of " << nEventsUsed << " events." << endl;

  return 0;
  
}

// ==========================================================
bool ShiftEta ( PseudoJet& j, float MinDist, float MaxJetRap ){
  // we want to generate a random number that is outside
  // j.eta +/- MinDist but inside MaxJetRap
  // Without bias

  // first see if there's empty space
  float LeftEdge  = j.eta() - MinDist;
  float RightEdge = j.eta() + MinDist;
  if ( LeftEdge < -MaxJetRap && RightEdge > MaxJetRap ) return false;
  // We could have done this below but I want to not worry about rounding errors

  // now let's see how much room we have
  if ( LeftEdge  < -MaxJetRap )   LeftEdge  = -MaxJetRap;
  if ( RightEdge >  MaxJetRap )   RightEdge =  MaxJetRap;
  float available = 2* MaxJetRap - ( RightEdge - LeftEdge );
  // cout << " --------------------------------------> " << available << endl;

  // get a random number in that interval and shift
  Double_t shift=gRandom->Uniform(0,available);  

  // NOTE: Ignoring the difference between eta and y here
  if ( shift< LeftEdge + MaxJetRap ){
    // stay on the left
    // j.reset_PtYPhiM ( j.pt(), -MaxJetRap+shift, j.phi(), j.m() );
    j.reset_PtYPhiM ( j.pt(), -MaxJetRap+shift, j.phi(), 0 );
    return true;
  }

  // Else subtract the left and slap the remainder on the right
  shift-= (LeftEdge + MaxJetRap);
  j.reset_PtYPhiM ( j.pt(), RightEdge + shift, j.phi(), 0 );
  
  return true; 
  
}

