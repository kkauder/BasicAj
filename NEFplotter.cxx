{
  TFile *fpp = TFile::Open("AjResults/Tow0_Eff0_Test_Groom_Aj_HT54_HTled_ppInAuAuAj.root");
  TFile *faa = TFile::Open("AjResults/Test_Groom_AuAu_HT54_HTled.root");

  TH2D* pp1 = (TH2D*) fpp->Get("j1NEF");
  TH2D* pp2 = (TH2D*) fpp->Get("j2NEF");

  TH2D* aa1 = (TH2D*) faa->Get("j1NEF");
  TH2D* aa2 = (TH2D*) faa->Get("j2NEF");

  new TCanvas;
  pp1->SetLineColor(kBlack);
  aa1->SetLineColor(kRed);
  aa1->ProjectionY()->DrawNormalized();
  pp1->ProjectionY()->DrawNormalized("same");

  
  new TCanvas;
  // aa2->ProjectionX()->Draw();
  // return;
  int binl = pp2->GetXaxis()->FindBin(10 + 0.001);
  int binr = pp2->GetXaxis()->FindBin(12 - 0.001);
  pp2->SetLineColor(kBlack);
  aa2->SetLineColor(kRed);
  TH1D* paa2 = (TH1D*) aa2->ProjectionY("paa2",binl, binr);
  TH1D* ppp2 = (TH1D*) pp2->ProjectionY("ppp2",binl, binr);
  // paa2->Rebin(2);
  // ppp2->Rebin(2);
  paa2->DrawNormalized();
  ppp2->DrawNormalized("same");

}
