{
  TChain * ResultTree = new TChain("ResultTree");
  ResultTree->Add("AjResults/Test_Groom_AuAu_HT54_HTled.root");

  double refmult;
  TLorentzVector *j1, *j2, *jm1, *jm2;
  TLorentzVector *j1s1, *j1s2, *j2s1, *j2s2, *jm1s1, *jm1s2, *jm2s1, *jm2s2;
  ResultTree->SetBranchAddress("j1", &j1);
  ResultTree->SetBranchAddress("j2", &j2);
  ResultTree->SetBranchAddress("refmult", &refmult);

  ResultTree->SetBranchAddress("jm1s1", &jm1s1);
  ResultTree->SetBranchAddress("jm1s2", &jm1s2);
  ResultTree->SetBranchAddress("jm2s1", &jm2s1);
  ResultTree->SetBranchAddress("jm2s2", &jm2s2);

  float aj_lo;
  ResultTree->SetBranchAddress("aj_lo", &aj_lo);
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);
  
  TH1D* DeltaR2 = new TH1D( "DeltaR2", "#Delta R", 50, 0, 1 );
  // TH1D* DeltaR2 = new TH1D( "DeltaR2", "#Delta R", 100, -1, 1 );
  TH1D* AJ_less = new TH1D( "AJ_less","A_{J} for {#Delta}R<0.2;|A_{J}|", 50, -0.6, 0.9 );
  TH1D* AJ_more = new TH1D( "AJ_more","A_{J} for {#Delta}R>0.2;|A_{J}|", 50, -0.6, 0.9 );

  
  int c = 0;
  for ( int i=0; i<ResultTree->GetEntries(); ++i ){
    ResultTree->GetEntry ( i );
    if ( refmult < 269 ) continue;
    c++;
    if ( jm2s1->Pt() > 0.1 && jm2s2->Pt() > 0.1 ){
      DeltaR2->Fill( jm2s1->DeltaR( *jm2s2 ) );
      if ( jm2s1->DeltaR( *jm2s2 )<0.2 )	AJ_less->Fill( aj_lo );
      else                                     	AJ_more->Fill( aj_lo );
      
    }

  }
  new TCanvas; DeltaR2->Draw();
  new TCanvas; AJ_less->Draw();
  new TCanvas; AJ_more->Draw();
}
