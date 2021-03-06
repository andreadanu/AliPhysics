  ///////////////////////////////////////////////////////////////////////////
  ///
  ///
  ///
  ///
  ///
  ///
  ///
  ///
  ///////////////////////////////////////////////////////////////////////////

AliAnalysisTaskEMCALClusterTurnOn* AddTaskEMCALClusterTurnOn(
                                                                 const char*            periodstr                 = "LHC11c",
                                                                 const char*            ntracks                   = "EmcalTracks",
                                                                 const char*            nclusters                 = "EmcCaloClusters",
                                                                 const UInt_t           pSel                      = AliVEvent::kEMC7,
                                                                 const TString          dType                     = "ESD",
                                                                 const Bool_t	        bHisto                    = kTRUE,
                                                                 const Bool_t           bNLMCut                   = kFALSE,
                                                                 const Int_t            NLMCut                    = 0,
                                                                 const Double_t         minPtCutCluster           = 0.3,
                                                                 const Double_t         TMdeta                    = 0.02,
                                                                 const Double_t         TMdphi                    = 0.03,
                                                                 const Bool_t           bTMClusterRejection       = kTRUE,
                                                                 const Bool_t           bTMClusterRejectionInCone = kTRUE,
                                                                 const Float_t          iIsoConeRadius            = 0.4,
                                                                 const Bool_t           isQA                      = kFALSE,
                                                                 TString                configBasePath            = "",
                                                                 const Int_t            minNLM                    = 1,
                                                                 const Int_t            trig                      = 0
                                                                 )
{

  Printf("Preparing neutral cluster analysis\n");

    // #### Define manager and data container names
  AliAnalysisManager *manager = AliAnalysisManager::GetAnalysisManager();
  if (!manager) {
    ::Error("AddTaskEMCALClusterTurnOn", "No analysis manager to connect to.");
    return NULL;
  }


  printf("Creating container names for cluster analysis\n");
  TString myContName("");
    myContName = Form("Analysis_Neutrals");

  myContName.Append(Form("_Trigger%d_TM_%s_CPVe%.2lf_CPVp%.2lf_IsoConeR%.1f_NLMCut_%s_minNLM%d_maxNLM%d", trig, bTMClusterRejection? "On" :"Off", TMdeta , TMdphi ,iIsoConeRadius,bNLMCut ? "On": "Off",minNLM, NLMCut ));

    // #### Define analysis task
  AliAnalysisTaskEMCALClusterTurnOn* task = new AliAnalysisTaskEMCALClusterTurnOn("Analysis",bHisto);

  TString configFile("config_File.C"); //Name of config file
//  if(gSystem->AccessPathName(configFile.Data())){ //Check for exsisting file and delete it
//    gSystem->Exec(Form("rm %s",configFile.Data()));
//  }

  if(configBasePath.IsNull()){ //Check if a specific config should be used and copy appropriate file
	configBasePath="$ALICE_PHYSICS/PWGGA/EMCALTasks/macros";
	gSystem->Exec(Form("cp %s/%s .",configBasePath.Data(),configFile.Data()));
  }
  else if(configBasePath.Contains("alien:///")){
	gSystem->Exec(Form("alien_cp %s/%s .",configBasePath.Data(),configFile.Data()));
  }
  else{
	gSystem->Exec(Form("cp %s/%s .",configBasePath.Data(),configFile.Data()));
  }

  configBasePath=Form("%s/",gSystem->pwd());
  ifstream configIn; //Open config file for hash calculation
  configIn.open(configFile);
  TString configStr;
  configStr.ReadFile(configIn);
  TString configMD5 = configStr.MD5();
  configMD5.Resize(5); //Short hash value for usable extension
  TString configFileMD5 = configFile;
  TDatime time; //Get timestamp
  Int_t timeStamp = time.GetTime();
  configFileMD5.ReplaceAll(".C",Form("\_%s_%i.C",configMD5.Data(),timeStamp));

  if(gSystem->AccessPathName(configFileMD5.Data())){ //Add additional identifier if file exists
    gSystem->Exec(Form("mv %s %s",configFile.Data(),configFileMD5.Data()));
  }
  else{
      while(!gSystem->AccessPathName(configFileMD5.Data())){
        configFileMD5.ReplaceAll(".C","_1.C");
      }
    gSystem->Exec(Form("mv %s %s",configFile.Data(),configFileMD5.Data()));
  }

  TString configFilePath(configBasePath+"/"+configFileMD5);
  gROOT->LoadMacro(configFilePath.Data());
  printf("Path of config file: %s\n",configFilePath.Data());

    // #### Task preferences
  task->SetIsoConeRadius(iIsoConeRadius);
  task->SetCTMdeltaEta(TMdeta); // after should be replaced by TMdeta
  task->SetCTMdeltaPhi(TMdphi); // after should be replaced by TMdphi
  task->SetQA(isQA);
  task->SetNLMCut(bNLMCut,NLMCut,minNLM);
  task->SetPtBinning(ptBin);
  task->SetPtClBinning(ptClBin);
  task->SetRejectionBinning(RejectionBin);
  task->SetEnergyBinning(EnergyBin);
  task->SetEtaBinning(EtaBin);
  task->SetPhiBinning(PhiBin);
  task->SetEtaClBinning(EtaClBin);
  task->SetPhiClBinning(PhiClBin);
  task->SetNeedEmcalGeom(kTRUE);

  TString name(Form("ClusterTurnOn_%s_%s", ntracks, nclusters));
  cout<<"name of the containers  "<<name.Data()<<endl;

    // tracks to be used for the track matching (already used in TM task, TPC only tracks)
  AliTrackContainer *trackCont  = task->AddTrackContainer("tracks");
  if(!trackCont)
    Printf("Error with TPCOnly!!");
  trackCont->SetName("tpconlyMatch");
  trackCont->SetTrackFilterType(AliEmcalTrackSelection::kTPCOnlyTracks);
    // clusters to be used in the analysis already filtered
  AliClusterContainer *clusterCont = task->AddClusterContainer(nclusters);

    // tracks to be used in the analysis (Hybrid tracks)
  AliTrackContainer * tracksForAnalysis = task->AddTrackContainer("tracks");
  if(!tracksForAnalysis)
    Printf("Error with Hybrids!!");
  tracksForAnalysis->SetName("filterTracksAna");
  tracksForAnalysis->SetFilterHybridTracks(kTRUE);
    tracksForAnalysis->SetTrackCutsPeriod(periodstr);
  tracksForAnalysis->SetDefTrackCutsPeriod(periodstr);
  Printf("Name of Tracks for matching: %s \n Name for Tracks for Isolation: %s",trackCont->GetName(),tracksForAnalysis->GetName());

  printf("Task for neutral cluster analysis created and configured, pass it to AnalysisManager\n");
    // #### Add analysis task
  manager->AddTask(task);


  AliAnalysisDataContainer *contHistos = manager->CreateContainer(myContName.Data(), TList::Class(), AliAnalysisManager::kOutputContainer,Form("%s:NeutralClusters",AliAnalysisManager::GetCommonFileName()));
  AliAnalysisDataContainer *cinput  = manager->GetCommonInputContainer();
  manager->ConnectInput(task, 0, cinput);
  manager->ConnectOutput(task, 1, contHistos);



  return task;
}
