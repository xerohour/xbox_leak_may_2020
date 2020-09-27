/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    sctrsz.c

Abstract:
  Filter driver to adapt the SectorSize of USB Mass Storage devices to effectively
  be 512 bytes.  Design to work with up to 8 kbyte sector size.

  All of the new implementation not taken from the boiler plate.
  

Author:

    mitchd

Environment:

    Kernel mode

Revision History:


--*/
#include <WDM.H>
#include <ntddscsi.h>
#include <scsi.h>
#include "filter.h"

//
//	Forward Declaration
//
char *ScsiOpName(UCHAR SciOp);

NTSTATUS SCTRSZ_Ioctl(struct DEVICE_EXTENSION *DevExt, PIRP Irp, BOOLEAN *PassIrpDown)
{
    PIO_STACK_LOCATION irpSp;
    UCHAR majorCode;
	ULONG ioControlCode;
	PSCSI_PASS_THROUGH srbControl;
    NTSTATUS status = STATUS_SUCCESS;

	*PassIrpDown = TRUE;
    irpSp = IoGetCurrentIrpStackLocation(Irp);
	ioControlCode = irpSp->Parameters.DeviceIoControl.IoControlCode;
	
	//DBGOUT(("IOCTL code = 0x%0.8x\n", ioControlCode));

	//
	//	We want to process IOCTL_SCSI_PASS_THROUGH
	//
	if( (IOCTL_SCSI_PASS_THROUGH == ioControlCode) || (IOCTL_SCSI_PASS_THROUGH_DIRECT == ioControlCode))
	{
		DBGOUT(("IOCTL_SCSI_PASS_THROUGH\n"));
		srbControl = Irp->AssociatedIrp.SystemBuffer;
		majorCode = !srbControl->DataIn ? IRP_MJ_WRITE : IRP_MJ_READ;
		if (srbControl->DataTransferLength == 0)
		{
			majorCode = IRP_MJ_FLUSH_BUFFERS;
		}
		switch(majorCode)
		{
			case IRP_MJ_FLUSH_BUFFERS:
				DBGOUT(("IRP_MJ_FLUSH_BUFFERS\n"));
				break;
			case IRP_MJ_READ:
				DBGOUT(("IRP_MJ_READ\n"));
				break;
			case IRP_MJ_WRITE:
				DBGOUT(("IRP_MJ_READ\n"));
				break;
		}
    }

	//
	//	As long as passIrpDown is true, the status doesn't matter
	//
	return status;
}


NTSTATUS SCTRSZ_Scsi(struct DEVICE_EXTENSION *DevExt, PIRP Irp, BOOLEAN *PassIrpDown)
{
	
    PIO_STACK_LOCATION irpSp;
	PSCSI_REQUEST_BLOCK srb;
    UCHAR minorFunc;
	UCHAR *funcName;
	NTSTATUS status = STATUS_SUCCESS;

	irpSp = IoGetCurrentIrpStackLocation(Irp);
	minorFunc = irpSp->MinorFunction;
    srb = irpSp->Parameters.Scsi.Srb;
	switch(srb->Function)
	{
		case SRB_FUNCTION_EXECUTE_SCSI:
			funcName = "SRB_FUNCTION_EXECUTE_SCSI";
			// Defer to the ScsiExecute Filter routine
			*PassIrpDown = FALSE;
			return SCTRSZ_ScsiExecute(DevExt, Irp);
		case SRB_FUNCTION_CLAIM_DEVICE:
			funcName = "SRB_FUNCTION_CLAIM_DEVICE";
			break;
		case SRB_FUNCTION_IO_CONTROL:
			funcName = "SRB_FUNCTION_IO_CONTROL";
			break;
		case SRB_FUNCTION_RECEIVE_EVENT:
			funcName = "SRB_FUNCTION_RECEIVE_EVENT";
			break;
		case SRB_FUNCTION_RELEASE_QUEUE:
			funcName = "SRB_FUNCTION_RELEASE_QUEUE";
			break;
		case SRB_FUNCTION_ATTACH_DEVICE:
			funcName = "SRB_FUNCTION_ATTACH_DEVICE";
			break;
		case SRB_FUNCTION_RELEASE_DEVICE:
			funcName = "SRB_FUNCTION_RELEASE_DEVICE";
			break;
		case SRB_FUNCTION_SHUTDOWN:
			funcName = "SRB_FUNCTION_SHUTDOWN";
			break;
		case SRB_FUNCTION_FLUSH:
			funcName = "SRB_FUNCTION_FLUSH";
			break;
		case SRB_FUNCTION_ABORT_COMMAND:
			funcName = "SRB_FUNCTION_ABORT_COMMAND";
			break;
		case SRB_FUNCTION_RELEASE_RECOVERY:
			funcName = "SRB_FUNCTION_RELEASE_RECOVERY";
			break;
		case SRB_FUNCTION_RESET_BUS:
			funcName = "SRB_FUNCTION_RESET_BUS";
			break;
		case SRB_FUNCTION_RESET_DEVICE:
			funcName = "SRB_FUNCTION_RESET_DEVICE";
			break;
		case SRB_FUNCTION_TERMINATE_IO:
			funcName = "SRB_FUNCTION_TERMINATE_IO";
			break;
		case SRB_FUNCTION_FLUSH_QUEUE:
			funcName = "SRB_FUNCTION_FLUSH_QUEUE";
			break;
		case SRB_FUNCTION_REMOVE_DEVICE:
			funcName = "SRB_FUNCTION_REMOVE_DEVICE";
			break;
		case SRB_FUNCTION_WMI:
			funcName = "SRB_FUNCTION_WMI";
			break;
		case SRB_FUNCTION_LOCK_QUEUE:
			funcName = "SRB_FUNCTION_LOCK_QUEUE";
			break;
		case SRB_FUNCTION_UNLOCK_QUEUE:
			funcName = "SRB_FUNCTION_UNLOCK_QUEUE";
			break;
		default:
			funcName = "SRB_FUNCTION_*** Unknown";
	}
	DBGOUT(("SCSI REQUEST = %s\n", funcName));
	//
	//	As long as passIrpDown is true, the status doesn't matter
	//
	return status;
}



NTSTATUS SCTRSZ_ScsiExecute(struct DEVICE_EXTENSION *DevExt, PIRP Irp)
{
    PIO_STACK_LOCATION irpSp;
	PSCSI_REQUEST_BLOCK srb;
	NTSTATUS status = STATUS_SUCCESS;
	KIRQL	oldIrql;
	LIST_ENTRY prevTail;
	
	irpSp = IoGetCurrentIrpStackLocation(Irp);
    srb = irpSp->Parameters.Scsi.Srb;
	switch(srb->Cdb[0])
	{
		case SCSIOP_READ_CAPACITY:
		case SCSIOP_READ:
		case SCSIOP_WRITE:
		{
			KeAcquireSpinLock(&DevExt->ext.IrpQueueLock, &oldIrql);
			
			//
			//	Set the cancel routine
			//
			IoSetCancelRoutine(Irp, SCTRSZ_ScsiCancel);

			//
			//	Check if the IRP has been canceled
			//
			if(Irp->Cancel)
			{
				DBGOUT(("SCTRSZ_ScsiExecute: Irp was cancelled"));
				//
				//	Clear the cancel routine
				//
				IoSetCancelRoutine(Irp, NULL);
				//
				//	set the status as cancelled.
				//
				status = STATUS_CANCELLED;	
				Irp->IoStatus.Status = status;
			} else
			{
				//
				//	We are going to queue, so increment our pending action count.
				//
				IncrementPendingActionCount(DevExt);
				//
				//	Mark the IRP Pending
				//
				//DBGOUT(("SCTRSZ_ScsiExecute: Queuing IRP"));
				IoMarkIrpPending(Irp);
				status = STATUS_PENDING;	
				//
				//	Insert the IRP in the queue.
				//
				InsertTailList(&DevExt->ext.IrpQueue, &Irp->Tail.Overlay.ListEntry);
				//
				//	Set the synchronization event to wake up the SrbThreadRoutine
				//	if it is blocked.
				//
				KeSetEvent(&DevExt->ext.NewIrpEvent, IO_NO_INCREMENT, FALSE);
			}
			KeReleaseSpinLock(&DevExt->ext.IrpQueueLock, oldIrql);
			//
			//	Complete the IRP here if it was cancelled (We didn't want
			//	to complete the IRP while holding a spin lock.)
			//
			if(STATUS_CANCELLED == status)
			{
				DBGOUT(("SCTRSZ_ScsiExecute: Completing cancelled IRP"));
				IoCompleteRequest(Irp, IO_NO_INCREMENT);
			}
		}
			break;
		default:
			//
			//	Anything we don't filter, just fire and forget.
			//
			IoCopyCurrentIrpStackLocationToNext(Irp);
	        status = IoCallDriver(DevExt->topDevObj, Irp);
	}
	return status;
}

VOID SCTRSZ_ScsiCancel(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	KIRQL	oldIrql;
	struct DEVICE_EXTENSION *devExt = (struct DEVICE_EXTENSION *)DeviceObject->DeviceExtension;
	BOOLEAN		found = FALSE;
	PLIST_ENTRY	firstIrpListEntry;
	PLIST_ENTRY	irpListEntry = NULL;

	DBGOUT(("SCTRSZ_ScsiCancel"));
	//
	//	Release the global cancel spinlock
	//
	IoReleaseCancelSpinLock(Irp->CancelIrql);
	//
	//	Get the spinlock protecting our queue
	//
	KeAcquireSpinLock(&devExt->ext.IrpQueueLock, &oldIrql);
	//
	// Rifle through our queue looking for the irp
	// to cancel.
	//
	firstIrpListEntry = RemoveHeadList(&devExt->ext.IrpQueue);
	irpListEntry = firstIrpListEntry;
	if(firstIrpListEntry)
	{
		do
		{
			//
			// check if this is the irp we are looking for
			//
			if(Irp == CONTAINING_RECORD(irpListEntry, IRP, Tail.Overlay.ListEntry))
			{
				found = TRUE;
				//
				//	If this is the first IRP break.
				//
				if(irpListEntry == firstIrpListEntry) break;
			} else
			//
			//	Otherwise, insert the Irp at the tail.
			//
			{
				InsertTailList(&devExt->ext.IrpQueue, irpListEntry);
			}
			//
			//	Get the next irp
			//
			irpListEntry = RemoveHeadList(&devExt->ext.IrpQueue);
		}while(irpListEntry != firstIrpListEntry);
		//
		//	If the irp is not the first irp, but the
		//	first irp back on the head
		if(Irp != CONTAINING_RECORD(firstIrpListEntry, IRP, Tail.Overlay.ListEntry))
		{
			InsertHeadList(&devExt->ext.IrpQueue, irpListEntry);
		}
	}
	KeReleaseSpinLock(&devExt->ext.IrpQueueLock, oldIrql);
	//
	//	If the IRP was on the queue, complete it
	//	as cancelled
	//
	if(found)
	{
		DBGOUT(("Cancelling Irp"));
		Irp->IoStatus.Status = STATUS_CANCELLED;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		DecrementPendingActionCount(devExt);
	}
}

VOID SCTRSZ_SrbThreadRoutine(PVOID StartContext)
{
	struct DEVICE_EXTENSION *devExt = (struct DEVICE_EXTENSION *)StartContext;
	PIO_STACK_LOCATION irpSp;
	PSCSI_REQUEST_BLOCK srb;

	PVOID WaitEvents[2];
	NTSTATUS status;
	WaitEvents[0] = &devExt->ext.NewIrpEvent;
	WaitEvents[1] = &devExt->ext.TerminateThreadEvent;
	//
	//	Loop until WaitEvents[1] is set.
	//
	do
	{
		status = KeWaitForMultipleObjects(2, WaitEvents, WaitAny, Executive, KernelMode, FALSE, NULL, NULL);
		ASSERT(NT_SUCCESS(status));
		if(0 == status)
		//
		//	There should be a new IRP, work on it.
		//
		{
			PIRP irp;
			//
			//	Keep pulling IRPS off the queue and processing them,
			//	until there are none left.
			//
			while(NULL != (irp = SCTRSZ_DequeueIrp(devExt)))
			{
				irpSp = IoGetCurrentIrpStackLocation(irp);
				srb = irpSp->Parameters.Scsi.Srb;
				DBGOUT(("Executing %s\n", ScsiOpName(srb->Cdb[0])));
				switch(srb->Cdb[0])
				{
					case SCSIOP_READ_CAPACITY:
						SCTRSZ_ScsiReadCapacity(devExt, irp);
						break;
					case SCSIOP_READ:
						SCTRSZ_ScsiRead(devExt, irp);
						break;
					case SCSIOP_WRITE:
						SCTRSZ_ScsiWrite(devExt, irp);
						break;
					default:
						ASSERT(FALSE);
				}
				DecrementPendingActionCount(devExt);
			} 
		} 
	} while(1 != status); // Thread should terminate
	//
	//	if the READ_CAPACITY routine allocated a block buffer
	//	delete it.
	//
	if(devExt->ext.BlockBuffer)
	{
		IoFreeMdl(devExt->ext.BlockBufferMdl);
		ExFreePool(devExt->ext.BlockBuffer);
		devExt->ext.BlockBufferMdl = NULL;
		devExt->ext.BlockBuffer = NULL;
	}
	DBGOUT(("About to terminate thread"));
	PsTerminateSystemThread(STATUS_SUCCESS);
	return;
}

PIRP SCTRSZ_DequeueIrp(struct DEVICE_EXTENSION *DevExt)
{
	KIRQL		oldIrql;
	PLIST_ENTRY irpListEntry;
	PIRP		irp = NULL;
	BOOLEAN		repeat;
	do
	{
		repeat = FALSE;
		KeAcquireSpinLock(&DevExt->ext.IrpQueueLock, &oldIrql);
		if(IsListEmpty(&DevExt->ext.IrpQueue))
		{
			irpListEntry = NULL;
		} else
		{
			irpListEntry = RemoveHeadList(&DevExt->ext.IrpQueue);
		}
		KeReleaseSpinLock(&DevExt->ext.IrpQueueLock, oldIrql);
		if(irpListEntry)
		{
			irp = CONTAINING_RECORD(irpListEntry, IRP, Tail.Overlay.ListEntry);
			//
			//	Clear the cancel routine
			//
			IoSetCancelRoutine(irp, NULL);
			//
			//	Check if it needs cancelled
			//
			if(irp->Cancel)
			{
				DBGOUT(("SCTRSZ_DequeueIrp: Irp was cancelled"));
				irp->IoStatus.Status = STATUS_CANCELLED;
				IoCompleteRequest(irp, IO_NO_INCREMENT);
				DecrementPendingActionCount(DevExt);
				irp = NULL;
				repeat = TRUE;
			}
		}
	} while(repeat);
	return irp;
}

VOID SCTRSZ_ScsiReadCapacity(struct DEVICE_EXTENSION *DevExt, PIRP Irp)
{
    PIO_STACK_LOCATION irpSp;
	PSCSI_REQUEST_BLOCK srb;
	NTSTATUS status = STATUS_SUCCESS;
	PREAD_CAPACITY_DATA readCapacityData;
	
	//
	//	If we do not know the capacity get it.
	//
	if(!DevExt->ext.CapacityKnown)
	{
		//
		// Forward the irp synchronously
		//
		IoCopyCurrentIrpStackLocationToNext( Irp );
		status = CallNextDriverSync( DevExt, Irp );
	
		if(NT_SUCCESS(status))
		{
			irpSp = IoGetCurrentIrpStackLocation(Irp);
			srb = irpSp->Parameters.Scsi.Srb;
			readCapacityData = (PREAD_CAPACITY_DATA)srb->DataBuffer;
			DevExt->ext.BlockCount = ChangeEndian(readCapacityData->LogicalBlockAddress);
			DevExt->ext.BlockSize = ChangeEndian(readCapacityData->BytesPerBlock);
			DevExt->ext.BlocksPerSector = (DevExt->ext.BlockSize/512);
			DevExt->ext.SectorCount = DevExt->ext.BlocksPerSector * DevExt->ext.BlockCount;
			DevExt->ext.BlockBuffer = ExAllocatePool(NonPagedPool, DevExt->ext.BlockSize);
			if(DevExt->ext.BlockBuffer)
			{
				DevExt->ext.CapacityKnown = TRUE;
				DBGOUT(("\n  BlockSize = %d\n  BlockCount = %d\n  BlocksPerSector = %d\n SectorCount = %d\n",
					DevExt->ext.BlockSize,
					DevExt->ext.BlockCount,
					DevExt->ext.BlocksPerSector,
					DevExt->ext.SectorCount));
				DevExt->ext.BlockBufferMdl = 
				IoAllocateMdl(DevExt->ext.BlockBuffer, DevExt->ext.BlockSize, FALSE, FALSE, NULL);
				if(DevExt->ext.BlockBufferMdl)
				{
					MmBuildMdlForNonPagedPool(DevExt->ext.BlockBufferMdl);
				} else
				{
					ExFreePool(DevExt->ext.BlockBuffer);
					DevExt->ext.BlockBuffer = NULL;
				}
			}
			//	
			//	Even if the BlockBuffer was allocated, it could have been
			//	free, if the attempt allocate a MDL failed.
			//
			if(NULL == DevExt->ext.BlockBuffer)
			{
				srb->ScsiStatus = SRB_STATUS_ERROR;
				Irp->IoStatus.Status = STATUS_NO_MEMORY;
			}

		}
	} else
	//
	//	We knew the capacity, so don't send it down.
	//
	{
		irpSp = IoGetCurrentIrpStackLocation(Irp);
		srb = irpSp->Parameters.Scsi.Srb;
	}
	//
	//	We ought to know the capacity now, so fill everything out
	//
	if(DevExt->ext.CapacityKnown)
	{
		readCapacityData = (PREAD_CAPACITY_DATA)srb->DataBuffer;
		readCapacityData->BytesPerBlock = ChangeEndian(512);
		readCapacityData->LogicalBlockAddress = ChangeEndian(DevExt->ext.SectorCount);
		/*
		DBGOUT(("LogicalAddress = %d, BytesPerBlock = %d\n",
					ChangeEndian(readCapacityData->LogicalBlockAddress),
					ChangeEndian(readCapacityData->BytesPerBlock)));
		*/
		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = sizeof(READ_CAPACITY_DATA);
		srb->SrbStatus = SRB_STATUS_SUCCESS;
	}
	//DBGOUT(("Completing read capacity request"));
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return;
}

VOID SCTRSZ_ScsiRead(struct DEVICE_EXTENSION *DevExt, PIRP Irp)
{
	PIO_STACK_LOCATION irpSp;
	PSCSI_REQUEST_BLOCK srb;
	PCDB cdb;
	SCRTSZ_BLOCK_DESC blockDesc;
	ULONG blockCopySize = 0;
	PVOID blockCopyStart = 0;
	ULONG bytesTransfered = 0;
	UCHAR savedCdb[10];
	PMDL originalMdl;
	PMDL partialMdl;
	NTSTATUS status = STATUS_SUCCESS;
	

	//
	//	Original data buffer
	//
	PCHAR dataBuffer;
	ULONG dataTransferLength;
	PCHAR dataVa;
	ULONG dataLength;
	
	//
	//	Get the CDB and start working on next stack location
	//
	IoCopyCurrentIrpStackLocationToNext(Irp);
	irpSp = IoGetNextIrpStackLocation(Irp);
	srb = irpSp->Parameters.Scsi.Srb;
	cdb = (PCDB)srb->Cdb;
	RtlCopyMemory( (PVOID)savedCdb, (PVOID)srb->Cdb, 10);
	originalMdl = Irp->MdlAddress;

	//
	//	If the capacity of the device is not
	//	known yet, we cannot complete this request.
	//
	if(!DevExt->ext.CapacityKnown)
	{
		srb->SrbStatus = SRB_STATUS_BUSY;
		Irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
		DBGOUT(("SCTRSZ_ScsiRead: Failing because we don't capacity yet."));
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return;
	}
	//
	//	Break up the request
	//
	SCTRSZ_BreakUpRequest(cdb, &blockDesc, (UCHAR)DevExt->ext.BlocksPerSector);

	dataBuffer = (PCHAR)srb->DataBuffer;
	dataTransferLength = srb->DataTransferLength;
	dataVa = MmGetSystemAddressForMdlSafe(originalMdl, NormalPagePriority);
	if(!dataVa)
	{
		status = STATUS_NO_MEMORY;
		goto read_failure;
	}
	dataLength = MmGetMdlByteCount(originalMdl);
	
	
	//
	//	Perform InitialBlock transfer
	//
	if(blockDesc.InitialBlockSectorCount)
	{
		//DBGOUT(("InitialBlockSectorCount = %d", blockDesc.InitialBlockSectorCount));
		//
		//	Alter the IRP to read into our buffer
		//
		Irp->MdlAddress = DevExt->ext.BlockBufferMdl;
		//
		//	Alter the SRB to read into our buffer
		//
		srb->DataBuffer = DevExt->ext.BlockBuffer;
		srb->DataTransferLength = DevExt->ext.BlockSize;
		//
		//	Alter the cdb to read the block that we need
		//
		*((PULONG)&cdb->CDB10.LogicalBlockByte0) = ChangeEndian(blockDesc.InitialBlock);
		//
		//	Get one block.
		//
		cdb->CDB10.TransferBlocksLsb = 1;
		cdb->CDB10.TransferBlocksMsb = 0;
		//
		//	Send the IRP down synchronously
		//
		status = CallNextDriverSync(DevExt, Irp);
		IoCopyCurrentIrpStackLocationToNext(Irp);
		if(!NT_SUCCESS(status)) goto read_failure;
		//
		//	Copy the initial sectors into the uses buffer.
		//  (512 is the size of a sector, always!)
		blockCopySize = 512*(ULONG)blockDesc.InitialBlockSectorCount;
		blockCopyStart = (PVOID)(((PCHAR)DevExt->ext.BlockBuffer) + 512*blockDesc.InitialSectorOffset);
		RtlCopyMemory((PVOID)dataVa,  blockCopyStart, blockCopySize);
		bytesTransfered += blockCopySize;
	}

	//
	//	Perform whole block transfer
	//
	if(blockDesc.WholeBlockCount)
	{
		//DBGOUT(("WholeBlockCount = %d", blockDesc.WholeBlockCount));
	
		srb->DataBuffer = dataBuffer + blockCopySize;
		srb->DataTransferLength = blockDesc.WholeBlockCount*DevExt->ext.BlockSize;
		
		//
		//	Build a partialMdl for transfer
		//
		partialMdl = IoAllocateMdl(srb->DataBuffer, srb->DataTransferLength, FALSE, FALSE, NULL);
		if(!partialMdl) goto read_failure;
		IoBuildPartialMdl(originalMdl, partialMdl, srb->DataBuffer, srb->DataTransferLength);
		Irp->MdlAddress = partialMdl;
		
		//
		//	Alter the cdb to read the block that we need
		//
		*((PULONG)&cdb->CDB10.LogicalBlockByte0) = ChangeEndian(blockDesc.FirstWholeBlock);
		cdb->CDB10.TransferBlocksLsb = (UCHAR)(blockDesc.WholeBlockCount & 0x00FF);
		cdb->CDB10.TransferBlocksMsb = (UCHAR)((blockDesc.WholeBlockCount & 0xFF00) >> 8);
		//
		//	Send the IRP down synchronously
		//
		status = CallNextDriverSync(DevExt, Irp);
		IoCopyCurrentIrpStackLocationToNext(Irp);
		IoFreeMdl(partialMdl);
		if(!NT_SUCCESS(status)) goto read_failure;
		//
		//	No copy necessary we wrote into the buffer passed from above
		//
		bytesTransfered += srb->DataTransferLength;
	}

	//
	//	Perform the final block transfersss
	//
	if(blockDesc.FinalBlockSectorCount)
	{
		//DBGOUT(("FinalBlockSectorCount = %d", blockDesc.FinalBlockSectorCount));
		//
		//	Alter the IRP to read into our buffer
		//
		Irp->MdlAddress = DevExt->ext.BlockBufferMdl;
		//
		//	Alter the SRB to read into our buffer
		//
		srb->DataBuffer = DevExt->ext.BlockBuffer;
		srb->DataTransferLength = DevExt->ext.BlockSize;
		//
		//	Alter the cdb to read the block that we need
		//
		*((PULONG)&cdb->CDB10.LogicalBlockByte0) = ChangeEndian(blockDesc.FinalBlock);
		//
		//	Get one block.
		//
		cdb->CDB10.TransferBlocksLsb = 1;
		cdb->CDB10.TransferBlocksMsb = 0;
		//
		//	Send the IRP down synchronously
		//
		status = CallNextDriverSync(DevExt, Irp);
		if(!NT_SUCCESS(status)) goto read_failure;
		//
		//	Copy the initial sectors into the user buffer.
		//  (512 is the size of a sector, always!)
		blockCopySize = 512*(ULONG)blockDesc.FinalBlockSectorCount;
		blockCopyStart = (PVOID)(dataVa + (dataLength - blockCopySize));
		/*
		DBGOUT(("blockCopyStart(0x%0.8x), blockBuffer(0x%0.8x), blockCopySize(%d)", 
			blockCopyStart, DevExt->ext.BlockBuffer, blockCopySize));
		DBGOUT(("dataVa = 0x%0.8x", dataVa));
		DBGOUT(("dataLength = %d", dataLength));
		*/
		RtlCopyMemory(blockCopyStart, DevExt->ext.BlockBuffer, blockCopySize);
		bytesTransfered += blockCopySize;
	}

read_failure:
	//
	//	Restore IRP
	//
	Irp->MdlAddress = originalMdl;
	srb->DataBuffer = (PVOID)dataBuffer;
	ASSERT(bytesTransfered == dataTransferLength);
	srb->DataTransferLength = bytesTransfered;
	RtlCopyMemory((PVOID)srb->Cdb, (PVOID)savedCdb, 10);
	Irp->IoStatus.Information = bytesTransfered;
	//DBGOUT(("Completing Read IRP"));
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
}

VOID SCTRSZ_ScsiWrite(struct DEVICE_EXTENSION *DevExt, PIRP Irp)
{
	PIO_STACK_LOCATION irpSp;
	PSCSI_REQUEST_BLOCK srb;
	PCDB cdb;
	UCHAR savedCdb[10];
	SCRTSZ_BLOCK_DESC blockDesc;
	ULONG blockCopySize = 0;
	PVOID blockCopyStart = 0;
	ULONG bytesTransfered = 0;
	PMDL originalMdl;
	PMDL partialMdl;
	NTSTATUS status = STATUS_SUCCESS;

	//
	//	Original data buffer
	//
	PCHAR dataBuffer;
	ULONG dataTransferLength;
	PCHAR dataVa;
	ULONG dataLength;
	
	
	//
	//	Get the CDB and start working on next stack location
	//
	IoCopyCurrentIrpStackLocationToNext(Irp);
	irpSp = IoGetNextIrpStackLocation(Irp);
	srb = irpSp->Parameters.Scsi.Srb;
	cdb = (PCDB)srb->Cdb;
	RtlCopyMemory( (PVOID)savedCdb, (PVOID)srb->Cdb, 10);
	originalMdl = Irp->MdlAddress;
	//
	//	If the capacity of the device is not
	//	known yet, we cannot complete this request.
	//
	if(!DevExt->ext.CapacityKnown)
	{
		srb->SrbStatus = SRB_STATUS_BUSY;
		Irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return;
	}
	//
	//	Break up the request
	//
	SCTRSZ_BreakUpRequest(cdb, &blockDesc, (UCHAR)DevExt->ext.BlocksPerSector);

	dataBuffer = (PCHAR)srb->DataBuffer;
	dataTransferLength = srb->DataTransferLength;
	dataVa = MmGetSystemAddressForMdlSafe(originalMdl, NormalPagePriority);
	if(!dataVa)
	{
		status = STATUS_NO_MEMORY;
		goto write_failure;
	}
	dataLength = MmGetMdlByteCount(originalMdl);
	
	//
	//	Perform InitialBlock transfer
	//
	if(blockDesc.InitialBlockSectorCount)
	{
		//DBGOUT(("InitialBlockSectorCount = %d", blockDesc.InitialBlockSectorCount));
		//
		//	Alter the IRP to read into our buffer
		//
		Irp->MdlAddress = DevExt->ext.BlockBufferMdl;
		//
		//	Alter the SRB to read into our buffer
		//
		srb->DataBuffer = DevExt->ext.BlockBuffer;
		srb->DataTransferLength = DevExt->ext.BlockSize;
		srb->SrbFlags &= ~SRB_FLAGS_DATA_OUT;
		srb->SrbFlags |= SRB_FLAGS_DATA_IN;
		//
		//	Alter the cdb to read the block that we need
		//
		*((PULONG)&cdb->CDB10.LogicalBlockByte0) = ChangeEndian(blockDesc.InitialBlock);
		//
		//	Get one block.
		//
		cdb->CDB10.OperationCode = SCSIOP_READ;
		cdb->CDB10.TransferBlocksLsb = 1;
		cdb->CDB10.TransferBlocksMsb = 0;
		//
		//	Send the IRP down synchronously
		//
		status = CallNextDriverSync(DevExt, Irp);
		IoCopyCurrentIrpStackLocationToNext(Irp);
		cdb->CDB10.OperationCode = SCSIOP_WRITE;  //win or lose, better switch it back to write
		srb->SrbFlags &= ~SRB_FLAGS_DATA_IN;
		srb->SrbFlags |= SRB_FLAGS_DATA_OUT;
		if(!NT_SUCCESS(status)) goto write_failure;
		//
		//	Copy the initial sectors over those in the output buffer
		//  (512 is the size of a sector, always!)
		blockCopySize = 512*(ULONG)blockDesc.InitialBlockSectorCount;
		blockCopyStart = (PVOID)(((PCHAR)DevExt->ext.BlockBuffer) + 512*blockDesc.InitialSectorOffset);
		RtlCopyMemory(blockCopyStart, (PVOID)dataVa, blockCopySize);
		//
		//	Write the databack out
		//
		status = CallNextDriverSync(DevExt, Irp);
		IoCopyCurrentIrpStackLocationToNext(Irp);
		if(!NT_SUCCESS(status)) goto write_failure;

		bytesTransfered += blockCopySize;
	}

	//
	//	Perform whole block transfer
	//
	if(blockDesc.WholeBlockCount)
	{
		//DBGOUT(("WholeBlockCount = %d", blockDesc.WholeBlockCount));
		//
		//	Alter the SRB to write from user buffer, but only the section spanning whole blocks.
		//
		srb->DataBuffer = dataBuffer + blockCopySize;
		srb->DataTransferLength = blockDesc.WholeBlockCount*DevExt->ext.BlockSize;
		//
		//	Build a partialMdl for transfer
		//
		partialMdl = IoAllocateMdl(srb->DataBuffer, srb->DataTransferLength, FALSE, FALSE, NULL);
		if(!partialMdl) goto write_failure;
		IoBuildPartialMdl(originalMdl, partialMdl, srb->DataBuffer, srb->DataTransferLength);
		Irp->MdlAddress = partialMdl;
		//
		//	Alter the cdb to read the block that we need
		//
		*((PULONG)&cdb->CDB10.LogicalBlockByte0) = ChangeEndian(blockDesc.FirstWholeBlock);
		cdb->CDB10.TransferBlocksLsb = (UCHAR)(blockDesc.WholeBlockCount & 0x00FF);
		cdb->CDB10.TransferBlocksMsb = (UCHAR)((blockDesc.WholeBlockCount & 0xFF00) >> 8);
		//
		//	Send the IRP down synchronously
		//
		status = CallNextDriverSync(DevExt, Irp);
		IoCopyCurrentIrpStackLocationToNext(Irp);
		if(!NT_SUCCESS(status)) goto write_failure;
		//
		//	No copy necessary we wrote into the buffer passed from above
		//
		bytesTransfered += srb->DataTransferLength;
	}

	//
	//	Perform the final block transfersss
	//
	if(blockDesc.FinalBlockSectorCount)
	{
		//DBGOUT(("FinalBlockSectorCount = %d", blockDesc.FinalBlockSectorCount));
		//
		//	Alter the IRP to read into our buffer
		//
		Irp->MdlAddress = DevExt->ext.BlockBufferMdl;
		//
		//	Alter the SRB to read into our buffer
		//
		srb->DataBuffer = DevExt->ext.BlockBuffer;
		srb->DataTransferLength = DevExt->ext.BlockSize;
		//
		//	Alter the cdb to read the block that we need
		//
		cdb->CDB10.OperationCode = SCSIOP_READ;
		srb->SrbFlags &= ~SRB_FLAGS_DATA_OUT;
		srb->SrbFlags |= SRB_FLAGS_DATA_IN;
		*((PULONG)&cdb->CDB10.LogicalBlockByte0) = ChangeEndian(blockDesc.FinalBlock);
		//
		//	Get one block.
		//
		cdb->CDB10.TransferBlocksLsb = 1;
		cdb->CDB10.TransferBlocksMsb = 0;
		//
		//	Send the IRP down synchronously
		//
		status = CallNextDriverSync(DevExt, Irp);
		IoCopyCurrentIrpStackLocationToNext(Irp);
		cdb->CDB10.OperationCode = SCSIOP_WRITE;  //win or lose, better switch it back to write
		srb->SrbFlags &= ~SRB_FLAGS_DATA_IN;
		srb->SrbFlags |= SRB_FLAGS_DATA_OUT;
		if(!NT_SUCCESS(status)) goto write_failure;
		//
		//	Copy the final sectors from the user buffer.
		//  (512 is the size of a sector, always!)
		blockCopySize = 512*(ULONG)blockDesc.FinalBlockSectorCount;
		blockCopyStart = (PVOID)(dataVa + (dataLength - blockCopySize));
		RtlCopyMemory(DevExt->ext.BlockBuffer, blockCopyStart, blockCopySize);
		//
		//	Write the data back out
		//
		status = CallNextDriverSync(DevExt, Irp);
		if(!NT_SUCCESS(status)) goto write_failure;
		bytesTransfered += blockCopySize;
	}

write_failure:
	//
	//	Restore IRP
	//
	Irp->MdlAddress = originalMdl;
	srb->DataBuffer = (PVOID)dataBuffer;
	ASSERT(bytesTransfered == dataTransferLength);
	srb->DataTransferLength = bytesTransfered;
	RtlCopyMemory((PVOID)srb->Cdb, (PVOID)savedCdb, 10);
	Irp->IoStatus.Information = bytesTransfered;
	//DBGOUT(("Completing Write IRP"));
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
}


void SCTRSZ_BreakUpRequest(PCDB Cdb, PSCRTSZ_BLOCK_DESC BlockDesc, UCHAR BlocksPerSector)
/*++
	Look at a read or write, break it into three
	sections:
		1) Initial partial block, if there is one.
		2) Zero or more whole blocks.
		3) Final partial block, if there is one.
--*/
{
	USHORT SectorCount;
	//
	//	Get the first sector. It is stored as big endian.
	//	We might as well use our ChangeEndian function.
	//
	BlockDesc->FirstSector = 0;
	BlockDesc->FirstSector = ChangeEndian(*((long *)&Cdb->CDB10.LogicalBlockByte0));
	SectorCount = (((USHORT)Cdb->CDB10.TransferBlocksMsb) << 8) + Cdb->CDB10.TransferBlocksLsb;
	
	//
	//	Calculate begining of initial block, and the number of sectors
	//	needed from it.  (note that DevExt->ext.BlocksPerSector is results
	//	instead of 0.)
	//
	BlockDesc->InitialBlock = BlockDesc->FirstSector/BlocksPerSector;
	BlockDesc->InitialSectorOffset = (USHORT)(BlockDesc->FirstSector%BlocksPerSector);
	if(BlockDesc->InitialSectorOffset)
	{
		BlockDesc->InitialBlockSectorCount = (USHORT)(BlocksPerSector - BlockDesc->InitialSectorOffset);
		if(BlockDesc->InitialBlockSectorCount > SectorCount)
					BlockDesc->InitialBlockSectorCount = SectorCount;
	} else
	{
		BlockDesc->InitialBlockSectorCount = 0;
	}
	
	
	//
	//	Calculate First whole block and fix InitialBlockSectorCount
	//	in the case that an initial block is not needed.
	//
	if(0 == BlockDesc->InitialBlockSectorCount)
	{
		BlockDesc->FirstWholeBlock = BlockDesc->InitialBlock;
	} else
	{
		BlockDesc->FirstWholeBlock = BlockDesc->InitialBlock+1;
	}
	BlockDesc->WholeBlockCount = (SectorCount - BlockDesc->InitialBlockSectorCount)/BlocksPerSector;
	
	//
	//	Calculate the final block.
	//
	BlockDesc->FinalBlockSectorCount = (SectorCount - BlockDesc->InitialBlockSectorCount)%BlocksPerSector;
	BlockDesc->FinalBlock = BlockDesc->FirstWholeBlock + BlockDesc->WholeBlockCount;
	//
	//	Dump the blocks
	//
	
	DBGOUT(("FirstSector             %d", BlockDesc->FirstSector));
	DBGOUT(("InitialSectorOffset     %d", BlockDesc->InitialSectorOffset));
	DBGOUT(("InitialBlock            %d", BlockDesc->InitialBlock));
	DBGOUT(("InitialBlockSectorCount %d", BlockDesc->InitialBlockSectorCount));
	DBGOUT(("FirstWholeBlock         %d", BlockDesc->FirstWholeBlock));
	DBGOUT(("WholeBlockCount         %d", BlockDesc->WholeBlockCount));
	DBGOUT(("FinalBlock              %d", BlockDesc->FinalBlock));
	DBGOUT(("FinalBlockSectorCount   %d", BlockDesc->FinalBlockSectorCount));
	
}

char *ScsiOpName(UCHAR SciOp)
{
	UCHAR *scsiopName = NULL;
	switch(SciOp)
	{
		case SCSIOP_TEST_UNIT_READY:
			scsiopName = "SCSIOP_TEST_UNIT_READY";
			break;
		case SCSIOP_REZERO_UNIT:
			scsiopName = "SCSIOP_REZERO_UNIT";
			break;
		//case SCSIOP_REWIND: same as SCSIOP_REZERO_UNIT
		//	scsiopName = "SCSIOP_REWIND";
		//	break;
		case SCSIOP_REQUEST_BLOCK_ADDR:
			scsiopName = "SCSIOP_REQUEST_BLOCK_ADDR";
			break;
		case SCSIOP_REQUEST_SENSE:
			scsiopName = "SCSIOP_REQUEST_SENSE";
			break;
		case SCSIOP_FORMAT_UNIT:
			scsiopName = "SCSIOP_FORMAT_UNIT";
			break;
		case SCSIOP_READ_BLOCK_LIMITS:
			scsiopName = "SCSIOP_READ_BLOCK_LIMITS";
			break;
		case SCSIOP_REASSIGN_BLOCKS:
			scsiopName = "SCSIOP_REASSIGN_BLOCKS";
			break;
		//case SCSIOP_INIT_ELEMENT_STATUS: same as SCSIOP_REASSIGN_BLOCKS
		//	scsiopName = "SCSIOP_INIT_ELEMENT_STATUS";
		//	break;
		case SCSIOP_READ6:
			scsiopName = "SCSIOP_READ6";
			break;
		//case SCSIOP_RECEIVE: same as SCSIOP_READ6
		//	scsiopName = "SCSIOP_RECEIVE";
		//	break;
		case SCSIOP_WRITE6:
			scsiopName = "SCSIOP_WRITE6";
			break;
		//case SCSIOP_PRINT: same as SCSIOP_WRITE6
		//	scsiopName = "SCSIOP_PRINT";
		//	break;
		//case SCSIOP_SEND: same as SCSIOP_WRITE6
		//	scsiopName = "SCSIOP_SEND";
		//	break;
		case SCSIOP_SEEK6:
			scsiopName = "SCSIOP_SEEK6";
			break;
		//case SCSIOP_TRACK_SELECT: same as SCSIOP_SEEK6
		//	scsiopName = "SCSIOP_TRACK_SELECT";
		//	break;
		//case SCSIOP_SLEW_PRINT: same as SCSIOP_SEEK6
		//	scsiopName = "SCSIOP_SLEW_PRINT";
		//	break;
		case SCSIOP_SEEK_BLOCK:
			scsiopName = "SCSIOP_SEEK_BLOCK";
			break;
		case SCSIOP_PARTITION:
			scsiopName = "SCSIOP_PARTITION";
			break;
		case SCSIOP_READ_REVERSE:
			scsiopName = "SCSIOP_READ_REVERSE";
			break;
		case SCSIOP_WRITE_FILEMARKS:
			scsiopName = "SCSIOP_WRITE_FILEMARKS";
			break;
		//case SCSIOP_FLUSH_BUFFER: same as SCSIOP_WRITE_FILEMARKS
		//	scsiopName = "SCSIOP_FLUSH_BUFFER";
		//	break;
		case SCSIOP_SPACE:
			scsiopName = "SCSIOP_SPACE";
			break;
		case SCSIOP_INQUIRY:
			scsiopName = "SCSIOP_INQUIRY";
			break;
		case SCSIOP_VERIFY6:
			scsiopName = "SCSIOP_VERIFY6";
			break;
		case SCSIOP_RECOVER_BUF_DATA:
			scsiopName = "SCSIOP_RECOVER_BUF_DATA";
			break;
		case SCSIOP_MODE_SELECT:
			scsiopName = "SCSIOP_MODE_SELECT";
			break;
		case SCSIOP_RESERVE_UNIT:
			scsiopName = "SCSIOP_RESERVE_UNIT";
			break;
		case SCSIOP_RELEASE_UNIT:
			scsiopName = "SCSIOP_RELEASE_UNIT";
			break;
		case SCSIOP_COPY:
			scsiopName = "SCSIOP_COPY";
			break;
		case SCSIOP_ERASE:
			scsiopName = "SCSIOP_ERASE";
			break;
		case SCSIOP_MODE_SENSE:
			scsiopName = "SCSIOP_MODE_SENSE";
			break;
		case SCSIOP_START_STOP_UNIT:
			scsiopName = "SCSIOP_START_STOP_UNIT";
			break;
		//case SCSIOP_STOP_PRINT: same as SCSIOP_START_STOP_UNIT
		//	scsiopName = "SCSIOP_STOP_PRINT";
		//	break;
		//case SCSIOP_LOAD_UNLOAD: same as SCSIOP_START_STOP_UNIT
		//	scsiopName = "SCSIOP_LOAD_UNLOAD";
		//	break;
		case SCSIOP_RECEIVE_DIAGNOSTIC:
			scsiopName = "SCSIOP_RECEIVE_DIAGNOSTIC";
			break;
		case SCSIOP_SEND_DIAGNOSTIC:
			scsiopName = "SCSIOP_SEND_DIAGNOSTIC";
			break;
		case SCSIOP_MEDIUM_REMOVAL:
			scsiopName = "SCSIOP_MEDIUM_REMOVAL";
			break;
		case SCSIOP_READ_FORMATTED_CAPACITY:
			scsiopName = "SCSIOP_READ_FORMATTED_CAPACITY";
			break;
		case SCSIOP_READ_CAPACITY:
			scsiopName = "SCSIOP_READ_CAPACITY";
			break;
		case SCSIOP_READ:
			scsiopName = "SCSIOP_READ";
			break;
		case SCSIOP_WRITE:
			scsiopName = "SCSIOP_WRITE";
			break;
		case SCSIOP_SEEK:
			scsiopName = "SCSIOP_SEEK";
			break;
		//case SCSIOP_LOCATE: same as SCSIOP_SEEK
		//	scsiopName = "SCSIOP_LOCATE";
		//	break;
		//case SCSIOP_POSITION_TO_ELEMENT: same as SCSIOP_SEEK
		//	scsiopName = "SCSIOP_POSITION_TO_ELEMENT";
		//	break;
		case SCSIOP_WRITE_VERIFY:
			scsiopName = "SCSIOP_WRITE_VERIFY";
			break;
		case SCSIOP_VERIFY:
			scsiopName = "SCSIOP_VERIFY";
			break;
		case SCSIOP_SEARCH_DATA_HIGH:
			scsiopName = "SCSIOP_SEARCH_DATA_HIGH";
			break;
		case SCSIOP_SEARCH_DATA_EQUAL:
			scsiopName = "SCSIOP_SEARCH_DATA_EQUAL";
			break;
		case SCSIOP_SEARCH_DATA_LOW:
			scsiopName = "SCSIOP_SEARCH_DATA_LOW";
			break;
		case SCSIOP_SET_LIMITS:
			scsiopName = "SCSIOP_SET_LIMITS";
			break;
		case SCSIOP_READ_POSITION:
			scsiopName = "SCSIOP_READ_POSITION";
			break;
		case SCSIOP_SYNCHRONIZE_CACHE:
			scsiopName = "SCSIOP_SYNCHRONIZE_CACHE";
			break;
		case SCSIOP_COMPARE:
			scsiopName = "SCSIOP_COMPARE";
			break;
		case SCSIOP_COPY_COMPARE:
			scsiopName = "SCSIOP_COPY_COMPARE";
			break;
		case SCSIOP_WRITE_DATA_BUFF:
			scsiopName = "SCSIOP_WRITE_DATA_BUFF";
			break;
		case SCSIOP_READ_DATA_BUFF:
			scsiopName = "SCSIOP_READ_DATA_BUFF";
			break;
		case SCSIOP_CHANGE_DEFINITION:
			scsiopName = "SCSIOP_CHANGE_DEFINITION";
			break;
		case SCSIOP_READ_SUB_CHANNEL:
			scsiopName = "SCSIOP_READ_SUB_CHANNEL";
			break;
		case SCSIOP_READ_TOC:
			scsiopName = "SCSIOP_READ_TOC";
			break;
		case SCSIOP_READ_HEADER:
			scsiopName = "SCSIOP_READ_HEADER";
			break;
		case SCSIOP_PLAY_AUDIO:
			scsiopName = "SCSIOP_PLAY_AUDIO";
			break;
		case SCSIOP_PLAY_AUDIO_MSF:
			scsiopName = "SCSIOP_PLAY_AUDIO_MSF";
			break;
		case SCSIOP_PLAY_TRACK_INDEX:
			scsiopName = "SCSIOP_PLAY_TRACK_INDEX";
			break;
		case SCSIOP_PLAY_TRACK_RELATIVE:
			scsiopName = "SCSIOP_PLAY_TRACK_RELATIVE";
			break;
		case SCSIOP_PAUSE_RESUME:
			scsiopName = "SCSIOP_PAUSE_RESUME";
			break;
		case SCSIOP_LOG_SELECT:
			scsiopName = "SCSIOP_LOG_SELECT";
			break;
		case SCSIOP_LOG_SENSE:
			scsiopName = "SCSIOP_LOG_SENSE";
			break;
		case SCSIOP_STOP_PLAY_SCAN:
			scsiopName = "SCSIOP_STOP_PLAY_SCAN";
			break;
		case SCSIOP_READ_DISK_INFORMATION:
			scsiopName = "SCSIOP_READ_DISK_INFORMATION";
			break;
		case SCSIOP_READ_TRACK_INFORMATION:
			scsiopName = "SCSIOP_READ_TRACK_INFORMATION";
			break;
		case SCSIOP_MODE_SELECT10:
			scsiopName = "SCSIOP_MODE_SELECT10";
			break;
		case SCSIOP_MODE_SENSE10:
			scsiopName = "SCSIOP_MODE_SENSE10";
			break;
		case SCSIOP_REPORT_LUNS:
			scsiopName = "SCSIOP_REPORT_LUNS";
			break;
		case SCSIOP_SEND_KEY:
			scsiopName = "SCSIOP_SEND_KEY";
			break;
		case SCSIOP_REPORT_KEY:
			scsiopName = "SCSIOP_REPORT_KEY";
			break;
		case SCSIOP_MOVE_MEDIUM:
			scsiopName = "SCSIOP_MOVE_MEDIUM";
			break;
		case SCSIOP_LOAD_UNLOAD_SLOT:
			scsiopName = "SCSIOP_LOAD_UNLOAD_SLOT";
			break;
		//case SCSIOP_EXCHANGE_MEDIUM: same as SCSIOP_LOAD_UNLOAD_SLOT
		//	scsiopName = "SCSIOP_EXCHANGE_MEDIUM";
		//	break;
		case SCSIOP_SET_READ_AHEAD:
			scsiopName = "SCSIOP_SET_READ_AHEAD";
			break;
		case SCSIOP_READ_DVD_STRUCTURE:
			scsiopName = "SCSIOP_READ_DVD_STRUCTURE";
			break;
		case SCSIOP_REQUEST_VOL_ELEMENT:
			scsiopName = "SCSIOP_REQUEST_VOL_ELEMENT";
			break;
		case SCSIOP_SEND_VOLUME_TAG:
			scsiopName = "SCSIOP_SEND_VOLUME_TAG";
			break;
		case SCSIOP_READ_ELEMENT_STATUS:
			scsiopName = "SCSIOP_READ_ELEMENT_STATUS";
			break;
		case SCSIOP_READ_CD_MSF:
			scsiopName = "SCSIOP_READ_CD_MSF";
			break;
		case SCSIOP_SCAN_CD:
			scsiopName = "SCSIOP_SCAN_CD";
			break;
		case SCSIOP_PLAY_CD:
			scsiopName = "SCSIOP_PLAY_CD";
			break;
		case SCSIOP_MECHANISM_STATUS:
			scsiopName = "SCSIOP_MECHANISM_STATUS";
			break;
		case SCSIOP_READ_CD:
			scsiopName = "SCSIOP_READ_CD";
			break;
		case SCSIOP_INIT_ELEMENT_RANGE:
			scsiopName = "SCSIOP_INIT_ELEMENT_RANGE";
			break;
		default:
			scsiopName = "SCSIOP_???";
	}
	return scsiopName;
}