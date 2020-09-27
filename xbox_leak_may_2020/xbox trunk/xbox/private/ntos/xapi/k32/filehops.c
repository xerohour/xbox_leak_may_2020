/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    filehops.c

Abstract:

    This module implements Win32 file handle APIs

Author:

    Mark Lucovsky (markl) 25-Sep-1990

Revision History:

--*/

#include "basedll.h"
#pragma hdrstop

BOOL
WINAPI
ReadFile(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    )

/*++

Routine Description:

    Data can be read from a file using ReadFile.

    This API is used to read data from a file.  Data is read from the
    file from the position indicated by the file pointer.  After the
    read completes, the file pointer is adjusted by the number of bytes
    actually read.  A return value of TRUE coupled with a bytes read of
    0 indicates that the file pointer was beyond the current end of the
    file at the time of the read.

Arguments:

    hFile - Supplies an open handle to a file that is to be read.  The
        file handle must have been created with GENERIC_READ access to
        the file.

    lpBuffer - Supplies the address of a buffer to receive the data read
        from the file.

    nNumberOfBytesToRead - Supplies the number of bytes to read from the
        file.

    lpNumberOfBytesRead - Returns the number of bytes read by this call.
        This parameter is always set to 0 before doing any IO or error
        checking.

    lpOverlapped - Optionally points to an OVERLAPPED structure to be used with the
    request. If NULL then the transfer starts at the current file position
    and ReadFile will not return until the operation completes.

    If the handle hFile was created without specifying FILE_FLAG_OVERLAPPED
    the file pointer is moved to the specified offset plus
    lpNumberOfBytesRead before ReadFile returns. ReadFile will wait for the
    request to complete before returning (it will not return
    ERROR_IO_PENDING).

    When FILE_FLAG_OVERLAPPED is specified, ReadFile may return
    ERROR_IO_PENDING to allow the calling function to continue processing
    while the operation completes. The event (or hFile if hEvent is NULL) will
    be set to the signalled state upon completion of the request.

    When the handle is created with FILE_FLAG_OVERLAPPED and lpOverlapped
    is set to NULL, ReadFile will return ERROR_INVALID_PARAMTER because
    the file offset is required.


Return Value:

    TRUE - The operation was successul.

    FALSE - The operation failed.  Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    DWORD InputMode;

    if ( ARGUMENT_PRESENT(lpNumberOfBytesRead) ) {
        *lpNumberOfBytesRead = 0;
        }

    if ( ARGUMENT_PRESENT( lpOverlapped ) ) {
        LARGE_INTEGER Li;

        lpOverlapped->Internal = (DWORD)STATUS_PENDING;
        Li.LowPart = lpOverlapped->Offset;
        Li.HighPart = lpOverlapped->OffsetHigh;
        Status = NtReadFile(
                hFile,
                lpOverlapped->hEvent,
                NULL,
                (ULONG_PTR)lpOverlapped->hEvent & 1 ? NULL : lpOverlapped,
                (PIO_STATUS_BLOCK)&lpOverlapped->Internal,
                lpBuffer,
                nNumberOfBytesToRead,
                &Li
                );


        if ( NT_SUCCESS(Status) && Status != STATUS_PENDING) {
            if ( ARGUMENT_PRESENT(lpNumberOfBytesRead) ) {
                *lpNumberOfBytesRead = (DWORD)lpOverlapped->InternalHigh;
                }
            return TRUE;
            }
        else
        if (Status == STATUS_END_OF_FILE) {
            if ( ARGUMENT_PRESENT(lpNumberOfBytesRead) ) {
                *lpNumberOfBytesRead = 0;
                }
            XapiSetLastNTError(Status);
            return FALSE;
            }
        else {
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
    else
        {
        Status = NtReadFile(
                hFile,
                NULL,
                NULL,
                NULL,
                &IoStatusBlock,
                lpBuffer,
                nNumberOfBytesToRead,
                NULL
                );

        if ( Status == STATUS_PENDING) {
            // Operation must complete before return & IoStatusBlock destroyed
            Status = NtWaitForSingleObject( hFile, FALSE, NULL );
            if ( NT_SUCCESS(Status)) {
                Status = IoStatusBlock.Status;
                }
            }

        if ( NT_SUCCESS(Status) ) {
            *lpNumberOfBytesRead = (DWORD)IoStatusBlock.Information;
            return TRUE;
            }
        else
        if (Status == STATUS_END_OF_FILE) {
            *lpNumberOfBytesRead = 0;
            return TRUE;
            }
        else {
            if ( NT_WARNING(Status) ) {
                *lpNumberOfBytesRead = (DWORD)IoStatusBlock.Information;
                }
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
}


BOOL
WINAPI
WriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    )

/*++

Routine Description:

    Data can be written to a file using WriteFile.

    This API is used to write data to a file.  Data is written to the
    file from the position indicated by the file pointer.  After the
    write completes, the file pointer is adjusted by the number of bytes
    actually written.

    Unlike DOS, a NumberOfBytesToWrite value of zero does not truncate
    or extend the file.  If this function is required, SetEndOfFile
    should be used.

Arguments:

    hFile - Supplies an open handle to a file that is to be written.  The
        file handle must have been created with GENERIC_WRITE access to
        the file.

    lpBuffer - Supplies the address of the data that is to be written to
        the file.

    nNumberOfBytesToWrite - Supplies the number of bytes to write to the
        file. Unlike DOS, a value of zero is interpreted a null write.

    lpNumberOfBytesWritten - Returns the number of bytes written by this
        call. Before doing any work or error processing, the API sets this
        to zero.

    lpOverlapped - Optionally points to an OVERLAPPED structure to be
        used with the request. If NULL then the transfer starts at the
        current file position and WriteFile will not return until the
        operation completes.

        If the handle <hFile> was created without specifying
        FILE_FLAG_OVERLAPPED the file pointer is moved to the specified
        offset plus lpNumberOfBytesWritten before WriteFile returns.
        WriteFile will wait for the request to complete before returning
        (it will not set ERROR_IO_PENDING).

        When FILE_FLAG_OVERLAPPED is specified, WriteFile may return
        ERROR_IO_PENDING to allow the calling function to continue processing
        while the operation completes. The event (or hFile if hEvent is NULL) will
        be set to the signalled state upon completion of the request.

        When the handle is created with FILE_FLAG_OVERLAPPED and lpOverlapped
        is set to NULL, WriteFile will return ERROR_INVALID_PARAMTER because
        the file offset is required.

Return Value:

    TRUE - The operation was a success.

    FALSE - The operation failed.  Extended error status is
        available using GetLastError.

--*/

{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;

    if ( ARGUMENT_PRESENT(lpNumberOfBytesWritten) ) {
        *lpNumberOfBytesWritten = 0;
        }

    if ( ARGUMENT_PRESENT( lpOverlapped ) ) {
        LARGE_INTEGER Li;

        lpOverlapped->Internal = (DWORD)STATUS_PENDING;
        Li.LowPart = lpOverlapped->Offset;
        Li.HighPart = lpOverlapped->OffsetHigh;
        Status = NtWriteFile(
                hFile,
                lpOverlapped->hEvent,
                NULL,
                (ULONG_PTR)lpOverlapped->hEvent & 1 ? NULL : lpOverlapped,
                (PIO_STATUS_BLOCK)&lpOverlapped->Internal,
                (PVOID)lpBuffer,
                nNumberOfBytesToWrite,
                &Li
                );

        if ( !NT_ERROR(Status) && Status != STATUS_PENDING) {
            if ( ARGUMENT_PRESENT(lpNumberOfBytesWritten) ) {
                *lpNumberOfBytesWritten = (DWORD)lpOverlapped->InternalHigh;
                }
            return TRUE;
            }
        else  {
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
    else {
        Status = NtWriteFile(
                hFile,
                NULL,
                NULL,
                NULL,
                &IoStatusBlock,
                (PVOID)lpBuffer,
                nNumberOfBytesToWrite,
                NULL
                );

        if ( Status == STATUS_PENDING) {
            // Operation must complete before return & IoStatusBlock destroyed
            Status = NtWaitForSingleObject( hFile, FALSE, NULL );
            if ( NT_SUCCESS(Status)) {
                Status = IoStatusBlock.Status;
                }
            }

        if ( NT_SUCCESS(Status)) {
            *lpNumberOfBytesWritten = (DWORD)IoStatusBlock.Information;
            return TRUE;
            }
        else {
            if ( NT_WARNING(Status) ) {
                *lpNumberOfBytesWritten = (DWORD)IoStatusBlock.Information;
                }
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
}

BOOL
WINAPI
SetEndOfFile(
    HANDLE hFile
    )

/*++

Routine Description:

    The end of file position of an open file can be set to the current
    file pointer using SetEndOfFile.

    This API is used to set the end of file position of a file to the
    same value as the current file pointer.  This has the effect of
    truncating or extending a file.  This functionality is similar to
    DOS (int 21h, function 40H with CX=0).

Arguments:

    hFile - Supplies an open handle to a file that is to be extended or
        truncated.  The file handle must have been created with
        GENERIC_WRITE access to the file.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{

    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_POSITION_INFORMATION CurrentPosition;
    FILE_END_OF_FILE_INFORMATION EndOfFile;
    FILE_ALLOCATION_INFORMATION Allocation;

    //
    // Get the current position of the file pointer
    //

    Status = NtQueryInformationFile(
                hFile,
                &IoStatusBlock,
                &CurrentPosition,
                sizeof(CurrentPosition),
                FilePositionInformation
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    //
    // Set the end of file based on the current file position
    //

    EndOfFile.EndOfFile = CurrentPosition.CurrentByteOffset;

    Status = NtSetInformationFile(
                hFile,
                &IoStatusBlock,
                &EndOfFile,
                sizeof(EndOfFile),
                FileEndOfFileInformation
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    //
    // Set the allocation based on the current file size
    //

    Allocation.AllocationSize = CurrentPosition.CurrentByteOffset;

    Status = NtSetInformationFile(
                hFile,
                &IoStatusBlock,
                &Allocation,
                sizeof(Allocation),
                FileAllocationInformation
                );
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

DWORD
WINAPI
SetFilePointer(
    HANDLE hFile,
    LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh,
    DWORD dwMoveMethod
    )

/*++

Routine Description:

    An open file's file pointer can be set using SetFilePointer.

    The purpose of this function is to update the current value of a
    file's file pointer.  Care should be taken in multi-threaded
    applications that have multiple threads sharing a file handle with
    each thread updating the file pointer and then doing a read.  This
    sequence should be treated as a critical section of code and should
    be protected using either a critical section object or a mutex
    object.

    This API provides the same functionality as DOS (int 21h, function
    42h) and OS/2's DosSetFilePtr.

Arguments:

    hFile - Supplies an open handle to a file whose file pointer is to be
        moved.  The file handle must have been created with
        GENERIC_READ or GENERIC_WRITE access to the file.

    lDistanceToMove - Supplies the number of bytes to move the file
        pointer.  A positive value moves the pointer forward in the file
        and a negative value moves backwards in the file.

    lpDistanceToMoveHigh - An optional parameter that if specified
        supplies the high order 32-bits of the 64-bit distance to move.
        If the value of this parameter is NULL, this API can only
        operate on files whose maximum size is (2**32)-2.  If this
        parameter is specified, than the maximum file size is (2**64)-2.
        This value also returns the high order 32-bits of the new value
        of the file pointer.  If this value, and the return value
        are 0xffffffff, then an error is indicated.

    dwMoveMethod - Supplies a value that specifies the starting point
        for the file pointer move.

        FILE_BEGIN - The starting point is zero or the beginning of the
            file.  If FILE_BEGIN is specified, then DistanceToMove is
            interpreted as an unsigned location for the new
            file pointer.

        FILE_CURRENT - The current value of the file pointer is used as
            the starting point.

        FILE_END - The current end of file position is used as the
            starting point.


Return Value:

    Not -1 - Returns the low order 32-bits of the new value of the file
        pointer.

    0xffffffff - If the value of lpDistanceToMoveHigh was NULL, then The
        operation failed.  Extended error status is available using
        GetLastError.  Otherwise, this is the low order 32-bits of the
        new value of the file pointer.

--*/

{

    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_POSITION_INFORMATION CurrentPosition;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;
    LARGE_INTEGER Large;

    if (ARGUMENT_PRESENT(lpDistanceToMoveHigh)) {
        Large.HighPart = *lpDistanceToMoveHigh;
        Large.LowPart = lDistanceToMove;
        }
    else {
        Large.QuadPart = lDistanceToMove;
        }
    switch (dwMoveMethod) {
        case FILE_BEGIN :
            CurrentPosition.CurrentByteOffset = Large;
                break;

        default:
            RIP("SetFilePointer() - invalid parameter (dwMoveMethod)");
            //
            // Fall through
            //
            
        case FILE_CURRENT :

            //
            // Get the current position of the file pointer
            //

            Status = NtQueryInformationFile(
                        hFile,
                        &IoStatusBlock,
                        &CurrentPosition,
                        sizeof(CurrentPosition),
                        FilePositionInformation
                        );
            if ( !NT_SUCCESS(Status) ) {
                XapiSetLastNTError(Status);
                return (DWORD)-1;
                }
            CurrentPosition.CurrentByteOffset.QuadPart += Large.QuadPart;
            break;

        case FILE_END :
            Status = NtQueryInformationFile(
                        hFile,
                        &IoStatusBlock,
                        &NetworkInfo,
                        sizeof(NetworkInfo),
                        FileNetworkOpenInformation
                        );
            if ( !NT_SUCCESS(Status) ) {
                XapiSetLastNTError(Status);
                return (DWORD)-1;
                }
            CurrentPosition.CurrentByteOffset.QuadPart =
                                NetworkInfo.EndOfFile.QuadPart + Large.QuadPart;
            break;
        }

    //
    // If the resulting file position is negative, or if the app is not
    // prepared for greater than
    // then 32 bits than fail
    //

    if ( CurrentPosition.CurrentByteOffset.QuadPart < 0 ) {
        SetLastError(ERROR_NEGATIVE_SEEK);
        return (DWORD)-1;
        }

    //
    // NOTE: leaving this parameter check in the retail build, because it
    // isn't likely to be hit right away in debug builds - which could hide
    // the problem.
    //
    if ( !ARGUMENT_PRESENT(lpDistanceToMoveHigh) &&
        (CurrentPosition.CurrentByteOffset.HighPart & MAXLONG) ) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (DWORD)-1;
        }

    //
    // Set the current file position
    //

    Status = NtSetInformationFile(
                hFile,
                &IoStatusBlock,
                &CurrentPosition,
                sizeof(CurrentPosition),
                FilePositionInformation
                );
    if ( NT_SUCCESS(Status) ) {
        if (ARGUMENT_PRESENT(lpDistanceToMoveHigh)){
            *lpDistanceToMoveHigh = CurrentPosition.CurrentByteOffset.HighPart;
            }
        if ( CurrentPosition.CurrentByteOffset.LowPart == -1 ) {
            SetLastError(0);
            }
        return CurrentPosition.CurrentByteOffset.LowPart;
        }
    else {
        XapiSetLastNTError(Status);
        if (ARGUMENT_PRESENT(lpDistanceToMoveHigh)){
            *lpDistanceToMoveHigh = -1;
            }
        return (DWORD)-1;
        }
}


BOOL
WINAPI
SetFilePointerEx(
    HANDLE hFile,
    LARGE_INTEGER liDistanceToMove,
    PLARGE_INTEGER lpNewFilePointer,
    DWORD dwMoveMethod
    )

/*++

Routine Description:

    An open file's file pointer can be set using SetFilePointer.

    The purpose of this function is to update the current value of a
    file's file pointer.  Care should be taken in multi-threaded
    applications that have multiple threads sharing a file handle with
    each thread updating the file pointer and then doing a read.  This
    sequence should be treated as a critical section of code and should
    be protected using either a critical section object or a mutex
    object.

    This API provides the same functionality as DOS (int 21h, function
    42h) and OS/2's DosSetFilePtr.

Arguments:

    hFile - Supplies an open handle to a file whose file pointer is to be
        moved.  The file handle must have been created with
        GENERIC_READ or GENERIC_WRITE access to the file.

    liDistanceToMove - Supplies the number of bytes to move the file
        pointer.  A positive value moves the pointer forward in the file
        and a negative value moves backwards in the file.

    lpNewFilePointer - An optional parameter that if specified returns
        the new file pointer

    dwMoveMethod - Supplies a value that specifies the starting point
        for the file pointer move.

        FILE_BEGIN - The starting point is zero or the beginning of the
            file.  If FILE_BEGIN is specified, then DistanceToMove is
            interpreted as an unsigned location for the new
            file pointer.

        FILE_CURRENT - The current value of the file pointer is used as
            the starting point.

        FILE_END - The current end of file position is used as the
            starting point.


Return Value:

    TRUE - The operation was successful

    FALSE - The operation failed. Extended error status is available using
        GetLastError.

--*/

{

    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_POSITION_INFORMATION CurrentPosition;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;
    LARGE_INTEGER Large;

    Large = liDistanceToMove;

    switch (dwMoveMethod) {
        case FILE_BEGIN :
            CurrentPosition.CurrentByteOffset = Large;
                break;

        default:
            RIP("SetFilePointerEx() - invalid parameter (dwMoveMethod)");
            //
            // Fall through
            //
            
        case FILE_CURRENT :

            //
            // Get the current position of the file pointer
            //

            Status = NtQueryInformationFile(
                        hFile,
                        &IoStatusBlock,
                        &CurrentPosition,
                        sizeof(CurrentPosition),
                        FilePositionInformation
                        );
            if ( !NT_SUCCESS(Status) ) {
                XapiSetLastNTError(Status);
                return FALSE;
                }
            CurrentPosition.CurrentByteOffset.QuadPart += Large.QuadPart;
            break;

        case FILE_END :
            Status = NtQueryInformationFile(
                        hFile,
                        &IoStatusBlock,
                        &NetworkInfo,
                        sizeof(NetworkInfo),
                        FileNetworkOpenInformation
                        );
            if ( !NT_SUCCESS(Status) ) {
                XapiSetLastNTError(Status);
                return FALSE;
                }
            CurrentPosition.CurrentByteOffset.QuadPart =
                                NetworkInfo.EndOfFile.QuadPart + Large.QuadPart;
            break;
        }

    //
    // If the resulting file position is negative fail
    //

    if ( CurrentPosition.CurrentByteOffset.QuadPart < 0 ) {
        SetLastError(ERROR_NEGATIVE_SEEK);
        return FALSE;
        }

    //
    // Set the current file position
    //

    Status = NtSetInformationFile(
                hFile,
                &IoStatusBlock,
                &CurrentPosition,
                sizeof(CurrentPosition),
                FilePositionInformation
                );
    if ( NT_SUCCESS(Status) ) {
        if (ARGUMENT_PRESENT(lpNewFilePointer)){
            *lpNewFilePointer = CurrentPosition.CurrentByteOffset;
            }
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}



BOOL
WINAPI
GetFileInformationByHandle(
    HANDLE hFile,
    LPBY_HANDLE_FILE_INFORMATION lpFileInformation
    )

/*++

Routine Description:


Arguments:

    hFile - Supplies an open handle to a file whose modification date and
        times are to be read.  The file handle must have been created with
        GENERIC_READ access to the file.

    lpCreationTime - An optional parameter that if specified points to
        the location to return the date and time the file was created.
        A returned time of all zero indicates that the file system
        containing the file does not support this time value.


Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    BY_HANDLE_FILE_INFORMATION LocalFileInformation;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;
    FILE_INTERNAL_INFORMATION InternalInfo;
    FILE_FS_VOLUME_INFORMATION VolumeInfo;

    Status = NtQueryVolumeInformationFile(
                hFile,
                &IoStatusBlock,
                &VolumeInfo,
                sizeof(VolumeInfo),
                FileFsVolumeInformation
                );
    if ( !NT_ERROR(Status) ) {
        LocalFileInformation.dwVolumeSerialNumber = VolumeInfo.VolumeSerialNumber;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    Status = NtQueryInformationFile(
                hFile,
                &IoStatusBlock,
                &InternalInfo,
                sizeof(InternalInfo),
                FileInternalInformation
                );
    if ( !NT_ERROR(Status) ) {
        LocalFileInformation.nFileIndexHigh = InternalInfo.IndexNumber.HighPart;
        LocalFileInformation.nFileIndexLow = InternalInfo.IndexNumber.LowPart;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    Status = NtQueryInformationFile(
                hFile,
                &IoStatusBlock,
                &NetworkInfo,
                sizeof(NetworkInfo),
                FileNetworkOpenInformation
                );

    if ( !NT_ERROR(Status) ) {
        LocalFileInformation.dwFileAttributes = NetworkInfo.FileAttributes;
        LocalFileInformation.ftCreationTime = *(LPFILETIME)&NetworkInfo.CreationTime;
        LocalFileInformation.ftLastAccessTime = *(LPFILETIME)&NetworkInfo.LastAccessTime;
        LocalFileInformation.ftLastWriteTime = *(LPFILETIME)&NetworkInfo.LastWriteTime;
        LocalFileInformation.nFileSizeHigh = NetworkInfo.EndOfFile.HighPart;
        LocalFileInformation.nFileSizeLow = NetworkInfo.EndOfFile.LowPart;
        LocalFileInformation.nNumberOfLinks = 0;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
    *lpFileInformation = LocalFileInformation;
    return TRUE;
}

BOOL
APIENTRY
GetFileTime(
    HANDLE hFile,
    LPFILETIME lpCreationTime,
    LPFILETIME lpLastAccessTime,
    LPFILETIME lpLastWriteTime
    )

/*++

Routine Description:

    The date and time that a file was created, last accessed or last
    modified can be read using GetFileTime.  File time stamps are
    returned as 64-bit values, that represent the number of 100
    nanoseconds since January 1st, 1601.  This date was chosen because
    it is the start of a new quadricentury.  At 100ns resolution 32 bits
    is good for about 429 seconds (or 7 minutes) and a 63-bit integer is
    good for about 29,247 years, or around 10,682,247 days.

    This API provides the same functionality as DOS (int 21h, function
    47H with AL=0), and provides a subset of OS/2's DosQueryFileInfo.

Arguments:

    hFile - Supplies an open handle to a file whose modification date and
        times are to be read.  The file handle must have been created with
        GENERIC_READ access to the file.

    lpCreationTime - An optional parameter that if specified points to
        the location to return the date and time the file was created.
        A returned time of all zero indicates that the file system
        containing the file does not support this time value.

    lpLastAccessTime - An optional parameter that if specified points to
        the location to return the date and time the file was last accessed.
        A returned time of all zero indicates that the file system
        containing the file does not support this time value.

    lpLastWriteTime - An optional parameter that if specified points to
        the location to return the date and time the file was last written.
        A file system must support this time and thus a valid value will
        always be returned for this time value.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;

    //
    // Get the attributes
    //

    Status = NtQueryInformationFile(
                hFile,
                &IoStatusBlock,
                &NetworkInfo,
                sizeof(NetworkInfo),
                FileNetworkOpenInformation
                );

    if ( NT_SUCCESS(Status) ) {
        if (ARGUMENT_PRESENT( lpCreationTime )) {
            *lpCreationTime = *(LPFILETIME)&NetworkInfo.CreationTime;
            }

        if (ARGUMENT_PRESENT( lpLastAccessTime )) {
            *lpLastAccessTime = *(LPFILETIME)&NetworkInfo.LastAccessTime;
            }

        if (ARGUMENT_PRESENT( lpLastWriteTime )) {
            *lpLastWriteTime = *(LPFILETIME)&NetworkInfo.LastWriteTime;
            }
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

BOOL
WINAPI
SetFileTime(
    HANDLE hFile,
    CONST FILETIME *lpCreationTime,
    CONST FILETIME *lpLastAccessTime,
    CONST FILETIME *lpLastWriteTime
    )

/*++

Routine Description:

    The date and time that a file was created, last accessed or last
    modified can be modified using SetFileTime.  File time stamps are
    returned as 64-bit values, that represent the number of 100
    nanoseconds since January 1st, 1601.  This date was chosen because
    it is the start of a new quadricentury.  At 100ns resolution 32 bits
    is good for about 429 seconds (or 7 minutes) and a 63-bit integer is
    good for about 29,247 years, or around 10,682,247 days.

    This API provides the same functionality as DOS (int 21h, function
    47H with AL=1), and provides a subset of OS/2's DosSetFileInfo.

Arguments:

    hFile - Supplies an open handle to a file whose modification date and
        times are to be written.  The file handle must have been created
        with GENERIC_WRITE access to the file.

    lpCreationTime - An optional parameter, that if specified supplies
        the new creation time for the file.  Some file system's do not
        support this time value, so this parameter may be ignored.

    lpLastAccessTime - An optional parameter, that if specified supplies
        the new last access time for the file.  Some file system's do
        not support this time value, so this parameter may be ignored.

    lpLastWriteTime - An optional parameter, that if specified supplies
        the new last write time for the file.  A file system must support
        this time value.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_BASIC_INFORMATION BasicInfo;

    //
    // Zero all the time values we can set.
    //

    RtlZeroMemory(&BasicInfo,sizeof(BasicInfo));

    //
    // For each time value that is specified, copy it to the I/O system
    // record.
    //
    if (ARGUMENT_PRESENT( lpCreationTime )) {
        BasicInfo.CreationTime.LowPart = lpCreationTime->dwLowDateTime;
        BasicInfo.CreationTime.HighPart = lpCreationTime->dwHighDateTime;
        }

    if (ARGUMENT_PRESENT( lpLastAccessTime )) {
        BasicInfo.LastAccessTime.LowPart = lpLastAccessTime->dwLowDateTime;
        BasicInfo.LastAccessTime.HighPart = lpLastAccessTime->dwHighDateTime;
        }

    if (ARGUMENT_PRESENT( lpLastWriteTime )) {
        BasicInfo.LastWriteTime.LowPart = lpLastWriteTime->dwLowDateTime;
        BasicInfo.LastWriteTime.HighPart = lpLastWriteTime->dwHighDateTime;
        }

    //
    // Set the requested times.
    //

    Status = NtSetInformationFile(
                hFile,
                &IoStatusBlock,
                &BasicInfo,
                sizeof(BasicInfo),
                FileBasicInformation
                );

    if ( NT_SUCCESS(Status) ) {
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

BOOL
WINAPI
FlushFileBuffers(
    HANDLE hFile
    )

/*++

Routine Description:

    Buffered data may be flushed out to the file using the
    FlushFileBuffers service.

    The FlushFileBuffers service causes all buffered data to be written
    to the specified file.

Arguments:

    hFile - Supplies an open handle to a file whose buffers are to be
        flushed.  The file handle must have been created with
        GENERIC_WRITE access to the file.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;

    Status = NtFlushBuffersFile(hFile,&IoStatusBlock);

    if ( NT_SUCCESS(Status) ) {
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

DWORD
WINAPI
GetFileSize(
    HANDLE hFile,
    LPDWORD lpFileSizeHigh
    )

/*++

Routine Description:

    This function returns the size of the file specified by
    hFile. It is capable of returning 64-bits worth of file size.

    The return value contains the low order 32-bits of the file's size.
    The optional lpFileSizeHigh returns the high order 32-bits of the
    file's size.

Arguments:

    hFile - Supplies an open handle to a file whose size is to be
        returned.  The handle must have been created with either
        GENERIC_READ or GENERIC_WRITE access to the file.

    lpFileSizeHigh - An optional parameter, that if specified, returns
        the high order 64-bits of the file's size.


Return Value:

    Not -1 - Returns the low order 32-bits of the specified file's size.


    0xffffffff - If the value of size of the file cannot be determined,
        or an invalid handle or handle with inappropriate access, or a
        handle to a non-file is specified, this error is returned.  If
        the file's size (low 32-bits) is -1, then this value is
        returned, and GetLastError() will return 0.  Extended error
        status is available using GetLastError.


--*/

{
    BOOL b;
    LARGE_INTEGER Li;

    b = GetFileSizeEx(hFile,&Li);

    if ( b ) {

        if ( ARGUMENT_PRESENT(lpFileSizeHigh) ) {
            *lpFileSizeHigh = (DWORD)Li.HighPart;
            }
        if (Li.LowPart == -1 ) {
            SetLastError(0);
            }
        }
    else {
        Li.LowPart = -1;
        }

    return Li.LowPart;
}

BOOL
WINAPI
GetFileSizeEx(
    HANDLE hFile,
    PLARGE_INTEGER lpFileSize
    )

/*++

Routine Description:

    This function returns the size of the file specified by
    hFile. It is capable of returning 64-bits worth of file size.

Arguments:

    hFile - Supplies an open handle to a file whose size is to be
        returned.  The handle must have been created with either
        GENERIC_READ or GENERIC_WRITE access to the file.

    lpFileSize - Returns the files size


Return Value:

    TRUE - The operation was successful


    FALSE - The operation failed. Extended error
        status is available using GetLastError.


--*/

{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;

    Status = NtQueryInformationFile(
                hFile,
                &IoStatusBlock,
                &NetworkInfo,
                sizeof(NetworkInfo),
                FileNetworkOpenInformation
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }
    else {
        *lpFileSize = NetworkInfo.EndOfFile;
        return TRUE;
        }
}

BOOL
WINAPI
ReadFileEx(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPOVERLAPPED lpOverlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    )

/*++

Routine Description:

    Data can be read from a file using ReadFileEx.

    This API reports its completion status asynchronously by calling the
    specified lpCompletionRoutine.

    The caller of this routine uses the lpOverlappedStructure to specify
    the byte offset within the file where the read is to begin from.
    For files that do not support this concept (pipes...), the starting
    file offset is ignored.

    Upon successful completion of this API (return value of TRUE), the
    calling thread has an I/O outstanding.  When the I/O completes, and
    the thread is blocked in an alertable wait, the lpCompletionRoutine
    will be called and the wait will return with a return code of
    WAIT_IO_COMPLETION.  If the I/O completes, but the thread issuing
    the I/O is not in an alertable wait, the call to the completion
    routine is queued until the thread executes an alertable wait.

    If this API fails (by returning FALSE), GetLastError can be used to
    get additional error information.  If this call fails because the
    thread issued a read beyond the end of file, GetLastError will
    return a value of ERROR_HANDLE_EOF.

Arguments:

    hFile - Supplies an open handle to a file that is to be read.  The
        file handle must have been created with GENERIC_READ access to
        the file.  The file must have been created with the
        FILE_FLAG_OVERLAPPED flag.

    lpBuffer - Supplies the address of a buffer to receive the data read
        from the file.

    nNumberOfBytesToRead - Supplies the number of bytes to read from the
        file.

    lpOverlapped - Supplies the address of an OVERLAPPED structure to be
        used with the request.  The caller of this function must specify
        a starting byte offset within the file to start the read from.
        It does this using the Offset and OffsetHigh fields of the
        overlapped structure.  This call does not use or modify the
        hEvent field of the overlapped structure.  The caller may use
        this field for any purpose.  This API does use the Internal and
        InternalHigh fields of the overlapped structure, the thread
        should not manipulate this.  The lpOverlapped structure must
        remain valid for the duration of the I/O.  It is not a good idea
        to make it a local variable and then possibly returning from the
        routine with the I/O that is using this structure still pending.

Return Value:

    TRUE - The operation was successul.  Completion status will be
        propagated to the caller using the completion callback
        mechanism.  Note that this information is only made available to
        the thread that issued the I/O, and only when the I/O completes,
        and the thread is executing in an alertable wait.

    FALSE - The operation failed.  Extended error status is available
        using GetLastError. Note that end of file is treated as a failure
        with an error code of ERROR_HANDLE_EOF.

--*/
{
    NTSTATUS Status;
    LARGE_INTEGER Li;

    Li.LowPart = lpOverlapped->Offset;
    Li.HighPart = lpOverlapped->OffsetHigh;

    Status = NtReadFile(
                hFile,
                NULL,
                NtUserIoApcDispatcher,
                (PVOID) lpCompletionRoutine,
                (PIO_STATUS_BLOCK)&lpOverlapped->Internal,
                lpBuffer,
                nNumberOfBytesToRead,
                &Li
                );
    if ( NT_ERROR(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }
    else {
        return TRUE;
        }
}

BOOL
WINAPI
WriteFileEx(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPOVERLAPPED lpOverlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    )

/*++

Routine Description:

    Data can be written to a file using WriteFileEx.

    This API reports its completion status asynchronously by calling the
    specified lpCompletionRoutine.

    The caller of this routine uses the lpOverlappedStructure to specify
    the byte offset within the file where the write is to begin.
    For files that do not support this concept (pipes...), the starting
    file offset is ignored.

    Upon successful completion of this API (return value of TRUE), the
    calling thread has an I/O outstanding.  When the I/O completes, and
    the thread is blocked in an alertable wait, the lpCompletionRoutine
    will be called and the wait will return with a return code of
    WAIT_IO_COMPLETION.  If the I/O completes, but the thread issuing
    the I/O is not in an alertable wait, the call to the completion
    routine is queued until the thread executes an alertable wait.

    If this API fails (by returning FALSE), GetLastError can be used to
    get additional error information.

    Unlike DOS, a NumberOfBytesToWrite value of zero does not truncate
    or extend the file.  If this function is required, SetEndOfFile
    should be used.

Arguments:

    hFile - Supplies an open handle to a file that is to be written.  The
        file handle must have been created with GENERIC_WRITE access to
        the file.

    lpBuffer - Supplies the address of the data that is to be written to
        the file.

    nNumberOfBytesToWrite - Supplies the number of bytes to write to the
        file. Unlike DOS, a value of zero is interpreted a null write.

    lpOverlapped - Supplies the address of an OVERLAPPED structure to be
        used with the request.  The caller of this function must specify
        a starting byte offset within the file to start the write to.
        It does this using the Offset and OffsetHigh fields of the
        overlapped structure.  This call does not use or modify the
        hEvent field of the overlapped structure.  The caller may use
        this field for any purpose.  This API does use the Internal and
        InternalHigh fields of the overlapped structure, the thread
        should not manipulate this.  The lpOverlapped structure must
        remain valid for the duration of the I/O.  It is not a good idea
        to make it a local variable and then possibly returning from the
        routine with the I/O that is using this structure still pending.

Return Value:

    TRUE - The operation was successul.  Completion status will be
        propagated to the caller using the completion callback
        mechanism.  Note that this information is only made available to
        the thread that issued the I/O, and only when the I/O completes,
        and the thread is executing in an alertable wait.

    FALSE - The operation failed.  Extended error status is available
        using GetLastError. Note that end of file is treated as a failure
        with an error code of ERROR_HANDLE_EOF.

--*/
{
    NTSTATUS Status;
    LARGE_INTEGER Li;

    Li.LowPart = lpOverlapped->Offset;
    Li.HighPart = lpOverlapped->OffsetHigh;

    Status = NtWriteFile(
                hFile,
                NULL,
                NtUserIoApcDispatcher,
                (PVOID) lpCompletionRoutine,
                (PIO_STATUS_BLOCK)&lpOverlapped->Internal,
                (LPVOID)lpBuffer,
                nNumberOfBytesToWrite,
                &Li
                );
    if ( NT_ERROR(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }
    else {
        return TRUE;
        }
}

BOOL
WINAPI
DeviceIoControl(
    HANDLE hDevice,
    DWORD dwIoControlCode,
    LPVOID lpInBuffer,
    DWORD nInBufferSize,
    LPVOID lpOutBuffer,
    DWORD nOutBufferSize,
    LPDWORD lpBytesReturned,
    LPOVERLAPPED lpOverlapped
    )

/*++

Routine Description:

    An operation on a device may be performed by calling the device driver
    directly using the DeviceIoContrl function.

    The device driver must first be opened to get a valid handle.

Arguments:

    hDevice - Supplies an open handle a device on which the operation is to
        be performed.

    dwIoControlCode - Supplies the control code for the operation. This
        control code determines on which type of device the operation must
        be performed and determines exactly what operation is to be
        performed.

    lpInBuffer - Suplies an optional pointer to an input buffer that contains
        the data required to perform the operation.  Whether or not the
        buffer is actually optional is dependent on the IoControlCode.

    nInBufferSize - Supplies the length of the input buffer in bytes.

    lpOutBuffer - Suplies an optional pointer to an output buffer into which
        the output data will be copied. Whether or not the buffer is actually
        optional is dependent on the IoControlCode.

    nOutBufferSize - Supplies the length of the output buffer in bytes.

    lpBytesReturned - Supplies a pointer to a dword which will receive the
        actual length of the data returned in the output buffer.

    lpOverlapped - An optional parameter that supplies an overlap structure to
        be used with the request. If NULL or the handle was created without
        FILE_FLAG_OVERLAPPED then the DeviceIoControl will not return until
        the operation completes.

        When lpOverlapped is supplied and FILE_FLAG_OVERLAPPED was specified
        when the handle was created, DeviceIoControl may return
        ERROR_IO_PENDING to allow the caller to continue processing while the
        operation completes. The event (or File handle if hEvent == NULL) will
        be set to the not signalled state before ERROR_IO_PENDING is
        returned. The event will be set to the signalled state upon completion
        of the request. GetOverlappedResult is used to determine the result
        when ERROR_IO_PENDING is returned.

Return Value:

    TRUE -- The operation was successful.

    FALSE -- The operation failed. Extended error status is available using
        GetLastError.

--*/
{

    NTSTATUS Status;
    BOOLEAN DevIoCtl;

    if ( dwIoControlCode >> 16 == FILE_DEVICE_FILE_SYSTEM ) {
        DevIoCtl = FALSE;
        }
    else {
        DevIoCtl = TRUE;
        }

    if ( ARGUMENT_PRESENT( lpOverlapped ) ) {
        lpOverlapped->Internal = (DWORD)STATUS_PENDING;

        if ( DevIoCtl ) {

            Status = NtDeviceIoControlFile(
                        hDevice,
                        lpOverlapped->hEvent,
                        NULL,             // APC routine
                        (ULONG_PTR)lpOverlapped->hEvent & 1 ? NULL : lpOverlapped,
                        (PIO_STATUS_BLOCK)&lpOverlapped->Internal,
                        dwIoControlCode,  // IoControlCode
                        lpInBuffer,       // Buffer for data to the FS
                        nInBufferSize,
                        lpOutBuffer,      // OutputBuffer for data from the FS
                        nOutBufferSize    // OutputBuffer Length
                        );
            }
        else {

            Status = NtFsControlFile(
                        hDevice,
                        lpOverlapped->hEvent,
                        NULL,             // APC routine
                        (ULONG_PTR)lpOverlapped->hEvent & 1 ? NULL : lpOverlapped,
                        (PIO_STATUS_BLOCK)&lpOverlapped->Internal,
                        dwIoControlCode,  // IoControlCode
                        lpInBuffer,       // Buffer for data to the FS
                        nInBufferSize,
                        lpOutBuffer,      // OutputBuffer for data from the FS
                        nOutBufferSize    // OutputBuffer Length
                        );

            }

        // handle warning value STATUS_BUFFER_OVERFLOW somewhat correctly
        if ( !NT_ERROR(Status) && ARGUMENT_PRESENT(lpBytesReturned) ) {
            *lpBytesReturned = (DWORD)lpOverlapped->InternalHigh;
            }
        if ( NT_SUCCESS(Status) && Status != STATUS_PENDING) {
            return TRUE;
            }
        else {
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
    else
        {
        IO_STATUS_BLOCK Iosb;

        if ( DevIoCtl ) {
            Status = NtDeviceIoControlFile(
                        hDevice,
                        NULL,
                        NULL,             // APC routine
                        NULL,             // APC Context
                        &Iosb,
                        dwIoControlCode,  // IoControlCode
                        lpInBuffer,       // Buffer for data to the FS
                        nInBufferSize,
                        lpOutBuffer,      // OutputBuffer for data from the FS
                        nOutBufferSize    // OutputBuffer Length
                        );
            }
        else {
            Status = NtFsControlFile(
                        hDevice,
                        NULL,
                        NULL,             // APC routine
                        NULL,             // APC Context
                        &Iosb,
                        dwIoControlCode,  // IoControlCode
                        lpInBuffer,       // Buffer for data to the FS
                        nInBufferSize,
                        lpOutBuffer,      // OutputBuffer for data from the FS
                        nOutBufferSize    // OutputBuffer Length
                        );
            }

        if ( Status == STATUS_PENDING) {
            // Operation must complete before return & Iosb destroyed
            Status = NtWaitForSingleObject( hDevice, FALSE, NULL );
            if ( NT_SUCCESS(Status)) {
                Status = Iosb.Status;
                }
            }

        if ( NT_SUCCESS(Status) ) {
            *lpBytesReturned = (DWORD)Iosb.Information;
            return TRUE;
            }
        else {
            // handle warning value STATUS_BUFFER_OVERFLOW somewhat correctly
            if ( !NT_ERROR(Status) ) {
                *lpBytesReturned = (DWORD)Iosb.Information;
            }
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
}

BOOL
WINAPI
ReadFileScatter(
    HANDLE hFile,
    FILE_SEGMENT_ELEMENT aSegementArray[],
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    )
/*++

Routine Description:

    Data can be read from a file using ReadFileScatter.  The data
    is then scatter to specified buffer segements.

    This API is used to read data from a file.  Data is read from the
    file from the position indicated by the file pointer.  After the
    read completes, the file pointer is adjusted by the number of bytes
    actually read.  A return value of TRUE coupled with a bytes read of
    0 indicates that the file pointer was beyond the current end of the
    file at the time of the read.

Arguments:

    hFile - Supplies an open handle to a file that is to be read.  The
        file handle must have been created with GENERIC_READ access to
        the file.

    aSegementArray - Supplies a pointer an array of virtual segments.
        A virtual segment is a memory buffer where part of the transferred data
        should be placed.  Segments are have a fix size of PAGE_SIZE
        and must be aligned on a PAGE_SIZE boundary.

    nNumberOfBytesToRead - Supplies the number of bytes to read from the file.

    lpNumberOfBytesRead - Returns the number of bytes read by this call.
        This parameter is always set to 0 before doing any IO or error
        checking.

    lpOverlapped - Optionally points to an OVERLAPPED structure to be used with the
    request. If NULL then the transfer starts at the current file position
    and ReadFile will not return until the operation completes.

    If the handle hFile was created without specifying FILE_FLAG_OVERLAPPED
    the file pointer is moved to the specified offset plus
    lpNumberOfBytesRead before ReadFile returns. ReadFile will wait for the
    request to complete before returning (it will not return
    ERROR_IO_PENDING).

    When FILE_FLAG_OVERLAPPED is specified, ReadFile may return
    ERROR_IO_PENDING to allow the calling function to continue processing
    while the operation completes. The event (or hFile if hEvent is NULL) will
    be set to the signalled state upon completion of the request.

    When the handle is created with FILE_FLAG_OVERLAPPED and lpOverlapped
    is set to NULL, ReadFile will return ERROR_INVALID_PARAMTER because
    the file offset is required.

Return Value:

    TRUE - The operation was successul.

    FALSE - The operation failed.  Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;

    if ( ARGUMENT_PRESENT(lpNumberOfBytesRead) ) {
        *lpNumberOfBytesRead = 0;
        }

    if ( ARGUMENT_PRESENT( lpOverlapped ) ) {
        LARGE_INTEGER Li;

        lpOverlapped->Internal = (DWORD)STATUS_PENDING;
        Li.LowPart = lpOverlapped->Offset;
        Li.HighPart = lpOverlapped->OffsetHigh;
        Status = NtReadFileScatter(
                hFile,
                lpOverlapped->hEvent,
                NULL,
                (ULONG_PTR)lpOverlapped->hEvent & 1 ? NULL : lpOverlapped,
                (PIO_STATUS_BLOCK)&lpOverlapped->Internal,
                aSegementArray,
                nNumberOfBytesToRead,
                &Li
                );

        if ( NT_SUCCESS(Status) && Status != STATUS_PENDING) {
            if ( ARGUMENT_PRESENT(lpNumberOfBytesRead) ) {
                *lpNumberOfBytesRead = (DWORD)lpOverlapped->InternalHigh;
                }
            return TRUE;
            }
        else
        if (Status == STATUS_END_OF_FILE) {
            if ( ARGUMENT_PRESENT(lpNumberOfBytesRead) ) {
                *lpNumberOfBytesRead = 0;
                }
            XapiSetLastNTError(Status);
            return FALSE;
            }
        else {
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
    else
        {
        Status = NtReadFileScatter(
                hFile,
                NULL,
                NULL,
                NULL,
                &IoStatusBlock,
                aSegementArray,
                nNumberOfBytesToRead,
                NULL
                );

        if ( Status == STATUS_PENDING) {
            // Operation must complete before return & IoStatusBlock destroyed
            Status = NtWaitForSingleObject( hFile, FALSE, NULL );
            if ( NT_SUCCESS(Status)) {
                Status = IoStatusBlock.Status;
                }
            }

        if ( NT_SUCCESS(Status) ) {
            *lpNumberOfBytesRead = (DWORD)IoStatusBlock.Information;
            return TRUE;
            }
        else
        if (Status == STATUS_END_OF_FILE) {
            *lpNumberOfBytesRead = 0;
            return TRUE;
            }
        else {
            if ( NT_WARNING(Status) ) {
                *lpNumberOfBytesRead = (DWORD)IoStatusBlock.Information;
                }
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
}


BOOL
WINAPI
WriteFileGather(
    HANDLE hFile,
    FILE_SEGMENT_ELEMENT aSegementArray[],
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    )

/*++

Routine Description:

    Data can be written to a file using WriteFileGather.  The data can
    be in multple file segement buffers.

    This API is used to write data to a file.  Data is written to the
    file from the position indicated by the file pointer.  After the
    write completes, the file pointer is adjusted by the number of bytes
    actually written.

    Unlike DOS, a NumberOfBytesToWrite value of zero does not truncate
    or extend the file.  If this function is required, SetEndOfFile
    should be used.

Arguments:

    hFile - Supplies an open handle to a file that is to be written.  The
        file handle must have been created with GENERIC_WRITE access to
        the file.

    aSegementArray - Supplies a pointer an array of virtual segments.
        A virtual segment is a memory buffer where part of the transferred data
        should be placed.  Segments are have a fix size of PAGE_SIZE
        and must be aligned on a PAGE_SIZE boundary. The number of
        entries in the array must be equal to nNumberOfBytesToRead /
        PAGE_SIZE.

    nNumberOfBytesToWrite - Supplies the number of bytes to write to the
        file. Unlike DOS, a value of zero is interpreted a null write.

    lpNumberOfBytesWritten - Returns the number of bytes written by this
        call. Before doing any work or error processing, the API sets this
        to zero.

    lpOverlapped - Optionally points to an OVERLAPPED structure to be
        used with the request. If NULL then the transfer starts at the
        current file position and WriteFileGather will not return until the
        operation completes.

        If the handle <hFile> was created without specifying
        FILE_FLAG_OVERLAPPED the file pointer is moved to the specified
        offset plus lpNumberOfBytesWritten before WriteFile returns.
        WriteFile will wait for the request to complete before returning
        (it will not set ERROR_IO_PENDING).

        When FILE_FLAG_OVERLAPPED is specified, WriteFile may return
        ERROR_IO_PENDING to allow the calling function to continue processing
        while the operation completes. The event (or hFile if hEvent is NULL) will
        be set to the signalled state upon completion of the request.

        When the handle is created with FILE_FLAG_OVERLAPPED and lpOverlapped
        is set to NULL, WriteFile will return ERROR_INVALID_PARAMTER because
        the file offset is required.

Return Value:

    TRUE - The operation was a success.

    FALSE - The operation failed.  Extended error status is
        available using GetLastError.

--*/

{

    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;

    if ( ARGUMENT_PRESENT(lpNumberOfBytesWritten) ) {
        *lpNumberOfBytesWritten = 0;
        }

    if ( ARGUMENT_PRESENT( lpOverlapped ) ) {
        LARGE_INTEGER Li;

        lpOverlapped->Internal = (DWORD)STATUS_PENDING;
        Li.LowPart = lpOverlapped->Offset;
        Li.HighPart = lpOverlapped->OffsetHigh;
        Status = NtWriteFileGather(
                hFile,
                lpOverlapped->hEvent,
                NULL,
                (ULONG_PTR)lpOverlapped->hEvent & 1 ? NULL : lpOverlapped,
                (PIO_STATUS_BLOCK)&lpOverlapped->Internal,
                aSegementArray,
                nNumberOfBytesToWrite,
                &Li
                );

        if ( !NT_ERROR(Status) && Status != STATUS_PENDING) {
            if ( ARGUMENT_PRESENT(lpNumberOfBytesWritten) ) {
                *lpNumberOfBytesWritten = (DWORD)lpOverlapped->InternalHigh;
                }
            return TRUE;
            }
        else  {
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
    else {
        Status = NtWriteFileGather(
                hFile,
                NULL,
                NULL,
                NULL,
                &IoStatusBlock,
                aSegementArray,
                nNumberOfBytesToWrite,
                NULL
                );

        if ( Status == STATUS_PENDING) {
            // Operation must complete before return & IoStatusBlock destroyed
            Status = NtWaitForSingleObject( hFile, FALSE, NULL );
            if ( NT_SUCCESS(Status)) {
                Status = IoStatusBlock.Status;
                }
            }

        if ( NT_SUCCESS(Status)) {
            *lpNumberOfBytesWritten = (DWORD)IoStatusBlock.Information;
            return TRUE;
            }
        else {
            if ( NT_WARNING(Status) ) {
                *lpNumberOfBytesWritten = (DWORD)IoStatusBlock.Information;
                }
            XapiSetLastNTError(Status);
            return FALSE;
            }
        }
}
