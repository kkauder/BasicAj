{
  gStyle->SetOptStat(0);

  TFile* fPP = new TFile("AjResults/Tow0_Eff0_HP_ppInAuAuAj.root", "READ");
  TH2D* PPhdPtLead = fPP->Get("hdPtLead");
  PPhdPtLead->SetName("PPhdPtLead");
  TH2D* PPhdPtSubLead = fPP->Get("hdPtSubLead");
  PPhdPtSubLead->SetName("PPhdPtSubLead");

  TFile* fAuAu = new TFile("AjResults/HP_TEST.root", "READ");
  TH2D* AuAuhdPtLead = fAuAu->Get("hdPtLead");
  AuAuhdPtLead->SetName("AuAuhdPtLead");
  TH2D* AuAuhdPtSubLead = fAuAu->Get("hdPtSubLead");
  AuAuhdPtSubLead->SetName("AuAuhdPtSubLead");


  double refmultcut = 0;
  int binl = AuAuhdPtSubLead->GetYaxis()->FindBin(refmultcut);
  int binr = AuAuhdPtSubLead->GetNbinsY();

  TH1D* PPLead = PPhdPtLead->ProjectionX("PPLead",binl, binr);
  TH1D* PPSubLead = PPhdPtSubLead->ProjectionX("PPSubLead",binl, binr);
  
  TH1D* AuAuLead = AuAuhdPtLead->ProjectionX("AuAuLead",binl, binr);
  TH1D* AuAuSubLead = AuAuhdPtSubLead->ProjectionX("AuAuSubLead",binl, binr);

  PPLead->Rebin(4);
  PPSubLead->Rebin(4);
  AuAuLead->Rebin(4);
  AuAuSubLead->Rebin(4);

  char s[400];
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
    
  PPLead->SetLineColor(kRed);
  AuAuLead->DrawNormalized();
  PPLead->DrawNormalized("same");

  TLegend* l1 = new TLegend( 0.51, 0.64, 0.89, 0.8);
  sprintf( s, "pp@AuAu, <#Delta p_{T}> = %.2f  #pm  %.2f ", PPLead->GetMean(), PPLead->GetMeanError() );
  l1->AddEntry(PPLead, s, "l" );
  sprintf( s, "AuAu, <#Delta p_{T}> = %.2f  #pm  %.2f ", AuAuLead->GetMean(), AuAuLead->GetMeanError() );
  l1->AddEntry(AuAuLead, s, "l" );
  l1->Draw();
  gPad->SaveAs("plots/HP_DPtComparison_Lead.png");

  
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
    
  PPSubLead->SetLineColor(kRed);
  AuAuSubLead->DrawNormalized();
  PPSubLead->DrawNormalized("same");

  TLegend* l2 = new TLegend( 0.51, 0.64, 0.89, 0.8);
  sprintf( s, "pp@AuAu, <#Delta p_{T}> = %.2f  #pm  %.2f ", PPSubLead->GetMean(), PPSubLead->GetMeanError() );
  l2->AddEntry(PPSubLead, s, "l" );
  sprintf( s, "AuAu, <#Delta p_{T}> = %.2f  #pm  %.2f ", AuAuSubLead->GetMean(), AuAuSubLead->GetMeanError() );
  l2->AddEntry(AuAuSubLead, s, "l" );
  l2->Draw();
  gPad->SaveAs("plots/HP_DPtComparison_SubLead.png");

}
