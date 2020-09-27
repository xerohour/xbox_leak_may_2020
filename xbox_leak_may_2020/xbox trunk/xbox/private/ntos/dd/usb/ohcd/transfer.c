/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    transfer.c

Abstract:
    
    Implementation of functions related to queueing and processing transfer.
    However, isochronous transfers support is not in this file, see isoch.c

    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    01-20-00 created by Mitchell Dernis (mitchd)

--*/


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


//----------------------------------------------------------------------------
// Forward declaration of functions defined and used only this in this module
//----------------------------------------------------------------------------
USHORT
FASTCALL
OHCD_fGetTDsRequired(
    PURB            Urb,
    POHCD_ENDPOINT  Endpoint
    );

USBD_STATUS
FASTCALL
OHCD_fQueueInterruptTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN POHCD_ENDPOINT           Endpoint,
    IN PURB                     Urb
    );

USBD_STATUS
FASTCALL
OHCD_fQueueBulkTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    );

USBD_STATUS
FASTCALL
OHCD_fQueueControlTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    );

VOID
FASTCALL
OHCD_fProgramTransfer(
        POHCD_DEVICE_EXTENSION  DeviceExtension,
        POHCD_ENDPOINT          Endpoint,
        PURB                    Urb
    );

ULONG
FASTCALL
OHCD_fMapTransfer(
    IN OUT  PVOID   *CurrentVa,
    IN OUT  PULONG  BytesRemaining,
    OUT     PULONG  BytesMapped
    );

//----------------------------------------------------------------------------
// Implementation of function called from other modules:
//      OHCD_fQueueTransferRequest
//----------------------------------------------------------------------------
USBD_STATUS
FASTCALL
OHCD_fQueueTransferRequest(
    POHCD_DEVICE_EXTENSION  DeviceExtension,
    PURB                    Urb
    )
/*++

Routine Description:
    
    QueueTransferRequest is the first stage of processing an asynchronous
    transfer request.  Part of the TD conservation program requires
    different queueing depending on endpoint type.  In this routine,
    we:
        1) Calculate TDs required.
        2) Dispatch to proper endpoint type determined queue routine.

Arguments:
    
    DeviceExtension - Device extension for our OHCD instance.
    HcdUrb          - Transfer URB to queue.

Return Value:

    STATUS_SUCCESS.  Otherwise we will KeBugcheck before returning.

--*/
{
    POHCD_ENDPOINT              endpoint;
    USBD_STATUS                 status;
    KIRQL                       oldIrql;
            
    USB_DBG_ENTRY_PRINT(("Entering OHCD_fQueueTransferRequest"));

    //
    //  Get the endpoint.
    //
    endpoint = (POHCD_ENDPOINT)Urb->CommonTransfer.EndpointHandle;
        
    //
    //  Figure out how many TDs this transfer requires
    //
    Urb->CommonTransfer.Hca.HcdTDCount = OHCD_fGetTDsRequired(Urb, endpoint);
        
    //
    //  Synchronize access to queues
    //
    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Class driver should be smart enough not to submit transfers
    //  while an endpoint is being closed.
    //
    ASSERT(!(OHCD_ENDPOINT_FLAG_CLOSING&endpoint->Flags));

    //
    //  We assume that it going to get queue.  Since the various
    //  queue routines may actually get programmed before returning
    //  here, we want to increment before calling them.
    //  
    endpoint->QueuedUrbCount++;
    Urb->CommonTransfer.Hca.HcdUrbFlags = OHCD_URB_FLAG_QUEUED;
    Urb->CommonTransfer.Hca.HcdUrbLink = NULL;
            
    //
    //  Queue the URB.  Each type of endpoint has its own queue
    //
    switch(endpoint->EndpointType)
    {
        case USB_ENDPOINT_TYPE_INTERRUPT:
            ASSERT(URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER==Urb->Header.Function);
            status = OHCD_fQueueInterruptTransfer(DeviceExtension, endpoint, Urb);
            break;
        case USB_ENDPOINT_TYPE_BULK:
            ASSERT(URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER==Urb->Header.Function);
            status = OHCD_fQueueBulkTransfer(DeviceExtension, Urb);
            break;
        case USB_ENDPOINT_TYPE_CONTROL:
            ASSERT(URB_FUNCTION_CONTROL_TRANSFER==Urb->Header.Function);
            status = OHCD_fQueueControlTransfer(DeviceExtension, Urb);
            break;
        default:
            //ISOCH endpoint never expected here.
            USB_DBG_ERROR_PRINT(("Unrecognized endpoint type in OHCD_fQueueTransferRequest\nClass driver is probably at fault."));
            status = USBD_STATUS_REQUEST_FAILED;
    }

    //
    //  If it didn't actually get queued than, we want to decrease 
    //  queue count.
    //
    if(USBD_ERROR(status))
    {
        Urb->CommonTransfer.Hca.HcdUrbFlags = 0;
        endpoint->QueuedUrbCount--;
    }

    //
    //  Done with queues
    //
    KeLowerIrql(oldIrql);

    USB_DBG_EXIT_PRINT(("Exiting OHCD_fQueueTransferRequest"));
    return status;
}

USBD_STATUS
FASTCALL
OHCD_fQueueInterruptTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN POHCD_ENDPOINT           Endpoint,
    IN PURB                     Urb
    )
{
    
    ASSERT_DISPATCH_LEVEL();

    //
    //  Check that this transfer is a legal transfer
    //
    if( Urb->BulkOrInterruptTransfer.Hca.HcdTDCount > OHCD_INTERRUPT_TD_QUOTA)
    {
        return USBD_STATUS_TRANSFER_TOO_LONG;
    }
    
    //
    //  Add URB to endpoints pending queue
    //
    if(Endpoint->PendingUrbTailP)
    {
        Endpoint->PendingUrbTailP->CommonTransfer.Hca.HcdUrbLink = Urb;
    }else
    {
        Endpoint->PendingUrbHeadP = Urb;
    }
    Endpoint->PendingUrbTailP = Urb;

    //
    //  Try programming this transfer
    //
    OHCD_fProgramInterruptTransfer(DeviceExtension, Endpoint);

    return USBD_STATUS_PENDING;
}


VOID
FASTCALL
OHCD_fProgramInterruptTransfer(
    POHCD_DEVICE_EXTENSION  DeviceExtension,
    POHCD_ENDPOINT          Endpoint
    )   
/*++

Routine Description:
    
    This routine is called in two places:
        1) When a new URB is placed on the queue for endpoint.
        2) When an URB completes.
    
    Interrupt endpoints queue URBs on a per endpoint basis.  Each endpoint
    has a quota of OHCD_INTERRUPT_TD_QUOTA (aside from the dummy TD).
    This routine will program all pending URBs until OHCD_INTERRUPT_TD_QUOTA
    TDs are in use.

    If an URB is encountered that requires more than OHCD_INTERRUPT_TD_QUOTA
    it is failed immediately.

Arguments:

   Endpoint - Endpoint to process.

Return Value:

   None.

--*/
{
    PURB_BULK_OR_INTERRUPT_TRANSFER urb;
    
    USB_DBG_ENTRY_PRINT(("Entry OHCD_ProgramInterruptTransfer"));

    
    ASSERT_DISPATCH_LEVEL();

    //
    //  If there are URBs waiting
    //  try to program them.
    //
    while(Endpoint->PendingUrbHeadP)
    {
        urb = &Endpoint->PendingUrbHeadP->BulkOrInterruptTransfer;
        
        //
        //  If there are not enough TDs available than break from this loop.
        //
        if( (urb->Hca.HcdTDCount + Endpoint->TDInUseCount) > OHCD_INTERRUPT_TD_QUOTA)
        {
            break;
        }

        //*
        //* There are enough TDs to proceed to program this transfer.
        //*
        
        //
        //  Remove URB from queue
        //
        Endpoint->PendingUrbHeadP = urb->Hca.HcdUrbLink;
        if(NULL == Endpoint->PendingUrbHeadP) Endpoint->PendingUrbTailP = NULL;

        //
        //  Mark the TDs as in use
        //
        Endpoint->TDInUseCount += urb->Hca.HcdTDCount;
    
        //
        //  Delegate the actual programming process off
        //  to a routine common to all endpoint types.
        //
        OHCD_fProgramTransfer(DeviceExtension, Endpoint, (PURB)urb);
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_ProgramInterruptTransfer"));
}


USBD_STATUS
FASTCALL
OHCD_fQueueBulkTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    )
{
    ASSERT_DISPATCH_LEVEL();

    //
    //  Check that this transfer is a legal transfer
    //
    if( Urb->BulkOrInterruptTransfer.Hca.HcdTDCount > OHCD_PoolGetBulkQuota())
    {
        return USBD_STATUS_TRANSFER_TOO_LONG;
    }

    //
    //  Add URB to endpoints pending queue (at the tail).
    //
    if(DeviceExtension->BulkUrbHeadP)
    {
        DeviceExtension->BulkUrbTailP->CommonTransfer.Hca.HcdUrbLink = Urb;
    }else
    {
        DeviceExtension->BulkUrbHeadP = Urb;
    }
    DeviceExtension->BulkUrbTailP = Urb;

    //
    //  Try programming this transfer
    //
    OHCD_fProgramBulkTransfer(DeviceExtension);

    return USBD_STATUS_PENDING;
}

VOID
FASTCALL
OHCD_fProgramBulkTransfer(
    POHCD_DEVICE_EXTENSION  DeviceExtension
    )
{
    PURB_BULK_OR_INTERRUPT_TRANSFER urb;
        
    USB_DBG_ENTRY_PRINT(("Entry OHCD_ProgramBulkTransfer"));

    
    ASSERT_DISPATCH_LEVEL();

    //
    //  If there are URBs waiting
    //  try to program them.
    //
    while(DeviceExtension->BulkUrbHeadP)
    {
        urb = &DeviceExtension->BulkUrbHeadP->BulkOrInterruptTransfer;
        
        //
        //  Draw TDs from the Bulk Quota (this does nothing other than mark that
        //  we will allocate the TDs)
        if(!OHCD_PoolDebitBulkTDQuota(urb->Hca.HcdTDCount))
        {
            break;
        }

        //*
        //* There are enough TDs to proceed to program this transfer.
        //*
        
        //
        //  Remove URB from queue
        //
        DeviceExtension->BulkUrbHeadP = urb->Hca.HcdUrbLink;
        if(NULL == DeviceExtension->BulkUrbHeadP) DeviceExtension->BulkUrbTailP = NULL;

        //
        //  Delegate the actual programming process off
        //  to a routine common to all endpoint types.
        //
        OHCD_fProgramTransfer(DeviceExtension, (POHCD_ENDPOINT)urb->EndpointHandle, (PURB)urb);
    
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_ProgramBulkTransfer"));
}

USBD_STATUS
FASTCALL
OHCD_fQueueControlTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    )
{
    ASSERT_DISPATCH_LEVEL();

    //
    //  Check that this transfer is a legal transfer
    //
    if( Urb->ControlTransfer.Hca.HcdTDCount > OHCD_PoolGetControlQuota())
    {
        return USBD_STATUS_TRANSFER_TOO_LONG;
    }

    //
    //  Add URB to endpoints pending queue (at the tail)
    //
    if(DeviceExtension->ControlUrbHeadP)
    {
        DeviceExtension->ControlUrbTailP->CommonTransfer.Hca.HcdUrbLink = Urb;
    }else
    {
        DeviceExtension->ControlUrbHeadP = Urb;
    }
    DeviceExtension->ControlUrbTailP = Urb;

    //
    //  Try programming this transfer
    //
    OHCD_fProgramControlTransfer(DeviceExtension);

    return USBD_STATUS_PENDING;
}

VOID
FASTCALL
OHCD_fProgramControlTransfer(
    POHCD_DEVICE_EXTENSION  DeviceExtension
    )
{
    PURB_CONTROL_TRANSFER   urb;
        
    USB_DBG_ENTRY_PRINT(("Entry OHCD_ProgramControlTransfer"));

    
    ASSERT_DISPATCH_LEVEL();

    //
    //  If there are URBs waiting
    //  try to program them.
    //
    while(DeviceExtension->ControlUrbHeadP)
    {
        urb = &DeviceExtension->ControlUrbHeadP->ControlTransfer;
        
        //
        //  Draw TDs from the Bulk Quota (this does nothing other than mark that
        //  we will allocate the TDs)
        if(!OHCD_PoolDebitControlTDQuota(urb->Hca.HcdTDCount))
        {
            break;
        }

        //*
        //* There are enough TDs to proceed to program this transfer.
        //*
        
        //
        //  Remove URB from queue
        //
        DeviceExtension->ControlUrbHeadP = urb->Hca.HcdUrbLink;
        if(NULL == DeviceExtension->ControlUrbHeadP) DeviceExtension->ControlUrbTailP = NULL;

        //
        //  Delegate the actual programming process off
        //  to a routine common to all endpoint types.
        //
        OHCD_fProgramTransfer(DeviceExtension, (POHCD_ENDPOINT)urb->EndpointHandle, (PURB)urb);
    
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_ProgramControlTransfer"));
}

//----------------------------------------------------------------------------
// Implementation of locally declared utility functions:
//
//  OHCD_GetTDsRequired
//  OHCD_AdapterControl
//----------------------------------------------------------------------------


USHORT
FASTCALL
OHCD_fGetTDsRequired(
    PURB            Urb,
    POHCD_ENDPOINT  Endpoint
    )
/*++

Routine Description:

    Local utilitiy function to calculate number of TDs required
    to transmit the data.
Arguments:

    Urb - URB for which to calculate required number of TDs

Return Value:

   Number of TDs required.

--*/
{       
        //
        //  Initialize to zero.
        //
        USHORT  numTDsRequired = 0;
        USHORT  maxPacket = (USHORT)Endpoint->HcEndpointDescriptor.Control.MaximumPacketSize;
                
        //
        //  Zero length transfers should never happen, unless it is
        //  a command on a control pipe.
        //
        ASSERT(Urb->CommonTransfer.TransferBufferLength || (USB_ENDPOINT_TYPE_CONTROL == Endpoint->EndpointType));
        
        if(Urb->CommonTransfer.TransferBufferLength)
        {
            //
            //  How many TD's do we need.
            //
            numTDsRequired += (USHORT)(Urb->CommonTransfer.TransferBufferLength/maxPacket);
            //
            // If it wasn't evenly divisible we need to add one.
            //
            if(Urb->CommonTransfer.TransferBufferLength%maxPacket)
            {
                numTDsRequired++;
            }
        }
        //
        //  If it is a control endpoint, there is a setup packet
        //  and a status packet.  We need a TD for each, plus the
        //  setup and status data each take up 8 bytes. We just
        //  grab a descritpor block for this.  So we need three
        //  extra blocks.
        //
        if(USB_ENDPOINT_TYPE_CONTROL == Endpoint->EndpointType)
        {
            numTDsRequired += 3;
        }
        return numTDsRequired;
}

VOID
FASTCALL
OHCD_fProgramTransfer(
        POHCD_DEVICE_EXTENSION  DeviceExtension,
        POHCD_ENDPOINT          Endpoint,
        PURB                    Urb
    )
/*++

Routine Description:

    This routine performs the actual work for control, interrupt, and bulk
    endpoint.  For isochronous endpoints, this routine never gets called.

Algorithm Description:

    This routine takes the form of a loop within a loop.
    The outer loop calls IoMapTransfer, which will map from
    the start of the user buffer to the end of the page.
    However, the data that can be transmitted within a single
    packet is limited to MaxPacketSize(8,16,32, or 64) which 
    is device and endpoint dependent.

    The inner loop creates these TDs.  However, it is also
    possible for a single TD to cross a page boundary, so the
    loop take into account that the first TD of page may begin
    on a previous page.

    There are a couple of other little annoyances:
        1) If it is a control transfer, we need to
        generate a TD for the setup and status packets.
        2) At the end we make sure that the endpoint is in order
        and that all the flags are set correctly.

Arguments:  

    HcdUrb - URB for which to calculate required number of TDs

Return Value:

   Number of TDs required.

--*/

{
    ULONG   maxPacketSize           = Endpoint->HcEndpointDescriptor.Control.MaximumPacketSize;
    PVOID   currentVa;
    ULONG   bytesRemaining          = Urb->CommonTransfer.TransferBufferLength;
    ULONG   bytesMapped             = 0;
    ULONG   currentPa               = 0;
    UCHAR   dataToggle              = OHCI_TD_TOGGLE_FROM_ED;
    UCHAR   hostControllerNumber    = (UCHAR)DeviceExtension->HostControllerNumber;
    ULONG   prevPageResidualPa      = 0;
    ULONG   prevPageResidualBytes   = 0;

    
    POHCD_TRANSFER_DESCRIPTOR       firstTD;
    POHCD_TRANSFER_DESCRIPTOR       previousTD = NULL;
    POHCD_TRANSFER_DESCRIPTOR       currentTD;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_ProgramTransfer."));

    //
    //  Traceout some summary stats of the URB
    //
    USB_DBG_TRACE_PRINT(("Programming DMA for URB @0x%0.8x.", Urb));
    USB_DBG_TRACE_PRINT(("  TransferBuffer        @0x%0.8x", Urb->CommonTransfer.TransferBuffer ));
    USB_DBG_TRACE_PRINT(("  TransferBufferLength  %d bytes", Urb->CommonTransfer.TransferBufferLength ));
    USB_DBG_TRACE_PRINT(("  MaximumPacketSize     %d bytes", maxPacketSize));
    USB_DBG_TRACE_PRINT(("  TDs Required          %d", Urb->CommonTransfer.Hca.HcdTDCount));
    USB_DBG_TRACE_PRINT(("  Endpoint              @0x%0.8x", Endpoint));
    USB_DBG_TRACE_PRINT(("  Control Endpoint?     %s", 
        (USB_ENDPOINT_TYPE_CONTROL == Endpoint->EndpointType) ? "Yes" : "No"));
    
    //
    //  We are about to program a TD that was previously queued.
    //  Adjust the counts that are kept with the Endpoint.
    //
    Endpoint->QueuedUrbCount--;
    Endpoint->ProgrammedUrbCount++;
    Urb->CommonTransfer.Hca.HcdUrbFlags &= ~OHCD_URB_FLAG_QUEUED;
    Urb->CommonTransfer.Hca.HcdUrbFlags |= OHCD_URB_FLAG_PROGRAMMED;

    //
    //  Setup the TD basics. Allocate the first TD.  However, in most cases we can
    //  recycle the dummy. Reuse the dummy at the tail for the first TD
    //
    if(Endpoint->HcEndpointDescriptor.TailP)
    {
        firstTD = 
        currentTD = OHCD_PoolTDFromPhysicalAddress(Endpoint->HcEndpointDescriptor.TailP);
    } else
    {
        //
        //  We need to allocate the head TD.
        //
        firstTD = 
        currentTD = OHCD_PoolAllocateTD(hostControllerNumber);
        
        //
        //  Endpoint sure ought to be paused
        //
        ASSERT(1==Endpoint->HcEndpointDescriptor.Control.Skip);
        //
        //  Write this newly allocated TD to the head
        //
        WRITE_HEADP(&Endpoint->HcEndpointDescriptor, firstTD->PhysicalAddress)
    }
    
    //
    //  If we are a control interface, handle the setup packet
    //
    if(USB_ENDPOINT_TYPE_CONTROL == Endpoint->EndpointType)
    {
        POHCD_SETUP_BUFFER SetupBuffer;
        //
        //  We need a descriptor block to hold the setup packet data
        //  and the status packet data.
        //  (we are not really using the TD as a TD, so don't stamp
        //  it with the hostControllerNumber)
        //
        SetupBuffer = (POHCD_SETUP_BUFFER) OHCD_PoolAllocateTD(0xFE-hostControllerNumber);

        //
        //  Copy over the setup bytes (8 of them).
        //  Normally I object to obfuscated code like this,
        //  but you cannot call RtlCopyMemory from DISPATCH_LEVEL, so I will
        //  use this trick I saw in the original Win2K driver.
        //
        *((PLONGLONG)SetupBuffer->Setup) = *((PLONGLONG)&Urb->ControlTransfer.SetupPacket);
        
        //
        //  We will use the firstTD for the setup packet, so allocate a new TD
        //  for the first data packet.
        //
        currentTD = OHCD_PoolAllocateTD(hostControllerNumber);
        
        //
        //  Fill out the hardware mandated fields
        //
        firstTD->HcTransferDescriptor.BufferRounding = FALSE;
        firstTD->HcTransferDescriptor.Direction_PID = OHCI_TD_DIRECTION_PID_SETUP;
        firstTD->HcTransferDescriptor.DelayInterrupt = OHCI_TD_DELAY_INTERRUPT_NONE;
        firstTD->HcTransferDescriptor.DataToggle = dataToggle = OHCI_TD_TOGGLE_DATA0;
        firstTD->HcTransferDescriptor.ErrorCount = 0;
        firstTD->HcTransferDescriptor.ConditionCode = OHCI_CC_NOT_ACCESSED;
        firstTD->HcTransferDescriptor.CurrentBufferPointer = SetupBuffer->PhysicalAddress;
        firstTD->HcTransferDescriptor.NextTD = currentTD->PhysicalAddress;
        firstTD->HcTransferDescriptor.BufferEnd = SetupBuffer->PhysicalAddress + 7;
        //
        //  Now the software fields
        //
        firstTD->Endpoint = Endpoint;
        firstTD->Flags = 0;
        firstTD->Type = OHCD_TD_TYPE_SETUP;
        firstTD->Bytes = 0;
        firstTD->Urb = Urb;
    }
    
    //
    //  Lock the buffer pagesand get the initial value for currentVa,
    //  if and only if there is a data phase.
    //
    if(bytesRemaining)
    {
        #ifdef DVTSNOOPBUG
        OHCD_PoolStartDoubleBufferTransfer(Urb);
        #endif
        MmLockUnlockBufferPages(Urb->CommonTransfer.TransferBuffer, bytesRemaining, FALSE);
        currentVa = Urb->CommonTransfer.TransferBuffer;
    }else
    {
        //
        //  It is a dataless control transfer.  Set the direction so that
        //  the status stage will be an IN.  Status is always reverse of
        //  data, so we set the direction to OUT.
        //
        Urb->ControlTransfer.TransferDirection = OHCI_TD_DIRECTION_PID_OUT;
    }

    //
    //  Now loop until we map the whole user buffer.
    //
    while(bytesRemaining)
    {
        //
        //  Get a physical address
        //
        currentPa = OHCD_fMapTransfer(
            &currentVa,
            &bytesRemaining,
            &bytesMapped
            );

        //
        //  Loop as long as: 
        //      * We have enough buffer mapped to program a whole packet.
        //      * Or it is the last packet.
        //
        while( 
                ((bytesMapped + prevPageResidualBytes) >= maxPacketSize) || 
                ( (0 != bytesMapped) && (0 == bytesRemaining) )
        )
        {
            //
            //  Fill out TD's beginning and ending buffer
            //  physical pointers, and update currentPa and bytesMapped
            //  for next iteration.
            //
            //  The first case, handles a TD that crosses a page boundary
            //  for non-contiguous physical memory.
            //
            if(prevPageResidualBytes)
            {
                ULONG   bytesFromCurrentPage;
                //
                //  Record the start of the buffer (which is from the previousPage)
                //
                currentTD->HcTransferDescriptor.CurrentBufferPointer = prevPageResidualPa;
                        
                //
                //  Find BufferEnd which is on the current page
                //
                bytesFromCurrentPage = maxPacketSize - prevPageResidualBytes;
                if(bytesMapped < bytesFromCurrentPage) bytesFromCurrentPage = bytesMapped;
                currentPa += bytesFromCurrentPage;
                bytesMapped -= bytesFromCurrentPage;
                currentTD->Bytes = (UCHAR)(prevPageResidualBytes + bytesFromCurrentPage);
                prevPageResidualBytes = 0;
                
            }
            else
            {
                //
                //  Record the start of the buffer.
                //
                currentTD->HcTransferDescriptor.CurrentBufferPointer = currentPa;
                //
                //  Find BufferEnd
                //
                if(bytesMapped < maxPacketSize)
                {
                    currentPa += bytesMapped;
                    currentTD->Bytes = (UCHAR)bytesMapped;
                    bytesMapped = 0;
                }
                else
                {
                    currentTD->Bytes = (UCHAR)maxPacketSize;
                    currentPa += maxPacketSize;
                    bytesMapped -= maxPacketSize;
                }
            }
            //
            //  Record BufferEnd.
            //
            currentTD->HcTransferDescriptor.BufferEnd =  currentPa - 1;

            //
            //  Setup the rest of the TD
            //
            currentTD->Endpoint = Endpoint;
            currentTD->Flags = 0;
            currentTD->Type = OHCD_TD_TYPE_DATA;
            currentTD->HcTransferDescriptor.BufferRounding = FALSE;
            currentTD->HcTransferDescriptor.ConditionCode = OHCI_CC_NOT_ACCESSED;
            dataToggle ^= OHCI_TD_TOGGLE_DATA_MASK;
            currentTD->HcTransferDescriptor.DataToggle = dataToggle;
            currentTD->HcTransferDescriptor.DelayInterrupt = OHCI_TD_DELAY_INTERRUPT_NONE;
            currentTD->HcTransferDescriptor.Direction_PID = Urb->ControlTransfer.TransferDirection;
            currentTD->HcTransferDescriptor.ErrorCount = 0;
            currentTD->Urb = Urb;
            
            //
            //  To continue we need to allocate another TD and initialize the
            //  basic stuff.
            //
            previousTD = currentTD;
            currentTD = OHCD_PoolAllocateTD(hostControllerNumber);
            
            //
            //  Link the new TD on to the end of the previous one
            //
            previousTD->HcTransferDescriptor.NextTD = currentTD->PhysicalAddress;

            USB_DBG_TRACE_PRINT(("Loop to assign another TD: bytesMapped(i.e. remaining from this page) = %d", bytesMapped));
        }
        //
        //  Set the previous page stuff for the next loop
        //
        prevPageResidualPa = currentPa;
        prevPageResidualBytes = bytesMapped;
    }

    //
    //  A short packet is OK on last data packet if thed
    //  caller (who submitted the URB) wants it to be OK.
    //  Notice above that the default is FALSE. Even if the caller
    //  allows short packets, the FailedTD routine must be called
    //  to handle short packets in the middle of a transfer, as the
    //  extra TD's must be cleaned from the schedule.
    //
    if(previousTD && Urb->CommonTransfer.ShortTransferOK)
    {
        previousTD->HcTransferDescriptor.BufferRounding = TRUE;
    } 

    //
    //  If it is a control endpoint,
    //  fill out the TD for the status packet.
    //
    if(USB_ENDPOINT_TYPE_CONTROL == Endpoint->EndpointType)
    {
        //
        //  Fill out the Hardware mandated fields
        //
        currentTD->HcTransferDescriptor.BufferRounding = FALSE;
        //
        //  Status Packet must reverse the direction of the data packets.
        //
        currentTD->HcTransferDescriptor.Direction_PID = (OHCI_TD_DIRECTION_PID_IN == Urb->CommonTransfer.TransferDirection) ?
                                                                            OHCI_TD_DIRECTION_PID_OUT : OHCI_TD_DIRECTION_PID_IN;
        currentTD->HcTransferDescriptor.DelayInterrupt = Urb->CommonTransfer.InterruptDelay;
        currentTD->HcTransferDescriptor.DataToggle = OHCI_TD_TOGGLE_DATA1;
        currentTD->HcTransferDescriptor.ErrorCount = 0;
        currentTD->HcTransferDescriptor.ConditionCode = OHCI_CC_NOT_ACCESSED;
        currentTD->HcTransferDescriptor.CurrentBufferPointer = 0;
        currentTD->HcTransferDescriptor.BufferEnd = 0;
        //
        //  Now the software fields
        //
        currentTD->Endpoint = Endpoint;
        currentTD->Flags = OHCD_TD_FLAG_LAST_TD;
        currentTD->Type = OHCD_TD_TYPE_STATUS;
        currentTD->Urb = Urb;
        currentTD->Bytes = 0;
        //
        //  We need a TD for the dummy packet
        //
        previousTD = currentTD;
        currentTD = OHCD_PoolAllocateTD(hostControllerNumber);
        previousTD->HcTransferDescriptor.NextTD = currentTD->PhysicalAddress;
    }
    else
    //
    //  Otherwise we need to signal an interrupt on the last data packet, and mark the
    //  packet as the last.
    //
    {
        previousTD->HcTransferDescriptor.DelayInterrupt = Urb->CommonTransfer.InterruptDelay;
        previousTD->Flags = OHCD_TD_FLAG_LAST_TD;
    }

    //
    //  Now fill out the dummy TD
    //
    currentTD->Type = OHCD_TD_TYPE_DUMMY;
    
    //
    //  Wipe out the transfer length, we will increment it as stuff completes
    //
    Urb->CommonTransfer.Hca.HcdOriginalLength = (USHORT)Urb->CommonTransfer.TransferBufferLength;
    Urb->CommonTransfer.TransferBufferLength = 0;

    //
    //  Update the tail pointer
    //
    Endpoint->HcEndpointDescriptor.TailP = currentTD->PhysicalAddress;
    
    //
    //  make sure that SKip bit is not set on the endpoint
    //  (unless it is supposed to be paused!)
    if(0==Endpoint->PendingPauseCount)
        Endpoint->HcEndpointDescriptor.Control.Skip = 0;
    
    //
    //  Now we need to tell the host controller that we added to the
    //  end of the TD queue if it is control or bulk
    //
    if(USB_ENDPOINT_TYPE_CONTROL == Endpoint->EndpointType)
    {
        WRITE_REGISTER_ULONG((PULONG)&DeviceExtension->OperationalRegisters->HcCommandStatus, HCCS_ControlListFilled);
    }
    else if(USB_ENDPOINT_TYPE_BULK == Endpoint->EndpointType)
    {
        WRITE_REGISTER_ULONG((PULONG)&DeviceExtension->OperationalRegisters->HcCommandStatus, HCCS_BulkListFilled);
    }
    
    USB_DBG_EXIT_PRINT(("Exiting OHCD_ProgramTransfer."));
    return;
}


ULONG
FASTCALL
OHCD_fMapTransfer(
    IN OUT  PVOID   *CurrentVa,
    IN OUT  PULONG  BytesRemaining,
    OUT     PULONG  BytesMapped
    )
/*++
    This routine is used instead of IoMapTransfer.  It works directly on
    a buffer, returning one page at a time.
--*/
{
    ULONG   returnAddress;

    //
    //  Get Physical address, and figure out number
    //  of bytes to the end of the page.
    //
    returnAddress = MmGetPhysicalAddress(*CurrentVa);
    *BytesMapped = PAGE_SIZE - BYTE_OFFSET(*CurrentVa);
    //
    //  Fix up return values
    //
    if(*BytesMapped > *BytesRemaining)
    {
        *BytesMapped = *BytesRemaining;
    }
    *BytesRemaining -= *BytesMapped;
    *CurrentVa = (PVOID)((ULONG)*CurrentVa + *BytesMapped);
    
    return returnAddress;
}
