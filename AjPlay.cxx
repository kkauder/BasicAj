{

  // TFile *_file0 = TFile::Open("AjResults/R0.2_NicksList_HC100_AuAu.root");
  TFile *_file0 = TFile::Open("AjResults/Tow0_Eff0_R0.2_NicksList_HC100_ppInAuAuAj.root");

  // TFile *_file0 = TFile::Open("AjResults/NicksList_HC100_AuAu.root");
  // TFile *_file0 = TFile::Open("AjResults/Tow0_Eff0_NicksList_HC100_ppInAuAuAj.root");
  
  // TH1D* h = AJ_lo->ProjectionX("h",270,1000);
  // TH1D* hh = (TH1D*) h->Clone("hh"); hh->Reset();
  // ResultTree->Draw("(j1.Pt()-j2.Pt()) / (j1.Pt()+j2.Pt())>>hh","refmult>269", "goff");
  // ResultTree->Draw("(jm1.Pt()-jm2.Pt()) / (jm1.Pt()+jm2.Pt())>>hh","refmult>269", "goff");
  // // hh->SaveAs("R0.2_AuAuFullAj.root");
  // hh->SaveAs("R0.2_EmbFullAj.root");

  // // hh->SaveAs("AuAuFullAj.root");
  // // hh->SaveAs("EmbFullAj.root");

  
  TH1D* hh = new TH1D( "hh","", 100, -1, 1);
  hh->Sumw2();
  ResultTree->Draw("(j1.Pt()-j2.Pt()) / (j1.Pt()+j2.Pt()) - (jm1.Pt()-jm2.Pt()) / (jm1.Pt()+jm2.Pt())>>hh",
  		   "refmult>269", "goff");

  // ResultTree->Draw("(j1.Pt()-j2.Pt()) / (j1.Pt()+j2.Pt()) - (jm1.Pt()-jm2.Pt()) / (jm1.Pt()+jm2.Pt())>>hh",
  // 		   "refmult>269 && (j1.Pt()-j2.Pt()) / (j1.Pt()+j2.Pt()) > 0.3", "goff");

  new TCanvas; hh->Draw();
  // hh->SaveAs("R0.2_AuAuDeltaAj.root");
  // hh->SaveAs("R0.2_EmbDeltaAj.root");

  hh->SaveAs("AuAuDeltaAj.root");
  // hh->SaveAs("EmbDeltaAj.root");

  // // hh->SaveAs("R0.2_AuAuLargeAjDeltaAj.root");
  // // hh->SaveAs("R0.2_EmbLargeAjDeltaAj.root");

  // // hh->SaveAs("AuAuLargeAjDeltaAj.root");
  // // hh->SaveAs("EmbLargeAjDeltaAj.root");


  // ResultTree->Draw("(j1.Pt()-j2.Pt()) / (j1.Pt()+j2.Pt()) - (jm1.Pt()-jm2.Pt()) / (jm1.Pt()+jm2.Pt())>>hh",
  // 		   "refmult>269 && (j1.Pt()-j2.Pt()) / (j1.Pt()+j2.Pt()) > 0.3", "goff");


}
