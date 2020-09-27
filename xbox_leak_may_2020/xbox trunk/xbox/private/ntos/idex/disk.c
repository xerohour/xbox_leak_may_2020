/*++

Copyright (c) 2000-2002  Microsoft Corporation

Module Name:

    disk.c

Abstract:

    This module implements routines that apply to the disk object.

--*/

#include "idex.h"

#if !defined(ARCADE) || defined(DEVKIT)

//
// Define the maximum number of disk partitions that we'll reserve space for.
//

#define IDE_DISK_PARTITION_ARRAY_ELEMENTS       20

//
// Name of the disk object directory and its device object.
//
INITIALIZED_OBJECT_STRING_RDATA(IdexDiskDirectoryName, "\\Device\\Harddisk0");

//
// These disk identification strings are used by various other pieces of code to
// help uniquely identify this system.  The strings are based on the data
// returned from IDE_COMMAND_IDENTIFY_DEVICE.
//
DECLSPEC_STICKY UCHAR IdexDiskModelNumberBuffer[40];
DECLSPEC_STICKY STRING HalDiskModelNumber;
DECLSPEC_STICKY UCHAR IdexDiskSerialNumberBuffer[20];
DECLSPEC_STICKY STRING HalDiskSerialNumber;

//
// Stores whether or not the disk was successfully unlocked or not.
//
DECLSPEC_STICKY BOOLEAN IdexDiskSecurityUnlocked;

//
// Array of all of the created disk and partition device objects.
//
PDEVICE_OBJECT IdexDiskPartitionArray[IDE_DISK_PARTITION_ARRAY_ELEMENTS];

//
// All device objects relative to \Device\Harddisk0 start with this string.
//
INITIALIZED_OBJECT_STRING_RDATA(IdexDiskPartitionPrefix, "Partition");

//
// Number of sectors addressable from the driver.
//
DECLSPEC_STICKY ULONG IdexDiskUserAddressableSectors;

#ifndef IDEX_DISK_FIXED_SECTOR_COUNT
//
// Number of sectors allocated to the data paritition (Partition1).
//
DECLSPEC_STICKY ULONG IdexDiskDataPartitionSectors;
#endif

//
// Number of cache partitions available on the disk.
//
#ifdef IDEX_DISK_FIXED_SECTOR_COUNT
DECLSPEC_RDATA ULONG HalDiskCachePartitionCount =
    XDISK_NUMBER_OF_CACHE_PARTITIONS(XDISK_FIXED_SECTOR_COUNT);
#else
DECLSPEC_STICKY ULONG HalDiskCachePartitionCount;
#endif

//
// Define macros to abstract the disk geometry data for a fixed vs. dynamic
// configuration.
//
#ifdef IDEX_DISK_FIXED_SECTOR_COUNT
#define IDEX_DISK_CACHE_PARTITION_COUNT \
    XDISK_NUMBER_OF_CACHE_PARTITIONS(XDISK_FIXED_SECTOR_COUNT)
#define IDEX_DISK_DATA_PARTITION_SECTORS \
    (XDISK_FIXED_SECTOR_COUNT - \
    (XDISK_UNPARTITIONED_SECTORS + XDISK_SHELL_PARTITION_SECTORS + \
    (IDEX_DISK_CACHE_PARTITION_COUNT * XDISK_CACHE_PARTITION_SECTORS)))
#else
#define IDEX_DISK_CACHE_PARTITION_COUNT     HalDiskCachePartitionCount
#define IDEX_DISK_DATA_PARTITION_SECTORS    IdexDiskDataPartitionSectors
#endif

//
// Local support.
//

DECLSPEC_NORETURN
VOID
IdexDiskFatalError(
    IN ULONG ErrorCode
    );

VOID
IdexDiskInitIdentifierString(
    IN PUCHAR IdentifyDataString,
    OUT PUCHAR DiskIdentifierBuffer,
    IN SIZE_T DiskIdentifierLength,
    OUT PSTRING DiskIdentifierString
    );

NTSTATUS
IdexDiskSetDeviceParameters(
    VOID
    );

VOID
IdexDiskSecurityUnlock(
    UCHAR Password[IDE_ATA_PASSWORD_LENGTH]
    );

VOID
IdexDiskIncrementPowerCycleCount(
    VOID
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, IdexDiskFatalError)
#pragma alloc_text(INIT, IdexDiskInitIdentifierString)
#pragma alloc_text(INIT, IdexDiskSetDeviceParameters)
#pragma alloc_text(INIT, IdexDiskSecurityUnlock)
#pragma alloc_text(INIT, IdexDiskCreate)
#pragma alloc_text(INIT, IdexDiskIncrementPowerCycleCount)
#endif

BOOLEAN
IdexDiskPollResetComplete(
    VOID
    )
/*++

Routine Description:

    This routine polls the disk device to check for completion of a reset
    sequence.

Arguments:

    None.

Return Value:

    Returns TRUE if the device has completed its reset sequence.

--*/
{
    UCHAR IdeStatus;

    //
    // The software reset caused both the disk device and the CD-ROM device to
    // start their reset sequences.  Check if the CD-ROM device has finished its
    // reset sequence first.
    //

    if (!IdexCdRomPollResetComplete()) {
        return FALSE;
    }

    //
    // Select the IDE device.
    //

    IdexProgramTargetDevice(IDE_DISK_DEVICE_NUMBER);

    //
    // Check if the device is busy.
    //

    IdeStatus = IdexReadStatusPort();

    if (IdexIsFlagClear(IdeStatus, IDE_STATUS_BSY) &&
        IdexIsFlagSet(IdeStatus, IDE_STATUS_DRDY)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

VOID
IdexDiskResetDevice(
    VOID
    )
/*++

Routine Description:

    This routine software resets the disk device.

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
    // Ensure that the software reset flag is not asserted.
    //

    IdexWriteDeviceControlPort(0);
    KeStallExecutionProcessor(50);

    //
    // Assert the software reset flag.
    //

    IdexWriteDeviceControlPort(IDE_DEVICE_CONTROL_SRST);
    KeStallExecutionProcessor(100);

    //
    // De-assert the software reset flag.
    //

    IdexWriteDeviceControlPort(0);
    KeStallExecutionProcessor(50);

    //
    // Set the reset complete poll routine and a reset timeout of 10 seconds.
    //

    IdexChannelObject.PollResetCompleteRoutine = IdexDiskPollResetComplete;
    IdexChannelObject.Timeout = 100;

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

    IdexChannelObject.PollResetCompleteRoutine = IdexDiskPollResetComplete;
    IdexChannelObject.Timeout = 250;
}

BOOLEAN
IdexDiskSelectDeviceAndSpinWhileBusy(
    VOID
    )
/*++

Routine Description:

    This routine selects the disk device and spins until the the IDE status
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

    IdexProgramTargetDevice(IDE_DISK_DEVICE_NUMBER);

    //
    // Spin for up to a tenth second.
    //

    for (Retries = 1000; Retries > 0; Retries--) {

        IdeStatus = IdexReadStatusPort();

        if (IdexIsFlagClear(IdeStatus, IDE_STATUS_BSY) &&
            IdexIsFlagSet(IdeStatus, IDE_STATUS_DRDY)) {
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
IdexDiskNoTransferInterrupt(
    VOID
    )
/*++

Routine Description:

    This routine is invoked when a hardware interrupt occurs on the channel's
    IRQ and the pending interrupt IRP is for an ATA request that doesn't
    transfer any data.

Arguments:

    None.

Return Value:

    None.

--*/
{
    UCHAR IdeStatus;
    PIRP Irp;

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
IdexDiskTransferInterrupt(
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
    PIRP Irp;

    //
    // Read the bus master status register and verify that the interrupt bit is
    // set if we haven't already seen the bus master interrupt.
    //

    BusMasterStatus = IdexReadBusMasterStatusPort();

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

    //
    // Read the status register to dismiss the interrupt.
    //

    IdeStatus = IdexReadStatusPort();

    //
    // Deactivate the bus master interface and dismiss the interrupt.
    //

    IdexWriteBusMasterCommandPort(0);
    IdexWriteBusMasterStatusPort(IDE_BUS_MASTER_STATUS_INTERRUPT);

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
IdexDiskFinishGeneric(
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

    Irp = IdexChannelObject.CurrentIrp;

    if (Irp->IoStatus.Status == STATUS_IO_TIMEOUT) {

        //
        // Attempt to reset the device.  If the reset completes successfully and
        // the retry count has not been exceeded the maximum retry count, then
        // the IRP will be restarted.
        //

        IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

        IdexDiskResetDevice();

        return;
    }

    IoCompleteRequest(Irp, IO_DISK_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
FASTCALL
IdexDiskStartVerify(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IOCTL_DISK_VERIFY requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Select the IDE device and spin until the device is not busy.
    //

    if (!IdexDiskSelectDeviceAndSpinWhileBusy()) {
        IdexDiskFinishGeneric();
        return;
    }

    //
    // Issue the IDE command.
    //

    IdexProgramLBATransfer(IDE_DISK_DEVICE_NUMBER,
        IrpSp->Parameters.IdexReadWrite.SectorNumber,
        (IrpSp->Parameters.IdexReadWrite.Length >> IDE_ATA_SECTOR_SHIFT));

    IdexWriteCommandPort(IDE_COMMAND_VERIFY);

    //
    // Indicate that we're expecting an interrupt for this IRP.
    //

    ASSERT(IdexChannelObject.InterruptRoutine == NULL);

    IdexChannelObject.InterruptRoutine = IdexDiskNoTransferInterrupt;
    IdexChannelObject.FinishIoRoutine = IdexDiskFinishGeneric;
    IdexChannelObject.Timeout = IDE_ATA_DEFAULT_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

NTSTATUS
IdexDiskVerify(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles the IOCTL_DISK_VERIFY request.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PIDE_DISK_EXTENSION DiskExtension;
    PIO_STACK_LOCATION IrpSp;
    PVERIFY_INFORMATION VerifyInformation;
    LONGLONG StartingByteOffset;

    DiskExtension = (PIDE_DISK_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(VERIFY_INFORMATION)) {
        return STATUS_INFO_LENGTH_MISMATCH;
    }

    VerifyInformation =
        (PVERIFY_INFORMATION)IrpSp->Parameters.DeviceIoControl.InputBuffer;

    //
    // Verify that the verify length doesn't exceed the ATA limits.
    //

    if (VerifyInformation->Length > IDE_ATA_MAXIMUM_TRANSFER_BYTES) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Compute the starting byte offset for the verify.
    //

    StartingByteOffset = VerifyInformation->StartingOffset.QuadPart +
        DiskExtension->PartitionInformation.StartingOffset.QuadPart;

    //
    // Convert the I/O request parameters to the internal IDE format.  The
    // internal format uses the same IRP parameters as an IDE read and write.
    //

    IrpSp->Parameters.IdexReadWrite.Length = VerifyInformation->Length;
    IrpSp->Parameters.IdexReadWrite.SectorNumber = (ULONG)(StartingByteOffset >>
        IDE_ATA_SECTOR_SHIFT);

    //
    // Initialize the IRP's information result to the number of bytes
    // transferred.
    //

    Irp->IoStatus.Information = 0;

    //
    // Queue the I/O reqeust.
    //

    IoMarkIrpPending(Irp);
    IdexChannelStartPacket(DeviceObject, Irp);

    return STATUS_PENDING;
}

VOID
IdexDiskFinishReadWrite(
    VOID
    )
/*++

Routine Description:

    This routine is invoked at DPC level to finish processing a IRP_MJ_READ or
    IRP_MJ_WRITE request.

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
    // Jump to the generic handler to process errors.
    //

    if (!NT_SUCCESS(Irp->IoStatus.Status)) {
        IdexDiskFinishGeneric();
        return;
    }

    //
    // If there are still more bytes to transfer in this request, then adjust
    // the transfer counts and requeue the packet.
    //

    if (IrpSp->Parameters.IdexReadWrite.Length > IDE_ATA_MAXIMUM_TRANSFER_BYTES) {

        //
        // If the console is preparing to reset or shutdown, there's no reason
        // to continue processing this request.
        //

        if (HalIsResetOrShutdownPending()) {
            IdexChannelAbortCurrentPacket();
            return;
        }

        IrpSp->Parameters.IdexReadWrite.Length -= IDE_ATA_MAXIMUM_TRANSFER_BYTES;
        IrpSp->Parameters.IdexReadWrite.BufferOffset += IDE_ATA_MAXIMUM_TRANSFER_BYTES;
        IrpSp->Parameters.IdexReadWrite.SectorNumber += IDE_ATA_MAXIMUM_TRANSFER_SECTORS;

        IdexChannelStartPacket(IrpSp->DeviceObject, Irp);
        IdexChannelStartNextPacket();
        return;
    }

    //
    // Complete the request and start the next packet.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_DISK_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
FASTCALL
IdexDiskStartReadWrite(
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

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // If the number of bytes remaining is greater than the maximum transfer
    // size allowed by the hardware, then clip the number of bytes to the
    // maximum.
    //

    TransferLength = IrpSp->Parameters.IdexReadWrite.Length;

    if (TransferLength > IDE_ATA_MAXIMUM_TRANSFER_BYTES) {
        TransferLength = IDE_ATA_MAXIMUM_TRANSFER_BYTES;
    }

    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Select the IDE device and spin until the device is not busy.
    //

    if (!IdexDiskSelectDeviceAndSpinWhileBusy()) {
        IdexDiskFinishReadWrite();
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

    //
    // Issue the IDE command and activate the bus master interface.
    //

    IdexProgramLBATransfer(IDE_DISK_DEVICE_NUMBER,
        IrpSp->Parameters.IdexReadWrite.SectorNumber,
        (TransferLength >> IDE_ATA_SECTOR_SHIFT));

    if (IrpSp->MajorFunction == IRP_MJ_READ) {
        IdexWriteCommandPort(IDE_COMMAND_READ_DMA);
        IdexWriteBusMasterCommandPort(IDE_BUS_MASTER_COMMAND_START |
            IDE_BUS_MASTER_COMMAND_READ);
    } else {
        IdexWriteCommandPort(IDE_COMMAND_WRITE_DMA);
        IdexWriteBusMasterCommandPort(IDE_BUS_MASTER_COMMAND_START);
    }

    //
    // Indicate that we're expecting an interrupt for this IRP.
    //

    ASSERT(IdexChannelObject.InterruptRoutine == NULL);

    IdexChannelObject.InterruptRoutine = IdexDiskTransferInterrupt;
    IdexChannelObject.FinishIoRoutine = IdexDiskFinishReadWrite;
    IdexChannelObject.ExpectingBusMasterInterrupt = TRUE;
    IdexChannelObject.Timeout = IDE_ATA_DEFAULT_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

NTSTATUS
IdexDiskReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_READ and
    IRP_MJ_WRITE requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PIDE_DISK_EXTENSION DiskExtension;
    PIO_STACK_LOCATION IrpSp;
    LONGLONG StartingByteOffset;
    LONGLONG EndingByteOffset;

    DiskExtension = (PIDE_DISK_EXTENSION)DeviceObject->DeviceExtension;
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
            ((IrpSp->Parameters.Read.Length & IDE_ATA_SECTOR_MASK) != 0) ||
            ((IrpSp->Parameters.Read.ByteOffset.LowPart & IDE_ATA_SECTOR_MASK) != 0) ||
            (EndingByteOffset >
                DiskExtension->PartitionInformation.PartitionLength.QuadPart)) {

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
            IDE_ATA_SECTOR_MASK) == 0);
        ASSERT((IrpSp->Parameters.Read.Length & IDE_ATA_SECTOR_MASK) == 0);
        ASSERT((IrpSp->Parameters.Read.ByteOffset.LowPart & IDE_ATA_SECTOR_MASK) == 0);
        ASSERT(EndingByteOffset <=
            DiskExtension->PartitionInformation.PartitionLength.QuadPart);
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
    // Adjust the starting byte offset by the partition's starting offset.
    //

    StartingByteOffset += DiskExtension->PartitionInformation.StartingOffset.QuadPart;

    //
    // Convert the I/O request parameters to the internal IDE format.  The
    // internal format unpacks the cache buffer and buffer offset fields to
    // different fields by collapsing the byte offset to a sector number.
    //

    IrpSp->Parameters.IdexReadWrite.SectorNumber = (ULONG)(StartingByteOffset >>
        IDE_ATA_SECTOR_SHIFT);

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

VOID
FASTCALL
IdexDiskStartFlushBuffers(
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles queued IRP_MJ_FLUSH_BUFFER requests.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION IrpSp;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // No retries are allowed for this IRP.
    //

    IdexChannelObject.MaximumIoRetries = IDE_NO_RETRY_COUNT;

    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Select the IDE device and spin until the device is not busy.
    //

    if (!IdexDiskSelectDeviceAndSpinWhileBusy()) {
        IdexDiskFinishGeneric();
        return;
    }

    //
    // Issue the IDE command.
    //

    IdexWriteCommandPort(IDE_COMMAND_FLUSH_CACHE);

    //
    // Indicate that we're expecting an interrupt for this IRP.
    //

    ASSERT(IdexChannelObject.InterruptRoutine == NULL);

    IdexChannelObject.InterruptRoutine = IdexDiskNoTransferInterrupt;
    IdexChannelObject.FinishIoRoutine = IdexDiskFinishGeneric;
    IdexChannelObject.Timeout = IDE_ATA_FLUSH_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

NTSTATUS
IdexDiskFlushBuffers(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is indirectly called by the I/O manager to handle
    IRP_MJ_FLUSH_BUFFER requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    //
    // Queue the I/O reqeust.
    //

    IoMarkIrpPending(Irp);
    IdexChannelStartPacket(DeviceObject, Irp);

    return STATUS_PENDING;
}

NTSTATUS
IdexDiskGetDriveGeometry(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles the IOCTL_DISK_GET_DRIVE_GEOMETRY request.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PDISK_GEOMETRY DiskGeometry;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the output buffer is large enough to receive the data.
    //

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(DISK_GEOMETRY)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Fill in the output buffer and return.
    //

    DiskGeometry = (PDISK_GEOMETRY)Irp->UserBuffer;
    DiskGeometry->Cylinders.QuadPart = IdexDiskUserAddressableSectors;
    DiskGeometry->MediaType = FixedMedia;
    DiskGeometry->TracksPerCylinder = 1;
    DiskGeometry->SectorsPerTrack = 1;
    DiskGeometry->BytesPerSector = IDE_ATA_SECTOR_SIZE;

    Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);

    return STATUS_SUCCESS;
}

NTSTATUS
IdexDiskGetPartitionInfo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine handles the IOCTL_DISK_GET_PARTITION_INFO request.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PIDE_DISK_EXTENSION DiskExtension;
    PIO_STACK_LOCATION IrpSp;
    PPARTITION_INFORMATION PartitionInformation;

    DiskExtension = (PIDE_DISK_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Verify that the output buffer is large enough to receive the data.
    //

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(PARTITION_INFORMATION)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Fill in the output buffer and return.
    //

    PartitionInformation = (PPARTITION_INFORMATION)Irp->UserBuffer;

    *PartitionInformation = DiskExtension->PartitionInformation;

    Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);

    return STATUS_SUCCESS;
}

NTSTATUS
IdexDiskDeviceControl(
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

        case IOCTL_DISK_GET_DRIVE_GEOMETRY:
            status = IdexDiskGetDriveGeometry(DeviceObject, Irp);
            break;

        case IOCTL_DISK_GET_PARTITION_INFO:
            status = IdexDiskGetPartitionInfo(DeviceObject, Irp);
            break;

        case IOCTL_DISK_VERIFY:
            status = IdexDiskVerify(DeviceObject, Irp);
            break;

        case IOCTL_IDE_PASS_THROUGH:
            IoMarkIrpPending(Irp);
            IdexChannelStartPacket(DeviceObject, Irp);
            status = STATUS_PENDING;
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
IdexDiskStartIo(
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
    // Mark all IRPS as must complete IRPs because we have to wait for
    // completion of any disk device command in order to issue the shutdown
    // commands.
    //

    IoMarkIrpMustComplete(Irp);

    //
    // Dispatch the IRP request.
    //

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    switch (IrpSp->MajorFunction) {

        case IRP_MJ_READ:
        case IRP_MJ_WRITE:
            IdexDiskStartReadWrite(Irp);
            break;

        case IRP_MJ_FLUSH_BUFFERS:
            IdexDiskStartFlushBuffers(Irp);
            break;

        case IRP_MJ_DEVICE_CONTROL:
            switch (IrpSp->Parameters.DeviceIoControl.IoControlCode) {

                case IOCTL_DISK_VERIFY:
                    IdexDiskStartVerify(Irp);
                    break;

                case IOCTL_IDE_PASS_THROUGH:
                    IdexChannelStartIdePassThrough(Irp, IDE_DISK_DEVICE_NUMBER,
                        IdexDiskResetDevice);
                    break;

                default:
                    IdexBugCheck(IDE_BUG_CHECK_DISK, Irp);
                    break;
            }
            break;

        default:
            IdexBugCheck(IDE_BUG_CHECK_DISK, Irp);
            break;
    }
}

VOID
IdexDiskShutdownSystem(
    VOID
    )
/*++

Routine Description:

    This routine flushes and spins down the disk device.

Arguments:

    None.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;
    NTSTATUS status;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // If we haven't yet connected to the interrupt object, then we don't know
    // what the real interrupt IRQL should be.  Because the interrupt service
    // routine can't run, we don't really care, so set the interrupt IRQL to the
    // current IRQL.
    //

    if (!IdexChannelObject.InterruptObject.Connected) {
        IdexChannelObject.InterruptIrql = DISPATCH_LEVEL;
    }

    //
    // The below calls may cause an interrupt to be raised, so while we're
    // initializing the hardware, raise IRQL to synchronize with the interrupt
    // service routine.
    //

    IdexRaiseIrqlToChannelDIRQL(&OldIrql);

    //
    // Flush the disk device's cache.
    //

    status = IdexChannelIssueImmediateCommand(IDE_DISK_DEVICE_NUMBER,
        IDE_COMMAND_FLUSH_CACHE);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: shutdown failed to flush disk (status=%08x).\n",
            status));
        KeLowerIrql(DISPATCH_LEVEL);
        return;
    }

    //
    // Spindown the disk device.
    //

    status = IdexChannelIssueImmediateCommand(IDE_DISK_DEVICE_NUMBER,
        IDE_COMMAND_STANDBY_IMMEDIATE);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: shutdown failed to spindown disk (status=%08x).\n",
            status));
        KeLowerIrql(DISPATCH_LEVEL);
        return;
    }

    KeLowerIrql(DISPATCH_LEVEL);
}

NTSTATUS
IdexDiskPartitionCreate(
    IN ULONG PartitionNumber
    )
/*++

Routine Description:

    This routine constructs and initializes a partition device object.

Arguments:

    PartitionNumber - Specifies the index of the partition.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PARTITION_INFORMATION PartitionInformation;
    PDEVICE_OBJECT DiskDeviceObject;
    PIDE_DISK_EXTENSION DiskExtension;

    ASSERT(PartitionNumber != 0);

    switch (PartitionNumber) {

        case XDISK_DATA_PARTITION:
            //
            // Compute the data partition's starting sector and length.
            //

            PartitionInformation.HiddenSectors = XDISK_UNPARTITIONED_SECTORS +
                (IDEX_DISK_CACHE_PARTITION_COUNT * XDISK_CACHE_PARTITION_SECTORS) +
                XDISK_SHELL_PARTITION_SECTORS;
            PartitionInformation.PartitionLength.QuadPart =
                ((ULONGLONG)IDEX_DISK_DATA_PARTITION_SECTORS * IDE_ATA_SECTOR_SIZE);
            break;

        case XDISK_SHELL_PARTITION:
            //
            // Compute the shell partition's starting sector and length.
            //

            PartitionInformation.HiddenSectors = XDISK_UNPARTITIONED_SECTORS +
                (IDEX_DISK_CACHE_PARTITION_COUNT * XDISK_CACHE_PARTITION_SECTORS);
            PartitionInformation.PartitionLength.QuadPart =
                XDISK_SHELL_PARTITION_SIZE;
            break;

        default:
            //
            // We're attempting to create a cache partition.  Verify that the
            // partition number is valid.
            //

            if (PartitionNumber >= IDEX_DISK_CACHE_PARTITION_COUNT +
                XDISK_FIRST_CACHE_PARTITION) {
                return STATUS_OBJECT_NAME_NOT_FOUND;
            }

            //
            // Compute the cache partition's starting sector and length.
            //

            PartitionInformation.HiddenSectors = XDISK_UNPARTITIONED_SECTORS +
                ((PartitionNumber - XDISK_FIRST_CACHE_PARTITION) *
                XDISK_CACHE_PARTITION_SECTORS);
            PartitionInformation.PartitionLength.QuadPart =
                XDISK_CACHE_PARTITION_SIZE;
            break;
    }

    PartitionInformation.StartingOffset.QuadPart =
        ((ULONGLONG)PartitionInformation.HiddenSectors * IDE_ATA_SECTOR_SIZE);
    PartitionInformation.PartitionNumber = PartitionNumber;

    ASSERT(PartitionInformation.StartingOffset.QuadPart +
        PartitionInformation.PartitionLength.QuadPart <=
        ((PIDE_DISK_EXTENSION)IdexDiskPartitionArray[0]->DeviceExtension)->
        PartitionInformation.PartitionLength.QuadPart);

    //
    // Create the partition's device object.
    //

    status = IoCreateDevice(&IdexDiskDriverObject, sizeof(IDE_DISK_EXTENSION),
        NULL, FILE_DEVICE_DISK, FALSE, &DiskDeviceObject);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Mark the device object as support direct I/O so that user buffers are
    // passed down to us as locked buffers.  Also indicate that we can accept
    // scatter/gather buffers.
    //

    DiskDeviceObject->Flags |= DO_DIRECT_IO | DO_SCATTER_GATHER_IO;

    //
    // Set the alignment requirement for the device.
    //

    DiskDeviceObject->AlignmentRequirement = IDE_ALIGNMENT_REQUIREMENT;

    //
    // Set the size of a sector in bytes for the device.
    //

    DiskDeviceObject->SectorSize = IDE_ATA_SECTOR_SIZE;

    //
    // Initialize the partition's device extension data.
    //

    DiskExtension = (PIDE_DISK_EXTENSION)DiskDeviceObject->DeviceExtension;

    DiskExtension->DeviceObject = DiskDeviceObject;
    DiskExtension->PartitionInformation = PartitionInformation;

    //
    // The device has finished initializing and is ready to accept requests.
    //

    DiskDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    //
    // Atomically insert the device object into the global array of created
    // device objects.  If another thread beats us to the creation of this
    // device object, then delete the one we created.
    //

    if (InterlockedCompareExchangePointer(&IdexDiskPartitionArray[PartitionNumber],
        DiskDeviceObject, NULL) != NULL) {
        IoDeleteDevice(DiskDeviceObject);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
IdexDiskParseDirectory(
    IN PVOID ParseObject,
    IN POBJECT_TYPE ObjectType,
    IN ULONG Attributes,
    IN OUT POBJECT_STRING CompleteName,
    IN OUT POBJECT_STRING RemainingName,
    IN OUT PVOID Context OPTIONAL,
    OUT PVOID *Object
    )
/*++

Routine Description:

    This routine is called by the object manager to parse a name relative to the
    "\Device\Harddisk0" disk directory.

Arguments:

    ParseObject - Specifies the object that the remaining name is relative to.

    ObjectType - Specifies the desired object type for the parsed object.

    Attributes - Specifies the object attributes for the parsed object.

    CompleteName - Specifies the entire name as supplied by the initiator of the
        object parse.

    RemainingName - Specifies the portion of the name that must still be
        resolved.

    Context - Specifies additional parse context information from the initiator
        of the object parse.

    Object - Specifies the buffer to receive the parsed object.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    BOOLEAN TrailingBackslash;
    OBJECT_STRING ElementName;
    POSTR Buffer;
    POSTR EndingBuffer;
    OCHAR Char;
    ULONG PartitionNumber;
    PDEVICE_OBJECT DiskDeviceObject;

    *Object = NULL;

    //
    // If there's no more name to parse, then fail the parse.
    //

    if (RemainingName->Length == 0) {
        return STATUS_ACCESS_DENIED;
    }

    //
    // Check if the name ends in a backslash.
    //

    if (RemainingName->Buffer[(RemainingName->Length / sizeof(OCHAR)) - 1] ==
        OBJ_NAME_PATH_SEPARATOR) {
        TrailingBackslash = TRUE;
    } else {
        TrailingBackslash = FALSE;
    }

    //
    // Pull off the next element of the path name.
    //

    ObDissectName(*RemainingName, &ElementName, RemainingName);

    //
    // Verify that there aren't multiple backslashes in the name.
    //

    if ((RemainingName->Length != 0) && (RemainingName->Buffer[0] ==
        OBJ_NAME_PATH_SEPARATOR)) {
        return STATUS_OBJECT_NAME_INVALID;
    }

    //
    // Move the remaining name buffer back to the path separator.  If we
    // successfully find or create a disk object below, then we'll want to pass
    // the remaining name on to the I/O manager where it'll be passed into a
    // file system which wants to see an absolute path.
    //

    if (TrailingBackslash || (RemainingName->Length != 0)) {
        RemainingName->Buffer--;
        RemainingName->Length += sizeof(OCHAR);
        RemainingName->MaximumLength = RemainingName->Length;
    }

    //
    // Check that the element starts with the expected prefix.
    //

    if ((ElementName.Length > IdexDiskPartitionPrefix.Length) &&
        RtlPrefixString(&IdexDiskPartitionPrefix, &ElementName, TRUE)) {

        //
        // Convert the partition number string into a number.  Note that we
        // don't worry about integer overflow in this loop; "Partition0" is
        // equal to "Partition4294967296".
        //

        PartitionNumber = 0;
        Buffer = ElementName.Buffer + IdexDiskPartitionPrefix.Length;
        EndingBuffer = ElementName.Buffer + ElementName.Length;

        while (Buffer < EndingBuffer) {

            Char = *Buffer;

            if (Char >= '0' && Char <= '9') {
                PartitionNumber = PartitionNumber * 10 + (Char - '0');
                Buffer++;
            } else {
                break;
            }
        }

        //
        // If we reached the end of the string without finding any non-numeric
        // characters and the partition number is in range, then the partition
        // number is valid.
        //

        if ((Buffer == EndingBuffer) &&
            (PartitionNumber < IDE_DISK_PARTITION_ARRAY_ELEMENTS)) {

            //
            // Check the array of created disk objects to see if we've already
            // created a disk object for the specified partition number.  If
            // not, then attempt to create the disk object.
            //

            for (;;) {

                DiskDeviceObject = IdexDiskPartitionArray[PartitionNumber];

                if (DiskDeviceObject != NULL) {
                    break;
                }

                status = IdexDiskPartitionCreate(PartitionNumber);

                if (!NT_SUCCESS(status)) {

                    if ((status == STATUS_OBJECT_NAME_NOT_FOUND) &&
                        (RemainingName->Length != 0)) {
                        status = STATUS_OBJECT_PATH_NOT_FOUND;
                    }

                    return status;
                }
            }

            //
            // Delegate the rest of the processing to the standard I/O
            // manager parse routine for devices (the object manager doesn't
            // support STATUS_REPARSE).
            //

            return IoParseDevice(DiskDeviceObject, ObjectType, Attributes,
                CompleteName, RemainingName, Context, Object);
        }
    }

    return (RemainingName->Length == 0) ? STATUS_OBJECT_NAME_NOT_FOUND :
        STATUS_OBJECT_PATH_NOT_FOUND;
}

VOID
IdexDiskFatalError(
    IN ULONG ErrorCode
    )
/*++

Routine Description:

    This routine is called when a fatal disk device related error is detected.

Arguments:

    ErrorCode - Specifies the fatal error code.

Return Value:

    None.

--*/
{
    IdexDbgBreakPoint();

    //
    // Flush the LEDs to indicate that there is disk related problem.
    //

    HalWriteSMCLEDStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_RED_STATE1 |
        SMC_LED_STATES_RED_STATE2 | SMC_LED_STATES_RED_STATE3);

    //
    // Display the universal error message.
    //

    ExDisplayFatalError(ErrorCode);
}

VOID
IdexDiskInitIdentifierString(
    IN PUCHAR IdentifyDataString,
    OUT PUCHAR DiskIdentifierBuffer,
    IN SIZE_T DiskIdentifierLength,
    OUT PSTRING DiskIdentifierString
    )
/*++

Routine Description:

    This routine takes a string from the identify data packet and copies it to a
    global buffer after converting it to the native host format.  The string is
    initialized with the global buffer, the length of the buffer with the
    trailing spaces stripped off, and the length of the buffer itself.

Arguments:

    IdentifyDataString - Specifies the string from the identify data packet.

    DiskIdentifierBuffer - Specifies the buffer to copy the identify data
        string to, in the native host format.

    DiskIdentifierLength - Specifies the length of the disk identifier buffer.

    DiskIdentifierString - Specifies the string to initialize with the raw
        buffer and length.

Return Value:

    None.

--*/
{
    PUSHORT UshortSource;
    PUSHORT UshortDestinationEnd;
    PUSHORT UshortDestination;
    PUCHAR EndBuffer;

    ASSERT((DiskIdentifierLength & 1) == 0);

    //
    // Copy the string from the identify data packet to the disk identifier
    // buffer.  The disk returns the characters in the wrong endian for us, so
    // swap them here.
    //

    UshortSource = (PUSHORT)IdentifyDataString;
    UshortDestinationEnd = (PUSHORT)(DiskIdentifierBuffer + DiskIdentifierLength);

    for (UshortDestination = (PUSHORT)DiskIdentifierBuffer; UshortDestination <
        UshortDestinationEnd; UshortDestination++) {
        *UshortDestination = RtlUshortByteSwap(*UshortSource++);
    }

    //
    // Strip off the trailing spaces from the string.
    //

    EndBuffer = DiskIdentifierBuffer + DiskIdentifierLength;

    while (EndBuffer > DiskIdentifierBuffer) {

        if (*(EndBuffer - 1) != ' ') {
            break;
        }

        EndBuffer--;
    }

    //
    // Initialize the global string with the pointer to the buffer, the string's
    // actual length, and the string's maximum possible length.
    //

    DiskIdentifierString->Buffer = DiskIdentifierBuffer;
    DiskIdentifierString->Length = (USHORT)(EndBuffer - DiskIdentifierBuffer);
    DiskIdentifierString->MaximumLength = (USHORT)DiskIdentifierLength;
}

VOID
IdexDiskCreateQuick(
    VOID
    )
/*++

Routine Description:

    This routine initializes the disk device subsystem after a quick reboot.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    PVOID DiskDirectoryObject;
    HANDLE DiskDirectoryHandle;
    PDEVICE_OBJECT DiskDeviceObject;
    PIDE_DISK_EXTENSION DiskExtension;

    //
    // Create the dynamic disk directory that will contain our device objects.
    //

    InitializeObjectAttributes(&ObjectAttributes, &IdexDiskDirectoryName,
        OBJ_PERMANENT | OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = ObCreateObject(&IdexDiskDirectoryObjectType, &ObjectAttributes, 0,
        (PVOID *)&DiskDirectoryObject);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_DISK, status);
    }

    status = ObInsertObject(DiskDirectoryObject, &ObjectAttributes, 0,
        &DiskDirectoryHandle);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_DISK, status);
    }

    NtClose(DiskDirectoryHandle);

    //
    // Create the disk's Partition0 device object.
    //

    status = IoCreateDevice(&IdexDiskDriverObject, sizeof(IDE_DISK_EXTENSION),
        NULL, FILE_DEVICE_DISK, FALSE, &DiskDeviceObject);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_DISK, status);
    }

    //
    // Mark the device object as support direct I/O so that user buffers are
    // passed down to us as locked buffers.  Also indicate that we can accept
    // scatter/gather buffers.
    //

    DiskDeviceObject->Flags |= DO_DIRECT_IO | DO_SCATTER_GATHER_IO;

    //
    // Indicate that the disk itself should only be mounted with the RAW file
    // system.
    //

    DiskDeviceObject->Flags |= DO_RAW_MOUNT_ONLY;

    //
    // Set the alignment requirement for the device.
    //

    DiskDeviceObject->AlignmentRequirement = IDE_ALIGNMENT_REQUIREMENT;

    //
    // Set the size of a sector in bytes for the device.
    //

    DiskDeviceObject->SectorSize = IDE_ATA_SECTOR_SIZE;

    //
    // Initialize the disk's device extension data.
    //

    DiskExtension = (PIDE_DISK_EXTENSION)DiskDeviceObject->DeviceExtension;

    DiskExtension->DeviceObject = DiskDeviceObject;

    //
    // Compute the length of the partition (the size in bytes of the entire
    // disk).
    //

    DiskExtension->PartitionInformation.PartitionLength.QuadPart =
        (ULONGLONG)IdexDiskUserAddressableSectors * IDE_ATA_SECTOR_SIZE;
    DiskExtension->PartitionInformation.PartitionNumber = 0;

    //
    // The device has finished initializing and is ready to accept requests.
    //

    DiskDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    //
    // Insert the Partition0 device object into the global array of created
    // device objects.
    //

    IdexDiskPartitionArray[0] = DiskDeviceObject;
}

NTSTATUS
IdexDiskSetDeviceParameters(
    VOID
    )
/*++

Routine Description:

    This routine sets the device parameters for the disk device to the maximum
    number of logical sectors per track and logical heads.

Arguments:

    None.

Return Value:

    Status of operation.

--*/
{
    UCHAR IdeStatus;

    IdexAssertIrqlAtChannelDIRQL();

    //
    // Select the IDE device.
    //

    IdexProgramTargetDevice(IDE_DISK_DEVICE_NUMBER);

    //
    // Spin until the device is not busy.
    //

    if (!IdexChannelSpinWhileBusy(&IdeStatus)) {
        return STATUS_IO_TIMEOUT;
    }

    //
    // Issue the set device parameters command.
    //

    IdexProgramTargetDeviceWithData(IDE_DISK_DEVICE_NUMBER, 15);

    IdexWriteSectorCountPort(63);

    IdexWriteCommandPort(IDE_COMMAND_SET_DEVICE_PARAMETERS);

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
IdexDiskSecurityUnlock(
    UCHAR Password[IDE_ATA_PASSWORD_LENGTH]
    )
/*++

Routine Description:

    This routine unlocks the disk device.

Arguments:

    Password - Specifies the password of the disk device.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    IDE_SECURITY_DATA SecurityData;
    ATA_PASS_THROUGH AtaPassThrough;
    KIRQL OldIrql;
    IDE_IDENTIFY_DATA IdentifyData;

    //
    // Prepare the security sector.
    //

    RtlZeroMemory(&SecurityData, sizeof(IDE_SECURITY_DATA));
    RtlCopyMemory(SecurityData.Password, Password, IDE_ATA_PASSWORD_LENGTH);

    //
    // Prepare the pass through structure.
    //

    RtlZeroMemory(&AtaPassThrough.IdeReg, sizeof(IDEREGS));
    AtaPassThrough.IdeReg.bCommandReg = IDE_COMMAND_SECURITY_UNLOCK;
    AtaPassThrough.IdeReg.bHostSendsData = TRUE;
    AtaPassThrough.DataBufferSize = sizeof(IDE_SECURITY_DATA);
    AtaPassThrough.DataBuffer = &SecurityData;

    //
    // Issue the security unlock command.
    //

    status = IoSynchronousDeviceIoControlRequest(IOCTL_IDE_PASS_THROUGH,
        IdexDiskPartitionArray[0], &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
        &AtaPassThrough, sizeof(ATA_PASS_THROUGH), NULL, FALSE);

    if (NT_SUCCESS(status)) {

        //
        // Check if the device raised an error.
        //

        if (IdexIsFlagSet(AtaPassThrough.IdeReg.bCommandReg, IDE_STATUS_ERR)) {
            status = STATUS_IO_DEVICE_ERROR;
        } else {
            IdexDiskSecurityUnlocked = TRUE;
            status = STATUS_SUCCESS;
        }
    }

    if (!NT_SUCCESS(status)) {

        IdexDbgPrint(("IDEX: hard disk failed to unlock (status=%08x).\n", status));

        //
        // We failed to unlock the drive.  If the console is not in the
        // manufacturing region, then fail to boot the system.
        //

        if (IdexIsFlagClear(XboxGameRegion, XC_GAME_REGION_MANUFACTURING)) {
            IdexDiskFatalError(FATAL_ERROR_HDD_CANNOT_UNLOCK);
        }
    }

    //
    // The Western Digital drives have a firmware bug where after unlocking the
    // drive, the sequential read mode of the drive's cache is not properly
    // initialized causing severe performance degradation.  The workaround to
    // cause the cache to be properly initialized is to issue another ATA
    // identify device command followed by a set device parameters command.
    //

    IdexRaiseIrqlToChannelDIRQL(&OldIrql);

    status = IdexChannelIdentifyDevice(IDE_DISK_DEVICE_NUMBER,
        IDE_COMMAND_IDENTIFY_DEVICE, &IdentifyData);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: hard disk not found (status=%02x).\n",
            IdexReadStatusPort()));
        KeLowerIrql(OldIrql);
        IdexDiskFatalError(FATAL_ERROR_HDD_NOT_FOUND);
    }

    status = IdexDiskSetDeviceParameters();

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: hard disk not found (status=%02x).\n",
            IdexReadStatusPort()));
        KeLowerIrql(OldIrql);
        IdexDiskFatalError(FATAL_ERROR_HDD_NOT_FOUND);
    }

    KeLowerIrql(OldIrql);
}

VOID
IdexDiskIncrementPowerCycleCount(
    VOID
    )
/*++

Routine Description:

    This routine increments the number of times that the system has been power
    cycled.  This information is stored in the refurb information sector on the
    disk device.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    XBOX_REFURB_INFO RefurbInfo;

    //
    // If this is a shadow boot, then don't increment the number of power
    // cycles.  If this boot is showing a short animation, then this can't be a
    // power on boot, so also don't increment the number of power cycles.
    //
    // Also, if the console is in the manufacturing region, then don't write out
    // anything to the disk.
    //

    if (IdexIsFlagSet(XboxBootFlags, XBOX_BOOTFLAG_SHADOW) ||
        IdexIsFlagSet(XboxBootFlags, XBOX_BOOTFLAG_SHORTANIMATION) ||
        IdexIsFlagSet(XboxGameRegion, XC_GAME_REGION_MANUFACTURING)) {
        return;
    }

    //
    // Read the refurb information from the drive.
    //

    status = ExReadWriteRefurbInfo(&RefurbInfo, sizeof(XBOX_REFURB_INFO), FALSE);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: failed to read refurb info (status=%08x).\n", status));
        return;
    }

    //
    // Increment the number of times that the system has been powered on.
    //

    RefurbInfo.PowerCycleCount++;

    //
    // Write the refurb information back out to the drive.
    //

    status = ExReadWriteRefurbInfo(&RefurbInfo, sizeof(XBOX_REFURB_INFO), TRUE);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: failed to write refurb info (status=%08x).\n", status));
        return;
    }
}

VOID
IdexDiskCreate(
    VOID
    )
/*++

Routine Description:

    This routine initializes the disk device subsystem.

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
    UCHAR Password[IDE_ATA_PASSWORD_LENGTH];

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

            if (IdexDiskPollResetComplete()) {
                break;
            }

            //
            // Delay for 50 milliseconds.
            //

            Interval.QuadPart = -50 * 10000;
            KeDelayExecutionThread(KernelMode, FALSE, &Interval);
        }

        if (Retries == 0) {
            IdexDbgPrint(("IDEX: hard disk timed out during reset.\n"));
            IdexDiskFatalError(FATAL_ERROR_HDD_TIMEOUT);
        }
    }

    //
    // The below calls may cause an interrupt to be raised, so while we're
    // initializing the hardware, raise IRQL to synchronize with the interrupt
    // service routine.
    //

    IdexRaiseIrqlToChannelDIRQL(&OldIrql);

    //
    // Set the PIO transfer mode for the disk device.
    //

    HalPulseHardwareMonitorPin();

    status = IdexChannelSetTransferMode(IDE_DISK_DEVICE_NUMBER,
        IDE_TRANSFER_MODE_PIO_MODE_4);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: hard disk not PIO configured (status=%02x).\n",
            IdexReadStatusPort()));
        KeLowerIrql(OldIrql);
        IdexDiskFatalError(FATAL_ERROR_HDD_BAD_CONFIG);
    }

    //
    // Set the DMA transfer mode for the disk device.
    //

    HalPulseHardwareMonitorPin();

    status = IdexChannelSetTransferMode(IDE_DISK_DEVICE_NUMBER,
        IDE_TRANSFER_MODE_UDMA_MODE_2);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: hard disk not DMA configured (status=%02x).\n",
            IdexReadStatusPort()));
        KeLowerIrql(OldIrql);
        IdexDiskFatalError(FATAL_ERROR_HDD_BAD_CONFIG);
    }

    //
    // Issue an ATA identify device command in order to determine the geometry
    // of the disk device.
    //

    HalPulseHardwareMonitorPin();

    status = IdexChannelIdentifyDevice(IDE_DISK_DEVICE_NUMBER,
        IDE_COMMAND_IDENTIFY_DEVICE, &IdentifyData);

    if (!NT_SUCCESS(status)) {
        IdexDbgPrint(("IDEX: hard disk not found (status=%02x).\n",
            IdexReadStatusPort()));
        KeLowerIrql(OldIrql);
        IdexDiskFatalError(FATAL_ERROR_HDD_NOT_FOUND);
    }

    KeLowerIrql(OldIrql);

    //
    // Build the disk identification strings.
    //

    IdexDiskInitIdentifierString(IdentifyData.ModelNumber,
        IdexDiskModelNumberBuffer, sizeof(IdexDiskModelNumberBuffer),
        &HalDiskModelNumber);
    IdexDiskInitIdentifierString(IdentifyData.SerialNumber,
        IdexDiskSerialNumberBuffer, sizeof(IdexDiskSerialNumberBuffer),
        &HalDiskSerialNumber);

#ifdef IDEX_DISK_FIXED_SECTOR_COUNT
    //
    // Verify that the disk has at least the fixed number of sectors that we're
    // hardcoded to support.
    //

    if (IdentifyData.UserAddressableSectors < XDISK_FIXED_SECTOR_COUNT) {
        IdexDbgPrint(("IDEX: hard disk too small.\n"));
        IdexDiskFatalError(FATAL_ERROR_HDD_BAD_CONFIG);
    }
#else
    //
    // Compute the number of cache partitions available on the disk.
    //

    HalDiskCachePartitionCount =
        XDISK_NUMBER_OF_CACHE_PARTITIONS(IdentifyData.UserAddressableSectors);

    if (HalDiskCachePartitionCount == 0) {
        IdexDbgPrint(("IDEX: hard disk too small to have a cache partition.\n"));
        IdexDiskFatalError(FATAL_ERROR_HDD_BAD_CONFIG);
    }

    //
    // Verify that the number of cache partitions doesn't exceed the size of the
    // global array of device objects minus the partitions 0 through 2 (the
    // entire disk, the data partition, and the shell partition).
    //

    if (HalDiskCachePartitionCount > IDE_DISK_PARTITION_ARRAY_ELEMENTS -
        XDISK_FIRST_CACHE_PARTITION) {
        HalDiskCachePartitionCount = IDE_DISK_PARTITION_ARRAY_ELEMENTS -
            XDISK_FIRST_CACHE_PARTITION;
    }

    //
    // Compute the number of sectors allocated to the data partition.
    //

    IdexDiskDataPartitionSectors = IdentifyData.UserAddressableSectors -
        (XDISK_UNPARTITIONED_SECTORS + XDISK_SHELL_PARTITION_SECTORS +
        (HalDiskCachePartitionCount * XDISK_CACHE_PARTITION_SECTORS));
#endif

    //
    // Store the number of user addressable sectors so that its available after
    // a quick reboot.
    //

    IdexDiskUserAddressableSectors = IdentifyData.UserAddressableSectors;

    //
    // At this point, we can use the quick boot creation code.
    //

    IdexDiskCreateQuick();

    //
    // Check if the drive has the security feature enabled.
    //

    HalPulseHardwareMonitorPin();

    if (IdentifyData.SecurityEnabled) {

        //
        // Check if the drive needs to be unlocked.
        //

        if (IdentifyData.SecurityLocked) {

            //
            // Generate the password using the hard disk key stored in the
            // EEPROM and the hard disk's model and serial numbers.
            //

            RtlZeroMemory(Password, IDE_ATA_PASSWORD_LENGTH);

            XcHMAC(XboxHDKey, XBOX_KEY_LENGTH, HalDiskModelNumber.Buffer,
                HalDiskModelNumber.Length, HalDiskSerialNumber.Buffer,
                HalDiskSerialNumber.Length, Password);

            //
            // Attempt to unlock the drive.
            //

            IdexDiskSecurityUnlock(Password);

        } else {

            //
            // The security feature is enabled, but the drive's password has
            // already been cleared.  There isn't anyway to verify the password
            // once the drive is unlocked, so we'll have to assume that we
            // unlocked it in a previous instance of the kernel.
            //

            IdexDiskSecurityUnlocked = TRUE;
        }

    } else {

        //
        // The loader will check to see if the XBE that is found is allowed to
        // run from a non-secure drive or if we're in the manufacturing region
        // where a non-secure drive is always allowed.
        //

        IdexDbgPrint(("IDEX: hard disk is not locked.\n"));
    }

    //
    // Increment the number of times that the system has been powered on.  The
    // CD-ROM device will still be busy detecting media, so this shouldn't have
    // a significant impact on boot time.
    //

    HalPulseHardwareMonitorPin();

    IdexDiskIncrementPowerCycleCount();
}

//
// Driver object for the disk object.
//
DECLSPEC_RDATA DRIVER_OBJECT IdexDiskDriverObject = {
    IdexDiskStartIo,                    // DriverStartIo
    NULL,                               // DriverDeleteDevice
    NULL,                               // DriverDismountVolume
    {
        IdexDriverIrpReturnSuccess,     // IRP_MJ_CREATE
        IdexDriverIrpReturnSuccess,     // IRP_MJ_CLOSE
        IdexDiskReadWrite,              // IRP_MJ_READ
        IdexDiskReadWrite,              // IRP_MJ_WRITE
        IoInvalidDeviceRequest,         // IRP_MJ_QUERY_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_SET_INFORMATION
        IdexDiskFlushBuffers,           // IRP_MJ_FLUSH_BUFFERS
        IoInvalidDeviceRequest,         // IRP_MJ_QUERY_VOLUME_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_DIRECTORY_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_FILE_SYSTEM_CONTROL
        IdexDiskDeviceControl,          // IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_SHUTDOWN
        IoInvalidDeviceRequest,         // IRP_MJ_CLEANUP
    }
};

//
// Object type for the dynamic disk directory.
//
DECLSPEC_RDATA OBJECT_TYPE IdexDiskDirectoryObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    NULL,
    IdexDiskParseDirectory,
    &ObpDefaultObject,
    'ksiD'
};

#else

//
// These disk identification strings are used by various other pieces of code to
// help uniquely identify this system.  The strings are based on the data
// returned from IDE_COMMAND_IDENTIFY_DEVICE.
//
DECLSPEC_RDATA STRING HalDiskModelNumber;
DECLSPEC_RDATA STRING HalDiskSerialNumber;

//
// Number of cache partitions available on the disk.
//
DECLSPEC_RDATA ULONG HalDiskCachePartitionCount = 0;

VOID
IdexDiskShutdownSystem(
    VOID
    )
/*++

Routine Description:

    This routine flushes and spins down the disk device.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NOTHING;
}

#endif
