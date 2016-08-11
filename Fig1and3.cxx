#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

int Fig1and3( TString R = "" ) {

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  // int RefmultCut = 351;  // 269 for 0-20%, 399 for 0-10%

  // This would give 40-100%:
  // int AuAuMultBinL = h2->GetYaxis()->FindBin( 0 ); 
  // int AuAuMultBinR = h2->GetYaxis()->FindBin( 113 ); 
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
	

  bool nofabs=true;
  // bool nofabs=false;

  // axis labels and titles
  float ls = 0.05;
  float ts = 0.05;
  float yoff = 1.06;
  float xoff = 0.92;

  // legend
  float legs = 0.04;

  // 100% hadronic correction
  // ========================
  if ( R=="" ){
    TFile *fAuAu         = TFile::Open("AjResults/Fresh_NicksList_HC100_AuAu.root");
    TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root");
    TFile *fSyst         = TFile::Open("AjResults/Systematics_Fresh_NicksList_HC100_ppInAuAuAj.root");
    // // cross check
    // TFile *fAuAu         = TFile::Open("AjResults/Groom_HT64_AuAu.root");
    // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_Groom_HT64_ppInAuAuAj.root");
    // TFile *fSyst         = TFile::Open("AjResults/Systematics_Groom_HT64_ppInAuAuAj.root");

  } else if ( R.Contains("0.2") ){
    TFile *fAuAu         = TFile::Open("AjResults/R0.2_Fresh_NicksList_HC100_AuAu.root");
    TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_R0.2_Fresh_NicksList_HC100_ppInAuAuAj.root");
    TFile *fSyst         = TFile::Open("AjResults/Systematics_R0.2_Fresh_NicksList_HC100_ppInAuAuAj.root");
  } else return 0;
  
  // Others
  // ======
  // TFile *fAuAu         = TFile::Open("AjResults/rndm2/HC30_Presel.root");
  // TFile *fAuAu         = TFile::Open("AjResults/FixedTowers_AuAu.root");
  // TFile *fAuAu         = TFile::Open("AjResults/OldTowers_AuAu.root");
  // TFile *fAuAu         = TFile::Open("AjResults/OldCodeTowers_AuAu.root");
  // TFile *fAuAu         = TFile::Open("AjResults/HC30_Presel.root");

  // TFile *fAuAu         = TFile::Open("AjResults/HC30_Presel.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_HC30_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/Systematics_HC30_ppInAuAuAj.root");

  // // TFile *fAuAu         = TFile::Open("AjResults/rndm1/R0.2_HC30_Presel.root");
  // TFile *fAuAu         = TFile::Open("AjResults/R0.2_HC30_Presel.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_R0.2_HC30_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/Systematics_R0.2_HC30_ppInAuAuAj.root");

  // // MIP correction
  // // TFile *fAuAu         = TFile::Open("AjResults/rndm1/Presel_AuAuAj.root");
  // TFile *fAuAu         = TFile::Open("AjResults/Presel_AuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/Systematics_ppInAuAuAj.root");

  // // TFile *fAuAu         = TFile::Open("AjResults/rndm1/R0.2_Presel_AuAuAj.root");
  // TFile *fAuAu         = TFile::Open("AjResults/R0.2_Presel_AuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_R0.2_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/R0.2_Systematics_ppInAuAuAj.root");
  
  TString RootOut="plots/R0.4_Fig.root";
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    RootOut="plots/R0.2_Fig.root";
  }

  if (TString(fppInAuAu->GetName()).Contains("MixTest") ){
    RootOut.ReplaceAll( "Fig", "MixTest_Fig");
  }

  // if (!( TString(fAuAu->GetName()).Contains("HC30") ) ){
  //   RootOut.ReplaceAll( "Fig", "MIP_Fig");
  // }

  TFile* out = new TFile( RootOut, "RECREATE");

  TObjArray vAJ_lo;
  TObjArray vAJ_hi;

  TH2D* h2 = (TH2D*) fAuAu->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();
  
  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* AuAuAJ_lo=0;
  if ( nofabs ) AuAuAJ_lo = (TH1D*) ( (TH2D*) fAuAu->Get( "NoFabsAJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  else AuAuAJ_lo = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAuAJ_lo=0;
  if ( nofabs ) ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "NoFabsAJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  else ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  cout << AuAuAJ_lo ->Integral(1,100) << " dijets in AuAu" << endl;
  cout << ppInAuAuAJ_lo ->Integral(1,100) << " dijets in pp@AuAu" << endl;
  
  // TH1D* AuAuAJ_hi     = (TH1D*) ( (TH2D*) fAuAu->Get( "UnmatchedAJ_hi") )->ProjectionX("AuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
  // TH1D* ppInAuAuAJ_hi = (TH1D*) ( (TH2D*) fppInAuAu->Get( "UnmatchedAJ_hi") )->ProjectionX("ppInAuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* AuAuAJ_hi     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_hi") )->ProjectionX("AuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAuAJ_hi = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_hi") )->ProjectionX("ppInAuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);

     
  TObjArray toa;
  toa.Add(AuAuAJ_lo);
  toa.Add(ppInAuAuAJ_lo);
  toa.Add(AuAuAJ_hi);
  toa.Add(ppInAuAuAJ_hi);
  
  TH1D* h;
  cout.precision(4);
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    cout << h->GetName() << ": <AJ> = " << h->GetMean() << " +/- " << h->GetMeanError() << endl;

    h->SetLineWidth( 2 );

    h->SetTitle(";|A_{J}|;Fraction");
    if ( nofabs ) h->SetTitle(";A_{J};Fraction");
    
    h->Rebin(2);
    // h->Scale(1./h->Integral());
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.23, "y");
    h->SetAxisRange(  0.0, 0.72, "x");
    if ( nofabs ) h->SetAxisRange(  0.0, 0.25, "y");
    if ( nofabs ) h->SetAxisRange(  -0.3, 0.8, "x");

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
  // AuAuAJ_lo->SetLineColor( kBlue );
  // AuAuAJ_lo->SetMarkerColor( kBlue );  
  AuAuAJ_lo->SetMarkerStyle( 21 );

  ppInAuAuAJ_lo->SetLineColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerColor( kBlack );
  // ppInAuAuAJ_lo->SetLineColor( kBlue );
  // ppInAuAuAJ_lo->SetMarkerColor( kBlue );
  ppInAuAuAJ_lo->SetMarkerStyle( 25 );

  AuAuAJ_hi->SetLineColor( kRed );
  AuAuAJ_hi->SetMarkerColor( kRed );
  // AuAuAJ_hi->SetLineColor( kMagenta );
  // AuAuAJ_hi->SetMarkerColor( kMagenta );
  AuAuAJ_hi->SetMarkerStyle( 20 );

  ppInAuAuAJ_hi->SetLineColor( kRed );
  ppInAuAuAJ_hi->SetMarkerColor( kRed );
  // ppInAuAuAJ_hi->SetLineColor( kMagenta );
  // ppInAuAuAJ_hi->SetMarkerColor( kMagenta );
  ppInAuAuAJ_hi->SetMarkerStyle( 24 );
  
  // ppInAuAuAJ_hi->GetYaxis()->SetTitleSize(0.04);
  // ppInAuAuAJ_hi->GetYaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_hi->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_hi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  // ppInAuAuAJ_hi->GetXaxis()->SetTitleSize(0.04);
  // ppInAuAuAJ_hi->GetXaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_hi->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_hi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  
  
  //  ppInAuAuAJ_hi->GetYaxis()->SetTitleOffset(1.14);
  if (TString(fAuAu->GetName()).Contains("HT54") ) ppInAuAuAJ_hi->SetTitle( "Offline HT 5.4");
  if (TString(fAuAu->GetName()).Contains("HT64") ) ppInAuAuAJ_hi->SetTitle( "Offline HT 6.4");    
  TH1D* dummy = ppInAuAuAJ_lo->Clone("dummy");
  dummy->Reset();
  dummy->SetLineWidth(0);
  dummy->SetAxisRange(  0.0, 0.23, "y");
  dummy->SetAxisRange(  0.0, 0.72, "x");
  if ( nofabs ) dummy->SetAxisRange(   0.0, 0.25, "y");
  if ( nofabs ) dummy->SetAxisRange(  -0.3, 0.8, "x");
  dummy->Draw("9");
  
  ppInAuAuAJ_hi->SetAxisRange(0, 1, "x");
  AuAuAJ_hi->SetAxisRange(0, 1, "x");

  ppInAuAuAJ_hi->Draw("9same");
  
  TH1D* AJ_hi_minmax=0;
  TH1D* AJ_lo_minmax=0;
  bool ShowPvalue=false;
  TParameter<double>* Tpar=0;
  
  // The full range of available values. Only loading some of them
  double Chi2_NominalE_Hi;
  double Chi2_NominalE_Lo;
  double Chi2_NominalT_Hi;
  double Chi2_NominalT_Lo;
  double BinKS_NominalE_Hi;
  double BinKS_NominalE_Lo;
  double BinKS_NominalT_Hi;
  double BinKS_NominalT_Lo;
  double KS_NominalE_Hi;
  double KS_NominalE_Lo;
  double KS_NominalT_Hi;
  double KS_NominalT_Lo;
  double Chi2_PlusE_Hi;
  double Chi2_PlusE_Lo;
  double Chi2_PlusT_Hi;
  double Chi2_PlusT_Lo;
  double BinKS_PlusE_Hi;
  double BinKS_PlusE_Lo;
  double BinKS_PlusT_Hi;
  double BinKS_PlusT_Lo;
  double KS_PlusE_Hi;
  double KS_PlusE_Lo;
  double KS_PlusT_Hi;
  double KS_PlusT_Lo;
  double Chi2_MinusE_Hi;
  double Chi2_MinusE_Lo;
  double Chi2_MinusT_Hi;
  double Chi2_MinusT_Lo;
  double BinKS_MinusE_Hi;
  double BinKS_MinusE_Lo;
  double BinKS_MinusT_Hi;
  double BinKS_MinusT_Lo;
  double KS_MinusE_Hi;
  double KS_MinusE_Lo;
  double KS_MinusT_Hi;
  double KS_MinusT_Lo;

  if ( fSyst ){
    if (RefmultCut!= 269 ) {
      cerr << "CAUTION: Systematics were computed for RefmultCult == 269" << endl;
      return -1;
    }
    AJ_hi_minmax = (TH1D*) fSyst->Get("AJ_hi_minmax");
    AJ_hi_minmax->SetFillColor( kRed-10 );
    AJ_hi_minmax->SetFillStyle(1001);
    AJ_hi_minmax->SetMarkerColor(kRed-10);
    AJ_hi_minmax->SetMarkerSize(0);
    AJ_hi_minmax->SetLineWidth( 0 );
    
    if ( nofabs ) AJ_lo_minmax = (TH1D*) fSyst->Get("NoFabsAJ_lo_minmax");
    else AJ_lo_minmax = (TH1D*) fSyst->Get("AJ_lo_minmax");
    AJ_lo_minmax->SetFillColor( kGray );
    AJ_lo_minmax->SetFillStyle(1001);
    AJ_lo_minmax->SetMarkerColor( kGray );
    AJ_lo_minmax->SetMarkerSize(0);
    AJ_lo_minmax->SetLineWidth( 0 );

    // Also pull p-values from that file
    Tpar=(TParameter<double>*) fSyst->Get("KS_NominalE_Hi");
    KS_NominalE_Hi = Tpar->GetVal();
    
    if ( nofabs ) Tpar=(TParameter<double>*) fSyst->Get("KS_NoFabsNominalE_Lo");
    else Tpar=(TParameter<double>*) fSyst->Get("KS_NominalE_Lo");
    KS_NominalE_Lo = Tpar->GetVal();
  }
  
  // // USING JOERN'S RESULT
  // // ---------------------
  // TFile* Jresult = new TFile("Jresults.root");
  // TH1D* AuAuAJ_lo     = (TH1D*) Jresult->Get("hAjMatchedA");
  // TH1D* ppInAuAuAJ_lo = (TH1D*) Jresult->Get("hAjMatchedEmb");
  // TH1D* AuAuAJ_hi     = (TH1D*) Jresult->Get("hAjA");
  // TH1D* ppInAuAuAJ_hi = (TH1D*) Jresult->Get("hAjEmb");

  // // Adjust syst errors as well
  // TH1D* AJ_hi_minmax = (TH1D*) ppInAuAuAJ_hi->Clone("AJ_hi_minmax" );
  // TH1D* AJ_lo_minmax = (TH1D*) ppInAuAuAJ_lo->Clone("AJ_lo_minmax" );
  // for (int i=1; i<=ppInAuAuAJ_hi->GetNbinsX() ; ++i){
  //   AJ_hi_minmax->SetBinError(i, 0);
  //   AJ_lo_minmax->SetBinError(i, 0);
  // }

  // TGraphErrors* gSysComb = (TGraphErrors*) Jresult->Get("gSysComb");
  // TGraphErrors* gSysMatchedComb = (TGraphErrors*) Jresult->Get("gSysMatchedComb");

  // double x, y, ey;
  // for (int i=0; i< gSysComb->GetN() ; ++i){
  //   gSysComb->GetPoint( i, x, y );
  //   int bin = AuAuAJ_hi->FindBin( x );    
  //   if ( fabs ( ppInAuAuAJ_hi->GetBinContent(bin) - y ) >1e-5 ){
  //     cerr << " Incompatible signal and syst histo. " << endl;
  //     return;
  //   }
  //   ey = gSysComb->GetErrorY( i );
  //   // AJ_hi_minmax->SetBinContent(i, y);
  //   AJ_hi_minmax->SetBinError(i, ey);    
  // }
  // for (int i=0; i<gSysMatchedComb->GetN() ; ++i){
  //   gSysMatchedComb->GetPoint( i, x, y );
  //   int bin = AuAuAJ_lo->FindBin( x );    
  //   if ( fabs ( ppInAuAuAJ_lo->GetBinContent(bin) - y ) >1e-5 ){
  //     cerr << " Incompatible signal and syst histo. " << endl;
  //     return;
  //   }
  //   ey = gSysMatchedComb->GetErrorY( i );
  //   // AJ_lo_minmax->SetBinContent(i, y);    
  //   AJ_lo_minmax->SetBinError(i, ey);    
  // }
  AJ_hi_minmax->SetAxisRange(0,1,"x");
  AJ_hi_minmax->SetFillColor( kRed-10 );
  AJ_hi_minmax->SetMarkerColor(kRed-10);
  AJ_hi_minmax->SetMarkerSize(0);
  
  AJ_lo_minmax->SetFillColor( kGray );
  AJ_lo_minmax->SetMarkerColor( kGray );
  AJ_lo_minmax->SetMarkerSize(0);

  ppInAuAuAJ_hi->Draw("9same");
  AJ_hi_minmax->Draw("9E2same");

  ppInAuAuAJ_hi->Draw("9same");
  AuAuAJ_hi->Draw("9same");

  // // For p-value, add systematics in quadrature
  // TH1D* AJ_hi_sysstat = (TH1D*) ppInAuAuAJ_hi->Clone("AJ_hi_sysstat");
  // if ( AJ_hi_minmax ){
  //   for (int i=1; i<AJ_hi_sysstat->GetNbinsX(); ++i ){
  //     if ( fabs (AJ_hi_sysstat->GetBinContent(i)-AJ_hi_minmax->GetBinContent(i))> 1e-4 ){
  // 	cerr << "syst. and stat. histo are incompatible" << endl;
  // 	return -1;
  //     }
  //     AJ_hi_sysstat->SetBinError( i, TMath::Sqrt(
  // 						 pow( AJ_hi_sysstat->GetBinError(i), 2) +
  // 						 pow( AJ_hi_minmax->GetBinError(i), 2) ));
  //   }
  // }

  // TH1D* AJ_lo_sysstat = (TH1D*) ppInAuAuAJ_lo->Clone("AJ_lo_sysstat");
  // if ( AJ_lo_minmax ){
  //   for (int i=1; i<AJ_lo_sysstat->GetNbinsX(); ++i ){
  //     if ( fabs (AJ_lo_sysstat->GetBinContent(i)-AJ_lo_minmax->GetBinContent(i))> 1e-3 ){
  // 	cerr << "syst. and stat. histo are incompatible" << endl;
  // 	new TCanvas;
  // 	ppInAuAuAJ_lo->Draw();
  // 	AJ_lo_minmax->Draw("same");
  // 	return -1;
  //     }
  //     AJ_lo_sysstat->SetBinError( i, TMath::Sqrt(
  // 						 pow( AJ_lo_sysstat->GetBinError(i), 2) +
  // 						 pow( AJ_lo_minmax->GetBinError(i), 2) ));
  //   }
  // }

  
  // TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  // if ( nofabs ) {
  //   leg = new TLegend( 0.12, 0.7, 0.53, 0.9, "" );
  // }
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);
  // leg->AddEntry ( ppInAuAuAJ_hi, "pp HT #oplus AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  // leg->AddEntry ( AuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  // 
  // // TLegendEntry* l1 = leg->AddEntry ( (TH1D*) NULL,          "pp HT #oplus AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "");
  // // l1->SetTextColor(kWhite); // Doesn't Work :(
  // leg->AddEntry ( (TH1D*) NULL,     "", "");
  // leg->AddEntry ( (TH1D*) NULL,     "", "");
  
  // leg->Draw();
  // return;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(ls);
  // latex.SetTextSize(0.04);
  // if ( nofabs ) latex.SetTextSize(0.037);
  char plabel[400];


  // sprintf ( plabel, "p-value = %0.2g", ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, ""));
  // sprintf ( plabel, "p-value = %0.2g", ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, ""));
  if ( ShowPvalue ){
    sprintf ( plabel, "p-value = %0.1g", KS_NominalE_Hi);
    latex.SetTextSize(0.04);
    latex.SetTextColor( AuAuAJ_hi->GetLineColor() );
    // if ( nofabs ) latex.DrawLatex( .7,.5, plabel);
    // if ( nofabs ) latex.DrawLatex( .65,.5, plabel);
    // if ( nofabs ) latex.DrawLatex( .14,.6, plabel);
    if ( nofabs ) latex.DrawLatex( .33,.23, plabel);
    else latex.DrawLatex( .6,.5, plabel);
  }

  // cout << "HI: Nominal chi^2: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") << endl;
  // ppInAuAuAJ_hi->Scale(50000);AuAuAJ_hi->Scale(1000);
  // cout << "HI: Scaled chi^2: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") << endl;
  // // cout << "HI: NormOpt chi^2: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "NORM") << endl;
  // cout << "LO: Nominal chi^2: " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;
  // ppInAuAuAJ_lo->Scale(50000);AuAuAJ_lo->Scale(1000);
  // cout << "LO: NormOpt chi^2: " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;
  // // cout << "LO: NormOpt chi^2: " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "NORM") << endl;

  // return 0;

  // latex.SetTextColor(kBlack);
  // if ( nofabs ) {
  //   latex.DrawLatex( 0.57, 0.84, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 GeV/c");
  //   latex.DrawLatex( 0.57, 0.78, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 GeV/c");
  // } else {
  //   latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 GeV/c");
  //   latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 GeV/c");
  // }

  latex.SetTextColor(kBlack);
  latex.SetTextSize( 0.04);
  // latex.DrawLatex( 0.05, 0.95, "p_{T,lead}(p_{T}^{Cut}>2 GeV/c)>20 GeV/c");
  // latex.DrawLatex( 0.54, 0.95, "p_{T,sublead}(p_{T}^{Cut}>2 GeV/c)>10 GeV/c");

  latex.DrawLatex( 0.65, 0.5, "With p_{T}^{Cut}>2 GeV/c:");

  latex.DrawLatex( 0.65, 0.45, "  p_{T,lead}>20 GeV/c");
  latex.DrawLatex( 0.65, 0.4, "  p_{T,sublead}>10 GeV/c");


  // if (TString(fAuAu->GetName()).Contains("R0.2") ){
  // latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>16 GeV/c");
  // latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>8 GeV/c");
  // } else {
  //   latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 GeV/c");
  //   latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 GeV/c");
  // }
  
  latex.SetTextSize( 0.04);
  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  } else {
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  }  

  // if (TString(fAuAu->GetName()).Contains("R0.2") ){
  //   gPad->SaveAs("plots/R0.2_Part1.png");
  // } else if (TString(fAuAu->GetName()).Contains("Pt1") ){
  //   gPad->SaveAs("plots/Pt1_Part1.png");
  // } else {
  //   gPad->SaveAs("plots/R0.4_Part1.png");
  // }



  // Second part of animation
  // ------------------------
  AJ_lo_minmax->Draw("9E2same");

  ppInAuAuAJ_hi->Draw("9same");
  AuAuAJ_hi->Draw("9same");
  ppInAuAuAJ_lo->Draw("9same");
  AuAuAJ_lo->Draw("9same");

  TLegend* leghi = new TLegend( 0.15, 0.7, 0.48, 0.88, "p_{T}^{Cut}>2 GeV/c:" );
  if ( !nofabs )leghi = new TLegend( 0.35, 0.75, 0.68, 0.88, "p_{T}^{Cut}>2 GeV/c:" );
  leghi->SetBorderSize(0);
  leghi->SetTextColor( kRed);
  leghi->SetTextSize(legs);
  if ( !nofabs )  leghi->SetTextSize(.6*legs);
  // leghi->SetLineWidth(10);
  leghi->SetFillStyle(0);
  // leghi->SetMargin(0.1);

  leghi->AddEntry ( ppInAuAuAJ_hi, "p+p HT #oplus Au+Au MB", "p");
  leghi->AddEntry ( AuAuAJ_hi,     "Au+Au HT", "p");
  leghi->Draw();

  TLegend* leglo = new TLegend( 0.55, 0.7, 0.88, 0.88, "p_{T}^{Cut}>0.2 GeV/c, Matched:" );
  if ( !nofabs )  leglo = new TLegend( 0.65, 0.75, 0.88, 0.88, "p_{T}^{Cut}>0.2 GeV/c, Matched:" );
  leglo->SetBorderSize(0);
  leglo->SetTextSize(legs);
  if ( !nofabs )  leglo->SetTextSize(.6*legs);
  // leglo->SetLineWidth(10);
  leglo->SetFillStyle(0);
  // leglo->SetMargin(0.1);
  leglo->AddEntry ( ppInAuAuAJ_lo, "p+p HT #oplus Au+Au MB", "p");
  leglo->AddEntry ( AuAuAJ_lo,     "Au+Au HT", "p");
  leglo->Draw();


  // Line to guide the eye
  TLine line;
  line.SetLineStyle( 2 );
  line.SetLineColor( kGray+2 );
  gPad->Update();
  line.DrawLine( 0, gPad->GetFrame()->GetY1(), 0, gPad->GetFrame()->GetY2() );
  
  
  if ( ShowPvalue ){
    sprintf ( plabel, "p-value = %0.1g", KS_NominalE_Lo);
    latex.SetTextColor( AuAuAJ_lo->GetLineColor() );
    latex.SetTextSize(0.04);
    if ( nofabs ) latex.DrawLatex( .33,.18, plabel);
    else latex.DrawLatex( .6,.45, plabel);
  }

  if (TString(fAuAu->GetName()).Contains("HT54") ){
    gPad->SaveAs("plots/HT54_R0.4_Fig1.png");
    gPad->SaveAs("plots/HT54_R0.4_Fig1.pdf");
  } else   if (TString(fAuAu->GetName()).Contains("HT64") ){
    gPad->SaveAs("plots/HT64_R0.4_Fig1.png");
    gPad->SaveAs("plots/HT64_R0.4_Fig1.pdf");
  } else if (TString(fAuAu->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/R0.2_Fig.png");
    gPad->SaveAs("plots/R0.2_Fig.pdf");
  } else if (TString(fAuAu->GetName()).Contains("Pt1") ){
    gPad->SaveAs("plots/Pt1_Fig.png");
    gPad->SaveAs("plots/Pt1_Fig.pdf");
  } else {
    gPad->SaveAs("plots/R0.4_Fig1.png");
    gPad->SaveAs("plots/R0.4_Fig1.pdf");
    TString alttitle = gSystem->BaseName( fAuAu->GetName() );
    alttitle.ReplaceAll(".root",".png");
    gPad->SaveAs( "plots/" + alttitle );
        
  }

  // float KSqHi = AJ_hi_sysstat->KolmogorovTest(AuAuAJ_hi, "");
  // float C2qHi = AJ_hi_sysstat->Chi2Test(AuAuAJ_hi, "");
  // float KSHi = ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "");
  // float C2Hi = ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "");
  // cerr << "WITH quadrature errors:" << endl;
  // cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << KSqHi<< endl;
  // cerr << "chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << C2qHi<< endl;
  // cerr << "WITHOUT quadrature errors:" << endl;
  // cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << KSHi << endl;
  // cerr << "chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << C2Hi << endl;

  // float KSqLo = AJ_lo_sysstat->KolmogorovTest(AuAuAJ_lo, "");
  // float C2qLo = AJ_lo_sysstat->Chi2Test(AuAuAJ_lo, "");
  // float KSLo = ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "");
  // float C2Lo = ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "");
  // cerr << "WITH quadrature errors:" << endl;
  // cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut:        " << KSqLo << endl;
  // cerr << "chi^2 test for pp @ AuAu vs. AuAu, LOW cut:                " << C2qLo << endl;
  // cerr << "WITHOUT quadrature errors:" << endl;
  // cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut:        " << KSLo << endl;
  // cerr << "chi^2 test for pp @ AuAu vs. AuAu, LOW cut:                " << C2Lo << endl;


  // // Ratios
  // // ------

  // new TCanvas;
  // gPad->SetGridx(0);  gPad->SetGridy(0);

  // leg->Clear();
  
  // TH1D* ratio_hi = AuAuAJ_hi->Clone( "ratio_hi" );
  // ratio_hi->Divide( ppInAuAuAJ_hi );
  
  // TH1D* ratio_lo = AuAuAJ_lo->Clone( "ratio_lo" );
  // ratio_lo->Divide( ppInAuAuAJ_lo );

  // ratio_hi->SetAxisRange( -0.1,2.1,"y");        
  // ratio_hi->Draw("9");
  // ratio_lo->Draw("9same");
  
  // leg->AddEntry ( ratio_hi, "AuAu / pp, p_{T}^{Cut}>2 GeV/c", "p");
  // leg->AddEntry ( ratio_lo, "AuAu / pp, p_{T}^{Cut}>0.2 GeV/c", "p");
  // leg->Draw();
  
  // if (TString(fAuAu->GetName()).Contains("R0.2") ){
  //   gPad->SaveAs("plots/R0.2_Ratio.png");
  //   gPad->SaveAs("plots/R0.2_Ratio.pdf");
  // } else if (TString(fAuAu->GetName()).Contains("Pt1") ){
  //   gPad->SaveAs("plots/Pt1_Ratio.png");
  //   gPad->SaveAs("plots/Pt1_Ratio.pdf");
  // } else {
  //   gPad->SaveAs("plots/R0.4_Ratio.png");
  //   gPad->SaveAs("plots/R0.4_Ratio.pdf");        
  // }

  // p-value systematics
  Tpar=(TParameter<double>*) fSyst->Get("KS_PlusE_Hi");
  KS_PlusE_Hi = Tpar->GetVal();
  Tpar=(TParameter<double>*) fSyst->Get("KS_MinusE_Hi");
  KS_MinusE_Hi = Tpar->GetVal();
  Tpar=(TParameter<double>*) fSyst->Get("KS_PlusT_Hi");
  KS_PlusT_Hi = Tpar->GetVal();
  Tpar=(TParameter<double>*) fSyst->Get("KS_MinusT_Hi");
  KS_MinusT_Hi = Tpar->GetVal();

  if ( nofabs ){
    Tpar=(TParameter<double>*) fSyst->Get("KS_NoFabsPlusE_Lo");
    KS_PlusE_Lo = Tpar->GetVal();
    Tpar=(TParameter<double>*) fSyst->Get("KS_NoFabsMinusE_Lo");
    KS_MinusE_Lo = Tpar->GetVal();
    Tpar=(TParameter<double>*) fSyst->Get("KS_NoFabsPlusT_Lo");
    KS_PlusT_Lo = Tpar->GetVal();
    Tpar=(TParameter<double>*) fSyst->Get("KS_NoFabsMinusT_Lo");
    KS_MinusT_Lo = Tpar->GetVal();
  } else {
    Tpar=(TParameter<double>*) fSyst->Get("KS_PlusE_Lo");
    KS_PlusE_Lo = Tpar->GetVal();
    Tpar=(TParameter<double>*) fSyst->Get("KS_MinusE_Lo");
    KS_MinusE_Lo = Tpar->GetVal();
    Tpar=(TParameter<double>*) fSyst->Get("KS_PlusT_Lo");
    KS_PlusT_Lo = Tpar->GetVal();
    Tpar=(TParameter<double>*) fSyst->Get("KS_MinusT_Lo");
    KS_MinusT_Lo = Tpar->GetVal();
  }

  double min_E_Hi  = TMath::Min(KS_PlusE_Hi,KS_MinusE_Hi);
  double max_E_Hi  = TMath::Max(KS_PlusE_Hi,KS_MinusE_Hi);
  double min_T_Hi  = TMath::Min(KS_PlusT_Hi,KS_MinusT_Hi);
  double max_T_Hi  = TMath::Max(KS_PlusT_Hi,KS_MinusT_Hi);
  double min_E_Lo  = TMath::Min(KS_PlusE_Lo,KS_MinusE_Lo);
  double max_E_Lo  = TMath::Max(KS_PlusE_Lo,KS_MinusE_Lo);
  double min_T_Lo  = TMath::Min(KS_PlusT_Lo,KS_MinusT_Lo);
  double max_T_Lo  = TMath::Max(KS_PlusT_Lo,KS_MinusT_Lo);

  if ( min_E_Hi > KS_NominalE_Hi ) min_E_Hi = KS_NominalE_Hi;
  if ( max_E_Hi < KS_NominalE_Hi ) max_E_Hi = KS_NominalE_Hi;
  if ( min_E_Lo > KS_NominalE_Lo ) min_E_Lo = KS_NominalE_Lo;
  if ( max_E_Lo < KS_NominalE_Lo ) max_E_Lo = KS_NominalE_Lo;
  if ( min_T_Hi > KS_NominalE_Hi ) min_T_Hi = KS_NominalE_Hi;
  if ( max_T_Hi < KS_NominalE_Hi ) max_T_Hi = KS_NominalE_Hi;
  if ( min_T_Lo > KS_NominalE_Lo ) min_T_Lo = KS_NominalE_Lo;
  if ( max_T_Lo < KS_NominalE_Lo ) max_T_Lo = KS_NominalE_Lo;


  double min_Hi = TMath::Min(min_E_Hi,min_T_Hi);
  double max_Hi = TMath::Max(max_E_Hi,max_T_Hi);
  double min_Lo = TMath::Min(min_E_Lo,min_T_Lo);
  double max_Lo = TMath::Max(max_E_Lo,max_T_Lo);
  
  cout.precision(1);
  cout << scientific;
  
  cout << "p-value range, E, hi:  	"
       << min_E_Hi << "  -  " << KS_NominalE_Hi << "  -  " << max_E_Hi
       << endl;
  cout << "p-value range, T, hi:  	"
       << min_T_Hi << "  -  " << KS_NominalE_Hi << "  -  " << max_T_Hi
       << endl;


  if ( ! (TString(fAuAu->GetName()).Contains("R0.2")) ){
    cout.precision(2);
    cout << fixed;
  }
  cout << "p-value range, E, lo:  	"
       << min_E_Lo << "  -  " << KS_NominalE_Lo << "  -  " << max_E_Lo
       << endl;
  cout << "p-value range, T, lo:  	"
       << min_T_Lo << "  -  " << KS_NominalE_Lo << "  -  " << max_T_Lo
       << endl;

  cout.precision(1);
  cout << scientific;

  cout << endl;
  cout << "p-value range, total, hi:  	"
       << min_Hi << "  -  " << KS_NominalE_Hi << "  -  " << max_Hi
       << endl;
  if ( ! (TString(fAuAu->GetName()).Contains("R0.2")) ){
    cout.precision(2);
    cout << fixed;
  }
  cout << "p-value range, total, lo:  	"
       << min_Lo << "  -  " << KS_NominalE_Lo << "  -  " << max_Lo
       << endl;
  cout << endl;

  
  // For Lanny:
  cout.precision(4);
  cout << "<AJ> for pp @ AuAu, HIGH cut: " << ppInAuAuAJ_hi->GetMean() << " +/- " << ppInAuAuAJ_hi->GetMeanError() << endl;
  cout << "<AJ> for      AuAu, HIGH cut: " <<     AuAuAJ_hi->GetMean() << " +/- " <<     AuAuAJ_hi->GetMeanError() << endl;
  cout << "<AJ> for pp @ AuAu, LOW cut: " << ppInAuAuAJ_lo->GetMean() << " +/- " << ppInAuAuAJ_lo->GetMeanError() << endl;
  cout << "<AJ> for      AuAu, LOW cut: " <<     AuAuAJ_lo->GetMean() << " +/- " <<     AuAuAJ_lo->GetMeanError() << endl;
  
  
  // Done. Save
  // ==========
  out->Write();
  cout << "Wrote to " << out->GetName() << endl;
  
  return 0;

  // cout << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "D") << endl;
  // cout << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo,"D") << endl;
  
  // cout << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") << endl;
  // cout << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut : " << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") << endl;

  // cout << "chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") << endl;
  // cout << "chi^2 test for pp @ AuAu vs. AuAu, LOW cut : " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;
  
  // // cout << "Anderson-Darling test for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->AndersonDarlingTest(AuAuAJ_hi, "D") << endl;
  // // cout << "Anderson-Darling test for pp @ AuAu vs. AuAu, LOW cut : " << ppInAuAuAJ_lo->AndersonDarlingTest(AuAuAJ_lo, "D") << endl;

}

