/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    l_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'L'. It also contains setup and cleanup
    funtions beginning with 'LPL'. For more information, please refer
    to BadMan.Doc.

Author:

    John Miller (johnmil) 02-Feb-1992

Environment:

    XBox

Revision History:

    01-Apr-2000     schanbai

        Ported to XBox and removed not needed cases

--*/


#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <setcln.h>
#include <l_cases.bmh>


LONG LONGSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LONG      Ret = 0;
    LPSTR     lpstrPtr;
    LPWSTR    lpwstrPtr;
    
    switch(CaseNo) {
        
    case LONG_ONE_K:
        Ret = 1024;
        break;
    case LONG_MINUS_ONE:
        Ret = -1;
        break;
        
    case LONG_VALID_STRINGA:
        lpstrPtr = (LPSTR)GlobalAlloc(GPTR, strlen("Test String"));
        if (lpstrPtr){
            strcpy(lpstrPtr, "Test String");
            Ret = (LONG)lpstrPtr;
        } else {
            ErrorPrint(hConOut,hLog,"LONGSetup",CaseNo,"GlobalAlloc failed");
        }
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LONG_VALID_STRINGW:
        lpwstrPtr = (LPWSTR)GlobalAlloc(GPTR,
            lstrlenW((LPCWSTR)TEXT("Test String"))*sizeof(WCHAR));
        if (lpwstrPtr){
            lstrcpyW(lpwstrPtr, (LPCWSTR)TEXT("Test String"));
            Ret = (LONG)lpwstrPtr;
        } else {
            ErrorPrint(hConOut,hLog,"LONGSetup",CaseNo,"GlobalAlloc failed");
        }
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LONG_ZERO:
        Ret=0;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LONGSetup",CaseNo,"Unknown Case");
        Ret = (LONG) NULL;
        break;
    }
    
    return(Ret);
}


void LONGCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LONG_VALID_STRINGA:
    case LONG_VALID_STRINGW:
        if (*SCInfo!=NULL)
            if (GlobalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"LONGCleanup",CaseNo,"GlobalFree failed!");
            break;
    default:
        break;
    }
}


PVOID LPLOGPALETTESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPLOGPALETTE   Ret;
    LPPALETTEENTRY lpPtr;
    
    switch(CaseNo) {
    case LPLOGPALETTE_NULL:
        Ret = (LPLOGPALETTE)NULL;
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPLOGPALETTE_VALID:
        Ret = (LPLOGPALETTE) GlobalAlloc(GPTR, sizeof(LOGPALETTE));
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"LPLOGPALETTESetup",CaseNo,"Alloc failed!");
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPLOGPALETTE_ONE:
        Ret = (LPLOGPALETTE) GlobalAlloc(GPTR, sizeof(LOGPALETTE));
        if (Ret == NULL){
            ErrorPrint(hConOut,hLog,"LPLOGPALETTESetup",CaseNo,"Alloc failed!");
        } else {
            Ret->palVersion     = 0x300;
            Ret->palNumEntries  = 1;
            lpPtr = Ret->palPalEntry;
            if ( NULL != (lpPtr = (LPPALETTEENTRY)GlobalAlloc(GPTR, sizeof(PALETTEENTRY))) ) {
                lpPtr->peRed   = 1;
                lpPtr->peGreen = 2;
                lpPtr->peBlue  = 3;
                lpPtr->peFlags = (BYTE)0;
            } else {
                ErrorPrint(hConOut,hLog,"LPLOGPALETTESetup",CaseNo,"Alloc failed!");
            }
        }
        *SCInfo = (LPVOID)Ret;
        break ;
        
    default:
        ErrorPrint(hConOut,hLog,"LPLOGPALETTESetup",CaseNo,"Unknown Case");
        Ret = (LPLOGPALETTE) NULL;
        break;
    }
    
    return(Ret);
#endif
    return 0;
}


void LPLOGPALETTECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPLOGPALETTE lpPtr = NULL;
    
    switch(CaseNo) {
    case LPLOGPALETTE_VALID:
        if (*SCInfo!=NULL)
            if (GlobalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"LPLOGPALETTECleanup",CaseNo,"Free failed!");
            break;
    case LPLOGPALETTE_ONE:
        if (*SCInfo!=NULL)
            lpPtr = (LPLOGPALETTE)(*SCInfo);
        if (GlobalFree( (HANDLE)(lpPtr->palPalEntry))!=NULL)
            ErrorPrint(hConOut,hLog,"LPLOGPALETTECleanup",CaseNo,"Free PalEntry failed!");
        if (GlobalFree( (HANDLE)(*SCInfo) )!=NULL)
            ErrorPrint(hConOut,hLog,"LPLOGPALETTECleanup",CaseNo,"Free failed!");
    default:
        break;
    }
#endif
}


LPLONG LPLONGSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPLONG Ret;

    UNREFERENCED_PARAMETER( SCInfo );
    
    switch(CaseNo) {
        
    case LPLONG_NULL:
        Ret = NULL;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPLONGSetup",CaseNo,"Unknown Case");
        Ret = (LPLONG) NULL;
        break;
    }
    
    return(Ret);
}


void LPLONGCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}
