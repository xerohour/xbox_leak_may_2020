
#ifndef FILUTIL_INC

#define FILUTIL_INC

#include <xtl.h>
#include <winerror.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

  //
  // error values
  //

enum fFileSystem {

  FIO_NO_ERR = 0,
  FIO_NO_MORE_CASES,
  FIO_NO_MORE_TYPES,
  FIO_ERR_ALLOC_MEM,
  FIO_ERR_GETVOLINFO,
  FIO_ERR_GET_CUR_DIR,
  FIO_ERR_BUFF_TOO_SMALL,
  FIO_ERR_CREATING_DIR,
  FIO_ERR_UNKNOWN_FILE_SYSTEM,
  FIO_ERR_UNKNOWN_BADF_TYPE
};

  //
  // types of bad filenames to pass to fGetBadFileName
  //
  // all of the FIO_BADFNAME_* constants are fully qualified filenames (off
  // current drive), each case differs only in which component of the full
  // path name contains the bogusness
  //
  // all of the FIO_BADDIRNAME_* constants are fully qualified pathnames
  // (again, qualified off the current drive)
  //

enum fBadNameTypes {

  FIO_BADFNAME_UNPRINT_CHARS_ROOT = 0,
  FIO_BADFNAME_UNPRINT_CHARS_SUBDIR,
  FIO_BADFNAME_UNPRINT_CHARS_DIRNAME,

  FIO_BADFNAME_PRINT_CHARS_ROOT,
  FIO_BADFNAME_PRINT_CHARS_SUBDIR,
  FIO_BADFNAME_PRINT_CHARS_DIRNAME,

  FIO_BADFNAME_INVALID_DRIVE_NAME,

  FIO_BADFNAME_ILL_FORMED,

  FIO_BADFNAME_NAME_TOO_LONG,

  FIO_BADDIRNAME_UNPRINT_CHARS,
  FIO_BADDIRNAME_PRINT_CHARS,
  FIO_BADDIRNAME_ILL_FORMED,
  FIO_BADDIRNAME_INVALID_DRIVE_NAME,

  FIO_BADF_NUM_TYPES
};

  //
  // file system types
  //

enum fFileSystems {

  FIO_FS_FAT = 0,
  FIO_FS_HPFS,
  FIO_FS_NTFS,
  FIO_FS_CDFS
};

  //
  // prototypes
  //

DWORD fGetNextBadFileName(LPSTR *lpFileName, int iType, int cCaseNum);
DWORD fGetNextBadFileNameW(LPWSTR *lpFileName, int iType, int cCaseNum);

DWORD fGetCurDriveName(LPSTR lpBuff, DWORD dwBuffLen);

DWORD fGetFileSystem(LPINT lpFsType);

#endif
