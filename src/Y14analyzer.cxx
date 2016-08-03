#include "AjAnalysis.hh"
#include "AjParameters.hh"
#include "ktTrackEff.hh"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <THnSparse.h>
#include <TFile.h>

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TParameter.h>
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

#include "fastjet/contrib/Recluster.hh"
#include "fastjet/contrib/SoftDrop.hh"

using namespace std;
using namespace fastjet;
using namespace fastjet::contrib;


int main ( int argc, const char** argv ) {

  TString InPattern = "Data/HaddedAuAu14/AuAu14Pico_96-100_5.root";
  TString OutFileName = "Y14details.root";
  TString TriggerName = "All";
  // TString TriggerName = "HT3";
  switch ( argc) {
  case 1 :
    break;
  case 3:
    InPattern = argv[1];
    OutFileName = argv[2];
    break;
  case 4:
    InPattern = argv[1];
    OutFileName = argv[2];
    TriggerName = argv[3];
    break;
  default :
    return -1;
    break;
  }

  TString ChainName  = "JetTree";
  TChain* chain = new TChain( ChainName );
  chain->Add( InPattern );
  cout << "Running over " << chain->GetEntries() << " events." << endl;

  double RefMultCut = 0;
  TStarJetPicoReader reader;
  reader.SetInputChain (chain);

  // Event and track selection
  // -------------------------
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  evCuts->SetTriggerSelection( TriggerName ); //All, MB, HT, pp, ppHT, ppJP
  // Additional cuts 
  // evCuts->SetVertexZCut (AjParameters::VzCut);
  // evCuts->SetRefMultCut ( RefMultCut );
  // evCuts->SetVertexZDiffCut( AjParameters::VzDiffCut );
  // evCuts->SetMaxEventPtCut ( AjParameters::MaxEventPtCut );
  // evCuts->SetMaxEventEtCut ( AjParameters::MaxEventEtCut );

  // Tracks cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  trackCuts->SetDCACut( AjParameters::DcaCut );
  trackCuts->SetMinNFitPointsCut( AjParameters::NMinFit );
  trackCuts->SetFitOverMaxPointsCut( AjParameters::FitOverMaxPointsCut );
  // trackCuts->SetMaxPtCut ( AjParameters::MaxTrackPt );

  std::cout << "Using these track cuts:" << std::endl;
  std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
  std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
  std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
  std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl;
  
  // Towers
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  // towerCuts->SetMaxEtCut(AjParameters::MaxEtCut);

  std::cout << "Using these tower cuts:" << std::endl;
  std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl;
  std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl;
  
  // V0s: Turn off
  reader.SetProcessV0s(false);

  // Additional
  reader.SetApplyFractionHadronicCorrection(kTRUE);
  reader.SetFractionHadronicCorrection(0.9999);
  reader.SetRejectTowerElectrons( kFALSE );
  // TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();

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

  TH1D* RefCent = new TH1D("RefCent", "RefCent",12, -1.5,10.5);
  TH1D* GRefCent = new TH1D("GRefCent", "GRefCent",12, -1.5,10.5);
  TH1D* RefMult = new TH1D("RefMult", "RefMult", 800, -0.5, 799.5 );
  TH1D* GRefMult = new TH1D("GRefMult", "GRefMult", 800, -0.5, 799.5 );
  
  TH1D* Vz    = new TH1D("Vz", "Vz",100, -50, 50 );
  TH1D* VpdVz = new TH1D("VpdVz", "VpdVz",100, -50, 50 );
  TH1D* DeltaVz = new TH1D("DeltaVz", "Vz - VpdVz",100, -50, 50 );
  
  // int GRefCent;
  // int RefCent;
  
  // Cycle through events
  // --------------------
  vector<PseudoJet> particles;
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv; // TLorentzVector* would be sufficient. 

  Long64_t nEvents=-1; // -1 for all
  //nEvents=100;
  reader.Init(nEvents);

  TParameter<double> DoubleParam;
  TParameter<Long64_t> LongParam;

  Long64_t nNoHT=0;
  Long64_t nHT1=0;
  Long64_t nHT2=0;
  Long64_t nHT3=0;
  Long64_t nHT2Not3=0;
  Long64_t nHT3Not2=0;
  Long64_t nHT2And3=0;
  Long64_t nvpdmb=0;
  Long64_t nmon=0;
  Long64_t nspecial=0;
  Long64_t nnonprod=0;
  Long64_t nunknown=0;

  // First Run 15076101,
  // Last Run 15167014
  // First Day: 076
  // Last Day: 167
  
  TH1D* nHT2_perRun = new TH1D("nHT2_perRun","nHT2 perRun", 92000, 75999.5, 167999.5 );
  TH1D* nHT3_perRun = new TH1D("nHT3_perRun","nHT3 perRun", 92000, 75999.5, 167999.5 );
  TH1D* nAccepted_perRun = new TH1D("nAccepted_perRun","nAccepted perRun", 92000, 75999.5, 167999.5 );
  TH1D* nHT2_perDay = new TH1D("nHT2_perDay","nHT2 perDay", 92, 75.5, 167.5 );
  TH1D* nHT3_perDay = new TH1D("nHT3_perDay","nHT3 perDay", 92, 75.5, 167.5 );
  TH1D* nAccepted_perDay = new TH1D("nAccepted_perDay","nAccepted perDay", 92, 75.5, 167.5 );
    
  int GRefMult_perRun_bins[2] = { 92000, 800 };
  double GRefMult_perRun_min[2] = {75999.5,-0.5};
  double GRefMult_perRun_max[2] = {167999.5, 799.5};
  THnSparseD* GRefMult_perRun = new THnSparseD("GRefMult_perRun","GRefMult perRun",2,GRefMult_perRun_bins, GRefMult_perRun_min, GRefMult_perRun_max);

  int GRefMult_perDay_bins[2] = { 92, 800 };
  double GRefMult_perDay_min[2] = {75.5,-0.5};
  double GRefMult_perDay_max[2] = {167.5, 799.5};
  THnSparseD* GRefMult_perDay = new THnSparseD("GRefMult_perDay","GRefMult perDay",2,GRefMult_perDay_bins, GRefMult_perDay_min, GRefMult_perDay_max);
  double sparsevalue[2];
  

  Long64_t nChain = chain->GetEntries();
  Long64_t nAccepted = 0;
  while ( reader.NextEvent() ) {
    reader.PrintStatus(10);
    
    // event info
    // ----------
    TStarJetPicoEventHeader* header = reader.GetEvent()->GetHeader();
    int shortrunid = header->GetRunId() - 15000000;
    int daynr      = shortrunid / 1000;
        
    nAccepted++;
    nAccepted_perRun->Fill( shortrunid );
    nAccepted_perDay->Fill( daynr );

    // Triggers
    int nTriggerIds = header->GetNOfTriggerIds();
    const TArrayI* TriggerIdArray = header->GetTriggerIdArray();
    bool isht1=false;
    bool isht2=false;
    bool isht3=false;
    bool isvpdmb=false;
    bool ismon=false;
    bool isspecial=false;
    bool isnonprod=false;
    bool isunknown=false;

    for ( int i=0; i< nTriggerIds; ++i ){
      int mTrigId=TriggerIdArray->At(i);
      
      switch (mTrigId){
      case 450201:
      case 450211:
	// HT1*VPDMB-30
	isht1=true;
	break;
      case 450202:
      case 450212:
	// HT2*VPDMB-30
	isht2=true;
	break;
      case 450203:
      case 450213:
	// HT3*VPDMB-30
	isht3=true;
	break;
      case 450010:
      case 450020:
	//VPDMB-30
	// break;
      case 450008:
      case 450018:
	//VPDMB-5
	// break;
      case 450014:
      case 450024:
	//VPDMB-5-nobsmd
	// break;
      case 450005:
      case 450015:
      case 450025:
	//VPDMB-5-p-nobsmd

	isvpdmb=true;
	break;
      case 450011:
      case 450021:
	//MB-mon
	// break;
      case 450012:
      case 450022:
	//ZDC-mon
	// break;
      case 450013:
      case 450023:
	//VPD-ZDC-novtx-mon
	ismon=true;
	break;
	
      default:
	if ( mTrigId >= 450600 && mTrigId<450700 ){
	  // muon, mtd, ...
	  isspecial=true;
	  break;
	} 
	if ( mTrigId < 1000 ){
	  // non-production triggers
	  isnonprod=true;
	  break;
	}
	isunknown=true;
	cout << "mTrigId = " << mTrigId << endl;
	break;
      }

    } // TriggerIds
    bool isht = isht1 || isht2 || isht3;
    
    if ( !isht ) nNoHT++;
    if ( isht1 ) nHT1++;
    if ( isht2 ) {
      nHT2++;
      nHT2_perRun->Fill( shortrunid );
      nHT2_perDay->Fill( daynr );
    }
    if ( isht3 ) {
      nHT3++;
      nHT3_perRun->Fill( shortrunid );
      nHT3_perDay->Fill( daynr );
    }

    if ( isht2 && isht3 )     nHT2And3++;
    if ( isht2 && !isht3 )    nHT2Not3++;
    if ( !isht2 && isht3 )    nHT3Not2++;

    if ( isvpdmb )   nvpdmb++;
    if ( ismon )     nmon++;
    if ( isspecial ) nspecial++;
    if ( isnonprod ) nnonprod++;
    if ( isunknown ) nunknown++;
    
    if ( !isht && !isvpdmb && !ismon && !isspecial && !isnonprod && !isunknown ){
      cerr << "No trigger at all??" << endl;
      return -1;
    }

    // Refmult and Vz
    int vGRefCent = header->GetGReferenceCentrality();
    int vRefCent  = header->GetReferenceCentrality();
    if ( vGRefCent<0 ) vGRefCent = -1;
    if ( vGRefCent>8 ) vGRefCent = 10;
    if (  vRefCent<0 )  vRefCent = -1;
    if (  vRefCent>8 )  vRefCent = 10;

    RefCent->Fill  ( vRefCent );
    GRefCent->Fill ( vGRefCent );
    RefMult->Fill  ( header->GetReferenceMultiplicity() );
    GRefMult->Fill ( header->GetGReferenceMultiplicity() );

    sparsevalue[1] =  header->GetGReferenceMultiplicity();
    sparsevalue[0] =  shortrunid;
    GRefMult_perRun->Fill ( sparsevalue, shortrunid );
    sparsevalue[0] =  daynr;
    GRefMult_perDay->Fill ( sparsevalue, daynr );

    Vz->Fill( header->GetPrimaryVertexZ() );
    VpdVz->Fill( header->GetVpdVz() );
    DeltaVz->Fill( header->GetPrimaryVertexZ() - header->GetVpdVz() );
    
  }

  fout->cd();
  fout->Write();
  GRefMult_perRun->Write();
  GRefMult_perDay->Write();
  LongParam.SetVal(nChain); LongParam.Write("nChain");
  LongParam.SetVal(nAccepted); LongParam.Write("nAccepted");
 
  LongParam.SetVal(nNoHT); LongParam.Write("nNoHT");
  LongParam.SetVal(nHT1); LongParam.Write("nHT1");
  LongParam.SetVal(nHT2); LongParam.Write("nHT2");
  LongParam.SetVal(nHT3); LongParam.Write("nHT3");
  LongParam.SetVal(nHT2Not3); LongParam.Write("nHT2Not3");
  LongParam.SetVal(nHT3Not2); LongParam.Write("nHT3Not2");
  LongParam.SetVal(nHT2And3); LongParam.Write("nHT2And3");
  LongParam.SetVal(nvpdmb); LongParam.Write("nvpdmb");
  LongParam.SetVal(nmon); LongParam.Write("nmon");
  LongParam.SetVal(nspecial); LongParam.Write("nspecial");
  LongParam.SetVal(nnonprod); LongParam.Write("nnonprod");
  LongParam.SetVal(nunknown); LongParam.Write("nunknown");
  

  cout << "Done." <<endl;
  return 0;
}
