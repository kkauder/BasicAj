#include <vector>

#define REBIN 2

void CalcAj ( TFile* file, int AuAuMultL, int AuAuMultR, vector<double>& UnbinnedAj_lo, vector<double>& UnbinnedAj_hi, TString opt="fabs" );
double CalcAj ( TLorentzVector* j1, TLorentzVector* j2, bool nofabs=false );
TH1D* minmax ( vector<TFile*> files, TString which, int AuAuMultL, int AuAuMultR, TString namehelper,  vector<TH1D*>& histos, 
	       vector<TH1D*>& ratios, vector<TH1D*>& minandmax );

// MULTIPLICITY CUT IS IGNORED!!!
TH1D* treeminmax ( vector<TFile*> files, TString which, int AuAuMultL, int AuAuMultR, TString namehelper,  vector<TH1D*>& histos,
		   vector<TH1D*>& ratios, vector<TH1D*>& minandmax );

  
int GroomPrepGeantSystematics( TString R="0.4", int AuAuMultL=0, int AuAuMultR=-1  )
{

  // TString Base = "AjResults/AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
  TString Base = "AjResults/Above25_AEff0_PtRes0_ATow0_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root";
  TString outname = Base;
  outname.ReplaceAll("AjResults/","AjResults/TotalzgSystematics_");
  outname.ReplaceAll("AEff0_PtRes0_ATow0_","");

  TString fe0r0t0 = Base;

  TString s;
  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff-1_PtRes0_ATow0_");
  TString feMr0t0 = s;

  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes1_ATow0_");
  TString fe0rPt0 = s;


  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes0_ATow-1_");
  TString fe0r0tM = s;

  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes0_ATow1_");
  TString fe0r0tP = s;

  // Mixes
  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff-1_PtRes0_ATow1_");
  TString feMr0tP = s;

  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff-1_PtRes0_ATow-1_");
  TString feMr0tM = s;

  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff-1_PtRes-1_ATow1_");
  TString feMrMtP = s;

  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff-1_PtRes-1_ATow-1_");
  TString feMrMtM = s;
  
  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes1_ATow-1_");
  TString fe0rPtM = s;

  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes1_ATow1_");
  TString fe0rPtP = s;

  s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes-1_ATow-1_");
  TString fe0rMtM = s;


  if ( AuAuMultL==351  ){
    cerr << "Don't try bad AuAuMultL settings" << endl;
    return -1;
  } 
  
  vector<TFile*> Efiles;
  Efiles.push_back(new TFile( fe0r0t0, "READ" ) );
  Efiles.push_back(new TFile( feMr0t0, "READ" ) );

  bool UsePtMinMax=true;
  vector<TFile*> Rfiles;
  if ( UsePtMinMax ){
    // Option 1: Min/Max
    s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes-1_ATow0_");
    TString fe0rMt0 = s;
    s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes0_ATow1_");
    TString fe0r0tP = s;
    Rfiles.push_back(new TFile( fe0r0t0, "READ" ) );
    Rfiles.push_back(new TFile( fe0rMt0, "READ" ) );
    Rfiles.push_back(new TFile( fe0rPt0, "READ" ) );
  } else {
    // Option 2: Smear
    outname.ReplaceAll("TotalzgSystematics_","TotalzgSystematics_PtSmear_");
    
    s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_Syst","AEff0_PtRes10_ATow0_PtSmearSyst");
    TString fe0rSt0 = s;
    Rfiles.push_back(new TFile( fe0r0t0, "READ" ) );
    Rfiles.push_back(new TFile( fe0rSt0, "READ" ) );
  }
 
  vector<TFile*> Tfiles;
  Tfiles.push_back(new TFile( fe0r0t0, "READ" ) );
  Tfiles.push_back(new TFile( fe0r0tM, "READ" ) );
  Tfiles.push_back(new TFile( fe0r0tP, "READ" ) );
    
  vector<TFile*> ACfiles;
  ACfiles.push_back(new TFile( fe0r0t0, "READ" ) );
  // ACfiles.push_back(new TFile( feMr0tP, "READ" ) );
  ACfiles.push_back(new TFile( feMr0tP, "READ" ) );
  ACfiles.push_back(new TFile( feMr0tM, "READ" ) );
  ACfiles.push_back(new TFile( feMrMtP, "READ" ) );
  ACfiles.push_back(new TFile( feMrMtM, "READ" ) );
  ACfiles.push_back(new TFile( fe0rPtM, "READ" ) );
  ACfiles.push_back(new TFile( fe0rPtP, "READ" ) );
  ACfiles.push_back(new TFile( fe0rMtM, "READ" ) );

  vector<TH1D*> Ehistos;
  vector<TH1D*> Rhistos;
  vector<TH1D*> Thistos;
  vector<TH1D*> AChistos;
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
    
  TH1D* h_E=0;
  TH1D* h_R=0;
  TH1D* h_T=0;
  TH1D* h_AC=0;
  TString which;  

  // Open outputfile
  // ---------------
  TFile* out = new TFile( outname, "RECREATE");

  // Matched, high
  // -------------
  vector<string> whiches;
  whiches.push_back ( "zgLead1015Lo" );
  whiches.push_back ( "zgSubLead1015Lo" );
  whiches.push_back ( "zgLead1520Lo" );
  whiches.push_back ( "zgSubLead1520Lo" );
  whiches.push_back ( "zgLead2025Lo" );
  whiches.push_back ( "zgSubLead2025Lo" );
  whiches.push_back ( "zgLead2530Lo" );
  whiches.push_back ( "zgSubLead2530Lo" );
  whiches.push_back ( "zgLead3040Lo" );
  whiches.push_back ( "zgSubLead3040Lo" );

  // whiches.push_back ( "zgLead1020Lo" );
  // whiches.push_back ( "zgLead2030Lo" );


  // whiches.push_back ( "zgLead2030Hi" );
  // whiches.push_back ( "zgLead2030Lo" );
  // whiches.push_back ( "zgLead3040Hi" );
  // whiches.push_back ( "zgLead3040Lo" );
  // whiches.push_back ( "zgLead4060Hi" );
  // whiches.push_back ( "zgLead4060Lo" );

  // whiches.push_back ( "zgSubLead1020Hi" );
  // whiches.push_back ( "zgSubLead1020Lo" );
  // whiches.push_back ( "zgSubLead2030Hi" );
  // whiches.push_back ( "zgSubLead2030Lo" );
  // whiches.push_back ( "zgSubLead3040Hi" );
  // whiches.push_back ( "zgSubLead3040Lo" );
  
  for (int w = 0; w<whiches.size(); ++w ){
    which = whiches.at(w);
    cout << which << endl;

    // h_E = minmax (  Efiles, which, AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
    h_E = treeminmax (  Efiles, which, AuAuMultL, AuAuMultR, "E", Ehistos, ratios, minandmax );
    h_E->SetName( which + "_E" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_E->GetName() + ".png";
      gPad->SaveAs( plotname );
    }
    
    // h_R = minmax (  Rfiles, which, AuAuMultL, AuAuMultR, "R", Rhistos, ratios, minandmax );
    h_R = treeminmax (  Rfiles, which, AuAuMultL, AuAuMultR, "R", Rhistos, ratios, minandmax );
    h_R->SetName( which + "_R" );
    if (gPad){
      if ( UsePtMinMax )       plotname = "plots/" + R + "_" + h_R->GetName() + ".png";
      else                     plotname = "plots/" + R + "_PtSmear_" + h_R->GetName() + ".png";
      gPad->SaveAs( plotname );
    }
    
    // h_T = minmax (  Tfiles, which, AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
    h_T = treeminmax (  Tfiles, which, AuAuMultL, AuAuMultR, "T", Thistos, ratios, minandmax );
    h_T->SetName( which + "_T" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_T->GetName() + ".png";
      gPad->SaveAs( plotname );
    }

    // Anti-Correlated
    // Currently NOT considered for overall
    h_AC = treeminmax (  ACfiles, which, AuAuMultL, AuAuMultR, "AC", AChistos, ratios, minandmax );
    h_AC->SetName( which + "_AC" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_AC->GetName() + ".png";
      gPad->SaveAs( plotname );
    }

    TH1D* h_minmax = h_T->Clone(which + "_minmax");
    TH1D* h_min = h_T->Clone(which + "_min");
    TH1D* h_max = h_T->Clone(which + "_max");
    
    TH1D* h_minmax_ratio = h_T->Clone(which + "_minmax_ratio");
    h_minmax_ratio->Reset();
    
    for (int i=1; i<=h_minmax->GetNbinsX() ; ++ i ){
      h_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );
      
      h_minmax->SetBinError (i, sqrt( pow( h_E->GetBinError(i), 2) +
				      pow( h_R->GetBinError(i), 2) + 
			              pow( h_T->GetBinError(i), 2)  )
			     );

      if ( h_minmax->GetBinError(i)>0 ){
	h_minmax_ratio->SetBinContent (i, h_minmax->GetBinError(i) / h_minmax->GetBinContent(i) );
      }
	    
      
      h_min->SetBinContent (i, Thistos.at(0)->GetBinContent(i) - h_minmax->GetBinError (i) );
      h_min->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
      
      h_max->SetBinContent (i, Thistos.at(0)->GetBinContent(i) + h_minmax->GetBinError (i) );
      h_max->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
      
    }

    s = Thistos.at(0)->GetName();
    s.ReplaceAll( "T0","nom");
    TH1D* h_nom=Thistos.at(0)->Clone( s );
    h_minmax->SetMarkerSize(0);
    h_minmax->SetLineWidth( 0 );
    h_minmax->SetFillStyle(1001);
    h_minmax->SetFillColor( kGray );
    h_minmax->SetMarkerColor( kGray );

    h_nom->Draw("9");
    h_minmax->Draw("9E2same");
    // h_minmax->Draw("9E2");
    h_nom->Draw("9same");
    if ( UsePtMinMax )       plotname = "plots/" + R + "_" + h_minmax->GetName() + ".png";
    else                     plotname = "plots/" + R + "_PtSmear_" + h_minmax->GetName() + ".png";
    gPad->SaveAs( plotname );

    h_minmax_ratio->Draw("9");
    if ( UsePtMinMax )       plotname = "plots/" + R + "_" + h_minmax_ratio->GetName() + ".png";
    else                     plotname = "plots/" + R + "_PtSmear_" + h_minmax_ratio->GetName() + ".png";

    
    gPad->SaveAs( plotname );



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
  if ( files.size()==3 ){
    TH1D* ratio2 = histos.at(2)->Clone( TString(histos.at(2)->GetName()) + "_ratio");
    ratio2->GetYaxis()->SetTitle("ratio to nominal");
    ratio2->Divide( nom);
    ratios.push_back(ratio2);
  }


  float ymax = 0.12*REBIN;
  if ( gPad ){      
    // histos.at(0)->SetTitle( "");
    // histos.at(0)->SetAxisRange(  0.0, 0.23, "y");
    // histos.at(0)->SetAxisRange(  0.0, 0.72, "x");
    histos.at(0)->SetAxisRange( 0.0, ymax, "y");
    histos.at(0)->SetAxisRange(  0.05, 0.5, "x");
    histos.at(0)->Draw();

    histos.at(1)->SetLineWidth(2);
    histos.at(1)->SetLineColor(kBlue);
    histos.at(1)->Draw("same");
    
    if ( files.size()==3 ){
      histos.at(2)->SetLineWidth(2);
      histos.at(2)->SetLineColor(kMagenta+1);
      histos.at(2)->Draw("same");
    }
    
    ret->SetFillStyle(3001 );
    ret->SetFillColor( kGray+1 );  
    
    ret->Draw("9E2same");

    TLegend* leg = new TLegend ( 0.6, 0.6, 0.88, 0.88 );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    // leg->SetMargin(0.1);

    leg->AddEntry(histos.at(0)->GetName(), "Nominal", "l");
    if ( namehelper.Contains("T") ){
      leg->AddEntry(histos.at(1)->GetName(), "Tow Scale Max", "l");
      leg->AddEntry(histos.at(2)->GetName(), "Tow Scale Min", "l");
    } else if ( namehelper.Contains("R") ){
      if ( histos.size() == 3 ){
	leg->AddEntry(histos.at(1)->GetName(), "Track Res Max", "l");
	leg->AddEntry(histos.at(2)->GetName(), "Track Res Min", "l");
      } else {
	leg->AddEntry(histos.at(1)->GetName(), "Track Res Smeared", "l");
      }
    } else {
      leg->AddEntry(histos.at(1)->GetName(), "Track Eff Min", "l");
    }
    
    leg->AddEntry(ret->GetName(), "Uncertainty", "f");
    leg->Draw("same");
    // cout << histos.at(0)->GetName() << endl;
  }
  return ret;

}

// ===========================================================================
// MULTIPLICITY CUT IS IGNORED!!!
TH1D* treeminmax ( vector<TFile*> files, TString which, int AuAuMultL, int AuAuMultR, TString namehelper,  vector<TH1D*>& histos,
		   vector<TH1D*>& ratios, vector<TH1D*>& minandmax ){

  histos.clear();
  
  TFile* file;
  TTree* tree;
  TH2D* h2;
  TH1D* h1;
  
  TRegexp re = "[0-9]+";
  Ssiz_t ext;

  Ssiz_t start=which.Index(re,&ext);
  if  ( ext!=4 ) {
    cerr << "Incompatible name " << which << endl;
    throw(-1);
  }
  cout << which(start,2) << "  " << which(start+2,2) << endl;
  TString sptmin = which(start,2);
  TString sptmax = which(start+2,2);

  TString cond;
  TString dopt;
  TString title;
  if ( which.Contains("SubLead") ){
    title = "SubLeading ";
    if ( which.Contains("Lo") ) {
      title += "Lo ";
      cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 ";
      cond += "&& AwayJetLo.Pt()>" + sptmin;
      cond += "&& AwayJetLo.Pt()<" + sptmax;    
      dopt  = "zgawaylo";
    } else {
      title += "Hi ";
      cond  = "weight*( abs(TriggerJet.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 ";
      cond += "&& AwayJet.Pt()>" + sptmin;
      cond += "&& AwayJet.Pt()<" + sptmax;
      dopt  = "zgawayhi";
    }
  } else { // Leading Jet
    title = "Leading ";
    if ( which.Contains("Lo") ) {
      title += "Lo ";
      cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 ";
      cond += "&& TriggerJetLo.Pt()>" + sptmin;
      cond += "&& TriggerJetLo.Pt()<" + sptmax;    
      dopt  = "zgtriglo";
    } else {
      title += "Hi ";
      cond  = "weight*( abs(TriggerJet.Eta())<0.6 ";
      cond += "&& TriggerJet.Pt()>" + sptmin;
      cond += "&& TriggerJet.Pt()<" + sptmax;
      dopt  = "zgtrighi";
    }      
  }
  
  title += " Jet with " + sptmin + "<p_{T}<" + sptmax + " GeV/c";
  cond += " ) ";
  dopt += " >> ";

  
  int AuAuMultBinL;
  int AuAuMultBinR;
  for ( int f=0; f<files.size() ; ++f ){
    file = files.at(f);
    tree = (TTree*) file->Get("TriggeredTree");
    h2 = ((TH2D*) file->Get("zgLead2030Lo")); // Just a helper for dimensions      

    AuAuMultBinL = 1;
    AuAuMultBinR = h2->GetNbinsY();
        
    // if ( AuAuMultL > 0 ) AuAuMultBinL = h2->GetYaxis()->FindBin( AuAuMultL );
    // if ( AuAuMultR > 0 ) AuAuMultBinR = h2->GetYaxis()->FindBin( AuAuMultR+1 );    

    TString newname = which;
    newname+="_"; newname+=namehelper; newname+=f;
    h1 = (TH1D*) ( h2->ProjectionX( newname, AuAuMultBinL, AuAuMultBinR )) ;
    h1->SetTitle(title);
    h1->Reset();
    h1->Rebin(REBIN);
    
    cout << dopt + h1->GetName() << endl;
    cout << cond << endl;
    // h1->Draw();
    // return 0;

    tree->Draw( dopt + h1->GetName()  , cond , "goff" );

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

    if ( histos.size()==2 ){
      // Symmetrize if we only have one different histo
      float v  = histos.at(0)->GetBinContent(i);
      float dv = fabs ( histos.at(1)->GetBinContent(i) - v );
      max = v+dv;
      min = v-dv;
    } else {
      for ( int j=0; j<histos.size() ; ++j ){
	if ( histos.at(j)->GetBinContent(i) < min ) min = histos.at(j)->GetBinContent(i);
	if ( histos.at(j)->GetBinContent(i) > max ) max = histos.at(j)->GetBinContent(i);
      }
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
  
  if ( histos.size()==3 ){
    TH1D* ratio2 = histos.at(2)->Clone( TString(histos.at(2)->GetName()) + "_ratio");
    ratio2->GetYaxis()->SetTitle("ratio to nominal");
    ratio2->Divide( nom);
    ratios.push_back(ratio2);
  }


  float ymax = 0.12*REBIN;
  if ( gPad ){      
    // histos.at(0)->SetTitle( "");
    // histos.at(0)->SetAxisRange(  0.0, 0.23, "y");
    // histos.at(0)->SetAxisRange(  0.0, 0.72, "x");
    // histos.at(0)->SetAxisRange(  0.0, 0.25, "y");
    // histos.at(0)->SetAxisRange(  -0.3, 0.8, "x");
    histos.at(0)->SetAxisRange( 0.0, ymax, "y");
    histos.at(0)->SetAxisRange(  0.05, 0.5, "x");
    histos.at(0)->Draw();

    histos.at(1)->SetLineWidth(2);
    histos.at(1)->SetLineColor(kBlue);
    histos.at(1)->Draw("same");

    if ( histos.size()>=3 ){
      histos.at(2)->SetLineWidth(2);
      histos.at(2)->SetLineColor(kMagenta+1);
      histos.at(2)->Draw("same");
    }
    
    for ( i=3; i<histos.size(); ++i ){
      histos.at(i)->SetLineWidth(2);
      histos.at(i)->SetLineColor(40+i);
      histos.at(i)->Draw("same");
    }

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
    } else if ( namehelper.Contains("R") ){
      if ( histos.size() == 3 ){
	leg->AddEntry(histos.at(1)->GetName(), "Track Res Max", "l");
	leg->AddEntry(histos.at(2)->GetName(), "Track Res Min", "l");
      } else {
	leg->AddEntry(histos.at(1)->GetName(), "Track Res Smeared", "l");
      }
    } else if ( namehelper.Contains("AC") ){
      leg->AddEntry(histos.at(1)->GetName(), "Track Min #oplus Tow Max", "l");
      // if ( histos.size() == 3 )     leg->AddEntry(histos.at(2)->GetName(), "Track Min #oplus Res. Min #oplus Tow Max ", "l");
      if ( histos.size() == 3 )     leg->AddEntry(histos.at(2)->GetName(), "Res. Max #oplus Tow Min ", "l");     
    } else {
      leg->AddEntry(histos.at(1)->GetName(), "Track Eff Min", "l");
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
    
    
