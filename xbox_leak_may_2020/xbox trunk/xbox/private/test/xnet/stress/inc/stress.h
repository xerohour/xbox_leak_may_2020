/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

    stress.h

Abstract:

    This module defines the common data for the xnetstress client and server

Author:

    Steven Kehrli (steveke) 8-Feb-2000

------------------------------------------------------------------------------*/

#pragma once



namespace XNetStressNamespace {



// Structures & Defines

typedef struct _STRESS_CLIENT {
    u_long  NetsyncClientInAddr;
    XNADDR  ClientXnAddr;
    DWORD   dwLastKeepAlive;
    XNKID   XnKid;
    XNKEY   XnKey;
    u_long  NetsyncRemoteInAddr;
    XNADDR  RemoteXnAddr;
    DWORD   dwStressType;
    DWORD   dwStatus;
} STRESS_CLIENT, *PSTRESS_CLIENT;



#define NUM_STRESS_CLIENTS         64

#define STRESS_TYPE_1              1
#define STRESS_TYPE_2              2

#define STATUS_REGISTERED          1
#define STATUS_STARTED             2



// Session Messages

typedef struct _STRESS_SESSION_MESSAGE {
    DWORD   dwMessageId;
    DWORD   dwStressType;
    XNADDR  XnAddr;
    XNKID   XnKid;
    XNKEY   XnKey;
} STRESS_SESSION_MESSAGE, *PSTRESS_SESSION_MESSAGE;



#define SERVER_KEYPAIR_MSG         NETSYNC_MSG_USER + 11
#define SERVER_REGISTER_MSG        NETSYNC_MSG_USER + 12
#define SERVER_START_MSG           NETSYNC_MSG_USER + 13
#define SERVER_STOP_MSG            NETSYNC_MSG_USER + 14

#define CLIENT_KEYPAIR_MSG         NETSYNC_MSG_USER + 21
#define CLIENT_REGISTER_MSG        NETSYNC_MSG_USER + 22
#define CLIENT_KEEPALIVE_MSG       NETSYNC_MSG_USER + 23



// Status Messages

typedef struct _STRESS_STATUS_REQUEST_MESSAGE {
    DWORD          dwMessageId;
} STRESS_STATUS_REQUEST_MESSAGE, *PSTRESS_STATUS_REQUEST_MESSAGE;

typedef struct _STRESS_STATUS_REPLY_MESSAGE {
    DWORD          dwMessageId;
    DWORD          dwNumStressSessions;
} STRESS_STATUS_REPLY_MESSAGE, *PSTRESS_STATUS_REPLY_MESSAGE;

typedef struct _STRESS_STATUS_SESSION_MESSAGE {
    DWORD          dwMessageId;
    DWORD          dwNumStressSessions;
    DWORD          dwCurStressSession;
    DWORD          dwNumStressClients;
} STRESS_STATUS_SESSION_MESSAGE, *PSTRESS_STATUS_SESSION_MESSAGE;

typedef struct _STRESS_STATUS_CLIENT_MESSAGE {
    DWORD          dwMessageId;
    DWORD          dwNumStressSessions;
    DWORD          dwCurStressSession;
    DWORD          dwNumStressClients;
    DWORD          dwCurStressClient;
    STRESS_CLIENT  StressClient;
} STRESS_STATUS_CLIENT_MESSAGE, *PSTRESS_STATUS_CLIENT_MESSAGE;



#define STRESS_STATUS_REQUEST_MSG  NETSYNC_MSG_USER + 1
#define STRESS_STATUS_REPLY_MSG    NETSYNC_MSG_USER + 2
#define STRESS_STATUS_SESSION_MSG  NETSYNC_MSG_USER + 3
#define STRESS_STATUS_CLIENT_MSG   NETSYNC_MSG_USER + 4

} // namespace XNetStressNamespace
