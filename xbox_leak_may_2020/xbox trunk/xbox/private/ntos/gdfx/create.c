/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    create.c

Abstract:

    This module implements routines related to handling IRP_MJ_CREATE.

--*/

#include "gdfx.h"

NTSTATUS
GdfxLookupElementNameInDirectory(
    IN PGDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PGDF_FCB DirectoryFcb,
    IN POBJECT_STRING ElementName,
    OUT PGDF_DIRECTORY_ENTRY ReturnedDirectoryEntry
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

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID CacheBuffer;
    ULONG CacheBufferByteOffset;
    ULONG DirectoryByteOffset;
    PGDF_DIRECTORY_ENTRY DirectoryEntry;
    OBJECT_STRING DirectoryEntryFileName;
    LONG CompareResult;
    ULONG NewDirectoryByteOffset;

    ASSERT(GdfxIsFlagSet(DirectoryFcb->Flags, GDF_FCB_DIRECTORY));

    //
    // If this is an empty directory, then return now that the name isn't found.
    //

    if (DirectoryFcb->FileSize == 0) {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    //
    // Process the directory stream.
    //

    CacheBuffer = NULL;
    CacheBufferByteOffset = 0;
    DirectoryByteOffset = 0;

    for (;;) {

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
        // Compute the cache buffer relative pointer to the directory entry.
        //

        DirectoryEntry = (PGDF_DIRECTORY_ENTRY)((PUCHAR)CacheBuffer +
            DirectoryByteOffset);

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

        CompareResult = RtlCompareString(ElementName, &DirectoryEntryFileName,
            TRUE);

        //
        // The names match, so copy the first part of the directory entry back
        // into the caller's buffers and return.
        //

        if (CompareResult == 0) {

            //
            // Verify that the file's extents are in the bounds of the CD-ROM
            // partition.
            //

            if ((DirectoryEntry->FirstSector < VolumeExtension->PartitionSectorCount) &&
                ((VolumeExtension->PartitionSectorCount - DirectoryEntry->FirstSector) >=
                    (GDF_CD_SECTOR_ALIGN_UP(DirectoryEntry->FileSize) >> GDF_CD_SECTOR_SHIFT))) {

                *ReturnedDirectoryEntry = *DirectoryEntry;
                status = STATUS_SUCCESS;
                goto CleanupAndExit;

            } else {
                status = STATUS_DISK_CORRUPT_ERROR;
                goto CleanupAndExit;
            }
        }

        //
        // Follow the left or right link from the directory entry.  The index is
        // shifted by two bits to obtain a four byte aligned byte offset.
        //

        if (CompareResult < 0) {
            NewDirectoryByteOffset = ((ULONG)DirectoryEntry->LeftEntryIndex << 2);
        } else {
            NewDirectoryByteOffset = ((ULONG)DirectoryEntry->RightEntryIndex << 2);
        }

        //
        // If the new directory byte offset is zero, then we're at the bottom of
        // the tree and should bail out.
        //

        if (NewDirectoryByteOffset == 0) {
            status = STATUS_OBJECT_NAME_NOT_FOUND;
            goto CleanupAndExit;
        }

        //
        // To prevent loops from occurring in the tree, don't allow the byte
        // offset to move back in the directory stream.
        //

        if (NewDirectoryByteOffset < (CacheBufferByteOffset + DirectoryByteOffset)) {
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        //
        // The disk is corrupt if the new directory byte offset is beyond the
        // end of the directory stream.  If the new directory byte offset is
        // close to the end of the directory stream, we may end up accessing
        // past the end of file, but the above checks ensure that we won't
        // access past the end of a sector.
        //
        // This check protects us from accessing past the end of the disk
        // because when we create the file control block, we validate that the
        // ending sector is not beyond the end of the disk.
        //

        if (NewDirectoryByteOffset >= DirectoryFcb->FileSize) {
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        DirectoryByteOffset = NewDirectoryByteOffset;
    }

CleanupAndExit:
    if (CacheBuffer != NULL) {
        FscUnmapBuffer(CacheBuffer);
    }

    return status;
}

NTSTATUS
GdfxFsdCreate(
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
    PGDF_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    ACCESS_MASK DesiredAccess;
    ULONG CreateOptions;
    PFILE_OBJECT FileObject;
    OBJECT_STRING RemainingName;
    PFILE_OBJECT RelatedFileObject;
    ULONG CreateDisposition;
    PGDF_FCB CurrentFcb;
    SHARE_ACCESS ShareAccess;
    BOOLEAN TrailingBackslash;
    BOOLEAN CreateFcbCalled;
    OBJECT_STRING ElementName;
    PGDF_FCB FoundOrNewFcb;
    GDF_DIRECTORY_ENTRY DirectoryEntry;

    VolumeExtension = (PGDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    DesiredAccess = IrpSp->Parameters.Create.DesiredAccess;
    CreateOptions = IrpSp->Parameters.Create.Options;
    FileObject = IrpSp->FileObject;
    RemainingName = *IrpSp->Parameters.Create.RemainingName;
    RelatedFileObject = FileObject->RelatedFileObject;

    //
    // Synchronize access to the file control block lists by acquiring the
    // global mutex.
    //

    GdfxAcquireGlobalMutexExclusive();

    //
    // Check if the volume has been dismounted.
    //

    if (VolumeExtension->Dismounted) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto CleanupAndExit;
    }

    //
    // Don't allow the target directory to be opened.  This is only used by the
    // I/O manager when it wants to rename a file across a directory on the same
    // volume.
    //

    if (GdfxIsFlagSet(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY)) {
        status = STATUS_ACCESS_DENIED;
        goto CleanupAndExit;
    }

    //
    // Check the desired access mask to make sure that no write rights are being
    // requested.
    //

    if (GdfxIsFlagSet(DesiredAccess, FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA |
        FILE_WRITE_EA | FILE_ADD_FILE | FILE_ADD_SUBDIRECTORY |
        FILE_APPEND_DATA | FILE_DELETE_CHILD | DELETE | WRITE_DAC)) {
        status = STATUS_ACCESS_DENIED;
        goto CleanupAndExit;
    }

    //
    // Don't allow a file to be opened based on its file ID.
    //

    if (GdfxIsFlagSet(CreateOptions, FILE_OPEN_BY_FILE_ID)) {
        status = STATUS_NOT_IMPLEMENTED;
        goto CleanupAndExit;
    }

    //
    // Verify that the create disposition is for open access only.
    //

    CreateDisposition = (CreateOptions >> 24) & 0xFF;

    if ((CreateDisposition != FILE_OPEN) && (CreateDisposition != FILE_OPEN_IF)) {
        status = STATUS_ACCESS_DENIED;
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

        CurrentFcb = (PGDF_FCB)RelatedFileObject->FsContext;

        //
        // Verify that the related file object is really a directory object.
        // Note that the file control block could be NULL if the file object was
        // opened as a result of a direct device open in the I/O manager.
        //

        if ((CurrentFcb == NULL) ||
            GdfxIsFlagClear(CurrentFcb->Flags, GDF_FCB_DIRECTORY)) {
            status = STATUS_INVALID_PARAMETER;
            goto CleanupAndExit;
        }

        //
        // Check if we're supposed to open the related directory.
        //

        if (RemainingName.Length == 0) {
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

    } else {

        //
        // Check if we're supposed to open the physical volume.
        //

        if (RemainingName.Length == 0) {

            CurrentFcb = VolumeExtension->VolumeFcb;

            //
            // The caller shouldn't be expecting to see a directory file.
            //

            if (GdfxIsFlagSet(CreateOptions, FILE_DIRECTORY_FILE)) {
                status = STATUS_NOT_A_DIRECTORY;
                goto CleanupAndExit;
            }

            //
            // Physical volume access is always non-cached.  Mark the file
            // object as non-cached so that the I/O manager enforces alignment
            // requirements.
            //

            FileObject->Flags |= FO_NO_INTERMEDIATE_BUFFERING;

            CurrentFcb->ReferenceCount++;
            goto InitializeFileObject;
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
            CurrentFcb->ReferenceCount++;
            status = STATUS_SUCCESS;
            goto OpenStartDirectoryFcb;
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

        if (!GdfxFindOpenChildFcb(CurrentFcb, &ElementName, &FoundOrNewFcb)) {
            break;
        }

        CurrentFcb = FoundOrNewFcb;

        //
        // If we have consumed the entire name, then the file is already open.
        // Bump up the reference count and skip past the on-disk search loop.
        //

        if (RemainingName.Length == 0) {
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
        // Lookup the element in the directory.
        //

        status = GdfxLookupElementNameInDirectory(VolumeExtension, Irp,
            CurrentFcb, &ElementName, &DirectoryEntry);

        if (!NT_SUCCESS(status)) {

            //
            // If we failed to find the element and there's more of a path name
            // to process, then convert the error from "file not found" to
            // "path not found".
            //

            if ((status == STATUS_OBJECT_NAME_NOT_FOUND) &&
                (RemainingName.Length != 0)) {
                status = STATUS_OBJECT_PATH_NOT_FOUND;
            }

            break;
        }

        //
        // Create a file control block for the file we found.
        //

        status = GdfxCreateFcb(CurrentFcb, &ElementName, &DirectoryEntry,
            &FoundOrNewFcb);

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

        //
        // If there's still more of a path name to process, then the file that
        // we found had better be a directory.
        //

        if ((RemainingName.Length != 0) &&
            GdfxIsFlagClear(CurrentFcb->Flags, GDF_FCB_DIRECTORY)) {
            status = STATUS_OBJECT_PATH_NOT_FOUND;
            break;
        }

    } while (RemainingName.Length != 0);

    //
    // If we failed to open the file, then before bailing out, we may need to
    // dereference the current file control block.  If we haven't created any
    // file control blocks, then there's no file control blocks to clean up.
    //

    if (!NT_SUCCESS(status)) {

        if (CreateFcbCalled) {
            GdfxDereferenceFcb(CurrentFcb);
        }

        goto CleanupAndExit;
    }

    //
    // If the caller is expecting to open only a file or directory file, then
    // verify that the file type matches.
    //

OpenCurrentFcb:
    if (GdfxIsFlagSet(CurrentFcb->Flags, GDF_FCB_DIRECTORY)) {

OpenStartDirectoryFcb:
        if (GdfxIsFlagSet(CreateOptions, FILE_NON_DIRECTORY_FILE)) {
            status = STATUS_FILE_IS_A_DIRECTORY;
        }

    } else {

        if (TrailingBackslash ||
            GdfxIsFlagSet(CreateOptions, FILE_DIRECTORY_FILE)) {
            status = STATUS_NOT_A_DIRECTORY;
        }
    }

    //
    // If the resulting file looks acceptable, then fill out the file object
    // and return success.
    //

    if (NT_SUCCESS(status)) {

InitializeFileObject:
        //
        // We don't care about sharing modes for this file system.
        //

        IoSetShareAccess(DesiredAccess, 0, FileObject, &ShareAccess);

        //
        // Fill in the file object with the file control block that we
        // created.
        //

        FileObject->FsContext = CurrentFcb;
        FileObject->FsContext2 = NULL;

        VolumeExtension->FileObjectCount++;

        //
        // Indicate to the caller that we opened the file as opposed to
        // creating or overwriting the file.
        //

        Irp->IoStatus.Information = FILE_OPENED;

        status = STATUS_SUCCESS;
        goto CleanupAndExit;
    }

    //
    // If we reach this point, then we're going to be failing the call so
    // cleanup any file control block we're still holding on to.
    //

    GdfxDereferenceFcb(CurrentFcb);

CleanupAndExit:
    GdfxReleaseGlobalMutex();

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
