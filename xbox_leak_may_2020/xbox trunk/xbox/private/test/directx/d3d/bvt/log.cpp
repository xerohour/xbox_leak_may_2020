/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    log.cpp

Description:

    Logging functions.

*******************************************************************************/

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <stdio.h>
#include <d3dx8.h>
//#include <xlog.h>
#ifndef UNDER_XBOX
#include <winsock.h>
#endif
#ifdef UNDER_XBOX
#include "xlog.h"
#endif // UNDER_XBOX
#include "log.h"
#include "util.h"

namespace D3DBVT {

//******************************************************************************
// Data types
//******************************************************************************

typedef HANDLE  (* XLOGPROC_CREATELOGW)(LPWSTR, PWEB_SERVER_INFO_W, SOCKET, DWORD, DWORD);
typedef BOOL    (* XLOGPROC_CLOSELOG)(HANDLE);
typedef BOOL    (* XLOGPROC_SETCOMPONENT)(HANDLE, LPSTR, LPSTR);
typedef BOOL    (* XLOGPROC_SETFUNCTIONNAME)(HANDLE, LPSTR);
typedef BOOL    (* XLOGPROC_STARTVARIATION)(HANDLE, LPSTR);
typedef DWORD   (* XLOGPROC_ENDVARIATION)(HANDLE);
typedef BOOL    (* XLOGPROC_LOG)(HANDLE, DWORD, LPSTR, ...);

//******************************************************************************
// Globals
//******************************************************************************

HINSTANCE                   g_hXLogDLL = NULL;
HANDLE                      g_hLog = INVALID_HANDLE_VALUE;
BOOL                        g_bLogCreated = FALSE;
XLOGPROC_CREATELOGW         g_pfnCreateLogW = NULL;
XLOGPROC_CLOSELOG           g_pfnCloseLog = NULL;
XLOGPROC_SETCOMPONENT       g_pfnSetComponent = NULL;
XLOGPROC_SETFUNCTIONNAME    g_pfnSetFunctionName = NULL;
XLOGPROC_STARTVARIATION     g_pfnStartVariation = NULL;
XLOGPROC_ENDVARIATION       g_pfnEndVariation = NULL;
XLOGPROC_LOG                g_pfnLog = NULL;
TCHAR                       g_szTextLog[2048] = {0};

//******************************************************************************
void CreateLog(HANDLE hLog) {

#ifdef UNDER_XBOX
    WEB_SERVER_INFO wsi;
#endif // UNDER_XBOX

    ReleaseLog();

#ifndef UNDER_XBOX

    g_hXLogDLL = LoadLibrary(TEXT("xlog.dll"));

    if (g_hXLogDLL) {
        g_pfnCreateLogW = (XLOGPROC_CREATELOGW)GetProcAddress(g_hXLogDLL, "xCreateLog_W");
        g_pfnCloseLog = (XLOGPROC_CLOSELOG)GetProcAddress(g_hXLogDLL, "xCloseLog");
        g_pfnSetComponent = (XLOGPROC_SETCOMPONENT)GetProcAddress(g_hXLogDLL, "xSetComponent");
        g_pfnSetFunctionName = (XLOGPROC_SETFUNCTIONNAME)GetProcAddress(g_hXLogDLL, "xSetFunctionName");
        g_pfnStartVariation = (XLOGPROC_STARTVARIATION)GetProcAddress(g_hXLogDLL, "xStartVariation");
        g_pfnEndVariation = (XLOGPROC_ENDVARIATION)GetProcAddress(g_hXLogDLL, "xEndVariation");
        g_pfnLog = (XLOGPROC_LOG)GetProcAddress(g_hXLogDLL, "xLog");

        if (g_pfnCreateLogW && g_pfnCloseLog && hLog == INVALID_HANDLE_VALUE) {
            hLog = g_pfnCreateLogW(L"d3dbvt.log", NULL, INVALID_SOCKET, XLL_LOGALL, XLO_DEBUG | XLO_CONFIG | XLO_REFRESH);
            g_bLogCreated = TRUE;
        }
    }
    else {
        hLog = INVALID_HANDLE_VALUE;
    }

    if (hLog == INVALID_HANDLE_VALUE) {

        g_pfnCreateLogW = NULL;
        g_pfnCloseLog = NULL;
        g_pfnSetComponent = NULL;
        g_pfnSetFunctionName = NULL;
        g_pfnStartVariation = NULL;
        g_pfnEndVariation = NULL;
        g_pfnLog = NULL;
    }

#else

    g_pfnCreateLogW = (XLOGPROC_CREATELOGW)xCreateLog_W;
    g_pfnCloseLog = (XLOGPROC_CLOSELOG)xCloseLog;
    g_pfnSetComponent = (XLOGPROC_SETCOMPONENT)xSetComponent;
    g_pfnSetFunctionName = (XLOGPROC_SETFUNCTIONNAME)xSetFunctionName;
    g_pfnStartVariation = (XLOGPROC_STARTVARIATION)xStartVariation;
    g_pfnEndVariation = (XLOGPROC_ENDVARIATION)xEndVariation;
    g_pfnLog = (XLOGPROC_LOG)xLog;

    memset(&wsi, 0, sizeof(WEB_SERVER_INFO));
    wsi.lpszHttpServer = TEXT("157.56.11.21");
    wsi.lpszConfigPage = TEXT("/regression/submit/sysConfig.asp");
    wsi.lpszLogPage = TEXT("/regression/submit/log.asp");

    if (g_pfnCreateLogW && g_pfnCloseLog && hLog == INVALID_HANDLE_VALUE) {
#if NOWEBLOG
        hLog = g_pfnCreateLogW(L"t:\\d3d.log", NULL, INVALID_SOCKET, XLL_LOGALL, XLO_CONFIG | XLO_REFRESH);
#else
        hLog = g_pfnCreateLogW(L"t:\\d3d.log", &wsi, INVALID_SOCKET, XLL_LOGALL, XLO_CONFIG | XLO_REFRESH);
#endif
        g_bLogCreated = TRUE;
    }

#endif // UNDER_XBOX

    g_hLog = hLog;
}

//******************************************************************************
void ReleaseLog() {

    if (g_bLogCreated && g_pfnCloseLog && g_hLog != INVALID_HANDLE_VALUE) {
        g_pfnCloseLog(g_hLog);
    }

#ifndef UNDER_XBOX
    if (g_hXLogDLL) {
        FreeLibrary(g_hXLogDLL);
    }
#endif // !UNDER_XBOX

    g_hXLogDLL = NULL;
    g_hLog = INVALID_HANDLE_VALUE;
    g_bLogCreated = FALSE;
    g_pfnCreateLogW = NULL;
    g_pfnCloseLog = NULL;
    g_pfnSetComponent = NULL;
    g_pfnSetFunctionName = NULL;
    g_pfnStartVariation = NULL;
    g_pfnEndVariation = NULL;
    g_pfnLog = NULL;
    *g_szTextLog = TEXT('\0');
}

//******************************************************************************
BOOL LogComponent(LPCTSTR szComponent, LPCTSTR szSubcomponent) {

    if (g_pfnSetComponent) {
#ifdef UNICODE
        char szComp[512], szSub[512];
        wcstombs(szComp, szComponent, 512);
        wcstombs(szSub, szSubcomponent, 512);
        g_pfnSetComponent(g_hLog, szComp, szSub);
#else
        g_pfnSetComponent(g_hLog, (LPSTR)szComponent, (LPSTR)szSubcomponent);
#endif // UNICODE        
    }

    return TRUE;
}

//******************************************************************************
BOOL LogFunction(LPCTSTR szFunction) {

    if (g_pfnSetFunctionName) {
#ifdef UNICODE
        char szFunc[512];
        wcstombs(szFunc, szFunction, 512);
        g_pfnSetFunctionName(g_hLog, szFunc);
#else
        g_pfnSetFunctionName(g_hLog, (LPSTR)szFunction);
#endif // UNICODE
    }

    return TRUE;
}

//******************************************************************************
BOOL LogBeginVariation(LPCTSTR szFormat, ...) {

    TCHAR   szBuffer[512];
    va_list vl;

    va_start(vl, szFormat);
    wvsprintf(szBuffer, szFormat, vl);
//    _vstprintf(szBuffer+8, szFormat, vl);
    va_end(vl);

    if (g_pfnStartVariation) {
#ifdef UNICODE
        char szVariation[512];
        wcstombs(szVariation, szBuffer, 512);
        g_pfnStartVariation(g_hLog, szVariation);
#else
        g_pfnStartVariation(g_hLog, szBuffer);
#endif // UNICODE
    }

    Log(LOG_TEXT, TEXT("BEGIN VARIATION: %s\n"), szBuffer);

    return TRUE;
}

//******************************************************************************
BOOL LogEndVariation() {

    if (g_pfnEndVariation) {
        g_pfnEndVariation(g_hLog);
    }

    Log(LOG_TEXT, TEXT("END VARIATION\n"));

    return TRUE;
}

//******************************************************************************
BOOL Log(LOGLEVEL ll, LPCTSTR szFormat, ...) {

    va_list vl;
    TCHAR   *pchH, *pchT;
    DWORD   dwLevel;
#ifndef UNDER_XBOX
    TCHAR   szBuffer[2048];
    TCHAR   szLog[2304];
#else
    TCHAR*  szBuffer;
    TCHAR*  szLog;
    LPTSTR  pszBuffer;

    pszBuffer = (LPTSTR)MemAlloc32(4352 * sizeof(TCHAR) + 2304 * sizeof(char));
    if (!pszBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
        __asm int 3;
    }
    szBuffer = pszBuffer;
    szLog = pszBuffer + 2048;
#endif // UNDER_XBOX

    va_start(vl, szFormat);
    wvsprintf(szBuffer, szFormat, vl);
//    _vstprintf(szBuffer+8, szFormat, vl);
    va_end(vl);

    if (ll == LOG_TEXT) {

        pchH = szBuffer;
        pchT = szBuffer;

        do {
            for (; !(*pchT == TEXT('\n') || *pchT == TEXT('\0')); pchT++);
            if (*pchT == TEXT('\n')) {
                *pchT++ = TEXT('\0');
                _tcscat(g_szTextLog, pchH);
                DebugString(g_szTextLog);
                *g_szTextLog = TEXT('\0');
                pchH = pchT;
            }
        } while (*pchT != TEXT('\0'));

        _tcscat(g_szTextLog, pchH);

#ifdef UNDER_XBOX
        MemFree32(pszBuffer);
#endif // UNDER_XBOX

        return TRUE;
    }

    switch (ll) {

        case LOG_EXCEPTION:
            dwLevel = XLL_EXCEPTION;
            _tcscpy(szLog, TEXT("EXCEPTION: "));
            break;
        case LOG_FAIL:
            _tcscpy(szLog, TEXT("FAIL: "));
            dwLevel = XLL_FAIL;
            break;
        case LOG_ABORT:
            _tcscpy(szLog, TEXT("ABORT: "));
            dwLevel = XLL_BLOCK;
            break;
        case LOG_WARN:
            _tcscpy(szLog, TEXT("WARNING: "));
            dwLevel = XLL_WARN;
            break;
        case LOG_SKIP:
            _tcscpy(szLog, TEXT("SKIP: "));
            dwLevel = XLL_WARN;
            break;
        case LOG_NOTIMPL:
            _tcscpy(szLog, TEXT("NOT IMPLEMENTED: "));
            dwLevel = XLL_WARN;
            break;
        case LOG_PASS:
            _tcscpy(szLog, TEXT("PASS: "));
            dwLevel = XLL_PASS;
            break;
        case LOG_DETAIL:
            _tcscpy(szLog, TEXT("DETAIL: "));
            dwLevel = XLL_INFO;
            break;
        case LOG_COMMENT:
        default:
            *szLog = TEXT('\0');
            dwLevel = XLL_INFO;
            break;
    }

    _tcscat(szLog, szBuffer);

    if (g_pfnLog) {
#ifdef UNICODE
#ifndef UNDER_XBOX
        char szLogA[2304];
#else
        char* szLogA = (char*)(pszBuffer + 4352);
#endif // UNDER_XBOX
        wcstombs(szLogA, szLog, 2304);
        g_pfnLog(g_hLog, dwLevel, szLogA);
#else
        g_pfnLog(g_hLog, dwLevel, szLog);
#endif // UNICODE
    }

    // Echo logging to the debugger
    DebugString(szLog);

#ifdef UNDER_XBOX
    MemFree32(pszBuffer);
#endif // UNDER_XBOX

    return TRUE;
}

//******************************************************************************
// Debugging functions
//******************************************************************************

//#if defined(DEBUG) || defined(_DEBUG)
//******************************************************************************
//
// Function:
//
//     DebugString
//
// Description:
//
//     Take the formatted output, prepend the application name, and send the
//     output to the debugger.
//
// Arguments:
//
//     LPCTSTR szFormat         - Formatting string describing the output
//
//     Variable argument list   - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DebugString(LPCTSTR szFormat, ...) {

    va_list vl;
#ifndef UNDER_XBOX
    TCHAR   szBuffer[2304];
#else
    TCHAR*  szBuffer = (LPTSTR)MemAlloc32(2304 * sizeof(TCHAR));
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
        __asm int 3;
    }
#endif // UNDER_XBOX

    va_start(vl, szFormat);
#ifdef UNDER_CE
    wvsprintf(szBuffer, szFormat, vl);
#else
    _vstprintf(szBuffer, szFormat, vl);
#endif // DRAGON
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    OutputDebugString(szBuffer);

#ifdef UNDER_XBOX
    MemFree32(szBuffer);
#endif // UNDER_XBOX
}

//******************************************************************************
//
// Function:
//
//     ResultFailed
//
// Description:
//
//     Test a given return code: if the code is an error, output a debug 
//     message with the error value.  If the code is a warning, output
//     a debug message with the warning value.
//
// Arguments:
//
//     HRESULT hr               - Return code to test for an error
//
//     LPCTSTR sz               - String describing the method that produced 
//                                the return code
//
// Return Value:
//
//     TRUE if the given return code is an error, FALSE otherwise.
//
//******************************************************************************
BOOL ResultFailed(HRESULT hr, LPCTSTR sz) {

    TCHAR szError[256];

    if (SUCCEEDED(hr)) {
        return FALSE;
    }

    D3DXGetErrorString(hr, szError, 256);

    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("%s failed with %s [0x%X]"), sz, szError, hr);
        return TRUE;
    }
    else {
        Log(LOG_WARN, TEXT("%s returned %s [0x%X]"), sz, szError, hr);
    }

    return FALSE;
}
//#endif // DEBUG || _DEBUG

} // namespace D3DBVT
