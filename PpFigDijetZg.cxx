#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>

#include <TLorentzVector.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TChain.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <exception>

using namespace std;

// canvas stuff
float lm = 0.11;
float bm = 0.11;
float yto = 0.5;
float xto = 0.5;


// =============================================================================
TH2D* SetupZgHisto2D ( double ptbins[], int nptbins, 
		       int nbinsx, float xleft, float xright,
		       TString namebase="h" );

void Fill ( TH2D* h2, TString FileName, TString TreeName, TString dopt, TString cond );

TObjArray* ChopNormPrettify ( TH2D* h2, TString titlebase="p_{T}=", 
			      TString xlabel = "", TString ylabel = "",
			      int rebin = 1, TString norm = "",
			      float ymin = -1e6, float ymax = -1e7 );

// =============================================================================

int PpFigDijetZg() {

  bool ShowHi=false;
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  int RebinZg=4;
  float zgleft  = 0.05;
  float zgright = 0.55;
  float zgmin   = 0;
  float zgmax   = 8.5;

  // For handmade histos
  // -------------------
  int nzgBins=40;
  double IncNSbins[] = { 20, 30, 40};
  int nIncNSbins  = sizeof(IncNSbins) / sizeof(IncNSbins[0])-1;

  double IncASbins[] = { 10, 20, 30, 40};
  int nIncASbins  = sizeof(IncASbins) / sizeof(IncASbins[0])-1;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.045);
  // latex.SetTextColor(kGray+3);
  latex.SetTextColor(kRed+3);

  TString sPpStd       = "AjResults/Groom_Aj_HT54_HTled_NoEff_ppAj.root";
  TString sPpSoft      = "AjResults/Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root";

  // TString outbase = gSystem->BaseName(sAuAu);
  // outbase.ReplaceAll (".root","");
  // outbase+="_";
  // outbase+=gSystem->BaseName(sppInAuAu);
  // outbase.ReplaceAll (".root","");
  TString outbase = "PpDijetComp";
  
  int RefmultCut = 0;  // 269 for 0-20%, 399 for 0-10%

  // --------------------------------------------------------------
  // ------------------------ Load  -------------------------------
  // --------------------------------------------------------------
  TFile *fPpStd = new TFile( sPpStd, "READ");
  TH1D* PpStd_zgLead2030Hi  = (TH1D*) ( (TH2D*) fPpStd->Get( "zgLead2030Hi") )->ProjectionX("PpStd_zgLead2030Hi" );
  TH1D* PpStd_zgLead3040Hi  = (TH1D*) ( (TH2D*) fPpStd->Get( "zgLead3040Hi") )->ProjectionX("PpStd_zgLead3040Hi" );
  TH1D* PpStd_zgLead2030Lo  = (TH1D*) ( (TH2D*) fPpStd->Get( "zgLead2030Lo") )->ProjectionX("PpStd_zgLead2030Lo" );
  TH1D* PpStd_zgLead3040Lo  = (TH1D*) ( (TH2D*) fPpStd->Get( "zgLead3040Lo") )->ProjectionX("PpStd_zgLead3040Lo" );

  TH1D* PpStd_zgSubLead1020Hi  = (TH1D*) ( (TH2D*) fPpStd->Get( "zgSubLead1020Hi") )->ProjectionX("PpStd_zgSubLead1020Hi");
  TH1D* PpStd_zgSubLead2030Hi  = (TH1D*) ( (TH2D*) fPpStd->Get( "zgSubLead2030Hi") )->ProjectionX("PpStd_zgSubLead2030Hi");
  TH1D* PpStd_zgSubLead1020Lo  = (TH1D*) ( (TH2D*) fPpStd->Get( "zgSubLead1020Lo") )->ProjectionX("PpStd_zgSubLead1020Lo");
  TH1D* PpStd_zgSubLead2030Lo  = (TH1D*) ( (TH2D*) fPpStd->Get( "zgSubLead2030Lo") )->ProjectionX("PpStd_zgSubLead2030Lo");

  TFile *fPpSoft = new TFile( sPpSoft, "READ");
  TH1D* PpSoft_zgLead2030Hi  = (TH1D*) ( (TH2D*) fPpSoft->Get( "zgLead2030Hi") )->ProjectionX("PpSoft_zgLead2030Hi" );
  TH1D* PpSoft_zgLead3040Hi  = (TH1D*) ( (TH2D*) fPpSoft->Get( "zgLead3040Hi") )->ProjectionX("PpSoft_zgLead3040Hi" );
  TH1D* PpSoft_zgLead2030Lo  = (TH1D*) ( (TH2D*) fPpSoft->Get( "zgLead2030Lo") )->ProjectionX("PpSoft_zgLead2030Lo" );
  TH1D* PpSoft_zgLead3040Lo  = (TH1D*) ( (TH2D*) fPpSoft->Get( "zgLead3040Lo") )->ProjectionX("PpSoft_zgLead3040Lo" );

  TH1D* PpSoft_zgSubLead1020Hi  = (TH1D*) ( (TH2D*) fPpSoft->Get( "zgSubLead1020Hi") )->ProjectionX("PpSoft_zgSubLead1020Hi");
  TH1D* PpSoft_zgSubLead2030Hi  = (TH1D*) ( (TH2D*) fPpSoft->Get( "zgSubLead2030Hi") )->ProjectionX("PpSoft_zgSubLead2030Hi");
  TH1D* PpSoft_zgSubLead1020Lo  = (TH1D*) ( (TH2D*) fPpSoft->Get( "zgSubLead1020Lo") )->ProjectionX("PpSoft_zgSubLead1020Lo");
  TH1D* PpSoft_zgSubLead2030Lo  = (TH1D*) ( (TH2D*) fPpSoft->Get( "zgSubLead2030Lo") )->ProjectionX("PpSoft_zgSubLead2030Lo");

  
  // --------------------------------------------------------------
  // --------------------- Prettifications ------------------------
  // --------------------------------------------------------------
  TObjArray toa;

  toa.Add(PpStd_zgLead2030Hi);
  toa.Add(PpStd_zgLead3040Hi);
  toa.Add(PpStd_zgSubLead1020Hi);
  toa.Add(PpStd_zgSubLead2030Hi);

  toa.Add(PpSoft_zgLead2030Hi);
  toa.Add(PpSoft_zgLead3040Hi);
  toa.Add(PpSoft_zgSubLead1020Hi);
  toa.Add(PpSoft_zgSubLead2030Hi);

  toa.Add(PpStd_zgLead2030Lo);
  toa.Add(PpStd_zgLead3040Lo);
  toa.Add(PpStd_zgSubLead1020Lo);
  toa.Add(PpStd_zgSubLead2030Lo);

  toa.Add(PpSoft_zgLead2030Lo);
  toa.Add(PpSoft_zgLead3040Lo);
  toa.Add(PpSoft_zgSubLead1020Lo);
  toa.Add(PpSoft_zgSubLead2030Lo);

  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);

    h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));

    h->Rebin( RebinZg );
    h->Scale(1./RebinZg );

    h->SetAxisRange( zgmin,zgmax, "y" );
    h->SetLineWidth( 2 );

    h->SetTitle(";z_{g};1/N dN/dz_{g}");

    if ( TString(h->GetName()).Contains("Hi") ){
      h->SetLineColor( kRed );
      h->SetMarkerColor( kRed );
    } else if ( TString(h->GetName()).Contains("Lo") ){
      h->SetLineColor( kBlack );
      h->SetMarkerColor( kBlack );
    }

    h->SetMarkerSize(2);
    if ( TString(h->GetName()).Contains("PpStd") ){
      h->SetMarkerStyle( 27 );
      h->SetLineColor(kRed);
      h->SetMarkerColor(kRed);
      h->SetMarkerStyle(29);
      h->SetMarkerSize(2);
    } else if ( TString(h->GetName()).Contains("PpSoft") ){
      h->SetLineColor(kGreen+2);
      h->SetMarkerColor(kGreen+2);
      h->SetMarkerStyle(21);
    } 
    
    h->GetXaxis()->SetTitleSize(0.07);
    h->GetYaxis()->SetTitleSize(0.07);
    h->GetXaxis()->SetTitleOffset(xto);
    h->GetYaxis()->SetTitleOffset(yto);

  }

  // --------------------------------------------------------------
  // ----------------------- Draw Leading -------------------------
  // --------------------------------------------------------------

  // 20-30
  new TCanvas;
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.4, 0.6, 0.89, 0.9, "Trigger Jet, p_{T}^{Trig} = 20-30 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  TH1D* dummy = (TH1D*) PpStd_zgLead2030Lo->Clone("dummy");
  dummy->Reset();    dummy->SetTitle("");
  dummy->SetAxisRange(zgleft, zgright, "x");
  dummy->SetAxisRange(zgmin, zgmax, "y");
  dummy->SetLineColor(kBlack);
  dummy->Draw();

  if ( ShowHi ){
    PpSoft_zgLead2030Hi->Draw("9same");
    leg->AddEntry( PpSoft_zgLead2030Hi, "p+p HT, p_{T}^{Cut}>0.2 GeV/c, Hi Cut");
  }
  
  PpSoft_zgLead2030Lo->Draw("9same");
  leg->AddEntry( PpSoft_zgLead2030Lo, "p+p HT, p_{T}^{Cut}>0.2 GeV/c");

  if ( ShowHi ){
    PpStd_zgLead2030Hi->Draw("9same");
    leg->AddEntry( PpStd_zgLead2030Hi, "p+p HT, p_{T}^{Cut}>2 GeV/c, Hi Cut");
  }
  
  PpStd_zgLead2030Lo->Draw("9same");
  leg->AddEntry( PpStd_zgLead2030Lo, "p+p HT, p_{T}^{Cut}>2 GeV/c");
  
  

  // FUVQjet->Draw("9same");
  // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();

  latex.DrawLatex( 0.55, 0.44, "STAR Preliminary");
  gPad->SaveAs( "plots/"+  outbase + ".DijetLeadZg2030.png");
  gPad->SaveAs( "plots/"+  outbase + ".DijetLeadZg2030.pdf");
  
  // --------------------------------------------------------------
  // ----------------------- Draw SubLeading -------------------------
  // --------------------------------------------------------------

  // 10-20
  new TCanvas;
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.4, 0.6, 0.89, 0.9, "Recoil Jet, p_{T}^{Recoil} = 10-20 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  
  TH1D* dummy2 = (TH1D*) PpStd_zgLead2030Lo->Clone("dummy2");
  dummy2->Reset();    dummy2->SetTitle("");
  dummy2->SetAxisRange(zgleft, zgright, "x");
  dummy2->SetAxisRange(zgmin, zgmax, "y");
  dummy2->SetLineColor(kBlack);
  dummy2->Draw();

  if ( ShowHi ){
    PpSoft_zgSubLead1020Hi->Draw("9same");
    leg->AddEntry( PpSoft_zgSubLead1020Hi, "p+p HT, p_{T}^{Cut}>0.2 GeV/c, Hi Cut");
  }
  
  PpSoft_zgSubLead1020Lo->Draw("9same");
  leg->AddEntry( PpSoft_zgSubLead1020Lo, "p+p HT, p_{T}^{Cut}>0.2 GeV/c");

  if ( ShowHi ){
    PpStd_zgSubLead1020Hi->Draw("9same");
    leg->AddEntry( PpStd_zgSubLead1020Hi, "p+p HT, p_{T}^{Cut}>2 GeV/c, Hi Cut");
  }
  
  PpStd_zgSubLead1020Lo->Draw("9same");
  leg->AddEntry( PpStd_zgSubLead1020Lo, "p+p HT, p_{T}^{Cut}>2 GeV/c");
  
  
    
  // FUVQjet->Draw("9same");
  // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  latex.DrawLatex( 0.55, 0.44, "STAR Preliminary");
  gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg1020.png");
  gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg1020.pdf");


  return 0;
}
// =============================================================================

void Fill ( TH2D* h2, TString FileName, TString TreeName, TString dopt, TString cond ){
  TChain* T = new TChain ( TreeName );
  T->Add( FileName );
  T->Draw( dopt , cond , "goff" );
  return;
}

// =============================================================================
TObjArray* ChopNormPrettify ( TH2D* h2, TString titlebase, TString xlabel, TString ylabel,
			      int rebin, TString norm, float ymin, float ymax )
{
  TObjArray* ret = new TObjArray();
  // ret->SetOwner(kTRUE);
  
  for (int i=1; i<=h2->GetNbinsX(); ++i ){
    int vbinl = int ( h2->GetXaxis()->GetBinLowEdge( i ) + 0.001 );
    int vbinr = int ( h2->GetXaxis()->GetBinLowEdge( i+1 ) + 0.001 );
    // cout << i << "  " << ptbins[i] << "-" << ptbins[i+1] << endl;
    TString name = h2->GetName();
    name += "_"; name += vbinl;
    name += "_"; name += vbinr;
    TString title = titlebase;
    title += ""; title += vbinl;
    title += "-"; title += vbinr;
    title += " GeV/c";
    title += ";"+xlabel+";"+ylabel;
    TH1D* h = (TH1D*) h2->ProjectionY( name, i, i, "nufnof" );
    h->SetTitle( title );

    // Rebin?
    if (rebin>1) h->Rebin(rebin);
    
    // Normalize?
    // options are to normalize 
    // -- integral to 1 (zg style)
    // -- sum to 1 (Aj style)
    norm.ToUpper();
    if ( norm=="INT" || norm=="ZG" ) h->Scale ( 1./ h->Integral(1, h->GetNbinsX()) / h->GetXaxis()->GetBinWidth(1));
    if ( norm=="SUM" || norm=="AJ" ) h->Scale ( 1./ h->Integral(0, h->GetNbinsX()+1) ); // In this case, incluse over/underflow
    
    // Further prettification here
    // y-axis?
    if (ymin<ymax && ymin >-1e5 ){
      h->SetAxisRange( ymin, ymax, "y" );
    }

    h->GetXaxis()->SetTitleSize(0.07);
    h->GetYaxis()->SetTitleSize(0.07);
    h->GetXaxis()->SetTitleOffset(xto);
    h->GetYaxis()->SetTitleOffset(yto);

    ret->Add(h);

  }

  return ret;
}

// =============================================================================
TH2D* SetupZgHisto2D ( double ptbins[], int nptbins, 
		       int nbinsx, float xleft, float xright,
		       TString namebase )
{
  TH2D* h2 = new TH2D( namebase, "All p_{T} bins", nptbins, ptbins,
		      nbinsx, xleft, xright);

  // for (int i=0; i<nptbins; ++i ){
  //   // cout << i << "  " << ptbins[i] << "-" << ptbins[i+1] << endl;
  //   TString name = namebase;
  //   name += "_"; name += ptbins[i];
  //   name += "_"; name += ptbins[i+1];
  //   TString title = titlebase;
  //   title += ""; title += ptbins[i];
  //   title += "-"; title += ptbins[i+1];
  //   title += " GeV/c";
  //   title += ";"+xlabel+";"+ylabel;
  //   TH1D* h = (TH1D*) h2->ProjectionY( name, i+1, i+2 );
  //   h->SetTitle( title );

  //   // Further prettification here
  //   ret->Add(h);

  //   // new TCanvas;
  //   // h->Draw();
  // }

  return h2;
}

// =============================================================================

