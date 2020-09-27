/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    o_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'O'. It also contains setup and cleanup
    funtions beginning with 'LPO'. For more information, please refer
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
#include <o_cases.bmh>


LPOVERLAPPED LPOVERLAPPEDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPOVERLAPPED Ret;
    
    switch(CaseNo) {
        
    case LPOVERLAPPED_NULL:
        Ret = NULL;
        break;
    case LPOVERLAPPED_VALID:
        if ( (Ret = malloc(sizeof(OVERLAPPED))) == NULL )
            ErrorPrint(hConOut,hLog,"LPOVERLAPPEDSetup",CaseNo,
            "Error allocating memory");
        *SCInfo = Ret;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPOVERLAPPEDSetup",CaseNo,"Unknown Case");
        Ret = (LPOVERLAPPED) NULL;
        break;
    }
    
    return(Ret);
}


void LPOVERLAPPEDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );

    switch(CaseNo) {
    case LPOVERLAPPED_VALID:
        if ( *SCInfo != NULL )
            free(*SCInfo);
        break;
    default:
        break;
    }
}


VOID CompletionRoutine(DWORD dwErrCode, DWORD dwNbytes, LPOVERLAPPED lpOver)
{
    dwErrCode; dwNbytes; lpOver;
}

LPOVERLAPPED_COMPLETION_ROUTINE LPOVERLAPPED_COMPLETION_ROUTINESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    LPOVERLAPPED_COMPLETION_ROUTINE Ret;
    
    UNREFERENCED_PARAMETER( SCInfo );

    switch(CaseNo) {
        
    case LPOVERLAPPED_COMPLETION_ROUTINE_VALID:
        Ret = CompletionRoutine;
        break;
        
    case LPOVERLAPPED_COMPLETION_ROUTINE_NULL:
        Ret = NULL;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPOVERLAPPED_COMPLETION_ROUTINESetup",CaseNo,
            "Unknown Case");
        Ret = (LPOVERLAPPED_COMPLETION_ROUTINE ) NULL;
        break;
    }
    
    return(Ret);
}



void LPOVERLAPPED_COMPLETION_ROUTINECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}
