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

#define SGBVT_HEARTBEAT_TIMEOUT 60000
#define SGBVT_HEARTBEAT_XMIT_TIME 30000

#define SGBVT_MIN_CLIENT_SIZE  (sizeof(SGBVT_CLIENT_HEADER))
#define SGBVT_MAX_CLIENT_SIZE  (sizeof(SGBVT_CLIENT_HEADER) + XONLINE_MAX_LOGON_USERS * \
                                   (sizeof(SGBVT_USER_HEADER) + sizeof(XUID) + XONLINE_MAX_NAME_LENGTH + XONLINE_MAX_KINGDOM_LENGTH))

using namespace std;

class CClientConnection;

typedef vector <CClientConnection> SGBVT_CLIENTVECTOR, *PSGBVT_CLIENTVECTOR;
typedef vector <CSessionUser> SGBVT_USERVECTOR, *PSGBVT_USERVECTOR;

#pragma pack(push, 1)

// This structure frames client lists that are sent on the wire
typedef struct
{
	DWORD dwClientListSize;
	DWORD dwClientCount;
} SGBVT_CLIENTLIST_HEADER, *PSGBVT_CLIENTLIST_HEADER;

// This structure frames individual clients that are sent on the wire
typedef struct
{
	DWORD  dwClientSize;
	XNADDR ClientAddr;
	DWORD  dwUserCount;
	BYTE   bOperation;

#define SGBVT_CLIENT_ADD 0x01
#define SGBVT_CLIENT_DELETE 0x02

} SGBVT_CLIENT_HEADER, *PSGBVT_CLIENT_HEADER;

#pragma pack(pop)

class CClientConnection  
{
public:
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
	SGBVT_USERVECTOR m_UserVector;
};
