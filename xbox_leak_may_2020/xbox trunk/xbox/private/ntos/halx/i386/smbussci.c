/*++

Copyright (c) 2001-2002  Microsoft Corporation

Module Name:

    smbussci.c

Abstract:

    This module implements the routines to send and receive data over SMBus.

    This module implements the routines to handle a System Control Interrupt
    (SCI) from the ACPI logic block or external SMI.

    This module implements the routines to interface with the System Management
    Controller (SMC).

--*/

#include "halp.h"
#include "mcpxsmb.inc"
#include "mcpxacpi.inc"
#include <av.h>
#include <xconfig.h>
#include <ldr.h>

VOID
IdexCdRomTrayOpenNotification(
    VOID
    );

VOID
IdexDiskShutdownSystem(
    VOID
    );

//
// Local support.
//

BOOLEAN
HalpHandleNextSMCInterruptReason(
    NTSTATUS SMBusStatus
    );

VOID
HalpDpcForSMBusInterrupt(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

VOID
HalpDpcForSMBusRetry(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

VOID
HalpDpcForSystemControlInterrupt(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

//
// Define the default number of retries allowed for a SMBus transaction.  Each
// retry is tried a millisecond apart.
//

#define HAL_DEFAULT_SMBUS_RETRIES           20

//
// Structure used to return information and signal completion of a SMBus
// transaction.
//

typedef struct _SMBUS_STATUS_BLOCK {
    NTSTATUS Status;
    ULONG DataValue;
    KEVENT CompletionEvent;
} SMBUS_STATUS_BLOCK, *PSMBUS_STATUS_BLOCK;

//
// Function prototype for an SMBus completion routine for SMC interrupt
// handling.
//

typedef
BOOLEAN
(*PSMBUS_COMPLETION_ROUTINE)(
    NTSTATUS SMBusStatus
    );

//
// Guards access to the SMBus.
//
INITIALIZED_KEVENT(HalpSMBusLock, SynchronizationEvent, TRUE);

//
// DPC object used to complete handling of an SMBus interrupt at DISPATCH_LEVEL.
//
INITIALIZED_KDPC(HalpSMBusInterruptDpc, HalpDpcForSMBusInterrupt, NULL);

//
// DPC object used to retry an SMBus transaction.
//
INITIALIZED_KDPC(HalpSMBusRetryDpc, HalpDpcForSMBusRetry, NULL);

//
// Timer object used to retry an SMBus transaction.
//
INITIALIZED_KTIMER(HalpSMBusRetryTimer, SynchronizationTimer);

//
// DPC object used to complete handling of an System Control Interrupt at
// DISPATCH_LEVEL.
//
INITIALIZED_KDPC(HalpSystemControlInterruptDpc, HalpDpcForSystemControlInterrupt, NULL);

//
// Stores the number of retries remaining for a SMBus transaction.
//
ULONG HalpSMBusRetries;

//
// Stores TRUE if a SMC interrupt was received while the SMBus was owned by a
// PASSIVE_LEVEL owner or the SMC interrupt state machine is already busy.
//
BOOLEAN HalpSMBusSMCInterruptPending;

//
// Stores the interrupt reason flags for the SMC interrupt in progress.
//
UCHAR HalpSMCInterruptReason;

//
// Points at the current SMBus status block.
//
PSMBUS_STATUS_BLOCK HalpSMBusStatusBlock;

//
// Points at the DPC completion routine for the current SMBus transaction.
//
PSMBUS_COMPLETION_ROUTINE HalpSMBusCompletionRoutine;

//
// Stores the SMC command code and data value to execute when the kernel is
// ready to reset or shutdown the system.
//
UCHAR HalpSMCResetOrShutdownCommandCode;
ULONG HalpSMCResetOrShutdownDataValue;

//
// Cached read of the tray state from the SMC.  This is initialized to an
// invalid value so that we know to go read the tray state from the SMC.
//
ULONG HalpTrayState = MAXULONG;

//
// Stores the number of times that the tray state has changed, which can be
// useful for detecting missed tray state changes.
//
ULONG HalpTrayStateChangeCount;

//
// Stores whether or not a tray eject in secure mode forces a reboot of the
// system or not.  This is FALSE for cold boots before we've started to load an
// Xbox executable.
//
DECLSPEC_STICKY BOOLEAN HalpTrayEjectRequiresReboot;

//
// Stores TRUE if a tray eject has occurred during the boot process.
//
BOOLEAN HalpTrayEjectDuringBootOccurred;

#ifdef DEVKIT
//
// Notification event for when a tray eject is complete.
//
INITIALIZED_KEVENT(HalpTrayEjectCompleteEvent, NotificationEvent, TRUE);
#endif

//
// Stores TRUE if HalInitiateShutdown has been called while the SMBus lock was
// owned.
//
BOOLEAN HalpShutdownRequestPending;

//
// Stores the AV pack that the console was booted with.
//
DECLSPEC_STICKY ULONG HalBootSMCVideoMode = SMC_VIDEO_MODE_NONE;

//
// Stores the flags that should be stored in the scratch register in addition to
// SMC_SCRATCH_TRAY_EJECT_PENDING when a tray eject occurs.
//
ULONG HalpSMCScratchRegister = SMC_SCRATCH_SHORT_ANIMATION;


//
// Grovel at private data in ntos\av.
//
extern ULONG AvpCapabilities;
extern ULONG AvpCurrentMode;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, HalpInitializeSystemControlInterrupt)
#pragma alloc_text(INIT, HalpProcessSMCInitialState)
#pragma alloc_text(INIT, HalBlockIfNoAVPack)
#endif

VOID
HalpKickoffReadSMBus(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN ReadWordValue
    )
/*++

Routine Description:

    This routine programs the SMBus controller for a "read byte" or "read word"
    transaction and starts the transaction.

Arguments:

    SlaveAddress - Specifies the slave address of the SMBus device to access.

    CommandCode - Specifies the command field to pass during as part of the host
        cycle.

    ReadWordValue - Specifies TRUE if 16-bits should be read, else FALSE if
        8-bits should be read.

Return Value:

    Status of operation.

--*/
{
    UCHAR ControlValue;

    //
    // Reset the number of retries for the SMBus transaction.
    //

    HalpSMBusRetries = HAL_DEFAULT_SMBUS_RETRIES;

    //
    // Setup the parameters for the SMBus transaction.
    //

    _outp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_ADDRESS_REGISTER,
        (UCHAR)(SlaveAddress | MCPX_ADDRESS_READ_CYCLE));
    _outp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_COMMAND_REGISTER,
        CommandCode);

    //
    // Clear the the status register.
    //

    _outpw(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_STATUS_REGISTER,
        _inpw(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_STATUS_REGISTER));

    //
    // Initiate the read transaction.
    //

    if (ReadWordValue) {
        ControlValue = MCPX_CONTROL_INTERRUPT_ENABLE | MCPX_CONTROL_HOST_START |
            MCPX_CONTROL_CYCLE_READ_WRITE_WORD;
    } else {
        ControlValue = MCPX_CONTROL_INTERRUPT_ENABLE | MCPX_CONTROL_HOST_START |
            MCPX_CONTROL_CYCLE_READ_WRITE_BYTE;
    }

    _outp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_CONTROL_REGISTER,
        ControlValue);
}

NTSTATUS
HalReadSMBusValue(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN ReadWordValue,
    OUT ULONG *DataValue
    )
/*++

Routine Description:

    This routine submits a "read byte" or "read word" transaction over SMBus.

Arguments:

    SlaveAddress - Specifies the slave address of the SMBus device to access.

    CommandCode - Specifies the command field to pass during as part of the host
        cycle.

    ReadWordValue - Specifies TRUE if 16-bits should be read, else FALSE if
        8-bits should be read.

    DataValue - Specifies the location of the buffer to receive the data read
        from the SMBus.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    SMBUS_STATUS_BLOCK SMBusStatusBlock;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    // Prevent thread suspension while we own the SMBus lock.
    //

    KeEnterCriticalRegion();

    //
    // Synchronize access to the SMBus by acquiring the SMBus lock.  Besides
    // other callers of this routine, we also need to synchronize with the SCI
    // handler: the SCI handler checks the SMBus lock at DPC level and either
    // takes ownership of the lock or else sets the HalpSMBusSMCInterruptPending
    // flag for delayed processing.
    //

    KeWaitForSingleObject(&HalpSMBusLock, Executive, KernelMode, FALSE, NULL);

    //
    // Initialize the completion event and set the global pointer to point at
    // our status block.
    //

    KeInitializeEvent(&SMBusStatusBlock.CompletionEvent, NotificationEvent, FALSE);

    HalpSMBusStatusBlock = &SMBusStatusBlock;

    //
    // Kickoff the SMBus read transaction.
    //

    HalpKickoffReadSMBus(SlaveAddress, CommandCode, ReadWordValue);

    //
    // Block for completion of the SMBus transaction and return the status.
    //

    KeWaitForSingleObject(&SMBusStatusBlock.CompletionEvent, Executive,
        KernelMode, FALSE, NULL);

    KeLeaveCriticalRegion();

    if (ReadWordValue) {
        *DataValue = SMBusStatusBlock.DataValue;
    } else {
        *DataValue = (UCHAR)SMBusStatusBlock.DataValue;
    }

    return SMBusStatusBlock.Status;
}

VOID
HalpKickoffWriteSMBus(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN WriteWordValue,
    IN ULONG DataValue
    )
/*++

Routine Description:

    This routine programs the SMBus controller for a "read byte" or "read word"
    transaction and starts the transaction.

Arguments:

    SlaveAddress - Specifies the slave address of the SMBus device to access.

    CommandCode - Specifies the command field to pass during as part of the host
        cycle.

    WriteWordValue - Specifies TRUE if 16-bits should be written, else FALSE
        if 8-bits should be written.

    DataValue - Specifies the data value to transmit over SMBus.

Return Value:

    None.

--*/
{
    UCHAR ControlValue;

    //
    // Reset the number of retries for the SMBus transaction.
    //

    HalpSMBusRetries = HAL_DEFAULT_SMBUS_RETRIES;

    //
    // Setup the parameters for the SMBus transaction.
    //

    _outp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_ADDRESS_REGISTER,
        (UCHAR)(SlaveAddress & ~MCPX_ADDRESS_READ_CYCLE));
    _outp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_COMMAND_REGISTER,
        CommandCode);
    _outpw(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_DATA_REGISTER,
        (USHORT)DataValue);

    //
    // Clear the the status register.
    //

    _outpw(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_STATUS_REGISTER,
        _inpw(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_STATUS_REGISTER));

    //
    // Initiate the write transaction.
    //

    if (WriteWordValue) {
        ControlValue = MCPX_CONTROL_INTERRUPT_ENABLE | MCPX_CONTROL_HOST_START |
            MCPX_CONTROL_CYCLE_READ_WRITE_WORD;
    } else {
        ControlValue = MCPX_CONTROL_INTERRUPT_ENABLE | MCPX_CONTROL_HOST_START |
            MCPX_CONTROL_CYCLE_READ_WRITE_BYTE;
    }

    _outp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_CONTROL_REGISTER,
        ControlValue);
}

NTSTATUS
HalWriteSMBusValue(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN WriteWordValue,
    IN ULONG DataValue
    )
/*++

Routine Description:

    This routine submits a "write byte" or "write word" transaction over SMBus.

Arguments:

    SlaveAddress - Specifies the slave address of the SMBus device to access.

    CommandCode - Specifies the command field to pass during as part of the host
        cycle.

    WriteWordValue - Specifies TRUE if 16-bits should be written, else FALSE
        if 8-bits should be written.

    DataValue - Specifies the data value to transmit over SMBus.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    SMBUS_STATUS_BLOCK SMBusStatusBlock;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    // Prevent thread suspension while we own the SMBus lock.
    //

    KeEnterCriticalRegion();

    //
    // Synchronize access to the SMBus by acquiring the SMBus lock.  Besides
    // other callers of this routine, we also need to synchronize with the SCI
    // handler: the SCI handler checks the SMBus lock at DPC level and either
    // takes ownership of the lock or else sets the HalpSMBusSMCInterruptPending
    // flag for delayed processing.
    //

    KeWaitForSingleObject(&HalpSMBusLock, Executive, KernelMode, FALSE, NULL);

    //
    // Initialize the completion event and set the global pointer to point at
    // our status block.
    //

    KeInitializeEvent(&SMBusStatusBlock.CompletionEvent, NotificationEvent, FALSE);

    HalpSMBusStatusBlock = &SMBusStatusBlock;

    //
    // Kickoff the SMBus write transaction.
    //

    HalpKickoffWriteSMBus(SlaveAddress, CommandCode, WriteWordValue, DataValue);

    //
    // Block for completion of the SMBus transaction and return the status.
    //

    KeWaitForSingleObject(&SMBusStatusBlock.CompletionEvent, Executive,
        KernelMode, FALSE, NULL);

    KeLeaveCriticalRegion();

    return SMBusStatusBlock.Status;
}

BOOLEAN
HalpWriteSMCResetOrShutdownComplete(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is invoked after sending the final reset or shutdown command
    to the SMC.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    //
    // If we failed to write out the reset or shutdown command, try repeating
    // the command.
    //

    if (!NT_SUCCESS(SMBusStatus)) {

        HalpSMBusCompletionRoutine = HalpWriteSMCResetOrShutdownComplete;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, HalpSMCResetOrShutdownCommandCode,
            FALSE, HalpSMCResetOrShutdownDataValue);

        return TRUE;
    }

    HalHaltSystem();

    return TRUE;
}

VOID
HalMustCompletePacketsFinished(
    VOID
    )
{
    ULONG DiskShutdownStartTickCount;

    ASSERT(IoPendingMustCompletePackets == 0);

    //
    // Check if the SMBus is busy.  If not, then we can attempt to shutdown now.
    // Otherwise, we'll wait for the SMBus completion routine to make the call.
    //

    if (HalpSMBusCompletionRoutine != NULL) {
        return;
    }

    //
    // If the command that we're sending is a shutdown command, then flush and
    // spindown the disk device.  This can take awhile, so pretend like we're
    // not in a DPC in order to avoid triggering the "DPC running too long"
    // debug code.
    //

    if (((HalpSMCResetOrShutdownCommandCode == SMC_COMMAND_OS_RESUME) &&
        (HalpSMCResetOrShutdownDataValue == SMC_OS_RESUME_SHUTDOWN_COMPLETE)) ||
        ((HalpSMCResetOrShutdownCommandCode == SMC_COMMAND_RESET) &&
        (HalpSMCResetOrShutdownDataValue == SMC_RESET_ASSERT_SHUTDOWN))) {

        KeGetCurrentPrcb()->DpcRoutineActive = FALSE;

        DiskShutdownStartTickCount = KeQueryTickCount();

        IdexDiskShutdownSystem();

        //
        // Older builds of the SMC have a synchronization bug where we can't
        // send a "shutdown in progress" notification followed shortly by a
        // "shutdown complete" notification: the SMC fails to see the second
        // notification and we take the full SMC timeout to finally shutdown.
        // To workaround the problem, we need to add a delay between the
        // notifications.  The above disk shutdown call will give us most of the
        // delay we need, but some drives respond to the shutdown commands
        // quickly enough that we need to spin here to close the window where
        // we're exposed to the SMC bug.
        //

        while ((KeQueryTickCount() - DiskShutdownStartTickCount) < 125);

    } else {

#ifdef DEVKIT
        //
        // Notify the debugger that we're resetting.
        //

        DbgUnLoadImageSymbols(NULL, (PVOID)-1, 0);
#endif
    }

    //
    // Write the reset or shutdown command to the SMC.
    //

    HalpSMBusCompletionRoutine = HalpWriteSMCResetOrShutdownComplete;

    HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, HalpSMCResetOrShutdownCommandCode,
        FALSE, HalpSMCResetOrShutdownDataValue);
}

BOOLEAN
HalpWriteSMCAudioClampComplete(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is called from the common reset and shutdown sequence after
    clamping the audio output.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    //
    // Clear the completion routine so that HalMustCompletePacketsFinished knows
    // that we aren't in the middle of an SMBus transaction.
    //

    HalpSMBusCompletionRoutine = NULL;

    //
    // Disable the video DACs using one of the GPIO pins.
    //

    _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xD3, 0x05);

    //
    // Now that we've disabled audio and video, check if there are any pending
    // must complete I/O request packets.  If not, then we can attempt to
    // shutdown now.  Otherwise, exit and wait for the I/O manager to call us
    // back.
    //

    if (IoPendingMustCompletePackets == 0) {
        HalMustCompletePacketsFinished();
    }

    //
    // Return TRUE so that we continue to own the SMBus even though we may not
    // have started a transaction.
    //

    return TRUE;
}

DECLSPEC_NORETURN
VOID
HalpCommonResetOrShutdown(
    VOID
    )
/*++

Routine Description:

    This routine is called to start the common reset and shutdown sequence.

Arguments:

    None.

Return Value:

    None.

--*/
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(KeIsExecutingDpc());

    //
    // There's at least one device that's busy processing I/O.  We're going to
    // need to block for that I/O to complete, but we want to disable audio and
    // video as quickly as possible.
    //
    // Disable the audio output by turning on the SMC's audio clamp.
    //

    HalpSMBusCompletionRoutine = HalpWriteSMCAudioClampComplete;

    HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_AUDIO_CLAMP, FALSE,
        SMC_AUDIO_CLAMP_CLAMP);

    //
    // Enter an infinite DPC processing loop to prevent any more title code from
    // running.  Note that we may be nested inside KeRetireDpcListLoop, but that
    // routine is reentrant and we aren't consuming much of the DPC stack.
    //

    KeRetireDpcListLoop();
}

BOOLEAN
HalpCommonResetOrShutdownComplete(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is invoked after an SMBus transaction that should be followed
    by a reset or shutdown.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    HalpCommonResetOrShutdown();

    return TRUE;
}

BOOLEAN
HalpReadSMCVideoModeComplete(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is invoked after the SMBus transaction to read the video mode
    from the SMC has completed.  This is called as a result of the SMC detecting
    that a new AV pack has been attached.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    ULONG VideoMode;

    //
    // If we failed to read the video mode from the SMC, then we'll leave the
    // video DACs disabled.  The user can replug in the AV pack to attempt to
    // retry this operation.
    //

    if (!NT_SUCCESS(SMBusStatus)) {
        return HalpHandleNextSMCInterruptReason(STATUS_SUCCESS);
    }

    //
    // Read the video mode data from the SMBus controller.
    //

    VideoMode = (UCHAR)_inp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_DATA_REGISTER);
    VideoMode = (VideoMode & SMC_VIDEO_MODE_VMODE_MASK);

    //
    // If no AV pack is attached, then treat this as a spurious interrupt.
    //

    if (VideoMode == SMC_VIDEO_MODE_NONE) {
        return HalpHandleNextSMCInterruptReason(SMBusStatus);
    }

    //
    // If the video mode doesn't match the video mode that the title was started
    // in, then reboot the system.
    //

    if (VideoMode != HalBootSMCVideoMode) {

        //
        // Setup the SMC command code and data value that should be sent to the
        // SMC when the shutdown is complete.
        //

        HalpSMCResetOrShutdownCommandCode = SMC_COMMAND_RESET;
        HalpSMCResetOrShutdownDataValue = SMC_RESET_ASSERT_RESET;

        HalpCommonResetOrShutdown();
    }

    //
    // If the capabilities haven't been set yet, then the system isn't using
    // video yet, so we don't need to do anything.
    //

    if (AvpCapabilities == 0) {
        return HalpHandleNextSMCInterruptReason(STATUS_SUCCESS);
    }

    //
    // The video mode matches the video mode that the title was started in.
    // Enable the video DACs.  If this fails for some unexpected reason, the
    // user can replug in the AV pack to attempt to retry this operation.
    //

    HalpSMBusCompletionRoutine = HalpHandleNextSMCInterruptReason;

    HalpKickoffWriteSMBus(TV_ENCODER_ID, 0xBA, FALSE,
        0x20 | ((AvpCurrentMode & 0x0F000000) >> 24));

    return TRUE;
}

BOOLEAN
HalpTrayEjectWriteSMCNonSecureComplete(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is invoked after the SMBus transaction to write that the system
    should move to the non-secure state after a tray eject event has occurred
    during a boot cycle.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    //
    // If we failed to switch to non-secure mode, then we can't eject the tray
    // without causing a reboot.  Ignore the user's press of the tray eject
    // button and make them hit it again in order to retry the state machine.
    //

    if (!NT_SUCCESS(SMBusStatus)) {
        return HalpHandleNextSMCInterruptReason(STATUS_SUCCESS);
    }

    //
    // Now that we've switch to non-secure mode, go ahead and eject the tray.
    //

    HalpSMBusCompletionRoutine = HalpHandleNextSMCInterruptReason;

    HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_DVD_TRAY_OPERATION,
        FALSE, SMC_DVD_TRAY_OPERATION_OPEN);

    return TRUE;
}

BOOLEAN
HalpTrayEjectWriteSMCResumeComplete(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is invoked after the SMBus transaction to write that the system
    should resume after a tray eject event has occurred.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    //
    // If we failed to write out the OS resume response, then all we can really
    // do is try the operation again.  The SMC will reboot us if it doesn't see
    // a response in a short period of time, so there's no point in trying to
    // advance the state machine.
    //

    if (!NT_SUCCESS(SMBusStatus)) {

        HalpSMBusCompletionRoutine = HalpTrayEjectWriteSMCResumeComplete;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_OS_RESUME,
            FALSE, SMC_OS_RESUME_RESUME);

        return TRUE;
    }

    //
    // Check if we're in non-secure mode where a tray open event doesn't force a
    // reboot (e.g., the dashboard scenerio).
    //
    // Also check if we're cold-booting and haven't reached the point where we
    // require a reboot in order to gracefully handle a tray eject.
    //

    if ((XboxBootFlags & XBOX_BOOTFLAG_NONSECUREMODE) != 0) {

        HalpSMBusCompletionRoutine = HalpHandleNextSMCInterruptReason;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_DVD_TRAY_OPERATION,
            FALSE, SMC_DVD_TRAY_OPERATION_OPEN);

    } else if (!HalpTrayEjectRequiresReboot) {

        ASSERT(!KeHasQuickBooted);

        //
        // This tray eject is happening asynchronously to the rest of the boot
        // process.  Other pieces of the kernel needed to know we've ejected the
        // tray and switched to non-secure mode.
        //

        HalpTrayEjectDuringBootOccurred = TRUE;

        //
        // Switch to non-secure mode so that we can eject the tray without
        // rebooting.
        //

        XboxBootFlags |= XBOX_BOOTFLAG_NONSECUREMODE | XBOX_BOOTFLAG_TRAYEJECT;

        HalpSMBusCompletionRoutine = HalpTrayEjectWriteSMCNonSecureComplete;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS,
            SMC_COMMAND_OVERRIDE_RESET_ON_TRAY_OPEN, FALSE,
            SMC_RESET_ON_TRAY_OPEN_NONSECURE_MODE);

    } else {

        //
        // Setup the SMC command code and data value that should be sent to the
        // SMC when the shutdown is complete.
        //

        HalpSMCResetOrShutdownCommandCode = SMC_COMMAND_RESET;
        HalpSMCResetOrShutdownDataValue = SMC_RESET_ASSERT_RESET;

        //
        // Write out to the scratch register that a tray eject is pending.
        //

        HalpSMBusCompletionRoutine = HalpCommonResetOrShutdownComplete;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_SCRATCH,
            FALSE, SMC_SCRATCH_TRAY_EJECT_PENDING | HalpSMCScratchRegister);
    }

    return TRUE;
}

BOOLEAN
HalpTrayOpenWriteSMCResumeComplete(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is invoked after the SMBus transaction to write that the system
    should resume after a tray open event has occurred.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    //
    // If we failed to write out the OS resume response, then all we can really
    // do is try the operation again.  The SMC will reboot us if it doesn't see
    // a response in a short period of time, so there's no point in trying to
    // advance the state machine.
    //

    if (!NT_SUCCESS(SMBusStatus)) {

        HalpSMBusCompletionRoutine = HalpTrayOpenWriteSMCResumeComplete;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_OS_RESUME,
            FALSE, SMC_OS_RESUME_RESUME);

        return TRUE;
    }

    //
    // If we got into this routine and we're not already set for non-secure
    // mode, then we must have received a tray open before we reached a point
    // where a tray eject requires a reboot of the system.  Switch into
    // non-secure mode which also forces a launch of the dashboard.
    //

    if ((XboxBootFlags & XBOX_BOOTFLAG_NONSECUREMODE) == 0) {

        ASSERT(!HalpTrayEjectRequiresReboot);

        //
        // This tray eject is happening asynchronously to the rest of the boot
        // process.  Other pieces of the kernel needed to know we've ejected the
        // tray and switched to non-secure mode.
        //

        HalpTrayEjectDuringBootOccurred = TRUE;

        //
        // Switch to non-secure mode which will also force us to boot the
        // dashboard.  Note that the SMC should be booting in non-secure mode
        // already, but explicitly setting the SMC to non-secure mode mimics
        // other control paths more closely.
        //

        XboxBootFlags |= XBOX_BOOTFLAG_NONSECUREMODE;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS,
            SMC_COMMAND_OVERRIDE_RESET_ON_TRAY_OPEN, FALSE,
            SMC_RESET_ON_TRAY_OPEN_NONSECURE_MODE);

        HalpSMBusCompletionRoutine = HalpHandleNextSMCInterruptReason;

        return TRUE;
    }

    //
    // Handle the next interrupt reason flag.
    //

    return HalpHandleNextSMCInterruptReason(STATUS_SUCCESS);
}

BOOLEAN
HalpHandleNextSMCInterruptReason(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is invoked to handle the next flag in the pending interrupt
    reason mask.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    //
    // Check if the user has hit the power button.
    //

    if ((HalpSMCInterruptReason & SMC_INTERRUPT_REASON_SHUTDOWN) != 0) {

        HalpSMCInterruptReason &= ~SMC_INTERRUPT_REASON_SHUTDOWN;

        //
        // Setup the SMC command code and data value that should be sent to the
        // SMC when the shutdown is complete.
        //

        HalpSMCResetOrShutdownCommandCode = SMC_COMMAND_OS_RESUME;
        HalpSMCResetOrShutdownDataValue = SMC_OS_RESUME_SHUTDOWN_COMPLETE;

        //
        // Notify the SMC that we're still alive and handling this interrupt by
        // telling it that we we're in the middle of shutting down the system.
        //

        HalpSMBusCompletionRoutine = HalpCommonResetOrShutdownComplete;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_OS_RESUME, FALSE,
            SMC_OS_RESUME_SHUTDOWN_IN_PROGRESS);

        //
        // Enter an infinite DPC processing loop to prevent any more title code
        // from running.
        //

        KeRetireDpcListLoop();
    }

    //
    // Check if an AV pack has been disconnected.
    //

    if ((HalpSMCInterruptReason & SMC_INTERRUPT_REASON_NO_AV_PACK) != 0) {

        HalpSMCInterruptReason &= ~SMC_INTERRUPT_REASON_NO_AV_PACK;

        //
        // If the console is configured to run manufacturing conent, then do
        // nothing.  Manufacturing test wants full control of the DACs.
        //

        if ((XboxGameRegion & XC_GAME_REGION_MANUFACTURING) == 0) {

            //
            // Disable the video DACs and return to this routine to handle the
            // next interrupt reason flag.
            //

            HalpSMBusCompletionRoutine = HalpHandleNextSMCInterruptReason;

            HalpKickoffWriteSMBus(TV_ENCODER_ID, 0xBA, FALSE, 0x3F);

            return TRUE;
        }
    }

    //
    // Check if an AV pack has been connected.
    //

    if ((HalpSMCInterruptReason & SMC_INTERRUPT_REASON_NEW_AV_PACK) != 0) {

        HalpSMCInterruptReason &= ~SMC_INTERRUPT_REASON_NEW_AV_PACK;

        //
        // If the console is configured to run manufacturing content, then do
        // nothing.  Manufacturing test wants full control of the DACs.
        //

        if ((XboxGameRegion & XC_GAME_REGION_MANUFACTURING) == 0) {

            //
            // Read the video mode from the SMC.
            //

            HalpSMBusCompletionRoutine = HalpReadSMCVideoModeComplete;

            HalpKickoffReadSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_VIDEO_MODE, FALSE);

            return TRUE;
        }
    }

    //
    // Check if the user has pressed the tray eject button.
    //

    if ((HalpSMCInterruptReason & SMC_INTERRUPT_REASON_TRAY_EJECT) != 0) {

        HalpSMCInterruptReason &= ~SMC_INTERRUPT_REASON_TRAY_EJECT;

#ifdef DEVKIT
        //
        // Indicate the a tray eject is in progress.
        //

        KeClearEvent(&HalpTrayEjectCompleteEvent);
#endif

        //
        // Increment the number of tray state changes and set the cached tray
        // state to the unloading state.  We do this before actually ejecting
        // the media so that the dashboard sees an immediate change as it's
        // polling the SMC tray state.
        //

        HalpTrayStateChangeCount++;
        HalpTrayState = SMC_TRAY_STATE_UNLOADING;

        //
        // Notify the SMC that we're still alive and handling this interrupt by
        // telling it that we want to resume.
        //

        HalpSMBusCompletionRoutine = HalpTrayEjectWriteSMCResumeComplete;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_OS_RESUME,
            FALSE, SMC_OS_RESUME_RESUME);

        return TRUE;
    }

    //
    // Check if the user has opened the tray.  Note that although we control the
    // tray eject button above, the user may attempt to force open the tray
    // through mechanical means.
    //

    if ((HalpSMCInterruptReason & SMC_INTERRUPT_REASON_TRAY_OPENED) != 0) {

        HalpSMCInterruptReason &= ~SMC_INTERRUPT_REASON_TRAY_OPENED;

#ifdef DEVKIT
        //
        // Indicate the a tray eject is complete.
        //

        KeSetEvent(&HalpTrayEjectCompleteEvent, EVENT_INCREMENT, FALSE);
#endif

        //
        // Increment the number of tray state changes and set the cached tray
        // state to an invalid state.
        //

        HalpTrayStateChangeCount++;
        HalpTrayState = MAXULONG;

        //
        // Notify the IDE driver that the tray has opened so that any state can
        // be reset.
        //

        IdexCdRomTrayOpenNotification();

        //
        // Reset the title identifier that the Xbox executable loader allows to
        // be loaded from media.
        //

        XeCdRomMediaTitleID = 0;

        //
        // Check if we're in non-secure mode where a tray open event doesn't
        // force a reboot (e.g., the dashboard scenerio).  If we see a tray open
        // event before we're at a point where a tray eject requires a reboot,
        // then pretend like this tray open is really a tray eject.
        //

        if (((XboxBootFlags & XBOX_BOOTFLAG_NONSECUREMODE) != 0) ||
            !HalpTrayEjectRequiresReboot) {

            //
            // Notify the SMC that we're still alive and handling this interrupt
            // by telling it that we want to resume.
            //

            HalpSMBusCompletionRoutine = HalpTrayOpenWriteSMCResumeComplete;

            HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_OS_RESUME,
                FALSE, SMC_OS_RESUME_RESUME);

            return TRUE;

        } else {

            //
            // Setup the SMC command code and data value that should be sent to
            // the SMC when the shutdown is complete.
            //

            HalpSMCResetOrShutdownCommandCode = SMC_COMMAND_OS_RESUME;
            HalpSMCResetOrShutdownDataValue = SMC_OS_RESUME_REBOOT;

            HalpCommonResetOrShutdown();
        }
    }

    //
    // Check if the user has closed the tray.
    //

    if ((HalpSMCInterruptReason & SMC_INTERRUPT_REASON_TRAY_CLOSED) != 0) {

        HalpSMCInterruptReason &= ~SMC_INTERRUPT_REASON_TRAY_CLOSED;

        //
        // Increment the number of tray state changes and set the cached tray
        // state to the closed state.
        //

        HalpTrayStateChangeCount++;
        HalpTrayState = SMC_TRAY_STATE_CLOSED;
    }

    //
    // Check if the drive has finished media detection.
    //

    if ((HalpSMCInterruptReason & SMC_INTERRUPT_REASON_TRAY_DETECTED) != 0) {

        HalpSMCInterruptReason &= ~SMC_INTERRUPT_REASON_TRAY_DETECTED;

        //
        // Increment the number of tray state changes and set the cached tray
        // state to an invalid state.  The SMC needs to be queried to see if
        // the drive has valid media or not.
        //

        HalpTrayStateChangeCount++;
        HalpTrayState = MAXULONG;
    }

    return FALSE;
}

BOOLEAN
HalpReadSMCInterruptReasonComplete(
    NTSTATUS SMBusStatus
    )
/*++

Routine Description:

    This routine is invoked after the SMBus transaction to read the interrupt
    reason from the SMC has completed.

Arguments:

    SMBusStatus - Specifies the status of the last SMBus transaction.

Return Value:

    Returns TRUE if the SMC interrupt handling state machine has more work to
    do, else FALSE.

--*/
{
    //
    // If we failed to read the interrupt reason from the SMC, then there's not
    // much we can do so bail out of the SMC interrupt handling.
    //

    if (!NT_SUCCESS(SMBusStatus)) {
        return FALSE;
    }

    //
    // Read the interrupt reason data from the SMBus controller.
    //

    HalpSMCInterruptReason =
        (UCHAR)_inp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_DATA_REGISTER);

    //
    // Start processing the next flag in the interrupt reason.
    //

    return HalpHandleNextSMCInterruptReason(SMBusStatus);
}

VOID
HalpReadSMCInterruptReason(
    VOID
    )
/*++

Routine Description:

    This routine is the starting point for the SMC interrupt handling state
    machine.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // Read the interrupt reason from the SMC.
    //

    HalpSMBusCompletionRoutine = HalpReadSMCInterruptReasonComplete;

    HalpKickoffReadSMBus(SMC_SLAVE_ADDRESS, SMC_COMMAND_INTERRUPT_REASON, FALSE);
}

VOID
HalpReleaseSMBusLock(
    VOID
    )
/*++

Routine Description:

    This routine releases the SMBus lock.  This routine must be called at
    DISPATCH_LEVEL.

Arguments:

    None.

Return Value:

    None.

--*/
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    if (HalpSMBusSMCInterruptPending) {

        //
        // The SMC has generated an interrupt but the SMBus was busy.  Enter the
        // SMC interrupt handling state machine.
        //

        HalpSMBusSMCInterruptPending = FALSE;

        HalpReadSMCInterruptReason();

    } else if (HalpShutdownRequestPending) {

        //
        // HalInitiateShutdown was called while the SMBus was busy.  Call that
        // routine now that the lock is available.
        //

        HalpShutdownRequestPending = FALSE;

        //
        // Setup the SMC command code and data value that should be sent to the SMC
        // when the shutdown is complete.
        //

        HalpSMCResetOrShutdownCommandCode = SMC_COMMAND_RESET;
        HalpSMCResetOrShutdownDataValue = SMC_RESET_ASSERT_SHUTDOWN;

        HalpCommonResetOrShutdown();

    } else {

        //
        // Release the SMBus lock on behalf of the SMBus service or the SMC
        // interrupt handling state machine.
        //

        KeSetEvent(&HalpSMBusLock, 0, FALSE);
    }
}

VOID
HalpDpcForSMBusInterrupt(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine is a DPC that is triggered when the interrupt service routine
    has work that must be executed at DISPATCH_LEVEL.

Arguments:

    Dpc - Specifies the finish DPC contained in the channel object.

    DeferredContext - Specifies the context associated with this DPC instance.

    SystemArgument1 - Specifies the first argument passed to KeInsertQueueDpc.

    SystemArgument2 - Specifies the second argument passed to KeInsertQueueDpc.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    ULONG StatusValue;
    LARGE_INTEGER TimerDueTime;

    StatusValue = PtrToUlong(SystemArgument1);

    //
    // Determine if there was a SMBus collision or protocol error.  If so and we
    // haven't exceeded our retry count, then delay and retry the transaction.
    //

    if (StatusValue & (MCPX_STATUS_COLLISION | MCPX_STATUS_PROTOCOL_ERROR)) {

        if (HalpSMBusRetries > 0) {

            //
            // Retry the transaction in two milliseconds.
            //

            TimerDueTime.QuadPart = -20000;

            KeSetTimer(&HalpSMBusRetryTimer, TimerDueTime, &HalpSMBusRetryDpc);

            HalpSMBusRetries--;

            return;
        }

        HalDbgPrint(("HAL: exceeded retry count for SMBus transaction.\n"));
    }

    //
    // Determine what status to return to the caller of the SMBus service.
    //

    if (StatusValue & MCPX_STATUS_HOST_CYCLE_COMPLETE) {
        status = STATUS_SUCCESS;
    } else if (StatusValue & MCPX_STATUS_TIMEOUT_ERROR) {
        HalDbgPrint(("HAL: SMBus transaction timed out.\n"));
        status = STATUS_IO_TIMEOUT;
    } else {
        HalDbgPrint(("HAL: SMBus transaction errored out.\n"));
        status = STATUS_IO_DEVICE_ERROR;
    }

    if (HalpSMBusCompletionRoutine != NULL) {

        //
        // Invoke the SMBus completion routine and bail out if the completion
        // routine indicates that there's still more work to do.
        //

        if (HalpSMBusCompletionRoutine(status)) {
            return;
        }

        HalpSMBusCompletionRoutine = NULL;

    } else if (HalpSMBusStatusBlock != NULL) {

        //
        // Return the status and the current value of the data register to the
        // SMBus service.
        //

        HalpSMBusStatusBlock->Status = status;
        HalpSMBusStatusBlock->DataValue =
            _inpw(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_DATA_REGISTER);

        //
        // Signal completion of the SMBus transaction.
        //

        KeSetEvent(&HalpSMBusStatusBlock->CompletionEvent, 0, FALSE);

        HalpSMBusStatusBlock = NULL;

    } else {
        HalDbgPrint(("HAL: Spurious SMBus interrupt.\n"));
    }

    //
    // Release the SMBus lock.
    //

    HalpReleaseSMBusLock();
}

VOID
HalpDpcForSMBusRetry(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine is a DPC that is triggered by the SMBus retry timer.

Arguments:

    Dpc - Specifies the finish DPC contained in the channel object.

    DeferredContext - Specifies the context associated with this DPC instance.

    SystemArgument1 - Specifies the first argument passed to KeInsertQueueDpc.

    SystemArgument2 - Specifies the second argument passed to KeInsertQueueDpc.

Return Value:

    None.

--*/
{
    //
    // The SMBus controller registers are already setup for the transaction;
    // the host start bit just needs to be set again.
    //

    _outp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_CONTROL_REGISTER,
        _inp(XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_CONTROL_REGISTER) |
        MCPX_CONTROL_INTERRUPT_ENABLE | MCPX_CONTROL_HOST_START);
}

VOID
HalpDpcForSystemControlInterrupt(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine is a DPC that is triggered when the interrupt service routine
    has work that must be executed at DISPATCH_LEVEL.

Arguments:

    Dpc - Specifies the finish DPC contained in the channel object.

    DeferredContext - Specifies the context associated with this DPC instance.

    SystemArgument1 - Specifies the first argument passed to KeInsertQueueDpc.

    SystemArgument2 - Specifies the second argument passed to KeInsertQueueDpc.

Return Value:

    None.

--*/
{
    //
    // Test if the SMBus lock is currently owned.  If so, set a flag so that
    // when the SMBus is available, we can take control.
    //

    if (!KeReadStateEvent(&HalpSMBusLock)) {
        HalpSMBusSMCInterruptPending = TRUE;
        return;
    }

    //
    // The SMBus lock is currently unowned.  Clear the event so that other
    // PASSIVE_LEVEL callers will block and so that this routine will not be
    // reentered.
    //

    KeClearEvent(&HalpSMBusLock);

    //
    // Enter the SMC interrupt handling state machine.
    //

    HalpReadSMCInterruptReason();
}

VOID
HalpInitializeSystemControlInterrupt(
    VOID
    )
/*++

Routine Description:

    This routine programs the ACPI management block to deliver all System
    Management Interrupts (SMIs) as a System Control Interrupts (SCIs) and
    enables all SCI sources that we're interested in using.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // Select SCI generation instead of SMI generation for ACPI events.
    //

    _outpw(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_PM1_CONTROL_REGISTER,
        _inpw(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_PM1_CONTROL_REGISTER) |
        PM1_SCI_ENABLE);

    //
    // Enable external SMIs (now routed as a system control interrupt).
    //

    _outpw(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_GPE0_ENABLE_REGISTER,
        _inpw(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_GPE0_ENABLE_REGISTER) |
        GPE0_EXTSMI_ENABLE);

    //
    // Enable the ACPI timer carry overflow interrupt.
    //

    _outpw(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_PM1_ENABLE_REGISTER,
        _inpw(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_PM1_ENABLE_REGISTER) |
        PM1_TIMER_ENABLE);

    //
    // Enable interrupts from the system management block.
    //

    _outpw(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_GLOBAL_SMI_CONTROL,
        _inpw(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_GLOBAL_SMI_CONTROL) |
        GPE0_SMI_ENABLE);
}

VOID
HalpProcessSMCInitialState(
    VOID
    )
/*++

Routine Description:

    This routine processes the initial state of the SMC at boot.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    ULONG VideoMode;
    BOOLEAN TrayEjectPending;
#ifdef DEVKIT
    ULONG Index;
    ULONG FirmwareRevision[3];
#endif
    ULONG InterruptReason;
    ULONG Scratch;

    TrayEjectPending = FALSE;

    //
    // Initialize the boot AV pack.
    //

    if (NT_SUCCESS(HalReadSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_VIDEO_MODE,
        &VideoMode))) {
        HalBootSMCVideoMode = (VideoMode & SMC_VIDEO_MODE_VMODE_MASK);
    }

#ifdef DEVKIT
    //
    // Read the SMC firmware revision and print it to the debugger.
    //

    for (Index = 0; Index < 3; Index++) {
        HalReadSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_FIRMWARE_REVISION,
            &FirmwareRevision[Index]);
    }

    HalDbgPrint(("HAL: SMC version %c%c%c\n", FirmwareRevision[0],
        FirmwareRevision[1], FirmwareRevision[2]));
#endif

    //
    // Read the interrupt reason to see if a DVD tray eject is pending, which
    // will happen if the user powers on the system by hitting the tray eject
    // button.
    //

    status = HalReadSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_INTERRUPT_REASON,
        &InterruptReason);

    if (NT_SUCCESS(status)) {

        if ((InterruptReason & SMC_INTERRUPT_REASON_TRAY_EJECT) != 0) {
            TrayEjectPending = TRUE;
        }

        //
        // If we happen to see a pending tray open interrupt, then tell the SMC
        // that we're resuming.  This should only happen when a DEVKIT kernel in
        // ROM loads a kernel from the hard disk after ejecting the tray.
        //
        // Also, a tray eject interrupt was pending at boot, then tell the SMC
        // that we're resuming so that the SMC knows that we're alive to handle
        // the interrupt.
        //

        if (((InterruptReason & SMC_INTERRUPT_REASON_TRAY_OPENED) != 0) ||
            ((InterruptReason & SMC_INTERRUPT_REASON_TRAY_EJECT) != 0)) {
            HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_OS_RESUME,
                SMC_OS_RESUME_RESUME);
        }
    }

    //
    // Read the our SMC scratch register to see if we're rebooting because the
    // user pressed the tray eject button.  The scratch register is cleared
    // across power cycles, but we need to clear it ourselves across cold
    // reboots.
    //

    status = HalReadSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_SCRATCH,
        &Scratch);

    if (NT_SUCCESS(status)) {

        if ((Scratch & SMC_SCRATCH_TRAY_EJECT_PENDING) != 0) {
            TrayEjectPending = TRUE;
        }
    }

    //
    // Always write out the following flags to the scratch register.  This has
    // the effect of clearing the tray eject request that we might have seen
    // above and also telling future cold boots of the kernel that only the
    // short animation needs to be displayed because the drives are already
    // spun up.
    //

    HalWriteSMCScratchRegister(SMC_SCRATCH_SHORT_ANIMATION);

    //
    // Propagate flags from the SMC scratch register to the boot flags.
    //

    if ((Scratch & SMC_SCRATCH_SHORT_ANIMATION) != 0) {
        XboxBootFlags |= XBOX_BOOTFLAG_SHORTANIMATION;
    }

    if ((Scratch & SMC_SCRATCH_DISPLAY_FATAL_ERROR) != 0) {
        XboxBootFlags |= XBOX_BOOTFLAG_DISPLAYFATALERROR;
    }

    if ((Scratch & SMC_SCRATCH_DASHBOARD_BOOT) != 0) {
        XboxBootFlags |= XBOX_BOOTFLAG_DASHBOARDBOOT;
    }

    //
    // If we've determined that we need to do a tray eject, then we're going to
    // force a boot into the dashboard.  The SMC needs to go into non-secure
    // mode now, so that the tray open event doesn't require us to reboot (which
    // would cause a IDE bus reset that would close the tray).
    //

    if (TrayEjectPending) {

        XboxBootFlags |= XBOX_BOOTFLAG_NONSECUREMODE | XBOX_BOOTFLAG_TRAYEJECT;

        HalWriteSMBusByte(SMC_SLAVE_ADDRESS,
            SMC_COMMAND_OVERRIDE_RESET_ON_TRAY_OPEN, SMC_RESET_ON_TRAY_OPEN_NONSECURE_MODE);

        HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_DVD_TRAY_OPERATION,
            SMC_DVD_TRAY_OPERATION_OPEN);

        HalpTrayState = SMC_TRAY_STATE_UNLOADING;
    }
}

VOID
HalBlockIfNoAVPack(
    VOID
    )
/*++

Routine Description:

    This routine blocks forever if no AV pack is attached at boot.  If an AV
    pack is attached, then this routine returns immediately.

Arguments:

    None.

Return Value:

    None.

--*/
{
    LARGE_INTEGER Interval;

    if ((AvSMCVideoModeToAVPack(HalBootSMCVideoMode) == AV_PACK_NONE) &&
        ((XboxGameRegion & XC_GAME_REGION_MANUFACTURING) == 0)) {

        //
        // Clear the scratch register so that when we do reboot, we see the full
        // boot animation.
        //

        HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_SCRATCH, 0);

        //
        // Block for several minutes to allow the user a chance to attach an AV
        // pack.  If no AV pack is attached after that time, then attempt to
        // shutdown the system.
        //

        for (;;) {

            Interval.QuadPart = -5 * 60000 * 10000i64;

            HalDbgPrint(("HAL: no AV pack detected; blocking for AV pack attachment.\n"));

            KeDelayExecutionThread(KernelMode, FALSE, &Interval);

            HalDbgPrint(("HAL: no AV pack detected; turning system off.\n"));

            HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_RESET,
                SMC_RESET_ASSERT_SHUTDOWN);
        }
    }
}

BOOLEAN
HalEnableTrayEjectRequiresReboot(
    IN BOOLEAN EnterNonSecureMode
    )
/*++

Routine Description:

    This routine sets the flag that indicates that a tray eject will require a
    reboot.

Arguments:

    EnterNonSecureMode - Specifies TRUE if the console should be switched to
        non-secure mode before returning from this routine.

Return Value:

    Returns TRUE if a tray eject has occurred before setting the flag.

--*/
{
    KIRQL OldIrql;
    SMBUS_STATUS_BLOCK SMBusStatusBlock;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    // Prevent thread suspension while we own the SMBus lock.
    //

    KeEnterCriticalRegion();

    //
    // Synchronize with the SCI handler which acquires the SMBus lock for the
    // entire handling of an SCI.  This ensures that we're not in the middle of
    // handling a tray eject interrupt while we're in this code.  Also, we
    // require the lock below in order to switch to non-secure mode if
    // requested.
    //

    KeWaitForSingleObject(&HalpSMBusLock, Executive, KernelMode, FALSE, NULL);

    //
    // Set the flag that any future tray eject interrupts will require a reboot
    // in order to do the eject work.
    //

    HalpTrayEjectRequiresReboot = TRUE;

    //
    // Check if the caller wants us to switch to non-secure mode and that we're
    // not already in non-secure mode.  We can only switch to non-secure mode if
    // this is a cold boot.
    //

    if (!KeHasQuickBooted && EnterNonSecureMode &&
        ((XboxBootFlags & XBOX_BOOTFLAG_NONSECUREMODE) == 0)) {

        //
        // Initialize the completion event and set the global pointer to point at
        // our status block.
        //

        KeInitializeEvent(&SMBusStatusBlock.CompletionEvent, NotificationEvent,
            FALSE);

        HalpSMBusStatusBlock = &SMBusStatusBlock;

        //
        // Switch to non-secure mode.
        //

        XboxBootFlags |= XBOX_BOOTFLAG_NONSECUREMODE;

        HalpKickoffWriteSMBus(SMC_SLAVE_ADDRESS,
            SMC_COMMAND_OVERRIDE_RESET_ON_TRAY_OPEN, FALSE,
            SMC_RESET_ON_TRAY_OPEN_NONSECURE_MODE);

        //
        // Block for completion of the SMBus transaction.
        //

        KeWaitForSingleObject(&SMBusStatusBlock.CompletionEvent, Executive,
            KernelMode, FALSE, NULL);

    } else {

        //
        // Release the SMBus lock.  The lock release must be called at DPC level
        // in order to properly synchronize with the SCI handler.
        //

        OldIrql = KeRaiseIrqlToDpcLevel();

        HalpReleaseSMBusLock();

        KeLowerIrql(OldIrql);
    }

    KeLeaveCriticalRegion();

    //
    // Return to the caller whether or not a tray eject has occurred before the
    // above flag was set.
    //

    return HalpTrayEjectDuringBootOccurred;
}

NTSTATUS
HalReadSMCTrayState(
    OUT PULONG TrayState,
    OUT PULONG TrayStateChangeCount OPTIONAL
    )
/*++

Routine Description:

    This routine reads the SMC tray state.  The SMC tray state is cached
    globally and invalidated when a tray open or close event occurs.  Callers
    that use this routine instead of directly polling the SMC can avoid
    generating unnecessary SMBus traffic.

Arguments:

    TrayState - Specifies the buffer to receive the tray state.

    TrayStateChangeCount - Specifies the optional buffer to receive the number
        of times that the tray has been opened and closed.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    KIRQL OldIrql;
    ULONG LocalTrayState;
    ULONG LocalTrayStateChangeCount;

    //
    // Capture the contents of the tray state globals.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    LocalTrayState = HalpTrayState;
    LocalTrayStateChangeCount = HalpTrayStateChangeCount;

    KeLowerIrql(OldIrql);

    //
    // Check if the tray state is either uninitialized or has been cleared by
    // the SMC interrupt handler because of a tray state change.
    //

    if (LocalTrayState == MAXULONG) {

        //
        // Read the tray state from the SMC.
        //

        status = HalReadSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_TRAY_STATE,
            &LocalTrayState);

        if (NT_SUCCESS(status)) {

            //
            // Mask out the bits not related to the tray state and normalize the
            // tray state.  Because the SMC only interrupts us when media is
            // detected, when the drive is empty, or when the tray is opening,
            // we'll only return this set of states.  Most callers don't care
            // about the other transition states anyway.
            //

            LocalTrayState &= SMC_TRAY_STATE_STATE_MASK;

            switch (LocalTrayState) {

                case SMC_TRAY_STATE_NO_MEDIA:
                case SMC_TRAY_STATE_MEDIA_DETECT:
                    break;

                default:
                    LocalTrayState = SMC_TRAY_STATE_OPEN;
                    break;
            }

            //
            // If the tray state change count hasn't changed since we performed
            // the above read, then cache the tray state back in the global.
            //

            OldIrql = KeRaiseIrqlToDpcLevel();

            if (LocalTrayStateChangeCount == HalpTrayStateChangeCount) {
                HalpTrayState = LocalTrayState;
            }

            KeLowerIrql(OldIrql);
        }

    } else {
        status = STATUS_SUCCESS;
    }

    //
    // Return the results to the caller.
    //

    *TrayState = LocalTrayState;

    if (TrayStateChangeCount != NULL) {
        *TrayStateChangeCount = LocalTrayStateChangeCount;
    }

    return status;
}

VOID
HalWriteSMCLEDStates(
    IN ULONG LEDStates
    )
/*++

Routine Description:

    This routine writes the supplied LED states to the SMC and enables the LED
    override so that the LEDs show the requested state.

Arguments:

    LEDStates - Specifies the LED states to send to the SMC.

Return Value:

    None.

--*/
{
    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_LED_STATES, LEDStates);

    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_LED_OVERRIDE,
        SMC_LED_OVERRIDE_USE_REQUESTED_LED_STATES);
}

BOOLEAN
HalIsResetOrShutdownPending(
    VOID
    )
/*++

Routine Description:

    This routine returns whether or not the console is in the middle of a reset
    or shutdown sequence.

Arguments:

    None.

Return Value:

    Returns TRUE if the console is in the middle of a reset or shutdown
    sequence.

--*/
{
    //
    // If the SMC reset or shutdown code has been filled in, then we're in the
    // middle of a reset or shutdown sequence.  The SMC doesn't support any
    // commands with a value of zero.
    //

    return (BOOLEAN)(HalpSMCResetOrShutdownCommandCode != 0);
}

VOID
HalInitiateShutdown(
    VOID
    )
/*++

Routine Description:

    This routine initiates a shutdown operation.

    Note that this routine may return if the SMBus lock is already owned.  The
    shutdown will start as soon as the SMBus lock is available.

Arguments:

    None.

Return Value:

    None.

--*/
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    HalDbgPrint(("HalInitiateShutdown called.\n"));

    //
    // Test if the SMBus lock is currently owned.  If so, set a flag so that
    // when the SMBus is available, we can take control.
    //

    if (!KeReadStateEvent(&HalpSMBusLock)) {
        HalpShutdownRequestPending = TRUE;
        return;
    }

    //
    // The SMBus lock is currently unowned.  Clear the event so that other
    // PASSIVE_LEVEL callers will block and so that this routine will not be
    // reentered.
    //

    KeClearEvent(&HalpSMBusLock);

    //
    // Setup the SMC command code and data value that should be sent to the SMC
    // when the shutdown is complete.
    //

    HalpSMCResetOrShutdownCommandCode = SMC_COMMAND_RESET;
    HalpSMCResetOrShutdownDataValue = SMC_RESET_ASSERT_SHUTDOWN;

    HalpCommonResetOrShutdown();
}

NTSTATUS
HalWriteSMCScratchRegister(
    IN ULONG ScratchRegister
    )
/*++

Routine Description:

    This routine initiates a shutdown operation.

    Note that this routine may return if the SMBus lock is already owned.  The
    shutdown will start as soon as the SMBus lock is available.

Arguments:

    None.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;

    //
    // Remember the value that we're writing out for the scratch register.  This
    // is needed when we're ejecting the tray: this value is OR'ed with
    // SMC_SCRATCH_TRAY_EJECT_PENDING.
    //

    HalpSMCScratchRegister = ScratchRegister;

    //
    // Write the value out to the SMC.
    //

    status = HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_SCRATCH,
        ScratchRegister);

    return status;
}

VOID
HalEnableSecureTrayEject(
    VOID
    )
/*++

Routine Description:

    This routine switches the console to secure mode where a tray eject or tray
    open interrupt causes the console to reboot.  Once the console is in secure
    mode, it cannot be switched back to non-secure mode.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NTSTATUS status;

    //
    // We only need to enable secure mode if the system is currently in
    // non-secure mode.
    //

    if ((XboxBootFlags & XBOX_BOOTFLAG_NONSECUREMODE) != 0) {

        XboxBootFlags &= ~XBOX_BOOTFLAG_NONSECUREMODE;

        do {
            status = HalWriteSMBusByte(SMC_SLAVE_ADDRESS,
                SMC_COMMAND_OVERRIDE_RESET_ON_TRAY_OPEN,
                SMC_RESET_ON_TRAY_OPEN_SECURE_MODE);
        } while (!NT_SUCCESS(status));
    }
}
