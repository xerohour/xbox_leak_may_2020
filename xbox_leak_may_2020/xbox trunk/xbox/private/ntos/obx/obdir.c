/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    obdir.c

Abstract:

    This module implements routines to manage directory objects.

--*/

#include "obp.h"

//
// Object type information for directories.
//
DECLSPEC_RDATA OBJECT_TYPE ObDirectoryObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    NULL,
    NULL,
    &ObpDefaultObject,
    'eriD'
};

//
// Array lookup of the DOS drive letters for the DOS devices directory object.
//
PVOID ObpDosDevicesDriveLetterMap['Z' - 'A' + 1];

NTSTATUS
NtCreateDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
/*++

Routine Description:

    This routine creates a directory object with the supplied attributes.

Arguments:

    DirectoryHandle - Supplies the location to receive the created handle.

    ObjectAttributes - Supplies the name and parent directory of the new object.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    POBJECT_DIRECTORY Directory;

    //
    // Create the directory object.
    //

    status = ObCreateObject(&ObDirectoryObjectType, ObjectAttributes,
        sizeof(OBJECT_DIRECTORY), (PVOID*)&Directory);

    if (NT_SUCCESS(status)) {

        RtlZeroMemory(Directory, sizeof(OBJECT_DIRECTORY));

        status = ObInsertObject(Directory, ObjectAttributes, 0, DirectoryHandle);
    }

    return status;
}

NTSTATUS
NtOpenDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
/*++

Routine Description:

    This routine opens an existing directory object with the supplied
    attributes.

Arguments:

    DirectoryHandle - Supplies the location to receive the opened handle.

    ObjectAttributes - Supplies the name and parent directory of the desired
        object.

Return Value:

    Status of operation.

--*/
{
    return ObOpenObjectByName(ObjectAttributes, &ObDirectoryObjectType, NULL,
        DirectoryHandle);
}

NTSTATUS
NtQueryDirectoryObject(
    IN HANDLE DirectoryHandle,
    OUT PVOID Buffer,
    IN ULONG Length,
    IN BOOLEAN RestartScan,
    IN OUT PULONG Context,
    OUT PULONG ReturnedLength OPTIONAL
    )
/*++

Routine Description:

    This routine returns the next directory entry from the supplied object
    directory.

Arguments:

    DirectoryHandle - Supplies the handle of the object directory.

    Buffer - Supplies the buffer to receive the information.

    Length - Supplies the length of the above buffer.

    RestartScan - Supplies whether or not the scan should restart.

    Context - Supplies data that controls the current position of the
        enumeration.

    ReturnedLength - Supplies the location to receive the number of bytes copied
        to the buffer or the number of bytes required if the buffer is too
        small.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    POBJECT_DIRECTORY_INFORMATION DirectoryInformation;
    POBJECT_DIRECTORY Directory;
    ULONG BytesRequired;
    ULONG DesiredIndex;
    ULONG CurrentIndex;
    KIRQL OldIrql;
    ULONG HashBucket;
    POBJECT_HEADER_NAME_INFO ObjectHeaderNameInfo;
    ULONG NameBytesToCopy;
    POSTR ReturnedObjectName;

    //
    // Verify that we have enough space to write out at least the header.
    //

    DirectoryInformation = (POBJECT_DIRECTORY_INFORMATION)Buffer;

    if (Length < sizeof(OBJECT_DIRECTORY_INFORMATION)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Reference the directory object.
    //

    status = ObReferenceObjectByHandle(DirectoryHandle, &ObDirectoryObjectType,
        (PVOID*)&Directory);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Assume that we won't find any more entries.
    //

    status = STATUS_NO_MORE_ENTRIES;
    BytesRequired = 0;

    //
    // Figure out which entry we want to find.
    //

    DesiredIndex = RestartScan ? 0 : *Context;
    CurrentIndex = 0;

    //
    // Loop over all of the hash buckets until we find our desired index.
    //

    ObpAcquireObjectManagerLock(&OldIrql);

    for (HashBucket = 0; HashBucket < OB_NUMBER_HASH_BUCKETS; HashBucket++) {

        ObjectHeaderNameInfo = Directory->HashBuckets[HashBucket];

        while (ObjectHeaderNameInfo != NULL) {

            if (CurrentIndex == DesiredIndex) {

                //
                // Compute the number of bytes required to copy the string and
                // the number of bytes that we can actually copy.
                //

                NameBytesToCopy = ObjectHeaderNameInfo->Name.Length;
                BytesRequired = sizeof(OBJECT_DIRECTORY_INFORMATION) +
                    NameBytesToCopy;

                if (BytesRequired <= Length) {
                    status = STATUS_SUCCESS;
                } else {
                    NameBytesToCopy = Length - sizeof(OBJECT_DIRECTORY_INFORMATION);
                    status = STATUS_BUFFER_TOO_SMALL;
                }

                ReturnedObjectName = (POCHAR)(DirectoryInformation + 1);

                DirectoryInformation->Type =
                    OBJECT_HEADER_NAME_INFO_TO_OBJECT_HEADER(ObjectHeaderNameInfo)->Type->PoolTag;
                DirectoryInformation->Name.Buffer = ReturnedObjectName;
                DirectoryInformation->Name.Length =
                    ObjectHeaderNameInfo->Name.Length;
                DirectoryInformation->Name.MaximumLength =
                    ObjectHeaderNameInfo->Name.Length;

                RtlCopyMemory(ReturnedObjectName,
                    ObjectHeaderNameInfo->Name.Buffer, NameBytesToCopy);

                //
                // Bump up the index number for the next iteration.
                //

                *Context = DesiredIndex + 1;

                goto FoundDesiredIndex;
            }

            ObjectHeaderNameInfo = ObjectHeaderNameInfo->ChainLink;
            CurrentIndex++;
        }
    }

FoundDesiredIndex:
    ObpReleaseObjectManagerLock(OldIrql);

    //
    // Dereference the object and return the number of bytes that we copied or
    // that are required.
    //

    ObDereferenceObject(Directory);

    if (ReturnedLength != NULL) {
        *ReturnedLength = BytesRequired;
    }

    return status;
}

ULONG
FASTCALL
ObpComputeHashIndex(
    IN POBJECT_STRING ElementName
    )
/*++

Routine Description:

    This routine computes the hash index for the supplied name.

Arguments:

    ElementName - Supplies the name to compute the hash value for.

Return Value:

    The hash index.

--*/
{
    ULONG HashIndex;
    PUCHAR Buffer;
    PUCHAR BufferEnd;
    UCHAR Char;

    HashIndex = 0;
    Buffer = (PUCHAR)ElementName->Buffer;
    BufferEnd = Buffer + ElementName->Length;

    while (Buffer < BufferEnd) {

        Char = *Buffer++;

        //
        // Don't allow extended characters to change the hash value since we're
        // going to be doing case insensitive comparisions.
        //

        if (Char >= 0x80) {
            continue;
        }

        //
        // Force any upper case characters to be lower case.
        //

        Char |= 0x20;

        HashIndex += (HashIndex << 1) + (HashIndex >> 1) + Char;
    }

    return HashIndex % OB_NUMBER_HASH_BUCKETS;
}

BOOLEAN
ObpLookupElementNameInDirectory(
    IN POBJECT_DIRECTORY Directory,
    IN POBJECT_STRING ElementName,
    IN BOOLEAN ResolveSymbolicLink,
    OUT PVOID *ReturnedObject
    )
/*++

Routine Description:

    This routine searches the object directory for the supplied object name.

Arguments:

    Directory - Supplies the object directory to search.

    ElementName - Supplies the name of the object to find.

    ResolveSymbolicLink - Supplies TRUE if the returned object should be the
        target of a symbolic link

    ReturnedObject - Supplies the location to receive the object if found.

Return Value:

    Returns TRUE if the element name was found, else FALSE.

--*/
{
    OCHAR DriveLetter;
    PVOID Object;
    ULONG HashIndex;
    POBJECT_HEADER_NAME_INFO ObjectHeaderNameInfo;

    ObpAssertObjectManagerLock();

    //
    // If we're looking up a drive letter relative to the DOS devices directory
    // and we're allowed to resolve symbolic links, then do a quick array lookup
    // instead of walking the hash table.
    //

    if ((Directory == ObpDosDevicesDirectoryObject) &&
        ResolveSymbolicLink &&
        (ElementName->Length == sizeof(OCHAR) * 2) &&
        (ElementName->Buffer[1] == (OCHAR)':')) {

        DriveLetter = ElementName->Buffer[0];

        if (DriveLetter >= 'a' && DriveLetter <= 'z') {
            Object = ObpDosDevicesDriveLetterMap[DriveLetter - 'a'];
        } else if (DriveLetter >= 'A' && DriveLetter <= 'Z') {
            Object = ObpDosDevicesDriveLetterMap[DriveLetter - 'A'];
        } else {
            Object = NULL;
        }

        if (Object != NULL) {
            *ReturnedObject = Object;
            return TRUE;
        }
    }

    //
    // Compute the hash index for the element name.
    //

    HashIndex = ObpComputeHashIndex(ElementName);

    //
    // Walk through the object's on this hash chain.
    //

    ObjectHeaderNameInfo = Directory->HashBuckets[HashIndex];

    while (ObjectHeaderNameInfo != NULL) {

        if (RtlEqualObjectString(&ObjectHeaderNameInfo->Name, ElementName, TRUE)) {

            Object = OBJECT_HEADER_NAME_INFO_TO_OBJECT(ObjectHeaderNameInfo);

            //
            // Resolve the symbolic link if requested to.
            //

            if (ResolveSymbolicLink &&
                (OBJECT_TO_OBJECT_HEADER(Object)->Type == &ObSymbolicLinkObjectType)) {
                Object = ((POBJECT_SYMBOLIC_LINK)Object)->LinkTargetObject;
            }

            *ReturnedObject = Object;
            return TRUE;
        }

        ObjectHeaderNameInfo = ObjectHeaderNameInfo->ChainLink;
    }

    *ReturnedObject = NULL;
    return FALSE;
}

NTSTATUS
ObpReferenceObjectByName(
    IN HANDLE RootDirectoryHandle,
    IN POBJECT_STRING ObjectName,
    IN ULONG Attributes,
    IN POBJECT_TYPE ObjectType,
    IN OUT PVOID ParseContext OPTIONAL,
    OUT PVOID *ReturnedObject
    )
/*++

Routine Description:

    This routine references the object with the supplied name and attributes.

Arguments:

    RootDirectoryHandle - Supplies the handle to the root directory to begin
        searching from.

    ObjectName - Supplies the name of the object to reference.

    Attributes - Supplies the attributes of the desired object.

    ObjectType - Optionally supplies the desired type for the returned object.

    ParseContext - Supplies state used while parsing the object's attributes.

    ReturnedObject - Supplies the location to receive the object.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    KIRQL OldIrql;
    OBJECT_STRING RemainingName;
    OBJECT_STRING ElementName;
    BOOLEAN ResolveSymbolicLink;
    PVOID FoundObject;
    POBJECT_HEADER ObjectHeader;
    POBJECT_DIRECTORY Directory;
    PVOID ParsedObject;

    *ReturnedObject = NULL;

    ObpAcquireObjectManagerLock(&OldIrql);

    if (ObjectName != NULL) {
        RemainingName = *ObjectName;
    } else {
        RtlZeroMemory(&RemainingName, sizeof(OBJECT_STRING));
    }

    ResolveSymbolicLink = TRUE;

    //
    // Determine whether we parse the object name from an absolute or
    // relative root directory.
    //

    if (RootDirectoryHandle != NULL) {

        if (RootDirectoryHandle == ObDosDevicesDirectory()) {

            //
            // Reference the root of the DOS devices space.
            //

            FoundObject = ObpDosDevicesDirectoryObject;

        } else if (RootDirectoryHandle == ObWin32NamedObjectsDirectory()) {

            //
            // Reference the root of the Win32 named objects space.
            //

            FoundObject = ObpWin32NamedObjectsDirectoryObject;

        } else {

            //
            // Reference the root directory handle.
            //

            FoundObject = (POBJECT_DIRECTORY)ObpGetObjectHandleContents(RootDirectoryHandle);

            if (FoundObject == NULL) {
                status = STATUS_INVALID_HANDLE;
                goto CleanupAndExit;
            }
        }

        //
        // Verify that this is not an absolute path.
        //

        if ((RemainingName.Length != 0) &&
            (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR)) {
            status = STATUS_OBJECT_NAME_INVALID;
            goto CleanupAndExit;
        }

        goto OpenRootDirectory;
    }

    //
    // Verify that this is an absolute path.
    //

    if ((RemainingName.Length == 0) ||
        (RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR)) {
        status = STATUS_OBJECT_NAME_INVALID;
        goto CleanupAndExit;
    }

    //
    // Reference the global root directory handle.
    //

    FoundObject = ObpRootDirectoryObject;

    //
    // Check if we're supposed to open the root directory.
    //

    if (RemainingName.Length == sizeof(OCHAR)) {

        //
        // Advance past the backslash.
        //

        RemainingName.Buffer++;
        RemainingName.Length = 0;
        RemainingName.MaximumLength = 0;

        goto OpenRootDirectory;
    }

    //
    // Process the object name.
    //

    for (;;) {

        Directory = (POBJECT_DIRECTORY)FoundObject;

        ASSERT(OBJECT_TO_OBJECT_HEADER(Directory)->Type == &ObDirectoryObjectType);

        ObDissectName(RemainingName, &ElementName, &RemainingName);

        if (RemainingName.Length != 0) {

            //
            // Verify that there aren't multiple backslashes in the name.
            //

            if (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
                status = STATUS_OBJECT_NAME_INVALID;
                goto CleanupAndExit;
            }

        } else {

            //
            // If the caller is trying to access a symbolic link object, then
            // don't resolve symbolic links for the last element.
            //

            if (ObjectType == &ObSymbolicLinkObjectType) {
                ResolveSymbolicLink = FALSE;
            }
        }

        //
        // Search for the object in the directory and return an error if it
        // doesn't already exist.
        //

        if (!ObpLookupElementNameInDirectory(Directory, &ElementName,
            ResolveSymbolicLink, &FoundObject)) {
            status = (RemainingName.Length != 0) ?
                STATUS_OBJECT_PATH_NOT_FOUND : STATUS_OBJECT_NAME_NOT_FOUND;
            goto CleanupAndExit;
        }

OpenRootDirectory:
        ObjectHeader = OBJECT_TO_OBJECT_HEADER(FoundObject);

        //
        // If we've consumed the entire path, then we found the object, so
        // return success.
        //

        if (RemainingName.Length == 0) {

            //
            // If the object has a parse procedure, then we need to invoke it
            // since it may want to return a different object (such as when we
            // open a device object, we really want to return a file object).
            //

            if (ObjectHeader->Type->ParseProcedure != NULL) {
                goto InvokeParseProcedure;
            }

            //
            // Verify that the found object matches the requested object type.
            //

            if ((ObjectType != NULL) && (ObjectType != ObjectHeader->Type)) {
                status = STATUS_OBJECT_TYPE_MISMATCH;
                goto CleanupAndExit;
            }

            ObjectHeader->PointerCount++;

            *ReturnedObject = FoundObject;

            status = STATUS_SUCCESS;
            goto CleanupAndExit;
        }

        //
        // Only continue parsing in this loop if we found a directory object.
        //

        if (ObjectHeader->Type != &ObDirectoryObjectType) {

            //
            // If the object has a parse procedure, use it to evaluate the
            // remainder of the string.  If not, then we can't go any further.
            //

            if (ObjectHeader->Type->ParseProcedure == NULL) {
                status = STATUS_OBJECT_PATH_NOT_FOUND;
                goto CleanupAndExit;
            }

            //
            // Make sure the object stays alive after we drop the object manager
            // lock.
            //

InvokeParseProcedure:
            ObjectHeader->PointerCount++;

            ObpReleaseObjectManagerLock(OldIrql);

            //
            // If this isn't a file object that we're parsing, then back the
            // remaining name up to the path separator.  If we do have a file
            // object, then it must be from a symbolic link to a directory.  For
            // a directory, we want it to appear that we're doing a relative
            // lookup, so we don't want the leading backslash.
            //

            if (ObjectHeader->Type != &IoFileObjectType &&
                (RemainingName.Buffer > ObjectName->Buffer)) {

                RemainingName.Buffer--;
                RemainingName.Length += sizeof(OCHAR);
                RemainingName.MaximumLength = RemainingName.Length;

                ASSERT(RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR);
            }

            //
            // Invoke the object's parse procedure.
            //

            ParsedObject = NULL;

            status = ObjectHeader->Type->ParseProcedure(FoundObject, ObjectType,
                Attributes, ObjectName, &RemainingName, ParseContext,
                &ParsedObject);

            ObDereferenceObject(FoundObject);

            //
            // Verify that the parsed object matches the requested object type.
            //

            if (NT_SUCCESS(status)) {

                ASSERT(ParsedObject != NULL);

                if ((ObjectType == NULL) ||
                    (ObjectType == OBJECT_TO_OBJECT_HEADER(ParsedObject)->Type)) {

                    *ReturnedObject = ParsedObject;
                    status = STATUS_SUCCESS;

                } else {

                    ObDereferenceObject(ParsedObject);
                    status = STATUS_OBJECT_TYPE_MISMATCH;
                }
            }

            return status;
        }
    }

CleanupAndExit:
    ObpReleaseObjectManagerLock(OldIrql);

    return status;
}
