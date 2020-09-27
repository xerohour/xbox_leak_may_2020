/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    g_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'G'. It also contains setup and cleanup
    funtions beginning with 'LPG'. For more information, please refer
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
#include <g_cases.bmh>


PGENERIC_MAPPING
PGENERIC_MAPPINGSetup( int     CaseNo,
                       LPVOID  *SCInfo,
                       HANDLE  hLog,
                       HANDLE  hConOut
                     )
{
    DWORD  PointerSize, PointerType;
    PGENERIC_MAPPING    GenM;
    SE_CLEANUP_INFO     *CleanupInfo = NULL;
    
    /*
    * If *SCInfo is NULL on entry we need to allocate space, otherwise
    * we'll try to put the SID at the location specified by *SCInfo.
    * This is so that more complex structures can be created with our
    * strange sets of data in them.
    */
    
    if (*SCInfo && (DWORD)*SCInfo != GET_REQUIRED_SIZE) {
        GenM = (PGENERIC_MAPPING)*SCInfo;
    } else {
    /*
    * Determine size, type of pointer and generate it.
        */
        
        PointerType = VALIDRANGE_POINTER;
        PointerSize = sizeof (GENERIC_MAPPING);
        
        switch (CaseNo) {
            
        case PGENERIC_MAPPING_VALID:
        /*
        * Defaults are correct
            */
            
            break;
            
        case PGENERIC_MAPPING_MALALIGNED:
        /*
        * pointer aligned on a byte boundary
            */
            
            PointerType = MALALIGNED_POINTER_1;
            
            break;
            
        case PGENERIC_MAPPING_INVALID:
        /*
        * An invalid pointer.
            */
            
            PointerType = INVALID_POINTER;
            PointerSize = 0;
            
            break;
            
        case PGENERIC_MAPPING_INVALID_1:
        /*
        * Only first DWORD of the generic mapping is valid.
            */
            
            PointerSize = 1 * sizeof (DWORD);
            
            break;
            
        case PGENERIC_MAPPING_INVALID_2:
        /*
        * frist & second DWORD of generic mapping valid
            */
            
            PointerSize = 2 * sizeof (DWORD);
            
            break;
            
        case PGENERIC_MAPPING_INVALID_3:
        /*
        * all but last DWORD of generic mapping valid
            */
            
            PointerSize = 2 * sizeof (DWORD);
            
            break;
            
        case PGENERIC_MAPPING_NULL:
            PointerType = NULL_POINTER;
            PointerSize = 0;
            
            break;
            
        default:
            ErrorPrint(hConOut, hLog, "PSIDSetup", CaseNo,
                "Unknown Case");
            
            break;
        }
        
        if ((DWORD)*SCInfo == GET_REQUIRED_SIZE) {
            *SCInfo = (VOID *)PointerSize;
            
            return NULL;
        } else {
            if (!MakeBadPointer((PVOID *)&GenM,
                &CleanupInfo,
                PointerType,
                PointerSize,
                hConOut,
                hLog)) {
                ErrorPrint(hConOut, hLog,
                    "PGENERIC_MAPPINGSetup", CaseNo,
                    "MakeBadPointer failed");
            }
        }
    }
    
    /*
    * part 2 builds the LUID
    */
    
    try {
        switch (CaseNo) {
        case PGENERIC_MAPPING_NULL:
        case PGENERIC_MAPPING_INVALID:
        /*
        * no valid bytes, don't waste the cycles to pop the
        * try
            */
            
            break;
            
        case PGENERIC_MAPPING_VALID:
        case PGENERIC_MAPPING_INVALID_1:
        case PGENERIC_MAPPING_INVALID_2:
        case PGENERIC_MAPPING_INVALID_3:
        case PGENERIC_MAPPING_MALALIGNED:
            // I don't think the un-aligned case is any
            // different for us, though it might be...
            
            /*
            * stuff stuff (stuff order is important)
            */
            
            GenM -> GenericRead = STANDARD_RIGHTS_READ |
                TOKEN_QUERY;
            
            GenM -> GenericWrite = STANDARD_RIGHTS_WRITE |
                TOKEN_ADJUST_PRIVILEGES | TOKEN_ADJUST_GROUPS |
                TOKEN_ADJUST_DEFAULT;
            
            GenM -> GenericExecute = STANDARD_RIGHTS_EXECUTE |
                TOKEN_IMPERSONATE;
            
            GenM -> GenericAll = TOKEN_ALL_ACCESS;
            
            break;
            
        default:
            ErrorPrint(hConOut, hLog, "PGENERIC_MAPPINGSetup",
                CaseNo, "CANT HAPPEN");
            
            break;
        }
    } except (EXCEPTION_EXECUTE_HANDLER) {
    /*
    * This isn't a problem since we expect that we might be
    * trying to put data in a bogus location.
        */
        
        // we almost anyway... I want to know if I trap setting
        // unaligned stuff...
        
        if (CaseNo == PGENERIC_MAPPING_MALALIGNED) {
            
            ErrorPrint(hConOut, hLog, "PGENERIC_MAPPINGSetup",
                CaseNo, "MALALIGNED TRAPPED");
        }
    }
    
    /*
    * If *SCInfo had useful data initially, don't clobber it, otherwise
    * set it to the CleanupInfo that we generated.
    */
    
    if (!*SCInfo) {
        *SCInfo = CleanupInfo;
    }
    
    return GenM;
}


void
PGENERIC_MAPPINGCleanup(
                        int         CaseNo,
                        LPVOID          *SCInfo,
                        HANDLE          hLog,
                        HANDLE          hConOut
                        )
{
    switch(CaseNo) {
    case PGENERIC_MAPPING_NULL:
    case PGENERIC_MAPPING_INVALID:
    case PGENERIC_MAPPING_VALID:
    case PGENERIC_MAPPING_INVALID_1:
    case PGENERIC_MAPPING_INVALID_2:
    case PGENERIC_MAPPING_INVALID_3:
    case PGENERIC_MAPPING_MALALIGNED:
    /*
    * Mop time.
        */
        
        MakeBadPointer(NULL,
            (SE_CLEANUP_INFO **)SCInfo,
            CLEANUP,
            0,
            hConOut,
            hLog);
        
        if (*SCInfo) {
            ErrorPrint(hConOut, hLog, "PGENERIC_MAPPINGCleanup",
                CaseNo, "Cleanup failed.");
        }
        
        break;
        
    default:
        /* look, no mess! */
        
        break;
    }
}
