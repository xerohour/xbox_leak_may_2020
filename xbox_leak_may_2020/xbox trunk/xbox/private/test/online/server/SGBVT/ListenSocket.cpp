// ListenSocket.cpp: implementation of the CListenSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "ListenSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListenSocket::CListenSocket()
{
	m_fListening = FALSE;
	m_pSessionID = NULL;
	m_pKeyExchangeKey = NULL;
	m_ListenSocket = INVALID_SOCKET;
}

CListenSocket::~CListenSocket()
{
	UnregisterKey();
	
	if(m_fListening)
		closesocket(m_ListenSocket);
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
HRESULT CListenSocket::IsConnectionPending()
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
HRESULT CListenSocket::StartListening(WORD wPort)
{
	SOCKADDR_IN ListenAddress;
    ULONG uNonBlockParam = TRUE;

	if(m_fListening)
		return TRUE;

	m_ListenSocket = socket( AF_INET, SOCK_DGRAM, 0 );

	if(m_ListenSocket == INVALID_SOCKET)
		goto Exit;

	if(SOCKET_ERROR == ioctlsocket(m_ListenSocket, FIONBIO, &uNonBlockParam))
		goto Exit;

	ListenAddress.sin_family = AF_INET;
    ListenAddress.sin_port = htons(wPort);
    ListenAddress.sin_addr.s_addr = INADDR_ANY;

	if(SOCKET_ERROR == bind(m_ListenSocket, (struct sockaddr *) &ListenAddress, sizeof(ListenAddress)))
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
 *  GetSocket
 *
 *  Description:
 *      Returns the socket
 *
 *  Arguments:
 *
 *  Returns:  
 *      The socket member variable
 *
 ****************************************************************************/
SOCKET CListenSocket::GetSocket()
{
	return m_ListenSocket;
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
void CListenSocket::UnregisterKey()
{
	if(!m_pSessionID)
		return;

	XNetUnregisterKey(m_pSessionID);

	m_pSessionID = NULL;
	m_pKeyExchangeKey = NULL;
}
