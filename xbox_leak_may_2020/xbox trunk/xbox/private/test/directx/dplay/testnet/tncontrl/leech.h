#ifndef _XBOX // no IPC supported
#ifndef __TNCONTROL_LEECH__
#define __TNCONTROL_LEECH__
//#pragma message("Defining __TNCONTROL_LEECH__")






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
// Prototypes
//==================================================================================
DWORD WINAPI LeechTestThreadProc(LPVOID lpvParameter);





//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNLeech:public CTNIPCObject
{
	// These are friends so that they can access the protected members
	friend CTNExecutor;
	friend CTNExecutorPriv;


	private:
		// These are implementations of the virtual functions.
		virtual HRESULT HandleMessage(DWORD dwCommand,
									PVOID pvSentData, DWORD dwSentDataSize,
									PVOID pvReplyData, DWORD* pdwReplyDataSize);

		virtual HRESULT HandleDisconnect(void);



		HRESULT HandleLogStringMsg(DWORD dwLogStringType, char* szString);

		HRESULT HandleReportMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID, 
								HRESULT hresult, BOOL fTestComplete,
								BOOL fSuccess, BOOL fExecCaseExitReport,
								DWORD dwOutputDataSize, DWORD dwOutputVarsSize,
								PVOID pvRemainingData);

		HRESULT HandleWarningMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID, 
								HRESULT hresult, PVOID pvUserData,
								DWORD dwUserDataSize);

		HRESULT HandleCreateSubTestMsg(DWORD dwTopLevelUniqueID,
										DWORD dwParentUniqueID,
										int iNumMachines,													
										PVOID pvRemainingData,
										DWORD* pdwAssignedUniqueID);

		HRESULT HandleSyncMsg(DWORD dwTopLevelUniqueID,
							DWORD dwTestUniqueID,
							int iNumMachines,
							DWORD dwSendDataSize,
							PVOID pvRemainingData,
							PVOID pvReplyData,
							DWORD* pdwReplyDataSize);

		HRESULT HandleIsTesterOnMachineMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
										int iTesterNum, BOOL* pfResult);

		HRESULT HandleGetTesterIPMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
									int iTesterNum, WORD wPort,
									char* pszReplyString, DWORD* pdwReplyStringSize);

		HRESULT HandleGetTesterPhoneNumMsg(DWORD dwTopLevelUniqueID,
										DWORD dwTestUniqueID,
										int iTesterNum,
										PVOID pvReplyMsg, // PVOID is actually a PLEECHMSG_GETTESTERPHONENUMREPLY
										DWORD* pdwReplyMsgSize);

		HRESULT HandleGetTesterCOMPortMsg(DWORD dwTopLevelUniqueID,
										DWORD dwTestUniqueID,
										int iTesterNum, PVOID pvReplyMsg); // PVOID is actually a PLEECHMSG_GETTESTERCOMPORTREPLY

		HRESULT HandleFreeResultsMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
									DWORD dwCaseIDSize, DWORD dwInstanceIDSize,
									PVOID pvRemainingData);

		HRESULT HandleFreeOutputVarsMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
										DWORD dwCaseIDSize, DWORD dwInstanceIDSize,
										DWORD dwNameSize, DWORD dwTypeSize,
										BOOL fFreeData, PVOID pvRemainingData);

		HRESULT HandleRunTestMsg(DWORD dwTestUniqueID, int iNumMachines,
								DWORD dwInputDataSize, PVOID pvRemainingData);

		HRESULT HandleGetMachineInfoMsg(DWORD dwTopLevelUniqueID,
										DWORD dwTestUniqueID,
										int iTesterNum,
										PVOID pvReplyMsg, // PVOID is actually a PLEECHMSG_GETMACHINEINFOREPLY
										DWORD* pdwReplyMsgSize);

		HRESULT HandleLostTesterMsg(DWORD dwTopLevelUniqueID,
									DWORD dwTestUniqueID,
									int iLostTesterNum);


	protected:
		HRESULT LogToOwner(DWORD dwLogStringType, char* szFormatString, DWORD dwNumParms, ...);

		HRESULT ReportToOwner(PTNTESTINSTANCES pTest, HRESULT hresult,
							BOOL fTestComplete, BOOL fSuccess,
							BOOL fExecCaseExitReport,
							PVOID pvOutputData, DWORD dwOutputDataSize,
							PTNOUTPUTVARSLIST pOutputVars);

		HRESULT WarnOwner(PTNTESTINSTANCES pTest, HRESULT hresult,
							PVOID pvUserData, DWORD dwUserDataSize);

		HRESULT HaveOwnerCreateSubTest(PTNTESTINSTANCES pTest, int iNumMachines,
										PTNCTRLMACHINEID aTesters);

		HRESULT HaveOwnerSync(PTNTESTINSTANCES pTest,
							char* szSyncName,
							PVOID pvSendData,
							DWORD dwSendDataSize,
							PTNSYNCDATALIST pReceiveData,
							int iNumMachines,
							int* aiTesters);

		BOOL HaveOwnerCheckIfTesterOnMachine(PTNTESTINSTANCES pTest,
											int iTesterNum);

		HRESULT HaveOwnerGetTesterIP(PTNTESTINSTANCES pTest,
									int iTesterNum, WORD wPort,
									char* szIPString);

		HRESULT HaveOwnerGetTesterPhoneNum(PTNTESTINSTANCES pTest,
										int iTesterNum, char* szPhoneNumber);

		HRESULT HaveOwnerGetTesterCOMPort(PTNTESTINSTANCES pTest,
										int iTesterNum, DWORD* pdwCOMPort);

		HRESULT HaveOwnerFreeResults(PTNTESTINSTANCES pTest, char* pszCaseID,
									char* pszInstanceID);

		HRESULT HaveOwnerFreeOutputVars(PTNTESTINSTANCES pTest, char* pszCaseID,
										char* pszInstanceID, char* pszName,
										char* pszType, BOOL fFreeData);

		HRESULT HaveOwnerGetTestersMachineInfo(PTNTESTINSTANCES pTest,
												int iTesterNum,
												PTNMACHINEINFO* ppInfo);

		HRESULT HaveLeechRunTest(PTNTESTINSTANCES pTest, PVOID pvInputData,
								DWORD dwInputDataSize);

		HRESULT SendLostTesterMsgToOtherSide(DWORD dwTopLevelUniqueID,
											DWORD dwTestUniqueID,
											int iLostTesterNum);


	public:
		CTNLeech(void);
		virtual ~CTNLeech(void);


		HRESULT PerformLeechTesting(PTNPERFORMLEECHTESTINGDATA pTNpltd);

		char* GetAttachPointID(void);

		HRESULT GetOwnerConnectData(PVOID* ppvData, DWORD* pdwDataSize);
};

class DLLEXPORT CTNLeechesList:public CTNIPCObjectsList
{
	// These are friends so that they can access the protected members
	friend CTNExecutor;
	friend CTNExecutorPriv;


	protected:
		PTNLEECH NewLeech(PTNMODULEID pTNModuleID,
							PTNEXECUTORPRIV pOwningExecutor,
							PVOID pvSendConnectData,
							DWORD dwSendConnectDataSize);
		HRESULT RemoveLeech(PTNLEECH pTNLeech);


	public:
		CTNLeechesList(void);
		virtual ~CTNLeechesList(void);
};






#else //__TNCONTROL_LEECH__
//#pragma message("__TNCONTROL_LEECH__ already included!")
#endif //__TNCONTROL_LEECH__
#endif // ! XBOX