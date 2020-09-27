/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    log.cpp

Description:

    Logging functions.

*******************************************************************************/

#include "d3dlocus.h"
#ifdef UNDER_XBOX
#include "xlog.h"
#endif // UNDER_XBOX

//******************************************************************************
// Globals
//******************************************************************************

static HANDLE g_hLog = INVALID_HANDLE_VALUE;
static TCHAR  g_szTextLog[2048] = {0};
static BOOL   m_bCreated = FALSE;

//******************************************************************************
void CreateLog(HANDLE hLog) {

#ifdef UNDER_XBOX
    WEB_SERVER_INFO wsi, *pwsi;
#endif // UNDER_XBOX

    ReleaseLog();

#ifndef UNDER_XBOX

    hLog = INVALID_HANDLE_VALUE;

#else

    if (GetStartupContext() & TSTART_STRESS) {
        hLog = INVALID_HANDLE_VALUE;
    }
    else {
        memset(&wsi, 0, sizeof(WEB_SERVER_INFO));
        wsi.lpszHttpServer = TEXT("157.56.11.21");
        wsi.lpszConfigPage = TEXT("/regression/submit/sysConfig.asp");
        wsi.lpszLogPage = TEXT("/regression/submit/log.asp");

        if (hLog == INVALID_HANDLE_VALUE) {
#if WEBLOG
            pwsi = &wsi;
#else
            pwsi = NULL;
#endif
            hLog = xCreateLog_W(L"t:\\d3d.log", pwsi, INVALID_SOCKET, XLL_LOGALL, XLO_CONFIG | XLO_REFRESH);
            if (hLog != INVALID_HANDLE_VALUE) {
                m_bCreated = TRUE;
            }
        }
    }

#endif // UNDER_XBOX

    g_hLog = hLog;

    if (g_hLog != INVALID_HANDLE_VALUE) {
        LogComponent(TEXT("Direct3D"), TEXT("Framework"));
        LogFunction(TEXT("Initialization"));
    }
}

//******************************************************************************
void ReleaseLog() {

#ifdef UNDER_XBOX
    if (g_hLog != INVALID_HANDLE_VALUE && m_bCreated) {
        xCloseLog(g_hLog);
        m_bCreated = FALSE;
    }
    g_hLog = INVALID_HANDLE_VALUE;
#endif

    *g_szTextLog = TEXT('\0');
}

//******************************************************************************
BOOL LogComponent(LPCTSTR szComponent, LPCTSTR szSubcomponent) {

#ifdef UNDER_XBOX

    if (g_hLog != INVALID_HANDLE_VALUE) {
#ifdef UNICODE
        char szComp[512], szSub[512];
        wcstombs(szComp, szComponent, 512);
        wcstombs(szSub, szSubcomponent, 512);
        xSetComponent(g_hLog, szComp, szSub);
#else
        xSetComponent(g_hLog, (LPSTR)szComponent, (LPSTR)szSubcomponent);
#endif // UNICODE        
    }

#endif

    return TRUE;
}

//******************************************************************************
BOOL LogFunction(LPCTSTR szFunction) {

#ifdef UNDER_XBOX

    if (g_hLog != INVALID_HANDLE_VALUE) {
#ifdef UNICODE
        char szFunc[512];
        wcstombs(szFunc, szFunction, 512);
        xSetFunctionName(g_hLog, szFunc);
#else
        xSetFunctionName(g_hLog, (LPSTR)szFunction);
#endif // UNICODE
    }

#endif

    Log(LOG_COMMENT, TEXT("Function: %s"), szFunction);

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

#ifdef UNDER_XBOX

//    if (!(GetStartupContext() & TSTART_HARNESS)) {

        if (g_hLog != INVALID_HANDLE_VALUE) {
#ifdef UNICODE
            char szVariation[512];
            wcstombs(szVariation, szBuffer, 512);
            xStartVariation(g_hLog, szVariation);
#else
            xStartVariation(g_hLog, szBuffer);
#endif // UNICODE
        }
//    }

#endif

    Log(LOG_TEXT, TEXT("BEGIN VARIATION: %s\n"), szBuffer);

    return TRUE;
}

//******************************************************************************
BOOL LogEndVariation() {

#ifdef UNDER_XBOX

//    if (!(GetStartupContext() & TSTART_HARNESS)) {
        if (g_hLog != INVALID_HANDLE_VALUE) {
            xEndVariation(g_hLog);
        }
//    }

#endif

    Log(LOG_TEXT, TEXT("END VARIATION\n"));

    return TRUE;
}

//******************************************************************************
BOOL Log(LOGLEVEL ll, LPCTSTR szFormat, ...) {

    va_list vl;
    TCHAR   *pchH, *pchT;
    DWORD   dwLevel;
#ifndef UNDER_XBOX
    TCHAR   szBuffer[16384];
    TCHAR   szLog[18432];
#else
    TCHAR*  szBuffer;
    TCHAR*  szLog;
    LPTSTR  pszBuffer;

    if (GetStartupContext() & TSTART_STRESS) {
        return TRUE;
    }

    pszBuffer = (LPTSTR)MemAlloc32(34816 * sizeof(TCHAR) + 18432 * sizeof(char));
    if (!pszBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
        __asm int 3;
    }
    szBuffer = pszBuffer;
    szLog = pszBuffer + 16384;
#endif // UNDER_XBOX

    va_start(vl, szFormat);
//    wvsprintf(szBuffer, szFormat, vl);
    _vstprintf(szBuffer, szFormat, vl);
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

#ifdef UNDER_XBOX

//    if (!(GetStartupContext() & TSTART_HARNESS) || ll == LOG_FAIL || ll == LOG_EXCEPTION || ll == LOG_ABORT) {

        if (g_hLog != INVALID_HANDLE_VALUE) {
#ifdef UNICODE
#ifndef UNDER_XBOX
            char szLogA[18432];
#else
            char* szLogA = (char*)(pszBuffer + 34816);
#endif // UNDER_XBOX
            wcstombs(szLogA, szLog, 18432);
            xLog(g_hLog, dwLevel, szLogA);
    //        xputs(szLogA);
#else
            xLog(g_hLog, dwLevel, szLog);
    //        xputs(szLog);
#endif // UNICODE
        }
//    }

#endif

    // Echo logging to the debugger
    DebugString(szLog);

#ifdef UNDER_XBOX
    MemFree32(pszBuffer);
#endif // UNDER_XBOX

    return TRUE;
}
