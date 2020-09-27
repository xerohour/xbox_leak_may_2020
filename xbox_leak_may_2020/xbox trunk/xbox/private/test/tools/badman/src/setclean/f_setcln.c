/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    f_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'F'. It also contains setup and cleanup
    funtions beginning with 'LPF'. For more information, please refer
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
#include <f_cases.bmh>
#include "float.h"

#pragma warning( disable : 4054 )

FARPROC FARPROCSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    FARPROC Ret;
    
    switch(CaseNo) {
        
    case FARPROC_NULL:
        Ret = (FARPROC)NULL;
        *SCInfo = (LPVOID)Ret;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"FARPROCSetup",CaseNo,"Unknown Case");
        Ret = (FARPROC)NULL;
    }
    
    return Ret;
}


void FARPROCCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


FLOAT FLOATSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    FLOAT Ret;
    
    UNREFERENCED_PARAMETER( SCInfo );

    switch(CaseNo) {
    case FLOAT_ONE:
        Ret = 1.0f;
        break;
    case FLOAT_ZERO:
        Ret = 0.0f;
        break;
    default:
        ErrorPrint(hConOut,hLog,"FLOATSetup",CaseNo,"Unknown Case");
        Ret = 0.0f;
        break;
    }
    
    return(Ret);
}


void FLOATCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


LPFILETIME LPFILETIMESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPFILETIME Ret;
    
    switch(CaseNo) {
        
    case LPFILETIME_VALID_ADDRESS:
    case LPFILETIME_VALID_DATA:
        if ( (Ret = malloc(sizeof(FILETIME))) == NULL )
            ErrorPrint(hConOut,hLog,"LPFILETIMESetup",CaseNo,
            "Error allocating memory");
        *SCInfo = Ret;
        break;
    case LPFILETIME_NULL:
        Ret = NULL;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPFILETIMESetup",CaseNo,"Unknown Case");
        Ret = (LPFILETIME) NULL;
        break;
    }
    
    return(Ret);
}


void LPFILETIMECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );

    switch(CaseNo) {
    case LPFILETIME_VALID_ADDRESS:
    case LPFILETIME_VALID_DATA:
        if ( *SCInfo != NULL )
            free(*SCInfo);
    default:
        break;
    }
}


PFLOAT PFLOATSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    PFLOAT    Ret,
        pfPtr;
    int       count;
    
    switch(CaseNo) {
    case PFLOAT_NULL:
        Ret = (PFLOAT) NULL;
        break;
        
    case PFLOAT_RANDOM:
        Ret = (PFLOAT) rand();
        break;
        
    case PFLOAT_MINUS_ONE:
        Ret = (PFLOAT) -1;
        break;
        
    case PFLOAT_MAX_FLOAT:
        Ret = (PFLOAT) LocalAlloc (LPTR, 5*sizeof(FLOAT));
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"PFLOATSetup",CaseNo,"LocalAlloc failed!");
        pfPtr = Ret;
        for (count = 0; count < 5; count++)
            *pfPtr++ = FLT_MAX;
        *SCInfo = Ret;
        break ;
        
    case PFLOAT_MIN_FLOAT:
        Ret = (PFLOAT) LocalAlloc (LPTR, 5*sizeof(FLOAT));
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"PFLOATSetup",CaseNo,"LocalAlloc failed!");
        
        pfPtr = Ret;
        for (count = 0; count < 5; count++)
            *pfPtr++ = FLT_MIN;
        *SCInfo = Ret;
        break ;
        
    case PFLOAT_65:
        Ret = (PFLOAT) LocalAlloc (LPTR, 65*sizeof(FLOAT));
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"PFLOATSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break ;
        
    default:
        ErrorPrint(hConOut,hLog,"PFLOATSetup",CaseNo,"Unknown Case");
        Ret = (PFLOAT ) NULL;
        break;
    }
    
    return(Ret);
}


void PFLOATCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    switch(CaseNo) {
    case PFLOAT_MIN_FLOAT:
    case PFLOAT_MAX_FLOAT:
    case PFLOAT_65:
        if (*SCInfo!=NULL)
            if (LocalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"LPFLOATCleanup",CaseNo,"LocalFree failed!");
            break;
    case PFLOAT_NULL:
    default:
        break;
    }
}
