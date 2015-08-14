{
  gStyle->SetOptStat(0);

  TFile* fMIP = new TFile("AjResults/Tow0_Eff0_ppInAuAuAj.root");
  TFile* fHad = new TFile("AjResults/Tow0_Eff0_HC30_ppInAuAuAj.root");
  int refmultcut=269;
  TLegend* leg = new TLegend ( 0.6, 0.6, 0.88, 0.88, "pp");
  TString Outfile = "pp_MIPvHad.png";
  
  // TFile* fMIP = new TFile("AjResults/Presel_AuAuAj.root");
  // TFile* fHad = new TFile("AjResults/HC30_Presel.root");
  // int refmultcut=269;
  // TLegend* leg = new TLegend ( 0.6, 0.6, 0.88, 0.88, "Au+Au");
  // TString Outfile = "AuAu_MIPvHad.png";
  
  TH2D* MIP_hi2D = fMIP->Get("AJ_hi");
  MIP_hi2D->SetName("MIP_hi2D");
  TH2D* Had_hi2D = fHad->Get("AJ_hi");
  Had_hi2D->SetName("Had_hi2D");

  TH2D* MIP_lo2D = fMIP->Get("AJ_lo");
  MIP_lo2D->SetName("MIP_lo2D");
  TH2D* Had_lo2D = fHad->Get("AJ_lo");
  Had_lo2D->SetName("Had_lo2D");

  int binl = MIP_hi2D->GetYaxis()->FindBin( refmultcut );
  int binr = MIP_hi2D->GetNbinsY();

  TH1D* MIP_hi = MIP_hi2D->ProjectionX("MIP_hi", binl, binr);
  TH1D* Had_hi = Had_hi2D->ProjectionX("Had_hi", binl, binr);
  TH1D* MIP_lo = MIP_lo2D->ProjectionX("MIP_lo", binl, binr);
  TH1D* Had_lo = Had_lo2D->ProjectionX("Had_lo", binl, binr);
  

  MIP_hi->SetLineColor(kRed);
  MIP_lo->SetLineColor(kMagenta);
  Had_hi->SetLineColor(kBlue);
  Had_lo->SetLineColor(kCyan+2);

  MIP_hi->SetLineWidth(2);
  MIP_lo->SetLineWidth(2);
  Had_hi->SetLineWidth(2);
  Had_lo->SetLineWidth(2);

  MIP_lo->Rebin(2);
  MIP_hi->Rebin(2);
  Had_lo->Rebin(2);
  Had_hi->Rebin(2);

  cout << "MIP integral: " << MIP_lo->Integral() << endl;
  cout << "MIP integral: " << MIP_hi->Integral() << endl;
    
  cout << "Had integral: " << Had_lo->Integral() << endl;
  cout << "Had integral: " << Had_hi->Integral() << endl;

  MIP_lo->Scale(1./MIP_lo->Integral(0, MIP_lo->GetNbinsX()+1)); // Include over/underflow
  Had_lo->Scale(1./Had_lo->Integral(0, Had_lo->GetNbinsX()+1)); // Include over/underflow
  MIP_hi->Scale(1./MIP_hi->Integral(0, MIP_hi->GetNbinsX()+1)); // Include over/underflow
  Had_hi->Scale(1./Had_hi->Integral(0, Had_hi->GetNbinsX()+1)); // Include over/underflow
 
  MIP_lo->SetTitle("");
  MIP_hi->SetTitle("");
  Had_lo->SetTitle("");
  Had_hi->SetTitle("");

  MIP_lo->SetAxisRange( 0, 0.8, "x");
  MIP_hi->SetAxisRange( 0, 0.8, "x");
  Had_lo->SetAxisRange( 0, 0.8, "x");
  Had_hi->SetAxisRange( 0, 0.8, "x");

  MIP_lo->SetAxisRange( 0, 0.25, "y");
  MIP_hi->SetAxisRange( 0, 0.25, "y");
  Had_lo->SetAxisRange( 0, 0.25, "y");
  Had_hi->SetAxisRange( 0, 0.25, "y");

  new TCanvas;
  MIP_lo->Draw();
  Had_lo->Draw("same");

  MIP_hi->Draw("same");
  Had_hi->Draw("same");

  leg->AddEntry( MIP_hi, "MIP Corr.");
  leg->AddEntry( Had_hi, "100% hadr. Corr.");

  leg->AddEntry( MIP_lo, "MIP Corr.");
  leg->AddEntry( Had_lo, "100% hadr. Corr.");

  leg->Draw();
  
  gPad->SaveAs(Outfile);

}

