int EmbeddingVisualizer(  ){

  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);

  // HT: Use 57 as target, 56 as source.
  // MB: 56 works. 
  
  gStyle->SetOptStat(0);
  TCanvas* cHT = new TCanvas("cHT");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  TCanvas* cMB = new TCanvas("cMB");
  gPad->SetGridx(0);  gPad->SetGridy(0);

  TString AuAuHT="HelperFiles/Kresult.root"; 
  TString AuAuMB="HelperFiles/LorentzMB_Top20.root"; 

  TFile* fHT = new TFile( AuAuHT, "READ");
  TTree* HTTree = fHT->Get( "TriggeredTree" );
  HTTree->SetName("HTTree");

  TFile* fMB = new TFile( AuAuMB, "READ");
  TTree* MBTree = fMB->Get( "TriggeredTree" );
  MBTree->SetName("MBTree");

  TClonesArray* pFullEvent = new TClonesArray("TLorentzVector");
  float pt;
  const float R=0.4;
  
  // Load HT event
  // -------------
  HTTree->GetBranch("FullEvent")->SetAutoDelete(kFALSE);
  HTTree->SetBranchAddress("FullEvent", &pFullEvent);

  TH2D* hHT = new TH2D("hHT",";#phi;#eta", 120, -TMath::Pi(), TMath::Pi(), 40, -1, 1);
  int HTevent=57;

  
  HTTree->GetEntry(HTevent);

  for ( int i=0; i<pFullEvent->GetEntries() ; ++i ){
    current = (TLorentzVector*) pFullEvent->At(i);
    pt=current->Pt();
    
    hHT->Fill( current->Phi(), current->Eta(), pt );    
  }
  
  cHT->cd();
  hHT->SetAxisRange( 0, 18, "z");
  hHT->Draw("lego2");


  // Select random positions
  // =======================
  int LeadPhiBin=27;
  int LeadEtaBin=27;
  int SubLeadPhiBin=89;
  int SubLeadEtaBin=9;
  float LeadPhi = hHT->GetXaxis()->GetBinCenter(LeadPhiBin);
  float LeadEta = hHT->GetYaxis()->GetBinCenter(LeadEtaBin);
  float SubLeadPhi = hHT->GetXaxis()->GetBinCenter(SubLeadPhiBin);
  float SubLeadEta = hHT->GetYaxis()->GetBinCenter(SubLeadEtaBin);
  
  cout << LeadEta << endl;
  cout << SubLeadEta << endl;

  // Color in jet are around the new position
  // In reality chosen randomly, here just realistically chosen by hand
  float NewLeadEta = -0.5; 
  float NewSubLeadEta = 0.3;

  TH2D* hEC = hHT->Clone("hEC");
  for ( int i=1; i <= hEC->GetNbinsX() ; ++i ){
    float phi = hEC->GetXaxis()->GetBinCenter(i);
    for ( int j=1; j <= hEC->GetNbinsY() ; ++j ){
      float eta = hEC->GetYaxis()->GetBinCenter(j);
      cout << endl;
      if ( pow(phi-LeadPhi,2)    + pow(eta-NewLeadEta,2)    <= R*R ) {
	if ( hEC->GetBinContent(i,j,0)< 1e-2 )       hEC->SetBinContent(i,j,1e-2); // fill holes for visual
	continue; // Keep leading
      }
      if ( pow(phi-SubLeadPhi,2) + pow(eta-NewSubLeadEta,2) <= R*R ) {
	if ( hEC->GetBinContent(i,j,0)< 1e-2 )       hEC->SetBinContent(i,j,1e-2); // fill holes for visual
	continue; // Keep Subleading
      }
      
      hEC->SetBinContent(i,j,0);
      hEC->SetBinError(i,j,0);
     
    }
  }
  hEC->SetFillColor(kMagenta);
  hEC->Draw("lego1,0 same");

  gPad->SaveAs("plots/EmbeddingHT.png");
  
  // Load MB event
  // -------------
  MBTree->GetBranch("FullEvent")->SetAutoDelete(kFALSE);
  MBTree->SetBranchAddress("FullEvent", &pFullEvent);

  TH2D* hMB = new TH2D("hMB",";#phi;#eta", 120, -TMath::Pi(), TMath::Pi(), 40, -1, 1);
  int MBevent=57;
  
  MBTree->GetEntry(MBevent);

  for ( int i=0; i<pFullEvent->GetEntries() ; ++i ){
    current = (TLorentzVector*) pFullEvent->At(i);
    pt=current->Pt();
    
    hMB->Fill( current->Phi(), current->Eta(), pt );    
  }
  
  cMB->cd();
  hMB->SetAxisRange( 0, 18, "z");
  hMB->Draw("lego2");


  float LeadPhi = -1;
  float SubLeadPhi = LeadPhi+TMath::Pi();

  // Color in jet are around the new position
  // In reality chosen randomly, here just realistically chosen by hand
  float NewLeadEta = 0.5; 
  float NewSubLeadEta = 0.1;

  TH2D* hRC = hMB->Clone("hRC");
  for ( int i=1; i <= hRC->GetNbinsX() ; ++i ){
    float phi = hRC->GetXaxis()->GetBinCenter(i);
    for ( int j=1; j <= hRC->GetNbinsY() ; ++j ){
      float eta = hRC->GetYaxis()->GetBinCenter(j);
      cout << endl;
      if ( pow(phi-LeadPhi,2)    + pow(eta-NewLeadEta,2)    <= R*R ) {
	if ( hRC->GetBinContent(i,j,0)< 1e-2 )       hRC->SetBinContent(i,j,1e-2); // fill holes for visual
	continue; // Keep leading
      }
      if ( pow(phi-SubLeadPhi,2) + pow(eta-NewSubLeadEta,2) <= R*R ) {
	if ( hRC->GetBinContent(i,j,0)< 1e-2 )       hRC->SetBinContent(i,j,1e-2); // fill holes for visual
	continue; // Keep Subleading
      }
      
      hRC->SetBinContent(i,j,0);
      hRC->SetBinError(i,j,0);
     
    }
  }
  hRC->SetFillColor(kMagenta);
  hRC->Draw("lego1,0 same");

  gPad->SaveAs("plots/EmbeddingMB.png");
  return 0;
}
