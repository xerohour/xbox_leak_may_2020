/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    mutntobj.c

Abstract:

    This module implements the kernel mutant object. Functions are
    provided to initialize, read, and release mutant objects.

    N.B. Kernel mutex objects have been subsumed by mutant objects.

Author:

    David N. Cutler (davec) 16-Oct-1989

Environment:

    Kernel mode only.

Revision History:

--*/

#include "ki.h"

//
// The following assert macro is used to check that an input mutant is
// really a kmutant and not something else, like deallocated pool.
//

#define ASSERT_MUTANT(E) {                    \
    ASSERT((E)->Header.Type == MutantObject); \
}

VOID
KeInitializeMutant (
    IN PRKMUTANT Mutant,
    IN BOOLEAN InitialOwner
    )

/*++

Routine Description:

    This function initializes a kernel mutant object.

Arguments:

    Mutant - Supplies a pointer to a dispatcher object of type mutant.

    InitialOwner - Supplies a boolean value that determines whether the
        current thread is to be the initial owner of the mutant object.

Return Value:

    None.

--*/

{

    PLIST_ENTRY ListEntry;
    KIRQL OldIrql;
    PRKTHREAD Thread;

    //
    // Initialize standard dispatcher object header, set the owner thread to
    // NULL, set the abandoned state to FALSE, and set the APC disable count
    // to zero (this is the only thing that distinguishes a mutex from a mutant).
    //

    Mutant->Header.Type = MutantObject;
    Mutant->Header.Size = sizeof(KMUTANT) / sizeof(LONG);
    if (InitialOwner == TRUE) {
        Thread = KeGetCurrentThread();
        Mutant->Header.SignalState = 0;
        Mutant->OwnerThread = Thread;
        KiLockDispatcherDatabase(&OldIrql);
        ListEntry = Thread->MutantListHead.Blink;
        InsertHeadList(ListEntry, &Mutant->MutantListEntry);
        KiUnlockDispatcherDatabase(OldIrql);

    } else {
        Mutant->Header.SignalState = 1;
        Mutant->OwnerThread = (PKTHREAD)NULL;
    }

    InitializeListHead(&Mutant->Header.WaitListHead);
    Mutant->Abandoned = FALSE;
    return;
}

LONG
KeReleaseMutant (
    IN PRKMUTANT Mutant,
    IN KPRIORITY Increment,
    IN BOOLEAN Abandoned,
    IN BOOLEAN Wait
    )

/*++

Routine Description:

    This function releases a mutant object by incrementing the mutant
    count. If the resultant value is one, then an attempt is made to
    satisfy as many Waits as possible. The previous signal state of
    the mutant is returned as the function value. If the Abandoned
    parameter is TRUE, then the mutant object is released by settings
    the signal state to one.

Arguments:

    Mutant - Supplies a pointer to a dispatcher object of type mutant.

    Increment - Supplies the priority increment that is to be applied
        if setting the event causes a Wait to be satisfied.

    Abandoned - Supplies a boolean value that signifies whether the
        mutant object is being abandoned.

    Wait - Supplies a boolean value that signifies whether the call to
        KeReleaseMutant will be immediately followed by a call to one
        of the kernel Wait functions.

Return Value:

    The previous signal state of the mutant object.

--*/

{

    KIRQL OldIrql;
    LONG OldState;
    PRKTHREAD Thread;


    ASSERT_MUTANT(Mutant);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the current signal state of the mutant object.
    //

    OldState = Mutant->Header.SignalState;

    //
    // If the Abandoned parameter is TRUE, then force the release of the
    // mutant object by setting its ownership count to one and setting its
    // abandoned state to TRUE. Otherwise increment mutant ownership count.
    // If the result count is one, then remove the mutant object from the
    // thread's owned mutant list, set the owner thread to NULL, and attempt
    // to satisfy a Wait for the mutant object if the mutant object wait
    // list is not empty.
    //

    Thread = KeGetCurrentThread();
    if (Abandoned != FALSE) {
        Mutant->Header.SignalState = 1;
        Mutant->Abandoned = TRUE;

    } else {

        //
        // If the Mutant object is not owned by the current thread, then
        // unlock the dispatcher data base and raise an exception. Otherwise
        // increment the ownership count.
        //

        if (Mutant->OwnerThread != Thread) {
            KiUnlockDispatcherDatabase(OldIrql);
            ExRaiseStatus(Mutant->Abandoned ?
                          STATUS_ABANDONED : STATUS_MUTANT_NOT_OWNED);
        }

        Mutant->Header.SignalState += 1;
    }

    if (Mutant->Header.SignalState == 1) {
        if (OldState <= 0) {
            RemoveEntryList(&Mutant->MutantListEntry);
        }

        Mutant->OwnerThread = (PKTHREAD)NULL;
        if (IsListEmpty(&Mutant->Header.WaitListHead) == FALSE) {
            KiWaitTest(Mutant, Increment);
        }
    }

    //
    // If the value of the Wait argument is TRUE, then return to
    // caller with IRQL raised and the dispatcher database locked.
    // Else release the dispatcher database lock and lower IRQL to
    // its previous value.
    //

    if (Wait != FALSE) {
        Thread->WaitNext = Wait;
        Thread->WaitIrql = OldIrql;

    } else {
        KiUnlockDispatcherDatabase(OldIrql);
    }

    //
    // Return previous signal state of mutant object.
    //

    return OldState;
}
