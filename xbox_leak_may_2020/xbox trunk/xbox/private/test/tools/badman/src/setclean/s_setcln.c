/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    s_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'S'. It also contains setup and cleanup
    funtions beginning with 'LPS'. For more information, please refer
    to BadMan.Doc.

Author:

    John Miller (johnmil) 02-Feb-1992

Environment:

    XBox

Revision History:

    01-Apr-2000     schanbai

        Ported to XBox and removed not needed cases

--*/


#include <stdlib.h>
#include <string.h>
#include <setcln.h>
#include <a_cases.bmh>
#include <p_cases.bmh>
#include <s_cases.bmh>
#include <memmgt.h>
#include <filever.h>


UINT
WINAPI
GetTempFileNameA(
    LPCSTR lpPathName,
    LPCSTR lpPrefixString,
    UINT uUnique,
    LPSTR lpTempFileName
    );


LPSECURITY_ATTRIBUTES LPSECURITY_ATTRIBUTESSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPSECURITY_ATTRIBUTES lpRet;
    
    switch(CaseNo) {
        
    case LPSECURITY_ATTRIBUTES_NULL:
        lpRet = (LPSECURITY_ATTRIBUTES) NULL;
        break;
        
    case LPSECURITY_ATTRIBUTES_SAME_AS_CALLING_PROCESS_INHERIT:
    case LPSECURITY_ATTRIBUTES_SAME_AS_CALLING_PROCESS_NO_INHERIT:
        lpRet = (LPSECURITY_ATTRIBUTES) LocalAlloc( LPTR, sizeof(SECURITY_ATTRIBUTES) );
        if (lpRet==NULL)
            ErrorPrint(hConOut,hLog,"LPSECURITY_ATTRIBUTESSetup",CaseNo,"LocalAlloc LPSECURITY_ATTRIBUTES_SAME_AS_CALLING_PROCESS failed!");
        else {
            lpRet->nLength = sizeof(SECURITY_ATTRIBUTES);
            lpRet->lpSecurityDescriptor = NULL;
            lpRet->bInheritHandle = (CaseNo==LPSECURITY_ATTRIBUTES_SAME_AS_CALLING_PROCESS_INHERIT);
        }
        *SCInfo = (LPVOID) lpRet;
        break;
        
    case LPSECURITY_ATTRIBUTES_MINUS_ONE:
        lpRet = (LPSECURITY_ATTRIBUTES) -1;
        break;
        
    case LPSECURITY_ATTRIBUTES_INVALID_LENGTH:
        lpRet = (LPSECURITY_ATTRIBUTES) LocalAlloc( LPTR, sizeof(SECURITY_ATTRIBUTES) );
        if (lpRet==NULL)
            ErrorPrint(hConOut,hLog,"LPSECURITY_ATTRIBUTESSetup",CaseNo,"LocalAlloc LPSECURITY_ATTRIBUTES_SAME_AS_CALLING_PROCESS failed!");
        else {
            lpRet->nLength = sizeof(SECURITY_ATTRIBUTES) - 4;
            lpRet->lpSecurityDescriptor = NULL;
            lpRet->bInheritHandle = TRUE;
        }
        *SCInfo = (LPVOID) lpRet;
        break;
        
    case LPSECURITY_ATTRIBUTES_INVALID_SECURITY_DESCRIPTOR:
        lpRet = (LPSECURITY_ATTRIBUTES) LocalAlloc( LPTR, sizeof(SECURITY_ATTRIBUTES) );
        if (lpRet==NULL)
            ErrorPrint(hConOut,hLog,"LPSECURITY_ATTRIBUTESSetup",CaseNo,"LocalAlloc LPSECURITY_ATTRIBUTES_SAME_AS_CALLING_PROCESS failed!");
        else {
            lpRet->nLength = sizeof(SECURITY_ATTRIBUTES);
            lpRet->lpSecurityDescriptor = (LPVOID)-1L;
            lpRet->bInheritHandle = TRUE;
        }
        *SCInfo = (LPVOID) lpRet;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPSECURITY_ATTRIBUTESSetup",CaseNo,"Unknown Case");
        lpRet = (LPSECURITY_ATTRIBUTES) NULL;
        break;
    }
    
    return(lpRet);
}


void LPSECURITY_ATTRIBUTESCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPSECURITY_ATTRIBUTES_SAME_AS_CALLING_PROCESS_INHERIT:
    case LPSECURITY_ATTRIBUTES_SAME_AS_CALLING_PROCESS_NO_INHERIT:
    case LPSECURITY_ATTRIBUTES_INVALID_LENGTH:
    case LPSECURITY_ATTRIBUTES_INVALID_SECURITY_DESCRIPTOR:
        if ((*SCInfo)!=NULL) {
            if (LocalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"LPSECURITY_ATTRIBUTESCleanup",CaseNo,"LocalFree failed!");
        }
        
    default:
        break;
    }
}



/************************************
L     PPPP   SSSS IIIII ZZZZZ EEEEE
L     P   P S       I      Z  E
L     PPPP   SSS    I     Z   EEEE
L     P         S   I    Z    E
LLLLL P     SSSS  IIIII ZZZZZ EEEEE
************************************/

LPSIZE LPSIZESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPSIZE Ret;
    
    switch(CaseNo) {
    case LPSIZE_VALID:
        Ret = (LPSIZE) LocalAlloc (LPTR, sizeof(SIZE));
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"LPSIZESetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break ;
    case LPSIZE_NULL:
        Ret=(LPSIZE) NULL;
        break;
    case LPSIZE_RANDOM:
        Ret=(LPSIZE) rand();
        break;
    case LPSIZE_MINUS_ONE:
        Ret=(LPSIZE) -1;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPSIZESetup",CaseNo,"Unknown Case");
        Ret = (LPSIZE) NULL;
        break;
    }
    
    return(Ret);
}


void LPSIZECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPSIZE_VALID:
        if ((*SCInfo)!=NULL) {
            if (LocalFree((HANDLE)(*SCInfo))!=NULL)
                ErrorPrint(hConOut,hLog,"LPSIZECleanup",CaseNo,"LocalFree failed!");
        }
    default:
        break;
    }
}


LPSTR LPSTRSetup(int CaseNo, LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPSTR lpRet;
    
    switch(CaseNo) {
    case LPSTR_RANDOM:
        lpRet = (LPSTR) rand();
        break;
        
    case LPSTR_MINUS_ONE:
        lpRet = (LPSTR) -1;
        break;
        
    case LPSTR_NULL:
        lpRet = (LPSTR) NULL;
        break;
    case LPSTR_BASE_VALID_LMEM_BLOCK:
    case LPSTR_INSIDE_VALID_LMEM_BLOCK:
    case LPSTR_INSIDE_FREED_LMEM_BLOCK:
        //  case LPSTR_INSIDE_DISCARDED_LMEM_BLOCK:
        lpRet = LMemPointerCreate(hLog,hConOut,CaseNo,SCInfo);
        break;
    case LPSTR_BASE_VALID_GMEM_BLOCK:
    case LPSTR_INSIDE_VALID_GMEM_BLOCK:
    case LPSTR_INSIDE_FREED_GMEM_BLOCK:
    case LPSTR_INSIDE_DISCARDED_GMEM_BLOCK:
        lpRet = GMemPointerCreate(hLog,hConOut,CaseNo,SCInfo);
        break;
    case LPSTR_VALID_HEAP_BLOCK:
    case LPSTR_DESTROYED_HEAP_BLOCK:
    case LPSTR_FREED_HEAP_BLOCK:
        lpRet = HeapPointerCreate(hLog,hConOut,CaseNo,SCInfo);
        break;
        
    case LPSTR_VALID_NT_IMAGE_NAME:
        lpRet = (LPSTR) LocalAlloc( LPTR, 64 );
        if (lpRet!=NULL)
            sprintf( lpRet, "bminfprc.exe" );
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_NON_EXISTANT_IMAGE_NAME:
        lpRet = (LPSTR) LocalAlloc( LPTR, 64 );
        if (lpRet!=NULL)
            sprintf( lpRet, "abcdefgh.ijk" );
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_INVALID_FILE_NAME:
        lpRet = (LPSTR) LocalAlloc( LPTR, 64 );
        if (lpRet!=NULL)
            sprintf( lpRet, "[/]" );
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_NON_EXISTENT_DIRECTORY:
        lpRet = (LPSTR) LocalAlloc( LPTR, 64 );
        if (lpRet!=NULL)
            sprintf( lpRet, "\\aaa\\bbb\\ccc\\ddd\\eee\\fff\\gomer\\hhh\\iii" );
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_VALID_CURRENT_DIRECTORY:
        lpRet = (LPSTR) LocalAlloc( LPTR, 64 );
        if (lpRet!=NULL)
            sprintf( lpRet, "\\" );
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_TEMP_FILE_NAME:
        {
            HANDLE hMem ;
            LPSTR  lpMem ;
            
            hMem  = LocalAlloc( LPTR, 145 ) ;
            if( !hMem )
            {
                ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
                lpRet = NULL ;
                *SCInfo = NULL ;
            }
            else
            {
                *SCInfo = (LPVOID)hMem ;
                
                lpMem = (LPSTR)LocalLock( hMem ) ;
                if( !lpMem )
                {
                    ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
                    lpRet = NULL ;
                }
                else
                {
                    if( !GetTempFileNameA( ".", "LPSTR", 0, lpMem ) )
                    {
                        ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"GetTempFileName failed!");
                        lpRet = NULL ;
                    }
                    else
                        lpRet = lpMem ;
                }
            }
        }
        break ;
        
    case LPSTR_DOUBLE_NULL_NULL:
        lpRet = (LPSTR) LocalAlloc( LPTR, 4 );
        if (lpRet!=NULL)
            sprintf( lpRet, "\0" );
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_DOUBLE_NULL_STRING:
        lpRet = (LPSTR) LocalAlloc( LPTR, 64 );
        if (lpRet!=NULL)
            sprintf( lpRet, "This String has a double NULL terminator\0" );
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_FILENAME_VALID:
    case LPSTR_FILENAME_INVALID1:
    case LPSTR_FILENAME_INVALID2:
    case LPSTR_FILENAME_INVALID3:
    case LPSTR_FILENAME_INVALID4:
    case LPSTR_FILENAME_INVALID5:
    case LPSTR_FILENAME_INVALID6:
    case LPSTR_FILENAME_INVALID7:
    case LPSTR_FILENAME_INVALID8:
    case LPSTR_FILENAME_INVALID9:
    case LPSTR_DIRNAME_INVALID1:
    case LPSTR_DIRNAME_INVALID2:
    case LPSTR_DIRNAME_INVALID3:
    case LPSTR_DIRNAME_INVALID4:
    case LPSTR_SRC_FILE_EXISTS:
    case LPSTR_SRC_FILE_NOT_EXISTS:
    case LPSTR_DEST_FILE_EXISTS:
    case LPSTR_DEST_FILE_NOT_EXISTS:
    case LPSTR_DIRNAME_VALID:
    case LPSTR_PATH_EXISTS:
    case LPSTR_PATH_NOT_EXISTS:
    case LPSTR_PREFIX_VALID:
    case LPSTR_PREFIX_TOO_LONG:
    case LPSTR_VALID_PATH_BUFFER:
    case LPSTR_VALID_ROOT_PATHNAME:
    case LPSTR_INVALID_ROOT_PATHNAME:
    case LPSTR_FILE_EXISTS:
    case LPSTR_FILE_NOT_EXISTS:
    case LPSTR_FILE_READONLY:
    case LPSTR_VOLUME_NAME_BUFFER:
    case LPSTR_FILE_SYSTEM_NAME_BUFFER:
    case LPSTR_VER_SRC_FILE_EXISTS:
    case LPSTR_VER_SRC_FILE_NOT_EXISTS:
    case LPSTR_VER_SRC_DIR_EXISTS:
    case LPSTR_VER_SRC_DIR_NOT_EXISTS:
    case LPSTR_VER_DEST_FILE_EXISTS:
    case LPSTR_VER_DEST_FILE_NOT_EXISTS:
    case LPSTR_VER_DEST_DIR_EXISTS:
    case LPSTR_VER_DEST_DIR_NOT_EXISTS:
    case LPSTR_NULL_STRING:
    case LPSTR_WINDOWS_DIR:
    case LPSTR_VER_APP_DIR:
    case LPSTR_VER_SUB_BLOCK_EXISTS:
    case LPSTR_VER_SUB_BLOCK_NOT_EXISTS:
    case LPSTR_SEARCH_PATH_ILL_FORMED:
    case LPSTR_FILE_IN_CURR_DIR_NO_EXT:
    case LPSTR_EXT_FILE_IN_CURR_DIR:
    case LPSTR_EXT_ILL_FORMED:
    case LPSTR_FILE_NOT_EXISTS_ON_PATH:
        lpRet = FIOLpstrSetup(CaseNo, SCInfo, hLog, hConOut);
        break;
        
    case LPSTR_EMPTY:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet!=NULL)
            lpRet[0] = '\0';
        else
            ErrorPrint (hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_INIFILE_FILENAME:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet!=NULL)
            sprintf (lpRet, ".\\bminitst.ini");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_INIFILE_SECTION_NAME:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet!=NULL)
            sprintf (lpRet, "badman_section");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_INIFILE_KEYNAME:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet!=NULL)
            sprintf (lpRet, "badman_key");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_INIFILE_INT:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet!=NULL)
            sprintf (lpRet, "64");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_INIFILE_STRING:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet!=NULL)
            sprintf (lpRet, "badman_inifile_string_value");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_INIFILE_KEY_VALUE:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet!=NULL)
            sprintf (lpRet, "badman_key = badman_inifile_string_value\0");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_INIFILE_SECTION:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet != NULL)
        {
            sprintf (lpRet, "key1 = section1 key2 = section2 key3 = section3 ");
            //012345678901234567890123456789012345678901234567
            lpRet[15] = '\0';
            lpRet[31] = '\0';
            lpRet[47] = '\0';
        }
        else
            ErrorPrint (hConOut, hLog, "LPSTRSetup", CaseNo, "LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_BUFFER_1K:
        lpRet = (LPSTR) LocalAlloc (LPTR, 1024);
        if (lpRet == NULL)
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_BUFFER_512_BYTE:
        lpRet = (LPSTR) LocalAlloc (LPTR, 512);
        if (lpRet == NULL)
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_BUFFER_1_BYTE:
        lpRet = (LPSTR) LocalAlloc (LPTR, 1);
        if (lpRet == NULL)
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_REG_KEYNAME:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet != NULL)
            sprintf (lpRet, "Badman test key");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_REG_VALUENAME:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet != NULL)
            sprintf (lpRet, "Badman value");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
    case LPSTR_257_CHAR:
        lpRet = (LPSTR) LocalAlloc (LPTR, 258);
        if (lpRet != NULL)
        {
            int i;
            for (i=0; i<258; i++)
                lpRet[i] = (char)(i | 1);
            lpRet[258] = '\0';
        }
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break ;
        
        /*
        * a nice way to avoid duplicating code:
        */
        
#define STRDUP(targ, src)                       \
    (targ) = (LPSTR)GlobalAlloc(GPTR, sizeof (src));        \
    \
    if (!targ) {                            \
    ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,     \
    "LocalAlloc failed!");              \
    } else {                            \
    strcpy((targ), src);                  \
    \
    *SCInfo = (targ);                   \
        }
        
    case LPSTR_INVALID:
        // an invalid pointer
        
        lpRet = (LPSTR)0x666;   // invalid, as well as magic, expand when
        // bad-pointer stuff is implemnted.
        
        *SCInfo = lpRet;
        
        break;
        
    case LPSTR_64K_BUFFER:
        // pointer to a 64k buffer
        
        lpRet = (LPSTR)GlobalAlloc(GPTR, 64*1024U);
        
        *SCInfo = lpRet;
        
        if (!lpRet) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "GlobalAlloc failed!");
        }
        
        break;
        
    case LPSTR_2BYTE_BUFFER:
        // pointer to a buffer too small for the shortest cannonical
        // privilege or name/ascii verison of a display name, or username.
        
        if ( NULL == (lpRet = GetNativeBuffer(SMALL_NATIVE_BUFFER_SIZE)) ) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "LocalAlloc failed!");
        }
        
        *SCInfo = lpRet;
        
        break;
        
    case LPSTR_FAT_FILENAME:
        // valid FAT filename
        STRDUP(lpRet, "D:\\testfile.fat");
        break;
        
    case LPSTR_FILENAME_BOGUS:
    /*
    * filename with invalid chars
        */
        
        STRDUP(lpRet, "bogus:test:filename");
        
        break;
        
    case LPSTR_INVALID_1: {
    /*
    * first DWORD of WSTR valid, non-null chars
        */
        
        int         i;
        SE_CLEANUP_INFO     *CleanupInfo;
        
        if (!MakeBadPointer((PVOID *)&lpRet,
            &CleanupInfo,
            VALIDRANGE_POINTER,
            1 * sizeof (DWORD),
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        try {
            for (i = 0; i < 1 * sizeof (DWORD); i++) {
                lpRet[i] = 'c';
            }
        } except (EXCEPTION_EXECUTE_HANDLER) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "Trapped too soon!");
            
            MakeBadPointer(NULL,
                &CleanupInfo,
                CLEANUP,
                0,
                hConOut,
                hLog);
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                          }
        
    case LPSTR_INVALID_N: {
    /*
    * a STR with all but last DWORD of length valid, arbitraily make it
    * 10 DWORDS long
        */
        
        int         i;
        SE_CLEANUP_INFO     *CleanupInfo;
        
        if (!MakeBadPointer((PVOID *)&lpRet,
            &CleanupInfo,
            VALIDRANGE_POINTER,
            10 * sizeof (DWORD),
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        try {
            for (i = 0; i < 10 * sizeof (DWORD); i++) {
                lpRet[i] = 'd';
            }
        } except (EXCEPTION_EXECUTE_HANDLER) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "Trapped too soon!");
            
            MakeBadPointer(NULL,
                &CleanupInfo,
                CLEANUP,
                0,
                hConOut,
                hLog);
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                          }
        
    case LPSTR_NTFS_FILENAME: {
    /*
    * valid NTFS filename (includes unicode chars)
        */
        
        char            c;
        
        STRDUP(lpRet, " :\\foo");
        
        for (c = 'c'; c <= 'z'; c++) {
            lpRet[0] = c;
            
            if (CreateFile(lpRet,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                CREATE_ALWAYS,
                FILE_FLAG_DELETE_ON_CLOSE,
                NULL)) {
                SetLastError(NO_ERROR);
                
                break;
            }
        }
        
        if (GetLastError() != NO_ERROR) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "CreateFile failed, file doesn't exist.");
            
            GlobalFree(lpRet);
            
            lpRet = NULL;
        }
        
        break;
                              }
        
    case LPSTR_RELATIVE_FILENAME:
    /*
    * a relative path/filename
        */
        
        STRDUP(lpRet, "foo");
        
        if (!CreateFile(lpRet,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_FLAG_DELETE_ON_CLOSE,
            NULL)) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "CreateFile failed, file doesn't exist.");
            
            GlobalFree(lpRet);
            
            lpRet = NULL;
        }
        
        break;
        
    case LPSTR_REMOTE_FILENAME:
    /*
    * a drive-relative remote filename
        */
        
        STRDUP(lpRet, "x:\\foo");
        
        if (!CreateFile(lpRet,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_FLAG_DELETE_ON_CLOSE,
            NULL)) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "CreateFile failed, file doesn't exist.");
            
            GlobalFree(lpRet);
            
            lpRet = NULL;
        }
        
        break;
        
    case LPSTR_TOO_LONG:
        // a string of characters which is > 64Kb
        
        if ( NULL == (lpRet = (LPSTR)GlobalAlloc(GPTR, 66000)) ) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "GlobalAlloc failed");
        } else {
            int         i;
            
            for (i = 0; i < 66000 - 1; i++) {
                lpRet[i] = 'a';
            }
            
            lpRet[i] = '\0';
        }
        
        *SCInfo = lpRet;
        
        break;
        
    case LPSTR_UNC_FILENAME:
    /*
    * a UNC filename
        */
        
        STRDUP(lpRet, "\\\\scratch\\scratch\\foo");
        
        if (!CreateFile(lpRet,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_FLAG_DELETE_ON_CLOSE,
            NULL)) {
            ErrorPrint(hConOut, hLog, "LPSTRSetup", CaseNo,
                "CreateFile failed, file doesn't exist.");
            
            GlobalFree(lpRet);
            
            lpRet = NULL;
        }
        
        break;
        
    case LPSTR_46CHAR:
        lpRet = (LPSTR) LocalAlloc (LPTR, 46);
        if (lpRet != NULL)
            sprintf (lpRet, "The quick brown fox jumped over the lazy dogs");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_DISPLAY:
        lpRet = (LPSTR) LocalAlloc (LPTR, 8);
        if (lpRet != NULL)
            sprintf (lpRet, "DISPLAY");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_OPEN:
        lpRet = (LPSTR) LocalAlloc (LPTR, 5);
        if (lpRet != NULL)
            sprintf (lpRet, "OPEN");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_COMMAND_LINE:
        lpRet = (LPSTR) LocalAlloc (LPTR, 64);
        if (lpRet != NULL)
            sprintf (lpRet, "asdf jkl; qwer uiop");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_TEMP_DIR:
        lpRet = (LPSTR) LocalAlloc (LPTR, MAX_PATH);
        if (lpRet != NULL)
            // BUGBUG GetTempPathA(MAXPATH, lpRet);
            strcpy(lpRet, "d:\\winnt\\xboxtest\\");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    case LPSTR_ROOT_DIR:
        lpRet = (LPSTR) LocalAlloc (LPTR, MAX_PATH);
        if (lpRet != NULL)
            sprintf (lpRet, "t:\\");
        else
            ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = lpRet;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"Unknown Case");
        lpRet = NULL;
        break;
    }
    
    return lpRet;
}


void LPSTRCleanup(int CaseNo, LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPSTR_BASE_VALID_LMEM_BLOCK:
    case LPSTR_INSIDE_VALID_LMEM_BLOCK:
        LocalFree((HANDLE) *SCInfo);
        break;
    case LPSTR_BASE_VALID_GMEM_BLOCK:
    case LPSTR_INSIDE_VALID_GMEM_BLOCK:
        GlobalFree((HANDLE) *SCInfo);
        break;
    case LPSTR_VALID_HEAP_BLOCK:
    case LPSTR_FREED_HEAP_BLOCK:
        HeapDestroy((HANDLE) *SCInfo);
        break;
    case LPSTR_VALID_NT_IMAGE_NAME:
    case LPSTR_NON_EXISTANT_IMAGE_NAME:
    case LPSTR_INVALID_FILE_NAME:
    case LPSTR_NON_EXISTENT_DIRECTORY:
    case LPSTR_VALID_CURRENT_DIRECTORY:
    case LPSTR_DOUBLE_NULL_STRING:
    case LPSTR_DOUBLE_NULL_NULL:
    case LPSTR_EMPTY:
    case LPSTR_INIFILE_FILENAME:
    case LPSTR_INIFILE_SECTION_NAME:
    case LPSTR_INIFILE_KEYNAME:
    case LPSTR_INIFILE_INT:
    case LPSTR_INIFILE_STRING:
    case LPSTR_INIFILE_KEY_VALUE:
    case LPSTR_INIFILE_SECTION:
    case LPSTR_BUFFER_1K:
    case LPSTR_BUFFER_512_BYTE:
    case LPSTR_BUFFER_1_BYTE:
    case LPSTR_REG_KEYNAME:
    case LPSTR_REG_VALUENAME:
    case LPSTR_257_CHAR:
    case LPSTR_46CHAR:
    case LPSTR_OPEN:
    case LPSTR_COMMAND_LINE:
    case LPSTR_TEMP_DIR:
    case LPSTR_ROOT_DIR:
        if (*SCInfo!=NULL)
            if (LocalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"LPSTRCleanup",CaseNo,"LocalFree failed!");
            break;
            
    case LPSTR_TEMP_FILE_NAME:
        if( *SCInfo )
        {
            LPSTR lpMem ;
            
            LocalUnlock( (HANDLE)*SCInfo ) ;
            
            lpMem = LocalLock( (HANDLE)*SCInfo ) ;
            if( lpMem )
                DeleteFile(lpMem) ;
            else
                ErrorPrint(hConOut,hLog,"LPSTRCleanup",CaseNo,"LocalLock failed!");
            
            LocalUnlock( (HANDLE)*SCInfo ) ;
            
            if (LocalFree( (HANDLE)(*SCInfo) )!=NULL)
                ErrorPrint(hConOut,hLog,"LPSTRCleanup",CaseNo,"LocalFree failed!");
        }
        break ;
        
    case LPSTR_FILENAME_VALID:
    case LPSTR_FILENAME_INVALID1:
    case LPSTR_FILENAME_INVALID2:
    case LPSTR_FILENAME_INVALID3:
    case LPSTR_FILENAME_INVALID4:
    case LPSTR_FILENAME_INVALID5:
    case LPSTR_FILENAME_INVALID6:
    case LPSTR_FILENAME_INVALID7:
    case LPSTR_FILENAME_INVALID8:
    case LPSTR_FILENAME_INVALID9:
    case LPSTR_DIRNAME_VALID:
    case LPSTR_DIRNAME_INVALID1:
    case LPSTR_DIRNAME_INVALID2:
    case LPSTR_DIRNAME_INVALID3:
    case LPSTR_DIRNAME_INVALID4:
    case LPSTR_SRC_FILE_EXISTS:
    case LPSTR_SRC_FILE_NOT_EXISTS:
    case LPSTR_DEST_FILE_EXISTS:
    case LPSTR_DEST_FILE_NOT_EXISTS:
        FIOLpstrCleanup(CaseNo, SCInfo, hLog, hConOut);
        break;
        
    case LPSTR_64K_BUFFER:
    case LPSTR_FAT_FILENAME:
    case LPSTR_FILENAME_BOGUS:
    case LPSTR_NTFS_FILENAME:
    case LPSTR_RELATIVE_FILENAME:
    case LPSTR_REMOTE_FILENAME:
    case LPSTR_TOO_LONG:
    case LPSTR_UNC_FILENAME:
        GlobalFree(*SCInfo);
        break;
        
    case LPSTR_2BYTE_BUFFER:
        FreeNativeBuffer(*SCInfo, SMALL_NATIVE_BUFFER_SIZE);
        
        break;
        
    case LPSTR_INVALID_1:
    case LPSTR_INVALID_N:
        MakeBadPointer(NULL,
            (SE_CLEANUP_INFO **)SCInfo,
            CLEANUP,
            0,
            hConOut,
            hLog);
        
        if (*SCInfo) {
            ErrorPrint(hConOut, hLog, "LPSTRCleanup", CaseNo,
                "Cleanup failed.");
        }
        
        break;
        
    default:
        break;
    }
}



/******************************************************************************
L     PPPP   SSSS Y   Y  SSSS TTTTT EEEEE M   M       IIIII N   N FFFFF  OOO
L     P   P S      Y Y  S       T   E     MM MM         I   NN  N F     O   O
L     PPPP   SSS    Y    SSS    T   EEEE  M M M         I   N N N FFFF  O   O
L     P         S   Y       S   T   E     M   M         I   N  NN F     O   O
LLLLL P     SSSS    Y   SSSS    T   EEEEE M   M _____ IIIII N   N F      OOO
******************************************************************************/

PVOID LPSYSTEM_INFOSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPSYSTEM_INFO Ret;
    
    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"LPSYSTEM_INFOSetup",CaseNo,"Unknown Case");
        Ret = (LPSYSTEM_INFO) NULL;
        break;
    }
    
    return(Ret);
    
    SCInfo;
#endif
    return 0;
}


void LPSYSTEM_INFOCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    CaseNo;
    SCInfo;
    hLog;
    hConOut;
}



/************************************************************************
L     PPPP   SSSS Y   Y  SSSS TTTTT EEEEE M   M TTTTT IIIII M   M EEEEE
L     P   P S      Y Y  S       T   E     MM MM   T     I   MM MM E
L     PPPP   SSS    Y    SSS    T   EEEE  M M M   T     I   M M M EEEE
L     P         S   Y       S   T   E     M   M   T     I   M   M E
LLLLL P     SSSS    Y   SSSS    T   EEEEE M   M   T   IIIII M   M EEEEE
************************************************************************/

LPSYSTEMTIME LPSYSTEMTIMESetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPSYSTEMTIME Ret;
    
    switch(CaseNo) {
        
    case LPSYSTEMTIME_NULL:
        Ret = NULL;
        break;
    case LPSYSTEMTIME_VALID_ADDRESS:
        if ( (Ret = malloc(sizeof(SYSTEMTIME))) == NULL )
            ErrorPrint(hConOut,hLog,"LPSYSTEMTIMESetup",CaseNo,
            "Error allocating memory");
        *SCInfo = Ret;
        break;
    case LPSYSTEMTIME_VALID_DATA:
        if ( (Ret = malloc(sizeof(SYSTEMTIME))) == NULL )
            ErrorPrint(hConOut,hLog,"LPSYSTEMTIMESetup",CaseNo,
            "Error allocating memory");
        Ret->wYear         = 1472;
        Ret->wMonth        = 3;    // march
        Ret->wDayOfWeek    = 5;    // friday
        Ret->wDay          = 20;
        Ret->wHour         = 10;
        Ret->wMinute       = 35;
        Ret->wSecond       = 40;
        Ret->wMilliseconds = 0;
        *SCInfo = Ret;
        break;
    case LPSYSTEMTIME_INVALID_DATA:
        if ( (Ret = malloc(sizeof(SYSTEMTIME))) == NULL )
            ErrorPrint(hConOut,hLog,"LPSYSTEMTIMESetup",CaseNo,
            "Error allocating memory");
        memchr(Ret, -1, sizeof(SYSTEMTIME));  // -1's should be garbage for all
        *SCInfo = Ret;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPSYSTEMTIMESetup",CaseNo,"Unknown Case");
        Ret = (LPSYSTEMTIME) NULL;
        break;
    }
    
    return(Ret);
}


void LPSYSTEMTIMECleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPSYSTEMTIME_VALID_ADDRESS:
    case LPSYSTEMTIME_VALID_DATA:
    case LPSYSTEMTIME_INVALID_DATA:
        if ( *SCInfo != NULL )
            free(*SCInfo);
    default:
        break;
    }
    
    hConOut;
    hLog;
}
