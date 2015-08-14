#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void Fig1and3() {

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  // int RefmultCut = 351;  // 269 for 0-20%, 399 for 0-10%

  // This would give 40-100%:
  // int AuAuMultBinL = h2->GetYaxis()->FindBin( 0 ); 
  // int AuAuMultBinR = h2->GetYaxis()->FindBin( 113 ); 

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  // TFile *fAuAu         = TFile::Open("AjResults/HC30_Presel.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_MixTest_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/Systematics_MixTest_ppInAuAuAj.root");

  // // 100% hadronic correction
  // // TFile *fAuAu         = TFile::Open("AjResults/rndm1/HC30_Presel.root");
  // TFile *fAuAu         = TFile::Open("AjResults/HC30_Presel.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_HC30_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/Systematics_HC30_ppInAuAuAj.root");

  // // TFile *fAuAu         = TFile::Open("AjResults/rndm1/R0.2_HC30_Presel.root");
  // TFile *fAuAu         = TFile::Open("AjResults/R0.2_HC30_Presel.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_R0.2_HC30_ppInAuAuAj.root");
  // TFile *fSyst         = TFile::Open("AjResults/Systematics_R0.2_HC30_ppInAuAuAj.root");

  // MIP correction
  // TFile *fAuAu         = TFile::Open("AjResults/rndm1/Presel_AuAuAj.root");
  TFile *fAuAu         = TFile::Open("AjResults/Presel_AuAuAj.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_ppInAuAuAj.root");
  TFile *fSyst         = TFile::Open("AjResults/Systematics_ppInAuAuAj.root");

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

  if (!( TString(fAuAu->GetName()).Contains("HC30") ) ){
    RootOut.ReplaceAll( "Fig", "MIP_Fig");
  }

  TFile* out = new TFile( RootOut, "RECREATE");

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
  // TH1D* AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "SmallAJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  // TH1D* ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "SmallAJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  cout << AuAuAJ_lo ->Integral(1,100) << endl;

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
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");
    
    h->Rebin(2);
    // h->Scale(1./h->Integral());
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.23, "y");
    h->SetAxisRange(  0.0, 0.72, "x");
    
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

  AJ_hi_minmax->SetFillColor( kRed-10 );
  AJ_hi_minmax->SetMarkerColor(kRed-10);
  AJ_hi_minmax->SetMarkerSize(0);
  
  AJ_lo_minmax->SetFillColor( kGray );
  AJ_lo_minmax->SetMarkerColor( kGray );
  AJ_lo_minmax->SetMarkerSize(0);

  ppInAuAuAJ_hi->Draw("9");
  // if ( fSyst || true){
    AJ_hi_minmax->Draw("9E2same");
    //   }
  ppInAuAuAJ_hi->Draw("9same");
  AuAuAJ_hi->Draw("9same");

  // For p-value, add systematics in quadrature
  TH1D* AJ_hi_sysstat = ppInAuAuAJ_hi->Clone("AJ_hi_sysstat");
  if ( AJ_hi_minmax ){
    for (int i=1; i<AJ_hi_sysstat->GetNbinsX(); ++i ){
      if ( fabs (AJ_hi_sysstat->GetBinContent(i)-AJ_hi_minmax->GetBinContent(i))> 1e-4 ){
	cerr << "syst. and stat. histo are incompatible" << endl;
	return -1;
      }
      AJ_hi_sysstat->SetBinError( i, TMath::Sqrt(
						 pow( AJ_hi_sysstat->GetBinError(i), 2) +
						 pow( AJ_hi_minmax->GetBinError(i), 2) ));
    }
  }

  TH1D* AJ_lo_sysstat = ppInAuAuAJ_lo->Clone("AJ_lo_sysstat");
  if ( AJ_lo_minmax ){
    for (int i=1; i<AJ_lo_sysstat->GetNbinsX(); ++i ){
      if ( fabs (AJ_lo_sysstat->GetBinContent(i)-AJ_lo_minmax->GetBinContent(i))> 1e-3 ){
	cerr << "syst. and stat. histo are incompatible" << endl;
	new TCanvas;
	ppInAuAuAJ_lo->Draw();
	AJ_lo_minmax->Draw("same");
	return -1;
      }
      AJ_lo_sysstat->SetBinError( i, TMath::Sqrt(
						 pow( AJ_lo_sysstat->GetBinError(i), 2) +
						 pow( AJ_lo_minmax->GetBinError(i), 2) ));
    }
  }



  
  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( ppInAuAuAJ_hi, "pp HT #otimes AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( AuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  // 
  // TLegendEntry* l1 = leg->AddEntry ( (TH1D*) NULL,          "pp HT #otimes AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "");
  // l1->SetTextColor(kWhite); // Doesn't Work :(
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  leg->AddEntry ( (TH1D*) NULL,     "", "");
  
  leg->Draw();
  // return;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);
  char plabel[400];

  float threshhi = 1e-5;
  // // if ( ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") >1e-4  || ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") >1e-4 ){
  // if ( ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") > threshhi ){
  //   cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") << endl;
  //   cerr << "chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") << endl;
  //   return;    
  // }

  // sprintf ( plabel, "p-value < %g", threshhi);
  // sprintf ( plabel, "p-value = %0.2g", ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, ""));
  sprintf ( plabel, "p-value = %0.2g", ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, ""));
  latex.SetTextColor( AuAuAJ_hi->GetLineColor() );
  latex.DrawLatex( .6,.5, plabel);


  latex.SetTextColor(kBlack);
  latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");
  // if (TString(fAuAu->GetName()).Contains("R0.2") ){
  // latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>16 Gev/c");
  // latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>8 Gev/c");
  // } else {
  //   latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  //   latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");
  // }
  
  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  } else {
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  }  

  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/R0.2_Part1.png");
  } else if (TString(fAuAu->GetName()).Contains("Pt1") ){
    gPad->SaveAs("plots/Pt1_Part1.png");
  } else {
    gPad->SaveAs("plots/R0.4_Part1.png");
  }



  // Second part of animation
  // ------------------------
  //  if ( fSyst || true){
    AJ_lo_minmax->Draw("9E2same");
    //}
  ppInAuAuAJ_lo->Draw("9same");
  AuAuAJ_lo->Draw("9same");

  leg->Clear();
  leg->AddEntry ( ppInAuAuAJ_hi, "pp HT #otimes AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( AuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->Draw();

  float threshlo = 0.05;

  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    sprintf ( plabel, "p-value = %0.2g", ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") );
    latex.SetTextColor( AuAuAJ_lo->GetLineColor() );
    latex.DrawLatex( .6,.45, plabel);
    gPad->SaveAs("plots/R0.2_Fig.png");
    gPad->SaveAs("plots/R0.2_Fig.pdf");
  } else if (TString(fAuAu->GetName()).Contains("Pt1") ){
    gPad->SaveAs("plots/Pt1_Fig.png");
    gPad->SaveAs("plots/Pt1_Fig.pdf");
  } else {
    // if ( ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") < threshlo ){
    //   cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut: " << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") << endl;
    //   cerr << "chi^2 test for pp @ AuAu vs. AuAu, LOW cut: " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;
    //   return;    
    // }    
    // sprintf ( plabel, "p-value >> %g", threshlo);
    sprintf ( plabel, "p-value = %0.2g", ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, ""));
    latex.SetTextColor( AuAuAJ_lo->GetLineColor() );
    latex.DrawLatex( .6,.45, plabel);
    gPad->SaveAs("plots/R0.4_Fig1.png");
    gPad->SaveAs("plots/R0.4_Fig1.pdf");
  }

  cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << AJ_hi_sysstat->KolmogorovTest(AuAuAJ_hi, "") << endl;
  cerr << "chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << AJ_hi_sysstat->Chi2Test(AuAuAJ_hi, "") << endl;
  cerr << "OLD: Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") << endl;
  cerr << "OLD: chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") << endl;

  cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut: " << AJ_lo_sysstat->KolmogorovTest(AuAuAJ_lo, "") << endl;
  cerr << "chi^2 test for pp @ AuAu vs. AuAu, LOW cut: " << AJ_lo_sysstat->Chi2Test(AuAuAJ_lo, "") << endl;
  cerr << "OLD: Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut: " << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") << endl;
  cerr << "OLD: chi^2 test for pp @ AuAu vs. AuAu, LOW cut: " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;


  out->Write();
  cout << "Wrote to " << out->GetName() << endl;

  return;
  
  // cout << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "D") << endl;
  // cout << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo,"D") << endl;
  
  // cout << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") << endl;
  // cout << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut : " << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") << endl;

  // cout << "chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") << endl;
  // cout << "chi^2 test for pp @ AuAu vs. AuAu, LOW cut : " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;
  
  // // cout << "Anderson-Darling test for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->AndersonDarlingTest(AuAuAJ_hi, "D") << endl;
  // // cout << "Anderson-Darling test for pp @ AuAu vs. AuAu, LOW cut : " << ppInAuAuAJ_lo->AndersonDarlingTest(AuAuAJ_lo, "D") << endl;

}

