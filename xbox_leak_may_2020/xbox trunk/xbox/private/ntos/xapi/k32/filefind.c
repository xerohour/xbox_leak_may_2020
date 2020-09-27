/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    filefind.c

Abstract:

    This module implements Win32 FindFirst/FindNext

Author:

    Mark Lucovsky (markl) 26-Sep-1990

Revision History:

--*/

#include "basedll.h"

VOID
XapiDirectoryInformationToFindData(
    IN PFILE_DIRECTORY_INFORMATION DirInfo,
    OUT LPWIN32_FIND_DATA lpFindFileData
    )
{
    lpFindFileData->dwFileAttributes = DirInfo->FileAttributes;
    lpFindFileData->ftCreationTime = *(LPFILETIME)&DirInfo->CreationTime;
    lpFindFileData->ftLastAccessTime = *(LPFILETIME)&DirInfo->LastAccessTime;
    lpFindFileData->ftLastWriteTime = *(LPFILETIME)&DirInfo->LastWriteTime;
    lpFindFileData->nFileSizeHigh = DirInfo->EndOfFile.HighPart;
    lpFindFileData->nFileSizeLow = DirInfo->EndOfFile.LowPart;

    RtlCopyMemory(lpFindFileData->cFileName, DirInfo->FileName,
        DirInfo->FileNameLength);
    lpFindFileData->cFileName[DirInfo->FileNameLength / sizeof(OCHAR)] = OBJECT_NULL;

    lpFindFileData->cAlternateFileName[0] = OBJECT_NULL;
}

HANDLE
APIENTRY
FindFirstFile(
    PCOSTR lpFileName,
    LPWIN32_FIND_DATA lpFindFileData
    )

/*++

Routine Description:

    A directory can be searched for the first entry whose name and
    attributes match the specified name using FindFirstFile.

    This API is provided to open a find file handle and return
    information about the first file whose name match the specified
    pattern.  Once established, the find file handle can be used to
    search for other files that match the same pattern.  When the find
    file handle is no longer needed, it should be closed.

    Note that while this interface only returns information for a single
    file, an implementation is free to buffer several matching files
    that can be used to satisfy subsequent calls to FindNextFile.  Also
    not that matches are done by name only.  This API does not do
    attribute based matching.

    This API is similar to DOS (int 21h, function 4Eh), and OS/2's
    DosFindFirst.  For portability reasons, its data structures and
    parameter passing is somewhat different.

Arguments:

    lpFileName - Supplies the file name of the file to find.  The file name
        may contain the DOS wild card characters '*' and '?'.

    lpFindFileData - On a successful find, this parameter returns information
        about the located file:

        WIN32_FIND_DATA Structure:

        DWORD dwFileAttributes - Returns the file attributes of the found
            file.

        FILETIME ftCreationTime - Returns the time that the file was created.
            A value of 0,0 specifies that the file system containing the
            file does not support this time field.

        FILETIME ftLastAccessTime - Returns the time that the file was last
            accessed.  A value of 0,0 specifies that the file system
            containing the file does not support this time field.

        FILETIME ftLastWriteTime - Returns the time that the file was last
            written.  A file systems support this time field.

        DWORD nFileSizeHigh - Returns the high order 32 bits of the
            file's size.

        DWORD nFileSizeLow - Returns the low order 32-bits of the file's
            size in bytes.

        UCHAR cFileName[MAX_PATH] - Returns the null terminated name of
            the file.

Return Value:

    Not -1 - Returns a find first handle
        that can be used in a subsequent call to FindNextFile or FindClose.

    0xffffffff - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    ULONG EndingIndex;
    OBJECT_STRING FileName;
    OBJECT_STRING SearchName;
    IO_STATUS_BLOCK IoStatusBlock;
    struct {
        FILE_DIRECTORY_INFORMATION DirInfo;
        OCHAR FileName[256];
    } FullDirInfo;

    RtlInitObjectString(&FileName, lpFileName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Strip off the search specification from the file name.
    //

    EndingIndex = (FileName.Length / sizeof(OCHAR));

    while (EndingIndex > 0) {

        if (FileName.Buffer[EndingIndex - 1] == OTEXT('\\')) {
            break;
        }

        EndingIndex--;
    }

    SearchName.Length = (USHORT)(FileName.Length - (EndingIndex * sizeof(OCHAR)));
    SearchName.MaximumLength = SearchName.Length;
    SearchName.Buffer = &FileName.Buffer[EndingIndex];

    FileName.Length = (USHORT)(EndingIndex * sizeof(OCHAR));
    FileName.MaximumLength = FileName.Length;

    //
    // Make sure that we didn't consume the entire string while searching for
    // the search specification and that the search specification is not empty.
    //

    if ((FileName.Length == 0) || (SearchName.Length == 0)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return INVALID_HANDLE_VALUE;
    }

    //
    // If the search name is "*.*", then convert it to an empty string so that
    // we end up matching all names, not just those with an extension.
    //

    if (SearchName.Length == sizeof(OCHAR) * 3 &&
        SearchName.Buffer[0] == OTEXT('*') && SearchName.Buffer[1] == OTEXT('.') &&
        SearchName.Buffer[2] == OTEXT('*')) {
        SearchName.Length = 0;
    }

    //
    // Open the directory for list access.
    //

    Status = NtOpenFile(
                 &Handle,
                 FILE_LIST_DIRECTORY | SYNCHRONIZE,
                 &Obja,
                 &IoStatusBlock,
                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                 FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                 );

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return INVALID_HANDLE_VALUE;
    }

    //
    // Get the first entry from the directory.
    //

    Status = NtQueryDirectoryFile(
                 Handle,
                 NULL,
                 NULL,
                 NULL,
                 &IoStatusBlock,
                 &FullDirInfo,
                 sizeof(FullDirInfo),
                 FileDirectoryInformation,
                 &SearchName,
                 FALSE
                 );

    if ( !NT_SUCCESS(Status) ) {
        NtClose(Handle);
        XapiSetLastNTError(Status);
        return INVALID_HANDLE_VALUE;
    }

    XapiDirectoryInformationToFindData(&FullDirInfo.DirInfo, lpFindFileData);

    return Handle;
}

BOOL
APIENTRY
FindNextFile(
    HANDLE hFindFile,
    LPWIN32_FIND_DATA lpFindFileData
    )

/*++

Routine Description:

    Once a successful call has been made to FindFirstFile, subsequent
    matching files can be located using FindNextFile.

    This API is used to continue a file search from a previous call to
    FindFirstFile.  This API returns successfully with the next file
    that matches the search pattern established in the original
    FindFirstFile call.  If no file match can be found NO_MORE_FILES is
    returned.

    Note that while this interface only returns information for a single
    file, an implementation is free to buffer several matching files
    that can be used to satisfy subsequent calls to FindNextFile.  Also
    not that matches are done by name only.  This API does not do
    attribute based matching.

    This API is similar to DOS (int 21h, function 4Fh), and OS/2's
    DosFindNext.  For portability reasons, its data structures and
    parameter passing is somewhat different.

Arguments:

    hFindFile - Supplies a find file handle returned in a previous call
        to FindFirstFile.

    lpFindFileData - On a successful find, this parameter returns information
        about the located file.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    struct {
        FILE_DIRECTORY_INFORMATION DirInfo;
        OCHAR FileName[256];
    } FullDirInfo;

    Status = NtQueryDirectoryFile(
                 hFindFile,
                 NULL,
                 NULL,
                 NULL,
                 &IoStatusBlock,
                 &FullDirInfo,
                 sizeof(FullDirInfo),
                 FileDirectoryInformation,
                 NULL,
                 FALSE
                 );

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
    }

    XapiDirectoryInformationToFindData(&FullDirInfo.DirInfo, lpFindFileData);

    return TRUE;
}
