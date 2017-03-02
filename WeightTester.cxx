int WeightTester( TString in="AjResults/AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root"){
  TChain* chain = new TChain( "TriggeredTree");
  chain->Add( in );

  // Triggers:
  double bins[] = { 0, 5, 10, 15, 20, 25, 30, 35, 40 };
  int nbins  = sizeof(bins) / sizeof(bins[0])-1;

  TString cond;

  cout << endl << endl;
  for ( int i=0 ; i<nbins ; ++i ){
    float ptleft  = bins[i];
    float ptright = bins[i+1];
    cout << " ======== Trigger Jets =============" << endl;
    cout << " pT = " << ptleft << " - " << ptright << endl;
    
    cond = "TriggerJet.Pt()>"; cond += ptleft;
    cond += " && TriggerJet.Pt()<"; cond += ptright;
    cond += " && abs(TriggerJet.Eta())<1 && zgtriglo>0.05";
    chain->Draw("weight", cond, "goff");

    double sum=0;
    for ( int j = htemp->GetNbinsX(); j>0; --j ){
      double v = htemp->GetBinContent(j);
      if ( v<1e-7 ) continue;
      sum += v;
      // cout << htemp->GetBinCenter(j-1) << " --> " << sum << endl;
      cout << sum <<  " > " << htemp->GetBinCenter(j-1) << endl;
      if ( sum > 100 ) break;
    }
  }

  cout << endl << endl;
  for ( int i=0 ; i<nbins ; ++i ){
    float ptleft  = bins[i];
    float ptright = bins[i+1];
    cout << " ======== RECOIL Jets =============" << endl;
    cout << " pT = " << ptleft << " - " << ptright << endl;
    
    cond = "AwayJet.Pt()>"; cond += ptleft;
    cond += " && AwayJet.Pt()<"; cond += ptright;
    cond += " && abs(TriggerJet.Eta())<1 && abs(AwayJet.Eta())<1 && zgawaylo>0.05";
    chain->Draw("weight", cond, "goff");

    double sum=0;
    for ( int j = htemp->GetNbinsX(); j>0; --j ){
      double v = htemp->GetBinContent(j);
      if ( v<1e-7 ) continue;
      sum += v;
      // cout << htemp->GetBinCenter(j-1) << " --> " << sum << endl;
      cout << sum <<  " > " << htemp->GetBinCenter(j-1) << endl;
      if ( sum > 100 ) break;
    }
  }

}



