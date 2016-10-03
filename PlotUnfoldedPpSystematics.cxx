/* FIRST RUN:
root -l -b -q NewBinByBinUnfolding.cxx'(0)'
root -l -b -q NewBinByBinUnfolding.cxx'(-1)'
root -l -b -q NewBinByBinUnfolding.cxx'(1)'
*/

// canvas stuff
float lm = 0.11;
float bm = 0.11;
float yto = 0.5;
float xto = 0.5;

// =============================================================================
TH2D* SetupZgHisto2D ( double ptbins[], int nptbins, 
		       int nbinsx, float xleft, float xright,
		       TString namebase="h" );

void Fill ( TH2D* h2, TString FileName, TString TreeName, TString dopt, TString cond );

TObjArray* ChopNormPrettify ( TH2D* h2, TString titlebase="p_{T}=", 
			      TString xlabel = "", TString ylabel = "",
			      int rebin = 1, TString norm = "",
			      float ymin = -1e6, float ymax = -1e7 );

// =============================================================================

int PlotUnfoldedPpSystematics(){
  
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);
  gStyle->SetHistLineWidth(2);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.045);
  // latex.SetTextColor(kGray+3);
  latex.SetTextColor(kRed+3);

  bool UseRelErr=false;
  bool UseFixedRelErr = true;
  float FixedRelErr = 0; 

  int RebinZg=2;
  float zgmin   = 0;
  float zgmax   = 8.5;

  if ( UseFixedRelErr ){
    if ( RebinZg == 2 ) 
      FixedRelErr = 0.08; // true for rebinning by 2, ignoring one outlier
    else if ( RebinZg == 1 ) 
      FixedRelErr = 0.11; // true for no rebinning, ignoring two outliers
    else {
      cerr << "Please reasses FixedRelErr for this rebinning." << endl;
      return -1;	
    }
  }
	
  
  TString plotpath = "./HPplots/";

  // Prep Theory curve
  // -----------------
  TF1* PbarQjet = new TF1("PbarQjet","1./[0] * 4./3. * ( (1+pow(x,2))/(1-x) + (1 + pow(1-x,2))/x )", 0.1, 0.5);
  PbarQjet->SetParameter( 0,4.2593);
  PbarQjet->SetLineColor(kGreen);
  
  TF1* FUVQjet = new TF1("FUVQjet", "[0]*(PbarQjet)", 0.1,0.5);
  FUVQjet->SetParameter( 0,1);
  FUVQjet->SetLineColor(kGray+2);
  FUVQjet->SetLineWidth(1);
  FUVQjet->SetLineStyle(2);
  FUVQjet->SetParameter( 0,1);

  // Load and rebin
  // --------------
  TFile* f0 = new TFile("HThistos/BinByBinUnfolding_HTGeant.root","READ");
  TFile* fP = new TFile("HThistos/BinByBinUnfolding_HTGeant_PtScale1.root","READ");
  TFile* fM = new TFile("HThistos/BinByBinUnfolding_HTGeant_PtScale-1.root","READ");
  
  TObjArray NS0; 
  TObjArray AS0; 
  LoadAndPrettify ( NS0, AS0, f0, "", RebinZg, zgmin, zgmax );

  TObjArray NSM; 
  TObjArray ASM; 
  LoadAndPrettify ( NSM, ASM, fM, "M_", RebinZg, zgmin, zgmax );
  
  TObjArray NSP; 
  TObjArray ASP; 
  LoadAndPrettify ( NSP, ASP, fP, "P_", RebinZg, zgmin, zgmax );

  // Reference

  // PYTHIA8 -- fully decayed
  // ------------------------
  int nzgBins=40;
  int McRebinZg=4;
  float zgleft  = 0.05;
  float zgright = 0.55;
  float zgmin   = 0;
  float zgmax   = 8.5;
  TString dopt;
  TString cond;
  bool UseAbove25 = true;
  double IncNSbins[] = { 10, 15, 20, 25, 30, 40, 60};
  int nIncNSbins  = sizeof(IncNSbins) / sizeof(IncNSbins[0])-1;

  double IncASbins[] = { 10, 15, 20, 25, 30, 40, 60};
  int nIncASbins  = sizeof(IncASbins) / sizeof(IncASbins[0])-1;

  TString sP8_SoftJets_MB = "HThistos/TrueMB_LargeEtaPythiaOnly.root";
  if (UseAbove25){
    TString sP8_SoftJets_MB = "HThistos/Above25_TrueMB_LargeEtaPythiaOnly.root";
  }

  //    -----------------------------------------
  //    -------- PYTHIA8 MB Soft Jets  --------
  //    -----------------------------------------
  TH2D* h2_NS_P8_SoftJets_MB=0;
  TObjArray* NS_P8_SoftJets_MB=0;
  TH2D* h2_AS_P8_SoftJets_MB=0;
  TObjArray* AS_P8_SoftJets_MB=0;

  // NS
  // --
  h2_NS_P8_SoftJets_MB = SetupZgHisto2D ( IncNSbins, nIncNSbins, nzgBins, zgleft, zgright, "NS_P8_SoftJets_MB");

  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 )";
  dopt  = "zgtriglo:TriggerJetLo.Pt() >> ";
  Fill ( h2_NS_P8_SoftJets_MB, sP8_SoftJets_MB, "TriggeredTree", dopt + h2_NS_P8_SoftJets_MB->GetName(), cond );
  NS_P8_SoftJets_MB = ChopNormPrettify ( h2_NS_P8_SoftJets_MB, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   McRebinZg, "ZG", zgmin, zgmax);
    
  // AS
  // --
  h2_AS_P8_SoftJets_MB = SetupZgHisto2D ( IncASbins, nIncASbins, nzgBins, zgleft, zgright, "AS_P8_SoftJets_MB");
  cond  = "weight*( abs(TriggerJetLo.Eta())<0.6 && abs(AwayJetLo.Eta())<0.6 )";
  dopt  = "zgawaylo:AwayJetLo.Pt() >> ";
  Fill ( h2_AS_P8_SoftJets_MB, sP8_SoftJets_MB, "TriggeredTree", dopt + h2_AS_P8_SoftJets_MB->GetName(), cond );
  AS_P8_SoftJets_MB = ChopNormPrettify ( h2_AS_P8_SoftJets_MB, "p_{T}=", "z_{g}", "1/N dN/dz_{g}",
					   McRebinZg, "ZG", zgmin, zgmax);
  

  // ==========================================================================================
  // -------------------------
  // --------- Draw ----------
  // -------------------------

  for ( int i=0; i<NS0.GetEntries()-1 ; ++i ){
    // cout << NSP.At(i)->GetName() << endl;
    TH1D* hNS0 = (TH1D*) NS0.At(i);
    TH1D* hNSP = (TH1D*) NSP.At(i);
    TH1D* hNSM = (TH1D*) NSM.At(i);
    
    TH1D* relerrNS;
    TH1D* minmaxNS = 0;
    TH1D* minmaxAS = 0;
    
    TH1D* hAS0 = (TH1D*) AS0.At(i);
    TH1D* hASP = (TH1D*) ASP.At(i);
    TH1D* hASM = (TH1D*) ASM.At(i);
    
    TH1D* relerrAS;
    
    TH1D* h8NS = (TH1D*)NS_P8_SoftJets_MB->At(i);
    TH1D* h8AS = (TH1D*)AS_P8_SoftJets_MB->At(i);      
    
    h8NS->SetAxisRange(0.11, 0.49);
    h8AS->SetAxisRange(0.11, 0.49);
    h8NS->SetLineColor(kRed);
    h8NS->SetLineStyle(2);
    // h8NS->SetMarkerColor(kRed);
    // h8NS->SetMarkerStyle(22);

    h8AS->SetLineColor(kBlack);
    h8AS->SetLineStyle(2);
    // h8AS->SetMarkerColor(kMagenta+1);
    // h8AS->SetMarkerStyle(26);
      
    if ( !UseFixedRelErr ){
      minmaxNS = minmax ( hNS0, hNSM, hNSP, relerrNS, UseRelErr );
      minmaxAS = minmax ( hAS0, hASM, hASP, relerrAS, UseRelErr );
    } else {
      minmaxNS = (TH1D*) hNS0->Clone( TString(hNS0->GetName()) +"_minmax" );
      minmaxAS = (TH1D*) hAS0->Clone( TString(hAS0->GetName()) +"_minmax" );      
      
      for (int j=1; j<=minmaxNS->GetNbinsX() ; ++j ){
	float v0 = hNS0->GetBinContent(j);	
	if ( fabs(v0)<1e-6) continue;      
	minmaxNS->SetBinContent( j, v0 );
	minmaxNS->SetBinError( j, FixedRelErr*v0 );   
      }
      for (int j=1; j<=minmaxAS->GetNbinsX() ; ++j ){
	float v0 = hAS0->GetBinContent(j);	
	if ( fabs(v0)<1e-6) continue;      
	minmaxAS->SetBinContent( j, v0 );
	minmaxAS->SetBinError( j, FixedRelErr*v0 );   
      }
    }

    // new TCanvas;
    // gPad->SetLeftMargin( lm );// for bigger labels
    // gPad->SetBottomMargin( bm );// for bigger labels

    // relerrNS->Draw();
    // relerrAS->Draw("same");

    new TCanvas;
    gPad->SetLeftMargin( lm );// for bigger labels
    gPad->SetBottomMargin( bm );// for bigger labels
    gPad->SetGridx(0);  gPad->SetGridy(0);


    TH1D* dummy = (TH1D*)hNS0->Clone("dummy");
    dummy->Reset();    dummy->SetTitle("");
    dummy->SetLineColor(kBlack);
    dummy->SetAxisRange(zgmin, zgmax, "y");
    dummy->Draw();
            
    if (i<4)    leg = new TLegend( 0.45 , 0.6, 0.89, 0.9, hNS0->GetTitle() );
    else        leg = new TLegend( 0.45 , 0.7, 0.89, 0.9, hNS0->GetTitle() );
    leg->SetBorderSize(0);    leg->SetLineWidth(10);
    leg->SetFillStyle(0);     leg->SetMargin(0.1);
    
    if (i<4){
      // AS
      minmaxAS->SetMarkerSize(0);
      minmaxAS->SetLineWidth( 0 );
      minmaxAS->SetFillStyle(1001);
      minmaxAS->SetFillColor( kGray );
      minmaxAS->SetMarkerColor( kGray );
      minmaxAS->Draw("9E2same");
      
      // hASP->SetLineColor(kBlue);
      // hASP->DrawCopy("9histsame");
      // hASM->SetLineColor(kGreen+1);
      // hASM->DrawCopy("9histsame");
      
      hAS0->SetLineColor(kBlack);
      hAS0->SetMarkerColor(kBlack);
      hAS0->SetMarkerStyle(30);
      hAS0->SetMarkerSize(2);
      hAS0->DrawCopy("9same");
    }

    // NS
    minmaxNS->SetMarkerSize(0);
    minmaxNS->SetLineWidth( 0 );
    minmaxNS->SetFillStyle(1001);
    minmaxNS->SetFillColor( kRed-10 );
    minmaxNS->SetMarkerColor( kRed-10 );
    minmaxNS->Draw("9E2same");

    hNS0->SetLineColor(kRed);
    hNS0->SetMarkerColor(kRed);
    hNS0->SetMarkerStyle(29);
    hNS0->SetMarkerSize(2);
    hNS0->DrawCopy("9same");

    // hNSP->SetLineColor(kBlue);
    // hNSP->DrawCopy("9histsame");
    // hNSM->SetLineColor(kGreen+1);
    // hNSM->DrawCopy("9histsame");
    
    if (i<4)   h8AS->DrawCopy("9lhist][same");
    h8NS->DrawCopy("9lhist][same");      
    // h8AS->DrawCopy("9lsame");
    // h8NS->DrawCopy("9lsame");      


    // legend etc.
    leg->AddEntry(hNS0,"p+p HT, Trigger Jet", "lp");
    leg->AddEntry(h8NS,"Pythia8, Leading Jet", "l");
    if (i<4){
      leg->AddEntry(hAS0,"p+p HT, Recoil Jet", "lp");
      leg->AddEntry(h8AS,"Pythia8, Recoil Jet", "l");
    }

    // FUVQjet->Draw("9same");
    // leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
    leg->Draw();
    
    latex.DrawLatex( 0.55, 0.54, "STAR Preliminary");


    // hNSP->SetLineColor(kRed);
    // hNSP->DrawCopy("9histsame");
    // hNSM->SetLineColor(kGreen);
    // hNSM->DrawCopy("9histsame");
    
    // new TCanvas;
    // gPad->SetLeftMargin( lm );// for bigger labels
    // gPad->SetBottomMargin( bm );// for bigger labels

    // relerrNS->Draw();
    // return 0;
    // hAS->SetLineColor(kGray+1);
    // hAS->SetMarkerColor(kGray+1);
    // hAS->SetMarkerStyle(30);
    // hAS->DrawCopy("9same");

    TString plotname = hNS0->GetName();    
    plotname = plotname(plotname.Length()-5, plotname.Length());
    
    plotname = plotpath+"UnfoldedPpSystematics_HTGeant_"+plotname;
    gPad->SaveAs( plotname+".png");


  }
  
  return 0;
}
// =============================================================================
void LoadAndPrettify ( TObjArray& retNS, TObjArray& retAS, TFile* f, TString titlebase,
		       int rebin, float ymin, float ymax )
{
  retNS.Clear();
  retAS.Clear();
  
  TIter next(f->GetListOfKeys());
  TKey *key;
  
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1")) continue;
    TH1D *h = (TH1D*)key->ReadObj();
    if ( TString(h->GetName()).Contains("UnfoldedNS")) retNS.Add(h);
    else if ( TString(h->GetName()).Contains("UnfoldedAS")) retAS.Add(h);
    else continue;

    // Unique name
    h->SetName( titlebase+h->GetName() );

    // Rebin?
    if (rebin>1) {
      h->Rebin(rebin);
      h->Scale(1./rebin);
    }

    // Further prettification here
    // y-axis?
    if (ymin<ymax && ymin >-1e5 ){
      h->SetAxisRange( ymin, ymax, "y" );
    }

    h->GetXaxis()->SetTitleSize(0.07);
    h->GetYaxis()->SetTitleSize(0.07);
    h->GetXaxis()->SetTitleOffset(xto);
    h->GetYaxis()->SetTitleOffset(yto);
    

  }

  return;
}


// =============================================================================
TH1D* minmax ( TH1D* h0, TH1D* hM, TH1D* hP, TH1D*& retrelerr, bool UseRelError=false){
  
  TH1D* ret = (TH1D*) h0->Clone( TString(h0->GetName()) +"_minmax" );
  retrelerr = (TH1D*) h0->Clone( TString(h0->GetName()) +"_relerr" );

  double dmax;
  for (int i=1; i<=ret->GetNbinsX() ; ++ i ){
    float v0 = h0->GetBinContent(i);
    if ( fabs(v0)<1e-6) continue;

    dmax = TMath:: Max ( fabs( hM->GetBinContent(i)-v0), fabs( hP->GetBinContent(i)-v0) );
    // cout << dmax/v0 << endl;

    ret->SetBinContent( i, v0 );
    ret->SetBinError( i, dmax );   
    retrelerr->SetBinContent( i, dmax/v0 );
    retrelerr->SetBinError( i, 0 );   
  }
  retrelerr->SetAxisRange( -0.01, 0.3, "y");
  
  if ( UseRelError ){
    float maxrel = retrelerr->GetBinContent(retrelerr->GetMaximumBin());
    
    for (int i=1; i<=ret->GetNbinsX() ; ++ i ){
      float v0 = h0->GetBinContent(i);
      if ( fabs(v0)<1e-6) continue;

      ret->SetBinContent( i, v0 );
      ret->SetBinError( i, maxrel*v0 );   
    }
  }
    
  // new TCanvas;
  // gPad->SetLeftMargin( lm );// for bigger labels
  // gPad->SetBottomMargin( bm );// for bigger labels
  // retrelerr->Draw();

  return ret;
}


// =============================================================================

void Fill ( TH2D* h2, TString FileName, TString TreeName, TString dopt, TString cond ){
  TChain* T = new TChain ( TreeName );
  T->Add( FileName );
  T->Draw( dopt , cond , "goff" );
  return;
}
// =============================================================================
TObjArray* ChopNormPrettify ( TH2D* h2, TString titlebase, TString xlabel, TString ylabel,
			      int rebin, TString norm, float ymin, float ymax )
{
  TObjArray* ret = new TObjArray();
  // ret->SetOwner(kTRUE);
  
  for (int i=1; i<=h2->GetNbinsX(); ++i ){
    int vbinl = int ( h2->GetXaxis()->GetBinLowEdge( i ) + 0.001 );
    int vbinr = int ( h2->GetXaxis()->GetBinLowEdge( i+1 ) + 0.001 );
    // cout << i << "  " << ptbins[i] << "-" << ptbins[i+1] << endl;
    TString name = h2->GetName();
    name += "_"; name += vbinl;
    name += "_"; name += vbinr;
    TString title = titlebase;
    title += ""; title += vbinl;
    title += "-"; title += vbinr;
    title += " GeV/c";
    title += ";"+xlabel+";"+ylabel;
    TH1D* h = (TH1D*) h2->ProjectionY( name, i, i, "nufnof" );
    h->SetTitle( title );

    // Rebin?
    if (rebin>1) h->Rebin(rebin);
    
    // Normalize?
    // options are to normalize 
    // -- integral to 1 (zg style)
    // -- sum to 1 (Aj style)
    norm.ToUpper();
    if ( norm=="INT" || norm=="ZG" ) h->Scale ( 1./ h->Integral(1, h->GetNbinsX()) / h->GetXaxis()->GetBinWidth(1));
    if ( norm=="SUM" || norm=="AJ" ) h->Scale ( 1./ h->Integral(0, h->GetNbinsX()+1) ); // In this case, incluse over/underflow
    
    // Further prettification here
    // y-axis?
    if (ymin<ymax && ymin >-1e5 ){
      h->SetAxisRange( ymin, ymax, "y" );
    }

    ret->Add(h);

  }

  return ret;
}

// =============================================================================
TH2D* SetupZgHisto2D ( double ptbins[], int nptbins, 
		       int nbinsx, float xleft, float xright,
		       TString namebase )
{
  TH2D* h2 = new TH2D( namebase, "All p_{T} bins", nptbins, ptbins,
		      nbinsx, xleft, xright);

  // for (int i=0; i<nptbins; ++i ){
  //   // cout << i << "  " << ptbins[i] << "-" << ptbins[i+1] << endl;
  //   TString name = namebase;
  //   name += "_"; name += ptbins[i];
  //   name += "_"; name += ptbins[i+1];
  //   TString title = titlebase;
  //   title += ""; title += ptbins[i];
  //   title += "-"; title += ptbins[i+1];
  //   title += " GeV/c";
  //   title += ";"+xlabel+";"+ylabel;
  //   TH1D* h = (TH1D*) h2->ProjectionY( name, i+1, i+2 );
  //   h->SetTitle( title );

  //   // Further prettification here
  //   ret->Add(h);

  //   // new TCanvas;
  //   // h->Draw();
  // }

  return h2;
}

// =============================================================================
