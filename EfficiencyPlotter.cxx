
/*Run with
  gSystem->Load("lib/libMyJetlib.so");
  .x EfficiencyPlotter.cxx
*/

// Need a global pointer to interface to TF2
ktTrackEff* tEff=0;

// Interfaces to TF2 signature
Double_t TF2_EffAAY07_20(Double_t *x, Double_t *pars){
  return tEff->EffAAY07_20( x[1], x[0] );
}

Double_t TF2_EffRatio_20(Double_t *x, Double_t *pars){
  return tEff->EffRatio_20( x[1], x[0] );
}

Double_t TF1_EffRatio_20(Double_t *x, Double_t *pars){
  // at eta=0
  // par[0] is uncertainty modifier
  tEff->SetSysUncertainty( pars[0] * 1.1 );
  double ret = tEff->EffRatio_20( 0, x[0] );
  tEff->SetSysUncertainty( 0 );
  return ret;
}


void EfficiencyPlotter()
{

  gStyle->SetOptStat(0);
  
  tEff = new ktTrackEff();

  float ptmin=0.2;
  float ptmax=10.0;
  
  
  TH2D* dummy = new TH2D( "dummy", ";p_{T} [GeV/c];#eta", 100, ptmin, ptmax, 100, -1, 1);
  dummy->SetAxisRange(0,0.9,"z");
  dummy->GetXaxis()->CenterTitle();
  dummy->GetYaxis()->CenterTitle();
  dummy->GetXaxis()->SetTitleOffset(1.5);
  dummy->GetYaxis()->SetTitleOffset(1.5);
  // dummy->SetLineColor(0);
  // TH2D* dummy2 = new TH2D( "dummy2", ";p_{T} [GeV/c];#eta", 100, ptmin, ptmax, 100, -1, 1); // for Eval: respect borders


  TLatex l;
  l.SetNDC();
  l.SetTextColor(kBlack);
  l.SetTextSize(0.05); 
   
    
  // Run 6
  new TCanvas; 
  tEff->GetEffY06()->SetRange(-1, 0.2, 1, 10);
  dummy->Reset();
  dummy->Eval ( tEff->GetEffY06(), "R" );
  dummy->DrawCopy("surf1");
  l.DrawLatex(.13, .88, "Run6 pp");
  gPad->SaveAs("EffPP.png");

  // Run 7
  new TCanvas;
  TF2* f07 = new TF2 ( "f07", TF2_EffAAY07_20, ptmin, ptmax, -1, 1, 0);
  dummy->Reset();
  dummy->Eval ( f07, "R" );
  dummy->DrawCopy("surf1");
  
  l.DrawLatex(.13, .88, "Run7 Au+Au 0-20% central");
  gPad->SaveAs("EffAuAu.png");

  // Ratio
  new TCanvas;
  TF2* fratio = new TF2 ( "fratio", TF2_EffRatio_20, ptmin, ptmax, -1, 1, 0);
  dummy->Reset();
  dummy->Eval ( fratio, "R" );
  dummy->Draw("surf1");
  l.DrawLatex(.13, .88, "pp / Au+Au");
  gPad->SaveAs("EffRatio.png");


  // 1D Ratio
  new TCanvas;
  TF1* fratio1D = new TF1 ( "fratio1D", TF1_EffRatio_20, 0.2, 5, 1 );
  TF1* fratio1Dp = new TF1 ( "fratio1D", TF1_EffRatio_20, 0.2, 5, 1 );
  fratio1Dp->SetParameter(0,1);
  TF1* fratio1Dm = new TF1 ( "fratio1D", TF1_EffRatio_20, 0.2, 5, 1 );
  fratio1Dm->SetParameter(0,-1);

  // fratio1D->SetTitle( "pp / Au+Au at #eta=0");
  fratio1D->SetTitle( "");
  TH2D* dummy2 = new TH2D( "dummy2", ";p_{T} [GeV/c]; efficiency ratio", 100, 0.2, 5, 100, 0.0, 1.05);
  dummy2->Draw();
  fratio1D->Draw("same");
  fratio1Dp->SetLineStyle(7);
  fratio1Dp->Draw("same");
  fratio1Dm->SetLineStyle(7);
  fratio1Dm->Draw("same");


  l.DrawLatex(.13, .92, "pp / Au+Au at #eta=0");
  gPad->SaveAs("EffRatio1D.png");

  
}
