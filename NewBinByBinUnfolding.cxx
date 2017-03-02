#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TParameter.h>

#include <TLorentzVector.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLatex.h>
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


int NewBinByBinUnfolding( int dir=0 ){

  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.045);
  // latex.SetTextColor(kGray+3);
  latex.SetTextColor(kRed+3);

  float scale = 1.00;
  scale += dir * 0.04;
  

  bool UseAbove25 = true;
  bool UseGeantMB = false; // use HT or MB for Geant (detector level; HT makes more sense)

  // Output
  // ------
  TString OutFileName = "HThistos/BinByBinUnfolding_";

  if ( UseGeantMB ) OutFileName += "MBGeant";
  else              OutFileName += "HTGeant";
  
  if ( dir!=0 ){
    TString s = "PtScale"; s+=dir;
    OutFileName.Append ("_" +s);
  }
  OutFileName += ".root";
  TFile* fout = new TFile( OutFileName, "RECREATE");

  TString plotpath = "./HPplots/";

  // PYTHIA6 -- GEANT Input, NOT decayed
  // -----------------------------------
  TString sP6_SoftJets_MB   = "HThistos/Groom_Aj_TrueMB_NoEff_GeantMc.root";
  if (UseAbove25){
    sP6_SoftJets_MB   = "HThistos/Above25_Groom_Aj_TrueMB_NoEff_GeantMc.root";
  }    

  // GEANT Level
  // -----------
  TString sGG_SoftJets_MB   = "HThistos/Groom_Aj_TrueMB_NoEff_Geant.root";
  TString sGG_SoftJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
  if (UseAbove25){
    // sGG_SoftJets_MB   = "HThistos/Above25_Groom_Aj_TrueMB_NoEff_Geant.root";
    sGG_SoftJets_HT54 = "HThistos/Above25_Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
  }    


  // Actual pp -- no efficiency mock
  // -------------------------------
  TString sPP_SoftJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root";

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
  
  //    ----------------------------------------------
  //    -------- Pythia6:       Soft Jets MB  --------
  //    -------- GEANT OPTION:  Soft Jets MB  --------
  //    ----------------------------------------------
  TH2D* h2_NS_P6_SoftJets_MB=0;
  TH2D* h2_AS_P6_SoftJets_MB=0;
  TObjArray* AS_P6_SoftJets_MB=0;
  TObjArray* NS_P6_SoftJets_MB=0;

  TH2D* h2_NS_GG_SoftJets_MB=0;
  TH2D* h2_AS_GG_SoftJets_MB=0;
  TObjArray* AS_GG_SoftJets_MB=0;
  TObjArray* NS_GG_SoftJets_MB=0;


  // NS
  // --
  h2_NS_P6_SoftJets_MB = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_P6_SoftJets_MB");
  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 )";
  dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
  Fill ( h2_NS_P6_SoftJets_MB, sP6_SoftJets_MB, "TriggeredTree", dopt + h2_NS_P6_SoftJets_MB->GetName(), cond );
  NS_P6_SoftJets_MB = ChopNormPrettify ( h2_NS_P6_SoftJets_MB, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					 RebinZg, "ZG", zgmin, zgmax);
  
  h2_NS_GG_SoftJets_MB = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_GG_SoftJets_MB");
  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 )";
  // dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
  dopt  = Form("zgtriglo:%f*TriggerJetLo.Pt() >> ",scale); 
  Fill ( h2_NS_GG_SoftJets_MB, sGG_SoftJets_MB, "TriggeredTree", dopt + h2_NS_GG_SoftJets_MB->GetName(), cond );
  NS_GG_SoftJets_MB = ChopNormPrettify ( h2_NS_GG_SoftJets_MB, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					 RebinZg, "ZG", zgmin, zgmax);
  

  // AS
  // --
  h2_AS_P6_SoftJets_MB = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_P6_SoftJets_MB");
  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 )";
  dopt  = "zgawaylo:AwayJetLo.Pt() >> "; 
  Fill ( h2_AS_P6_SoftJets_MB, sP6_SoftJets_MB, "TriggeredTree", dopt + h2_AS_P6_SoftJets_MB->GetName(), cond );
  AS_P6_SoftJets_MB = ChopNormPrettify ( h2_AS_P6_SoftJets_MB, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					 RebinZg, "ZG", zgmin, zgmax);
  
  h2_AS_GG_SoftJets_MB = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_GG_SoftJets_MB");
  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 )";
  // dopt  = "zgawaylo:AwayJetLo.Pt() >> "; 
  dopt  = Form("zgawaylo:%f*AwayJetLo.Pt() >> ",scale); 
  Fill ( h2_AS_GG_SoftJets_MB, sGG_SoftJets_MB, "TriggeredTree", dopt + h2_AS_GG_SoftJets_MB->GetName(), cond );
  AS_GG_SoftJets_MB = ChopNormPrettify ( h2_AS_GG_SoftJets_MB, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					 RebinZg, "ZG", zgmin, zgmax);

  //    -----------------------------------------------
  //    -------- pp and Geant: HT54 Soft Jets  --------
  //    -----------------------------------------------
  TH2D* h2_NS_GG_SoftJets_HT54=0;
  TH2D* h2_NS_PP_SoftJets_HT54=0;

  TObjArray* NS_GG_SoftJets_HT54=0;
  TObjArray* NS_PP_SoftJets_HT54=0;

  TH2D* h2_AS_GG_SoftJets_HT54=0;
  TH2D* h2_AS_PP_SoftJets_HT54=0;

  TObjArray* AS_GG_SoftJets_HT54=0;
  TObjArray* AS_PP_SoftJets_HT54=0;

  // NS
  // --
  h2_NS_GG_SoftJets_HT54 = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_GG_SoftJets_HT54");
  h2_NS_PP_SoftJets_HT54 = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_PP_SoftJets_HT54");

  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 )";
  // dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
  dopt  = Form("zgtriglo:%f*TriggerJetLo.Pt() >> ",scale); 
  Fill ( h2_NS_GG_SoftJets_HT54, sGG_SoftJets_HT54, "TriggeredTree", dopt + h2_NS_GG_SoftJets_HT54->GetName(), cond );
  NS_GG_SoftJets_HT54 = ChopNormPrettify ( h2_NS_GG_SoftJets_HT54, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   RebinZg, "ZG", zgmin, zgmax);

  dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
  Fill ( h2_NS_PP_SoftJets_HT54, sPP_SoftJets_HT54, "TriggeredTree", dopt + h2_NS_PP_SoftJets_HT54->GetName(), cond );
  NS_PP_SoftJets_HT54 = ChopNormPrettify ( h2_NS_PP_SoftJets_HT54, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   RebinZg, "ZG", zgmin, zgmax);
  
  // AS
  // --
  h2_AS_GG_SoftJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_GG_SoftJets_HT54");
  h2_AS_PP_SoftJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_PP_SoftJets_HT54");

  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 )";
  // dopt  = "zgawaylo:AwayJetLo.Pt() >> "; 
  dopt  = Form("zgawaylo:%f*AwayJetLo.Pt() >> ",scale); 
  Fill ( h2_AS_GG_SoftJets_HT54, sGG_SoftJets_HT54, "TriggeredTree", dopt + h2_AS_GG_SoftJets_HT54->GetName(), cond );
  AS_GG_SoftJets_HT54 = ChopNormPrettify ( h2_AS_GG_SoftJets_HT54, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   RebinZg, "ZG", zgmin, zgmax);
    
  
  dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
  Fill ( h2_AS_PP_SoftJets_HT54, sPP_SoftJets_HT54, "TriggeredTree", dopt + h2_AS_PP_SoftJets_HT54->GetName(), cond );
  AS_PP_SoftJets_HT54 = ChopNormPrettify ( h2_AS_PP_SoftJets_HT54, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   RebinZg, "ZG", zgmin, zgmax);

  
  // ==========================================================================================
  // --------------------------------------
  // ---------- Unfolding histos ----------
  // --------------------------------------
  TString s;
  
  TObjArray* NS_Geant=NS_GG_SoftJets_HT54;
  TObjArray* AS_Geant=AS_GG_SoftJets_HT54;

  TObjArray* NS_ratio;
  TObjArray* AS_ratio;

  if ( UseGeantMB ){
    NS_Geant=NS_GG_SoftJets_MB;
    AS_Geant=AS_GG_SoftJets_MB;
  }
  
  
  TObjArray PpUnfoldedNS;
  TObjArray PpUnfoldedAS;

  for (int i=0; i<NS_Geant->GetEntries(); ++i ){
    TH1D* hrecoNS  = (TH1D*)NS_Geant->At(i);
    TH1D* htruthNS = (TH1D*)NS_P6_SoftJets_MB->At(i);

    TH1D* hrecoAS  = (TH1D*)AS_Geant->At(i);
    TH1D* htruthAS = (TH1D*)AS_P6_SoftJets_MB->At(i);

    
    TString name = hrecoNS->GetName();
    name = name(name.Length()-5, name.Length());    
    // cout << hratioNS->GetName() << endl; continue;

    TH1D* hratioNS = (TH1D*)hrecoNS->Clone( "ratioNS_"+name );
    hratioNS->GetYaxis()->SetTitle("reco/truth");
    hratioNS->Divide(htruthNS);

    TH1D* hratioAS = (TH1D*)hrecoAS->Clone( "ratioAS_"+name );
    hratioAS->GetYaxis()->SetTitle("reco/truth");
    hratioAS->Divide(htruthAS);

    TH1D* hPpNS = (TH1D*)NS_PP_SoftJets_HT54->At(i);
    TH1D* hPpUnfoldedNS = (TH1D*)hPpNS->Clone( "UnfoldedNS_"+name );
    hPpUnfoldedNS->Divide(hratioNS);
    // Renormalize
    h = hPpUnfoldedNS; h->Scale ( 1./ h->Integral(1, h->GetNbinsX()) / h->GetXaxis()->GetBinWidth(1));
    PpUnfoldedNS.Add(hPpUnfoldedNS);

    TH1D* hPpAS = (TH1D*)AS_PP_SoftJets_HT54->At(i);
    TH1D* hPpUnfoldedAS = (TH1D*)hPpAS->Clone( "UnfoldedAS_"+name );
    // hratioAS->SaveAs(TString(hratioAS->GetName())+".root");
    // hPpAS->SaveAs(TString(hPpAS->GetName())+".root");
    // hPpUnfoldedAS->SaveAs(TString(hPpUnfoldedAS->GetName())+".root");
    hPpUnfoldedAS->Divide(hratioAS);
    // Renormalize
    h = hPpUnfoldedNS; h->Scale ( 1./ h->Integral(1, h->GetNbinsX()) / h->GetXaxis()->GetBinWidth(1));
    PpUnfoldedAS.Add(hPpUnfoldedAS);

    // Draw Ratios
    new TCanvas;
    TH1D* dummy = (TH1D*)hratioNS->Clone("dummy");
    dummy->Reset();    dummy->SetTitle("");
    dummy->SetLineColor(kBlack);
    dummy->SetAxisRange(0, 3, "y");
    dummy->Draw();

    if ( UseGeantMB ) leg = new TLegend( 0.45 , 0.7, 0.89, 0.9, TString("MB GEANT to MB truth, ") + hratioNS->GetTitle() );
    else              leg = new TLegend( 0.45 , 0.7, 0.89, 0.9, TString("HT GEANT to MB truth, ") + hratioNS->GetTitle() );

    hratioNS->SetLineColor(kGreen+1);
    hratioNS->Draw("9same");
    hratioAS->SetLineColor(kBlue);
    hratioAS->Draw("9same");

    leg->SetBorderSize(0);    leg->SetLineWidth(10);
    leg->SetFillStyle(0);     leg->SetMargin(0.1);
    leg->AddEntry(hratioNS,"Correction factor, Trigger Jet", "lp");
    leg->AddEntry(hratioAS,"Correction factor, Trigger Jet", "lp");
    leg->Draw("same");

    TString plotname = hPpUnfoldedNS->GetName();    
    plotname = plotname(plotname.Length()-5, plotname.Length());
    if ( dir!=0 ){
      TString s = "PtScale"; s+=dir;
      plotname.Prepend (s+"_");
    }

    if ( UseGeantMB ) plotname = plotpath+"BinByBinRatio_MBGeant_"+plotname;
    else              plotname = plotpath+"BinByBinRatio_HTGeant_"+plotname;
    gPad->SaveAs( plotname+".png");

    // hPpUnfoldedNS->Draw("9");
    // hPpNS->SetLineColor(kRed);
    // hPpNS->Draw("9same");
    // htruthNS->SetLineColor(kGreen);
    // htruthNS->Draw("9same");

    // hPpUnfoldedNS->Draw("9");
    // hPpNS->SetLineColor(kRed);
    // hPpNS->Draw("9same");
    // htruthNS->SetLineColor(kGreen);
    // htruthNS->Draw("9same");

    // hPpUnfoldedAS->Draw("9same");
    // hPpAS->SetLineColor(kRed);
    // hPpAS->Draw("9same");
    // htruthAS->SetLineColor(kGreen);
    // htruthAS->Draw("9same");


  }

  // ==========================================================================================
  // -------------------------
  // --------- Draw ----------
  // -------------------------
  for (int i=0; i<PpUnfoldedNS.GetEntries(); ++i ){
    TH1D* hPpUnfoldedNS = (TH1D*)PpUnfoldedNS.At(i);
    TH1D* hPpUnfoldedAS = (TH1D*)PpUnfoldedAS.At(i);

    new TCanvas;
    
    TH1D* dummy = (TH1D*)hPpUnfoldedNS->Clone("dummy");
    dummy->Reset();    dummy->SetTitle("");
    dummy->SetLineColor(kBlack);
    dummy->SetAxisRange(zgmin, zgmax, "y");
    dummy->Draw();
            
    // leg = new TLegend( 0.55, 0.55, 0.89, 0.9, hGNS->GetTitle() );
    leg = new TLegend( 0.45 , 0.6, 0.89, 0.9, TString ("Corrected, ")+hPpUnfoldedNS->GetTitle() );
    leg->SetBorderSize(0);    leg->SetLineWidth(10);
    leg->SetFillStyle(0);     leg->SetMargin(0.1);
    
    // hGNS->SetLineColor(kGreen+1);
    // hGNS->SetMarkerColor(kGreen+1);
    // hGNS->SetMarkerStyle(21);
    // hGNS->DrawCopy("9same");
    
    hPpUnfoldedNS->SetLineColor(kBlack);
    hPpUnfoldedNS->SetMarkerColor(kBlack);
    hPpUnfoldedNS->SetMarkerStyle(30);
    hPpUnfoldedNS->DrawCopy("9same");
    
    // hGAS->SetLineColor(kBlue);
    // hGAS->SetMarkerColor(kBlue);
    // hGAS->SetMarkerStyle(25);
    // hGAS->DrawCopy("9same");
    
    hPpUnfoldedAS->SetLineColor(kGray+1);
    hPpUnfoldedAS->SetMarkerColor(kGray+1);
    hPpUnfoldedAS->SetMarkerStyle(30);
    hPpUnfoldedAS->DrawCopy("9same");
    
    // leg->AddEntry(hGNS,"Pythia6@GEANT, Trigger Jet", "lp");
    leg->AddEntry(hPpUnfoldedNS,"p+p HT, Trigger Jet", "lp");
    // leg->AddEntry(hGAS,"Pythia6@GEANT, Away Jet", "lp");
    leg->AddEntry(hPpUnfoldedAS,"p+p HT, Away Jet", "lp");


    FUVQjet->Draw("9same");
    leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
    leg->Draw();
    
    latex.DrawLatex( 0.55, 0.54, "STAR Preliminary");
    
       
    // pull bin name from end of histo name
    TString plotname = hPpUnfoldedNS->GetName();
    plotname = plotname(plotname.Length()-5, plotname.Length());
    if ( dir!=0 ){
      TString s = "PtScale"; s+=dir;
      plotname.Prepend (s+"_");
    }
    plotname = plotpath+"UnfoldedPp_"+plotname;
    gPad->SaveAs(plotname+".png");



  }




  fout->Write();

  return 0;
  // for (int i=0; i<AS_GG_SoftJets_HT54->GetEntries(); ++i ){
  //   new TCanvas;
  //   TH1D* hGNS = (TH1D*)NS_GG_SoftJets_HT54->At(i);
  //   TH1D* hPNS = (TH1D*)NS_PP_SoftJets_HT54->At(i);
  //   TH1D* hGAS = (TH1D*)AS_GG_SoftJets_HT54->At(i);
  //   TH1D* hPAS = (TH1D*)AS_PP_SoftJets_HT54->At(i);
    
  //   TH1D* dummy = (TH1D*)hGNS->Clone("dummy");
  //   dummy->Reset();    dummy->SetTitle("");
  //   dummy->SetAxisRange(zgmin, zgmax, "y");
  //   dummy->Draw();
    
  //   // leg = new TLegend( 0.55, 0.55, 0.89, 0.9, hGNS->GetTitle() );
  //   leg = new TLegend( 0.5 , 0.5, 0.89, 0.9, hGNS->GetTitle() );
  //   leg->SetBorderSize(0);    leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);     leg->SetMargin(0.1);
    
  //   hGNS->SetLineColor(kBlack);
  //   hGNS->SetMarkerColor(kBlack);
  //   hGNS->SetMarkerStyle(21);
  //   hGNS->DrawCopy("9same");
    
  //   hPNS->SetLineColor(kRed);
  //   hPNS->SetMarkerColor(kRed);
  //   hPNS->SetMarkerStyle(22);
  //   hPNS->DrawCopy("9same");
    
  //   // hGAS->SetLineColor(kGray+1);
  //   // hGAS->SetMarkerColor(kGray+1);
  //   // hGAS->SetMarkerStyle(23);
  //   // hGAS->DrawCopy("9same");
    
  //   hGAS->SetLineColor(kMagenta+1);
  //   hGAS->SetMarkerColor(kMagenta+1);
  //   hGAS->SetMarkerStyle(26);
  //   hGAS->DrawCopy("9same");
    
  //   hPAS->SetLineColor(kBlue);
  //   hPAS->SetMarkerColor(kBlue);
  //   hPAS->SetMarkerStyle(29);
  //   hPAS->DrawCopy("9same");
    
  //   leg->AddEntry(hGNS,"Pythia6@GEANT, Trigger Jet", "lp");
  //   leg->AddEntry(hPNS,"p+p HT, Trigger Jet", "lp");
  //   leg->AddEntry(hGAS,"Pythia6@GEANT, Away Jet", "lp");
  //   leg->AddEntry(hPAS,"p+p HT, Away Jet", "lp");
    
    
  //   FUVQjet->Draw("9same");
  //   leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  //   leg->Draw();
    
  //   // pull bin name from end of histo name
  //   TString plotname = hGNS->GetName();
  //   plotname = plotname(plotname.Length()-5, plotname.Length());
  //   plotname = plotpath+"PpVGeant_SoftJets_HT54_"+plotname;
  //   if ( dir!=0 ){
  //     TString s = "PtScale"; s+=dir;
  //     plotname.Prepend (s+"_");
  //   }
  //   gPad->SaveAs(plotname+".png");
    
  // }

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
