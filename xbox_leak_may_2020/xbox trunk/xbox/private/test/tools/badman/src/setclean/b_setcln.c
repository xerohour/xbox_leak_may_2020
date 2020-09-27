/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    b_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'B'. It also contains setup and cleanup
    funtions beginning with 'LPB'. For more information, please refer
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
#include <B_Cases.bmh>


/************************
BBBB   OOO   OOO  L
B   B O   O O   O L
BBBB  O   O O   O L
B   B O   O O   O L
BBBB   OOO   OOO  LLLLL
************************/

BOOL BOOLSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    BOOL Ret;

    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hConOut );
    UNREFERENCED_PARAMETER( hLog );
    
    switch(CaseNo) {
    case BOOL_TRUE:
        Ret = TRUE;
        break;
    case BOOL_FALSE:
        Ret = FALSE;
        break;
    default:
        ErrorPrint(hConOut,hLog,"BOOLSetup",CaseNo,"Unknown Case");
        Ret = (BOOL) NULL;
        break;
    }
    
    return(Ret);
}


void BOOLCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}



/************************
BBBB  Y   Y TTTTT EEEEE
B   B  Y Y    T   E
BBBB    Y     T   EEEE
B   B   Y     T   E
BBBB    Y     T   EEEEE
************************/

BYTE BYTESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    BYTE Ret;
    
    UNREFERENCED_PARAMETER( SCInfo );

    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"BYTESetup",CaseNo,"Unknown Case");
        Ret = (BYTE)0;
        break;
    }
    
    return(Ret);
}


void BYTECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}



/************************************************
L     PPPP  BBBB  IIIII TTTTT M   M   A   PPPP
L     P   P B   B   I     T   MM MM  A A  P   P
L     PPPP  BBBB    I     T   M M M AAAAA PPPP
L     P     B   B   I     T   M   M A   A P
LLLLL P     BBBB  IIIII   T   M   M A   A P
************************************************/

PVOID LPBITMAPSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPBITMAP Ret;
    
    switch(CaseNo) {
    case LPBITMAP_NULL:
        Ret     = NULL;
        *SCInfo = ( LPVOID ) Ret;
        break;
        
    case LPBITMAP_VALID:
        if ( NULL != (Ret = LocalAlloc(LPTR,sizeof(BITMAP))) ) {
            Ret->bmType       = 0;
            Ret->bmWidth      = 32;
            Ret->bmHeight     = Ret->bmWidth;
            Ret->bmWidthBytes = Ret->bmWidth / 8;
            Ret->bmPlanes     = 1;
            Ret->bmBitsPixel  = 1;
            Ret->bmBits       = NULL;
        }  //  if ( Ret = LocalAlloc(LPTR,sizeof(BITMAP)) )
        
        *SCInfo = ( LPVOID ) Ret;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPBITMAPSetup",CaseNo,"Unknown Case");
        Ret = (LPBITMAP) NULL;
        break;
    }
    
    return(Ret);
#endif
    return 0;
}


void LPBITMAPCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );

    switch(CaseNo) {
    case LPBITMAP_NULL:
        break;
        
    case LPBITMAP_VALID:
        LocalFree ( (HGLOBAL) *SCInfo );
        break;
    default:
        break;
    }
}



/************************************************************************
L     PPPP  BBBB  IIIII TTTTT M   M   A   PPPP  IIIII N   N FFFFF  OOO
L     P   P B   B   I     T   MM MM  A A  P   P   I   NN  N F     O   O
L     PPPP  BBBB    I     T   M M M AAAAA PPPP    I   N N N FFFF  O   O
L     P     B   B   I     T   M   M A   A P       I   N  NN F     O   O
LLLLL P     BBBB  IIIII   T   M   M A   A P     IIIII N   N F      OOO
************************************************************************/

PVOID LPBITMAPINFOSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPBITMAPINFO Ret;
    
    switch(CaseNo) {
        
    case LPBITMAPINFO_NULL:
        Ret = (LPBITMAPINFO) NULL;
        *SCInfo = (LPVOID) Ret;
        break;
        
    case LPBITMAPINFO_MINUS_ONE:
        Ret = (LPBITMAPINFO) -1;
        *SCInfo = (LPVOID) Ret;
        break;
        
    case LPBITMAPINFO_VALID:
        Ret = (LPBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFO));
        if(!Ret) {
            ErrorPrint(hConOut,hLog,"LPBITMAPINFOSetup",CaseNo, "Insufficient Memory");
            Ret = (LPBITMAPINFO)NULL;
        }
        
        *SCInfo = (LPVOID) Ret;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPBITMAPINFOSetup",CaseNo,"Unknown Case");
        Ret = (LPBITMAPINFO) NULL;
        break;
    }
    
    return(Ret);
#endif
    return 0;
}


void LPBITMAPINFOCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );

    switch(CaseNo) {
        
    case LPBITMAPINFO_VALID:
        if(*SCInfo)
            LocalFree(*SCInfo);
        break;
        
    default:
        break;
    }
}



/******************************************************************************
L     PPPP  BBBB  IIIII TTTTT M   M   A   PPPP  IIIII N   N FFFFF  OOO  H   H
L     P   P B   B   I     T   MM MM  A A  P   P   I   NN  N F     O   O H   H
L     PPPP  BBBB    I     T   M M M AAAAA PPPP    I   N N N FFFF  O   O HHHHH
L     P     B   B   I     T   M   M A   A P       I   N  NN F     O   O H   H
LLLLL P     BBBB  IIIII   T   M   M A   A P     IIIII N   N F      OOO  H   H
******************************************************************************/

PVOID LPBITMAPINFOHEADERSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPBITMAPINFOHEADER Ret;
    
    switch(CaseNo) {
        
    case LPBITMAPINFOHEADER_NULL:
        Ret = (LPBITMAPINFOHEADER) NULL;
        *SCInfo = (LPVOID) Ret;
        break;
        
    case LPBITMAPINFOHEADER_MINUS_ONE:
        Ret = (LPBITMAPINFOHEADER) -1;
        *SCInfo = (LPVOID) Ret;
        break;
        
    case LPBITMAPINFOHEADER_VALID:
        Ret = (LPBITMAPINFOHEADER) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
        if(!Ret) {
            ErrorPrint(hConOut,hLog,"LPBITMAPINFOHEADERSetup",CaseNo, "Insufficient Memory");
            Ret = (LPBITMAPINFOHEADER)NULL;
        }
        
        *SCInfo = (LPVOID) Ret;
        break;
        
        
    default:
        ErrorPrint(hConOut,hLog,"LPBITMAPINFOHEADERSetup",CaseNo,"Unknown Case");
        Ret = (LPBITMAPINFOHEADER) NULL;
        *SCInfo = (LPVOID) Ret;
        break;
    }
    
    return(Ret);
#endif

    return 0;
}


void LPBITMAPINFOHEADERCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );

    switch(CaseNo) {
        
    case LPBITMAPINFOHEADER_VALID:
        if(*SCInfo)
            LocalFree(*SCInfo);
        break;
        
    default:
        break;
    }
}



/************************************
L     PPPP  BBBB   OOO   OOO  L
L     P   P B   B O   O O   O L
L     PPPP  BBBB  O   O O   O L
L     P     B   B O   O O   O L
LLLLL P     BBBB   OOO   OOO  LLLLL
************************************/

LPBOOL LPBOOLSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPBOOL Ret;
    BOOL *bPresent;
    
    switch(CaseNo) {
        
    case LPBOOL_VALID_BUFFER:
        bPresent = (BOOL *) LocalAlloc(LPTR, sizeof(BOOL));
        if(!bPresent) {
            ErrorPrint(hConOut,hLog,"LPBOOLSetup",CaseNo, "Insufficient Memory");
            Ret = (BOOL)NULL;
        }
        else {
            *SCInfo = bPresent;
            Ret = bPresent;
        }
        break;
    case LPBOOL_NULL:
        Ret = (LPBOOL)NULL;
        break;
        
    case LPBOOL_INVALID: {
        SE_CLEANUP_INFO		*CleanupInfo = NULL;
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            INVALID_POINTER,
            0,
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPBOOLSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                         }
        
    case LPBOOL_MALALIGNED: {
        SE_CLEANUP_INFO		*CleanupInfo = NULL;
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            MALALIGNED_POINTER_1,
            sizeof (BOOL),
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPBOOLSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                            }
        
    default:
        ErrorPrint(hConOut,hLog,"LPBOOLSetup",CaseNo,"Unknown Case");
        Ret = (LPBOOL) NULL;
        break;
    }
    
    return(Ret);
}


void LPBOOLCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPBOOL_VALID_BUFFER:
        if(*SCInfo)
            LocalFree(*SCInfo);
        break;
        
    case LPBOOL_INVALID:
    case LPBOOL_MALALIGNED: {
        SE_CLEANUP_INFO		*CleanupInfo = NULL;
        
        MakeBadPointer(NULL,
            (SE_CLEANUP_INFO **)SCInfo,
            CLEANUP,
            0,
            hConOut,
            hLog);
        
        if (*SCInfo) {
            ErrorPrint(hConOut, hLog, "LPBOOLCleanup", CaseNo,
                "Cleanup failed.");
        }
        
        break;
                            }
        
    default:
        break;
    }
}


/************************************
L     PPPP  BBBB  Y   Y TTTTT EEEEE
L     P   P B   B  Y Y    T   E
L     PPPP  BBBB    Y     T   EEEE
L     P     B   B   Y     T   E
LLLLL P     BBBB    Y     T   EEEEE
************************************/

LPBYTE LPBYTESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPBYTE Ret,lpbTemp;
    int i;
    
    switch(CaseNo) {
    case LPBYTE_13_PTS:
        
        Ret = (LPBYTE)GlobalAlloc(GPTR, 13 * sizeof (BYTE));
        if (Ret){
            lpbTemp= Ret;
            for (i = 0; i < 13; i++)
                *lpbTemp++ = 0x02;
        } else {
            ErrorPrint(hConOut,hLog,"LPBYTESetup",CaseNo,"allocation failed");
        }  
        *SCInfo = (LPVOID) Ret;
        break;
        
    case LPBYTE_BAD_13_PTS:
        
        // alloc 13 bytes set to 0 (not PT_*)      
        Ret = (LPBYTE)GlobalAlloc(GPTR, 13 * sizeof (BYTE));
        if (!Ret){
            ErrorPrint(hConOut,hLog,"LPBYTESetup",CaseNo,"allocation failed");
        }  
        *SCInfo = (LPVOID) Ret;
        break;
        
    case LPBYTE_NULL:
        Ret = (LPBYTE)NULL;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPBYTESetup",CaseNo,"Unknown Case");
        Ret = (LPBYTE) NULL;
        break;
    }
    
    return(Ret);
}


void LPBYTECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPBYTE_13_PTS:
    case LPBYTE_BAD_13_PTS:
        if (SCInfo != NULL) 
            if (GlobalFree((HANDLE)*SCInfo) != NULL) 
                ErrorPrint(hConOut,hLog,"LPBYTECleanup",CaseNo,"Couldn't free");
            break;
    default:
        break;
    }
}
