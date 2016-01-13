#include <vector>

#define REBIN 2

int PlotPt( TString R="0.4", int AuAuMultL=269, int AuAuMultR=-1  )
{

  // TString outname = "AjResults/Systematics_Fresh_NicksList_HC100_ppInAuAuAj.root";
  TString refname = "AjResults/Fresh_NicksList_HC100_AuAu.root";
  
  if ( R=="0.2"  ){
    // outname.ReplaceAll("Systematics", "Systematics_R0.2");
  } else if ( R=="0.4"  ){
    // do nothing
  } else if ( R=="Pt1"  ){
    // outname.ReplaceAll("Systematics", "Pt1_Systematics");
  } else {
    cerr << "R = " << R << " not supported." << endl;
    return -1;
  }
      
  TFile* file = new TFile ( refname,"READ");

  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);    

  TString plotname;

  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);

  TH1D* hPtLeadHi = new TH1D( "hPtLeadHi","p_{T}^{C} > 2 GeV/c;p_{T}^{Jet} [GeV/c]", 50, 0, 50 );
  TH1D* hPtSubLeadHi = new TH1D( "hPtSubLeadHi","p_{T}^{C} > 2 GeV/c;p_{T}^{Jet} [GeV/c]", 50, 0, 50 );
  TH1D* hPtLeadLo = new TH1D( "hPtLeadLo","p_{T}^{C} > 2 GeV/c;p_{T}^{Jet} [GeV/c]", 50, 0, 50 );
  TH1D* hPtSubLeadLo = new TH1D( "hPtSubLeadLo","p_{T}^{C} > 2 GeV/c;p_{T}^{Jet} [GeV/c]", 50, 0, 50 );

  // Attach branches
  // ---------------
  TTree* ResultTree = (TTree*) file->Get("ResultTree");
  TLorentzVector *pJ1 = new TLorentzVector();
  TLorentzVector *pJ2 = new TLorentzVector();
  TLorentzVector *pJM1 = new TLorentzVector();
  TLorentzVector *pJM2 = new TLorentzVector();
  double refmult;

  ResultTree->SetBranchAddress("j1", &pJ1);
  ResultTree->SetBranchAddress("j2", &pJ2);
  ResultTree->SetBranchAddress("jm1", &pJM1);
  ResultTree->SetBranchAddress("jm2", &pJM2);
  ResultTree->SetBranchAddress("refmult", &refmult);

  if ( AuAuMultL < 0 ) AuAuMultL = 0;
  if ( AuAuMultR < 0 ) AuAuMultR = INT_MAX;

  for ( int i=0; i<ResultTree->GetEntries(); ++i ){
    ResultTree->GetEntry ( i );
    if ( refmult<AuAuMultL || refmult > AuAuMultR ) continue;

    hPtLeadHi->Fill ( pJ1->Pt() );
    hPtSubLeadHi->Fill ( pJ2->Pt() );

    hPtLeadLo->Fill ( pJM1->Pt() );
    hPtSubLeadLo->Fill ( pJM2->Pt() );
  }

  hPtLeadHi->SetLineColor( kRed );
  hPtSubLeadHi->SetLineColor( kRed );

  gPad->SetLogy();
  hPtLeadHi->SetLineColor( kRed );
  hPtLeadHi->SetMarkerColor( kRed );
  hPtLeadHi->SetMarkerStyle( 20 );

  hPtSubLeadHi->SetLineColor( kRed );
  hPtSubLeadHi->SetMarkerColor( kRed );
  hPtSubLeadHi->SetMarkerStyle( 21 );

  hPtLeadLo->SetLineColor( kBlack );
  hPtLeadLo->SetMarkerColor( kBlack );
  hPtLeadLo->SetMarkerStyle( 20 );

  hPtSubLeadLo->SetLineColor( kBlack );
  hPtSubLeadLo->SetMarkerColor( kBlack );
  hPtSubLeadLo->SetMarkerStyle( 21 );

  hPtLeadHi->SetAxisRange( 1e-1, 5E3, "y");

  hPtLeadHi->SetTitle("");
  hPtLeadHi->Draw();
  hPtSubLeadHi-> Draw("same");
  hPtLeadLo-> Draw("same");
  hPtSubLeadLo-> Draw("same");

  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( hPtLeadHi,     "Leading Jet, AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( hPtSubLeadHi,     "SubLeading Jet, AuAu HT, p_{T}^{Cut}>2 GeV/c", "p");
  leg->AddEntry ( hPtLeadLo,     "Leading Jet, AuAu HT, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->AddEntry ( hPtSubLeadLo,     "SubLeading Jet, AuAu HT, p_{T}^{Cut}>0.2 GeV/c", "p");
  leg->Draw();

  gPad->SaveAs("Plots/AuAuPtSpectra.png");

  // cout << "Wrote to " << endl << out->GetName() << endl;
  return 0;
  
}


// ===========================================================================
    
    
