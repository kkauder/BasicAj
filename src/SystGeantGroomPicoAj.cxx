/** @file GroomPicoAj.cxx
    @author Kolja Kauder
    @version Revision 0.2
    @brief Aj analysis and embedding prep in p+p, added zg analysis.
    @details Perform Aj analysis in a given TStarPicoJetTree chain. Can also save events with 10 GeV jets for embedding.
    @date Mar 04, 2015
*/

// FOR MC:
// If using PicoReader class to read MC, it would be NOT be a
// problem as the number of towers is set to 0 and nothing could be
// loaded. and I think that is the way both Kolja and I read the Pico
// data instead of pulling out of branch by hand.

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
    - [4] : tower uncertainty switch ( -1/0/1 )
    - [5] : efficiency correction uncertainty switch ( -1/0/1 )
    <BR> Note that we use a vector for argv and shift the index down by one.
*/

int main ( int argc, const char** argv ) {

  // Set up some convenient default
  // ------------------------------
  // const char *defaults[] = {"GroomPicoAj","GeantGroomtest.root","all","Data/AddedGeantPythia/picoDst*root", "0", "0", "0","" };
  // const char *defaults[] = {"GroomPicoAj","GeantGroomtest.root","all","Data/AddedGeantPythia/picoDst_15_25_9.root", "0", "0", "0","" };
  // const char *defaults[] = {"GroomPicoAj","SystGeantGroom_HT54_HTled_NoEff.root","all","Data/AddedGeantPythia/picoDst_25_35_9.root", "0", "0", "0","" };
  // const char *defaults[] = {"GroomPicoAj","SystGeantGroom_HT54_HTled_NoEff.root","all","Data/AddedGeantPythia/picoDst_25_35_9.root", "0", "0", "-1","" };
  const char *defaults[] = {"GroomPicoAj","SystGeantGroom_HT54_HTled_NoEff.root","all","Data/AddedGeantPythia/picoDst_25_35_9.root", "0", "10", "0","" };

  // const char *defaults[] = {"GroomPicoAj","GeantGroomtest.root","all","Data/AddedGeantPythia/picoDst_15_25_9.root", "+1", "+1", "0","" };

  if ( argc==1 ) {
    argv=defaults;
    argc=sizeof (defaults ) / sizeof (defaults[0] );
  }

  bool DoSubjets=true;

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

  // AJ Cut for zg?
  // --------------
  float AjCut = -999;
  int AjCutDir = 0;

  if ( OutFileName.Contains ("AjGt3") ){
    AjCut = 0.3;
    AjCutDir = 1;
  }

  if ( OutFileName.Contains ("AjLt3") ){
    AjCut = 0.3;
    AjCutDir = -1;
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
    // Kind of a bad name. Oh well.
    
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
  TString InPattern=arguments.at(2);

  // Au+Au?
  // ------
  bool isAuAu=false;
  string INPATTERN=InPattern.Data();
  std::transform(INPATTERN.begin(), INPATTERN.end(),INPATTERN.begin(), ::toupper);
  if (INPATTERN.find("AUAU") != std::string::npos ) isAuAu=true;
  if (isAuAu){
    cerr << "This macro is not supposed to be used with AuAu" << endl;
    return -1;
  }

  bool SubtractSoftBg = true;
  if ( OutFileName.Contains("Geant") ) SubtractSoftBg = false;
  else  {
    cerr << "This macro is not supposed to be used with anything but GEANT" << endl;
    return -1;
  }

  // bool DoEfficiencyCorrection=!isAuAu;
  // if ( OutFileName.Contains("GeantMc") ) DoEfficiencyCorrection = false;
  // if ( OutFileName.Contains("NoEff") ) DoEfficiencyCorrection = false;
	
  if ( OutFileName.Contains("GeantMc") ){
    ChainName = "JetTreeMc";
  }

  TChain* chain = new TChain( ChainName );
  chain->Add( InPattern );
  cout << "Running over " << chain->GetEntries() << " events." << endl;
  if ( chain->GetEntries() >= INT_MAX) {
    cerr << "Sorry, currently unable to process more than INT_MAX entries" << endl;
    return 0;
  }
  
  // for systematics
  // ---------------

  // Tower GAIN: 4.8%
  Int_t IntTowScale=atoi( arguments.at(3).data() ); // +/- 4.8%
  Float_t fTowScale = 1.0 + IntTowScale*0.048;
  switch ( IntTowScale ){
  case 0 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("ATow0_")+ gSystem->BaseName(OutFileName));
    break;
  case 1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("ATow1_")+ gSystem->BaseName(OutFileName));
    break;
  case -1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("ATow-1_")+ gSystem->BaseName(OutFileName));
    break;
  default :
    cerr << "IntTowScale = " << IntTowScale << " not supported." <<endl;
    return -1;
  }

  // Track pT RESOLUTION:
  // DpT/pT = 0.01 + 0.005 * pT (in GeV)
  // --> DpT = 0.01*pT + 0.05*pT^2
  // OR pT shifted = pT +/- (0.01*pT + 0.05*pT^2)
  // OR pT_new = pT * (1+Dpt/pT) = pT * ( 1 +/- (0.01 + 0.005 * pT) )
  // Two options
  // === +/-1: Use min/max
  // === 10: Smear
  // --- for min/max:
  TF1* TrackPtScaler = new TF1( "TrackPtScaler","1 + [0] * ([1]  + [2]*x)", 0, 15);
  TrackPtScaler->FixParameter(1,0.01);
  TrackPtScaler->FixParameter(2,0.005);
  Int_t IntTrackResShift=atoi( arguments.at(4).data() );
  TrackPtScaler->FixParameter(0,float(IntTrackResShift));

  // --- for smearing:
  TF1* TrackPtSigma = new TF1( "TrackPtSigma","[0] * ([1]  + [2]*x)", 0, 30);
  TrackPtSigma->SetParameters(1,0.01, 0.005);
  TF1* TrackPtRandom = new TF1("TrackPtRandom","TMath::Gaus(x,[0],[1],1)",-10,10);

  switch ( IntTrackResShift ){
  case 0 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("PtRes0_")+ gSystem->BaseName(OutFileName));
    break;
  case 10 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("PtRes10_")+ gSystem->BaseName(OutFileName));
    break;
  case 1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("PtRes1_")+ gSystem->BaseName(OutFileName));
    break;
  case -1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("PtRes-1_")+ gSystem->BaseName(OutFileName));
    break;
  default :
    cerr << "IntTrackResShift = " << IntTrackResShift << " not supported." <<endl;
    return -1;
  }

  // Tracking EFFICIENCY
  Int_t mEffUn=atoi( arguments.at(5).data() ); // can be down 5%
  float EffScale=1.0 + mEffUn*0.05;
  switch ( mEffUn ){
  case 0 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("AEff0_")+ gSystem->BaseName(OutFileName));
    break;
  case -1 :
    if ( !isAuAu ) OutFileName.ReplaceAll ( gSystem->BaseName(OutFileName), TString ("AEff-1_")+ gSystem->BaseName(OutFileName));
    break;
  default :
    cerr << "mEffUn = " << mEffUn << " not supported." <<endl;
    return -1;
  }
      
  if ( isAuAu && IntTowScale ){
    cerr << "IntTowScale = " << IntTowScale << " not supported in AuAu." <<endl;
    return -1;    
  }
  
  double RefMultCut = 0;
  // WARNING: ~putschke/Data/Pico_Eflow/auau_ht* is cut off at 351!
  TStarJetPicoReader reader = SetupReader( chain, TriggerName, RefMultCut );

  // reader.SetApplyFractionHadronicCorrection(kFALSE);
  reader.SetApplyFractionHadronicCorrection(kTRUE);
  reader.SetFractionHadronicCorrection(0.9999);
  reader.SetRejectTowerElectrons( kFALSE );

  if ( InPattern.Contains( "picoDst_4_5" ) ) reader.SetUseRejectAnyway( true );  
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  
  float ManualHtCut = 0;
  // Add High Tower cuts
  if ( OutFileName.Contains ("HT54") ){
    // Systematics:
    float HTcut =5.4;
    if ( IntTowScale!=0 )  HTcut /= fTowScale;
    
    evCuts->SetMinEventEtCut ( HTcut );
    evCuts->SetUseRawForMinEventEtCut ( true );

    // This method does NOT WORK for GEANT MC trees because everything is in the tracks...
    // Do it by hand then...
    ManualHtCut = HTcut;
  }
  
  // Run 11: Use centrality cut
  if ( InPattern.Contains("NPE18") ){
    evCuts->SetReferenceCentralityCut (  6, 8 ); // 6,8 for 0-20%
  }

  // Explicitly choose bad tower list here
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  if ( InPattern.Contains("NPE") || InPattern.Contains("11picoMB") ){
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/badTowerList_y11.txt");
  } else if ( InPattern.Contains("Y14") ){
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Alternate_QuickAndDirty_y14.txt");
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
  } else {
    // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/OrigY7MBBadTowers.txt");
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_AuAu_Nick.txt");
    towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
  }
  // Add the following to y11 as well, once we're embedding!
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");

  // DEBUG ONLY
  // towerCuts->AddBadTowers( "emptyBadTowerList.txt");

  // // DEBUG: KK: Reject bad phi strip  
  // towerCuts->SetPhiCut(0, -1.2);
  // TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  // trackCuts->SetPhiCut(0, -1.2);

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
  
  TH2D* UnmatchedAJ_hi = new TH2D( "UnmatchedAJ_hi","Unmatched A_{J} for hard constituent jets;|A_{J}|;Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_hi = new TH2D( "AJ_hi","A_{J} for hard constituent jets;|A_{J}|;Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_lo = new TH2D( "AJ_lo","A_{J} for soft constituent jets;|A_{J}|;Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );

  TH2D* UnmatchedNoFabsAJ_hi = new TH2D( "UnmatchedNoFabsAJ_hi","Unmatched A_{J} for hard constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* NoFabsAJ_hi = new TH2D( "NoFabsAJ_hi","A_{J} for hard constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  TH2D* NoFabsAJ_lo = new TH2D( "NoFabsAJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );

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

  // Quick debug
  TH1D* hLeadPtInTop20 = new TH1D( "hLeadPtInTop20","p_{T}^{reco}", 120, 0, 60 );
  TH1D* hSubLeadPtInTop20 = new TH1D( "hSubLeadPtInTop20","p_{T}^{reco}", 120, 0, 60 );
    
  TH1D* HT54InTop20 = new TH1D( "HT54InTop20","", 2, -0.5, 1.5 );
  TH1D* HT64InTop20 = new TH1D( "HT64InTop20","", 2, -0.5, 1.5 );


  // For Elke...
  // -----------
  TH2D* hNConstChHiLead = new TH2D( "hNConstChHiLead","p_{T}^{C} > 2 GeV/c, Leading;Charged Constituents;Refmult", 100, -0.5, 99, 800, -0.5, 799.5 );
  TH2D* hNConstChLoLead = new TH2D( "hNConstChLoLead","p_{T}^{C} > 0.2 GeV/c, Leading;Charged Constituents;Refmult", 100, -0.5, 99, 800, -0.5, 799.5 );
  TH2D* hNConstNeHiLead = new TH2D( "hNConstNeHiLead","p_{T}^{C} > 2 GeV/c, Leading;Neutral Constituents;Refmult", 100, -0.5, 99, 800, -0.5, 799.5 );
  TH2D* hNConstNeLoLead = new TH2D( "hNConstNeLoLead","p_{T}^{C} > 0.2 GeV/c, Leading;Neutral Constituents;Refmult", 100, -0.5, 99, 800, -0.5, 799.5 );

  TH2D* hNConstChHiSubLead = new TH2D( "hNConstChHiSubLead","p_{T}^{C} > 2 GeV/c, SubLeading;Charged Constituents;Refmult", 100, -0.5, 99, 800, -0.5, 799.5 );
  TH2D* hNConstChLoSubLead = new TH2D( "hNConstChLoSubLead","p_{T}^{C} > 0.2 GeV/c, SubLeading;Charged Constituents;Refmult", 100, -0.5, 99, 800, -0.5, 799.5 );
  TH2D* hNConstNeHiSubLead = new TH2D( "hNConstNeHiSubLead","p_{T}^{C} > 2 GeV/c, SubLeading;Neutral Constituents;Refmult", 100, -0.5, 99, 800, -0.5, 799.5 );
  TH2D* hNConstNeLoSubLead = new TH2D( "hNConstNeLoSubLead","p_{T}^{C} > 0.2 GeV/c, SubLeading;Neutral Constituents;Refmult", 100, -0.5, 99, 800, -0.5, 799.5 );

  TH2D* hPtMissed1 = new TH2D( "hPtMissed1","Missed After Matching, Leading;p_{T}^{C};Refmult", 100, 0, 20, 800, -0.5, 799.5 );
  TH2D* hPtMissed2 = new TH2D( "hPtMissed2","Missed After Matching, SubLeading;p_{T}^{C};Refmult", 100, 0, 20, 800, -0.5, 799.5 );
  TH2D* hDRMissed1 = new TH2D( "hDRMissed1","Missed After Matching, Leading;#Delta R to orig Jet;Refmult", 80, 0, 0.8, 800, -0.5, 799.5 );
  TH2D* hDRMissed2 = new TH2D( "hDRMissed2","Missed After Matching, SubLeading;#Delta R to orig Jet;Refmult", 80, 0, 0.8, 800, -0.5, 799.5 );
  TH2D* hvDRMissed1 = new TH2D( "hvDRMissed1","Missed After Matching, Leading;#Delta R to new Jet;Refmult", 80, 0, 0.8, 800, -0.5, 799.5 );
  TH2D* hvDRMissed2 = new TH2D( "hvDRMissed2","Missed After Matching, SubLeading;#Delta R to new Jet;Refmult", 80, 0, 0.8, 800, -0.5, 799.5 );


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
  // TString OrigResultName="AjResults/Presel_AuAuAj.root";

  TString OrigResultName= "";
  if ( arguments.size() >=6 ) OrigResultName=arguments.at(5);
  bool RunEtaCone= isAuAu && gSystem->GetPathInfo(OrigResultName, filestat)==0 && fabs(R-0.4)<1e-4 && true;

  if ( RunEtaCone ){
    cout << "Creating Eta Cone histo using jets from " << OrigResultName << endl; 
    OrigJets = new TChain("ResultTree");
    OrigJets->Add(OrigResultName);
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
  fout->cd();

  // Save results
  // ------------
  TTree* ResultTree=new TTree("ResultTree","Result Jets");
  TLorentzVector j1, j2, jm1, jm2;
  TLorentzVector j1s1, j1s2, j2s1, j2s2, jm1s1, jm1s2, jm2s1, jm2s2;
  
  ResultTree->Branch("j1",&j1);
  ResultTree->Branch("j2",&j2);
  ResultTree->Branch("jm1",&jm1);
  ResultTree->Branch("jm2",&jm2);
  ResultTree->Branch("j1s1",&j1s1);
  ResultTree->Branch("j1s2",&j1s2);
  ResultTree->Branch("j2s1",&j2s1);
  ResultTree->Branch("j2s2",&j2s2);
  ResultTree->Branch("jm1s1",&jm1s1);
  ResultTree->Branch("jm1s2",&jm1s2);
  ResultTree->Branch("jm2s1",&jm2s1);
  ResultTree->Branch("jm2s2",&jm2s2);
  TLorentzVector gjm1, gjm2;
  ResultTree->Branch("gjm1",&gjm1);
  ResultTree->Branch("gjm2",&gjm2);

  float zg1, zg2, zgm1, zgm2;
  ResultTree->Branch("zg1",&zg1,"zg1/F");
  ResultTree->Branch("zg2",&zg2,"zg2/F");
  ResultTree->Branch("zgm1",&zgm1,"zgm1/F");
  ResultTree->Branch("zgm2",&zgm2,"zgm2/F");

  int eventid;
  int runid;
  double refmult; // Really an int, but may change when using refmultcorr
  float rho;
  float rhoerr;
  float rhoHi;
  float j1area;
  float j2area;

  int GRefCent;
  int RefCent;
  float aj_lo, aj_hi;
  double weight;     // Primarily to stitch together MC data
  ResultTree->Branch("eventid",&eventid, "eventid/I");
  ResultTree->Branch("runid",&runid, "runid/I");
  ResultTree->Branch("refmult",&refmult, "refmult/D");
  ResultTree->Branch("GRefCent",&GRefCent, "GRefCent/I");
  ResultTree->Branch("RefCent",&RefCent, "RefCent/I");
  ResultTree->Branch("weight",&weight, "weight/D"); 
  ResultTree->Branch("aj_lo",&aj_lo, "aj_lo/F");
  ResultTree->Branch("aj_hi",&aj_hi, "aj_hi/F");
  ResultTree->Branch("rho",&rho, "rho/F");
  ResultTree->Branch("rhoerr",&rhoerr, "rhoerr/F");
  ResultTree->Branch("rhoHi",&rhoHi, "rhoHi/F");
  ResultTree->Branch("j1area",&j1area, "j1area/F");
  ResultTree->Branch("j2area",&j2area, "j2area/F");

  float MatchPerc1, MatchPerc2;
  ResultTree->Branch("MatchPerc1",&MatchPerc1, "MatchPerc1/F");
  ResultTree->Branch("MatchPerc2",&MatchPerc2, "MatchPerc2/F");
  float MatchLoss1, MatchLoss2;
  ResultTree->Branch("MatchLoss1",&MatchLoss1, "MatchLoss1/F");
  ResultTree->Branch("MatchLoss2",&MatchLoss2, "MatchLoss2/F");

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
  bool PrepEtaConeEmbedding=true;
  bool SaveFullEvents = TriggerName.Contains("ppHT") || PrepEtaConeEmbedding;
  TTree* TriggeredTree=0;

  TClonesArray TriggerJet( "TLorentzVector",1 ); 
  TClonesArray TriggerJetLo( "TLorentzVector",1 );
  TClonesArray GroomedTriggerJetLo( "TLorentzVector",1 ); 
  TClonesArray AwayJet( "TLorentzVector",1 ); 
  TClonesArray AwayJetLo( "TLorentzVector",1 );
  TClonesArray GroomedAwayJetLo( "TLorentzVector",1 );
 
  static const Int_t kmaxT=5000; // max # of particles
  // TClonesArray FullEvent("TLorentzVector",kmaxT);
  TClonesArray FullEvent("TStarJetVector",kmaxT);

  float zgtrighi, zgtriglo;
  float zgawayhi, zgawaylo;

  TLorentzVector Thi_s1, Thi_s2, Tlo_s1, Tlo_s2;
  TLorentzVector Ahi_s1, Ahi_s2, Alo_s1, Alo_s2;
  if (SaveFullEvents) {
    TriggeredTree = new TTree("TriggeredTree","Triggered Events");
    // NOTE: Ignore "Warning in <TTree::Bronch>: Using split mode on a class: TLorentzVector with a custom Streamer"
    TriggeredTree->Branch("FullEvent", &FullEvent );
    // TriggeredTree->Branch("Tracks", &Tracks );
    // TriggeredTree->Branch("Towers", &Towers );
    TriggeredTree->Branch("TriggerJet", &TriggerJet);
    TriggeredTree->Branch("TriggerJetLo", &TriggerJetLo);
    TriggeredTree->Branch("GroomedTriggerJetLo", &GroomedTriggerJetLo);
    TriggeredTree->Branch("AwayJet", &AwayJet);
    TriggeredTree->Branch("AwayJetLo", &AwayJetLo);
    TriggeredTree->Branch("GroomedAwayJetLo", &GroomedAwayJetLo);
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
    TriggeredTree->Branch("gjm1",&gjm1);
    TriggeredTree->Branch("gjm2",&gjm2);

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

    TriggeredTree->Branch("Thi_s1",&Thi_s1);
    TriggeredTree->Branch("Thi_s2",&Thi_s2);
    TriggeredTree->Branch("Tlo_s1",&Tlo_s1);
    TriggeredTree->Branch("Tlo_s2",&Tlo_s2);

    TriggeredTree->Branch("Ahi_s1",&Ahi_s1);
    TriggeredTree->Branch("Ahi_s2",&Ahi_s2);
    TriggeredTree->Branch("Alo_s1",&Alo_s1);
    TriggeredTree->Branch("Alo_s2",&Alo_s2);

  } 
    
  // // Initialize tracking efficiency
  // // ------------------------------
  // ktTrackEff* tEff=0;
  // if ( DoEfficiencyCorrection) {
  //   tEff = new ktTrackEff();
  //   tEff->SetSysUncertainty(mEffUn);
  //   cout<<endl;
  //   tEff->PrintInfo();
  //   cout<<endl;
  // }
  
  // Initialize analysis class
  // -------------------------
  double max_track_rap = AjParameters::max_track_rap;

  // // DEBUG ONLY
  // evCuts->SetVertexZCut (99999);
  // evCuts->SetMaxEventPtCut (99999);
  // evCuts->SetMaxEventEtCut (99999);
  
  // Special settings for GEANT data
  // -------------------------------
  if ( OutFileName.Contains("GeantMc") ){
    max_track_rap = 7;

    reader.SetProcessTowers ( false );
    evCuts->SetVertexZCut ( 30 );
    // No Additional cuts 
    // evCuts->SetVertexZCut (99999);
    evCuts->SetRefMultCut (0);
    evCuts->SetVertexZDiffCut(999999);

    //test
    evCuts->SetMaxEventPtCut (99999);
    // evCuts->SetMaxEventPtCut ( AjParameters::MaxEventPtCut );	//test 
    evCuts->SetMaxEventEtCut (99999);

    evCuts->SetPVRankingCutOff();		//  Use SetPVRankingCutOff() to turn off vertex ranking cut.  default is OFF

    // Tracks cuts
    TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
    trackCuts->SetDCACut(99999);
    trackCuts->SetMinNFitPointsCut(-1);
    trackCuts->SetFitOverMaxPointsCut(-1);
    trackCuts->SetMaxPtCut (99999);

    // Towers: should be no tower in MC. All (charged or neutral) are handled in track
    TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
    towerCuts->SetMaxEtCut(99999);

    std::cout << "Using these track cuts:" << std::endl;
    std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
    std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
    std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
    std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl;

  // /DEBUG

  }

  AjAnalysis AjA( R,
		  AjParameters::jet_ptmin, AjParameters::jet_ptmax,
		  LeadPtMin, SubLeadPtMin, 
		  max_track_rap,
		  PtConsLo,
		  PtConsHi,
		  AjParameters::dPhiCut,
		  SubtractSoftBg
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

  // For additional QA things
  fastjet::Selector GrabCone = fastjet::SelectorCircle( R );    
  fastjet::Selector NotGhost = !fastjet::SelectorIsPureGhost ();    
  fastjet::Selector OnlyCharged = NotGhost && ( SelectorChargeRange( -3, -1) || SelectorChargeRange( 1, 3) );
  fastjet::Selector OnlyNeutral = NotGhost && SelectorChargeRange( 0, 0);

  // // DEBUG
  // int No4Gev=0;
  TString cname = "";
  UInt_t filehash = 0;
  TStarJetVector* pHT = 0;
  try{
    while ( reader.NextEvent() ) {
      reader.PrintStatus(10);
      
      // event info
      // ----------
      TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();            

      eventid = header->GetEventId();
      runid   = header->GetRunId();
      // For GEANT: Need to devise a runid that's unique but also
      // reproducible to match Geant and GeantMc data.
      if ( OutFileName.Contains("Geant") ) {
	cname = reader.GetInputChain()->GetCurrentFile()->GetName();
	filehash = cname.Hash();
	// while ( filehash > INT_MAX - 100000 ) filehash /= 10; // CRAPCRAPCRAP - Good job destroying significant digits
	while ( filehash > INT_MAX - 100000 ) filehash -= INT_MAX / 4; // some random large number
	if ( filehash < 1000000 ) filehash += 1000001;
	runid = filehash;
	// Sigh. Apparently also need to uniquefy the event id
	// since some are the same in the same file. Grr.
	eventid = reader.GetNOfCurrentEvent();
      }
      
      
      GRefCent = header->GetGReferenceCentrality();
      RefCent = header->GetReferenceCentrality();
      if ( GRefCent<0 || GRefCent>8 ){
	cout << " GRefCent = " << GRefCent << ". Setting to -1." << endl;
	GRefCent=-1;
      }
      if ( RefCent<0 || RefCent>8 ){
	cout << " RefCent = " << RefCent << ". Setting to -1." << endl;
	RefCent=-1;
      }
      
      // For GEANT data
      // --------------
      weight=1;
      if ( InPattern.Contains("Geant") ){
	TString currentfile = reader.GetInputChain()->GetCurrentFile()->GetName();
	weight=LookupXsec ( currentfile );
	if ( fabs(weight-1)<1e-4){
	  cerr << "weight unchanged!" << endl;
	  throw(-1);
	}
	// cout << weight << endl;
      }

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
      //if ( isAuAu ) refmult=header->GetGReferenceMultiplicity();
      if ( isAuAu ) refmult=header->GetProperReferenceMultiplicity();
      
      // Load event
      // ----------
      container = reader.GetOutputContainer();
      
      // Make particle vector
      // --------------------
      particles.clear();

      // // DEBUG: Look for HT trigger
      // bool Has4GevTower=false;
      bool Has54GevTower=false;
      bool Has64GevTower=false;

      // We have no way of artificially creating high towers via BEMC gain variation
      // So leave the triggering untouched!
      
      // Off-line High Tower
      // -------------------
      if ( HTled ) pHT = reader.GetHighTower();
      
      // MANUAL HT cut for Geant MC data
      // -------------------------------
      bool HasManualHighTower=true;
      if ( OutFileName.Contains("GeantMc") ) HasManualHighTower=false;
      for (int ip = 0; ip<container->GetEntries() ; ++ip ){
      	sv = container->Get(ip);  // Note that TStarJetVector  contains more info, such as charge;

	if (sv->GetCharge()==0 && sv->Pt() > 5.4 ) Has54GevTower=true;
	if (sv->GetCharge()==0 && sv->Pt() > 6.4 ) Has64GevTower=true;
	
	// MANUAL HT cut for Geant MC data
	// -------------------------------
	if ( OutFileName.Contains("GeantMc")
	     && sv->GetCharge()==0
	     && fabs(sv->Eta())<1.0
	     && sv->Pt()>ManualHtCut
	     ) {
	  cout << sv->Pt() << "  " << sv->Phi() << "  " << sv->Eta() << "  " << sv->GetCharge() << endl;
	  HasManualHighTower=true;
	  if ( HTled ) pHT = sv;
	}

	// GAIN calibration uncertainty
	// ----------------------------
	if (sv->GetCharge()==0 ) (*sv) *= fTowScale; // for systematics

	// pT RESOLUTION
	// -------------
      	if ( sv->GetCharge()!=0  ) {
	  float ptscaler=1.0;
	  float pt, psig;
	  switch ( IntTrackResShift ){
	  case 0 :
	    // do nothing
	    break;
	  case 1 :
	    // FALLTHROUGH    
	  case -1 :
	    ptscaler=TrackPtScaler->Eval(sv->Pt());
	    break;
	  case 10 :
	    pt=sv->Pt();
	    if ( pt<1e-4)  break;
	    psig= pt*TrackPtSigma->Eval(pt);
	    TrackPtRandom->SetParameters( pt, psig);
	    TrackPtRandom->SetRange(pt-5*psig, pt+5*psig);
	    ptscaler = TrackPtRandom->GetRandom() / pt;
	    break;
	  default :
	    cerr << "IntTrackResShift = " << IntTrackResShift << " not supported." <<endl;
	    return -1;
	  }

	  // cout << sv->Pt() << "  --> ";
	  (*sv) *= ptscaler;
	  // cout << sv->Pt() << endl;;
	}
	
      	pj=MakePseudoJet( sv );
	pj.set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge() ) );

	// Tracks:
      	if ( sv->GetCharge()!=0  ) {
	  // EFFICIENCY uncertainty
	  // ----------------------
	  Double_t mran=gRandom->Uniform(0,1);
      	  if (mran<EffScale)  {
      	    particles.push_back ( pj );
      	  }
	  
      	} else { // no charge 
      	  particles.push_back ( pj );
      	}	      
      }

      if ( !HasManualHighTower) {
	// cerr << "Skipped due to manual ht cut" << endl;
	continue;
      }
      
      // if ( pHT )
      // 	// if ( pHT->Pt() < 5.4 ) 
      // 	cout << pHT->Pt() << endl;
	
      // Run analysis
      // ------------
      int ret;      
      j1 = TLorentzVector();
      j2 = TLorentzVector();
      jm1 = TLorentzVector();
      jm2 = TLorentzVector();
      j1s1 = TLorentzVector();
      j1s2 = TLorentzVector();
      j2s1 = TLorentzVector();
      j2s2 = TLorentzVector();
      jm1s1 = TLorentzVector();
      jm1s2 = TLorentzVector();
      jm2s1 = TLorentzVector();
      jm2s2 = TLorentzVector();

      gjm1 = TLorentzVector();
      gjm2 = TLorentzVector();
      aj_lo=-999;
      aj_hi=-999;
      zg1=zg2=zgm1=zgm2=0;

      ret =AjA.AnalyzeAndFill( particles, 0,refmult, 
			       UnmatchedAJ_hi, AJ_hi, AJ_lo,
			       UnmatchedNoFabsAJ_hi, NoFabsAJ_hi, NoFabsAJ_lo,

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
	    
      // if ( ret>0 && refmult>268 ) {
      if ( ret>0 ){
	if ( ( InPattern.Contains("NPE18")  && RefCent>5 ) || 
	     ( InPattern.Contains("NPE25")  && refmult>279 ) ||
	     ( InPattern.Contains("Clean")  && refmult>268 ) ||
	     ( InPattern.Contains("AuAu14") && GRefCent>5 ) ||
	     !isAuAu
	     ) {
	  HT54InTop20->Fill( Has54GevTower );
	  HT64InTop20->Fill( Has64GevTower );
	  
	  csize->Fill(AjA.GetLoConstituents().size());
	  // DEBUG CONSTSUB: Can't remember user info :-/
	  vector<PseudoJet> chargedconsts = OnlyCharged (AjA.GetLoConstituents());
	  for (int i =0; i< chargedconsts.size(); ++i ){	  
	    Chargedptphieta->Fill( chargedconsts.at(i).pt(), chargedconsts.at(i).phi(), chargedconsts.at(i).eta(), weight );
	  }
	  vector<PseudoJet> neutralconsts = OnlyNeutral (AjA.GetLoConstituents());
	  for (int i =0; i< neutralconsts.size(); ++i ){	  
	    Neutralptphieta->Fill( neutralconsts.at(i).pt(), neutralconsts.at(i).phi(), neutralconsts.at(i).eta(), weight );
	  }
	  // /DEBUG CONSTSUB: Can't remember user info :-/

	  if ( DiJetsLo.size()==2 && fabs(DiJetsHi.at(0).eta())<0.6 && fabs(DiJetsLo.at(0).eta())<0.6 ){
	    vector<PseudoJet> j1consts = DiJetsHi.at(0).constituents();
	    for (int i =0; i< j1consts.size(); ++i ){
	      j1ptphieta->Fill(j1consts.at(i).pt(), j1consts.at(i).phi(), j1consts.at(i).eta(), weight );
	    }
	    vector<PseudoJet> jm1consts = DiJetsLo.at(0).constituents();
	    for (int i =0; i< jm1consts.size(); ++i ){
	      jm1ptphieta->Fill(jm1consts.at(i).pt(), jm1consts.at(i).phi(), jm1consts.at(i).eta(), weight );
	    }
	  }
	}
      }
      
      // Check for matching? yes, for now.
      // cout << DiJetsHi.size() << "  " << DiJetsLo.size() << endl;
      if ( DiJetsLo.size()==2 ){
	
	j1 = MakeTLorentzVector( DiJetsHi.at(0) );
	j2 = MakeTLorentzVector( DiJetsHi.at(1) );
	jm1 = MakeTLorentzVector( DiJetsLo.at(0) );
	jm2 = MakeTLorentzVector( DiJetsLo.at(1) );

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

	aj_hi=AjA.CalcAj( DiJetsHi );
	aj_lo=AjA.CalcAj( DiJetsLo );

	// For Elke
	PseudoJet& pj1 = DiJetsHi.at(0);
	PseudoJet& pj2 = DiJetsHi.at(1);
	PseudoJet& pjm1 = DiJetsLo.at(0);
	PseudoJet& pjm2 = DiJetsLo.at(1);
		
	// DEBUG CONSTSUB: Can't remember user info :-/
	// hNConstChHiLead->Fill( OnlyCharged(pj1.constituents()).size(), refmult);
	// hNConstChHiSubLead->Fill( OnlyCharged(pj2.constituents()).size(), refmult);
	// cout << pjm1.constituents().size() << endl; 
	// cout << OnlyCharged(pjm1.constituents()).size() << endl; return 0;
	// hNConstChLoLead->Fill( OnlyCharged(pjm1.constituents()).size(), refmult);
	// hNConstChLoSubLead->Fill( OnlyCharged(pjm2.constituents()).size(), refmult);

	// hNConstNeHiLead->Fill( OnlyNeutral(pj1.constituents()).size(), refmult);
	// hNConstNeHiSubLead->Fill( OnlyNeutral(pj2.constituents()).size(), refmult);
	// hNConstNeLoLead->Fill( OnlyNeutral(pjm1.constituents()).size(), refmult);
	// hNConstNeLoSubLead->Fill( OnlyNeutral(pjm2.constituents()).size(), refmult);
	// /DEBUG CONSTSUB: Can't remember user info :-/

	// save matching percentage (number of shared constituents)
	MatchPerc1=0;
	MatchLoss1=0;
	vector <PseudoJet> j1consts = NotGhost( pj1.constituents() );
	vector <PseudoJet> jm1consts = NotGhost( pjm1.constituents() );
	if ( j1consts.size()<1 ){
	  cerr << "Empty jet?" << endl;
	  return -1;
	}
	for ( vector<PseudoJet>::iterator c = j1consts.begin(); c != j1consts.end(); ++c ){
	  bool found=false;
	  for ( vector<PseudoJet>::iterator cm = jm1consts.begin(); cm != jm1consts.end(); ++cm ){
	    if ( have_same_momentum ( *c, *cm ) ){
	      // if ( pjm1.contains ( *c ) ){
	      MatchLoss1+=1.0;
	      found=true;
	      break;
	    } else {
	      // if ( (*c - *cm).p()<0.05 ) 
	      // 	cout << "However... " << (*c - *cm).p() << endl;
	    } 
	  }
	  if ( !found ) {
	    hPtMissed1->Fill( c->pt(), refmult );
	    hDRMissed1->Fill( c->delta_R( j1 ), refmult );
	    hvDRMissed1->Fill( c->delta_R( jm1 ), refmult );
	  }
	}
	MatchPerc1 = MatchLoss1 / j1consts.size();
	MatchLoss1 = int(j1consts.size()) - MatchLoss1;

	MatchPerc2=0;
	MatchLoss2=0;

	vector <PseudoJet> j2consts = NotGhost( pj2.constituents() );
	vector <PseudoJet> jm2consts = NotGhost( pjm2.constituents() );
	if ( j2consts.size()<1 ){
	  cerr << "Empty jet?" << endl;
	  return -1;
	}
	for ( vector<PseudoJet>::iterator c = j2consts.begin(); c != j2consts.end(); ++c ){
	  bool found=false;
	  for ( vector<PseudoJet>::iterator cm = jm2consts.begin(); cm != jm2consts.end(); ++cm ){
	    if ( have_same_momentum ( *c, *cm ) ){
	      // if ( pjm2.contains ( *c ) ){
	      MatchLoss2+=1.0;
	      found=true;
	      break;
	    } else {
	      // if ( (*c - *cm).p()<0.05 ) 
	      // 	cout << "However... " << (*c - *cm).p() << endl;
	    } 
	  }
	  if (!found){
	    // THIS DOESN"T WORK FOR CONSTITUENT SUBTRRACTION
	    // cout << "Huh. " << c->plain_distance( j2 ) << endl;
	    hPtMissed2->Fill( c->pt(), refmult );
	    hDRMissed2->Fill( c->delta_R( j2 ), refmult );
	    hvDRMissed2->Fill( c->delta_R( jm2 ), refmult );
	  }
	}
	MatchPerc2 = MatchLoss2 / j2consts.size();
	MatchLoss2 = j2consts.size() - MatchLoss2;

	if ( refmult >= 269 ){
	  hrho->Fill(rho) ;
	  hrhoerr->Fill(rhoerr) ;
	  hrhoHi->Fill(rhoHi) ;
	  vector<PseudoJet> lojets = fastjet::sorted_by_pt( selector_bkgd ( AjA.GetJAlo()->inclusive_jets() ) );
	  // DEBUG CONSTSUB: One or more of this composite jet's pieces does not support area?? :-/
	  // j1area=DiJetsLo.at(0).area();
	  // j2area=DiJetsLo.at(1).area();
	  
	  // hj1area->Fill(DiJetsLo.at(0).area());
	  // hj2area->Fill(DiJetsLo.at(1).area());
	  // for ( int i=2; i <lojets.size() ; ++ i ){
	  //   hrestarea->Fill(lojets.at(i).area());
	  // }
	  // /DEBUG CONSTSUB: One or more of this composite jet's pieces does not support area?? :-/
	  
	  // cout << DiJetsLo.at(0).pt()<< "  "  << jm1.Pt() << "  " << lojets.at(0).pt() << endl;
	  nRestJ=0;
	  for ( int i=0; i< int ( lojets.size() ) -2 ; ++i ){
	    if ( lojets.at(i+2).pt() < 1e-4 ) continue;
	    // DEBUG CONSTSUB: One or more of this composite jet's pieces does not support area?? :-/
	    // RestArea[i] = lojets.at(i+2).area();
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
      
	if ( DoSubjets
	     && 
	     ( AjCutDir ==0 || 
	       ( AjCutDir==1  && aj_hi > AjCut ) ||
	       ( AjCutDir==-1 && aj_hi < AjCut ) 
	       )
	     ){
	  // ---------------
	  // Subjet analysis
	  // ---------------
	  double z_cut = 0.10;
	  double beta  = 0.0;
	  
	  // RecursiveSymmetryCutBase::_verbose=true;
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
	    // sd.set_input_jet_is_subtracted( true );
	    sd.set_input_jet_is_subtracted( false );
	  }
	  
	  PseudoJet sd_jet = sd( DiJetsHi.at(0) );
	  if ( sd_jet == 0){
	    cout <<  " FOREGROUND Original Jet, HI:   " << DiJetsHi.at(0) << endl;
	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	  } else {
	    zg1=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	    // cout << "Hi, lead: " << sd_jet.pt() << endl;
	    // cout << zg1 << endl;
	    if ( sd_jet.pieces().size()>1 ){
	      j1s1=MakeTLorentzVector ( sd_jet.pieces().at(0) );
	      j1s2=MakeTLorentzVector ( sd_jet.pieces().at(1) );
	    }
	  }
	  
	  sd_jet = sd( DiJetsHi.at(1) );
	  if ( sd_jet == 0){
	    cout <<  " FOREGROUND Original sub-leading Jet, HI:   " << DiJetsHi.at(1) << endl;
	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	  } else {
	    zg2=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	    // cout << "Hi, sublead: " << sd_jet.pt() << endl;
	    // cout << zg2 << endl;
	    if ( sd_jet.pieces().size()>1 ){
	      j2s1=MakeTLorentzVector ( sd_jet.pieces().at(0) );
	      j2s2=MakeTLorentzVector ( sd_jet.pieces().at(1) );
	    }
	  }
	  
	  // Lo cut jets
	  Subtractor* BackgroundSubtractorLo = AjA.GetJAlo()->GetBackgroundSubtractor();
	  if ( BackgroundSubtractorLo ){
	    sd.set_subtractor( BackgroundSubtractorLo );
	    // sd.set_input_jet_is_subtracted( true );
	    sd.set_input_jet_is_subtracted( false );
	  }
	  
	  sd_jet = sd( DiJetsLo.at(0) );
	  if ( sd_jet == 0){
	    cout <<  " FOREGROUND Original Jet, LO:   " << DiJetsLo.at(0) << endl;
	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	  } else {
	    // cout << "Lo, lead: " << sd_jet.pt() << endl;
	    zgm1=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	    gjm1 = MakeTLorentzVector( sd_jet );
	    cout << "Pt: " << gjm1.Pt() << "  " << jm1.Pt() << endl;
	    if ( gjm1.Pt() > 1.2* jm1.Pt() ){
	      cout << " ================================== " << endl;
	      cout << "Pt: " << gjm1.Pt() << "  " << jm1.Pt() << endl;
	      cout << "Orig Area: " << DiJetsLo.at(0).area()<< endl;
	      float rho=AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	      cout << "Orig Pt: " << "  " << jm1.Pt() + rho*DiJetsLo.at(0).area()<< endl;
	      // cout << "Subjet Pt: " << sd_jet.pieces().at(0).pt() << "  " << sd_jet.pieces().at(1).pt() << endl;
	      // cout << "Subjet Area: " << sd_jet.pieces().at(0).area() << "  " << sd_jet.pieces().at(1).area() << endl;
	      // return -1;
	    }
	    if ( sd_jet.pieces().size()>1 ){
	      jm1s1=MakeTLorentzVector ( sd_jet.pieces().at(0) );
	      jm1s2=MakeTLorentzVector ( sd_jet.pieces().at(1) );
	    }
	  }

	  sd_jet = sd( DiJetsLo.at(1) );
	  if ( sd_jet == 0){
	    cout <<  " FOREGROUND Original sub-leading Jet, LO:   " << DiJetsLo.at(1) << endl;
	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	  } else {
	    // cout << "Lo, sublead: " << sd_jet.pt() << endl;
	    zgm2=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	    gjm2 = MakeTLorentzVector( sd_jet );
	    if ( sd_jet.pieces().size()>1 ){
	      jm2s1=MakeTLorentzVector ( sd_jet.pieces().at(0) );
	      jm2s2=MakeTLorentzVector ( sd_jet.pieces().at(1) );
	    }
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

	// if ( GRefCent>8 ){
	//   cout << " WTF?! GRefCent = " << GRefCent << endl;
	// }

	// DEBUG: Count ht matching in 0-20%
	if ( refmult >= 269 || !isAuAu ){
	  TStarJetVector* tmpHT = reader.GetHighTower();
	  if ( tmpHT ) {
	    if ( IsMatched( DiJetsHi.at( 0 ), *tmpHT, R ) ){
	      cout << "HT in leading jet" << endl;
	    } else if ( IsMatched( DiJetsHi.at( 1 ), *tmpHT, R ) ){
	      cout << "HT in subleading jet" << endl;
	    } else {
	      cout << "HT outside jets." << endl;
	    }
	  }
	}

	if ( refmult >= 269 )  cout << " Filling ResultTree" << endl;
	ResultTree->Fill();
      } // DijetsLo.size()==2
      
      // quick debug
      if ( AjA.Has10Gev && refmult >= 269 ){
	vector<PseudoJet> JAhiResult = AjA.GetJAhiResult();	
	hLeadPtInTop20->Fill( JAhiResult.at(0).pt() );
	if (JAhiResult.size()>1){
	  hSubLeadPtInTop20->Fill( JAhiResult.at(1).pt() );
	}	
      }

      // Save the full event for embedding if there's at least one 10 GeV jet
      // -------------------------------------------------------------------
      // ALSO save the full event for Au+Au HT events that qualify for Eta Cone
      if (SaveFullEvents){
	FullEvent.Clear();
	TriggerJet.Clear();
	TriggerJetLo.Clear();
	GroomedTriggerJetLo.Clear();
	AwayJet.Clear();
	AwayJetLo.Clear();
	GroomedAwayJetLo.Clear();
	zgtrighi = zgtriglo = 0;
	zgawayhi = zgawaylo = 0;

	Thi_s1=TLorentzVector();
	Thi_s2=TLorentzVector();
	Tlo_s1=TLorentzVector();
	Tlo_s2=TLorentzVector();

	Ahi_s1=TLorentzVector();
	Ahi_s2=TLorentzVector();
	Alo_s1=TLorentzVector();
	Alo_s2=TLorentzVector();

	
	if ( ( InPattern.Contains("pp") && AjA.Has10Gev)  ||
	     // ( InPattern.Contains("pp") && AjA.GetJAhiResult().size()>0 )  || // DEBUG
	     ( InPattern.Contains("Geant") && AjA.Has10Gev)  ||
	     ( !(InPattern.Contains("pp")) && PrepEtaConeEmbedding && DiJetsLo.size()==2) ) { 
	  int j=0;
	  int k=0;
	  for ( int i = 0; i<particles.size() ; ++i ){
	    // if ( particles.at(i).pt() >50 ) { 
	    //   cerr << " =====> " <<particles.at(i).pt()
	    // 	   << "  " << particles.at(i).phi()
	    // 	   << "  " << particles.at(i).eta()
	    // 	   << "  " << particles.at(i).user_info<JetAnalysisUserInfo>().GetQuarkCharge() << endl;
	    //   if ( ret>0 ) {
	    // 	cerr << " AND ret == " << ret << endl;
	    //   }
	    // } 
	    TLorentzVector lv = MakeTLorentzVector( particles.at(i) );
	    new (FullEvent[i])   TStarJetVector ( lv  );
	    // Multiply by 1.1 to avoid rounding to integer problems
	    ((TStarJetVector*)FullEvent[i])->SetCharge( int ( 1.1* particles.at(i).user_info<JetAnalysisUserInfo>().GetQuarkCharge()/3.0) );
	    // cout << ((TStarJetVector*)FullEvent[i])->GetCharge() << "  " << particles.at(i).user_info<JetAnalysisUserInfo>().GetQuarkCharge() << endl;
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
	      // sd.set_input_jet_is_subtracted( true );
	      sd.set_input_jet_is_subtracted( false );
	    }
	    
	    // near-side
	    PseudoJet sd_jet = sd( TrigHi );
	    if ( sd_jet == 0){
	      cout <<  " TriggerJet:   " << TrigHi << endl;
	      cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	    } else {
	      zgtrighi=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	      if (zgtrighi>0) {
		Thi_s1=MakeTLorentzVector ( sd_jet.pieces().at(0) );
		Thi_s2=MakeTLorentzVector ( sd_jet.pieces().at(1) );
	      }
	    }
	    
	    // away-side
	    if ( AwayHi.pt()>1e-4 ){
	      sd_jet = sd( AwayHi );
	      if ( sd_jet == 0){
		cout <<  "AwayJet:   " << AwayHi << endl;
		cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	      } else {
		zgawayhi=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
		if (zgawayhi>0) {
		  Ahi_s1=MakeTLorentzVector ( sd_jet.pieces().at(0) );
		  Ahi_s2=MakeTLorentzVector ( sd_jet.pieces().at(1) );
		}
	      }
	    }

	    // Lo cut jets
	    Subtractor* BackgroundSubtractorLo = AjA.GetJAlo()->GetBackgroundSubtractor();
	    if ( BackgroundSubtractorLo ){
	      sd.set_subtractor( BackgroundSubtractorLo );
	      // sd.set_input_jet_is_subtracted( true );
	      sd.set_input_jet_is_subtracted( false );
	    }
	    
	    // near-side
	    sd_jet = sd( TrigLo );
	    if ( sd_jet == 0){
	      cout <<  " Trigger Jet, soft match:   " << TrigLo << endl;
	      cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	    } else {
	      zgtriglo=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
	      if (zgtriglo>0) {
		Tlo_s1=MakeTLorentzVector ( sd_jet.pieces().at(0) );
		Tlo_s2=MakeTLorentzVector ( sd_jet.pieces().at(1) );
		new ( GroomedTriggerJetLo[0] ) TLorentzVector ( MakeTLorentzVector( sd_jet ) );
	      }
	    }

	    // away-side
	    if ( AwayLo.pt()>1e-4 ){
	      sd_jet = sd( AwayLo );
	      if ( sd_jet == 0){
		cout <<  "AwayJet, soft match:   " << AwayLo << endl;
		cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
	      } else {
		zgawaylo=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
		if (zgawaylo>0) {
		  Alo_s1=MakeTLorentzVector ( sd_jet.pieces().at(0) );
		  Alo_s2=MakeTLorentzVector ( sd_jet.pieces().at(1) );
		  new ( GroomedAwayJetLo[0] ) TLorentzVector ( MakeTLorentzVector( sd_jet ) ) ;
		}
	      }
	    }
	    
	  } // DoSubjets

	  TriggeredTree->Fill();
	} // has Trigger
      } // SaveFullEvents
      
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
	  // NoFabsEtaShiftAJ_lo->Fill( (pt1 - pt2) / (pt1 + pt2), refmult );
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
	    pt1 = pJ1->Pt() + RC1.pt();
	    pt2 = pJ2->Pt() + RC2.pt();
	    
	    if ( AjA.GetJAhi()->GetBackgroundSubtractor() ){
	      pt1 -= TMath::Pi()*R*R * AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	      pt2 -= TMath::Pi()*R*R * AjA.GetJAlo()->GetBackgroundEstimator()->rho() ;
	    }
	    EtaShiftAJ_lo->Fill( fabs ( (pt1 - pt2) / (pt1 + pt2) ), refmult );
	    NoFabsEtaShiftAJ_lo->Fill( (pt1 - pt2) / (pt1 + pt2), refmult );
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
  reader.PrintStatus();
  
  cout << "##################################################################" << endl;
  
  Long64_t nEventsUsed=reader.GetNOfEvents();  

  // Close up shop
  // -------------
  fout->cd();
  reader.GetHadronicResult()->Write();
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



