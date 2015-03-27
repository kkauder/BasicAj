//----------------------------------------------------------------------
// Make a simple pythia tree for playing around
// run it with    : ./bin/SimpleTree
///

#include "Pythia.h"

#include <iostream>
#include <string>
#include <sstream>

#include "TROOT.h"
#include "TMath.h"
#include "TString.h"
#include "TObjString.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

using namespace std;
using namespace Pythia8;

Int_t main(int argc, char **argv) {

  // Set up output file
  TFile * fout = new TFile("pytest40.root","recreate");
  
  // Generator. Shorthand for event (use FastJet3 and Pythia8 interface!!!)
  //------------------------------------------------------------------------
  Float_t ptHatMin=40.0;
  Float_t ptHatMax=100.0;
  // Float_t ptHatMin=20.0;
  // Float_t ptHatMax=60.0;

  Pythia pythia("/Users/putschke/pythia8100/xmldoc");
  Event& event = pythia.event;

  // Process selection.
  pythia.readString("HardQCD:all = on");  
  pythia.readString("HadronLevel:Decay = on");
  pythia.readString("PartonLevel:ISR = on");
  pythia.readString("PartonLevel:FSR = on");

  stringstream numbf(stringstream::app|stringstream::in|stringstream::out);
  numbf.setf(ios::fixed, ios::floatfield);
  numbf.setf(ios::showpoint);
  numbf.precision(1);
  numbf.str("PhaseSpace:pTHatMin = "); numbf << ptHatMin;
  string pmin=numbf.str();
  numbf.str("PhaseSpace:pTHatMax = "); numbf << ptHatMax;
  string pmax=numbf.str();

  cout<<pmin<<endl;
  cout<<pmax<<endl;

  pythia.readString(pmin);  
  pythia.readString(pmax);

  // No event record printout.
  pythia.readString("Next:numberShowInfo = 0"); 
  pythia.readString("Next:numberShowProcess = 0"); 
  pythia.readString("Next:numberShowEvent = 0"); 

  // for pPb
  //pythia.readString("Tune:pp = 5");
  //pythia.readString("SigmaProcess:Kfactor = 0.7");

  //random seed
  //pythia.readString("Random:setSeed = on");
  //pythia.readString("Random:seed = 0");

  // LHC initialization.
  //pythia.init(2212, 2212,5020.);
  // pythia.init(2212, 2212,2760.);

  // RHIC initialization.
  pythia.init(2212, 2212,200.);


  // Set up tree
  // -----------
  static const Int_t kmaxT=5000; // # of particles per event

  TTree* tree  = new TTree("tree","Some pythia jets");
  TClonesArray Particles("TLorentzVector",kmaxT);
  tree->Branch("Particles", &Particles);

  // The original outgoing hard partons, status -23
  TClonesArray HardPartons("TLorentzVector",kmaxT); 
  tree->Branch("HardPartons", &HardPartons);
  // Could use PDG id or something to identify as well
  TClonesArray HardPartonNames("TObjString",kmaxT);
  tree->Branch("HardPartonNames", &HardPartonNames);


  // Generate events
  int nEv = 0;
  // Int_t nEvent = 1000000;
  Int_t nEvent = 100000;
  do {
    if (!pythia.next()) continue;

    Particles.Clear();
    HardPartons.Clear();
    HardPartonNames.Clear();
    
    // Grab particles
    int fkTrack=0;
    int fkHard=0;
    for (int nP = 0; nP<event.size() ; ++nP ){
      if ( nP<3 )continue;      // 0, 1, 2: total event and beams
      
      Particle& particle = event[nP];
      // Save hardest partons
      // --------------------

      if ( particle.status()==-23 ){
	Vec4 pv = particle.p();
	new ( HardPartons[fkHard]) TLorentzVector ( pv.px(), pv.py(), pv.pz(), pv.e() ) ;
	new ( HardPartonNames[fkHard]) TObjString ( particle.name().data() ) ;
	fkHard++;
      }
	      
      if ( !particle.isFinal() ) continue;
      
      // skip neutrinos
      if ( !particle.name().compare(0, 3, "nu_") ) continue;

      // skip neutral particles we can't see
      if ( particle.name() == "K_L0"  ) continue;
      if ( particle.name() == "n0"    ) continue;
      if ( particle.name() == "nbar0" ) continue;
      
      if (
	  particle.name() != "e+"    && 
	  particle.name() != "e-"    && 
	  particle.name() != "mu+"   && 
	  particle.name() != "mu-"   && 
	  particle.name() != "pi+"   && 
	  particle.name() != "pi-"   && 
	  // particle.name() != "pi0"   && // CHECKME
	  particle.name() != "K+"    && 
	  particle.name() != "K-"    && 
	  // particle.name() != "K_L0"  && // KICK ME OUT
	  particle.name() != "p+"    && 
	  particle.name() != "pbar-" && 
	  // particle.name() != "n0"    && // KICK ME OUT
	  // particle.name() != "nbar0" && // KICK ME OUT
	  particle.name() != "gamma"
	  )	cout << particle.name() << endl;

      
      Vec4 pv = particle.p();

      // some preselections to cut down size and simulate STAR
      if ( pv.pT() < 0.2 ) continue;
      if ( fabs(pv.eta()) > 1.0 ) continue;
      
      new ( Particles[fkTrack] ) TLorentzVector ( pv.px(), pv.py(), pv.pz(), pv.e() ) ;

      ++fkTrack;

    }
    
    tree->Fill();
    
    ++nEv;
  } while (nEv<nEvent);

   

  
  fout->Write();
  fout->Close();
  
  return 0;
}
