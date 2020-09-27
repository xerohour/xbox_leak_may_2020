/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.h

Abstract:

  This module contains the definitions for util.c

Author:

  Steven Kehrli (steveke) 24-Jan-2002

-----------------------------------------------------------------------------*/

#pragma once



BOOL
GetResourceString(
    UINT    uResource,
    LPWSTR  lpszString,
    DWORD   dwStringSize
    ...
);



#define LOG_CONNECTED     1
#define LOG_DISCONNECTED  2
#define LOG_TIMEOUT       3
#define LOG_UDP_DATA      4
#define LOG_TCP_DATA      5

VOID
LogActivityHeader(
    HANDLE      hFile
);

VOID
LogActivity(
    HANDLE      hFile,
    SYSTEMTIME  *ActivityTime,
    LPWSTR      RemoteName,
    DWORD       dwLogType,
    DWORD       dwBytes
);
