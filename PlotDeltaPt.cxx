// Int_t PlotDeltaPt(TString filename="AjResults/pythiaFollowAj.root")
// Int_t PlotDeltaPt(TString filename="AjResults/FollowAuAuAj.root")
// Int_t PlotDeltaPt(TString filename="AjResults/VarFollowAuAuAj.root")
Int_t PlotDeltaPt(TString filename="AjResults/ppFollowAj.root")
// Int_t PlotDeltaPt(TString filename="AjResults/ppInAuAuFollowAj.root")
{
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  
  TLegend* leg = new TLegend( 0.12, 0.65, 0.6, 0.9, "p_{T} (R=0.2) - p_{T} (R=0.4, matched)" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  TFile * file = new TFile(filename,"read");
  LeadDeltaPtHi->SetLineColor(kRed);
  LeadDeltaPtLo->SetLineColor(kBlack);
  SubLeadDeltaPtHi->SetLineColor(kMagenta);
  SubLeadDeltaPtLo->SetLineColor(kGreen+1);

  float MeanLeadDeltaPtHi    = LeadDeltaPtHi->GetMean();
  float MeanSubLeadDeltaPtHi = SubLeadDeltaPtHi->GetMean();

  float MeanLeadDeltaPtLo    = LeadDeltaPtLo->GetMean();
  float MeanSubLeadDeltaPtLo = SubLeadDeltaPtLo->GetMean();


  LeadDeltaPtHi->SetAxisRange(-20, 4);
  LeadDeltaPtHi->SetLineWidth(2);
  SubLeadDeltaPtHi->SetLineWidth(2);
  
  TString title = "";
  if ( filename.Contains("FollowAuAuAj") ) title = "Au+Au HT";
  if ( filename.Contains("pythiaFollowAj") ) title = "Pythia";
  if ( filename.Contains("ppFollowAj") ) title = "pp HT";
  if ( filename.Contains("ppInAuAuFollowAj") ) title = "pp HT #otimes Au+Au MB";
  LeadDeltaPtHi->SetTitle(title);

  LeadDeltaPtHi->Draw("9");
  SubLeadDeltaPtHi->Draw("9same");
  // LeadDeltaPtLo->Draw("9same");
  // SubLeadDeltaPtLo->Draw("9same");

  char legentry[500];
  sprintf (legentry, "Leading jet, p_{T}^{Cut}>2 GeV/c, <#Delta p_{T}>=%.2f", MeanLeadDeltaPtHi);
  leg->AddEntry(LeadDeltaPtHi,legentry);
  sprintf (legentry, "Sub-Leading jet, p_{T}^{Cut}>2 GeV/c, <#Delta p_{T}>=%.2f", MeanSubLeadDeltaPtHi);
  leg->AddEntry(SubLeadDeltaPtHi,legentry);
  // sprintf (legentry, "Leading jet, p_{T}^{Cut}>0.2 GeV/c, <#Delta p_{T}>=%.2f", MeanLeadDeltaPtLo);
  // leg->AddEntry(LeadDeltaPtLo,legentry);
  // sprintf (legentry, "Sub-Leading jet, p_{T}^{Cut}>0.2 GeV/c, <#Delta p_{T}>=%.2f", MeanSubLeadDeltaPtLo);
  // leg->AddEntry(SubLeadDeltaPtLo,legentry);
  leg->Draw();
  
  gPad->Update();
  
  
  
  TLine l;
  l.SetLineColor ( LeadDeltaPtHi->GetLineColor() );  l.SetLineWidth ( LeadDeltaPtHi->GetLineWidth() );
  l.DrawLine ( MeanLeadDeltaPtHi, pow(10,gPad->GetUymin()), MeanLeadDeltaPtHi, pow(10,gPad->GetUymax()));
  
  l.SetLineColor ( SubLeadDeltaPtHi->GetLineColor() );  l.SetLineWidth ( SubLeadDeltaPtHi->GetLineWidth() );
  l.DrawLine ( MeanSubLeadDeltaPtHi, pow(10,gPad->GetUymin()), MeanSubLeadDeltaPtHi, pow(10,gPad->GetUymax()));
  
  // l.SetLineColor ( LeadDeltaPtLo->GetLineColor() );  l.SetLineWidth ( LeadDeltaPtLo->GetLineWidth() );
  // l.DrawLine ( MeanLeadDeltaPtLo, pow(10,gPad->GetUymin()), MeanLeadDeltaPtLo, pow(10,gPad->GetUymax()));
  
  // l.SetLineColor ( SubLeadDeltaPtLo->GetLineColor() );  l.SetLineWidth ( SubLeadDeltaPtLo->GetLineWidth() );
  // l.DrawLine ( MeanSubLeadDeltaPtLo, pow(10,gPad->GetUymin()), MeanSubLeadDeltaPtLo, pow(10,gPad->GetUymax()));


  TString outname=gSystem->BaseName(filename);
  outname.Prepend("plots/");
  outname.ReplaceAll(".root",".DeltaPt.png");
  gPad->SaveAs( outname );
  
}
