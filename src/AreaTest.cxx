// run it with    : AreaTest < ../fastjet3examples/data/single-event.dat

// #include "fastjet/ClusterSequenceArea.hh"  // use this instead of the "usual" ClusterSequence to get area support
#include "JetAnalyzer.hh"

#include <iostream> // needed for io
#include <cstdio>   // needed for io

using namespace std;

// an example program showing how to use fastjet
int main(){
  
  // read in input particles
  //----------------------------------------------------------
  vector<fastjet::PseudoJet> input_particles;
  
  double px, py , pz, E;
  while (cin >> px >> py >> pz >> E) {
    // create a fastjet::PseudoJet with these components and put it onto
    // back of the input_particles vector
    input_particles.push_back(fastjet::PseudoJet(px,py,pz,E)); 
  }

  // create a jet definition: 
  // a jet algorithm with a given radius parameter
  //----------------------------------------------------------
  double R = 0.6;
  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);
  
  double maxrap = 5.0;
  unsigned int n_repeat = 3; // default is 1
  double ghost_area = 0.01; // this is the default
  fastjet::GhostedAreaSpec area_spec(maxrap, n_repeat, ghost_area);
  
  fastjet::AreaDefinition area_def(fastjet::active_area_explicit_ghosts, area_spec);
  
  // fastjet::ClusterSequenceArea clust_seq(input_particles, jet_def, area_def);
  JetAnalyzer clust_seq(input_particles, jet_def, area_def);

  cout << area_def.description() << endl;
  cout << clust_seq.has_explicit_ghosts() << endl; 

  return 0;
}
