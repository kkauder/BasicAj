//----------------------------------------------------------------------
// Make a simple pythia tree for playing around. Add some mock AuAu background.
// run it with    : ./bin/TreeWithMc
///

#include "AjAnalysis.hh"
#include "AjParameters.hh"

#include "Pythia.h"

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

using namespace std;
using namespace Pythia8;
using namespace fastjet;

Int_t main(int argc, char **argv) {

  // Set up output file
  TString OutFileName = "PythiaAndMc.root";
  TFile * fout = new TFile(OutFileName,"recreate");
  
  // Generator. Shorthand for event (use FastJet3 and Pythia8 interface!!!)
  //------------------------------------------------------------------------
  // Float_t ptHatMin=40.0;
  // Float_t ptHatMax=100.0;
  Float_t ptHatMin=20.0;
  Float_t ptHatMax=60.0;

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


  gRandom->SetSeed(1);
    
  // Set up tree
  // -----------
  static const Int_t kmaxT=5000; // # of particles per event

  TTree* tree  = new TTree("tree","Some pythia jets");
  // TClonesArray PythiaParticles("TLorentzVector",kmaxT);
  // tree->Branch("PythiaParticles", &PythiaParticles);

  TClonesArray PythiaParticles("TStarJetVector",kmaxT);
  tree->Branch("PythiaParticles", &PythiaParticles);


  // The original outgoing hard partons, status -23
  TClonesArray HardPartons("TLorentzVector",kmaxT); 
  tree->Branch("HardPartons", &HardPartons);
  // Could use PDG id or something to identify as well
  TClonesArray HardPartonNames("TObjString",kmaxT);
  tree->Branch("HardPartonNames", &HardPartonNames);

  TClonesArray McParticles("TStarJetVector",kmaxT);
  // TClonesArray McParticles("TLorentzVector",kmaxT);
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


  // Multiplicity
  // ------------
  TFile* fMult = new TFile("TrueMult_0_20.root","READ");
  TH1D* AuAuN = (TH1D*) fMult->Get("AuAuN"); // Maybe reject N<800 or so
  
  // Generate events
  // ---------------
  int nEv = 0;
  // Int_t nEvent = 1000000;
  Int_t nEvent = 50000;
  //  Int_t nEvent = 500;
  fastjet::PseudoJet v; // helper
  // const double M = 0.13957;  // set pi+ mass
  const double M = 0;  // for background, use massless particles, as in real data

  float pt, rap, phi, p, E;
  do {
    if (!pythia.next()) continue;
    
    PythiaParticles.Clear();
    // SvPythiaParticles.Clear();
    HardPartons.Clear();
    HardPartonNames.Clear();
    McParticles.Clear();
    
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
	  //	  particle.name() != "pi0"   && // CHECKME
	  particle.name() != "K+"    && 
	  particle.name() != "K-"    && 
	  //	  particle.name() != "K_L0"  && // KICK ME OUT
	  particle.name() != "p+"    && 
	  particle.name() != "pbar-" && 
	  // particle.name() != "n0"    && // KICK ME OUT
	  // particle.name() != "nbar0" && // KICK ME OUT
	  particle.name() != "gamma"
	  )	cout << particle.name() << endl;
      
      
      // Vec4 pv = particle.p();
      // turn off mass
      double E = TMath::Sqrt( pow(particle.px(), 2) + pow(particle.py(), 2) + pow(particle.pz(), 2) );
      Vec4 pv( particle.px(), particle.py(), particle.pz(), E );

      // some preselections to cut down size and simulate STAR
      if ( pv.pT() < 0.2 ) continue;
      if ( fabs(pv.eta()) > 1.0 ) continue;
      
      // new ( PythiaParticles[fkTrack] ) TLorentzVector ( pv.px(), pv.py(), pv.pz(), pv.e() ) ;

      // Use TStarJetVector, which has more information (like charge)
      // ------------------------------------------------------------
      TStarJetVector part;      
      if ( particle.charge() !=0 ){
	part.SetPtEtaPhiM( pv.pT(), pv.eta(), pv.phi(), 0);
	part.SetType(TStarJetVector::_TRACK);
	part.SetCharge(particle.charge());
      } else {
	part.SetPtEtaPhiM( pv.eT(), pv.eta(), pv.phi(), 0);
	part.SetType(TStarJetVector::_TOWER);
	part.SetCharge(TStarJetVector::_NEUTRAL);	      
      }
      new ( PythiaParticles[fkTrack] ) TStarJetVector (part) ;

      
      ++fkTrack;
    }

    // Now add some mock Au+Au background.
    // -----------------------------------

    // Multiplicity
    int N;
    while ( (N=AuAuN->GetRandom())<800);

    // Need to shift down
    N-=PythiaParticles.GetEntries();    
    
    int nAuAu=0;
    while ( nAuAu<N ){
      pt = AuAuSoftPt->GetRandom();
      rap = gRandom->Uniform(-1,1);
      phi = gRandom->Uniform(-TMath::Pi(),TMath::Pi());      
      
      double p = pt * TMath::CosH( rap );
      double E = sqrt( p*p + M*M);
      double px = pt * TMath::Cos( phi );
      double py = pt * TMath::Sin( phi );
      double pz = pt * TMath::SinH( rap );

      int charge =  int( gRandom->Integer(3) ) -1;

      Vec4 pv( px, py, pz, E );
      if ( pv.pT() < 0.2 ) continue;
      if ( fabs(pv.eta()) > 1.0 ) continue;
      
      // new ( McParticles[nAuAu] ) TLorentzVector ( pv.px(), pv.py(), pv.pz(), pv.e() ) ;

      // Use TStarJetVector, which has more information (like charge)
      // ------------------------------------------------------------
      TStarJetVector part;      
      if ( charge !=0 ){
	part.SetPtEtaPhiM( pv.pT(), pv.eta(), pv.phi(), 0);
	part.SetType(TStarJetVector::_TRACK);
	part.SetCharge( charge );
      } else {
	part.SetPtEtaPhiM( pv.eT(), pv.eta(), pv.phi(), 0);
	part.SetType(TStarJetVector::_TOWER);
	part.SetCharge(TStarJetVector::_NEUTRAL);	      
      }

      new ( McParticles[nAuAu] ) TStarJetVector (part) ;

      // v.reset_PtYPhiM( pt, rap, phi, E );
      // if ( fabs(v.eta()) > 1.0 ) continue;
      // new ( McParticles[nAuAu] ) TLorentzVector ( MakeTLorentzVector( v )  );
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
  
  return 0;
}
