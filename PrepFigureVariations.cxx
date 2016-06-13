#include <vector>
#include <algorithm>

#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

int PrepFigureVariations(TString infile="AjResults/Fresh_NicksList_HC100_AuAu.root") {

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%

  gStyle->SetOptStat(0);

  TFile *f         = TFile::Open(infile);  
  TString outfilename = gSystem->BaseName( f->GetName() );
  outfilename.Prepend ( "AjResults/Var_" );

  TFile* out = new TFile( outfilename, "RECREATE");

  TH2D* h2 = (TH2D*) f->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();
  
  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* origAJ_lo     = (TH1D*) ( (TH2D*) f->Get( "AJ_lo") )->ProjectionX("origAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* origAJ_hi     = (TH1D*) ( (TH2D*) f->Get( "AJ_hi") )->ProjectionX("origAJ_hi", AuAuMultBinL, AuAuMultBinR);

  // Clone and fill by hand
  // ----------------------
  TH1D* AJ_lo = (TH1D*) origAJ_lo->Clone("AJ_lo");
  AJ_lo->Reset();
  TH1D* AJ_hi = (TH1D*) origAJ_hi->Clone("AJ_hi");
  AJ_hi->Reset();

  TH1D* NoFabsAJ_lo = (TH1D*) origAJ_lo->Clone("NoFabsAJ_lo");
  NoFabsAJ_lo->Reset();
  TH1D* NoFabsAJ_hi = (TH1D*) origAJ_hi->Clone("NoFabsAJ_hi");
  NoFabsAJ_hi->Reset();

  TH1D* PosOnlyAJ_lo = (TH1D*) origAJ_lo->Clone("PosOnlyAJ_lo");
  PosOnlyAJ_lo->Reset();
  TH1D* PosOnlyAJ_hi = (TH1D*) origAJ_hi->Clone("PosOnlyAJ_hi");
  PosOnlyAJ_hi->Reset();

  TH1D* PerfectMatchAJ_lo = (TH1D*) origAJ_lo->Clone("PerfectMatchAJ_lo");
  PerfectMatchAJ_lo->Reset();
  TH1D* PerfectMatchAJ_hi = (TH1D*) origAJ_hi->Clone("PerfectMatchAJ_hi");
  PerfectMatchAJ_hi->Reset();

  // Prep KS calculation while we're at it
  vector<double> vUnbinnedAJ_lo;
  vector<double> vUnbinnedAJ_hi; 

  vector<double> vUnbinnedNoFabsAJ_lo;
  vector<double> vUnbinnedNoFabsAJ_hi;

  vector<double> vUnbinnedPosOnlyAJ_lo;
  vector<double> vUnbinnedPosOnlyAJ_hi;

  vector<double> vUnbinnedPerfectMatchAJ_lo;
  vector<double> vUnbinnedPerfectMatchAJ_hi;

  TTree* ResultTree = (TTree*) f->Get("ResultTree");
  TLorentzVector *pJ1 = new TLorentzVector();
  TLorentzVector *pJ2 = new TLorentzVector();
  TLorentzVector *pJM1 = new TLorentzVector();
  TLorentzVector *pJM2 = new TLorentzVector();
  double refmult;
  float MatchPerc1, MatchPerc2;
  
  ResultTree->SetBranchAddress("j1", &pJ1);
  ResultTree->SetBranchAddress("j2", &pJ2);
  ResultTree->SetBranchAddress("jm1", &pJM1);
  ResultTree->SetBranchAddress("jm2", &pJM2);
  ResultTree->SetBranchAddress("refmult", &refmult);
  ResultTree->SetBranchAddress("MatchPerc1", &MatchPerc1);
  ResultTree->SetBranchAddress("MatchPerc2", &MatchPerc2);

  int AuAuMultL=RefmultCut;
  int AuAuMultR=-1;;
  if ( AuAuMultL < 0 ) AuAuMultL = 0;
  if ( AuAuMultR < 0 ) AuAuMultR = 10000;

  for ( int i=0; i<ResultTree->GetEntries(); ++i ){
    ResultTree->GetEntry ( i );
    if ( refmult<RefmultCut || refmult > AuAuMultR ) continue;

    double aj_hi = CalcAj( pJ1, pJ2 );
    double aj_lo = CalcAj( pJM1, pJM2 );

    NoFabsAJ_hi->Fill(aj_hi);
    NoFabsAJ_lo->Fill(aj_lo);
    vUnbinnedNoFabsAJ_hi.push_back( aj_hi );
    vUnbinnedNoFabsAJ_lo.push_back( aj_lo );
  
    AJ_hi->Fill( fabs(aj_hi) );
    AJ_lo->Fill( fabs(aj_lo) );
    vUnbinnedAJ_hi.push_back( fabs(aj_hi) );
    vUnbinnedAJ_lo.push_back( fabs(aj_lo) );
    
    if ( aj_hi > 0 ){
      PosOnlyAJ_hi->Fill(aj_hi);
      vUnbinnedPosOnlyAJ_hi.push_back( aj_hi );
    }
    if ( aj_lo > 0 ){
      PosOnlyAJ_lo->Fill(aj_lo);
      vUnbinnedPosOnlyAJ_lo.push_back( aj_lo );
    }

    // if ( MatchPerc1 > 0.99 && MatchPerc1 < 1.01 ){
    PerfectMatchAJ_hi->Fill( fabs(aj_hi) );
    vUnbinnedPerfectMatchAJ_hi.push_back( fabs(aj_hi) );
      // } else cerr << " MatchPerc1 = " << MatchPerc1 << endl;

    if ( MatchPerc2 > 0.99 && MatchPerc2 < 1.01 && MatchPerc2 > 0.99 && MatchPerc2 < 1.01 ){
      PerfectMatchAJ_lo->Fill( fabs(aj_lo) );
      vUnbinnedPerfectMatchAJ_lo.push_back( fabs(aj_lo) );
    } else cerr << " MatchPerc2 = " << MatchPerc2 << " MatchPerc1 = " << MatchPerc1 << endl;

  }

  
  // // Diagnostics
  // // Scale to see there are two histos
  // new TCanvas; 
  // origAJ_lo->Scale( 1.01 );
  // origAJ_lo->SetLineColor(kGreen+1);
  // origAJ_lo->Draw();
  // AJ_lo->SetLineColor(kMagenta);
  // AJ_lo->Draw("same");
  
  
  // new TCanvas; 
  // origAJ_hi->Scale( 1.01 );
  // origAJ_hi->SetLineColor(kGreen+1);
  // origAJ_hi->Draw();
  // AJ_hi->SetLineColor(kMagenta);
  // AJ_hi->Draw("same");
  

  // Grrr - need to be sorted for KS
  std::sort(vUnbinnedAJ_lo.begin(), vUnbinnedAJ_lo.end());
  std::sort(vUnbinnedAJ_hi.begin(), vUnbinnedAJ_hi.end());  

  std::sort(vUnbinnedNoFabsAJ_lo.begin(), vUnbinnedNoFabsAJ_lo.end());
  std::sort(vUnbinnedNoFabsAJ_hi.begin(), vUnbinnedNoFabsAJ_hi.end());  

  std::sort(vUnbinnedPosOnlyAJ_lo.begin(), vUnbinnedPosOnlyAJ_lo.end());
  std::sort(vUnbinnedPosOnlyAJ_hi.begin(), vUnbinnedPosOnlyAJ_hi.end());  

  std::sort(vUnbinnedPerfectMatchAJ_lo.begin(), vUnbinnedPerfectMatchAJ_lo.end());
  std::sort(vUnbinnedPerfectMatchAJ_hi.begin(), vUnbinnedPerfectMatchAJ_hi.end());


  // Translate to serializable root object
  TVectorT<double> UnbinnedAJ_lo ( vUnbinnedAJ_lo.size(), (const double*) &vUnbinnedAJ_lo.at(0) );
  UnbinnedAJ_lo.Write("UnbinnedAJ_lo");
  TVectorT<double> UnbinnedAJ_hi ( vUnbinnedAJ_hi.size(), (const double*) &vUnbinnedAJ_hi.at(0) );
  UnbinnedAJ_hi.Write("UnbinnedAJ_hi");  

  TVectorT<double> UnbinnedNoFabsAJ_lo ( vUnbinnedNoFabsAJ_lo.size(), (const double*) &vUnbinnedNoFabsAJ_lo.at(0) );
  UnbinnedNoFabsAJ_lo.Write("UnbinnedNoFabsAJ_lo");
  TVectorT<double> UnbinnedNoFabsAJ_hi ( vUnbinnedNoFabsAJ_hi.size(), (const double*) &vUnbinnedNoFabsAJ_hi.at(0) );
  UnbinnedNoFabsAJ_hi.Write("UnbinnedNoFabsAJ_hi");  

  TVectorT<double> UnbinnedPosOnlyAJ_lo ( vUnbinnedPosOnlyAJ_lo.size(), (const double*) &vUnbinnedPosOnlyAJ_lo.at(0) );
  UnbinnedPosOnlyAJ_lo.Write("UnbinnedPosOnlyAJ_lo");
  TVectorT<double> UnbinnedPosOnlyAJ_hi ( vUnbinnedPosOnlyAJ_hi.size(), (const double*) &vUnbinnedPosOnlyAJ_hi.at(0) );
  UnbinnedPosOnlyAJ_hi.Write("UnbinnedPosOnlyAJ_hi");  

  TVectorT<double> UnbinnedPerfectMatchAJ_lo ( vUnbinnedPerfectMatchAJ_lo.size(), (const double*) &vUnbinnedPerfectMatchAJ_lo.at(0) );
  UnbinnedPerfectMatchAJ_lo.Write("UnbinnedPerfectMatchAJ_lo");
  TVectorT<double> UnbinnedPerfectMatchAJ_hi ( vUnbinnedPerfectMatchAJ_hi.size(), (const double*) &vUnbinnedPerfectMatchAJ_hi.at(0) );
  UnbinnedPerfectMatchAJ_hi.Write("UnbinnedPerfectMatchAJ_hi");  

  // Done
  // ----
  out->Write();
  cout << "Wrote to " << out->GetName() << endl;

  return 0;

}

// ===========================================================================
double CalcAj ( TLorentzVector* j1, TLorentzVector* j2 ){
  return (( j1->Pt()-j2->Pt() ) / ( j1->Pt()+j2->Pt() ));    
}
    
