{
  TString plotpath="./GeantDiagnosis/";

  bool drawtot=false;
  bool drawdef=true;
  bool drawmb=true;
  bool drawlowpt=true;
  bool drawpt1=true;
  bool drawconsts=false;
  bool drawjetspectra=true;
  
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);
 
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);
  TString dopt;
  TString cond;
  TLegend* leg=0;


  if ( drawtot )  {
    // -----------------------------------  
    // ------- Draw all pT bins ----------
    // -----------------------------------  
    TFile *f6_54_tot = TFile::Open("AjResults/Groom_Aj_HT54_GeantMc.root");
    TFile *f8_54_tot = TFile::Open("AjResults/HT54_LargeEtaPythiaOnly.root");
  
    T6 = (TTree*) f6_54_tot->Get("ResultTree");
    T8 = (TTree*) f8_54_tot->Get("ResultTree");

    // --- aj_hi
    dopt= "aj_hi >> ";
    cond= "weight*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

    TH1D* aj_hi_6_54_tot = new TH1D("aj_hi_6_54_tot","ALL #hat{p_{T}} bins, E_{T}>5.4 GeV;|A_{J}|;a.u.",40, -0.3, 0.9);
    T6->Draw(   dopt + TString("aj_hi_6_54_tot") , cond , "goff" );
    TH1D* aj_hi_8_54_tot = new TH1D("aj_hi_8_54_tot","ALL #hat{p_{T}} bins, E_{T}>5.4 GeV;|A_{J}|;a.u.",40, -0.3, 0.9);
    T8->Draw(   dopt + TString("aj_hi_8_54_tot") , cond , "goff" );

    new TCanvas;
    aj_hi_6_54_tot->SetAxisRange( 0, 0.8, "x");
    aj_hi_8_54_tot->SetAxisRange( 0, 0.8, "x");
    aj_hi_6_54_tot->SetLineColor( kBlack);
    aj_hi_8_54_tot->SetLineColor( kRed);
  
    aj_hi_6_54_tot->DrawNormalized();
    aj_hi_8_54_tot->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "p_{T}^{Cut}>2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( aj_hi_6_54_tot, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( aj_hi_8_54_tot, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "aj_hi_54_tot.png");
  
    // --- aj_lo
    dopt= "abs(aj_lo) >> ";
    cond= "weight*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

    TH1D* aj_lo_6_54_tot = new TH1D("aj_lo_6_54_tot","ALL #hat{p_{T}} bins, E_{T}>5.4 GeV;|A_{J}|;a.u.",40, -0.3, 0.9);
    T6->Draw(   dopt + TString("aj_lo_6_54_tot") , cond , "goff" );
    TH1D* aj_lo_8_54_tot = new TH1D("aj_lo_8_54_tot","ALL #hat{p_{T}} bins, E_{T}>5.4 GeV;|A_{J}|;a.u.",40, -0.3, 0.9);
    T8->Draw(   dopt + TString("aj_lo_8_54_tot") , cond , "goff" );

    new TCanvas;
    aj_lo_6_54_tot->SetAxisRange( 0, 0.8, "x");
    aj_lo_8_54_tot->SetAxisRange( 0, 0.8, "x");
    aj_lo_6_54_tot->SetLineColor( kBlack);
    aj_lo_8_54_tot->SetLineColor( kRed);
    aj_lo_6_54_tot->DrawNormalized();
    aj_lo_8_54_tot->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "Matched, p_{T}^{Cut}>0.2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( aj_lo_6_54_tot, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( aj_lo_8_54_tot, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "aj_lo_54_tot.png");

  } // drawtot

  if ( drawdef )  {
    // ---------------------------------------  
    // ------- Default, HT54, 25-35 ----------
    // ---------------------------------------  
    TFile *f6_54 = TFile::Open("AjResults/Groom_Aj_HT54_GeantMc_25_35.root");
    TFile *f8_54 = TFile::Open("AjResults/HT54_LargeEtaPythiaOnly_1_ptHat\=25_35.root");

    T6 = (TTree*) f6_54->Get("ResultTree");
    T8 = (TTree*) f8_54->Get("ResultTree");

    // --- aj_hi
    dopt= "aj_hi >> ";
    cond= "1*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

    TH1D* aj_hi_6_54 = new TH1D("aj_hi_6_54","#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV;|A_{J}|;a.u.",40, -0.3, 0.9);
    T6->Draw(   dopt + TString("aj_hi_6_54") , cond , "goff" );
    TH1D* aj_hi_8_54 = new TH1D("aj_hi_8_54","#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV;|A_{J}|;a.u.",40, -0.3, 0.9);
    T8->Draw(   dopt + TString("aj_hi_8_54") , cond , "goff" );

    new TCanvas;
    aj_hi_6_54->SetAxisRange( 0, 0.8, "x");
    aj_hi_8_54->SetAxisRange( 0, 0.8, "x");
    aj_hi_6_54->SetLineColor( kBlack);
    aj_hi_8_54->SetLineColor( kRed);
  
    aj_hi_6_54->DrawNormalized();
    aj_hi_8_54->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "p_{T}^{Cut}>2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( aj_hi_6_54, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( aj_hi_8_54, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "aj_hi_54.png");
  }  // drawdef
  
  if ( drawconsts )  {
    // ---------------------------------------  
    // ------- Default, HT54, 25-35 ----------
    // ---------------------------------------  
    TFile *f6_54 = TFile::Open("AjResults/Groom_Aj_HT54_GeantMc_25_35.root");
    TFile *f8_54 = TFile::Open("AjResults/HT54_LargeEtaPythiaOnly_1_ptHat\=25_35.root");

    TH1D* jm1consts_6_54 = ((TH3D*) f6_54->Get( "jm1ptphieta"))->ProjectionX("jm1consts_6_54");
    TH1D* jm1consts_8_54 = ((TH3D*) f8_54->Get( "jm1ptphieta"))->ProjectionX("jm1consts_8_54");
    jm1consts_6_54->SetTitle( "#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV; p_{T}^{const,1}");
    jm1consts_8_54->SetTitle( "#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV; p_{T}^{const,1}");

    new TCanvas;
    gPad->SetLogy();
    jm1consts_6_54->SetAxisRange( 0, 40, "x");
    jm1consts_8_54->SetAxisRange( 0, 40, "x");
    jm1consts_6_54->SetLineColor( kBlack);
    jm1consts_8_54->SetLineColor( kRed);
  
    jm1consts_8_54->DrawNormalized();
    jm1consts_6_54->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "Matched, p_{T}^{Cut}>0.2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( jm1consts_6_54, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( jm1consts_8_54, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "jm1consts.png");

    // ZOOM
    new TCanvas;
    gPad->SetLogy();
    jm1consts_6_54->SetAxisRange( 0, 6, "x");
    jm1consts_8_54->SetAxisRange( 0, 6, "x");
  
    jm1consts_8_54->DrawNormalized();
    jm1consts_6_54->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "Matched, p_{T}^{Cut}>0.2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( jm1consts_6_54, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( jm1consts_8_54, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "jm1consts_zoom.png");

    
  }  // drawconsts

  if ( drawmb )  {
    // ---------------------------------------  
    // ------------------ MB, 25-35 ----------
    // ---------------------------------------  
    TFile *f6_MB = TFile::Open("AjResults/Groom_Aj_MB_GeantMc_25_35.root");
    TFile *f8_MB = TFile::Open("AjResults/MB_LargeEtaPythiaOnly_1_ptHat\=25_35.root");

    T6 = (TTree*) f6_MB->Get("ResultTree");
    T8 = (TTree*) f8_MB->Get("ResultTree");

    // --- aj_hi
    dopt= "aj_hi >> ";
    cond= "1*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

    TH1D* aj_hi_6_MB = new TH1D("aj_hi_6_MB","#hat{p_{T}} = 25-35 GeV/c, MB;|A_{J}|;a.u.",40, -0.3, 0.9);
    T6->Draw(   dopt + TString("aj_hi_6_MB") , cond , "goff" );
    TH1D* aj_hi_8_MB = new TH1D("aj_hi_8_MB","#hat{p_{T}} = 25-35 GeV/c, MB;|A_{J}|;a.u.",40, -0.3, 0.9);
    T8->Draw(   dopt + TString("aj_hi_8_MB") , cond , "goff" );

    new TCanvas;
    aj_hi_6_MB->SetAxisRange( 0, 0.8, "x");
    aj_hi_8_MB->SetAxisRange( 0, 0.8, "x");
    aj_hi_6_MB->SetLineColor( kBlack);
    aj_hi_8_MB->SetLineColor( kRed);
  
    aj_hi_6_MB->DrawNormalized();
    aj_hi_8_MB->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "p_{T}^{Cut}>2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( aj_hi_6_MB, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( aj_hi_8_MB, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "aj_hi_MB.png");
  }   // drawmb

  if ( drawlowpt )  {
    // ---------------------------------------  
    // ------------------ LowPt, 25-35 ----------
    // ---------------------------------------  
    TFile *f6_LowPt = TFile::Open("AjResults/Groom_Aj_LowPt_GeantMc_25_35.root");
    TFile *f8_LowPt = TFile::Open("AjResults/LowPt_LargeEtaPythiaOnly_1_ptHat\=25_35.root");

    T6 = (TTree*) f6_LowPt->Get("ResultTree");
    T8 = (TTree*) f8_LowPt->Get("ResultTree");

    // --- aj_hi
    dopt= "aj_hi >> ";
    cond= "1*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

    TH1D* aj_hi_6_LowPt = new TH1D("aj_hi_6_LowPt","#hat{p_{T}} = 25-35 GeV/c, No HT, p_{T}^{Cut}>0.2 GeV/c;|A_{J}|;a.u.",40, -0.3, 0.9);
    T6->Draw(   dopt + TString("aj_hi_6_LowPt") , cond , "goff" );
    TH1D* aj_hi_8_LowPt = new TH1D("aj_hi_8_LowPt","#hat{p_{T}} = 25-35 GeV/c, No HT, p_{T}^{Cut}>0.2 GeV/c;|A_{J}|;a.u.",40, -0.3, 0.9);
    T8->Draw(   dopt + TString("aj_hi_8_LowPt") , cond , "goff" );

    new TCanvas;
    aj_hi_6_LowPt->SetAxisRange( 0, 0.8, "x");
    aj_hi_8_LowPt->SetAxisRange( 0, 0.8, "x");
    aj_hi_6_LowPt->SetLineColor( kBlack);
    aj_hi_8_LowPt->SetLineColor( kRed);
  
    aj_hi_6_LowPt->DrawNormalized();
    aj_hi_8_LowPt->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "NO matching, p_{T}^{Cut}>0.2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( aj_hi_6_LowPt, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( aj_hi_8_LowPt, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "aj_hi_LowPt.png");
  }   // drawlowpt

  if ( drawpt1 )  {
    // ---------------------------------------  
    // ------------------ Pt1, 25-35 ----------
    // ---------------------------------------  
    TFile *f6_Pt1 = TFile::Open("AjResults/Pt1_Groom_Aj_HT54_GeantMc_25_35.root");
    TFile *f8_Pt1 = TFile::Open("AjResults/Pt1_HT54_LargeEtaPythiaOnly_1_ptHat\=25_35.root");

    T6 = (TTree*) f6_Pt1->Get("ResultTree");
    T8 = (TTree*) f8_Pt1->Get("ResultTree");

    // --- aj_hi
    dopt= "aj_hi >> ";
    cond= "1*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

    TH1D* aj_hi_6_Pt1 = new TH1D("aj_hi_6_Pt1","#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV, p_{T}^{Const}>1.0 GeV/c;|A_{J}|;a.u.",40, -0.3, 0.9);
    T6->Draw(   dopt + TString("aj_hi_6_Pt1") , cond , "goff" );
    TH1D* aj_hi_8_Pt1 = new TH1D("aj_hi_8_Pt1","#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV, p_{T}^{Const}>1.0;|A_{J}|;a.u.",40, -0.3, 0.9);
    T8->Draw(   dopt + TString("aj_hi_8_Pt1") , cond , "goff" );

    new TCanvas;
    aj_hi_6_Pt1->SetAxisRange( 0, 0.8, "x");
    aj_hi_8_Pt1->SetAxisRange( 0, 0.8, "x");
    aj_hi_6_Pt1->SetLineColor( kBlack);
    aj_hi_8_Pt1->SetLineColor( kRed);
  
    aj_hi_6_Pt1->DrawNormalized();
    aj_hi_8_Pt1->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "p_{T}^{Cut}>2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( aj_hi_6_Pt1, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( aj_hi_8_Pt1, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "aj_hi_Pt1.png");
  }   // drawpt1

  if ( drawjetspectra )  {
    // ---------------------------------------------------  
    // ------- Jetspectra, default, HT54, 25-35 ----------
    // ---------------------------------------------------  
    TFile *f6_54 = TFile::Open("AjResults/Groom_Aj_HT54_GeantMc_25_35.root");
    TFile *f8_54 = TFile::Open("AjResults/HT54_LargeEtaPythiaOnly_1_ptHat\=25_35.root");

    T6 = (TTree*) f6_54->Get("ResultTree");
    T8 = (TTree*) f8_54->Get("ResultTree");

    // --- j1
    dopt= "j1.Pt() >> ";
    cond= "1*(abs(j1.Eta())<0.6 && abs(j2.Eta())<0.6 )";

    TH1D* j1_hi_6_54 = new TH1D("j1_hi_6_54","#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV;p_{T}^{lead};a.u.",100, 0, 50);
    T6->Draw(   dopt + TString("j1_hi_6_54") , cond , "goff" );
    TH1D* j1_hi_8_54 = new TH1D("j1_hi_8_54","#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV;p_{T}^{lead};a.u.",100, 0, 50);
    T8->Draw(   dopt + TString("j1_hi_8_54") , cond , "goff" );

    new TCanvas;
    gPad->SetLogy();
    j1_hi_6_54->SetAxisRange( 18, 50, "x");
    j1_hi_8_54->SetAxisRange( 18, 50, "x");
    j1_hi_6_54->SetLineColor( kBlack);
    j1_hi_8_54->SetLineColor( kRed);
  
    j1_hi_6_54->DrawNormalized();
    j1_hi_8_54->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "p_{T}^{Cut}>2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( j1_hi_6_54, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( j1_hi_8_54, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "j1_hi_54.png");

    // --- j2
    dopt= "j2.Pt() >> ";
    cond= "1*(abs(j2.Eta())<0.6 && abs(j2.Eta())<0.6 )";

    TH1D* j2_hi_6_54 = new TH1D("j2_hi_6_54","#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV;p_{T}^{sublead};a.u.",100, 0, 50);
    T6->Draw(   dopt + TString("j2_hi_6_54") , cond , "goff" );
    TH1D* j2_hi_8_54 = new TH1D("j2_hi_8_54","#hat{p_{T}} = 25-35 GeV/c, E_{T}>5.4 GeV;p_{T}^{sublead};a.u.",100, 0, 50);
    T8->Draw(   dopt + TString("j2_hi_8_54") , cond , "goff" );

    new TCanvas;
    gPad->SetLogy();
    j2_hi_6_54->SetAxisRange( 8, 40, "x");
    j2_hi_8_54->SetAxisRange( 8, 40, "x");
    j2_hi_6_54->SetLineColor( kBlack);
    j2_hi_8_54->SetLineColor( kRed);
  
    j2_hi_6_54->DrawNormalized();
    j2_hi_8_54->DrawNormalized("same");

    leg = new TLegend( 0.5, 0.65, 0.88, 0.88, "p_{T}^{Cut}>2 GeV/c" );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry ( j2_hi_6_54, "Pythia 6.410, CTEQ5L", "l");
    leg->AddEntry ( j2_hi_8_54, "Pythia 8.219, vanilla", "l");
    leg->Draw();
    gPad->SaveAs( plotpath + "j2_hi_54.png");



  }  // drawjetspectra


}
