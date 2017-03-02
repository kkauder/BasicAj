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

// canvas stuff
float lm = 0.11;
float bm = 0.11;
float yto = 0.5;
float xto = 0.5;

int PpGeantOverlays(){

  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  bool UseAbove25 = true;

  bool Do_NS_SoftJets_HT54=true;
  bool Do_AS_SoftJets_HT54=true;
  // // bool Do_NS_SoftJets_HT54=false;
  // // bool Do_AS_SoftJets_HT54=false;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.045);
  // latex.SetTextColor(kGray+3);
  latex.SetTextColor(kRed+3);



  TString plotpath = "./HPplots/";

  // GEANT Level
  // -----------
  // TString sGG_SoftJets_HT54 = "AjResults/AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root"; // DEBUG    
  TString sGG_SoftJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
  TString sGG_HardJets_HT54 = "HThistos/Groom_Aj_TrueMB_NoEff_Geant.root";
  if (UseAbove25){
    // sGG_SoftJets_MB   = "HThistos/Above25_Groom_Aj_TrueMB_NoEff_Geant.root";
    sGG_SoftJets_HT54 = "HThistos/Above25_Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
    sGG_HardJets_HT54 = "HThistos/Above25_Groom_Aj_TrueMB_NoEff_Geant.root";
  }    


  // Actual pp -- no efficiency mock
  // -------------------------------
  TString sPP_SoftJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root";
  TString sPP_HardJets_HT54 = "HThistos/Groom_Aj_HT54_HTled_NoEff_ppAj.root";

  // Settings and helpers
  // --------------------
  int RebinZg=4;
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
  //    -------- HT54 Soft Jets  --------
  //    -------------------------------
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
  dopt  = "zgtriglo:TriggerJetLo.Pt() >> "; 
  Fill ( h2_NS_GG_SoftJets_HT54, sGG_SoftJets_HT54, "TriggeredTree", dopt + h2_NS_GG_SoftJets_HT54->GetName(), cond );
  NS_GG_SoftJets_HT54 = ChopNormPrettify ( h2_NS_GG_SoftJets_HT54, "p_{T}^{det}=", "z_{g}", "1/N dN/dz_{g}",
					   RebinZg, "ZG", zgmin, zgmax);
    
  Fill ( h2_NS_PP_SoftJets_HT54, sPP_SoftJets_HT54, "TriggeredTree", dopt + h2_NS_PP_SoftJets_HT54->GetName(), cond );
  NS_PP_SoftJets_HT54 = ChopNormPrettify ( h2_NS_PP_SoftJets_HT54, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   RebinZg, "ZG", zgmin, zgmax);
  
  // AS
  // --
  h2_AS_GG_SoftJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_GG_SoftJets_HT54");
  h2_AS_PP_SoftJets_HT54 = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_PP_SoftJets_HT54");

  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 )";
  dopt  = "zgawaylo:AwayJetLo.Pt() >> "; 
  Fill ( h2_AS_GG_SoftJets_HT54, sGG_SoftJets_HT54, "TriggeredTree", dopt + h2_AS_GG_SoftJets_HT54->GetName(), cond );
  AS_GG_SoftJets_HT54 = ChopNormPrettify ( h2_AS_GG_SoftJets_HT54, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   RebinZg, "ZG", zgmin, zgmax);
    
  
  dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
  Fill ( h2_AS_PP_SoftJets_HT54, sPP_SoftJets_HT54, "TriggeredTree", dopt + h2_AS_PP_SoftJets_HT54->GetName(), cond );
  AS_PP_SoftJets_HT54 = ChopNormPrettify ( h2_AS_PP_SoftJets_HT54, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   RebinZg, "ZG", zgmin, zgmax);


  // ==========================================================================================
  // -------------------------
  // --------- Draw ----------
  // -------------------------
  if ( false && Do_NS_SoftJets_HT54 && Do_AS_SoftJets_HT54 ){    
    for (int i=0; i<AS_GG_SoftJets_HT54->GetEntries(); ++i ){
      new TCanvas;
      gPad->SetLeftMargin( lm );// for bigger labels
      gPad->SetBottomMargin( bm );// for bigger labels

      TH1D* hGNS = (TH1D*)NS_GG_SoftJets_HT54->At(i);
      TH1D* hPNS = (TH1D*)NS_PP_SoftJets_HT54->At(i);
      TH1D* hGAS = (TH1D*)AS_GG_SoftJets_HT54->At(i);
      TH1D* hPAS = (TH1D*)AS_PP_SoftJets_HT54->At(i);
      
      TH1D* dummy = (TH1D*)hGNS->Clone("dummy");
      dummy->Reset();    dummy->SetTitle("");
      dummy->SetAxisRange(zgmin, zgmax, "y");
      dummy->Draw();
            
      if (i<4)    leg = new TLegend( 0.4 , 0.5, 0.89, 0.9, hGNS->GetTitle() );
      else     leg = new TLegend( 0.4 , 0.6, 0.89, 0.9, hGNS->GetTitle() );
      leg->SetBorderSize(0);    leg->SetLineWidth(10);
      leg->SetFillStyle(0);     leg->SetMargin(0.1);
      
      hGNS->SetLineColor(kMagenta+3);
      hGNS->SetMarkerColor(kMagenta+3);
      hGNS->SetMarkerStyle(21);
      hGNS->DrawCopy("9same");

      hPNS->SetLineColor(kRed);
      hPNS->SetMarkerColor(kRed);
      hPNS->SetMarkerStyle(29);
      hPNS->SetMarkerSize(2);
      hPNS->DrawCopy("9same");
      
      if (i<4){
	hGAS->SetLineColor(kGreen+2);
	hGAS->SetMarkerColor(kGreen+2);
	hGAS->SetMarkerStyle(25);
	hGAS->DrawCopy("9same");
	
	hPAS->SetLineColor(kBlack);
	hPAS->SetMarkerColor(kBlack);
	hPAS->SetMarkerSize(2);
	hPAS->SetMarkerStyle(30);
	hPAS->DrawCopy("9same");
      }

      leg->AddEntry(hPNS,"p+p HT, Trigger Jet", "lp");
      leg->AddEntry(hGNS,"Pythia6 #oplus GEANT, HT Trigger Jet", "lp");
      if (i<4){
	leg->AddEntry(hPAS,"p+p HT, Recoil Jet", "lp");
	leg->AddEntry(hGAS,"Pythia6 #oplus GEANT, Recoil Jet", "lp");
      }


      // FUVQjet->Draw("9same");
      // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
      leg->Draw();

      latex.DrawLatex( 0.55, 0.4, "STAR Preliminary");

  
      
      // pull bin name from end of histo name
      TString plotname = hGNS->GetName();
      plotname = plotname(plotname.Length()-5, plotname.Length());
      plotname = plotpath+"PpVGeant_SoftJets_HT54_"+plotname;
      gPad->SaveAs(plotname+".png");
      gPad->SaveAs(plotname+".pdf");
      
    }
  } //   if ( Do_NS_SoftJets_HT54 && Do_AS_SoftJets_HT54 )

  // Draw Separately
  // ---------------
  TString s;
  if ( Do_NS_SoftJets_HT54 && Do_AS_SoftJets_HT54 ){    
    for (int i=0; i<AS_GG_SoftJets_HT54->GetEntries(); ++i ){

      s="cNS"; s+=i;
      TCanvas* cNS = new TCanvas(s);
      gPad->SetLeftMargin( lm );// for bigger labels
      gPad->SetBottomMargin( bm );// for bigger labels
      cNS->cd();

      TH1D* hGNS = (TH1D*)NS_GG_SoftJets_HT54->At(i);
      TH1D* hPNS = (TH1D*)NS_PP_SoftJets_HT54->At(i);
      TH1D* hGAS = (TH1D*)AS_GG_SoftJets_HT54->At(i);
      TH1D* hPAS = (TH1D*)AS_PP_SoftJets_HT54->At(i);
      
      TH1D* dummy = (TH1D*)hGNS->Clone("dummy");
      dummy->Reset();    dummy->SetTitle("");
      dummy->SetAxisRange(zgmin, zgmax, "y");
      dummy->DrawCopy();
            
      legNS = new TLegend( 0.4 , 0.6, 0.89, 0.9, hGNS->GetTitle() );
      legNS->SetBorderSize(0);    legNS->SetLineWidth(10);
      legNS->SetFillStyle(0);     legNS->SetMargin(0.1);
      
      hGNS->SetLineColor(kMagenta+3);
      hGNS->SetMarkerColor(kMagenta+3);
      hGNS->SetMarkerStyle(21);
      hGNS->DrawCopy("9same");

      hPNS->SetLineColor(kRed);
      hPNS->SetMarkerColor(kRed);
      hPNS->SetMarkerStyle(29);
      hPNS->SetMarkerSize(2);
      hPNS->DrawCopy("9same");

      legNS->AddEntry(hPNS,"p+p HT, Trigger Jet", "lp");
      legNS->AddEntry(hGNS,"Pythia6 #oplus GEANT, HT Trigger Jet", "lp");
      legNS->Draw();
      latex.DrawLatex( 0.55, 0.4, "STAR Preliminary");
      
      // pull bin name from end of histo name
      TString plotname = hGNS->GetName();
      plotname = plotname(plotname.Length()-5, plotname.Length());
      plotname = plotpath+"PpVGeant_NS_SoftJets_HT54_"+plotname;
      gPad->SaveAs(plotname+".png");
      gPad->SaveAs(plotname+".pdf");
      return 0;

      if (i<4){
	s="cAS"; s+=i;
	TCanvas* cAS = new TCanvas(s);
	gPad->SetLeftMargin( lm );// for bigger labels
	gPad->SetBottomMargin( bm );// for bigger labels
	cAS->cd();
	legAS = new TLegend( 0.4 , 0.6, 0.89, 0.9, hGAS->GetTitle() );
	legAS->SetBorderSize(0);    legAS->SetLineWidth(10);
	legAS->SetFillStyle(0);     legAS->SetMargin(0.1);

	dummy->DrawCopy();
	hGAS->SetLineColor(kGreen+2);
	hGAS->SetMarkerColor(kGreen+2);
	hGAS->SetMarkerStyle(25);
	hGAS->DrawCopy("9same");
	
	hPAS->SetLineColor(kBlack);
	hPAS->SetMarkerColor(kBlack);
	hPAS->SetMarkerSize(2);
	hPAS->SetMarkerStyle(30);
	hPAS->DrawCopy("9same");

	legAS->AddEntry(hPAS,"p+p HT, Recoil Jet", "lp");
	legAS->AddEntry(hGAS,"Pythia6 #oplus GEANT, Recoil Jet", "lp");
	legAS->Draw();
	latex.DrawLatex( 0.55, 0.4, "STAR Preliminary");
      
	// pull bin name from end of histo name
	TString plotname = hGNS->GetName();
	plotname = plotname(plotname.Length()-5, plotname.Length());
	plotname = plotpath+"PpVGeant_AS_SoftJets_HT54_"+plotname;
	gPad->SaveAs(plotname+".png");
	gPad->SaveAs(plotname+".pdf");

      }
      
    }
  } //   if ( Do_NS_SoftJets_HT54 && Do_AS_SoftJets_HT54 )


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
