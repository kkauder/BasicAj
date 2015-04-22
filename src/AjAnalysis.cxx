/** @file AjAnalysis.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Class for A<SUB>J</SUB> analysis
    @details Uses JetAnalyzer objects to perform A<SUB>J</SUB> analysis.
    @date Mar 02, 2015
*/

#include "AjAnalysis.hh"

// Standard ctor
AjAnalysis::AjAnalysis ( double R,
			 double jet_ptmin, double jet_ptmax,
			 double LeadPtMin, double SubLeadPtMin, 
			 double max_track_rap, double PtConsLo, double PtConsHi,
			 double dPhiCut
			 )
  : R(R),
    jet_ptmin(jet_ptmin), jet_ptmax(jet_ptmax),
    LeadPtMin(LeadPtMin), SubLeadPtMin(SubLeadPtMin),
    max_track_rap (max_track_rap), PtConsLo (PtConsLo), PtConsHi (PtConsHi),
    dPhiCut (dPhiCut)
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
  fastjet::GhostedAreaSpec area_spec( ghost_maxrap );
  fastjet::AreaDefinition  area_def(fastjet::active_area, area_spec);
  
  std::cout << " ################################################### " << std::endl;
  std::cout << "Clustered with " << jet_def.description() << std::endl;
  std::cout << "Area Spec " << area_spec.description() << std::endl;
  std::cout << "Area Def  " << area_def.description() << std::endl;
  std::cout << " ################################################### " << std::endl;

}

// Main analysis method
// ====================
int AjAnalysis::AnalyzeAndFill ( std::vector<fastjet::PseudoJet>& particles, fastjet::PseudoJet* ToMatch,
				 Double_t EventClassifier,
				 TH2D* UnmatchedAJ_hi, TH2D* AJ_hi, TH2D* AJ_lo,

				 TH2D* UnmatchedhPtHi,  TH2D* hPtHi,  TH2D* hPtLo,
				 TH1D* UnmatchedhdPtHi, TH1D* hdPtHi, TH1D* hdPtLo,  
				 TH1D* hdphiHi, TH1D* hdphiLo
				 ){
  DiJetsHi.clear();
  DiJetsLo.clear();
  Has10Gev=false;

  // Select particles to perform analysis on
  // ---------------------------------------
  std::vector<fastjet::PseudoJet> pLo = slo( particles );
  std::vector<fastjet::PseudoJet> pHi = shi( particles );
  
  // find high constituent pT jets
  // -----------------------------
  JetAnalyzer JAhi ( pHi, jet_def ); // NO background subtraction
  JAhiResult = fastjet::sorted_by_pt( sjet ( JAhi.inclusive_jets() ) );
  
  if ( JAhiResult.size() < 1 )                 {     return 0; }
  if ( JAhiResult.at(0).pt() > 10 )            { Has10Gev=true; }

  if ( JAhiResult.size() < 2 )                 {     return 0; }
  if ( JAhiResult.at(0).pt() < LeadPtMin )     {     return 0; }
  if ( JAhiResult.at(1).pt() < SubLeadPtMin )  {     return 0; }
      
  // back to back? Answer this question with a selector
  // ---------------------------------------------------
  DiJetsHi = SelectorDijets( dPhiCut ) ( JAhiResult );
  if ( DiJetsHi.size() == 0 ) {
    // std::cout << " NO dijet found" << std::endl;
    return 0;
  }
  assert ( DiJetsHi.size() == 2 && "SelectorDijets returned impossible number of Dijets." );  

  // FOR EMBEDDING: At least one matched to the reference jet?
  // ---------------------------------------------------------
  if ( ToMatch ){
    if ( !IsMatched( DiJetsHi, *ToMatch, R ) ) return 0;
  }

  // Calculate Aj and fill histos -- for unmatched high constituent pT di-jets
  UnmatchedAJ_hi->Fill ( CalcAj( DiJetsHi ), EventClassifier );
  UnmatchedhPtHi->Fill ( DiJetsHi.at(0).pt() , DiJetsHi.at(1).pt());  
  UnmatchedhdPtHi->Fill ( DiJetsHi.at(0).pt() - DiJetsHi.at(1).pt());  
      
  // find corresponding jets with soft constituents
  // ----------------------------------------------
  JetAnalyzer JAlo( pLo, jet_def, area_def ); // WITH background subtraction
  fastjet::Subtractor* BackgroundSubtractor =  JAlo.GetBackgroundSubtractor();
  JAloResult = fastjet::sorted_by_pt( (*BackgroundSubtractor)( JAlo.inclusive_jets() ) );
  
  // Old logic: Run the same logic on JAloResult, then reject the event if DiJetsLo is not matched to DiJetsHi
  // std::vector<fastjet::PseudoJet> DiJetsLo = SelectorDijets ( dPhiCut ) ( sjet ( JAloResult)  );
  // if ( DiJetsLo.size() == 0 ) {
  //   // std::cout << " NO dijet found" << std::endl;
  //   return 1;
  // }
  // assert ( DiJetsLo.size() == 2 && "SelectorDijets returned impossible number of Dijets." );
  
  // // cout << " SOFT constituents: Found a dijet!" << endl;
  // // cout << " Jet 1: pt = " << DiJetsLo.at(0).pt() << "  phi = " << DiJetsLo.at(0).phi() << endl;
  // // cout << " Jet 2: pt = " << DiJetsLo.at(1).pt() << "  phi = " << DiJetsLo.at(1).phi() << endl;

  // // Matched?
  // // --------
  // // Top two hard constituent jets should be geometrically close to the top two soft ones.
  // // Overloading the == operator for vector<PseudoJet> does NOT work.
  // // This function is implemented in JetAnalyzer
  // if ( !IsMatched( DiJetsHi, DiJetsLo, R ) ){
  //   // cout << " Couldn't find a match" << endl;
  //   return 2;
  // }

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
  AJ_hi->Fill( CalcAj( DiJetsHi ), EventClassifier );
  AJ_lo->Fill( CalcAj( DiJetsLo ), EventClassifier );

  // Also fill some diagnostic histos
  // -----------------------------
  hdphiHi->Fill( JetAnalyzer::phimod2pi( DiJetsHi.at(0).phi() - DiJetsHi.at(1).phi() - TMath::Pi()) );
  hdphiLo->Fill( JetAnalyzer::phimod2pi( DiJetsLo.at(0).phi() - DiJetsLo.at(1).phi() - TMath::Pi()) );
  
  hPtHi->Fill( DiJetsHi.at(0).pt(), DiJetsHi.at(1).pt());
  hPtLo->Fill( DiJetsLo.at(0).pt(), DiJetsLo.at(1).pt());
  hdPtHi->Fill( DiJetsHi.at(0).pt() - DiJetsHi.at(1).pt());
  hdPtLo->Fill( DiJetsLo.at(0).pt() - DiJetsLo.at(1).pt());

  // // cleanup
  // // -------
  // delete JAhi;
  // delete JAlo;
  // pLo.clear();
  // pHi.clear();


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

