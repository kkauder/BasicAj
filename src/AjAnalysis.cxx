/** @file AjAnalysis.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Class for A<SUB>J</SUB> analysis
    @details Uses JetAnalyzer objects to perform A<SUB>J</SUB> analysis.
    @date Mar 02, 2015
*/

#include "AjAnalysis.hh"
#include <stdlib.h>     // for getenv

using std::cout;
using std::cerr;
using std::endl;

// Standard ctor
AjAnalysis::AjAnalysis ( double R,
			 double jet_ptmin, double jet_ptmax,
			 double LeadPtMin, double SubLeadPtMin, 
			 double max_track_rap, double PtConsLo, double PtConsHi,
			 double dPhiCut, bool SubtractSoftBg
			 )
  : R(R),
    jet_ptmin(jet_ptmin), jet_ptmax(jet_ptmax),
    LeadPtMin(LeadPtMin), SubLeadPtMin(SubLeadPtMin),
    max_track_rap (max_track_rap), PtConsLo (PtConsLo), PtConsHi (PtConsHi),
    dPhiCut (dPhiCut),
    pJAhi (0), pJAlo(0), pOtherJAlo(0), SubtractSoftBg(SubtractSoftBg)
{
  // derived rapidity cuts
  // ---------------------
  max_rap      = max_track_rap-R;
  ghost_maxrap = max_rap + 2.0 * R;
  
  // Constituent selectors
  // ---------------------
  select_track_rap = fastjet::SelectorAbsRapMax(max_track_rap);
  select_lopt      = fastjet::SelectorPtMin( PtConsLo );
  select_hipt      = fastjet::SelectorPtMin( PtConsHi );
  
  // Provide but turn off charge selector
  // fastjet::Selector select_track_charge= SelectorChargeRange( -3, 3);
  fastjet::Selector select_track_charge= fastjet::SelectorIdentity();
  slo     = select_track_rap * select_lopt * select_track_charge;
  shi     = select_track_rap * select_hipt * select_track_charge;
  
  // Jet candidate selectors
  // -----------------------
  select_jet_rap     = fastjet::SelectorAbsRapMax(max_rap);
  select_jet_pt_min  = fastjet::SelectorPtMin( jet_ptmin );
  select_jet_pt_max  = fastjet::SelectorPtMax( jet_ptmax );
  sjet = select_jet_rap && select_jet_pt_min && select_jet_pt_max;
  
  // Choose a jet and area definition
  // --------------------------------
  jet_def = fastjet::JetDefinition(fastjet::antikt_algorithm, R);

  // create an area definition for the clustering
  //----------------------------------------------------------
  // ghosts should go up to the acceptance of the detector or
  // (with infinite acceptance) at least 2R beyond the region
  // where you plan to investigate jets.
  area_spec = fastjet::GhostedAreaSpec( ghost_maxrap, AjParameters::ghost_repeat, AjParameters::ghost_area );
  // // DEBUG
  // // area_spec.set_grid_scatter(0);
  // // area_spec.set_pt_scatter(0);
  area_def = fastjet::AreaDefinition(fastjet::active_area_explicit_ghosts, area_spec);

  // DEBUG
  // area_spec = fastjet::VoronoiAreaSpec( 0.9 );
  // area_def = fastjet::AreaDefinition( fastjet::VoronoiAreaSpec( 0.9 ) );
  
  std::cout << " ################################################### " << std::endl;
  std::cout << "Leading jet above " << LeadPtMin << std::endl;
  std::cout << "Sub-Leading jet above " << SubLeadPtMin << std::endl;
  std::cout << "Clustered with " << jet_def.description() << std::endl;
  std::cout << "Area Spec " << area_spec.description() << std::endl;
  std::cout << "Area Def  " << area_def.description() << std::endl;
  std::cout << " ################################################### " << std::endl;

  // std::cout << slo.applies_jet_by_jet() << std::endl;
  // std::cout << shi.applies_jet_by_jet() << std::endl;
  // std::cout << sjet.applies_jet_by_jet() << std::endl;  
  // std::cout << " ################################################### " << std::endl;

  OverrideJetMin=-1;
}

// Main analysis method
// ====================
int AjAnalysis::AnalyzeAndFill ( std::vector<fastjet::PseudoJet>& particles, fastjet::PseudoJet* ToMatch,
				 Double_t EventClassifier,
				 TH2D* UnmatchedAJ_hi, TH2D* AJ_hi, TH2D* AJ_lo,
				 TH2D* UnmatchedNoFabsAJ_hi, TH2D* NoFabsAJ_hi, TH2D* NoFabsAJ_lo,

				 TH2D* UnmatchedhPtHi,  TH2D* hPtHi,  TH2D* hPtLo,
				 TH1D* UnmatchedhdPtHi, TH1D* hdPtHi, TH1D* hdPtLo,  
				 TH1D* hdphiHi, TH1D* hdphiLo,

				 TH2D* OtherAJ_lo, TH2D* OtherLeadPtLoss_lo, TH2D* OtherSubLeadPtLoss_lo, float OtherR,

				 TH2D* hdPtLead, TH2D* hdPtSubLead,
				 TH2D* SpecialhdPtLead, TH2D* SpecialhdPtSubLead,
				 std::vector<fastjet::PseudoJet>* ToReject,
				 double ForceRho,
				 TLorentzVector* SnapToHt
				 ){

  // We want to hold onto the jetanalyzer objects, so they're created dynamically
  // Need to delete them by hand
  if (pJAhi){    delete pJAhi;    pJAhi=0;  }
  if (pJAlo){    delete pJAlo;    pJAlo=0;  }
  if ( pOtherJAlo ){    delete pOtherJAlo; pOtherJAlo=0;  }

  DiJetsHi.clear();
  DiJetsLo.clear();
  OtherDiJetsLo.clear();
  
  pLo.clear();
  pHi.clear();

  Has10Gev=false;

  // Select particles to perform analysis on
  // ---------------------------------------
  pLo = slo( particles );
  pHi = shi( particles );
  
  // Background selector
  // -------------------
  // It is unclear to me why, but it leads to segfaults if only a once-initialized member :-/
  fastjet::Selector selector_bkgd = fastjet::SelectorAbsRapMax( max_rap ) * (!fastjet::SelectorNHardest(2));
  // selector_bkgd=fastjet::SelectorAbsRapMax( max_rap );

  // find high constituent pT jets
  // -----------------------------
  // DEBUG -- Use if you want to pull out rho for these events (which is 0)
  // WARNING -- A lot more expensive (factor ~15)
  // pJAhi = new JetAnalyzer( pHi, jet_def, area_def, selector_bkgd ); // WITH background estimator
  // JetAnalyzer& JAhi = *pJAhi;
  // JAhi.GetBackgroundSubtractor();

  pJAhi = new JetAnalyzer( pHi, jet_def ); // NO background subtraction
  JetAnalyzer& JAhi = *pJAhi;
  
  JAhiResult = fastjet::sorted_by_pt( sjet ( JAhi.inclusive_jets() ) );

  // Remove some jets (for eta-cone like analyses)
  int origsize = JAhiResult.size();// DEBUG  
  fastjet::Selector RejectClose = !fastjet::SelectorCircle( R );
  if ( ToReject ) {
    for ( int i=0; i< ToReject->size(); ++i ){
      RejectClose.set_reference( ToReject->at(i) );
      JAhiResult = fastjet::sorted_by_pt( RejectClose ( JAhiResult ) );
    }
    // DEBUG
    if ( origsize - ToReject->size() != JAhiResult.size() )
      std::cout << origsize << " - " << ToReject->size() << " != " << JAhiResult.size() << std::endl;
  }  
  
  if ( JAhiResult.size() < 1 )                 {     return 0; }

  // Force the trigger jet (for preselected events) to a high tower as well?
  bool HTmatched=true;
  if ( SnapToHt ){
    HTmatched=false;
    for (int i=0; i<JAhiResult.size(); ++i ){
      if ( IsMatched( JAhiResult.at(i), *SnapToHt, R ) ){
	std::iter_swap(JAhiResult.begin(),JAhiResult.begin()+i);
	HTmatched=true;
	break;
      }
    }
  }
  if ( !HTmatched )                                {     return 0; }
  if ( JAhiResult.at(0).pt() > 10 )                { Has10Gev=true; }
  if ( OverrideJetMin<0 && !Has10Gev )             {     return 0; }
  if ( OverrideJetMin>0 && JAhiResult.at(0).pt()<OverrideJetMin )   {     return 0; }
 
  // New logic: Want to learn more about the trigger jet.
  // So, get the low jets now, regardless of whether or not this event qualifies. 
  // This is sadly expensive, let's hope not too bad  
  // find corresponding jets with soft constituents
  // ----------------------------------------------
  if ( SubtractSoftBg )
    pJAlo = new JetAnalyzer( pLo, jet_def, area_def, selector_bkgd ); // WITH background subtraction
  else
    pJAlo = new JetAnalyzer( pLo, jet_def ); // WITHOUT background subtraction
  
  JetAnalyzer& JAlo = *pJAlo;
  
  // Standard:
  if ( SubtractSoftBg ) {
    fastjet::Subtractor* BackgroundSubtractor =  JAlo.GetBackgroundSubtractor();
    if ( ForceRho >=0 ) BackgroundSubtractor = new fastjet::Subtractor( ForceRho );
    JAloResult = fastjet::sorted_by_pt( (*BackgroundSubtractor)( JAlo.inclusive_jets() ) );
    if ( ForceRho >=0 ) {
      delete BackgroundSubtractor;
      BackgroundSubtractor=0;
    }
    if ( ForceRho >=0 ) {
      delete BackgroundSubtractor;
      BackgroundSubtractor=0;
    }
    JAloResult = fastjet::sorted_by_pt( (*BackgroundSubtractor)( JAlo.inclusive_jets() ) );
  } else {
    JAloResult = fastjet::sorted_by_pt( JAlo.inclusive_jets() );
  }

  // for (int i=0; i<JAloResult.size() ; ++i ) cout << i << "  " << JAloResult.at(i).pt() << endl;
  // throw(-1);
  
  // // Let's try something new:
  // fastjet::contrib::ConstituentSubtractor* BackgroundSubtractor =  pJAlo->GetConstituentBackgroundSubtractor();
  // if ( ForceRho >=0 ) {
  //   std::cerr << "Can't currently handle ForceRho" << std::endl;
  //   throw(-1);
  //   // BackgroundSubtractor = new fastjet::contrib::ConstituentSubtractor( ForceRho );
  // }
  
  //   //  Introducing a finite standardDeltaR may improve the performance of the correction and the speed of the algorithm when running over the full event.
  // BackgroundSubtractor->set_max_standardDeltaR(1); // free parameter for the maximal allowed distance sqrt((y_i-y_k)^2+(phi_i-phi_k)^2) between particle i and ghost k
  // BackgroundSubtractor->set_alpha(0);  // free parameter for the distance measure (the exponent of particle pt). The larger the parameter alpha, the more are favoured the lower pt particles in the subtraction process
  // // BackgroundSubtractor->set_ghost_area(0.01); // free parameter for the density of ghosts. The smaller, the better - but also the computation is slower.
  
  
  // // for ( int i=0; i<JAlo.inclusive_jets().size() ; ++i ){
  // //   fastjet::PseudoJet hhh = JAlo.inclusive_jets().at(i);
  // //   std::cout << hhh.has_associated_cluster_sequence();
  // //   hhh = (*BackgroundSubtractor)(hhh);
  // //   std::cout << "  --  > "
  // // 	      << hhh.has_associated_cluster_sequence()
  // // 	      << std::endl;
  // // }
  // // cout << "Trying subtraction." << endl;
  // std::vector<fastjet::PseudoJet> corrected_particles=BackgroundSubtractor->subtract_event( pLo, 1.0 );
  // // Remember the old estimator
  // fastjet::JetMedianBackgroundEstimator* bge = pJAlo->GetBackgroundEstimator();
  
  // // Now create a new ClusterSequence
  // pJAlo = new JetAnalyzer( corrected_particles, jet_def ); // Now WITHOUT background subtraction
  // JetAnalyzer& JAlo = *pJAlo;
  // JAlo.SetBackgroundEstimator( bge );  // Remember the old estimator
  // JAloResult = fastjet::sorted_by_pt( JAlo.inclusive_jets() );
  
  // Now continue dijet analysis.
  // ----------------------------
  if ( JAhiResult.size() < 2 )                 {     return 0; }
  // if ( JAhiResult.at(0).pt() < LeadPtMin )     {     return 0; }
  // if ( JAhiResult.at(1).pt() < SubLeadPtMin )  {     return 0; }
  

  // back to back? Answer this question with a selector
  // ---------------------------------------------------
  DiJetsHi = SelectorDijets( dPhiCut ) ( JAhiResult );
  if ( DiJetsHi.size() == 0 ) {
    // std::cout << " NO dijet found" << std::endl;
    return 0;
  }
  assert ( DiJetsHi.size() == 2 && "SelectorDijets returned impossible number of Dijets." );  

  // If requested, check and maybe reshuffle the dijets 
  // to redefine "leading"
  // Doing this _before_ kinematic cuts. 
  // -----------------------------------
  if ( SnapToHt ){
    if ( !IsMatched( DiJetsHi.at(0), *SnapToHt, R ) ) std::iter_swap(DiJetsHi.begin(),DiJetsHi.begin()+1);
    if ( !IsMatched( DiJetsHi.at(0), *SnapToHt, R ) ) return 0;
  }

  if ( DiJetsHi.at(0).pt() < LeadPtMin )     {     return 0; }
  if ( DiJetsHi.at(1).pt() < SubLeadPtMin )  {     return 0; }

  
  // FOR EMBEDDING: At least one matched to the reference jet?
  // ---------------------------------------------------------
  if ( ToMatch ){
    if ( !IsMatched( DiJetsHi, *ToMatch, R ) ) return 0;
  }

  // Calculate Aj and fill histos -- for unmatched high constituent pT di-jets
  UnmatchedAJ_hi->Fill ( fabs(CalcAj( DiJetsHi )), EventClassifier );
  UnmatchedNoFabsAJ_hi->Fill ( CalcAj( DiJetsHi ), EventClassifier );
  UnmatchedhPtHi->Fill ( DiJetsHi.at(0).pt() , DiJetsHi.at(1).pt());  
  UnmatchedhdPtHi->Fill ( DiJetsHi.at(0).pt() - DiJetsHi.at(1).pt());  
      
      
  

  // New logic, Apr 09, 2015: Just find the closest, highest pT jet near leading and sub-leading
  // NO further requirements.
  // That means we're NOT using sjet, i.e., no rapidity cuts, no pT cuts.
  // Using selectors mostly because I can :)
  fastjet::Selector SelectClose = fastjet::SelectorCircle( R );
  
  // Leading:
  SelectClose.set_reference( DiJetsHi.at(0) );  
  // Pulling apart this one-liner,   DiJetsLo.push_back ( ((SelectClose && fastjet::SelectorNHardest(1)) ( JAloResult )).at(0));
  // More readable and we can catch abnormalities
  std::vector<fastjet::PseudoJet> MatchedToLead = sorted_by_pt(SelectClose( JAloResult ));
  if ( MatchedToLead.size() == 0 ) {
    std::cerr << "PROBLEM: SelectorClose returned no match to leading jet." << std::endl;
    return 2;
  }
  DiJetsLo.push_back ( MatchedToLead.at(0) );

  SelectClose.set_reference( DiJetsHi.at(1) );
  std::vector<fastjet::PseudoJet> MatchedToSubLead = sorted_by_pt(SelectClose( JAloResult ));
  if ( MatchedToSubLead.size() == 0 ) {
    std::cerr << "PROBLEM: SelectorClose returned no match to sub-leading jet." << std::endl;
    return 2;
  }
  DiJetsLo.push_back ( MatchedToSubLead.at(0) );

  if ( fabs(DiJetsLo.at(0).eta())>max_rap ) std:: cerr << "Uh-oh... Lead jet eta = " << DiJetsLo.at(0).eta() << std::endl;
  if ( fabs(DiJetsLo.at(1).eta())>max_rap ) std:: cerr << "Uh-oh... SubLead jet eta = " << DiJetsLo.at(1).eta() << std::endl;
  
  // And we're done! Calculate A_J
  // -----------------------------
  AJ_hi->Fill( fabs(CalcAj( DiJetsHi )), EventClassifier );
  AJ_lo->Fill( fabs(CalcAj( DiJetsLo )), EventClassifier );
  NoFabsAJ_hi->Fill( CalcAj( DiJetsHi ), EventClassifier );
  NoFabsAJ_lo->Fill( CalcAj( DiJetsLo ), EventClassifier );

  // Also fill some diagnostic histos
  // -----------------------------
  hdphiHi->Fill( JetAnalyzer::phimod2pi( DiJetsHi.at(0).phi() - DiJetsHi.at(1).phi() - TMath::Pi()) );
  hdphiLo->Fill( JetAnalyzer::phimod2pi( DiJetsLo.at(0).phi() - DiJetsLo.at(1).phi() - TMath::Pi()) );
  
  if ( hdPtLead )    hdPtLead->Fill( DiJetsLo.at(0).pt() - DiJetsHi.at(0).pt(), EventClassifier );
  if ( hdPtSubLead ) hdPtSubLead->Fill( DiJetsLo.at(1).pt() - DiJetsHi.at(1).pt(), EventClassifier );

  if ( fabs(CalcAj( DiJetsHi ))>0.25 ){
    if ( SpecialhdPtLead )    SpecialhdPtLead->Fill( DiJetsLo.at(0).pt() - DiJetsHi.at(0).pt(), EventClassifier );
    if ( SpecialhdPtSubLead ) SpecialhdPtSubLead->Fill( DiJetsLo.at(1).pt() - DiJetsHi.at(1).pt(), EventClassifier );
  }

  hPtHi->Fill( DiJetsHi.at(0).pt(), DiJetsHi.at(1).pt());
  hPtLo->Fill( DiJetsLo.at(0).pt(), DiJetsLo.at(1).pt());
  hdPtHi->Fill( DiJetsHi.at(0).pt() - DiJetsHi.at(1).pt());
  hdPtLo->Fill( DiJetsLo.at(0).pt() - DiJetsLo.at(1).pt());

  // If requested, also see how much is recovered with another R
  // -----------------------------------------------------------
  if ( OtherAJ_lo ){
    // need new jet definition
    other_jet_def = fastjet::JetDefinition( fastjet::antikt_algorithm, OtherR);
      
    // find jets and subtract background
    if (SubtractSoftBg)
      pOtherJAlo = new JetAnalyzer( pLo, other_jet_def, area_def, selector_bkgd );
    else 
      pOtherJAlo = new JetAnalyzer( pLo, other_jet_def );
    JetAnalyzer& OtherJAlo = *pOtherJAlo;

    if (SubtractSoftBg){
      fastjet::Subtractor* OtherBackgroundSubtractor =  OtherJAlo.GetBackgroundSubtractor();
      OtherJAloResult = fastjet::sorted_by_pt( (*OtherBackgroundSubtractor)( OtherJAlo.inclusive_jets() ) );
    } else {
      OtherJAloResult = fastjet::sorted_by_pt( OtherJAlo.inclusive_jets() );
    }


    // Unclear whether to match in R or OtherR. Using R for now.
    fastjet::Selector SelectClose = fastjet::SelectorCircle( R );
    SelectClose.set_reference( DiJetsHi.at(0) );
    std::vector<fastjet::PseudoJet> OtherMatchedToLead = sorted_by_pt( SelectClose( OtherJAloResult ) );
    if ( OtherMatchedToLead.size() == 0 ) {
      std::cerr << "OTHER R PROBLEM: SelectorClose returned no match to leading jet." << std::endl;
      return 3;
    }
    OtherDiJetsLo.push_back ( OtherMatchedToLead.at(0) );
    
    SelectClose.set_reference( DiJetsHi.at(1) );
    std::vector<fastjet::PseudoJet> OtherMatchedToSubLead = sorted_by_pt(SelectClose( OtherJAloResult ));
    if ( OtherMatchedToSubLead.size() == 0 ) {
      std::cerr << "OTHER R PROBLEM: SelectorClose returned no match to sub-leading jet." << std::endl;
      return 3;
    }
    OtherDiJetsLo.push_back ( OtherMatchedToSubLead.at(0) );

    // And fill
    OtherAJ_lo->Fill( fabs(CalcAj( OtherDiJetsLo )), EventClassifier );
    OtherLeadPtLoss_lo->Fill( DiJetsLo.at(0).pt() - OtherDiJetsLo.at(0).pt() , EventClassifier );
    OtherSubLeadPtLoss_lo->Fill( DiJetsLo.at(1).pt() - OtherDiJetsLo.at(1).pt() , EventClassifier );

    
  }
  //cout << "Done here." << endl;
  return 3;
  
}


// Helper to deal with repetitive stuff
TStarJetPicoReader SetupReader ( TChain* chain, TString TriggerString, const double RefMultCut ){
  TStarJetPicoDefinitions::SetDebugLevel(0); // 10 for more output

  TStarJetPicoReader reader;
  reader.SetInputChain (chain);

  // Event and track selection
  // -------------------------
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  evCuts->SetTriggerSelection( TriggerString ); //All, MB, HT, pp, ppHT, ppJP
  // Additional cuts 
  evCuts->SetVertexZCut (AjParameters::VzCut);
  evCuts->SetRefMultCut ( RefMultCut );
  evCuts->SetVertexZDiffCut( AjParameters::VzDiffCut );
  evCuts->SetMaxEventPtCut ( AjParameters::MaxEventPtCut );
  evCuts->SetMaxEventEtCut ( AjParameters::MaxEventEtCut );
  // // TESTING ONLY:
  // evCuts->SetMaxEventPtCut ( 20000000. );
  // evCuts->SetMaxEventEtCut ( 20000000. );
  // evCuts->SetReferenceCentralityCut (  6, 8 ); // 6,8 for 0-20%
  // evCuts->SetMinEventEtCut ( -1.0 );
  // evCuts->SetMinEventEtCut ( 6.0 );


  // Tracks cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  trackCuts->SetDCACut( AjParameters::DcaCut );
  trackCuts->SetMinNFitPointsCut( AjParameters::NMinFit );
  trackCuts->SetFitOverMaxPointsCut( AjParameters::FitOverMaxPointsCut );
  trackCuts->SetMaxPtCut ( AjParameters::MaxTrackPt );

  std::cout << "Using these track cuts:" << std::endl;
  std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
  std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
  std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
  std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl;
  
  // Towers
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  towerCuts->SetMaxEtCut(AjParameters::MaxEtCut);

  std::cout << "Using these tower cuts:" << std::endl;
  std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl;
  std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl;
  
  // V0s: Turn off
  reader.SetProcessV0s(false);

  return reader;
}
//----------------------------------------------------------------------
double LookupXsec( TString filename ){
  // Some data for geant
  // -------------------
  //cross-sections for simulated GEANT data sample
  // From Renee.
  // also available via
  // http://people.physics.tamu.edu/sakuma/star/jets/c101121_event_selection/s0150_mclist_001/web.php
  // Double_t MinbXsec=28.12;
  // Double_t Xsec[12];
  // Xsec[0]=28.11;//2
  // Xsec[1]=1.287;//3
  // Xsec[2]=0.3117;//4
  // Xsec[3]=0.1360;//5
  // Xsec[4]=0.02305;//7
  // Xsec[5]=0.005494;//9
  // Xsec[6]=0.002228;//11
  // Xsec[7]=0.0003895;//15
  // Xsec[8]=0.00001016;//25
  // Xsec[9]=0.0000005010;//35
  // Xsec[10]=0.0000000283;//45
  // Xsec[11]=0.000000001443;//55
  
  static const Double_t MinbXsec=28.12;
  // static const Double_t Xsec[12] = {
  //   28.11,		// 2-3
  //   1.287,		// 3-4
  //   0.3117,		// 4-5
  //   0.1360,		// 5-7
  //   0.02305,		// 7-9
  //   0.005494,		// 9-11
  //   0.002228,		// 11-15
  //   0.0003895,		// 15-25
  //   0.00001016,		// 25-35
  //   0.0000005010,	// 35-45
  //   0.0000000283,	// 45-55
  //   0.000000001443	// 55-65
  // };

  static const Double_t Xsec[12] = {
    1.0,        // Placeholder for 2-3
    1.30E+09,	// 3-4
    3.15E+08,	// 4-5
    1.37E+08,	// 5-7
    2.30E+07,	// 7-9
    5.53E+06,	// 9-11
    2.22E+06,	// 11-15
    3.90E+05,	// 15-25
    1.02E+04,	// 25-35
    5.01E+02,	// 35-45
    2.86E+01,	// 45-55
    1.46E+00	// 55-65
  };

  static const Double_t Nmc[12] = {
    1,			// 2-3
    672518,		// 3-4
    672447,		// 4-5
    393498,		// 5-7
    417659,		// 7-9
    412652,		// 9-11
    419030,		// 11-15
    396744,		// 15-25
    399919,		// 25-35
    119995,		// 35-45
    117999,		// 45-55
    119999		// 55-65
  };

  Double_t w[12];
  for ( int i=0; i<12 ; ++i ){
    w[i] = Xsec[i] / Nmc[i];
    // w[i] = Nmc[i] / Xsec[i] ;
  }

  // static const Double_t w[12] = {
  //   1,			// Placeholder
  //   1.90E+03,
  //   6.30E+02,
  //   3.43E+02,
  //   5.49E+01,
  //   1.33E+01,
  //   5.30E+00,
  //   9.81E-01,
  //   2.56E-02,
  //   4.56E-03,
  //   2.43E-04,
  //   1.20E-05
  // };
    
  if ( filename.Contains("picoDst_3_4") ) return w[1];
  if ( filename.Contains("picoDst_4_5") ) return w[2];
  if ( filename.Contains("picoDst_5_7") ) return w[3];
  if ( filename.Contains("picoDst_7_9") ) return w[4];
  if ( filename.Contains("picoDst_9_11") ) return w[5];
  if ( filename.Contains("picoDst_11_15") ) return w[6];
  if ( filename.Contains("picoDst_15_25") ) return w[7];
  if ( filename.Contains("picoDst_25_35") ) return w[8];
  if ( filename.Contains("picoDst_35_45") ) return w[9];
  if ( filename.Contains("picoDst_45_55") ) return w[10];
  if ( filename.Contains("picoDst_55_65") ) return w[11];

  return 1;

}

