{
  TFile *f1 = TFile::Open("AjResults/CrossCheckForResAj_HC0_NoEff_NoCut_TrueMB_HT54_HTled_Geant.root");
  TFile *f2 = TFile::Open("AjResults/CrossCheckForResAj_NoEff_NoCut_TrueMB_HT54_HTled_Geant.root");
  TFile *f3 = TFile::Open("AjResults/CrossCheckForResAj_NoEff_NoCut_TrueMB_HT54_Geant.root");
  TFile *f4 = TFile::Open("AjResults/ForResAj_NoEff_HT54_Geant.root");
  TFile *f5 = TFile::Open("AjResults/ForResAj_HT54_Geant.root");
  
  TFile *ff1 = TFile::Open("AjResults/CrossCheckForResAj_HC0_NoEff_NoCut_TrueMB_HT54_HTled_Geant_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");
  TFile *ff2 = TFile::Open("AjResults/CrossCheckForResAj_NoEff_NoCut_TrueMB_HT54_HTled_Geant_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");
  TFile *ff3 = TFile::Open("AjResults/CrossCheckForResAj_NoEff_NoCut_TrueMB_HT54_Geant_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");
  TFile *ff4 = TFile::Open("AjResults/ForResAj_NoEff_HT54_Geant_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");
  TFile *ff5 = TFile::Open("AjResults/ForResAj_HT54_Geant_MatchedTo_ForResAj_NoEff_TrueMB_NoCut_GeantMc.root");


  // canvas stuff
  float lm = 0.13;
  float bm = 0.13;
  float yto = 1.25;
  float xto = 1.25;
  
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);

  TLegend* leg = 0;
  TString base="plots/CrossCheckResAj_R0.4_";

  TH1D* h1 = new TH1D("h1","",80,0,80);  h1->Sumw2();
  TH1D* h2 = new TH1D("h2","",80,0,80);  h2->Sumw2();
  TH1D* h3 = new TH1D("h3","",80,0,80);  h3->Sumw2();
  TH1D* h4 = new TH1D("h4","",80,0,80);  h4->Sumw2();
  TH1D* h5 = new TH1D("h5","",80,0,80);  h5->Sumw2();

    
  new TCanvas;
  gPad->SaveAs( base+"All.pdf[");
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();  
  ((TTree*) f1->Get("TriggeredTree"))->Draw( "TriggerJet.Pt() >> h1","weight","goff" );
  ((TTree*) f2->Get("TriggeredTree"))->Draw( "TriggerJet.Pt() >> h2","weight","goff" );
  ((TTree*) f3->Get("TriggeredTree"))->Draw( "TriggerJet.Pt() >> h3","weight","goff" );

  ((TTree*) f4->Get("ResultTree"))->Draw( "j1.Pt() >> h4","weight","goff" );
  ((TTree*) f5->Get("ResultTree"))->Draw( "j1.Pt() >> h5","weight","goff" );

  h1->SetAxisRange(1e-5, 1e5, "y");
  h1->SetTitle(";p_{T}^{Det} [GeV/c]; counts");
  h1->SetLineColor(kGreen+1); h1->Draw();
  h2->SetLineColor(kTeal+1); h2->Draw("same");
  h3->SetLineColor(kBlue); h3->Draw("same");
  h4->SetLineColor(kBlack); h4->Draw("same");
  h5->SetLineColor(kRed); h5->Draw("same");

  // leg = new TLegend( 0.52, 0.5, 0.9, 0.9, "R=0.4, Leading Jet p_{T}" );
  leg = new TLegend( 0.17, 0.14, 0.57, 0.5, "R=0.4, Leading Jet p_{T}" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  leg->AddEntry( "h1", "Spin Paper: No HC, HT in jet", "l");
  leg->AddEntry( "h2", "With 100% HC, HT in jet", "l");
  leg->AddEntry( "h3", "With 100% HC, HT anywhere", "l");
  leg->AddEntry( "h4", "A_{J} paper, hard core, p+p effic.", "l");
  leg->AddEntry( "h5", "A_{J} paper, hard core, Au+Au effic.", "l");
  leg->Draw();
  gPad->SaveAs( base+"LeadPt.png");
  gPad->SaveAs( base+"All.pdf");

  // Figure 2
  // From https://drupal.star.bnl.gov/STAR/files/starpublications/103/data.html
  // https://arxiv.org/pdf/0710.2048v1.pdf  

  new TCanvas ( "Profile1","",500, 500);
  gPad->SetCanvasSize(450,450);


  int rebin = 4;
  float ptmin=0;
  float ptmax=35;
  float ptmax=65;
  TH2D* dummy = new TH2D ( "dummy","", 200, 0,100, 200, 0,100 );
  dummy->SetTitle(";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c];");
  dummy->SetAxisRange(ptmin, ptmax, "x");
  dummy->SetAxisRange(ptmin, ptmax, "y");
  dummy->Draw("Axis Y+");
  dummy->GetXaxis()->SetTitleOffset(xto);
  dummy->GetYaxis()->SetTitleOffset(yto);
    
  
  
  TH2D* hh1 = ff1->Get("PpMcTriggerPt");
  TH2D* hh2 = ff2->Get("PpMcTriggerPt");
  TH2D* hh3 = ff3->Get("PpMcTriggerPt");
  TH2D* hh4 = ff4->Get("PpMcTriggerPt");
  TH2D* hh5 = ff5->Get("PpMcTriggerPt");

  hh1->Rebin2D(rebin,rebin);
  hh2->Rebin2D(rebin,rebin);
  hh3->Rebin2D(rebin,rebin);
  hh4->Rebin2D(rebin,rebin);
  hh5->Rebin2D(rebin,rebin);

  TProfile* p1 = hh1->ProfileX("p1",1,-1,"s");
  TProfile* p2 = hh2->ProfileX("p2",1,-1,"s");
  TProfile* p3 = hh3->ProfileX("p3",1,-1,"s");
  TProfile* p4 = hh4->ProfileX("p4",1,-1,"s");
  TProfile* p5 = hh5->ProfileX("p5",1,-1,"s");
  
  p1->SetAxisRange( 5, 80, "x");
  p2->SetAxisRange( 5, 80, "x");
  p3->SetAxisRange( 5, 80, "x");
  p4->SetAxisRange( 5, 80, "x");
  p5->SetAxisRange( 5, 80, "x");

  p1->SetLineColor( h1->GetLineColor() );  p1->Draw("same");
  p2->SetLineColor( h2->GetLineColor() );  p2->Draw("same");
  p3->SetLineColor( h3->GetLineColor() );  p3->Draw("same");
  p4->SetLineColor( h4->GetLineColor() );  p4->Draw("same");
  p5->SetLineColor( h5->GetLineColor() );  p5->Draw("same");

  leg = new TLegend( 0.12, 0.5, 0.57, 0.9, "R=0.4, Leading Jet p_{T}" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  leg->AddEntry( "p1", "Spin Paper: No HC, HT in jet", "l");
  leg->AddEntry( "p2", "With 100% HC, HT in jet", "l");
  leg->AddEntry( "p3", "With 100% HC, HT anywhere", "l");
  leg->AddEntry( "p4", "A_{J} paper, hard core, p+p effic.", "l");
  leg->AddEntry( "p5", "A_{J} paper, hard core, Au+Au effic.", "l");
  
  int nbins = 10;    
  const float Det[10]  = { 5.575,   6.85725, 8.43442, 10.3743, 12.7604, 15.6953, 19.3053, 23.7454, 29.2069, 35.9245};
  const float Part[10] = { 5.44646, 6.14727, 7.0034,  8.60749, 10.3545, 13.0381, 15.8978, 19.434,  23.5765, 28.2171};
  const float Perr[10] = { 1.8695,  2.11456, 2.24134, 2.50937, 3.05165, 3.04097, 3.29543, 3.64129,  4.3053, 4.6465};
  
  TGraphErrors* g = new TGraphErrors( nbins, (const float*) Det, (const float*) Part, (const float*) 0, (const float*) Perr);
  g->Draw("*");

  TLine l;
  l.SetLineStyle(7);
  l.DrawLine( ptmin, ptmin, ptmax, ptmax );

  leg->AddEntry( g, "Original Spin Paper", "l");

  leg->Draw();
  gPad->SaveAs( base+"PVD.png");
  gPad->SaveAs( base+"All.pdf");
  

  gPad->SaveAs( base+"All.pdf]");
  
}
