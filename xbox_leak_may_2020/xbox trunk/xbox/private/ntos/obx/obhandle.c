/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    obhandle.c

Abstract:

    This module implements routines to manage object handles.

--*/

#include "obp.h"

VOID
ObpDetachNamedObject(
    IN PVOID Object,
    IN KIRQL OldIrql
    )
/*++

Routine Description:

    This routine detaches a named object from its parent directory.

Arguments:

    Object - Supplies the named object to remove.

Return Value:

    None.

--*/
{
    POBJECT_HEADER_NAME_INFO ObjectHeaderNameInfo;
    POBJECT_DIRECTORY Directory;
    OCHAR DriveLetter;
    ULONG HashIndex;
    POBJECT_HEADER_NAME_INFO LastObjectHeaderNameInfo;
    POBJECT_HEADER_NAME_INFO CurrentObjectHeaderNameInfo;

    ObpAssertObjectManagerLock();

    ASSERT(ObpIsFlagSet(OBJECT_TO_OBJECT_HEADER(Object)->Flags, OB_FLAG_NAMED_OBJECT));
    ASSERT(ObpIsFlagSet(OBJECT_TO_OBJECT_HEADER(Object)->Flags, OB_FLAG_ATTACHED_OBJECT));

    ObjectHeaderNameInfo = OBJECT_TO_OBJECT_HEADER_NAME_INFO(Object);

    Directory = ObjectHeaderNameInfo->Directory;

    //
    // If we're removing a drive letter relative to the DOS devices directory,
    // then also remove the object from the quick array lookup.
    //

    if ((Directory == ObpDosDevicesDirectoryObject) &&
        (ObjectHeaderNameInfo->Name.Length == sizeof(OCHAR) * 2) &&
        (ObjectHeaderNameInfo->Name.Buffer[1] == (OCHAR)':')) {

        DriveLetter = ObjectHeaderNameInfo->Name.Buffer[0];

        if (DriveLetter >= 'a' && DriveLetter <= 'z') {
            ASSERT(ObpDosDevicesDriveLetterMap[DriveLetter - 'a'] != NULL);
            ObpDosDevicesDriveLetterMap[DriveLetter - 'a'] = NULL;
        } else if (DriveLetter >= 'A' && DriveLetter <= 'Z') {
            ASSERT(ObpDosDevicesDriveLetterMap[DriveLetter - 'A'] != NULL);
            ObpDosDevicesDriveLetterMap[DriveLetter - 'A'] = NULL;
        }
    }

    //
    // Compute the hash index for the object's name.
    //

    HashIndex = ObpComputeHashIndex(&ObjectHeaderNameInfo->Name);

    //
    // Walk through the directory's hash table to find the object so that we
    // can detach it.
    //

    LastObjectHeaderNameInfo = NULL;
    CurrentObjectHeaderNameInfo = Directory->HashBuckets[HashIndex];

    while (CurrentObjectHeaderNameInfo != ObjectHeaderNameInfo) {

        LastObjectHeaderNameInfo = CurrentObjectHeaderNameInfo;
        CurrentObjectHeaderNameInfo = CurrentObjectHeaderNameInfo->ChainLink;

        ASSERT(CurrentObjectHeaderNameInfo != NULL);
    }

    //
    // Remove the entry.
    //

    if (LastObjectHeaderNameInfo == NULL) {
        Directory->HashBuckets[HashIndex] = CurrentObjectHeaderNameInfo->ChainLink;
    } else {
        LastObjectHeaderNameInfo->ChainLink = CurrentObjectHeaderNameInfo->ChainLink;
    }

    //
    // Null out the link and directory fields so that we don't attempt to do
    // this again.
    //

    ObjectHeaderNameInfo->ChainLink = NULL;
    ObjectHeaderNameInfo->Directory = NULL;

    //
    // Release the reference to the directory and the object which applied in
    // ObInsertObject.
    //

    ObpReleaseObjectManagerLock(OldIrql);

    ObDereferenceObject(Directory);
    ObDereferenceObject(Object);
}

NTSTATUS
NtClose(
    IN HANDLE Handle
    )
/*++

Routine Description:

    This routine closes the supplied handle.

Arguments:

    Handle - Supplies the handle to close.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    KIRQL OldIrql;
    PVOID Object;
    POBJECT_HEADER ObjectHeader;
    ULONG HandleCount;

    ObpAcquireObjectManagerLock(&OldIrql);

    //
    // Attempt to destroy the handle and obtain the object that was stored
    // in the handle.
    //

    Object = ObpDestroyObjectHandle(Handle);

    if (Object != NULL) {

        ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

        //
        // Grab the current handle count for the object.
        //

        HandleCount = ObjectHeader->HandleCount;
        ASSERT(HandleCount > 0);

        //
        // Decrement the number of handles for this object.
        //

        ObjectHeader->HandleCount--;

        //
        // If the object type has a close procedure, invoke it with the old
        // handle count.
        //

        if (ObjectHeader->Type->CloseProcedure != NULL) {

            ObpReleaseObjectManagerLock(OldIrql);

            ObjectHeader->Type->CloseProcedure(Object, HandleCount);

            ObpAcquireObjectManagerLock(&OldIrql);
        }

        //
        // Check if the object should be removed from its parent directory.
        //

        if ((ObjectHeader->HandleCount == 0) &&
            ObpIsFlagSet(ObjectHeader->Flags, OB_FLAG_ATTACHED_OBJECT) &&
            ObpIsFlagClear(ObjectHeader->Flags, OB_FLAG_PERMANENT_OBJECT)) {
            ObpDetachNamedObject(Object, OldIrql);
        } else {
            ObpReleaseObjectManagerLock(OldIrql);
        }

        //
        // Release the reference that the handle held on the object.
        //

        ObDereferenceObject(Object);

        status = STATUS_SUCCESS;

    } else {

        ObpReleaseObjectManagerLock(OldIrql);

        status = STATUS_INVALID_HANDLE;
    }

    return status;
}

NTSTATUS
NtDuplicateObject(
    IN HANDLE SourceHandle,
    OUT PHANDLE TargetHandle,
    IN ULONG Options
    )
/*++

Routine Description:

    This routine duplicates the supplied handle.

Arguments:

    SourceHandle - Supplies the handle to duplicate.

    TargetHandle - Supplies the location to receive the duplicated handle.

    Options - Supplies options that control the duplication process.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID Object;

    //
    // Obtain the object referenced by the handle.
    //

    status = ObReferenceObjectByHandle(SourceHandle, NULL, &Object);

    if (!NT_SUCCESS(status)) {
        *TargetHandle = NULL;
        return status;
    }

    //
    // Close the source handle if requested to.
    //

    if (ObpIsFlagSet(Options, DUPLICATE_CLOSE_SOURCE)) {
        NtClose(SourceHandle);
    }

    //
    // Create a handle for the object and release the reference we acquired
    // above.
    //

    status = ObOpenObjectByPointer(Object, OBJECT_TO_OBJECT_HEADER(Object)->Type,
        TargetHandle);

    ObDereferenceObject(Object);

    return status;
}

NTSTATUS
NtMakeTemporaryObject(
    IN HANDLE Handle
    )
/*++

Routine Description:

    This routine converts a permanent object to a non permanent object.  Non
    permanent objects are removed from the object directory when the last
    handle is closed.

Arguments:

    Handle - Supplies the handle to make temporary.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID Object;

    status = ObReferenceObjectByHandle(Handle, NULL, &Object);

    if (NT_SUCCESS(status)) {
        ObMakeTemporaryObject(Object);
        ObDereferenceObject(Object);
    }

    return status;
}

VOID
ObMakeTemporaryObject (
    IN PVOID Object
    )
/*++

Routine Description:

    This routine converts a permanent object to a non permanent object.  Non
    permanent objects are removed from the object directory when the last
    handle is closed.

Arguments:

    Object - Supplies the object to make temporary.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;
    POBJECT_HEADER ObjectHeader;

    ObpAcquireObjectManagerLock(&OldIrql);

    //
    // Clear the permanent flag.
    //

    ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
    ObjectHeader->Flags &= ~OB_FLAG_PERMANENT_OBJECT;

    //
    // Check if the object should be removed from its parent directory.
    //

    if ((ObjectHeader->HandleCount == 0) &&
        ObpIsFlagSet(ObjectHeader->Flags, OB_FLAG_ATTACHED_OBJECT)) {
        ObpDetachNamedObject(Object, OldIrql);
    } else {
        ObpReleaseObjectManagerLock(OldIrql);
    }
}
