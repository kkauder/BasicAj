#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void FollowPlot() {
  gStyle->SetOptStat(0);
  TCanvas* cLo = new TCanvas("cLo");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TCanvas* cHi = new TCanvas("cHi");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TLegend* legLo = new TLegend( 0.55, 0.7, 0.88, 0.88, "  p_{T}^{C}>0.2 GeV/c" );
  TLegend* legHi = new TLegend( 0.55, 0.7, 0.88, 0.88, "  p_{T}^{C}>2 GeV/c" );

  TFile *fppLarge   = TFile::Open("AjResults/pythiaAj.root");
  TFile *fppFollow  = TFile::Open("AjResults/pythiaFollowAj.root");
  // TFile *fppLarge   = TFile::Open("AjResults/pythiaAj_gg.root");
  // TFile *fppFollow  = TFile::Open("AjResults/pythiaFollowAj_gg.root");
  // TFile *fppLarge   = TFile::Open("AjResults/ppAj.root");
  // TFile *fppFollow  = TFile::Open("AjResults/ppFollowAj.root");
  // TFile *fppLarge   = TFile::Open("AjResults/AuAuAj.root");
  // TFile *fppFollow  = TFile::Open("AjResults/FollowAuAuAj.root");
  // TFile *fppLarge   = TFile::Open("AjResults/AuAuAj.root");
  // TFile *fppFollow  = TFile::Open("AjResults/ppInAuAuFollowAj.root");
  
  // --------------------------------------------------
  TH1D* origHi=(TH1D*)  fppLarge->Get( "UnmatchedAJ_hi");
  TH1D* SmallHi=(TH1D*) fppFollow->Get( "SmallUnmatchedAJ_hi");
  TH1D* LargeHi=(TH1D*) fppFollow->Get( "LargeUnmatchedAJ_hi");
  origHi->Rebin(2);
  origHi->SetLineColor( kGray+2 );  origHi->SetLineWidth( 2 );  origHi->SetTitle( ";A_{J};fraction" );  origHi->Scale(1./origHi->Integral());
  SmallHi->Rebin(2);
  SmallHi->SetLineColor( kRed );  SmallHi->SetLineWidth( 2 );  SmallHi->SetTitle( ";A_{J};fraction" );  SmallHi->Scale(1./SmallHi->Integral());
  LargeHi->Rebin(2);
  LargeHi->SetLineColor( kGreen+1 );  LargeHi->SetLineWidth( 2 );  LargeHi->SetTitle( ";A_{J};fraction" );  LargeHi->Scale(1./LargeHi->Integral());
  
  origHi->SetAxisRange( -0.02, 0.5, "y");
  // origHi->SetAxisRange( -0.02, 0.3, "y");
  origHi->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  TH1D* origLo=(TH1D*)  fppLarge->Get( "AJ_lo");
  TH1D* SmallLo=(TH1D*) fppFollow->Get( "SmallAJ_lo");
  TH1D* LargeLo=(TH1D*) fppFollow->Get( "LargeAJ_lo");
  origLo->Rebin(2);
  origLo->SetLineColor( kGray+2 );  origLo->SetLineWidth( 2 );  origLo->SetTitle( ";A_{J};fraction" );  origLo->Scale(1./origLo->Integral());
  SmallLo->Rebin(2);
  SmallLo->SetLineColor( kRed );  SmallLo->SetLineWidth( 2 );  SmallLo->SetTitle( ";A_{J};fraction" );  SmallLo->Scale(1./SmallLo->Integral());
  LargeLo->Rebin(2);
  LargeLo->SetLineColor( kGreen+1 );  LargeLo->SetLineWidth( 2 );  LargeLo->SetTitle( ";A_{J};fraction" );  LargeLo->Scale(1./LargeLo->Integral());
  
  origLo->SetAxisRange( -0.02, 0.5, "y");
  // origLo->SetAxisRange( -0.02, 0.3, "y");
  origLo->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  cHi->cd();
  origHi->Draw();
  SmallHi->Draw("same");
  LargeHi->Draw("same");
  
  legHi->AddEntry( origHi->GetName(), "pp, R=0.4","l");
  legHi->AddEntry( SmallHi->GetName(), "pp, R=0.2","l");
  legHi->AddEntry( LargeHi->GetName(), "pp, R=0.4, matched to 0.2","l");
  legHi->Draw();
  gPad->SaveAs("plots/ppFollowHi.png");

  // --------------------------------------------------
  cLo->cd();
  origLo->Draw();
  SmallLo->Draw("same");
  LargeLo->Draw("same");

  legLo->AddEntry( origLo->GetName(), "pp, R=0.4","l");
  legLo->AddEntry( SmallLo->GetName(), "pp, R=0.2","l");
  legLo->AddEntry( LargeLo->GetName(), "pp, R=0.4, matched to 0.2","l");
  legLo->Draw();

  gPad->SaveAs("plots/ppFollowLo.png");



  new TCanvas;
  TH1D* SmallHiX = ((TH2D*) fppFollow->Get( "SmallhPtHi"))->ProjectionX("SmallHiX");
  TH1D* LargeHiX = ((TH2D*) fppFollow->Get( "LargehPtHi"))->ProjectionX("LargeHiX");
  
  SmallHiX->Draw();
  LargeHiX->Draw("same");
  // fppLarge->Get( "UnmatchedAJ_hi");


  return;
  
  // --------------------------------------------------
  // --------------------------------------------------
  // --------------------------------------------------
  TCanvas* cdPtLo = new TCanvas("cdPtLo");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TCanvas* cdPtHi = new TCanvas("cdPtHi");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TLegend* legdPtLo = new TLegend( 0.55, 0.7, 0.88, 0.88, "  p_{T}^{C}>0.2 GeV/c" );
  TLegend* legdPtHi = new TLegend( 0.55, 0.7, 0.88, 0.88, "  p_{T}^{C}>2 GeV/c" );

  
  TH1D* origdPtHi=(TH1D*)  fppLarge->Get( "UnmatchedhdPtHi");
  TH1D* SmalldPtHi=(TH1D*) fppFollow->Get( "SmallUnmatchedhdPtHi");
  TH1D* LargedPtHi=(TH1D*) fppFollow->Get( "LargeUnmatchedhdPtHi");
  origdPtHi->Rebin(4);
  origdPtHi->SetLineColor( kGray+2 );  origdPtHi->SetLineWidth( 2 );  origdPtHi->SetTitle( ";#Delta p_{T};fraction" );  origdPtHi->Scale(1./origdPtHi->Integral());
  SmalldPtHi->Rebin(4);
  SmalldPtHi->SetLineColor( kRed );  SmalldPtHi->SetLineWidth( 2 );  SmalldPtHi->SetTitle( ";#Delta p_{T};fraction" );  SmalldPtHi->Scale(1./SmalldPtHi->Integral());
  LargedPtHi->Rebin(4);
  LargedPtHi->SetLineColor( kGreen+1 );  LargedPtHi->SetLineWidth( 2 );  LargedPtHi->SetTitle( ";#Delta p_{T};fraction" );  LargedPtHi->Scale(1./LargedPtHi->Integral());
  
  origdPtHi->SetAxisRange( -0.02, 0.2, "y");
  // origdPtHi->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  TH1D* origdPtLo=(TH1D*)  fppLarge->Get( "hdPtLo");
  TH1D* SmalldPtLo=(TH1D*) fppFollow->Get( "SmallhdPtLo");
  TH1D* LargedPtLo=(TH1D*) fppFollow->Get( "LargehdPtLo");
  origdPtLo->Rebin(4);
  origdPtLo->SetLineColor( kGray+2 );  origdPtLo->SetLineWidth( 2 );  origdPtLo->SetTitle( ";#Delta p_{T};fraction" );  origdPtLo->Scale(1./origdPtLo->Integral());
  SmalldPtLo->Rebin(4);
  SmalldPtLo->SetLineColor( kRed );  SmalldPtLo->SetLineWidth( 2 );  SmalldPtLo->SetTitle( ";#Delta p_{T};fraction" );  SmalldPtLo->Scale(1./SmalldPtLo->Integral());
  LargedPtLo->Rebin(4);
  LargedPtLo->SetLineColor( kGreen+1 );  LargedPtLo->SetLineWidth( 2 );  LargedPtLo->SetTitle( ";#Delta p_{T};fraction" );  LargedPtLo->Scale(1./LargedPtLo->Integral());
  
  origdPtLo->SetAxisRange( -0.02, 0.2, "y");
  // origdPtLo->SetAxisRange( -0.1, 0.75, "x");

  // --------------------------------------------------
  cdPtHi->cd();
  origdPtHi->Draw();
  SmalldPtHi->Draw("same");
  LargedPtHi->Draw("same");
  
  legdPtHi->AddEntry( origdPtHi->GetName(), "pp, R=0.4","l");
  legdPtHi->AddEntry( SmalldPtHi->GetName(), "pp, R=0.2","l");
  legdPtHi->AddEntry( LargedPtHi->GetName(), "pp, R=0.4, matched to 0.2","l");
  legdPtHi->Draw();
  gPad->SaveAs("plots/ppFollowdPtHi.png");

  // --------------------------------------------------
  cdPtLo->cd();
  origdPtLo->Draw();
  SmalldPtLo->Draw("same");
  LargedPtLo->Draw("same");

  legdPtLo->AddEntry( origdPtLo->GetName(), "pp, R=0.4","l");
  legdPtLo->AddEntry( SmalldPtLo->GetName(), "pp, R=0.2","l");
  legdPtLo->AddEntry( LargedPtLo->GetName(), "pp, R=0.4, matched to 0.2","l");
  legdPtLo->Draw();

  gPad->SaveAs("plots/ppFollowdPtLo.png");


  
}

