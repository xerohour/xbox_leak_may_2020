/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    t_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'T'. It also contains setup and cleanup
    funtions beginning with 'LPT'. For more information, please refer
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
#include <t_cases.bmh>


DWORD WINAPI DoNothingThreadProc()
{
    SleepEx( 100, TRUE );
    return 0xdeadbeef;
}


LPTHREAD_START_ROUTINE LPTHREAD_START_ROUTINESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPTHREAD_START_ROUTINE Ret;
    
    switch(CaseNo) {
    case LPTHREAD_START_ROUTINE_VALID_THREAD_ADDRESS:
        Ret = (LPTHREAD_START_ROUTINE)DoNothingThreadProc;
        break;
        
    case LPTHREAD_START_ROUTINE_NULL:
        Ret = (LPTHREAD_START_ROUTINE) NULL;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPTHREAD_START_ROUTINESetup",CaseNo,"Unknown Case");
        Ret = (LPTHREAD_START_ROUTINE) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
}


void LPTHREAD_START_ROUTINECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elimination
    hLog;     // -W3 warning elimination
    hConOut;  // -W3 warning elimination
}
