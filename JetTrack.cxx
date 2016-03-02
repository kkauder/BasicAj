{
  gStyle->SetOptStat(0);

  TFile *f         = TFile::Open("AjResults/Fresh_NicksList_HC100_AuAu.root");

  TH1D* hJetHadronHiLead = (TH1D*) f->Get( "hJetHadronHiLead");
  TH1D* hJetHadronHiSubLead = (TH1D*) f->Get( "hJetHadronHiSubLead");
  TH1D* hJetHadronLoLead = (TH1D*) f->Get( "hJetHadronLoLead");
  TH1D* hJetHadronLoSubLead = (TH1D*) f->Get( "hJetHadronLoSubLead");


  hJetHadronHiLead->SetTitle("");
  hJetHadronHiLead->SetLineColor( kRed );
  hJetHadronHiSubLead->SetTitle("");
  hJetHadronHiSubLead->SetLineColor( kBlue );
  
  hJetHadronLoLead->SetTitle("");
  hJetHadronLoLead->SetLineColor( kRed );
  hJetHadronLoSubLead->SetTitle("");
  hJetHadronLoSubLead->SetLineColor( kBlue );

  hJetHadronLoLead->Rebin(2);
  hJetHadronLoSubLead->Rebin(2);

  // ============================== Hi pT ==============================
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  hJetHadronHiLead->Draw();
  hJetHadronHiSubLead->Draw("same");

  TLegend* leg = new TLegend( 0.53, 0.7, 0.89, 0.9, "AuAu, charged tracks, p_{T}>2 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( hJetHadronHiLead,     "#phi^{charged} - #phi^{Jet},  Leading Jet", "L");
  leg->AddEntry ( hJetHadronHiSubLead,  "#phi^{charged} - #phi^{Jet},  SubLeading Jet", "L");
  leg->Draw();
  gPad->SaveAs("plots/AuAuJetHadronHi.png");

  // ============================== Lo pT ==============================
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  hJetHadronLoLead->Draw();
  hJetHadronLoSubLead->Draw("same");

  TLegend* leg = new TLegend( 0.53, 0.7, 0.89, 0.9, "AuAu, charged tracks, p_{T}>0.2 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( hJetHadronLoLead,     "#phi^{charged} - #phi^{Jet},  Leading Jet", "L");
  leg->AddEntry ( hJetHadronLoSubLead,  "#phi^{charged} - #phi^{Jet},  SubLeading Jet", "L");
  leg->Draw();
  gPad->SaveAs("plots/AuAuJetHadronLo.png");

}
