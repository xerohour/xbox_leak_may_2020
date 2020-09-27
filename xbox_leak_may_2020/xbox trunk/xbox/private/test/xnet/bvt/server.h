/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  server.h

Abstract:

  This modules contains the definitions for server.c

Author:

  Steven Kehrli (steveke) 5-Oct-2000

------------------------------------------------------------------------------*/

#pragma once



// Function prototypes

VOID
WINAPI
BVT_UDPTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
);

VOID
WINAPI
BVT_TCPTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
);
