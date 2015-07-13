/** @file FollowAjAnalysis.hh
    @author Kolja Kauder
    @version Revision 0.1
    @brief Class for A<SUB>J</SUB> analysis
    @details Uses JetAnalyzer objects to perform A<SUB>J</SUB> analysis.
    @date Mar 02, 2015
*/

#ifndef __FOLLOWAJANALYSIS_HH
#define __FOLLOWAJANALYSIS_HH

#include "AjParameters.hh"
#include "JetAnalyzer.hh"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TString.h"
#include "TChain.h"


// Not needed for analysis per se
#include "TStarJetPicoReader.h"
#include "TStarJetPicoEvent.h"
#include "TStarJetPicoEventHeader.h"
#include "TStarJetPicoEventCuts.h"

#include "TStarJetPicoPrimaryTrack.h"
#include "TStarJetPicoTrackCuts.h"
#include "TStarJetPicoTowerCuts.h"

#include "TStarJetVectorContainer.h"
#include "TStarJetVector.h"
#include "TStarJetPicoTriggerInfo.h"
#include "TStarJetPicoUtils.h"

#include <assert.h>
#include <iostream>
#include <cmath>

/**
   The main class
 */
class FollowAjAnalysis {
private :

  double SmallR;         ///< Resolution parameter ("radius") for original selection
  double LargeR;         ///< Resolution parameter ("radius") for reclustering
  double jet_ptmin;      ///< minimum jet p<SUB>T</SUB>
  double jet_ptmax;      ///< maximum jet p<SUB>T</SUB>
  double LeadPtMin;      ///< leading jet minimum p<SUB>T</SUB>
  double SubLeadPtMin;   ///< subleading jet minimum p<SUB>T</SUB>
  double max_rap;        ///< jet rapidity acceptance
  double ghost_maxrap;   ///< for ghosted area, should be >= max_rap + 2*R

  double max_track_rap;  ///< constituent rapidity cut
  double PtConsLo;       ///< constituent minimum p<SUB>T</SUB>
  double PtConsHi;       ///< constituent maximum p<SUB>T</SUB>

  double dPhiCut;        ///< opening angle for dijet requirement. Accept only  |&phi;1 - &phi;2 - &pi;| < &Delta;&phi;.

  fastjet::JetDefinition small_jet_def;       ///< jet definition with SmallR
  fastjet::JetDefinition large_jet_def;       ///< jet definition with LargeR

  fastjet::Selector select_track_rap;   ///< constituent rapidity selector
  fastjet::Selector select_lopt;        ///< constituent p<SUB>T</SUB> selector
  fastjet::Selector select_hipt;        ///< constituent p<SUB>T</SUB> selector

  fastjet::Selector slo;                ///< compound selector for low  p<SUB>T</SUB> constituents
  fastjet::Selector shi;                ///< compound selector for high p<SUB>T</SUB> constituents
  
  // Relevant jet candidates
  fastjet::Selector select_jet_rap;        ///< jet rapidity selector
  fastjet::Selector select_jet_pt_min;     ///< jet p<SUB>T</SUB> selector
  fastjet::Selector select_jet_pt_max;     ///< jet p<SUB>T</SUB> selector
  fastjet::Selector sjet;                  ///< compound jet selector

  fastjet::GhostedAreaSpec area_spec;      ///< ghosted area specification
  fastjet::AreaDefinition area_def;        ///< jet area definition

  JetAnalyzer* pSmallJAhi;                      ///< JetAnalyzer object for high pT, small R
  JetAnalyzer* pSmallJAlo;                      ///< JetAnalyzer object for low pT, small R

  JetAnalyzer* pLargeJAhi;                      ///< JetAnalyzer object for high pT, large R
  JetAnalyzer* pLargeJAlo;                      ///< JetAnalyzer object for low pT, large R


  std::vector<fastjet::PseudoJet> SmallJAhiResult;  ///< Clustering result with high pT constituents, small R
  std::vector<fastjet::PseudoJet> SmallJAloResult;  ///< Clustering result with low pT constituents, small R

  std::vector<fastjet::PseudoJet> LargeJAhiResult;  ///< Clustering result with high pT constituents, large R
  std::vector<fastjet::PseudoJet> LargeJAloResult;  ///< Clustering result with low pT constituents, large R

  std::vector<fastjet::PseudoJet> SmallDiJetsHi;    ///< Dijet result with high pT constituents, small R
  std::vector<fastjet::PseudoJet> SmallDiJetsLo;    ///< Dijet result with low pT constituents, small R

  std::vector<fastjet::PseudoJet> LargeDiJetsHi;    ///< Dijet result with high pT constituents, large R
  std::vector<fastjet::PseudoJet> LargeDiJetsLo;    ///< Dijet result with low pT constituents, large R
  
  // Histos to fill
  // --------------
  TH1D* LeadDeltaPtHi;        ///< Matched, hard constituents, p_T^{Lead} difference between small and large R
  TH1D* SubLeadDeltaPtHi;     ///< Matched, hard constituents, p_T^{Sub} difference between small and large R
  TH1D* LeadDeltaPtLo;        ///< Matched, soft constituents, p_T^{Lead} difference between small and large R
  TH1D* SubLeadDeltaPtLo;     ///< Matched, soft constituents, p_T^{Sub} difference between small and large R
  
  TH1D* hdPtLo;               ///< Matched, soft constituents, p_T difference between small and large R

  TH2D* SmallUnmatchedhPtHi;  ///< Unmatched hard constituent jet spectrum, small R
  TH2D* SmallhPtHi;           ///< Matched hard constituent jet spectrum, small R
  TH2D* SmallhPtLo;           ///< Matched soft constituent jet spectrum, small R
  
  TH1D* SmallhdphiHi;         ///< Matched hard constituent dijet angle, small R
  TH1D* SmallhdphiLo;         ///< Matched soft constituent dijet angle, small R

  TH1D* SmallUnmatchedAJ_hi;  ///< Unmatched hard constituent A_J, small R
  TH1D* SmallAJ_hi;           ///< Matched hard constituent A_J, small R
  TH1D* SmallAJ_lo;           ///< Matched soft constituent A_J, small R

  TH2D* LargehPtHi;           ///< Matched hard constituent jet spectrum, large R
  TH2D* LargehPtLo;           ///< Matched soft constituent jet spectrum, large R
  
  TH1D* LargehdphiHi;         ///< Matched hard constituent dijet angle, large R
  TH1D* LargehdphiLo;         ///< Matched soft constituent dijet angle, large R

  TH1D* LargeAJ_hi;           ///< Matched hard constituent A_J, large R
  TH1D* LargeAJ_lo;           ///< Matched soft constituent A_J, large R

  TH1D* SmallDeltaAJ_hilo;    ///< Small R, A_J (hard) - A_J (soft)
  TH1D* LargeDeltaAJ_hilo;    ///< Large R, A_J (hard) - A_J (soft)
  TH1D* DeltaAJ_hi;           ///< Matched, hard constituents, A_J (Small R) - A_J (Large R)
  TH1D* DeltaAJ_lo;           ///< Matched, soft constituents, A_J (Small R) - A_J (Large R)
  
public:
  /** Standard constructor. Set up analysis parameters.
      \param SmallR: Resolution parameter ("radius") for original selection
      \param LargeR: Resolution parameter ("radius") for reclustering
      \param jet_ptmin: minimum jet p<SUB>T</SUB>
      \param jet_ptmax: maximum jet p<SUB>T</SUB>
      \param LeadPtMin: leading jet minimum p<SUB>T</SUB>
      \param SubLeadPtMin: subleading jet minimum p<SUB>T</SUB>
      \param max_track_rap: constituent rapidity cut
      \param PtConsLo: constituent minimum p<SUB>T</SUB>
      \param PtConsHi: constituent maximum p<SUB>T</SUB>
      \param dPhiCut: opening angle for dijet requirement. Accept only  |&phi;1 - &phi;2 - &pi;| < dPhiCut.
   */
  FollowAjAnalysis ( double SmallR = 0.2, double LargeR = 0.4,
		     double jet_ptmin = 10.0, double jet_ptmax = 100.0,
		     double LeadPtMin = 20.0, double SubLeadPtMin = 10, 
		     double max_track_rap = 1.0, double PtConsLo=0.2, double PtConsHi=2.0,
		     double dPhiCut = 0.4
		     );
  /** Dijet asymmetry A<SUB>J</SUB> = &Delta;p<SUB>T</SUB> / &Sigma;p<SUB>T</SUB> */
  inline double CalcAj ( std::vector<fastjet::PseudoJet>& jets ){
    if ( jets.size()!=2 ){
      throw ( -1 );
      return -1e10;
    }
    return fabs (( jets.at(0).pt()-jets.at(1).pt() ) / ( jets.at(0).pt()+jets.at(1).pt() ));    
  }

  
  /** Main analysis routine.
   \param particles: Current event
   \param ToMatch: Optionally enforce matching of at least one of the dijets to a trigger
   \param EventClassifier: Used to separate between events, e.g. by RefmultBin
   \param UnmatchedhPtHi: p<SUB>T</SUB><SUP>sub</SUP> vs. p<SUB>T</SUB><SUP>lead</SUP> spectrum for all jets with high p<SUB>T</SUB> constituents.
   \param hPtHi: p<SUB>T</SUB><SUP>sub</SUP> vs. p<SUB>T</SUB><SUP>lead</SUP> spectrum for matched jets with high p<SUB>T</SUB> constituents.
   \param hPtLo: p<SUB>T</SUB><SUP>sub</SUP> vs. p<SUB>T</SUB><SUP>lead</SUP> spectrum for matched jets with low p<SUB>T</SUB> constituents.
   \param hdphiHi: Dijet angle for matched jets with high p<SUB>T</SUB> constituents.
   \param hdphiLo: Dijet angle for matched jets with low p<SUB>T</SUB> constituents.
   \param UnmatchedAJ_hi: Dijet imbalance &Delta;p<SUB>T</SUB> / &Sigma;p<SUB>T</SUB> for all jets with high p<SUB>T</SUB> constituents.
   \param AJ_hi: Dijet imbalance &Delta;p<SUB>T</SUB> / &Sigma;p<SUB>T</SUB> for matched jets with high p<SUB>T</SUB> constituents.
   \param AJ_lo: Dijet imbalance &Delta;p<SUB>T</SUB> / &Sigma;p<SUB>T</SUB> for matched jets with low p<SUB>T</SUB> constituents.
   \param UsedEventsHiPhiEtaPt: p<SUB>T</SUB>, &phi;, &eta;  for high p<SUB>T</SUB> constituent candidates in all events
   \param UsedEventsLoPhiEtaPt: p<SUB>T</SUB>, &phi;, &eta;  for low p<SUB>T</SUB> constituent candidates in all events
   * Return value:
   *   - 0: No hard constituent dijet found, or not matched to ToMatch
   *   - 1: No soft constituent dijet found
   *   - 2: Soft constituent dijet found but not matched
   */
  int AnalyzeAndFill ( std::vector<fastjet::PseudoJet>& particles, fastjet::PseudoJet* ToMatch=0,
		       Double_t EventClassifier=0,
		       TH1D* LeadDeltaPtHi=0,  TH1D* SubLeadDeltaPtHi=0, TH1D* LeadDeltaPtLo=0,  TH1D* SubLeadDeltaPtLo=0,
		       TH2D* SmallUnmatchedhPtHi=0,  TH2D* SmallhPtHi=0,  TH2D* SmallhPtLo=0,
		       TH1D* SmallhdphiHi=0, TH1D* SmallhdphiLo=0,
		       TH2D* SmallUnmatchedAJ_hi=0, TH2D* SmallAJ_hi=0, TH2D* SmallAJ_lo=0,
		       TH2D* LargehPtHi=0,  TH2D* LargehPtLo=0,
		       TH1D* LargehdphiHi=0, TH1D* LargehdphiLo=0,
		       TH2D* LargeAJ_hi=0, TH2D* LargeAJ_lo=0,
		       TH2D* SmallDeltaAJ_hilo=0, TH2D* LargeDeltaAJ_hilo=0,
		       TH2D* DeltaAJ_hi=0, TH2D* DeltaAJ_lo=0
		       );
  
  /** This little helper is true if there's at least one 10 GeV jet
   **/
  bool Has10Gev;

  // Getters and Setters
  // -------------------
  /// Get minimum jet p<SUB>T</SUB>
  inline double GetJet_ptmin ( )                   { return jet_ptmin; };
  /// Set minimum jet p<SUB>T</SUB>
  inline void   SetJet_ptmin ( const double newv ) { jet_ptmin=newv;   };

  /// Get maximum jet p<SUB>T</SUB>
  inline double GetJet_ptmax ( )                   { return jet_ptmax; };
  /// Set maximum jet p<SUB>T</SUB>
  inline void   SetJet_ptmax ( const double newv ) { jet_ptmax=newv;   };

  /// Get jet rapidity acceptance
  inline double GetMax_rap ( )                   { return max_rap; };
  /// Set jet rapidity acceptance
  inline void   SetMax_rap ( const double newv ) { max_rap=newv;   };

  /// Get ghosted area rapidity cut, should be >= max_rap + 2*R
  inline double GetGhost_maxrap ( )                   { return ghost_maxrap; };
  /// Set ghosted area rapidity cut, should be >= max_rap + 2*R
  inline void   SetGhost_maxrap ( const double newv ) { ghost_maxrap=newv;   };

   /// Get dijet opening angle
  inline double GetDPhiCut ( )                   { return dPhiCut; };
  /// Set dijet opening angle
  inline void   SetDPhiCut ( const double newv ) { dPhiCut=newv;   };
  

  // Objects will be handed by _reference_! Obviates need for setter
  /// Handle to jet definition
  inline fastjet::JetDefinition& GetSmallJet_def () { return small_jet_def; }
  inline fastjet::JetDefinition& GetLargeJet_def () { return large_jet_def; }
  /// Handle to selector for low  p<SUB>T</SUB> constituents
  inline fastjet::Selector& GetLoConsSelector () { return slo; }
  /// Handle to selector for high  p<SUB>T</SUB> constituents
  inline fastjet::Selector& GetHiConsSelector () { return shi; }
  
  /// Handle to selector for jet candidates
  inline fastjet::Selector& GetJetSelector () { return sjet; }

  /// Handle to ghosted area specification
  inline fastjet::GhostedAreaSpec& GetArea_spec () { return area_spec; }
  /// Handle to jet area definition
  inline fastjet::AreaDefinition& GetArea_def () { return area_def; }


  /// Handle to unaltered clustering result with high pT constituents, small R
  inline std::vector<fastjet::PseudoJet> GetSmallJAhiResult() {return SmallJAhiResult; };
  /// Handle to unaltered clustering result with low pT constituents
  inline std::vector<fastjet::PseudoJet> GetSmallJAloResult() {return SmallJAloResult; };

    /// Handle to unaltered clustering result with high pT constituents, large R
  inline std::vector<fastjet::PseudoJet> GetLargeJAhiResult() {return LargeJAhiResult; };
  /// Handle to unaltered clustering result with low pT constituents
  inline std::vector<fastjet::PseudoJet> GetLargeJAloResult() {return LargeJAloResult; };

  
};  

/** Helper to perform the TStarJetPicoReader initialization
 */
TStarJetPicoReader GetReader ( TString ChainPattern="~putschke/Data/Pico_ppHT/*.root", 
			       TString TriggerString="ppHT",
			       TString ChainName="JetTree",
			       const double RefMultCut=0
			       );

/** Slightly different, preferred version of GetReader
 */
TStarJetPicoReader SetupReader ( TChain* chain, TString TriggerString, const double RefMultCut=0 );


#endif // __AJANALYSIS_HH
