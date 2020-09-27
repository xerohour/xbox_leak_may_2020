/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    c_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'C'. It also contains setup and cleanup
    funtions beginning with 'LPC'. For more information, please refer
    to badman.doc.

Author:

    John Miller (johnmil) 02-Feb-1992

Environment:

    XBox

Revision History:

    17-Apr-2000     schanbai

        Ported to XBox and made the code more readable

--*/


#include <stdlib.h>
#include <setcln.h>
#include <c_cases.bmh>


//
// CHAR type
//

CHAR
CHARSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    CHAR Ret;

    UNREFERENCED_PARAMETER( SCInfo );

    switch ( CaseNo ) {
    case CHAR_A:
        Ret = 'A';
        break;

    default:
        ErrorPrint( hConOut, hLog, "CHARSetup", CaseNo, "Unknown Case" );
        Ret = 0;
    }

    return Ret;
}


void
CHARCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


//
// char type
//

char
charSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    char Ret;

    switch ( CaseNo ) {
        
    default:
        ErrorPrint( hConOut, hLog, "charSetup", CaseNo, "Unknown Case" );
        Ret = (char)0;
    }

    return Ret;
    
    UNREFERENCED_PARAMETER( SCInfo );
}


void
charCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


//
// COLORREF type
//

COLORREF
COLORREFSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    COLORREF Ret;

    switch (CaseNo) {

    case COLORREF_BLUE:
        Ret=0x00ff0000;
        break;

    case COLORREF_RED:
        Ret=0x000000ff;
        break;

    case COLORREF_GREEN:
        Ret=0x0000ff00;
        break;

    case COLORREF_BAD_TOP:
        Ret=0xff000000;
        break;

    case COLORREF_BLACK:
        Ret=0x00ffffff;
        break;

    case COLORREF_WHITE:
        Ret=0x00000000;
        break;

    default:
        ErrorPrint( hConOut, hLog, "COLORREFSetup", CaseNo, "Unknown Case" );
        Ret = (COLORREF)0;
    }

    return Ret;
    
    UNREFERENCED_PARAMETER( SCInfo );
}


void
COLORREFCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


PVOID
COORDSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
#if 0
    COORD Ret = { 0 };
    
    switch (CaseNo) {

    default:
        ErrorPrint( hConOut, hLog, "COORDSetup", CaseNo, "Unknown Case" );
    }

    return Ret;

    UNREFERENCED_PARAMETER( SCInfo );
#endif
    return 0;
}


void
COORDCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


PVOID
LPCOMMTIMEOUTSSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
#if 0
    LPCOMMTIMEOUTS Ret;

    switch (CaseNo) {

    default:
        ErrorPrint( hConOut, hLog, "LPCOMMTIMEOUTSSetup", CaseNo, "Unknown Case" );
        Ret = (LPCOMMTIMEOUTS)NULL;
    }

    return Ret;

    UNREFERENCED_PARAMETER( SCInfo );
#endif
    return 0;
}


void
LPCOMMTIMEOUTSCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


PVOID
LPCOMSTATSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
#if 0
    LPCOMSTAT Ret;

    switch (CaseNo) {
        
    default:
        ErrorPrint( hConOut, hLog, "LPCOMSTATSetup", CaseNo, "Unknown Case" );
        Ret = (LPCOMSTAT)NULL;
    }

    return Ret;

    UNREFERENCED_PARAMETER( SCInfo );
#endif
    return 0;
}


void
LPCOMSTATCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}


LPCONTEXT
LPCONTEXTSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    LPCONTEXT Ret;
    
    switch (CaseNo) {
    case LPCONTEXT_VALID_STRUCTURE:
        Ret = (LPCONTEXT)LocalAlloc( LPTR, sizeof(CONTEXT) );

        if ( Ret == NULL ) {
            ErrorPrint( hConOut, hLog, "LPCONTEXTSetup", CaseNo, "Valid Structure => LocalAlloc failed!" );
        }
        else {
            Ret->ContextFlags = CONTEXT_FULL;
        }
        *SCInfo = (LPVOID)Ret;
        break;

    case LPCONTEXT_RANDOM_POINTER:
        Ret = (LPCONTEXT)rand();
        break;

    case LPCONTEXT_NULL_POINTER:
        Ret = (LPCONTEXT)NULL;
        break;

    case LPCONTEXT_BLOCK_TOO_SMALL:
        // BUGBUG: this will definitely corrupt the heap
        Ret = (LPCONTEXT)LocalAlloc( LPTR, sizeof(CONTEXT)/2 );
        if ( Ret == NULL ) {
            ErrorPrint( hConOut, hLog, "LPCONTEXTSetup", CaseNo, "Block Too Small => LocalAlloc failed!" );
        }
        *SCInfo = (LPVOID)Ret;
        break;

    case LPCONTEXT_INVALID_FLAGS:
        Ret = (LPCONTEXT)LocalAlloc( LPTR, sizeof(CONTEXT) );
        if ( Ret == NULL ) {
            ErrorPrint( hConOut, hLog, "LPCONTEXTSetup", CaseNo, "Valid Structure => LocalAlloc failed!" );
        }
        else {
            Ret->ContextFlags = 0;
        }
        *SCInfo = (LPVOID)Ret;
        break;

    default:
        ErrorPrint( hConOut, hLog, "LPCONTEXTSetup", CaseNo, "Unknown Case" );
        Ret = (LPCONTEXT)NULL;
    }

    return Ret;
}


void
LPCONTEXTCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    switch (CaseNo) {
    case LPCONTEXT_VALID_STRUCTURE:
    case LPCONTEXT_BLOCK_TOO_SMALL:
    case LPCONTEXT_INVALID_FLAGS:
        if ( LocalFree( (HANDLE)(*SCInfo) ) != NULL ) {
            ErrorPrint( hConOut, hLog, "LPCONTEXTCleanup", CaseNo, "LocalFree failed!" );
        }
        break;
    }
}


LPCRITICAL_SECTION
LPCRITICAL_SECTIONSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    LPCRITICAL_SECTION Ret;
    
    switch (CaseNo) {

    case LPCRITICAL_SECTION_NULL:
        Ret = (LPCRITICAL_SECTION)NULL;
        break;

    case LPCRITICAL_SECTION_INITIALIZED:
        Ret = LocalAlloc( LPTR, sizeof(CRITICAL_SECTION) );
        if ( Ret ) {
            InitializeCriticalSection( Ret );
        }
        break;

    case LPCRITICAL_SECTION_UNINITIALIZED:
        Ret = LocalAlloc( LPTR, sizeof(CRITICAL_SECTION) );
        break;

    case LPCRITICAL_SECTION_MINUS_ONE:
        Ret = (LPCRITICAL_SECTION)-1;
        break;

    default:
        ErrorPrint( hConOut, hLog, "LPCRITICAL_SECTIONSetup", CaseNo, "Unknown Case" );
        Ret = (LPCRITICAL_SECTION)NULL;
    }

    *SCInfo = (LPVOID)Ret;
    return Ret;
}


void
LPCRITICAL_SECTIONCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    LPCRITICAL_SECTION Ret = (LPCRITICAL_SECTION)*SCInfo;
    
    switch (CaseNo) {

    case LPCRITICAL_SECTION_NULL:
    case LPCRITICAL_SECTION_MINUS_ONE:
        break;

    case LPCRITICAL_SECTION_INITIALIZED:
        if ( Ret ) {
            DeleteCriticalSection( Ret );
        }
        // Fall thru
    case LPCRITICAL_SECTION_UNINITIALIZED:
        if ( NULL != LocalFree( Ret ) ) {
            ErrorPrint( hConOut, hLog, "LPCRITICAL_SECTIONCleanup", CaseNo, "Unable to free memory" );
        }
        break;

    default:
        ErrorPrint( hConOut, hLog, "LPCRITICAL_SECTIONSetup", CaseNo, "Unknown Case" );
        Ret = (LPCRITICAL_SECTION)NULL;
    }
}


LPCOLORREF
LPCOLORREFSetup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    COLORREF* Ret;

    switch (CaseNo) {
    case LPCOLORREF_NULL:
        Ret = (COLORREF *)NULL;
        break;

    case LPCOLORREF_MINUS_ONE:
        Ret = (COLORREF *)-1;
        break;

    case LPCOLORREF_VALID:
        if ((Ret = (COLORREF *)GlobalAlloc(GPTR, sizeof(COLORREF))) == NULL) {
            ErrorPrint(hConOut,hLog,"LPCOLORREFSetup",CaseNo,"alloc failed" );
        }
        *Ret=0x00F0F0F0; 
        *SCInfo = Ret;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPCOLORREFSetup",CaseNo,"Unknown Case");
        Ret = (COLORREF *)NULL;
    }
    
    return Ret;
}


void
LPCOLORREFCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    switch (CaseNo) {
    case LPCOLORREF_VALID:
        if (GlobalFree( (HANDLE)(*SCInfo) ) != NULL) {
            ErrorPrint(hConOut,hLog,"LPCOLORREFSetup",CaseNo,"GlobalFree failed!" );
        }
        break;
        
    default:
        break;
    }
}
