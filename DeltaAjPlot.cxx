{
  gStyle->SetOptStat(0);

  TFile *fAuAu         = TFile::Open("AjResults/ppFollowAj.root"); // DEBUG
  // TFile *fAuAu         = TFile::Open("AjResults/FollowAuAuAj.root");
  // TFile *fppInAuAu     = TFile::Open("AjResults/ppInAuAuFollowAj.root");

  TH1D* AuAuDeltaAJ_hi     = (TH1D*) fAuAu->Get( "DeltaAJ_hi");
  AuAuDeltaAJ_hi->SetName("AuAuDeltaAJ_hi");
  TH1D* AuAuDeltaAJ_lo     = (TH1D*) fAuAu->Get( "DeltaAJ_lo");
  AuAuDeltaAJ_lo->SetName("AuAuDeltaAJ_lo");
  
  TH1D* AuAuSmallDeltaAJ_hilo     = (TH1D*) fAuAu->Get( "SmallDeltaAJ_hilo");
  AuAuSmallDeltaAJ_hilo->SetName("AuAuSmallDeltaAJ_hilo");
  TH1D* AuAuLargeDeltaAJ_hilo     = (TH1D*) fAuAu->Get( "LargeDeltaAJ_hilo");
  AuAuLargeDeltaAJ_hilo->SetName("AuAuLargeDeltaAJ_hilo");

  TCanvas* c1 = new TCanvas("c1");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg1 = new TLegend( 0.55, 0.7, 0.89, 0.89, "A_{J}(Small R) - A_{J}(Large R)" );
  leg1->SetBorderSize(0);
  leg1->SetLineWidth(10);
  leg1->SetFillStyle(0);
  leg1->SetMargin(0.1);


  AuAuDeltaAJ_hi->SetTitle("");
  AuAuDeltaAJ_hi->SetAxisRange(-0.4, 0.5,"x");
  AuAuDeltaAJ_hi->SetLineColor(kRed);
  AuAuDeltaAJ_hi->Draw("9");
  AuAuDeltaAJ_lo->SetLineColor(kBlack);
  AuAuDeltaAJ_lo->Draw("9same");

  leg1->AddEntry(AuAuDeltaAJ_hi, "pp, p_{T}^{Cut}>2 GeV/c", "l");
  leg1->AddEntry(AuAuDeltaAJ_lo, "pp, p_{T}^{Cut}>0.2 GeV/c", "l");
  leg1->Draw();
  gPad->SaveAs("tmp1.png");
  

  // ------------------------------------------------

  TCanvas* c2 = new TCanvas("c2");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg2 = new TLegend( 0.55, 0.7, 0.89, 0.89, "A_{J}(2 GeV) - A_{J}(0.2 GeV)" );
  leg2->SetBorderSize(0);
  leg2->SetLineWidth(10);
  leg2->SetFillStyle(0);
  leg2->SetMargin(0.1);

  AuAuSmallDeltaAJ_hilo->SetTitle("");
  AuAuSmallDeltaAJ_hilo->SetAxisRange(-0.4, 0.5,"x");

  AuAuSmallDeltaAJ_hilo->SetLineColor(kBlack);
  AuAuSmallDeltaAJ_hilo->Draw("9");
  AuAuLargeDeltaAJ_hilo->SetLineColor(kGreen+1);
  AuAuLargeDeltaAJ_hilo->Draw("9same");

  AuAuDeltaAJ_lo->SetLineColor(kBlack);
  AuAuDeltaAJ_lo->Draw("9same");

  leg2->AddEntry(AuAuSmallDeltaAJ_hilo, "pp, R=0.2", "l");
  leg2->AddEntry(AuAuLargeDeltaAJ_hilo, "pp, R=0.4, matched", "l");
  leg2->Draw();
  gPad->SaveAs("tmp2.png");  

}
