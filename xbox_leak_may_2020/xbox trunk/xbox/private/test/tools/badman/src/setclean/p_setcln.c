/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    p_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'P'. It also contains setup and cleanup
    funtions beginning with 'LPP'. For more information, please refer
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
#include <a_Cases.bmh>
#include <p_Cases.bmh>
#include "limits.h"


LPPOINT LPPOINTSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPPOINT Ret, lpptTemp;
    int i;
    
    switch(CaseNo) {
    case LPPOINT_13_POINTS:
        Ret = (LPPOINT)GlobalAlloc(GPTR, 13 * sizeof(POINT));
        if (Ret != 0) {
            lpptTemp = Ret;
            for (i = 0; i < 13; i++){
                lpptTemp->x = (int)rand();
                lpptTemp->y = (int)rand();
                lpptTemp++;
            }
        } else {
            ErrorPrint(hConOut,hLog,"LPPPOINTSetup",CaseNo,"GlobalAlloc failed!" );
        }
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPPOINT_NULL:
        Ret = (LPPOINT)NULL;
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPPOINT_RANDOM:
        Ret = (LPPOINT)rand();
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPPOINT_MINUS_ONE:
        Ret = (LPPOINT)-1;
        *SCInfo = (LPVOID)Ret;
        break;
        
    case LPPOINT_VALID:
        Ret = (LPPOINT)GlobalAlloc(GPTR, sizeof(POINT));
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"LPPPOINTSetup",CaseNo,"GlobalAlloc failed!" );
        *SCInfo = (LPVOID)Ret;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPPOINTSetup",CaseNo,"Unknown Case");
        Ret = (LPPOINT) NULL;
        break;
    }
    
    return(Ret);
    
}


void LPPOINTCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPPOINT_VALID:
    case LPPOINT_13_POINTS:
        if (SCInfo != NULL) {
            if (GlobalFree ((HANDLE)*SCInfo) != NULL) {
                ErrorPrint(hConOut,hLog,"LPPOINTCleanup",CaseNo,"GlobalFree() failed!");
            }
        }
        break;
        
    default:
        break;
    }
}


/******************************************************************************
L     PPPP  PPPP  RRRR   OOO   CCC  EEEEE  SSSS  SSSS       IIIII N   N FFFFF
L     P   P P   P R   R O   O C   C E     S     S             I   NN  N F
L     PPPP  PPPP  RRRR  O   O C     EEEE   SSS   SSS          I   N N N FFFF
L     P     P     R  R  O   O C   C E         S     S         I   N  NN F
LLLLL P     P     R   R  OOO   CCC  EEEEE SSSS  SSSS  _____ IIIII N   N F
******************************************************************************/

PVOID LPPROCESS_INFORMATIONSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPPROCESS_INFORMATION lpRet;
    
    switch(CaseNo) {
        
    case LPPROCESS_INFORMATION_VALID_STRUCTURE:
        lpRet = (LPPROCESS_INFORMATION)LocalAlloc( LPTR, sizeof(PROCESS_INFORMATION) );
        if (lpRet==NULL)
            ErrorPrint(hConOut,hLog,"ProcInfoSetup",CaseNo,"LocalAlloc for ProcInfo strucutre failed" );
        *SCInfo = (LPVOID)lpRet;
        break;
        
    case LPPROCESS_INFORMATION_NULL:
        lpRet = (LPPROCESS_INFORMATION) NULL;
        break;
        
    case LPPROCESS_INFORMATION_TOO_SMALL:
        lpRet = (LPPROCESS_INFORMATION)LocalAlloc( LPTR, (sizeof(PROCESS_INFORMATION)-4) );
        if (lpRet==NULL)
            ErrorPrint(hConOut,hLog,"ProcInfoSetup",CaseNo,"LocalAlloc for too small ProcInfo structure failed" );
        *SCInfo = (LPVOID)lpRet;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"ProcInfoSetup",CaseNo,"Unknown Case");
        lpRet = (LPPROCESS_INFORMATION) NULL;
        break;
    }
    
    return(lpRet);
#endif
    return 0;
}


void LPPROCESS_INFORMATIONCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0    
    LPPROCESS_INFORMATION lppi;
    
    switch(CaseNo) {
    case LPPROCESS_INFORMATION_VALID_STRUCTURE:
        lppi = (LPPROCESS_INFORMATION)(*SCInfo);
        if (lppi!=NULL) {
            
            if (lppi->hThread!=NULL)
                if (!CloseHandle( lppi->hThread ))
                    ErrorPrint(hConOut,hLog,"ProcInfoCleanup",CaseNo,"Unable to close thread handle");
                
                if (lppi->hProcess!=NULL)
                    if (lppi->hProcess!=NULL)
                        if (!CloseHandle( lppi->hProcess ))
                            ErrorPrint(hConOut,hLog,"ProcInfoCleanup",CaseNo,"Unable to close process handle");
                        
                        if (LocalFree( (HANDLE)lppi )!=NULL)
                            ErrorPrint(hConOut,hLog,"ProcInfoCleanup",CaseNo,"LocalFree failed");
                        
        }
        break;
        
    case LPPROCESS_INFORMATION_TOO_SMALL:
        if (SCInfo!=NULL){
            if (LocalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"ProcInfoCleanup",CaseNo,"LocalFree Failed!");
        }
        break;
        
    default:
        break;
    }
#endif
}




/******************************
PPPP  BBBB  Y   Y TTTTT EEEEE
P   P B   B  Y Y    T   E
PPPP  BBBB    Y     T   EEEE
P     B   B   Y     T   E
P     BBBB    Y     T   EEEEE
******************************/

PBYTE PBYTESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    PBYTE Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PBYTESetup",CaseNo,"Unknown Case");
        Ret = (PBYTE) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
}


void PBYTECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/************************************************************
PPPP   CCC  H   H   A   RRRR        IIIII N   N FFFFF  OOO
P   P C   C H   H  A A  R   R         I   NN  N F     O   O
PPPP  C     HHHHH AAAAA RRRR          I   N N N FFFF  O   O
P     C   C H   H A   A R  R          I   N  NN F     O   O
P      CCC  H   H A   A R   R _____ IIIII N   N F      OOO
************************************************************/

PVOID PCHAR_INFOSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    PCHAR_INFO Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PCHAR_INFOSetup",CaseNo,"Unknown Case");
        Ret = (PCHAR_INFO) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
#endif
    return 0;
}


void PCHAR_INFOCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/******************************************************************************
PPPP   CCC   OOO  N   N  SSSS  OOO  L     EEEEE        CCC  U   U RRRR   SSSS
P   P C   C O   O NN  N S     O   O L     E           C   C U   U R   R S
PPPP  C     O   O N N N  SSS  O   O L     EEEE        C     U   U RRRR   SSS
P     C   C O   O N  NN     S O   O L     E           C   C U   U R  R      S
P      CCC   OOO  N   N SSSS   OOO  LLLLL EEEEE _____  CCC   UUU  R   R SSSS
******************************************************************************/

PVOID PCONSOLE_CURSOR_INFOSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    PCONSOLE_CURSOR_INFO Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PCONSOLE_CURSOR_INFOSetup",CaseNo,"Unknown Case");
        Ret = (PCONSOLE_CURSOR_INFO) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
#endif
    return 0;
}


void PCONSOLE_CURSOR_INFOCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}




/******************************************************************************
PPPP   CCC   OOO  N   N  SSSS  OOO  L     EEEEE        SSSS  CCC  RRRR  EEEEE
P   P C   C O   O NN  N S     O   O L     E           S     C   C R   R E
PPPP  C     O   O N N N  SSS  O   O L     EEEE         SSS  C     RRRR  EEEE
P     C   C O   O N  NN     S O   O L     E               S C   C R  R  E
P      CCC   OOO  N   N SSSS   OOO  LLLLL EEEEE _____ SSSS   CCC  R   R EEEEE
******************************************************************************/

PVOID PCONSOLE_SCREEN_BUFFER_INFOSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    PCONSOLE_SCREEN_BUFFER_INFO Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PCONSOLE_SCREEN_BUFFER_INFOSetup",CaseNo,"Unknown Case");
        Ret = (PCONSOLE_SCREEN_BUFFER_INFO) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
#endif
    return 0;
}


void PCONSOLE_SCREEN_BUFFER_INFOCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/************************************
PPPP  DDD   W   W  OOO  RRRR  DDD
P   P D  D  W   W O   O R   R D  D
PPPP  D   D W W W O   O RRRR  D   D
P     D   D WW WW O   O R  R  D   D
P     DDDD  W   W  OOO  R   R DDDD
************************************/

PDWORD PDWORDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    PDWORD pdRet;
    
    *SCInfo = (PDWORD)NULL;
    
    switch(CaseNo) {
    case PDWORD_VALID_0:
        pdRet = (PDWORD) LocalAlloc(LPTR,sizeof(DWORD));
        if (pdRet == NULL) {
            ErrorPrint(hConOut,hLog,"PDWORDSetup",CaseNo,
                "Unable to allocate DWORD");
        }
        else {
            *pdRet = 0;
        }
        *SCInfo = pdRet;
        break;
        
    case PDWORD_BUFFER:
        pdRet = (PDWORD) LocalAlloc(LPTR,sizeof(DWORD));
        if (pdRet == NULL) {
            ErrorPrint(hConOut,hLog,"PDWORDSetup",CaseNo,
                "Unable to allocate DWORD");
            break;
        }
        *SCInfo = pdRet;
        break;
        
    case PDWORD_ACCESSMASK_GENREAD:
        pdRet = (PDWORD) LocalAlloc(LPTR,sizeof(DWORD));
        if (pdRet == NULL) {
            ErrorPrint(hConOut,hLog,"PDWORDSetup",CaseNo,
                "Unable to allocate DWORD");
        }
        else {
            *pdRet = GENERIC_READ;
        }
        *SCInfo = pdRet;
        break;
        
    case PDWORD_ACCESSMASK_GENALL:
        pdRet = (PDWORD) LocalAlloc(LPTR,sizeof(DWORD));
        if (pdRet == NULL) {
            ErrorPrint(hConOut,hLog,"PDWORDSetup",CaseNo,
                "Unable to allocate DWORD");
        }
        else {
            *pdRet = GENERIC_ALL;
        }
        *SCInfo = pdRet;
        break;
        
    case PDWORD_NULL:
        pdRet = (PDWORD) NULL;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"PDWORDSetup",CaseNo,"Unknown Case");
        pdRet = (PDWORD) NULL;
        break;
    }
    
    return(pdRet);
}


void PDWORDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case PDWORD_VALID_0:
    case PDWORD_BUFFER:
    case PDWORD_ACCESSMASK_GENREAD:
    case PDWORD_ACCESSMASK_GENALL:
        LocalFree(*SCInfo);
        break;
    default:
        break;
    }
    
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/****************************************************
PPPP  FFFFF IIIII L     EEEEE TTTTT IIIII M   M EEEEE
P   P F       I   L     E       T     I   MM MM E
PPPP  FFFF    I   L     EEEE    T     I   M M M EEEE
P     F       I   L     E       T     I   M   M E
P     F     IIIII LLLLL EEEEE   T   IIIII M   M EEEEE
****************************************************/

PFILETIME PFILETIMESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    PFILETIME Ret;
    
    switch(CaseNo) {
    case PFILETIME_NULL:
        Ret = NULL;
        break;
        
    case PFILETIME_VALID_BUFFER:
        Ret = malloc (sizeof (FILETIME));
        *SCInfo = Ret;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"PFILETIMESetup",CaseNo,"Unknown Case");
        Ret = NULL;
        break;
    }
    
    return(Ret);
}


void PFILETIMECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case PFILETIME_VALID_BUFFER:
        free (*SCInfo);
        break;
        
    default:
        break;
    }
    
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/******************************************
PPPP  H   H   A   N   N DDD   L     EEEEE
P   P H   H  A A  NN  N D  D  L     E
PPPP  HHHHH AAAAA N N N D   D L     EEEE
P     H   H A   A N  NN D   D L     E
P     H   H A   A N   N DDDD  LLLLL EEEEE
******************************************/

PHANDLE
PHANDLESetup(
             int         CaseNo,
             LPVOID          *SCInfo,
             HANDLE          hLog,
             HANDLE          hConOut
             )
{
/*
* Technically, we should never see "PHANDLE" in the win32 stuff; it
* should be replaced with LPHANDLE.  If that happens this code should
* be added to the LPHANDLE setup and munged as necessary to fit.
* TimF 10-Jun-92
    */
    
    PHANDLE pRet = NULL;
    
    switch (CaseNo) {
    case PHANDLE_VALID_TOKEN:
        pRet = (PHANDLE)LocalAlloc(LPTR, sizeof(HANDLE));
        
        if (!pRet) {
            ErrorPrint(hConOut,hLog, "PHANDLESetup",
                CaseNo, "insufficient memory");
        }
        
        break;
        
    case PHANDLE_NULL:
    /*
    * ignore, pRet was set to NULL at entry
        */
        
        break;
        
    case PHANDLE_INVALID:
    /*
    * Until there is decent invalid pointer generation,
    * we will use a hard-coded magic number:  evil
    * incarnate.
        */
        
        pRet = (PHANDLE)0x666;
        
        break;
        
    default:
        ErrorPrint(hConOut, hLog, "PHANDLESetup", CaseNo,
            "Unknown Case");
        
        break;
    }
    
    *SCInfo = pRet;
    
    return pRet;
}


void
PHANDLECleanup(
               int         CaseNo,
               LPVOID          *SCInfo,
               HANDLE          hLog,
               HANDLE          hConOut
               )
{
    switch (CaseNo) {
    case PHANDLE_VALID_TOKEN:
        if (*SCInfo)
            LocalFree(*SCInfo);
        
        break;
        
    case PHANDLE_NULL:
    case PHANDLE_INVALID:
    /*
    * ignore, unless there is bad-pointer stuff to clean
    * up.
        */
        
        break;
        
    default:
        ErrorPrint(hConOut, hLog, "PHANDLECleanup", CaseNo,
            "Unknown Case");
        
        break;
    }
}


/******************************************************************************
PPPP  H   H   A   N   N DDD   L     EEEEE RRRR        RRRR   OOO  U   U TTTTT
P   P H   H  A A  NN  N D  D  L     E     R   R       R   R O   O U   U   T
PPPP  HHHHH AAAAA N N N D   D L     EEEE  RRRR        RRRR  O   O U   U   T
P     H   H A   A N  NN D   D L     E     R  R        R  R  O   O U   U   T
P     H   H A   A N   N DDDD  LLLLL EEEEE R   R _____ R   R  OOO   UUU    T
******************************************************************************/

PVOID PHANDLER_ROUTINESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    PHANDLER_ROUTINE Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PHANDLER_ROUTINESetup",CaseNo,"Unknown Case");
        Ret = (PHANDLER_ROUTINE) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
#endif
    return 0;
}


void PHANDLER_ROUTINECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/******************************************************
PPPP  IIIII  CCC   OOO  N   N IIIII N   N FFFFF  OOO
P   P   I   C   C O   O NN  N   I   NN  N F     O   O
PPPP    I   C     O   O N N N   I   N N N FFFF  O   O
P       I   C   C O   O N  NN   I   N  NN F     O   O
P     IIIII  CCC   OOO  N   N IIIII N   N F      OOO
******************************************************/

PVOID PICONINFOSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    PICONINFO Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PICONINFOSetup",CaseNo,"Unknown Case");
        Ret = (PICONINFO) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
#endif
    return 0;
}


void PICONINFOCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/******************************************************************************
PPPP  IIIII N   N PPPP  U   U TTTTT       RRRR  EEEEE  CCC   OOO  RRRR  DDD
P   P   I   NN  N P   P U   U   T         R   R E     C   C O   O R   R D  D
PPPP    I   N N N PPPP  U   U   T         RRRR  EEEE  C     O   O RRRR  D   D
P       I   N  NN P     U   U   T         R  R  E     C   C O   O R  R  D   D
P     IIIII N   N P      UUU    T   _____ R   R EEEEE  CCC   OOO  R   R DDDD
******************************************************************************/

PVOID PINPUT_RECORDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    PINPUT_RECORD Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PINPUT_RECORDSetup",CaseNo,"Unknown Case");
        Ret = (PINPUT_RECORD) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
#endif
    return 0;
}


void PINPUT_RECORDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}


PLONG PLONGSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    PLONG Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PLONGSetup",CaseNo,"Unknown Case");
        Ret = (PLONG) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
}


void PLONGCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/******************************************************************************
PPPP  M   M EEEEE M   M  OOO  RRRR  Y   Y       BBBB    A    SSSS IIIII  CCC
P   P MM MM E     MM MM O   O R   R  Y Y        B   B  A A  S       I   C   C
PPPP  M M M EEEE  M M M O   O RRRR    Y         BBBB  AAAAA  SSS    I   C
P     M   M E     M   M O   O R  R    Y         B   B A   A     S   I   C   C
P     M   M EEEEE M   M  OOO  R   R   Y   _____ BBBB  A   A SSSS  IIIII  CCC
******************************************************************************/

PMEMORY_BASIC_INFORMATION
PMEMORY_BASIC_INFORMATIONSetup(
                               int CaseNo,
                               LPVOID *SCInfo,
                               HANDLE hLog,
                               HANDLE hConOut
                               )
{
    PMEMORY_BASIC_INFORMATION pRet;
    
    switch ( CaseNo ) {
        
    case PMEMORY_BASIC_INFORMATION_INVAL_BUFFER:
        pRet = (PMEMORY_BASIC_INFORMATION)NULL;
        break;
        
    case PMEMORY_BASIC_INFORMATION_2SMALL_BUFFER:
        pRet = VirtualAlloc( 0, 0x2000, MEM_RESERVE, PAGE_READWRITE );
        pRet = VirtualAlloc( pRet, 0x1000, MEM_COMMIT, PAGE_READWRITE );
        if (pRet == NULL) {
            ErrorPrint(hConOut, hLog, "PMEMORY_BASIC_INFORMATIONSetup", CaseNo, "Unable to allocate buffer");
        } else {
            *(PBYTE)pRet = 0;
            pRet = (PMEMORY_BASIC_INFORMATION) \
                ( (PBYTE)pRet + (0x1000 - (sizeof(MEMORY_BASIC_INFORMATION)/2)) );
        }
        *SCInfo = (LPVOID)pRet;
        break;
        
    case PMEMORY_BASIC_INFORMATION_BUFFER:
        pRet = VirtualAlloc( 0, sizeof(MEMORY_BASIC_INFORMATION), MEM_COMMIT, PAGE_READWRITE );
        if (pRet == NULL) {
            ErrorPrint(hConOut, hLog, "PMEMORY_BASIC_INFORMATIONSetup", CaseNo, "Unable to allocate buffer");
        } else {
            *(PBYTE)pRet = 0;
        }
        *SCInfo = (LPVOID)pRet;
        break;
        
    default:
        ErrorPrint(hConOut, hLog, "PMEMORY_BASIC_INFORMATIONSetup", CaseNo, "Unknown Case");
        pRet = (PMEMORY_BASIC_INFORMATION)NULL;
        break;
    }
    
    return pRet;
}


void
PMEMORY_BASIC_INFORMATIONCleanup(
                                 int CaseNo,
                                 LPVOID *SCInfo,
                                 HANDLE hLog,
                                 HANDLE hConOut
                                 )
{
    switch ( CaseNo ) {
        
    case PMEMORY_BASIC_INFORMATION_2SMALL_BUFFER:
    case PMEMORY_BASIC_INFORMATION_BUFFER:
        if ( *SCInfo ) {
            VirtualFree( *SCInfo, 0, MEM_RELEASE );
        }
        break;
        
    default:
        break;
    }
    
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/******************************
PPPP   OOO  IIIII N   N TTTTT
P   P O   O   I   NN  N   T
PPPP  O   O   I   N N N   T
P     O   O   I   N  NN   T
P      OOO  IIIII N   N   T
******************************/

POINT POINTSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    POINT Ret = {0};
    
    switch(CaseNo) {
        
    case POINT_ZERO:
        Ret.x = 0;
        Ret.y = 0;
        break;
        
    case POINT_100:
        Ret.x = 100;
        Ret.y = 100;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"POINTSetup",CaseNo,"Unknown Case");
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
}


void POINTCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}



/************************
PPPP  RRRR   OOO   CCC
P   P R   R O   O C   C
PPPP  RRRR  O   O C
P     R  R  O   O C   C
P     R   R  OOO   CCC
************************/

PROC PROCSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    PROC Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PROCSetup",CaseNo,"Unknown Case");
        Ret = (PROC) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
}


void PROCCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}


/******************************************************************************
PPPP   SSSS EEEEE  CCC  U   U RRRR  IIIII TTTTT Y   Y        CCC   OOO  N   N
P   P S     E     C   C U   U R   R   I     T    Y Y        C   C O   O NN  N
PPPP   SSS  EEEE  C     U   U RRRR    I     T     Y         C     O   O N N N
P         S E     C   C U   U R  R    I     T     Y         C   C O   O N  NN
P     SSSS  EEEEE  CCC   UUU  R   R IIIII   T     Y   _____  CCC   OOO  N   N
******************************************************************************/

PSECURITY_DESCRIPTOR_CONTROL PSECURITY_DESCRIPTOR_CONTROLSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    PSECURITY_DESCRIPTOR_CONTROL pRet;
    
    *SCInfo = (PSECURITY_DESCRIPTOR_CONTROL)NULL;
    
    switch(CaseNo) {
        
    case PSECURITY_DESCRIPTOR_CONTROL_VALID:
        pRet = (PSECURITY_DESCRIPTOR_CONTROL)
            LocalAlloc(LPTR, sizeof(SECURITY_DESCRIPTOR_CONTROL));
        
        if(!pRet) {
            ErrorPrint(hConOut,hLog, "PSECURITY_DESCRIPTOR_CONTROLSetup", CaseNo,
                "Insufficient memory");
            break;
        }
        *SCInfo = pRet;
        break;
        
    case PSECURITY_DESCRIPTOR_CONTROL_NULL:
        pRet = (PSECURITY_DESCRIPTOR_CONTROL)NULL;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"PSECURITY_DESCRIPTOR_CONTROLSetup",CaseNo,
            "Unknown Case");
        pRet = (PSECURITY_DESCRIPTOR_CONTROL) NULL;
        break;
    }
    
    return(pRet);
}


void PSECURITY_DESCRIPTOR_CONTROLCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case PSECURITY_DESCRIPTOR_CONTROL_VALID:
        if(*SCInfo)
            LocalFree(*SCInfo);
        break;
    default:
        break;
    }
    
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}

/******************************************************************************
PPPP   SSSS EEEEE  CCC  U   U RRRR  IIIII TTTTT Y   Y       IIIII N   N FFFFF
P   P S     E     C   C U   U R   R   I     T    Y Y          I   NN  N F
PPPP   SSS  EEEE  C     U   U RRRR    I     T     Y           I   N N N FFFF
P         S E     C   C U   U R  R    I     T     Y           I   N  NN F
P     SSSS  EEEEE  CCC   UUU  R   R IIIII   T     Y   _____ IIIII N   N F
******************************************************************************/

PSECURITY_INFORMATION PSECURITY_INFORMATIONSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    PSECURITY_INFORMATION Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"PSECURITY_INFORMATIONSetup",CaseNo,"Unknown Case");
        Ret = (PSECURITY_INFORMATION) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;   // -W3 warning elimination
}


void PSECURITY_INFORMATIONCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;   // -W3 warning elmination
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}


PVOID PVOIDSetup(
                 int                     CaseNo,
                 LPVOID                  *SCInfo,
                 HANDLE                  hLog,
                 HANDLE                  hConOut
                 )
{
    PVOID                   Ret;
    
    *SCInfo = (PVOID)NULL;
    
    switch(CaseNo) {
    case PVOID_INVALID: // pick an arb addr (which will probably be invalid
        
        Ret = (PVOID)0x12345678;
        
        break;
        
    case PVOID_NULL:
        
        Ret = (PVOID)NULL;
        
        break;
        
    case PVOID_VALID:
        
        Ret = (PVOID) LocalAlloc(LPTR, sizeof(WCHAR)*50);
        *SCInfo = (PVOID) Ret;
        break;
        
    default:
        
        ErrorPrint(hConOut,
            hLog,
            "PVOIDSetup",
            CaseNo,
            "Unknown Case");
        
        Ret = (PVOID)NULL;
        
        break;
    }
    
    *SCInfo = Ret;
    
    return Ret;
}


void PVOIDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case PVOID_VALID:
        if(*SCInfo)
            LocalFree(*SCInfo);
        break;
        
    default:
        break;
        
    }
    
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}

/***********************************
PPPP   PPPP  V   V  OOO  IIIII DDD
P   p  P   P V   V O   O   I   D  D
PPPP   PPPP  V   V O   O   I   D   D
P      P      V V  O   O   I   D   D
P    - P       V    OOO  IIIII DDDD
***********************************/


PVOID *P_PVOIDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    PVOID       *Ret;
    
    *SCInfo = (PVOID)NULL;
    
    switch(CaseNo) {
        
        
    case P_PVOID_NULL:
        Ret = (PVOID)NULL;
        break;
        
    case P_PVOID_VALID_NULL:
        if ( NULL != (Ret = (PVOID *)LocalAlloc(LPTR, sizeof(DWORD))) ) {
            *Ret = NULL;
        } else {
            ErrorPrint(hConOut, hLog, "P_PVOID_SETUP",
                CaseNo, "alloc failed\n");
        }
        *SCInfo = Ret;
        break;
        
    case P_PVOID_5K:
        if ((Ret = (PVOID *)LocalAlloc(LPTR, 5*1024)) == NULL) {
            ErrorPrint(hConOut, hLog, "P_PVOID_SETUP",
                CaseNo, "alloc failed\n");
        }
        *SCInfo = Ret;
        break;
        
    case P_PVOID_VALID_ADDRESS:
        
        Ret = (PVOID)LocalAlloc(LPTR, sizeof(DWORD));
        if(!Ret) {
            ErrorPrint(hConOut, hLog, "PVOID_VALID_ADDRESS",
                CaseNo, "Insufficient memory\n");
        }
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"PVOIDSetup",CaseNo,"Unknown Case");
        Ret = (PVOID) NULL;
        break;
    }
    
    *SCInfo = Ret;
    
    return(Ret);
}


void P_PVOIDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case P_PVOID_VALID_NULL:
    case P_PVOID_5K:
        if(*SCInfo)
            LocalFree(*SCInfo);
        break;
        
    default:
        break;
    }
    
    hLog;     // -W3 warning elmination
    hConOut;  // -W3 warning elmination
}


LPSTR *P_LPSTRSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    LPSTR *Ret;
    
    switch(CaseNo) {
        
    case P_LPSTR_VALID_ADDRESS:
        Ret = *SCInfo = (LPSTR *) LocalAlloc(LPTR, sizeof(LPSTR));
        if ( Ret == NULL )
            ErrorPrint(hConOut,hLog,"P_LPSTRSetup",CaseNo,"out of memory\n");
        break;
    case P_LPSTR_NULL:
        Ret = NULL;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"P_LPSTRSetup",CaseNo,"Unknown Case");
        Ret = NULL;
        break;
    }
    
    return(Ret);
}



void P_LPSTRCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    switch(CaseNo) {
    case P_LPSTR_VALID_ADDRESS:
        if ( *SCInfo != NULL )
            LocalFree((HANDLE)*SCInfo);
        break;
    default:
        break;
    }
    
    hConOut;
    hLog;
}


LPWSTR *P_LPWSTRSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    LPWSTR *Ret;
    
    switch(CaseNo) {
        
    case P_LPWSTR_VALID_ADDRESS:
        Ret = *SCInfo = (LPWSTR *) LocalAlloc(LPTR, sizeof(LPWSTR));
        if ( Ret == NULL )
            ErrorPrint(hConOut,hLog,"P_LPWSTRSetup",CaseNo,"out of memory\n");
        break;
    case P_LPWSTR_NULL:
        Ret = NULL;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"P_LPWSTRSetup",CaseNo,"Unknown Case");
        Ret = NULL;
        break;
    }
    
    return(Ret);
}



void P_LPWSTRCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog,HANDLE hConOut)
{
    switch(CaseNo) {
    case P_LPWSTR_VALID_ADDRESS:
        if ( *SCInfo != NULL )
            LocalFree((HANDLE)*SCInfo);
        break;
    default:
        break;
    }
    
    hConOut;
    hLog;
}

PVOID LPPIXELFORMATDESCRIPTORSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPPIXELFORMATDESCRIPTOR Ret;
    
    switch(CaseNo) {
        
    case LPPIXELFORMATDESCRIPTOR_NULL:
        Ret = (LPPIXELFORMATDESCRIPTOR) NULL;
        *SCInfo = Ret;
        break;
        
    case LPPIXELFORMATDESCRIPTOR_VALID:
        if ( (Ret = LocalAlloc(LPTR, sizeof(PIXELFORMATDESCRIPTOR))) == NULL )
            ErrorPrint(hConOut,hLog,"LPPIXELFORMATDESCRIPTORSetup",CaseNo,
            "Error allocating memory");
        
        Ret->nSize = sizeof(PIXELFORMATDESCRIPTOR);
        Ret->nVersion = 1;
        Ret->dwFlags = PFD_DRAW_TO_WINDOW;
        
        *SCInfo = Ret;
        break;
        
    case LPPIXELFORMATDESCRIPTOR_MINUS_ONE:
        Ret = (LPPIXELFORMATDESCRIPTOR) -1;
        *SCInfo = Ret;
        break;
        
        
        
    default:
        ErrorPrint(hConOut,hLog,"LPPIXELFORMATDESCRIPTORSetup",CaseNo,"Unknown Case");
        Ret = (LPPIXELFORMATDESCRIPTOR) NULL;
        break;
    }
    
    return(Ret);
#endif
    return 0;
}


void LPPIXELFORMATDESCRIPTORCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPPIXELFORMATDESCRIPTOR_VALID:
        if ( *SCInfo != NULL )
            LocalFree(*SCInfo);
        break;
        
    default:
        break;
    }
    
    hConOut;
    hLog;
}
