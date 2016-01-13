{
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleBorderSize(0);

  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  // TFile *_file0 = TFile::Open("EmbFullAj.root");
  // TH1D* Emb = (TH1D*) hh->Clone("Emb");
  // TFile *_file1 = TFile::Open("AuAuFullAj.root");
  // TH1D* AuAu = (TH1D*) hh->Clone("AuAu");

  TFile *_file0 = TFile::Open("R0.2_EmbFullAj.root");
  TH1D* Emb = (TH1D*) hh->Clone("Emb");
  TFile *_file1 = TFile::Open("R0.2_AuAuFullAj.root");
  TH1D* AuAu = (TH1D*) hh->Clone("AuAu");


  Emb->SetStats(0);
  AuAu->SetStats(0);

  AuAu->Rebin(2);
  Emb->Rebin(2);


  AuAu->SetMarkerStyle( 21 );
  Emb->SetMarkerStyle( 25 );
    
  AuAu->DrawNormalized();
  Emb->DrawNormalized("same");

  char plabel[500];

  TLegend* leglo = new TLegend( 0.6, 0.66, 0.89, 0.9, "" );
  leglo->SetBorderSize(0);
  leglo->SetLineWidth(10);
  leglo->SetFillStyle(0);
  leglo->SetMargin(0.1);

  leglo->AddEntry ( AuAu, "Au+Au HT", "p");
  leglo->AddEntry ( Emb, "pp HT #oplus Au+Au MB", "p");
  // sprintf ( plabel, "p-value = %0.2g", AuAu->Chi2Test(Emb, "") );
  // //  sprintf ( plabel, "p-value = %0.2g", AuAu->KolmogorovTest(Emb, "") );
  // leglo->AddEntry ( (TObject*) 0, plabel, "");
  leglo->AddEntry ( (TObject*) 0, "", "");
  sprintf ( plabel, "p-value (#chi^{2}) = %0.2g", AuAu->Chi2Test(Emb, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");
  sprintf ( plabel, "p-value (KS) = %0.2g", AuAu->KolmogorovTest(Emb, "") );
  leglo->AddEntry ( (TObject*) 0, plabel, "");
  leglo->Draw();

  if ( TString(_file0->GetName()).Contains("R0.2") ){
    gPad->SaveAs("R0.2_FullAj.png");
  } else {
    gPad->SaveAs("FullAj.png");
  }
  
}
