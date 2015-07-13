#include "TFile.h"
#include "TTree.h"
#include "TVectorT.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"


#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

void MakeAjFromScratch(TString inname = "AjResults/AuAuAj.root")
{
  // -------------------------------------------------------------
  // Purpose: Recalculate AJ unbinned, for better p-value testing
  // -------------------------------------------------------------

  TFile * fin = new TFile( inname, "READ" );

  // for comparison, get the original histos
  // ---------------------------------------
  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  TH2D* h2 = (TH2D*) fin->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();

  TH1D* hAJ_hi     = (TH1D*) ( (TH2D*) fin->Get( "AJ_hi") )->ProjectionX("hAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* hAJ_lo     = (TH1D*) ( (TH2D*) fin->Get( "AJ_lo") )->ProjectionX("hAJ_lo", AuAuMultBinL, AuAuMultBinR);

  TH1D* NewAJ_hi = (TH1D*) hAJ_hi->Clone("NewAJ_hi");
  NewAJ_hi->Reset();
  TH1D* NewAJ_lo = (TH1D*) hAJ_lo->Clone("NewAJ_lo");
  NewAJ_lo->Reset();
  
  // load result tree
  // ----------------
  TTree* ResultTree = (TTree*) fin->Get("ResultTree");
  TLorentzVector *pJ1 = new TLorentzVector();
  ResultTree->SetBranchAddress("j1", &pJ1);
  TLorentzVector *pJ2 = new TLorentzVector();
  ResultTree->SetBranchAddress("j2", &pJ2);
  TLorentzVector *pJM1 = new TLorentzVector();
  ResultTree->SetBranchAddress("jm1", &pJM1);
  TLorentzVector *pJM2 = new TLorentzVector();
  ResultTree->SetBranchAddress("jm2", &pJM2);
  double Origrefmult;
  ResultTree->SetBranchAddress("refmult", &Origrefmult);


  // exact result
  // ------------
  vector <double> vAJ_lo;
  vector <double> vAJ_hi;
  
  for ( int i=0; i<ResultTree->GetEntries() ; ++i ){
    ResultTree->GetEntry(i);
    
    // cout << i << "  " << Origrefmult << endl;
    if ( Origrefmult < RefmultCut ) continue;

    double ajhi = fabs ( ( pJ1->Pt() -  pJ2->Pt()) / ( pJ1->Pt() +  pJ2->Pt()) );
    double ajlo = fabs ( (pJM1->Pt() - pJM2->Pt()) / (pJM1->Pt() + pJM2->Pt()) );

    vAJ_hi.push_back( ajhi );
    vAJ_lo.push_back( ajlo );

    NewAJ_hi->Fill( ajhi );
    NewAJ_lo->Fill( ajlo );
    
  }
  
  hAJ_hi->SetLineColor(kRed);
  hAJ_lo->SetLineColor(kRed);

  NewAJ_hi->SetLineColor(kGreen+1);
  NewAJ_lo->SetLineColor(kGreen+1);

  // new TCanvas;
  // NewAJ_lo->Draw();
  // NewAJ_hi->Draw("same");
  // hAJ_lo->Draw("same");
  // hAJ_hi->Draw("same");

  // double* p;
  // int n;
  // p = (double*) &(vAJ_hi[0]);   n = vAJ_hi.size();
  // TMath::Sort( n, p );
  // return;
  // TMath::Sort( (int) vAJ_lo.size(), (double*) &(vAJ_lo[0]) );
  
  std::sort( vAJ_hi.begin(), vAJ_hi.end() );
  std::sort( vAJ_lo.begin(), vAJ_lo.end() );
  
  TVectorD TVAJ_hi (vAJ_hi.size(), &(vAJ_hi[0]));
  TVectorD TVAJ_lo (vAJ_lo.size(), &(vAJ_lo[0]));
  
  TString outname=inname;
  outname.ReplaceAll(".root",".vectors.root");
  TFile* fout = new TFile( outname,"RECREATE");

  TVAJ_hi.Write("TVAJ_hi");
  TVAJ_lo.Write("TVAJ_lo");

  // TVAJ_hi.Print();
  // TVAJ_lo.Print();
  
  cout << "Wrote to " << outname << endl;
  
  
}
