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
				     TH1D* LeadDeltaPtHi,  TH1D* SubLeadDeltaPtHi, TH1D* LeadDeltaPtLo,  TH1D* SubLeadDeltaPtLo,
				     TH2D* SmallUnmatchedhPtHi,  TH2D* SmallhPtHi,  TH2D* SmallhPtLo,
				     TH1D* SmallhdphiHi, TH1D* SmallhdphiLo,
				     TH1D* SmallUnmatchedAJ_hi, TH1D* SmallAJ_hi, TH1D* SmallAJ_lo,
				     TH2D* LargehPtHi,  TH2D* LargehPtLo,
				     TH1D* LargehdphiHi, TH1D* LargehdphiLo,
				     TH1D* LargeAJ_hi, TH1D* LargeAJ_lo,
				     TH3D* UsedEventsHiPhiEtaPt, TH3D* UsedEventsLoPhiEtaPt,
				     TH1D* SmallDeltaAJ_hilo, TH1D* LargeDeltaAJ_hilo,
				     TH1D* DeltaAJ_hi, TH1D* DeltaAJ_lo

				     )
  : SmallR(SmallR), LargeR(LargeR),
    jet_ptmin(jet_ptmin), jet_ptmax(jet_ptmax),
    LeadPtMin(LeadPtMin), SubLeadPtMin(SubLeadPtMin),
    max_track_rap (max_track_rap), PtConsLo (PtConsLo), PtConsHi (PtConsHi),
    dPhiCut (dPhiCut),
    LeadDeltaPtHi (LeadDeltaPtHi),  SubLeadDeltaPtHi (SubLeadDeltaPtHi),
    LeadDeltaPtLo (LeadDeltaPtLo),  SubLeadDeltaPtLo (SubLeadDeltaPtLo),
    SmallUnmatchedhPtHi (SmallUnmatchedhPtHi),  SmallhPtHi (SmallhPtHi),  SmallhPtLo (SmallhPtLo),
    SmallhdphiHi (SmallhdphiHi), SmallhdphiLo (SmallhdphiLo),
    SmallUnmatchedAJ_hi (SmallUnmatchedAJ_hi), SmallAJ_hi(SmallAJ_hi), SmallAJ_lo (SmallAJ_lo),
    LargehPtHi (LargehPtHi),  LargehPtLo (LargehPtLo),
    LargehdphiHi (LargehdphiHi), LargehdphiLo (LargehdphiLo),
    LargeAJ_hi(LargeAJ_hi), LargeAJ_lo (LargeAJ_lo),
    UsedEventsHiPhiEtaPt (UsedEventsHiPhiEtaPt), UsedEventsLoPhiEtaPt (UsedEventsLoPhiEtaPt),
    SmallDeltaAJ_hilo (SmallDeltaAJ_hilo),  LargeDeltaAJ_hilo (LargeDeltaAJ_hilo),
    DeltaAJ_hi(DeltaAJ_hi),  DeltaAJ_lo(DeltaAJ_lo)
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

  // First run standard analysis with small radius
  // ---------------------------------------------
  
  // find high constituent pT jets
  // -----------------------------
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

  // Calculate Aj and fill histos -- for unmatched high constituent pT di-jets
  SmallUnmatchedAJ_hi->Fill ( CalcAj( SmallDiJetsHi ) );
  SmallUnmatchedhPtHi->Fill ( SmallDiJetsHi.at(0).pt() , SmallDiJetsHi.at(1).pt());

  // find corresponding jets with soft constituents
  // ----------------------------------------------
  JetAnalyzer SmallJAlo( pLo, small_jet_def, area_def ); // WITH background subtraction
  fastjet::Subtractor* SmallBackgroundSubtractor =  SmallJAlo.GetBackgroundSubtractor();
  SmallJAloResult = fastjet::sorted_by_pt( (*SmallBackgroundSubtractor)( SmallJAlo.inclusive_jets() ) );
  
  std::vector<fastjet::PseudoJet> SmallDiJetsLo;  
  fastjet::Selector SmallSelectClose = fastjet::SelectorCircle( SmallR );
  
  // Leading:
  SmallSelectClose.set_reference( SmallDiJetsHi.at(0) );
  std::vector<fastjet::PseudoJet> SmallMatchedToLead = sorted_by_pt( SmallSelectClose( SmallJAloResult ));
  if ( SmallMatchedToLead.size() == 0 ) {
    std::cerr << "PROBLEM: SmallSelectorClose returned no match to leading jet." << std::endl;
  } else {
    SmallDiJetsLo.push_back ( SmallMatchedToLead.at(0) );
  }

  SmallSelectClose.set_reference( SmallDiJetsHi.at(1) );
  std::vector<fastjet::PseudoJet> SmallMatchedToSubLead = sorted_by_pt(SmallSelectClose( SmallJAloResult ));
  if ( SmallMatchedToSubLead.size() == 0 ) {
    std::cerr << "PROBLEM: SmallSelectorClose returned no match to sub-leading jet." << std::endl;
  } else {
    SmallDiJetsLo.push_back ( SmallMatchedToSubLead.at(0) );
  }
  

  // Stop here if for whatever reason we don't have two matched pairs
  // ----------------------------------------------------------------
  if ( SmallDiJetsHi.size()!=2 || SmallDiJetsLo.size()!=2 ) return 2;
      
  // Diagnostic
  if ( fabs( SmallDiJetsLo.at(0).eta())>max_rap ) std:: cerr << "Uh-oh... Small R Lead jet eta = "    << SmallDiJetsLo.at(0).eta() << std::endl;
  if ( fabs( SmallDiJetsLo.at(1).eta())>max_rap ) std:: cerr << "Uh-oh... Small R SubLead jet eta = " << SmallDiJetsLo.at(1).eta() << std::endl;
    
  // Fill small R histos
  SmallAJ_hi->Fill( CalcAj( SmallDiJetsHi ) );
  SmallAJ_lo->Fill( CalcAj( SmallDiJetsLo ) );
  SmallhdphiHi->Fill( JetAnalyzer::phimod2pi( SmallDiJetsHi.at(0).phi() - SmallDiJetsHi.at(1).phi() - TMath::Pi()) );
  SmallhdphiLo->Fill( JetAnalyzer::phimod2pi( SmallDiJetsLo.at(0).phi() - SmallDiJetsLo.at(1).phi() - TMath::Pi()) );  
  SmallhPtHi->Fill( SmallDiJetsHi.at(0).pt(), SmallDiJetsHi.at(1).pt());
  SmallhPtLo->Fill( SmallDiJetsLo.at(0).pt(), SmallDiJetsLo.at(1).pt());

  if ( SmallDeltaAJ_hilo ) SmallDeltaAJ_hilo->Fill( CalcAj( SmallDiJetsHi ) - CalcAj( SmallDiJetsLo ) );

  // Now match to large radius
  // -------------------------
  // KEEP IN MIND: Could choose SmallR, but large seems more logical
  fastjet::Selector LargeSelectClose = fastjet::SelectorCircle( LargeR ); 

  // Hi pT constituents
  // ------------------
  JetAnalyzer LargeJAhi ( pHi, large_jet_def ); // NO background subtraction
  LargeJAhiResult = fastjet::sorted_by_pt( LargeJAhi.inclusive_jets() );  
  std::vector<fastjet::PseudoJet> LargeDiJetsHi;  

  // Leading:
  LargeSelectClose.set_reference( SmallDiJetsHi.at(0) );
  std::vector<fastjet::PseudoJet> LargeMatchedToLeadHi = sorted_by_pt( LargeSelectClose( LargeJAhiResult ));
  if ( LargeMatchedToLeadHi.size() == 0 ) {
    std::cerr << "PROBLEM: LargeSelectorClose returned no high pT cut match to leading jet." << std::endl;    
  } else {
    LargeDiJetsHi.push_back ( LargeMatchedToLeadHi.at(0) );
  }
  // SubLeading:
  LargeSelectClose.set_reference( SmallDiJetsHi.at(1) );
  std::vector<fastjet::PseudoJet> LargeMatchedToSubLeadHi = sorted_by_pt( LargeSelectClose( LargeJAhiResult ));
  if ( LargeMatchedToSubLeadHi.size() == 0 ) {
    std::cerr << "PROBLEM: LargeSelectorClose returned no high pT cut match to sub-leading jet." << std::endl;    
  } else {
    LargeDiJetsHi.push_back ( LargeMatchedToSubLeadHi.at(0) );
  }


  // Lo pT constituents
  // ------------------
  JetAnalyzer LargeJAlo( pLo, large_jet_def, area_def ); // WITH background subtraction
  fastjet::Subtractor* LargeBackgroundSubtractor =  LargeJAlo.GetBackgroundSubtractor();
  LargeJAloResult = fastjet::sorted_by_pt( (*LargeBackgroundSubtractor)( LargeJAlo.inclusive_jets() ) );
  std::vector<fastjet::PseudoJet> LargeDiJetsLo;  

  // Leading:
  LargeSelectClose.set_reference( SmallDiJetsLo.at(0) );
  std::vector<fastjet::PseudoJet> LargeMatchedToLeadLo = sorted_by_pt( LargeSelectClose( LargeJAloResult ));
  if ( LargeMatchedToLeadLo.size() == 0 ) {
    std::cerr << "PROBLEM: LargeSelectorClose returned no logh pT cut match to leading jet." << std::endl;    
  } else {
    LargeDiJetsLo.push_back ( LargeMatchedToLeadLo.at(0) );
  }
  // SubLeading:
  LargeSelectClose.set_reference( SmallDiJetsLo.at(1) );
  std::vector<fastjet::PseudoJet> LargeMatchedToSubLeadLo = sorted_by_pt( LargeSelectClose( LargeJAloResult ));
  if ( LargeMatchedToSubLeadLo.size() == 0 ) {
    std::cerr << "PROBLEM: LargeSelectorClose returned no logh pT cut match to sub-leading jet." << std::endl;    
  } else {
    LargeDiJetsLo.push_back ( LargeMatchedToSubLeadLo.at(0) );
  }

  // Stop here if for whatever reason we don't have corresponding dijets
  // -------------------------------------------------------------------
  if ( LargeDiJetsHi.size()!=2 || LargeDiJetsLo.size()!=2 ) return 101;

  if ( fabs( LargeDiJetsLo.at(0).eta())>max_rap ) std:: cerr << "Uh-oh... Large R, Lo,  Lead jet eta = "    << LargeDiJetsLo.at(0).eta() << std::endl;
  if ( fabs( LargeDiJetsLo.at(1).eta())>max_rap ) std:: cerr << "Uh-oh... Large R, Lo, SubLead jet eta = " << LargeDiJetsLo.at(1).eta() << std::endl;
  
  if ( fabs( LargeDiJetsHi.at(0).eta())>max_rap ) std:: cerr << "Uh-oh... Large R, Hi,  Lead jet eta = "    << LargeDiJetsHi.at(0).eta() << std::endl;
  if ( fabs( LargeDiJetsHi.at(1).eta())>max_rap ) std:: cerr << "Uh-oh... Large R, Hi, SubLead jet eta = " << LargeDiJetsHi.at(1).eta() << std::endl;

  // Fill remaining histos.
  LargeAJ_hi->Fill( CalcAj( LargeDiJetsHi ) );
  LargeAJ_lo->Fill( CalcAj( LargeDiJetsLo ) );

  if ( DeltaAJ_hi )         DeltaAJ_hi->Fill( CalcAj( SmallDiJetsHi ) - CalcAj( LargeDiJetsHi ) );
  if ( DeltaAJ_hi )         DeltaAJ_lo->Fill( CalcAj( SmallDiJetsLo ) - CalcAj( LargeDiJetsLo ) );
  if ( LargeDeltaAJ_hilo )  LargeDeltaAJ_hilo->Fill( CalcAj( LargeDiJetsHi ) - CalcAj( LargeDiJetsLo ) );
  
  LargehdphiHi->Fill( JetAnalyzer::phimod2pi( LargeDiJetsHi.at(0).phi() - LargeDiJetsHi.at(1).phi() - TMath::Pi()) );
  LargehdphiLo->Fill( JetAnalyzer::phimod2pi( LargeDiJetsLo.at(0).phi() - LargeDiJetsLo.at(1).phi() - TMath::Pi()) );  
  LargehPtHi->Fill( LargeDiJetsHi.at(0).pt(), LargeDiJetsHi.at(1).pt());
  LargehPtLo->Fill( LargeDiJetsLo.at(0).pt(), LargeDiJetsLo.at(1).pt());

  LeadDeltaPtHi   ->Fill( SmallDiJetsHi.at(0).pt() - LargeDiJetsHi.at(0).pt());
  SubLeadDeltaPtHi->Fill( SmallDiJetsHi.at(1).pt() - LargeDiJetsHi.at(1).pt());
  LeadDeltaPtLo   ->Fill( SmallDiJetsLo.at(0).pt() - LargeDiJetsLo.at(0).pt());
  SubLeadDeltaPtLo->Fill( SmallDiJetsLo.at(1).pt() - LargeDiJetsLo.at(1).pt());

  
  // // cleanup
  // // -------
  // delete JAhi;
  // delete JAlo;
  // pLo.clear();
  // pHi.clear();

  return 103;
  // return 3;
  
}


// Helper to deal with repetitive stuff
TStarJetPicoReader GetReader ( TString ChainPattern, TString TriggerString, TString ChainName, const double RefMultCut ){
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
  evCuts->SetRefMultCut ( RefMultCut);
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

// Slightly different, preferred version of GetReader
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
  
  // Tracks: Some standard high quality cuts
  // TODO: Add track, tower quality cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  // std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
  // std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
  // std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
  // std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl;
  // throw (std::string("done"));
  
  trackCuts->SetDCACut( AjParameters::DcaCut ); 
  trackCuts->SetMinNFitPointsCut( AjParameters::NMinFit );
  trackCuts->SetFitOverMaxPointsCut( AjParameters::FitOverMaxPointsCut );
  trackCuts->SetMaxPtCut ( AjParameters::MaxTrackPt );
  
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

