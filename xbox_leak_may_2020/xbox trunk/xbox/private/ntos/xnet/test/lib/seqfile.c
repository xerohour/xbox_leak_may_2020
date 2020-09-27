/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    seqfile.c

Abstract:

    Sequential file I/O functions

Revision History:

    08/07/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// File buffer size (must be a multiple of 512, default = 4KB)
//
UINT cfgSeqFileBufSize = 4*1024;

//
// Sequential file object
//
typedef struct _SeqFile {
    HANDLE handle;
    DWORD accessMode;
    UINT writeTotal;
    UINT bufsize;
    UINT bufptr;
    CHAR* buf;
} SeqFile;

//
// Forward function declarations
//
PRIVATE BOOL WriteDataToDisk(SeqFile* file);
PRIVATE BOOL FixupFileSize(SeqFile* file);


SeqFile*
_CreateFile(
    const CHAR* filename,
    DWORD accessMode
    )

/*++

Routine Description:

    Open a file for sequential read or write access

Arguments:

    filename - Specifies the filename
    accessMode - Access mode: GENERIC_READ or GENERIC_WRITE

Return Value:

    Pointer to the open file object
    NULL if there is an error

--*/

{
    SeqFile* file;
    OBJECT_STRING str;
    CHAR filenamebuf[MAX_PATH];
    const CHAR* prefix;
    CHAR* p;
    INT prefixlen, filenamelen;
    HANDLE handle;
    IO_STATUS_BLOCK iostatusBlock;
    OBJECT_ATTRIBUTES objAttrs;
    NTSTATUS status;

    ASSERT(filename != NULL);
    ASSERT(accessMode == GENERIC_READ || accessMode == GENERIC_WRITE);

    // HACK: munge the input filename:
    //  convert from ASCII to unicode
    //  map drive letters to prefix strings

    filenamelen = strlen(filename);
    if (filenamelen >= 3 && filename[1] == ':' && filename[2] == '\\') {
        if (filename[0] == 'c' || filename[0] == 'C') {
            filename += 3;
            filenamelen -= 3;
            prefix = "\\Device\\Harddisk0\\Partition1\\";
        } else {
            prefix = "\\??\\";
        }
    } else {
        prefix = "";
    }

    prefixlen = strlen(prefix);
    if (filenamelen == 0 || (filenamelen += prefixlen) >= MAX_PATH) {
        SetLastError(ERROR_INVALID_NAME);
        return NULL;
    }

    p = filenamebuf;
    CopyMem(p, prefix, prefixlen);

    p += prefixlen;
    strcpy(p, filename);

    // Prepare parameters to call NtCreateFile

    str.Buffer = filenamebuf;
    str.Length = (USHORT) filenamelen;
    str.MaximumLength = str.Length + 1;
    InitializeObjectAttributes(&objAttrs, &str, OBJ_CASE_INSENSITIVE, NULL, NULL);

    // Allocate memory for the file object

    file = MAlloc0(sizeof(SeqFile));
    if (!file) return NULL;
    file->handle = INVALID_HANDLE_VALUE;
    file->accessMode = accessMode;

    file->buf = VirtualAlloc(NULL, cfgSeqFileBufSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    if (!file->buf) {
        Free(file);
        return NULL;
    }

    // Open the file
    //  Notice that we always set the "no buffering" flag

    status = NtCreateFile(
               &file->handle,
               accessMode|SYNCHRONIZE|FILE_READ_ATTRIBUTES,
               &objAttrs,
               &iostatusBlock,
               NULL,
               FILE_ATTRIBUTE_NORMAL,
               (accessMode == GENERIC_READ) ? FILE_SHARE_READ : 0,
               (accessMode == GENERIC_READ) ? FILE_OPEN : FILE_OVERWRITE_IF,
               FILE_SYNCHRONOUS_IO_NONALERT |
                   FILE_SEQUENTIAL_ONLY |
                   FILE_NO_INTERMEDIATE_BUFFERING);

    if (!NT_SUCCESS(status)) {
        WARNING_("NtCreateFile failed: 0x%x", status);
        SetLastError(RtlNtStatusToDosError(status));
        _CloseFile(file);
        return NULL;
    }

    return file;
}


VOID
_CloseFile(
    SeqFile* file
    )

/*++

Routine Description:

    Close a file object returned by _CreateFile

Arguments:

    file - Points to the file object to be closed

Return Value:

    NONE

--*/

{
    if (!file) return;

    //
    // If the file was opened for writing, we to update
    // the correct file size before closing the file.
    //
    if (file->accessMode == GENERIC_WRITE) {
        FixupFileSize(file);
    }

    //
    // Close the file handle and
    // dispose of the file object
    //
    if (file->handle != INVALID_HANDLE_VALUE) {
        NTSTATUS status = NtClose(file->handle);
        if (!NT_SUCCESS(status)) {
            WARNING_("NtClose failed: 0x%x", status);
        }
    }

    if (file->buf && !VirtualFree(file->buf, 0, MEM_RELEASE)) {
        WARNING_("VirtualFree failed: %d", GetLastError());
    }
    Free(file);
}


BOOL
_ReadFile(
    SeqFile* file,
    CHAR** buf,
    UINT* buflen
    )

/*++

Routine Description:

    Read data from a file

Arguments:

    file - Points to the file object
    buf - Returns a pointer to the buffered data
    buflen - On input, contain the maximum buffer size;
        On exit, contain the actual number of bytes read

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    ASSERT(file != NULL && file->accessMode == GENERIC_READ);

    //
    // Check if we still have data left in our internal buffer
    // If not, read more data from the disk into our buffer
    //
    if (file->bufptr == file->bufsize) {
        NTSTATUS status;
        IO_STATUS_BLOCK iostatusBlock;

        file->bufptr = file->bufsize = 0;
        status = NtReadFile(
                    file->handle,
                    NULL,
                    NULL,
                    NULL,
                    &iostatusBlock,
                    file->buf,
                    cfgSeqFileBufSize,
                    NULL);

        if (status == STATUS_PENDING) {
            status = NtWaitForSingleObject(file->handle, FALSE, NULL);
            if (NT_SUCCESS(status))
                status = iostatusBlock.Status;
        }

        if (NT_SUCCESS(status)) {
            file->bufsize = iostatusBlock.Information;
        } else if (status != STATUS_END_OF_FILE) {
            WARNING_("NtReadFile failed: 0x%x", status);
            SetLastError(RtlNtStatusToDosError(status));
            return FALSE;
        }
    }

    //
    // Copy data from our internal buffer to
    // the caller-supplied output buffer
    //
    if (*buflen > file->bufsize - file->bufptr)
        *buflen = file->bufsize - file->bufptr;

    *buf = file->buf + file->bufptr;
    file->bufptr += *buflen;
    return TRUE;
}


BOOL
_WriteFile(
    SeqFile* file,
    const VOID* buf,
    UINT buflen
    )

/*++

Routine Description:

    Write data to a file

Arguments:

    file - Points to the file object
    buf - Points to the data buffer
    buflen - Specifies the buffer size

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    const CHAR* srcbuf = (const CHAR*) buf;
    ASSERT(file != NULL && file->accessMode == GENERIC_WRITE);

    while (buflen) {
        UINT count;

        // Copy data from the caller's buffer to our internal buffer
        count = min(cfgSeqFileBufSize - file->bufptr, buflen);
        CopyMem(file->buf + file->bufptr, srcbuf, count);
        srcbuf += count;
        file->bufptr += count;
        file->writeTotal += count;

        // If our buffer is full, write data out to the disk
        if (file->bufptr == cfgSeqFileBufSize && !WriteDataToDisk(file))
            return FALSE;

        buflen -= count;
    }

    return TRUE;
}


PRIVATE BOOL
FixupFileSize(
    SeqFile* file
    )

/*++

Routine Description:

    Fix up the correct file size information. This is called
    when a file that was opened for write is closed.

Arguments:

    file - Points to the file object

Return Value:

    TRUE if successful, FALSE otherwise

--*/

{
    NTSTATUS status;
    IO_STATUS_BLOCK iostatusBlock;
    FILE_END_OF_FILE_INFORMATION endOfFile;
    FILE_ALLOCATION_INFORMATION allocation;

    ASSERT(file->accessMode == GENERIC_WRITE);
    if (file->bufptr == 0) return TRUE;

    //
    // Write the last unfinished sector to disk
    //
    if (!WriteDataToDisk(file)) return FALSE;

    //
    // Set the end-of-file pointer
    //
    endOfFile.EndOfFile.QuadPart = file->writeTotal;
    status = NtSetInformationFile(
                file->handle,
                &iostatusBlock,
                &endOfFile,
                sizeof(endOfFile),
                FileEndOfFileInformation);

    if (NT_SUCCESS(status)) {
        //
        // Set the allocation based on the file size
        //
        allocation.AllocationSize.QuadPart = file->writeTotal;
        status = NtSetInformationFile(
                    file->handle,
                    &iostatusBlock,
                    &allocation,
                    sizeof(allocation),
                    FileAllocationInformation);
    }

    if (!NT_SUCCESS(status)) {
        WARNING_("Failed to set file size: 0x%x", status);
        SetLastError(RtlNtStatusToDosError(status));
        return FALSE;
    }

    return TRUE;
}


PRIVATE BOOL
WriteDataToDisk(
    SeqFile* file
    )

/*++

Routine Description:

    Write data from the file buffer to the disk

Arguments:

    file - Points to the file object

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    NTSTATUS status;
    IO_STATUS_BLOCK iostatusBlock;
    UINT count;

    // Always write in 512-byte blocks
    count = (file->bufptr + 511) & ~511;
    file->bufptr = 0;

    status = NtWriteFile(
                file->handle,
                NULL,
                NULL,
                NULL,
                &iostatusBlock,
                file->buf,
                count,
                NULL);

    if (status == STATUS_PENDING) {
        status = NtWaitForSingleObject(file->handle, FALSE, NULL);
        if (NT_SUCCESS(status))
            status = iostatusBlock.Status;
    }

    if (NT_SUCCESS(status) && count == iostatusBlock.Information)
        return TRUE;

    WARNING_("NtWriteFile failed: 0x%x", status);
    SetLastError(RtlNtStatusToDosError(status));
    return FALSE;
}

