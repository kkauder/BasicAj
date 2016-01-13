{

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TFile* fAA = new TFile("AjResults/NicksList_HC100_AuAu.root");
  // TFile* fAA = new TFile("AjResults/Soft6_NicksList_HC100_AuAu.root");
  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  TH2D* h2 = (TH2D*) fAA->Get( "AJ_lo");
  int AAMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AAMultBinR = h2->GetNbinsY();

  TH1D* AAhi = (TH1D*) ( (TH2D*) fAA->Get( "AJ_hi") )->ProjectionX("AA_hi", AAMultBinL, AAMultBinR);
  AAhi->SetName("AAhi");
  AAhi->SetLineColor( kRed );
  AAhi->SetMarkerColor( kRed );
  AAhi->SetMarkerStyle( 20 );

  TH1D* AAlo = (TH1D*) ( (TH2D*) fAA->Get( "AJ_lo") )->ProjectionX("AA_lo", AAMultBinL, AAMultBinR);
  AAlo->SetName("AAlo");
  AAlo->SetLineColor( kBlack );
  AAlo->SetMarkerColor( kBlack );
  AAlo->SetMarkerStyle( 20 );

  int BBMultBinL = 1;
  int BBMultBinR = h2->GetNbinsY();

  TFile* fBB = new TFile("AjResults/NPE18_HC100_AuAu.root");
  // TFile* fBB = new TFile("AjResults/Soft6_NPE18_HC100_AuAu.root");
  TH1D* BBhi = (TH1D*) ( (TH2D*) fBB->Get( "AJ_hi") )->ProjectionX("BB_hi", BBMultBinL, BBMultBinR);
  BBhi->SetName("BBhi");
  BBhi->SetLineColor( kGreen+1 );
  BBhi->SetMarkerColor( kGreen+1 );
  BBhi->SetMarkerStyle( 20 );

  TH1D* BBlo = (TH1D*) ( (TH2D*) fBB->Get( "AJ_lo") )->ProjectionX("BB_lo", BBMultBinL, BBMultBinR);
  BBlo->SetName("BBlo");
  BBlo->SetLineColor( kBlue );
  BBlo->SetMarkerColor( kBlue );
  BBlo->SetMarkerStyle( 20 );
  
  int nAA = AAhi->Integral();
  int nBB = BBhi->Integral();

  TObjArray toa;
  toa.Add(AAlo);
  toa.Add(AAhi);
  toa.Add(BBlo);
  toa.Add(BBhi);
  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");
    
    h->SetTitle("0-20%");
    // h->Rebin(2);
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.14, "y");
    // h->SetAxisRange(  0.0, 0.23, "y");
    h->SetAxisRange(  0.0, 0.72, "x");
  }

  AAhi->GetYaxis()->SetTitleSize(0.04);
  AAhi->GetYaxis()->SetLabelSize(0.04);
  AAhi->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  AAhi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  AAhi->GetXaxis()->SetTitleSize(0.04);
  AAhi->GetXaxis()->SetLabelSize(0.04);
  AAhi->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  AAhi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  AAhi->GetYaxis()->SetTitleOffset(1.14);
  AAhi->Draw("9");

  AAlo->Draw("9same");
  BBhi->Draw("9same");
  BBlo->Draw("9same");

  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "100% had. corr." );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  leg->AddEntry ( AAhi,     "Year  7 AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( BBhi,     "Year 11 AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( AAlo,     "Year  7 AuAu HT, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( BBlo,     "Year 11 AuAu HT, p_{T}^{Cut}>0.2 GeV/c", "p");

  leg->Draw();


  TPaveText* msg = new TPaveText( 0.59, 0.38, 0.89, 0.63, "brNDC" );
  TString s;
  msg->AddText("No further cuts");
  // msg->AddText("Require tower>6 GeV");
  // msg->AddText("Rejecting \"bad\" runs");
  // msg->AddText("Rejecting #phi strip");

  s = " - Run 7: #dijets = "; s+=nAA;
  msg->AddText(s);
  s = " - Run 11: #dijets = "; s+=nBB;
  msg->AddText(s);
  s = " Factor ~ "; s+= int ( float(nBB) / float(nAA) +0.5 );
  msg->AddText(s);

  msg->Draw();

  gPad->SaveAs("y7_y11_Nominal.gif");


}
