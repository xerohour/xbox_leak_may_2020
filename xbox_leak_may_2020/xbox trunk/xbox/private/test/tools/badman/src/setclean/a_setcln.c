/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    a_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'A'. It also contains setup and cleanup
    funtions beginning with 'LPA'. For more information, please refer
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
#include <a_cases.bmh>


PVOID LPABCSetup(int CaseNo, LPVOID *SCInfo, HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPABC Ret;
    
    switch (CaseNo) {

    case LPABC_VALID:
        Ret = (LPABC)LocalAlloc(LPTR, 65 * sizeof(ABC));
        if (Ret == NULL) {
            ErrorPrint(hConOut, hLog, "LPABCSetup", CaseNo, "LocalAlloc failed!");
        }
        *SCInfo = (LPVOID)Ret;
        break ;

    case LPABC_NULL:
        Ret = NULL;
        break;

    default:
        ErrorPrint(hConOut, hLog, "LPABCSetup", CaseNo, "Unknown Case");
        Ret = NULL;
    }
    
    return Ret;
#endif
    return 0;
}


void LPABCCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    switch(CaseNo) {
    case LPABC_VALID:
        if ((*SCInfo)!=NULL) {
            if (LocalFree((HANDLE)(*SCInfo))!=NULL)
                ErrorPrint(hConOut,hLog,"LPABCCleanup",CaseNo,"LocalFree failed!");
        }
    case LPABC_NULL:
    default:
        break;
    }
}


PVOID LPABCFLOATSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
#if 0
    LPABCFLOAT Ret;
    
    switch(CaseNo) {
    case LPABCFLOAT_VALID:
        Ret = (LPABCFLOAT) LocalAlloc (LPTR, sizeof(ABCFLOAT)*65);
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"LPABCSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break ;
    case LPABCFLOAT_NULL:
        Ret = NULL;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPABCFLOATSetup",CaseNo,"Unknown Case");
        Ret = (LPABCFLOAT ) NULL;
        break;
    }
    
    return(Ret);
#endif
    return 0;
}



void LPABCFLOATCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    switch(CaseNo) {
    case LPABCFLOAT_VALID:
        if ((*SCInfo)!=NULL) {
            if (LocalFree((HANDLE)(*SCInfo))!=NULL)
                ErrorPrint(hConOut,hLog,"LPABCFLOATBCleanup",CaseNo,"LocalFree failed!");
        }
    case LPABCFLOAT_NULL:
    default:
        break;
    }
}
