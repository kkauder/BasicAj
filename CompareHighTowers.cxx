{

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness

  // TFile *fAuAu54         = TFile::Open("AjResults/Fresh_NicksList_HC100_AuAu.root");
  // TFile *fppInAuAu54     = TFile::Open("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root");
  TFile *fAuAu54         = TFile::Open("AjResults/Groom_HT54_AuAu.root");
  TFile *fppInAuAu54     = TFile::Open("AjResults/Tow0_Eff0_Groom_HT54_ppInAuAuAj.root");
  
  TFile *fAuAu64         = TFile::Open("AjResults/Groom_HT64_AuAu.root");
  TFile *fppInAuAu64     = TFile::Open("AjResults/Tow0_Eff0_Groom_HT64_ppInAuAuAj.root");

  // TFile *fSyst54         = TFile::Open("AjResults/Systematics_Groom_HT54_ppInAuAuAj.root");
  // TFile *fSyst64         = TFile::Open("AjResults/Systematics_Groom_HT64_ppInAuAuAj.root");

  TH2D* h2 = (TH2D*) fAuAu54->Get( "AJ_lo");
  
  int RefmultCut = 269;
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();

    cout << "Projecting over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* AuAu54AJ_lo=0;
  AuAu54AJ_lo = (TH1D*) ( (TH2D*) fAuAu54->Get( "AJ_lo") )->ProjectionX("AuAu54AJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu54AJ_lo=0;
  ppInAuAu54AJ_lo = (TH1D*) ( (TH2D*) fppInAuAu54->Get( "AJ_lo") )->ProjectionX("ppInAuAu54AJ_lo", AuAuMultBinL, AuAuMultBinR);

  TH1D* AuAu54AJ_hi=0;
  AuAu54AJ_hi = (TH1D*) ( (TH2D*) fAuAu54->Get( "AJ_hi") )->ProjectionX("AuAu54AJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu54AJ_hi=0;
  ppInAuAu54AJ_hi = (TH1D*) ( (TH2D*) fppInAuAu54->Get( "AJ_hi") )->ProjectionX("ppInAuAu54AJ_hi", AuAuMultBinL, AuAuMultBinR);

  TH1D* AuAu64AJ_lo=0;
  AuAu64AJ_lo = (TH1D*) ( (TH2D*) fAuAu64->Get( "AJ_lo") )->ProjectionX("AuAu64AJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu64AJ_lo=0;
  ppInAuAu64AJ_lo = (TH1D*) ( (TH2D*) fppInAuAu64->Get( "AJ_lo") )->ProjectionX("ppInAuAu64AJ_lo", AuAuMultBinL, AuAuMultBinR);

  TH1D* AuAu64AJ_hi=0;
  AuAu64AJ_hi = (TH1D*) ( (TH2D*) fAuAu64->Get( "AJ_hi") )->ProjectionX("AuAu64AJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* ppInAuAu64AJ_hi=0;
  ppInAuAu64AJ_hi = (TH1D*) ( (TH2D*) fppInAuAu64->Get( "AJ_hi") )->ProjectionX("ppInAuAu64AJ_hi", AuAuMultBinL, AuAuMultBinR);


  TObjArray toa;
  toa.Add(AuAu54AJ_lo);
  toa.Add(ppInAuAu54AJ_lo);
  toa.Add(AuAu54AJ_hi);
  toa.Add(ppInAuAu54AJ_hi);
  toa.Add(AuAu64AJ_lo);
  toa.Add(ppInAuAu64AJ_lo);
  toa.Add(AuAu64AJ_hi);
  toa.Add(ppInAuAu64AJ_hi);
  
  TH1D* h;
  // cout.precision(4);
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");
    
    h->Rebin(2);
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.23, "y");
    h->SetAxisRange(  0.0, 0.72, "x");	  
  }  
  
  
  AuAu54AJ_lo->SetMarkerStyle( 21 );
  ppInAuAu54AJ_lo->SetMarkerStyle( 25 );  
  AuAu54AJ_hi->SetMarkerStyle( 20 );
  ppInAuAu54AJ_hi->SetMarkerStyle( 24 );

  AuAu64AJ_lo->SetMarkerStyle( 21 );
  ppInAuAu64AJ_lo->SetMarkerStyle( 25 );  
  AuAu64AJ_hi->SetMarkerStyle( 20 );
  ppInAuAu64AJ_hi->SetMarkerStyle( 24 );

  TLegend* leg=0;

  // AuAu hi:
  // --------
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  AuAu54AJ_hi->SetLineColor(kGreen+1);
  AuAu54AJ_hi->SetMarkerColor(kGreen+1);
  AuAu64AJ_hi->SetLineColor(kMagenta+1);
  AuAu64AJ_hi->SetMarkerColor(kMagenta+1);

  AuAu54AJ_hi->Draw();
  AuAu64AJ_hi->Draw("same");

  leg = new TLegend( 0.55, 0.7, 0.88, 0.88, "Au+Au p_{T}^{Cut}>2 GeV/c:" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry ( AuAu54AJ_hi,     "Offline E_{T}>5.4 GeV", "p");
  leg->AddEntry ( AuAu64AJ_hi,     "Offline E_{T}>6.4 GeV", "p");
  leg->Draw();

  gPad->SaveAs( "plots/AuAu_HTcomp_hi.png");

  // AuAu lo:
  // --------
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  AuAu54AJ_lo->SetLineColor(kGreen+1);
  AuAu54AJ_lo->SetMarkerColor(kGreen+1);
  AuAu64AJ_lo->SetLineColor(kMagenta+1);
  AuAu64AJ_lo->SetMarkerColor(kMagenta+1);

  AuAu54AJ_lo->Draw();
  AuAu64AJ_lo->Draw("same");

  leg = new TLegend( 0.55, 0.7, 0.88, 0.88, "Au+Au Soft Matched:" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry ( AuAu54AJ_lo,     "Offline E_{T}>5.4 GeV", "p");
  leg->AddEntry ( AuAu64AJ_lo,     "Offline E_{T}>6.4 GeV", "p");
  leg->Draw();
  
  gPad->SaveAs( "plots/AuAu_HTcomp_lo.png");
  
  // ppInAuAu hi:
  // --------
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  ppInAuAu54AJ_hi->SetLineColor(kGreen+1);
  ppInAuAu54AJ_hi->SetMarkerColor(kGreen+1);
  ppInAuAu64AJ_hi->SetLineColor(kMagenta+1);
  ppInAuAu64AJ_hi->SetMarkerColor(kMagenta+1);

  ppInAuAu54AJ_hi->Draw();
  ppInAuAu64AJ_hi->Draw("same");

  leg = new TLegend( 0.55, 0.7, 0.88, 0.88, "pp #oplus Au+Au p_{T}^{Cut}>2 GeV/c:" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry ( ppInAuAu54AJ_hi,     "Offline E_{T}>5.4 GeV", "p");
  leg->AddEntry ( ppInAuAu64AJ_hi,     "Offline E_{T}>6.4 GeV", "p");
  leg->Draw();

  gPad->SaveAs( "plots/ppInAuAu_HTcomp_hi.png");

  // ppInAuAu lo:
  // --------
  new TCanvas();
  gPad->SetGridx(0);  gPad->SetGridy(0);

  ppInAuAu54AJ_lo->SetLineColor(kGreen+1);
  ppInAuAu54AJ_lo->SetMarkerColor(kGreen+1);
  ppInAuAu64AJ_lo->SetLineColor(kMagenta+1);
  ppInAuAu64AJ_lo->SetMarkerColor(kMagenta+1);

  ppInAuAu54AJ_lo->Draw();
  ppInAuAu64AJ_lo->Draw("same");

  leg = new TLegend( 0.55, 0.7, 0.88, 0.88, "pp #oplus Au+Au Soft Matched:" );
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->AddEntry ( ppInAuAu54AJ_lo,     "Offline E_{T}>5.4 GeV", "p");
  leg->AddEntry ( ppInAuAu64AJ_lo,     "Offline E_{T}>6.4 GeV", "p");
  leg->Draw();
  
  gPad->SaveAs( "plots/ppInAuAu_HTcomp_lo.png");

  cout << "ppInAuAu54AJ_hi->GetEntries()  = " << ppInAuAu54AJ_hi->GetEntries() << endl;
  cout << "ppInAuAu54AJ_lo->GetEntries()  = " << ppInAuAu54AJ_lo->GetEntries() << endl;

  cout << "ppInAuAu64AJ_hi->GetEntries()  = " << ppInAuAu64AJ_hi->GetEntries() << endl;
  cout << "ppInAuAu64AJ_lo->GetEntries()  = " << ppInAuAu64AJ_lo->GetEntries() << endl;

  cout << endl;
  cout << "AuAu54AJ_hi->GetEntries()  = " << AuAu54AJ_hi->GetEntries() << endl;
  cout << "AuAu54AJ_lo->GetEntries()  = " << AuAu54AJ_lo->GetEntries() << endl;

  cout << "AuAu64AJ_hi->GetEntries()  = " << AuAu64AJ_hi->GetEntries() << endl;
  cout << "AuAu64AJ_lo->GetEntries()  = " << AuAu64AJ_lo->GetEntries() << endl;

}
