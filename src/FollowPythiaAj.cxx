/** @file FollowPythiaAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Variation of Aj analysis in Pythia.
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
    - Set up vector of pythia events
    - Set up output histos and tree
    - For each Pythia event, perform Aj Analysis
*/
int main ( int argc, const char** argv ) {
  //TString triggertype="";
  //TString triggertype="qq";
  TString triggertype="gg";

  gRandom->SetSeed(1);

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
      if ( lv->Pt()< AjParameters::PtConsLo ) continue;
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

  // Files and histograms
  // --------------------
  TString OutFileName = "AjResults/pythiaFollowAj.root";
  if ( triggertype== "qq" || triggertype== "gg" ) OutFileName.ReplaceAll(".root", TString("_")+triggertype+TString(".root") );
  TFile* fout = new TFile( OutFileName, "RECREATE");
  assert ( fout->IsOpen() );
  cout << " ################################################### " << endl;
  cout << "Writing to: " << fout->GetName() << endl;
  cout << " ################################################### " << endl;
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  float SmallR=0.2; // DEBUG
  float LargeR=0.4;

  TH1D* LeadDeltaPtHi    = new TH1D( "LeadDeltaPtHi",   "Matched, hard constituents, p_T^{Lead} difference between small and large R", 200, -30, 10 );
  TH1D* SubLeadDeltaPtHi = new TH1D( "SubLeadDeltaPtHi","Matched, hard constituents, p_T^{Sub} difference between small and large R", 200, -30, 10 );
  TH1D* LeadDeltaPtLo    = new TH1D( "LeadDeltaPtLo",   "Matched, soft constituents, p_T^{Lead} difference between small and large R", 200, -30, 10 );
  TH1D* SubLeadDeltaPtLo = new TH1D( "SubLeadDeltaPtLo","Matched, soft constituents, p_T^{Sub} difference between small and large R", 200, -30, 10 );
    
  TH2D* SmallUnmatchedhPtHi = new TH2D( "SmallUnmatchedhPtHi","R=0.2, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* SmallhPtHi = new TH2D( "SmallhPtHi","R=0.2, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* SmallhPtLo = new TH2D( "SmallhPtLo","R=0.2, p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* SmallhdphiHi        = new TH1D( "SmallhdphiHi","R=0.2, #Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* SmallhdphiLo        = new TH1D( "SmallhdphiLo","R=0.2, #Delta#phi for soft constituent jets", 200, -2, 2 );

  TH1D* SmallUnmatchedAJ_hi = new TH1D( "SmallUnmatchedAJ_hi","R=0.2, Unmatched A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* SmallAJ_hi = new TH1D( "SmallAJ_hi","R=0.2, A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* SmallAJ_lo = new TH1D( "SmallAJ_lo","R=0.2, A_{J} for soft constituent jets", 40, -0.3, 0.9 );

  TH2D* LargehPtHi = new TH2D( "LargehPtHi","R=0.4, p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* LargehPtLo = new TH2D( "LargehPtLo","R=0.4, p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* LargehdphiHi = new TH1D( "LargehdphiHi","R=0.4, #Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* LargehdphiLo = new TH1D( "LargehdphiLo","R=0.4, #Delta#phi for soft constituent jets", 200, -2, 2 );
  
  TH1D* LargeAJ_hi = new TH1D( "LargeAJ_hi","R=0.4, A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* LargeAJ_lo = new TH1D( "LargeAJ_lo","R=0.4, A_{J} for soft constituent jets", 40, -0.3, 0.9 );

  TH3D* UsedEventsHiPhiEtaPt=new TH3D("UsedEventsHiPhiEtaPt","UsedEventsHiPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.0, 10); // QA
  TH3D* UsedEventsLoPhiEtaPt=new TH3D("UsedEventsLoPhiEtaPt","UsedEventsLoPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.0, 10); // QA  

  TH1D* SmallDeltaAJ_hilo = new TH1D( "SmallDeltaAJ_hilo","Small R, A_J (hard) - A_J (soft);#Delta A_{J}", 100, -1, 1);
  TH1D* LargeDeltaAJ_hilo = new TH1D( "LargeDeltaAJ_hilo","Large R, A_J (hard) - A_J (soft);#Delta A_{J}", 100, -1, 1);
  TH1D* DeltaAJ_hi = new TH1D( "DeltaAJ_hi","Matched, hard constituents, A_J (Small R) - A_J (Large R);#Delta A_{J}", 100, -1, 1);
  TH1D* DeltaAJ_lo = new TH1D( "DeltaAJ_lo","Matched, soft constituents, A_J (Small R) - A_J (Large R);#Delta A_{J}", 100, -1, 1);

  // Helpers
  // -------
  vector<PseudoJet> particles;
  vector<PseudoJet> PythiaParticles;
  TStarJetVectorContainer<TStarJetVector>* container;

  const double M = 0.13957;  // set pi+ mass
  PseudoJet v; // helper
  
  // Initialize analysis class
  // -------------------------
  FollowAjAnalysis AjA( SmallR, LargeR,
			AjParameters::jet_ptmin, AjParameters::jet_ptmax,
			AjParameters::LeadPtMin, AjParameters::SubLeadPtMin, 
			AjParameters::max_track_rap, AjParameters::PtConsLo, AjParameters::PtConsHi,
			AjParameters::dPhiCut,
			LeadDeltaPtHi, SubLeadDeltaPtHi, LeadDeltaPtLo, SubLeadDeltaPtLo,
			SmallUnmatchedhPtHi,  SmallhPtHi, SmallhPtLo,
			SmallhdphiHi, SmallhdphiLo,
			SmallUnmatchedAJ_hi, SmallAJ_hi, SmallAJ_lo,
			LargehPtHi, LargehPtLo,
			LargehdphiHi, LargehdphiLo,
			LargeAJ_hi, LargeAJ_lo,
			UsedEventsHiPhiEtaPt, UsedEventsLoPhiEtaPt,
			SmallDeltaAJ_hilo,  LargeDeltaAJ_hilo,
			DeltaAJ_hi,  DeltaAJ_lo			
			);  
  
  // Cycle through events
  // --------------------  
  Long64_t nJetsUsed=0;
  int nHardDijets = 0;
  int nLargeToSmallHiMatched=0;
  int nLargeToSmallLoMatched=0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;
  
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

    nJetsUsed++;    
    if ( !(nJetsUsed%1000) ) cout << "Used " << nJetsUsed << " / " << FullPythiaEvent.size() << endl;
  } // pythiai< FullPythiaEvent.size()
  
  cout << "##################################################################" << endl;
  
  // Scale per used pythia event
  // ------------------------
  UsedEventsHiPhiEtaPt->Scale( 1./nJetsUsed ); 

  // Close up shop
  // -------------
  fout->Write();

  cout << "Analyzed " << nJetsUsed << " events and found " << endl
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
