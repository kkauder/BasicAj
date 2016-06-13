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
  
  bool nofabs=true;

  // axis labels and titles
  float ls = 0.05;
  float ts = 0.05;
  float yoff = 1.06;
  float xoff = 0.92;

  // legend
  float legs = 0.04;

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  // TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  // if ( nofabs ) {
  //   leg = new TLegend( 0.12, 0.7, 0.53, 0.9, "" );
  // }
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);

  TLegend* legem = new TLegend( 0.13, 0.7, 0.46, 0.88, " " );
  legem->SetBorderSize(0);
  // legem->SetTextColor( kRed);
  legem->SetTextSize(legs);
  // legem->SetLineWidth(10);
  legem->SetFillStyle(0);
  // legem->SetMargin(0.1);


  TLegend* leglo = new TLegend( 0.55, 0.7, 0.88, 0.88, "p_{T}^{Cut}>0.2 GeV/c, Matched:" );
  leglo->SetBorderSize(0);
  leglo->SetTextSize(legs);
  // leglo->SetLineWidth(10);
  leglo->SetFillStyle(0);
  // leglo->SetMargin(0.1);

  
  TFile *fAuAu         = TFile::Open("AjResults/Fresh_NicksList_HC100_AuAu.root");
  TFile *fRC           = TFile::Open("AjResults/Fresh_NicksList_HC100_RandomCone.root"); // RandomCone_Fresh_NicksList_HC100_AuAu.root
  TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root");
  TFile *fSyst         = TFile::Open("AjResults/Systematics_Fresh_NicksList_HC100_ppInAuAuAj.root");


  TH2D* h2 = (TH2D*) fAuAu->Get( "AJ_lo");
  if ( nofabs ) h2 = (TH2D*) fAuAu->Get( "NoFabsAJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();


  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* AuAuAJ_lo=0;
  TH1D* ppInAuAuAJ_lo=0;
  TH1D* ECAJ_lo=0;
  TH1D* RCAJ_lo=0;

  if ( nofabs ) {
    AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "NoFabsAJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "NoFabsAJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    ECAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "NoFabsEtaShiftAJ_lo") )->ProjectionX("ECAJ_lo", AuAuMultBinL, AuAuMultBinR);
    RCAJ_lo     = (TH1D*) ( (TH2D*) fRC->Get( "NoFabsRandomAJ_lo") )->ProjectionX("RCAJ_lo", AuAuMultBinL, AuAuMultBinR);
  } else {
    AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    ECAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "EtaShiftAJ_lo") )->ProjectionX("ECAJ_lo", AuAuMultBinL, AuAuMultBinR);
    RCAJ_lo     = (TH1D*) ( (TH2D*) fRC->Get( "RandomAJ_lo") )->ProjectionX("RCAJ_lo", AuAuMultBinL, AuAuMultBinR);
  }

  
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
    if ( nofabs ) h->SetTitle(";A_{J};Fraction");
      
    h->Rebin(2);
    // h->Scale(1./h->Integral());
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.23, "y");
    h->SetAxisRange(  0.0, 0.72, "x");
    if ( nofabs ) {
      h->SetAxisRange(  0.0, 0.25, "y");
      h->SetAxisRange(  -0.3, 0.8, "x");
    }

    h->GetXaxis()->SetLabelSize( ls );
    h->GetXaxis()->SetTitleSize( ls );
    h->GetXaxis()->SetTitleOffset( xoff );
    h->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetXaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

    h->GetYaxis()->SetLabelSize( ls );
    h->GetYaxis()->SetTitleSize( ls );
    h->GetYaxis()->SetTitleOffset( yoff );
    h->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
    
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
  // RCAJ_lo->SetFillColor( kGreen-7 );  
  // ECAJ_lo->SetFillStyle(3007 );
  // ECAJ_lo->SetFillColor( kAzure-4 );  
  // RCAJ_lo->SetFillStyle(3002 );
  // RCAJ_lo->SetFillColor( kGray+1 );  
  // ECAJ_lo->SetFillStyle(3007 );
  // ECAJ_lo->SetFillColor( kGray+1 );  

  RCAJ_lo->SetFillColor( kPink+6 );
  ECAJ_lo->SetFillColor( kAzure-4 );

  // ppInAuAuAJ_lo->GetYaxis()->SetTitleSize(0.04);
  // ppInAuAuAJ_lo->GetYaxis()->SetLabelSize(0.04);
  // ppInAuAuAJ_lo->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  // ppInAuAuAJ_lo->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  // ppInAuAuAJ_lo->GetXaxis()->SetTitleSize(0.04);
  // ppInAuAuAJ_lo->GetXaxis()->SetLabelSize(0.04);
  // ppInAuAuAJ_lo->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  // ppInAuAuAJ_lo->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  
  
  // ppInAuAuAJ_lo->GetYaxis()->SetTitleOffset(1.14);
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
    if ( nofabs ) sys_ppInAuAuAJ_lo = (TH1D*) fSyst->Get("NoFabsAJ_lo_minmax");

    // sys_ppInAuAuAJ_lo->SetFillStyle(3001 );
    sys_ppInAuAuAJ_lo->SetFillColor( kGray );
    sys_ppInAuAuAJ_lo->SetFillStyle(1001);
    sys_ppInAuAuAJ_lo->Draw("9E2same");
  }
  
  ppInAuAuAJ_lo->Draw("9same");
  AuAuAJ_lo->Draw("9same");
  
  leglo->AddEntry ( ppInAuAuAJ_lo, "pp HT #oplus AuAu MB", "p");
  leglo->AddEntry ( AuAuAJ_lo,     "AuAu HT", "p");
  leglo->Draw();

  legem->AddEntry ( RCAJ_lo,       "Random Cone Bkg (RC)", "f");
  legem->AddEntry ( ECAJ_lo,       "Eta Cone Bkg (EC)", "f");
  legem->Draw();


  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);
  if ( nofabs ) latex.SetTextSize(0.037);    

  latex.SetTextSize( 0.04);
  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  } else {
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  }  

  latex.SetTextColor(kBlack);
  latex.SetTextSize( 0.04);
  // latex.DrawLatex( 0.05, 0.95, "p_{T,lead}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  // latex.DrawLatex( 0.54, 0.95, "p_{T,sublead}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");

  latex.DrawLatex( 0.65, 0.5, "With p_{T}^{Cut}>2 GeV/c:");

  latex.DrawLatex( 0.65, 0.45, "  p_{T,lead}>20 Gev/c");
  latex.DrawLatex( 0.65, 0.4, "  p_{T,sublead}>10 Gev/c");

  // if ( nofabs ) {
  //   latex.DrawLatex( 0.57, 0.84, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  //   latex.DrawLatex( 0.57, 0.78, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");
  // } else {
  //   latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  //   latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");
  //}

  
  gPad->SaveAs("plots/BkgFigure.png");
  gPad->SaveAs("plots/BkgFigure.pdf");

  // Line to guide the eye
  TLine line;
  line.SetLineStyle( 2 );
  line.SetLineColor( kGray+2 );
  gPad->Update();
  line.DrawLine( 0, gPad->GetFrame()->GetY1(), 0, gPad->GetFrame()->GetY2() );



  // leg->AddEntry ( ppInAuAuAJ_lo, "pp HT #oplus AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  // leg->AddEntry ( AuAuAJ_lo,     "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  // leg->AddEntry ( RCAJ_lo,       "Random Cone Bkg (RC)", "f");
  // leg->AddEntry ( ECAJ_lo,       "Eta Cone Bkg (EC)", "f");

  cout << "chi^2 between RC and AuAu: " << RCAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;
  cout << "chi^2 between EC and AuAu: " << ECAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;
  cout << "chi^2 between RC and EC: " << RCAJ_lo->Chi2Test(ECAJ_lo, "") << endl;

  
}

