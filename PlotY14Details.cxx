int PlotY14Details() {
  gStyle->SetOptStat(0);

  TFile* f = new TFile ("AjResults/Analyze_Y14.root","READ");
  
  TH1D* nHT2_perRun = (TH1D*) f->Get( "nHT2_perRun" );
  TH1D* nHT3_perRun = (TH1D*) f->Get( "nHT3_perRun" );
  TH1D* nAccepted_perRun = (TH1D*) f->Get( "nAccepted_perRun");
  TH1D* nHT2_perDay = (TH1D*) f->Get( "nHT2_perDay" );
  TH1D* nHT3_perDay = (TH1D*) f->Get( "nHT3_perDay" );
  TH1D* nAccepted_perDay = (TH1D*) f->Get("nAccepted_perDay" );

  // new TCanvas;
  // nHT3_perDay->Draw();

  TObjArray toa;
  toa.Add( nHT2_perDay );
  toa.Add( nHT3_perDay );
  
  for ( int ih =0; ih < toa.GetEntries() ; ++ih ){    
    TH1D* h = (TH1D*) toa.At(ih);
    for ( int i=1; i<h->GetNbinsX() ; ++i ){
      int day = int ( h->GetXaxis()->GetBinCenter(i) + 0.01 );
      switch ( day ){
      case 76 :
	h->GetXaxis()->SetBinLabel(i, "17 Mar");
	break;
      case 90 :
	h->GetXaxis()->SetBinLabel(i, "31 Mar");
	break;
      case 104 :
	h->GetXaxis()->SetBinLabel(i, "14 Apr");
	break;
      case 118 :
	h->GetXaxis()->SetBinLabel(i, "28 Apr");
	break;
      case 132 :
	h->GetXaxis()->SetBinLabel(i, "12 May");
	break;
      case 146 :
	h->GetXaxis()->SetBinLabel(i, "26 May");
	break;
      case 160 :
	h->GetXaxis()->SetBinLabel(i, "09 Jun");
	break;
      case 174 :
	h->GetXaxis()->SetBinLabel(i, "23 Jun");
	break;
      default :
	// h->GetXaxis()->SetBinLabel(i, "");
	break;
      }
    }
    h->LabelsOption("h","x");
    h->SetLabelSize( 0.05);
    h->LabelsDeflate();	 
  }
  
  new TCanvas;
  gPad->SetTopMargin(0.13);
  TH1D* cHT3_perDay = nHT3_perDay->GetCumulative();
  cHT3_perDay->Draw();
  // http://www.star.bnl.gov/protected/common/common2014/trigger2014/lumiau200GeV/nev_perday_BHT3.png
  gPad->SaveAs("my_nev_perday_BHT3.png");
  
  new TCanvas;
  gPad->SetTopMargin(0.13);
  TH1D* cHT2_perDay = nHT2_perDay->GetCumulative();
  cHT2_perDay->Draw();
  // http://www.star.bnl.gov/protected/common/common2014/trigger2014/lumiau200GeV/nev_perday_BHT2*VPDMB-30.png
  gPad->SaveAs("my_nev_perday_BHT2.png");

  return 0;
}
