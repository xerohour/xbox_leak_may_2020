/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    mediabrd.c

Abstract:

    This module implements routines that apply to the media board object.

--*/

#include "idex.h"
#include "mediabrd.h"

//
// Name of the media board object directory and its device object.
//
INITIALIZED_OBJECT_STRING_RDATA(IdexMediaBoardDirectoryName,
    "\\Device\\MediaBoard");

//
// Name of the media board's file system area device object and its DOS devices
// symbolic link.
//
INITIALIZED_OBJECT_STRING_RDATA(IdexMediaBoardFsDeviceName,
    "\\Device\\MediaBoard\\Partition0");
INITIALIZED_OBJECT_STRING_RDATA(IdexMediaBoardFsDosDevicesName, "\\??\\mbfs:");

//
// Name of the media board's communication area device object and its DOS
// devices symbolic link.
//
INITIALIZED_OBJECT_STRING_RDATA(IdexMediaBoardCommunicationDeviceName,
    "\\Device\\MediaBoard\\Partition1");
INITIALIZED_OBJECT_STRING_RDATA(IdexMediaBoardCommunicationDosDevicesName,
    "\\??\\mbcom:");

//
// Name of the media board's boot ROM device object and its DOS devices symbolic
// link.
//
INITIALIZED_OBJECT_STRING_RDATA(IdexMediaBoardBootROMDeviceName,
    "\\Device\\MediaBoard\\Partition2");
INITIALIZED_OBJECT_STRING_RDATA(IdexMediaBoardBootROMDosDevicesName,
    "\\??\\mbrom:");

//
//
//
INITIALIZED_OBJECT_STRING_RDATA(IdexMediaBoardCdRomDeviceName, "\\Device\\CdRom0");

//
// Number of sectors addressable from the driver.
//
DECLSPEC_STICKY ULONG IdexMediaBoardUserAddressableSectors;

//
// Stores whether or not this is the FPGA version of the media board.
//
DECLSPEC_STICKY BOOLEAN IdexMediaBoardFPGABoard;

//
// Local support.
//

VOID
IdexMediaBoardFinishBootROMEraseSector(
    VOID
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, IdexMediaBoardFatalError)
#pragma alloc_text(INIT, IdexMediaBoardCreate)
#endif

BOOLEAN
IdexMediaBoardPollResetComplete(
    VOID
    )
/*++

Routine Description:

    This routine polls the media board device to check for completion of a reset
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

    IdexProgramTargetDevice(IDE_MEDIA_BOARD_DEVICE_NUMBER);

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
IdexMediaBoardResetDevice(
    VOID
    )
/*++

Routine Description:

    This routine software resets the media board device.

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

    IdexProgramTargetDevice(IDE_MEDIA_BOARD_DEVICE_NUMBER);

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

    IdexChannelObject.PollResetCompleteRoutine = IdexMediaBoardPollResetComplete;
    IdexChannelObject.Timeout = 250;
}

BOOLEAN
IdexMediaBoardSelectDeviceAndSpinWhileBusy(
    VOID
    )
/*++

Routine Description:

    This routine selects the media board device and spins until the the IDE
    status register's BSY bit is clear, which indicates that the device is ready
    to accept a command.

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

    IdexProgramTargetDevice(IDE_MEDIA_BOARD_DEVICE_NUMBER);

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

VOID
IdexMediaBoardTransferInterrupt(
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
IdexMediaBoardNoTransferInterrupt(
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
IdexMediaBoardFinishGeneric(
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

        IdexMediaBoardResetDevice();

        return;
    }

    IoCompleteRequest(Irp, IO_DISK_INCREMENT);
    IdexChannelStartNextPacket();
}

VOID
IdexMediaBoardFinishBootROMWriteFlash(
    VOID
    )
/*++

Routine Description:

    This routine flashes the next 16-bit value from the user's buffer.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PIRP Irp;
    PIO_STACK_LOCATION IrpSp;
    ULONG FlashAddress;
    PUCHAR Buffer;
    PIDE_FINISHIO_ROUTINE FinishIoRoutine;

    Irp = IdexChannelObject.CurrentIrp;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Select the IDE device and spin until the device is not busy.
    //

    if (!IdexMediaBoardSelectDeviceAndSpinWhileBusy()) {
        IdexMediaBoardFinishGeneric();
        return;
    }

    //
    // Program the address in the ROM to be flashed.
    //

    FlashAddress = ((IrpSp->Parameters.IdexReadWrite.SectorNumber -
        MEDIA_BOARD_BOOT_ROM_START_LBA) << IDE_ATA_SECTOR_SHIFT) +
        (IDE_ATA_SECTOR_SIZE -
        (IrpSp->Parameters.IdexReadWrite.Length & IDE_ATA_SECTOR_MASK));
DbgPrint("flash address=%08x\n", FlashAddress);

    IdexWriteFeaturesPort((UCHAR)FlashAddress);
    IdexWriteSectorCountPort((UCHAR)(FlashAddress >> 8));
    IdexWriteSectorNumberPort((UCHAR)(FlashAddress >> 16));

    //
    // Program the 16-bit value to be flashed.
    //

    Buffer = IrpSp->Parameters.IdexReadWrite.Buffer +
        IrpSp->Parameters.IdexReadWrite.BufferOffset;
DbgPrint("buffer=%08x, data=%04x\n", Buffer, *((PUSHORT)Buffer));

    IdexWriteCylinderLowPort(*Buffer);
    IdexWriteCylinderHighPort(*(Buffer + 1));

    //
    // Issue the IDE command.
    //

    IdexWriteCommandPort(MEDIA_BOARD_COMMAND_BOOT_WRITE);

    //
    // Adjust the number of bytes remaining to be transferred
    //

    IrpSp->Parameters.IdexReadWrite.Length -= sizeof(USHORT);
    IrpSp->Parameters.IdexReadWrite.BufferOffset += sizeof(USHORT);

    //
    // Determine which step to advance to next.  If there are no more bytes
    // remaining to be transferred, then we'll complete the request on
    // completion.  If we're at a sector boundary, then we'll need to erase the
    // next sector.  Otherwise, return to this routine in order to flash the
    // next byte.
    //

    if (IrpSp->Parameters.IdexReadWrite.Length == 0) {
        FinishIoRoutine = IdexMediaBoardFinishGeneric;
    } else if ((IrpSp->Parameters.IdexReadWrite.Length & IDE_ATA_SECTOR_MASK) == 0) {
        IrpSp->Parameters.IdexReadWrite.SectorNumber++;
        FinishIoRoutine = IdexMediaBoardFinishBootROMEraseSector;
    } else {
        FinishIoRoutine = IdexMediaBoardFinishBootROMWriteFlash;
    }

    //
    // Indicate that we're expecting an interrupt for this IRP.
    //

    ASSERT(IdexChannelObject.InterruptRoutine == NULL);

    IdexChannelObject.InterruptRoutine = IdexMediaBoardNoTransferInterrupt;
    IdexChannelObject.FinishIoRoutine = FinishIoRoutine;
    IdexChannelObject.Timeout = IDE_ATA_DEFAULT_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

VOID
IdexMediaBoardFinishBootROMEraseSector(
    VOID
    )
/*++

Routine Description:

    This routine erases a sector of the boot ROM.  This must be done prior to
    flashing new data into the boot ROM.

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

    ASSERT((IrpSp->Parameters.IdexReadWrite.Length & IDE_ATA_SECTOR_MASK) == 0);

    //
    // Jump to the generic handler to process errors.
    //

    if (!NT_SUCCESS(Irp->IoStatus.Status)) {
        IdexMediaBoardFinishGeneric();
        return;
    }

    //
    // Synchronize execution with the interrupt service routine.
    //

    IdexRaiseIrqlToChannelDIRQLFromDPCLevel();

    //
    // Select the IDE device and spin until the device is not busy.
    //

    if (!IdexMediaBoardSelectDeviceAndSpinWhileBusy()) {
        IdexMediaBoardFinishGeneric();
        return;
    }

    //
    // Issue the IDE command.
    //

    IdexProgramLBATransfer(IDE_MEDIA_BOARD_DEVICE_NUMBER,
        IrpSp->Parameters.IdexReadWrite.SectorNumber, 0);
DbgPrint("erase sector=%08x\n", IrpSp->Parameters.IdexReadWrite.SectorNumber);

    IdexWriteCommandPort(MEDIA_BOARD_COMMAND_BOOT_ERASE);

    //
    // Indicate that we're expecting an interrupt for this IRP.
    //

    ASSERT(IdexChannelObject.InterruptRoutine == NULL);

    IdexChannelObject.InterruptRoutine = IdexMediaBoardNoTransferInterrupt;
    IdexChannelObject.FinishIoRoutine = IdexMediaBoardFinishBootROMWriteFlash;
    IdexChannelObject.Timeout = IDE_ATA_DEFAULT_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

VOID
IdexMediaBoardFinishReadWrite(
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
        IdexMediaBoardFinishGeneric();
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
IdexMediaBoardStartReadWrite(
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
    PIDE_MEDIA_BOARD_EXTENSION MediaBoardExtension;
    ULONG TransferLength;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // If this is a write request to the boot ROM, then we need to go down a
    // different code path.  The boot ROM doesn't implement the write DMA
    // command.
    //

    if (IrpSp->MajorFunction == IRP_MJ_WRITE) {

        MediaBoardExtension =
            (PIDE_MEDIA_BOARD_EXTENSION)IrpSp->DeviceObject->DeviceExtension;

        if (MediaBoardExtension->PartitionInformation.PartitionNumber ==
            MEDIA_BOARD_BOOT_ROM_PARTITION) {

            //
            // No retries are allowed for this IRP.
            //

            IdexChannelObject.MaximumIoRetries = IDE_NO_RETRY_COUNT;

            //
            // Initialize the status of the operation to success.
            //

            Irp->IoStatus.Status = STATUS_SUCCESS;

            //
            // Enter the state machine for writing to the boot ROM.
            //

__asm int 3;
            IdexMediaBoardFinishBootROMEraseSector();

            return;
        }
    }

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

    if (!IdexMediaBoardSelectDeviceAndSpinWhileBusy()) {
        IdexMediaBoardFinishReadWrite();
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

    IdexProgramLBATransfer(IDE_MEDIA_BOARD_DEVICE_NUMBER,
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

    IdexChannelObject.InterruptRoutine = IdexMediaBoardTransferInterrupt;
    IdexChannelObject.FinishIoRoutine = IdexMediaBoardFinishReadWrite;
    IdexChannelObject.ExpectingBusMasterInterrupt = TRUE;
    IdexChannelObject.Timeout = IDE_ATA_DEFAULT_TIMEOUT;

    IdexLowerIrqlFromChannelDIRQL(DISPATCH_LEVEL);
}

NTSTATUS
IdexMediaBoardReadWrite(
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
    PIDE_MEDIA_BOARD_EXTENSION MediaBoardExtension;
    PIO_STACK_LOCATION IrpSp;
    LONGLONG StartingByteOffset;
    LONGLONG EndingByteOffset;

    MediaBoardExtension = (PIDE_MEDIA_BOARD_EXTENSION)DeviceObject->DeviceExtension;
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
                MediaBoardExtension->PartitionInformation.PartitionLength.QuadPart)) {

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
            MediaBoardExtension->PartitionInformation.PartitionLength.QuadPart);
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

    StartingByteOffset += MediaBoardExtension->PartitionInformation.StartingOffset.QuadPart;

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

NTSTATUS
IdexMediaBoardGetDriveGeometry(
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
    DiskGeometry->Cylinders.QuadPart = IdexMediaBoardUserAddressableSectors;
    DiskGeometry->MediaType = FixedMedia;
    DiskGeometry->TracksPerCylinder = 1;
    DiskGeometry->SectorsPerTrack = 1;
    DiskGeometry->BytesPerSector = IDE_ATA_SECTOR_SIZE;

    Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);

    return STATUS_SUCCESS;
}

NTSTATUS
IdexMediaBoardGetPartitionInfo(
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
    PIDE_MEDIA_BOARD_EXTENSION MediaBoardExtension;
    PIO_STACK_LOCATION IrpSp;
    PPARTITION_INFORMATION PartitionInformation;

    MediaBoardExtension = (PIDE_MEDIA_BOARD_EXTENSION)DeviceObject->DeviceExtension;
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

    *PartitionInformation = MediaBoardExtension->PartitionInformation;

    Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);

    return STATUS_SUCCESS;
}

NTSTATUS
IdexMediaBoardDeviceControl(
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
            status = IdexMediaBoardGetDriveGeometry(DeviceObject, Irp);
            break;

        case IOCTL_DISK_GET_PARTITION_INFO:
            status = IdexMediaBoardGetPartitionInfo(DeviceObject, Irp);
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
IdexMediaBoardStartIo(
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
            IdexMediaBoardStartReadWrite(Irp);
            break;

        case IRP_MJ_DEVICE_CONTROL:
            switch (IrpSp->Parameters.DeviceIoControl.IoControlCode) {

                case IOCTL_IDE_PASS_THROUGH:
                    IdexChannelStartIdePassThrough(Irp,
                        IDE_MEDIA_BOARD_DEVICE_NUMBER, IdexMediaBoardResetDevice);
                    break;

                default:
                    IdexBugCheck(IDE_BUG_CHECK_MEDIA_BOARD, Irp);
                    break;
            }
            break;

        default:
            IdexBugCheck(IDE_BUG_CHECK_MEDIA_BOARD, Irp);
            break;
    }
}

VOID
IdexMediaBoardFatalError(
    IN ULONG ErrorCode
    )
/*++

Routine Description:

    This routine is called when a fatal media board device related error is
    detected.

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
IdexMediaBoardCreateDevice(
    IN POBJECT_STRING DeviceName,
    IN POBJECT_STRING DosDevicesName,
    IN ULONG StartingSector,
    IN ULONG NumberOfSectors,
    IN ULONG PartitionNumber,
    IN BOOLEAN RawMountOnly
    )
/*++

Routine Description:

    This routine constructs and initializes a media board device object.

Arguments:

    DeviceName - Specifies the name of the device object.

    DosDevicesName - Specifies the DOS devices name of the device object.

    StartingSector - Specifies the starting sector number of the partition.

    NumberOfSectors - Specifies the number of sectors in the partition.

    PartitionNumber - Specifies the partition number.

    RawMountOnly - Specifies TRUE if the device should only be mounted with the
        RAW file system, else FALSE.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    PDEVICE_OBJECT MediaBoardDeviceObject;
    PIDE_MEDIA_BOARD_EXTENSION MediaBoardExtension;

    //
    // Create the media board's device object.
    //

    status = IoCreateDevice(&IdexMediaBoardDriverObject,
        sizeof(IDE_MEDIA_BOARD_EXTENSION), DeviceName, FILE_DEVICE_MEDIA_BOARD,
        FALSE, &MediaBoardDeviceObject);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_MEDIA_BOARD, status);
    }

    //
    // Create the \DosDevices symbolic link.
    //

    status = IoCreateSymbolicLink(DosDevicesName, DeviceName);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_MEDIA_BOARD, status);
    }

    //
    // Mark the device object as support direct I/O so that user buffers are
    // passed down to us as locked buffers.  Also indicate that we can accept
    // scatter/gather buffers.
    //

    MediaBoardDeviceObject->Flags |= DO_DIRECT_IO | DO_SCATTER_GATHER_IO;

    //
    // Indicate that the disk itself should only be mounted with the RAW file
    // system, if appropriate.
    //

    if (RawMountOnly) {
        MediaBoardDeviceObject->Flags |= DO_RAW_MOUNT_ONLY;
    }

    //
    // Set the alignment requirement for the device.
    //

    MediaBoardDeviceObject->AlignmentRequirement = IDE_ALIGNMENT_REQUIREMENT;

    //
    // Set the size of a sector in bytes for the device.
    //

    MediaBoardDeviceObject->SectorSize = IDE_ATA_SECTOR_SIZE;

    //
    // Initialize the media board's device extension data.
    //

    MediaBoardExtension = (PIDE_MEDIA_BOARD_EXTENSION)MediaBoardDeviceObject->DeviceExtension;

    MediaBoardExtension->DeviceObject = MediaBoardDeviceObject;

    //
    // Compute the partition information for the device.
    //

    MediaBoardExtension->PartitionInformation.StartingOffset.QuadPart =
        ((ULONGLONG)StartingSector * IDE_ATA_SECTOR_SIZE);
    MediaBoardExtension->PartitionInformation.PartitionLength.QuadPart =
        ((ULONGLONG)NumberOfSectors * IDE_ATA_SECTOR_SIZE);
    MediaBoardExtension->PartitionInformation.PartitionNumber = PartitionNumber;

    //
    // The device has finished initializing and is ready to accept requests.
    //

    MediaBoardDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
}

VOID
IdexMediaBoardCreateQuick(
    VOID
    )
/*++

Routine Description:

    This routine initializes the media board device subsystem after a quick
    reboot.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE MediaBoardDirectoryHandle;
    ULONG FileSystemAreaSectorCount;
    ULONG BootROMSectorCount;

    //
    // Create the static media board directory that will contain our device
    // objects.
    //

    InitializeObjectAttributes(&ObjectAttributes, &IdexMediaBoardDirectoryName,
        OBJ_PERMANENT | OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtCreateDirectoryObject(&MediaBoardDirectoryHandle,
        &ObjectAttributes);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_MEDIA_BOARD, status);
    }

    NtClose(MediaBoardDirectoryHandle);

    //
    // Create the file system area partition object.  For the FPGA board, we
    // have to remove some of the sectors from the end of the partition for the
    // communication area.
    //

    FileSystemAreaSectorCount = IdexMediaBoardUserAddressableSectors;

    if (IdexMediaBoardFPGABoard) {
        FileSystemAreaSectorCount -= MEDIA_BOARD_COMMUNICATION_AREA_SECTOR_COUNT;
    }

    IdexMediaBoardCreateDevice(&IdexMediaBoardFsDeviceName,
        &IdexMediaBoardFsDosDevicesName, MEDIA_BOARD_FILE_SYSTEM_AREA_START_LBA,
        FileSystemAreaSectorCount, 0, FALSE);

    //
    // Create the communication area partition object.  For the FPGA board, the
    // communication area is at the top of the DIMM memory.  For the ASIC board,
    // the communication area is in its own address range.
    //

    if (IdexMediaBoardFPGABoard) {
        IdexMediaBoardCreateDevice(&IdexMediaBoardCommunicationDeviceName,
            &IdexMediaBoardCommunicationDosDevicesName,
            FileSystemAreaSectorCount,
            MEDIA_BOARD_COMMUNICATION_AREA_SECTOR_COUNT, 1, TRUE);
    } else {
        IdexMediaBoardCreateDevice(&IdexMediaBoardCommunicationDeviceName,
            &IdexMediaBoardCommunicationDosDevicesName,
            MEDIA_BOARD_COMMUNICATION_AREA_ASIC_START_LBA,
            MEDIA_BOARD_COMMUNICATION_AREA_SECTOR_COUNT, 1, TRUE);
    }

    //
    // Create the boot ROM partition object.
    //

    BootROMSectorCount = (IdexMediaBoardFPGABoard ?
        MEDIA_BOARD_BOOT_ROM_FPGA_SECTOR_COUNT :
        MEDIA_BOARD_BOOT_ROM_ASIC_SECTOR_COUNT);

    IdexMediaBoardCreateDevice(&IdexMediaBoardBootROMDeviceName,
        &IdexMediaBoardBootROMDosDevicesName, MEDIA_BOARD_BOOT_ROM_START_LBA,
        BootROMSectorCount, 2, TRUE);

    //
    //
    //

    status = IoCreateSymbolicLink(&IdexMediaBoardCdRomDeviceName,
        &IdexMediaBoardFsDeviceName);

    if (!NT_SUCCESS(status)) {
        IdexBugCheck(IDE_BUG_CHECK_MEDIA_BOARD, status);
    }
}

VOID
IdexMediaBoardCreate(
    VOID
    )
/*++

Routine Description:

    This routine initializes the media board device subsystem.

Arguments:

    None.

Return Value:

    None.

--*/
{
    ULONG Retries;
    LARGE_INTEGER Interval;
    USHORT ChipRevision;
    PCI_SLOT_NUMBER PCISlotNumber;
    UCHAR PCIData;
    USHORT DimmSize;
    ULONG UserAddressableSectors;

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

            if (IdexMediaBoardPollResetComplete()) {
                break;
            }

            //
            // Delay for 50 milliseconds.
            //

            Interval.QuadPart = -50 * 10000;
            KeDelayExecutionThread(KernelMode, FALSE, &Interval);
        }

        if (Retries == 0) {
            IdexDbgPrint(("IDEX: media board timed out during reset.\n"));
            IdexMediaBoardFatalError(FATAL_ERROR_HDD_TIMEOUT);
        }
    }

    //
    // Check the chip revision of the media board.  Different versions of the
    // media board operate at different DMA and PIO transfer modes.
    //

    PCISlotNumber.u.AsULONG = 0;
    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_IDE_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_IDE_FUNCTION_ID;

    ChipRevision = IdexReadPortUshort(SEGA_REGISTER_CHIP_REVISION);

    if ((ChipRevision & SEGA_CHIP_REVISION_CHIP_ID_MASK) ==
        SEGA_CHIP_REVISION_FPGA_CHIP_ID) {

        //
        // Remember that this is a FPGA board.
        //

        IdexMediaBoardFPGABoard = TRUE;

        //
        // Disable UDMA access.
        //

        PCIData = 0x00;
        HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x62, &PCIData, sizeof(UCHAR));

        //
        // Change the DMA transfer mode to MDMA mode 0.
        //

        PCIData = 0x77;
        HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x5A, &PCIData, sizeof(UCHAR));
    }

    //
    // Determine the number of user addressable sectors given the DIMM size
    // reported by the media board.
    //

    DimmSize = IdexReadPortUshort(SEGA_REGISTER_DIMM_SIZE);

    switch (DimmSize & SEGA_DIMM_SIZE_SIZE_MASK) {

        case SEGA_DIMM_SIZE_128M:
            UserAddressableSectors = ((128 * 1024 * 1024) >> IDE_ATA_SECTOR_SHIFT);
            break;

        case SEGA_DIMM_SIZE_256M:
            UserAddressableSectors = ((256 * 1024 * 1024) >> IDE_ATA_SECTOR_SHIFT);
            break;

        case SEGA_DIMM_SIZE_512M:
            UserAddressableSectors = ((512 * 1024 * 1024) >> IDE_ATA_SECTOR_SHIFT);
            break;

        case SEGA_DIMM_SIZE_1024M:
            UserAddressableSectors = ((1024 * 1024 * 1024) >> IDE_ATA_SECTOR_SHIFT);
            break;

        default:
            __assume(0);
    }

    IdexMediaBoardUserAddressableSectors = UserAddressableSectors;

    //
    // At this point, we can use the quick boot creation code.
    //

    IdexMediaBoardCreateQuick();
}

//
// Driver object for the media board object.
//
DECLSPEC_RDATA DRIVER_OBJECT IdexMediaBoardDriverObject = {
    IdexMediaBoardStartIo,              // DriverStartIo
    NULL,                               // DriverDeleteDevice
    NULL,                               // DriverDismountVolume
    {
        IdexDriverIrpReturnSuccess,     // IRP_MJ_CREATE
        IdexDriverIrpReturnSuccess,     // IRP_MJ_CLOSE
        IdexMediaBoardReadWrite,        // IRP_MJ_READ
        IdexMediaBoardReadWrite,        // IRP_MJ_WRITE
        IoInvalidDeviceRequest,         // IRP_MJ_QUERY_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_SET_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_FLUSH_BUFFERS
        IoInvalidDeviceRequest,         // IRP_MJ_QUERY_VOLUME_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_DIRECTORY_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_FILE_SYSTEM_CONTROL
        IdexMediaBoardDeviceControl,    // IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_SHUTDOWN
        IoInvalidDeviceRequest,         // IRP_MJ_CLEANUP
    }
};
