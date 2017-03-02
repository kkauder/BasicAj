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
TH1D* SetupAjHisto ( int nbinsx, float xleft, float xright,
		       TString namebase="h" );
void Fill ( TH1D* h, TString FileName, TString TreeName, TString dopt, TString cond );

void NormPrettify ( TH1D* h, TString titlebase="p_{T}=", 
			TString xlabel = "", TString ylabel = "",
			int rebin = 1, TString norm = "",
			float ymin = -1e6, float ymax = -1e7 );

// =============================================================================

// canvas stuff
float lm = 0.13;
float bm = 0.11;
float yto = 0.9;
float xto = 0.9;

int AjMCoverlays(){

  // gROOT->ForceStyle();
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);
  // gStyle->SetHistMarkerSize(2);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);


  bool UseAbove25 = true;
  bool Do_HT54=true;
  bool ShowLo=true;
  bool ShowHi=true;

  // bool Do_SoftJets_MB=true;
  // bool Do_SoftJets_HT54=true;
  // bool Do_HardJets_HT54=true;


  TString plotpath = "./plots/";

  
  // PYTHIA6 -- GEANT
  // ----------------
  TString sGeantMc   = "AjResults/Groom_Aj_HT54_GeantMc.root";
  TString sGeant     = "AjResults/Groom_Aj_HT54_Geant.root";

  // PYTHIA8 -- fully decayed
  // ------------------------
  TString sPythia    = "AjResults/HT54_LargeEtaPythiaOnly.root";

  // Actual pp -- no efficiency mock
  // -------------------------------
  TString sPp        = "AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppAj.root";


  if (UseAbove25){
    sGeantMc   = "AjResults/Above25_Groom_Aj_HT54_GeantMc.root";
    sGeant     = "AjResults/Above25_Groom_Aj_HT54_Geant.root";
    sPythia    = "AjResults/Above25_HT54_LargeEtaPythiaOnly.root";
  }

  
  // // PYTHIA6 -- GEANT Output. Compare to pp
  // // --------------------------------------
  // TString sP6_SoftJets_MB   = "HThistos/Groom_Aj_TrueMB_NoEff_Geant.root";
  // TString sP6_SoftJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
  // TString sP6_HardJets_HT54 = "HThistos/Groom_Aj_TrueMB_NoEff_Geant.root";
  // if (UseAbove25){
  //   sP6_SoftJets_MB   = "HThistos/Above25_Groom_Aj_TrueMB_NoEff_Geant.root";
  //   sP6_SoftJets_HT54 = "HThistos/Above25_Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
  //   sP6_HardJets_HT54 = "HThistos/Above25_Groom_Aj_TrueMB_NoEff_Geant.root";
  // }    

  // // PYTHIA8 -- fully decayed
  // // ------------------------
  // TString sP8_SoftJets_MB   = "HThistos/TrueMB_LargeEtaPythiaOnly.root";
  // TString sP8_SoftJets_HT54 = "HThistos/HT54_HTled_TrueMB_LargeEtaPythiaOnly.root";
  // TString sP8_HardJets_HT54 = "HThistos/HT54_HTled_LargeEtaPythiaOnly.root";
  // if (UseAbove25){
  //   TString sP8_SoftJets_MB   = "HThistos/Above25_TrueMB_LargeEtaPythiaOnly.root";
  //   TString sP8_SoftJets_HT54 = "HThistos/Above25_HT54_HTled_TrueMB_LargeEtaPythiaOnly.root";
  //   TString sP8_HardJets_HT54 = "HThistos/Above25_HT54_HTled_LargeEtaPythiaOnly.root";
  // }


  // // Actual pp -- no efficiency mock
  // // -------------------------------
  // TString sPP_SoftJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root";
  // TString sPP_HardJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_NoEff_ppAj.root";

  // Settings and helpers
  // --------------------
  int Rebin=1;
  int najBins=50;
  float ajleft  = -0.6;
  float ajright = 0.9;
  float ajmin   = 0;
  float ajmax   = 0.16;
  TString dopt;
  TString cond;
  TLegend* leg=0;
  
  //    -------------------------------
  //    -------- HT54 Lo and High  --------
  //    -------------------------------
  TH1D* Lo_Geant_HT54=0;
  TH1D* Lo_GeantMc_HT54=0;
  TH1D* Lo_P8_HT54=0;
  TH1D* Lo_PP_HT54=0;

  TH1D* Hi_Geant_HT54=0;
  TH1D* Hi_GeantMc_HT54=0;
  TH1D* Hi_P8_HT54=0;
  TH1D* Hi_PP_HT54=0;

  if ( Do_HT54 ){

    // Lo
    Lo_Geant_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "Lo_Geant_HT54");
    Lo_GeantMc_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "Lo_GeantMc_HT54");
    Lo_P8_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "Lo_P8_HT54");
    Lo_PP_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "Lo_PP_HT54");
    
    cond  = "weight*( abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";
    dopt  = "aj_lo >> "; 
    Fill ( Lo_Geant_HT54, sGeant, "ResultTree", dopt + Lo_Geant_HT54->GetName(), cond );
    NormPrettify ( Lo_Geant_HT54, "", "A_{J}", "event fraction", Rebin, "AJ", ajmin, ajmax);
    
    Fill ( Lo_GeantMc_HT54, sGeant, "ResultTree", dopt + Lo_GeantMc_HT54->GetName(), cond );
    NormPrettify ( Lo_GeantMc_HT54, "", "A_{J}", "event fraction", Rebin, "AJ", ajmin, ajmax);

    Fill ( Lo_P8_HT54, sPythia, "ResultTree", dopt + Lo_P8_HT54->GetName(), cond );
    NormPrettify ( Lo_P8_HT54, "", "A_{J}", "event fraction", Rebin, "AJ", ajmin, ajmax);

    Fill ( Lo_PP_HT54, sPp, "ResultTree", dopt + Lo_PP_HT54->GetName(), cond );
    NormPrettify ( Lo_PP_HT54, "", "A_{J}", "event fraction", Rebin, "AJ", ajmin, ajmax);

    // Hi
    Hi_Geant_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "Hi_Geant_HT54");
    Hi_GeantMc_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "Hi_GeantMc_HT54");
    Hi_P8_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "Hi_P8_HT54");
    Hi_PP_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "Hi_PP_HT54");
    
    cond  = "weight*( abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";
    dopt  = "aj_hi >> "; 
    Fill ( Hi_Geant_HT54, sGeant, "ResultTree", dopt + Hi_Geant_HT54->GetName(), cond );
    NormPrettify ( Hi_Geant_HT54, "", "A_{J}", "event fraction", Rebin, "AJ", ajmin, ajmax);
    
    Fill ( Hi_GeantMc_HT54, sGeant, "ResultTree", dopt + Hi_GeantMc_HT54->GetName(), cond );
    NormPrettify ( Hi_GeantMc_HT54, "", "A_{J}", "event fraction", Rebin, "AJ", ajmin, ajmax);

    Fill ( Hi_P8_HT54, sPythia, "ResultTree", dopt + Hi_P8_HT54->GetName(), cond );
    NormPrettify ( Hi_P8_HT54, "", "A_{J}", "event fraction", Rebin, "AJ", ajmin, ajmax);

    Fill ( Hi_PP_HT54, sPp, "ResultTree", dopt + Hi_PP_HT54->GetName(), cond );
    NormPrettify ( Hi_PP_HT54, "", "A_{J}", "event fraction", Rebin, "AJ", ajmin, ajmax);
    // new TCanvas; Lo_Geant_HT54->Draw();
    // new TCanvas; Lo_GeantMc_HT54->Draw();
    // new TCanvas; Lo_P8_HT54->Draw();
    // new TCanvas; Lo_PP_HT54->Draw();
    

  } // Do_HT54

  // //    -------------------------------
  // //    -------- MB Soft Jets  --------
  // //    -------------------------------
  // TH1D* P6_SoftJets_MB=0;
  // TH1D* P8_SoftJets_MB=0;

  // if ( Do_SoftJets_MB ){
  //   P6_SoftJets_MB = SetupAjHisto ( najBins, ajleft, ajright, "P6_SoftJets_MB");
  //   P8_SoftJets_MB = SetupAjHisto ( najBins, ajleft, ajright, "P8_SoftJets_MB");
  //   cond  = "weight*( abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";
  //   dopt  = "aj_lo >> "; 
  //   Fill ( P6_SoftJets_MB, sP6_SoftJets_MB, "ResultTree", dopt + P6_SoftJets_MB->GetName(), cond );
  //   NormPrettify ( P6_SoftJets_MB, "", "A_{J}", "event fraction",
  // 		   Rebin, "AJ", ajmin, ajmax);
    
  //   dopt  = "aj_lo >> "; 
  //   Fill ( P8_SoftJets_MB, sP8_SoftJets_MB, "ResultTree", dopt + P8_SoftJets_MB->GetName(), cond );
  //   NormPrettify ( P8_SoftJets_MB, "", "A_{J}", "event fraction",
  // 		   Rebin, "AJ", ajmin, ajmax);
    
  // } // Do_SoftJets_MB
    
  // //    -------------------------------
  // //    -------- HT54 Soft Jets  --------
  // //    -------------------------------
  // TH1D* P6_SoftJets_HT54=0;
  // TH1D* P8_SoftJets_HT54=0;
  // TH1D* PP_SoftJets_HT54=0;

  // if ( Do_SoftJets_HT54 ){
  //   P6_SoftJets_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "P6_SoftJets_HT54");
  //   P8_SoftJets_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "P8_SoftJets_HT54");
  //   PP_SoftJets_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "PP_SoftJets_HT54");
    
  //   cond  = "weight*( abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";
  //   dopt  = "aj_lo >> "; 
  //   Fill ( P6_SoftJets_HT54, sP6_SoftJets_HT54, "ResultTree", dopt + P6_SoftJets_HT54->GetName(), cond );
  //   NormPrettify ( P6_SoftJets_HT54, "", "A_{J}", "event fraction",
  // 		   Rebin, "AJ", ajmin, ajmax);
    
  //   dopt  = "aj_lo >> "; 
  //   Fill ( P8_SoftJets_HT54, sP8_SoftJets_HT54, "ResultTree", dopt + P8_SoftJets_HT54->GetName(), cond );
  //   NormPrettify ( P8_SoftJets_HT54, "", "A_{J}", "event fraction",
  // 				      Rebin, "AJ", ajmin, ajmax);
    
  //   Fill ( PP_SoftJets_HT54, sPP_SoftJets_HT54, "ResultTree", dopt + PP_SoftJets_HT54->GetName(), cond );
  //   NormPrettify ( PP_SoftJets_HT54, "", "A_{J}", "event fraction",
  // 		   Rebin, "AJ", ajmin, ajmax);


  //   // new TCanvas;P6_SoftJets_HT54->Draw();
  //   // new TCanvas;P8_SoftJets_HT54->Draw();
  //   // new TCanvas;PP_SoftJets_HT54->Draw();

  // } // Do_SoftJets_HT54

  // //    -------------------------------
  // //    -------- HT54 Hard Jets  --------
  // //    -------------------------------
  // TH1D* P6_HardJets_HT54=0;
  // TH1D* P8_HardJets_HT54=0;
  // TH1D* PP_HardJets_HT54=0;

  // if ( Do_HardJets_HT54 ){
  //   P6_HardJets_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "P6_HardJets_HT54");
  //   P8_HardJets_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "P8_HardJets_HT54");
  //   PP_HardJets_HT54 = SetupAjHisto ( najBins, ajleft, ajright, "PP_HardJets_HT54");

  //   cond  = "weight*( abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";
  //   dopt  = "aj_hi >> "; 
  //   Fill ( P6_HardJets_HT54, sP6_HardJets_HT54, "ResultTree", dopt + P6_HardJets_HT54->GetName(), cond );
  //   NormPrettify ( P6_HardJets_HT54, "", "A_{J}", "event fraction",
  // 		   Rebin, "AJ", ajmin, ajmax);
    
  //   dopt  = "aj_hi >> "; 
  //   Fill ( P8_HardJets_HT54, sP8_HardJets_HT54, "ResultTree", dopt + P8_HardJets_HT54->GetName(), cond );
  //   NormPrettify ( P8_HardJets_HT54, "", "A_{J}", "event fraction",
  // 		   Rebin, "AJ", ajmin, ajmax);
    
  //   dopt  = "aj_hi >> "; 
  //   Fill ( PP_HardJets_HT54, sPP_HardJets_HT54, "ResultTree", dopt + PP_HardJets_HT54->GetName(), cond );
  //   NormPrettify ( PP_HardJets_HT54, "", "A_{J}", "event fraction",
  // 		   Rebin, "AJ", ajmin, ajmax);

  // } // Do_HardJets_HT54


  // ==========================================================================================
  // -------------------------
  // --------- Draw ----------
  // -------------------------

  if ( ShowLo ){
    new TCanvas;
    gPad->SetLeftMargin( lm );// for bigger labels
    gPad->SetBottomMargin( bm );// for bigger labels
    
    TH1D* dummy = (TH1D*)Lo_P8_HT54->Clone("dummy");
    dummy->Reset();    dummy->SetTitle("");
    dummy->SetAxisRange(ajmin, ajmax, "y");
    dummy->Draw();
    
    leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "AJ_lo" );
    leg->SetBorderSize(0);    leg->SetLineWidth(10);
    leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
    Lo_Geant_HT54->SetLineColor(kBlack);
    Lo_Geant_HT54->DrawCopy("9same");
    Lo_P8_HT54->SetLineColor(kRed);
    Lo_P8_HT54->DrawCopy("9same");
    Lo_PP_HT54->SetLineColor(kGreen+1);
    Lo_PP_HT54->DrawCopy("9same");
      
    leg->AddEntry(Lo_Geant_HT54,"Pythia6 #otimes GEANT", "lp");
    leg->AddEntry(Lo_P8_HT54,"Pythia8", "lp");
    leg->AddEntry(Lo_PP_HT54,"p+p HT", "lp");
    leg->Draw();
      
    // pull bin name from end of histo name
    // TString plotname = h6->GetName();
    // plotname = plotname(plotname.Length()-5, plotname.Length());
    TString plotname = "";
    plotname = plotpath+"LoJets_HT54"+plotname;
    gPad->SaveAs(plotname+".png");
    

  }

      
  if ( ShowHi ){
    new TCanvas;
    gPad->SetLeftMargin( lm );// for bigger labels
    gPad->SetBottomMargin( bm );// for bigger labels
    
    TH1D* dummy = (TH1D*)Hi_P8_HT54->Clone("dummy");
    dummy->Reset();    dummy->SetTitle("");
    dummy->SetAxisRange(ajmin, ajmax, "y");
    dummy->Draw();
    
    leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "AJ_hi" );
    leg->SetBorderSize(0);    leg->SetLineWidth(10);
    leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
    Hi_Geant_HT54->SetLineColor(kBlack);
    Hi_Geant_HT54->DrawCopy("9same");
    Hi_P8_HT54->SetLineColor(kRed);
    Hi_P8_HT54->DrawCopy("9same");
    Hi_PP_HT54->SetLineColor(kGreen+1);
    Hi_PP_HT54->DrawCopy("9same");
      
    leg->AddEntry(Hi_Geant_HT54,"Pythia6 #otimes GEANT", "lp");
    leg->AddEntry(Hi_P8_HT54,"Pythia8", "lp");
    leg->AddEntry(Hi_PP_HT54,"p+p HT", "lp");
    leg->Draw();
      
    // pull bin name from end of histo name
    // TString plotname = h6->GetName();
    // plotname = plotname(plotname.Length()-5, plotname.Length());
    TString plotname = "";
    plotname = plotpath+"HiJets_HT54"+plotname;
    gPad->SaveAs(plotname+".png");
    

  }

  // //    -------- MB Soft Jets  --------
  // if ( Do_SoftJets_MB ){
  //   new TCanvas;
  //   gPad->SetLeftMargin( lm );// for bigger labels
  //   gPad->SetBottomMargin( bm );// for bigger labels

  //   TH1D* h6 = P6_SoftJets_MB;
  //   TH1D* h8 = P8_SoftJets_MB;
    
  //   TH1D* dummy = (TH1D*) h6->Clone("dummy");
  //   dummy->Reset();    dummy->SetTitle("");
  //   dummy->SetAxisRange(ajmin, ajmax, "y");
  //   dummy->SetLineColor(kBlack);
  //   dummy->Draw();
            
  //   leg = new TLegend( 0.45 , 0.52, 0.89, 0.9, h6->GetTitle() );
  //   leg->SetBorderSize(0);    leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);     leg->SetMargin(0.1);
    
  //   h6->SetLineColor(kBlack);
  //   h6->SetMarkerColor(kBlack);
  //   h6->SetMarkerStyle(21);
  //   h6->DrawCopy("9same");
  //   h8->SetLineColor(kRed);
  //   h8->SetMarkerColor(kRed);
  //   h8->SetMarkerStyle(22);
  //   h8->DrawCopy("9same");
    
  //   leg->AddEntry(h6,"Pythia6, AJ_lo", "lp");       
  //   leg->AddEntry(h8,"Pythia8, AJ_lo", "lp");
  //   leg->Draw();

  //   // pull bin name from end of histo name
  //   TString plotname = h6->GetName();
  //   plotname = plotname(plotname.Length()-5, plotname.Length());
  //   plotname = plotpath+"Geant_SoftJets_MB_"+plotname;
  //   gPad->SaveAs(plotname+".png");
  // } //   if ( Do_SoftJets_ )
  
  // //    -------- HT54 Soft Jets  --------
  // if ( Do_SoftJets_HT54 ){
  //   new TCanvas;
  //   gPad->SetLeftMargin( lm );// for bigger labels
  //   gPad->SetBottomMargin( bm );// for bigger labels
    
  //   TH1D* h6 = P6_SoftJets_HT54;
  //   TH1D* h8 = P8_SoftJets_HT54;
  //   TH1D* hP = PP_SoftJets_HT54;
      
  //   TH1D* dummy = (TH1D*) h6->Clone("dummy");
  //   dummy->Reset();    dummy->SetTitle("");
  //   dummy->SetAxisRange(ajmin, ajmax, "y");
  //   dummy->Draw();
            
  //   leg = new TLegend( 0.5 , 0.5, 0.89, 0.9, h6->GetTitle() );
  //   leg->SetBorderSize(0);    leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
  //     h6->SetLineColor(kBlack);
  //     h6->SetMarkerColor(kBlack);
  //     h6->SetMarkerStyle(21);
  //     h6->DrawCopy("9same");

  //     h8->SetLineColor(kRed);
  //     h8->SetMarkerColor(kRed);
  //     h8->SetMarkerStyle(22);
  //     h8->DrawCopy("9same");
      
  //     hP->SetLineColor(kGreen+1);
  //     hP->SetMarkerColor(kGreen+1);
  //     hP->SetMarkerStyle(29);
  //     hP->DrawCopy("9same");

  //     leg->AddEntry(h6,"Pythia6, AJ_lo", "lp");
  //     leg->AddEntry(h8,"Pythia8, AJ_lo", "lp");
  //     leg->AddEntry(hP,"p+p HT, AJ_lo", "lp");
  //     leg->Draw();
      
  //     // pull bin name from end of histo name
  //     TString plotname = h6->GetName();
  //     plotname = plotname(plotname.Length()-5, plotname.Length());
  //     plotname = plotpath+"Geant_SoftJets_HT54_"+plotname;
  //     // gPad->SaveAs(plotname+".png");
  // } //   if ( Do_SoftJets_HT54  )
  
  // //    -------- Soft Jets HT54 v MB  --------
  // if ( Do_SoftJets_HT54 && Do_SoftJets_MB  ){
  //   new TCanvas;
  //   gPad->SetLeftMargin( lm );// for bigger labels
  //   gPad->SetBottomMargin( bm );// for bigger labels
    
  //   TH1D* h8MB = P8_SoftJets_MB;
  //   TH1D* h8HT = P8_SoftJets_HT54;
      
  //   TH1D* dummy = (TH1D*) h8MB->Clone("dummy");
  //   dummy->Reset();    dummy->SetTitle("");
  //   dummy->SetAxisRange(ajmin, ajmax, "y");
  //   dummy->SetLineColor(kBlack);
  //   dummy->Draw();
    
  //   leg = new TLegend( 0.45 , 0.45, 0.89, 0.9, TString("Pythia8, ")+h8MB->GetTitle() );
  //   leg->SetBorderSize(0);    leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);     leg->SetMargin(0.1);
    
  //   h8MB->SetLineColor(kRed);
  //   h8MB->SetMarkerColor(kRed);
  //   h8MB->SetMarkerStyle(22);
  //   h8MB->DrawCopy("9same");
    
  //   h8HT->SetLineColor(kBlue+1);
  //   h8HT->SetMarkerColor(kBlue+1);
  //   h8HT->SetMarkerStyle(21);
  //   h8HT->DrawCopy("9same");
    
  //   leg->AddEntry(h8MB,"Untriggered, AJ_lo", "lp");
  //   leg->AddEntry(h8HT,"HT, AJ_lo", "lp");
  //   leg->Draw();
    
  //   // pull bin name from end of histo name
  //   TString plotname = h8MB->GetName();
  //   plotname = plotname(plotname.Length()-5, plotname.Length());
  //   plotname = plotpath+"Geant_MB_v_HT54_"+plotname;
  //   gPad->SaveAs(plotname+".png");
    
  // } //   if ( Do_SoftJets_HT54 )
  
  // //    -------- Soft Trigger Jets Compilation HT54 v MB  --------
  // if ( Do_SoftJets_HT54 && Do_SoftJets_MB  ){

  //   new TCanvas;
  //   gPad->SetLeftMargin( lm );// for bigger labels
  //   gPad->SetBottomMargin( bm );// for bigger labels
    
  //   TH1D* h8MB0 = P8_SoftJets_MB;
  //   TH1D* h8HT0 = P8_SoftJets_HT54;
      
  //   TH1D* dummy = (TH1D*) h8MB0->Clone("dummy");
  //   dummy->Reset();    dummy->SetTitle("");
  //   dummy->SetAxisRange(ajmin, ajmax, "y");
  //   dummy->SetLineColor(kBlack);
  //   dummy->Draw();
    
  //   leg = new TLegend( 0.4 , 0.4, 0.89, 0.9, TString("Pythia8") );
  //   leg->SetBorderSize(0);    leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);     leg->SetMargin(0.1);
    
  //   h8MB0->SetLineColor(kOrange+8);
  //   h8MB0->SetMarkerColor(kOrange+8);
  //   h8MB0->SetMarkerStyle(22);
  //   h8MB0->DrawCopy("9same");
    
  //   h8HT0->SetLineColor(kGreen+1);
  //   h8HT0->SetMarkerColor(kGreen+1);
  //   h8HT0->SetMarkerStyle(21);
  //   h8HT0->DrawCopy("9same");
    
  //   h = h8HT0; leg->AddEntry(h,TString ("Untriggered, AJ_lo, ")+h->GetTitle(), "lp");
  //   h = h8MB0; leg->AddEntry(h,TString ("HT, AJ_lo, ")+h->GetTitle(), "lp");
  //   leg->Draw();

  //   // pull bin name from end of histo name
  //   TString plotname = plotpath+"Geant_MB_v_HT54_Triggers";
  //   gPad->SaveAs(plotname+".png");
  // } //   if ( Do_SoftJets_HT54 )

  // //    -------- HT54 Hard Jets  --------
  // if ( Do_HardJets_HT54 ){
  //   new TCanvas;
  //   gPad->SetLeftMargin( lm );// for bigger labels
  //   gPad->SetBottomMargin( bm );// for bigger labels
    
  //   TH1D* h6 = P6_HardJets_HT54;
  //   TH1D* h8 = P8_HardJets_HT54;
  //   TH1D* hP = PP_HardJets_HT54;

  //   TH1D* dummy = (TH1D*)h6->Clone("dummy");
  //   dummy->Reset();    dummy->SetTitle("");
  //   dummy->SetAxisRange(ajmin, ajmax, "y");
  //   dummy->Draw();
            
  //   leg = new TLegend( 0.55, 0.55, 0.89, 0.9, h6->GetTitle() );
  //   leg->SetBorderSize(0);    leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
  //   h6->SetLineColor(kBlack);
  //   h6->DrawCopy("9same");
  //   h8->SetLineColor(kRed);
  //   h8->DrawCopy("9same");
  //   hP->SetLineColor(kGreen+1);
  //   hP->DrawCopy("9same");
      
  //   leg->AddEntry(h6,"Pythia6, AJ_lo", "lp");
  //   leg->AddEntry(h8,"Pythia8, AJ_lo", "lp");
  //   leg->AddEntry(hP,"p+p HT, AJ_lo", "lp");

  //   leg->Draw();
      
  //   // pull bin name from end of histo name
  //   TString plotname = h6->GetName();
  //   plotname = plotname(plotname.Length()-5, plotname.Length());
  //   plotname = plotpath+"Geant_HardJets_HT54_"+plotname;
  //   gPad->SaveAs(plotname+".png");
    
  // } //   if ( Do_HardJets_HT54 )

  
  return 0;
}

// =============================================================================

void Fill ( TH1D* h2, TString FileName, TString TreeName, TString dopt, TString cond ){
  TChain* T = new TChain ( TreeName );
  T->Add( FileName );
  T->Draw( dopt , cond , "goff" );
  return;
}

// =============================================================================
void NormPrettify ( TH1D* h, TString titlebase, TString xlabel, TString ylabel,
		    int rebin, TString norm, float ymin, float ymax )
{
  
  TString name = h->GetName();
  TString title = titlebase;
  title += ";"+xlabel+";"+ylabel;
  if (rebin>1) h->Rebin(rebin);
  h->SetName(name);
  h->SetTitle( title );

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
  
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetTitleOffset(xto);
  h->GetYaxis()->SetTitleOffset(yto);
  
  return ;
}

// =============================================================================
TH1D* SetupAjHisto ( int nbinsx, float xleft, float xright,
		     TString namebase )
{
  TH1D* h = new TH1D( namebase, "", nbinsx, xleft, xright);
  return h;
}

// =============================================================================

