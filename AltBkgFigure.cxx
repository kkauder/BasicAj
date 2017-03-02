#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"

#include <TObjArray.h>
#include <iostream>

#include <vector>
#include <algorithm>

using namespace std;

void AltBkgFigure() {
  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  // int RefmultCut = 351;  // 269 for 0-20%, 399 for 0-10%
  
  bool nofabs=true;
  bool histstyle=true;
  
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

  TLegend* legem;
  if ( nofabs ){
    legem = new TLegend( 0.13, 0.65, 0.46, 0.88, "RC R=0.2 #rightarrow R=0.4: " );
  } else {
    legem = new TLegend( 0.13, 0.15, 0.46, 0.38, "RC R=0.2 #rightarrow R=0.4: " );
  }
  legem->SetBorderSize(0);
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
  TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root");
  TFile *fSyst         = TFile::Open("AjResults/Systematics_Fresh_NicksList_HC100_ppInAuAuAj.root");

  TFile *fVRCAuAu      = TFile::Open("AjResults/VRC_Fresh_NicksList_HC100.root");
  TFile *fVRCppInAuAu  = TFile::Open("AjResults/VRC_Fresh_NicksList_HC100_ppInAuAu.root");
  // TFile *fVRCppInAuAu  = TFile::Open("AjResults/Tow1_Eff0_VRC_Fresh_NicksList_HC100_ppInAuAu.root");
  
  TFile *f2AuAu         = TFile::Open("AjResults/R0.2_Fresh_NicksList_HC100_AuAu.root");
  TFile *f2ppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_R0.2_Fresh_NicksList_HC100_ppInAuAuAj.root");

  TH2D* h2 = (TH2D*) fAuAu->Get( "AJ_lo");
  if ( nofabs ) h2 = (TH2D*) fAuAu->Get( "NoFabsAJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();

  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* AuAuAJ_lo=0;
  TH1D* ppInAuAuAJ_lo=0;
  TH1D* R2AuAuAJ_lo=0;
  TH1D* R2ppInAuAuAJ_lo=0;
  TH1D* VRCAuAuAJ_lo=0;
  TH1D* VRCppInAuAuAJ_lo=0;
  TH1D* RCAJ_lo=0;

  if ( nofabs ) {
    AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "NoFabsAJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "NoFabsAJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    R2AuAuAJ_lo     = (TH1D*) ( (TH2D*) f2AuAu->Get( "NoFabsAJ_lo") )->ProjectionX("R2AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    R2ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) f2ppInAuAu->Get( "NoFabsAJ_lo") )->ProjectionX("R2ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    VRCAuAuAJ_lo     = (TH1D*) ( (TH2D*) fVRCAuAu->Get( "NoFabsRandomAJ_lo") )->ProjectionX("VRCAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    VRCppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fVRCppInAuAu->Get( "NoFabsRandomAJ_lo") )->ProjectionX("VRCppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  } else {
    AuAuAJ_lo     = (TH1D*) ( (TH2D*) fAuAu->Get( "AJ_lo") )->ProjectionX("AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    R2AuAuAJ_lo     = (TH1D*) ( (TH2D*) f2AuAu->Get( "AJ_lo") )->ProjectionX("R2AuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    R2ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) f2ppInAuAu->Get( "AJ_lo") )->ProjectionX("R2ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    VRCAuAuAJ_lo     = (TH1D*) ( (TH2D*) fVRCAuAu->Get( "RandomAJ_lo") )->ProjectionX("VRCAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
    VRCppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fVRCppInAuAu->Get( "RandomAJ_lo") )->ProjectionX("VRCppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  }

  
  TObjArray toa;
  toa.Add(AuAuAJ_lo);
  toa.Add(ppInAuAuAJ_lo);
  toa.Add(R2AuAuAJ_lo);
  toa.Add(R2ppInAuAuAJ_lo);
  toa.Add(VRCAuAuAJ_lo);
  toa.Add(VRCppInAuAuAJ_lo);
  
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

  R2AuAuAJ_lo->SetLineColor( kGreen+2 );
  R2AuAuAJ_lo->SetMarkerColor( kGreen+2 );
  R2AuAuAJ_lo->SetMarkerStyle( 21 );

  R2ppInAuAuAJ_lo->SetLineColor( kGreen+2 );
  R2ppInAuAuAJ_lo->SetMarkerColor( kGreen+2 );
  R2ppInAuAuAJ_lo->SetMarkerStyle( 25 );

  if ( histstyle ){
    VRCAuAuAJ_lo->SetLineColor( kPink+6 );
    VRCAuAuAJ_lo->SetMarkerColor( kPink+6 );
    VRCAuAuAJ_lo->SetMarkerStyle( 21 );
    
    VRCppInAuAuAJ_lo->SetLineColor( kAzure-6 );
    VRCppInAuAuAJ_lo->SetMarkerColor( kAzure-6 );
    VRCppInAuAuAJ_lo->SetMarkerStyle( 25 );
  } else {
    VRCAuAuAJ_lo->SetMarkerStyle(0);
    VRCppInAuAuAJ_lo->SetMarkerStyle(0);
    VRCAuAuAJ_lo->SetFillColor( kPink+6 );
    VRCppInAuAuAJ_lo->SetFillColor( kAzure-6 );
  }

    
  // ppInAuAuAJ_lo->Draw("9");
  // ppInAuAuAJ_lo->Draw("9same");
  // AuAuAJ_lo->Draw("9same");
  
  // R2ppInAuAuAJ_lo->Draw("9same");
  // R2AuAuAJ_lo->Draw("9same");

  if ( histstyle ){
    VRCppInAuAuAJ_lo->Draw("9");
    // VRCppInAuAuAJ_lo->Draw("9same");
    VRCAuAuAJ_lo->Draw("9same");
  } else {
    // VRCppInAuAuAJ_lo->Draw("9E3same");
    VRCAuAuAJ_lo->Draw("9E3");
    VRCppInAuAuAJ_lo->Draw("9E3same");
  }

  if ( fSyst && false ){
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
  // ppInAuAuAJ_lo->Draw("9same");
  // AuAuAJ_lo->Draw("9same");

  // leglo->AddEntry ( ppInAuAuAJ_lo, "p+p HT #oplus Au+Au MB", "p");
  // leglo->AddEntry ( AuAuAJ_lo,     "Au+Au HT", "p");
  // leglo->Draw();

  if ( histstyle ){
    legem->AddEntry ( VRCppInAuAuAJ_lo, "p+p HT #oplus Au+Au MB", "p");
    legem->AddEntry ( VRCAuAuAJ_lo,     "Au+Au HT", "p");
  } else {
    legem->AddEntry ( VRCppInAuAuAJ_lo, "p+p HT #oplus Au+Au MB", "f");
    legem->AddEntry ( VRCAuAuAJ_lo,     "Au+Au HT", "f");
  }
  legem->Draw();

  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.04);
  if ( nofabs ) latex.SetTextSize(0.037);    

  latex.SetTextSize( 0.04);
  latex.DrawLatex( 0.65, 0.63, "Au+Au, 0-20%");
  // if (TString(fAuAu->GetName()).Contains("R0.2") ){
  //   latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.2");
  // } else {
  //   latex.DrawLatex( 0.65, 0.58, "Anti-k_{T}, R=0.4");
  // }  

  latex.SetTextColor(kBlack);
  latex.SetTextSize( 0.04);
  // latex.DrawLatex( 0.05, 0.95, "p_{T,lead}(p_{T}^{Cut}>2 GeV/c)>20 GeV/c");
  // latex.DrawLatex( 0.54, 0.95, "p_{T,sublead}(p_{T}^{Cut}>2 GeV/c)>10 GeV/c");

  // latex.DrawLatex( 0.65, 0.5, "With p_{T}^{Cut}>2 GeV/c:");

  // latex.DrawLatex( 0.65, 0.45, "  p_{T,lead}>20 GeV/c");
  // latex.DrawLatex( 0.65, 0.4, "  p_{T,sublead}>10 GeV/c");

  // if ( nofabs ) {
  //   latex.DrawLatex( 0.57, 0.84, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 GeV/c");
  //   latex.DrawLatex( 0.57, 0.78, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 GeV/c");
  // } else {
  //   latex.DrawLatex( 0.14, 0.26, "p_{T,1}(p_{T}^{Cut}>2 GeV/c)>20 GeV/c");
  //   latex.DrawLatex( 0.14, 0.19, "p_{T,2}(p_{T}^{Cut}>2 GeV/c)>10 GeV/c");
  //}

  
  // // Line to guide the eye
  TLine line;
  line.SetLineStyle( 2 );
  line.SetLineColor( kGray+2 );
  gPad->Update();
  line.DrawLine( 0, gPad->GetFrame()->GetY1(), 0, gPad->GetFrame()->GetY2() );

  // latex.SetTextColor( kAzure-6 );
  // latex.SetTextSize(0.07);
  // latex.SetTextFont( 62 ); // 42: helvetica, 62: helvetica bold
  // latex.DrawLatex( .14,.6, "STAR");


  // Unbinned KS
  // -----------
  //  vector<double> RefUnbinnedAJ_lo;
  vector<double> Unbinned_VRCAuAu_AJ_lo;
  vector<double> Unbinned_VRCppInAuAu_AJ_lo;
  
  CalcAj( fVRCAuAu, 269,-1, Unbinned_VRCAuAu_AJ_lo, nofabs );
  CalcAj( fVRCppInAuAu, 269, -1, Unbinned_VRCppInAuAu_AJ_lo, nofabs );
  

  float KS = TMath::KolmogorovTest( Unbinned_VRCAuAu_AJ_lo.size(), (const double*) &Unbinned_VRCAuAu_AJ_lo.at(0), Unbinned_VRCppInAuAu_AJ_lo.size(), (const double*) &Unbinned_VRCppInAuAu_AJ_lo.at(0), "");
  float BinKS = VRCAuAuAJ_lo->KolmogorovTest( VRCppInAuAuAJ_lo, "" );
  float BinChi2 = VRCAuAuAJ_lo->Chi2Test( VRCppInAuAuAJ_lo, "" );
  cout << " KS:    " << KS << endl;
  cout << " Binned KS: " << BinKS  << endl;
  cout << " chi^2: " << BinChi2  << endl;

  //cout << " Unbinned chi^2: " <<  TMath::Chi2Test( Unbinned_VRCAuAu_AJ_lo.size(), (const double*) &Unbinned_VRCAuAu_AJ_lo.at(0), Unbinned_VRCppInAuAu_AJ_lo.size(), (const double*) &Unbinned_VRCppInAuAu_AJ_lo.at(0), "") << endl;
  
  char plabel[400];
  latex.SetTextSize(0.04);
  // latex.SetTextColor( AuAuAJ_hi->GetLineColor() );

  // latex.DrawLatex( .33,.23, plabel);

  sprintf ( plabel, "KS p-value = %0.1g", KS);
  latex.DrawLatex( .6,.5, plabel);

  sprintf ( plabel, "Bin KS p-value = %0.1g", BinKS);
  latex.DrawLatex( .6,.45, plabel);

  sprintf ( plabel, "Bin #chi^{2} p-value = %0.1g", BinChi2);
  latex.DrawLatex( .6,.4, plabel);

  if ( nofabs) {
    gPad->SaveAs("plots/NoFabsAltBkgFigure.png");
    gPad->SaveAs("plots/NoFabsAltBkgFigure.pdf");
  } else {
    gPad->SaveAs("plots/AltBkgFigure.png");
    gPad->SaveAs("plots/AltBkgFigure.pdf");
  }
  
}

// ===========================================================================


void CalcAj ( TFile* file, int AuAuMultL, int AuAuMultR, vector<double>& UnbinnedAj_lo, bool nofabs ){
  UnbinnedAj_lo.clear();
  
  TTree* RCTree = (TTree*) file->Get("RCTree");

  double refmult;
  RCTree->SetBranchAddress("refmult", &refmult);
  float modaj_lo;
  RCTree->SetBranchAddress("modaj_lo", &modaj_lo);

  if ( AuAuMultL < 0 ) AuAuMultL = 0;
  if ( AuAuMultR < 0 ) AuAuMultR = 1e6;

  for ( int i=0; i<RCTree->GetEntries(); ++i ){
    RCTree->GetEntry ( i );
    // cout << i << endl;

    if ( refmult<AuAuMultL || refmult > AuAuMultR ) continue;
    if ( nofabs ) {
      UnbinnedAj_lo.push_back( modaj_lo );
    } else {
      UnbinnedAj_lo.push_back( fabs( modaj_lo ) );
    }
  }

  // Grrr - need to be sorted
  std::sort(UnbinnedAj_lo.begin(), UnbinnedAj_lo.end());
}

