#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>
using namespace std;

int SoftRemovedCrosscheck() {

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  // int RefmultCut = 351;  // 269 for 0-20%, 399 for 0-10%

  // This would give 40-100%:
  // int AuAuMultBinL = h2->GetYaxis()->FindBin( 0 ); 
  // int AuAuMultBinR = h2->GetYaxis()->FindBin( 113 ); 

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TFile *fAuAu         = TFile::Open("AjResults/Fresh_NicksList_HC100_AuAu.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root");
  TFile *fSoftRemoved     = TFile::Open("AjResults/Tow0_Eff0_SoftDropped_Fresh_NicksList_HC100_ppInAuAuAj.root");
  TFile *fVerySoftRemoved = TFile::Open("AjResults/Tow0_Eff0_VerySoftDropped_Fresh_NicksList_HC100_ppInAuAuAj.root");
  
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
  TH1D* SoftRemoved_lo     = (TH1D*) ( (TH2D*) fSoftRemoved->Get( "AJ_lo") )->ProjectionX("SoftRemoved_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* VerySoftRemoved_lo = (TH1D*) ( (TH2D*) fVerySoftRemoved->Get( "AJ_lo") )->ProjectionX("VerySoftRemoved_lo", AuAuMultBinL, AuAuMultBinR);
  cout << AuAuAJ_lo ->Integral(1,100) << " dijets in AuAu" << endl;
  cout << ppInAuAuAJ_lo ->Integral(1,100) << " dijets in pp@AuAu" << endl;
  cout << SoftRemoved_lo ->Integral(1,100) << " dijets in quenched pp@AuAu" << endl;
  cout << VerySoftRemoved_lo ->Integral(1,100) << " dijets in slightly quenched pp@AuAu" << endl;
  
     
  TObjArray toa;
  toa.Add(AuAuAJ_lo);
  toa.Add(ppInAuAuAJ_lo);
  toa.Add(SoftRemoved_lo);
  toa.Add(VerySoftRemoved_lo);
  
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
  AuAuAJ_lo->SetMarkerStyle( 21 );

  ppInAuAuAJ_lo->SetLineColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerStyle( 25 );

  SoftRemoved_lo->SetLineColor( kMagenta );
  SoftRemoved_lo->SetMarkerColor( kMagenta );
  SoftRemoved_lo->SetMarkerStyle( 25 );

  VerySoftRemoved_lo->SetLineColor( kGreen+1 );
  VerySoftRemoved_lo->SetMarkerColor( kGreen+1 );
  VerySoftRemoved_lo->SetMarkerStyle( 25 );
  
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
  SoftRemoved_lo->Draw("9same");
  VerySoftRemoved_lo->Draw("9same");
//  AuAuAJ_lo->Draw("9same");
    
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);
  char plabel[400];


  latex.SetTextColor(kBlack);
  latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 Gev/c");
  latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 Gev/c");
  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  
  if (TString(fAuAu->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  } else {
    latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  }  

  TLegend* leg = new TLegend( 0.24, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( ppInAuAuAJ_lo,       "pp HT #oplus AuAu MB Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( VerySoftRemoved_lo,  "(pp - 0.9 GeV) #oplus AuAu MB, Matched", "p");
  leg->AddEntry ( SoftRemoved_lo,      "(pp - 1.7 GeV) #oplus AuAu MB, Matched", "p");
//  leg->AddEntry ( AuAuAJ_lo,              "AuAu HT Matched, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->Draw();

  float KS_Nom_AuAu = ppInAuAuAJ_lo->KolmogorovTest(AuAuAJ_lo, "");
  float KS_Soft_AuAu = SoftRemoved_lo->KolmogorovTest(AuAuAJ_lo, "");
  float KS_Soft_Nom = SoftRemoved_lo->KolmogorovTest(ppInAuAuAJ_lo, "");
  float KS_VerySoft_AuAu = VerySoftRemoved_lo->KolmogorovTest(AuAuAJ_lo, "");
  float KS_VerySoft_Nom = VerySoftRemoved_lo->KolmogorovTest(ppInAuAuAJ_lo, "");
  
  cerr << "Kolmogorov-Smirnov for pp @ AuAu vs. AuAu, LOW cut                   :        " << KS_Nom_AuAu << endl;
  cerr << "Kolmogorov-Smirnov for soft-quenched pp @ AuAu vs. AuAu, LOW cut     :        " << KS_Soft_AuAu << endl;
  cerr << "Kolmogorov-Smirnov for soft-quenched pp @ AuAu vs. pp @ AuAu, LOW cut:        " << KS_Soft_Nom << endl;
  cerr << "Kolmogorov-Smirnov for very-soft-quenched pp @ AuAu vs. AuAu, LOW cut     :        " << KS_VerySoft_AuAu << endl;
  cerr << "Kolmogorov-Smirnov for very-soft-quenched pp @ AuAu vs. pp @ AuAu, LOW cut:        " << KS_VerySoft_Nom << endl;

  sprintf ( plabel, "Binned KS p-value:");
  latex.DrawLatex( .5,.5, plabel);

  sprintf ( plabel, "pp vs (pp - 0.9) = %0.1g", KS_VerySoft_Nom);
  latex.DrawLatex( .5,.4, plabel);
  sprintf ( plabel, "pp vs (pp - 1.7) = %0.1g", KS_Soft_Nom);
  latex.DrawLatex( .5,.45, plabel);
  // sprintf ( plabel, "AuAu vs pp = %0.1g", KS_Nom_AuAu);
  // latex.DrawLatex( .5,.45, plabel);
  // sprintf ( plabel, "AuAu vs quenched pp = %0.1g", KS_Drop_AuAu);
  // latex.DrawLatex( .5,.4, plabel);

  gPad->SaveAs("VerySoftRemovedCrosscheck.png");

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

