void VisualizeSelection(int event=0){
  gStyle->SetOptStat(0);
  gStyle->SetOptDate(0);
  
  TChain* TriggeredTree = new TChain("TriggeredTree");
  // TriggeredTree->Add("AjResults/ppAj.root");
  TriggeredTree->Add("AjResults/VarAuAuAj.root");
  
  TClonesArray* pFullEvent = new TClonesArray("TLorentzVector");
  TriggeredTree->GetBranch("FullEvent")->SetAutoDelete(kFALSE);
  TriggeredTree->SetBranchAddress("FullEvent", &pFullEvent);

  TClonesArray* pTriggerJet = new TClonesArray("TLorentzVector");
  TriggeredTree->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  TriggeredTree->SetBranchAddress("TriggerJet", &pTriggerJet);

  // for (int e=0; e<TriggeredTree->GetEntries(); ++e){
  //   TriggeredTree->GetEntry(e);
  //   TLorentzVector* lead = (TLorentzVector*) pTriggerJet->At(0);
  //   if ( lead->Pt()>40 && lead->Pt()<50 ) cout << e << "  " << lead->Pt() << endl;
  // }
    

  TriggeredTree->GetEntry(event);
  
  TH2D* hLo = new TH2D("hLo",";#pi;#eta;p_{T}",60, -TMath::Pi(), TMath::Pi(), 50, -2, 2);
  TH2D* hHi = new TH2D("hHi",";#pi;#eta;p_{T}",60, -TMath::Pi(), TMath::Pi(), 50, -2, 2);
  for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
    TLorentzVector* p = (TLorentzVector*) pFullEvent->At(i);
    hLo->Fill ( p->Phi(), p->Eta(), p->Pt() );
    if ( p->Pt()>2 ){
      hHi->Fill ( p->Phi(), p->Eta(), p->Pt() );
    }
    
  }
  
  TLorentzVector* lead = (TLorentzVector*) pTriggerJet->At(0);
  TEllipse* eLead = new TEllipse(lead->Phi(), lead->Eta(), 0.4, 0.4);
  cout << lead->Pt() << endl;

  eLead->SetFillStyle(0);
  eLead->SetLineWidth(6);

  new TCanvas;
  gPad->SetRightMargin(0.15);
  gPad->SetBottomMargin(0.15);
  gPad->SetGridx(0);  gPad->SetGridy(0);    
  hLo->Draw("colz");
  eLead->SetLineColor(kCyan);
  eLead->DrawClone();
  gPad->SaveAs("plots/VisLargeRlo.png");
  
  new TCanvas;
  gPad->SetRightMargin(0.15);
  gPad->SetBottomMargin(0.15);
  gPad->SetGridx(0);  gPad->SetGridy(0);  
  hHi->Draw("colz");
  eLead->SetLineColor(kRed);
  eLead->DrawClone();
  gPad->SaveAs("plots/VisLargeRhi.png");  
}
