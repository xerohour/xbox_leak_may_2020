#ifndef __TNCONTRL_EXECUTOR__
#define __TNCONTRL_EXECUTOR__
//#pragma message("Defining __TNCONTRL_EXECUTOR__")






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
// External Structures
//==================================================================================
typedef struct tagTNEXECUTORINITBYSLAVEDATA
{
	// Pointer to slave object this executor represents.  This must be non-NULL.
	PTNSLAVE						pOwningSlave;

	// The mode this session will be run in (TNMODE_xxx).
	DWORD							dwMode;

	// Pointer to structure that uniquely identifies the calling application and
	// version of that app.
	PTNMODULEID						pModuleID;

	// Pointer to root testnet path.  Must end in a backslash.
	char*							pszTestNetRootPath;

	// Array of procedures that will be called to retrieve the total test list.
	// At least one entry required.
	PTNLOADTESTTABLEPROC*			apfnLoadTestTable;

	// Number of procedures in preceding array.  At least one is required.
	DWORD							dwNumLoadTestTables;

	// Procedure that will be called to retrieve the list of binaries that the
	// module wants to make note of, and/or may swap during a test.  Optional.
	PTNADDIMPORTANTBINARIESPROC		pfnAddImportantBinaries;

	// Procedure that will be called just before testing begins, to allow the slave
	// a chance to create any objects and do preparation for testing if it needs to.
	// Optional.
	PTNINITIALIZETESTINGPROC		pfnInitializeTesting;

	// Procedure that will be called after testing has completed, to allow the slave
	// to free any resources allocated during testing. Optional.
	PTNCLEANUPTESTINGPROC			pfnCleanupTesting;

	// Procedure that will be called to retrieve the window for APIs that require
	// it.  Optional.
	PTNGETTESTINGWINDOWPROC			pfnGetTestingWindow;

	// Whether CoInitialize (or CoInitializeEx, if available) should be called to
	// setup the testthread.
	BOOL							fInitializeCOMInTestThread;

	// Whether to prompt user for action if test thread has problems shutting down
	// or not.
	BOOL							fPromptTestThreadDieTimeout;

	// Handle to an manual-reset event that the app can set when it wants to abort
	// or cancel the session.  Optional.
	HANDLE							hUserCancelEvent;

	// Optional path to file to use when writing ErrToSrc data.
	char*							pszErrToSrcFilepath;

	// Optional search path to redirect ErrToSrc filepaths.
	char*							pszErrToSrcPathSearch;

	// Optional replace path to redirect ErrToSrc filepaths.
	char*							pszErrToSrcPathReplace;

	// Optional path to file to use when writing documentation build data.
	char*							pszDocFilepath;

	// Number of other slave instances on this machine.  Used to generate a unique
	// computer name.
	int								iMultiInstanceNum;
} TNEXECUTORINITBYSLAVEDATA, * PTNEXECUTORINITBYSLAVEDATA;





//==================================================================================
// Prototypes
//==================================================================================
DWORD WINAPI ExecutorTestThreadProc(LPVOID lpvParameter);





//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNExecutorPriv
{
	// These are friends so they can access the protected members
	friend CTNExecutor;
	friend DWORD WINAPI ExecutorTestThreadProc(LPVOID lpvParameter);
	friend CTNSlave;
	friend DWORD WINAPI SlavePeriodicSendThreadProc(LPVOID lpvParameter);
#ifndef _XBOX // no IPC supported
	friend CTNIPCObject;
	friend CTNLeech;
	friend CTNLeechesList;
#endif // ! XBOX


	private:
		HANDLE						m_hTestThread; // the test thread
		HANDLE						m_hKillTestThreadEvent; // when to kill the test thread
		HANDLE						m_hNewTestEvent; // set when there is a new test for the thread to run

		CTNTestSsList				m_queuedtests; // list of tests queued up to run
		PTNTESTINSTANCES			m_pTest; // test to run
		CTNTestSsList				m_ongoingtests; // list of ongoing tests we're still working on
		CTNResultsChain				m_results; // list of all results for this machine

		BOOL						m_fInitCOMForTestThread; // whether the test thread should initialize COM
		PTNINITIALIZETESTINGPROC	m_pfnInitializeTesting; // callback to use to prep for testing
		PTNCLEANUPTESTINGPROC		m_pfnCleanupTesting; // callback to use to clean up after testing
		PTNGETTESTINGWINDOWPROC		m_pfnGetTestingWindow; // callback to use to retrieve testing window

		HANDLE						m_hErrToSrcFile; // handle to file being used to write ErrToSrc data
		char*						m_pszErrToSrcPathSearch; // pointer to ErrToSrc path strings to search for
		char*						m_pszErrToSrcPathReplace;  // pointer to ErrToSrc path strings to replace with
		HANDLE						m_hDocFile; // handle to file being used to write documentation-build data

		BOOL						m_fPromptTestThreadDieTimeout; // boolean whether we should prompt for action when test thread has problems shutting down


	protected:
		CTNMachineInfo				m_info; // information about this machine object
		DWORD						m_dwMode; // what mode this object is in (TNMODE_xxx)
		TNMODULEID					m_moduleID; // ID of module using us
		HANDLE						m_hUserCancelEvent; // event given to us by user, set when the user wants to abort

		DWORD						m_dwTestThreadID; // ID of the test thread
		BOOL						m_fDeadlockCheck; // whether to do the deadlock check or not
		DWORD						m_dwLastTestthreadPing; // last time we heard from the test thread

		PTNSLAVE					m_pOwningSlave; // pointer to slave object this executor represents

#ifndef _XBOX // no IPC supported
		PTNLEECH					m_pOwningLeech; // pointer to leech object this executor represents
#endif // ! XBOX

		TNCTRLMACHINEID				m_id; // ID to identify this object in the control session
		CTNTestTableGroup			m_testtable; // table of tests generated from the module

#ifndef _XBOX // no IPC supported
		CTNLeechesList				m_leeches; // list of leech processes attached to this object
#endif // ! XBOX

		CTNFaultSimList				m_faultsims; // list of fault simulator objects used by this object

		
		DLLSAFE_NEWS_AND_DELETES;

		CTNExecutorPriv(void);
		virtual ~CTNExecutorPriv(void);


		HRESULT InitializeBySlave(PTNEXECUTORINITBYSLAVEDATA pTNeibsd);

		HRESULT Cleanup(void);

		HRESULT StartTestThread(void);

		HRESULT KillTestThread(BOOL fKillingSelf);

		HRESULT HandleNewTest(DWORD dwUniqueTestID,
							char* pszCaseID, char* pszInstanceID,
							int iNumMachines, PTNCTRLMACHINEID aTesters,
							PVOID pvInputData, DWORD dwInputDataSize);

		HRESULT HandleAnnounceResponse(DWORD dwResponseID, DWORD dwUniqueTestID);

		HRESULT HandleSyncResponse(DWORD dwResponseID, PVOID pvData, DWORD dwDataSize);

		HRESULT HandleLostTester(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
								int iLostTesterNum);

		HRESULT HandleGetMachineInfoResponse(DWORD dwResponseID, PVOID pvData,
											DWORD dwDataSize);

		HRESULT DoTestThread(void);

		HRESULT LogInternal(DWORD dwLogStringType, char* szString);

		HRESULT SprintfLogInternal(DWORD dwLogStringType, char* szFormatString,
								DWORD dwNumParms, ...);

		HRESULT ReportInternal(PTNTESTINSTANCES pTest, HRESULT hresult,
								BOOL fTestComplete, BOOL fSuccess,
								BOOL fExecCaseExitReport,
								PVOID pvOutputData, DWORD dwOutputDataSize,
								PTNOUTPUTVARSLIST pOutputVars);

		HRESULT WarnInternal(PTNTESTINSTANCES pTest, HRESULT hresult,
							PVOID pvUserData, DWORD dwUserDataSize);

		HRESULT CreateSubTest(PTNTESTINSTANCES pParentTest,
								PTNTESTTABLECASE pCase,
								int iNumMachines,
								PTNCTRLMACHINEID aTesters);

		HRESULT SyncInternal(PTNTESTINSTANCES pTest,
							char* szSyncName,
							PVOID pvSendData,
							DWORD dwSendDataSize,
							PTNSYNCDATALIST pReceiveData,
							int iNumMachines,
							int* aiTesters);

		BOOL IsTesterOnSameMachineInternal(PTNTESTINSTANCES pTest,
											int iTesterNum);

		HRESULT GetTesterIPInternal(PTNTESTINSTANCES pTest, int iTesterNum,
									WORD wPort, char* szIPString);

		HRESULT GetTesterPhoneNumInternal(PTNTESTINSTANCES pTest, int iTesterNum,
										char* szPhoneNumber);

		HRESULT GetTesterCOMPortInternal(PTNTESTINSTANCES pTest, int iTesterNum,
										DWORD* lpdwCOMPort);

		PTNTESTINSTANCES GetTest(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID);

		PTNTESTINSTANCES GetLeechTest(DWORD dwLeechTopLevelUniqueID,
										DWORD dwTestUniqueID);

		HRESULT RunTest(PTNTESTINSTANCES pTest, PVOID pvInputData,
						DWORD dwInputDataSize);

		HRESULT FreeResultsInternal(char* pszCaseID, char* pszInstanceID,
									PTNTESTINSTANCES pUserCallingTest);

		HRESULT FreeOutputVarsInternal(char* pszCaseID, char* pszInstanceID,
									char* pszName, char* pszType,
									BOOL fFreeData,
									PTNTESTINSTANCES pUserCallingTest);

		HRESULT GetTestersMachineInfoInternal(PTNTESTINSTANCES pTest,
											int iTesterNum,
											PTNMACHINEINFO* ppInfo);

		HRESULT DoDocumentation(PTNTESTTABLEITEM pItem);
};


class DLLEXPORT CTNExecutor
{
	// These are friends so they can access the protected members
	friend CTNExecutorPriv;


	private:
		PTNEXECUTORPRIV		m_pRealExecutor; // pointer to real executor class
		PTNTESTINSTANCES	m_pTest; // test being run
		char*				m_pszTestSectionName; // string name of most recent test section name
		char*				m_pszTestSectionFilepath; // string with most recent test section source file path
		int					m_iTestSectionLineNum; // most recent test section source line number


	protected:
		DLLSAFE_NEWS_AND_DELETES;

		CTNExecutor(PTNEXECUTORPRIV pRealExecutor, PTNTESTINSTANCES pTest);
		virtual ~CTNExecutor(void);


	public:
		BOOL IsCase(char* szCaseID);

		HWND GetTestingWindow(void);

		HRESULT FirstBinIsVersion(char* szBinaryName, DWORD dwHighVersion,
									DWORD dwLowVersion, BOOL* pfResult);

		HRESULT FirstBinIsAtLeastVersion(char* szBinaryName, DWORD dwHighVersion,
										DWORD dwLowVersion, BOOL* lpfResult);

		HRESULT Log(DWORD dwFlags, char* szFormatString, DWORD dwNumParms, ...);

		HRESULT Report(HRESULT hresult, BOOL fSuccess);

		HRESULT Warn(HRESULT hresult, PVOID pvUserData, DWORD dwUserDataSize);

		HRESULT WaitForEventOrCancel(HANDLE* pahEvents, int iNumEvents,
									int* paiRelevantTesters, int iNumRelevantTesters,
									DWORD dwTimeout, int* piItemNum);

		HRESULT ExecSubTestCase(char* szCaseModuleID, PVOID pvSubInputData,
								DWORD dwSubInputDataSize, int iNumMachines, ...);

		HRESULT ExecSubTestCaseArray(char* szCaseModuleID, PVOID pvSubInputData,
									DWORD dwSubInputDataSize, int iNumMachines,
									int* aiTesterArray);

		HRESULT SyncWithTesters(char* szSyncName,
								PVOID pvSendData,
								DWORD dwSendDataSize,
								PTNSYNCDATALIST pReceiveData,
								int iNumMachines, ...);

		HRESULT SyncWithTestersArray(char* szSyncName,
									PVOID pvSendData,
									DWORD dwSendDataSize,
									PTNSYNCDATALIST pReceiveData,
									int iNumMachines,
									int* aiTesters);

		BOOL IsTesterOnSameMachine(int iTesterNum);

		HRESULT GetTestersIPForPort(int iTesterNum, WORD wPort,
									char* szIPString);

		HRESULT GetFirstTAPIDeviceNameWithNumber(char** ppszDeviceName);

		HRESULT GetPhoneNumberForTester(int iTesterNum, char* szPhoneNumber);

		HRESULT GetCOMPortConnectedToTester(int iTesterNum, DWORD* pdwCOMPort);

#ifndef _XBOX // no IPC supported
		HRESULT CreateNewLeechAttachment(PTNLEECH* ppLeech,
										PVOID pvSendConnectData,
										DWORD dwSendConnectDataSize);

		HRESULT DetachAndReleaseLeech(PTNLEECH* ppLeech);

		HRESULT AllowStaticIDAttachment(PTNLEECH pLeech, char* szPreplannedID);

		HRESULT WaitForLeechConnection(PTNLEECH pTNLeech, DWORD dwTimeout);

		HRESULT GetLeechConnectData(PTNLEECH pTNLeech, PVOID* ppvData,
									DWORD* pdwDataSize);

		HRESULT ExecLeechTestCase(PTNLEECH pLeech, char* szCaseModuleID,
								PVOID pvLeechInputData, DWORD dwLeechInputDataSize,
								int iNumMachines, ...);

		HRESULT ExecLeechTestCaseArray(PTNLEECH pLeech, char* szCaseModuleID,
									PVOID pvLeechInputData, DWORD dwLeechInputDataSize,
									int iNumMachines, int* aiTesterArray);
#endif // ! XBOX

		HRESULT CreateNewFaultSim(PTNFAULTSIM* ppFaultSim, DWORD dwFaultSimID,
									PVOID pvInitData, DWORD dwInitDataSize);

		HRESULT ReleaseFaultSim(PTNFAULTSIM* ppFaultSim);

		HRESULT SetFaultSimBandwidth(PTNFAULTSIM pFaultSim,
									BOOL fSend, DWORD dwHundredBytesPerSec);
		HRESULT SetFaultSimLatency(PTNFAULTSIM pFaultSim,
									BOOL fSend, DWORD dwMSDelay);
		HRESULT SetFaultSimDropPacketsPercent(PTNFAULTSIM pFaultSim,
											BOOL fSend, DWORD dwPercent);
		/*
		HRESULT SetFaultSimDropPacketsPattern(PTNFAULTSIM pFaultSim,
											BOOL fSend, char* szPatternString);
		HRESULT FaultSimCorruptPackets(PTNFAULTSIM pFaultSim,
										BOOL fSend, ?);
		HRESULT FaultSimReorderPackets(PTNFAULTSIM pFaultSim,
										BOOL fSend, ?);
		*/

		HRESULT FaultSimReconnect(PTNFAULTSIM pFaultSim,
									BOOL fSend);
		
		HRESULT FaultSimDisconnect(PTNFAULTSIM pFaultSim,
									BOOL fSend);
		
		void NoteTestIsStillRunning(void);

		HRESULT SetDeadlockCheck(BOOL fOn);

		HRESULT FreeResults(char* pszCaseID, char* pszInstanceID);

		HRESULT FreeOutputVars(char* pszCaseID, char* pszInstanceID,
							char* pszName, char* pszType, BOOL fFreeData);

#ifndef _XBOX
		HRESULT PromptUser(char* szTitle, char* szMessageFormat,
							DWORD dwDialogType,
							PVOID pvDialogData, DWORD dwDialogDataSize,
							int* paiRelevantTesters, int iNumRelevantTesters,
							int* piResponse,
							DWORD dwNumParms, ...);
#endif // ! XBOX

		HRESULT GetMachineInfoForTester(int iTesterNum, PTNMACHINEINFO* ppInfo);
		HRESULT ReleaseMachineInfoForTester(PTNMACHINEINFO* ppInfo);

		HRESULT NoteBeginTestCase(char* szDebugModule,
									char* szDebugSection,
									char* szBeginTestCaseFilepath,
									int iBeginTestCaseLineNum);

		HRESULT NoteNewTestSection(char* szTestSectionName,
									char* szDebugModule,
									char* szDebugSection,
									char* szTestSectionFilepath,
									int iTestSectionLineNum);

		HRESULT NoteThrowTestResult(char* szThrowTestResultFilepath,
									int iThrowTestResultLineNum);

#ifndef _XBOX // no documentation builds supported
		HRESULT DocBuildBeginTestCase(char* szBeginTestCaseFilepath,
									int iBeginTestCaseLineNum);

		HRESULT DocBuildEndTestCase(char* szEndTestCaseFilepath,
									int iEndTestCaseLineNum);

		HRESULT DocBuildNewTestSection(char* szTestSectionName,
										char* szTestSectionFilepath,
										int iTestSectionLineNum);

		HRESULT DocBuildTestSectionControl(char* szControl,
										char* pszControlExpression,
										char* szControlFilepath,
										int iControlLineNum);
#endif // ! XBOX
};




#else //__TNCONTRL_EXECUTOR__
//#pragma message("__TNCONTRL_EXECUTOR__ already included!")
#endif //__TNCONTRL_EXECUTOR__
