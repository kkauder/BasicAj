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
int FigDijetZg() {
  bool ShowHi=false;
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  int RebinZg=2;  // Gotta stick to 1 until Systematics can deal with it

  // TString sAuAu       = "AjResults/Groom_AuAu_HT54_HTled.root";
  // TString sppInAuAu   = "AjResults/Tow0_Eff0_Groom_Aj_HT54_HTled_ppInAuAuAj.root";
  // TString sSyst       = "AjResults/zgSystematics_Groom_Aj_HT54_HTled_ppInAuAuAj.root";
  // TString sAuAu       = "AjResults/Groom_AuAu_AjGt3_HT54_HTled.root";
  // TString sppInAuAu   = "AjResults/Tow0_Eff0_Groom_Aj_HT54_HTled_ppInAuAuAj_AjGt3.root";
  // TString sSyst       = "AjResults/zgSystematics_Groom_Aj_HT54_HTled_ppInAuAuAj_AjGt3.root";
  TString sAuAu       = "AjResults/Groom_AuAu_AjLt3_HT54_HTled.root";
  TString sppInAuAu   = "AjResults/Tow0_Eff0_Groom_Aj_HT54_HTled_ppInAuAuAj_AjLt3.root";
  TString sSyst       = "AjResults/zgSystematics_Groom_Aj_HT54_HTled_ppInAuAuAj_AjLt3.root";

  TString outbase = gSystem->BaseName(sAuAu);
  outbase.ReplaceAll (".root","");
  outbase+="_";
  outbase+=gSystem->BaseName(sppInAuAu);
  outbase.ReplaceAll (".root","");

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%

  // Some references
  TString spp       = "AjResults/Groom_Aj_HT54_HTled_NoEff_ppAj.root";
  // TString sGeant    = "AjResults/Groom_Aj_HT54_HTled_NoEff_Geant.root";
  TString sGeant    = "AjResults/Above25_Groom_Aj_HT54_HTled_NoEff_Geant.root";
    
  // --------------------------------------------------------------
  // ------------------------ Load AuAu ---------------------------
  // --------------------------------------------------------------
  TFile *ftmp = new TFile( sAuAu, "READ");
  TH2D* h2 = (TH2D*) ftmp->Get( "zgLead2030Hi");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();
  
  cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  


  TFile *fAuAu = new TFile( sAuAu, "READ");
  TH1D* AuAu_zgLead2030Hi  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgLead2030Hi") )->ProjectionX("AuAu_zgLead2030Hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* AuAu_zgLead3040Hi  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgLead3040Hi") )->ProjectionX("AuAu_zgLead3040Hi", AuAuMultBinL, AuAuMultBinR);

  TH1D* AuAu_zgSubLead1020Hi  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgSubLead1020Hi") )->ProjectionX("AuAu_zgSubLead1020Hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* AuAu_zgSubLead2030Hi  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgSubLead2030Hi") )->ProjectionX("AuAu_zgSubLead2030Hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* AuAu_zgSubLead3040Hi  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgSubLead3040Hi") )->ProjectionX("AuAu_zgSubLead3040Hi", AuAuMultBinL, AuAuMultBinR);

  TH1D* AuAu_zgLead2030Lo  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgLead2030Lo") )->ProjectionX("AuAu_zgLead2030Lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* AuAu_zgLead3040Lo  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgLead3040Lo") )->ProjectionX("AuAu_zgLead3040Lo", AuAuMultBinL, AuAuMultBinR);

  TH1D* AuAu_zgSubLead1020Lo  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgSubLead1020Lo") )->ProjectionX("AuAu_zgSubLead1020Lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* AuAu_zgSubLead2030Lo  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgSubLead2030Lo") )->ProjectionX("AuAu_zgSubLead2030Lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* AuAu_zgSubLead3040Lo  = (TH1D*) ( (TH2D*) fAuAu->Get( "zgSubLead3040Lo") )->ProjectionX("AuAu_zgSubLead3040Lo", AuAuMultBinL, AuAuMultBinR);


  // --------------------------------------------------------------
  // ---------------------- Load ppInAuAu -------------------------
  // --------------------------------------------------------------
  TFile *fppInAuAu = new TFile( sppInAuAu, "READ");
  TH1D* ppInAuAu_zgLead2030Hi  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgLead2030Hi") )->ProjectionX("ppInAuAu_zgLead2030Hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu_zgLead3040Hi  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgLead3040Hi") )->ProjectionX("ppInAuAu_zgLead3040Hi", AuAuMultBinL, AuAuMultBinR);

  TH1D* ppInAuAu_zgSubLead1020Hi  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgSubLead1020Hi") )->ProjectionX("ppInAuAu_zgSubLead1020Hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu_zgSubLead2030Hi  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgSubLead2030Hi") )->ProjectionX("ppInAuAu_zgSubLead2030Hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu_zgSubLead3040Hi  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgSubLead3040Hi") )->ProjectionX("ppInAuAu_zgSubLead3040Hi", AuAuMultBinL, AuAuMultBinR);

  TH1D* ppInAuAu_zgLead2030Lo  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgLead2030Lo") )->ProjectionX("ppInAuAu_zgLead2030Lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu_zgLead3040Lo  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgLead3040Lo") )->ProjectionX("ppInAuAu_zgLead3040Lo", AuAuMultBinL, AuAuMultBinR);

  TH1D* ppInAuAu_zgSubLead1020Lo  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgSubLead1020Lo") )->ProjectionX("ppInAuAu_zgSubLead1020Lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu_zgSubLead2030Lo  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgSubLead2030Lo") )->ProjectionX("ppInAuAu_zgSubLead2030Lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu_zgSubLead3040Lo  = (TH1D*) ( (TH2D*) fppInAuAu->Get( "zgSubLead3040Lo") )->ProjectionX("ppInAuAu_zgSubLead3040Lo", AuAuMultBinL, AuAuMultBinR);


  // --------------------------------------------------------------
  // -------------------- Load systematics ------------------------
  // --------------------------------------------------------------
  TFile * fSyst = new TFile( sSyst, "READ");
  TH1D* Syst_zgLead2030Hi_minmax  = fSyst->Get("zgLead2030Hi_minmax");
  Syst_zgLead2030Hi_minmax->SetName("Syst_zgLead2030Hi_minmax");
  TH1D* Syst_zgLead3040Hi_minmax  = fSyst->Get("zgLead3040Hi_minmax");
  Syst_zgLead3040Hi_minmax->SetName("Syst_zgLead3040Hi_minmax");

  TH1D* Syst_zgSubLead1020Hi_minmax  = fSyst->Get("zgSubLead1020Hi_minmax");
  Syst_zgSubLead1020Hi_minmax->SetName("Syst_zgSubLead1020Hi_minmax");
  TH1D* Syst_zgSubLead2030Hi_minmax  = fSyst->Get("zgSubLead2030Hi_minmax");
  Syst_zgSubLead2030Hi_minmax->SetName("Syst_zgSubLead2030Hi_minmax");
  TH1D* Syst_zgSubLead3040Hi_minmax  = fSyst->Get("zgSubLead3040Hi_minmax");
  Syst_zgSubLead3040Hi_minmax->SetName("Syst_zgSubLead3040Hi_minmax");

  TH1D* Syst_zgLead2030Lo_minmax  = fSyst->Get("zgLead2030Lo_minmax");
  Syst_zgLead2030Lo_minmax->SetName("Syst_zgLead2030Lo_minmax");
  TH1D* Syst_zgLead3040Lo_minmax  = fSyst->Get("zgLead3040Lo_minmax");
  Syst_zgLead3040Lo_minmax->SetName("Syst_zgLead3040Lo_minmax");

  TH1D* Syst_zgSubLead1020Lo_minmax  = fSyst->Get("zgSubLead1020Lo_minmax");
  Syst_zgSubLead1020Lo_minmax->SetName("Syst_zgSubLead1020Lo_minmax");
  TH1D* Syst_zgSubLead2030Lo_minmax  = fSyst->Get("zgSubLead2030Lo_minmax");
  Syst_zgSubLead2030Lo_minmax->SetName("Syst_zgSubLead2030Lo_minmax");
  TH1D* Syst_zgSubLead3040Lo_minmax  = fSyst->Get("zgSubLead3040Lo_minmax");
  Syst_zgSubLead3040Lo_minmax->SetName("Syst_zgSubLead3040Lo_minmax");

  // --------------------------------------------------------------
  // -------------------- Load references -------------------------
  // --------------------------------------------------------------

  // pp
  TFile *fpp = new TFile( spp, "READ");
  TH1D* pp_zgLead2030Hi  = (TH1D*) ( (TH2D*) fpp->Get( "zgLead2030Hi") )->ProjectionX("pp_zgLead2030Hi");
  TH1D* pp_zgLead3040Hi  = (TH1D*) ( (TH2D*) fpp->Get( "zgLead3040Hi") )->ProjectionX("pp_zgLead3040Hi");

  TH1D* pp_zgSubLead1020Hi  = (TH1D*) ( (TH2D*) fpp->Get( "zgSubLead1020Hi") )->ProjectionX("pp_zgSubLead1020Hi");
  TH1D* pp_zgSubLead2030Hi  = (TH1D*) ( (TH2D*) fpp->Get( "zgSubLead2030Hi") )->ProjectionX("pp_zgSubLead2030Hi");
  TH1D* pp_zgSubLead3040Hi  = (TH1D*) ( (TH2D*) fpp->Get( "zgSubLead3040Hi") )->ProjectionX("pp_zgSubLead3040Hi");

  TH1D* pp_zgLead2030Lo  = (TH1D*) ( (TH2D*) fpp->Get( "zgLead2030Lo") )->ProjectionX("pp_zgLead2030Lo");
  TH1D* pp_zgLead3040Lo  = (TH1D*) ( (TH2D*) fpp->Get( "zgLead3040Lo") )->ProjectionX("pp_zgLead3040Lo");

  TH1D* pp_zgSubLead1020Lo  = (TH1D*) ( (TH2D*) fpp->Get( "zgSubLead1020Lo") )->ProjectionX("pp_zgSubLead1020Lo");
  TH1D* pp_zgSubLead2030Lo  = (TH1D*) ( (TH2D*) fpp->Get( "zgSubLead2030Lo") )->ProjectionX("pp_zgSubLead2030Lo");
  TH1D* pp_zgSubLead3040Lo  = (TH1D*) ( (TH2D*) fpp->Get( "zgSubLead3040Lo") )->ProjectionX("pp_zgSubLead3040Lo");

  // Geant
  TFile *fGeant = new TFile( sGeant, "READ");
  TH1D* Geant_zgLead2030Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgLead2030Hi") )->ProjectionX("Geant_zgLead2030Hi");
  TH1D* Geant_zgLead3040Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgLead3040Hi") )->ProjectionX("Geant_zgLead3040Hi");

  TH1D* Geant_zgSubLead1020Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead1020Hi") )->ProjectionX("Geant_zgSubLead1020Hi");
  TH1D* Geant_zgSubLead2030Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead2030Hi") )->ProjectionX("Geant_zgSubLead2030Hi");
  TH1D* Geant_zgSubLead3040Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead3040Hi") )->ProjectionX("Geant_zgSubLead3040Hi");

  TH1D* Geant_zgLead2030Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgLead2030Lo") )->ProjectionX("Geant_zgLead2030Lo");
  TH1D* Geant_zgLead3040Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgLead3040Lo") )->ProjectionX("Geant_zgLead3040Lo");

  TH1D* Geant_zgSubLead1020Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead1020Lo") )->ProjectionX("Geant_zgSubLead1020Lo");
  TH1D* Geant_zgSubLead2030Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead2030Lo") )->ProjectionX("Geant_zgSubLead2030Lo");
  TH1D* Geant_zgSubLead3040Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead3040Lo") )->ProjectionX("Geant_zgSubLead3040Lo");

  // Prep Theory curve
  TF1* PbarQjet = new TF1("PbarQjet","1./[0] * 4./3. * ( (1+pow(x,2))/(1-x) + (1 + pow(1-x,2))/x )", 0.1, 0.5);
  PbarQjet->SetParameter( 0,4.2593);
  PbarQjet->SetLineColor(kGreen);
  
  TF1* FUVQjet = new TF1("FUVQjet", "[0]*(PbarQjet)", 0.1,0.5);
  FUVQjet->SetParameter( 0,1);

  FUVQjet->SetLineColor(kGray+2);
  FUVQjet->SetLineWidth(1);
  FUVQjet->SetLineStyle(2);
  FUVQjet->SetParameter( 0,1);

  // --------------------------------------------------------------
  // --------------------- Prettifications ------------------------
  // --------------------------------------------------------------
  TObjArray toa;
  toa.Add(AuAu_zgLead2030Hi);
  toa.Add(AuAu_zgLead3040Hi);
  toa.Add(AuAu_zgSubLead1020Hi);
  toa.Add(AuAu_zgSubLead2030Hi);
  toa.Add(AuAu_zgSubLead3040Hi);

  toa.Add(ppInAuAu_zgLead2030Hi);
  toa.Add(ppInAuAu_zgLead3040Hi);
  toa.Add(ppInAuAu_zgSubLead1020Hi);
  toa.Add(ppInAuAu_zgSubLead2030Hi);
  toa.Add(ppInAuAu_zgSubLead3040Hi);

  toa.Add(pp_zgLead2030Hi);
  toa.Add(pp_zgLead3040Hi);
  toa.Add(pp_zgSubLead1020Hi);
  toa.Add(pp_zgSubLead2030Hi);
  toa.Add(pp_zgSubLead3040Hi);

  toa.Add(Geant_zgLead2030Hi);
  toa.Add(Geant_zgLead3040Hi);
  toa.Add(Geant_zgSubLead1020Hi);
  toa.Add(Geant_zgSubLead2030Hi);
  toa.Add(Geant_zgSubLead3040Hi);


  toa.Add(AuAu_zgLead2030Lo);
  toa.Add(AuAu_zgLead3040Lo);
  toa.Add(AuAu_zgSubLead1020Lo);
  toa.Add(AuAu_zgSubLead2030Lo);
  toa.Add(AuAu_zgSubLead3040Lo);

  toa.Add(ppInAuAu_zgLead2030Lo);
  toa.Add(ppInAuAu_zgLead3040Lo);
  toa.Add(ppInAuAu_zgSubLead1020Lo);
  toa.Add(ppInAuAu_zgSubLead2030Lo);
  toa.Add(ppInAuAu_zgSubLead3040Lo);

  toa.Add(pp_zgLead2030Lo);
  toa.Add(pp_zgLead3040Lo);
  toa.Add(pp_zgSubLead1020Lo);
  toa.Add(pp_zgSubLead2030Lo);
  toa.Add(pp_zgSubLead3040Lo);

  toa.Add(Geant_zgLead2030Lo);
  toa.Add(Geant_zgLead3040Lo);
  toa.Add(Geant_zgSubLead1020Lo);
  toa.Add(Geant_zgSubLead2030Lo);
  toa.Add(Geant_zgSubLead3040Lo);


  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);

    h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));

    h->Rebin( RebinZg );
    h->Scale(1./RebinZg );

    h->SetAxisRange( 0,8.5, "y" );
    // h->SetAxisRange( 0,12, "y" );
    // if (TString(h->GetName()).Contains("SubLead"))     h->SetAxisRange( 0,8, "y" );
    h->SetLineWidth( 2 );

    h->SetTitle(";z_{g};arb.");
    h->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetXaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

    if ( TString(h->GetName()).Contains("Hi") ){
      h->SetLineColor( kRed );
      h->SetMarkerColor( kRed );
    } else if ( TString(h->GetName()).Contains("Lo") ){
      h->SetLineColor( kBlack );
      h->SetMarkerColor( kBlack );
    }

    if ( TString(h->GetName()).Contains("ppInAuAu") ){
      h->SetMarkerStyle( 25 );
    } else if ( TString(h->GetName()).Contains("AuAu") ){
      h->SetMarkerStyle( 21 );
    } else if ( TString(h->GetName()).Contains("pp") ){
      h->SetMarkerStyle( 27 );
      h->SetLineColor( kMagenta+1 );
      h->SetMarkerColor( kMagenta+1 );
    } else if ( TString(h->GetName()).Contains("Geant") ){
      h->SetMarkerStyle( 28 );
      h->SetLineColor( kTeal+1 );
      h->SetMarkerColor( kTeal+1 );
    } 

    
  }

  toa.Clear();
  toa.Add(Syst_zgLead2030Hi_minmax);
  toa.Add(Syst_zgLead3040Hi_minmax);
  toa.Add(Syst_zgSubLead1020Hi_minmax);
  toa.Add(Syst_zgSubLead2030Hi_minmax);
  toa.Add(Syst_zgSubLead3040Hi_minmax);

  toa.Add(Syst_zgLead2030Lo_minmax);
  toa.Add(Syst_zgLead3040Lo_minmax);
  toa.Add(Syst_zgSubLead1020Lo_minmax);
  toa.Add(Syst_zgSubLead2030Lo_minmax);
  toa.Add(Syst_zgSubLead3040Lo_minmax);

  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);


    h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
    h->Rebin( RebinZg );
    h->Scale(1./RebinZg );

    h->SetMarkerSize(0);
    h->SetLineWidth( 0 );
    h->SetFillStyle(1001);

    if ( TString(h->GetName()).Contains("Hi") ){
      h->SetFillColor( kRed-10 );
      h->SetMarkerColor(kRed-10);
    } else if ( TString(h->GetName()).Contains("Lo") ){
      h->SetFillColor( kGray );
      h->SetMarkerColor( kGray );
    }
    
  }

    
  // --------------------------------------------------------------
  // ----------------------- Draw Leading -------------------------
  // --------------------------------------------------------------

  // 20-30
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "Leading jet, 20-30 GeV" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  // AuAu_zgLead2030Hi->Draw("9");
  AuAu_zgLead2030Lo->Draw("9");
  if ( ShowHi ) Syst_zgLead2030Hi_minmax->Draw("9E2same");
  Syst_zgLead2030Lo_minmax->Draw("9E2same");

  if ( ShowHi ){
    AuAu_zgLead2030Hi->Draw("9same");
    leg->AddEntry( AuAu_zgLead2030Hi, "Au+Au, Hi Cut");
  
    ppInAuAu_zgLead2030Hi->Draw("9same");
    leg->AddEntry( ppInAuAu_zgLead2030Hi, "pp #oplus Au+Au, Hi Cut");
  }

  
  AuAu_zgLead2030Lo->Draw("9same");
  leg->AddEntry( AuAu_zgLead2030Lo, "Au+Au, Lo Cut");
  
  ppInAuAu_zgLead2030Lo->Draw("9same");
  leg->AddEntry( ppInAuAu_zgLead2030Lo, "pp #oplus Au+Au, Lo Cut");

  pp_zgLead2030Lo->Draw("9same");
  leg->AddEntry( pp_zgLead2030Lo, "pp, Lo Cut");

  Geant_zgLead2030Lo->Draw("9same");
  leg->AddEntry( Geant_zgLead2030Lo, "Geant, Lo Cut");

  FUVQjet->Draw("9same");
  leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  cout << outbase << endl;

  gPad->SaveAs( "plots/"+  outbase + ".DijetLeadZg2030.png");
  
  // if ( false ){
  //   // 30-40
  //   new TCanvas;
  //   gPad->SetGridx(0);  gPad->SetGridy(0);
  //   TLegend* leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "Leading jet, 30-40 GeV" );
  //   leg->SetBorderSize(0);
  //   leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);
  //   leg->SetMargin(0.1);
    
  //   AuAu_zgLead3040Hi->Draw("9");
  //   Syst_zgLead3040Hi_minmax->Draw("9E2same");
  //   Syst_zgLead3040Lo_minmax->Draw("9E2same");
    
  //   AuAu_zgLead3040Hi->Draw("9same");
  //   leg->AddEntry( AuAu_zgLead3040Hi, "Au+Au, Hi Cut");
    
  //   ppInAuAu_zgLead3040Hi->Draw("9same");
  //   leg->AddEntry( ppInAuAu_zgLead3040Hi, "pp #oplus Au+Au, Hi Cut");
    
  //   AuAu_zgLead3040Lo->Draw("9same");
  //   leg->AddEntry( AuAu_zgLead3040Lo, "Au+Au, Lo Cut");
    
  //   ppInAuAu_zgLead3040Lo->Draw("9same");
  //   leg->AddEntry( ppInAuAu_zgLead3040Lo, "pp #oplus Au+Au, Lo Cut");
    
  //   FUVQjet->Draw("9same");
  //   leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  //   leg->Draw();
  //   gPad->SaveAs( "plots/"+  outbase + ".DijetLeadZg3040.png");
  // }
  
  // --------------------------------------------------------------
  // ----------------------- Draw SubLeading -------------------------
  // --------------------------------------------------------------

  // 10-20
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "SubLeading jet, 10-20 GeV" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  
  AuAu_zgSubLead1020Lo->Draw("9");
  if ( ShowHi ) Syst_zgSubLead1020Hi_minmax->Draw("9E2same");
  Syst_zgSubLead1020Lo_minmax->Draw("9E2same");
  
  if ( ShowHi ) {
    AuAu_zgSubLead1020Hi->Draw("9same");
    leg->AddEntry( AuAu_zgSubLead1020Hi, "Au+Au, Hi Cut");
    
    ppInAuAu_zgSubLead1020Hi->Draw("9same");
    leg->AddEntry( ppInAuAu_zgSubLead1020Hi, "pp #oplus Au+Au, Hi Cut");
  }
  
  AuAu_zgSubLead1020Lo->Draw("9same");
  leg->AddEntry( AuAu_zgSubLead1020Lo, "Au+Au, Lo Cut");
  
  ppInAuAu_zgSubLead1020Lo->Draw("9same");
  leg->AddEntry( ppInAuAu_zgSubLead1020Lo, "pp #oplus Au+Au, Lo Cut");
  
  pp_zgSubLead1020Lo->Draw("9same");
  leg->AddEntry( pp_zgSubLead1020Lo, "pp, Lo Cut");

  Geant_zgSubLead1020Lo->Draw("9same");
  leg->AddEntry( Geant_zgSubLead1020Lo, "Geant, Lo Cut");

  FUVQjet->Draw("9same");
  leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg1020.png");

  // // 20-30
  // new TCanvas;
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // TLegend* leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "SubLeading jet, 20-30 GeV" );
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);

  // AuAu_zgSubLead2030Hi->Draw("9");
  // if ( ShowHi )   Syst_zgSubLead2030Hi_minmax->Draw("9E2same");
  // Syst_zgSubLead2030Lo_minmax->Draw("9E2same");
  
  // if ( ShowHi ) {
  //   AuAu_zgSubLead2030Hi->Draw("9same");
  //   leg->AddEntry( AuAu_zgSubLead2030Hi, "Au+Au, Hi Cut");
    
  //   ppInAuAu_zgSubLead2030Hi->Draw("9same");
  //   leg->AddEntry( ppInAuAu_zgSubLead2030Hi, "pp #oplus Au+Au, Hi Cut");
  // }

  // AuAu_zgSubLead2030Lo->Draw("9same");
  // leg->AddEntry( AuAu_zgSubLead2030Lo, "Au+Au, Lo Cut");
  
  // ppInAuAu_zgSubLead2030Lo->Draw("9same");
  // leg->AddEntry( ppInAuAu_zgSubLead2030Lo, "pp #oplus Au+Au, Lo Cut");

  // FUVQjet->Draw("9same");
  // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  // leg->Draw();
  // gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg2030.png");

}
