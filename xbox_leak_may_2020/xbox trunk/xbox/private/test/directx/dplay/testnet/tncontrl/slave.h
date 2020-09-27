#ifndef __TNCONTRL_SLAVE__
#define __TNCONTRL_SLAVE__
//#pragma message("Defining __TNCONTRL_SLAVE__")






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
#define IDLE_INTERVAL		10000 // in milliseconds




//==================================================================================
// Prototypes
//==================================================================================
DWORD WINAPI SlavePeriodicSendThreadProc(LPVOID lpvParameter);





//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNSlave:public CTNControlLayer
{
	// These are friends so they can access the protected members
	friend DWORD WINAPI SlavePeriodicSendThreadProc(LPVOID lpvParameter);
	friend CTNExecutor;
	friend CTNExecutorPriv;


	private:
		CTNExecutorPriv				m_executor; // executor object we use
		DWORD						m_dwCurrentUniqueID; // uniqueness counter for various operations
		CTNTestStats				m_totalstats; // statistics maintained on all tests

		DWORD						m_dwTimelimit; // time limit for testing
		char*						m_pszSessionFilter; // user specified session filter string
		PTNDOSTARTUPPROC			m_pfnDoStartup; // callback to use to run startup data
		PTNCHANGEBINARYSETPROC		m_pfnChangeBinarySet; // callback to use when swapping binary sets
		char*						m_pszSavedBinsDirPath; // directory to use when saving and restoring binary sets
		PLSTRINGLIST				m_pFailureBreakRules; // pointer to list of cases to DebugBreak() on
		char*						m_pszFailureMemDumpDirPath; // path to directory to dump logs to on failures
		BOOL						m_fGetNetStatInfoOnFailure; // whether to dump net stat info on failure or not

		DWORD						m_dwSessionID; // ID of current session
		HANDLE						m_hPeriodicSendThread; // the enumeration/joining etc. thread
		BOOL						m_fReadyToConnect; // should we respond to EnumReplies?
		BOOL						m_fInSession; // are we currently in a session?
		BOOL						m_fTestingComplete; // is all testing finished?
		PVOID						m_pvMetaMasterAddress; // pointer to buffer with meta-master's address
		DWORD						m_dwMetaMasterAddressSize; // size of meta-master's address
		PVOID						m_pvTestmasterCommData; // actually a PCOMMDATA pointer; testmaster's address data
		DWORD						m_dwLastMasterReceive; // tick count of when we last received data from the master
		PTNBINARYMANAGER			m_pBinMgr; // object to use for snapshot
		CTNReachCheckTargetsList	m_reachchecktargets; // list of targets of reach checks that have been performed
		PTNPERIODICSEND				m_pSessionPeriodicSend; // currently outstanding periodic send we're using to join a session
		HANDLE						m_hMultiInstanceMutex; // mutex used to synchronize access to the shared instance list
		HANDLE						m_hMultiInstanceMMFile; // handle to shared instance list
		int							m_iInstanceIndex; // index of our slot in the shared instance list
		CTNOtherMachineInfosList	m_otherinfos; // info objects for other machines in session

#ifdef _XBOX // We don't support filemapping, so we just store the key that the master wants
		DWORD						m_dwMultiInstanceKey;
#endif

		HRESULT HandleEnumReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_ENUMREPLY pEnumReplyMsg);
		HRESULT HandleHandshakeReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_HANDSHAKEREPLY pHandshakeReplyMsg);
		HRESULT HandleJoinReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_JOINREPLY pJoinReplyMsg);
		HRESULT HandleTestingStatusMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_TESTINGSTATUS pTestingStatusMsg);
		HRESULT HandleNewTestMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_NEWTEST pNewTestMsg);
		HRESULT HandleAnnounceTestReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_ANNOUNCETESTREPLY pAnnounceTestReplyMsg);
		HRESULT HandleSyncReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_SYNCREPLY pSyncReplyMsg);
		HRESULT HandleUserResponseMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_USERRESPONSE pUserResponseMsg);
		HRESULT HandleIdleReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_IDLEREPLY pIdleReplyMsg);
		HRESULT HandleDumpLogMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_DUMPLOG pDumpLogMsg);
		HRESULT HandleBreakMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_BREAK pBreakMsg);
		HRESULT HandleMetaMasterReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_METAMASTERREPLY pMetaMasterReplyMsg);
		HRESULT HandleMasterPingSlaveMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_MASTERPINGSLAVE pMasterPingMsg);
		HRESULT HandleLostTesterMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_LOSTTESTER pLostTesterMsg);
		HRESULT HandleAcceptReachCheckMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_ACCEPTREACHCHECK pAcceptReachCheckMsg);
		HRESULT HandleConnectReachCheckMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_CONNECTREACHCHECK pConnectReachCheckMsg);
		HRESULT HandleInterSlaveReachCheckMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_INTERSLAVEREACHCHECK pInterSlaveReachCheckMsg);
		HRESULT HandleInterSlaveReachCheckReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
													PCTRLMSG_INTERSLAVEREACHCHECKREPLY pReplyMsg);
		HRESULT HandleGetMachineInfoReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_GETMACHINEINFOREPLY pReplyMsg);


		HRESULT RegisterWithOtherLocalSlaves(void);

		HRESULT GetMultiInstanceKey(DWORD* pdwKey);

		HRESULT UnregisterWithOtherLocalSlaves(void);

		HRESULT InitializeDBLogging(void);

		HRESULT SendReadyForFirstTest(void);

		HRESULT DoIdle(DWORD dwCurrentOp);

		HRESULT StartPeriodicSend(PTNCTRLCOMM pCtrlComm, BOOL fGuaranteed,
									PVOID pvAddress, DWORD dwAddressSize,
									PVOID pvData, DWORD dwDataSize,
									DWORD dwInterval, DWORD dwTimeout,
									PVOID pvUserContext,
									PTNPERIODICSEND* ppPeriodicSend);

		HRESULT StopPeriodicSend(PTNPERIODICSEND pPeriodicSend);

		HRESULT KillPeriodicSendThread(BOOL fKillingSelf);

		HRESULT StartDBCase(PTNTESTINSTANCES pTest);

		HRESULT EndDBCase(PTNTESTINSTANCES pTest, BOOL fSuccess);

		HRESULT DoConnectReachCheckResult(PTNREACHCHECK pReachCheck, BOOL fSuccess,
										HRESULT hresult, PVOID pvAddress,
										DWORD dwAddressSize);

		void GetNetStatInfo(void);



	protected:
		HANDLE						m_hKillPeriodicSendThreadEvent; // when to kill the enumeration/joining etc thread
		CTNPeriodicSendQueue		m_periodicsends; // periodic sends that are going on
		BOOL						m_fQueryingMetaMaster; // should we respond to MetaMasterReplies?
		BOOL						m_fShakingHands; // should we respond to Handshake/JoinReplies?

#ifndef _XBOX
		LPTMAGENT					m_lpTMAgent; // pointer to object to use when/if logging to database
#endif // ! XBOX


		HRESULT TakeSnapshot(void);
		HRESULT RestoreSnapshot(void);

		HRESULT FreeOutputVars(char* pszCaseID, char* pszInstanceID,
							PTNSLAVEINFO pSlave, char* pszName, char* pszType);

		HRESULT ReportInternal(PTNTESTINSTANCES pTest, HRESULT hresult,
								BOOL fTestComplete, BOOL fSuccess,
								PVOID pvOutputData, DWORD dwOutputDataSize,
								PTNOUTPUTVARSLIST pOutputVars);

		HRESULT WarnInternal(PTNTESTINSTANCES pTest, HRESULT hresult,
							PVOID pvUserData, DWORD dwUserDataSize);

		HRESULT AnnounceSubTest(PTNTESTINSTANCES pTest, int iNumMachines,
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

		HRESULT GetTestersIPForPortInternal(PTNTESTINSTANCES pTest,
											int iTesterNum, WORD wPort,
											char* szIPString);

		HRESULT GetTestersPhoneNumInternal(PTNTESTINSTANCES pTest,
											int iTesterNum,
											char* szPhoneNumber);

		HRESULT GetTestersCOMPortInternal(PTNTESTINSTANCES pTest,
										int iTesterNum, DWORD* pdwCOMPort);

		HRESULT SendFreeOutputVars(char* pszCaseID, char* pszInstanceID,
									char* pszName, char* pszType);

		HRESULT GetTestersMachineInfoInternal(PTNTESTINSTANCES pTest,
											int iTesterNum,
											PTNMACHINEINFO* ppInfo);

		HRESULT PeriodicSendExpired(PTNPERIODICSEND pPeriodicSend);

		// Virtual implementations
		HRESULT HandleMessage(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
							  PVOID pvData, DWORD dwDataSize);


	public:
		CTNSlave(void);
		virtual ~CTNSlave(void);


		HRESULT StartJoiningControlSession(PTNJOINCONTROLSESSIONDATA pTNjcsd);

		HRESULT LeaveControlSession(void);

		BOOL IsInSession(void);

		DWORD GetSessionID(void);

		BOOL IsTestingComplete(void);

		HRESULT GetModuleString(char* szString);

		PTNTESTSTATS GetTotalStats(void);

		int GetNumberOfCases(void);

		HRESULT GetCaseIndex(int iIndex, char** ppszID, char** ppszName,
							PTNTESTSTATS* ppStats);

		PTNMACHINEINFO GetMachineInfo(void);

		HRESULT PrintTestTableToFile(char* szFilepath);

		HRESULT PrintResultsToFile(char* szFilepath);

		HRESULT UserQuery(DWORD dwQuery, PVOID pvQueryData, DWORD dwQueryDataSize);

		HRESULT ExecPokeTest(char* szCaseID,
							PVOID pvInputData,
							DWORD dwInputDataSize,
							PTNPOKETESTCOMPLETEDPROC pfnPokeTestCompleted,
							PVOID pvUserContext);
};






#else //__TNCONTRL_SLAVE__
//#pragma message("__TNCONTRL_SLAVE__ already included!")
#endif //__TNCONTRL_SLAVE__
