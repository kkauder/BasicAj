{

  gStyle->SetOptStat(0);
  TCanvas* clo = new TCanvas("clo");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TCanvas* chi = new TCanvas("chi");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TFile* fAA = new TFile("AjResults/NPE18_HC100_AuAu.root");
  TH2D* h2 = (TH2D*) fAA->Get( "AJ_hi");
  int MultBinL = 1;
  int MultBinR = h2->GetNbinsY();
  TObjArray toa;

  // ==============================================================================================
  // TFile* fAA = new TFile("AjResults/NPE18_HC100_AuAu.root");
  TH1D* AAhi = (TH1D*) ( (TH2D*) fAA->Get( "AJ_hi") )->ProjectionX("AA_hi", MultBinL, MultBinR);
  AAhi->SetName("AAhi");
  AAhi->SetLineColor( kBlack );
  AAhi->SetMarkerColor( kBlack );
  AAhi->SetMarkerStyle( 20 );

  TH1D* AAlo = (TH1D*) ( (TH2D*) fAA->Get( "AJ_lo") )->ProjectionX("AA_lo", MultBinL, MultBinR);
  AAlo->SetName("AAlo");
  AAlo->SetLineColor( kBlack );
  AAlo->SetMarkerColor( kBlack );
  AAlo->SetMarkerStyle( 20 );

  TString AAtitle = "Year 11 NPE18, No Cuts";
  toa.Add(AAlo);
  toa.Add(AAhi);

  // ==============================================================================================
  TFile* fBB = new TFile("AjResults/Soft6_NPE18_HC100_AuAu.root");
  TH1D* BBhi = (TH1D*) ( (TH2D*) fBB->Get( "AJ_hi") )->ProjectionX("BB_hi", MultBinL, MultBinR);
  BBhi->SetName("BBhi");
  BBhi->SetLineColor( kBlue );
  BBhi->SetMarkerColor( kBlue );
  BBhi->SetMarkerStyle( 20 );

  TH1D* BBlo = (TH1D*) ( (TH2D*) fBB->Get( "AJ_lo") )->ProjectionX("BB_lo", MultBinL, MultBinR);
  BBlo->SetName("BBlo");
  BBlo->SetLineColor( kBlue );
  BBlo->SetMarkerColor( kBlue );
  BBlo->SetMarkerStyle( 20 );

  TString BBtitle = "Year 11 NPE18, Require 6 GeV tower";
  toa.Add(BBlo);
  toa.Add(BBhi);

  // ==============================================================================================
  TFile* fCC = new TFile("AjResults/NoBadRefmult_Soft6_NPE18_HC100_AuAu.root");
  TH1D* CChi = (TH1D*) ( (TH2D*) fCC->Get( "AJ_hi") )->ProjectionX("CC_hi", MultBinL, MultBinR);
  CChi->SetName("CChi");
  CChi->SetLineColor( kGreen+1 );
  CChi->SetMarkerColor( kGreen+1 );
  CChi->SetMarkerStyle( 20 );

  TH1D* CClo = (TH1D*) ( (TH2D*) fCC->Get( "AJ_lo") )->ProjectionX("CC_lo", MultBinL, MultBinR);
  CClo->SetName("CClo");
  CClo->SetLineColor( kGreen+1 );
  CClo->SetMarkerColor( kGreen+1 );
  CClo->SetMarkerStyle( 20 );

  TString CCtitle = "Year 11 NPE18, 6 GeV tower, Runs cut";
  toa.Add(CClo);
  toa.Add(CChi);

  // ==============================================================================================
  TFile* fDD = new TFile("AjResults/NoBadRefmult_PhiCut_Soft6_NPE18_HC100_AuAu.root");
  TH1D* DDhi = (TH1D*) ( (TH2D*) fDD->Get( "AJ_hi") )->ProjectionX("DD_hi", MultBinL, MultBinR);
  DDhi->SetName("DDhi");
  DDhi->SetLineColor( kRed );
  DDhi->SetMarkerColor( kRed );
  DDhi->SetMarkerStyle( 20 );

  TH1D* DDlo = (TH1D*) ( (TH2D*) fDD->Get( "AJ_lo") )->ProjectionX("DD_lo", MultBinL, MultBinR);
  DDlo->SetName("DDlo");
  DDlo->SetLineColor( kRed );
  DDlo->SetMarkerColor( kRed );
  DDlo->SetMarkerStyle( 20 );

  TString DDtitle = "Year 11 NPE18, 6 GeV tower, Runs cut && #phi cut";
  toa.Add(DDlo);
  toa.Add(DDhi);
  
  // ==============================================================================================
  TFile* fEE = new TFile("AjResults/PhiCut_Soft6_NPE18_HC100_AuAu.root");
  TH1D* EEhi = (TH1D*) ( (TH2D*) fEE->Get( "AJ_hi") )->ProjectionX("EE_hi", MultBinL, MultBinR);
  EEhi->SetName("EEhi");
  EEhi->SetLineColor( kCyan );
  EEhi->SetMarkerColor( kCyan );
  EEhi->SetMarkerStyle( 20 );

  TH1D* EElo = (TH1D*) ( (TH2D*) fEE->Get( "AJ_lo") )->ProjectionX("EE_lo", MultBinL, MultBinR);
  EElo->SetName("EElo");
  EElo->SetLineColor( kCyan );
  EElo->SetMarkerColor( kCyan );
  EElo->SetMarkerStyle( 20 );

  TString EEtitle = "Year 11 NPE18, 6 GeV tower, #phi cut";
  toa.Add(EElo);
  toa.Add(EEhi);

  // ==============================================================================================
  TFile* fNN = new TFile("AjResults/NicksList_HC100_AuAu.root");
  // TFile* fNN = new TFile("AjResults/Soft6_NicksList_HC100_AuAu.root");
  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  h2 = (TH2D*) fNN->Get( "AJ_lo");
  int NNMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int NNMultBinR = h2->GetNbinsY();

  TH1D* NNhi = (TH1D*) ( (TH2D*) fNN->Get( "AJ_hi") )->ProjectionX("NN_hi", NNMultBinL, NNMultBinR);
  NNhi->SetName("NNhi");
  NNhi->SetLineColor( kGray+2 );
  NNhi->SetMarkerColor( kGray+2 );
  NNhi->SetMarkerStyle( 20 );

  TH1D* NNlo = (TH1D*) ( (TH2D*) fNN->Get( "AJ_lo") )->ProjectionX("NN_lo", NNMultBinL, NNMultBinR);
  NNlo->SetName("NNlo");
  NNlo->SetLineColor( kGray+2 );
  NNlo->SetMarkerColor( kGray+2 );
  NNlo->SetMarkerStyle( 20 );

  TString NNtitle = "Year 7 L2gamma, No cuts";
  toa.Add(NNlo);
  toa.Add(NNhi);

  // ==============================================================================================

  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");
    
    h->Rebin(2);
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.14, "y");
    h->SetAxisRange(  0.0, 0.23, "y");
    h->SetAxisRange(  0.0, 0.72, "x");
  }

  // ==============================================================================================
  AAhi->SetTitle("0-20%, p_{T}^{Cut}>2 GeV/c");
  AAhi->GetYaxis()->SetTitleSize(0.04);
  AAhi->GetYaxis()->SetLabelSize(0.04);
  AAhi->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  AAhi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  AAhi->GetXaxis()->SetTitleSize(0.04);
  AAhi->GetXaxis()->SetLabelSize(0.04);
  AAhi->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  AAhi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  AAhi->GetYaxis()->SetTitleOffset(1.14);

  chi->cd();
  // AAhi->Draw("9");
  // BBhi->Draw("9same");
  // CChi->Draw("9same");
  // DDhi->Draw("9same");
  // EEhi->Draw("9same");
  DDhi->Draw("9");
  NNhi->Draw("9histsame");
  EEhi->Draw("9same");
  CChi->Draw("9same");
  BBhi->Draw("9same");
  AAhi->Draw("9same");


  TLegend* leghi = new TLegend( 0.51, 0.46, 0.89, 0.9, "100% had. corr." );
  leghi->SetBorderSize(0);
  leghi->SetLineWidth(10);
  leghi->SetFillStyle(0);
  leghi->SetMargin(0.1);
  char plabel[500];

  leghi->AddEntry ( AAhi, AAtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", AAhi->KolmogorovTest(AAhi, "") );
  leghi->AddEntry ( (TObject*) 0, plabel, "");

  leghi->AddEntry ( BBhi, BBtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", BBhi->KolmogorovTest(AAhi, "") );
  leghi->AddEntry ( (TObject*) 0, plabel, "");

  leghi->AddEntry ( CChi, CCtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", CChi->KolmogorovTest(AAhi, "") );
  leghi->AddEntry ( (TObject*) 0, plabel, "");

  leghi->AddEntry ( DDhi, DDtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", DDhi->KolmogorovTest(AAhi, "") );
  leghi->AddEntry ( (TObject*) 0, plabel, "");

  leghi->AddEntry ( EEhi, EEtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", EEhi->KolmogorovTest(AAhi, "") );
  leghi->AddEntry ( (TObject*) 0, plabel, "");
  leghi->Draw();

  leghi->AddEntry ( NNhi, NNtitle, "l");
  sprintf ( plabel, "p-value = %0.2g", NNhi->KolmogorovTest(AAhi, "") );
  leghi->AddEntry ( (TObject*) 0, plabel, "");
  leghi->Draw();

  gPad->SaveAs("y11_Hi_Overlay.png");
  // ==============================================================================================


  AAlo->SetTitle("0-20%, p_{T}^{Cut}>0.2 GeV/c, matched");
  AAlo->GetYaxis()->SetTitleSize(0.04);
  AAlo->GetYaxis()->SetLabelSize(0.04);
  AAlo->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  AAlo->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  AAlo->GetXaxis()->SetTitleSize(0.04);
  AAlo->GetXaxis()->SetLabelSize(0.04);
  AAlo->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  AAlo->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  AAlo->GetYaxis()->SetTitleOffset(1.14);

  clo->cd();
  // AAlo->Draw("9");
  // BBlo->Draw("9same");
  // CClo->Draw("9same");
  // DDlo->Draw("9same");
  // EElo->Draw("9same");
  DDlo->Draw("9");
  NNlo->Draw("9histsame");
  EElo->Draw("9same");
  CClo->Draw("9same");
  BBlo->Draw("9same");
  AAlo->Draw("9same");

  char plabel[500];

  TLegend* leglo = new TLegend( 0.51, 0.46, 0.89, 0.9, "100% had. corr." );
  leglo->SetBorderSize(0);
  leglo->SetLineWidth(10);
  leglo->SetFillStyle(0);
  leglo->SetMargin(0.1);

  leglo->AddEntry ( AAlo, AAtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", AAlo->KolmogorovTest(AAlo, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");

  leglo->AddEntry ( BBlo, BBtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", BBlo->KolmogorovTest(AAlo, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");

  leglo->AddEntry ( CClo, CCtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", CClo->KolmogorovTest(AAlo, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");

  leglo->AddEntry ( DDlo, DDtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", DDlo->KolmogorovTest(AAlo, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");

  leglo->AddEntry ( EElo, EEtitle, "p");
  sprintf ( plabel, "p-value = %0.2g", EElo->KolmogorovTest(AAlo, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");
  leglo->Draw();

  leglo->AddEntry ( NNlo, NNtitle, "l");
  sprintf ( plabel, "p-value = %0.2g", NNlo->KolmogorovTest(AAlo, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");
  leglo->Draw();

  gPad->SaveAs("y11_Lo_Overlay.png");
  // ==============================================================================================



  // TPaveText* msglo = new TPaveText( 0.59, 0.38, 0.89, 0.63, "brNDC" );
  // msglo->AddText( "p-value compared to no cuts:" );
  // msglo->AddText( plabel );
  // msglo->Draw();



}
