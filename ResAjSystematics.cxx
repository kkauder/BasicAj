#include <vector>
using namespace std;

TH1D* minmax ( vector<TH1D*> profiles, TString namehelper, vector<TH1D*>& minandmax );

TH1D* QuadSum ( const TH1D* const h1, const TH1D* const h2, TString name );


int ResAjSystematics(TString R = "")
// int ResAjSystematics(TString R = "R0.2_")
{

  TString Rstring = "R=0.4";
  TString base="plots/SystResAj_R0.4_";

  if ( R=="R0.2_" ) {
    Rstring = "R=0.2";
    base="plots/SystResAj_R0.2_";
  }

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);

  TString BasePp = "ForResAj_HT54_Geant";
  TString BaseMc = "ForResAj_NoEff_TrueMB_NoCut_GeantMc";

  TString ft0e0 = "AjResults/Tow0_Eff0_"  + R + BasePp + "_MatchedTo_" + R + BaseMc + ".root";

  TString ftPe0 = "AjResults/Tow1_Eff0_"  + R + BasePp + "_MatchedTo_" + R + BaseMc + ".root";
  TString ftMe0 = "AjResults/Tow-1_Eff0_" + R + BasePp + "_MatchedTo_" + R + BaseMc + ".root";

  TString ft0eP = "AjResults/Tow0_Eff1_"  + R + BasePp + "_MatchedTo_" + R + BaseMc + ".root";
  TString ft0eM = "AjResults/Tow0_Eff-1_" + R + BasePp + "_MatchedTo_" + R + BaseMc + ".root";
  
  vector<TFile*> Tfiles;
  Tfiles.push_back(new TFile( ft0e0, "READ" ) );
  Tfiles.push_back(new TFile( ftPe0, "READ" ) );
  Tfiles.push_back(new TFile( ftMe0, "READ" ) );

  vector<TFile*> Efiles;
  Efiles.push_back(new TFile( ft0e0, "READ" ) );
  Efiles.push_back(new TFile( ft0eP, "READ" ) );
  Efiles.push_back(new TFile( ft0eM, "READ" ) );

  vector<TFile*> Allfiles;
  // 0 - nominal
  Allfiles.push_back(new TFile( ft0e0, "READ" ) );
  // 1&2 - Tower Scale
  Allfiles.push_back(new TFile( ftPe0, "READ" ) );
  Allfiles.push_back(new TFile( ftMe0, "READ" ) );

  // 3&4 - Tracking Efficiency
  Allfiles.push_back(new TFile( ft0eP, "READ" ) );
  Allfiles.push_back(new TFile( ft0eM, "READ" ) );

  TLegend* leg = 0;
  TLine l;
  int rebin = 4;
  float showptmin=8;
  // float showptmax=35;
  // float showptmax=45;
  float showptmax=55;

  float axptmin=5;
  float axptmax=60;
  TString s;

  // Leading Jet
  // -----------
  new TCanvas ( "Profile1","",500, 500);
  gPad->SetCanvasSize(450,450);

  leg = new TLegend( 0.15, 0.6, 0.7, 0.85, Rstring+", Leading Jets" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  
  TH2D* dummy = new TH2D( "dummy",";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c]", 100, axptmin,axptmax,100, axptmin,axptmax);
  dummy->Draw("AXIS");

  vector<TH1D*> T_Trig_histos;
  vector<TH1D*> E_Trig_histos;
  vector<TH1D*> T_TrigLo_histos;
  vector<TH1D*> E_TrigLo_histos;

  for ( int i=0; i<Allfiles.size(); ++i ){
    // for ( int i=1; i<2; ++i ){
    
    TFile* file = Allfiles.at(i);

    TH2D* PpMcTriggerPt = (TH2D*) file->Get("PpMcTriggerPt");
    s  = PpMcTriggerPt->GetName(); s+= "_"; s+=i;
    PpMcTriggerPt->SetName ( s );
    PpMcTriggerPt->Rebin2D(rebin,rebin);
    TH2D* PpMcTriggerPtLo = (TH2D*) file->Get("PpMcTriggerPtLo");
    s  = PpMcTriggerPtLo->GetName(); s+= "_"; s+=i;
    PpMcTriggerPtLo->SetName ( s );
    PpMcTriggerPtLo->Rebin2D(rebin,rebin);

    s  = "PpMcTriggerPtProf_"; s+=i;
    TProfile* PpMcTriggerPtProf = PpMcTriggerPt->ProfileX( s ,1,-1,"s");
    s  = "PpMcTriggerPtProfLo_"; s+=i;
    TProfile* PpMcTriggerPtProfLo = PpMcTriggerPtLo->ProfileX( s ,1,-1,"s");

    PpMcTriggerPtProf->GetYaxis()->SetTitle("p_{T}^{Part} [GeV/c]");
    PpMcTriggerPtProfLo->GetYaxis()->SetTitle("p_{T}^{Part} [GeV/c]");

    PpMcTriggerPtProf->SetAxisRange( showptmin, showptmax, "x");
    // PpMcTriggerPtProf->SetAxisRange( showptmin, showptmax, "y");
    
    PpMcTriggerPtProfLo->SetAxisRange( showptmin, showptmax, "x");
    // PpMcTriggerPtProfLo->SetAxisRange( showptmin, ptmax, "y");

    switch (i){
    case 0:
      PpMcTriggerPtProf->SetLineColor(kRed);
      PpMcTriggerPtProfLo->SetLineColor(kBlue);
      T_Trig_histos.push_back( (TH1D*) PpMcTriggerPtProf->ProjectionX( TString("h_T_")+PpMcTriggerPtProf->GetName() ) );
      T_TrigLo_histos.push_back( (TH1D*) PpMcTriggerPtProfLo->ProjectionX( TString("h_T_")+PpMcTriggerPtProfLo->GetName() ) );
      E_Trig_histos.push_back( (TH1D*) PpMcTriggerPtProf->ProjectionX( TString("h_E_")+PpMcTriggerPtProf->GetName() ) );
      E_TrigLo_histos.push_back( (TH1D*) PpMcTriggerPtProfLo->ProjectionX( TString("h_E_")+PpMcTriggerPtProfLo->GetName() ) );
      leg->AddEntry(   PpMcTriggerPtProf, "hard core");
      leg->AddEntry(   PpMcTriggerPtProfLo, "matched");
    break;
    case 1:
      PpMcTriggerPtProf->SetLineColor(kRed+1);
      PpMcTriggerPtProfLo->SetLineColor(kBlue+1);
      T_Trig_histos.push_back( (TH1D*) PpMcTriggerPtProf->ProjectionX( TString("h_T_")+PpMcTriggerPtProf->GetName() ) );
      T_TrigLo_histos.push_back( (TH1D*) PpMcTriggerPtProfLo->ProjectionX( TString("h_T_")+PpMcTriggerPtProfLo->GetName() ) );
      break;
    case 2:
      PpMcTriggerPtProf->SetLineColor(kRed+3);
      PpMcTriggerPtProfLo->SetLineColor(kBlue+3);
      T_Trig_histos.push_back( (TH1D*) PpMcTriggerPtProf->ProjectionX( TString("h_T_")+PpMcTriggerPtProf->GetName() ) );
      T_TrigLo_histos.push_back( (TH1D*) PpMcTriggerPtProfLo->ProjectionX( TString("h_T_")+PpMcTriggerPtProfLo->GetName() ) );
      break;
    case 3:
      PpMcTriggerPtProf->SetLineColor(kMagenta+1);
      PpMcTriggerPtProfLo->SetLineColor(kGreen+1);
      E_Trig_histos.push_back( (TH1D*) PpMcTriggerPtProf->ProjectionX( TString("h_E_")+PpMcTriggerPtProf->GetName() ) );
      E_TrigLo_histos.push_back( (TH1D*) PpMcTriggerPtProfLo->ProjectionX( TString("h_E_")+PpMcTriggerPtProfLo->GetName() ) );
      break;
    case 4:
      PpMcTriggerPtProf->SetLineColor(kMagenta+3);
      PpMcTriggerPtProfLo->SetLineColor(kGreen+3);
      E_Trig_histos.push_back( (TH1D*) PpMcTriggerPtProf->ProjectionX( TString("h_E_")+PpMcTriggerPtProf->GetName() ) );
      E_TrigLo_histos.push_back( (TH1D*) PpMcTriggerPtProfLo->ProjectionX( TString("h_E_")+PpMcTriggerPtProfLo->GetName() ) );
      break;
    default:
      cerr << "File counter i out of range, i=" << i << endl;
      return -1;
      break;
    }
    PpMcTriggerPtProf->Draw("same");
    PpMcTriggerPtProfLo->Draw("same");
    
    // PpMcTriggerPtProf->SaveAs( TString( PpMcTriggerPtProf->GetName() ) + ".root");
  }
  l.SetLineStyle(7);
  l.DrawLine( axptmin, axptmin, axptmax, axptmax );
  leg->Draw();
  gPad->SaveAs( base+"FullLeadProfile.png");
  
  // SubLeading Jet
  // -----------
  new TCanvas ( "Profile2","",500, 500);
  gPad->SetCanvasSize(450,450);
  dummy->Draw("AXIS");

  leg = new TLegend( 0.15, 0.6, 0.7, 0.85, Rstring+", SubLeading Jets" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  
  vector<TH1D*> T_Recoil_histos;
  vector<TH1D*> E_Recoil_histos;
  vector<TH1D*> T_RecoilLo_histos;
  vector<TH1D*> E_RecoilLo_histos;

  for ( int i=0; i<Allfiles.size(); ++i ){
    // for ( int i=1; i<2; ++i ){
    
    TFile* file = Allfiles.at(i);

    TH2D* PpMcRecoilPt = (TH2D*) file->Get("PpMcRecoilPt");
    s  = PpMcRecoilPt->GetName(); s+= "_"; s+=i;
    PpMcRecoilPt->SetName ( s );
    PpMcRecoilPt->Rebin2D(rebin,rebin);
    TH2D* PpMcRecoilPtLo = (TH2D*) file->Get("PpMcRecoilPtLo");
    s  = PpMcRecoilPtLo->GetName(); s+= "_"; s+=i;
    PpMcRecoilPtLo->SetName ( s );
    PpMcRecoilPtLo->Rebin2D(rebin,rebin);

    s  = "PpMcRecoilPtProf_"; s+=i;
    TProfile* PpMcRecoilPtProf = PpMcRecoilPt->ProfileX( s ,1,-1,"s");
    s  = "PpMcRecoilPtProfLo_"; s+=i;
    TProfile* PpMcRecoilPtProfLo = PpMcRecoilPtLo->ProfileX( s ,1,-1,"s");

    PpMcRecoilPtProf->GetYaxis()->SetTitle("p_{T}^{Part} [GeV/c]");
    PpMcRecoilPtProfLo->GetYaxis()->SetTitle("p_{T}^{Part} [GeV/c]");

    PpMcRecoilPtProf->SetAxisRange( showptmin, showptmax, "x");
    // PpMcRecoilPtProf->SetAxisRange( ptmin, ptmax, "y");
    
    PpMcRecoilPtProfLo->SetAxisRange( showptmin, showptmax, "x");
    // PpMcRecoilPtProfLo->SetAxisRange( ptmin, ptmax, "y");

    switch (i){
    case 0:
      PpMcRecoilPtProf->SetLineColor(kRed);
      PpMcRecoilPtProfLo->SetLineColor(kBlue);
      T_Recoil_histos.push_back( (TH1D*) PpMcRecoilPtProf->ProjectionX( TString("h_T_")+PpMcRecoilPtProf->GetName() ) );
      T_RecoilLo_histos.push_back( (TH1D*) PpMcRecoilPtProfLo->ProjectionX( TString("h_T_")+PpMcRecoilPtProfLo->GetName() ) );
      E_Recoil_histos.push_back( (TH1D*) PpMcRecoilPtProf->ProjectionX( TString("h_E_")+PpMcRecoilPtProf->GetName() ) );
      E_RecoilLo_histos.push_back( (TH1D*) PpMcRecoilPtProfLo->ProjectionX( TString("h_E_")+PpMcRecoilPtProfLo->GetName() ) );
      leg->AddEntry(   PpMcRecoilPtProf, "hard core");
      leg->AddEntry(   PpMcRecoilPtProfLo, "matched");
    break;
    case 1:
      PpMcRecoilPtProf->SetLineColor(kRed+1);
      PpMcRecoilPtProfLo->SetLineColor(kBlue+1);
      T_Recoil_histos.push_back( (TH1D*) PpMcRecoilPtProf->ProjectionX( TString("h_T_")+PpMcRecoilPtProf->GetName() ) );
      T_RecoilLo_histos.push_back( (TH1D*) PpMcRecoilPtProfLo->ProjectionX( TString("h_T_")+PpMcRecoilPtProfLo->GetName() ) );
      break;
    case 2:
      PpMcRecoilPtProf->SetLineColor(kRed+3);
      PpMcRecoilPtProfLo->SetLineColor(kBlue+3);
      T_Recoil_histos.push_back( (TH1D*) PpMcRecoilPtProf->ProjectionX( TString("h_T_")+PpMcRecoilPtProf->GetName() ) );
      T_RecoilLo_histos.push_back( (TH1D*) PpMcRecoilPtProfLo->ProjectionX( TString("h_T_")+PpMcRecoilPtProfLo->GetName() ) );
      break;
    case 3:
      PpMcRecoilPtProf->SetLineColor(kMagenta+1);
      PpMcRecoilPtProfLo->SetLineColor(kGreen+1);
      E_Recoil_histos.push_back( (TH1D*) PpMcRecoilPtProf->ProjectionX( TString("h_E_")+PpMcRecoilPtProf->GetName() ) );
      E_RecoilLo_histos.push_back( (TH1D*) PpMcRecoilPtProfLo->ProjectionX( TString("h_E_")+PpMcRecoilPtProfLo->GetName() ) );
      break;
    case 4:
      PpMcRecoilPtProf->SetLineColor(kMagenta+3);
      PpMcRecoilPtProfLo->SetLineColor(kGreen+3);
      E_Recoil_histos.push_back( (TH1D*) PpMcRecoilPtProf->ProjectionX( TString("h_E_")+PpMcRecoilPtProf->GetName() ) );
      E_RecoilLo_histos.push_back( (TH1D*) PpMcRecoilPtProfLo->ProjectionX( TString("h_E_")+PpMcRecoilPtProfLo->GetName() ) );
      break;
    default:
      cerr << "File counter i out of range, i=" << i << endl;
      return -1;
      break;
    }
    PpMcRecoilPtProf->Draw("same");
    PpMcRecoilPtProfLo->Draw("same");
    

  }
  l.SetLineStyle(7);
  l.DrawLine( axptmin, axptmin, axptmax, axptmax );
  leg->Draw();
  gPad->SaveAs( base+"FullSubLeadProfile.png");
  

  // ----------------------------
  // Now condense into error bars
  // ----------------------------

  // Leading Jet
  // -----------
  vector<TH1D*> E_Trig_minandmax;
  vector<TH1D*> T_Trig_minandmax;
  TH1D* E_Trig_minmax = minmax ( E_Trig_histos, "E_Trig", E_Trig_minandmax );
  TH1D* T_Trig_minmax = minmax ( T_Trig_histos, "T_Trig", T_Trig_minandmax );
  TH1D* Trig_minmax = QuadSum ( T_Trig_minmax, E_Trig_minmax, "Trig_minmax" );
  
  vector<TH1D*> E_TrigLo_minandmax;
  vector<TH1D*> T_TrigLo_minandmax;
  TH1D* E_TrigLo_minmax = minmax ( E_TrigLo_histos, "E_TrigLo", E_TrigLo_minandmax );
  TH1D* T_TrigLo_minmax = minmax ( T_TrigLo_histos, "T_TrigLo", T_TrigLo_minandmax );
  TH1D* TrigLo_minmax = QuadSum ( T_TrigLo_minmax, E_TrigLo_minmax, "TrigLo_minmax" );

  new TCanvas ( "SystProfile2","",500, 500);
  gPad->SetCanvasSize(450,450);
  dummy->Draw("AXIS");

  E_TrigLo_histos.at(0)->SetAxisRange( showptmin, showptmax, "x");
  E_Trig_histos.at(0)->SetAxisRange( showptmin, showptmax, "x");
  TrigLo_minmax->SetAxisRange( showptmin, showptmax, "x");
  Trig_minmax->SetAxisRange( showptmin, showptmax, "x");
    
  TrigLo_minmax->SetFillColor( kGray );
  TrigLo_minmax->SetMarkerColor( kGray );
  TrigLo_minmax->SetMarkerSize(0);
  TrigLo_minmax->Draw("9E2same");
  E_TrigLo_histos.at(0)->SetLineColor ( kBlack );  
  E_TrigLo_histos.at(0)->SetMarkerColor( kBlack );
  E_TrigLo_histos.at(0)->SetMarkerStyle( 29 );
  E_TrigLo_histos.at(0)->SetMarkerSize( 2 );
  E_TrigLo_histos.at(0)->Draw("9same");

  Trig_minmax->SetFillColor( kRed-10 );
  Trig_minmax->SetMarkerColor(kRed-10);
  Trig_minmax->SetMarkerSize(0);
  Trig_minmax->Draw("9E2same");
  E_Trig_histos.at(0)->SetLineColor ( kRed );
  E_Trig_histos.at(0)->SetMarkerColor( kRed );
  E_Trig_histos.at(0)->SetMarkerSize( 2 );
  E_Trig_histos.at(0)->SetMarkerStyle( 29 );
  E_Trig_histos.at(0)->Draw("9same");

  leg = new TLegend( 0.15, 0.6, 0.7, 0.85, Rstring+", Leading Jets" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  leg->SetTextSize(0.04);
  leg->AddEntry(   E_Trig_histos.at(0), "hard core");
  leg->AddEntry(   E_TrigLo_histos.at(0), "matched");

  l.SetLineStyle(7);
  l.DrawLine( axptmin, axptmin, axptmax, axptmax );
  leg->Draw();
  gPad->SaveAs( base+"LeadProfile.png");

  
  // SubLeading Jet
  // -----------
  vector<TH1D*> E_Recoil_minandmax;
  vector<TH1D*> T_Recoil_minandmax;
  TH1D* E_Recoil_minmax = minmax ( E_Recoil_histos, "E_Recoil", E_Recoil_minandmax );
  TH1D* T_Recoil_minmax = minmax ( T_Recoil_histos, "T_Recoil", T_Recoil_minandmax );
  TH1D* Recoil_minmax = QuadSum ( T_Recoil_minmax, E_Recoil_minmax, "Recoil_minmax" );
  
  vector<TH1D*> E_RecoilLo_minandmax;
  vector<TH1D*> T_RecoilLo_minandmax;
  TH1D* E_RecoilLo_minmax = minmax ( E_RecoilLo_histos, "E_RecoilLo", E_RecoilLo_minandmax );
  TH1D* T_RecoilLo_minmax = minmax ( T_RecoilLo_histos, "T_RecoilLo", T_RecoilLo_minandmax );
  TH1D* RecoilLo_minmax = QuadSum ( T_RecoilLo_minmax, E_RecoilLo_minmax, "RecoilLo_minmax" );

  new TCanvas ( "SystProfile1","",500, 500);
  gPad->SetCanvasSize(450,450);
  dummy->Draw("AXIS");

  E_RecoilLo_histos.at(0)->SetAxisRange( showptmin, showptmax, "x");
  E_Recoil_histos.at(0)->SetAxisRange( showptmin, showptmax, "x");
  RecoilLo_minmax->SetAxisRange( showptmin, showptmax, "x");
  Recoil_minmax->SetAxisRange( showptmin, showptmax, "x");

  RecoilLo_minmax->SetFillColor( kGray );
  RecoilLo_minmax->SetMarkerColor( kGray );
  RecoilLo_minmax->SetMarkerSize(0);
  RecoilLo_minmax->Draw("9E2same");
  E_RecoilLo_histos.at(0)->SetLineColor ( kBlack );  
  E_RecoilLo_histos.at(0)->SetMarkerColor( kBlack );
  E_RecoilLo_histos.at(0)->SetMarkerStyle( 29 );
  E_RecoilLo_histos.at(0)->SetMarkerSize( 2 );
  E_RecoilLo_histos.at(0)->Draw("9same");

  Recoil_minmax->SetFillColor( kRed-10 );
  Recoil_minmax->SetMarkerColor(kRed-10);
  Recoil_minmax->SetMarkerSize(0);
  Recoil_minmax->Draw("9E2same");
  E_Recoil_histos.at(0)->SetLineColor ( kRed );
  E_Recoil_histos.at(0)->SetMarkerColor( kRed );
  E_Recoil_histos.at(0)->SetMarkerSize( 2 );
  E_Recoil_histos.at(0)->SetMarkerStyle( 29 );
  E_Recoil_histos.at(0)->Draw("9same");

  leg = new TLegend( 0.15, 0.6, 0.7, 0.85, Rstring+", SubLeading" );
  leg->SetBorderSize(0);  leg->SetLineWidth(10);
  leg->SetFillStyle(0);   leg->SetMargin(0.1);
  leg->SetTextSize(0.04);
  leg->AddEntry(   E_Recoil_histos.at(0), "hard core");
  leg->AddEntry(   E_RecoilLo_histos.at(0), "matched");

  l.SetLineStyle(7);
  l.DrawLine( axptmin, axptmin, axptmax, axptmax );
  leg->Draw();
  gPad->SaveAs( base+"SubLeadProfile.png");

  
}
// -----------------------------------------------------------------------------
TH1D* minmax ( vector<TH1D*> profiles, TString namehelper, vector<TH1D*>& minandmax ){

  minandmax.clear();
  TH1D* h;
  
  TH1D* ret =(TH1D*) profiles.at(0)->Clone( namehelper+"_minmax" );
  ret->Reset();
  TH1D* hmin = (TH1D*) profiles.at(0)->Clone( namehelper+"_min" );
  hmin->Reset();
  TH1D* hmax = (TH1D*) profiles.at(0)->Clone( namehelper+"_max" );
  hmax->Reset();

  double min, max;
  for (int i=0; i<=ret->GetNbinsX() ; ++ i ){

    // // Option 1: Asymmetrical
    // min = 1e15;
    // max = -1e15;
    // for ( int j=0; j<profiles.size() ; ++j ){
    //   if ( profiles.at(j)->GetBinContent(i) < min ) min = profiles.at(j)->GetBinContent(i);
    //   if ( profiles.at(j)->GetBinContent(i) > max ) max = profiles.at(j)->GetBinContent(i);
    // }    
    // ret->SetBinContent( i, 0.5 * ( max+min ) );
    // ret->SetBinError( i, 0.5 * ( max-min ) );
    // cout << i << " " << max << "  " << min;
    // cout << " --> " << ret->GetBinContent( i ) << " +/- " <<     ret->GetBinError( i )<< endl;
    // hmin->SetBinContent( i, min );
    // hmin->SetBinError( i, profiles.at(0)->GetBinError(i) );   

    // hmax->SetBinContent( i, max );
    // hmax->SetBinError( i, profiles.at(0)->GetBinError(i) );   

    // Option 2: Symmetrical, maximal
    float e1 = fabs ( profiles.at(1)->GetBinContent(i) - profiles.at(0)->GetBinContent(i) );
    float e2 = fabs ( profiles.at(2)->GetBinContent(i) - profiles.at(0)->GetBinContent(i) );

    float v = profiles.at(0)->GetBinContent(i) ;
    float e = max ( e1, e2 );
    ret->SetBinContent( i, v );
    ret->SetBinError( i, e );
    	    
    hmin->SetBinContent( i, v-e );
    hmin->SetBinError( i, profiles.at(0)->GetBinError(i) );   

    hmax->SetBinContent( i, v+e );
    hmax->SetBinError( i, profiles.at(0)->GetBinError(i) );   
    
  }
  
  minandmax.push_back(hmin);
  minandmax.push_back(hmax);

  // // ratios. Assuming 0 is nominal
  // // -----------------------------
  // TH1D* nom = histos.at(0)->Clone( TString(histos.at(0)->GetName()) + "_nom");
  
  // TH1D* ratio1 = histos.at(1)->Clone( TString(histos.at(1)->GetName()) + "_ratio");
  // ratio1->GetYaxis()->SetTitle("ratio to nominal");
  // ratio1->Divide( nom);
  // ratios.push_back(ratio1);
  // TH1D* ratio2 = histos.at(2)->Clone( TString(histos.at(2)->GetName()) + "_ratio");
  // ratio2->GetYaxis()->SetTitle("ratio to nominal");
  // ratio2->Divide( nom);
  // ratios.push_back(ratio2);

  // if ( gPad ){      
  //   histos.at(0)->SetTitle( "");
  //   // histos.at(0)->SetAxisRange(  0.0, 0.23, "y");
  //   // histos.at(0)->SetAxisRange(  0.0, 0.72, "x");
  //   histos.at(0)->SetAxisRange(  0.0, 0.25, "y");
  //   histos.at(0)->SetAxisRange(  -0.3, 0.8, "x");
  //   histos.at(0)->Draw();

  //   histos.at(1)->SetLineWidth(2);
  //   histos.at(1)->SetLineColor(kBlue);
  //   histos.at(1)->Draw("same");

  //   histos.at(2)->SetLineWidth(2);
  //   histos.at(2)->SetLineColor(kMagenta+1);
  //   histos.at(2)->Draw("same");
    
  //   ret->SetFillStyle(3001 );
  //   ret->SetFillColor( kGray+1 );  
    
  //   ret->Draw("9E2same");

  //   //   histos.at(0)->Draw();
  //   // for ( int i=1; i<histos.size() ; ++i ){
  //   //   histos.at(i)->Draw("same");
  //   // }
  //   // ret->SetFillStyle(3001 );
  //   // ret->SetFillColor( kGray+1 );  
    
  //   // ret->Draw("9E2same");

  //   TLegend* leg = new TLegend ( 0.6, 0.6, 0.88, 0.88 );
  //   leg->SetBorderSize(0);
  //   leg->SetLineWidth(10);
  //   leg->SetFillStyle(0);
  //   // leg->SetMargin(0.1);

  //   leg->AddEntry(histos.at(0)->GetName(), "Nominal", "l");
  //   if ( namehelper.Contains("T") ){
  //     leg->AddEntry(histos.at(1)->GetName(), "Tow Scale Max", "l");
  //     leg->AddEntry(histos.at(2)->GetName(), "Tow Scale Min", "l");
  //   } else {
  //     leg->AddEntry(histos.at(1)->GetName(), "Track Eff Max", "l");
  //     leg->AddEntry(histos.at(2)->GetName(), "Track Eff Min", "l");
  //   }
  //   leg->AddEntry(ret->GetName(), "Uncertainty", "f");
  //   leg->Draw("same");
  //   // cout << histos.at(0)->GetName() << endl;
  // }

  
  return ret;

}
// -----------------------------------------------------------------------------
TH1D* QuadSum ( const TH1D* const h1, const TH1D* const h2, TString name ){
  TH1D* ret =(TH1D*) h1->Clone( name );

  for (int i=0; i<=ret->GetNbinsX() ; ++ i ){
    float v1 = h1->GetBinContent( i );
    float e1 = h1->GetBinError( i );
    float v2 = h2->GetBinContent( i );
    float e2 = h2->GetBinError( i );
    if ( fabs ( v1-v2 ) > 0.001*v1 ){
      throw string ("Incompatible histos!");
    }
    ret->SetBinError( i, TMath::Sqrt( e1*e1 + e2*e2 ) );    
  }

  return ret;
}
