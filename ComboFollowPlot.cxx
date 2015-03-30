#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void ComboFollowPlot() {
  gStyle->SetOptStat(0);
  TCanvas* cLo = new TCanvas("cLo");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TCanvas* cHi = new TCanvas("cHi");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TLegend* legLo = new TLegend( 0.55, 0.7, 0.88, 0.88, "  p_{T}^{C}>0.2 GeV/c" );
  TLegend* legHi = new TLegend( 0.55, 0.7, 0.88, 0.88, "  p_{T}^{C}>2 GeV/c" );

  TFile *fppLarge   = TFile::Open("AjResults/ppAj.root");
  TFile *fppFollow  = TFile::Open("AjResults/ppFollowAj.root");
  TFile *fAuAuLarge   = TFile::Open("AjResults/AuAuAj.root");
  TFile *fAuAuFollow  = TFile::Open("AjResults/FollowAuAuAj.root");
  //TFile *fPpInAuAuLarge   = TFile::Open("AjResults/PpInAuAuAj.root");
  TFile *fPpInAuAuLarge   = TFile::Open("AjResults/AuAuAj.root");
  TFile *fPpInAuAuFollow  = TFile::Open("AjResults/PpInAuAuFollowAj.root");
  
  // --------------------------------------------------
  // == pp ==
  TH1D* origPpHi=(TH1D*)  fppLarge->Get( "UnmatchedAJ_hi");
  TH1D* SmallPpHi=(TH1D*) fppFollow->Get( "SmallUnmatchedAJ_hi");
  TH1D* LargePpHi=(TH1D*) fppFollow->Get( "LargeUnmatchedAJ_hi");
  origPpHi->Rebin(2);
  origPpHi->SetLineColor( kGray+2 );  origPpHi->SetLineWidth( 1 );  origPpHi->SetTitle( ";A_{J};fraction" );  origPpHi->Scale(1./origPpHi->Integral());
  SmallPpHi->Rebin(2);
  SmallPpHi->SetLineColor( kRed );  SmallPpHi->SetLineWidth( 1 );  SmallPpHi->SetTitle( ";A_{J};fraction" );  SmallPpHi->Scale(1./SmallPpHi->Integral());
  LargePpHi->Rebin(2);
  LargePpHi->SetLineColor( kGreen+1 );  LargePpHi->SetLineWidth( 1 );  LargePpHi->SetTitle( ";A_{J};fraction" );  LargePpHi->Scale(1./LargePpHi->Integral());

  origPpHi->SetAxisRange( -0.02, 0.3, "y");
  origPpHi->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  // == AuAu ==
  TH1D* origAuAuHi=(TH1D*)  fAuAuLarge->Get( "UnmatchedAJ_hi");
  TH1D* SmallAuAuHi=(TH1D*) fAuAuFollow->Get( "SmallUnmatchedAJ_hi");
  TH1D* LargeAuAuHi=(TH1D*) fAuAuFollow->Get( "LargeUnmatchedAJ_hi");
  origAuAuHi->Rebin(2);
  origAuAuHi->SetLineColor( kGray+2 );  origAuAuHi->SetLineWidth( 2 );  origAuAuHi->SetTitle( ";A_{J};fraction" );  origAuAuHi->Scale(1./origAuAuHi->Integral());
  SmallAuAuHi->Rebin(2);
  SmallAuAuHi->SetLineColor( kRed );  SmallAuAuHi->SetLineWidth( 2 );  SmallAuAuHi->SetTitle( ";A_{J};fraction" );  SmallAuAuHi->Scale(1./SmallAuAuHi->Integral());
  LargeAuAuHi->Rebin(2);
  LargeAuAuHi->SetLineColor( kGreen+1 );  LargeAuAuHi->SetLineWidth( 2 );  LargeAuAuHi->SetTitle( ";A_{J};fraction" );  LargeAuAuHi->Scale(1./LargeAuAuHi->Integral());

  // --------------------------------------------------
  // == PpInAuAu ==
  TH1D* origPpInAuAuHi=(TH1D*)  fPpInAuAuLarge->Get( "UnmatchedAJ_hi");
  TH1D* SmallPpInAuAuHi=(TH1D*) fPpInAuAuFollow->Get( "SmallUnmatchedAJ_hi");
  TH1D* LargePpInAuAuHi=(TH1D*) fPpInAuAuFollow->Get( "LargeUnmatchedAJ_hi");
  origPpInAuAuHi->Rebin(2);
  origPpInAuAuHi->SetLineColor( kGray+2 );  origPpInAuAuHi->SetLineWidth( 2 );  origPpInAuAuHi->SetTitle( ";A_{J};fraction" );  origPpInAuAuHi->Scale(1./origPpInAuAuHi->Integral());
  SmallPpInAuAuHi->Rebin(2);
  SmallPpInAuAuHi->SetLineColor( kRed );  SmallPpInAuAuHi->SetLineWidth( 2 );  SmallPpInAuAuHi->SetTitle( ";A_{J};fraction" );  SmallPpInAuAuHi->Scale(1./SmallPpInAuAuHi->Integral());
  LargePpInAuAuHi->Rebin(2);
  LargePpInAuAuHi->SetLineColor( kGreen+1 );  LargePpInAuAuHi->SetLineWidth( 2 );  LargePpInAuAuHi->SetTitle( ";A_{J};fraction" );  LargePpInAuAuHi->Scale(1./LargePpInAuAuHi->Integral());

  // --------------------------------------------------
  TH1D* origPpLo=(TH1D*)  fppLarge->Get( "AJ_lo");
  TH1D* SmallPpLo=(TH1D*) fppFollow->Get( "SmallAJ_lo");
  TH1D* LargePpLo=(TH1D*) fppFollow->Get( "LargeAJ_lo");
  origPpLo->Rebin(2);
  origPpLo->SetLineColor( kGray+2 );  origPpLo->SetLineWidth( 2 );  origPpLo->SetTitle( ";A_{J};fraction" );  origPpLo->Scale(1./origPpLo->Integral());
  SmallPpLo->Rebin(2);
  SmallPpLo->SetLineColor( kRed );  SmallPpLo->SetLineWidth( 2 );  SmallPpLo->SetTitle( ";A_{J};fraction" );  SmallPpLo->Scale(1./SmallPpLo->Integral());
  LargePpLo->Rebin(2);
  LargePpLo->SetLineColor( kGreen+1 );  LargePpLo->SetLineWidth( 2 );  LargePpLo->SetTitle( ";A_{J};fraction" );  LargePpLo->Scale(1./LargePpLo->Integral());
  
  origPpLo->SetAxisRange( -0.02, 0.3, "y");
  origPpLo->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  cHi->cd();
  origPpHi->Draw();
  SmallPpHi->Draw("same");
  LargePpHi->Draw("same");

  origAuAuHi->Draw("same");
  SmallAuAuHi->Draw("same");
  LargeAuAuHi->Draw("same");

  origPpInAuAuHi->Draw("same");
  SmallPpInAuAuHi->Draw("same");
  LargePpInAuAuHi->Draw("same");

  legHi->AddEntry( origPpHi->GetName(), "pp, R=0.4","l");
  legHi->AddEntry( SmallPpHi->GetName(), "pp, R=0.2","l");
  legHi->AddEntry( LargePpHi->GetName(), "pp, R=0.4, matched to 0.2","l");
  legHi->Draw();
  gPad->SaveAs("plots/ppFollowHi.png");

  // --------------------------------------------------
  cLo->cd();
  origPpLo->Draw();
  SmallPpLo->Draw("same");
  LargePpLo->Draw("same");

  origAuAuHi->Draw("same");
  SmallAuAuHi->Draw("same");
  LargeAuAuHi->Draw("same");

  origPpInAuAuHi->Draw("same");
  SmallPpInAuAuHi->Draw("same");
  LargePpInAuAuHi->Draw("same");

  legLo->AddEntry( origPpLo->GetName(), "pp, R=0.4","l");
  legLo->AddEntry( SmallPpLo->GetName(), "pp, R=0.2","l");
  legLo->AddEntry( LargePpLo->GetName(), "pp, R=0.4, matched to 0.2","l");
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
  TH1D* LargedPtPpHi=(TH1D*) fppFollow->Get( "LargeUnmatchedhdPtHi");
  origdPtPpHi->Rebin(4);
  origdPtPpHi->SetLineColor( kGray+2 );  origdPtPpHi->SetLineWidth( 2 );  origdPtPpHi->SetTitle( ";#Delta p_{T};fraction" );  origdPtPpHi->Scale(1./origdPtPpHi->Integral());
  SmalldPtPpHi->Rebin(4);
  SmalldPtPpHi->SetLineColor( kRed );  SmalldPtPpHi->SetLineWidth( 2 );  SmalldPtPpHi->SetTitle( ";#Delta p_{T};fraction" );  SmalldPtPpHi->Scale(1./SmalldPtPpHi->Integral());
  LargedPtPpHi->Rebin(4);
  LargedPtPpHi->SetLineColor( kGreen+1 );  LargedPtPpHi->SetLineWidth( 2 );  LargedPtPpHi->SetTitle( ";#Delta p_{T};fraction" );  LargedPtPpHi->Scale(1./LargedPtPpHi->Integral());
  
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
  LargedPtPpLo->SetLineColor( kGreen+1 );  LargedPtPpLo->SetLineWidth( 2 );  LargedPtPpLo->SetTitle( ";#Delta p_{T};fraction" );  LargedPtPpLo->Scale(1./LargedPtPpLo->Integral());
  
  origdPtPpLo->SetAxisRange( -0.02, 0.2, "y");
  // origdPtPpLo->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  cdPtHi->cd();
  origdPtPpHi->Draw();
  SmalldPtPpHi->Draw("same");
  LargedPtPpHi->Draw("same");
  
  legdPtHi->AddEntry( origdPtPpHi->GetName(), "pp, R=0.4","l");
  legdPtHi->AddEntry( SmalldPtPpHi->GetName(), "pp, R=0.2","l");
  legdPtHi->AddEntry( LargedPtPpHi->GetName(), "pp, R=0.4, matched to 0.2","l");
  legdPtHi->Draw();
  gPad->SaveAs("plots/ppFollowdPtHi.png");

  // --------------------------------------------------
  cdPtLo->cd();
  origdPtPpLo->Draw();
  SmalldPtPpLo->Draw("same");
  LargedPtPpLo->Draw("same");

  legdPtLo->AddEntry( origdPtPpLo->GetName(), "pp, R=0.4","l");
  legdPtLo->AddEntry( SmalldPtPpLo->GetName(), "pp, R=0.2","l");
  legdPtLo->AddEntry( LargedPtPpLo->GetName(), "pp, R=0.4, matched to 0.2","l");
  legdPtLo->Draw();

  gPad->SaveAs("plots/ppFollowdPtLo.png");


  
}

