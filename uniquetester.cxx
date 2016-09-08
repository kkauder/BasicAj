#include <TFile.h>
#include <TChain.h>
#include <TTree.h>

#include <map>
#include <iostream>
#include <climits>
using namespace std;

typedef map< pair<int,int>,int> mpi;
int uniquetester(){
  // TFile *f = TFile::Open("AjResults/Groom_Aj_HT54_NoEff_GeantMc.root");
  // TFile *f = TFile::Open("AjResults/Tow0_Eff0_Groom_Aj_HT54_Geant.root");
  // TFile *f = TFile::Open("AjResults/Tow0_Eff0_Groom_Aj_HT54_GeantInAuAuAj.root");

  // TFile *f = TFile::Open("HThistos/Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root");
  // TFile *f = TFile::Open("HThistos/Groom_Aj_TrueMB_NoEff_GeantMc.root");
  // TFile *f = TFile::Open("delme.root");
  
  
  // TTree* Tree = (TTree*) f->Get("Tree");
  // TTree* Tree = (TTree*) f->Get("TriggeredTree");

  TChain *Tree = new TChain("TriggeredTree");
  // Tree->Add("AjResults/Pieces/BetterIdGroom_Aj_TrueMB_NoEff_GeantMc_picoDst_9_11*");
  Tree->Add("AjResults/Groom_Aj_TrueMB_NoEff_GeantMc.root");

  int runid;
  Tree->SetBranchAddress("runid", &runid );
  int eventid;
  Tree->SetBranchAddress("eventid", &eventid );

  map<Long64_t,int> m;
  mpi m2;
  pair<int,int> p;

  // for ( int evi=0; evi<100; ++evi ){
  for ( int evi=0; evi<Tree->GetEntries(); ++evi ){
    Tree->GetEntry(evi);
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
