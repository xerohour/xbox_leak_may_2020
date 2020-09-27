/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    obref.c

Abstract:

    This module implements routines to reference and dereference objects.

--*/

#include "obp.h"

VOID
FASTCALL
ObfReferenceObject(
    IN PVOID Object
    )
/*++

Routine Description:

    This routine increments the reference count for the supplied object.

Arguments:

    Object - Supplies the object to reference.

Return Value:

    None.

--*/
{
    InterlockedIncrement(&OBJECT_TO_OBJECT_HEADER(Object)->PointerCount);
}

NTSTATUS
ObReferenceObjectByPointer(
    IN PVOID Object,
    IN POBJECT_TYPE ObjectType
    )
/*++

Routine Description:

    This routine increments the reference count for the supplied object after
    validating the object type.

Arguments:

    Object - Supplies the object to reference.

    ObjectType - Optionally supplies the desired type for the returned object.

Return Value:

    Status of operation.

--*/
{
    POBJECT_HEADER ObjectHeader;

    ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

    if (ObjectType == ObjectHeader->Type) {

        InterlockedIncrement(&ObjectHeader->PointerCount);

        return STATUS_SUCCESS;

    } else {
        return STATUS_OBJECT_TYPE_MISMATCH;
    }
}

NTSTATUS
ObOpenObjectByPointer(
    IN PVOID Object,
    IN POBJECT_TYPE ObjectType,
    OUT PHANDLE ReturnedHandle
    )
/*++

Routine Description:

    This routine creates a new handle for the supplied object after validating
    the object type.

Arguments:

    Object - Supplies the object to reference.

    ObjectType - Optionally supplies the desired type for the returned object.

    ReturnedHandle - Supplies the location to receive the object's handle.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    HANDLE Handle;
    KIRQL OldIrql;

    status = ObReferenceObjectByPointer(Object, ObjectType);

    if (NT_SUCCESS(status)) {

        ObpAcquireObjectManagerLock(&OldIrql);

        Handle = ObpCreateObjectHandle(Object);

        ObpReleaseObjectManagerLock(OldIrql);

        if (Handle != NULL) {
            status = STATUS_SUCCESS;
        } else {
            ObDereferenceObject(Object);
            status = STATUS_INSUFFICIENT_RESOURCES;
        }

    } else {
        Handle = NULL;
    }

    *ReturnedHandle = Handle;

    return status;
}

NTSTATUS
ObReferenceObjectByHandle(
    IN HANDLE Handle,
    IN POBJECT_TYPE ObjectType OPTIONAL,
    OUT PVOID *ReturnedObject
    )
/*++

Routine Description:

    This routine returns the object referenced by the supplied handle and
    optionally validates the object type.

Arguments:

    Handle - Supplies the handle to reference.

    ObjectType - Optionally supplies the desired type for the returned object.

    ReturnedObject - Supplies the pointer to receive the returned object.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID Object;
    POBJECT_HEADER ObjectHeader;

    //
    // Check for the static current process or thread handles.
    //

    if (Handle == NtCurrentThread()) {

        if ((ObjectType == &PsThreadObjectType) || (ObjectType == NULL)) {

            Object = PsGetCurrentThread();
            ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
            InterlockedIncrement(&ObjectHeader->PointerCount);

            *ReturnedObject = Object;

            return STATUS_SUCCESS;

        } else {
            status = STATUS_OBJECT_TYPE_MISMATCH;
        }

    } else {

        //
        // The handle is not one of the static handles, so reference the object
        // from the handle and validate the type.
        //

        Object = ObpGetObjectHandleReference(Handle);

        if (Object != NULL) {

            ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

            if ((ObjectType == ObjectHeader->Type) || (ObjectType == NULL)) {

                *ReturnedObject = Object;

                return STATUS_SUCCESS;

            } else {
                ObDereferenceObject(Object);
                status = STATUS_OBJECT_TYPE_MISMATCH;
            }

        } else {
            status = STATUS_INVALID_HANDLE;
        }
    }

    *ReturnedObject = NULL;

    return status;
}

NTSTATUS
ObReferenceObjectByName(
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

    ObjectName - Supplies the name of the object to reference.

    Attributes - Supplies the attributes of the desired object.

    ObjectType - Optionally supplies the desired type for the returned object.

    ParseContext - Supplies state used while parsing the object's attributes.

    ReturnedObject - Supplies the location to receive the object.

Return Value:

    Status of operation.

--*/
{
    return ObpReferenceObjectByName(NULL, ObjectName, Attributes, ObjectType,
        ParseContext, ReturnedObject);
}

NTSTATUS
ObOpenObjectByName(
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN POBJECT_TYPE ObjectType,
    IN OUT PVOID ParseContext OPTIONAL,
    OUT PHANDLE ReturnedHandle
    )
/*++

Routine Description:

    This routine creates a new handle for the object with the supplied
    attributes after validating the object type.

Arguments:

    ObjectAttributes - Supplies the attributes of the desired object.

    ObjectType - Optionally supplies the desired type for the returned object.

    ParseContext - Supplies state used while parsing the object's attributes.

    ReturnedHandle - Supplies the location to receive the object's handle.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID Object;
    HANDLE Handle;
    KIRQL OldIrql;

    status = ObpReferenceObjectByName(ObjectAttributes->RootDirectory,
        ObjectAttributes->ObjectName, ObjectAttributes->Attributes, ObjectType,
        ParseContext, &Object);

    if (NT_SUCCESS(status)) {

        ObpAcquireObjectManagerLock(&OldIrql);

        Handle = ObpCreateObjectHandle(Object);

        ObpReleaseObjectManagerLock(OldIrql);

        if (Handle == NULL) {
            ObDereferenceObject(Object);
            status = STATUS_INSUFFICIENT_RESOURCES;
        }

    } else {
        Handle = NULL;
    }

    *ReturnedHandle = Handle;

    return status;
}

VOID
FASTCALL
ObfDereferenceObject(
    IN PVOID Object
    )
/*++

Routine Description:

    This routine decrements the reference count for the supplied object and
    destroys the object when the reference count reaches zero.

Arguments:

    Object - Supplies the object to reference.

Return Value:

    None.

--*/
{
    POBJECT_HEADER ObjectHeader;
    PVOID ObjectBase;

    ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

    if (InterlockedDecrement(&ObjectHeader->PointerCount) == 0) {

        ASSERT(ObjectHeader->HandleCount == 0);

        //
        // If the object has a delete procedure, then invoke it.
        //

        if (ObjectHeader->Type->DeleteProcedure != NULL) {
            ObjectHeader->Type->DeleteProcedure(Object);
        }

        //
        // Determine the allocation base for the object and free the memory.
        //

        if (ObpIsFlagSet(ObjectHeader->Flags, OB_FLAG_NAMED_OBJECT)) {
            ObjectBase = OBJECT_HEADER_TO_OBJECT_HEADER_NAME_INFO(ObjectHeader);
            ASSERT(((POBJECT_HEADER_NAME_INFO)ObjectBase)->Directory == NULL);
        } else {
            ObjectBase = ObjectHeader;
        }

        ObjectHeader->Type->FreeProcedure(ObjectBase);
    }
}
