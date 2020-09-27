/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  xlog.h

Abstract:

  This module contains the definitions for XLL_ logging levels and XLO_ logging options

Author:

  Steven Kehrli (steveke) 13-Mar-2000
  
------------------------------------------------------------------------------*/

#pragma once



#ifdef _XBOX

#include <winsockx.h>

#else

#include <winsock2.h>

#endif



#ifdef __cplusplus
extern "C" {
#endif



// XLL_ logging levels

#define XLL_LOGNONE               0x00000000L  // Disables all logging
#define XLL_LOGDEFAULT            0x000001FFL  // Enables logging at all levels except XLL_VARIATION_TIME
#define XLL_LOGALL                0xFFFFFFFFL  // Enables logging at all levels

#define XLL_EXCEPTION             0x00000001L  // Enables logging of exception events
#define XLL_BREAK                 0x00000002L  // Enables logging to break into the debugger
#define XLL_FAIL                  0x00000004L  // Enables logging of failure events
#define XLL_WARN                  0x00000008L  // Enables logging of warning events
#define XLL_BLOCK                 0x00000010L  // Enables logging of blocking events
#define XLL_PASS                  0x00000020L  // Enables logging of pass events
#define XLL_INFO                  0x00000040L  // Enables logging of informational events
#define XLL_VARIATION_END         0x00000080L  // Enables logging of variation end events
#define XLL_VARIATION_START       0x00000100L  // Enables logging of variation start events
#define XLL_VARIATION             (XLL_VARIATION_START | XLL_VARIATION_END)  // Enables logging of variation start and end events
#define XLL_VARIATION_TIME        0x00000200L  // Enables logging of variation time events



// XLO_ logging options

#define XLO_DEBUG                 0x00000001L  // Log output is echoed to the debugger
#define XLO_CONFIG                0x00000002L  // Enables logging of system configuration when the log object is created
#define XLO_STATE                 0x00000004L  // Enables logging of system resource status when the log object is created and again when the log object is closed
#define XLO_REFRESH               0x00000008L  // Creates a new log file.  If the log file already exists, overwrites the file and clears the existing attributes.



// Global definitions

#define OWNER_ALIAS_LENGTH        16           // Maximum length of an owner alias, in characters
#define COMPONENT_NAME_LENGTH     32           // Maximum length of a component name, in characters
#define SUBCOMPONENT_NAME_LENGTH  32           // Maximum length of a subcomponent name, in characters
#define FUNCTION_NAME_LENGTH      32           // Maximum length of a function name, in characters
#define VARIATION_NAME_LENGTH     32           // Maximum length of a variation name, in characters
#define LOG_STRING_LENGTH         255          // Maximum length of the string in a line of log output, in characters



// Global structures

typedef struct _WEB_SERVER_INFO_A {
    LPSTR  lpszHttpServer;                     // Pointer to a null-terminated string (ANSI) that specifies the http server to which the web posts will be directed.  This member is NULL if no web posts will be made.
    LPSTR  lpszConfigPage;                     // Pointer to a null-terminated string (ANSI) that specifies the web page to which the system configuration will be web posted.  This member is NULL if no web posts will be made.
    LPSTR  lpszStatePage;                      // Pointer to a null-terminated string (ANSI) that specifies the web page to which the system state will be web posted.  This member is NULL if no web posts will be made.
    LPSTR  lpszLogPage;                        // Pointer to a null-terminated string (ANSI) that specifies the web page to which the log output will be web posted.  This member is NULL if no web posts will be made.
    LPSTR  lpszWebPostFileName;                // Pointer to a null-terminated string (ANSI) that specifies the file to which the web posts will be cached.  This member is NULL to use the default file name.
} WEB_SERVER_INFO_A, *PWEB_SERVER_INFO_A;

typedef struct _WEB_SERVER_INFO_W {
    LPWSTR  lpszHttpServer;                    // Pointer to a null-terminated string (UNICODE) that specifies the http server to which the web posts will be directed.  This member is NULL if no web posts will be made.
    LPWSTR  lpszConfigPage;                    // Pointer to a null-terminated string (UNICODE) that specifies the web page to which the system configuration will be web posted.  This member is NULL if no web posts will be made.
    LPWSTR  lpszStatePage;                     // Pointer to a null-terminated string (UNICODE) that specifies the web page to which the system state will be web posted.  This member is NULL if no web posts will be made.
    LPWSTR  lpszLogPage;                       // Pointer to a null-terminated string (UNICODE) that specifies the web page to which the log output will be web posted.  This member is NULL if no web posts will be made.
    LPWSTR  lpszWebPostFileName;               // Pointer to a null-terminated string (UNICODE) that specifies the file to which the web posts will be cached.  This member is NULL to use the default file name.
} WEB_SERVER_INFO_W, *PWEB_SERVER_INFO_W;

#ifdef UNICODE
#define WEB_SERVER_INFO  WEB_SERVER_INFO_W
#define PWEB_SERVER_INFO  PWEB_SERVER_INFO_W
#else
#define WEB_SERVER_INFO  WEB_SERVER_INFO_A
#define PWEB_SERVER_INFO  PWEB_SERVER_INFO_A
#endif



// Function prototypes

HANDLE
WINAPI
xCreateLog_A(
    IN  LPSTR               lpszLogFileName_A,
    IN  PWEB_SERVER_INFO_A  pWebServerInfo_A,
    IN  SOCKET              sConsoleSocket,
    IN  DWORD               dwLogLevel,
    IN  DWORD               dwLogOptions
);

HANDLE
WINAPI
xCreateLog_W(
    IN  LPWSTR              lpszLogFileName_W,
    IN  PWEB_SERVER_INFO_W  pWebServerInfo_W,
    IN  SOCKET              sConsoleSocket,
    IN  DWORD               dwLogLevel,
    IN  DWORD               dwLogOptions
);

#ifdef UNICODE
#define xCreateLog  xCreateLog_W
#else
#define xCreateLog  xCreateLog_A
#endif

HANDLE
WINAPI
xSetLogSocket(
    IN  HANDLE  hLog,
    IN  SOCKET  sConsoleSocket
);

BOOL
WINAPI
xCloseLog(
    IN  HANDLE  hLog
);

BOOL
WINAPI
xSetOwnerAlias(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszOwnerAlias_A
);

BOOL
WINAPI
xSetComponent(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszComponentName_A,
    IN  LPSTR   lpszSubcomponentName_A
);

BOOL
WINAPI
xSetFunctionName(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszFunctionName_A
);

BOOL
WINAPI
xStartVariationEx(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszVariationName_A,
    IN  int     nVariationTime
);

BOOL
WINAPI
xStartVariation(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszVariationName_A
);

DWORD
WINAPI
xEndVariation(
    IN  HANDLE  hLog
);

BOOL
WINAPI
xLog_va(
    IN  HANDLE   hLog,
    IN  DWORD    dwLogLevel,
    IN  LPSTR    lpszFormatString_A,
    IN  va_list  varg_ptr
);

BOOL
WINAPI
xLog(
    IN  HANDLE  hLog,
    IN  DWORD   dwLogLevel,
    IN  LPSTR   lpszFormatString_A,
    IN  ...
);

#ifdef __cplusplus
}
#endif
