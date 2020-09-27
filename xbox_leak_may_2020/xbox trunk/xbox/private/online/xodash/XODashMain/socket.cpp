//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#include "std.h"
#include "Socket.h"
#include "AutomationMsg.h"
#include "globals.h"
#include "NetConfig.h"

CSocket::CSocket(): m_ListenSocket(INVALID_SOCKET), 
					m_AcceptedSocket(INVALID_SOCKET), 
					m_bClientConnected(false)
				
					
{} 



CSocket::~CSocket()
{
	Close();
}


//-------------------------------------------------------------
// Function name	: CSocket::InitNetworkStack
// Description	    : Global network initialization
// Return type		: HRESULT 
//-------------------------------------------------------------
HRESULT CSocket::InitNetworkStack()
{
		XNetStartupParams xnsp;
		ZeroMemory( &xnsp, sizeof(xnsp) );
		xnsp.cfgSizeOfStruct = sizeof(xnsp);
    
        xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;

        INT iResult = XNetStartup( &xnsp );
        if( iResult != NO_ERROR )
        {
            return E_FAIL;
        }

       // Standard WinSock startup
        WSADATA WsaData;
        iResult = WSAStartup( MAKEWORD(2,2), &WsaData );
        if( iResult != NO_ERROR )
        {
            return E_FAIL;
        }
		return S_OK;
}

//-------------------------------------------------------------
// Function name	: CSocket::InitServer
// Description	    : Initialize listen socket
// Return type		: HRESULT 
// Argument         : bool bTCP/*=true*/
//-------------------------------------------------------------
HRESULT CSocket::InitServer(bool bTCP/*=true*/)
{
	if (!g_ResetAutomation)
	{
		return S_FALSE;
	}
	HRESULT hr = S_OK;
	// wait till IP obtained and only then reinitialize the socket
	eNetworkState eCurState = g_NetConfig.GetNetworkStatus();

	switch(eCurState)
	{
	case eCablePending:
	case eCableFail: 
	case eCablePass: 
		return S_FALSE;
	case eIPPass:
	case eDNSFail: 
	case eServiceFail: 
	case eDone: 
		break;
	}

	unsigned short iType, iProtocol = 0; 
	if(bTCP) 
	{
		iType = SOCK_STREAM;
		iProtocol = IPPROTO_TCP;
	}
	else
	{
		iType = SOCK_DGRAM;
		iProtocol = IPPROTO_UDP;
	}

	do
	{
		// create socket
		m_ListenSocket = socket( AF_INET, iType, iProtocol);
		if(INVALID_SOCKET == m_ListenSocket)
		{
			DbgPrint( "CSocket::InitServer - Fail to open a socket hr = 0x%x\n", WSAGetLastError() );
			break;
		}

		// bind socket
		hr = BindSocket();
		if (FAILED(hr))
		{
			break;
		}

		// listen
		int iResult = listen(m_ListenSocket, SOMAXCONN);
		if(SOCKET_ERROR == iResult)
		{
			DbgPrint("CSocket::InitServer - Fail to listen hr = 0x%x\n", WSAGetLastError() );
			break;
		}
	}while (0);

	if(FAILED(hr))
	{
		Close();
	}
	else
	{
		DbgPrint("CSocket::InitServer - TCP server waits for connections...\n");
	}

	g_ResetAutomation = false;
	return hr;
}


//-------------------------------------------------------------
// Function name	: CSocket::BindSocket
// Description	    : part of initialization process
// Return type		: HRESULT 
// Argument         : bool bNonBlock /*=true*/
//-------------------------------------------------------------
HRESULT CSocket::BindSocket(bool bNonBlock /*=true*/)
{
	HRESULT hr = S_OK;
	ASSERT(m_ListenSocket != INVALID_SOCKET);

    SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(ciTCPServerPort);

    int iResult = bind( m_ListenSocket, (const sockaddr *)&sa, 
                        sizeof( SOCKADDR_IN ) );

	if(SOCKET_ERROR == iResult)
	{
		DbgPrint("CSocket::BindSocket - Fail to bind the socket, hr=0x%x\n", WSAGetLastError());
		return E_FAIL;
	}

	if(bNonBlock)
	{
		DWORD dwNonBlocking = 1;
		int iResult = ioctlsocket( m_ListenSocket, FIONBIO, &dwNonBlocking);
		if(SOCKET_ERROR == iResult )
		{
			DbgPrint("CSocket::BindSocket - Fail in ioctlsocket, hr=0x%x\n",WSAGetLastError());
			return E_FAIL;
		}
	}

	return hr;
}



bool CSocket::IsAutomationActive()
{
	if(g_ResetAutomation)
	{
		InitServer();
	}
	else if(!m_bClientConnected)
	{
		int iSize = sizeof( SOCKADDR_IN );
		SOCKADDR sa;
		m_AcceptedSocket = accept( m_ListenSocket, &sa, &iSize );
		DWORD dw = WSAGetLastError();
		if ( dw != 10035)
			DbgPrint("Accept result = [%d]",WSAGetLastError());

		if(m_AcceptedSocket != INVALID_SOCKET)
		{
			DbgPrint("CSocket::ReceiveMessage - Client is connected\n");
			m_bClientConnected = true;
			m_AutomationTimer.StartZero();
		}
	}
	return m_bClientConnected;
}

//-------------------------------------------------------------
// Function name	: CSocket::ReceiveMessage
// Description	    : Get new message, initiate/discover client disconnect
// Return type		: HRESULT 
// Argument         : CAutomationMsg* pMsg
//-------------------------------------------------------------
bool CSocket::ReceiveMessage(CAutomationMsg* pMsg)
{
	HRESULT hr = S_OK;
	ASSERT(pMsg);
	if (!pMsg)
	{
		return false;
	}

	if(!IsAutomationActive())
	{
		return false;
	}
	
	pMsg->m_byMessageId = eNullMsg;
	ASSERT( m_ListenSocket != INVALID_SOCKET );
	
	// client is connected, check for messages 
	int nSize = pMsg->GetMaxSize();
	int iResult = recv(m_AcceptedSocket, (char*) pMsg, nSize, 0);

	// exactly one message is expected to be in the Q
	if(iResult > 0)
	{
		ASSERT(iResult == nSize);
	}

	// do we need to disconnect
	if (SOCKET_ERROR == iResult || !iResult )
	{
		int iReason = WSAGetLastError();
		// message supposed to fit into provided buffer
		ASSERT(iReason!= WSAEMSGSIZE);
		bool bTimeout = (m_AutomationTimer.GetElapsedSeconds() >= cfAutomationTimeout);
		if(!iResult || iResult < 0 && iReason == WSAECONNRESET || bTimeout)
		{
			DbgPrint("CSocket::ReceiveMessage - Client is disconnected [%d]\n", iReason);
			// close 
			Close();
			m_AutomationTimer.Stop();
		}
	}
	else
	{
		// new msg is received
		pMsg->Dump();
		m_AutomationTimer.StartZero();
	}
	
	// filter irrelevant msgs
	return !pMsg->IsFiltered();
}


//-------------------------------------------------------------
// Function name	: CSocket::Close
// Description	    : cleanup both listen and accepted sockets
// Return type		: void 
// Argument         : bool bCloseListen /*= true*/
//-------------------------------------------------------------
void CSocket::Close()
{
    int iResult = 0;
	// close accepted socket if open
	if( m_AcceptedSocket != INVALID_SOCKET )
    {
		iResult = shutdown(m_AcceptedSocket, SD_SEND);
        iResult = closesocket( m_AcceptedSocket );
		if (SOCKET_ERROR == iResult)
		{
			DbgPrint("CSocket::Close - Fail to close the Accepted socket, hr=0x%x\n",WSAGetLastError());
		}
        m_AcceptedSocket = INVALID_SOCKET;
    }

	// close listen socket if open
    if( m_ListenSocket != INVALID_SOCKET )
    {
        iResult = closesocket( m_ListenSocket );
		if (SOCKET_ERROR == iResult)
		{
			DbgPrint("CSocket::Close - Fail to close the Listen socket, hr=0x%x\n",WSAGetLastError());
		}
        m_ListenSocket = INVALID_SOCKET;
    }

	m_bClientConnected = false;
	g_ResetAutomation = true;
}