#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

int FigureVariations() {

  gStyle->SetOptStat(0);
  TFile *fSyst     = TFile::Open("AjResults/Var_Systematics_Fresh_NicksList_HC100_ppInAuAuAj.root");

  // TString RootOut="plots/R0.4_VarFig.root";
  // if (TString(fAuAu->GetName()).Contains("R0.2") ){
  //   RootOut="plots/R0.2_VarFig.root";
  // }

  // TFile* out = new TFile( RootOut, "RECREATE");

  // Grab all relevant histos and prep them
  TH1D* origAuAuAJ_lo = (TH1D*) fSyst->Get( "AuAuAJ_lo");
  TH1D* origppInAuAuAJ_lo = (TH1D*) fSyst->Get( "AJ_lo_T0" );
  TH1D* origAuAuAJ_hi = (TH1D*) fSyst->Get( "AuAuAJ_hi");
  TH1D* origppInAuAuAJ_hi = (TH1D*) fSyst->Get( "AJ_hi_T0" );

  TH1D* NoFabsAuAuAJ_lo = (TH1D*) fSyst->Get( "AuAuNoFabsAJ_lo");
  TH1D* NoFabsppInAuAuAJ_lo = (TH1D*) fSyst->Get( "NoFabsAJ_lo_T0" );
  TH1D* NoFabsAuAuAJ_hi = (TH1D*) fSyst->Get( "AuAuNoFabsAJ_hi");
  TH1D* NoFabsppInAuAuAJ_hi = (TH1D*) fSyst->Get( "NoFabsAJ_hi_T0" );
  
  TH1D* PosOnlyAuAuAJ_lo = (TH1D*) fSyst->Get( "AuAuPosOnlyAJ_lo");
  TH1D* PosOnlyppInAuAuAJ_lo = (TH1D*) fSyst->Get( "PosOnlyAJ_lo_T0" );
  TH1D* PosOnlyAuAuAJ_hi = (TH1D*) fSyst->Get( "AuAuPosOnlyAJ_hi");
  TH1D* PosOnlyppInAuAuAJ_hi = (TH1D*) fSyst->Get( "PosOnlyAJ_hi_T0" );

  TH1D* PerfectMatchAuAuAJ_lo = (TH1D*) fSyst->Get( "AuAuPerfectMatchAJ_lo");
  TH1D* PerfectMatchppInAuAuAJ_lo = (TH1D*) fSyst->Get( "PerfectMatchAJ_lo_T0" );
  TH1D* PerfectMatchAuAuAJ_hi = (TH1D*) fSyst->Get( "AuAuPerfectMatchAJ_hi");
  TH1D* PerfectMatchppInAuAuAJ_hi = (TH1D*) fSyst->Get( "PerfectMatchAJ_hi_T0" ); 

  // Prettify
  // --------
  TObjArray toa;
  toa.Add(origAuAuAJ_lo);
  toa.Add(origppInAuAuAJ_lo);
  toa.Add(origAuAuAJ_hi);
  toa.Add(origppInAuAuAJ_hi);

  toa.Add(NoFabsAuAuAJ_lo);
  toa.Add(NoFabsppInAuAuAJ_lo);
  toa.Add(NoFabsAuAuAJ_hi);
  toa.Add(NoFabsppInAuAuAJ_hi);

  toa.Add(PosOnlyAuAuAJ_lo);
  toa.Add(PosOnlyppInAuAuAJ_lo);
  toa.Add(PosOnlyAuAuAJ_hi);
  toa.Add(PosOnlyppInAuAuAJ_hi);

  toa.Add(PerfectMatchAuAuAJ_lo);
  toa.Add(PerfectMatchppInAuAuAJ_lo);
  toa.Add(PerfectMatchAuAuAJ_hi);
  toa.Add(PerfectMatchppInAuAuAJ_hi);
  
  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");
    if (  TString(h->GetName()).Contains("NoFabs") )      h->SetTitle(";A_{J};Fraction");
    if (  TString(h->GetName()).Contains("PosOnly") )      h->SetTitle(";A_{J};Fraction");

    h->SetAxisRange(  0.0, 0.23, "y");    
    // h->SetAxisRange(  0.0, 0.72, "x");

    if ( TString(h->GetName()).Contains("_lo") ){
      h->SetLineColor( kBlack );
      h->SetMarkerColor( kBlack );
      if (TString(h->GetName()).Contains("AuAu")){
	h->SetMarkerStyle( 21 );
      } else {
	h->SetMarkerStyle( 25 );
      }      
    } else { // _hi
      h->SetLineColor( kRed );
      h->SetMarkerColor( kRed );      
      if (TString(h->GetName()).Contains("AuAu")){
	h->SetMarkerStyle( 20 );
      } else {
	h->SetMarkerStyle( 24 );
      }      
    }
        
    h->GetYaxis()->SetTitleSize(0.04);
    h->GetYaxis()->SetLabelSize(0.04);
    h->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
    
    h->GetXaxis()->SetTitleSize(0.04);
    h->GetXaxis()->SetLabelSize(0.04);
    h->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
      
    h->GetYaxis()->SetTitleOffset(1.14);
  }

  // Same for Systematics
  // --------------------
  TH1D* orig_minmax_lo = (TH1D*) fSyst->Get( "AJ_lo_minmax" );
  TH1D* orig_minmax_hi = (TH1D*) fSyst->Get( "AJ_hi_minmax" );

  TH1D* NoFabs_minmax_lo = (TH1D*) fSyst->Get( "NoFabsAJ_lo_minmax" );
  TH1D* NoFabs_minmax_hi = (TH1D*) fSyst->Get( "NoFabsAJ_hi_minmax" );

  TH1D* PosOnly_minmax_lo = (TH1D*) fSyst->Get( "PosOnlyAJ_lo_minmax" );
  TH1D* PosOnly_minmax_hi = (TH1D*) fSyst->Get( "PosOnlyAJ_hi_minmax" );

  TH1D* PerfectMatch_minmax_lo = (TH1D*) fSyst->Get( "PerfectMatchAJ_lo_minmax" );
  TH1D* PerfectMatch_minmax_hi = (TH1D*) fSyst->Get( "PerfectMatchAJ_hi_minmax" );

  toa.Clear();
  toa.Add(orig_minmax_lo);
  toa.Add(orig_minmax_hi);
  toa.Add(NoFabs_minmax_lo);
  toa.Add(NoFabs_minmax_hi);
  toa.Add(PosOnly_minmax_lo);
  toa.Add(PosOnly_minmax_hi);
  toa.Add(PerfectMatch_minmax_lo);
  toa.Add(PerfectMatch_minmax_hi);

  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    if ( TString(h->GetName()).Contains("_lo") ){
      h->SetFillColor( kGray );
      h->SetMarkerColor(kGray);
      h->SetMarkerSize(0);
    } else {
      h->SetFillColor( kRed-10 );
      h->SetMarkerColor(kRed-10);
      h->SetMarkerSize(0);
    }
    
  }

  TParameter<double>* Tpar;
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);
  char plabel[400];

  // ============================================
  // Draw Nominal
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  origppInAuAuAJ_hi->SetAxisRange(  0.0, 0.72, "x");
  
  origppInAuAuAJ_hi->Draw("9");
  orig_minmax_hi->Draw("9E2same");
  origppInAuAuAJ_hi->Draw("9same");
  origAuAuAJ_hi->Draw("9same");

  origppInAuAuAJ_lo->Draw("9same");
  orig_minmax_lo->Draw("9E2same");
  origppInAuAuAJ_lo->Draw("9same");
  origAuAuAJ_lo->Draw("9same");
  
  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( origppInAuAuAJ_hi, "pp HT #oplus AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( origAuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( origppInAuAuAJ_lo,          "pp HT #oplus AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( origAuAuAJ_lo,              "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  
  leg->Draw();

  Tpar=(TParameter<double>*) fSyst->Get("KS_AJ_NominalE_Hi");
  double KS_AJ_NominalE_Hi = Tpar->GetVal();
  Tpar=(TParameter<double>*) fSyst->Get("KS_AJ_NominalE_Lo");
  double KS_AJ_NominalE_Lo = Tpar->GetVal();
  
  sprintf ( plabel, "p-value = %0.1g", KS_AJ_NominalE_Hi);
  latex.SetTextColor( origAuAuAJ_hi->GetLineColor() );
  latex.DrawLatex( .6,.5, plabel);
  sprintf ( plabel, "p-value = %0.2g", KS_AJ_NominalE_Lo);
  latex.SetTextColor( origAuAuAJ_lo->GetLineColor() );
  latex.DrawLatex( .6,.45, plabel);

  if (TString(fSyst->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/Var_R0.2_Fig.png");
    gPad->SaveAs("plots/Var_R0.2_Fig.pdf");
  } else {
    gPad->SaveAs("plots/Var_R0.4_Fig1.png");
    gPad->SaveAs("plots/Var_R0.4_Fig1.pdf");
  }

  // ============================================
  // Draw full AJ (NoFabs)
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  NoFabsppInAuAuAJ_hi->SetAxisRange(  -0.3, 0.72, "x");
  
  NoFabsppInAuAuAJ_hi->Draw("9");
  NoFabs_minmax_hi->Draw("9E2same");
  NoFabsppInAuAuAJ_hi->Draw("9same");
  NoFabsAuAuAJ_hi->Draw("9same");

  NoFabsppInAuAuAJ_lo->Draw("9same");
  NoFabs_minmax_lo->Draw("9E2same");
  NoFabsppInAuAuAJ_lo->Draw("9same");
  NoFabsAuAuAJ_lo->Draw("9same");
  
  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( NoFabsppInAuAuAJ_hi, "pp HT #oplus AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( NoFabsAuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( NoFabsppInAuAuAJ_lo,          "pp HT #oplus AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( NoFabsAuAuAJ_lo,              "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  
  leg->Draw();

  Tpar=(TParameter<double>*) fSyst->Get("KS_NoFabsAJ_NominalE_Hi");
  double KS_NoFabsAJ_NominalE_Hi = Tpar->GetVal();
  Tpar=(TParameter<double>*) fSyst->Get("KS_NoFabsAJ_NominalE_Lo");
  double KS_NoFabsAJ_NominalE_Lo = Tpar->GetVal();
  
  sprintf ( plabel, "p-value = %0.1g", KS_NoFabsAJ_NominalE_Hi);
  latex.SetTextColor( NoFabsAuAuAJ_hi->GetLineColor() );
  latex.DrawLatex( .6,.5, plabel);
  sprintf ( plabel, "p-value = %0.2g", KS_NoFabsAJ_NominalE_Lo);
  latex.SetTextColor( NoFabsAuAuAJ_lo->GetLineColor() );
  latex.DrawLatex( .6,.45, plabel);

  if (TString(fSyst->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/Var_NoFabs_R0.2_Fig.png");
    gPad->SaveAs("plots/Var_NoFabs_R0.2_Fig.pdf");
  } else {
    gPad->SaveAs("plots/Var_NoFabs_R0.4_Fig1.png");
    gPad->SaveAs("plots/Var_NoFabs_R0.4_Fig1.pdf");
  }

      
  // ============================================
  // Draw only AJ>0 (PosOnly)
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  PosOnlyppInAuAuAJ_hi->SetAxisRange(  0, 0.72, "x");
  
  PosOnlyppInAuAuAJ_hi->Draw("9");
  PosOnly_minmax_hi->Draw("9E2same");
  PosOnlyppInAuAuAJ_hi->Draw("9same");
  PosOnlyAuAuAJ_hi->Draw("9same");

  PosOnlyppInAuAuAJ_lo->Draw("9same");
  PosOnly_minmax_lo->Draw("9E2same");
  PosOnlyppInAuAuAJ_lo->Draw("9same");
  PosOnlyAuAuAJ_lo->Draw("9same");
  
  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( PosOnlyppInAuAuAJ_hi, "pp HT #oplus AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( PosOnlyAuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( PosOnlyppInAuAuAJ_lo,          "pp HT #oplus AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( PosOnlyAuAuAJ_lo,              "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  
  leg->Draw();

  Tpar=(TParameter<double>*) fSyst->Get("KS_PosOnlyAJ_NominalE_Hi");
  double KS_PosOnlyAJ_NominalE_Hi = Tpar->GetVal();
  Tpar=(TParameter<double>*) fSyst->Get("KS_PosOnlyAJ_NominalE_Lo");
  double KS_PosOnlyAJ_NominalE_Lo = Tpar->GetVal();
  
  sprintf ( plabel, "p-value = %0.1g", KS_PosOnlyAJ_NominalE_Hi);
  latex.SetTextColor( PosOnlyAuAuAJ_hi->GetLineColor() );
  latex.DrawLatex( .6,.5, plabel);
  sprintf ( plabel, "p-value = %0.2g", KS_PosOnlyAJ_NominalE_Lo);
  latex.SetTextColor( PosOnlyAuAuAJ_lo->GetLineColor() );
  latex.DrawLatex( .6,.45, plabel);

  if (TString(fSyst->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/Var_PosOnly_R0.2_Fig.png");
    gPad->SaveAs("plots/Var_PosOnly_R0.2_Fig.pdf");
  } else {
    gPad->SaveAs("plots/Var_PosOnly_R0.4_Fig1.png");
    gPad->SaveAs("plots/Var_PosOnly_R0.4_Fig1.pdf");
  }

  // ============================================
  // Draw only perfect matches (PerfectMatch)
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  PerfectMatchppInAuAuAJ_hi->SetAxisRange(  0, 0.72, "x");
  
  PerfectMatchppInAuAuAJ_hi->Draw("9");
  PerfectMatch_minmax_hi->Draw("9E2same");
  PerfectMatchppInAuAuAJ_hi->Draw("9same");
  PerfectMatchAuAuAJ_hi->Draw("9same");

  PerfectMatchppInAuAuAJ_lo->Draw("9same");
  PerfectMatch_minmax_lo->Draw("9E2same");
  PerfectMatchppInAuAuAJ_lo->Draw("9same");
  PerfectMatchAuAuAJ_lo->Draw("9same");
  
  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( PerfectMatchppInAuAuAJ_hi, "pp HT #oplus AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( PerfectMatchAuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( PerfectMatchppInAuAuAJ_lo,          "pp HT #oplus AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( PerfectMatchAuAuAJ_lo,              "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  
  leg->Draw();

  Tpar=(TParameter<double>*) fSyst->Get("KS_PerfectMatchAJ_NominalE_Hi");
  double KS_PerfectMatchAJ_NominalE_Hi = Tpar->GetVal();
  Tpar=(TParameter<double>*) fSyst->Get("KS_PerfectMatchAJ_NominalE_Lo");
  double KS_PerfectMatchAJ_NominalE_Lo = Tpar->GetVal();
  
  sprintf ( plabel, "p-value = %0.1g", KS_PerfectMatchAJ_NominalE_Hi);
  latex.SetTextColor( PerfectMatchAuAuAJ_hi->GetLineColor() );
  latex.DrawLatex( .6,.5, plabel);
  sprintf ( plabel, "p-value = %0.2g", KS_PerfectMatchAJ_NominalE_Lo);
  latex.SetTextColor( PerfectMatchAuAuAJ_lo->GetLineColor() );
  latex.DrawLatex( .6,.45, plabel);

  if (TString(fSyst->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/Var_PerfectMatch_R0.2_Fig.png");
    gPad->SaveAs("plots/Var_PerfectMatch_R0.2_Fig.pdf");
  } else {
    gPad->SaveAs("plots/Var_PerfectMatch_R0.4_Fig1.png");
    gPad->SaveAs("plots/Var_PerfectMatch_R0.4_Fig1.pdf");
  }
      
      


  return 0;
  

}

