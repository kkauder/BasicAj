/** @file AjParameters.hh
    @author Kolja Kauder
    @brief Common parameters
    @details Used to quickly include the same parameters into different macros.
    @date Feb 10, 2015
   
 */

#ifndef AJPARAMETERS_HH
#define AJPARAMETERS_HH

namespace AjParameters{
  const double R = 0.4;             ///< Resolution parameter ("radius").
  const double jet_ptmin = 10.0;    ///< Min jet pT
  const double jet_ptmax = 100.0;   ///< Max jet pT
  
  const double LeadPtMin=20.0;      ///< Min leading jet pT 
  const double SubLeadPtMin=10.0;   ///< Min sub-leading jet pT 
  const double dPhiCut = 0.4;       ///< Dijet acceptance angle,  |&phi;1 - &phi;2 -&pi;| < dPhiCut
  
  const double max_track_rap = 1.0; ///< Constituent &eta; acceptance
  const double PtConsLo=0.2;        ///< Low constituent pT cut
  const double PtConsHi=2.0;        ///< High constituent pT cut  

  const double VzCut=30;            ///< Vertex z 
  const double RefMultCut=0;        ///< Reference multiplicity. Needs to be rethought to accomodate pp and AuAu
  const double VzDiffCut=6;         ///< |Vz(TPC) - Vz(VPD)|

  // TODO: Add track, tower quality cuts
}
#endif // AJPARAMETERS_HH
