/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  client.h

Abstract:

  This module contains the definitions for client.h

Author:

  Steven Kehrli (steveke) 5-Oct-2000

------------------------------------------------------------------------------*/

#pragma once



namespace XNetBVTNamespace {

#define BVTBuffer "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"



// Function prototypes

BOOL
BVT_UDPTest(
    IN HANDLE  hLog,
    IN LPSTR   pNetsyncRemote
);

BOOL
BVT_TCPTest(
    IN HANDLE  hLog,
    IN LPSTR   pNetsyncRemote
);

} // namespace XNetBVTNamespace
