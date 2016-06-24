{
  gStyle->SetOptStat(0);

  TFile* fStd = new TFile("AjResults/Fresh_NicksList_HC100_AuAu.root");
  TFile* fNoMax = new TFile("AjResults/NoMaxPtEt_Fresh_NicksList.root");
  int refmultcut=269;
  TLegend* leg = new TLegend ( 0.6, 0.6, 0.88, 0.88, "Au+Au HT, R=0.4");
  TString Outfile = "plots/AuAu_NoMaxPt.png";
    
  TH2D* Std_hi2D = fStd->Get("AJ_hi");
  Std_hi2D->SetName("Std_hi2D");
  TH2D* NoMax_hi2D = fNoMax->Get("AJ_hi");
  NoMax_hi2D->SetName("NoMax_hi2D");

  TH2D* Std_lo2D = fStd->Get("NoFabsAJ_lo");
  Std_lo2D->SetName("Std_lo2D");
  TH2D* NoMax_lo2D = fNoMax->Get("NoFabsAJ_lo");
  NoMax_lo2D->SetName("NoMax_lo2D");

  int binl = Std_hi2D->GetYaxis()->FindBin( refmultcut );
  int binr = Std_hi2D->GetNbinsY();

  TH1D* Std_hi = Std_hi2D->ProjectionX("Std_hi", binl, binr);
  TH1D* NoMax_hi = NoMax_hi2D->ProjectionX("NoMax_hi", binl, binr);
  TH1D* Std_lo = Std_lo2D->ProjectionX("Std_lo", binl, binr);
  TH1D* NoMax_lo = NoMax_lo2D->ProjectionX("NoMax_lo", binl, binr);
  

  Std_hi->SetLineColor(kRed);
  Std_lo->SetLineColor(kMagenta);
  NoMax_hi->SetLineColor(kBlue);
  NoMax_lo->SetLineColor(kCyan+2);

  Std_hi->SetLineWidth(2);
  Std_lo->SetLineWidth(2);
  NoMax_hi->SetLineWidth(2);
  NoMax_lo->SetLineWidth(2);

  Std_lo->Rebin(2);
  Std_hi->Rebin(2);
  NoMax_lo->Rebin(2);
  NoMax_hi->Rebin(2);

  cout << "Std integral: " << Std_lo->Integral() << endl;
  cout << "Std integral: " << Std_hi->Integral() << endl;
    
  cout << "NoMax integral: " << NoMax_lo->Integral() << endl;
  cout << "NoMax integral: " << NoMax_hi->Integral() << endl;

  Std_lo->Scale(1./Std_lo->Integral(0, Std_lo->GetNbinsX()+1)); // Include over/underflow
  NoMax_lo->Scale(1./NoMax_lo->Integral(0, NoMax_lo->GetNbinsX()+1)); // Include over/underflow
  Std_hi->Scale(1./Std_hi->Integral(0, Std_hi->GetNbinsX()+1)); // Include over/underflow
  NoMax_hi->Scale(1./NoMax_hi->Integral(0, NoMax_hi->GetNbinsX()+1)); // Include over/underflow
 
  Std_lo->SetTitle("");
  Std_hi->SetTitle("");
  NoMax_lo->SetTitle("");
  NoMax_hi->SetTitle("");

  Std_lo->SetAxisRange( -0.3, 0.8, "x");
  Std_hi->SetAxisRange( -0.3, 0.8, "x");
  NoMax_lo->SetAxisRange( -0.3, 0.8, "x");
  NoMax_hi->SetAxisRange( -0.3, 0.8, "x");

  Std_lo->SetAxisRange( 0, 0.25, "y");
  Std_hi->SetAxisRange( 0, 0.25, "y");
  NoMax_lo->SetAxisRange( 0, 0.25, "y");
  NoMax_hi->SetAxisRange( 0, 0.25, "y");

  new TCanvas;
  Std_lo->Draw();
  NoMax_lo->Draw("same");

  Std_hi->Draw("same");
  NoMax_hi->Draw("same");

  leg->AddEntry( Std_hi, "Standard");
  leg->AddEntry( NoMax_hi, "No max p_{T} rejection");

  leg->AddEntry( Std_lo, "Standard");
  leg->AddEntry( NoMax_lo, "No max p_{T} rejection");

  leg->Draw();
  
  gPad->SaveAs(Outfile);

}

