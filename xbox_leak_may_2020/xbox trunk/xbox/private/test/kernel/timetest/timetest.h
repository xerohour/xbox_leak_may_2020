/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    timetest.h

Abstract:

    support for timetest.c
    
Author:

    John Daly (johndaly) 12-May-2000

Revision History:

--*/

#ifndef _TIMETEST_
#define _TIMETEST_

//
// includes
//

#include <stdio.h>
#include <xapip.h>
#include <xboxp.h>
#include <xlog.h>
#include <xtestlib.h>

//
// macros
//

#define YEAR_TIME ((ULONGLONG)(10000000) * (ULONGLONG)(60 * 60 * 24 * 365))
// UTC 00:00.0000001, 100 microsseconds, .0000001 second
#define TIME_0001 ((ULONGLONG)(1))  
// UTC 12:00.0000000
#define TIME_1200 ((ULONGLONG)(10000000) * (ULONGLONG)(60 * 60 * 12))
// UTC 23:59.9999999
#define TIME_2359 (((ULONGLONG)(10000000) * (ULONGLONG)(60 * 60 * 24)) -1)

//
//  Macro used to dump the SYSTEM_TIME structure
//

#define  DumpSystemTime( SYS_TIME, DateName ){\
    swprintf(wbuffer,                                                               \
          L"%s wYear:%ld wMonth:%ld Hour:%ld Minute:%ld Second:%ld DayOfWeek:%ld wDay:%ld \n", \
          DateName,                                                                 \
          (long)SYS_TIME.wYear, (long) SYS_TIME.wMonth,                             \
          (long)SYS_TIME.wHour, (long) SYS_TIME.wMinute, (long)SYS_TIME.wSecond,    \
          SYS_TIME.wDayOfWeek, SYS_TIME.wDay);                                      \
    OutputDebugString(wbuffer);}

#define  DumpTimeZone( TIME_ZONE ){     \
        swprintf(wbuffer,               \
              L"Bias:%d STDname:%s\n",  \
              TIME_ZONE.Bias,           \
              TIME_ZONE.StandardName);  \
        OutputDebugString(wbuffer);     \
        DumpSystemTime( TIME_ZONE.StandardDate, TIME_ZONE.StandardName );   \
        swprintf(wbuffer,               \
              L"StandardBias:%d DaylightName:%s\n", \
              TIME_ZONE.StandardBias,   \
              TIME_ZONE.DaylightName);  \
        OutputDebugString(wbuffer);     \
        DumpSystemTime( TIME_ZONE.DaylightDate, TIME_ZONE.DaylightName );   \
        swprintf(wbuffer,               \
              L"DaylightBias:%d\n",     \
              TIME_ZONE.DaylightBias);  \
        OutputDebugString(wbuffer);}

//
// global data
//

//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The 
// default is only one thread.
// 

LONG ThreadID = 0;

//
// Heap handle from HeapCreate
//

HANDLE HeapHandle;

//
// defines
//
#define MAX_DWORD   0xffffffff        // maximum DWORD

//
// function declarations
//

VOID
WINAPI
test_CompareFileTime(
    HANDLE LogHandle
    );

VOID
WINAPI
test_FileTimeToLocalFileTime(
    HANDLE LogHandle
    );

VOID
WINAPI
test_GetTickCount(
    HANDLE LogHandle
    );

VOID
WINAPI
test_GetTimeZoneInformation(
    HANDLE LogHandle
    );

VOID
WINAPI
test_GetTimeZoneInformationSimpleCase(
    HANDLE LogHandle
    );

VOID
WINAPI
test_Sleep(
    HANDLE LogHandle
    );

VOID
WINAPI
test_SleepEx(
    HANDLE LogHandle
    );

VOID
WINAPI
StartTest(
    HANDLE LogHandle
    );

VOID
WINAPI
EndTest(
    VOID
    );

BOOL
WINAPI
DllMain(
    HINSTANCE   hInstance,
    DWORD       fdwReason,
    LPVOID      lpReserved
    );

DWORD
WINAPI
SleepTestThread(
    LPVOID lpParameter
    );

DWORD
WINAPI
SleepExTestThread(
    LPVOID lpParameter
    );

VOID 
CALLBACK 
TestAPCProc(
    ULONG_PTR dwParam
    );

VOID
CALLBACK 
TestFileIOCompletionRoutine(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
    );

BOOL
WINAPI
EqualTime(
    HANDLE LogHandle,
    LPSYSTEMTIME pst1, 
    LPSYSTEMTIME pst2
    );

#endif // _TIMETEST_


