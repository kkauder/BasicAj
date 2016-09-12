
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldTUnfold.h"

#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TLegend.h>



int DoUnfolding( int nBayes2D=3,
		 TString trainname = "HThistos/TrainedWith_Groom_Aj_HT54_HTled_TrueMB_NoEff_GeantMc.root",
		 // TString trainname = "HThistos/TrainedWith_Groom_Aj_TrueMB_NoEff_GeantMc.root",
		 TString ppname = "HThistos/ForUnfolding_Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root",
		 int nBayes1D=3
		) {
  
  int RebinZg=2;  // SHOULD BE DONE EARLIER

  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);

  TFile* ftrain = new TFile( trainname, "READ");
  // TString outbase = gSystem->BaseName( ftrain->GetName() );
  // outbase.ReplaceAll( ".root", "");
  // outbase.Prepend("Plots/");

  // Load pp data
  // ------------
  TFile* fmeas = new TFile( ppname, "READ");  
  TH2D* TestMeas2D  = (TH2D*) fmeas->Get("TestMeas2D");

  // Load response
  // ------------
  TH2D* TrainMeas2D  = (TH2D*) ftrain->Get("Meas2D");
  TH2D* TrainTruth2D  = (TH2D*) ftrain->Get("Truth2D");

  RooUnfoldResponse* LeadPtResponse = (RooUnfoldResponse*) ftrain->Get("LeadPtResponse");
  TH1D* PtTestMeas  = (TH1D*) TestMeas2D->ProjectionX("PtTestMeas");
  RooUnfoldBayes    PtBayesUnfold ( LeadPtResponse, PtTestMeas, nBayes1D);
  TH1D* PtBayesUnfolded= (TH1D*) PtBayesUnfold.Hreco();
  PtBayesUnfolded->SetName("PtBayesUnfolded");

  
  RooUnfoldResponse* LeadPtZgResponse2D = (RooUnfoldResponse*) ftrain->Get("LeadPtZgResponse2D");
  RooUnfoldBayes    BayesUnfold ( LeadPtZgResponse2D, TestMeas2D, nBayes2D);
  BayesUnfold.SetVerbose(1);
  TH2D* BayesUnfolded= (TH2D*) BayesUnfold.Hreco();


  // Output
  // ------
  TString OutFileName = "HThistos/Unfolded_Smooth";
  OutFileName += nBayes2D;   OutFileName += "_";
  OutFileName += gSystem->BaseName(trainname);
  TFile* fout = new TFile( OutFileName, "RECREATE");

  TString PlotBase = "HTplots/Unfolded_Smooth";
  PlotBase += nBayes2D;   PlotBase += "_";
  PlotBase += gSystem->BaseName(trainname);
  PlotBase.ReplaceAll(".root","");



  // =========================== Draw Spectra ===============================
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  TLegend* leg = new TLegend( 0.55, 0.6, 0.89, 0.9, "Leading Jet p_{T}" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  // TH1D* truthpt = (TH1D*) TestTruth2D->ProjectionX("truthpt");
  // truthpt->SetLineColor(kRed);
  // truthpt->SetTitle( ";p_{T};arb. units" );
  // truthpt->SetMinimum( 1E-5 );
  // truthpt->Draw();
  // leg->AddEntry("truthpt", "Truth");
  
    
  TH1D* measpt = (TH1D*) TestMeas2D->ProjectionX("measpt");
  measpt->SetLineColor(kBlue);
  measpt->Draw("same");
  leg->AddEntry("measpt", "Measured");
    
  TH1D* unfoldpt = (TH1D*) BayesUnfolded->ProjectionX("unfoldpt");
  unfoldpt->SetLineColor(kGreen+1);
  unfoldpt->Draw("same");
  leg->AddEntry("unfoldpt", "Unfolded");

  TH1D* TrainTruthpt = (TH1D*) TrainTruth2D->ProjectionX("TrainTruthpt");
  TrainTruthpt->SetLineColor(kPink+7);
  TrainTruthpt->Draw("same");
  leg->AddEntry("TrainTruthpt", "Training Truth");

  TH1D* TrainMeaspt = (TH1D*) TrainMeas2D->ProjectionX("TrainMeaspt");
  TrainMeaspt->SetLineColor(kCyan+2);
  TrainMeaspt->Draw("same");
  leg->AddEntry("TrainMeaspt", "Training Meas.");

  PtBayesUnfolded->SetLineColor(kGray+1);
  PtBayesUnfolded->Draw("same");
  leg->AddEntry("PtBayesUnfolded", "1D Unfolded");    

  leg->Draw();
  gPad->SaveAs( PlotBase + "_UnfoldedSpectra.png");  
    
  // =========================== Draw Zg ===============================
  TH1D* h;
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);

  leg = new TLegend( 0.55, 0.65, 0.89, 0.9, "Leading Jet z_{g} NORMALIZED" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
    
  // TH1D* truth0 = (TH1D*) TestTruth2D->ProjectionY("truth0", TestTruth2D->GetXaxis()->FindBin( 10+0.01), TestTruth2D->GetXaxis()->FindBin( 20-0.01));
  // truth0->SetLineColor(kGray+1);
  // truth0->SetTitle( ";z_{g};arb. units" );
  // truth0->Scale (1./truth0->Integral());
  // truth0->SetAxisRange(0, 0.18, "y");
  // truth0->Draw();
  // leg->AddEntry("truth0", "Truth, 10 < p_{T} < 10");

  TH1D* truth0 = (TH1D*) TrainTruth2D->ProjectionY("truth0", TrainTruth2D->GetXaxis()->FindBin( 10+0.01), TrainTruth2D->GetXaxis()->FindBin( 20-0.01));
  truth0->SetLineColor(kGray+1);
  truth0->SetTitle( ";z_{g};arb. units" );
  // truth0->Scale (1./truth0->Integral());
  h=truth0; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );

  h->SetAxisRange(0, 9, "y");
  h->Draw();

  leg->AddEntry("truth0", "TrainTruth, 10 < p_{T} < 20");

  TH1D* meas0 = (TH1D*) TestMeas2D->ProjectionY("meas0", TestMeas2D->GetXaxis()->FindBin( 10+0.01), TestMeas2D->GetXaxis()->FindBin( 20-0.01));
  meas0->SetLineColor(kRed+1);
  h=meas0; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  h->SetAxisRange(0, 9, "y");
  h->Draw("same");
  leg->AddEntry("meas0", "Measured, 10 < p_{T}^{Meas} < 20");

  TH1D* unfold0 = (TH1D*) BayesUnfolded->ProjectionY("unfold0", BayesUnfolded->GetXaxis()->FindBin( 10+0.01), BayesUnfolded->GetXaxis()->FindBin( 20-0.01));
  unfold0->SetLineColor(kBlack);
  h=unfold0; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  h->SetAxisRange(0, 9, "y");
  h->Draw("same");
  leg->AddEntry("unfold0", "Unfolded, 10 < p_{T} < 20");
  leg->Draw();
  gPad->SaveAs( PlotBase + "_UnfoldedZg1020.png");  

  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);

  leg = new TLegend( 0.55, 0.65, 0.89, 0.9, "Leading Jet z_{g} NORMALIZED" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  // TH1D* truth1 = (TH1D*) TestTruth2D->ProjectionY("truth1", TestTruth2D->GetXaxis()->FindBin( 20+0.01), TestTruth2D->GetXaxis()->FindBin( 30-0.01));
  // truth1->SetLineColor(kRed);
  // truth1->SetTitle( ";z_{g};arb. units" );
  // truth1->DrawNormalized("same");
  // leg->AddEntry("truth1", "Truth, 20 < p_{T} < 30");

  TH1D* truth1 = (TH1D*) TrainTruth2D->ProjectionY("truth1", TrainTruth2D->GetXaxis()->FindBin( 20+0.01), TrainTruth2D->GetXaxis()->FindBin( 30-0.01));
  truth1->SetLineColor(kGray+1);
  truth1->SetTitle( ";z_{g};arb. units" );
  h=truth1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  h->SetAxisRange(0, 9, "y");
  h->Draw("");
  leg->AddEntry("truth1", "Train Truth, 20 < p_{T} < 30");

  TH1D* meas1 = (TH1D*) TestMeas2D->ProjectionY("meas1", TestMeas2D->GetXaxis()->FindBin( 20+0.01), TestMeas2D->GetXaxis()->FindBin( 30-0.01));
  meas1->SetLineColor(kRed);
  h=meas1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  h->SetAxisRange(0, 9, "y");
  h->Draw("same");
  leg->AddEntry("meas1", "Measured, 20 < p_{T}^{Meas} < 30");

  TH1D* unfold1 = (TH1D*) BayesUnfolded->ProjectionY("unfold1", BayesUnfolded->GetXaxis()->FindBin( 20+0.01), BayesUnfolded->GetXaxis()->FindBin( 30-0.01));
  unfold1->SetLineColor(kBlack);
  h=unfold1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  h->SetAxisRange(0, 9, "y");
  h->Draw("same");

  leg->AddEntry("unfold1", "Unfolded, 20 < p_{T} < 30");
  leg->Draw();
  gPad->SaveAs( PlotBase + "_UnfoldedZg2030.png");  

  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);

  leg = new TLegend( 0.55, 0.65, 0.89, 0.9, "Leading Jet z_{g} NORMALIZED" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  // TH1D* truth2 = (TH1D*) TestTruth2D->ProjectionY("truth2", TestTruth2D->GetXaxis()->FindBin( 30+0.01), TestTruth2D->GetXaxis()->FindBin( 40-0.01));
  // truth2->SetLineColor(kMagenta);
  // truth2->DrawNormalized("same");
  // leg->AddEntry("truth2", "Truth, 30 < p_{T} < 40");

  TH1D* truth2 = (TH1D*) TrainTruth2D->ProjectionY("truth2", TrainTruth2D->GetXaxis()->FindBin( 30+0.01), TrainTruth2D->GetXaxis()->FindBin( 40-0.01));
  truth2->SetLineColor(kGray+1);
  h=truth2; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->SetTitle("");
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  h->SetAxisRange(0, 9, "y");
  h->Draw("");
  leg->AddEntry("truth2", "Train Truth, 30 < p_{T} < 40");

  TH1D* meas2 = (TH1D*) TestMeas2D->ProjectionY("meas2", TestMeas2D->GetXaxis()->FindBin( 30+0.01), TestMeas2D->GetXaxis()->FindBin( 40-0.01));
  meas2->SetLineColor(kRed);
  h=meas2; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  h->SetAxisRange(0, 9, "y");
  h->Draw("same");

  leg->AddEntry("meas2", "Measured, 30 < p_{T}^{Meas} < 40");

  TH1D* unfold2 = (TH1D*) BayesUnfolded->ProjectionY("unfold2", BayesUnfolded->GetXaxis()->FindBin( 30+0.01), BayesUnfolded->GetXaxis()->FindBin( 40-0.01));
  unfold2->SetLineColor(kBlack);
  h=unfold2; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  h->SetAxisRange(0, 9, "y");
  h->Draw("same");


  leg->AddEntry("unfold2", "Unfolded, 30 < p_{T} < 40");
  leg->Draw();
  
  gPad->SaveAs( PlotBase + "_UnfoldedZg3040.png");  

  if ( false ){
    // =========================== Draw unnormalized Zg ===============================
    new TCanvas;
    gPad->SetGridx(0);  gPad->SetGridy(0);
    
    leg = new TLegend( 0.55, 0.65, 0.89, 0.9, "Leading Jet z_{g}" );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    leg->SetMargin(0.1);
    
    truth1->Draw();
    leg->AddEntry("truth1", "Truth, 20 < p_{T} < 30");
    
    unfold1->Draw("same");
    leg->AddEntry("unfold1", "Unfolded, 20 < p_{T} < 30");
    
    truth2->Draw("same");
    leg->AddEntry("truth2", "Truth, 30 < p_{T} < 40");
    
    unfold2->Draw("same");
    leg->AddEntry("unfold2", "Unfolded, 30 < p_{T} < 40");
    leg->Draw();
    
    gPad->SaveAs( PlotBase + "_VarUnfoldedZg.png");  
  }
   
#if FALSE
  if (false){
      // =========================== Draw Zg Ratio ===============================
    new TCanvas;
    gPad->SetGridx(0);  gPad->SetGridy(0);
    
    leg = new TLegend( 0.25, 0.75, 0.59, 0.9, "z_{g} Ratio Unfolded / Truth" );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    leg->SetMargin(0.1);
    
    TH1D* ratio1 = (TH1D*) unfold1->Clone("ratio1");
    ratio1->Divide(truth1);
    // ratio1->SetLineColor(kRed);
    ratio1->SetTitle( ";z_{g};unfolded / truth" );
    ratio1->SetAxisRange( 0.5, 1.6, "y" );
    ratio1->Draw();
    leg->AddEntry("ratio1", "20 < p_{T} < 30");
    
    TH1D* ratio2 = (TH1D*) unfold2->Clone("ratio2");
    ratio2->Divide(truth2);
    // ratio2->SetLineColor(kBlue);
    ratio2->SetAxisRange( 0.5, 1.6, "y" );
    ratio2->Draw("same");
    leg->AddEntry("ratio2", "30 < p_{T} < 40");
    
    leg->Draw();
    gPad->SaveAs( PlotBase + "_UnfoldedZgRatio.png");  
    
    
    // =========================== Draw Pt Ratio ===============================
    new TCanvas;
    gPad->SetGridx(0);  gPad->SetGridy(0);
    // gPad->SetLogy();
    TLegend* leg = new TLegend( 0.55, 0.8, 0.89, 0.9, "Leading Jet p_{T}" );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    leg->SetMargin(0.1);
    
    TH1D* ratiopt = (TH1D*) unfoldpt->Clone("ratiopt");
    ratiopt->Divide(truthpt);
    ratiopt->SetLineColor(kRed);
    ratiopt->SetTitle( ";p_{T}" );
    ratiopt->SetAxisRange( 0.5, 1.5, "y" );
    ratiopt->Draw();
    leg->AddEntry("ratiopt", "2D Unfolded / Truth");
    
    TH1D* ratiopt1d = (TH1D*) PtBayesUnfolded->Clone("ratiopt1d");
    ratiopt1d->Divide(truthpt);
    ratiopt1d->SetLineColor(kBlue);
    ratiopt1d->SetTitle( ";p_{T}" );
    ratiopt1d->SetAxisRange( 0.5, 1.5, "y" );
    ratiopt1d->Draw("same");
    leg->AddEntry("ratiopt1d", "1D Unfolded / Truth");
    
    leg->Draw();
    gPad->SaveAs( PlotBase + "_UnfoldedPtRatio.png");  
    
    // // DEBUG
    // new TCanvas;
    // gPad->SetGridx(0);  gPad->SetGridy(0);
    // gPad->SetLogy();
    
    // truthpt->Draw();
    // unfoldpt->Draw("same");
    // PtBayesUnfolded->Draw("same");
  }
#endif 
    
  fout->Write();
  cout << " Wrote to" << endl << OutFileName << endl;
  return 0;
}
