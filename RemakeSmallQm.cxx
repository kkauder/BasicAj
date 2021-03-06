#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void RemakeSmallQm() {
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg1 = new TLegend( 0.44, 0.77, 0.89, 0.9, "" );
  leg1->SetBorderSize(0);
  leg1->SetLineWidth(10);
  leg1->SetFillStyle(0);
  leg1->SetMargin(0.1);

  TCanvas* c2 = new TCanvas("c2");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg2 = new TLegend( 0.44, 0.77, 0.89, 0.9, "" );
  leg2->SetBorderSize(0);
  leg2->SetLineWidth(10);
  leg2->SetFillStyle(0);
  leg2->SetMargin(0.1);

  // TFile *fAuAu         = TFile::Open("AjResults/ShiftFollowAuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/ppInAuAuShiftFollowAj.root");
  // TFile *fAuAu         = TFile::Open("AjResults/FollowAuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/ppInAuAuFollowAj.root");
  
  TFile *fAuAu         = TFile::Open("AjResults/VarFollowAuAuAj.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/ppInAuAuFollowAj.root");

  TObjArray vAJ_lo;
  TObjArray vAJ_hi;
  TObjArray vUnmatchedAJ_hi;
  
  TH1D* AuAuAJ_lo     = (TH1D*) fAuAu->Get( "SmallAJ_lo");
  AuAuAJ_lo->SetName("AuAuAJ_lo");
  TH1D* ppInAuAuAJ_lo = (TH1D*) fppInAuAu->Get( "SmallAJ_lo");
  ppInAuAuAJ_lo->SetName("ppInAuAuAJ_lo");  

  TH1D* AuAuUnmatchedAJ_hi     = (TH1D*) fAuAu->Get( "SmallUnmatchedAJ_hi");
  AuAuUnmatchedAJ_hi->SetName("AuAuUnmatchedAJ_hi");
  TH1D* ppInAuAuUnmatchedAJ_hi = (TH1D*) fppInAuAu->Get( "SmallUnmatchedAJ_hi");
  ppInAuAuUnmatchedAJ_hi->SetName("ppInAuAuUnmatchedAJ_hi");  
   
  TObjArray toa;
  toa.Add(AuAuAJ_lo);
  toa.Add(ppInAuAuAJ_lo);
  toa.Add(AuAuUnmatchedAJ_hi);
  toa.Add(ppInAuAuUnmatchedAJ_hi);  
  
  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");

    h->Rebin(2);
    h->Scale(1./h->Integral());
    // h->Scale(1./8147);
      
    h->SetAxisRange(  0.0, 0.22, "y");
    h->SetAxisRange(  0.0, 0.72, "x");
  }

  AuAuAJ_lo->SetLineColor( kBlack );
  AuAuAJ_lo->SetMarkerColor( kBlack );
  AuAuAJ_lo->SetMarkerStyle( 21 );

  ppInAuAuAJ_lo->SetLineColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerStyle( 25 );

  AuAuUnmatchedAJ_hi->SetLineColor( kRed );
  AuAuUnmatchedAJ_hi->SetMarkerColor( kRed );
  AuAuUnmatchedAJ_hi->SetMarkerStyle( 20 );

  ppInAuAuUnmatchedAJ_hi->SetLineColor( kRed );
  ppInAuAuUnmatchedAJ_hi->SetMarkerColor( kRed );
  ppInAuAuUnmatchedAJ_hi->SetMarkerStyle( 24 );
  
  // TFile* debug = new TFile("Joern/hAjMatchedAbs.root");
  // TH1D* hAjMatchedAbs = (TH1D*) debug->Get("hAjMatchedAbs");
  // ppInAuAuAJ_lo->DrawNormalized();
  // hAjMatchedAbs->DrawNormalized("same");
  // gPad->SaveAs("plots/CompareToMe.png");
  // return;


  c1->cd();
  ppInAuAuUnmatchedAJ_hi->Draw("9");
  AuAuUnmatchedAJ_hi->Draw("9same");

  leg1->AddEntry ( ppInAuAuUnmatchedAJ_hi, "pp HT #otimes AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg1->AddEntry ( AuAuUnmatchedAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg1->Draw();
  gPad->SaveAs("plots/RemakeSmallHi.png");
  
  c2->cd();
  ppInAuAuAJ_lo->Draw("9");
  AuAuAJ_lo->Draw("9same");

  leg2->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg2->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg2->Draw();
  gPad->SaveAs("plots/RemakeSmallLo.png");
  
  cout << "pT>2 GeV,   KolmogorovTest: " << ppInAuAuUnmatchedAJ_hi->KolmogorovTest(AuAuUnmatchedAJ_hi, "D") << endl;
  cout << "pT>0.2 GeV, KolmogorovTest: " << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo,"D") << endl;


}

