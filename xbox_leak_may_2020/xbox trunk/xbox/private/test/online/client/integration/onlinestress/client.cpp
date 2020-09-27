/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       client.cpp
 *  Content:    Code for the client algorithm of the integrated online stress test
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

static DWORD g_dwMinuteCounter = 0;
static DWORD g_dwFriendRequestCount = 0;
static DWORD g_dwFriendAcceptCount = 0;
static DWORD g_dwFriendRejectCount = 0;
static DWORD g_dwFriendRemoveCount = 0;
static DWORD g_dwPresenceStateChangeCount = 0;
static DWORD g_dwFriendListChangeCount = 0;
static DWORD g_dwFriendGameInviteCount = 0;
static DWORD g_dwFriendGameInviteRevokeCount = 0;
static DWORD g_dwFriendGameInviteAcceptCount = 0;
static DWORD g_dwFriendGameInviteRejectCount = 0;
static DWORD g_dwMatchSearchCount = 0;
static ULONGLONG g_qwBytesReceived = 0;
static ULONGLONG g_qwBytesSent = 0;

/****************************************************************************
 *
 *  CreateClientConnectionToHost
 *
 *  Description:
 *      Creates a connection to the host and sends the local user information
 *
 *  Arguments:
 *      CClientConnection     *pClientConnection  Pointer to a client connection that is to be filled
 *                                                connection information with the host
 *      XNADDR                *pHostAddr          Address of the host
 *      XNKID                 *pSessionID         XNKID to register with the network stack
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if the socket timed out waiting to connect
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT CreateClientConnectionToHost(CClientConnection *pClientConnection, XNADDR *pHostAddr, XNKID *pSessionID)
{
	COnlineTimer ConnectionTimer;
	SOCKADDR_IN HostAddress;
	HRESULT hr = S_OK;
	SOCKET HostSocket = INVALID_SOCKET;
    fd_set writefds, exceptfds;
	TIMEVAL sTimeout = {0, 0}; 
    ULONG uNonBlockParam = TRUE;
	INT nActiveSockets = 0;

	HostAddress.sin_family = AF_INET;
	HostAddress.sin_port = htons(XONSTRESS_HOST_PORT);

	//
	// Get a dummy IP address binding for the host's address
	//
	if(XNetXnAddrToInAddr(pHostAddr, pSessionID, &(HostAddress.sin_addr)))
	{
		LOGTASKINFO(g_hLog, "Error getting a private INADDR from host's XNADDR");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Create the socket to communicate with the host
	//
	HostSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
	if(HostSocket == INVALID_SOCKET)
	{
		LOGTASKINFO(g_hLog, "Error creating socket");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Set non-blocking IO
	//
	if(SOCKET_ERROR == ioctlsocket(HostSocket, FIONBIO, &uNonBlockParam))
	{
		LOGTASKINFO(g_hLog, "Error setting client socket to use non-blocking IO");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Connect to the host socket
	//
	if(connect(HostSocket, (const struct sockaddr *) &HostAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			LOGTASKINFO(g_hLog, "Error connecting to host");
			hr = E_FAIL;
			goto Exit;
		}
	}

	//
	// Wait until socket finishes connecting
	//

	ConnectionTimer.SetAllowedTime(XONSTRESS_MAX_CONNECT_TIME);
	ConnectionTimer.Start();

	do
	{
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
		FD_SET(HostSocket, &writefds);
		FD_SET(HostSocket, &exceptfds);

		nActiveSockets = select(0, NULL, &writefds, &exceptfds, &sTimeout);

		if(FAILED(WaitAndPump(0, g_pLogonTask)))
		{
			LOGTASKINFO(g_hLog, "Error pumping logon handle");
			hr = E_FAIL;
			goto Exit;
		}

		Sleep(0);

	} while((nActiveSockets <= 0) && (!ConnectionTimer.HasTimeExpired()));

    if(nActiveSockets <= 0)
    {
        hr = S_FALSE;
        goto Exit;
    }
                
    if(FD_ISSET(HostSocket, &exceptfds))
    {
        hr = E_FAIL;
        goto Exit;
    }

    if(!FD_ISSET(HostSocket, &writefds))
    {
        hr = S_FALSE;
        goto Exit;
    }

	hr = S_OK;

	pClientConnection->SetSocket(HostSocket);

Exit:

	if(hr == E_FAIL)
	{
		closesocket(HostSocket);
		HostSocket = INVALID_SOCKET;
	}

	return hr;
}

/****************************************************************************
 *
 *  SendClientInfoToServer
 *
 *  Description:
 *      Creates a connection to the host and sends the local user information
 *
 *  Arguments:
 *      CClientConnection     *pClientConnection  Pointer to a client connection that will be
 *                                                used to send info to the server
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if the socket timed out waiting to connect
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT SendClientInfoToServer(CClientConnection *pClientConnection)
{
	COnlineTimer SendTimer;
	CSessionUser *pUser = NULL;
	PXONLINE_USER pLogonUsers = NULL, pCurrentUser = NULL;
	HRESULT hr = S_OK;
	DWORD dwClientBuffSize = 0;
	CHAR *pClientBuff = NULL;
	INT n = 0;

	//
	// Get the local address of the client
	//
	if(!(XNetGetTitleXnAddr(&(pClientConnection->m_ClientAddr)) & XNET_GET_XNADDR_ONLINE))
	{
		LOGTASKINFO(g_hLog, "Error retrieving title XNADDR");
        hr = E_FAIL;
        goto Exit;
	}

	//
	// Get information about all currently logged on users
	//
	if(!(pLogonUsers = XOnlineGetLogonUsers()))
	{
		LOGTASKINFO(g_hLog, "Error retrieving locally logged on users");
        hr = E_FAIL;
        goto Exit;
	}

	//
	// Add user information to the client connection object
	//
	for(n = 0, pCurrentUser = pLogonUsers;n < XONLINE_MAX_LOGON_USERS; ++n,++pCurrentUser)
	{
		if(!(pUser = new CSessionUser))
		{
			LOGTASKINFO(g_hLog, "Not enough memory to allocate session user");
			hr = E_FAIL;
			goto Exit;
		}
		pUser->SetUserInfo(&(pCurrentUser->xuid), pCurrentUser->name, pCurrentUser->kingdom);
		pClientConnection->AddUser(*pUser);
		delete pUser;
		pUser = NULL;
	}

	//
	// Retrieve the required client buffer size
	//
	if(FAILED(pClientConnection->PackIntoBuffer(NULL, &dwClientBuffSize)))
	{
		LOGTASKINFO(g_hLog, "Error retrieving required client buffer size");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Allocate the client buffer
	//
	if(!(pClientBuff = new CHAR[dwClientBuffSize]))
	{
		LOGTASKINFO(g_hLog, "Not enough memory to allocate client buffer");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Pack the client information into the buffer
	//
	if(pClientConnection->PackIntoBuffer(pClientBuff, &dwClientBuffSize) != S_OK)
	{
		LOGTASKINFO(g_hLog, "Error packing client information into buffer");
		hr = E_FAIL;
		goto Exit;
	}

	SendTimer.SetAllowedTime(XONSTRESS_CLIENT_SEND_TIME);

	TRACE(g_hLog, "Sending client info to host");

	//
	// Send the client information.  This should never timeout, so fail in that case
	//
	if(SendPacket(pClientConnection->GetSocket(), pClientBuff, (INT) dwClientBuffSize, &SendTimer) != S_OK)
	{
		LOGTASKINFO(g_hLog, "Timed out on first send!");
		hr = E_FAIL;
		goto Exit;
	}

Exit:

	if(pUser)
	{
		delete pUser;
		pUser = NULL;
	}

	return hr;
}

/****************************************************************************
 *
 *  RemoveClientFromVector
 *
 *  Description:
 *      Scans through the client vector for the specified client and removes it
 *      (A client is considered equal to the specified client if it has the same XNADDR)
 *
 *  Arguments:
 *      CClientConnection      *pClientConnection Pointer to a client connection to be removed
 *      XONSTRESS_CLIENTVECTOR *pClientVector     Pointer to the vector of clients connected
 *                                                to this session
 *
 *  Returns:  
 *      (none)
 *
 ****************************************************************************/
void RemoveClientFromVector(CClientConnection *pNewClient, XONSTRESS_CLIENTVECTOR *pClientVector)
{
	XONSTRESS_CLIENTVECTOR::iterator CurrentClient, ClientEnd;

	// If pNewClient is present in the client vector, remove it
	for(CurrentClient = pClientVector->begin(), ClientEnd = pClientVector->end(); CurrentClient != ClientEnd; ++CurrentClient)
	{
		if(*CurrentClient == *pNewClient)
		{
			pClientVector->erase(CurrentClient);
			return;
		}
	}

	return;
}

/****************************************************************************
 *
 *  ProcessNewInfoFromHost
 *
 *  Description:
 *      Processes new list information from the host and adds it to the client vector
 *
 *  Arguments:
 *      CClientConnection      *pClientConnection Pointer to a client connection that will be
 *                                                receiving data from the host
 *      XONSTRESS_CLIENTVECTOR *pClientVector     Pointer to the vector of clients connected
 *                                                to this session
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if the socket timed out waiting to received data
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT ProcessNewInfoFromHost(CClientConnection *pClientConnection, XONSTRESS_CLIENTVECTOR *pClientVector)
{
	COnlineTimer ReadTimer;
    fd_set readfds, exceptfds;
	TIMEVAL sTimeout = {0, 0}; 
	INT nActiveSockets = 0;
	HRESULT hr = S_OK;
	XONSTRESS_CLIENTLIST_HEADER ClientListHeader;
	DWORD dwCurrentOffset = 0, dwCurrentClient = 0, dwLastClientSize = 0;
	CHAR *pClientListBuffer = NULL;
	CClientConnection *pNewClient = NULL;
	ULONG uBytesAvailable = 0;

	ReadTimer.SetAllowedTime(XONSTRESS_CLIENT_RECV_TIME);

	//
	// Poll the socket to see if there is any pending data to read
	//
    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_SET(pClientConnection->GetSocket(), &readfds);
    FD_SET(pClientConnection->GetSocket(), &exceptfds);

	nActiveSockets = select(0, &readfds, NULL, &exceptfds, &sTimeout);

	if(FAILED(WaitAndPump(0, g_pLogonTask)))
	{
		LOGTASKINFO(g_hLog, "Error pumping logon handle");
		hr = E_FAIL;
		goto Exit;
	}

    if(nActiveSockets <= 0)
    {
        hr = S_FALSE;
        goto Exit;
    }

    if(FD_ISSET(pClientConnection->GetSocket(), &exceptfds))
    {
        hr = E_FAIL;
        goto Exit;
    }

    if(!FD_ISSET(pClientConnection->GetSocket(), &readfds))
    {
        hr = S_FALSE;
        goto Exit;
    }

	if(SOCKET_ERROR == ioctlsocket(pClientConnection->GetSocket(), FIONREAD, &uBytesAvailable))
	{
		TRACE(g_hLog, "Error determining number of bytes pending on socket");
        hr = E_FAIL;
        goto Exit;
	}

//	TRACE(g_hLog, "%u bytes pending on host socket", uBytesAvailable);

	//
	// Read pending data from the socket
	//

	memset(&ClientListHeader, 0, sizeof(ClientListHeader));
	hr = ReadPacket(pClientConnection->GetSocket(), (CHAR *) &(ClientListHeader.dwClientListSize), sizeof(DWORD), &ReadTimer);
	if(FAILED(hr))
	{
		LOGTASKINFO(g_hLog, "A socket error occured, disconnecting");
		pClientConnection->SetAsDisconnected();
		goto Exit;
	}
	else if(hr == S_FALSE)
	{
		TRACE(g_hLog, "Timed out reading info from host");
		goto Exit;
	}

	// Update the last received heartbeat time
	pClientConnection->ResetIncomingHeartbeatTimer();

	// If the pending data is a heartbeat, exit
	if(ClientListHeader.dwClientListSize == sizeof(DWORD))
	{
//		TRACE(g_hLog, "Received heartbeat from host");
		goto Exit;
	}

//	TRACE(g_hLog, "Receiving %u bytes of list data from host", ClientListHeader.dwClientListSize);

	// If the pending data is too small, then exit
	if(ClientListHeader.dwClientListSize <= sizeof(XONSTRESS_CLIENTLIST_HEADER))
	{
		LOGTASKINFO(g_hLog, "Bad client list received");
		goto Exit;
	}

	// Read the remaining data 
	hr = ReadPacket(pClientConnection->GetSocket(), (CHAR *) &(ClientListHeader.dwClientCount), sizeof(DWORD), &ReadTimer);
	if(FAILED(hr))
	{
		LOGTASKINFO(g_hLog, "A socket error occured, disconnecting");
		pClientConnection->SetAsDisconnected();
		goto Exit;
	}
	else if(hr == S_FALSE)
	{
		TRACE(g_hLog, "Timed out reading info from host");
		goto Exit;
	}

	// If there are no clients in the list, then exit
	if(!ClientListHeader.dwClientCount)
	{
		TRACE(g_hLog, "Received empty list from host");
		goto Exit;
	}

	ClientListHeader.dwClientListSize -= sizeof(XONSTRESS_CLIENTLIST_HEADER);

	for(dwCurrentOffset = 0, dwCurrentClient = 0, dwLastClientSize = 0;
	(dwCurrentOffset < ClientListHeader.dwClientListSize) && (dwCurrentClient < ClientListHeader.dwClientCount);
	dwCurrentOffset += dwLastClientSize, ++dwCurrentClient)
	{
//		TRACE(g_hLog, "Reading client %u from host", dwCurrentClient + 1);

		// Read the client header
		hr = ReadPacket(pClientConnection->GetSocket(), (CHAR *) &dwLastClientSize, sizeof(DWORD), &ReadTimer);
		if(FAILED(hr))
		{
			LOGTASKINFO(g_hLog, "A socket error occured, disconnecting");
			pClientConnection->SetAsDisconnected();
			goto Exit;
		}
		else if(hr == S_FALSE)
		{
			TRACE(g_hLog, "Timed out reading info from host");
			goto Exit;
		}

		if(dwLastClientSize < sizeof(XONSTRESS_CLIENT_HEADER))
		{
			LOGTASKINFO(g_hLog, "Bad client info received");
			continue;
		}

		// Allocate a buffer to read the next client info
		pClientListBuffer = new CHAR[dwLastClientSize];
		memset(pClientListBuffer, 0, dwLastClientSize);
		*((DWORD *) pClientListBuffer) = dwLastClientSize;

//		TRACE(g_hLog, "Receiving %u bytes of client data from host", ClientListHeader.dwClientListSize);

		// Read the client header
		hr = ReadPacket(pClientConnection->GetSocket(), pClientListBuffer + sizeof(DWORD), dwLastClientSize - sizeof(DWORD), &ReadTimer);
		if(FAILED(hr))
		{
			LOGTASKINFO(g_hLog, "A socket error occured, disconnecting");
			pClientConnection->SetAsDisconnected();
			goto Exit;
		}
		else if(hr == S_FALSE)
		{
			TRACE(g_hLog, "Timed out reading info from host");
			goto Exit;
		}

		// Unpack the client info from the buffer into a new client object
		pNewClient = new CClientConnection;
		hr = pNewClient->UnpackFromBuffer(pClientListBuffer, &dwLastClientSize);
		if(FAILED(hr))
		{
			LOGTASKINFO(g_hLog, "Error unpacking client from buffer");
			goto Exit;
		}

		// If this is a client disconnect, then remove this client from the master client list
		if(pNewClient->IsDisconnected())
		{
			TRACE(g_hLog, "Marking client at %02X%2X%02X%02X%02X%02X as disconnected", 
				pNewClient->m_ClientAddr.abEnet[0],
				pNewClient->m_ClientAddr.abEnet[1],
				pNewClient->m_ClientAddr.abEnet[2],
				pNewClient->m_ClientAddr.abEnet[3],
				pNewClient->m_ClientAddr.abEnet[4],
				pNewClient->m_ClientAddr.abEnet[5]);
			RemoveClientFromVector(pNewClient, pClientVector);
		}
		// Otherwise, add the new client to the master client list
		else
		{
			TRACE(g_hLog, "Adding new client at %02X%2X%02X%02X%02X%02X", 
				pNewClient->m_ClientAddr.abEnet[0],
				pNewClient->m_ClientAddr.abEnet[1],
				pNewClient->m_ClientAddr.abEnet[2],
				pNewClient->m_ClientAddr.abEnet[3],
				pNewClient->m_ClientAddr.abEnet[4],
				pNewClient->m_ClientAddr.abEnet[5]);
			pClientVector->push_back(*pNewClient);
		}

		delete pNewClient;
		pNewClient = NULL;

		delete [] pClientListBuffer;
		pClientListBuffer = NULL;
	}


Exit:

	// If we didn't receive any data from the server, and the timeout has expired, then fail
	if(hr == S_FALSE)
	{
		if(pClientConnection->IsIncomingHearbeatExpired())
		{
			LOGTASKINFO(g_hLog, "Timed out waiting for server");
			hr = E_FAIL;
		}
	}

	if(pNewClient)
	{
		delete pNewClient;
		pNewClient = NULL;
	}

	if(pClientListBuffer)
	{
		delete [] pClientListBuffer;
		pClientListBuffer = NULL;
	}

	return hr;
}

/****************************************************************************
 *
 *  SendClientHeartbeat
 *
 *  Description:
 *      Sends a heartbeat message to the session host if the timeout has expired
 *
 *  Arguments:
 *      CClientConnection      *pClientConnection  Client connection to send a heartbeat on
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT SendClientHeartbeat(CClientConnection *pClientConnection)
{
	HRESULT hr = S_OK;
	DWORD dwOutgoingHeartbeat = sizeof(DWORD);

	if(pClientConnection->IsOutgoingHeartbeatExpired())
	{
		// Don't worry about if the send fails, since the lack of heartbeat will cause this client
		// to be removed eventually anyway
//		TRACE(g_hLog, "Sending heartbeat to host");

		if((hr = SendPacket(pClientConnection->GetSocket(), (CHAR *) &dwOutgoingHeartbeat, sizeof(DWORD), NULL)) != S_OK)
		{
			TRACE(g_hLog, "Send failed with 0x%08x", hr);
			hr = S_OK;
		}
		pClientConnection->ResetOutgoingHeartbeatTimer();
	}

	return hr;
}

/****************************************************************************
 *
 *  PickUserToInvite
 *
 *  Description:
 *      Pick a random user from the client list and return their XUID for invitation
 *
 *  Arguments:
 *      XONSTRESS_CLIENTVECTOR  *pClientVector     Vector containing info on all clients and users connected
 *                                                 to this session
 *      XUID                    *pTargetUserXUID   Pointer to return the target user XUID
 *      CHAR                    *szTargetUserName  Pointer to return the target user name
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
BOOL PickUserToInvite(XONSTRESS_CLIENTVECTOR *pClientVector, XUID *pTargetUserXUID, CHAR *szTargetUserName)
{
	DWORD dwNumClients = pClientVector->size();
	DWORD dwTargetClientIndex = 0, dwTargetUserIndex = (rand() % XONLINE_MAX_LOGON_USERS);

	if(!dwNumClients)
		return FALSE;

	dwTargetClientIndex = (rand() % dwNumClients);

	return ((*pClientVector)[dwTargetClientIndex]).GetRandomUserInfo(pTargetUserXUID, szTargetUserName);
}

/****************************************************************************
 *
 *  PickFriendToAcceptReject
 *
 *  Description:
 *      Pick the first pending friend from the client list and return their XONLINE_FRIEND info for accept/reject
 *
 *  Arguments:
 *      CFriendEnumTask         *pFriendEnumTask   Enumeration task for this user (lets us get at their friends list)
 *      XONLINE_FRIEND          *pTargetFriend     Pointer for handing back the friend to accept/reject
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
BOOL PickFriendToAcceptReject(CFriendEnumTask *pFriendEnumTask, XONLINE_FRIEND *pTargetFriend)
{
	XONLINE_FRIEND *pFriendsList = NULL;
	DWORD dwFriendCount = 0, dwFriendIndex = 0;

	dwFriendCount = pFriendEnumTask->GetLatestFriendList(&pFriendsList);

	for(dwFriendIndex = 0; dwFriendIndex < dwFriendCount; ++dwFriendIndex)
	{
		// If the request flag is set, then we need to either accept or reject
		if(pFriendsList[dwFriendIndex].friendState & XONLINE_FRIENDSTATE_FLAG_REQUEST)
		{
			if(pTargetFriend)
				memcpy(pTargetFriend, pFriendsList + dwFriendIndex, sizeof(XONLINE_FRIEND));

			return TRUE;
		}
	}

	return FALSE;
}

/****************************************************************************
 *
 *  PickInvitingFriendForAction
 *
 *  Description:
 *      Pick the first friend that has invited us to a game session and return their XONLINE_FRIEND info
 *
 *  Arguments:
 *      CFriendEnumTask         *pFriendEnumTask   Enumeration task for this user (lets us get at their friends list)
 *      XONLINE_FRIEND          *pTargetFriend     Pointer for handing back the friend that has invited us to a game
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT PickInvitingFriendForAction(CFriendEnumTask *pFriendEnumTask, XONLINE_FRIEND *pTargetFriend)
{
	XONLINE_FRIEND *pFriendsList = NULL;
	DWORD dwFriendCount = 0, dwFriendIndex = 0;

	dwFriendCount = pFriendEnumTask->GetLatestFriendList(&pFriendsList);

	for(dwFriendIndex = 0; dwFriendIndex < dwFriendCount; ++dwFriendIndex)
	{
		// If the received invite flag is set, then this friend has invited us
		if(pFriendsList[dwFriendIndex].friendState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE)
		{
			if(pTargetFriend)
				memcpy(pTargetFriend, pFriendsList + dwFriendIndex, sizeof(XONLINE_FRIEND));

			return TRUE;
		}
	}

	return FALSE;
}

/****************************************************************************
 *
 *  PickInvitedFriendForAction
 *
 *  Description:
 *      Pick the first friend that we have invited to a game session and return their XONLINE_FRIEND info
 *
 *  Arguments:
 *      CFriendEnumTask         *pFriendEnumTask   Enumeration task for this user (lets us get at their friends list)
 *      XONLINE_FRIEND          *pTargetFriend     Pointer for handing back the friend that we have invited to a game
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT PickInvitedFriendForAction(CFriendEnumTask *pFriendEnumTask, XONLINE_FRIEND *pTargetFriend)
{
	XONLINE_FRIEND *pFriendsList = NULL;
	DWORD dwFriendCount = 0, dwFriendIndex = 0;

	dwFriendCount = pFriendEnumTask->GetLatestFriendList(&pFriendsList);

	for(dwFriendIndex = 0; dwFriendIndex < dwFriendCount; ++dwFriendIndex)
	{
		// If the received invite flag is set, then this friend has invited us
		if(pFriendsList[dwFriendIndex].friendState & XONLINE_FRIENDSTATE_FLAG_SENTINVITE)
		{
			if(pTargetFriend)
				memcpy(pTargetFriend, pFriendsList + dwFriendIndex, sizeof(XONLINE_FRIEND));

			return TRUE;
		}
	}

	return FALSE;
}


/****************************************************************************
 *
 *  PickFriendForAction
 *
 *  Description:
 *      Pick the first friend from the client list and return their XONLINE_FRIEND info for some invitation action
 *
 *  Arguments:
 *      CFriendEnumTask         *pFriendEnumTask   Enumeration task for this user (lets us get at their friends list)
 *      XONLINE_FRIEND          *pTargetFriend     Pointer for handing back the friend to remove
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
BOOL PickFriendForAction(CFriendEnumTask *pFriendEnumTask, XONLINE_FRIEND *pTargetFriend)
{
	XONLINE_FRIEND *pFriendsList = NULL;
	DWORD dwFriendCount = 0, dwFriendIndex = 0;

	dwFriendCount = pFriendEnumTask->GetLatestFriendList(&pFriendsList);

	for(dwFriendIndex = 0; dwFriendIndex < dwFriendCount; ++dwFriendIndex)
	{
		// If the request and pending flags aren't set, then we can remove this friend
		if(!(pFriendsList[dwFriendIndex].friendState & (XONLINE_FRIENDSTATE_FLAG_REQUEST | XONLINE_FRIENDSTATE_FLAG_PENDING)))
		{
			if(pTargetFriend)
				memcpy(pTargetFriend, pFriendsList + dwFriendIndex, sizeof(XONLINE_FRIEND));

			return TRUE;
		}
	}

	return FALSE;
}

/****************************************************************************
 *
 *  PickFriendForRemoval
 *
 *  Description:
 *      Pick the first friend from the client list and return their XONLINE_FRIEND info for removal 
 *
 *  Arguments:
 *      CFriendEnumTask         *pFriendEnumTask   Enumeration task for this user (lets us get at their friends list)
 *      XONLINE_FRIEND          *pTargetFriend     Pointer for handing back the friend to remove
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
BOOL PickFriendForRemoval(CFriendEnumTask *pFriendEnumTask, XONLINE_FRIEND *pTargetFriend)
{
	XONLINE_FRIEND *pFriendsList = NULL;
	DWORD dwFriendCount = 0, dwFriendIndex = 0;

	dwFriendCount = pFriendEnumTask->GetLatestFriendList(&pFriendsList);

	for(dwFriendIndex = 0; dwFriendIndex < dwFriendCount; ++dwFriendIndex)
	{
		// If the request flag isn't set, then we can remove this friend
		if(!(pFriendsList[dwFriendIndex].friendState & XONLINE_FRIENDSTATE_FLAG_REQUEST))
		{
			if(pTargetFriend)
				memcpy(pTargetFriend, pFriendsList + dwFriendIndex, sizeof(XONLINE_FRIEND));

			return TRUE;
		}
	}

	return FALSE;
}

/****************************************************************************
 *
 *  FriendListChange
 *
 *  Description:
 *      Make a change to the friends list
 *
 *  Arguments:
 *      CFriendOpTask           *pFriendOpTask     Pointer to a friend operation task that will be
 *                                                 used to issue friend list requests
 *      CFriendEnumTask         *pFriendEnumArray  Array of friend enumeration tasks.  Used to access
 *                                                 users friends for accepting, rejecting and removing them
 *      XONNSTRESS_CLIENTVECTOR *pClientVector     Pointer to the list of clients and users in the session
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT FriendListChange(CFriendOpTask *pFriendOpTask, CFriendEnumTask *pFriendEnumArray, XONSTRESS_CLIENTVECTOR *pClientVector)
{
	HRESULT hr = S_OK;
	DWORD dwUserDieRoll = (rand() % XONLINE_MAX_LOGON_USERS), dwActionDieRoll = (rand() & 100);
	PXONLINE_USER pCurrentUser = NULL;
	XONLINE_FRIEND TargetFriend;
	XUID TargetXUID;
	CHAR szTargetName[XONLINE_NAME_SIZE];

	pCurrentUser = XOnlineGetLogonUsers();
	pCurrentUser += dwUserDieRoll;

	// If this user doesn't have any friends, then invite someone
	if(pFriendEnumArray[dwUserDieRoll].GetLatestFriendList(NULL) == 0)
	{
		if(!PickUserToInvite(pClientVector, &TargetXUID, szTargetName))
		{
			// This is OK, because we might not have gotten any client info yet
			TRACE(g_hLog, "Failed to pick a user to send a friend invite");
			goto Exit;
		}

		// We can't invite ourselves
		if(pCurrentUser->xuid.qwUserID == TargetXUID.qwUserID)
		{
			goto Exit;
		}

		if(!pFriendOpTask->RequestFriend(dwUserDieRoll, TargetXUID))
		{
			TRACE(g_hLog, "Failed to issue friend request");
			hr = E_FAIL;
			goto Exit;
		}

		++g_dwFriendRequestCount;

//		TRACE(g_hLog, "Issuing friend request to %S", szTargetName);
	}
	// If there are pending invites, then there should be a relatively high priority of handling
	else if(PickFriendToAcceptReject(pFriendEnumArray + dwUserDieRoll, &TargetFriend))
	{		
		// 80% chance we'll accept (this will get friend lists larger)
		if(dwActionDieRoll < 80)
		{
			if(!pFriendOpTask->AnswerFriendRequest(dwUserDieRoll, &TargetFriend, XONLINE_REQUEST_YES))
			{
				TRACE(g_hLog, "Failed to accept friend request");
				hr = E_FAIL;
				goto Exit;
			}
			
			++g_dwFriendAcceptCount;

//			TRACE(g_hLog, "Accepting friend invite from %S", TargetFriend.username);
		}
		// 20% chance we'll reject
		else
		{
			if(!pFriendOpTask->AnswerFriendRequest(dwUserDieRoll, &TargetFriend, XONLINE_REQUEST_NO))
			{
				TRACE(g_hLog, "Failed to accept friend request");
				hr = E_FAIL;
				goto Exit;
			}
			
			++g_dwFriendRejectCount;

//			TRACE(g_hLog, "Rejecting friend invite from %S", TargetFriend.username);
		}
		
	}
	// If this user has the maximum number of friends, then remove one
	else if(pFriendEnumArray[dwUserDieRoll].GetLatestFriendList(NULL) == MAX_FRIENDS)
	{
		if(!PickFriendForRemoval(pFriendEnumArray + dwUserDieRoll, &TargetFriend))
		{
			TRACE(g_hLog, "Failed to pick a user to remove");
			hr = E_FAIL;
			goto Exit;
		}
		
		if(!pFriendOpTask->RemoveFriend(dwUserDieRoll, &TargetFriend))
		{
			TRACE(g_hLog, "Failed to remove friend");
			hr = E_FAIL;
			goto Exit;
		}
		
		++g_dwFriendRemoveCount;

//		TRACE(g_hLog, "Removing %S from friends list", TargetFriend.username);
	}
	// Otherwise, we've got friends, but there are no pending requests
	// We'll have to either remove someone or invite someone new
	else
	{
		// 80% chance we'll invite another user (this will get friend lists larger)
		if(dwActionDieRoll < 80)
		{
			if(!PickUserToInvite(pClientVector, &TargetXUID, szTargetName))
			{
				// This is OK, because we might not have gotten any client info yet
				TRACE(g_hLog, "Failed to pick a user to send a friend invite");
				goto Exit;
			}
		
			// We can't invite ourselves
			if(pCurrentUser->xuid.qwUserID == TargetXUID.qwUserID)
			{
				goto Exit;
			}

			if(!pFriendOpTask->RequestFriend(dwUserDieRoll, TargetXUID))
			{
				TRACE(g_hLog, "Failed to issue friend request");
				hr = E_FAIL;
				goto Exit;
			}
			
			++g_dwFriendRequestCount;

//			TRACE(g_hLog, "Issuing friend request to %S", szTargetName);
		}
		// 20% chance we'll remove a user
		else
		{
			if(!PickFriendForRemoval(pFriendEnumArray + dwUserDieRoll, &TargetFriend))
			{
				TRACE(g_hLog, "Failed to pick a user to remove");
				hr = E_FAIL;
				goto Exit;
			}
			
			if(!pFriendOpTask->RemoveFriend(dwUserDieRoll, &TargetFriend))
			{
				TRACE(g_hLog, "Failed to remove friend");
				hr = E_FAIL;
				goto Exit;
			}
			
			++g_dwFriendRemoveCount;

//			TRACE(g_hLog, "Removing %S from friends list", TargetFriend.username);
		}
	}

Exit:

	return hr;
}

/****************************************************************************
 *
 *  PresenceStatusChange
 *
 *  Description:
 *      Changes the presence status for a random user
 *
 *  Arguments:
 *      (none)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT PresenceStatusChange(XNKID *pCurrentSession)
{
	HRESULT hr = S_OK;
	DWORD dwUserDieRoll = (rand() % XONLINE_MAX_LOGON_USERS);
	DWORD dwRandomStateFlags = rand() & 0x000000FF, cbData = 0;
	BYTE bDataIndex = 0;
	BYTE *pData = NULL;

	cbData = (rand() % MAX_STATEDATA_SIZE);
	
	if(cbData)
	{
		pData = new BYTE[cbData];
		if(!pData)
		{
			TRACE(g_hLog, "Couldn't allocate state data");
			hr = E_FAIL;
			goto Exit;
		}
		// Fill the state buffer with random data
		for(bDataIndex = 0;bDataIndex < cbData;++bDataIndex)
		{
			pData[bDataIndex] = (BYTE) (rand() & 0x000000FF);
		}
	}
	
	// Make sure the state flags use valid values
	dwRandomStateFlags &= (XONLINE_FRIENDSTATE_FLAG_ONLINE
		| XONLINE_FRIENDSTATE_FLAG_PLAYING
		| XONLINE_FRIENDSTATE_FLAG_CLOAKED
		| XONLINE_FRIENDSTATE_FLAG_VOICE
		| XONLINE_FRIENDSTATE_FLAG_JOINABLE);
	// And make sure the state flags still say that we're online
	dwRandomStateFlags |= XONLINE_FRIENDSTATE_FLAG_ONLINE;

	if(!XOnlineNotificationSetState(dwUserDieRoll, dwRandomStateFlags, *pCurrentSession, cbData, pData))
	{
		TRACE(g_hLog, "Failed to set presence state");
		hr = E_FAIL;
		goto Exit;
	}

	++g_dwPresenceStateChangeCount;

Exit:

	if(pData)
		delete [] pData;

	return hr;
}

/****************************************************************************
 *
 *  GameInviteChange
 *
 *  Description:
 *      Changes the presence status for a random user
 *
 *  Arguments:
 *      CFriendOpTask    *pFriendOpTask     Pointer to friend operation task for performing
 *                                          game invite operations
 *      CFriendEnumTask  *pFriendEnumArray  Array of friend enumeration tasks.  Used to access
 *                                          users friends for issuing game invites
 *      XNKID            *pSessionID        Session ID of the current session
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT GameInviteChange(CFriendOpTask *pFriendOpTask, CFriendEnumTask *pFriendEnumArray, XNKID *pSessionID)
{
	XONLINE_GAMEINVITE_ANSWER_TYPE Answer = XONLINE_GAMEINVITE_NO;
	HRESULT hr = S_OK;
	DWORD dwUserDieRoll = (rand() % XONLINE_MAX_LOGON_USERS), dwActionDieRoll = (rand() % 100);
	XONLINE_FRIEND TargetFriend;

	if(!PickFriendForAction(pFriendEnumArray + dwUserDieRoll, NULL))
	{
		goto Exit;
	}

	// If we've received an invite, but haven't responded to it yet, do so
	if(PickInvitingFriendForAction(pFriendEnumArray + dwUserDieRoll, &TargetFriend))
	{
		// We have an 80% chance of handling an incoming invite
		if(dwActionDieRoll < 80)
		{
			// Roll the die again
			dwActionDieRoll = (rand() % 100);

			// We have a 50% chance of accepting the game invite
			if(dwActionDieRoll < 50)
				Answer = XONLINE_GAMEINVITE_YES;
			// We have a 50% chance of rejecting the game invite
			else
				Answer = XONLINE_GAMEINVITE_NO;

			if(!pFriendOpTask->AnswerGameInvite(dwUserDieRoll, &TargetFriend, Answer))
			{
				TRACE(g_hLog, "Failed to answer game invite");
				hr = E_FAIL;
				goto Exit;
			}

			if(Answer == XONLINE_GAMEINVITE_YES)
			{
				++g_dwFriendGameInviteAcceptCount;
//				TRACE(g_hLog, "Accepted game invite from %S", TargetFriend.username);
			}
			else
			{
				++g_dwFriendGameInviteRejectCount;
//				TRACE(g_hLog, "Rejected game invite from %S", TargetFriend.username);
			}

			goto Exit;
		}
		//Otherwise, we'll fall through to invite a new friend to a game
	}
	// If we've issued an invite, act accordingly
	else if(PickInvitedFriendForAction(pFriendEnumArray + dwUserDieRoll, &TargetFriend))
	{
		// We have an 20% chance of revoking an existing invite
		if(dwActionDieRoll < 20)
		{
			if(!pFriendOpTask->RevokeGameInvite(dwUserDieRoll, *pSessionID, 1, &TargetFriend))
			{
				TRACE(g_hLog, "Failed to revoke game invite");
				hr = E_FAIL;
				goto Exit;
			}
			
			++g_dwFriendGameInviteRevokeCount;

//			TRACE(g_hLog, "Revoked game invite to  %S", TargetFriend.username);

			goto Exit;
		}
		//Otherwise, we'll fall through to invite a new friend to a game
	}

	// If none of the previous actions were taken, then issue a new game invite
	if(!PickFriendForAction(pFriendEnumArray + dwUserDieRoll, &TargetFriend))
	{
		TRACE(g_hLog, "Failed to pick a user to invite to a game");
		hr = E_FAIL;
		goto Exit;
	}

	if(!pFriendOpTask->GameInvite(dwUserDieRoll, *pSessionID, 1, &TargetFriend))
	{
		TRACE(g_hLog, "Failed to revoke game invite");
		hr = E_FAIL;
		goto Exit;
	}

	++g_dwFriendGameInviteCount;

//	TRACE(g_hLog, "Issued game invite to  %S", TargetFriend.username);

Exit:

	return hr;
}

/****************************************************************************
 *
 *  CreateClientUDPSocket
 *
 *  Description:
 *      Creates a UDP socket and binds it to a specific port
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      INVALID_SOCKET on failure
 *      Valid socket handle on success
 *
 ****************************************************************************/
SOCKET CreateClientUDPSocket()
{
	SOCKET ClientSocket = INVALID_SOCKET;
    SOCKADDR_IN	LocalSockAddr = { AF_INET };
	DWORD dwNonBlocking = TRUE;

	// Create the socket
	ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(ClientSocket == INVALID_SOCKET)
	{
		TRACE(g_hLog, "Socket creation failed with 0x%08x", WSAGetLastError());
		goto Exit;
	}

	// Create the socket	
	if(ioctlsocket(ClientSocket, FIONBIO, &dwNonBlocking) == SOCKET_ERROR)
	{
		TRACE(g_hLog, "Setting for non-blocking IO failed with 0x%08x", WSAGetLastError());
		goto Exit;
	}

    LocalSockAddr.sin_port = htons(XONSTRESS_HOST_PORT);
    LocalSockAddr.sin_addr.s_addr = INADDR_ANY;
	
	// Bind the socket to the local address
    if (bind(ClientSocket, (const struct sockaddr *) &LocalSockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
		TRACE(g_hLog, "Socket binding failed with 0x%08x", WSAGetLastError());
		goto Exit;
    }

Exit:

	return ClientSocket;
}

/****************************************************************************
 *
 *  SendUDPPacketToRandomClient
 *
 *  Description:
 *      Picks a client at random and sends an IP datagram to it
 *
 *  Arguments:
 *      SOCKET                   ClientSocket       Socket to send the datagram from
 *      XNKID                    *pSessionID        XNKID of this session
 *      XONSTRESS_CLIENTVECTOR   *pClientVector     Pointer to the list of clients and users in the session
 *
 *  Returns:  
 *      TRUE on successful send
 *      FALSE on any failure other than WSAEWOULDBLOCK
 *
 ****************************************************************************/
BOOL SendUDPPacketToRandomClient(SOCKET ClientSocket, XNKID *pSessionID, XONSTRESS_CLIENTVECTOR *pClientVector)
{
	XNADDR TargetXnAddr, LocalXnAddr;
	SOCKADDR_IN TargetSockAddr;
	DWORD dwNumClients = pClientVector->size();
	DWORD dwTargetClientIndex = 0;
	CHAR szPacketBuff[256];
	INT cbPacketSize = (rand() % 256), nCurrentByte = 0, nBytesSent = 0;
	BOOL fRet = TRUE;

	// This should only happen if we haven't gotten the client list from the host yet
	if(!dwNumClients)
		return TRUE;

	// Randomly pick a target client
	dwTargetClientIndex = (rand() % dwNumClients);

	TargetSockAddr.sin_family = AF_INET;
	TargetSockAddr.sin_port = htons(XONSTRESS_HOST_PORT);

	if(!(XNetGetTitleXnAddr(&LocalXnAddr) & XNET_GET_XNADDR_ONLINE))
	{
		TRACE(g_hLog, "Error retrieving title XNADDR");
        fRet = FALSE;
        goto Exit;
	}

	// Don't send to ourselves
	if(!(memcmp(&LocalXnAddr, &(((*pClientVector)[dwTargetClientIndex]).m_ClientAddr), sizeof(XNADDR))))
	{
		fRet = TRUE;
		goto Exit;
	}

	// Get the address information
	if(XNetXnAddrToInAddr(&(((*pClientVector)[dwTargetClientIndex]).m_ClientAddr), pSessionID, &(TargetSockAddr.sin_addr)))
	{
		// Allow failure here, because this may be due to network conditions
		TRACE(g_hLog, "Error getting a private INADDR from host's XNADDR");
		fRet = TRUE;
		goto Exit;
	}

	// Generate the random data
	for(nCurrentByte = 0;nCurrentByte < cbPacketSize; ++nCurrentByte)
	{
		szPacketBuff[nCurrentByte] = (CHAR) (rand() & 0x000000FF);
	}

	nBytesSent = sendto(ClientSocket, szPacketBuff, cbPacketSize, 0, (sockaddr *) &TargetSockAddr, sizeof(TargetSockAddr));
	if(nBytesSent == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			TRACE(g_hLog, "Unexpected error on UDP socket send 0x%08x", WSAGetLastError());
			fRet = FALSE;
			goto Exit;
		}
	}

	g_qwBytesSent += nBytesSent;

Exit:
	return fRet;
}

/****************************************************************************
 *
 *  ReceiveUDPData
 *
 *  Description:
 *      Checks the given socket for any incoming datagrams and reads them
 *
 *  Arguments:
 *      SOCKET                   ClientSocket       Socket to recv datagrams from
 *
 *  Returns:  
 *      TRUE on success
 *      FALSE on failure unexpected socket error
 *
 ****************************************************************************/
BOOL ReceiveUDPData(SOCKET ClientSocket)
{
	COnlineTimer ReadTimer;
	CHAR szPacketBuff[256];
	TIMEVAL sTimeout = {0, 0}; 
	FD_SET readfds;
	BOOL fRet = TRUE;
	INT nActiveSockets = 0, nBytesReceived = 0;

	ReadTimer.SetAllowedTime(5);

	FD_ZERO(&readfds);
	FD_SET(ClientSocket, &readfds);

	nActiveSockets = select(0, &readfds, NULL, NULL, &sTimeout);

	while((nActiveSockets > 0) && (!ReadTimer.HasTimeExpired()));
	{
		nBytesReceived = recvfrom(ClientSocket, (char *) szPacketBuff, sizeof(szPacketBuff), 0, NULL, NULL);
		if(nBytesReceived == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK)
			{
				TRACE(g_hLog, "Unexpected error on UDP socket recv 0x%08x", WSAGetLastError());
				fRet = FALSE;
			}

			goto Exit;
		}
		g_qwBytesReceived += nBytesReceived;

		FD_ZERO(&readfds);
		FD_SET(ClientSocket, &readfds);
		
		nActiveSockets = select(0, &readfds, NULL, NULL, &sTimeout);
	}

Exit:

	return fRet;
}

/****************************************************************************
 *
 *  FindValidHostSession
 *
 *  Description:
 *      Loop through all of the returned search results and try connecting to each
 *      until one replies or we have exhausted them all
 *
 *  Arguments:
 *      CMatchSearchTask      *pMatchSearchTask  Pointer to a match search task that has results
 *      CClientConnection     **ppClientConn     Used to pass back a client connection object
 *                                               which allows the client to communicate with the host
 *      XNKID                 *pSessionID        Returns the key ID of the host connected to
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if all hosts were tried but none responded
 *      E_FAIL on an unexpected socket failure
 *
 ****************************************************************************/
HRESULT FindValidHostSession(CMatchSearchTask *pMatchSearchTask, CClientConnection **ppClientConn, XNKID *pSessionID)
{
	PXMATCH_SEARCHRESULT *rgpSearchResults = NULL;
	XNADDR HostAddress;
	XNKEY KeyExchangeKey;
	HRESULT hr = S_OK;
	DWORD dwAvailableResults = 0, dwCurrentResult = 0;
	BOOL fConnectedToHost = FALSE;

	if(!pSessionID || !ppClientConn)
	{
		TRACE(g_hLog, "Invalid params");
		goto Exit;
	}

	// Retrieve the search results
	hr = pMatchSearchTask->GetSearchResults(&rgpSearchResults, &dwAvailableResults);
	if(hr != S_OK)
	{
		TRACE(g_hLog, "Failed getting search results: 0x%08x", hr);
		goto Exit;
	}

	// Loop through the search results and try connecting to each host
	for(dwCurrentResult = 0;(dwCurrentResult < dwAvailableResults) && (!fConnectedToHost);++dwCurrentResult)
	{
		// Return all of the info needed to connect to this session
		memcpy(&HostAddress, &((rgpSearchResults[dwCurrentResult])->HostAddress), sizeof(XNADDR));
		memcpy(pSessionID, &((rgpSearchResults[dwCurrentResult])->SessionID), sizeof(XNKID));
		memcpy(&KeyExchangeKey, &((rgpSearchResults[dwCurrentResult])->KeyExchangeKey), sizeof(XNKEY));

		//
		// Register the host's key information
		//
		if(XNetRegisterKey(pSessionID, &KeyExchangeKey))
		{
			TRACE(g_hLog, "Failed registering key");
			hr = E_FAIL;
			goto Exit;
		}
		
		//
		// Create a socket to connect to the session host
		//
		*ppClientConn = new CClientConnection;
		if(!*ppClientConn)
		{
			TRACE(g_hLog, "Failed creating client connection");
			hr = E_FAIL;
			goto Exit;
		}
		
		hr = CreateClientConnectionToHost(*ppClientConn, &HostAddress, pSessionID);
		if(hr != S_OK)
		{
			XNetUnregisterKey(pSessionID);
			delete *ppClientConn;
			*ppClientConn = NULL;
			continue;
		}

		fConnectedToHost = TRUE;
	}

	if(fConnectedToHost)
		hr = S_OK;

Exit:

	if(FAILED(hr) && *ppClientConn)
		delete *ppClientConn;

	return hr;
}

/****************************************************************************
 *
 *  client_Main
 *
 *  Description:
 *      Master algorithm for the online stress client
 *
 *  Arguments:
 *      CMatchSearchTask      *pMatchSearchTask   Pointer to a match search task that has results
 *
 *  Returns:  
 *      S_OK on success

 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT client_Main(CMatchSearchTask *pMatchSearchTask)
{
	COnlineTimer FriendEnumTimer, FriendActionTimer, StatsTimer, SendTimer;
	CFriendOpTask FriendOpTask;
	CFriendEnumTask FriendEnumArray[XONLINE_MAX_LOGON_USERS];
	XONSTRESS_CLIENTVECTOR ClientVector;
	CClientConnection *pClientConnection = NULL;
	XNKID SessionID;
	HRESULT hr = S_OK;
	SOCKET ClientSocket = INVALID_SOCKET;
	DWORD dwCurrentUser = 0, LastFriendCountArray[XONLINE_MAX_LOGON_USERS], dwNewFriendCount = 0, dwDieRoll = 0;

	//
	// Loop through the available sessions and try to connect to each
	//
	hr = FindValidHostSession(pMatchSearchTask, &pClientConnection, &SessionID);
	if(hr != S_OK)
	{
		TRACE(g_hLog, "No valid sessions found");
		hr = E_FAIL;
		goto Exit;
	}

	// Create enumeration handles for all users
	for(dwCurrentUser = 0;dwCurrentUser < XONLINE_MAX_LOGON_USERS;++dwCurrentUser)
	{
		LastFriendCountArray[dwCurrentUser] = 0;
		FriendEnumArray[dwCurrentUser].RegisterLogHandle(g_hLog);
		FriendEnumArray[dwCurrentUser].SetUserIndex(dwCurrentUser);
		XOnlineNotificationSetState(dwCurrentUser, XONLINE_FRIENDSTATE_FLAG_ONLINE, SessionID, 0, NULL);
		if(!FriendEnumArray[dwCurrentUser].StartTask())
		{
			TRACE(g_hLog, "Failed starting friend enumeration task for user at index %u", dwCurrentUser);
			hr = E_FAIL;
			goto Exit;
		}
	}

	// Create a friend operation task
	FriendOpTask.RegisterLogHandle(g_hLog);
	if(!FriendOpTask.StartTask())
	{
		TRACE(g_hLog, "Failed starting friend operation task");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Create a socket entry for the local listening socket
	//
	hr = SendClientInfoToServer(pClientConnection);
	if(hr != S_OK)
		goto Exit;

	//
	// Create local UDP socket
	//
	ClientSocket = CreateClientUDPSocket();
	if(ClientSocket == INVALID_SOCKET)
	{
		TRACE(g_hLog, "Failed creating socket");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Start main client loop
	// 

	SendTimer.SetAllowedTime(100);
	SendTimer.Start();
	StatsTimer.SetAllowedTime(60000);
	StatsTimer.Start();
	FriendActionTimer.SetAllowedTime(XONSTRESS_FRIEND_ACTION_INTERVAL);
	FriendActionTimer.Start();

	while(SUCCEEDED(hr) && !g_fExitTest)
	{
		//
		// Check for client list updates and heartbeats from the host
		//
		hr = ProcessNewInfoFromHost(pClientConnection, &ClientVector);
		// Expect either S_OK or S_FALSE if there wasn't a network error
		if(FAILED(hr))
		{
			// If an error occurred in this phase, the client must exit
			goto Exit;
		}
		
		//
		// Send heartbeats if necessary
		//
		hr = SendClientHeartbeat(pClientConnection);
		if(hr != S_OK)
			goto Exit;
		
		//
		// Update friend lists if necessary
		//

		FriendEnumTimer.SetAllowedTime(XONSTRESS_MAX_ENUM_TIME);
			
		for(dwCurrentUser = 0;dwCurrentUser < XONLINE_MAX_LOGON_USERS;++dwCurrentUser)
		{
			FriendEnumTimer.Start();
			while(!FriendEnumTimer.HasTimeExpired())
			{
				if(FAILED(hr = FriendEnumArray[dwCurrentUser].TaskContinue()))
				{
					TRACE(g_hLog, "Failed pumping friend enumeration task with 0x%08x", hr);
					hr = E_FAIL;
					goto Exit;
				}
			}

			dwNewFriendCount = FriendEnumArray[dwCurrentUser].GetLatestFriendList(NULL);
			if(dwNewFriendCount != LastFriendCountArray[dwCurrentUser])
			{
				++g_dwFriendListChangeCount;

//				TRACE(g_hLog, "User at index %u now has %u friends", dwCurrentUser, dwNewFriendCount);
				LastFriendCountArray[dwCurrentUser] = dwNewFriendCount;
			}
		}


		//
		// Run client tests (start by checking for incoming connections from other clients)
		//

		srand(time(NULL));
		
		dwDieRoll = (rand() % 100);
		// 15% of the time, make a buddy list change
		if(dwDieRoll < 15)
		{
			// The FriendActionTimer prevents us from swamping the underlying friend op task
			if(FriendActionTimer.HasTimeExpired())
			{
				hr = FriendListChange(&FriendOpTask, FriendEnumArray, &ClientVector);
				if(FAILED(hr))
				{
					TRACE(g_hLog, "Failed making buddy list change");
					hr = E_FAIL;
					goto Exit;
				}

				FriendActionTimer.Start();
			}
		}
		// 15% of the time, make a presence status change
		else if(dwDieRoll < 30)
		{
			hr = PresenceStatusChange(&SessionID);
			if(FAILED(hr))
			{
				TRACE(g_hLog, "Failed making presence status change");
				hr = E_FAIL;
				goto Exit;
			}
		}
		// 15% of the time make or accept a game invitation
		else if(dwDieRoll < 45)
		{
			// The FriendActionTimer prevents us from swamping the underlying friend op task
			if(FriendActionTimer.HasTimeExpired())
			{
				hr = GameInviteChange(&FriendOpTask, FriendEnumArray, &SessionID);
				if(FAILED(hr))
				{
					TRACE(g_hLog, "Failed performing a game invite function");
					hr = E_FAIL;
					goto Exit;
				}

				FriendActionTimer.Start();
			}
		}
		// 5% of the time make a lockout list change
//		else if(dwDieRoll < 50)
//		{
//
//		}

		
		// Process friend operations
		hr = FriendOpTask.TaskContinue();
		if(FAILED(hr))
		{
			TRACE(g_hLog, "Failed pumping friend operation task");
			hr = E_FAIL;
			goto Exit;
		}

		// Look for incoming datagrams
		if(!ReceiveUDPData(ClientSocket))
		{
			TRACE(g_hLog, "Failed receiving UDP datagrams");
			hr = E_FAIL;
			goto Exit;
		}
	
		if(SendTimer.HasTimeExpired())
		{
			if(!SendUDPPacketToRandomClient(ClientSocket, &SessionID, &ClientVector))
			{
				TRACE(g_hLog, "Failed sending UDP datagram");
				hr = E_FAIL;
				goto Exit;
			}

			SendTimer.Start();
		}

		// Process lockout operations


		// Process ongoing match session stuff


		// Print data statistics
		if(StatsTimer.HasTimeExpired())
		{
			++g_dwMinuteCounter;
			TRACE(g_hLog, "Stress stats: (after %u minutes)", g_dwMinuteCounter);
			TRACE(g_hLog, "  Data:     %I64u sent, %I64u received", g_qwBytesSent, g_qwBytesReceived);
			TRACE(g_hLog, "  Friends:  %u requests, %u accepts, %u rejects, %u removes, %u list changes",
				g_dwFriendRequestCount, g_dwFriendAcceptCount, g_dwFriendRejectCount, g_dwFriendRemoveCount, g_dwFriendListChangeCount);
			TRACE(g_hLog, "  Invites:  %u invites, %u accepts, %u rejects",
				g_dwFriendGameInviteCount, g_dwFriendGameInviteAcceptCount, g_dwFriendGameInviteRejectCount, g_dwFriendGameInviteRevokeCount);
			TRACE(g_hLog, "  Presence: %u state changes issued", g_dwPresenceStateChangeCount);

			StatsTimer.Start();
		}

		PUMPLOGONHANDLE();
		Sleep(5);
	}

Exit:

	if(&SessionID)
		XNetUnregisterKey(&SessionID);

	if(ClientSocket != INVALID_SOCKET)
	{
		closesocket(ClientSocket);
	}

	pClientConnection ? delete pClientConnection : 0;

    //
    // Return the Hresult
    //

    return hr;
}
