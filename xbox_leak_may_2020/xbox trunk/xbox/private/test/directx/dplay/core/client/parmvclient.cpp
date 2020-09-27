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
typedef struct tagPARMVCGETSENDQINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVCGETSENDQINFOCONTEXT, * PPARMVCGETSENDQINFOCONTEXT;

typedef struct tagPARMVCSETCLIENTINFOCONTEXT
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
	BOOL					fCanGetClientInfoUpdate; // whether the peer info indication can arrive or not
	BOOL					fGotClientInfoUpdate; // whether the peer info indication did arrive
	HANDLE					hGotClientInfoUpdateEvent; // event to set when peer info message is received
	PWRAPDP8SERVER			pDP8Server; // pointer to DP8Server wrapper object to use
	WCHAR*					pwszExpectedClientInfoName; // expected client info name
	PVOID*					pvExpectedClientInfoData; // expected client info data
	DWORD					dwExpectedClientInfoDataSize; // expected client info data size
	DWORD					dwExpectedClientInfoPlayerFlags; // expected client info player flags
} PARMVCSETCLIENTINFOCONTEXT, * PPARMVCSETCLIENTINFOCONTEXT;

typedef struct tagPARMVCGETSERVERINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVCGETSERVERINFOCONTEXT, * PPARMVCGETSERVERINFOCONTEXT;

typedef struct tagPARMVCGETSERVERADDRESSCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVCGETSERVERADDRESSCONTEXT, * PPARMVCGETSERVERADDRESSCONTEXT;

typedef struct tagPARMVCGETCONNINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fServer; // whether this is the server context or not
	DPNID					dpnidServer; // ID of server player (server only)
	DPNID					dpnidClient; // ID of client player (server only)
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVCGETCONNINFOCONTEXT, * PPARMVCGETCONNINFOCONTEXT;





//==================================================================================
// Prototypes
//==================================================================================
//HRESULT ParmVClientExec_CreateQI(HANDLE hLog);
//HRESULT ParmVClientExec_Init(HANDLE hLog);
//HRESULT ParmVClientExec_EnumSPs(HANDLE hLog);
//HRESULT ParmVClientExec_EnumHosts(HANDLE hLog);
//HRESULT ParmVClientExec_Cancel(HANDLE hLog);
//HRESULT ParmVClientExec_Connect(HANDLE hLog);
//HRESULT ParmVClientExec_Send(HANDLE hLog);
HRESULT ParmVClientExec_GetSendQInfo(HANDLE hLog);
//HRESULT ParmVClientExec_GetAppDesc(HANDLE hLog);
HRESULT ParmVClientExec_SetClientInfo(HANDLE hLog);
HRESULT ParmVClientExec_GetServerInfo(HANDLE hLog);
HRESULT ParmVClientExec_GetServerAddress(HANDLE hLog);
//HRESULT ParmVClientExec_Close(HANDLE hLog);
//HRESULT ParmVClientExec_Return(HANDLE hLog);
//HRESULT ParmVClientExec_GetCaps(HANDLE hLog);
//HRESULT ParmVClientExec_SetCaps(HANDLE hLog);
//HRESULT ParmVClientExec_SetSPCaps(HANDLE hLog);
//HRESULT ParmVClientExec_GetSPCaps(HANDLE hLog);
HRESULT ParmVClientExec_GetConnInfo(HANDLE hLog);
//HRESULT ParmVClientExec_RegLobby(HANDLE hLog);




HRESULT ParmVCGetSendQInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

HRESULT ParmVCSetClientInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ParmVCGetServerInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ParmVCGetServerAddressDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

HRESULT ParmVCGetConnInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);






/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientLoadTestTable()"
//==================================================================================
// ParmVClientLoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				2.1.3		Client interface parameter validation tests
//				2.1.3.1		Client DirectPlay8Create and QueryInterface parameter validation
//				2.1.3.2		Client Initialize parameter validation
//				2.1.3.3		Client EnumServiceProviders parameter validation
//
//				2.1.3.8		Client GetSendQueueInfo parameter validation
//
//				2.1.3.10	Client SetClientInfo parameter validation
//				2.1.3.11	Client GetServerInfo parameter validation
//				2.1.3.12	Client GetServerAddress parameter validation
//
//				2.1.3.19	Client GetConnectionInfo parameter validation
//
// Arguments:
//	PTNLOADTESTTABLEDATA pTNlttd	Pointer to data to use when loading the tests.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ParmVClientLoadTestTable(PTNLOADTESTTABLEDATA pTNlttd)
{
	PTNTESTTABLEGROUP	pSubGroup;
	TNADDTESTDATA		tnatd;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.3	Client interface parameter validation tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pTNlttd->pBase,
				"2.1.3", "Client interface parameter validation tests",
				&pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.3.8	Client GetSendQueueInfo parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.3.8";
	tnatd.pszName			= "Client GetSendQueueInfo parameter validation";
	tnatd.pszDescription	= "Client GetSendQueueInfo parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVClientExec_GetSendQInfo;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.3.10	Client SetClientInfo parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.3.10";
	tnatd.pszName			= "Client SetClientInfo parameter validation";
	tnatd.pszDescription	= "Client SetClientInfo parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVClientExec_SetClientInfo;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.3.11	Client GetServerInfo parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.3.11";
	tnatd.pszName			= "Client GetServerInfo parameter validation";
	tnatd.pszDescription	= "Client GetServerInfo parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVClientExec_GetServerInfo;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.3.12	Client GetServerAddress parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.3.12";
	tnatd.pszName			= "Client GetServerAddress parameter validation";
	tnatd.pszDescription	= "Client GetServerAddress parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVClientExec_GetServerAddress;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.3.19	Client GetConnectionInfo parameter validation
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.1.3.19";
	tnatd.pszName			= "Client GetConnectionInfo parameter validation";
	tnatd.pszDescription	= "Client GetConnectionInfo parameter validation";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST
								| TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ParmVClientExec_GetConnInfo;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);

	
	
	return (S_OK);
} // ParmVClientLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientExec_CreateQI()"
//==================================================================================
// ParmVClientExec_CreateQI
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.3.1 - Client DirectPlay8Create and QueryInterface parameter validation
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
HRESULT ParmVClientExec_CreateQI(HANDLE hLog)
{
	return (S_OK);
} // ParmVClientExec_CreateQI
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientExec_Init()"
//==================================================================================
// ParmVClientExec_Init
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.3.2 - Client Initialize parameter validation
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
HRESULT ParmVClientExec_Init(HANDLE hLog)
{
	return (S_OK);
} // ParmVClientExec_Init
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientExec_EnumSPs()"
//==================================================================================
// ParmVClientExec_EnumSPs
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.3.3 - Client EnumServiceProviders parameter validation
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
HRESULT ParmVClientExec_EnumSPs(HANDLE hLog)
{
	return (S_OK);
} // ParmVClientExec_EnumSPs
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientExec_GetSendQInfo()"
//==================================================================================
// ParmVClientExec_GetSendQInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.3.8 - Client GetSendQueueInfo parameter validation
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
HRESULT ParmVClientExec_GetSendQInfo(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8CLIENT				pDP8Client = NULL;
	PWRAPDP8SERVER				pDP8Server = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	DWORD						dwNumMsgs = 666;
	DWORD						dwNumBytes = 666;
	DPN_APPLICATION_DESC		dpnad;
	DPN_APPLICATION_DESC		dpnadCompare;
	PARMVCGETSENDQINFOCONTEXT	servercontext;
	PARMVCGETSENDQINFOCONTEXT	clientcontext;
	DP_DOWORKLIST				DoWorkList;
	DPNHANDLE					hAsyncOp;
	HANDLE						hConnectComplete = NULL;


	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));

	ZeroMemory(&servercontext, sizeof (PARMVCGETSENDQINFOCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVCGETSENDQINFOCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Client = new CWrapDP8Client(hLog);
		if (pDP8Client == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Client->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info with all NULLs and 0 using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Client_GetSendQueueInfo(pDP8Client->m_pDP8Client, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting send queue info with all NULLs and 0 using C++ macro didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info with all NULLs and 0");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetSendQueueInfo(NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting send queue info with all NULLs and 0 didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes, 0x666);
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

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes, 0);
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
		TEST_SECTION("Initializing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		clientcontext.hLog = hLog;

		tr = pDP8Client->DP8C_Initialize(&clientcontext,
										ParmVCGetSendQInfoDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info without a connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes, 0);
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
		TEST_SECTION("Creating new server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_CLIENT_GETSENDQINFO;
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

		servercontext.hLog = hLog;

		tr = ParmVCreateServer(hLog,
								ParmVCGetSendQInfoDPNMessageHandler,
								&servercontext,
								&dpnad,
								&(servercontext.dpnidServer),
								&pDP8Server,
								&pDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating server failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create server)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Connecting client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// XBOX - Had to make connection asynchronous

		// Host should get the create player now.
		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(hConnectComplete, NULL, FALSE, FALSE, NULL);

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		clientcontext.fConnectCanComplete = TRUE;

		tr = pDP8Client->DP8C_Connect(&dpnad,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									hConnectComplete,
									&hAsyncOp,
									0);

		if((tr != DPNSUCCESS_PENDING) && (tr != DPN_OK))
		{
			DPTEST_FAIL(hLog, "Couldn't connect! (error 0x%08x)", 1, tr);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for create player to be indicated on server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;
		
		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hClientCreatedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for connect completion to be indicated on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;
		
		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hConnectComplete))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Prevent any weird connects from completing.
		clientcontext.fConnectCanComplete = FALSE;

		// Make sure we saw the connect completion indication.
		if (! clientcontext.fConnectCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		clientcontext.fConnectCompleted = FALSE;

		ClearDoWorkList(&DoWorkList);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting send queue info on client failed!", 0);
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
		TEST_SECTION("Getting send queue info on client with NULL num msgs");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification value.
		dwNumBytes = 666;

		tr = pDP8Client->DP8C_GetSendQueueInfo(NULL, &dwNumBytes, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting send queue info on client with NULL num msgs failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumBytes != 0)
		{
			DPTEST_FAIL(hLog, "Num bytes was not expected (%u != 0)!", 1, dwNumBytes);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num bytes was unexpected)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting send queue info on client with NULL num bytes");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification value.
		dwNumMsgs = 666;

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, NULL, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting send queue info on client with NULL num bytes failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get send queue info)

		if (dwNumMsgs != 0)
		{
			DPTEST_FAIL(hLog, "Num msgs was not expected (%u != 0)!", 1, dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num msgs was unexpected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting low priority send queue info on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification values.
		dwNumMsgs = 666;
		dwNumBytes = 666;

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes,
												DPNGETSENDQUEUEINFO_PRIORITY_LOW);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting low priority send queue info on client failed!", 0);
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
		TEST_SECTION("Getting normal priority send queue info on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification values.
		dwNumMsgs = 666;
		dwNumBytes = 666;

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes,
												DPNGETSENDQUEUEINFO_PRIORITY_NORMAL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting normal priority send queue info on client failed!", 0);
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
		TEST_SECTION("Getting high priority send queue info on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification values.
		dwNumMsgs = 666;
		dwNumBytes = 666;

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes,
												DPNGETSENDQUEUEINFO_PRIORITY_HIGH);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting high priority send queue info on client failed!", 0);
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
		TEST_SECTION("Getting all priority send queues' info on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Reset the modification values.
		dwNumMsgs = 666;
		dwNumBytes = 666;

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes,
												(DPNGETSENDQUEUEINFO_PRIORITY_LOW | DPNGETSENDQUEUEINFO_PRIORITY_NORMAL | DPNGETSENDQUEUEINFO_PRIORITY_HIGH));
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting all priority send queues' info on client failed!", 0);
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
			DPTEST_FAIL(hLog, "Closing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





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





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;



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

	ClearDoWorkList(&DoWorkList);
	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);
	SAFE_CLOSEHANDLE(hConnectComplete);


	return (sr);
} // ParmVClientExec_GetSendQInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientExec_SetClientInfo()"
//==================================================================================
// ParmVClientExec_SetClientInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.3.10 - Client SetClientInfo parameter validation
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
HRESULT ParmVClientExec_SetClientInfo(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8CLIENT				pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PARMVCSETCLIENTINFOCONTEXT	servercontext;
	PARMVCSETCLIENTINFOCONTEXT	clientcontext;
	DPN_APPLICATION_DESC		dpnad;
	DPN_APPLICATION_DESC		dpnadCompare;
	DPN_PLAYER_INFO				dpnpi;
	DPN_PLAYER_INFO				dpnpiCompare;
	DPNHANDLE					dpnhSetClientInfo;
	DP_DOWORKLIST				DoWorkList;
	DPNHANDLE					hAsyncOp;
	HANDLE						hConnectComplete = NULL;

	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));

	ZeroMemory(&servercontext, sizeof (PARMVCSETCLIENTINFOCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVCSETCLIENTINFOCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Client = new CWrapDP8Client(hLog);
		if (pDP8Client == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Client->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info with all NULLs and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Client_SetClientInfo(pDP8Client->m_pDP8Client, NULL, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting client info with all NULLs and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info with all NULLs and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetClientInfo(NULL, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting client info with all NULLs and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)
*/


		dpnhSetClientInfo = (DPNHANDLE) 0x666;

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info with NULL player info structure");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetClientInfo(NULL, NULL, &dpnhSetClientInfo, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting client info with NULL player info structure didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)

		if (dpnhSetClientInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetClientInfo);
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
		TEST_SECTION("Setting client info with NULL async op handle");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Setting client info with NULL async op handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, &dpnhSetClientInfo, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Setting client info with invalid flags structure didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetClientInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetClientInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)
*/


/* XBOX - No support for synchronous
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info synchronously with async op handle");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, &dpnhSetClientInfo,
											DPNSETCLIENTINFO_SYNC);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Setting client info synchronously with async op handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetClientInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetClientInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, &dpnhSetClientInfo, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Setting client info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetClientInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetClientInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		clientcontext.hLog = hLog;

		tr = pDP8Client->DP8C_Initialize(&clientcontext,
										ParmVCSetClientInfoDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info prior to connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, &dpnhSetClientInfo, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting client info prior to connection failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetClientInfo != (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetClientInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating new server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_CLIENT_SETCLIENTINFO;
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

		servercontext.hLog = hLog;

		tr = ParmVCreateServer(hLog,
								ParmVCSetClientInfoDPNMessageHandler,
								&servercontext,
								&dpnad,
								&(servercontext.dpnidServer),
								&(servercontext.pDP8Server),
								&pDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating server failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create server)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Connecting client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// XBOX - Had to make connection asynchronous

		// Host should get the create player now.
		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,	NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(hConnectComplete, NULL, FALSE, FALSE, NULL);

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		clientcontext.fConnectCanComplete = TRUE;

		tr = pDP8Client->DP8C_Connect(&dpnad,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									hConnectComplete,
									&hAsyncOp,
									0);

		if((tr != DPNSUCCESS_PENDING) && (tr != DPN_OK))
		{
			DPTEST_FAIL(hLog, "Couldn't connect! (error 0x%08x)", 1, tr);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for create player to be indicated on server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = servercontext.pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hClientCreatedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for connect completion to be indicated on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = servercontext.pDP8Server->m_pDP8Server;
		
		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hConnectComplete))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Prevent any weird connects from completing.
		clientcontext.fConnectCanComplete = FALSE;

		// Make sure we saw the connect completion indication.
		if (! clientcontext.fConnectCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		clientcontext.fConnectCompleted = FALSE;

		ClearDoWorkList(&DoWorkList);

/* XBOX - Synchornous calls not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info synchronously");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the client info message on the server.
		servercontext.fCanGetClientInfoUpdate = TRUE;
		CREATEEVENT_OR_THROW(servercontext.hGotClientInfoUpdateEvent,
							NULL, FALSE, FALSE, NULL);
		//servercontext.pwszExpectedClientInfoName = NULL;
		//servercontext.pvExpectedClientInfoData = NULL;
		//servercontext.dwExpectedClientInfoDataSize = 0;
		//servercontext.dwExpectedClientInfoPlayerFlags = 0;

#pragma BUGBUG(vanceo, "No local message")

//		// Expect the client info message on client.
//		clientcontext.fCanGetClientInfoUpdate = TRUE;
//		//clientcontext.pwszExpectedClientInfoName = NULL;
//		//clientcontext.pvExpectedClientInfoData = NULL;
//		//clientcontext.dwExpectedClientInfoDataSize = 0;
//		clientcontext.dwExpectedClientInfoPlayerFlags = DPNPLAYER_LOCAL;

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, NULL, DPNSETCLIENTINFO_SYNC);

#pragma BUGBUG(vanceo, "No local message")
		// The client info message should have arrived by now.
		//clientcontext.fCanGetClientInfoUpdate = FALSE;

		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting client info synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)

#pragma BUGBUG(vanceo, "No local message")

//		// Make sure we got the client info message.
//		if (! clientcontext.fGotClientInfoUpdate)
//		{
//			DPTEST_FAIL(hLog, "Didn't get client info message on server!", 0);
//			SETTHROW_TESTRESULT(ERROR_NO_DATA);
//		} // end if (didn't get client info)
//
//		// Reset the context.
//		clientcontext.fGotClientInfoUpdate = FALSE;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for client info to be indicated on server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = servercontext.pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hGotClientInfoUpdateEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		// Prevent any weird client info indications.
		servercontext.fCanGetClientInfoUpdate = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! servercontext.fGotClientInfoUpdate)
		{
			DPTEST_FAIL(hLog, "Didn't get client info indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get client info indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(servercontext.hGotClientInfoUpdateEvent);
		servercontext.hGotClientInfoUpdateEvent = NULL;
		servercontext.fGotClientInfoUpdate = FALSE;

*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting client info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhSetClientInfo = (DPNHANDLE) 0x666;

		// Expect the client info message on the server.
		servercontext.fCanGetClientInfoUpdate = TRUE;
		CREATEEVENT_OR_THROW(servercontext.hGotClientInfoUpdateEvent,
							NULL, FALSE, FALSE, NULL);
		//servercontext.pwszExpectedClientInfoName = NULL;
		//servercontext.pvExpectedClientInfoData = NULL;
		//servercontext.dwExpectedClientInfoDataSize = 0;
		//servercontext.dwExpectedClientInfoPlayerFlags = 0;

#pragma BUGBUG(vanceo, "No local message")
		/*
		// Expect the client info message and async op completion on the client.
		clientcontext.fCanGetClientInfoUpdate = TRUE;
		//clientcontext.pwszExpectedClientInfoName = NULL;
		//clientcontext.pvExpectedClientInfoData = 0;
		//clientcontext.dwExpectedClientInfoDataSize = 0;
		//clientcontext.dwExpectedClientInfoPlayerFlags = DPNPLAYER_LOCAL;
		*/
		CREATEEVENT_OR_THROW(clientcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		clientcontext.fAsyncOpCanComplete = TRUE;

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, &dpnhSetClientInfo, 0);
		if (tr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPTEST_FAIL(hLog, "Setting client info didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set client info)

		if (dpnhSetClientInfo == (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSetClientInfo == NULL)
		{
			DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for async op to complete on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = servercontext.pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, clientcontext.hAsyncOpCompletedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

#pragma BUGBUG(vanceo, "No local message")
		// The client info message should have arrived by now.
		//clientcontext.fCanGetClientInfoUpdate = FALSE;

		// Prevent any weird async op completions.
		clientcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! clientcontext.fAsyncOpCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (clientcontext.dpnhCompletedAsyncOp != dpnhSetClientInfo)
		{
			DPTEST_FAIL(hLog, "Set client info completed with different async op handle (%x != %x)!",
				2, clientcontext.dpnhCompletedAsyncOp, dpnhSetClientInfo);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		clientcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(clientcontext.hAsyncOpCompletedEvent);
		clientcontext.hAsyncOpCompletedEvent = NULL;


#pragma BUGBUG(vanceo, "No local message")
		/*
		// Make sure we got the client info message.
		if (! clientcontext.fGotClientInfoUpdate)
		{
			DPTEST_FAIL(hLog, "Didn't get client info message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get client info)

		// Reset the context.
		clientcontext.fGotClientInfoUpdate = FALSE;
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for client info to be indicated on server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = servercontext.pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hGotClientInfoUpdateEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		// Prevent any weird client info indications.
		servercontext.fCanGetClientInfoUpdate = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! servercontext.fGotClientInfoUpdate)
		{
			DPTEST_FAIL(hLog, "Didn't get client info indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get client info indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(servercontext.hGotClientInfoUpdateEvent);
		servercontext.hGotClientInfoUpdateEvent = NULL;
		servercontext.fGotClientInfoUpdate = FALSE;






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

		tr = servercontext.pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = servercontext.pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (servercontext.pDP8Server);
		servercontext.pDP8Server = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Client != NULL)
	{
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	if (servercontext.pDP8Server != NULL)
	{
		delete (servercontext.pDP8Server);
		servercontext.pDP8Server = NULL;
	} // end if (have server object)

	ClearDoWorkList(&DoWorkList);
	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(clientcontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(servercontext.hGotClientInfoUpdateEvent);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);
	SAFE_CLOSEHANDLE(hConnectComplete);


	return (sr);
} // ParmVClientExec_SetClientInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientExec_GetServerInfo()"
//==================================================================================
// ParmVClientExec_GetServerInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.3.11 - Client GetServerInfo parameter validation
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
HRESULT ParmVClientExec_GetServerInfo(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8CLIENT				pDP8Client = NULL;
	PWRAPDP8SERVER				pDP8Server = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PARMVCGETSERVERINFOCONTEXT	servercontext;
	PARMVCGETSERVERINFOCONTEXT	clientcontext;
	DPN_APPLICATION_DESC		dpnad;
	DPN_APPLICATION_DESC		dpnadCompare;
	PDPN_PLAYER_INFO			pdpnpi = NULL;
	DPN_PLAYER_INFO				dpnpi;
	DWORD						dwSize;
	DWORD						dwExpectedSize;
	DP_DOWORKLIST				DoWorkList;
	DPNHANDLE					hAsyncOp;
	HANDLE						hConnectComplete = NULL;

	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));

	ZeroMemory(&servercontext, sizeof (PARMVCGETSERVERINFOCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVCGETSERVERINFOCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Client = new CWrapDP8Client(hLog);
		if (pDP8Client == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Client->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server info with NULLs and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Client_GetServerInfo(pDP8Client->m_pDP8Client, NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting server info with NULLs and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server 0 info with NULLs and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerInfo(NULL, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting server 0 info with NULLs and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server info with NULL buffer and non-zero size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSize = 666;

		tr = pDP8Client->DP8C_GetServerInfo(NULL, &dwSize, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting server info with NULL buffer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		if (dwSize != 666)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 666)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size was touched)
*/

		dwSize = 0;

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server info with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerInfo(NULL, &dwSize, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Getting server info with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server info before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerInfo(NULL, &dwSize, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Getting server info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		clientcontext.hLog = hLog;

		tr = pDP8Client->DP8C_Initialize(&clientcontext,
										ParmVCGetServerInfoDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server info prior to connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerInfo(NULL, &dwSize, 0);
		if (tr != DPNERR_NOCONNECTION)
		{
			DPTEST_FAIL(hLog, "Getting server info prior to connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		if (dwSize != 0)
		{
			DPTEST_FAIL(hLog, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating new server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_CLIENT_GETSERVERINFO;
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

		servercontext.hLog = hLog;

		tr = ParmVCreateServer(hLog,
								ParmVCGetServerInfoDPNMessageHandler,
								&servercontext,
								&dpnad,
								&(servercontext.dpnidServer),
								&pDP8Server,
								&pDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating server failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create server)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Connecting client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// XBOX - Had to make connection asynchronous

		// Host should get the create player now.
		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,	NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(hConnectComplete, NULL, FALSE, FALSE, NULL);

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		clientcontext.fConnectCanComplete = TRUE;

		tr = pDP8Client->DP8C_Connect(&dpnad,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									hConnectComplete,
									&hAsyncOp,
									0);

		if((tr != DPNSUCCESS_PENDING) && (tr != DPN_OK))
		{
			DPTEST_FAIL(hLog, "Couldn't connect! (error 0x%08x)", 1, tr);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for create player to be indicated on server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hClientCreatedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for connect completion to be indicated on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;
		
		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hConnectComplete))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Prevent any weird connects from completing.
		clientcontext.fConnectCanComplete = FALSE;

		// Make sure we saw the connect completion indication.
		if (! clientcontext.fConnectCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		clientcontext.fConnectCompleted = FALSE;

		ClearDoWorkList(&DoWorkList);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server info with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwExpectedSize = sizeof (DPN_PLAYER_INFO);

		tr = pDP8Client->DP8C_GetServerInfo(NULL, &dwSize, 0);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting server info with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		if (dwSize != dwExpectedSize)
		{
			DPTEST_FAIL(hLog, "Size was not expected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PDPN_PLAYER_INFO, pdpnpi, dwSize + BUFFERPADDING_SIZE);

		FillWithDWord((((PBYTE) pdpnpi) + sizeof (DWORD)),
						(dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE),
						DONT_TOUCH_MEMORY_PATTERN);

		pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO);

		tr = pDP8Client->DP8C_GetServerInfo(pdpnpi, &dwSize, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting server info failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

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
		dpnpi.dwPlayerFlags = DPNPLAYER_HOST;

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
			DPTEST_FAIL(hLog, "Closing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





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





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;



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

	ClearDoWorkList(&DoWorkList);
	SAFE_LOCALFREE(pdpnpi);
	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);
	SAFE_CLOSEHANDLE(hConnectComplete);


	return (sr);
} // ParmVClientExec_GetServerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientExec_GetServerAddress()"
//==================================================================================
// ParmVClientExec_GetServerAddress
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.3.12 - Client GetServerAddress parameter validation
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
HRESULT ParmVClientExec_GetServerAddress(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PWRAPDP8CLIENT					pDP8Client = NULL;
	PWRAPDP8SERVER					pDP8Server = NULL;
	PDIRECTPLAY8ADDRESS				pDP8ServerAddress = NULL;
	PDIRECTPLAY8ADDRESS				pDP8ServerAddressCompare = NULL;
	PDIRECTPLAY8ADDRESS				pDP8ReturnedAddress = NULL;
	PARMVCGETSERVERADDRESSCONTEXT	servercontext;
	PARMVCGETSERVERADDRESSCONTEXT	clientcontext;
	DPN_APPLICATION_DESC			dpnad;
	DPN_APPLICATION_DESC			dpnadCompare;
	DP_DOWORKLIST					DoWorkList;
	DPNHANDLE						hAsyncOp;
	HANDLE							hConnectComplete = NULL;

	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));

	ZeroMemory(&servercontext, sizeof (PARMVCGETSERVERADDRESSCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVCGETSERVERADDRESSCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Client = new CWrapDP8Client(hLog);
		if (pDP8Client == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Client->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server address with NULL and 0 flags using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Client_GetServerAddress(pDP8Client->m_pDP8Client, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting server address with NULL and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server address)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server address with NULL and 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerAddress(NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting server address with NULL and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server address)
*/




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server address with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerAddress(&pDP8ReturnedAddress, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Getting server address with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server address)

		if (pDP8ReturnedAddress != NULL)
		{
			DPTEST_FAIL(hLog, "Address was changed (%u != NULL)!", 1, pDP8ReturnedAddress);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (address is not NULL)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server address before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerAddress(&pDP8ReturnedAddress, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Getting server address before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server address)

		if (pDP8ReturnedAddress != NULL)
		{
			DPTEST_FAIL(hLog, "Address was changed (%u != NULL)!", 1, pDP8ReturnedAddress);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (address is not NULL)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		clientcontext.hLog = hLog;

		tr = pDP8Client->DP8C_Initialize(&clientcontext,
										ParmVCGetServerAddressDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server address prior to connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerAddress(&pDP8ReturnedAddress, 0);
		if (tr != DPNERR_NOCONNECTION)
		{
			DPTEST_FAIL(hLog, "Getting server address prior to connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server address)

		if (pDP8ReturnedAddress != NULL)
		{
			DPTEST_FAIL(hLog, "Address was changed (%u != NULL)!", 1, pDP8ReturnedAddress);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (address is not NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating new server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_CLIENT_GETSERVERADDRESS;
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

		servercontext.hLog = hLog;

		tr = ParmVCreateServer(hLog,
								ParmVCGetServerAddressDPNMessageHandler,
								&servercontext,
								&dpnad,
								&(servercontext.dpnidServer),
								&pDP8Server,
								&pDP8ServerAddress);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating server failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create server)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Saving new server address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8ServerAddress->Duplicate(&pDP8ServerAddressCompare);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't duplicate server address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't duplicate address object)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Connecting client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// XBOX - Had to make connection asynchronous

		// Host should get the create player now.
		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,	NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(hConnectComplete, NULL, FALSE, FALSE, NULL);

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		clientcontext.fConnectCanComplete = TRUE;

		tr = pDP8Client->DP8C_Connect(&dpnad,
									pDP8ServerAddress,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									hConnectComplete,
									&hAsyncOp,
									0);

		if((tr != DPNSUCCESS_PENDING) && (tr != DPN_OK))
		{
			DPTEST_FAIL(hLog, "Couldn't connect! (error 0x%08x)", 1, tr);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		tr = pDP8ServerAddress->IsEqual(pDP8ServerAddressCompare);
		if (tr != DPNSUCCESS_EQUAL)
		{
			DPTEST_FAIL(hLog, "Host address object was modified (0x%08x != 0x%08x)!",
				2, pDP8ServerAddress, pDP8ServerAddressCompare);
			THROW_TESTRESULT;
		} // end if (addresses aren't equal)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for create player to be indicated on server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hClientCreatedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for connect completion to be indicated on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;
		
		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hConnectComplete))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Prevent any weird connects from completing.
		clientcontext.fConnectCanComplete = FALSE;

		// Make sure we saw the connect completion indication.
		if (! clientcontext.fConnectCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		clientcontext.fConnectCompleted = FALSE;

		ClearDoWorkList(&DoWorkList);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting server address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetServerAddress(&pDP8ReturnedAddress, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting server address failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server address)

		if (pDP8ReturnedAddress == NULL)
		{
			DPTEST_FAIL(hLog, "No address was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (no address returned)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Ensuring returned server address is expected");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8ReturnedAddress->IsEqual(pDP8ServerAddress);
		if (tr != DPNSUCCESS_EQUAL)
		{
			DPTEST_FAIL(hLog, "Server's address returned doesn't match expected (0x%08x != 0x%08x)!",
				2, pDP8ReturnedAddress, pDP8ServerAddress);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (addresses don't match)





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
			DPTEST_FAIL(hLog, "Closing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





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





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;



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

	ClearDoWorkList(&DoWorkList);
	SAFE_RELEASE(pDP8ServerAddress);
	SAFE_RELEASE(pDP8ServerAddressCompare);
	SAFE_RELEASE(pDP8ReturnedAddress);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);
	SAFE_CLOSEHANDLE(hConnectComplete);

	return (sr);
} // ParmVClientExec_GetServerAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVClientExec_GetConnInfo()"
//==================================================================================
// ParmVClientExec_GetConnInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.19 - Client GetConnectionInfo parameter validation
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
HRESULT ParmVClientExec_GetConnInfo(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8CLIENT				pDP8Client = NULL;
	PWRAPDP8SERVER				pDP8Server = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	DPN_CONNECTION_INFO			dpnci;
	DPN_CONNECTION_INFO			dpnciCompare;
	DPN_APPLICATION_DESC		dpnad;
	DPN_APPLICATION_DESC		dpnadCompare;
	PARMVCGETCONNINFOCONTEXT	servercontext;
	PARMVCGETCONNINFOCONTEXT	clientcontext;
	DP_DOWORKLIST				DoWorkList;
	DPNHANDLE					hAsyncOp;
	HANDLE						hConnectComplete = NULL;

	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));

	ZeroMemory(&servercontext, sizeof (PARMVCGETCONNINFOCONTEXT));
	servercontext.fServer = TRUE;

	ZeroMemory(&clientcontext, sizeof (PARMVCGETCONNINFOCONTEXT));
	//clientcontext.fServer = FALSE;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Client = new CWrapDP8Client(hLog);
		if (pDP8Client == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Client->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting connection info with NULL pointer using C++ macro");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = IDirectPlay8Client_GetConnectionInfo(pDP8Client->m_pDP8Client, NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting connection info with NULL pointer using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting connection info with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetConnectionInfo(NULL, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting connection info with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)
*/




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting connection info with 0 size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnci, sizeof (DPN_CONNECTION_INFO));
		//dpnci.dwSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO));

		tr = pDP8Client->DP8C_GetConnectionInfo(&dpnci, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting connection info with 0 size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)
*/




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting connection info with too-large size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnci, sizeof (DPN_CONNECTION_INFO));
		dpnci.dwSize = sizeof (DPN_CONNECTION_INFO) + 1;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO));

		tr = pDP8Client->DP8C_GetConnectionInfo(&dpnci, 0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Getting connection info with too-large size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)

*/

		ZeroMemory(&dpnci, sizeof (DPN_CONNECTION_INFO));
		dpnci.dwSize = sizeof (DPN_CONNECTION_INFO);

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO));


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting connection info with invalid flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetConnectionInfo(&dpnci, 0x666);
		if (tr != DPNERR_INVALIDFLAGS)
		{
			DPTEST_FAIL(hLog, "Getting connection info with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)
*/





/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting connection info before initialization");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetConnectionInfo(&dpnci, 0);
		if (tr != DPNERR_UNINITIALIZED)
		{
			DPTEST_FAIL(hLog, "Getting connection info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		clientcontext.hLog = hLog;

		tr = pDP8Client->DP8C_Initialize(&clientcontext,
										ParmVCGetConnInfoDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting connection info without a connection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetConnectionInfo(&dpnci, 0);
		if (tr != DPNERR_NOCONNECTION)
		{
			DPTEST_FAIL(hLog, "Getting connection info without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating new server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_PARMV_CLIENT_GETCONNINFO;
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

		servercontext.hLog = hLog;

		tr = ParmVCreateServer(hLog,
								ParmVCGetConnInfoDPNMessageHandler,
								&servercontext,
								&dpnad,
								&(servercontext.dpnidServer),
								&pDP8Server,
								&pDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating server failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create server)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Connecting client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// XBOX - Had to make connection asynchronous

		// Host should get the create player now.
		CREATEEVENT_OR_THROW(servercontext.hClientCreatedEvent,	NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(hConnectComplete, NULL, FALSE, FALSE, NULL);

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		clientcontext.fConnectCanComplete = TRUE;

		tr = pDP8Client->DP8C_Connect(&dpnad,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									hConnectComplete,
									&hAsyncOp,
									0);

		if((tr != DPNSUCCESS_PENDING) && (tr != DPN_OK))
		{
			DPTEST_FAIL(hLog, "Couldn't connect! (error 0x%08x)", 1, tr);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for create player to be indicated on server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, servercontext.hClientCreatedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for connect completion to be indicated on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;
		
		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hConnectComplete))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Prevent any weird connects from completing.
		clientcontext.fConnectCanComplete = FALSE;

		// Make sure we saw the connect completion indication.
		if (! clientcontext.fConnectCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		clientcontext.fConnectCompleted = FALSE;

		ClearDoWorkList(&DoWorkList);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting connection info on client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_GetConnectionInfo(&dpnci, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting connection info on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

#pragma TODO(vanceo, "Validate returned structure")
		/*
		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)
		*/





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
			DPTEST_FAIL(hLog, "Closing server object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure we got the destroy player messages.
		if (servercontext.dpnidServer != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for server on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)

		if (servercontext.dpnidClient != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get DESTROY_PLAYER indication for client on server!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player)





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





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;



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

	ClearDoWorkList(&DoWorkList);
	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(servercontext.hClientCreatedEvent);
	SAFE_CLOSEHANDLE(hConnectComplete);


	return (sr);
} // ParmVClientExec_GetConnInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCGetSendQInfoDPNMessageHandler()"
//==================================================================================
// ParmVCGetSendQInfoDPNMessageHandler
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
HRESULT ParmVCGetSendQInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PPARMVCGETSENDQINFOCONTEXT	pContext = (PPARMVCGETSENDQINFOCONTEXT) pvContext;
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

			// XBOX - Handle async connect
			if(pConnectCompleteMsg->pvUserContext)
			{
				SetEvent((HANDLE) pConnectCompleteMsg->pvUserContext);
			}

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
} // ParmVCGetSendQInfoDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCSetClientInfoDPNMessageHandler()"
//==================================================================================
// ParmVCSetClientInfoDPNMessageHandler
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
HRESULT ParmVCSetClientInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PPARMVCSETCLIENTINFOCONTEXT		pContext = (PPARMVCSETCLIENTINFOCONTEXT) pvContext;
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

		case DPN_MSGID_CLIENT_INFO:
			PDPNMSG_CLIENT_INFO		pClientInfoMsg;


			pClientInfoMsg = (PDPNMSG_CLIENT_INFO) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CLIENT_INFO", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pClientInfoMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidClient = %u/%x", 2, pClientInfoMsg->dpnidClient, pClientInfoMsg->dpnidClient);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pClientInfoMsg->pvPlayerContext);


			// Make sure we expect the indication.
			if (! pContext->fCanGetClientInfoUpdate)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CLIENT_INFO!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the indication.
			if (pClientInfoMsg->dwSize != sizeof (DPNMSG_CLIENT_INFO))
			{
				DPTEST_TRACE(pContext->hLog, "Size of DPNMSG_CLIENT_INFO is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pClientInfoMsg->dwSize, sizeof (DPNMSG_CLIENT_INFO));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pClientInfoMsg->dpnidClient != pContext->dpnidClient)
			{
				DPTEST_TRACE(pContext->hLog, "Client ID is not expected (%u/%x != %u/%x)!  DEBUGBREAK()-ing.",
					4, pClientInfoMsg->dpnidClient, pClientInfoMsg->dpnidClient,
					pContext->dpnidClient, pContext->dpnidClient);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (player ID is available)

			if (pClientInfoMsg->pvPlayerContext != &(pContext->dpnidClient))
			{
				DPTEST_TRACE(pContext->hLog, "Player context is not expected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pClientInfoMsg->pvPlayerContext, &(pContext->dpnidClient));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (player context is wrong)


			// Retrieve the client info.
			hr = pContext->pDP8Server->DP8S_GetClientInfo(pClientInfoMsg->dpnidClient,
														NULL, &dwSize, 0);
			if (hr != DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_TRACE(pContext->hLog, "Getting client info size didn't return expected error BUFFERTOOSMALL!  DEBUGBREAK()-ing.  0x%08x",
					1, hr);
				DEBUGBREAK();
				goto DONE;
			} // end if (couldn't get client info)

			pdpnpi = (PDPN_PLAYER_INFO) MemAlloc(dwSize);
			if (pdpnpi == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO);

			hr = pContext->pDP8Server->DP8S_GetClientInfo(pClientInfoMsg->dpnidClient,
														pdpnpi, &dwSize, 0);
			if (hr != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Getting client info failed!  DEBUGBREAK()-ing.  0x%08x",
					1, hr);
				DEBUGBREAK();
				goto DONE;
			} // end if (couldn't get client info)

			// Validate the client info.
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

			if (pContext->pwszExpectedClientInfoName == NULL)
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
					(wcscmp(pdpnpi->pwszName, pContext->pwszExpectedClientInfoName) != 0))
				{
					DPTEST_TRACE(pContext->hLog, "Player info name string doesn't match expected (\"%S\" != \"%S\")!  DEBUGBREAK()-ing.",
						2, pdpnpi->pwszName, pContext->pwszExpectedClientInfoName);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (player info name doesn't match)
			} // end else (name is expected)

			if (pContext->pvExpectedClientInfoData == NULL)
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
					(pdpnpi->dwDataSize != pContext->dwExpectedClientInfoDataSize) ||
					(memcmp(pdpnpi->pvData, pContext->pvExpectedClientInfoData, pdpnpi->dwDataSize) != 0))
				{
					DPTEST_TRACE(pContext->hLog, "Player info data doesn't match expected (%u bytes at %x != %u bytes at %x)!  DEBUGBREAK()-ing.",
						4, pdpnpi->pvData,
						pdpnpi->dwDataSize,
						pContext->pvExpectedClientInfoData,
						pContext->dwExpectedClientInfoDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (player data doesn't match)
			} // end else (name is expected)

			if (pdpnpi->dwPlayerFlags != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Player info player flags are unexpected (%x)!  DEBUGBREAK()-ing.",
					1, pdpnpi->dwInfoFlags);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (player info player flags unexpected)


			// Make sure this isn't a double indication.
			if (pContext->fGotClientInfoUpdate)
			{
				DPTEST_TRACE(pContext->hLog, "Client info update was indicated twice!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the indication came in.
			pContext->fGotClientInfoUpdate = TRUE;

			if (! SetEvent(pContext->hGotClientInfoUpdateEvent))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set got client info update event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hGotClientInfoUpdateEvent);

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

			// XBOX - Handle async connect
			if(pConnectCompleteMsg->pvUserContext)
			{
				SetEvent((HANDLE) pConnectCompleteMsg->pvUserContext);
			}

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
				break;
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

	SAFE_LOCALFREE(pdpnpi);

	return (hr);
} // ParmVCSetClientInfoDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCGetServerInfoDPNMessageHandler()"
//==================================================================================
// ParmVCGetServerInfoDPNMessageHandler
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
HRESULT ParmVCGetServerInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PPARMVCGETSERVERINFOCONTEXT		pContext = (PPARMVCGETSERVERINFOCONTEXT) pvContext;
	BOOL							fSetEvent = FALSE;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = 0x%08x", 1, pConnectCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				hr = E_FAIL;
				goto DONE;
				break;
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

			// XBOX - Handle async connect
			if(pConnectCompleteMsg->pvUserContext)
			{
				SetEvent((HANDLE) pConnectCompleteMsg->pvUserContext);
			}

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
} // ParmVCGetServerInfoDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCGetServerAddressDPNMessageHandler()"
//==================================================================================
// ParmVCGetServerAddressDPNMessageHandler
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
HRESULT ParmVCGetServerAddressDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PPARMVCGETSERVERADDRESSCONTEXT	pContext = (PPARMVCGETSERVERADDRESSCONTEXT) pvContext;
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

			// XBOX - Handle async connect
			if(pConnectCompleteMsg->pvUserContext)
			{
				SetEvent((HANDLE) pConnectCompleteMsg->pvUserContext);
			}

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
} // ParmVCGetServerAddressDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCGetConnInfoDPNMessageHandler()"
//==================================================================================
// ParmVCGetConnInfoDPNMessageHandler
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
HRESULT ParmVCGetConnInfoDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PPARMVCGETCONNINFOCONTEXT	pContext = (PPARMVCGETCONNINFOCONTEXT) pvContext;
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

			// XBOX - Handle async connect
			if(pConnectCompleteMsg->pvUserContext)
			{
				SetEvent((HANDLE) pConnectCompleteMsg->pvUserContext);
			}
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
} // ParmVCGetConnInfoDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

} // namespace DPlayCoreNamespace