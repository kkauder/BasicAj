/** @file GroomPythiaAj.cxx
    @author Kolja Kauder
    @version Revision 0.2
    @brief Aj analysis and embedding prep in p+p, added zg analysis.
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
#include <climits>
#include <exception>
#include <cstdlib>      // std::rand, std::srand
#include <algorithm>    // std::random_shuffle

#include "fastjet/contrib/Recluster.hh"
#include "fastjet/contrib/SoftDrop.hh"

using namespace std;
using namespace fastjet;
using namespace fastjet::contrib;

/// Used to randomly shift a jet, for Eta Cone 
bool ShiftEta ( PseudoJet& j, float MinDist, float MaxJetRap );

ostream & operator<<(ostream &, const PseudoJet &);

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
    <BR> Note that we use a vector for argv and shift the index down by one.
*/

int main ( int argc, const char** argv ) {

  // Set up some convenient default
  // ------------------------------
  // const char *defaults[] = {"PythiaAj","pythiatest.root","Data/RhicPythia/RhicPythiaOnly_10_ptHat=20_23.root" };
  // const char *defaults[] = {"PythiaAj","pythiatest.root","Data/AlternateRhicPythia/LargeEtaPythiaOnly_1_ptHat=25_35.root" };
  // const char *defaults[] = {"PythiaAj","pythiatest_HT54_HTled_TrueMB.root","Data/AlternateRhicPythia/LargeEtaPythiaOnly_1_ptHat=25_35.root" };
  const char *defaults[] = {"PythiaAj","pythiatest_HT54_HTled.root","Data/AlternateRhicPythia/LargeEtaPythiaOnly_1_ptHat=25_35.root" };

  if ( argc==1 ) {
    argv=defaults;
    argc=sizeof (defaults ) / sizeof (defaults[0] );
  }  

  // Probably shouldn't subtract BG for pythia
  // At least not while going to large eta
  bool SubtractSoftBg = false;
  // if ( OutFileName.Contains("Geant") ) SubtractSoftBg = false;

  bool DoSubjets=true;

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

  // High constituent cut
  // --------------------
  float PtConsHi=AjParameters::PtConsHi;
  if ( OutFileName.Contains ("TrueMB") ){
    PtConsHi=0.2;
    // if ( OutFileName.Contains ("HT54") ) {
    //   cerr << "Incompatible Setting" << endl;
    //   return -1;
    // }
  }
  

  //  "High Tower" cut
  // -----------------
  float ManualHtCut = 0;
  if ( OutFileName.Contains ("HT54") ){
    ManualHtCut = 5.4;
  }

  // High tower matching
  // -------------------
  bool HTled=false;
  if ( OutFileName.Contains ("HTled") ){
    HTled=true;
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
  
  TH2D* UnmatchedAJ_hi = new TH2D( "UnmatchedAJ_hi","Unmatched A_{J} for hard constituent jets;|A_{J}|;Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_hi = new TH2D( "AJ_hi","A_{J} for hard constituent jets;|A_{J}|;Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_lo = new TH2D( "AJ_lo","A_{J} for soft constituent jets;|A_{J}|;Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );

  TH2D* UnmatchedNoFabsAJ_hi = new TH2D( "UnmatchedNoFabsAJ_hi","Unmatched A_{J} for hard constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* NoFabsAJ_hi = new TH2D( "NoFabsAJ_hi","A_{J} for hard constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* NoFabsAJ_lo = new TH2D( "NoFabsAJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );

  // Filling these differently, so we need a dummy
  TH2D* dummy = (TH2D*) AJ_hi->Clone("dummy");

  // save in the tree as well 

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

  // zg Analysis
  // ===========
  int nzgBins=40;
  TH2D* zgLead1020Hi = new TH2D("zgLead1020Hi","Leading Hi jet with 10<p_{T}<20;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgLead2030Hi = new TH2D("zgLead2030Hi","Leading Hi jet with 20<p_{T}<30;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgLead3040Hi = new TH2D("zgLead3040Hi","Leading Hi jet with 30<p_{T}<40;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgLead4060Hi = new TH2D("zgLead4060Hi","Leading Hi jet with 40<p_{T}<60;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );

  TH2D* zgLead1020Lo = new TH2D("zgLead1020Lo","Leading Lo jet with 10<p_{T}<20;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgLead2030Lo = new TH2D("zgLead2030Lo","Leading Lo jet with 20<p_{T}<30;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgLead3040Lo = new TH2D("zgLead3040Lo","Leading Lo jet with 30<p_{T}<40;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgLead4060Lo = new TH2D("zgLead4060Lo","Leading Lo jet with 40<p_{T}<60;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );

  TH2D* zgSubLead1020Hi = new TH2D("zgSubLead1020Hi","SubLeading Hi jet with 10<p_{T}<20;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgSubLead2030Hi = new TH2D("zgSubLead2030Hi","SubLeading Hi jet with 20<p_{T}<30;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgSubLead3040Hi = new TH2D("zgSubLead3040Hi","SubLeading Hi jet with 30<p_{T}<40;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgSubLead4060Hi = new TH2D("zgSubLead4060Hi","SubLeading Hi jet with 40<p_{T}<60;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );

  TH2D* zgSubLead1020Lo = new TH2D("zgSubLead1020Lo","SubLeading Lo jet with 10<p_{T}<20;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgSubLead2030Lo = new TH2D("zgSubLead2030Lo","SubLeading Lo jet with 20<p_{T}<30;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgSubLead3040Lo = new TH2D("zgSubLead3040Lo","SubLeading Lo jet with 30<p_{T}<40;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );
  TH2D* zgSubLead4060Lo = new TH2D("zgSubLead4060Lo","SubLeading Lo jet with 40<p_{T}<60;z_{g};Refmult", nzgBins, 0.05, 0.55, 800, -0.5, 799.5 );

  
  // Follow to different R
  TH2D* OtherAJ_lo = new TH2D( "OtherAJ_lo","A_{J} for soft constituent jets with other R ;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* OtherLeadPtLoss_lo    = new TH2D( "OtherLeadPtLoss_lo","Leading #Delta p_{T} for soft constituent jets with other R ;A_{J};Refmult;fraction", 120, -10, 50, 800, -0.5, 799.5 );
  TH2D* OtherSubLeadPtLoss_lo = new TH2D( "OtherSubLeadPtLoss_lo","SubLeading #Delta p_{T} for soft constituent jets with other R ;A_{J};Refmult;fraction", 120, -10, 50, 800, -0.5, 799.5 );

  // eta shift destroys "true" soft jet components
  TH2D* EtaShiftAJ_lo = new TH2D( "EtaShiftAJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* NoFabsEtaShiftAJ_lo = new TH2D( "NoFabsEtaShiftAJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
	
  // DEBUG histos
  // ------------
  TH3D* Chargedptphieta = new TH3D("Chargedptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 100, -1, 1);
  TH3D* Neutralptphieta = new TH3D("Neutralptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 100, -1, 1);
  TH3D* totNeutralptphieta = new TH3D("totNeutralptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 100, -1, 1);
  //TH3D* totChargedptphieta = new TH3D("totChargedptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 100, -1, 1);
  TH3D* totChargedptphieta = new TH3D("totChargedptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 300, -6, 6);
  TH3D* totptphieta = new TH3D("totptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 300, -6, 6);
  TH3D* j1ptphieta = new TH3D("j1ptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 300, -6, 6);
  TH3D* jm1ptphieta = new TH3D("jm1ptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 300, -6, 6);

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

  float zg1, zg2, zgm1, zgm2;
  ResultTree->Branch("zg1",&zg1,"zg1/F");
  ResultTree->Branch("zg2",&zg2,"zg2/F");
  ResultTree->Branch("zgm1",&zgm1,"zgm1/F");
  ResultTree->Branch("zgm2",&zgm2,"zgm2/F");

  double refmult; // Really an int, but may change when using refmultcorr
  float rho;
  float rhoerr;
  float rhoHi;
  float j1area;
  float j2area;

  float aj_lo, aj_hi;
  ResultTree->Branch("eventid",&eventid, "eventid/I");
  ResultTree->Branch("runid",&runid, "runid/I");
  ResultTree->Branch("refmult",&refmult, "refmult/D");
  ResultTree->Branch("weight",&weight, "weight/D"); 
  ResultTree->Branch("aj_lo",&aj_lo, "aj_lo/F");
  ResultTree->Branch("aj_hi",&aj_hi, "aj_hi/F");
  ResultTree->Branch("rho",&rho, "rho/F");
  ResultTree->Branch("rhoerr",&rhoerr, "rhoerr/F");
  ResultTree->Branch("rhoHi",&rhoHi, "rhoHi/F");
  ResultTree->Branch("j1area",&j1area, "j1area/F");
  ResultTree->Branch("j2area",&j2area, "j2area/F");

  // area and pT of all remaining jets (those used for rho)
  static const Int_t kmaxJ=500; // max # of jets
  int nRestJ=0;
  float RestArea[kmaxJ];
  float RestPt[kmaxJ];
  ResultTree->Branch("nRestJ",&nRestJ, "nRestJ/I");
  ResultTree->Branch("RestArea",RestArea, "RestArea[nRestJ]/F");
  ResultTree->Branch("RestPt",  RestPt,   "RestPt[nRestJ]/F");
  
  // For embedding: save the full event and the trigger jet
  // if this is pp and there's at least a 10 GeV jet.
  // -------------------------------------------------------------------
  // This is somewhat wasteful, we could instead read the original trees.
  bool SaveFullEvents = true;
  TTree* TriggeredTree=0;

  TClonesArray TriggerJet( "TLorentzVector",1 ); 
  TClonesArray TriggerJetLo( "TLorentzVector",1 ); 
  TClonesArray AwayJet( "TLorentzVector",1 ); 
  TClonesArray AwayJetLo( "TLorentzVector",1 ); 
  static const Int_t kmaxT=5000; // max # of particles
  // TClonesArray FullEvent("TLorentzVector",kmaxT);
  TClonesArray FullEvent("TStarJetVector",kmaxT);

  float zgtrighi, zgtriglo;
  float zgawayhi, zgawaylo;

  if (SaveFullEvents) {
    TriggeredTree = new TTree("TriggeredTree","Triggered Events");
    // NOTE: Ignore "Warning in <TTree::Bronch>: Using split mode on a class: TLorentzVector with a custom Streamer"
    TriggeredTree->Branch("FullEvent", &FullEvent );
    // TriggeredTree->Branch("Tracks", &Tracks );
    // TriggeredTree->Branch("Towers", &Towers );
    TriggeredTree->Branch("TriggerJet", &TriggerJet);
    TriggeredTree->Branch("TriggerJetLo", &TriggerJetLo);
    TriggeredTree->Branch("AwayJet", &AwayJet);
    TriggeredTree->Branch("AwayJetLo", &AwayJetLo);    
    TriggeredTree->Branch("eventid",&eventid, "eventid/I");
    TriggeredTree->Branch("runid",&runid, "runid/I");
    TriggeredTree->Branch("weight",&weight, "weight/D"); 

    TriggeredTree->Branch("refmult",&refmult, "refmult/D");
    TriggeredTree->Branch("rho",&rho, "rho/F");
    TriggeredTree->Branch("rhoerr",&rhoerr, "rhoerr/F");

    TriggeredTree->Branch("j1",&j1);
    TriggeredTree->Branch("j2",&j2);
    TriggeredTree->Branch("jm1",&jm1);
    TriggeredTree->Branch("jm2",&jm2);

    TriggeredTree->Branch("aj_hi",&aj_hi, "aj_hi/F");
    TriggeredTree->Branch("aj_lo",&aj_lo, "aj_lo/F");

    TriggeredTree->Branch("zgtrighi",&zgtrighi,"zgtrighi/F");
    TriggeredTree->Branch("zgtriglo",&zgtriglo,"zgtriglo/F");
    TriggeredTree->Branch("zgawayhi",&zgawayhi,"zgawayhi/F");
    TriggeredTree->Branch("zgawaylo",&zgawaylo,"zgawaylo/F");
    TriggeredTree->Branch("zg1",&zg1,"zg1/F");
    TriggeredTree->Branch("zg2",&zg2,"zg2/F");
    TriggeredTree->Branch("zgm1",&zgm1,"zgm1/F");
    TriggeredTree->Branch("zgm2",&zgm2,"zgm2/F");
  } 

  // Initialize analysis class
  // -------------------------
  // DEBUG
  double max_track_rap = AjParameters::max_track_rap;
  max_track_rap=7.0;
  AjAnalysis AjA( R,
		  AjParameters::jet_ptmin, AjParameters::jet_ptmax,
		  LeadPtMin, SubLeadPtMin, 
		  max_track_rap,
		  PtConsLo,
		  PtConsHi,
		  AjParameters::dPhiCut,
		  SubtractSoftBg
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

  // For additional QA things
  fastjet::Selector GrabCone = fastjet::SelectorCircle( R );    
  fastjet::Selector NotGhost = !fastjet::SelectorIsPureGhost ();    
  fastjet::Selector OnlyCharged = NotGhost && ( SelectorChargeRange( -3, -1) || SelectorChargeRange( 1, 3) );
  fastjet::Selector OnlyNeutral = NotGhost && SelectorChargeRange( 0, 0);

  Long64_t evi =0;
  TStarJetVector* pHT = 0;
  try{
    while ( true ){    
      if ( evi>= NEvents ) {
	break;
      }
      if ( !(evi%1000) ) cout << "Working on " << evi << " / " << NEvents << endl;
      Events->GetEntry(evi);
      ++evi;

      // MANUAL HT cut for MC data
      // --------------------------
      bool HasManualHighTower=false;

      // Fill particle container
      // -----------------------
      particles.clear();
      for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
	sv = (TStarJetVector*) pFullEvent->At(i);
	// Ensure kinematic similarity
	if ( sv->Pt()< PtConsLo ) continue;
	if ( fabs( sv->Eta() )>max_track_rap ) continue;
	
	// MANUAL HT cut for MC data
	// -------------------------
	if ( sv->GetCharge()==0
	     && fabs(sv->Eta())<1.0
	     && sv->Pt()>ManualHtCut) {
	  HasManualHighTower=true;	
	  if ( HTled ) {
	    if ( !pHT ) pHT=sv;
	    if ( sv->Pt() > pHT->Pt() ) pHT=sv;
	  }
	}
	
	pj=MakePseudoJet( sv );
      	pj.set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge() ) );
	particles.push_back ( pj );
      }
      if ( particles.size()==0 ) continue;
      if ( !HasManualHighTower) continue;
	    
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
	cerr << "SeedStatus.size() !=2" << endl;
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
      j1 = TLorentzVector();
      j2 = TLorentzVector();
      jm1 = TLorentzVector();
      jm2 = TLorentzVector();
      aj_lo=-999;
      aj_hi=-999;
      zg1=zg2=zgm1=zgm2=0;
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
			       SpecialhdPtLead, SpecialhdPtSubLead,
			       0,
			       -1,
			       pHT
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
      vector<PseudoJet> chargedconsts = OnlyCharged (AjA.GetLoConstituents());
      for (int i =0; i< chargedconsts.size(); ++i ){
       	totChargedptphieta->Fill( chargedconsts.at(i).pt(), chargedconsts.at(i).phi(), chargedconsts.at(i).eta(), weight );
	totptphieta->Fill( chargedconsts.at(i).pt(), chargedconsts.at(i).phi(), chargedconsts.at(i).eta(), weight );
      }
      vector<PseudoJet> neutralconsts = OnlyNeutral (AjA.GetLoConstituents());
      for (int i =0; i< neutralconsts.size(); ++i ){	  
	totNeutralptphieta->Fill( neutralconsts.at(i).pt(), neutralconsts.at(i).phi(), neutralconsts.at(i).eta(), weight );
	totptphieta->Fill( neutralconsts.at(i).pt(), neutralconsts.at(i).phi(), neutralconsts.at(i).eta(), weight );
      }
      
      // Save results
      vector<PseudoJet> DiJetsHi = AjA.GetDiJetsHi();
      vector<PseudoJet> DiJetsLo = AjA.GetDiJetsLo();

      // Save the full event for embedding if there's at least one 10 GeV jet
      // ------------------------------------------------------------------
      if (SaveFullEvents){
	FullEvent.Clear();
	TriggerJet.Clear();
	TriggerJetLo.Clear();
	AwayJet.Clear();
	AwayJetLo.Clear();
	zgtrighi = zgtriglo = 0;
	zgawayhi = zgawaylo = 0;
	
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
	  // ------------------------
	  vector<PseudoJet> JAhiResult = AjA.GetJAhiResult();
	  PseudoJet& TrigHi = JAhiResult.at(0);
	  new (TriggerJet[0]) TLorentzVector ( MakeTLorentzVector( TrigHi )  );


	  // Save away jet as well
	  // ------------------------
	  PseudoJet AwayHi;
	  vector<PseudoJet> DiJetsHi = SelectorDijets( AjParameters::dPhiCut ) ( JAhiResult );
	  if ( DiJetsHi.size()>1 ){
	    // if ( DiJetsHi.at(0).pt() < DiJetsHi.at(1).pt() ) {
	    //   cerr << "Huh??" << endl;
	    //   return -1;
	    // }
	    // new (AwayJet[0]) TLorentzVector ( MakeTLorentzVector( DiJetsHi.at(1) )  );
	    AwayHi = DiJetsHi.at(1);
	  }
	  new ( AwayJet[0] ) TLorentzVector ( MakeTLorentzVector( AwayHi )  );

	  // get soft match and do zg analysis on this jet as well
	  // -----------------------------------------------------
	  // Trigger
	  vector<PseudoJet> JAloResult = AjA.GetJAloResult();	 
	  fastjet::Selector SelectClose = fastjet::SelectorCircle( R );
	  SelectClose.set_reference( TrigHi );
	  vector<PseudoJet> MatchedToTrig = sorted_by_pt(SelectClose( JAloResult ));
	  if ( MatchedToTrig.size() == 0 ) {
	    std::cerr << "PROBLEM: SelectorClose returned no match to Trigger jet." << std::endl;
	    return -1;
	  }
	  PseudoJet& TrigLo = MatchedToTrig.at(0);
	  new (TriggerJetLo[0]) TLorentzVector ( MakeTLorentzVector( TrigLo )  );

	  // Away
	  PseudoJet AwayLo;
	  if ( AwayHi.pt()>1e-4 ){
	    SelectClose.set_reference( AwayHi );
	    vector<PseudoJet> MatchedToAway = sorted_by_pt(SelectClose( JAloResult ));
	    if ( MatchedToAway.size() == 0 ) {
	      std::cerr << "PROBLEM: SelectorClose returned no match to Away jet." << std::endl;
	      return -1;
	    }
	    AwayLo = MatchedToAway.at(0);
	  }
	  new (AwayJetLo[0]) TLorentzVector ( MakeTLorentzVector( AwayLo )  );


	  if ( DoSubjets ){
	    // ---------------
	    // Subjet analysis
	    // ---------------
	    double z_cut = 0.10;
	    double beta  = 0.0;
	    
	    bool CustomRecluster=false;
	    JetAlgorithm ReclusterJetAlgorithm;
	    JetDefinition ReclusterJetDef;
	    Recluster * recluster = 0; 	  
	    
	    contrib::SoftDrop sd(beta, z_cut);
	    if ( CustomRecluster ) {
	      sd.set_reclustering(true, recluster);
	    }
	  
	    // Hi cut jets
	    Subtractor* BackgroundSubtractorHi = AjA.GetJAhi()->GetBackgroundSubtractor();
	    if ( BackgroundSubtractorHi ){
	      sd.set_subtractor( BackgroundSubtractorHi );
	      sd.set_input_jet_is_subtracted( true );
	    }
	    
	    // near-side
	    PseudoJet sd_jet = sd( TrigHi );
	    if ( sd_jet == 0){
	      cout <<  " TriggerJet:   " << TrigHi << endl;
	      cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	    } else {
	      zgtrighi=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	    }
	    
	    // away-side
	    if ( AwayHi.pt()>1e-4 ){
	      sd_jet = sd( AwayHi );
	      if ( sd_jet == 0){
		cout <<  "AwayJet:   " << AwayHi << endl;
		cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	      } else {
		zgawayhi=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	      }
	    }

	    // Lo cut jets
	    Subtractor* BackgroundSubtractorLo = AjA.GetJAlo()->GetBackgroundSubtractor();
	    if ( BackgroundSubtractorLo ){
	      sd.set_subtractor( BackgroundSubtractorLo );
	      sd.set_input_jet_is_subtracted( true );
	    }
	    
	    // near-side
	    sd_jet = sd( TrigLo );
	    if ( sd_jet == 0){
	      cout <<  " Trigger Jet, soft match:   " << TrigLo << endl;
	      cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	    } else {
	      zgtriglo=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	      // cout << zgtriglo << endl;
	    }

	    // away-side
	    if ( AwayLo.pt()>1e-4 ){
	      sd_jet = sd( AwayLo );
	      if ( sd_jet == 0){
		cout <<  "AwayJet, soft match:   " << AwayLo << endl;
		cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	      } else {
		zgawaylo=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	      }
	    }
	  	    
	  } // DoSubjets


	  TriggeredTree->Fill();
	} // has Trigger
      } // SaveFullEvents

      // Save results

      // Check for matching? yes, for now.
      // cout << DiJetsHi.size() << "  " << DiJetsLo.size() << endl;
      if ( DiJetsLo.size()==2 ){
	
	j1 = MakeTLorentzVector( DiJetsHi.at(0) );
	j2 = MakeTLorentzVector( DiJetsHi.at(1) );
	jm1 = MakeTLorentzVector( DiJetsLo.at(0) );
	jm2 = MakeTLorentzVector( DiJetsLo.at(1) );

	aj_hi=AjA.CalcAj( DiJetsHi );
	aj_lo=AjA.CalcAj( DiJetsLo );

	// DEBUG
	rho=rhoerr=j1area=j2area=0;
	nRestJ=0;
	fastjet::Selector selector_bkgd = fastjet::SelectorAbsRapMax( 0.6 ) * (!fastjet::SelectorNHardest(2));
	if ( AjA.GetJAhi()->GetBackgroundSubtractor() ){
	  rho=AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	  rhoerr=AjA.GetJAlo()->GetBackgroundEstimator()->sigma() ;
	}
	// // More Debug
	// rhoHi=0;
	// // rhoHi=AjA.GetJAhi()->GetBackgroundEstimator()->rho() ;	
	// // cout << "rhoHi = "  << rhoHi << endl;
	// // cout << AjA.GetJAhi()->GetBackgroundEstimator()->n_jets_used() << " used for HI BG" << endl;
	// // cout << AjA.GetJAhi()->GetBackgroundEstimator()->n_empty_jets() << " of them empty" << endl;
	
	if ( fabs(DiJetsHi.at(0).eta())<0.6 && fabs(DiJetsLo.at(0).eta())<0.6 ){
	  vector<PseudoJet> j1consts = DiJetsHi.at(0).constituents();
	  for (int i =0; i< j1consts.size(); ++i ){
	      j1ptphieta->Fill(j1consts.at(i).pt(), j1consts.at(i).phi(), j1consts.at(i).eta(), weight );
	  }
	  vector<PseudoJet> jm1consts = DiJetsLo.at(0).constituents();
	  for (int i =0; i< jm1consts.size(); ++i ){
	    jm1ptphieta->Fill(jm1consts.at(i).pt(), jm1consts.at(i).phi(), jm1consts.at(i).eta(), weight );
	  }
	}

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
	  for ( int i=0; i< int ( lojets.size() ) -2 ; ++i ){
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
	// 	continue;
      
	if ( DoSubjets ){
	  // ---------------
	  // Subjet analysis
	  // ---------------
	  double z_cut = 0.10;
	  double beta  = 0.0;
	  
	  bool CustomRecluster=false;
	  JetAlgorithm ReclusterJetAlgorithm;
	  JetDefinition ReclusterJetDef;
	  Recluster * recluster = 0; 	  
	  
	  contrib::SoftDrop sd(beta, z_cut);
	  if ( CustomRecluster ) {
	    sd.set_reclustering(true, recluster);
	  }
	  
	  // Hi cut jets
	  Subtractor* BackgroundSubtractorHi = AjA.GetJAhi()->GetBackgroundSubtractor();
	  if ( BackgroundSubtractorHi ){
	    sd.set_subtractor( BackgroundSubtractorHi );
	    sd.set_input_jet_is_subtracted( true );
	  }
	  
	  PseudoJet sd_jet = sd( DiJetsHi.at(0) );
	  if ( sd_jet == 0){
	    cout <<  " FOREGROUND Original Jet, HI:   " << DiJetsHi.at(0) << endl;
	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	  } else {
	    zg1=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	    // cout << "Hi, lead: " << sd_jet.pt() << endl;
	    // cout << zg1 << endl;
	  }
	  
	  sd_jet = sd( DiJetsHi.at(1) );
	  if ( sd_jet == 0){
	    cout <<  " FOREGROUND Original sub-leading Jet, HI:   " << DiJetsHi.at(1) << endl;
	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	  } else {
	    zg2=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	    // cout << "Hi, sublead: " << sd_jet.pt() << endl;
	    // cout << zg2 << endl;
	  }
	  
	  // Lo cut jets
	  Subtractor* BackgroundSubtractorLo = AjA.GetJAlo()->GetBackgroundSubtractor();
	  if ( BackgroundSubtractorLo ){
	    sd.set_subtractor( BackgroundSubtractorLo );
	    sd.set_input_jet_is_subtracted( true );
	  }
	  
	  sd_jet = sd( DiJetsLo.at(0) );
	  if ( sd_jet == 0){
	    cout <<  " FOREGROUND Original Jet, LO:   " << DiJetsLo.at(0) << endl;
	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	  } else {
	    // cout << "Lo, lead: " << sd_jet.pt() << endl;
	    zgm1=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	  }

	  sd_jet = sd( DiJetsLo.at(1) );
	  if ( sd_jet == 0){
	    cout <<  " FOREGROUND Original sub-leading Jet, LO:   " << DiJetsLo.at(1) << endl;
	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	  } else {
	    // cout << "Lo, sublead: " << sd_jet.pt() << endl;
	    zgm2=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	  }

	  float ptHi = DiJetsHi.at(0).pt();
	  if ( ptHi >= 10 && ptHi < 20 ){
	    zgLead1020Hi->Fill ( zg1, refmult, weight );
	    zgLead1020Lo->Fill ( zgm1, refmult, weight );
	  }
	  if ( ptHi >= 20 && ptHi < 30 ){
	    zgLead2030Hi->Fill ( zg1, refmult, weight );
	    zgLead2030Lo->Fill ( zgm1, refmult, weight );
	  }
	  if ( ptHi >= 30 && ptHi < 40 ){
	    zgLead3040Hi->Fill ( zg1, refmult, weight );
	    zgLead3040Lo->Fill ( zgm1, refmult, weight );
	  }
	  if ( ptHi >= 40 && ptHi < 60 ){
	    zgLead4060Hi->Fill ( zg1, refmult, weight );
	    zgLead4060Lo->Fill ( zgm1, refmult, weight );
	  }

	  float SubLeadingptHi = DiJetsHi.at(1).pt();
	  if ( SubLeadingptHi >= 10 && SubLeadingptHi < 20 ){
	    zgSubLead1020Hi->Fill ( zg2, refmult, weight );
	    zgSubLead1020Lo->Fill ( zgm2, refmult, weight );
	  }
	  if ( SubLeadingptHi >= 20 && SubLeadingptHi < 30 ){
	    zgSubLead2030Hi->Fill ( zg2, refmult, weight );
	    zgSubLead2030Lo->Fill ( zgm2, refmult, weight );
	  }
	  if ( SubLeadingptHi >= 30 && SubLeadingptHi < 40 ){
	    zgSubLead3040Hi->Fill ( zg2, refmult, weight );
	    zgSubLead3040Lo->Fill ( zgm2, refmult, weight );
	  }
	  if ( SubLeadingptHi >= 40 && SubLeadingptHi < 60 ){
	    zgSubLead4060Hi->Fill ( zg2, refmult, weight );
	    zgSubLead4060Lo->Fill ( zgm2, refmult, weight );
	  }
	  
	} // DoSubjets

	// Fill here instead b/c of the weight
	AJ_hi->Fill( fabs( AjA.CalcAj( DiJetsHi ) ), refmult, weight );
	AJ_lo->Fill( fabs( AjA.CalcAj( DiJetsLo ) ), refmult, weight );

	NoFabsAJ_hi->Fill( AjA.CalcAj( DiJetsHi ), refmult, weight );
	NoFabsAJ_lo->Fill( AjA.CalcAj( DiJetsLo ), refmult, weight );


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

  cout << "Aj->GetEntries() = " << AJ_hi->GetEntries() << endl;
  // cout << "Unmatched GetEntries() = " << UnmatchedAJ_hi->GetEntries() << endl;
  
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

//----------------------------------------------------------------------
/// overloaded jet info output
ostream & operator<<(ostream & ostr, const PseudoJet & jet) {
  if (jet == 0) {
    ostr << " 0 ";
  } else {
    ostr << " pt = " << jet.pt()
         << " m = " << jet.m()
         << " y = " << jet.rap()
         << " phi = " << jet.phi()
         << " ClusSeq = " << (jet.has_associated_cs() ? "yes" : "no");
  }
  return ostr;
}
//----------------------------------------------------------------------



