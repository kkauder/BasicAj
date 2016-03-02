{
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TFile *fpp           = TFile::Open("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppAj.root");
  TFile *fppInAuAu     = TFile::Open("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root");


  TObjArray vAJ_lo;
  TObjArray vAJ_hi;

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  TH2D* h2 = (TH2D*) fppInAuAu->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();
  
  cout << "Projecting Au+Au over " << h2->GetYaxis()->GetTitle() << " = "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinL ) << " -- "
       << h2->GetYaxis()->GetBinLowEdge( AuAuMultBinR+1 ) << endl;
  
  TH1D* ppInAuAuAJ_lo = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_lo") )->ProjectionX("ppInAuAuAJ_lo", AuAuMultBinL, AuAuMultBinR);
  TH1D* pp_lo = (TH1D*) ( (TH2D*) fpp->Get( "AJ_lo") )->ProjectionX("pp_lo", 1, AuAuMultBinR);

  cout << pp_lo ->Integral(1,100) << " dijets in pp" << endl;
  cout << ppInAuAuAJ_lo ->Integral(1,100) << " dijets in pp@AuAu" << endl;

  TH1D* ppInAuAuAJ_hi = (TH1D*) ( (TH2D*) fppInAuAu->Get( "AJ_hi") )->ProjectionX("ppInAuAuAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* pp_hi = (TH1D*) ( (TH2D*) fpp->Get( "AJ_hi") )->ProjectionX("pp_hi", 1, AuAuMultBinR);


  TObjArray toa;
  toa.Add(pp_lo);
  toa.Add(ppInAuAuAJ_lo);
  toa.Add(pp_hi);
  toa.Add(ppInAuAuAJ_hi);

  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );
    h->SetTitle(";|A_{J}|;Fraction");
    
    h->Rebin(2);
    // h->Scale(1./h->Integral());
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.27, "y");
    h->SetAxisRange(  0.0, 0.72, "x");    
  }

  pp_lo->SetLineColor( kBlack );
  pp_lo->SetMarkerColor( kBlack );
  pp_lo->SetMarkerStyle( 21 );

  ppInAuAuAJ_lo->SetLineColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerColor( kBlack );
  ppInAuAuAJ_lo->SetMarkerStyle( 25 );

  pp_hi->SetLineColor( kRed );
  pp_hi->SetMarkerColor( kRed );
  pp_hi->SetMarkerStyle( 20 );

  ppInAuAuAJ_hi->SetLineColor( kRed );
  ppInAuAuAJ_hi->SetMarkerColor( kRed );
  ppInAuAuAJ_hi->SetMarkerStyle( 24 );
  
  ppInAuAuAJ_hi->GetYaxis()->SetTitleSize(0.04);
  ppInAuAuAJ_hi->GetYaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_hi->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_hi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

  ppInAuAuAJ_hi->GetXaxis()->SetTitleSize(0.04);
  ppInAuAuAJ_hi->GetXaxis()->SetLabelSize(0.04);
  ppInAuAuAJ_hi->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
  ppInAuAuAJ_hi->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  
  
  ppInAuAuAJ_hi->GetYaxis()->SetTitleOffset(1.14);
  ppInAuAuAJ_hi->Draw("9");

  ppInAuAuAJ_lo->Draw("same");
  pp_lo->Draw("same");
  pp_hi->Draw("same");

  

}
