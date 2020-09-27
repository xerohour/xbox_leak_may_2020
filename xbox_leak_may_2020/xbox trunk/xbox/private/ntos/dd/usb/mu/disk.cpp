/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    disk.c

Abstract:

    MU disk and partition routines. Top level interface of MU driver

Environment:

    kernel mode

Revision History:

    06/12/2000 - started re-write - georgioc 
    03/08/20001 - arbitray media block size support, see comments - mitchd

--*/


/*++

    03/08/2001 - MEDIA BLOCK SIZES.  Flash ROM media in used in the Memory
                 Units has two relevant lengths: a media page size and a
                 media block size.  The media page size is the smallest
                 unit of storage that can be read from or written to.  The
                 catch is that write doesn't really write it just ORs.
                 The media block size is the smallest block unit of storage
                 that can be erased.

                 A typical media page size is 512 bytes, and a typical media
                 block size is 8k for an 8 MB media and larger for larger media.
                 On all products for Windows, the FW in the media drives report the
                 SCSI Logical Block Size as their page size.  The devices manage
                 the media block internally using on-board RAM to buffer writes
                 as needed.  In addition to supporting much smaller logical
                 block sizes, the extra RAM helps them improve through-put.  An
                 erase operation takes significant time.  Without on-board RAM
                 a write command stalls while blocks are being erased.  With enough 
                 on-board the write is buffered during the erase and transfer proceeds
                 at the USB bandwidth limit.  Still the media write (and read) is
                 comparable and often slower than the USB bus.  Again the RAM helps
                 by allowing multiple blocks to be read to and written from simultaneously.
                 With a well implemented media drive, the USB bus is definately the
                 bottle-neck.

                 On Xbox, COGS were a signficant issue since the media drive and the
                 media are both sold as a single integrated unit.  Thus there is no
                 tolerance for added cost to the drive.  RAM was reduced to a minimum
                 and the Xbox side Memory Unit driver (this code) manages the media block
                 size.  The FW is designed to erase the relevant blocks with any write.
                 So all writes must be to full media blocks, or there adjacent media pages
                 will be erased.  A simple solution is to increase the sector size reported
                 to the filesystem to the media block size.  Unfortunately, the filesystem
                 only supports sector sizes up to one x86 page of 4 kilobytes, while the
                 media block size are typically larger than this.  So writes to less than
                 a full block are supported by reading the remaining portion of the block
                 and writing the whole block out.

                 The initial implementation was designed around the 8 MB media with 8 kbyte
                 media block sizes.  The simplifying assumption that the block size is
                 twice the 4k sector size of the filesystem was used extensively. However,
                 it has become clear that media prices are dropping and larger media (With
                 larger block sizes) are becoming available.

                 This revision addresses this issue by generalizing the scheme to support any
                 media block size that is a positive integral multiple of the 4k sector size.
                 Since the driver is not allowed to dynamically allocate memory according to
                 the MU size, for predictability reasons, a maximum buffer size and thus maximum
                 media block size must be set at compile time of XSS.  That limit has been
                 chosen as of this date to be 16 kbyte.  The code has been written so that this
                 limit is simply a #define and can be changed at any time.  However, if units
                 with larger block sizes become available, the games compiled with the
                 smaller maximum block size will refuse to mount them.  Unfortunately, it is
                 much more difficult to completely hide the units from these games.

                 All writes are broken up into up to three of four phases.  An initial unaligned
                 portion, a whole portion that is completely aligned, and a final portion that
                 is unaligned on the end.  The fourth type I have dubbed a "middle partial portion"
                 which is a small write that begins and ends within a single media block and is
                 not aligned to either end of the block.

                 Partial Middle Writes have their own state machine that reads the portion before,
                 then the portion after and then spits out the whole block.

                 Other writes, send the whole poriton first (if there is one), and then if there
                 was an initial or final portion enter a state machine that handles the initial
                 and final portions in sequence.

                 In order to minimize buffer memory there is one global buffer for partial writes.
                 Requests to either state machine are queued in a single global queue.  Note that
                 since requests for a given device are also queued, there can never be more than
                 one request per device in the partial write queue.  Both state machines, whenever
                 they complete a partial write, pull the next request off the partial write queue
                 and start in the proper state machine.

                 See the comments at the top mrb.cpp for discussion of changes to the mrb state
                 machine (that implements the MU Bulk-Only protocol) to better accomodate the
                 needs of the more generalized buffering.

--*/

//*******************************************************************************
// Includes
//*******************************************************************************
#include "mu.h"

//*******************************************************************************
// local function declarations
//*******************************************************************************
DEFINE_USB_DEBUG_FUNCTIONS("MU");

VOID
MU_fDiskDeviceControl(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN PIRP Irp
    );

NTSTATUS
FASTCALL
MU_fDiskReadDriveCapacity(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    PIRP Irp
    );

VOID
MU_DiskReadCapacityCompletion(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    );

VOID
FASTCALL
MU_fDiskVerify(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN PIRP Irp
    );

VOID
MU_DiskReadComplete(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    );

VOID
MU_DiskIoComplete(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    );

VOID
FASTCALL
MU_fDiskReadWrite(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN PIRP Irp
    );

VOID
MU_DiskStartPartialWrite(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    );

VOID
MU_DiskWriteBuildPartialRequest(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    );

VOID
MU_DiskWriteBuildMiddlePartialRequest(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    );

VOID
FASTCALL
MU_fMarkWriteBufferCorrupt(
    IN PMU_REQUEST_BLOCK Mrb
    );


VOID
MU_DiskWriteStartNextPartialRequest();




//*******************************************************************************
// Declarations for diagnostic IOCTL's
//*******************************************************************************

#ifdef MU_DIAGNOSTIC_IOCTLS

NTSTATUS
FASTCALL         
MU_fVscCommand(
 IN PMU_DEVICE_EXTENSION DeviceExtension,
 IN PIRP Irp,
 IN BYTE bRequest,
 IN ULONG ulOutputSize
 );


VOID
MU_VscComplete(
    IN PURB   Urb,
    IN PVOID  Context
    );

#endif


//*******************************************************************************
// debug write log stuff
//*******************************************************************************
#if DBG

    #define WRITE_LOG_LENGTH 2000
    ULONG WriteLog[WRITE_LOG_LENGTH];
	ULONG WriteBlock[WRITE_LOG_LENGTH];
    LONG  WriteCount = 0;

    ULONG MU_GetWriteSize(LONG count) { return WriteLog[count]; }
	ULONG MU_GetWriteBlock(LONG count) { return WriteBlock[count]; }
    LONG MU_GetWriteCount() { return WriteCount; }

#endif

//*******************************************************************************
// Implementation
//*******************************************************************************
NTSTATUS
MU_InternalIo (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
/*++
    Routine Description:
    
      Entry Point for:
        IRP_MJ_READ
        IRP_MJ_WRITE
        IRP_MJ_DEVICE_CONTROL
        IRP_MJ_INTERNAL_DEVICE

    In other words, excepting IRP_MJ_CREATE and IRP_MJ_CLOSE, all
    supported IRPs go through here.

    Since all USB requests to a single device are serialized by the bulk-only
    protocol anyway, we just serialize all the IRPs coming in with a device queue.


--*/
{
    PMU_DEVICE_EXTENSION deviceExtension;
    KIRQL                oldIrql;
    NTSTATUS             ntStatus = STATUS_PENDING;

    USB_DBG_ENTRY_PRINT(("MU_InternalIo(DeviceObject=0x%0.8x,Irp=0x%0.8x)", DeviceObject, Irp));

    deviceExtension = (PMU_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Other parts of the system (XAPI, or intelligent private caller of MU_CreateDeviceObject
    //  and MU_CloseDeviceObject) are supposed to ensure that we do not have outstanding I/O
    //  before calling MU_CloseDeviceObject, and that no new I/O will be initiated after calling
    //  MU_CloseDeviceObject.  We will just ASSERT that this is so.
    //

    ASSERT(!TEST_FLAG(deviceExtension->DeviceFlags, DF_PENDING_CLOSE));

    //
    //  Ensure that device is connected.
    //
    if(TEST_FLAG(deviceExtension->DeviceFlags, DF_REMOVED|DF_PENDING_REMOVE))
    {
        ntStatus = STATUS_DEVICE_NOT_CONNECTED;
        Irp->IoStatus.Status = ntStatus;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        USB_DBG_WARN_PRINT(("An I/O Request has been sent to an MU which is removed"));
    } else
    {
        //
        //  Mark the Irp Pending and add it to the device queue.
        //
        IoMarkIrpPending(Irp);
        IoStartPacket(DeviceObject,
                      Irp,
                      NULL);
    }
    KeLowerIrql(oldIrql);

    USB_DBG_EXIT_PRINT(("MU_InternalIo returning 0x%0.8x", ntStatus));
    return ntStatus;
}

VOID
MU_StartIo (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
/*++
    Routine Description:
        Typical StartIo routine.  Handles one request at a time
        and dispatches it.
--*/
{
    PMU_DEVICE_EXTENSION   deviceExtension;
    PIO_STACK_LOCATION  irpStack;

    USB_DBG_ENTRY_PRINT(("MU_StartIo(DeviceObject=0x%0.8x,Irp=0x%0.8x)", DeviceObject, Irp));

    deviceExtension = (PMU_DEVICE_EXTENSION) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    USB_DBG_ENTRY_PRINT(("MU_StartIo(0x%0.8x(0x%0.8x), 0x%0.8x)", DeviceObject, deviceExtension, Irp));

    //
    //  Other parts of the system (XAPI, or intelligent private caller of MU_CreateDeviceObject
    //  and MU_CloseDeviceObject) are supposed to ensure that we do not have outstanding I/O
    //  before calling MU_CloseDeviceObject, and that no new I/O will be initiated after calling
    //  MU_CloseDeviceObject.  We will just ASSERT that this is so.
    //

    ASSERT(!TEST_FLAG(deviceExtension->DeviceFlags, DF_PENDING_CLOSE));

    //
    //  If the console is preparing to reset or shutdown, there's no reason to
    //  continue processing this request.
    //

    if (HalIsResetOrShutdownPending())
    {
        Irp->IoStatus.Status = STATUS_REQUEST_ABORTED;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        IoStartNextPacket(DeviceObject);
        return;
    }

    //
    //  Store our device object in the
    //  our stack so we can get back at it.
    //  (This is used by the partial write queue
    //  to relate an Irp back to its device object.)
    //

    irpStack->DeviceObject = DeviceObject;

    //
    //  Save the IRP we are working on
    //  so it doesn't need to be passed everywhere.
    //

    deviceExtension->PendingIrp = Irp;

    //
    //  For debug builds only set a watchdog timer
    //  on the IRP.
    //
    MU_DEBUG_SET_WATCHDOG(deviceExtension);

    //
    //  Handle\Dispatch the request
    //

    switch (irpStack->MajorFunction)
    {
      case IRP_MJ_DEVICE_CONTROL:
        MU_fDiskDeviceControl(deviceExtension,Irp);
        break;
      case IRP_MJ_WRITE:
        IoMarkIrpMustComplete(Irp);
        // FALL THROUGH
      case IRP_MJ_READ:
        MU_fDiskReadWrite(deviceExtension,Irp);
        break;
      default:
        ASSERT("MU called with unsupported I/O Request.");
    }

    USB_DBG_EXIT_PRINT(("MU_StartIo returning"));
    return;
}

VOID
MU_fDiskDeviceControl(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by MU_StartIo to handle
    IRP_MJ_DEVICE_CONTROL sent to an MU.
        
    The following IOCTLs are supported, others spew ERROR strings:
        IOCTL_DISK_GET_DRIVE_GEOMETRY
        IOCTL_DISK_GET_PARTITION_INFO
        IOCTL_DISK_VERIFY

    Whenever possible these are satisified with cached information.
    Otherwise, they are dispatched to a routine which builds and
    submits an Mrb to the MRB state machine.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None

--*/
{
    NTSTATUS               status;
    PDISK_GEOMETRY         diskGeometry;
    PPARTITION_INFORMATION partitionInformation;
    PIO_STACK_LOCATION     irpStack = IoGetCurrentIrpStackLocation(Irp);


    //
    //  Most of the time, the request
    //  will just be pending, so set it here,
    //  and change it if necessary when processing
    //  specific cases.
    //

    status = STATUS_PENDING;

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
    {
      case IOCTL_DISK_GET_DRIVE_GEOMETRY:

        diskGeometry = (PDISK_GEOMETRY)Irp->UserBuffer;
        //debug parameter check
        ASSERT(sizeof(DISK_GEOMETRY) <= irpStack->Parameters.DeviceIoControl.OutputBufferLength);

        //
        //  If the Partition Length is zero, we need
        //  to fetch the capacity from the device.
        //
        if (DeviceExtension->PartitionLength.QuadPart == 0)
        {
            //
            //  MU_fDiskReadDriveCapacity doesn't
            //  return a status, it always, pends
            //  the IRP.
            //

            USB_DBG_TRACE_PRINT(("GetDriveGeometry, issuing read capacity"));
            status = MU_fDiskReadDriveCapacity(DeviceExtension,Irp);

        } else
        //
        //  We already know the geometry, just copy it.
        //
        {

            RtlCopyMemory(diskGeometry, &DeviceExtension->DiskGeometry,
                          sizeof(DISK_GEOMETRY));

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
        }

        break;        

      case IOCTL_DISK_GET_PARTITION_INFO:

        //debug parameter check
        ASSERT(sizeof(PARTITION_INFORMATION) <= irpStack->Parameters.DeviceIoControl.OutputBufferLength);

        //
        // Fill in the output buffer and return.
        //

        partitionInformation = (PPARTITION_INFORMATION)Irp->UserBuffer;
        RtlZeroMemory(partitionInformation, sizeof(PARTITION_INFORMATION));

        //
        //  If the Partition Length is zero, we need
        //  to fetch the capacity from the device.
        //

        if (DeviceExtension->PartitionLength.QuadPart == 0) {

            //
            //  MU_fDiskReadDriveCapacity doesn't
            //  return a status, it always pends
            //  the IRP.
            //

            USB_DBG_TRACE_PRINT(("GetPartitionInfo, issuing read capacity"));
            status = MU_fDiskReadDriveCapacity(DeviceExtension,Irp);

        } else
        //
        //  We already know the partition length, just copy it.
        //
        {
            partitionInformation->PartitionLength = DeviceExtension->PartitionLength;
            partitionInformation->RecognizedPartition = TRUE;
            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);
        }

        break;

      case IOCTL_DISK_VERIFY:
        
        //
        //  MU_fDiskVerify doesn't return status, it always
        //  pends the IRP.
        //
        MU_fDiskVerify(DeviceExtension, Irp);
        break;

        //
        //  The following requests are for running
        //  diagnostics on the memory unit and require
        //  a special build.
        //

#ifdef MU_DIAGNOSTIC_IOCTLS
      case MU_IOCTL_GET_BAD_BLOCK_TABLE:
         
        ASSERT(Irp->UserBuffer && "MU_IOCTL_GET_BAD_BLOCK_TABLE");
         
        ASSERT((MU_VSC_BAD_BLOCK_TABLE_SIZE <= irpStack->Parameters.DeviceIoControl.OutputBufferLength) &&
                "MU_IOCTL_GET_BAD_BLOCK_TABLE");

        //
        //  MU_VSC_GET_BAD_BLOCK_TABLE, must always be proceeded
        //  by a read capacity.
        //
        status = MU_fDiskReadDriveCapacity(DeviceExtension,Irp);
        break;

      case MU_IOCTL_MEMORY_TEST:
   
        ASSERT(Irp->UserBuffer && "MU_IOCTL_MEMORY_TEST");
         
        ASSERT((MU_VSC_BAD_BLOCK_COUNT_SIZE <= irpStack->Parameters.DeviceIoControl.OutputBufferLength) && 
                MU_IOCTL_MEMORY_TEST);
        
        status = MU_fVscCommand(
                    DeviceExtension,
                    Irp,
                    MU_VSC_MEMORY_TEST,
                    MU_VSC_BAD_BLOCK_COUNT_SIZE
                    );
        break;
      
#endif

      default:
        USB_DBG_ERROR_PRINT((
            "MuDiskDeviceControl: disk device doesn't handle IOCTL %08x\n",
            irpStack->Parameters.DeviceIoControl.IoControlCode
            ));
        Irp->IoStatus.Information = 0;
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }
    
    //
    //  If the status is not pending, complete
    //  the current IRP and return an error
    //

    if(status != STATUS_PENDING)
    {
        Irp->IoStatus.Status = status;
        MU_COMPLETE_REQUEST(DeviceExtension, Irp, IO_NO_INCREMENT);
        IoStartNextPacket(DeviceExtension->DeviceObject);
    }
    return;    
}


NTSTATUS
FASTCALL
MU_fDiskReadDriveCapacity(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    PIRP Irp
    )
/*++
    Routine Description:
        Builds an MRB for Read Capacity and submits it
        to the MRB state machine
--*/
{
    PREAD_CAPACITY_DATA readCapacityBuffer;
    PMU_REQUEST_BLOCK   mrb;

    //
    // Allocate read capacity buffer
    //

    readCapacityBuffer = (PREAD_CAPACITY_DATA) RTL_ALLOCATE_HEAP(sizeof(READ_CAPACITY_DATA));
    
    if (!readCapacityBuffer)
    {
        DeviceExtension->PendingIrp->IoStatus.Information = 0;
        USB_DBG_WARN_PRINT(("Insufficient Memory to allocate READ_CAPACITY_DATA buffer"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Fill out the mrb
    //
 
    mrb = &DeviceExtension->Mrb;
    mrb->DataBuffer = (PUCHAR) readCapacityBuffer;
    mrb->TransferLength = sizeof(READ_CAPACITY_DATA);
    mrb->CompletionRoutine = MU_DiskReadCapacityCompletion;
    mrb->TimeOutValue = MRB_READ_CAPACITY_TIMEOUT;
    mrb->Retries = MRB_MAXIMUM_RETRIES;
    mrb->Flags = MRB_FLAGS_DATA_IN;

    //
    //  Fill out the CDB
    //

    RtlZeroMemory(&mrb->Cbw.Cdb, sizeof(CDB));
    mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_READ_CAPACITY;
    
    //
    //  Submit the MRB
    //

    MU_fStartMrb(DeviceExtension);

    return STATUS_PENDING;
} // end MuDiskReadDriveCapacity()

VOID
MU_DiskReadCapacityCompletion(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    )
/*++
    Routine Description:
        The MRB completion routine called when SCIOP_READ_CAPACITY
        returns.

        This was called either so we could complete an
        IOCTL_DISK_GET_DRIVE_GEOMETRY request or to complete
        IOCTL_DISK_GET_PARTITION_INFO.

        So we calculate both and cache them, then figure
        out which Irp is outstanding and complete it.
--*/
{

    PDISK_GEOMETRY diskGeometry;
    PPARTITION_INFORMATION partitionInformation;
    PREAD_CAPACITY_DATA readCapacityBuffer = (PREAD_CAPACITY_DATA) DeviceExtension->Mrb.DataBuffer;
    PIO_STACK_LOCATION  irpStack = IoGetCurrentIrpStackLocation(DeviceExtension->PendingIrp);

    //
    //  If the Mrb failed, then fail the Irp.  The Mrb
    //

    if(NT_ERROR(Status))
    {
        DeviceExtension->PendingIrp->IoStatus.Information = 0;
        DeviceExtension->PendingIrp->IoStatus.Status = Status;
        USB_DBG_WARN_PRINT(("ReadCapacity Transfer Failed"));
        goto MU_DiskReadCapacityCompletionCleanup;
    }
        
    ULONG bytesPerLogicalBlock = ReverseEndian(readCapacityBuffer->BytesPerLogicalBlock);
    ULONG mediaBlockSize = bytesPerLogicalBlock * 
                           ReverseEndian(readCapacityBuffer->LogicalBlocksPerMediaBlock);
    ULONG logicalBlockCount = ReverseEndian(readCapacityBuffer->LogicalBlockAddress) + 1;
    ULONGLONG totalCapacity = ((ULONGLONG)logicalBlockCount) * bytesPerLogicalBlock ;

    // The LogicalBlocksPerMediaBlock was added after the original FW, so if it is zero
    // use the default value.
    if(0 == mediaBlockSize) 
        mediaBlockSize = DEFAULT_MEDIA_BLOCK_SIZE;

    //
    //  Check the capacity data to ensure we can support the device.
    //  If any of our requirements are not met, we will fail the IRP.
    //  This effectively makes the MU unmountable.
    //  Requirements:
    //  1) BytesPerLogicalBlock - Must be a power of 2 not greater than the EMULATED_SECTOR_SIZE(=4096).
    //  2) MediaBlockSize - Must be an integer multiple of the EMULATED_SECTOR_SIZE, but not greater than
    //                      the MAXIMUM_MEDIA_BLOCK_SIZE.
    //  3) Total Capacity - must be not less than the mediaBlockSize and not greater than 2^32 = 4 GB.
    //  4) logicalBlockCount - must be non-zero.  This is an artifact of adding 1 to the LogicalBlockAddress
    //                         really it is saying that the LogicalBlockAddress may not be the maximum
    //                         value, which would exceed the capacity limit anyway.
    if(
        !IsPowerOf2(bytesPerLogicalBlock) ||
        (bytesPerLogicalBlock > EMULATED_SECTOR_SIZE) ||
        (mediaBlockSize > MAXIMUM_MEDIA_BLOCK_SIZE) ||
        (mediaBlockSize%EMULATED_SECTOR_SIZE) ||
        (0 == logicalBlockCount) ||
        (totalCapacity < mediaBlockSize) ||
        (totalCapacity > (((ULONGLONG)1)<<32))
    )
    {
        DeviceExtension->PendingIrp->IoStatus.Information = 0;
        DeviceExtension->PendingIrp->IoStatus.Status = STATUS_UNRECOGNIZED_VOLUME;
        USB_DBG_WARN_PRINT(("The logical block configuration of an MU is outside supported parameter ranges."));
        goto MU_DiskReadCapacityCompletionCleanup;
    }

    //
    //  Store media capacity and media block size
    //
    DeviceExtension->PartitionLength.QuadPart = totalCapacity;
    DeviceExtension->MediaBlockSize = mediaBlockSize;
    
    //
    // Calculate logical block to byte shift.
    //
    DeviceExtension->LogicalBlockShift = WhichBit(bytesPerLogicalBlock);
    
    //
    //  Fill out the geometry information (note that BytesPerSector
    //  is our fixed EMULATED_SECTOR_SIZE.)
    USB_DBG_TRACE_PRINT(("Geometry Buffer @ 0x%0.8x", &DeviceExtension->DiskGeometry));
    DeviceExtension->DiskGeometry.MediaType = FixedMedia;
    DeviceExtension->DiskGeometry.Cylinders.QuadPart = totalCapacity/mediaBlockSize;
    DeviceExtension->DiskGeometry.TracksPerCylinder = 1; 
    DeviceExtension->DiskGeometry.SectorsPerTrack = mediaBlockSize / EMULATED_SECTOR_SIZE;
    DeviceExtension->DiskGeometry.BytesPerSector = EMULATED_SECTOR_SIZE;

    //
    // now fill the appropriate Irp buffer results
    //

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
    {
      case IOCTL_DISK_GET_DRIVE_GEOMETRY:

        diskGeometry = (PDISK_GEOMETRY)DeviceExtension->PendingIrp->UserBuffer;
        RtlCopyMemory(diskGeometry, &DeviceExtension->DiskGeometry, sizeof(DISK_GEOMETRY));

        DeviceExtension->PendingIrp->IoStatus.Information = sizeof(DISK_GEOMETRY);
        DeviceExtension->PendingIrp->IoStatus.Status = STATUS_SUCCESS;

        break;        

      case IOCTL_DISK_GET_PARTITION_INFO:

        //
        // Fill in the output buffer and return.
        //

        partitionInformation = (PPARTITION_INFORMATION)DeviceExtension->PendingIrp->UserBuffer;

        RtlZeroMemory(partitionInformation, sizeof(PARTITION_INFORMATION));

        partitionInformation->PartitionLength = DeviceExtension->PartitionLength;
        partitionInformation->RecognizedPartition = TRUE;

        DeviceExtension->PendingIrp->IoStatus.Information = sizeof(PARTITION_INFORMATION);
        DeviceExtension->PendingIrp->IoStatus.Status = STATUS_SUCCESS;

        break;

#ifdef MU_DIAGNOSTIC_IOCTLS
      //
      //  MU_IOCTL_GET_BAD_BLOCK_TABLE requires a READ_CAPACITY to proceed it.
      //  This could be why we are here.
      //
      case MU_IOCTL_GET_BAD_BLOCK_TABLE:
        {        
          NTSTATUS status = MU_fVscCommand(
                              DeviceExtension,
                              DeviceExtension->PendingIrp,
                              MU_VSC_GET_BAD_BLOCK_TABLE,
                              MU_VSC_BAD_BLOCK_TABLE_SIZE
                              );
          if(STATUS_PENDING == status)
          {
            RTL_FREE_HEAP(readCapacityBuffer);
            return;
          }
        }        
        break;            
#endif

    }

MU_DiskReadCapacityCompletionCleanup: //error paths rejoin here for cleanup

    //
    // Deallocate read capacity buffer.
    //

    RTL_FREE_HEAP(readCapacityBuffer);

    //
    //  Complete the Irp
    //

    MU_COMPLETE_REQUEST(DeviceExtension, DeviceExtension->PendingIrp, IO_NO_INCREMENT);
    
    //
    //  Start processing the next Irp.
    //

    IoStartNextPacket(DeviceExtension->DeviceObject);

   return;
} 

VOID
FASTCALL
MU_fDiskVerify(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
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
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PVERIFY_INFORMATION verifyInfo = (PVERIFY_INFORMATION)irpStack->Parameters.DeviceIoControl.InputBuffer;
    PMU_REQUEST_BLOCK    mrb = &DeviceExtension->Mrb;

    ULONG         logicalBlockOffset;
    USHORT        logicalBlockCount;

    //
    // Verify sectors
    //

    mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_VERIFY;

    //
    //  Assert that the parameter to verify are reasonable.
    //
    ASSERT(sizeof(VERIFY_INFORMATION) <= irpStack->Parameters.DeviceIoControl.InputBufferLength);
    ASSERT(verifyInfo);
    ASSERT(0==verifyInfo->StartingOffset.QuadPart%EMULATED_SECTOR_SIZE);
    ASSERT(verifyInfo->Length);
    ASSERT(0==verifyInfo->Length%EMULATED_SECTOR_SIZE);
    
    //
    // Convert byte offset to logical block offset.
    //

    logicalBlockOffset = (ULONG)(verifyInfo->StartingOffset.QuadPart >> DeviceExtension->LogicalBlockShift);

    //
    // Convert ULONG byte count to USHORT sector count.
    //
    
    logicalBlockCount = (USHORT)(verifyInfo->Length >> DeviceExtension->LogicalBlockShift);

    //
    // Fill out the mrb
    //
 
    mrb->DataBuffer = NULL;
    mrb->TransferLength = 0;
    mrb->CompletionRoutine = MU_DiskIoComplete;
    mrb->TimeOutValue = MRB_CALC_VERIFY_TIMEOUT(verifyInfo->Length);
    mrb->Retries = MRB_MAXIMUM_RETRIES;
    mrb->Flags = MRB_FLAGS_NO_DATA_TRANSFER;
    
    //
    //  Fill out the CDB
    //

    RtlZeroMemory(&mrb->Cbw.Cdb, sizeof(CDB));
    mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_VERIFY;
    
    //
    // Move little endian values into CDB in big endian format.
    //

    mrb->Cbw.Cdb.CDB10.LogicalBlock = ReverseEndian(logicalBlockOffset);
    mrb->Cbw.Cdb.CDB10.TransferBlocks = ReverseEndian(logicalBlockCount);

    //
    //  Submit the MRB
    //

    MU_fStartMrb(DeviceExtension);

    return;
}


VOID
MU_DiskReadComplete(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    )
/*++
    Routine Description:
        Completion routine for read requests.  Added so we can check for
        the corruption pattern at the start of each EMULATED_SECTOR.
--*/
{

    PUCHAR buffer = DeviceExtension->Mrb.DataBuffer;
    PUCHAR bufferEnd = DeviceExtension->Mrb.DataBuffer + DeviceExtension->Mrb.TransferLength;
    //Loop over the buffer checking the beginning of each emulated sector
    for(; buffer < bufferEnd; buffer += EMULATED_SECTOR_SIZE)
    {
        ULONG patternOffset = 0;
        //
        //  Loop as long as the pattern matches
        //
        while( ((PULONG)buffer)[patternOffset] == MU_CORRUPT_SECTOR_PATTERN[patternOffset])
        {
            //If we have reach the end of the pattern, then there is a match
            //we need to complete with an error
            if(++patternOffset == MU_CORRUPT_SECTOR_PATTERN_ULONG_COUNT)
            {
                ULONG oldProtect = MmQueryAddressProtect(buffer);
                ULONG newProtect = ((ULONG)-1);
                USB_DBG_WARN_PRINT(("MU_DiskReadComplete Corrupt Sector Found"));

                //Only change the status, if it was a success code.
                if(NT_SUCCESS(Status)) 
                {
                    Status = STATUS_DATA_ERROR;
                }

                //erase the pattern from the return buffer (if the user doesn't
                //check the error and reads it, then they may write and this
                //will make us return more problems.  Just nip it in the bud
                //here. Fill with "FAIL" a.k.a 'LIAF'.
                //
                // BTW, even though we read into it via DMA, if it is the filesystem
                // cache it may be marked PAGE_READONLY.  If so we need to
                // switch that off first, then switch it back on.
                if(PAGE_READONLY == ((PAGE_READONLY|PAGE_READWRITE)&oldProtect))
                {
                    newProtect = oldProtect; //Use the old protection as the base.
                    newProtect &= ~PAGE_READONLY; //clear readonly
                    newProtect |= PAGE_READWRITE; //set readwrite
	                MmSetAddressProtect(buffer, MU_CORRUPT_SECTOR_PATTERN_SIZE, newProtect);
                }
                
                while(patternOffset--)
                {
                    ((PULONG)buffer)[patternOffset] = 'LIAF';
                }
        
                if(((ULONG)-1) != newProtect)
                {
                    MmSetAddressProtect(buffer, MU_CORRUPT_SECTOR_PATTERN_SIZE, oldProtect);
                }
                break;
            }
        }
    }

    MU_DiskIoComplete(DeviceExtension, Status);
    return;
}


VOID
MU_DiskIoComplete(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    )
/*++
    Routine Description:
        General compeltion routine for MRB requests that require no post processing.
        The status is checked and the Irp is completed accordingly.  On success,
        Information is set to the length of the transfer, on failure it is set to
        zero.

    This is used as the completion for MU_fDiskVerify and MU_fDiskRead.  Unfortunately,
    it cannot be worked into the write state machine.
--*/
{
    DeviceExtension->PendingIrp->IoStatus.Status = Status;
    if(NT_ERROR(Status))
    {
        DeviceExtension->PendingIrp->IoStatus.Information = 0;
        USB_DBG_WARN_PRINT(("MU_DiskIoComplete Transfer Failed"));
    } else
    {
        DeviceExtension->PendingIrp->IoStatus.Information = DeviceExtension->Mrb.TransferLength;
    }
    
    //
    //  Complete the request
    //

    MU_COMPLETE_REQUEST(DeviceExtension, DeviceExtension->PendingIrp, IO_NO_INCREMENT);
    
    //
    //  Start processing the next Irp.
    //
    IoStartNextPacket(DeviceExtension->DeviceObject);
}


VOID
FASTCALL
MU_fDiskReadWrite(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN PIRP Irp
    )
/*++
    Routine Description:
       Does the basic work of setting up a Read or Write to determine if
       a the operation may be performed, to calculate the logical block
       offset and count. On read submits the Mrb and sets MU_DiskIoComplete
       as the completion routine.  On write, it needs to worry about
       aligment issues and to submit the request to the MrbState machine.
--*/
{
    PIO_STACK_LOCATION  irpStack = IoGetCurrentIrpStackLocation(Irp);
    PUCHAR              virtualAddress = NULL;

    ULONG               byteOffset = irpStack->Parameters.Read.ByteOffset.LowPart;
    ASSERT(0==irpStack->Parameters.Read.ByteOffset.HighPart);

    ULONG               transferLength = irpStack->Parameters.Read.Length;

    //
    //  Check the byteOffset and transferLength against the disk
    //  partition length, also double check that they are properly
    //  sector aligned.
    //
    if(
        (0!=byteOffset%EMULATED_SECTOR_SIZE) ||
        (0!=transferLength%EMULATED_SECTOR_SIZE) ||
        (irpStack->Parameters.Read.ByteOffset.QuadPart + transferLength) >
        DeviceExtension->PartitionLength.QuadPart)
    {
        USB_DBG_WARN_PRINT(("MU_fDiskReadWrite: failing transfer with illegal byte offset or length"));
        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
    }

    //
    //  Check for Zero length transfer, just succeed these
    //

    else if (transferLength == 0)
    {
        Irp->IoStatus.Status = STATUS_SUCCESS;
    }

    //
    //  Get the usable virtual address of the buffer to send,
    //  or receive data.
    //
    else
    {

        //
        //  SL_FSCACHE_REQUEST indicate that the transfer is to\from cached memory mapped
        //  by the buffer in the irpStack.
        //

        if(TEST_FLAG(irpStack->Flags,SL_FSCACHE_REQUEST))
        {
            virtualAddress = (PUCHAR) irpStack->Parameters.Read.CacheBuffer;

            ASSERT(virtualAddress != NULL);
        }
        else
        //
        //  SL_FSCACHE_REQUEST is not set.  The transfer is to\from the UserBuffer.
        //
        {
            virtualAddress = (PUCHAR) Irp->UserBuffer;

            ASSERT(virtualAddress != NULL);

            //
            //  Add the buffer offset to the address we were passed.
            //
            virtualAddress += irpStack->Parameters.Read.BufferOffset;
        }

        //
        //  For convience we reuse the BufferOffset to store
        //  the computed virtual address for later.
        //

        irpStack->Parameters.Read.BufferOffset = (ULONG) virtualAddress;
    }

    //
    //  If for some reason no transfer is required
    //  complete the Irp and start the next one.
    //

    if(NULL == virtualAddress)
    {
        Irp->IoStatus.Information = 0;
        MU_COMPLETE_REQUEST(DeviceExtension, Irp, IO_NO_INCREMENT);
        IoStartNextPacket(DeviceExtension->DeviceObject);
        return;
    }

    //
    //  Zero out the Cdb, we will start filling it out.
    //

    RtlZeroMemory(&DeviceExtension->Mrb.Cbw.Cdb, sizeof(CDB));
    
    //
    //  If this is a write then make then check for alignment,
    //  set flags and make appropriate adjustment.
    //

    if(IRP_MJ_WRITE == irpStack->MajorFunction)
    {

        //
        //  Clear the write state machine has flags
        //

        CLEAR_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_FLAGS);

        //
        //  Adjust the starting offset alignment.
        //

        if(byteOffset%DeviceExtension->MediaBlockSize)
        {
            ULONG initialLength = DeviceExtension->MediaBlockSize - (byteOffset%DeviceExtension->MediaBlockSize);
            DeviceExtension->InitialWriteByteCount = initialLength;
            
            // Check for a Partial Middle Write (i.e. a write
            // which starts in the middle of a media block and
            // and ends before the end of the same media block).
            if(initialLength > transferLength)
            {
                //Set the middle portion flag 
                SET_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_MIDDLE_PORTION);
                //Fix up the FinalWriteByteCount the way the middle state machine likes it.
                DeviceExtension->FinalWriteByteCount = initialLength - transferLength;
                //Start the partial write state machine.
                MU_DiskStartPartialWrite(DeviceExtension, STATUS_SUCCESS);
                return;
            }
            byteOffset += initialLength;
            virtualAddress += initialLength;
            transferLength -= initialLength;
            SET_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_INITIAL_PORTION);
            USB_DBG_TRACE_PRINT(("Write has initial portion."));
        }

        //
        //  Adjust the ending alignment
        //
        if(transferLength%DeviceExtension->MediaBlockSize)
        {
            DeviceExtension->FinalWriteByteCount = transferLength%DeviceExtension->MediaBlockSize;
            transferLength -= DeviceExtension->FinalWriteByteCount;
            SET_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_FINAL_PORTION);
            USB_DBG_TRACE_PRINT(("Write has final portion."));
        }

        //
        //  If there is no whole portion, then jump into partial
        //  transfer state machine.
        //

        if(0 == transferLength)
        {
           MU_DiskStartPartialWrite(DeviceExtension, STATUS_SUCCESS);
           return;
        }

        USB_DBG_TRACE_PRINT(("Write has whole portion."));
        //
        //  If there were alignment adjustments then set the completion
        //  routine to MU_DiskStartPartialWrite, otherwise
        //  it is a one step write and we can use MU_DiskIoComplete.
        //

        if(TEST_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_FLAGS))
        {
            DeviceExtension->Mrb.CompletionRoutine = MU_DiskStartPartialWrite;        
        } else
        {
            DeviceExtension->Mrb.CompletionRoutine = MU_DiskIoComplete;
        }

        //
        //  Setup the parts of the MRB that indicate a write request
        //

        DeviceExtension->Mrb.Flags = MRB_FLAGS_DATA_OUT;
        DeviceExtension->Mrb.Cbw.Cdb.CDB10.OperationCode = SCSIOP_WRITE;

    } else
    //
    //  This is a read request, setup the parts of the MRB
    //  that indicate a read request
    //
    {
        DeviceExtension->Mrb.CompletionRoutine = MU_DiskReadComplete;
        DeviceExtension->Mrb.Flags = MRB_FLAGS_DATA_IN;
        DeviceExtension->Mrb.Cbw.Cdb.CDB10.OperationCode = SCSIOP_READ;
    }
    
    //
    //  Setup the rest of the MRB
    //

    DeviceExtension->Mrb.DataBuffer = virtualAddress;
    DeviceExtension->Mrb.TransferLength = transferLength;
    DeviceExtension->Mrb.TimeOutValue = MRB_STANDARD_TIMEOUT;
    DeviceExtension->Mrb.Retries = MRB_MAXIMUM_RETRIES;

    //
    //  Set the block information in the CDB
    //
    DeviceExtension->Mrb.Cbw.Cdb.CDB10.LogicalBlock = ReverseEndian(byteOffset >> DeviceExtension->LogicalBlockShift);
    DeviceExtension->Mrb.Cbw.Cdb.CDB10.TransferBlocks = ReverseEndian((USHORT)(transferLength >> DeviceExtension->LogicalBlockShift));

    //
    //  Submit the MRB
    //

    MU_fStartMrb(DeviceExtension);
}

VOID
MU_DiskStartPartialWrite(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    )
/*++
    Routine Description:
        This routine is called to begin writes of partial media blocks.  It can
        be called directly by MU_fDiskReadWrite if there is no contiguous portion
        of the write or it is called as the MRB completion routine for the
        contiguous portion.  Either way, it is called at DPC level.

        Partial Write requests use a global write buffer.  As such they must be
        serialized system wide across all MUs.  This routine either begins
        the partial transfer, or it inserts the request into the queue.
--*/
{
    PIRP irp = DeviceExtension->PendingIrp;
    
    //
    //  Clear the write state machine flags
    //
    CLEAR_FLAG(DeviceExtension->DeviceFlags,MU_WRITE_STATE_BITS);

    //
    //  Make sure that the previous stage succeeded, if not
    //  just fail the Irp here.
    //
    if(NT_ERROR(Status))
    {
       irp->IoStatus.Status = Status;
       irp->IoStatus.Information = 0;
       MU_COMPLETE_REQUEST(DeviceExtension, irp, IO_NO_INCREMENT);
       IoStartNextPacket(DeviceExtension->DeviceObject);
       return;
    }

    //
    //  We can setup the common section of the MRB here
    //
    
    PMU_REQUEST_BLOCK mrb = &DeviceExtension->Mrb;
    RtlZeroMemory(&mrb->Cbw.Cdb, sizeof(CDB));
    mrb->TimeOutValue = MRB_STANDARD_TIMEOUT;
    mrb->Retries = MRB_MAXIMUM_RETRIES;

    // Choose the correct state machine, and initialize to start.
    if(TEST_FLAG(DeviceExtension->DeviceFlags,MU_WRITE_HAS_MIDDLE_PORTION))
    {
        mrb->CompletionRoutine = MU_DiskWriteBuildMiddlePartialRequest;
        SET_FLAG(DeviceExtension->DeviceFlags,MU_WRITE_STATE_MIDDLE_START);
    } else
    {
        mrb->CompletionRoutine = MU_DiskWriteBuildPartialRequest;
        SET_FLAG(DeviceExtension->DeviceFlags,MU_WRITE_STATE_START);
    }

    //
    // see if the list of pending partial requests is empty
    //

    if (IsListEmpty(&MU_DriverExtension.PartialRequestQueue))
    {
        //
        // process this one immediately
        //

        InsertHeadList(&MU_DriverExtension.PartialRequestQueue,
                       &irp->Tail.Overlay.ListEntry);

        USB_DBG_TRACE_PRINT(("StartPartialRequest: Queuing Irp at the head %x, flink %x, blink %x",
                   irp,
                   MU_DriverExtension.PartialRequestQueue.Flink,
                   MU_DriverExtension.PartialRequestQueue.Blink));
        //
        //  Start the request, call through the mrb->CompletionRoutine
        //  pointer so as to start the proper state machine.
        //

        mrb->CompletionRoutine(DeviceExtension, STATUS_SUCCESS);
        return;
       
    } else {

        ASSERT(MU_DriverExtension.PartialRequestQueue.Flink != (PLIST_ENTRY)&irp->Tail.Overlay.ListEntry);

        //
        // queue this request...
        //

        InsertTailList(&MU_DriverExtension.PartialRequestQueue, &irp->Tail.Overlay.ListEntry);

        USB_DBG_TRACE_PRINT(("StartPartialRequest: Queuing Irp %x, f %x, b %x",
                       irp,
                       MU_DriverExtension.PartialRequestQueue.Flink,
                       MU_DriverExtension.PartialRequestQueue.Blink));
    }
}

VOID
MU_DiskWriteBuildPartialRequest(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    )
/*++
    Routine Description:
        This routine is the heart of MU write state machine.  It is its own completion routine
        routine.   It builds and submits MRBs.  It works as follows:

        1) Increment the write state to the next state needed based on the DF_WRITE_HAS_XXX
        flags.

        2) switch the new state, and build an appriopriate MRB.

        3) submit the MRB.

        When the last required stage completes:
        
        4) Complete the Irp.

        5) Call IoStartNextPacket for current device

        5) Continue with the next pending partial write (for a different device)

   Note:
    There are fields of the MRB that are NOT setup here:  


    These are the same for every stage and MU_DiskStartPartialWrite before the partial portion of the request
    is queued.

--*/
{
        PIRP irp = DeviceExtension->PendingIrp;
        PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
        ULONG  byteOffset;
        ULONG  writeState = DeviceExtension->DeviceFlags&MU_WRITE_STATE_BITS;

        if(NT_SUCCESS(Status))
        {
            //
            // Increment to the next state.  
            //
            writeState += MU_WRITE_STATE_INCREMENT;
        
            //
            //  If the current stage is an initial read, and an initial
            //  read is not required, skip to final read.
            //
            if(MU_WRITE_STATE_INITIAL_READ == writeState)
            {
                if(!TEST_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_INITIAL_PORTION))
                {
                    writeState = MU_WRITE_STATE_FINAL_READ;
                }
            }

            //
            //  If the current stage is a final read, and a final
            //  read is not required, skip to done.
            //

            if(MU_WRITE_STATE_FINAL_READ == writeState)
            {
                if(!TEST_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_FINAL_PORTION))
                {
                    writeState = MU_WRITE_STATE_DONE;
                }
            }
        } else
        //
        //  On any read errors in the state machine
        //  we need to modify the buffer with a bad
        //  sector pattern pattern and proceed to
        //  the write stage anyway
        //
        {
            //
            if(
                ( (MU_WRITE_STATE_INITIAL_READ == writeState)
                  ||(MU_WRITE_STATE_FINAL_READ == writeState) ) &&
                (STATUS_DATA_ERROR == Status)
            )
            {
                MU_fMarkWriteBufferCorrupt(&DeviceExtension->Mrb);
                // Go on to the next phase
                Status = STATUS_SUCCESS;
                writeState += MU_WRITE_STATE_INCREMENT;
            } else
            //
            //  If the write failed during a write phase, or
            //  for a reason other than a STATUS_DATA_ERROR
            //  (failed CSW), then just keep the error go to
            //  the done state, which will complete the IRP
            //  with the error.
            //
            {
                writeState = MU_WRITE_STATE_DONE;
            }
        }

        //
        //  Write the writeState back
        //
        CLEAR_FLAG(DeviceExtension->DeviceFlags,MU_WRITE_STATE_BITS);
        SET_FLAG(DeviceExtension->DeviceFlags,writeState);
        
        //
        //  Start working on the mrb.
        //
        PMU_REQUEST_BLOCK mrb = &DeviceExtension->Mrb;
        
        //
        //  switch on the stage we need to program
        //
        switch(writeState)
        {
          case MU_WRITE_STATE_INITIAL_READ:
            USB_DBG_TRACE_PRINT(("Starting initial portion."));
            
            //
            //  Read the portion of the first block prior to the start of the
            //  requested write.
            //

            mrb->DataBuffer = MU_DriverExtension.WriteBuffer;
            mrb->TransferLength = DeviceExtension->MediaBlockSize - DeviceExtension->InitialWriteByteCount;
            mrb->Flags = MRB_FLAGS_DATA_IN;
            mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_READ;

            //
            //  The byteOffset is transfer length bytes behind the original byteOffset of the
            //  write request
            //
            byteOffset = irpStack->Parameters.Read.ByteOffset.LowPart-mrb->TransferLength;
            

            break;
          case MU_WRITE_STATE_INITIAL_WRITE:

            //
            //  Write one media block size out.  DataBuffer points to the user's
            //  data we are writing.

            //
            //  Setup MRB_FLAGS_SPLIT_WRITE feature, and its parameters: UserStartOffset,
            //  UserEndOffset so that MRB state machine knows to get the remaining data
            //  from the WriteBuffer.
            //
            mrb->Flags = MRB_FLAGS_DATA_OUT | MRB_FLAGS_SPLIT_WRITE;
            mrb->UserStartOffset = mrb->TransferLength;  //the UserStartOffset is the length we just read.
            mrb->UserEndOffset = DeviceExtension->MediaBlockSize; //The UserEndOffset is the end of the block

            mrb->DataBuffer = (PUCHAR)irpStack->Parameters.Read.BufferOffset;
            mrb->TransferLength = DeviceExtension->MediaBlockSize;
            mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_WRITE;
            
            //
            //  The byteOffset is the UserStartOffset before the original write request byteOffset
            //
            byteOffset = irpStack->Parameters.Read.ByteOffset.LowPart-mrb->UserStartOffset;
            break;

          case MU_WRITE_STATE_FINAL_READ:
            
            USB_DBG_TRACE_PRINT(("Starting final portion."));
            
            //
            //  Read the the end of the block behind the final write byte count
            //
            
            mrb->DataBuffer = MU_DriverExtension.WriteBuffer;
            mrb->TransferLength = DeviceExtension->MediaBlockSize - DeviceExtension->FinalWriteByteCount;
            mrb->Flags = MRB_FLAGS_DATA_IN;
            mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_READ;
            
            //
            //  The logicalBlockAddress should be the sector right after the length that
            //  we really want to write to.
            //
            //
            //  The byteOffset is the end of the original write request.
            //
            byteOffset = irpStack->Parameters.Read.ByteOffset.LowPart +
                         irpStack->Parameters.Read.Length;

            break;
          case MU_WRITE_STATE_FINAL_WRITE:

            //
            //  Write one media block size out.  DataBuffer points to the user's buffer.
            //
            
            //
            //  Setup MRB_FLAGS_SPLIT_WRITE feature, and its parameters: UserStartOffset,
            //  UserEndOffset so that MRB state machine knows to get the remaining data
            //  from the WriteBuffer.
            //
            mrb->Flags = MRB_FLAGS_DATA_OUT | MRB_FLAGS_SPLIT_WRITE;
            mrb->UserStartOffset = 0;  //The UserStartOffset is the beginning of the write.
            mrb->UserEndOffset = DeviceExtension->FinalWriteByteCount; //The UserEndOffset is the size of the user data
            mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_WRITE;

            //
            //  The user data comes from the end of the user buffer.
            //
            mrb->DataBuffer = (PUCHAR) (irpStack->Parameters.Read.BufferOffset +
                                        irpStack->Parameters.Read.Length -
                                        DeviceExtension->FinalWriteByteCount);
            mrb->TransferLength = DeviceExtension->MediaBlockSize;

            //
            //  The byte offset is the length of the user buffer, DeviceExtension->FinalWriteByteCount
            //  before the end of the of the original request
            //
            byteOffset = irpStack->Parameters.Read.ByteOffset.LowPart +
                         irpStack->Parameters.Read.Length -
                         DeviceExtension->FinalWriteByteCount;

            break;
          case MU_WRITE_STATE_DONE:

            //*
            //* This case can be reached due to an error or because
            //* the transfer is over, either way the Status is correct
            //* complete the Irp with that status.
            //

            //
            //  Clear the write state flags.
            //

            CLEAR_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_STATE_BITS);

            //
            // Remove the IRP we are working on from the global queue.
            //

            ASSERT(!IsListEmpty(&MU_DriverExtension.PartialRequestQueue));
            RemoveHeadList(&MU_DriverExtension.PartialRequestQueue);
            
            //
            //  Complete the write Irp
            //

            irp->IoStatus.Information = irpStack->Parameters.Read.Length;
            irp->IoStatus.Status = Status;
            MU_COMPLETE_REQUEST(DeviceExtension, irp, IO_NO_INCREMENT);

            //
            //  Start the next partial request.
            //  NOTE: that this request could not be
            //  for the same device we just finished with.
            //
            MU_DiskWriteStartNextPartialRequest();

            //
            //  Start the next I/O for the current device.
            //
            IoStartNextPacket(DeviceExtension->DeviceObject);

            return;

          default:

            //
            //  Reaching here is a bug in the write state machine.
            //

            ASSERT(FALSE);
            return; //To avoid a compiler warning, without extra code.
        }

        //
        //  Do the endian conversions to plug the LBA and Block Counts
        //  into the Cdb structure.
        //

        mrb->Cbw.Cdb.CDB10.LogicalBlock = ReverseEndian(byteOffset >> DeviceExtension->LogicalBlockShift);
        mrb->Cbw.Cdb.CDB10.TransferBlocks = ReverseEndian((USHORT)(mrb->TransferLength >> DeviceExtension->LogicalBlockShift));
        
        //
        //  Start the MRB
        //

        MU_fStartMrb(DeviceExtension);

        return;
}

VOID
MU_DiskWriteBuildMiddlePartialRequest(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    )
/*++
    This routine is an alternative MU write state machine.  It is its own completion routine
    routine.   It handles the special case of a write to a "middle" partial request, that is a request
    that starts unaligned on the media block boundary and ends unaligned on the same media block.

    This state machine breaks a single media block up as follows:

    1) Read the portion of the media block before user's buffer into the WriteBuffer.
    2) Read the portion of the media block after the user's buffer into the WriteBuffer
       after the data read in 1).
    3) Write, use the MRB_FLAG_SPLIT_WRITE flag, to glue it all together properly.

    Only one instance of this state machine or the MU_DiskWriteBuildPartialRequest state machine
    can be running on the system globally.  Thus these routine work in tandem.

    NOTE: InitialWriteByteCount and FinalWriteByteCount were really named for the other
    state machine.  The interpretation here is as follows:
        InitialWriteByteCount - The length from offset of the beginning of the user buffer to
                                the end of the media block.
        FinalWriteByteCount - The length from offset of the end of the user buffer to
                              the end of the media block. (This is actually set after MU_fDiskReadWrite
                              knows the write will end up here).

--*/
{

        PIRP irp = DeviceExtension->PendingIrp;
        PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
        ULONG  byteOffset;
        ULONG  writeState = DeviceExtension->DeviceFlags&MU_WRITE_STATE_BITS;

        if(NT_SUCCESS(Status))
        {
            //
            // Increment to the next state.  
            //
            writeState += MU_WRITE_STATE_INCREMENT;
        } else
        //
        //  Handle errors
        //
        {

            //
            //  If the error occured during either of the read
            //  states and it was STATUS_DATA_ERROR, mark the
            //  sector as corrupt and continue
            //
            if(
                ( (MU_WRITE_STATE_MIDDLE_READ_BEFORE == writeState)
                  ||(MU_WRITE_STATE_MIDDLE_READ_AFTER == writeState) ) &&
                (STATUS_DATA_ERROR == Status)
            )
            {
                MU_fMarkWriteBufferCorrupt(&DeviceExtension->Mrb);
                // Go on to the next phase
                Status = STATUS_SUCCESS;
                writeState += MU_WRITE_STATE_INCREMENT;
            } else
            //
            //  If the write failed during a write phase, or
            //  for a reason other than a STATUS_DATA_ERROR
            //  (failed CSW), then just keep the error and go to
            //  the done state, which will complete the IRP
            //  with the error.
            //
            {
                writeState = MU_WRITE_STATE_MIDDLE_DONE;
            }
        }

        //
        //  Write the writeState back
        //
        CLEAR_FLAG(DeviceExtension->DeviceFlags,MU_WRITE_STATE_BITS);
        SET_FLAG(DeviceExtension->DeviceFlags,writeState);
        
        //
        //  Start working on the mrb.
        //
        PMU_REQUEST_BLOCK mrb = &DeviceExtension->Mrb;
        
        //
        //  switch on the stage we need to program
        //
        switch(writeState)
        {
      
          case MU_WRITE_STATE_MIDDLE_READ_BEFORE:
                
            //
            //  Read operation goes in
            //
            mrb->Flags = MRB_FLAGS_DATA_IN;
            mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_READ;
            //
            //  Read the length of the media block before the requested read
            //  into the start of the WriteBuffer.
            //
            mrb->DataBuffer = MU_DriverExtension.WriteBuffer;
            mrb->TransferLength = DeviceExtension->MediaBlockSize - DeviceExtension->InitialWriteByteCount;
            //
            //  byteOffset start transferLength before the requested write.
            //
            byteOffset = irpStack->Parameters.Read.ByteOffset.LowPart-mrb->TransferLength;
            break;

          case MU_WRITE_STATE_MIDDLE_READ_AFTER:
            // Still a read since the last step.

            // Pickup read into the buffer where we left off in the last step, and
            // read until the end of the block
            mrb->DataBuffer = MU_DriverExtension.WriteBuffer + mrb->TransferLength;
            mrb->TransferLength = DeviceExtension->FinalWriteByteCount;
            //  
            //  The byteOffset starts right after the original requested write.
            //
            byteOffset = irpStack->Parameters.Read.ByteOffset.LowPart+
                                   irpStack->Parameters.Read.Length;

            break;

          case MU_WRITE_STATE_MIDDLE_WRITE:
            //
            //  Switch to a write operation and use MRB_FLAGS_SPLIT_WRITE
            //  feature.
            //
            mrb->Cbw.Cdb.CDB10.OperationCode = SCSIOP_WRITE;
            mrb->Flags = MRB_FLAGS_DATA_OUT | MRB_FLAGS_SPLIT_WRITE;

            //Set the offsets for the split write
            mrb->UserStartOffset = DeviceExtension->MediaBlockSize - DeviceExtension->InitialWriteByteCount;
            mrb->UserEndOffset = DeviceExtension->MediaBlockSize - DeviceExtension->FinalWriteByteCount;

            //The data buffer is the users data buffer, the transfer length
            //is a full media block
            mrb->DataBuffer = (PUCHAR)irpStack->Parameters.Read.BufferOffset;
            mrb->TransferLength = DeviceExtension->MediaBlockSize;

            //The byteOffste is StartOffset before the requested write.
            byteOffset = irpStack->Parameters.Read.ByteOffset.LowPart-mrb->UserStartOffset;
            break;

          case MU_WRITE_STATE_MIDDLE_DONE:       
            //*
            //* This case can be reached due to an error or because
            //* the transfer is over, either way the Status is correct
            //* complete the Irp with that status.
            //

            //
            //  Clear the write state flags.
            //

            CLEAR_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_STATE_BITS);

            //
            // Remove the IRP we are working on from the global queue.
            //

            ASSERT(!IsListEmpty(&MU_DriverExtension.PartialRequestQueue));
            RemoveHeadList(&MU_DriverExtension.PartialRequestQueue);
            
            //
            //  Complete the write Irp
            //

            irp->IoStatus.Information = irpStack->Parameters.Read.Length;
            irp->IoStatus.Status = Status;
            MU_COMPLETE_REQUEST(DeviceExtension, irp, IO_NO_INCREMENT);

            //
            //  Start the next partial request.
            //  NOTE: that this request could not be
            //  for the same device we just finished with.
            //
            MU_DiskWriteStartNextPartialRequest();

            //
            //  Start the next I/O for the current device.
            //
            IoStartNextPacket(DeviceExtension->DeviceObject);

            return;

          default:

            //
            //  Reaching here is a bug in the write state machine.
            //

            ASSERT(FALSE);
            return; //To avoid a compiler warning, without extra code.
        }
        
        //
        //  Do the endian conversions to plug the LBA and Block Counts
        //  into the Cdb structure.
        //

        mrb->Cbw.Cdb.CDB10.LogicalBlock = ReverseEndian(byteOffset >> DeviceExtension->LogicalBlockShift);
        mrb->Cbw.Cdb.CDB10.TransferBlocks = ReverseEndian((USHORT)(mrb->TransferLength >> DeviceExtension->LogicalBlockShift));

        //
        //  Start the MRB
        //

        MU_fStartMrb(DeviceExtension);

        return;
}

VOID MU_DiskWriteStartNextPartialRequest()
/*++
  Routine Description:
    When either of the partial write state machines completes
    it calls this function to start the next pending partial
    on the correct state machine.
--*/
{
    PIRP irp;
    PIO_STACK_LOCATION irpStack;
    PMU_DEVICE_EXTENSION deviceExtension;

    if(!IsListEmpty(&MU_DriverExtension.PartialRequestQueue))
    {
        //
        //  Peek at head of list to get IRP, but don't
        //  dequeue it.
        //
        irp = CONTAINING_RECORD(
               MU_DriverExtension.PartialRequestQueue.Flink,
               IRP,
               Tail.Overlay.ListEntry
               );

        //
        //  We can find the device object in the stack location
        //  for the irp.  From their we get the device extension
        //  to start the irp on the proper partial write state machine
        //
        irpStack = IoGetCurrentIrpStackLocation(irp);
        deviceExtension = (PMU_DEVICE_EXTENSION) irpStack->DeviceObject->DeviceExtension;
        deviceExtension->Mrb.CompletionRoutine(deviceExtension,STATUS_SUCCESS);
    }
};



VOID
FASTCALL
MU_fMarkWriteBufferCorrupt(
    IN PMU_REQUEST_BLOCK Mrb
    )
/*++
    Routine Description:
        Marks up the WriteBuffer (used for partial write
        operations) with a pattern marking corruption at the
        beginning of each emulated sector.  This routine
        is called when the read phase of a partial write fails
        with a STATUS_DATA_ERROR (i.e. the device returned
        an error in the CSW).
--*/
{
    //
    //  Write the corrupt sector pattern at the beginning of each 4k of the 
    //  the data buffer in the MRB (should point somewhere into global write
    //  buffer).
    //
    for(ULONG Offset = 0; Offset < Mrb->TransferLength; Offset += EMULATED_SECTOR_SIZE)
    {
        RtlCopyMemory(
            Mrb->DataBuffer+Offset,
            MU_CORRUPT_SECTOR_PATTERN,
            MU_CORRUPT_SECTOR_PATTERN_SIZE
            );
    }
}


//*
//* These IOCTL are for running diagnostics on Memory Units.  The production line tests application
//* is the intended client, but there may be others.  A special compile switch is required
//* to build this.
//*
//*
#ifdef MU_DIAGNOSTIC_IOCTLS

NTSTATUS
FASTCALL         
MU_fVscCommand(
 IN PMU_DEVICE_EXTENSION DeviceExtension,
 IN PIRP Irp,
 IN BYTE bRequest,
 IN ULONG ulOutputSize
 )
/*++
    Routine Description:
       Sets up a request to get the Bad Block Table from an MU.
       
    Arguments:
        Irp->UserBuffer should have the output buffer for the
        Bad Block Table
        
        irpStack->DeviceIoControl.OutputBufferLength should be greater
        than or equal to MU_VSD_BAD_BLOCK_TABLE_SIZE
--*/
{
    NTSTATUS status = STATUS_PENDING;
    if(DeviceExtension->MuInstance)
    {
        PURB urb = (PURB)RTL_ALLOCATE_HEAP(sizeof(URB));
        if(urb)
        {
            //
            //  Build VSC request
            //
            
            USB_BUILD_CONTROL_TRANSFER(
                &urb->ControlTransfer,
                NULL,
                Irp->UserBuffer,
                ulOutputSize,
                USB_TRANSFER_DIRECTION_IN,
                MU_VscComplete,
                DeviceExtension,
                FALSE,
                USB_DEVICE_TO_HOST|USB_CLASS_COMMAND|USB_COMMAND_TO_INTERFACE,
                bRequest,
                0,
                DeviceExtension->MuInstance->InterfaceNumber,
                ulOutputSize
                );
            //
            //  Submit the request
            //

            DeviceExtension->MuInstance->Device->SubmitRequest(urb);
        } else
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
        }
    } else
    {
        status = STATUS_DEVICE_NOT_CONNECTED;
    }

    return status;
}

VOID
MU_VscComplete(
    IN PURB   Urb,
    IN PVOID  Context
    )
/*++
    Routine Description:
       Completion routine for all VSC, just fills out
       the Irp.IoStatus fields and completes the IRP.
--*/
{

    PMU_DEVICE_EXTENSION deviceExtension = (PMU_DEVICE_EXTENSION)Context;
    PIRP irp = deviceExtension->PendingIrp;

    irp->IoStatus.Status = IUsbDevice::NtStatusFromUsbdStatus(Urb->Header.Status);
    irp->IoStatus.Information = Urb->ControlTransfer.TransferBufferLength;

    //
    //  Free the URB
    //
    RTL_FREE_HEAP(Urb);

     //
    //  Complete the Irp
    //

    MU_COMPLETE_REQUEST(deviceExtension, irp, IO_NO_INCREMENT);
    
    //
    //  Start processing the next Irp.
    //

    IoStartNextPacket(deviceExtension->DeviceObject);

    return;
}

#endif


//*
//* These routines are called via macros that should only call them in debug
//* builds.
//*

#if DBG

VOID
MUDebugWatchdogDpcRoutine(
    PKDPC,
    PMU_DEVICE_EXTENSION DeviceExtension,
    PVOID,
    PVOID
    )
{
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(DeviceExtension->PendingIrp);
    //
    //  This gets hit when an IRP hangs.
    //  Spew as much useful information
    //  The call KeDebugBreak;
    //
    DbgPrint("MU DRIVER IRP WATCHDOG TIMER HAS EXPIRED. Please report this hang.\n");
    DbgPrint("MU Device Extension: 0x%0.8x, Irp: 0x%0.8x\n", DeviceExtension, DeviceExtension->PendingIrp);

    // Print Irp Information
    switch (irpStack->MajorFunction)
    {
      case IRP_MJ_DEVICE_CONTROL:
        switch(irpStack->Parameters.DeviceIoControl.IoControlCode)
        {
            case IOCTL_DISK_GET_DRIVE_GEOMETRY:
                DbgPrint("IOCTL_DISK_GET_DRIVE_GEOMETRY\n");
                break;
            case IOCTL_DISK_GET_PARTITION_INFO:
                DbgPrint("IOCTL_DISK_GET_PARTITION_INFO\n");
                break;
            case IOCTL_DISK_VERIFY:
                DbgPrint("IOCTL_DISK_VERIFY: Offset = 0x%0.8x, Length = 0x%0.8x\n",
                        irpStack->Parameters.Read.ByteOffset.LowPart,irpStack->Parameters.Read.Length);
                break;
            #ifdef MU_DIAGNOSTIC_IOCTLS
            case MU_IOCTL_GET_BAD_BLOCK_TABLE:
                DbgPrint("MU_IOCTL_GET_BAD_BLOCK_TABLE\n");
                break;
            case MU_IOCTL_MEMORY_TEST:
                DbgPrint("MU_IOCTL_MEMORY_TEST\n");
                break;
            #endif
            default:
                DbgPrint("IOCTL = 0x%0.8x\n", irpStack->Parameters.DeviceIoControl.IoControlCode);
                break;
        }
        break;
      case IRP_MJ_WRITE:
        DbgPrint("Write: Buffer = 0x%0.8x, Offset = 0x%0.8x, Length = 0x%0.8x\n",
            irpStack->Parameters.Read.BufferOffset,
            irpStack->Parameters.Read.ByteOffset.LowPart,
            irpStack->Parameters.Read.Length
            );
        if(TEST_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_MIDDLE_PORTION))
        {
            DbgPrint("Middle Partial Write:");
            switch(DeviceExtension->DeviceFlags&MU_WRITE_STATE_BITS)
            {
                case MU_WRITE_STATE_MIDDLE_START:
                    DbgPrint("MU_WRITE_STATE_MIDDLE_START\n");
                    break;
                case MU_WRITE_STATE_MIDDLE_READ_BEFORE:
                    DbgPrint("MU_WRITE_STATE_MIDDLE_READ_BEFORE\n");
                    break;
                case MU_WRITE_STATE_MIDDLE_READ_AFTER:
                    DbgPrint("MU_WRITE_STATE_MIDDLE_READ_AFTER\n");
                    break;
                case MU_WRITE_STATE_MIDDLE_WRITE:
                    DbgPrint("MU_WRITE_STATE_MIDDLE_WRITE\n");
                    break;
                case MU_WRITE_STATE_MIDDLE_DONE:
                    DbgPrint("MU_WRITE_STATE_MIDDLE_DONE\n");
                    break;
                default:
                    break;
            }
        } else
        {
            if(TEST_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_INITIAL_PORTION))
                DbgPrint("Start on a block aligned.\n");
            if(TEST_FLAG(DeviceExtension->DeviceFlags, MU_WRITE_HAS_FINAL_PORTION))
                DbgPrint("End on a block aligned.\n");
            DbgPrint("Partial Write State:");
            switch(DeviceExtension->DeviceFlags&MU_WRITE_STATE_BITS)
            {
                case MU_WRITE_STATE_START:
                    DbgPrint("MU_WRITE_STATE_START\n");
                    break;
                case MU_WRITE_STATE_INITIAL_READ:
                    DbgPrint("MU_WRITE_STATE_INITIAL_READ\n");
                    break;
                case MU_WRITE_STATE_INITIAL_WRITE:
                    DbgPrint("MU_WRITE_STATE_INITIAL_WRITE\n");
                    break;
                case MU_WRITE_STATE_FINAL_READ:
                    DbgPrint("MU_WRITE_STATE_FINAL_READ\n");
                    break;
                case MU_WRITE_STATE_FINAL_WRITE:
                    DbgPrint("MU_WRITE_STATE_FINAL_WRITE\n");
                case MU_WRITE_STATE_DONE:
                    DbgPrint("MU_WRITE_STATE_DONE\n");
                default:
                    break;
            }
        }
        break;
      case IRP_MJ_READ:
        DbgPrint("Read: Buffer = 0x%0.8x, Offset = 0x%0.8x, Length = 0x%0.8x\n",
            irpStack->Parameters.Read.BufferOffset,
            irpStack->Parameters.Read.ByteOffset.LowPart,
            irpStack->Parameters.Read.Length
            );
        break;
      default:
        DbgPrint("Unknown Request: 0x%0.8x\n", irpStack->MajorFunction);
    }
    //Now print out MRB state machine information
    DbgPrint("MU Request State machine flags:\n");
    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING))
      DbgPrint("   DF_MRB_TIMER_RUNNING\n");
    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_PRIMARY_URB_PENDING))
      DbgPrint("   DF_PRIMARY_URB_PENDING\n");
    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_SECONDARY_URB_PENDING))
      DbgPrint("   DF_SECONDARY_URB_PENDING\n");
    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_ERROR_PENDING))
      DbgPrint("   DF_ERROR_PENDING\n");
    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_RESET_STEP1))
      DbgPrint("   DF_RESET_STEP1\n");
    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_RESET_STEP2))
      DbgPrint("   DF_RESET_STEP2\n");
    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_RESET_STEP3))
      DbgPrint("   DF_RESET_STEP3\n");

    // Print out state machine info
    DbgPrint("Primary Urb @0x%0.8x, Secondary Urb @0x%0.8x\n", &DeviceExtension->Urb, &DeviceExtension->BulkUrbSecondary);
    //Break into debugger
    DbgBreakPoint();
}


VOID
MUDebugInitWatchDogParameters(
    PMU_DEVICE_EXTENSION DeviceExtension
    )
{
    KeInitializeTimer(&DeviceExtension->DbgIrpTimer);
    KeInitializeDpc(
        &DeviceExtension->DbgIrpTimeoutDpc, 
        (PKDEFERRED_ROUTINE)MUDebugWatchdogDpcRoutine,
        (PVOID)DeviceExtension
        );
}

VOID 
MuDebugSetWatchDogTimer(
    PMU_DEVICE_EXTENSION DeviceExtension
    )
{
    //If the timer is running cancel it, it should not have been
    //running so ASSERT that it wasn't.
    LARGE_INTEGER timeoutTime;
    
    BOOL fWasSet = KeCancelTimer(&DeviceExtension->DbgIrpTimer);
    ASSERT(!fWasSet);
    
    
    //
    //  GEt the stack location
    //
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(DeviceExtension->PendingIrp);

    //
    //  Based on the IRP, guess how long this should take, in ms
    //  (we will multiple accordingly after the switch statements)
    //
    switch (irpStack->MajorFunction)
    {
      case IRP_MJ_DEVICE_CONTROL:
        switch(irpStack->Parameters.DeviceIoControl.IoControlCode)
        {
            case IOCTL_DISK_GET_DRIVE_GEOMETRY:
            case IOCTL_DISK_GET_PARTITION_INFO:
                timeoutTime.QuadPart = 2000;  // 2 second
                break;
            case IOCTL_DISK_VERIFY:
                timeoutTime.QuadPart = irpStack->Parameters.Read.Length; //assume 25k verified per second
                timeoutTime.QuadPart /= 25;
                timeoutTime.QuadPart += 4000; //add 4 seconds for overhead
                break;
            #ifdef MU_DIAGNOSTIC_IOCTLS
            case MU_IOCTL_GET_BAD_BLOCK_TABLE:
                timeoutTime.QuadPart = 4000; // 4 seconds
                break;
            case MU_IOCTL_MEMORY_TEST:
                timeoutTime.QuadPart = 60000; //1 minute
                break;
            #endif
            default:
                timeoutTime.QuadPart = 10000; //10 seconds for anything else (there are none at this time)
        }
        break;
      case IRP_MJ_WRITE:
        timeoutTime.QuadPart = irpStack->Parameters.Read.Length; //assume 25k written per second
        timeoutTime.QuadPart /= 25;
        timeoutTime.QuadPart += 4000; //add 4 seconds for overhead
        break;
      case IRP_MJ_READ:
        timeoutTime.QuadPart = irpStack->Parameters.Read.Length; //assume 50k read per second
        timeoutTime.QuadPart /= 50;
        timeoutTime.QuadPart += 4000; //add 4 seconds for overhead
        break;
      default:
        timeoutTime.QuadPart = 10000; // 10 seconds in all other cases, actually there shouldn't be any.
    }
    
    //Convert from milliseconds to relative .1 microsecond units.
    timeoutTime.QuadPart *= -10000;
    DeviceExtension->DbgIrpTimeoutTime.QuadPart = timeoutTime.QuadPart;
    //Set the timer running
    KeSetTimer(&DeviceExtension->DbgIrpTimer, timeoutTime, &DeviceExtension->DbgIrpTimeoutDpc);
}

VOID 
MuDebugPetWatchDogTimer(
    PMU_DEVICE_EXTENSION DeviceExtension
    )
{
    KeSetTimer(&DeviceExtension->DbgIrpTimer, DeviceExtension->DbgIrpTimeoutTime, &DeviceExtension->DbgIrpTimeoutDpc);
}
 

VOID 
MuDebugCompleteRequest(
    PMU_DEVICE_EXTENSION DeviceExtension,
    PIRP Irp,
    CCHAR PriorityBoost
    )
{
    
    BOOL fWasSet = KeCancelTimer(&DeviceExtension->DbgIrpTimer);
    ASSERT(fWasSet);
    IoCompleteRequest(Irp, PriorityBoost);
}

#endif
