void CompareAjVectors(TString inname1 = "AjResults/AuAuAj.vectors.root",
		      TString inname2 = "AjResults/Presel_AuAuAj.vectors.root" )
{
  // ----------------------------------
  // Compatibility tests on unbinned AJ
  // ----------------------------------

  TFile * fin1 = new TFile( inname1, "READ" );
  TFile * fin2 = new TFile( inname2, "READ" );

  TVectorD* TVAJ_hi1 = fin1->Get("TVAJ_hi");
  TVectorD* TVAJ_hi2 = fin2->Get("TVAJ_hi");
  
  TVectorD* TVAJ_lo1 = fin1->Get("TVAJ_lo");
  TVectorD* TVAJ_lo2 = fin2->Get("TVAJ_lo");

  cout.precision(8);
  // TVAJ_hi1->Print();
  // TVAJ_hi2->Print();
  
  
  cout << "Kolmogorov-Smirnov for " << inname1 << " vs. " << inname2 << ", HIGH cut: " << endl
       << TMath::KolmogorovTest(TVAJ_hi1->GetNoElements(), (double*)&(TVAJ_hi1(0)), TVAJ_hi2->GetNoElements(), (double*)&(TVAJ_hi2(0)), "") << endl;
  cout << "Kolmogorov-Smirnov for " << inname1 << " vs. " << inname2 << ", LOW  cut: "<< endl
       << TMath::KolmogorovTest(TVAJ_lo1->GetNoElements(), (double*)&(TVAJ_lo1(0)), TVAJ_lo2->GetNoElements(), (double*)&(TVAJ_lo2(0)), "") << endl;

  
}
