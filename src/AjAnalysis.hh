/** @file AjAnalysis.hh
    @author Kolja Kauder
    @version Revision 0.1
    @brief Class for A<SUB>J</SUB> analysis
    @details Uses JetAnalyzer objects to perform A<SUB>J</SUB> analysis.
    @date Mar 02, 2015
*/

#ifndef __AJANALYSIS_HH
#define __AJANALYSIS_HH

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
class AjAnalysis {
private :

  double R;              ///< Resolution parameter ("jet radius")
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

  fastjet::JetDefinition jet_def;       ///< jet definition

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

  std::vector<fastjet::PseudoJet> JAhiResult;  ///< Unaltered clustering result with high pT constituents
  std::vector<fastjet::PseudoJet> JAloResult;  ///< Unaltered clustering result with low pT constituents

  std::vector<fastjet::PseudoJet> DiJetsHi;    ///< Dijet result with high pT constituents
  std::vector<fastjet::PseudoJet> DiJetsLo;    ///< Dijet result with low pT constituents

  // Histos to fill
  // --------------
  TH2D* UnmatchedhPtHi;  ///< Unmatched hard constituent jet spectrum
  TH2D* hPtHi;           ///< Matched hard constituent jet spectrum
  TH2D* hPtLo;           ///< Matched soft constituent jet spectrum
  
  TH1D* UnmatchedhdPtHi;  ///< Unmatched hard constituent &Delta;p<SUB>T</SUB>
  TH1D* hdPtHi;           ///< Matched hard constituent &Delta;p<SUB>T</SUB>
  TH1D* hdPtLo;           ///< Matched soft constituent &Delta;p<SUB>T</SUB>

  TH1D* hdphiHi;         ///< Matched hard constituent dijet angle
  TH1D* hdphiLo;         ///< Matched soft constituent dijet angle

  TH1D* UnmatchedAJ_hi;  ///< Unmatched hard constituent A_J
  TH1D* AJ_hi;           ///< Matched hard constituent A_J
  TH1D* AJ_lo;           ///< Matched soft constituent A_J

  TH3D* UsedEventsHiPhiEtaPt; ///< phi, eta, p<SUB>T</SUB> of hard constituents
  TH3D* UsedEventsLoPhiEtaPt; ///< phi, eta, p<SUB>T</SUB> of soft constituents

  
public:
  /** Standard constructor. Set up analysis parameters.
      \param R: jet resolution parameter (radius)
      \param jet_ptmin: minimum jet p<SUB>T</SUB>
      \param jet_ptmax: maximum jet p<SUB>T</SUB>
      \param LeadPtMin: leading jet minimum p<SUB>T</SUB>
      \param SubLeadPtMin: subleading jet minimum p<SUB>T</SUB>
      \param max_track_rap: constituent rapidity cut
      \param PtConsLo: constituent minimum p<SUB>T</SUB>
      \param PtConsHi: constituent maximum p<SUB>T</SUB>
      \param dPhiCut: opening angle for dijet requirement. Accept only  |&phi;1 - &phi;2 - &pi;| < dPhiCut.
      \param UnmatchedhPtHi: p<SUB>T</SUB><SUP>sub</SUP> vs. p<SUB>T</SUB><SUP>lead</SUP> spectrum for all jets with high p<SUB>T</SUB> constituents.
      \param hPtHi: p<SUB>T</SUB><SUP>sub</SUP> vs. p<SUB>T</SUB><SUP>lead</SUP> spectrum for matched jets with high p<SUB>T</SUB> constituents.
      \param hPtLo: p<SUB>T</SUB><SUP>sub</SUP> vs. p<SUB>T</SUB><SUP>lead</SUP> spectrum for matched jets with low p<SUB>T</SUB> constituents.
      \param UnmatchedhdPtHi: &Delta;p<SUB>T</SUB><SUP>sub</SUP> for all jets with high p<SUB>T</SUB> constituents.
      \param hdPtHi: &Delta;p<SUB>T</SUB><SUP>sub</SUP> for matched jets with high p<SUB>T</SUB> constituents.
      \param hdPtLo: &Delta;p<SUB>T</SUB><SUP>sub</SUP> for matched jets with low p<SUB>T</SUB> constituents.
      \param hdphiHi: Dijet angle for matched jets with high p<SUB>T</SUB> constituents.
      \param hdphiLo: Dijet angle for matched jets with low p<SUB>T</SUB> constituents.
      \param UnmatchedAJ_hi: Dijet imbalance &Delta;p<SUB>T</SUB> / &Sigma;p<SUB>T</SUB> for all jets with high p<SUB>T</SUB> constituents.
      \param AJ_hi: Dijet imbalance &Delta;p<SUB>T</SUB> / &Sigma;p<SUB>T</SUB> for matched jets with high p<SUB>T</SUB> constituents.
      \param AJ_lo: Dijet imbalance &Delta;p<SUB>T</SUB> / &Sigma;p<SUB>T</SUB> for matched jets with low p<SUB>T</SUB> constituents.
      \param UsedEventsHiPhiEtaPt: p<SUB>T</SUB>, &phi;, &eta;  for high p<SUB>T</SUB> constituent candidates in all events
      \param UsedEventsLoPhiEtaPt: p<SUB>T</SUB>, &phi;, &eta;  for low p<SUB>T</SUB> constituent candidates in all events
   */
  AjAnalysis ( double R = 0.4,
	       double jet_ptmin = 10.0, double jet_ptmax = 100.0,
	       double LeadPtMin = 20.0, double SubLeadPtMin = 10, 
	       double max_track_rap = 1.0, double PtConsLo=0.2, double PtConsHi=2.0,
	       double dPhiCut = 0.4,
	       TH2D* UnmatchedhPtHi=0, TH2D* hPtHi=0, TH2D* hPtLo=0,  
	       TH1D* UnmatchedhdPtHi=0, TH1D* hdPtHi=0, TH1D* hdPtLo=0,
	       TH1D* hdphiHi=0, TH1D* hdphiLo=0,
	       TH1D* UnmatchedAJ_hi=0, TH1D* AJ_hi=0, TH1D* AJ_lo=0,
	       TH3D* UsedEventsHiPhiEtaPt=0, TH3D* UsedEventsLoPhiEtaPt=0
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
   * \param particles: Current event
   * \param ToMatch: Optionally enforce matching of at least one of the dijets to a trigger
   * Return value:
   *   - 0: No hard constituent dijet found, or not matched to ToMatch
   *   - 1: No soft constituent dijet found
   *   - 2: Soft constituent dijet found but not matched
   */
  int AnalyzeAndFill ( std::vector<fastjet::PseudoJet>& particles, fastjet::PseudoJet* ToMatch=0 );
  
  /** This little helper is true if there's at least one 10 GeV jet
   **/
  bool Has10Gev;

  // Getters and Setters
  // -------------------
  /// Get jet radius
  inline double GetR ( )                   { return R; };
  /// Set jet radius
  inline void   SetR ( const double newv ) { R=newv;   };

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
  inline fastjet::JetDefinition& GetJet_def () { return jet_def; }
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


  /// Handle to unaltered clustering result with high pT constituents
  inline std::vector<fastjet::PseudoJet> GetJAhiResult() {return JAhiResult; };
  /// Handle to unaltered clustering result with low pT constituents
  inline std::vector<fastjet::PseudoJet> GetJAloResult() {return JAloResult; };


  /// Handle to Dijet result with high pT constituents
  inline std::vector<fastjet::PseudoJet> GetDiJetsHi() {return DiJetsHi; };
  /// Handle to Dijet result with low pT constituents
  inline std::vector<fastjet::PseudoJet> GetDiJetsLo() {return DiJetsLo; };

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
