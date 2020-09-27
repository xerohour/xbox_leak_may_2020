/*++

Copyright (c) 2000-2002  Microsoft Corporation

Module Name:

    cdrom.c

Abstract:

    This module implements routines that apply to the CD-ROM object.

--*/

#include "idex.h"

#if !defined(ARCADE) || defined(DEVKIT)

//
// Single instance of the CD-ROM device object.
//
PDEVICE_OBJECT IdexCdRomDeviceObject;

//
// Name of the CD-ROM device object and its DOS devices symbolic link.
//
INITIALIZED_OBJECT_STRING_RDATA(IdexCdRomDeviceName, "\\Device\\CdRom0");
INITIALIZED_OBJECT_STRING_RDATA(IdexCdRomDosDevicesName, "\\??\\CdRom0:");

//
// Static transfer buffer used for IOCTLs that need to transfer a small number
// of bytes.  Used to avoid adding pool allocation and free code to several code
// paths.  The buffer is already locked due to being part of the kernel image.
//
UCHAR IdexCdRomStaticTransferBuffer[32];

//
// Number of bytes that are available on the single "partition" of the device
// media.  The actual number is filled in when IOCTL_CDROM_GET_DRIVE_GEOMETRY is
// invoked.
//
LONGLONG IdexCdRomPartitionLength = _I64_MAX;

//
// Stores whether or not the disc has passed DVD-X2 authentication.
//
DECLSPEC_STICKY BOOLEAN IdexCdRomDVDX2Authenticated;

#if DBG
//
// Stores the last SCSI error read from the drive.
//
SENSE_DATA IdexCdRomDebugSenseData;

//
// Stores the number of errors that have occurred at the various spindle speeds.
//
ULONG IdexCdRomDebugErrors[DVDX2_SPINDLE_SPEED_MAXIMUM];

//
// Stores the number of sectors that have been transferred.
//
ULONG IdexCdRomDebugReadsFinished;
#endif

//
// Stores the maximum spindle speed requested by the title.
//
ULONG IdexCdRomMaximumSpindleSpeed = DVDX2_SPINDLE_SPEED_MAXIMUM;

//
// Stores the current spindle speed; this should be less than or equal to the
// maximum spindle speed.  Note that this is sticky across boots so that if the
// spindle speed is reduced in one instance of the kernel, the next instance of
// the kernel can correct it.
//
DECLSPEC_STICKY ULONG IdexCdRomCurrentSpindleSpeed = DVDX2_SPINDLE_SPEED_MAXIMUM;

//
// Stores the last sector that was read from while the spindle was slowed down.
//
DECLSPEC_STICKY ULONG IdexCdRomSpindleSlowdownSectorNumber;

//
// Stores the number of sectors that must be transfered before allowing the
// spindle to speed back up.
//
DECLSPEC_STICKY ULONG IdexCdRomSpindleSlowdownSectorsRemaining;

//
// Stores the number of sectors that need to be crossed before we'll attempt to
// restore the spindle speed to the next faster spindle speed.
//
const ULONG IdexCdRomSpindleSpeedupTable[DVDX2_SPINDLE_SPEED_MAXIMUM + 1] = {
    4096,                               // DVDX2_SPINDLE_SPEED_MINIMUM
    8192,                               // DVDX2_SPINDLE_SPEED_MEDIUM
    MAXULONG,                           // DVDX2_SPINDLE_SPEED_MAXIMUM
};

//
// Local support.
//

DECLSPEC_NORETURN
VOID
IdexCdRomFatalError(
    IN ULONG ErrorCode
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, IdexCdRomFatalError)
#pragma alloc_text(INIT, IdexCdRomCreate)
#endif

BOOLEAN
IdexCdRomPollResetComplete(
    VOID
    )
/*++

Routine Description:

    This routine polls the CD-ROM device to check for completion of a reset
    sequence.

Arguments:

    None.

Return Value:

    Returns TRUE if the device has completed its reset sequence.

--*/
{
    UCHAR IdeStatus;

    //
    // Select the IDE device.
    //

    IdexProgramTargetDevice(IDE_CDROM_DEVICE_NUMBER);

    //
    // Check if the device is busy.
    //

    IdeStatus = IdexReadStatusPort();

    if (IdexIsFlagClear(IdeStatus, IDE_STATUS_BSY)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

VOID
IdexCdRomResetDevice(
    VOID
    )
/*++

Routine Description:

    This routine software resets the CD-ROM device.

Arguments:

    None.

Return Value:

    None.

--*/
{
    IdexAssertIrqlAtChannelDIRQL();

    //
    // If the console is preparing to reset or shutdown, there's no reason to
    // continue processing this request.
    //

    if (HalIsResetOrShutdownPending()) {
        IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
        IdexChannelAbortCurrentPacket();
        return;
    }

    //
    // Select the IDE device.
    //

    IdexProgramTargetDevice(IDE_CDROM_DEVICE_NUMBER);

    //
    // Issue the IDE command.
    //

    IdexWriteCommandPort(IDE_COMMAND_DEVICE_RESET);

    //
    // Lower the IRQL back to DISPATCH_LEVEL.
    //

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);

    //
    // Speed up the timer to check for completion of the reset sequence.
    //

    IdexChannelSetTimerPeriod(IDE_FAST_TIMER_PERIOD);

    //
    // Delay for up to twenty-five seconds (100 milliseconds per unit).
    //

    IdexChannelObject.PollResetCompleteRoutine = IdexCdRomPollResetComplete;
    IdexChannelObject.Timeout = 250;
}

BOOLEAN
IdexCdRomSelectDeviceAndSpinWhileBusy(
    VOID
    )
/*++

Routine Description:

    This routine selects the CD-ROM device and spins until the the IDE status
    register's BSY bit is clear, which indicates that the device is ready to
    accept a command.

Arguments:

    None.

Return Value:

    Returns TRUE if the BSY bit was clear before timing out, else FALSE.

--*/
{
    ULONG Retries;
    UCHAR IdeStatus;

    IdexAssertIrqlAtChannelDIRQL();

    //
    // Select the IDE device.
    //

    IdexProgramTargetDevice(IDE_CDROM_DEVICE_NUMBER);

    //
    // Spin for up to a tenth second.
    //

    for (Retries = 1000; Retries > 0; Retries--) {

        IdeStatus = IdexReadStatusPort();

        if (IdexIsFlagClear(IdeStatus, IDE_STATUS_BSY)) {
            return TRUE;
        }

        KeStallExecutionProcessor(100);
    }

    //
    // Lower the IRQL back to DISPATCH_LEVEL.
    //

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);

    //
    // Mark the IRP as timed out.  The caller will pass this IRP to the generic
    // error handler which will reset the device and retry the IRP if allowed.
    //

    IdexChannelObject.CurrentIrp->IoStatus.Status = STATUS_IO_TIMEOUT;

    return FALSE;
}

BOOLEAN
IdexCdRomWritePacket(
    PCDB Cdb
    )
/*++

Routine Description:

    This routine selects the CD-ROM device and spins until the the IDE status
    register's BSY bit is clear, which indicates that the device is ready to
    accept a command.

Arguments:

    None.

Return Value:

    Returns TRUE if the BSY bit was clear before timing out, else FALSE.

--*/
{
    ULONG Retries;
    UCHAR IdeStatus;

    IdexAssertIrqlAtChannelDIRQL();

    //
    // Issue the IDE command.
    //

    IdexWriteCommandPort(IDE_COMMAND_PACKET);

    //
    // Spin until the device indicates that it's ready to receive the SCSI CDB.
    //

    for (Retries = 1000; Retries > 0; Retries--) {

        IdeStatus = IdexReadStatusPort();

        if (IdexIsFlagClear(IdeStatus, IDE_STATUS_BSY) &&
            IdexIsFlagSet(IdeStatus, IDE_STATUS_DRQ)) {

            //
            // Issue the SCSI CDB.
            //

            IdexWriteDataPortCdb(Cdb);

            return TRUE;
        }

        KeStallExecutionProcessor(100);
    }

    //
    // Lower the IRQL back to DISPATCH_LEVEL.
    //

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);

    //
    // Mark the IRP as timed out.  The caller will pass this IRP to the generic
    // error handler which will reset the device and retry the IRP if allowed.
    //

    IdexChannelObject.CurrentIrp->IoStatus.Status = STATUS_IO_TIMEOUT;

    return FALSE;
}

VOID
IdexCdRomNoTransferInterrupt(
    VOID
    )
/*++

Routine Description:

    This routine is invoked when a hardware interrupt occurs on the channel's
    IRQ and the pending interrupt IRP is for an ATA request that transfers data.

Arguments:

    None.

Return Value:

    None.

--*/
{
    UCHAR IdeStatus;
    UCHAR InterruptReason;
    PIRP Irp;

    //
    // Read the status register to dismiss the interrupt.
    //

    IdeStatus = IdexReadStatusPort();

    //
    // Verify that the interrupt reason indicates that the command is complete.
    //

    InterruptReason = IdexReadInterruptReasonPort();

    if ((InterruptReason & (IDE_INTERRUPT_REASON_IO | IDE_INTERRUPT_REASON_CD)) !=
        (IDE_INTERRUPT_REASON_IO | IDE_INTERRUPT_REASON_CD)) {
        return;
    }

    //
    // Clear the interrupt routine and grab the current IRP.
    //

    IdexChannelObject.InterruptRoutine = NULL;
    Irp = IdexChannelObject.CurrentIrp;

    //
    // Set the status code as appropriate.
    //

    if (IdexIsFlagSet(IdeStatus, IDE_STATUS_ERR)) {
        Irp->IoStatus.Status = STATUS_IO_DEVICE_ERROR;
    } else {
        Irp->IoStatus.Status = STATUS_SUCCESS;
    }

    //
    // Finish processing the IRP at DPC level.
    //

    KeInsertQueueDpc(&IdexChannelObject.FinishDpc, NULL, NULL);
}

VOID
IdexCdRomTransferInterrupt(
    VOID
    )
/*++

Routine Description:

    This routine is invoked when a hardware interrupt occurs on the channel's
    IRQ and the pending interrupt IRP is for an ATA request that transfers data.

Arguments:

    None.

Return Value:

    None.

--*/
{
    UCHAR BusMasterStatus;
    UCHAR IdeStatus;
    UCHAR InterruptReason;
    PIRP Irp;

    //
    // Read the bus master status register and verify that the interrupt bit is
    // set if we haven't already seen the bus master interrupt.
    //

    BusMasterStatus = IdexReadBusMasterStatusPort();

    //
    // Read the status register to dismiss the interrupt.
    //

    IdeStatus = IdexReadStatusPort();

    //
    // Verify that the interrupt reason indicates that the command is complete.
    //

    InterruptReason = IdexReadInterruptReasonPort();

    if (IdexChannelObject.ExpectingBusMasterInterrupt) {

        if (IdexIsFlagClear(BusMasterStatus, IDE_BUS_MASTER_STATUS_INTERRUPT)) {
            return;
        }

        //
        // Deactivate the bus master interface and dismiss the interrupt.
        //

        IdexWriteBusMasterCommandPort(0);
        IdexWriteBusMasterStatusPort(IDE_BUS_MASTER_STATUS_INTERRUPT);

        IdexChannelObject.ExpectingBusMasterInterrupt = FALSE;
    }

    if ((InterruptReason & (IDE_INTERRUPT_REASON_IO | IDE_INTERRUPT_REASON_CD)) !=
        (IDE_INTERRUPT_REASON_IO | IDE_INTERRUPT_REASON_CD)) {
        return;
    }

    //
    // Clear the interrupt routine and grab the current IRP.
    //

    IdexChannelObject.InterruptRoutine = NULL;
    Irp = IdexChannelObject.CurrentIrp;

    //
    // Set the status code as appropriate.
    //

    if (IdexIsFlagSet(IdeStatus, IDE_STATUS_ERR) ||
        IdexIsFlagSet(BusMasterStatus, IDE_BUS_MASTER_STATUS_ERROR)) {
        Irp->IoStatus.Status = STATUS_IO_DEVICE_ERROR;
    } else if (IdexIsFlagSet(BusMasterStatus, IDE_BUS_MASTER_STATUS_ACTIVE)) {
        Irp->IoStatus.Status = STATUS_DATA_OVERRUN;
    } else {
        Irp->IoStatus.Status = STATUS_SUCCESS;
    }

    //
    // Finish processing the IRP at DPC level.
    //

    KeInsertQueueDpc(&IdexChannelObject.FinishDpc, NULL, NULL);
}

VOID
IdexCdRomIssueAtapiRequest(
    IN PCDB Cdb,
    IN PVOID TransferBuffer,
    IN ULONG TransferLength,
    IN BOOLEAN DataOut,
    IN PIDE_FINISHIO_ROUTINE FinishIoRoutine
    )
/*++

Routine Description:

    This routine issues the supplied ATAPI request.

Arguments:

    Cdb - Specifies the SCSI CDB to issue.

    TransferBuffer - Specifies the pointer to the input or output buffer.

    TransferLength - Specifies the number of bytes to transfer.

    DataOut - Specifies the direction of the data transfer.

    FinishIoRoutine - Specifies the routine to invoke on completion of the ATAPI
        request.

Return Value:

    None.

--*/
{
    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Select the IDE device and spin until the device is not busy.
    //

    if (!IdexCdRomSelectDeviceAndSpinWhileBusy()) {
        FinishIoRoutine();
        return;
    }

    //
    // Prepare the bus master interface for the DMA transfer.
    //

    if (TransferLength != 0) {
        IdexChannelPrepareBufferTransfer(TransferBuffer, TransferLength);
        IdexWriteFeaturesPort(IDE_FEATURE_DMA);
    } else {
        IdexWriteFeaturesPort(0);
    }

    //
    // Write out the IDE command and the SCSI CDB.
    //

    if (!IdexCdRomWritePacket(Cdb)) {
        FinishIoRoutine();
        return;
    }

    //
    // Activate the bus master interface.
    //

    if (TransferLength != 0) {
        if (DataOut) {
            IdexWriteBusMasterCommandPort(IDE_BUS_MASTER_COMMAND_START);
        } else {
            IdexWriteBusMasterCommandPort(IDE_BUS_MASTER_COMMAND_START |
                IDE_BUS_MASTER_COMMAND_READ);
        }
    }

    //
    // Indicate that we're expecting an interrupt for this IRP.
    //

    ASSERT(IdexChannelObject.InterruptRoutine == NULL);

    IdexChannelObject.InterruptRoutine = (TransferLength != 0) ?
        IdexCdRomTransferInterrupt : IdexCdRomNoTransferInterrupt;
    IdexChannelObject.FinishIoRoutine = FinishIoRoutine;
    IdexChannelObject.ExpectingBusMasterInterrupt = TRUE;
    IdexChannelObject.Timeout = IDE_ATAPI_DEFAULT_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

VOID
IdexCdRomSetSpindleSpeed(
    IN ULONG SpindleSpeedControl,
    IN PIDE_FINISHIO_ROUTINE FinishIoRoutine
    )
/*++

Routine Description:

    This routine prepares a spindle speed ATAPI request and issues the request.

Arguments:

    SpindleSpeedControl - Specifies the desired speed of the spindle.

    FinishIoRoutine - Specifies the routine to invoke on completion of the ATAPI
        request.

Return Value:

    None.

--*/
{
    PDVDX2_ADVANCED_DRIVE_CONTROL AdvancedDriveControl;
    CDB Cdb;

    ASSERT(SpindleSpeedControl <= IdexCdRomMaximumSpindleSpeed);

    IdexDbgPrint(("IDEX: setting spindle speed to %d.\n", SpindleSpeedControl));

    //
    // Change the current spindle speed to the requested spindle speed.  Note
    // that if the drive fails to handle the below ATAPI request, the software
    // state won't match the hardware state, but that's not likely to occur and
    // won't cause any problems for the drive or driver, so we'll ignore that
    // possibility.
    //

    IdexCdRomCurrentSpindleSpeed = SpindleSpeedControl;

    //
    // If spindle speed change is part of the error recovery path in this driver
    // and not an explicit call to IOCTL_CDROM_SET_SPINDLE_SPEED, then we need
    // to keep track of the number of sectors where we'll apply the new spindle
    // speed before we increase to the new spindle speed.
    //

    IdexCdRomSpindleSlowdownSectorsRemaining =
        IdexCdRomSpindleSpeedupTable[SpindleSpeedControl];

    //
    // Prepare the advanced drive control page.
    //

    ASSERT(sizeof(IdexCdRomStaticTransferBuffer) >= sizeof(DVDX2_ADVANCED_DRIVE_CONTROL));

    AdvancedDriveControl = (PDVDX2_ADVANCED_DRIVE_CONTROL)IdexCdRomStaticTransferBuffer;

    RtlZeroMemory(AdvancedDriveControl, sizeof(DVDX2_ADVANCED_DRIVE_CONTROL));

    *((PUSHORT)&AdvancedDriveControl->Header.ModeDataLength) =
        IdexConstantUshortByteSwap(sizeof(DVDX2_ADVANCED_DRIVE_CONTROL) -
        FIELD_OFFSET(DVDX2_ADVANCED_DRIVE_CONTROL, Header.MediumType));
    AdvancedDriveControl->AdvancedDriveControlPage.PageCode =
        DVDX2_MODE_PAGE_ADVANCED_DRIVE_CONTROL;
    AdvancedDriveControl->AdvancedDriveControlPage.PageLength =
        sizeof(DVDX2_ADVANCED_DRIVE_CONTROL_PAGE) -
        FIELD_OFFSET(DVDX2_ADVANCED_DRIVE_CONTROL_PAGE, SpindleSpeedControl);
    AdvancedDriveControl->AdvancedDriveControlPage.SpindleSpeedControl =
        (UCHAR)SpindleSpeedControl;

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.MODE_SENSE10.OperationCode = SCSIOP_MODE_SELECT10;
    Cdb.MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_ADVANCED_DRIVE_CONTROL;
    *((PUSHORT)&Cdb.MODE_SENSE10.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_ADVANCED_DRIVE_CONTROL));

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, IdexCdRomStaticTransferBuffer,
        sizeof(DVDX2_ADVANCED_DRIVE_CONTROL), TRUE, FinishIoRoutine);
}

VOID
IdexCdRomFinishSpeedReduction(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing after reducing
    the spindle speed as a result of an error from the drive.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // Restart the current packet in around 300ms.
    //

    IdexChannelSetTimerPeriod(IDE_FAST_TIMER_PERIOD);

    IdexChannelObject.TimeoutExpiredRoutine = IdexChannelRestartCurrentPacket;
    IdexChannelObject.Timeout = 3;
}

BOOLEAN
IdexCdRomSpeedReductionRequested(
    VOID
    )
/*++

Routine Description:

    This routine is invoked when the drive has returned a sense code indicating
    that cache errors have been detected and a speed reduction is requested.

    If a speed reduction is initiated, then the current IRP will be retried at
    the lower spindle speed.

Arguments:

    None.

Return Value:

    Returns TRUE if a speed reduction ATAPI request has been issued, else FALSE.

--*/
{
    PIO_STACK_LOCATION IrpSp;

    //
    // If the current spindle speed is already at the minimum value, then do
    // nothing.
    //

    if (IdexCdRomCurrentSpindleSpeed != DVDX2_SPINDLE_SPEED_MINIMUM) {

        //
        // If this is not a read request, then do nothing.
        //

        IrpSp = IoGetCurrentIrpStackLocation(IdexChannelObject.CurrentIrp);

        if (IrpSp->MajorFunction == IRP_MJ_READ) {

            //
            // Remember the sector number where we last slowed down the drive.
            //

            IdexCdRomSpindleSlowdownSectorNumber =
                IrpSp->Parameters.IdexReadWrite.SectorNumber;

            //
            // Reduce the spindle speed by another notch and then restart the
            // current packet.
            //

            IdexCdRomSetSpindleSpeed(IdexCdRomCurrentSpindleSpeed - 1,
                IdexCdRomFinishSpeedReduction);

            return TRUE;
        }
    }

    return FALSE;
}

VOID
IdexCdRomFinishSpindleSpeedRestore(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing after restoring
    the spindle speed to the maximum value.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    PIO_STACK_LOCATION IrpSp;

    //
    // Don't use IdexChannelRestartCurrentPacket in order to avoid incrementing
    // the number of retries for this packet.
    //

    Irp = IdexChannelObject.CurrentIrp;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Invoke the driver's StartIo routine to start the IRP.
    //

    IrpSp->DeviceObject->DriverObject->DriverStartIo(IrpSp->DeviceObject, Irp);
}

BOOLEAN
IdexCdRomCheckForRestoreSpindleSpeed(
    IN ULONG StartingSectorNumber,
    IN ULONG NumberOfSectors
    )
/*++

Routine Description:

    This routine checks if the supplied sector number is outside of the range of
    sectors that caused the spindle speed of the drive to be reduced.

Arguments:

    StartingSectorNumber - Specifies the starting sector number of the current
        read request.

    NumberOfSectors - Specifies the number of sectors for the current read
        request.

Return Value:

    Returns TRUE if the spindle speed is being restored, else FALSE.

--*/
{
    ASSERT(IdexCdRomCurrentSpindleSpeed < IdexCdRomMaximumSpindleSpeed);

    //
    // Reduce the number of sectors that must be read before attempting to
    // increase the spindle speed.  This check is to prevent scenerios where
    // reading the same block of data over and over keeps the spindle speed from
    // ever being restored.
    //

    if (NumberOfSectors < IdexCdRomSpindleSlowdownSectorsRemaining) {
        IdexCdRomSpindleSlowdownSectorsRemaining -= NumberOfSectors;
    } else {
        IdexCdRomSpindleSlowdownSectorsRemaining = 0;
    }

    //
    // Check if we're still inside the sector range that caused us to last
    // slowdown and that we haven't run the count of transfered sectors down to
    // zero.
    //

    if ((IdexCdRomSpindleSlowdownSectorsRemaining != 0) &&
        (StartingSectorNumber >= IdexCdRomSpindleSlowdownSectorNumber) &&
        (StartingSectorNumber < IdexCdRomSpindleSlowdownSectorNumber +
            IdexCdRomSpindleSpeedupTable[IdexCdRomCurrentSpindleSpeed])) {
        return FALSE;
    }

    //
    // Remember the sector number where we last speed up the drive.
    //

    IdexCdRomSpindleSlowdownSectorNumber = StartingSectorNumber;

    //
    // We're outside of the slowdown sector range.  Attempt to restore the
    // spindle speed back to the maximum value.
    //

    IdexCdRomSetSpindleSpeed(IdexCdRomCurrentSpindleSpeed + 1,
        IdexCdRomFinishSpindleSpeedRestore);

    return TRUE;
}

VOID
IdexCdRomFinishRequestSense(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing a request to read
    the sense data from the drive.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    PSENSE_DATA SenseData;
    NTSTATUS status;
    BOOLEAN Retry;
    BOOLEAN DelayedRetry;
    BOOLEAN AdjustSpindleSpeed;
    PIO_STACK_LOCATION IrpSp;

    Irp = IdexChannelObject.CurrentIrp;

#if DBG
    //
    // Keep track of the number of errors that have occurred at the various
    // spindle speeds.
    //

    IdexCdRomDebugErrors[IdexCdRomCurrentSpindleSpeed]++;
#endif

    //
    // If we're unable to get the correct sense information, give up and just
    // complete the IRP with STATUS_IO_DEVICE_ERROR.
    //

    if (!NT_SUCCESS(Irp->IoStatus.Status)) {
        Irp->IoStatus.Status = STATUS_IO_DEVICE_ERROR;
        goto CompleteRequest;
    }

    //
    // Interpret and process the sense data.
    //

    SenseData = (PSENSE_DATA)IdexCdRomStaticTransferBuffer;
    status = STATUS_IO_DEVICE_ERROR;
    Retry = FALSE;
    DelayedRetry = FALSE;
    AdjustSpindleSpeed = FALSE;

    switch (SenseData->SenseKey) {

        case SCSI_SENSE_NO_SENSE:
            if (SenseData->IncorrectLength) {
                status = STATUS_INVALID_BLOCK_LENGTH;
            } else {
                Retry = TRUE;
            }
            break;

        case SCSI_SENSE_RECOVERED_ERROR:
            if (SenseData->IncorrectLength) {
                status = STATUS_INVALID_BLOCK_LENGTH;
            } else if (SenseData->AdditionalSenseCode ==
                DVDX2_ADSENSE_SPEED_REDUCTION_REQUESTED) {
                Retry = TRUE;
                AdjustSpindleSpeed = TRUE;
            } else {
                status = STATUS_SUCCESS;
            }
            break;

        case SCSI_SENSE_NOT_READY:
            status = STATUS_DEVICE_NOT_READY;

            switch (SenseData->AdditionalSenseCode) {

                case SCSI_ADSENSE_LUN_NOT_READY:
                    Retry = TRUE;
                    DelayedRetry = TRUE;
                    break;

                case SCSI_ADSENSE_INVALID_MEDIA:
                    status = STATUS_UNRECOGNIZED_MEDIA;
                    break;

                case SCSI_ADSENSE_NO_MEDIA_IN_DEVICE:
                    status = STATUS_NO_MEDIA_IN_DEVICE;
                    break;
            }
            break;

        case SCSI_SENSE_MEDIUM_ERROR:
            status = STATUS_DEVICE_DATA_ERROR;

            switch (SenseData->AdditionalSenseCode) {

                case SCSI_ADSENSE_INVALID_MEDIA:
                    status = STATUS_UNRECOGNIZED_MEDIA;
                    break;

                case DVDX2_ADSENSE_GENERAL_READ_ERROR:
                    Retry = TRUE;

                    if (IdexChannelObject.IoRetries != 0) {
                        AdjustSpindleSpeed = TRUE;
                    }

                    break;
            }
            break;

        case SCSI_SENSE_ILLEGAL_REQUEST:
            status = STATUS_INVALID_DEVICE_REQUEST;

            switch (SenseData->AdditionalSenseCode) {

                case SCSI_ADSENSE_ILLEGAL_BLOCK:
                    status = STATUS_NONEXISTENT_SECTOR;
                    break;

                case SCSI_ADSENSE_INVALID_MEDIA:
                    status = STATUS_UNRECOGNIZED_MEDIA;
                    break;

                case SCSI_ADSENSE_COPY_PROTECTION_FAILURE:
                case DVDX2_ADSENSE_COPY_PROTECTION_FAILURE:
                    status = STATUS_TOO_MANY_SECRETS;
                    break;

                case DVDX2_ADSENSE_COMMAND_ERROR:
                    Retry = TRUE;
                    break;
            }
            break;

        case SCSI_SENSE_UNIT_ATTENTION:
            Retry = TRUE;

            if (SenseData->AdditionalSenseCode == DVDX2_ADSENSE_INSUFFICIENT_TIME) {
                AdjustSpindleSpeed = TRUE;
            }

            break;

        default:
            Retry = TRUE;
            break;
    }

    Irp->IoStatus.Status = status;

#if DBG
    //
    // Keep a copy of the the last sense data for debugging purposes.
    //

    RtlCopyMemory(&IdexCdRomDebugSenseData, SenseData, sizeof(SENSE_DATA));
#endif

    //
    // Retry the request if possible.
    //

    if (Retry &&
        (IdexChannelObject.IoRetries < IdexChannelObject.MaximumIoRetries)) {

        //
        // If this isn't a retry associated with a spindle speed reduction, then
        // restart the current packet and bail out now.
        //

        if (!AdjustSpindleSpeed) {

            if (!DelayedRetry) {
                IdexChannelRestartCurrentPacket();
                return;
            }

            //
            // Restart the current packet in around 800ms.
            //

            IdexChannelSetTimerPeriod(IDE_FAST_TIMER_PERIOD);

            IdexChannelObject.TimeoutExpiredRoutine = IdexChannelRestartCurrentPacket;
            IdexChannelObject.Timeout = 8;

            return;
        }

        //
        // Otherwise, try to slow down the spindle.  If the spindle cannot be
        // slowed down, then abandon the retry and fall into the non-retry path.
        //

        if (IdexCdRomSpeedReductionRequested()) {
            return;
        }
    }

    //
    // If the request is a SCSI pass through request and the caller has supplied
    // an output buffer, then copy the sense data to the caller's output buffer
    // and return success.  The caller checks the number of bytes written to the
    // output buffer to know whether or not an error has actually occurred.
    //

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    if ((IrpSp->MajorFunction == IRP_MJ_DEVICE_CONTROL) &&
        (IrpSp->Parameters.DeviceIoControl.IoControlCode ==
            IOCTL_SCSI_PASS_THROUGH_DIRECT) &&
        (IrpSp->Parameters.DeviceIoControl.OutputBufferLength != 0)) {

        RtlCopyMemory(Irp->UserBuffer, SenseData, sizeof(SENSE_DATA));

        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(SENSE_DATA);
    }

    //
    // The request cannot be retried or the retry count has exceeded the limit, so
    // complete the IRP and start the next request.
    //

CompleteRequest:
    IoCompleteRequest(Irp, IO_CD_ROM_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
IdexCdRomFinishGeneric(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to generically finish processing a
    request after its hardware interrupt has fired or after the IDE command has
    timed out.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    CDB Cdb;

    Irp = IdexChannelObject.CurrentIrp;

    if (Irp->IoStatus.Status == STATUS_IO_DEVICE_ERROR) {

        //
        // The command completed with an error.  Request the sense data so that
        // we can return a more useful error to the caller.
        //

        RtlZeroMemory(&Cdb, sizeof(CDB));

        Cdb.CDB6INQUIRY.OperationCode = SCSIOP_REQUEST_SENSE;
        Cdb.CDB6INQUIRY.AllocationLength = sizeof(SENSE_DATA);

        IdexCdRomIssueAtapiRequest(&Cdb, IdexCdRomStaticTransferBuffer,
            sizeof(SENSE_DATA), FALSE, IdexCdRomFinishRequestSense);

        return;

    } else if (Irp->IoStatus.Status == STATUS_IO_TIMEOUT) {

        //
        // Attempt to reset the device.  If the reset completes successfully and
        // the retry count has not been exceeded the maximum retry count, then
        // the IRP will be restarted.
        //

        IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

        IdexCdRomResetDevice();

        return;
    }

    IoCompleteRequest(Irp, IO_CD_ROM_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
IdexCdRomFinishGenericWithOverrun(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to generically finish processing a
    request after its hardware interrupt has fired or after the IDE command has
    timed out.

    If the request completed with STATUS_DATA_OVERRUN, then this is converted
    to STATUS_SUCCESS.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    Irp = IdexChannelObject.CurrentIrp;

    //
    // The media may not have enough data to fill the output buffer, but this
    // isn't an error.
    //

    if (Irp->IoStatus.Status == STATUS_DATA_OVERRUN) {
        Irp->IoStatus.Status = STATUS_SUCCESS;
    }

    //
    // Jump to the generic handler to process errors.
    //

    if (!NT_SUCCESS(Irp->IoStatus.Status)) {
        IdexCdRomFinishGeneric();
        return;
    }

    //
    // Complete the request and start the next packet.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_CD_ROM_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
FASTCALL
IdexCdRomStartReadTOC(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_CDROM_READ_TOC and
    IOCTL_CDROM_GET_LAST_SESSION requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    ULONG TransferLength;
    CDB Cdb;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    TransferLength = IrpSp->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // Verify that the output buffer and length is non-zero and has the proper
    // alignment requirement.
    //

    if ((TransferLength == 0) ||
        ((TransferLength & IDE_ALIGNMENT_REQUIREMENT) != 0) ||
        ((PtrToUlong(Irp->UserBuffer) & IDE_ALIGNMENT_REQUIREMENT) != 0)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // The maximum transfer length for the TOC is sizeof(CDROM_TOC).
    //

    if (TransferLength > sizeof(CDROM_TOC)) {
        TransferLength = sizeof(CDROM_TOC);
    }

    //
    // Lock the user's buffer into memory if necessary.
    //

    IoLockUserBuffer(Irp, TransferLength);

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = TransferLength;

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.READ_TOC.OperationCode = SCSIOP_READ_TOC;
    *((PUSHORT)Cdb.READ_TOC.AllocationLength) = RtlUshortByteSwap((USHORT)TransferLength);

    if (IrpSp->Parameters.DeviceIoControl.IoControlCode == IOCTL_CDROM_READ_TOC) {
        Cdb.READ_TOC.Msf = CDB_USE_MSF;
    } else {
        Cdb.READ_TOC.Format = GET_LAST_SESSION;
    }

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, Irp->UserBuffer, TransferLength, FALSE,
        IdexCdRomFinishGenericWithOverrun);
}

VOID
FASTCALL
IdexCdRomStartCheckVerify(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_CDROM_CHECK_VERIFY requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    CDB Cdb;

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = 0;

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.CDB6GENERIC.OperationCode = SCSIOP_TEST_UNIT_READY;

    //
    // No retries are allowed for this IRP.
    //

    IdexChannelObject.MaximumIoRetries = IDE_NO_RETRY_COUNT;

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, NULL, 0, FALSE, IdexCdRomFinishGeneric);
}

VOID
IdexCdRomFinishGetDriveGeometry(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing a
    IOCTL_CDROM_GET_DRIVE_GEOMETRY request.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    PREAD_CAPACITY_DATA ReadCapacity;
    ULONG NumberOfSectors;
    PDISK_GEOMETRY DiskGeometry;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    Irp = IdexChannelObject.CurrentIrp;

    //
    // Jump to the generic handler to process errors.
    //

    if (!NT_SUCCESS(Irp->IoStatus.Status)) {
        IdexCdRomFinishGeneric();
        return;
    }

    //
    // Compute the drive's geometry.
    //

    ReadCapacity = (PREAD_CAPACITY_DATA)IdexCdRomStaticTransferBuffer;
    NumberOfSectors = RtlUlongByteSwap(ReadCapacity->LogicalBlockAddress) + 1;
    IdexCdRomPartitionLength = (ULONGLONG)NumberOfSectors * IDE_ATAPI_CD_SECTOR_SIZE;

    DiskGeometry = (PDISK_GEOMETRY)Irp->UserBuffer;
    DiskGeometry->Cylinders.QuadPart = NumberOfSectors;
    DiskGeometry->MediaType = RemovableMedia;
    DiskGeometry->TracksPerCylinder = 1;
    DiskGeometry->SectorsPerTrack = 1;
    DiskGeometry->BytesPerSector = IDE_ATAPI_CD_SECTOR_SIZE;

    //
    // Complete the request and start the next packet.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_CD_ROM_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
FASTCALL
IdexCdRomStartGetDriveGeometry(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_CDROM_GET_DRIVE_GEOMETRY requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    CDB Cdb;

    ASSERT(sizeof(IdexCdRomStaticTransferBuffer) >= sizeof(READ_CAPACITY_DATA));

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.CDB6GENERIC.OperationCode = SCSIOP_READ_CAPACITY;

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, IdexCdRomStaticTransferBuffer,
        sizeof(READ_CAPACITY_DATA), FALSE, IdexCdRomFinishGetDriveGeometry);
}

VOID
FASTCALL
IdexCdRomStartRawRead(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_CDROM_RAW_READ requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PRAW_READ_INFO RawReadInfo;
    ULONG StartingSector;
    ULONG TransferLength;
    CDB Cdb;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength != sizeof(RAW_READ_INFO)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    RawReadInfo = (PRAW_READ_INFO)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Compute the logical block address and transfer length for the request.
    //

    StartingSector = (ULONG)(RawReadInfo->DiskOffset.QuadPart >>
        IDE_ATAPI_CD_SECTOR_SHIFT);
    TransferLength = RawReadInfo->SectorCount * IDE_ATAPI_RAW_CD_SECTOR_SIZE;

    //
    // Verify that the parameters are valid.  The requested read length must be
    // less than the maximum transfer size, the ending offset must be in within
    // the bounds of the disk, the output buffer must have enough bytes to
    // contain the read length, and the output buffer must have the proper
    // alignment requirement.
    //

    if ((RawReadInfo->SectorCount > (IDE_ATAPI_MAXIMUM_TRANSFER_BYTES / IDE_ATAPI_RAW_CD_SECTOR_SIZE) + 1) ||
        (TransferLength > IDE_ATAPI_MAXIMUM_TRANSFER_BYTES) ||
        ((RawReadInfo->DiskOffset.QuadPart + TransferLength) > IdexCdRomPartitionLength) ||
        ((PtrToUlong(Irp->UserBuffer) & IDE_ALIGNMENT_REQUIREMENT) != 0)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = TransferLength;

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.READ_CD.OperationCode = SCSIOP_READ_CD;
    *((PULONG)Cdb.READ_CD.StartingLBA) = RtlUlongByteSwap(StartingSector);
    Cdb.READ_CD.TransferBlocks[2] = (UCHAR)(RawReadInfo->SectorCount);
    Cdb.READ_CD.TransferBlocks[1] = (UCHAR)(RawReadInfo->SectorCount >> 8);
    Cdb.READ_CD.TransferBlocks[0] = (UCHAR)(RawReadInfo->SectorCount >> 16);

    switch (RawReadInfo->TrackMode) {

        case CDDA:
            Cdb.READ_CD.ExpectedSectorType = 1;
            Cdb.READ_CD.IncludeUserData = 1;
            Cdb.READ_CD.HeaderCode = 3;
            Cdb.READ_CD.IncludeSyncData = 1;
            break;

        case YellowMode2:
            Cdb.READ_CD.ExpectedSectorType = 3;
            Cdb.READ_CD.IncludeUserData = 1;
            Cdb.READ_CD.HeaderCode = 1;
            Cdb.READ_CD.IncludeSyncData = 1;
            break;

        case XAForm2:
            Cdb.READ_CD.ExpectedSectorType = 5;
            Cdb.READ_CD.IncludeUserData = 1;
            Cdb.READ_CD.HeaderCode = 3;
            Cdb.READ_CD.IncludeSyncData = 1;
            break;

        default:
            IdexChannelInvalidParameterRequest(Irp);
            return;
    }

    //
    // No retries are allowed for this IRP.
    //

    IdexChannelObject.MaximumIoRetries = IDE_NO_RETRY_COUNT;

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, Irp->UserBuffer, TransferLength, FALSE,
        IdexCdRomFinishGeneric);
}

VOID
FASTCALL
IdexCdRomStartSetSpindleSpeed(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_CDROM_SET_SPINDLE_SPEED requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PULONG SpindleSpeedControl;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength != sizeof(ULONG)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    SpindleSpeedControl = (PULONG)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Verify that this is a valid spindle speed.
    //

    if (*SpindleSpeedControl > DVDX2_SPINDLE_SPEED_MAXIMUM) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Change the maximum spindle speed to the requested spindle speed.  Note
    // that if the drive fails to handle the below ATAPI request, the software
    // state won't match the hardware state, but that's not likely to occur and
    // won't cause any problems for the drive or driver, so we'll ignore that
    // possibility.
    //

    IdexCdRomMaximumSpindleSpeed = *SpindleSpeedControl;

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = 0;

    //
    // Issue the ATAPI spindle speed control request.
    //

    IdexCdRomSetSpindleSpeed(*SpindleSpeedControl, IdexCdRomFinishGeneric);
}

VOID
IdexCdRomFinishStartSession(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing a
    IOCTL_DVD_START_SESSION request.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    PCDVD_KEY_HEADER KeyHeader;
    PCDVD_REPORT_AGID_DATA ReportAGIDData;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    Irp = IdexChannelObject.CurrentIrp;

    //
    // Jump to the generic handler to process errors.
    //

    if (!NT_SUCCESS(Irp->IoStatus.Status)) {
        IdexCdRomFinishGeneric();
        return;
    }

    //
    // Copy the authentication grant ID (AGID) to the user's buffer.
    //

    KeyHeader = (PCDVD_KEY_HEADER)IdexCdRomStaticTransferBuffer;
    ReportAGIDData = (PCDVD_REPORT_AGID_DATA)KeyHeader->Data;
    *((PDVD_SESSION_ID)Irp->UserBuffer) = ReportAGIDData->AGID;

    //
    // Complete the request and start the next packet.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_CD_ROM_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
FASTCALL
IdexCdRomStartStartSession(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_DVD_START_SESSION requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    CDB Cdb;

    ASSERT(sizeof(IdexCdRomStaticTransferBuffer) >= sizeof(CDVD_KEY_HEADER) +
        sizeof(CDVD_REPORT_AGID_DATA));

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the output buffer is large enough to receive the data.
    //

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(DVD_SESSION_ID)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = sizeof(DVD_SESSION_ID);

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.REPORT_KEY.OperationCode = SCSIOP_REPORT_KEY;
    *((PUSHORT)Cdb.REPORT_KEY.AllocationLength) =
        IdexConstantUshortByteSwap(sizeof(CDVD_KEY_HEADER) + sizeof(CDVD_REPORT_AGID_DATA));

    ASSERT(Cdb.REPORT_KEY.KeyFormat == DVD_REPORT_AGID);
    ASSERT(Cdb.REPORT_KEY.AGID == 0);

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, IdexCdRomStaticTransferBuffer,
        sizeof(CDVD_KEY_HEADER) + sizeof(CDVD_REPORT_AGID_DATA), FALSE,
        IdexCdRomFinishStartSession);
}

VOID
IdexCdRomFinishReadKey(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing a
    IOCTL_DVD_READ_KEY request.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    PIO_STACK_LOCATION IrpSp;
    PCDVD_KEY_HEADER KeyHeader;
    PDVD_COPY_PROTECT_KEY CopyProtectKey;
    ULONG KeyDataLength;
    ULONG TransferLength;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    Irp = IdexChannelObject.CurrentIrp;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // The media may not have enough data to fill the output buffer, but this
    // isn't an error.
    //

    if (Irp->IoStatus.Status == STATUS_DATA_OVERRUN) {
        Irp->IoStatus.Status = STATUS_SUCCESS;
    }

    //
    // Jump to the generic handler to process errors.
    //

    if (!NT_SUCCESS(Irp->IoStatus.Status)) {
        IdexCdRomFinishGeneric();
        return;
    }

    KeyHeader = (PCDVD_KEY_HEADER)IdexCdRomStaticTransferBuffer;
    CopyProtectKey = (PDVD_COPY_PROTECT_KEY)Irp->UserBuffer;

    //
    // Compute the number of bytes that the device should have returned for the
    // given key format and the number of bytes that were actually transferred.
    // We're not returning the Reserved bytes in the CDVD_KEY_HEADER to the
    // caller, so subtract these from the length of the key.
    //

    KeyDataLength = RtlUshortByteSwap(*((PUSHORT)&KeyHeader->DataLength));

    if (KeyDataLength > sizeof(KeyHeader->Reserved)) {
        KeyDataLength -= sizeof(KeyHeader->Reserved);
    }

    TransferLength = IrpSp->Parameters.DeviceIoControl.OutputBufferLength -
        sizeof(DVD_COPY_PROTECT_KEY);

    if (KeyDataLength < TransferLength) {
        TransferLength = KeyDataLength;
    }

    //
    // Copy the input buffer's DVD_COPY_PROTECT_KEY structure to the output
    // buffer.  The I/O manager doesn't handle buffered I/O by using the same
    // input and output buffer, so we need to do this copy ourselves.
    //

    RtlCopyMemory(CopyProtectKey, IrpSp->Parameters.DeviceIoControl.InputBuffer,
        sizeof(DVD_COPY_PROTECT_KEY));

    //
    // Copy the key data to the output buffer as appropriate.
    //

    if (CopyProtectKey->KeyType == DvdTitleKey) {
        RtlCopyMemory(CopyProtectKey->KeyData, KeyHeader->Data + 1,
            TransferLength - 1);
        CopyProtectKey->KeyData[TransferLength - 1] = 0;
        CopyProtectKey->KeyFlags = KeyHeader->Data[0];
    } else {
        RtlCopyMemory(CopyProtectKey->KeyData, KeyHeader->Data, TransferLength);
    }

    CopyProtectKey->KeyLength = sizeof(DVD_COPY_PROTECT_KEY) + TransferLength;

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = CopyProtectKey->KeyLength;

    //
    // Complete the request and start the next packet.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_CD_ROM_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
FASTCALL
IdexCdRomStartReadKey(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_DVD_READ_KEY requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PDVD_COPY_PROTECT_KEY CopyProtectKey;
    ULONG TransferLength;
    ULONG LogicalBlockAddress;
    CDB Cdb;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(DVD_COPY_PROTECT_KEY)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    CopyProtectKey = (PDVD_COPY_PROTECT_KEY)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Verify that the output buffer is large enough to receive at least the
    // DVD_COPY_PROTECT_KEY header.
    //

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(DVD_COPY_PROTECT_KEY)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Compute the transfer length for the request.  The transfer length is
    // restricted to the static tranfer buffer length.
    //

    TransferLength = sizeof(CDVD_KEY_HEADER) +
        IrpSp->Parameters.DeviceIoControl.OutputBufferLength -
        sizeof(DVD_COPY_PROTECT_KEY);

    if (TransferLength > sizeof(IdexCdRomStaticTransferBuffer)) {
        IdexDbgPrint(("IDEX: transfer length exceeds static buffer length.\n"));
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.REPORT_KEY.OperationCode = SCSIOP_REPORT_KEY;
    *((PUSHORT)Cdb.REPORT_KEY.AllocationLength) =
        RtlUshortByteSwap((USHORT)TransferLength);
    Cdb.REPORT_KEY.KeyFormat = (UCHAR)CopyProtectKey->KeyType;
    Cdb.REPORT_KEY.AGID = (UCHAR)CopyProtectKey->SessionId;

    if (CopyProtectKey->KeyType == DvdTitleKey) {
        LogicalBlockAddress = (ULONG)(CopyProtectKey->Parameters.TitleOffset.QuadPart >>
            IDE_ATAPI_CD_SECTOR_SHIFT);
        *((PULONG)Cdb.REPORT_KEY.LogicalBlockAddress) =
            RtlUlongByteSwap(LogicalBlockAddress);
    }

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, IdexCdRomStaticTransferBuffer,
        TransferLength, FALSE, IdexCdRomFinishReadKey);
}

VOID
FASTCALL
IdexCdRomStartSendKey(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_DVD_SEND_KEY requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PDVD_COPY_PROTECT_KEY CopyProtectKey;
    ULONG TransferLength;
    PCDVD_KEY_HEADER KeyHeader;
    CDB Cdb;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(DVD_COPY_PROTECT_KEY)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    CopyProtectKey = (PDVD_COPY_PROTECT_KEY)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Compute the transfer length for the request.  The transfer length is
    // restricted to the static tranfer buffer length.
    //

    TransferLength = CopyProtectKey->KeyLength - sizeof(DVD_COPY_PROTECT_KEY) +
        sizeof(CDVD_KEY_HEADER);

    if (TransferLength > sizeof(IdexCdRomStaticTransferBuffer)) {
        IdexDbgPrint(("IDEX: transfer length exceeds static buffer length.\n"));
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = 0;

    //
    // Prepare the transfer buffer.
    //

    KeyHeader = (PCDVD_KEY_HEADER)IdexCdRomStaticTransferBuffer;
    *((PUSHORT)KeyHeader->DataLength) = RtlUshortByteSwap((USHORT)(TransferLength -
        FIELD_OFFSET(CDVD_KEY_HEADER, Reserved)));
    RtlZeroMemory(KeyHeader->Reserved, sizeof(KeyHeader->Reserved));
    RtlCopyMemory(KeyHeader->Data, CopyProtectKey->KeyData, TransferLength -
        sizeof(CDVD_KEY_HEADER));

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.SEND_KEY.OperationCode = SCSIOP_SEND_KEY;
    *((PUSHORT)Cdb.SEND_KEY.ParameterListLength) =
        RtlUshortByteSwap((USHORT)TransferLength);
    Cdb.SEND_KEY.KeyFormat = (UCHAR)CopyProtectKey->KeyType;
    Cdb.SEND_KEY.AGID = (UCHAR)CopyProtectKey->SessionId;

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, IdexCdRomStaticTransferBuffer,
        TransferLength, TRUE, IdexCdRomFinishGeneric);
}

VOID
FASTCALL
IdexCdRomStartEndSession(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_DVD_END_SESSION requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PDVD_SESSION_ID SessionId;
    CDB Cdb;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(DVD_SESSION_ID)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    SessionId = (PDVD_SESSION_ID)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Verify that the output buffer is large enough to receive the data.
    //

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(DVD_SESSION_ID)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = 0;

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.SEND_KEY.OperationCode = SCSIOP_SEND_KEY;
    Cdb.SEND_KEY.AGID = (UCHAR)*SessionId;
    Cdb.SEND_KEY.KeyFormat = DVD_INVALIDATE_AGID;

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, NULL, 0, FALSE, IdexCdRomFinishGeneric);
}

VOID
IdexCdRomStartReadStructure(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_DVD_READ_STRUCTURE requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PDVD_READ_STRUCTURE ReadStructure;
    ULONG TransferLength;
    ULONG MinimumTransferLength;
    ULONG LogicalBlockAddress;
    CDB Cdb;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(DVD_READ_STRUCTURE)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    ReadStructure = (PDVD_READ_STRUCTURE)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Verify that the output buffer is large enough to receive the data, but
    // isn't too large to overflow READ_DVD_STRUCTURES_HEADER.Length.  The read
    // length must have the proper alignment.
    //

    TransferLength = IrpSp->Parameters.DeviceIoControl.OutputBufferLength;

    if (ReadStructure->Format == DvdPhysicalDescriptor) {
        MinimumTransferLength = sizeof(DVD_DESCRIPTOR_HEADER) +
            sizeof(DVD_LAYER_DESCRIPTOR);
    } else {
        MinimumTransferLength = sizeof(DVD_DESCRIPTOR_HEADER);
    }

    if ((TransferLength < MinimumTransferLength) ||
        (TransferLength > MAXUSHORT) ||
        ((TransferLength & IDE_ALIGNMENT_REQUIREMENT) != 0)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Compute the logical block address.
    //

    LogicalBlockAddress = (ULONG)(ReadStructure->BlockByteOffset.QuadPart >>
        IDE_ATAPI_CD_SECTOR_SHIFT);

    //
    // Lock the user's buffer into memory if necessary.
    //

    IoLockUserBuffer(Irp, TransferLength);

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = TransferLength;

    //
    // Prepare the SCSI CDB.
    //

    Cdb.READ_DVD_STRUCTURE.OperationCode = SCSIOP_READ_DVD_STRUCTURE;
    *((PUSHORT)Cdb.READ_DVD_STRUCTURE.AllocationLength) =
        RtlUshortByteSwap((USHORT)TransferLength);
    *((PULONG)Cdb.READ_DVD_STRUCTURE.RMDBlockNumber) =
        RtlUlongByteSwap(LogicalBlockAddress);
    Cdb.READ_DVD_STRUCTURE.LayerNumber = ReadStructure->LayerNumber;
    Cdb.READ_DVD_STRUCTURE.Format = (UCHAR)ReadStructure->Format;

    if (IdexIsFlagSet(ReadStructure->LayerNumber, 0x80)) {
        Cdb.READ_DVD_STRUCTURE.Control = 0xC0;
    }

    if (ReadStructure->Format == DvdDiskKeyDescriptor) {
        Cdb.READ_DVD_STRUCTURE.AGID = (UCHAR)ReadStructure->SessionId;
    }

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest(&Cdb, Irp->UserBuffer, TransferLength, FALSE,
        IdexCdRomFinishGenericWithOverrun);
}

VOID
IdexCdRomFinishScsiPassThrough(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing a
    IOCTL_SCSI_PASS_THROUGH_DIRECT request.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    PIO_STACK_LOCATION IrpSp;
    PSCSI_PASS_THROUGH_DIRECT PassThrough;

    Irp = IdexChannelObject.CurrentIrp;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    PassThrough =
        (PSCSI_PASS_THROUGH_DIRECT)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Manually unlock the caller's data buffer if IdexCdRomStartScsiPassThrough
    // locked down the buffer.
    //

    if (PassThrough->DataTransferLength != 0) {
        MmLockUnlockBufferPages(PassThrough->DataBuffer,
            PassThrough->DataTransferLength, TRUE);
    }

    //
    // Jump to the generic handler.
    //

    IdexCdRomFinishGeneric();
}

VOID
FASTCALL
IdexCdRomStartScsiPassThrough(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_SCSI_PASS_THROUGH_DIRECT requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PSCSI_PASS_THROUGH_DIRECT PassThrough;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the output buffer is either empty or large enough to receive
    // the sense data.
    //

    if ((IrpSp->Parameters.DeviceIoControl.OutputBufferLength != 0) &&
        (IrpSp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(SENSE_DATA))) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(SCSI_PASS_THROUGH_DIRECT)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    PassThrough =
        (PSCSI_PASS_THROUGH_DIRECT)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Perform minimal verification of the contents of the pass through data
    // structure and the data transfer buffer.
    //

    if ((PassThrough->Length != sizeof(SCSI_PASS_THROUGH_DIRECT)) ||
        (PassThrough->DataTransferLength > IDE_ATAPI_MAXIMUM_TRANSFER_BYTES) ||
        (((ULONG_PTR)PassThrough->DataBuffer & IDE_ALIGNMENT_REQUIREMENT) != 0) ||
        ((PassThrough->DataTransferLength & IDE_ALIGNMENT_REQUIREMENT) != 0) ||
        (PassThrough->DataTransferLength > IDE_ATAPI_MAXIMUM_TRANSFER_BYTES)) {
        IdexChannelInvalidParameterRequest(Irp);
        return;
    }

    //
    // If the caller is expecting data to be transferred, then manually lock
    // down the pages.
    //

    if (PassThrough->DataTransferLength != 0) {
        MmLockUnlockBufferPages(PassThrough->DataBuffer,
            PassThrough->DataTransferLength, FALSE);
    }

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = 0;

    //
    // Issue the ATAPI request.
    //

    IdexCdRomIssueAtapiRequest((PCDB)&PassThrough->Cdb, PassThrough->DataBuffer,
        PassThrough->DataTransferLength, (BOOLEAN)(PassThrough->DataIn ==
        SCSI_IOCTL_DATA_OUT), IdexCdRomFinishScsiPassThrough);
}

VOID
IdexCdRomFinishRead(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing a IRP_MJ_READ
    request.

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

#if DBG
    //
    // Keep track of the number of reads that have been completed.
    //

    IdexCdRomDebugReadsFinished++;
#endif

    //
    // Jump to the generic handler to process errors.
    //

    if (!NT_SUCCESS(Irp->IoStatus.Status)) {
        IdexCdRomFinishGeneric();
        return;
    }

    //
    // If there are still more bytes to transfer in this request, then adjust
    // the transfer counts and requeue the packet.
    //

    if (IrpSp->Parameters.IdexReadWrite.Length > IDE_ATAPI_MAXIMUM_TRANSFER_BYTES) {

        IrpSp->Parameters.IdexReadWrite.Length -= IDE_ATAPI_MAXIMUM_TRANSFER_BYTES;
        IrpSp->Parameters.IdexReadWrite.BufferOffset += IDE_ATAPI_MAXIMUM_TRANSFER_BYTES;
        IrpSp->Parameters.IdexReadWrite.SectorNumber += IDE_ATAPI_CD_MAXIMUM_TRANSFER_SECTORS;

        IdexChannelStartPacket(IrpSp->DeviceObject, Irp);
        IdexChannelStartNextPacket();
        return;
    }

    //
    // Complete the request and start the next packet.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_CD_ROM_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
IdexCdRomStartRead(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IRP_MJ_READ and IRP_MJ_WRITE requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    ULONG TransferLength;
    ULONG NumberOfSectors;
    CDB Cdb;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // If the number of bytes remaining is greater than the maximum transfer
    // size allowed by the hardware, then clip the number of bytes to the
    // maximum.
    //

    TransferLength = IrpSp->Parameters.IdexReadWrite.Length;

    if (TransferLength > IDE_ATAPI_MAXIMUM_TRANSFER_BYTES) {
        TransferLength = IDE_ATAPI_MAXIMUM_TRANSFER_BYTES;
    }

    NumberOfSectors = (TransferLength >> IDE_ATAPI_CD_SECTOR_SHIFT);

    //
    // Check if the current spindle speed is less than the maximum speed and if
    // the spindle speed can be restored to normal.  If so, then this will have
    // issued an ATAPI request that on completion, will cause this routine to be
    // called again.
    //

    if ((IdexCdRomCurrentSpindleSpeed < IdexCdRomMaximumSpindleSpeed) &&
        IdexCdRomCheckForRestoreSpindleSpeed(IrpSp->Parameters.IdexReadWrite.SectorNumber,
            NumberOfSectors)) {
        return;
    }

    //
    // Prepare the SCSI CDB.
    //

    RtlZeroMemory(&Cdb, sizeof(CDB));

    Cdb.CDB10.OperationCode = SCSIOP_READ;
    Cdb.CDB10.LogicalBlock = RtlUlongByteSwap(IrpSp->Parameters.IdexReadWrite.SectorNumber);
    Cdb.CDB10.TransferBlocks = RtlUshortByteSwap((USHORT)NumberOfSectors);

    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Select the IDE device and spin until the device is not busy.
    //

    if (!IdexCdRomSelectDeviceAndSpinWhileBusy()) {
        IdexCdRomFinishRead();
        return;
    }

    //
    // Prepare the bus master interface for the DMA transfer.
    //

    if (IdexIsFlagClear(Irp->Flags, IRP_SCATTER_GATHER_OPERATION) ||
        IdexIsFlagSet(IrpSp->Flags, SL_FSCACHE_REQUEST)) {
        IdexChannelPrepareBufferTransfer(IrpSp->Parameters.IdexReadWrite.Buffer +
            IrpSp->Parameters.IdexReadWrite.BufferOffset, TransferLength);
    } else {
        IdexChannelPrepareScatterGatherTransfer(Irp->SegmentArray,
            IrpSp->Parameters.IdexReadWrite.BufferOffset, TransferLength);
    }

    IdexWriteFeaturesPort(IDE_FEATURE_DMA);

    //
    // Write out the IDE command and the SCSI CDB.
    //

    if (!IdexCdRomWritePacket(&Cdb)) {
        IdexCdRomFinishRead();
        return;
    }

    //
    // Activate the bus master interface.
    //

    IdexWriteBusMasterCommandPort(IDE_BUS_MASTER_COMMAND_START |
        IDE_BUS_MASTER_COMMAND_READ);

    //
    // Indicate that we're expecting an interrupt for this IRP.
    //

    ASSERT(IdexChannelObject.InterruptRoutine == NULL);

    IdexChannelObject.InterruptRoutine = IdexCdRomTransferInterrupt;
    IdexChannelObject.FinishIoRoutine = IdexCdRomFinishRead;
    IdexChannelObject.ExpectingBusMasterInterrupt = TRUE;
    IdexChannelObject.Timeout = IDE_ATAPI_DEFAULT_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

NTSTATUS
IdexCdRomRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is indirectly called by the I/O manager to handle IRP_MJ_READ
    requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    LONGLONG StartingByteOffset;
    LONGLONG EndingByteOffset;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the parameters are valid.  The buffer length must be an
    // integral number of sectors, the starting offset must be sector aligned,
    // and the ending offset must be within the bounds of the disk.  The I/O
    // buffer must have the proper device alignment.
    //

    StartingByteOffset = IrpSp->Parameters.Read.ByteOffset.QuadPart;
    EndingByteOffset = StartingByteOffset + IrpSp->Parameters.Read.Length;

    if (IdexIsFlagClear(IrpSp->Flags, SL_FSCACHE_REQUEST)) {

        if (((PtrToUlong(Irp->UserBuffer) & IDE_ALIGNMENT_REQUIREMENT) != 0) ||
            ((IrpSp->Parameters.Read.BufferOffset & IDE_ALIGNMENT_REQUIREMENT) != 0) ||
            ((IrpSp->Parameters.Read.Length & IDE_ATAPI_CD_SECTOR_MASK) != 0) ||
            ((IrpSp->Parameters.Read.ByteOffset.LowPart & IDE_ATAPI_CD_SECTOR_MASK) != 0) ||
            (EndingByteOffset > IdexCdRomPartitionLength)) {

            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            return STATUS_INVALID_PARAMETER;
        }

    } else {

        //
        // Bypass parameter checking if this request is coming from the cache.
        // We'll trust that the file system cache is submitting a valid request,
        // but we'll double check in debug.
        //

        ASSERT((PtrToUlong(IrpSp->Parameters.Read.CacheBuffer) &
            IDE_ATAPI_CD_SECTOR_MASK) == 0);
        ASSERT((IrpSp->Parameters.Read.Length & IDE_ATAPI_CD_SECTOR_MASK) == 0);
        ASSERT((IrpSp->Parameters.Read.ByteOffset.LowPart & IDE_ATAPI_CD_SECTOR_MASK) == 0);
        ASSERT(EndingByteOffset <= IdexCdRomPartitionLength);
    }

    //
    // If this is a zero length request, then we can complete the IRP now.
    //

    if (IrpSp->Parameters.Read.Length == 0) {

        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }

    //
    // Convert the I/O request parameters to the internal IDE format.  The
    // internal format unpacks the cache buffer and buffer offset fields to
    // different fields by collapsing the byte offset to a sector number.
    //

    IrpSp->Parameters.IdexReadWrite.SectorNumber = (ULONG)(StartingByteOffset >>
        IDE_ATAPI_CD_SECTOR_SHIFT);

    ASSERT(FIELD_OFFSET(IO_STACK_LOCATION, Parameters.Read.Length) ==
           FIELD_OFFSET(IO_STACK_LOCATION, Parameters.IdexReadWrite.Length));

    //
    // Move the buffer pointer and the buffer starting offset around depending
    // on whether or not this is a file system cache request.
    //

    if (IdexIsFlagSet(IrpSp->Flags, SL_FSCACHE_REQUEST)) {
        IrpSp->Parameters.IdexReadWrite.BufferOffset = 0;
        ASSERT(FIELD_OFFSET(IO_STACK_LOCATION, Parameters.Read.CacheBuffer) ==
               FIELD_OFFSET(IO_STACK_LOCATION, Parameters.IdexReadWrite.Buffer));
    } else {
        IrpSp->Parameters.IdexReadWrite.BufferOffset =
            IrpSp->Parameters.Read.BufferOffset;
        IrpSp->Parameters.IdexReadWrite.Buffer = (PUCHAR)Irp->UserBuffer;
    }

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = IrpSp->Parameters.IdexReadWrite.Length;

    //
    // Queue the I/O reqeust.
    //

    IoMarkIrpPending(Irp);
    IdexChannelStartPacket(DeviceObject, Irp);

    return STATUS_PENDING;
}

NTSTATUS
IdexCdRomRequestDVDX2AuthenticationPage(
    OUT PDVDX2_AUTHENTICATION Authentication
    )
/*++

Routine Description:

    This routine requests the DVD-X2 authentication page from the CD-ROM device.

Arguments:

    Authentication - Specifies the buffer to receive the authentication page.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    PCDB Cdb = (PCDB)&PassThrough.Cdb;

    //
    // Prepare the SCSI pass through structure.
    //

    RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));

    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

    //
    // Request the authentication page from the DVD-X2 drive.
    //

    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = Authentication;
    PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);

    RtlZeroMemory(Cdb, sizeof(CDB));

    Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
    Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
    *((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

    RtlZeroMemory(Authentication, sizeof(DVDX2_AUTHENTICATION));

    status = IoSynchronousDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
        IdexCdRomDeviceObject, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT),
        NULL, 0, NULL, FALSE);

    if (!NT_SUCCESS(status)) {

        //
        // If the drive reported back an authentication failure at this point,
        // then change the error to STATUS_UNRECOGNIZED_MEDIA to help
        // distinguish between a disc that doesn't contain any anti-piracy
        // protection from a disc that failed the anti-piracy checks below.
        //

        if (status == STATUS_TOO_MANY_SECRETS) {
            return STATUS_UNRECOGNIZED_MEDIA;
        } else {
            return status;
        }
    }

    //
    // Check if the DVD-X2 drive thinks that this is a valid CDF header.
    //

    if (Authentication->AuthenticationPage.CDFValid != DVDX2_CDF_VALID) {
        return STATUS_UNRECOGNIZED_MEDIA;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
IdexCdRomRequestDVDX2ControlData(
    OUT PDVDX2_CONTROL_DATA ControlData
    )
/*++

Routine Description:

    This routine requests the DVD-X2 control data structure from the CD-ROM
    device.

Arguments:

    ControlData - Specifies the buffer to receive the control data structure.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    PCDB Cdb = (PCDB)&PassThrough.Cdb;

    //
    // Prepare the SCSI pass through structure.
    //

    RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));

    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

    //
    // Request the control data structure from the DVD-X2 drive.
    //

    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = ControlData;
    PassThrough.DataTransferLength = sizeof(DVDX2_CONTROL_DATA);

    RtlZeroMemory(Cdb, sizeof(CDB));

    Cdb->READ_DVD_STRUCTURE.OperationCode = SCSIOP_READ_DVD_STRUCTURE;
    *((PUSHORT)&Cdb->READ_DVD_STRUCTURE.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_CONTROL_DATA));
    *((PULONG)Cdb->READ_DVD_STRUCTURE.RMDBlockNumber) =
        IdexConstantUlongByteSwap((ULONG)(~DVDX2_CONTROL_DATA_BLOCK_NUMBER));
    Cdb->READ_DVD_STRUCTURE.LayerNumber = (UCHAR)(~DVDX2_CONTROL_DATA_LAYER);
    Cdb->READ_DVD_STRUCTURE.Format = DvdPhysicalDescriptor;
    Cdb->READ_DVD_STRUCTURE.Control = 0xC0;

    status = IoSynchronousDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
        IdexCdRomDeviceObject, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT),
        NULL, 0, NULL, FALSE);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: control data read failed (status=%08x).\n", status));
    }

    return status;
}

NTSTATUS
IdexCdRomVerifyDVDX2AuthoringSignature(
    IN PDVDX2_CONTROL_DATA ControlData
    )
/*++

Routine Description:

    This routine verifies the authoring signature of the supplied DVD-X2 control
    data structure.

Arguments:

    ControlData - Specifies the control data structure to be verified.

Return Value:

    Status of operation.

--*/
{
    PUCHAR PublicKeyData;
    UCHAR AuthoringDigest[XC_DIGEST_LEN];
    PUCHAR Workspace;
    BOOLEAN Verified;

    PublicKeyData = XePublicKeyData;

    //
    // Calculate the digest for bytes 0 to 1226 of the control data structure
    // and verify that this matches the authoring hash stored in the control
    // data structure.
    //

    XCCalcDigest((PUCHAR)&ControlData->LayerDescriptor,
        FIELD_OFFSET(DVDX2_CONTROL_DATA, AuthoringHash) -
        FIELD_OFFSET(DVDX2_CONTROL_DATA, LayerDescriptor), AuthoringDigest);

    if (!RtlEqualMemory(AuthoringDigest, ControlData->AuthoringHash,
        XC_DIGEST_LEN)) {
        return STATUS_TOO_MANY_SECRETS;
    }

    //
    // Allocate a workspace to do the digest verification.
    //

    Workspace = (PUCHAR)ExAllocatePoolWithTag(XCCalcKeyLen(PublicKeyData) * 2,
        'sWxI');

    if (Workspace == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Verify that the SHA1 digest matches the encrypted header digest.
    //

    Verified = XCVerifyDigest(ControlData->AuthoringSignature, PublicKeyData,
        Workspace, AuthoringDigest);

    ExFreePool(Workspace);

    if (Verified) {
        return STATUS_SUCCESS;
    } else {
        return STATUS_TOO_MANY_SECRETS;
    }
}

VOID
IdexCdRomDecryptHostChallengeResponseTable(
    IN PDVDX2_CONTROL_DATA ControlData
    )
/*++

Routine Description:

    This routine decrypts the host challenge response table of the supplied
    DVD-X2 control data structure.

Arguments:

    ControlData - Specifies the control data structure that contains the host
        challenge response table to be decrypted.

Return Value:

    None.

--*/
{
    A_SHA_CTX SHAHash;
    UCHAR SHADigest[A_SHA_DIGEST_LEN];
    struct RC4_KEYSTRUCT RC4KeyStruct;

    //
    // Compute the SHA-1 hash of the data between bytes 1183 to 1226 of the
    // control data structure.
    //

    A_SHAInit(&SHAHash);
    A_SHAUpdate(&SHAHash, (PUCHAR)&ControlData->AuthoringTimeStamp,
        FIELD_OFFSET(DVDX2_CONTROL_DATA, AuthoringHash) -
        FIELD_OFFSET(DVDX2_CONTROL_DATA, AuthoringTimeStamp));
    A_SHAFinal(&SHAHash, SHADigest);

    //
    // Compute a 56-bit RC4 session key from the SHA-1 hash.
    //

    rc4_key(&RC4KeyStruct, 56 / 8, SHADigest);

    //
    // Decrypt the host challenge response table in place using the RC4 session
    // key.
    //

    rc4(&RC4KeyStruct, sizeof(ControlData->HostChallengeResponseTable.Entries),
        (PUCHAR)&ControlData->HostChallengeResponseTable.Entries);
}

NTSTATUS
IdexCdRomAuthenticationChallenge(
    IN PDVDX2_CONTROL_DATA ControlData,
    IN PDVDX2_HOST_CHALLENGE_RESPONSE_ENTRY HostChallengeResponseEntry,
    IN BOOLEAN FirstChallenge,
    IN BOOLEAN FinalChallenge
    )
/*++

Routine Description:

    This routine issues the supplied challenge to the drive and checks the
    response from the drive.

Arguments:

    ControlData - Specifies the control data structure.

    HostChallengeResponseEntry - Specifies the challenge to issue.

    FirstChallenge - Specifies TRUE if this is the first challenge being made to
        the drive.

    FinalChallenge - Specifies TRUE if this is the final challenge being made to
        the drive.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    PCDB Cdb = (PCDB)&PassThrough.Cdb;
    DVDX2_AUTHENTICATION Authentication;

    ASSERT(HostChallengeResponseEntry->ChallengeLevel == 1);

    //
    // Prepare the SCSI pass through structure.
    //

    RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));

    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

    //
    // Prepare the authentication page.
    //

    RtlZeroMemory(&Authentication, sizeof(DVDX2_AUTHENTICATION));

    *((PUSHORT)&Authentication.Header.ModeDataLength) =
        IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION) -
        FIELD_OFFSET(DVDX2_AUTHENTICATION, Header.MediumType));
    Authentication.AuthenticationPage.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
    Authentication.AuthenticationPage.PageLength = sizeof(DVDX2_AUTHENTICATION_PAGE) -
        FIELD_OFFSET(DVDX2_AUTHENTICATION_PAGE, PartitionArea);
    Authentication.AuthenticationPage.CDFValid = 1;
    Authentication.AuthenticationPage.DiscCategoryAndVersion =
        (ControlData->LayerDescriptor.BookType << 4) +
        ControlData->LayerDescriptor.BookVersion;
    Authentication.AuthenticationPage.DrivePhaseLevel = 1;
    Authentication.AuthenticationPage.ChallengeID = HostChallengeResponseEntry->ChallengeID;
    Authentication.AuthenticationPage.ChallengeValue = HostChallengeResponseEntry->ChallengeValue;

    if (!FirstChallenge) {
        Authentication.AuthenticationPage.Authentication = 1;
    }

    if (FinalChallenge) {
        Authentication.AuthenticationPage.PartitionArea = 1;
    }

    //
    // Issue the challenge to the DVD-X2 drive.
    //

    PassThrough.DataIn = SCSI_IOCTL_DATA_OUT;
    PassThrough.DataBuffer = &Authentication;
    PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);

    RtlZeroMemory(Cdb, sizeof(CDB));

    Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SELECT10;
    *((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

    status = IoSynchronousDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
        IdexCdRomDeviceObject, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT),
        NULL, 0, NULL, FALSE);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: challenge operation failed (status=%08x).\n", status));
        return status;
    }

    //
    // Read the response from the DVD-X2 drive.
    //

    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;

    Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
    Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
    *((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

    status = IoSynchronousDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
        IdexCdRomDeviceObject, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT),
        NULL, 0, NULL, FALSE);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: response operation failed (status=%08x).\n", status));
        return status;
    }

    //
    // Check that the drive's response matches the expected response.
    //

    if (!Authentication.AuthenticationPage.Authentication ||
        (Authentication.AuthenticationPage.ResponseValue !=
        HostChallengeResponseEntry->ResponseValue)) {
        IdexDbgPrint(("IDEX: invalid response from drive.\n"));
        return STATUS_TOO_MANY_SECRETS;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
IdexCdRomAuthenticationSequence(
    VOID
    )
/*++

Routine Description:

    This routine handles the IOCTL_CDROM_AUTHENTICATION_SEQUENCE request.

Arguments:

    None.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    DVDX2_AUTHENTICATION Authentication;
    DVDX2_CONTROL_DATA ControlData;
    LONG StartingIndex;
    LONG Index;
    LONG EndingIndex;
    BOOLEAN FirstChallenge;
    PDVDX2_HOST_CHALLENGE_RESPONSE_ENTRY HostChallengeResponseEntry;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    // If we've already authenticated the DVD-X2 disc, then don't bother doing
    // it again.
    //

    if (IdexCdRomDVDX2Authenticated) {
        return STATUS_SUCCESS;
    }

    //
    // Request the authentication page from the DVD-X2 drive.
    //

    status = IdexCdRomRequestDVDX2AuthenticationPage(&Authentication);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Request the control data structure from the DVD-X2 drive.
    //

    status = IdexCdRomRequestDVDX2ControlData(&ControlData);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Verify that the disc category and version from the authentication page
    // matches the control data structure.
    //

    if (Authentication.AuthenticationPage.DiscCategoryAndVersion !=
        (ControlData.LayerDescriptor.BookType << 4) + ControlData.LayerDescriptor.BookVersion) {
        IdexDbgPrint(("IDEX: disc category and version mismatch.\n"));
        return STATUS_TOO_MANY_SECRETS;
    }

#ifdef DEVKIT
    //
    // Check if the DVD-X2 drive already thinks that we're authenticated.  This
    // can only happen in a DEVKIT environment where we loaded a ROM from the
    // CD-ROM or hard disk and cold booted into that ROM.  The second instance
    // of the ROM doesn't know if the first instance of the ROM already ran the
    // authentication sequence, so we rely on the the drive.
    //

    if ((Authentication.AuthenticationPage.PartitionArea != 0) &&
        (Authentication.AuthenticationPage.Authentication != 0)) {
        IdexCdRomDVDX2Authenticated = TRUE;
        return STATUS_SUCCESS;
    }
#endif

    //
    // Verify that the encrypted digest stored in the control data structure
    // matches the digest of the structure.
    //

    status = IdexCdRomVerifyDVDX2AuthoringSignature(&ControlData);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: failed to verify control data structure (status=%08x).\n", status));
        return status;
    }

    //
    // Decrypt the contents of the host challenge response table.
    //

    IdexCdRomDecryptHostChallengeResponseTable(&ControlData);

    //
    // Validate the the host challenge response table is the expected version
    // and that there are the expected number of entries in the table.
    //

    if ((ControlData.HostChallengeResponseTable.Version != 1) ||
        (ControlData.HostChallengeResponseTable.NumberOfEntries == 0) ||
        (ControlData.HostChallengeResponseTable.NumberOfEntries >
            DVDX2_HOST_CHALLENGE_RESPONSE_ENTRY_COUNT)) {
        IdexDbgPrint(("IDEX: invalid host challenge response table.\n"));
        return STATUS_TOO_MANY_SECRETS;
    }

    //
    // Walk through the host challenge response starting at a random starting
    // index and issue challenge response values.
    //

    StartingIndex = KeQueryPerformanceCounter().LowPart %
        ControlData.HostChallengeResponseTable.NumberOfEntries;
    Index = StartingIndex;
    EndingIndex = -1;
    FirstChallenge = TRUE;

    do {

        HostChallengeResponseEntry = &ControlData.HostChallengeResponseTable.Entries[Index];

        //
        // Check if this is a supported challenge level.  If so, issue the
        // authentication challenge to the DVD-X2 drive.
        //

        if (HostChallengeResponseEntry->ChallengeLevel == 1) {

            status = IdexCdRomAuthenticationChallenge(&ControlData,
                HostChallengeResponseEntry, FirstChallenge, FALSE);

            if (!NT_SUCCESS(status)) {
                return status;
            }

            EndingIndex = Index;
            FirstChallenge = FALSE;
        }

        //
        // Advance the index and jump back to zero if we reach the end of the
        // table.
        //

        Index++;

        if (Index == ControlData.HostChallengeResponseTable.NumberOfEntries) {
            Index = 0;
        }

    } while (Index != StartingIndex);

    //
    // If we found a challenge response table with no entries that we can
    // process, then fail authentication.  We need at least one entry in order
    // to issue the final request to switch to the Xbox partition.
    //

    if (EndingIndex == -1) {
        IdexDbgPrint(("IDEX: no usable challenge response entries.\n"));
        return STATUS_TOO_MANY_SECRETS;
    }

    //
    // Issue the last challenge response entry again, but this time switch to
    // the Xbox partition.
    //

    HostChallengeResponseEntry = &ControlData.HostChallengeResponseTable.Entries[EndingIndex];

    status = IdexCdRomAuthenticationChallenge(&ControlData,
        HostChallengeResponseEntry, FALSE, TRUE);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Remember that the disc has passed DVD-X2 authentication.
    //

    IdexCdRomDVDX2Authenticated = TRUE;

    return STATUS_SUCCESS;
}

NTSTATUS
IdexCdRomDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is indirectly called by the I/O manager to handle
    IRP_MJ_DEVICE_CONTROL requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION IrpSp;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    switch (IrpSp->Parameters.DeviceIoControl.IoControlCode) {

        case IOCTL_CDROM_READ_TOC:
        case IOCTL_CDROM_GET_LAST_SESSION:
        case IOCTL_CDROM_CHECK_VERIFY:
        case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
        case IOCTL_CDROM_RAW_READ:
        case IOCTL_CDROM_SET_SPINDLE_SPEED:
        case IOCTL_DVD_START_SESSION:
        case IOCTL_DVD_READ_KEY:
        case IOCTL_DVD_SEND_KEY:
        case IOCTL_DVD_END_SESSION:
        case IOCTL_DVD_SEND_KEY2:
        case IOCTL_DVD_READ_STRUCTURE:
        case IOCTL_IDE_PASS_THROUGH:
        case IOCTL_SCSI_PASS_THROUGH_DIRECT:
            IoMarkIrpPending(Irp);
            IdexChannelStartPacket(DeviceObject, Irp);
            status = STATUS_PENDING;
            break;

        case IOCTL_CDROM_AUTHENTICATION_SEQUENCE:
            status = IdexCdRomAuthenticationSequence();
            break;

        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }

    //
    // If the request is no longer pending, then complete the request.
    //

    if (status != STATUS_PENDING) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    return status;
}

VOID
IdexCdRomStartIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the channel to handle the next queued I/O request.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // If the console is preparing to reset or shutdown, there's no reason to
    // continue processing this request.
    //

    if (HalIsResetOrShutdownPending()) {
        IdexChannelAbortCurrentPacket();
        return;
    }

    //
    // Dispatch the IRP request.
    //

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    switch (IrpSp->MajorFunction) {

        case IRP_MJ_READ:
            IdexCdRomStartRead(Irp);
            break;

        case IRP_MJ_DEVICE_CONTROL:
            switch (IrpSp->Parameters.DeviceIoControl.IoControlCode) {

                case IOCTL_CDROM_READ_TOC:
                case IOCTL_CDROM_GET_LAST_SESSION:
                    IdexCdRomStartReadTOC(Irp);
                    break;

                case IOCTL_CDROM_CHECK_VERIFY:
                    IdexCdRomStartCheckVerify(Irp);
                    break;

                case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
                    IdexCdRomStartGetDriveGeometry(Irp);
                    break;

                case IOCTL_CDROM_RAW_READ:
                    IdexCdRomStartRawRead(Irp);
                    break;

                case IOCTL_CDROM_SET_SPINDLE_SPEED:
                    IdexCdRomStartSetSpindleSpeed(Irp);
                    break;

                case IOCTL_DVD_START_SESSION:
                    IdexCdRomStartStartSession(Irp);
                    break;

                case IOCTL_DVD_READ_KEY:
                    IdexCdRomStartReadKey(Irp);
                    break;

                case IOCTL_DVD_SEND_KEY:
                case IOCTL_DVD_SEND_KEY2:
                    IdexCdRomStartSendKey(Irp);
                    break;

                case IOCTL_DVD_END_SESSION:
                    IdexCdRomStartEndSession(Irp);
                    break;

                case IOCTL_DVD_READ_STRUCTURE:
                    IdexCdRomStartReadStructure(Irp);
                    break;

                case IOCTL_IDE_PASS_THROUGH:
                    IdexChannelStartIdePassThrough(Irp, IDE_CDROM_DEVICE_NUMBER,
                        IdexCdRomResetDevice);
                    break;

                case IOCTL_SCSI_PASS_THROUGH_DIRECT:
                    IdexCdRomStartScsiPassThrough(Irp);
                    break;

                default:
                    IdexBugCheck(IDE_BUG_CHECK_CDROM, Irp);
                    break;
            }
            break;

        default:
            IdexBugCheck(IDE_BUG_CHECK_CDROM, Irp);
            break;
    }
}

VOID
IdexCdRomTrayOpenNotification(
    VOID
    )
/*++

Routine Description:

    This routine is called by the SMC interrupt handler when a tray open event
    has occurred.

Arguments:

    None.

Return Value:

    None.

--*/
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Reset any assumptions about the state of the media.
    //

    IdexCdRomPartitionLength = _I64_MAX;
    IdexCdRomDVDX2Authenticated = FALSE;

#if DBG
    //
    // Reset any debug statistics.
    //

    RtlZeroMemory(IdexCdRomDebugErrors, sizeof(IdexCdRomDebugErrors));
    IdexCdRomDebugReadsFinished = 0;
#endif
}

VOID
IdexCdRomFatalError(
    IN ULONG ErrorCode
    )
/*++

Routine Description:

    This routine is called when a fatal CD-ROM device related error is detected.

Arguments:

    ErrorCode - Specifies the fatal error code.

Return Value:

    None.

--*/
{
    IdexDbgBreakPoint();

    //
    // Flush the LEDs to indicate that there is CD-ROM related problem.
    //

    HalWriteSMCLEDStates(SMC_LED_STATES_RED_STATE0 | SMC_LED_STATES_GREEN_STATE1 |
        SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);

    //
    // Display the universal error message.
    //

    ExDisplayFatalError(ErrorCode);
}

VOID
IdexCdRomCreateQuick(
    VOID
    )
/*++

Routine Description:

    This routine initializes the CD-ROM device subsystem after a quick reboot.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    PDEVICE_OBJECT CdRomDeviceObject;
    DVDX2_AUTHENTICATION Authentication;

    //
    // Create the CD-ROM's device object.
    //

    status = IoCreateDevice(&IdexCdRomDriverObject, 0, &IdexCdRomDeviceName,
        FILE_DEVICE_CD_ROM, FALSE, &CdRomDeviceObject);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_CDROM, status);
    }

    //
    // Create the \DosDevices symbolic link.
    //

    status = IoCreateSymbolicLink(&IdexCdRomDosDevicesName, &IdexCdRomDeviceName);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_CDROM, status);
    }

    //
    // Mark the device object as support direct I/O so that user buffers are
    // passed down to us as locked buffers.  Also indicate that we can accept
    // scatter/gather buffers.
    //

    CdRomDeviceObject->Flags |= DO_DIRECT_IO | DO_SCATTER_GATHER_IO;

    //
    // Set the alignment requirement for the device.
    //

    CdRomDeviceObject->AlignmentRequirement = IDE_ALIGNMENT_REQUIREMENT;

    //
    // Set the size of a sector in bytes for the device.
    //

    CdRomDeviceObject->SectorSize = IDE_ATAPI_CD_SECTOR_SIZE;

    //
    // The device has finished initializing and is ready to accept requests.
    //

    CdRomDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    //
    // Store the device object globally so that it can be quickly found later.
    //

    IdexCdRomDeviceObject = CdRomDeviceObject;

    //
    // If the kernel is quick booting and we think that the current media is
    // DVD-X2, then verify that assumption by checking the DVD-X2 authentication
    // page.  This code is in place to prevent media swap piracy attacks.
    //

    if (KeHasQuickBooted && IdexCdRomDVDX2Authenticated) {

        //
        // Assume that the drive has not passed DVD-X2 authentication.
        //

        IdexCdRomDVDX2Authenticated = FALSE;

        //
        // Request the authentication page from the DVD-X2 drive.
        //

        status = IdexCdRomRequestDVDX2AuthenticationPage(&Authentication);

        if (NT_SUCCESS(status)) {

            //
            // If the DVD-X2 drive is in the Xbox partition and has successfully
            // completed DVD-X2 authentication, then the media likely hasn't
            // changed from the last time DVD-X2 authentication was performed.
            //

            if ((Authentication.AuthenticationPage.PartitionArea != 0) &&
                (Authentication.AuthenticationPage.Authentication != 0)) {
                IdexCdRomDVDX2Authenticated = TRUE;
            }
        }
    }
}

VOID
IdexCdRomCreate(
    VOID
    )
/*++

Routine Description:

    This routine initializes the CD-ROM device subsystem.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    ULONG Retries;
    LARGE_INTEGER Interval;
    KIRQL OldIrql;
    IDE_IDENTIFY_DATA IdentifyData;
    ULONG IOErrors;

    //
    // Wait for the device to finish its hardware reset sequence.  When cold
    // booting, the IDE controller in the south bridge will do a hardware reset
    // of the IDE bus, so we don't need to a software reset here.
    //
    // If we're booting a kernel that was loaded from the hard disk or CD-ROM,
    // then the device should already be ready to go, so we can skip the
    // initialization here.
    //

    HalPulseHardwareMonitorPin();

    if (IdexIsFlagClear(XboxBootFlags, XBOX_BOOTFLAG_SHADOW)) {

        //
        // Spin for up to thirty seconds as required by the ATA specification.
        //

        for (Retries = 600; Retries > 0; Retries--) {

            if (IdexCdRomPollResetComplete()) {
                break;
            }

            //
            // Delay for 50 milliseconds.
            //

            Interval.QuadPart = -50 * 10000;
            KeDelayExecutionThread(KernelMode, FALSE, &Interval);
        }

        if (Retries == 0) {
            IdexDbgPrint(("IDEX: CD-ROM timed out during reset.\n"));
            IdexCdRomFatalError(FATAL_ERROR_DVD_TIMEOUT);
        }
    }

    //
    // The below calls may cause an interrupt to be raised, so while we're
    // initializing the hardware, raise IRQL to synchronize with the interrupt
    // service routine.
    //

    IdexRaiseIrqlToChannelDIRQL(&OldIrql);

    //
    // Set the PIO transfer mode for the CD-ROM device.
    //

    HalPulseHardwareMonitorPin();

    status = IdexChannelSetTransferMode(IDE_CDROM_DEVICE_NUMBER,
        IDE_TRANSFER_MODE_PIO_MODE_4);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: CD-ROM not PIO configured (status=%02x).\n",
            IdexReadStatusPort()));
        KeLowerIrql(OldIrql);
        IdexCdRomFatalError(FATAL_ERROR_DVD_BAD_CONFIG);
    }

    //
    // Set the DMA transfer mode for the CD-ROM device.
    //

    HalPulseHardwareMonitorPin();

    status = IdexChannelSetTransferMode(IDE_CDROM_DEVICE_NUMBER,
        IDE_TRANSFER_MODE_UDMA_MODE_2);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: CD-ROM not DMA configured (status=%02x).\n",
            IdexReadStatusPort()));
        KeLowerIrql(OldIrql);
        IdexCdRomFatalError(FATAL_ERROR_DVD_BAD_CONFIG);
    }

    //
    // Issue an ATAPI identify device command in order to verify that the device
    // is present.
    //

    HalPulseHardwareMonitorPin();

    status = IdexChannelIdentifyDevice(IDE_CDROM_DEVICE_NUMBER,
        IDE_COMMAND_IDENTIFY_PACKET_DEVICE, &IdentifyData);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: CD-ROM not found (status=%02x).\n",
            IdexReadStatusPort()));
        KeLowerIrql(OldIrql);
        IdexCdRomFatalError(FATAL_ERROR_DVD_NOT_FOUND);
    }

    KeLowerIrql(OldIrql);

    //
    // At this point, we can use the quick boot creation code.
    //

    IdexCdRomCreateQuick();

    //
    // If we're cold-booting and we're ejecting the tray, then don't bother
    // waiting for the CD-ROM device to become ready because the tray is about
    // to be ejected and we're going to force a launch into the dashboard
    // anyway.
    //

    HalPulseHardwareMonitorPin();

    if (!KeHasQuickBooted &&
        IdexIsFlagClear(XboxBootFlags, XBOX_BOOTFLAG_TRAYEJECT)) {

        //
        // Delay until the CD-ROM device reports that it's ready.  We also handle
        // a 6-29 error here which is returned as STATUS_IO_DEVICE_ERROR.  We only
        // want to handle a certain number of IO errors here.
        //

        IOErrors = 0;

        while (IOErrors < 5) {

            status = IoSynchronousDeviceIoControlRequest(IOCTL_CDROM_CHECK_VERIFY,
                IdexCdRomDeviceObject, NULL, 0, NULL, 0, NULL, FALSE);

            if (status == STATUS_IO_DEVICE_ERROR) {
                IOErrors++;
            } else if (status != STATUS_DEVICE_NOT_READY) {
                break;
            }

            //
            // Delay for 200 milliseconds.
            //

            Interval.QuadPart = -200 * 10000;
            KeDelayExecutionThread(KernelMode, FALSE, &Interval);
        }
    }
}

//
// Driver object for the CD-ROM object.
//
DECLSPEC_RDATA DRIVER_OBJECT IdexCdRomDriverObject = {
    IdexCdRomStartIo,                   // DriverStartIo
    NULL,                               // DriverDeleteDevice
    NULL,                               // DriverDismountVolume
    {
        IdexDriverIrpReturnSuccess,     // IRP_MJ_CREATE
        IdexDriverIrpReturnSuccess,     // IRP_MJ_CLOSE
        IdexCdRomRead,                  // IRP_MJ_READ
        IoInvalidDeviceRequest,         // IRP_MJ_WRITE
        IoInvalidDeviceRequest,         // IRP_MJ_QUERY_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_SET_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_FLUSH_BUFFERS
        IoInvalidDeviceRequest,         // IRP_MJ_QUERY_VOLUME_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_DIRECTORY_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_FILE_SYSTEM_CONTROL
        IdexCdRomDeviceControl,         // IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_SHUTDOWN
        IoInvalidDeviceRequest,         // IRP_MJ_CLEANUP
    }
};

#else

//
// Single instance of the CD-ROM device object.
//
PDEVICE_OBJECT IdexCdRomDeviceObject;

VOID
IdexCdRomTrayOpenNotification(
    VOID
    )
/*++

Routine Description:

    This routine is called by the SMC interrupt handler when a tray open event
    has occurred.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NOTHING;
}

#endif
