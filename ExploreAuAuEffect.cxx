{
  // gStyle->SetOptFit(1);
  // gStyle->SetOptStat(220001111);
  gStyle->SetOptStat(0);

  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  
  gStyle->SetHistLineWidth(2);
    
  // axis labels and titles
  float ls = 0.05;
  float ts = 0.05;
  float yoff = 1.06;
  float xoff = 1.06;

// canvas stuff
  float lm = 0.11;
  float bm = 0.13;
  float yto = 0.5;
  float xto = 0.5;

  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // TFile *f = TFile::Open("GroomppInAuAuAj_105_MixTest.root");
  TFile *f = TFile::Open("AjResults/Tow0_Eff0_R0.2_Remade_Groom_Fresh_NicksList_HC100_105_ppInAuAuAj.root");
  // TFile *f = TFile::Open("AjResults/Tow0_Eff0_Remade_Groom_Fresh_NicksList_HC100_105_ppInAuAuAj.root");

  TTree * ResultTree = f->Get("ResultTree");

  TString plotpath="plots/";
  double Rmatch = 0.4; // Could change for R=0.2
  if ( TString(f->GetName()).Contains("R0.2") ){
    plotpath+="R0.2_";
  }
       

  TLorentzVector* pTriggerJet = new TLorentzVector();
  ResultTree->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  ResultTree->SetBranchAddress("TriggerJet", &pTriggerJet);

  TLorentzVector* pAwayJet = new TLorentzVector();
  ResultTree->GetBranch("AwayJet")->SetAutoDelete(kFALSE);
  ResultTree->SetBranchAddress("AwayJet", &pAwayJet);
  
  TLorentzVector* pTriggerJetLo = new TLorentzVector();
  ResultTree->GetBranch("TriggerJetLo")->SetAutoDelete(kFALSE);
  ResultTree->SetBranchAddress("TriggerJetLo", &pTriggerJetLo);

  TLorentzVector* pAwayJetLo = new TLorentzVector();
  ResultTree->GetBranch("AwayJetLo")->SetAutoDelete(kFALSE);
  ResultTree->SetBranchAddress("AwayJetLo", &pAwayJetLo);
  
  TLorentzVector* pJ1 = new TLorentzVector();
  ResultTree->GetBranch("j1")->SetAutoDelete(kFALSE);
  ResultTree->SetBranchAddress("j1", &pJ1);

  TLorentzVector* pJ2 = new TLorentzVector();
  ResultTree->GetBranch("j2")->SetAutoDelete(kFALSE);
  ResultTree->SetBranchAddress("j2", &pJ2);

  TLorentzVector* pJM1 = new TLorentzVector();
  ResultTree->GetBranch("jm1")->SetAutoDelete(kFALSE);
  ResultTree->SetBranchAddress("jm1", &pJM1);

  TLorentzVector* pJM2 = new TLorentzVector();
  ResultTree->GetBranch("jm2")->SetAutoDelete(kFALSE);
  ResultTree->SetBranchAddress("jm2", &pJM2);

  double refmult;
  ResultTree->SetBranchAddress("refmult", &refmult);

  
  TH1D* tdr = new TH1D( "tdr","tdr",100, 0, TMath::TwoPi() );
  TH1D* tdrlo = new TH1D( "tdrlo","tdrlo",100, 0, TMath::TwoPi() );
  TH1D* adr = new TH1D( "adr","adr",100, 0, TMath::TwoPi() );
  TH1D* adrlo = new TH1D( "adrlo","adrlo",100, 0, TMath::TwoPi() );

  TH1D* tdpt = new TH1D( "tdpt","",200, -5, 15  );
  TH1D* adpt = new TH1D( "adpt","",200, -5, 15  );
  
  TH1D* tdptlo = new TH1D( "tdptlo","",300, -30,30  );
  TH1D* adptlo = new TH1D( "adptlo","",300, -30,30  );

  TH2D* tdpt2 = new TH2D( "tdpt2","tdpt2",100, 0, 50, 200, -5, 15  );
  TH2D* adpt2 = new TH2D( "adpt2","adpt2",100, 0, 50, 200, -5, 15  );
  
  TH2D* tdptlo2 = new TH2D( "tdptlo2","tdptlo2",100, 0, 50, 300, -30,30  );
  TH2D* adptlo2 = new TH2D( "adptlo2","adptlo2",100, 0, 50, 300, -30,30  );

  // DEBUG
  // float rho;
  // ResultTree->SetBranchAddress("rho", &rho);
  
  // TH1D* rho0= new TH1D("rho0","rho0",100, 20, 120);
  // TH1D* rho1= new TH1D("rho1","rho1",100, 20, 120);
  // new TCanvas;
  // ResultTree->Draw("rho>>rho0","refmult>269");    
  // TH2D* h2 = new TH2D("h2","",100, -TMath::Pi(), TMath::Pi(), 100, -1, 1);

  
  for ( Long64_t evi = 0; evi< ResultTree->GetEntries() ; ++evi ){
    ResultTree->GetEntry(evi);
    if ( refmult < 269)     continue;

    // Swap if necessary
    // -----------------
    TLorentzVector* tMatch = pJ1;
    if ( pTriggerJet->DeltaR( *tMatch )> Rmatch) tMatch = pJ2;
    TLorentzVector* tMatchLo = pJM1;
    if ( pTriggerJetLo->DeltaR( *tMatchLo )> Rmatch) tMatchLo = pJM2;

    TLorentzVector* aMatch = pJ2;
    if ( pAwayJet->DeltaR( *aMatch )> Rmatch) aMatch = pJ1;
    TLorentzVector* aMatchLo = pJM2;
    if ( pAwayJetLo->DeltaR( *aMatchLo )> Rmatch) aMatchLo = pJM1;

    // Check if now matched
    // --------------------
      
    if ( pTriggerJet->DeltaR( *tMatch )> Rmatch ){
	cout << "Unmatched TriggerJet."<< endl;      
    } else {
      tdr->Fill( pTriggerJet->DeltaR( *tMatch ) );
      tdpt->Fill( tMatch->Pt() - pTriggerJet->Pt() );
      tdpt2->Fill( pTriggerJet->Pt(), tMatch->Pt() - pTriggerJet->Pt() );
    }
    
    if ( pTriggerJetLo->DeltaR( *tMatchLo )> Rmatch ){
    //if ( pTriggerJetLo->DeltaR( *tMatchLo )> Rmatch || pTriggerJetLo->Pt()>12){
    // if ( pTriggerJetLo->DeltaR( *tMatchLo )> Rmatch || pTriggerJetLo->Pt()<20){ // DEBUG
      cout << "Unmatched TriggerJetLo."<< endl;      
    } else {
      // if ( tMatchLo->Pt() - pTriggerJetLo->Pt()<-10){ // DEBUG
      // 	cout << tMatchLo->Pt() << "  " << pTriggerJetLo->Pt() << "  ----> dR = " << pTriggerJetLo->DeltaR( *tMatchLo ) << endl;
      // 	//cout << "hard core distance is " << tMatch->DeltaR( *tMatchLo ) << " and " << tMatch->Pt() << " - " << pTriggerJet->Pt() << "  = " << tMatch->Pt() - pTriggerJet->Pt() << endl;
      // 	h2->Fill( tMatchLo->Phi(), tMatchLo->Eta());
      // // tMatchLo->Print();
      // 	// pTriggerJetLo->Print();
      // 	// rho1->Fill(rho);
      //     }
      
      tdrlo->Fill( pTriggerJetLo->DeltaR( *tMatchLo ) );
      tdptlo->Fill( tMatchLo->Pt() - pTriggerJetLo->Pt() );
      tdptlo2->Fill( pTriggerJetLo->Pt(), tMatchLo->Pt() - pTriggerJetLo->Pt() );
    }


    if ( pAwayJet->Pt()>5 ){
      if ( pAwayJet->DeltaR( *aMatch )> Rmatch ){
	cout << "Unmatched AwayJet."<< endl;      
      } else {
	adr->Fill( pAwayJet->DeltaR( *aMatch ) );
	adpt->Fill( aMatch->Pt() - pAwayJet->Pt() );
	adpt2->Fill( pAwayJet->Pt(), aMatch->Pt() - pAwayJet->Pt() );
      }
    }
    
    if ( pAwayJetLo->Pt()>5 ){
      if ( pAwayJetLo->DeltaR( *aMatchLo )> Rmatch ){
	cout << "Unmatched AwayJetLo."<< endl;      
      } else {
	adrlo->Fill( pAwayJetLo->DeltaR( *aMatchLo ) );
	adptlo->Fill( aMatchLo->Pt() - pAwayJetLo->Pt() );
	adptlo2->Fill( pAwayJetLo->Pt(), aMatchLo->Pt() - pAwayJetLo->Pt() );
      }
    }
  }    

  // new TCanvas;
  // h2->Draw("colz");
  
  // new TCanvas;
  // tdr->SetLineColor(kRed);
  // tdr->Draw();
  // tdrlo->SetLineColor(kBlack);
  // tdrlo->Draw("same");

  // new TCanvas;
  // adr->SetLineColor(kRed);
  // adr->Draw();
  // adrlo->SetLineColor(kBlack);
  // adrlo->Draw("same");

  int rebin=2;
  int rebinlo=6;
  tdpt->Rebin(rebin);
  tdptlo->Rebin(rebinlo);
  TObjArray toa;
  toa.Add(tdpt);
  toa.Add(tdptlo);
  toa.Add(adpt);
  toa.Add(adptlo);

  TH1D* h;
  for (int i=0 ; i<toa.GetEntries() ; ++i ){
    h=(TH1D*) toa.At(i);
    h->Scale(1./h->Integral("width") );

    h->SetTitle(";#deltap_{T}=p_{T}^{pp#oplusAA}-p_{T}^{pp} [GeV/c];dN/d#deltap_{T} [(GeV/c)^{-1}]");
 
    h->GetXaxis()->SetLabelSize( ls );
    h->GetXaxis()->SetTitleSize( ls );
    h->GetXaxis()->SetTitleOffset( xoff );
    h->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetXaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
    
    h->GetYaxis()->SetLabelSize( ls );
    h->GetYaxis()->SetTitleSize( ls );
    h->GetYaxis()->SetTitleOffset( yoff );
    h->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold    
  }

    
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(ls);
  latex.SetTextFont( 42 ); // 42: helvetica, 62: helvetica bold

  // float ymin = 5e-4;
  float ymin = 1e-3;

  new TCanvas;
  gPad->SetLogy();
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
    
  tdpt->SetLineColor(kRed);
  tdpt->SetMinimum( ymin );
  // tdpt->Draw("hist");
  tdpt->Draw("");
  tdpt->Draw("histsame");
  latex.SetTextColor(kBlack);  latex.DrawLatex( .65,.85, "p_{T}^{Cut}>2 GeV/c");

  latex.DrawLatex( 0.65, 0.77, "Au+Au, 0-20%");
  if (TString(f->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.65, 0.7, "Anti-k_{T}, R=0.2");
    latex.SetTextColor(kBlack);  latex.DrawLatex( .15,.25, "c)");
  } else {
    latex.DrawLatex( 0.65, 0.7, "Anti-k_{T}, R=0.4");
    latex.SetTextColor(kBlack);  latex.DrawLatex( .15,.25, "a)");
  }  
  latex.SetTextColor( kAzure-6 );   latex.DrawLatex( .75,.5, "STAR");
  
  gPad->SaveAs(plotpath+"tdpt.png");  


  new TCanvas;
  gPad->SetLogy();
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
    
  tdptlo->SetMinimum( ymin );
  tdptlo->SetLineColor(kBlack);
  tdptlo->Draw("");
  tdptlo->Draw("histsame");
  tdptlo->Fit("gaus","N","",-30,0);
  tdptlo->Fit("gaus","N","",-30,30);
  latex.SetTextColor(kBlack); latex.DrawLatex( .15,.85, "p_{T}^{Cut}>0.2 GeV/c, Matched");
  latex.DrawLatex( 0.15, 0.77, "Au+Au, 0-20%");
  if (TString(f->GetName()).Contains("R0.2") ){
    latex.DrawLatex( 0.15, 0.7, "Anti-k_{T}, R=0.2");
    latex.SetTextColor(kBlack);  latex.DrawLatex( .15,.25, "d)");      
  } else {
    latex.DrawLatex( 0.15, 0.7, "Anti-k_{T}, R=0.4");
    latex.SetTextColor(kBlack);  latex.DrawLatex( .15,.25, "b)");
  }
  latex.SetTextColor( kAzure-6 );   latex.DrawLatex( .75,.5, "STAR");

  gPad->SaveAs(plotpath+"tdptlo.png");

  // leg->AddEntry(   PpMcRecoilPtProf, "p_{T}^{Cut}>2 GeV/c");
  // leg->AddEntry(   PpMcRecoilPtProfLo, "p_{T}^{Cut}>0.2 GeV/c, Matched");


  // new TCanvas;
  // rho1->Draw();

  return 0;
  new TCanvas;
  gPad->SetLogy();
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels
    
  adpt->SetLineColor(kRed);
  adpt->Draw("hist");

  latex.SetTextColor(kBlack); latex.DrawLatex( .65,.85, "p_{T}^{Cut}>2 GeV/c");
  if ( TString(f->GetName()).Contains("R0.2") )    latex.DrawLatex( .65,.77, "R=0.2");
  else     latex.DrawLatex( .65,.77, "R=0.4");
  latex.SetTextColor( kAzure-6 );   latex.DrawLatex( .65,.7, "STAR");
  gPad->SaveAs(plotpath+"adpt.png");
    
  new TCanvas;
  gPad->SetLogy();
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLeftMargin( lm );// for bigger labels
  gPad->SetBottomMargin( bm );// for bigger labels

  adptlo->SetLineColor(kBlack);
  adptlo->Draw("");
  adptlo->Fit("gaus","N","",-30,0);
  adptlo->Fit("gaus","N","",-30,30);

  latex.SetTextColor(kBlack);  latex.DrawLatex( .15,.85, "p_{T}^{Cut}>0.2 GeV/c, Matched");
  if ( TString(f->GetName()).Contains("R0.2") )    latex.DrawLatex( .15,.77, "R=0.2");
  else     latex.DrawLatex( .15,.77, "R=0.4");
  latex.SetTextColor( kAzure-6 );   latex.DrawLatex( .15,.7, "STAR");
  gPad->SaveAs(plotpath+"adptlo.png");

  // new TCanvas;
  // tdpt2->Draw("colz");
  // new TCanvas;
  // tdptlo2->Draw("colz");
  // new TCanvas;
  // adpt2->Draw("colz");
  // new TCanvas;
  // adptlo2->Draw("colz");

  // new TCanvas;
  // tdpt2->ProfileX()->Draw();
  // new TCanvas;
  // tdptlo2->ProfileX()->Draw();
  // new TCanvas;
  // adpt2->ProfileX()->Draw();
  // new TCanvas;
  // adptlo2->ProfileX()->Draw();

}
