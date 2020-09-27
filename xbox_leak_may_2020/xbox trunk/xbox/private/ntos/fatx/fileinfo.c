/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fileinfo.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_QUERY_INFORMATION and IRP_MJ_SET_INFORMATION.

--*/

#include "fatx.h"

VOID
FatxQueryNetworkOpenInformation(
    IN PFILE_OBJECT FileObject,
    OUT PFILE_NETWORK_OPEN_INFORMATION NetworkOpenInformation
    )
/*++

Routine Description:

    This routine fills the information structure with attributes about the
    supplied file object.

Arguments:

    FileObject - Specifies the file object to obtain the information from.

    NetworkOpenInformation - Specifies the buffer to receive the file
        information.

Return Value:

    None.

--*/
{
    PFAT_FCB Fcb;

    Fcb = (PFAT_FCB)FileObject->FsContext;

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME));

    NetworkOpenInformation->CreationTime =
        FatxFatTimestampToTime(&Fcb->CreationTime);
    NetworkOpenInformation->LastAccessTime =
        FatxFatTimestampToTime(&Fcb->LastAccessTime);
    NetworkOpenInformation->LastWriteTime = FatxRoundToFatTime(&Fcb->LastWriteTime);
    NetworkOpenInformation->ChangeTime = NetworkOpenInformation->LastWriteTime;

    if (Fcb->FileAttributes == 0) {
        NetworkOpenInformation->FileAttributes = FILE_ATTRIBUTE_NORMAL;
    } else {
        NetworkOpenInformation->FileAttributes = Fcb->FileAttributes;
    }

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {

        NetworkOpenInformation->AllocationSize.QuadPart = 0;
        NetworkOpenInformation->EndOfFile.QuadPart = 0;

    } else {

        //
        // AllocationSize should be filled in with the number of clusters
        // actually allocated to the file, but we don't want to have to go parse
        // the entire FAT chain to obtain this piece of information.  We'll let
        // the caller assume that the file has been allocated the number of
        // bytes actually in the file.
        //

        NetworkOpenInformation->AllocationSize.QuadPart = (ULONGLONG)Fcb->FileSize;
        NetworkOpenInformation->EndOfFile.QuadPart = (ULONGLONG)Fcb->FileSize;
    }
}

NTSTATUS
FatxFsdQueryInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_QUERY_INFORMATION
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
    PFAT_FCB Fcb;
    ULONG BytesWritten;
    PFILE_INTERNAL_INFORMATION InternalInformation;
    PFILE_POSITION_INFORMATION PositionInformation;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    Fcb = (PFAT_FCB)FileObject->FsContext;

    FatxAcquireVolumeMutexShared(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto CleanupAndExit;
    }

    //
    // Check if the file object has already been cleaned up.  We don't allow a
    // a file object to be accessed after its handle has been closed.
    //

    if (FatxIsFlagSet(FileObject->Flags, FO_CLEANUP_COMPLETE)) {
        status = STATUS_FILE_CLOSED;
        goto CleanupAndExit;
    }

    //
    // For volume file control blocks, the only thing that can be queried is the
    // current file position.
    //

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_VOLUME) &&
        (IrpSp->Parameters.SetFile.FileInformationClass != FilePositionInformation)) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    //
    // Clear the output buffer.
    //

    RtlZeroMemory(Irp->UserBuffer, IrpSp->Parameters.QueryFile.Length);

    //
    // Dispatch the information class function.
    //

    status = STATUS_SUCCESS;

    switch (IrpSp->Parameters.QueryFile.FileInformationClass) {

        case FileInternalInformation:
            InternalInformation = (PFILE_INTERNAL_INFORMATION)Irp->UserBuffer;
            InternalInformation->IndexNumber.HighPart = PtrToUlong(VolumeExtension);
            InternalInformation->IndexNumber.LowPart = PtrToUlong(Fcb);
            BytesWritten = sizeof(FILE_INTERNAL_INFORMATION);
            break;

        case FilePositionInformation:
            PositionInformation = (PFILE_POSITION_INFORMATION)Irp->UserBuffer;
            PositionInformation->CurrentByteOffset = FileObject->CurrentByteOffset;
            BytesWritten = sizeof(FILE_POSITION_INFORMATION);
            break;

        case FileNetworkOpenInformation:
            FatxQueryNetworkOpenInformation(FileObject,
                (PFILE_NETWORK_OPEN_INFORMATION)Irp->UserBuffer);
            BytesWritten = sizeof(FILE_NETWORK_OPEN_INFORMATION);
            break;

        default:
            BytesWritten = 0;
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    //
    // Fill in the number of bytes that we wrote to the user's buffer.
    //

    Irp->IoStatus.Information = BytesWritten;

    //
    // Check that we didn't overflow the user's buffer.  The I/O manager does
    // the initial check to make sure there's enough space for the static
    // structure for a given information class, but we might overflow the buffer
    // when copying in the variable length file name.
    //

    ASSERT(Irp->IoStatus.Information <= IrpSp->Parameters.QueryFile.Length);

CleanupAndExit:
    FatxReleaseVolumeMutex(VolumeExtension);

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
FatxSetBasicInformation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFILE_OBJECT FileObject,
    IN PFILE_BASIC_INFORMATION BasicInformation
    )
/*++

Routine Description:

    This routine changes the basic information of the supplied file object.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    FileObject - Specifies the file object to apply the information to.

    BasicInformation - Specifies the basic information to apply to the file.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    BOOLEAN ChangesMade;
    PFAT_FCB Fcb;
    FAT_TIME_STAMP CreationTime;
    LARGE_INTEGER LastWriteTime;
    FAT_TIME_STAMP FatTimestamp;
    FAT_TIME_STAMP LastAccessTime;
    UCHAR FileAttributes;

    ChangesMade = FALSE;
    Fcb = (PFAT_FCB)FileObject->FsContext;

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // Validate all of the timestamps from the information structure.  If the
    // timestamp is -1, then that means that the user doesn't want the file
    // system automatically updating the timestamps, such as modifying the last
    // write time when writing to the file.  If the timestamp is zero, then no
    // change should be made to that timestamp.
    //

    if ((BasicInformation->CreationTime.QuadPart == -1) ||
        (BasicInformation->CreationTime.QuadPart == 0)) {

        //
        // Don't change the creation time.  Grab the current creation time from
        // the file control block.
        //

        CreationTime = Fcb->CreationTime;

    } else {

        //
        // Convert the NT system time to a FAT time.
        //

        if (!FatxTimeToFatTimestamp(&BasicInformation->CreationTime,
            &CreationTime)) {
            return STATUS_INVALID_PARAMETER;
        }

        if (CreationTime.AsULONG != Fcb->CreationTime.AsULONG) {
            ChangesMade = TRUE;
        }
    }

    if ((BasicInformation->LastWriteTime.QuadPart == -1) ||
        (BasicInformation->LastWriteTime.QuadPart == 0)) {

        //
        // Don't change the last write time.  Grab the current last write time
        // from the file control block.
        //

        LastWriteTime = Fcb->LastWriteTime;

        //
        // If the caller doesn't want us updating the last write time, then
        // mark the file control block.
        //

        if (BasicInformation->LastWriteTime.LowPart == -1) {
            Fcb->Flags |= FAT_FCB_DISABLE_LAST_WRITE_TIME;
        }

    } else {

        //
        // Verify that we can convert the time to a FAT timestamp.  At this
        // point, we don't care about the result.
        //

        if (!FatxTimeToFatTimestamp(&BasicInformation->LastWriteTime,
            &FatTimestamp)) {
            return STATUS_INVALID_PARAMETER;
        }

        LastWriteTime = BasicInformation->LastWriteTime;

        if (LastWriteTime.QuadPart != Fcb->LastWriteTime.QuadPart) {
            ChangesMade = TRUE;
        }
    }

    if ((BasicInformation->LastAccessTime.QuadPart == -1) ||
        (BasicInformation->LastAccessTime.QuadPart == 0)) {

        //
        // Don't change the last access time.  Grab the current last access time
        // from the file control block.
        //

        LastAccessTime = Fcb->LastAccessTime;

    } else {

        //
        // Convert the NT system time to a FAT time.
        //

        if (!FatxTimeToFatTimestamp(&BasicInformation->LastAccessTime,
            &LastAccessTime)) {
            return STATUS_INVALID_PARAMETER;
        }

        if (LastAccessTime.AsULONG != Fcb->LastAccessTime.AsULONG) {
            ChangesMade = TRUE;
        }
    }

    //
    // Validate the file attributes.
    //

    if (BasicInformation->FileAttributes == 0) {

        //
        // Don't change the file attributes.  Grab the current file attributes
        // from the file control block.
        //

        FileAttributes = Fcb->FileAttributes;

    } else {

        FileAttributes =
            (UCHAR)FatxFilterFileAttributes(BasicInformation->FileAttributes);

        //
        // Make sure that the directory attribute doesn't change.
        //

        if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
            FileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        } else if (FatxIsFlagSet(FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
            return STATUS_INVALID_PARAMETER;
        }

        if (FileAttributes != Fcb->FileAttributes) {
            ChangesMade = TRUE;
        }
    }

    if (ChangesMade) {

        //
        // Now that we've validated all of the input from the information
        // structure, apply the changes to the file control block and write out
        // the changes.  If the write fails, we can live with the in-memory copy
        // of these attributes being different than the on-disk copies.
        //

        Fcb->CreationTime = CreationTime;
        Fcb->LastWriteTime = LastWriteTime;
        Fcb->LastAccessTime = LastAccessTime;
        Fcb->FileAttributes = FileAttributes;

        status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, Fcb);

    } else {
        status = STATUS_SUCCESS;
    }

    return status;
}

NTSTATUS
FatxSetRenameInformation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine renames the supplied file object.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;
    PFAT_FCB Fcb;
    PFILE_RENAME_INFORMATION RenameInformation;
    POSTR OriginalFileNameBuffer;
    UCHAR OriginalFileNameLength;
    POSTR EndOfFileName;
    POSTR StartOfFileName;
    OBJECT_STRING TargetFileName;
    POSTR TargetFileNameBuffer;
    PFAT_FCB TargetDirectoryFcb;
    ULONG NewPathNameLength;
    DIRENT DirectoryEntry;
    ULONG DirectoryByteOffset;
    ULONG EmptyDirectoryByteOffset;
    PFAT_FCB FoundOrNewFcb;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    Fcb = (PFAT_FCB)FileObject->FsContext;
    RenameInformation = (PFILE_RENAME_INFORMATION)Irp->UserBuffer;

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // It's possible for the file control block to be NULL if a file failed to
    // be moved from one directory to another.  In that case,
    // FatxSetRenameInformation sets the file's parent file control block to
    // NULL.
    //

    if (Fcb->ParentFcb == NULL) {
        return STATUS_FILE_CORRUPT_ERROR;
    }

    //
    // There must be at least some characters in the file name.
    //

    if (RenameInformation->FileName.Length == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Save off the original file name buffer and length so that we can attempt
    // to unroll errors below.
    //

    OriginalFileNameBuffer = Fcb->FileNameBuffer;
    OriginalFileNameLength = Fcb->FileNameLength;

    ASSERT(OriginalFileNameBuffer != NULL);

    //
    // Figure out the starting and ending (exclusive) range of the target file
    // name.  In a normal NT file system, the target file name would have been
    // attached to the target file object, but we don't keep the file name as
    // part of the file objects, so we have to go back to user's original
    // buffer.
    //
    // For compatibility with NT, if the name ends with a backslash, ignore the
    // character.
    //

    EndOfFileName = (POSTR)((PCHAR)RenameInformation->FileName.Buffer +
        RenameInformation->FileName.Length);

    if (*(EndOfFileName - 1) == OBJ_NAME_PATH_SEPARATOR) {
        EndOfFileName--;
    }

    StartOfFileName = EndOfFileName;

    while (StartOfFileName > RenameInformation->FileName.Buffer) {

        if (*StartOfFileName == OBJ_NAME_PATH_SEPARATOR) {
            StartOfFileName++;
            break;
        }

        StartOfFileName--;
    }

    //
    // Validate that this is a legal FAT file name.
    //

    TargetFileName.Length = (USHORT)((PCHAR)EndOfFileName - (PCHAR)StartOfFileName);
    TargetFileName.MaximumLength = TargetFileName.Length;
    TargetFileName.Buffer = StartOfFileName;

    if (!FatxIsValidFatFileName(&TargetFileName)) {
        return STATUS_OBJECT_NAME_INVALID;
    }

    //
    // Figure out which directory the file will be renamed into.
    //

    if (IrpSp->Parameters.SetFile.FileObject != NULL) {
        TargetDirectoryFcb =
            (PFAT_FCB)IrpSp->Parameters.SetFile.FileObject->FsContext;
    } else {
        TargetDirectoryFcb = Fcb->ParentFcb;
    }

    ASSERT(FatxIsFlagSet(TargetDirectoryFcb->Flags, FAT_FCB_DIRECTORY));

    //
    // Verify that the path doesn't exceed the length restictions.
    //

    NewPathNameLength = TargetDirectoryFcb->PathNameLength +
        sizeof(OBJ_NAME_PATH_SEPARATOR) + TargetFileName.Length;

    if (NewPathNameLength > FAT_PATH_NAME_LIMIT * sizeof(OCHAR)) {
        return STATUS_OBJECT_NAME_INVALID;
    }

    //
    // If this is a directory, then verify that there are no open files under
    // this directory.
    //

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY) &&
        !IsListEmpty(&Fcb->Directory.ChildFcbList)) {
        return STATUS_ACCESS_DENIED;
    }

    //
    // Check if the target file name already exists.
    //

    status = FatxLookupElementNameInDirectory(VolumeExtension, Irp,
        TargetDirectoryFcb, &TargetFileName, &DirectoryEntry,
        &DirectoryByteOffset, &EmptyDirectoryByteOffset);

    if (NT_SUCCESS(status)) {

        //
        // The target file name already exists.  Check if the caller allows us
        // to replace an existing file and that we didn't find a directory or
        // read-only file.
        //

        if (!RenameInformation->ReplaceIfExists ||
            FatxIsFlagSet(DirectoryEntry.FileAttributes, FILE_ATTRIBUTE_DIRECTORY) ||
            FatxIsFlagSet(DirectoryEntry.FileAttributes, FILE_ATTRIBUTE_READONLY)) {
            return STATUS_OBJECT_NAME_COLLISION;
        }

        //
        // Don't overwrite a file that's already open.
        //

        if (FatxFindOpenChildFcb(TargetDirectoryFcb, &TargetFileName,
            &FoundOrNewFcb)) {
            return STATUS_ACCESS_DENIED;
        }

        //
        // Delete the existing file.
        //

        status = FatxDeleteFile(VolumeExtension, Irp, TargetDirectoryFcb,
            DirectoryByteOffset);

        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // We can use the old file's directory byte offset as the byte offset
        // for the renamed file, if needed.
        //

        EmptyDirectoryByteOffset = DirectoryByteOffset;

    } else if (status != STATUS_OBJECT_NAME_NOT_FOUND) {

        //
        // Some unknown error was returned, so bail out now.
        //

        return status;
    }

    //
    // We'll need to replace the file name buffer that's currently stored in the
    // file control block.  Allocate a new string and copy the target file name
    // into the buffer.
    //

    TargetFileNameBuffer = (POSTR)ExAllocatePoolWithTag(TargetFileName.Length,
        'nFtF');

    if (TargetFileNameBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(TargetFileNameBuffer, TargetFileName.Buffer,
        TargetFileName.Length);

    //
    // Check if we're renaming inside the same directory.  If so, change the
    // file name in the file control block and commit the change.  If this
    // fails, back out to the original file name.
    //

    if (TargetDirectoryFcb == Fcb->ParentFcb) {

        Fcb->FileNameBuffer = TargetFileNameBuffer;
        Fcb->FileNameLength = (UCHAR)TargetFileName.Length;

        status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, Fcb);

        if (NT_SUCCESS(status)) {

            //
            // If the original file name buffer didn't come from the file
            // control block itself, then free it now.
            //

            if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_FREE_FILE_NAME_BUFFER)) {
                ExFreePool(OriginalFileNameBuffer);
            }

            //
            // We're going to keep the file name that we allocated, so set a
            // flag that the close code can look at to know that the file name
            // buffer must be freed.
            //

            Fcb->Flags |= FAT_FCB_FREE_FILE_NAME_BUFFER;

        } else {
            Fcb->FileNameBuffer = OriginalFileNameBuffer;
            Fcb->FileNameLength = OriginalFileNameLength;
        }

        return status;
    }

    //
    // We're moving a file across directories.
    //

    //
    // If there isn't an empty directory entry, then we'll need to add
    // another cluster to the directory.
    //

    if (EmptyDirectoryByteOffset == MAXULONG) {

        //
        // If we found the end of the directory stream, then we must have
        // established how many bytes are allocated to the file.
        //

        ASSERT(TargetDirectoryFcb->AllocationSize != MAXULONG);

        //
        // The new directory empty will be placed at the start of the new
        // extension.
        //

        EmptyDirectoryByteOffset = TargetDirectoryFcb->AllocationSize;

        //
        // Attempt to add another cluster to the directory's allocation.
        //

        status = FatxExtendDirectoryAllocation(VolumeExtension, Irp,
            TargetDirectoryFcb);

        if (!NT_SUCCESS(status)) {
            ExFreePool(TargetFileNameBuffer);
            return status;
        }
    }

    //
    // Mark the directory entry as deleted.  If the system goes down after
    // successfully committing this change, then we've lost the file.
    //

    status = FatxMarkDirectoryEntryDeleted(VolumeExtension, Irp, Fcb);

    if (!NT_SUCCESS(status)) {
        ExFreePool(TargetFileNameBuffer);
        return status;
    }

    //
    // Detach the file control block from its old parent.
    //

    RemoveEntryList(&Fcb->SiblingFcbLink);
    FatxDereferenceFcb(Fcb->ParentFcb);

    //
    // Attach the file control block to its new parent.
    //

    TargetDirectoryFcb->ReferenceCount++;
    Fcb->ParentFcb = TargetDirectoryFcb;
    InsertHeadList(&TargetDirectoryFcb->Directory.ChildFcbList,
        &Fcb->SiblingFcbLink);

    //
    // Update the file control block with its new file name and directory byte
    // offset.
    //

    Fcb->FileNameBuffer = TargetFileNameBuffer;
    Fcb->FileNameLength = (UCHAR)TargetFileName.Length;
    Fcb->DirectoryByteOffset = EmptyDirectoryByteOffset;

    status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, Fcb);

    if (NT_SUCCESS(status)) {

        //
        // Update the path name length stored in the file control block.
        //

        Fcb->PathNameLength = (UCHAR)NewPathNameLength;

        //
        // If the original file name buffer didn't come from the file control
        // block itself, then free it now.
        //

        if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_FREE_FILE_NAME_BUFFER)) {
            ExFreePool(OriginalFileNameBuffer);
        }

        //
        // We're going to keep the file name that we allocated, so set a flag
        // that the close code can look at to know that the file name buffer
        // must be freed.
        //

        Fcb->Flags |= FAT_FCB_FREE_FILE_NAME_BUFFER;

    } else {

        //
        // If this commit fails, then assume that we're not going to have any
        // hope of restoring the original state of the on-disk structures.
        // Leave the file control block in a zombie state by disconnecting it
        // from any parent directory.
        //

        FatxDbgPrint(("FATX: failed to move file across directories\n"));

        Fcb->FileNameBuffer = OriginalFileNameBuffer;
        Fcb->FileNameLength = OriginalFileNameLength;

        RemoveEntryList(&Fcb->SiblingFcbLink);
        FatxDereferenceFcb(TargetDirectoryFcb);

        Fcb->ParentFcb = NULL;

        ExFreePool(TargetFileNameBuffer);
    }

    return status;
}

NTSTATUS
FatxSetDispositionInformation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFILE_OBJECT FileObject,
    IN PFILE_DISPOSITION_INFORMATION DispositionInformation
    )
/*++

Routine Description:

    This routine changes the disposition information of the supplied file
    object.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    FileObject - Specifies the file object to apply the information to.

    DispositionInformation - Specifies the disposition information to apply to
        the file.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_FCB Fcb;

    Fcb = (PFAT_FCB)FileObject->FsContext;

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // If the caller doesn't want to delete the file, then clear the flags from
    // the file control block and the file object.
    //

    if (!DispositionInformation->DeleteFile) {

        Fcb->Flags &= ~FAT_FCB_DELETE_ON_CLOSE;
        FileObject->DeletePending = FALSE;

        return STATUS_SUCCESS;
    }

    //
    // If this is a read only file, then it cannot be deleted.
    //

    if (FatxIsFlagSet(Fcb->FileAttributes, FILE_ATTRIBUTE_READONLY)) {
        return STATUS_CANNOT_DELETE;
    }

    //
    // If this is a directory, then check if the directory is empty.  We'll also
    // allow corrupt directories to be removed from the disk.
    //

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {

        status = FatxIsDirectoryEmpty(VolumeExtension, Irp, Fcb);

        if ((status != STATUS_SUCCESS) && (status != STATUS_FILE_CORRUPT_ERROR)) {
            return status;
        }
    }

    //
    // Mark the file as delete pending.
    //

    Fcb->Flags |= FAT_FCB_DELETE_ON_CLOSE;
    FileObject->DeletePending = TRUE;

    return STATUS_SUCCESS;
}

NTSTATUS
FatxSetEndOfFileInformation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFILE_OBJECT FileObject,
    IN PFILE_END_OF_FILE_INFORMATION EndOfFileInformation
    )
/*++

Routine Description:

    This routine changes the end of file information of the supplied file
    object.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    FileObject - Specifies the file object to apply the information to.

    EndOfFileInformation - Specifies the end of file information to apply to
        the file.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_FCB FileFcb;
    ULONG NewFileSize;
    ULONG OriginalFileSize;

    FileFcb = (PFAT_FCB)FileObject->FsContext;

    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());
    ASSERT(FileFcb->File.FileMutexExclusiveOwner == KeGetCurrentThread());

    //
    // Validate that the upper 32-bits of the end of file are zero.
    //

    if (EndOfFileInformation->EndOfFile.HighPart != 0) {
        return STATUS_DISK_FULL;
    }

    //
    // If the new file size and the current file size match, then there's
    // nothing to do.
    //

    NewFileSize = EndOfFileInformation->EndOfFile.LowPart;
    OriginalFileSize = FileFcb->FileSize;

    if (NewFileSize == OriginalFileSize) {
        return STATUS_SUCCESS;
    }

    //
    // If the new file size is beyond the current file size, then we might need
    // to extend the file allocation.  Note that we want to make sure that we
    // don't truncate the allocation here if the end of file is less than the
    // allocated size.
    //

    if (NewFileSize > OriginalFileSize) {

        status = FatxSetAllocationSize(VolumeExtension, Irp, FileFcb,
            NewFileSize, FALSE, TRUE);

        if (!NT_SUCCESS(status)) {
            return status;
        }

        ASSERT(FileFcb->FileSize == OriginalFileSize);
    }

    //
    // Change the file size in the file control block to the requested file
    // size.
    //

    FileFcb->FileSize = NewFileSize;

    //
    // Commit the directory change.  If this fails, then back out the changes to
    // the file control block so that we still mirror the on-disk structure.
    //

    status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, FileFcb);

    if (!NT_SUCCESS(status)) {
        FileFcb->FileSize = OriginalFileSize;
    }

    return status;
}

NTSTATUS
FatxFsdSetInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_SET_INFORMATION
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
    PFAT_FCB Fcb;
    PFILE_POSITION_INFORMATION PositionInformation;
    PFILE_ALLOCATION_INFORMATION AllocationInformation;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    Fcb = (PFAT_FCB)FileObject->FsContext;

    //
    // If this is a file control block for a standard file, then we need to
    // acquire the file's mutex in order to synchronize access to the Flags
    // field of the file control block.
    //

    if (FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY)) {
        FatxAcquireFileMutexExclusive(Fcb);
    }

    FatxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto CleanupAndExit;
    }

    //
    // Check if the file object has already been cleaned up.  We don't allow a
    // a file object to be accessed after its handle has been closed.
    //

    if (FatxIsFlagSet(FileObject->Flags, FO_CLEANUP_COMPLETE)) {
        status = STATUS_FILE_CLOSED;
        goto CleanupAndExit;
    }

    //
    // Dispatch the information class function.
    //

    status = STATUS_SUCCESS;

    switch (IrpSp->Parameters.QueryFile.FileInformationClass) {

        case FileBasicInformation:
            //
            // We can't change the time stamps or the file attributes for the
            // volume or the root directory.
            //

            if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_VOLUME) ||
                FatxIsFlagSet(Fcb->Flags, FAT_FCB_ROOT_DIRECTORY)) {
                status = STATUS_INVALID_PARAMETER;
                goto CleanupAndExit;
            }

            status = FatxSetBasicInformation(VolumeExtension, Irp, FileObject,
                (PFILE_BASIC_INFORMATION)Irp->UserBuffer);
            break;

        case FileRenameInformation:
            //
            // We can't rename the volume or the root directory.
            //

            if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_VOLUME) ||
                FatxIsFlagSet(Fcb->Flags, FAT_FCB_ROOT_DIRECTORY)) {
                status = STATUS_INVALID_PARAMETER;
                goto CleanupAndExit;
            }

            status = FatxSetRenameInformation(VolumeExtension, Irp);
            break;

        case FileDispositionInformation:
            //
            // We can't delete the volume or the root directory.
            //

            if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_VOLUME) ||
                FatxIsFlagSet(Fcb->Flags, FAT_FCB_ROOT_DIRECTORY)) {
                status = STATUS_INVALID_PARAMETER;
                goto CleanupAndExit;
            }

            status = FatxSetDispositionInformation(VolumeExtension, Irp, FileObject,
                (PFILE_DISPOSITION_INFORMATION)Irp->UserBuffer);
            break;

        case FilePositionInformation:
            PositionInformation = (PFILE_POSITION_INFORMATION)Irp->UserBuffer;

            //
            // If the file was opened without intermediate buffering, then the
            // byte offset must be sector aligned.
            //

            if (FatxIsFlagSet(FileObject->Flags, FO_NO_INTERMEDIATE_BUFFERING) &&
                !FatxIsSectorAligned(VolumeExtension,
                    PositionInformation->CurrentByteOffset.LowPart)) {
                status = STATUS_INVALID_PARAMETER;
                goto CleanupAndExit;
            }

            FileObject->CurrentByteOffset = PositionInformation->CurrentByteOffset;
            break;

        case FileAllocationInformation:
            AllocationInformation =
                (PFILE_ALLOCATION_INFORMATION)Irp->UserBuffer;

            //
            // We can't change the allocation size for the volume or any
            // directory.
            //

            if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_VOLUME) ||
                FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
                status = STATUS_INVALID_PARAMETER;
                goto CleanupAndExit;
            }

            //
            // Validate that the upper 32-bits of the allocation size are zero.
            //

            if (AllocationInformation->AllocationSize.HighPart != 0) {
                status = STATUS_DISK_FULL;
                goto CleanupAndExit;
            }

            status = FatxSetAllocationSize(VolumeExtension, Irp, Fcb,
                AllocationInformation->AllocationSize.LowPart, FALSE, FALSE);
            break;

        case FileEndOfFileInformation:
            //
            // We can't change the end of file for the volume or any directory.
            //

            if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_VOLUME) ||
                FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
                status = STATUS_INVALID_PARAMETER;
                goto CleanupAndExit;
            }

            status = FatxSetEndOfFileInformation(VolumeExtension, Irp, FileObject,
                (PFILE_END_OF_FILE_INFORMATION)Irp->UserBuffer);
            break;

        default:
            status = STATUS_INVALID_PARAMETER;
            break;
    }

CleanupAndExit:
    FatxReleaseVolumeMutex(VolumeExtension);

    if (FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY)) {
        FatxReleaseFileMutex(Fcb);
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
