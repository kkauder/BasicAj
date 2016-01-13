//----------------------------------------------------------------------
// Make a simple AuAu background for playing around
// run it with    : ./bin/JustMc
// random seed is an optional argument

#include "Pythia.h"

#include "TStarJetVector.h"

#include <iostream>
#include <string>
#include <sstream>

#include "TROOT.h"
#include "TRandom.h"
#include "TMath.h"
#include "TString.h"
#include "TObjString.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TParameter.h"

using namespace std;
using namespace Pythia8;

Int_t main(int argc, char **argv) {
    
  TString OutFileName = "Data/FakeAuAu20.root";
  TString seed = "";
  Long64_t pseed = 1;
  if ( argc>1 ) {
    seed = argv[1];
    pseed = atoi( seed );
    OutFileName.ReplaceAll ( ".root", "_" + seed + ".root" );
  }
  cout << "Writing to: " << OutFileName << endl;

  // Multiplicity -- pull before opening outfile
  // -------------------------------------------
  TFile* fMult = new TFile("TrueMult_0_20.root","READ");
  TH1D* AuAuN = (TH1D*) fMult->Get("AuAuN"); // Maybe reject N<800 or so

  // Float_t maxeta = 1.0;  // STAR
  Float_t maxeta = 3.0;
  
  gRandom->SetSeed(pseed);
    
  // Set up output file
  // ------------------
  TFile * fout = new TFile(OutFileName,"recreate");

  // Set up tree
  // -----------
  static const Int_t kmaxT=25000; // # of particles per event

  TTree* tree  = new TTree("tree","Fake AuAu events");

  TClonesArray McParticles("TStarJetVector",kmaxT);
  tree->Branch("McParticles", &McParticles);

  // also give a fake run and event id to seed the RNG
  int runid;
  int eventid;
  tree->Branch("runid",&runid, "runid/i");
  tree->Branch("eventid",&eventid, "eventid/i");

  // pT distribution
  // ---------------
  TF1* fExpo = new TF1("fExpo" ,"expo",0.2,10);
  fExpo->SetParameters( 1, -2.9);
  //  fExpo->SetParameters( 1, -2.6711);
  
  TH1D* AuAuSoftPt = new TH1D("AuAuSoftPt","AuAuSoftPt", 100, 0.2, 10);
  AuAuSoftPt->Eval( fExpo ); // Easier to draw from histo
  
  // Generate events
  // ---------------
  int nEv = 0;
  Int_t nEvent = 100000;
  // Int_t nEvent = 500;
  // fastjet::PseudoJet v; // helper
  // const double M = 0.13957;  // set pi+ mass
  const double M = 0;  // for background, use massless particles, as in real data

  double E;
  float pt, rap, phi, p;
  Vec4 pv;
  do {
    McParticles.Clear();
    
    // Only create mock Au+Au background.
    // -----------------------------------

    // Multiplicity
    int N;
    while ( (N=AuAuN->GetRandom())<800);
    // Scale up by rapidity coverage
    N*=maxeta;

    // // Need to shift down
    // N-=PythiaParticles.GetEntries();    
    
    int nAuAu=0;
    while ( nAuAu<N ){
      pt = AuAuSoftPt->GetRandom();
      rap = gRandom->Uniform(-maxeta,maxeta);
      phi = gRandom->Uniform(-TMath::Pi(),TMath::Pi());      
      
      double p = pt * TMath::CosH( rap );
      double E = sqrt( p*p + M*M);
      double px = pt * TMath::Cos( phi );
      double py = pt * TMath::Sin( phi );
      double pz = pt * TMath::SinH( rap );

      int charge =  int( gRandom->Integer(3) ) -1;

      pv = Vec4( px, py, pz, E );
      if ( pv.pT() < 0.2 ) continue;
      if ( fabs(pv.eta()) > maxeta ) continue;
      
      // Use ConstructedAt!! new defeats the purpose of tclonesarray (and swamps the memory)
      // new ( McParticles[nAuAu] ) TStarJetVector () ; 
      TStarJetVector* ppart = (TStarJetVector*) McParticles.ConstructedAt(nAuAu);
      if ( charge !=0 ){
      	ppart->SetPtEtaPhiM( pv.pT(), pv.eta(), pv.phi(), 0);
      	ppart->SetType(TStarJetVector::_TRACK);
      	ppart->SetCharge( charge );
      } else {
      	ppart->SetPtEtaPhiM( pv.eT(), pv.eta(), pv.phi(), 0);
      	ppart->SetType(TStarJetVector::_TOWER);
      	ppart->SetCharge(TStarJetVector::_NEUTRAL);
      }

      nAuAu++;
      
    }
    
    // generate fake runid and eventid
    // -------------------------------
    runid   = gRandom->Integer(1000000);
    eventid = gRandom->Integer(1000000);

    // Done. Fill tree
    // ---------------
    tree->Fill();
    
    ++nEv;
  } while (nEv<nEvent);
  
  fout->Write();
  fout->Close();


  cout << "Written to: " << fout->GetName() << endl;
  return 0;
}
