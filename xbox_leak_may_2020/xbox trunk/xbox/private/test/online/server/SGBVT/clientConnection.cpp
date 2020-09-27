// ClientConnection.cpp: implementation of the CClientConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "ClientConnection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientConnection::CClientConnection()
{
	m_fNew = FALSE;
	m_fDisconnected = FALSE;
	m_IncomingHeartbeatTimer.SetAllowedTime(SGBVT_HEARTBEAT_TIMEOUT);
	m_OutgoingHeartbeatTimer.SetAllowedTime(SGBVT_HEARTBEAT_XMIT_TIME);
	m_IncomingHeartbeatTimer.Start();
	m_IncomingHeartbeatTimer.Start();
	m_ClientSocket = INVALID_SOCKET;
	memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
	m_UserVector.reserve(XONLINE_MAX_LOGON_USERS);
}

CClientConnection::~CClientConnection()
{

}

CClientConnection::CClientConnection(const CClientConnection &ClientConnection)
{
	m_fNew = ClientConnection.m_fNew;
	m_fDisconnected = ClientConnection.m_fDisconnected;
	m_IncomingHeartbeatTimer = ClientConnection.m_IncomingHeartbeatTimer;
	m_OutgoingHeartbeatTimer = ClientConnection.m_OutgoingHeartbeatTimer;
	m_ClientSocket = ClientConnection.m_ClientSocket;
	memcpy(&m_ClientAddr, &(ClientConnection.m_ClientAddr), sizeof(m_ClientAddr));
}

CClientConnection& CClientConnection::operator = (const CClientConnection& ClientConnection)
{
	m_fNew = ClientConnection.m_fNew;
	m_fDisconnected = ClientConnection.m_fDisconnected;
	m_IncomingHeartbeatTimer = ClientConnection.m_IncomingHeartbeatTimer;
	m_OutgoingHeartbeatTimer = ClientConnection.m_OutgoingHeartbeatTimer;
	m_ClientSocket = ClientConnection.m_ClientSocket;
	memcpy(&m_ClientAddr, &(ClientConnection.m_ClientAddr), sizeof(m_ClientAddr));

	return *this;
}

bool CClientConnection::operator ==(CClientConnection &ClientConnection)
{
	if(memcmp(&m_ClientAddr, &(ClientConnection.m_ClientAddr), sizeof(m_ClientAddr)))
		return false;

	return true;
}

bool CClientConnection::operator <(CClientConnection &ClientConnection)
{
	return (m_ClientAddr.inaOnline.S_un.S_addr < ClientConnection.m_ClientAddr.inaOnline.S_un.S_addr);
}

/****************************************************************************
 *
 *  PackIntoBuffer
 *
 *  Description:
 *      Packs all of the CClientConnection information into a buffer for transmission
 *      over the wire to another client
 *
 *  Arguments:
 *      CHAR                *pBuff              Buffer to hold the user information.  If this
 *                                              parameter is NULL and a non-NULL pdwBuffSize is
 *                                              provided, then the function will fill pdwBuffSize
 *                                              with the required size necessary
 *      DWORD               *pdwBuffSize        Holds the size of the provided buffer. On output
 *                                              this parameter will contain either the required size
 *                                              (if the provided size was insufficient) or the
 *                                              ammount of space that was actually used
 *
 *  Returns:  
 *      S_OK if the client was succesfully packed into the buffer
 *      S_FALSE if a buffer was supplied but it was too small to hold the data
 *      E_FAIL if bad parameters were passed in or a system error occured
 *
 ****************************************************************************/
HRESULT CClientConnection::PackIntoBuffer(CHAR *pBuff, DWORD *pdwBuffSize)
{
	PSGBVT_CLIENT_HEADER pClientHeader = (PSGBVT_CLIENT_HEADER) pBuff;
	SGBVT_USERVECTOR::iterator UserCurrent, UserEnd;
	DWORD dwRequiredSize = 0, dwUsedSize = 0, dwUserCount = 0, dwCurrentUserSize = 0;

	if(!pdwBuffSize)
		return E_FAIL;

	dwRequiredSize = sizeof(SGBVT_CLIENT_HEADER);
	
	// Only pack user information if this is a client ADD message
	if(!m_fDisconnected)
	{
		// Loop through all users in the user vector and add to the total required size for the client packet
		for(UserCurrent = m_UserVector.begin(), UserEnd = m_UserVector.end(); UserCurrent != UserEnd; ++UserCurrent)
		{
			dwCurrentUserSize = 0;
			UserCurrent->PackIntoBuffer(NULL, &dwCurrentUserSize);
			dwRequiredSize += dwCurrentUserSize;
			++dwUserCount;
		}
	}
	
	if(*pdwBuffSize < dwRequiredSize)
	{
		*pdwBuffSize = dwRequiredSize;
		return S_FALSE;
	}

	*pdwBuffSize = dwRequiredSize;

	// Fill in the client header
	pClientHeader->dwClientSize = dwRequiredSize;
	memcpy(&(pClientHeader->ClientAddr), &m_ClientAddr, sizeof(XNADDR));
	pClientHeader->dwUserCount = dwUserCount;
	pClientHeader->bOperation = m_fDisconnected ? SGBVT_CLIENT_DELETE : SGBVT_CLIENT_ADD;

	// Only pack user information if this is a client ADD message
	if(!m_fDisconnected)
	{
		dwUsedSize += sizeof(SGBVT_CLIENT_HEADER);
		
		// Loop through all users in the user vector and add each to the client packet
		for(UserCurrent = m_UserVector.begin(), UserEnd = m_UserVector.end(); UserCurrent != UserEnd; ++UserCurrent)
		{
			dwCurrentUserSize = dwRequiredSize - dwUsedSize;
			UserCurrent->PackIntoBuffer(pBuff + dwUsedSize, &dwCurrentUserSize);
			dwUsedSize += dwCurrentUserSize;
		}
	}

	return S_OK;
}

/****************************************************************************
 *
 *  UnpackFromBuffer
 *
 *  Description:
 *      Unpacks a client information packet received over the wire into a new
 *      CClientConnection object
 *
 *  Arguments:
 *      CHAR                *pBuff              Buffer containing the received user information packet
 *      DWORD               *pdwBuffSize        On input, indicates the size of the received packet
 *                                              On output, indicates the size used to build the client object
 *
 *  Returns:  
 *      S_OK if the client object was succesfully unpacked from the buffer
 *      E_FAIL if bad parameters were passed in or a system error occured
 *
 ****************************************************************************/
HRESULT CClientConnection::UnpackFromBuffer(CHAR *pBuff, DWORD *pdwBuffSize)
{
	PSGBVT_CLIENT_HEADER pClientHeader = (PSGBVT_CLIENT_HEADER) pBuff;
	CSessionUser *pNewUser = NULL;
	HRESULT hr = S_OK;
	DWORD dwCurrentClient = 0, dwUsedSize = 0, dwUserBufferSize = 0;

	if(m_UserVector.size() != 0)
		ClearUsers();

	if((*pdwBuffSize < sizeof(SGBVT_CLIENT_HEADER)) || !pBuff)
		return E_FAIL;

	if(pClientHeader->dwClientSize > *pdwBuffSize)
		return E_FAIL;

	memcpy(&m_ClientAddr, &(pClientHeader->ClientAddr), sizeof(XNADDR));
	m_fDisconnected = (pClientHeader->bOperation == SGBVT_CLIENT_DELETE);

	// If this is a client disconnect, ignore any user information present
	if(!m_fDisconnected)
	{
		
		if(pClientHeader->dwUserCount > XONLINE_MAX_LOGON_USERS)
			return E_FAIL;
		
		dwUsedSize += sizeof(SGBVT_CLIENT_HEADER);
		
		// Unpack each user from the received packet and add to the user vector
		for(dwCurrentClient = 0;dwCurrentClient < pClientHeader->dwUserCount; ++dwCurrentClient)
		{
			pNewUser = new CSessionUser;
			
			dwUserBufferSize = *pdwBuffSize - dwUsedSize;
			if((hr = pNewUser->UnpackFromBuffer(pBuff + dwUsedSize, &dwUserBufferSize)) != S_OK)
			{
				delete pNewUser;
				goto Exit;
			}
			
			m_UserVector.push_back(*pNewUser);
			delete pNewUser;
			
			dwUsedSize += dwUserBufferSize;
		}
	}

Exit:

	return hr;
}

/****************************************************************************
 *
 *  AddUser
 *
 *  Description:
 *      Adds a user to the list of users associated with this client
 *
 *  Arguments:
 *      CSessionUser        &SessionUser        Reference to the session user to be associated
 *                                              with this client
 *
 *  Returns:  
 *      S_OK if the user was succesfully associated with the client
 *      S_FALSE if the client already has the maximum number of users
 *      E_FAIL if bad parameters were passed in or a system error occured
 *
 ****************************************************************************/
HRESULT CClientConnection::AddUser(CSessionUser &SessionUser)
{
	if(m_UserVector.size() == XONLINE_MAX_LOGON_USERS)
		return S_FALSE;

	m_UserVector.push_back(SessionUser);

	return S_OK;
}

/****************************************************************************
 *
 *  ClearUsers
 *
 *  Description:
 *      Erases all existing users from the client connection object
 *
 *  Arguments:
 *      (none)
 *
 *  Returns:  
 *      (none)
 *
 ****************************************************************************/
void CClientConnection::ClearUsers()
{
	SGBVT_USERVECTOR::iterator UserBegin, UserEnd;

	UserBegin = m_UserVector.begin();
	UserEnd = m_UserVector.end();

	m_UserVector.erase(UserBegin, UserEnd);
}

void CClientConnection::SetSocket(SOCKET Socket)
{
	m_ClientSocket = Socket;
}

SOCKET CClientConnection::GetSocket()
{
	return m_ClientSocket;
}

void CClientConnection::SetAsNew()
{
	m_fNew = TRUE;
}

void CClientConnection::SetAsOld()
{
	m_fNew = FALSE;
}

BOOL CClientConnection::IsNew()
{
	return m_fNew;
}

void CClientConnection::ResetIncomingHeartbeatTimer()
{
	m_IncomingHeartbeatTimer.Start();
}

void CClientConnection::ResetOutgoingHeartbeatTimer()
{
	m_OutgoingHeartbeatTimer.Start();
}

void CClientConnection::SetAsDisconnected()
{
	m_fDisconnected = TRUE;
	if(m_ClientSocket != INVALID_SOCKET)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;
	}
}

BOOL CClientConnection::IsDisconnected()
{
	return m_fDisconnected;
}

BOOL CClientConnection::IsIncomingHearbeatExpired()
{
	return m_IncomingHeartbeatTimer.HasTimeExpired();
}

BOOL CClientConnection::IsOutgoingHeartbeatExpired()
{
	return m_OutgoingHeartbeatTimer.HasTimeExpired();
}
