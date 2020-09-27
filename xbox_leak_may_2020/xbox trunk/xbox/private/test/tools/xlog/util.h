/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  util.h

Abstract:

  This module contains the definitions for util.c

Author:

  Steven Kehrli (steveke) 13-Mar-2000

------------------------------------------------------------------------------*/

#pragma once



namespace xLogNamespace {

// Function prototypes

LPWSTR
MultiByteToUnicodeString(
    IN  HANDLE  hMemObject,
    IN  LPSTR   lpszString_A
);

LPSTR
UnicodeToMultiByteString(
    IN  HANDLE  hMemObject,
    IN  LPWSTR  lpszString_W
);

BOOL
FormatAndWriteString(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszFormatString_A,
    IN  ...
);

BOOL
LogBanner(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszTitleString_A,
    IN  LPSTR   lpszLogString_A
);

SOCKET
ConnectWebServer(
    IN  u_long  HttpAddr
);

BOOL
PostString(
    IN  SOCKET  sWebSocket,
    IN  LPSTR   szString_A,
    IN  DWORD   dwStringLen
);

VOID
PostResult(
    IN  HANDLE  hLog,
    IN  DWORD   dwVariationResult,
    IN  LPSTR   lpszComponentName_A,
    IN  LPSTR   lpszSubcomponentName_A,
    IN  LPSTR   lpszFunctionName_A,
    IN  LPSTR   lpszVariationName_A,
    IN  LPSTR   lpszLogString_A
);

VOID
IncrementVariationResult(
    IN  HANDLE  hLog,
    IN  DWORD   dwVariationResult,
    IN  BOOL    bIncrementTotal
);

BOOL
AddRefNet(
);

VOID
ReleaseNet(
);

LPSTR
GetHostName(
    IN HANDLE  hMemObject
);

} // namespace xLogNamespace
