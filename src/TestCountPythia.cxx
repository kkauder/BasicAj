/** @file TestCountPythia.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Count how many jets actually survive the 2 GeV cut
    @date Mar 04, 2015
*/

#include "AjAnalysis.hh"
#include "AjParameters.hh"

#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <THnSparse.h>
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
    - Set up vector of pythia and MC events
    - Set up output histos and tree
*/
Int_t main(int argc, char **argv) {
  
  // Load input
  // ----------
  TString InputName = "PythiaAndMc.root";
  TChain* InputChain = new TChain("tree");
  InputChain->Add(InputName);
  assert ( InputChain->GetEntries()>0 && "Something went wrong loading input.");
  
  TClonesArray* pPythia = new TClonesArray("TStarJetVector");
  InputChain->GetBranch("PythiaParticles")->SetAutoDelete(kFALSE);
  InputChain->SetBranchAddress("PythiaParticles", &pPythia);

  TClonesArray* pMc = new TClonesArray("TStarJetVector");
  InputChain->GetBranch("McParticles")->SetAutoDelete(kFALSE);
  InputChain->SetBranchAddress("McParticles", &pMc);

  TClonesArray* pHardPartons = new TClonesArray("TLorentzVector");
  InputChain->GetBranch("HardPartons")->SetAutoDelete(kFALSE);
  InputChain->SetBranchAddress("HardPartons", &pHardPartons);

  int eventid;
  InputChain->SetBranchAddress("eventid", &eventid);
  int runid;
  InputChain->SetBranchAddress("runid", &runid);

  // Output
  // ------
  TString OutFileName = "TestCountPythia.root";
  TFile* fout = new TFile( OutFileName, "RECREATE");

  // jet resolution parameter
  // ------------------------
  float R = 0.4;

  // soft constituent cut
  // --------------------
  float PtConsLo=0.2;

  cout << " ################################################### " << endl;
  cout << "Triggering with R=" << R << endl;
  cout << "Low pT cut =" << PtConsLo << endl;
  cout << " ################################################### " << endl;
  
  // Histograms
  // ----------
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);
  
  TH2D* Leading20Orig = new TH2D( "Leading20Orig","p_{T} with constituent cut vs. hard parton;p_{T}^{true};p_{T}^{Rec}|p_{T}^{Cut}>2 GeV", 100, 10, 60, 120, 0, 60 );
  TH2D* Leading20Jets = new TH2D( "Leading20Jets","p_{T} with vs. without constituent cut;p_{T}^{Cor}|p_{T}^{Cut}>0.2 GeV;p_{T}^{Rec}|p_{T}^{Cut}>2 GeV", 100, 10, 60, 120, 0, 60 );
  TH2D* All20Jets = new TH2D( "All20Jets","p_{T} with vs. without constituent cut;p_{T}^{Cor}|p_{T}^{Cut}>0.2 GeV;p_{T}^{Rec}|p_{T}^{Cut}>2 GeV", 100, 10, 60, 120, 0, 60 );
  // Int_t bins[4] = {100, 100, 120, 100};
  // Double_t xmin[4] = {10,  0,  0, 0 };
  // Double_t xmax[4] = {60, 50, 60, 50};
  // THnSparseF* DiJets = new THnSparseF( "DiJets","Dijets", 4, bins, xmin, xmax );


  // // Save results
  // // ------------
  // TTree* ResultTree=new TTree("ResultTree","Result Jets");
  // TLorentzVector j1, j2, jm1, jm2;
  // ResultTree->Branch("j1",&j1);
  // ResultTree->Branch("j2",&j2);
  // ResultTree->Branch("jm1",&jm1);
  // ResultTree->Branch("jm2",&jm2);
  // double refmult=0; // Really an int, but may change if using refmultcorr
  // float rho;
  // float rhoerr;
  // float j1area;
  // float j2area;

  // ResultTree->Branch("eventid",&eventid, "eventid/i");
  // ResultTree->Branch("runid",&runid, "runid/i");
  // ResultTree->Branch("refmult",&refmult, "refmult/d");
  // ResultTree->Branch("rho",&rho, "rho/f");
  // ResultTree->Branch("rhoerr",&rhoerr, "rhoerr/f");
  // ResultTree->Branch("j1area",&j1area, "j1area/f");
  // ResultTree->Branch("j2area",&j2area, "j2area/f");

  // // area and pT of all remaining jets (those used for rho)
  // static const Int_t kmaxJ=500; // max # of jets
  // int nRestJ=0;
  // float RestArea[kmaxJ];
  // float RestPt[kmaxJ];
  // ResultTree->Branch("nRestJ",&nRestJ, "nRestJ/i");
  // ResultTree->Branch("RestArea",RestArea, "RestArea[nRestJ]/f");
  // ResultTree->Branch("RestPt",  RestPt,   "RestPt[nRestJ]/f");

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

  const double jet_ptmin = 0.0;    ///< Min jet pT
  // const double jet_ptmin = 10.0;    ///< Min jet pT
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

  cout << "Performing analysis." << endl;

  JetAnalyzer* pJAlo=0;
  JetAnalyzer* pJAhi=0;
  vector<PseudoJet> OrigPartons;
  vector<PseudoJet> JAloResult;
  vector<PseudoJet> JAhiResult;

  vector<PseudoJet> pLo;
  vector<PseudoJet> pHi;

  
  // Cycle through events
  // --------------------  
  int nEvents=InputChain->GetEntries();
  // nEvents=100;
  for ( int ev=0; ev<nEvents  ; ++ev ){    
    if ( !(ev%1000) ) cerr << "Event " << ev << " / " << nEvents << endl;
    
    if ( pJAlo ) { delete pJAlo; pJAlo=0;}
    if ( pJAhi ) { delete pJAhi; pJAhi=0;}

    particles.clear();
    InputChain->GetEntry(ev);
    pLo.clear();
    pHi.clear();

    OrigPartons.clear();
    JAloResult.clear();
    JAhiResult.clear();    

    // Hard Partons
    // ------------
    for ( int i=0; i < pHardPartons->GetEntries() ; ++i ){
      TLorentzVector* pL = (TLorentzVector*) pHardPartons->At(i);
      if ( pL->Pt() < LeadPtMin ) continue;
      if ( fabs ( pL->Eta() ) > max_rap  ) continue;
      // if ( fabs ( pL->Eta() ) > 0.9*max_rap  ) continue; // DEBUG
      OrigPartons.push_back( MakePseudoJet ( pL ) );
    }

    // If we have no chance of finding anything, skip
    // ----------------------------------------------
    if ( OrigPartons.size() ==0 ) continue;
  
    // Jet particles
    // -------------
    for ( int i=0; i < pPythia->GetEntries() ; ++i ){
      particles.push_back( MakePseudoJet ( (TLorentzVector*)pPythia->At(i)) );
    }
    
      
    // // Fill up with pseudo Au+Au
    // // -------------------------
    // for ( int i=0; i < pMc->GetEntries() ; ++i ){
    //   particles.push_back( MakePseudoJet ( (TLorentzVector*)pMc->At(i)) );
    // }
    
    // Select particles to perform analysis on
    // ---------------------------------------
    pLo = slo( particles );
    pHi = shi( particles );

    // // find jets with SOFT constituents
    // // ----------------------------------------------
    // Selector selector_bkgd = fastjet::SelectorAbsRapMax( max_rap ) * (!fastjet::SelectorNHardest(2));
    // pJAlo = new JetAnalyzer( pLo, jet_def, area_def, selector_bkgd ); // WITH background subtraction
    // JetAnalyzer& JAlo = *pJAlo;
    // fastjet::Subtractor* BackgroundSubtractor =  JAlo.GetBackgroundSubtractor();
    // JAloResult = fastjet::sorted_by_pt( (*BackgroundSubtractor)( sjet( JAlo.inclusive_jets() ) ) );

    // // // Found anything?
    // // if ( JAloResult.size()==0 ) continue;
    // // if ( JAloResult.at(0).pt() < LeadPtMin ) continue;

    // Find HARD constituent jet(s)
    // -------------------------------
    pJAhi = new JetAnalyzer( pHi, jet_def ); // NO background subtraction
    JetAnalyzer& JAhi = *pJAhi;
    JAhiResult = fastjet::sorted_by_pt( sjet ( JAhi.inclusive_jets() ) );


    // fastjet::Selector SelectClose = fastjet::SelectorCircle( R );
    // for ( int i=0; i<JAloResult.size(); ++i ){
    //   float softpt=JAloResult.at(i).pt();
    //   if ( softpt < LeadPtMin ) break;
      
    //   SelectClose.set_reference( JAloResult.at(i) );
    //   vector<PseudoJet> Matched = sorted_by_pt(SelectClose( JAhiResult ));

    //   float matchedpt=0;
    //   if ( Matched.size()>0 ){
    // 	// if ( Matched.at(0).pt() > LeadPtMin ) matchedpt = Matched.at(0).pt();
    // 	matchedpt = Matched.at(0).pt();
    //   }
    //   All20Jets->Fill( softpt, matchedpt );
    //   if ( i==0 )Leading20Jets->Fill( softpt, matchedpt );
	
    // }    

    // Match leading jet to original hard parton
    // -----------------------------------------
    // fastjet::Selector SelectClose = fastjet::SelectorCircle( R );
    float origpt=0;
    float hardpt=0;    
    for ( int i=0; i<OrigPartons.size(); ++i ){
      origpt=OrigPartons.at(i).pt();
      cout << i << " Orig eta: " << OrigPartons.at(i).eta()  << endl;
      cout << i << " Orig phi: " << OrigPartons.at(i).phi() << endl;
      cout << i << " Orig pt: " << OrigPartons.at(i).pt() << endl;
      if ( JAhiResult.size()>0 ) {
	cout << " Leading eta: " << JAhiResult.at(0).eta()  << endl;
	cout << " Leading phi: " << JAhiResult.at(0).phi() << endl;
	cout << " Leading pt: " << JAhiResult.at(0).pt() << endl;	
      } else {
	cout << " NOTHING FOUND. "<< endl;
      }
      if ( JAhiResult.size()>0 && OrigPartons.at(i).delta_R( JAhiResult.at(0) ) ){
	hardpt = JAhiResult.at(0).pt();
	cout << "  --> matched. "<< endl;
	// break;
      }
      // SelectClose.set_reference( OrigPartons.at(i) );
      // vector<PseudoJet> Matched = sorted_by_pt(SelectClose( JAhiResult ) );
      // hardpt = Matched.at(0).pt();      
    }
    if ( hardpt<1 && JAhiResult.size()>0 ) {
      cout << " !!! MISSED" << endl << endl;
      return -1;
    }
    Leading20Orig->Fill( origpt, hardpt );
      
  } // nEvents
    

  TH1D* Leading20Tot = (TH1D*) Leading20Jets->ProjectionX("Leading20Tot");
  int Bin20 = Leading20Jets->GetYaxis()->FindBin(LeadPtMin+0.0001);
  TH1D* Leading20Above = (TH1D*) Leading20Jets->ProjectionX("Leading20Above", Bin20, Leading20Jets->GetNbinsY() );
  TH1D* Leading20Ratio = (TH1D*) Leading20Above->Clone("Leading20Ratio");
  Leading20Ratio->Divide( Leading20Tot );
    
  TH1D* All20Tot = (TH1D*) All20Jets->ProjectionX("All20Tot");
  Bin20 = All20Jets->GetYaxis()->FindBin(LeadPtMin+0.0001);
  TH1D* All20Above = (TH1D*) All20Jets->ProjectionX("All20Above", Bin20, All20Jets->GetNbinsY() );
  TH1D* All20Ratio = (TH1D*) All20Above->Clone("All20Ratio");
  All20Ratio->Divide( All20Tot );

  TH1D* OrigLeading20Tot = (TH1D*) Leading20Orig->ProjectionX("OrigLeading20Tot");
  Bin20 = Leading20Orig->GetYaxis()->FindBin(LeadPtMin+0.0001);
  TH1D* OrigLeading20Above = (TH1D*) Leading20Orig->ProjectionX("OrigLeading20Above", Bin20, Leading20Orig->GetNbinsY() );
  TH1D* OrigLeading20Ratio = (TH1D*) OrigLeading20Above->Clone("OrigLeading20Ratio");
  OrigLeading20Ratio->Divide( OrigLeading20Tot );

  // Close up shop
  // -------------
  fout->Write();

  cout << "In " << nEvents << " events, found " << endl
       << nHardDijets << " dijets with constituents above 2 GeV," << endl
       << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
       << " of which " <<  nMatchedDijets << " could be matched." << endl;

  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;
  
  return 0;
  
}
