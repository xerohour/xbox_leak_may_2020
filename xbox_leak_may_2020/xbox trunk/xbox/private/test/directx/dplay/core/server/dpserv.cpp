#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <tchar.h>

#include "dpserv.h"

//PDP_SERV_FUNCTION	g_ServerFunctions[DPMSG_NET_MAXTEST] = 
//{
//	DPSourceMaxServ,			// DPMSG_SOURCEPORT_MAX
//};

//==================================================================================
// DllMain
//----------------------------------------------------------------------------------
//
// Description: Global entry point
//
// Arguments:
//	HINSTANCE	hInstance	Handle to calling instance
//	DWORD		dwReason	Reason for calling entry point
//	LPVOID		lpContext	
//
// Returns:
//	TRUE on successful attach or detach, FALSE otherwise
//==================================================================================
BOOL WINAPI DllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext)
{
    switch (dwReason) {

    case DLL_PROCESS_ATTACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

//==================================================================================
// Helper functions
//==================================================================================

//==================================================================================
// FindEvent
//----------------------------------------------------------------------------------
//
// Description: Finds the event structure (if present) for the given named event
//
// Arguments:
//	PDPSYNC_LIST_ENTRY	pListHead		Points to the head of the list of outstanding events
//	LPCSTR				szSyncDataName	ANSI name of the event
//
// Returns:
//	Returns TRUE if the message was received was successfully handled, FALSE otherwise
//==================================================================================
PDPSYNC_LIST_ENTRY FindEvent(PDPSYNC_LIST_ENTRY pListHead, LPCSTR szSyncDataName)
{
	PDPSYNC_LIST_ENTRY pCurrentEvent = pListHead;

	while(pCurrentEvent)
	{
		if(strcmp(pCurrentEvent->szSyncDataName, szSyncDataName) == 0)
			break;

		pCurrentEvent = pCurrentEvent->pNext;
	}

	return pCurrentEvent;
}

//==================================================================================
// AddEvent
//----------------------------------------------------------------------------------
//
// Description: Adds the given event structure to the current session info
//
// Arguments:
//	PDPCORE_SESSION_INFO		pSessionInfo	Points to the current session info
//	PDPSYNC_LIST_ENTRY	pNewEntry		Points to the new event entry to be added to the session info
//
// Returns:
//	Returns TRUE
//==================================================================================
BOOL AddEvent(PDPCORE_SESSION_INFO pSessionInfo, PDPSYNC_LIST_ENTRY pNewEntry)
{
	if(!pSessionInfo || !pNewEntry)
		return FALSE;

	pNewEntry->pNext = pSessionInfo->pSyncListHead;
	pSessionInfo->pSyncListHead = pNewEntry;

	return TRUE;
}

//==================================================================================
// RemoveEvent
//----------------------------------------------------------------------------------
//
// Description: Removes the given event structure to the current session info
//
// Arguments:
//	PDPCORE_SESSION_INFO		pSessionInfo	Points to the current session info
//	PDPSYNC_LIST_ENTRY	pEntryToRemove	Points to the entry to be removed from the sesion info
//
// Returns:
//	Returns TRUE
//==================================================================================
BOOL RemoveEvent(PDPCORE_SESSION_INFO pSessionInfo, PDPSYNC_LIST_ENTRY pEntryToRemove)
{
	PDPSYNC_LIST_ENTRY pCurrentEvent = pSessionInfo->pSyncListHead;

	if(!pSessionInfo || !pEntryToRemove)
		return FALSE;

	if(pSessionInfo->pSyncListHead == pEntryToRemove)
	{
		pSessionInfo->pSyncListHead = pEntryToRemove->pNext;
		goto RemoveItem;
	}

	while(pCurrentEvent)
	{
		if(pCurrentEvent->pNext == pEntryToRemove)
		{
			pCurrentEvent->pNext = pEntryToRemove->pNext;
			goto RemoveItem;
		}

		pCurrentEvent = pCurrentEvent->pNext;
	}

	return FALSE;

RemoveItem:
	
	pEntryToRemove->pbSyncData ? LocalFree(pEntryToRemove->pbSyncData) : 0;
	LocalFree(pEntryToRemove);
	return TRUE;
}


//==================================================================================
// DpProcessServerMessage
//----------------------------------------------------------------------------------
//
// Description: Function that handles server callbacks
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	DWORD		FromAddr		Address of sender of message that caused the callback
//	CHAR		*ReceiveMessage	Buffer containing the packet that caused the callback
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	Returns TRUE if the message was received was successfully handled, FALSE otherwise
//==================================================================================
BOOL DpProcessServerMessage(HANDLE hNetsyncObject, DWORD FromAddr, CHAR *ReceiveMessage, LPVOID lpContext)
{
	PDPCORE_SESSION_INFO pSessionInfo = (PDPCORE_SESSION_INFO) lpContext;
	BYTE nClient;

	DbgPrint("Processing server message\n");

	switch(((PNETSYNC_GENERIC_MESSAGE) ReceiveMessage)->dwMessageId)
	{
	case NETSYNC_MSG_STOPSESSION:
		DbgPrint("Stopping session due to message received from 0x%08x!\n", FromAddr);
		return FALSE;
	case NETSYNC_MSG_ADDCLIENT:
		DbgPrint("Ignoring added client: 0x%08x\n", FromAddr);
		return TRUE;
	case NETSYNC_MSG_DELETECLIENT:
		// Client is signalling that this test is done
		DbgPrint("Machine at address 0x%08x is signalling that it is done.\n", FromAddr);
		
		// Find which client sent this message
		for(nClient = 0; nClient < pSessionInfo->byClientCount; ++nClient)
		{
			if(pSessionInfo->ClientAddrs[nClient] == FromAddr)
				break;
		}

		// If we don't recogize the sender as one of our connected machines, then ignore the message and
		// print a warning
		if(nClient >= pSessionInfo->byClientCount)
		{
			DbgPrint("Received DELETECLIENT message from unknown machine 0x%08x\n", FromAddr);
			return TRUE;
		}

		// Otherwise, mark this client as no longer running the test
		pSessionInfo->pTestStatus[nClient] = FALSE;

		for(nClient = 0; nClient < pSessionInfo->byClientCount; ++nClient)
		{
			if(pSessionInfo->pTestStatus[nClient])
				break;
		}

		// If all the machines are now disconnected, then signal the session to close by returning FALSE
		if(nClient >= pSessionInfo->byClientCount)
		{
			DbgPrint("All machines have left the session.\n");
			return FALSE;
		}
		break;
	default:
		DbgPrint("WARINING: Unknown server message received 0x%08x\n",
			((PNETSYNC_GENERIC_MESSAGE) ReceiveMessage)->dwMessageId);
		break;
	}

	return TRUE;
}

//==================================================================================
// DpProcessClientMessage
//----------------------------------------------------------------------------------
//
// Description: Function that handles client callbacks
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	DWORD		FromAddr		Address of sender of message that caused the callback
//	CHAR		*ReceiveMessage	Buffer containing the packet that caused the callback
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	Returns TRUE if the message was received was successfully handled, FALSE otherwise
//==================================================================================
BOOL DpProcessClientMessage(HANDLE hNetsyncObject, DWORD FromAddr, CHAR *ReceiveMessage, LPVOID lpContext)
{
	PDPSYNC_LIST_ENTRY pCurrentEvent;
	PDPCORE_SESSION_INFO pSessionInfo = (PDPCORE_SESSION_INFO) lpContext;
	PDP_SYNCWAIT pSyncWaitMsg;
	BYTE nClient;

	DbgPrint("Processing client message\n");

	switch(((PNETSYNC_GENERIC_MESSAGE) ReceiveMessage)->dwMessageId)
	{
	case DPMSG_SYNCWAIT:
		pSyncWaitMsg = (PDP_SYNCWAIT) ReceiveMessage;

		// Scan the list of client machines to see if any have already left the test
		for(nClient = 0; nClient < pSessionInfo->byClientCount; ++nClient)
		{
			if(!pSessionInfo->pTestStatus[nClient])
				break;
		}

		// If any of the machines have already left the test, then ignore the message (the client
		// will eventually timeout
		if(nClient < pSessionInfo->byClientCount)
		{
			DbgPrint("Received SYNCWAIT message but some machines have already left the session!\n");
			return TRUE;
		}

		// Quick sanity check on the sync data name
		if(strlen(pSyncWaitMsg->szSyncDataName) >= 16)
			return TRUE;

		// Find which client sent this message
		for(nClient = 0; nClient < pSessionInfo->byClientCount; ++nClient)
		{
			if(pSessionInfo->ClientAddrs[nClient] == FromAddr)
				break;
		}

		// If we don't recogize the sender as one of our connected machines, then ignore the message and
		// print a warning
		if(nClient >= pSessionInfo->byClientCount)
		{
			DbgPrint("Received SYNCWAIT message from unknown machine 0x%08x\n", FromAddr);
			return TRUE;
		}

		// Find the data on this event if it is present
		pCurrentEvent = FindEvent(pSessionInfo->pSyncListHead, pSyncWaitMsg->szSyncDataName);

		// If we've already received wait requests for this event, update the existing block
		if(pCurrentEvent)
		{
			++(pCurrentEvent->dwCurrentMachines);
			pCurrentEvent->pWaitStatus[nClient] = TRUE;
		}
		// If this is the first time we've received a wait request for this event...
		// Then create a new block to track it
		else
		{
			// Structure must be large enough for the DPSYNC_LIST_ENTRY followed by the variable length
			// list of BOOLs for tracking which machines are waiting for this event
			pCurrentEvent = (DPSYNC_LIST_ENTRY *) LocalAlloc(LPTR, sizeof(DPSYNC_LIST_ENTRY) + pSessionInfo->byClientCount * sizeof(BOOL));
			memset(pCurrentEvent, 0, sizeof(DPSYNC_LIST_ENTRY) + pSessionInfo->byClientCount * sizeof(BOOL));

			pCurrentEvent->dwCurrentMachines = 1;
			pCurrentEvent->dwMinMachinesRequired = pSyncWaitMsg->dwMinMachinesRequired;

			// Set up the wait status pointer to point to the end of the structure
			pCurrentEvent->pWaitStatus = (int *) ((BYTE *) pCurrentEvent + sizeof(DPSYNC_LIST_ENTRY));

			// Mark that the machine sending the message is waiting for the event
			pCurrentEvent->pWaitStatus[nClient] = TRUE;

			// Copy the event name
			strcpy(pCurrentEvent->szSyncDataName, pSyncWaitMsg->szSyncDataName);

			pCurrentEvent->pNext = NULL;
			
			if(!AddEvent(pSessionInfo, pCurrentEvent))
			{
				DbgPrint("Failed adding new event to event list\n");
				return TRUE;
			}
		}

		// If no previous machine has left any data with this event, then see if data was provided
		// this this message.  If so, then allocate a buffer for it and copy it.
		if(!pCurrentEvent->dwSyncDataSize)
		{
			// Set up the sync data size and pointer
			if(pCurrentEvent->dwSyncDataSize = pSyncWaitMsg->dwSyncDataSize)
			{
				pCurrentEvent->pbSyncData = (LPBYTE) LocalAlloc(LPTR, pSyncWaitMsg->dwSyncDataSize);
				memcpy(pCurrentEvent->pbSyncData, (BYTE *) pSyncWaitMsg + sizeof(DP_SYNCWAIT), pSyncWaitMsg->dwSyncDataSize);
			}
			else
				pCurrentEvent->pbSyncData = NULL;
		}		

		// If we've reached the minimum number of machines necessary to signal this event...
		// Or we're requiring all machines to wait before signalling and they have all signalled
		// Then send the signal to all machines that were waiting and release the tracking block.
		if((pCurrentEvent->dwMinMachinesRequired && (pCurrentEvent->dwCurrentMachines >= pCurrentEvent->dwMinMachinesRequired)) ||
			(pCurrentEvent->dwCurrentMachines >= pSessionInfo->byClientCount))
		{
			PDP_SYNCSIGNAL pSyncSignal;

			DbgPrint("Signalling event: %s\n", pCurrentEvent->szSyncDataName);

			// Allocate the sync signal message to send to clients to indicate that the event has been triggered
			pSyncSignal = (PDP_SYNCSIGNAL) LocalAlloc(LPTR, sizeof(DP_SYNCSIGNAL) + pCurrentEvent->dwSyncDataSize);
			pSyncSignal->dwMessageId = DPMSG_SYNCSIGNAL;
			strcpy(pSyncSignal->szSyncDataName, pCurrentEvent->szSyncDataName);
			if(pSyncSignal->dwSyncDataSize = pCurrentEvent->dwSyncDataSize)
				memcpy((BYTE *) pSyncSignal + sizeof(DP_SYNCSIGNAL), pCurrentEvent->pbSyncData, pCurrentEvent->dwSyncDataSize);

			// Cycle through the clients and determine which were waiting
			for(nClient = 0; nClient < pSessionInfo->byClientCount; ++nClient)
			{
				if(pCurrentEvent->pWaitStatus[nClient])
				{
					NetsyncSendClientMessage(hNetsyncObject,
						pSessionInfo->ClientAddrs[nClient], 
						sizeof(DP_SYNCSIGNAL) + pCurrentEvent->dwSyncDataSize,
						(char *) pSyncSignal);
				}
			}

			LocalFree(pSyncSignal);

			// The event was signalled, remove the event from our list.
			RemoveEvent(pSessionInfo, pCurrentEvent);
		}
		else
		{
			DbgPrint("Not signalling event %s: %u of %u machines waiting\n",
				pCurrentEvent->szSyncDataName,
				pCurrentEvent->dwCurrentMachines,
				pCurrentEvent->dwMinMachinesRequired ?
				pCurrentEvent->dwMinMachinesRequired : pSessionInfo->byClientCount);
		}

		break;

	default:
		// Unknown message received
		DbgPrint("WARINING: Unknown client message received 0x%08x\n",
			((PNETSYNC_GENERIC_MESSAGE) ReceiveMessage)->dwMessageId);
		break;
	}

	return TRUE;
}

//==================================================================================
// DpServerCallbackFunction
//----------------------------------------------------------------------------------
//
// Description: Function exported for Netsync callbacks
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	DWORD		FromAddr		Address of sender of message that caused the callback
//	DWORD		dwMessageType	Parameter indicating if this is a client or server message
//	DWORD		dwMessageSize	Parameter indicating the size of the message
//	CHAR		*ReceiveMessage	Buffer containing the packet that caused the callback
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	Returns TRUE if the message was received was successfully handled, FALSE otherwise
//==================================================================================
BOOL DpServerCallbackFunction(HANDLE hNetsyncObject, DWORD FromAddr, DWORD dwMessageType, DWORD dwMessageSize, CHAR *ReceiveMessage, LPVOID lpContext)
{
	DbgPrint("Callback entered!\n");

	switch(dwMessageType)
	{
	case NETSYNC_MSGTYPE_SERVER:
		return DpProcessServerMessage(hNetsyncObject, FromAddr, ReceiveMessage, lpContext);
	case NETSYNC_MSGTYPE_CLIENT:
		return DpProcessClientMessage(hNetsyncObject, FromAddr, ReceiveMessage, lpContext);
	default:
		DbgPrint("Unknown message type received: 0x%08x\n", dwMessageType);
		break;
	}

	return TRUE;
}

//==================================================================================
// DpServerStartFunction
//----------------------------------------------------------------------------------
//
// Description: Funtion called once for each session that is created
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	BYTE		byClientCount	Number of clients this test is being started with
//	DWORD		*ClientAddrs	List of client addresses (number of addresses indicated by byClientCount)
//	WORD		LowPort			The lowest port to use for this test
//	WORD		HighPort		The higest port to use for this test
//
// Returns:
//	Returns a pointer to the session information that is being stored for this session
//==================================================================================
LPVOID DpServerStartFunction(HANDLE hNetsyncObject, BYTE byClientCount, DWORD *ClientAddrs, WORD LowPort, WORD HighPort)
{
	PDPCORE_SESSION_INFO pCurrentSession = NULL;
	IN_ADDR ServerAddr;
	DWORD dwHostInfoSize;
	CHAR szServerAddr[16];
	BYTE n;

	// Check for too few clients in session.  Doesn't make sense to use the DP test layer with
	// only 1 client connected
	if(byClientCount < 2)
	{
		DbgPrint("Can't start session with only %u client\n", byClientCount);
		return NULL;
	}

	// Check for too many clients in session.  We have to limit this because the size of the
	// HOST_INFO message has to have an upper bound for the receiver to be able to allocate
	// a receive buffer that will always be adequate.  Besides, this many should be fine
	if(byClientCount > MAX_DPTEST_CLIENTS)
	{
		DbgPrint("Can't start session with only %u client\n", byClientCount);
		return NULL;
	}

	pCurrentSession = (PDPCORE_SESSION_INFO) LocalAlloc(LPTR, sizeof(DPCORE_SESSION_INFO));
	DbgPrint("Starting session: 0x%08x\n", pCurrentSession);

	pCurrentSession->pTestStatus = (BOOL *) LocalAlloc(LPTR, byClientCount * sizeof(BOOL));
	memset(pCurrentSession->pTestStatus, TRUE, byClientCount * sizeof(BOOL));

	// Fill the session info structure
	pCurrentSession->hNetsyncObject = hNetsyncObject;
	pCurrentSession->byClientCount = byClientCount;
	pCurrentSession->ClientAddrs = ClientAddrs;
	pCurrentSession->LowPort = LowPort;
	pCurrentSession->HighPort = HighPort;
	pCurrentSession->pSyncListHead = NULL;

	// Calculate the server address string
// BUGBUG - doesn't work this way anymore.  If we need the server address, we'll have to get
//     it the old fashioned way
//	ServerAddr.S_un.S_addr = ClientAddrs[0];
//	strcpy(pCurrentSession->szServerIPAddr, inet_ntoa(ServerAddr));

	// By default the DP_HOSTINFO structure has room for 2 clients, the elected host and the
	// first of the clients in the list.  So if there are more than 2 clients, we need to
	// allocate space for the extra ones
	dwHostInfoSize = sizeof(DP_HOSTINFO) + ((byClientCount < 3) ? 0 : (byClientCount - 2 * sizeof(DWORD)));

	pCurrentSession->pHostInfo = (DP_HOSTINFO *) LocalAlloc(LPTR, dwHostInfoSize);

	// Initialize the host info message
	memset(pCurrentSession->pHostInfo, 0, dwHostInfoSize);
	pCurrentSession->pHostInfo->dwMessageId = DPMSG_HOSTINFO;
	pCurrentSession->pHostInfo->dwTotalMachines = (DWORD) byClientCount;
	pCurrentSession->pHostInfo->dwHostAddr = ClientAddrs[0];

	// Fill in the non-host addresses
	for(n = 1; n < byClientCount; ++n)
		pCurrentSession->pHostInfo->dwNonHostAddrs[n - 1] = ClientAddrs[n];

	for(n = 0; n < byClientCount; ++n)
		NetsyncSendClientMessage(hNetsyncObject, ClientAddrs[n], dwHostInfoSize, (char *) (pCurrentSession->pHostInfo));

	// If anything needs to be setup for all the tests or if the currently
	// connected clients need to be tracked, it will be done here
	return (LPVOID) pCurrentSession;
}

//==================================================================================
// DpServerStopFunction
//----------------------------------------------------------------------------------
//
// Description: Function called once when each session is closed
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
VOID DpServerStopFunction(HANDLE hNetsyncObject, LPVOID lpContext)
{
	PDPCORE_SESSION_INFO pCurrentSession = (PDPCORE_SESSION_INFO) lpContext;

	DbgPrint("Stopping session: 0x%08x\n", pCurrentSession);

	LocalFree(pCurrentSession->pTestStatus);
	LocalFree(pCurrentSession->pHostInfo);
	LocalFree(pCurrentSession);

	// If anything needs to be cleaned up, it will be done here.
	return;
}

//==================================================================================
// DpProcessServerStressMessage
//----------------------------------------------------------------------------------
//
// Description: Function that handles server stress callbacks
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	DWORD		FromAddr		Address of sender of message that caused the callback
//	CHAR		*ReceiveMessage	Buffer containing the packet that caused the callback
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	Returns TRUE if the message was received was successfully handled, FALSE otherwise
//==================================================================================
BOOL DpProcessServerStressMessage(HANDLE hNetsyncObject, DWORD FromAddr, CHAR *ReceiveMessage, LPVOID lpContext)
{
	PDPSTRESS_SESSION_INFO pSessionInfo = (PDPSTRESS_SESSION_INFO) lpContext;
	IN_ADDR ClientAddress, HostAddress;
	DWORD dwCurrentTime = 0, dwTimeElapsed = 0, dwSendStatus = 0;

	ClientAddress.S_un.S_addr = FromAddr;
	HostAddress.S_un.S_addr = pSessionInfo->pHostInfo->dwHostAddr;

	DbgPrint("Processing server stress message\n");

	switch(((PNETSYNC_GENERIC_MESSAGE) ReceiveMessage)->dwMessageId)
	{
	case NETSYNC_MSG_STOPSESSION:
		DbgPrint("Stopping session due to message received from %s!\n", inet_ntoa(ClientAddress));
		return FALSE;
	case NETSYNC_MSG_ADDCLIENT:
		// Get the current time
		dwCurrentTime = GetTickCount();
		
		// Calculate the amount of time that has elapsed since the last keepalive
		if(dwCurrentTime < pSessionInfo->dwLastHostPing)
			dwTimeElapsed = ((DWORD) 0xffffffff) - pSessionInfo->dwLastHostPing + dwCurrentTime;
		else
			dwTimeElapsed = dwCurrentTime - pSessionInfo->dwLastHostPing;
		
		// Check to see if the last known host has responded recently
		if(dwTimeElapsed > LOST_HOST_TIME)
		{
			// If not, the we have a bad session...
			// The session probably dwindled down to just the host and then the host died.
			// So there was no one around to which to be migrated.  Need to kill the entire session.
			// Clients can then resync

			DbgPrint("Tried to add new machine %s, but host %s hasn't responded in %u ms\n",
				inet_ntoa(ClientAddress), inet_ntoa(HostAddress), dwTimeElapsed);
			return FALSE;
		}
		
		dwSendStatus = NetsyncSendClientMessage(hNetsyncObject, FromAddr, sizeof(DP_HOSTINFO), (char *) (pSessionInfo->pHostInfo));
		if(dwSendStatus != WAIT_OBJECT_0)
		{
			DbgPrint("Tried to add new machine %s, but Netsync send failed! (error 0x%08x)\n",
				inet_ntoa(ClientAddress), dwSendStatus);
			return FALSE;
		}

		DbgPrint("Added new machine %s to DirectPlay stress session 0x%08x\n", inet_ntoa(ClientAddress), pSessionInfo);

		break;
	case NETSYNC_MSG_DELETECLIENT:
		// If the host is exiting, set the host info message's host address to 0...
		// New clients that connect will recognize that a host address of 0 means that there is
		// no know host at this time.  These clients will then leave the session and try again later.
		// We'll also update the ping time to indicate when the host left.  If clients get added later
		// and we haven't gotten a new host within a resonable amount of time (LOST_HOST_TIME), then
		// the session will kill itself
		if(FromAddr == pSessionInfo->pHostInfo->dwHostAddr)
		{
			DbgPrint("Removed host %s from DirectPlay stress session 0x%08x\n", inet_ntoa(ClientAddress), pSessionInfo);
			pSessionInfo->pHostInfo->dwHostAddr = 0;
			pSessionInfo->dwLastHostPing = GetTickCount();
		}
		// Not that complicated when a non-host leaves.  The netsync server only needs to know who the host is.
		else
		{
			DbgPrint("Removed machine %s from DirectPlay stress session 0x%08x\n", inet_ntoa(ClientAddress), pSessionInfo);
		}

		break;
	default:
		DbgPrint("WARINING: Unknown server message received 0x%08x\n",
			((PNETSYNC_GENERIC_MESSAGE) ReceiveMessage)->dwMessageId);
		break;
	}

	return TRUE;
}

//==================================================================================
// DpProcessClientStressMessage
//----------------------------------------------------------------------------------
//
// Description: Function that handles client stress callbacks
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	DWORD		FromAddr		Address of sender of message that caused the callback
//	CHAR		*ReceiveMessage	Buffer containing the packet that caused the callback
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	Returns TRUE if the message was received was successfully handled, FALSE otherwise
//==================================================================================
BOOL DpProcessClientStressMessage(HANDLE hNetsyncObject, DWORD FromAddr, CHAR *ReceiveMessage, LPVOID lpContext)
{
	NETSYNC_GENERIC_MESSAGE StopSessionMsg;
	PDPSTRESS_SESSION_INFO pSessionInfo = (PDPSTRESS_SESSION_INFO) lpContext;
	IN_ADDR ClientAddress, HostAddress, NewHostAddress;
	PDP_NEWHOST pNewHost = NULL;
	CHAR szTemp[20] = "";

	ClientAddress.S_un.S_addr = FromAddr;
	HostAddress.S_un.S_addr = pSessionInfo->pHostInfo->dwHostAddr;

	switch(((PNETSYNC_GENERIC_MESSAGE) ReceiveMessage)->dwMessageId)
	{
	case DPMSG_HOSTKEEPALIVE:
		// If the sender of the keepalive has a different address than our recorded address for the host
		// there are two possiblities...
		if(FromAddr != pSessionInfo->pHostInfo->dwHostAddr)
		{

			// The first possiblity is that the host left earlier and we haven't received a NEWHOST
			// message... it's minimally possible that this message was lost.  But since we got our
			// currently listed host address is 0, we know that the original host must have told us
			// they were leaving.  So we at least know that we don't have two machines that both think
			// they are the host.  Mark the machine that sent this keepalive as the new host.
			if(!pSessionInfo->pHostInfo->dwHostAddr)
			{
				pSessionInfo->pHostInfo->dwHostAddr = FromAddr;
			}
			// The remaining possibilities are that either...
			// 1) The original host left the session in a non-graceful way so we never got a
			// notification that they were gone, and then we never received the NEWHOST message from
			// the machine that the host migrated to.  While it's possible that the NEWHOST message
			// was lost on the network, it is pretty unlikely.  So just kill the whole session.
			// 2) There is another machine on the network that thinks its the host and is sending us
			// keepalives.  This could have happened if we had to kill the server app.  It's possible
			// that this rogue host is not a part of this Netsync session anymore so we'll have to manually
			// send a STOPSESSION message to this machine and then return FALSE from this function to
			// tell Netsync to kill everyone else.
			else
			{
				strcpy(szTemp, inet_ntoa(ClientAddress));
				DbgPrint("Received a keepalive from %s but current host is listed as %s\n",
					szTemp, inet_ntoa(HostAddress));
				DbgPrint("Sending STOPSESSION to rogue server and ending this session\n");

				StopSessionMsg.dwMessageId = NETSYNC_MSG_STOPSESSION;
				NetsyncSendClientMessage(hNetsyncObject, FromAddr, sizeof(NETSYNC_GENERIC_MESSAGE), (char *) &StopSessionMsg);

				return FALSE;
			}
		}

		pSessionInfo->dwLastHostPing = GetTickCount();
		break;

	case DPMSG_NEWHOST:
		pNewHost = (PDP_NEWHOST) ReceiveMessage;

		NewHostAddress.S_un.S_addr = pNewHost->dwHostAddr;

		// This should never happen... a machine is sending a new host message
		// with another machine's IP address... Fail and check this out...
		if(pNewHost->dwHostAddr != FromAddr)
		{
			DbgPrint("Received new host message for %s from %s\n",
				inet_ntoa(NewHostAddress), inet_ntoa(ClientAddress));
			return FALSE;
		}

		// This shouldn't happen either... a machine is sending a new host message
		// when its already known to be the host...  Go ahead and let this slide,
		// but log it...
		if(pNewHost->dwHostAddr == pSessionInfo->pHostInfo->dwHostAddr)
		{
			DbgPrint("Received new host message for %s from %s\n",
				inet_ntoa(NewHostAddress), inet_ntoa(ClientAddress));;
		}

		DbgPrint("Host has migrated to %s\n", inet_ntoa(NewHostAddress));;

		// Update the new server and reset its ping time
		pSessionInfo->pHostInfo->dwHostAddr = pNewHost->dwHostAddr;
		pSessionInfo->dwLastHostPing = GetTickCount();
		break;

	default:
		// Unknown message received
		DbgPrint("WARINING: Unknown client message received 0x%08x\n",
			((PNETSYNC_GENERIC_MESSAGE) ReceiveMessage)->dwMessageId);
		break;
	}

	return TRUE;
}

//==================================================================================
// DpStressCallbackFunction
//----------------------------------------------------------------------------------
//
// Description: Function exported for Netsync callbacks
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	DWORD		FromAddr		Address of sender of message that caused the callback
//	DWORD		dwMessageType	Parameter indicating if this is a client or server message
//	DWORD		dwMessageSize	Parameter indicating the size of the message
//	CHAR		*ReceiveMessage	Buffer containing the packet that caused the callback
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	Returns TRUE if the message was received was successfully handled, FALSE otherwise
//==================================================================================
BOOL DpStressCallbackFunction(HANDLE hNetsyncObject, DWORD FromAddr, DWORD dwMessageType, DWORD dwMessageSize, CHAR *ReceiveMessage, LPVOID lpContext)
{
	switch(dwMessageType)
	{
	case NETSYNC_MSGTYPE_SERVER:
		return DpProcessServerStressMessage(hNetsyncObject, FromAddr, ReceiveMessage, lpContext);
	case NETSYNC_MSGTYPE_CLIENT:
		return DpProcessClientStressMessage(hNetsyncObject, FromAddr, ReceiveMessage, lpContext);
	default:
		DbgPrint("Unknown message type received: 0x%08x\n", dwMessageType);
		break;
	}

	return TRUE;
}

//==================================================================================
// DpStressStartFunction
//----------------------------------------------------------------------------------
//
// Description: Funtion called once for each stress session that is created
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	BYTE		byClientCount	Number of clients this test is being started with
//	DWORD		*ClientAddrs	List of client addresses (number of addresses indicated by byClientCount)
//	WORD		LowPort			The lowest port to use for this test
//	WORD		HighPort		The higest port to use for this test
//
// Returns:
//	Returns a pointer to the session information that is being stored for this session
//==================================================================================
LPVOID DpStressStartFunction(HANDLE hNetsyncObject, BYTE byClientCount, DWORD *ClientAddrs, WORD LowPort, WORD HighPort)
{
	PDPSTRESS_SESSION_INFO pCurrentSession = NULL;
	IN_ADDR CurrentAddr;
	BYTE n;

	pCurrentSession = (PDPSTRESS_SESSION_INFO) LocalAlloc(LPTR, sizeof(DPSTRESS_SESSION_INFO));
	if(!pCurrentSession)
	{
		DbgPrint("Couldn't allocate session info structure! (error %u)\n", GetLastError());
		return NULL;
	}

	DbgPrint("Starting stress session: 0x%08x\n", pCurrentSession);

	// Fill the session info structure
	pCurrentSession->hNetsyncObject = hNetsyncObject;
	pCurrentSession->byClientCount = byClientCount;
	pCurrentSession->ClientAddrs = ClientAddrs;
	pCurrentSession->dwLastHostPing = GetTickCount();
	pCurrentSession->LowPort = LowPort;
	pCurrentSession->HighPort = HighPort;

	// Stress sessions ignore the client addresses provided in the host info message
	// so the size of the DP_HOSTINFO structure is constant for this case

	// Allocate and initialize the host info message
	pCurrentSession->pHostInfo = (DP_HOSTINFO *) LocalAlloc(LPTR, sizeof(DP_HOSTINFO));
	if(!pCurrentSession->pHostInfo)
	{
		DbgPrint("Couldn't allocate host info message! (error %u)\n", GetLastError());
		return NULL;
	}

	memset(pCurrentSession->pHostInfo, 0, sizeof(DP_HOSTINFO));
	pCurrentSession->pHostInfo->dwMessageId = DPMSG_HOSTINFO;
	pCurrentSession->pHostInfo->dwTotalMachines = (DWORD) byClientCount;
	pCurrentSession->pHostInfo->dwHostAddr = ClientAddrs[0];

	// Send the host info message to all machines that started the test
	for(n = 0; n < byClientCount; ++n)
	{
		CurrentAddr.S_un.S_addr = ClientAddrs[n];

		if(n == 0)
		{
			DbgPrint("  Host is %s\n", inet_ntoa(CurrentAddr));
		}
		else
		{
			DbgPrint("  Client %u is %s\n", n, inet_ntoa(CurrentAddr));
		}

		NetsyncSendClientMessage(hNetsyncObject, ClientAddrs[n], sizeof(DP_HOSTINFO), (char *) (pCurrentSession->pHostInfo));
	}

	return (LPVOID) pCurrentSession;
}

//==================================================================================
// DpStressStopFunction
//----------------------------------------------------------------------------------
//
// Description: Function called once when each stress session is closed
//
// Arguments:
//	HANDLE		hNetsyncObject	Handle to calling instance
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
VOID DpStressStopFunction(HANDLE hNetsyncObject, LPVOID lpContext)
{
	PDPSTRESS_SESSION_INFO pCurrentSession = (PDPSTRESS_SESSION_INFO) lpContext;

	DbgPrint("Stopping stress session: 0x%08x\n", pCurrentSession);

	LocalFree(pCurrentSession->pHostInfo);
	LocalFree(pCurrentSession);

	return;
}

//==================================================================================
// Test functions
//==================================================================================

