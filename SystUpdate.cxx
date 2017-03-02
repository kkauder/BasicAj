#include <string>
#include <vector>
using namespace std;

int SystUpdate()
{

  TString plotpath="./HPplots/";
  // canvas stuff
  float lm = 0.11;
  float bm = 0.11;
  float yto = 0.7;
  float xto = 0.5;
  TLegend* leg;
  
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);
  gStyle->SetHistLineWidth(3);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.045);
  // latex.SetTextColor(kGray+3);
  latex.SetTextColor(kRed+3);

  TFile * fRelErrors     = new TFile ("AjResults/RelErr_UnfoldedPpSystematics_Histos.root","READ");
  TFile * fDefault       = new TFile ("AjResults/FixedRelErr_UnfoldedPpSystematics_Histos.root","READ");
  TFile * fNew           = new TFile ("AjResults/TotalzgSystematics_Above25_SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root","READ");
  TFile * fBinByBinRatio = new TFile("HThistos/BinByBinUnfolding_HTGeant.root", "READ");
  
  // Plot all relative errors
  // ------------------------
  vector<TH1D*> RatiosToPlot;

  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedNS_10_15_relerr") );
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedNS_15_20_relerr") );
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedNS_20_25_relerr") );
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedNS_25_30_relerr") );
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedNS_30_40_relerr") );
  
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedAS_10_15_relerr") );
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedAS_15_20_relerr") );
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedAS_20_25_relerr") );
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedAS_25_30_relerr") );
  RatiosToPlot.push_back( (TH1D*) fRelErrors->Get("UnfoldedAS_30_40_relerr") );

  new TCanvas;
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels

  TH1D* axisdummy=0;
  for (int i=0; i<RatiosToPlot.size(); ++i ){
    TH1D* h=RatiosToPlot.at(i);
    h->SetAxisRange(0,0.25,"y");
    h->SetTitle(";z_{g};rel. uncertainty");
    h->GetXaxis()->SetTitleSize(0.07);
    h->GetYaxis()->SetTitleSize(0.07);
    h->GetXaxis()->SetTitleOffset(xto);
    h->GetYaxis()->SetTitleOffset(yto);
    h->SetLineColor(40+i);
    h->SetFillColor(40+i);

    h->SetBinContent(1,-10);
    h->SetBinContent(10,-10);
    if ( i==0 ) {
      axisdummy=(TH1D*) h->Clone("axisdummy");axisdummy->Clear();
      axisdummy->DrawCopy("AXIS");
    }
    h->SetAxisRange(0.11,0.49,"x");
    h->DrawCopy("9same");
  }
	 
  for (int i=0; i<RatiosToPlot.size(); ++i ){
    TH1D* h=RatiosToPlot.at(i);
    h->SetFillColor(0);
    h->Draw("9same");
  }
  gPad->SaveAs(plotpath+"OrigSystUncRatio.png");
  gPad->SaveAs(plotpath+"OrigSystUncRatio.pdf");

  // Demonstrate
  // -----------
  TString pivot   = "UnfoldedAS_20_25";
  TH1D* origsyst  = (TH1D*) fRelErrors->Get( pivot+"_minmax" )->Clone("origsyst");
  TH1D* origstat  = (TH1D*) fRelErrors->Get( pivot )->Clone("origstat");
  TH1D* fixedsyst = (TH1D*) fDefault->Get( pivot+"_minmax" )->Clone("fixedsyst");
  
  for (int i=0; i<origsyst->GetNbinsX(); ++i ){
    origstat->SetBinError(i, origstat->GetBinError(i) / origstat->GetBinContent(i) );
    origstat->SetBinContent(i, 1 );
    origsyst->SetBinError(i, origsyst->GetBinError(i) / origsyst->GetBinContent(i) );
    origsyst->SetBinContent(i, 1 );
    fixedsyst->SetBinError(i, fixedsyst->GetBinError(i) / fixedsyst->GetBinContent(i) );
    fixedsyst->SetBinContent(i, 1 );
  }
  
  new TCanvas;
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels

  origstat->SetTitle(";z_{g};rel. uncertainty");
  origstat->GetXaxis()->SetTitleSize(0.07);
  origstat->GetYaxis()->SetTitleSize(0.07);
  origstat->GetXaxis()->SetTitleOffset(xto);
  origstat->GetYaxis()->SetTitleOffset(yto);

  axisdummy->SetAxisRange(0.8,1.35,"y");
  axisdummy->DrawCopy("AXIS");
	
  origstat->SetAxisRange(0.11,0.49,"x");
  origstat->SetLineColor(kBlack);
  origstat->SetMarkerColor(kBlack);
  // origstat->SetMarkerStyle(30);
  origstat->SetMarkerStyle(29);
  origstat->SetMarkerSize(2);

  fixedsyst->SetAxisRange(0.11,0.49,"x");
  fixedsyst->SetMarkerSize(0);
  fixedsyst->SetLineWidth( 0 );
  fixedsyst->SetFillStyle(1001);
  fixedsyst->SetFillColor( kGray );
  fixedsyst->SetMarkerColor( kGray );
  fixedsyst->Draw("9E2same");

  origsyst->SetAxisRange(0.11,0.49,"x");
  origsyst->SetMarkerSize(0);
  origsyst->SetLineWidth( 0 );
  origsyst->SetFillStyle(1001);
  origsyst->SetFillColor( kYellow+3 );
  origsyst->SetMarkerColor( kYellow+3 );
  origsyst->Draw("9E2same");

  TH1D* origmin = origstat->Clone( "origmin" );
  TH1D* origmax = origstat->Clone( "origmax" );
  for (int i=1; i<origstat->GetNbinsX(); ++i ){
    origmin->SetBinContent(i, origstat->GetBinContent( i ) - origstat->GetBinError( i ));
    origmax->SetBinContent(i, origstat->GetBinContent( i ) + origstat->GetBinError( i ));
  }
  origmin->SetAxisRange(0.11,0.49,"x");
  origmax->SetAxisRange(0.11,0.49,"x");
  origmin->SetLineStyle( 1 );
  origmin->SetLineWidth( 2 );
  origmax->SetLineStyle( 1 );
  origmax->SetLineWidth( 2 );
  origmin->Draw("9hist][same");
  origmax->Draw("9hist][same");
  // origstat->DrawCopy("9 hist p same");

  leg = new TLegend( 0.15 , 0.65, 0.89, 0.89 );
  leg->SetBorderSize(0);    leg->SetLineWidth(10);
  leg->SetFillStyle(0);     leg->SetMargin(0.2);

  leg->AddEntry(origstat, "stat. error", "l");
  leg->AddEntry(origsyst, "syst. error estimate from JES variation", "f");
  leg->AddEntry(fixedsyst, "syst. error fixed to 8%", "f");
  leg->Draw();
  gPad->SaveAs(plotpath+"OrigSystUncComparison.png");
  gPad->SaveAs(plotpath+"OrigSystUncComparison.pdf");

  // Composition of new uncertainties
  // --------------------------------
  // IMPORTANT:
  // Previous was on _data_
  // Now e only have GEANT (because that's how we get them)
  // So, propagate raw syst. uncertainties onto BBB
  // Then set stat. errors as they were before
  // Work with 20 bins until the end
  
  TString newpivot   = "zgSubLead2025Lo";
  TString scorrector = "ratioAS_20_25";
  // Corrector
  TH1D* corrector  = (TH1D*) fBinByBinRatio->Get( scorrector )->Clone("corrector");
  TH1D* newstat  = (TH1D*) fNew->Get( newpivot+"_E0" )->Clone("newstat");
  TH1D* newE  = (TH1D*) fNew->Get( newpivot+"_E" )->Clone("new_E");
  TH1D* newT  = (TH1D*) fNew->Get( newpivot+"_T" )->Clone("new_T");
  TH1D* newR  = (TH1D*) fNew->Get( newpivot+"_R" )->Clone("new_R");
  TH1D* newTot  = (TH1D*) fNew->Get( newpivot+"_minmax" )->Clone("newTot");

  // You gotta be kidding. Some nonsense about different limits X-(
  if ( newstat->GetNbinsX() != corrector->GetNbinsX() ){
    cerr << "Incompatible corrector binning." << endl;
    return -1;
  }  
  TH1D* hratio = newstat->Clone( "hratio");
  hratio->Reset();
  for (int i=0; i<=hratio->GetNbinsX(); ++i ){
    hratio->SetBinContent(i, corrector->GetBinContent( i ) );
  }

  // Propagate through BBB
  newstat->Divide(hratio);
  newE->Divide(hratio);
  newT->Divide(hratio);
  newR->Divide(hratio);
  newTot->Divide(hratio);

  // Now rebin
  newstat->Rebin(2);
  newE->Rebin(2);
  newT->Rebin(2);
  newR->Rebin(2);
  newTot->Rebin(2);

  // And scale to unity
  for (int i=0; i<origsyst->GetNbinsX(); ++i ){
    newstat->SetBinError(i, origstat->GetBinError(i) );
    newstat->SetBinContent(i, 1 );
    newE->SetBinError(i, newE->GetBinError(i) / newE->GetBinContent(i) );
    newE->SetBinContent(i, 1 );
    newT->SetBinError(i, newT->GetBinError(i) / newT->GetBinContent(i) );
    newT->SetBinContent(i, 1 );
    newR->SetBinError(i, newR->GetBinError(i) / newR->GetBinContent(i) );
    newR->SetBinContent(i, 1 );
    newTot->SetBinError(i, newTot->GetBinError(i) / newTot->GetBinContent(i) );
    newTot->SetBinContent(i, 1 );
  }

  new TCanvas;
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels

  newstat->SetTitle(";z_{g};rel. uncertainty");
  newstat->GetXaxis()->SetTitleSize(0.07);
  newstat->GetYaxis()->SetTitleSize(0.07);
  newstat->GetXaxis()->SetTitleOffset(xto);
  newstat->GetYaxis()->SetTitleOffset(yto);

  axisdummy->SetAxisRange(0.8,1.35,"y");
  axisdummy->DrawCopy("AXIS");
  
  newstat->SetAxisRange(0.11,0.49,"x");
  newstat->SetAxisRange(0.8,1.35,"y");
  newstat->SetLineColor(kBlack);
  newstat->SetMarkerColor(kBlack);
  // newstat->SetMarkerStyle(30);
  newstat->SetMarkerStyle(29);
  newstat->SetMarkerSize(2);
  // newstat->SetLineStyle(2);
  // newstat->SetLineWidth(1); 
  // newstat->DrawCopy("9same");

  newTot->SetAxisRange(0.11,0.49,"x");
  newTot->SetFillColor(kGray);
  newTot->SetLineWidth( 0 );
  newTot->DrawCopy("9e2same");

  newR->SetAxisRange(0.11,0.49,"x");
  newR->SetLineWidth( 0 );
  newT->SetAxisRange(0.11,0.49,"x");
  newT->SetLineWidth( 0 );
  newE->SetAxisRange(0.11,0.49,"x");
  newE->SetLineWidth( 0 );

  newT->SetFillColor( kPink+6 );
  newE->SetFillColor( kAzure-6 );
  newR->SetFillColor( kGreen+1 );

  newR->DrawCopy("9e2same");
  newE->DrawCopy("9e2same");
  newT->DrawCopy("9e2same");

  TH1D* newmin = newstat->Clone( "newmin" );
  TH1D* newmax = newstat->Clone( "newmax" );
  for (int i=1; i<newstat->GetNbinsX(); ++i ){
    newmin->SetBinContent(i, newstat->GetBinContent( i ) - newstat->GetBinError( i ));
    newmax->SetBinContent(i, newstat->GetBinContent( i ) + newstat->GetBinError( i ));
  }
  newmin->SetAxisRange(0.11,0.49,"x");
  newmax->SetAxisRange(0.11,0.49,"x");
  newmin->SetLineStyle( 1 );
  newmin->SetLineWidth( 2 );
  newmax->SetLineStyle( 1 );
  newmax->SetLineWidth( 2 );
  newmin->Draw("9hist][same");
  newmax->Draw("9hist][same");
  // newstat->DrawCopy("9 p hist same");
  
  leg = new TLegend( 0.4 , 0.6, 0.89, 0.89 );
  leg->SetBorderSize(0);    leg->SetLineWidth(10);
  leg->SetFillStyle(0);     leg->SetMargin(0.15);

  leg->AddEntry(newstat, "stat. error", "l");
  leg->AddEntry(newE, "Tracking Efficiency", "f");
  leg->AddEntry(newR, "Tracking Resolution", "f");
  leg->AddEntry(newT, "Tower Scale", "f");
  leg->AddEntry(newTot, "Quadrature Sum", "f");
  leg->Draw();
  gPad->SaveAs(plotpath+"NewErrorsStacked.png");
  gPad->SaveAs(plotpath+"NewErrorsStacked.pdf");

  // new TCanvas;
  // newR->SetAxisRange(0.9,1.1,"y");
  // newR->SetTitle("Resolution");
  // newR->DrawCopy("");
  // new TCanvas;
  // newE->SetAxisRange(0.9,1.1,"y");    
  // newE->SetTitle("Efficiency");
  // newE->DrawCopy("");
  // new TCanvas;
  // newT->SetAxisRange(0.9,1.1,"y");
  // newT->SetTitle("Tower Scale");
  // newT->DrawCopy("");

  // Incremental sum
  // ---------------
  TH1D* newET  = newE->Clone("newET");
  TH1D* newETR = newE->Clone("newETR"); // Just a cross check, should be all but identical to newTot
  for (int i=0; i<origsyst->GetNbinsX(); ++i ){
    newET->SetBinError(i, TMath::Sqrt( pow(newE->GetBinError(i),2 ) + pow(newT->GetBinError(i),2 ) ) );
    newET->SetBinContent(i, 1 );

    newETR->SetBinError(i, TMath::Sqrt( pow(newE->GetBinError(i),2 ) + pow(newT->GetBinError(i),2 ) + pow(newR->GetBinError(i),2 ) ) );
    newETR->SetBinContent(i, 1 );
  }
   
  new TCanvas;
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels

  newstat->SetTitle(";z_{g};rel. uncertainty");
  newstat->GetXaxis()->SetTitleSize(0.07);
  newstat->GetYaxis()->SetTitleSize(0.07);
  newstat->GetXaxis()->SetTitleOffset(xto);
  newstat->GetYaxis()->SetTitleOffset(yto);

  axisdummy->SetAxisRange(0.8,1.35,"y");
  axisdummy->DrawCopy("AXIS");
  newstat->SetAxisRange(0.11,0.49,"x");
  newstat->SetLineColor(kBlack);
  newstat->SetMarkerColor(kBlack);
  newstat->SetMarkerStyle(30);
  newstat->SetMarkerStyle(29);
  newstat->SetMarkerSize(2);  
  // newstat->DrawCopy("9same");

  newTot->SetAxisRange(0.11,0.49,"x");
  newTot->SetFillColor(kGray);
  newTot->SetLineWidth( 0 );
  newTot->DrawCopy("9e2same");

  newET->SetFillColor( kPink+6 );
  newE->SetFillColor( kAzure-6 );

  newET->SetAxisRange(0.11,0.49,"x");
  // newET->SetFillColor(kBlue);
  newET->SetLineWidth( 0 );
  newET->DrawCopy("9e2same");

  newE->SetAxisRange(0.11,0.49,"x");
  // newE->SetFillColor(kGreen);
  newE->SetLineWidth( 0 );
  newE->DrawCopy("9e2same");

  newmin->Draw("9hist][same");
  newmax->Draw("9hist][same");
  // newstat->DrawCopy("9 p hist same");
  
  leg = new TLegend( 0.15 , 0.6, 0.89, 0.89 );
  leg->SetBorderSize(0);    leg->SetLineWidth(10);
  leg->SetFillStyle(0);     leg->SetMargin(0.15);

  leg->AddEntry(newstat, "stat. error", "l");
  leg->AddEntry(newE, "Tracking Efficiency", "f");
  leg->AddEntry(newET, "Tracking Efficiency #oplus Tower Scale", "f");
  leg->AddEntry(newTot, "Tracking Eff. #oplus Tower Scale #oplus Tracking Resolution", "f");
  leg->Draw();
  gPad->SaveAs(plotpath+"NewErrorsSummed.png");
  gPad->SaveAs(plotpath+"NewErrorsSummed.pdf");

  return 0;

}
