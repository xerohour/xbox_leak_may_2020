/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    loader.h

Abstract:

    This module contains loader routine that loads and initializes test modules

--*/

#ifndef __LOADER_H__
#define __LOADER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef VOID (WINAPI * LPTEST_STARTUP_PROC)( HANDLE );
typedef VOID (WINAPI * LPTEST_CLEANUP_PROC)( VOID );
typedef BOOL (WINAPI * LPTEST_DLLMAIN_PROC)( HINSTANCE, DWORD, PVOID );

//
// Structure to keep track of module list that the harness has loaded
// from TESTINI.INI. It also keeps any per-module data, handles, etc.
//

typedef struct _HARNESS_TEST_LIST_ENTRY {

    LIST_ENTRY List;

    //
    // The below fields are per-module, there is no global settings
    // equivalent
    //

    CHAR    FullTestPath[MAX_PATH];     // Full DOS-style path of test module
    HMODULE ModuleHandle;               // Module handle
    CHAR    FullLogPath[MAX_PATH];      // Full DOS-style path of log file
    HANDLE  LogHandle;
    HANDLE  StartTestEventHandle;       // Event to start calling StartTest
    HANDLE  EndTestEventHandle;         // Event to start calling EndTest
    ULONG   NumberOfRunningThreads;     // Number of threads running for this module

    LPTEST_STARTUP_PROC pStartupProc;   // Test startup routine
    LPTEST_CLEANUP_PROC pCleanupProc;   // Test cleanup routine
    LPTEST_CLEANUP_PROC pShutdownProc;  // Optional test shutdown routine
    LPTEST_DLLMAIN_PROC pDllMain;       // Optional DllMain

    //
    // The below fields are per-module and there are global settings
    // equivalent. Per-module settings always override global settings
    //

    ULONG ThreadStackSize;              // Similar to GlobalThreadStackSize
    ULONG LogLevel;                     // Similar to GlobalLogLevel
    ULONG LogOptions;                   // Similar to GlobalLogOptions
    ULONG Iterations;                   // Similar to GlobalIterations
    ULONG NumberOfThreads;              // Similar to GlobalNumberOfThreads
    ULONG SleepBetweenRun;              // Similar to GlobalSleepBetweenRun

} HARNESS_TEST_LIST_ENTRY, *PHARNESS_TEST_LIST_ENTRY;

BOOL
HarnessLoadTestModule(
    PHARNESS_TEST_LIST_ENTRY p,
    SOCKET Socket,
    BOOL CreateLog
    );

VOID
HarnessUnloadTestModule(
    PHARNESS_TEST_LIST_ENTRY p,
    BOOL CloseLogFile
    );

BOOL
HarnessLoadTestModules(
    VOID
    );

BOOL
HarnessInitializeModule(
    LPCSTR ModuleName,
    SOCKET Socket,
    PHARNESS_TEST_LIST_ENTRY ModuleInfo
    );

VOID
HarnessCleanupModule(
    PHARNESS_TEST_LIST_ENTRY ModuleInfo
    );

VOID
HarnessInitGlobalVars(
    VOID
    );

VOID
HarnessPostLog(
    VOID
    );

LONG
WINAPI
HarnessUnhandledExceptionFilter(
    LPEXCEPTION_POINTERS ExceptionInfo
    );

extern ULONG DebugVerbose;
extern BOOL TrackAllocation;
extern BOOL BreaksBeforeLoadTestModule;
extern BOOL FormatAllMUs;
extern const CHAR GeneralSettingsSectionName[];

#ifdef __cplusplus
}
#endif

#endif // __LOADER_H__
