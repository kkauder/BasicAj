/** @file PythiaAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Run Aj on pythia data
    @date Mar 04, 2015
*/

#include "AjAnalysis.hh"
#include "AjParameters.hh"

#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TChain.h>
#include <tBranch.h>
#include <TMath.h>
#include <TRandom.h>

#include <iostream>
#include <cmath>

using namespace std;
using namespace fastjet;

/** 
    - Set up vector of pythia events
    - Set up output histos and tree
    - For each Pythia event, perform Aj Analysis
*/
int main () {
  TString triggertype="";
  // TString triggertype="qq";
  // TString triggertype="gg";
  gRandom->SetSeed(1);

  // File name
  // ---------
  TString OutFileName = "AjResults/pythiaAj.root";
  if ( triggertype== "qq" || triggertype== "gg" ) OutFileName.ReplaceAll(".root", TString("_")+triggertype+TString(".root") );
  TFile* fout = new TFile( OutFileName, "RECREATE");

  // jet resolution parameter
  // ------------------------
  float R = 0.4;
  // Follow to different R
  // float OtherR = 0.2;   // will be set to 0.4 if we trigger on 0.2, i.e., we can follow in either direction
  if ( OutFileName.Contains ("R0.2") ){
    R=0.2;
    // OtherR=0.4;    
  }

  // soft constituent cut
  // --------------------
  float PtConsLo=0.2;
  if ( OutFileName.Contains ("Pt1") ){
    PtConsLo=1.0;
  }


  // Load pythia Jets
  // ----------------
  TString PythiaName = "pytest.root";
  TChain* PythiaJets = new TChain("tree");
  PythiaJets->Add(PythiaName);
  assert ( PythiaJets->GetEntries()>0 && "Something went wrong loading the Pythia jets.");
  
  // Throw them into convenient vectors
  // ----------------------------------
  TClonesArray* pFullEvent = new TClonesArray("TLorentzVector");
  PythiaJets->GetBranch("Particles")->SetAutoDelete(kFALSE);
  PythiaJets->SetBranchAddress("Particles", &pFullEvent);

  TClonesArray* pHardPartons= new TClonesArray("TLorentzVector");
  PythiaJets->GetBranch("HardPartons")->SetAutoDelete(kFALSE);
  PythiaJets->SetBranchAddress("HardPartons", &pHardPartons);
  
  TClonesArray* pHardPartonNames= new TClonesArray("TObjString");
  PythiaJets->GetBranch("HardPartonNames")->SetAutoDelete(kFALSE);
  PythiaJets->SetBranchAddress("HardPartonNames", &pHardPartonNames);
  
  vector< vector<PseudoJet> > FullPythiaEvent;
  vector<PseudoJet> CurrentPythiaEvent;
  TLorentzVector* lv;

  vector< vector<PseudoJet> > TriggerPartons;
  vector<PseudoJet> CurrentTriggerPartons;
  
  Int_t NPythiaEvents=PythiaJets->GetEntries();
  // Int_t NPythiaEvents=10000;
  cout << "Filling pythia vector" << endl;
  for ( Long64_t pythiaEv = 0; pythiaEv< NPythiaEvents ; ++ pythiaEv ){
    if ( !(pythiaEv%1000) ) cout << "Working on " << pythiaEv << " / " << NPythiaEvents << endl;
    PythiaJets->GetEntry(pythiaEv);
    CurrentPythiaEvent.clear();
    for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
      lv = (TLorentzVector*) pFullEvent->At(i);
      // Ensure kinematic similarity
      if ( lv->Pt()< PtConsLo ) continue;
      if ( fabs( lv->Eta()>1) ) continue;
      CurrentPythiaEvent.push_back( PseudoJet (*lv ) );
    }

    // Save original hard partons
    // --------------------------
    CurrentTriggerPartons.clear();
    for ( int i=0 ; i<pHardPartons->GetEntries() ; ++i ){
      lv = (TLorentzVector*) pHardPartons->At(i);
      PseudoJet pj = PseudoJet (*lv );

      // flavor info
      TString& s = ((TObjString*)(pHardPartonNames->At(i)))->String();
      int qcharge=-999;
      if ( s=="g" ) qcharge = 0;
      
      if ( s(0)=='u' || s(0)=='c' || s(0)=='t' ) qcharge  = 2;
      if ( s(0)=='d' || s(0)=='s' || s(0)=='b' ) qcharge = -1;
      if ( s.Contains("bar") ) qcharge*=-1;
      
      if ( abs ( qcharge ) >3 ) cout<< s << endl;

      pj.set_user_info ( new JetAnalysisUserInfo( qcharge ) );
      CurrentTriggerPartons.push_back( pj );
    }

    // quick and dirty trigger type selection
    if ( triggertype == "qq" || triggertype == "gg" ) {
      if ( CurrentTriggerPartons.size()!=2 ) continue;
      int aqcharge0= abs (CurrentTriggerPartons.at(0).user_info<JetAnalysisUserInfo>().GetQuarkCharge());
      int aqcharge1= abs (CurrentTriggerPartons.at(1).user_info<JetAnalysisUserInfo>().GetQuarkCharge());
      if ( aqcharge0 >2 || aqcharge1 >2 ) continue;
    
      if ( triggertype == "qq" && !(aqcharge0>0  && aqcharge1> 0) ) continue;
      if ( triggertype == "gg" && !(aqcharge0==0 && aqcharge1==0) ) continue;
    }    

    FullPythiaEvent.push_back ( CurrentPythiaEvent );
    TriggerPartons.push_back( CurrentTriggerPartons );  
  }

  // Histograms
  // --------------------

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

  TH1D* hdphiHi = new TH1D( "hdphiHi","#Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* hdphiLo = new TH1D( "hdphiLo","#Delta#phi for soft constituent jets", 200, -2, 2 );
  
  // Helpers
  // -------
  vector<PseudoJet> particles;
  vector<PseudoJet> PythiaParticles;
  TStarJetVectorContainer<TStarJetVector>* container;

  const double M = 0.13957;  // set pi+ mass
  PseudoJet v; // helper
  
  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  Long64_t nJetsUsed=0;

  // Initialize analysis class
  // -------------------------
  AjAnalysis AjA( R,
		  AjParameters::jet_ptmin, AjParameters::jet_ptmax,
		  AjParameters::LeadPtMin, AjParameters::SubLeadPtMin, 
		  AjParameters::max_track_rap,
		  PtConsLo,
		  AjParameters::PtConsHi,
		  AjParameters::dPhiCut
		  );  


  cout << "Performing analysis." << endl;
  // Cycle through pythia jets
  // ---------------------
  for ( int pythiai =0; pythiai< FullPythiaEvent.size() ; ++pythiai ){

    // Base pythia vector
    // --------------
    PythiaParticles.clear();
    for ( int i=0; i < FullPythiaEvent.at(pythiai).size() ; ++i ){
      PythiaParticles.push_back( FullPythiaEvent.at( pythiai).at(i) );
    }

    particles=PythiaParticles;      

    // Run analysis
    // ------------
    double refmult=0;
    int ret =AjA.AnalyzeAndFill( particles, 0,refmult, 
				 UnmatchedAJ_hi, AJ_hi, AJ_lo,
				 
				 UnmatchedhPtHi,  hPtHi, hPtLo,
				 UnmatchedhdPtHi, hdPtHi, hdPtLo,
				 hdphiHi, hdphiLo
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
    } //  switch (ret)
    nJetsUsed++;

    if ( !(nJetsUsed%1000) ) cout << "Used " << nJetsUsed << " / " << FullPythiaEvent.size() << endl;
  } // pythiai< FullPythiaEvent.size()
  

  // Close up shop
  // -------------
  fout->Write();

  cout << "Analyzed " << nJetsUsed << " events and found " << endl
       << nHardDijets << " dijets with constituents above 2 GeV," << endl
       << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
       << " of which " <<  nMatchedDijets << " could be matched." << endl;

  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;
  
  return 0;
  
}
