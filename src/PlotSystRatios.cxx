{
  // TString inname = "AjResults/Systematics_Fresh_NicksList_HC100_ppInAuAuAj.root";
  TString inname = "AjResults/Systematics_R0.2_Fresh_NicksList_HC100_ppInAuAuAj.root";
  TFile* f = new TFile(inname,"READ");

  TString R="plots/R0.4_";
  if (inname.Contains("0.2") ) R="plots/R0.2_";
  
  TH1D* AJ_hi_T1_ratio = (TH1D*) f->Get("AJ_hi_T1_ratio");
  TH1D* AJ_hi_T2_ratio = (TH1D*) f->Get("AJ_hi_T2_ratio");
  TH1D* AJ_lo_T1_ratio = (TH1D*) f->Get("NoFabsAJ_lo_T1_ratio");
  TH1D* AJ_lo_T2_ratio = (TH1D*) f->Get("NoFabsAJ_lo_T2_ratio");

  TH1D* AJ_hi_E1_ratio = (TH1D*) f->Get("AJ_hi_E1_ratio");
  TH1D* AJ_hi_E2_ratio = (TH1D*) f->Get("AJ_hi_E2_ratio");
  TH1D* AJ_lo_E1_ratio = (TH1D*) f->Get("NoFabsAJ_lo_E1_ratio");
  TH1D* AJ_lo_E2_ratio = (TH1D*) f->Get("NoFabsAJ_lo_E2_ratio");

  new TCanvas;
  TLine* l = new TLine();
  TLegend* leg;
  l->SetLineStyle(7);

  // ----------------------------------------------------
  AJ_hi_T1_ratio->SetTitle( "" );
  // AJ_hi_T1_ratio->SetAxisRange(  0.7, 1.3, "y");
  // AJ_hi_T1_ratio->SetAxisRange(  0.0, 0.72, "x");
  AJ_hi_T1_ratio->SetAxisRange(  0.4, 1.6, "y");
  AJ_hi_T1_ratio->SetAxisRange(  -0.3, 0.8, "x");
  AJ_hi_T1_ratio->SetLineColor(kBlue);
  AJ_hi_T1_ratio->Draw();
  AJ_hi_T2_ratio->SetLineColor(kGreen+1);
  AJ_hi_T2_ratio->Draw("same");
  // l->DrawLine(0,1,0.77,1);
  l->DrawLine(-0.3,1,0.84,1);
  
  leg= new TLegend ( 0.2, 0.65, 0.4, 0.88, "p_{T}^{Cut}>2 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry( AJ_hi_T2_ratio, "Tow Scale Min", "l"); 
  leg->AddEntry( AJ_hi_T1_ratio, "Tow Scale Max", "l");
  leg->Draw("same");
  gPad->SaveAs(R+"TratioHi.png");
  
  // ----------------------------------------------------
  AJ_hi_E1_ratio->SetTitle( "" );
  // AJ_hi_E1_ratio->SetAxisRange(  0.7, 1.3, "y");
  // AJ_hi_E1_ratio->SetAxisRange(  0.0, 0.72, "x");
  AJ_hi_E1_ratio->SetAxisRange(  0.4, 1.6, "y");
  AJ_hi_E1_ratio->SetAxisRange(  -0.3, 0.8, "x");
  AJ_hi_E1_ratio->SetLineColor(kBlue);
  AJ_hi_E1_ratio->Draw();
  AJ_hi_E2_ratio->SetLineColor(kGreen+1);
  AJ_hi_E2_ratio->Draw("same");
  // l->DrawLine(0,1,0.77,1);
  l->DrawLine(-0.3,1,0.84,1);
  
  leg= new TLegend ( 0.2, 0.65, 0.4, 0.88, "p_{T}^{Cut}>2 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry( AJ_hi_E2_ratio, "Track Eff Min", "l"); 
  leg->AddEntry( AJ_hi_E1_ratio, "Track Eff Max", "l");
  leg->Draw("same");
  gPad->SaveAs(R+"EratioHi.png");

  // ----------------------------------------------------
  AJ_lo_T1_ratio->SetTitle( "" );
  // AJ_lo_T1_ratio->SetAxisRange(  0.7, 1.3, "y");
  // AJ_lo_T1_ratio->SetAxisRange(  0.0, 0.72, "x");
  AJ_lo_T1_ratio->SetAxisRange(  0.4, 1.6, "y");
  AJ_lo_T1_ratio->SetAxisRange(  -0.3, 0.8, "x");
  AJ_lo_T1_ratio->SetLineColor(kBlue);
  AJ_lo_T1_ratio->Draw();
  AJ_lo_T2_ratio->SetLineColor(kGreen+1);
  AJ_lo_T2_ratio->Draw("same");
  // l->DrawLine(0,1,0.77,1);
  l->DrawLine(-0.3,1,0.84,1);
  
  leg= new TLegend ( 0.2, 0.65, 0.4, 0.88, "p_{T}^{Cut}>0.2 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry( AJ_lo_T2_ratio, "Tow Scale Min", "l"); 
  leg->AddEntry( AJ_lo_T1_ratio, "Tow Scale Max", "l");
  leg->Draw("same");
  gPad->SaveAs(R+"TratioLo.png");
  
  // ----------------------------------------------------
  AJ_lo_E1_ratio->SetTitle( "" );
  // AJ_lo_E1_ratio->SetAxisRange(  0.7, 1.3, "y");
  // AJ_lo_E1_ratio->SetAxisRange(  0.0, 0.72, "x");
  AJ_lo_E1_ratio->SetAxisRange(  0.4, 1.6, "y");
  AJ_lo_E1_ratio->SetAxisRange(  -0.3, 0.8, "x");
  AJ_lo_E1_ratio->SetLineColor(kBlue);
  AJ_lo_E1_ratio->Draw();
  AJ_lo_E2_ratio->SetLineColor(kGreen+1);
  AJ_lo_E2_ratio->Draw("same");
  // l->DrawLine(0,1,0.77,1);
  l->DrawLine(-0.3,1,0.84,1);
  
  leg= new TLegend ( 0.2, 0.65, 0.4, 0.88, "p_{T}^{Cut}>0.2 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry( AJ_lo_E2_ratio, "Track Eff Min", "l"); 
  leg->AddEntry( AJ_lo_E1_ratio, "Track Eff Max", "l");
  leg->Draw("same");
  gPad->SaveAs(R+"EratioLo.png");


}
