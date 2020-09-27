/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    v_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'V'. It also contains setup and cleanup
    funtions beginning with 'LPV'. For more information, please refer
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
#include <v_cases.bmh>
#include <filever.h>
#include <memmgt.h>


LPVOID LPVOIDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPVOID lpRet = NULL;
    DWORD  dwSize;
    
    switch(CaseNo) {
        // Cases created by JohnMil, 2/28
    case LPVOID_NONALIGNED_V_ADDR:
    case LPVOID_ALIGNED_V_ADDR:
    case LPVOID_RES_UNCOMMIT_V_ADDR:
    case LPVOID_REGION_BEG_V_ADDR:
    case LPVOID_LOCKED_V_ADDR:
    case LPVOID_UNLOCKED_V_ADDR:
        lpRet = VirtAddrCreate(hLog,hConOut,CaseNo,SCInfo);   // Memmgt.c
        break;
    case LPVOID_NULL:
        lpRet = (LPVOID) NULL;
        break;
    case LPVOID_READWRITE_VALID_BUFFER:
        if ( (lpRet = malloc(FIO_READWRITE_BUFFER_SIZE)) == NULL )
            ErrorPrint(hConOut,hLog,"LPVOIDSetup",CaseNo,
            "Error allocating space for read/write buffer");
        *SCInfo = lpRet;
        break;
        
    case LPVOID_VER_VALID_INFO_BUFFER:
    case LPVOID_VER_VALID_INFO_BLOCK:
        
        *SCInfo = NULL;
        
        // get the ver info size from the file
        
        dwSize = 1;//GetVersionInfoSize(VER_DEFAULT_FILE_WITH_VER_INFO, &dwBogusHandle);
        
        if ( dwSize == 0 ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "error getting size of version info\n");
            break;
        }
        
        // alloc buffer and return it
        
        if ( (lpRet = malloc(dwSize)) == NULL ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "error allocing memory for version info\n");
            break;
        }
        
        *SCInfo = lpRet;
        break;
        
    case LPVOID_VER_INVALID_INFO_BLOCK:
        
#ifdef IT_EVER_MAKES_IT_TO_A_BUILD
        *SCInfo = NULL;
        
        // get the ver info size from the file
        
        dwSize = GetVersionInfoSize(VER_DEFAULT_FILE_WITH_VER_INFO,
            &dwBogusHandle);
        
        if ( dwSize == 0 ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "error getting size of version info\n");
            break;
        }
        
        // alloc buffer
        
        if ( (lpRet = malloc(dwSize)) == NULL ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "error allocing memory for version info\n");
            break;
        }
        
        // make pointer point 20 bytes past start
        
        lpRet = (int) lpRet + 20;
        *SCInfo = lpRet;
#endif
        lpRet = *SCInfo = NULL;
        
        break;
        
    case LPVOID_INVALID: {
        SE_CLEANUP_INFO		*CleanupInfo;
        
        if (!MakeBadPointer((PVOID *)&lpRet,
            &CleanupInfo,
            INVALID_POINTER,
            0,
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPVOIDSetup", CaseNo,
                "MakeBadPointer failed.");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                         }
        
    case LPVOID_MALALIGNED: {
    /*
    * Make a randomly large and return an un-aligned pointer to it, in
    * hopes that alignment checking will cause a trap, not reaching an
    * invalid section.
        */
        
        SE_CLEANUP_INFO		*CleanupInfo;
        
        if (!MakeBadPointer((PVOID *)&lpRet,
            &CleanupInfo,
            MALALIGNED_POINTER_1,
            65535,			// randomly large
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPVOIDSetup", CaseNo,
                "MakeBadPointer failed.");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                            }
    case LPVOID_VALID:
        lpRet = (LPVOID)LocalAlloc( LPTR, 5*1024 ); // 5k block
        *SCInfo = lpRet;
        break;
        
    case LPVOID_FILE_INFO:
        
        *SCInfo = NULL;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPVOIDSetup",CaseNo,"Unknown Case");
        lpRet = (LPVOID) NULL;
        break;
    }
    
    return(lpRet);
}


void LPVOIDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
        // Cleanup by JohnMil, 2/28
    case LPVOID_LOCKED_V_ADDR:
    case LPVOID_NONALIGNED_V_ADDR:
    case LPVOID_ALIGNED_V_ADDR:
    case LPVOID_RES_UNCOMMIT_V_ADDR:
    case LPVOID_REGION_BEG_V_ADDR:
    case LPVOID_UNLOCKED_V_ADDR:
        VirtualFree(*SCInfo,0,MEM_RELEASE);   //Memmgt.c
        break;
    case LPVOID_VALID:
    case LPVOID_FILE_INFO:
        if (LocalFree( *SCInfo )!=NULL)
            ErrorPrint(hConOut,hLog,"LPVOIDCleanup",CaseNo,"LocalFree failed" );
        break;
    case LPVOID_READWRITE_VALID_BUFFER:
    case LPVOID_VER_VALID_INFO_BUFFER:
        if ( *SCInfo != NULL )
            free(*SCInfo);
        break;                                   
    case LPVOID_VER_INVALID_INFO_BLOCK:
        if ( *SCInfo != NULL )
            free((LPVOID)(((int)*SCInfo) - 20));
        break;
    default:
        break;
    }
}
