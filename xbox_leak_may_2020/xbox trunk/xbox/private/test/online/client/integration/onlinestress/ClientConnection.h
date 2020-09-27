// ClientConnection.h: interface for the CClientConnection class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <winsockx.h>
#include <stdio.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xonlinep.h>
#include <vector>
#include "onlinetimer.h"
#include "sessionuser.h"

#define XONSTRESS_HEARTBEAT_TIMEOUT 60000
#define XONSTRESS_HEARTBEAT_XMIT_TIME 30000

#define XONSTRESS_MIN_CLIENT_SIZE  (sizeof(XONSTRESS_CLIENT_HEADER))
#define XONSTRESS_MAX_CLIENT_SIZE  (sizeof(XONSTRESS_CLIENT_HEADER) + XONLINE_MAX_LOGON_USERS * \
                                   (sizeof(XONSTRESS_USER_HEADER) + sizeof(XUID) + XONLINE_MAX_NAME_LENGTH + XONLINE_MAX_KINGDOM_LENGTH))

using namespace std;

class CClientConnection;

typedef vector <CClientConnection> XONSTRESS_CLIENTVECTOR, *PXONSTRESS_CLIENTVECTOR;
typedef vector <CSessionUser> XONSTRESS_USERVECTOR, *PXONSTRESS_USERVECTOR;

#pragma pack(push, 1)

// This structure frames client lists that are sent on the wire
typedef struct
{
	DWORD dwClientListSize;
	DWORD dwClientCount;
} XONSTRESS_CLIENTLIST_HEADER, *PXONSTRESS_CLIENTLIST_HEADER;

// This structure frames individual clients that are sent on the wire
typedef struct
{
	DWORD  dwClientSize;
	XNADDR ClientAddr;
	DWORD  dwUserCount;
	BYTE   bOperation;

#define XONSTRESS_CLIENT_ADD 0x01
#define XONSTRESS_CLIENT_DELETE 0x02

} XONSTRESS_CLIENT_HEADER, *PXONSTRESS_CLIENT_HEADER;

#pragma pack(pop)

class CClientConnection  
{
public:
	BOOL GetRandomUserInfo(XUID *pUserXUID, CHAR *szUserName);
	BOOL IsOutgoingHeartbeatExpired();
	BOOL IsIncomingHearbeatExpired();
	BOOL IsDisconnected();
	void SetAsDisconnected();
	void ResetOutgoingHeartbeatTimer();
	void ResetIncomingHeartbeatTimer();
	BOOL IsNew();
	void SetAsOld();
	void SetAsNew();
	SOCKET GetSocket();
	void SetSocket(SOCKET Socket);
	void ClearUsers();
	HRESULT AddUser(CSessionUser &);
	HRESULT UnpackFromBuffer(CHAR *pBuff, DWORD *pdwBuffSize);
	HRESULT PackIntoBuffer(CHAR *pBuff, DWORD *pdwBuffSize);
	bool operator <(CClientConnection &);
	bool operator ==(CClientConnection &);
	CClientConnection& operator = (const CClientConnection&);
	CClientConnection(const CClientConnection &);
	CClientConnection();
	virtual ~CClientConnection();
	
	XNADDR m_ClientAddr;

protected:
	COnlineTimer m_OutgoingHeartbeatTimer;
	COnlineTimer m_IncomingHeartbeatTimer;
	SOCKET m_ClientSocket;
	BOOL m_fDisconnected;
	BOOL m_fNew;
	XONSTRESS_USERVECTOR m_UserVector;
};
