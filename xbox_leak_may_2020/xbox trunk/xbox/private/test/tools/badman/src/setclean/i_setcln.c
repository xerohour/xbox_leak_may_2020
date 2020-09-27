/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    i_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'I'. It also contains setup and cleanup
    funtions beginning with 'LPI'. For more information, please refer
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
#include <i_cases.bmh>
#include "limits.h"


int intSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    int Ret;
    
    UNREFERENCED_PARAMETER( SCInfo );
    
    switch(CaseNo) {
    case int_THREAD_PRIORITY_LOWEST:
        Ret = THREAD_PRIORITY_LOWEST;
        break;
    case int_THREAD_PRIORITY_BELOW_NORMAL:
        Ret = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case int_THREAD_PRIORITY_NORMAL:
        Ret = THREAD_PRIORITY_NORMAL;
        break;
    case int_THREAD_PRIORITY_ABOVE_NORMAL:
        Ret = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case int_THREAD_PRIORITY_HIGHEST:
        Ret = THREAD_PRIORITY_HIGHEST;
        break;
    case int_BAD_THREAD_PRIORITY:
        Ret = 256;
        break;
    case int_THIRTEEN:
        Ret = 13;
        break;
    case int_ZERO:
        Ret = 0;
        break;
    case int_MAXINT:
        Ret = INT_MAX;
        break;
    case int_MININT:
        Ret = INT_MIN;
        break;
    case int_MINUS_ONE:
        Ret = -1;
        break;
    case int_46:
        Ret = 46;
        break;
    case int_ONE:
        Ret = 1;
        break;
    case int_2:
        Ret = 2;
        break;
    case int_THREE:
        Ret = 3;
        break;
    case int_100:
        Ret = 100;
        break;
    case int_TIMER:
        Ret=int_TIMER;
        break;
        
    case int_HOTKEY:
        Ret=0xBFFF;
        break;
        
    case int_MAX_POSSIBLE_STRING:
        Ret = 1024;  // get the correct value from Bodin.
        break;
        
    case int_SYSRGN:
        Ret = 4;
        break;
        
    case int_BIGGER_THAN_MAX_POSSIBLE:
        Ret = 1025;  // get the correct value from Bodin.
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"intSetup",CaseNo,"Unknown Case");
        Ret = (int) NULL;
        break;
    }
    
    return(Ret);
}


void intCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


LPINT LPINTSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPINT  Ret, ipPtr;
    int       count;
    
    switch(CaseNo) {
    case LPINT_MIN_INT:
        Ret   = (LPINT)LocalAlloc( LPTR, (6 * sizeof(UINT)));
        if (Ret != NULL) {
            ipPtr = (LPINT)Ret;
            
            for (count = 0; count < 6; count++)
                *ipPtr++ = INT_MIN;
            
        } else
            ErrorPrint(hConOut,hLog,"LPINTSetup",CaseNo,"LocalAlloc failed!");
        (*SCInfo) = (LPVOID)Ret;
        break;
        
    case LPINT_MAX_INT:
        Ret   = (LPINT)LocalAlloc( LPTR, (6 * sizeof(UINT)));
        if (Ret != NULL) {
            
            ipPtr = (LPINT)Ret;
            
            for (count = 0; count < 6; count++)
                *ipPtr++ = INT_MAX;
            
        } else
            ErrorPrint(hConOut,hLog,"LPINTSetup",CaseNo,"LocalAlloc failed!");
        (*SCInfo) = (LPVOID)Ret;
        break;
        
    case LPINT_454:
        Ret   = (LPINT)LocalAlloc( LPTR, (3 * sizeof(UINT)));
        
        if (Ret != NULL) {
            ipPtr = (LPINT)Ret;
            *ipPtr++ = 4;
            *ipPtr++ = 5;
            *ipPtr   = 4;
            
        } else
            ErrorPrint(hConOut,hLog,"LPINTSetup",CaseNo,"LocalAlloc failed!");
        (*SCInfo) = (LPVOID)Ret;
        break;
        
    case LPINT_VALID:
        Ret   = (LPINT)LocalAlloc( LPTR, (6 * sizeof(UINT)));
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"LPINTSetup",CaseNo,"LocalAlloc failed!");
        (*SCInfo) = (LPVOID)Ret;
        break;
        
    case LPINT_65:
        Ret   = (LPINT)LocalAlloc( LPTR, (65 * sizeof(UINT)));
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"LPINTSetup",CaseNo,"LocalAlloc failed!");
        (*SCInfo) = (LPVOID)Ret;
        break;
        
        
        //don't use this one to hard to debug
    case LPINT_RANDOM:
        Ret =  (LPINT) rand();
        break;
        
    case LPINT_MINUS_ONE:
        Ret = (LPINT) -1;
        break;
        
    case LPINT_NULL:
        Ret = (LPINT) NULL;
        break;

    default:
        ErrorPrint(hConOut,hLog,"LPINTSetup",CaseNo,"Unknown Case");
        Ret = (LPINT) NULL;
        break;
    }
    
    return(Ret);
}


void LPINTCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPINT_MIN_INT:
    case LPINT_MAX_INT:
    case LPINT_VALID:
    case LPINT_65:
    case LPINT_454:
    case LPINT_SYSCOLOR:
        
        if (*SCInfo!=NULL){
            if (LocalFree( (HANDLE)(*SCInfo) ) != NULL)
                ErrorPrint(hConOut,hLog,"LPINTCleanup",CaseNo,"LocalFree failed!");
        }
        break;
    default:
        break;
    }
}
