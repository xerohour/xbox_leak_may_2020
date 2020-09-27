//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include "dpstress.h"

extern DWORD g_dwDPStressBytesAllocated = 0;

using namespace DPlayStressNamespace;

namespace DPlayStressNamespace {

//==================================================================================
// Globals
//==================================================================================

// Test specific globals
static CRITICAL_SECTION g_csSerializeAccess;
static HANDLE g_hLog = INVALID_HANDLE_VALUE;
static HANDLE g_hHeap = NULL;

#undef DEBUG_SECTION
#define DEBUG_SECTION	"MemAlloc()"
//==================================================================================
// MemAlloc
//----------------------------------------------------------------------------------
//
// Description: Private heap allocation function
//
// Arguments:
//	DWORD		dwSize		Size of buffer to allocate from the private heap
//
// Returns: Valid pointer to newly allocated memory, NULL otherwise
//==================================================================================
LPVOID MemAlloc(DWORD dwSize)
{
	LPVOID pBuffer = NULL;
	DWORD dwAllocatedSize = 0;

	if(!g_hHeap)
	{
//		DbgPrint("DirectPlay test's private heap hasn't been created!");
		return NULL;
	}

	pBuffer = HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, dwSize);

	// Determine the size that was allocated and increment the global tracking variable
	dwAllocatedSize = HeapSize(g_hHeap, 0, pBuffer);
	if(dwAllocatedSize == -1)
	{
//		DbgPrint("Couldn't get size of allocated buffer 0x%08x", pBuffer);
		return NULL;
	}

	g_dwDPStressBytesAllocated += dwAllocatedSize;

	return pBuffer;
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"MemFree()"
//==================================================================================
// MemFree
//----------------------------------------------------------------------------------
//
// Description: Private heap deallocation function
//
// Arguments:
//	LPVOID		pBuffer		Pointer to buffer to be released
//
// Returns: TRUE if the buffer was deallocated, FALSE otherwise
//==================================================================================
BOOL MemFree(LPVOID pBuffer)
{
	DWORD dwAllocatedSize = 0;

	if(!g_hHeap)
	{
//		DbgPrint("DirectPlay test's private heap hasn't been created!");
		return FALSE;
	}

	// Determine the size that was allocated and increment the global tracking variable
	dwAllocatedSize = HeapSize(g_hHeap, 0, pBuffer);
	if(dwAllocatedSize == -1)
	{
//		DbgPrint("Couldn't get size of buffer being freed 0x%08x", pBuffer);
		return FALSE;
	}

	g_dwDPStressBytesAllocated -= dwAllocatedSize;

	return HeapFree(g_hHeap, 0, pBuffer);
}

//==================================================================================
// DpPeerStressDllMain
//----------------------------------------------------------------------------------
//
// Description: DLL entry for peer stress test
//
// Arguments:
//	HINSTANCE	hInstance		Handle to module
//	DWORD		dwReason		Indicates the reason for calling the function
//	LPVOID		lpContext		reserved
// Returns:
//	TRUE on success
//==================================================================================
BOOL WINAPI DpPeerStressDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XnetInitialize(NULL, TRUE);
		InitializeCriticalSection(&g_csSerializeAccess);
		g_hHeap = HeapCreate(0,0,0);
		break;
	case DLL_PROCESS_DETACH:
		XnetCleanup();
		DeleteCriticalSection(&g_csSerializeAccess);
		HeapDestroy(g_hHeap);
		break;
	default:
		break;
	}

    return TRUE;
}

//==================================================================================
// DpPeerStressStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point for peer to peer stress
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI DpPeerStressStartTest(IN HANDLE  hLog)
{
	SESSION_TYPE_CALLBACK CallbackSessionInfo;
	HRESULT hr = DPN_OK;
	IN_ADDR NetsyncServerAddr;
	WSADATA WSAData;
	INT nRet = 0;

	// Since we have to be thread safe, serialize entry for now
	EnterCriticalSection(&g_csSerializeAccess);

	g_hLog = hLog;

    // Set XLog info
	xSetOwnerAlias(g_hLog, "tristanj");
    xSetComponent(g_hLog, "Network(S)", "DirectPlay (S-n)");
	xSetFunctionName(g_hLog, "Stress");

    // Init winsock
    if(nRet = WSAStartup(MAKEWORD(2, 2), &WSAData))
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't init winsock: %d", nRet);
		goto Exit;
	}

//	xLog(g_hLog, XLL_INFO, "Winsock layer initialized");

	// Prepare to find a server that can run the DP tests
	CallbackSessionInfo.byMinClientCount = 1;
	CallbackSessionInfo.byMaxClientCount = 8;
	CallbackSessionInfo.nMinPortCount = 2;
	CallbackSessionInfo.lpszDllName_W = DP_PEER_STRESS_DLL_W;
	CallbackSessionInfo.lpszMainFunction_A = DP_PEER_STRESS_MAIN_FUNCTION_A;
	CallbackSessionInfo.lpszStartFunction_A = DP_PEER_STRESS_START_FUNCTION_A;
	CallbackSessionInfo.lpszStopFunction_A = DP_PEER_STRESS_STOP_FUNCTION_A;

	BEGIN_STRESS_LOOP(hLog)
	{	
		// For each loop of the test, find out if this Xbox is on the Corpnet or Internet
		// and set the address of the netsync server for the rest of this iteration.
		// In this way, it would be possible to take an
		
		if(FindServerIP(hLog, CORPNET_NETSYNC_NAME, &NetsyncServerAddr))
		{
			xLog(g_hLog, XLL_INFO, "Found the Corpnet Netsync server at %s", inet_ntoa(NetsyncServerAddr));
		}
		else if(FindServerIP(hLog, INTERNET_NETSYNC_NAME, &NetsyncServerAddr))
		{
			xLog(g_hLog, XLL_INFO, "Found the Internet Netsync server at %s", inet_ntoa(NetsyncServerAddr));
		}
		else
		{
			xLog(g_hLog, XLL_WARN, "Both the Corpnet and Internet Netsync servers are unavailable at this time");
			WAIT10_AND_LOOP(hLog);
		}

//		xLog(g_hLog, XLL_INFO, "Querying Netsync for DirectPlay host information...");

		// Query the Netsync server for host information
		GET_HOSTINFO(hLog, NetsyncServerAddr, &CallbackSessionInfo);

//		xLog(g_hLog, XLL_INFO, "Determining local address information...");

		// Determine local address information
		GET_LOCALINFO(hLog);
		
//		xLog(g_hLog, XLL_INFO, "Calling DirectPlay peer stress function...");

		// Run the peer stress function
		RUN_PEERSTRESS(hLog);
	}
	END_STRESS_LOOP(hLog)

Exit:


	// Clean up winsock
	xLog(g_hLog, XLL_INFO, "Cleaning up Winsock layer");
	WSACleanup();

	g_hLog = INVALID_HANDLE_VALUE;

	// Allow other threads to enter now
	LeaveCriticalSection(&g_csSerializeAccess);

}            

//==================================================================================
// DpPeerStressEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI DpPeerStressEndTest()
{
	// We already cleaned up everything at the end of StartTest
}

//==================================================================================
// DpCSStressDllMain
//----------------------------------------------------------------------------------
//
// Description: DLL entry for client/server stress test
//
// Arguments:
//	HINSTANCE	hInstance		Handle to module
//	DWORD		dwReason		Indicates the reason for calling the function
//	LPVOID		lpContext		reserved
// Returns:
//	TRUE on success
//==================================================================================
BOOL WINAPI DpCSStressDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XnetInitialize(NULL, TRUE);
		InitializeCriticalSection(&g_csSerializeAccess);
		break;
	case DLL_PROCESS_DETACH:
		XnetCleanup();
		DeleteCriticalSection(&g_csSerializeAccess);
		break;
	default:
		break;
	}

    return TRUE;
}

/*
//==================================================================================
// DpCSStressStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point for client/server stress
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI DpCSStressStartTest(IN HANDLE  hLog)
{
	SESSION_TYPE_CALLBACK CallbackSessionInfo;
	PDP_HOSTINFO pHostInfoMsg = NULL;
	HRESULT hr = DPN_OK;
	IN_ADDR NetsyncServerAddr, HostAddr, FromAddr, LocalAddr;
	WSADATA WSAData;
	HANDLE hEnumObject = NULL, hClientObject = NULL;
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
	xSetFunctionName(g_hLog, "Stress");

    // Init winsock
    if(nRet = WSAStartup(MAKEWORD(2, 2), &WSAData))
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't init winsock: %d", nRet);
		goto Exit;
	}

	xLog(g_hLog, XLL_INFO, "Winsock layer initialized");

	hr = DPlayInitialize(1024 * 200);
	if(hr != DPN_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't init DPlay: 0x%08x", hr);
		goto Exit;
	}

	fDPlayInitialized = TRUE;
	xLog(g_hLog, XLL_INFO, "DirectPlay layer initialized");

	// Determine our local IP address and how it was obtained
	dwAddressType = XnetGetIpAddress(&LocalAddr);

	xLog(g_hLog, XLL_INFO, "Network info:", inet_ntoa(LocalAddr));

	switch(dwAddressType & (XNET_ADDR_NONE | XNET_ADDR_DHCP | XNET_ADDR_AUTOIP | XNET_ADDR_STATIC | XNET_ADDR_LOOPBACK))
	{
	case XNET_ADDR_NONE:
		xLog(g_hLog, XLL_FAIL, "    No IP address available!");
		goto Exit;
	case XNET_ADDR_DHCP:
		xLog(g_hLog, XLL_INFO, "    Local IP (%s) was obtained via DHCP", inet_ntoa(LocalAddr));
		break;
	case XNET_ADDR_AUTOIP:
		xLog(g_hLog, XLL_INFO, "    Local IP (%s) was obtained via AutoIP", inet_ntoa(LocalAddr));
		break;
	case XNET_ADDR_STATIC:
		xLog(g_hLog, XLL_INFO, "    Local IP (%s) was statically assigned", inet_ntoa(LocalAddr));
		break;
	case XNET_ADDR_LOOPBACK:
		xLog(g_hLog, XLL_INFO, "    Local IP (%s) is a loopback address", inet_ntoa(LocalAddr));
		break;
	default:
		xLog(g_hLog, XLL_FAIL, "GetBestIpAddress returned an unknown value: 0x%08x", dwAddressType);
		goto Exit;
	}

	if(dwAddressType & XNET_HAS_GATEWAY)
		xLog(g_hLog, XLL_INFO, "    Gateway is available");
	else
		xLog(g_hLog, XLL_INFO, "    No gateway is avaiable");


	// Prepare to find a server that can run the DP tests
	CallbackSessionInfo.byMinClientCount = 2;
	CallbackSessionInfo.byMaxClientCount = 100;
	CallbackSessionInfo.nMinPortCount = 2;
	CallbackSessionInfo.lpszDllName_W = DP_PEER_STRESS_DLL_W;
	CallbackSessionInfo.lpszMainFunction_A = DP_PEER_STRESS_MAIN_FUNCTION_A;
	CallbackSessionInfo.lpszStartFunction_A = DP_PEER_STRESS_START_FUNCTION_A;
	CallbackSessionInfo.lpszStopFunction_A = DP_PEER_STRESS_STOP_FUNCTION_A;

	// BUGBUG - determine server address

	// Begin enumerating servers
	hEnumObject = NetsyncQueryServer(NetsyncServerAddr.S_un.S_addr, SESSION_CALLBACK, &CallbackSessionInfo);
	if(hEnumObject == INVALID_HANDLE_VALUE)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't find a server to run DPSTRESS test");
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
	// Run simple peer tests
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	if(!SyncWithOtherTesters(hLog, hClientObject, SIMPLE_HOST_EVENT, NULL, 0, NULL, 0))
		goto Exit;
	
	(HostAddr.S_un.S_addr == LocalAddr.S_un.S_addr) ? 
		DirectPlayExec_SimpleHost(hLog, pHostInfoMsg, LowPort) :
		DirectPlayExec_SimpleConnect(hLog, pHostInfoMsg, LowPort);

Exit:

	hClientObject ? NetsyncCloseClient(hClientObject) : 0;
	hEnumObject ? NetsyncCloseFindServer(hEnumObject) : 0;
	pHostInfoMsg ? NetsyncFreeMessage((char *) pHostInfoMsg) : 0;

	if(fDPlayInitialized)
	{
		hr = DPlayCleanup();
		if(hr != DPN_OK)
			xLog(g_hLog, XLL_FAIL, "Couldn't cleanup DPlay: 0x%08x", hr);
		else
			xLog(g_hLog, XLL_INFO, "DirectPlay layer cleaned-up");
	}


	// Clean up winsock
	xLog(g_hLog, XLL_INFO, "Cleaning up Winsock layer");
	WSACleanup();

	g_hLog = INVALID_HANDLE_VALUE;

	// Allow other threads to enter now
	LeaveCriticalSection(&g_csSerializeAccess);

}            

//==================================================================================
// DpCSStressEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI DpCSStressEndTest()
{
	// We already cleaned up everything at the end of StartTest
}
*/

} // namespace DPlayStressNamespace


// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dppeerstress )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( dppeerstress )
    EXPORT_TABLE_ENTRY( "StartTest", DpPeerStressStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DpPeerStressEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", DpPeerStressDllMain )
END_EXPORT_TABLE( dppeerstress )

/*
// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dpcsstress )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( dpcsstress )
    EXPORT_TABLE_ENTRY( "StartTest", DpCSStressStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DpCSStressEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", DpCSStressDllMain )
END_EXPORT_TABLE( dpcsstress )
*/