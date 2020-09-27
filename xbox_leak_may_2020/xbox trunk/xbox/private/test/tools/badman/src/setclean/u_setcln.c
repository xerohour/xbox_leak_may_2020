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
#include <u_cases.bmh>
#include "limits.h"


UINT UINTSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UINT Ret;
    
    switch(CaseNo) {
        
    case UINT_VALID_PATH_BUFFER_LENGTH:
        Ret = (UINT) MAX_PATH;
        break;
    case UINT_UNIQUE_VALID:
        Ret = 5;
        break;
    case UINT_UNIQUE_INVALID:
        Ret = 0xffffffff;
        break;
    case UINT_MAXIMUM:
        Ret = UINT_MAX;
        break;
    case UINT_ZERO:
        Ret = 0;
        break;
    case UINT_A_VALUE:
        Ret = 65;
        break;
    case UINT_ONE:
        Ret = (UINT)1;
        break;
    case UINT_16:
        Ret = (UINT)16;
        break;
    case UINT_13:
        Ret = (UINT)13;
        break;
    case UINT_2:
        Ret = (UINT)2;
        break;
    case UINT_3:
        Ret = (UINT)3;
        break;
    case UINT_4:
        Ret = (UINT)4;
        break;
    case UINT_70:
        Ret = (UINT)70;
        break;
    default:
        ErrorPrint(hConOut,hLog,"UINTSetup",CaseNo,"Unknown Case");
        Ret = (UINT) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;
}


void UINTCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    CaseNo;
    SCInfo;
    hLog;
    hConOut;
}

