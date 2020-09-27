/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       client.cpp
 *  Content:    Code for the client algorithm of the integrated online stress test
 *  History:
 *   Date		By		Reason
 *   ====	==		======
 *  2/18/02	johnblac	Created to hold all code related to the client stress algorithm
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
 *      XNKEY                 *pKeyExchangeKey    XNKEY to register with the network stack
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if the socket timed out waiting to connect
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT CreateClientConnectionToHost(SOCKET* pHostSocket, XNADDR *pHostAddr, XNKID *pSessionID)
{
	COnlineTimer ConnectionTimer;
	HRESULT hr = S_OK;
	fd_set writefds, exceptfds;
	TIMEVAL sTimeout = {0, 0}; 
	ULONG uNonBlockParam = TRUE;
	INT nActiveSockets = 0;

	//
	// Create the socket to communicate with the host
	//
	*pHostSocket = socket( AF_INET, SOCK_DGRAM, 0 );
	if(*pHostSocket == INVALID_SOCKET)
	{
		LOGTASKINFO(g_hLog, "Error creating socket");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Set non-blocking IO
	//
	if(SOCKET_ERROR == ioctlsocket(*pHostSocket, FIONBIO, &uNonBlockParam))
	{
		LOGTASKINFO(g_hLog, "Error setting client socket to use non-blocking IO");
		hr = E_FAIL;
		goto Exit;
	}

	hr = S_OK;

Exit:

	if(hr == E_FAIL)
	{
		closesocket(*pHostSocket);
		*pHostSocket = INVALID_SOCKET;
	}

	return hr;
}

/****************************************************************************
 *
 *  VerifySentData
 *
 *  Description:
 *      Waits for a receive packet on the socket passed into it, then compares the buffer
 *	  received with the buffer that was sent.
 *
 *  Arguments:
 *      SOCKET sHostSocket - socket to receive buffer on
 *	  char* pBuffSend	     - buffer to compare
 *
 *  Returns:  
 *      
 *
 ****************************************************************************/
 
HRESULT VerifySentData(SOCKET sHostSocket, char* pBuffSend)
{
	HRESULT hr = S_OK;
	COnlineTimer	ClientWaitTimer;
	fd_set readfds;
	fd_set exceptfds;	
	sockaddr_in	SrcAddress;
	int iBuffSize = 0, iLastError = 0;
	ClientWaitTimer.SetAllowedTime(SGBVT_HOST_WAIT_TIME);
	TIMEVAL sTimeout = {0, 0}; 
	char rgbBuffRecv[LARGEST_PACKET];

	if(!pBuffSend)
		return E_INVALIDARG;

	ZeroMemory(rgbBuffRecv, LARGEST_PACKET);
	
	//
	// The host should send it back
	//
	while(true)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&exceptfds);

		FD_SET(sHostSocket, &readfds);
		FD_SET(sHostSocket, &exceptfds);

		int iRes = select(0, &readfds, NULL, &exceptfds, &sTimeout);
				
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
			hr = ReadPacket(sHostSocket, rgbBuffRecv, LARGEST_PACKET, NULL, &SrcAddress, &iBuffSize);
			if(FAILED(hr))
			{
				LOGTASKINFO(g_hLog, "A socket error occured on a client, marking as disconnected");
				hr = S_OK;
			}
			else
			{
				// Compare the buffers
				if(memcmp(pBuffSend, rgbBuffRecv, iBuffSize))
				{
					hr = E_BUFFER_MISMATCH;
				}
			}
			break;
		}
		
		// Wait for SGBVT_HOST_WAIT_TIME miliseconds to throttle the speed at which connections and disconnections occur
		ClientWaitTimer.Start();
		while(!ClientWaitTimer.HasTimeExpired())
		{
			PUMPLOGONHANDLE();
			Sleep(0);
		}
	}
Exit:
	return hr;
}


/****************************************************************************
 *
 *  FillData
 *
 *  Description:
 *      Takes a BYTE pointer to a buffer with a given size and maximum length of the buffer.  
 *      The buffer is cleared and filled with 'bogus' data.
 *
 *  Arguments:
 *	BYTE* pBuff - pointer to the buffer to be filled
 *	WORD wSize - Size of data to be created
 *	WORD wBufferLength - Total size of the buffer
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
//-----------------------------------------------------------------------------
//
// FillData
//
// Takes a BYTE pointer to a buffer with a given size and maximum length of the buffer.  The 
// buffer is filled with 'bogus' data.
//
HRESULT FillData(BYTE* pBuff, WORD wSize, WORD wBufferLength)
{
	// Check for a valid buffer pointer and that we're filling the buffer
	// with no more than its maximum size
	if(!pBuff || wSize > wBufferLength)
	{
		return E_FAIL;
	}

	ZeroMemory(pBuff, wBufferLength);
	WORD wCount = 0;
	while(wCount < wSize)
	{
		switch(wCount % 3)
		{
		case 0:
			*pBuff = 0xC0;
			break;
		case 1:
			*pBuff = 0xFF;
			break;
		case 2:
			*pBuff = 0xEE;
			break;
		default:
			return E_UNEXPECTED;
		}
		wCount++;
		pBuff++;
	}

	return S_OK;
}


/****************************************************************************
 *
 *  client_Main
 *
 *  Description:
 *      Master algorithm for the online stress client
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT client_Main(XNADDR *pHostXnAddr, XNKID *pSessionID, XNKEY *pKeyExchangeKey )
{
	HRESULT 	hr = S_OK;
	sockaddr_in	HostAddress;
	IN_ADDR 	ClientInAddr;
	XNADDR		ClientXnAddr;
	SOCKET		sHostSocket;
	BYTE *pBuffSend = NULL;
	BYTE *pBuffRecv = NULL;
	
	// Allocate some buffer space for sending and receiving data
	pBuffSend = new BYTE[LARGEST_PACKET];	
	if(!pBuffSend)
		return E_OUTOFMEMORY;
	
	HostAddress.sin_family = AF_INET;
	HostAddress.sin_port = htons(SGBVT_HOST_PORT);

	//
	// Register the host's key information
	//
	if(XNetRegisterKey(pSessionID, pKeyExchangeKey))
	{
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Get a dummy IP address binding for the host's address
	//
	if(XNetXnAddrToInAddr(pHostXnAddr, pSessionID, &(HostAddress.sin_addr)))
	{
		LOGTASKINFO(g_hLog, "Error getting a private INADDR from host's XNADDR");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Create a socket to connect to the session host
	//
	hr = CreateClientConnectionToHost(&sHostSocket, pHostXnAddr, pSessionID);
	if(hr != S_OK)
		goto Exit;

	// ************* 0 sized Packet Test
	// Clean the buffers and put something in the send buffer
	if(FAILED(FillData(pBuffSend, 0, LARGEST_PACKET)))
		return E_COULDNT_FILL_SEND_BUFFER;
	
	//
	// Send some data to the host
	//
	hr = SendPacket(sHostSocket, (char*)pBuffSend, 4, NULL, &HostAddress);
	if(FAILED(hr))
	{
		LOGTASKINFO(g_hLog, "A socket error occured on a client, marking as disconnected");
		hr = S_OK;
	}

	hr = VerifySentData(sHostSocket, (char*)pBuffSend);
	if(FAILED(hr))
		goto Exit;

	// ************* N sized packet Test	
	// Clean the buffers and put something in the send buffer
	if(FAILED(FillData(pBuffSend, 4, LARGEST_PACKET)))
		return E_COULDNT_FILL_SEND_BUFFER;
	
	//
	// Send some data to the host
	//
	hr = SendPacket(sHostSocket, (char*)pBuffSend, 4, NULL, &HostAddress);
	if(FAILED(hr))
	{
		LOGTASKINFO(g_hLog, "A socket error occured on a client, marking as disconnected");
		hr = S_OK;
	}

	hr = VerifySentData(sHostSocket, (char*)pBuffSend);
	if(FAILED(hr))
		goto Exit;

	// ************* Largest Packet Test
	// Clean the buffers and put something in the send buffer
	if(FAILED(FillData(pBuffSend, LARGEST_PACKET, LARGEST_PACKET)))
		return E_COULDNT_FILL_SEND_BUFFER;
	
	//
	// Send some data to the host
	//
	hr = SendPacket(sHostSocket, (char*)pBuffSend, 4, NULL, &HostAddress);
	if(FAILED(hr))
	{
		LOGTASKINFO(g_hLog, "A socket error occured on a client, marking as disconnected");
		hr = S_OK;
	}

	hr = VerifySentData(sHostSocket, (char*)pBuffSend);
	if(FAILED(hr))
		goto Exit;
	
Exit:
	//
	// Close the listent socket and client connections
	//

	closesocket(sHostSocket);
	
	pBuffSend ? delete [] pBuffSend : 0;
	pBuffRecv ? delete [] pBuffRecv : 0;	
	
	//
	// Return the Hresult
	//
	return hr;
}
