/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    fileapis.c

Abstract:

    This module contains APIs for KERNEL32. We want to use MSVCRT.DLL 
    but it couldn't be built from NT source tree. So we wrap all calls 
    required by MSVCRT.DLL here since in XBox we don't need KERNEL32.DLL

Author:

    Sakphong Chanbai (schanbai) 28-Mar-2000

Environment:

    XBox

Revision History:

--*/


#include "precomp.h"
#include <stdio.h>


BOOL
APIENTRY
CreateDirectoryA(
    LPCSTR lpPathName,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    )

/*++

Routine Description:

    ANSI thunk to CreateDirectoryW

--*/

{
    UNICODE_STRING Unicode;
    ANSI_STRING Ansi;
    NTSTATUS Status;
    BOOL b = FALSE;
    WCHAR Buffer[MAX_PATH];

    RtlInitAnsiString( &Ansi, lpPathName );
    Unicode.Buffer = Buffer;
    Unicode.Length = 0;
    Unicode.MaximumLength = MAX_PATH * sizeof(WCHAR);
    Status = RtlAnsiStringToUnicodeString( &Unicode, &Ansi, FALSE );

    if ( NT_SUCCESS(Status) ) {
        b = CreateDirectoryW( Unicode.Buffer, lpSecurityAttributes );
    } else {
        SetLastError( RtlNtStatusToDosError(Status) );
    }

    return b;
}


BOOL
APIENTRY
DeleteFileA(
    LPCSTR lpFileName
    )

/*++

Routine Description:

    ANSI thunk to DeleteFileW

--*/

{
    UNICODE_STRING Unicode;
    ANSI_STRING Ansi;
    NTSTATUS Status;
    BOOL b = FALSE;
    WCHAR Buffer[MAX_PATH];

    RtlInitAnsiString( &Ansi, lpFileName );
    Unicode.Buffer = Buffer;
    Unicode.Length = 0;
    Unicode.MaximumLength = MAX_PATH * sizeof(WCHAR);
    Status = RtlAnsiStringToUnicodeString( &Unicode, &Ansi, FALSE );

    if ( NT_SUCCESS(Status) ) {
        b = DeleteFileW( Unicode.Buffer );
    } else {
        SetLastError( RtlNtStatusToDosError(Status) );
    }

    return b;
}


BOOL
APIENTRY
SetFileAttributesA(
    LPCSTR lpFileName,
    DWORD dwFileAttributes
    )
{
    UNICODE_STRING Unicode;
    ANSI_STRING Ansi;
    NTSTATUS Status;
    BOOL b = FALSE;
    WCHAR Buffer[MAX_PATH];

    RtlInitAnsiString( &Ansi, lpFileName );
    Unicode.Buffer = Buffer;
    Unicode.Length = 0;
    Unicode.MaximumLength = MAX_PATH * sizeof(WCHAR);
    Status = RtlAnsiStringToUnicodeString( &Unicode, &Ansi, FALSE );

    if ( NT_SUCCESS(Status) ) {
        b = SetFileAttributesW( Unicode.Buffer, dwFileAttributes );
    } else {
        SetLastError( RtlNtStatusToDosError(Status) );
    }

    return b;
}


DWORD
APIENTRY
GetFileAttributesA(
    LPCSTR lpFileName
    )
{
    UNICODE_STRING Unicode;
    ANSI_STRING Ansi;
    WCHAR FileName[MAX_PATH];
    NTSTATUS Status;

    RtlInitAnsiString( &Ansi, lpFileName );
    Unicode.Buffer = FileName;
    Unicode.Length = 0;
    Unicode.MaximumLength = MAX_PATH * sizeof(WCHAR);
    Status = RtlAnsiStringToUnicodeString( &Unicode, &Ansi, FALSE );

    if ( NT_SUCCESS(Status) ) {
        return GetFileAttributesW( FileName );
    }

    return ~0ul;
}


HANDLE
APIENTRY
FindFirstFileA(
    LPCSTR lpFileName,
    LPWIN32_FIND_DATAA lpFindFileData
    )

/*++

Routine Description:

    ANSI thunk to FindFirstFileW

--*/

{
    HANDLE ReturnValue;
    UNICODE_STRING Unicode;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;
    WIN32_FIND_DATAW FindFileData;
    ANSI_STRING AnsiString;
    WCHAR Buffer[MAX_PATH];

    RtlInitAnsiString( &AnsiString, lpFileName );
    Unicode.Buffer = Buffer;
    Unicode.Length = 0;
    Unicode.MaximumLength = MAX_PATH * sizeof(WCHAR);
    Status = RtlAnsiStringToUnicodeString( &Unicode, &AnsiString, FALSE );

    if ( !NT_SUCCESS(Status) ) {
        return INVALID_HANDLE_VALUE;
    }

    ReturnValue = FindFirstFileW(
                    Unicode.Buffer,
                    &FindFileData
                    );

    if ( ReturnValue == INVALID_HANDLE_VALUE ) {
        return ReturnValue;
        }
    RtlMoveMemory(
        lpFindFileData,
        &FindFileData,
        sizeof(WIN32_FIND_DATAA)
        );
    RtlInitUnicodeString(&UnicodeString,(PWSTR)FindFileData.cFileName);
    AnsiString.Buffer = lpFindFileData->cFileName;
    AnsiString.MaximumLength = MAX_PATH;
    Status = RtlUnicodeStringToAnsiString(&AnsiString,&UnicodeString,FALSE);
    if (NT_SUCCESS(Status)) {
        RtlInitUnicodeString(&UnicodeString,(PWSTR)FindFileData.cAlternateFileName);
        AnsiString.Buffer = lpFindFileData->cAlternateFileName;
        AnsiString.MaximumLength = 14;
        Status = RtlUnicodeStringToAnsiString(&AnsiString,&UnicodeString,FALSE);
    }
    if ( !NT_SUCCESS(Status) ) {
        FindClose(ReturnValue);
        SetLastError( RtlNtStatusToDosError(Status) );
        return INVALID_HANDLE_VALUE;
        }
    return ReturnValue;
}


BOOL
APIENTRY
FindNextFileA(
    HANDLE hFindFile,
    LPWIN32_FIND_DATAA lpFindFileData
    )

/*++

Routine Description:

    ANSI thunk to FindFileDataW

--*/

{

    BOOL ReturnValue;
    ANSI_STRING AnsiString;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;
    WIN32_FIND_DATAW FindFileData;

    ReturnValue = FindNextFileW(hFindFile,&FindFileData);
    if ( !ReturnValue ) {
        return ReturnValue;
        }
    RtlMoveMemory(
        lpFindFileData,
        &FindFileData,
        sizeof(WIN32_FIND_DATAA)
        );
    RtlInitUnicodeString(&UnicodeString,(PWSTR)FindFileData.cFileName);
    AnsiString.Buffer = lpFindFileData->cFileName;
    AnsiString.MaximumLength = MAX_PATH;
    Status = RtlUnicodeStringToAnsiString(&AnsiString,&UnicodeString,FALSE);
    if (NT_SUCCESS(Status)) {
        RtlInitUnicodeString(&UnicodeString,(PWSTR)FindFileData.cAlternateFileName);
        AnsiString.Buffer = lpFindFileData->cAlternateFileName;
        AnsiString.MaximumLength = 14;
        Status = RtlUnicodeStringToAnsiString(&AnsiString,&UnicodeString,FALSE);
    }
    if ( !NT_SUCCESS(Status) ) {
        SetLastError(RtlNtStatusToDosError(Status));
        return FALSE;
        }
    return ReturnValue;
}


UINT
WINAPI
GetTempFileNameA(
    LPCSTR lpPathName,
    LPCSTR lpPrefixString,
    UINT uUnique,
    LPSTR lpTempFileName
    )
{
    char Slash[2];

    if ( !lpPathName ) {
        return 0;
    }

    if ( !uUnique ) {
        uUnique = NtGetTickCount();
    }

    if ( *lpPathName == '.' || *lpPathName != '\\' ) {
        lpPathName = "Z:\\";
    }

    if ( lpPathName[strlen(lpPathName) - 1] != '\\' ) {
        Slash[0] = '\\';
        Slash[1] = 0;
    } else {
        Slash[0] = 0;
    }

    sprintf( lpTempFileName, "%s%s%s%u", lpPathName, Slash, lpPrefixString, uUnique );
    return uUnique;
}


UINT
WINAPI
GetTempFileNameW(
    LPCWSTR lpPathName,
    LPCWSTR lpPrefixString,
    UINT uUnique,
    LPWSTR lpTempFileName
    )
{
    WCHAR Slash[2];

    if ( !lpPathName ) {
        return 0;
    }

    if ( !uUnique ) {
        uUnique = NtGetTickCount();
    }

    if ( *lpPathName == '.' || *lpPathName != L'\\' ) {
        lpPathName = L"Z:\\";
    }

    if ( lpPathName[wcslen(lpPathName) - 1] != L'\\' ) {
        Slash[0] = L'\\';
        Slash[1] = 0;
    } else {
        Slash[0] = 0;
    }

    swprintf( lpTempFileName, L"%s%s%s%u", lpPathName, Slash, lpPrefixString, uUnique );
    return uUnique;
}


BOOL
APIENTRY
GetDiskFreeSpaceW(
    LPCWSTR lpRootPathName,
    LPDWORD lpSectorsPerCluster,
    LPDWORD lpBytesPerSector,
    LPDWORD lpNumberOfFreeClusters,
    LPDWORD lpTotalNumberOfClusters
    )

/*++

Routine Description:

    The free space on a disk and the size parameters can be returned
    using GetDiskFreeSpace.

Arguments:

    lpRootPathName - An optional parameter, that if specified, supplies
        the root directory of the disk whose free space is to be
        returned for.  If this parameter is not specified, then the root
        of the current directory is used.

    lpSectorsPerCluster - Returns the number of sectors per cluster
        where a cluster is the allocation granularity on the disk.

    lpBytesPerSector - Returns the number of bytes per sector.

    lpNumberOfFreeClusters - Returns the total number of free clusters
        on the disk.

    lpTotalNumberOfClusters - Returns the total number of clusters on
        the disk.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    UNICODE_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_FS_SIZE_INFORMATION SizeInfo;
    WCHAR DefaultPath[8];

    if ( ARGUMENT_PRESENT(lpRootPathName) ) {
        wcscpy( DefaultPath, L"\\??\\T:\\" );
        DefaultPath[4] = *lpRootPathName;
    } else {
        SetLastError(ERROR_PATH_NOT_FOUND);
        return FALSE;
    }

    RtlInitUnicodeString( &FileName, DefaultPath );

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    //
    // Open the file
    //

    Status = NtOpenFile(
                &Handle,
                (ACCESS_MASK)FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE | FILE_OPEN_FOR_FREE_SPACE_QUERY
                );
    if ( !NT_SUCCESS(Status) ) {
        SetLastError( RtlNtStatusToDosError(Status) );

        //
        // Prior releases of NT where these parameters were not optional
        // zeroed out this field even in the failure case.  Some applications
        // failed to check the return value from this function and instead
        // relied on this side effect.  I'm putting that back now so the apps
        // can still treat an unformatted volume as a zero size volume.
        //

        if (ARGUMENT_PRESENT( lpBytesPerSector )) {
            *lpBytesPerSector = 0;
            }
        return FALSE;
        }

    //
    // Determine the size parameters of the volume.
    //

    Status = NtQueryVolumeInformationFile(
                Handle,
                &IoStatusBlock,
                &SizeInfo,
                sizeof(SizeInfo),
                FileFsSizeInformation
                );
    NtClose(Handle);
    if ( !NT_SUCCESS(Status) ) {
        SetLastError( RtlNtStatusToDosError(Status) );
        return FALSE;
        }

    //
    // Deal with 64 bit sizes
    //

    if ( SizeInfo.TotalAllocationUnits.HighPart ) {
        SizeInfo.TotalAllocationUnits.LowPart = (ULONG)-1;
        }
    if ( SizeInfo.AvailableAllocationUnits.HighPart ) {
        SizeInfo.AvailableAllocationUnits.LowPart = (ULONG)-1;
        }

    if (ARGUMENT_PRESENT( lpSectorsPerCluster )) {
        *lpSectorsPerCluster = SizeInfo.SectorsPerAllocationUnit;
        }
    if (ARGUMENT_PRESENT( lpBytesPerSector )) {
        *lpBytesPerSector = SizeInfo.BytesPerSector;
        }
    if (ARGUMENT_PRESENT( lpNumberOfFreeClusters )) {
        *lpNumberOfFreeClusters = SizeInfo.AvailableAllocationUnits.LowPart;
        }
    if (ARGUMENT_PRESENT( lpTotalNumberOfClusters )) {
        *lpTotalNumberOfClusters = SizeInfo.TotalAllocationUnits.LowPart;
        }

    return TRUE;
}


BOOL
WINAPI
GetDiskFreeSpaceA(
    LPCSTR  lpRootPathName,          // root path
    LPDWORD lpSectorsPerCluster,     // sectors per cluster
    LPDWORD lpBytesPerSector,        // bytes per sector
    LPDWORD lpNumberOfFreeClusters,  // free clusters
    LPDWORD lpTotalNumberOfClusters  // total clusters
    )
{
    BOOL b = FALSE;
    NTSTATUS Status;
    ANSI_STRING Ansi;
    UNICODE_STRING Unicode;
    WCHAR Buffer[MAX_PATH];

    RtlInitAnsiString( &Ansi, lpRootPathName );
    Unicode.Buffer = Buffer;
    Unicode.Length = 0;
    Unicode.MaximumLength = MAX_PATH * sizeof(WCHAR);
    Status = RtlAnsiStringToUnicodeString( &Unicode, &Ansi, FALSE );

    if ( NT_SUCCESS(Status) ) {
        b = GetDiskFreeSpaceW(
                Unicode.Buffer,
                lpSectorsPerCluster,
                lpBytesPerSector,
                lpNumberOfFreeClusters,
                lpTotalNumberOfClusters
                );
    } else {
        SetLastError( RtlNtStatusToDosError(Status) );
    }

    return b;
}


BOOL
WINAPI
MoveFileA(
    LPCSTR lpExistingFileName, // file name
    LPCSTR lpNewFileName       // new file name
    )
{
    UNICODE_STRING ExistingFileName, NewFileName;
    ANSI_STRING Ansi;
    NTSTATUS Status;
    BOOL b = FALSE;
    WCHAR FileName1[MAX_PATH];
    WCHAR FileName2[MAX_PATH];

    ExistingFileName.Buffer = FileName1;
    ExistingFileName.Length = 0;
    ExistingFileName.MaximumLength = sizeof(FileName1);

    NewFileName.Buffer = FileName2;
    NewFileName.Length = 0;
    NewFileName.MaximumLength = sizeof(FileName2);

    RtlInitAnsiString( &Ansi, lpExistingFileName );
    Status = RtlAnsiStringToUnicodeString( &ExistingFileName, &Ansi, FALSE );

    if ( NT_SUCCESS(Status) ) {
        RtlInitAnsiString( &Ansi, lpNewFileName );
        Status = RtlAnsiStringToUnicodeString( &NewFileName, &Ansi, FALSE );

        if ( NT_SUCCESS(Status) ) {
            b = MoveFileW( ExistingFileName.Buffer, NewFileName.Buffer );
        }
    }

    return b;
}
