/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    dirctrl.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_DIRECTORY_CONTROL.

--*/

#include "udfx.h"

NTSTATUS
UdfxReadNextFileIdentifierDescriptor(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PUDF_FCB DirectoryFcb,
    IN PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext,
    OUT PNSR_FID *ReturnedFileIdentifierDescriptor
    )
/*++

Routine Description:

    This routine returns the file identifier descriptor located at the
    Directory.QueryOffset of the supplied file control block.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the file control block that describes where to
        read the directory stream from.

    DirectoryEnumContext - Specifies the directory enumeration context.

    ReturnedFileIdentifierDescriptor - Specifies the buffer to receive the pool
        allocated file identifier descriptor.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID CacheBuffer;
    PNSR_FID PoolFileIdentifierDescriptor;
    ULONG FileBytesRemaining;
    ULONG LogicalSectorNumber;
    ULONG CacheBufferOffset;
    ULONG CacheBytesRemaining;
    NSR_FID StackFileIdentifierDescriptor;
    PNSR_FID FileIdentifierDescriptor;
    ULONG FileIdentifierDescriptorLength;

    ASSERT(UdfxIsFlagSet(DirectoryFcb->Flags, UDF_FCB_DIRECTORY));

    *ReturnedFileIdentifierDescriptor = NULL;

    CacheBuffer = NULL;
    PoolFileIdentifierDescriptor = NULL;

    //
    // If there aren't enough bytes left in the directory stream to contain a
    // full descriptor, then stop enumerating files.
    //

    FileBytesRemaining = DirectoryFcb->FileSize.LowPart -
        DirectoryEnumContext->QueryOffset;

    if (FileBytesRemaining < ISONsrFidConstantSize) {
        status = STATUS_END_OF_FILE;
        goto CleanupAndExit;
    }

    //
    // Compute the logical sector number of the current query offset.
    //

    LogicalSectorNumber = DirectoryFcb->AllocationSectorStart +
        (DirectoryEnumContext->QueryOffset >> UDF_CD_SECTOR_SHIFT);

    //
    // Map the directory sector into the cache.
    //

    status = UdfxMapLogicalSector(VolumeExtension, Irp, LogicalSectorNumber,
        &CacheBuffer);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Compute the current offset into the cache and the number of bytes
    // remaining in this mapping.
    //

    CacheBufferOffset = (DirectoryEnumContext->QueryOffset & UDF_CD_SECTOR_MASK);

    if (UdfxIsFlagSet(DirectoryFcb->Flags, UDF_FCB_EMBEDDED_DATA)) {
        CacheBufferOffset += DirectoryFcb->EmbeddedDataOffset;
    }

    CacheBytesRemaining = UDF_CD_SECTOR_SIZE - CacheBufferOffset;

    //
    // Check if there are enough bytes in the cache buffer to contain a full
    // header.
    //

    if (CacheBytesRemaining < ISONsrFidConstantSize) {

        //
        // Copy the first part of the descriptor to the stack.
        //

        RtlCopyMemory(&StackFileIdentifierDescriptor, (PUCHAR)CacheBuffer +
            CacheBufferOffset, CacheBytesRemaining);

        //
        // Unmap this directory sector.
        //

        FscUnmapBuffer(CacheBuffer);
        CacheBuffer = NULL;

        //
        // Advance to the next directory sector.
        //

        LogicalSectorNumber++;

        //
        // Map the directory sector into the cache.
        //

        status = UdfxMapLogicalSector(VolumeExtension, Irp, LogicalSectorNumber,
            &CacheBuffer);

        if (!NT_SUCCESS(status)) {
            goto CleanupAndExit;
        }

        //
        // Copy the last part of the descriptor to the stack.  We've already
        // validated that the stream has at least enough bytes to contain a
        // header, so this copy is safe.
        //

        RtlCopyMemory((PUCHAR)&StackFileIdentifierDescriptor +
            CacheBytesRemaining, CacheBuffer, ISONsrFidConstantSize -
            CacheBytesRemaining);

        FileIdentifierDescriptor = &StackFileIdentifierDescriptor;

    } else {

        //
        // There's enough bytes in the current buffer to contain at least the
        // header, so point directly into the cache buffer.
        //

        FileIdentifierDescriptor = (PNSR_FID)((PUCHAR)CacheBuffer +
            CacheBufferOffset);
    }

    //
    // Validate that the descriptor header looks valid and that its size
    // doesn't exceed a sector as specified by UDF or the number of
    // remaining bytes in the directory stream.  We can't use
    // UdfxVerifyDescriptor here because we don't have a contiguous
    // descriptor in memory.
    //

    FileIdentifierDescriptorLength = ISONsrFidSize(FileIdentifierDescriptor);

    if ((FileIdentifierDescriptor->Destag.Version != DESTAG_VER_CURRENT) ||
        (FileIdentifierDescriptor->Destag.Ident != DESTAG_ID_NSR_FID) ||
        (FileIdentifierDescriptorLength > UDF_CD_SECTOR_SIZE) ||
        (FileIdentifierDescriptorLength > FileBytesRemaining)) {
        status = STATUS_DISK_CORRUPT_ERROR;
        goto CleanupAndExit;
    }

    //
    // Allocate a buffer to receive the file identifier descriptor.
    //

    PoolFileIdentifierDescriptor = (PNSR_FID)ExAllocatePoolWithTag(
        FileIdentifierDescriptorLength, 'iFxU');

    if (PoolFileIdentifierDescriptor == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto CleanupAndExit;
    }

    //
    // Carefully copy the full file identifier descriptor to the pool block.
    //

    if (FileIdentifierDescriptor == &StackFileIdentifierDescriptor) {

        ASSERT(UdfxIsFlagClear(DirectoryFcb->Flags, UDF_FCB_EMBEDDED_DATA));

        //
        // The descriptor's header crossed a sector boundary.  Copy the first
        // part of the descriptor from the stack copy of the header and then
        // copy the rest from the current cache buffer.
        //

        RtlCopyMemory(PoolFileIdentifierDescriptor, &StackFileIdentifierDescriptor,
            CacheBytesRemaining);

        RtlCopyMemory((PUCHAR)PoolFileIdentifierDescriptor + CacheBytesRemaining,
            CacheBuffer, FileIdentifierDescriptorLength - CacheBytesRemaining);

    } else if (CacheBytesRemaining < FileIdentifierDescriptorLength) {

        ASSERT(UdfxIsFlagClear(DirectoryFcb->Flags, UDF_FCB_EMBEDDED_DATA));

        //
        // Part of the descriptor crosses a sector boundary, so we'll need to
        // copy as much as we can from the current cache buffer and then map in
        // the next sector.
        //

        //
        // Copy the first part of the descriptor to the pool block.
        //

        RtlCopyMemory(PoolFileIdentifierDescriptor, FileIdentifierDescriptor,
            CacheBytesRemaining);

        //
        // Unmap this directory sector.
        //

        FscUnmapBuffer(CacheBuffer);
        CacheBuffer = NULL;

        //
        // Advance to the next directory sector.
        //

        LogicalSectorNumber++;

        //
        // Map the directory sector into the cache.
        //

        status = UdfxMapLogicalSector(VolumeExtension, Irp, LogicalSectorNumber,
            &CacheBuffer);

        if (!NT_SUCCESS(status)) {
            goto CleanupAndExit;
        }

        //
        // Copy the last part of the descriptor to the pool block.
        //

        RtlCopyMemory((PUCHAR)PoolFileIdentifierDescriptor + CacheBytesRemaining,
            CacheBuffer, FileIdentifierDescriptorLength - CacheBytesRemaining);

    } else {

        //
        // The entire descriptor resides in one sector.  Copy it from the
        // current cache buffer.
        //

        RtlCopyMemory(PoolFileIdentifierDescriptor, FileIdentifierDescriptor,
            FileIdentifierDescriptorLength);
    }

    *ReturnedFileIdentifierDescriptor = PoolFileIdentifierDescriptor;
    PoolFileIdentifierDescriptor = NULL;

    status = STATUS_SUCCESS;

CleanupAndExit:
    if (PoolFileIdentifierDescriptor != NULL) {
        ExFreePool(PoolFileIdentifierDescriptor);
    }

    if (CacheBuffer != NULL) {
        FscUnmapBuffer(CacheBuffer);
    }

    return status;
}

NTSTATUS
UdfxFindNextFileIdentifierDescriptor(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PUDF_FCB DirectoryFcb,
    IN PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext,
    OUT PNSR_FID *ReturnedFileIdentifierDescriptor,
    OUT POBJECT_STRING FileName
    )
/*++

Routine Description:

    This routine finds the next file identifier descriptor that matches the
    query template specification.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the file control block that describes where to
        read the directory stream from.

    DirectoryEnumContext - Specifies the directory enumeration context.

    ReturnedFileIdentifierDescriptor - Specifies the buffer to receive the pool
        allocated file identifier descriptor.

    FileName - Specifies the buffer to receive the pool allocated matching file
        name.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PNSR_FID FileIdentifierDescriptor;

    //
    // Process the directory stream until we find a match or run out of
    // file identifier descriptors.
    //

    for (;;) {

        //
        // Read in the next file identifier descriptor.
        //

        status = UdfxReadNextFileIdentifierDescriptor(VolumeExtension, Irp,
            DirectoryFcb, DirectoryEnumContext, &FileIdentifierDescriptor);

        if (!NT_SUCCESS(status)) {
            break;
        }

        //
        // Check if the file identifier descriptor has a name.  This filters out
        // the special directory entry that would map to "..".  We don't bother
        // faking this special name or the "." directory entry.
        //

        if (FileIdentifierDescriptor->FileIDLen != 0) {

            //
            // Convert the string to a Unicode string.
            //

            status = UdfxOSTACS0StringToObjectString(
                (PUCHAR)FileIdentifierDescriptor + ISONsrFidConstantSize +
                FileIdentifierDescriptor->ImpUseLen,
                FileIdentifierDescriptor->FileIDLen, FileName);

            if (!NT_SUCCESS(status)) {
                break;
            }

            //
            // If there's no template file name, then this is an automatic
            // match.  Otherwise, check if the file name matches the template
            // file name criteria.
            //

            if ((DirectoryEnumContext->TemplateFileName.Buffer == NULL) ||
                IoIsNameInExpression(&DirectoryEnumContext->TemplateFileName, FileName)) {
                *ReturnedFileIdentifierDescriptor = FileIdentifierDescriptor;
                return STATUS_SUCCESS;
            }

            ExFreePool(FileName->Buffer);
        }

        //
        // Advance to the offset of the next file identifier descriptor.
        //

        DirectoryEnumContext->QueryOffset += ISONsrFidSize(FileIdentifierDescriptor);

        //
        // Free the memory of the current file identifier descriptor.
        //

        ExFreePool(FileIdentifierDescriptor);
        FileIdentifierDescriptor = NULL;
    }

    if (FileIdentifierDescriptor != NULL) {
        ExFreePool(FileIdentifierDescriptor);
    }

    return status;
}

NTSTATUS
UdfxFsdDirectoryControl(
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
    PUDF_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;
    PUDF_FCB DirectoryFcb;
    PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext;
    ULONG FileInformationClass;
    SIZE_T FileInformationBaseLength;
    BOOLEAN InitialQuery;
    POBJECT_STRING TemplateFileName;
    PNSR_FID FileIdentifierDescriptor;
    OBJECT_STRING FileName;
    PFILE_DIRECTORY_INFORMATION DirectoryInformation;
    PUDF_FCB InformationFcb;
    ULONG FileNameBytesToCopy;
    ULONG OutputBytesRemaining;

    VolumeExtension = (PUDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    DirectoryFcb = (PUDF_FCB)FileObject->FsContext;

    //
    // Synchronize the creation and access of the directory context control
    // block by acquiring the global mutex.
    //

    UdfxAcquireGlobalMutexExclusive();

    //
    // Ensure that the file object is for a directory.
    //

    if (UdfxIsFlagClear(DirectoryFcb->Flags, UDF_FCB_DIRECTORY)) {
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

    if (UdfxIsFlagSet(IrpSp->Flags, SL_INDEX_SPECIFIED)) {
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

    if (UdfxIsFlagSet(IrpSp->Flags, SL_RESTART_SCAN)) {
        DirectoryEnumContext->QueryOffset = 0;
    }

    //
    // Find the next file identifier descriptor that matches our query criteria.
    //
    // On return, Directory.QueryOffset still points at the matching identifier.
    // It's only updated after we're about to successfully return so that no
    // entries are lost in the event of an invalid parameter or pool allocation
    // failure.
    //

    status = UdfxFindNextFileIdentifierDescriptor(VolumeExtension, Irp,
        DirectoryFcb, DirectoryEnumContext, &FileIdentifierDescriptor, &FileName);

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
            sizeof(ULONG))) = FileName.Length;

        //
        // If this is a FileDirectoryInformation request, then fill in more
        // information.  We have to go to dig into the file entry descriptor
        // to get the information we need, so we'll construct a file control
        // block to get the attributes.
        //

        if (FileInformationClass == FileDirectoryInformation) {

            status = UdfxCreateFcbFromFileEntry(VolumeExtension, Irp,
                &FileIdentifierDescriptor->Icb, NULL, NULL, &InformationFcb);

            if (NT_SUCCESS(status)) {

                DirectoryInformation->EndOfFile =
                DirectoryInformation->AllocationSize =
                    InformationFcb->FileSize;

                DirectoryInformation->CreationTime =
                DirectoryInformation->LastAccessTime =
                DirectoryInformation->LastWriteTime =
                DirectoryInformation->ChangeTime =
                    UdfxUdfTimestampToTime(&InformationFcb->ModifyTime);

                if (UdfxIsFlagSet(InformationFcb->Flags, UDF_FCB_DIRECTORY)) {
                    DirectoryInformation->FileAttributes =
                        FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_DIRECTORY;
                } else {
                    DirectoryInformation->FileAttributes =
                        FILE_ATTRIBUTE_READONLY;
                }

                UdfxDereferenceFcb(InformationFcb);
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
            FileNameBytesToCopy = FileName.Length;

            if (FileNameBytesToCopy > OutputBytesRemaining) {
                FileNameBytesToCopy = OutputBytesRemaining;
                status = STATUS_BUFFER_OVERFLOW;
            } else {
                status = STATUS_SUCCESS;
            }

            RtlCopyMemory((PUCHAR)DirectoryInformation + FileInformationBaseLength,
                FileName.Buffer, FileNameBytesToCopy);

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

            DirectoryEnumContext->QueryOffset += ISONsrFidSize(FileIdentifierDescriptor);
        }

        ExFreePool(FileName.Buffer);
        ExFreePool(FileIdentifierDescriptor);

    } else if (status == STATUS_END_OF_FILE) {

        //
        // If we hit the end of the directory stream, then return an appropriate
        // status code depending on whether this was the first pass through this
        // routine for this handle or not.
        //

        status = InitialQuery ? STATUS_NO_SUCH_FILE : STATUS_NO_MORE_FILES;
    }

CleanupAndExit:
    UdfxReleaseGlobalMutex();

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
