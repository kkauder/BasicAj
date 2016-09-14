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


int PlotUnfoldedPpSystematics(){
  
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);
  gStyle->SetHistLineWidth(2);

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
            
    // leg = new TLegend( 0.55, 0.55, 0.89, 0.9, hGNS->GetTitle() );
    leg = new TLegend( 0.45 , 0.6, 0.89, 0.9, TString ("Corrected, ")+hNS0->GetTitle() );
    leg->SetBorderSize(0);    leg->SetLineWidth(10);
    leg->SetFillStyle(0);     leg->SetMargin(0.1);
    
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
    
    // legend etc.
    leg->AddEntry(hNS0,"p+p HT, Trigger Jet", "lp");
    leg->AddEntry(hAS0,"p+p HT, Away Jet", "lp");

    FUVQjet->Draw("9same");
    leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
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


