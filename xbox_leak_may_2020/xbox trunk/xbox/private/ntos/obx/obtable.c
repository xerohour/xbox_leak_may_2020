/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    obtable.c

Abstract:

    This module implements routines to maintain a handle table.

--*/

#include "obp.h"

//
// Global object handle table.
//
OBJECT_HANDLE_TABLE ObpObjectHandleTable;

//
// Define macros to encode and decode the linked list of free handle in the
// object handle table.
//

#define ObpEncodeFreeHandleLink(Link) \
    (((ULONG_PTR)(Link)) | 1)

#define ObpDecodeFreeHandleLink(Link) \
    (((ULONG_PTR)(Link)) & (~1))

#define ObpIsFreeHandleLink(Link) \
    ((LOGICAL)(((ULONG_PTR)(Link)) & 1))

//
// Define a macro to mask off the reserved application bits from a handle.
//

#define ObpMaskOffApplicationBits(Handle) \
    ((HANDLE)(((ULONG_PTR)(Handle)) & ~(sizeof(ULONG) - 1)))

//
// Define macros to index the object handle table.
//

#define ObpGetTableByteOffsetFromHandle(Handle) \
    (HandleToUlong(Handle) & (OB_HANDLES_PER_TABLE * sizeof(PVOID) - 1))

#define ObpGetTableFromHandle(Handle) \
    ObpObjectHandleTable.RootTable[HandleToUlong(Handle) >> (OB_HANDLES_PER_TABLE_SHIFT + 2)]

#define ObpGetHandleContentsPointer(Handle) \
    ((PVOID*)((PUCHAR)ObpGetTableFromHandle(Handle) + ObpGetTableByteOffsetFromHandle(Handle)))

BOOLEAN
ObpExtendObjectHandleTable(
    VOID
    )
/*++

Routine Description:

    This routine extends the tables in the object handle table in order to hold
    more handles.

Arguments:

    None.

Return Value:

    Returns TRUE if the tables were successfully extended, else FALSE.

--*/
{
    PVOID *NewTable;
    SIZE_T OldRootTableSize;
    SIZE_T NewRootTableSize;
    PVOID **NewRootTable;
    HANDLE Handle;
    LONG_PTR FreeHandleLink;
    PVOID *HandleContents;
    ULONG Index;

    ObpAssertObjectManagerLock();

    //
    // Allocate a new table.
    //

    NewTable = (PVOID*)ExAllocatePoolWithTag(sizeof(PVOID) * OB_HANDLES_PER_TABLE,
        'tHbO');

    if (NewTable == NULL) {
        return FALSE;
    }

    //
    // Check if the root table needs to be extended.
    //

    if ((HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool) &
        (sizeof(PVOID) * OB_HANDLES_PER_SEGMENT - 1)) == 0) {

        if (ObpObjectHandleTable.NextHandleNeedingPool == NULL) {

            //
            // Simple case.  Use the builtin root table.
            //

            NewRootTable = ObpObjectHandleTable.BuiltinRootTable;

        } else {

            OldRootTableSize = HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool) /
                (sizeof(PVOID*) * OB_HANDLES_PER_TABLE);
            NewRootTableSize = OldRootTableSize + OB_TABLES_PER_SEGMENT;

            //
            // Allocate a new root table.
            //

            NewRootTable = (PVOID**)ExAllocatePoolWithTag(sizeof(PVOID*) *
                NewRootTableSize, 'rHbO');

            if (NewRootTable == NULL) {
                ExFreePool(NewTable);
                return FALSE;
            }

            //
            // Copy the old table to the new table.
            //

            RtlCopyMemory(NewRootTable, ObpObjectHandleTable.RootTable,
                sizeof(PVOID*) * OldRootTableSize);

            //
            // Free the old root table if necessary and switch to the new
            // root table.
            //

            if (ObpObjectHandleTable.RootTable !=
                ObpObjectHandleTable.BuiltinRootTable) {
                ExFreePool(ObpObjectHandleTable.RootTable);
            }
        }

        ObpObjectHandleTable.RootTable = NewRootTable;
    }

    //
    // Attach the new table to the root table.
    //

    ObpGetTableFromHandle(ObpObjectHandleTable.NextHandleNeedingPool) = NewTable;

    //
    // Compute the first handle that will be placed on the free list and it's
    // table position.
    //

    Handle = ObpObjectHandleTable.NextHandleNeedingPool;
    HandleContents = NewTable;
    ASSERT(HandleContents == ObpGetHandleContentsPointer(Handle));

    //
    // Chain the handles from the new table into a free list.
    //

    FreeHandleLink = ObpEncodeFreeHandleLink(Handle);
    ObpObjectHandleTable.FirstFreeTableEntry = FreeHandleLink;

    for (Index = 0; Index < OB_HANDLES_PER_TABLE - 1; Index++) {
        FreeHandleLink += sizeof(PVOID);
        *HandleContents++ = (PVOID)FreeHandleLink;
    }

    //
    // Terminate the free handle list with a negative one.
    //

    *HandleContents = (PVOID)-1;

    //
    // Special case the NULL handle by removing it from the list and setting
    // it's value to NULL.
    //

    if (Handle == NULL) {

        HandleContents = NewTable;
        ASSERT(HandleContents == ObpGetHandleContentsPointer(Handle));

        ObpObjectHandleTable.FirstFreeTableEntry = (LONG_PTR)*HandleContents;
        *HandleContents = NULL;
    }

    ASSERT(ObpIsFreeHandleLink(ObpObjectHandleTable.FirstFreeTableEntry));

    ObpObjectHandleTable.NextHandleNeedingPool = (HANDLE)(HandleToLong(Handle) +
        (sizeof(PVOID) * OB_HANDLES_PER_TABLE));

    return TRUE;
}

HANDLE
ObpCreateObjectHandle(
    PVOID Object
    )
/*++

Routine Description:

    This routine creates an object handle for the supplied object pointer.

Arguments:

    Object - Supplies the object pointer to insert into the handle table.

Return Value:

    Returns the allocated handle or NULL if no resources are available.

--*/
{
    HANDLE Handle;
    PVOID *HandleContents;

    ASSERT((Object != NULL) && !ObpIsFreeHandleLink(Object));

    ObpAssertObjectManagerLock();

    //
    // Check if the object handle table needs to be extended.
    //

    if (ObpObjectHandleTable.FirstFreeTableEntry == -1) {
        if (!ObpExtendObjectHandleTable()) {
            return NULL;
        }
    }

    //
    // Pop the first free handle from the list and initialize the handle to
    // contain the supplied object pointer.
    //

    ASSERT(ObpIsFreeHandleLink(ObpObjectHandleTable.FirstFreeTableEntry));

    Handle = (HANDLE)ObpDecodeFreeHandleLink(ObpObjectHandleTable.FirstFreeTableEntry);
    HandleContents = ObpGetHandleContentsPointer(Handle);

    ObpObjectHandleTable.FirstFreeTableEntry = (LONG_PTR)*HandleContents;

    ASSERT(ObpIsFreeHandleLink(ObpObjectHandleTable.FirstFreeTableEntry));

    ObpObjectHandleTable.HandleCount++;

    OBJECT_TO_OBJECT_HEADER(Object)->HandleCount++;

    *HandleContents = Object;

    return Handle;
}

PVOID
ObpGetObjectHandleReference(
    HANDLE Handle
    )
/*++

Routine Description:

    This routine returns a reference to the supplied handle's contents.

Arguments:

    Handle - Supplies the object handle to reference.

Return Value:

    Returns a referenced object pointer.  ObDereferenceObject should be used to
    release the reference.  Returns NULL if the supplied object handle is
    invalid.

--*/
{
    KIRQL OldIrql;
    PVOID *HandleContents;
    PVOID Object;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    Handle = ObpMaskOffApplicationBits(Handle);

    ObpAcquireObjectManagerLock(&OldIrql);

    //
    // Verify that the handle is not past the end of the allocate handle range.
    //

    if (HandleToUlong(Handle) < HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool)) {

        HandleContents = ObpGetHandleContentsPointer(Handle);
        Object = *HandleContents;

        //
        // Verify that the handle isn't a free handle link.
        //

        if (Object != NULL && !ObpIsFreeHandleLink(Object)) {

            OBJECT_TO_OBJECT_HEADER(Object)->PointerCount++;

            ObpReleaseObjectManagerLock(OldIrql);

            return Object;
        }
    }

    ObpReleaseObjectManagerLock(OldIrql);

    return NULL;
}

PVOID
ObpGetObjectHandleContents(
    HANDLE Handle
    )
/*++

Routine Description:

    This routine returns the supplied handle's contents.

Arguments:

    Handle - Supplies the object handle to reference.

Return Value:

    Returns a unreferenced object pointer.

--*/
{
    PVOID *HandleContents;
    PVOID Object;

    Handle = ObpMaskOffApplicationBits(Handle);

    ObpAssertObjectManagerLock();

    //
    // Verify that the handle is not past the end of the allocate handle range.
    //

    if (HandleToUlong(Handle) < HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool)) {

        HandleContents = ObpGetHandleContentsPointer(Handle);
        Object = *HandleContents;

        //
        // Verify that the handle isn't a free handle link.
        //

        if (Object != NULL && !ObpIsFreeHandleLink(Object)) {
            return Object;
        }
    }

    return NULL;
}

PVOID
ObpDestroyObjectHandle(
    HANDLE Handle
    )
/*++

Routine Description:

    This routine destroys an object handle allocated by ObCreateObjectHandle.

Arguments:

    Handle - Supplies the object handle to remove from the handle table.

Return Value:

    Returns the object pointer if the handle is valid, else NULL.

--*/
{
    PVOID *HandleContents;
    PVOID Object;

    Handle = ObpMaskOffApplicationBits(Handle);

    ObpAssertObjectManagerLock();

    //
    // Verify that the handle is not past the end of the allocate handle range.
    //

    if (HandleToUlong(Handle) < HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool)) {

        HandleContents = ObpGetHandleContentsPointer(Handle);
        Object = *HandleContents;

        //
        // Verify that the handle isn't a free handle link.
        //

        if (Object != NULL && !ObpIsFreeHandleLink(Object)) {

            //
            // Push this handle on the free list.
            //

            *HandleContents = (PVOID)ObpObjectHandleTable.FirstFreeTableEntry;

            ObpObjectHandleTable.FirstFreeTableEntry =
                ObpEncodeFreeHandleLink(Handle);

            ObpObjectHandleTable.HandleCount--;

            return Object;
        }
    }

    return NULL;
}
