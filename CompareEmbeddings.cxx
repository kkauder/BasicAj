{
  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%
  bool nofabs=false;
  
  TFile* f0 = TFile::Open("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root");
  TH2D* h2 = (TH2D*) f0->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();

  TH1D* origAJ_hi = (TH1D*) ( (TH2D*) f0->Get( "AJ_hi") )->ProjectionX("origAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* origAJ_lo = (TH1D*) ( (TH2D*) f0->Get( "AJ_lo") )->ProjectionX("origAJ_lo", AuAuMultBinL, AuAuMultBinR);

  // TFile* f1 = TFile::Open("AlternateEmbeddingTest.root");
  // TFile* f1 = TFile::Open("AlternateEmbedding_DumbRho.root");
  TFile* f1 = TFile::Open("AlternateEmbedding_ProperRho.root");
  TH1D* newAJ_hi = (TH1D*) ( (TH2D*) f1->Get( "AJ_hi") )->ProjectionX("newAJ_hi", AuAuMultBinL, AuAuMultBinR);
  TH1D* newAJ_lo = (TH1D*) ( (TH2D*) f1->Get( "AJ_lo") )->ProjectionX("newAJ_lo", AuAuMultBinL, AuAuMultBinR);

  // axis labels and titles
  float ls = 0.05;
  float ts = 0.05;
  float yoff = 1.06;
  float xoff = 0.92;

  // legend
  float legs = 0.04;
  TObjArray toa;
  toa.Add(origAJ_lo);
  toa.Add(newAJ_lo);
  toa.Add(origAJ_hi);
  toa.Add(newAJ_hi);
  
  // Prettify
  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->SetLineWidth( 2 );

    h->SetTitle(";|A_{J}|;Fraction");
    if ( nofabs ) h->SetTitle(";A_{J};Fraction");
    
    h->Rebin(2);
    h->Scale(1./h->Integral(0, h->GetNbinsX()+1)); // Include over/underflow
    
    h->SetAxisRange(  0.0, 0.23, "y");
    h->SetAxisRange(  0.0, 0.72, "x");
    if ( nofabs ) h->SetAxisRange(  0.0, 0.25, "y");
    if ( nofabs ) h->SetAxisRange(  -0.3, 0.8, "x");

    h->GetXaxis()->SetLabelSize( ls );
    h->GetXaxis()->SetTitleSize( ls );
    h->GetXaxis()->SetTitleOffset( xoff );
    h->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetXaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

    h->GetYaxis()->SetLabelSize( ls );
    h->GetYaxis()->SetTitleSize( ls );
    h->GetYaxis()->SetTitleOffset( yoff );
    h->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
  }

  origAJ_lo->SetLineColor( kBlack );
  origAJ_lo->SetMarkerColor( kBlack );
  origAJ_lo->SetMarkerStyle( 25 );

  origAJ_hi->SetLineColor( kRed );
  origAJ_hi->SetMarkerColor( kRed );
  origAJ_hi->SetMarkerStyle( 24 );
  
  newAJ_lo->SetLineColor( kGray+1 );
  newAJ_lo->SetMarkerColor( kGray+1 );
  newAJ_lo->SetMarkerStyle( 25 );

  newAJ_hi->SetLineColor( kGreen+1 );
  newAJ_hi->SetMarkerColor( kGreen+1 );
  newAJ_hi->SetMarkerStyle( 24 );
  
  
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  origAJ_hi->SetTitle( f1->GetName() );
  origAJ_hi->Draw("9");
  origAJ_lo->Draw("9same");
  newAJ_hi->Draw("9same");
  newAJ_lo->Draw("9same");

  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( origAJ_hi, "pp HT #oplus AuAu MB, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( newAJ_hi, "pp HT #oplus AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( origAJ_lo, "pp HT #oplus AuAu MB, p_{T}^{Cut}>0.2 GeV/c, matched", "p");
  leg->AddEntry ( newAJ_lo, "pp HT #oplus AuAu HT, p_{T}^{Cut}>0.2 GeV/c, matched", "p");

  leg->Draw();
  TString outname=f1.GetName();
  outname.ReplaceAll( ".root","_CompareEmbedding.png");
  gPad->SaveAs( outname );
  
}
