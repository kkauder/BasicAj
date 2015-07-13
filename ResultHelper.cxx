int ResultHelper ( TString in="AjResults/Presel_AuAuAj_Small_Clean810_0*" ){
  TChain * t = new TChain ( "ResultTree");
  t->Add ( in );
  int eventid;
  t->SetBranchAddress ( "eventid", &eventid );
  int runid;
  t->SetBranchAddress ( "runid", &runid );
  double refmult;
  t->SetBranchAddress ( "refmult", &refmult );
  TLorentzVector *pJ1 = new TLorentzVector();
  t->SetBranchAddress("j1", &pJ1);
  TLorentzVector *pJ2 = new TLorentzVector();
  t->SetBranchAddress("j2", &pJ2);
  
  for ( int i =0; i< t->GetEntries() ; ++i ){
    t->GetEvent(i);
    // cout << runid << "  " << eventid << endl;

    if (
	eventid==424 ||
	eventid==3216 ||
	eventid==618 ||
	eventid==6802 ||
	eventid==7687 ||
	eventid==15486 ||
	eventid==9437 ||
	eventid==47527 ||
	eventid==16189 ||
	eventid==12733 ||
	eventid==24191 ||
	eventid==3135 ||
	eventid==4474 ||
	eventid==5553 ){
      // cout << runid << "  " << eventid << "  " << pJ2->Pt() << endl;
      cout << runid << "  " << eventid << "  " << refmult << endl;
    }
      
	
    
  }
  

}
