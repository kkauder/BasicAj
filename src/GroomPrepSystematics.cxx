#include <vector>

#define REBIN 1

void CalcAj ( TFile* file, int AuAuMultL, int AuAuMultR, vector<double>& UnbinnedAj_lo, vector<double>& UnbinnedAj_hi, TString opt="fabs" );
double CalcAj ( TLorentzVector* j1, TLorentzVector* j2, bool nofabs=false );
TH1D* minmax ( vector<TFile*> files, TString which, int AuAuMultL, int AuAuMultR, TString namehelper,  vector<TH1D*>& histos, 
	       vector<TH1D*>& ratios, vector<TH1D*>& minandmax );


int GroomPrepSystematics( TString R="0.4", int AuAuMultL=269, int AuAuMultR=-1  )
//int PrepSystematics( TString R="Pt1", int AuAuMultL=351, int AuAuMultR=-1  )
{
  // TString outname = "AjResults/Systematics_Groom_Fresh_NicksList_HC100_ppInAuAuAj.root";
  // TString refname = "AjResults/Groom_Fresh_NicksList_HC100.root";
  
  // TString ft0e0 = "AjResults/Tow0_Eff0_Groom_Fresh_NicksList_HC100_ppInAuAuAj.root";
  
  // TString ftPe0 = "AjResults/Tow1_Eff0_Groom_Fresh_NicksList_HC100_ppInAuAuAj.root";
  // TString ftMe0 = "AjResults/Tow-1_Eff0_Groom_Fresh_NicksList_HC100_ppInAuAuAj.root";

  // TString ft0eP = "AjResults/Tow0_Eff1_Groom_Fresh_NicksList_HC100_ppInAuAuAj.root";
  // TString ft0eM = "AjResults/Tow0_Eff-1_Groom_Fresh_NicksList_HC100_ppInAuAuAj.root";

  TString refname = "AjResults/Groom_AuAu_HT54_HTled.root";

  // TString Base = "Groom_Aj_HT54_HTled_ppInAuAuAj";
  TString Base = "Groom_Aj_HT54_HTled_ppInAuAuAj_AjLt3";
  // TString Base = "Groom_Aj_HT54_HTled_ppInAuAuAj_AjGt3";

  TString outname = "AjResults/zgSystematics_" + Base + ".root";
  
  TString ft0e0 = "AjResults/Tow0_Eff0_" + Base + ".root";
  TString ftPe0 = "AjResults/Tow1_Eff0_" + Base + ".root";
  TString ftMe0 = "AjResults/Tow-1_Eff0_" + Base + ".root";

  TString ft0eP = "AjResults/Tow0_Eff1_" + Base + ".root";
  TString ft0eM = "AjResults/Tow0_Eff-1_" + Base + ".root";


  if ( AuAuMultL==351  ){
    cerr << "Don't try bad AuAuMultL settings" << endl;
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

  vector<TH1D*> Thistos;
  vector<TH1D*> Ehistos;
  vector<TH1D*> ratios;
  vector<TH1D*> minandmax;

  bool visualize=true;
  if (visualize){
    TCanvas* c = new TCanvas("c");
    gPad->SetGridx(0);  gPad->SetGridy(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptDate(0);    
  }
  TString plotname;
    
  TH1D* h_T=0;
  TH1D* h_E=0;
  TString which;

  // Open outputfile
  // ---------------
  TFile* out = new TFile( outname, "RECREATE");

  // Matched, high
  // -------------
  vector<string> whiches;
  whiches.push_back ( "zgLead2030Hi" );
  whiches.push_back ( "zgLead2030Lo" );
  whiches.push_back ( "zgLead3040Hi" );
  whiches.push_back ( "zgLead3040Lo" );
  whiches.push_back ( "zgLead4060Hi" );
  whiches.push_back ( "zgLead4060Lo" );

  whiches.push_back ( "zgSubLead1020Hi" );
  whiches.push_back ( "zgSubLead1020Lo" );
  whiches.push_back ( "zgSubLead2030Hi" );
  whiches.push_back ( "zgSubLead2030Lo" );
  whiches.push_back ( "zgSubLead3040Hi" );
  whiches.push_back ( "zgSubLead3040Lo" );


  for (int w = 0; w<whiches.size(); ++w ){
    which = whiches.at(w);
    cout << which << endl;

    h_T = minmax (  Tfiles, which, AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
    h_T->SetName( which + "_T" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_T->GetName() + ".png";
      gPad->SaveAs( plotname );
    }
    
    h_E = minmax (  Efiles, which, AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
    h_E->SetName( which + "_E" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_E->GetName() + ".png";
      gPad->SaveAs( plotname );
    }
    
    TH1D* h_minmax = h_T->Clone(which + "_minmax");
    TH1D* h_min = h_T->Clone(which + "_min");
    TH1D* h_max = h_T->Clone(which + "_max");
    
    for (int i=1; i<=h_minmax->GetNbinsX() ; ++ i ){
      h_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );
      
      h_minmax->SetBinError (i, sqrt( pow( h_T->GetBinError(i), 2) +
				      pow( h_E->GetBinError(i), 2) ) );
      
      h_min->SetBinContent (i, Thistos.at(0)->GetBinContent(i) - h_minmax->GetBinError (i) );
      h_min->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
      
      h_max->SetBinContent (i, Thistos.at(0)->GetBinContent(i) + h_minmax->GetBinError (i) );
      h_max->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
      
    }
  }
  
  // Done. Save
  // ----------
  out->Write();  

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
    
    
