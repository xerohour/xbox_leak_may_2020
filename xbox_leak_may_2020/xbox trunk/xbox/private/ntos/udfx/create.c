/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    create.c

Abstract:

    This module implements routines related to handling IRP_MJ_CREATE.

--*/

#include "udfx.h"

//
// Structure that wraps up all of the variables related to walk through a
// directory stream.
//

typedef struct _UDF_DIRECTORY_MAPPING {
    PUDF_VOLUME_EXTENSION VolumeExtension;
    PIRP Irp;
    PUDF_FCB DirectoryFcb;
    ULONG LogicalSectorNumber;
    ULONG FileBytesRemaining;
    ULONG CacheBytesRemaining;
    PVOID CacheBuffer;
    PUCHAR CurrentBuffer;
} UDF_DIRECTORY_MAPPING, *PUDF_DIRECTORY_MAPPING;

NTSTATUS
UdfxMapNextDirectorySector(
    IN OUT PUDF_DIRECTORY_MAPPING DirectoryMapping
    )
/*++

Routine Description:

    This routine maps the next directory sector into the cache using the state
    in the supplied mapping structure.

Arguments:

    DirectoryMapping - Specifies a structure that describes the current sector
        mapping and how to map the next sector.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PUDF_VOLUME_EXTENSION VolumeExtension;
    PUCHAR LocalCurrentBuffer;
    ULONG LocalCacheBytesRemaining;

    //
    // If another sector has already been mapped, then unmap it now.
    //

    if (DirectoryMapping->CacheBuffer != NULL) {
        FscUnmapBuffer(DirectoryMapping->CacheBuffer);
        DirectoryMapping->CacheBuffer = NULL;
    }

    //
    // If there aren't any more bytes remaining in the directory stream, then
    // bail out now.
    //

    if (DirectoryMapping->FileBytesRemaining == 0) {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    //
    // Map the directory sector into the cache.
    //

    VolumeExtension = DirectoryMapping->VolumeExtension;

    status = UdfxMapLogicalSector(VolumeExtension, DirectoryMapping->Irp,
        DirectoryMapping->LogicalSectorNumber, &DirectoryMapping->CacheBuffer);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    DirectoryMapping->LogicalSectorNumber++;

    LocalCurrentBuffer = (PUCHAR)DirectoryMapping->CacheBuffer;

    //
    // If the data is part of the file descriptor, then adjust the current
    // buffer to point to the real start of the data.  The number of remaining
    // bytes will be adjusted below.
    //

    if (UdfxIsFlagSet(DirectoryMapping->DirectoryFcb->Flags, UDF_FCB_EMBEDDED_DATA)) {
        LocalCurrentBuffer += DirectoryMapping->DirectoryFcb->EmbeddedDataOffset;
        ASSERT(DirectoryMapping->FileBytesRemaining < UDF_CD_SECTOR_SIZE);
    }

    //
    // Compute the number of bytes remaining in the file and in this cache
    // buffer taking into account whether or not this is the last sector of the
    // directory stream.
    //

    if (DirectoryMapping->FileBytesRemaining > UDF_CD_SECTOR_SIZE) {
        LocalCacheBytesRemaining = UDF_CD_SECTOR_SIZE;
        DirectoryMapping->FileBytesRemaining -= UDF_CD_SECTOR_SIZE;
    } else {
        LocalCacheBytesRemaining = DirectoryMapping->FileBytesRemaining;
        DirectoryMapping->FileBytesRemaining = 0;
    }

    //
    // If we're to skip over bytes from a descriptor that crosses a sector
    // boundary, then do that adjustment here.
    //

    LocalCurrentBuffer -= DirectoryMapping->CacheBytesRemaining;
    LocalCacheBytesRemaining += DirectoryMapping->CacheBytesRemaining;

    //
    // Return the results back to the caller.
    //

    DirectoryMapping->CurrentBuffer = LocalCurrentBuffer;
    DirectoryMapping->CacheBytesRemaining = LocalCacheBytesRemaining;

    return STATUS_SUCCESS;
}

VOID
UdfxCopyPartialDirectoryBuffer(
    IN OUT PUDF_DIRECTORY_MAPPING DirectoryMapping,
    OUT PVOID Destination,
    IN PVOID Source,
    IN SIZE_T Length
    )
/*++

Routine Description:

    This routine is a wrapper for RtlCopyMemory that asserts that the arguments
    are valid cache pointers in a debug build and reduces the code bloat in a
    retail build caused by RtlCopyMemory inlining.

Arguments:

    DirectoryMapping - Specifies a structure that describes the current sector
        mapping and how to map the next sector.

    Destination - Specifies the destination of the move.

    Source - Specifies the memory to be copied.

    Length - Specifies the number of bytes to be copied.

Return Value:

    Status of operation.

--*/
{
    ASSERT((PUCHAR)Source >= (PUCHAR)DirectoryMapping->CacheBuffer);
    ASSERT((PUCHAR)Source < (PUCHAR)DirectoryMapping->CacheBuffer +
        UDF_CD_SECTOR_SIZE);
    ASSERT((PUCHAR)Source + Length <= (PUCHAR)DirectoryMapping->CacheBuffer +
        UDF_CD_SECTOR_SIZE);
    ASSERT(Length <= UDF_CD_SECTOR_SIZE);

    RtlCopyMemory(Destination, Source, Length);
}

NTSTATUS
UdfxLookupElementNameInDirectory(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PUDF_FCB DirectoryFcb,
    IN POBJECT_STRING ElementName,
    OUT PLONGAD ElementIcb
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

    ElementIcb - Specifies the buffer to receive the extent of the found file.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG ExpectedFileID8BitLength;
    UDF_DIRECTORY_MAPPING DirectoryMapping;
    union {
        NSR_FID FileIdentifierDescriptor;
        UCHAR FileID[256];
    } Stack;
    ULONG PartialStackBufferBytes;
    PNSR_FID FileIdentifierDescriptor;
    ULONG FileIdentifierDescriptorLength;
    ULONG FileIDLength;
    ULONG FileIDStartingOffset;
    PUCHAR FileID;

    ASSERT(UdfxIsFlagSet(DirectoryFcb->Flags, UDF_FCB_DIRECTORY));

    //
    // Compute the expected lengths for a file identifier that will match the
    // supplied file name that we're searching for.  We only care about 8-bit
    // encodings.
    //

    ExpectedFileID8BitLength = sizeof(UCHAR) + ElementName->Length / sizeof(OCHAR);

    //
    // Initialize the directory mapping structure.
    //

    DirectoryMapping.VolumeExtension = VolumeExtension;
    DirectoryMapping.Irp = Irp;
    DirectoryMapping.DirectoryFcb = DirectoryFcb;
    DirectoryMapping.LogicalSectorNumber = DirectoryFcb->AllocationSectorStart;
    DirectoryMapping.FileBytesRemaining = DirectoryFcb->FileSize.LowPart;
    DirectoryMapping.CacheBytesRemaining = 0;
    DirectoryMapping.CacheBuffer = NULL;
    DirectoryMapping.CurrentBuffer = NULL;

    //
    // Process the directory stream.
    //

    for (;;) {

        //
        // If we're out of bytes in the current cache buffer, then map in the
        // next sector.
        //

        if ((LONG)DirectoryMapping.CacheBytesRemaining <= 0) {

            status = UdfxMapNextDirectorySector(&DirectoryMapping);

            if (!NT_SUCCESS(status)) {
                break;
            }
        }

        //
        // If the current cache buffer doesn't have enough bytes to contain a
        // full descriptor, then we'll have to buffer the descriptor on the
        // stack and carefully cross sector boundaries.
        //

        if (DirectoryMapping.CacheBytesRemaining < ISONsrFidConstantSize) {

            //
            // If the bytes from this cache buffer and what remains in the file
            // won't be enough to contain a full descriptor, then bail out now.
            //

            if (DirectoryMapping.CacheBytesRemaining +
                DirectoryMapping.FileBytesRemaining < ISONsrFidConstantSize) {
                status = STATUS_OBJECT_NAME_NOT_FOUND;
                break;
            }

            //
            // Copy the first part of the descriptor to the stack.
            //

            PartialStackBufferBytes = DirectoryMapping.CacheBytesRemaining;

            UdfxCopyPartialDirectoryBuffer(&DirectoryMapping,
                &Stack.FileIdentifierDescriptor, DirectoryMapping.CurrentBuffer,
                PartialStackBufferBytes);

            //
            // Map in the next sector.  Note that on return, the CurrentBuffer
            // CurrentBuffer and CacheBytesRemaining fields have been adjusted
            // adjusted to point back in memory as if the previous sector was
            // virtually mapped in order to make the below math easier.
            //

            status = UdfxMapNextDirectorySector(&DirectoryMapping);

            if (!NT_SUCCESS(status)) {
                break;
            }

            //
            // Copy the last part of the descriptor to the stack.
            //

            UdfxCopyPartialDirectoryBuffer(&DirectoryMapping,
                (PUCHAR)&Stack.FileIdentifierDescriptor + PartialStackBufferBytes,
                DirectoryMapping.CurrentBuffer + PartialStackBufferBytes,
                ISONsrFidConstantSize - PartialStackBufferBytes);

            FileIdentifierDescriptor = &Stack.FileIdentifierDescriptor;

        } else {

            //
            // There's enough bytes in the current buffer to contain at least
            // the header, so point directly into the cache buffer.
            //

            FileIdentifierDescriptor = (PNSR_FID)DirectoryMapping.CurrentBuffer;
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
            (FileIdentifierDescriptorLength > DirectoryMapping.FileBytesRemaining +
                (ULONG)DirectoryMapping.CacheBytesRemaining)) {
            UdfxDbgPrint(("UDFX: file identifier descriptor doesn't verify\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
            break;
        }

        FileIDLength = FileIdentifierDescriptor->FileIDLen;

        //
        // Only bother checking the file identifier descriptor if the file
        // identifier length matches the length of the element name we're
        // looking for.
        //

        if (FileIDLength == ExpectedFileID8BitLength) {

            //
            // Fill in the output buffer now since we'll shortly lose access to
            // the file identifier descriptor.
            //

            *ElementIcb = FileIdentifierDescriptor->Icb;

            //
            // Compute the byte offset of the file identifier.
            //

            FileIDStartingOffset = ISONsrFidConstantSize +
                FileIdentifierDescriptor->ImpUseLen;

            //
            // The below code can cause the current sector buffer to become
            // invalid, so we can no longer safely touch the
            // FileIdentifierDescriptor.
            //

            FileIdentifierDescriptor = NULL;

            //
            // If the current cache buffer doesn't have enough bytes to contain
            // the string, then we'll have to buffer the string on the stack and
            // carefully cross sector boundaries.
            //

            if (DirectoryMapping.CacheBytesRemaining < FileIDStartingOffset +
                FileIDLength) {

                //
                // Check if any of the string is inside the current cache
                // buffer.
                //

                if (DirectoryMapping.CacheBytesRemaining > FileIDStartingOffset) {

                    //
                    // Copy the first part of the string to the stack.
                    //

                    PartialStackBufferBytes =
                        DirectoryMapping.CacheBytesRemaining - FileIDStartingOffset;

                    UdfxCopyPartialDirectoryBuffer(&DirectoryMapping,
                        &Stack.FileID, DirectoryMapping.CurrentBuffer +
                        FileIDStartingOffset, PartialStackBufferBytes);

                } else {

                    PartialStackBufferBytes = 0;
                }

                //
                // Map in the next sector.  Note that on return, the
                // CurrentBuffer and CacheBytesRemaining fields have been
                // adjusted to point back in memory as if the previous sector
                // was virtually mapped in order to make the below math easier.
                //

                status = UdfxMapNextDirectorySector(&DirectoryMapping);

                if (!NT_SUCCESS(status)) {
                    break;
                }

                //
                // Copy the last part of the string to the stack.
                //

                UdfxCopyPartialDirectoryBuffer(&DirectoryMapping,
                    Stack.FileID + PartialStackBufferBytes,
                    DirectoryMapping.CurrentBuffer + FileIDStartingOffset +
                    PartialStackBufferBytes, FileIDLength -
                    PartialStackBufferBytes);

                FileID = Stack.FileID;

            } else {

                //
                // There's enough bytes in the current buffer to contain all of
                // the string, so point directly into the cache.
                //

                FileID = DirectoryMapping.CurrentBuffer + FileIDStartingOffset;
            }

            if (UdfxEqualOSTACS0StringAndObjectString(FileID, FileIDLength,
                ElementName)) {
                status = STATUS_SUCCESS;
                break;
            }
        }

        //
        // Advance to the next descriptor.
        //

        DirectoryMapping.CacheBytesRemaining -= FileIdentifierDescriptorLength;
        DirectoryMapping.CurrentBuffer += FileIdentifierDescriptorLength;
    }

    if (DirectoryMapping.CacheBuffer != NULL) {
        FscUnmapBuffer(DirectoryMapping.CacheBuffer);
    }

    return status;
}

NTSTATUS
UdfxFsdCreate(
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
    PUDF_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    ACCESS_MASK DesiredAccess;
    ULONG CreateOptions;
    PFILE_OBJECT FileObject;
    OBJECT_STRING RemainingName;
    PFILE_OBJECT RelatedFileObject;
    ULONG CreateDisposition;
    PUDF_FCB CurrentFcb;
    SHARE_ACCESS ShareAccess;
    BOOLEAN TrailingBackslash;
    BOOLEAN CreateFcbCalled;
    OBJECT_STRING ElementName;
    PUDF_FCB FoundOrNewFcb;
    LONGAD ElementIcb;

    VolumeExtension = (PUDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
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

    UdfxAcquireGlobalMutexExclusive();

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

    if (UdfxIsFlagSet(IrpSp->Flags, SL_OPEN_TARGET_DIRECTORY)) {
        status = STATUS_ACCESS_DENIED;
        goto CleanupAndExit;
    }

    //
    // Check the desired access mask to make sure that no write rights are being
    // requested.
    //

    if (UdfxIsFlagSet(DesiredAccess, FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA |
        FILE_WRITE_EA | FILE_ADD_FILE | FILE_ADD_SUBDIRECTORY |
        FILE_APPEND_DATA | FILE_DELETE_CHILD | DELETE | WRITE_DAC)) {
        status = STATUS_ACCESS_DENIED;
        goto CleanupAndExit;
    }

    //
    // Don't allow a file to be opened based on its file ID.
    //

    if (UdfxIsFlagSet(CreateOptions, FILE_OPEN_BY_FILE_ID)) {
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

        CurrentFcb = (PUDF_FCB)RelatedFileObject->FsContext;

        //
        // Verify that the related file object is really a directory object.
        // Note that the file control block could be NULL if the file object was
        // opened as a result of a direct device open in the I/O manager.
        //

        if ((CurrentFcb == NULL) ||
            UdfxIsFlagClear(CurrentFcb->Flags, UDF_FCB_DIRECTORY)) {
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

            if (UdfxIsFlagSet(CreateOptions, FILE_DIRECTORY_FILE)) {
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

        if (!UdfxFindOpenChildFcb(CurrentFcb, &ElementName, &FoundOrNewFcb)) {
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

        status = UdfxLookupElementNameInDirectory(VolumeExtension, Irp,
            CurrentFcb, &ElementName, &ElementIcb);

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

        status = UdfxCreateFcbFromFileEntry(VolumeExtension, Irp, &ElementIcb,
            CurrentFcb, &ElementName, &FoundOrNewFcb);

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
            UdfxIsFlagClear(CurrentFcb->Flags, UDF_FCB_DIRECTORY)) {
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
            UdfxDereferenceFcb(CurrentFcb);
        }

        goto CleanupAndExit;
    }

    //
    // If the caller is expecting to open only a file or directory file, then
    // verify that the file type matches.
    //

OpenCurrentFcb:
    if (UdfxIsFlagSet(CurrentFcb->Flags, UDF_FCB_DIRECTORY)) {

OpenStartDirectoryFcb:
        if (UdfxIsFlagSet(CreateOptions, FILE_NON_DIRECTORY_FILE)) {
            status = STATUS_FILE_IS_A_DIRECTORY;
        }

    } else {

        if (TrailingBackslash ||
            UdfxIsFlagSet(CreateOptions, FILE_DIRECTORY_FILE)) {
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

    UdfxDereferenceFcb(CurrentFcb);

CleanupAndExit:
    UdfxReleaseGlobalMutex();

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
