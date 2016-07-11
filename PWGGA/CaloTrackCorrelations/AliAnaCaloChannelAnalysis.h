#ifndef ALIANACALOCHANNELANALYSIS_H
#define ALIANACALOCHANNELANALYSIS_H

/// \class AliAnaCaloChannelAnalysis
/// \brief Analyses cell properties and identifies bad cells
///
/// This is used for bad channel identification in EMCal and DCal.
/// The class builds a mean distribution of certain cell observables
/// and compares single cell properties to this mean.
/// That way bad channels (far off the mean) are identified and flagged.
/// A .pdf file with their spectra is created. This should be
/// cross checked by hand.
///
/// \author Eliane Epple <eliane.epple@yale.edu>, Yale Univeristy
/// \author Chiara Bianchin <chiara.bianchin@cern.ch>, Wein University
/// based on the work from
/// \author Alexis Mas <aleximas@if.usp.br> & M. Germain <Marie.Germain@subatech.in2p3.fr>, SUBATECH
/// which is in turn based on getCellsRunQA.C from
/// \author Olga Driga, SUBATECH
/// \date Jun 24, 2016

/* Copyright(c) 1998-2016, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#if !defined(__CINT__) || defined(__MAKECINT__) 
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TROOT.h>
#include <TArrayD.h>
#include <Riostream.h>
#include <AliCalorimeterUtils.h>
#endif
using namespace std;
using std::vector;
using std::array;



class AliAnaCaloChannelAnalysis : public TObject {

public:

    AliAnaCaloChannelAnalysis() ;                // default ctor
	  virtual ~AliAnaCaloChannelAnalysis()  { ; }  // virtual dtor
	  AliAnaCaloChannelAnalysis(TString period, TString pass, TString trigger, Int_t runNumber);

	  void Run();

    //Setters
	  void SetExternalMergedFile(TString inputName)     {fExternalFileName = inputName;}
	  void SetInputFileList(TString inputName)          {fRunListFileName  = inputName;}
	  void SetWorkDir(TString inputName)                {fWorkdir          = inputName;}
	  void SetNTrial(Int_t inputNr)                     {fTrial            = inputNr  ;}
  
	  void AddPeriodAnalysis(Int_t criteria, Double_t Nsigma, Double_t Emin, Double_t Emax);


protected:

	  void Init();
	  void Draw2(Int_t cell, Int_t cellref=400);
	  void SaveBadCellsToPDF(Int_t cell[], Int_t iBC, Int_t nBC, TString pdfName, Int_t cellref=2377);
	  void Process(Int_t *pflag[23040][7], TH1* inhisto, Double_t nsigma = 4., Int_t dnbins = 200, Double_t dmaxval = -1.);
	  void TestCellEandN(Int_t *pflag[23040][7], Double_t Emin = 0.1, Double_t emax=2., Double_t nsigma = 4.);
	  void TestCellShapes(Int_t *pflag[23040][7], Double_t fitEmin, Double_t fitEmax, Double_t nsigma =4.);
	  void ExcludeCells(Int_t *pexclu[23040], Int_t nrCells);
	  void KillCells(Int_t filter[], Int_t nbc);
	  void PeriodAnalysis(Int_t criterum=7, Double_t nsigma = 4.0, Double_t emin=0.1, Double_t emax=2.0);
	  void BCAnalysis();
	  TString Convert();


	  //Settings for analysed period
	  Int_t   fCurrentRunNumber;            ///< A run number of an analyzed period. This is important for the AliCalorimeterUtils initialization
    TString fPeriod;                      ///< The name of the analyzed period
	  TString fPass;                        ///< Pass of the analyzed data
	  TString fTrigger;                     ///< Selected trigger for the analysis
	  Int_t   fNoOfCells;                   ///< Number of cells in EMCal and DCal

	  //Genergal paths
	  TString fMergeOutput;                 ///< Here the merged files of a period are saved for a later analysis
	  TString fAnalysisOutput;              ///< The list with bad channels and histograms are saved in this folder
	  TString fAnalysisInput;               ///< Here the .root files of each run of the period are saved
	  TString fRunList;                     ///< Thats the full path and name of the file which contains a list of all runs to be merged together

	  //
	  TString fQADirect;                    ///< Dierctory in the QA.root files where the input histograms are stored
	  TString fMergedFileName;              ///< Filename of the .root file containing the merged runs
	  TString fFilteredFileName;            ///< Filename of the .root file containing histograms with removed cells
	  std::vector<TArrayD> fAnalysisVector; ///< Vector of analysis information. Each place is filled with 4 doubles: version, sigma, lower, and upper energy range


	  //Things to be individualized by setters
	  TString fRunListFileName;             ///< This is the name of the file with the run numbers to be merged, by default it's 'runList.txt'
	  TString fWorkdir;                     ///< Directory which contains the folders fMergeOutput, fAnalysisInput and fAnalysisOutput. By default it is './'
	  Int_t   fTrial;                       ///< Number of trial that this specific analyis is. By default '0'
	  TString fExternalFileName;            ///< If you have already a file that contains many runs merged together you can place it in fMergeOutput and set it with SetExternalMergedFile(FileName)

	  //arrays to store information
	  Int_t *fnewBC;                        //!<! starts at newBC[0] stores cellIDs  (cellID = bin-1)
	  Int_t *fnewDC;                        //!<! starts at newDC[0] stores cellIDs  (cellID = bin-1)
	  Int_t *fpexclu;                       //!<! starts at 0 pexclu[CellID] stores 0 not excluded, 1 excluded
	  Int_t *fexclu;                        //!<! is the same as above
	  Int_t **fpflag[7];                    //!<! pflag[cellID][crit] = 1(ok),2(bad),0(bad)     start at 0 (cellID 0 = histobin 1)
	  Int_t **fflag[7];                     //!<! is the same as above

	  AliCalorimeterUtils* fCaloUtils;      //!<! Calorimeter information for the investigated runs
  
private:
	  AliAnaCaloChannelAnalysis           (const AliAnaCaloChannelAnalysis&); // not implemented
	  AliAnaCaloChannelAnalysis &operator=(const AliAnaCaloChannelAnalysis&); // not implemented

  /// \cond CLASSIMP
  ClassDef(AliAnaCaloChannelAnalysis, 1);
  /// \endcond
};
#endif