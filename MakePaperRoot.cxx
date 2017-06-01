{
  TFile *fdpt2 = TFile::Open("R0.2_dpt.root");
  TFile *fdpt4 = TFile::Open("R0.4_dpt.root");
  TFile *fRes2 = TFile::Open("R0.2_Res.root");
  TFile *fRes4 = TFile::Open("R0.4_Res.root");

  TFile *fout = new TFile("AjSupplement.root","RECREATE");
  fRes4->Get("Trig")->Write("Trig4");
  fRes4->Get("TrigMatched")->Write("TrigMatched4");

  fRes2->Get("Trig")->Write("Trig2");
  fRes2->Get("TrigMatched")->Write("TrigMatched2");

  
  fRes4->Get("Recoil")->Write("Recoil4");
  fRes4->Get("RecoilMatched")->Write("RecoilMatched4");
  fRes2->Get("Recoil")->Write("Recoil2");
  fRes2->Get("RecoilMatched")->Write("RecoilMatched2");

  fdpt4->Get("DeltaPt")->Write("DeltaPt4");
  fdpt4->Get("DeltaPtMatched")->Write("DeltaPtMatched4");
    
  fdpt2->Get("DeltaPt")->Write("DeltaPt2");
  fdpt2->Get("DeltaPtMatched")->Write("DeltaPtMatched2");
}
