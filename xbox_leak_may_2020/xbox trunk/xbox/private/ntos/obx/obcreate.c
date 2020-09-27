/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    obcreate.c

Abstract:

    This module implements routines to create and insert objects into the global
    handle table.

--*/

#include "obp.h"

NTSTATUS
ObCreateObject(
    IN POBJECT_TYPE ObjectType,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN ULONG ObjectBodySize,
    OUT PVOID *Object
    )
/*++

Routine Description:

    This routine creates an object with the supplied size and attributes.

Arguments:

    ObjectType - Supplies the type for the object.

    ObjectAttributes - Supplies the attributes for the object.

    ObjectBodySize - Supplies the number of bytes to allocate for the object's
        body.

    Object - Supplies the location to receive the created object.

Return Value:

    Status of operation.

--*/
{
    POBJECT_HEADER ObjectHeader;
    OBJECT_STRING RemainingName;
    OBJECT_STRING ElementName;
    POBJECT_HEADER_NAME_INFO ObjectNameInfo;

    *Object = NULL;

    //
    // Handle the trivial case of creating an unnamed object.
    //

    if (ObjectAttributes == NULL || ObjectAttributes->ObjectName == NULL) {

        ObjectHeader = (POBJECT_HEADER)ObjectType->AllocateProcedure(
            FIELD_OFFSET(OBJECT_HEADER, Body) + ObjectBodySize,
            ObjectType->PoolTag);

        if (ObjectHeader == NULL) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        ObjectHeader->PointerCount = 1;
        ObjectHeader->HandleCount = 0;
        ObjectHeader->Type = ObjectType;
        ObjectHeader->Flags = 0;

        *Object = &ObjectHeader->Body;

        return STATUS_SUCCESS;
    }

    //
    // The object is named.  Dissect the path to obtain the last element which
    // is the object's name.
    //

    RemainingName = *ObjectAttributes->ObjectName;
    ElementName.Buffer = NULL;
    ElementName.Length = 0;

    while (RemainingName.Length != 0) {

        ObDissectName(RemainingName, &ElementName, &RemainingName);

        //
        // Verify that there aren't multiple backslashes in the name.
        //

        if ((RemainingName.Length != 0) && (RemainingName.Buffer[0] ==
            OBJ_NAME_PATH_SEPARATOR)) {
            return STATUS_OBJECT_NAME_INVALID;
        }
    }

    //
    // There should be at least one character in the element name.
    //

    if (ElementName.Length == 0) {
        return STATUS_OBJECT_NAME_INVALID;
    }

    //
    // Allocate the object header, body, and name in one chunk.
    //

    ObjectBodySize = ALIGN_UP(ObjectBodySize, ULONG);

    ObjectNameInfo = (POBJECT_HEADER_NAME_INFO)ObjectType->AllocateProcedure(
        sizeof(OBJECT_HEADER_NAME_INFO) + FIELD_OFFSET(OBJECT_HEADER, Body) +
        ObjectBodySize + ElementName.Length, ObjectType->PoolTag);

    if (ObjectNameInfo == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Initialize the object.
    //

    ObjectHeader = (POBJECT_HEADER)(ObjectNameInfo + 1);

    ObjectNameInfo->ChainLink = NULL;
    ObjectNameInfo->Directory = NULL;
    ObjectNameInfo->Name.Buffer = (POSTR)((PUCHAR)&ObjectHeader->Body +
        ObjectBodySize);
    ObjectNameInfo->Name.Length = ElementName.Length;
    ObjectNameInfo->Name.MaximumLength = ElementName.Length;

    RtlCopyMemory(ObjectNameInfo->Name.Buffer, ElementName.Buffer,
        ElementName.Length);

    ObjectHeader->PointerCount = 1;
    ObjectHeader->HandleCount = 0;
    ObjectHeader->Type = ObjectType;
    ObjectHeader->Flags = OB_FLAG_NAMED_OBJECT;

    *Object = &ObjectHeader->Body;

    return STATUS_SUCCESS;
}

NTSTATUS
ObInsertObject(
    IN PVOID Object,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN ULONG ObjectPointerBias,
    OUT PHANDLE ReturnedHandle
    )
/*++

Routine Description:

    This routine inserts the supplied object into the handle table and
    optionally into an object directory.

Arguments:

    Object - Supplies the object to insert into the handle table.

    ObjectAttributes - Supplies the attributes for the object.

    ObjectPointerBias - Supplies the number of additional pointer references
        that should be applied to the object.

    ReturnedHandle - Supplies the location to receive the object's handle.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    HANDLE Handle;
    PVOID InsertObject;
    HANDLE RootDirectoryHandle;
    POBJECT_HEADER ObjectHeader;
    POBJECT_DIRECTORY Directory;
    KIRQL OldIrql;
    OBJECT_STRING RemainingName;
    OBJECT_STRING ElementName;
    PVOID FoundObject;
    POBJECT_HEADER_NAME_INFO ObjectHeaderNameInfo;
    ULONG HashIndex;
    PVOID DosDevicesObject;
    OCHAR DriveLetter;

    ObpAcquireObjectManagerLock(&OldIrql);

    Handle = NULL;
    InsertObject = Object;

    //
    // Check if the object is named.  If so, then we need to go find the
    // directory to attach the object to.
    //

    if (ObjectAttributes != NULL && ObjectAttributes->ObjectName != NULL) {

        RemainingName = *ObjectAttributes->ObjectName;

        //
        // ObCreateObject has already checked that the name has at least one
        // character in it and that there aren't multiple backslashes in the
        // name.
        //

        ASSERT(RemainingName.Length != 0);

        //
        // Determine whether we parse the object name from an absolute or
        // relative root directory.
        //

        RootDirectoryHandle = ObjectAttributes->RootDirectory;

        if (RootDirectoryHandle != NULL) {

            //
            // Verify that this is not an absolute path.
            //

            if (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
                status = STATUS_OBJECT_NAME_INVALID;
                goto CleanupAndExit;
            }

            if (RootDirectoryHandle == ObDosDevicesDirectory()) {

                //
                // Reference the root of the DOS devices space.
                //

                Directory = ObpDosDevicesDirectoryObject;

            } else if (RootDirectoryHandle == ObWin32NamedObjectsDirectory()) {

                //
                // Reference the root of the Win32 named objects space.
                //

                Directory = ObpWin32NamedObjectsDirectoryObject;

            } else {

                //
                // Reference the root directory handle.
                //

                Directory = (POBJECT_DIRECTORY)
                    ObpGetObjectHandleContents(RootDirectoryHandle);

                if (Directory == NULL) {
                    status = STATUS_INVALID_HANDLE;
                    goto CleanupAndExit;
                }

                if (OBJECT_TO_OBJECT_HEADER(Directory)->Type != &ObDirectoryObjectType) {
                    status = STATUS_OBJECT_TYPE_MISMATCH;
                    goto CleanupAndExit;
                }
            }

        } else {

            //
            // Verify that this is an absolute path.
            //

            if (RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR) {
                status = STATUS_OBJECT_NAME_INVALID;
                goto CleanupAndExit;
            }

            //
            // Reference the global root directory handle.
            //

            Directory = ObpRootDirectoryObject;
        }

        for (;;) {

            ObDissectName(RemainingName, &ElementName, &RemainingName);

            if (ObpLookupElementNameInDirectory(Directory, &ElementName, TRUE,
                &FoundObject)) {

                //
                // If we found the element and there's no more of a path to
                // process, then we can't create the object due to a name
                // collision.
                //

                if (RemainingName.Length == 0) {

                    if (ObpIsFlagSet(ObjectAttributes->Attributes, OBJ_OPENIF)) {

                        if (OBJECT_TO_OBJECT_HEADER(FoundObject)->Type ==
                            OBJECT_TO_OBJECT_HEADER(Object)->Type) {

                            //
                            // We found an object with the same name and type.
                            // The caller wants us to open that object instead
                            // of the one we were given, so change the insertion
                            // object.
                            //
                            // Null out the directory pointer so that we don't
                            // attempt to reinsert this object into the
                            // directory.
                            //

                            InsertObject = FoundObject;
                            Directory = NULL;
                            break;

                        } else {
                            status = STATUS_OBJECT_TYPE_MISMATCH;
                            goto CleanupAndExit;
                        }

                    } else {
                        status = STATUS_OBJECT_NAME_COLLISION;
                        goto CleanupAndExit;
                    }
                }

                //
                // We found the element and there's more of a path to process.
                // The object we found had better be a directory.
                //

                if (OBJECT_TO_OBJECT_HEADER(FoundObject)->Type !=
                    &ObDirectoryObjectType) {
                    status = STATUS_OBJECT_PATH_NOT_FOUND;
                    goto CleanupAndExit;
                }

                Directory = (POBJECT_DIRECTORY)FoundObject;

            } else {

                //
                // If we failed to find the element and there's more of a path
                // name to process, then return path not found.
                //

                if (RemainingName.Length != 0) {
                    status = STATUS_OBJECT_PATH_NOT_FOUND;
                    goto CleanupAndExit;
                }

                //
                // Break out of the loop since we've consumed the entire path
                // and we didn't collide with an existing object's name.
                //

                break;
            }
        }

    } else {
        Directory = NULL;
    }

    //
    // Create the handle for the object.
    //

    Handle = ObpCreateObjectHandle(InsertObject);

    if (Handle == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto CleanupAndExit;
    }

    //
    // Apply the reference bias to the object, including one for the handle
    // itself.
    //

    ObjectHeader = OBJECT_TO_OBJECT_HEADER(InsertObject);
    ObjectHeader->PointerCount += ObjectPointerBias + 1;

    //
    // If the object is to be attached to an object directory, then do it now.
    //

    if (Directory != NULL) {

        ObjectHeaderNameInfo = OBJECT_TO_OBJECT_HEADER_NAME_INFO(Object);

        HashIndex = ObpComputeHashIndex(&ObjectHeaderNameInfo->Name);

        ObjectHeader->Flags |= OB_FLAG_ATTACHED_OBJECT;

        ObjectHeaderNameInfo->Directory = Directory;
        ObjectHeaderNameInfo->ChainLink = Directory->HashBuckets[HashIndex];
        Directory->HashBuckets[HashIndex] = ObjectHeaderNameInfo;

        //
        // If we're attaching a drive letter relative to the DOS devices
        // directory, then also add the object to the quick array lookup.  If
        // the object that we're attaching is a symbolic link object, then put
        // the resolved link target in the quick array-- the common case is to
        // lookup fully resolve paths.
        //

        if ((Directory == ObpDosDevicesDirectoryObject) &&
            (ObjectHeaderNameInfo->Name.Length == sizeof(OCHAR) * 2) &&
            (ObjectHeaderNameInfo->Name.Buffer[1] == (OCHAR)':')) {

            DosDevicesObject = Object;

            if (OBJECT_TO_OBJECT_HEADER(DosDevicesObject)->Type ==
                &ObSymbolicLinkObjectType) {
                DosDevicesObject =
                    ((POBJECT_SYMBOLIC_LINK)DosDevicesObject)->LinkTargetObject;
            }

            DriveLetter = ObjectHeaderNameInfo->Name.Buffer[0];

            if (DriveLetter >= 'a' && DriveLetter <= 'z') {
                ASSERT(ObpDosDevicesDriveLetterMap[DriveLetter - 'a'] == NULL);
                ObpDosDevicesDriveLetterMap[DriveLetter - 'a'] = DosDevicesObject;
            } else if (DriveLetter >= 'A' && DriveLetter <= 'Z') {
                ASSERT(ObpDosDevicesDriveLetterMap[DriveLetter - 'A'] == NULL);
                ObpDosDevicesDriveLetterMap[DriveLetter - 'A'] = DosDevicesObject;
            }
        }

        //
        // Increment the number of references to the object and the directory.
        // These references are undone in ObpDetachNamedObject.
        //

        OBJECT_TO_OBJECT_HEADER(Directory)->PointerCount++;
        ObjectHeader->PointerCount++;
    }

    //
    // Mark the object as permanent if requested.
    //

    if ((ObjectAttributes != NULL) &&
        ObpIsFlagSet(ObjectAttributes->Attributes, OBJ_PERMANENT)) {
        ObjectHeader->Flags |= OB_FLAG_PERMANENT_OBJECT;
    }

    status = (Object == InsertObject) ? STATUS_SUCCESS : STATUS_OBJECT_NAME_EXISTS;

CleanupAndExit:
    ObpReleaseObjectManagerLock(OldIrql);

    ObDereferenceObject(Object);

    *ReturnedHandle = Handle;

    return status;
}
