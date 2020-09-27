/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    h_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'H'. It also contains setup and cleanup
    funtions beginning with 'LPH'. For more information, please refer
    to BadMan.Doc.

Author:

    John Miller (johnmil) 02-Feb-1992

Environment:

    XBox

Revision History:

    01-Apr-2000     schanbai

        Ported to XBox and removed not needed cases

--*/


#include <setcln.h>
#include <h_Cases.bmh>
#include <i_Cases.bmh>
#include <memmgt.h>
#include <stdlib.h>
#include <time.h>
#include <filever.h>
#include "limits.h"
#include <psapi.h>
#include <tchar.h>


HANDLE HANDLESetup(int CaseNo, LPVOID *SCInfo, HANDLE hLog, HANDLE hConOut)
{
    //
    // INVALID_HANDLE_VALUE isn't always invalid...  It just so happens that
    // INVALID_HANDLE_VALUE == GetCurrentProcess()...
    //
    
    HANDLE  hRet = (HANDLE)0xDEADBEEF;
    DWORD   id;
    
    switch (CaseNo) {
        
    case HANDLE_RANDOM:
        // you know that 'random' handles are neither always valid, nor
        // always invalid, don't you?
        srand( (unsigned) clock() );      // added 4/3/92 tommcg
        hRet = (HANDLE) rand();
        break;
        
    case HANDLE_NULL:
        hRet = (HANDLE) NULL;
        break;

    case HANDLE_INVALID:
        hRet = INVALID_HANDLE_VALUE;
        break;
        
    case HANDLE_UNLOCKED_LMEM_BLOCK:
    case HANDLE_LOCKED_LMEM_BLOCK:
    case HANDLE_NONDISCARDABLE_LMEM_BLOCK:
        //  case HANDLE_DISCARDED_LMEM_BLOCK:
    case HANDLE_FREED_LMEM_BLOCK:
        hRet = LMemHandleCreate(hLog,hConOut,CaseNo);
        *SCInfo = (LPVOID) hRet;
        break;
        
    case HANDLE_UNLOCKED_GMEM_BLOCK:
    case HANDLE_LOCKED_GMEM_BLOCK:
    case HANDLE_NONDISCARDABLE_GMEM_BLOCK:
    case HANDLE_DISCARDED_GMEM_BLOCK:
    case HANDLE_FREED_GMEM_BLOCK:
        hRet = GMemHandleCreate(hLog,hConOut,CaseNo);
        *SCInfo = (LPVOID) hRet;
        break;

    case HANDLE_PROCESS_HEAP:
        hRet = GetProcessHeap();
        *SCInfo = hRet;
        break;
        
    case HANDLE_GROWABLE_HEAP:
    case HANDLE_32K_HEAP:
    case HANDLE_UNGROWABLE_FILLED_HEAP:
    case HANDLE_DESTROYED_32K_HEAP:
        hRet = HeapHandleCreate(hLog,hConOut,CaseNo);
        *SCInfo = (LPVOID) hRet;
        break;
        
    case HANDLE_THREAD:
        
        hRet = CreateThread(NULL,
                    0L,
                    (LPTHREAD_START_ROUTINE)InfThread,
                    0L,
                    0L,
                    &id);
        
        *SCInfo = (LPVOID)hRet;
        
        break;
        
    case HANDLE_CURRENT_THREAD:
        
        hRet = GetCurrentThread();
        
        break;
        
    case HANDLE_SUSPENDED_THREAD:
        hRet = CreateThread( NULL, 0L, (LPTHREAD_START_ROUTINE)InfThread, 0L, CREATE_SUSPENDED, &id );
        *SCInfo = (LPVOID)hRet;
        break;
        
    case HANDLE_NORMAL_FILE:
    case HANDLE_READONLY_FILE:
    case HANDLE_CLOSED_FILE:
    case HANDLE_FINDFILE:
    case HANDLE_CLOSED_FINDFILE:
    case HANDLE_NORMAL_ASYNC_FILE:
    case HANDLE_READONLY_ASYNC_FILE:
        hRet = FIOHandleSetup(CaseNo, SCInfo, hLog, hConOut);
        break;
        
    case HANDLE_GOOD_CHANGE_NOTIFY:
        //hRet = FindFirstChangeNotification("c:\\", TRUE, FILE_NOTIFY_CHANGE_FILE_NAME);
        if ( hRet == INVALID_HANDLE_VALUE ) {
            
            ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
                "Could not open change notification handle");
            *SCInfo = NULL;
            break;
        }
        *SCInfo = (LPVOID)hRet;
        break;
        
    case HANDLE_CLOSED_CHANGE_NOTIFY:
        //hRet = FindFirstChangeNotification("c:\\", TRUE, FILE_NOTIFY_CHANGE_FILE_NAME);
        if ( hRet == INVALID_HANDLE_VALUE ) {
            
            ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
                "Could not open change notification handle");
            *SCInfo = NULL;
            break;
        }
        /*if ( 0 ) {
            ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo, "error closing change notification handle");
            break;
        }*/
        break;
        
    case HANDLE_LOCAL_FILE:
        // handle to a local file
        //ErrorPrint(hConOut, hLog, "HANDLESetup", CaseNo, "(HANDLE_LOCAL_FILE) not implemented");
        break;
        
    case HANDLE_MAILSLOT:
        // handle to a mailslot
        //ErrorPrint(hConOut, hLog, "HANDLESetup", CaseNo, "(HANDLE_MAILSLOT) not implemented");
        break;
        
    case HANDLE_NAMED_EVENT:
        hRet = CreateEventW( NULL, FALSE, TRUE, (LPCWSTR)L"BADMAN Named Event" );
        *SCInfo = hRet;
        
        if (!hRet) {
            ErrorPrint(
                hConOut,
                hLog,
                "HANDLESetup",
                CaseNo,
                "CreateEvent failed"
                );
        }
        
        break;
        
    case HANDLE_NAMED_MUTANT:
        // handle to a named mutant
        //ErrorPrint(hConOut, hLog, "HANDLESetup", CaseNo, "(HANDLE_NAMED_MUTANT) not implemented");
        break;
        
    case HANDLE_NAMED_SECTION:
        // handle to a named section
        //ErrorPrint(hConOut, hLog, "HANDLESetup", CaseNo, "(HANDLE_NAMED_SECTION) not implemented");
        break;
        
    case HANDLE_NAMED_SEMAPHORE:
        hRet = CreateSemaphoreW(NULL, 0, 10, (LPCWSTR)L"BADMAN Named Semaphore");
        *SCInfo = hRet;
        break;
        
    case HANDLE_NAMEDPIPE:
        // handle to a named pipe
        //ErrorPrint(hConOut, hLog, "HANDLESetup", CaseNo, "(HANDLE_NAMEDPIPE) not implemented");
        break;
        
    case HANDLE_REMOTE_FILE:
        // handle to a remote file
        //ErrorPrint(hConOut, hLog, "HANDLESetup", CaseNo, "(HANDLE_REMOTE_FILE) not implemented");
        break;
        
    case HANDLE_UNNAMED_EVENT:
        // handle to an unnamed event
        *SCInfo = hRet = CreateEventW(NULL, FALSE, TRUE, NULL);
        break;
        
    case HANDLE_UNNAMED_SECTION:
        // handle to an unnamed section
        //ErrorPrint(hConOut, hLog, "HANDLESetup", CaseNo, "(HANDLE_UNNAMED_SECTION) not implemented");
        break;
        
    case HANDLE_UNNAMED_SEMAPHORE:
        *SCInfo = hRet = CreateSemaphoreW(NULL, 0, 10, NULL);
        break;
        
    case HANDLE_NAMED_WAITABLE_TIMER:
        hRet = CreateWaitableTimerW( 0, TRUE, (LPCWSTR)L"Badman Waitable Timer" );
        *SCInfo = (LPVOID)hRet;
        break;
        
    case HANDLE_UNNAMED_WAITABLE_TIMER:
        hRet = CreateWaitableTimerW( 0, TRUE, NULL );
        *SCInfo = (LPVOID)hRet;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"HANDLESetup",CaseNo,"Unknown Case");
        hRet = (HANDLE)0;
        break;
    }
    
    return hRet;
}

void HANDLECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    char                  szError[50];  //debug ouput string
    UINT                  uFlags;    //stores *Flags information

    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
    
    switch(CaseNo) {
        
    case HANDLE_LOCKED_LMEM_BLOCK:
    case HANDLE_UNLOCKED_LMEM_BLOCK:
    case HANDLE_NONDISCARDABLE_LMEM_BLOCK:
        if (LocalFree((HANDLE) *SCInfo) != NULL){
            uFlags = 0;
            sprintf(szError, "LocalFree Failed-LocalFlags = %u", uFlags);
            ErrorPrint( hConOut, hLog, "HANDLECleanup", CaseNo, szError );
        }
        break;
        
    case HANDLE_LOCKED_GMEM_BLOCK:
    case HANDLE_UNLOCKED_GMEM_BLOCK:
    case HANDLE_NONDISCARDABLE_GMEM_BLOCK:
        if (GlobalFree((HANDLE) *SCInfo) != NULL){
            uFlags = 0;
            sprintf(szError, "GlobalFree Failed-GlobalFlags = %u", uFlags);
            ErrorPrint( hConOut, hLog, "HANDLECleanup", CaseNo, szError );
        }
        break;
        
    case HANDLE_GROWABLE_HEAP:
    case HANDLE_32K_HEAP:
    case HANDLE_UNGROWABLE_FILLED_HEAP:
        HeapDestroy((HANDLE) *SCInfo);
        break;
        
    case HANDLE_THREAD:
    case HANDLE_SUSPENDED_THREAD:
        if (*SCInfo) {
            ULONG PreviousSuspendCount;
            NTSTATUS Status;

            do {
                Status = NtResumeThread( (HANDLE)*SCInfo, &PreviousSuspendCount );
            } while ( NT_SUCCESS(Status) && PreviousSuspendCount );

            SetThreadPriority( (HANDLE)*SCInfo, THREAD_PRIORITY_HIGHEST );
            QueueUserAPC((PAPCFUNC)ExitThread, (HANDLE)*SCInfo, 0xdeadbeef);
            NtAlertThreadEx( (HANDLE)*SCInfo, UserMode );
            NtYieldExecution();

            while ( WaitForSingleObject((HANDLE)*SCInfo, 5000) == WAIT_TIMEOUT ) {
                KdPrint(( "SETCLN: timeout while waiting for thread to be terminated\n" ));
            }
            
            if (!CloseHandle( (HANDLE)(*SCInfo) ))
                ErrorPrint(hConOut,hLog,"HANDLECleanup",CaseNo,"CloseHandle(hThread) failed");
        }
        break;
        
    case HANDLE_NORMAL_FILE:
    case HANDLE_READONLY_FILE:
    case HANDLE_CLOSED_FILE:
    case HANDLE_FINDFILE:
    case HANDLE_CLOSED_FINDFILE:
    case HANDLE_NORMAL_ASYNC_FILE:
    case HANDLE_READONLY_ASYNC_FILE:
        FIOHandleCleanup(CaseNo, SCInfo, hLog, hConOut);
        break;
        
    case HANDLE_GOOD_CHANGE_NOTIFY:
        if ( *SCInfo != NULL ) {
            //if ( 0 ) {
                //ErrorPrint(hConOut, hLog, "FIOHandleCleanup", CaseNo,
                    //"error closing change notification handle");
                //break;
            //}
        }
        break;
        
    case HANDLE_LOCAL_FILE:
    case HANDLE_MAILSLOT:
    case HANDLE_NAMED_EVENT:
    case HANDLE_NAMED_MUTANT:
    case HANDLE_NAMED_SECTION:
    case HANDLE_NAMED_SEMAPHORE:
    case HANDLE_NAMEDPIPE:
    case HANDLE_REMOTE_FILE:
    case HANDLE_UNNAMED_EVENT:
    case HANDLE_UNNAMED_SECTION:
    case HANDLE_UNNAMED_SEMAPHORE:
    case HANDLE_NAMED_WAITABLE_TIMER:
    case HANDLE_UNNAMED_WAITABLE_TIMER:
        if (*SCInfo) {
            if (!CloseHandle((HANDLE)*SCInfo)) {
                ErrorPrint(hConOut,
                    hLog,
                    "HandleCleanup",
                    CaseNo,
                    "error closing handle");
            }
        }
        
        break;
        
    default:
        break;
    }
}


LPHANDLE
LPHANDLESetup(
              int         CaseNo,
              LPVOID          *SCInfo,
              HANDLE          hLog,
              HANDLE          hConOut
              )
{
    LPHANDLE        Ret = (LPHANDLE)NULL;
    SE_CLEANUP_INFO     *CleanupInfo = NULL;
    
    switch (CaseNo) {
    case LPHANDLE_NULL:
    /*
    * no action, default is correct.
        */
        
        break;
        
    case LPHANDLE_INVALID:
    /*
    * an invalid pointer
        */
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            INVALID_POINTER,
            0,
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPHANDLESetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
        
    case LPHANDLE_VALID:
    /*
    * pointer to a handle sized region
        */
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            VALIDRANGE_POINTER,
            sizeof (HANDLE),
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPHANDLESetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
        
    default:
        ErrorPrint(hConOut, hLog, "LPHANDLESetup", CaseNo,
            "Unknown Case");
        
        break;
    }
    
    return Ret;
}


void
LPHANDLECleanup(
                int         CaseNo,
                LPVOID          *SCInfo,
                HANDLE          hLog,
                HANDLE          hConOut
                )
{
    switch(CaseNo) {
    case LPHANDLE_INVALID:
    case LPHANDLE_VALID:
    /*
    * MakeBadPointer was used in setup, so clean up the mess that
    * way too.
        */
        
        MakeBadPointer(NULL,
            (SE_CLEANUP_INFO **)SCInfo,
            CLEANUP,
            0,
            hConOut,
            hLog);
        
        if (*SCInfo) {
            ErrorPrint(hConOut, hLog, "LPHANDLECleanup", CaseNo,
                "Cleanup failed.");
        }
        
        break;
        
    default:
    /*
    * no action
        */
        
        break;
    }
}
