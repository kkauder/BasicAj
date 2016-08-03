{
  //gROOT->ForceStyle();
  gStyle->SetOptStat(0);

  // TFile* fAA = new TFile("hAuAuLead.root");
  // TFile* fAA = new TFile("hAuAuLeadTop20.root");
  // TH1D* hAuAuLead = fAA->Get("hAuAuLead");
  // TFile* fAA = new TFile("hAuAuInc.root");
  // TH1D* hAuAuLead = fAA->Get("hLeadPtInTop20");
  TFile* fAA = new TFile("hAuAuSubLeadTop20.root");
  TH1D* hAuAuLead = fAA->Get("hSubLeadPtInTop20");
  hAuAuLead->SetLineColor(kBlue);

  // TFile* fpp = new TFile("hEmbLead.root");
  // TFile* fpp = new TFile("hEmbLeadTop20.root");  
  // TH1D* hEmbLead = fpp->Get("hEmbLead");
  // TFile* fpp = new TFile("hEmbInc.root");  
  // TH1D* hEmbLead = fpp->Get("hEmbInc");
  TFile* fpp = new TFile("hEmbSubLeadTop20.root");  
  TH1D* hEmbLead = fpp->Get("hSubLeadPtInTop20");

  hEmbLead->SetLineColor(kRed);
  hEmbLead->SetTitle(";p_{T}^{Jet};counts");
  
  // int n20 = hIncLead->GetXaxis()->FindBin(20 + 0.01);
  // hIncLead->Scale ( 1./hIncLead->GetBinContent(n20) );
  int n20 = hEmbLead->GetXaxis()->FindBin(23 + 0.01);
  hEmbLead->Scale ( 1./hEmbLead->GetBinContent(n20) );
  hAuAuLead->Scale ( 1./hAuAuLead->GetBinContent(n20) );

  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  // hIncLead->Draw();
  // hEmbLead->SetAxisRange( 19,50,"x");
  hAuAuLead->Draw();
  hEmbLead->Draw("same");
  // hAuAuLead->Draw("same");

  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "0-20%" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  // leg->AddEntry( hIncLead, "pp HT, inclusive leading jet", "L");
  // leg->AddEntry( hEmbLead, "pp HT #oplus AuAu MB, \"leading di-jet\"", "L");
  // leg->AddEntry( hAuAuLead, "AuAu HT, \"leading di-jet\"", "L");

  leg->AddEntry( hEmbLead, "pp HT #oplus AuAu MB, inclusive leading jet", "L");
  leg->AddEntry( hAuAuLead, "AuAu HT, inclusive leading jet", "L");

  
  leg->Draw();

  gPad->SaveAs("CompareJetSpectra_LeadingOnly.png");


  // Ratio
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TH1D* r = (TH1D*) hAuAuLead->Clone("r");
  TH1D* d = (TH1D*) hEmbLead->Clone("d");
  r->Rebin(4);
  r->Scale(1./r->Integral());
  d->Rebin(4);
  d->Scale(1./d->Integral());
  r->Divide(d);
  r->Draw("hist");
  gPad->SaveAs("CompareJetSpectra_LeadingOnly_Ratio.png");
  
  
}
