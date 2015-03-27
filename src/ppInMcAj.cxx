/** @file ppInMcAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Similar to ppInAuAuAj, but use mock BG instead
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

#include "TClonesArray.h"

#include <iostream>
#include <cmath>

using namespace std;
using namespace fastjet;

/** 
    - Set up vector of pp jets
    - Create or load mock "Au+Au" data parameters (spectrum, multiplicity, ...)
    - Set up output histos and tree
    - Since we can create infinite data, we don't need to go through 
      the same mixing pool hoops as in true Au+Au
    - For each pp event, perform Aj Analysis with as many pseudo AuAu events as required.
*/
int main () {
  // How many times to use every pp jet
  // ----------------------------------
  Int_t nMix=3;

  gRandom->SetSeed(1);

  // Load pp Jets
  // ------------
  TString ppAjName = "AjResults/ppAj.root";
  TChain* ppJets = new TChain("TriggeredTree");
  ppJets->Add(ppAjName);
  assert ( ppJets->GetEntries()>0 && "Something went wrong loading the pp jets.");

  TClonesArray* pFullEvent = new TClonesArray("TLorentzVector");
  ppJets->GetBranch("FullEvent")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("FullEvent", &pFullEvent);

  // Throw them into convenient vectors
  // ----------------------------------
  vector< vector<PseudoJet> > FullPpEvent;
  vector<PseudoJet> CurrentPpEvent;
  for ( Long64_t ppEv = 0; ppEv< ppJets->GetEntries() ; ++ ppEv ){
    ppJets->GetEntry(ppEv);
    CurrentPpEvent.clear();
    for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
      CurrentPpEvent.push_back( MakePseudoJet( (TLorentzVector*) pFullEvent->At(i) ) );
    }
    FullPpEvent.push_back ( CurrentPpEvent );
  }

  // Make or load AuAu info
  // ----------------------
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  TString AuAuInfoFile="AjResults/AuAuPhiEtaPt.root";
  TFile* AuAuData = new TFile (AuAuInfoFile, "READ");
  TH3D* AuAuPhiEtaPt=0;
  TH1D* AuAuN=0;
  TH1D* AuAuPt=0;
  TH1D* AuAuSoftPt=0;
  TF1* fExpo=0;  
  
  if ( AuAuData->IsOpen() ) {
    cout << "Getting Au+Au data from " << AuAuData->GetName() << endl;
    AuAuPhiEtaPt = (TH3D*) AuAuData->Get("AuAuPhiEtaPt");
    AuAuN        = (TH1D*) AuAuData->Get("AuAuN");
    AuAuPt       = (TH1D*) AuAuData->Get("AuAuPt");
    AuAuSoftPt   = (TH1D*) AuAuData->Get("AuAuSoftPt");
    fExpo        = (TF1*) AuAuData->Get("fExpo");
  } else {
    AuAuData = new TFile(AuAuInfoFile,"RECREATE");
    cout << "Saving Au+Au data to " << AuAuData->GetName();
    
    // Load and set up Au+Au tree, just for the spectrum.
    // --------------------------------------------------
    TStarJetPicoDefinitions::SetDebugLevel(0);

    TString ChainName  = "JetTree";
    TString TriggerName = "MB";
    TString InFileName = "Data/AuAuMB_0_20/*.root";

    TChain* chain = new TChain( ChainName );
    chain->Add( InFileName );
    TStarJetPicoReader reader = SetupReader( chain, TriggerName );
    
    // Output Histos
    // -------------
    AuAuPhiEtaPt = new TH3D("AuAuPhiEtaPt","AuAuPhiEtaPt",20, -pi, pi, 20, -1, 1, 100, 0.2, 10);
    AuAuN = new TH1D("AuAuN","AuAuN",5000, -0.5, 5000-0.5);
    
    TStarJetVectorContainer<TStarJetVector>* container;
    TLorentzVector* lv;
    
    Long64_t nEvents=-1; // -1 for all
    // Long64_t nEvents=10;
    reader.Init(nEvents);
    while ( reader.NextEvent() ) {    
      reader.PrintStatus(10);

      // Load event
      // ----------
      container = reader.GetOutputContainer();
      AuAuN->Fill(container->GetEntries());
      
      for (int ip = 0; ip<container->GetEntries() ; ++ip ){
	lv = container->Get(ip);
	AuAuPhiEtaPt->Fill(JetAnalyzer::phimod2pi( lv->Phi() ),lv->Eta(),lv->Pt());
      }
    }
    
    // Smooth pT spectrum
    // ------------------
    fExpo = new TF1("fExpo" ,"expo",0.2,10);
    AuAuPt = (TH1D*) AuAuPhiEtaPt->ProjectionZ("AuAuPt");
    AuAuPt->Fit("fExpo","","",1,4);
    // fExpo->Write(); // Doesn't work for some reason. Do it differently:
    AuAuPt->GetFunction("fExpo")->Write();

    // TF1 *fLevy = new TF1("fLevy" ,"[0]*x*([1]-1)*([1]-2)/[1]/ [2] / ( [1]*[2]+[3]*([1]-2)  ) * pow( 1+ ( TMath::Sqrt(x*x+[3]*3)-[3] ) / [1]/[2] , -[1])"
    // 			 ,0.2,10);
    // fLevy->SetParameters( 4e6, 14.9, 1.6e-1);//, 0.2);
    // AuAuPt->Fit("fLevy","","",0.5,6);
    // AuAuPt->GetFunction("fLevy")->Write();

    // Histo with soft pT spectrum only
    // --------------------------------
    // Reason: Want to draw from realistic low pT spectrum
    // while using expo for the high end. Easiest way to accomplish that is to replace
    // the high pT bin contents with fit values.
    double CutoffPoint=3.8; // a little below the fit range for smoother transition
    AuAuSoftPt = (TH1D*) AuAuPt->Clone("AuAuSoftPt");
    AuAuSoftPt->Reset();
    for ( int i=1; i<=AuAuSoftPt->GetNbinsX() ; ++ i ){
      double pt = AuAuSoftPt->GetXaxis()->GetBinCenter(i); // shouldn't really use the center, but whatever
      if ( pt < CutoffPoint ){
	AuAuSoftPt->SetBinContent( i, AuAuPt->GetBinContent( i ) );
      } else {
	AuAuSoftPt->SetBinContent( i, fExpo->Eval( pt ) );
      }
    }
    AuAuPt->SetLineColor(kRed);
    AuAuSoftPt->SetLineColor(kGreen+1);

    // Check what we did here
    // ----------------------
    // Paste into root
    // new TCanvas;
    // gPad->SetLogy();
    // AuAuPt->DrawCopy();
    // AuAuSoftPt->DrawCopy("same");
    // fExpo->DrawCopy("same");
    
    // Save
    // ----
    AuAuData->Write();
  }
  
  TH1D* AuAuPhi = AuAuPhiEtaPt->ProjectionX("AuAuPhi");
  TH1D* AuAuEta = AuAuPhiEtaPt->ProjectionY("AuAuEta");

  // Files and histograms
  // --------------------
  TString OutFileName = "AjResults/ppInMcAj.root";
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH2D* UnmatchedhPtHi = new TH2D( "UnmatchedhPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* hPtHi = new TH2D( "hPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
  TH2D* hPtLo = new TH2D( "hPtLo","p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c];p_{T}^{sub} [GeV/c]", 100, 10 , 60, 100, 0, 50 );
    
  TH1D* UnmatchedhdPtHi = new TH1D( "UnmatchedhdPtHi","#Delta p_{T} for unmatched hard constituent jets", 120, -10, 50 );
  TH1D* hdPtHi = new TH1D( "hdPtHi","#Delta p_{T} for hard constituent jets", 120, -10, 50 );
  TH1D* hdPtLo = new TH1D( "hdPtLo","#Delta p_{T} for soft constituent jets", 120, -10, 50 );

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
  vector<PseudoJet> PPparticles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TLorentzVector* lv;

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
		  UnmatchedhdPtHi, hdPtHi, hdPtLo,
		  hdphiHi, hdphiLo,
		  UnmatchedAJ_hi, AJ_hi, AJ_lo,
		  UsedEventsHiPhiEtaPt, UsedEventsLoPhiEtaPt
		  );



  // Cycle through pp jets
  // ---------------------
  for ( int ppi =0; ppi< FullPpEvent.size() ; ++ppi ){

    // Base pp vector
    // --------------
    PPparticles.clear();
    for ( int i=0; i < FullPpEvent.at(ppi).size() ; ++i ){
      PPparticles.push_back( FullPpEvent.at( ppi).at(i) );
    }

    // Fill up with pseudo Au+Au
    // -------------------------
    for ( int mix=0; mix<nMix ; ++mix ){
      particles=PPparticles;      
      int nAu = AuAuN->GetRandom(); // How many AuAu particles?
      for ( int n = 0; n<nAu ; ++n ){
	float phi = AuAuPhi->GetRandom();
	float eta = AuAuEta->GetRandom();
	float pt = AuAuSoftPt->GetRandom();

	float p = pt * TMath::CosH( eta );
	double E = sqrt( p*p + M*M);
	
	v.reset_PtYPhiM( pt, eta, phi, E );
	particles.push_back(v);	
      } // n<nAu

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
      if ( !(nJetsUsed%1000) ) cout << "Embedded " << nJetsUsed << " / " << nMix*FullPpEvent.size() << endl;
    } // mix< nMix
  } // ppi< FullPpEvent.size()
  
  // Scale per used pp event
  // ------------------------
  UsedEventsHiPhiEtaPt->Scale( 1./nJetsUsed ); 
  

  // Close up shop
  // -------------
  fout->Write();

  cout << "Embedded " << nJetsUsed << " events into mock Au+Au and found " << endl
       << nHardDijets << " dijets with constituents above 2 GeV," << endl
       << nCorrespondingLowDijets << " corresponding dijets with constituents above 0.2 GeV," << endl
       << " of which " <<  nMatchedDijets << " could be matched." << endl;

  cout << "Wrote to " << fout->GetName() << endl;
  cout << "Bye." << endl;
  
  return 0;
  
}
