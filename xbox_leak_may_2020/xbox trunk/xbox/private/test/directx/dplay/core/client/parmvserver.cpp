//==================================================================================
// Includes
//==================================================================================
#include "dptest.h"
#include "macros.h"
#include "parmvalidation.h"

using namespace DPlayCoreNamespace;

namespace DPlayCoreNamespace {

//==================================================================================
// Structures
//==================================================================================
typedef struct tagPARMVSGETSENDQINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVSGETSENDQINFOCONTEXT, * PPARMVSGETSENDQINFOCONTEXT;

typedef struct tagPARMVSGETAPPDESCCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVSGETAPPDESCCONTEXT, * PPARMVSGETAPPDESCCONTEXT;

typedef struct tagPARMVSSETSERVERINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	HANDLE					hAsyncOpCompletedEvent; // event to set when async op completes
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	BOOL					fCanGetServerInfoUpdate; // whether the server info indication can arrive or not
	BOOL					fGotServerInfoUpdate; // whether the server info indication did arrive
	HANDLE					hGotServerInfoUpdateEvent; // event to set when server info message is received
	PWRAPDP8CLIENT			pDP8Client; // pointer to DP8Client wrapper object to use
	WCHAR*					pwszExpectedServerInfoName; // expected client info name
	PVOID*					pvExpectedServerInfoData; // expected server info data
	DWORD					dwExpectedServerInfoDataSize; // expected server info data size
	DWORD					dwExpectedServerInfoPlayerFlags; // expected server info player flags
} PARMVSSETSERVERINFOCONTEXT, * PPARMVSSETSERVERINFOCONTEXT;

typedef struct tagPARMVSGETCLIENTINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVSGETCLIENTINFOCONTEXT, * PPARMVSGETCLIENTINFOCONTEXT;

typedef struct tagPARMVSGETCLIENTADDRESSCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVSGETCLIENTADDRESSCONTEXT, * PPARMVSGETCLIENTADDRESSCONTEXT;




//==================================================================================
// Prototypes
//==================================================================================
HRESULT ParmVServerExec_CreateQI(HANDLE hLog);
HRESULT ParmVServerExec_Init(HANDLE hLog);
HRESULT ParmVServerExec_EnumSPs(HANDLE hLog);
HRESULT ParmVServerExec_Cancel(HANDLE hLog);
HRESULT ParmVServerExec_GetSendQInfo(HANDLE hLog);
HRESULT ParmVServerExec_GetAppDesc(HANDLE hLog);
HRESULT ParmVServerExec_SetServerInfo(HANDLE hLog);
HRESULT ParmVServerExec_GetClientInfo(HANDLE hLog);
HRESULT ParmVServerExec_GetClientAddress(HANDLE hLog);
//HRESULT ParmVServerExec_GetLHostAddresses(HANDLE hLog);
//HRESULT ParmVServerExec_SetAppDesc(HANDLE hLog);
//HRESULT ParmVServerExec_Host(HANDLE hLog);
//HRESULT ParmVServerExec_SendTo(HANDLE hLog);
//HRESULT ParmVServerExec_CreateGroup(HANDLE hLog);
//HRESULT ParmVServerExec_DestroyGroup(HANDLE hLog);
//HRESULT ParmVServerExec_AddPlayerToGroup(HANDLE hLog);
//HRESULT ParmVServerExec_RemovePlayerFromGroup(HANDLE hLog);
//HRESULT ParmVServerExec_SetGroupInfo(HANDLE hLog);
//HRESULT ParmVServerExec_GetGroupInfo(HANDLE hLog);
//HRESULT ParmVServerExec_EnumPlayersAndGroups(HANDLE hLog);
//HRESULT ParmVServerExec_EnumGroupMembers(HANDLE hLog);
//HRESULT ParmVServerExec_Close(HANDLE hLog);
//HRESULT ParmVServerExec_DestroyClient(HANDLE hLog);
//HRESULT ParmVServerExec_Return(HANDLE hLog);
//HRESULT ParmVServerExec_GetPlayerContext(HANDLE hLog);
//HRESULT ParmVServerExec_GetGroupContext(HANDLE hLog);
//HRESULT ParmVServerExec_GetCaps(HANDLE hLog);
//HRESULT ParmVServerExec_SetCaps(HANDLE hLog);
//HRESULT ParmVServerExec_SetSPCaps(HANDLE hLog);
//HRESULT ParmVServerExec_GetSPCaps(HANDLE hLog);
//HRESULT ParmVServerExec_GetConnInfo(HANDLE hLog);
//HRESULT ParmVServerExec_RegLobby(HANDLE hLog);



HRESULT ParmVSGetSendQInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ParmVSGetAppDescDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ParmVSSetServerInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ParmVSGetClientInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ParmVSGetClientAddressDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);





/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerLoadTestTable()"
//==================================================================================
// ParmVServerLoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				2.1.2		Server interface parameter validation tests
//				2.1.2.1		Server DirectPlay8Create and QueryInterface parameter validation
//				2.1.2.2		Server Initialize parameter validation
//				2.1.2.3		Server EnumServiceProviders parameter validation
//				2.1.2.4		Server CancelAsyncOperation parameter validation
//				2.1.2.5		Server GetSendQueueInfo parameter validation
//				2.1.2.6		Server GetApplicationDesc parameter validation
//				2.1.2.7		Server SetServerInfo parameter validation
//				2.1.2.8		Server GetClientInfo parameter validation
//				2.1.2.9		Server GetClientAddress parameter validation
//
// Arguments:
//	PTNLOADTESTTABLEDATA pTNlttd	Pointer to data to use when loading the tests.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ParmVServerLoadTestTable(PTNLOADTESTTABLEDATA pTNlttd)
{
	PTNTESTTABLEGROUP	pSubGroup;
	TNADDTESTDATA		tnatd;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2	Server interface parameter validation tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pTNlttd->pBase,
				"2.1.2", "Server interface parameter validation tests",
				&pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.1	Server DirectPlay8Create and QueryInterface parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.1";
	tnatd.pszName			= "Server DirectPlay8Create and QueryInterface parameter validation";
	tnatd.pszDescription	= "Server DirectPlay8Create and QueryInterface parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_CreateQI;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.2	Server Initialize parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.2";
	tnatd.pszName			= "Server Initialize parameter validation";
	tnatd.pszDescription	= "Server Initialize parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_Init;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.3	Server EnumServiceProviders parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.3";
	tnatd.pszName			= "Server EnumServiceProviders parameter validation";
	tnatd.pszDescription	= "Server EnumServiceProviders parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_EnumSPs;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.4	Server CancelAsyncOperation parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.4";
	tnatd.pszName			= "Server CancelAsyncOperation parameter validation";
	tnatd.pszDescription	= "Server CancelAsyncOperation parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_Cancel;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.5	Server GetSendQueueInfo parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.5";
	tnatd.pszName			= "Server GetSendQueueInfo parameter validation";
	tnatd.pszDescription	= "Server GetSendQueueInfo parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_GetSendQInfo;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.6	Server GetApplication parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.6";
	tnatd.pszName			= "Server GetApplication parameter validation";
	tnatd.pszDescription	= "Server GetApplication parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_GetAppDesc;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.7	Server SetServerInfo parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.7";
	tnatd.pszName			= "Server SetServerInfo parameter validation";
	tnatd.pszDescription	= "Server SetServerInfo parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_SetServerInfo;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.8	Server GetClientInfo parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.8";
	tnatd.pszName			= "Server GetClientInfo parameter validation";
	tnatd.pszDescription	= "Server GetClientInfo parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_GetClientInfo;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.2.9	Server GetClientAddress parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.2.9";
	tnatd.pszName			= "Server GetClientAddress parameter validation";
	tnatd.pszDescription	= "Server GetClientAddress parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVServerExec_GetClientAddress;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);


	return (S_OK);
} // ParmVServerLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_CreateQI()"
//==================================================================================
// ParmVServerExec_CreateQI
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.1 - Server DirectPlay8Create and QueryInterface parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_CreateQI(HANDLE hLog)
{
	CTNSystemResult			sr;
	CTNTestResult			tr;
	PWRAPDP8SERVER			pDP8Server = NULL;


	BEGIN_TESTCASE
	{
		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("DirectPlay8Creating a DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = DirectPlay8Create(IID_IDirectPlay8Server, (LPVOID *) &pDP8Server->m_pDP8Server, NULL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "DirectPlay8Creating a DirectPlay8Server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't DirectPlay8Create)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;


		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	return (sr);
} // ParmVServerExec_CreateQI
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_Init()"
//==================================================================================
// ParmVServerExec_Init
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.2 - Server Initialize parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_Init(HANDLE hLog)
{
	CTNSystemResult		sr;
	CTNTestResult		tr;
	PWRAPDP8SERVER		pDP8Server = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with NULL context, handler, 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Server_Initialize(pDP8Server->m_pDP8Server, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Initializing with NULL context, handler, 0 flags using C++ macro didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with NULL context, handler, 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Initialize(NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Initializing with NULL context, handler, 0 flags didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with NULL context, invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Initialize(NULL, ParmVNoMessagesDPNMessageHandler, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Initializing with NULL context, invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Initialize(NULL, ParmVNoMessagesDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with 0 flags again");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Initialize(NULL, ParmVNoMessagesDPNMessageHandler, 0);
		if (tr != DPNERR_ALREADYINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Initializing with context, 0 flags again didn't return expected error ALREADYINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Re-initializing with 0 flags after init and close");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Initialize(NULL, ParmVNoMessagesDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Re-initializing with 0 flags after init and close failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVServerExec_Init
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_EnumSPs()"
//==================================================================================
// ParmVServerExec_EnumSPs
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.3 - Server EnumServiceProviders parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_EnumSPs(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8SERVER				pDP8Server = NULL;
	DWORD						dwSPBufferSize;
	DWORD						dwExpectedSPBufferSize;
	DWORD						dwNumSPs;
	DWORD						dwExpectedNumSPs;
	PDPN_SERVICE_PROVIDER_INFO	pdpnspi = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with all NULLs and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Server_EnumServiceProviders(pDP8Server->m_pDP8Server,
													NULL, NULL, NULL, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with all NULLs and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with all NULLs and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, NULL, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with all NULLs and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)
*/

		dwNumSPs = 0;

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with NULL GUIDs, NULL buffer, NULL size, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, NULL, NULL, &dwNumSPs, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with NULL GUIDs, NULL buffer, NULL size, and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

		if (dwNumSPs != 0)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 0)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with NULL GUIDs, NULL buffer, NULL count, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with NULL GUIDs, NULL buffer, NULL count, and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize != 0)
		{
			DPTEST_FAIL(hLog, "Buffer size modified (%u != 0)!", 1, dwSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size modified)
*/

		dwSPBufferSize = 0;
		dwNumSPs = 0;

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with NULL GUIDs, NULL buffer, and invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize,
												&dwNumSPs, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with NULL GUIDs, NULL buffer, and invalid flags didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize != 0)
		{
			DPTEST_FAIL(hLog, "Buffer size modified (%u != 0)!", 1, dwSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size modified)

		if (dwNumSPs != 0)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 0)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with NULL context, 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Initialize(NULL, ParmVNoMessagesDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing with NULL context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with NULL GUIDs, NULL buffer, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;
		// We expect to see at least the IP Service Provider.  We could technically
		// make sure there's room for the string, too, but that would have to be
		// localized.
		dwExpectedSPBufferSize = sizeof (DPN_SERVICE_PROVIDER_INFO);
		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize,
												&dwNumSPs, 0);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with NULL GUIDs, NULL buffer, and 0 flags didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize <= dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size smaller than expected (%u <= %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs < 1)
		{
			DPTEST_FAIL(hLog, "Number of SPs not expected (%u < 1)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (count not expected)



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with NULL GUIDs, NULL buffer, non-zero size, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 12345;
		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize,
												&dwNumSPs, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with NULL GUIDs, NULL buffer, non-zero size, and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize != 12345)
		{
			DPTEST_FAIL(hLog, "Buffer size modified (%u != 12345)!", 1, dwSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size modified)

		if (dwNumSPs != 0)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 0)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with NULL GUIDs, NULL buffer, non-zero count, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;
		//dwExpectedSPBufferSize = above;
		dwNumSPs = 12345;

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize,
												&dwNumSPs, 0);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with NULL GUIDs, NULL buffer, non-zero count, and 0 flags didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize <= dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size smaller than expected (%u <= %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs < 1)
		{
			DPTEST_FAIL(hLog, "Number of SPs not expected (%u < 1)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (count not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with NULL GUIDs, NULL buffer, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;
		//dwExpectedSPBufferSize = above;
		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize,
												&dwNumSPs, 0);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with NULL GUIDs, NULL buffer, and 0 flags didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize <= dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size smaller than expected (%u <= %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs < 1)
		{
			DPTEST_FAIL(hLog, "Number of SPs not expected (%u < 1)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (count not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating service providers with NULL GUIDs, 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PDPN_SERVICE_PROVIDER_INFO, pdpnspi,
							dwSPBufferSize + BUFFERPADDING_SIZE);

		FillWithDWord(((PBYTE) pdpnspi) + dwSPBufferSize, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);

		// Expect to get the same values again
		dwExpectedSPBufferSize = dwSPBufferSize;
		dwExpectedNumSPs = dwNumSPs;

		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(NULL, NULL, pdpnspi, &dwSPBufferSize,
												&dwNumSPs, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Enumerating service providers with NULL GUIDs, and 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize != dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size not expected (%u != %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs != dwExpectedNumSPs)
		{
			DPTEST_FAIL(hLog, "Number of SPs not expected (%u != %u)!",
				2, dwNumSPs, dwExpectedNumSPs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (count not expected)

#pragma TODO(vanceo, "Verify TCP/IP is in results")

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(((PBYTE) pdpnspi) + dwSPBufferSize, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pdpnspi);
		pdpnspi = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating invalid SP GUID devices");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;
		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(&GUID_UNKNOWN, NULL, NULL,
												&dwSPBufferSize, &dwNumSPs, 0);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Enumerating invalid SP GUID devices didn't return expected error DOESNOTEXIST!", 0);
				THROW_TESTRESULT;
			}
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize != 0)
		{
			DPTEST_FAIL(hLog, "Buffer size not expected (%u != 0)!", 1, dwSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs != 0)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 0)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating TCP/IP devices with invalid app GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		//dwExpectedSPBufferSize = ?;

		tr = pDP8Server->DP8S_EnumServiceProviders(&CLSID_DP8SP_TCPIP, &GUID_UNKNOWN,
												NULL, &dwSPBufferSize, &dwNumSPs, 0);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Enumerating TCP/IP devices with invalid app GUID didn't return expected error BUFFERTOOSMALL!", 0);
				THROW_TESTRESULT;
			}
		} // end if (couldn't enum service providers)

#pragma TODO(vanceo, "What do we expect?")
		/*
		if (dwSPBufferSize != dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size not expected (%u != %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs != 4)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 4)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating TCP/IP devices with NULL GUIDs, NULL buffer, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;
		//dwExpectedSPBufferSize = ?;
		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(&CLSID_DP8SP_TCPIP, NULL, NULL,
												&dwSPBufferSize, &dwNumSPs, 0);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Enumerating TCP/IP devices with NULL GUIDs, NULL buffer, and 0 flags didn't return expected error BUFFERTOOSMALL!", 0);
				THROW_TESTRESULT;
			}
		} // end if (couldn't enum service providers)

#pragma TODO(vanceo, "What do we expect?")
		/*
		if (dwSPBufferSize != dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size not expected (%u != %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs != 4)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 4)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating TCP/IP devices with NULL GUIDs, 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PDPN_SERVICE_PROVIDER_INFO, pdpnspi,
							dwSPBufferSize + BUFFERPADDING_SIZE);

		FillWithDWord(((PBYTE) pdpnspi) + dwSPBufferSize, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);

		// Expect to get the same values again
		dwExpectedSPBufferSize = dwSPBufferSize;
		dwExpectedNumSPs = dwNumSPs;

		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(&CLSID_DP8SP_TCPIP, NULL, pdpnspi,
												&dwSPBufferSize, &dwNumSPs, 0);
		if (tr != DPN_OK)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Enumerating TCP/IP devices with NULL GUIDs, and 0 flags failed!", 0);
				THROW_TESTRESULT;
			}
		} // end if (couldn't enum service providers)

		if (dwSPBufferSize != dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size not expected (%u != %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs != dwExpectedNumSPs)
		{
			DPTEST_FAIL(hLog, "Number of SPs not expected (%u != %u)!",
				2, dwNumSPs, dwExpectedNumSPs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (count not expected)

#pragma TODO(vanceo, "Verify results")

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(((PBYTE) pdpnspi) + dwSPBufferSize, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pdpnspi);
		pdpnspi = NULL;




/* XBOX - IPX not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating IPX devices with NULL GUIDs, NULL buffer, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;
		//dwExpectedSPBufferSize = ?;
		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(&CLSID_DP8SP_IPX, NULL, NULL,
													&dwSPBufferSize, &dwNumSPs, 0);

		// If IPX is not installed, then this will return UNSUPPORTED.
		if ((tr != DPNERR_BUFFERTOOSMALL) && (tr != DPNERR_UNSUPPORTED))
		{
			DPTEST_FAIL(hLog, "Enumerating IPX devices with NULL GUIDs, NULL buffer, and 0 flags didn't return expected error BUFFERTOOSMALL or UNSUPPORTED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

#pragma TODO(vanceo, "What do we expect?")
		if (dwSPBufferSize != dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size not expected (%u != %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs != 4)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 4)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)
*/



/* XBOX - MODEM not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating modem devices with NULL GUIDs, NULL buffer, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;
		//dwExpectedSPBufferSize = ?;
		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(&CLSID_DP8SP_MODEM, NULL, NULL,
													&dwSPBufferSize, &dwNumSPs, 0);

		// If there aren't any modems installed, then this will return UNSUPPORTED.
		if ((tr != DPNERR_BUFFERTOOSMALL) && (tr != DPNERR_UNSUPPORTED))
		{
			DPTEST_FAIL(hLog, "Enumerating modem devices with NULL GUIDs, NULL buffer, and 0 flags didn't return expected error BUFFERTOOSMALL or UNSUPPORTED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

#pragma TODO(vanceo, "What do we expect?")
		if (dwSPBufferSize != dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size not expected (%u != %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs != 4)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 4)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)
*/



/* XBOX - SERIAL not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerating serial devices with NULL GUIDs, NULL buffer, and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSPBufferSize = 0;
		//dwExpectedSPBufferSize = ?;
		dwNumSPs = 0;

		tr = pDP8Server->DP8S_EnumServiceProviders(&CLSID_DP8SP_SERIAL, NULL, NULL,
													&dwSPBufferSize, &dwNumSPs, 0);

		// If there aren't any modems installed, then this will return UNSUPPORTED.
		if ((tr != DPNERR_BUFFERTOOSMALL) && (tr != DPNERR_UNSUPPORTED))
		{
			DPTEST_FAIL(hLog, "Enumerating serial devices with NULL GUIDs, NULL buffer, and 0 flags didn't return expected error BUFFERTOOSMALL or UNSUPPORTED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum service providers)

#pragma TODO(vanceo, "What do we expect?")
		if (dwSPBufferSize != dwExpectedSPBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size not expected (%u != %u)!",
				2, dwSPBufferSize, dwExpectedSPBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size not expected)

		if (dwNumSPs != 4)
		{
			DPTEST_FAIL(hLog, "Number of SPs modified (%u != 4)!", 1, dwNumSPs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (count modified)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	SAFE_LOCALFREE(pdpnspi);


	return (sr);
} // ParmVServerExec_EnumSPs
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_Cancel()"
//==================================================================================
// ParmVServerExec_Cancel
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.4 - Server CancelAsyncOperation parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_Cancel(HANDLE hLog)
{
	CTNSystemResult			sr;
	CTNTestResult			tr;
	PWRAPDP8SERVER			pDP8Server = NULL;
	//PARMVSCANCELCONTEXT		context;
	PDIRECTPLAY8ADDRESS		pDP8AddressHost = NULL;
	PDIRECTPLAY8ADDRESS		pDP8AddressDevice = NULL;
	//DPN_APPLICATION_DESC	dpnad;
	HOSTENT *pHostent;
	WCHAR szLocalIPAddress[16];
	CHAR szLocalMacAddress[16], *szAnsiLocalIPAddress = NULL;


	//ZeroMemory(&context, sizeof (PARMVSCANCELCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating host DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressHost, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't create host DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (create failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting host DirectPlay8Address SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressHost->SetSP(&CLSID_DP8SP_TCPIP);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set host DirectPlay8Address SP!", 0);
			THROW_TESTRESULT;
		} // end if (set SP failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating host DirectPlay8Address for a device address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressHost->Duplicate(&pDP8AddressDevice);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't duplicate host DirectPlay8Address for a device address!", 0);
			THROW_TESTRESULT;
		} // end if (duplicate failed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting remote address hostname to local machine");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// In order to run these tests, need to get the local address information
		// Start by getting the Mac address
		if(gethostname(szLocalMacAddress, sizeof(szLocalMacAddress)))
		{
			xLog(hLog, XLL_FAIL, "Couldn't retrieve local Mac address");
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Then get the IP address
		pHostent = NULL;
		if((pHostent = gethostbyname(szLocalMacAddress)) == NULL)
		{
			xLog(hLog, XLL_FAIL, "Couldn't retrieve IP information for local host");
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		szAnsiLocalIPAddress = inet_ntoa(*((IN_ADDR *)pHostent->h_addr));

		// Extract the IP address from the hostent stucture
		mbstowcs(szLocalIPAddress, szAnsiLocalIPAddress, strlen(szAnsiLocalIPAddress));
		szLocalIPAddress[strlen(szAnsiLocalIPAddress)] = 0;

		tr = pDP8AddressHost->AddComponent(DPNA_KEY_HOSTNAME,
											szLocalIPAddress,
											(wcslen(szLocalIPAddress) + 1) * sizeof(WCHAR),
											DPNA_DATATYPE_STRING);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set remote address hostname to local machine!", 0);
			THROW_TESTRESULT;
		} // end if (add component failed)





/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Cancelling async operation with NULL and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Server_CancelAsyncOperation(pDP8Server->m_pDP8Server, NULL, 0);
		if (tr != DPNERR_INVALIDHANDLE)
		{
			DPTEST_FAIL(hLog, "Cancelling async operation with NULL and 0 flags using C++ macro didn't return expected error INVALIDHANDLE!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cancel async operation)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Cancelling async operation with NULL and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_CancelAsyncOperation(NULL, 0);
		if (tr != DPNERR_INVALIDHANDLE)
		{
			DPTEST_FAIL(hLog, "Cancelling async operation with NULL and 0 flags didn't return expected error INVALIDHANDLE!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cancel async operation)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with context, 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		//tr = pDP8Server->DP8S_Initialize(&context, ParmVSCancelDPNMessageHandler, 0);
		tr = pDP8Server->DP8S_Initialize(NULL, ParmVNoMessagesDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Cancelling invalid handle with 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_CancelAsyncOperation(0xABABABAB, 0);
		if (tr != DPNERR_INVALIDHANDLE)
		{
			DPTEST_FAIL(hLog, "Cancelling invalid handle with 0 flags didn't return expected error INVALIDHANDLE!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cancel async operation)



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Cancelling NULL handle with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_CancelAsyncOperation(NULL, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Cancelling NULL handle with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cancel async operation)
*/


#pragma TODO(vanceo, "Cancel send")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	SAFE_RELEASE(pDP8AddressHost);
	SAFE_RELEASE(pDP8AddressDevice);


	return (sr);
} // ParmVServerExec_Cancel
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_GetSendQInfo()"
//==================================================================================
// ParmVServerExec_GetSendQInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.5 - Server GetSendQueueInfo parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_GetSendQInfo(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8SERVER				pDP8Server = NULL;
	PWRAPDP8CLIENT				pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	DWORD						dwNumMsgs = 666;
	DWORD						dwNumBytes = 666;
	DPN_APPLICATION_DESC		dpnad;
	DPN_APPLICATION_DESC		dpnadCompare;
	PARMVSGETSENDQINFOCONTEXT	servercontext;
	PARMVSGETSENDQINFOCONTEXT	clientcontext;



	ZeroMemory(&servercontext, sizeof (PARMVSGETSENDQINFOCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVSGETSENDQINFOCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8Address, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't create DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (create failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info with all NULLs and 0s using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Server_GetSendQueueInfo(pDP8Server->m_pDP8Server, 0, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting send queue info with all NULLs and 0s using C++ macro didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info with all NULLs and 0s");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetSendQueueInfo(0, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting send queue info with all NULLs and 0s didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetSendQueueInfo(0, &dwNumMsgs, &dwNumBytes, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Getting send queue info with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 666)
		{
			DPTEST_FAIL(hLog, "Num msgs was changed (%u != 666)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was touched)

		if (dwNumBytes != 666)
		{
			DPTEST_FAIL(hLog, "Num bytes was changed (%u != 666)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was touched)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetSendQueueInfo(0, &dwNumMsgs, &dwNumBytes, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Getting send queue info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 666)
		{
			DPTEST_FAIL(hLog, "Num msgs was changed (%u != 666)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was touched)

		if (dwNumBytes != 666)
		{
			DPTEST_FAIL(hLog, "Num bytes was changed (%u != 666)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was touched)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing server object with host context");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		servercontext.hLog = hLog;

		tr = pDP8Server->DP8S_Initialize(&servercontext,
										ParmVSGetSendQInfoDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing server object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info without a connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetSendQueueInfo(0, &dwNumMsgs, &dwNumBytes, 0);
		if (tr != DPNERR_NOCONNECTION)
		{
			DPTEST_FAIL(hLog, "Getting send queue info without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 666)
		{
			DPTEST_FAIL(hLog, "Num msgs was changed (%u != 666)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was touched)

		if (dwNumBytes != 666)
		{
			DPTEST_FAIL(hLog, "Num bytes was changed (%u != 666)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Hosting session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_SERVER_GETSENDQINFO;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		//dpnad.pwszSessionName = NULL;
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));

		tr = pDP8Server->DP8S_Host(&dpnad,
									&pDP8Address,
									1,
									NULL,
									NULL,
									&(servercontext.dpnidServer),
									0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we got the create player message.
		if (servercontext.dpnidServer == 0)
		{
			DPTEST_FAIL(hLog, "Didn't get CREATE_PLAYER indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info for 0");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetSendQueueInfo(0, &dwNumMsgs, &dwNumBytes, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting send queue info for 0 didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 666)
		{
			DPTEST_FAIL(hLog, "Num msgs was changed (%u != 666)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was touched)

		if (dwNumBytes != 666)
		{
			DPTEST_FAIL(hLog, "Num bytes was changed (%u != 666)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was touched)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info for invalid player ID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetSendQueueInfo((DPNID) 0x666, &dwNumMsgs, &dwNumBytes, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting send queue info for invalid player ID didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 666)
		{
			DPTEST_FAIL(hLog, "Num msgs was changed (%u != 666)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was touched)

		if (dwNumBytes != 666)
		{
			DPTEST_FAIL(hLog, "Num bytes was changed (%u != 666)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was touched)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info for local player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetSendQueueInfo(servercontext.dpnidServer, &dwNumMsgs,
											&dwNumBytes, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting send queue info for local player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 666)
		{
			DPTEST_FAIL(hLog, "Num msgs was changed (%u != 666)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was touched)

		if (dwNumBytes != 666)
		{
			DPTEST_FAIL(hLog, "Num bytes was changed (%u != 666)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was touched)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating and connecting new client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,
							NULL, FALSE, FALSE, NULL);

		clientcontext.fConnectCanComplete = TRUE;

		clientcontext.hLog = hLog;

		tr = ParmVCreateAndConnectClient(hLog,
										ParmVSGetSendQInfoDPNMessageHandler,
										&clientcontext,
										pDP8Server,
										&dpnad,
										servercontext.hClientCreatedEvent,
										&pDP8Client);

		clientcontext.fConnectCanComplete = FALSE;

		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating and connecting client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create and connect client)

		CloseHandle(servercontext.hClientCreatedEvent);
		servercontext.hClientCreatedEvent = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info of client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetSendQueueInfo(servercontext.dpnidClient, &dwNumMsgs,
												&dwNumBytes, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting send queue info of client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 0)
		{
			DPTEST_FAIL(hLog, "Num msgs was not expected (%u != 0)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was unexpected)

		if (dwNumBytes != 0)
		{
			DPTEST_FAIL(hLog, "Num bytes was not expected (%u != 0)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was unexpected)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info of client with NULL num msgs");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification value.
		dwNumBytes = 666;

		tr = pDP8Server->DP8S_GetSendQueueInfo(servercontext.dpnidClient, NULL,
												&dwNumBytes, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting send queue info of client with NULL num msgs failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumBytes != 0)
		{
			DPTEST_FAIL(hLog, "Num bytes was not expected (%u != 0)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was unexpected)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info of client with NULL num bytes");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification value.
		dwNumMsgs = 666;

		tr = pDP8Server->DP8S_GetSendQueueInfo(servercontext.dpnidClient, &dwNumMsgs,
												NULL, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting send queue info of client with NULL num bytes failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 0)
		{
			DPTEST_FAIL(hLog, "Num msgs was not expected (%u != 0)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was unexpected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting low priority send queue info of client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification values.
		dwNumMsgs = 666;
		dwNumBytes = 666;

		tr = pDP8Server->DP8S_GetSendQueueInfo(servercontext.dpnidClient,
											&dwNumMsgs, &dwNumBytes,
											DPNGETSENDQUEUEINFO_PRIORITY_LOW);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting low priority send queue info of client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 0)
		{
			DPTEST_FAIL(hLog, "Num msgs was not expected (%u != 0)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was unexpected)

		if (dwNumBytes != 0)
		{
			DPTEST_FAIL(hLog, "Num bytes was not expected (%u != 0)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was unexpected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting normal priority send queue info of client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification values.
		dwNumMsgs = 666;
		dwNumBytes = 666;

		tr = pDP8Server->DP8S_GetSendQueueInfo(servercontext.dpnidClient,
											&dwNumMsgs, &dwNumBytes,
											DPNGETSENDQUEUEINFO_PRIORITY_NORMAL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting normal priority send queue info of client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 0)
		{
			DPTEST_FAIL(hLog, "Num msgs was not expected (%u != 0)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was unexpected)

		if (dwNumBytes != 0)
		{
			DPTEST_FAIL(hLog, "Num bytes was not expected (%u != 0)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was unexpected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting high priority send queue info of client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification values.
		dwNumMsgs = 666;
		dwNumBytes = 666;

		tr = pDP8Server->DP8S_GetSendQueueInfo(servercontext.dpnidClient,
											&dwNumMsgs, &dwNumBytes,
											DPNGETSENDQUEUEINFO_PRIORITY_HIGH);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting high priority send queue info of client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 0)
		{
			DPTEST_FAIL(hLog, "Num msgs was not expected (%u != 0)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was unexpected)

		if (dwNumBytes != 0)
		{
			DPTEST_FAIL(hLog, "Num bytes was not expected (%u != 0)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was unexpected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting all priority send queues' info of client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification values.
		dwNumMsgs = 666;
		dwNumBytes = 666;

		tr = pDP8Server->DP8S_GetSendQueueInfo(servercontext.dpnidClient,
											&dwNumMsgs, &dwNumBytes,
											(DPNGETSENDQUEUEINFO_PRIORITY_LOW | DPNGETSENDQUEUEINFO_PRIORITY_NORMAL | DPNGETSENDQUEUEINFO_PRIORITY_HIGH));
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting all priority send queues' info of client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 0)
		{
			DPTEST_FAIL(hLog, "Num msgs was not expected (%u != 0)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was unexpected)

		if (dwNumBytes != 0)
		{
			DPTEST_FAIL(hLog, "Num bytes was not expected (%u != 0)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was unexpected)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



		// Technically we should wait to make sure the host saw the client go away
		// as expected but that's more hassle than it's worth.  Just make sure he's
		// gone after Close() has returned.




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing hosting DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release hosting DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Client != NULL)
	{
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);


	return (sr);
} // ParmVServerExec_GetSendQInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_GetAppDesc()"
//==================================================================================
// ParmVServerExec_GetAppDesc
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.6 - Server GetApplication parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_GetAppDesc(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8SERVER				pDP8Server = NULL;
	PWRAPDP8CLIENT				pDP8Client = NULL;
	DPN_APPLICATION_DESC		dpnad;
	DPN_APPLICATION_DESC		dpnadCompare;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PDIRECTPLAY8ADDRESS			pDP8AddressCompare = NULL;
	PDPN_APPLICATION_DESC		pdpnad = NULL;
	DWORD						dwSize = 0;
	DWORD						dwExpectedSize;
	PARMVSGETAPPDESCCONTEXT		servercontext;
	PARMVSGETAPPDESCCONTEXT		clientcontext;



	ZeroMemory(&servercontext, sizeof (PARMVSGETAPPDESCCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVSGETAPPDESCCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8Address, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't create DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (create failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc with NULLs and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Server_GetApplicationDesc(pDP8Server->m_pDP8Server, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting app desc with NULLs and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc with NULLs and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetApplicationDesc(NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting app desc with NULLs and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc with NULL buffer and invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetApplicationDesc(NULL, &dwSize, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Getting app desc with NULL buffer and invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc with NULL buffer before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetApplicationDesc(NULL, &dwSize, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Getting app desc with NULL buffer before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		servercontext.hLog = hLog;

		tr = pDP8Server->DP8S_Initialize(&servercontext,
										ParmVSGetAppDescDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc with NULL buffer without a connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetApplicationDesc(NULL, &dwSize, 0);
		if (tr != DPNERR_NOCONNECTION)
		{
			DPTEST_FAIL(hLog, "Getting app desc with NULL buffer without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Saving device address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Duplicate(&pDP8AddressCompare);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't duplicate device address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't duplicate device address object)

		

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Hosting session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_SERVER_GETAPPDESC;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		//dpnad.pwszSessionName = NULL;
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));

		tr = pDP8Server->DP8S_Host(&dpnad,
									&pDP8Address,
									1,
									NULL,
									NULL,
									&(servercontext.dpnidServer),
									0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		tr = pDP8Address->IsEqual(pDP8AddressCompare);
		if (tr != DPNSUCCESS_EQUAL)
		{
			DPTEST_FAIL(hLog, "Device address object was modified (0x%08x != 0x%08x)!",
				2, pDP8Address, pDP8AddressCompare);
			THROW_TESTRESULT;
		} // end if (addresses aren't equal)

		// Make sure we got the create player message.
		if (servercontext.dpnidServer == 0)
		{
			DPTEST_FAIL(hLog, "Didn't get CREATE_PLAYER indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwExpectedSize = sizeof (DPN_APPLICATION_DESC);

		tr = pDP8Server->DP8S_GetApplicationDesc(NULL, &dwSize, 0);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting app desc with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		LOCALALLOC_OR_THROW(PDPN_APPLICATION_DESC, pdpnad,
							dwSize + BUFFERPADDING_SIZE);

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc using buffer with app desc size of 0");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		FillWithDWord((((PBYTE) pdpnad) + sizeof (DWORD)),
						(dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE),
						DONT_TOUCH_MEMORY_PATTERN);

		pdpnad->dwSize = 0;


		tr = pDP8Server->DP8S_GetApplicationDesc(pdpnad, &dwSize, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting app desc using buffer with app desc size of 0 didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		// Make sure the buffer was not modified or overrun.
		if ((pdpnad->dwSize != 0) ||
			(! IsFilledWithDWord((((PBYTE) pdpnad) + sizeof (DWORD)), (dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE), DONT_TOUCH_MEMORY_PATTERN)))
		{
			DPTEST_FAIL(hLog, "The %u byte app desc buffer (at %x) was modified or overrun!",
				2, dwSize, pdpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc using buffer with app desc size too large");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		FillWithDWord((((PBYTE) pdpnad) + sizeof (DWORD)),
						(dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE),
						DONT_TOUCH_MEMORY_PATTERN);

		pdpnad->dwSize = sizeof (DPN_APPLICATION_DESC) + 1;


		tr = pDP8Server->DP8S_GetApplicationDesc(pdpnad, &dwSize, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting app desc using buffer with app desc size too large didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		// Make sure the buffer was not modified or overrun.
		if ((pdpnad->dwSize != (sizeof (DPN_APPLICATION_DESC) + 1)) ||
			(! IsFilledWithDWord((((PBYTE) pdpnad) + sizeof (DWORD)), (dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE), DONT_TOUCH_MEMORY_PATTERN)))
		{
			DPTEST_FAIL(hLog, "The %u byte app desc buffer (at %x) was modified or overrun!",
				2, dwSize, pdpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpnad->dwSize = sizeof (DPN_APPLICATION_DESC);

		dpnad.dwCurrentPlayers = 1; // there will be one player when the app desc is retrieved


		FillWithDWord(((PBYTE) pdpnad) + dwSize, BUFFERPADDING_SIZE,
						DONT_TOUCH_MEMORY_PATTERN);

		tr = pDP8Server->DP8S_GetApplicationDesc(pdpnad, &dwSize, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting app desc failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		// Make sure the structure returned was expected.
		tr = ParmVCompareAppDesc(hLog, pdpnad, &dpnad);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Comparing application desc buffer %x with expected %x failed!",
				2, pdpnad, &dpnad);
			THROW_TESTRESULT;
		} // end if (failed comparison)


		// Check for buffer overrun.
		if (! IsFilledWithDWord(((PBYTE) pdpnad) + dwSize, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "The %u byte app desc buffer (at %x) was overrun!",
				2, dwSize, pdpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc with extra buffer size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		FillWithDWord(((PBYTE) pdpnad) + dwSize, BUFFERPADDING_SIZE,
						DONT_TOUCH_MEMORY_PATTERN);

		dwSize += BUFFERPADDING_SIZE;

		tr = pDP8Server->DP8S_GetApplicationDesc(pdpnad, &dwSize, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting app desc failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		// Make sure the structure returned was expected.
		tr = ParmVCompareAppDesc(hLog, pdpnad, &dpnad);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Comparing application desc buffer %x with expected %x failed!",
				2, pdpnad, &dpnad);
			THROW_TESTRESULT;
		} // end if (failed comparison)


		// Check for buffer overrun.
		if (! IsFilledWithDWord(((PBYTE) pdpnad) + dwSize, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "The %u byte app desc buffer (at %x) was overrun!",
				2, dwSize, pdpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating and connecting new client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,
							NULL, FALSE, FALSE, NULL);

		clientcontext.fConnectCanComplete = TRUE;

		clientcontext.hLog = hLog;

		tr = ParmVCreateAndConnectClient(hLog,
										ParmVSGetAppDescDPNMessageHandler,
										&clientcontext,
										pDP8Server,
										&dpnad,
										servercontext.hClientCreatedEvent,
										&pDP8Client);

		clientcontext.fConnectCanComplete = FALSE;

		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating and connecting client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create and connect client)

		CloseHandle(servercontext.hClientCreatedEvent);
		servercontext.hClientCreatedEvent = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting app desc again");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpnad->dwSize = sizeof (DPN_APPLICATION_DESC);

		dpnad.dwCurrentPlayers = 2; // there will be two players when the app desc is retrieved


		FillWithDWord(((PBYTE) pdpnad) + dwSize, BUFFERPADDING_SIZE,
						DONT_TOUCH_MEMORY_PATTERN);

		tr = pDP8Server->DP8S_GetApplicationDesc(pdpnad, &dwSize, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting app desc again failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		// Make sure the structure returned was expected.
		tr = ParmVCompareAppDesc(hLog, pdpnad, &dpnad);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Comparing application desc buffer %x with expected %x failed!",
				2, pdpnad, &dpnad);
			THROW_TESTRESULT;
		} // end if (failed comparison)


		// Check for buffer overrun.
		if (! IsFilledWithDWord(((PBYTE) pdpnad) + dwSize, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "The %u byte app desc buffer (at %x) was overrun!",
				2, dwSize, pdpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



		// Technically we should wait to make sure the host saw the client go away
		// as expected but that's more hassle than it's worth.  Just make sure he's
		// gone after Close() has returned.




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing hosting DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release hosting DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Client != NULL)
	{
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	SAFE_LOCALFREE(pdpnad);
	SAFE_RELEASE(pDP8Address);
	SAFE_RELEASE(pDP8AddressCompare);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);


	return (sr);
} // ParmVServerExec_GetAppDesc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_SetServerInfo()"
//==================================================================================
// ParmVServerExec_SetServerInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.7 - Server SetServerInfo parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_SetServerInfo(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PWRAPDP8SERVER				pDP8Server = NULL;
	PARMVSSETSERVERINFOCONTEXT	servercontext;
	PARMVSSETSERVERINFOCONTEXT	clientcontext;
	DPN_APPLICATION_DESC		dpnad;
	DPN_APPLICATION_DESC		dpnadCompare;
	DPN_PLAYER_INFO				dpnpi;
	DPN_PLAYER_INFO				dpnpiCompare;
	DPNHANDLE					dpnhSetServerInfo;
	DP_DOWORKLIST				DoWorkList;

	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));

	ZeroMemory(&servercontext, sizeof (PARMVSSETSERVERINFOCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVSSETSERVERINFOCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8Address, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't create DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (create failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info with all NULLs and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Server_SetServerInfo(pDP8Server->m_pDP8Server,
										NULL, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting server info with all NULLs and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info with all NULLs and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_SetServerInfo(NULL, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting server info with all NULLs and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)
*/

		dpnhSetServerInfo = (DPNHANDLE) 0x666;

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info with NULL player info structure");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_SetServerInfo(NULL, NULL, &dpnhSetServerInfo, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting server info with NULL player info structure didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

		if (dpnhSetServerInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetServerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)
*/

		ZeroMemory(&dpnpi, sizeof (DPN_PLAYER_INFO));
		dpnpi.dwSize = sizeof (DPN_PLAYER_INFO);
		//dpnpi.dwInfoFlags = 0;
		//dpnpi.pwszName = NULL;
		//dpnpi.pvData = NULL;
		//dpnpi.dwDataSize = 0;
		//dpnpi.dwPlayerFlags = 0;

		CopyMemory(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO));

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info with NULL async op handle");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Setting server info with NULL async op handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, &dpnhSetServerInfo, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Setting server info with invalid flags structure didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetServerInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetServerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)
*/



/* XBOX - SYNC operations not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info synchronously with async op handle");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, &dpnhSetServerInfo,
											DPNSETPEERINFO_SYNC);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Setting server info synchronously with async op handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetServerInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetServerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, &dpnhSetServerInfo, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Setting server info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetServerInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetServerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		servercontext.hLog = hLog;

		tr = pDP8Server->DP8S_Initialize(&servercontext,
										ParmVSSetServerInfoDPNMessageHandler,
										0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info prior to connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, &dpnhSetServerInfo, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting server info prior to connection failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetServerInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetServerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Hosting session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_SERVER_SETSERVERINFO;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		//dpnad.pwszSessionName = NULL;
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));

		tr = pDP8Server->DP8S_Host(&dpnad,
									&pDP8Address,
									1,
									NULL,
									NULL,
									&(servercontext.dpnidServer),
									0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		// Make sure we got the create player message.
		if (servercontext.dpnidServer == 0)
		{
			DPTEST_FAIL(hLog, "Didn't get CREATE_PLAYER indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "No local message")
		/*
		// Expect the server info message and async op completion.
		servercontext.fCanGetServerInfoUpdate = TRUE;
		//servercontext.pwszExpectedServerInfoName = NULL;
		//servercontext.pvExpectedServerInfoData = NULL;
		//servercontext.dwExpectedServerInfoDataSize = 0;
		servercontext.dwExpectedServerInfoPlayerFlags = DPNPLAYER_LOCAL | DPNPLAYER_HOST;
		*/
		CREATEEVENT_OR_THROW(servercontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		servercontext.fAsyncOpCanComplete = TRUE;


		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, &dpnhSetServerInfo, 0);
		if (tr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPTEST_FAIL(hLog, "Setting server info didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetServerInfo == (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSetServerInfo == NULL)
		{
			DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for async op to complete");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hAsyncOpCompletedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

#pragma BUGBUG(vanceo, "No local message")
		// The server info message should have arrived by now.
		//servercontext.fCanGetServerInfoUpdate = FALSE;

		// Prevent any weird async op completions.
		servercontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! servercontext.fAsyncOpCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (servercontext.dpnhCompletedAsyncOp != dpnhSetServerInfo)
		{
			DPTEST_FAIL(hLog, "Set server info completed with different async op handle (%x != %x)!",
				2, servercontext.dpnhCompletedAsyncOp, dpnhSetServerInfo);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		servercontext.fAsyncOpCompleted = FALSE;
		CloseHandle(servercontext.hAsyncOpCompletedEvent);
		servercontext.hAsyncOpCompletedEvent = NULL;


#pragma BUGBUG(vanceo, "No local message")
		/*
		// Make sure we got the server info message.
		if (! servercontext.fGotServerInfoUpdate)
		{
			DPTEST_FAIL(hLog, "Didn't get server info message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get server info)

		// Reset the context.
		servercontext.fGotServerInfoUpdate = FALSE;
		*/





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating and connecting new client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,
							NULL, FALSE, FALSE, NULL);

		clientcontext.fConnectCanComplete = TRUE;

		clientcontext.hLog = hLog;

		tr = ParmVCreateAndConnectClient(hLog,
										ParmVSSetServerInfoDPNMessageHandler,
										&clientcontext,
										pDP8Server,
										&dpnad,
										servercontext.hClientCreatedEvent,
										&(clientcontext.pDP8Client));

		clientcontext.fConnectCanComplete = FALSE;

		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating and connecting client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create and connect client)

		CloseHandle(servercontext.hClientCreatedEvent);
		servercontext.hClientCreatedEvent = NULL;






/* XBOX - SYNC operations not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info synchronously");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "No local message")
		// Expect the server info message on the server.
		servercontext.fCanGetServerInfoUpdate = TRUE;
		//servercontext.pwszExpectedServerInfoName = NULL;
		//servercontext.pvExpectedServerInfoData = NULL;
		//servercontext.dwExpectedServerInfoDataSize = 0;
		//servercontext.dwExpectedServerInfoPlayerFlags = DPNPLAYER_LOCAL | DPNPLAYER_HOST;

		// Expect the server info message on the client.
		clientcontext.fCanGetServerInfoUpdate = TRUE;
		//clientcontext.pwszExpectedServerInfoName = NULL;
		//clientcontext.pvExpectedServerInfoData = NULL;
		//clientcontext.dwExpectedServerInfoDataSize = 0;
		clientcontext.dwExpectedServerInfoPlayerFlags = DPNPLAYER_HOST;
		CREATEEVENT_OR_THROW(clientcontext.hGotServerInfoUpdateEvent,
							NULL, FALSE, FALSE, NULL);

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, NULL, DPNSETSERVERINFO_SYNC);

#pragma BUGBUG(vanceo, "No local message")
		// The server info message should have arrived by now.
		//servercontext.fCanGetServerInfoUpdate = FALSE;

		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting server info synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

#pragma BUGBUG(vanceo, "No local message")
		// Make sure we got the server info message.
		if (! servercontext.fGotServerInfoUpdate)
		{
			DPTEST_FAIL(hLog, "Didn't get server info message on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get server info)

		// Reset the context.
		servercontext.fGotServerInfoUpdate = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for server info to be indicated on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = clientcontext.pDP8Client;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 240000, clientcontext.hGotServerInfoUpdateEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		// Prevent any weird server info indications.
		clientcontext.fCanGetServerInfoUpdate = FALSE;

#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! clientcontext.fGotServerInfoUpdate)
		{
			DPTEST_FAIL(hLog, "Didn't get server info indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get server info indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(clientcontext.hGotServerInfoUpdateEvent);
		clientcontext.hGotServerInfoUpdateEvent = NULL;
		clientcontext.fGotServerInfoUpdate = FALSE;
*/





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting server info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhSetServerInfo = (DPNHANDLE) 0x666;

#pragma BUGBUG(vanceo, "No local message")
		/*
		// Expect the server info message and async op completion on the server.
		servercontext.fCanGetServerInfoUpdate = TRUE;
		//servercontext.pwszExpectedServerInfoName = NULL;
		//servercontext.pvExpectedServerInfoData = NULL;
		//servercontext.dwExpectedServerInfoDataSize = 0;
		//servercontext.dwExpectedServerInfoPlayerFlags = DPNPLAYER_LOCAL | DPNPLAYER_HOST;
		*/
		CREATEEVENT_OR_THROW(servercontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		servercontext.fAsyncOpCanComplete = TRUE;

		// Expect the server info message on the client.
		clientcontext.fCanGetServerInfoUpdate = TRUE;
		CREATEEVENT_OR_THROW(clientcontext.hGotServerInfoUpdateEvent,
							NULL, FALSE, FALSE, NULL);
		//clientcontext.pwszExpectedServerInfoName = NULL;
		//clientcontext.pvExpectedServerInfoData = NULL;
		//clientcontext.dwExpectedServerInfoDataSize = 0;
		//clientcontext.dwExpectedServerInfoPlayerFlags = DPNPLAYER_HOST;

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, &dpnhSetServerInfo, 0);
		if (tr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPTEST_FAIL(hLog, "Setting server info didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set server info)

		if (dpnhSetServerInfo == (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSetServerInfo == NULL)
		{
			DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for async op to complete on server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = clientcontext.pDP8Client->m_pDP8Client;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hAsyncOpCompletedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

#pragma BUGBUG(vanceo, "No local message")
		// The server info message should have arrived by now.
		//servercontext.fCanGetServerInfoUpdate = FALSE;

		// Prevent any weird async op completions.
		servercontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! servercontext.fAsyncOpCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (servercontext.dpnhCompletedAsyncOp != dpnhSetServerInfo)
		{
			DPTEST_FAIL(hLog, "Set server info completed with different async op handle (%x != %x)!",
				2, servercontext.dpnhCompletedAsyncOp, dpnhSetServerInfo);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		servercontext.fAsyncOpCompleted = FALSE;
		CloseHandle(servercontext.hAsyncOpCompletedEvent);
		servercontext.hAsyncOpCompletedEvent = NULL;


#pragma BUGBUG(vanceo, "No local message")
		/*
		// Make sure we got the server info message.
		if (! servercontext.fGotServerInfoUpdate)
		{
			DPTEST_FAIL(hLog, "Didn't get server info message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get server info)

		// Reset the context.
		servercontext.fGotServerInfoUpdate = FALSE;
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for server info to be indicated on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = clientcontext.pDP8Client->m_pDP8Client;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, clientcontext.hGotServerInfoUpdateEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		// Prevent any weird server info indications.
		clientcontext.fCanGetServerInfoUpdate = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! clientcontext.fGotServerInfoUpdate)
		{
			DPTEST_FAIL(hLog, "Didn't get server info indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get server info indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(clientcontext.hGotServerInfoUpdateEvent);
		clientcontext.hGotServerInfoUpdateEvent = NULL;
		clientcontext.fGotServerInfoUpdate = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = clientcontext.pDP8Client->DP8C_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



		// Technically we should wait to make sure the host saw the client go away
		// as expected but that's more hassle than it's worth.  Just make sure he's
		// gone after Close() has returned.




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing hosting DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = clientcontext.pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release hosting DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (clientcontext.pDP8Client);
		clientcontext.pDP8Client = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (clientcontext.pDP8Client != NULL)
	{
		delete (clientcontext.pDP8Client);
		clientcontext.pDP8Client = NULL;
	} // end if (have client object)

	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(servercontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(clientcontext.hGotServerInfoUpdateEvent);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);
	ClearDoWorkList(&DoWorkList);

	return (sr);
} // ParmVServerExec_SetServerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_GetClientInfo()"
//==================================================================================
// ParmVServerExec_GetClientInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.8 - Server SetClientInfo parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_GetClientInfo(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8SERVER				pDP8Server = NULL;
	PWRAPDP8CLIENT				pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PARMVSGETCLIENTINFOCONTEXT	servercontext;
	PARMVSGETCLIENTINFOCONTEXT	clientcontext;
	DPN_APPLICATION_DESC		dpnad;
	DPN_APPLICATION_DESC		dpnadCompare;
	PDPN_PLAYER_INFO			pdpnpi = NULL;
	DPN_PLAYER_INFO				dpnpi;
	DWORD						dwSize;
	DWORD						dwExpectedSize;



	ZeroMemory(&servercontext, sizeof (PARMVSGETCLIENTINFOCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVSGETCLIENTINFOCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8Address, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't create DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (create failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client 0 info with NULLs and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Server_GetClientInfo(pDP8Server->m_pDP8Server, 0, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting client 0 info with NULLs and 0 flags using C++ macro didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client 0 info with NULLs and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientInfo(0, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting client 0 info with NULLs and 0 flags didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)
*/




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info with NULL buffer and non-zero size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSize = 666;

		// Use 0x666 to avoid DPlay's player ID == 0 check.
		tr = pDP8Server->DP8S_GetClientInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting client info with NULL buffer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != 666)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 666)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size was touched)
*/

		dwSize = 0;

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientInfo((DPNID) 0x666, NULL, &dwSize, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Getting client info with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Getting client info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		servercontext.hLog = hLog;

		tr = pDP8Server->DP8S_Initialize(&servercontext,
										ParmVSGetClientInfoDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info prior to connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (tr != DPNERR_NOCONNECTION)
		{
			DPTEST_FAIL(hLog, "Getting client info prior to connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Hosting session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_SERVER_GETCLIENTINFO;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		//dpnad.pwszSessionName = NULL;
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));

		tr = pDP8Server->DP8S_Host(&dpnad,
									&pDP8Address,
									1,
									NULL,
									NULL,
									&(servercontext.dpnidServer),
									0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		// Make sure we got the create player message.
		if (servercontext.dpnidServer == 0)
		{
			DPTEST_FAIL(hLog, "Didn't get CREATE_PLAYER indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info for invalid player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting client info for invalid player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info for local player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientInfo(servercontext.dpnidServer, NULL, &dwSize, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting client info for local player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating and connecting new client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,
							NULL, FALSE, FALSE, NULL);

		clientcontext.fConnectCanComplete = TRUE;

		clientcontext.hLog = hLog;

		tr = ParmVCreateAndConnectClient(hLog,
										ParmVSGetClientInfoDPNMessageHandler,
										&clientcontext,
										pDP8Server,
										&dpnad,
										servercontext.hClientCreatedEvent,
										&pDP8Client);

		clientcontext.fConnectCanComplete = FALSE;

		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating and connecting client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create and connect client)

		CloseHandle(servercontext.hClientCreatedEvent);
		servercontext.hClientCreatedEvent = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwExpectedSize = sizeof (DPN_PLAYER_INFO);

		tr = pDP8Server->DP8S_GetClientInfo(servercontext.dpnidClient, NULL, &dwSize, 0);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting client info with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size was not expected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		LOCALALLOC_OR_THROW(PDPN_PLAYER_INFO, pdpnpi, dwSize + BUFFERPADDING_SIZE);

		FillWithDWord((((PBYTE) pdpnpi) + sizeof (DWORD)),
						(dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE),
						DONT_TOUCH_MEMORY_PATTERN);

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info with player info size of 0");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpnpi->dwSize = 0;


		tr = pDP8Server->DP8S_GetClientInfo(servercontext.dpnidClient, pdpnpi, &dwSize, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting client info using buffer with player info size of 0 didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		// Make sure the buffer was not modified or overrun.
		if ((pdpnpi->dwSize != 0) ||
			(! IsFilledWithDWord((((PBYTE) pdpnpi) + sizeof (DWORD)), (dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE), DONT_TOUCH_MEMORY_PATTERN)))
		{
			DPTEST_FAIL(hLog, "The %u byte player info buffer (at %x) was modified or overrun!",
				2, dwSize, pdpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info with player info size too large");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO) + 1;

		tr = pDP8Server->DP8S_GetClientInfo(servercontext.dpnidClient, pdpnpi, &dwSize, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting client info using buffer with player info size too large didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		// Make sure the buffer was not modified or overrun.
		if ((pdpnpi->dwSize != (sizeof (DPN_PLAYER_INFO) + 1)) ||
			(! IsFilledWithDWord((((PBYTE) pdpnpi) + sizeof (DWORD)), (dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE), DONT_TOUCH_MEMORY_PATTERN)))
		{
			DPTEST_FAIL(hLog, "The %u byte player info buffer (at %x) was modified or overrun!",
				2, dwSize, pdpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)
*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO);

		tr = pDP8Server->DP8S_GetClientInfo(servercontext.dpnidClient, pdpnpi, &dwSize, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting client info failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		// Make sure the structure returned was expected.

		ZeroMemory(&dpnpi, sizeof (DPN_PLAYER_INFO));
		dpnpi.dwSize = sizeof (DPN_PLAYER_INFO);
		dpnpi.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
		//dpnpi.pwszName = NULL;
		//dpnpi.pvData = NULL;
		//dpnpi.dwDataSize = 0;
		//dpnpi.dwPlayerFlags = 0;

		tr = ParmVComparePlayerInfo(hLog, pdpnpi, &dpnpi);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Comparing player info buffer %x with expected %x failed!",
				2, pdpnpi, &dpnpi);
			THROW_TESTRESULT;
		} // end if (failed comparison)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



		// Technically we should wait to make sure the host saw the client go away
		// as expected but that's more hassle than it's worth.  Just make sure he's
		// gone after Close() has returned.




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing hosting DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release hosting DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Client != NULL)
	{
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	SAFE_LOCALFREE(pdpnpi);
	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);


	return (sr);
} // ParmVServerExec_GetClientInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVServerExec_GetClientAddress()"
//==================================================================================
// ParmVServerExec_GetClientAddress
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.2.9 - Server GetClientAddress parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVServerExec_GetClientAddress(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PWRAPDP8SERVER					pDP8Server = NULL;
	PWRAPDP8CLIENT					pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS				pDP8HostAddress = NULL;
	PDIRECTPLAY8ADDRESS				pDP8HostAddressCompare = NULL;
	PDIRECTPLAY8ADDRESS				pDP8ReturnedAddress = NULL;
	PARMVSGETCLIENTADDRESSCONTEXT	servercontext;
	PARMVSGETCLIENTADDRESSCONTEXT	clientcontext;
	DPN_APPLICATION_DESC			dpnad;
	DPN_APPLICATION_DESC			dpnadCompare;



	ZeroMemory(&servercontext, sizeof (PARMVSGETCLIENTADDRESSCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVSGETCLIENTADDRESSCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8HostAddress, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't create DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (create failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8HostAddress->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client 0 address with NULL and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Server_GetClientAddress(pDP8Server->m_pDP8Server, 0, NULL, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting client 0 address with NULL and 0 flags using C++ macro didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client 0 address with NULL and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientAddress(0, NULL, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting client 0 address with NULL and 0 flags didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)
*/




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client address with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Use 0x666 to avoid DPlay's player ID == 0 check.
		tr = pDP8Server->DP8S_GetClientAddress((DPNID) 0x666, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting client address with NULL buffer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)
*/




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client address with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientAddress((DPNID) 0x666, &pDP8ReturnedAddress, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Getting client address with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)

		if (pDP8ReturnedAddress != NULL)
		{
			DPTEST_FAIL(hLog, "Address was changed (%u != NULL)!", 1, pDP8ReturnedAddress);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (address is not NULL)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client address before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientAddress((DPNID) 0x666, &pDP8ReturnedAddress, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Getting client address before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)

		if (pDP8ReturnedAddress != NULL)
		{
			DPTEST_FAIL(hLog, "Address was changed (%u != NULL)!", 1, pDP8ReturnedAddress);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (address is not NULL)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		servercontext.hLog = hLog;

		tr = pDP8Server->DP8S_Initialize(&servercontext,
										ParmVSGetClientAddressDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client address prior to connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientAddress((DPNID) 0x666, &pDP8ReturnedAddress, 0);
		if (tr != DPNERR_NOCONNECTION)
		{
			DPTEST_FAIL(hLog, "Getting client address prior to connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)

		if (pDP8ReturnedAddress != NULL)
		{
			DPTEST_FAIL(hLog, "Address was changed (%u != NULL)!", 1, pDP8ReturnedAddress);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (address is not NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Saving device address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8HostAddress->Duplicate(&pDP8HostAddressCompare);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't duplicate device address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't duplicate device address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Hosting session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_SERVER_GETCLIENTADDRESS;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		//dpnad.pwszSessionName = NULL;
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));

		tr = pDP8Server->DP8S_Host(&dpnad,
									&pDP8HostAddress,
									1,
									NULL,
									NULL,
									&(servercontext.dpnidServer),
									0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		tr = pDP8HostAddress->IsEqual(pDP8HostAddressCompare);
		if (tr != DPNSUCCESS_EQUAL)
		{
			DPTEST_FAIL(hLog, "Device address object was modified (0x%08x != 0x%08x)!",
				2, pDP8HostAddress, pDP8HostAddressCompare);
			THROW_TESTRESULT;
		} // end if (addresses aren't equal)

		// Make sure we got the create player message.
		if (servercontext.dpnidServer == 0)
		{
			DPTEST_FAIL(hLog, "Didn't get CREATE_PLAYER indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client address for invalid player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientAddress((DPNID) 0x666, &pDP8ReturnedAddress, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting client address for invalid player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)

		if (pDP8ReturnedAddress != NULL)
		{
			DPTEST_FAIL(hLog, "Address was changed (%u != NULL)!", 1, pDP8ReturnedAddress);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (address is not NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting local server's address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientAddress(servercontext.dpnidServer,
												&pDP8ReturnedAddress, 0);
		if (tr != DPNERR_INVALIDPLAYER)
		{
			DPTEST_FAIL(hLog, "Getting local server's address didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)

		if (pDP8ReturnedAddress != NULL)
		{
			DPTEST_FAIL(hLog, "Address was changed (%u != NULL)!", 1, pDP8ReturnedAddress);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (address is not NULL)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating and connecting new client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,
							NULL, FALSE, FALSE, NULL);

		clientcontext.fConnectCanComplete = TRUE;

		clientcontext.hLog = hLog;

		tr = ParmVCreateAndConnectClient(hLog,
										ParmVSGetClientAddressDPNMessageHandler,
										&clientcontext,
										pDP8Server,
										&dpnad,
										servercontext.hClientCreatedEvent,
										&pDP8Client);

		clientcontext.fConnectCanComplete = FALSE;

		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating and connecting client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create and connect client)

		CloseHandle(servercontext.hClientCreatedEvent);
		servercontext.hClientCreatedEvent = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting client's address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_GetClientAddress(servercontext.dpnidClient,
												&pDP8ReturnedAddress, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting client's address failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client address)

		if (pDP8ReturnedAddress == NULL)
		{
			DPTEST_FAIL(hLog, "No address was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (no address returned)



#pragma TODO(vanceo, "Validate the address")
		DPTEST_TRACE(hLog, "Client's address = 0x%08x", 1, pDP8ReturnedAddress);





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



		// Technically we should wait to make sure the host saw the client go away
		// as expected but that's more hassle than it's worth.  Just make sure he's
		// gone after Close() has returned.




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing hosting DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release hosting DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Server);
		pDP8Server = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Client != NULL)
	{
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	SAFE_RELEASE(pDP8HostAddress);
	SAFE_RELEASE(pDP8HostAddressCompare);
	SAFE_RELEASE(pDP8ReturnedAddress);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);

	return (sr);
} // ParmVServerExec_GetClientAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVSGetSendQInfoDPNMessageHandler()"
//==================================================================================
// ParmVSGetSendQInfoDPNMessageHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT ParmVSGetSendQInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PPARMVSGETSENDQINFOCONTEXT	pContext = (PPARMVSGETSENDQINFOCONTEXT) pvContext;
	BOOL						fSetEvent = FALSE;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

/* Now that connects must be async, this callback must accept async handles
			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong op handle)
*/

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  0x%08x",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = pConnectCompleteMsg->hResultCode;
				goto DONE;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting CREATE_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting create player on client)


			// Validate the indication.
			if (pCreatePlayerMsg->dwSize != sizeof (DPNMSG_CREATE_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CREATE_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pCreatePlayerMsg->dwSize, sizeof (DPNMSG_CREATE_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pCreatePlayerMsg->pvPlayerContext == NULL)
			{
				// Assume this is the remote player being created.  Store the
				// player context.
				pCreatePlayerMsg->pvPlayerContext = &(pContext->dpnidClient);
				fSetEvent = TRUE;
			} // end if (context is not set yet)


			// Make sure this isn't a duplicate message.
			if (*((DPNID*) pCreatePlayerMsg->pvPlayerContext) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate CREATE_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Save the player ID.
			*((DPNID*) pCreatePlayerMsg->pvPlayerContext) = pCreatePlayerMsg->dpnidPlayer;


			// Now set the event, if we're supposed too.
			if (fSetEvent)
			{
				if (! SetEvent(pContext->hClientCreatedEvent))
				{
					hr = GetLastError();
					DPTEST_TRACE(pContext->hLog, "Couldn't set client created event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hClientCreatedEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end if (should set the event)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    dwReason = %x", 1, pDestroyPlayerMsg->dwReason);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting DESTROY_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting destroy player on client)


			// Validate the indication.
			if (pDestroyPlayerMsg->dwSize != sizeof (DPNMSG_DESTROY_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_DESTROY_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwSize, sizeof (DPNMSG_DESTROY_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pDestroyPlayerMsg->pvPlayerContext == NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Player context is NULL!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is wrong)

			if (pDestroyPlayerMsg->dwReason != DPNDESTROYPLAYERREASON_NORMAL)
			{
				DPTEST_TRACE(pContext->hLog, "Player destruction reason is unexpected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwReason, DPNDESTROYPLAYERREASON_NORMAL);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (reason is wrong)


			// Make sure this isn't a duplicate or bogus message.
			if ((*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) == 0) ||
				(pDestroyPlayerMsg->dpnidPlayer != *((DPNID*) pDestroyPlayerMsg->pvPlayerContext)))
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate or unexpected DESTROY_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Clear the player ID.
			*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) = 0;
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPTEST_TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		default:
			DPTEST_TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ParmVSGetSendQInfoDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVSGetAppDescDPNMessageHandler()"
//==================================================================================
// ParmVSGetAppDescDPNMessageHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT ParmVSGetAppDescDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PPARMVSGETAPPDESCCONTEXT	pContext = (PPARMVSGETAPPDESCCONTEXT) pvContext;
	BOOL						fSetEvent = FALSE;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

/* Now that connects must be async, this callback must accept async handles
			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong op handle)
*/

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  0x%08x",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = pConnectCompleteMsg->hResultCode;
				goto DONE;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting CREATE_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting create player on client)


			// Validate the indication.
			if (pCreatePlayerMsg->dwSize != sizeof (DPNMSG_CREATE_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CREATE_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pCreatePlayerMsg->dwSize, sizeof (DPNMSG_CREATE_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pCreatePlayerMsg->pvPlayerContext == NULL)
			{
				// Assume this is the remote player being created.  Store the
				// player context.
				pCreatePlayerMsg->pvPlayerContext = &(pContext->dpnidClient);
				fSetEvent = TRUE;
			} // end if (context is not set yet)


			// Make sure this isn't a duplicate message.
			if (*((DPNID*) pCreatePlayerMsg->pvPlayerContext) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate CREATE_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Save the player ID.
			*((DPNID*) pCreatePlayerMsg->pvPlayerContext) = pCreatePlayerMsg->dpnidPlayer;


			// Now set the event, if we're supposed too.
			if (fSetEvent)
			{
				if (! SetEvent(pContext->hClientCreatedEvent))
				{
					hr = GetLastError();
					DPTEST_TRACE(pContext->hLog, "Couldn't set client created event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hClientCreatedEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end if (should set the event)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    dwReason = %x", 1, pDestroyPlayerMsg->dwReason);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting DESTROY_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting destroy player on client)


			// Validate the indication.
			if (pDestroyPlayerMsg->dwSize != sizeof (DPNMSG_DESTROY_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_DESTROY_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwSize, sizeof (DPNMSG_DESTROY_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pDestroyPlayerMsg->pvPlayerContext == NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Player context is NULL!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is wrong)

			if (pDestroyPlayerMsg->dwReason != DPNDESTROYPLAYERREASON_NORMAL)
			{
				DPTEST_TRACE(pContext->hLog, "Player destruction reason is unexpected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwReason, DPNDESTROYPLAYERREASON_NORMAL);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (reason is wrong)


			// Make sure this isn't a duplicate or bogus message.
			if ((*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) == 0) ||
				(pDestroyPlayerMsg->dpnidPlayer != *((DPNID*) pDestroyPlayerMsg->pvPlayerContext)))
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate or unexpected DESTROY_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Clear the player ID.
			*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) = 0;
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPTEST_TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		default:
			DPTEST_TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ParmVSGetAppDescDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVSSetServerInfoDPNMessageHandler()"
//==================================================================================
// ParmVSSetServerInfoDPNMessageHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT ParmVSSetServerInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PPARMVSSETSERVERINFOCONTEXT		pContext = (PPARMVSSETSERVERINFOCONTEXT) pvContext;
	PDPN_PLAYER_INFO				pdpnpi = NULL;
	DWORD							dwSize = 0;
	BOOL							fSetEvent = FALSE;


	switch (dwMsgType)
	{
		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE	pAsyncOpCompleteMsg;


			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_ASYNC_OP_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pAsyncOpCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %x", 1, pAsyncOpCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pAsyncOpCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pAsyncOpCompleteMsg->hResultCode);


			// Make sure we expect the completion.
			if (! pContext->fAsyncOpCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_ASYNC_OP_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the completion.
			if (pAsyncOpCompleteMsg->dwSize != sizeof (DPNMSG_ASYNC_OP_COMPLETE))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_ASYNC_OP_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pAsyncOpCompleteMsg->dwSize, sizeof (DPNMSG_ASYNC_OP_COMPLETE));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pAsyncOpCompleteMsg->pvUserContext != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Async op completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong context)

			if (pAsyncOpCompleteMsg->hResultCode != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Async op completed with failure!  DEBUGBREAK()-ing.  0x%08x",
					1, pAsyncOpCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong result code)


			// Make sure this isn't a double completion.
			if (pContext->fAsyncOpCompleted)
			{
				DPTEST_TRACE(pContext->hLog, "Async op completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fAsyncOpCompleted = TRUE;

			// Remember the async op handle for comparison by the main thread.
			pContext->dpnhCompletedAsyncOp = pAsyncOpCompleteMsg->hAsyncOp;

			if (! SetEvent(pContext->hAsyncOpCompletedEvent))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set async op completed event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hAsyncOpCompletedEvent);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

/* Now that connects must be async, this callback must accept async handles
			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong op handle)
*/

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  0x%08x",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = pConnectCompleteMsg->hResultCode;
				goto DONE;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting CREATE_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting create player on client)


			// Validate the indication.
			if (pCreatePlayerMsg->dwSize != sizeof (DPNMSG_CREATE_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CREATE_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pCreatePlayerMsg->dwSize, sizeof (DPNMSG_CREATE_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pCreatePlayerMsg->pvPlayerContext == NULL)
			{
				// Assume this is the remote player being created.  Store the
				// player context.
				pCreatePlayerMsg->pvPlayerContext = &(pContext->dpnidClient);
				fSetEvent = TRUE;
			} // end if (context is not set yet)


			// Make sure this isn't a duplicate message.
			if (*((DPNID*) pCreatePlayerMsg->pvPlayerContext) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate CREATE_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Save the player ID.
			*((DPNID*) pCreatePlayerMsg->pvPlayerContext) = pCreatePlayerMsg->dpnidPlayer;


			// Now set the event, if we're supposed too.
			if (fSetEvent)
			{
				if (! SetEvent(pContext->hClientCreatedEvent))
				{
					hr = GetLastError();
					DPTEST_TRACE(pContext->hLog, "Couldn't set client created event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hClientCreatedEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end if (should set the event)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    dwReason = %x", 1, pDestroyPlayerMsg->dwReason);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting DESTROY_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting destroy player on client)


			// Validate the indication.
			if (pDestroyPlayerMsg->dwSize != sizeof (DPNMSG_DESTROY_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_DESTROY_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwSize, sizeof (DPNMSG_DESTROY_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pDestroyPlayerMsg->pvPlayerContext == NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Player context is NULL!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is wrong)

			if (pDestroyPlayerMsg->dwReason != DPNDESTROYPLAYERREASON_NORMAL)
			{
				DPTEST_TRACE(pContext->hLog, "Player destruction reason is unexpected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwReason, DPNDESTROYPLAYERREASON_NORMAL);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (reason is wrong)


			// Make sure this isn't a duplicate or bogus message.
			if ((*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) == 0) ||
				(pDestroyPlayerMsg->dpnidPlayer != *((DPNID*) pDestroyPlayerMsg->pvPlayerContext)))
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate or unexpected DESTROY_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Clear the player ID.
			*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) = 0;
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPTEST_TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		case DPN_MSGID_SERVER_INFO:
			PDPNMSG_SERVER_INFO		pServerInfoMsg;


			pServerInfoMsg = (PDPNMSG_SERVER_INFO) pvMsg;

#pragma WAITFORDEVFIX(vanceo, ?, ?, "Ummmmmmmmmmmmmmmmmmmm message is meaningless!??")
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_SERVER_INFO", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pServerInfoMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidServer = %u/%x", 2, pServerInfoMsg->dpnidServer, pServerInfoMsg->dpnidServer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pServerInfoMsg->pvPlayerContext);


			// Make sure we expect the indication.
			if (! pContext->fCanGetServerInfoUpdate)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_SERVER_INFO!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the indication.
			if (pServerInfoMsg->dwSize != sizeof (DPNMSG_SERVER_INFO))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_SERVER_INFO is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pServerInfoMsg->dwSize, sizeof (DPNMSG_SERVER_INFO));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pServerInfoMsg->dpnidServer != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Server ID is not 0 (%u/%x)!  DEBUGBREAK()-ing.",
					2, pServerInfoMsg->dpnidServer, pServerInfoMsg->dpnidServer);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (player ID is available)

			if (pServerInfoMsg->pvPlayerContext != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Player context is not NULL (%x)!  DEBUGBREAK()-ing.",
					1, pServerInfoMsg->pvPlayerContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (player context is wrong)


			// Retrieve the server info.
			if (! pContext->fServer)
			{
				hr = pContext->pDP8Client->DP8C_GetServerInfo(NULL, &dwSize, 0);
				if (hr != DPNERR_BUFFERTOOSMALL)
				{
					DPTEST_TRACE(pContext->hLog, "Getting server info size didn't return expected error BUFFERTOOSMALL!  DEBUGBREAK()-ing.  0x%08x",
						1, hr);
					DEBUGBREAK();
					goto DONE;
				} // end if (couldn't get server info)

				pdpnpi = (PDPN_PLAYER_INFO) MemAlloc(dwSize);
				if (pdpnpi == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)

				pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO);

				hr = pContext->pDP8Client->DP8C_GetServerInfo(pdpnpi, &dwSize, 0);
				if (hr != DPN_OK)
				{
					DPTEST_TRACE(pContext->hLog, "Getting server info failed!  DEBUGBREAK()-ing.  0x%08x",
						1, hr);
					DEBUGBREAK();
					goto DONE;
				} // end if (couldn't get server info)

				// Validate the server info.
				if (pdpnpi->dwSize != sizeof (DPN_PLAYER_INFO))
				{
					DPTEST_TRACE(pContext->hLog, "Player info structure size modified (%u != %u)!  DEBUGBREAK()-ing.",
						2, pdpnpi->dwSize, sizeof (DPN_PLAYER_INFO));
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (player structure size modified)

				if (pdpnpi->dwInfoFlags != (DPNINFO_NAME | DPNINFO_DATA))
				{
					DPTEST_TRACE(pContext->hLog, "Player info info flags don't match expected (%x != %x)!  DEBUGBREAK()-ing.",
						2, pdpnpi->dwInfoFlags, (DPNINFO_NAME | DPNINFO_DATA));
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (player info info flags unexpected)

				if (pContext->pwszExpectedServerInfoName == NULL)
				{
					if (pdpnpi->pwszName != NULL)
					{
						DPTEST_TRACE(pContext->hLog, "Player info contains an unexpected name string \"%S\"!  DEBUGBREAK()-ing.",
							1, pdpnpi->pwszName);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end if (name was returned)
				} // end if (no name is expected)
				else
				{
					if ((pdpnpi->pwszName == NULL) ||
						(wcscmp(pdpnpi->pwszName, pContext->pwszExpectedServerInfoName) != 0))
					{
						DPTEST_TRACE(pContext->hLog, "Player info name string doesn't match expected (\"%S\" != \"%S\")!  DEBUGBREAK()-ing.",
							2, pdpnpi->pwszName, pContext->pwszExpectedServerInfoName);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end if (player info name doesn't match)
				} // end else (name is expected)

				if (pContext->pvExpectedServerInfoData == NULL)
				{
					if (pdpnpi->pvData != NULL)
					{
						DPTEST_TRACE(pContext->hLog, "Player info contains unexpected data %x (size = %u)!  DEBUGBREAK()-ing.",
							2, pdpnpi->pvData, pdpnpi->dwDataSize);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end if (name was returned)
				} // end if (no name is expected)
				else
				{
					if ((pdpnpi->pvData == NULL) ||
						(pdpnpi->dwDataSize != pContext->dwExpectedServerInfoDataSize) ||
						(memcmp(pdpnpi->pvData, pContext->pvExpectedServerInfoData, pdpnpi->dwDataSize) != 0))
					{
						DPTEST_TRACE(pContext->hLog, "Player info data doesn't match expected (%u bytes at %x != %u bytes at %x)!  DEBUGBREAK()-ing.",
							4, pdpnpi->pvData,
							pdpnpi->dwDataSize,
							pContext->pvExpectedServerInfoData,
							pContext->dwExpectedServerInfoDataSize);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end if (player data doesn't match)
				} // end else (name is expected)

				if (pdpnpi->dwPlayerFlags != DPNPLAYER_HOST)
				{
					DPTEST_TRACE(pContext->hLog, "Player info player flags don't match expected (%x != DPNPLAYER_HOST)!  DEBUGBREAK()-ing.",
						2, pdpnpi->dwPlayerFlags, DPNPLAYER_HOST);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (player info player flags unexpected)
			} // end if (not server)


			// Make sure this isn't a double indication.
			if (pContext->fGotServerInfoUpdate)
			{
				DPTEST_TRACE(pContext->hLog, "Server info update was indicated twice!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the indication came in.
			pContext->fGotServerInfoUpdate = TRUE;

			if (! SetEvent(pContext->hGotServerInfoUpdateEvent))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set got server info update event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hGotServerInfoUpdateEvent);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't set event)
		  break;

		default:
			DPTEST_TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	SAFE_LOCALFREE(pdpnpi);

	return (hr);
} // ParmVSSetServerInfoDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVSGetClientInfoDPNMessageHandler()"
//==================================================================================
// ParmVSGetClientInfoDPNMessageHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT ParmVSGetClientInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PPARMVSGETCLIENTINFOCONTEXT		pContext = (PPARMVSGETCLIENTINFOCONTEXT) pvContext;
	BOOL							fSetEvent = FALSE;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

/* Now that connects must be async, this callback must accept async handles
			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong op handle)
*/

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  0x%08x",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = pConnectCompleteMsg->hResultCode;
				goto DONE;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting CREATE_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting create player on client)


			// Validate the indication.
			if (pCreatePlayerMsg->dwSize != sizeof (DPNMSG_CREATE_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CREATE_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pCreatePlayerMsg->dwSize, sizeof (DPNMSG_CREATE_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pCreatePlayerMsg->pvPlayerContext == NULL)
			{
				// Assume this is the remote player being created.  Store the
				// player context.
				pCreatePlayerMsg->pvPlayerContext = &(pContext->dpnidClient);
				fSetEvent = TRUE;
			} // end if (context is not set yet)


			// Make sure this isn't a duplicate message.
			if (*((DPNID*) pCreatePlayerMsg->pvPlayerContext) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate CREATE_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Save the player ID.
			*((DPNID*) pCreatePlayerMsg->pvPlayerContext) = pCreatePlayerMsg->dpnidPlayer;


			// Now set the event, if we're supposed too.
			if (fSetEvent)
			{
				if (! SetEvent(pContext->hClientCreatedEvent))
				{
					hr = GetLastError();
					DPTEST_TRACE(pContext->hLog, "Couldn't set client created event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hClientCreatedEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end if (should set the event)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    dwReason = %x", 1, pDestroyPlayerMsg->dwReason);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting DESTROY_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting destroy player on client)


			// Validate the indication.
			if (pDestroyPlayerMsg->dwSize != sizeof (DPNMSG_DESTROY_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_DESTROY_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwSize, sizeof (DPNMSG_DESTROY_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pDestroyPlayerMsg->pvPlayerContext == NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Player context is NULL!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is wrong)

			if (pDestroyPlayerMsg->dwReason != DPNDESTROYPLAYERREASON_NORMAL)
			{
				DPTEST_TRACE(pContext->hLog, "Player destruction reason is unexpected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwReason, DPNDESTROYPLAYERREASON_NORMAL);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (reason is wrong)


			// Make sure this isn't a duplicate or bogus message.
			if ((*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) == 0) ||
				(pDestroyPlayerMsg->dpnidPlayer != *((DPNID*) pDestroyPlayerMsg->pvPlayerContext)))
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate or unexpected DESTROY_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Clear the player ID.
			*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) = 0;
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPTEST_TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		default:
			DPTEST_TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ParmVSGetClientInfoDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVSGetClientAddressDPNMessageHandler()"
//==================================================================================
// ParmVSGetClientAddressDPNMessageHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT ParmVSGetClientAddressDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PPARMVSGETCLIENTADDRESSCONTEXT	pContext = (PPARMVSGETCLIENTADDRESSCONTEXT) pvContext;
	BOOL							fSetEvent = FALSE;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

/* Now that connects must be async, this callback must accept async handles
			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong op handle)
*/

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  0x%08x",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = pConnectCompleteMsg->hResultCode;
				goto DONE;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				DPTEST_TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting CREATE_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting create player on client)


			// Validate the indication.
			if (pCreatePlayerMsg->dwSize != sizeof (DPNMSG_CREATE_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CREATE_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pCreatePlayerMsg->dwSize, sizeof (DPNMSG_CREATE_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pCreatePlayerMsg->pvPlayerContext == NULL)
			{
				// Assume this is the remote player being created.  Store the
				// player context.
				pCreatePlayerMsg->pvPlayerContext = &(pContext->dpnidClient);
				fSetEvent = TRUE;
			} // end if (context is not set yet)


			// Make sure this isn't a duplicate message.
			if (*((DPNID*) pCreatePlayerMsg->pvPlayerContext) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate CREATE_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pCreatePlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Save the player ID.
			*((DPNID*) pCreatePlayerMsg->pvPlayerContext) = pCreatePlayerMsg->dpnidPlayer;


			// Now set the event, if we're supposed too.
			if (fSetEvent)
			{
				if (! SetEvent(pContext->hClientCreatedEvent))
				{
					hr = GetLastError();
					DPTEST_TRACE(pContext->hLog, "Couldn't set client created event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hClientCreatedEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end if (should set the event)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    dwReason = %x", 1, pDestroyPlayerMsg->dwReason);


			// Make sure this isn't a client.
			if (! pContext->fServer)
			{
				DPTEST_TRACE(pContext->hLog, "Getting DESTROY_PLAYER on client (msg = %x)!?  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (getting destroy player on client)


			// Validate the indication.
			if (pDestroyPlayerMsg->dwSize != sizeof (DPNMSG_DESTROY_PLAYER))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_DESTROY_PLAYER is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwSize, sizeof (DPNMSG_DESTROY_PLAYER));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pDestroyPlayerMsg->pvPlayerContext == NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Player context is NULL!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is wrong)

			if (pDestroyPlayerMsg->dwReason != DPNDESTROYPLAYERREASON_NORMAL)
			{
				DPTEST_TRACE(pContext->hLog, "Player destruction reason is unexpected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pDestroyPlayerMsg->dwReason, DPNDESTROYPLAYERREASON_NORMAL);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (reason is wrong)


			// Make sure this isn't a duplicate or bogus message.
			if ((*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) == 0) ||
				(pDestroyPlayerMsg->dpnidPlayer != *((DPNID*) pDestroyPlayerMsg->pvPlayerContext)))
			{
				DPTEST_TRACE(pContext->hLog, "Got duplicate or unexpected DESTROY_PLAYER (msg = %x)!  DEBUGBREAK()-ing.",
					1, pDestroyPlayerMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Clear the player ID.
			*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) = 0;
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPTEST_TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		default:
			DPTEST_TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ParmVSGetClientAddressDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

} // namespace DPlayCoreNamespace