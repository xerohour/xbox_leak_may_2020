// HostSocket.h: interface for the CHostSocket class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <winsockx.h>
#include <stdio.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xonlinep.h>

// Defines the maximum simultaneous incoming client connection requests allowed
#define MAX_CONNECTION_BACKLOG 5 

class CHostSocket  
{
public:
	void UnregisterKey();
	HRESULT AcceptIncomingClient(SOCKET *pSocket, IN_ADDR *pInAddr, XNADDR *pXnAddr, XNKID *pXnKid);
	HRESULT StartListening(WORD wPort);
	HRESULT IsConnectionPending();
	HRESULT RegisterKey(XNKID *pSessionID, XNKEY *pKeyExchangeKey);
	CHostSocket();
	virtual ~CHostSocket();

protected:
	SOCKET m_ListenSocket;
	BOOL m_fListening;
	XNKID *m_pSessionID;
	XNKEY *m_pKeyExchangeKey;
};
