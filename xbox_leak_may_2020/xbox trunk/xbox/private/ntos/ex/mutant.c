/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    mutant.c

Abstract:

    This module implements the executive mutant object. Functions are
    provided to create, open, release, and query mutant objects.

--*/

#include "exp.h"

//
// Define private function prototypes.
//

VOID
ExpDeleteMutant (
    IN PVOID Mutant
    );

//
// Mutant object type information.
//

DECLSPEC_RDATA OBJECT_TYPE ExMutantObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    ExpDeleteMutant,
    NULL,
    (PVOID)FIELD_OFFSET(KMUTANT, Header),
    'atuM'
};

VOID
ExpDeleteMutant (
    IN PVOID Mutant
    )

/*++

Routine Description:

    This function is called when an executive mutant object is about to
    be deleted. The mutant object is released with an abandoned status to
    ensure that it is removed from the owner thread's mutant list if the
    mutant object is currently owned by a thread.

Arguments:

    Mutant - Supplies a pointer to an executive mutant object.

Return Value:

    None.

--*/

{

    //
    // Release the mutant object with an abandoned status to ensure that it
    // is removed from the owner thread's mutant list if the mutant is
    // currently owned by a thread.
    //

    KeReleaseMutant((PKMUTANT)Mutant, MUTANT_INCREMENT, TRUE, FALSE);
    return;
}

NTSTATUS
NtCreateMutant (
    OUT PHANDLE MutantHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN BOOLEAN InitialOwner
    )

/*++

Routine Description:

    This function creates a mutant object, sets its initial count to one
    (signaled), and opens a handle to the object with the specified desired
    access.

Arguments:

    MutantHandle - Supplies a pointer to a variable that will receive the
        mutant object handle.

    ObjectAttributes - Supplies a pointer to an object attributes structure.

    InitialOwner - Supplies a boolean value that determines whether the
        creator of the object desires immediate ownership of the object.

Return Value:

    TBS

--*/

{

    PVOID Mutant;
    NTSTATUS Status;

    //
    // Allocate mutant object.
    //

    Status = ObCreateObject(&ExMutantObjectType,
                            ObjectAttributes,
                            sizeof(KMUTANT),
                            (PVOID *)&Mutant);

    //
    // If the mutant object was successfully allocated, then initialize
    // the mutant object and attempt to insert the mutant object in the
    // current process' handle table.
    //

    if (NT_SUCCESS(Status)) {
        KeInitializeMutant((PKMUTANT)Mutant, InitialOwner);
        Status = ObInsertObject(Mutant,
                                ObjectAttributes,
                                0,
                                MutantHandle);
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtQueryMutant (
    IN HANDLE MutantHandle,
    OUT PMUTANT_BASIC_INFORMATION MutantInformation
    )

/*++

Routine Description:

    This function queries the state of a mutant object and returns the
    requested information in the specified record structure.

Arguments:

    MutantHandle - Supplies a handle to a mutant object.

    MutantInformation - Supplies a pointer to a record that is to receive
        the requested information.

Return Value:

    TBS

--*/

{

    BOOLEAN Abandoned;
    BOOLEAN OwnedByCaller;
    LONG Count;
    PVOID Mutant;
    NTSTATUS Status;

    //
    // Reference mutant object by handle.
    //

    Status = ObReferenceObjectByHandle(MutantHandle,
                                       &ExMutantObjectType,
                                       &Mutant);

    //
    // If the reference was successful, then read the current state and
    // abandoned status of the mutant object, dereference mutant object,
    // fill in the information structure, and return the length of the
    // information structure if specified. If the write of the mutant
    // information or the return length fails, then do not report an error.
    // When the caller accesses the information structure or length an
    // access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        Count = KeReadStateMutant((PKMUTANT)Mutant);
        Abandoned = ((PKMUTANT)Mutant)->Abandoned;
        OwnedByCaller = (BOOLEAN)((((PKMUTANT)Mutant)->OwnerThread ==
                                                     KeGetCurrentThread()));

        ObDereferenceObject(Mutant);

        MutantInformation->CurrentCount = Count;
        MutantInformation->OwnedByCaller = OwnedByCaller;
        MutantInformation->AbandonedState = Abandoned;
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtReleaseMutant (
    IN HANDLE MutantHandle,
    OUT PLONG PreviousCount OPTIONAL
    )

/*++

Routine Description:

    This function releases a mutant object.

Arguments:

    Mutant - Supplies a handle to a mutant object.

    PreviousCount - Supplies an optional pointer to a variable that will
        receive the previous mutant count.

Return Value:

    TBS

--*/

{

    LONG Count;
    PVOID Mutant;
    NTSTATUS Status;

    //
    // Reference mutant object by handle.
    //
    // Note that the desired access is specified as zero since only the
    // owner can release a mutant object.
    //

    Status = ObReferenceObjectByHandle(MutantHandle,
                                       &ExMutantObjectType,
                                       &Mutant);

    //
    // If the reference was successful, then release the mutant object. If
    // an exception occurs because the caller is not the owner of the mutant
    // object, then dereference mutant object and return the exception code
    // as the service status. Otherise write the previous count value if
    // specified. If the write of the previous count fails, then do not
    // report an error. When the caller attempts to access the previous
    // count value, an access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        try {
            Count = KeReleaseMutant((PKMUTANT)Mutant, MUTANT_INCREMENT, FALSE, FALSE);
            ObDereferenceObject(Mutant);
            if (ARGUMENT_PRESENT(PreviousCount)) {
                *PreviousCount = Count;
            }
        } except(EXCEPTION_EXECUTE_HANDLER) {
            ObDereferenceObject(Mutant);
            return GetExceptionCode();
        }
    }

    //
    // Return service status.
    //

    return Status;
}
