#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

void RemakeQm() {

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.44, 0.65, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  TFile *fAuAu         = TFile::Open("AjResults/R0.2_AuAuAj.root");
  TFile *fpp           = TFile::Open("AjResults/R0.2_ppAj.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/R0.2_ppInAuAuAj.root");

  // TFile *fAuAu         = TFile::Open("AjResults/Presel_AuAuAj.root");
  // TFile *fpp           = TFile::Open("AjResults/ppAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/ppInAuAuAj.root");


  // TFile *fppInAuAu     = TFile::Open("AjResults/ForcedMatchppInAuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/NoForcedMatchppInAuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/Vz5_ForcedMatchppInAuAuAj.root");

  TObjArray vAJ_lo;
  TObjArray vAJ_hi;

  TH2D* h2 = (TH2D*) fAuAu->Get( "AJ_lo");
  // int AuAuMultBinL = (TH1D*) h2->GetYaxis()->FindBin( 352 );
  int AuAuMultBinL = (TH1D*) h2->GetYaxis()->FindBin( 269 ); // 269 for 0-20%
  int AuAuMultBinR = (TH1D*) h2->GetNbinsY();
  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);

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
    h->Scale(1./h->Integral());
    // h->Scale(1./8147);
      
    h->SetAxisRange(  0.0, 0.22, "y");
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
  
  // TFile* debug = new TFile("Joern/hAjMatchedAbs.root");
  // TH1D* hAjMatchedAbs = (TH1D*) debug->Get("hAjMatchedAbs");
  // ppInAuAuAJ_lo->DrawNormalized();
  // hAjMatchedAbs->DrawNormalized("same");
  // gPad->SaveAs("plots/CompareToMe.png");
  // return;

  ppInAuAuAJ_hi->Draw("9");
  ppInAuAuAJ_lo->Draw("9same");
  AuAuAJ_hi->Draw("9same");
  AuAuAJ_lo->Draw("9same");

  leg->AddEntry ( ppInAuAuAJ_hi, "pp HT #otimes AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( ppInAuAuAJ_lo,          "pp HT #otimes AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( AuAuAJ_hi,     "AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->Draw();

  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    gPad->SaveAs("plots/R0.2_Remake.png");
  } else {
    gPad->SaveAs("plots/RemakeLarge.png");
  }


  // cout << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "D") << endl;
  // cout << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo,"D") << endl;
  
  cout << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->KolmogorovTest(AuAuAJ_hi, "") << endl;
  cout << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut : " << ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "") << endl;

  cout << "chi^2 test for pp @ AuAu vs. AuAu, HIGH cut: " << ppInAuAuAJ_hi->Chi2Test(AuAuAJ_hi, "") << endl;
  cout << "chi^2 test for pp @ AuAu vs. AuAu, LOW cut : " << ppInAuAuAJ_lo->Chi2Test(AuAuAJ_lo, "") << endl;

  //   vAJ_lo.Add (h);
  //   cLo->cd();
  //   TString drawopt="";
  //   if (i!=0) drawopt+="same";
  //   h->Draw(drawopt);

  //   legLo->AddEntry( h->GetName(), legtitles.at(i).data(), "L");

  //   // ========================================================
    
  //   h=(TH1D*) ( ((TFile*) files.At(i))->Get( "AJ_hi") );
  //   TString s = h->GetName(); s+=i;
  //   h->SetName( s );

  //   h->SetLineColor( colors.at(i) );
  //   h->SetLineWidth( 2 );
  //   h->SetTitle( "" );

  //   h->Rebin(2);
  //   h->Scale(1./h->Integral());
  //   h->SetAxisRange( -0.05, 0.3, "y");
  //   h->SetAxisRange( -0.1, 0.75, "x");

    
  //   vAJ_hi.Add (h);
  //   cHi->cd();
  //   TString drawopt="";
  //   if (i!=0) drawopt+="same";
  //   h->Draw(drawopt);

  //   legHi->AddEntry( h->GetName(), legtitles.at(i).data(), "L");


  // }
  // cLo->cd();
  // legLo->Draw();

  // cHi->cd();
  // legHi->Draw();

  // // AJ_lo->Rebin(2);
  // // AJ_hi->Rebin(2);
  
  // // AJ_lo->SetTitle("pp;A_{J}");
  // // AJ_lo->SetLineColor(kBlack);
  // // AJ_lo->SetLineWidth(2.5);
  // // AJ_lo->DrawNormalized();
  // // // AJ_hi->DrawNormalized("same");
  // // AJ_hi->SetLineWidth(2.5);
  // // AJ_hi->SetLineColor(kRed);
  // // AJ_hi->DrawNormalized("same");
  
  // // TLegend* leg = new TLegend ( 0.62, 0.8, 0.99, 0.99);
  // // leg->AddEntry( AJ_hi->GetName(), "pp @ AuAu, p_{T}^{C}>2 GeV/c","l");
  // // leg->AddEntry( AJ_lo->GetName(), "pp @ AuAu, Matched, p_{T}^{C}>0.2 GeV/c","l");
  // // // leg->AddEntry( AJ_hi->GetName(), "pp HT @ AuAu 0-20%, p_{T}^{C}>2 GeV/c","l");
  // // // leg->AddEntry( AJ_lo->GetName(), "p_{T}^{C}>0.2 GeV/c, matched","l");
  // // leg->Draw();
  // // gPad->SaveAs("plots/ppInAuAu.png");
  

  // // new TCanvas;
  // // TFile *f1 = TFile::Open("ppAj.root");
  // // if ( f1->IsOpen() && f1->Get("AJ_lo") ){
  // //   AJ_lo->SetTitle("pp;A_{J}");
  // //   AJ_lo->SetLineColor(kBlack);
  // //   AJ_lo->DrawNormalized();

  // //   AJ_hi->SetLineColor(kGreen);
  // //   AJ_hi->DrawNormalized("same");

  // //   AJ_hi->SetLineColor(kRed);
  // //   AJ_hi->DrawNormalized("same");

  // //   TLegend* leg = new TLegend ( 0.58, 0.6, 0.88, 0.75);
  // //   leg->AddEntry( AJ_hi->GetName(), "p_{T}^{C}>2 GeV/c","l");
  // //   leg->AddEntry( AJ_hi->GetName(), "p_{T}^{C}>2 GeV/c, matched","l");
  // //   leg->AddEntry( AJ_lo->GetName(), "p_{T}^{C}>0.2 GeV/c, matched","l");
  // //   leg->Draw();
  // //   gPad->SaveAs("pp.png");
  // // }
  // // return;

  // // new TCanvas;
  // // TFile *f0 = TFile::Open("ppInAuAu.root");
  // // if ( f0->IsOpen() && f0->Get("AJ_lo") ){
  // //   AJ_lo->SetTitle("pp @ Au+Au, 0-20%");
  // //   AJ_lo->SetLineColor(kRed);
  // //   AJ_lo->DrawNormalized();
  // //   AJ_hi->DrawNormalized("same");
  // //   TLegend* leg = new TLegend ( 0.6, 0.6, 0.85, 0.75);
  // //   leg->AddEntry( AJ_lo->GetName(), "p_{T}^{C}>0.2 GeV/c","l");
  // //   leg->AddEntry( AJ_hi->GetName(), "p_{T}^{C}>2 GeV/c","l");
  // //   leg->Draw();
  // //   gPad->SaveAs("ppEmb.png");
  // // }


  // // new TCanvas;
  // // TFile *f2 = TFile::Open("ppInMc.root");  
  // // if ( f2->IsOpen() && f2->Get("AJ_lo") ){
  // //   AJ_lo->SetTitle("pp @ Mock Au+Au, 0-20%");
  // //   AJ_lo->SetLineColor(kRed);
  // //   AJ_lo->DrawNormalized();
  // //   AJ_hi->DrawNormalized("same");
  // //   TLegend* leg = new TLegend ( 0.6, 0.6, 0.85, 0.75);
  // //   leg->AddEntry( AJ_lo->GetName(), "p_{T}^{C}>0.2 GeV/c","l");
  // //   leg->AddEntry( AJ_hi->GetName(), "p_{T}^{C}>2 GeV/c","l");
  // //   leg->Draw();
  // //   gPad->SaveAs("ppMcEmb.png");
  // // }



}

