/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       SGBVT.h
 *  Content:    SGBVT utility interface
 *  History:
 *   Date       	By       	Reason
 *   ====      ==		======
 *  2/18/02   johnblac	Ripped from TristanJs onlinestress
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xtestlib.h>
#include <xnetref.h>
#include "OnlineAccounts.h"
#include "FriendEnumTask.h"
#include "OfferingEnumerateTask.h"
#include "ContentInstallTask.h"
#include "ContentVerifyTask.h"
#include "ContentRemoveTask.h"
#include "logontask.h"
#include "matchsessioncreatetask.h"
#include "onlinetimer.h"
#include "clientconnection.h"
#include "sessionuser.h"
#include "listensocket.h"

#define SGBVT_HOST_PORT 1001
#define MAX_NEW_CLIENT_INFO_TIME 5000
#define MAX_SESSION_CREATE_TIME 5000
#define DEFAULT_SLOTS_AVAILABLE 10
#define MAX_LOGON_TIME 30000
#define SGBVT_HOST_WAIT_TIME 5000
#define SGBVT_MAX_CONNECT_TIME 10000
#define SGBVT_MAX_CLIENT_READ_TIME 1000
#define SGBVT_MAX_MATCH_SEARCH_TIME 30000
#define SGBVT_MAX_SESSION_DELETE_TIME 10000
#define LARGEST_PACKET 1430

#define SGBVT_SEARCH_PROC 129
#define SGBVT_SESSION_PARAM (X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_INTEGER | 0xAAAA)
#define SGBVT_SESSION_VALUE 67890
#define SGBVT_MAX_RESULTS 20
#define SGBVT_MAX_RESULTS_LEN (SGBVT_MAX_RESULTS * sizeof(XMATCH_SEARCHRESULT))

#define XBOX_TIMEOUT				125000
#define MAX_GAMER_ENUM_TIME      	10000
#define MAX_SESSION_FIND_TIME		10000

const HRESULT E_FAILED_KEY_REGISTER 	= 0x80007100;
const HRESULT E_LOGON_TIMED_OUT		= 0x80007101;
const HRESULT E_OVERFLOW				= 0x80007102;
const HRESULT E_BUFFER_MISMATCH		= 0x80007103;
const HRESULT E_COULDNT_FILL_SEND_BUFFER = 0x80007104;

#define PUMPLOGONHANDLE() if(FAILED(hr = WaitAndPump(0, g_pLogonTask))) \
{ \
	LOGTASKINFOHR(g_hLog, "Online task handle failed", hr); \
	goto Exit; \
}

HRESULT SendPacket(SOCKET Socket, CHAR *pBuff, INT nSizeToSend, COnlineTimer *pTimer, sockaddr_in* pDestAddr);
HRESULT ReadPacket(SOCKET Socket, CHAR *pBuff, INT nSizeOfBuffer, COnlineTimer *pTimer, sockaddr_in* pSrcAddr, int* piBuffSize);
HRESULT CreateListenSocket(CListenSocket **ppListenSocket, XNKID *pSessionID, XNKEY *pKeyExchangeKey);
HRESULT DeleteStressSession(XNKID *pSessionID);

HRESULT client_Main(XNADDR *pHostAddress, XNKID *pSessionID, XNKEY *pKeyExchangeKey);
HRESULT host_Main(void);

VOID WINAPI SGBVT_StartTest( HANDLE LogHandle );
VOID WINAPI SGBVT_EndTest( VOID );
