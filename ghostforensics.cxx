{
  gStyle->SetOptStat(0); 

  
  TFile* lg = new TFile( "AjResults/testlg.root");
  TFile* sg = new TFile( "AjResults/testsg.root");

  TString which = "j1area";
  TH1D* hl = lg->Get( which );  hl->SetName("hl");
  TH1D* hs = sg->Get( which );  hs->SetName("hs");

  if ( which.Contains("area") ){    
    hl->SetAxisRange(0.4, 0.7, "x");
    hs->SetAxisRange(0.4, 0.7, "x");
  }
  
  
  hl->SetLineColor(kBlack);
  hs->SetLineColor(kGreen+1);

  new TCanvas;
  hs->Draw();
  hl->Draw("same");

  TLegend* leg = new TLegend( 0.65, 0.7, 0.88, 0.88, which,"brNDC");
  leg->AddEntry( hl->GetName(), "ghostarea =  0.01", "l");
  leg->AddEntry( hs->GetName(), "ghostarea = 0.001", "l");
  leg->Draw();

  TLatex l;
  l.SetNDC();  l.SetTextSize(0.045);

  char text[400];
  sprintf (text, "Mean = %.3f", hl->GetMean());
  l.SetTextColor( hl->GetLineColor() ); l.DrawLatex ( 0.6, 0.5, text);
    sprintf (text, "Mean = %.3f", hs->GetMean());
  l.SetTextColor( hs->GetLineColor() ); l.DrawLatex ( 0.6, 0.42, text);

  sprintf (text, "RMS = %.3f", hl->GetRMS());
  l.SetTextColor( hl->GetLineColor() ); l.DrawLatex ( 0.6, 0.3, text);
    sprintf (text, "RMS = %.3f", hs->GetRMS());
  l.SetTextColor( hs->GetLineColor() ); l.DrawLatex ( 0.6, 0.22, text);

  
  gPad->SaveAs(which+".png");

}
