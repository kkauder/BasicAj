{
  TFile *_file0 = TFile::Open("AjResults/HC30_Presel.root");


  new TCanvas;
  hj1area->SetLineColor(kBlack);
  hj1area->SetTitle(";A [sr];counts");
  hj1area->Draw();
  hj2area->SetLineColor(kRed);
  hj2area->Draw("same");

  TLegend* leg = new TLegend ( 0.2, 0.7, 0.4, 0.82 );
  leg->AddEntry(hj1area, "Leading Jet", "l");
  leg->AddEntry(hj2area, "Sub-leading Jet", "l");
  
  leg->Draw();
  gPad->SaveAs("AuAuArea.png");

  new TCanvas;
  hrho->SetLineColor(kBlack);
  hrho->SetTitle(";#rho [(GeV/c)/sr];counts");
  hrho->Draw();
  
  gPad->SaveAs("AuAuRho.png");

}
