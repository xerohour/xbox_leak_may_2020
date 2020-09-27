/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    dir.c

Abstract:

    This module implements Win32 Directory functions.

Author:

    Mark Lucovsky (markl) 26-Sep-1990

Revision History:

--*/

#include "basedll.h"

BOOL
APIENTRY
CreateDirectory(
    PCOSTR lpPathName,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    )

/*++

Routine Description:

    A directory can be created using CreateDirectory.

    This API causes a directory with the specified pathname to be
    created.  If the underlying file system supports security on files
    and directories, then the SecurityDescriptor argument is applied to
    the new directory.

    This call is similar to DOS (int 21h, function 39h) and OS/2's
    DosCreateDir.

Arguments:

    lpPathName - Supplies the pathname of the directory to be created.

    lpSecurityAttributes - Ignored on xbox.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;

    RtlInitObjectString(&FileName, lpPathName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    Status = NtCreateFile(
                &Handle,
                FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                NULL,
                FILE_ATTRIBUTE_NORMAL,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_CREATE,
                FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                );

    if ( NT_SUCCESS(Status) ) {
        NtClose(Handle);
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

BOOL
APIENTRY
RemoveDirectory(
    PCOSTR lpPathName
    )

/*++

Routine Description:

    An existing directory can be removed using RemoveDirectory.

    This API causes a directory with the specified pathname to be
    deleted.  The directory must be empty before this call can succeed.

    This call is similar to DOS (int 21h, function 3Ah) and OS/2's
    DosDeleteDir.

Arguments:

    lpPathName - Supplies the pathname of the directory to be removed.
        The path must specify an empty directory to which the caller has
        delete access.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_DISPOSITION_INFORMATION Disposition;

    RtlInitObjectString(&FileName, lpPathName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Open the directory for delete access.
    //

    Status = NtOpenFile(
                 &Handle,
                 DELETE | SYNCHRONIZE,
                 &Obja,
                 &IoStatusBlock,
                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                 FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                 );

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    //
    // Delete the file
    //
#undef DeleteFile
    Disposition.DeleteFile = TRUE;

    Status = NtSetInformationFile(
                Handle,
                &IoStatusBlock,
                &Disposition,
                sizeof(Disposition),
                FileDispositionInformation
                );

    NtClose(Handle);
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}
