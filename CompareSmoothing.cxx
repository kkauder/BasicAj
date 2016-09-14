//int CompareSmoothing( TString inname = "HThistos/Unfolded_SmoothX_TrainedWith_Groom_Aj_HT54_HTled_TrueMB_NoEff_GeantMc.root"	) {

{
  // TString inname = "HThistos/Unfolded_SmoothX_TrainedWith_Groom_Aj_HT54_HTled_TrueMB_NoEff_GeantMc.root";
  TString inname = "HThistos/Unfolded_SmoothX_TrainedWith_Groom_Aj_TrueMB_NoEff_GeantMc.root";

  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);

  const int NX=5;

  TH1D* h0[NX];
  TH1D* h1[NX];
  TH1D* h2[NX];
  for ( int i=0; i<NX; ++i ){
    int X = i+1;
    TString sX = "Smooth"; sX+=X;
    TString fname = inname;
    fname.ReplaceAll( "SmoothX", sX);
    cout << fname << endl;
    TFile * f = new TFile (fname, "READ");

    h0[i] = (TH1D*) f->Get( "unfold0" );
    TString newname = h0[i]->GetName();
    newname += "_";    newname += X;
    h0[i]->SetName( newname );


    h1[i] = (TH1D*) f->Get( "unfold1" );
    TString newname = h1[i]->GetName();
    newname += "_";    newname += X;
    h1[i]->SetName( newname );

    h2[i] = (TH1D*) f->Get( "unfold2" );
    TString newname = h2[i]->GetName();
    newname += "_";    newname += X;
    h2[i]->SetName( newname );
  }
  TLegend* leg=0;
  int i=0;
  TString title;
  TString outname;
    
  // ---------------------------------------------------------------------------
  new TCanvas; 
  gPad->SetGridx(0);  gPad->SetGridy(0);
  leg = new TLegend( 0.55, 0.6, 0.89, 0.9, "Leading Jet z_{g}, 10<p_{T}^{part}<20 GeV/c " );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  i=0;
  h0[i]->SetTitle("");
  h0[i]->Draw();
  title="Smoothing Par = ";  title+=i;
  leg->AddEntry( h0[i]->GetName(), title, "l");
  for ( int i=1; i<NX; ++i ){
    h0[i]->SetLineColor(i+5);
    h0[i]->Draw("same");
    title="Smoothing Par = ";  title+=i;
    leg->AddEntry( h0[i]->GetName(), title, "l");
  }
  leg->Draw();
  TString outname=gSystem->BaseName(inname);
  outname.Prepend("1020_");
  outname.Prepend("HTplots/");
  outname.ReplaceAll(".root","");
  gPad->SaveAs( outname+".png" );

  // ---------------------------------------------------------------------------
  new TCanvas; 
  gPad->SetGridx(0);  gPad->SetGridy(0);
  leg = new TLegend( 0.55, 0.6, 0.89, 0.9, "Leading Jet z_{g}, 20<p_{T}^{part}<30 GeV/c " );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  i=0;
  h1[i]->SetTitle("");
  h1[i]->Draw();
  title="Smoothing Par = ";  title+=i;
  leg->AddEntry( h1[i]->GetName(), title, "l");
  for ( int i=1; i<NX; ++i ){
    h1[i]->SetLineColor(i+5);
    h1[i]->Draw("same");
    title="Smoothing Par = ";  title+=i;
    leg->AddEntry( h1[i]->GetName(), title, "l");
  }
  leg->Draw();
  outname=gSystem->BaseName(inname);
  outname.Prepend("2030_");
  outname.Prepend("HTplots/");
  outname.ReplaceAll(".root","");
  gPad->SaveAs( outname+".png" );

  // ---------------------------------------------------------------------------
  new TCanvas; 
  gPad->SetGridx(0);  gPad->SetGridy(0);
  leg = new TLegend( 0.55, 0.6, 0.89, 0.9, "Leading Jet z_{g}, 30<p_{T}^{part}<40 GeV/c " );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  i=0;
  h2[i]->SetTitle("");
  h2[i]->Draw();
  title="Smoothing Par = ";  title+=i;
  leg->AddEntry( h2[i]->GetName(), title, "l");
  for ( int i=1; i<NX; ++i ){
    h2[i]->SetLineColor(i+5);
    h2[i]->Draw("same");
    title="Smoothing Par = ";  title+=i;
    leg->AddEntry( h2[i]->GetName(), title, "l");
  }
  leg->Draw();
  outname=gSystem->BaseName(inname);
  outname.Prepend("3040_");
  outname.Prepend("HTplots/");
  outname.ReplaceAll(".root","");
  gPad->SaveAs( outname+".png" );
  // ---------------------------------------------------------------------------

  return 0;
}
