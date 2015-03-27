/** @file FollowAjAnalysis.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Class for A<SUB>J</SUB> analysis
    @details based on AjAnalysis.cxx, follow small R jets to see if they recover with large R
    @date Mar 26, 2015
*/

#include "FollowAjAnalysis.hh"

// Standard ctor
FollowAjAnalysis::FollowAjAnalysis ( double SmallR, double LargeR,
				     double jet_ptmin, double jet_ptmax,
				     double LeadPtMin, double SubLeadPtMin, 
				     double max_track_rap, double PtConsLo, double PtConsHi,
				     double dPhiCut,
				     TH2D* SmallUnmatchedhPtHi,  TH2D* SmallhPtHi,  TH2D* SmallhPtLo,  
				     TH1D* SmallUnmatchedhdPtHi,  TH1D* SmallhdPtHi,  TH1D* SmallhdPtLo,  
				     TH1D* SmallhdphiHi, TH1D* SmallhdphiLo,
				     TH1D* SmallUnmatchedAJ_hi, TH1D* SmallAJ_hi, TH1D* SmallAJ_lo,
				     TH2D* LargeUnmatchedhPtHi,  TH2D* LargehPtHi,  TH2D* LargehPtLo,
				     TH1D* LargeUnmatchedhdPtHi,  TH1D* LargehdPtHi,  TH1D* LargehdPtLo,
				     TH1D* LargehdphiHi, TH1D* LargehdphiLo,
				     TH1D* LargeUnmatchedAJ_hi, TH1D* LargeAJ_hi, TH1D* LargeAJ_lo,
				     TH3D* UsedEventsHiPhiEtaPt, TH3D* UsedEventsLoPhiEtaPt 
				     )
  : SmallR(SmallR), LargeR(LargeR),
    jet_ptmin(jet_ptmin), jet_ptmax(jet_ptmax),
    LeadPtMin(LeadPtMin), SubLeadPtMin(SubLeadPtMin),
    max_track_rap (max_track_rap), PtConsLo (PtConsLo), PtConsHi (PtConsHi),
    dPhiCut (dPhiCut),
    SmallUnmatchedhPtHi (SmallUnmatchedhPtHi),  SmallhPtHi (SmallhPtHi),  SmallhPtLo (SmallhPtLo),
    SmallUnmatchedhdPtHi (SmallUnmatchedhdPtHi),  SmallhdPtHi (SmallhdPtHi),  SmallhdPtLo (SmallhdPtLo),  
    SmallhdphiHi (SmallhdphiHi), SmallhdphiLo (SmallhdphiLo),
    SmallUnmatchedAJ_hi (SmallUnmatchedAJ_hi), SmallAJ_hi(SmallAJ_hi), SmallAJ_lo (SmallAJ_lo),
    LargeUnmatchedhPtHi (LargeUnmatchedhPtHi),  LargehPtHi (LargehPtHi),  LargehPtLo (LargehPtLo),
    LargeUnmatchedhdPtHi (LargeUnmatchedhdPtHi),  LargehdPtHi (LargehdPtHi),  LargehdPtLo (LargehdPtLo),  
    LargehdphiHi (LargehdphiHi), LargehdphiLo (LargehdphiLo),
    LargeUnmatchedAJ_hi (LargeUnmatchedAJ_hi), LargeAJ_hi(LargeAJ_hi), LargeAJ_lo (LargeAJ_lo),
    UsedEventsHiPhiEtaPt (UsedEventsHiPhiEtaPt), UsedEventsLoPhiEtaPt (UsedEventsLoPhiEtaPt)
{
  // derived rapidity cuts
  // ---------------------
  /// Use largest R for uniform rapidity cuts
  max_rap      = max_track_rap-LargeR; 
  ghost_maxrap = max_rap + 2.0 * LargeR;
  
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
  small_jet_def = fastjet::JetDefinition(fastjet::antikt_algorithm, SmallR);
  large_jet_def = fastjet::JetDefinition(fastjet::antikt_algorithm, LargeR);

  // create an area definition for the clustering
  //----------------------------------------------------------
  // ghosts should go up to the acceptance of the detector or
  // (with infinite acceptance) at least 2R beyond the region
  // where you plan to investigate jets.
  fastjet::GhostedAreaSpec area_spec( ghost_maxrap );
  fastjet::AreaDefinition  area_def(fastjet::active_area, area_spec);
  
  std::cout << " ################################################### " << std::endl;
  std::cout << "Small jet def: " << small_jet_def.description() << std::endl;
  std::cout << "Large jet def: " << large_jet_def.description() << std::endl;
  std::cout << "Area Spec " << area_spec.description() << std::endl;
  std::cout << "Area Def  " << area_def.description() << std::endl;
  std::cout << " ################################################### " << std::endl;

}

// Main analysis method
// ====================
int FollowAjAnalysis::AnalyzeAndFill ( std::vector<fastjet::PseudoJet>& particles, fastjet::PseudoJet* ToMatch ){
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
  
  // find high constituent pT jets with small radius
  // -----------------------------------------------
  JetAnalyzer SmallJAhi ( pHi, small_jet_def ); // NO background subtraction
  SmallJAhiResult = fastjet::sorted_by_pt( SmallJAhi.inclusive_jets() );
  
  if ( SmallJAhiResult.size() < 1 )                 {     return 0; }
  if ( SmallJAhiResult.at(0).pt() > 10 )            { Has10Gev=true; }

  if ( SmallJAhiResult.size() < 2 )                 {     return 0; }
  if ( SmallJAhiResult.at(0).pt() < LeadPtMin )     {     return 0; }
  if ( SmallJAhiResult.at(1).pt() < SubLeadPtMin )  {     return 0; }
      
  // back to back? Answer this question with a selector
  // ---------------------------------------------------
  std::vector<fastjet::PseudoJet> SmallDiJetsHi = SelectorDijets( dPhiCut ) ( sjet ( SmallJAhiResult ) );
  if ( SmallDiJetsHi.size() == 0 ) {
    // std::cout << " NO dijet found" << std::endl;
    return 0;
  }
  assert ( SmallDiJetsHi.size() == 2 && "SelectorDijets returned impossible number of Dijets." );  

  // FOR EMBEDDING: At least one matched to the reference jet?
  // ---------------------------------------------------------
  if ( ToMatch ){
    if ( !IsMatched( SmallDiJetsHi, *ToMatch, SmallR ) ) return 0;
  }

  // Now see if we can match to a larger radius
  // -------------------------------------------
  bool LargeToSmallHiMatched=true;
  JetAnalyzer LargeJAhi ( pHi, large_jet_def ); // NO background subtraction
  LargeJAhiResult = fastjet::sorted_by_pt( LargeJAhi.inclusive_jets() );
  
  if ( LargeJAhiResult.size() < 2 )                 {     LargeToSmallHiMatched=false; }
  if ( LargeJAhiResult.at(0).pt() < LeadPtMin )     {     LargeToSmallHiMatched=false; }
  if ( LargeJAhiResult.at(1).pt() < SubLeadPtMin )  {     LargeToSmallHiMatched=false; }
  
  // back to back? Answer this question with a selector
  // ---------------------------------------------------
  std::vector<fastjet::PseudoJet> LargeDiJetsHi = SelectorDijets( dPhiCut ) ( sjet ( LargeJAhiResult ) );
  if ( LargeDiJetsHi.size() == 0 ) {
    std::cout << "Large R dijet is NOT back-to-back" << std::endl;
    LargeToSmallHiMatched=false;
  }

  // Actually matching the original ones?
  // ------------------------------------
  if ( !IsMatched( LargeDiJetsHi, SmallDiJetsHi, SmallR ) ){
    std::cout << "High pT: Large R dijet is NOT matched to small R one." << std::endl;
    LargeToSmallHiMatched=false;
  } 

  if ( LargeToSmallHiMatched ) assert ( LargeDiJetsHi.size() == 2 && "SelectorDijets returned impossible number of Dijets." );  
  // std::cout << " HARD constituents: Found a dijet!" << std::endl;
  // std::cout << " Jet 1: pt = " << DiJetsHi.at(0).pt() << "  eta = " << DiJetsHi.at(0).eta() << std::endl;
  // std::cout << " Jet 2: pt = " << DiJetsHi.at(1).pt() << "  eta = " << DiJetsHi.at(1).eta() << std::endl;

  // Calculate Aj and fill histos -- for unmatched high constituent pT di-jets
  SmallUnmatchedAJ_hi->Fill ( CalcAj( SmallDiJetsHi ) );
  SmallUnmatchedhPtHi->Fill ( SmallDiJetsHi.at(0).pt() , SmallDiJetsHi.at(1).pt());
  SmallUnmatchedhdPtHi->Fill ( SmallDiJetsHi.at(0).pt() - SmallDiJetsHi.at(1).pt());  
  if ( LargeToSmallHiMatched ){
    LargeUnmatchedAJ_hi->Fill ( CalcAj( LargeDiJetsHi ) );
    LargeUnmatchedhPtHi->Fill ( LargeDiJetsHi.at(0).pt() , LargeDiJetsHi.at(1).pt());
    LargeUnmatchedhdPtHi->Fill ( LargeDiJetsHi.at(0).pt() - LargeDiJetsHi.at(1).pt());  
  }

  // find corresponding jets with soft constituents
  // ----------------------------------------------
  JetAnalyzer SmallJAlo( pLo, small_jet_def, area_def ); // WITH background subtraction
  fastjet::Subtractor* SmallBackgroundSubtractor =  SmallJAlo.GetBackgroundSubtractor();
  SmallJAloResult = fastjet::sorted_by_pt( (*SmallBackgroundSubtractor)( SmallJAlo.inclusive_jets() ) );
  
  std::vector<fastjet::PseudoJet> SmallDiJetsLo = SelectorDijets ( dPhiCut ) ( sjet ( SmallJAloResult)  );
  if ( SmallDiJetsLo.size() == 0 ) {
    // std::cout << " NO dijet found" << std::endl;
    if ( LargeToSmallHiMatched ) return 101;
    return 1;
  }
  assert ( SmallDiJetsLo.size() == 2 && "SelectorDijets returned impossible number of Dijets." );

  // cout << " SOFT constituents: Found a dijet!" << endl;
  // cout << " Jet 1: pt = " << DiJetsLo.at(0).pt() << "  phi = " << DiJetsLo.at(0).phi() << endl;
  // cout << " Jet 2: pt = " << DiJetsLo.at(1).pt() << "  phi = " << DiJetsLo.at(1).phi() << endl;

  // Matched?
  // --------
  // Top two hard constituent jets should be geometrically close to the top two soft ones.
  // Overloading the == operator for vector<PseudoJet> does NOT work.
  // This function is implemented in JetAnalyzer
  if ( !IsMatched( SmallDiJetsHi, SmallDiJetsLo, SmallR ) ){
    // cout << " Couldn't find a match" << endl;
    if ( LargeToSmallHiMatched ) return 102;
    return 2;
  }

  // Now see if we can match to a larger radius
  // -------------------------------------------
  bool LargeToSmallLoMatched=true;
  JetAnalyzer LargeJAlo( pLo, large_jet_def, area_def ); // WITH background subtraction
  fastjet::Subtractor* LargeBackgroundSubtractor =  LargeJAlo.GetBackgroundSubtractor();
  LargeJAloResult = fastjet::sorted_by_pt( (*LargeBackgroundSubtractor)( LargeJAlo.inclusive_jets() ) );
  
  std::vector<fastjet::PseudoJet> LargeDiJetsLo = SelectorDijets ( dPhiCut ) ( sjet ( LargeJAloResult)  );
  if ( LargeDiJetsLo.size() == 0 ) {
    std::cout << "Low pT: No Large R dijet found." << std::endl;
    LargeToSmallLoMatched=false;
  }
  if ( LargeToSmallLoMatched ) assert ( LargeDiJetsLo.size() == 2 && "SelectorDijets returned impossible number of Dijets." );

  if ( LargeToSmallLoMatched ){
    // Matched to high pT?
    // -------------------
    if ( !IsMatched( LargeDiJetsHi, LargeDiJetsLo, LargeR ) ){
      std::cout << "Low pT: Large R dijet is NOT matched to high pT" << std::endl;
      LargeToSmallLoMatched=false;
    }
    
    // Actually matching the original ones?
    // ------------------------------------
    if ( !IsMatched( LargeDiJetsLo, SmallDiJetsLo, SmallR ) ){
      std::cout << "Low pT: Large R dijet is NOT matched to small R one." << std::endl;
      LargeToSmallLoMatched=false;
    }
  }


  // And we're done! Calculate A_J
  // -----------------------------
  SmallAJ_hi->Fill( CalcAj( SmallDiJetsHi ) );
  SmallAJ_lo->Fill( CalcAj( SmallDiJetsLo ) );
  SmallhdphiHi->Fill( JetAnalyzer::phimod2pi( SmallDiJetsHi.at(0).phi() - SmallDiJetsHi.at(1).phi() - TMath::Pi()) );
  SmallhdphiLo->Fill( JetAnalyzer::phimod2pi( SmallDiJetsLo.at(0).phi() - SmallDiJetsLo.at(1).phi() - TMath::Pi()) );  
  SmallhPtHi->Fill( SmallDiJetsHi.at(0).pt(), SmallDiJetsHi.at(1).pt());
  SmallhPtLo->Fill( SmallDiJetsLo.at(0).pt(), SmallDiJetsLo.at(1).pt());
  SmallhdPtHi->Fill( SmallDiJetsHi.at(0).pt() - SmallDiJetsHi.at(1).pt());
  SmallhdPtLo->Fill( SmallDiJetsLo.at(0).pt() - SmallDiJetsLo.at(1).pt());

  
  if (LargeToSmallLoMatched){
    LargeAJ_hi->Fill( CalcAj( LargeDiJetsHi ) );
    LargeAJ_lo->Fill( CalcAj( LargeDiJetsLo ) );
    LargehdphiHi->Fill( JetAnalyzer::phimod2pi( LargeDiJetsHi.at(0).phi() - LargeDiJetsHi.at(1).phi() - TMath::Pi()) );
    LargehdphiLo->Fill( JetAnalyzer::phimod2pi( LargeDiJetsLo.at(0).phi() - LargeDiJetsLo.at(1).phi() - TMath::Pi()) );  
    LargehPtHi->Fill( LargeDiJetsHi.at(0).pt(), LargeDiJetsHi.at(1).pt());
    LargehPtLo->Fill( LargeDiJetsLo.at(0).pt(), LargeDiJetsLo.at(1).pt());
    LargehdPtHi->Fill( LargeDiJetsHi.at(0).pt() - LargeDiJetsHi.at(1).pt());
    LargehdPtLo->Fill( LargeDiJetsLo.at(0).pt() - LargeDiJetsLo.at(1).pt());
  } else {
    std::cout << "Nothing found for large R." << std::endl;
  }

  
  // // cleanup
  // // -------
  // delete JAhi;
  // delete JAlo;
  // pLo.clear();
  // pHi.clear();

  if ( LargeToSmallLoMatched ) return 103;
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

