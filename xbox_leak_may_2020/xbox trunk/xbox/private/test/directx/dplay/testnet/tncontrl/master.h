#ifndef _XBOX // no master supported
#ifndef __TESTNET_MASTER__
#define __TESTNET_MASTER__
//#pragma message("Defining __TESTNET_MASTER__")






//==================================================================================
// Common Defines
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
// Defines
//==================================================================================
// How long to wait for the PeriodicCheck thread to terminate.
#define TIMEOUT_DIE_PERIODICCHECKTHREAD		10000 // in milliseconds

// How often to traverse the list, looking for slaves we haven't heard from in a
// while, in milliseconds.
#define SLAVE_CHECK_INTERVAL				4000

// These pings will occur every SLAVE_CHECK_INTERVAL ms.  If the transport layer/
// control method is reliable, then there should be no need to send multiple pings.
// In fact, the only reason we ping at all is because the control method's timeout
// period may be longer than we want/non-existent.
#define NUM_PINGS_TO_ATTEMPT				1 

// How long to go without hearing from a slave before trying to ping him, in ticks.
#define TIMEOUT_SLAVE_PING					15000

// How long to go without hearing from a slave before killing him, in ticks.
#define TIMEOUT_SLAVE_KILL					(TIMEOUT_SLAVE_PING + 20000)



//==================================================================================
// Internal Prototypes
//==================================================================================
DWORD WINAPI MasterJobThread(LPVOID lpvParameter);
HRESULT MasterReadFileCB(char* szLine, PFILELINEINFO pInfo, PVOID pvContext,
						BOOL* pfStopReading);



//==================================================================================
// Internal structures
//==================================================================================
typedef struct tagREADSCRIPTFILECONTEXT
{
	PTNMASTER			pMaster; // pointer to master object
	DWORD				dwNumReps; // number of repetitions for tests found in the file
	PLSTRINGLIST		pStartupStrings; // pointer to list currently holding module startup strings
	DWORD				dwUniqueID; // unique ID of current test
	PTNTESTTABLECASE	pCase; // pointer to case of current test
	char*				pszInstanceID; // pointer to string with current test instance ID
	char*				pszFriendlyName; // pointer to optional friendly name for current test
	BOOL				fNumMachinesSpecified; // whether the number of machines has been specified or not
	int					iNumMachines; // total number of testers running the instance
	DWORD				dwPermutationMode; // permutation mode to run this test in
	//DWORD				dwOptions; // pointer to options for current test
	PTNSTOREDDATA		pStoredData; // pointer to object holding stored data for test
	PLSTRINGLIST		pTesterAssignments; // pointer to tester assignments for test
	int					iTesterNum; // current tester number
} READSCRIPTFILECONTEXT, * PREADSCRIPTFILECONTEXT;




//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNMaster:public CTNControlLayer
{
	// This is a friend so it can access the protected members
	friend DWORD WINAPI MasterJobThread(LPVOID lpvParameter);
	friend HRESULT MasterReadFileCB(char* szLine, PFILELINEINFO pInfo,
									PVOID pvContext, BOOL* pfStopReading);


	private:
		DWORD						m_dwMode; // what mode this object is in (TNMODE_xxx)
		TNMODULEID					m_moduleID; // ID of module using us
		char*						m_pszSessionFilter; // user specified session filter string
		char*						m_pszClosingReportPath; // path to file to dump report of all tests on close
		char*						m_pszSiblingFailuresMemDumpDirPath; // directory to have siblings of slaves reporting failures dump their memlog
		char*						m_pszSkippedTestsPath; // path to file to use when printing list of all the tests skipped
		CTNTestTableGroup			m_testtable; // table of tests generated from the module
		CTNTestStats				m_totalstats; // statistics maintained on all tests
		PLSTRINGLIST				m_pFailureBreakRules; // pointer to list of cases to DebugBreak() on

		PTNGETSTARTUPDATAPROC		m_pfnGetStartupData; // callback to retrieve startup data for new joiners
		PTNLOADSTARTUPDATAPROC		m_pfnLoadStartupData; // callback to load startup data from strings
		PTNWRITESTARTUPDATAPROC		m_pfnWriteStartupData; // callback to write startup data to strings
		PTNHANDLEUSERQUERYPROC		m_pfnHandleUserQuery; // callback to use when a UserQuery is received

		DWORD						m_dwSessionID; // ID of current session
		CHECKSUM					m_testChecksum; // checksum generated for tests
		//HANDLE					m_hAllSlavesCommStatusEvent; // handle to event ctrl comm fires when a slaves status changes
		DWORD						m_dwCurrentUniqueID; // incremented counter used to identify instances of given tests
		DWORD						m_dwStartupTicks; // ticks when session started
		time_t						m_tStartupTime; // clock time when session started
		DWORD						m_dwTimelimit; // how long to let slaves run, in ticks
		int							m_iSlaveNumLimit; // total number of slaves that will be running tests
		DWORD						m_dwAllowJoinersTime; // how long after the first joiner to allow additional joiners, in minutes
		BOOL						m_fJoinersAllowed; // whether new slaves are allowed to join
		BOOL						m_fTestingComplete; // is all testing finished?
		CTNTestFromFilesList		m_loadedtests; // list of all the tests loaded from the file
		CTNTestMsList				m_remainingtests; // remaining tests list object
		CTNTestMsList				m_skippedtests; // list of tests that had to be skipped
		CTNIPAddressesList			m_ipaddrs; // ip addresses this machine is using and/or ones slaves see
		PVOID						m_pvMetaMasterAddress; // pointer to buffer with meta-master's address
		DWORD						m_dwMetaMasterAddressSize; // size of meta-master's address
		TNCTRLMACHINEID				m_id; // ID given to us by meta-master
		BOOL						m_fRegistered; // whether we're registered with the meta-master or not
		BOOL						m_fPingSlaves; // whether to ping slaves or not
		HANDLE						m_hJobThread; // worker thread handle
		CTNSlavesList				m_slaves; // machines attached to the current session
		CTNSlavesList				m_droppedslaves; // machines no longer in the current session
		int							m_iTotalNumSlaves; // total number of machines that have ever been in the session
		CTNReportsList				m_reports; // test log object
		CTNOutputVarsList			m_builtinvars; // built in dynamic variables
		CTNTestMsList				m_testswithvars; // list of tests with output data and output variables
		BOOL						m_fReportInBVTFormat; // whether to print report files in BVT format or not

		DWORD						m_dwAutoReportHeaders; // report headers to print
		DWORD						m_dwAutoReportBodyStyle; // style of report body
		DWORD						m_dwAutoReportFlags; // what types of reports to count and print
		int							m_iAutoReportNumReports; // limit to report on
		char*						m_pszAutoReportDirpath; // path to directory to report to
		BOOL						m_fAutoReportDeleteNonPrinted; // whether to delete reports that weren't printed



		HRESULT HandleEnumMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_ENUM pEnumMsg);

		HRESULT HandleHandshakeMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_HANDSHAKE pHandshakeMsg);

		HRESULT HandleJoinMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_JOIN pJoinMsg);

		HRESULT HandleReadyToTestMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_READYTOTEST pReadyToTestMsg);

		HRESULT HandleAnnounceSubTestMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_ANNOUNCESUBTEST pAnnounceSubTestMsg);

		HRESULT HandleReportMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_REPORT pReportMsg);

		HRESULT HandleWarningMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_WARNING pWarningMsg);

		HRESULT HandleRequestSyncMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_REQUESTSYNC pRequestSyncMsg);

		HRESULT HandleUserQueryMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_USERQUERY pUserQueryMsg);

		HRESULT HandleIdleMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
							PCTRLMSG_IDLE pIdleMsg);

		HRESULT HandleRegisterMasterReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_REGISTERMASTERREPLY pRegisterMasterReplyMsg);

		HRESULT HandleMasterPingReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_MASTERPINGSLAVEREPLY pMasterReplyMsg);

		HRESULT HandleRequestPokeTestMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_REQUESTPOKETEST pRequestPokeTestMsg);

		HRESULT HandleAcceptReachCheckReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
												PCTRLMSG_ACCEPTREACHCHECKREPLY pReplyMsg);

		HRESULT HandleConnectReachCheckReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
												PCTRLMSG_CONNECTREACHCHECKREPLY pReplyMsg);

		HRESULT HandleFreeOutputVarsMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_FREEOUTPUTVARS pFreeOutputVarsMsg);

		HRESULT HandleGetMachineInfoMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_GETMACHINEINFO pGetMachineInfoMsg);


		HRESULT GetNextCmdForMachine(PTNSLAVEINFO pSlave);

		HRESULT AssignSlaveToTest(PTNSLAVEINFO pSlave, PTNTESTINSTANCEM pTest,
									int iTesterNum, BOOL fPlaceholder,
									BOOL fGetInputData, PVOID pvInputData,
									DWORD dwInputDataSize);

		HRESULT CompleteSlavesTest(PTNSLAVEINFO pSlave, PTNTESTINSTANCEM pTest,
									BOOL fSuccess, BOOL fSubTest, BOOL fOngoing);

		HRESULT KillSlave(PTNSLAVEINFO pSlave, DWORD dwKillReason);

		HRESULT CompleteAllTesting(void);

		HRESULT SendTestingStatusMessageTo(PTNSLAVEINFO pSlave, DWORD dwStatus);

		HRESULT SendNewTestMessageTo(PTNSLAVEINFO pSlave, PTNTESTINSTANCEM pTest,
									PVOID pvInputData, DWORD dwInputDataSize,
									PTNSENDDATAQUEUE pSendsBuffer);

		HRESULT SendAnnounceTestReplyMessageTo(PTNSLAVEINFO pSlave,
												DWORD dwResponseID,
												DWORD dwUniqueTestID,
												PTNSENDDATAQUEUE pSendsBuffer);

		HRESULT SendLostTesterTo(PTNSLAVEINFO pSlave, PTNTESTINSTANCEM pTest,
								int iTesterNum);

		HRESULT SendBreakMsgTo(PTNSLAVEINFO pSlave, DWORD dwUniqueTestID);

		HRESULT SendDumpLogMsgTo(PTNSLAVEINFO pSlave, DWORD dwUniqueTestID,
								char* pszPath);

		HRESULT SendMasterUpdate(void);

		HRESULT RegisterWithMetaMaster(void);

		HRESULT UnregisterWithMetaMaster(void);

		HRESULT DoBeginReport(HANDLE hFile, DWORD dwStyle);

		int CountMatchingReports(DWORD dwFlags, PTNSLAVESLIST pMachineList,
								PTNTESTMSLIST pTestList);

		HRESULT DoAutoReport(void);

		HRESULT CloseSession(void);

		HRESULT DoPingCheck(void);

		HRESULT DoSlaveReadyToTest(PTNCTRLMACHINEID pSlaveID);

		HRESULT DoHandleReport(PTNSLAVEINFO pSlave, PCTRLMSG_REPORT pReportMsg);

		HRESULT ReadFromFile(char* szFilepath, DWORD dwNumReps);

		HRESULT StoreReadTest(PREADSCRIPTFILECONTEXT pData);

		HRESULT BuildAPITestList(void);

		HRESULT BuildTestPermutationsList(PTNTESTFROMFILE pBaseTest,
										PTNTESTMSLIST pTestList,
										int* piMaxNumPermutations);

		HRESULT PermuteRemainingSlots(PTNTESTFROMFILE pBaseTest,
									int iTestingSlot,
									PTNSLAVEINFO* papTesters,
									PTNSLAVESLIST* papRemainingPossibleLists,
									PTNTESTMSLIST pResults,
									int* piMaxNumPermutations);

		PTNTESTINSTANCEM GetActiveTest(PTNTESTINSTANCEM pParentTest,
										char* szCaseID,
										int iNumMachines,
										TNCTRLMACHINEID* aTestersArray);

		HRESULT StartJobThread(void);

		HRESULT KillJobThread(void);
		
		HRESULT SendPingTo(PTNSLAVEINFO pSlave);

		HRESULT InternalExpandStringVars(char* szString, char* pszBuffer,
										DWORD* pdwBufferSize,
										PTNOUTPUTVARSLIST* apLists,
										DWORD dwNumLists);

		HRESULT InternalIsVarOfType(char* szString, char* szType,
									PVOID* ppvValueLocation,
									DWORD* pdwValueSize,
									PTNOUTPUTVARSLIST* apLists,
									DWORD dwNumLists);

		HRESULT FreeOutputVarsAndData(char* pszCaseID, char* pszInstanceID,
									PTNSLAVEINFO pSlave, char* pszName,
									char* pszType);

		HRESULT PerformReachCheck(DWORD dwMethod,
								PTNSLAVEINFO pSource,							
								PVOID pvSourceMethodData,
								DWORD dwSourceMethodDataSize,
								PVOID pvAddressesToTry,
								DWORD dwAddressesToTrySize,
								PTNSLAVEINFO pTarget,
								PVOID pvTargetMethodData,
								DWORD dwTargetMethodDataSize,
								BOOL* pfResult);



	protected:
		HANDLE			m_hKillJobThreadEvent; // event to set to gracefully exit the thread
		CTNJobQueue		m_jobs; // job queue for worker thread


		HRESULT ProcessJobQueue(DWORD* pdwTimeToNextJob);

		// Virtual implementations
		HRESULT HandleMessage(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
							  PVOID pvData, DWORD dwDataSize);



	public:
		CTNMaster(void);
		virtual ~CTNMaster(void);


		HRESULT CreateControlSession(PTNCREATECONTROLSESSIONDATA pTNccsd);

		HRESULT DestroyControlSession(void);

		DWORD GetSessionID(void);
		
		HRESULT GetModuleString(char* szString);

		PTNMACHINEINFO GetIndexedMachineInfo(int iMachineNum);

		PTNTESTSTATS GetTotalStats(void);

		int GetNumberOfCases(void);

		int GetNumberOfActiveAndRemainingTests(void);

		int GetNumTestsSkipped(void);

		BOOL IsSessionClosed(void);

		BOOL IsTestingComplete(void);

		HRESULT GetCaseIndex(int iIndex, char** ppszID, char** ppszName,
							PTNTESTSTATS* ppStats);

		HRESULT SearchStringForDynamicVars(char* szString, char** ppszNameFound,
											DWORD* pdwNameLength);

		HRESULT ExpandBuiltinStringVars(char* szString, char* pszBuffer,
										DWORD* pdwBufferSize);

		HRESULT ExpandSlaveStringVars(char* szString, PTNMACHINEINFO pSlave,
										char* pszBuffer, DWORD* pdwBufferSize);

		HRESULT ExpandTestersStringVars(char* szString, PTNTESTINSTANCEM pTest,
										char* pszBuffer, DWORD* pdwBufferSize);

		HRESULT IsBuiltinVarOfType(char* szString, char* szType,
									PVOID* ppvValueLocation,
									DWORD* pdwValueSize);

		HRESULT IsSlavesVarOfType(char* szString, char* szType,
									PTNMACHINEINFO pSlave,
									PVOID* ppvValueLocation,
									DWORD* pdwValueSize);

		HRESULT IsTestersVarOfType(char* szString, char* szType,
									PTNTESTINSTANCEM pTest,
									PVOID* ppvValueLocation,
									DWORD* pdwValueSize);

		HRESULT PrintTestTableToFile(char* szFilepath);

		HRESULT PrintResultsToFile(char* szFilepath);

		HRESULT PrintReports(DWORD dwHeaders, DWORD dwBodyStyle, DWORD dwFlags,
							char* szFilepath, BOOL fAppend, BOOL fDeletePrinted);

		void SetMaxNumberReports(int iNumReports);

		HRESULT AutoPrintReports(DWORD dwHeaders, DWORD dwBodyStyle, DWORD dwFlags,
								int iNumReports, char* szDirpath, BOOL fDeleteNonPrinted);

		BOOL AreOnSameMachine(PTNMACHINEINFO pFirstSlave,
							PTNMACHINEINFO pSecondSlave);

		HRESULT CanReachViaIP(PTNMACHINEINFO pConnector,
								PTNMACHINEINFO pListener,
								WORD wPort,
								DWORD dwMethodFlags,
								BOOL* pfReachable);

		HRESULT CanReachViaIPX(PTNMACHINEINFO pSender,
								PTNMACHINEINFO pReceiver,
								BOOL* pfReachable);

		HRESULT CanReachViaModem(PTNMACHINEINFO pCaller,
								PTNMACHINEINFO pAnswerer,
								BOOL* pfReachable);

		HRESULT CanReachViaSerial(PTNMACHINEINFO pSender,
								PTNMACHINEINFO pReceiver,
								BOOL* pfReachable);

		HRESULT GetPlannedInputData(int iTesterNum, PTNCANRUNDATA pTNcrd,
									PVOID pvInputData, DWORD* pdwInputDataSize);
};







#else //__TESTNET_MASTER__
//#pragma message("__TESTNET_MASTER__ already included!")
#endif //__TESTNET_MASTER__
#endif // ! XBOX