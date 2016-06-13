{
  TFile *_file0 = TFile::Open("AjResults/Fresh_NicksList_HC100_AuAu.root");
  TH1D* m1 = new TH1D( "m1","Matching Percentage, Leading Jet;%;counts",55,0,1.1);
  new TCanvas;
  ResultTree->Draw("MatchPerc1>>m1","refmult>=269");
  gPad->SaveAs("plots/MatchPerc1.png");
  
  TH1D* m2 = new TH1D( "m2","Matching Percentage, SubLeading Jet;%;counts",55,0,1.1);
  new TCanvas;
  ResultTree->Draw("MatchPerc2>>m2","refmult>=269");
  gPad->SaveAs("plots/MatchPerc2.png");

  int b1 = m1->FindBin(1);
  cout << "Out of " << m1->GetEntries()
       << " leading jets, " << m1->GetEntries() - m1->GetBinContent(b1)
       << " were not 100% matched." << endl;

  int b2 = m2->FindBin(1);
  cout << "Out of " << m2->GetEntries()
       << " leading jets, " << m2->GetEntries() - m2->GetBinContent(b2)
       << " were not 100% matched." << endl;

  // What about coincidence?
  new TCanvas;
  ResultTree->Draw("j1.Pt()","refmult>=269 && MatchPerc2<1 && MatchPerc1<1");
  cout << "Out of " << m2->GetEntries()
       << " dijets, " << htemp->GetEntries() << " were affected on both sides." << endl;

  // Number of lost constituents
  new TCanvas;
  ResultTree->Draw("MatchLoss1","MatchPerc1<1 && refmult>=269");
  gPad->SaveAs("plots/MatchPerc1Loss.png");

  new TCanvas;
  ResultTree->Draw("MatchLoss2","MatchPerc2<1 && refmult>=269");
  gPad->SaveAs("plots/MatchPerc2Loss.png");
  


  // pT of jets with missing E
  new TCanvas;
  ResultTree->Draw("j1.Pt()","MatchPerc1<1 && refmult>=269");
  gPad->SaveAs("plots/MatchPerc1Imperfect_Pt1.png");

  new TCanvas;
  ResultTree->Draw("j2.Pt()","MatchPerc2<1 && refmult>=269");
  gPad->SaveAs("plots/MatchPerc2Imperfect_Pt2.png");

  int bin269 = hPtMissed1->GetYaxis()->FindBin(269);
  new TCanvas;
  hPtMissed1->ProjectionX("miss1",bin269,1000)->Draw();
  gPad->SaveAs("plots/MatchMissPt1.png");
  
  new TCanvas;
  hPtMissed2->ProjectionX("miss2",bin269,1000)->Draw();
  gPad->SaveAs("plots/MatchMissPt2.png");

  new TCanvas;
  hDRMissed1->ProjectionX("miss1",bin269,1000)->Draw();
  gPad->SaveAs("plots/MatchMissDR1.png");
  
  new TCanvas;
  hDRMissed2->ProjectionX("miss2",bin269,1000)->Draw();
  gPad->SaveAs("plots/MatchMissDR2.png");

}
