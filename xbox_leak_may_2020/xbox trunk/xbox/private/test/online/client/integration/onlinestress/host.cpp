/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       host.cpp
 *  Content:    Code for the host algorithm of the integrated online stress test
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  01/08/01    tristanj Created to hold all code related to the client stress algorithm
 *
 ****************************************************************************/

#include "onlinestress.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;
extern CLogonTask *g_pLogonTask;
extern BOOL g_fExitTest;

/****************************************************************************
 *
 *  AdvertiseStressSession
 *
 *  Description:
 *      Sends a session creation/update request with the specified number of current
 *      and available slots.  It then pumps the task handle until completion.
 *
 *  Arguments:
 *      DWORD               dwPublicCurrent     Number of clients currently connected to the session
 *      DWORD               dwPublicAvailable   Number of client spaces still available in the session
 *      XNKID               *pSessionID         If the XNKID pointed to is set to all 0's, this is a
 *                                              session creation, otherwise update the session specified
 *                                              On exit, this will contain the returned XNKID
 *      XNKEY               *pKeyExchangeKey    On exit, this will contain the returned XNKEY
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT AdvertiseStressSession(CMatchSessionCreateTask *pSessionCreateTask, DWORD dwPublicCurrent, DWORD dwPublicAvailable, XNKID *pSessionID, XNKEY *pKeyExchangeKey)
{
	COnlineTimer Timer;
	HRESULT hr = S_OK;
	DWORD dwStartTick = 0, dwCurrentTick = 0;

	if(!pSessionCreateTask)
	{
		hr = E_FAIL;
		goto Exit;
	}

	pSessionCreateTask->SetAvailableSlots(dwPublicCurrent, dwPublicAvailable, 0, 0);

	// Start match session creation task
	if(!pSessionCreateTask->StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}

	Timer.SetAllowedTime(MAX_SESSION_CREATE_TIME);
	Timer.Start();

	// Pump task for MAX_SESSION_CREATE_TIME seconds
	while(!pSessionCreateTask->IsTaskComplete())
	{
		if(Timer.HasTimeExpired())
			break;

		g_pLogonTask->TaskContinue();
		pSessionCreateTask->TaskContinue();
	}

	if(!pSessionCreateTask->IsTaskComplete())
	{
		LOGTASKINFO(g_hLog, "Session creation timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	pSessionCreateTask->GetSessionInfo(pSessionID, pKeyExchangeKey);

	if(*((ULONGLONG *) pSessionID) == 0)
	{
		LOGTASKINFO(g_hLog, "Session ID wasn't set");
		hr = E_FAIL;
		goto Exit;
	}

Exit:

	return hr;
}

/****************************************************************************
 *
 *  ProcessNewClients
 *
 *  Description:
 *      Check the host socket for incoming connection requests and add them to the client
 *      connection list.
 *
 *  Arguments:
 *      CListenSocket          *pListenSocket      The host socket that accepts incoming connections
 *      XONSTRESS_CLIENTVECTOR *pClientVector      The list to add new client connections to
 *      DWORD                  *pdwPublicCurrent   On input, indicates current number of connected clients.
 *                                                 On output, indicates the new number of connected clients.
 *      DWORD                  dwPublicAvailable   Indicates the number of client slots left that are available
 *                                                 for incoming connections
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT ProcessNewClients(CListenSocket *pListenSocket, XONSTRESS_CLIENTVECTOR *pClientVector, DWORD *pdwPublicCurrent, DWORD dwPublicAvailable)
{
	HRESULT hr = S_OK, hrLogon = S_OK;
	CClientConnection *pNewClient = NULL;
	SOCKET NewClientSock = INVALID_SOCKET;
	XNADDR NewClientAddr;
	XNKID NewClientId;
	DWORD dwClientCount = 0;

	memset(&NewClientAddr, 0, sizeof(NewClientAddr));
	memset(&NewClientId, 0, sizeof(NewClientId));

	// Loop and process all pending incoming connections
	while(((hr = pListenSocket->IsConnectionPending()) == S_OK) && dwPublicAvailable)
	{

		// Process any available incoming connections
		if(hr == S_OK)
		{
			hr = pListenSocket->AcceptIncomingClient(&NewClientSock, NULL, &NewClientAddr, &NewClientId);
			if(hr == S_OK)
			{
				pNewClient = new CClientConnection;
				pNewClient->SetSocket(NewClientSock);
				
				hr = ReadNewClientInfo(NewClientSock, pNewClient);
				if(hr != S_OK)
				{
					closesocket(NewClientSock);
					break;
				}
				
				TRACE(g_hLog, "Client at %02X%2X%02X%02X%02X%02X joined session", 
					pNewClient->m_ClientAddr.abEnet[0],
					pNewClient->m_ClientAddr.abEnet[1],
					pNewClient->m_ClientAddr.abEnet[2],
					pNewClient->m_ClientAddr.abEnet[3],
					pNewClient->m_ClientAddr.abEnet[4],
					pNewClient->m_ClientAddr.abEnet[5]);

				// Do one final check to verify that the client and the server see
				// the address the same.  Otherwise, other clients won't be able to see
				// this client
				if(memcmp(&NewClientAddr, &(pNewClient->m_ClientAddr), sizeof(NewClientAddr)))
				{
					LOGTASKINFO(g_hLog, "Address seen by host and that reported by client are different!");
					return E_FAIL;
				}

				pNewClient->SetAsNew();

				// Add a copy of the client to the vector
				pClientVector->push_back(*pNewClient);
				++(*pdwPublicCurrent);
				--dwPublicAvailable;

				// Remove the original client
				delete pNewClient;
				pNewClient = NULL;

			}

			// Since reading the client information could take time,
			// go ahead and process the logon task for 100 ms
			if(FAILED(hrLogon = WaitAndPump(GetTickCount() + 100, g_pLogonTask)))
			{
				LOGTASKINFOHR(g_hLog, "Online task handle failed", hrLogon);
				hr = E_FAIL;
				break;
			}
		}
		
		// If there were no incoming connections pending, then we're done
		// Don't use else if because the accept phase could return S_FALSE as well
		if(hr == S_FALSE)
		{
			hr = S_OK;
			break;
		}
		// Otherwise, we received some error code
		else
			break;;
	}

	// Clean up any leftover client object
	if(pNewClient)
		delete pNewClient;

	return hr;
}


/****************************************************************************
 *
 *  ProcessIncomingHeartbeats
 *
 *  Description:
 *      Check the currently connected clients and process any incoming heartbeats
 *
 *  Arguments:
 *      XONSTRESS_CLIENTVECTOR *pClientVector      The list of client connections to process for heartbeats
 *
 *  Returns:  
 *      S_OK on success
 *      E_FAIL on system failure
 *
 ****************************************************************************/
HRESULT ProcessIncomingHeartbeats(XONSTRESS_CLIENTVECTOR *pClientVector)
{
	COnlineTimer ReadTimer;
	XONSTRESS_CLIENTVECTOR::iterator ClientCurrent, ClientEnd;
	fd_set readfds;
	INT nActiveSockets = 0;
	HRESULT hr = S_OK, hrLogon = S_OK;
    TIMEVAL sTimeout = {0, 0}; 
	DWORD dwHeartbeat = 0;
	BOOL fSocketsToProcess = FALSE;
	
	ReadTimer.SetAllowedTime(XONSTRESS_HOST_RECV_TIME);

	while(TRUE)
	{
		FD_ZERO(&readfds);

		// Loop through all users in the user vector and add to the total required size for the client packet
		for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
		{
			FD_SET(ClientCurrent->GetSocket(), &readfds);
			fSocketsToProcess = TRUE;
		}
		
		if(!fSocketsToProcess)
		{
			hr = S_OK;
			goto Exit;
		}

		nActiveSockets = select(0, &readfds, NULL, NULL, &sTimeout);
		
		if(nActiveSockets <= 0)
		{
			hr = S_OK;
			goto Exit;
		}
		
//		TRACE(g_hLog, "Data pending from %u clients", nActiveSockets);

		// Loop through all clients in the client vector and read any pending heartbeats
		for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
		{
			// Handle data on the socket
			if(FD_ISSET(ClientCurrent->GetSocket(), &readfds))
			{
				hr = ReadPacket(ClientCurrent->GetSocket(), (CHAR *) &dwHeartbeat, sizeof(DWORD), &ReadTimer);
				if(FAILED(hr))
				{
					LOGTASKINFO(g_hLog, "A socket error occured on a client, marking as disconnected");
					ClientCurrent->SetAsDisconnected();
					hr = S_OK;
					continue;
				}
				else if(hr == S_OK)
				{
					// If the received message wasn't a heartbeat, then there was a socket error, so fail
					if(dwHeartbeat != sizeof(DWORD))
					{
						LOGTASKINFO(g_hLog, "An unexpected message was received from a client, marking as disconnected");
						ClientCurrent->SetAsDisconnected();
						continue;
					}
					else
					{
						ClientCurrent->ResetIncomingHeartbeatTimer();
					}
				}
				else if(hr == S_FALSE)
				{
					TRACE(g_hLog, "Timed out reading info from client");
				}

			}
		}
		
		hr = S_OK;

		// Since reading the client information could take time,
		// go ahead and process the logon task for 100 ms
		if(FAILED(hrLogon = WaitAndPump(GetTickCount() + 100, g_pLogonTask)))
		{
			LOGTASKINFOHR(g_hLog, "Online task handle failed", hrLogon);
			hr = E_FAIL;
			break;
		}
	}

Exit:

	return hr;
}

/****************************************************************************
 *
 *  ProcessDisconnectedClients
 *
 *  Description:
 *      Check the currently connected clients and process any incoming heartbeats
 *
 *  Arguments:
 *      XONSTRESS_CLIENTVECTOR *pClientVector      The list of client connections to check for disconnections
 *      DWORD                  *pdwPublicCurrent   Number of public slots currently available in this session
 *
 *  Returns:  
 *      S_OK on success
 *      E_FAIL on system failure
 *
 ****************************************************************************/
HRESULT ProcessDisconnectedClients(XONSTRESS_CLIENTVECTOR *pClientVector, DWORD *pdwNewPublicCurrent)
{
	XONSTRESS_CLIENTVECTOR::iterator ClientCurrent, ClientEnd;

	// Loop through all clients in the client vector and mark any that haven't responded
	// within the timeout interval as disconnected (this also closes the socket)
	for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
	{
		if(!ClientCurrent->IsDisconnected())
		{
			if(ClientCurrent->IsIncomingHearbeatExpired())
			{
				TRACE(g_hLog, "Client at %02X%2X%02X%02X%02X%02X timed out", 
					ClientCurrent->m_ClientAddr.abEnet[0],
					ClientCurrent->m_ClientAddr.abEnet[1],
					ClientCurrent->m_ClientAddr.abEnet[2],
					ClientCurrent->m_ClientAddr.abEnet[3],
					ClientCurrent->m_ClientAddr.abEnet[4],
					ClientCurrent->m_ClientAddr.abEnet[5]);

				LOGTASKINFO(g_hLog, "Timed out waiting for client");
				ClientCurrent->SetAsDisconnected();

				--(*pdwNewPublicCurrent);
			}
		}
	}

	return S_OK;
}

/****************************************************************************
 *
 *  BuildFullClientList
 *
 *  Description:
 *      Go through the client vector and add all new and existing users to the
 *      full client list buffer which will be transmitted to remote clients
 *
 *  Arguments:
 *      XONSTRESS_CLIENTVECTOR *pClientVector      The list of client connections to add to the list packet
 *      CHAR                   *pBuff              Buffer to hold the client/user information.  If this
 *                                                 parameter is NULL and a non-NULL pdwBuffSize is
 *                                                 provided, then the function will fill pdwBuffSize
 *                                                 with the required size necessary
 *      DWORD                  *pdwBuffSize        Holds the size of the provided buffer. On output
 *                                                 this parameter will contain either the required size
 *                                                 (if the provided size was insufficient) or the
 *                                                 ammount of space that was actually used
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if the provided buffer was too small
 *      E_FAIL on system failure
 *
 ****************************************************************************/
HRESULT BuildFullClientList(XONSTRESS_CLIENTVECTOR *pClientVector, CHAR *pBuff, DWORD *pdwBuffSize)
{
	PXONSTRESS_CLIENTLIST_HEADER pClientListHeader = (PXONSTRESS_CLIENTLIST_HEADER) pBuff;
	XONSTRESS_CLIENTVECTOR::iterator ClientCurrent, ClientEnd;
	HRESULT hr = S_OK;
	DWORD dwCurrentClientSize = 0, dwRequiredSize = 0, dwClientCount = 0, dwUsedSize = 0;

	if(!pClientVector || !pdwBuffSize)
	{
		hr = E_FAIL;
		goto Exit;
	}

	dwRequiredSize += sizeof(XONSTRESS_CLIENTLIST_HEADER);

	// Loop through all clients in the client vector and determine the required list size
	for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
	{
		// Don't count disconnected clients since they won't be included in the full list
		if(ClientCurrent->IsDisconnected())
			continue;

		dwCurrentClientSize = 0;

		hr = ClientCurrent->PackIntoBuffer(NULL, &dwCurrentClientSize);

		dwRequiredSize += dwCurrentClientSize;
		++dwClientCount;
	}

	if(*pdwBuffSize < dwRequiredSize)
	{
		*pdwBuffSize = dwRequiredSize;
		hr = S_FALSE;
		goto Exit;
	}

	if(!pBuff)
	{
		hr = E_FAIL;
		goto Exit;
	}

	pClientListHeader->dwClientCount = dwClientCount;
	pClientListHeader->dwClientListSize = dwRequiredSize;
	dwUsedSize += sizeof(XONSTRESS_CLIENTLIST_HEADER);

	// Loop through all clients in the client vector and pack them into the buffer
	for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
	{
		// Don't count disconnected clients since they won't be include in the full list
		if(ClientCurrent->IsDisconnected())
			continue;

		dwCurrentClientSize = dwRequiredSize - dwUsedSize;

		hr = ClientCurrent->PackIntoBuffer(pBuff + dwUsedSize, &dwCurrentClientSize);

		dwUsedSize += dwCurrentClientSize;
	}

Exit:

	return hr;
}

/****************************************************************************
 *
 *  BuildUpdateClientList
 *
 *  Description:
 *      Go through the client vector and add all new and disconnected users to
 *      the full client list buffer which will be transmitted to remote clients
 *
 *  Arguments:
 *      XONSTRESS_CLIENTVECTOR *pClientVector      The list of client connections to add to the list packet
 *      CHAR                   *pBuff              Buffer to hold the client/user information.  If this
 *                                                 parameter is NULL and a non-NULL pdwBuffSize is
 *                                                 provided, then the function will fill pdwBuffSize
 *                                                 with the required size necessary
 *      DWORD                  *pdwBuffSize        Holds the size of the provided buffer. On output
 *                                                 this parameter will contain either the required size
 *                                                 (if the provided size was insufficient) or the
 *                                                 ammount of space that was actually used
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if the provided buffer was too small
 *      E_FAIL on system failure
 *
 ****************************************************************************/
HRESULT BuildUpdateClientList(XONSTRESS_CLIENTVECTOR *pClientVector, CHAR *pBuff, DWORD *pdwBuffSize)
{
	PXONSTRESS_CLIENTLIST_HEADER pClientListHeader = (PXONSTRESS_CLIENTLIST_HEADER) pBuff;
	XONSTRESS_CLIENTVECTOR::iterator ClientCurrent, ClientEnd;
	HRESULT hr = S_OK;
	DWORD dwCurrentClientSize = 0, dwRequiredSize = 0, dwClientCount = 0, dwUsedSize = 0;

	if(!pClientVector || !pdwBuffSize)
	{
		hr = E_FAIL;
		goto Exit;
	}

	dwRequiredSize += sizeof(XONSTRESS_CLIENTLIST_HEADER);

	// Loop through all clients in the client vector and determine the required list size
	for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
	{
		// Don't count existing clients since they won't be included in the update list
		if(!ClientCurrent->IsDisconnected() && !ClientCurrent->IsNew())
			continue;

		dwCurrentClientSize = 0;

		hr = ClientCurrent->PackIntoBuffer(NULL, &dwCurrentClientSize);

		dwRequiredSize += dwCurrentClientSize;
		++dwClientCount;
	}

	if(*pdwBuffSize < dwRequiredSize)
	{
		*pdwBuffSize = dwRequiredSize;
		hr = S_FALSE;
		goto Exit;
	}

	if(!pBuff)
	{
		hr = E_FAIL;
		goto Exit;
	}

	pClientListHeader->dwClientCount = dwClientCount;
	pClientListHeader->dwClientListSize = dwRequiredSize;
	dwUsedSize += sizeof(XONSTRESS_CLIENTLIST_HEADER);

	// Loop through all clients in the client vector and pack them into the buffer
	for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
	{
		// Don't count existing clients since they won't be included in the update list
		if(!ClientCurrent->IsDisconnected() && !ClientCurrent->IsNew())
			continue;

		dwCurrentClientSize = dwRequiredSize - dwUsedSize;

		hr = ClientCurrent->PackIntoBuffer(pBuff + dwUsedSize, &dwCurrentClientSize);

		dwUsedSize += dwCurrentClientSize;
	}

Exit:

	return hr;
}

/****************************************************************************
 *
 *  ProcessListChangesAndHeartbeats
 *
 *  Description:
 *      Builds lists of client/user changes and sends them to the appropriate clients
 *      If there is no list to send, and the outgoing heartbeat timer has expired,
 *      then sends a heartbeat to the appropriate clients
 *
 *  Arguments:
 *      XONSTRESS_CLIENTVECTOR *pClientVector      The list of client connections to check for disconnections
 *
 *  Returns:  
 *      S_OK on success
 *      E_FAIL on system failure
 *
 ****************************************************************************/
HRESULT ProcessListChangesAndHeartbeats(XONSTRESS_CLIENTVECTOR *pClientVector)
{
	COnlineTimer SendTimer;
	XONSTRESS_CLIENTVECTOR::iterator ClientCurrent, ClientEnd;
	HRESULT hr = S_OK;
	CHAR *pFullClientList = NULL, *pUpdateClientList = NULL;
	DWORD dwFullListSize = 0, dwUpdateListSize = 0, dwOutgoingHeartbeat = sizeof(DWORD);
	BOOL fNewClientsExist = FALSE;
	BOOL fDisconnectedClientsExist = FALSE;

	SendTimer.SetAllowedTime(XONSTRESS_HOST_SEND_TIME);

	// Loop through all clients in the client vector and see if any are new or disconnected
	for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
	{
		if(ClientCurrent->IsNew())
			fNewClientsExist = TRUE;

		if(ClientCurrent->IsDisconnected())
			fDisconnectedClientsExist = TRUE;
	}

	// If there are new clients, then build the full client list for those clients
	if(fNewClientsExist)
	{
		// Determine the required size of the list
		if(FAILED(BuildFullClientList(pClientVector, NULL, &dwFullListSize)))
		{
			LOGTASKINFO(g_hLog, "Failed to create the full client list");
			hr = E_FAIL;
			goto Exit;
		}

		// Allocate the list
		pFullClientList = new CHAR[dwFullListSize];

		// Build the full list
		if(FAILED(BuildFullClientList(pClientVector, pFullClientList, &dwFullListSize)))
		{
			LOGTASKINFO(g_hLog, "Failed to create the full client list");
			hr = E_FAIL;
			goto Exit;
		}
	}
	
	// If there have been client list changes, then build a list of changes to send to existing clients
	if(fNewClientsExist || fDisconnectedClientsExist)
	{
		// Build the update list
		if(FAILED(BuildUpdateClientList(pClientVector, NULL, &dwUpdateListSize)))
		{
			LOGTASKINFO(g_hLog, "Failed to create the update client list");
			hr = E_FAIL;
			goto Exit;
		}

		// Allocate the list
		pUpdateClientList = new CHAR[dwUpdateListSize];

		// Build the update list
		if(FAILED(BuildUpdateClientList(pClientVector, pUpdateClientList, &dwUpdateListSize)))
		{
			LOGTASKINFO(g_hLog, "Failed to create the update client list");
			hr = E_FAIL;
			goto Exit;
		}
	}

	// Removed deleted clients from the list
	for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
	{
		if(ClientCurrent->IsDisconnected())
		{
			pClientVector->erase(ClientCurrent);
		}
	}

	// Send lists and heartbeats to the appropriate clients
	for(ClientCurrent = pClientVector->begin(), ClientEnd = pClientVector->end(); ClientCurrent != ClientEnd; ++ClientCurrent)
	{
		// If the client disconnected, then don't send any list data
		if(ClientCurrent->IsDisconnected())
			continue;

		// If the client is new, then send a full list update
		if(ClientCurrent->IsNew())
		{
			TRACE(g_hLog, "Sending full list (%u bytes) to client at %02X%2X%02X%02X%02X%02X", 
				dwFullListSize,
				ClientCurrent->m_ClientAddr.abEnet[0],
				ClientCurrent->m_ClientAddr.abEnet[1],
				ClientCurrent->m_ClientAddr.abEnet[2],
				ClientCurrent->m_ClientAddr.abEnet[3],
				ClientCurrent->m_ClientAddr.abEnet[4],
				ClientCurrent->m_ClientAddr.abEnet[5]);

			// Don't worry about if the send fails, since the lack of heartbeat will cause this client
			// to be removed eventually anyway
			if((hr = SendPacket(ClientCurrent->GetSocket(), pFullClientList, (INT) dwFullListSize, &SendTimer)) != S_OK)
			{
				TRACE(g_hLog, "Send failed with 0x%08x", hr);
				hr = S_OK;
			}

			ClientCurrent->SetAsOld();
			ClientCurrent->ResetOutgoingHeartbeatTimer();
		}
		// If the client is old and there were list changes, then send the updates
		else if(fNewClientsExist || fDisconnectedClientsExist)
		{
			TRACE(g_hLog, "Sending updated list (%u bytes) to client at %02X%2X%02X%02X%02X%02X", 
				dwUpdateListSize,
				ClientCurrent->m_ClientAddr.abEnet[0],
				ClientCurrent->m_ClientAddr.abEnet[1],
				ClientCurrent->m_ClientAddr.abEnet[2],
				ClientCurrent->m_ClientAddr.abEnet[3],
				ClientCurrent->m_ClientAddr.abEnet[4],
				ClientCurrent->m_ClientAddr.abEnet[5]);

			// Don't worry about if the send fails, since the lack of heartbeat will cause this client
			// to be removed eventually anyway
			if((hr = SendPacket(ClientCurrent->GetSocket(), pUpdateClientList, (INT) dwUpdateListSize, &SendTimer)) != S_OK)
			{
				TRACE(g_hLog, "Send failed with 0x%08x", hr);
				hr = S_OK;
			}

			ClientCurrent->ResetOutgoingHeartbeatTimer();
		}
		// Otherwise, if an outgoing heartbeat is due, send one
		else if(ClientCurrent->IsOutgoingHeartbeatExpired())
		{
//			TRACE(g_hLog, "Sending heartbeat (%u bytes) to client at %02X%2X%02X%02X%02X%02X",
//				sizeof(DWORD),
//				ClientCurrent->m_ClientAddr.abEnet[0],
//				ClientCurrent->m_ClientAddr.abEnet[1],
//				ClientCurrent->m_ClientAddr.abEnet[2],
//				ClientCurrent->m_ClientAddr.abEnet[3],
//				ClientCurrent->m_ClientAddr.abEnet[4],
//				ClientCurrent->m_ClientAddr.abEnet[5]);
			// Don't worry about if the send fails, since the lack of heartbeat will cause this client
			// to be removed eventually anyway
			if((hr = SendPacket(ClientCurrent->GetSocket(), (CHAR *) &dwOutgoingHeartbeat, sizeof(DWORD), &SendTimer)) != S_OK)
			{
				TRACE(g_hLog, "Send failed with 0x%08x", hr);
				hr = S_OK;
			}
			ClientCurrent->ResetOutgoingHeartbeatTimer();
		}
	}

Exit:

	pFullClientList ? delete [] pFullClientList : 0;
	pUpdateClientList ? delete [] pUpdateClientList : 0;

	return hr;
}

/****************************************************************************
 *
 *  host_Main
 *
 *  Description:
 *      Master algorithm for the online stress host
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT host_Main( void )
{
	XONSTRESS_CLIENTVECTOR ClientVector;
	XONLINE_ATTRIBUTE AttributesArray[1];
	CMatchSessionCreateTask SessionCreateTask;
	COnlineTimer HostWaitTimer, SessionRefreshTimer;
	CListenSocket *pListenSocket = NULL;
    HRESULT hr = S_OK;
	DWORD dwPublicCurrent = 0, dwNewPublicCurrent = 0, dwAttributesCount = (sizeof(AttributesArray) / sizeof(XONLINE_ATTRIBUTE));
	XNKID SessionID;
	XNKEY KeyExchangeKey;

	AttributesArray[0].dwAttributeID = XONLINESTRESS_SESSION_PARAM;
	AttributesArray[0].info.integer.qwValue = XONLINESTRESS_SESSION_VALUE;
	AttributesArray[0].fChanged = TRUE;
	
	SessionCreateTask.RegisterLogHandle(g_hLog);
	SessionCreateTask.SetAttributes(AttributesArray, dwAttributesCount);

	memset(&SessionID, 0, sizeof(XNKID));
	memset(&KeyExchangeKey, 0, sizeof(XNKEY));

    //
    // Create and advertise a new session on the match server
    //
	hr = AdvertiseStressSession(&SessionCreateTask, dwPublicCurrent, DEFAULT_SLOTS_AVAILABLE - dwPublicCurrent, &SessionID, &KeyExchangeKey);
	if(hr != S_OK)
		goto Exit;

	//
	// Register the host's key information
	//
	if(XNetRegisterKey(&SessionID, &KeyExchangeKey))
	{
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Create a socket to listen for incoming clients
	//
	hr = CreateListenSocket(&pListenSocket, &SessionID, &KeyExchangeKey);
	if(hr != S_OK)
		goto Exit;

	//
	// Start main host loop
	// 

	HostWaitTimer.SetAllowedTime(XONSTRESS_HOST_WAIT_TIME);
	SessionRefreshTimer.SetAllowedTime(XONSTRESS_SESSION_REFRESH_TIME);
	SessionRefreshTimer.Start();

	while(SUCCEEDED(hr) && !g_fExitTest)
	{
		//
		// Process new client connections incoming from the host socket
		//
		dwNewPublicCurrent = dwPublicCurrent;
		hr = ProcessNewClients(pListenSocket, &ClientVector, &dwNewPublicCurrent, DEFAULT_SLOTS_AVAILABLE - dwNewPublicCurrent);
		// We expect either S_OK or S_FALSE if there wasn't a network error
		if(FAILED(hr))
			goto Exit;

		if(dwNewPublicCurrent > DEFAULT_SLOTS_AVAILABLE)
			goto Exit;

		//
		// If new clients have connected update the session advertisement to reflect the new number of slots available
		//
		if(dwNewPublicCurrent > dwPublicCurrent)
		{
			dwPublicCurrent = dwNewPublicCurrent;
	
			hr = AdvertiseStressSession(&SessionCreateTask, dwPublicCurrent, DEFAULT_SLOTS_AVAILABLE - dwPublicCurrent, &SessionID, &KeyExchangeKey);
			if(hr != S_OK)
				goto Exit;
		}

		//
		// Process new client heartbeats incoming from the client connections
		//
		hr = ProcessIncomingHeartbeats(&ClientVector);
		if(hr != S_OK)
			goto Exit;

		//
		// Process new client disconnects
		//
		dwNewPublicCurrent = dwPublicCurrent;
		hr = ProcessDisconnectedClients(&ClientVector, &dwNewPublicCurrent);
		if(hr != S_OK)
			goto Exit;

		//
		// If new clients have connected update the session advertisement to reflect the new number of slots available
		//
		if(dwNewPublicCurrent < dwPublicCurrent)
		{
			dwPublicCurrent = dwNewPublicCurrent;
	
			hr = AdvertiseStressSession(&SessionCreateTask, dwPublicCurrent, DEFAULT_SLOTS_AVAILABLE - dwPublicCurrent, &SessionID, &KeyExchangeKey);
			if(hr != S_OK)
				goto Exit;
		}

		//
		// Send list changes and heartbeats to clients
		//
		hr = ProcessListChangesAndHeartbeats(&ClientVector);
		if(hr != S_OK)
			goto Exit;

		// Wait for XONSTRESS_HOST_WAIT_TIME seconds to throttle the speed at which connections and disconnections occur
		HostWaitTimer.Start();
		while(!HostWaitTimer.HasTimeExpired())
		{
			Sleep(0);
		}

		// Keep the session advertisement from being erased by refreshing it every 15 minutes
		if(SessionRefreshTimer.HasTimeExpired())
		{
			hr = AdvertiseStressSession(&SessionCreateTask, dwPublicCurrent, DEFAULT_SLOTS_AVAILABLE - dwPublicCurrent, &SessionID, &KeyExchangeKey);
			if(hr != S_OK)
				goto Exit;
		}

		PUMPLOGONHANDLE();
	}

Exit:

	//
	// Close the host socket and client connections
	//
	pListenSocket ? delete pListenSocket : 0;


    //
    // Return the Hresult
    //

    return hr;
}
