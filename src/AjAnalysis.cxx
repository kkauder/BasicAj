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
			 double dPhiCut,
			 TH2D* UnmatchedhPtHi,  TH2D* hPtHi,  TH2D* hPtLo,
			 TH1D* UnmatchedhdPtHi, TH1D* hdPtHi, TH1D* hdPtLo,  
			 TH1D* hdphiHi, TH1D* hdphiLo,
			 TH1D* UnmatchedAJ_hi, TH1D* AJ_hi, TH1D* AJ_lo,
			 TH3D* UsedEventsHiPhiEtaPt, TH3D* UsedEventsLoPhiEtaPt 
			 )
  : R(R),
    jet_ptmin(jet_ptmin), jet_ptmax(jet_ptmax),
    LeadPtMin(LeadPtMin), SubLeadPtMin(SubLeadPtMin),
    max_track_rap (max_track_rap), PtConsLo (PtConsLo), PtConsHi (PtConsHi),
    dPhiCut (dPhiCut),
    UnmatchedhPtHi (UnmatchedhPtHi),  hPtHi (hPtHi),  hPtLo (hPtLo),
    UnmatchedhdPtHi (UnmatchedhdPtHi),  hdPtHi (hdPtHi),  hdPtLo (hdPtLo),  
    hdphiHi (hdphiHi), hdphiLo (hdphiLo),
    UnmatchedAJ_hi (UnmatchedAJ_hi), AJ_hi(AJ_hi), AJ_lo (AJ_lo),
    UsedEventsHiPhiEtaPt (UsedEventsHiPhiEtaPt), UsedEventsLoPhiEtaPt (UsedEventsLoPhiEtaPt)
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
  
  // Quick & dirty charge selector test
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
int AjAnalysis::AnalyzeAndFill ( std::vector<fastjet::PseudoJet>& particles, fastjet::PseudoJet* ToMatch ){
  Has10Gev=false;

  // Select particles to perform analysis on
  // ---------------------------------------
  std::vector<fastjet::PseudoJet> pLo = slo( particles );
  std::vector<fastjet::PseudoJet> pHi = shi( particles );

  // Save for QA
  // -----------
  for ( int i = 0; i< pHi.size(); ++i )
    UsedEventsHiPhiEtaPt->Fill( JetAnalyzer::phimod2pi( pHi.at(i).phi() ), pHi.at(i).eta(),pHi.at(i).pt() );
  
  for ( int i = 0; i< pLo.size(); ++i )
    UsedEventsLoPhiEtaPt->Fill( JetAnalyzer::phimod2pi( pLo.at(i).phi()) , pLo.at(i).eta(),pLo.at(i).pt() );
  
  // find high constituent pT jets
  // -----------------------------
  JetAnalyzer JAhi ( pHi, jet_def ); // NO background subtraction
  JAhiResult = fastjet::sorted_by_pt( JAhi.inclusive_jets() );
  
  if ( JAhiResult.size() < 1 )                 {     return 0; }
  if ( JAhiResult.at(0).pt() > 10 )            { Has10Gev=true; }

  if ( JAhiResult.size() < 2 )                 {     return 0; }
  if ( JAhiResult.at(0).pt() < LeadPtMin )     {     return 0; }
  if ( JAhiResult.at(1).pt() < SubLeadPtMin )  {     return 0; }
      
  // back to back? Answer this question with a selector
  // ---------------------------------------------------
  std::vector<fastjet::PseudoJet> DiJetsHi = SelectorDijets( dPhiCut ) ( sjet ( JAhiResult ) );
  if ( DiJetsHi.size() == 0 ) {
    // std::cout << " NO dijet found" << std::endl;
    return 0;
  }
  assert ( DiJetsHi.size() == 2 && "SelectorDijets returned impossible number of Dijets." );  

  // At least one of these matched to the reference jet?
  if ( ToMatch ){
    if ( !IsMatched( DiJetsHi, *ToMatch, R ) ) return 0;
  }

  // std::cout << " HARD constituents: Found a dijet!" << std::endl;
  // std::cout << " Jet 1: pt = " << DiJetsHi.at(0).pt() << "  eta = " << DiJetsHi.at(0).eta() << std::endl;
  // std::cout << " Jet 2: pt = " << DiJetsHi.at(1).pt() << "  eta = " << DiJetsHi.at(1).eta() << std::endl;

  // Calculate Aj and fill histos -- for unmatched high constituent pT di-jets
  UnmatchedAJ_hi->Fill ( CalcAj( DiJetsHi ) );
  UnmatchedhPtHi->Fill ( DiJetsHi.at(0).pt() , DiJetsHi.at(1).pt());  
  UnmatchedhdPtHi->Fill ( DiJetsHi.at(0).pt() - DiJetsHi.at(1).pt());  
      
  // find corresponding jets with soft constituents
  // ----------------------------------------------
  JetAnalyzer JAlo( pLo, jet_def, area_def ); // WITH background subtraction
  fastjet::Subtractor* BackgroundSubtractor =  JAlo.GetBackgroundSubtractor();
  JAloResult = fastjet::sorted_by_pt( (*BackgroundSubtractor)( JAlo.inclusive_jets() ) );
  
  std::vector<fastjet::PseudoJet> DiJetsLo = SelectorDijets ( dPhiCut ) ( sjet ( JAloResult)  );
  if ( DiJetsLo.size() == 0 ) {
    // std::cout << " NO dijet found" << std::endl;
    return 1;
  }
  assert ( DiJetsLo.size() == 2 && "SelectorDijets returned impossible number of Dijets." );

  // cout << " SOFT constituents: Found a dijet!" << endl;
  // cout << " Jet 1: pt = " << DiJetsLo.at(0).pt() << "  phi = " << DiJetsLo.at(0).phi() << endl;
  // cout << " Jet 2: pt = " << DiJetsLo.at(1).pt() << "  phi = " << DiJetsLo.at(1).phi() << endl;

  // Matched?
  // --------
  // Top two hard constituent jets should be geometrically close to the top two soft ones.
  // Overloading the == operator for vector<PseudoJet> does NOT work.
  // This function is implemented in JetAnalyzer
  if ( !IsMatched( DiJetsHi, DiJetsLo, R ) ){
    // cout << " Couldn't find a match" << endl;
    return 2;
  }

  // And we're done! Calculate A_J
  // -----------------------------
  AJ_hi->Fill( CalcAj( DiJetsHi ) );
  AJ_lo->Fill( CalcAj( DiJetsLo ) );

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
TStarJetPicoReader GetReader ( TString ChainPattern, TString TriggerString, TString ChainName ){
  TStarJetPicoDefinitions::SetDebugLevel(0); // 10 for more output

  TStarJetPicoReader reader;
  TChain* tree = new TChain( ChainName );
  tree->Add( ChainPattern );
  reader.SetInputChain (tree);

  // Event and track selection
  // -------------------------
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  evCuts->SetTriggerSelection( TriggerString ); //All, MB, HT, pp, ppHT, ppJP
  // Additional cuts 
  evCuts->SetVertexZCut (AjParameters::VzCut);
  evCuts->SetRefMultCut (AjParameters::RefMultCut);
  // evCuts->SetVertexZDiffCut( AjParameters::VzDiffCut );

  // Tracks: Some standard high quality cuts
  // TODO: Add track, tower quality cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  // trackCuts->SetDCACut( 1.0 ); // maybe too high for low pT
  // trackCuts->SetMinNFitPointsCut( 20 );
  // trackCuts->SetFitOverMaxPointsCut( 0.52 );
  // trackCuts->SetMaxPtCut ( 30 ); // should it be 10? 15?

  // Towers: Don't know. Let's print out the default
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  std::cout << "Using these tower cuts:" << std::endl;
  std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl;
  std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl;
  
  // V0s: Turn off
  reader.SetProcessV0s(false);

  return reader;
}

// Slightly different, preferred version of GetReader
TStarJetPicoReader SetupReader ( TChain* chain, TString TriggerString ){
  TStarJetPicoDefinitions::SetDebugLevel(0); // 10 for more output

  TStarJetPicoReader reader;
  reader.SetInputChain (chain);

  // Event and track selection
  // -------------------------
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  evCuts->SetTriggerSelection( TriggerString ); //All, MB, HT, pp, ppHT, ppJP
  // Additional cuts 
  evCuts->SetVertexZCut (AjParameters::VzCut);
  evCuts->SetRefMultCut (AjParameters::RefMultCut);
  // evCuts->SetVertexZDiffCut( AjParameters::VzDiffCut );
  
  // Tracks: Some standard high quality cuts
  // TODO: Add track, tower quality cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  // std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
  // std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
  // std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
  // std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl;
  // throw (std::string("done"));
  
  trackCuts->SetDCACut( 1.0 ); // maybe too high for low pT
  trackCuts->SetMinNFitPointsCut( 20 );
  trackCuts->SetFitOverMaxPointsCut( 0.52 );
  trackCuts->SetMaxPtCut ( 30 ); // should it be 10? 15?
  
  // Towers: Don't know. Let's print out the default
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  towerCuts->SetMaxEtCut(40);  // Let's try
  std::cout << "Using these tower cuts:" << std::endl;
  std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl;
  std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl;
  
  // V0s: Turn off
  reader.SetProcessV0s(false);

  return reader;
}

