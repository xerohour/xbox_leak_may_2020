// HostSocket.cpp: implementation of the CHostSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "HostSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHostSocket::CHostSocket()
{
	m_fListening = FALSE;
	m_pSessionID = NULL;
	m_pKeyExchangeKey = NULL;
	m_ListenSocket = INVALID_SOCKET;
}

CHostSocket::~CHostSocket()
{
	UnregisterKey();
	
	if(m_fListening)
		closesocket(m_ListenSocket);
}

/****************************************************************************
 *
 *  RegisterKey
 *
 *  Description:
 *      Registers security information with the network stack so that the host
 *      can accept incoming connections from clients that saw the session on
 *      the match server
 *
 *  Arguments:
 *      (none)
 *
 *  Returns:  
 *      S_OK if the key was registered successfully
 *      E_FAIL if an error occured registering the key
 *
 ****************************************************************************/
HRESULT CHostSocket::RegisterKey(XNKID *pSessionID, XNKEY *pKeyExchangeKey)
{
	INT nRegisterRet = 0;

	if(m_pSessionID || m_pKeyExchangeKey)
		return E_FAIL;

	m_pSessionID = pSessionID;
	m_pKeyExchangeKey = pKeyExchangeKey;

	
	nRegisterRet = XNetRegisterKey(pSessionID, pKeyExchangeKey);

	if((nRegisterRet != 0) && (nRegisterRet != WSAEALREADY))
		return E_FAIL;

	return S_OK;
}

/****************************************************************************
 *
 *  IsConnectionPending
 *
 *  Description:
 *      Return whether or not there are any pending connections available on
 *      the host socket
 *
 *  Arguments:
 *      (none)
 *
 *  Returns:  
 *      S_OK if there are incoming connections pending
 *      S_FALSE if there are no incoming connections pending
 *      E_FAIL if the API was called incorrectly or a winsock or system error occured
 *
 ****************************************************************************/
HRESULT CHostSocket::IsConnectionPending()
{
	HRESULT hr = S_FALSE;
	INT nActiveSockets = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_ListenSocket, &readfds);

    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    FD_SET(m_ListenSocket, &exceptfds);
    
    TIMEVAL sTimeout = {0, 0}; 

	if(!m_fListening)
		return E_FAIL;

    nActiveSockets = select(0, &readfds, NULL, &exceptfds, &sTimeout);

    if(nActiveSockets <= 0)
    {
        hr = S_FALSE;
        goto Exit;
    }
                
    if(FD_ISSET(m_ListenSocket, &exceptfds))
    {
        hr = E_FAIL;
        goto Exit;
    }

    if(!FD_ISSET(m_ListenSocket, &readfds))
    {
        hr = E_FAIL;
        goto Exit;
    }

	hr = S_OK;

Exit:

	return hr;
}

/****************************************************************************
 *
 *  StartListening
 *
 *  Description:
 *      Create a host socket, set it to non-blocking, bind it to a designated port
 *      and begin listening for incoming connections
 *
 *  Arguments:
 *      WORD                wPort               Port to listen for incoming connection
 *                                              requests on
 *
 *  Returns:  
 *      S_OK if the socket was created and is listening
 *      E_FAIL if the API was called incorrectly or a winsock or system error occured
 *
 ****************************************************************************/
HRESULT CHostSocket::StartListening(WORD wPort)
{
	SOCKADDR_IN ListenAddress;
    ULONG uNonBlockParam = TRUE;

	if(m_fListening)
		return TRUE;

	m_ListenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );

	if(m_ListenSocket == INVALID_SOCKET)
		goto Exit;

	if(SOCKET_ERROR == ioctlsocket(m_ListenSocket, FIONBIO, &uNonBlockParam))
		goto Exit;

	ListenAddress.sin_family = AF_INET;
    ListenAddress.sin_port = htons(wPort);
    ListenAddress.sin_addr.s_addr = INADDR_ANY;

	if(SOCKET_ERROR == bind(m_ListenSocket, (struct sockaddr *) &ListenAddress, sizeof(ListenAddress)))
		goto Exit;

	if(SOCKET_ERROR == listen(m_ListenSocket, MAX_CONNECTION_BACKLOG))
		goto Exit;

	m_fListening = TRUE;

Exit:

	if(!m_fListening)
	{
		if(m_ListenSocket != INVALID_SOCKET)
		{
			closesocket(m_ListenSocket);
			m_ListenSocket = INVALID_SOCKET;
		}
	}

	return m_fListening ? S_OK : E_FAIL;
}

/****************************************************************************
 *
 *  AcceptIncomingClient
 *
 *  Description:
 *      Pulls one client off the host socket backlog
 *
 *  Arguments:
 *      SOCKET              *pSocket            Returns the incoming client socket
 *      IN_ADDR             *pInAddr            Returns the locally bound IP of the incoming socket
 *      XNADDR              *pXnAddr            Returns the actual Xbox Online address of the incoming socket
 *
 *  Returns:  
 *      S_OK if the socket successfully accepted
 *      S_FALSE if the API timed out waiting for an incoming connection
 *      E_FAIL if the API was called incorrectly or a winsock or system error occured
 *
 ****************************************************************************/
HRESULT CHostSocket::AcceptIncomingClient(SOCKET *pSocket, IN_ADDR *pInAddr, XNADDR *pXnAddr, XNKID *pXnKid)
{
	SOCKADDR_IN IncomingSockAddr;
	INT nSockAddrSize = sizeof(SOCKADDR_IN);

	if(!pSocket || !m_fListening)
		return E_FAIL;

	if((*pSocket = accept(m_ListenSocket, (struct sockaddr *) &IncomingSockAddr, &nSockAddrSize)) == INVALID_SOCKET)
	{
		if(WSAGetLastError() == WSAEWOULDBLOCK)
			return S_FALSE;

		return E_FAIL;
	}

	if(pInAddr)
		(*pInAddr).S_un.S_addr = IncomingSockAddr.sin_addr.S_un.S_addr;

	if(pXnAddr && pXnKid)
	{
		if(XNetInAddrToXnAddr(IncomingSockAddr.sin_addr, pXnAddr, pXnKid))
		{
			closesocket(*pSocket);
			return E_FAIL;
		}
	}

	return S_OK;
}

/****************************************************************************
 *
 *  UnregisterKey
 *
 *  Description:
 *      Unregisters the security information used for the host socket
 *
 *  Arguments:
 *      (none)
 *
 *  Returns:  
 *      (none)
 *
 ****************************************************************************/
void CHostSocket::UnregisterKey()
{
	if(!m_pSessionID)
		return;

	XNetUnregisterKey(m_pSessionID);

	m_pSessionID = NULL;
	m_pKeyExchangeKey = NULL;
}
