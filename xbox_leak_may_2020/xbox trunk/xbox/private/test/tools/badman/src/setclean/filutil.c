//---------------------------------------------------------------------------
//
//  filutil.c - fileio utilities
//  ----------------------------
//
//  these are in a separate file and coded as such in hopes that they will
//  be used by more than just badman code (if and when they find a home).
//  in all cases they return error codes defined in the header file and
//  never print error messages.
//  
//  entry points:
//  -------------
//
//    fGetNextBadFileName - allows looping over many bad file names.  file
//                          system specific details are handled by this
//                          routine.
//
//    fGetCurrDriveName   - gets the name of the current drive and handles
//                          details of UNC names
//
//    fGetFileSystem      - gets the file system of the current drive and
//                          converts it to one of the enumerated constants
//                          defined in the header file
//
//  History:
//  --------
//  4-15-92 steveko - wrote
//
//---------------------------------------------------------------------------


#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include "filutil.h"

#define MAX_CHAR_VAL 255

#define MAX_BUFF     1024

  //
  // Ill-formed FAT file names
  //

char *IllFormedFatNames[] = {
  "",
  "xyz.",
  ".xyz",
  "xyz .a",
  "xyzabcdef.ghi",
  "xyzabcde.fghi",
  "xyz.abc.lmn"

};

#define NUM_ILL_FORMED_FAT_FILE_NAMES (sizeof(IllFormedFatNames) / \
                                       sizeof(IllFormedFatNames[0]))

  //
  // Ill-formed HPFS file names
  //

char *IllFormedHPFSNames[] = {
  "",
  "xyz.",
  "xyz ",
};

#define NUM_ILL_FORMED_HPFS_FILE_NAMES (sizeof(IllFormedHPFSNames) / \
                                        sizeof(IllFormedHPFSNames[0]))

  //
  // Ill-formed NTFS file names
  //

char *IllFormedNTFSNames[] = {  // more to be added later when know more
  "",
  "abc:",
  "abc::::",
  "ab:ab:ab"
};

#define NUM_ILL_FORMED_NTFS_FILE_NAMES (sizeof(IllFormedNTFSNames) / \
                                        sizeof(IllFormedNTFSNames[0]))

  //
  // Ill-formed CDFS file names
  //

char *IllFormedCDFSNames[] = {
  "",
  "abcdefghijklmnop.qrstuvwxyz12345",  // 1 char too long
  "abc.ab;1;2",
  "abc.xyz;40000"
};

#define NUM_ILL_FORMED_CDFS_FILE_NAMES (sizeof(IllFormedCDFSNames) / \
                                        sizeof(IllFormedCDFSNames[0]))

  //
  // printable illegal chars
  //

#define ILLEGAL_FAT_CHARS  "//:<>?|*+,;=[]"

#define ILLEGAL_HPFS_CHARS "//:<>?|*"

#define ILLEGAL_NTFS_CHARS "//<>?|*"

//---------------------------------------------------------------------------
//
//  function to get the next bad file name of the given type
//
//  Example:
//
//   for ( iType = 0, lpFileName=NULL; iType <= FIO_BADF_NUM_TYPES; iType++ )
//
//     for ( cCaseNum = 0; ; cCaseNum++ )
//
//       dwRet = fGetNextBadFileName(&lpFileName, iType, cCaseNum);
//
//       if ( dwRet == FIO_NO_MORE_CASES )
//         break
//
//       if ( dwRet != FIO_NO_ERR )
//         problem
//
//  since the max amount of memory required is 64k + 10 bytes, this function
//  will allocate and free the memory needed for the bad file names.
//
//  be sure that the value of the pointer given for lplpFileName is NULL
//  when no memory is allocated (i.e. first time called)
//
//  the memory allocated by this routine will only be freed when passed
//  an iType out of range, hence the inclusive iType (0, FIO_BADF_NUM_TYPES)
//  loop.  another example in case you don't want to loop over all types
//
//   for ( cCaseNum = 0, lpFileName=NULL; ; cCaseNum++ )
//
//     dwRet = fGetNextBadFileName(&lpFileName,
//                                 FIO_BADFNAME_UNPRINT_CHARS_ROOT, cCaseNum);
//
//     if ( dwRet == FIO_NO_MORE_CASES )
//       break
//
//     if ( dwRet != FIO_NO_ERR )
//       problem
//
//   fGetNextBadFileName(&lpFileName, -1 /* i.e. out of range */ , 0);
//
//---------------------------------------------------------------------------

DWORD fGetNextBadFileName(LPSTR *lplpFileName,
                          int    iType,
                          int    cCaseNum)
{
    char   lpCurDrive[MAX_PATH];
    char   lpUnprintIllegalStr[MAX_BUFF];
    char   lpPrintIllegalStr[MAX_BUFF];
    char   lpBuffer[MAX_PATH];
    char   *p;
    LPSTR  *lpIllFormedList;
    int    iFSType;
    int    cIllFormedCount, cMaxLength, i, iMemNeeded;
    DWORD  dwRet;
    BOOL   bRet;

    //
    // If type is out of range, free any memory allocated and return
    //
    
    if ( iType < 0 || iType >= FIO_BADF_NUM_TYPES ) {
        
        if ( *lplpFileName != NULL )
            LocalFree((HANDLE)(HANDLE) *lplpFileName);
        
        *lplpFileName = NULL;
        
        return(FIO_NO_MORE_TYPES);
    }
    
    //
    // get current drive
    //
    
    if ( (dwRet = fGetCurDriveName(lpCurDrive, MAX_PATH)) != 0 )
        return(dwRet);
    
    //
    // get file system
    //
    
    if ( (dwRet = fGetFileSystem(&iFSType)) != 0 )
        return(dwRet);
    
    //
    // set vars for below based on file system
    //
    
    switch ( iFSType ) {
        
    case FIO_FS_FAT:
        
        for ( i = 0x1f, p=lpUnprintIllegalStr; i >= 0; i-- )
            *p++ = (char) i;
        
        strcpy(lpPrintIllegalStr, ILLEGAL_FAT_CHARS);
        
        lpIllFormedList = IllFormedFatNames;
        cIllFormedCount = NUM_ILL_FORMED_FAT_FILE_NAMES;
        
        cMaxLength = 8;  // note the 8.3 type cases are handled by the
        // ill-formed test cases
        break;
        
    case FIO_FS_HPFS:
        
        for ( i = 0x1f, p=lpUnprintIllegalStr; i >= 0; i-- )
            *p++ = (char) i;
        
        strcpy(lpPrintIllegalStr, ILLEGAL_HPFS_CHARS);
        
        lpIllFormedList = IllFormedHPFSNames;
        cIllFormedCount = NUM_ILL_FORMED_HPFS_FILE_NAMES;
        
        cMaxLength = 255;
        
        break;
        
    case FIO_FS_NTFS:
        
        for ( i = 0x1f, p=lpUnprintIllegalStr; i >= 0; i-- )
            *p++ = (char) i;
        
        strcpy(lpPrintIllegalStr, ILLEGAL_NTFS_CHARS);
        
        lpIllFormedList = IllFormedNTFSNames;
        cIllFormedCount = NUM_ILL_FORMED_NTFS_FILE_NAMES;
        
        cMaxLength = 65536;
        
        break;
        
        //    case FIO_FS_CDFS: // legal chars . ; 0-9 A-F _ a-z 
        //
        //      p  = lpPrintIllegalStr;
        //      p2 = lpUnprintIllegalStr;
        //
        //      for ( i = MAX_CHAR_VAL; i >= 0; i-- ) {
        //        if ( (i == '.') ||
        //             (i == '_') ||
        //             (i == ';') ||
        //             (i >= '0' && i <= '9') ||
        //             (i >= 'A' && i <= 'F') ||
        //             (i >= 'a' && i <= 'z') )
        //          continue;
        //        if (isprint(i))
        //          *p++  = (char) i;
        //        else
        //          *p2++ = (char) i;
        //      }
        //
        //      lpIllFormedList = IllFormedCDFSNames;
        //      cIllFormedCount = NUM_ILL_FORMED_CDFS_FILE_NAMES;
        //
        //      cMaxLength = 0;
        //
        //      break;
        
    default:
        return(FIO_ERR_UNKNOWN_FILE_SYSTEM);
    }
    
    //
    // Get enough memory to hold the string, which is MAX_PATH except in the
    // special cases handled below
    //
    
    iMemNeeded = MAX_PATH;
    
    if ( iType == FIO_BADFNAME_NAME_TOO_LONG )
        iMemNeeded = cMaxLength + 10;
    
    if ( *lplpFileName == NULL )
        
        *lplpFileName = (LPSTR) LocalAlloc(LPTR, iMemNeeded);
    
    else if ( LocalSize((HANDLE) *lplpFileName) != (unsigned int) iMemNeeded ) {
        
        LocalFree((HANDLE) *lplpFileName);
        
        *lplpFileName = (LPSTR) LocalAlloc(LPTR, iMemNeeded);
    }
    
    //
    // build the next bad file name
    //
    
    switch ( iType ) {
        
    case FIO_BADFNAME_UNPRINT_CHARS_ROOT: // valid drive, illegal chars in
        // filename off root
        
        if ( cCaseNum >= (int) strlen(lpUnprintIllegalStr) )
            return(FIO_NO_MORE_CASES);
        
        sprintf(*lplpFileName, "%s\\xyz%ca", lpCurDrive,
            lpUnprintIllegalStr[cCaseNum]);
        break;
        
    case FIO_BADFNAME_UNPRINT_CHARS_SUBDIR: // valid drive, valid &
        // existing subdir, illegal
        // chars in filename
        
        if ( cCaseNum >= (int) strlen(lpUnprintIllegalStr) )
            return(FIO_NO_MORE_CASES);
        
        sprintf(lpBuffer, "%s\\xyzabc.tst", lpCurDrive);
        
        bRet = CreateDirectory(lpBuffer, NULL);  // create dir to ensure that
        // its not a bad file name
        // because of that
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS )
            return(FIO_ERR_CREATING_DIR);
        
        sprintf(*lplpFileName, "%s\\xyz%ca", lpBuffer,
            lpUnprintIllegalStr[cCaseNum]);
        break;
        
    case FIO_BADFNAME_UNPRINT_CHARS_DIRNAME: // valid drive, illegal chars in
        // subdir, good file name
        
        if ( cCaseNum >= (int) strlen(lpUnprintIllegalStr) )
            return(FIO_NO_MORE_CASES);
        
        sprintf(*lplpFileName, "%s\\xyz%ca\\xyz.abc", lpCurDrive,
            lpUnprintIllegalStr[cCaseNum]);
        break;
        
    case FIO_BADFNAME_PRINT_CHARS_ROOT: // valid drive, illegal chars in
        // filename off root
        
        if ( cCaseNum >= (int) strlen(lpPrintIllegalStr) )
            return(FIO_NO_MORE_CASES);
        
        sprintf(*lplpFileName, "%s\\xyz%ca", lpCurDrive,
            lpPrintIllegalStr[cCaseNum]);
        break;
        
    case FIO_BADFNAME_PRINT_CHARS_SUBDIR: // valid drive, valid &
        // existing subdir, illegal
        // chars in filename
        
        if ( cCaseNum >= (int) strlen(lpPrintIllegalStr) )
            return(FIO_NO_MORE_CASES);
        
        sprintf(lpBuffer, "%s\\xyzabc.tst", lpCurDrive);
        
        bRet = CreateDirectory(lpBuffer, NULL);  // create dir to ensure that
        // its not a bad file name
        // because of that
        
        if ( ! bRet && GetLastError() != ERROR_ALREADY_EXISTS )
            return(FIO_ERR_CREATING_DIR);
        
        sprintf(*lplpFileName, "%s\\xyz%ca", lpBuffer,
            lpPrintIllegalStr[cCaseNum]);
        break;
        
    case FIO_BADFNAME_PRINT_CHARS_DIRNAME: // valid drive, illegal chars in
        // subdir, good file name
        
        if ( cCaseNum >= (int) strlen(lpPrintIllegalStr) )
            return(FIO_NO_MORE_CASES);
        
        sprintf(*lplpFileName, "%s\\xyz%ca\\xyz.abc", lpCurDrive,
            lpPrintIllegalStr[cCaseNum]);
        break;
        
    case FIO_BADFNAME_INVALID_DRIVE_NAME:
        
        if ( cCaseNum > 0 )
            return(FIO_NO_MORE_CASES);
        
        strcpy(*lplpFileName, "cx:\\xyz.abc");
        break;
        
    case FIO_BADDIRNAME_ILL_FORMED:
    case FIO_BADFNAME_ILL_FORMED:
        
        if ( cCaseNum >= cIllFormedCount )
            return(FIO_NO_MORE_CASES);
        
        strcpy(*lplpFileName, lpIllFormedList[cCaseNum]);
        
        break;
        
    case FIO_BADFNAME_NAME_TOO_LONG:
        
        if ( cCaseNum > 0 )
            return(FIO_NO_MORE_CASES);
        
        for ( i = 0; i < cMaxLength + 10; i++ )
            (*lplpFileName)[i] = 'A';
        
        break;
        
    case FIO_BADDIRNAME_UNPRINT_CHARS: // valid drive, illegal chars in
        // filename off root
        
        if ( cCaseNum >= (int) strlen(lpUnprintIllegalStr) )
            return(FIO_NO_MORE_CASES);
        
        sprintf(*lplpFileName, "%s\\xyz%ca", lpCurDrive,
            lpUnprintIllegalStr[cCaseNum]);
        break;
        
    case FIO_BADDIRNAME_PRINT_CHARS: // valid drive, illegal chars in
        // filename off root
        
        if ( cCaseNum >= (int) strlen(lpUnprintIllegalStr) )
            return(FIO_NO_MORE_CASES);
        
        sprintf(*lplpFileName, "%s\\xyz%ca", lpCurDrive,
            lpPrintIllegalStr[cCaseNum]);
        break;
        
    case FIO_BADDIRNAME_INVALID_DRIVE_NAME:
        
        if ( cCaseNum > 0 )
            return(FIO_NO_MORE_CASES);
        
        strcpy(*lplpFileName, "cx:\\xyz.abc");
        break;
  }
  
  return(FIO_NO_ERR);
}

DWORD fGetNextBadFileNameW(
    LPWSTR *lplpFileName,
    int    iType,
    int    cCaseNum)
{
    LPSTR lpFileName = NULL;
    DWORD ReturnStatus;
    NTSTATUS Status;

    ReturnStatus = fGetNextBadFileName(&lpFileName, iType, cCaseNum);

    if ( FIO_NO_ERR == ReturnStatus ) {
        ANSI_STRING Ansi;
        UNICODE_STRING Unicode;

        ASSERT( lpFileName );

        RtlInitAnsiString( &Ansi, lpFileName );
        Status = RtlAnsiStringToUnicodeString( &Unicode, &Ansi, TRUE );
        LocalFree( lpFileName );

        if ( NT_SUCCESS(Status) ) {
            *lplpFileName = Unicode.Buffer;
        } else {
            KdPrint(( "FILEUTIL: RtlAnsiStringToUnicodeString failed, status = 0x%x\n", Status ));
            *lplpFileName = NULL;
        }
    } else {
        KdPrint(( "FILEUTIL: fGetNextBadFileName failed (%d)\n", ReturnStatus ));
    }

    return ReturnStatus;
}

//---------------------------------------------------------------------------
//
// Function to get the current drive name
//
//---------------------------------------------------------------------------

DWORD fGetCurDriveName(LPSTR lpBuff, DWORD dwBuffLen)
{
  char lpCurDir[MAX_PATH];
  int i, cNumSlash;

  strcpy( lpCurDir, "z:\\" );

  //
  // if first char a \, then a UNC name
  //

  if ( lpCurDir[0] == '\\' ) {

    for ( i = 0, cNumSlash = 0; lpCurDir[i] && cNumSlash < 4; i++ )
      if ( lpCurDir[i] == '\\' )
        cNumSlash++;

    if ( (unsigned int) i > dwBuffLen )
      return(FIO_ERR_BUFF_TOO_SMALL);

    strncpy(lpBuff, lpCurDir, i);
    lpBuff[i] = '\0';
  }

  //
  // else first two chars are logical drive
  //

  else {

    if ( dwBuffLen < 3 )
      return(FIO_ERR_BUFF_TOO_SMALL);

    strncpy(lpBuff, lpCurDir, 2);
    lpBuff[2] = '\0';
  }

  return(FIO_NO_ERR);
}

//---------------------------------------------------------------------------
//
//  function to get the file system (one of the fFileSystems enum'd values)
//
//  passing NULL for lpRootPathName will get the file system type for the
//  current drive
//
//---------------------------------------------------------------------------

DWORD fGetFileSystem(LPINT lpFsType)
{
  // BUGBUG: Always return FAT for now
  *lpFsType = FIO_FS_FAT;
  return FIO_NO_ERR;

  /*char lpFSName[MAX_BUFF];

  if ( ! GetVolumeInformation(NULL, NULL, 0, NULL, NULL, NULL,
                              lpFSName, MAX_BUFF) ) {
      return(FIO_ERR_GETVOLINFO);
  }

  if ( strcmp(lpFSName, "FAT") == 0 )
      *lpFsType = FIO_FS_FAT;
  else if ( strcmp(lpFSName, "HPFS") == 0 )
      *lpFsType = FIO_FS_HPFS;
  else if ( strcmp(lpFSName, "NTFS") == 0 )
      *lpFsType = FIO_FS_NTFS;
  else if ( strcmp(lpFSName, "CDFS") == 0 )
      *lpFsType = FIO_FS_CDFS;
  else
      return(FIO_ERR_UNKNOWN_FILE_SYSTEM);

  return(FIO_NO_ERR);*/
}
