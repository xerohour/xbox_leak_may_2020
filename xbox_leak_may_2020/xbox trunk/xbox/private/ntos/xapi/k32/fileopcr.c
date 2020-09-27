/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    fileopcr.c

Abstract:

    This module implements File open and Create APIs for Win32

Author:

    Mark Lucovsky (markl) 25-Sep-1990

Revision History:

--*/

#include "basedll.h"

#define BASE_COPY_FILE_CHUNK (64*1024)

HANDLE
WINAPI
CreateFile(
    PCOSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    )

/*++

Routine Description:

    A file can be created, opened, or truncated, and a handle opened to
    access the new file using CreateFile.

    This API is used to create or open a file and obtain a handle to it
    that allows reading data, writing data, and moving the file pointer.

    This API allows the caller to specify the following creation
    dispositions:

      - Create a new file and fail if the file exists ( CREATE_NEW )

      - Create a new file and succeed if it exists ( CREATE_ALWAYS )

      - Open an existing file ( OPEN_EXISTING )

      - Open and existing file or create it if it does not exist (
        OPEN_ALWAYS )

      - Truncate and existing file ( TRUNCATE_EXISTING )

    If this call is successful, a handle is returned that has
    appropriate access to the specified file.

    If as a result of this call, a file is created,

      - The attributes of the file are determined by the value of the
        FileAttributes parameter or'd with the FILE_ATTRIBUTE_ARCHIVE bit.

      - The length of the file will be set to zero.

    This call is logically equivalent to DOS (int 21h, function 5Bh), or
    DOS (int 21h, function 3Ch) depending on the value of the
    FailIfExists parameter.

Arguments:

    lpFileName - Supplies the file name of the file to open.  Depending on
        the value of the FailIfExists parameter, this name may or may
        not already exist.

    dwDesiredAccess - Supplies the caller's desired access to the file.

        DesiredAccess Flags:

        GENERIC_READ - Read access to the file is requested.  This
            allows data to be read from the file and the file pointer to
            be modified.

        GENERIC_WRITE - Write access to the file is requested.  This
            allows data to be written to the file and the file pointer to
            be modified.

    dwShareMode - Supplies a set of flags that indicates how this file is
        to be shared with other openers of the file.  A value of zero
        for this parameter indicates no sharing of the file, or
        exclusive access to the file is to occur.

        ShareMode Flags:

        FILE_SHARE_READ - Other open operations may be performed on the
            file for read access.

        FILE_SHARE_WRITE - Other open operations may be performed on the
            file for write access.

    lpSecurityAttributes - Ignored on xbox

    dwCreationDisposition - Supplies a creation disposition that
        specifies how this call is to operate.  This parameter must be
        one of the following values.

        dwCreationDisposition Value:

        CREATE_NEW - Create a new file.  If the specified file already
            exists, then fail.  The attributes for the new file are what
            is specified in the dwFlagsAndAttributes parameter or'd with
            FILE_ATTRIBUTE_ARCHIVE.

        CREATE_ALWAYS - Always create the file.  If the file already
            exists, then it is overwritten.  The attributes for the new
            file are what is specified in the dwFlagsAndAttributes
            parameter or'd with FILE_ATTRIBUTE_ARCHIVE.

        OPEN_EXISTING - Open the file, but if it does not exist, then
            fail the call.

        OPEN_ALWAYS - Open the file if it exists.  If it does not exist,
            then create the file using the same rules as if the
            disposition were CREATE_NEW.

        TRUNCATE_EXISTING - Open the file, but if it does not exist,
            then fail the call.  Once opened, the file is truncated such
            that its size is zero bytes.  This disposition requires that
            the caller open the file with at least GENERIC_WRITE access.

    dwFlagsAndAttributes - Specifies flags and attributes for the file.
        The attributes are only used when the file is created (as
        opposed to opened or truncated).  Any combination of attribute
        flags is acceptable except that all other attribute flags
        override the normal file attribute, FILE_ATTRIBUTE_NORMAL.  The
        FILE_ATTRIBUTE_ARCHIVE flag is always implied.

        dwFlagsAndAttributes Flags:

        FILE_ATTRIBUTE_NORMAL - A normal file should be created.

        FILE_ATTRIBUTE_READONLY - A read-only file should be created.

        FILE_ATTRIBUTE_HIDDEN - A hidden file should be created.

        FILE_ATTRIBUTE_SYSTEM - A system file should be created.

        FILE_FLAG_WRITE_THROUGH - Indicates that the system should
            always write through any intermediate cache and go directly
            to the file.  The system may still cache writes, but may not
            lazily flush the writes.

        FILE_FLAG_OVERLAPPED - Indicates that the system should initialize
            the file so that ReadFile and WriteFile operations that may
            take a significant time to complete will return ERROR_IO_PENDING.
            An event will be set to the signalled state when the operation
            completes. When FILE_FLAG_OVERLAPPED is specified the system will
            not maintain the file pointer. The position to read/write from
            is passed to the system as part of the OVERLAPPED structure
            which is an optional parameter to ReadFile and WriteFile.

        FILE_FLAG_NO_BUFFERING - Indicates that the file is to be opened
            with no intermediate buffering or caching done by the
            system.  Reads and writes to the file must be done on sector
            boundries.  Buffer addresses for reads and writes must be
            aligned on at least disk sector boundries in memory.

        FILE_FLAG_RANDOM_ACCESS - Indicates that access to the file may
            be random. The system cache manager may use this to influence
            its caching strategy for this file.

        FILE_FLAG_SEQUENTIAL_SCAN - Indicates that access to the file
            may be sequential.  The system cache manager may use this to
            influence its caching strategy for this file.  The file may
            in fact be accessed randomly, but the cache manager may
            optimize its cacheing policy for sequential access.

        FILE_FLAG_DELETE_ON_CLOSE - Indicates that the file is to be
            automatically deleted when the last handle to it is closed.

        FILE_FLAG_BACKUP_SEMANTICS - Indicates that the file is being opened
            or created for the purposes of either a backup or a restore
            operation.  Thus, the system should make whatever checks are
            appropriate to ensure that the caller is able to override
            whatever security checks have been placed on the file to allow
            this to happen.

    hTemplateFile - Not supported on xbox (used for EA's)

Return Value:

    Not -1 - Returns an open handle to the specified file.  Subsequent
        access to the file is controlled by the DesiredAccess parameter.

    0xffffffff - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    ULONG CreateDisposition;
    ULONG CreateFlags;
    BOOL EndsInSlash;

#if DBG
    if ( NULL != hTemplateFile )
    {
        // xbox doesn't support this parameter because we don't support EA's
        RIP("CreateFile() - invalid parameter (hTemplateFile not supported)");
    }
#endif // DBG

    switch ( dwCreationDisposition ) {
        case CREATE_NEW        :
            CreateDisposition = FILE_CREATE;
            break;
        case CREATE_ALWAYS     :
            CreateDisposition = FILE_OVERWRITE_IF;
            break;
        case OPEN_EXISTING     :
            CreateDisposition = FILE_OPEN;
            break;
        case OPEN_ALWAYS       :
            CreateDisposition = FILE_OPEN_IF;
            break;
        case TRUNCATE_EXISTING :
            CreateDisposition = FILE_OVERWRITE;
            if ( !(dwDesiredAccess & GENERIC_WRITE) ) {
                XapiSetLastNTError(STATUS_INVALID_PARAMETER);
                return INVALID_HANDLE_VALUE;
                }
            break;
        default :
            XapiSetLastNTError(STATUS_INVALID_PARAMETER);
            return INVALID_HANDLE_VALUE;
        }

    RtlInitObjectString(&FileName,lpFileName);

    if ( FileName.Length > 1 &&
         lpFileName[(FileName.Length / sizeof(OCHAR))-1] == (OCHAR)'\\' ) {
        EndsInSlash = TRUE;
        }
    else {
        EndsInSlash = FALSE;
        }

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    CreateFlags = 0;
    CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING ? FILE_NO_INTERMEDIATE_BUFFERING : 0 );
    CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_WRITE_THROUGH ? FILE_WRITE_THROUGH : 0 );
    CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_OVERLAPPED ? 0 : FILE_SYNCHRONOUS_IO_NONALERT );
    CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_SEQUENTIAL_SCAN ? FILE_SEQUENTIAL_ONLY : 0 );
    CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_RANDOM_ACCESS ? FILE_RANDOM_ACCESS : 0 );
    CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_BACKUP_SEMANTICS ? FILE_OPEN_FOR_BACKUP_INTENT : 0 );

    if ( dwFlagsAndAttributes & FILE_FLAG_DELETE_ON_CLOSE ) {
        CreateFlags |= FILE_DELETE_ON_CLOSE;
        dwDesiredAccess |= DELETE;
        }

    //
    // Backup semantics allow directories to be opened
    //

    if ( !(dwFlagsAndAttributes & FILE_FLAG_BACKUP_SEMANTICS) ) {
        CreateFlags |= FILE_NON_DIRECTORY_FILE;
        }

    Status = NtCreateFile(
                &Handle,
                (ACCESS_MASK)dwDesiredAccess | SYNCHRONIZE | FILE_READ_ATTRIBUTES,
                &Obja,
                &IoStatusBlock,
                NULL,
                dwFlagsAndAttributes & (FILE_ATTRIBUTE_VALID_FLAGS & ~FILE_ATTRIBUTE_DIRECTORY),
                dwShareMode,
                CreateDisposition,
                CreateFlags
                );

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        if ( Status == STATUS_OBJECT_NAME_COLLISION ) {
            SetLastError(ERROR_FILE_EXISTS);
            }
        else if ( Status == STATUS_FILE_IS_A_DIRECTORY ) {
            if ( EndsInSlash ) {
                SetLastError(ERROR_PATH_NOT_FOUND);
                }
            else {
                SetLastError(ERROR_ACCESS_DENIED);
                }
            }
        return INVALID_HANDLE_VALUE;
        }

    //
    // if NT returns supersede/overwritten, it means that a create_always, openalways
    // found an existing copy of the file. In this case ERROR_ALREADY_EXISTS is returned
    //

    if ( (dwCreationDisposition == CREATE_ALWAYS && IoStatusBlock.Information == FILE_OVERWRITTEN) ||
         (dwCreationDisposition == OPEN_ALWAYS && IoStatusBlock.Information == FILE_OPENED) ){
        SetLastError(ERROR_ALREADY_EXISTS);
        }
    else {
        SetLastError(0);
        }

    return Handle;
}

BOOL
WINAPI
CopyFile(
    PCOSTR lpExistingFileName,
    PCOSTR lpNewFileName,
    BOOL bFailIfExists
    )

/*++

Routine Description:

    A file, its extended attributes, alternate data streams, and any other
    attributes can be copied using CopyFile.

Arguments:

    lpExistingFileName - Supplies the name of an existing file that is to be
        copied.

    lpNewFileName - Supplies the name where a copy of the existing
        files data and attributes are to be stored.

    bFailIfExists - Supplies a flag that indicates how this operation is
        to proceed if the specified new file already exists.  A value of
        TRUE specifies that this call is to fail.  A value of FALSE
        causes the call to the function to succeed whether or not the
        specified new file exists.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    BOOL b;

    b = CopyFileEx(
            lpExistingFileName,
            lpNewFileName,
            (LPPROGRESS_ROUTINE)NULL,
            (LPVOID)NULL,
            (LPBOOL)NULL,
            bFailIfExists ? COPY_FILE_FAIL_IF_EXISTS : 0
            );

    return b;
}

BOOL
CopyFileEx(
    PCOSTR lpExistingFileName,
    PCOSTR lpNewFileName,
    LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    LPVOID lpData OPTIONAL,
    LPBOOL pbCancel OPTIONAL,
    DWORD dwCopyFlags
    )
/*++

Routine Description:

    A file, its extended attributes, alternate data streams, and any other
    attributes can be copied using CopyFileEx.  CopyFileEx also provides
    callbacks and cancellability.

Arguments:

    lpExistingFileName - Supplies the name of an existing file that is to be
        copied.

    lpNewFileName - Supplies the name where a copy of the existing
        files data and attributes are to be stored.

    lpProgressRoutine - Optionally supplies the address of a callback routine
        to be called as the copy operation progresses.

    lpData - Optionally supplies a context to be passed to the progress callback
        routine.

    lpCancel - Optionally supplies the address of a boolean to be set to TRUE
        if the caller would like the copy to abort.

    dwCopyFlags - Specifies flags that modify how the file is to be copied:

        COPY_FILE_FAIL_IF_EXISTS - Indicates that the copy operation should
            fail immediately if the target file already exists.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    LPVOID CopyBuffer;
    NTSTATUS status;
    IO_STATUS_BLOCK IoStatusBlock;
    OBJECT_ATTRIBUTES ObjectAttributes;
    OBJECT_STRING PathName;
    HANDLE SourceHandle;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;
    HANDLE DestinationHandle;
    LARGE_INTEGER ByteOffset;
    DWORD dwProgress;
    ULONG BytesThisPass;
    FILE_BASIC_INFORMATION BasicInfo;
    FILE_END_OF_FILE_INFORMATION EndOfFileInfo;
    FILE_DISPOSITION_INFORMATION DispositionInfo;

    InitializeObjectAttributes(&ObjectAttributes,
                               &PathName,
                               OBJ_CASE_INSENSITIVE,
                               ObDosDevicesDirectory(),
                               NULL
                               );

    //
    // Allocate a buffer to copy the file stream.
    //

    CopyBuffer = MmAllocateSystemMemory(BASE_COPY_FILE_CHUNK, PAGE_READWRITE);

    if (CopyBuffer == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }

    //
    // Open the source file.
    //

    RtlInitObjectString(&PathName, lpExistingFileName);

    status = NtOpenFile(&SourceHandle,
                        GENERIC_READ | SYNCHRONIZE,
                        &ObjectAttributes,
                        &IoStatusBlock,
                        FILE_SHARE_READ,
                        FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE
                        );

    if (!NT_SUCCESS(status)) {
        MmFreeSystemMemory(CopyBuffer, BASE_COPY_FILE_CHUNK);
        XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Get the attributes for the source file.
    //

    status = NtQueryInformationFile(SourceHandle,
                                    &IoStatusBlock,
                                    &NetworkInfo,
                                    sizeof(NetworkInfo),
                                    FileNetworkOpenInformation
                                    );

    if (!NT_SUCCESS(status)) {
        NtClose(SourceHandle);
        MmFreeSystemMemory(CopyBuffer, BASE_COPY_FILE_CHUNK);
        XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Create the destination file.  The file is created with the attributes of
    // the source file and given an initial file allocation equal to the size of
    // the source file.
    //

    RtlInitObjectString(&PathName, lpNewFileName);

    status = NtCreateFile(&DestinationHandle,
                          GENERIC_WRITE | SYNCHRONIZE,
                          &ObjectAttributes,
                          &IoStatusBlock,
                          &NetworkInfo.EndOfFile,
                          FILE_ATTRIBUTE_NORMAL,
                          0,
                          (dwCopyFlags & COPY_FILE_FAIL_IF_EXISTS) ?
                              FILE_CREATE : FILE_OVERWRITE_IF,
                          FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE | FILE_SEQUENTIAL_ONLY
                          );

    if (!NT_SUCCESS(status)) {
        NtClose(SourceHandle);
        MmFreeSystemMemory(CopyBuffer, BASE_COPY_FILE_CHUNK);
        XapiSetLastNTError(status);
        return FALSE;
    }

    ByteOffset.QuadPart = 0;

    //
    // Invoke the callback to inform it of the start of the copy process.  Note
    // that for simplicity, we treat PROGRESS_STOP or any unknow return value as
    // PROGRESS_CANCEL.
    //

    if (lpProgressRoutine != NULL) {

        dwProgress = lpProgressRoutine(NetworkInfo.EndOfFile,
                                       ByteOffset,
                                       NetworkInfo.EndOfFile,
                                       ByteOffset,
                                       1,
                                       CALLBACK_STREAM_SWITCH,
                                       SourceHandle,
                                       DestinationHandle,
                                       lpData
                                       );

        if (dwProgress == PROGRESS_QUIET) {
            lpProgressRoutine = NULL;
        } else if (dwProgress != PROGRESS_CONTINUE) {
            status = STATUS_REQUEST_ABORTED;
            goto CancelOperation;
        }
    }

    //
    // Set the end of file for the destination file to the size of the source
    // file.
    //

    EndOfFileInfo.EndOfFile = NetworkInfo.EndOfFile;

    status = NtSetInformationFile(DestinationHandle,
                                  &IoStatusBlock,
                                  &EndOfFileInfo,
                                  sizeof(EndOfFileInfo),
                                  FileEndOfFileInformation
                                  );

    if (!NT_SUCCESS(status)) {
        goto CancelOperation;
    }

    //
    // Copy the bytes from the source file to the destination file.
    //

    while (ByteOffset.QuadPart < NetworkInfo.EndOfFile.QuadPart) {

        status = NtReadFile(SourceHandle,
                            NULL,
                            NULL,
                            NULL,
                            &IoStatusBlock,
                            CopyBuffer,
                            BASE_COPY_FILE_CHUNK,
                            &ByteOffset
                            );

        if (!NT_SUCCESS(status)) {
            break;
        }

        BytesThisPass = IoStatusBlock.Information;

        status = NtWriteFile(DestinationHandle,
                             NULL,
                             NULL,
                             NULL,
                             &IoStatusBlock,
                             CopyBuffer,
                             BytesThisPass,
                             &ByteOffset
                             );

        if (!NT_SUCCESS(status)) {
            break;
        }

        ByteOffset.QuadPart += BytesThisPass;

        //
        // Invoke the callback to inform it that another chunk has been copied.
        //

        if (lpProgressRoutine != NULL) {

            dwProgress = lpProgressRoutine(NetworkInfo.EndOfFile,
                                           ByteOffset,
                                           NetworkInfo.EndOfFile,
                                           ByteOffset,
                                           1,
                                           CALLBACK_CHUNK_FINISHED,
                                           SourceHandle,
                                           DestinationHandle,
                                           lpData
                                           );

            if (dwProgress == PROGRESS_QUIET) {
                lpProgressRoutine = NULL;
            } else if (dwProgress != PROGRESS_CONTINUE) {
                status = STATUS_REQUEST_ABORTED;
                break;
            }
        }
    }

    //
    // Apply the time stamps from the source file to the destination file.
    //

    if (NT_SUCCESS(status)) {

        BasicInfo.CreationTime = NetworkInfo.CreationTime;
        BasicInfo.LastAccessTime = NetworkInfo.LastAccessTime;
        BasicInfo.LastWriteTime = NetworkInfo.LastWriteTime;
        BasicInfo.ChangeTime = NetworkInfo.ChangeTime;
        BasicInfo.FileAttributes = NetworkInfo.FileAttributes;

        status = NtSetInformationFile(DestinationHandle,
                                      &IoStatusBlock,
                                      &BasicInfo,
                                      sizeof(BasicInfo),
                                      FileBasicInformation
                                      );
    }

    //
    // If the copy operation failed or a progress callback canceled the
    // operation, then delete the file.
    //

CancelOperation:
    MmFreeSystemMemory(CopyBuffer, BASE_COPY_FILE_CHUNK);
    NtClose(SourceHandle);

    if (!NT_SUCCESS(status)) {

#undef DeleteFile
        DispositionInfo.DeleteFile = TRUE;

        NtSetInformationFile(DestinationHandle,
                             &IoStatusBlock,
                             &DispositionInfo,
                             sizeof(DispositionInfo),
                             FileDispositionInformation
                             );

        XapiSetLastNTError(status);
        NtClose(DestinationHandle);
        return FALSE;
    }

    NtClose(DestinationHandle);
    return TRUE;
}
