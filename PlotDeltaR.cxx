#include <vector>

#define REBIN 2

int PlotDeltaR( )
{

  TString inname = "AjResults/Groom_Aj_TrueMB_NoEff_Geant.root";
  // TString inname = "AjResults/Groom_AuAu_HT54_HTled.root";

  
  TFile* file = new TFile ( inname,"READ");

  TCanvas* c = new TCanvas("c");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);    

  TString plotname;

  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);

  TH1D* hDRLeadLo = new TH1D( "hDRLeadLo",";#{Delta}R", 50, 0, 0.5 );
  TH1D* hDRSubLeadLo = new TH1D( "hDRSubLeadLo",";#{Delta}R", 50, 0, 0.5 );

  // Attach branches
  // ---------------
  TTree* TriggeredTree = (TTree*) file->Get("TriggeredTree");

  TClonesArray *aTriggerJetLo = new TClonesArray("TLorentzVector");
  TClonesArray *aAwayJetLo = new TClonesArray ("TLorentzVector");
  TriggeredTree->SetBranchAddress("TriggerJetLo", &aTriggerJetLo);
  TriggeredTree->SetBranchAddress("AwayJetLo", &aAwayJetLo);
 
  TLorentzVector *Tlo_s1 = new TLorentzVector();
  TLorentzVector *Tlo_s2 = new TLorentzVector();
  TLorentzVector *Alo_s1 = new TLorentzVector();
  TLorentzVector *Alo_s2 = new TLorentzVector();

  TriggeredTree->SetBranchAddress("Tlo_s1", &Tlo_s1);
  TriggeredTree->SetBranchAddress("Tlo_s2", &Tlo_s2);
  TriggeredTree->SetBranchAddress("Alo_s1", &Alo_s1);
  TriggeredTree->SetBranchAddress("Alo_s2", &Alo_s2);

  float zgtriglo;
  TriggeredTree->SetBranchAddress("zgtriglo",&zgtriglo);
  float zgawaylo;
  TriggeredTree->SetBranchAddress("zgawaylo",&zgawaylo);


  for ( int i=0; i<TriggeredTree->GetEntries(); ++i ){
    TriggeredTree->GetEntry ( i );

    TLorentzVector *TriggerJetLo = (TLorentzVector*) aTriggerJetLo->At(0);    
    if ( zgtriglo<0.05 ) continue;
    if ( Tlo_s1->Pt()<1e-2 || Tlo_s2->Pt()<1e-2 ) continue;
    if ( TriggerJetLo->Pt()<20 ) continue;

    hDRLeadLo->Fill ( Tlo_s1->DeltaR( *Tlo_s2) );

    TLorentzVector *AwayJetLo = (TLorentzVector*) aAwayJetLo->At(0);    
    if ( zgawaylo<0.05 ) continue;
    if ( Alo_s1->Pt()<1e-2 || Alo_s2->Pt()<1e-2 ) continue;
    if ( AwayJetLo->Pt()<10 ) continue;

    hDRSubLeadLo->Fill ( Alo_s1->DeltaR( *Alo_s2) );


  }

  hDRLeadLo->SetLineColor( kRed );
  hDRLeadLo->SetMarkerColor( kRed );
  hDRLeadLo->SetMarkerStyle( 20 );

  hDRSubLeadLo->SetLineColor( kBlack );
  hDRSubLeadLo->SetMarkerColor( kBlack );
  hDRSubLeadLo->SetMarkerStyle( 21 );
  
  hDRLeadLo->SetTitle("");
  hDRLeadLo->Draw();
  hDRSubLeadLo-> Draw("same");

  TLegend* leg = new TLegend( 0.48, 0.7, 0.89, 0.9, "Pythia @ GEANT, untriggered" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry ( hDRLeadLo,     "Leading Jet, p_{T}^{Jet}>20 GeV/c", "lp");
  leg->AddEntry ( hDRSubLeadLo,  "Sub-Leading Jet, p_{T}^{Jet}>10 GeV/c", "lp");
  leg->Draw();

  // Line to indicate Marta's cut
  TLine line;
  line.SetLineStyle( 2 );
  line.SetLineColor( kGray+2 );
  gPad->Update();
  line.DrawLine( 0.1, gPad->GetFrame()->GetY1(), 0.1, gPad->GetFrame()->GetY2() );

  gPad->SaveAs("HPplots/GeantDeltaR.png");

  int targetbin = hDRLeadLo->GetXaxis()->FindBin(0.1+0.001);
  cout << " Leading jet: " << 100. * hDRLeadLo->Integral ( 1, targetbin-1 ) / hDRLeadLo->Integral ( 1, hDRLeadLo->GetNbinsX() )  << "% below 0.1" << endl;
  cout << " SubLeading jet: " << 100. * hDRSubLeadLo->Integral ( 1, targetbin-1 ) / hDRSubLeadLo->Integral ( 1, hDRSubLeadLo->GetNbinsX() )  << "% below 0.1" << endl;
      

  // cout << "Wrote to " << endl << out->GetName() << endl;
  return 0;
  
}


// ===========================================================================
    
    
