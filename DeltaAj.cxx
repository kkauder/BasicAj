{
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleBorderSize(0);


  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TFile *_file0 = TFile::Open("EmbLargeAjDeltaAj.root");
  TH1D* Emb = (TH1D*) hh->Clone("Emb");
  TFile *_file1 = TFile::Open("AuAuLargeAjDeltaAj.root");
  TH1D* AuAu = (TH1D*) hh->Clone("AuAu");

  // TFile *_file0 = TFile::Open("R0.2_EmbLargeAjDeltaAj.root");
  // TH1D* Emb = (TH1D*) hh->Clone("Emb");
  // TFile *_file1 = TFile::Open("R0.2_AuAuLargeAjDeltaAj.root");
  // TH1D* AuAu = (TH1D*) hh->Clone("AuAu");

  // TFile *_file0 = TFile::Open("EmbDeltaAj.root");
  // TH1D* Emb = (TH1D*) hh->Clone("Emb");
  // TFile *_file1 = TFile::Open("AuAuDeltaAj.root");
  // TH1D* AuAu = (TH1D*) hh->Clone("AuAu");

  // TFile *_file0 = TFile::Open("R0.2_EmbDeltaAj.root");
  // TH1D* Emb = (TH1D*) hh->Clone("Emb");
  // TFile *_file1 = TFile::Open("R0.2_AuAuDeltaAj.root");
  // TH1D* AuAu = (TH1D*) hh->Clone("AuAu");

  Emb->SetStats(0);
  AuAu->SetStats(0);
  
  AuAu->Rebin(4);
  Emb->Rebin(4);
  
  AuAu->SetMarkerStyle( 21 );
  Emb->SetMarkerStyle( 25 );

  AuAu->SetTitle("#DeltaA_{J} = A_{J}(p_{T}^{Cut}>2 GeV/c) - A_{J}(p_{T}^{Cut}>0.2 GeV/c);#DeltaA_{J};fraction");

  AuAu->DrawNormalized();
  Emb->DrawNormalized("same");

  char plabel[500];

  TLegend* leglo = new TLegend( 0.6, 0.62, 0.89, 0.9, "" );
  leglo->SetBorderSize(0);
  leglo->SetLineWidth(10);
  leglo->SetFillStyle(0);
  leglo->SetMargin(0.1);

  leglo->AddEntry ( AuAu, "Au+Au HT", "p");
  sprintf ( plabel, "Mean = %0.2g", AuAu->GetMean() );
  leglo->AddEntry ( (TObject*) 0, plabel, "");
  
  leglo->AddEntry ( Emb, "pp HT #oplus Au+Au MB", "p");
  sprintf ( plabel, "Mean = %0.2g", Emb->GetMean() );
  leglo->AddEntry ( (TObject*) 0, plabel, "");
  
  leglo->AddEntry ( (TObject*) 0, "", "");
  sprintf ( plabel, "p-value (#chi^{2}) = %0.2g", AuAu->Chi2Test(Emb, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");
  sprintf ( plabel, "p-value (KS) = %0.2g", AuAu->KolmogorovTest(Emb, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");


  leglo->Draw();

  // TPaveText* msg = new TPaveText( 0.15, 0.68, 0.4, 0.85, "brNDC" );
  // sprintf ( plabel, "p-value = %0.2g", AuAu->Chi2Test(Emb, "") );
  // //sprintf ( plabel, "p-value = %0.2g", AuAu->KolmogorovTest(Emb, "") );
  // msg->AddText( plabel );
  // msg->Draw();
  
  // if ( TString(_file0->GetName()).Contains("R0.2") ){
  //   gPad->SaveAs("R0.2_DeltaAj.png");
  // } else {
  //   gPad->SaveAs("DeltaAj.png");
  // }

  if ( TString(_file0->GetName()).Contains("R0.2") ){
    gPad->SaveAs("R0.2_LargeAjDeltaAj.png");
  } else {
    gPad->SaveAs("LargeAjDeltaAj.png");
  }

}
