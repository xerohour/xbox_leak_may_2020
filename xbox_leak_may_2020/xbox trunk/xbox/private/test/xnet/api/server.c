/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  server.c

Abstract:

  This modules contains the server side exports

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef _XBOX

#ifdef __cplusplus
extern "C" {
#endif

// Netsync export table for syncsrv

#pragma data_seg(NETSYNC_EXPORT_SECTION_NAME)
DECLARE_NETSYNC_EXPORT_DIRECTORY(xnetapi_xbox)
#pragma data_seg()

BEGIN_NETSYNC_EXPORT_TABLE(xnetapi_xbox)
    NETSYNC_EXPORT_TABLE_ENTRY("listenTestServer", listenTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("acceptTestServer", acceptTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("connectTestServer", connectTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("sendTestServer", sendTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("WSASendTestServer", WSASendTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("sendtoTestServer", sendtoTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("WSASendToTestServer", WSASendToTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("recvTestServer", recvTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("WSARecvTestServer", WSARecvTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("recvfromTestServer", recvfromTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("WSARecvFromTestServer", WSARecvFromTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("getsocknameTestServer", getsocknameTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("getpeernameTestServer", getpeernameTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("ioctlsocketTestServer", ioctlsocketTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("WSAGetOverlappedResultTestServer", WSAGetOverlappedResultTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("WSACancelOverlappedIOTestServer", WSACancelOverlappedIOTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("selectTestServer", selectTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("shutdownTestServer", shutdownTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("closesocketTestServer", closesocketTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("XNetRegisterKeyTestServer", XNetRegisterKeyTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("XNetUnregisterKeyTestServer", XNetUnregisterKeyTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("XNetXnAddrToInAddrTestServer", XNetXnAddrToInAddrTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("XNetInAddrToXnAddrTestServer", XNetInAddrToXnAddrTestServer)
END_NETSYNC_EXPORT_TABLE(xnetapi_xbox)

#ifdef __cplusplus
}
#endif

#endif // XBOX
