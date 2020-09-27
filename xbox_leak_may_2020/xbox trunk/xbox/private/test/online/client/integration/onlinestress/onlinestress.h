/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       onlinestress.h
 *  Content:    onlinestress utility interface
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  12/ 6/01    tristanj Created to test Xbox onlinestress
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xtestlib.h>
#include <xnetref.h>
#include <time.h>
#include "OnlineAccounts.h"
#include "logontask.h"
#include "matchsessioncreatetask.h"
#include "matchsearchtask.h"
#include "friendenumtask.h"
#include "friendoptask.h"
#include "onlinetimer.h"
#include "clientconnection.h"
#include "sessionuser.h"
#include "listensocket.h"

#if defined(NOLOGGING)
#undef LOGTASKINFO
#undef LOGTASKINFOHR
void WINAPI TRACE(HANDLE hLog, LPCSTR szFormat, ...);
#define LOGTASKINFO TRACE
#define LOGTASKINFOHR TRACE
#else
#define TRACE
#endif

#define XONSTRESS_HOST_PORT 1000
#define MAX_NEW_CLIENT_INFO_TIME 10000
#define MAX_SESSION_CREATE_TIME 10000
#define DEFAULT_SLOTS_AVAILABLE 5
#define MAX_LOGON_TIME 30000
#define XONSTRESS_HOST_WAIT_TIME 5000
#define XONSTRESS_MAX_CONNECT_TIME 10000
#define XONSTRESS_MAX_CLIENT_READ_TIME 1000
#define XONLINESTRESS_MAX_MATCH_SEARCH_TIME 30000
#define XONLINESTRESS_MAX_SESSION_DELETE_TIME 10000
#define XONSTRESS_CLIENT_SEND_TIME 500
#define XONSTRESS_HOST_SEND_TIME 500
#define XONSTRESS_CLIENT_RECV_TIME 500
#define XONSTRESS_HOST_RECV_TIME 500
#define XONSTRESS_MAX_ENUM_TIME 5
#define XONSTRESS_FRIEND_ACTION_INTERVAL 3000  // Bump this down later
#define XONSTRESS_SESSION_REFRESH_TIME (15 /*minutes*/ * 60 * 1000)

#define XONLINESTRESS_SEARCH_PROC 128
#define XONLINESTRESS_SESSION_PARAM (X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_INTEGER | 0xAAAA)
#define XONLINESTRESS_SESSION_VALUE 12345
#define XONLINESTRESS_MAX_RESULTS 20
#define XONLINESTRESS_MAX_RESULTS_LEN (XONLINESTRESS_MAX_RESULTS * sizeof(XMATCH_SEARCHRESULT))

#define PUMPLOGONHANDLE() if(FAILED(hr = WaitAndPump(0, g_pLogonTask))) \
{ \
	LOGTASKINFOHR(g_hLog, "Online task handle failed", hr); \
	goto Exit; \
}

HRESULT SendPacket(SOCKET Sock, CHAR *pBuff, INT nSizeToSend, COnlineTimer *pTimer);
HRESULT ReadPacket(SOCKET Socket, CHAR *pBuff, INT nSizeToRead, COnlineTimer *pTimer);
HRESULT ReadNewClientInfo(SOCKET Socket, CClientConnection *pNewClient);
HRESULT CreateListenSocket(CListenSocket **ppListenSocket, XNKID *pSessionID, XNKEY *pKeyExchangeKey);

HRESULT client_Main(CMatchSearchTask *pMatchSearchTask);
HRESULT host_Main(void);

VOID WINAPI onlinestress_StartTest( HANDLE LogHandle );
VOID WINAPI onlinestress_EndTest( VOID );