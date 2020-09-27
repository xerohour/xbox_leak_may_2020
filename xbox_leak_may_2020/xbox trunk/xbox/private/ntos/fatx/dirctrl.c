/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    dirctrl.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_DIRECTORY_CONTROL.

--*/

#include "fatx.h"

NTSTATUS
FatxFindNextDirectoryEntry(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb,
    IN ULONG DirectoryByteOffset,
    IN POBJECT_STRING TemplateFileName,
    OUT PDIRENT ReturnedDirectoryEntry,
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

    DirectoryByteOffset - Specifies the directory byte offset to start scanning
        from.

    TemplateFileName - Specifies the optional file name to match.

    ReturnedDirectoryEntry - Specifies the buffer to receive the directory entry
        for the file if found.

    ReturnedDirectoryByteOffset - Specifies the buffer to receive the byte
        offset of the entry in the directory stream.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID CacheBuffer;
    ULONG ClusterNumber;
    LONG FileNameLengthLimit;
    ULONGLONG PhysicalByteOffset;
    ULONG ClusterBytesRemaining;
    ULONG ClusterByteOffset;
    ULONG PageBytesRemaining;
    PDIRENT DirectoryEntry;
    PDIRENT EndingDirectoryEntry;
    OBJECT_STRING DirectoryEntryFileName;

    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    CacheBuffer = NULL;

    //
    // If we're in the first cluster, then we can skip accessing the cluster
    // cache and go directly to the first cluster.  Otherwise, we need to go
    // through the cluster cache to map the current byte offset to the cluster
    // number.
    //

    if (DirectoryByteOffset < VolumeExtension->BytesPerCluster) {

        //
        // Get the first cluster of the directory stream and validate it.
        //

        ClusterNumber = DirectoryFcb->FirstCluster;

        if (!FatxIsValidCluster(VolumeExtension, ClusterNumber)) {
            FatxDbgPrint(("FATX: invalid starting cluster for directory\n"));
            status = STATUS_FILE_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

    } else {

        //
        // Get the cluster number corresponding to the current byte offset.
        //

        status = FatxFileByteOffsetToCluster(VolumeExtension, Irp, DirectoryFcb,
            DirectoryByteOffset, &ClusterNumber, NULL);

        if (!NT_SUCCESS(status)) {
            goto CleanupAndExit;
        }
    }

    //
    // Compute the longest file name that we'll allow to be enumerated.  If the
    // directory's path name length is near FAT_PATH_NAME_LIMIT, then we don't
    // want to return names that would cause us to overflow that limit.
    //

    FileNameLengthLimit = FAT_PATH_NAME_LIMIT - (DirectoryFcb->PathNameLength +
        sizeof(OBJ_NAME_PATH_SEPARATOR));

    if (FileNameLengthLimit > FAT_FILE_NAME_LENGTH) {
        FileNameLengthLimit = FAT_FILE_NAME_LENGTH;
    } else if (FileNameLengthLimit <= 0) {
        status = STATUS_END_OF_FILE;
        goto CleanupAndExit;
    }

    //
    // Process the directory stream.
    //

    for (;;) {

        //
        // Compute the physical byte offset of the next cluster and the number
        // of bytes remaining in this cluster.
        //

        PhysicalByteOffset = FatxClusterToPhysicalByteOffset(VolumeExtension,
            ClusterNumber);
        ClusterBytesRemaining = VolumeExtension->BytesPerCluster;

        //
        // If the directory byte offset is not cluster aligned, then bump up the
        // physical byte offset and reduce the number of bytes remaining by the
        // number of misaligned bytes.
        //

        ClusterByteOffset = FatxByteOffsetIntoCluster(VolumeExtension,
            DirectoryByteOffset);
        PhysicalByteOffset += ClusterByteOffset;
        ClusterBytesRemaining -= ClusterByteOffset;

        do {

            //
            // Map in the next page of the cluster.
            //

            status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
                PhysicalByteOffset, FALSE, &CacheBuffer);

            if (!NT_SUCCESS(status)) {
                goto CleanupAndExit;
            }

            //
            // Compute the number of bytes remaining on this cache buffer.
            //

            PageBytesRemaining = PAGE_SIZE - BYTE_OFFSET(PhysicalByteOffset);

            //
            // Adjust the number of bytes remaining in this cluster and the next
            // cache byte offset depending on whether we're near the end of the
            // cluster or not.
            //

            if (PageBytesRemaining < ClusterBytesRemaining) {
                ClusterBytesRemaining -= PageBytesRemaining;
                PhysicalByteOffset += PageBytesRemaining;
            } else {
                PageBytesRemaining = ClusterBytesRemaining;
                ClusterBytesRemaining = 0;
            }

            //
            // Process the directory entries on this cache buffer.
            //

            DirectoryEntry = (PDIRENT)CacheBuffer;
            EndingDirectoryEntry = (PDIRENT)((PUCHAR)DirectoryEntry +
                PageBytesRemaining);

            do {

                //
                // If we find a directory entry with a file name starting with
                // FAT_DIRENT_NEVER_USED or FAT_DIRENT_NEVER_USED2, then we're
                // at the end of the directory stream.
                //

                if ((DirectoryEntry->FileNameLength == FAT_DIRENT_NEVER_USED) ||
                    (DirectoryEntry->FileNameLength == FAT_DIRENT_NEVER_USED2)) {
                    status = STATUS_END_OF_FILE;
                    goto CleanupAndExit;
                }

                //
                // Check if the file name matches the name we're looking for.
                //
                // Make sure that the file name is legal so that we don't return
                // names to the caller that FatxFsdCreate will fail to locate.
                //

                DirectoryEntryFileName.Length = DirectoryEntry->FileNameLength;
                DirectoryEntryFileName.Buffer = DirectoryEntry->FileName;

                if ((DirectoryEntry->FileNameLength <= (ULONG)FileNameLengthLimit) &&
                    FatxIsValidFileAttributes(DirectoryEntry->FileAttributes) &&
                    FatxIsValidFatFileName(&DirectoryEntryFileName) &&
                    ((TemplateFileName->Buffer == NULL) ||
                    IoIsNameInExpression(TemplateFileName, &DirectoryEntryFileName))) {

                    //
                    // Copy the directory entry and its file byte offset back
                    // into the caller's buffers.
                    //

                    *ReturnedDirectoryEntry = *DirectoryEntry;
                    *ReturnedDirectoryByteOffset = DirectoryByteOffset;

                    status = STATUS_SUCCESS;
                    goto CleanupAndExit;
                }

                //
                // Advance to the next directory entry.
                //

                DirectoryEntry++;
                DirectoryByteOffset += sizeof(DIRENT);

            } while (DirectoryEntry < EndingDirectoryEntry);

            //
            // Unmap this cache buffer.
            //

            FscUnmapBuffer(CacheBuffer);
            CacheBuffer = NULL;

        } while (ClusterBytesRemaining > 0);

        //
        // Advance to the next cluster.
        //

        status = FatxFileByteOffsetToCluster(VolumeExtension, Irp, DirectoryFcb,
            DirectoryByteOffset, &ClusterNumber, NULL);

        if (!NT_SUCCESS(status)) {
            goto CleanupAndExit;
        }

        //
        // Don't allow a directory to exceed the maximum allowed directory size.
        // We check for this case after the call to FatxFileByteOffsetToCluster
        // so that a directory that's exactly the maximum allowed directory size
        // will properly fill its allocation size and ending cluster number for
        // the sake of code further downstream.
        //

        if (NT_SUCCESS(status) &&
            (DirectoryByteOffset >= FAT_MAXIMUM_DIRECTORY_FILE_SIZE)) {
            status = STATUS_FILE_CORRUPT_ERROR;
            goto CleanupAndExit;
        }
    }

CleanupAndExit:
    if (CacheBuffer != NULL) {
        FscUnmapBuffer(CacheBuffer);
    }

    return status;
}

NTSTATUS
FatxFsdDirectoryControl(
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
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;
    PFAT_FCB DirectoryFcb;
    PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext;
    POBJECT_STRING TemplateFileName;
    ULONG FileInformationClass;
    SIZE_T FileInformationBaseLength;
    BOOLEAN InitialQuery;
    DIRENT DirectoryEntry;
    ULONG DirectoryByteOffset;
    PFILE_DIRECTORY_INFORMATION DirectoryInformation;
    ULONG FileNameBytesToCopy;
    ULONG OutputBytesRemaining;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    DirectoryFcb = (PFAT_FCB)FileObject->FsContext;

    //
    // Synchronize the creation and access of the directory context control
    // block by acquiring the exclusive mutex for the volume.
    //

    FatxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto CleanupAndExit;
    }

    //
    // Ensure that the file object is for a directory.
    //

    if (FatxIsFlagClear(DirectoryFcb->Flags, FAT_FCB_DIRECTORY)) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    //
    // If the directory is marked for deletion, then the directory won't contain
    // any files and it won't allow any files to be created, so bail out now.
    // This also catches code that uses a directory file object after its handle
    // has been closed and the directory has been deleted, such as for symbolic
    // link objects.
    //

    if (FatxIsFlagSet(DirectoryFcb->Flags, FAT_FCB_DELETE_ON_CLOSE)) {
        status = STATUS_DELETE_PENDING;
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

    if (FatxIsFlagSet(IrpSp->Flags, SL_INDEX_SPECIFIED)) {
        status = STATUS_NOT_IMPLEMENTED;
        goto CleanupAndExit;
    }

    //
    // If this is the first query for this directory, then allocate a directory
    // context control block and initialize it.
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

    if (FatxIsFlagSet(IrpSp->Flags, SL_RESTART_SCAN)) {
        DirectoryEnumContext->QueryOffset = 0;
    }

    //
    // Find the next directory entry that matches our query criteria.
    //
    // On return, DirectoryEnumContext->QueryOffset still points at the original
    // identifier.  It's only updated after we're about to successfully return
    // so that no entries are lost in the event of an invalid parameter or pool
    // allocation failure.
    //

    status = FatxFindNextDirectoryEntry(VolumeExtension, Irp, DirectoryFcb,
        DirectoryEnumContext->QueryOffset,
        &DirectoryEnumContext->TemplateFileName, &DirectoryEntry,
        &DirectoryByteOffset);

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
            sizeof(ULONG))) = DirectoryEntry.FileNameLength;

        //
        // If this is a FileDirectoryInformation request, then fill in more
        // information.  We have to go to dig into the file entry descriptor
        // to get the information we need, so we'll construct a file control
        // block to get the attributes.
        //

        if (FileInformationClass == FileDirectoryInformation) {

            if (FatxIsFlagSet(DirectoryEntry.FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {

                DirectoryInformation->EndOfFile.QuadPart = 0;
                DirectoryInformation->AllocationSize.QuadPart = 0;

            } else {

                //
                // AllocationSize should be filled in with the number of clusters
                // actually allocated to the file, but we don't want to have to
                // go parse the entire FAT chain to obtain this piece of
                // information.  We'll let the caller assume that the file has
                // been allocated the number of bytes actually in the file.
                //

                DirectoryInformation->EndOfFile.QuadPart =
                    (ULONGLONG)DirectoryEntry.FileSize;
                DirectoryInformation->AllocationSize.QuadPart =
                    (ULONGLONG)DirectoryEntry.FileSize;
            }

            DirectoryInformation->CreationTime =
                FatxFatTimestampToTime(&DirectoryEntry.CreationTime);
            DirectoryInformation->LastAccessTime =
            DirectoryInformation->LastWriteTime =
            DirectoryInformation->ChangeTime =
                FatxFatTimestampToTime(&DirectoryEntry.LastWriteTime);

            if (DirectoryEntry.FileAttributes == 0) {
                DirectoryInformation->FileAttributes = FILE_ATTRIBUTE_NORMAL;
            } else {
                DirectoryInformation->FileAttributes = DirectoryEntry.FileAttributes;
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
            FileNameBytesToCopy = DirectoryEntry.FileNameLength;

            if (FileNameBytesToCopy > OutputBytesRemaining) {
                FileNameBytesToCopy = OutputBytesRemaining;
                status = STATUS_BUFFER_OVERFLOW;
            } else {
                status = STATUS_SUCCESS;
            }

            RtlCopyMemory((PUCHAR)DirectoryInformation + FileInformationBaseLength,
                DirectoryEntry.FileName, FileNameBytesToCopy);

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
                sizeof(DIRENT);

            //
            // Store the directory byte offset in the directory file control
            // block.  If the caller attempts to open this file, then we'll use
            // this to quickly locate the file's directory entry.
            //

            DirectoryFcb->Directory.DirectoryByteOffsetLookupHint =
                DirectoryByteOffset;
        }

    } else if (status == STATUS_END_OF_FILE) {

        //
        // If we hit the end of the directory stream, then return an appropriate
        // status code depending on whether this was the first pass through this
        // routine for this handle or not.
        //

        status = InitialQuery ? STATUS_NO_SUCH_FILE : STATUS_NO_MORE_FILES;
    }

CleanupAndExit:
    FatxReleaseVolumeMutex(VolumeExtension);

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
