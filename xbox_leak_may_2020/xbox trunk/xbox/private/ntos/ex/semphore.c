/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    semphore.c

Abstract:

    This module implements the executive semaphore object. Functions are
    provided to create, open, release, and query semaphore objects.

--*/

#include "exp.h"

//
// Semaphore object type information.
//

DECLSPEC_RDATA OBJECT_TYPE ExSemaphoreObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    NULL,
    NULL,
    (PVOID)FIELD_OFFSET(KSEMAPHORE, Header),
    'ameS'
};

NTSTATUS
NtCreateSemaphore (
    IN PHANDLE SemaphoreHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN LONG InitialCount,
    IN LONG MaximumCount
    )

/*++

Routine Description:

    This function creates a semaphore object, sets its initial count to the
    specified value, sets its maximum count to the specified value, and opens
    a handle to the object with the specified desired access.

Arguments:

    SemaphoreHandle - Supplies a pointer to a variable that will receive the
        semaphore object handle.

    ObjectAttributes - Supplies a pointer to an object attributes structure.

    InitialCount - Supplies the initial count of the semaphore object.

    MaximumCount - Supplies the maximum count of the semaphore object.

Return Value:

    TBS

--*/

{

    PVOID Semaphore;
    NTSTATUS Status;

    //
    // Check argument validity.
    //

    if ((MaximumCount <= 0) || (InitialCount < 0) ||
       (InitialCount > MaximumCount)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Allocate semaphore object.
    //

    Status = ObCreateObject(&ExSemaphoreObjectType,
                            ObjectAttributes,
                            sizeof(KSEMAPHORE),
                            (PVOID *)&Semaphore);

    //
    // If the semaphore object was successfully allocated, then initialize
    // the semaphore object and attempt to insert the semaphore object in
    // the current process' handle table.
    //

    if (NT_SUCCESS(Status)) {
        KeInitializeSemaphore((PKSEMAPHORE)Semaphore,
                              InitialCount,
                              MaximumCount);

        Status = ObInsertObject(Semaphore,
                                ObjectAttributes,
                                0,
                                SemaphoreHandle);
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtQuerySemaphore (
    IN HANDLE SemaphoreHandle,
    OUT PSEMAPHORE_BASIC_INFORMATION SemaphoreInformation
    )

/*++

Routine Description:

    This function queries the state of a semaphore object and returns the
    requested information in the specified record structure.

Arguments:

    SemaphoreHandle - Supplies a handle to a semaphore object.

    SemaphoreInformation - Supplies a pointer to a record that is to receive
        the requested information.

Return Value:

    TBS

--*/

{

    PVOID Semaphore;
    LONG Count;
    LONG Maximum;
    NTSTATUS Status;

    //
    // Reference semaphore object by handle.
    //

    Status = ObReferenceObjectByHandle(SemaphoreHandle,
                                       &ExSemaphoreObjectType,
                                       &Semaphore);

    //
    // If the reference was successful, then read the current state and
    // maximum count of the semaphore object, dereference semaphore object,
    // fill in the information structure, and return the length of the
    // information structure if specified. If the write of the semaphore
    // information or the return length fails, then do not report an error.
    // When the caller accesses the information structure or length an
    // access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        Count = KeReadStateSemaphore((PKSEMAPHORE)Semaphore);
        Maximum = ((PKSEMAPHORE)Semaphore)->Limit;
        ObDereferenceObject(Semaphore);

        SemaphoreInformation->CurrentCount = Count;
        SemaphoreInformation->MaximumCount = Maximum;
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtReleaseSemaphore (
    IN HANDLE SemaphoreHandle,
    IN LONG ReleaseCount,
    OUT PLONG PreviousCount OPTIONAL
    )

/*++

Routine Description:

    This function releases a semaphore object by adding the specified release
    count to the current value.

Arguments:

    Semaphore - Supplies a handle to a semaphore object.

    ReleaseCount - Supplies the release count that is to be added to the
        current semaphore count.

    PreviousCount - Supplies an optional pointer to a variable that will
        receive the previous semaphore count.

Return Value:

    TBS

--*/

{

    LONG Count;
    PVOID Semaphore;
    NTSTATUS Status;

    //
    // Check argument validity.
    //

    if (ReleaseCount <= 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Reference semaphore object by handle.
    //

    Status = ObReferenceObjectByHandle(SemaphoreHandle,
                                       &ExSemaphoreObjectType,
                                       &Semaphore);

    //
    // If the reference was successful, then release the semaphore object.
    // If an exception occurs because the maximum count of the semaphore
    // has been exceeded, then dereference the semaphore object and return
    // the exception code as the service status. Otherwise write the previous
    // count value if specified. If the write of the previous count fails,
    // then do not report an error. When the caller attempts to access the
    // previous count value, an access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        try {
            Count = KeReleaseSemaphore((PKSEMAPHORE)Semaphore,
                                       SEMAPHORE_INCREMENT,
                                       ReleaseCount,
                                       FALSE);
            ObDereferenceObject(Semaphore);
            if (ARGUMENT_PRESENT(PreviousCount)) {
                *PreviousCount = Count;
            }
        } except(EXCEPTION_EXECUTE_HANDLER) {
            ObDereferenceObject(Semaphore);
            return GetExceptionCode();
        }
    }

    //
    // Return service status.
    //

    return Status;
}
