int EventVisualizer(  ){

  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);

  gStyle->SetOptStat(0);
  TCanvas* csoft = new TCanvas("csoft");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TCanvas* chard = new TCanvas("chard");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  int event=0;

  // Au+Au HT. Older result, but it has a full result tree.
  // Use 56, 57 or 66
  // HP: 57
  // TString in="HelperFiles/Kresult.root"; 
    
  // pp.
  // Use 134, 40, or 61
  // TString in="AjResults/Tow0_Eff0_ppAj.root";

  // Au+Au MB
  // Use pretty much any
  TString in="HelperFiles/LorentzMB_Top20.root";

  TString out=in;
  out.ReplaceAll("AjResults/","");
  out.ReplaceAll(".root",".Visual.root");

  TFile* fin = new TFile( in, "READ");
  TTree* TriggeredTree = fin->Get( "TriggeredTree" );

  TClonesArray* pFullEvent = new TClonesArray("TLorentzVector");
  TriggeredTree->GetBranch("FullEvent")->SetAutoDelete(kFALSE);
  TriggeredTree->SetBranchAddress("FullEvent", &pFullEvent);
  
  // TClonesArray* pTriggerJet = new TClonesArray("TLorentzVector");
  // TriggeredTree->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  // TriggeredTree->SetBranchAddress("TriggerJet", &pTriggerJet);
  
  TH2D* hsoft = new TH2D("hsoft",";#phi;#eta", 120, -TMath::Pi(), TMath::Pi(), 40, -1, 1);
  TH2D* hhard = new TH2D("hhard",";#phi;#eta", 120, -TMath::Pi(), TMath::Pi(), 40, -1, 1);
  
  float pt=0;
  TLorentzVector* current=0; 
  TLorentzVector* trigger=0;
  
  char dummy[10];

  // for ( event=0; event < TriggeredTree->GetEntries() ; ++event ){
  //   if ( event != 13 &&
  //   	 event != 21 && 
  //   	 event != 37 && 
  //   	 event != 54 && 
  //   	 event != 56 && // This is good
  //   	 event != 57 && 
  //   	 event != 66    // Also good
  //   	 ) continue;
    
  // Au+Au HT:
  // Use 56, 57 or 66
  // pp:
  // Use 134, 40, or 61
  // Au+Au MB:
  // Whatever

  event= 57; {
    TriggeredTree->GetEntry(event);

    // trigger =  (TLorentzVector*) pTriggerJet->At(0);
    // cout << trigger->Pt() << endl;
    // if ( trigger->Pt() < 20 ) continue;

    cout << event << endl;
    hsoft->Reset();
    hhard->Reset();
    
    for ( int i=0; i<pFullEvent->GetEntries() ; ++i ){
      current = (TLorentzVector*) pFullEvent->At(i);
      pt=current->Pt();
      
      if ( pt>0.2 )    hsoft->Fill( current->Phi(), current->Eta(), pt );
      if ( pt>2.0 )    hhard->Fill( current->Phi(), current->Eta(), pt );
      
    }
    
    csoft->cd();
    hsoft->SetAxisRange( 0, 16, "z");
    hsoft->Draw("lego2");
    // hsoft->Draw("lego2,0");
    // hsoft->Draw("lego same");
    // hsoft->DrawCopy("lego");
    // hsoft->SetFillColor(kRed);
    // hsoft->Draw("lego1,0 same");
    
    chard->cd();
    hhard->Draw("lego2,0");
    hhard->Draw("lego same");

    csoft->Update();
    chard->Update();
    gets(dummy);
    
  }

  TFile* fout = new TFile(out, "RECREATE");
  hsoft->Write();
  hhard->Write();
  
  
  
  return 0;
}
