//---------------------------------------------------------------------------
//
//  MODULE: filever.c
//  AUTHOR: Steveko
//  DATE:  03-27-92
//
//  Copyright (c) 1992 Microsoft Corporation
//
//  This module contains the fileio and versioning functions to make and
//  destroy parameters for BadMan.
//
//---------------------------------------------------------------------------

#include <setcln.h>
#include <d_cases.bmh>
#include <h_cases.bmh>
#include <s_cases.bmh>
#include <w_cases.bmh>

#include <filever.h>
#include <winver.h>
#include <filutil.h>

#ifdef DEBUG
#define FIODebugPrint  SCPrintf
#else
#define FIODebugPrint
#endif

static HANDLE FIOMakeFile(int     CaseNo,
                          LPVOID *SCInfo,
                          HANDLE  hLog,
                          HANDLE  hConOut,
                          LPSTR   lpFileName,
                          DWORD   dwFileAttribs);

static HANDLE FIOMakeFileW(int     CaseNo,
                           LPVOID *SCInfo,
                           HANDLE  hLog,
                           HANDLE  hConOut,
                           LPWSTR  lpFileName,
                           DWORD   dwFileAttribs);

static BOOL FIOCloseDeleteFile(int     CaseNo,
                               LPVOID *SCInfo,
                               HANDLE  hLog,
                               HANDLE  hConOut,
                               HANDLE  hFile,
                               LPSTR   lpFileName,
                               BOOL    bSilent);

static BOOL FIOCloseDeleteFileW(int     CaseNo,
                                LPVOID *SCInfo,
                                HANDLE  hLog,
                                HANDLE  hConOut,
                                HANDLE  hFile,
                                LPWSTR  lpFileName,
                                BOOL    bSilent);

//---------------------------------------------------------------------------
//
//  Function for support HANDLESetup cases for fileio and versioning.
//
//  Returns INVALID_HANDLE_VALUE if any errors.
//
//---------------------------------------------------------------------------

HANDLE FIOHandleSetup(int     CaseNo,
                      LPVOID *SCInfo,
                      HANDLE  hLog,
                      HANDLE  hConOut)
{
  HANDLE          hRet;
  WIN32_FIND_DATA wfd;

  hRet = INVALID_HANDLE_VALUE;

  FIODebugPrint(hConOut, "\nFIOHandleSetup: case %d\n", CaseNo);

  switch (CaseNo) {

    case HANDLE_NORMAL_FILE:

      // make the file with normal attribs.

      hRet = FIOMakeFile(CaseNo,
                         SCInfo,
                         hLog,
                         hConOut,
                         FIO_DEFAULT_FILE_NAME,
                         FILE_ATTRIBUTE_NORMAL);

      if ( hRet == INVALID_HANDLE_VALUE )
        break;

      // open the file

      hRet = CreateFile(FIO_DEFAULT_FILE_NAME,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);

      if ( hRet == INVALID_HANDLE_VALUE ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Could not open normal file");
        break;
      }

      break;

    case HANDLE_READONLY_FILE:

      // make the file with normal attribs.

      hRet = FIOMakeFile(CaseNo,
                         SCInfo,
                         hLog,
                         hConOut,
                         FIO_DEFAULT_FILE_NAME,
                         FILE_ATTRIBUTE_NORMAL);

      if ( hRet == INVALID_HANDLE_VALUE )
        break;

      // open the file

      hRet = CreateFile(FIO_DEFAULT_FILE_NAME,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);

      if ( hRet == INVALID_HANDLE_VALUE ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Could not open normal file");
        break;
      }

      break;

    case HANDLE_CLOSED_FILE:

      // make the file with normal attribs.

      hRet = FIOMakeFile(CaseNo,
                         SCInfo,
                         hLog,
                         hConOut,
                         FIO_DEFAULT_FILE_NAME,
                         FILE_ATTRIBUTE_NORMAL);

      if ( hRet == INVALID_HANDLE_VALUE )
        break;

      // open the file

      hRet = CreateFile(FIO_DEFAULT_FILE_NAME,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);


      if ( hRet == INVALID_HANDLE_VALUE ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Could not open file");
        break;
      }

      // close the file

      if ( ! CloseHandle(hRet) ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Error closing file");
        break;
      }

      break;

    case HANDLE_FINDFILE:

      // get handle

      hRet = FindFirstFile("t:\\*.*", &(wfd));

      if ( hRet == INVALID_HANDLE_VALUE ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Error getting file search handle for *.*");
        break;
      }

      break;

    case HANDLE_CLOSED_FINDFILE:

      // get handle

      hRet = FindFirstFile("t:\\*.*", &(wfd));

      if ( hRet == INVALID_HANDLE_VALUE ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Error getting file search handle for *.*");
        break;
      }

      // close handle

      if ( ! FindClose(hRet) ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Error closing file search handle");
        break;
      }

      break;

    case HANDLE_NORMAL_ASYNC_FILE:

      // make a normal file

      hRet = FIOMakeFile(CaseNo,
                         SCInfo,
                         hLog,
                         hConOut,
                         FIO_DEFAULT_FILE_NAME,
                         FILE_ATTRIBUTE_NORMAL);

      if ( hRet == INVALID_HANDLE_VALUE )
        break;

      // open the file overlapped

      hRet = CreateFile(FIO_DEFAULT_FILE_NAME,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        (HANDLE) NULL);

      if ( hRet == INVALID_HANDLE_VALUE ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Could not open normal async file");
        break;
      }

      break;

    case HANDLE_READONLY_ASYNC_FILE:

      // make a readonly file

      hRet = FIOMakeFile(CaseNo,
                         SCInfo,
                         hLog,
                         hConOut,
                         FIO_DEFAULT_FILE_NAME,
                         FILE_ATTRIBUTE_READONLY);

      if ( hRet == INVALID_HANDLE_VALUE )
        break;

      // open the file overlapped

      hRet = CreateFile(FIO_DEFAULT_FILE_NAME,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        (HANDLE) NULL);

      if ( hRet == INVALID_HANDLE_VALUE ) {

        ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
             "Could not open readonly async file");
        break;
      }

      break;

    default:
      ErrorPrint(hConOut, hLog, "FIOHandleSetup", CaseNo,
                 "unknown case");
      break;
  }

  *SCInfo = (LPVOID) hRet;

  return(hRet);
}

//---------------------------------------------------------------------------
//
//  Function for support HANDLECleanup cases for fileio and versioning.
//
//  Returns INVALID_HANDLE_VALUE if any errors.
//
//---------------------------------------------------------------------------

VOID FIOHandleCleanup(int     CaseNo,
                      LPVOID *SCInfo,
                      HANDLE  hLog,
                      HANDLE  hConOut)
{
  BOOL bval;

  FIODebugPrint(hConOut, "\nFIOHandleCleanup: case %d\n", CaseNo);

  switch ( CaseNo ) {

    case HANDLE_NORMAL_FILE:
    case HANDLE_READONLY_FILE:
    case HANDLE_NORMAL_ASYNC_FILE:
    case HANDLE_READONLY_ASYNC_FILE:

      bval = FIOCloseDeleteFile(CaseNo,
                                SCInfo,
                                hLog,
                                hConOut,
                                (HANDLE) *SCInfo,
                                FIO_DEFAULT_FILE_NAME,
                                FALSE);
      break;

    case HANDLE_FINDFILE:

      if ( (HANDLE) *SCInfo != INVALID_HANDLE_VALUE )
        if ( ! FindClose((HANDLE) *SCInfo) )
          ErrorPrint(hConOut, hLog, "FIOHandleCleanup", CaseNo,
                     "Error closing file search handle");
      break;

    case HANDLE_CLOSED_FINDFILE:
      break;

    default:
      break;
  }
}

//---------------------------------------------------------------------------
//
//  Function for support DWORDSetup cases for fileio and versioning.
//
//---------------------------------------------------------------------------

DWORD FIODwordSetup(int     CaseNo,
                    LPVOID *SCInfo,
                    HANDLE  hLog,
                    HANDLE  hConOut)
{
  DWORD dwRet = 0;

  switch ( CaseNo ) {

    /* CreateFile generic flags. */

    case DWORD_GENERIC_READ_AND_WRITE:

      dwRet = DWORD_GENERIC_READ_AND_WRITE;
      break;

    case DWORD_GENERIC_INVALID_VALUE:

      dwRet = ~ ( GENERIC_READ | GENERIC_WRITE );
      break;

    /* CreateFile sharing modes. */

    case DWORD_FILE_SHARE_READ_AND_WRITE:

      dwRet = FILE_SHARE_READ | FILE_SHARE_WRITE;
      break;

    case DWORD_FILE_SHARE_READ_WRITE_DELETE:
      dwRet = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
      break;

    case DWORD_FILE_SHARE_INVALID_VALUE:

      dwRet = (DWORD)~ (FILE_SHARE_READ | FILE_SHARE_WRITE);
      break;

    /* CreateFile creation dispositions. */

    case DWORD_DISPOSITION_CREATE_FILE_ALWAYS:

      dwRet = CREATE_ALWAYS;
      break;

    case DWORD_DISPOSITION_INVALID_VALUE:

      dwRet = (DWORD)~ ( CREATE_NEW    |
                  CREATE_ALWAYS |
                  OPEN_EXISTING |
                  OPEN_ALWAYS   |
                  TRUNCATE_EXISTING );
      break;

    case DWORD_FILE_FLAGS_NORMAL:

      dwRet = FILE_ATTRIBUTE_NORMAL;
      break;

    case DWORD_FILE_FLAGS_INVALID_VALUE:

      dwRet = ~ ( FILE_ATTRIBUTE_NORMAL   |
                  FILE_ATTRIBUTE_ARCHIVE  |
                  FILE_ATTRIBUTE_READONLY |
                  FILE_ATTRIBUTE_HIDDEN   |
                  FILE_ATTRIBUTE_SYSTEM   |
                  FILE_FLAG_WRITE_THROUGH |
                  FILE_FLAG_OVERLAPPED    |
                  FILE_FLAG_NO_BUFFERING  |
                  FILE_FLAG_RANDOM_ACCESS |
                  FILE_FLAG_SEQUENTIAL_SCAN );
      break;

    case DWORD_READWRITE_VALID_BUFFER_SIZE:

      dwRet = FIO_READWRITE_BUFFER_SIZE;
      break;

    case DWORD_READWRITE_INVALID_BUFFER_SIZE:

      dwRet = 0xffffffff;
      break;

    case DWORD_VOLUME_NAME_BUFFER_LENGTH:

      dwRet = FIO_VOLUME_NAME_BUFFER_LENGTH;
      break;

    case DWORD_FILE_SYSTEM_NAME_BUFFER_LENGTH:

      dwRet = FIO_FILE_SYSTEM_NAME_BUFFER_LENGTH;
      break;

    case DWORD_NORMAL_FILE_ATTRIBUTES:

      dwRet = FILE_ATTRIBUTE_NORMAL;
      break;

    case DWORD_INVALID_FILE_ATTRIBUTES:

      dwRet = (DWORD)~ ( FILE_ATTRIBUTE_NORMAL   |
                  FILE_ATTRIBUTE_ARCHIVE  |
                  FILE_ATTRIBUTE_READONLY |
                  FILE_ATTRIBUTE_HIDDEN   |
                  FILE_ATTRIBUTE_SYSTEM );
      break;

    case DWORD_VER_INVALID_FIND_FLAGS:

      dwRet = (DWORD)~ ( VFFF_ISSHAREDFILE );
      break;

    case DWORD_VER_INVALID_INST_FLAGS:

      dwRet = (DWORD)~ ( VIFF_FORCEINSTALL |
                  VIFF_DONTDELETEOLD );
      break;

    case DWORD_VER_VALID_INFO_LENGTH:

#ifdef IT_EVER_MAKES_IT_TO_A_BUILD

      // get the ver info size from the file

      dwRet = GetVersionInfoSize(VER_DEFAULT_FILE_WITH_VER_INFO,
                                  &dwBogusHandle);
      if ( dwRet == 0 ) {
        ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                   "error getting size of version info\n");
        break;
      }
#endif
      dwRet = 0;
      break;

    case DWORD_VER_LANG_US_ENGLISH:

      dwRet = 0x409;
      break;

    case DWORD_VER_LANG_UNDEFINED:

      dwRet = 0xffffffff;
      break;

    case DWORD_VER_LANGUAGE_BUFFER_SIZE:

      dwRet = VER_SIZE_LANGUAGE_BUFFER;
      break;

    default:
      ErrorPrint(hConOut, hLog, "FIODwordSetup", CaseNo, "unknown case");
      break;
  }

  return(dwRet);

  *SCInfo;   // shut-up compiler
}

//---------------------------------------------------------------------------
//
//  Function for fileio LPSTRSetup cases for fileio and versioning
//
//---------------------------------------------------------------------------

LPSTR FIOLpstrSetup(int     CaseNo,
                    LPVOID *SCInfo,
                    HANDLE  hLog,
                    HANDLE  hConOut)
{
    LPSTR lpString = NULL;
    BOOL  bRet;
    DWORD dwRet;
    
    switch ( CaseNo ) {
        
    case LPSTR_FILENAME_VALID:  // used only by create and open
        
        lpString = FIO_DEFAULT_FILE_TO_CREATE;
        
        FIOCloseDeleteFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE);
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID1:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADFNAME_UNPRINT_CHARS_ROOT, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName1");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID2:
        
        dwRet=fGetNextBadFileName(&lpString,
            FIO_BADFNAME_UNPRINT_CHARS_SUBDIR, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName2");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID3:
        
        dwRet=fGetNextBadFileName(&lpString,
            FIO_BADFNAME_UNPRINT_CHARS_DIRNAME, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName3");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID4:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADFNAME_PRINT_CHARS_ROOT, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName4");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID5:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADFNAME_PRINT_CHARS_SUBDIR, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName5");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID6:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADFNAME_PRINT_CHARS_DIRNAME, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName6");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID7:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADFNAME_INVALID_DRIVE_NAME, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName7");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID8:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADFNAME_ILL_FORMED, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName8");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILENAME_INVALID9:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADFNAME_NAME_TOO_LONG, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad fileName9");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_DIRNAME_INVALID1:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADDIRNAME_UNPRINT_CHARS, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad DirName1");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_DIRNAME_INVALID2:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADDIRNAME_PRINT_CHARS, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad DirName2");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_DIRNAME_INVALID3:
        
        dwRet=fGetNextBadFileName(&lpString,
            FIO_BADDIRNAME_INVALID_DRIVE_NAME, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad DirName3");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_DIRNAME_INVALID4:
        
        dwRet=fGetNextBadFileName(&lpString, FIO_BADDIRNAME_ILL_FORMED, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad DirName4");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_SRC_FILE_EXISTS:
        
        lpString = FIO_DEFAULT_SRC_FILE_NAME;
        
        FIOMakeFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_NORMAL);
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_SRC_FILE_NOT_EXISTS:
        
        lpString = FIO_DEFAULT_SRC_FILE_NAME;
        
        FIOCloseDeleteFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE);
        *SCInfo = lpString;
        break;
        
    case LPSTR_DEST_FILE_EXISTS:
        
        lpString = FIO_DEFAULT_DEST_FILE_NAME;
        
        FIOMakeFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_NORMAL);
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_DEST_FILE_NOT_EXISTS:
        
        lpString = FIO_DEFAULT_DEST_FILE_NAME;
        
        FIOCloseDeleteFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE);
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILE_EXISTS:
        
        lpString = FIO_DEFAULT_FILE_NAME;
        
        FIOMakeFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_NORMAL);
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILE_NOT_EXISTS:
        
        lpString = FIO_DEFAULT_FILE_NAME;
        
        FIOCloseDeleteFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE);
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILE_READONLY:
        
        lpString = FIO_DEFAULT_FILE_NAME;
        
        FIOMakeFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_READONLY);
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_DIRNAME_VALID:
        
        lpString = "t:\\";
        break;
        
    case LPSTR_PATH_EXISTS:
        
        lpString = "z:\\xyzabc.tst";
        
        bRet = CreateDirectory(lpString, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS )
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
            "Error creating directory\n");
        
        break;
        
    case LPSTR_PATH_NOT_EXISTS:
        
        lpString = "xyz123.abc";  // not likely
        break;
        
    case LPSTR_PREFIX_VALID:
        
        lpString = "ppp";
        break;
        
    case LPSTR_PREFIX_TOO_LONG:
        
        lpString = "ppppppppppppppp";
        break;
        
    case LPSTR_VALID_PATH_BUFFER:
        
        if ( (lpString = (LPSTR) LocalAlloc(LPTR, MAX_PATH)) == NULL )
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
            "Error allocating memory");
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_VALID_ROOT_PATHNAME:
        
        lpString = "t:\\";
        break;
        
    case LPSTR_INVALID_ROOT_PATHNAME:
        
        lpString = "c:\\tmp";
        break;
        
    case LPSTR_VOLUME_NAME_BUFFER:
        
        if((lpString = (LPSTR) LocalAlloc(LPTR, FIO_VOLUME_NAME_BUFFER_LENGTH))
            == NULL){
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error allocating memory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_FILE_SYSTEM_NAME_BUFFER:
        
        if((lpString = (LPSTR) LocalAlloc(LPTR,
            FIO_FILE_SYSTEM_NAME_BUFFER_LENGTH)) == NULL){
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error allocating memory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_VER_SRC_FILE_EXISTS:
        
        bRet = CreateDirectory(VER_DEFAULT_SRC_DIR, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error creating directory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        lpString = *SCInfo = VER_DEFAULT_SRC_FILE;
        
        FIOMakeFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_NORMAL);
        break;
        
    case LPSTR_VER_SRC_FILE_NOT_EXISTS:
        
        bRet = CreateDirectory(VER_DEFAULT_SRC_DIR, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error creating directory");
            lpString = NULL;
            *SCInfo  = NULL;
            break;
        }
        
        lpString = *SCInfo = VER_DEFAULT_SRC_FILE;
        
        FIOCloseDeleteFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE);
        break;
        
    case LPSTR_VER_SRC_DIR_EXISTS:
        
        lpString = *SCInfo = VER_DEFAULT_SRC_DIR;
        
        bRet = CreateDirectory(lpString, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error creating directory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        break;
        
    case LPSTR_VER_SRC_DIR_NOT_EXISTS:
        
        lpString = *SCInfo = VER_DEFAULT_DIR_NOT_EXISTS;
        break;
        
    case LPSTR_VER_DEST_FILE_EXISTS:
        
        bRet = CreateDirectory(VER_DEFAULT_DEST_DIR, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error creating directory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        lpString = *SCInfo = VER_DEFAULT_DEST_FILE;
        
        FIOMakeFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_NORMAL);
        
        *SCInfo = lpString;
        break;
        
    case LPSTR_VER_DEST_FILE_NOT_EXISTS:
        
        bRet = CreateDirectory(VER_DEFAULT_DEST_DIR, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error creating directory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        lpString = *SCInfo = VER_DEFAULT_DEST_FILE;
        
        FIOCloseDeleteFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE);
        break;
        
    case LPSTR_VER_DEST_DIR_EXISTS:
        
        lpString = *SCInfo = VER_DEFAULT_DEST_DIR;
        
        bRet = CreateDirectory(lpString, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error creating directory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        break;
        
    case LPSTR_VER_DEST_DIR_NOT_EXISTS:
        
        lpString = *SCInfo = VER_DEFAULT_DIR_NOT_EXISTS;
        break;
        
    case LPSTR_NULL_STRING:
        
        lpString = *SCInfo = "";
        break;
        
        /*case LPSTR_WINDOWS_DIR:
        
          if ( (lpString = (LPSTR) LocalAlloc(LPTR, MAX_PATH)) == NULL ) {
          ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
          "Error allocating memory");
          *SCInfo = NULL;
          break;
          }
          
            if ( GetWindowsDirectory(lpString, MAX_PATH) == 0 ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
            "Error getting windows directory");
            *SCInfo = NULL;
            break;
            }
            
              *SCInfo = lpString;
        break;*/
        
    case LPSTR_VER_APP_DIR:
        
        lpString = *SCInfo = VER_DEFAULT_APP_DIR;
        
        bRet = CreateDirectory(lpString, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
                "Error creating directory");
            lpString = NULL;
            break;
        }
        
        break;
        
    case LPSTR_VER_VALID_LANGUAGE_BUFFER:
        
        if ( (lpString = *SCInfo = malloc(VER_SIZE_LANGUAGE_BUFFER)) == NULL )
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo,
            "Error allocating memory\n");
        break;
        
    case LPSTR_VER_SUB_BLOCK_EXISTS:
        
        lpString = *SCInfo = "\\StringFileInfo";
        break;
        
    case LPSTR_VER_SUB_BLOCK_NOT_EXISTS:
        
        lpString = *SCInfo = "\\notexists";
        break;
        
    case LPSTR_SEARCH_PATH_ILL_FORMED:
        
        lpString = *SCInfo = "::+t:\\nt\\windows--";
        break;
        
    case LPSTR_SEARCH_PATH_BAD_CHARS:
        
        lpString = *SCInfo = "t:\\xyz\x0f\xfe";
        break;
        
    case LPSTR_FILE_IN_CURR_DIR_NO_EXT:
        
        lpString = "good";
        *SCInfo  = "good.txt";
        
        FIOMakeFile(CaseNo,
            SCInfo,
            hLog,
            hConOut,
            (LPSTR) *SCInfo,
            FILE_ATTRIBUTE_NORMAL);
        break;
        
    case LPSTR_EXT_FILE_IN_CURR_DIR:
        
        lpString = *SCInfo = ".txt";
        break;
        
    case LPSTR_EXT_ILL_FORMED:
        
        lpString = *SCInfo = "txt";
        break;
        
    case LPSTR_EXT_BAD_CHARS:
        
        lpString = *SCInfo = ".\xfe\x0f";
        break;
        
    case LPSTR_FILE_NOT_EXISTS_ON_PATH:
        
        lpString = *SCInfo = "t:\\noton.pth";  // hope root not on path or curr
        // dir
        break;
        
    default:
        ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "unknown case");
        break;
  }
  
  return(lpString);
}


LPWSTR
FIOLpstrSetupW(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    LPWSTR lpString = NULL;
    BOOL  bRet;
    DWORD dwRet;
    
    switch ( CaseNo ) {
        
    case LPWSTR_FILENAME_VALID:  // used only by create and open
        lpString = (LPWSTR)FIO_DEFAULT_FILE_TO_CREATE_W;

        FIOCloseDeleteFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE
            );

        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID1:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_UNPRINT_CHARS_ROOT, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName1");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID2:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_UNPRINT_CHARS_SUBDIR, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName2");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID3:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_UNPRINT_CHARS_DIRNAME, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName3");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID4:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_PRINT_CHARS_ROOT, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName4");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID5:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_PRINT_CHARS_SUBDIR, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName5");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID6:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_PRINT_CHARS_DIRNAME, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName6");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID7:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_INVALID_DRIVE_NAME, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName7");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID8:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_ILL_FORMED, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName8");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILENAME_INVALID9:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADFNAME_NAME_TOO_LONG, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad fileName9");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_DIRNAME_INVALID1:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADDIRNAME_UNPRINT_CHARS, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad DirName1");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_DIRNAME_INVALID2:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADDIRNAME_PRINT_CHARS, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad DirName2");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_DIRNAME_INVALID3:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADDIRNAME_INVALID_DRIVE_NAME, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetup", CaseNo, "Error getting bad DirName3");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_DIRNAME_INVALID4:
        dwRet = fGetNextBadFileNameW(&lpString, FIO_BADDIRNAME_ILL_FORMED, 0);
        
        if ( dwRet != FIO_NO_ERR ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error getting bad DirName4");
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_SRC_FILE_EXISTS:
        lpString = (LPWSTR)FIO_DEFAULT_SRC_FILE_NAME_W;
        
        FIOMakeFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_NORMAL
            );
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_SRC_FILE_NOT_EXISTS:
        lpString = (LPWSTR)FIO_DEFAULT_SRC_FILE_NAME_W;
        
        FIOCloseDeleteFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE
            );

        *SCInfo = lpString;
        break;
        
    case LPWSTR_DEST_FILE_EXISTS:
        lpString = (LPWSTR)FIO_DEFAULT_DEST_FILE_NAME_W;
        
        FIOMakeFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_NORMAL
            );
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_DEST_FILE_NOT_EXISTS:
        lpString = (LPWSTR)FIO_DEFAULT_DEST_FILE_NAME_W;
        
        FIOCloseDeleteFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE
            );

        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILE_EXISTS:
        lpString = (LPWSTR)FIO_DEFAULT_FILE_NAME_W;
        
        FIOMakeFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_NORMAL
            );
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILE_NOT_EXISTS:
        lpString = (LPWSTR)FIO_DEFAULT_FILE_NAME_W;
        
        FIOCloseDeleteFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            INVALID_HANDLE_VALUE,
            lpString,
            TRUE
            );

        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILE_READONLY:
        lpString = (LPWSTR)FIO_DEFAULT_FILE_NAME_W;
        
        FIOMakeFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            lpString,
            FILE_ATTRIBUTE_READONLY
            );
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_DIRNAME_VALID:
        lpString = (LPWSTR)L"t:\\";
        break;
        
    case LPWSTR_PATH_EXISTS:
        lpString = (LPWSTR)L"z:\\xyzabc.tst";
        bRet = CreateDirectoryW(lpString, NULL);
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS )
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error creating directory\n");
        break;
        
    case LPWSTR_PATH_NOT_EXISTS:
        lpString = (LPWSTR)L"xyz123.abc";  // not likely
        break;
        
    case LPWSTR_PREFIX_VALID:
        lpString = (LPWSTR)L"ppp";
        break;
        
    case LPWSTR_PREFIX_TOO_LONG:
        lpString = (LPWSTR)L"ppppppppppppppp";
        break;
        
    case LPWSTR_VALID_PATH_BUFFER:
        if ( (lpString = (LPWSTR)LocalAlloc(LPTR, sizeof(WCHAR)*MAX_PATH)) == NULL ) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error allocating memory");
        }
        *SCInfo = lpString;
        break;
        
    case LPWSTR_VALID_ROOT_PATHNAME:
        lpString = (LPWSTR)L"t\\";
        break;
        
    case LPWSTR_INVALID_ROOT_PATHNAME:
        lpString = (LPWSTR)L"c:\\tmp";
        break;
        
    case LPWSTR_VOLUME_NAME_BUFFER:
        if ((lpString = (LPWSTR)LocalAlloc(LPTR, sizeof(WCHAR)*FIO_VOLUME_NAME_BUFFER_LENGTH)) == NULL) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error allocating memory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_FILE_SYSTEM_NAME_BUFFER:
        if((lpString = (LPWSTR)LocalAlloc(LPTR, sizeof(WCHAR)*FIO_FILE_SYSTEM_NAME_BUFFER_LENGTH)) == NULL) {
            ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "Error allocating memory");
            lpString = *SCInfo = NULL;
            break;
        }
        
        *SCInfo = lpString;
        break;
        
    case LPWSTR_SEARCH_PATH_ILL_FORMED:
        lpString = *SCInfo = L"::+t:\\nt\\windows--";
        break;
        
    case LPWSTR_SEARCH_PATH_BAD_CHARS:
        lpString = *SCInfo = L"z:\\xyz\x0f\xfe";
        break;
        
    case LPWSTR_FILE_IN_CURR_DIR_NO_EXT:
        lpString = (LPWSTR)L"good";
        *SCInfo  = L"good.txt";
        
        FIOMakeFileW(
            CaseNo,
            SCInfo,
            hLog,
            hConOut,
            (LPWSTR)*SCInfo,
            FILE_ATTRIBUTE_NORMAL
            );
        break;
        
    case LPWSTR_EXT_FILE_IN_CURR_DIR:
        lpString = *SCInfo = L".txt";
        break;
        
    case LPWSTR_EXT_ILL_FORMED:
        lpString = *SCInfo = L"txt";
        break;
        
    case LPWSTR_EXT_BAD_CHARS:
        lpString = *SCInfo = L".\xfe\x0f";
        break;
        
    case LPWSTR_FILE_NOT_EXISTS_ON_PATH:
        lpString = *SCInfo = L"t:\\noton.pth";  // hope root not on path or curr dir
        break;
        
    default:
        ErrorPrint(hConOut, hLog, "FIOLpstrSetupW", CaseNo, "unknown case");
        break;
  }
  
  return lpString;
}


//---------------------------------------------------------------------------
//
//  Cleanup function for Fileio LPSTR's
//
//---------------------------------------------------------------------------

VOID FIOLpstrCleanup(int     CaseNo,
                     LPVOID *SCInfo,
                     HANDLE  hLog,
                     HANDLE  hConOut)
{
  switch ( CaseNo ) {

    case LPSTR_FILENAME_VALID:
    case LPSTR_SRC_FILE_EXISTS:
    case LPSTR_SRC_FILE_NOT_EXISTS:
    case LPSTR_DEST_FILE_EXISTS:
    case LPSTR_DEST_FILE_NOT_EXISTS:
    case LPSTR_FILE_EXISTS:
    case LPSTR_FILE_READONLY:
    case LPSTR_FILE_IN_CURR_DIR_NO_EXT:

      FIOCloseDeleteFile(CaseNo,
                         SCInfo,
                         hLog,
                         hConOut,
                         INVALID_HANDLE_VALUE,
                         (LPSTR) *SCInfo,
                         FALSE);
      break;

    case LPSTR_VALID_PATH_BUFFER:
    case LPSTR_VOLUME_NAME_BUFFER:
    case LPSTR_FILE_SYSTEM_NAME_BUFFER:
    case LPSTR_VER_VALID_LANGUAGE_BUFFER:
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

      if ( *SCInfo != NULL )
        LocalFree((HANDLE) *SCInfo);
      break;

    default:
      break;
  }
}

//---------------------------------------------------------------------------
//
//  Function to make the given file with the given attributes.
//
//  INVALID_HANDLE_VALUE will be returned if errors, and a message
//  will be printed.
//
//---------------------------------------------------------------------------

static HANDLE FIOMakeFile(int     CaseNo,
                          LPVOID *SCInfo,
                          HANDLE  hLog,
                          HANDLE  hConOut,
                          LPSTR   lpFileName,
                          DWORD   dwFileAttribs)
{
    HANDLE hRet;
    
    FIODebugPrint(hConOut, "\nMaking file %s, attribs = %x\n",
        lpFileName, dwFileAttribs);
    
    // create the file
    
    hRet = CreateFile(
        lpFileName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        (LPSECURITY_ATTRIBUTES) NULL,
        CREATE_ALWAYS,
        dwFileAttribs,
        (HANDLE) NULL
        );
    
    if ( hRet == INVALID_HANDLE_VALUE ) {
        
        KdPrint(( "SETCLN: couldn't create file %s\n", lpFileName ));
            
            ErrorPrint(hConOut, hLog, "FIOMakeFile", CaseNo,
            "Could not create file");
        
        FIODebugPrint(hConOut, "\nFIOMakeFile: Error = %d\n",
            GetLastError());
        
        return(INVALID_HANDLE_VALUE);
    }
    
    // close handle
    
    if ( ! CloseHandle(hRet) ) {
        
        ErrorPrint(hConOut, hLog, "FIOMakeFile", CaseNo,
            "Error closing created file");
        
        return(INVALID_HANDLE_VALUE);
    }
    
    return(hRet);
    
    SCInfo;   // shut-up compiler
}

static HANDLE FIOMakeFileW(int     CaseNo,
                           LPVOID *SCInfo,
                           HANDLE  hLog,
                           HANDLE  hConOut,
                           LPWSTR  lpFileName,
                           DWORD   dwFileAttribs
                           )
{
    HANDLE hRet;
    
    FIODebugPrint(hConOut, "\nMaking file %ws, attribs = %x\n", lpFileName, dwFileAttribs);
    
    // create the file
    
    hRet = CreateFileW(
                lpFileName,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ,
                (LPSECURITY_ATTRIBUTES) NULL,
                CREATE_ALWAYS,
                dwFileAttribs,
                (HANDLE) NULL
                );
    
    if ( hRet == INVALID_HANDLE_VALUE ) {
        KdPrint(( "SETCLN: couldn't create file %ws\n", lpFileName ));
        ErrorPrint(hConOut, hLog, "FIOMakeFileW", CaseNo, "Could not create file");
        FIODebugPrint(hConOut, "\nFIOMakeFile: Error = %d\n", GetLastError());
        return INVALID_HANDLE_VALUE;
    }
    
    // close handle
    
    if ( !CloseHandle(hRet) ) {
        
        ErrorPrint(hConOut, hLog, "FIOMakeFile", CaseNo, "Error closing created file");
        return INVALID_HANDLE_VALUE;
    }
    
    return hRet;
    
    SCInfo;   // shut-up compiler
}

//---------------------------------------------------------------------------
//
//  Function to close and delete the given file.  Pass -1 for hFile
//  if you don't know it, or its already closed.  Pass TRUE for bSilent
//  if calling as part of a setup (trying to ensure that the given file
//  doesn't already exist), and it won't gripe that the file does not exist.
//
//  Returns false if fails, true otherwise.
//
//---------------------------------------------------------------------------

static BOOL FIOCloseDeleteFile(int     CaseNo,
                               LPVOID *SCInfo,
                               HANDLE  hLog,
                               HANDLE  hConOut,
                               HANDLE  hFile,
                               LPSTR   lpFileName,
                               BOOL    bSilent)
{
  HANDLE hTemp;

  FIODebugPrint(hConOut, "\nClosing/deleting file %s\n", lpFileName);

  // close handle if specified

  if ( hFile != INVALID_HANDLE_VALUE ) {

    if ( ! CloseHandle(hFile) ) {

      if ( ! bSilent )
        ErrorPrint(hConOut, hLog, "FIOCloseDeleteFile", CaseNo,
                   "Error closing file");
        return(0);
      }
  }

  // if file does not exist, never mind

  if ( (hTemp = CreateFile(lpFileName,
                           GENERIC_READ,
                           FILE_SHARE_READ,
                           (LPSECURITY_ATTRIBUTES) NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           (HANDLE) NULL)) == INVALID_HANDLE_VALUE ) {
      return(0);
  }

  CloseHandle(hTemp);

  // change to normal file, so can be deleted

  if ( ! SetFileAttributes(lpFileName, FILE_ATTRIBUTE_NORMAL) ) {

      if ( ! bSilent )
          ErrorPrint(hConOut, hLog, "FIOCloseDeleteFile", CaseNo,
                     "Error setting attributes of file trying to delete");
      return(0);
  }

  // delete file

  if ( ! DeleteFile(lpFileName) ) {

      if ( ! bSilent )
          ErrorPrint(hConOut, hLog, "FIOCloseDeleteFile", CaseNo,
                     "Error trying to delete file");
      return(0);
  }

  return(1);

  SCInfo;   // shut-up compiler
}


static
BOOL
FIOCloseDeleteFileW(
    int     CaseNo,
    LPVOID *SCInfo,
    HANDLE  hLog,
    HANDLE  hConOut,
    HANDLE  hFile,
    LPWSTR  lpFileName,
    BOOL    bSilent
    )
{
    HANDLE hTemp;
    
    FIODebugPrint(hConOut, "\nClosing/deleting file %ws\n", lpFileName);
    
    // Close handle if specified
    
    if ( hFile != INVALID_HANDLE_VALUE ) {
        
        if ( ! CloseHandle(hFile) ) {
            
            if ( ! bSilent )
                ErrorPrint(hConOut, hLog, "FIOCloseDeleteFileW", CaseNo, "Error closing file");
            return(0);
        }
    }
    
    // if file does not exist, never mind
    
    if ( (hTemp = CreateFileW(lpFileName,
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    (LPSECURITY_ATTRIBUTES) NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    (HANDLE) NULL)) == INVALID_HANDLE_VALUE ) {
        return(0);
    }
    
    CloseHandle(hTemp);
    
    // change to normal file, so can be deleted
    
    if ( !SetFileAttributesW(lpFileName, FILE_ATTRIBUTE_NORMAL) ) {
        
        if ( ! bSilent ) {
            ErrorPrint(
                hConOut, 
                hLog, 
                "FIOCloseDeleteFile", 
                CaseNo, 
                "Error setting attributes of file trying to delete"
                );
        }

        return(0);
    }
    
    // delete file
    
    if ( !DeleteFileW( lpFileName ) ) {
        
        if ( !bSilent ) {
            ErrorPrint(
                hConOut, 
                hLog, 
                "FIOCloseDeleteFile", 
                CaseNo,
                "Error trying to delete file"
                );
        }

        return(0);
    }
    
    return(1);
    
    SCInfo;   // shut-up compiler
}

