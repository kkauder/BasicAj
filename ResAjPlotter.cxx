// int ResAjPlotter(TString fs = "AjResults/R0.2_ForResAj_HT54_Geant_MatchedTo_R0.2_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root")
int ResAjPlotter(TString fs = "AjResults/ForResAj_HT54_Geant_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root")
{
  // TFile *f = TFile::Open("AjResults/ForResAj_Geant_MatchedTo_ForResAj_NoEff_NoCut_GeantMc.root");
  // TFile *f = TFile::Open("AjResults/ForResAj_Geant_NoOutliers_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");
  // TFile *f = TFile::Open("AjResults/ForResAj_Geant_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");
  // TFile *f = TFile::Open("AjResults/ForResAj_Geant_NoOutliers_MatchedTo_ForResAj_NoEff_NoCut_GeantMc.root");
  // TFile *f = TFile::Open("AjResults/ForResAj_HT54_Geant_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");
  // TFile *f = TFile::Open("AjResults/R0.2_ForResAj_HT54_Geant_MatchedTo_R0.2_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");

  
  TFile *f = TFile::Open(fs);  

  TString fouts = fs;
  fouts.ReplaceAll("AjResults/","AjResults/Processed_");
  TFile *fout = new TFile( fouts, "RECREATE");
  
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);

  TLegend* leg = 0;
  bool IsR02 = TString( f->GetName() ).Contains("R0.2");
  TString base="plots/ResAj_R0.4_";
  if ( IsR02 ) base="plots/ResAj_R0.2_";
  TString Rstring = "R=0.4";
  if ( IsR02 ) Rstring = "R=0.2";
  
  TH1D* McTriggerPt = (TH1D*) f->Get("McTriggerPt");
  TH1D* PpTriggerPt = (TH1D*) f->Get("PpTriggerPt");
  TH2D* PpMcTriggerPt = (TH2D*) f->Get("PpMcTriggerPt");
  TH2D* DeltaTriggerPt    = (TH2D*) f->Get("DeltaTriggerPt");
  TH2D* RelDeltaTriggerPt = (TH2D*) f->Get("RelDeltaTriggerPt");

  TH1D* McTriggerPtLo = (TH1D*) f->Get("McTriggerPtLo");
  TH1D* PpTriggerPtLo = (TH1D*) f->Get("PpTriggerPtLo");
  TH2D* PpMcTriggerPtLo = (TH2D*) f->Get("PpMcTriggerPtLo");
  TH2D* DeltaTriggerPtLo    = (TH2D*) f->Get("DeltaTriggerPtLo");
  TH2D* RelDeltaTriggerPtLo = (TH2D*) f->Get("RelDeltaTriggerPtLo");

  TH1D* McRecoilPt = (TH1D*) f->Get("McRecoilPt");
  TH1D* PpRecoilPt = (TH1D*) f->Get("PpRecoilPt");
  TH2D* PpMcRecoilPt = (TH2D*) f->Get("PpMcRecoilPt");
  TH2D* DeltaRecoilPt    = (TH2D*) f->Get("DeltaRecoilPt");
  TH2D* RelDeltaRecoilPt = (TH2D*) f->Get("RelDeltaRecoilPt");

  TH1D* McRecoilPtLo = (TH1D*) f->Get("McRecoilPtLo");
  TH1D* PpRecoilPtLo = (TH1D*) f->Get("PpRecoilPtLo");
  TH2D* PpMcRecoilPtLo = (TH2D*) f->Get("PpMcRecoilPtLo");
  TH2D* DeltaRecoilPtLo    = (TH2D*) f->Get("DeltaRecoilPtLo");
  TH2D* RelDeltaRecoilPtLo = (TH2D*) f->Get("RelDeltaRecoilPtLo");

  new TCanvas;
  gPad->SaveAs( base+"All.pdf[");

  // JER - a la Fig 3 in hJet paper
  // ------------------------------

  // Trigger, High Cut
  // -----------------
  TH1D* trig31 = (TH1D*) PpMcTriggerPt->ProjectionX("trig31",31,31);
  trig31->Fit("gaus","","goff");
  TF1* gtrig31 = trig31->GetFunction("gaus");
  gtrig31->SetName("gtrig31");
  trig31->SetLineColor(kRed);
  gtrig31->SetLineColor(kRed);
  double sigtrig31 = gtrig31->GetParameter(2);
  double mutrig31 = gtrig31->GetParameter(1);
  double dettrig31 = 31-0.5;

  
  TH1D* trig36 = (TH1D*) PpMcTriggerPt->ProjectionX("trig36",36,36);
  trig36->Fit("gaus","","goff");
  TF1* gtrig36 = trig36->GetFunction("gaus");
  gtrig36->SetName("gtrig36");
  trig36->SetLineColor(kGreen+1);
  gtrig36->SetLineColor(kGreen+1);
  double sigtrig36 = gtrig36->GetParameter(2);
  double mutrig36 = gtrig36->GetParameter(1);
  double dettrig36 = 36-0.5;

  TH1D* trig41 = (TH1D*) PpMcTriggerPt->ProjectionX("trig41",41,41);
  trig41->Fit("gaus","","goff");
  TF1* gtrig41 = trig41->GetFunction("gaus");
  gtrig41->SetName("gtrig41");
  trig41->SetLineColor(kBlue);
  gtrig41->SetLineColor(kBlue);
  double sigtrig41 = gtrig41->GetParameter(2);
  double mutrig41 = gtrig41->GetParameter(1);
  double dettrig41 = 41-0.5;

  trig31->SetAxisRange(10,50,"x");
  trig31->SetAxisRange(0,2.0,"y");
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.037);
  char plabel[1000];

  leg = new TLegend( 0.57, 0.6, 0.9, 0.9, Rstring+", Leading Jet" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  leg->SetTextSize(0.04);
  
  trig31->Draw();
  gtrig31->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    dettrig31,int ( 100*sigtrig31/dettrig31+0.5), int ( 100*sigtrig31/mutrig31+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", dettrig31-0.5,dettrig31+0.5);
  leg->AddEntry(trig31->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigtrig31/dettrig31+0.5), int ( 100*sigtrig31/mutrig31+0.5));
  latex.SetTextColor(kRed);
  latex.DrawLatex( 0.15, 0.7, plabel);
  
  trig36->Draw("same");
  gtrig36->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    dettrig36,int ( 100*sigtrig36/dettrig36+0.5), int ( 100*sigtrig36/mutrig36+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", dettrig36-0.5,dettrig36+0.5);
  leg->AddEntry(trig36->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigtrig36/dettrig36+0.5), int ( 100*sigtrig36/mutrig36+0.5));
  latex.SetTextColor(kGreen+1);
  latex.DrawLatex( 0.5, 0.4, plabel);

  trig41->Draw("same");
  gtrig41->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    dettrig41,int ( 100*sigtrig41/dettrig41+0.5), int ( 100*sigtrig41/mutrig41+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{ part}<%.0f GeV/c", dettrig41-0.5,dettrig41+0.5);
  leg->AddEntry(trig41->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigtrig41/dettrig41+0.5), int ( 100*sigtrig41/mutrig41+0.5));
  latex.SetTextColor(kBlue);
  latex.DrawLatex( 0.65, 0.25, plabel);

  leg->Draw();
  gPad->SaveAs( base+"TriggerHighJER.png");
  gPad->SaveAs( base+"All.pdf");
    

  // Recoil, High Cut
  // ----------------
  new TCanvas;

  // TH1D* recoil21 = (TH1D*) PpMcRecoilPt->ProjectionX("recoil21",21,21);
  // recoil21->Fit("gaus","","goff");
  // TF1* grecoil21 = recoil21->GetFunction("gaus");
  // grecoil21->SetName("grecoil21");
  // recoil21->SetLineColor(kBlack);
  // grecoil21->SetLineColor(kBlack);
  // double sigrecoil21 = grecoil21->GetParameter(2);
  // double murecoil21 = grecoil21->GetParameter(1);
  // double detrecoil21 = 21-0.5;

  TH1D* recoil26 = (TH1D*) PpMcRecoilPt->ProjectionX("recoil26",26,26);
  recoil26->Fit("gaus","","goff");
  TF1* grecoil26 = recoil26->GetFunction("gaus");
  grecoil26->SetName("grecoil26");
  recoil26->SetLineColor(kYellow+2);
  grecoil26->SetLineColor(kYellow+2);
  double sigrecoil26 = grecoil26->GetParameter(2);
  double murecoil26 = grecoil26->GetParameter(1);
  double detrecoil26 = 26-0.5;


  TH1D* recoil31 = (TH1D*) PpMcRecoilPt->ProjectionX("recoil31",31,31);
  recoil31->Fit("gaus","","goff");
  TF1* grecoil31 = recoil31->GetFunction("gaus");
  grecoil31->SetName("grecoil31");
  recoil31->SetLineColor(kRed);
  grecoil31->SetLineColor(kRed);
  double sigrecoil31 = grecoil31->GetParameter(2);
  double murecoil31 = grecoil31->GetParameter(1);
  double detrecoil31 = 31-0.5;

  
  TH1D* recoil36 = (TH1D*) PpMcRecoilPt->ProjectionX("recoil36",36,36);
  recoil36->Fit("gaus","","goff");
  TF1* grecoil36 = recoil36->GetFunction("gaus");
  grecoil36->SetName("grecoil36");
  recoil36->SetLineColor(kGreen+1);
  grecoil36->SetLineColor(kGreen+1);
  double sigrecoil36 = grecoil36->GetParameter(2);
  double murecoil36 = grecoil36->GetParameter(1);
  double detrecoil36 = 36-0.5;


  leg = new TLegend( 0.57, 0.6, 0.9, 0.9, Rstring+", SubLeading Jet" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  leg->SetTextSize(0.04);
  
  // recoil21->SetAxisRange(0,40,"x");
  // recoil21->SetAxisRange(0,3.0,"y");

  // recoil21->Draw();
  // grecoil21->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    detrecoil21,int ( 100*sigrecoil21/detrecoil21+0.5), int ( 100*sigrecoil21/murecoil21+0.5));
  // latex.SetTextColor(kBlack);
  // latex.DrawLatex( 0.12, 0.7, plabel);

  recoil26->SetAxisRange(3,43,"x");
  recoil26->SetAxisRange(0,2.0,"y");

  recoil26->Draw();
  grecoil26->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    detrecoil26,int ( 100*sigrecoil26/detrecoil26+0.5), int ( 100*sigrecoil26/murecoil26+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", detrecoil26-0.5,detrecoil26+0.5);
  leg->AddEntry(recoil26->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigrecoil26/detrecoil26+0.5), int ( 100*sigrecoil26/murecoil26+0.5));
  latex.SetTextColor(kYellow+2);
  latex.DrawLatex( 0.2, 0.7, plabel);


  recoil31->Draw("same");
  grecoil31->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    detrecoil31,int ( 100*sigrecoil31/detrecoil31+0.5), int ( 100*sigrecoil31/murecoil31+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", detrecoil31-0.5,detrecoil31+0.5);
  leg->AddEntry(recoil31->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigrecoil31/detrecoil31+0.5), int ( 100*sigrecoil31/murecoil31+0.5));
  latex.SetTextColor(kRed);
  latex.DrawLatex( 0.5, 0.45, plabel);
  
  recoil36->Draw("same");
  grecoil36->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    detrecoil36,int ( 100*sigrecoil36/detrecoil36+0.5), int ( 100*sigrecoil36/murecoil36+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", detrecoil36-0.5,detrecoil36+0.5);
  leg->AddEntry(recoil36->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigrecoil36/detrecoil36+0.5), int ( 100*sigrecoil36/murecoil36+0.5));
  latex.SetTextColor(kGreen+1);
  latex.DrawLatex( 0.62, 0.25, plabel);

  leg->Draw();
  gPad->SaveAs( base+"RecoilHighJER.png");
  gPad->SaveAs( base+"All.pdf");



  // Trigger, Low Cut
  // -----------------
  new TCanvas;
  TH1D* triglo31 = (TH1D*) PpMcTriggerPtLo->ProjectionX("triglo31",31,31);
  triglo31->Fit("gaus","","goff");
  TF1* gtriglo31 = triglo31->GetFunction("gaus");
  gtriglo31->SetName("gtriglo31");
  triglo31->SetLineColor(kRed);
  gtriglo31->SetLineColor(kRed);
  double sigtriglo31 = gtriglo31->GetParameter(2);
  double mutriglo31 = gtriglo31->GetParameter(1);
  double dettriglo31 = 31-0.5;

  
  TH1D* triglo36 = (TH1D*) PpMcTriggerPtLo->ProjectionX("triglo36",36,36);
  triglo36->Fit("gaus","","goff");
  TF1* gtriglo36 = triglo36->GetFunction("gaus");
  gtriglo36->SetName("gtriglo36");
  triglo36->SetLineColor(kGreen+1);
  gtriglo36->SetLineColor(kGreen+1);
  double sigtriglo36 = gtriglo36->GetParameter(2);
  double mutriglo36 = gtriglo36->GetParameter(1);
  double dettriglo36 = 36-0.5;

  TH1D* triglo41 = (TH1D*) PpMcTriggerPtLo->ProjectionX("triglo41",41,41);
  triglo41->Fit("gaus","","goff");
  TF1* gtriglo41 = triglo41->GetFunction("gaus");
  gtriglo41->SetName("gtriglo41");
  triglo41->SetLineColor(kBlue);
  gtriglo41->SetLineColor(kBlue);
  double sigtriglo41 = gtriglo41->GetParameter(2);
  double mutriglo41 = gtriglo41->GetParameter(1);
  double dettriglo41 = 41-0.5;

  triglo31->SetAxisRange(15,55,"x");
  triglo31->SetAxisRange(0,2.0,"y");

  leg = new TLegend( 0.57, 0.6, 0.9, 0.9, Rstring+", Matched Leading" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  leg->SetTextSize(0.04);
  
  triglo31->Draw();
  gtriglo31->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    dettriglo31,int ( 100*sigtriglo31/dettriglo31+0.5), int ( 100*sigtriglo31/mutriglo31+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", dettriglo31-0.5,dettriglo31+0.5);
  leg->AddEntry(triglo31->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigtriglo31/dettriglo31+0.5), int ( 100*sigtriglo31/mutriglo31+0.5));
  latex.SetTextColor(kRed);
  latex.DrawLatex( 0.15, 0.7, plabel);
  
  triglo36->Draw("same");
  gtriglo36->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    dettriglo36,int ( 100*sigtriglo36/dettriglo36+0.5), int ( 100*sigtriglo36/mutriglo36+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", dettriglo36-0.5,dettriglo36+0.5);
  leg->AddEntry(triglo36->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigtriglo36/dettriglo36+0.5), int ( 100*sigtriglo36/mutriglo36+0.5));
  latex.SetTextColor(kGreen+1);
  latex.DrawLatex( 0.5, 0.45, plabel);

  triglo41->Draw("same");
  gtriglo41->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    dettriglo41,int ( 100*sigtriglo41/dettriglo41+0.5), int ( 100*sigtriglo41/mutriglo41+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", dettriglo41-0.5,dettriglo41+0.5);
  leg->AddEntry(triglo41->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigtriglo41/dettriglo41+0.5), int ( 100*sigtriglo41/mutriglo41+0.5));
  latex.SetTextColor(kBlue);
  latex.DrawLatex( 0.67, 0.25, plabel);

  leg->Draw();
  gPad->SaveAs( base+"TriggerLowJER.png");
  gPad->SaveAs( base+"All.pdf");

  // Recoillo, High Cut
  // ----------------
  new TCanvas;

  // TH1D* recoillo21 = (TH1D*) PpMcRecoilPtLo->ProjectionX("recoillo21",21,21);
  // recoillo21->Fit("gaus","","goff");
  // TF1* grecoillo21 = recoillo21->GetFunction("gaus");
  // grecoillo21->SetName("grecoillo21");
  // recoillo21->SetLineColor(kBlack);
  // grecoillo21->SetLineColor(kBlack);
  // double sigrecoillo21 = grecoillo21->GetParameter(2);
  // double murecoillo21 = grecoillo21->GetParameter(1);
  // double detrecoillo21 = 21-0.5;

  TH1D* recoillo26 = (TH1D*) PpMcRecoilPtLo->ProjectionX("recoillo26",26,26);
  recoillo26->Fit("gaus","","goff");
  TF1* grecoillo26 = recoillo26->GetFunction("gaus");
  grecoillo26->SetName("grecoillo26");
  recoillo26->SetLineColor(kYellow+2);
  grecoillo26->SetLineColor(kYellow+2);
  double sigrecoillo26 = grecoillo26->GetParameter(2);
  double murecoillo26 = grecoillo26->GetParameter(1);
  double detrecoillo26 = 26-0.5;


  TH1D* recoillo31 = (TH1D*) PpMcRecoilPtLo->ProjectionX("recoillo31",31,31);
  recoillo31->Fit("gaus","","goff");
  TF1* grecoillo31 = recoillo31->GetFunction("gaus");
  grecoillo31->SetName("grecoillo31");
  recoillo31->SetLineColor(kRed);
  grecoillo31->SetLineColor(kRed);
  double sigrecoillo31 = grecoillo31->GetParameter(2);
  double murecoillo31 = grecoillo31->GetParameter(1);
  double detrecoillo31 = 31-0.5;

  
  TH1D* recoillo36 = (TH1D*) PpMcRecoilPtLo->ProjectionX("recoillo36",36,36);
  recoillo36->Fit("gaus","","goff");
  TF1* grecoillo36 = recoillo36->GetFunction("gaus");
  grecoillo36->SetName("grecoillo36");
  recoillo36->SetLineColor(kGreen+1);
  grecoillo36->SetLineColor(kGreen+1);
  double sigrecoillo36 = grecoillo36->GetParameter(2);
  double murecoillo36 = grecoillo36->GetParameter(1);
  double detrecoillo36 = 36-0.5;


  leg = new TLegend( 0.57, 0.6, 0.9, 0.9, Rstring+", Matched SubLead" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  leg->SetTextSize(0.04);
  
  // recoillo21->SetAxisRange(0,40,"x");
  // recoillo21->SetAxisRange(0,3.0,"y");

  // recoillo21->Draw();
  // grecoillo21->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    detrecoillo21,int ( 100*sigrecoillo21/detrecoillo21+0.5), int ( 100*sigrecoillo21/murecoillo21+0.5));
  // latex.SetTextColor(kBlack);
  // latex.DrawLatex( 0.12, 0.7, plabel);

  recoillo26->SetAxisRange(8,51,"x");
  recoillo26->SetAxisRange(0,2.0,"y");

  recoillo26->Draw();
  grecoillo26->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    detrecoillo26,int ( 100*sigrecoillo26/detrecoillo26+0.5), int ( 100*sigrecoillo26/murecoillo26+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", detrecoillo26-0.5,detrecoillo26+0.5);
  leg->AddEntry(recoillo26->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigrecoillo26/detrecoillo26+0.5), int ( 100*sigrecoillo26/murecoillo26+0.5));
  latex.SetTextColor(kYellow+2);
  latex.DrawLatex( 0.2, 0.7, plabel);


  recoillo31->Draw("same");
  grecoillo31->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    detrecoillo31,int ( 100*sigrecoillo31/detrecoillo31+0.5), int ( 100*sigrecoillo31/murecoillo31+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", detrecoillo31-0.5,detrecoillo31+0.5);
  leg->AddEntry(recoillo31->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigrecoillo31/detrecoillo31+0.5), int ( 100*sigrecoillo31/murecoillo31+0.5));
  latex.SetTextColor(kRed);
  latex.DrawLatex( 0.5, 0.45, plabel);
  
  recoillo36->Draw("same");
  grecoillo36->Draw("same");
  // sprintf ( plabel, "#splitline{p_{T}^{part}=%.1f GeV/c}{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%, #frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
  // 	    detrecoillo36,int ( 100*sigrecoillo36/detrecoillo36+0.5), int ( 100*sigrecoillo36/murecoillo36+0.5));
  sprintf ( plabel, "%.0f<p_{T}^{part}<%.0f GeV/c", detrecoillo36-0.5,detrecoillo36+0.5);
  leg->AddEntry(recoillo36->GetName(),plabel,"L");
  sprintf ( plabel, "#splitline{#frac{#sigma p_{T}^{det}}{p_{T}^{part}} = %d%%}{#frac{#sigma p_{T}^{det}}{p_{T}^{det}} = %d%%}",
	    int ( 100*sigrecoillo36/detrecoillo36+0.5), int ( 100*sigrecoillo36/murecoillo36+0.5));
  latex.SetTextColor(kGreen+1);
  latex.DrawLatex( 0.62, 0.25, plabel);

  leg->Draw();
  gPad->SaveAs( base+"RecoilLowJER.png");
  gPad->SaveAs( base+"All.pdf");

  // Spectra
  // -------
  new TCanvas;
  // gPad->SaveAs( base+"All.pdf[");
  gPad->SetLogy();
  PpTriggerPt->SetLineColor(kRed);
  PpTriggerPt->Draw();
  PpTriggerPtLo->SetLineColor(kBlue);
  PpTriggerPtLo->Draw("same");
  McTriggerPt->SetLineColor(kGreen+1);
  McTriggerPt->Draw("same");

  leg = new TLegend( 0.55, 0.65, 0.9, 0.85, Rstring+", Leading Jet p_{T}" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);

  leg->AddEntry( PpTriggerPt, "Detector level, hard core");
  leg->AddEntry( PpTriggerPtLo, "Detector level, matched");
  leg->AddEntry( McTriggerPt, "Particle Level");
  leg->Draw();
  gPad->SaveAs( base+"LeadPt.png");
  gPad->SaveAs( base+"All.pdf");

  new TCanvas;
  gPad->SetLogy();
  PpRecoilPt->SetLineColor(kRed);
  PpRecoilPt->Draw();
  PpRecoilPtLo->SetLineColor(kBlue);
  PpRecoilPtLo->Draw("same");
  McRecoilPt->SetLineColor(kGreen+1);
  McRecoilPt->Draw("same");

  leg = new TLegend( 0.55, 0.65, 0.9, 0.85, Rstring+", SubLeading Jet p_{T}" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);

  leg->AddEntry( PpRecoilPt, "Detector level");
  leg->AddEntry( PpRecoilPtLo, "Detector level, matched");
  leg->AddEntry( McRecoilPt, "Particle Level");
  leg->Draw();
  gPad->SaveAs( base+"SubLeadPt.png");
  gPad->SaveAs( base+"All.pdf");

  // Absolute shift
  // --------------
  TH1D* DelTrigger = (TH1D*) DeltaTriggerPt->ProjectionY("DelTrigger");
  TH1D* DelRecoil  = (TH1D*) DeltaRecoilPt->ProjectionY("DelRecoil");
  TH1D* DelTriggerLo = (TH1D*) DeltaTriggerPtLo->ProjectionY("DelTriggerLo");
  TH1D* DelRecoilLo  = (TH1D*) DeltaRecoilPtLo->ProjectionY("DelRecoilLo");
  char s[1000];
  new TCanvas;
  gPad->SetLogy();

  DelTrigger->SetAxisRange( 1e-5, 1e4, "y");
  DelTrigger->SetLineColor(kBlack);
  DelTrigger->Draw();
  DelTriggerLo->SetLineColor(kGray);
  DelTriggerLo->Draw("same");
  DelRecoil->SetLineColor(kMagenta);
  DelRecoil->Draw("same");
  DelRecoilLo->SetLineColor(kTeal);
  DelRecoilLo->Draw("same");

  leg = new TLegend( 0.15, 0.5, 0.5, 0.85, Rstring+", #Delta p_{T}" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);

  leg->AddEntry( DelTrigger, "Leading Jet");
  sprintf (s, "#mu=%0.1f, #sigma=%0.1f",DelTrigger->GetMean(), DelTrigger->GetRMS());
  leg->AddEntry( (TH1D*) NULL, s,"");

  leg->AddEntry( DelTriggerLo, "matched");
  sprintf (s, "#mu=%0.1f, #sigma=%0.1f",DelTriggerLo->GetMean(), DelTriggerLo->GetRMS());
  leg->AddEntry( (TH1D*) NULL, s,"");

  leg->AddEntry( DelRecoil, "SubLeading Jet");
  sprintf (s, "#mu=%0.1f, #sigma=%0.1f",DelRecoil->GetMean(), DelRecoil->GetRMS());
  leg->AddEntry( (TH1D*) NULL, s,"");

  leg->AddEntry( DelRecoilLo, "matched");
  sprintf (s, "#mu=%0.1f, #sigma=%0.1f",DelRecoilLo->GetMean(), DelRecoilLo->GetRMS());
  leg->AddEntry( (TH1D*) NULL, s,"");

  leg->Draw();
  gPad->SaveAs( base+"Del.png");
  gPad->SaveAs( base+"All.pdf");

  // Relative shift
  // --------------
  TH1D* RelDelTrigger = (TH1D*) RelDeltaTriggerPt->ProjectionY("RelDelTrigger");
  TH1D* RelDelRecoil  = (TH1D*) RelDeltaRecoilPt->ProjectionY("RelDelRecoil");
  TH1D* RelDelTriggerLo = (TH1D*) RelDeltaTriggerPtLo->ProjectionY("RelDelTriggerLo");
  TH1D* RelDelRecoilLo  = (TH1D*) RelDeltaRecoilPtLo->ProjectionY("RelDelRecoilLo");
  char s[1000];
  new TCanvas;
  gPad->SetLogy();
  RelDelTrigger->SetAxisRange(-3,2,"x");
  RelDelTrigger->SetAxisRange(1e-5,1e3,"y");
  RelDelTrigger->SetLineColor(kBlack);
  RelDelTrigger->Draw();
  RelDelTriggerLo->SetLineColor(kGray);
  RelDelTriggerLo->Draw("same");
  RelDelRecoil->SetLineColor(kMagenta);
  RelDelRecoil->Draw("same");
  RelDelRecoilLo->SetLineColor(kTeal);
  RelDelRecoilLo->Draw("same");

  
  leg = new TLegend( 0.15, 0.5, 0.5, 0.85, Rstring+", #Delta p_{T} / p_{T}" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);

  leg->AddEntry( RelDelTrigger, "Leading Jet");
  sprintf (s, "#mu=%0.2f, #sigma=%0.1f",RelDelTrigger->GetMean(), RelDelTrigger->GetRMS());
  leg->AddEntry( (TH1D*) NULL, s,"");

  leg->AddEntry( RelDelTriggerLo, "matched");
  sprintf (s, "#mu=%0.2f, #sigma=%0.1f",RelDelTriggerLo->GetMean(), RelDelTriggerLo->GetRMS());
  leg->AddEntry( (TH1D*) NULL, s,"");

  leg->AddEntry( RelDelRecoil, "SubLeading Jet");
  sprintf (s, "#mu=%0.2f, #sigma=%0.1f",RelDelRecoil->GetMean(), RelDelRecoil->GetRMS());
  leg->AddEntry( (TH1D*) NULL, s,"");

  leg->AddEntry( RelDelRecoilLo, "matched");
  sprintf (s, "#mu=%0.2f, #sigma=%0.1f",RelDelRecoilLo->GetMean(), RelDelRecoilLo->GetRMS());
  leg->AddEntry( (TH1D*) NULL, s,"");


  leg->Draw();
  gPad->SaveAs( base+"RelDel.png");
  gPad->SaveAs( base+"All.pdf");


  // 2D

  new TCanvas ( "Profile1","",500, 500);
  gPad->SetCanvasSize(450,450);

  int rebin = 4;
  float ptmin=0;
  // float ptmax=35;
  float ptmax=50;
  PpMcTriggerPt->Rebin2D(rebin,rebin);
  PpMcTriggerPtLo->Rebin2D(rebin,rebin);
  TProfile* PpMcTriggerPtProf = PpMcTriggerPt->ProfileX("PpMcTriggerPtProf",1,-1,"s");
  TProfile* PpMcTriggerPtProfLo = PpMcTriggerPtLo->ProfileX("PpMcTriggerPtProfLo",1,-1,"s");

  float axptmin=0;
  float axptmax=60;
  TH2D* dummy = new TH2D( "dummy",";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c]", 100, axptmin,axptmax,100, axptmin,axptmax);
  dummy->Draw("AXIS");

  PpMcTriggerPtProf->GetYaxis()->SetTitle("p_{T}^{Part} [GeV/c]");
  PpMcTriggerPtProf->SetAxisRange( ptmin, ptmax, "x");
  PpMcTriggerPtProf->SetAxisRange( ptmin, ptmax, "y");
  PpMcTriggerPtProfLo->GetYaxis()->SetTitle("p_{T}^{Part} [GeV/c]");
  if ( IsR02 )   PpMcTriggerPtProfLo->SetAxisRange( ptmin, ptmax, "x");
  else           PpMcTriggerPtProfLo->SetAxisRange( 18, ptmax, "x");
  PpMcTriggerPtProfLo->SetAxisRange( ptmin, ptmax, "y");

  PpMcTriggerPtProf->SetLineColor(kRed);
  PpMcTriggerPtProfLo->SetLineColor(kBlue);
  PpMcTriggerPtProfLo->Draw("same");
  PpMcTriggerPtProf->Draw("same");

  latex.SetTextSize(0.045);

  TLine l;
  l.SetLineStyle(7);
  l.DrawLine( axptmin, axptmin, axptmax, axptmax );

  latex.SetTextColor(kBlack);
  latex.DrawLatex( 0.15, 0.8, Rstring+", Leading Jets" );
  latex.SetTextColor( kAzure-6 );   
  latex.DrawLatex( 0.15,0.73, "STAR Simulation");

  // leg = new TLegend( 0.15, 0.6, 0.7, 0.85, Rstring+", Leading Jets" );
  leg = new TLegend( 0.32, 0.14, 0.87, 0.28 );
  // leg = new TLegend( 0.34, 0.12, 0.89, 0.34, 
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  
  leg->AddEntry(   PpMcTriggerPtProf, "p_{T}^{Cut}>2 GeV/c");
  leg->AddEntry(   PpMcTriggerPtProfLo, "p_{T}^{Cut}>0.2 GeV/c, Matched");


  leg->Draw();
  gPad->SaveAs( base+"LeadProfile.png");
  gPad->SaveAs( base+"All.pdf");

  new TCanvas ( "Profile2","",500, 500);
  gPad->SetCanvasSize(450,450);

  PpMcRecoilPt->Rebin2D(rebin,rebin);
  PpMcRecoilPtLo->Rebin2D(rebin,rebin);
  TProfile* PpMcRecoilPtProf = PpMcRecoilPt->ProfileX("PpMcRecoilPtProf",1,-1,"s");
  TProfile* PpMcRecoilPtProfLo = PpMcRecoilPtLo->ProfileX("PpMcRecoilPtProfLo",1,-1,"s");

    
  // ptmin=0;
  // ptmax=50;
  dummy->Draw("AXIS");
  PpMcRecoilPtProf->GetYaxis()->SetTitle("p_{T}^{Part}");
  PpMcRecoilPtProf->SetAxisRange( ptmin, ptmax, "x");
  PpMcRecoilPtProf->SetAxisRange( ptmin, ptmax, "y");
  PpMcRecoilPtProfLo->GetYaxis()->SetTitle("p_{T}^{Part} [GeV/c]");
  if ( IsR02 ) PpMcRecoilPtProfLo->SetAxisRange( ptmin, ptmax, "x");
  else         PpMcRecoilPtProfLo->SetAxisRange( 10, ptmax, "x");
  PpMcRecoilPtProfLo->SetAxisRange( ptmin, ptmax, "y");

  PpMcRecoilPtProf->SetLineColor(kRed);
  PpMcRecoilPtProfLo->SetLineColor(kBlue);

  PpMcRecoilPtProfLo->Draw("same");
  PpMcRecoilPtProf->Draw("same");

  l.DrawLine( axptmin, axptmin, axptmax, axptmax );

  latex.SetTextColor(kBlack);
  latex.DrawLatex( 0.15, 0.8, Rstring+", SubLeading Jets" );
  latex.SetTextColor( kAzure-6 );   
  latex.DrawLatex( 0.15,0.73, "STAR Simulation");

  leg = new TLegend( 0.32, 0.14, 0.87, 0.28 );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  
  leg->AddEntry(   PpMcRecoilPtProf, "p_{T}^{Cut}>2 GeV/c");
  leg->AddEntry(   PpMcRecoilPtProfLo, "p_{T}^{Cut}>0.2 GeV/c, Matched");

  leg->Draw();
  gPad->SaveAs( base+"SubLeadProfile.png");
  gPad->SaveAs( base+"All.pdf");

  gPad->SaveAs( base+"All.pdf]");

  fout->Write();
  return 0;
  
}
