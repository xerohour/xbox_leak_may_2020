/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  server.h

Abstract:

  This module contains the definitions for server.c

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#pragma once



// Function prototypes


BOOL
WINAPI
XNetStressMain(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN u_long   FromInAddr,
    IN XNADDR   *FromXnAddr,
    IN DWORD    dwMessageType,
    IN DWORD    dwMessageSize,
    IN char     *pMessage,
    IN LPVOID   lpContext
);

LPVOID
WINAPI
XNetStressStart(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientInAddrs,
    IN XNADDR   *ClientXnAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
);

VOID
WINAPI
XNetStressStop(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN LPVOID   lpContext
);

BOOL
WINAPI
XNetStressStatusMain(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN u_long   FromInAddr,
    IN XNADDR   *FromXnAddr,
    IN DWORD    dwMessageType,
    IN DWORD    dwMessageSize,
    IN char     *pMessage,
    IN LPVOID   lpContext
);

LPVOID
WINAPI
XNetStressStatusStart(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientInAddrs,
    IN XNADDR   *ClientXnAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
);

VOID
WINAPI
XNetStressStatusStop(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN LPVOID   lpContext
);
