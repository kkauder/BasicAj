#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void MakePtPlot() {

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  // int RefmultCut = 351;  // 269 for 0-20%, 399 for 0-10%

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

  TFile *fPt1AuAu         = TFile::Open("AjResults/Pt1_Presel_AuAuAj.root");
  TFile *fPt1ppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_Pt1_ppInAuAuAj.root");
  TFile *fPt1Syst         = TFile::Open("AjResults/Pt1_Systematics_ppInAuAuAj.root");

  TFile *fAuAu         = TFile::Open("AjResults/Presel_AuAuAj.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_ppInAuAuAj.root");
  TFile *fSyst         = TFile::Open("AjResults/Systematics_ppInAuAuAj.root");


  TObjArray vAJ_lo;
  TObjArray vAJ_hi;

  TH2D* h2 = (TH2D*) fAuAu->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();

  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  cout << AuAuAJ_lo ->Integral(1,100) << endl;

  TH1D* AuAuAJ_hi     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_hi") )->ProjectionX("AuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAuAJ_hi = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_hi") )->ProjectionX("ppInAuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
   
  TH1D* Pt1AuAuAJ_lo     = (TH1D*) ( (TH2D*) fPt1AuAu->Get( "AJ_lo") )->ProjectionX("Pt1AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* Pt1ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fPt1ppInAuAu->Get( "AJ_lo") )->ProjectionX("Pt1ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  cout << AuAuAJ_lo ->Integral(1,100) << endl;

  TH1D* Pt1AuAuAJ_hi     = (TH1D*) ( (TH2D*) fPt1AuAu->Get( "AJ_hi") )->ProjectionX("Pt1AuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* Pt1ppInAuAuAJ_hi = (TH1D*) ( (TH2D*) fPt1ppInAuAu->Get( "AJ_hi") )->ProjectionX("Pt1ppInAuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);

  TObjArray toa;
  toa.Add(AuAuAJ_lo);
  toa.Add(ppInAuAuAJ_lo);
  toa.Add(AuAuAJ_hi);
  toa.Add(ppInAuAuAJ_hi);
  toa.Add(Pt1AuAuAJ_lo);
  toa.Add(Pt1ppInAuAuAJ_lo);
  toa.Add(Pt1AuAuAJ_hi);
  toa.Add(Pt1ppInAuAuAJ_hi);
      
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

  AuAuAJ_hi->SetLineColor( kRed );
  AuAuAJ_hi->SetMarkerColor( kRed );
  AuAuAJ_hi->SetMarkerStyle( 20 );

  ppInAuAuAJ_hi->SetLineColor( kRed );
  ppInAuAuAJ_hi->SetMarkerColor( kRed );
  ppInAuAuAJ_hi->SetMarkerStyle( 24 );
  

  Pt1AuAuAJ_lo->SetLineColor( kBlue+1 );
  Pt1AuAuAJ_lo->SetMarkerColor( kBlue+1 );
  Pt1AuAuAJ_lo->SetMarkerStyle( 33 );

  Pt1ppInAuAuAJ_lo->SetLineColor( kBlue+1 );
  Pt1ppInAuAuAJ_lo->SetMarkerColor( kBlue+1 );
  Pt1ppInAuAuAJ_lo->SetMarkerStyle( 27 );

  Pt1AuAuAJ_hi->SetLineColor( kRed );
  Pt1AuAuAJ_hi->SetMarkerColor( kRed );
  Pt1AuAuAJ_hi->SetMarkerStyle( 20 );

  Pt1ppInAuAuAJ_hi->SetLineColor( kRed );
  Pt1ppInAuAuAJ_hi->SetMarkerColor( kRed );
  Pt1ppInAuAuAJ_hi->SetMarkerStyle( 24 );


  
  ppInAuAuAJ_hi->GetYaxis()->SetTitleSize(0.04);
  ppInAuAuAJ_hi->GetYaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_hi->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_hi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  ppInAuAuAJ_hi->GetXaxis()->SetTitleSize(0.04);
  ppInAuAuAJ_hi->GetXaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_hi->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_hi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  
  
  ppInAuAuAJ_hi->GetYaxis()->SetTitleOffset(1.14);
  ppInAuAuAJ_hi->Draw("9");

  TH1D* AJ_hi_minmax=0;
  TH1D* AJ_lo_minmax=0;
  TH1D* Pt1AJ_hi_minmax=0;
  TH1D* Pt1AJ_lo_minmax=0;
  if ( fSyst ){
    if (RefmultCut!= 269 ) {
      cerr << "CAUTION: Systematics were computed for RefmultCult == 269" << endl;
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

    Pt1AJ_hi_minmax = (TH1D*) fPt1Syst->Get("AJ_hi_minmax");
    Pt1AJ_hi_minmax->SetFillColor( kRed-10 );
    Pt1AJ_hi_minmax->SetMarkerColor( kRed-10 );
    Pt1AJ_hi_minmax->SetMarkerSize(0);

    Pt1AJ_lo_minmax = (TH1D*) fPt1Syst->Get("AJ_lo_minmax");
    Pt1AJ_lo_minmax->SetFillColor( kBlue-10 );  
    Pt1AJ_lo_minmax->SetMarkerColor( kBlue-10 );
    Pt1AJ_lo_minmax->SetMarkerSize(0);
  }

  if ( fSyst ){
    AJ_hi_minmax->Draw("9E2same");
  }
  AuAuAJ_hi->Draw("9same");
  ppInAuAuAJ_hi->Draw("9same");

  leg->AddEntry ( Pt1ppInAuAuAJ_hi, "pp HT #otimes AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( Pt1AuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  leg->AddEntry ( (TH1D*) NULL,     "", "");

  leg->Draw();

    TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);
  char plabel[400];

  float threshhi = 1e-5;
  // if ( ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") >1e-4  || ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") >1e-4 ){
  if ( ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") > threshhi ){
    cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") << endl;
    cerr << "chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") << endl;
    return;    
  }

  sprintf ( plabel, "p-value < %g", threshhi);
  latex.SetTextColor( AuAuAJ_hi->GetLineColor() );
  latex.DrawLatex( .6,.5, plabel);


  latex.SetTextColor(kBlack);
  latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");

  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  
  gPad->SaveAs("plots/Pt1_Part1.png");

  // part 2
  // ------
  Pt1AJ_lo_minmax->Draw("9E2same");
  Pt1ppInAuAuAJ_lo->Draw("9same");
  Pt1AuAuAJ_lo->Draw("9same");

  leg->Clear();
  leg->AddEntry ( Pt1ppInAuAuAJ_hi, "pp HT #otimes AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( Pt1AuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( Pt1ppInAuAuAJ_lo, "pp HT #otimes AuAu MB Matched, p_{T}^{Cut}>1.0 GeV/c", "p");
  leg->AddEntry ( Pt1AuAuAJ_lo,     "AuAu HT Matched, p_{T}^{Cut}>1.0 GeV/c", "p");
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  leg->Draw();

  sprintf ( plabel, "p-value = %.2g", Pt1ppInAuAuAJ_lo->KolmogorovTest(Pt1AuAuAJ_lo, "") );
  latex.SetTextColor( Pt1ppInAuAuAJ_lo->GetLineColor() );
  latex.DrawLatex( .6,.45, plabel);

  gPad->SaveAs("plots/Pt1_Part2.png");

  // part 3
  // ------
  AJ_lo_minmax->Draw("9E2same");
  ppInAuAuAJ_lo->Draw("9same");
  AuAuAJ_lo->Draw("9same");

  leg->Clear();
  leg->AddEntry ( Pt1ppInAuAuAJ_hi, "pp HT #otimes AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( Pt1AuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( Pt1ppInAuAuAJ_lo, "pp HT #otimes AuAu MB Matched, p_{T}^{Cut}>1.0 GeV/c", "p");
  leg->AddEntry ( Pt1AuAuAJ_lo,     "AuAu HT Matched, p_{T}^{Cut}>1.0 GeV/c", "p");
  leg->AddEntry ( AuAuAJ_lo,        "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( ppInAuAuAJ_lo,    "pp HT #otimes AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->Draw();

  float threshlo = 0.05;

  if ( ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") < threshlo ){
    cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut: " << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") << endl;
    cerr << "chi^2 test for pp @ AuAu vs. AuAu, LOW cut: " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;
    return;    
  }    
  sprintf ( plabel, "p-value >> %g", threshlo);
  latex.SetTextColor( AuAuAJ_lo->GetLineColor() );
  latex.DrawLatex( .6,.4, plabel);

  gPad->SaveAs("plots/Pt1Plot.png");

  
}


