#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void VarComboFollowPlot() {
  gStyle->SetOptStat(0);
  TCanvas* cLo = new TCanvas("cLo");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TCanvas* cHi = new TCanvas("cHi");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TLegend* legLo = new TLegend( 0.55, 0.55, 0.88, 0.88, "  p_{T}^{C}>0.2 GeV/c" );
  TLegend* legHi = new TLegend( 0.55, 0.55, 0.88, 0.88, "  p_{T}^{C}>2 GeV/c" );
  legLo->SetMargin(0.2);
  legHi->SetMargin(0.2);

  TFile *fppLarge   = TFile::Open("AjResults/ppAj.root");
  TFile *fppFollow  = TFile::Open("AjResults/ppFollowAj.root");
  TFile *fAuAuLarge   = TFile::Open("AjResults/VarAuAuAj.root");
  TFile *fAuAuFollow  = TFile::Open("AjResults/VarFollowAuAuAj.root");
  TFile *fPpInAuAuLarge   = TFile::Open("AjResults/PpInAuAuAj.root");
  TFile *fPpInAuAuFollow  = TFile::Open("AjResults/PpInAuAuFollowAj.root");
  
  // --------------------------------------------------
  // == pp ==
  TH1D* origPpHi=(TH1D*)  fppLarge->Get( "UnmatchedAJ_hi");
  TH1D* SmallPpHi=(TH1D*) fppFollow->Get( "SmallUnmatchedAJ_hi");
  TH1D* LargePpHi=(TH1D*) fppFollow->Get( "LargeAJ_hi");
  origPpHi->Rebin(2);
  origPpHi->SetLineColor( kGray+2 );  origPpHi->SetLineWidth( 2 );  origPpHi->SetTitle( ";|A_{J}|;Fraction" );  origPpHi->Scale(1./origPpHi->Integral());
  SmallPpHi->Rebin(2);
  SmallPpHi->SetLineColor( kRed );  SmallPpHi->SetLineWidth( 2 );  SmallPpHi->SetTitle( ";|A_{J}|;Fraction" );  SmallPpHi->Scale(1./SmallPpHi->Integral());
  LargePpHi->Rebin(2);
  LargePpHi->SetLineColor( kGreen+1 );  LargePpHi->SetLineWidth( 2 );  LargePpHi->SetTitle( ";|A_{J}|;Fraction" );  LargePpHi->Scale(1./LargePpHi->Integral());

  origPpHi->SetAxisRange( -0.02, 0.22, "y");
  origPpHi->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  // == AuAu ==
  TH1D* origAuAuHi=(TH1D*)  fAuAuLarge->Get( "UnmatchedAJ_hi");
  TH1D* SmallAuAuHi=(TH1D*) fAuAuFollow->Get( "SmallUnmatchedAJ_hi");
  TH1D* LargeAuAuHi=(TH1D*) fAuAuFollow->Get( "LargeAJ_hi");
  origAuAuHi->Rebin(2);
  origAuAuHi->SetLineColor( kRed );    origAuAuHi->SetLineWidth( 1 );  origAuAuHi->SetTitle( ";|A_{J}|;Fraction" );  origAuAuHi->Scale(1./origAuAuHi->Integral());
  origAuAuHi->SetMarkerColor( kRed );  origAuAuHi->SetMarkerStyle( 20 );
  SmallAuAuHi->Rebin(2);
  SmallAuAuHi->SetLineColor( kMagenta+1 );    SmallAuAuHi->SetLineWidth( 1 );  SmallAuAuHi->SetTitle( ";|A_{J}|;Fraction" );  SmallAuAuHi->Scale(1./SmallAuAuHi->Integral());
  SmallAuAuHi->SetMarkerColor( kMagenta+1 );  SmallAuAuHi->SetMarkerStyle( 20 );
  LargeAuAuHi->Rebin(2);
  LargeAuAuHi->SetLineColor( kBlue );    LargeAuAuHi->SetLineWidth( 1 );  LargeAuAuHi->SetTitle( ";|A_{J}|;Fraction" );  LargeAuAuHi->Scale(1./LargeAuAuHi->Integral());
  LargeAuAuHi->SetMarkerColor( kBlue );  LargeAuAuHi->SetMarkerStyle( 20 );
  origAuAuHi->SetAxisRange( -0.02, 0.22, "y");
  origAuAuHi->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  // == PpInAuAu ==
  TH1D* origPpInAuAuHi=(TH1D*)  fPpInAuAuLarge->Get( "UnmatchedAJ_hi");
  TH1D* SmallPpInAuAuHi=(TH1D*) fPpInAuAuFollow->Get( "SmallUnmatchedAJ_hi");
  TH1D* LargePpInAuAuHi=(TH1D*) fPpInAuAuFollow->Get( "LargeAJ_hi");
  origPpInAuAuHi->Rebin(2);
  origPpInAuAuHi->SetLineColor( kRed );    origPpInAuAuHi->SetLineWidth( 1 );  origPpInAuAuHi->SetTitle( ";|A_{J}|;Fraction" );  origPpInAuAuHi->Scale(1./origPpInAuAuHi->Integral());
  origPpInAuAuHi->SetMarkerColor( kRed );  origPpInAuAuHi->SetMarkerStyle( 24 );
  SmallPpInAuAuHi->Rebin(2);
  SmallPpInAuAuHi->SetLineColor( kMagenta+1 );    SmallPpInAuAuHi->SetLineWidth( 1 );  SmallPpInAuAuHi->SetTitle( ";|A_{J}|;Fraction" );  SmallPpInAuAuHi->Scale(1./SmallPpInAuAuHi->Integral());
  SmallPpInAuAuHi->SetMarkerColor( kMagenta+1 );  SmallPpInAuAuHi->SetMarkerStyle( 24 );
  LargePpInAuAuHi->Rebin(2);
  LargePpInAuAuHi->SetLineColor( kBlue );    LargePpInAuAuHi->SetLineWidth( 1 );  LargePpInAuAuHi->SetTitle( ";|A_{J}|;Fraction" );  LargePpInAuAuHi->Scale(1./LargePpInAuAuHi->Integral());
  LargePpInAuAuHi->SetMarkerColor( kBlue );  LargePpInAuAuHi->SetMarkerStyle( 24 );
  origPpInAuAuHi->SetAxisRange( -0.02, 0.22, "y");
  origPpInAuAuHi->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  // == pp ==
  TH1D* origPpLo=(TH1D*)  fppLarge->Get( "AJ_lo");
  TH1D* SmallPpLo=(TH1D*) fppFollow->Get( "SmallAJ_lo");
  TH1D* LargePpLo=(TH1D*) fppFollow->Get( "LargeAJ_lo");
  origPpLo->Rebin(2);
  origPpLo->SetLineColor( kGray+2 );  origPpLo->SetLineWidth( 2 );  origPpLo->SetTitle( ";|A_{J}|;Fraction" );  origPpLo->Scale(1./origPpLo->Integral());
  SmallPpLo->Rebin(2);
  SmallPpLo->SetLineColor( kRed );  SmallPpLo->SetLineWidth( 2 );  SmallPpLo->SetTitle( ";|A_{J}|;Fraction" );  SmallPpLo->Scale(1./SmallPpLo->Integral());
  LargePpLo->Rebin(2);
  LargePpLo->SetLineColor( kGreen+1 );  LargePpLo->SetLineWidth( 2 );  LargePpLo->SetTitle( ";|A_{J}|;Fraction" );  LargePpLo->Scale(1./LargePpLo->Integral());
  origPpLo->SetAxisRange( -0.02, 0.22, "y");
  origPpLo->SetAxisRange( -0.1, 0.75, "x");


  // --------------------------------------------------
  // == AuAu ==
  TH1D* origAuAuLo=(TH1D*)  fAuAuLarge->Get( "AJ_lo");
  TH1D* SmallAuAuLo=(TH1D*) fAuAuFollow->Get( "SmallAJ_lo");
  TH1D* LargeAuAuLo=(TH1D*) fAuAuFollow->Get( "LargeAJ_lo");
  origAuAuLo->Rebin(2);
  origAuAuLo->SetLineColor( kBlack );    origAuAuLo->SetLineWidth( 1 );  origAuAuLo->SetTitle( ";|A_{J}|;Fraction" );  origAuAuLo->Scale(1./origAuAuLo->Integral());
  origAuAuLo->SetMarkerColor( kBlack );  origAuAuLo->SetMarkerStyle( 20 );
  SmallAuAuLo->Rebin(2);
  SmallAuAuLo->SetLineColor( kGray+1 );    SmallAuAuLo->SetLineWidth( 1 );  SmallAuAuLo->SetTitle( ";|A_{J}|;Fraction" );  SmallAuAuLo->Scale(1./SmallAuAuLo->Integral());
  SmallAuAuLo->SetMarkerColor( kGray+1 );  SmallAuAuLo->SetMarkerStyle( 20 );
  LargeAuAuLo->Rebin(2);
  LargeAuAuLo->SetLineColor( kGreen+1 );    LargeAuAuLo->SetLineWidth( 1 );  LargeAuAuLo->SetTitle( ";|A_{J}|;Fraction" );  LargeAuAuLo->Scale(1./LargeAuAuLo->Integral());
  LargeAuAuLo->SetMarkerColor( kGreen+1 );  LargeAuAuLo->SetMarkerStyle( 20 );
  origAuAuLo->SetAxisRange( -0.02, 0.22, "y");
  origAuAuLo->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  // == PpInAuAu ==
  TH1D* origPpInAuAuLo=(TH1D*)  fPpInAuAuLarge->Get( "AJ_lo");
  TH1D* SmallPpInAuAuLo=(TH1D*) fPpInAuAuFollow->Get( "SmallAJ_lo");
  TH1D* LargePpInAuAuLo=(TH1D*) fPpInAuAuFollow->Get( "LargeAJ_lo");
  origPpInAuAuLo->Rebin(2);
  origPpInAuAuLo->SetLineColor( kBlack );    origPpInAuAuLo->SetLineWidth( 1 );  origPpInAuAuLo->SetTitle( ";|A_{J}|;Fraction" );  origPpInAuAuLo->Scale(1./origPpInAuAuLo->Integral());
  origPpInAuAuLo->SetMarkerColor( kBlack );  origPpInAuAuLo->SetMarkerStyle( 24 );
  SmallPpInAuAuLo->Rebin(2);
  SmallPpInAuAuLo->SetLineColor( kGray+1 );    SmallPpInAuAuLo->SetLineWidth( 1 );  SmallPpInAuAuLo->SetTitle( ";|A_{J}|;Fraction" );  SmallPpInAuAuLo->Scale(1./SmallPpInAuAuLo->Integral());
  SmallPpInAuAuLo->SetMarkerColor( kGray+1 );  SmallPpInAuAuLo->SetMarkerStyle( 24 );
  LargePpInAuAuLo->Rebin(2);
  LargePpInAuAuLo->SetLineColor( kGreen+1 );    LargePpInAuAuLo->SetLineWidth( 1 );  LargePpInAuAuLo->SetTitle( ";|A_{J}|;Fraction" );  LargePpInAuAuLo->Scale(1./LargePpInAuAuLo->Integral());
  LargePpInAuAuLo->SetMarkerColor( kGreen+1 );  LargePpInAuAuLo->SetMarkerStyle( 24 );
  origPpInAuAuLo->SetAxisRange( -0.02, 0.22, "y");
  origPpInAuAuLo->SetAxisRange( -0.1, 0.75, "x");






  // --------------------------------------------------
  // -------- Draw ------------------------------------
  // --------------------------------------------------
  TLegend* leg;
  // Compare 0.2 to 0.4
  // ------------------
  // PpInAuAu
  cHi->cd();
  leg = new TLegend( 0.55, 0.71, 0.88, 0.88);
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.2);

  origPpInAuAuHi->Draw("9");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>2 GeV/c");
  leg->AddEntry( origPpInAuAuHi, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->Draw();
  gPad->SaveAs("plots/FollowPpHi_0.png");

  SmallPpInAuAuHi->Draw("9same");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>2 GeV/c");
  leg->AddEntry( origPpInAuAuHi, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( SmallPpInAuAuHi, "pp #otimes Au+Au, R=0.2","lp");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->Draw();
  gPad->SaveAs("plots/FollowPpHi_1.png");

  LargePpInAuAuHi->Draw("9same");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>2 GeV/c");
  leg->AddEntry( origPpInAuAuHi, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( SmallPpInAuAuHi, "pp #otimes Au+Au, R=0.2","lp");
  leg->AddEntry( LargePpInAuAuHi, "pp #otimes Au+Au, R=0.4 #cbar R=0.2","lp");
  leg->Draw();
  gPad->SaveAs("plots/FollowPpHi.png");
  
  cLo->cd();
  leg = new TLegend( 0.55, 0.71, 0.88, 0.88, "  p_{T}^{C}>0.2 GeV/c, matched" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.2);

  origPpInAuAuLo->Draw("9");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>0.2 GeV/c");
  leg->AddEntry( origPpInAuAuLo, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->Draw();
  gPad->SaveAs("plots/FollowPpLo_0.png");
  

  SmallPpInAuAuLo->Draw("9same");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>0.2 GeV/c");
  leg->AddEntry( origPpInAuAuLo, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( SmallPpInAuAuLo, "pp #otimes Au+Au, R=0.2","lp");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->Draw();
  gPad->SaveAs("plots/FollowPpLo_1.png");


  LargePpInAuAuLo->Draw("9same");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>0.2 GeV/c");
  leg->AddEntry( origPpInAuAuLo, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( SmallPpInAuAuLo, "pp #otimes Au+Au, R=0.2","lp");
  leg->AddEntry( LargePpInAuAuLo, "pp #otimes Au+Au, R=0.4 #cbar R=0.2","lp");
  leg->Draw();
  gPad->SaveAs("plots/FollowPpLo.png");
  

  // AuAu
  cHi->cd();
  leg = new TLegend( 0.55, 0.71, 0.88, 0.88, "  p_{T}^{C}>2 GeV/c" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.2);
  
  origAuAuHi->Draw("9");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>2 GeV/c");
  leg->AddEntry( origAuAuHi, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->Draw();
  gPad->SaveAs("plots/FollowAuAuHi_0.png");

  SmallAuAuHi->Draw("9same");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>2 GeV/c");
  leg->AddEntry( origAuAuHi, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( SmallAuAuHi, "pp #otimes Au+Au, R=0.2","lp");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->Draw();
  gPad->SaveAs("plots/FollowAuAuHi_1.png");

  LargeAuAuHi->Draw("9same");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>2 GeV/c");
  leg->AddEntry( origAuAuHi, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( SmallAuAuHi, "pp #otimes Au+Au, R=0.2","lp");
  leg->AddEntry( LargeAuAuHi, "pp #otimes Au+Au, R=0.4 #cbar R=0.2","lp");
  leg->Draw();
  gPad->SaveAs("plots/FollowAuAuHi.png");





  cLo->cd();
  leg = new TLegend( 0.55, 0.71, 0.88, 0.88, "  p_{T}^{C}>0.2 GeV/c, matched" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.2);


  origAuAuLo->Draw("9");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>0.2 GeV/c");
  leg->AddEntry( origAuAuLo, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->Draw();
  gPad->SaveAs("plots/FollowAuAuLo_0.png");
  

  SmallAuAuLo->Draw("9same");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>0.2 GeV/c");
  leg->AddEntry( origAuAuLo, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( SmallAuAuLo, "pp #otimes Au+Au, R=0.2","lp");
  leg->AddEntry( (TH1D*) NULL, "","");
  leg->Draw();
  gPad->SaveAs("plots/FollowAuAuLo_1.png");


  LargeAuAuLo->Draw("9same");
  leg->Clear();  leg->SetHeader("  p_{T}^{C}>0.2 GeV/c");
  leg->AddEntry( origAuAuLo, "pp #otimes Au+Au, R=0.4","lp");
  leg->AddEntry( SmallAuAuLo, "pp #otimes Au+Au, R=0.2","lp");
  leg->AddEntry( LargeAuAuLo, "pp #otimes Au+Au, R=0.4 #cbar R=0.2","lp");
  leg->Draw();
  gPad->SaveAs("plots/FollowAuAuLo.png");
  return;
















  return;
  cHi->cd();
  // origPpHi->Draw();
  // SmallPpHi->Draw("same");
  // LargePpHi->Draw("same");

  origPpInAuAuHi->Draw();
  SmallPpInAuAuHi->Draw("same");
  LargePpInAuAuHi->Draw("same");
  origAuAuHi->Draw("same");
  SmallAuAuHi->Draw("same");
  LargeAuAuHi->Draw("same");


  // legHi->AddEntry( origPpHi, "pp, R=0.4","lp");
  // legHi->AddEntry( SmallPpHi, "pp, R=0.2","lp");
  // legHi->AddEntry( LargePpHi, "pp, R=0.4, matched to 0.2","lp");
  legHi->AddEntry( origPpInAuAuHi, "pp #otimes Au+Au, R=0.4","lp");
  legHi->AddEntry( SmallPpInAuAuHi, "pp #otimes Au+Au, R=0.2","lp");
  legHi->AddEntry( LargePpInAuAuHi, "pp #otimes Au+Au, R=0.4 #cbar R=0.2","lp");
  legHi->AddEntry( origAuAuHi, "Au+Au, R=0.4","lp");
  legHi->AddEntry( SmallAuAuHi, "Au+Au, R=0.2","lp");
  legHi->AddEntry( LargeAuAuHi, "Au+Au, R=0.4 #cbar R=0.2","lp");
  legHi->Draw();
  gPad->SaveAs("plots/ppFollowHi.png");

  // --------------------------------------------------
  cLo->cd();
  // origPpLo->Draw();
  // SmallPpLo->Draw("same");
  // LargePpLo->Draw("same");

  origPpInAuAuLo->Draw();
  SmallPpInAuAuLo->Draw("same");
  LargePpInAuAuLo->Draw("same");
  origAuAuLo->Draw("same");
  SmallAuAuLo->Draw("same");
  LargeAuAuLo->Draw("same");

  // legLo->AddEntry( origPpLo, "pp, R=0.4","lp");
  // legLo->AddEntry( SmallPpLo, "pp, R=0.2","lp");
  // legLo->AddEntry( LargePpLo, "pp, R=0.4, matched to 0.2","lp");
  legLo->AddEntry( origPpInAuAuLo, "pp #otimes Au+Au, R=0.4","lp");
  legLo->AddEntry( SmallPpInAuAuLo, "pp #otimes Au+Au, R=0.2","lp");
  legLo->AddEntry( LargePpInAuAuLo, "pp #otimes Au+Au, R=0.4 #cbar R=0.2","lp");
  legLo->AddEntry( origAuAuLo, "Au+Au, R=0.4","lp");
  legLo->AddEntry( SmallAuAuLo, "Au+Au, R=0.2","lp");
  legLo->AddEntry( LargeAuAuLo, "Au+Au, R=0.4 #cbar R=0.2","lp");

  legLo->Draw();

  gPad->SaveAs("plots/ppFollowLo.png");
  return 0;

  // --------------------------------------------------
  // --------------------------------------------------
  // --------------------------------------------------
  TCanvas* cdPtLo = new TCanvas("cdPtLo");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TCanvas* cdPtHi = new TCanvas("cdPtHi");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TLegend* legdPtLo = new TLegend( 0.55, 0.7, 0.88, 0.88, "  p_{T}^{C}>0.2 GeV/c" );
  TLegend* legdPtHi = new TLegend( 0.55, 0.7, 0.88, 0.88, "  p_{T}^{C}>2 GeV/c" );

  
  TH1D* origdPtPpHi=(TH1D*)  fppLarge->Get( "UnmatchedhdPtHi");
  TH1D* SmalldPtPpHi=(TH1D*) fppFollow->Get( "SmallUnmatchedhdPtHi");
  // TH1D* LargedPtPpHi=(TH1D*) fppFollow->Get( "LargeUnmatchedhdPtHi");
  origdPtPpHi->Rebin(4);
  origdPtPpHi->SetLineColor( kGray+2 );  origdPtPpHi->SetLineWidth( 2 );  origdPtPpHi->SetTitle( ";#Delta p_{T};fraction" );  origdPtPpHi->Scale(1./origdPtPpHi->Integral());
  SmalldPtPpHi->Rebin(4);
  SmalldPtPpHi->SetLineColor( kRed );  SmalldPtPpHi->SetLineWidth( 2 );  SmalldPtPpHi->SetTitle( ";#Delta p_{T};fraction" );  SmalldPtPpHi->Scale(1./SmalldPtPpHi->Integral());
  // LargedPtPpHi->Rebin(4);
  // LargedPtPpHi->SetLineColor( kGray+1 );  LargedPtPpHi->SetLineWidth( 2 );  LargedPtPpHi->SetTitle( ";#Delta p_{T};fraction" );  LargedPtPpHi->Scale(1./LargedPtPpHi->Integral());
  
  origdPtPpHi->SetAxisRange( -0.02, 0.2, "y");
  // origdPtPpHi->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  TH1D* origdPtPpLo=(TH1D*)  fppLarge->Get( "hdPtLo");
  TH1D* SmalldPtPpLo=(TH1D*) fppFollow->Get( "SmallhdPtLo");
  TH1D* LargedPtPpLo=(TH1D*) fppFollow->Get( "LargehdPtLo");
  origdPtPpLo->Rebin(4);
  origdPtPpLo->SetLineColor( kGray+2 );  origdPtPpLo->SetLineWidth( 2 );  origdPtPpLo->SetTitle( ";#Delta p_{T};fraction" );  origdPtPpLo->Scale(1./origdPtPpLo->Integral());
  SmalldPtPpLo->Rebin(4);
  SmalldPtPpLo->SetLineColor( kRed );  SmalldPtPpLo->SetLineWidth( 2 );  SmalldPtPpLo->SetTitle( ";#Delta p_{T};fraction" );  SmalldPtPpLo->Scale(1./SmalldPtPpLo->Integral());
  LargedPtPpLo->Rebin(4);
  LargedPtPpLo->SetLineColor( kGray+1 );  LargedPtPpLo->SetLineWidth( 2 );  LargedPtPpLo->SetTitle( ";#Delta p_{T};fraction" );  LargedPtPpLo->Scale(1./LargedPtPpLo->Integral());
  
  origdPtPpLo->SetAxisRange( -0.02, 0.2, "y");
  // origdPtPpLo->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  cdPtHi->cd();
  origdPtPpHi->Draw();
  SmalldPtPpHi->Draw("same");
  LargedPtPpHi->Draw("same");
  
  legdPtHi->AddEntry( origdPtPpHi, "pp, R=0.4","lp");
  legdPtHi->AddEntry( SmalldPtPpHi, "pp, R=0.2","lp");
  legdPtHi->AddEntry( LargedPtPpHi, "pp, R=0.4, matched to 0.2","lp");
  legdPtHi->Draw();
  gPad->SaveAs("plots/ppFollowdPtHi.png");

  // --------------------------------------------------
  cdPtLo->cd();
  origdPtPpLo->Draw();
  SmalldPtPpLo->Draw("same");
  LargedPtPpLo->Draw("same");

  legdPtLo->AddEntry( origdPtPpLo, "pp, R=0.4","lp");
  legdPtLo->AddEntry( SmalldPtPpLo, "pp, R=0.2","lp");
  legdPtLo->AddEntry( LargedPtPpLo, "pp, R=0.4, matched to 0.2","lp");
  legdPtLo->Draw();

  gPad->SaveAs("plots/ppFollowdPtLo.png");


  
}

