/*++

Copyright (c) 2000-2002  Microsoft Corporation

Module Name:

    channel.c

Abstract:

    This module implements routines that apply to the channel object.

--*/

#include "idex.h"

//
// Global IDE channel object for the primary IDE channel.
//
IDE_CHANNEL_OBJECT IdexChannelObject;

//
// Global physical region descriptor table.  Because of PCI IDE hardware
// restrictions, the table cannot span a 64K physical address boundary.  To
// ensure that this happens, we'll place the table in its own section and align
// it to a boundary greater than the size of the table.
//
#pragma bss_seg("IDEXPRDT")
IDE_PCI_PHYSICAL_REGION_DESCRIPTOR
    IdexChannelPhysicalRegionDescriptorTable[IDE_ATA_MAXIMUM_TRANSFER_PAGES + 1];
#pragma bss_seg()
#pragma comment(linker, "/SECTION:IDEXPRDT,,ALIGN=512")

#if DBG
//
// Stores whether or not IdexChannelPrepareToQuickRebootSystem has been called.
//
BOOLEAN IdexChannelQuickRebooting;
#endif

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, IdexChannelIdentifyDevice)
#pragma alloc_text(INIT, IdexChannelSetTransferMode)
#endif

BOOLEAN
FASTCALL
IdexChannelSpinWhileBusy(
    OUT PUCHAR IdeStatus
    )
/*++

Routine Description:

    This routine spins until the the IDE status register's DRQ bit is clear,
    which indicates that the device is ready to accept a command.

Arguments:

    IdeStatus - Specifies a location to receive the final read of the IDE status
        register.

Return Value:

    Returns TRUE if the BSY bit was clear before timing out, else FALSE.

--*/
{
    ULONG Retries;

    IdexAssertIrqlAtChannelDIRQL();

    //
    // Spin for up to one second.
    //

    for (Retries = 10000; Retries > 0; Retries--) {

        *IdeStatus = IdexReadStatusPort();

        if (IdexIsFlagClear(*IdeStatus, IDE_STATUS_BSY)) {
            return TRUE;
        }

        KeStallExecutionProcessor(100);
    }

    return FALSE;
}

BOOLEAN
FASTCALL
IdexChannelSpinWhileBusyAndNotDrq(
    OUT PUCHAR IdeStatus
    )
/*++

Routine Description:

    This routine spins until the the IDE status register's DRQ bit is set and
    the BSY flag is clear, which indicates that the device is ready to return
    data.

Arguments:

    IdeStatus - Specifies a location to receive the final read of the IDE status
        register.

Return Value:

    Returns TRUE if the DRQ bit was set before timing out, else FALSE.

--*/
{
    ULONG Retries;

    IdexAssertIrqlAtChannelDIRQL();

    //
    // Spin for up to one second.
    //

    for (Retries = 10000; Retries > 0; Retries--) {

        *IdeStatus = IdexReadStatusPort();

        if (IdexIsFlagClear(*IdeStatus, IDE_STATUS_BSY) &&
            IdexIsFlagSet(*IdeStatus, IDE_STATUS_DRQ)) {
            return TRUE;
        }

        KeStallExecutionProcessor(100);
    }

    return FALSE;
}

VOID
IdexChannelSetTimerPeriod(
    IN LONG Period
    )
/*++

Routine Description:

    This routine sets the period for the channel's timer.

Arguments:

    Period - Specifies the period for the channel's timer in milliseconds.

Return Value:

    None.

--*/
{
    LARGE_INTEGER TimerDueTime;

    TimerDueTime.QuadPart = (LONG)(-10000 * Period);

    KeSetTimerEx(&IdexChannelObject.Timer, TimerDueTime, Period,
        &IdexChannelObject.TimerDpc);
}

VOID
FASTCALL
IdexChannelPrepareBufferTransfer(
    IN PUCHAR Buffer,
    IN ULONG ByteCount
    )
/*++

Routine Description:

    This routine initializes the channel's physical descriptor table to describe
    the physical pages in the supplied user buffer.

Arguments:

    Buffer - Specifies the buffer with the pages to be placed in the physical
        descriptor table.

    ByteCount - Specifies the number of bytes from the buffer to be placed in
        the physical descriptor table.

Return Value:

    None.

--*/
{
    PIDE_PCI_PHYSICAL_REGION_DESCRIPTOR PhysicalRegionDescriptor;
    PIDE_PCI_PHYSICAL_REGION_DESCRIPTOR EndingPhysicalRegionDescriptor;
    ULONG BytesRemaining;
    ULONG RegionPhysicalAddress;
    ULONG RegionByteCount;
    ULONG CurrentPhysicalAddress;

    ASSERT((ByteCount > 0) && (ByteCount <= IDE_ATA_MAXIMUM_TRANSFER_BYTES));

    PhysicalRegionDescriptor = IdexChannelPhysicalRegionDescriptorTable;
    EndingPhysicalRegionDescriptor = PhysicalRegionDescriptor +
        (IDE_ATA_MAXIMUM_TRANSFER_PAGES + 1);
    BytesRemaining = ByteCount;

    //
    // Handle the first page of the buffer specially since it can be non-page
    // aligned.
    //

    RegionPhysicalAddress = MmGetPhysicalAddress(Buffer);
    RegionByteCount = PAGE_SIZE - BYTE_OFFSET(Buffer);

    if (RegionByteCount > BytesRemaining) {
        RegionByteCount = BytesRemaining;
    }

    Buffer += RegionByteCount;
    BytesRemaining -= RegionByteCount;

    //
    // Handle the rest of the pages in the buffer.  All of these elements will
    // start on a page boundary.
    //

    if (BytesRemaining > 0) {

        for (;;) {

            CurrentPhysicalAddress = MmGetPhysicalAddress(Buffer);

            //
            // Check if this page is physically contiguous with the active
            // region and that it's in the same 64K chunk of memory.  If either
            // of these are false, then the active region is complete and we'll
            // start a new region.
            //

            if ((RegionPhysicalAddress + RegionByteCount != CurrentPhysicalAddress) ||
                ((RegionPhysicalAddress >> 16) != (CurrentPhysicalAddress >> 16))) {

                ASSERT((RegionPhysicalAddress & IDE_ALIGNMENT_REQUIREMENT) == 0);
                ASSERT((RegionByteCount & IDE_ALIGNMENT_REQUIREMENT) == 0);
                ASSERT(RegionByteCount <= 0x10000);

                PhysicalRegionDescriptor->PhysicalAddress = RegionPhysicalAddress;
                PhysicalRegionDescriptor->ByteCount.AsULong = (USHORT)RegionByteCount;
                PhysicalRegionDescriptor++;

                ASSERT(PhysicalRegionDescriptor <= EndingPhysicalRegionDescriptor);

                RegionPhysicalAddress = CurrentPhysicalAddress;
                RegionByteCount = 0;
            }

            //
            // Adjust the active region size and the number of bytes remaining
            // while watching for the last possibly non-whole page.
            //

            if (BytesRemaining > PAGE_SIZE) {

                RegionByteCount += PAGE_SIZE;
                Buffer += PAGE_SIZE;
                BytesRemaining -= PAGE_SIZE;

            } else {

                RegionByteCount += BytesRemaining;
                break;
            }
        }
    }

    //
    // There are no more bytes remaining, so save off the active region and set
    // the end of table flag.
    //

    ASSERT((RegionPhysicalAddress & IDE_ALIGNMENT_REQUIREMENT) == 0);
    ASSERT((RegionByteCount & IDE_ALIGNMENT_REQUIREMENT) == 0);
    ASSERT(RegionByteCount <= 0x10000);

    PhysicalRegionDescriptor->PhysicalAddress = RegionPhysicalAddress;
    PhysicalRegionDescriptor->ByteCount.AsULong = (USHORT)RegionByteCount;
    PhysicalRegionDescriptor->ByteCount.b.EndOfTable = 1;

    ASSERT(PhysicalRegionDescriptor <= EndingPhysicalRegionDescriptor);

    //
    // Reload the bus master's descriptor table register and clear any
    // interrupts and errors.
    //

    IdexWriteBusMasterDescriptorTablePort(
        IdexChannelObject.PhysicalRegionDescriptorTablePhysical);
    IdexWriteBusMasterStatusPort(IDE_BUS_MASTER_STATUS_INTERRUPT |
        IDE_BUS_MASTER_STATUS_ERROR);
}

VOID
IdexChannelPrepareScatterGatherTransfer(
    IN PFILE_SEGMENT_ELEMENT SegmentArray,
    IN ULONG SegmentByteOffset,
    IN ULONG ByteCount
    )
/*++

Routine Description:

    This routine initializes the channel's physical descriptor table to describe
    the physical pages in the supplied user buffer.

Arguments:

    SegmentArray - Specifies the virtual addresses of the pages that make up the
        transfer buffer.

    SegmentByteOffset - Specifies the byte offset to start reading from the
        segment array.

    ByteCount - Specifies the number of bytes from the buffer to be placed in
        the physical descriptor table.

Return Value:

    None.

--*/
{
    PIDE_PCI_PHYSICAL_REGION_DESCRIPTOR PhysicalRegionDescriptor;
    PIDE_PCI_PHYSICAL_REGION_DESCRIPTOR EndingPhysicalRegionDescriptor;
    ULONG BytesRemaining;
    ULONG RegionByteCount;
    ULONG RegionPhysicalAddress;
    PVOID Buffer;
    ULONG CurrentPhysicalAddress;

    ASSERT((ByteCount > 0) && (ByteCount <= IDE_ATA_MAXIMUM_TRANSFER_BYTES));

    PhysicalRegionDescriptor = IdexChannelPhysicalRegionDescriptorTable;
    EndingPhysicalRegionDescriptor = PhysicalRegionDescriptor +
        (IDE_ATA_MAXIMUM_TRANSFER_PAGES + 1);
    BytesRemaining = ByteCount;

    //
    // Advance the segment array to the specified byte offset.  The byte offset
    // should be a multiple of the page size.
    //

    ASSERT(BYTE_OFFSET(SegmentByteOffset) == 0);

    SegmentArray += (SegmentByteOffset >> PAGE_SHIFT);

    //
    // Process the file segment element array.
    //

    RegionByteCount = 0;
    RegionPhysicalAddress = 0;

    for (;;) {

        Buffer = PAGE_ALIGN(SegmentArray->Buffer);
        CurrentPhysicalAddress = MmGetPhysicalAddress(Buffer);

        //
        // For the first iteration of the loop, initialize the starting physical
        // region address to the current physical address.
        //

        if (RegionByteCount == 0) {
            RegionPhysicalAddress = CurrentPhysicalAddress;
        }

        //
        // Check if this page is physically contiguous with the active
        // region and that it's in the same 64K chunk of memory.  If either
        // of these are false, then the active region is complete and we'll
        // start a new region.
        //

        if ((RegionPhysicalAddress + RegionByteCount != CurrentPhysicalAddress) ||
            ((RegionPhysicalAddress >> 16) != (CurrentPhysicalAddress >> 16))) {

            ASSERT((RegionPhysicalAddress & IDE_ALIGNMENT_REQUIREMENT) == 0);
            ASSERT((RegionByteCount & IDE_ALIGNMENT_REQUIREMENT) == 0);
            ASSERT(RegionByteCount <= 0x10000);

            PhysicalRegionDescriptor->PhysicalAddress = RegionPhysicalAddress;
            PhysicalRegionDescriptor->ByteCount.AsULong = (USHORT)RegionByteCount;
            PhysicalRegionDescriptor++;

            ASSERT(PhysicalRegionDescriptor <= EndingPhysicalRegionDescriptor);

            RegionPhysicalAddress = CurrentPhysicalAddress;
            RegionByteCount = 0;
        }

        //
        // Adjust the active region size and the number of bytes remaining
        // while watching for the last possibly non-whole page.
        //

        if (BytesRemaining > PAGE_SIZE) {

            RegionByteCount += PAGE_SIZE;
            SegmentArray++;
            BytesRemaining -= PAGE_SIZE;

        } else {

            RegionByteCount += BytesRemaining;
            break;
        }
    }

    //
    // There are no more bytes remaining, so save off the active region and set
    // the end of table flag.
    //

    ASSERT((RegionPhysicalAddress & IDE_ALIGNMENT_REQUIREMENT) == 0);
    ASSERT((RegionByteCount & IDE_ALIGNMENT_REQUIREMENT) == 0);
    ASSERT(RegionByteCount <= 0x10000);

    PhysicalRegionDescriptor->PhysicalAddress = RegionPhysicalAddress;
    PhysicalRegionDescriptor->ByteCount.AsULong = (USHORT)RegionByteCount;
    PhysicalRegionDescriptor->ByteCount.b.EndOfTable = 1;

    ASSERT(PhysicalRegionDescriptor <= EndingPhysicalRegionDescriptor);

    //
    // Reload the bus master's descriptor table register and clear any
    // interrupts and errors.
    //

    IdexWriteBusMasterDescriptorTablePort(
        IdexChannelObject.PhysicalRegionDescriptorTablePhysical);
    IdexWriteBusMasterStatusPort(IDE_BUS_MASTER_STATUS_INTERRUPT |
        IDE_BUS_MASTER_STATUS_ERROR);
}

BOOLEAN
IdexChannelInterrupt(
    IN PKINTERRUPT InterruptObject,
    IN PVOID ServiceContext
    )
/*++

Routine Description:

    This routine is invoked when a hardware interrupt occurs on the channel's
    IRQ.

Arguments:

    InterruptObject - Specifies the interrupt object.

    ServiceContext - Specifies the context associated with this interrupt
        instance.

Return Value:

    TRUE if the interrupt was consumed by this interrupt routine, else FALSE.

--*/
{
    PIDE_INTERRUPT_ROUTINE InterruptRoutine;
    UCHAR IdeStatus;

    InterruptRoutine = IdexChannelObject.InterruptRoutine;

    if (InterruptRoutine != NULL) {

        //
        // Dispatch the interrupt to the appropriate handler.
        //

        InterruptRoutine();

    } else {

        //
        // Read the status register to dismiss the interrupt.
        //

        IdeStatus = IdexReadStatusPort();
    }

    return TRUE;
}

VOID
IdexChannelDpcForIsr(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystmeArgument2
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
    // Dispatch the DPC request to the appropriate handler.
    //

    IdexChannelObject.FinishIoRoutine();
}

VOID
IdexChannelPollResetComplete(
    VOID
    )
/*++

Routine Description:

    This routine polls the device that is currently resetting itself to check if
    the reset sequence has completed.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(IdexChannelObject.Timeout > 0);

    //
    // Poll the device to see if the reset sequence has completed.
    //

    if (IdexChannelObject.PollResetCompleteRoutine()) {

        //
        // Clear the poll reset routine.
        //

        IdexChannelObject.PollResetCompleteRoutine = NULL;

        //
        // Restore the channel's timer to the normal period.
        //

        IdexChannelSetTimerPeriod(IDE_SLOW_TIMER_PERIOD);

        //
        // If the number of retries hasn't exceeded the maximum retry count,
        // then restart the current packet.
        //

        if (IdexChannelObject.IoRetries < IdexChannelObject.MaximumIoRetries) {
            IdexChannelRestartCurrentPacket();
            return;
        }

    } else {

        //
        // Decrement the reset timeout and bail if the countdown hasn't reached
        // zero.
        //

        IdexChannelObject.Timeout--;

        if (IdexChannelObject.Timeout != 0) {
            return;
        }

        //
        // Clear the poll reset routine.
        //

        IdexChannelObject.PollResetCompleteRoutine = NULL;

        //
        // Restore the channel's timer to the normal period.
        //

        IdexChannelSetTimerPeriod(IDE_SLOW_TIMER_PERIOD);
    }

    //
    // The reset has timed out or the number of retries has exceeded the maximum
    // retry count, so complete the IRP with a device error and start the next
    // packet.
    //

    Irp = IdexChannelObject.CurrentIrp;

    Irp->IoStatus.Status = STATUS_DISK_RESET_FAILED;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
IdexChannelTimer(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine is invoked once per second by the I/O manager in order to check
    for commands that have timed out.

Arguments:

    Dpc - Specifies the timer DPC contained in the channel object.

    DeferredContext - Specifies the context associated with this DPC instance.

    SystemArgument1 - Specifies the low 32-bits of the system time.

    SystemArgument2 - Specifies the high 32-bits of the system time.

Return Value:

    None.

--*/
{
    PIDE_TIMEOUT_EXPIRED_ROUTINE TimeoutExpiredRoutine;
    PIRP Irp;

    //
    // Do the quick check to see if we're waiting for an interrupt to occur.  If
    // there's no interrupt routine set, then there's no point checking the
    // I/O timeout value.
    //

    if (IdexChannelObject.InterruptRoutine == NULL) {

        //
        // Check if we're in the middle of resetting a device.  If so, check if
        // the device has completed its reset sequence.
        //

        if (IdexChannelObject.PollResetCompleteRoutine != NULL) {
            ASSERT(IdexChannelObject.Timer.Period == IDE_FAST_TIMER_PERIOD);
            IdexChannelPollResetComplete();
            return;
        }

        //
        // Bail out now if there isn't a timeout expired routine set.  If
        // there's no timer work to be done, then the timer had better be set to
        // use the slow period.
        //

        if (IdexChannelObject.TimeoutExpiredRoutine == NULL) {
            ASSERT(IdexChannelObject.Timer.Period == IDE_SLOW_TIMER_PERIOD);
            return;
        }

        //
        // Decrement the timeout and bail if the countdown hasn't reached
        // zero.
        //

        ASSERT(IdexChannelObject.Timeout > 0);
        IdexChannelObject.Timeout--;

        if (IdexChannelObject.Timeout != 0) {
            return;
        }

        //
        // Save and clear the timeout expired routine.
        //

        TimeoutExpiredRoutine = IdexChannelObject.TimeoutExpiredRoutine;
        IdexChannelObject.TimeoutExpiredRoutine = NULL;

        //
        // Restore the channel's timer to the normal period.
        //

        IdexChannelSetTimerPeriod(IDE_SLOW_TIMER_PERIOD);

        //
        // Invoke the timeout expired routine.
        //

        TimeoutExpiredRoutine();

        return;
    }

    //
    // If we're waiting for an interrupt to occur, then the timer had better be
    // set to use the slow period.  Also, verify that the other types of timer
    // callback routines are not set.
    //

    ASSERT(IdexChannelObject.Timer.Period == IDE_SLOW_TIMER_PERIOD);
    ASSERT(IdexChannelObject.PollResetCompleteRoutine == NULL);
    ASSERT(IdexChannelObject.TimeoutExpiredRoutine == NULL);

    //
    // Decrement the timeout and bail if the countdown hasn't reached zero.
    //

    ASSERT(IdexChannelObject.Timeout > 0);
    IdexChannelObject.Timeout--;

    if (IdexChannelObject.Timeout != 0) {
        return;
    }

    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Check again if the interrupt was triggered now that we're synchronized
    // with the interrupt service routine.
    //

    if (IdexChannelObject.InterruptRoutine == NULL) {
        IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
        return;
    }

    IdexDbgPrint(("IDEX: command timed out.\n"));

    //
    // Clear the interrupt routine and grab the current IRP.
    //

    IdexChannelObject.InterruptRoutine = NULL;
    Irp = IdexChannelObject.CurrentIrp;

    //
    // Ensure that the bus master interface is stopped.
    //

    IdexWriteBusMasterCommandPort(0);
    IdexWriteBusMasterStatusPort(IDE_BUS_MASTER_STATUS_INTERRUPT);

    //
    // Indicate that the IRP timed out and finish processing the packet back at
    // DPC level.
    //

    Irp->IoStatus.Status = STATUS_IO_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);

    IdexChannelObject.FinishIoRoutine();
}

VOID
IdexChannelStartNextPacketStock(
    VOID
    )
/*++

Routine Description:

    This routine starts the next I/O request packet or marks the IDE channel as
    not busy if no I/O request packets are queued.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PKDEVICE_QUEUE_ENTRY DeviceQueueEntry;
    PIRP Irp;
    PDEVICE_OBJECT DeviceObject;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Clear out the current IRP for debugging purposes.
    //

    IdexChannelObject.CurrentIrp = NULL;

    //
    // Check if we're already nested inside of a StartIo call.  If so, set a
    // flag so that when we pop out of the StartIo call, we'll start another
    // packet.
    //

    if (IdexChannelObject.StartPacketBusy) {
        IdexChannelObject.StartPacketRequested = TRUE;
        return;
    }

    do {

        //
        // Reset the start packet requested flag.
        //

        IdexChannelObject.StartPacketRequested = FALSE;

        //
        // Pull off the next IRP from the device queue.  If there aren't any
        // IRPs queued, then bail out.
        //

        DeviceQueueEntry = KeRemoveDeviceQueue(&IdexChannelObject.DeviceQueue);

        if (DeviceQueueEntry == NULL) {
            break;
        }

        Irp = CONTAINING_RECORD(DeviceQueueEntry, IRP, Tail.Overlay.DeviceQueueEntry);
        DeviceObject = IoGetCurrentIrpStackLocation(Irp)->DeviceObject;

        //
        // Store the current IRP in the channel for debugging purposes.
        //

        IdexChannelObject.CurrentIrp = Irp;

        //
        // Reset the number of retries that have been performed for the current
        // IRP.
        //

        IdexChannelObject.IoRetries = 0;

        //
        // Set the default number of retries that are allowed per IRP.
        //

        IdexChannelObject.MaximumIoRetries = IDE_NORMAL_RETRY_COUNT;

        //
        // Indicate that we're already inside of a start packet call so that
        // recursive calls to start another packet don't overflow the stack.
        //

        IdexChannelObject.StartPacketBusy = TRUE;

        //
        // Invoke the driver's StartIo routine to start the IRP.
        //

        DeviceObject->DriverObject->DriverStartIo(DeviceObject, Irp);

        //
        // We're no longer busy handling a start packet call.
        //

        IdexChannelObject.StartPacketBusy = FALSE;

        //
        // Continue pulling packets off of the device queue while we received a
        // nested start packet call.
        //

    } while (IdexChannelObject.StartPacketRequested);
}

VOID
IdexChannelStartPacketStock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine attempts to start the specified I/O request packet.  If the
    IDE channel is already busy, then the packet is queued as appropriate.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;
    BOOLEAN Inserted;

    //
    // If a DMA transfer is still in progress and we quick reboot, then we could
    // corrupt the memory from the next instance of the kernel.
    //

    ASSERTMSG("I/O cannot be started after reboot was requested\n",
        !IdexChannelQuickRebooting);

    //
    // Synchronize access to the device queue by raising to DPC level.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    //
    // Attempt to insert the packet into the channel's device queue.
    //

    Inserted = KeInsertDeviceQueue(&IdexChannelObject.DeviceQueue,
        &Irp->Tail.Overlay.DeviceQueueEntry);

    //
    // If the packet wasn't inserted into the device queue, then the device
    // queue was not busy and we can start the IRP now.
    //

    if (!Inserted) {

        //
        // Store the current IRP in the channel for debugging purposes.
        //

        IdexChannelObject.CurrentIrp = Irp;

        //
        // Reset the number of retries that have been performed for the current
        // IRP.
        //

        IdexChannelObject.IoRetries = 0;

        //
        // Set the default number of retries that are allowed per IRP.
        //

        IdexChannelObject.MaximumIoRetries = IDE_NORMAL_RETRY_COUNT;

        //
        // Indicate that we're already inside of a start packet call so that
        // recursive calls to start another packet don't overflow the stack.
        //

        IdexChannelObject.StartPacketBusy = TRUE;

        //
        // Invoke the driver's StartIo routine to start the IRP.
        //

        DeviceObject->DriverObject->DriverStartIo(DeviceObject, Irp);

        //
        // We're no longer busy handling a start packet call.
        //

        IdexChannelObject.StartPacketBusy = FALSE;

        //
        // If a request was made to start a packet while we were nested inside
        // of the above StartIo call, then handle the deferred start now.
        //

        if (IdexChannelObject.StartPacketRequested) {
            IdexChannelStartNextPacket();
        }
    }

    KeLowerIrql(OldIrql);
}

VOID
IdexChannelRestartCurrentPacket(
    VOID
    )
/*++

Routine Description:

    This routine increments the I/O retry count for the channel and restarts the
    current IRP.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    PIO_STACK_LOCATION IrpSp;

    Irp = IdexChannelObject.CurrentIrp;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Increment the number of times that we've retried the current packet.
    //

    IdexChannelObject.IoRetries++;

    //
    // Invoke the driver's StartIo routine to start the IRP.
    //

    IrpSp->DeviceObject->DriverObject->DriverStartIo(IrpSp->DeviceObject, Irp);
}

VOID
IdexChannelAbortCurrentPacket(
    VOID
    )
/*++

Routine Description:

    This routine aborts the current IRP.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    Irp = IdexChannelObject.CurrentIrp;

    Irp->IoStatus.Status = STATUS_REQUEST_ABORTED;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
FASTCALL
IdexChannelInvalidParameterRequest(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called from a queued device I/O control routine when an
    invalid parameter is detected.  The I/O request is completed with
    STATUS_INVALID_PARAMETER and the next packet is started.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    IdexChannelStartNextPacket();
}

VOID
IdexChannelIdePassThroughInterrupt(
    VOID
    )
/*++

Routine Description:

    This routine is invoked when a hardware interrupt occurs on the channel's
    IRQ and the pending interrupt IRP is for an IDE pass through request.

Arguments:

    None.

Return Value:

    None.

--*/
{
    UCHAR IdeStatus;
    PIRP Irp;
    PATA_PASS_THROUGH AtaPassThrough;

    //
    // Read the status register to dismiss the interrupt.
    //

    IdeStatus = IdexReadStatusPort();

    //
    // Clear the interrupt routine and grab the current IRP.
    //

    IdexChannelObject.InterruptRoutine = NULL;
    Irp = IdexChannelObject.CurrentIrp;

    //
    // Take a snapshot of the IDE registers.
    //

    AtaPassThrough = (PATA_PASS_THROUGH)Irp->UserBuffer;

    AtaPassThrough->IdeReg.bFeaturesReg = IdexReadErrorPort();
    AtaPassThrough->IdeReg.bSectorCountReg = IdexReadSectorCountPort();
    AtaPassThrough->IdeReg.bSectorNumberReg = IdexReadSectorNumberPort();
    AtaPassThrough->IdeReg.bCylLowReg = IdexReadCylinderLowPort();
    AtaPassThrough->IdeReg.bCylHighReg = IdexReadCylinderHighPort();
    AtaPassThrough->IdeReg.bCommandReg = IdeStatus;

    //
    // If this is a data in command, then read the data from the device.
    //

    if ((AtaPassThrough->DataBufferSize != 0) &&
        !AtaPassThrough->IdeReg.bHostSendsData) {

        IdexReadDataPortBufferUshort((PUSHORT)AtaPassThrough->DataBuffer,
            AtaPassThrough->DataBufferSize / sizeof(USHORT));

        if ((AtaPassThrough->DataBufferSize & 1) != 0) {
            ((PUCHAR)AtaPassThrough->DataBuffer)[AtaPassThrough->DataBufferSize - 1] =
                IdexReadDataPortUchar();
        }
    }

    Irp->IoStatus.Status = STATUS_SUCCESS;

    //
    // Finish processing the IRP at DPC level.
    //

    KeInsertQueueDpc(&IdexChannelObject.FinishDpc, NULL, NULL);
}

VOID
IdexChannelFinishIdePassThrough(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing a
    IOCTL_IDE_PASS_THROUGH request.

Arguments:

    None.

Return Value:

    None.

--*/
{
    IoCompleteRequest(IdexChannelObject.CurrentIrp, IO_DISK_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
IdexChannelStartIdePassThrough(
    IN PIRP Irp,
    IN UCHAR TargetDevice,
    IN PIDE_RESET_DEVICE_ROUTINE ResetDeviceRoutine
    )
/*++

Routine Description:

    This routine handles queued IOCTL_IDE_PASS_THROUGH requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

    TargetDevice - Specifies the device number to send the pass through to.

    ResetDeviceRoutine - Specifies the routine to invoke to reset the device if
        it's busy.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PATA_PASS_THROUGH AtaPassThrough;
    UCHAR IdeStatus;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    AtaPassThrough = (PATA_PASS_THROUGH)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Verify that the input buffer is the same as the output buffer, that the
    // input buffer is large enough, and that the output buffer is large enough
    // (including the data buffer size).
    //

    if ((Irp->UserBuffer != IrpSp->Parameters.DeviceIoControl.InputBuffer) ||
        (IrpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof(ATA_PASS_THROUGH)) ||
        (IrpSp->Parameters.DeviceIoControl.OutputBufferLength < sizeof(ATA_PASS_THROUGH))) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Initialize the IRP's information result to the number of bytes in the
    // output buffer.
    //

    Irp->IoStatus.Information = IrpSp->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Select the IDE device and spin until the device is not busy.
    //

    IdexProgramTargetDevice(TargetDevice);

    if (!IdexChannelSpinWhileBusy(&IdeStatus)) {

        //
        // Attempt to reset the device.  If the reset completes successfully and
        // the retry count has not been exceeded the maximum retry count, then
        // the IRP will be restarted.
        //

        ResetDeviceRoutine();
        return;
    }

    //
    // Issue the IDE command.
    //

    IdexWriteFeaturesPort(AtaPassThrough->IdeReg.bFeaturesReg);
    IdexWriteSectorCountPort(AtaPassThrough->IdeReg.bSectorCountReg);
    IdexWriteSectorNumberPort(AtaPassThrough->IdeReg.bSectorNumberReg);
    IdexWriteCylinderLowPort(AtaPassThrough->IdeReg.bCylLowReg);
    IdexWriteCylinderHighPort(AtaPassThrough->IdeReg.bCylHighReg);
    IdexWriteCommandPort(AtaPassThrough->IdeReg.bCommandReg);

    //
    // If this is a data out command, then wait for the device to be not busy
    // and ready to accept data.
    //

    if ((AtaPassThrough->DataBufferSize != 0) &&
        AtaPassThrough->IdeReg.bHostSendsData) {

        if (!IdexChannelSpinWhileBusyAndNotDrq(&IdeStatus)) {
            ResetDeviceRoutine();
            return;
        }

        //
        // Write out the data to the device.
        //

        IdexWriteDataPortBufferUshort((PUSHORT)AtaPassThrough->DataBuffer,
            AtaPassThrough->DataBufferSize / sizeof(USHORT));

        if ((AtaPassThrough->DataBufferSize & 1) != 0) {
            IdexWriteDataPortUchar(((PUCHAR)AtaPassThrough->DataBuffer)[AtaPassThrough->DataBufferSize - 1]);
        }
    }

    //
    // Indicate that we're expecting an interrupt for this IRP.
    //

    ASSERT(IdexChannelObject.InterruptRoutine == NULL);

    IdexChannelObject.InterruptRoutine = IdexChannelIdePassThroughInterrupt;
    IdexChannelObject.FinishIoRoutine = IdexChannelFinishIdePassThrough;
    IdexChannelObject.Timeout = IDE_ATA_DEFAULT_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

NTSTATUS
IdexChannelIdentifyDevice(
    IN UCHAR TargetDevice,
    IN UCHAR IdentifyCommand,
    OUT PIDE_IDENTIFY_DATA IdentifyData
    )
/*++

Routine Description:

    This routine executes an identify command for the supplied device.

Arguments:

    TargetDrive - Specifies the IDE drive to be identified.

    IdentifyCommand - Specifies the IDE identify command to be sent.

    IdentifyData - Specifies the buffer to receive the identification data.

Return Value:

    Status of operation.

--*/
{
    UCHAR IdeStatus;

    IdexAssertIrqlAtChannelDIRQL();

    //
    // Select the IDE device.
    //

    IdexProgramTargetDevice(TargetDevice);

    //
    // Check if the channel has anything attached to it.
    //

    IdexWriteCylinderLowPort(0x5A);
    IdexWriteCylinderHighPort(0xA5);

    if ((IdexReadCylinderLowPort() != 0x5A) ||
        (IdexReadCylinderHighPort() != 0xA5)) {
        return STATUS_DEVICE_DOES_NOT_EXIST;
    }

    //
    // Spin until the device is not busy.
    //

    if (!IdexChannelSpinWhileBusy(&IdeStatus)) {
        return STATUS_IO_TIMEOUT;
    }

    //
    // Issue the identify command.
    //

    IdexWriteCommandPort(IdentifyCommand);

    //
    // Spin until the device has is not busy and is ready to send data.
    //

    if (!IdexChannelSpinWhileBusyAndNotDrq(&IdeStatus)) {
        return STATUS_IO_TIMEOUT;
    }

    //
    // If the channel has raised an error, then abandon the request.
    //

    if (IdexIsFlagSet(IdeStatus, IDE_STATUS_ERR)) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Read the identify data.
    //

    IdexReadDataPortBufferUlong((PULONG)IdentifyData, sizeof(*IdentifyData) /
        sizeof(ULONG));

    //
    // If the channel has raised an error, then abandon the request.  The device
    // may have indicated that the data is available but is invalid.
    //

    if (IdexIsFlagSet(IdeStatus, IDE_STATUS_ERR)) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
IdexChannelSetTransferMode(
    IN UCHAR TargetDevice,
    IN UCHAR TransferMode
    )
/*++

Routine Description:

    This routine sets the transfer mode for the supplied device to the supplied
    transfer mode.

Arguments:

    TargetDevice - Specifies the device number to set the transfer mode for.

    TransferMode - Specifies the desired transfer mode for the device.

Return Value:

    Status of operation.

--*/
{
    UCHAR IdeStatus;

    IdexAssertIrqlAtChannelDIRQL();

    //
    // Select the IDE device.
    //

    IdexProgramTargetDevice(TargetDevice);

    //
    // Spin until the device is not busy.
    //

    if (!IdexChannelSpinWhileBusy(&IdeStatus)) {
        return STATUS_IO_TIMEOUT;
    }

    //
    // Issue the set transfer mode command.
    //

    IdexWriteFeaturesPort(IDE_FEATURE_SET_TRANSFER_MODE);
    IdexWriteSectorCountPort(TransferMode);

    IdexWriteCommandPort(IDE_COMMAND_SET_FEATURES);

    //
    // Spin until the device is not busy.
    //

    if (!IdexChannelSpinWhileBusy(&IdeStatus)) {
        return STATUS_IO_TIMEOUT;
    }

    //
    // If the channel has raised an error, then abandon the request.
    //

    if (IdexIsFlagSet(IdeStatus, IDE_STATUS_ERR)) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
IdexChannelIssueImmediateCommand(
    IN UCHAR TargetDevice,
    IN UCHAR IdeCommand
    )
/*++

Routine Description:

    This routine issues the supplied immediate command to the supplied device.

Arguments:

    TargetDevice - Specifies the device number to set the transfer mode for.

    IdeCommand - Specifies the IDE command to issue.

Return Value:

    Status of operation.

--*/
{
    UCHAR IdeStatus;

    IdexAssertIrqlAtChannelDIRQL();

    //
    // Select the IDE device.
    //

    IdexProgramTargetDevice(TargetDevice);

    //
    // Spin until the device is not busy.
    //

    if (!IdexChannelSpinWhileBusy(&IdeStatus)) {
        return STATUS_IO_TIMEOUT;
    }

    //
    // Issue the IDE command.
    //

    IdexWriteCommandPort(IdeCommand);

    //
    // Spin until the device is not busy.
    //

    if (!IdexChannelSpinWhileBusy(&IdeStatus)) {
        return STATUS_IO_TIMEOUT;
    }

    //
    // If the channel has raised an error, then abandon the request.
    //

    if (IdexIsFlagSet(IdeStatus, IDE_STATUS_ERR)) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    return STATUS_SUCCESS;
}

VOID
IdexChannelCreate(
    VOID
    )
/*++

Routine Description:

    This routine constructs and initializes a channel device object.

Arguments:

    None.

Return Value:

    None.

--*/
{
    ULONG InterruptVector;
    PIDE_PCI_PHYSICAL_REGION_DESCRIPTOR PhysicalRegionDescriptor;
    PIDE_PCI_PHYSICAL_REGION_DESCRIPTOR EndingPhysicalRegionDescriptor;
#ifdef ARCADE
    BOOLEAN MediaBoardDetected;
#endif

    //
    // Initialize the pointers to the stock start packet routines.
    //

    IdexChannelObject.StartPacketRoutine = IdexChannelStartPacketStock;
    IdexChannelObject.StartNextPacketRoutine = IdexChannelStartNextPacketStock;

    //
    // Initialize the channe's device queue.
    //

    KeInitializeDeviceQueue(&IdexChannelObject.DeviceQueue);

    //
    // Initialize and start the channel's timer.
    //

    KeInitializeDpc(&IdexChannelObject.TimerDpc, IdexChannelTimer, NULL);

    KeInitializeTimerEx(&IdexChannelObject.Timer, SynchronizationTimer);

    IdexChannelSetTimerPeriod(IDE_SLOW_TIMER_PERIOD);

    //
    // Initialize the channel's post interrupt DPC handler.
    //

    KeInitializeDpc(&IdexChannelObject.FinishDpc, IdexChannelDpcForIsr, NULL);

    //
    // Enable interrupts for the channel.
    //

    IdexWriteDeviceControlPort(0);

    //
    // Connect to the channel's interrupt.
    //

    InterruptVector = HalGetInterruptVector(IDE_CHANNEL_IRQ_RESOURCE,
        &IdexChannelObject.InterruptIrql);

    KeInitializeInterrupt(&IdexChannelObject.InterruptObject,
        IdexChannelInterrupt, NULL, InterruptVector,
        IdexChannelObject.InterruptIrql, Latched, FALSE);

    if (!KeConnectInterrupt(&IdexChannelObject.InterruptObject)) {
        IdexBugCheck(IDE_BUG_CHECK_CHANNEL, STATUS_UNSUCCESSFUL);
    }

    //
    // Verify that the physical region descriptor table doesn't straddle a 64K
    // physical address boundary.
    //

    PhysicalRegionDescriptor = IdexChannelPhysicalRegionDescriptorTable;
    EndingPhysicalRegionDescriptor = PhysicalRegionDescriptor +
        (IDE_ATA_MAXIMUM_TRANSFER_PAGES + 1);

    ASSERT(((ULONG_PTR)PhysicalRegionDescriptor >> 16) ==
        (ULONG_PTR)EndingPhysicalRegionDescriptor >> 16);

    //
    // Cache the physical address of the table so that we can point the hardware
    // at it later.
    //

    IdexChannelObject.PhysicalRegionDescriptorTablePhysical =
        MmGetPhysicalAddress(IdexChannelPhysicalRegionDescriptorTable);

#ifdef ARCADE
    //
    // Check for the existence of a media board by comparing the chip
    // revision and DIMM size registers against bus noise
    //

    MediaBoardDetected =
        (BOOLEAN)(IdexReadPortUshort(SEGA_REGISTER_CHIP_REVISION) != 0xFFFF) &&
        (BOOLEAN)(IdexReadPortUshort(SEGA_REGISTER_DIMM_SIZE) != 0xFFFF);

#ifdef DEVKIT
    //
    // Create the disk and either the media board or CD-ROM device objects.
    //

    if (KeHasQuickBooted) {
        IdexDiskCreateQuick();
        MediaBoardDetected ? IdexMediaBoardCreateQuick() : IdexCdRomCreateQuick();
    } else {
        IdexDiskCreate();
        MediaBoardDetected ? IdexMediaBoardCreate() : IdexCdRomCreate();
    }
#else
    //
    // Create the media board device objects.
    //

    if (KeHasQuickBooted) {
        IdexMediaBoardCreateQuick();
    } else if (!MediaBoardDetected) {
        IdexDbgPrint(("IDEX: media board not detected.\n"));
        IdexMediaBoardFatalError(FATAL_ERROR_HDD_NOT_FOUND);
    } else {
        IdexMediaBoardCreate();
    }
#endif
#else
    //
    // Create the disk and CD-ROM device objects.
    //

    if (KeHasQuickBooted) {
        IdexDiskCreateQuick();
        IdexCdRomCreateQuick();
    } else {
        IdexDiskCreate();
        IdexCdRomCreate();
    }
#endif
}

#if DBG

VOID
IdexChannelPrepareToQuickRebootSystem(
    VOID
    )
/*++

Routine Description:

    This routine is called in debug versions of the kernel in order to verify
    that no I/O is in progress and that no further I/O is submitted.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // If a DMA transfer is still in progress and we quick reboot, then we could
    // corrupt the memory from the next instance of the kernel.
    //

    ASSERTMSG("I/O still in progress when reboot was requested\n",
        (IdexChannelObject.CurrentIrp == NULL));

    IdexChannelQuickRebooting = TRUE;
}

#endif
