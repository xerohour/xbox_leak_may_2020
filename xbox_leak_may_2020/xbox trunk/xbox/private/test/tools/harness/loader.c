/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    loader.c

Abstract:

    This module contains loader routine that loads and initializes test
    modules (statically linked).

--*/

#include "precomp.h"
#include "loader.h"
#include "logging.h"

#pragma data_seg( ".EXP$A" )
EXP_TABLE_DIRECTORY __export_table_directory_start[] = { NULL };

#pragma data_seg( ".EXP$Z" )
EXP_TABLE_DIRECTORY __export_table_directory_end[] = { NULL };

#pragma data_seg()

#pragma comment(linker, "/merge:.EXP=.export")

//
// Section names that we use in TESTINI.INI
//

const CHAR GeneralSettingsSectionName[] = "GeneralSettings";
const CHAR ServersSectionName[]         = "Servers";

//
// Key name that we use in both global setting and per-module settings
//

const CHAR LogLevelKeyName[]        = "LogLevel";
const CHAR LogOptionsKeyName[]      = "LogOptions";
const CHAR IterationsKeyName[]      = "Iterations";
const CHAR NumberOfThreadsKeyName[] = "NumberOfThreads";
const CHAR ThreadStackSizeKeyName[] = "ThreadStackSize";
const CHAR SleepBetweenRunKeyName[] = "SleepBetweenRun";
const CHAR ServerKeyName[]          = "Server";
const CHAR FormatAllMUsKeyName[]    = "FormatAllMUs";

//
// The followings are global settings. However per-module settings will
// always override global settings. We will use global settings only if
// there is no per-module settings specified
//

ULONG GlobalThreadStackSize;
ULONG GlobalLogLevel;
ULONG GlobalIterations;
ULONG GlobalNumberOfThreads;
ULONG GlobalLogOptions;
ULONG GlobalSleepBetweenRun;

ULONG DebugVerbose;
BOOL  BreaksBeforeLoadTestModule;
BOOL  FormatAllMUs;

//
// Web server and web pages information for web posting
//

WEB_SERVER_INFO_A WebServerInfo;
CHAR HttpServer[1024];
CHAR ConfigPage[1024];
CHAR StatePage[1024];
CHAR LogPage[1024];
CHAR WebPostFileName[MAX_PATH];

HANDLE
FindModuleHandle(
    LPCSTR ModuleName
    )
{
    PEXP_TABLE_DIRECTORY _ptbegin = __export_table_directory_start;
    PEXP_TABLE_DIRECTORY _ptend = __export_table_directory_end;

    while ( _ptbegin < _ptend ) {
        if ( _ptbegin->ModuleName && _stricmp(_ptbegin->ModuleName, ModuleName) == 0 ) {
            return (HANDLE)_ptbegin;
        }
        _ptbegin++;
    }

    return NULL;
}

DWORD
WINAPI
GetModuleFileNameA(
    HANDLE ModuleHandle,
    LPSTR FileName,
    DWORD Size
    )
{
    PEXP_TABLE_DIRECTORY _ptbegin = __export_table_directory_start;
    PEXP_TABLE_DIRECTORY _ptend = __export_table_directory_end;

    while ( _ptbegin < _ptend ) {
        if ( _ptbegin->ModuleName && ModuleHandle == (HANDLE)_ptbegin ) {
            lstrcpynA( FileName, _ptbegin->ModuleName, Size );
            return lstrlenA( FileName );
        }
        _ptbegin++;
    }

    return 0;
}

PROC
FindProcAddress(
    PCHAR ModuleName,
    PCHAR ProcName
    )
{
    PEXP_TABLE_DIRECTORY _ptbegin = __export_table_directory_start;
    PEXP_TABLE_DIRECTORY _ptend = __export_table_directory_end;
    PROC Proc = NULL;

    while ( _ptbegin < _ptend ) {
        if ( _ptbegin->ModuleName && _stricmp(_ptbegin->ModuleName, ModuleName) == 0 ) {
            PEXP_TABLE_ENTRY p = _ptbegin->TableEntry;
            while ( p->pProcName && p->pfnProc ) {
                if ( _stricmp(p->pProcName, ProcName) == 0 ) {
                    Proc = p->pfnProc;
                    goto done;
                }
                p++;
            }
        }
        _ptbegin++;
    }

done:
    return Proc;
}

VOID
HarnessInitGlobalVars(
    VOID
    )
{
    DebugVerbose = GetProfileIntA( GeneralSettingsSectionName, "DebugVerbose", 0 );
    BreaksBeforeLoadTestModule = GetProfileIntA( GeneralSettingsSectionName, "BreaksBeforeLoadTestModule", FALSE );

    //
    // Get global setting of stack size of thread, the per-module settings
    // will however override this global value (if exists)
    //

    GlobalThreadStackSize = GetProfileIntA( GeneralSettingsSectionName, ThreadStackSizeKeyName, 16384 );

    //
    // Get global setting of SleepBetweenRun, the per-module settings
    // will however override this global value (if exists)
    //

    GlobalSleepBetweenRun = GetProfileIntA( GeneralSettingsSectionName, SleepBetweenRunKeyName, 10000 );

    //
    // Get global setting of log level, the per-module settings
    // will however override this global value (if exists)
    //
    GlobalLogLevel = GetProfileIntA( GeneralSettingsSectionName, LogLevelKeyName, XLL_LOGALL );

    //
    // Get global setting of log level, the per-module settings
    // will however override this global value (if exists)
    //

    GlobalLogOptions = GetProfileIntA(
                            GeneralSettingsSectionName,
                            LogOptionsKeyName,
                            XLO_DEBUG | XLO_CONFIG | XLO_STATE | XLO_REFRESH
                            );

    //
    // Get global setting of iterations, the per-module settings
    // will however override this global value (if exists)
    //

    GlobalIterations = GetProfileIntA( GeneralSettingsSectionName, IterationsKeyName, 1 );

    //
    // Get global setting of number of threads, the per-module settings
    // will however override this global value (if exists)
    //

    GlobalNumberOfThreads = GetProfileIntA( GeneralSettingsSectionName, NumberOfThreadsKeyName, 1 );

    if ( GlobalIterations == 0 ) {
        GlobalIterations = 1;
    }

    if ( GlobalNumberOfThreads == 0 ) {
        GlobalNumberOfThreads = 1;
    }
    
    //
    // Get global setting of whether ot not to format all attached MUs
    //

    FormatAllMUs = GetProfileIntA( GeneralSettingsSectionName, FormatAllMUsKeyName, 0 );

    GetProfileStringA( ServersSectionName, "HttpServer", "", HttpServer, ARRAY_SIZE(HttpServer) );
    IniRemoveStringCommentA( HttpServer );

    GetProfileStringA( ServersSectionName, "ConfigPage", "", ConfigPage, ARRAY_SIZE(ConfigPage) );
    IniRemoveStringCommentA( ConfigPage );

    GetProfileStringA( ServersSectionName, "StatePage", "", StatePage, ARRAY_SIZE(StatePage) );
    IniRemoveStringCommentA( StatePage );

    GetProfileStringA( ServersSectionName, "LogPage", "", LogPage, ARRAY_SIZE(LogPage) );
    IniRemoveStringCommentA( LogPage );

    GetProfileStringA( ServersSectionName, "WebPostFileName", "", WebPostFileName, ARRAY_SIZE(WebPostFileName) );
    IniRemoveStringCommentA( WebPostFileName );

    ZeroMemory( &WebServerInfo, sizeof(WebServerInfo) );
    WebServerInfo.lpszHttpServer      = HttpServer[0] ? HttpServer : NULL;
    WebServerInfo.lpszConfigPage      = ConfigPage[0] ? ConfigPage : NULL;
    WebServerInfo.lpszStatePage       = StatePage[0]  ? StatePage  : NULL;
    WebServerInfo.lpszLogPage         = LogPage[0]    ? LogPage    : NULL;
    WebServerInfo.lpszWebPostFileName = WebPostFileName[0] ? WebPostFileName : NULL;

    if ( DebugVerbose ) {
        DbgPrint(
            "HARNESS: HttpServer = \"%s\"\n"
            "         ConfigPage = \"%s\"\n"
            "         StatePage  = \"%s\"\n"
            "         LogPage    = \"%s\"\n",
            HttpServer,
            ConfigPage,
            StatePage,
            LogPage
            );
    }
}

VOID
HarnessInitPerModuleVars(
    PHARNESS_TEST_LIST_ENTRY p,
    PCSTR LogPath
    )
{
    p->LogLevel = GetProfileIntA( p->FullTestPath, LogLevelKeyName, GlobalLogLevel );
    p->LogOptions = GetProfileIntA( p->FullTestPath, LogOptionsKeyName, GlobalLogOptions );
    p->Iterations = GetProfileIntA( p->FullTestPath, IterationsKeyName, GlobalIterations );
    p->NumberOfThreads = GetProfileIntA( p->FullTestPath, NumberOfThreadsKeyName, GlobalNumberOfThreads );
    p->ThreadStackSize = GetProfileIntA( p->FullTestPath, ThreadStackSizeKeyName, GlobalThreadStackSize );
    p->SleepBetweenRun = GetProfileIntA( p->FullTestPath, SleepBetweenRunKeyName, GlobalSleepBetweenRun );

    if ( p->Iterations == 0 ) {
        p->Iterations = 1;
    }

    if ( p->NumberOfThreads == 0 ) {
        p->NumberOfThreads = 1;
    }

    //
    // If full log file path is specified, we will use that path
    //

    GetProfileStringA( p->FullTestPath, "LogFile", "", p->FullLogPath, ARRAY_SIZE(p->FullLogPath) );
    p->LogHandle = INVALID_HANDLE_VALUE;

    if ( p->FullLogPath[0] == 0 ) {

        PSTR DotPtr;

        strcpy( p->FullLogPath, "t:\\" );
        strcat( p->FullLogPath, LogPath );
        IniRemoveStringCommentA( p->FullLogPath );
        DotPtr = strrchr( p->FullLogPath, '.' );
        if ( DotPtr ) {
            strcpy( DotPtr, ".log" );
        } else {
            strcat( p->FullLogPath, ".log" );
        }

    } else if ( p->FullLogPath[0] && p->FullLogPath[1] != ':' ) {

        RtlMoveMemory(
            &p->FullLogPath[ sizeof("t:\\")-1 ],
            p->FullLogPath,
            sizeof(CHAR) * (strlen(p->FullLogPath) + 1)
            );

        RtlMoveMemory( p->FullLogPath, "t:\\", sizeof("t:\\")-1 );
    }

    IniRemoveStringCommentA( p->FullLogPath );
}

VOID
HarnessUnloadTestModule(
    PHARNESS_TEST_LIST_ENTRY p,
    BOOL CloseLogFile
    )
/*++

Routine Description:

    Unload the test module section to free up memory. This routine will
    close log file if requested

Arguments:

    p - Pointer to HARNESS_TEST_LIST_ENTRY structure

Return Value:

    None

--*/
{
    p->pStartupProc = NULL;
    p->pCleanupProc = NULL;
    p->pShutdownProc = NULL;
    p->pDllMain = NULL;
    p->ModuleHandle = NULL;

    if ( CloseLogFile && p->LogHandle != INVALID_HANDLE_VALUE ) {
        xCloseLog( p->LogHandle );
        p->LogHandle = INVALID_HANDLE_VALUE;
    }
}

BOOL
HarnessLoadTestModule(
    PHARNESS_TEST_LIST_ENTRY p,
    SOCKET Socket,
    BOOL CreateLog
    )
/*++

Routine Description:

    This routine will load the test module

Arguments:

    p - Pointer to HARNESS_TEST_LIST_ENTRY structure

Return Value:

    TRUE/FALSE

--*/
{
    ASSERT( p->pStartupProc == NULL );
    ASSERT( p->pCleanupProc == NULL );
    ASSERT( p->pShutdownProc == NULL );
    ASSERT( p->pDllMain == NULL );
    ASSERT( p->ModuleHandle == NULL );

    if ( BreaksBeforeLoadTestModule ) {
        __asm int 3
    }

    //
    // TODO: add code to load XE section later
    //

    p->ModuleHandle = FindModuleHandle( p->FullTestPath );
    p->pStartupProc = (LPTEST_STARTUP_PROC)FindProcAddress( p->FullTestPath, "StartTest" );
    p->pCleanupProc = (LPTEST_CLEANUP_PROC)FindProcAddress( p->FullTestPath, "EndTest" );
    p->pShutdownProc = (LPTEST_CLEANUP_PROC)FindProcAddress( p->FullTestPath, "ShutdownTest" );
    p->pDllMain = (LPTEST_DLLMAIN_PROC)FindProcAddress( p->FullTestPath, "DllMain" );

    if ( !p->pStartupProc || !p->pCleanupProc) {
        HarnessLog( XLL_WARN, "Unable to locate StartTest or EndTest in %s", p->FullTestPath );
        HarnessUnloadTestModule( p, FALSE );
        return FALSE;
    }

    //
    // Create log file for this module
    //

    if ( CreateLog && p->LogHandle == INVALID_HANDLE_VALUE ) {
        p->LogHandle = xCreateLog_A(
                            p->FullLogPath,
                            &WebServerInfo,
                            Socket,
                            p->LogLevel,
                            p->LogOptions
                            );
    }

    if ( CreateLog && p->LogHandle == INVALID_HANDLE_VALUE ) {
        HarnessLog(
            XLL_WARN,
            "Unable to create log object for log file %s (%s)\n",
            p->FullLogPath,
            WinErrorSymbolicName(GetLastError())
            );
        HarnessUnloadTestModule( p, FALSE );
        return FALSE;
    }

    return TRUE;
}

BOOL
HarnessCreateSynchronizationObjects(
    IN PHARNESS_TEST_LIST_ENTRY p
    )
{
    p->StartTestEventHandle = CreateEvent( NULL, TRUE, FALSE, NULL );
    p->EndTestEventHandle = CreateEvent( NULL, TRUE, FALSE, NULL );

    if ( !p->StartTestEventHandle || !p->EndTestEventHandle ) {
        return FALSE;
    }

    return TRUE;
}

VOID
HarnessDestroySynchronizationObjects(
    IN PHARNESS_TEST_LIST_ENTRY p
    )
{
    if ( p->StartTestEventHandle ) {
        CloseHandle( p->StartTestEventHandle );
        p->StartTestEventHandle = NULL;
    }

    if ( p->EndTestEventHandle ) {
        CloseHandle( p->EndTestEventHandle );
        p->EndTestEventHandle = NULL;
    }
}

VOID
HarnessCleanupModule(
    PHARNESS_TEST_LIST_ENTRY ModuleInfo
    )
{
    HarnessDestroySynchronizationObjects( ModuleInfo );

#if DBG
    if ( !GetProfileIntA( GeneralSettingsSectionName, "StressTestMode", FALSE ) ) {
        ASSERT( ModuleInfo->LogHandle == INVALID_HANDLE_VALUE );
    }
#endif

    if ( ModuleInfo->LogHandle != INVALID_HANDLE_VALUE ) {
        xCloseLog( ModuleInfo->LogHandle );
        ModuleInfo->LogHandle = INVALID_HANDLE_VALUE;
    }
}

BOOL
HarnessInitializeModule(
    LPCSTR ModuleName,
    SOCKET  Socket,
    PHARNESS_TEST_LIST_ENTRY ModuleInfo
    )
/*++

Routine Description:

    This routine will load the test module, create synchronization objects

Arguments:

    p - Pointer to HARNESS_TEST_LIST_ENTRY structure

Return Value:

    TRUE/FALSE

--*/
{
    RtlZeroMemory( ModuleInfo, sizeof(HARNESS_TEST_LIST_ENTRY) );
    ModuleInfo->LogHandle = INVALID_HANDLE_VALUE;
    strncpy( ModuleInfo->FullTestPath, ModuleName, MAX_PATH );
    ModuleInfo->FullTestPath[MAX_PATH-1] = 0;
    IniRemoveStringCommentA( ModuleInfo->FullTestPath );

    HarnessInitPerModuleVars( ModuleInfo, ModuleName );

    if ( HarnessLoadTestModule(ModuleInfo, Socket, FALSE) ) {
        if ( !HarnessCreateSynchronizationObjects(ModuleInfo) ) {
            HarnessLog(
                XLL_WARN,
                "Unable to create synchronization objects for %s (%s)\n",
                ModuleInfo->FullTestPath,
                WinErrorSymbolicName(GetLastError())
                );
        } else {
            // Unload test module here because module runner will load it again
            HarnessUnloadTestModule( ModuleInfo, FALSE );
            return TRUE;
        }
    }

    HarnessUnloadTestModule( ModuleInfo, FALSE );
    HarnessDestroySynchronizationObjects( ModuleInfo );

    if ( ModuleInfo->LogHandle != INVALID_HANDLE_VALUE ) {
        xCloseLog( ModuleInfo->LogHandle );
        ModuleInfo->LogHandle = INVALID_HANDLE_VALUE;
        DeleteFile( ModuleInfo->FullLogPath );
    }

    return FALSE;
}
