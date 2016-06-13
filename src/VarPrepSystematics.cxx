#include <vector>

TH1D* minmax ( vector<TFile*> files, TString which, int AuAuMultL, int AuAuMultR, TString namehelper,  vector<TH1D*>& histos, 
	       vector<TH1D*>& ratios, vector<TH1D*>& minandmax );

void DoMinMax( vector<TFile*>& Tfiles, vector<TFile*>& Efiles, TFile* reffile, TFile* out, TString which, TString R, int rebin );
  
int VarPrepSystematics( TString R="0.4"  )
{

  TString outname = "AjResults/Var_Systematics_Fresh_NicksList_HC100_ppInAuAuAj.root";
  TString refname = "AjResults/Var_Fresh_NicksList_HC100_AuAu.root";
  
  TString ft0e0 = "AjResults/Var_Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root";
  
  TString ftPe0 = "AjResults/Var_Tow1_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root";
  TString ftMe0 = "AjResults/Var_Tow-1_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root";

  TString ft0eP = "AjResults/Var_Tow0_Eff1_Fresh_NicksList_HC100_ppInAuAuAj.root";
  TString ft0eM = "AjResults/Var_Tow0_Eff-1_Fresh_NicksList_HC100_ppInAuAuAj.root";

  if ( R=="0.2"  ){
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
      
  vector<TFile*> Tfiles;
  Tfiles.push_back(new TFile( ft0e0, "READ" ) );
  Tfiles.push_back(new TFile( ftPe0, "READ" ) );
  Tfiles.push_back(new TFile( ftMe0, "READ" ) );

  vector<TFile*> Efiles;
  Efiles.push_back(new TFile( ft0e0, "READ" ) );
  Efiles.push_back(new TFile( ft0eP, "READ" ) );
  Efiles.push_back(new TFile( ft0eM, "READ" ) );

  TFile* reffile = new TFile( refname, "READ");

  // Outputfile
  // ----------
  TFile* out = new TFile( outname, "RECREATE");

  bool visualize=true;
  if (visualize){
    TCanvas* c = new TCanvas("c");
    gPad->SetGridx(0);  gPad->SetGridy(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptDate(0);    
  }

  int rebin=2;

  DoMinMax( Tfiles, Efiles, reffile, out, "AJ", R, rebin );
  DoMinMax( Tfiles, Efiles, reffile, out, "NoFabsAJ", R, rebin );
  DoMinMax( Tfiles, Efiles, reffile, out, "PosOnlyAJ", R, rebin );
  DoMinMax( Tfiles, Efiles, reffile, out, "PerfectMatchAJ", R, rebin );

  out->Write();

  cout << "Wrote to " << endl << out->GetName() << endl;
  return 0;
}

// ----------------------------------------------------------------------------------------
void DoMinMax( vector<TFile*>& Tfiles, vector<TFile*>& Efiles, TFile* reffile, TFile* out, TString which, TString R, int rebin ){
  vector<TH1D*> Thistos;
  vector<TH1D*> Ehistos;
  vector<TH1D*> ratios;
  vector<TH1D*> minandmax;

  // for p-value stuff
  vector<TH1D*> ThistosHi;
  vector<TH1D*> EhistosHi;
  vector<TH1D*> ThistosLo;
  vector<TH1D*> EhistosLo;

  TString plotname;

  // Save AuAu
  // ---------
  // out->cd();
  TH1D* ref_lo = reffile->Get(which+"_lo")->Clone("AuAu" + which+"_lo");
  ref_lo->Rebin( rebin );
  ref_lo->Scale(1./ref_lo->Integral(0, ref_lo->GetNbinsX()+1)); // Include over/underflow
  TH1D* ref_hi = reffile->Get(which+"_hi")->Clone("AuAu" + which+"_hi");
  ref_hi->Rebin( rebin );
  ref_hi->Scale(1./ref_hi->Integral(0, ref_hi->GetNbinsX()+1)); // Include over/underflow
  

  // Matched, high
  // -------------
  TH1D* AJ_hi_T = minmax (  Tfiles, which+"_hi", "T", Thistos, ratios, minandmax, 2 );
  AJ_hi_T->SetName(which+"_hi_T");
  if (gPad){
    plotname = "plots/Var_" + R + "_" + AJ_hi_T->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_hi_E = minmax (  Efiles, which+"_hi", "E", Ehistos, ratios, minandmax, 2 );
  AJ_hi_E->SetName(which+"_hi_E");
  if (gPad){
    plotname = "plots/Var_" + R + "_" + AJ_hi_E->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_hi_minmax = AJ_hi_T->Clone(which+"_hi_minmax");
  TH1D* AJ_hi_min = AJ_hi_T->Clone(which+"_hi_min");
  TH1D* AJ_hi_max = AJ_hi_T->Clone(which+"_hi_max");

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

  // Matched, low
  // ------------
  TH1D* AJ_lo_T = minmax (  Tfiles, which+"_lo", "T", Thistos, ratios, minandmax, rebin );
  AJ_lo_T->SetName(which+"_lo_T");
  if (gPad){
    plotname = "plots/Var_" + R + "_" + AJ_lo_T->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_lo_E = minmax (  Efiles, which+"_lo", "E", Ehistos, ratios, minandmax, rebin );
  AJ_lo_E->SetName(which+"_lo_E");
  if (gPad){
    plotname = "plots/Var_" + R + "_" + AJ_lo_E->GetName() + ".png";
    gPad->SaveAs( plotname );
  }
  TH1D* AJ_lo_minmax = AJ_lo_T->Clone(which+"_lo_minmax");
  TH1D* AJ_lo_min = AJ_lo_T->Clone(which+"_lo_min");
  TH1D* AJ_lo_max = AJ_lo_T->Clone(which+"_lo_max");
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

  // Calculate p-values
  // ------------------
  TParameter<double> Pvalue;

  // For Unbinned KS value
  // ---------------------
  TVectorT<double> * RefUnbinnedAJ_lo = (TVectorT<double>*) reffile->Get("Unbinned"+which+"_lo");
  TVectorT<double> * RefUnbinnedAJ_hi = (TVectorT<double>*) reffile->Get("Unbinned"+which+"_hi");

  // cout << RefUnbinnedAJ_lo->GetNoElements() << endl;
  // cout << RefUnbinnedAJ_hi->GetNoElements() << endl;
 
  TVectorT<double> * UnbinnedAJ_lo = 0;
  TVectorT<double> * UnbinnedAJ_hi = 0;

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

    // out->cd();
    // Kolmogorov UNBINNED
    // -------------------
    //  ---- Efficiency
    UnbinnedAJ_lo = (TVectorT<double>*) Efiles.at(i)->Get("Unbinned"+which+"_lo");
    UnbinnedAJ_hi = (TVectorT<double>*) Efiles.at(i)->Get("Unbinned"+which+"_hi");
  
    Pvalue.SetVal( TMath::KolmogorovTest( UnbinnedAJ_hi->GetNoElements(), UnbinnedAJ_hi->GetMatrixArray(), RefUnbinnedAJ_hi->GetNoElements(), RefUnbinnedAJ_hi->GetMatrixArray(), "") );
    Pvalue.Write( "KS_" + which+ "_" + PvalBase+"E_Hi");

    UnbinnedAJ_lo = (TVectorT<double>*) Efiles.at(i)->Get("Unbinned"+which+"_lo");
    UnbinnedAJ_hi = (TVectorT<double>*) Efiles.at(i)->Get("Unbinned"+which+"_hi");
  
    Pvalue.SetVal( TMath::KolmogorovTest( UnbinnedAJ_lo->GetNoElements(), UnbinnedAJ_lo->GetMatrixArray(), RefUnbinnedAJ_lo->GetNoElements(), RefUnbinnedAJ_lo->GetMatrixArray(), "") );
    Pvalue.Write( "KS_" + which+ "_" + PvalBase+"E_Lo");


    //  ---- Tower Scale
    UnbinnedAJ_lo = (TVectorT<double>*) Tfiles.at(i)->Get("Unbinned"+which+"_lo");
    UnbinnedAJ_hi = (TVectorT<double>*) Tfiles.at(i)->Get("Unbinned"+which+"_hi");
  
    Pvalue.SetVal( TMath::KolmogorovTest( UnbinnedAJ_hi->GetNoElements(), UnbinnedAJ_hi->GetMatrixArray(), RefUnbinnedAJ_hi->GetNoElements(), RefUnbinnedAJ_hi->GetMatrixArray(), "") );
    Pvalue.Write( "KS_" + which+ "_" + PvalBase+"T_Hi");

    UnbinnedAJ_lo = (TVectorT<double>*) Tfiles.at(i)->Get("Unbinned"+which+"_lo");
    UnbinnedAJ_hi = (TVectorT<double>*) Tfiles.at(i)->Get("Unbinned"+which+"_hi");
  
    Pvalue.SetVal( TMath::KolmogorovTest( UnbinnedAJ_lo->GetNoElements(), UnbinnedAJ_lo->GetMatrixArray(), RefUnbinnedAJ_lo->GetNoElements(), RefUnbinnedAJ_lo->GetMatrixArray(), "") );
    Pvalue.Write( "KS_" + which+ "_" + PvalBase+"T_Lo");
  }


  
}



// ===========================================================================
TH1D* minmax ( vector<TFile*> files, TString which, TString namehelper,  vector<TH1D*>& histos,
	       vector<TH1D*>& ratios, vector<TH1D*>& minandmax, int rebin ){

  histos.clear();
  
  TFile* file;
  TH1D* h1;
  
  for ( int f=0; f<files.size() ; ++f ){
    file = files.at(f);
    h1 = (TH1D*) file->Get(which);

    TString newname = which;
    newname+="_"; newname+=namehelper; newname+=f;
    h1 = (TH1D*) h1->Clone( newname );

    h1->Rebin(2);
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

    
