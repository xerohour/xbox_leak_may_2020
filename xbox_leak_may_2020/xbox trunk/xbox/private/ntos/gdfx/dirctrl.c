/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    dirctrl.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_DIRECTORY_CONTROL.

--*/

#include "gdfx.h"

NTSTATUS
GdfxFindNextDirectoryEntry(
    IN PGDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PGDF_FCB DirectoryFcb,
    IN PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext,
    OUT PGDF_DIRECTORY_ENTRY *ReturnedDirectoryEntry,
    OUT PULONG ReturnedDirectoryByteOffset
    )
/*++

Routine Description:

    This routine finds the next directory entry that matches the query template
    specification.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the file control block that describes where to
        read the directory stream from.

    DirectoryEnumContext - Specifies the directory enumeration context.

    ReturnedDirectoryEntry - Specifies the buffer to receive the pointer to the
        directory entry for the file if found.

    ReturnedDirectoryByteOffset - Specifies the buffer to receive the byte
        offset of the entry in the directory stream.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG DirectoryByteOffset;
    POBJECT_STRING TemplateFileName;
    PVOID CacheBuffer;
    ULONG CacheBufferByteOffset;
    PGDF_DIRECTORY_ENTRY DirectoryEntry;
    OBJECT_STRING DirectoryEntryFileName;

    ASSERT(GdfxIsFlagSet(DirectoryFcb->Flags, GDF_FCB_DIRECTORY));

    DirectoryByteOffset = DirectoryEnumContext->QueryOffset;
    TemplateFileName = &DirectoryEnumContext->TemplateFileName;

    //
    // If this is an empty directory, then return now that the name isn't found.
    //

    if (DirectoryFcb->FileSize == 0) {
        return STATUS_END_OF_FILE;
    }

    //
    // Process the directory stream.
    //

    CacheBuffer = NULL;
    CacheBufferByteOffset = 0;

    while (DirectoryByteOffset < DirectoryFcb->FileSize) {

        //
        // If we haven't mapped in a cache buffer yet or if we're switching
        // sectors, then we need to switch cache buffers.
        //

        if ((CacheBuffer == NULL) ||
            (CacheBufferByteOffset != (DirectoryByteOffset & ~GDF_CD_SECTOR_MASK))) {

            if (CacheBuffer != NULL) {
                FscUnmapBuffer(CacheBuffer);
                CacheBuffer = NULL;
            }

            CacheBufferByteOffset = (DirectoryByteOffset & ~GDF_CD_SECTOR_MASK);

            status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
                GdfxSectorToPhysicalByteOffset(DirectoryFcb->FirstSector) +
                CacheBufferByteOffset, FALSE, &CacheBuffer);

            if (!NT_SUCCESS(status)) {
                goto CleanupAndExit;
            }
        }

        //
        // Make the directory byte offset relative to the current cache buffer.
        //

        DirectoryByteOffset &= GDF_CD_SECTOR_MASK;

        //
        // Compute the cache buffer relative pointer to the directory entry.
        //

        DirectoryEntry = (PGDF_DIRECTORY_ENTRY)((PUCHAR)CacheBuffer +
            DirectoryByteOffset);

        //
        // If the left and right entry indexes are negative one, then this is
        // sector padding, so we should advance to the next directory sector and
        // continue.
        //

        if ((DirectoryEntry->LeftEntryIndex == (USHORT)-1) &&
            (DirectoryEntry->RightEntryIndex == (USHORT)-1)) {
            DirectoryByteOffset = CacheBufferByteOffset + GDF_CD_SECTOR_SIZE;
            continue;
        }

        //
        // If there's not enough space in the current sector to hold the header
        // of a directory entry, then the directory entry is invalid and the
        // disk is corrupt.
        //
        // We check for these types of disk corruption in order to guard against
        // taking a potential page fault if we extend past the current sector.
        //

        if (DirectoryByteOffset > (GDF_CD_SECTOR_SIZE -
            FIELD_OFFSET(GDF_DIRECTORY_ENTRY, FileName))) {
            GdfxDbgPrint(("GDFX: found invalid directory byte offset\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        //
        // If there's not enough space in the current sector to hold the header
        // and the file name, then the directory entry is invalid and the disk
        // is corrupt.
        //

        if ((DirectoryByteOffset + DirectoryEntry->FileNameLength) >
            (GDF_CD_SECTOR_SIZE - FIELD_OFFSET(GDF_DIRECTORY_ENTRY, FileName))) {
            GdfxDbgPrint(("GDFX: found invalid directory byte offset\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        //
        // Check if the file name matches the name we're looking for.
        //

        DirectoryEntryFileName.Length = DirectoryEntry->FileNameLength;
        DirectoryEntryFileName.Buffer = DirectoryEntry->FileName;

        if (((TemplateFileName->Buffer == NULL) ||
            IoIsNameInExpression(TemplateFileName, &DirectoryEntryFileName))) {

            //
            // The file name matches the template file name.  Leave the cache
            // buffer mapped and return a pointer to the directory entry and its
            // directory byte offset to the caller.  The caller is responsible
            // for unmapping the cache buffer.
            //

            *ReturnedDirectoryEntry = DirectoryEntry;
            *ReturnedDirectoryByteOffset = CacheBufferByteOffset +
                DirectoryByteOffset;

            return STATUS_SUCCESS;
        }

        //
        // Advance to the next directory byte offset.
        //

        DirectoryByteOffset = CacheBufferByteOffset + DirectoryByteOffset +
            ALIGN_UP(FIELD_OFFSET(GDF_DIRECTORY_ENTRY, FileName) +
            DirectoryEntry->FileNameLength, sizeof(ULONG));
    }

    //
    // We reached the end of the file without finding the template file name.
    //

    status = STATUS_END_OF_FILE;

CleanupAndExit:
    if (CacheBuffer != NULL) {
        FscUnmapBuffer(CacheBuffer);
    }

    return status;
}

NTSTATUS
GdfxFsdDirectoryControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_DIRECTORY_CONTROL
    requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PGDF_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;
    PGDF_FCB DirectoryFcb;
    ULONG FileInformationClass;
    SIZE_T FileInformationBaseLength;
    PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext;
    POBJECT_STRING TemplateFileName;
    BOOLEAN InitialQuery;
    PGDF_DIRECTORY_ENTRY DirectoryEntry;
    ULONG DirectoryByteOffset;
    PFILE_DIRECTORY_INFORMATION DirectoryInformation;
    LARGE_INTEGER TimeStamp;
    ULONG FileSize;
    ULONG FileNameBytesToCopy;
    ULONG OutputBytesRemaining;

    VolumeExtension = (PGDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    DirectoryFcb = (PGDF_FCB)FileObject->FsContext;

    //
    // Synchronize the creation and access of the directory context control
    // block by acquiring the global mutex.
    //

    GdfxAcquireGlobalMutexExclusive();

    //
    // Ensure that the file object is for a directory.
    //

    if (GdfxIsFlagClear(DirectoryFcb->Flags, GDF_FCB_DIRECTORY)) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    //
    // Verify that this is a supported information class.
    //

    FileInformationClass = IrpSp->Parameters.QueryDirectory.FileInformationClass;

    switch (FileInformationClass) {

        case FileDirectoryInformation:
            FileInformationBaseLength = FIELD_OFFSET(FILE_DIRECTORY_INFORMATION,
                FileName[0]);
            break;

        case FileNamesInformation:
            FileInformationBaseLength = FIELD_OFFSET(FILE_NAMES_INFORMATION,
                FileName[0]);
            break;

        default:
            status = STATUS_INVALID_INFO_CLASS;
            goto CleanupAndExit;
    }

    //
    // The query cannot be started relative to a starting index.
    //

    if (GdfxIsFlagSet(IrpSp->Flags, SL_INDEX_SPECIFIED)) {
        status = STATUS_NOT_IMPLEMENTED;
        goto CleanupAndExit;
    }

    //
    // If this is the first query for this directory, then prepare the template
    // file name.
    //

    DirectoryEnumContext = (PDIRECTORY_ENUM_CONTEXT)FileObject->FsContext2;

    if (DirectoryEnumContext == NULL) {

        TemplateFileName = IrpSp->Parameters.QueryDirectory.FileName;

        status = IoCreateDirectoryEnumContext(TemplateFileName,
            &DirectoryEnumContext);

        if (!NT_SUCCESS(status)) {
            goto CleanupAndExit;
        }

        //
        // Connect the directory enumeration context to the file object.
        //

        FileObject->FsContext2 = DirectoryEnumContext;

        InitialQuery = TRUE;

    } else {

        InitialQuery = FALSE;
    }

    //
    // If we're to restart the directory scan, then reset the current index to
    // zero.
    //

    if (GdfxIsFlagSet(IrpSp->Flags, SL_RESTART_SCAN)) {
        DirectoryEnumContext->QueryOffset = 0;
    }

    //
    // Find the next file identifier descriptor that matches our query criteria.
    //
    // On return, DirectoryEnumContext->QueryOffset still points at the original
    // identifier.  It's only updated after we're about to successfully return
    // so that no entries are lost in the event of an invalid parameter or pool
    // allocation failure.
    //

    status = GdfxFindNextDirectoryEntry(VolumeExtension, Irp, DirectoryFcb,
        DirectoryEnumContext, &DirectoryEntry, &DirectoryByteOffset);

    if (NT_SUCCESS(status)) {

        //
        // The I/O manager has already checked that the user's buffer has enough
        // space to contain at least the header.
        //

        ASSERT(IrpSp->Parameters.QueryDirectory.Length >= FileInformationBaseLength);

        //
        // Zero out the header.
        //

        DirectoryInformation = (PFILE_DIRECTORY_INFORMATION)Irp->UserBuffer;
        RtlZeroMemory(DirectoryInformation, FileInformationBaseLength);

        //
        // For FileDirectoryInformation and FileNamesInformation, the
        // FileNameLength field is immediately before the FileName buffer.
        //

        *((PULONG)((PUCHAR)DirectoryInformation + FileInformationBaseLength -
            sizeof(ULONG))) = DirectoryEntry->FileNameLength;

        //
        // If this is a FileDirectoryInformation request, then fill in more
        // information.  We have to go to dig into the file entry descriptor
        // to get the information we need, so we'll construct a file control
        // block to get the attributes.
        //

        if (FileInformationClass == FileDirectoryInformation) {

            TimeStamp = VolumeExtension->TimeStamp;

            DirectoryInformation->CreationTime = TimeStamp;
            DirectoryInformation->LastAccessTime = TimeStamp;
            DirectoryInformation->LastWriteTime = TimeStamp;
            DirectoryInformation->ChangeTime = TimeStamp;

            if (GdfxIsFlagSet(DirectoryEntry->FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {

                DirectoryInformation->FileAttributes = FILE_ATTRIBUTE_READONLY |
                    FILE_ATTRIBUTE_DIRECTORY;

                DirectoryInformation->EndOfFile.QuadPart = 0;
                DirectoryInformation->AllocationSize.QuadPart = 0;

            } else {

                DirectoryInformation->FileAttributes = FILE_ATTRIBUTE_READONLY;

                FileSize = DirectoryEntry->FileSize;

                DirectoryInformation->EndOfFile.QuadPart = (ULONGLONG)FileSize;
                DirectoryInformation->AllocationSize.QuadPart = (ULONGLONG)FileSize;
            }
        }

        //
        // If nothing has gone wrong yet, then copy the file name to the user's
        // buffer.
        //

        if (NT_SUCCESS(status)) {

            OutputBytesRemaining =
                ALIGN_DOWN(IrpSp->Parameters.QueryDirectory.Length -
                FileInformationBaseLength, sizeof(OCHAR));
            FileNameBytesToCopy = DirectoryEntry->FileNameLength;

            if (FileNameBytesToCopy > OutputBytesRemaining) {
                FileNameBytesToCopy = OutputBytesRemaining;
                status = STATUS_BUFFER_OVERFLOW;
            } else {
                status = STATUS_SUCCESS;
            }

            RtlCopyMemory((PUCHAR)DirectoryInformation + FileInformationBaseLength,
                DirectoryEntry->FileName, FileNameBytesToCopy);

            //
            // Fill in the number of bytes that we wrote to the user's buffer.
            //

            Irp->IoStatus.Information = FileInformationBaseLength +
                FileNameBytesToCopy;

            //
            // Check that we didn't overflow the user's buffer.  The I/O manager
            // does the initial check to make sure there's enough space for the
            // static structure for a given information class, but we might
            // overflow the buffer when copying in the variable length file
            // name.
            //

            ASSERT(Irp->IoStatus.Information <=
                IrpSp->Parameters.QueryDirectory.Length);

            //
            // Update the query offset.
            //

            DirectoryEnumContext->QueryOffset = DirectoryByteOffset +
                ALIGN_UP(FIELD_OFFSET(GDF_DIRECTORY_ENTRY, FileName) +
                DirectoryEntry->FileNameLength, sizeof(ULONG));
        }

        //
        // Unmap the buffer holding the directory entry.
        //

        FscUnmapBuffer(DirectoryEntry);

    } else if (status == STATUS_END_OF_FILE) {

        //
        // If we hit the end of the directory stream, then return an appropriate
        // status code depending on whether this was the first pass through this
        // routine for this handle or not.
        //

        status = InitialQuery ? STATUS_NO_SUCH_FILE : STATUS_NO_MORE_FILES;
    }

CleanupAndExit:
    GdfxReleaseGlobalMutex();

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
