#include <vector>

#define REBIN 2

void CalcAj ( TFile* file, int AuAuMultL, int AuAuMultR, vector<double>& UnbinnedAj_lo, vector<double>& UnbinnedAj_hi, TString opt="fabs" );
double CalcAj ( TLorentzVector* j1, TLorentzVector* j2, bool nofabs=false );
TH1D* minmax ( vector<TFile*> files, TString which, int AuAuMultL, int AuAuMultR, TString namehelper,  vector<TH1D*>& histos, 
	       vector<TH1D*>& ratios, vector<TH1D*>& minandmax );


int PrepSystematics( TString R="0.4", int AuAuMultL=269, int AuAuMultR=-1  )
//int PrepSystematics( TString R="Pt1", int AuAuMultL=351, int AuAuMultR=-1  )
{

  // TString outname = "AjResults/Systematics_Fresh_NicksList_HC100_ppInAuAuAj.root";
  // TString refname = "AjResults/Fresh_NicksList_HC100_AuAu.root";
  
  // TString ft0e0 = "AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root";
  
  // TString ftPe0 = "AjResults/Tow1_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root";
  // TString ftMe0 = "AjResults/Tow-1_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root";

  // TString ft0eP = "AjResults/Tow0_Eff1_Fresh_NicksList_HC100_ppInAuAuAj.root";
  // TString ft0eM = "AjResults/Tow0_Eff-1_Fresh_NicksList_HC100_ppInAuAuAj.root";

  TString refname = "AjResults/Groom_AuAu_HT54_HTled.root";

  TString Base = "Groom_Aj_HT54_HTled_ppInAuAuAj";
  TString outname = "AjResults/Systematics_" + Base + ".root";
  
  TString ft0e0 = "AjResults/Tow0_Eff0_" + Base + ".root";
  TString ftPe0 = "AjResults/Tow1_Eff0_" + Base + ".root";
  TString ftMe0 = "AjResults/Tow-1_Eff0_" + Base + ".root";

  TString ft0eP = "AjResults/Tow0_Eff1_" + Base + ".root";
  TString ft0eM = "AjResults/Tow0_Eff-1_" + Base + ".root";


  if ( R=="0.2"  ){
    // ft0e0.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // ftPe0.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // ftMe0.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // ft0eP.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // ft0eM.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // outname.ReplaceAll("Systematics", "R0.2_Systematics");
    // ft0e0.ReplaceAll("HC30", "R0.2_HC30");
    // ftPe0.ReplaceAll("HC30", "R0.2_HC30");
    // ftMe0.ReplaceAll("HC30", "R0.2_HC30");
    // ft0eP.ReplaceAll("HC30", "R0.2_HC30");
    // ft0eM.ReplaceAll("HC30", "R0.2_HC30");

    ft0e0.ReplaceAll("Eff0", "Eff0_R0.2");
    ftPe0.ReplaceAll("Eff0", "Eff0_R0.2");
    ftMe0.ReplaceAll("Eff0", "Eff0_R0.2");
    ft0eP.ReplaceAll("Eff1", "Eff1_R0.2");
    ft0eM.ReplaceAll("Eff-1", "Eff-1_R0.2");
    outname.ReplaceAll("Systematics", "Systematics_R0.2");

  } else if ( R=="0.4"  ){
    // do nothing
  } else if ( R=="Pt1"  ){
    ft0e0.ReplaceAll("ppInAuAuAj", "Pt1_ppInAuAuAj");
    ftPe0.ReplaceAll("ppInAuAuAj", "Pt1_ppInAuAuAj");
    ftMe0.ReplaceAll("ppInAuAuAj", "Pt1_ppInAuAuAj");
    ft0eP.ReplaceAll("ppInAuAuAj", "Pt1_ppInAuAuAj");
    ft0eM.ReplaceAll("ppInAuAuAj", "Pt1_ppInAuAuAj");
    outname.ReplaceAll("Systematics", "Pt1_Systematics");
  } else {
    cerr << "R = " << R << " not supported." << endl;
    return -1;
  }
      
  // // DEBUG
  // ft0e0.ReplaceAll("HC30", "MixTest");
  // ftPe0.ReplaceAll("HC30", "MixTest");
  // ftMe0.ReplaceAll("HC30", "MixTest");
  // ft0eP.ReplaceAll("HC30", "MixTest");
  // ft0eM.ReplaceAll("HC30", "MixTest");
  // outname.ReplaceAll("HC30", "MixTest");

  if ( AuAuMultL==351  ){
    ft0e0.ReplaceAll("ppInAuAuAj", "HP_ppInAuAuAj");
    ftPe0.ReplaceAll("ppInAuAuAj", "HP_ppInAuAuAj");
    ftMe0.ReplaceAll("ppInAuAuAj", "HP_ppInAuAuAj");
    ft0eP.ReplaceAll("ppInAuAuAj", "HP_ppInAuAuAj");
    ft0eM.ReplaceAll("ppInAuAuAj", "HP_ppInAuAuAj");
    outname.ReplaceAll("Systematics", "HP_Systematics");
  } 
  
  vector<TFile*> Tfiles;
  Tfiles.push_back(new TFile( ft0e0, "READ" ) );
  Tfiles.push_back(new TFile( ftPe0, "READ" ) );
  Tfiles.push_back(new TFile( ftMe0, "READ" ) );

  vector<TFile*> Efiles;
  Efiles.push_back(new TFile( ft0e0, "READ" ) );
  Efiles.push_back(new TFile( ft0eP, "READ" ) );
  Efiles.push_back(new TFile( ft0eM, "READ" ) );

  vector<TH1D*> Thistos;
  vector<TH1D*> Ehistos;
  vector<TH1D*> ratios;
  vector<TH1D*> minandmax;

  // for p-value stuff
  vector<TH1D*> ThistosHi;
  vector<TH1D*> EhistosHi;
  vector<TH1D*> ThistosLo;
  vector<TH1D*> EhistosLo;
  vector<TH1D*> NoFabsThistosHi;
  vector<TH1D*> NoFabsEhistosHi;
  vector<TH1D*> NoFabsThistosLo;
  vector<TH1D*> NoFabsEhistosLo;

  bool visualize=true;
  if (visualize){
    TCanvas* c = new TCanvas("c");
    gPad->SetGridx(0);  gPad->SetGridy(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptDate(0);    
  }
  TString plotname;

  // Unmatched
  // ---------
  TH1D* UnmatchedAJ_hi_T = minmax (  Tfiles, "UnmatchedAJ_hi", AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
  UnmatchedAJ_hi_T->SetName("UnmatchedAJ_hi_T");
  TH1D* UnmatchedAJ_hi_E = minmax (  Efiles, "UnmatchedAJ_hi", AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
  UnmatchedAJ_hi_E->SetName("UnmatchedAJ_hi_E");
  TH1D* UnmatchedAJ_hi_minmax = UnmatchedAJ_hi_T->Clone("UnmatchedAJ_hi_minmax");

  for (int i=1; i<=UnmatchedAJ_hi_minmax->GetNbinsX() ; ++ i ){
    UnmatchedAJ_hi_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );
    UnmatchedAJ_hi_minmax->SetBinError (i, sqrt( pow( UnmatchedAJ_hi_T->GetBinError(i), 2) +
						 pow( UnmatchedAJ_hi_E->GetBinError(i), 2) ) );
  }
    
  // Matched, high
  // -------------
  TH1D* AJ_hi_T = minmax (  Tfiles, "AJ_hi", AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
  AJ_hi_T->SetName("AJ_hi_T");
  if (gPad){
    plotname = "plots/" + R + "_" + AJ_hi_T->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_hi_E = minmax (  Efiles, "AJ_hi", AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
  AJ_hi_E->SetName("AJ_hi_E");
  if (gPad){
    plotname = "plots/" + R + "_" + AJ_hi_E->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_hi_minmax = AJ_hi_T->Clone("AJ_hi_minmax");
  TH1D* AJ_hi_min = AJ_hi_T->Clone("AJ_hi_min");
  TH1D* AJ_hi_max = AJ_hi_T->Clone("AJ_hi_max");
  for (int i=1; i<=AJ_hi_minmax->GetNbinsX() ; ++ i ){
    AJ_hi_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );

    AJ_hi_minmax->SetBinError (i, sqrt( pow( AJ_hi_T->GetBinError(i), 2) +
						 pow( AJ_hi_E->GetBinError(i), 2) ) );

    AJ_hi_min->SetBinContent (i, Thistos.at(0)->GetBinContent(i) - AJ_hi_minmax->GetBinError (i) );
    AJ_hi_min->SetBinError   (i, Thistos.at(0)->GetBinError(i) );

    AJ_hi_max->SetBinContent (i, Thistos.at(0)->GetBinContent(i) + AJ_hi_minmax->GetBinError (i) );
    AJ_hi_max->SetBinError   (i, Thistos.at(0)->GetBinError(i) );

  }
  ThistosHi=Thistos;
  EhistosHi=Ehistos;

  // Matched, hi, allowing negative NoFabsAJ
  // ----------------------------------------
  TH1D* NoFabsAJ_hi_T = minmax (  Tfiles, "NoFabsAJ_hi", AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
  NoFabsAJ_hi_T->SetName("NoFabsAJ_hi_T");
  if (gPad){
    plotname = "plots/" + R + "_" + NoFabsAJ_hi_T->GetName() + ".png";
    gPad->SaveAs( plotname );
  }
  
  TH1D* NoFabsAJ_hi_E = minmax (  Efiles, "NoFabsAJ_hi", AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
  NoFabsAJ_hi_E->SetName("NoFabsAJ_hi_E");
  if (gPad){
    plotname = "plots/" + R + "_" + NoFabsAJ_hi_E->GetName() + ".png";
    gPad->SaveAs( plotname );
  }  
  TH1D* NoFabsAJ_hi_minmax = NoFabsAJ_hi_T->Clone("NoFabsAJ_hi_minmax");
  TH1D* NoFabsAJ_hi_min = NoFabsAJ_hi_T->Clone("NoFabsAJ_hi_min");
  TH1D* NoFabsAJ_hi_max = NoFabsAJ_hi_T->Clone("NoFabsAJ_hi_max");
  for (int i=1; i<=NoFabsAJ_hi_minmax->GetNbinsX() ; ++ i ){
    NoFabsAJ_hi_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );
    NoFabsAJ_hi_minmax->SetBinError (i, sqrt( pow( NoFabsAJ_hi_T->GetBinError(i), 2) +
					      pow( NoFabsAJ_hi_E->GetBinError(i), 2) ) );

    NoFabsAJ_hi_min->SetBinContent (i, Thistos.at(0)->GetBinContent(i) - NoFabsAJ_hi_minmax->GetBinError (i) );
    NoFabsAJ_hi_min->SetBinError   (i, Thistos.at(0)->GetBinError(i) );

    NoFabsAJ_hi_max->SetBinContent (i, Thistos.at(0)->GetBinContent(i) + NoFabsAJ_hi_minmax->GetBinError (i) );
    NoFabsAJ_hi_max->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
  }

  NoFabsThistosHi=Thistos;
  NoFabsEhistosHi=Ehistos;

  // Matched, low
  // ------------
  TH1D* AJ_lo_T = minmax (  Tfiles, "AJ_lo", AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
  AJ_lo_T->SetName("AJ_lo_T");
  if (gPad){
    plotname = "plots/" + R + "_" + AJ_lo_T->GetName() + ".png";
    gPad->SaveAs( plotname );
  }
  
  TH1D* AJ_lo_E = minmax (  Efiles, "AJ_lo", AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
  AJ_lo_E->SetName("AJ_lo_E");
  if (gPad){
    plotname = "plots/" + R + "_" + AJ_lo_E->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_lo_minmax = AJ_lo_T->Clone("AJ_lo_minmax");
  TH1D* AJ_lo_min = AJ_lo_T->Clone("AJ_lo_min");
  TH1D* AJ_lo_max = AJ_lo_T->Clone("AJ_lo_max");
  for (int i=1; i<=AJ_lo_minmax->GetNbinsX() ; ++ i ){
    AJ_lo_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );

    AJ_lo_minmax->SetBinError (i, sqrt( pow( AJ_lo_T->GetBinError(i), 2) +
						 pow( AJ_lo_E->GetBinError(i), 2) ) );

    AJ_lo_min->SetBinContent (i, Thistos.at(0)->GetBinContent(i) - AJ_lo_minmax->GetBinError (i) );
    AJ_lo_min->SetBinError   (i, Thistos.at(0)->GetBinError(i) );

    AJ_lo_max->SetBinContent (i, Thistos.at(0)->GetBinContent(i) + AJ_lo_minmax->GetBinError (i) );
    AJ_lo_max->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
  }
  
  ThistosLo=Thistos;
  EhistosLo=Ehistos;

  
  // Matched, low, allowing negative NoFabsAJ
  // ----------------------------------------
  TH1D* NoFabsAJ_lo_T = minmax (  Tfiles, "NoFabsAJ_lo", AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
  NoFabsAJ_lo_T->SetName("NoFabsAJ_lo_T");
  if (gPad){
    plotname = "plots/" + R + "_" + NoFabsAJ_lo_T->GetName() + ".png";
    gPad->SaveAs( plotname );
  }
  
  TH1D* NoFabsAJ_lo_E = minmax (  Efiles, "NoFabsAJ_lo", AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
  NoFabsAJ_lo_E->SetName("NoFabsAJ_lo_E");
  if (gPad){
    plotname = "plots/" + R + "_" + NoFabsAJ_lo_E->GetName() + ".png";
    gPad->SaveAs( plotname );
  }  
  TH1D* NoFabsAJ_lo_minmax = NoFabsAJ_lo_T->Clone("NoFabsAJ_lo_minmax");
  TH1D* NoFabsAJ_lo_min = NoFabsAJ_lo_T->Clone("NoFabsAJ_lo_min");
  TH1D* NoFabsAJ_lo_max = NoFabsAJ_lo_T->Clone("NoFabsAJ_lo_max");
  for (int i=1; i<=NoFabsAJ_lo_minmax->GetNbinsX() ; ++ i ){
    NoFabsAJ_lo_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );
    NoFabsAJ_lo_minmax->SetBinError (i, sqrt( pow( NoFabsAJ_lo_T->GetBinError(i), 2) +
					      pow( NoFabsAJ_lo_E->GetBinError(i), 2) ) );

    NoFabsAJ_lo_min->SetBinContent (i, Thistos.at(0)->GetBinContent(i) - NoFabsAJ_lo_minmax->GetBinError (i) );
    NoFabsAJ_lo_min->SetBinError   (i, Thistos.at(0)->GetBinError(i) );

    NoFabsAJ_lo_max->SetBinContent (i, Thistos.at(0)->GetBinContent(i) + NoFabsAJ_lo_minmax->GetBinError (i) );
    NoFabsAJ_lo_max->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
  }

  NoFabsThistosLo=Thistos;
  NoFabsEhistosLo=Ehistos;

  // Followed
  // --------
  TH1D* OtherAJ_lo_T = minmax (  Tfiles, "OtherAJ_lo", AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
  OtherAJ_lo_T->SetName("OtherAJ_lo_T");
  TH1D* OtherAJ_lo_E = minmax (  Efiles, "OtherAJ_lo", AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
  OtherAJ_lo_E->SetName("OtherAJ_lo_E");
  TH1D* OtherAJ_lo_minmax = OtherAJ_lo_T->Clone("OtherAJ_lo_minmax");
  for (int i=1; i<=OtherAJ_lo_minmax->GetNbinsX() ; ++ i ){
    OtherAJ_lo_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );

    OtherAJ_lo_minmax->SetBinError (i, sqrt( pow( OtherAJ_lo_T->GetBinError(i), 2) +
						 pow( OtherAJ_lo_E->GetBinError(i), 2) ) );
  }
  
  // Calculate various p-values
  // --------------------------
  TParameter<double> Pvalue;
  TFile* reffile = new TFile(refname, "READ");  
  TH2D* h2 = (TH2D*) reffile->Get("AJ_hi");
  int AuAuMultBinL = 1;
  int AuAuMultBinR = h2->GetNbinsY();
  if ( AuAuMultL > 0 ) AuAuMultBinL = h2->GetYaxis()->FindBin( AuAuMultL );
  if ( AuAuMultR > 0 ) AuAuMultBinR = h2->GetYaxis()->FindBin( AuAuMultR+1 );    

  TString newname;

  h2 = (TH2D*) reffile->Get("AJ_hi");
  newname= "refAJ_hi";  
  TH1D* refAJ_hi = h2->ProjectionX( newname, AuAuMultBinL, AuAuMultBinR );
  refAJ_hi->Rebin( REBIN );
  refAJ_hi->Scale(1./refAJ_hi->Integral(0, refAJ_hi->GetNbinsX()+1)); // Include over/underflow

  h2 = (TH2D*) reffile->Get("NoFabsAJ_hi");
  newname= "NoFabsrefAJ_hi";
  TH1D* NoFabsrefAJ_hi = h2->ProjectionX( newname, AuAuMultBinL, AuAuMultBinR );
  NoFabsrefAJ_hi->Rebin( REBIN );
  NoFabsrefAJ_hi->Scale(1./NoFabsrefAJ_hi->Integral(0, NoFabsrefAJ_hi->GetNbinsX()+1)); // Include over/underflow

  h2 = (TH2D*) reffile->Get("AJ_lo");
  newname= "refAJ_lo";  
  TH1D* refAJ_lo = h2->ProjectionX( newname, AuAuMultBinL, AuAuMultBinR );
  refAJ_lo->Rebin( REBIN );
  refAJ_lo->Scale(1./refAJ_lo->Integral(0, refAJ_lo->GetNbinsX()+1)); // Include over/underflow

  h2 = (TH2D*) reffile->Get("NoFabsAJ_lo");
  newname= "NoFabsrefAJ_lo";
  TH1D* NoFabsrefAJ_lo = h2->ProjectionX( newname, AuAuMultBinL, AuAuMultBinR );
  NoFabsrefAJ_lo->Rebin( REBIN );
  NoFabsrefAJ_lo->Scale(1./NoFabsrefAJ_lo->Integral(0, NoFabsrefAJ_lo->GetNbinsX()+1)); // Include over/underflow

  float xl=0.0;
  float xr=0.72-0.01;
  float yl=0.0;
  float yr=0.23;


  refAJ_hi->SetAxisRange(  yl, yr, "y");
  refAJ_hi->SetAxisRange(  xl, xr, "x");
  refAJ_lo->SetAxisRange(  yl, yr, "y");
  refAJ_lo->SetAxisRange(  xl, xr, "x");

  float NoFabsxl=-0.3;
  float NoFabsxr=0.8;
  float NoFabsyl=0.0;
  float NoFabsyr=0.25;
  NoFabsrefAJ_lo->SetAxisRange(  NoFabsyl, NoFabsyr, "y");
  NoFabsrefAJ_lo->SetAxisRange(  NoFabsxl, NoFabsxr, "x");

  // For Unbinned KS value
  // ---------------------
  vector<double> RefUnbinnedAJ_lo;
  vector<double> NoFabsRefUnbinnedAJ_lo;
  vector<double> RefUnbinnedAJ_hi;
  vector<double> NoFabsRefUnbinnedAJ_hi;
  CalcAj( reffile, AuAuMultL, AuAuMultR, RefUnbinnedAJ_lo, RefUnbinnedAJ_hi,"fabs" );
  CalcAj( reffile, AuAuMultL, AuAuMultR, NoFabsRefUnbinnedAJ_lo, NoFabsRefUnbinnedAJ_hi,"nofabs" );
  cout << "Done" << endl;
  vector<double> UnbinnedAJ_lo;
  vector<double> NoFabsUnbinnedAJ_lo;
  vector<double> UnbinnedAJ_hi;
  vector<double> NoFabsUnbinnedAJ_hi;

  // Open outputfile before creating output numbers
  // ----------------------------------------------
  TFile* out = new TFile( outname, "RECREATE");
  
  TString PvalBase="";
  for (int i=0; i<3 ; ++i){
    PvalBase="";
    switch (i){
    case 0:
      PvalBase="Nominal";
      break;
    case 1:
      PvalBase="Plus";
      break;
    case 2:
      PvalBase="Minus";
      break;
    default:
      cerr << "Unknown value for i" << endl;
      return -1;
      break;      
    }
    // Range
    EhistosHi.at(i)->SetAxisRange(  yl, yr, "y");
    EhistosHi.at(i)->SetAxisRange(  xl, xr, "x");
    ThistosHi.at(i)->SetAxisRange(  yl, yr, "y");
    ThistosHi.at(i)->SetAxisRange(  xl, xr, "x");

    // Chi^2
    // -----
    //  ---- Efficiency
    Pvalue.SetVal( EhistosHi.at(i)->Chi2Test(refAJ_hi, ""));
    Pvalue.Write( "Chi2_" + PvalBase+"E_Hi");
    Pvalue.SetVal( NoFabsEhistosHi.at(i)->Chi2Test(NoFabsrefAJ_hi, ""));
    Pvalue.Write( "NoFabsChi2_" + PvalBase+"E_Hi");    
    Pvalue.SetVal( EhistosLo.at(i)->Chi2Test(refAJ_lo, ""));
    Pvalue.Write( "Chi2_" + PvalBase+"E_Lo");
    Pvalue.SetVal( NoFabsEhistosLo.at(i)->Chi2Test(NoFabsrefAJ_lo, ""));
    Pvalue.Write( "NoFabsChi2_" + PvalBase+"E_Lo");    

    //  ---- Tower Scale
    Pvalue.SetVal( ThistosHi.at(i)->Chi2Test(refAJ_hi, ""));
    Pvalue.Write( "Chi2_" + PvalBase+"T_Hi");
    Pvalue.SetVal( NoFabsThistosHi.at(i)->Chi2Test(NoFabsrefAJ_hi, ""));
    Pvalue.Write( "NoFabsChi2_" + PvalBase+"T_Hi");    
    Pvalue.SetVal( ThistosLo.at(i)->Chi2Test(refAJ_lo, ""));
    Pvalue.Write( "Chi2_" + PvalBase+"T_Lo");
    Pvalue.SetVal( NoFabsThistosLo.at(i)->Chi2Test(NoFabsrefAJ_lo, ""));
    Pvalue.Write( "NoFabsChi2_" + PvalBase+"T_Lo");    

    // Kolmogorov BINNED
    // -----------------
    //  ---- Efficiency
    Pvalue.SetVal( EhistosHi.at(i)->KolmogorovTest(refAJ_hi, ""));
    Pvalue.Write( "BinKS_" + PvalBase+"E_Hi");
    Pvalue.SetVal( NoFabsEhistosHi.at(i)->KolmogorovTest(NoFabsrefAJ_hi, ""));
    Pvalue.Write( "NoFabsBinKS_" + PvalBase+"E_Hi");
    Pvalue.SetVal( EhistosLo.at(i)->KolmogorovTest(refAJ_lo, ""));
    Pvalue.Write( "BinKS_" + PvalBase+"E_Lo");
    Pvalue.SetVal( NoFabsEhistosLo.at(i)->KolmogorovTest(NoFabsrefAJ_lo, ""));
    Pvalue.Write( "NoFabsBinKS_" + PvalBase+"E_Lo");
    
    //  ---- Tower Scale
    Pvalue.SetVal( ThistosHi.at(i)->KolmogorovTest(refAJ_hi, ""));
    Pvalue.Write( "BinKS_" + PvalBase+"T_Hi");
    Pvalue.SetVal( NoFabsThistosHi.at(i)->KolmogorovTest(NoFabsrefAJ_hi, ""));
    Pvalue.Write( "NoFabsBinKS_" + PvalBase+"T_Hi");
    Pvalue.SetVal( ThistosLo.at(i)->KolmogorovTest(refAJ_lo, ""));
    Pvalue.Write( "BinKS_" + PvalBase+"T_Lo");
    Pvalue.SetVal( NoFabsThistosLo.at(i)->KolmogorovTest(NoFabsrefAJ_lo, ""));
    Pvalue.Write( "NoFabsBinKS_" + PvalBase+"T_Lo");

    // Kolmogorov UNBINNED
    // -------------------
    //  ---- Efficiency
    CalcAj( Efiles.at(i), AuAuMultL, AuAuMultR, UnbinnedAJ_lo, UnbinnedAJ_hi );
    Pvalue.SetVal( TMath::KolmogorovTest( UnbinnedAJ_hi.size(), (const double*) &UnbinnedAJ_hi.at(0), RefUnbinnedAJ_hi.size(), (const double*) &RefUnbinnedAJ_hi.at(0), "") );
    Pvalue.Write( "KS_" + PvalBase+"E_Hi");
    Pvalue.SetVal( TMath::KolmogorovTest( UnbinnedAJ_lo.size(), (const double*) &UnbinnedAJ_lo.at(0), RefUnbinnedAJ_lo.size(), (const double*) &RefUnbinnedAJ_lo.at(0), "") );
    Pvalue.Write( "KS_" + PvalBase+"E_Lo");

    CalcAj( Efiles.at(i), AuAuMultL, AuAuMultR, NoFabsUnbinnedAJ_hi, UnbinnedAJ_hi, "nofabs" );
    Pvalue.SetVal( TMath::KolmogorovTest( NoFabsUnbinnedAJ_hi.size(), (const double*) &NoFabsUnbinnedAJ_hi.at(0), NoFabsRefUnbinnedAJ_hi.size(), (const double*) &NoFabsRefUnbinnedAJ_hi.at(0), "") );
    Pvalue.Write( "KS_NoFabs" + PvalBase+"E_Hi");
    CalcAj( Efiles.at(i), AuAuMultL, AuAuMultR, NoFabsUnbinnedAJ_lo, UnbinnedAJ_hi, "nofabs" );
    Pvalue.SetVal( TMath::KolmogorovTest( NoFabsUnbinnedAJ_lo.size(), (const double*) &NoFabsUnbinnedAJ_lo.at(0), NoFabsRefUnbinnedAJ_lo.size(), (const double*) &NoFabsRefUnbinnedAJ_lo.at(0), "") );
    Pvalue.Write( "KS_NoFabs" + PvalBase+"E_Lo");

    // cout << "HI, KS UNbinned, p-value = "
    // 	 << TMath::KolmogorovTest( UnbinnedAJ_hi.size(), (const double*) &UnbinnedAJ_hi.at(0), RefUnbinnedAJ_hi.size(), (const double*) &RefUnbinnedAJ_hi.at(0), "")
    // 	 << endl;
    // cout << "LO, KS UNbinned, p-value = "
    // 	 << TMath::KolmogorovTest( UnbinnedAJ_lo.size(), (const double*) &UnbinnedAJ_lo.at(0), RefUnbinnedAJ_lo.size(), (const double*) &RefUnbinnedAJ_lo.at(0), "")
    // 	 << endl;

    //  ---- Tower Scale
    CalcAj( Tfiles.at(i), AuAuMultL, AuAuMultR, UnbinnedAJ_lo, UnbinnedAJ_hi );
    Pvalue.SetVal( TMath::KolmogorovTest( UnbinnedAJ_hi.size(), (const double*) &UnbinnedAJ_hi.at(0), RefUnbinnedAJ_hi.size(), (const double*) &RefUnbinnedAJ_hi.at(0), "") );
    Pvalue.Write( "KS_" + PvalBase+"T_Hi");
    Pvalue.SetVal( TMath::KolmogorovTest( UnbinnedAJ_lo.size(), (const double*) &UnbinnedAJ_lo.at(0), RefUnbinnedAJ_lo.size(), (const double*) &RefUnbinnedAJ_lo.at(0), "") );
    Pvalue.Write( "KS_" + PvalBase+"T_Lo");

    CalcAj( Tfiles.at(i), AuAuMultL, AuAuMultR, NoFabsUnbinnedAJ_hi, UnbinnedAJ_hi, "nofabs" );
    Pvalue.SetVal( TMath::KolmogorovTest( NoFabsUnbinnedAJ_hi.size(), (const double*) &NoFabsUnbinnedAJ_hi.at(0), NoFabsRefUnbinnedAJ_hi.size(), (const double*) &NoFabsRefUnbinnedAJ_hi.at(0), "") );
    Pvalue.Write( "KS_NoFabs" + PvalBase+"T_hi");
    CalcAj( Tfiles.at(i), AuAuMultL, AuAuMultR, NoFabsUnbinnedAJ_lo, UnbinnedAJ_hi, "nofabs" );
    Pvalue.SetVal( TMath::KolmogorovTest( NoFabsUnbinnedAJ_lo.size(), (const double*) &NoFabsUnbinnedAJ_lo.at(0), NoFabsRefUnbinnedAJ_lo.size(), (const double*) &NoFabsRefUnbinnedAJ_lo.at(0), "") );
    Pvalue.Write( "KS_NoFabs" + PvalBase+"T_Lo");

    // cout << "HI, KS UNbinned, p-value = "
    // 	 << TMath::KolmogorovTest( UnbinnedAJ_hi.size(), (const double*) &UnbinnedAJ_hi.at(0), RefUnbinnedAJ_hi.size(), (const double*) &RefUnbinnedAJ_hi.at(0), "")
    // 	 << endl;
    // cout << "LO, KS UNbinned, p-value = "
    // 	 << TMath::KolmogorovTest( UnbinnedAJ_lo.size(), (const double*) &UnbinnedAJ_lo.at(0), RefUnbinnedAJ_lo.size(), (const double*) &RefUnbinnedAJ_lo.at(0), "")
    // 	 << endl;
  }

  // Using dumb qudrature min max
  // ----------------------------
  AJ_hi_min->SetAxisRange(  yl, yr, "y");
  AJ_hi_min->SetAxisRange(  xl, xr, "x");
  AJ_hi_max->SetAxisRange(  yl, yr, "y");
  AJ_hi_max->SetAxisRange(  xl, xr, "x");
  
  AJ_lo_min->SetAxisRange(  yl, yr, "y");
  AJ_lo_min->SetAxisRange(  xl, xr, "x");
  AJ_lo_max->SetAxisRange(  yl, yr, "y");
  AJ_lo_max->SetAxisRange(  xl, xr, "x");

  // Chi^2 using dumb qudrature min max
  // ----------------------------------
  Pvalue.SetVal( AJ_hi_min->Chi2Test(refAJ_hi, ""));
  Pvalue.Write( "Chi2_Hi_min");
  Pvalue.SetVal( AJ_hi_max->Chi2Test(refAJ_hi, ""));
  Pvalue.Write( "Chi2_Hi_max");
  
  Pvalue.SetVal( AJ_lo_min->Chi2Test(refAJ_lo, ""));
  Pvalue.Write( "Chi2_Lo_min");
  Pvalue.SetVal( AJ_lo_max->Chi2Test(refAJ_lo, ""));
  Pvalue.Write( "Chi2_Lo_max");

  // KS using dumb qudrature min max
  // -------------------------------
  Pvalue.SetVal( AJ_hi_min->KolmogorovTest(refAJ_hi, ""));
  Pvalue.Write( "BinKS_Hi_min");
  Pvalue.SetVal( AJ_hi_max->KolmogorovTest(refAJ_hi, ""));
  Pvalue.Write( "BinKS_Hi_max");
  
  Pvalue.SetVal( AJ_lo_min->KolmogorovTest(refAJ_lo, ""));
  Pvalue.Write( "BinKS_Lo_min");
  Pvalue.SetVal( AJ_lo_max->KolmogorovTest(refAJ_lo, ""));
  Pvalue.Write( "BinKS_Lo_max");

  // Finally, use standard value with quadrature errors
  // --------------------------------------------------
  TH1D* AJ_hi_sysstat = (TH1D*) EhistosHi.at(0)->Clone( "AJ_hi_sysstat");
  for (int i=1; i<AJ_hi_sysstat->GetNbinsX(); ++i ){
    if ( fabs (AJ_hi_sysstat->GetBinContent(i)-AJ_hi_minmax->GetBinContent(i))> 1e-4 ){
      cerr << "syst. and stat. histo are incompatible" << endl;
      return -1;
    }
    AJ_hi_sysstat->SetBinError( i, TMath::Sqrt( pow( AJ_hi_sysstat->GetBinError(i), 2) + pow( AJ_hi_minmax->GetBinError(i), 2) ));
  }

  TH1D* AJ_lo_sysstat = (TH1D*) EhistosLo.at(0)->Clone( "AJ_lo_sysstat");
  for (int i=1; i<AJ_lo_sysstat->GetNbinsX(); ++i ){
    if ( fabs (AJ_lo_sysstat->GetBinContent(i)-AJ_lo_minmax->GetBinContent(i))> 1e-4 ){
      cerr << "syst. and stat. histo are incompatible" << endl;
      return -1;
    }
    AJ_lo_sysstat->SetBinError( i, TMath::Sqrt( pow( AJ_lo_sysstat->GetBinError(i), 2) + pow( AJ_lo_minmax->GetBinError(i), 2) ));
  }
  
  TH1D* NoFabsAJ_hi_sysstat = (TH1D*) NoFabsEhistosHi.at(0)->Clone( "NoFabsAJ_hi_sysstat");
  for (int i=1; i<NoFabsAJ_hi_sysstat->GetNbinsX(); ++i ){
    if ( fabs (NoFabsAJ_hi_sysstat->GetBinContent(i)-NoFabsAJ_hi_minmax->GetBinContent(i))> 1e-4 ){
      cerr << "syst. and stat. histo are incompatible" << endl;
      return -1;
    }
    NoFabsAJ_hi_sysstat->SetBinError( i, TMath::Sqrt( pow( NoFabsAJ_hi_sysstat->GetBinError(i), 2) + pow( NoFabsAJ_hi_minmax->GetBinError(i), 2) ));
  }
  TH1D* NoFabsAJ_lo_sysstat = (TH1D*) NoFabsEhistosLo.at(0)->Clone( "NoFabsAJ_lo_sysstat");
  for (int i=1; i<NoFabsAJ_lo_sysstat->GetNbinsX(); ++i ){
    if ( fabs (NoFabsAJ_lo_sysstat->GetBinContent(i)-NoFabsAJ_lo_minmax->GetBinContent(i))> 1e-4 ){
      cerr << "syst. and stat. histo are incompatible" << endl;
      return -1;
    }
    NoFabsAJ_lo_sysstat->SetBinError( i, TMath::Sqrt( pow( NoFabsAJ_lo_sysstat->GetBinError(i), 2) + pow( NoFabsAJ_lo_minmax->GetBinError(i), 2) ));
  }

  AJ_hi_sysstat->SetAxisRange(  yl, yr, "y");
  AJ_hi_sysstat->SetAxisRange(  xl, xr, "x");
  AJ_lo_sysstat->SetAxisRange(  yl, yr, "y");
  AJ_lo_sysstat->SetAxisRange(  xl, xr, "x");
  NoFabsAJ_hi_sysstat->SetAxisRange(  NoFabsyl, NoFabsyr, "y");
  NoFabsAJ_hi_sysstat->SetAxisRange(  NoFabsxl, NoFabsxr, "x");
  NoFabsAJ_lo_sysstat->SetAxisRange(  NoFabsyl, NoFabsyr, "y");
  NoFabsAJ_lo_sysstat->SetAxisRange(  NoFabsxl, NoFabsxr, "x");

  // Chi^2 using dumb errors
  // -----------------------
  Pvalue.SetVal( AJ_hi_sysstat->Chi2Test(refAJ_hi, ""));
  Pvalue.Write( "Chi2_QErrors_Hi");
  Pvalue.SetVal( AJ_lo_sysstat->Chi2Test(refAJ_lo, ""));
  Pvalue.Write( "Chi2_QErrors_Lo");
  Pvalue.SetVal( NoFabsAJ_lo_sysstat->Chi2Test(NoFabsrefAJ_lo, ""));
  Pvalue.Write( "NoFabsChi2_QErrors_Lo");

  // KS using dumb errors
  // --------------------
  Pvalue.SetVal( AJ_hi_sysstat->KolmogorovTest(refAJ_hi, ""));
  Pvalue.Write( "BinKS_QErrors_Hi");
  Pvalue.SetVal( AJ_lo_sysstat->KolmogorovTest(refAJ_lo, ""));
  Pvalue.Write( "BinKS_QErrors_Lo");
  Pvalue.SetVal( NoFabsAJ_lo_sysstat->KolmogorovTest(NoFabsrefAJ_lo, ""));
  Pvalue.Write( "NoFabsBinKS_QErrors_Lo");

  // Done. Save
  // ----------
  for (int i=0; i<ratios.size(); ++i){
    ratios.at(i)->Write();
  }
  for (int i=0; i<minandmax.size(); ++i){
    minandmax.at(i)->Write();
  }

  UnmatchedAJ_hi_E->Write();
  UnmatchedAJ_hi_T->Write();
  UnmatchedAJ_hi_minmax->Write();

  OtherAJ_lo_E->Write();
  OtherAJ_lo_T->Write();
  OtherAJ_lo_minmax->Write();

  AJ_hi_E->Write();
  AJ_hi_T->Write();
  AJ_hi_minmax->Write();
  AJ_hi_min->Write();
  AJ_hi_max->Write();

  NoFabsAJ_hi_E->Write();
  NoFabsAJ_hi_T->Write();
  NoFabsAJ_hi_minmax->Write();
  NoFabsAJ_hi_min->Write();
  NoFabsAJ_hi_max->Write();

  AJ_lo_E->Write();
  AJ_lo_T->Write();
  AJ_lo_minmax->Write();
  AJ_lo_min->Write();
  AJ_lo_max->Write();
  
  NoFabsAJ_lo_E->Write();
  NoFabsAJ_lo_T->Write();
  NoFabsAJ_lo_minmax->Write();
  NoFabsAJ_lo_min->Write();
  NoFabsAJ_lo_max->Write();

  cout << "Wrote to " << endl << out->GetName() << endl;
  return 0;
  
}



// ===========================================================================
TH1D* minmax ( vector<TFile*> files, TString which, int AuAuMultL, int AuAuMultR, TString namehelper,  vector<TH1D*>& histos,
	       vector<TH1D*>& ratios, vector<TH1D*>& minandmax ){
  histos.clear();

  
  TFile* file;
  TH2D* h2;
  TH1D* h1;
  
  int AuAuMultBinL;
  int AuAuMultBinR;

  for ( int f=0; f<files.size() ; ++f ){
    file = files.at(f);
    h2 = (TH2D*) file->Get(which);
    AuAuMultBinL = 1;
    AuAuMultBinR = h2->GetNbinsY();
    
    if ( AuAuMultL > 0 ) AuAuMultBinL = h2->GetYaxis()->FindBin( AuAuMultL );
    if ( AuAuMultR > 0 ) AuAuMultBinR = h2->GetYaxis()->FindBin( AuAuMultR+1 );    

    TString newname = which;
    newname+="_"; newname+=namehelper; newname+=f;
    h1 = h2->ProjectionX( newname, AuAuMultBinL, AuAuMultBinR );

    h1->Rebin(REBIN);
    // h1->Scale(1./h1->Integral());
    h1->Scale(1./h1->Integral(0, h1->GetNbinsX()+1)); // Include over/underflow

    histos.push_back( h1 );		      
  }
  
  TH1D* ret = histos.at(0)->Clone( which+"_minmax" );
  ret->Reset();

  TH1D* hmin = histos.at(0)->Clone( TString(histos.at(0)->GetName())+"_min" );
  hmin->Reset();
  TH1D* hmax = histos.at(0)->Clone( TString(histos.at(0)->GetName())+"_max" );
  hmax->Reset();

  double min, max;
  for (int i=1; i<=ret->GetNbinsX() ; ++ i ){
    min = 1e15;
    max = -1e15;
    
    for ( int j=0; j<histos.size() ; ++j ){
      if ( histos.at(j)->GetBinContent(i) < min ) min = histos.at(j)->GetBinContent(i);
      if ( histos.at(j)->GetBinContent(i) > max ) max = histos.at(j)->GetBinContent(i);
    }

    ret->SetBinContent( i, 0.5 * ( max+min ) );
    ret->SetBinError( i, 0.5 * ( max-min ) );   
    
    hmin->SetBinContent( i, min );
    hmin->SetBinError( i, histos.at(0)->GetBinError(i) );   

    hmax->SetBinContent( i, max );
    hmax->SetBinError( i, histos.at(0)->GetBinError(i) );   
    
  }

  minandmax.push_back(hmin);
  minandmax.push_back(hmax);

  // ratios. Assuming 0 is nominal
  // -----------------------------
  TH1D* nom = histos.at(0)->Clone( TString(histos.at(0)->GetName()) + "_nom");
  
  TH1D* ratio1 = histos.at(1)->Clone( TString(histos.at(1)->GetName()) + "_ratio");
  ratio1->GetYaxis()->SetTitle("ratio to nominal");
  ratio1->Divide( nom);
  ratios.push_back(ratio1);
  TH1D* ratio2 = histos.at(2)->Clone( TString(histos.at(2)->GetName()) + "_ratio");
  ratio2->GetYaxis()->SetTitle("ratio to nominal");
  ratio2->Divide( nom);
  ratios.push_back(ratio2);


  if ( gPad ){      
    histos.at(0)->SetTitle( "");
    // histos.at(0)->SetAxisRange(  0.0, 0.23, "y");
    // histos.at(0)->SetAxisRange(  0.0, 0.72, "x");
    histos.at(0)->SetAxisRange(  0.0, 0.25, "y");
    histos.at(0)->SetAxisRange(  -0.3, 0.8, "x");
    histos.at(0)->Draw();

    histos.at(1)->SetLineWidth(2);
    histos.at(1)->SetLineColor(kBlue);
    histos.at(1)->Draw("same");

    histos.at(2)->SetLineWidth(2);
    histos.at(2)->SetLineColor(kMagenta+1);
    histos.at(2)->Draw("same");
    
    ret->SetFillStyle(3001 );
    ret->SetFillColor( kGray+1 );  
    
    ret->Draw("9E2same");

    //   histos.at(0)->Draw();
    // for ( int i=1; i<histos.size() ; ++i ){
    //   histos.at(i)->Draw("same");
    // }
    // ret->SetFillStyle(3001 );
    // ret->SetFillColor( kGray+1 );  
    
    // ret->Draw("9E2same");

    TLegend* leg = new TLegend ( 0.6, 0.6, 0.88, 0.88 );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    // leg->SetMargin(0.1);

    leg->AddEntry(histos.at(0)->GetName(), "Nominal", "l");
    if ( namehelper.Contains("T") ){
      leg->AddEntry(histos.at(1)->GetName(), "Tow Scale Max", "l");
      leg->AddEntry(histos.at(2)->GetName(), "Tow Scale Min", "l");
    } else {
      leg->AddEntry(histos.at(1)->GetName(), "Track Eff Max", "l");
      leg->AddEntry(histos.at(2)->GetName(), "Track Eff Min", "l");
    }
    leg->AddEntry(ret->GetName(), "Uncertainty", "f");
    leg->Draw("same");
    // cout << histos.at(0)->GetName() << endl;
  }
  return ret;

}

// ===========================================================================
void CalcAj ( TFile* file, int AuAuMultL, int AuAuMultR, vector<double>& UnbinnedAj_lo, vector<double>& UnbinnedAj_hi, TString opt ){

  UnbinnedAj_hi.clear();
  UnbinnedAj_lo.clear();
  
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
    // cout << i << endl;

    if ( refmult<AuAuMultL || refmult > AuAuMultR ) continue;
    UnbinnedAj_hi.push_back( CalcAj( pJ1, pJ2, opt=="nofabs" ) );
    UnbinnedAj_lo.push_back( CalcAj( pJM1, pJM2, opt=="nofabs" ) );
  }

  // Grrr - need to be sorted
  std::sort(UnbinnedAj_lo.begin(), UnbinnedAj_lo.end());
  std::sort(UnbinnedAj_hi.begin(), UnbinnedAj_hi.end());
}

// ===========================================================================
double CalcAj ( TLorentzVector* j1, TLorentzVector* j2, bool nofabs ){
  // return fabs (( j1->Pt()-j2->Pt() ) / ( j1->Pt()+j2->Pt() ));
  if ( nofabs )  return ( j1->Pt()-j2->Pt() ) / ( j1->Pt()+j2->Pt() );
  return fabs( ( j1->Pt()-j2->Pt() ) / ( j1->Pt()+j2->Pt() ) );

  return -999;
}
    
    
