//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include "dptest.h"
#include "macros.h"
#include "parmvalidation.h"

using namespace DPlayCoreNamespace;

namespace DPlayCoreNamespace {

//==================================================================================
// Globals
//==================================================================================
// Test specific globals
static CRITICAL_SECTION g_csSerializeAccess;
static HANDLE g_hLog = INVALID_HANDLE_VALUE;
HANDLE g_hHeap = NULL;

static PDP_LOCAL_TEST_FUNCTION g_LocalTestFunctions[DPMSG_LOCAL_MAXTEST] = 
{
	ParmVExec_BBuildW,							//DPMSG_ADDR_BUILDW
	ParmVExec_BBuildA,							//DPMSG_ADDR_BUILDA
	ParmVExec_BDuplicate,						//DPMSG_ADDR_DUPLICATE
	ParmVExec_BSetEqual,						//DPMSG_ADDR_SETEQUAL
	ParmVExec_BIsEqual,							//DPMSG_ADDR_ISEQUAL
	ParmVExec_BClear,							//DPMSG_ADDR_CLEAR
	ParmVExec_BGetURLW,							//DPMSG_ADDR_GETURLW
	ParmVExec_BGetURLA,							//DPMSG_ADDR_GETURLA
	ParmVExec_BGetSP,							//DPMSG_ADDR_GETSP
	ParmVExec_BGetUserData,						//DPMSG_ADDR_GETUSERDATA
	ParmVExec_BSetSP,							//DPMSG_ADDR_SETSP
	ParmVExec_BSetUserData,						//DPMSG_ADDR_SETUSERDATA
	ParmVExec_BGetNumComponents,				//DPMSG_ADDR_GETNUMCOMPONENTS
	ParmVExec_BGetByName,						//DPMSG_ADDR_GETBYNAME
	ParmVExec_BGetByIndex,						//DPMSG_ADDR_GETBYINDEX
	ParmVExec_BAdd,								//DPMSG_ADDR_ADD
	ParmVExec_BGetDevice,						//DPMSG_ADDR_GETDEVICE
	ParmVExec_BSetDevice,						//DPMSG_ADDR_SETDEVICE
	
	ParmVExec_IPBuildSockAddr,					//DPMSG_ADDRIP_BUILDSOCKADDR
	ParmVExec_IPGetSockAddr,					//DPMSG_ADDRIP_GETSOCKADDR
	ParmVExec_IPBuildAddress,					//DPMSG_ADDRIP_BUILDADDRESS
	ParmVExec_IPGetAddress,						//DPMSG_ADDRIP_GETADDRESS
	ParmVExec_IPBuildLocalAddress,				//DPMSG_ADDRIP_BUILDLOCALADDRESS
	ParmVExec_IPGetLocalAddress,				//DPMSG_ADDRIP_GETLOCALADDRESS

	ParmVClientExec_GetSendQInfo,				//DPMSG_CLIENT_GETSENDQINFO
	ParmVClientExec_SetClientInfo,				//DPMSG_CLIENT_SETCLIENTINFO
	ParmVClientExec_GetServerInfo,				//DPMSG_CLIENT_GETSERVERINFO
	ParmVClientExec_GetServerAddress,			//DPMSG_CLIENT_GETSERVERADDRESS
	ParmVClientExec_GetConnInfo,				//DPMSG_CLIENT_GETCONNINFO

	ParmVPeerExec_CreateQI,						//DPMSG_PEER_CREATEQI
	ParmVPeerExec_Init,							//DPMSG_PEER_INIT
	ParmVPeerExec_EnumSPs,						//DPMSG_PEER_ENUMSPS
	ParmVPeerExec_Cancel,						//DPMSG_PEER_CANCEL
	ParmVPeerExec_Connect,						//DPMSG_PEER_CONNECT
	ParmVPeerExec_SendTo,						//DPMSG_PEER_SENDTO
	ParmVPeerExec_GetSendQInfo,					//DPMSG_PEER_GETSENDQINFO
	ParmVPeerExec_Host,							//DPMSG_PEER_HOST
	ParmVPeerExec_GetAppDesc,					//DPMSG_PEER_GETAPPDESC
	ParmVPeerExec_SetAppDesc,					//DPMSG_PEER_SETAPPDESC
	ParmVPeerExec_CreateGroup,					//DPMSG_PEER_CREATEGROUP
	ParmVPeerExec_DestroyGroup,					//DPMSG_PEER_DESTROYGROUP
	ParmVPeerExec_AddToGroup,					//DPMSG_PEER_ADDTOGROUP
	ParmVPeerExec_RemoveFromGroup,				//DPMSG_PEER_REMOVEFROMGROUP
	ParmVPeerExec_SetGroupInfo,					//DPMSG_PEER_SETGROUPINFO
	ParmVPeerExec_GetGroupInfo,					//DPMSG_PEER_GETGROUPINFO
	ParmVPeerExec_EnumPAndG,					//DPMSG_PEER_ENUMPANDG
	ParmVPeerExec_EnumGroupMembers,				//DPMSG_PEER_ENUMGROUPMEMBERS
	ParmVPeerExec_SetPeerInfo,					//DPMSG_PEER_SETPEERINFO
	ParmVPeerExec_GetPeerInfo,					//DPMSG_PEER_GETPEERINFO
	ParmVPeerExec_GetPeerAddress,				//DPMSG_PEER_GETPEERADDRESS
	ParmVPeerExec_GetLHostAddresses,			//DPMSG_PEER_GETLHOSTADDRESSES
	ParmVPeerExec_Close,						//DPMSG_PEER_CLOSE
	ParmVPeerExec_EnumHosts,					//DPMSG_PEER_ENUMHOSTS
	ParmVPeerExec_DestroyPeer,					//DPMSG_PEER_DESTROYPEER
	ParmVPeerExec_Return,						//DPMSG_PEER_RETURN
	ParmVPeerExec_GetPlayerContext,				//DPMSG_PEER_GETPLAYERCONTEXT
	ParmVPeerExec_GetGroupContext,				//DPMSG_PEER_GETGROUPCONTEXT
	ParmVPeerExec_GetCaps,						//DPMSG_PEER_GETCAPS
	ParmVPeerExec_SetCaps,						//DPMSG_PEER_SETCAPS
	ParmVPeerExec_SetSPCaps,					//DPMSG_PEER_SETSPCAPS
	ParmVPeerExec_GetSPCaps,					//DPMSG_PEER_GETSPCAPS
	ParmVPeerExec_GetConnInfo,					//DPMSG_PEER_GETCONNINFO
	ParmVPeerExec_Terminate,					//DPMSG_PEER_TERMINATE

	ParmVServerExec_CreateQI,					//DPMSG_SERVER_CREATEQI
	ParmVServerExec_Init,						//DPMSG_SERVER_INIT
	ParmVServerExec_EnumSPs,					//DPMSG_SERVER_ENUMSPS
	ParmVServerExec_Cancel,						//DPMSG_SERVER_CANCEL
	ParmVServerExec_GetSendQInfo,				//DPMSG_SERVER_GETSENDQINFO
	ParmVServerExec_GetAppDesc,					//DPMSG_SERVER_GETAPPDESC
	ParmVServerExec_SetServerInfo,				//DPMSG_SERVER_SETSERVERINFO
	ParmVServerExec_GetClientInfo,				//DPMSG_SERVER_GETCLIENTINFO
	ParmVServerExec_GetClientAddress			//DPMSG_SERVER_GETCLIENTADDRESS
};

static LPSTR g_LocalTestNames[DPMSG_LOCAL_MAXTEST] = 
{
	"Addr: BBuildW",							//DPMSG_ADDR_BUILDW
	"Addr: BBuildA",							//DPMSG_ADDR_BUILDA
	"Addr: BDuplicate",							//DPMSG_ADDR_DUPLICATE
	"Addr: BSetEqual",							//DPMSG_ADDR_SETEQUAL
	"Addr: BIsEqual",							//DPMSG_ADDR_ISEQUAL
	"Addr: BClear",								//DPMSG_ADDR_CLEAR
	"Addr: BGetURLW",							//DPMSG_ADDR_GETURLW
	"Addr: BGetURLA",							//DPMSG_ADDR_GETURLA
	"Addr: BGetSP",								//DPMSG_ADDR_GETSP
	"Addr: BGetUserData",						//DPMSG_ADDR_GETUSERDATA
	"Addr: BSetSP",								//DPMSG_ADDR_SETSP
	"Addr: BSetUserData",						//DPMSG_ADDR_SETUSERDATA
	"Addr: BGetNumComponents",					//DPMSG_ADDR_GETNUMCOMPONENTS
	"Addr: BGetByName",							//DPMSG_ADDR_GETBYNAME
	"Addr: BGetByIndex",						//DPMSG_ADDR_GETBYINDEX
	"Addr: BAdd",								//DPMSG_ADDR_ADD
	"Addr: BGetDevice",							//DPMSG_ADDR_GETDEVICE
	"Addr: BSetDevice",							//DPMSG_ADDR_SETDEVICE
	
	"AddrIP: BuildSockAddr",					//DPMSG_ADDRIP_BUILDSOCKADDR
	"AddrIP: GetSockAddr",						//DPMSG_ADDRIP_GETSOCKADDR
	"AddrIP: BuildAddress",						//DPMSG_ADDRIP_BUILDADDRESS
	"AddrIP: GetAddress",						//DPMSG_ADDRIP_GETADDRESS
	"AddrIP: BuildLocalAddress",				//DPMSG_ADDRIP_BUILDLOCALADDRESS
	"AddrIP: GetLocalAddress",					//DPMSG_ADDRIP_GETLOCALADDRESS

	"Client: GetSendQInfo",						//DPMSG_CLIENT_GETSENDQINFO
	"Client: SetClientInfo",					//DPMSG_CLIENT_SETCLIENTINFO
	"Client: GetServerInfo",					//DPMSG_CLIENT_GETSERVERINFO
	"Client: GetServerAddress",					//DPMSG_CLIENT_GETSERVERADDRESS
	"Client: GetConnInfo",						//DPMSG_CLIENT_GETCONNINFO

	"Peer: CreateQI",							//DPMSG_PEER_CREATEQI
	"Peer: Init",								//DPMSG_PEER_INIT
	"Peer: EnumSPs",							//DPMSG_PEER_ENUMSPS
	"Peer: Cancel",								//DPMSG_PEER_CANCEL
	"Peer: Connect",							//DPMSG_PEER_CONNECT
	"Peer: SendTo",								//DPMSG_PEER_SENDTO
	"Peer: GetSendQInfo",						//DPMSG_PEER_GETSENDQINFO
	"Peer: Host",								//DPMSG_PEER_HOST
	"Peer: GetAppDesc",							//DPMSG_PEER_GETAPPDESC
	"Peer: SetAppDesc",							//DPMSG_PEER_SETAPPDESC
	"Peer: CreateGroup",						//DPMSG_PEER_CREATEGROUP
	"Peer: DestroyGroup",						//DPMSG_PEER_DESTROYGROUP
	"Peer: AddToGroup",							//DPMSG_PEER_ADDTOGROUP
	"Peer: RemoveFromGroup",					//DPMSG_PEER_REMOVEFROMGROUP
	"Peer: SetGroupInfo",						//DPMSG_PEER_SETGROUPINFO
	"Peer: GetGroupInfo",						//DPMSG_PEER_GETGROUPINFO
	"Peer: EnumPAndG",							//DPMSG_PEER_ENUMPANDG
	"Peer: EnumGroupMembers",					//DPMSG_PEER_ENUMGROUPMEMBERS
	"Peer: SetPeerInfo",						//DPMSG_PEER_SETPEERINFO
	"Peer: GetPeerInfo",						//DPMSG_PEER_GETPEERINFO
	"Peer: GetPeerAddress",						//DPMSG_PEER_GETPEERADDRESS
	"Peer: GetLHostAddresses",					//DPMSG_PEER_GETLHOSTADDRESSES
	"Peer: Close",								//DPMSG_PEER_CLOSE
	"Peer: EnumHosts",							//DPMSG_PEER_ENUMHOSTS
	"Peer: DestroyPeer",						//DPMSG_PEER_DESTROYPEER
	"Peer: Return",								//DPMSG_PEER_RETURN
	"Peer: GetPlayerContext",					//DPMSG_PEER_GETPLAYERCONTEXT
	"Peer: GetGroupContext",					//DPMSG_PEER_GETGROUPCONTEXT
	"Peer: GetCaps",							//DPMSG_PEER_GETCAPS
	"Peer: SetCaps",							//DPMSG_PEER_SETCAPS
	"Peer: SetSPCaps",							//DPMSG_PEER_SETSPCAPS
	"Peer: GetSPCaps",							//DPMSG_PEER_GETSPCAPS
	"Peer: GetConnInfo",						//DPMSG_PEER_GETCONNINFO
	"Peer: Terminate",							//DPMSG_PEER_TERMINATE

	"Server: CreateQI",							//DPMSG_SERVER_CREATEQI
	"Server: Init",								//DPMSG_SERVER_INIT
	"Server: EnumSPs",							//DPMSG_SERVER_ENUMSPS
	"Server: Cancel",							//DPMSG_SERVER_CANCEL
	"Server: GetSendQInfo",						//DPMSG_SERVER_GETSENDQINFO
	"Server: GetAppDesc",						//DPMSG_SERVER_GETAPPDESC
	"Server: SetServerInfo",					//DPMSG_SERVER_SETSERVERINFO
	"Server: GetClientInfo",					//DPMSG_SERVER_GETCLIENTINFO
	"Server: GetClientAddress"					//DPMSG_SERVER_GETCLIENTADDRESS
};

//static PDP_TEST_FUNCTION g_NetworkTestFunctions[DPMSG_NET_MAXTEST] = 
//{
//	DPSourceMaxTest										// DPMSG_SOURCEPORT_MAX
//};

//static LPSTR g_NetworkTestNames[DPMSG_NET_MAXTEST] = 
//{
//	"Packet from maximum port"							// DPMSG_SOURCEPORT_MAX
//};

//==================================================================================
// DpTestDllMain
//----------------------------------------------------------------------------------
//
// Description: DLL entry
//
// Arguments:
//	HINSTANCE	hInstance		Handle to module
//	DWORD		dwReason		Indicates the reason for calling the function
//	LPVOID		lpContext		reserved
// Returns:
//	TRUE on success
//==================================================================================
BOOL WINAPI DpTestDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	WSADATA WSAData;

	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XnetInitialize(NULL, TRUE);
		WSAStartup(MAKEWORD(2, 2), &WSAData);
		InitializeCriticalSection(&g_csSerializeAccess);
		g_hHeap = HeapCreate(0,0,0);
		break;
	case DLL_PROCESS_DETACH:
		WSACleanup();
		XnetCleanup();
		DeleteCriticalSection(&g_csSerializeAccess);
		HeapDestroy(g_hHeap);
		g_hHeap = NULL;
		break;
	default:
		break;
	}

    return TRUE;
}

HRESULT TestHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	return S_OK;
}

/*
HRESULT ParmVPCreateGroupDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

typedef struct tagPARMVPCREATEGROUPCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fHost; // whether this is the host context or not
	DPNID					dpnidHost; // ID of host player
	DPNID					dpnidClient; // ID of client player
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	HRESULT					hrExpectedResult; // the expected result code of the connect/group creation
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	HANDLE					hAsyncOpCompletedEvent; // event to set when async op completes
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	DPNID					dpnidGroupCreated; // the DPNID of the group created
	HANDLE					hGotCreateGroupEvent; // event to set when create group indication is received
} PARMVPCREATEGROUPCONTEXT, * PPARMVPCREATEGROUPCONTEXT;

#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_CreateGroupProblem()"
HRESULT ParmVPeerExec_CreateGroupProblem(HANDLE hLog)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PWRAPDP8PEER				pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	DPN_GROUP_INFO				dpngi;
	DPN_APPLICATION_DESC		dpnad;
	PARMVPCREATEGROUPCONTEXT	hostcontext;
	PARMVPCREATEGROUPCONTEXT	nonhostcontext;
	DPNID						dpnidGroup;
	DPNHANDLE					dpnhCreateGroup;
	DP_DOWORKLIST				DoWorkList;
	DPNHANDLE					hAsyncOp;
	HANDLE						hAsyncOpComplete = NULL;
	DWORD						i = 0;

	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));
	ZeroMemory(&hostcontext, sizeof (PARMVPCREATEGROUPCONTEXT));
	hostcontext.fHost = TRUE;

	ZeroMemory(&nonhostcontext, sizeof (PARMVPCREATEGROUPCONTEXT));

	ZeroMemory(&dpngi, sizeof (DPN_GROUP_INFO));
	dpngi.dwSize = sizeof (DPN_GROUP_INFO);

	ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
	dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpnad.guidApplication = GUID_PARMV_PEER_GETAPPDESC;

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new CWrapDP8Peer(hLog);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Peer->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing peer object with nonhost context");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		nonhostcontext.hLog = hLog;

		tr = pDP8Peer->DP8P_Initialize(&nonhostcontext,
										ParmVPCreateGroupDPNMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating new peer host object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hostcontext.hLog = hLog;

		tr = ParmVCreatePeerHost(hLog, ParmVPCreateGroupDPNMessageHandler,
								&hostcontext,
								&dpnad,
								&(hostcontext.dpnidHost),
								&pDP8PeerHost,
								&pDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Connecting peer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// XBOX - Had to make connection asynchronous

		// Host should get the create player now.
		CREATEEVENT_OR_THROW(hostcontext.hClientCreatedEvent, NULL, FALSE, FALSE, NULL);
		SAFE_CLOSEHANDLE(hAsyncOpComplete);
		CREATEEVENT_OR_THROW(hAsyncOpComplete, NULL, FALSE, FALSE, NULL);

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		tr = pDP8Peer->DP8P_Connect(&dpnad,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									&(nonhostcontext.dpnidClient),
									hAsyncOpComplete,
									&hAsyncOp,
									0);

		if((tr != DPNSUCCESS_PENDING) && (tr != DPN_OK))
		{
			DPTEST_FAIL(hLog, "Couldn't connect! (error 0x%08x)", 1, tr);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for connect to complete on non-host");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumPeers = 2;
		DoWorkList.apDP8Peers = (PDIRECTPLAY8PEER *) MemAlloc(sizeof(LPVOID) * 2);
		DoWorkList.apDP8Peers[0] = pDP8Peer->m_pDP8Peer;
		DoWorkList.apDP8Peers[1] = pDP8PeerHost->m_pDP8Peer;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hAsyncOpComplete))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for create player to be indicated on host");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = FALSE;
		DoWorkList.dwNumPeers = 2;
		DoWorkList.apDP8Peers = (PDIRECTPLAY8PEER *) MemAlloc(sizeof(LPVOID) * 2);
		DoWorkList.apDP8Peers[0] = pDP8Peer->m_pDP8Peer;
		DoWorkList.apDP8Peers[1] = pDP8PeerHost->m_pDP8Peer;

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hostcontext.hClientCreatedEvent))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		ClearDoWorkList(&DoWorkList);

		for(i = 0; i < 1; i++)
		{
			
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating group on host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			dpnhCreateGroup = (DPNHANDLE) 0x666;
			
			// Expect the create group message and async op completion on the host.
			//hostcontext.dpnidExpectedGroupOwner = 0;
			hostcontext.fCanGetCreateGroup = TRUE;
			SAFE_CLOSEHANDLE(hostcontext.hAsyncOpCompletedEvent);
			CREATEEVENT_OR_THROW(hostcontext.hAsyncOpCompletedEvent, NULL, FALSE, FALSE, NULL);
			hostcontext.fAsyncOpCanComplete = TRUE;
			
			// Expect the create group message on the client.
			//nonhostcontext.dpnidExpectedGroupOwner = 0;
			SAFE_CLOSEHANDLE(nonhostcontext.hGotCreateGroupEvent);
			CREATEEVENT_OR_THROW(nonhostcontext.hGotCreateGroupEvent, NULL, FALSE, FALSE, NULL);
			nonhostcontext.fCanGetCreateGroup = TRUE;
			
			tr = pDP8PeerHost->DP8P_CreateGroup(&dpngi, NULL, NULL, &dpnhCreateGroup, 0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Creating group on host didn't return expected PENDING success code!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't create group)
			
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for async op to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
			DoWorkList.fTrackDoWorkTimes = FALSE;
			DoWorkList.dwNumPeers = 2;
			DoWorkList.apDP8Peers = (PDIRECTPLAY8PEER *) MemAlloc(sizeof(LPVOID) * 2);
			DoWorkList.apDP8Peers[0] = pDP8Peer->m_pDP8Peer;
			DoWorkList.apDP8Peers[1] = pDP8PeerHost->m_pDP8Peer;
			
			if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hostcontext.hAsyncOpCompletedEvent))
			{
				DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
				SETTHROW_TESTRESULT(E_FAIL);
			}
			
			ClearDoWorkList(&DoWorkList);
			
			// The create group message should have arrived by now.
			hostcontext.fCanGetCreateGroup = FALSE;
			
			// Prevent any weird async op completions.
			hostcontext.fAsyncOpCanComplete = FALSE;
			
			// Reset the context.
			hostcontext.fAsyncOpCompleted = FALSE;
			CloseHandle(hostcontext.hAsyncOpCompletedEvent);
			hostcontext.hAsyncOpCompletedEvent = NULL;
			
			// Reset the context.
			hostcontext.dpnidGroupCreated = 0;
			
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for group creation to be indicated on client");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
			DoWorkList.fTrackDoWorkTimes = FALSE;
			DoWorkList.dwNumPeers = 2;
			DoWorkList.apDP8Peers = (PDIRECTPLAY8PEER *) MemAlloc(sizeof(LPVOID) * 2);
			DoWorkList.apDP8Peers[0] = pDP8Peer->m_pDP8Peer;
			DoWorkList.apDP8Peers[1] = pDP8PeerHost->m_pDP8Peer;
			
			if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, nonhostcontext.hGotCreateGroupEvent))
			{
				DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
				SETTHROW_TESTRESULT(E_FAIL);
			}
			
			ClearDoWorkList(&DoWorkList);
			
			// Prevent any weird group creations.
			nonhostcontext.fCanGetCreateGroup = FALSE;
			
			// Reset the context.
			CloseHandle(nonhostcontext.hGotCreateGroupEvent);
			nonhostcontext.hGotCreateGroupEvent = NULL;
			nonhostcontext.dpnidGroupCreated = 0;
			
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating group on client");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			dpnhCreateGroup = (DPNHANDLE) 0x666;
			
			// Expect the create group message on the host.
			//hostcontext.dpnidExpectedGroupOwner = 0;
			SAFE_CLOSEHANDLE(hostcontext.hGotCreateGroupEvent);
			CREATEEVENT_OR_THROW(hostcontext.hGotCreateGroupEvent, NULL, FALSE, FALSE, NULL);
			hostcontext.fCanGetCreateGroup = TRUE;
			
			// Expect the create group message and async op completion on the client.
			//nonhostcontext.dpnidExpectedGroupOwner = 0;
			nonhostcontext.fCanGetCreateGroup = TRUE;
			SAFE_CLOSEHANDLE(nonhostcontext.hAsyncOpCompletedEvent);
			CREATEEVENT_OR_THROW(nonhostcontext.hAsyncOpCompletedEvent, NULL, FALSE, FALSE, NULL);
			nonhostcontext.fAsyncOpCanComplete = TRUE;
			
			tr = pDP8Peer->DP8P_CreateGroup(&dpngi, NULL, NULL, &dpnhCreateGroup, 0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Creating group on client didn't return expected PENDING success code!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't create group)
			
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for async op to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
			DoWorkList.fTrackDoWorkTimes = FALSE;
			DoWorkList.dwNumPeers = 2;
			DoWorkList.apDP8Peers = (PDIRECTPLAY8PEER *) MemAlloc(sizeof(LPVOID) * 2);
			DoWorkList.apDP8Peers[0] = pDP8Peer->m_pDP8Peer;
			DoWorkList.apDP8Peers[1] = pDP8PeerHost->m_pDP8Peer;
			
			if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, nonhostcontext.hAsyncOpCompletedEvent))
			{
				DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
				SETTHROW_TESTRESULT(E_FAIL);
			}
			
			ClearDoWorkList(&DoWorkList);
			
			// The create group message should have arrived by now.
			nonhostcontext.fCanGetCreateGroup = FALSE;
			
			// Prevent any weird async op completions.
			nonhostcontext.fAsyncOpCanComplete = FALSE;
			
			// Reset the context.
			nonhostcontext.fAsyncOpCompleted = FALSE;
			CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
			nonhostcontext.hAsyncOpCompletedEvent = NULL;
			
			// Reset the context.
			nonhostcontext.dpnidGroupCreated = 0;
			
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for group creation to be indicated on host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
			DoWorkList.fTrackDoWorkTimes = FALSE;
			DoWorkList.dwNumPeers = 2;
			DoWorkList.apDP8Peers = (PDIRECTPLAY8PEER *) MemAlloc(sizeof(LPVOID) * 2);
			DoWorkList.apDP8Peers[0] = pDP8Peer->m_pDP8Peer;
			DoWorkList.apDP8Peers[1] = pDP8PeerHost->m_pDP8Peer;
			
			if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hostcontext.hGotCreateGroupEvent))
			{
				DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
				SETTHROW_TESTRESULT(E_FAIL);
			}
			
			ClearDoWorkList(&DoWorkList);
			
			// Prevent any weird group creations.
			hostcontext.fCanGetCreateGroup = FALSE;
			
			// Reset the context.
			CloseHandle(hostcontext.hGotCreateGroupEvent);
			hostcontext.hGotCreateGroupEvent = NULL;
			hostcontext.dpnidGroupCreated = 0;
		}
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing peer client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Peer->DP8P_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Technically we should wait to make sure the host saw the client go away
		// as expected but that's more hassle than it's worth.  Just make sure he's
		// gone after Close() has returned.

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing host object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8PeerHost->DP8P_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing hosting DirectPlay8Peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8PeerHost->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Peer->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;

		FINAL_SUCCESS;
	}
	END_TESTCASE

	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_CLOSEHANDLE(hostcontext.hGotCreateGroupEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hGotCreateGroupEvent);
	SAFE_CLOSEHANDLE(hostcontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(hostcontext.hClientCreatedEvent);
	SAFE_RELEASE(pDP8Address);
	ClearDoWorkList(&DoWorkList);
	SAFE_CLOSEHANDLE(hAsyncOpComplete);

	return (sr);
} // ParmVPeerExec_CreateGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/

//==================================================================================
// DpTestStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI DpTestStartTest(IN HANDLE  hLog)
{
	SESSION_TYPE_CALLBACK CallbackSessionInfo;
	DP_GROUPTESTPARAM GroupTestParam;
	DP_ENUMTESTPARAM EnumTestParam;
	DP_SENDTESTPARAM SendTestParam;
	PDP_HOSTINFO pHostInfoMsg = NULL;
	HRESULT hr = DPN_OK;
	IN_ADDR NetsyncServerAddr, HostAddr, FromAddr, LocalAddr;
	WSADATA WSAData;
	HANDLE hPingObject = NULL, hEnumObject = NULL, hClientObject = NULL;
	DWORD dwHostInfoSize, dwReceiveStatus, dwAddressType;
	BOOL fDPlayInitialized = FALSE;
	WORD LowPort, HighPort;
	INT nRet = 0, i;

	// Since we have to be thread safe, serialize entry for now
	EnterCriticalSection(&g_csSerializeAccess);

	g_hLog = hLog;

    // Set XLog info
	xSetOwnerAlias(g_hLog, "tristanj");
    xSetComponent(g_hLog, "Network(S)", "DirectPlay (S-n)");
	xSetFunctionName(g_hLog, "<none>");

	hr = DPlayInitialize(1024 * 200);
	if(hr != DPN_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't init DPlay: 0x%08x", hr);
		goto Exit;
	}

	fDPlayInitialized = TRUE;
//	xLog(g_hLog, XLL_INFO, "DirectPlay layer initialized");

	// Determine our local IP address and how it was obtained
	dwAddressType = XnetGetIpAddress(&LocalAddr);

	xLog(g_hLog, XLL_INFO, "Network info:", inet_ntoa(LocalAddr));

	switch(dwAddressType & (XNET_ADDR_NONE | XNET_ADDR_DHCP | XNET_ADDR_AUTOIP | XNET_ADDR_STATIC | XNET_ADDR_LOOPBACK))
	{
	case XNET_ADDR_NONE:
//		xLog(g_hLog, XLL_FAIL, "    No IP address available!");
		goto Exit;
	case XNET_ADDR_DHCP:
//		xLog(g_hLog, XLL_INFO, "    Local IP (%s) was obtained via DHCP", inet_ntoa(LocalAddr));
		break;
	case XNET_ADDR_AUTOIP:
//		xLog(g_hLog, XLL_INFO, "    Local IP (%s) was obtained via AutoIP", inet_ntoa(LocalAddr));
		break;
	case XNET_ADDR_STATIC:
//		xLog(g_hLog, XLL_INFO, "    Local IP (%s) was statically assigned", inet_ntoa(LocalAddr));
		break;
	case XNET_ADDR_LOOPBACK:
//		xLog(g_hLog, XLL_INFO, "    Local IP (%s) is a loopback address", inet_ntoa(LocalAddr));
		break;
	default:
		xLog(g_hLog, XLL_FAIL, "GetBestIpAddress returned an unknown value: 0x%08x", dwAddressType);
		goto Exit;
	}

	if(dwAddressType & XNET_HAS_GATEWAY)
		xLog(g_hLog, XLL_INFO, "    Gateway is available");
	else
		xLog(g_hLog, XLL_INFO, "    No gateway is avaiable");

	// Run tests that don't require another machine
//	for(i = DPMSG_LOCAL_BASE; i < DPMSG_LOCAL_MAXTEST; i++)
	for(i = DPMSG_PEER_DESTROYPEER; i <= DPMSG_PEER_DESTROYPEER; i++)
	{
		xSetFunctionName(g_hLog, g_LocalTestNames[i]);

		g_LocalTestFunctions[i](hLog);

		xSetFunctionName(g_hLog, "");
	}
//	for(i = 1; TRUE; i++)
//	{
//		xSetFunctionName(g_hLog, "Test func");
//
//		ParmVPeerExec_CreateGroupProblem(hLog);
//		g_LocalTestFunctions[DPMSG_PEER_GETPEERADDRESS](hLog);
//
//		xSetFunctionName(g_hLog, "");
//	}

/*
	// Prepare to find a server that can run the DP tests
	CallbackSessionInfo.byMinClientCount = 2;
	CallbackSessionInfo.byMaxClientCount = 2;
	CallbackSessionInfo.nMinPortCount = 1;
	CallbackSessionInfo.lpszDllName_W = DP_SERVER_DLL_W;
	CallbackSessionInfo.lpszMainFunction_A = DP_SERVER_MAIN_FUNCTION_A;
	CallbackSessionInfo.lpszStartFunction_A = DP_SERVER_START_FUNCTION_A;
	CallbackSessionInfo.lpszStopFunction_A = DP_SERVER_STOP_FUNCTION_A;


	hPingObject = NetsyncFindFirstServer(0, NULL);
	hPingObject ? NetsyncCloseFindServer(hPingObject) : 0;

	// Begin enumerating servers
	hEnumObject = NetsyncFindFirstServer(SESSION_CALLBACK, &CallbackSessionInfo);
	if(hEnumObject == INVALID_HANDLE_VALUE)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't find a server to run DP test");
		goto Exit;
	}

	// Find a specific server
	if(!NetsyncFindServer(hEnumObject, &(NetsyncServerAddr.S_un.S_addr)))
	{
		xLog(g_hLog, XLL_FAIL, "Server refused to run DP test");
		goto Exit;
	}

	// Create a client to communicate with that server
	hClientObject = NetsyncCreateClient(hEnumObject, NetsyncServerAddr.S_un.S_addr, &LowPort, &HighPort);
	if(hClientObject == INVALID_HANDLE_VALUE)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't create Netsync client");
		goto Exit;
	}
	
	dwHostInfoSize = 0;
	pHostInfoMsg = NULL;

	// Wait for information on the other hosts in the session from the Netsync server
	dwReceiveStatus = NetsyncReceiveClientMessage(hClientObject, RECEIVE_TIMEOUT, &(FromAddr.S_un.S_addr), &dwHostInfoSize, (char **) &pHostInfoMsg);

	// If we didn't receive the host information from the server then report failure and end the test
	if((dwReceiveStatus != WAIT_OBJECT_0) || (pHostInfoMsg->dwMessageId != DPMSG_HOSTINFO))
	{
		xLog(g_hLog, XLL_FAIL, "Didn't receive host info from server");
		goto Exit;
	}

	// Set up the host machines address as an IN_ADDR for easy conversion to a string
	HostAddr.S_un.S_addr = pHostInfoMsg->dwHostAddr;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run simple server/client tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	if(!SyncWithOtherTesters(hLog, hClientObject, SIMPLE_SERVER_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	(HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr) ? 
		DirectPlayExec_SimpleServer(hLog, pHostInfoMsg, LowPort) : 
		DirectPlayExec_SimpleClient(hLog, pHostInfoMsg, LowPort);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run simple peer tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	if(!SyncWithOtherTesters(hLog, hClientObject, SIMPLE_HOST_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	(HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr) ? 
		DirectPlayExec_SimpleHost(hLog, pHostInfoMsg, LowPort) :
		DirectPlayExec_SimpleConnect(hLog, pHostInfoMsg, LowPort);

/*
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run basic client/server enum tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_ENUM_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.fClientServer = TRUE;
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_SimpleAll(g_hLog, &EnumTestParam);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run basic peer enum tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_ENUM_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_SimpleAll(g_hLog, &EnumTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server enum reject tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CSREJECT_ENUM_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.fClientServer = TRUE;
	EnumTestParam.fReject = TRUE;
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_SimpleAll(g_hLog, &EnumTestParam);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer enum reject tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEERREJECT_ENUM_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.fReject = TRUE;
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_SimpleAll(g_hLog, &EnumTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server app data enum tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CSAPP_ENUM_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.fClientServer = TRUE;
	EnumTestParam.dwAppDescDataSize = 100;
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_SimpleAll(g_hLog, &EnumTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer app data enum tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEERAPP_ENUM_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.dwAppDescDataSize = 100;
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_SimpleAll(g_hLog, &EnumTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server user data enum tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CSUSER_ENUM_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.fClientServer = TRUE;
	EnumTestParam.dwUserEnumDataSize = 100;
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_SimpleAll(g_hLog, &EnumTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server user data enum tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEERUSER_ENUM_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.dwUserEnumDataSize = 100;
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_SimpleAll(g_hLog, &EnumTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run enum validation tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, ENUM_VALIDATE_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&EnumTestParam, 0, sizeof(DP_ENUMTESTPARAM));
	EnumTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	EnumTestParam.hNetsyncObject = hClientObject;
	strcpy(EnumTestParam.szServerIPAddr, inet_ntoa(HostAddr));
	
	EnumsExec_Validate(g_hLog, &EnumTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer group tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_GROUP_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&GroupTestParam, 0, sizeof(DP_GROUPTESTPARAM));
	GroupTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	GroupTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	GroupTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	GroupTestParam.hNetsyncObject = hClientObject;

	GroupsExec_SimplePeer(g_hLog, &GroupTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server group tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_GROUP_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&GroupTestParam, 0, sizeof(DP_GROUPTESTPARAM));
	GroupTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	GroupTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	GroupTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	GroupTestParam.hNetsyncObject = hClientObject;

	GroupsExec_SimpleCS(g_hLog, &GroupTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer sends tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimplePeer(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer sends tests to AllPlayersGroup
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.fSendToAllPlayersGroup = TRUE;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimplePeer(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer sends tests, high priority
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_PRIORITY_HIGH;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimplePeer(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer sends tests to AllPlayersGroup, high priority
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.fSendToAllPlayersGroup = TRUE;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_PRIORITY_HIGH;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimplePeer(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer sends tests, low priority
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_PRIORITY_LOW;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimplePeer(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer sends tests to AllPlayersGroup, low priority
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.fSendToAllPlayersGroup = TRUE;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_PRIORITY_LOW;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimplePeer(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer sends tests, complete on process
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_COMPLETEONPROCESS | DPNSEND_GUARANTEED;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimplePeer(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run peer sends tests to AllPlayersGroup, complete on process
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, PEER_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.fSendToAllPlayersGroup = TRUE;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_COMPLETEONPROCESS | DPNSEND_GUARANTEED;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimplePeer(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server sends tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimpleCS(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server sends tests to AllPlayersGroup
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.fSendToAllPlayersGroup = TRUE;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimpleCS(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server sends tests, high priority
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_PRIORITY_HIGH;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimpleCS(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server sends tests to AllPlayersGroup, high priority
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.fSendToAllPlayersGroup = TRUE;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_PRIORITY_HIGH;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimpleCS(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server sends tests, low priority
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_PRIORITY_LOW;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimpleCS(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server sends tests to AllPlayersGroup, low priority
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.fSendToAllPlayersGroup = TRUE;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_PRIORITY_LOW;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimpleCS(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server sends tests, complete on process
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_COMPLETEONPROCESS | DPNSEND_GUARANTEED;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimpleCS(g_hLog, &SendTestParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Run client/server sends tests to AllPlayersGroup, complete on process
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if(!SyncWithOtherTesters(hLog, hClientObject, CS_SENDS_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	memset(&SendTestParam, 0, sizeof(DP_SENDTESTPARAM));
	SendTestParam.dwHostAddr = HostAddr.S_un.S_addr;
	SendTestParam.fSendToAllPlayersGroup = TRUE;
	SendTestParam.dwMsgSize = 256;
	SendTestParam.dwNumMachines = pHostInfoMsg->dwTotalMachines;
	SendTestParam.dwNumMsgs = 1000;
	SendTestParam.fHostTest = (HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr);
	SendTestParam.fSendToAllPlayersGroup = FALSE;
	SendTestParam.dwFlags = DPNSEND_COMPLETEONPROCESS | DPNSEND_GUARANTEED;
	SendTestParam.hNetsyncObject = hClientObject;

	SendsExec_SimpleCS(g_hLog, &SendTestParam);
*/

Exit:

/*
	hClientObject ? NetsyncCloseClient(hClientObject) : 0;
	hEnumObject ? NetsyncCloseFindServer(hEnumObject) : 0;
	pHostInfoMsg ? NetsyncFreeMessage((char *) pHostInfoMsg) : 0;
*/

	if(fDPlayInitialized)
	{
		hr = DPlayCleanup();
		if(hr != DPN_OK)
			xLog(g_hLog, XLL_FAIL, "Couldn't cleanup DPlay: 0x%08x", hr);
//		else
//			xLog(g_hLog, XLL_INFO, "DirectPlay layer cleaned-up");
	}

	g_hLog = INVALID_HANDLE_VALUE;

	// Allow other threads to enter now
	LeaveCriticalSection(&g_csSerializeAccess);

}            

//==================================================================================
// DpTestEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI DpTestEndTest()
{
	// We already cleaned up everything at the end of StartTest
}

//==================================================================================
// Helper functions
//==================================================================================

//==================================================================================
// DPTEST_TRACE
//----------------------------------------------------------------------------------
//
// Description: Wrapper for the logging stuff that was imported from testnet
//
// Arguments:
//	HANDLE hLog							Indicates level to log message as
//	LPCTSTR szFormat					Buffer that contains the format string
//	INT nNumArgs						Indicates the number of variable arguments present
//
// Returns: nothing
//==================================================================================
void DPTEST_TRACE(HANDLE hLog, LPSTR szFormat, INT nNumArgs ...) {
	CHAR szBuffer[1024] = "";
	va_list pArgs; 

	va_start(pArgs, nNumArgs);
	
	if(!hLog)
	{
		vsprintf(szBuffer, szFormat, pArgs);
//		DbgPrint(szBuffer);
	}
	else
		xLog_va(hLog, XLL_INFO, szFormat, pArgs);
	
	va_end(pArgs);
}

//==================================================================================
// DPTEST_FAIL
//----------------------------------------------------------------------------------
//
// Description: Wrapper for the logging stuff that was imported from testnet
//
// Arguments:
//	HANDLE hLog							Indicates level to log message as
//	LPCTSTR szFormat					Buffer that contains the format string
//	INT nNumArgs						Indicates the number of variable arguments present
//
// Returns: nothing
//==================================================================================
void DPTEST_FAIL(HANDLE hLog, LPSTR szFormat, INT nNumArgs ...) {
	CHAR szBuffer[1024] = "";
	va_list pArgs; 

	va_start(pArgs, nNumArgs);
	
	if(!hLog)
	{
		vsprintf(szBuffer, szFormat, pArgs);
//		DbgPrint(szBuffer);
	}
	else
		xLog_va(hLog, XLL_FAIL, szFormat, pArgs);
	
	va_end(pArgs);
}

} // namespace DPlayCoreNamespace

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dptest )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( dptest )
    EXPORT_TABLE_ENTRY( "StartTest", DpTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DpTestEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", DpTestDllMain )
END_EXPORT_TABLE( dptest )
