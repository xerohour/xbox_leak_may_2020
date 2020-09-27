/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    r_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'R'. It also contains setup and cleanup
    funtions beginning with 'LPR'. For more information, please refer
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
#include <r_cases.bmh>
#include <wtypes.h>
#include "limits.h"


#define SetRect( rc, l, t, r, b ) \
            (rc)->left   = l; \
            (rc)->top    = t; \
            (rc)->right  = r; \
            (rc)->bottom = b


LPRECT LPRECTSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPRECT		Ret = NULL;
    HANDLE		hMem;
    
    switch(CaseNo)
    {
    case LPRECT_NULL:
        //Ret = (LPRECT) NULL; 
        //don't need to do anything
        break;
        
    case LPRECT_RANDOM:
        //don't use this one for regressions
        Ret = (LPRECT)rand();
        break;
        
    case LPRECT_MINUS_ONE:
        Ret = (LPRECT)-1;
        break;
        
    default:
        if ((hMem = LocalAlloc( LPTR, sizeof(RECT))) != NULL)
        {
            if ((Ret = (LPRECT)LocalLock( hMem )) != NULL)
            {
                switch(CaseNo)
                {
                case LPRECT_0_0_100_100:
                    SetRect( Ret, 0, 0, 100, 100 ) ;
                    break;
                    
                case LPRECT_100_50_150_100:
                    SetRect( Ret, 100, 50, 150, 100 ) ;
                    break;
                    
                case LPRECT_20_20_40_40:
                    SetRect( Ret, 20, 20, 40, 40 ) ;
                    break;
                    
                case LPRECT_MAX:
                    SetRect( Ret,INT_MIN, INT_MIN, INT_MAX, INT_MAX ) ;
                    break;
                    
                case LPRECT_MAXRGN:
                    SetRect( Ret,0xf8000000,0xf8000000,0x08000000, 0x08000000);
                    break;
                    
                case LPRECT_NOT_WELL_ORDERED:
                    SetRect( Ret, 100, 0, 0, 100 ) ;
                    break;
                    
                case LPRECT_EMPTY:
                    SetRect( Ret, 200, 200, 200, 200 ) ;
                    break;
                    
                default:
                    ErrorPrint(hConOut,hLog,"LPRECTSetup",CaseNo,"Unknown Case");
                    break;
                }
            } else {
                ErrorPrint(hConOut,hLog,"LPRECTSetup",CaseNo,"LocalLock Failed");
                LocalFree(hMem);
                return NULL;
            }
        } else {
            ErrorPrint(hConOut,hLog,"LPRECTSetup",CaseNo,"LocalAlloc Failed");
            return NULL;
        }
        break;
    }
    
    //assign the return val to the storage pointer
    *SCInfo = (LPVOID)Ret;
    
    return(Ret);
}


void LPRECTCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo)
    {
    case LPRECT_NULL:
    case LPRECT_RANDOM:
    case LPRECT_MINUS_ONE:
        break ;
        
    case LPRECT_0_0_100_100:
    case LPRECT_100_50_150_100:
    case LPRECT_20_20_40_40:
    case LPRECT_NOT_WELL_ORDERED:
    case LPRECT_EMPTY:
    case LPRECT_MAX:
    case LPRECT_MAXRGN:
        if (*SCInfo!=NULL)
            if (LocalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"LPRECTCleanup",CaseNo,"LocalFree failed!");
            break ;
            
    default:
        ErrorPrint(hConOut,hLog,"LPRECTCleanup",CaseNo,"Unknown Case");
        break;
    }
}


PVOID LPRGBQUADSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
#if 0
    LPRGBQUAD Ret, lpPtr;
    int       i;
    
    switch(CaseNo) {
    case LPRGBQUAD_NULL:
        Ret = (LPRGBQUAD)NULL;
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPRGBQUAD_RANDOM:
        Ret = (LPRGBQUAD)rand();
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPRGBQUAD_MINUS_ONE:
        Ret = (LPRGBQUAD)-1;
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPRGBQUAD_VALID:
        Ret = (LPRGBQUAD)GlobalAlloc(GPTR, sizeof(RGBQUAD));
        if (!Ret)
            ErrorPrint(hConOut,hLog,"LPRGBQUADSetup",CaseNo,"Alloc failed!");
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPRGBQUAD_SIMPLE:  // this actually generates a complete rgndata structure
        Ret = (LPRGBQUAD)GlobalAlloc(GPTR, 300 * sizeof(RGBQUAD));
        if (!Ret){
            ErrorPrint(hConOut,hLog,"LPRGBQUADSetup",CaseNo,"Alloc failed!");
        } else {
            lpPtr = Ret;
            for (i=0; i<4; i++) {
                lpPtr->rgbBlue   = 0xA;
                lpPtr->rgbRed    = 0x2;
                lpPtr->rgbGreen  = 0xC;
                lpPtr++;
            }
        }
        *SCInfo = (LPVOID)Ret;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPRGBQUADSetup",CaseNo,"Unknown Case");
        Ret = (LPRGBQUAD ) NULL;
        break;
    }
    
    return(Ret);
#endif
    return 0;
}



void LPRGBQUADCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    switch(CaseNo) {
    case LPRGBQUAD_SIMPLE:
    case LPRGBQUAD_VALID:
        if (*SCInfo!=NULL)
            if (GlobalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"LPRGBQUADCleanup",CaseNo,"GlobalFree failed!");
            break;
    default:
        break;
    }
}
