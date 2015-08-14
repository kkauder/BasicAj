#include <vector>
int PrepSystematics( TString R="0.2", int AuAuMultL=269, int AuAuMultR=-1  )
//int PrepSystematics( TString R="Pt1", int AuAuMultL=351, int AuAuMultR=-1  )
{

  TString outname = "AjResults/Systematics_HC30_ppInAuAuAj.root";
  
  TString ft0e0 = "AjResults/Tow0_Eff0_HC30_ppInAuAuAj.root";
  
  TString ftPe0 = "AjResults/Tow1_Eff0_HC30_ppInAuAuAj.root";
  TString ftMe0 = "AjResults/Tow-1_Eff0_HC30_ppInAuAuAj.root";

  TString ft0eP = "AjResults/Tow0_Eff1_HC30_ppInAuAuAj.root";
  TString ft0eM = "AjResults/Tow0_Eff-1_HC30_ppInAuAuAj.root";

  if ( R=="0.2"  ){
    // ft0e0.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // ftPe0.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // ftMe0.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // ft0eP.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // ft0eM.ReplaceAll("ppInAuAuAj", "R0.2_ppInAuAuAj");
    // outname.ReplaceAll("Systematics", "R0.2_Systematics");
    ft0e0.ReplaceAll("HC30", "R0.2_HC30");
    ftPe0.ReplaceAll("HC30", "R0.2_HC30");
    ftMe0.ReplaceAll("HC30", "R0.2_HC30");
    ft0eP.ReplaceAll("HC30", "R0.2_HC30");
    ft0eM.ReplaceAll("HC30", "R0.2_HC30");
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
  TH1D* UnmatchedAJ_hi_T = minmax (  Tfiles, "UnmatchedAJ_hi", AuAuMultL, AuAuMultR, "T", Thistos );
  UnmatchedAJ_hi_T->SetName("UnmatchedAJ_hi_T");
  TH1D* UnmatchedAJ_hi_E = minmax (  Efiles, "UnmatchedAJ_hi", AuAuMultL, AuAuMultR, "E", Ehistos );
  UnmatchedAJ_hi_E->SetName("UnmatchedAJ_hi_E");
  TH1D* UnmatchedAJ_hi_minmax = UnmatchedAJ_hi_T->Clone("UnmatchedAJ_hi_minmax");

  for (int i=1; i<=UnmatchedAJ_hi_minmax->GetNbinsX() ; ++ i ){
    UnmatchedAJ_hi_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );
    UnmatchedAJ_hi_minmax->SetBinError (i, sqrt( pow( UnmatchedAJ_hi_T->GetBinError(i), 2) +
						 pow( UnmatchedAJ_hi_E->GetBinError(i), 2) ) );
  }
    
  // Matched, high
  // -------------
  TH1D* AJ_hi_T = minmax (  Tfiles, "AJ_hi", AuAuMultL, AuAuMultR, "T", Thistos );
  AJ_hi_T->SetName("AJ_hi_T");
  if (gPad){
    plotname = "plots/" + R + "_" + AJ_hi_T->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_hi_E = minmax (  Efiles, "AJ_hi", AuAuMultL, AuAuMultR, "E", Ehistos );
  AJ_hi_E->SetName("AJ_hi_E");
  if (gPad){
    plotname = "plots/" + R + "_" + AJ_hi_E->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_hi_minmax = AJ_hi_T->Clone("AJ_hi_minmax");
  for (int i=1; i<=AJ_hi_minmax->GetNbinsX() ; ++ i ){
    AJ_hi_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );

    AJ_hi_minmax->SetBinError (i, sqrt( pow( AJ_hi_T->GetBinError(i), 2) +
						 pow( AJ_hi_E->GetBinError(i), 2) ) );
  }
  
  // Matched, low
  // ------------
  TH1D* AJ_lo_T = minmax (  Tfiles, "AJ_lo", AuAuMultL, AuAuMultR, "T", Thistos );
  AJ_lo_T->SetName("AJ_lo_T");
  if (gPad){
    plotname = "plots/" + R + "_" + AJ_lo_T->GetName() + ".png";
    gPad->SaveAs( plotname );
  }

  TH1D* AJ_lo_E = minmax (  Efiles, "AJ_lo", AuAuMultL, AuAuMultR, "E", Ehistos );
  AJ_lo_E->SetName("AJ_lo_E");
  if (gPad){
    plotname = "plots/" + R + "_" + AJ_lo_E->GetName() + ".png";
    gPad->SaveAs( plotname );
  }
  TH1D* AJ_lo_minmax = AJ_lo_T->Clone("AJ_lo_minmax");
  for (int i=1; i<=AJ_lo_minmax->GetNbinsX() ; ++ i ){
    AJ_lo_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );

    AJ_lo_minmax->SetBinError (i, sqrt( pow( AJ_lo_T->GetBinError(i), 2) +
						 pow( AJ_lo_E->GetBinError(i), 2) ) );
  }

  // Followed
  // --------
  TH1D* OtherAJ_lo_T = minmax (  Tfiles, "OtherAJ_lo", AuAuMultL, AuAuMultR, "T", Thistos );
  OtherAJ_lo_T->SetName("OtherAJ_lo_T");
  TH1D* OtherAJ_lo_E = minmax (  Efiles, "OtherAJ_lo", AuAuMultL, AuAuMultR, "E", Ehistos );
  OtherAJ_lo_E->SetName("OtherAJ_lo_E");
  TH1D* OtherAJ_lo_minmax = OtherAJ_lo_T->Clone("OtherAJ_lo_minmax");
  for (int i=1; i<=OtherAJ_lo_minmax->GetNbinsX() ; ++ i ){
    OtherAJ_lo_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );

    OtherAJ_lo_minmax->SetBinError (i, sqrt( pow( OtherAJ_lo_T->GetBinError(i), 2) +
						 pow( OtherAJ_lo_E->GetBinError(i), 2) ) );
  }
  
  // Done. Save
  // ----------
  TFile* out = new TFile( outname, "RECREATE");
  UnmatchedAJ_hi_E->Write();
  UnmatchedAJ_hi_T->Write();
  UnmatchedAJ_hi_minmax->Write();

  AJ_hi_E->Write();
  AJ_hi_T->Write();
  AJ_hi_minmax->Write();

  AJ_lo_E->Write();
  AJ_lo_T->Write();
  AJ_lo_minmax->Write();
  
  OtherAJ_lo_E->Write();
  OtherAJ_lo_T->Write();
  OtherAJ_lo_minmax->Write();

  cout << "Wrote to " << endl << out->GetName() << endl;
  return 0;
  
}



// ===========================================================================
TH1D* minmax ( vector<TFile*> files, TString which, int AuAuMultL, int AuAuMultR, TString namehelper,  vector<TH1D*>& histos ){
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

    h1->Rebin(2);
    // h1->Scale(1./h1->Integral());
    h1->Scale(1./h1->Integral(0, h1->GetNbinsX()+1)); // Include over/underflow

    histos.push_back( h1 );		      
  }
  
  TH1D* ret = histos.at(0)->Clone( which+"_minmax" );
  ret->Reset();

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
    
  }

  if ( gPad ){      
    histos.at(0)->SetTitle( "");
    histos.at(0)->SetAxisRange(  0.0, 0.23, "y");
    histos.at(0)->SetAxisRange(  0.0, 0.72, "x");
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

