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
  // const double jet_ptmax = 100.0;   ///< Max jet pT
  const double jet_ptmax = 1000.0;   ///< DEBUG
  
  const double LeadPtMin=20.0;      ///< Min leading jet pT 
  const double SubLeadPtMin=10.0;   ///< Min sub-leading jet pT 
  const double dPhiCut = 0.4;       ///< Dijet acceptance angle,  |&phi;1 - &phi;2 -&pi;| < dPhiCut
  
  const double max_track_rap = 1.0; ///< Constituent &eta; acceptance
  const double PtConsLo=0.2;        ///< Low constituent pT cut
  const double PtConsHi=2.0;        ///< High constituent pT cut  

  const double VzCut=30;            ///< Vertex z 
  const double AuAuRefMultCut=268;  ///< Reference multiplicity cut for Au+Au data
  const double VzDiffCut=6;         ///< |Vz(TPC) - Vz(VPD)|

  const double DcaCut=1.0;          ///< track dca
  const int NMinFit=20;             ///< minimum number of fit points for tracks
  const double FitOverMaxPointsCut=0.52; ///< NFit / NFitPossible
  const double MaxTrackPt=1000;     ///< track pT cut
  const double MaxEtCut=70;         ///< tower ET cut
  
  // // TO REPRODUCE JOERN:
  // const double VzCut=30;
  // const double AuAuRefMultCut=351;
  // const double DcaCut=1.0;
  // const int NMinFit=20;
  // const double FitOverMaxPointsCut=0.52;
  // // NO VzDiffCut
  // // NO MaxTrackPt or MaxEtCut
  // // HOWEVER, we do need some Et cut to get rid off crazy outliers
  // const double MaxEtCut=70;  
  // const double MaxTrackPt=1000;
  // // const double MaxEtCut=1000;
  // const double VzDiffCut=1000;

  
}
#endif // AJPARAMETERS_HH
