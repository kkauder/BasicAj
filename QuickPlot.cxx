#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void QuickPlot() {
  gStyle->SetOptStat(0);
  TCanvas* cLo = new TCanvas("cLo");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TCanvas* cHi = new TCanvas("cHi");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* legLo = new TLegend( 0.6, 0.5, 0.88, 0.88, "  p_{T}^{C}>0.2 GeV/c" );
  TLegend* legHi = new TLegend( 0.6, 0.5, 0.88, 0.88, "  p_{T}^{C}>2 GeV/c" );

  TFile *fAuAu         = TFile::Open("AjResults/AuAuAj.root");
  TFile *fpp           = TFile::Open("AjResults/ppAj.root");
  // TFile *fpp         = TFile::Open("AjResults/VarPpAj.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/ppInAuAuAj.root");
  TFile *fppInMc       = TFile::Open("AjResults/ppInMcAj.root");
  TFile *fpythiaInAuAu = TFile::Open("AjResults/pythiaInAuAuAj.root");
  TFile *fpythiaInMc   = TFile::Open("AjResults/pythiaInMcAj.root");

  TObjArray files;
  files.Add(fAuAu);
  files.Add(fpp);
  files.Add(fppInAuAu);
  files.Add(fppInMc);
  files.Add(fpythiaInAuAu);
  files.Add(fpythiaInMc);
  
  vector<string> legtitles;
  legtitles.push_back( "Au+Au" );
  legtitles.push_back( "p+p" ); 
  legtitles.push_back( "p+p @ Au+Au" );
  legtitles.push_back( "p+p @ Toy" );
  legtitles.push_back( "pythia @ Au+Au" );
  legtitles.push_back( "pythia @ Toy" );

  vector<int> colors;
  colors.push_back( kBlack );
  colors.push_back( kBlue );
  colors.push_back( kRed );
  colors.push_back( kMagenta );
  colors.push_back( kGreen );
  colors.push_back( kTeal );

  TObjArray vAJ_lo;
  TObjArray vAJ_hi;
  TObjArray vUnmatchedAJ_hi;

  TH1D* h;     
  for (int i=0 ; i<files.GetEntries() ; ++i ){
    h=(TH1D*) ( ((TFile*) files.At(i))->Get( "AJ_lo") );
    TString s = h->GetName(); s+=i;
    h->SetName( s );

    h->SetLineColor( colors.at(i) );
    h->SetLineWidth( 2 );
    h->SetTitle( "" );

    h->Rebin(2);
    h->Scale(1./h->Integral());
    h->SetAxisRange( -0.02, 0.3, "y");
    h->SetAxisRange( -0.1, 0.75, "x");

    
    vAJ_lo.Add (h);
    cLo->cd();
    TString drawopt="";
    if (i!=0) drawopt+="same";
    h->Draw(drawopt);

    legLo->AddEntry( h->GetName(), legtitles.at(i).data(), "L");

    // ========================================================
    
    h=(TH1D*) ( ((TFile*) files.At(i))->Get( "UnmatchedAJ_hi") );
    TString s = h->GetName(); s+=i;
    h->SetName( s );

    h->SetLineColor( colors.at(i) );
    h->SetLineWidth( 2 );
    h->SetTitle( "" );

    h->Rebin(2);
    h->Scale(1./h->Integral());
    h->SetAxisRange( -0.05, 0.3, "y");
    h->SetAxisRange( -0.1, 0.75, "x");

    
    vUnmatchedAJ_hi.Add (h);
    cHi->cd();
    TString drawopt="";
    if (i!=0) drawopt+="same";
    h->Draw(drawopt);

    legHi->AddEntry( h->GetName(), legtitles.at(i).data(), "L");


  }
  cLo->cd();
  legLo->Draw();

  cHi->cd();
  legHi->Draw();

  // AJ_lo->Rebin(2);
  // UnmatchedAJ_hi->Rebin(2);
  
  // AJ_lo->SetTitle("pp;A_{J}");
  // AJ_lo->SetLineColor(kBlack);
  // AJ_lo->SetLineWidth(2.5);
  // AJ_lo->DrawNormalized();
  // // AJ_hi->DrawNormalized("same");
  // UnmatchedAJ_hi->SetLineWidth(2.5);
  // UnmatchedAJ_hi->SetLineColor(kRed);
  // UnmatchedAJ_hi->DrawNormalized("same");
  
  // TLegend* leg = new TLegend ( 0.62, 0.8, 0.99, 0.99);
  // leg->AddEntry( UnmatchedAJ_hi->GetName(), "pp @ AuAu, p_{T}^{C}>2 GeV/c","l");
  // leg->AddEntry( AJ_lo->GetName(), "pp @ AuAu, Matched, p_{T}^{C}>0.2 GeV/c","l");
  // // leg->AddEntry( AJ_hi->GetName(), "pp HT @ AuAu 0-20%, p_{T}^{C}>2 GeV/c","l");
  // // leg->AddEntry( AJ_lo->GetName(), "p_{T}^{C}>0.2 GeV/c, matched","l");
  // leg->Draw();
  // gPad->SaveAs("ppInAuAu.png");
  

  // new TCanvas;
  // TFile *f1 = TFile::Open("ppAj.root");
  // if ( f1->IsOpen() && f1->Get("AJ_lo") ){
  //   AJ_lo->SetTitle("pp;A_{J}");
  //   AJ_lo->SetLineColor(kBlack);
  //   AJ_lo->DrawNormalized();

  //   AJ_hi->SetLineColor(kGreen);
  //   AJ_hi->DrawNormalized("same");

  //   UnmatchedAJ_hi->SetLineColor(kRed);
  //   UnmatchedAJ_hi->DrawNormalized("same");

  //   TLegend* leg = new TLegend ( 0.58, 0.6, 0.88, 0.75);
  //   leg->AddEntry( AJ_hi->GetName(), "p_{T}^{C}>2 GeV/c","l");
  //   leg->AddEntry( UnmatchedAJ_hi->GetName(), "p_{T}^{C}>2 GeV/c, matched","l");
  //   leg->AddEntry( AJ_lo->GetName(), "p_{T}^{C}>0.2 GeV/c, matched","l");
  //   leg->Draw();
  //   gPad->SaveAs("pp.png");
  // }
  // return;

  // new TCanvas;
  // TFile *f0 = TFile::Open("ppInAuAu.root");
  // if ( f0->IsOpen() && f0->Get("AJ_lo") ){
  //   AJ_lo->SetTitle("pp @ Au+Au, 0-20%");
  //   AJ_lo->SetLineColor(kRed);
  //   AJ_lo->DrawNormalized();
  //   AJ_hi->DrawNormalized("same");
  //   TLegend* leg = new TLegend ( 0.6, 0.6, 0.85, 0.75);
  //   leg->AddEntry( AJ_lo->GetName(), "p_{T}^{C}>0.2 GeV/c","l");
  //   leg->AddEntry( AJ_hi->GetName(), "p_{T}^{C}>2 GeV/c","l");
  //   leg->Draw();
  //   gPad->SaveAs("ppEmb.png");
  // }


  // new TCanvas;
  // TFile *f2 = TFile::Open("ppInMc.root");  
  // if ( f2->IsOpen() && f2->Get("AJ_lo") ){
  //   AJ_lo->SetTitle("pp @ Mock Au+Au, 0-20%");
  //   AJ_lo->SetLineColor(kRed);
  //   AJ_lo->DrawNormalized();
  //   AJ_hi->DrawNormalized("same");
  //   TLegend* leg = new TLegend ( 0.6, 0.6, 0.85, 0.75);
  //   leg->AddEntry( AJ_lo->GetName(), "p_{T}^{C}>0.2 GeV/c","l");
  //   leg->AddEntry( AJ_hi->GetName(), "p_{T}^{C}>2 GeV/c","l");
  //   leg->Draw();
  //   gPad->SaveAs("ppMcEmb.png");
  // }



}

