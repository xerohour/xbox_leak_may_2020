/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    isoch.c

Abstract:

    Functions used to support isochronous transfers.

Environment:

    Designed for XBOX.

Notes:

Revision History:

    06-29-00 created by Mitchell Dernis (mitchd)

--*/
#ifdef  OHCD_ISOCHRONOUS_SUPPORTED
//
//  Pull in OS headers
//
#include <ntos.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          'DCHO'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("OHCD");


//
//  Pull in usb headers
//
#include <hcdi.h>       //Interface between USBD and HCD
#include "ohcd.h"       //Private OHCD stuff

USBD_STATUS
FASTCALL
OHCD_fIsochOpenEndpoint(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    )
/*++
    Routine Description:
        Called to open an isochronous endpoint.
--*/
{
    POHCD_ISOCH_ENDPOINT     endpoint = NULL;
    USBD_STATUS              status = USBD_STATUS_SUCCESS;
    ULONG                    allocSize;
    ULONG                    maxBuffers;
    ULONG_PTR                endpointMemory;
    UCHAR                    index;
    ULONG_PTR                nextTdPhysical;
    ULONG_PTR                tdPhysical;
    KIRQL                    oldIrql;


    USB_DBG_ENTRY_PRINT(("Entering OHCD_OpenEndpoint"));

    maxBuffers = OHCD_IsochPoolGetMaxBuffers();
    allocSize = maxBuffers*sizeof(OHCD_ISOCH_TRANSFER_DESCRIPTOR)+sizeof(OHCD_ISOCH_ENDPOINT);

    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Allocate the isoch endpoint (with prepended TD's) form pool
    //
    endpointMemory = OHCD_IsochPoolAllocateEndpoint();
    
    //
    //  If there was no memory than fail the open.
    //
    if(!endpointMemory)
    {
        status = USBD_STATUS_NO_MEMORY;
        goto ExitOpenIsochEndpoint;
    }
    
    //
    //  Zero the memory
    //
    RtlZeroMemory((PVOID)endpointMemory, allocSize);

    //
    // Break the memory up into the endpoint and the transfer descriptor memory.
    // (The endpoint starts after the transfer descriptor memory).
    //
    endpoint = (POHCD_ISOCH_ENDPOINT)(
                            endpointMemory +
                            maxBuffers*sizeof(OHCD_ISOCH_TRANSFER_DESCRIPTOR)
                            );
    endpoint->TransferDescriptors = (POHCD_ISOCH_TRANSFER_DESCRIPTOR)(endpointMemory);
    endpoint->PhysicalAddress = OHCD_PoolGetPhysicalAddress(endpoint);
    endpoint->EndpointType = USB_ENDPOINT_TYPE_ISOCHRONOUS;
    endpoint->PollingInterval = 1;
    endpoint->Bandwidth = USBD_CalculateUsbBandwidth(
                                Urb->IsochOpenEndpoint.MaxPacketSize,
                                endpoint->EndpointType, 
                                FALSE
                                );
    endpoint->MaxAttachedBuffers = (UCHAR)maxBuffers;
    endpoint->Flags = (Urb->IsochOpenEndpoint.Flags&URB_FLAG_ISOCH_CIRCULAR_DMA) ? OHCD_ISOCH_EDFL_CIRCULAR_DMA : 0;

    //
    //  Fill out everything in the control structure.
    //  Note the Direction here is set correctly for non-control endpoints.
    //  in the next step we have an if USB_ENDPOINT_TYPE_CONTROL and we fix it
    //  for the control endpoints.
    //
    endpoint->HcEndpointDescriptor.Control.FunctionAddress = Urb->IsochOpenEndpoint.FunctionAddress;
    endpoint->HcEndpointDescriptor.Control.EndpointAddress = Urb->IsochOpenEndpoint.EndpointAddress;
    endpoint->HcEndpointDescriptor.Control.Direction = (USB_ENDPOINT_DIRECTION_MASK & Urb->OpenEndpoint.EndpointAddress) ? 2 : 1;
    endpoint->HcEndpointDescriptor.Control.Speed = 0;
    endpoint->HcEndpointDescriptor.Control.Skip = 1;
    endpoint->HcEndpointDescriptor.Control.Format = 1;
    endpoint->HcEndpointDescriptor.Control.MaximumPacketSize = Urb->IsochOpenEndpoint.MaxPacketSize;

    //
    //  Set the head and tail pointer to point to the first transfer descriptor
    //
    tdPhysical = OHCD_PoolGetPhysicalAddress(endpoint->TransferDescriptors);
    WRITE_HEADP(&endpoint->HcEndpointDescriptor, (ULONG)tdPhysical);
    endpoint->HcEndpointDescriptor.TailP = (ULONG)tdPhysical;
    //
    //  Link up all transfer descriptors in a big circle.
    //  The hardware won't see them because HeadP and TailP point
    //  to the same one.
    //  
    for(index = 0; index < maxBuffers; index++)
    {
        nextTdPhysical = OHCD_PoolGetPhysicalAddress(endpoint->TransferDescriptors+index+1);
        endpoint->TransferDescriptors[index].TdIndexPrev = index-1;
        endpoint->TransferDescriptors[index].TdIndex = index;
        endpoint->TransferDescriptors[index].Endpoint = endpoint;
        endpoint->TransferDescriptors[index].Context = NULL;
        endpoint->TransferDescriptors[index].TransferComplete = NULL;
        endpoint->TransferDescriptors[index].IsochTransferDescriptor.Format=1;
        endpoint->TransferDescriptors[index].IsochTransferDescriptor.NextTD = nextTdPhysical;
        tdPhysical = nextTdPhysical;
    }
    endpoint->TransferDescriptors[index-1].IsochTransferDescriptor.NextTD = 0;
    endpoint->TransferDescriptors[0].TdIndexPrev = index - 1;

    //
    //  Insert the endpoint into the schedule (it will then be visible to the hardware).
    //  (The cast is safe because, the beginning of the endpoint structure is the same
    //  as the beginning of the isochronous endpoint structure.)
    //
    status = OHCD_ScheduleAddEndpointPeriodic(DeviceExtension, (POHCD_ENDPOINT)endpoint);
    //
    //  Udpate URB handle
    //
    if(USBD_SUCCESS(status))
    {
        Urb->IsochOpenEndpoint.EndpointHandle = endpoint;
    }
    else
    {
        Urb->IsochOpenEndpoint.EndpointHandle = NULL;
        //
        // Free the endpoint memory
        //
        OHCD_IsochPoolFreeEndpoint(endpointMemory);
    }

ExitOpenIsochEndpoint:
    Urb->Header.Status = status;
    KeLowerIrql(oldIrql);
    USB_DBG_EXIT_PRINT(("Exiting OHCD_OpenIsochEndpoint: usbdStatus = 0x%0.8x", status));
    return status;
}

USBD_STATUS
FASTCALL
OHCD_fIsochCloseEndpoint(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    )
/*++
    Routine Description:
        This routine is the handler for URB_ISOCH_CLOSE_ENDPOINT
        
        By the time this routine returns, all of the attached buffers can be freed.

    Parameters:

        DeviceExtension - host controller context
        Urb             - urb to handle

    Return Value:

        USBD_STATUS_SUCCESS                     success
--*/
{
    POHCD_ISOCH_ENDPOINT endpoint = (POHCD_ISOCH_ENDPOINT)Urb->IsochCloseEndpoint.EndpointHandle;
    KIRQL oldIrql;
    //
    //  Use high Irql to synchronize.
    //
    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Remove the endpoint from the schedule.
    //
    OHCD_ScheduleRemoveEndpointPeriodic(DeviceExtension, (POHCD_ENDPOINT)endpoint);

    //
    //  Pause the endpoint.  The cast is OK, because the relevant fields are the same.
    //
    OHCD_fPauseEndpoint(DeviceExtension, (POHCD_ENDPOINT)endpoint);
    
    //
    //  Link the URB into the list of pending closes.
    //
    ASSERT(DeviceExtension->PendingCloses != Urb);
    Urb->CloseEndpoint.HcdNextClose = DeviceExtension->PendingCloses;
    DeviceExtension->PendingCloses = Urb;
    //
    //  At this point, the only software that knows about the endpoint is us, so high
    //  Irql is no longer needed.
    //
    KeLowerIrql(oldIrql);
   
    //
    //  Complete this as pending.
    //
    return USBD_STATUS_PENDING;
}

VOID
FASTCALL
OHCD_fIsochCompleteCloseEndpoint(
    IN POHCD_DEVICE_EXTENSION      DeviceExtension,
    IN PURB_ISOCH_CLOSE_ENDPOINT   Urb
    )
{
    
    POHCD_ISOCH_ENDPOINT endpoint = (POHCD_ISOCH_ENDPOINT)Urb->EndpointHandle;
    ULONG_PTR endpointMemory;
    POHCD_ISOCH_TRANSFER_DESCRIPTOR transferDescriptor;
    ULONG descriptorIndex = endpoint->NextFreeTD + (endpoint->MaxAttachedBuffers - endpoint->AttachedBuffers);

    //
    //  Unlock pages of attached buffers
    //
    
    while(endpoint->AttachedBuffers--)
    {
        descriptorIndex = descriptorIndex%endpoint->MaxAttachedBuffers;
        transferDescriptor = endpoint->TransferDescriptors + (descriptorIndex++);
        //
        //  Unlock the page(s)
        //
        //
        MmLockUnlockPhysicalPage(transferDescriptor->IsochTransferDescriptor.BufferPage0, TRUE);
        if((transferDescriptor->IsochTransferDescriptor.BufferPage0 ^ transferDescriptor->IsochTransferDescriptor.BufferEnd) & ~(PAGE_SIZE-1))
        {
            MmLockUnlockPhysicalPage(transferDescriptor->IsochTransferDescriptor.BufferEnd, TRUE);
        }
    }

    //
    //  Find the original endpoint pointer allocated
    //
    
    endpointMemory = ((ULONG_PTR)(endpoint)) - (endpoint->MaxAttachedBuffers*sizeof(OHCD_ISOCH_TRANSFER_DESCRIPTOR));
    OHCD_IsochPoolFreeEndpoint(endpointMemory);
    
    //
    //  Complete the close
    //
    Urb->Hdr.Status = USBD_STATUS_SUCCESS;
    USBD_CompleteRequest((PURB)Urb);
}


USBD_STATUS
FASTCALL
OHCD_fIsochAttachBuffer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    )
/*++
    Routine Description:
        This routine is the handler for URB_ISOCH_ATTACH_BUFFER
        1) check parameters
        2) figure out which TD to use, and fill out basic stuff.
        3) calculate the offset information
        4) Move the tail pointer
        5) Deal with starting frame if transfers are started.
        
    Parameters:
        DeviceExtension - host controller context
        Urb             - urb to handle
    Return Value:
        USBD_STATUS_SUCCESS                     success
        USBD_STATUS_ISOCH_ALREADY_STARTED       the transfers have already been started.
        USBD_STATUS_ISOCH_ATTACH_MORE_BUFFERS   circular DMA and fewer the MaxAttached Buffers are attached.
        USBD_STATUS_BAD_START_FRAME             Start frame is in the past or too far in the future.
--*/

{
    POHCD_ISOCH_ENDPOINT endpoint = (POHCD_ISOCH_ENDPOINT)Urb->IsochAttachBuffer.EndpointHandle;
    USBD_STATUS status = USBD_STATUS_SUCCESS;
    POHCD_ISOCH_TRANSFER_DESCRIPTOR transferDescriptor;
    PUSBD_ISOCH_BUFFER_DESCRIPTOR bufferDescriptor;
    ULONG_PTR physicalAddress;
    ULONG initialByteOffset, byteOffset;
    ULONG frameIndex;
    ULONG tdIndex;
    ULONG bufferSize;
    KIRQL oldIrql;

    ASSERT(endpoint);

    //
    //  Raise IRQL for synchronization purposes.  Also it would
    //  could cause some frame problems if we were interrupted for
    //  an extended period of time.
    //
    oldIrql = KeRaiseIrqlToDpcLevel();
    
    //
    //  make sure we have transfer descriptors for the buffers.
    //
    if(endpoint->MaxAttachedBuffers == endpoint->AttachedBuffers)
    {
        status = USBD_STATUS_ISOCH_TOO_MANY_BUFFERS;
        goto ExitIsochAttachBuffers;
    }
    //
    //  Get a pointer to the buffer descriptor and transfer descriptor
    //
    bufferDescriptor = Urb->IsochAttachBuffer.BufferDescriptor;
    tdIndex = endpoint->NextFreeTD;
    transferDescriptor =  endpoint->TransferDescriptors + tdIndex;
    endpoint->NextFreeTD = (endpoint->NextFreeTD+1)%endpoint->MaxAttachedBuffers;
    
    //
    //  Fill out the software section of the TD.
    //
    transferDescriptor->TransferComplete = bufferDescriptor->TransferComplete;
    transferDescriptor->Context = bufferDescriptor->Context;
    transferDescriptor->Endpoint = endpoint;
    //
    //  Fill copy the Delay interrupt information.
    //
    transferDescriptor->IsochTransferDescriptor.DelayInterrupt = Urb->IsochAttachBuffer.InterruptDelay;
    
    //
    //  Fill out the offset from the pattern.
    //
    transferDescriptor->IsochTransferDescriptor.FrameCount = bufferDescriptor->FrameCount-1;
    initialByteOffset = byteOffset = BYTE_OFFSET(bufferDescriptor->TransferBuffer);
    for(frameIndex = 0; frameIndex < bufferDescriptor->FrameCount; frameIndex++)
    {
        transferDescriptor->IsochTransferDescriptor.Offset_PSW[frameIndex] = (USHORT)(byteOffset|(OHCI_CC_NOT_ACCESSED << 12));
        byteOffset += bufferDescriptor->Pattern[frameIndex];
    }
    bufferSize = (byteOffset-initialByteOffset);

    //
    //  Fillout the start address (and lock it)
    //
    MmLockUnlockBufferPages(bufferDescriptor->TransferBuffer, bufferSize, FALSE);
    physicalAddress = MmGetPhysicalAddress(bufferDescriptor->TransferBuffer);
    transferDescriptor->IsochTransferDescriptor.BufferPage0 = physicalAddress;
    
    //
    //  Fill out the end address (and lock it, if it is a different page).
    //
    physicalAddress = ((ULONG_PTR)(bufferDescriptor->TransferBuffer)) + bufferSize - 1;
    physicalAddress = MmGetPhysicalAddress((PVOID)physicalAddress);
    transferDescriptor->IsochTransferDescriptor.BufferEnd = physicalAddress;

    //
    //   If we are in circular DMA mode, cache the offset information.
    //
    if(endpoint->Flags&OHCD_ISOCH_EDFL_CIRCULAR_DMA)
    {
        RtlCopyMemory((PVOID)(&transferDescriptor->Offsets), (PVOID)transferDescriptor->IsochTransferDescriptor.Offset_PSW, sizeof(USHORT)*8);
    }
    //
    //  If the endpoint is running than we need to deal with the starting frame stuff.
    //
    if(endpoint->Flags&OHCD_ISOCH_EDFL_TRANSFERING)
    {
        //
        //  If the next unused frame is not in the past, use it.
        //  Otherwise, use the current frame (plus one for safety).
        //
        ULONG frame = OHCD_Get32BitFrameNumber(DeviceExtension)+1;
        if( 0 < (((LONG)endpoint->NextUnusedFrame) - ((LONG)frame)) )
        {
            frame = endpoint->NextUnusedFrame;
        }
        transferDescriptor->IsochTransferDescriptor.StartingFrame = frame;
        endpoint->NextUnusedFrame = frame + bufferDescriptor->FrameCount;
    }
    //
    //   Mark that we used a TD
    //
    endpoint->AttachedBuffers++;
    //
    //   Move the tail pointer, if we can, if we cannot, the completion routine
    //   will get it.
    if(endpoint->AttachedBuffers == endpoint->MaxAttachedBuffers)
    {
        ASSERT(0 == transferDescriptor->IsochTransferDescriptor.NextTD);
    } else
    {
        ASSERT(transferDescriptor->IsochTransferDescriptor.NextTD);
        endpoint->HcEndpointDescriptor.TailP = transferDescriptor->IsochTransferDescriptor.NextTD;        
    }
    
   
ExitIsochAttachBuffers:
    //
    //  Done, we can lower the IRQL again.
    //
    KeLowerIrql(oldIrql);
    Urb->Header.Status = status;
    USBD_CompleteRequest(Urb);
    return status;
}

USBD_STATUS
FASTCALL
OHCD_fIsochStartTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    )
/*++
    Routine Description:
        This routine is the handler for URB_ISOCH_START_TRANSFER
        1) check parameters
        2) Make sure transfer's are stopped.
        3) Fix up starting frame information.
        4) Clear the skip bit.
        5) Set the transfering flag.
    Parameters:
        DeviceExtension - host controller context
        Urb             - urb to handle
    Return Value:
        USBD_STATUS_SUCCESS                     success
        USBD_STATUS_ISOCH_ALREADY_STARTED       the transfers have already been started.
        USBD_STATUS_ISOCH_ATTACH_MORE_BUFFERS   circular DMA and fewer the MaxAttached Buffers are attached.
        USBD_STATUS_BAD_START_FRAME             Start frame is in the past or too far in the future.
--*/
{
    ULONG startingFrame, currentFrame;
    UCHAR startingTD, tdIndex;
    USBD_STATUS status = STATUS_SUCCESS;
    KIRQL oldIrql;
    POHCD_ISOCH_ENDPOINT endpoint = (POHCD_ISOCH_ENDPOINT)Urb->IsochStartTransfer.EndpointHandle;
    
    ASSERT(USB_ENDPOINT_TYPE_ISOCHRONOUS == endpoint->EndpointType);
    
    //
    //  Raise IRQL for synchronization purposes.  Also it would
    //  could cause some frame problems if we were interrupted for
    //  an extended period of time.
    //
    oldIrql = KeRaiseIrqlToDpcLevel();
    //
    //  Make sure that the endpoint is not running already.
    //
    if(endpoint->Flags&OHCD_ISOCH_EDFL_TRANSFERING) 
    {
        status = USBD_STATUS_ISOCH_ALREADY_STARTED;
        goto ExitIsochStartTransfer;
    }
    //
    //  If the endpoint has been stopped, make sure it is late enough to restart.
    //
    currentFrame = OHCD_Get32BitFrameNumber(DeviceExtension); //We need the current frame now.
    if(endpoint->Flags & OHCD_ISOCH_EDFL_STOPPING)
    {
        endpoint->Flags &= ~OHCD_ISOCH_EDFL_STOPPING;
        //
        //  If the pause frame is the current frame, than wait a millsecond to
        //  make sure that the stop completed.
        //
        if(endpoint->PauseFrame == currentFrame)
        {
           LARGE_INTEGER Wait;
           KeLowerIrql(oldIrql);  //Can't wait at high Irql
           Wait.QuadPart = -10000;
           KeDelayExecutionThread(KernelMode, FALSE, &Wait);
           oldIrql = KeRaiseIrqlToDpcLevel();
        }
        //The frame has changed by now
        currentFrame = OHCD_Get32BitFrameNumber(DeviceExtension);
    }

    //
    //  If circular DMA, make sure that all of the buffers are attached.
    //
    if(
        (endpoint->Flags&OHCD_ISOCH_EDFL_CIRCULAR_DMA) && 
        (endpoint->MaxAttachedBuffers != endpoint->AttachedBuffers)
    )
    {
        status = USBD_STATUS_ISOCH_ATTACH_MORE_BUFFERS;
        goto ExitIsochStartTransfer;
    }
    //
    //  Check that the starting frame is valid.
    //
    startingFrame = currentFrame+1;
    if(!(Urb->IsochStartTransfer.Flags&URB_FLAG_ISOCH_START_ASAP))
    {
        //
        //  Signed subtraction account for wrap around
        //
        LONG FrameDiff = (LONG)Urb->IsochStartTransfer.FrameNumber - (LONG)startingFrame;
        if( FrameDiff < 0 || FrameDiff > USBD_ISOCH_START_FRAME_RANGE)
        {
            status = USBD_STATUS_BAD_START_FRAME;
            goto ExitIsochStartTransfer;
        } else
        {
            startingFrame = Urb->IsochStartTransfer.FrameNumber;            
        }
    }
    //
    //  Propogate the frame information into all the TDs
    //
    //
    startingTD = (endpoint->MaxAttachedBuffers - endpoint->AttachedBuffers) + endpoint->NextFreeTD;
    startingTD = startingTD%endpoint->MaxAttachedBuffers;
    tdIndex = startingTD;
    do
    {
        endpoint->TransferDescriptors[tdIndex].IsochTransferDescriptor.StartingFrame = startingFrame;
        startingFrame += endpoint->TransferDescriptors[tdIndex].IsochTransferDescriptor.FrameCount+1;        
        tdIndex = (tdIndex+1)%endpoint->MaxAttachedBuffers;
    }while(tdIndex != endpoint->NextFreeTD);

    endpoint->NextUnusedFrame = startingFrame;

    //
    //  Clear the skip bit.
    //
    endpoint->HcEndpointDescriptor.Control.Skip = 0;

    //
    //  Set the OHCD_ISOCH_EDFL_TRANSFERING flag
    //
    endpoint->Flags |= OHCD_ISOCH_EDFL_TRANSFERING;

ExitIsochStartTransfer:
    KeLowerIrql(oldIrql);
    Urb->Header.Status = status;
    USBD_CompleteRequest((PURB)Urb);
    return status;

}

USBD_STATUS
FASTCALL
OHCD_fIsochStopTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    )
/*++
    Routine Description:
        This routine is the handler for URB_ISOCH_STOP_TRANSFER
        1) Set the Skip Bit.
        2) Record the current frame number.
    Parameters:
        DeviceExtension - host controller context
        Urb             - urb to handle
    Return Value:
    USBD_STATUS_SUCCESS or 
    USBD_STATUS_ISOCH_NOT_STARTED if the device has not been started.

--*/
{
    KIRQL oldIrql;
    POHCD_ISOCH_ENDPOINT endpoint = (POHCD_ISOCH_ENDPOINT)Urb->IsochStartTransfer.EndpointHandle;
    USBD_STATUS  status = USBD_STATUS_SUCCESS;

    ASSERT(USB_ENDPOINT_TYPE_ISOCHRONOUS == endpoint->EndpointType);
    
    oldIrql = KeRaiseIrqlToDpcLevel();
    //
    //  If the transfer is started, stop it.
    //
    if(endpoint->Flags&OHCD_ISOCH_EDFL_TRANSFERING)
    {
        //
        //  Set the skip bit.
        //
        endpoint->HcEndpointDescriptor.Control.Skip = TRUE;
        //
        //  Set the frame that we need to wait for.
        //
        endpoint->PauseFrame = OHCD_Get32BitFrameNumber(DeviceExtension)+2;
        //
        //   Change the flags.
        //
        endpoint->Flags &= ~OHCD_ISOCH_EDFL_TRANSFERING;
        endpoint->Flags |= OHCD_ISOCH_EDFL_STOPPING;
    } else
    {
        status = USBD_STATUS_ISOCH_NOT_STARTED;
    }
    KeLowerIrql(oldIrql);
    Urb->Header.Status = status;
    USBD_CompleteRequest((PURB)Urb);
    return status;
}

VOID
FASTCALL
OHCD_fIsochProcessTD(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN POHCD_TRANSFER_DESCRIPTOR TransferDescriptor
    )
/*++
    Routine Description:
        This routine is called every time a transfer descriptor is retired.
        1) copy the status and notify the class driver.
        2) Relink the TD on to the end of the queue.
        3) if circular mode, recopy the offsets and set the starting frame
        4) if circular or needs dummy, move the tail pointer.
        5) if not circular mode, "free" the attached buffer (decrease attached buffer count).
    Parameters:
        DeviceExtension - host controller context
        TransferDescriptor - needs to be cast to an isoch transfer descriptor.
--*/
{
    POHCD_ISOCH_TRANSFER_DESCRIPTOR transferDescriptor = (POHCD_ISOCH_TRANSFER_DESCRIPTOR)TransferDescriptor;
    ULONG tdPhysical;
    POHCD_ISOCH_ENDPOINT endpoint = transferDescriptor->Endpoint;
    UCHAR tdIndexPrev = transferDescriptor->TdIndexPrev;
    PFNUSBD_ISOCH_TRANSFER_COMPLETE       transferComplete;
    PVOID                                 callbackContext;
    USBD_ISOCH_TRANSFER_STATUS            transferStatus;
    BOOLEAN  moveTail;
    
    //
    //  Copy the info we need for the callback, including the status, the callback itself, and the context
    //
    transferStatus.Status = (USBD_STATUS) transferDescriptor->IsochTransferDescriptor.ConditionCode;
    transferStatus.FrameCount = transferDescriptor->IsochTransferDescriptor.FrameCount + 1;
    RtlCopyMemory((PVOID)transferStatus.PacketStatus, (PVOID)transferDescriptor->IsochTransferDescriptor.Offset_PSW, sizeof(USHORT)*8);
    callbackContext = transferDescriptor->Context;
    transferComplete = transferDescriptor->TransferComplete;
    //
    //  Relink the transfer descriptor at the end of the endpoints queue.
    //
    transferDescriptor->IsochTransferDescriptor.NextTD = 0;
    tdPhysical = OHCD_PoolGetPhysicalAddress((PVOID)transferDescriptor);
    endpoint->TransferDescriptors[tdIndexPrev].IsochTransferDescriptor.NextTD = tdPhysical;
    
    //
    //  If we are in circular DMA mode, update the starting frame
    //  and recopy the offset information, and move nextFree
    //
    if(endpoint->Flags&OHCD_ISOCH_EDFL_CIRCULAR_DMA)
    {
        ULONG startingFrame = OHCD_Get32BitFrameNumber(DeviceExtension)+1;
        LONG FrameDiff = (LONG)endpoint->NextUnusedFrame - (LONG)startingFrame;
        if(FrameDiff >= 0)
        {
            startingFrame = endpoint->NextUnusedFrame;
        }
        transferDescriptor->IsochTransferDescriptor.StartingFrame = startingFrame;
        endpoint->NextUnusedFrame = startingFrame + transferDescriptor->IsochTransferDescriptor.FrameCount+1;
        RtlCopyMemory((PVOID)transferDescriptor->IsochTransferDescriptor.Offset_PSW, (PVOID)&transferDescriptor->Offsets, sizeof(USHORT)*8);
        //
        //  NextFreeTD is moved so that if we get a stop request it can restart on the
        //  next TD.  This may or may not be desirable.
        //
        endpoint->NextFreeTD = (endpoint->NextFreeTD+1)%endpoint->MaxAttachedBuffers;
        //
        //  Always move the tail pointer for circular mode.
        //
        moveTail = TRUE;
    } else
    {
        //
        //  Unlock the page(s)
        //
        //
        MmLockUnlockPhysicalPage(transferDescriptor->IsochTransferDescriptor.BufferPage0, TRUE);
        if((transferDescriptor->IsochTransferDescriptor.BufferPage0 ^ transferDescriptor->IsochTransferDescriptor.BufferEnd) & ~(PAGE_SIZE-1))
        {
            MmLockUnlockPhysicalPage(transferDescriptor->IsochTransferDescriptor.BufferEnd, TRUE);
        }
        //
        //  The number of attached buffers (prior to this one completing) is at the
        //  maximum, then the attach routine would not have been able to move the
        //  tail pointer to point to a dummy, so set moveTail.
        //
        moveTail = (endpoint->AttachedBuffers == endpoint->MaxAttachedBuffers) ? TRUE : FALSE;
        
        //
        //  Mark that the buffer is now unattached.  (i.e. the TD can be reused.
        //
        endpoint->AttachedBuffers--;
    }
    //
    //  If moveTail is TRUE, either because we are in circular, or because we need to
    //  add a dummy TD, then do it now.
    if(moveTail)
    {
        endpoint->HcEndpointDescriptor.TailP = tdPhysical;
    }
    //
    //  Call the class driver's callback
    //
    transferComplete(&transferStatus, callbackContext);
}

#endif  //OHCD_ISOCHRONOUS_SUPPORTED
