/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  server.h

Abstract:

  This module contains the definitions for server.c

Author:

  Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#pragma once



// Function prototypes

LPVOID
WINAPI
StreamServerStart(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientInAddrs,
    IN XNADDR   *ClientXnAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
);

BOOL
WINAPI
StreamServerMain(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN u_long   FromInAddr,
    IN XNADDR   *FromXnAddr,
    IN DWORD    dwMessageType,
    IN DWORD    dwMessageSize,
    IN char     *pMessage,
    IN LPVOID   lpContext
);

VOID
WINAPI
StreamServerStop(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN LPVOID   lpContext
);
