{
  TFile *_file0 = TFile::Open("AjResults/Tow0_Eff0_ForResAj_HT54_Geant.root");

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);

  TLegend* leg = 0;

  new TCanvas;

  TProfile* pj1NEF = (TProfile*) j1NEF->ProfileX("pj1NEF",1,-1,"s");
  TProfile* pj2NEF = (TProfile*) j2NEF->ProfileX("pj2NEF",1,-1,"s");
  TProfile* pmj1NEF = (TProfile*) mj1NEF->ProfileX("pmj1NEF",1,-1,"s");
  TProfile* pmj2NEF = (TProfile*) mj2NEF->ProfileX("pmj2NEF",1,-1,"s");

  TH1D* dummy = new TH1D( "dummy", "", 100, 0, 50);
  dummy->SetAxisRange(0, 1.15, "y");
  dummy->Draw("Axis");
  pmj1NEF->Rebin(4);
  pmj1NEF->SetMarkerColor(kBlack);
  pmj1NEF->SetMarkerStyle(25);
  pmj1NEF->SetLineColor(kBlack);
  pmj1NEF->Draw("same");

  pj1NEF->Rebin(4);
  pj1NEF->SetMarkerColor(kRed);
  pj1NEF->SetMarkerStyle(24);
  pj1NEF->SetLineColor(kRed);
  pj1NEF->Draw("same");

  
  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "R=0.4, Leading Jets" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry(pj1NEF, "Hard Core");
  leg->AddEntry(pmj1NEF, "Matched");
  leg->Draw();

  gPad->SaveAs("plots/j1NEF.png");

  
  new TCanvas();
  dummy->Draw("Axis");
  pmj2NEF->Rebin(4);
  pmj2NEF->SetMarkerColor(kBlack);
  pmj2NEF->SetMarkerStyle(25);
  pmj2NEF->SetLineColor(kBlack);
  pmj2NEF->Draw("same");

  pj2NEF->Rebin(4);
  pj2NEF->SetMarkerColor(kRed);
  pj2NEF->SetMarkerStyle(24);
  pj2NEF->SetLineColor(kRed);
  pj2NEF->Draw("same");

  
  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "R=0.4, SubLeading Jets" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry(pj2NEF, "Hard Core");
  leg->AddEntry(pmj2NEF, "Matched");
  leg->Draw();

  gPad->SaveAs("plots/j2NEF.png");

  return 0;
  // TFile *fpp = TFile::Open("AjResults/Tow0_Eff0_Test_Groom_Aj_HT54_HTled_ppInAuAuAj.root");
  // TFile *faa = TFile::Open("AjResults/Test_Groom_AuAu_HT54_HTled.root");

  // TH2D* pp1 = (TH2D*) fpp->Get("j1NEF");
  // TH2D* pp2 = (TH2D*) fpp->Get("j2NEF");

  // TH2D* aa1 = (TH2D*) faa->Get("j1NEF");
  // TH2D* aa2 = (TH2D*) faa->Get("j2NEF");

  // new TCanvas;
  // pp1->SetLineColor(kBlack);
  // aa1->SetLineColor(kRed);
  // aa1->ProjectionY()->DrawNormalized();
  // pp1->ProjectionY()->DrawNormalized("same");

  
  // new TCanvas;
  // // aa2->ProjectionX()->Draw();
  // // return;
  // int binl = pp2->GetXaxis()->FindBin(10 + 0.001);
  // int binr = pp2->GetXaxis()->FindBin(12 - 0.001);
  // pp2->SetLineColor(kBlack);
  // aa2->SetLineColor(kRed);
  // TH1D* paa2 = (TH1D*) aa2->ProjectionY("paa2",binl, binr);
  // TH1D* ppp2 = (TH1D*) pp2->ProjectionY("ppp2",binl, binr);
  // // paa2->Rebin(2);
  // // ppp2->Rebin(2);
  // paa2->DrawNormalized();
  // ppp2->DrawNormalized("same");

}
