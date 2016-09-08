#include <TFile.h>
#include <TTree.h>

#include <map>
#include <iostream>
#include <climits>
using namespace std;

typedef map< pair<int,int>,int> mpi;
int uniquetester(){
  // TFile *f = TFile::Open("AjResults/Groom_Aj_HT54_NoEff_GeantMc.root");
  // TFile *f = TFile::Open("AjResults/Tow0_Eff0_Groom_Aj_HT54_Geant.root");
  TFile *f = TFile::Open("AjResults/Tow0_Eff0_Groom_Aj_HT54_GeantInAuAuAj.root");

  TTree* ResultTree = (TTree*) f->Get("ResultTree");

  int runid;
  ResultTree->SetBranchAddress("runid", &runid );
  int eventid;
  ResultTree->SetBranchAddress("eventid", &eventid );

  map<Long64_t,int> m;
  mpi m2;
  pair<int,int> p;

  // for ( int evi=0; evi<100; ++evi ){
  for ( int evi=0; evi<ResultTree->GetEntries(); ++evi ){
    ResultTree->GetEntry(evi);
    // cout << runid << "  " << eventid << endl;
    Long64_t evrun = Long64_t(eventid+1)*Long64_t(runid);
    m[evrun]++;
    p.first=runid;
    p.second=eventid;
    m2[p]++;
  }
  // cout << "Done filling, now searching";
  // for ( map<Long64_t,int>::iterator it=m.begin(); it!=m.end(); it++ ){
  //   if ( it->second != 1 ){
  //     cout << "ALARUM WITHIN! " << it->first << "  " << it->second << endl;
  //   }
  // }
  
  for ( mpi::iterator it=m2.begin(); it!=m2.end(); it++ ){
    if ( it->second != 1 ){
      cout << "ALARUM WITHIN! " << it->first.first << "  " << it->first.second << "  " << it->second << endl;
    }
  }

  return 0;
}
