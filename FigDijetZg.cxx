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

int FigDijetZg() {

  bool ShowHi=false;
  bool ShowGeant=false;
  bool ShowPp=false;
  // gROOT->ForceStyle();
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

  TString sAuAu       = "AjResults/Groom_AuAu_HT54_HTled.root";
  TString sppInAuAu   = "AjResults/Tow0_Eff0_Groom_Aj_HT54_HTled_ppInAuAuAj.root";
  TString sSyst       = "AjResults/zgSystematics_Groom_Aj_HT54_HTled_ppInAuAuAj.root";

  // TString sAuAu       = "AjResults/Test_Groom_AuAu_HT54_HTled.root";  
  // TString sppInAuAu   = "AjResults/Tow0_Eff0_Test_Groom_Aj_HT54_HTled_ppInAuAuAj.root";
  // TString sAuAu       = "AjResults/Groom_AuAu_HT54_HTled.root";

  // TString sAuAu       = "AjResults/Groom_AuAu_AjGt3_HT54_HTled.root";
  // TString sppInAuAu   = "AjResults/Tow0_Eff0_Groom_Aj_HT54_HTled_ppInAuAuAj_AjGt3.root";
  // TString sSyst       = "AjResults/zgSystematics_Groom_Aj_HT54_HTled_ppInAuAuAj_AjGt3.root";
  // TString sAuAu       = "AjResults/Groom_AuAu_AjLt3_HT54_HTled.root";
  // TString sppInAuAu   = "AjResults/Tow0_Eff0_Groom_Aj_HT54_HTled_ppInAuAuAj_AjLt3.root";
  // TString sSyst       = "AjResults/zgSystematics_Groom_Aj_HT54_HTled_ppInAuAuAj_AjLt3.root";

  // // TString sAuAu       = "AjResults/Y14Test_Groom_AuAu_HT54_HTled.root";
  // TString sAuAu       = "AjResults/HiY14Test_Groom_AuAu_HT54_HTled.root";
  // TString sppInAuAu   = "AjResults/Tow0_Eff0_Y14Test_Groom_Aj_HT54_HTled_ppInAuAuAj.root";
  // TString sSyst       = "AjResults/zgSystematics_Y14Test_Groom_Aj_HT54_HTled_ppInAuAuAj.root";

  // Some references
  TString sGeant = "AjResults/Above25_Groom_Aj_HT54_HTled_NoEff_Geant.root";
  TString spp = "AjResults/Groom_Aj_HT54_HTled_NoEff_ppAj.root";

  TString outbase = gSystem->BaseName(sAuAu);
  outbase.ReplaceAll (".root","");
  outbase+="_";
  outbase+=gSystem->BaseName(sppInAuAu);
  outbase.ReplaceAll (".root","");

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  // int GCentCut = 6; // 6 for 0-20%
  if ( sAuAu.Contains("Y14Test") ) RefmultCut = 260; // Reasonable approx for y14
    

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
  // -----
  // TFile *fGeant = new TFile( sGeant, "READ");
  // Better to make these ourselves - not sure I included the eta cut 
  TString dopt;
  TString cond;

  if ( ShowGeant){
    TH2D* h2_GeantNS = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "Geant_zgLead");
    cond  = "weight*( abs(j2.Eta())<0.6 && abs(j1.Eta())<0.6 )";
    dopt  = "zgm1:j1.Pt() >> "; 
    Fill ( h2_GeantNS, sGeant, "ResultTree", dopt + h2_GeantNS->GetName(), cond );
    TObjArray* GeantNS = ChopNormPrettify ( h2_GeantNS, "p_{T}^{det}=", "z_{g}", "1/N dN/dz_{g}",
					    1, "ZG", zgmin, zgmax);
    
    TH2D* h2_GeantAS = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "Geant_zgSubLead");
    cond  = "weight*( abs(j2.Eta())<0.6 && abs(j1.Eta())<0.6 )";
    dopt  = "zgm2:j2.Pt() >> "; 
    Fill ( h2_GeantAS, sGeant, "ResultTree", dopt + h2_GeantAS->GetName(), cond );
    TObjArray* GeantAS = ChopNormPrettify ( h2_GeantAS, "p_{T}^{det}=", "z_{g}", "1/N dN/dz_{g}",
					    1, "ZG", zgmin, zgmax);

    TH1D* Geant_zgLead2030Lo  = (TH1D*) GeantNS->At(0);
    TH1D* Geant_zgLead3040Lo  = (TH1D*) GeantNS->At(1);
    
    TH1D* Geant_zgSubLead1020Lo  = (TH1D*) GeantAS->At(0);
    TH1D* Geant_zgSubLead2030Lo  = (TH1D*) GeantAS->At(1);
    TH1D* Geant_zgSubLead3040Lo  = (TH1D*) GeantAS->At(2);
    
    // TH1D* Geant_zgLead2030Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgLead2030Hi") )->ProjectionX("Geant_zgLead2030Hi");
    // TH1D* Geant_zgLead3040Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgLead3040Hi") )->ProjectionX("Geant_zgLead3040Hi");
    
    // TH1D* Geant_zgSubLead1020Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead1020Hi") )->ProjectionX("Geant_zgSubLead1020Hi");
    // TH1D* Geant_zgSubLead2030Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead2030Hi") )->ProjectionX("Geant_zgSubLead2030Hi");
    // TH1D* Geant_zgSubLead3040Hi  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead3040Hi") )->ProjectionX("Geant_zgSubLead3040Hi");
    
    // TH1D* Geant_zgLead2030Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgLead2030Lo") )->ProjectionX("Geant_zgLead2030Lo");
    // TH1D* Geant_zgLead3040Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgLead3040Lo") )->ProjectionX("Geant_zgLead3040Lo");
    
    // TH1D* Geant_zgSubLead1020Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead1020Lo") )->ProjectionX("Geant_zgSubLead1020Lo");
    // TH1D* Geant_zgSubLead2030Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead2030Lo") )->ProjectionX("Geant_zgSubLead2030Lo");
    // TH1D* Geant_zgSubLead3040Lo  = (TH1D*) ( (TH2D*) fGeant->Get( "zgSubLead3040Lo") )->ProjectionX("Geant_zgSubLead3040Lo");
    
  }
  
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

  // toa.Add(Geant_zgLead2030Hi);
  // toa.Add(Geant_zgLead3040Hi);
  // toa.Add(Geant_zgSubLead1020Hi);
  // toa.Add(Geant_zgSubLead2030Hi);
  // toa.Add(Geant_zgSubLead3040Hi);


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

  if ( ShowGeant){
    toa.Add(Geant_zgLead2030Lo);
    toa.Add(Geant_zgLead3040Lo);
    toa.Add(Geant_zgSubLead1020Lo);
    toa.Add(Geant_zgSubLead2030Lo);
    toa.Add(Geant_zgSubLead3040Lo);
  }


  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);

    h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));

    h->Rebin( RebinZg );
    h->Scale(1./RebinZg );

    h->SetAxisRange( zgmin,zgmax, "y" );
    // h->SetAxisRange( 0,12, "y" );
    // if (TString(h->GetName()).Contains("SubLead"))     h->SetAxisRange( 0,8, "y" );
    h->SetLineWidth( 2 );

    h->SetTitle(";z_{g};1/N dN/dz_{g}");
    // h->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    // h->GetXaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
    // h->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    // h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

    if ( TString(h->GetName()).Contains("Hi") ){
      h->SetLineColor( kRed );
      h->SetMarkerColor( kRed );
    } else if ( TString(h->GetName()).Contains("Lo") ){
      h->SetLineColor( kBlack );
      h->SetMarkerColor( kBlack );
    }

    h->SetMarkerSize(2);
    if ( TString(h->GetName()).Contains("ppInAuAu") ){
      h->SetMarkerStyle( 25 );
    } else if ( TString(h->GetName()).Contains("AuAu") ){
      h->SetMarkerStyle( 21 );
    } else if ( TString(h->GetName()).Contains("pp") ){
      h->SetMarkerStyle( 27 );
      h->SetLineColor(kRed);
      h->SetMarkerColor(kRed);
      h->SetMarkerStyle(29);
      h->SetMarkerSize(2);
      // h->SetLineColor( kMagenta+1 );
      // h->SetMarkerColor( kMagenta+1 );
    } else if ( TString(h->GetName()).Contains("Geant") ){
      h->SetLineColor(kMagenta+3);
      h->SetMarkerColor(kMagenta+3);
      h->SetMarkerStyle(21);
      // h->SetMarkerStyle( 28 );
      // h->SetLineColor( kTeal+1 );
      // h->SetMarkerColor( kTeal+1 );
    } 
    
    h->GetXaxis()->SetTitleSize(0.07);
    h->GetYaxis()->SetTitleSize(0.07);
    h->GetXaxis()->SetTitleOffset(xto);
    h->GetYaxis()->SetTitleOffset(yto);

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
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.4, 0.6, 0.89, 0.9, "Trigger Jet, p_{T}^{Trig} = 20-30 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  TH1D* dummy = (TH1D*) AuAu_zgLead2030Lo->Clone("dummy");
  dummy->Reset();    dummy->SetTitle("");
  dummy->SetAxisRange(zgleft, zgright, "x");
  dummy->SetAxisRange(zgmin, zgmax, "y");
  dummy->SetLineColor(kBlack);
  dummy->Draw("AXIS");

  AuAu_zgLead2030Lo->Draw("9same");
  if ( ShowHi ) Syst_zgLead2030Hi_minmax->Draw("9E2same");
  Syst_zgLead2030Lo_minmax->Draw("9E2same");

  if ( ShowHi ){
    AuAu_zgLead2030Hi->Draw("9same");
    leg->AddEntry( AuAu_zgLead2030Hi, "Au+Au, Hi Cut");
  
    ppInAuAu_zgLead2030Hi->Draw("9same");
    leg->AddEntry( ppInAuAu_zgLead2030Hi, "pp #oplus Au+Au, Hi Cut");
  }

  
  if ( ShowPp )  {
    pp_zgLead2030Lo->Draw("9same");
    leg->AddEntry( pp_zgLead2030Lo, "p+p HT");
    // pp_zgLead2030Lo->Draw("9histsame");
    // leg->AddEntry( pp_zgLead2030Lo, "p+p HT","l");
  }

  AuAu_zgLead2030Lo->Draw("9same");
  leg->AddEntry( AuAu_zgLead2030Lo, "Au+Au HT 0-20%");
  
  ppInAuAu_zgLead2030Lo->Draw("9same");
  leg->AddEntry( ppInAuAu_zgLead2030Lo, "pp #oplus Au+Au MB 0-20%");

  if ( ShowGeant )  {
    //Geant_zgLead2030Lo->Draw("9same");
    // leg->AddEntry( Geant_zgLead2030Lo, "PYTHIA6 #oplus GEANT");
    Geant_zgLead2030Lo->Draw("9histsame");
    leg->AddEntry( Geant_zgLead2030Lo, "PYTHIA6 #oplus GEANT","l");
  }

  // FUVQjet->Draw("9same");
  // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();

  latex.DrawLatex( 0.55, 0.44, "STAR Preliminary");
  // latex.DrawLatex( 0.55, 0.55, "STAR Preliminary");
  // latex.DrawLatex( 0.65, 0.5, "With p_{T}^{Cut}>2 GeV/c:");
  // latex.DrawLatex( 0.65, 0.45, "  p_{T,lead}>20 GeV/c");
  // latex.DrawLatex( 0.65, 0.4, "  p_{T,sublead}>10 GeV/c");

  gPad->SaveAs( "plots/"+  outbase + ".DijetLeadZg2030.png");
  gPad->SaveAs( "plots/"+  outbase + ".DijetLeadZg2030.pdf");
  
  // 30-40
  new TCanvas;
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.4, 0.6, 0.89, 0.9, "Trigger Jet, p_{T}^{Trig} = 30-40 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  TH1D* dummy = (TH1D*) AuAu_zgLead3040Lo->Clone("dummy");
  dummy->Reset();    dummy->SetTitle("");
  dummy->SetAxisRange(zgleft, zgright, "x");
  dummy->SetAxisRange(zgmin, zgmax, "y");
  dummy->SetLineColor(kBlack);
  dummy->Draw("AXIS");

  AuAu_zgLead3040Lo->Draw("9same");
  if ( ShowHi ) Syst_zgLead3040Hi_minmax->Draw("9E2same");
  Syst_zgLead3040Lo_minmax->Draw("9E2same");

  if ( ShowHi ){
    AuAu_zgLead3040Hi->Draw("9same");
    leg->AddEntry( AuAu_zgLead3040Hi, "Au+Au, Hi Cut");
  
    ppInAuAu_zgLead3040Hi->Draw("9same");
    leg->AddEntry( ppInAuAu_zgLead3040Hi, "pp #oplus Au+Au, Hi Cut");
  }

  
  if ( ShowPp )  {
    pp_zgLead3040Lo->Draw("9same");
    leg->AddEntry( pp_zgLead3040Lo, "p+p HT");
    // pp_zgLead3040Lo->Draw("9histsame");
    // leg->AddEntry( pp_zgLead3040Lo, "p+p HT","l");
  }

  AuAu_zgLead3040Lo->Draw("9same");
  leg->AddEntry( AuAu_zgLead3040Lo, "Au+Au HT 0-20%");
  
  ppInAuAu_zgLead3040Lo->Draw("9same");
  leg->AddEntry( ppInAuAu_zgLead3040Lo, "pp #oplus Au+Au MB 0-20%");

  if ( ShowGeant )  {
    //Geant_zgLead3040Lo->Draw("9same");
    // leg->AddEntry( Geant_zgLead3040Lo, "PYTHIA6 #oplus GEANT");
    Geant_zgLead3040Lo->Draw("9histsame");
    leg->AddEntry( Geant_zgLead3040Lo, "PYTHIA6 #oplus GEANT","l");
  }

  // FUVQjet->Draw("9same");
  // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();

  // latex.DrawLatex( 0.55, 0.55, "STAR Preliminary");
  // latex.DrawLatex( 0.65, 0.5, "With p_{T}^{Cut}>2 GeV/c:");
  // latex.DrawLatex( 0.65, 0.45, "  p_{T,lead}>20 GeV/c");
  // latex.DrawLatex( 0.65, 0.4, "  p_{T,sublead}>10 GeV/c");

  gPad->SaveAs( "plots/"+  outbase + ".DijetLeadZg3040.png");
  gPad->SaveAs( "plots/"+  outbase + ".DijetLeadZg3040.pdf");
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
  
  TH1D* dummy2 = (TH1D*) AuAu_zgLead2030Lo->Clone("dummy2");
  dummy2->Reset();    dummy2->SetTitle("");
  dummy2->SetAxisRange(zgleft, zgright, "x");
  dummy2->SetAxisRange(zgmin, zgmax, "y");
  dummy2->SetLineColor(kBlack);
  dummy2->Draw("AXIS");

  AuAu_zgSubLead1020Lo->Draw("9same");
  if ( ShowHi ) Syst_zgSubLead1020Hi_minmax->Draw("9E2same");
  Syst_zgSubLead1020Lo_minmax->Draw("9E2same");
  
  if ( ShowHi ) {
    AuAu_zgSubLead1020Hi->Draw("9same");
    leg->AddEntry( AuAu_zgSubLead1020Hi, "Au+Au, Hi Cut");
    
    ppInAuAu_zgSubLead1020Hi->Draw("9same");
    leg->AddEntry( ppInAuAu_zgSubLead1020Hi, "pp #oplus Au+Au, Hi Cut");
  }
  
  if ( ShowPp )  {
    pp_zgSubLead1020Lo->Draw("9same");
    leg->AddEntry( pp_zgSubLead1020Lo, "p+p HT");
    // pp_zgSubLead1020Lo->Draw("9histsame");
    // leg->AddEntry( pp_zgSubLead1020Lo, "p+p HT","l");
  }    

  AuAu_zgSubLead1020Lo->Draw("9same");
  leg->AddEntry( AuAu_zgSubLead1020Lo, "Au+Au HT 0-20%");
  
  ppInAuAu_zgSubLead1020Lo->Draw("9same");
  leg->AddEntry( ppInAuAu_zgSubLead1020Lo, "pp #oplus Au+Au MB 0-20%");
  
  if ( ShowGeant )  {
    //Geant_zgSubLead1020Lo->Draw("9same");
    // leg->AddEntry( Geant_zgSubLead1020Lo, "PYTHIA6 #oplus GEANT");
    Geant_zgSubLead1020Lo->Draw("9histsame");
    leg->AddEntry( Geant_zgSubLead1020Lo, "PYTHIA6 #oplus GEANT","l");
  }
    
  // FUVQjet->Draw("9same");
  // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  latex.DrawLatex( 0.55, 0.44, "STAR Preliminary");
  gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg1020.png");
  gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg1020.pdf");


  // 20-30
  new TCanvas;
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.4, 0.6, 0.89, 0.9, "Recoil Jet, p_{T}^{Recoil} = 20-30 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  
  TH1D* dummy2 = (TH1D*) AuAu_zgLead2030Lo->Clone("dummy2");
  dummy2->Reset();    dummy2->SetTitle("");
  dummy2->SetAxisRange(zgleft, zgright, "x");
  dummy2->SetAxisRange(zgmin, zgmax, "y");
  dummy2->SetLineColor(kBlack);
  dummy2->Draw("AXIS");

  AuAu_zgSubLead2030Lo->Draw("9same");
  if ( ShowHi ) Syst_zgSubLead2030Hi_minmax->Draw("9E2same");
  Syst_zgSubLead2030Lo_minmax->Draw("9E2same");
  
  if ( ShowHi ) {
    AuAu_zgSubLead2030Hi->Draw("9same");
    leg->AddEntry( AuAu_zgSubLead2030Hi, "Au+Au, Hi Cut");
    
    ppInAuAu_zgSubLead2030Hi->Draw("9same");
    leg->AddEntry( ppInAuAu_zgSubLead2030Hi, "pp #oplus Au+Au, Hi Cut");
  }
  
  if ( ShowPp )  {
    pp_zgSubLead2030Lo->Draw("9same");
    leg->AddEntry( pp_zgSubLead2030Lo, "p+p HT");
    // pp_zgSubLead2030Lo->Draw("9histsame");
    // leg->AddEntry( pp_zgSubLead2030Lo, "p+p HT","l");
  }    

  AuAu_zgSubLead2030Lo->Draw("9same");
  leg->AddEntry( AuAu_zgSubLead2030Lo, "Au+Au HT 0-20%");
  
  ppInAuAu_zgSubLead2030Lo->Draw("9same");
  leg->AddEntry( ppInAuAu_zgSubLead2030Lo, "pp #oplus Au+Au MB 0-20%");
  
  if ( ShowGeant )  {
    //Geant_zgSubLead2030Lo->Draw("9same");
    // leg->AddEntry( Geant_zgSubLead2030Lo, "PYTHIA6 #oplus GEANT");
    Geant_zgSubLead2030Lo->Draw("9histsame");
    leg->AddEntry( Geant_zgSubLead2030Lo, "PYTHIA6 #oplus GEANT","l");
  }
    
  // FUVQjet->Draw("9same");
  // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  latex.DrawLatex( 0.55, 0.44, "STAR Preliminary");
  gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg2030.png");
  gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg2030.pdf");

  
  // ---------------------------------------
  // For Olga :)
  // RATIOS
  // ---------------------------------------
  TF1* f = new TF1 ("f","[0]+[1]*x", 0, 1);
  f->SetParameters(1,0);
  f->SetLineStyle(7);

  TH1D* rLead = AuAu_zgLead2030Lo->Clone("rLead");
  rLead->Divide( ppInAuAu_zgLead2030Lo );
    
  TH1D* Syst_rLead = AuAu_zgLead2030Lo->Clone("Syst_rLead");
  // Do not propagate stat. errors !
  for (int i=0; i<Syst_rLead->GetNbinsX(); ++i ){ Syst_rLead->SetBinError(i,0); }
  Syst_rLead->Divide( Syst_zgLead2030Lo_minmax );  
  Syst_rLead->SetMarkerSize(0);
  Syst_rLead->SetLineWidth( 0 );
  Syst_rLead->SetFillStyle(1001);
  Syst_rLead->SetFillColor( kGray );
  Syst_rLead->SetMarkerColor( kGray );
      

  new TCanvas;
  // rLead->SetTitle ("AuAu / ppInAuAu, Leading, pT>20" );

  TH1D* dummy3 = dummy2->Clone( "dummy3"); dummy3->Reset();
  dummy3->SetTitle("");
  dummy3->SetAxisRange(zgleft, zgright, "x");
  dummy3->SetAxisRange(0, 2, "y");
  dummy3->SetLineColor(kBlack);
  dummy3->SetTitle ("");
  // dummy3->GetYaxis()->SetTitle("p+p / Au+Au");
  dummy3->GetYaxis()->SetTitle("Au+Au / p+p");
  dummy3->SetAxisRange(0,2, "y");
  dummy3->DrawCopy("AXIS");

  rLead->Draw("9same");
  Syst_rLead->Draw("9E2same");
  rLead->Draw("9same");
  f->Draw("9same");
  leg = new TLegend( 0.4, 0.7, 0.89, 0.9, "Trigger Jet, p_{T}^{Trig} = 20-30 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry( rLead, "Au+Au HT / p+p HT #oplus Au+Au MB");
  leg->Draw();
  latex.DrawLatex( 0.55, 0.64, "STAR Preliminary");

  gPad->SaveAs( "plots/"+  outbase + ".RatioLeadZg2030.png");
  gPad->SaveAs( "plots/"+  outbase + ".RatioLeadZg2030.pdf");

  
  // new TCanvas;
  // cout << Syst_rLead->GetBinContent(5) << endl;
  // Syst_rLead->Draw("9E2"); return 0;
  
  TH1D* rSubLead = AuAu_zgSubLead1020Lo->Clone("rSubLead");
  rSubLead->Divide( ppInAuAu_zgSubLead1020Lo );

  TH1D* Syst_rSubLead = AuAu_zgSubLead1020Lo->Clone("Syst_rSubLead");
  // Do not propagate stat. errors !
  for (int i=0; i<Syst_rSubLead->GetNbinsX(); ++i ){ Syst_rSubLead->SetBinError(i,0); }
  Syst_rSubLead->Divide( Syst_zgSubLead1020Lo_minmax );  
  Syst_rSubLead->SetMarkerSize(0);
  Syst_rSubLead->SetLineWidth( 0 );
  Syst_rSubLead->SetFillStyle(1001);
  Syst_rSubLead->SetFillColor( kGray );
  Syst_rSubLead->SetMarkerColor( kGray );

  new TCanvas;
  // rSubLead->SetTitle ("AuAu / ppInAuAu, SubLeading, pT>10" );
  // rSubLead->GetYaxis()->SetTitle("Ratio Au+Au HT / p+p HT #oplus Au+Au MB");
  rSubLead->GetYaxis()->SetTitle("Ratio");
  rSubLead->SetAxisRange(0, 2, "y");
  // rSubLead->Draw("9");
  dummy3->DrawCopy("AXIS");
  rSubLead->Draw("9same");
  Syst_rSubLead->Draw("9E2same");
  rSubLead->Draw("9same");
  f->Draw("same");

  leg = new TLegend( 0.4, 0.7, 0.89, 0.9, "Recoil Jet, p_{T}^{Recoil} = 10-20 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry( rSubLead, "Au+Au HT / p+p HT #oplus Au+Au MB");
  leg->Draw();
  latex.DrawLatex( 0.55, 0.64, "STAR Preliminary");
  
  gPad->SaveAs( "plots/"+  outbase + ".RatioSubLeadZg1020.png");
  gPad->SaveAs( "plots/"+  outbase + ".RatioSubLeadZg1020.pdf");

  
  // // 20-30
  // new TCanvas;
  // gPad->SetLeftMargin( lm );// for bigger labels
  // gPad->SetBottomMargin( bm );// for bigger labels
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
  // latex.DrawLatex( 0.55, 0.64, "STAR Preliminary");
  // gPad->SaveAs( "plots/"+  outbase + ".DijetSubLeadZg2030.png");


  
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

