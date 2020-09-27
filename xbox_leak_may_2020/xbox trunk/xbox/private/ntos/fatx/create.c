/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    create.c

Abstract:

    This module implements routines related to handling IRP_MJ_CREATE.

--*/

#include "fatx.h"

NTSTATUS
FatxLookupElementNameInDirectory(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb,
    IN POBJECT_STRING ElementName,
    OUT PDIRENT ReturnedDirectoryEntry,
    OUT PULONG ReturnedDirectoryByteOffset,
    OUT PULONG ReturnedEmptyDirectoryByteOffset
    )
/*++

Routine Description:

    This routine looks up the supplied file name in the supplied directory
    file control block.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the file control block that describes where to
        read the directory stream from.

    ElementName - Specifies the name to search for in the directory.

    ReturnedDirectoryEntry - Specifies the buffer to receive the directory entry
        for the file if found.

    ReturnedDirectoryByteOffset - Specifies the buffer to receive the byte
        offset of the entry in the directory stream.

    ReturnedEmptyDirectoryByteOffset - Specifies the buffer to receive the byte
        offset of the first free directory empty.  The value is only meaningful
        if STATUS_OBJECT_NAME_NOT_FOUND is returned.  MAXULONG is returned if we
        don't find an empty slot before finding the end of the directory stream.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG EmptyDirectoryByteOffset;
    ULONG DirectoryByteOffset;
    ULONGLONG PhysicalByteOffset;
    ULONG PhysicalRunLength;
    PVOID CacheBuffer;
    PDIRENT DirectoryEntry;
    OBJECT_STRING DirectoryEntryFileName;
    ULONG ClusterNumber;
    ULONG ClusterBytesRemaining;
    PDIRENT EndingDirectoryEntry;

    ASSERT(FatxIsFlagSet(DirectoryFcb->Flags, FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    CacheBuffer = NULL;
    EmptyDirectoryByteOffset = MAXULONG;

    //
    // If the directory byte offset lookup hint is non-zero, then the caller is
    // enumerating files in this directory.  To speed up opens for the file that
    // was last enumerated, we'll compare the directory entry at this byte
    // offset against the desired name.  If this fails, reset the hint so that
    // future lookups aren't impacted.
    //

    DirectoryByteOffset = DirectoryFcb->Directory.DirectoryByteOffsetLookupHint;

    if (DirectoryByteOffset != 0) {

        //
        // Get the physical byte offset corresponding to the file's directory
        // byte offset.
        //

        status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension, Irp,
            DirectoryFcb, DirectoryByteOffset, FALSE, &PhysicalByteOffset,
            &PhysicalRunLength);

        if (!NT_SUCCESS(status)) {
            goto CleanupAndExit;
        }

        //
        // Map the directory entry into the cache.
        //

        status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
            PhysicalByteOffset, FALSE, &CacheBuffer);

        if (!NT_SUCCESS(status)) {
            goto CleanupAndExit;
        }

        //
        // Check if the file name matches the name we're looking for.
        //

        DirectoryEntry = (PDIRENT)CacheBuffer;

        DirectoryEntryFileName.Length = DirectoryEntry->FileNameLength;
        DirectoryEntryFileName.Buffer = DirectoryEntry->FileName;

        if ((DirectoryEntryFileName.Length == ElementName->Length) &&
            (DirectoryEntryFileName.Length <= FAT_FILE_NAME_LENGTH) &&
            RtlEqualObjectString(&DirectoryEntryFileName, ElementName, TRUE)) {

            //
            // Copy the directory entry and its file byte offset back into the
            // caller's buffers.
            //

            *ReturnedDirectoryEntry = *DirectoryEntry;
            *ReturnedDirectoryByteOffset = DirectoryByteOffset;

            status = STATUS_SUCCESS;
            goto CleanupAndExit;
        }

        //
        // Unmap this cache buffer.
        //

        FscUnmapBuffer(CacheBuffer);
        CacheBuffer = NULL;

        //
        // Reset the directory byte offset lookup hint so that we don't keep
        // checking this specific directory entry.
        //

        DirectoryFcb->Directory.DirectoryByteOffsetLookupHint = 0;
        DirectoryByteOffset = 0;
    }

    //
    // Get the first cluster of the directory stream and validate it.
    //

    ClusterNumber = DirectoryFcb->FirstCluster;

    if (!FatxIsValidCluster(VolumeExtension, ClusterNumber)) {
        FatxDbgPrint(("FATX: invalid starting cluster for directory\n"));
        status = STATUS_FILE_CORRUPT_ERROR;
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
        // The starting byte offset for the file area is always page aligned and
        // the cluster size is a multiple of the page size, so we can make some
        // optimizations below.
        //

        ASSERT(BYTE_OFFSET(PhysicalByteOffset) == 0);
        ASSERT(BYTE_OFFSET(ClusterBytesRemaining) == 0);

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
            // Process the directory entries on this cache buffer.
            //

            DirectoryEntry = (PDIRENT)CacheBuffer;
            EndingDirectoryEntry = (PDIRENT)((PUCHAR)CacheBuffer + PAGE_SIZE);

            do {

                //
                // If we've found the last directory entry in the stream or a
                // deleted directory entry, then save off its byte offset in
                // case we want to create a new entry.
                //

                if ((DirectoryEntry->FileNameLength == FAT_DIRENT_NEVER_USED) ||
                    (DirectoryEntry->FileNameLength == FAT_DIRENT_NEVER_USED2) ||
                    (DirectoryEntry->FileNameLength == FAT_DIRENT_DELETED)) {

                    if (EmptyDirectoryByteOffset == MAXULONG) {
                        EmptyDirectoryByteOffset = DirectoryByteOffset;
                    }
                }

                //
                // If we find a directory entry with a file name starting with
                // FAT_DIRENT_NEVER_USED or FAT_DIRENT_NEVER_USED2, then we're
                // at the end of the directory stream.
                //

                if ((DirectoryEntry->FileNameLength == FAT_DIRENT_NEVER_USED) ||
                    (DirectoryEntry->FileNameLength == FAT_DIRENT_NEVER_USED2)) {
                    status = STATUS_OBJECT_NAME_NOT_FOUND;
                    goto CleanupAndExit;
                }

                //
                // Check if the file name matches the name we're looking for.
                //

                DirectoryEntryFileName.Length = DirectoryEntry->FileNameLength;
                DirectoryEntryFileName.Buffer = DirectoryEntry->FileName;

                if ((DirectoryEntryFileName.Length == ElementName->Length) &&
                    (DirectoryEntryFileName.Length <= FAT_FILE_NAME_LENGTH) &&
                    RtlEqualObjectString(&DirectoryEntryFileName, ElementName, TRUE)) {

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

            //
            // Adjust the number of bytes remaining in this cluster and the next
            // cache byte offset.
            //

            PhysicalByteOffset += PAGE_SIZE;
            ClusterBytesRemaining -= PAGE_SIZE;

        } while (ClusterBytesRemaining > 0);

        //
        // Advance to the next cluster.
        //

        status = FatxFileByteOffsetToCluster(VolumeExtension, Irp, DirectoryFcb,
            DirectoryByteOffset, &ClusterNumber, NULL);

        if (status == STATUS_END_OF_FILE) {
            status = STATUS_OBJECT_NAME_NOT_FOUND;
        }

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

#if DBG
    //
    // We should only use the empty directory byte offset when we return
    // STATUS_OBJECT_NAME_NOT_FOUND.  For any other return code, return a bogus
    // direcotory byte offset so that we catch illegal uses of the byte offset.
    //

    if (status != STATUS_OBJECT_NAME_NOT_FOUND) {
        EmptyDirectoryByteOffset = MAXULONG - 1;
    }
#endif

    *ReturnedEmptyDirectoryByteOffset = EmptyDirectoryByteOffset;

    return status;
}

NTSTATUS
FatxCheckDesiredAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG CreateOptions,
    IN UCHAR FileAttributes,
    IN BOOLEAN CreatingFile
    )
/*++

Routine Description:

    This routine checks that the desired access mask is compatible with the
    supplied file attributes.  For example, a file that's marked read-only
    can't be opened for write access.

Arguments:

    DesiredAccess - Specifies the access rights that the caller would like for
        the file handle.

    CreateOptions - Specifies the options controlling how the file is to be
        created or opened.

    FileAttributes - Specifies the file attributes of the file to be opened.

    CreatingFile - Specifies whether or not we're in the process of creating a
        new file.

Return Value:

    Status of operation.

--*/
{
    //
    // Never allow a volume ID or device to be opened.
    //

    if (!FatxIsValidFileAttributes(FileAttributes)) {
        return STATUS_ACCESS_DENIED;
    }

    //
    // Check the access mask depending on whether this is a directory or not.
    //

    if (FatxIsFlagSet(FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {

        if (FatxIsFlagSet(DesiredAccess, ~(DELETE | READ_CONTROL | WRITE_OWNER |
            WRITE_DAC | SYNCHRONIZE | ACCESS_SYSTEM_SECURITY | FILE_WRITE_DATA |
            FILE_READ_EA | FILE_WRITE_EA | FILE_READ_ATTRIBUTES |
            FILE_WRITE_ATTRIBUTES | FILE_LIST_DIRECTORY | FILE_TRAVERSE |
            FILE_DELETE_CHILD | FILE_APPEND_DATA))) {
            return STATUS_ACCESS_DENIED;
        }

    } else {

        if (FatxIsFlagSet(DesiredAccess, ~(DELETE | READ_CONTROL | WRITE_OWNER |
            WRITE_DAC | SYNCHRONIZE | ACCESS_SYSTEM_SECURITY | FILE_READ_DATA |
            FILE_WRITE_DATA | FILE_READ_EA | FILE_WRITE_EA | FILE_READ_ATTRIBUTES |
            FILE_WRITE_ATTRIBUTES | FILE_EXECUTE | FILE_DELETE_CHILD |
            FILE_APPEND_DATA))) {
            return STATUS_ACCESS_DENIED;
        }
    }

    //
    // Check the access mask if this is a read-only file.
    //

    if (FatxIsFlagSet(FileAttributes, FILE_ATTRIBUTE_READONLY)) {

        if (!CreatingFile &&
            FatxIsFlagSet(DesiredAccess, ~(DELETE | READ_CONTROL | WRITE_OWNER |
            WRITE_DAC | SYNCHRONIZE | ACCESS_SYSTEM_SECURITY | FILE_READ_DATA |
            FILE_READ_EA | FILE_WRITE_EA | FILE_READ_ATTRIBUTES |
            FILE_WRITE_ATTRIBUTES | FILE_EXECUTE | FILE_LIST_DIRECTORY |
            FILE_TRAVERSE))) {
            return STATUS_ACCESS_DENIED;
        }

        if (FatxIsFlagSet(CreateOptions, FILE_DELETE_ON_CLOSE)) {
            return STATUS_CANNOT_DELETE;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
FatxOpenTargetDirectory(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb,
    IN ULONG FileExists,
    IN BOOLEAN NoReferenceCount
    )
/*++

Routine Description:

    This routine opens the supplied target directory for a rename operation.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the file control block of the target directory to
        open.

    FileExists - Specifies the value, FILE_EXISTS or FILE_DOES_NOT_EXIST, that
        should be placed in the IRP's information field on success.

    NoReferenceCount - Specifies whether or not the reference count for
        the directory file control block should be incremented.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION IrpSp;
    ACCESS_MASK DesiredAccess;
    USHORT ShareAccess;
    PFILE_OBJECT FileObject;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    DesiredAccess = IrpSp->Parameters.Create.DesiredAccess;
    ShareAccess = IrpSp->Parameters.Create.ShareAccess;
    FileObject = IrpSp->FileObject;

    //
    // It's possible for the file control block to be NULL if a file failed to
    // be moved from one directory to another.  In that case,
    // FatxSetRenameInformation sets the file's parent file control block to
    // NULL.
    //

    if (DirectoryFcb == NULL) {
        return STATUS_FILE_CORRUPT_ERROR;
    }

    ASSERT(FatxIsFlagSet(DirectoryFcb->Flags, FAT_FCB_DIRECTORY));

    //
    // If this is the first open handle to the file, then initialize the sharing
    // mode.  Otherwise, verify that the requested sharing mode is compatible
    // with the current sharing mode.
    //

    if (DirectoryFcb->ShareAccess.OpenCount == 0) {
        IoSetShareAccess(DesiredAccess, ShareAccess, FileObject,
            &DirectoryFcb->ShareAccess);
        status = STATUS_SUCCESS;
    } else {
        status = IoCheckShareAccess(DesiredAccess, ShareAccess, FileObject,
            &DirectoryFcb->ShareAccess, TRUE);
    }

    //
    // Fill in the file object with the file control block.
    //

    if (NT_SUCCESS(status)) {

        if (!NoReferenceCount) {
            DirectoryFcb->ReferenceCount++;
        }

        FileObject->FsContext = DirectoryFcb;
        FileObject->FsContext2 = NULL;

        VolumeExtension->FileObjectCount++;

        //
        // Indicate to the caller whether or not a file already exists with the
        // specified name.
        //

        Irp->IoStatus.Information = FileExists;

        status = STATUS_SUCCESS;
    }

    return status;
}

NTSTATUS
FatxCreateNewFile(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb,
    IN POBJECT_STRING ElementName,
    IN ULONG EmptyDirectoryByteOffset,
    IN UCHAR FileAttributes,
    IN ULONG AllocationSize,
    OUT PDIRENT DirectoryEntry,
    OUT PFAT_FCB *ReturnedFcb
    )
/*++

Routine Description:

    This routine is called to create a new file or directory.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the directory to create the new file in.

    ElementName - Specifies the name of the file to create.

    EmptyDirectoryByteOffset - Specifies the byte offset of the first free
        directory entry or MAXULONG if there aren't any free entries in the
        directory.

    FileAttributes - Specifies the new attributes for the file.

    AllocationSize - Specifies the new allocation size for the file.

    DirectoryEntry - Specifies a local buffer that can be used to construct the
        directory entry.

    ReturnedFcb - Specifies the buffer to receive the created file control
        block.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    FAT_CLUSTER_RUN ClusterRuns[FAT_MAXIMUM_CLUSTER_RUNS];
    ULONG NumberOfClusterRuns;
    ULONG FirstCluster;
    ULONG EndingCluster;
    LARGE_INTEGER CreationTime;
    PFAT_FCB Fcb;

    ASSERT(FatxIsFlagSet(DirectoryFcb->Flags, FAT_FCB_DIRECTORY));
    ASSERT(ElementName->Length != 0);
    ASSERT(ElementName->Length <= FAT_FILE_NAME_LENGTH);

    //
    // If there isn't an empty directory entry, then we'll need to add another
    // cluster to the directory.
    //

    if (EmptyDirectoryByteOffset == MAXULONG) {

        //
        // If we found the end of the directory stream, then we must have
        // established how many bytes are allocated to the file.
        //

        ASSERT(DirectoryFcb->AllocationSize != MAXULONG);

        //
        // The new directory empty will be placed at the start of the new
        // extension.
        //

        EmptyDirectoryByteOffset = DirectoryFcb->AllocationSize;

        //
        // Attempt to add another cluster to the directory's allocation.
        //

        status = FatxExtendDirectoryAllocation(VolumeExtension, Irp,
            DirectoryFcb);

        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    //
    // Figure out if we need to allocate some clusters to this file now or not.
    //

    if (FatxIsFlagSet(FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {

        //
        // Allocate a single cluster.
        //

        status = FatxAllocateClusters(VolumeExtension, Irp, FAT_CLUSTER_NULL, 1,
            ClusterRuns, &NumberOfClusterRuns, &EndingCluster);

        if (!NT_SUCCESS(status)) {
            return status;
        }

        ASSERT(NumberOfClusterRuns == 1);
        ASSERT(EndingCluster == ClusterRuns[0].PhysicalClusterNumber);

        FirstCluster = ClusterRuns[0].PhysicalClusterNumber;
        AllocationSize = VolumeExtension->BytesPerCluster;

        //
        // Initialize the contents of the directory cluster.
        //

        status = FatxInitializeDirectoryCluster(VolumeExtension, Irp, FirstCluster);

        if (!NT_SUCCESS(status)) {
            goto CleanupAndExit;
        }

    } else if (AllocationSize != 0) {

        //
        // Allocate the requested number of clusters.
        //

        AllocationSize = FatxRoundToClusters(VolumeExtension, AllocationSize);

        status = FatxAllocateClusters(VolumeExtension, Irp, FAT_CLUSTER_NULL,
            AllocationSize >> VolumeExtension->ClusterShift, ClusterRuns,
            &NumberOfClusterRuns, &EndingCluster);

        if (!NT_SUCCESS(status)) {
            return status;
        }

        FirstCluster = ClusterRuns[0].PhysicalClusterNumber;

    } else {

        //
        // The file doesn't require an initial allocation.
        //

        FirstCluster = FAT_CLUSTER_NULL;
        EndingCluster = FAT_CLUSTER_NULL;
        NumberOfClusterRuns = 0;
        status = STATUS_SUCCESS;
    }

    //
    // Construct a copy of the new directory entry on the stack.
    //

    RtlZeroMemory(DirectoryEntry, sizeof(DIRENT));

    DirectoryEntry->FileNameLength = (UCHAR)ElementName->Length;

    RtlCopyMemory(DirectoryEntry->FileName, ElementName->Buffer,
        ElementName->Length);

    DirectoryEntry->FileAttributes = FileAttributes;
    DirectoryEntry->FirstCluster = FirstCluster;

    KeQuerySystemTime(&CreationTime);
    FatxTimeToFatTimestamp(&CreationTime, &DirectoryEntry->CreationTime);
    DirectoryEntry->LastWriteTime = DirectoryEntry->CreationTime;
    DirectoryEntry->LastAccessTime = DirectoryEntry->CreationTime;

    //
    // Construct a file control block for the desired file from the directory
    // entry on the stack.
    //

    status = FatxCreateFcb(DirectoryFcb, FirstCluster, DirectoryEntry,
        EmptyDirectoryByteOffset, &Fcb);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Commit the directory entry.
    //

    status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, Fcb);

    if (!NT_SUCCESS(status)) {
        FatxDereferenceFcb(Fcb);
        goto CleanupAndExit;
    }

    //
    // If the file or directory was given an initial allocation, then fill in
    // the allocation size and ending cluster number for the file control block.
    // Copy the allocated cluster runs into the file's cluster cache.
    //

    if (AllocationSize != 0) {

        ASSERT(Fcb->AllocationSize == MAXULONG);

        Fcb->AllocationSize = AllocationSize;
        Fcb->EndingCluster = EndingCluster;

        FatxAppendClusterRunsToClusterCache(Fcb, 0, ClusterRuns,
            NumberOfClusterRuns);

    } else {
        ASSERT(Fcb->AllocationSize == 0);
        ASSERT(Fcb->EndingCluster == FAT_CLUSTER_NULL);
    }

    *ReturnedFcb = Fcb;
    status = STATUS_SUCCESS;

CleanupAndExit:
    if (!NT_SUCCESS(status)) {
        FatxFreeClusters(VolumeExtension, Irp, FirstCluster, FALSE);
    }

    return status;
}

NTSTATUS
FatxOverwriteExistingFile(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB FileFcb,
    IN UCHAR FileAttributes,
    IN ULONG AllocationSize
    )
/*++

Routine Description:

    This routine is called to overwrite an existing file with the file having
    the supplied file attributes and allocation size.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    FileFcb - Specifies the file control block that describes the file to
        overwrite.

    FileAttributes - Specifies the new attributes for the file.

    AllocationSize - Specifies the new allocation size for the file.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    UCHAR OriginalFileAttributes;
    ULONG OriginalFileSize;
    LARGE_INTEGER OriginalLastWriteTime;
    FAT_TIME_STAMP OriginalCreationTime;
    FAT_TIME_STAMP OriginalLastAccessTime;

    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_DIRECTORY));

    //
    // Verify that the attributes don't try to change this file into a
    // directory.
    //

    if (FatxIsFlagSet(FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the file has already been opened, then we can't safely truncate the
    // file.  We hold the volume's mutex for exclusive access and would need to
    // acquire the file's mutex for exclusive access, but that's a violation of
    // our locking order.  Instead of trying to rearrange the code to fix the
    // locking order, treat this as a sharing violation.
    //
    // For the case of a reference count of exactly one, we know that no other
    // thread could have a reference to this file control block, so there's no
    // possibility that we can dead lock by acquiring the file's mutex for
    // exclusive access here.  We go to the effort of acquiring the file mutex
    // to satisfy the assertions in FatxSetAllocationSize.
    //

    if (FileFcb->ReferenceCount >= 2) {
        return STATUS_SHARING_VIOLATION;
    }

    FatxAcquireFileMutexExclusive(FileFcb);

    //
    // Save off the fields from the file control block that we're going to
    // change so that we can back out any changes if we fail below.
    //

    OriginalFileAttributes = FileFcb->FileAttributes;
    OriginalFileSize = FileFcb->FileSize;
    OriginalLastWriteTime = FileFcb->LastWriteTime;
    OriginalCreationTime = FileFcb->CreationTime;
    OriginalLastAccessTime = FileFcb->LastAccessTime;

    //
    // Fill in the file control block with the values for an overwritten file
    // and flag that the directory entry needs to be updated.
    //

    FileFcb->FileAttributes = FileAttributes;
    FileFcb->FileSize = 0;

    FileFcb->Flags &= ~FAT_FCB_DISABLE_LAST_WRITE_TIME;
    FileFcb->Flags |= FAT_FCB_UPDATE_DIRECTORY_ENTRY;

    KeQuerySystemTime(&FileFcb->LastWriteTime);
    FatxTimeToFatTimestamp(&FileFcb->LastWriteTime, &FileFcb->CreationTime);
    FileFcb->LastAccessTime = FileFcb->CreationTime;

    //
    // Set the allocation size for the file.  The directory entry may be updated
    // inside this call.
    //

    status = FatxSetAllocationSize(VolumeExtension, Irp, FileFcb, AllocationSize,
        TRUE, FALSE);

    //
    // If changing the allocation size for the file didn't cause the directory
    // entry to be updated, then do it here.
    //

    if (NT_SUCCESS(status) &&
        FatxIsFlagSet(FileFcb->Flags, FAT_FCB_UPDATE_DIRECTORY_ENTRY)) {
        status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, FileFcb);
    }

    //
    // If we failed for any reason, then restore the fields of the file control
    // block so that we still mirror the on-disk structures.
    //

    if (!NT_SUCCESS(status)) {
        FileFcb->FileAttributes = OriginalFileAttributes;
        FileFcb->FileSize = OriginalFileSize;
        FileFcb->LastWriteTime = OriginalLastWriteTime;
        FileFcb->CreationTime = OriginalCreationTime;
        FileFcb->LastAccessTime = OriginalLastAccessTime;
    }

    FatxReleaseFileMutex(FileFcb);

    return status;
}

NTSTATUS
FatxFsdCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_CREATE requests.

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
    ACCESS_MASK DesiredAccess;
    UCHAR FileAttributes;
    USHORT ShareAccess;
    ULONG CreateOptions;
    ULONG CreateDisposition;
    PFILE_OBJECT FileObject;
    OBJECT_STRING RemainingName;
    PFILE_OBJECT RelatedFileObject;
    PFAT_FCB CurrentFcb;
    BOOLEAN TrailingBackslash;
    BOOLEAN CreateFcbCalled;
    OBJECT_STRING ElementName;
    DIRENT DirectoryEntry;
    ULONG DirectoryByteOffset;
    ULONG EmptyDirectoryByteOffset;
    PFAT_FCB FoundOrNewFcb;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    DesiredAccess = IrpSp->Parameters.Create.DesiredAccess;
    FileAttributes =
        (UCHAR)FatxFilterFileAttributes(IrpSp->Parameters.Create.FileAttributes);
    ShareAccess = IrpSp->Parameters.Create.ShareAccess;
    CreateOptions = IrpSp->Parameters.Create.Options;
    CreateDisposition = (CreateOptions >> 24) & 0xFF;
    FileObject = IrpSp->FileObject;
    RemainingName = *IrpSp->Parameters.Create.RemainingName;
    RelatedFileObject = FileObject->RelatedFileObject;

    //
    // Always grab the exclusive lock because we could end up writing to the
    // disk and we also use this lock to guard access to the file control block
    // and file control block list.
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
    // Don't allow a file to be opened based on its file ID.
    //

    if (FatxIsFlagSet(CreateOptions, FILE_OPEN_BY_FILE_ID)) {
        status = STATUS_NOT_IMPLEMENTED;
        goto CleanupAndExit;
    }

    //
    // Verify that the initial file allocation size is limited to 32-bits.
    //

    if (Irp->Overlay.AllocationSize.HighPart != 0) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    //
    // Determine whether we parse the file name from the root of the volume or
    // from a subdirectory by looking at RelatedFileObject.
    //

    if (RelatedFileObject != NULL) {

        //
        // Grab the file control block out of the related file object.
        //

        CurrentFcb = (PFAT_FCB)RelatedFileObject->FsContext;

        //
        // Verify that the related file object is really a directory object.
        // Note that the file control block could be NULL if the file object was
        // opened as a result of a direct device open in the I/O manager.
        //

        if ((CurrentFcb == NULL) ||
            FatxIsFlagClear(CurrentFcb->Flags, FAT_FCB_DIRECTORY)) {
            status = STATUS_INVALID_PARAMETER;
            goto CleanupAndExit;
        }

        //
        // If the directory is marked for deletion, then the directory won't
        // contain any files and it won't allow any files to be created, so bail
        // out now.  This also catches code that uses a directory file object
        // after its handle has been closed and the directory has been deleted,
        // such as for symbolic link objects.
        //

        if (FatxIsFlagSet(CurrentFcb->Flags, FAT_FCB_DELETE_ON_CLOSE)) {
            status = STATUS_DELETE_PENDING;
            goto CleanupAndExit;
        }

        //
        // Check if we're supposed to open the related directory.
        //

        if (RemainingName.Length == 0) {

            if (FatxIsFlagSet(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY)) {
                status = FatxOpenTargetDirectory(VolumeExtension, Irp,
                    CurrentFcb->ParentFcb, FILE_EXISTS, FALSE);
                goto CleanupAndExit;
            }

            CurrentFcb->ReferenceCount++;
            status = STATUS_SUCCESS;
            goto OpenStartDirectoryFcb;
        }

        //
        // Verify that this is not an absolute path.
        //

        if (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
            status = STATUS_OBJECT_NAME_INVALID;
            goto CleanupAndExit;
        }

        //
        // Verify that the path doesn't exceed the length restictions.
        //

        if ((CurrentFcb->PathNameLength + sizeof(OBJ_NAME_PATH_SEPARATOR) +
            RemainingName.Length) > FAT_PATH_NAME_LIMIT * sizeof(OCHAR)) {
            status = STATUS_OBJECT_NAME_INVALID;
            goto CleanupAndExit;
        }

    } else {

        //
        // Check if we're supposed to open the physical volume.
        //

        if (RemainingName.Length == 0) {

            CurrentFcb = (PFAT_FCB)&VolumeExtension->VolumeFcb;

            //
            // Verify that the create disposition allows us to open an existing
            // file.
            //

            if ((CreateDisposition != FILE_OPEN) &&
                (CreateDisposition != FILE_OPEN_IF)) {
                status = STATUS_ACCESS_DENIED;
                goto CleanupAndExit;
            }

            //
            // The caller shouldn't be expecting to see a directory file.
            //

            if (FatxIsFlagSet(CreateOptions, FILE_DIRECTORY_FILE)) {
                status = STATUS_NOT_A_DIRECTORY;
                goto CleanupAndExit;
            }

            //
            // The physical volume cannot be deleted.
            //

            if (FatxIsFlagSet(CreateOptions, FILE_DELETE_ON_CLOSE)) {
                status = STATUS_CANNOT_DELETE;
                goto CleanupAndExit;
            }

            //
            // The physical volume cannot be renamed.
            //

            if (FatxIsFlagSet(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY)) {
                status = STATUS_INVALID_PARAMETER;
                goto CleanupAndExit;
            }

            //
            // Physical volume access is always non-cached.  Mark the file
            // object as non-cached so that the I/O manager enforces alignment
            // requirements.
            //

            FileObject->Flags |= FO_NO_INTERMEDIATE_BUFFERING;

            //
            // If this is the first open handle to the file, then initialize the
            // sharing mode.  Otherwise, verify that the requested sharing mode
            // is compatible with the current sharing mode.
            //

            if (CurrentFcb->ShareAccess.OpenCount == 0) {
                IoSetShareAccess(DesiredAccess, ShareAccess, FileObject,
                    &CurrentFcb->ShareAccess);
                status = STATUS_SUCCESS;
            } else {
                status = IoCheckShareAccess(DesiredAccess, ShareAccess,
                    FileObject, &CurrentFcb->ShareAccess, TRUE);
            }

            //
            // Fill in the file object with the file control block.
            //

            if (NT_SUCCESS(status)) {

                CurrentFcb->ReferenceCount++;

                FileObject->FsContext = CurrentFcb;
                FileObject->FsContext2 = NULL;

                VolumeExtension->FileObjectCount++;

                //
                // Indicate to the caller that we opened the file as opposed to creating
                // or overwriting the file.
                //

                Irp->IoStatus.Information = FILE_OPENED;

                status = STATUS_SUCCESS;
            }

            goto CleanupAndExit;
        }

        //
        // Start searching relative to the root directory.
        //

        CurrentFcb = VolumeExtension->RootDirectoryFcb;

        //
        // Verify that this is an absolute path.
        //

        if (RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR) {
            status = STATUS_OBJECT_NAME_INVALID;
            goto CleanupAndExit;
        }

        //
        // Check if we're supposed to open the root directory.
        //

        if (RemainingName.Length == sizeof(OCHAR)) {

            //
            // The root directory cannot be renamed.
            //

            if (FatxIsFlagSet(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY)) {
                status = STATUS_INVALID_PARAMETER;
                goto CleanupAndExit;
            }

            CurrentFcb->ReferenceCount++;
            status = STATUS_SUCCESS;
            goto OpenStartDirectoryFcb;
        }

        //
        // Verify that the path doesn't exceed the length restictions.
        //

        if (RemainingName.Length > FAT_PATH_NAME_LIMIT * sizeof(OCHAR)) {
            status = STATUS_OBJECT_NAME_INVALID;
            goto CleanupAndExit;
        }
    }

    //
    // Check if the file name ends in a backslash.  If so, strip it off and set
    // a flag so that we can later verify that the target file is a directory.
    //
    // We've already checked for an empty file name or a file name that consists
    // of a single backslash above, so we know that before and after this check
    // that the remaining name will still have some characters in it.
    //

    ASSERT(RemainingName.Length > 0);

    if (RemainingName.Buffer[(RemainingName.Length / sizeof(OCHAR)) - 1] ==
        OBJ_NAME_PATH_SEPARATOR) {
        RemainingName.Length -= sizeof(OCHAR);
        TrailingBackslash = TRUE;
    } else {
        TrailingBackslash = FALSE;
    }

    ASSERT(RemainingName.Length > 0);

    //
    // Process the file name.  At this point, we're only walking the open file
    // control block list.
    //

    for (;;) {

        //
        // Pull off the next element of the file name.
        //

        ObDissectName(RemainingName, &ElementName, &RemainingName);

        //
        // Verify that there aren't multiple backslashes in the name.
        //

        if ((RemainingName.Length != 0) && (RemainingName.Buffer[0] ==
            OBJ_NAME_PATH_SEPARATOR)) {
            status = STATUS_OBJECT_NAME_INVALID;
            break;
        }

        //
        // Check if a file control block already exists for this file name.
        //

        if (!FatxFindOpenChildFcb(CurrentFcb, &ElementName, &FoundOrNewFcb)) {
            break;
        }

        CurrentFcb = FoundOrNewFcb;

        //
        // If we have consumed the entire name, then the file is already open.
        // Bump up the reference count and skip past the on-disk search loop.
        //

        if (RemainingName.Length == 0) {

            if (FatxIsFlagSet(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY)) {
                status = FatxOpenTargetDirectory(VolumeExtension, Irp,
                    CurrentFcb->ParentFcb, FILE_EXISTS, FALSE);
                goto CleanupAndExit;
            }

            CurrentFcb->ReferenceCount++;
            status = STATUS_SUCCESS;
            goto OpenCurrentFcb;
        }
    }

    //
    // Continue processing the file name.  At this point, we're searching
    // directory streams for the requested file.
    //

    CreateFcbCalled = FALSE;

    do {

        //
        // On the first iteration of the loop, we've already dissected the name
        // we're looking for so don't dissect another piece of the name.
        //

        if (CreateFcbCalled) {

            //
            // Pull off the next element of the file name.
            //

            ObDissectName(RemainingName, &ElementName, &RemainingName);

            //
            // Verify that there aren't multiple backslashes in the name.
            //

            if ((RemainingName.Length != 0) && (RemainingName.Buffer[0] ==
                OBJ_NAME_PATH_SEPARATOR)) {
                status = STATUS_OBJECT_NAME_INVALID;
                break;
            }
        }

        //
        // Check if the name is a legal FAT file name.
        //

        if (!FatxIsValidFatFileName(&ElementName)) {
            status = STATUS_OBJECT_NAME_INVALID;
            break;
        }

        //
        // Lookup the element in the directory.
        //

        status = FatxLookupElementNameInDirectory(VolumeExtension, Irp,
            CurrentFcb, &ElementName, &DirectoryEntry, &DirectoryByteOffset,
            &EmptyDirectoryByteOffset);

        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {

            //
            // If the element wasn't found and we still have more elements to
            // process, then the path was not found.
            //

            if (RemainingName.Length != 0) {
                status = STATUS_OBJECT_PATH_NOT_FOUND;
                break;
            }

            //
            // If the parent directory is marked for deletion, don't allow any
            // files to be created.
            //

            if (FatxIsFlagSet(CurrentFcb->Flags, FAT_FCB_DELETE_ON_CLOSE)) {
                status = STATUS_DELETE_PENDING;
                break;
            }

            //
            // If we're inside a move file operation, then call the helper to
            // open the target directory and indicate that the file doesn't
            // already exist.
            //

            if (FatxIsFlagSet(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY)) {

                status = FatxOpenTargetDirectory(VolumeExtension, Irp,
                    CurrentFcb, FILE_DOES_NOT_EXIST, CreateFcbCalled);

                if (NT_SUCCESS(status)) {
                    goto CleanupAndExit;
                }

                break;
            }

            //
            // If the create disposition doesn't allow us to create files, then
            // we're done.
            //

            if ((CreateDisposition == FILE_OPEN) ||
                (CreateDisposition == FILE_OVERWRITE)) {
                break;
            }

            //
            // Don't allow a trailing backslash to be mixed with a request to
            // create a non-directory file.
            //

            if (TrailingBackslash &&
                FatxIsFlagSet(CreateOptions, FILE_NON_DIRECTORY_FILE)) {
                status = STATUS_OBJECT_NAME_INVALID;
                break;
            }

            //
            // Ensure that the directory file attribute is set or clear
            // depending on the creation options.
            //

            if (FatxIsFlagSet(CreateOptions, FILE_DIRECTORY_FILE)) {
                FileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            } else {
                FileAttributes &= ~FILE_ATTRIBUTE_DIRECTORY;
            }

            //
            // Verify that the desired access is compatible with the file's
            // desired attributes.
            //

            status = FatxCheckDesiredAccess(DesiredAccess, CreateOptions,
                FileAttributes, TRUE);

            if (!NT_SUCCESS(status)) {
                break;
            }

            //
            // Create the new file.
            //

            status = FatxCreateNewFile(VolumeExtension, Irp, CurrentFcb,
                &ElementName, EmptyDirectoryByteOffset, FileAttributes,
                Irp->Overlay.AllocationSize.LowPart, &DirectoryEntry,
                &FoundOrNewFcb);

            if (!NT_SUCCESS(status)) {
                break;
            }

            //
            // If this is the second or greater iteration of this loop, then we want
            // want to release the reference to the parent directory from
            // FatxCreateNewFile.  The parent directory already has a reference count
            // of one from when we created that file control block.
            //

            if (CreateFcbCalled) {
                ASSERT(CurrentFcb->ReferenceCount >= 2);
                CurrentFcb->ReferenceCount--;
            }

            //
            // Initializing the sharing mode.
            //

            IoSetShareAccess(DesiredAccess, ShareAccess, FileObject,
                &FoundOrNewFcb->ShareAccess);

            //
            // Fill in the file object with the file control block that we
            // created.
            //

            FileObject->FsContext = FoundOrNewFcb;
            FileObject->FsContext2 = NULL;

            VolumeExtension->FileObjectCount++;

            //
            // Apply the delete on close flag if necessary.
            //

            if (FatxIsFlagSet(CreateOptions, FILE_DELETE_ON_CLOSE) &&
                FatxIsFlagClear(FoundOrNewFcb->Flags, FAT_FCB_DIRECTORY)) {
                FoundOrNewFcb->Flags |= FAT_FCB_DELETE_ON_CLOSE;
            }

            //
            // Indicate to the caller that we created a new file as opposed to
            // opening an existing file.
            //

            Irp->IoStatus.Information = FILE_CREATED;

            status = STATUS_SUCCESS;
            goto CleanupAndExit;

        } else if (!NT_SUCCESS(status)) {
            break;
        }

        //
        // If there's still more of a path name to process, then verify that the
        // file we found is a directory.
        //

        if ((RemainingName.Length != 0) &&
            FatxIsFlagClear(DirectoryEntry.FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
            status = STATUS_OBJECT_PATH_NOT_FOUND;
            break;
        }

        //
        // If there's no more path name to process and we're opening the file
        // for a rename operation, then call the helper to open the target
        // directory and indicate that the file already exists.
        //

        if ((RemainingName.Length == 0) &&
            FatxIsFlagSet(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY)) {

            status = FatxOpenTargetDirectory(VolumeExtension, Irp, CurrentFcb,
                FILE_EXISTS, CreateFcbCalled);

            if (NT_SUCCESS(status)) {
                goto CleanupAndExit;
            }

            break;
        }

        //
        // The file isn't already open.  Construct a new file control block
        // for this file.
        //

        status = FatxCreateFcb(CurrentFcb, DirectoryEntry.FirstCluster,
            &DirectoryEntry, DirectoryByteOffset, &FoundOrNewFcb);

        if (!NT_SUCCESS(status)) {
            break;
        }

        //
        // If this is the second or greater iteration of this loop, then we want
        // to release the reference to the parent directory from FatxCreateFcb.
        // The parent directory already has a reference count of one from when
        // we created that file control block.
        //

        if (CreateFcbCalled) {
            ASSERT(CurrentFcb->ReferenceCount >= 2);
            CurrentFcb->ReferenceCount--;
        }

        CreateFcbCalled = TRUE;
        CurrentFcb = FoundOrNewFcb;

    } while (RemainingName.Length != 0);

    //
    // If we failed to open the file, then before bailing out, we may need to
    // dereference the current file control block.  If we haven't created any
    // file control blocks, then there's no file control blocks to clean up.
    //

    if (!NT_SUCCESS(status)) {

        if (CreateFcbCalled) {
            FatxDereferenceFcb(CurrentFcb);
        }

        goto CleanupAndExit;
    }

    //
    // If the caller is expecting to open only a file or directory file, then
    // verify that the file type matches.
    //

OpenCurrentFcb:
    if (FatxIsFlagSet(CurrentFcb->Flags, FAT_FCB_DIRECTORY)) {

OpenStartDirectoryFcb:
        if ((CreateDisposition != FILE_OPEN) &&
            (CreateDisposition != FILE_OPEN_IF)) {
            status = STATUS_OBJECT_NAME_COLLISION;
        } else if (FatxIsFlagSet(CreateOptions, FILE_NON_DIRECTORY_FILE)) {
            status = STATUS_FILE_IS_A_DIRECTORY;
        }

    } else {

        if (CreateDisposition == FILE_CREATE) {
            status = STATUS_OBJECT_NAME_COLLISION;
        } else if (TrailingBackslash ||
            FatxIsFlagSet(CreateOptions, FILE_DIRECTORY_FILE)) {
            status = STATUS_NOT_A_DIRECTORY;
        }
    }

    //
    // By the time we get here, we should have already handled the flag that
    // indicates a move file operation.
    //

    ASSERT(FatxIsFlagClear(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY));

    //
    // Verify that the desired access is compatible with the file's attributes.
    //

    if (NT_SUCCESS(status)) {
        status = FatxCheckDesiredAccess(DesiredAccess, CreateOptions,
            CurrentFcb->FileAttributes, FALSE);
    }

    //
    // If this is the first open handle to the file, then initialize the sharing
    // mode.  Otherwise, verify that the requested sharing mode is compatible
    // with the current sharing mode.
    //

    if (NT_SUCCESS(status)) {

        if (CurrentFcb->ShareAccess.OpenCount == 0) {
            IoSetShareAccess(DesiredAccess, ShareAccess, FileObject,
                &CurrentFcb->ShareAccess);
        } else {
            status = IoCheckShareAccess(DesiredAccess, ShareAccess,
                FileObject, &CurrentFcb->ShareAccess, TRUE);
        }
    }

    //
    // Check if we should overwrite or open the existing file.
    //

    if (NT_SUCCESS(status)) {

        if ((CreateDisposition == FILE_SUPERSEDE) ||
            (CreateDisposition == FILE_OVERWRITE) ||
            (CreateDisposition == FILE_OVERWRITE_IF)) {

            //
            // The code above has already verified that CreateDisposition is
            // either FILE_OPEN or FILE_OPEN_IF for directories.
            //

            ASSERT(FatxIsFlagClear(CurrentFcb->Flags, FAT_FCB_DIRECTORY));

            status = FatxOverwriteExistingFile(VolumeExtension, Irp, CurrentFcb,
                FileAttributes, Irp->Overlay.AllocationSize.LowPart);

            if (NT_SUCCESS(status)) {

                //
                // Indicate to the caller that we overwrote an existing file as
                // opposed to creating the file.
                //

                Irp->IoStatus.Information = FILE_OVERWRITTEN;

            } else {

                //
                // Cleanup the sharing mode associated with the file object;
                // we're going to be failing this creation.
                //

                IoRemoveShareAccess(FileObject, &CurrentFcb->ShareAccess);
            }

        } else {

            //
            // Indicate to the caller that we opened the file as opposed to
            // creating or overwriting the file.
            //

            Irp->IoStatus.Information = FILE_OPENED;
        }
    }

    //
    // Fill in the file object with the file control block that we created.
    //

    if (NT_SUCCESS(status)) {

        FileObject->FsContext = CurrentFcb;
        FileObject->FsContext2 = NULL;

        VolumeExtension->FileObjectCount++;

        //
        // Apply the delete on close flag if necessary.
        //

        if (FatxIsFlagSet(CreateOptions, FILE_DELETE_ON_CLOSE) &&
            FatxIsFlagClear(CurrentFcb->Flags, FAT_FCB_DIRECTORY)) {
            CurrentFcb->Flags |= FAT_FCB_DELETE_ON_CLOSE;
        }

        status = STATUS_SUCCESS;
        goto CleanupAndExit;
    }

    //
    // If we reach this point, then we're going to be failing the call so
    // cleanup any file control block we're still holding on to.
    //

    FatxDereferenceFcb(CurrentFcb);

CleanupAndExit:
    FatxReleaseVolumeMutex(VolumeExtension);

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
