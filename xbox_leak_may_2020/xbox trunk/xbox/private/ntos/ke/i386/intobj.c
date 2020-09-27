/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    intobj.c

Abstract:

    This module implements the kernel interrupt object. Functions are provided
    to initialize, connect, and disconnect interrupt objects.

--*/

#include "ki.h"

//
//  Externs from trap.asm used to compute and set handlers for unexpected
//  hardware interrupts.
//

extern  ULONG   KiStartUnexpectedRange(VOID);
extern  ULONG   KiEndUnexpectedRange(VOID);
extern  ULONG   KiUnexpectedEntrySize;

VOID
KeInitializeInterrupt (
    IN PKINTERRUPT Interrupt,
    IN PKSERVICE_ROUTINE ServiceRoutine,
    IN PVOID ServiceContext,
    IN ULONG Vector,
    IN KIRQL Irql,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN ShareVector
    )
/*++

Routine Description:

    This function initializes a kernel interrupt object. The service routine,
    service context, spin lock, vector, IRQL, SynchronizeIrql, and floating
    context save flag are initialized.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

    ServiceRoutine - Supplies a pointer to a function that is to be
        executed when an interrupt occurs via the specified interrupt
        vector.

    ServiceContext - Supplies a pointer to an arbitrary data structure which is
        to be passed to the function specified by the ServiceRoutine parameter.

    Vector - Supplies the index of the entry in the Interrupt Dispatch Table
        that is to be associated with the ServiceRoutine function.

    Irql - Supplies the request priority of the interrupting source.

    InterruptMode - Supplies the mode of the interrupt; LevelSensitive or

    ShareVector - Supplies a boolean value that specifies whether the
        vector can be shared with other interrupt objects or not.  If FALSE
        then the vector may not be shared, if TRUE it may be.
        Latched.

Return Value:

    None.

--*/
{
    PULONG pl;
    PULONG NormalDispatchCode;
    ULONG InterruptDispatch;

    ASSERT(Irql <= HIGH_LEVEL);
    ASSERT(Vector >= PRIMARY_VECTOR_BASE && Vector < PRIMARY_VECTOR_BASE + 16);

    //
    // Initialize the address of the service routine,
    // the service context, the address of the spin lock, the vector
    // number, the IRQL of the interrupting source, the Irql used for
    // synchronize execution, abd the interrupt mode.
    //

    Interrupt->ServiceRoutine = ServiceRoutine;
    Interrupt->ServiceContext = ServiceContext;

    Interrupt->BusInterruptLevel = Vector - PRIMARY_VECTOR_BASE;
    Interrupt->Irql = (UCHAR)Irql;
    Interrupt->Mode = InterruptMode;

    //
    // Copy the interrupt dispatch code template into the interrupt object
    // and edit the machine code stored in the structure (please see
    // _KiInterruptTemplate in intsup.asm.)
    //

    NormalDispatchCode = &(Interrupt->DispatchCode[0]);

    RtlCopyMemory(NormalDispatchCode, KiInterruptTemplate,
        NORMAL_DISPATCH_LENGTH * sizeof(ULONG));

    //
    // Fill in the address of the interrupt object.
    //

    pl = (PULONG)((PUCHAR)NormalDispatchCode + ((PUCHAR)&KiInterruptTemplateObject -
                                (PUCHAR)KiInterruptTemplate) -4);
    *pl = (ULONG)Interrupt;

    //
    // Fill in the address of interrupt dispatch code.
    //

    pl = (PULONG)((PUCHAR)NormalDispatchCode +
                ((PUCHAR)&KiInterruptTemplateDispatch -
                 (PUCHAR)KiInterruptTemplate) -4);

    if (InterruptMode == LevelSensitive) {
        InterruptDispatch = (ULONG)KiLevelInterruptDispatch;
    } else {
        InterruptDispatch = (ULONG)KiInterruptDispatch;
    }

    *pl = InterruptDispatch-(ULONG)((PUCHAR)pl+4);

    //
    // Set the connected state of the interrupt object to FALSE.
    //

    Interrupt->Connected = FALSE;
}

BOOLEAN
KeConnectInterrupt (
    IN PKINTERRUPT Interrupt
    )
/*++

Routine Description:

    This function connects an interrupt object to the interrupt vector
    specified by the interrupt object. If the interrupt object is already
    connected, or an attempt is made to connect to an interrupt that cannot
    be connected, then a value of FALSE is returned. Else the specified
    interrupt object is connected to the interrupt vector, the connected
    state is set to TRUE, and TRUE is returned as the function value.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

Return Value:

    If the interrupt object is already connected or an attempt is made to
    connect to an interrupt vector that cannot be connected, then a value
    of FALSE is returned. Else a value of TRUE is returned.

--*/
{
    KIRQL OldIrql;
    BOOLEAN Connected;
    ULONG BusInterruptLevel;

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Is interrupt object already connected?
    //

    Connected = FALSE;

    if (!Interrupt->Connected) {

        BusInterruptLevel = Interrupt->BusInterruptLevel;

        //
        // Is the IDT entry unclaimed?
        //

        if (KiReturnHandlerAddressFromIDT(BusInterruptLevel + PRIMARY_VECTOR_BASE) ==
            (((ULONG)&KiStartUnexpectedRange) +
            BusInterruptLevel * KiUnexpectedEntrySize)) {

            //
            // Connect the dispatch code in the interrupt object to the IDT
            // and enable the interrupt.
            //

            KiSetHandlerAddressToIDT(BusInterruptLevel + PRIMARY_VECTOR_BASE,
                (ULONG)&Interrupt->DispatchCode);

            HalEnableSystemInterrupt(BusInterruptLevel, Interrupt->Mode);

            Interrupt->Connected = TRUE;
            Connected = TRUE;
        }
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return whether interrupt was connected to the specified vector.
    //

    return Connected;
}

BOOLEAN
KeDisconnectInterrupt (
    IN PKINTERRUPT Interrupt
    )
/*++

Routine Description:

    This function disconnects an interrupt object from the interrupt vector
    specified by the interrupt object. If the interrupt object is not
    connected, then a value of FALSE is returned. Else the specified interrupt
    object is disconnected from the interrupt vector, the connected state is
    set to FALSE, and TRUE is returned as the function value.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

Return Value:

    If the interrupt object is not connected, then a value of FALSE is
    returned. Else a value of TRUE is returned.

--*/
{
    KIRQL OldIrql;
    BOOLEAN Connected;
    ULONG BusInterruptLevel;

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // If the interrupt object is connected, then disconnect it from the
    // specified vector.
    //

    Connected = Interrupt->Connected;

    if (Connected) {

        BusInterruptLevel = Interrupt->BusInterruptLevel;

        //
        // Disable the interrupt and connect the unexpected interrupt code to
        // the IDT.
        //

        HalDisableSystemInterrupt(BusInterruptLevel);

        KiSetHandlerAddressToIDT(BusInterruptLevel + PRIMARY_VECTOR_BASE,
            (((ULONG)&KiStartUnexpectedRange) +
            BusInterruptLevel * KiUnexpectedEntrySize));

        Interrupt->Connected = FALSE;
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return whether interrupt was disconnected from the specified vector.
    //

    return Connected;
}
