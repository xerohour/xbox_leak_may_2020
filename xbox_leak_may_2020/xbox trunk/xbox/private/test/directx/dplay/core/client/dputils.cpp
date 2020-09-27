//==================================================================================
// Includes
//==================================================================================
#include "dptest.h"
#include "macros.h"
#include "wrappeer.h"
#include "wrapserver.h"
#include "wrapclient.h"

using namespace DPlayCoreNamespace;

namespace DPlayCoreNamespace {

extern HANDLE g_hHeap;

#undef DEBUG_SECTION
#define DEBUG_SECTION	"MemAlloc()"
//==================================================================================
// MemAlloc
//----------------------------------------------------------------------------------
//
// MemAlloc: Private heap allocation function
//
// Arguments:
//	DWORD		dwSize		Size of buffer to allocate from the private heap
//
// Returns: Valid pointer to newly allocated memory, NULL otherwise
//==================================================================================
LPVOID MemAlloc(DWORD dwSize)
{
	if(!g_hHeap)
	{
//		DbgPrint("DirectPlay test's private heap hasn't been created!");
		return NULL;
	}

	return HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, dwSize);
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
	if(!g_hHeap)
	{
//		DbgPrint("DirectPlay test's private heap hasn't been created!");
		return FALSE;
	}

	return HeapFree(g_hHeap, 0, pBuffer);
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"SyncWithOtherTesters()"
//==================================================================================
// SyncWithOtherTesters
//----------------------------------------------------------------------------------
//
// Description: Function that establishes sync with other clients and exchanges data with them
//
// Arguments:
//	HANDLE		hLog
//	HANDLE		hNetsyncObject
//	LPCSTR		szEventName
//	PBYTE		pInputData
//	DWORD		dwInputDataSize
//	PBYTE		pOutputData
//	DWORD		dwOutputDataSize
// Returns: TRUE if sync was established, FALSE otherwise
//==================================================================================
BOOL SyncWithOtherTesters(HANDLE hLog, HANDLE hNetsyncObject, LPCSTR szEventName, PBYTE pInputData, DWORD dwInputDataSize, PBYTE pOutputData, DWORD *pdwOutputDataSize)
{
	PDP_SYNCWAIT pSyncWaitMsg = NULL;
	PDP_SYNCSIGNAL pSyncSignalMsg = NULL;
	IN_ADDR FromAddr;
	DWORD dwReceiveStatus, dwReceivedDataSize = 0;
	BOOL fRet = TRUE;

	if(dwInputDataSize > MAX_SYNC_DATASIZE)
	{
		xLog(hLog, XLL_FAIL, "Trying to sync with data size above maximum (%u > %u)", dwInputDataSize, MAX_SYNC_DATASIZE);
		fRet = FALSE;
		goto Exit;
	}

	if(dwInputDataSize && !pInputData)
	{
		xLog(hLog, XLL_FAIL, "Input data size %u is greater than 0, but no input buffer provided", dwInputDataSize);
		fRet = FALSE;
		goto Exit;
	}

	// Allocate the wait message
	pSyncWaitMsg = (PDP_SYNCWAIT) MemAlloc(sizeof(DP_SYNCWAIT) + dwInputDataSize);
				
	// Populate the wait message
	pSyncWaitMsg->dwMessageId = DPMSG_SYNCWAIT;
	pSyncWaitMsg->dwMinMachinesRequired = 0;
	strcpy(pSyncWaitMsg->szSyncDataName, szEventName);
	
	// Copy the input sync data into the wait message
	(pSyncWaitMsg->dwSyncDataSize = dwInputDataSize) ?
		memcpy((BYTE *) pSyncWaitMsg + sizeof(DP_SYNCWAIT), pInputData, dwInputDataSize) : 0;
				
	// TODO - No retries yet.  If we miss a single UDP packet we lose sync and the whole test is screwed
	// Acceptable for now... at least we'll get an error message when we don't receive the signal
	
	// Send the wait message to the server
	NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(DP_SYNCWAIT) + dwInputDataSize, (char *) pSyncWaitMsg);

	dwReceivedDataSize = 0;

	// Wait 2 minutes to sync with other testers
	dwReceiveStatus = NetsyncReceiveClientMessage(hNetsyncObject,
		120000,
		&(FromAddr.S_un.S_addr), 
		&dwReceivedDataSize, 
		(char **) &pSyncSignalMsg);
				
	// If we didn't receive a sync signal from the server within 2 minutes
	if((dwReceiveStatus != WAIT_OBJECT_0) || (pSyncSignalMsg->dwMessageId != DPMSG_SYNCSIGNAL))
	{
		xLog(hLog, XLL_FAIL, "Didn't synchronize with other testers within 2 minutes", 0);
		
		fRet = FALSE;
		goto Exit;
	}
	
	xLog(hLog, XLL_INFO, "Synced on event: %s", pSyncWaitMsg->szSyncDataName);

	if(pOutputData)
	{
		if(pSyncSignalMsg->dwSyncDataSize > *pdwOutputDataSize)
		{
			xLog(hLog, XLL_WARN, "Received more data than output buffer can hold (%u > %u)",
				pSyncSignalMsg->dwSyncDataSize, *pdwOutputDataSize);
			
			*pdwOutputDataSize = 0;
			goto Exit;
		}
		
		// Copy the signal message data into the output buffer
		(*pdwOutputDataSize = pSyncSignalMsg->dwSyncDataSize) ? 
			memcpy(pOutputData, (BYTE *) pSyncSignalMsg + sizeof(DP_SYNCSIGNAL), pSyncSignalMsg->dwSyncDataSize) : 0;			
	}

Exit:

	pSyncWaitMsg ? MemFree(pSyncWaitMsg) : 0;
	pSyncSignalMsg ? NetsyncFreeMessage((char *) pSyncSignalMsg) : 0;

	return fRet;
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"CreateHostAndWaitForPlayers"
//==================================================================================
// CreateHostAndWaitForPlayers
//----------------------------------------------------------------------------------
//
// Description: Creates a host at and waits for a given number of users to be created
//
// Arguments:
//	HANDLE						hLog				Handle to logging subsystem
//	PWRAPDP8PEER				*ppDP8Peer			Pointer filled with host peer
//	PBASESESSIONCREATECONTEXT	pContext			Callback context
//	DPN_APPLICATION_DESC		*pdpnad				Application descriptor of host session
//	PFNDPNMESSAGEHANDLER		pfnMessageHandler	Callback function
//	PDPNID						*ppPlayerList		Pointer filled with player ID's
//	DWORD						dwPlayersExpected	Number of players expected
//	DWORD						dwTimeout			Milliseconds before timing out operation
//
//
// Returns: TRUE if the session is connected to and the expected number of players is presen
//			FALSE otherwise
//==================================================================================
BOOL CreateHostAndWaitForPlayers(HANDLE hLog, PWRAPDP8PEER *ppDP8Peer, PBASESESSIONCREATECONTEXT pContext, DPN_APPLICATION_DESC *pdpnad, PFNDPNMESSAGEHANDLER pfnMessageHandler, PDPNID *ppPlayerList, DWORD dwPlayersExpected, DWORD dwTimeout)
{
	PDIRECTPLAY8ADDRESS		pDP8DeviceAddress = NULL;
	HRESULT					hr;
	DWORD					dwPort, dwPlayersActual;
	BOOL					fRet = TRUE;

	// Create the peer wrapper
	*ppDP8Peer = new CWrapDP8Peer(hLog);
	if (*ppDP8Peer == NULL)
	{
		DPTEST_TRACE(hLog, "Couldn't create peer wrapper object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create the DirectPlay peer object
	hr = (*ppDP8Peer)->CoCreate();
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate DirectPlay8Peer object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create an address to host with
	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8DeviceAddress, NULL);
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate first DirectPlay8Address object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Set up the context
	pContext->dwPlayersCreated = 0;
	pContext->dwPlayersExpected = dwPlayersExpected;
	pContext->fPlayersHaveLeft = FALSE;
	pContext->hbsccAllPlayersCreated = CreateEvent(NULL, FALSE, FALSE, NULL);

	if(!pContext->hbsccAllPlayersCreated)
	{
		DPTEST_TRACE(hLog, "Couldn't create AllPlayersCreated event!", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Initialized the host
	hr = (*ppDP8Peer)->DP8P_Initialize(pContext, pfnMessageHandler, 0);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Initializing with context, 0 flags failed!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Always default to this port.  Allows netmon parsers to detect DirectPlay packets
	dwPort = 2302;
	
	if(pDP8DeviceAddress->AddComponent(DPNA_KEY_PORT, &dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD) != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't set port for host", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Host the session
	hr = (*ppDP8Peer)->DP8P_Host(pdpnad, &pDP8DeviceAddress, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Hosting with 1 device failed!", 0);
		fRet = FALSE;
		goto EXIT;
	} // end if (couldn't host)

	// Wait for all the players to be connected
	if(WaitForSingleObject(pContext->hbsccAllPlayersCreated, 120000) != WAIT_OBJECT_0)
	{
		DPTEST_TRACE(hLog, "Players weren't created within 2 minutes", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Get the number of players created
	dwPlayersActual = 0;
	(*ppDP8Peer)->DP8P_EnumPlayersAndGroups(NULL, &dwPlayersActual, DPNENUM_PLAYERS);
	if(dwPlayersActual != pContext->dwPlayersExpected)
	{
		DPTEST_TRACE(hLog, "Unexpected number of players in session: (%u != %u)", 2, dwPlayersActual, dwPlayersExpected);
		fRet = FALSE;
		goto EXIT;
	}

	// Allocate a buffer for the players
	*ppPlayerList = (DPNID *) MemAlloc(sizeof(DPNID) * dwPlayersActual);
	if(!*ppPlayerList)
	{
		DPTEST_TRACE(hLog, "Couldn't allocate player ID buffer", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Get the players that were created
	hr = (*ppDP8Peer)->DP8P_EnumPlayersAndGroups(*ppPlayerList, &dwPlayersActual, DPNENUM_PLAYERS);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Error enumerating players from session: 0x%08x", 1, hr);
		fRet = FALSE;
		goto EXIT;
	}

	DPTEST_TRACE(hLog, "Session successfully hosted and all %u players created", 1, pContext->dwPlayersExpected);

EXIT:

	// Release the address
	if(pDP8DeviceAddress)
	{
		pDP8DeviceAddress->Release();
		pDP8DeviceAddress = NULL;
	}

	return fRet;
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnectNonHostAndWaitForPlayers()"
//==================================================================================
// ConnectNonHostAndWaitForPlayers
//----------------------------------------------------------------------------------
//
// Description: Connects to a host at a specified address and waits for a
//				given number of users to be created
//
// Arguments:
//	HANDLE						hLog				Handle to logging subsystem
//	PWRAPDP8PEER				*ppDP8Peer			Pointer filled with non-host peer
//	PBASESESSIONCREATECONTEXT	pContext			Callback context
//	DWORD						dwHostAddr			DWORD form of host's IP address
//	DPN_APPLICATION_DESC		*pdpnad				Application descriptor of host session
//	PFNDPNMESSAGEHANDLER		pfnMessageHandler	Callback function
//	PDPNID						*ppPlayerList		Pointer filled with player ID's
//	DWORD						dwPlayersExpected	Number of players expected
//	DWORD						dwTimeout			Milliseconds before timing out operation
//
//
// Returns: TRUE if the session is connected to and the expected number of players is presen
//			FALSE otherwise
//==================================================================================
BOOL ConnectNonHostAndWaitForPlayers(HANDLE hLog, PWRAPDP8PEER *ppDP8Peer, PBASESESSIONCREATECONTEXT pContext, DWORD dwHostAddr, DPN_APPLICATION_DESC *pdpnad, PFNDPNMESSAGEHANDLER pfnMessageHandler, PDPNID *ppPlayerList, DWORD dwPlayersExpected, DWORD dwTimeout)
{
	PDIRECTPLAY8ADDRESS		pDP8HostAddress = NULL, pDP8DeviceAddress = NULL;
	SOCKADDR_IN				HostAddr;
	HRESULT					hr;
	DPNHANDLE				hAsync = NULL;
	DWORD					dwPort, dwPlayersActual;
	BOOL					fRet = TRUE;

	// Create the peer wrapper
	*ppDP8Peer = new CWrapDP8Peer(hLog);
	if (*ppDP8Peer == NULL)
	{
		DPTEST_TRACE(hLog, "Couldn't create peer wrapper object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create the DirectPlay peer object
	hr = (*ppDP8Peer)->CoCreate();
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate DirectPlay8Peer object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create an address to host with
	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8HostAddress, NULL);
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate first DirectPlay8Address object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create an address for the device to connect with
	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8DeviceAddress, NULL);
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate device DirectPlay8Address object!", 0);
		fRet = FALSE;
		goto EXIT;
	} // end if (CoCreate failed)


	// Set up the context
	pContext->dwPlayersCreated = 0;
	pContext->dwPlayersExpected = dwPlayersExpected;
	pContext->fPlayersHaveLeft = FALSE;
	pContext->hbsccAllPlayersCreated = CreateEvent(NULL, FALSE, FALSE, NULL);

	if(!pContext->hbsccAllPlayersCreated)
	{
		DPTEST_TRACE(hLog, "Couldn't create AllPlayersCreated event!", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Initialized the host
	hr = (*ppDP8Peer)->DP8P_Initialize(pContext, pfnMessageHandler, 0);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Initializing with context, 0 flags failed!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Build a socket address for the session on the host machine
	HostAddr.sin_family = AF_INET;
	HostAddr.sin_port = htons(2302);
	HostAddr.sin_addr.S_un.S_addr = dwHostAddr;

	// Set that address
	if(pDP8HostAddress->BuildFromSockAddr((SOCKADDR *) &HostAddr) != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't set port for host", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Host the session
	hr = (*ppDP8Peer)->DP8P_Connect(pdpnad, pDP8HostAddress, pDP8DeviceAddress, NULL, NULL, NULL, 0, NULL, NULL, &hAsync, 0);
	if (hr != DPNERR_PENDING)
	{
		DPTEST_TRACE(hLog, "Connecting to host failed: 0x%08x", 1, hr);
		fRet = FALSE;
		goto EXIT;
	} // end if (couldn't host)


	// Wait for all the players to be connected
	if(WaitForSingleObject(pContext->hbsccAllPlayersCreated, 120000) != WAIT_OBJECT_0)
	{
		DPTEST_TRACE(hLog, "Players weren't created within 2 minutes", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Get the number of players created
	dwPlayersActual = 0;
	(*ppDP8Peer)->DP8P_EnumPlayersAndGroups(NULL, &dwPlayersActual, DPNENUM_PLAYERS);
	if(dwPlayersActual != pContext->dwPlayersExpected)
	{
		DPTEST_TRACE(hLog, "Unexpected number of players in session: (%u != %u)", 2, dwPlayersActual, dwPlayersExpected);
		fRet = FALSE;
		goto EXIT;
	}

	// Allocate a buffer for the players
	*ppPlayerList = (DPNID *) MemAlloc(sizeof(DPNID) * dwPlayersActual);
	if(!*ppPlayerList)
	{
		DPTEST_TRACE(hLog, "Couldn't allocate player ID buffer", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Get the players that were created
	hr = (*ppDP8Peer)->DP8P_EnumPlayersAndGroups(*ppPlayerList, &dwPlayersActual, DPNENUM_PLAYERS);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Error enumerating players from session: 0x%08x", 1, hr);
		delete (*ppDP8Peer);
		*ppDP8Peer = NULL;
		fRet = FALSE;
		goto EXIT;
	}

	DPTEST_TRACE(hLog, "Session successfully connected and all %u players created", 1, pContext->dwPlayersExpected);

EXIT:

	// Release the address
	if(pDP8HostAddress)
	{
		pDP8HostAddress->Release();
		pDP8HostAddress = NULL;
	}

	// Release the address
	if(pDP8DeviceAddress)
	{
		pDP8DeviceAddress->Release();
		pDP8DeviceAddress = NULL;
	}

	return fRet;
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#undef DEBUG_SECTION
#define DEBUG_SECTION	"CreateServerAndWaitForPlayers"
//==================================================================================
// CreateServerAndWaitForPlayers
//----------------------------------------------------------------------------------
//
// Description: Creates a host at and waits for a given number of users to be created
//
// Arguments:
//	HANDLE						hLog				Handle to logging subsystem
//	PWRAPDP8PEER				*ppDP8Server		Pointer filled with server
//	PBASESESSIONCREATECONTEXT	pContext			Callback context
//	DPN_APPLICATION_DESC		*pdpnad				Application descriptor of host session
//	PFNDPNMESSAGEHANDLER		pfnMessageHandler	Callback function
//	PDPNID						*ppPlayerList		Pointer filled with player ID's
//	DWORD						dwPlayersExpected	Number of players expected
//	DWORD						dwTimeout			Milliseconds before timing out operation
//
//
// Returns: TRUE if the session is connected to and the expected number of players is presen
//			FALSE otherwise
//==================================================================================
BOOL CreateServerAndWaitForPlayers(HANDLE hLog, PWRAPDP8SERVER *ppDP8Server, PBASESESSIONCREATECONTEXT pContext, DPN_APPLICATION_DESC *pdpnad, PFNDPNMESSAGEHANDLER pfnMessageHandler, PDPNID *ppPlayerList, DWORD dwPlayersExpected, DWORD dwTimeout)
{
	PDIRECTPLAY8ADDRESS		pDP8DeviceAddress = NULL;
	HRESULT					hr;
	DWORD					dwPort, dwPlayersActual;
	BOOL					fRet = TRUE;

	// Create the server wrapper
	*ppDP8Server = new CWrapDP8Server(hLog);
	if (*ppDP8Server == NULL)
	{
		DPTEST_TRACE(hLog, "Couldn't create server wrapper object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create the DirectPlay server object
	hr = (*ppDP8Server)->CoCreate();
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create an address to host with
	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8DeviceAddress, NULL);
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate first DirectPlay8Address object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Set up the context
	pContext->dwPlayersCreated = 0;
	pContext->dwPlayersExpected = dwPlayersExpected;
	pContext->fPlayersHaveLeft = FALSE;
	pContext->hbsccAllPlayersCreated = CreateEvent(NULL, FALSE, FALSE, NULL);

	if(!pContext->hbsccAllPlayersCreated)
	{
		DPTEST_TRACE(hLog, "Couldn't create AllPlayersCreated event!", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Initialized the host
	hr = (*ppDP8Server)->DP8S_Initialize(pContext, pfnMessageHandler, 0);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Initializing with context, 0 flags failed!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Always default to this port.  Allows netmon parsers to detect DirectPlay packets
	dwPort = 2302;
	
	if(pDP8DeviceAddress->AddComponent(DPNA_KEY_PORT, &dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD) != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't set port for host", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Host the session
	hr = (*ppDP8Server)->DP8S_Host(pdpnad, &pDP8DeviceAddress, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Hosting with 1 device failed!", 0);
		fRet = FALSE;
		goto EXIT;
	} // end if (couldn't host)

	// Wait for all the players to be connected
	if(WaitForSingleObject(pContext->hbsccAllPlayersCreated, 120000) != WAIT_OBJECT_0)
	{
		DPTEST_TRACE(hLog, "Players weren't created within 2 minutes", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Get the number of players created
	dwPlayersActual = 0;
	(*ppDP8Server)->DP8S_EnumPlayersAndGroups(NULL, &dwPlayersActual, DPNENUM_PLAYERS);
	if(dwPlayersActual != pContext->dwPlayersExpected)
	{
		DPTEST_TRACE(hLog, "Unexpected number of players in session: (%u != %u)", 2, dwPlayersActual, dwPlayersExpected);
		fRet = FALSE;
		goto EXIT;
	}

	// Allocate a buffer for the players
	*ppPlayerList = (DPNID *) MemAlloc(sizeof(DPNID) * dwPlayersActual);
	if(!*ppPlayerList)
	{
		DPTEST_TRACE(hLog, "Couldn't allocate player ID buffer", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Get the players that were created
	hr = (*ppDP8Server)->DP8S_EnumPlayersAndGroups(*ppPlayerList, &dwPlayersActual, DPNENUM_PLAYERS);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Error enumerating players from session: 0x%08x", 1, hr);
		fRet = FALSE;
		goto EXIT;
	}

	DPTEST_TRACE(hLog, "Session successfully hosted and all %u players created", 1, pContext->dwPlayersExpected);

EXIT:

	// Release the address
	if(pDP8DeviceAddress)
	{
		pDP8DeviceAddress->Release();
		pDP8DeviceAddress = NULL;
	}

	return fRet;
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnectClient()"
//==================================================================================
// ConnectClient
//----------------------------------------------------------------------------------
//
// Description: Connects to a server at a specified address
//
// Arguments:
//	HANDLE						hLog				Handle to logging subsystem
//	PWRAPDP8CLIENT				*ppDP8Client		Pointer filled with client
//	PBASESESSIONCREATECONTEXT	pContext			Callback context
//	DWORD						dwHostAddr			DWORD form of host's IP address
//	DPN_APPLICATION_DESC		*pdpnad				Application descriptor of host session
//	PFNDPNMESSAGEHANDLER		pfnMessageHandler	Callback function
//	DWORD						dwTimeout			Milliseconds before timing out operation
//
//
// Returns: TRUE if the session is connected to and the expected number of players is presen
//			FALSE otherwise
//==================================================================================
BOOL ConnectClient(HANDLE hLog, PWRAPDP8CLIENT *ppDP8Client, PBASESESSIONCREATECONTEXT pContext, DWORD dwHostAddr, DPN_APPLICATION_DESC *pdpnad, PFNDPNMESSAGEHANDLER pfnMessageHandler, DWORD dwTimeout)
{
	PDIRECTPLAY8ADDRESS		pDP8HostAddress = NULL, pDP8DeviceAddress = NULL;
	SOCKADDR_IN				HostAddr;
	DPNHANDLE				hAsync = NULL;
	HRESULT					hr;
	DWORD					dwPort;
	BOOL					fRet;

	// Create the clinet wrapper
	*ppDP8Client = new CWrapDP8Client(hLog);
	if (*ppDP8Client == NULL)
	{
		DPTEST_TRACE(hLog, "Couldn't create client wrapper object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create the DirectPlay client object
	hr = (*ppDP8Client)->CoCreate();
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate DirectPlay8Client object!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Create an address to host with
	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8HostAddress, NULL);
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate first DirectPlay8Address object!", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Create an address for the device we are connecting with
	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8DeviceAddress, NULL);
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't CoCreate first DirectPlay8Address object!", 0);
		fRet = FALSE;
		goto EXIT;
	}

	// Clients done get CREATE_PLAYER callbacks, to this shouldn't be needed
	pContext->hbsccAllPlayersCreated = NULL;

	// Initialized the host
	hr = (*ppDP8Client)->DP8C_Initialize(pContext, pfnMessageHandler, 0);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Initializing with context, 0 flags failed!", 0);
		fRet = FALSE;
		goto EXIT;
	}
	
	// Build a socket address for the session on the host machine
	HostAddr.sin_family = AF_INET;
	HostAddr.sin_port = htons(2302);
	HostAddr.sin_addr.S_un.S_addr = dwHostAddr;

	// Set that address
	if(pDP8HostAddress->BuildFromSockAddr((SOCKADDR *) &HostAddr) != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't set port for host", 0);
		fRet = FALSE;
		goto EXIT;
	}


	// Host the session
	hr = (*ppDP8Client)->DP8C_Connect(pdpnad, pDP8HostAddress, pDP8DeviceAddress, NULL, NULL, NULL, 0, NULL, &hAsync, 0);
	if (hr != DPNERR_PENDING)
	{
		DPTEST_TRACE(hLog, "Connecting to server failed: 0x%08x", 1, hr);
		fRet = FALSE;
		goto EXIT;
	} // end if (couldn't host)

	// Wait for all the players to be connected
	if(WaitForSingleObject(pContext->hbsccAllPlayersCreated, 120000) != WAIT_OBJECT_0)
	{
		DPTEST_TRACE(hLog, "Players weren't created within 2 minutes", 0);
		fRet = FALSE;
		goto EXIT;
	}

	DPTEST_TRACE(hLog, "Session successfully connected", 0);

EXIT:

	// Release the address
	if(pDP8HostAddress)
	{
		pDP8HostAddress->Release();
		pDP8HostAddress = NULL;
	}

	// Release the address
	if(pDP8DeviceAddress)
	{
		pDP8DeviceAddress->Release();
		pDP8DeviceAddress = NULL;
	}

	return fRet;


}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



#undef DEBUG_SECTION
#define DEBUG_SECTION	"PollUntilEventSignalled()"
//==================================================================================
// PollUntilEventSignalled
//----------------------------------------------------------------------------------
//
// Description: Calls the peer's DoWork function until the specified event is signalled
//
// Arguments:
//	HANDLE					hLog			Handle to logging subsystem
//	PDP_DOWORKLIST			pDoWorkList		Pointer to structure containing objects to call DoWork on
//	DWORD					dwMaxTime		Maximum time to wait before returning failure
//	HANDLE					hEvent			Handle to event that must be signalled to exit
//
// Returns: TRUE if the event signalled successfully
//			FALSE otherwise
//==================================================================================
BOOL PollUntilEventSignalled(HANDLE hLog, PDP_DOWORKLIST pDoWorkList, DWORD dwMaxTime, HANDLE hEvent)
{
	HRESULT	hr = S_OK;
	DWORD	dwStartDoWorkTime, dwEndDoWorkTime, dwAdjustedDoWorkDuration, dwWaitResult;
	DWORD	dwTotalTime, dwStartTime, dwEndTime, dwCurrentObject;
	DWORD	*pdwPeerDoWorkTotalTimes = NULL;
	DWORD	*pdwPeerDoWorkTotalCalls = NULL;
	DWORD	*pdwPeerDoWorkMaxTime = NULL;
	DWORD	*pdwPeerDoWorkMinTime = NULL;
	DWORD	*pdwClientDoWorkTotalTimes = NULL;
	DWORD	*pdwClientDoWorkTotalCalls = NULL;
	DWORD	*pdwClientDoWorkMaxTime = NULL;
	DWORD	*pdwClientDoWorkMinTime = NULL;
	DWORD	*pdwServerDoWorkTotalTimes = NULL;
	DWORD	*pdwServerDoWorkTotalCalls = NULL;
	DWORD	*pdwServerDoWorkMaxTime = NULL;
	DWORD	*pdwServerDoWorkMinTime = NULL;
	BOOL	fRet = TRUE, fRunDoWorkLoop = TRUE;

	// Need to get the start time to determine later if too much time has been
	// spent doing work and waiting
	if(dwMaxTime != INFINITE)
	{
		dwStartTime = GetTickCount();
		dwTotalTime = 0;
	}

	// If we're tracking DoWork times.  Take a global DoWork start time.
	if(pDoWorkList->fTrackDoWorkTimes)
	{
		// Allocate and initialize variables for tracking any peer object DoWork times
		if(pDoWorkList->dwNumPeers)
		{
			pdwPeerDoWorkTotalTimes = (DWORD *) MemAlloc(pDoWorkList->dwNumPeers * sizeof(DWORD));
			pdwPeerDoWorkTotalCalls = (DWORD *) MemAlloc(pDoWorkList->dwNumPeers * sizeof(DWORD));
			pdwPeerDoWorkMaxTime = (DWORD *) MemAlloc(pDoWorkList->dwNumPeers * sizeof(DWORD));
			pdwPeerDoWorkMinTime = (DWORD *) MemAlloc(pDoWorkList->dwNumPeers * sizeof(DWORD));

			if(!pdwPeerDoWorkTotalTimes || !pdwPeerDoWorkTotalCalls || !pdwPeerDoWorkMaxTime || !pdwPeerDoWorkMinTime)
			{
				xLog(hLog, XLL_WARN, "Couldn't allocate variables for tracking DoWork time on peer objects!");
				fRet = FALSE;
				goto Exit;
			}

			memset(pdwPeerDoWorkTotalTimes, 0, pDoWorkList->dwNumPeers * sizeof(DWORD));
			memset(pdwPeerDoWorkTotalCalls, 0, pDoWorkList->dwNumPeers * sizeof(DWORD));
			memset(pdwPeerDoWorkMaxTime, 0, pDoWorkList->dwNumPeers * sizeof(DWORD));
			memset(pdwPeerDoWorkMinTime, 0xFF, pDoWorkList->dwNumPeers * sizeof(DWORD));
		}

		// Allocate and initialize variables for tracking any client object DoWork times
		if(pDoWorkList->dwNumClients)
		{
			pdwClientDoWorkTotalTimes = (DWORD *) MemAlloc(pDoWorkList->dwNumClients * sizeof(DWORD));
			pdwClientDoWorkTotalCalls = (DWORD *) MemAlloc(pDoWorkList->dwNumClients * sizeof(DWORD));
			pdwClientDoWorkMaxTime = (DWORD *) MemAlloc(pDoWorkList->dwNumClients * sizeof(DWORD));
			pdwClientDoWorkMinTime = (DWORD *) MemAlloc(pDoWorkList->dwNumClients * sizeof(DWORD));

			if(!pdwClientDoWorkTotalTimes || !pdwClientDoWorkTotalCalls || !pdwClientDoWorkMaxTime || !pdwClientDoWorkMinTime)
			{
				xLog(hLog, XLL_WARN, "Couldn't allocate variables for tracking DoWork time on client objects!");
				fRet = FALSE;
				goto Exit;
			}

			memset(pdwClientDoWorkTotalTimes, 0, pDoWorkList->dwNumClients * sizeof(DWORD));
			memset(pdwClientDoWorkTotalCalls, 0, pDoWorkList->dwNumClients * sizeof(DWORD));
			memset(pdwClientDoWorkMaxTime, 0, pDoWorkList->dwNumClients * sizeof(DWORD));
			memset(pdwClientDoWorkMinTime, 0xFF, pDoWorkList->dwNumClients * sizeof(DWORD));
		}

		// Allocate and initialize variables for tracking any server object DoWork times
		if(pDoWorkList->dwNumServers)
		{
			pdwServerDoWorkTotalTimes = (DWORD *) MemAlloc(pDoWorkList->dwNumServers * sizeof(DWORD));
			pdwServerDoWorkTotalCalls = (DWORD *) MemAlloc(pDoWorkList->dwNumServers * sizeof(DWORD));
			pdwServerDoWorkMaxTime = (DWORD *) MemAlloc(pDoWorkList->dwNumServers * sizeof(DWORD));
			pdwServerDoWorkMinTime = (DWORD *) MemAlloc(pDoWorkList->dwNumServers * sizeof(DWORD));

			if(!pdwServerDoWorkTotalTimes || !pdwServerDoWorkTotalCalls || !pdwServerDoWorkMaxTime || !pdwServerDoWorkMinTime)
			{
				xLog(hLog, XLL_WARN, "Couldn't allocate variables for tracking DoWork time on server objects!");
				fRet = FALSE;
				goto Exit;
			}

			memset(pdwServerDoWorkTotalTimes, 0, pDoWorkList->dwNumServers * sizeof(DWORD));
			memset(pdwServerDoWorkTotalCalls, 0, pDoWorkList->dwNumServers * sizeof(DWORD));
			memset(pdwServerDoWorkMaxTime, 0, pDoWorkList->dwNumServers * sizeof(DWORD));
			memset(pdwServerDoWorkMinTime, 0xFF, pDoWorkList->dwNumServers * sizeof(DWORD));
		}
	}

	// Wait to see if the event has completed.  May not even require a DoWork call
	dwWaitResult = WaitForSingleObject(hEvent, 0);
	switch(dwWaitResult)
	{
	case WAIT_OBJECT_0:
		xLog(hLog, XLL_INFO, "Event completed without needing to call DoWork!");
		fRunDoWorkLoop = FALSE;
		break;
	case WAIT_TIMEOUT:
		break;
	default:
		xLog(hLog, XLL_WARN, "Error %u occurred while waiting on event handle 0x%08x", dwWaitResult, hEvent);
		fRet = FALSE;
		goto Exit;
	}

	while(fRunDoWorkLoop)
	{
		// Cycle through peer objects and perform DoWork processing
		for(dwCurrentObject = 0; dwCurrentObject < pDoWorkList->dwNumPeers; ++dwCurrentObject)
		{
			if(pDoWorkList->fTrackDoWorkTimes)
			{
				// Blank out the callback time tracker for this object
				// This is a pointer to the part of the objects context that contains
				// the DWORD that tracks total time spent in the callback
				*((pDoWorkList->apdwPeerCallbackTimes)[dwCurrentObject]) = 0;

				// Get the time immediately before calling DoWork
				dwStartDoWorkTime = GetTickCount();
			}

			// Do work on this object
			pDoWorkList->apDP8Peers[dwCurrentObject]->DoWork(0);

			if(pDoWorkList->fTrackDoWorkTimes)
			{
				// Get the end time immediately after calling DoWork
				dwEndDoWorkTime = GetTickCount();
				
				// Compute the delta between the start time of DoWork and the end time
				// Taking into account the rare possibility of the system clock rolling over
				if(dwEndDoWorkTime >= dwStartDoWorkTime)
					dwAdjustedDoWorkDuration = dwEndDoWorkTime - dwStartDoWorkTime;
				else
					dwAdjustedDoWorkDuration = (0xFFFFFFFF - dwStartDoWorkTime) + dwEndDoWorkTime;
				
				// Not sure if this is possible... but if a callback was triggered by another thread
				// between the time that the callback time was set to 0 and the time that DoWork was
				// called, then it could be possible for the callback time to exceed the total DoWork
				// time...  In this case, log a message and just assume that 0 time was spent in callback
				if(dwAdjustedDoWorkDuration < *((pDoWorkList->apdwPeerCallbackTimes)[dwCurrentObject]))
				{
					xLog(hLog, XLL_WARN, "Timing problem!  DoWork time (%u) is less than callback time (%u)",
						dwAdjustedDoWorkDuration, *((pDoWorkList->apdwPeerCallbackTimes)[dwCurrentObject]));
				}

				// Account for the time spent in the callback
				dwAdjustedDoWorkDuration -= *((pDoWorkList->apdwPeerCallbackTimes)[dwCurrentObject]);

				// Update the new minimum's and maximums
				if(pdwPeerDoWorkMaxTime[dwCurrentObject] < dwAdjustedDoWorkDuration)
					pdwPeerDoWorkMaxTime[dwCurrentObject] = dwAdjustedDoWorkDuration;

				if(pdwPeerDoWorkMinTime[dwCurrentObject] > dwAdjustedDoWorkDuration)
					pdwPeerDoWorkMinTime[dwCurrentObject] = dwAdjustedDoWorkDuration;

				// Update the new totals.  Taking into account the rare possibility that
				// the measure totals have rolled over the value of a DWORD
				if((0xFFFFFFFF - pdwPeerDoWorkTotalTimes[dwCurrentObject]) < dwAdjustedDoWorkDuration)
					pdwPeerDoWorkTotalTimes[dwCurrentObject] = 0xFFFFFFFF;
				else
					pdwPeerDoWorkTotalTimes[dwCurrentObject] += dwAdjustedDoWorkDuration;

				if(pdwPeerDoWorkTotalCalls[dwCurrentObject] != 0xFFFFFFFF)
					++pdwPeerDoWorkTotalCalls[dwCurrentObject];
			}

			if(hr != DPN_OK)
			{
				xLog(hLog, XLL_WARN, "Couldn't do DirectPlay work on a peer object! (0x%08x)", hr);
				fRet = FALSE;
				goto Exit;
			}

			// Wait for the desired result
			dwWaitResult = WaitForSingleObject(hEvent, 0);

			// Event completed.  Exit the loop
			if(dwWaitResult == WAIT_OBJECT_0)
				goto Exit;

			// If there's an upper bound on the amount of time that can be spent waiting for is event
			// See if that amount of time has already passed
			if(dwMaxTime != INFINITE)
			{
				dwEndTime = GetTickCount();
				dwTotalTime = (dwEndTime >= dwStartTime) ? dwEndTime - dwStartTime : (0xFFFFFFFF - dwStartTime) + dwEndTime;

				// If time has expired... bail
				if(dwTotalTime > dwMaxTime)
				{
					xLog(hLog, XLL_WARN, "Event wasn't signalled within %u ms", dwMaxTime);
					fRet = FALSE;
					goto Exit;
				}
			}
		} // End of loop for doing work over peer objects

		// Cycle through client objects and perform DoWork processing
		for(dwCurrentObject = 0; dwCurrentObject < pDoWorkList->dwNumClients; ++dwCurrentObject)
		{
			if(pDoWorkList->fTrackDoWorkTimes)
			{
				// Blank out the callback time tracker for this object
				// This is a pointer to the part of the objects context that contains
				// the DWORD that tracks total time spent in the callback
				*((pDoWorkList->apdwClientCallbackTimes)[dwCurrentObject]) = 0;

				// Get the time immediately before calling DoWork
				dwStartDoWorkTime = GetTickCount();
			}

			// Do work on this object
			pDoWorkList->apDP8Clients[dwCurrentObject]->DoWork(0);

			if(pDoWorkList->fTrackDoWorkTimes)
			{
				// Get the end time immediately after calling DoWork
				dwEndDoWorkTime = GetTickCount();
				
				// Compute the delta between the start time of DoWork and the end time
				// Taking into account the rare possibility of the system clock rolling over
				if(dwEndDoWorkTime >= dwStartDoWorkTime)
					dwAdjustedDoWorkDuration = dwEndDoWorkTime - dwStartDoWorkTime;
				else
					dwAdjustedDoWorkDuration = (0xFFFFFFFF - dwStartDoWorkTime) + dwEndDoWorkTime;
				
				// Not sure if this is possible... but if a callback was triggered by another thread
				// between the time that the callback time was set to 0 and the time that DoWork was
				// called, then it could be possible for the callback time to exceed the total DoWork
				// time...  In this case, log a message and just assume that 0 time was spent in callback
				if(dwAdjustedDoWorkDuration < *((pDoWorkList->apdwClientCallbackTimes)[dwCurrentObject]))
				{
					xLog(hLog, XLL_WARN, "Timing problem!  DoWork time (%u) is less than callback time (%u)",
						dwAdjustedDoWorkDuration, *((pDoWorkList->apdwClientCallbackTimes)[dwCurrentObject]));
				}

				// Account for the time spent in the callback
				dwAdjustedDoWorkDuration -= *((pDoWorkList->apdwClientCallbackTimes)[dwCurrentObject]);

				// Update the new minimum's and maximums
				if(pdwClientDoWorkMaxTime[dwCurrentObject] < dwAdjustedDoWorkDuration)
					pdwClientDoWorkMaxTime[dwCurrentObject] = dwAdjustedDoWorkDuration;

				if(pdwClientDoWorkMinTime[dwCurrentObject] > dwAdjustedDoWorkDuration)
					pdwClientDoWorkMinTime[dwCurrentObject] = dwAdjustedDoWorkDuration;

				// Update the new totals.  Taking into account the rare possibility that
				// the measure totals have rolled over the value of a DWORD
				if(0xFFFFFFFF - pdwClientDoWorkTotalTimes[dwCurrentObject] < dwAdjustedDoWorkDuration)
					pdwClientDoWorkTotalTimes[dwCurrentObject] = 0xFFFFFFFF;
				else
					pdwClientDoWorkTotalTimes[dwCurrentObject] += dwAdjustedDoWorkDuration;

				if(pdwClientDoWorkTotalCalls[dwCurrentObject] != 0xFFFFFFFF)
					++pdwClientDoWorkTotalCalls[dwCurrentObject];
			}

			if(hr != DPN_OK)
			{
				xLog(hLog, XLL_WARN, "Couldn't do DirectPlay work on a client object! (0x%08x)", hr);
				fRet = FALSE;
				goto Exit;
			}

			// Wait for the desired result
			dwWaitResult = WaitForSingleObject(hEvent, 0);

			// Event completed.  Exit the loop
			if(dwWaitResult == WAIT_OBJECT_0)
				goto Exit;

			// If there's an upper bound on the amount of time that can be spent waiting for is event
			// See if that amount of time has already passed
			if(dwMaxTime != INFINITE)
			{
				dwEndTime = GetTickCount();
				dwTotalTime = (dwEndTime >= dwStartTime) ? dwEndTime - dwStartTime : (0xFFFFFFFF - dwStartTime) + dwEndTime;

				// If time has expired... bail
				if(dwTotalTime > dwMaxTime)
				{
					xLog(hLog, XLL_WARN, "Event wasn't signalled within %u ms", dwMaxTime);
					fRet = FALSE;
					goto Exit;
				}
			}
		} // End of loop for doing work over client objects


		// Cycle through server objects and perform DoWork processing
		for(dwCurrentObject = 0; dwCurrentObject < pDoWorkList->dwNumServers; ++dwCurrentObject)
		{
			if(pDoWorkList->fTrackDoWorkTimes)
			{
				// Blank out the callback time tracker for this object
				// This is a pointer to the part of the objects context that contains
				// the DWORD that tracks total time spent in the callback
				*((pDoWorkList->apdwServerCallbackTimes)[dwCurrentObject]) = 0;

				// Get the time immediately before calling DoWork
				dwStartDoWorkTime = GetTickCount();
			}

			// Do work on this object
			pDoWorkList->apDP8Servers[dwCurrentObject]->DoWork(0);

			if(pDoWorkList->fTrackDoWorkTimes)
			{
				// Get the end time immediately after calling DoWork
				dwEndDoWorkTime = GetTickCount();
				
				// Compute the delta between the start time of DoWork and the end time
				// Taking into account the rare possibility of the system clock rolling over
				if(dwEndDoWorkTime >= dwStartDoWorkTime)
					dwAdjustedDoWorkDuration = dwEndDoWorkTime - dwStartDoWorkTime;
				else
					dwAdjustedDoWorkDuration = (0xFFFFFFFF - dwStartDoWorkTime) + dwEndDoWorkTime;
				
				// Not sure if this is possible... but if a callback was triggered by another thread
				// between the time that the callback time was set to 0 and the time that DoWork was
				// called, then it could be possible for the callback time to exceed the total DoWork
				// time...  In this case, log a message and just assume that 0 time was spent in callback
				if(dwAdjustedDoWorkDuration < *((pDoWorkList->apdwServerCallbackTimes)[dwCurrentObject]))
				{
					xLog(hLog, XLL_WARN, "Timing problem!  DoWork time (%u) is less than callback time (%u)",
						dwAdjustedDoWorkDuration, *((pDoWorkList->apdwServerCallbackTimes)[dwCurrentObject]));
				}

				// Account for the time spent in the callback
				dwAdjustedDoWorkDuration -= *((pDoWorkList->apdwServerCallbackTimes)[dwCurrentObject]);

				// Update the new minimum's and maximums
				if(pdwServerDoWorkMaxTime[dwCurrentObject] < dwAdjustedDoWorkDuration)
					pdwServerDoWorkMaxTime[dwCurrentObject] = dwAdjustedDoWorkDuration;

				if(pdwServerDoWorkMinTime[dwCurrentObject] > dwAdjustedDoWorkDuration)
					pdwServerDoWorkMinTime[dwCurrentObject] = dwAdjustedDoWorkDuration;

				// Update the new totals.  Taking into account the rare possibility that
				// the measure totals have rolled over the value of a DWORD
				if(0xFFFFFFFF - pdwServerDoWorkTotalTimes[dwCurrentObject] < dwAdjustedDoWorkDuration)
					pdwServerDoWorkTotalTimes[dwCurrentObject] = 0xFFFFFFFF;
				else
					pdwServerDoWorkTotalTimes[dwCurrentObject] += dwAdjustedDoWorkDuration;

				if(pdwServerDoWorkTotalCalls[dwCurrentObject] != 0xFFFFFFFF)
					++pdwServerDoWorkTotalCalls[dwCurrentObject];
			}

			if(hr != DPN_OK)
			{
				xLog(hLog, XLL_WARN, "Couldn't do DirectPlay work on a server object! (0x%08x)", hr);
				fRet = FALSE;
				goto Exit;
			}

			// Wait for the desired result
			dwWaitResult = WaitForSingleObject(hEvent, 0);

			// Event completed.  Exit the loop
			if(dwWaitResult == WAIT_OBJECT_0)
				goto Exit;

			// If there's an upper bound on the amount of time that can be spent waiting for is event
			// See if that amount of time has already passed
			if(dwMaxTime != INFINITE)
			{
				dwEndTime = GetTickCount();
				dwTotalTime = (dwEndTime >= dwStartTime) ? dwEndTime - dwStartTime : (0xFFFFFFFF - dwStartTime) + dwEndTime;

				// If time has expired... bail
				if(dwTotalTime > dwMaxTime)
				{
					xLog(hLog, XLL_WARN, "Event wasn't signalled within %u ms", dwMaxTime);
					fRet = FALSE;
					goto Exit;
				}
			}
		} // End of loop for doing work over server objects
	}

Exit:

	// The loop only exits if the event that's being waited for is successfully signalled
	// Otherwise, there would be a goto directly to the Exit.  So this is where DoWork
	// time information should be logged
	if(pDoWorkList->fTrackDoWorkTimes)
	{
		xLog(hLog, XLL_INFO, "DoWork processing times:");

		// Loop through peer objects and display DoWork times
		for(dwCurrentObject = 0; dwCurrentObject < pDoWorkList->dwNumPeers; ++dwCurrentObject)
		{
			if(pdwPeerDoWorkTotalCalls[dwCurrentObject])
			{
				xLog(hLog, XLL_INFO, "    Peer   %2u:  %u calls  (%u ms total, %u ms max, %u ms min)",
					dwCurrentObject + 1,
					pdwPeerDoWorkTotalCalls[dwCurrentObject],
					pdwPeerDoWorkTotalTimes[dwCurrentObject],
					pdwPeerDoWorkMaxTime[dwCurrentObject],
					pdwPeerDoWorkMinTime[dwCurrentObject]);
			}
			else
			{
				xLog(hLog, XLL_INFO, "    Peer   %2u:  0 calls",
					dwCurrentObject + 1);
			}
		}

		// Loop through client objects and display DoWork times
		for(dwCurrentObject = 0; dwCurrentObject < pDoWorkList->dwNumClients; ++dwCurrentObject)
		{
			if(pdwClientDoWorkTotalCalls[dwCurrentObject])
			{
				xLog(hLog, XLL_INFO, "    Client %2u:  %u calls  (%u ms total, %u ms max, %u ms min)",
					dwCurrentObject + 1,
					pdwClientDoWorkTotalCalls[dwCurrentObject],
					pdwClientDoWorkTotalTimes[dwCurrentObject],
					pdwClientDoWorkMaxTime[dwCurrentObject],
					pdwClientDoWorkMinTime[dwCurrentObject]);
			}
			else
			{
				xLog(hLog, XLL_INFO, "    Client %2u:  0 calls",
					dwCurrentObject + 1);
			}
		}

		// Loop through server objects and display DoWork times
		for(dwCurrentObject = 0; dwCurrentObject < pDoWorkList->dwNumServers; ++dwCurrentObject)
		{
			if(pdwServerDoWorkTotalCalls[dwCurrentObject])
			{
				xLog(hLog, XLL_INFO, "    Server %2u:  %u calls  (%u ms total, %u ms max, %u ms min)",
					dwCurrentObject + 1,
					pdwServerDoWorkTotalCalls[dwCurrentObject],
					pdwServerDoWorkTotalTimes[dwCurrentObject],
					pdwServerDoWorkMaxTime[dwCurrentObject],
					pdwServerDoWorkMinTime[dwCurrentObject]);
			}
			else
			{
				xLog(hLog, XLL_INFO, "    Server %2u:  0 calls",
					dwCurrentObject + 1);
			}
		}
	}

	if(pDoWorkList->fTrackDoWorkTimes)
	{
		// Release peer DoWork time tracker variables
		if(pDoWorkList->dwNumPeers)
		{
			pdwPeerDoWorkTotalTimes ? MemFree(pdwPeerDoWorkTotalTimes) : 0;
			pdwPeerDoWorkTotalCalls ? MemFree(pdwPeerDoWorkTotalCalls) : 0;
			pdwPeerDoWorkMaxTime ? MemFree(pdwPeerDoWorkMaxTime) : 0;
			pdwPeerDoWorkMinTime ? MemFree(pdwPeerDoWorkMinTime) : 0;
		}

		// Release client DoWork time tracker variables
		if(pDoWorkList->dwNumClients)
		{
			pdwClientDoWorkTotalTimes ? MemFree(pdwClientDoWorkTotalTimes) : 0;
			pdwClientDoWorkTotalCalls ? MemFree(pdwClientDoWorkTotalCalls) : 0;
			pdwClientDoWorkMaxTime ? MemFree(pdwClientDoWorkMaxTime) : 0;
			pdwClientDoWorkMinTime ? MemFree(pdwClientDoWorkMinTime) : 0;
		}

		// Release server DoWork time tracker variables
		if(pDoWorkList->dwNumServers)
		{
			pdwServerDoWorkTotalTimes ? MemFree(pdwServerDoWorkTotalTimes) : 0;
			pdwServerDoWorkTotalCalls ? MemFree(pdwServerDoWorkTotalCalls) : 0;
			pdwServerDoWorkMaxTime ? MemFree(pdwServerDoWorkMaxTime) : 0;
			pdwServerDoWorkMinTime ? MemFree(pdwServerDoWorkMinTime) : 0;
		}
	}

	return fRet;
}

} // namespace DPlayCoreNamespace