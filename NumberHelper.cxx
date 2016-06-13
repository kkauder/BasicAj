{
  gStyle->SetOptStat(0);
  
  TFile* f = new TFile("AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root","READ");
  
  TH2D* hLeadPtrecoVPtorigInTop20 = (TH2D*) f->Get("hLeadPtrecoVPtorigInTop20");
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogz();
  hLeadPtrecoVPtorigInTop20->Draw("colz");
  gPad->SaveAs("plots/PtRecoVPtOrig.png");

  int xbinl = hLeadPtrecoVPtorigInTop20->GetXaxis()->FindBin(10+0.001);
  int xbinr = hLeadPtrecoVPtorigInTop20->GetXaxis()->FindBin(20-0.001);
  int ybinl = hLeadPtrecoVPtorigInTop20->GetYaxis()->FindBin(20+0.001);
  int ybinr = hLeadPtrecoVPtorigInTop20->GetNbinsY();
  cout << hLeadPtrecoVPtorigInTop20->Integral(xbinl, xbinr, ybinl, ybinr) << " jets were bumped above 20 GeV" << endl;
  cout << " Out of " << hLeadPtrecoVPtorigInTop20->Integral(1, hLeadPtrecoVPtorigInTop20->GetNbinsX(), ybinl, ybinr) << " total above 20 GeV" << endl;

  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  hLeadPtrecoVPtorigInTop20->ProjectionX( "px", ybinl, ybinr)->Draw();
  gPad->SaveAs("plots/PtOrigForEmbeddedLeading.png");  

  // ========================================================================
  TH1D* CountingInTop20 = (TH1D*) f->Get("CountingInTop20");
  double nJetsUsedInTop20 = CountingInTop20->GetBinContent(1);
  double nOrigOkInTop20 = CountingInTop20->GetBinContent(2);
  double nNewOkInTop20 = CountingInTop20->GetBinContent(3);    
  double nNoLeadingInTop20 = CountingInTop20->GetBinContent(4);    
  double nNewLeadingInTop20 = CountingInTop20->GetBinContent(5);    
  double nRecoveredLeadingInTop20 = CountingInTop20->GetBinContent(6);    
  double nLostLeadingInTop20 = CountingInTop20->GetBinContent(7);
  double nOrigDijetsOkInTop20 = CountingInTop20->GetBinContent(8);    
  double nNewDijetsOkInTop20 = CountingInTop20->GetBinContent(9);
  double nRecoveredDijetsInTop20 = CountingInTop20->GetBinContent(10);
  double nNewfoundDijetsInTop20 = CountingInTop20->GetBinContent(11);  
  double nLostDijetsInTop20 = CountingInTop20->GetBinContent(12);

  
    
  cout << " ======================================================================== " << endl
       << "             IN 0-20%: " << endl
       << " Embedded " << nJetsUsedInTop20     << " events with a jet above 10 GeV total. Of these :" << endl
       << "  --  " << nOrigOkInTop20           << " originally had a jet above 20 GeV." << endl
       << "  --  " << nNewOkInTop20            << " now have a jet above 20 GeV." << endl
       
       << "  --  " << nNoLeadingInTop20        << " did not have a jet above 10 GeV anymore." << endl
       << "  --  " << nRecoveredLeadingInTop20 << " had a 20+ GeV jet before and after." << endl
       << "  --  " << nNewLeadingInTop20       << " jumped above the 20 GeV threshold." << endl
       << "  --  " << nLostLeadingInTop20      << " dropped below the 20 GeV threshold." << endl
       << "  --> Altogether, we should have "
       << nOrigOkInTop20 <<  " + " << nNewLeadingInTop20 << " - " << nLostLeadingInTop20
       << " = " << nOrigOkInTop20 + nNewLeadingInTop20 - nLostLeadingInTop20
       << " == " << nNewOkInTop20 << endl;
  cout << endl << "             DIJETS: " << endl
       << " Originally had " << nOrigDijetsOkInTop20 << " dijets." << endl
       << " Now finding "    << nNewDijetsOkInTop20 << "." << endl
       << " Of these events, " << nRecoveredDijetsInTop20 << " also had dijets in the original, " << endl
       << " and " << nNewfoundDijetsInTop20 << " did not." << endl
       << " Conversely, " << nLostDijetsInTop20 << " originally had a dijet pair and now lost it."
       << "  --> Altogether, we should have "
       << nOrigDijetsOkInTop20 <<  " + " << nNewfoundDijetsInTop20 << " - " << nLostDijetsInTop20
       << " = " << nOrigDijetsOkInTop20 + nNewfoundDijetsInTop20 - nLostDijetsInTop20
       << " == " << nNewDijetsOkInTop20 << " dijets."
       << endl;
    

  
  

}
