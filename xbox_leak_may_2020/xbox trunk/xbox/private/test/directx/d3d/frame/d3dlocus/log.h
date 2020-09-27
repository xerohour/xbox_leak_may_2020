/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    log.h

Description:

    Direct3D logging functions.

*******************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define LOG_EXCEPTION           0
#define LOG_FAIL                2
#define LOG_ABORT               4
#define LOG_WARN                5
#define LOG_SKIP                6
#define LOG_NOTIMPL             8
#define LOG_PASS                10
#define LOG_DETAIL              12
#define LOG_COMMENT             14
#define LOG_TEXT                16
//#define LOG_BREAK?
//#define LOG_RESULT?

#ifndef UNDER_XBOX

// XLL_ logging levels
#define XLL_LOGNONE             0x00000000L  // Disables all logging
#define XLL_LOGALL              0xFFFFFFFFL  // Enables logging at all levels

#define XLL_EXCEPTION           0x00000001L  // Enables logging of exception events
#define XLL_BREAK               0x00000002L  // Enables logging to break into the debugger
#define XLL_FAIL                0x00000004L  // Enables logging of failure events
#define XLL_WARN                0x00000008L  // Enables logging of warning events
#define XLL_BLOCK               0x00000010L  // Enables logging of blocking events
#define XLL_PASS                0x00000020L  // Enables logging of pass events
#define XLL_INFO                0x00000040L  // Enables logging of informational events
#define XLL_VARIATION           0x00000080L  // Enables logging of variation start/end events

// XLO_ logging options
#define XLO_DEBUG	            0x00000001L  // Log output is echoed to the debugger
#define XLO_CONFIG	            0x00000002L  // Enables logging of system configuration when the log object is created
#define XLO_STATE	            0x00000004L  // Enables logging of system state when the log object is created and again when the log object is closed
#define XLO_REFRESH	            0x00000008L  // Creates a new log file.  If the log file already exists, overwrites the file and clears the existing attributes.

typedef struct _WEB_SERVER_INFO_W {
    LPWSTR  lpszHttpServer;                    // Pointer to a null-terminated string (UNICODE) that specifies the http server to which the web posts will be directed.  This member is NULL if no web posts will be made.
    LPWSTR  lpszConfigPage;                    // Pointer to a null-terminated string (UNICODE) that specifies the web page to which the system configuration will be web posted.  This member is NULL if no web posts will be made.
    LPWSTR  lpszStatePage;                     // Pointer to a null-terminated string (UNICODE) that specifies the web page to which the system state will be web posted.  This member is NULL if no web posts will be made.
    LPWSTR  lpszLogPage;                       // Pointer to a null-terminated string (UNICODE) that specifies the web page to which the log output will be web posted.  This member is NULL if no web posts will be made.
    LPWSTR  lpszWebPostFileName;               // Pointer to a null-terminated string (UNICODE) that specifies the file to which the web posts will be cache.  This member is NULL if web posts will not be cached and instead be made immediately.
} WEB_SERVER_INFO_W, *PWEB_SERVER_INFO_W;

#endif // !UNDER_XBOX

//******************************************************************************
// Data types
//******************************************************************************

typedef DWORD LOGLEVEL;

//******************************************************************************
// Logging function prototypes
//******************************************************************************

void                            CreateLog(HANDLE hLog);
void                            ReleaseLog();
BOOL                            LogComponent(LPCTSTR szComponent, LPCTSTR szSubcomponent);
BOOL                            LogFunction(LPCTSTR szFunction);
BOOL                            LogBeginVariation(LPCTSTR szFormat, ...);
BOOL                            LogEndVariation();
BOOL                            Log(LOGLEVEL ll, LPCTSTR szFormat, ...);

#endif //__LOG_H__
