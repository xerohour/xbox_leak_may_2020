/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fcbsup.c

Abstract:

    This module implements routines which provide support for file control
    blocks.

--*/

#include "gdfx.h"

NTSTATUS
GdfxCreateFcb(
    IN PGDF_FCB ParentFcb OPTIONAL,
    IN POBJECT_STRING FileName,
    IN PGDF_DIRECTORY_ENTRY DirectoryEntry,
    OUT PGDF_FCB *ReturnedFcb
    )
/*++

Routine Description:

    This routine reads the file entry descriptor at the supplied extent and
    constructs a file control block that represents the file.

Arguments:

    ParentFcb - Specifies the parent directory that contains the supplied file.

    FileName - Specifies the name of the file.

    DirectoryEntry - Specifies the directory entry to obtain more attributes
        about the file.  The directory entry does not include the full file
        name.

    ReturnedFcb - Specifies the buffer to receive the created file control
        block.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    SIZE_T FcbSize;
    ULONG FcbPoolTag;
    PGDF_FCB Fcb;

    //
    // Compute the size of the file control block.
    //

    if (GdfxIsFlagSet(DirectoryEntry->FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
        FcbPoolTag = 'cDxG';
    } else {
        FcbPoolTag = 'cFxG';
    }

    FcbSize = sizeof(GDF_FCB) + FileName->Length;

    //
    // Allocate the file control block.
    //

    Fcb = ExAllocatePoolWithTag(FcbSize, FcbPoolTag);

    if (Fcb != NULL) {

        //
        // Initialize the file control block.
        //

        RtlZeroMemory(Fcb, FcbSize);

        //
        // A file control block starts with the single reference for the caller.
        //

        Fcb->ReferenceCount = 1;

        //
        // Copy the file name to the file control block.
        //

        RtlCopyMemory(Fcb->FileName, FileName->Buffer, FileName->Length);
        Fcb->FileNameLength = (UCHAR)FileName->Length;

        //
        // Copy the file's starting sector and size to the file control block.
        //

        Fcb->FirstSector = DirectoryEntry->FirstSector;
        Fcb->FileSize = DirectoryEntry->FileSize;

        //
        // Increment the reference count for the parent file control block and
        // attach it to this file control block.
        //

        ParentFcb->ReferenceCount++;
        Fcb->ParentFcb = ParentFcb;
        InsertHeadList(&ParentFcb->ChildFcbList, &Fcb->SiblingFcbLink);

        //
        // For directories, mark the file control block as a directory and
        // initialize the child file control block list.
        //

        if (GdfxIsFlagSet(DirectoryEntry->FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
            Fcb->Flags |= GDF_FCB_DIRECTORY;
            InitializeListHead(&Fcb->ChildFcbList);
        }

        status = STATUS_SUCCESS;

    } else {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    *ReturnedFcb = Fcb;

    return status;
}

BOOLEAN
GdfxFindOpenChildFcb(
    IN PGDF_FCB DirectoryFcb,
    IN POBJECT_STRING FileName,
    OUT PGDF_FCB *ReturnedFcb
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
    PGDF_FCB Fcb;
    OBJECT_STRING FcbFileName;

    //
    // Walk through the file control blocks actively in use by the volume and
    // find a match.
    //

    NextFcbLink = DirectoryFcb->ChildFcbList.Flink;

    while (NextFcbLink != &DirectoryFcb->ChildFcbList) {

        Fcb = CONTAINING_RECORD(NextFcbLink, GDF_FCB, SiblingFcbLink);

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
GdfxDereferenceFcb(
    IN PGDF_FCB Fcb
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
    PGDF_FCB ParentFcb;

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

        if (GdfxIsFlagSet(Fcb->Flags, GDF_FCB_DIRECTORY)) {
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
