/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       host.cpp
 *  Content:    Code for the host algorithm of the integrated online stress test
 *  History:
 *   Date		By		Reason
 *   ====	==		======
 *  01/08/01	tristanj	Created to hold all code related to the client stress algorithm
 *
 ****************************************************************************/

#include "SGBVT.h"

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
	HRESULT hr = S_OK;
	SGBVT_CLIENTVECTOR ClientVector;
	sockaddr_in ClientAddress;
	XONLINE_ATTRIBUTE AttributesArray[1];
	COnlineTimer HostWaitTimer;
	COnlineTimer TransactionWaitTimer;	
	CMatchSessionCreateTask SessionCreateTask;
	CListenSocket *pListenSocket = NULL;
	DWORD dwPublicCurrent = 0, dwNewPublicCurrent = 0, dwAttributesCount = (sizeof(AttributesArray) / sizeof(XONLINE_ATTRIBUTE));
	XNKID SessionID;
	XNKEY KeyExchangeKey;
	char *pBuff = NULL;
	int iRes = 0, iLastError = 0, iBuffSize = 0;

	pBuff = new char[LARGEST_PACKET];	
	if(!pBuff)
		return E_OUTOFMEMORY;

	ZeroMemory(pBuff, LARGEST_PACKET);

	// These attributes define the match making session
	AttributesArray[0].dwAttributeID = SGBVT_SESSION_PARAM;
	AttributesArray[0].info.integer.qwValue = SGBVT_SESSION_VALUE;
	AttributesArray[0].fChanged = TRUE;
	
	SessionCreateTask.RegisterLogHandle(g_hLog);
	SessionCreateTask.SetAttributes(AttributesArray, dwAttributesCount);

	memset(&ClientAddress, 0, sizeof(sockaddr_in));
	memset(&SessionID, 0, sizeof(XNKID));
	memset(&KeyExchangeKey, 0, sizeof(XNKEY));

	fd_set readfds;
	fd_set exceptfds;	
	
	TIMEVAL sTimeout = {0, 0}; 

	//
	// Create and advertise a new session on the match server
	//
	hr = AdvertiseStressSession(&SessionCreateTask, dwPublicCurrent, DEFAULT_SLOTS_AVAILABLE, &SessionID, &KeyExchangeKey);	
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

	HostWaitTimer.SetAllowedTime(SGBVT_HOST_WAIT_TIME);
	TransactionWaitTimer.SetAllowedTime(SGBVT_MAX_CONNECT_TIME);

	while(SUCCEEDED(hr) && !g_fExitTest)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&exceptfds);
		FD_SET(pListenSocket->GetSocket(), &readfds);
		FD_SET(pListenSocket->GetSocket(), &exceptfds);

		iRes = select(0, &readfds, NULL, &exceptfds, &sTimeout);
				
		if(iRes == 0)
		{
			// Timeout
		}
		else if (iRes == SOCKET_ERROR )
		{
			iLastError = WSAGetLastError();
		}
		else
		{
			// We have something to receive
			hr = ReadPacket(pListenSocket->GetSocket(), pBuff, LARGEST_PACKET, &TransactionWaitTimer, &ClientAddress, &iBuffSize);
			if(FAILED(hr))
			{
				LOGTASKINFO(g_hLog, "A socket error occured on a client, marking as disconnected");
				hr = S_OK;
			}
			else
			{
				// Send it back
				hr = SendPacket(pListenSocket->GetSocket(), pBuff, iBuffSize, &TransactionWaitTimer, &ClientAddress);
				if(FAILED(hr))
				{
					LOGTASKINFO(g_hLog, "A socket error occured on a client, marking as disconnected");
					hr = S_OK;
				}
			}
		}
		
		// Wait for SGBVT_HOST_WAIT_TIME miliseconds to throttle the speed at which connections and disconnections occur
		HostWaitTimer.Start();
		while(!HostWaitTimer.HasTimeExpired())
		{
			PUMPLOGONHANDLE();
			Sleep(0);
		}
	}

Exit:

	//
	// Delete stress session
	//
	hr = DeleteStressSession(&SessionID);
	if(hr != S_OK)
	{
		LOGTASKINFO(g_hLog, "Failed to delete session from match server");
	}

	//
	// Close the host socket and client connections
	//
	pListenSocket ? delete pListenSocket : 0;

	pBuff ? delete [] pBuff : 0;
    //
    // Return the Hresult
    //

    return hr;
}
