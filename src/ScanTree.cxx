#include "AjAnalysis.hh"
#include "AjParameters.hh"

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
#include <vector>
#include <set>
#include <cmath>
#include <exception>

using namespace std;
using namespace fastjet;

int main ( ){
  // TString ChainPath="Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root";
  TString ChainPath="Data/HaddedAuAu11picoMB/AuAu11PicoMB_0.root";

  TString OutFileName="Y7PhiEtaPt.root";
  int CentMin=0;
  int RefMultCut=0;
  TString TriggerName = "MB";

  if ( ChainPath.Contains ("newpicoDstCentralMB") ){
    RefMultCut=269;
    CentMin=0;
    OutFileName="Y7PhiEtaPt.root";
  } else {
    RefMultCut=0;
    CentMin=5;
    OutFileName="Y11PhiEtaPt.root";
    // DEBUG: KLUDGE! Need to circumvent missing trigger IDs in MB
    TriggerName = "All";
  }

  TString ChainName  = "JetTree";
  TChain* chain = new TChain( ChainName );
  chain->Add( ChainPath );

  TStarJetPicoReader reader;
  reader.SetInputChain (chain);

  // Event and track selection
  // -------------------------
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  evCuts->SetTriggerSelection( TriggerName ); //All, MB, HT, pp, ppHT, ppJP
  // Additional cuts 
  evCuts->SetVertexZCut ( AjParameters::VzCut);
  evCuts->SetRefMultCut ( RefMultCut );
  evCuts->SetReferenceCentralityCut (  CentMin, 8 );
  evCuts->SetVertexZDiffCut( AjParameters::VzDiffCut );
  evCuts->SetMaxEventPtCut ( AjParameters::MaxEventPtCut );
  evCuts->SetMaxEventEtCut ( AjParameters::MaxEventEtCut );

  // Tracks cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  trackCuts->SetDCACut( AjParameters::DcaCut );
  trackCuts->SetMinNFitPointsCut( AjParameters::NMinFit );
  trackCuts->SetFitOverMaxPointsCut( AjParameters::FitOverMaxPointsCut );
  trackCuts->SetMaxPtCut ( AjParameters::MaxTrackPt );

  std::cout << "Using these track cuts:" << std::endl;
  std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
  std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
  std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
  std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl;
  
  // Towers
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/OrigY7MBBadTowers.txt");
  towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_AuAu_Nick.txt");
  towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
  // towerCuts->AddBadTowers( "emptyBadTowerList.txt");
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/badTowerList_y11.txt");
  towerCuts->SetMaxEtCut(AjParameters::MaxEtCut);

  std::cout << "Using these tower cuts:" << std::endl;
  std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl;
  std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl;
  
  // V0s: Turn off
  reader.SetProcessV0s(false);

  TStarJetPicoDefinitions::SetDebugLevel(0);
  TFile* fout = new TFile( OutFileName, "RECREATE");
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);

  TH3D* ptphieta = new TH3D("ptphieta","",100, 0.2, 10.2, 100, -TMath::Pi(), TMath::Pi(), 100, -1, 1);
  
  // Loop
  // ====
  Long64_t nEvents=-1;
  reader.Init(nEvents);
  
  Long64_t nAccepted=0;
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv;
  while ( reader.NextEvent() ) {
    ++nAccepted;
    reader.PrintStatus(10);      

    // Load event
    // ----------
    container = reader.GetOutputContainer();
    
    for (int ip = 0; ip<container->GetEntries() ; ++ip ){
      sv = container->Get(ip);\
      if ( fabs(sv->GetCharge())>1e-4 ) {
	ptphieta->Fill( sv->Pt(), sv->Phi(), sv->Eta() );
      }      
    }

  }

  ptphieta->Scale(1./float(nAccepted));
  fout->Write();
  fout->Close();

  return 0;
  
}
