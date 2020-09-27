#ifndef __TNCONTROL_CLASSES__
#define __TNCONTROL_CLASSES__
//#pragma message("Defining __TNCONTROL_CLASSES__")






//==================================================================================
// Defines
//==================================================================================
#ifdef TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)

#else // ! TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllimport)

#endif // ! TNCONTRL_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG






//==================================================================================
// Class Typedefs
//==================================================================================
typedef class CTNBinary					CTNBinary,					* PTNBINARY;
typedef class CTNBinaryLocsList			CTNBinaryLocsList,			* PTNBINARYLOCSLIST;
typedef class CTNBinaryManager			CTNBinaryManager,			* PTNBINARYMANAGER;

typedef class CTNCOMPort				CTNCOMPort,					* PTNCOMPORT;
typedef class CTNCOMPortsList			CTNCOMPortsList,			* PTNCOMPORTSLIST;
typedef class CTNControlLayer			CTNControlLayer,			* PTNCONTROLLAYER;
typedef class CTNCtrlComm				CTNCtrlComm,				* PTNCTRLCOMM;
typedef class CTNCtrlCommTCP			CTNCtrlCommTCP,				* PTNCTRLCOMMTCP;

typedef class CTNExecutor				CTNExecutor,				* PTNEXECUTOR;
typedef class CTNExecutorPriv			CTNExecutorPriv,			* PTNEXECUTORPRIV;

typedef class CTNFaultSim				CTNFaultSim,				* PTNFAULTSIM;
typedef class CTNFaultSimIMTest			CTNFaultSimIMTest,			* PTNFAULTSIMIMTEST;
typedef class CTNFaultSimsList			CTNFaultSimsList,			* PTNFAULTSIMSLIST;

typedef class CTNIPAddress				CTNIPAddress,				* PTNIPADDRESS;
typedef class CTNIPAddressesList		CTNIPAddressesList,			* PTNIPADDRESSESLIST;

#ifndef _XBOX // no IPC supported
typedef class CTNIPCObject				CTNIPCObject,				* PTNIPCOBJECT;
typedef class CTNIPCObjectsList			CTNIPCObjectsList,			* PTNIPCOBJECTSLIST;
#endif // ! XBOX

typedef class CTNJob					CTNJob,						* PTNJOB;
typedef class CTNJobQueue				CTNJobQueue,				* PTNJOBQUEUE;

#ifndef _XBOX // no IPC supported
typedef class CTNLeech					CTNLeech,					* PTNLEECH;
typedef class CTNLeechesList			CTNLeechesList,				* PTNLEECHESLIST;
#endif // ! XBOX

typedef class CTNMachineInfo			CTNMachineInfo,				* PTNMACHINEINFO;
#ifndef _XBOX // no master supported
typedef class CTNMaster					CTNMaster,					* PTNMASTER;
typedef class CTNMasterInfo				CTNMasterInfo,				* PTNMASTERINFO;
typedef class CTNMastersList			CTNMastersList,				* PTNMASTERSLIST;
typedef class CTNMetaMaster				CTNMetaMaster,				* PTNMETAMASTER;
#endif // ! XBOX

typedef class CTNOtherMachineInfo		CTNOtherMachineInfo,		* PTNOTHERMACHINEINFO;
typedef class CTNOtherMachineInfosList	CTNOtherMachineInfosList,	* PTNOTHERMACHINEINFOSLIST;
typedef class CTNOutputVar				CTNOutputVar,				* PTNOUTPUTVAR;
typedef class CTNOutputVarsList			CTNOutputVarsList,			* PTNOUTPUTVARSLIST;

typedef class CTNPeriodicSend			CTNPeriodicSend,			* PTNPERIODICSEND;
typedef class CTNPeriodicSendsList		CTNPeriodicSendsList,		* PTNPERIODICSENDSLIST;

typedef class CTNReachCheck				CTNReachCheck,				* PTNREACHCHECK;
typedef class CTNReachChecksList		CTNReachChecksList,			* PTNREACHCHECKSLIST;
typedef class CTNReachCheckTarget		CTNReachCheckTarget,		* PTNREACHCHECKTARGET;
typedef class CTNReachCheckTargetsList	CTNReachCheckTargetsList,	* PTNREACHCHECKTARGETSLIST;

#ifndef _XBOX // reports not supported
typedef class CTNReport					CTNReport,					* PTNREPORT;
typedef class CTNReportsList			CTNReportsList,				* PTNREPORTSLIST;
#endif // ! XBOX

typedef class CTNResult					CTNResult,					* PTNRESULT;
typedef class CTNResultsChain			CTNResultsChain,			* PTNRESULTSCHAIN;

typedef class CTNSendData				CTNSendData,				* PTNSENDDATA;
typedef class CTNSendDataQueue			CTNSendDataQueue,			* PTNSENDDATAQUEUE;
typedef class CTNSlave					CTNSlave,					* PTNSLAVE;
typedef class CTNSlaveInfo				CTNSlaveInfo,				* PTNSLAVEINFO;
typedef class CTNSlavesList				CTNSlavesList,				* PTNSLAVESLIST;
typedef class CTNSoundCard				CTNSoundCard,				* PTNSOUNDCARD;
typedef class CTNSoundCardsList			CTNSoundCardsList,			* PTNSOUNDCARDSLIST;
typedef class CTNStoredData				CTNStoredData,				* PTNSTOREDDATA;
typedef class CTNStoredDataList			CTNStoredDataList,			* PTNSTOREDDATALIST;
typedef class CTNSyncData				CTNSyncData,				* PTNSYNCDATA;
typedef class CTNSyncDataList			CTNSyncDataList,			* PTNSYNCDATALIST;
typedef class CTNSystemResult			CTNSystemResult,			* PTNSYSTEMRESULT;

typedef class CTNTAPIDevice				CTNTAPIDevice,				* PTNTAPIDEVICE;
typedef class CTNTAPIDevicesList		CTNTAPIDevicesList,			* PTNTAPIDEVICESLIST;
typedef class CTNTestFromFile			CTNTestFromFile,			* PTNTESTFROMFILE;
typedef class CTNTestFromFilesList		CTNTestFromFilesList,		* PTNTESTFROMFILESLIST;
typedef class CTNTestInstanceM			CTNTestInstanceM,			* PTNTESTINSTANCEM;
typedef class CTNTestInstanceS			CTNTestInstanceS,			* PTNTESTINSTANCES;
typedef class CTNTestMsList				CTNTestMsList,				* PTNTESTMSLIST;
typedef class CTNTestSsList				CTNTestSsList,				* PTNTESTSSLIST;
typedef class CTNTestResult				CTNTestResult,				* PTNTESTRESULT;
typedef class CTNTestStats				CTNTestStats,				* PTNTESTSTATS;
typedef class CTNTestTableItem			CTNTestTableItem,			* PTNTESTTABLEITEM;
typedef class CTNTestTableCase			CTNTestTableCase,			* PTNTESTTABLECASE;
typedef class CTNTestTableGroup			CTNTestTableGroup,			* PTNTESTTABLEGROUP;
typedef class CTNTrackedBinaries		CTNTrackedBinaries,			* PTNTRACKEDBINARIES;





#else //__TNCONTROL_CLASSES__
//#pragma message("__TNCONTROL_CLASSES__ already included!")
#endif //__TNCONTROL_CLASSES__
