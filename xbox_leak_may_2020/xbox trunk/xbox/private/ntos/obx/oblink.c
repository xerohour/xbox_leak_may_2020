/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    oblink.c

Abstract:

    This module implements routines to manage symbolic link objects.

--*/

#include "obp.h"

//
// Local support.
//

VOID
ObpDeleteSymbolicLink(
    PVOID Object
    );

//
// Object type information for symbolic links.
//
DECLSPEC_RDATA OBJECT_TYPE ObSymbolicLinkObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    ObpDeleteSymbolicLink,
    NULL,
    &ObpDefaultObject,
    'bmyS'
};

NTSTATUS
ObpResolveLinkTarget(
    IN POBJECT_STRING LinkTarget,
    OUT PVOID *ReturnedLinkTargetObject
    )
/*++

Routine Description:

    This routine resolves the object specified by the supplied target name.

Arguments:

    LinkTarget - Supplies the name of the link target.

    ReturnedLinkTargetObject - Supplies the location to receive the object
        reference.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    KIRQL OldIrql;
    POBJECT_DIRECTORY Directory;
    OBJECT_STRING RemainingName;
    OBJECT_STRING ElementName;
    PVOID FoundObject;
    POBJECT_HEADER ObjectHeader;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE FileHandle;
    IO_STATUS_BLOCK IoStatusBlock;

    RemainingName = *LinkTarget;

    //
    // Verify that this is an absolute path.
    //

    if ((RemainingName.Length == 0) ||
        (RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR)) {
        return STATUS_INVALID_PARAMETER;
    }

    ObpAcquireObjectManagerLock(&OldIrql);

    Directory = ObpRootDirectoryObject;

    for (;;) {

        ObDissectName(RemainingName, &ElementName, &RemainingName);

        //
        // Verify that there aren't multiple backslashes in the name.
        //

        if ((RemainingName.Length != 0) &&
            (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR)) {
            break;
        }

        //
        // Search for the object in the directory and return an error if it
        // doesn't already exist.
        //

        if (!ObpLookupElementNameInDirectory(Directory, &ElementName, TRUE,
            &FoundObject)) {
            break;
        }

        ObjectHeader = OBJECT_TO_OBJECT_HEADER(FoundObject);

        //
        // If we've consumed the entire path, then we found the object, so
        // return success.
        //

        if (RemainingName.Length == 0) {

            ObjectHeader->PointerCount++;

            *ReturnedLinkTargetObject = FoundObject;

            ObpReleaseObjectManagerLock(OldIrql);

            return STATUS_SUCCESS;
        }

        //
        // Only continue parsing in this loop if we found a directory object.
        //

        if (ObjectHeader->Type != &ObDirectoryObjectType) {

            //
            // Check if the object has a parse procedure.  If not, we don't know
            // how to continue resolving the link.
            //

            if (ObjectHeader->Type->ParseProcedure == NULL) {
                break;
            }

            //
            // Make sure the object stays alive after we drop the object manager
            // lock.
            //

            ObjectHeader->PointerCount++;

            ObpReleaseObjectManagerLock(OldIrql);

            //
            // Invoke the object's parse procedure.
            //

            status = ObjectHeader->Type->ParseProcedure(FoundObject, NULL,
                OBJ_CASE_INSENSITIVE, LinkTarget, &RemainingName, NULL,
                ReturnedLinkTargetObject);

            if (status == STATUS_OBJECT_TYPE_MISMATCH) {

                //
                // The parse procedure failed due to an object type mismatch.
                // We didn't specify a desired object type, so the most likely
                // reason for the failure is that we needed the I/O manager to
                // build a parse context object in order to resolve a file
                // object.  Attempt to resolve the link using NtOpenFile.
                //

                InitializeObjectAttributes(&ObjectAttributes, LinkTarget,
                    OBJ_CASE_INSENSITIVE, NULL, NULL);

                status = NtOpenFile(&FileHandle, 0, &ObjectAttributes,
                    &IoStatusBlock, FILE_SHARE_READ | FILE_SHARE_WRITE |
                    FILE_SHARE_DELETE, FILE_DIRECTORY_FILE);

                if (NT_SUCCESS(status)) {
                    status = ObReferenceObjectByHandle(FileHandle,
                        &IoFileObjectType, ReturnedLinkTargetObject);
                    NtClose(FileHandle);
                }
            }

            ObDereferenceObject(FoundObject);

            return status;
        }

        Directory = (POBJECT_DIRECTORY)FoundObject;
    }

    ObpReleaseObjectManagerLock(OldIrql);

    return STATUS_INVALID_PARAMETER;
}

NTSTATUS
NtCreateSymbolicLinkObject(
    OUT PHANDLE LinkHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN POBJECT_STRING LinkTarget
    )
/*++

Routine Description:

    This routine creates a symbolic link object with the supplied attributes.

Arguments:

    LinkHandle - Supplies the location to receive the created handle.

    ObjectAttributes - Supplies the name and parent directory of the new object.

    LinkTarget - Supplies the name of the link target.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID LinkTargetObject;
    ULONG LinkTargetLength;
    POBJECT_SYMBOLIC_LINK SymbolicLink;
    POSTR LinkTargetBuffer;

    //
    // Resolve the link target.  Unlike the NT implementation, we require the
    // target object to exist.
    //

    status = ObpResolveLinkTarget(LinkTarget, &LinkTargetObject);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Create the symbolic link object.
    //

    LinkTargetLength = LinkTarget->Length;

    status = ObCreateObject(&ObSymbolicLinkObjectType, ObjectAttributes,
        sizeof(OBJECT_SYMBOLIC_LINK) + LinkTargetLength, (PVOID*)&SymbolicLink);

    if (NT_SUCCESS(status)) {

        //
        // Copy the link target name to the symbolic link object.
        //

        LinkTargetBuffer = (POSTR)(SymbolicLink + 1);
        RtlCopyMemory(LinkTargetBuffer, LinkTarget->Buffer, LinkTargetLength);

        SymbolicLink->LinkTargetObject = LinkTargetObject;
        SymbolicLink->LinkTarget.Buffer = LinkTargetBuffer;
        SymbolicLink->LinkTarget.Length = (USHORT)LinkTargetLength;
        SymbolicLink->LinkTarget.MaximumLength = (USHORT)LinkTargetLength;

        status = ObInsertObject(SymbolicLink, ObjectAttributes, 0, LinkHandle);

    } else {
        ObDereferenceObject(LinkTargetObject);
    }

    return status;
}

NTSTATUS
NtOpenSymbolicLinkObject(
    OUT PHANDLE LinkHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
/*++

Routine Description:

    This routine opens an existing symbolic link object with the supplied
    attributes.

Arguments:

    LinkHandle - Supplies the location to receive the opened handle.

    ObjectAttributes - Supplies the name and parent directory of the desired
        object.

Return Value:

    Status of operation.

--*/
{
    return ObOpenObjectByName(ObjectAttributes, &ObSymbolicLinkObjectType, NULL,
        LinkHandle);
}

NTSTATUS
NtQuerySymbolicLinkObject(
    IN HANDLE LinkHandle,
    IN OUT POBJECT_STRING LinkTarget,
    OUT PULONG ReturnedLength OPTIONAL
    )
/*++

Routine Description:

    This routine returns the link target string from the supplied symbolic link.

Arguments:

    LinkHandle - Supplies the handle of the symbolic link.

    LinkTarget - Supplies the buffer to receive the link target string.

    ReturnedLength - Supplies the location to receive the number of bytes
        required to hold the link target string.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    POBJECT_SYMBOLIC_LINK SymbolicLink;
    ULONG BytesRequired;
    ULONG NameBytesToCopy;

    //
    // Reference the symbolic link object.
    //

    status = ObReferenceObjectByHandle(LinkHandle, &ObSymbolicLinkObjectType,
        (PVOID*)&SymbolicLink);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Compute the number of bytes required to copy the string and the number of
    // bytes that we can actually copy.
    //

    BytesRequired = SymbolicLink->LinkTarget.Length;

    if (BytesRequired <= LinkTarget->MaximumLength) {
        NameBytesToCopy = BytesRequired;
        status = STATUS_SUCCESS;
    } else {
        NameBytesToCopy = LinkTarget->MaximumLength;
        status = STATUS_BUFFER_TOO_SMALL;
    }

    RtlCopyMemory(LinkTarget->Buffer, SymbolicLink->LinkTarget.Buffer,
        NameBytesToCopy);

    LinkTarget->Length = (USHORT)NameBytesToCopy;

    //
    // Dereference the object and return the number of bytes that we copied or
    // that are required.
    //

    ObDereferenceObject(SymbolicLink);

    if (ReturnedLength != NULL) {
        *ReturnedLength = BytesRequired;
    }

    return status;
}

VOID
ObpDeleteSymbolicLink(
    PVOID Object
    )
/*++

Routine Description:

    This routine is called when the last reference to a symbolic link object is
    released.  The link target object is released.

Arguments:

    Object - Supplies the symbolic link to delete.

Return Value:

    None.

--*/
{
    ObDereferenceObject(((POBJECT_SYMBOLIC_LINK)Object)->LinkTargetObject);
}
