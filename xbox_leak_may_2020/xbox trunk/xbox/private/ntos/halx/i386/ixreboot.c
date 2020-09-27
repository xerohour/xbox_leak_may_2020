/*++

Copyright (c) 1991-2001  Microsoft Corporation

Module Name:

    ixreboot.c

Abstract:

    Provides the interface to the firmware for x86.  Since there is no
    firmware to speak of on x86, this is just reboot support.

--*/

#include "halp.h"

VOID
IdexChannelPrepareToQuickRebootSystem(
    VOID
    );

#define RESET_CONTROL_REGISTER          0xCF9

#define RESET_CONTROL_FULL_RESET        0x08
#define RESET_CONTROL_RESET_CPU         0x04
#define RESET_CONTROL_SYSTEM_RESET      0x02

//
// Linked list of routines to invoke on shutdown.
//
INITIALIZED_LIST_ENTRY(HalpShutdownRegistrationList);

//
// Private function prototypes
//

DECLSPEC_NORETURN
VOID
HalpReboot (
    VOID
    )
/*++

Routine Description:

    This procedure resets the system.

    N.B.

        Will NOT return.

--*/
{
    //
    // If we can safely use the SMBus helpers, then have the SMC do a full
    // reset.
    //

    if (KeGetCurrentIrql() < DISPATCH_LEVEL) {
        HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_RESET, SMC_RESET_ASSERT_RESET);
    }

    //
    // Poke the south bridge to do a full PCI reset.
    //

    _outp(RESET_CONTROL_REGISTER, RESET_CONTROL_FULL_RESET |
        RESET_CONTROL_RESET_CPU | RESET_CONTROL_SYSTEM_RESET);

    HalHaltSystem();
}

VOID
HalHaltSystem (
    VOID
    )
/*++

Routine Description:

    This procedure is called when the machine has crashed and is to be halted.

    N.B.

        Will NOT return.

--*/
{
    for (;;) {
        __asm {
            cli
            hlt
        }
    }
}

VOID
HalReturnToFirmware(
    IN FIRMWARE_REENTRY Routine
    )
/*++

Routine Description:

    Returns control to the firmware routine specified.  Since the x86 has
    no useful firmware, it just stops the system.

Arguments:

    Routine - Supplies a value indicating which firmware routine to invoke.

Return Value:

    Does not return.

--*/
{
    KIRQL OldIrql;
    PLIST_ENTRY ListEntry;
    PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration;

    if (Routine != HalQuickRebootRoutine) {

        //
        // If we're asked to reboot in order to display the univeral error
        // message, then set a flag in the SMC scratch register so that we
        // immediately display the message when the kernel boots again.
        //

        if (Routine == HalFatalErrorRebootRoutine) {
            HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_SCRATCH,
                SMC_SCRATCH_DISPLAY_FATAL_ERROR);
        }

#ifdef DEVKIT
        //
        // Notify the debugger that we're shutting down, but only if this wasn't
        // initiated by the kernel debugger.  The kernel debugger has already
        // detached from the kernel and making the following call only causes
        // the debugger to temporarily connect and disconnect again.
        //

        if (Routine != HalKdRebootRoutine) {
            DbgUnLoadImageSymbols(NULL, (PVOID)-1, 0);
        }
#endif

        //
        // Never returns.
        //

        HalpReboot();
    }

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    //
    // Call out to any registered shutdown routines.
    //

    for (;;) {

        //
        // Synchronize access to the registration list by raising to
        // DISPATCH_LEVEL.
        //

        OldIrql = KeRaiseIrqlToDpcLevel();

        ListEntry = RemoveHeadList(&HalpShutdownRegistrationList);

        KeLowerIrql(OldIrql);

        //
        // If there are no more entries, break out.
        //

        if (ListEntry == &HalpShutdownRegistrationList) {
            break;
        }

        //
        // Invoke the notification procedure.
        //

        ShutdownRegistration = CONTAINING_RECORD(ListEntry,
            HAL_SHUTDOWN_REGISTRATION, ListEntry);
        ShutdownRegistration->NotificationRoutine(ShutdownRegistration);
    }

#if DBG
    //
    // Notify the IDE driver that we're about to quick reboot the system.  The
    // driver will verify that no requests are pending and that all further
    // requests will assert.
    //

    IdexChannelPrepareToQuickRebootSystem();
#endif

#ifdef DEVKIT
    //
    // When running with the kernel debugger, it can take a number of cycles for
    // the debugger to shutdown and to restart in the next instance of the
    // kernel.  When ejecting the tray from DVD video playback, the dashboard
    // will quick reboot into another instance of the dashboard.  Because the
    // system is blocked most of this time inside the debugger, the system can't
    // handle the "tray open" interrupt and the system reboots.  To workaround
    // this problem, we'll block until the tray eject is complete.
    //

    if (KdDebuggerEnabled && !KdDebuggerNotPresent) {
        KeWaitForSingleObject(&HalpTrayEjectCompleteEvent, Executive, KernelMode,
            FALSE, NULL);
    }

    //
    // Notify the debugger that we're quick rebooting.
    //

    DbgUnLoadImageSymbols(NULL, (PVOID)-1, 0);

    //
    // Delete all breakpoints so that we don't hit any lingering breakpoints in
    // the next instance of the kernel.
    //

    KdDeleteAllBreakpoints();
#endif

    //
    // Never returns.
    //

    KeQuickRebootSystem();
}

VOID
HalRegisterShutdownNotification(
    IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration,
    IN BOOLEAN Register
    )
/*++

Routine Description:

    This routine registers or unregisters the supplied record with the list of
    routines that should be called when the system is rebooting.

Arguments:

    ShutdownRegistration - Specifies the record to register or unregister.

    Register - Specifies whether or not to register or unregister the record.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;
    PLIST_ENTRY ListEntry;

    //
    // Guard access to the shutdown registration lists by raising to
    // DISPATCH_LEVEL.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    if (Register) {

        //
        // Insert the registration record into the list in order of the
        // specified priority.
        //

        ListEntry = HalpShutdownRegistrationList.Flink;

        while (ListEntry != &HalpShutdownRegistrationList) {

            if (ShutdownRegistration->Priority > CONTAINING_RECORD(ListEntry,
                HAL_SHUTDOWN_REGISTRATION, ListEntry)->Priority) {
                InsertTailList(ListEntry, &ShutdownRegistration->ListEntry);
                break;
            }

            ListEntry = ListEntry->Flink;
        }

        if (ListEntry == &HalpShutdownRegistrationList) {
            InsertTailList(ListEntry, &ShutdownRegistration->ListEntry);
        }

    } else {

        //
        // Carefully remove the registration record by making sure that it's
        // already in the list.  If somebody else already started shutting down
        // the system, then the registration may have already been removed.
        //

        ListEntry = HalpShutdownRegistrationList.Flink;

        while (ListEntry != &HalpShutdownRegistrationList) {

            if (ShutdownRegistration == CONTAINING_RECORD(ListEntry,
                HAL_SHUTDOWN_REGISTRATION, ListEntry)) {
                RemoveEntryList(&ShutdownRegistration->ListEntry);
                break;
            }

            ListEntry = ListEntry->Flink;
        }
    }

    KeLowerIrql(OldIrql);
}
