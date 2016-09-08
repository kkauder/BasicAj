{
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);
 
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);


  TString plotpath="./GeantDiagnosis/";
  TString dopt;
  TString cond;
  TLegend* leg=0;

  // -----------------------------------  
  // ------- Compare to pp -------------
  // -----------------------------------
  TFile *fmc_ppcomp = TFile::Open("AjResults/Groom_Aj_HT54_GeantMc.root");
  TFile *fg_ppcomp = TFile::Open("AjResults/Groom_Aj_HT54_Geant.root");
  TFile *fpp_ppcomp = TFile::Open("AjResults/Groom_Aj_HT54_NoEff_ppAj.root");
  TFile *f8_54_tot = TFile::Open("AjResults/HT54_LargeEtaPythiaOnly.root");
  
  TTree* Tmc = (TTree*) fmc_ppcomp->Get("ResultTree");
  TTree* Tg = (TTree*) fg_ppcomp->Get("ResultTree");
  TTree* Tpp = (TTree*) fpp_ppcomp->Get("ResultTree");
  TTree* T8 = (TTree*) f8_54_tot->Get("ResultTree");

  // --- aj_hi
  dopt= "aj_hi >> ";
  cond= "weight*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

  int nBins = 40;
  TH1D* aj_hi_mc_ppcomp = new TH1D("aj_hi_mc_ppcomp","E_{T}>5.4 GeV;|A_{J}|;a.u.",nBins, -0.3, 0.9);
  Tmc->Draw(   dopt + TString("aj_hi_mc_ppcomp") , cond , "goff" );
  TH1D* aj_hi_g_ppcomp = new TH1D("aj_hi_g_ppcomp","ALL #hat{p_{T}} bins, E_{T}>5.4 GeV;|A_{J}|;a.u.",nBins, -0.3, 0.9);
  Tg->Draw(   dopt + TString("aj_hi_g_ppcomp") , cond , "goff" );
  TH1D* aj_hi_pp_ppcomp = new TH1D("aj_hi_pp_ppcomp","E_{T}>5.4 GeV;|A_{J}|;a.u.",nBins, -0.3, 0.9);
  Tpp->Draw(   dopt + TString("aj_hi_pp_ppcomp") , cond , "goff" );
  TH1D* aj_hi_8_54_tot = new TH1D("aj_hi_8_54_tot","ALL #hat{p_{T}} bins, E_{T}>5.4 GeV;|A_{J}|;a.u.",nBins, -0.3, 0.9);
  T8->Draw(   dopt + TString("aj_hi_8_54_tot") , cond , "goff" );


  new TCanvas;
  aj_hi_g_ppcomp->Scale( 1./aj_hi_g_ppcomp->Integral(1,aj_hi_g_ppcomp->GetNbinsX()) );
  aj_hi_mc_ppcomp->Scale( 1./aj_hi_mc_ppcomp->Integral(1,aj_hi_mc_ppcomp->GetNbinsX()) );
  aj_hi_pp_ppcomp->Scale( 1./aj_hi_pp_ppcomp->Integral(1,aj_hi_pp_ppcomp->GetNbinsX()) );
  aj_hi_8_54_tot->Scale( 1./aj_hi_8_54_tot->Integral(1,aj_hi_8_54_tot->GetNbinsX()) );
  aj_hi_g_ppcomp->SetAxisRange( 0, 0.7, "x");
  aj_hi_mc_ppcomp->SetAxisRange( 0, 0.7, "x");
  aj_hi_pp_ppcomp->SetAxisRange( 0, 0.7, "x");
  aj_hi_8_54_tot->SetAxisRange( 0, 0.7, "x");
  if ( nBins==40 ){
    aj_hi_g_ppcomp->SetAxisRange( 0, 0.16, "y");
    aj_hi_mc_ppcomp->SetAxisRange( 0, 0.16, "y");
    aj_hi_pp_ppcomp->SetAxisRange( 0, 0.16, "y");
    aj_hi_8_54_tot->SetAxisRange( 0, 0.16, "y");
  }
  if ( nBins==20 ){
    aj_hi_g_ppcomp->SetAxisRange( 0, 0.32, "y");
    aj_hi_mc_ppcomp->SetAxisRange( 0, 0.32, "y");
    aj_hi_pp_ppcomp->SetAxisRange( 0, 0.32, "y");
    aj_hi_8_54_tot->SetAxisRange( 0, 0.32, "y");
  }
    
      
  aj_hi_mc_ppcomp->SetLineColor( kBlack );
  aj_hi_8_54_tot->SetLineColor( kRed);

  aj_hi_g_ppcomp->SetLineColor( kGreen+1 );
  aj_hi_g_ppcomp->SetMarkerColor( kGreen+1 );
  aj_hi_g_ppcomp->SetMarkerStyle( 24 );
  aj_hi_pp_ppcomp->SetLineColor( kMagenta+1);
  aj_hi_pp_ppcomp->SetMarkerColor( kMagenta+1 );
  aj_hi_pp_ppcomp->SetMarkerStyle( 24 );

  aj_hi_g_ppcomp->Draw();
  aj_hi_8_54_tot->Draw("same");
  aj_hi_mc_ppcomp->Draw("same");
  aj_hi_pp_ppcomp->Draw("same");

  leg = new TLegend( 0.5, 0.55, 0.88, 0.88, "p_{T}^{Cut}>2 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry ( aj_hi_mc_ppcomp, "Pythia 6.410, CTEQ5L", "l");
  leg->AddEntry ( aj_hi_8_54_tot, "Pythia 8.219, vanilla", "l");
  leg->AddEntry ( aj_hi_g_ppcomp, "Pythia 6 #otimes GEANT", "lp");
  leg->AddEntry ( aj_hi_pp_ppcomp, "pp HT, no eff. corr.", "lp");
  leg->Draw();
  gPad->SaveAs( plotpath + "aj_hi_54_ppcomp.png");
  
  // --- aj_lo
  dopt= "abs(aj_lo) >> ";
  cond= "weight*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

  TH1D* aj_lo_mc_ppcomp = new TH1D("aj_lo_mc_ppcomp","E_{T}>5.4 GeV;|A_{J}|;a.u.",nBins, -0.3, 0.9);
  Tmc->Draw(   dopt + TString("aj_lo_mc_ppcomp") , cond , "goff" );
  TH1D* aj_lo_g_ppcomp = new TH1D("aj_lo_g_ppcomp","ALL #hat{p_{T}} bins, E_{T}>5.4 GeV;|A_{J}|;a.u.",nBins, -0.3, 0.9);
  Tg->Draw(   dopt + TString("aj_lo_g_ppcomp") , cond , "goff" );
  TH1D* aj_lo_pp_ppcomp = new TH1D("aj_lo_pp_ppcomp","E_{T}>5.4 GeV;|A_{J}|;a.u.",nBins, -0.3, 0.9);
  Tpp->Draw(   dopt + TString("aj_lo_pp_ppcomp") , cond , "goff" );
  TH1D* aj_lo_8_54_tot = new TH1D("aj_lo_8_54_tot","ALL #hat{p_{T}} bins, E_{T}>5.4 GeV;|A_{J}|;a.u.",nBins, -0.3, 0.9);
  T8->Draw(   dopt + TString("aj_lo_8_54_tot") , cond , "goff" );
    

  new TCanvas;
  aj_lo_g_ppcomp->Scale( 1./aj_lo_g_ppcomp->Integral(1,aj_lo_g_ppcomp->GetNbinsX()) );
  aj_lo_mc_ppcomp->Scale( 1./aj_lo_mc_ppcomp->Integral(1,aj_lo_mc_ppcomp->GetNbinsX()) );
  aj_lo_pp_ppcomp->Scale( 1./aj_lo_pp_ppcomp->Integral(1,aj_lo_pp_ppcomp->GetNbinsX()) );
  aj_lo_8_54_tot->Scale( 1./aj_lo_8_54_tot->Integral(1,aj_lo_8_54_tot->GetNbinsX()) );
  aj_lo_g_ppcomp->SetAxisRange( 0, 0.7, "x");
  aj_lo_mc_ppcomp->SetAxisRange( 0, 0.7, "x");
  aj_lo_pp_ppcomp->SetAxisRange( 0, 0.7, "x");
  aj_lo_8_54_tot->SetAxisRange( 0, 0.7, "x");
  if ( nBins==40 ){
    aj_lo_g_ppcomp->SetAxisRange( 0, 0.3, "y");
    aj_lo_mc_ppcomp->SetAxisRange( 0, 0.3, "y");
    aj_lo_pp_ppcomp->SetAxisRange( 0, 0.3, "y");
    aj_lo_8_54_tot->SetAxisRange( 0, 0.3, "y");
  }
  if ( nBins==20 ){
    aj_lo_g_ppcomp->SetAxisRange( 0, 0.6, "y");
    aj_lo_mc_ppcomp->SetAxisRange( 0, 0.6, "y");
    aj_lo_pp_ppcomp->SetAxisRange( 0, 0.6, "y");
    aj_lo_8_54_tot->SetAxisRange( 0, 0.6, "y");
  }
    
  aj_lo_8_54_tot->SetLineColor( kRed);
  aj_lo_mc_ppcomp->SetLineColor( kBlack );

  aj_lo_g_ppcomp->SetLineColor( kGreen+1 );
  aj_lo_g_ppcomp->SetMarkerColor( kGreen+1 );
  aj_lo_g_ppcomp->SetMarkerStyle( 25 );
  aj_lo_pp_ppcomp->SetLineColor( kMagenta+1);
  aj_lo_pp_ppcomp->SetMarkerColor( kMagenta+1 );
  aj_lo_pp_ppcomp->SetMarkerStyle( 25 );
  
  aj_lo_g_ppcomp->Draw();
  aj_lo_8_54_tot->Draw("same");
  aj_lo_mc_ppcomp->Draw("same");
  aj_lo_pp_ppcomp->Draw("same");

  leg = new TLegend( 0.5, 0.55, 0.88, 0.88, "Matched, p_{T}^{Cut}>0.2 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry ( aj_lo_mc_ppcomp, "Pythia 6.410, CTEQ5L", "l");
  leg->AddEntry ( aj_lo_8_54_tot, "Pythia 8.219, vanilla", "l");
  leg->AddEntry ( aj_lo_g_ppcomp, "Pythia 6 #otimes GEANT", "lp");
  leg->AddEntry ( aj_lo_pp_ppcomp, "pp HT, no eff. corr.", "lp");
  leg->Draw();
  gPad->SaveAs( plotpath + "aj_lo_54_ppcomp.png");


}
