#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>

#include <TLorentzVector.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TChain.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <exception>


using namespace std;

int MatchGeantToPythia ( ) {
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  TLegend* leg = 0;
  
  float RCut = 0.4;
  float EtaCut = 1.0-RCut;

  // Output
  // ------
  TString OutFileName = "GeantMatching.root";
    
  // Input
  // -----
  TString DetLevelFile = "AjResults/Tow0_Eff0_Groom_Aj_HT54_Geant.root"; // pp-like events
  TString McLevelFile  = "AjResults/Groom_Aj_TrueMB_NoEff_GeantMc.root"; // Reference (particle level) jets

  // Refmult Cut
  // -----------
  int RefMultCut=0;

  // Load MC level tree
  // ------------------
  TChain* McJets = new TChain("TriggeredTree");
  McJets->Add ( McLevelFile );
  McJets->BuildIndex("runid","eventid");

  // The leading jet above 10 GeV that caused
  // this event to be recorded
  TClonesArray* McTrigger = new TClonesArray("TLorentzVector");
  McJets->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  McJets->SetBranchAddress("TriggerJet", &McTrigger);

  TClonesArray* McAwayJet = new TClonesArray("TLorentzVector");
  McJets->GetBranch("AwayJet")->SetAutoDelete(kFALSE);
  McJets->SetBranchAddress("AwayJet", &McAwayJet);

  // Only filled when AJ pair is found
  TLorentzVector* Mcj1;
  McJets->SetBranchAddress("j1", &Mcj1);

  TLorentzVector* Mcj2;
  McJets->SetBranchAddress("j2", &Mcj2);
  
  // TLorentzVector* Mcjm1;
  // McJets->SetBranchAddress("jm1", &Mcjm1);

  // TLorentzVector* Mcjm2;
  // McJets->SetBranchAddress("jm2", &Mcjm2);
  
  int mceventid;
  int mcrunid;
  double mcweight;     // Double-check, should be the same as below
  McJets->SetBranchAddress("eventid", &mceventid);
  McJets->SetBranchAddress("runid", &mcrunid);
  McJets->SetBranchAddress("weight",&mcweight );

  float mczg1, mczg2, mczgm1, mczgm2;
  float mcaj_hi, mcaj_lo;
  McJets->SetBranchAddress("zg1",&mczg1 );
  McJets->SetBranchAddress("zg2",&mczg2 );
  McJets->SetBranchAddress("zgm1",&mczgm1 );
  McJets->SetBranchAddress("zgm2",&mczgm2 );
  McJets->SetBranchAddress("aj_lo",&mcaj_lo );
  McJets->SetBranchAddress("aj_hi",&mcaj_hi );
  
  // Set up pp events
  // ----------------
  TChain* ppJets = new TChain("TriggeredTree");
  ppJets->Add( DetLevelFile );
  assert ( ppJets->GetEntries()>0 && "Something went wrong loading the pp jets.");
  
  TClonesArray* pFullEvent = new TClonesArray("TStarJetVector");
  ppJets->GetBranch("FullEvent")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("FullEvent", &pFullEvent);

  TClonesArray* pTriggerJet = new TClonesArray("TLorentzVector");
  ppJets->GetBranch("TriggerJet")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("TriggerJet", &pTriggerJet);

  TClonesArray* pAwayJet = new TClonesArray("TLorentzVector");
  ppJets->GetBranch("AwayJet")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("AwayJet", &pAwayJet);

  TLorentzVector* pOrigJ1 = new TLorentzVector();
  ppJets->GetBranch("j1")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("j1", &pOrigJ1);

  TLorentzVector* pOrigJ2 = new TLorentzVector();
  ppJets->GetBranch("j2")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("j2", &pOrigJ2);

  TLorentzVector* pOrigJM1 = new TLorentzVector();
  ppJets->GetBranch("jm1")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("jm1", &pOrigJM1);

  TLorentzVector* pOrigJM2 = new TLorentzVector();
  ppJets->GetBranch("jm2")->SetAutoDelete(kFALSE);
  ppJets->SetBranchAddress("jm2", &pOrigJM2);

  int deteventid;
  int detrunid;
  double weight;     // Primarily to stitch together MC data
  ppJets->SetBranchAddress("eventid", &deteventid);
  ppJets->SetBranchAddress("runid", &detrunid);
  ppJets->SetBranchAddress("weight",&weight );

  float detzg1, detzg2, detzgm1, detzgm2;
  float detaj_hi, detaj_lo;
  ppJets->SetBranchAddress("zg1",&detzg1 );
  ppJets->SetBranchAddress("zg2",&detzg2 );
  ppJets->SetBranchAddress("zgm1",&detzgm1 );
  ppJets->SetBranchAddress("zgm2",&detzgm2 );
  ppJets->SetBranchAddress("aj_lo",&detaj_lo );
  ppJets->SetBranchAddress("aj_hi",&detaj_hi );

  // Output and histograms
  // ----------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // Trigger jet info
  TH1D* McTrigPt = new TH1D( "McTrigPt",";Trigger p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* McTrigPtMatched = new TH1D( "McTrigPtMatched",";Trigger p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* DetTrigPt = new TH1D( "DetTrigPt",";Trigger p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH2D* TriggerJetForLostMc = new TH2D( "TriggerJetForLostMc", ";p_{T};#eta", 120, 10, 70, 100,-1,1 );

  // Dijets
  TH1D* McJ1Pt = new TH1D( "McJ1Pt",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* McJ1PtMatched = new TH1D( "McJ1PtMatched",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* DetJ1Pt = new TH1D( "DetJ1Pt",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* McJM1PtMatched = new TH1D( "McJM1PtMatched",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* DetJM1Pt = new TH1D( "DetJM1Pt",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );

  TH1D* McJ2Pt = new TH1D( "McJ2Pt",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* McJ2PtMatched = new TH1D( "McJ2PtMatched",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* DetJ2Pt = new TH1D( "DetJ2Pt",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* McJM2PtMatched = new TH1D( "McJM2PtMatched",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );
  TH1D* DetJM2Pt = new TH1D( "DetJM2Pt",";p_{T}^{Jet} [GeV/c]", 140, 10 , 80 );

  // AJ
  TH1D* McAJ = new TH1D( "McAJ",";A_{J};fraction", 50, -0.6, 0.9 );
  TH1D* McAJMatched = new TH1D( "McAJMatched",";A_{J};fraction", 50, -0.6, 0.9 );
  TH1D* DetAJ_hi = new TH1D( "DetAJ_hi",";A_{J};fraction", 50, -0.6, 0.9 );
  TH1D* DetAJ_lo = new TH1D( "DetAJ_lo",";A_{J};fraction", 50, -0.6, 0.9 );

  // zg Lead
  int nzgBins=40;
  TH1D* MczgLead2030Lo = new TH1D("MczgLead2030Lo","Leading Lo jet with 20<p_{T}<30;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* MczgLead3040Lo = new TH1D("MczgLead3040Lo","Leading Lo jet with 30<p_{T}<40;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* MczgLead2030LoMatched = new TH1D("MczgLead2030LoMatched","Leading Lo jet with 20<p_{T}<30;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* MczgLead3040LoMatched = new TH1D("MczgLead3040LoMatched","Leading Lo jet with 30<p_{T}<40;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* DetzgLead2030Lo = new TH1D("DetzgLead2030Lo","Leading Lo jet with 20<p_{T}<30;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* DetzgLead3040Lo = new TH1D("DetzgLead3040Lo","Leading Lo jet with 30<p_{T}<40;z_{g}", nzgBins, 0.05, 0.55);

  // zg SubLead
  TH1D* MczgSubLead1020Lo = new TH1D("MczgSubLead1020Lo","SubLeading Lo jet with 30<p_{T}<40;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* MczgSubLead2030Lo = new TH1D("MczgSubLead2030Lo","SubLeading Lo jet with 20<p_{T}<30;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* MczgSubLead1020LoMatched = new TH1D("MczgSubLead1020LoMatched","SubLeading Lo jet with 30<p_{T}<40;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* MczgSubLead2030LoMatched = new TH1D("MczgSubLead2030LoMatched","SubLeading Lo jet with 20<p_{T}<30;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* DetzgSubLead1020Lo = new TH1D("DetzgSubLead1020Lo","SubLeading Lo jet with 30<p_{T}<40;z_{g}", nzgBins, 0.05, 0.55);
  TH1D* DetzgSubLead2030Lo = new TH1D("DetzgSubLead2030Lo","SubLeading Lo jet with 20<p_{T}<30;z_{g}", nzgBins, 0.05, 0.55);

  // ------------------------
  // Loop over particle level
  // ------------------------

  cout << "Loop over particle level" << endl;
  cout << "------------------------" << endl;

  for ( Long64_t mcEvi = 0; mcEvi< McJets->GetEntries() ; ++mcEvi ){
    if ( !(mcEvi%10000) ) cout << "Working on " << mcEvi << " / " << McJets->GetEntries() << endl;
    McJets->GetEntry(mcEvi);

    TLorentzVector *McTriggerJet = (TLorentzVector*) McTrigger->At(0);
    if ( fabs ( McTriggerJet->Eta() ) > EtaCut ) continue;

    McTrigPt->Fill( McTriggerJet->Pt(), mcweight );

    // Dijets:
    if ( Mcj1->Pt() < 1e-1 ) continue;
    if ( Mcj2->Pt() < 1e-1 ) {
      cerr << "This should never happen!" << endl;
      return -1;
    }

    McJ1Pt->Fill( Mcj1->Pt(), mcweight );
    McJ2Pt->Fill( Mcj2->Pt(), mcweight );

    McAJ->Fill( mcaj_lo ); 

    float Mcpt1 = Mcj1->Pt();
    if ( Mcpt1 >= 20 && Mcpt1 < 30 ){
      MczgLead2030Lo->Fill ( mczgm1, mcweight );
    }
    if ( Mcpt1 >= 30 && Mcpt1 < 40 ){
      MczgLead3040Lo->Fill ( mczgm1, mcweight );
    }

    float Mcpt2 = Mcj2->Pt();
    if ( Mcpt2 >= 10 && Mcpt2 < 20 ){
      MczgSubLead1020Lo->Fill ( mczgm2, mcweight );
    }
    if ( Mcpt2 >= 20 && Mcpt2 < 30 ){
      MczgSubLead2030Lo->Fill ( mczgm2, mcweight );
    }

  }

  // ------------------------
  // Loop over detector level
  // ------------------------

  cout << "Loop over detector level" << endl;
  cout << "------------------------" << endl;

  // count how many events were found after geant that
  // missed the cut in MC
  int lostmc=0;
  int leadinglost=0;
  int triggermismatch=0;
  int largeetamcdijets=0;
  int nodijet=0;
  int lostmcdijet=0;
  int newdetdijet=0;
  int softmatchlost=0;
  int accepteddijets=0;
  int acceptedevents=0;
  for ( Long64_t ppEv = 0; ppEv< ppJets->GetEntries() ; ++ppEv ){
    if ( !(ppEv%10000) ) cout << "Working on " << ppEv << " / " << ppJets->GetEntries() << endl;
    ppJets->GetEntry(ppEv);

    if (pTriggerJet->GetEntries()!=1 ){
      cout << " No trigger jet in data?!" << endl;
      return -1;
    } 
    TLorentzVector* tdet = (TLorentzVector*) pTriggerJet->At(0);

    // Get corresponding MC event
    // --------------------------
    Long64_t mcevi=-1;
    mcevi = McJets->GetEntryNumberWithIndex( detrunid, deteventid );
    if ( mcevi < 0 ){
      TriggerJetForLostMc->Fill( tdet->Pt(),tdet->Eta() );
      lostmc++;
      
      // Most losses are due to the high tower cut allowing
      // raw hadronic towers that didn't pass at the MC level
      // (because they were charged and thus not tested)
      // But there's also a small fraction where GEANT seems to "harden" the jet
      // such that there's a sizable trigger jet with constituents above 2 in GEANT
      // but at the truth level it was too soft to trigger recording.
      // In any case, this is currently a "ghost", and I cannot connect it to truth
      // So... disregard it
      // I don't even know how to deal with cross-section weights like this...
      continue;
    }

    McJets->GetEntry( mcevi );
    if ( fabs(mcweight / weight - 1) > 1e-4 ) {
      // Sanity check
      cerr << "mcweight / weight = " << mcweight / weight  << endl;
      cerr << "mcweight = " <<  mcweight  << "    weight = " <<  weight << endl;
      cout << "Mc: runid = " << mcrunid << "  eventid = " << mceventid << endl;
      cout << "Mc: evi = " << mcevi << endl;
      cout << "Det: runid = " << detrunid << "  eventid = " << deteventid << endl;
      // cout << "Signal: evi = " << evi << endl;

      return -1;
    }
	
    // Only accept if original MC jet is in the acceptance
    TLorentzVector *McTriggerJet = (TLorentzVector*) McTrigger->At(0);
    if ( fabs ( McTriggerJet->Eta()) > EtaCut ) {
      TriggerJetForLostMc->Fill( tdet->Pt(),tdet->Eta() );
      leadinglost++;
      continue;
    }

    // This is mostly useless.
    TLorentzVector* adet = 0;
    if (pAwayJet->GetEntries()!=1 ) adet = (TLorentzVector*) pAwayJet->At(0);

    bool triggermatched=false;
    bool awaymatched=false;
    if ( McTriggerJet->DeltaR( *tdet ) < RCut ) triggermatched=true;
    if ( adet ){
      // if we have an away-side jet, allow that one to match as well      
      // cout << "    Checking away-side" << endl;
      if ( McTriggerJet->DeltaR( *adet ) < RCut) awaymatched=true;
    }
    
    // Enforcing trigger matching loses about 1%
    // And it's questionable to detect. So ignore for now
    if ( false ){
      if ( !triggermatched ){
	// cout << "Trigger Jet mismatch: " << McTriggerJet->DeltaR( *tdet ) << endl;
	if ( awaymatched ) {
	  // cout << "Trigger Jet mismatch: " << McTriggerJet->DeltaR( *tdet ) << endl;
	  // cout << " --> But away-side is matched"<< endl;
	  // Sigh. This never works for some reason. Let's just allow everything on the away-side
	} else if ( fabs( TVector2::Phi_mpi_pi ( McTriggerJet->DeltaPhi( *tdet ) - TMath::Pi()) ) < RCut ){
	  // cout << "Trigger Jet mismatch: " << McTriggerJet->DeltaR( *tdet ) << endl;
	  // cout << " --> But at least close in phi to away-side"<< endl;
	} else {
	  // cout << "-- Trigger Jet mismatch: DeltaR      = " << McTriggerJet->DeltaR( *tdet ) << endl;
	  // cout << "-- Trigger Jet mismatch: DeltaPhi-Pi = " <<   TVector2::Phi_mpi_pi ( McTriggerJet->DeltaPhi( *tdet ) - TMath::Pi() ) << endl;
	  triggermismatch++;
	  continue;
	}    
      }
    }

    // Okay, we have an event that's acceptable in MC and detector level.
    // Record trigger jet data
    acceptedevents++;
    McTrigPtMatched->Fill( McTriggerJet->Pt(), mcweight );
    DetTrigPt->Fill ( tdet->Pt(), mcweight );

    // Now let's look at dijets
    bool mcdijet=false;
    bool detdijet=false;
    if ( Mcj1->Pt() > 1e-1 ) {
      if( fabs(Mcj1->Eta())> EtaCut || fabs(Mcj2->Eta())> EtaCut ){ 
	largeetamcdijets++;
	continue;
      }
      mcdijet=true;
    }
    if ( pOrigJ1->Pt() > 1e-1 ) detdijet=true;

    if ( !mcdijet && !detdijet ){ nodijet++; continue; }
    if ( mcdijet && !detdijet ){ lostmcdijet++; continue; }
    if ( !mcdijet && detdijet ){ newdetdijet++; continue; }


    accepteddijets++;
    McJ1PtMatched->Fill( Mcj1->Pt(), mcweight );
    McJ2PtMatched->Fill( Mcj2->Pt(), mcweight );
    McAJMatched->Fill( mcaj_lo, mcweight );
    
    DetJ1Pt->Fill( pOrigJ1->Pt(), mcweight );
    DetJ2Pt->Fill( pOrigJ2->Pt(), mcweight );
    DetAJ_hi->Fill( detaj_hi, mcweight );


    // soft-matched?
    if ( pOrigJM1->Pt()<1e-1 || pOrigJM2->Pt()<1e-1 ){
      softmatchlost++;
      continue;
    }
    DetJM1Pt->Fill( pOrigJM1->Pt(), mcweight );
    DetJM2Pt->Fill( pOrigJM2->Pt(), mcweight );
    DetAJ_lo->Fill( detaj_lo, mcweight );

    // zg Lead
    // -------
    // Question: What pT to use here? Start with Mc
    // float Mcpt1 = Mcj1->Pt();
    // That seems kinda bad. Let's try det level (true match)
    float Mcpt1 = pOrigJ1->Pt();
    if ( Mcpt1 >= 20 && Mcpt1 < 30 ){
      MczgLead2030LoMatched->Fill ( mczgm1, mcweight );
    }
    if ( Mcpt1 >= 30 && Mcpt1 < 40 ){
      MczgLead3040LoMatched->Fill ( mczgm1, mcweight );
    }

    float Detpt1 = pOrigJ1->Pt();
    if ( Detpt1 >= 20 && Detpt1 < 30 ){
      DetzgLead2030Lo->Fill ( detzgm1, mcweight );
    }
    if ( Detpt1 >= 30 && Detpt1 < 40 ){
      DetzgLead3040Lo->Fill ( detzgm1, mcweight );
    }

    // zg SubLead
    // ----------
    // Question: What pT to use here? Start with Mc
    // float Mcpt2 = Mcj2->Pt();
    // That seems kinda bad. Let's try det level (true match)
    float Mcpt2 = pOrigJ2->Pt();
    if ( Mcpt2 >= 10 && Mcpt2 < 20 ){
      MczgSubLead1020LoMatched->Fill ( mczgm2, mcweight );
    }
    if ( Mcpt2 >= 20 && Mcpt2 < 30 ){
      MczgSubLead2030LoMatched->Fill ( mczgm2, mcweight );
    }

    float Detpt2 = pOrigJ2->Pt();
    if ( Detpt2 >= 10 && Detpt2 < 20 ){
      DetzgSubLead1020Lo->Fill ( detzgm2, mcweight );
    }
    if ( Detpt2 >= 20 && Detpt2 < 30 ){
      DetzgSubLead2030Lo->Fill ( detzgm2, mcweight );
    }

	  
  }
  // Plot trigger spectra
  // --------------------
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  leg = new TLegend( 0.55, 0.55, 0.89, 0.9, "Trigger Jet (leading, above 10 GeV)" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  McTrigPt->SetLineColor(kGreen+1);
  McTrigPtMatched->SetLineColor(kBlue);
  DetTrigPt->SetLineColor(kRed+1);

  McTrigPt->Draw();
  leg->AddEntry( McTrigPt, "MC truth, no bias", "l");
  McTrigPtMatched->Draw("same");
  leg->AddEntry( McTrigPtMatched, "MC truth, matched to Det", "l");
  DetTrigPt->Draw("same");
  leg->AddEntry( DetTrigPt, "Detector level Hard-Core", "l");
  leg->Draw();
  gPad->SaveAs( "GeantDiagnosis/TriggerJetSpectra.png");

  // Draw dijet spectra
  // ------------------
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  leg = new TLegend( 0.5, 0.55, 0.89, 0.9, "Dijets:" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  McJ1Pt->SetLineColor(kGreen+1);
  McJ1Pt->Draw();
  leg->AddEntry( McJ1Pt, "Leading MC truth, no bias", "l");

  McJ1PtMatched->SetLineColor(kBlue);
  McJ1PtMatched->Draw("same");
  leg->AddEntry( McJ1PtMatched, "Leading MC truth, matched", "l");

  DetJ1Pt->SetLineColor(kRed+1);
  DetJ1Pt->Draw("same");
  leg->AddEntry( DetJ1Pt, "Leading Detector level Hard-Core", "l");

  DetJM1Pt->SetLineColor(kGray+2);
  DetJM1Pt->Draw("same");
  leg->AddEntry( DetJM1Pt, "Leading Detector level Soft-Match", "l");

  leg->Draw();
  gPad->SaveAs( "GeantDiagnosis/LeadJetSpectra.png");

  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  leg = new TLegend( 0.5, 0.55, 0.89, 0.9, "Dijets:" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  McJ2Pt->SetLineColor(kGreen+1);
  McJ2Pt->Draw("");
  leg->AddEntry( McJ2Pt, "Sub-Leading MC truth, no bias", "l");

  McJ2PtMatched->SetLineColor(kBlue);
  McJ2PtMatched->Draw("same");
  leg->AddEntry( McJ2PtMatched, "Sub-Leading MC truth, matched", "l");

  DetJ2Pt->SetLineColor(kRed+1);
  DetJ2Pt->Draw("same");
  leg->AddEntry( DetJ2Pt, "Sub-Leading Detector level Hard-Core", "l");

  DetJM2Pt->SetLineColor(kGray+2);
  DetJM2Pt->Draw("same");
  leg->AddEntry( DetJM2Pt, "Sub-Leading Detector level Soft-Match", "l");

  leg->Draw();
  gPad->SaveAs( "GeantDiagnosis/SubLeadJetSpectra.png");
  
  // Draw AJ
  // -------
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  leg = new TLegend( 0.5, 0.55, 0.89, 0.9, "A_{J}" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  McAJ->Scale (1./McAJ->Integral(1,McAJ->GetNbinsX()) );
  McAJMatched->Scale (1./McAJMatched->Integral(1,McAJMatched->GetNbinsX()) );
  DetAJ_hi->Scale (1./DetAJ_hi->Integral(1,DetAJ_hi->GetNbinsX()) );
  DetAJ_lo->Scale (1./DetAJ_lo->Integral(1,DetAJ_lo->GetNbinsX()) );

  //McAJ->SetAxisRange(  0.0, 0.25, "y");
  McAJ->SetAxisRange(  -0.3, 0.8, "x");
  McAJMatched->SetAxisRange(  -0.3, 0.8, "x");
  DetAJ_hi->SetAxisRange(  -0.3, 0.8, "x");
  DetAJ_lo->SetAxisRange(  -0.3, 0.8, "x");
  

	       
  McAJ->SetLineColor(kGreen+1);
  McAJ->Draw();
  leg->AddEntry( McAJ, "Leading MC truth, no bias", "l");

  McAJMatched->SetLineColor(kBlue);
  McAJMatched->Draw("same");
  leg->AddEntry( McAJMatched, "Leading MC truth, matched", "l");

  DetAJ_hi->SetLineColor(kRed+1);
  DetAJ_hi->Draw("same");
  leg->AddEntry( DetAJ_hi, "Leading Detector level Hard-Core", "l");

  DetAJ_lo->SetLineColor(kGray+2);
  DetAJ_lo->Draw("same");
  leg->AddEntry( DetAJ_lo, "Leading Detector level Soft-Match", "l");

  leg->Draw();
  gPad->SaveAs( "GeantDiagnosis/AJ.png");

  // TObjArray toa;
  TH1D** toa = new TH1D*[6];
  TH1D* h;
  int RebinZg=4;

  // Prep Theory curve
  TF1* PbarQjet = new TF1("PbarQjet","1./[0] * 4./3. * ( (1+pow(x,2))/(1-x) + (1 + pow(1-x,2))/x )", 0.1, 0.5);
  PbarQjet->SetParameter( 0,4.2593);
  PbarQjet->SetLineColor(kGreen);

  TF1* FUVQjet = new TF1("FUVQjet", "[0]*(PbarQjet)", 0.1,0.5);
  FUVQjet->SetParameter( 0,1);

  FUVQjet->SetLineColor(kGray+2);
  FUVQjet->SetLineWidth(1);
  FUVQjet->SetLineStyle(2);
  FUVQjet->SetParameter( 0,1);

  // Draw Lead zg
  // ------------
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  leg = new TLegend( 0.5, 0.55, 0.89, 0.9, "Lead z_{g}, 20<p_{T}^{(Truth | Detector)}<30 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  
  toa[0]=MczgLead2030Lo;
  toa[1]=MczgLead3040Lo;
  toa[2]=MczgLead2030LoMatched;
  toa[3]=MczgLead3040LoMatched;
  toa[4]=DetzgLead2030Lo;
  toa[5]=DetzgLead3040Lo;

  for (int i=0 ; i<6 ; ++i ){
    h=toa[i];

    h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));

    h->Rebin( RebinZg );
    h->Scale(1./RebinZg );

    h->SetAxisRange( 0,10, "y" );
    h->SetLineWidth( 2 );

    h->SetTitle(";z_{g};arb.");
    h->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetXaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold

    // if ( TString(h->GetName()).Contains("AuAu") ){
    //   h->SetMarkerStyle( 21 );
    // } else if ( TString(h->GetName()).Contains("pp") ){
    //   h->SetMarkerStyle( 25 );
    // }

    // if ( TString(h->GetName()).Contains("Hi") ){
    //   h->SetLineColor( kRed );
    //   h->SetMarkerColor( kRed );
    // } else if ( TString(h->GetName()).Contains("Lo") ){
    //   h->SetLineColor( kBlack );
    //   h->SetMarkerColor( kBlack );
    // }
    
  }
	       
  MczgLead2030Lo->SetLineColor(kGreen+1);
  MczgLead2030Lo->Draw();
  leg->AddEntry( MczgLead2030Lo, "Leading MC truth, no bias", "l");

  MczgLead2030LoMatched->SetLineColor(kBlue);
  MczgLead2030LoMatched->Draw("same");
  leg->AddEntry( MczgLead2030LoMatched, "Leading MC truth, matched", "l");

  DetzgLead2030Lo->SetLineColor(kRed+1);
  DetzgLead2030Lo->Draw("same");
  leg->AddEntry( DetzgLead2030Lo, "Leading Detector level Soft-Matched", "l");

  FUVQjet->Draw("same");
  leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  gPad->SaveAs( "GeantDiagnosis/zgLead.png");

  // Draw SubLead zg
  // ------------
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  leg = new TLegend( 0.5, 0.55, 0.89, 0.9, "SubLead z_{g}, 10<p_{T}^{(Truth | Detector)}<20 GeV/c" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  
  toa[0]=MczgSubLead1020Lo;
  toa[1]=MczgSubLead2030Lo;
  toa[2]=MczgSubLead1020LoMatched;
  toa[3]=MczgSubLead2030LoMatched;
  toa[4]=DetzgSubLead1020Lo;
  toa[5]=DetzgSubLead2030Lo;

  for (int i=0 ; i<6 ; ++i ){
    h=toa[i];

    h->Scale ( 1./ h->Integral() / h->GetXaxis()->GetBinWidth(1));

    h->Rebin( RebinZg );
    h->Scale(1./RebinZg );

    h->SetAxisRange( 0,10, "y" );
    h->SetLineWidth( 2 );

    h->SetTitle(";z_{g};arb.");
    h->GetXaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetXaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetTitleFont( 42 ); // 42: helvetica, 62: helvetica bold
    h->GetYaxis()->SetLabelFont( 42 ); // 42: helvetica, 62: helvetica bold    
  }
  h=0;
	       
  MczgSubLead1020Lo->SetLineColor(kGreen+1);
  MczgSubLead1020Lo->Draw();
  leg->AddEntry( MczgSubLead1020Lo, "SubLeading MC truth, no bias", "l");

  MczgSubLead1020LoMatched->SetLineColor(kBlue);
  MczgSubLead1020LoMatched->Draw("same");
  leg->AddEntry( MczgSubLead1020LoMatched, "SubLeading MC truth, matched", "l");

  DetzgSubLead1020Lo->SetLineColor(kRed+1);
  DetzgSubLead1020Lo->Draw("same");
  leg->AddEntry( DetzgSubLead1020Lo, "SubLeading Detector level Soft-Matched", "l");

  FUVQjet->Draw("same");
  leg->AddEntry( FUVQjet, "F_{UV} (quarks)","l");
  leg->Draw();
  gPad->SaveAs( "GeantDiagnosis/zgSubLead.png");


  // Done.
  cout << "Started with " << ppJets->GetEntries() << " GEANT events." << endl;
  cout << "Dropped " << lostmc << " \"new\" events that didn't make the cut in MC." << endl;
  cout << "Dropped " << leadinglost << " events where the leading MC jet was outside |eta|<"<<EtaCut << endl;
  cout << "Total loss =  " << float(lostmc+leadinglost)/ppJets->GetEntries()*100.0 << "%." << endl;
  cout << " -->  Accepted " << acceptedevents << " GEANT events" << endl;
  cout << endl;
  cout << "In those, accepted " << accepteddijets << " dijets." << endl;
  cout << "No dijet in MC acceptance: " << largeetamcdijets << endl;
  cout << "No dijet in MC and GEANT: " << nodijet << endl;
  cout << "Dropped " << lostmcdijet << " events that lost a truth dijet " << endl;
  cout << "Dropped " << newdetdijet << " dijets that didn't correspond to truth " << endl;
  cout << "Dijet loss =  " << (1.0-float(accepteddijets)/float(acceptedevents))*100.0 << "%." << endl;
  cout << "Additionally, " << softmatchlost << " hard-core detector dijets didn't have a soft match " << endl;


  cout << "Dropped " << triggermismatch << " events where the leading MC jet didn't match the detector one or was back-to-back with it" << endl;
  cout << "Total loss =  " << float(lostmc+leadinglost + triggermismatch)/ppJets->GetEntries()*100.0 << "%." << endl;
  
  return 0;
}
