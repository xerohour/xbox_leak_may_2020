/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  util.h

Abstract:

  This module contains the definitions for util.c

Author:

  Steven Kehrli (steveke) 21-Jul-2000

------------------------------------------------------------------------------*/

#pragma once



namespace NetsyncNamespace {

// Function prototypes

VOID
DebugMessage(
    IN  DWORD  dwDebugLevel,
    IN  LPSTR  lpszFormatString,
    ...
);

#define NETSYNC_DEBUG_SEND     0x00000001
#define NETSYNC_DEBUG_RECEIVE  0x00000002
#define NETSYNC_DEBUG_QUEUE    0x00000004
#define NETSYNC_DEBUG_CLIENT   0x00000008
#define NETSYNC_DEBUG_SERVER   0x00000010
#define NETSYNC_DEBUG_SESSION  0x00000020

#define NETSYNC_DEBUG_ALL      0xFFFFFFFF

BOOL
AddRefNet(
);

VOID
ReleaseNet(
);

LPWSTR
GetIniSection(
    HANDLE   hMemObject,
    LPCWSTR  lpszSectionName
);

BOOL
FindString(
    LPWSTR  lpszString,
    LPWSTR  lpszSubString
);

} // namespace NetsyncNamespace
