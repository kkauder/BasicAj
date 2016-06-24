#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

int PlotPythiaAj() {

  // int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  float legs = 0.04;
  
  //TFile *f         = TFile::Open("AjResults/Pythia_RhicPythia_Aj.root");
  TFile *f         = TFile::Open("AjResults/R0.2_Pythia_RhicPythia_Aj.root");  

  // // TFile *f         = TFile::Open("AjResults/Pythia_RhicPythiaOnly_Aj.root");  
  // TFile *f         = TFile::Open("AjResults/R0.2_Pythia_RhicPythiaOnly_Aj.root");  

  TObjArray vAJ_lo;
  TObjArray vAJ_hi;

  TH2D* h2 = (TH2D*) f->Get( "NoFabsAJ_lo");
  // TH2D* h2 = (TH2D*) f->Get( "AJ_lo");
  int AuAuMultBinL = 1;
  int AuAuMultBinR = h2->GetNbinsY();
  
  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* PythiaAJ_lo     = (TH1D*) ( (TH2D*) f->Get( "NoFabsAJ_lo") )->ProjectionX("PythiaAJ_lo", AuAuMultBinL, AuAuMultBinR);
  // TH1D* PythiaAJ_lo     = (TH1D*) ( (TH2D*) f->Get( "AJ_lo") )->ProjectionX("PythiaAJ_lo", AuAuMultBinL, AuAuMultBinR);
  cout << PythiaAJ_lo ->Integral(1,100) << " dijets in Pythia" << endl;

  TH1D* PythiaAJ_hi     = (TH1D*) ( (TH2D*) f->Get( "AJ_hi") )->ProjectionX("PythiaAJ_hi", AuAuMultBinL, AuAuMultBinR);

     
  TObjArray toa;
  toa.Add(PythiaAJ_lo);
  toa.Add(PythiaAJ_hi);
  
  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");
    
    h->Rebin(2);
    // h->Scale(1./h->Integral());
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    // if (TString(f->GetName()).Contains("R0.2") ){
    //   h->SetAxisRange(  0.0, 0.23, "y");
    // } else { 
    //   h->SetAxisRange(  0.0, 0.33, "y");
    // }

    h->SetAxisRange(  0.0, 0.3, "y");    
    h->SetAxisRange(  -0.3, 0.8, "x");
    // h->SetAxisRange(  0.0, 0.72, "x");    
  }
  
  PythiaAJ_lo->SetLineColor( kBlack );
  PythiaAJ_lo->SetMarkerColor( kBlack );
  PythiaAJ_lo->SetMarkerStyle( 21 );

  PythiaAJ_hi->SetLineColor( kRed );
  PythiaAJ_hi->SetMarkerColor( kRed );
  PythiaAJ_hi->SetMarkerStyle( 20 );

  PythiaAJ_hi->GetYaxis()->SetTitleSize(0.04);
  PythiaAJ_hi->GetYaxis()->SetLabelSize(0.04);
  PythiaAJ_hi->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  PythiaAJ_hi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  PythiaAJ_hi->GetXaxis()->SetTitleSize(0.04);
  PythiaAJ_hi->GetXaxis()->SetLabelSize(0.04);
  PythiaAJ_hi->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  PythiaAJ_hi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  
  
  PythiaAJ_hi->GetYaxis()->SetTitleOffset(1.14);
  PythiaAJ_hi->Draw("9");
  PythiaAJ_lo->Draw("9same");
  
  
  // TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);
  // leg->AddEntry ( PythiaAJ_hi, "Pythia8, p_{T}^{Cut}>2 GeV/c", "p");
  // leg->AddEntry ( PythiaAJ_lo, "Pythia8, p_{T}^{Cut}>0.2 GeV/c", "p");
  // leg->Draw();

  TLegend* leghi = new TLegend( 0.15, 0.75, 0.48, 0.88, "p_{T}^{Cut}>2 GeV/c:" );
  leghi->SetBorderSize(0);
  leghi->SetTextColor( kRed);
  leghi->SetTextSize(legs);
  leghi->SetFillStyle(0);
  leghi->AddEntry ( PythiaAJ_hi, "Pythia8, p_{T}^{Cut}>2 GeV/c", "p");
  leghi->Draw();

  TLegend* leglo = new TLegend( 0.55, 0.75, 0.88, 0.88, "p_{T}^{Cut}>0.2 GeV/c, Matched:" );
  leglo->SetBorderSize(0);
  leglo->SetTextSize(legs);
  leglo->SetFillStyle(0);
  leglo->AddEntry ( PythiaAJ_lo, "Pythia8, p_{T}^{Cut}>0.2 GeV/c", "p");
  leglo->Draw();


  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);
  char plabel[400];

  latex.SetTextColor(kBlack);
  latex.SetTextSize( 0.04);

  latex.DrawLatex( 0.65, 0.5, "With p_{T}^{Cut}>2 GeV/c:");
  latex.DrawLatex( 0.65, 0.45, "  p_{T,lead}>20 Gev/c");
  latex.DrawLatex( 0.65, 0.4, "  p_{T,sublead}>10 Gev/c");

  latex.SetTextSize( 0.04);
  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  if (TString(f->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  } else {
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  }  

  // latex.SetTextColor(kBlack);
  // latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  // latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");
  // if (TString(f->GetName()).Contains("R0.2") ){
  //   latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  // } else {
  //   latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  // }  

  if (TString(f->GetName()).Contains("R0.2") ){
    gPad->SaveAs( "plots/R0.2_PythiaAj.png" );
  } else {
    gPad->SaveAs( "plots/PythiaAj.png" );
  }  

  return 0;
  
}

