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
#include <TROOT.h>

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <exception>


using namespace std;

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


int MCoverlays(){

  // gROOT->ForceStyle();
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);
  // gStyle->SetHistMarkerSize(2);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  bool UseAbove25 = false;

  bool Do_NS_SoftJets_MB=true;
  bool Do_AS_SoftJets_MB=true;
  // bool Do_NS_SoftJets_MB=false;
  // bool Do_AS_SoftJets_MB=false;
  
  bool Do_NS_SoftJets_HT54=true;
  bool Do_AS_SoftJets_HT54=true;
  // bool Do_NS_SoftJets_HT54=false;
  // bool Do_AS_SoftJets_HT54=false;

  // bool Do_NS_HardJets_HT54=true;
  // bool Do_AS_HardJets_HT54=true;
  bool Do_NS_HardJets_HT54=false;
  bool Do_AS_HardJets_HT54=false;


  TString plotpath = "./HPplots/";

  // PYTHIA6 -- GEANT Output. Compare to pp
  // --------------------------------------
  TString sP6_SoftJets_MB   = "HThistos/Groom_Aj_TrueMB_NoEff_Geant.root";
  TString sP6_SoftJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
  TString sP6_HardJets_HT54 = "HThistos/Groom_Aj_TrueMB_NoEff_Geant.root";
  if (UseAbove25){
    sP6_SoftJets_MB   = "HThistos/Above25_Groom_Aj_TrueMB_NoEff_Geant.root";
    sP6_SoftJets_HT54 = "HThistos/Above25_Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
    sP6_HardJets_HT54 = "HThistos/Above25_Groom_Aj_TrueMB_NoEff_Geant.root";
  }    

  // PYTHIA8 -- fully decayed
  // ------------------------
  TString sP8_SoftJets_MB   = "HThistos/TrueMB_LargeEtaPythiaOnly.root";
  TString sP8_SoftJets_HT54 = "HThistos/HT54_HTled_TrueMB_LargeEtaPythiaOnly.root";
  TString sP8_HardJets_HT54 = "HThistos/HT54_HTled_LargeEtaPythiaOnly.root";
  if (UseAbove25){
    TString sP8_SoftJets_MB   = "HThistos/Above25_TrueMB_LargeEtaPythiaOnly.root";
    TString sP8_SoftJets_HT54 = "HThistos/Above25_HT54_HTled_TrueMB_LargeEtaPythiaOnly.root";
    TString sP8_HardJets_HT54 = "HThistos/Above25_HT54_HTled_LargeEtaPythiaOnly.root";
  }


  // Actual pp -- no efficiency mock
  // -------------------------------
  TString sPP_SoftJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root";
  TString sPP_HardJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_NoEff_ppAj.root";

  // Settings and helpers
  // --------------------
  int RebinZg=2;
  int nzgBins=40;
  float zgleft  = 0.05;
  float zgright = 0.55;
  float zgmin   = 0;
  float zgmax   = 8.5;
  TString dopt;
  TString cond;
  TLegend* leg=0;
  
  double IncNSbins[] = { 10, 15, 20, 25, 30, 40, 60};
  int nIncNSbins  = sizeof(IncNSbins) / sizeof(IncNSbins[0])-1;

  double IncASbins[] = { 10, 15, 20, 25, 30, 40, 60};
  int nIncASbins  = sizeof(IncASbins) / sizeof(IncASbins[0])-1;

  // Prep Theory curve
  // -----------------
  TF1* PbarQjet = new TF1("PbarQjet","1./[0] * 4./3. * ( (1+pow(x,2))/(1-x) + (1 + pow(1-x,2))/x )", 0.1, 0.5);
  PbarQjet->SetParameter( 0,4.2593);
  PbarQjet->SetLineColor(kGreen);
  
  TF1* FUVQjet = new TF1("FUVQjet", "[0]*(PbarQjet)", 0.1,0.5);
  FUVQjet->SetParameter( 0,1);
  FUVQjet->SetLineColor(kGray+2);
  FUVQjet->SetLineWidth(1);
  FUVQjet->SetLineStyle(2);
  FUVQjet->SetParameter( 0,1);

  // Fill 2D histos and chop up into 1D
  // ----------------------------------
  
  //    -------------------------------
  //    -------- MB Soft Jets  --------
  //    -------------------------------
  TH2D* h2_NS_P6_SoftJets_MB=0;
  TH2D* h2_NS_P8_SoftJets_MB=0;
  TObjArray* NS_P6_SoftJets_MB=0;
  TObjArray* NS_P8_SoftJets_MB=0;

  TH2D* h2_AS_P6_SoftJets_MB=0;
  TH2D* h2_AS_P8_SoftJets_MB=0;
  TObjArray* AS_P6_SoftJets_MB=0;
  TObjArray* AS_P8_SoftJets_MB=0;

  // NS
  // --
  if ( Do_NS_SoftJets_MB ){
    h2_NS_P6_SoftJets_MB = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_P6_SoftJets_MB");
    h2_NS_P8_SoftJets_MB = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_P8_SoftJets_MB");
    cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 )";
    dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
    Fill ( h2_NS_P6_SoftJets_MB, sP6_SoftJets_MB, "TriggeredTree", dopt + h2_NS_P6_SoftJets_MB->GetName(), cond );
    NS_P6_SoftJets_MB = ChopNormPrettify ( h2_NS_P6_SoftJets_MB, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
    dopt  = "zgtriglo:TriggerJetLo.Pt() >> ";
    Fill ( h2_NS_P8_SoftJets_MB, sP8_SoftJets_MB, "TriggeredTree", dopt + h2_NS_P8_SoftJets_MB->GetName(), cond );
    NS_P8_SoftJets_MB = ChopNormPrettify ( h2_NS_P8_SoftJets_MB, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
  } // Do_NS_SoftJets_MB

  if ( Do_AS_SoftJets_MB ){
    // AS
    // --
    h2_AS_P6_SoftJets_MB = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_P6_SoftJets_MB");
    h2_AS_P8_SoftJets_MB = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_P8_SoftJets_MB");
    cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 )";
    dopt  = "zgawaylo:AwayJetLo.Pt() >> "; 
    Fill ( h2_AS_P6_SoftJets_MB, sP6_SoftJets_MB, "TriggeredTree", dopt + h2_AS_P6_SoftJets_MB->GetName(), cond );
    AS_P6_SoftJets_MB = ChopNormPrettify ( h2_AS_P6_SoftJets_MB, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
  
    dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
    Fill ( h2_AS_P8_SoftJets_MB, sP8_SoftJets_MB, "TriggeredTree", dopt + h2_AS_P8_SoftJets_MB->GetName(), cond );
    AS_P8_SoftJets_MB = ChopNormPrettify ( h2_AS_P8_SoftJets_MB, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
  } // Do_AS_SoftJets_MB
  
  //    -------------------------------
  //    -------- HT54 Soft Jets  --------
  //    -------------------------------
  TH2D* h2_NS_P6_SoftJets_HT54=0;
  TH2D* h2_NS_P8_SoftJets_HT54=0;
  TH2D* h2_NS_PP_SoftJets_HT54=0;

  TObjArray* NS_P6_SoftJets_HT54=0;
  TObjArray* NS_P8_SoftJets_HT54=0;
  TObjArray* NS_PP_SoftJets_HT54=0;

  TH2D* h2_AS_P6_SoftJets_HT54=0;
  TH2D* h2_AS_P8_SoftJets_HT54=0;
  TH2D* h2_AS_PP_SoftJets_HT54=0;

  TObjArray* AS_P6_SoftJets_HT54=0;
  TObjArray* AS_P8_SoftJets_HT54=0;
  TObjArray* AS_PP_SoftJets_HT54=0;

  // NS
  // --
  if ( Do_NS_SoftJets_HT54 ){
    h2_NS_P6_SoftJets_HT54 = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_P6_SoftJets_HT54");
    h2_NS_P8_SoftJets_HT54 = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_P8_SoftJets_HT54");
    h2_NS_PP_SoftJets_HT54 = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_PP_SoftJets_HT54");

    cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 )";
    dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
    Fill ( h2_NS_P6_SoftJets_HT54, sP6_SoftJets_HT54, "TriggeredTree", dopt + h2_NS_P6_SoftJets_HT54->GetName(), cond );
    NS_P6_SoftJets_HT54 = ChopNormPrettify ( h2_NS_P6_SoftJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
    dopt  = "zgtriglo:TriggerJetLo.Pt() >> ";
    Fill ( h2_NS_P8_SoftJets_HT54, sP8_SoftJets_HT54, "TriggeredTree", dopt + h2_NS_P8_SoftJets_HT54->GetName(), cond );
    NS_P8_SoftJets_HT54 = ChopNormPrettify ( h2_NS_P8_SoftJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
    // // cond shouldn't matter for pp
    // dopt  = "zgtriglo:TriggerJetLo.Pt() >> ";
    // cout << sPP_SoftJets_HT54<< endl;
    // cout << dopt << "  " << cond << endl;
    // return 0;

    Fill ( h2_NS_PP_SoftJets_HT54, sPP_SoftJets_HT54, "TriggeredTree", dopt + h2_NS_PP_SoftJets_HT54->GetName(), cond );
    NS_PP_SoftJets_HT54 = ChopNormPrettify ( h2_NS_PP_SoftJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);

  } // Do_NS_SoftJets_HT54

  if ( Do_AS_SoftJets_HT54 ){
    // AS
    // --
    h2_AS_P6_SoftJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_P6_SoftJets_HT54");
    h2_AS_P8_SoftJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_P8_SoftJets_HT54");
    h2_AS_PP_SoftJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_PP_SoftJets_HT54");

    cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 )";
    dopt  = "zgawaylo:AwayJetLo.Pt() >> "; 
    Fill ( h2_AS_P6_SoftJets_HT54, sP6_SoftJets_HT54, "TriggeredTree", dopt + h2_AS_P6_SoftJets_HT54->GetName(), cond );
    AS_P6_SoftJets_HT54 = ChopNormPrettify ( h2_AS_P6_SoftJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
  
    dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
    Fill ( h2_AS_P8_SoftJets_HT54, sP8_SoftJets_HT54, "TriggeredTree", dopt + h2_AS_P8_SoftJets_HT54->GetName(), cond );
    AS_P8_SoftJets_HT54 = ChopNormPrettify ( h2_AS_P8_SoftJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
    // cond shouldn't matter for pp
    dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
    Fill ( h2_AS_PP_SoftJets_HT54, sPP_SoftJets_HT54, "TriggeredTree", dopt + h2_AS_PP_SoftJets_HT54->GetName(), cond );
    AS_PP_SoftJets_HT54 = ChopNormPrettify ( h2_AS_PP_SoftJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);

  } // Do_AS_SoftJets_HT54

  //    -------------------------------
  //    -------- HT54 Hard Jets  --------
  //    -------------------------------
  TH2D* h2_NS_P6_HardJets_HT54=0;
  TH2D* h2_NS_P8_HardJets_HT54=0;
  TH2D* h2_NS_PP_HardJets_HT54=0;

  TObjArray* NS_P6_HardJets_HT54=0;
  TObjArray* NS_P8_HardJets_HT54=0;
  TObjArray* NS_PP_HardJets_HT54=0;

  TH2D* h2_AS_P6_HardJets_HT54=0;
  TH2D* h2_AS_P8_HardJets_HT54=0;
  TH2D* h2_AS_PP_HardJets_HT54=0;

  TObjArray* AS_P6_HardJets_HT54=0;
  TObjArray* AS_P8_HardJets_HT54=0;
  TObjArray* AS_PP_HardJets_HT54=0;

  // OPTIONS HERE!!
  // -- SELECTing on hard pT, but can plot against soft pT
  // -- Using soft zg, could also show hard!

  // NS
  // --
  if ( Do_NS_HardJets_HT54 ){
    h2_NS_P6_HardJets_HT54 = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_P6_HardJets_HT54");
    h2_NS_P8_HardJets_HT54 = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_P8_HardJets_HT54");
    h2_NS_PP_HardJets_HT54 = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_PP_HardJets_HT54");

    cond  = "weight*( abs(TriggerJet.Eta())<0.6 )";
    // dopt  = "zgtrighi:TriggerJet.Pt() >> "; 
    dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
    Fill ( h2_NS_P6_HardJets_HT54, sP6_HardJets_HT54, "TriggeredTree", dopt + h2_NS_P6_HardJets_HT54->GetName(), cond );
    NS_P6_HardJets_HT54 = ChopNormPrettify ( h2_NS_P6_HardJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
    // dopt  = "zgtrighi:TriggerJet.Pt() >> ";
    dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
    Fill ( h2_NS_P8_HardJets_HT54, sP8_HardJets_HT54, "TriggeredTree", dopt + h2_NS_P8_HardJets_HT54->GetName(), cond );
    NS_P8_HardJets_HT54 = ChopNormPrettify ( h2_NS_P8_HardJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
    // cond shouldn't matter for pp
    dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
    Fill ( h2_NS_PP_HardJets_HT54, sPP_HardJets_HT54, "TriggeredTree", dopt + h2_NS_PP_HardJets_HT54->GetName(), cond );
    NS_PP_HardJets_HT54 = ChopNormPrettify ( h2_NS_PP_HardJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);

  } // Do_NS_HardJets_HT54

  if ( Do_AS_HardJets_HT54 ){
    // AS
    // --
    h2_AS_P6_HardJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_P6_HardJets_HT54");
    h2_AS_P8_HardJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_P8_HardJets_HT54");
    h2_AS_PP_HardJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_PP_HardJets_HT54");

    cond  = "weight*( abs(TriggerJet.Eta())<0.6 && abs(AwayJet.Eta())<0.6 )";
    // dopt  = "zgawayhi:AwayJet.Pt() >> "; 
    dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
    Fill ( h2_AS_P6_HardJets_HT54, sP6_HardJets_HT54, "TriggeredTree", dopt + h2_AS_P6_HardJets_HT54->GetName(), cond );
    AS_P6_HardJets_HT54 = ChopNormPrettify ( h2_AS_P6_HardJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
  
    //dopt  = "zgawayhi:AwayJet.Pt() >> ";
    dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
    Fill ( h2_AS_P8_HardJets_HT54, sP8_HardJets_HT54, "TriggeredTree", dopt + h2_AS_P8_HardJets_HT54->GetName(), cond );
    AS_P8_HardJets_HT54 = ChopNormPrettify ( h2_AS_P8_HardJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);
    
    dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
    Fill ( h2_AS_PP_HardJets_HT54, sPP_HardJets_HT54, "TriggeredTree", dopt + h2_AS_PP_HardJets_HT54->GetName(), cond );
    AS_PP_HardJets_HT54 = ChopNormPrettify ( h2_AS_PP_HardJets_HT54, "p_{T}=", "z_{g}", "arb. u.",
					   RebinZg, "ZG", zgmin, zgmax);

  } // Do_AS_HardJets_HT54

  // ==========================================================================================
  // -------------------------
  // --------- Draw ----------
  // -------------------------

  //    -------- MB Soft Jets  --------
  // if ( Do_NS_SoftJets_MB && Do_AS_SoftJets_MB ){
  if ( Do_NS_SoftJets_MB ){
    for (int i=0; i<NS_P6_SoftJets_MB->GetEntries(); ++i ){
      new TCanvas;
      TH1D* h6NS = (TH1D*)NS_P6_SoftJets_MB->At(i);
      TH1D* h8NS = (TH1D*)NS_P8_SoftJets_MB->At(i);
      
      TH1D* h6AS=0;
      TH1D* h8AS=0;
      if ( Do_AS_SoftJets_MB ){
	h6AS = (TH1D*)AS_P6_SoftJets_MB->At(i);
	h8AS = (TH1D*)AS_P8_SoftJets_MB->At(i);
      }
      
      TH1D* dummy = (TH1D*)h6NS->Clone("dummy");
      dummy->Reset();    dummy->SetTitle("");
      dummy->SetAxisRange(zgmin, zgmax, "y");
      dummy->Draw();
            
      if ( Do_AS_SoftJets_MB )  leg = new TLegend( 0.55, 0.55, 0.89, 0.9, h6NS->GetTitle() );
      else                      leg = new TLegend( 0.5 , 0.5, 0.89, 0.9, h6NS->GetTitle() );
      leg->SetBorderSize(0);    leg->SetLineWidth(10);
      leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
      h6NS->SetLineColor(kBlack);
      h6NS->SetMarkerColor(kBlack);
      h6NS->SetMarkerStyle(21);
      h6NS->DrawCopy("9same");
      h8NS->SetLineColor(kRed);
      h8NS->SetMarkerColor(kRed);
      h8NS->SetMarkerStyle(22);
      h8NS->DrawCopy("9same");

      if ( Do_AS_SoftJets_MB ){
	h6AS->SetLineColor(kGray+1);
	h6AS->SetMarkerColor(kGray+1);
	h6AS->SetMarkerStyle(23);
	h6AS->DrawCopy("9same");

	h8AS->SetLineColor(kMagenta+1);
	h8AS->SetMarkerColor(kMagenta+1);
	h8AS->SetMarkerStyle(24);
	h8AS->DrawCopy("9same");
      }
      
      if ( Do_AS_SoftJets_MB ) {
	leg->AddEntry(h6NS,"Pythia6, Trigger Jet", "lp");
	leg->AddEntry(h6AS,"Pythia6, Away Jet", "lp");
	leg->AddEntry(h8NS,"Pythia8, Trigger Jet", "lp");
	leg->AddEntry(h8AS,"Pythia8, Away Jet", "lp");
      } else {
	leg->AddEntry(h6NS,"Pythia6, Leading Jet", "lp");       
	leg->AddEntry(h8NS,"Pythia8, Leading Jet", "lp");
      }

      FUVQjet->Draw("9same");
      leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
      leg->Draw();
      
      // pull bin name from end of histo name
      TString plotname = h6NS->GetName();
      plotname = plotname(plotname.Length()-5, plotname.Length());
      plotname = plotpath+"Geant_SoftJets_MB_"+plotname;
      gPad->SaveAs(plotname+".png");
      
    }
  } //   if ( Do_NS_SoftJets_MB && Do_AS_SoftJets_MB )

  //    -------- HT54 Soft Jets  --------
  if ( Do_NS_SoftJets_HT54 && Do_AS_SoftJets_HT54 ){
    for (int i=0; i<AS_P6_SoftJets_HT54->GetEntries(); ++i ){
      new TCanvas;
      TH1D* h6NS = (TH1D*)NS_P6_SoftJets_HT54->At(i);
      TH1D* h8NS = (TH1D*)NS_P8_SoftJets_HT54->At(i);
      TH1D* hPNS = (TH1D*)NS_PP_SoftJets_HT54->At(i);
      TH1D* h6AS = (TH1D*)AS_P6_SoftJets_HT54->At(i);
      TH1D* h8AS = (TH1D*)AS_P8_SoftJets_HT54->At(i);
      TH1D* hPAS = (TH1D*)AS_PP_SoftJets_HT54->At(i);
      
      TH1D* dummy = (TH1D*)h6NS->Clone("dummy");
      dummy->Reset();    dummy->SetTitle("");
      dummy->SetAxisRange(zgmin, zgmax, "y");
      dummy->Draw();
            
      // leg = new TLegend( 0.55, 0.55, 0.89, 0.9, h6NS->GetTitle() );
      leg = new TLegend( 0.5 , 0.5, 0.89, 0.9, h6NS->GetTitle() );
      leg->SetBorderSize(0);    leg->SetLineWidth(10);
      leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
      // h6NS->SetLineColor(kBlack);
      // h6NS->SetMarkerColor(kBlack);
      // h6NS->SetMarkerStyle(21);
      // h6NS->DrawCopy("9same");
      h8NS->SetLineColor(kRed);
      h8NS->SetMarkerColor(kRed);
      h8NS->SetMarkerStyle(22);
      h8NS->DrawCopy("9same");

      // h6AS->SetLineColor(kGray+1);
      // h6AS->SetMarkerColor(kGray+1);
      // h6AS->SetMarkerStyle(23);
      // h6AS->DrawCopy("9same");
      
      h8AS->SetLineColor(kMagenta+1);
      h8AS->SetMarkerColor(kMagenta+1);
      h8AS->SetMarkerStyle(26);
      h8AS->DrawCopy("9same");
      
      hPNS->SetLineColor(kGreen+1);
      hPNS->SetMarkerColor(kGreen+1);
      hPNS->SetMarkerStyle(29);
      hPNS->DrawCopy("9same");

      hPAS->SetLineColor(kBlue);
      hPAS->SetMarkerColor(kBlue);
      hPAS->SetMarkerStyle(29);
      hPAS->DrawCopy("9same");

      leg->AddEntry(h8NS,"Pythia8, Trigger Jet", "lp");
      leg->AddEntry(h8NS,"p+p HT, Trigger Jet", "lp");
      leg->AddEntry(h8AS,"Pythia8, Away Jet", "lp");
      leg->AddEntry(h8AS,"p+p HT, Away Jet", "lp");



      // h6NS->SetLineColor(kBlack);
      // h6NS->DrawCopy("9same");
      // h8NS->SetLineColor(kRed);
      // h8NS->DrawCopy("9same");
      // hPNS->SetLineColor(kGreen+1);
      // hPNS->DrawCopy("9same");
      
      // h6AS->SetLineColor(kGray+1);
      // h6AS->DrawCopy("9same");
      // h8AS->SetLineColor(kMagenta+1);
      // h8AS->DrawCopy("9same");
      // hPAS->SetLineColor(kTeal);
      // hPAS->DrawCopy("9same");
      
      // leg->AddEntry(h6NS,"Pythia6, TriggerJet", "lp");
      // leg->AddEntry(h6AS,"Pythia6, AwayJet", "lp");
      // leg->AddEntry(h8NS,"Pythia8, TriggerJet", "lp");
      // leg->AddEntry(h8AS,"Pythia8, AwayJet", "lp");
      // leg->AddEntry(hPNS,"p+p HT, TriggerJet", "lp");
      // leg->AddEntry(hPAS,"p+p HT, AwayJet", "lp");

      FUVQjet->Draw("9same");
      leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
      leg->Draw();
      
      // pull bin name from end of histo name
      TString plotname = h6NS->GetName();
      plotname = plotname(plotname.Length()-5, plotname.Length());
      plotname = plotpath+"Geant_SoftJets_HT54_"+plotname;
      gPad->SaveAs(plotname+".png");
      
    }
  } //   if ( Do_NS_SoftJets_HT54 && Do_AS_SoftJets_HT54 )

  //    -------- Soft Jets HT54 v MB  --------
  if ( Do_NS_SoftJets_HT54 && Do_AS_SoftJets_HT54  && 
       Do_NS_SoftJets_MB && Do_AS_SoftJets_MB  ){

    for (int i=0; i<AS_P8_SoftJets_HT54->GetEntries(); ++i ){
      new TCanvas;
      TH1D* h8NS_MB = (TH1D*)NS_P8_SoftJets_MB->At(i);
      TH1D* h8AS_MB = (TH1D*)AS_P8_SoftJets_MB->At(i);
      TH1D* h8NS_HT = (TH1D*)NS_P8_SoftJets_HT54->At(i);
      TH1D* h8AS_HT = (TH1D*)AS_P8_SoftJets_HT54->At(i);
      
      TH1D* dummy = (TH1D*)h8NS_MB->Clone("dummy");
      dummy->Reset();    dummy->SetTitle("");
      dummy->SetAxisRange(zgmin, zgmax, "y");
      dummy->Draw();
            
      leg = new TLegend( 0.5 , 0.5, 0.89, 0.9, TString("Pythia8")+h8AS_MB->GetTitle() );
      leg->SetBorderSize(0);    leg->SetLineWidth(10);
      leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
      h8NS_MB->SetLineColor(kRed);
      h8NS_MB->SetMarkerColor(kRed);
      h8NS_MB->SetMarkerStyle(22);
      h8NS_MB->DrawCopy("9same");

      h8AS_MB->SetLineColor(kMagenta+1);
      h8AS_MB->SetMarkerColor(kMagenta+1);
      h8AS_MB->SetMarkerStyle(26);
      h8AS_MB->DrawCopy("9same");
      
      h8NS_HT->SetLineColor(kGreen+1);
      h8NS_HT->SetMarkerColor(kGreen+1);
      h8NS_HT->SetMarkerStyle(29);
      h8NS_HT->DrawCopy("9same");

      h8AS_HT->SetLineColor(kBlue);
      h8AS_HT->SetMarkerColor(kBlue);
      h8AS_HT->SetMarkerStyle(29);
      h8AS_HT->DrawCopy("9same");

      leg->AddEntry(h8NS_MB,"No trigger, Leading Jet", "lp");
      leg->AddEntry(h8NS_HT,"HT        , Trigger Jet", "lp");
      leg->AddEntry(h8AS_MB,"No trigger, Away Jet", "lp");
      leg->AddEntry(h8NS_HT,"HT        , Away Jet", "lp");



      // h6NS->SetLineColor(kBlack);
      // h6NS->DrawCopy("9same");
      // h8NS->SetLineColor(kRed);
      // h8NS->DrawCopy("9same");
      // hPNS->SetLineColor(kGreen+1);
      // hPNS->DrawCopy("9same");
      
      // h6AS->SetLineColor(kGray+1);
      // h6AS->DrawCopy("9same");
      // h8AS->SetLineColor(kMagenta+1);
      // h8AS->DrawCopy("9same");
      // hPAS->SetLineColor(kTeal);
      // hPAS->DrawCopy("9same");
      
      // leg->AddEntry(h6NS,"Pythia6, TriggerJet", "lp");
      // leg->AddEntry(h6AS,"Pythia6, AwayJet", "lp");
      // leg->AddEntry(h8NS,"Pythia8, TriggerJet", "lp");
      // leg->AddEntry(h8AS,"Pythia8, AwayJet", "lp");
      // leg->AddEntry(hPNS,"p+p HT, TriggerJet", "lp");
      // leg->AddEntry(hPAS,"p+p HT, AwayJet", "lp");

      FUVQjet->Draw("9same");
      leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
      leg->Draw();
      
      // pull bin name from end of histo name
      TString plotname = h8NS_MB->GetName();
      plotname = plotname(plotname.Length()-5, plotname.Length());
      plotname = plotpath+"Geant_MB_v_HT54_"+plotname;
      gPad->SaveAs(plotname+".png");
      
    }
  } //   if ( Do_NS_SoftJets_HT54 && Do_AS_SoftJets_HT54 )

  //    -------- HT54 Hard Jets  --------
  if ( Do_NS_HardJets_HT54 && Do_AS_HardJets_HT54 ){
    for (int i=0; i<AS_P6_HardJets_HT54->GetEntries(); ++i ){
      new TCanvas;
      TH1D* h6NS = (TH1D*)NS_P6_HardJets_HT54->At(i);
      TH1D* h8NS = (TH1D*)NS_P8_HardJets_HT54->At(i);
      TH1D* hPNS = (TH1D*)NS_PP_HardJets_HT54->At(i);
      TH1D* h6AS = (TH1D*)AS_P6_HardJets_HT54->At(i);
      TH1D* h8AS = (TH1D*)AS_P8_HardJets_HT54->At(i);
      TH1D* hPAS = (TH1D*)AS_PP_HardJets_HT54->At(i);
      
      TH1D* dummy = (TH1D*)h6NS->Clone("dummy");
      dummy->Reset();    dummy->SetTitle("");
      dummy->SetAxisRange(zgmin, zgmax, "y");
      dummy->Draw();
            
      leg = new TLegend( 0.55, 0.55, 0.89, 0.9, h6NS->GetTitle() );
      leg->SetBorderSize(0);    leg->SetLineWidth(10);
      leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
      h6NS->SetLineColor(kBlack);
      h6NS->DrawCopy("9same");
      h8NS->SetLineColor(kRed);
      h8NS->DrawCopy("9same");
      hPNS->SetLineColor(kGreen+1);
      hPNS->DrawCopy("9same");
      
      h6AS->SetLineColor(kGray+1);
      h6AS->DrawCopy("9same");
      h8AS->SetLineColor(kMagenta+1);
      h8AS->DrawCopy("9same");
      hPAS->SetLineColor(kTeal);
      hPAS->DrawCopy("9same");
      
      leg->AddEntry(h6NS,"Pythia6, TriggerJet", "lp");
      leg->AddEntry(h6AS,"Pythia6, AwayJet", "lp");
      leg->AddEntry(h8NS,"Pythia8, TriggerJet", "lp");
      leg->AddEntry(h8AS,"Pythia8, AwayJet", "lp");
      leg->AddEntry(hPNS,"p+p HT, TriggerJet", "lp");
      leg->AddEntry(hPAS,"p+p HT, AwayJet", "lp");

      FUVQjet->Draw("9same");
      leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
      leg->Draw();
      
      // pull bin name from end of histo name
      TString plotname = h6NS->GetName();
      plotname = plotname(plotname.Length()-5, plotname.Length());
      plotname = plotpath+"Geant_HardJets_HT54_"+plotname;
      gPad->SaveAs(plotname+".png");
      
    }
  } //   if ( Do_NS_HardJets_HT54 && Do_AS_HardJets_HT54 )

  
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



// Leftover snippets
  // for (int i=0; i<NS_P6_SoftJets_MB->GetEntries(); ++i ){
  //   new TCanvas;
  //   TH1D* h6 = (TH1D*)NS_P6_SoftJets_MB->At(i);
  //   TH1D* h8 = (TH1D*)NS_P8_SoftJets_MB->At(i);
  //   h6->Draw();
  //   h8->SetLineColor(kRed);
  //   h8->Draw("9same");
  // }

  // for (int i=0; i<AS_P6_SoftJets_MB->GetEntries(); ++i ){
  //   new TCanvas;
  //   TH1D* h6 = (TH1D*)AS_P6_SoftJets_MB->At(i);
  //   TH1D* h8 = (TH1D*)AS_P8_SoftJets_MB->At(i);
  //   h6->Draw();
  //   h8->SetLineColor(kRed);
  //   h8->Draw("9same");
  // }
    

