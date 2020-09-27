/*
 *
 * ochar.h
 *
 * a/w versions of system APIs
 *
 * Copyright (C) 2000 Microsoft Corporation
 *
 */

#ifndef _OCHAR_H
#define _OCHAR_H

#ifndef _XBOX
#error XBOX-specific A/W substitutions
#endif

#ifdef WPRFLAG

#define _AWSUBS
#undef CreateDirectory
#undef CreateFile
#undef DeleteFile
#undef FindFirstFile
#undef FindNextFile
#undef GetFileAttributes
#undef MoveFile
#undef RemoveDirectory
#undef SetFileAttributes
#undef WIN32_FIND_DATA
#undef PWIN32_FIND_DATA
#undef LPWIN32_FIND_DATA

#define CreateDirectory __crtCreateDirectoryW
#define CreateFile __crtCreateFileW
#define DeleteFile __crtDeleteFileW
#define FindFirstFile __crtFindFirstFileW
#define FindNextFile __crtFindNextFileW
#define GetFileAttributes __crtGetFileAttributesW
#define MoveFile __crtMoveFileW
#define RemoveDirectory __crtRemoveDirectoryW
#define SetFileAttributes __crtSetFileAttributesW

#endif // WPRFLAG

#ifdef _AWSUBS

BOOL
CreateDirectory(
    IN LPCTSTR lpPathName,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
    
HANDLE
CreateFile(
    IN LPCTSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
    );

BOOL
DeleteFile(
    IN LPCTSTR lpFileName
    );

typedef struct _WIN32_FIND_DATAT {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    TCHAR cFileName[ MAX_PATH ];
    TCHAR cAlternateFileName[ 14 ];
} WIN32_FIND_DATAT, *PWIN32_FIND_DATAT, *LPWIN32_FIND_DATAT;
#define WIN32_FIND_DATA WIN32_FIND_DATAT
#define PWIN32_FIND_DATA PWIN32_FIND_DATAT
#define LPWIN32_FIND_DATA LPWIN32_FIND_DATAT

HANDLE
FindFirstFile(
    IN LPCTSTR lpFileName,
    OUT LPWIN32_FIND_DATAT lpFindFileData
    );

BOOL
FindNextFile(
    IN HANDLE hFindFile,
    OUT LPWIN32_FIND_DATAT lpFindFileData
    );

DWORD
GetFileAttributes(
    IN LPCTSTR lpFileName
    );

BOOL
MoveFile(
    IN LPCTSTR lpExistingFileName,
    IN LPCTSTR lpNewFileName
    );

BOOL
RemoveDirectory(
    IN LPCTSTR lpPathName
    );

BOOL
SetFileAttributes(
    IN LPCTSTR lpFileName,
    IN DWORD dwFileAttributes
    );

#endif // _AWSUBS

#endif // _OCHAR_H
