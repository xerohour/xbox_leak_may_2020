#ifndef __TNCONTROL_TESTINSTANCE__
#define __TNCONTROL_TESTINSTANCE__
//#pragma message("Defining __TNCONTROL_TESTINSTANCE__")






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
// Structures
//==================================================================================
typedef struct tagTNTESTERSLOTM
{
	PTNSLAVEINFO		pSlave; // slave for this slot
	DWORD				dwAnnounceRequestID; // ID slave used to request this test
	DWORD				dwAssignTime; // time the slave filled his own slot
	HRESULT				hresult; // the final result for this slave
	BOOL				fComplete; // whether this slave has completed the test or not
	BOOL				fSuccess; // the final success/failure for this slave
	int					iNumWarnings; // number of warnings this slave has generated
	PVOID				pvOutputData; // output data for this slave
	DWORD				dwOutputDataSize; // size of output data
	PTNOUTPUTVARSLIST	pVars; // pointer to list of variables, if any
} TNTESTERSLOTM, * PTNTESTERSLOTM;

typedef struct tagTNTESTERSLOTS
{
	TNCTRLMACHINEID		id; // ID of tester at this location
	BOOL				fGone; // has this tester left the test?
} TNTESTERSLOTS, * PTNTESTERSLOTS;



//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNTestInstanceM:public LLITEM
{
	// These are friends so they can access the protected members
#ifndef _XBOX 
	friend CTNMaster;						// Masters not supported
	friend CTNReport;						// Reports not supported
	friend CTNReportsList;					// Reports not supported
#endif // ! XBOX
	friend CTNSlaveInfo;
	friend CTNTestMsList;


	protected:
		DWORD					m_dwUniqueID; // control defined ID of this test
		PTNTESTFROMFILE			m_pLoadedTest; // the original item loaded from file, if done so
		PTNTESTTABLECASE		m_pCase; // the test case this instance is representing
		int						m_iNumMachines; // the number of machines actually going to run this test
		//DWORD					m_dwOptions; // options for this instance (TNTIO_xxx)
		PTNTESTINSTANCEM		m_pParentTest; // pointer to parent for this test, refcount held for any object here
		PTNSYNCDATALIST			m_pSyncDataList; // list to use to store sync receive data
		CTNTestMsList			m_subtests; // list of currently executing subtests
		BOOL					m_fStarted; // have the testers been told to run this test yet
		PTNTESTERSLOTM			m_paTesterSlots; // used when handling tester assignments
		PTNTESTMSLIST			m_pTestsToFree; // pointer to list of subtests which had output data that should be freed when this test completes



		CTNTestInstanceM(DWORD dwUniqueID,
						PTNTESTFROMFILE pLoadedTest,
						PTNTESTTABLECASE pCase,
						int iNumMachines,
						//DWORD dwOptions,
						PTNTESTINSTANCEM pParentTest);
		virtual ~CTNTestInstanceM(void);



		PTNTESTINSTANCEM GetTopLevelTest(void);

		PTNTESTINSTANCEM GetTest(DWORD dwUniqueID);

		int GetSlavesTesterNum(PTNSLAVEINFO pSlave);

#ifndef _XBOX // no file printing supported
		HRESULT PrintToFile(HANDLE hFile, BOOL fSeparateSection,
							BOOL fSubTest);
#endif // ! XBOX
};


class DLLEXPORT CTNTestInstanceS:public LLITEM
{
	// These are friends so they can access the protected members
	friend CTNExecutor;
	friend CTNExecutorPriv;
	friend CTNSlave;

#ifndef _XBOX // no IPC supported
	friend CTNLeech;
#endif // ! XBOX

	friend CTNResult;
	friend CTNTestSsList;


	protected:
		DWORD						m_dwUniqueID; // control defined ID of this test
		PTNTESTTABLECASE			m_pCase; // the original item in the table
		char*						m_pszInstanceID; // user defined unique ID of this test instance
		int							m_iNumMachines; // the number of machines actually going to run this test
		//DWORD						m_dwOptions; // options for this instance (TNTIO_xxx)
		PTNTESTINSTANCES			m_pParentTest; // pointer to parent for this test, refcount held for any object here
		PTNTESTINSTANCES			m_pSubTest; // pointer to subtest for this test.  NOTE: no refcount is held for this!
		PTNTESTERSLOTS				m_paTesterSlots; // array of machines testing this
		PTNRESULTSCHAIN				m_pResultsToFree; // pointer to list of results from subtests that should be freed when this test completes
		PVOID						m_pvInputData; // pointer to initial data for test
		DWORD						m_dwInputDataSize; // size of initial data for test
		CRITICAL_SECTION			m_csMasterOp; // make sure we only have one operation waiting on the master outstanding at a time
		DWORD						m_dwRequestID; // ID of the master request currently outstanding
		HANDLE						m_hResponseEvent; // event to set once we get a response from the master
		HRESULT						m_hrResponseResult; // result code for phone number search returned by master
		PTNSYNCDATALIST				m_pSyncDataList; // pointer to user's list used to receive data
		char*						m_pszFoundPhoneNumber; // pointer to found phone number
		DWORD						m_dwFoundCOMPort; // COM port returned by master
		PTNOTHERMACHINEINFO			m_pReturnedInfo; // info returned by master
		HANDLE						m_hExecCaseExitEvent; // handle to event to set when ExecCase function returns
		BOOL						m_fComplete; // whether the test was complete or ongoing
		PTNPOKETESTCOMPLETEDPROC	m_pfnPokeTestCompleted; // pointer to user's callback to run when the test completes
		PVOID						m_pvUserContext; // user's context to pass to completion callback



		CTNTestInstanceS(DWORD dwUniqueID,
						PTNTESTTABLECASE pCase,
						char* pszInstanceID,
						int iNumMachines,
						PTNCTRLMACHINEID aTesters,
						//DWORD dwOptions,
						PTNTESTINSTANCES pParentTest);
		virtual ~CTNTestInstanceS(void);

		PTNTESTINSTANCES GetTopLevelTest(void);
		PTNTESTINSTANCES GetTest(DWORD dwUniqueID);
		PTNTESTINSTANCES GetTestWithRequestID(DWORD dwRequestID);
		int GetSlavesTesterNum(PTNCTRLMACHINEID pSlaveID);
		HRESULT SetInputData(PVOID pvData, DWORD dwDataSize);
		void FreeInputData(void);
};





#else //__TNCONTROL_TESTINSTANCE__
//#pragma message("__TNCONTROL_TESTINSTANCE__ already included!")
#endif //__TNCONTROL_TESTINSTANCE__
