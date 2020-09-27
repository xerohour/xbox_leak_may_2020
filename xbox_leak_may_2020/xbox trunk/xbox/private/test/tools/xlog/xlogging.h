/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  xlogging.h

Abstract:

  This module contains the definitions for xlogging.c

Author:

  Steven Kehrli (steveke) 13-Mar-2000

------------------------------------------------------------------------------*/

#pragma once



namespace xLogNamespace {

// Global definitions

#define BOX_ID_LENGTH            18                      // Maximum length of the box id, in characters
#define OBJECT_ID_LENGTH         38                      // Maximum length of the object id, in characters
#define OS_BUILD_LENGTH          13                      // Maximum length of the os build string, in characters
#define HW_BUILD_LENGTH          13                      // Maximum length of the hw build string, in characters
#define THREAD_ID_LENGTH         4                       // Maximum length of a thread id, in characters
#define TIME_LENGTH              8                       // Maximum length of the time string, in characters
#define LOG_LEVEL_VALUE_LENGTH   10                      // Maximum length of the log level value, in characters
#define LOG_LEVEL_STRING_LENGTH  25                      // Maximum length of the log level string, in characters
#define LOG_OUTPUT_LENGTH        THREAD_ID_LENGTH + TIME_LENGTH + LOG_LEVEL_STRING_LENGTH + COMPONENT_NAME_LENGTH + SUBCOMPONENT_NAME_LENGTH + FUNCTION_NAME_LENGTH + VARIATION_NAME_LENGTH + LOG_STRING_LENGTH + 8

#define NO_COMPONENT_STRING      "{No Component}"        // Default string when no component is specified
#define NO_SUBCOMPONENT_STRING   "{No Subcomponent}"     // Default string when no subcomponent is specified
#define NO_FUNCTION_STRING       "{No Function}"         // Default string when no function is specified
#define NO_VARIATION_STRING      "{No Variation}"        // Default string when no variation is specified

#ifdef _XBOX

#define WEBPOST_LOGFILE_NAME     "T:\\xlog_webpost.log"  // Default web post log file name

#else

#define WEBPOST_LOGFILE_NAME     "xlog_webpost.log"      // Default web post log file name

#endif // _XBOX



// Structure definitions

typedef struct _XLOG_OBJECT {
    HANDLE                     hMemObject;                                          // Handle to the memory object
    CRITICAL_SECTION           *pcs;                                                // Pointer to the object to synchronize access to this log object.
    UUID                       ObjectUUID;                                          // A UUID to uniquely identify this log object.
    CHAR                       szObjectUUID_A[39];                                  // String representation of the ObjectUUID.
    CHAR                       szOwnerAlias_A[OWNER_ALIAS_LENGTH + 1];              // Null-terminated string that specifies the owner alias of this log object.
    CHAR                       szHostName_A[BOX_ID_LENGTH + 1];                     // Null-terminated string that specifies the host name.
    DWORD                      dwPlatformId;                                        // Specifies the operating system platform.
    BOOL                       KernelDebug;                                         // Specifies if ntoskrnl is free or debug.
    ULONG                      KernelVersion;                                       // Specifies the version number of ntoskrnl.
    BOOL                       XapiDebug;                                           // Specifies if xapi is free or debug.
    ULONG                      XapiVersion;                                         // Specifies the version number of xapi.
    DWORD                      dwMajorVersion;                                      // Specifies the major version number of the operating system.
    DWORD                      dwMinorVersion;                                      // Specifies the minor version number of the operating system.
    DWORD                      dwBuildNumber;                                       // Specifies the build number of the operating system.
    LPSTR                      lpszLogFileName_A;                                   // Pointer to a null-terminated string that specifies the filename to which the log output will be directed.  This member is NULL if no log file will be created.
    DWORD                      dwLogFileNameLen;                                    // Specifies the size of the lpszLogFileName_A, in bytes, including the null-terminating character.
    HANDLE                     hLogFile;                                            // An open handle to the specified log file.  This member is INVALID_HANDLE_VALUE if no log file is created.
    LPSTR                      lpszHttpServer_A;                                    // Pointer to a null-terminated string that specifies the http server to which the web posts will be directed.  This member is NULL if no web posts will be made.
    DWORD                      dwHttpServerLen;                                     // Specifies the size of the lpszHttpServer_A, in bytes, including the null-terminating character.
    u_long                     HttpAddr;                                            // Specifies the address of the http server, in network byte order.
    LPSTR                      lpszConfigPage_A;                                    // Pointer to a null-terminated string that specifies the web page to which the system configuration will be web-posted.  This member is NULL if no web posts will be made.
    DWORD                      dwConfigPageLen;                                     // Specifies the size of the lpszConfigPage_A, in bytes, including the null-terminating character.
    LPSTR                      lpszStatePage_A;                                     // Pointer to a null-terminated string that specifies the web page to which the system state will be web-posted.  This member is NULL if no web posts will be made.
    DWORD                      dwStatePageLen;                                      // Specifies the size of the lpszStatePage_A, in bytes, including the null-terminating character.
    LPSTR                      lpszLogPage_A;                                       // Pointer to a null-terminated string that specifies the web page to which the log output will be web-posted.  This member is NULL if no web posts will be made.
    DWORD                      dwLogPageLen;                                        // Specifies the size of the lpszLogPage_A, in bytes, including the null-terminating character.
    LPSTR                      lpszWebPostFileName_A;                               // Pointer to a null-terminated string that specifies the file to cache web posts.  This member is NULL is web posts are made immediately.
    DWORD                      dwWebPostFileNameLen;                                // Specifies the size of lpszWebPostFileName_A, in bytes, including the null-terminating character.
    HANDLE                     hWebPostFile;                                        // An open handle to the specified web post log file.  This member is INVALID_HANDLE_VALUE is web posts are made immediately.
    CRITICAL_SECTION           *pConsoleSocketcs;                                   // Pointer to the object to synchronize access to the console socket.
    SOCKET                     sConsoleSocket;                                      // Socket descriptor used for directing output to a remote console.
    DWORD                      dwLogLevel;                                          // Specifies the logging level for this log object.  It can be any combination of the XLL_ #defines.
    DWORD                      dwLogOptions;                                        // Specifies the logging options for this log object.  It can be any combination of the XLO_ #defines.
    DWORD                      dwVariationsTotal;                                   // Specifies the number of variations total for this log object.
    DWORD                      dwVariationsException;                               // Specifies the number of variations excepted for this log object.
    DWORD                      dwVariationsBreak;                                   // Specifies the number of variations broken for this log object.
    DWORD                      dwVariationsFail;                                    // Specifies the number of variations failed for this log object.
    DWORD                      dwVariationsWarning;                                 // Specifies the number of variations warned for this log object.
    DWORD                      dwVariationsBlock;                                   // Specifies the number of variations blocked for this log object.
    DWORD                      dwVariationsPass;                                    // Specifies the number of variations passed for this log object.
    DWORD                      dwVariationsUnknown;                                 // Specifies the number of variations unknown for this log object.
    struct _THREAD_INFO        *pThreadInfo;                                        // Pointer to the list of thread info associated with this log object.
} XLOG_OBJECT, *PXLOG_OBJECT;



typedef struct _XLOG_OBJECT_INFO_A {
    LPSTR                      lpszLogFileName_A;                                   // Pointer to the log file name
    LPSTR                      lpszHttpServer_A;                                    // Pointer to the http server name
    LPSTR                      lpszConfigPage_A;                                    // Pointer to the http server config page
    LPSTR                      lpszStatePage_A;                                     // Pointer to the http server state page
    LPSTR                      lpszLogPage_A;                                       // Pointer to the http server log page
    LPSTR                      lpszWebPostFileName_A;                               // Pointer to the web post log file
    SOCKET                     sConsoleSocket;                                      // Specifies the console socket
    DWORD                      dwLogLevel;                                          // Specifies the log level
    DWORD                      dwLogOptions;                                        // Specifies the log options
} XLOG_OBJECT_INFO_A, *PXLOG_OBJECT_INFO_A;



typedef struct _XLOG_OBJECT_INFO_W {
    LPWSTR                     lpszLogFileName_W;                                   // Pointer to the log file name
    LPWSTR                     lpszHttpServer_W;                                    // Pointer to the http server name
    LPWSTR                     lpszConfigPage_W;                                    // Pointer to the http server config page
    LPWSTR                     lpszStatePage_W;                                     // Pointer to the http server state page
    LPWSTR                     lpszLogPage_W;                                       // Pointer to the http server log page
    LPWSTR                     lpszWebPostFileName_W;                               // Pointer to the web post log file
    SOCKET                     sConsoleSocket;                                      // Specifies the console socket
    DWORD                      dwLogLevel;                                          // Specifies the log level
    DWORD                      dwLogOptions;                                        // Specifies the log options
} XLOG_OBJECT_INFO_W, *PXLOG_OBJECT_INFO_W;



typedef struct _FUNCTION_INFO {
    LPSTR                      lpszComponentName_A;                                 // Pointer to the null-terminated string that specifies the component name (szComponentName_A).
    LPSTR                      lpszSubcomponentName_A;                              // Pointer to the null-terminated string that specifies the subcomponent name (szSubcomponentName_A).
    LPSTR                      lpszFunctionName_A;                                  // Pointer to the null-terminated string that specifies the function name (szFunctionName_A).
    CHAR                       szComponentName_A[COMPONENT_NAME_LENGTH + 1];        // Null-terminated string that specifies the component name.
    CHAR                       szSubcomponentName_A[SUBCOMPONENT_NAME_LENGTH + 1];  // Null-terminated string that specifies the subcomponent name.
    CHAR                       szFunctionName_A[FUNCTION_NAME_LENGTH + 1];          // Null-terminated string that specifies the function name.
} FUNCTION_INFO, *PFUNCTION_INFO;



typedef struct _VARIATION_ELEMENT {
    LPSTR                      lpszVariationName_A;                                 // Pointer to the null-terminated string that specifies the variation name (szVariationName_A).
    LPSTR                      lpszLogString_A;                                     // Pointer to the null-terminated string that specifies the log string (szLogString_A).
    DWORD                      dwVariationResult;                                   // Specifies the variation result.
    BOOL                       bVariationTime;                                      // Specifies if variation time is logged.
    ULONGLONG                  InitialTime;                                         // Specifies the time at the start of the variation.
    ULONGLONG                  InitialKernelTime;                                   // Specifies the kernel time at the start of the variation.
    ULONGLONG                  InitialIdleTime;                                     // Specifies the idle time at the start of the variation.
    ULONGLONG                  InitialDpcTime;                                      // Specifies the dpc time at the start of the variation.
    ULONGLONG                  InitialInterruptTime;                                // Specifies the interrupt time at the start of the variation.
    struct _VARIATION_ELEMENT  *pPrevVariationElement;                              // Pointer to the previous variation element in the list.
    struct _VARIATION_ELEMENT  *pNextVariationElement;                              // Pointer to the next variation element in the list.
    CHAR                       szVariationName_A[VARIATION_NAME_LENGTH + 1];        // Null-terminated string that specifies the variation name.
    CHAR                       szLogString_A[LOG_STRING_LENGTH + 1];                // Null-terminated string that specifies the log string.
} VARIATION_ELEMENT, *PVARIATION_ELEMENT;



typedef struct _THREAD_INFO {
    DWORD                      dwThreadId;                                          // Specifies the parent of this thread info
    PFUNCTION_INFO             pFunctionInfo;                                       // Pointer to the function info associated with this thread
    PVARIATION_ELEMENT         pVariationList;                                      // Pointer to the list of variation elements associated with this thread
    struct _THREAD_INFO        *pPrevThreadInfo;                                    // Pointer to the previous thread info in the list
    struct _THREAD_INFO        *pNextThreadInfo;                                    // Pointer to the next thread info in the list
} THREAD_INFO, *PTHREAD_INFO;



typedef struct _WEBPOST_LOGFILE {
    CHAR                       szWebPostFileName_A[MAX_PATH];                       // Null-terminated string that specifies the web post log file name.
    DWORD                      dwRefCount;                                          // Specifies the reference count of the log file.
    HANDLE                     hFile;                                               // Handle to the log file.
    struct _WEBPOST_LOGFILE    *pPrevWebPostLogFile;                                // Pointer to the previous web post log file in the list.
    struct _WEBPOST_LOGFILE    *pNextWebPostLogFile;                                // Pointer to the next web post log file in the list.
} WEBPOST_LOGFILE, *PWEBPOST_LOGFILE;



typedef struct _WEB_SOCKET {
    u_long                     HttpAddr;                                            // Specifies the address of the http server, in network byte order.
    SOCKET                     sWebSocket;                                          // Socket descriptor used for web posts.
    struct _WEB_SOCKET         *pPrevWebSocket;                                     // Pointer to the previous web socket in the list.
    struct _WEB_SOCKET         *pNextWebSocket;                                     // Pointer to the next web socket in the list.
} WEB_SOCKET, *PWEB_SOCKET;



typedef struct _XLOG_INFO {
    HANDLE                     hMemObject;                                          // Handle to the memory object
    DWORD                      dwRefCount;                                          // dwRefCount is the xLog ref count
    PWEBPOST_LOGFILE           pWebPostLogFile;                                     // pWebPostLogFile is a pointer to the list of web post log files
    LPSTR                      lpszHostName_A;                                      // lpszHostName_A is a pointer to a null-terminated string that specifies the host name
    LPSTR                      lpszDiskModel;                                       // lpszDiskModel is a pointer to a null-terminated string that specifies the disk model
    LPSTR                      lpszDiskSerial;                                      // lpszDiskSerial is a pointer to a null-terminated string that specifies the disk serial number
} XLOG_INFO, *PXLOG_INFO;



// More global definitions

#define OS_PLATFORM_XBOX  1
#define OS_PLATFORM_NT    2

#define WEBPOST_CONFIGDATA_FORMAT_UNKNOWN_A                \
    "boxID=%s"                                             \
    "&testID=%s"                                           \
    "&osBuild=00.00.0000.00"                               \
    "&hwBuild=00.00.0000.00"
#define WEBPOST_CONFIGDATA_FORMAT_XBOX_A                   \
    "boxID=%s"                                             \
    "&testID=%s"                                           \
    "&osBuild=%02u.%02u.%04u.%02u"                         \
    "&hwBuild=%02u.%02u.%04u.%02u"
#define WEBPOST_CONFIGDATA_FORMAT_NT_A                     \
    "boxID=%s"                                             \
    "&testID=%s"                                           \
    "&osBuild=%02u.%02u.%04u.%02u"                         \
    "&hwBuild=00.00.0000.00"
#define WEBPOST_CONFIGDATA_FORMAT_LENGTH  32
#define WEBPOST_CONFIGDATA_LENGTH         WEBPOST_CONFIGDATA_FORMAT_LENGTH + BOX_ID_LENGTH + OBJECT_ID_LENGTH + OS_BUILD_LENGTH + HW_BUILD_LENGTH + 1

#define WEBPOST_LOGDATA_FORMAT_A                           \
    "boxID=%s"                                             \
    "&testID=%s"                                           \
    "&tester=%s"                                           \
    "&threadID=%04x"                                       \
    "&logLevel=%d"                                         \
    "&component=%s"                                        \
    "&subcomponent=%s"                                     \
    "&function=%s"                                         \
    "&variation=%s"                                        \
    "&logString=%s"
#define WEBPOST_LOGDATA_FORMAT_LENGTH     99
#define WEBPOST_LOGDATA_LENGTH            WEBPOST_LOGDATA_FORMAT_LENGTH + BOX_ID_LENGTH + OBJECT_ID_LENGTH + OWNER_ALIAS_LENGTH + THREAD_ID_LENGTH + LOG_LEVEL_VALUE_LENGTH + COMPONENT_NAME_LENGTH + SUBCOMPONENT_NAME_LENGTH + FUNCTION_NAME_LENGTH + VARIATION_NAME_LENGTH + LOG_STRING_LENGTH + 1

#define WEBPOST_FORMAT_A                                  \
    "HEAD %s HTTP/1.1\r\n"                                 \
    "Accept: */*\r\n"                                      \
    "User-Agent: MSIE 5.0 - XBox Test Logging\r\n"         \
    "Host: %s\r\n"                                         \
    "Content-Length: %d\r\n"                               \
    "Content-Type: application/x-www-form-urlencoded\r\n"  \
    "\r\n"                                                 \
    "%s"
#define WEBPOST_FORMAT_LENGTH             148
#define WEBPAGE_LENGTH                    256
#define HOST_NAME_LENGTH                  128
#define CONTENTLENGTH_LENGTH              10
#define WEBPOST_CONFIG_LENGTH             WEBPOST_FORMAT_LENGTH + WEBPAGE_LENGTH + HOST_NAME_LENGTH + CONTENTLENGTH_LENGTH + WEBPOST_CONFIGDATA_LENGTH
#define WEBPOST_LOG_LENGTH                WEBPOST_FORMAT_LENGTH + WEBPAGE_LENGTH + HOST_NAME_LENGTH + CONTENTLENGTH_LENGTH + WEBPOST_LOGDATA_LENGTH
#define MAX_WEBPOST_LENGTH                WEBPOST_CONFIG_LENGTH > WEBPOST_LOG_LENGTH ? WEBPOST_CONFIG_LENGTH : WEBPOST_LOG_LENGTH



// Function prototypes

VOID
ReleaseWebPostLogFile(
    IN  LPSTR   lpszWebPostFileName_A
);

BOOL
UploadWebPostLogFile(
    IN  HANDLE  hFile
);

BOOL
xPostConfiguration(
    IN  HANDLE  hLog
);

BOOL
WINAPI
xLogConfiguration(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszLogString_A
);

BOOL
WINAPI
xLogResourceStatus(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszLogString_A
);

BOOL
WINAPI
xLogResults(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszLogString
);

} // namespace xLogNamespace
