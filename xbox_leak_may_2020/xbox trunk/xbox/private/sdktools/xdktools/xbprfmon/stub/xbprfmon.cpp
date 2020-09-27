/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbprfmon.cpp

Abstract:

    This module implements custom PerfMon DLL for Xbox performance
    monitoring.  It polls various performance counters from Xbox using
    debug channel.

--*/

#include "xbprfmonp.h"

SIZE_T g_TotalCounters;
ULONG  g_FirstCounterId;
HINSTANCE g_hInstance;
DWORD g_ConnectionTimeout = 2000;

VOID
WINAPI
EstablishConnection(
    BOOL* fConnectionLost
    )
{
    HRESULT hr;

    DmUseSharedConnection( FALSE );

    for (;;) {
        hr = DmUseSharedConnection( TRUE );
        if ( SUCCEEDED(hr) ) {
            *fConnectionLost = FALSE;
            DmSetConnectionTimeout(g_ConnectionTimeout, g_ConnectionTimeout);
            break;
        }
    }
}

VOID
WINAPI
PollPerformanceData(
    VOID
    )
/*++

Routine Description:

    This routine polls various performance counters from Xbox using debug
    channel (DM APIs) and update into shared memory area for reading by
    xbprfdll.dll

Arguments:

    None

Return Value:

    None

--*/
{
    SIZE_T count;
    HRESULT hr;
    INT64 Value;
    HANDLE hThread;
    BOOL fConnectionLost = FALSE;

    for (;;) {

        WaitForSingleObject( g_PrfData.GetUpdateEventHandle(), INFINITE );

        //
        // BUGBUG: right now this is ugly, client shouldn't have to keep track
        // of number of PO/CO, CPrfData should expose interface to do so
        //

        for ( count=0; count<g_TotalCounters; count++ ) {

            const DM_COUNTINFO* pdmci = g_PrfData.GetDmCtr(
                (CPrfData::CTRID)count+g_FirstCounterId );
            DWORD dwType = pdmci->Type;
            DWORD dwQuery = dwType & DMCOUNT_COUNTTYPE;
            DM_COUNTDATA dmcd;

            if ( dwQuery & DMCOUNT_EVENT || dwQuery & DMCOUNT_PRATIO ) {
                dwQuery |= DMCOUNT_PERSEC;
            }

            if ( !fConnectionLost ) {
                hr = DmQueryPerformanceCounter( pdmci->Name, dwQuery, &dmcd );
            } else {
                ZeroMemory( &dmcd, sizeof(dmcd) );
                hr = E_UNEXPECTED;
            }

            if ( FAILED(hr) ) {
                Value = 0;
                if ( hr == XBDM_CONNECTIONLOST ) {
                    hThread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)EstablishConnection,
                                            (LPVOID)&fConnectionLost, 0, 0 );
                    if ( hThread ) {
                        fConnectionLost = TRUE;
                        CloseHandle( hThread );
                    }
                }
            } else {
                if ( dwQuery & DMCOUNT_PRATIO ) {
                    Value = (INT64)(100.0 * (double)dmcd.CountValue.QuadPart/(double)dmcd.RateValue.QuadPart);
                } else if ( dwQuery & DMCOUNT_EVENT ) {
                    Value = (INT64)((double)dmcd.CountValue.QuadPart/(double)dmcd.RateValue.QuadPart);
                } else {
                    Value = dmcd.CountValue.QuadPart;
                }
            }
            g_PrfData.GetCtr64( (CPrfData::CTRID)count+g_FirstCounterId ) = Value;
        }
    }
}

VOID
XbPrfMonTranslateDmError(
    IN HRESULT Hr
    )
/*++

Routine Description:

    This routine translates HRESULT from DM APIs, displays the results
    using MessageBox API

Arguments:

    Hr - HRESULT returned vt various DM APIs

Return Value:

    None

--*/
{
    CHAR Buffer[512];

    DmTranslateErrorA( Hr, Buffer, ARRAYSIZE(Buffer) );
    MessageBoxA( 0, Buffer, "XbPerfMon",
                 MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL );
}

VOID
XbPrfMonReportError(
    DWORD Error,
    LPCTSTR Format = NULL,
    ...
    )
/*++

Routine Description:

    This routine translates Win32 error code into human-readable text and
    displays it using MessageBox API

Arguments:

    Error - Win32 error code

Return Value:

    None

--*/
{
    LPTSTR p;
    va_list argptr;
    LPVOID lpMsgBuf;
    TCHAR Buffer[512];

    va_start( argptr, Format );

    Buffer[0] = '\n';

    if ( Format ) {
        wvsprintf( &Buffer[1], Format, argptr );
    } else {
        Buffer[0] = _T('\0');
    }

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        Error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
        );

    p = (LPTSTR)LocalReAlloc( lpMsgBuf,
            (lstrlen((LPTSTR)lpMsgBuf)+lstrlen(Buffer)+3) * sizeof(TCHAR), 0 );

    if ( p ) {
        lpMsgBuf = p;
        lstrcat( p, Buffer );
    }

    MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("XbPerfMon"),
                MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL );
    LocalFree( lpMsgBuf );
    va_end( argptr );
}

HANDLE
ExecutePerfMon(
    IN int nCmdShow,
    IN PCWSTR pwszClientName OPTIONAL
    )
/*++

Routine Description:

    This routines invokes perfmon.exe from default search path, waits until
    perfmon terminated and returns

Arguments:

    nCmdShow - Flags that specify how perfmon is to be displayed when it
        is opened. It can be one of the SW_XXX flags

Return Value:

    Process handle of PerfMon.exe

--*/
{
    BOOL b;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    TCHAR CmdLine[256];
    HANDLE hThread;

    ZeroMemory( &pi, sizeof(pi) );

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = (WORD)nCmdShow;

    if ( pwszClientName ) {
        ExpandEnvironmentStrings(
            pwszClientName, CmdLine, ARRAYSIZE(CmdLine) );
    } else {
        ExpandEnvironmentStrings(
            _T("mmc.exe %windir%\\system32\\perfmon.msc"), CmdLine,
            ARRAYSIZE(CmdLine) );
    }

    b = CreateProcess( 0, CmdLine, 0, 0, TRUE, NORMAL_PRIORITY_CLASS,
                       0, 0, &si, &pi );

    if ( b ) {

        CloseHandle( pi.hThread );
        hThread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)PollPerformanceData, 0, 0, 0 );

        if ( hThread ) {
            CloseHandle( hThread );
            return pi.hProcess;
        }

        CloseHandle( pi.hProcess );
    }

    XbPrfMonReportError( GetLastError(), CmdLine );
    return NULL;
}

BOOL
XbPrfMonDisplayUsage(
    VOID
    )
/*++

Routine Description:

    This routine desplay program's command line usage using MessageBox API.
    The command line usage text is stored in string resource so that it
    can be localized.

Arguments:

    None

Return Value:

    Always TRUE (not used)

--*/
{
    TCHAR szText[512];

    LoadString( NULL, IDS_USAGE, szText, ARRAYSIZE(szText) );
    MessageBox( NULL, szText, _T("XbPerfMon"), MB_OK | MB_ICONINFORMATION );
    return TRUE;
}

VOID
InitializePerformanceObject(
    OUT CPrfData::PPRFITM p,
    IN  DWORD  dwId,
    IN  LPCWSTR  pszName,
    IN  LPCWSTR  pszHelp,
    IN  DWORD  CtrIdDefCounter,
    IN  LONG   lMaxInstances,
    IN  DWORD  cchMaxInstName
    )
{
    ZeroMemory( p, sizeof(*p) );
    p->pit = CPrfData::PIT_OBJECT;
    p->dwId = dwId;
    p->pwszName = pszName;
    p->pwszHelp = pszHelp;
    p->dwDetailLevel = PERF_DETAIL_NOVICE;
    p->DefCounter = (CPrfData::CTRID)CtrIdDefCounter;
    p->MaxInstances = (CPrfData::INSTID)lMaxInstances;
    p->cchMaxInstName = cchMaxInstName;
    p->fCollectThisObj = FALSE;
}

HRESULT
InitializeCounterObject(
    OUT CPrfData::PPRFITM p,
    IN DWORD  dwId,
    IN PDM_COUNTINFO pdmci,
    IN BOOL  Sentinel = FALSE
    )
{
    PWSTR pw;
    SIZE_T AllocSize;

    ZeroMemory( p, sizeof(*p) );

    p->pit = Sentinel ? CPrfData::PIT_END : CPrfData::PIT_COUNTER;
    p->dwId = dwId;
    p->dwDetailLevel = PERF_DETAIL_NOVICE;
    p->DefCounter = (CPrfData::CTRID)-1;

    if ( !pdmci ) {
        return NOERROR;
    }

    memcpy( &p->dmci, pdmci, sizeof(*pdmci) );
    AllocSize = sizeof(WCHAR) * sizeof(pdmci->Name);
    pw = (PWSTR)LocalAlloc( LPTR, AllocSize );

    if ( !pw ) {
        return E_OUTOFMEMORY;
    }

    MultiByteToWideChar( CP_ACP, 0, pdmci->Name, -1, pw,
                         sizeof(pdmci->Name) * sizeof(WCHAR) );


    WCHAR SubKey[64 + sizeof(pdmci->Name)];
    CRegSettings regPerf;

    lstrcpy(SubKey, L"Software\\Microsoft\\XboxSDK\\PerfCounter\\");
    lstrcat(SubKey, pw);

    if (ERROR_SUCCESS == regPerf.OpenSubkey(TRUE, HKEY_LOCAL_MACHINE, SubKey)) {
        WCHAR NameSubKey[32], DescSubKey[32], ScaleSubKey[32];
        LANGID LangID = GetSystemDefaultUILanguage();
        DWORD Size;

        wsprintf(NameSubKey,  L"Name%03x", LangID);
        wsprintf(DescSubKey,  L"Description%03x", LangID);
        wsprintf(ScaleSubKey, L"Scale%03x", LangID);

        //
        // Look up name, description and default scale for this counter
        //

        if (regPerf.GetSize(NameSubKey, &Size) == ERROR_SUCCESS && Size != 0) {
            if (Size > AllocSize) {
                AllocSize = Size;
                LocalFree(pw);
                pw = (PWSTR)LocalAlloc(LPTR, Size);
                if (!pw) {
                    return E_OUTOFMEMORY;
                }
            }
            regPerf.GetString(NameSubKey, pw, AllocSize);
            p->pwszName = pw;
        } else {
            p->pwszName = pw;
        }

        if (regPerf.GetSize(DescSubKey, &Size) == ERROR_SUCCESS && Size != 0) {
            pw = (PWSTR)LocalAlloc(LPTR, Size);
            if (!pw) {
                return E_OUTOFMEMORY;
            }
            regPerf.GetString(DescSubKey, pw, Size);
            p->pwszHelp = pw;
        } else if (regPerf.GetSize(L"Description409", &Size) == ERROR_SUCCESS && Size != 0) {
            pw = (PWSTR)LocalAlloc(LPTR, Size);
            if (!pw) {
                return E_OUTOFMEMORY;
            }
            regPerf.GetString(L"Description409", pw, Size);
            p->pwszHelp = pw;
        } else {
            p->pwszHelp = L" ";
        }

        if (regPerf.GetDWORD(ScaleSubKey, &p->dwDefaultScale) != ERROR_SUCCESS && \
            regPerf.GetDWORD(L"Scale409", &p->dwDefaultScale) != ERROR_SUCCESS) {
            p->dwDefaultScale = 0;
        }
    } else {
        if ( pdmci->Type & DMCOUNT_EVENT || pdmci->Type & DMCOUNT_PRATIO ) {
            lstrcatW( pw, L" (/sec average)" );
        }
        p->pwszName = pw;
        p->pwszHelp = L" ";
        p->dwDefaultScale = 0;
    }

    //
    // Internally xbdm use 64-bit counter, so all of ours are 64-bit too
    //

    p->dwCounterType = PERF_COUNTER_LARGE_RAWCOUNT;

    return NOERROR;
}

HRESULT
XbPrfMonConstructPerfItems(
    VOID
    )
{
    HRESULT hr, hr2;
    DM_COUNTINFO dmci;
    CPrfData::PPRFITM pPrfItems;
    SIZE_T NumCounters = 0;
    SIZE_T NumItems = 0;
    SIZE_T MaxItems = 256;
    PDM_WALK_COUNTERS pdmwc = NULL;
    DWORD PrfObjId = 0x1000;

    //
    // Allocate buffer large enough to hold 1 PO and 254 COs (and 1 sentinel)
    //

    pPrfItems = (CPrfData::PPRFITM)LocalAlloc(
        LPTR, sizeof(CPrfData::PRFITM) * (MaxItems) );

    if ( !pPrfItems ) {
        return E_OUTOFMEMORY;
    }

    //
    // Since xbdm has no concept of hierarchy counters, first item here will
    // always be the only one performance object
    //

    InitializePerformanceObject( pPrfItems, PrfObjId, L"Xbox", L"Xbox Performance Object",
                                 PrfObjId+1, PERF_NO_INSTANCES, 0 );
    NumItems++;

    do {
        hr = DmWalkPerformanceCounters( &pdmwc, &dmci );
        if ( SUCCEEDED(hr) ) {

            NumCounters++;

            //
            // Re-allocate buffer if necessary, we have to leave the last
            // item for sentinel as well
            //

            if ( NumItems >= (MaxItems-2) ) {
                MaxItems += 256;
                pPrfItems = (CPrfData::PPRFITM)LocalReAlloc(
                    pPrfItems, sizeof(CPrfData::PPRFITM) * MaxItems, LPTR );
                if ( !pPrfItems ) {
                    hr = E_OUTOFMEMORY;
                    break;
                }
            }

            hr2 = InitializeCounterObject(&pPrfItems[NumItems], PrfObjId+NumCounters, &dmci );

            if ( FAILED(hr2) ) {
                hr = hr2;
                break;
            }

            NumItems++;
        }
    } while ( SUCCEEDED(hr) );

    DmCloseCounters( pdmwc );

    if ( hr == XBDM_ENDOFLIST && NumCounters ) {
        InitializeCounterObject( &pPrfItems[NumItems], (DWORD)-1, 0, TRUE );
        g_PrfData.AssignDynamicPerfItems(pPrfItems);
        g_TotalCounters = NumCounters;
        g_FirstCounterId = PrfObjId+1;
        return NOERROR;
    }

    return E_UNEXPECTED;
}

INT_PTR
WINAPI
GuiDialog(
    HWND hDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM /*lParam*/
    )
{
    if ( uMsg == WM_INITDIALOG ) {
        return TRUE;
    } else if ( uMsg == WM_COMMAND && LOWORD(wParam) == IDCANCEL) {
        DestroyWindow( hDlg );
        return TRUE;
    }

    return FALSE;
}

VOID
WINAPI
GuiThread(
    HWND* hDlg
    )
{
    MSG msg;

    *hDlg = CreateDialog( g_hInstance, MAKEINTRESOURCE(IDD_CONNECTING), 0, (DLGPROC)GuiDialog );

    if ( *hDlg ) {
        ShowWindow( *hDlg, SW_SHOW );
        UpdateWindow( *hDlg );
        while (GetMessage(&msg,NULL, 0, 0)) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
}

INT
WINAPI
_tWinMain(
    HINSTANCE hInstance,
    HINSTANCE /* hInstancePrev */,
    LPTSTR    /* pszCmdLine */,
    int nCmdShow
    )
{
    HRESULT hr;
    DWORD Error;
    int i, argc;
    LPWSTR *argv, arg;
    TCHAR szPath[MAX_PATH];
    LPWSTR pwszXboxName = NULL;
    LPWSTR pwszClientName = NULL;
    BOOL bMachineName = FALSE;
    BOOL bClientName = FALSE;
    HANDLE hMutex, hProcess, hThread;
    HWND hDlg;

    g_hInstance = hInstance;

    //
    // Process and validate command line arguments
    //

    argv = CommandLineToArgvW( GetCommandLineW(), &argc );

    for ( i=1; i<argc; i++ ) {
        arg = argv[i];
        if ( *arg == '/' || *arg == '-' ) {
            arg++;
            if ( *arg == 'x' || *arg == 'X' ) {
                bMachineName = TRUE;
                pwszXboxName = (++i < argc ? argv[i] : NULL);
            } else if ( *arg == 'c' || *arg == 'C' ) {
                bClientName = TRUE;
                pwszClientName = (++i < argc ? argv[i] : NULL);
            } else if ( *arg == 't' || *arg == 'T' ) {
                g_ConnectionTimeout = (++i < argc ? _ttoi(argv[i]) : 2000);
            } else {
                return XbPrfMonDisplayUsage();
            }
        } else {
            return XbPrfMonDisplayUsage();
        }
    }

    if ( (bMachineName && !pwszXboxName) || \
         (bClientName && !pwszClientName) ) {
        return XbPrfMonDisplayUsage();
    }

    if (!g_ConnectionTimeout) {
        g_ConnectionTimeout = 2000;
    }

    //
    // Validate the Xbox machine name (or IP address)
    //

    if ( pwszXboxName ) {

        LPSTR p = (LPSTR)pwszXboxName;
        LPWSTR pw = pwszXboxName;

        while ( *pw ) {
            *p++ = (CHAR)*pw++; // Simple in-place Unicode to ANSI convertion
        }

        *p = 0;
        DmSetXboxName( (LPSTR)pwszXboxName );
    }

    //
    // Check for previous instance of xbprfmon.exe.  There are couple issues
    // that prevent us from launching xbprfmon more than one copy.  The main
    // reason is that xboxdbg will not work for multiple connections at the
    // same time.
    //

    hMutex = CreateMutex( 0, TRUE, XBPRFMON_UUID_STR );

    if ( !hMutex || GetLastError() == ERROR_ALREADY_EXISTS ) {

        //
        // BUGBUG: should we try to bring perfmon's window to top level?
        // or notify user that it's already running
        //

        XbPrfMonReportError( ERROR_SERVICE_EXISTS );
        return 0;
    }

    hThread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)GuiThread, (LPVOID)&hDlg, 0, 0 );
    if ( hThread ) {
        CloseHandle( hThread );
    }

    hr = DmUseSharedConnection( TRUE );

    if ( FAILED(hr) ) {
        XbPrfMonTranslateDmError( hr );
        return 0;
    }

    DmSetConnectionTimeout(g_ConnectionTimeout, g_ConnectionTimeout);

    //
    // Enumerate and construct performance counters on the fly
    //

    hr = XbPrfMonConstructPerfItems();

    if ( FAILED(hr) ) {
        XbPrfMonTranslateDmError( hr );
        return 0;
    }

    //
    // Install and add required keys in registry to expose Xbox performance
    // counters to PerfMon
    //

    GetModuleFileName( hInstance, szPath, ARRAYSIZE(szPath) );
    _tcscpy( _tcsrchr(szPath, _T('\\')) + 1, _T("xbprfdll.dll") );

    if ( GetFileAttributes(szPath) == (DWORD)-1 ) {
        XbPrfMonReportError( GetLastError(), szPath );
        return 0;
    }

    g_PrfData.Install( szPath );

    //
    // Activate performance data collector by creating the memory-mapped file
    //

    Error = g_PrfData.Activate();

    if ( Error != ERROR_SUCCESS ) {
        XbPrfMonReportError( Error );
    } else {
        if ( SUCCEEDED(hr) ) {
            hProcess = ExecutePerfMon( nCmdShow, pwszClientName );
            if ( hProcess ) {
                if ( hDlg ) {
                    PostMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
                }
                DmEnableGPUCounter(TRUE);
                WaitForSingleObject(hProcess, INFINITE);
                CloseHandle(hProcess);
            }
        } else {
            XbPrfMonTranslateDmError( hr );
        }
    }

    //
    // Uninstall, remove registry keys and destroy mutex
    //

    g_PrfData.Uninstall();
    CloseHandle(hMutex);
    DmEnableGPUCounter(FALSE);
    DmUseSharedConnection(FALSE);

    return 0;
}
