/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include <TMath.h>
#include <TH1F.h>
#include <TFile.h>
#include "AliVertexingHFUtils.h"
#include "AliEventPlaneResolutionHandler.h"
#include "AliLog.h"

/* $Id$ */

///////////////////////////////////////////////////////////////////
//                                                               //
// Class with functions useful for different D2H analyses        //
// - event plane resolution                                      //
// - <pt> calculation with side band subtraction                 //
// - tracklet multiplicity calculation                            //
// Origin: F.Prino, Torino, prino@to.infn.it                     //
//                                                               //
///////////////////////////////////////////////////////////////////

ClassImp(AliEventPlaneResolutionHandler)

//______________________________________________________________________
AliEventPlaneResolutionHandler::AliEventPlaneResolutionHandler():TObject(),
  fEventPlane(kTPCPosEta),
  fResolOption(kTwoEtaSub),
  fMinCent(30.),
  fMaxCent(50.),
  fCorrHistoName1(""),
  fCorrHistoName2(""),
  fCorrHistoName3(""),
  fNsubevents(2),
  fExtrapToFull(kFALSE),
  fWeight(0),
  fHistoAB(0x0),
  fHistoBC(0x0),
  fHistoAC(0x0),
  fRootFileName("$ALICE_PHYSICS/../src/PWGHF/vertexingHF/charmFlow/EventPlaneResolutionHistos.root")
{
  // Default contructor
  InitializeWeights();
}
//______________________________________________________________________
AliEventPlaneResolutionHandler::AliEventPlaneResolutionHandler(TString filename):TObject(),
  fEventPlane(kTPCPosEta),
  fResolOption(kTwoEtaSub),
  fMinCent(30.),
  fMaxCent(50.),
  fCorrHistoName1(""),
  fCorrHistoName2(""),
  fCorrHistoName3(""),
  fNsubevents(2),
  fExtrapToFull(kFALSE),
  fWeight(0),
  fHistoAB(0x0),
  fHistoBC(0x0),
  fHistoAC(0x0),
  fRootFileName(filename.Data())
{
  // Standard contructor
  InitializeWeights();
}
//______________________________________________________________________
Double_t AliEventPlaneResolutionHandler::GetEventPlaneResolution(Double_t minCent, Double_t maxCent){
  // method to compute the event plane resolution starting from correlations

  TDirectory *current = gDirectory;

  TFile* inputFile=new TFile(fRootFileName.Data());
  if(!inputFile || (inputFile && !inputFile->IsOpen())){
    printf("Root file not opened --- Return dummy resolution value\n");
    return -1.;
  }
  Bool_t isOk=SetHistoNames();
  if(!isOk){
    printf("Return dummy resolution value\n");
    return -1.;
  }
  
  Int_t minCentrTimesTen=(Int_t)(minCent*10);
  Int_t maxCentrTimesTen=(Int_t)(maxCent*10);
  Int_t ncBin=minCentrTimesTen/25;
  Int_t whichWei=fWeight;
  if(minCent>50.5 || maxCent>50.5) fWeight=0;
  Double_t weight=-1.;  
  if(fWeight==1) weight=fNcoll[ncBin];
  // protection: weights from yield computed for 30-50, 10-30 and 0-10 centralities
  // cannot be used on wider ranges
  else if(fWeight==2 && minCent>29.5 && maxCent<50.5)  weight=fYield24[ncBin]; 
  else if(fWeight==3 && minCent>29.5 && maxCent<50.5)  weight=fYield46[ncBin];
  else if(fWeight==4 && minCent>29.5 && maxCent<50.5)  weight=fYield612[ncBin];
  else if(fWeight==2 && minCent>9.5 && maxCent<30.5)  weight=fYield24[ncBin];
  else if(fWeight==3 && minCent>9.5 && maxCent<30.5)  weight=fYield46[ncBin];
  else if(fWeight==4 && minCent>9.5 && maxCent<30.5)  weight=fYield612[ncBin];
  else if(fWeight==2 && maxCent<10.5)  weight=fYield24[ncBin];
  else if(fWeight==3 && maxCent<10.5)  weight=fYield46[ncBin];
  else if(fWeight==4 && maxCent<10.5)  weight=fYield612[ncBin];
  TH1F* hevpls2=0x0;
  TH1F* hevpls3=0x0;
  TH1F* hevpls1=(TH1F*)inputFile->Get(Form("%sCentr%d_%d",fCorrHistoName1.Data(),minCentrTimesTen,minCentrTimesTen+25));
  hevpls1->SetName(Form("%sCentr%d_%d",fCorrHistoName1.Data(),minCentrTimesTen,maxCentrTimesTen));
  if(weight>0.) hevpls1->Scale(weight);
  if(fNsubevents==3){
    hevpls2=(TH1F*)inputFile->Get(Form("%sCentr%d_%d",fCorrHistoName2.Data(),minCentrTimesTen,minCentrTimesTen+25));
    hevpls2->SetName(Form("%sCentr%d_%d",fCorrHistoName2.Data(),minCentrTimesTen,maxCentrTimesTen));
    hevpls3=(TH1F*)inputFile->Get(Form("%sCentr%d_%d",fCorrHistoName3.Data(),minCentrTimesTen,minCentrTimesTen+25));
    hevpls3->SetName(Form("%sCentr%d_%d",fCorrHistoName2.Data(),minCentrTimesTen,maxCentrTimesTen));
    if(weight>0.){
      hevpls2->Scale(weight);
      hevpls3->Scale(weight);
    }
  }
  for(Int_t iCentr=minCentrTimesTen+25; iCentr<maxCentrTimesTen; iCentr+=25){
    ncBin=iCentr/25;
    weight=-1.;  
    if(fWeight==1) weight=fNcoll[ncBin];
    else if(fWeight==2 && minCent>29.5 && maxCent<50.5)  weight=fYield24[ncBin];
    else if(fWeight==3 && minCent>29.5 && maxCent<50.5)  weight=fYield46[ncBin];
    else if(fWeight==4 && minCent>29.5 && maxCent<50.5)  weight=fYield612[ncBin];
    else if(fWeight==2 && minCent>9.5 && maxCent<30.5)  weight=fYield24[ncBin];
    else if(fWeight==3 && minCent>9.5 && maxCent<30.5)  weight=fYield46[ncBin];
    else if(fWeight==4 && minCent>9.5 && maxCent<30.5)  weight=fYield612[ncBin];
    else if(fWeight==2 && maxCent<10.5)  weight=fYield24[ncBin];
    else if(fWeight==3 && maxCent<10.5)  weight=fYield46[ncBin];
    else if(fWeight==4 && maxCent<10.5)  weight=fYield612[ncBin];
    TH1F* htmp1=(TH1F*)inputFile->Get(Form("%sCentr%d_%d",fCorrHistoName1.Data(),iCentr,iCentr+25));
    if(weight>0.) htmp1->Scale(weight);
    hevpls1->Add(htmp1);
    if(fNsubevents==3){
      TH1F* htmp2=(TH1F*)inputFile->Get(Form("%sCentr%d_%d",fCorrHistoName2.Data(),iCentr,iCentr+25));
      TH1F* htmp3=(TH1F*)inputFile->Get(Form("%sCentr%d_%d",fCorrHistoName3.Data(),iCentr,iCentr+25));
      if(weight>0.){
	htmp2->Scale(weight);
	htmp3->Scale(weight);
      }
      hevpls2->Add(htmp2);
      hevpls3->Add(htmp3);
    }
  }
  fWeight=whichWei;
  Double_t resol=1.;
  if(fNsubevents==2){
    if(fExtrapToFull) resol=AliVertexingHFUtils::GetFullEvResol(hevpls1);
    else resol=AliVertexingHFUtils::GetSubEvResol(hevpls1);
    current->cd();
    if(fHistoAB) delete fHistoAB;
    fHistoAB=(TH1F*)hevpls1->Clone("hCorrelSubAB");
  }else if(fNsubevents==3){
    Double_t correl1=hevpls1->GetMean();
    Double_t correl2=hevpls2->GetMean();
    Double_t correl3=hevpls3->GetMean();
    resol=TMath::Sqrt(correl1*correl2/correl3);
    current->cd();
    if(fHistoAB) delete fHistoAB;
    fHistoAB=(TH1F*)hevpls1->Clone("hCorrelSubAB");
    if(fHistoBC) delete fHistoBC;
    fHistoBC=(TH1F*)hevpls2->Clone("hCorrelSubBC");
    if(fHistoAC) delete fHistoAC;
    fHistoAC=(TH1F*)hevpls3->Clone("hCorrelSubAC");
  }
  inputFile->Close();
  return resol;
}
//______________________________________________________________________
Bool_t AliEventPlaneResolutionHandler::SetHistoNames(){
  // set names for histograms to be read
  if(fEventPlane==kTPCPosEta && fResolOption==kTwoRandSub){
    fCorrHistoName1="hCorrelRandSubTPCPosEta";
    fNsubevents=2;
    fExtrapToFull=kTRUE;
  }else if(fEventPlane==kTPCPosEta && fResolOption==kTwoChargeSub){
    fCorrHistoName1="hCorrelChargeSubTPCPosEta";
    fNsubevents=2;
    fExtrapToFull=kTRUE;
  }else if(fEventPlane==kTPCPosEta && fResolOption==kTwoEtaSub){
    fCorrHistoName1="hCorrelTPCEtaPosTPCEtaNeg";
    fNsubevents=2;
    fExtrapToFull=kFALSE;
  }else if(fEventPlane==kTPCPosEta && fResolOption==kThreeSub){
    fCorrHistoName1="hCorrelTPCPosEtaV0A";
    fCorrHistoName2="hCorrelTPCPosEtaV0C";
    fCorrHistoName3="hCorrelV0AV0C";
    fNsubevents=3;
    fExtrapToFull=kFALSE;
  }else if(fEventPlane==kTPCFullEta && fResolOption==kTwoRandSub){
    fCorrHistoName1="hCorrelRandSubTPCFullEta";
    fNsubevents=2;
    fExtrapToFull=kTRUE;
  }else if(fEventPlane==kTPCFullEta && fResolOption==kTwoEtaSub){
    fCorrHistoName1="hCorrelTPCEtaPosTPCEtaNeg";
    fNsubevents=2;
    fExtrapToFull=kTRUE;
  }else if(fEventPlane==kTPCFullEta && fResolOption==kThreeSub){
    fCorrHistoName1="hCorrelTPCFullEtaV0A";
    fCorrHistoName2="hCorrelTPCFullEtaV0C";
    fCorrHistoName3="hCorrelV0AV0C";
    fNsubevents=3;
    fExtrapToFull=kFALSE;
  }else if(fEventPlane==kVZERO && fResolOption==kThreeSub){
    fCorrHistoName1="hCorrelTPCEtaPosV0";
    fCorrHistoName2="hCorrelTPCEtaNegV0";
    fCorrHistoName3="hCorrelTPCEtaPosTPCEtaNeg";
    fNsubevents=3;
    fExtrapToFull=kFALSE;
  }else if(fEventPlane==kVZERO && fResolOption==kThreeSubTPCGap){
    fCorrHistoName1="hCorrelTPCEtaGt02V0";
    fCorrHistoName2="hCorrelTPCEtaLt02V0";
    fCorrHistoName3="hCorrelTPCEtaGt02TPCEtaLt02";
    fNsubevents=3;
    fExtrapToFull=kFALSE;
  }else if(fEventPlane==kVZEROA && fResolOption==kThreeSub){
    fCorrHistoName1="hCorrelTPCFullEtaV0A";
    fCorrHistoName2="hCorrelV0AV0C";
    fCorrHistoName3="hCorrelTPCFullEtaV0C";
    fNsubevents=3;
    fExtrapToFull=kFALSE;
  }else if(fEventPlane==kVZEROC && fResolOption==kThreeSub){
    fCorrHistoName1="hCorrelTPCFullEtaV0C";
    fCorrHistoName2="hCorrelV0AV0C";
    fCorrHistoName3="hCorrelTPCFullEtaV0A";
    fNsubevents=3;
    fExtrapToFull=kFALSE;
  }else{
    printf("Not possible to compute the resolution with the required option\n");
    return kFALSE;
  }
  return kTRUE;
}
//______________________________________________________________________
void AliEventPlaneResolutionHandler::InitializeWeights(){
  // Ncoll values in 2.5% wide classes
  Double_t ncoll[20]={1790.77,1578.44,1394.82,1236.17,1095.08,
		      969.86,859.571,759.959,669.648,589.588,
		      516.039,451.409,392.853,340.493,294.426,
		      252.385,215.484,183.284,155.101,130.963};

  Double_t y24[20]={544.155,491.803,533.516,449.618,
		    213.097,179.483,194.977,156.73,118.84,152.306,92.7,122.538,
		    36.64,33.01,27.79,22.57,17.90,15.25,15.03,8.57};

  Double_t y46[20]={158.386,129.294,135.683,112.203,
		    59.425,61.506,65.779,43.521,35.530,41.014,26.744,33.593,
		    12.24,12.32,10.44,8.55,7.20,5.85,5.80,4.37};

  Double_t y612[20]={76.010,85.944,99.105,84.779,
		     45.404,25.059,36.734,31.177,31.587,46.088,19.367,39.945,
		     7.31,7.55,6.47,5.38,4.68,4.86,3.10,4.78};

  for(Int_t i=0; i<20; i++){
    fNcoll[i]=ncoll[i];
    fYield24[i]=y24[i];
    fYield46[i]=y46[i];
    fYield612[i]=y612[i];
  }
}
