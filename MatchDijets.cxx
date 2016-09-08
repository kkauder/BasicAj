  // TFile* fLo = new TFile("Results/Groom_RhicPythiaOnly_10_ptHat=30_35.root","READ");
  // TFile* fLo = new TFile("Results/Groom_Rhic.root","READ");
  // TFile* fLo = new TFile("Results/Groom_pp.root","READ");

double CalcAj ( TLorentzVector* j1, TLorentzVector* j2 );

int MatchDijets(TString sRef = "AjResults/Groom_Aj_HT54_NoEff_GeantMc.root",
		TString sEmb = "AjResults/Tow-1_Eff0_Groom_Aj_HT54_GeantInAuAuAj_newpicoDstcentralMB_8177020_DC4BA348C050D5562E7461357C4B341D_0.root" ) {

  int MinRefmult = 269;

  //  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);

  float MatchingR=0.4;

  TFile* fRef = new TFile( sRef ,"READ");
  TTree* RRef = (TTree*) fRef->Get("ResultTree");
  RRef->SetName("RRef");

  TFile* fEmb = new TFile( sEmb ,"READ");
  TTree* REmb = (TTree*) fEmb->Get("ResultTree");
  REmb->SetName("REmb");

  // TString outbase = gSystem->BaseName( sLo );
  // outbase.ReplaceAll( ".root", "");
  
  //  TFile* fOut = new TFile("Results/" + outbase + ".DijetZg.root","RECREATE");
  
  RRef->BuildIndex("runid","eventid");
  REmb->BuildIndex("jetrunid","jeteventid");
  
  // To match, use runid and eventid
  // -------------------------------
  int runidRef;
  int eventidRef;  
  RRef->SetBranchAddress("eventid", &eventidRef );
  RRef->SetBranchAddress("runid", &runidRef );

  int runidEmb;
  int eventidEmb;  
  REmb->SetBranchAddress("jeteventid", &eventidEmb );
  REmb->SetBranchAddress("jetrunid", &runidEmb );
  
  // Interesting measurables -- Ref
  // ------------------------------
  TLorentzVector *pJ1Ref = new TLorentzVector();
  TLorentzVector *pJ2Ref = new TLorentzVector();
  TLorentzVector *pJM1Ref = new TLorentzVector();
  TLorentzVector *pJM2Ref = new TLorentzVector();
  double refmultRef;
  double weightRef=1;
  RRef->SetBranchAddress("weight", &weightRef );
  
  ResultTree->SetBranchAddress("j1", &pJ1Ref);
  ResultTree->SetBranchAddress("j2", &pJ2Ref);
  ResultTree->SetBranchAddress("jm1", &pJM1Ref);
  ResultTree->SetBranchAddress("jm2", &pJM2Ref);
  ResultTree->SetBranchAddress("refmult", &refmultRef);
  
  // Interesting measurables -- Emb
  // ------------------------------
  TLorentzVector *pJ1Emb = new TLorentzVector();
  TLorentzVector *pJ2Emb = new TLorentzVector();
  TLorentzVector *pJM1Emb = new TLorentzVector();
  TLorentzVector *pJM2Emb = new TLorentzVector();
  double refmultEmb;
  double weightEmb=1;
  REmb->SetBranchAddress("weight", &weightEmb );
  
  ResultTree->SetBranchAddress("j1", &pJ1Emb);
  ResultTree->SetBranchAddress("j2", &pJ2Emb);
  ResultTree->SetBranchAddress("jm1", &pJM1Emb);
  ResultTree->SetBranchAddress("jm2", &pJM2Emb);
  ResultTree->SetBranchAddress("refmult", &refmultEmb);
  

  // Histos
  // -------
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // TH2D* LeadPtLoVPtHi = new TH2D("LeadPtLoVPtHi", ";p_{T,Hi};p_{T,Lo}",100, 0, 60, 100, 0, 60 );
  // TH1D* LeadPtLoMinusPtHi = new TH1D("LeadPtLoMinusPtHi", ";p_{T,Lo}-p_{T,Hi}",120, -30, 30 );

  // TH1D* hLeadPtHi = new TH1D( "hLeadPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{lead} [GeV/c]", 100, 10 , 60 );
  // TH1D* hLeadPtLo = new TH1D( "hLeadPtLo","p_{T}^{C} > 0.2 GeV/c;p_{T}^{lead} [GeV/c]", 100, 10 , 60 );
  // TH1D* hSubLeadPtHi = new TH1D( "hSubLeadPtHi","p_{T}^{C} > 2 GeV/c;p_{T}^{sublead} [GeV/c]", 100, 0 , 50 );
  // TH1D* hSubLeadPtLo = new TH1D( "hSubLeadPtLo","p_{T}^{C} > 0.2 GeV/c;p_{T}^{sublead} [GeV/c]", 100, 0 , 50 );
  // // TH1D* LeadPt = new TH1D("LeadPt","Leading jet p_{T}", 120, 0, 240);
  // TH1D* EmbLeadPt = new TH1D("EmbLeadPt","Embedded Leading jet p_{T}", 120, 0, 240);
  // TH1D* GroomLeadPt = new TH1D("GroomLeadPt","Groomed Leading jet p_{T}", 120, 0, 240);
  // TH1D* EmbGroomLeadPt = new TH1D("EmbGroomLeadPt","Groomed Embedded Leading jet p_{T}", 120, 0, 240);

  // int nzgBins=40;
  // TH1D* zgLead1020Hi = new TH1D("zgLead1020Hi","Leading Hi jet with 10<p_{T}<20", nzgBins, 0.05, 0.55);
  // TH1D* zgLead2030Hi = new TH1D("zgLead2030Hi","Leading Hi jet with 20<p_{T}<30", nzgBins, 0.05, 0.55);
  // TH1D* zgLead3040Hi = new TH1D("zgLead3040Hi","Leading Hi jet with 30<p_{T}<40", nzgBins, 0.05, 0.55);
  // TH1D* zgLead4060Hi = new TH1D("zgLead4060Hi","Leading Hi jet with 40<p_{T}<60", nzgBins, 0.05, 0.55);

  // TH1D* zgLead1020Lo = new TH1D("zgLead1020Lo","Leading Lo jet with 10<p_{T}<20", nzgBins, 0.05, 0.55);
  // TH1D* zgLead2030Lo = new TH1D("zgLead2030Lo","Leading Lo jet with 20<p_{T}<30", nzgBins, 0.05, 0.55);
  // TH1D* zgLead3040Lo = new TH1D("zgLead3040Lo","Leading Lo jet with 30<p_{T}<40", nzgBins, 0.05, 0.55);
  // TH1D* zgLead4060Lo = new TH1D("zgLead4060Lo","Leading Lo jet with 40<p_{T}<60", nzgBins, 0.05, 0.55);

  // TH2D* SubLeadPtLoVPtHi = new TH2D("SubLeadPtLoVPtHi", ";p_{T,Hi};p_{T,Lo}",100, 0, 60, 100, 0, 60 );
  // TH1D* SubLeadPtLoMinusPtHi = new TH1D("SubLeadPtLoMinusPtHi", ";p_{T,Lo}-p_{T,Hi}",120, -30, 30 );

  // TH1D* SubLeadPt = new TH1D("SubLeadPt","SubLeading jet p_{T}", 120, 0, 240);
  // TH1D* EmbSubLeadPt = new TH1D("EmbSubLeadPt","Embedded SubLeading jet p_{T}", 120, 0, 240);
  // TH1D* GroomSubLeadPt = new TH1D("GroomSubLeadPt","Groomed SubLeading jet p_{T}", 120, 0, 240);
  // TH1D* EmbGroomSubLeadPt = new TH1D("EmbGroomSubLeadPt","Groomed Embedded SubLeading jet p_{T}", 120, 0, 240);

  // TH1D* zgSubLead1020Hi = new TH1D("zgSubLead1020Hi","SubLeading Hi jet with 10<p_{T}<20", nzgBins, 0.05, 0.55);
  // TH1D* zgSubLead2030Hi = new TH1D("zgSubLead2030Hi","SubLeading Hi jet with 20<p_{T}<30", nzgBins, 0.05, 0.55);
  // TH1D* zgSubLead3040Hi = new TH1D("zgSubLead3040Hi","SubLeading Hi jet with 30<p_{T}<40", nzgBins, 0.05, 0.55);
  // TH1D* zgSubLead4060Hi = new TH1D("zgSubLead4060Hi","SubLeading Hi jet with 40<p_{T}<60", nzgBins, 0.05, 0.55);

  // TH1D* zgSubLead1020Lo = new TH1D("zgSubLead1020Lo","SubLeading Lo jet with 10<p_{T}<20", nzgBins, 0.05, 0.55);
  // TH1D* zgSubLead2030Lo = new TH1D("zgSubLead2030Lo","SubLeading Lo jet with 20<p_{T}<30", nzgBins, 0.05, 0.55);
  // TH1D* zgSubLead3040Lo = new TH1D("zgSubLead3040Lo","SubLeading Lo jet with 30<p_{T}<40", nzgBins, 0.05, 0.55);
  // TH1D* zgSubLead4060Lo = new TH1D("zgSubLead4060Lo","SubLeading Lo jet with 40<p_{T}<60", nzgBins, 0.05, 0.55);

  // TH1D* hdphiHi = new TH1D( "hdphiHi","#Delta#phi for hard constituent jets", 200, -2, 2 );
  // TH1D* hdphiLo = new TH1D( "hdphiLo","#Delta#phi for soft constituent jets", 200, -2, 2 );

  // TH2D* AJ_hi = new TH2D( "AJ_hi","A_{J} for hard constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );
  // TH2D* AJ_lo = new TH2D( "AJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 50, -0.6, 0.9, 800, -0.5, 799.5 );

  // Analysis
  // --------
  
  Long64_t NEvents = RRef->GetEntries();
  // NEvents = 1000;

  for ( int evi=0; evi<NEvents; ++evi){
    if ( !(evi%10000) ) cout << "Working on " << evi << " / " << NEvents << endl;
    RRef->GetEntry(evi);

    //cout << refmult << endl;
    // if ( refmult<MinRefmult ) continue;
    
    Long64_t eviEmb=-1;
    eviEmb = REmb->GetEntryNumberWithIndex( runidRef, eventidRef );
    if ( eviEmb < 0 ){
      // MISSED or LOST
      continue;
    }
    cout << "Found it" << endl;
    REmb->GetEntry( eviEmb );

    if ( fabs(weightRef / weightEmb - 1) > 1e-4 ) {
      cerr << "weightRef / weightEmb = " << weightRef / weightEmb  << endl;
      cerr << "weightRef = " <<  weightRef  << "    weightEmb = " <<  weightEmb << endl;
      cout << "Ref: runid = " << runidRef << "  eventid = " << eventidRef << endl;
      cout << "Ref: evi = " << evi << endl;
      cout << "Emb: runid = " << runidEmb << "  eventid = " << eventidEmb << endl;
      cout << "Emb: evi = " << eviEmb << endl;

      return -1;
    }

    // // Leading jets
    // if ( LongIndex ){
    //   // cout << "HI: runid = " << runidHi << "  eventid = " << eventidHi << endl;
    //   // cout << "HI: runeventHi = " << runeventHi << "  EmbruneventHi = " << EmbruneventHi << endl;
    //   // cout << "HI: evi = " << evi << endl;
    //   // // cout << "LO: evi = " << RLo->GetEntryNumberWithIndex( EmbruneventHi, runeventHi ) << endl;
    //   // cout << "LO: evi = " << eviLo << endl;
    //   // cout << "pJetsLo->GetEntries() = " << pJetsLo->GetEntries() << endl;
    //   // // RLo->Show( eviLo );
    //   // // RLo->Show( RLo->GetEntryNumberWithIndex( EmbruneventHi, runeventHi ) );
    // }

    // if ( pJetsLo->GetEntries()<1 ) continue;
    // JetLo = (TStarJetVectorJet*) pJetsLo->At(0);
    // JetHi = (TStarJetVectorJet*) pJetsHi->At(0);
    
    // LeadPtLoVPtHi->Fill ( JetHi->Pt(), JetLo->Pt(), weightHi );
    // LeadPtLoMinusPtHi->Fill ( JetLo->Pt() - JetHi->Pt(), weightHi );

    // // Do we have a sub-leading jet?
    // if ( pJetsHi->GetEntries() < 2 )      continue;

    // TStarJetVectorJet* SubLeadingJetHi = (TStarJetVectorJet*) pJetsHi->At(1);    
    
    // // Back-to-back?
    // float dPhiHi = TMath::Pi() - fabs ( SubLeadingJetHi->DeltaPhi( *JetHi ) );
    // hdphiHi->Fill( dPhiHi, weightHi );
    // //if ( dPhiHi > openingAngle ) continue;
    
    // // Matched?
    // if ( pJetsLo->GetEntries() < 2 )      continue;
    // TStarJetVectorJet* SubLeadingJetLo = (TStarJetVectorJet*) pJetsLo->At(1);

    // TStarJetVectorJet* LeadingMatch=0;
    // TStarJetVectorJet* SubLeadingMatch=0;
    // // The following logic selects ONLY among the two leading lo cut jets
    // // if ( JetHi->DeltaR( *JetLo ) < MatchingR ) LeadingMatch = JetLo;
    // // if ( JetHi->DeltaR( *SubLeadingJetLo ) < MatchingR ) LeadingMatch = SubLeadingJetLo;
    // // if ( SubLeadingJetHi->DeltaR( *JetLo ) < MatchingR ) SubLeadingMatch = JetLo;
    // // if ( SubLeadingJetHi->DeltaR( *SubLeadingJetLo ) < MatchingR ) SubLeadingMatch = SubLeadingJetLo;

    // // Instead, do this:
    // for (int i=0; i<pJetsLo->GetEntries(); ++i ){
    //   TStarJetVectorJet* j = (TStarJetVectorJet*) pJetsLo->At(i);
    //   if ( JetHi->DeltaR( *j  ) < MatchingR ) {
    // 	LeadingMatch = j;
    // 	break;
    //   }
    // }
    // for (int i=0; i<pJetsLo->GetEntries(); ++i ){
    //   TStarJetVectorJet* j = (TStarJetVectorJet*) pJetsLo->At(i);
    //   if ( SubLeadingJetHi->DeltaR( *j ) < MatchingR ) {
    // 	SubLeadingMatch = j;
    // 	break;
    //   }
    // }
    
    // if ( !SubLeadingMatch || !LeadingMatch ){
    //   if ( JetHi->Pt()>20 && SubLeadingJetHi->Pt()>10 ) cout << "Couldn't match both sides" << endl;
    //   continue;
    // }
    // if ( JetHi->Pt()>20 && SubLeadingJetHi->Pt()>10 ) {
    //   nDijets++;
    //   // LeadPt->Fill(JetHi->Pt());
    //   hLeadPtHi->Fill( JetHi->Pt() );
    //   hLeadPtLo->Fill( LeadingMatch->Pt() );
    //   hSubLeadPtHi->Fill( SubLeadingJetHi->Pt() );
    //   hSubLeadPtLo->Fill( SubLeadingMatch->Pt() );
    //   AJ_hi->Fill( CalcAj( JetHi, SubLeadingJetHi ), refmult );
    //   AJ_lo->Fill( CalcAj( LeadingMatch, SubLeadingMatch), refmult );
    // }
    
    // // Only now fill
    // // if ( Jet.GetMatch()<0 ) continue;
    // float ptHi = JetHi->Pt();
    // // GroomedJetHi = (TStarJetVectorJet*) pGroomedJetsHi->At(0);
    // // float gptHi = GroomedJetHi->Pt();

    // if ( ptHi >= 10 && ptHi < 20 ){
    //   zgLead1020Hi->Fill ( zgHi[0], weightHi );
    //   zgLead1020Lo->Fill ( zgLo[0], weightLo );
    // }
    // if ( ptHi >= 20 && ptHi < 30 ){
    //   zgLead2030Hi->Fill ( zgHi[0], weightHi );
    //   zgLead2030Lo->Fill ( zgLo[0], weightLo );
    // }
    // if ( ptHi >= 30 && ptHi < 40 ){
    //   zgLead3040Hi->Fill ( zgHi[0], weightHi );
    //   zgLead3040Lo->Fill ( zgLo[0], weightLo );
    // }
    // if ( ptHi >= 40 && ptHi < 60 ){
    //   zgLead4060Hi->Fill ( zgHi[0], weightHi );
    //   zgLead4060Lo->Fill ( zgLo[0], weightLo );
    // }


    // float SubLeadingptHi = SubLeadingJetHi->Pt();

    // if ( SubLeadingptHi >= 10 && SubLeadingptHi < 20 ){
    //   zgSubLead1020Hi->Fill ( zgHi[1], weightHi );
    //   zgSubLead1020Lo->Fill ( zgLo[1], weightLo );
    // }
    // if ( SubLeadingptHi >= 20 && SubLeadingptHi < 30 ){
    //   zgSubLead2030Hi->Fill ( zgHi[1], weightHi );
    //   zgSubLead2030Lo->Fill ( zgLo[1], weightLo );
    // }
    // if ( SubLeadingptHi >= 30 && SubLeadingptHi < 40 ){
    //   zgSubLead3040Hi->Fill ( zgHi[1], weightHi );
    //   zgSubLead3040Lo->Fill ( zgLo[1], weightLo );
    // }
    // if ( SubLeadingptHi >= 40 && SubLeadingptHi < 60 ){
    //   zgSubLead4060Hi->Fill ( zgHi[1], weightHi );
    //   zgSubLead4060Lo->Fill ( zgLo[1], weightLo );
    // }

    // cout << JetLo->Pt() << "  "  << JetLo->Phi() << "  "  << JetLo->Eta() << endl;
    // cout << JetHi->Pt() << "  "  << JetHi->Phi() << "  "  << JetHi->Eta() << endl;
  }
  // new TCanvas;
  // hdphiHi->Draw();

  return 0;
  LeadPtLoVPtHi->SaveAs("LeadPtLoVPtHi.root");
  new TCanvas;
  LeadPtLoVPtHi->Draw("colz");

  LeadPtLoMinusPtHi->SaveAs("LeadPtLoMinusPtHi.root");
  new TCanvas;
  LeadPtLoMinusPtHi->Draw("");


  // SubLeadPtLoVPtHi->SaveAs("SubLeadPtLoVPtHi.root");
  // new TCanvas;
  // SubLeadPtLoVPtHi->Draw("colz");

  // SubLeadPtLoMinusPtHi->SaveAs("SubLeadPtLoMinusPtHi.root");
  // new TCanvas;
  // SubLeadPtLoMinusPtHi->Draw("");


  // Also show theory
  TF1* PbarQjet = new TF1("PbarQjet","1./[0] * 4./3. * ( (1+pow(x,2))/(1-x) + (1 + pow(1-x,2))/x )", 0.1, 0.5);
  PbarQjet->SetParameter( 0,4.2593);
  PbarQjet->SetLineColor(kGreen);
  
  TF1* FUVQjet = new TF1("FUVQjet", "[0]*(PbarQjet)", 0.1,0.5);
  FUVQjet->SetParameter( 0,1);
  FUVQjet->SetLineColor(kBlack);

  // Gluon Jets: contributions from Pqg and Pgg
  // QG
  TF1* PbarQG = new TF1("PbarQG","1./[0] * ( x*x + (1-x)*(1-x) )", 0.1, 0.5);
  PbarQG->SetParameter( 0,0.2427 / (1./6.));
  PbarQG->SetLineColor(kRed);

  // GG
  TF1* PbarGG = new TF1("PbarGG","1./[0] * 6.0 * 2.0 * ( x/(1-x) + (1-x)/x + x*(1-x))", 0.1, 0.5);
  PbarGG->SetParameter( 0, 17.7107 / (1.-1./6.));
  PbarGG->SetLineColor(kGreen);

  // Sum
  TF1* FUVGjet = new TF1("FUVGjet", "PbarGG + PbarQG", 0.1,0.5);
  FUVGjet->SetLineColor(kGray+1);

  // =========================== Draw Hi ===============================
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "Hi Cut, Leading jet" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  zgLead1020Hi->Scale ( 1./ zgLead1020Hi->Integral() / zgLead1020Hi->GetXaxis()->GetBinWidth(1));
  zgLead2030Hi->Scale ( 1./ zgLead2030Hi->Integral() / zgLead2030Hi->GetXaxis()->GetBinWidth(1));
  zgLead3040Hi->Scale ( 1./ zgLead3040Hi->Integral() / zgLead3040Hi->GetXaxis()->GetBinWidth(1));
  zgLead4060Hi->Scale ( 1./ zgLead4060Hi->Integral() / zgLead4060Hi->GetXaxis()->GetBinWidth(1));
    
  zgLead1020Hi->SetTitle(";z_{g};arb.");
  zgLead1020Hi->SetAxisRange(0,10,"y");

  zgLead2030Hi->SetTitle(";z_{g};arb.");
  zgLead2030Hi->SetAxisRange(0,10,"y");

  zgLead2030Hi->SetLineColor(kBlack);
  zgLead2030Hi->Draw("");
  leg->AddEntry( zgLead2030Hi, "20<p_{T}<30");

  zgLead3040Hi->SetLineColor(kRed);
  zgLead3040Hi->Draw("same");
  leg->AddEntry( zgLead3040Hi, "30<p_{T}<40");

  // zgLead4060Hi->SetLineColor(kGreen+1);
  // zgLead4060Hi->Draw("same");
  // leg->AddEntry( zgLead4060Hi, "40<p_{T}<60");

  FUVQjet->Draw("same");
  leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  gPad->SaveAs(  "Plots/" + outbase + "_MatchedLeadZgHi.png");

  // =========================== Draw Lo ===============================
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "Lo Cut, Leading jet" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  zgLead1020Lo->Scale ( 1./ zgLead1020Lo->Integral() / zgLead1020Lo->GetXaxis()->GetBinWidth(1));
  zgLead2030Lo->Scale ( 1./ zgLead2030Lo->Integral() / zgLead2030Lo->GetXaxis()->GetBinWidth(1));
  zgLead3040Lo->Scale ( 1./ zgLead3040Lo->Integral() / zgLead3040Lo->GetXaxis()->GetBinWidth(1));
  zgLead4060Lo->Scale ( 1./ zgLead4060Lo->Integral() / zgLead4060Lo->GetXaxis()->GetBinWidth(1));
    
  zgLead1020Lo->SetTitle(";z_{g};arb.");
  zgLead1020Lo->SetAxisRange(0,10,"y");

  zgLead2030Lo->SetTitle(";z_{g};arb.");
  zgLead2030Lo->SetAxisRange(0,10,"y");

  zgLead2030Lo->SetLineColor(kBlack);
  zgLead2030Lo->Draw("");
  leg->AddEntry( zgLead2030Lo, "20<p_{T}<30");

  zgLead3040Lo->SetLineColor(kRed);
  zgLead3040Lo->Draw("same");
  leg->AddEntry( zgLead3040Lo, "30<p_{T}<40");

  // zgLead4060Lo->SetLineColor(kGreen+1);
  // zgLead4060Lo->Draw("same");
  // leg->AddEntry( zgLead4060Lo, "40<p_{T}<60");

  FUVQjet->Draw("same");
  leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  gPad->SaveAs( "Plots/" + outbase + "_MatchedLeadZgLo.png");

  // ===================================================================
  // ========================= SUBLEADING ==============================
  // ===================================================================
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "Hi Cut, SubLeading jet" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  zgSubLead1020Hi->Scale ( 1./ zgSubLead1020Hi->Integral() / zgSubLead1020Hi->GetXaxis()->GetBinWidth(1));
  zgSubLead2030Hi->Scale ( 1./ zgSubLead2030Hi->Integral() / zgSubLead2030Hi->GetXaxis()->GetBinWidth(1));
  zgSubLead3040Hi->Scale ( 1./ zgSubLead3040Hi->Integral() / zgSubLead3040Hi->GetXaxis()->GetBinWidth(1));
  zgSubLead4060Hi->Scale ( 1./ zgSubLead4060Hi->Integral() / zgSubLead4060Hi->GetXaxis()->GetBinWidth(1));
    
  zgSubLead1020Hi->SetTitle(";z_{g};arb.");
  zgSubLead1020Hi->SetAxisRange(0,10,"y");

  zgSubLead2030Hi->SetTitle(";z_{g};arb.");
  zgSubLead2030Hi->SetAxisRange(0,10,"y");

  zgSubLead1020Hi->SetLineColor(kTeal);
  zgSubLead1020Hi->Draw("");
  leg->AddEntry( zgSubLead1020Hi, "10<p_{T}<20");

  zgSubLead2030Hi->SetLineColor(kBlack);
  zgSubLead2030Hi->Draw("same");
  leg->AddEntry( zgSubLead2030Hi, "20<p_{T}<30");

  // zgSubLead3040Hi->SetLineColor(kRed);
  // zgSubLead3040Hi->Draw("same");
  // leg->AddEntry( zgSubLead3040Hi, "30<p_{T}<40");

  // zgSubLead4060Hi->SetLineColor(kGreen+1);
  // zgSubLead4060Hi->Draw("same");
  // leg->AddEntry( zgSubLead4060Hi, "40<p_{T}<60");

  FUVQjet->Draw("same");
  leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  gPad->SaveAs("Plots/" + outbase + "_MatchedSubLeadZgHi.png");

  // =========================== Draw Lo ===============================
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TLegend* leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "Lo Cut, SubLeading jet" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  zgSubLead1020Lo->Scale ( 1./ zgSubLead1020Lo->Integral() / zgSubLead1020Lo->GetXaxis()->GetBinWidth(1));
  zgSubLead2030Lo->Scale ( 1./ zgSubLead2030Lo->Integral() / zgSubLead2030Lo->GetXaxis()->GetBinWidth(1));
  zgSubLead3040Lo->Scale ( 1./ zgSubLead3040Lo->Integral() / zgSubLead3040Lo->GetXaxis()->GetBinWidth(1));
  zgSubLead4060Lo->Scale ( 1./ zgSubLead4060Lo->Integral() / zgSubLead4060Lo->GetXaxis()->GetBinWidth(1));
    
  zgSubLead1020Lo->SetTitle(";z_{g};arb.");
  zgSubLead1020Lo->SetAxisRange(0,10,"y");

  zgSubLead2030Lo->SetTitle(";z_{g};arb.");
  zgSubLead2030Lo->SetAxisRange(0,10,"y");

  zgSubLead1020Lo->SetLineColor(kTeal);
  zgSubLead1020Lo->Draw("");
  leg->AddEntry( zgSubLead1020Lo, "10<p_{T}<20");

  zgSubLead2030Lo->SetLineColor(kBlack);
  zgSubLead2030Lo->Draw("same");
  leg->AddEntry( zgSubLead2030Lo, "20<p_{T}<30");

  // zgSubLead3040Lo->SetLineColor(kRed);
  // zgSubLead3040Lo->Draw("same");
  // leg->AddEntry( zgSubLead3040Lo, "30<p_{T}<40");

  // zgSubLead4060Lo->SetLineColor(kGreen+1);
  // zgSubLead4060Lo->Draw("same");
  // leg->AddEntry( zgSubLead4060Lo, "40<p_{T}<60");

  FUVQjet->Draw("same");
  leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  gPad->SaveAs("Plots/" + outbase + "_MatchedSubLeadZgLo.png");

  FUVQjet->Write();
  fOut->Write();

  // new TCanvas; hLeadPtHi->Draw();
  // new TCanvas; hSubLeadPtHi->Draw();
  // new TCanvas; hLeadPtLo->Draw();
  // new TCanvas; hSubLeadPtLo->Draw();


  new TCanvas; AJ_hi->ProjectionX()->Draw();
  new TCanvas; AJ_lo->ProjectionX()->Draw();

  cout << "Found " << nDijets << " 20/10 pairs" << endl;
  return;
  
}
// ===========================================================================
double CalcAj ( TLorentzVector* j1, TLorentzVector* j2 ){
  return (( j1->Pt()-j2->Pt() ) / ( j1->Pt()+j2->Pt() ));
}
// ===========================================================================
    
