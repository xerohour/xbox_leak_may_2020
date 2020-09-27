//==================================================================================
// Includes
//==================================================================================
#include "dpstress.h"

using namespace DPlayStressNamespace;

namespace DPlayStressNamespace {

template void DoWorkForFixedInterval<IDirectPlay8Peer> (IDirectPlay8Peer *, DWORD);

#undef DEBUG_SECTION
#define DEBUG_SECTION	"IsNetsyncSessionOver()"
//==================================================================================
// IsNetsyncSessionOver
//----------------------------------------------------------------------------------
//
// Description: Reads messages from Netsync server and verifies that session is still valid
//
// Arguments:
//	HANDLE		hNetsync		Handle to the Netsync subsystem
//
// Returns: TRUE if Netsync said the session is no longer valid, FALSE otherwise
//==================================================================================
BOOL IsNetsyncSessionOver(HANDLE hNetsync)
{
	DP_GENERIC	*pMsg = NULL;
	DWORD		dwFromAddr = 0, dwMsgSize = 0, dwRecvStatus = 0, dwMessageType = 0;
	BOOL		fIsOver = FALSE;

	// Read all client messages until there aren't anymore or a STOPSESSION message is encountered
	while(TRUE)
	{
		pMsg ? NetsyncFreeMessage((char *) pMsg) : 0;
		pMsg = NULL;

		// Read a message from the queue
		dwRecvStatus = NetsyncReceiveMessage(hNetsync, 0, &dwMessageType, &dwFromAddr, &dwMsgSize, (char **) &pMsg);
		if(dwRecvStatus == WAIT_OBJECT_0)
		{
			// If the received message was invalid, keep reading messages
			if(dwMsgSize < sizeof(DP_GENERIC))
				continue;

			// If the recieved message wasn't a STOPSESSION message, ignore it
			if(pMsg->dwMessageId != NETSYNC_MSG_STOPSESSION)
				continue;

			// If this point is reached, then a STOPSESSION was received... exit
			fIsOver = TRUE;
			goto Exit;
		}

		// If there aren't any messages... exit
		else if(dwRecvStatus == WAIT_TIMEOUT)
		{
			fIsOver = FALSE;
			goto Exit;
		}
		
		// If we experienced a low level Netsync problem... exit
		else
		{
			fIsOver = TRUE;
			goto Exit;
		}
	}

Exit:

	pMsg ? NetsyncFreeMessage((char *) pMsg) : 0;
	pMsg = NULL;

	return fIsOver;
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"SendNetsyncHostKeepalive()"
//==================================================================================
// SendNetsyncHostKeepalive
//----------------------------------------------------------------------------------
//
// Description: Sends message to Netsync server indicating that host is still alive
//
// Arguments:
//	HANDLE		hNetsync		Handle to the Netsync subsystem
//
// Returns:
//    If the function succeeds, the return value is WAIT_OBJECT_0.
//    If the function times out, the return value is WAIT_TIMEOUT.
//    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().
//==================================================================================
DWORD SendNetsyncHostKeepalive(HANDLE hNetsync)
{
	DP_HOSTKEEPALIVE	HostKeepAlive;
	DWORD				dwSendStatus = WAIT_FAILED;

	HostKeepAlive.dwMessageId = DPMSG_HOSTKEEPALIVE;

	dwSendStatus = NetsyncSendClientMessage(hNetsync, 0, sizeof(DP_HOSTKEEPALIVE), (char *) &HostKeepAlive);

	return dwSendStatus;
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"SendNetsyncHostUpdate()"
//==================================================================================
// SendNetsyncHostUpdate
//----------------------------------------------------------------------------------
//
// Description: Sends message to Netsync server indicating that host has migrated
//
// Arguments:
//	HANDLE		hNetsync		Handle to the Netsync subsystem
//	IN_ADDR		*pLocalAddr		Pointer to the address of the local machine
//
// Returns:
//    If the function succeeds, the return value is WAIT_OBJECT_0.
//    If the function times out, the return value is WAIT_TIMEOUT.
//    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().
//==================================================================================
DWORD SendNetsyncHostUpdate(HANDLE hNetsync, IN_ADDR *pLocalAddr)
{
	DP_NEWHOST	NewHostMessage;
	DWORD		dwSendStatus;

	NewHostMessage.dwMessageId = DPMSG_NEWHOST;
	NewHostMessage.dwHostAddr = pLocalAddr->S_un.S_addr;

	dwSendStatus = NetsyncSendClientMessage(hNetsync, 0, sizeof(DP_NEWHOST), (char *) &NewHostMessage);

	return dwSendStatus;
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"FindServerIP()"
//==================================================================================
// FindServerIP
//----------------------------------------------------------------------------------
//
// Description: Resolve a given machine name to an IP address
//
// Arguments:
//	HANDLE		hLog			Handle to the logging subsystem
//	CHAR		*szServerName	Machine name to resolve
//	IN_ADDR		*pServerAddr	Pointer to an IP address that will hold the resolved address
//
// Returns:
//	TRUE if the machine name was successfully resolved, FALSE otherwise
//==================================================================================
BOOL FindServerIP(HANDLE hLog, CHAR *szServerName, IN_ADDR *pServerAddr)
{
	HOSTENT *pHost = NULL;
	DWORD dwNetError = 0;

	if(!szServerName || !pServerAddr)
	{
		xLog(hLog, XLL_WARN, "FindServerIP must be called with a valid server name and address pointer!");
		return FALSE;
	}

	pHost = gethostbyname(szServerName);
	if(!pHost)
	{
		dwNetError = WSAGetLastError();
		if(dwNetError == WSAHOST_NOT_FOUND)
			xLog(hLog, XLL_INFO, "Server %s wasn't found\n", szServerName);
		else
			xLog(hLog, XLL_WARN, "Network error %u occured while searching for %s\n", dwNetError, szServerName);

		return FALSE;
	}

	pServerAddr->S_un.S_addr = *((DWORD *)pHost->h_addr);

	return TRUE;
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"GenerateRandomDataBuffer()"
//==================================================================================
// GenerateRandomDataBuffer
//----------------------------------------------------------------------------------
//
// Description: Generate a data buffer of a given size with a header and checksum
//
// Arguments:
//	LPVOID		*ppBuffer		Parameter for passing back a pointer to the created buffer
//	DWORD		dwMinSize		Minimum size for this buffer (must be >= sizeof(DATAHEADER))
//	DWORD		dwMaxSize		Maximum size for this buffer (must be >= dwMinSize)
//
// Returns:
//	Returns the number of bytes in the generated packet, 0 indicates failure
//==================================================================================
DWORD GenerateRandomDataBuffer(BYTE **ppBuffer, DWORD dwMinSize, DWORD dwMaxSize)
{
	DATAHEADER *pHeader = NULL;
	DWORD dwBufferSize = 0;
	BYTE *pCurrent = NULL;

	if(!ppBuffer || (dwMinSize > dwMaxSize) || (dwMinSize < sizeof(DATAHEADER)))
		return 0;

	dwBufferSize = (rand() % (dwMaxSize - dwMinSize + 1)) + dwMinSize;

	if(!(pCurrent = (BYTE *) MemAlloc(dwBufferSize)))
		return 0;

	// Pass back the buffer via the ppBuffer variable
	*ppBuffer =  pCurrent;

	pHeader = (DATAHEADER *) pCurrent;
	pHeader->dwDataLength = dwBufferSize;
	pHeader->dwChecksum = 0;

	for(DWORD dwCurrent = sizeof(DATAHEADER); dwCurrent < dwBufferSize; ++dwCurrent)
	{
		(pHeader->dwChecksum) += (pCurrent[dwCurrent] = (BYTE) (rand() & 0x000000ff));
	}

	return dwBufferSize;
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"VerifyDataBuffer()"
//==================================================================================
// VerifyDataBuffer
//----------------------------------------------------------------------------------
//
// Description: Verify the validity of a data buffer
//
// Arguments:
//	LPVOID		pBuffer			Pointer to the buffer to verify
//	DWORD		dwBufferSize	Size of the buffer
//
// Returns:
//	TRUE if the packet passes its checksum, FALSE otherwise
//==================================================================================
BOOL VerifyDataBuffer(LPVOID pBuffer, DWORD dwBufferSize)
{
	DATAHEADER *pHeader = (DATAHEADER *) pBuffer;
	BYTE *pCurrent = (BYTE *) pBuffer;

	if(!pBuffer || (pHeader->dwDataLength != dwBufferSize) || (dwBufferSize < sizeof(DATAHEADER)))
		return FALSE;

	DWORD dwComputedChecksum = 0;

	for(DWORD dwCurrent = sizeof(DATAHEADER); dwCurrent < dwBufferSize; ++dwCurrent)
	{
		dwComputedChecksum += (pCurrent[dwCurrent]);
	}

	return (dwComputedChecksum == pHeader->dwChecksum);
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"GetAddressString()"
//==================================================================================
// GetAddressString
//----------------------------------------------------------------------------------
//
// Description: Retrieves a string representation of a DirectPlay 8 address
//
// Arguments:
//	DIRECTPLAY8ADDRESS		*pDP8Address		Pointer to the DirectPlay address
//
// Returns:
//	Pointer to a character string representing the DirectPlay 8 address, NULL on failure
//==================================================================================
LPSTR GetAddressString(PDIRECTPLAY8ADDRESS pDP8Address)
{
	HRESULT hr = NULL;
	DWORD dwAddressStringSize = 0;
	LPSTR szAddressString = NULL;
	BOOL fSuccess = TRUE;

	// Query for the address size
	hr = pDP8Address->GetURLA(NULL, &dwAddressStringSize);
	if(hr != DPNERR_BUFFERTOOSMALL)
	{
		fSuccess = FALSE;
		goto Exit;
	}
				
	// Allocate a buffer for it
	szAddressString = (CHAR *) MemAlloc(dwAddressStringSize);
	if(!szAddressString)
	{
		fSuccess = FALSE;
		goto Exit;
	}
				
	// Retrieve the URL
	hr = pDP8Address->GetURLA(szAddressString, &dwAddressStringSize);
	if(hr != DPN_OK)
	{
		fSuccess = FALSE;
		goto Exit;
	}

Exit:
	
	if(!fSuccess && szAddressString)
	{
		MemFree(szAddressString);
		szAddressString = NULL;
	}

	return szAddressString;
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"DoWorkForFixedInterval()"
//==================================================================================
// DoWorkForFixedInterval
//----------------------------------------------------------------------------------
//
// Description: Calls the peer's DoWork function until the specified event is signalled
//
// Arguments:
//
//
// Returns: nothing
//==================================================================================
template< class T > void DoWorkForFixedInterval( T *pDPObject, DWORD dwInterval)
{
	DWORD dwDoWorkStartTime = GetTickCount(), dwCurrentTime = 0;
	DWORD dwElapsedTime = 0;

	while(dwElapsedTime <= dwInterval)
	{
		pDPObject->DoWork(0);

		dwCurrentTime = GetTickCount();
		if(dwCurrentTime >= dwDoWorkStartTime)
		{
			dwElapsedTime = dwCurrentTime - dwDoWorkStartTime;
		}
		else
		{
			dwElapsedTime = (DWORD) 0xFFFFFFFF - dwDoWorkStartTime + dwCurrentTime;
		}
	}
}

#undef DEBUG_SECTION
#define DEBUG_SECTION	"PollPeerUntilEventSignalled()"
//==================================================================================
// PollPeerUntilEventSignalled
//----------------------------------------------------------------------------------
//
// Description: Calls the peer's DoWork function until the specified event is signalled
//
// Arguments:
//	HANDLE				hLog			Handle to logging subsystem
//	PDIRECTPLAY8PEER	pDP8Peer		Pointer to the peer to call DoWork on
//	DWORD				dwMaxTime		Maximum time to wait before returning failure
//	HANDLE				hEvent			Handle to event that must be signalled to exit
//	LPDWORD				pdwCallbackTime	Number of milliseconds spent in our callback routine
//
//
// Returns: TRUE if the event signalled successfully
//			FALSE otherwise
//==================================================================================
BOOL PollPeerUntilEventSignalled(HANDLE hLog, PDIRECTPLAY8PEER pDP8Peer, DWORD dwMaxTime, HANDLE hEvent, LPDWORD pdwCallbackTime)
{
	HRESULT hr = S_OK;
	DWORD dwDoWorkCalls = 0, dwTotalTimeInDoWork = 0, dwStartDoWorkTime = 0, dwEndDoWorkTime = 0, dwWaitResult = 0;
	DWORD dwTotalTime, dwStartTime, dwEndTime;
	BOOL fRet = TRUE;

	if(pdwCallbackTime)
		*pdwCallbackTime = 0;

	dwStartTime = GetTickCount();
	dwTotalTime = 0;

	dwWaitResult = WaitForSingleObject(hEvent, 0);
	while((dwWaitResult == WAIT_TIMEOUT) && ((dwMaxTime == INFINITE) || (dwTotalTime <= dwMaxTime)))
	{
		dwStartDoWorkTime = GetTickCount();
		
		// Call the work pump function
		hr = pDP8Peer->DoWork(0);

// BUGBUG - later we will move to the global online state-machine model
//		hr = XONAsyncWorkPump(XON_ASYNCWORK_ALL);
		
		dwEndDoWorkTime = GetTickCount();
		
		// Update the total time we have spent in the DoWork function
		dwTotalTimeInDoWork += (dwEndDoWorkTime >= dwStartDoWorkTime) ? dwEndDoWorkTime - dwStartDoWorkTime : (0xFFFFFFFF - dwStartDoWorkTime) + dwEndDoWorkTime;
		
		if((hr != DPN_OK) && (hr != S_FALSE))
		{
			xLog(hLog, XLL_WARN, "Couldn't do DirectPlay work! (0x%08x)", hr);
			fRet = FALSE;
			goto Exit;
		}
		
		++dwDoWorkCalls;
		
		// Wait for the desired result
		dwWaitResult = WaitForSingleObject(hEvent, 0);

		// Calculate the total time we have been in this function
		if(dwMaxTime != INFINITE)
		{
			dwEndTime = GetTickCount();
			dwTotalTime = (dwEndTime >= dwStartTime) ? dwEndTime - dwStartTime : (0xFFFFFFFF - dwStartTime) + dwEndTime;
		}

	}
	
	// If the wait failed, then return an error
	if(dwWaitResult != WAIT_OBJECT_0)
	{
		// If we wound up waiting too long, then return an error
		if((dwMaxTime != INFINITE) && (dwTotalTime > dwMaxTime))
		{
			xLog(hLog, XLL_WARN, "Spent more than %u ms waiting for event", dwMaxTime);
			fRet = FALSE;
			goto Exit;
		}

		xLog(hLog, XLL_WARN, "Error while waiting for event to signal!");
		fRet = FALSE;
		goto Exit;
	}

	
	if(pdwCallbackTime)
		xLog(hLog, XLL_INFO, "Called work function %u times for %u ms (%u ms callback)", dwDoWorkCalls, dwTotalTimeInDoWork, *pdwCallbackTime);
//	else
//		xLog(hLog, XLL_INFO, "Called work function %u times for %u ms", dwDoWorkCalls, dwTotalTimeInDoWork);

Exit:

	return fRet;
}


} // namespace DPlayStressNamespace
