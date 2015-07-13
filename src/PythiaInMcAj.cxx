/** @file src/PythiaInMcAj.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Similar to ppInAuAuAj, but use mock BG and pythia events
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

  int eventid;
  InputChain->SetBranchAddress("eventid", &eventid);
  int runid;
  InputChain->SetBranchAddress("runid", &runid);

  // Output
  // ------
  TString OutFileName = "AjResults/pythiaInMcAj.root";
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

  cout << " ################################################### " << endl;
  cout << "Triggering with R=" << R << endl;
  cout << "Following to R=" << OtherR << endl;
  cout << "Low pT cut =" << PtConsLo << endl;
  cout << " ################################################### " << endl;

  cout << " ################################################### " << endl;
  cout << "   FastJet random seeds offset by" << randomoff << endl;
  cout << " ################################################### " << endl;
  
  
  // Histograms
  // ----------
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

  // Follow to different R
  TH2D* OtherAJ_lo = new TH2D( "OtherAJ_lo","A_{J} for soft constituent jets with other R ;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  TH2D* OtherLeadPtLoss_lo    = new TH2D( "OtherLeadPtLoss_lo","Leading #Delta p_{T} for soft constituent jets with other R ;A_{J};Refmult;fraction", 120, -10, 50, 800, -0.5, 799.5 );
  TH2D* OtherSubLeadPtLoss_lo = new TH2D( "OtherSubLeadPtLoss_lo","SubLeading #Delta p_{T} for soft constituent jets with other R ;A_{J};Refmult;fraction", 120, -10, 50, 800, -0.5, 799.5 );
	
  // DEBUG histos
  // ------------
  // Find some info on the background
  TH1D* hrho = new TH1D( "hrho","#rho", 240, 0, 120 );
  TH1D* hrhoerr = new TH1D( "hrhoerr","#sigma_{#rho}", 100, 0, 50 );

  // can't compute area_error(), take rms of this instead
  TH1D* hj1area    = new TH1D( "hj1area","j1 area", 320, 0.0, 0.8 );
  TH1D* hj2area    = new TH1D( "hj2area","j2 area", 320, 0.0, 0.8 );
  TH1D* hrestarea    = new TH1D( "hrestarea","restarea", 160, 0.0, 0.8 );

  // Save results
  // ------------
  TTree* ResultTree=new TTree("ResultTree","Result Jets");
  TLorentzVector j1, j2, jm1, jm2;
  ResultTree->Branch("j1",&j1);
  ResultTree->Branch("j2",&j2);
  ResultTree->Branch("jm1",&jm1);
  ResultTree->Branch("jm2",&jm2);
  double refmult=0; // Really an int, but may change if using refmultcorr
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

  // Helpers
  // -------
  vector<PseudoJet> particles;

  int nHardDijets = 0;
  int nCorrespondingLowDijets = 0;
  int nMatchedDijets=0;

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
      particles.push_back( MakePseudoJet ( (TLorentzVector*)pPythia->At(i)) );
    }
    
    // Fill up with pseudo Au+Au
    // -------------------------
    for ( int i=0; i < pMc->GetEntries() ; ++i ){
      particles.push_back( MakePseudoJet ( (TLorentzVector*)pMc->At(i)) );
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
      
    int ret =AjA.AnalyzeAndFill( particles, 0, 0,
				 UnmatchedAJ_hi, AJ_hi, AJ_lo,
				 
				 UnmatchedhPtHi,  hPtHi, hPtLo,
				 UnmatchedhdPtHi, hdPtHi, hdPtLo,
				 hdphiHi, hdphiLo,
				 
				 OtherAJ_lo, OtherLeadPtLoss_lo, OtherSubLeadPtLoss_lo, OtherR
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
    // if ( ret==0 ){
    //   vector<PseudoJet> hi = AjA.GetJAhi()->inclusive_jets();
    //   cerr << "####################" << endl;
    //   if (hi.size() == 0 ){
    // 	cerr << "Nothing found" << endl;
    //   } else {
    // 	cerr << (sorted_by_pt( hi )).at(0).pt() << "   " << (sorted_by_pt( hi )).at(0).eta() <<  endl;
    //   }
    // }
    
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
      

      ResultTree->Fill();
    } // DiJetsLo.size()==2        
  } // nEvents
    

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
