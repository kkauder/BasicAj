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
  // const char *defaults[] = {"PicoAj","test.root","ppHT","~putschke/Data/ppHT/*.root", "0", "0" };
  // const char *defaults[] = {"PicoAj","AjResults/Test11_ppAj.root","HT","/data3/AuAu11picoNPE15_150424/AuAu11Pico_1800-899.root", "0", "0" };
  // const char *defaults[] = {"PicoAj","AuAuAj.root","HT","CleanAuAu/Clean809.root", "0", "0" };
  const char *defaults[] = {"PicoAj","test.root","HT","SmallAuAu/Small_Clean809*.root", "0", "0" };

  if ( argc==1 ) {
    argv=defaults;
    argc=sizeof (defaults ) / sizeof (defaults[0] );
  }

  // Throw arguments in a vector
  // ---------------------------
  vector<string> arguments(argv + 1, argv + argc);

  // Load and set up tree
  // --------------------
  TString ChainName  = "JetTree";
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

  TString TriggerName = arguments.at(1);

  TChain* chain = new TChain( ChainName );
  chain->Add( arguments.at(2).data() );

  // Au+Au?
  // ------
  bool isAuAu=false;
  if (arguments.at(2).find("AuAu") != std::string::npos ) isAuAu=true; // Quick and dirty...
  if (arguments.at(2).find("auau") != std::string::npos ) isAuAu=true; // Quick and dirty...
  
  // OLD: With some tweaks, this can be used to explicitly hand over multiple files
  // for (int i=2; i<arguments.size() ; ++i ) {
  //   chain->Add( arguments.at(i).data() );
  //   if (arguments.at(i).find("AuAu") != std::string::npos ) isAuAu=true; // Quick and dirty...
  //   if (arguments.at(i).find("auau") != std::string::npos ) isAuAu=true; // Quick and dirty...
  // }  

  // for systematics
  // ---------------
  Int_t IntTowScale=atoi( arguments.at(3).data() ); // +/- 2%
  Float_t fTowScale = 1.0 + IntTowScale*0.02;
  Int_t mEffUn=atoi( arguments.at(4).data() ) ;
  
  switch ( mEffUn ){
  case 0 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("Eff0_")+ gSystem->BaseName(OutFileName));
    break;
  case 1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("Eff1_")+ gSystem->BaseName(OutFileName));
    break;
  case -1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("Eff-1_")+ gSystem->BaseName(OutFileName));
    break;
  default :
    cerr << "mEffUn = " << mEffUn << " not supported." <<endl;
    return -1;
  }

  switch ( IntTowScale ){
  case 0 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("Tow0_")+ gSystem->BaseName(OutFileName));
    break;
  case 1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("Tow1_")+ gSystem->BaseName(OutFileName));
    break;
  case -1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("Tow-1_")+ gSystem->BaseName(OutFileName));
    break;
  default :
    cerr << "IntTowScale = " << IntTowScale << " not supported." <<endl;
    return -1;
  }
  if ( isAuAu && IntTowScale ){
    cerr << "IntTowScale = " << IntTowScale << " not supported in AuAu." <<endl;
    return -1;    
  }
  
  double RefMultCut = 0;
  // WARNING: ~putschke/Data/Pico_Eflow/auau_ht* is cut off at 351!
  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );
  reader.SetApplyFractionHadronicCorrection(kTRUE);
  reader.SetFractionHadronicCorrection(0.9999);
  reader.SetRejectTowerElectrons( kFALSE );
  //  reader.SetApplyFractionHadronicCorrection(kFALSE);

  // DEBUG: KK: Reject bad phi strip  
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  trackCuts->SetPhiCut(0, -1.2);
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  towerCuts->SetPhiCut(0, -1.2);

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
  TH3::SetDefaultSumw2(true);
  
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

  // can't compute area_error(), take rms of this instead
  TH1D* hj1area    = new TH1D( "hj1area","j1 area", 320, 0.0, 0.8 );
  TH1D* hj2area    = new TH1D( "hj2area","j2 area", 320, 0.0, 0.8 );
  TH1D* hrestarea    = new TH1D( "hrestarea","restarea", 160, 0.0, 0.8 );

  // For eta cone embedding: need a previous result
  // ----------------------------------------------
  TChain* OrigJets=0;
  TLorentzVector *pJ1 = new TLorentzVector();
  TLorentzVector *pJ2 = new TLorentzVector();
  int Origeventid;
  int Origrunid;
  double Origrefmult;
  vector<int> OrigEvents;

  FileStat_t filestat;
  TString OrigResultName="AjResults/Presel_AuAuAj.root";
  bool RunEtaCone= isAuAu && gSystem->GetPathInfo(OrigResultName, filestat)==0 && fabs(R-0.4)<1e-4 && true;

  if ( RunEtaCone ){
    cout << "Creating Eta Cone histo using jets from " << OrigResultName << endl; 
    OrigJets = new TChain("ResultTree");
    OrigJets->Add("AjResults/Presel_AuAuAj.root");
    OrigJets->SetBranchAddress("j1", &pJ1);
    OrigJets->SetBranchAddress("j2", &pJ2);
    OrigJets->SetBranchAddress("eventid", &Origeventid);
    OrigJets->SetBranchAddress("runid", &Origrunid);
    OrigJets->SetBranchAddress("refmult", &Origrefmult);
    // Random selection: shuffle event indices then take from the top
    for ( int i=0 ; i< OrigJets->GetEntries() ; ++ i )	  OrigEvents.push_back(i);
  } else {
    cout << "Can't create Eta Cone histo using jets from " << OrigResultName << endl; 
  }
  
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
  double refmult; // Really an int, but may change when using refmultcorr
  float rho;
  float rhoerr;
  float j1area;
  float j2area;

  ResultTree->Branch("eventid",&eventid, "eventid/i");
  ResultTree->Branch("runid",&runid, "runid/i");
  ResultTree->Branch("refmult",&refmult, "refmult/d");
  ResultTree->Branch("rho",&rho, "rho/f");
  ResultTree->Branch("rhoerr",&rhoerr, "rhoerr/f");
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
    
  // Initialize tracking efficiency
  // ------------------------------
  ktTrackEff* tEff=0;
  if ( !isAuAu ) {
    tEff = new ktTrackEff();
    tEff->SetSysUncertainty(mEffUn);
    cout<<endl;
    tEff->PrintInfo();
    cout<<endl;
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
  
  // // DEBUG: KK: Reject jets near bad phi strip  
  // Selector& sjet = AjA.GetJetSelector();
  // // Note: It's not 100% clear to me whether jet phi is in -pi--pi or 0--2pi. So we'll provide for both
  // Selector sphi = fastjet::SelectorPhiRange ( -TMath::Pi(), -1.2 - R ) || fastjet::SelectorPhiRange ( 0 + R, TMath::TwoPi() -1.2 - R );
  // sjet = sjet * sphi;

  // Cycle through events
  // --------------------
  vector<PseudoJet> particles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv; // TLorentzVector* would be sufficient. 
  
  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  
  Long64_t nEvents=-1; // -1 for all
  //nEvents=100000;
  reader.Init(nEvents);

  PseudoJet pj;
  float pt1, pt2;
	
  fastjet::GhostedAreaSpec TmpArea; // for access to static random seed
  vector<int> SeedStatus;
  
  fastjet::Selector GrabCone = fastjet::SelectorCircle( R );    
  // DEBUG
  int No4Gev=0;
  try{
    while ( reader.NextEvent() ) {
      reader.PrintStatus(10);

      // event info
      // ----------
      TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();
      
      eventid = header->GetEventId();
      runid   = header->GetRunId();
      // // DEBUG: KK: Reject suspicious refMult runs
      // if (runid >= 12138081 && runid<= 12145020 ) continue;

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
      if ( isAuAu ) refmult=header->GetGReferenceMultiplicity();
      
      // Load event
      // ----------
      container = reader.GetOutputContainer();
      
      // Make particle vector
      // --------------------
      particles.clear();

      // DEBUG: Look for HT trigger
      bool Has4GevTower=false;
      for (int ip = 0; ip<container->GetEntries() ; ++ip ){
	sv = container->Get(ip);  // Note that TStarJetVector  contains more info, such as charge;
	// DEBUG
	if (sv->GetCharge()==0 && sv->Pt() > 4 ) Has4GevTower=true;

	if (sv->GetCharge()==0 ) (*sv) *= fTowScale; // for systematics
	pj=MakePseudoJet( sv );
	pj.set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge() ) );

	if ( sv->GetCharge()!=0 && tEff ) {
	  Double_t reff=tEff->EffRatio_20(sv->Eta(),sv->Pt());
	  Double_t mran=gRandom->Uniform(0,1);
	  // cout << reff << "  " << mran << endl;
	  if (mran<reff)  {
	    particles.push_back ( pj );
	  }
	} else { // no charge or no efficiency class
	  particles.push_back ( pj );
	}	      
      }
      // DEBUG
      if ( !Has4GevTower ) {
	cerr << " EVENT DOESN'T HAVE A 4 GeV TOWER???" << endl;
	No4Gev++;
      }

      // Run analysis
      // ------------
      int ret;      
      ret =AjA.AnalyzeAndFill( particles, 0,refmult, 
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
	if ( AjA.Has10Gev )  { 
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
      if ( DiJetsLo.size()==2 ){
	j1 = MakeTLorentzVector( DiJetsHi.at(0) );
	j2 = MakeTLorentzVector( DiJetsHi.at(1) );
	jm1 = MakeTLorentzVector( DiJetsLo.at(0) );
	jm2 = MakeTLorentzVector( DiJetsLo.at(1) );

	// DEBUG
	rho=rhoerr=j1area=j2area=0;
	nRestJ=0;
	fastjet::Selector selector_bkgd = fastjet::SelectorAbsRapMax( 0.6 ) * (!fastjet::SelectorNHardest(2));
	if ( refmult >= 269 ){
	  rho=AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	  rhoerr=AjA.GetJAlo()->GetBackgroundEstimator()->sigma() ;
	  
	  hrho->Fill(AjA.GetJAlo()->GetBackgroundEstimator()->rho()) ;
	  hrhoerr->Fill(AjA.GetJAlo()->GetBackgroundEstimator()->sigma()) ;
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
	  
	}

	ResultTree->Fill();
      }
      
      if ( RunEtaCone ) {      
	// If possible, shift jets along eta and try again
	// -----------------------------------------------
	// Could make parameters dependent on R but that's a little tricky
	float MinDist = 2*0.4;
	float MaxJetRap = 0.6;
	
	if ( DiJetsLo.size()==2 ){	
	  // THESE WILL BE CHANGED!
	  PseudoJet j1 = DiJetsHi.at(0);
	  PseudoJet j2 = DiJetsHi.at(1);
	  
	  float orig1 = j1.eta();
	  float orig2 = j2.eta();
	  
	  if ( !ShiftEta ( j1, MinDist, MaxJetRap ) ) goto donewithetacone;
	  if ( !ShiftEta ( j2, MinDist, MaxJetRap ) ) goto donewithetacone;
	  
	  if ( fabs( j1.eta()-orig1 ) < 0.8 || fabs( j2.eta()-orig2 ) < 0.8 ){
	    cout << orig1 << " --> " << j1.eta() << endl;
	    cout << orig2 << "--> " << j2.eta() << endl << endl;
	    cerr << "What the hell??" << endl;
	    return -1;
	  }

	  GrabCone.set_reference( j1 );
	  vector<PseudoJet> C1 = GrabCone( AjA.GetLoConstituents() );
	  PseudoJet RC1 (0,0,0,0);
	  for (vector<PseudoJet>::iterator it = C1.begin() ; it!= C1.end() ; ++ it ) RC1+= *it;
	  
	  GrabCone.set_reference( j2 );
	  vector<PseudoJet> C2 = GrabCone( AjA.GetLoConstituents() );
	  PseudoJet RC2 (0,0,0,0);
	  for (vector<PseudoJet>::iterator it = C2.begin() ; it!= C2.end() ; ++ it ) RC2+= *it;
	  	  
	  // // =================== METHOD A ======================
	  // // "Mix" with this event
	  
	  // pt1 = j1.pt() + RC1.pt() - TMath::Pi()*R*R * AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	  // pt2 = j2.pt() + RC2.pt() - TMath::Pi()*R*R * AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	  // EtaShiftAJ_lo->Fill( fabs ( (pt1 - pt2) / (pt1 + pt2) ), refmult );
	  // =================== END METHOD A ======================
	  
		
	  // =================== METHOD B ======================
	  // Instead of the original jet, use a couple of other ones
	  int nMix=6;
	  random_shuffle(OrigEvents.begin(), OrigEvents.end()) ;
	  int mixed=0;
	  for ( int i=0; i<OrigEvents.size(); ++i ){
	    OrigJets->GetEntry ( OrigEvents.at(i) );

	    if ( runid == Origrunid && eventid == Origeventid ) continue; // avoid self-correlation
	    // Some matching options, pretty much unnecessary
	    // if ( fabs ( pJ1->Eta() - orig1 ) > 0.1 ) continue;
	    // if ( fabs ( pJ2->Eta() - orig2 ) > 0.1 ) continue;
	    // if ( fabs ( pJ1->Eta() - j1.eta() ) > 0.1 ) continue;
	    // if ( fabs ( pJ2->Eta() - j2.eta() ) > 0.1 ) continue;
	    // if ( fabs ( refmult - Origrefmult ) > 20 ) continue;
	    
	    mixed++;
	    if ( mixed>=nMix ) break;
	    pt1 = pJ1->Pt() + RC1.pt() - TMath::Pi()*R*R * AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	    pt2 = pJ2->Pt() + RC2.pt() - TMath::Pi()*R*R * AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	    EtaShiftAJ_lo->Fill( fabs ( (pt1 - pt2) / (pt1 + pt2) ), refmult );
	  }
	  
	  if ( mixed != nMix){
	    cerr << "WARNING: Only matched " << mixed << " jets" << endl;
	  }	
	  // =================== END METHOD B ======================
	}
      donewithetacone:
	;
      } // etacone
      
    } // while NextEvent
  } catch ( exception& e) {
    cerr << "Caught " << e.what() << endl;
    return -1;
  }
  
  cout << "##################################################################" << endl;
  
  Long64_t nEventsUsed=reader.GetNOfEvents();  

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

  // DEBUG
  cout << " No 4 GeV tower found in " << No4Gev << " out of " << nEventsUsed << " events." << endl;

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

