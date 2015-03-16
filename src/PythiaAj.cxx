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
  gRandom->SetSeed(1);

  // Load pythia Jets
  // ----------------
  TString PythiaName = "pytest.root";
  TChain* PythiaJets = new TChain("tree");
  PythiaJets->Add(PythiaName);
  assert ( PythiaJets->GetEntries()>0 && "Something went wrong loading the Pythia jets.");
  
  TClonesArray* pFullEvent = new TClonesArray("TLorentzVector");
  PythiaJets->GetBranch("Particles")->SetAutoDelete(kFALSE);
  PythiaJets->SetBranchAddress("Particles", &pFullEvent);

  // Throw them into convenient vectors
  // ----------------------------------
  vector< vector<PseudoJet> > FullPythiaEvent;
  vector<PseudoJet> CurrentPythiaEvent;
  TLorentzVector* lv;
  Int_t NPythiaEvents=PythiaJets->GetEntries();
  // Int_t NPythiaEvents=100000;
  cout << "Filling pythia vector" << endl;
  for ( Long64_t pythiaEv = 0; pythiaEv< NPythiaEvents ; ++ pythiaEv ){
    PythiaJets->GetEntry(pythiaEv);
    CurrentPythiaEvent.clear();
    for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
      lv = (TLorentzVector*) pFullEvent->At(i);
      // Ensure kinematic similarity
      if ( lv->Pt()<0.2 ) continue;
      if ( fabs( lv->Eta()>1) ) continue;
      CurrentPythiaEvent.push_back( MakePseudoJet( lv ) );
    }
    FullPythiaEvent.push_back ( CurrentPythiaEvent );
  }
  

  // Files and histograms
  // --------------------
  TString OutFileName = "AjResults/pythiaAj.root";
  TFile* fout = new TFile( OutFileName, "RECREATE");

  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  TH2D* UnmatchedhPtHi = new TH2D( "UnmatchedhPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* hPtHi = new TH2D( "hPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* hPtLo = new TH2D( "hPtLo","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* hdphiHi = new TH1D( "hdphiHi","#Delta#phi for hard constituent jets", 200, -2, 2 );
  TH1D* hdphiLo = new TH1D( "hdphiLo","#Delta#phi for soft constituent jets", 200, -2, 2 );

  TH1D* UnmatchedAJ_hi = new TH1D( "UnmatchedAJ_hi","Unmatched A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* AJ_hi = new TH1D( "AJ_hi","A_{J} for hard constituent jets", 40, -0.3, 0.9 );
  TH1D* AJ_lo = new TH1D( "AJ_lo","A_{J} for soft constituent jets", 40, -0.3, 0.9 );

  TH3D* UsedEventsHiPhiEtaPt=new TH3D("UsedEventsHiPhiEtaPt","UsedEventsHiPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.0, 10); // QA
  TH3D* UsedEventsLoPhiEtaPt=new TH3D("UsedEventsLoPhiEtaPt","UsedEventsLoPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.0, 10); // QA  
  
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
  AjAnalysis AjA( AjParameters::R, AjParameters::jet_ptmin, AjParameters::jet_ptmax,
		  AjParameters::LeadPtMin, AjParameters::SubLeadPtMin, 
		  AjParameters::max_track_rap, AjParameters::PtConsLo, AjParameters::PtConsHi,
		  AjParameters::dPhiCut,
		  UnmatchedhPtHi,  hPtHi, hPtLo,  
		  hdphiHi, hdphiLo,
		  UnmatchedAJ_hi, AJ_hi, AJ_lo,
		  UsedEventsHiPhiEtaPt, UsedEventsLoPhiEtaPt
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
    int ret =AjA.AnalyzeAndFill( particles );
    
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
  
  // Scale per used pythia event
  // ------------------------
  UsedEventsHiPhiEtaPt->Scale( 1./nJetsUsed ); 
  

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
