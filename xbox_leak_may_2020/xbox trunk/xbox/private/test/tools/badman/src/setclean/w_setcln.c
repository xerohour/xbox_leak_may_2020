/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    w_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'W'. It also contains setup and cleanup
    funtions beginning with 'LPW'. For more information, please refer
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
#include <W_Cases.bmh>
#include <filever.h>

UINT
WINAPI
GetTempFileNameW(
    LPCWSTR lpPathName,
    LPCWSTR lpPrefixString,
    UINT uUnique,
    LPWSTR lpTempFileName
    );



LPWIN32_FIND_DATAA LPWIN32_FIND_DATAASetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPWIN32_FIND_DATAA Ret;
    
    switch(CaseNo) {
        
    case LPWIN32_FIND_DATAA_VALID:
        if ( (Ret = malloc(sizeof(WIN32_FIND_DATAA))) == NULL )
            ErrorPrint(hConOut,hLog,"LPWIN32_FIND_DATAASetup",CaseNo,
            "Error allocating memory");
        *SCInfo = Ret;
        break;
    case LPWIN32_FIND_DATAA_NULL:
        Ret = NULL;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPWIN32_FIND_DATAASetup",CaseNo,"Unknown Case");
        Ret = (LPWIN32_FIND_DATAA) NULL;
        break;
    }
    
    return(Ret);
}


void LPWIN32_FIND_DATAACleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
        
    case LPWIN32_FIND_DATAA_VALID:
        if ( *SCInfo != NULL )
            free(*SCInfo);
        break;
    default:
        break;
    }
    
    hConOut;
    hLog;
}


LPWIN32_FIND_DATAW LPWIN32_FIND_DATAWSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPWIN32_FIND_DATAW Ret;
    
    switch(CaseNo) {
        
    case LPWIN32_FIND_DATAW_VALID:
        if ( (Ret = malloc(sizeof(WIN32_FIND_DATAW))) == NULL )
            ErrorPrint(hConOut,hLog,"LPWIN32_FIND_DATAWSetup",CaseNo,
            "Error allocating memory");
        *SCInfo = Ret;
        break;
    case LPWIN32_FIND_DATAW_NULL:
        Ret = NULL;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPWIN32_FIND_DATAWSetup",CaseNo,"Unknown Case");
        Ret = (LPWIN32_FIND_DATAW) NULL;
        break;
    }
    
    return(Ret);
}


void LPWIN32_FIND_DATAWCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPWIN32_FIND_DATAW_VALID:
        if ( *SCInfo != NULL )
            free(*SCInfo);
        break;
    default:
        break;
    }
    
    hConOut;
    hLog;
}


LPWORD LPWORDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPWORD Ret;
    LPWORD lpTemp;
    WORD   i;
    
    switch(CaseNo) {
        
    case LPWORD_NULL:
        Ret     = NULL;
        *SCInfo = Ret;
        break;
    case LPWORD_MINUS_ONE:
        if ( (Ret = malloc(sizeof(WORD))) == NULL )
            ErrorPrint(hConOut,hLog,"LPWORDSetup",CaseNo,
            "Error allocating memory");
        else
            *Ret = (WORD)-1;
        *SCInfo = Ret;
        break;
    case LPWORD_FREED_LOCAL:
        if ( (Ret = LocalAlloc(LPTR, sizeof(WORD))) == NULL )
            ErrorPrint(hConOut,hLog,"LPWORDSetup",CaseNo,
            "Error allocating memory");
        else
        { // bogus memory
            *SCInfo = Ret;
            LocalFree(Ret);
        }
        
        break;
    case LPWORD_65_VALID:
        if ( (Ret = LocalAlloc(LPTR, 65*sizeof(WORD))) == NULL )
            ErrorPrint(hConOut,hLog,"LPWORDSetup",CaseNo,
            "Error allocating memory");
        else
        { // 0 to 64
            lpTemp = Ret; 
            for (i=0; i< 65; i++)
            {
                *lpTemp++ = i;
            }
            *SCInfo = Ret;
        }
        
        break;
    case LPWORD_VALID_ADDRESS:
        if ( (Ret = malloc(sizeof(WORD))) == NULL )
            ErrorPrint(hConOut,hLog,"LPWORDSetup",CaseNo,
            "Error allocating memory");
        *SCInfo = Ret;
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPWORDSetup",CaseNo,"Unknown Case");
        Ret = (LPWORD) NULL;
        break;
    }
    
    return(Ret);
}


void LPWORDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPWORD_VALID_ADDRESS:
    case LPWORD_MINUS_ONE:
        if ( *SCInfo != NULL )
            free(*SCInfo);
        break;
    default:
        break;
    }
    
    hConOut;
    hLog;
}



/************************************
L     PPPP  W   W  SSSS TTTTT RRRR
L     P   P W   W S       T   R   R
L     PPPP  W W W  SSS    T   RRRR
L     P     WW WW     S   T   R  R
LLLLL P     W   W SSSS    T   R   R
************************************/

LPWSTR
LPWSTRSetup(
            int CaseNo,
            LPVOID *SCInfo,
            HANDLE hLog,
            HANDLE hConOut
            )
{
    LPWSTR Ret;
    
    UNREFERENCED_PARAMETER( hConOut );
    
    *SCInfo = NULL;
    switch (CaseNo) {
        
    case LPWSTR_RANDOM:
        Ret = (LPWSTR) rand();
        break;
        
    case LPWSTR_MINUS_ONE:
        Ret = (LPWSTR) -1;
        break;
        
    case LPWSTR_46CHAR:
        Ret = (LPWSTR) LocalAlloc (LPTR, (50 * sizeof(WCHAR)));
        if (Ret != NULL)
            lstrcpyW (Ret, (LPCWSTR)L"The quick brown fox jumped over the lazy dogs");
        else
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break;
        
    case LPWSTR_DISPLAY:
        Ret = (LPWSTR) LocalAlloc (LPTR, (8 * sizeof(WCHAR)));
        if (Ret != NULL)
            lstrcpyW (Ret, (LPCWSTR)L"DISPLAY");
        else
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break;
        
    case LPWSTR_OPEN:
        Ret = (LPWSTR) LocalAlloc (LPTR, 5 * sizeof(WCHAR));
        if (Ret != NULL)
            lstrcpyW (Ret, (LPCWSTR)L"OPEN");
        else
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break;
        
    case LPWSTR_FILE_NAME:
        if ((Ret = (LPWSTR) LocalAlloc(LPTR, MAX_PATH*sizeof(WCHAR))) != NULL) {
            lstrcpyW(Ret, (LPCWSTR)L"D:\\WinNT\\system32\\setup.bmp");
        } else {
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        }
        *SCInfo = Ret;
        break;
        
    case LPWSTR_COMMAND_LINE:
        Ret = (LPWSTR) LocalAlloc( LPTR, 64 );
        if (Ret!=NULL)
            lstrcpyW( Ret, (LPCWSTR)L"a b c d e f g h i j k l m" );
        else
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break;
        
    case LPWSTR_INVALID_FILE_NAME:
        Ret = (LPWSTR) LocalAlloc( LPTR, 64 );
        if (Ret!=NULL)
            lstrcpyW( Ret, (LPCWSTR)L"[/]" );
        else
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break;
        
    case LPWSTR_NON_EXISTENT_DIRECTORY:
        Ret = (LPWSTR) LocalAlloc( LPTR, 100 );
        if (Ret!=NULL)
            lstrcpyW( Ret, (LPCWSTR)L"\\aaa\\bbb\\ccc\\ddd\\eee\\fff\\gomer\\hhh\\iii" );
        break;
        
    case LPWSTR_EMPTY:
    case LPWSTR_VALID_100:
        Ret = (LPWSTR) LocalAlloc( LPTR, 100*sizeof(WCHAR) );
        if (Ret == NULL)
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break;
        
    case LPWSTR_NULL:
        Ret = (LPWSTR) NULL;
        break;
        
    case LPWSTR_NULL_STRING:
        *SCInfo = Ret = (LPWSTR)malloc(sizeof L"");
        
        if (!Ret) {
            ErrorPrint(hConOut,
                hLog,
                "LPWSTRSetup",
                CaseNo,
                "GlobalAlloc failed");
            
            break;
        }
        
        *Ret = L'\0';
        
        break;
        
    case LPWSTR_2BYTE_BUFFER:
        // pointer to a buffer too small for the shortest cannonical
        // privilege or name/ascii verison of a display name, or username.
        
        if ( NULL == (Ret = GetNativeBuffer(SMALL_NATIVE_BUFFER_SIZE)) ) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "LocalAlloc failed!");
        }
        
        *SCInfo = Ret;
        
        break;
        
    case LPWSTR_INVALID: {
    /*
    * first DWORD of WSTR valid, non-null chars
        */
        
        SE_CLEANUP_INFO      *CleanupInfo;
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            INVALID_POINTER,
            0,
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                         }
        
                         /*
                         * a nice way to avoid duplicating code:
        */
        
#define  STRDUP( targ, src ) \
    (targ) = (LPWSTR)malloc( sizeof(src) ); \
    if ( !targ ) { \
    ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo, "malloc failed!" ); \
    } else { \
    lstrcpyW( targ, src ); \
    *SCInfo = (targ); \
                         }
        
    case LPWSTR_64K_BUFFER:
    /*
    * pointer to a 64k buffer
        */
        
        Ret = (LPWSTR)malloc(64*1024U);
        
        *SCInfo = Ret;
        
        if (!Ret) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "LocalAlloc failed!");
        }
        
        break;
        
        
    case LPWSTR_BUFFER_1K:
    /*
    * pointer to a 1k buffer
        */
        
        Ret = (LPWSTR)malloc(1024 * sizeof(WCHAR));
        
        *SCInfo = Ret;
        
        if (!Ret) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "LocalAlloc failed!");
        }
        
        break;
        
        
    case LPWSTR_FAT_FILENAME: {
        //
        // Valid FAT filename
        //
        WCHAR c;
        DWORD FSNameLen = 256, Drives, MaxComponentLength;
        WCHAR Path[] = L" :\\";
        LPWSTR FSName;
        
        Ret = NULL;
        
        FSName = (LPWSTR)malloc(FSNameLen);
        
        if (!FSName) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "GlobalAlloc failed.");
        }
        
        Drives = 0x03FFFFFF >> 2;   // skip 'a' and 'b'
        
        for (c = L'c'; Drives; c++, Drives >>= 1) {
            if (Drives & 1) {
                Path[0] = c;
                
                if (GetVolumeInformationW(Path,
                    NULL,         // lpVolumeNameBuffer
                    0,            // nVolumeNameSize
                    NULL,         // lpVolumeSerialNumber
                    &MaxComponentLength,
                    NULL,         // lpFileSystemFlags
                    FSName,
                    FSNameLen)) {
                    if (lstrcmpW(FSName, (LPCWSTR)L"FAT")) {
                    /*
                    * found
                        */
                        
                        break;
                    }
                } else {
                    // ChrisPi disabled 11-10-00 (GVI() can fail since we pass all possible drives to it)
                    
                    // ErrorPrint(hConOut, hLog, "LPWSTRSetup",
                    //     CaseNo,
                    //     "GetVolumeInformation failed.");
                }
            }
        }
        
        if (!Drives) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "No FAT drive found.");
        } else {
            STRDUP(Ret, (LPCWSTR)L" :\\testfile.fat");
            
            Ret[0] = c;
        }
        
        free(FSName);
        
        break;
                              }
        
    case LPWSTR_FILENAME_BOGUS:
    /*
    * filename with invalid chars
        */
        
        STRDUP(Ret, (LPCWSTR)L"bogus:test:filename");
        
        break;
        
    case LPWSTR_INVALID_1: {
    /*
    * first DWORD of WSTR valid, non-null chars
        */
        
        int         i;
        SE_CLEANUP_INFO      *CleanupInfo;
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            VALIDRANGE_POINTER,
            1 * sizeof (DWORD),
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        try {
            for (i = 0; i < (1 * sizeof (DWORD)) / sizeof (WCHAR); i++) {
                Ret[i] = L'c';
            }
        } except (EXCEPTION_EXECUTE_HANDLER) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
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
        
    case LPWSTR_INVALID_N: {
    /*
    * a STR with all but last DWORD of length valid, arbitraily make it
    * 10 DWORDS long
        */
        
        int         i;
        SE_CLEANUP_INFO      *CleanupInfo;
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            VALIDRANGE_POINTER,
            10 * sizeof (DWORD),
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        try {
            for (i = 0; i < (10 * sizeof (DWORD)) / sizeof (WCHAR); i++) {
                Ret[i] = L'd';
            }
        } except (EXCEPTION_EXECUTE_HANDLER) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
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
        
    case LPWSTR_NTFS_FILENAME: {
    /*
    * valid NTFS filename (includes unicode chars)
        */
        
        STRDUP(Ret, (LPCWSTR)L"D:\\foo");
        
        //   for (c = L'c'; c <= L'z'; c++) {
        //      Ret[0] = c;
        
        if (CreateFileW(Ret,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_FLAG_DELETE_ON_CLOSE,
            NULL)) {
            SetLastError(NO_ERROR);
            
            break;
        }
        //   }
        
        if (GetLastError() != NO_ERROR) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "CreateFile failed, file doesn't exist.");
            
            free(Ret);
            
            *SCInfo = Ret = NULL;
        }
        
        break;
                               }
        
    case LPWSTR_RELATIVE_FILENAME:
    /*
    * a relative path/filename
        */
        
        STRDUP(Ret, (LPCWSTR)L"foo");
        
        if (!CreateFileW(Ret,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_FLAG_DELETE_ON_CLOSE,
            NULL)) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "CreateFile failed, file doesn't exist.");
            
            free(Ret);
            
            *SCInfo = Ret = NULL;
        }
        
        break;
        
    case LPWSTR_TOO_LONG: {
    /*
    * a string of characters which is > 64Kb
        */
        
        SE_CLEANUP_INFO      *CleanupInfo;
        
#define  TOO_LONG_STRLEN      66600
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            VALIDRANGE_POINTER,
            TOO_LONG_STRLEN,
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "MakeBadPointer failed");
        } else {
            int         i;
            
            /*
            * This string is 64k *bytes* long, but we're stomping
            * characters in to the array.
            */
            
            for (i = 0; i < (TOO_LONG_STRLEN - 1) / sizeof (WCHAR); i++) {
                Ret[i] = L'a';
            }
            
            Ret[i] = L'\0';
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                          }
        
    case LPWSTR_UNC_FILENAME:
    /*
    * a UNC filename
        */
        
        STRDUP(Ret, (LPCWSTR)L"\\\\scratch\\scratch\\foo");
        
        if (!CreateFileW(Ret,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_FLAG_DELETE_ON_CLOSE,
            NULL)) {
            ErrorPrint(hConOut, hLog, "LPWSTRSetup", CaseNo,
                "CreateFile failed, file doesn't exist.");
            
            free(Ret);
            
            *SCInfo = Ret = NULL;
        }
        
        break;
        
    case LPWSTR_TEMP_FILE_NAME:
        {
            HANDLE hMem ;
            LPWSTR  lpMem ;
            
            hMem  = LocalAlloc( LPTR, 145 ) ;
            if( !hMem )
            {
                ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
                Ret = NULL ;
                *SCInfo = NULL ;
            }
            else
            {
                *SCInfo = (LPVOID)hMem ;
                
                lpMem = (LPWSTR)LocalLock( hMem ) ;
                if( !lpMem )
                {
                    ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"LocalAlloc failed!");
                    Ret = NULL ;
                }
                else
                {
                    if( !GetTempFileNameW( (LPCWSTR)L".", (LPCWSTR)L"LPWSTR", 0, lpMem ) )
                    {
                        ErrorPrint(hConOut,hLog,"LPSTRSetup",CaseNo,"GetTempFileName failed!");
                        Ret = NULL ;
                    }
                    else
                        Ret = lpMem ;
                }
            }
        }
        break ;
        
    case LPWSTR_TEMP_DIR:
        Ret = (LPWSTR) LocalAlloc (LPTR, (MAX_PATH * sizeof(WCHAR)));
        if (Ret != NULL)
            //GetTempPathW(MAXPATH, Ret);
            wcscpy(Ret, (LPCWSTR)L"z:\\");
        else
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break;
        
    case LPWSTR_ROOT_DIR:
        Ret = (LPWSTR) LocalAlloc (LPTR, (MAX_PATH * sizeof(WCHAR)));
        if (Ret != NULL)
            lstrcpyW (Ret, (LPCWSTR)L"t:\\");
        else
            ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo,"LocalAlloc failed!");
        *SCInfo = Ret;
        break;
        
    case LPWSTR_SRC_FILE_EXISTS:
    case LPWSTR_SRC_FILE_NOT_EXISTS:
    case LPWSTR_FILENAME_VALID:
    case LPWSTR_FILENAME_INVALID1:
    case LPWSTR_FILENAME_INVALID2:
    case LPWSTR_FILENAME_INVALID3:
    case LPWSTR_FILENAME_INVALID4:
    case LPWSTR_FILENAME_INVALID5:
    case LPWSTR_FILENAME_INVALID6:
    case LPWSTR_FILENAME_INVALID7:
    case LPWSTR_FILENAME_INVALID8:
    case LPWSTR_FILENAME_INVALID9:
    case LPWSTR_DIRNAME_INVALID1:
    case LPWSTR_DIRNAME_INVALID2:
    case LPWSTR_DIRNAME_INVALID3:
    case LPWSTR_DIRNAME_INVALID4:
    case LPWSTR_LOGICAL_DRIVE_STRING_BUFFER:
    case LPWSTR_TEMP_PATH_BUFFER:
    case LPWSTR_DEST_FILE_EXISTS:
    case LPWSTR_DEST_FILE_NOT_EXISTS:
    case LPWSTR_DIRNAME_VALID:
    case LPWSTR_PATH_EXISTS:
    case LPWSTR_PATH_NOT_EXISTS:
    case LPWSTR_PREFIX_VALID:
    case LPWSTR_PREFIX_TOO_LONG:
    case LPWSTR_VALID_PATH_BUFFER:
    case LPWSTR_VALID_ROOT_PATHNAME:
    case LPWSTR_INVALID_ROOT_PATHNAME:
    case LPWSTR_FILE_EXISTS:
    case LPWSTR_FILE_NOT_EXISTS:
    case LPWSTR_FILE_READONLY:
    case LPWSTR_VOLUME_NAME_BUFFER:
    case LPWSTR_FILE_SYSTEM_NAME_BUFFER:
    case LPWSTR_SEARCH_PATH_ILL_FORMED:
    case LPWSTR_SEARCH_PATH_BAD_CHARS:
    case LPWSTR_FILE_IN_CURR_DIR_NO_EXT:
    case LPWSTR_EXT_FILE_IN_CURR_DIR:
    case LPWSTR_EXT_ILL_FORMED:
    case LPWSTR_EXT_BAD_CHARS:
    case LPWSTR_FILE_NOT_EXISTS_ON_PATH:
        Ret = FIOLpstrSetupW(CaseNo, SCInfo, hLog, hConOut);
        break;
        
    default:
        Ret = (LPWSTR)NULL;
        ErrorPrint(hConOut,hLog,"LPWSTRSetup",CaseNo, "Unknown Case");
        break;
    }
    
    return Ret;
}


void
LPWSTRCleanup(
              int                     CaseNo,
              LPVOID                  *SCInfo,
              HANDLE                  hLog,
              HANDLE                  hConOut
              )
{
    switch(CaseNo) {
    case LPWSTR_64K_BUFFER:
    case LPWSTR_BUFFER_1K:
    case LPWSTR_FAT_FILENAME:
    case LPWSTR_FILENAME_BOGUS:
    case LPWSTR_NON_EXISTENT_DIRECTORY:
    case LPWSTR_NTFS_FILENAME:
    case LPWSTR_NULL_STRING:
    case LPWSTR_RELATIVE_FILENAME:
    case LPWSTR_UNC_FILENAME:
        
    /*
    * These allocations were made with malloc(), free()
    * them.
        */
        
        free(*SCInfo);
        
        break;
        
    case LPWSTR_46CHAR:
    case LPWSTR_DISPLAY:
    case LPWSTR_OPEN:
    case LPWSTR_INVALID_FILE_NAME:
    case LPWSTR_NON_EXISTANT_DIRECTORY:
    case LPWSTR_VALID_100:
    case LPWSTR_COMMAND_LINE:
    case LPWSTR_EMPTY:
    case LPWSTR_TEMP_DIR:
    case LPWSTR_ROOT_DIR:
    case LPWSTR_FILE_NAME:
        if ((*SCInfo)!=NULL) {
            if (LocalFree((HANDLE)(*SCInfo))!=NULL)
                ErrorPrint(hConOut,hLog,"LPWSTRCleanup",CaseNo,"LocalFree failed!");
        }
        break;
        
    case LPWSTR_TEMP_FILE_NAME:
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
        
    case LPWSTR_2BYTE_BUFFER:
        
    /*
    * This allocation was made by calling
    * GetNativeBuffer(), FreeNativeBuffer() them.
        */
        
        FreeNativeBuffer(*SCInfo, SMALL_NATIVE_BUFFER_SIZE);
        
        break;
        
    case LPWSTR_INVALID:
    case LPWSTR_INVALID_1:
    case LPWSTR_TOO_LONG:
    case LPWSTR_INVALID_N:
        
    /*
    * These allocations were made by calling
    * MakeBadPointer(), call MakeBadPointer again telling
    * it to clean up.
        */
        
        MakeBadPointer(NULL,
            (SE_CLEANUP_INFO **)SCInfo,
            CLEANUP,
            0,
            hConOut,
            hLog);
        
        if (*SCInfo) {
            ErrorPrint(hConOut,
                hLog,
                "LPSTRCleanup",
                CaseNo,
                "Cleanup failed.");
        }
        
        break;
        
    case LPWSTR_FILENAME_INVALID1:
    case LPWSTR_FILENAME_INVALID2:
    case LPWSTR_FILENAME_INVALID3:
    case LPWSTR_FILENAME_INVALID4:
    case LPWSTR_FILENAME_INVALID5:
    case LPWSTR_FILENAME_INVALID6:
    case LPWSTR_FILENAME_INVALID7:
    case LPWSTR_FILENAME_INVALID8:
    case LPWSTR_FILENAME_INVALID9:
    case LPWSTR_DIRNAME_INVALID1:
    case LPWSTR_DIRNAME_INVALID2:
    case LPWSTR_DIRNAME_INVALID3:
    case LPWSTR_DIRNAME_INVALID4:
        if ( *SCInfo ) {
            UNICODE_STRING Unicode;
            Unicode.Buffer = *SCInfo;
            RtlFreeUnicodeString( &Unicode );
        }
        break;
        
    case LPWSTR_NULL:
    default:
        
    /*
    * There was no allocation made for these cases.
        */
        
        break;
    }
}


WCHAR WCHARSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    WCHAR Ret;
    
    switch(CaseNo) {
        
    case WCHAR_A:
        Ret = L'a';
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"WCHARSetup",CaseNo,"Unknown Case");
        Ret = (WCHAR)0;
        break;
    }
    
    return(Ret);
    
    SCInfo;
}


void WCHARCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    hConOut;
    hLog;
    SCInfo;
}


WORD WORDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    WORD Ret;
    
    switch(CaseNo) {
        
    case WORD_NULL:
        Ret = (WORD)0;
        break;
    case WORD_MINUS_ONE:
        Ret = (WORD) -1;
        break;
    case WORD_RANDOM:
        Ret = (WORD) rand();
        break;
    case WORD_ONE:
        Ret = (WORD) 1;
        break;
    case WORD_VALID_DOS_DATE:
        Ret = MAKE_DOS_DATE(11, 14, 2);   // Nov 14, 1982
        break;
    case WORD_INVALID_DOS_DATE:
        Ret = MAKE_DOS_DATE(13, 32, 127); // each out of range
        break;
    case WORD_VALID_DOS_TIME:
        Ret = MAKE_DOS_TIME(22, 20, 12);  // 10:20 and 24 seconds
        break;
    case WORD_INVALID_DOS_TIME:
        Ret = MAKE_DOS_TIME(24, 60, 30);  // each out of range
        break;
    case WORD_ZERO:
        Ret=0;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"WORDSetup",CaseNo,"Unknown Case");
        Ret = (WORD)0;
        break;
    }
    
    return(Ret);
    
    SCInfo;
}


void WORDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    default:
        break;
    }
    
    SCInfo;
    hLog;
    hConOut;
}
