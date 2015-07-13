#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void BkgFigure() {
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

  TFile *fAuAu         = TFile::Open("AjResults/Presel_AuAuAj.root");
  // TFile *fAuAu         = TFile::Open("AjResults/RefmultMatched.root");
  // TFile *fAuAu         = TFile::Open("AjResults/EtaMatchedToOrig.root");
  // TFile *fAuAu         = TFile::Open("AjResults/JStyleEC.root");
  TFile *fRC           = TFile::Open("AjResults/RandomCone.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_ppInAuAuAj.root");
  TFile *fSyst         = TFile::Open("AjResults/Systematics_ppInAuAuAj.root");

  TH2D* h2 = (TH2D*) fAuAu->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut ); // 269 for 0-20%
  int AuAuMultBinR = h2->GetNbinsY();


  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);

  TH1D* ECAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "EtaShiftAJ_lo") )->ProjectionX("ECAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* RCAJ_lo     = (TH1D*) ( (TH2D*) fRC->Get( "RandomAJ_lo") )->ProjectionX("RCAJ_lo", AuAuMultBinL, AuAuMultBinR);

  
  TObjArray toa;
  toa.Add(AuAuAJ_lo);
  toa.Add(ppInAuAuAJ_lo);
  toa.Add(ECAJ_lo);
  toa.Add(RCAJ_lo);  
  
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

  RCAJ_lo->SetMarkerStyle(0);
  ECAJ_lo->SetMarkerStyle(0);
  // RCAJ_lo->SetFillStyle(3002 );
  RCAJ_lo->SetFillColor( kGreen-7 );  
  // ECAJ_lo->SetFillStyle(3007 );
  ECAJ_lo->SetFillColor( kAzure-4 );  
  // RCAJ_lo->SetFillStyle(3002 );
  // RCAJ_lo->SetFillColor( kGray+1 );  
  // ECAJ_lo->SetFillStyle(3007 );
  // ECAJ_lo->SetFillColor( kGray+1 );  


  ppInAuAuAJ_lo->GetYaxis()->SetTitleSize(0.04);
  ppInAuAuAJ_lo->GetYaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_lo->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_lo->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  ppInAuAuAJ_lo->GetXaxis()->SetTitleSize(0.04);
  ppInAuAuAJ_lo->GetXaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_lo->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_lo->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  
  
  ppInAuAuAJ_lo->GetYaxis()->SetTitleOffset(1.14);
  ppInAuAuAJ_lo->Draw("9");

  // ECAJ_lo->Draw("9E2same");
  // RCAJ_lo->Draw("9E2same");

  ECAJ_lo->Draw("9E3same");
  RCAJ_lo->Draw("9E3same");

  if ( fSyst ){
    if (RefmultCut!= 269 ) {
      cerr << "CAUTION: Systematics were computed for RefmultCut == 269" << endl;
      return -1;
    }
    TH1D* sys_ppInAuAuAJ_lo = (TH1D*) fSyst->Get("AJ_lo_minmax");
    // AJ_lo_minmax->SetFillStyle(3001 );
    AJ_lo_minmax->SetFillColor( kGray );  
    AJ_lo_minmax->Draw("9E2same");
  }
  
  ppInAuAuAJ_lo->Draw("9same");
  AuAuAJ_lo->Draw("9same");
  
  leg->AddEntry ( ppInAuAuAJ_lo, "pp HT #otimes AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( AuAuAJ_lo,     "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( RCAJ_lo,       "Random Cone Bkg (RC)", "f");
  leg->AddEntry ( ECAJ_lo,       "Eta Cone Bkg (EC)", "f");
  leg->Draw();

  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);

  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  } else {
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  }  

  latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");

  
  gPad->SaveAs("plots/BkgFigure.png");
  gPad->SaveAs("plots/BkgFigure.pdf");

  
}

