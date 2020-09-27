/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fcbsup.c

Abstract:

    This module implements routines which provide support for file control
    blocks.

--*/

#include "udfx.h"

NTSTATUS
UdfxCreateFcbFromFileEntry(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PLONGAD IcbExtent,
    IN PUDF_FCB ParentFcb OPTIONAL,
    IN POBJECT_STRING FileName OPTIONAL,
    OUT PUDF_FCB *ReturnedFcb
    )
/*++

Routine Description:

    This routine reads the file entry descriptor at the supplied extent and
    constructs a file control block that represents the file.

Arguments:

    VolumeExtension - Specifies the volume to read the file entry descriptor
        from.

    Irp - Specifies an IRP that can be used for cache I/O.

    IcbExtent - Specifies the extent of the file entry descriptor.

    ParentFcb - Specifies the parent directory that contains the supplied file.

    FileName - Specifies the name of the file.

    ReturnedFcb - Specifies the buffer to receive the created file control
        block.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    SIZE_T FcbSize;
    PUDF_FCB Fcb;
    PICBFILE IcbFile;
    ULONG AllocationDescriptorType;
    PLONGAD LongExtent;
    PSHORTAD ShortExtent;

    *ReturnedFcb = NULL;

    Fcb = NULL;
    IcbFile = NULL;

    //
    // Verify that the extent specifies the single supported partition.
    //

    if (IcbExtent->Start.Partition != 0) {
        status = STATUS_DISK_CORRUPT_ERROR;
        goto CleanupAndExit;
    }

    //
    // Compute the size of the file control block.
    //

    FcbSize = sizeof(UDF_FCB) + ((FileName != NULL) ? FileName->Length : 0);

    //
    // Allocate and initialize the file control block.
    //

    Fcb = ExAllocatePoolWithTag(FcbSize, 'cFxU');

    if (Fcb == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto CleanupAndExit;
    }

    RtlZeroMemory(Fcb, FcbSize);

    InitializeListHead(&Fcb->ChildFcbList);

    //
    // Copy the file name into the file control block.
    //

    if (FileName != NULL) {
        RtlCopyMemory(Fcb->FileName, FileName->Buffer, FileName->Length);
        Fcb->FileNameLength = (UCHAR)FileName->Length;
    }

    //
    // Map the file descriptor into the cache.
    //

    status = UdfxMapLogicalSector(VolumeExtension, Irp, IcbExtent->Start.Lbn,
        (PVOID*)&IcbFile);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Verify that this is a file descriptor.
    //

    if (!UdfxVerifyDescriptor(&IcbFile->Destag, UDF_CD_SECTOR_SIZE,
        DESTAG_ID_NSR_FILE, IcbExtent->Start.Lbn)) {

        UdfxDbgPrint(("UDFX: file descriptor doesn't verify\n"));
        status = STATUS_DISK_CORRUPT_ERROR;
        goto CleanupAndExit;
    }

    //
    // Verify that the size of the file descriptor's variable fields doesn't
    // exceed the size of a sector.  Avoid overflow problems by checking each
    // field individually as well.
    //

    if ((IcbFile->EALength > UDF_CD_SECTOR_SIZE) ||
        (IcbFile->AllocLength > UDF_CD_SECTOR_SIZE) ||
        ((FIELD_OFFSET(ICBFILE, EAs) + IcbFile->EALength + IcbFile->AllocLength) >
            UDF_CD_SECTOR_SIZE)) {
        UdfxDbgPrint(("UDFX: file descriptor has too large of variant structures\n"));
        status = STATUS_DISK_CORRUPT_ERROR;
        goto CleanupAndExit;
    }

    //
    // Verify that this is a file type that we can support and mark the file
    // control block accordingly.
    //

    if (IcbFile->Icbtag.FileType == ICBTAG_FILE_T_DIRECTORY) {
        Fcb->Flags |= UDF_FCB_DIRECTORY;
    } else if (IcbFile->Icbtag.FileType != ICBTAG_FILE_T_FILE) {
        UdfxDbgPrint(("UDFX: file descriptor has unsupported file type\n"));
        status = STATUS_DISK_CORRUPT_ERROR;
        goto CleanupAndExit;
    }

    //
    // Copy the file size to the file control block and verify that the length
    // is limited to 32-bits.  We only support a single allocation extent and
    // that's limited to a single ULONG.
    //

    Fcb->FileSize.QuadPart = IcbFile->InfoLength;

    if (Fcb->FileSize.HighPart != 0) {
        UdfxDbgPrint(("UDFX: file descriptor has too large of file size\n"));
        status = STATUS_DISK_CORRUPT_ERROR;
        goto CleanupAndExit;
    }

    //
    // Copy the modify timestamp to the file control block.  We don't bother
    // storing any of the other timestamps that are part of the file control
    // block or stored as extended attributes.
    //

    Fcb->ModifyTime = IcbFile->ModifyTime;

    //
    // Verify that the strategy type is direct mode.
    //

    if (IcbFile->Icbtag.StratType != ICBTAG_STRAT_DIRECT) {
        UdfxDbgPrint(("UDFX: file descriptor has unsupported ICB strategy\n"));
        status = STATUS_DISK_CORRUPT_ERROR;
        goto CleanupAndExit;
    }

    //
    // Verify that we know how to process the allocation descriptor encoding.
    //

    AllocationDescriptorType = (IcbFile->Icbtag.Flags & ICBTAG_F_ALLOC_MASK);

    if ((Fcb->FileSize.LowPart == 0) && (IcbFile->AllocLength == 0)) {

        //
        // If this is a zero length file, then there may not be any allocation
        // descriptors for the file.
        //

        ASSERT(Fcb->AllocationSectorStart == 0);
        ASSERT(Fcb->EmbeddedDataOffset == 0);

    } else if (AllocationDescriptorType == ICBTAG_F_ALLOC_IMMEDIATE) {

        //
        // The size of the file should match the length of the allocation data.
        //

        if (Fcb->FileSize.LowPart != IcbFile->AllocLength) {
            UdfxDbgPrint(("UDFX: file descriptor has invalid allocation length\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        Fcb->Flags |= UDF_FCB_EMBEDDED_DATA;
        Fcb->AllocationSectorStart = IcbExtent->Start.Lbn;
        Fcb->EmbeddedDataOffset = (USHORT)
            (FIELD_OFFSET(ICBFILE, EAs) + IcbFile->EALength);

    } else if (AllocationDescriptorType == ICBTAG_F_ALLOC_LONG) {

        if (IcbFile->AllocLength != sizeof(LONGAD)) {
            UdfxDbgPrint(("UDFX: file descriptor has invalid allocation length\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        LongExtent = (PLONGAD)((PUCHAR)IcbFile + FIELD_OFFSET(ICBFILE, EAs) +
            IcbFile->EALength);

        //
        // Verify that the extent specifies the single supported partition.
        //

        if (LongExtent->Start.Partition != 0) {
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        //
        // The size of the file should match the length of the extent.
        //

        if (Fcb->FileSize.LowPart != LongExtent->Length.Length) {
            UdfxDbgPrint(("UDFX: file descriptor has invalid extent length\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        Fcb->AllocationSectorStart = LongExtent->Start.Lbn;
        ASSERT(Fcb->EmbeddedDataOffset == 0);

    } else if (AllocationDescriptorType == ICBTAG_F_ALLOC_SHORT) {

        if (IcbFile->AllocLength != sizeof(SHORTAD)) {
            UdfxDbgPrint(("UDFX: file descriptor has invalid allocation length\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        ShortExtent = (PSHORTAD)((PUCHAR)IcbFile + FIELD_OFFSET(ICBFILE, EAs) +
            IcbFile->EALength);

        //
        // The size of the file should match the length of the extent.
        //

        if (Fcb->FileSize.LowPart != ShortExtent->Length.Length) {
            UdfxDbgPrint(("UDFX: file descriptor has invalid extent length\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
            goto CleanupAndExit;
        }

        Fcb->AllocationSectorStart = ShortExtent->Start;
        ASSERT(Fcb->EmbeddedDataOffset == 0);

    } else {

        UdfxDbgPrint(("UDFX: file descriptor has unsupported allocation type\n"));
        status = STATUS_DISK_CORRUPT_ERROR;
        goto CleanupAndExit;
    }

    //
    // A file control block starts with the single reference for the caller.
    //

    Fcb->ReferenceCount = 1;

    //
    // Increment the reference count for the parent file control block and
    // attach it to this file control block.
    //

    if (ParentFcb != NULL) {
        ParentFcb->ReferenceCount++;
        Fcb->ParentFcb = ParentFcb;
        InsertHeadList(&ParentFcb->ChildFcbList, &Fcb->SiblingFcbLink);
    }

    //
    // The file control block is successfully filled in.  Return the results to
    // the caller.
    //

    *ReturnedFcb = Fcb;
    Fcb = NULL;
    status = STATUS_SUCCESS;

CleanupAndExit:
    if (IcbFile != NULL) {
        FscUnmapBuffer(IcbFile);
    }

    if (Fcb != NULL) {
        ExFreePool(Fcb);
    }

    return status;
}

BOOLEAN
UdfxFindOpenChildFcb(
    IN PUDF_FCB DirectoryFcb,
    IN POBJECT_STRING FileName,
    OUT PUDF_FCB *ReturnedFcb
    )
/*++

Routine Description:

    This routine searches the child file control block for the supplied
    directory for a file that has the supplied file name.

Arguments:

    DirectoryFcb - Specifies the file control block of the directory to search.

    FileName - Specifies the name to search for in the directory.

    ReturnedFcb - Specifies the buffer to receive the found file control block.

Return Value:

    Returns TRUE if the file control block was found, else FALSE.

--*/
{
    PLIST_ENTRY NextFcbLink;
    PUDF_FCB Fcb;
    OBJECT_STRING FcbFileName;

    //
    // Walk through the file control blocks actively in use by the volume and
    // find a match.
    //

    NextFcbLink = DirectoryFcb->ChildFcbList.Flink;

    while (NextFcbLink != &DirectoryFcb->ChildFcbList) {

        Fcb = CONTAINING_RECORD(NextFcbLink, UDF_FCB, SiblingFcbLink);

        FcbFileName.Length = Fcb->FileNameLength;
        FcbFileName.Buffer = Fcb->FileName;

        if ((FcbFileName.Length == FileName->Length) &&
            RtlEqualObjectString(&FcbFileName, FileName, TRUE)) {
            *ReturnedFcb = Fcb;
            return TRUE;
        }

        NextFcbLink = Fcb->SiblingFcbLink.Flink;
    }

    *ReturnedFcb = NULL;
    return FALSE;
}

VOID
UdfxDereferenceFcb(
    IN PUDF_FCB Fcb
    )
/*++

Routine Description:

    This routine decrements the reference count on the supplied file control
    block.  If the reference count reaches zero, then the file control block is
    deleted.

Arguments:

    Fcb - Specifies the file control block to dereference.

Return Value:

    None.

--*/
{
    PUDF_FCB ParentFcb;

    ASSERT(Fcb->ReferenceCount > 0);

    do {

        //
        // Decrement the reference count and bail out if there are still
        // outstanding references to the file control block.
        //

        if (--Fcb->ReferenceCount != 0) {
            return;
        }

        //
        // Verify that the child file control block list is empty if this is a
        // directory.
        //

        if (UdfxIsFlagSet(Fcb->Flags, UDF_FCB_DIRECTORY)) {
            ASSERT(IsListEmpty(&Fcb->ChildFcbList));
        }

        //
        // Save off the parent file control block so that we can dereference it
        // in a bit.
        //

        ParentFcb = Fcb->ParentFcb;

        //
        // Remove this file control block from the list of siblings.
        //

        if (ParentFcb != NULL) {
            RemoveEntryList(&Fcb->SiblingFcbLink);
        }

        //
        // Free the file control block.
        //

        ExFreePool(Fcb);

        //
        // Switch to the parent file control block and restart the loop to
        // dereference this object.
        //

        Fcb = ParentFcb;

    } while (Fcb != NULL);
}
