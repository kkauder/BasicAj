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

#include "FastJet3.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/ClusterSequencePassiveArea.hh"
#include "fastjet/ClusterSequenceActiveArea.hh"
#include "fastjet/ClusterSequenceActiveAreaExplicitGhosts.hh"
#include "fastjet/Selector.hh"

#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include "fastjet/tools/Subtractor.hh"
#include "fastjet/tools/Filter.hh"
#include "fastjet/FunctionOfPseudoJet.hh"

using namespace std;
using namespace fastjet;

int main ( int argc, const char** argv ) {

  // Load input
  // ----------
  TString InputName = "PythiaAndMc.root";
  TChain* InputChain = new TChain("tree");
  InputChain->Add(InputName);
  assert ( InputChain->GetEntries()>0 && "Something went wrong loading input.");

  TClonesArray* pPythia = new TClonesArray("TLorentzVector");
  InputChain->GetBranch("PythiaParticles")->SetAutoDelete(kFALSE);
  InputChain->SetBranchAddress("PythiaParticles", &pPythia);

  TClonesArray* pMc = new TClonesArray("TLorentzVector");
  InputChain->GetBranch("McParticles")->SetAutoDelete(kFALSE);
  InputChain->SetBranchAddress("McParticles", &pMc);

  int eventid;
  InputChain->SetBranchAddress("eventid", &eventid);
  int runid;
  InputChain->SetBranchAddress("runid", &runid);

  // Output
  // ------
  TString OutFileName = "AjResults/BBpythiaInMcAj.root";
  int randomoff = 0;
  if ( argc>1 ){
    randomoff = TString( argv[1][0] ).Atoi(); // defaults to zero
    if ( randomoff>0 ){
      TString dirname = "AjResults/rndm";
      dirname+=randomoff;
      OutFileName.ReplaceAll("AjResults",dirname);
    }
  }
  randomoff *= 10000000;
  TFile* fout = new TFile( OutFileName, "RECREATE");

  // jet resolution parameter
  // ------------------------
  float R = 0.4;

  // soft constituent cut
  // --------------------
  float PtConsLo=0.2;

  // Histograms
  // ----------
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  TH2D* UnmatchedAJ_hi = new TH2D( "UnmatchedAJ_hi","Unmatched A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_hi = new TH2D( "AJ_hi","A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* AJ_lo = new TH2D( "AJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  
  // Helpers
  // -------
  vector<PseudoJet> particles;

  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;

  // Parameters
  // ----------
  const int ghost_repeat = 1;
  const double ghost_area = 0.01;

  const double jet_ptmin = 10.0;    ///< Min jet pT
  const double jet_ptmax = 1000.0;   ///< DEBUG
  
  const double LeadPtMin=20.0;      ///< Min leading jet pT 
  const double SubLeadPtMin=10.0;   ///< Min sub-leading jet pT 
  const double dPhiCut = 0.4;       ///< Dijet acceptance angle,  |&phi;1 - &phi;2 -&pi;| < dPhiCut
  
  const double max_track_rap = 1.0; ///< Constituent &eta; acceptance
  const double PtConsHi=2.0;        ///< High constituent pT cut  

  // derived rapidity cuts
  // ---------------------
  double max_rap      = max_track_rap-R;
  double ghost_maxrap = max_rap + 2.0 * R;

  // Constituent selectors
  // ---------------------
  Selector select_track_rap = fastjet::SelectorAbsRapMax(max_track_rap);
  Selector select_lopt      = fastjet::SelectorPtMin( PtConsLo );
  Selector select_hipt      = fastjet::SelectorPtMin( PtConsHi );
  
  Selector slo = select_track_rap * select_lopt;
  Selector shi = select_track_rap * select_hipt;

  // Jet candidate selectors
  // -----------------------
  Selector select_jet_rap     = fastjet::SelectorAbsRapMax(max_rap);
  Selector select_jet_pt_min  = fastjet::SelectorPtMin( jet_ptmin );
  Selector select_jet_pt_max  = fastjet::SelectorPtMax( jet_ptmax );
  Selector sjet = select_jet_rap && select_jet_pt_min && select_jet_pt_max;
  
  // Choose a jet and area definition
  // --------------------------------
  JetDefinition jet_def = fastjet::JetDefinition(fastjet::antikt_algorithm, R);
  
  // create an area definition for the clustering
  //----------------------------------------------------------
  // ghosts should go up to the acceptance of the detector or
  // (with infinite acceptance) at least 2R beyond the region
  // where you plan to investigate jets.
  GhostedAreaSpec area_spec = fastjet::GhostedAreaSpec( ghost_maxrap, ghost_repeat, ghost_area );
  AreaDefinition  area_def = fastjet::AreaDefinition(fastjet::active_area_explicit_ghosts, area_spec);

  // For random seed
  // ---------------
  fastjet::GhostedAreaSpec TmpArea; // for access to static random seed
  vector<int> SeedStatus;

  cout << "Performing analysis." << endl;

  // Cycle through events
  // --------------------  
  int nEvents=InputChain->GetEntries();
  //nEvents=100;
  for ( int ev=0; ev<nEvents  ; ++ev ){
    if ( !(ev%1000) ) cerr << "Event " << ev << " / " << nEvents << endl;

    particles.clear();
    InputChain->GetEntry(ev);

    // Jet particles
    // -------------
    for ( int i=0; i < pPythia->GetEntries() ; ++i ){
      particles.push_back( *( (TLorentzVector*) pPythia->At(i)) );
    }
    
    // Fill up with pseudo Au+Au
    // -------------------------
    for ( int i=0; i < pMc->GetEntries() ; ++i ){
      particles.push_back( *( (TLorentzVector*) pMc->At(i)) );
    }

    // random seed for fastjet
    // -----------------------
    TmpArea.get_random_status(SeedStatus);
    if ( SeedStatus.size() !=2 ) {
      throw std::string("SeedStatus.size() !=2");
      return -1;
    }
    SeedStatus.at(0) = runid   + randomoff;
    SeedStatus.at(1) = eventid + randomoff;
    TmpArea.set_random_status(SeedStatus);

    // Background selector
    // -------------------
    Selector selector_bkgd = fastjet::SelectorAbsRapMax( max_rap ) * (!fastjet::SelectorNHardest(2));
    
    // Analysis
    // --------
    vector<PseudoJet> pLo = slo( particles );
    vector<PseudoJet> pHi = shi( particles );

    // find high constituent pT jets
    // -----------------------------
    ClusterSequence csaHi ( pLo, jet_def );
    vector<PseudoJet> HiResult = fastjet::sorted_by_pt( sjet ( csaHi.inclusive_jets() ) );

    if ( HiResult.size() < 2 )                 {     continue; }
    if ( HiResult.at(0).pt() < LeadPtMin )     {     continue; }
    if ( HiResult.at(1).pt() < SubLeadPtMin )  {     continue; }

    // back to back?
    // -------------
    vector<PseudoJet> DiJetsHi;
    DiJetsHi.at(0) = HiResult.at(0);
    DiJetsHi.at(1) = HiResult.at(1);
    // if ( fabs ( DiJetsHi.at(0).delta_R ( DiJetsHi.at(1) ) - TMath::Pi() ) 
    
    if ( DiJetsHi.size() == 0 ) {
      // std::cout << " NO dijet found" << std::endl;
      return 0;
    }
    assert ( DiJetsHi.size() == 2 && "SelectorDijets returned impossible number of Dijets." );  



  }

  
  return 0;
}
  
