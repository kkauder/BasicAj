{
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);
  gStyle->SetOptFit(1);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleBorderSize(0);
  // TCanvas* c = new TCanvas("c");
  //   gPad->SetGridx(0);  gPad->SetGridy(0);

  TFile *_file0 = TFile::Open("Y7PhiEtaPt.root");
  TH3D* Y7 = (TH3D*) ptphieta->Clone("Y7");
  TFile *_file1 = TFile::Open("Y11PhiEtaPt.root");
  TH3D* Y11 = (TH3D*) ptphieta->Clone("Y11");

  Y7->SetStats(0);
  Y11->SetStats(0);
  
  Y7->SetMarkerColor(kRed);
  Y7->SetMarkerStyle( 25 );
  Y7->SetLineColor(kRed);
  Y11->SetMarkerColor(kGreen+1);
  Y11->SetMarkerStyle( 21 );
  Y11->SetLineColor(kGreen+1);
  

  TH1D* pt7  = Y7->ProjectionX("pt7");
  TH1D* pt11 = Y11->ProjectionX("pt11");

  float lowpt = 0.6;
  int lowptbin = pt7->FindBin( lowpt - 0.001 );
  float highpt = 1.0;
  int highptbin = pt7->FindBin( highpt + 0.001 );
  int nptbins = pt7->GetNbinsX();

  float lphi = -1;
  int lphibin = Y7->GetYaxis()->FindBin( lphi - 0.001 );
  float rphi = 0;
  int rphibin = Y7->GetYaxis()->FindBin( rphi + 0.001 );
  int nphibins = Y7->GetNbinsY();

  // =====================================================
  // ================ Low pt histos: =====================
  // =====================================================
  // ================ pT ====================
  Y7 ->GetXaxis()->SetRange( 1, lowptbin ); 
  Y11->GetXaxis()->SetRange( 1, lowptbin ); 
  TH1D* lowpt7  = Y7->Project3D("x"); lowpt7->SetName("lowpt7");
  lowpt7->SetTitle ("p_{T}<0.6 tracks;p_{T};tracks per event");
  TH1D* lowpt11  = Y11->Project3D("x"); lowpt11->SetName("lowpt11");
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  lowpt7->DrawCopy("9");
  lowpt11->DrawCopy("9same");

  TLegend* leglo = new TLegend( 0.6, 0.73, 0.89, 0.9, "" );
  leglo->SetBorderSize(0);
  leglo->SetLineWidth(10);
  leglo->SetFillStyle(0);
  // leglo->SetMargin(0.1);
  leglo->AddEntry ( lowpt7, "Year 7", "lp");
  leglo->AddEntry ( lowpt11, "Year 11", "lp");
  leglo->Draw();
  gPad->SaveAs("lowptMB.pdf(");


  TH1D* lowptratio = lowpt11->Clone("ptratio");
  lowptratio->SetTitle("p_{T}<0.6, Ratio;p_{T};Year 11 / Year7");
  lowptratio->Divide(lowpt7);
  lowptratio->SetAxisRange(0.7,1.0,"y");
  lowptratio->SetMarkerColor(kBlack);
  lowptratio->SetMarkerStyle( 20 );
  lowptratio->SetLineColor(kBlack);
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  lowptratio->Draw("9");
  gPad->SaveAs("lowptMB.pdf");

  // ================ pT with strip removed ====================
  Y7 ->GetYaxis()->SetRange( 1, lphibin-1 ); 
  TH1D* VarLowpt7  = Y7->Project3D("x"); VarLowpt7->SetName("VarLowpt7");
  VarLowpt7->SetTitle ("p_{T}<0.6 tracks, bad #phi excluded;p_{T};tracks per event");
  Y7 ->GetYaxis()->SetRange( rphibin+1, nphibins ); 
  TH1D* SecondVarLowpt7  = Y7->Project3D("x"); SecondVarLowpt7->SetName("SecondVarLowpt7");
  VarLowpt7->Add(SecondVarLowpt7);

  Y11 ->GetYaxis()->SetRange( 1, lphibin-1 ); 
  TH1D* VarLowpt11  = Y11->Project3D("x"); VarLowpt11->SetName("VarLowpt11");
  VarLowpt11->SetTitle ("p_{T}<0.6 tracks, bad #phi excluded;p_{T};tracks per event");
  Y11 ->GetYaxis()->SetRange( rphibin+1, nphibins ); 
  TH1D* SecondVarLowpt11  = Y11->Project3D("x"); SecondVarLowpt11->SetName("SecondVarLowpt11");
  VarLowpt11->Add(SecondVarLowpt11);

  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  VarLowpt7->DrawCopy("9");
  VarLowpt11->DrawCopy("9same");

  TLegend* vleglo = new TLegend( 0.6, 0.73, 0.89, 0.9, "" );
  vleglo->SetBorderSize(0);
  vleglo->SetLineWidth(10);
  vleglo->SetFillStyle(0);
  vleglo->AddEntry ( VarLowpt7, "Year 7", "lp");
  vleglo->AddEntry ( VarLowpt11, "Year 11", "lp");
  vleglo->Draw();
  gPad->SaveAs("lowptMB.pdf");


  TH1D* VarLowptratio = VarLowpt11->Clone("VarLowptratio");
  VarLowptratio->SetTitle("p_{T}<0.6, Ratio, bad #phi excluded;p_{T};Year 11 / Year7");
  VarLowptratio->Divide(VarLowpt7);
  VarLowptratio->SetAxisRange(0.7,1.0,"y");
  VarLowptratio->SetMarkerColor(kBlack);
  VarLowptratio->SetMarkerStyle( 20 );
  VarLowptratio->SetLineColor(kBlack);
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  VarLowptratio->Draw("9");
  gPad->SaveAs("lowptMB.pdf");

  Y7 ->GetYaxis()->SetRange( 1, nphibins ); 
  Y11 ->GetYaxis()->SetRange( 1, nphibins ); 

  // ================ phi eta ====================

  TH2D* lowphieta7  = (TH2D*) Y7->Project3D("zy"); lowphieta7->SetName("lowphieta7");
  lowphieta7->SetTitle ("p_{T}<0.6 tracks, Year 7;#phi;#eta");
  // lowphieta7->Rebin2D(2,2);
  // lowphieta7->SetAxisRange(0, 0.18, "z");
  lowphieta7->SetAxisRange(0, 0.045, "z");

  TH2D* lowphieta11 = (TH2D*) Y11->Project3D("zy"); lowphieta11->SetName("lowphieta11");
  lowphieta11->SetTitle ("p_{T}<0.6 tracks, Year 11;#phi;#eta");
  // lowphieta11->Rebin2D(2,2);
  // lowphieta11->SetAxisRange(0, 0.18, "z");
  lowphieta11->SetAxisRange(0, 0.045, "z");

  new TCanvas;  
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetRightMargin( 0.15);
  lowphieta7->DrawCopy("colz");
  gPad->SaveAs("lowptMB.pdf");

  new TCanvas;  
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetRightMargin( 0.15);
  lowphieta11->DrawCopy("colz");
  gPad->SaveAs("lowptMB.pdf");

  // Rebin to get rid of fluctuations
  lowphieta7->Rebin2D(2,2);
  lowphieta11->Rebin2D(2,2);


  TH1D* lowphietaratio = lowphieta11->Clone("phietaratio");
  lowphietaratio->SetTitle("p_{T}<0.6, Ratio Year 11 / Year7;#phi;#eta");
  lowphietaratio->SetAxisRange( 0, 2.1, "z");
  lowphietaratio->Divide(lowphieta7);
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  lowphietaratio->Draw("colz9");
  gPad->SaveAs("lowptMB.pdf)");


  // =====================================================
  // ================ High pt histos: ====================
  // =====================================================
  // ================ pT ====================
  Y7 ->GetXaxis()->SetRange( highptbin, nptbins ); 
  Y11->GetXaxis()->SetRange( highptbin, nptbins ); 
  TH1D* highpt7  = Y7->Project3D("x"); highpt7->SetName("highpt7");
  highpt7->SetTitle ("p_{T}>1.0 tracks;p_{T};tracks per event");
  TH1D* highpt11  = Y11->Project3D("x"); highpt11->SetName("highpt11");
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  highpt7->DrawCopy("9");
  highpt11->DrawCopy("9same");

  TLegend* leglo = new TLegend( 0.6, 0.73, 0.89, 0.9, "" );
  leglo->SetBorderSize(0);
  leglo->SetLineWidth(10);
  leglo->SetFillStyle(0);
  // leglo->SetMargin(0.1);
  leglo->AddEntry ( highpt7, "Year 7", "lp");
  leglo->AddEntry ( highpt11, "Year 11", "lp");
  leglo->Draw();
  gPad->SaveAs("highptMB.pdf(");


  TH1D* highptratio = highpt11->Clone("ptratio");
  highptratio->SetTitle("p_{T}>1.0, Ratio;p_{T};Year 11 / Year7");
  highptratio->Divide(highpt7);
  highptratio->SetAxisRange(0.7,1.0,"y");
  highptratio->SetMarkerColor(kBlack);
  highptratio->SetMarkerStyle( 20 );
  highptratio->SetLineColor(kBlack);
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  highptratio->Fit("pol0");
  highptratio->Draw("9");
  gPad->SaveAs("highptMB.pdf");

  // ================ pT with strip removed ====================
  Y7 ->GetYaxis()->SetRange( 1, lphibin-1 ); 
  TH1D* VarHighpt7  = Y7->Project3D("x"); VarHighpt7->SetName("VarHighpt7");
  VarHighpt7->SetTitle ("p_{T}<0.6 tracks, bad #phi excluded;p_{T};tracks per event");
  Y7 ->GetYaxis()->SetRange( rphibin+1, nphibins ); 
  TH1D* SecondVarHighpt7  = Y7->Project3D("x"); SecondVarHighpt7->SetName("SecondVarHighpt7");
  VarHighpt7->Add(SecondVarHighpt7);

  Y11 ->GetYaxis()->SetRange( 1, lphibin-1 ); 
  TH1D* VarHighpt11  = Y11->Project3D("x"); VarHighpt11->SetName("VarHighpt11");
  VarHighpt11->SetTitle ("p_{T}<0.6 tracks, bad #phi excluded;p_{T};tracks per event");
  Y11 ->GetYaxis()->SetRange( rphibin+1, nphibins ); 
  TH1D* SecondVarHighpt11  = Y11->Project3D("x"); SecondVarHighpt11->SetName("SecondVarHighpt11");
  VarHighpt11->Add(SecondVarHighpt11);

  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  VarHighpt7->DrawCopy("9");
  VarHighpt11->DrawCopy("9same");

  TLegend* vleglo = new TLegend( 0.6, 0.73, 0.89, 0.9, "" );
  vleglo->SetBorderSize(0);
  vleglo->SetLineWidth(10);
  vleglo->SetFillStyle(0);
  vleglo->AddEntry ( VarHighpt7, "Year 7", "lp");
  vleglo->AddEntry ( VarHighpt11, "Year 11", "lp");
  vleglo->Draw();
  gPad->SaveAs("highptMB.pdf");


  TH1D* VarHighptratio = VarHighpt11->Clone("VarHighptratio");
  VarHighptratio->SetTitle("p_{T}<0.6, Ratio, bad #phi excluded;p_{T};Year 11 / Year7");
  VarHighptratio->Divide(VarHighpt7);
  VarHighptratio->SetAxisRange(0.7,1.0,"y");
  VarHighptratio->SetMarkerColor(kBlack);
  VarHighptratio->SetMarkerStyle( 20 );
  VarHighptratio->SetLineColor(kBlack);
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  VarHighptratio->Fit("pol0");
  VarHighptratio->Draw("9");
  gPad->SaveAs("highptMB.pdf");

  Y7 ->GetYaxis()->SetRange( 1, nphibins ); 
  Y11 ->GetYaxis()->SetRange( 1, nphibins ); 

  // ================ phi eta ====================

  TH2D* highphieta7  = (TH2D*) Y7->Project3D("zy"); highphieta7->SetName("highphieta7");
  highphieta7->SetTitle ("p_{T}>1.0 tracks, Year 7;#phi;#eta");
  // highphieta7->Rebin2D(2,2);
  // highphieta7->SetAxisRange(0, 0.18, "z");
  highphieta7->SetAxisRange(0, 0.013, "z");

  TH2D* highphieta11 = (TH2D*) Y11->Project3D("zy"); highphieta11->SetName("highphieta11");
  highphieta11->SetTitle ("p_{T}>1.0 tracks, Year 11;#phi;#eta");
  // highphieta11->Rebin2D(2,2);
  // highphieta11->SetAxisRange(0, 0.18, "z");
  highphieta11->SetAxisRange(0, 0.013, "z");

  new TCanvas;  
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetRightMargin( 0.15);
  highphieta7->DrawCopy("colz");
  gPad->SaveAs("highptMB.pdf");

  new TCanvas;  
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetRightMargin( 0.15);
  highphieta11->DrawCopy("colz");
  gPad->SaveAs("highptMB.pdf");

  // Rebin to get rid of fluctuations
  highphieta7->Rebin2D(2,2);
  highphieta11->Rebin2D(2,2);


  TH1D* highphietaratio = highphieta11->Clone("phietaratio");
  highphietaratio->SetTitle("p_{T}>1.0, Ratio Year 11 / Year7;#phi;#eta");
  highphietaratio->SetAxisRange( 0, 2.1, "z");
  highphietaratio->Divide(highphieta7);
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  highphietaratio->Draw("colz9");
  gPad->SaveAs("highptMB.pdf)");
  return;
  
  
  
  



  new TCanvas;
  gPad->SetLogy();
  pt7->Draw("9");
  pt11->Draw("9same");

  TH1D* ptratio = pt11->Clone("ptratio");
  ptratio->Divide(pt7);
  ptratio->SetAxisRange(0,5);
  ptratio->SetMarkerColor(kBlack);
  ptratio->SetMarkerStyle( 20 );
  ptratio->SetLineColor(kBlack);
  new TCanvas;
  ptratio->Draw("9");
  

  // AuAu->DrawNormalized();
  // Emb->DrawNormalized("same");

  // char plabel[500];

  // TLegend* leglo = new TLegend( 0.6, 0.66, 0.89, 0.9, "Not rebinned" );
  // leglo->SetBorderSize(0);
  // leglo->SetLineWidth(10);
  // leglo->SetFillStyle(0);
  // leglo->SetMargin(0.1);

  // leglo->AddEntry ( AuAu, "Au+Au HT", "p");
  // leglo->AddEntry ( Emb, "pp HT #oplus Au+Au MB", "p");
  // sprintf ( plabel, "p-value = %0.2g", AuAu->KolmogorovTest(Emb, "") );
  // leglo->AddEntry ( (TObject*) 0, plabel, "");
  // leglo->Draw();
  
  // gPad->SaveAs("FullAj.png");

}
