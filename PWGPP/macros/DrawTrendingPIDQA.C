#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TTree.h>
#include <TH1.h>
#include <TFile.h>
#include <TObjString.h>
#include <TCanvas.h>
#endif

TH1F* CreateHisto(TString nam, Int_t tote);

Bool_t DrawTrendingPIDQA(TString mergedTrendFile = "trending.root"){
  TFile *fin = TFile::Open(mergedTrendFile.Data());
  if(!fin){
    printf("Cannot open file with PID QA trending: %s\n",mergedTrendFile.Data());
    return kFALSE;
  }
  TTree * ttree = (TTree*) fin->Get("trending");
  if (!ttree){
    printf("Trending tree not found in file %s\n",mergedTrendFile.Data());
    return kFALSE;
  }
  Int_t nrun;
  ttree->SetBranchAddress("nrun",&nrun);
  TObjArray* lb=(TObjArray*)ttree->GetListOfBranches();
  Int_t nVars=lb->GetEntries();
  Float_t* vect=new Float_t[nVars-1];
  TH1F** htr=new TH1F*[nVars-1];
  Int_t kv=0;
  Int_t totEnt=ttree->GetEntries();
  for(Int_t j=0; j<nVars; j++){
    TBranch* br=(TBranch*)lb->At(j);
    printf("Branch %d  %s\n",j,br->GetName());
    TString bnam=br->GetName();
    if(!bnam.Contains("nrun")){
      ttree->SetBranchAddress(bnam,&vect[kv]);
      htr[kv]=CreateHisto(bnam,totEnt);
      ++kv;
    }
  }
  for(Int_t je=0; je<totEnt; je++){
    ttree->GetEvent(je);
    printf(" Run %d\n",nrun);
    for(Int_t k=0; k<nVars-1; k++){
      htr[k]->SetBinContent(je+1,vect[k]);
      htr[k]->GetXaxis()->SetBinLabel(je+1,Form("%d",nrun));
    }
  }


  TCanvas** c=new TCanvas*[20];
  TString detStrings[4]={"ITS","TPC_Basic","TPC_TOF","TOF"};
  TString partStrings[5]={"electron","pion","kaon","proton","deuteron"};
  TString canname[20];
  for(Int_t k=0; k<20; k++){ 
    c[k]=new TCanvas(Form("c%d",k),Form("c%d",k),1500,600);
    c[k]->Divide(2,2);
    canname[k]="PIDqaTrend";
  }
  for(Int_t jdet=0; jdet<4; jdet++){
    for(Int_t jpar=0; jpar<5; jpar++){
      Int_t k=jdet*5+jpar;
      canname[k].Append(detStrings[jdet].Data());
      canname[k].Append(partStrings[jpar].Data());
    }
  }

  for(Int_t jvar=0; jvar<nVars-1; jvar++){
    TString hname=htr[jvar]->GetName();
    Int_t theDet=-1;
    for(Int_t jdet=0; jdet<4; jdet++){
      if(hname.Contains(Form("nSigma%s",detStrings[jdet].Data()))) theDet=jdet;
    }
    Int_t thePar=-1;
    for(Int_t jpar=0; jpar<5; jpar++){     
      if(hname.Contains(partStrings[jpar].Data())) thePar=jpar;
    }
    if(theDet>=0 && thePar>=0){
      Int_t theCanv=theDet*5+thePar;
      Int_t thePad=-1;
      if(hname.Contains("meannSigma")) thePad=1;
      else if(hname.Contains("signSigma")) thePad=2;
      if(thePad>0){
	TObjArray* arr=hname.Tokenize("_");
	TObjString* lasts=(TObjString*)arr->At(arr->GetEntries()-1);
	TString stmom=lasts->GetString();
	if(stmom.Contains("MeV")){
	  stmom.ReplaceAll("p","");
	  stmom.ReplaceAll("MeV","");
	  Int_t mom=stmom.Atoi();
	  if(theDet<=2 && mom>800) thePad+=2;
	  if(theDet==3 && mom>1100) thePad+=2;
	  c[theCanv]->cd(thePad);
	  htr[jvar]->Draw("");
	  htr[jvar]->Draw("psame");
	}
      }
    }
  }
  for(Int_t k=0; k<20; k++) c[k]->SaveAs(Form("%s.png",canname[k].Data()));
  delete [] vect;

}


TH1F* CreateHisto(TString nam, Int_t tote){
  TH1F* h=new TH1F(nam.Data(),Form(" ; run ; %s",nam.Data()),tote,0.5,tote+0.5);
  h->SetStats(0);
  h->SetMarkerStyle(20);
  return h;
}
