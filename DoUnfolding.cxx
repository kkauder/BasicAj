
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

void SetErrors ( TH1D* const nom, const TH1D* const var, TH1D* const ratio=0 );

int DoUnfolding( // TString trainname = "AjResults/NewTrainedWith_WithMisses_NoOutliers_Groom_Aj_TrueMB_NoCut_NoEff_GeantMc.root",
		// TString trainname = "AjResults/AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant_WithMisses_NoOutliers_TrainedWith_Groom_Aj_TrueMB_NoCut_NoEff_GeantMc.root",
		// Closure
		TString trainname = "AjResults/ForClosure_AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant_WithMisses_NoOutliers_TrainedWith_Groom_Aj_TrueMB_NoCut_NoEff_GeantMc.root",
		// // Closure without outlier removal ("truth" is more correct)
		// TString trainname = "AjResults/ForClosure_AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant_WithMisses_TrainedWith_Groom_Aj_TrueMB_NoCut_NoEff_GeantMc.root",
		const int MaxnBayes2D = 4,
		TString ppname    = "AjResults/ForUnfolding_Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root",
		// TString ppname = "AjResults/ForClosure_AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant_WithMisses_NoOutliers_TrainedWith_Groom_Aj_TrueMB_NoCut_NoEff_GeantMc.root",
		int nBayes1D=3
		) {

  cerr << "FIX THE Eta Cut!!" << endl;  return -1;
  
  int RebinZg=2;  // SHOULD BE DONE EARLIER

  bool ClosureTest = ( trainname.Contains( "ForClosure" ) || ppname.Contains( "ForClosure" ) );
  if ( ClosureTest && trainname != ppname ){
    cerr << "Inconsistent naming for closure test" << endl;
    return -1;
  }
  
  // if >0, take this N as the nominal value and provide
  // systematics histos using the full difference to N+1
  // (symmetrized for easiness. FIXME)
  int NDefault=2;
  
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);

  TFile* ftrain = new TFile( trainname, "READ");
  // TString outbase = gSystem->BaseName( ftrain->GetName() );
  // outbase.ReplaceAll( ".root", "");
  // outbase.Prepend("Plots/");

  // Load pp data
  // ------------
  TFile* fmeas = new TFile( ppname, "READ");  
  TH2D* TrigTestMeas2D  = (TH2D*) fmeas->Get("TrigTestMeas2D");
  TH2D* RecoilTestMeas2D  = (TH2D*) fmeas->Get("RecoilTestMeas2D");

  TH2D* TrigTestTruth2D=0;
  TH2D* RecoilTestTruth2D=0;
  if ( ClosureTest ){
    TrigTestTruth2D  = (TH2D*) fmeas->Get("TrigTestTruth2D");
    RecoilTestTruth2D  = (TH2D*) fmeas->Get("RecoilTestTruth2D");
  }

  // Load response
  // ------------
  TH2D* TrigTrainMeas2D  = (TH2D*) ftrain->Get("TrigMeas2D");
  TH2D* TrigTrainTruth2D  = (TH2D*) ftrain->Get("TrigTruth2D");
  TH2D* RecoilTrainMeas2D  = (TH2D*) ftrain->Get("RecoilMeas2D");
  TH2D* RecoilTrainTruth2D  = (TH2D*) ftrain->Get("RecoilTruth2D");

  // Load Pythia8 comparison
  TFile* fP8 = new TFile( "AjResults/UnfoldedPpSystematics_Histos.root", "READ");  

  
  // Unfold 1D
  // ---------
  RooUnfoldResponse* TrigPtResponse = (RooUnfoldResponse*) ftrain->Get("TrigPtResponse");
  TH1D* TrigPtTestMeas  = (TH1D*) TrigTestMeas2D->ProjectionX("TrigPtTestMeas");
  RooUnfoldBayes    TrigPtBayesUnfold ( TrigPtResponse, TrigPtTestMeas, nBayes1D);
  TH1D* TrigPtBayesUnfolded= (TH1D*) TrigPtBayesUnfold.Hreco();
  TrigPtBayesUnfolded->SetName("TrigPtBayesUnfolded");
  
  RooUnfoldResponse* TrigPtZgResponse2D = (RooUnfoldResponse*) ftrain->Get("TrigPtZgResponse2D");

  // const int MaxnBayes2D = 7;
  // const int MaxnBayes2D = 3;
  cout << endl << " Starting 2D Unfolding " <<  endl ;
  TH2D** TrigBayesUnfolded = new TH2D*[MaxnBayes2D];
  for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D ){
    cout << endl << "=== Unfolding trigger distribution with nBayes2D=" << nBayes2D << " ===" << endl;
    RooUnfoldBayes    TrigBayesUnfold ( TrigPtZgResponse2D, TrigTestMeas2D, nBayes2D);
    TrigBayesUnfold.SetVerbose(1);
    TrigBayesUnfolded[nBayes2D-1] = (TH2D*) TrigBayesUnfold.Hreco();
     
    TString hname = "TrigBayesUnfolded_"; hname+=nBayes2D;
    TrigBayesUnfolded[nBayes2D-1] ->SetName(hname);
  }

  RooUnfoldResponse* RecoilPtResponse = (RooUnfoldResponse*) ftrain->Get("RecoilPtResponse");
  TH1D* RecoilPtTestMeas  = (TH1D*) RecoilTestMeas2D->ProjectionX("RecoilPtTestMeas");
  RooUnfoldBayes    RecoilPtBayesUnfold ( RecoilPtResponse, RecoilPtTestMeas, nBayes1D);
  TH1D* RecoilPtBayesUnfolded= (TH1D*) RecoilPtBayesUnfold.Hreco();
  RecoilPtBayesUnfolded->SetName("RecoilPtBayesUnfolded");
  
  RooUnfoldResponse* RecoilPtZgResponse2D = (RooUnfoldResponse*) ftrain->Get("RecoilPtZgResponse2D");

  TH2D** RecoilBayesUnfolded = new TH2D*[MaxnBayes2D];
  for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
    cout << endl << "=== Unfolding recoil distribution with nBayes2D=" << nBayes2D << " ===" << endl;
    RooUnfoldBayes    RecoilBayesUnfold ( RecoilPtZgResponse2D, RecoilTestMeas2D, nBayes2D);
    RecoilBayesUnfold.SetVerbose(1);
    RecoilBayesUnfolded[nBayes2D-1] = (TH2D*) RecoilBayesUnfold.Hreco();
     
    TString hname = "RecoilBayesUnfolded_"; hname+=nBayes2D;
    RecoilBayesUnfolded[nBayes2D-1] ->SetName(hname);
  }
  
  // Output
  // ------
  TString OutFileName = "AjResults/Unfolded_";
  // OutFileName += nBayes2D;   OutFileName += "_";
  OutFileName += gSystem->BaseName(trainname);
  TFile* fout = new TFile( OutFileName, "RECREATE");

  TString PlotBase = "plots/HT_Unfolded_Smooth";
  // PlotBase += nBayes2D;   PlotBase += "_";
  PlotBase += gSystem->BaseName(trainname);
  PlotBase.ReplaceAll(".root","");



  // =========================== Draw Spectra ===============================
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  TLegend* leg = new TLegend( 0.55, 0.45, 0.89, 0.9, "Trigger Jet p_{T}" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  gPad->SaveAs( PlotBase + "_Everything.pdf[");
    
  int rebinpt=2;
  TH1D* Trigmeaspt = (TH1D*) TrigTestMeas2D->ProjectionX("Trigmeaspt");
  Trigmeaspt->SetLineColor( kMagenta+2 );
  Trigmeaspt->Rebin( rebinpt );
  Trigmeaspt->SetTitle("");

  TH1D* TrigTrainTruthpt = (TH1D*) TrigTrainTruth2D->ProjectionX("TrigTrainTruthpt");
  // TrigTrainTruthpt->SetLineColor(kPink+7);
  TrigTrainTruthpt->SetLineColor(kBlack);
  TrigTrainTruthpt->Rebin( rebinpt );

  TH1D* TrigTestTruthpt=0;
  if ( ClosureTest ){
    TrigTestTruthpt = (TH1D*) TrigTestTruth2D->ProjectionX("TrigTestTruth2D");
    // TrigTestTruthpt->SetLineColor(kPink+7);
    TrigTestTruthpt->SetLineColor(kBlack);
    TrigTestTruthpt->Rebin( rebinpt );
    TrigTestTruthpt->SetAxisRange( 2, 80, "x");
    TrigTestTruthpt->SetAxisRange( 1e-1, 1e8, "y");
    TrigTestTruthpt->SetTitle(";p_{T};arb. units" );
  }
      
  TH1D* TrigTrainMeaspt = (TH1D*) TrigTrainMeas2D->ProjectionX("TrigTrainMeaspt");
  TrigTrainMeaspt->SetLineColor(kCyan+2);
  TrigTrainMeaspt->Rebin( rebinpt );

  TrigTrainTruthpt->SetAxisRange( 2, 80, "x");
  TrigTrainTruthpt->SetAxisRange( 1e-1, 1e8, "y");
  TrigTrainTruthpt->SetTitle(";p_{T};arb. units" );
  // tcs->cd();
  if ( ClosureTest ) TrigTestTruthpt->Draw("9histsame");
  else TrigTrainTruthpt->Draw("9histsame");

  Trigmeaspt->Draw("9same");
  TrigTrainMeaspt->Draw("9same");

  if ( ClosureTest ) leg->AddEntry("TrigTestTruthpt", "Test Truth");
  else               leg->AddEntry("TrigTrainTruthpt", "Training Truth");
  leg->AddEntry("TrigTrainMeaspt", "Training Meas.");
  leg->AddEntry("Trigmeaspt", "Measured");

  TH1D* SysTrigunfoldpt=0;
  for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
    TString hname = "Trigunfoldpt_"; hname+=nBayes2D;
    TH1D* Trigunfoldpt = (TH1D*) TrigBayesUnfolded[nBayes2D-1]->ProjectionX(hname);

    Trigunfoldpt->SetLineColor(nBayes2D+1);
    Trigunfoldpt->Rebin( rebinpt );
    
    Trigunfoldpt->Draw("same");
    TString title = "Unfolded, N = ";
    title +=nBayes2D;
    leg->AddEntry(Trigunfoldpt->GetName(), title);

    if ( NDefault>0 ) {
       // Initialize systematics histo
      if (nBayes2D==NDefault )	SysTrigunfoldpt = (TH1D*) Trigunfoldpt->Clone("SysTrigunfoldpt");
      // Set Errors
      if (nBayes2D==NDefault+1 ) {
	if ( !SysTrigunfoldpt ){
	  cerr << "Trying to calculate systematics but nominal histo doesn't exist!" << endl;
	  return -1;
	}
	SetErrors ( SysTrigunfoldpt, Trigunfoldpt );

	// And draw this and all before it on top
	SysTrigunfoldpt->SetMarkerSize(0);
	SysTrigunfoldpt->SetLineWidth( 0 );
	SysTrigunfoldpt->SetFillStyle(1001);
	SysTrigunfoldpt->SetFillColor( kGray );
	SysTrigunfoldpt->SetMarkerColor( kGray );
	SysTrigunfoldpt->Draw("9E2same");

	for ( int i=1; i<=nBayes2D; ++i){
	  hname = "Trigunfoldpt_"; hname+=i;
	  TH1D* h = (TH1D*) gDirectory->Get( hname );
	  if ( h ) h->Draw("9same");
	}
      }      
    }
  }

  if ( ClosureTest ) TrigTestTruthpt->Draw("9histsame");
  else TrigTrainTruthpt->Draw("9histsame");
  Trigmeaspt->Draw("9same");
  TrigTrainMeaspt->Draw("9same");

  // PtBayesUnfolded->SetLineColor(kGray+1);
  // PtBayesUnfolded->Draw("same");
  // leg->AddEntry("PtBayesUnfolded", "1D Unfolded");    

  leg->Draw();
  gPad->SaveAs( PlotBase + "_TrigUnfoldedSpectra.png");
  gPad->SaveAs( PlotBase + "_Everything.pdf");

  // RECOIL
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  leg = new TLegend( 0.55, 0.45, 0.89, 0.9, "Recoil Jet p_{T}" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  TH1D* Recoilmeaspt = (TH1D*) RecoilTestMeas2D->ProjectionX("Recoilmeaspt");
  Recoilmeaspt->SetLineColor( kMagenta+2 );
  Recoilmeaspt->Rebin( rebinpt );
  Recoilmeaspt->SetTitle("");
    
  TH1D* RecoilTrainTruthpt = (TH1D*) RecoilTrainTruth2D->ProjectionX("RecoilTrainTruthpt");
  // RecoilTrainTruthpt->SetLineColor(kPink+7);
  RecoilTrainTruthpt->SetLineColor(kBlack);
  RecoilTrainTruthpt->Rebin( rebinpt );
  RecoilTrainTruthpt->SetTitle(";p_{T};arb. units" );

  TH1D* RecoilTestTruthpt=0;
  if ( ClosureTest ){
    RecoilTestTruthpt = (TH1D*) RecoilTestTruth2D->ProjectionX("RecoilTestTruth2D");
    // RecoilTestTruthpt->SetLineColor(kPink+7);
    RecoilTestTruthpt->SetLineColor(kBlack);
    RecoilTestTruthpt->Rebin( rebinpt );
    RecoilTestTruthpt->SetAxisRange( 2, 80, "x");
    RecoilTestTruthpt->SetAxisRange( 1e-1, 1e8, "y");
    RecoilTestTruthpt->SetTitle(";p_{T};arb. units" );
  }


  TH1D* RecoilTrainMeaspt = (TH1D*) RecoilTrainMeas2D->ProjectionX("RecoilTrainMeaspt");
  RecoilTrainMeaspt->SetLineColor(kCyan+2);
  RecoilTrainMeaspt->Rebin( rebinpt );

  RecoilTrainTruthpt->SetAxisRange( 2, 80, "x");
  RecoilTrainTruthpt->SetAxisRange( 1e-2, 1e8, "y");
  if ( ClosureTest ) RecoilTestTruthpt->Draw("9histsame");
  else RecoilTrainTruthpt->Draw("9histsame");
  Recoilmeaspt->Draw("9same");
  RecoilTrainMeaspt->Draw("9same");


  if ( ClosureTest ) leg->AddEntry("RecoilTestTruthpt", "Test Truth");
  else               leg->AddEntry("RecoilTrainTruthpt", "Training Truth");
  leg->AddEntry("RecoilTrainMeaspt", "Training Meas.");
  leg->AddEntry("Recoilmeaspt", "Measured");

  TH1D* SysRecoilunfoldpt=0;
  for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
    TString hname = "Recoilunfoldpt_"; hname+=nBayes2D;
    TH1D* Recoilunfoldpt = (TH1D*) RecoilBayesUnfolded[nBayes2D-1]->ProjectionX(hname);

    Recoilunfoldpt->SetLineColor(nBayes2D+1);
    Recoilunfoldpt->Rebin( rebinpt );
    Recoilunfoldpt->Draw("same");
    TString title = "Unfolded, N = ";
    title +=nBayes2D;
    leg->AddEntry(Recoilunfoldpt->GetName(), title);

    if ( NDefault>0 ) {
      // Initialize systematics histo
      if (nBayes2D==NDefault )	SysRecoilunfoldpt = (TH1D*) Recoilunfoldpt->Clone("SysRecoilunfoldpt");
      // Set Errors
      if (nBayes2D==NDefault+1 ) {
	if ( !SysRecoilunfoldpt ){
	  cerr << "Trying to calculate systematics but nominal histo doesn't exist!" << endl;
	  return -1;
	}
	SetErrors ( SysRecoilunfoldpt, Recoilunfoldpt );
	
	// And draw this and all before it on top
	SysRecoilunfoldpt->SetMarkerSize(0);
	SysRecoilunfoldpt->SetLineWidth( 0 );
	SysRecoilunfoldpt->SetFillStyle(1001);
	SysRecoilunfoldpt->SetFillColor( kGray );
	SysRecoilunfoldpt->SetMarkerColor( kGray );
	SysRecoilunfoldpt->Draw("9E2same");
	
	for ( int i=1; i<=nBayes2D; ++i){
	  hname = "Recoilunfoldpt_"; hname+=i;
	  TH1D* h = (TH1D*) gDirectory->Get( hname );
	  if ( h ) h->Draw("9same");
	}
      }      
    }    
  }

  if ( ClosureTest ) RecoilTestTruthpt->Draw("9histsame");
  else RecoilTrainTruthpt->Draw("9histsame");
  Recoilmeaspt->Draw("9same");
  RecoilTrainMeaspt->Draw("9same");

  // PtBayesUnfolded->SetLineColor(kGray+1);
  // PtBayesUnfolded->Draw("same");
  // leg->AddEntry("PtBayesUnfolded", "1D Unfolded");    

  leg->Draw();
  gPad->SaveAs( PlotBase + "_RecoilUnfoldedSpectra.png");
  gPad->SaveAs( PlotBase + "_Everything.pdf");
  
  TString name;
  // =========================== Draw Zg ===============================
  TH1D* h;
  
  // =========================== Near Side ===============================
  double IncNSbins[] = { 10, 15, 20, 25, 30};
  //  double IncNSbins[] = { 10, 15, 20 };
  int nIncNSbins  = sizeof(IncNSbins) / sizeof(IncNSbins[0])-1;

  for ( int i=0 ; i<nIncNSbins ; ++i ){
    float ptleft  = IncNSbins[i];
    float ptright = IncNSbins[i+1];

    new TCanvas;
    gPad->SetGridx(0);  gPad->SetGridy(0);
    TString title = "Trigger Jet z_{g}, ";
    title += int(ptleft+0.01);   title += " < p_{T} < ";
    title += int(ptright+0.01);  title += " GeV/c";
    leg = new TLegend( 0.55, 0.45, 0.89, 0.9, title );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    leg->SetMargin(0.1);

    name = "Trig_truth_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* Trigtruth=0;
    if ( ClosureTest )    Trigtruth = (TH1D*) TrigTestTruth2D->ProjectionY(title,TrigTestTruth2D->GetXaxis()->FindBin( ptleft+0.01), TrigTestTruth2D->GetXaxis()->FindBin( ptright-0.01));
    else                  Trigtruth = (TH1D*) TrigTrainTruth2D->ProjectionY(title,TrigTrainTruth2D->GetXaxis()->FindBin( ptleft+0.01), TrigTrainTruth2D->GetXaxis()->FindBin( ptright-0.01));
    Trigtruth->SetLineColor(kBlack);
    Trigtruth->SetTitle( ";z_{g};1/N dN/dz_{g}" );

    h=Trigtruth; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
    h->Rebin( RebinZg );    h->Scale(1./RebinZg );
    h->SetAxisRange(0, 9, "y");

    Trigtruth->Draw("9");
    if ( ClosureTest )    leg->AddEntry("Trigtruth", "Test Truth");
    else leg->AddEntry("Trigtruth", "Training Truth");	  

    name = "Trigmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* Trigmeas =
      (TH1D*) TrigTestMeas2D->ProjectionY("Trigmeas", TrigTestMeas2D->GetXaxis()->FindBin( ptleft+0.01), TrigTestMeas2D->GetXaxis()->FindBin( ptright-0.01));
    Trigmeas->SetLineColor(kRed+1);
    h=Trigmeas; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
    h->Rebin( RebinZg );    h->Scale(1./RebinZg );
    h->SetAxisRange(0, 9, "y");
    Trigmeas->Draw("9same");
    leg->AddEntry(Trigmeas->GetName(), "Measured, 10 < p_{T}^{Meas} < 20");
    

    TH1D* SysTrigunfold=0;
    TH1D* SysTrigRatio=0;
    for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
      name = "Trigunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01); name +="_" ; name+=nBayes2D;
      TH2D* h2 = TrigBayesUnfolded[nBayes2D-1];
      TH1D* Trigunfold = (TH1D*) h2->ProjectionY(name, h2->GetXaxis()->FindBin( ptleft+0.01),h2 ->GetXaxis()->FindBin( ptright-0.01));
      Trigunfold->SetLineColor(nBayes2D+1);
      h=Trigunfold; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
      h->SetTitle( ";z_{g};1/N dN/dz_{g}" );
      h->Rebin( RebinZg );    h->Scale(1./RebinZg );
      h->SetAxisRange(0, 9, "y");
      h->Draw("same");
      TString title = "Unfolded, N = ";
      title +=nBayes2D;
      leg->AddEntry(h->GetName(), title);

      if ( NDefault>0 ) {
	// Initialize systematics histo
	if (nBayes2D==NDefault ){
	  name = "SysTrigunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01);
	  SysTrigunfold = (TH1D*) Trigunfold->Clone(name);
	  name = "SysTrigRatio_"; name += int(ptleft+0.01); name += int(ptright+0.01);
	  SysTrigRatio = (TH1D*) Trigunfold->Clone(name);
	  SysTrigRatio->Reset();
	  title = "Rel. Unc. Trigger pT=";
	  title += int(ptleft+0.01); title +="-";
	  title += int(ptright+0.01); title +=" GeV/c" ;
	  SysTrigRatio->SetTitle( title );		
	  SysTrigRatio->SetAxisRange( -.01,0.20, "y");
	}
	
	// Set Errors
	if (nBayes2D==NDefault+1 ) {
	  if ( !SysTrigunfold ){
	    cerr << "Trying to calculate systematics but nominal histo doesn't exist!" << endl;
	    return -1;
	  }
	  SetErrors ( SysTrigunfold, Trigunfold, SysTrigRatio );
	  
	  // And draw this and all before it on top
	  SysTrigunfold->SetMarkerSize(0);
	  SysTrigunfold->SetLineWidth( 0 );
	  SysTrigunfold->SetFillStyle(1001);
	  SysTrigunfold->SetFillColor( kGray );
	  SysTrigunfold->SetMarkerColor( kGray );
	  SysTrigunfold->Draw("9E2same");
	  
	  for ( int i=1; i<=nBayes2D; ++i){
	    name = "Trigunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01); name +="_" ; name+=i;
	    TH1D* h = (TH1D*) gDirectory->Get( name );
	    cout << "Drawing " << name << endl;
	    if ( h ) h->Draw("9same");
	  }
	}      
      }
     
    }

    Trigtruth->Draw("9same");
    Trigmeas->Draw("9same");
	
    name = "NS_P8_SoftJets_MB_"; name += int(ptleft+0.01); name+="_"; name += int(ptright+0.01);    
    h = (TH1D*) fP8->Get(name);
    if ( h ){
      h->SetLineStyle(2);
      h->Draw("9lhist][same");
      leg->AddEntry(h->GetName(), "Pythia8","l");
    }

    
    leg->Draw();
    name = PlotBase; name += "_Trigger_";
    name += int(ptleft+0.01); name += int(ptright+0.01);	
    gPad->SaveAs( name + ".png");
    gPad->SaveAs( PlotBase + "_Everything.pdf");

    if ( SysTrigRatio ) {
      new TCanvas;
      SysTrigRatio->Draw();
      name = PlotBase; name += "_TriggerSysRatio_";
      name += int(ptleft+0.01); name += int(ptright+0.01);	
      gPad->SaveAs( name + ".png");
      gPad->SaveAs( PlotBase + "_Everything.pdf");
    } 
  }
  
  // =========================== Away Side ===============================
  double IncASbins[] = { 10, 15, 20, 25, 30 };
  // double IncASbins[] = { 10, 15, 20 };
  int nIncASbins  = sizeof(IncASbins) / sizeof(IncASbins[0])-1;

  for ( int i=0 ; i<nIncASbins ; ++i ){
    float ptleft  = IncASbins[i];
    float ptright = IncASbins[i+1];

    new TCanvas;
    gPad->SetGridx(0);  gPad->SetGridy(0);      
    TString title = "Recoil Jet z_{g}, ";
    title += int(ptleft+0.01);   title += " < p_{T} < ";
    title += int(ptright+0.01);  title += " GeV/c";
    leg = new TLegend( 0.55, 0.45, 0.89, 0.9, title );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    leg->SetMargin(0.1);

    name = "Recoil_truth_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* Recoiltruth = 0;
    if ( ClosureTest )    Recoiltruth = (TH1D*) RecoilTestTruth2D->ProjectionY(title,RecoilTestTruth2D->GetXaxis()->FindBin( ptleft+0.01), RecoilTestTruth2D->GetXaxis()->FindBin( ptright-0.01));
    else                  Recoiltruth = (TH1D*) RecoilTrainTruth2D->ProjectionY(title,RecoilTrainTruth2D->GetXaxis()->FindBin( ptleft+0.01), RecoilTrainTruth2D->GetXaxis()->FindBin( ptright-0.01));
    Recoiltruth->SetLineColor(kBlack);
    Recoiltruth->SetTitle( ";z_{g};1/N dN/dz_{g}" );

    h=Recoiltruth; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
    h->SetTitle( ";z_{g};1/N dN/dz_{g}" );
    h->Rebin( RebinZg );    h->Scale(1./RebinZg );
    h->SetAxisRange(0, 9, "y");
    Recoiltruth->Draw("9");
    if ( ClosureTest )    leg->AddEntry(Recoiltruth->GetName(), "Test Truth");
    else                  leg->AddEntry(Recoiltruth->GetName(), "Training Truth");

    name = "Recoilmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* Recoilmeas =
      (TH1D*) RecoilTestMeas2D->ProjectionY("Recoilmeas", RecoilTestMeas2D->GetXaxis()->FindBin( ptleft+0.01), RecoilTestMeas2D->GetXaxis()->FindBin( ptright-0.01));
    Recoilmeas->SetLineColor(kRed+1);
    h=Recoilmeas; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
    h->SetTitle( ";z_{g};1/N dN/dz_{g}" );
    h->Rebin( RebinZg );    h->Scale(1./RebinZg );
    h->SetAxisRange(0, 9, "y");
    Recoilmeas->Draw("9same");
    leg->AddEntry(Recoilmeas->GetName(), "Measured, 10 < p_{T}^{Meas} < 20");
	
    TH1D* SysRecoilunfold=0;
    TH1D* SysRecoilRatio=0;
    for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
      name = "Recoilunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01); name +="_" ; name+=nBayes2D;
      TH2D* h2 = RecoilBayesUnfolded[nBayes2D-1];
      TH1D* Recoilunfold = (TH1D*) h2->ProjectionY(name, h2->GetXaxis()->FindBin( ptleft+0.01),h2 ->GetXaxis()->FindBin( ptright-0.01));
      Recoilunfold->SetLineColor(nBayes2D+1);
      h=Recoilunfold; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
      h->SetTitle( ";z_{g};1/N dN/dz_{g}" );
      h->Rebin( RebinZg );    h->Scale(1./RebinZg );
      h->SetAxisRange(0, 9, "y");
      h->Draw("same");
      TString title = "Unfolded, N = ";
      title +=nBayes2D;
      leg->AddEntry(h->GetName(), title);

      if ( NDefault>0 ) {
	// Initialize systematics histo
	if (nBayes2D==NDefault ){
	  name = "SysRecoilunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01);
	  SysRecoilunfold = (TH1D*) Recoilunfold->Clone(name);
	  name = "SysRecoilRatio_"; name += int(ptleft+0.01); name += int(ptright+0.01);
	  SysRecoilRatio = (TH1D*) Recoilunfold->Clone(name);
	  SysRecoilRatio->Reset();
	  title = "Rel. Unc. Recoil pT=";
	  title += int(ptleft+0.01); title +="-";
	  title += int(ptright+0.01); title +=" GeV/c" ;
	  SysRecoilRatio->SetTitle( title );		
	  SysRecoilRatio->SetAxisRange( -.01,0.2, "y");
	}
	
	// Set Errors
	if (nBayes2D==NDefault+1 ) {
	  if ( !SysRecoilunfold ){
	    cerr << "Trying to calculate systematics but nominal histo doesn't exist!" << endl;
	    return -1;
	  }
	  SetErrors ( SysRecoilunfold, Recoilunfold, SysRecoilRatio );
	  
	  // And draw this and all before it on top
	  SysRecoilunfold->SetMarkerSize(0);
	  SysRecoilunfold->SetLineWidth( 0 );
	  SysRecoilunfold->SetFillStyle(1001);
	  SysRecoilunfold->SetFillColor( kGray );
	  SysRecoilunfold->SetMarkerColor( kGray );
	  SysRecoilunfold->Draw("9E2same");
	  
	  for ( int i=1; i<=nBayes2D; ++i){
	    name = "Recoilunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01); name +="_" ; name+=i;
	    TH1D* h = (TH1D*) gDirectory->Get( name );
	    cout << "Drawing " << name << endl;
	    if ( h ) h->Draw("9same");
	  }
	}      
      }

    }

    Recoiltruth->Draw("9same");
    Recoilmeas->Draw("9same");

    name = "AS_P8_SoftJets_MB_"; name += int(ptleft+0.01); name+="_"; name += int(ptright+0.01);
    h = (TH1D*) fP8->Get(name);
    if ( h ){
      h->SetLineStyle(2);
      h->Draw("9lhist][same");
      leg->AddEntry(h->GetName(), "Pythia8","l");
    }

    leg->Draw();

    name = PlotBase; name += "_Recoil_";
    name += int(ptleft+0.01); name += int(ptright+0.01);	
    gPad->SaveAs( name + ".png");
    gPad->SaveAs( PlotBase + "_Everything.pdf");

    if ( SysRecoilRatio ) {
      new TCanvas;
      SysRecoilRatio->Draw();
      name = PlotBase; name += "_RecoilSysRatio_";
      name += int(ptleft+0.01); name += int(ptright+0.01);	
      gPad->SaveAs( name + ".png");
      gPad->SaveAs( PlotBase + "_Everything.pdf");
    } 

  }

  gPad->SaveAs( PlotBase + "_Everything.pdf]");
  //  return 0;

  // // =========================== OLD STYLE Draw Zg IGNORE/Delete ===============================

  
  // //TH1D* h;
  // new TCanvas;
  // gPad->SetGridx(0);  gPad->SetGridy(0);

  // leg = new TLegend( 0.55, 0.45, 0.89, 0.9, "Leading Jet z_{g}, 10 < p_{T} < 20" );
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);
    
  // // TH1D* truth0 = (TH1D*) TestTruth2D->ProjectionY("truth0", TestTruth2D->GetXaxis()->FindBin( 10+0.01), TestTruth2D->GetXaxis()->FindBin( 20-0.01));
  // // truth0->SetLineColor(kGray+1);
  // // truth0->SetTitle( ";z_{g};arb. units" );
  // // truth0->Scale (1./truth0->Integral());
  // // truth0->SetAxisRange(0, 0.18, "y");
  // // truth0->Draw();
  // // leg->AddEntry("truth0", "Truth, 10 < p_{T} < 10");

  // TH1D* truth0 = (TH1D*) TrainTruth2D->ProjectionY("truth0", TrainTruth2D->GetXaxis()->FindBin( 10+0.01), TrainTruth2D->GetXaxis()->FindBin( 20-0.01));
  // // truth0->SetLineColor(kGray+1);
  // truth0->SetLineColor(kBlack);
  // truth0->SetTitle( ";z_{g};arb. units" );
  // // truth0->Scale (1./truth0->Integral());
  // h=truth0; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );

  // h->SetAxisRange(0, 9, "y");
  // h->Draw("hist");

  // leg->AddEntry("truth0", "TrainTruth");

  // TH1D* meas0 = (TH1D*) TestMeas2D->ProjectionY("meas0", TestMeas2D->GetXaxis()->FindBin( 10+0.01), TestMeas2D->GetXaxis()->FindBin( 20-0.01));
  // meas0->SetLineColor(kRed+1);
  // h=meas0; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // h->SetAxisRange(0, 9, "y");
  // // h->Draw("same");
  // // leg->AddEntry("meas0", "Measured, 10 < p_{T}^{Meas} < 20");

  // for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
  //   TString hname = "unfold0_"; hname+=nBayes2D;
  //   TH2D* h2 = BayesUnfolded[nBayes2D-1];
  //   TH1D* unfold0 = (TH1D*) h2->ProjectionY(hname, h2->GetXaxis()->FindBin( 10+0.01),h2 ->GetXaxis()->FindBin( 20-0.01));
  //   // unfold0->SetLineColor(kBlack);
  //   unfold0->SetLineColor(nBayes2D+1);
  //   h=unfold0; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  //   h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  //   h->SetAxisRange(0, 9, "y");
  //   h->Draw("same");
  //   // leg->AddEntry(h->GetName(), "Unfolded, 10 < p_{T} < 20");
  //   TString title = "Unfolded, N = ";
  //   title +=nBayes2D;
  //   leg->AddEntry(h->GetName(), title);
  // }

  // // TH1D* unfold0 = (TH1D*) BayesUnfolded->ProjectionY("unfold0", BayesUnfolded->GetXaxis()->FindBin( 10+0.01), BayesUnfolded->GetXaxis()->FindBin( 20-0.01));
  // // unfold0->SetLineColor(kBlack);
  // // h=unfold0; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // // h->SetAxisRange(0, 9, "y");
  // // h->Draw("same");
  // // leg->AddEntry("unfold0", "Unfolded, 10 < p_{T} < 20");
  // leg->Draw();
  // gPad->SaveAs( PlotBase + "_UnfoldedZg1020.png");


  // // 20-30
  // new TCanvas;
  // gPad->SetGridx(0);  gPad->SetGridy(0);

  // leg = new TLegend( 0.55, 0.45, 0.89, 0.9, "Leading Jet z_{g}, 20 < p_{T} < 30" );
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);
    

  // TH1D* truth1 = (TH1D*) TrainTruth2D->ProjectionY("truth1", TrainTruth2D->GetXaxis()->FindBin( 20+0.01), TrainTruth2D->GetXaxis()->FindBin( 30-0.01));
  // // truth0->SetLineColor(kGray+1);
  // truth1->SetLineColor(kBlack);
  // truth1->SetTitle( ";z_{g};arb. units" );
  // // truth1->Scale (1./truth0->Integral());
  // h=truth1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );

  // h->SetAxisRange(0, 9, "y");
  // h->Draw("hist");

  // leg->AddEntry("truth1", "TrainTruth");

  // TH1D* meas1 = (TH1D*) TestMeas2D->ProjectionY("meas1", TestMeas2D->GetXaxis()->FindBin( 20+0.01), TestMeas2D->GetXaxis()->FindBin( 30-0.01));
  // meas1->SetLineColor(kRed+1);
  // h=meas1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // h->SetAxisRange(0, 9, "y");
  // // h->Draw("same");
  // // leg->AddEntry("meas0", "Measured, 10 < p_{T}^{Meas} < 20");

  // for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
  //   TString hname = "unfold1_"; hname+=nBayes2D;
  //   TH2D* h2 = BayesUnfolded[nBayes2D-1];
  //   TH1D* unfold1 = (TH1D*) h2->ProjectionY(hname, h2->GetXaxis()->FindBin( 20+0.01),h2 ->GetXaxis()->FindBin( 30-0.01));
  //   // unfold1->SetLineColor(kBlack);
  //   unfold1->SetLineColor(nBayes2D+1);
  //   h=unfold1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  //   h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  //   h->SetAxisRange(0, 9, "y");
  //   h->Draw("same");
  //   // leg->AddEntry(h->GetName(), "Unfolded, 10 < p_{T} < 20");
  //   TString title = "Unfolded, N = ";
  //   title +=nBayes2D;
  //   leg->AddEntry(h->GetName(), title);
  // }

  // leg->Draw();
  // gPad->SaveAs( PlotBase + "_UnfoldedZg2030.png");


  // // 20-30
  // new TCanvas;
  // gPad->SetGridx(0);  gPad->SetGridy(0);

  // leg = new TLegend( 0.55, 0.65, 0.89, 0.9, "Leading Jet z_{g} NORMALIZED" );
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);

  // // TH1D* truth1 = (TH1D*) TestTruth2D->ProjectionY("truth1", TestTruth2D->GetXaxis()->FindBin( 20+0.01), TestTruth2D->GetXaxis()->FindBin( 30-0.01));
  // // truth1->SetLineColor(kRed);
  // // truth1->SetTitle( ";z_{g};arb. units" );
  // // truth1->DrawNormalized("same");
  // // leg->AddEntry("truth1", "Truth, 20 < p_{T} < 30");

  // truth1 = (TH1D*) TrainTruth2D->ProjectionY("truth1", TrainTruth2D->GetXaxis()->FindBin( 20+0.01), TrainTruth2D->GetXaxis()->FindBin( 30-0.01));
  // truth1->SetLineColor(kGray+1);
  // truth1->SetTitle( ";z_{g};arb. units" );
  // h=truth1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // h->SetAxisRange(0, 9, "y");
  // h->Draw("");
  // leg->AddEntry("truth1", "Train Truth, 20 < p_{T} < 30");

  // meas1 = (TH1D*) TestMeas2D->ProjectionY("meas1", TestMeas2D->GetXaxis()->FindBin( 20+0.01), TestMeas2D->GetXaxis()->FindBin( 30-0.01));
  // meas1->SetLineColor(kRed);
  // h=meas1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // h->SetAxisRange(0, 9, "y");
  // h->Draw("same");
  // leg->AddEntry("meas1", "Measured, 20 < p_{T}^{Meas} < 30");

  // TH1D* unfold1 = (TH1D*) BayesUnfolded->ProjectionY("unfold1", BayesUnfolded->GetXaxis()->FindBin( 20+0.01), BayesUnfolded->GetXaxis()->FindBin( 30-0.01));
  // unfold1->SetLineColor(kBlack);
  // h=unfold1; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // h->SetAxisRange(0, 9, "y");
  // h->Draw("same");

  // leg->AddEntry("unfold1", "Unfolded, 20 < p_{T} < 30");
  // leg->Draw();
  // gPad->SaveAs( PlotBase + "_UnfoldedZg2030.png");  

  // new TCanvas;
  // gPad->SetGridx(0);  gPad->SetGridy(0);

  // leg = new TLegend( 0.55, 0.65, 0.89, 0.9, "Leading Jet z_{g} NORMALIZED" );
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);

  // // truth2 = (TH1D*) TestTruth2D->ProjectionY("truth2", TestTruth2D->GetXaxis()->FindBin( 30+0.01), TestTruth2D->GetXaxis()->FindBin( 40-0.01));
  // // truth2->SetLineColor(kMagenta);
  // // truth2->DrawNormalized("same");
  // // leg->AddEntry("truth2", "Truth, 30 < p_{T} < 40");

  // truth2 = (TH1D*) TrainTruth2D->ProjectionY("truth2", TrainTruth2D->GetXaxis()->FindBin( 30+0.01), TrainTruth2D->GetXaxis()->FindBin( 40-0.01));
  // truth2->SetLineColor(kGray+1);
  // h=truth2; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->SetTitle("");
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // h->SetAxisRange(0, 9, "y");
  // h->Draw("");
  // leg->AddEntry("truth2", "Train Truth, 30 < p_{T} < 40");

  // TH1D* meas2 = (TH1D*) TestMeas2D->ProjectionY("meas2", TestMeas2D->GetXaxis()->FindBin( 30+0.01), TestMeas2D->GetXaxis()->FindBin( 40-0.01));
  // meas2->SetLineColor(kRed);
  // h=meas2; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // h->SetAxisRange(0, 9, "y");
  // h->Draw("same");

  // leg->AddEntry("meas2", "Measured, 30 < p_{T}^{Meas} < 40");

  // TH1D* unfold2 = (TH1D*) BayesUnfolded->ProjectionY("unfold2", BayesUnfolded->GetXaxis()->FindBin( 30+0.01), BayesUnfolded->GetXaxis()->FindBin( 40-0.01));
  // unfold2->SetLineColor(kBlack);
  // h=unfold2; h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));
  // h->Rebin( RebinZg );    h->Scale(1./RebinZg );
  // h->SetAxisRange(0, 9, "y");
  // h->Draw("same");


  // leg->AddEntry("unfold2", "Unfolded, 30 < p_{T} < 40");
  // leg->Draw();
  
  // gPad->SaveAs( PlotBase + "_UnfoldedZg3040.png");  

  // if ( false ){
  //   // =========================== Draw unnormalized Zg ===============================
  //   new TCanvas;
  //   gPad->SetGridx(0);  gPad->SetGridy(0);
    
  //   leg = new TLegend( 0.55, 0.65, 0.89, 0.9, "Leading Jet z_{g}" );
  //   leg->SetBorderSize(0);
  //   leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);
  //   leg->SetMargin(0.1);
    
  //   truth1->Draw();
  //   leg->AddEntry("truth1", "Truth, 20 < p_{T} < 30");
    
  //   unfold1->Draw("same");
  //   leg->AddEntry("unfold1", "Unfolded, 20 < p_{T} < 30");
    
  //   truth2->Draw("same");
  //   leg->AddEntry("truth2", "Truth, 30 < p_{T} < 40");
    
  //   unfold2->Draw("same");
  //   leg->AddEntry("unfold2", "Unfolded, 30 < p_{T} < 40");
  //   leg->Draw();
    
  //   gPad->SaveAs( PlotBase + "_VarUnfoldedZg.png");  
  // }
   
// #if FALSE
//   if (false){
//       // =========================== Draw Zg Ratio ===============================
//     new TCanvas;
//     gPad->SetGridx(0);  gPad->SetGridy(0);
    
//     leg = new TLegend( 0.25, 0.75, 0.59, 0.9, "z_{g} Ratio Unfolded / Truth" );
//     leg->SetBorderSize(0);
//     leg->SetLineWidth(10);
//     leg->SetFillStyle(0);
//     leg->SetMargin(0.1);
    
//     TH1D* ratio1 = (TH1D*) unfold1->Clone("ratio1");
//     ratio1->Divide(truth1);
//     // ratio1->SetLineColor(kRed);
//     ratio1->SetTitle( ";z_{g};unfolded / truth" );
//     ratio1->SetAxisRange( 0.5, 1.6, "y" );
//     ratio1->Draw();
//     leg->AddEntry("ratio1", "20 < p_{T} < 30");
    
//     TH1D* ratio2 = (TH1D*) unfold2->Clone("ratio2");
//     ratio2->Divide(truth2);
//     // ratio2->SetLineColor(kBlue);
//     ratio2->SetAxisRange( 0.5, 1.6, "y" );
//     ratio2->Draw("same");
//     leg->AddEntry("ratio2", "30 < p_{T} < 40");
    
//     leg->Draw();
//     gPad->SaveAs( PlotBase + "_UnfoldedZgRatio.png");  
    
    
//     // =========================== Draw Pt Ratio ===============================
//     new TCanvas;
//     gPad->SetGridx(0);  gPad->SetGridy(0);
//     // gPad->SetLogy();
//     TLegend* leg = new TLegend( 0.55, 0.8, 0.89, 0.9, "Leading Jet p_{T}" );
//     leg->SetBorderSize(0);
//     leg->SetLineWidth(10);
//     leg->SetFillStyle(0);
//     leg->SetMargin(0.1);
    
//     TH1D* ratiopt = (TH1D*) unfoldpt->Clone("ratiopt");
//     ratiopt->Divide(truthpt);
//     ratiopt->SetLineColor(kRed);
//     ratiopt->SetTitle( ";p_{T}" );
//     ratiopt->SetAxisRange( 0.5, 1.5, "y" );
//     ratiopt->Draw();
//     leg->AddEntry("ratiopt", "2D Unfolded / Truth");
    
//     TH1D* ratiopt1d = (TH1D*) PtBayesUnfolded->Clone("ratiopt1d");
//     ratiopt1d->Divide(truthpt);
//     ratiopt1d->SetLineColor(kBlue);
//     ratiopt1d->SetTitle( ";p_{T}" );
//     ratiopt1d->SetAxisRange( 0.5, 1.5, "y" );
//     ratiopt1d->Draw("same");
//     leg->AddEntry("ratiopt1d", "1D Unfolded / Truth");
    
//     leg->Draw();
//     gPad->SaveAs( PlotBase + "_UnfoldedPtRatio.png");  
    
//     // // DEBUG
//     // new TCanvas;
//     // gPad->SetGridx(0);  gPad->SetGridy(0);
//     // gPad->SetLogy();
    
//     // truthpt->Draw();
//     // unfoldpt->Draw("same");
//     // PtBayesUnfolded->Draw("same");
//   }
// #endif 
    
  fout->Write();
  cout << " Wrote to" << endl << OutFileName << endl;
  return 0;
}

void SetErrors ( TH1D* const nom, const TH1D* const var, TH1D* const ratio ){
  for (int i=1; i< nom->GetNbinsX(); ++i ){
    double n = nom->GetBinContent(i);
    double v = var->GetBinContent(i);
    double e = fabs ( n - v );
    nom->SetBinError ( i, e );
    if ( ratio && fabs(n)>1e-6 )     ratio->SetBinContent ( i, e, e/n );
  }

}
