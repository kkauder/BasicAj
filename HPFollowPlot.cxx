#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void HPFollowPlot() {

  // int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  int RefmultCut = 351;  // 269 for 0-20%, 399 for 0-10%

  // This would give 40-100%:
  // int AuAuMultBinL = h2->GetYaxis()->FindBin( 0 ); 
  // int AuAuMultBinR = h2->GetYaxis()->FindBin( 113 ); 

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  // TFile *fAuAu         = TFile::Open("AjResults/R0.2_Presel_AuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_R0.2_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/R0.2_Systematics_ppInAuAuAj.root");

  // TFile *fAuAu         = TFile::Open("AjResults/Presel_AuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/Systematics_ppInAuAuAj.root");

  TFile *fAuAu         = TFile::Open("AjResults/HP_Presel_AuAuAj.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_HP_ppInAuAuAj.root");
  TFile *fSyst         = TFile::Open("AjResults/HP_Systematics_ppInAuAuAj.root");


  TH2D* h2 = (TH2D*) fAuAu->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();

  // int AuAuMultBinL = h2->GetYaxis()->FindBin( 0 ); 
  // int AuAuMultBinR = h2->GetYaxis()->FindBin( 113 ); 

  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;

  // USING JOERN'S RESULT
  // ---------------------
  TFile* Jresult = new TFile("Jresults.root");
  // TH1D* AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  // TH1D* ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* AuAuAJ_lo = (TH1D*) Jresult->Get("hAjMatchedA");
  TH1D* ppInAuAuAJ_lo = (TH1D*) Jresult->Get("hAjMatchedEmb");
  TGraphErrors* gSysMatchedComb = (TGraphErrors*) Jresult->Get("gSysMatchedComb");

  TH1D* AuAuOtherAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "OtherAJ_lo") )->ProjectionX("AuAuOtherAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAuOtherAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "OtherAJ_lo") )->ProjectionX("ppInAuAuOtherAJ_lo", AuAuMultBinL, AuAuMultBinR);

  TH1D* AuAuAJ_hi     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_hi") )->ProjectionX("AuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAuAJ_hi = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_hi") )->ProjectionX("ppInAuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
   
  TObjArray toa;
  //  toa.Add(AuAuAJ_lo);
  // toa.Add(ppInAuAuAJ_lo);
  toa.Add(AuAuOtherAJ_lo);
  toa.Add(ppInAuAuOtherAJ_lo);
  toa.Add(AuAuAJ_hi);
  toa.Add(ppInAuAuAJ_hi);
      
  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");

    h->Rebin(2);
    // h->Scale(1./h->Integral());
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.22, "y");
    h->SetAxisRange(  0.0, 0.72, "x");

  }
  
  AuAuAJ_lo->SetLineColor( kBlack );
  AuAuAJ_lo->SetMarkerColor( kBlack );
  AuAuAJ_lo->SetMarkerStyle( 21 );

  ppInAuAuAJ_lo->SetLineColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerStyle( 25 );

  AuAuOtherAJ_lo->SetMarkerStyle( 21 );
  ppInAuAuOtherAJ_lo->SetMarkerStyle( 25 );
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    AuAuOtherAJ_lo->SetLineColor( kMagenta );
    AuAuOtherAJ_lo->SetMarkerColor( kMagenta );
    ppInAuAuOtherAJ_lo->SetLineColor( kMagenta );
    ppInAuAuOtherAJ_lo->SetMarkerColor( kMagenta );
  } else {
    AuAuOtherAJ_lo->SetLineColor( kGreen+2 );
    AuAuOtherAJ_lo->SetMarkerColor( kGreen+2 );
    ppInAuAuOtherAJ_lo->SetLineColor( kGreen+2 );
    ppInAuAuOtherAJ_lo->SetMarkerColor( kGreen+2 );
  }



  AuAuAJ_hi->SetLineColor( kRed );
  AuAuAJ_hi->SetMarkerColor( kRed );
  AuAuAJ_hi->SetMarkerStyle( 20 );

  ppInAuAuAJ_hi->SetLineColor( kRed );
  ppInAuAuAJ_hi->SetMarkerColor( kRed );
  ppInAuAuAJ_hi->SetMarkerStyle( 24 );
  
  ppInAuAuAJ_lo->GetYaxis()->SetTitleSize(0.04);
  ppInAuAuAJ_lo->GetYaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_lo->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_lo->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  ppInAuAuAJ_lo->GetXaxis()->SetTitleSize(0.04);
  ppInAuAuAJ_lo->GetXaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_lo->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_lo->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  
  ppInAuAuAJ_lo->GetYaxis()->SetTitleOffset(1.14);
  ppInAuAuAJ_lo->SetAxisRange( 0,0.22,"y");
  ppInAuAuAJ_lo->SetTitle(";|A_{J}|;Event Fraction");
  ppInAuAuAJ_lo->Draw("9");
  
  TH1D* AJ_hi_minmax=0;
  TH1D* AJ_lo_minmax=0;
  TH1D* OtherAJ_lo_minmax;
  if ( fSyst ){
    if (RefmultCut!= 351 ) {
      cerr << "CAUTION: Systematics were computed for RefmultCult == 351" << endl;
      return -1;
    }
    AJ_hi_minmax = (TH1D*) fSyst->Get("AJ_hi_minmax");
    AJ_hi_minmax->SetFillColor( kRed-10 );
    AJ_hi_minmax->SetMarkerColor(kRed-10);
    AJ_hi_minmax->SetMarkerSize(0);
    
    AJ_lo_minmax = (TH1D*) fSyst->Get("AJ_lo_minmax");
    AJ_lo_minmax->SetFillColor( kGray );
    AJ_lo_minmax->SetMarkerColor( kGray );
    AJ_lo_minmax->SetMarkerSize(0);

    OtherAJ_lo_minmax = (TH1D*) fSyst->Get("OtherAJ_lo_minmax");
    OtherAJ_lo_minmax->SetMarkerSize(0);
    if (TString(fAuAu->GetName()).Contains("R0.2") ){
      OtherAJ_lo_minmax->SetFillColor( kMagenta-10 );
    } else {
      OtherAJ_lo_minmax->SetFillColor( kGreen-9 );
    }
  }


  // if ( fSyst ){
  //   AJ_lo_minmax->Draw("9E2same");
  // }
  gSysMatchedComb->Draw("9e2");
  ppInAuAuAJ_lo->Draw("9same");
  AuAuAJ_lo->Draw("9same");
  //ppInAuAuAJ_hi->Draw("9same");
  // AuAuAJ_hi->Draw("9same");


  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    leg->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    // leg->AddEntry ( ppInAuAuOtherAJ_lo,     "pp HT #otimes AuAu MB Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    // leg->AddEntry ( AuAuOtherAJ_lo,         "AuAu HT Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
  } else {
    leg->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    // leg->AddEntry ( ppInAuAuOtherAJ_lo,     "pp HT #otimes AuAu MB Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    // leg->AddEntry ( AuAuOtherAJ_lo,         "AuAu HT Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
  }
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  leg->Draw();
  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);
  char plabel[400];

  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  } else {
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  }  
  latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");  
  
  // sprintf ( plabel, "p-value = %0.2g", ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") );
  latex.SetTextColor( AuAuAJ_lo->GetLineColor() );
  latex.DrawLatex( .6,.45, plabel);

  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/R0.2_HP_FollowUp_Part1.png");
  } else {
    gPad->SaveAs("plots/HP_FollowDown_Part1.png");
  }

  if ( fSyst ){
    OtherAJ_lo_minmax->Draw("9E2same");
  }
  ppInAuAuAJ_lo->Draw("9same");
  ppInAuAuOtherAJ_lo->Draw("9same");

  leg->Clear();
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    leg->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( ppInAuAuOtherAJ_lo,     "pp HT #otimes AuAu MB Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( (TH1D*) NULL,     "", "");
  } else {
    leg->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( ppInAuAuOtherAJ_lo,     "pp HT #otimes AuAu MB Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( (TH1D*) NULL,     "", "");
  }
  leg->Draw();

  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/R0.2_HP_FollowUp_Part2.png");
  } else {
    gPad->SaveAs("plots/HP_FollowDown_Part2.png");
  }

  
  if ( fSyst ){
    OtherAJ_lo_minmax->Draw("9E2same");
  }
  ppInAuAuAJ_lo->Draw("9same");
  AuAuAJ_lo->Draw("9same");  
  ppInAuAuOtherAJ_lo->Draw("9same");
  AuAuOtherAJ_lo->Draw("9same");  

  leg->Clear();
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    leg->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( ppInAuAuOtherAJ_lo,     "pp HT #otimes AuAu MB Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( AuAuOtherAJ_lo,         "AuAu HT Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
  } else {
    leg->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, R=0.4, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( ppInAuAuOtherAJ_lo,     "pp HT #otimes AuAu MB Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
    leg->AddEntry ( AuAuOtherAJ_lo,         "AuAu HT Matched, R=0.2, p_{T}^{Cut}>0.2 GeV/c", "p");
  }
  leg->Draw();

  // sprintf ( plabel, "p-value = %.2g", ppInAuAuOtherAJ_lo->KolmogorovTest(AuAuOtherAJ_lo, "") );
  // latex.SetTextColor( AuAuOtherAJ_lo->GetLineColor() );
  // latex.DrawLatex( .6,.4, plabel);

  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/R0.2_HP_FollowUp_Fig.png");
    gPad->SaveAs("plots/R0.2_HP_FollowUp_Fig.pdf");
  } else {
    gPad->SaveAs("plots/HP_FollowDown_Fig.png");
    gPad->SaveAs("plots/HP_FollowDown_Fig.pdf");
  }

  cerr << "OLD: Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut, LARGE R: " << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") << endl;
  cerr << "OLD: chi^2 test for pp @ AuAu vs. AuAu, LOW cut, LARGE R: " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;

  cerr << "OLD: Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut, SMALL R: " << ppInAuAuOtherAJ_lo->KolmogorovTest(AuAuOtherAJ_lo, "") << endl;
  cerr << "OLD: chi^2 test for pp @ AuAu vs. AuAu, LOW cut, SMALL R: " << ppInAuAuOtherAJ_lo->Chi2Test(AuAuOtherAJ_lo, "") << endl;
}

