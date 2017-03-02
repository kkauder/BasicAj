{
  TFile * f = new TFile ("test.root");
  c->Draw();
  TASImage *img = TASImage::Open("STAR-logo-base-black.png");
  TPad* p = new TPad("p","",0.2,0.4, 0.4,0.6 );
  p->Draw();
  p->cd();
  img->Draw();
  c->cd();
  gPad->SaveAs("delme.eps");
    

}
