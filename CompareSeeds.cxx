void CompareSeeds(){

  gStyle->SetOptStat(0);  
  gStyle->SetOptFit(1);  

  int RefmultCut = 269;  // 269 for 0-20%, 399 for 0-10%

  // TString s1 = "AjResults/test.root";
  // TString s2 = "AjResults/rndm1/test.root";

  // TString s1 = "test.root";
  // TString s1 = "AjResults/rndm1/newtest.root";
  // TString s2 = "AjResults/rndm2/newtest.root";

  // TString s1 = "AjResults/testSmall.root";
  // TString s2 = "AjResults/rndm1/testSmall.root";

  TString s1 = "AjResults/rndm1/HP_TEST.root";
  TString s2 = "AjResults/rndm2/HP_TEST.root";

  // TString s1 = "AjResults/testsg.root";
  // TString s2 = "AjResults/rndm1/testsg.root";

  TFile* f1 = new TFile( s1 );
  TFile* f2 = new TFile( s2 );

  TH2D* h2 = (TH2D*) f1->Get( "AJ_lo");
  int AuAuMultBinL = h2->GetYaxis()->FindBin( RefmultCut );
  int AuAuMultBinR = h2->GetNbinsY();
  
  TH1D* p1 = ((TH2D*)f1->Get( "AJ_lo" ))->ProjectionX("p1", AuAuMultBinL, AuAuMultBinR);
  p1->SetLineColor(kRed);
  TH1D* p2 = ((TH2D*)f2->Get( "AJ_lo" ))->ProjectionX("p2", AuAuMultBinL, AuAuMultBinR);
  p2->SetLineColor(kBlue);

  // TCanvas* c = new TCanvas("c");
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // p1->Draw();
  // p2->Draw("same");
  

  TChain* t1 = new TChain ( "ResultTree" );
  t1->Add ( s1 );
  double refmult;
  t1->SetBranchAddress ( "refmult", &refmult );

  int eventid1;
  t1->SetBranchAddress ( "eventid", &eventid1 );
  int runid1;
  t1->SetBranchAddress ( "runid", &runid1 );
  TLorentzVector *t1J1 = new TLorentzVector();
  t1->SetBranchAddress("jm1", &t1J1);
  TLorentzVector *t1J2 = new TLorentzVector();
  t1->SetBranchAddress("jm2", &t1J2);
  float rho1;
  t1->SetBranchAddress("rho", &rho1);
  float j1area1;
  t1->SetBranchAddress("j1area", &j1area1);
  float j2area1;
  t1->SetBranchAddress("j2area", &j2area1);

  if ( s1=="test.root"){
    int nRestJ1;
    t1->SetBranchAddress("nRestJ", &nRestJ1 );
    float RestArea1[500];
    t1->SetBranchAddress("RestArea", RestArea1 );
    float RestPt1[500];
    t1->SetBranchAddress("RestPt", RestPt1 );
  }
  
  TChain* t2 = new TChain ( "ResultTree" );
  t2->Add ( s2 );
  int eventid2;
  t2->SetBranchAddress ( "eventid", &eventid2 );
  int runid2;
  t2->SetBranchAddress ( "runid", &runid2 );
  TLorentzVector *t2J1 = new TLorentzVector();
  t2->SetBranchAddress("jm1", &t2J1);
  TLorentzVector *t2J2 = new TLorentzVector();
  t2->SetBranchAddress("jm2", &t2J2);
  float rho2;
  t2->SetBranchAddress("rho", &rho2);
  float j1area2;
  t2->SetBranchAddress("j1area", &j1area2);
  float j2area2;
  t2->SetBranchAddress("j2area", &j2area2);

  // int nRestJ2;
  // t2->SetBranchAddress("nRestJ", &nRestJ2 );
  // float RestArea2[500];
  // t2->SetBranchAddress("RestArea", RestArea2 );
  // float RestPt2[500];
  // t2->SetBranchAddress("RestPt", RestPt2 );
  
  t2->BuildIndex("runid","eventid");

  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH2D* VardiffJ1 = new TH2D("VardiffJ1", "#Delta p_{T,1} for different seeds;p_{T,2};#Delta p_{T,1}", 100, 0, 100, 100, -10, 10);
  TH2D* diffJ1 = new TH2D("diffJ1", "#Delta p_{T,1} for different seeds;p_{T,1};#Delta p_{T,1}", 100, 0, 100, 100, -10, 10);
  TH2D* diffJ2 = new TH2D("diffJ2", "#Delta p_{T,2} for different seeds;p_{T,2};#Delta p_{T,2}", 100, 0, 100, 100, -10, 10);

  TH2D* rho = new TH2D("rho", "#rho;#rho1;#rho2", 100, 0, 100, 100, 0, 100);
  TH1D* diffrho = new TH1D("diffrho", "#Delta #rho for different seeds;#Delta #rho", 100, -10, 10);
  TH1D* diffj1area = new TH1D("diffj1area", "#Delta j1area for different seeds;#Delta A_{1}", 100, -0.15, 0.15);
  TH1D* diffj2area = new TH1D("diffj2area", "#Delta j2area for different seeds;#Delta A_{2}", 100, -0.15, 0.15);
  TH1D* diffRhoA1  = new TH1D("diffRhoA1", "#Delta #rho A1 for different seeds;#Delta (#rho A_{1})", 100, -10, 10);
  TH1D* diffRhoA2  = new TH1D("diffRhoA2", "#Delta #rho A2 for different seeds;#Delta (#rho A_{2})", 100, -10, 10);

  // are leading jet area fluctuations (anti-)correlated with rho fluctuations?
  TH2D* DA1vsDRho = new TH2D("DA1vsDRho", "#Delta A_{1} vs. #Delta #rho for different seeds;#Delta #rho;#Delta A_{1};",
			     100, -10, 10, 100, -0.15, 0.15);
  TH2D* DA2vsDRho = new TH2D("DA2vsDRho", "#Delta A_{2} vs. #Delta #rho for different seeds;#Delta #rho;#Delta A_{2};",
			     100, -10, 10, 100, -0.15, 0.15);
  
  
  Long64_t nEvents=t1->GetEntries();
  for (Long64_t ev=0; ev<nEvents; ++ev ){
    t1->GetEntry(ev);

    if ( refmult<RefmultCut ) continue;
    
    t2->GetEntryWithIndex(runid1, eventid1);
    // not sure what happens if it doesn't find it. Let's play it safe
    if ( runid1!=runid2 || eventid1!=eventid2 ) continue;
    if ( rho1==0 ) continue;
    
    diffJ1->Fill( t1J1->Pt() , t1J1->Pt() -t2J1->Pt() );
    VardiffJ1->Fill( t2J1->Pt() , t1J1->Pt() -t2J1->Pt() );
    diffJ2->Fill( t1J2->Pt() , t1J2->Pt() -t2J2->Pt() );

    rho->Fill( rho1,rho2 );
    diffrho->Fill( rho1-rho2 );
    diffj1area->Fill( j1area1-j1area2 );
    diffj2area->Fill( j2area1-j2area2 );

    diffRhoA1->Fill( (j1area1*rho1) - (j1area2*rho2) );
    diffRhoA2->Fill( (j2area1*rho1) - (j2area2*rho2) );

    DA1vsDRho->Fill( rho1-rho2, j1area1-j1area2 );
    DA2vsDRho->Fill( rho1-rho2, j2area1-j2area2 );

    if ( s1=="test.root"){
      float * ratio1 = new float[nRestJ1];
      for ( int i=0 ; i<nRestJ1 ; i++ ){
	ratio1[i]=RestPt1[i] / RestArea1[i];
      }
      cout << TMath::Median ( nRestJ1, RestPt1 ) << endl;
      cout << TMath::Median ( nRestJ1, RestArea1 ) << endl; 
      cout << TMath::Median ( nRestJ1, RestPt1 ) / TMath::Median ( nRestJ1, RestArea1 ) << endl;  
      cout << TMath::Median ( nRestJ1, ratio1 ) << endl; 
      cout << rho1 << endl << endl;;
      delete ratio1;
    }

}
  
  
  new TCanvas("cJ1");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  diffJ1->ProjectionY("pdJ1")->Draw();
  pdJ1->Fit("gaus");
  
  // new TCanvas("cJ2");
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // diffJ2->ProjectionY("pdJ2")->Draw();
  // pdJ2->Fit("gaus");

  new TCanvas("cdrho");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  diffrho->Draw();
  diffrho->Fit("gaus");

  // new TCanvas;
  // rho->Draw("colz");

  // new TCanvas;
  // gStyle->SetOptStat(1);  
  // rho->ProjectionX()->Draw("");
  
  new TCanvas("cdj1area");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  diffj1area->Draw();
  diffj1area->Fit("gaus");

  // new TCanvas("cdj2area");
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // diffj2area->Draw();
  // diffj2area->Fit("gaus");

  // new TCanvas("cdrhoa1");
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // diffRhoA1->Draw();
  // diffRhoA1->Fit("gaus");

  // new TCanvas("cdrhoa2");
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // diffRhoA2->Draw();
  // diffRhoA2->Fit("gaus");

  // new TCanvas("cDcorr1");
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // DA1vsDRho->Draw("colz");
  cout <<  "DA1vsDRho->GetCorrelationFactor() = " <<  DA1vsDRho->GetCorrelationFactor() << endl;

  // new TCanvas("cDcorr2");
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // DA2vsDRho->Draw("colz");
  cout <<  "DA2vsDRho->GetCorrelationFactor() = " <<  DA2vsDRho->GetCorrelationFactor() << endl;

  // new TCanvas;
  // diffJ1->ProjectionX("pt1")->Draw();
  // VardiffJ1->ProjectionX("pt2")->Draw("same");
  // pt2->SetLineColor(kRed);
  // pt2->Draw("same");
  

  return 0;
}
