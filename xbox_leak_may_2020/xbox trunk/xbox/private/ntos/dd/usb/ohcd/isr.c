/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    isr.c

Abstract:
    
    Implementation of the OpenHCI interrupt service routine and related code
    including the DPC.
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    01-24-00 created by Mitchell Dernis (mitchd)

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

int IsrEntryCount=0;
//
//  Pull in usb headers
//
#include <hcdi.h>       //Interface between USBD and HCD
#include "ohcd.h"       //Private OHCD stuff


//----------------------------------------------------------------------------
// Forward declaration of functions defined and used only this in this module
//----------------------------------------------------------------------------
VOID
FASTCALL
OHCD_fProcessDoneTD(
    POHCD_DEVICE_EXTENSION      DeviceExtension,
    POHCD_TRANSFER_DESCRIPTOR   TD
    );

VOID
FASTCALL
OHCD_fProcessFailedTD(
    POHCD_DEVICE_EXTENSION      DeviceExtension,
    POHCD_TRANSFER_DESCRIPTOR   TD
    );

VOID
FASTCALL
OHCD_fCompleteRequest(
    IN POHCD_DEVICE_EXTENSION DeviceExtension, 
    IN POHCD_ENDPOINT Endpoint,
    IN PURB Urb
    );

VOID
FASTCALL
OHCD_fCreditTDQuota(
    IN POHCD_DEVICE_EXTENSION DeviceExtension, 
    IN POHCD_ENDPOINT Endpoint
    );

BOOLEAN
FASTCALL
OHCD_fProcessCancelPendingList(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    ULONG                  CurrentFrame
    );

BOOLEAN
FASTCALL
OHCD_fProcessAbortPendingList(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    ULONG                  CurrentFrame
    );

BOOLEAN
FASTCALL
OHCD_fProcessClosePendingList(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    ULONG                  CurrentFrame
    );

VOID
FASTCALL
OHCD_fCleanEndpointTDs(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN POHCD_ENDPOINT           Endpoint
    );

//----------------------------------------------------------------------------
// Implementation of function called from other modules:
//      OHCD_InterruptService
//      OHCD_IsrDpc
//      OHCD_Get32BitFrameNumber
//----------------------------------------------------------------------------
BOOLEAN 
OHCD_InterruptService( 
    IN PKINTERRUPT Interrupt, 
    IN PVOID ServiceContext 
    )
{
    POHCD_DEVICE_EXTENSION      deviceExtension = (POHCD_DEVICE_EXTENSION)ServiceContext;
    ULONG                       interruptsSignaled;
    ULONG                       interruptEnable;
    POHCI_OPERATIONAL_REGISTERS HcRegs = deviceExtension->OperationalRegisters;
        
    IsrEntryCount++;
    //
    //  Get the interrupts which are actually signaled on USB.  According to OpenHCI 7.1.5
    //  the ine bits set in HcInterruptEnable and in HcInterrupt are actually signaled.
    //
    //
    interruptEnable = READ_REGISTER_ULONG(&HcRegs->HcInterruptEnable);
    interruptsSignaled = interruptEnable &  READ_REGISTER_ULONG(&HcRegs->HcInterruptStatus);

    //
    //  If an enabled interrupt was not signaled, or if the master interrupt
    //  is off this interrupt couldn't have been for us.
    //
    if( (0 == interruptsSignaled) || !(interruptEnable&HCINT_MasterInterruptEnable) ) return FALSE;

    //
    // Disable Interrupts on Host Controller
    //
    WRITE_REGISTER_ULONG(&HcRegs->HcInterruptDisable, HCINT_MasterInterruptEnable);

    //**
    //**    In sequence check and handle the following conditions:
    //**
    //**    SchedulingOverrun
    //**    FrameNumberOverflow
    //**
    //**    The following conditions are handled only in the DPC:
    //**
    //**    WritebackDoneHead
    //**    StartOfFrame
    //**    UnrecoverableError
    //**    RootHubStatusChange
    //**

    //
    // Today we ack but otherwise ignore a SchedulingOverrun interrupt.
    // In the future we may want to do something as suggested in section
    // 5.2.10.1 of the OpenHCI specification.
    //
    if (interruptsSignaled & HCINT_SchedulingOverrun)
    {
        WRITE_REGISTER_ULONG(&HcRegs->HcInterruptStatus, HCINT_SchedulingOverrun);
        //
        //  record that we handled the interrupt
        //
        interruptsSignaled &= ~HCINT_SchedulingOverrun;
    }

    //
    // Check for Frame Number Overflow The following insures that the 32 bit
    // frame never runs backward.
    //
    if (interruptsSignaled & HCINT_FrameNumberOverflow)
    {
        //
        //  Fix up our high part.
        //
        deviceExtension->FrameHighPart += 0x10000 -
                    (0x8000 & (deviceExtension->HCCA->FrameNumber ^ deviceExtension->FrameHighPart));
        //
        //  Clear the status flag, we handled it.
        //
        WRITE_REGISTER_ULONG(&HcRegs->HcInterruptStatus, HCINT_FrameNumberOverflow);

        //
        //  record that we handled the interrupt
        //
        interruptsSignaled &= ~HCINT_FrameNumberOverflow;
    };
    
    //
    //  If there is something left to do,
    //  queue a DPC item, let the DPC reenable interrupts
    //  when it is done.
    //
    if(interruptsSignaled)
    {
        deviceExtension->IsrDpc_Context.InterruptsSignaled  = interruptsSignaled;
        KeInsertQueueDpc(&deviceExtension->IsrDpc, NULL, NULL);
    }
    else
    {
        //
        // If we didn't queue anything than reenable interrupts.
        //
        WRITE_REGISTER_ULONG(&deviceExtension->OperationalRegisters->HcInterruptEnable, HCINT_MasterInterruptEnable);
    }
    return TRUE;
}

VOID
OHCD_IsrDpc(
    PKDPC Dpc,
    PVOID DeviceExtensionPtr,
    PVOID Bogus1,
    PVOID Bogus2
    )
/*++

Routine Description:

  The OHCD_InterruptService will have queued the DPC, if any of the following
  conditions occured:

    WritebackDoneHead
    StartOfFrame
    UnrecoverableError
    RootHubStatusChange

  We handled any and all of these conditions which are present, and then
  reenable interrupts for the host controller.

Arguments:

    Dpc                 - pointer to our DPC object
    DeviceExtensionPtr  - pointer to device extension for our HC.

    Bogus1              - These are arguments that the ISR
    Bogus2              - could have supplied, but we don't need them.

Return Value:
    
    Noned

--*/               
{
    //
    //  Declare and initialize a whole bunch of convenience variables.
    //
    POHCD_DEVICE_EXTENSION          deviceExtension = (POHCD_DEVICE_EXTENSION)DeviceExtensionPtr;
    POHCI_OPERATIONAL_REGISTERS     hcRegs = deviceExtension->OperationalRegisters;
    ULONG                           interruptsSignaled = deviceExtension->IsrDpc_Context.InterruptsSignaled;
    ULONG                           doneHead;
    POHCD_TRANSFER_DESCRIPTOR       completedTDs = NULL;
    POHCD_TRANSFER_DESCRIPTOR       tempTD = 0;
    BOOLEAN                         fItemsStillPending = FALSE;
    ULONG                           currentFrame;
    
    USB_DBG_ENTRY_PRINT(("Entering OHCD_IsrDpc"));

    //
    //  The pause routine checks this to see if it
    //  is between an ISR and the DPC executing.
    //

    deviceExtension->IsrDpc_Context.InterruptsSignaled = 0;

    //
    //  Check for UnrecoverableError first, because there is no point.
    //  in doing anything else, if this has occured.
    //
    if (interruptsSignaled & HCINT_UnrecoverableError)
    {
        ASSERT(FALSE && "Wow, what do we do here?  Try reseting USB?  Reset the whole box?");
    }

    // 
    // Check for WritebackDoneHead, to see if we have TDs on the done list
    //
    if(interruptsSignaled & HCINT_WritebackDoneHead)
    {
        USB_DBG_TRACE_PRINT(("HCINT_WritebackDoneHead is set: processing . . ."));
        doneHead = deviceExtension->HCCA->DoneHead;
        doneHead &= 0xFFFFFFF0;    //LSb's can be used to indicate other interrupt conditions, get rid of them
        deviceExtension->HCCA->DoneHead = 0;

#ifdef SILVER
        //
        //  If the done head was lost, try to find it.
        //
        if(0==doneHead)
        {
            doneHead = OHCD_fPoolFindLostDoneHead(deviceExtension);
        }
#else 
        ASSERT(doneHead && "Xbox hardware should not have this bug");
#endif
        //
        //  The Done Queue can be treated as a LIFO.  Furthermore, they are linked only
        //  by physical address pointers.  So in one fell swoop we:
        //      1) dequeue them from the head
        //      2) reverse the links
        //      3) switch the physical addresses to virtual addresses.
        //
        while(doneHead)
        {
            //
            //  Get a virtual pointer to a TD
            //
            tempTD = OHCD_PoolTDFromPhysicalAddress(doneHead);
            ASSERT(tempTD);

            //
            //  Dequeue TD  from Host Controller Queue
            //
            doneHead = tempTD->HcTransferDescriptor.NextTD;

            //
            //  Queue TD at head our CompletedTDs list
            //
            tempTD->HcTransferDescriptor.NextTDVirtual = (POHCI_TRANSFER_DESCRIPTOR)completedTDs;
            completedTDs = tempTD;
        }
        //
        //  Now step through each TD in the order of completion
        //  and deal with it.
        //
        while(completedTDs)
        {
            //
            //  Dequeue a TD off the head
            //
            tempTD = completedTDs;
            completedTDs = (POHCD_TRANSFER_DESCRIPTOR)tempTD->HcTransferDescriptor.NextTDVirtual;
            //
            //  Process the TD
            //
            if(OHCD_IS_ISOCH_TD(tempTD))
            {
                OHCD_PROCESS_ISOCHRONOUS_TD(deviceExtension, tempTD);
            } else
            {
                OHCD_fProcessDoneTD(deviceExtension, tempTD);
            }
        }
        //
        //  Tell the Host Controller we are done
        //
        WRITE_REGISTER_ULONG(&hcRegs->HcInterruptStatus, HCINT_WritebackDoneHead);
        interruptsSignaled &= ~HCINT_WritebackDoneHead;
        //
        //  This shouldn't be necessary! However, in some cases we added to the schedule while interrupts were
        //  still disabled and the operation completed before this DPC routine finished.  The interrupt seems to
        //  be lost in these cases.  Hitting these flags after restoring the interrupt gets things going again.
        //  I would call this a hardware bug.  Not worth complaining about though, this fix is pretty straightforward.
        //
        WRITE_REGISTER_ULONG((PULONG)&deviceExtension->OperationalRegisters->HcCommandStatus, HCCS_ControlListFilled | HCCS_BulkListFilled);
    }

    // 
    // Check for StartOfFrame
    //
    if (interruptsSignaled & HCINT_StartOfFrame)
    {
        //
        //  Why did we ask for a SOF?  Probably to guarantee an interrupt so
        //  that we could deal with closing endpoints, or canceling requests
        //
        
        //
        //  We treat SOF interrupts as one time events, so shut them
        //  back off.
        WRITE_REGISTER_ULONG(&hcRegs->HcInterruptStatus, HCINT_StartOfFrame);
        WRITE_REGISTER_ULONG(&hcRegs->HcInterruptDisable, HCINT_StartOfFrame);
        interruptsSignaled &= ~HCINT_StartOfFrame;
    }

    //
    //  Handle pending cancels
    //
    currentFrame = OHCD_Get32BitFrameNumber(deviceExtension);
    if(OHCD_fProcessCancelPendingList(deviceExtension, currentFrame))
    {
        fItemsStillPending = TRUE;
    }

    //
    //  Handle pending aborts
    //
    //  IT IS IMPORTANT TO HANDLE PENDING ABORTS BEFORE PENDING
    //  CLOSES, SO THAT WE DO NOT CLOSE AN ENDPOINT WITH AN ABORT
    //  PENDING.
    //
    if(OHCD_fProcessAbortPendingList(deviceExtension, currentFrame))
    {
        fItemsStillPending = TRUE;
    }

    //
    //  Handle pending closes
    //
    if(OHCD_fProcessClosePendingList(deviceExtension, currentFrame))
    {
        fItemsStillPending = TRUE;
    }

    //
    //  If there anything (cancels, aborts, or closes)
    //  are still pending, then set an interrupt at
    //  the next start of frame.
    //
    if(fItemsStillPending)
    {
        USB_DBG_TRACE_PRINT(("Items still pending setting an SOF interrupt."));
        WRITE_REGISTER_ULONG(&deviceExtension->OperationalRegisters->HcInterruptStatus, HCINT_StartOfFrame);
        WRITE_REGISTER_ULONG(&deviceExtension->OperationalRegisters->HcInterruptEnable, HCINT_StartOfFrame);
    }

    // 
    // Process Root Hub changes 
    //
    if(interruptsSignaled & HCINT_RootHubStatusChange)
    {

        USB_DBG_TRACE_PRINT(("Root hub interrupt signaled"));
        OHCD_RootHubProcessInterrupt(deviceExtension);
        
        WRITE_REGISTER_ULONG(&hcRegs->HcInterruptStatus, HCINT_RootHubStatusChange);                                  
        interruptsSignaled &= ~HCINT_RootHubStatusChange;
    }
    
    //
    //  If some interrupt occured and we haven't
    //  handled it, we obviously don't care about it, so disable
    //  it.
    if(interruptsSignaled)
    {
        ASSERT(FALSE && "Not expecting interrupts that we don't handle.");
        WRITE_REGISTER_ULONG(&hcRegs->HcInterruptStatus, interruptsSignaled);
        interruptsSignaled = 0;
    }

    //
    //  Turn interrupts back on.
    //
    WRITE_REGISTER_ULONG(&hcRegs->HcInterruptEnable, HCINT_MasterInterruptEnable);

    USB_DBG_EXIT_PRINT(("Exiting OHCD_IsrDpc"));
}

ULONG
FASTCALL
OHCD_Get32BitFrameNumber(
    POHCD_DEVICE_EXTENSION DeviceExtension
    )
{
    ULONG hp, fn, n;
    /* This code accounts for the fact that HccaFrameNumber is updated by the
     * HC before the HCD gets an interrupt that will adjust FrameHighPart. No
     * synchronization is necessary due to great cleaverness. */
    hp = DeviceExtension->FrameHighPart;
    fn = DeviceExtension->HCCA->FrameNumber;
    n = ((fn & 0x7FFF) | hp) + ((fn ^ hp) & 0x8000);
    return n;
}

//----------------------------------------------------------------------------
// Implementation of functions used only locally:
//      OHCD_ProcessDoneTD
//      OHCD_ProcessFailedTD
//      OHCD_ProcessCancelPendingList
//----------------------------------------------------------------------------

VOID
FASTCALL
OHCD_fProcessDoneTD(
    POHCD_DEVICE_EXTENSION      DeviceExtension,
    POHCD_TRANSFER_DESCRIPTOR   TD
    )
/*++

Routine Description:

    Called from the IsrDpc to cleanup each TD that was
    in the done queue.  There are a large number of cases.

Arguments:

    DeviceExtension - pointer to device extension for our HC.
    TD              - Transfer Descriptor that completed.
    

Return Value:
    
    None

--*/               
{
    PURB                        urb = TD->Urb;
    POHCD_ENDPOINT              endpoint = TD->Endpoint;
    POHCD_TRANSFER_DESCRIPTOR   setupBlock;
    BOOLEAN                     isLastTD;
    USB_DBG_ENTRY_PRINT(("Entering OHCD_ProcessDoneTD"));
    
    //
    //  Success or failure, if this is a setup TD, we need to free the
    //  setupBlock.
    //
    if(OHCD_TD_TYPE_SETUP == TD->Type)
    {
        USB_DBG_TRACE_PRINT(("TD is a setup TD, so free the descriptor used for the data payload."));
        //
        //  Clean up the setup packet which we stored in a descriptor block.
        //  Get the virtual address of the setup packet (BufferEnd points
        //  to the physical address of the last byte of the 8 byte setup
        //  packet).  
        //
        setupBlock = OHCD_PoolTDFromPhysicalAddress(TD->HcTransferDescriptor.BufferEnd - 7);
        //
        //  With Virtual address in hand we can free it.
        //
        OHCD_PoolFreeTD(setupBlock);
        OHCD_PoolCreditControlTDQuota(1);
    }

    //
    //  If this TD completed with an error, than
    //  let OHCD_ProcessFailedTD take over.
    //
    if(OHCI_CC_NO_ERROR != TD->HcTransferDescriptor.ConditionCode)
    {
        USB_DBG_TRACE_PRINT(("TD @0x%0.8x had an error. CC = %x", TD, TD->HcTransferDescriptor.ConditionCode));   
        ASSERT(TD->Type != OHCD_TD_TYPE_DUMMY);
        
        OHCD_fProcessFailedTD(DeviceExtension, TD);
        return;
    }

    //
    //  Calculate bytes transfered.
    //
    if(TD->HcTransferDescriptor.CurrentBufferPointer)
    {
        //
        //  We do everything signed to account for page wrap
        //
        LONG currentBufferPointerOffset = (LONG)BYTE_OFFSET(TD->HcTransferDescriptor.CurrentBufferPointer);
        LONG endBufferPointerOffset = (LONG)BYTE_OFFSET(TD->HcTransferDescriptor.BufferEnd);
        LONG bytesTransfered;
        if(endBufferPointerOffset >= currentBufferPointerOffset)
        {
            bytesTransfered = TD->Bytes - (endBufferPointerOffset - currentBufferPointerOffset);
        } else
        {
            bytesTransfered = TD->Bytes - (PAGE_SIZE - (currentBufferPointerOffset - endBufferPointerOffset));
        }
        bytesTransfered--;
        ASSERT(bytesTransfered >= 0);
        urb->CommonTransfer.TransferBufferLength += bytesTransfered;
    } else
    {
        urb->CommonTransfer.TransferBufferLength += TD->Bytes;
    }
    //
    //  Record whether this was the last TD in an URB
    //
    isLastTD = TD->Flags & OHCD_TD_FLAG_LAST_TD;

    //
    //  Free the TD
    //
    OHCD_PoolFreeTD(TD);

    //
    //  Credit the proper quota
    //
    OHCD_fCreditTDQuota(DeviceExtension, endpoint);
    
    //
    //  If that TD completed the URB then we need to process
    //  the URB
    //
    if(isLastTD)
    {
        USB_DBG_TRACE_PRINT(("TD was last in an URB."));

        urb->Header.Status = USBD_STATUS_SUCCESS;
        OHCD_fCompleteRequest(DeviceExtension, endpoint, urb);
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_ProcessDoneTD"));
    //
    //  Nothing else to do
    //
    return;
}


VOID
FASTCALL
OHCD_fProcessFailedTD(
    POHCD_DEVICE_EXTENSION      DeviceExtension,
    POHCD_TRANSFER_DESCRIPTOR   TD
    )
/*++

Routine Description:

    Called from the OHCD_ProcessDoneTD (in the context of IsrDPC),
    because the condition code indicated an error.  Not called if
    TD is for an isochronous endpoint.

    Our job is to deal with that condition.  Firstly, the
    associated endpoint has definately been halted, so we
    can party on the endpoint. In most cases we do
    the following:
        1) Copy the error condition into the URB's status field.
        2) Clean up all the TD's associated with that URB.
        5) Complete the URB.

    There is one exceptions for DataUnderrun when ShortXferOK. There
    is a slight difference in the way control endpoints are handled.
    For Bulk and  Interrupt endpoints we proceed as follows:
        1) Clean up all the remaining TD's associated with the URB.
        2) Complete the URB successfully.
    
    For control endpoints we do the following:
        1) Clean up all the remaining TD's associated with the URB, except
        the status TD.

    When we are done the endpoint should be ready to go again.  The
    caller must clear the halted bit though.
        
Arguments:

    DeviceExtension - pointer to device extension for our HC.
    TD              - Transfer Descriptor that completed.

Return Value:
    
    None

--*/               
{
    PURB                        urb = TD->Urb;
    POHCD_ENDPOINT              endpoint = TD->Endpoint;
    BOOLEAN                     errorCondition = TRUE;
    BOOLEAN                     isLastTD;
    ULONG                       tempTDPhysical;
    POHCD_TRANSFER_DESCRIPTOR   tempTD;
    
    ASSERT(OHCI_CC_NO_ERROR != TD->HcTransferDescriptor.ConditionCode);

    //
    //  DataUnderrun when USBD_SHORT_TRANSFER_OK, is not an error
    //  mark it as success, and record the bytes that were transfered
    //
    if(
        (OHCI_CC_DATA_UNDERRUN == TD->HcTransferDescriptor.ConditionCode) &&
        (urb->CommonTransfer.ShortTransferOK)
    )
    {
       if(TD->HcTransferDescriptor.CurrentBufferPointer)
       {
            //
            //  We do everything signed to account for page wrap
            //
            LONG currentBufferPointerOffset = (LONG)BYTE_OFFSET(TD->HcTransferDescriptor.CurrentBufferPointer);
            LONG endBufferPointerOffset = (LONG)BYTE_OFFSET(TD->HcTransferDescriptor.BufferEnd);
            LONG bytesTransfered;
            if(endBufferPointerOffset >= currentBufferPointerOffset)
            {
                bytesTransfered = TD->Bytes - (endBufferPointerOffset - currentBufferPointerOffset);
            } else
            {
                bytesTransfered = TD->Bytes - (PAGE_SIZE - (currentBufferPointerOffset - endBufferPointerOffset));
            }
            bytesTransfered--;
            ASSERT(bytesTransfered >= 0);
            urb->CommonTransfer.TransferBufferLength += bytesTransfered;
        } else
        {
            urb->CommonTransfer.TransferBufferLength += TD->Bytes;
        }

        USB_DBG_TRACE_PRINT(("OHCI_CC_DATA_UNDERRUN with ShortTransferOK"));
        urb->Header.Status = USBD_STATUS_SUCCESS;
        errorCondition = FALSE;
    }
    else
    //
    //  It really is an error, convert the condition code to a USBD_STATUS_XXXX code.
    //
    {
        if(OHCI_CC_NOT_ACCESSED2 == TD->HcTransferDescriptor.ConditionCode)
        {
            //
            //  TDs are initialized with OHCD_CC_DATA_NOT_ACCESSED.  If the condition
            //  code is OHCD_CC_DATA_NOT_ACCESSED2, it is because the OHCD_ProcessCancelPendingList
            //  called OHCD_ProcessDoneTD to finish off its dirty work.
            //
            urb->Header.Status = USBD_STATUS_CANCELED;
        }
        urb->Header.Status = OHCI_CC_TO_USBD_STATUS(TD->HcTransferDescriptor.ConditionCode);
        
    }
    
    tempTD = TD;
    //
    //  tempTDPhysical will be kept one TD ahead of tempTD
    //  on the queue.  This is because, we need to get the
    //  first one from the endpoints programmed queue (it hasn't
    //  failed yet, so it was not in the done queue).  Subsequent
    //  tempTDPhysical values are obtained in the loop, by walking the
    //  queue.
    //
    tempTDPhysical = READ_HEADP(&endpoint->HcEndpointDescriptor);
    //
    //  Loop until all the TDs of the failed URB are freed.
    //
    USB_DBG_TRACE_PRINT(("Freeing TDs from failed Transfer:"));
    do
    {
        //
        //  Before freeing the TD, record whether or
        //  not it is the last one in the URB.
        //
        isLastTD = tempTD->Flags & OHCD_TD_FLAG_LAST_TD;

        //
        //  Free the TD
        //
        USB_DBG_TRACE_PRINT(("0x%0.8x", tempTD));
        OHCD_PoolFreeTD(tempTD);

        //
        //  Credit the proper quota
        //
        OHCD_fCreditTDQuota(DeviceExtension, endpoint);

        //
        //  update tempTD for the next iteration.
        //
        tempTD = OHCD_PoolTDFromPhysicalAddress(tempTDPhysical);
        tempTDPhysical = tempTD->HcTransferDescriptor.NextTD;
        
        //
        //  If tempTD is a OHCD_TD_TYPE_STATUS and we
        //  did not really fail, than we should break out
        //  before freeing it.
        //
        if( (OHCD_TD_TYPE_STATUS == tempTD->Type) && !errorCondition)
        {
            USB_DBG_TRACE_PRINT(("Leaving Status TD (0x%0.8x) on ED", tempTD));
            break;
        }
    } while(!isLastTD);
        
    //
    //  Update the Queue to reflect that we freed all of them TDs
    //  (remember that tempTDPhysical was one TD ahead of the last
    //  one we didn't free)
    tempTDPhysical = tempTD->PhysicalAddress;
    WRITE_HEADP(&endpoint->HcEndpointDescriptor, tempTDPhysical);
    
    //
    // If we cleaned up a whole URB (i.e. it was not
    // a short control transfer) then complete the Urb.
    //
    if(isLastTD)
    {
        OHCD_fCompleteRequest(DeviceExtension, endpoint, urb);
    }

    //
    //  For control endpoints and short transfers, automatically clear the
    //  halted condition.
    //
    if(USB_ENDPOINT_TYPE_CONTROL == endpoint->EndpointType || !errorCondition)
    {
        CLEAR_HALTED(&endpoint->HcEndpointDescriptor);
    }
}


VOID
FASTCALL
OHCD_fCompleteRequest(
    IN POHCD_DEVICE_EXTENSION DeviceExtension, 
    IN POHCD_ENDPOINT Endpoint,
    IN PURB Urb
    )
/*++
  Routine Description:
    Helper method for OHCD_fProcessDoneTD and OHCD_fProcessFailedTD.
    It just pulls some duplicate code out, and puts it in one place.

   Actions Performed:
    
    1) Clean up Urb out of endpoint.
    2) If the Urb is pending cancel, do maintenance
       on the pending cancel list and endpoint.
    3) Mark Urb Completed.
    4) Complete URb.

    It does not set the Urb status, the caller does that.
--*/
{
    Endpoint->ProgrammedUrbCount--;
    
    //
    //  Unlock buffer pages if needed
    //
    if(Urb->CommonTransfer.TransferBuffer)
    {
        MmLockUnlockBufferPages(Urb->CommonTransfer.TransferBuffer, Urb->CommonTransfer.Hca.HcdOriginalLength, TRUE);
        #ifdef DVTSNOOPBUG
        OHCD_PoolEndDoubleBufferTransfer(Urb);
        #endif
    }

    //
    //  If OHCD_URB_FLAG_CANCELED is set, it may mean
    //  that the URB is sitting in our device extension's
    //  pending cancel list.  If so we have some work to do.
    //
    if(OHCD_URB_FLAG_CANCELED&Urb->CommonTransfer.Hca.HcdUrbFlags)
    {
        //
        //  Search the pending cancel list for the Urb.
        //
        PURB urbPrev=NULL;
        PURB urbCur=DeviceExtension->PendingCancels;
        while(urbCur && Urb!=urbCur)
        {
            urbPrev = urbCur;
            urbCur = urbPrev->CommonTransfer.Hca.HcdUrbLink;
        }

        //
        // If we find it, better clean up.
        //
        if(urbCur)
        {
            ASSERT(urbCur==Urb);

            //
            //  Splice the Urb out of the pending cancel list.
            //
            if(!urbPrev)
            {
                DeviceExtension->PendingCancels = urbCur->CommonTransfer.Hca.HcdUrbLink;
            } else
            {
                urbPrev->CommonTransfer.Hca.HcdUrbLink = urbCur->CommonTransfer.Hca.HcdUrbLink;
            }
            urbCur->CommonTransfer.Hca.HcdUrbLink = NULL;
    
            //
            //  Handle the endpoint pause associated with the Urb pending cancel.
            //
            if( 0 == --Endpoint->PendingPauseCount)
            {   
                Endpoint->Flags &= ~OHCD_ENDPOINT_FLAG_PAUSING;
                Endpoint->HcEndpointDescriptor.Control.Skip = 0;
            }

        } else
        //
        //  There is only one legitimate reason that the Urb would
        //  not be in the pending cancel list.  That is if 
        //  OHCD_fProcessCancelPendingList is in our call stack.  If that
        //  is the case Urb->Header.Status  will already be set to
        //  USBD_STATUS_CANCELED.
        //
        {
            ASSERT(USBD_STATUS_CANCELED == Urb->Header.Status);
        }
    }
    
    //
    //  Complete the request.
    //

    Urb->CommonTransfer.Hca.HcdUrbFlags |= OHCD_URB_FLAG_COMPLETED;
    USBD_CompleteRequest(Urb);
}

VOID
FASTCALL
OHCD_fCreditTDQuota(
    IN POHCD_DEVICE_EXTENSION DeviceExtension, 
    IN POHCD_ENDPOINT Endpoint
    )
/*++
    Routine Description:
        Called whenever a TD is retired.  It credits the proper
        quota (control, bulk or interrupt), and jogs the appropriate
        queue that may be waiting to fit under the quota.

   DeviceExtension - pointer to device extension for our HC.
   Endpoint - endpoint the TD was associated with.

--*/
{
    switch(Endpoint->EndpointType)
    {
        case USB_ENDPOINT_TYPE_CONTROL:
            OHCD_PoolCreditControlTDQuota(1);
            OHCD_fProgramControlTransfer(DeviceExtension);
            break;
        case USB_ENDPOINT_TYPE_BULK:
            OHCD_PoolCreditBulkTDQuota(1);
            OHCD_fProgramBulkTransfer(DeviceExtension);
            break;
        case USB_ENDPOINT_TYPE_INTERRUPT:
            Endpoint->TDInUseCount--;
            OHCD_fProgramInterruptTransfer(DeviceExtension, Endpoint);
            break;
		default:
            //ISOCH endpoint never expected here.
            USB_DBG_ERROR_PRINT(("Unrecognized endpoint type in OHCD_ProcessDoneTD"));
            //This is a serious error, please dump the stack, bug it, and reboot.
    }
}

BOOLEAN
FASTCALL
OHCD_fProcessCancelPendingList(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    ULONG                  CurrentFrame
    )
/*++

Routine Description:

    Called from the IsrDpc to cleanup any pending cancels.
    The DPC has to do it, because the endpoint must be paused.

Arguments:

    DeviceExtension - pointer to device extension for our HC.

    Return Value:
    
    None

--*/               
{
    PURB            stillPending = NULL;
    PURB            urb;
    POHCD_ENDPOINT  endpoint;

    ASSERT_DISPATCH_LEVEL();
    //
    //  Pop the first URB in each request pending cancel off
    //  the Pending Cancel queue and deal with it.  Note that
    //  we have an inner loop to deal with URBs of the same request.
    //
    while(DeviceExtension->PendingCancels)
    {
        //
        //  pop an urb off the pending cancels queue
        //
        urb = DeviceExtension->PendingCancels;
        DeviceExtension->PendingCancels = urb->CommonTransfer.Hca.HcdUrbLink;

        //
        //  Get the endpoint for the urb
        //
        endpoint = (POHCD_ENDPOINT)urb->CommonTransfer.EndpointHandle;
        
        //
        //  If the endpoint is not paused yet, hold the urb aside on
        //  the stillPending list.
        //
        if(CurrentFrame < endpoint->PauseFrame)
        {
            urb->CommonTransfer.Hca.HcdUrbLink = stillPending;
            stillPending = urb;
        } else if(endpoint->Flags&OHCD_ENDPOINT_FLAG_DELAY_PAUSE)
        {
            // Change the pause frame so that OHCD_fProcessAbortPendingList
            // and OHCD_fProcessClosePendingList won't process anything for
            // this endpoint until the next interrupt.
            endpoint->PauseFrame = CurrentFrame+1; 
            // Clear the delay pause flag.
            endpoint->Flags &= ~OHCD_ENDPOINT_FLAG_DELAY_PAUSE;
            // Put the Urb on the still pending list
            urb->CommonTransfer.Hca.HcdUrbLink = stillPending;
            stillPending = urb;
        } else   
        {
            //
            //  The endpoint is paused,  we need to cleanup all the TDs associated with
            //  the Urb and complete it.  We can leverage the OHCD_ProcessDoneTD routine,
            //  here's how:
            //
            //  1) Find our URB on the endpoints queue.
            //  2) Save physical pointers to the original first TD, and the
            //     last TD prior to the first TD of our URB.
            //  3) Set the head of the queue to point the second TD of our URB.
            //  4) Set the condition code of the first TD is set to
            //     OHCI_CC_NOT_ACCESSED2.
            //  5) Call OHCD_ProcessDoneTD.
            //  6) Restore the portion of the queue prior to our URB
            //
            
            ULONG                       firstTDPhysical;
            ULONG                       priorTDPhysical;
            ULONG                       currentTDPhysical;
            POHCD_TRANSFER_DESCRIPTOR   currentTD;

            //
            //  ASSERT that the Urb is not yet completed.
            //
            ASSERT(!(urb->CommonTransfer.Hca.HcdUrbFlags & OHCD_URB_FLAG_COMPLETED));

            //
            //  Loop until we find the first TD of our URB, or until we found the end
            //  of the queue.
            //  Steps 1) and 2)
            //
            priorTDPhysical = 0;
            firstTDPhysical =
            currentTDPhysical = READ_HEADP(&endpoint->HcEndpointDescriptor);
            currentTD = OHCD_PoolTDFromPhysicalAddress(currentTDPhysical);
            while(
                (urb != currentTD->Urb) &&
                (currentTDPhysical != endpoint->HcEndpointDescriptor.TailP)
            )
            {
                priorTDPhysical = currentTDPhysical;
                currentTDPhysical = currentTD->HcTransferDescriptor.NextTD;
                currentTD = OHCD_PoolTDFromPhysicalAddress(currentTDPhysical);
            }
            //
            //  ASSERT that we found the Urb.  Basically, we should
            //  always find it.  If we didn't, it got lost, and that
            //  would be very bad.
            //
            ASSERT(urb == currentTD->Urb);
            
            //
            //  3) Adjusting queue for OHCD_ProcessDoneTD
            //
            WRITE_HEADP(&endpoint->HcEndpointDescriptor, currentTD->HcTransferDescriptor.NextTD);
            //
            //  4) Verify that the condition code of the first TD is set to
            //     OHCI_CC_NOT_ACCESSED2.
            currentTD->HcTransferDescriptor.ConditionCode = OHCI_CC_NOT_ACCESSED2;
            //
            //  5) Call OHCD_ProcessDoneTD.
            //
            OHCD_fProcessDoneTD(DeviceExtension, currentTD);
            //
            //  6) Restore endpoint's queue
            //
            if(0 != priorTDPhysical)
            {
                currentTD = OHCD_PoolTDFromPhysicalAddress(priorTDPhysical);
                currentTD->HcTransferDescriptor.NextTD = READ_HEADP(&endpoint->HcEndpointDescriptor);
                WRITE_HEADP(&endpoint->HcEndpointDescriptor, firstTDPhysical);
            }
            //
            //  Decrement pending pause count, and restart the endpoint
            //  if it is time.
            //
            if( 0 == --endpoint->PendingPauseCount)
            {   
                endpoint->Flags &= ~OHCD_ENDPOINT_FLAG_PAUSING;
                endpoint->HcEndpointDescriptor.Control.Skip = 0;
            }
        }
    }//Loop over all URBs pending cancel
        
    //
    //  Put all the cancels we passed over back on the
    //  pending list.
    //
    DeviceExtension->PendingCancels = stillPending;
    return stillPending ? TRUE : FALSE;
}

BOOLEAN
FASTCALL
OHCD_fProcessAbortPendingList(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    ULONG                  CurrentFrame
    )
/*++

Routine Description:

    Called from the IsrDpc to cleanup any pending aborts.
    The DPC has to do it, because the endpoint must be paused.

Arguments:

    DeviceExtension - pointer to device extension for our HC.

    Return Value:
    
    None

--*/               
{
    PURB            stillPending = NULL;
    PURB            urb;
    POHCD_ENDPOINT  endpoint;

    ASSERT_DISPATCH_LEVEL();
    //
    //  Pop the first URB in each request pending cancel off
    //  the Pending Cancel queue and deal with it.  Note that
    //  we have an inner loop to deal with URBs of the same request.
    //
    while(DeviceExtension->PendingAborts)
    {
        //
        //  pop an urb off the pending aborts queue
        //
        urb = DeviceExtension->PendingAborts;
        DeviceExtension->PendingAborts = urb->CommonTransfer.Hca.HcdUrbLink;

        //
        //  Get the endpoint for the urb
        //
        endpoint = (POHCD_ENDPOINT)urb->CommonTransfer.EndpointHandle;
        
        //
        //  If the endpoint is not paused yet, hold the urb aside on
        //  the stillPending list.
        if(CurrentFrame < endpoint->PauseFrame)
        {
            urb->AbortEndpoint.HcdNextAbort = stillPending;
            stillPending = urb;
        } else if(endpoint->Flags&OHCD_ENDPOINT_FLAG_DELAY_PAUSE)
        {
            // Change the pause frame so that OHCD_fProcessClosePendingList won't
            // process anything for this endpoint until the next interrupt.
            endpoint->PauseFrame = CurrentFrame+1; 
            // Clear the delay pause flag.
            endpoint->Flags &= ~OHCD_ENDPOINT_FLAG_DELAY_PAUSE;
            // Put the Urb on the still pending list
            urb->AbortEndpoint.HcdNextAbort = stillPending;
            stillPending = urb;
        } else
        //
        //  The endpoint is paused.
        //
        {
            OHCD_fCleanEndpointTDs(DeviceExtension, endpoint);
            //
            //  Decrement PendingPauseCount, and restart endpoint if necessary.
            //
            if(0 == --endpoint->PendingPauseCount)
            {   
                endpoint->Flags &= ~OHCD_ENDPOINT_FLAG_PAUSING;
                endpoint->HcEndpointDescriptor.Control.Skip = 0;
            }
            //
            //  Complete the Abort URB
            //
            urb->Header.Status = USBD_STATUS_SUCCESS;
            USBD_CompleteRequest(urb);
        }
    }//Loop over all URBs pending cancel
        
    //
    //  Put all the cancels we passed over back on the
    //  pending list.
    //
    DeviceExtension->PendingAborts = stillPending;
    return stillPending ? TRUE : FALSE;
}

BOOLEAN
FASTCALL    
OHCD_fProcessClosePendingList(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    ULONG                     CurrentFrame
    )
{
    PURB_CLOSE_ENDPOINT urb;
    PURB_CLOSE_ENDPOINT notClosedYet = NULL;
    POHCD_ENDPOINT      endpoint;
    
    while(DeviceExtension->PendingCloses)
    {
        //
        // Remove an endpoint from the queue.
        //
        urb = &DeviceExtension->PendingCloses->CloseEndpoint;
        DeviceExtension->PendingCloses = urb->HcdNextClose;
        
        //
        //  Retrieve the endpoint from the URB
        //
        endpoint = (POHCD_ENDPOINT)urb->EndpointHandle;

        //
        //  If the endpoint is not paused yet, hold the endpoint aside on
        //  the notClosedYet list.
        //
        if(CurrentFrame < endpoint->PauseFrame)
        {
            urb->HcdNextClose = (PURB)notClosedYet;
            notClosedYet = urb;
        } else if(endpoint->Flags&OHCD_ENDPOINT_FLAG_DELAY_PAUSE)
        {
            // Clear the delay pause flag.
            endpoint->Flags &= ~OHCD_ENDPOINT_FLAG_DELAY_PAUSE;
            // Put the Urb on the still pending list
            urb->HcdNextClose = (PURB)notClosedYet;
            notClosedYet = urb;
        }
        //
        //  Otherwise, cleanup the endpoint and complete the URB
        //
        else
        {
            if(OHCD_IS_ISOCH_CLOSE(urb))
            {
                OHCD_ISOCH_COMPLETE_CLOSE_ENDPOINT(DeviceExtension, urb);
            } else
            {
                //
                //  Cleanup any straggling TDS, canceling any pending transfers
                //  in the process.
                //
                OHCD_fCleanEndpointTDs(DeviceExtension, endpoint);

                //
                //  Serious programming error if we haven't cleaned everything.
                //
                ASSERT(0==endpoint->QueuedUrbCount);
                ASSERT(0==endpoint->ProgrammedUrbCount);
                ASSERT(0==--endpoint->PendingPauseCount);
                //
                //  Update the data toggle bits in the close URB
                //
                if(urb->DataToggleBits)
                {
                    ULONG dataToggleMask = 1 << endpoint->HcEndpointDescriptor.Control.EndpointAddress;
                    if(endpoint->HcEndpointDescriptor.Control.Direction == 2)
                    {
                        dataToggleMask <<= 16;
                    }
                    if(GET_TOGGLECARRY(&endpoint->HcEndpointDescriptor))
                    {
                        *urb->DataToggleBits |= dataToggleMask;
                    } else
                    {
                        *urb->DataToggleBits &= ~dataToggleMask;
                    }
                }
                OHCD_PoolFreeEndpoint(endpoint);
                urb->Hdr.Status = USBD_STATUS_SUCCESS;
                USBD_CompleteRequest((PURB)urb);
            }
        }
    }
    //
    //  Place the endpoints that weren't closed yet, back on
    //  the PendingCloses list.  So what if we reversed the order.
    //
    DeviceExtension->PendingCloses = (PURB)notClosedYet;
    return notClosedYet ? TRUE : FALSE;
}

VOID
FASTCALL
OHCD_fCleanEndpointTDs(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN POHCD_ENDPOINT           Endpoint
    )
{
    ULONG                       TDPhysical;
    POHCD_TRANSFER_DESCRIPTOR   TD = NULL;
    BOOLEAN                     done = FALSE;
    
    //
    //  1) Pull the TDs off one at a time.
    //  2) Make sure that the ConditionCode is OHCI_CC_NOT_ACCESSED2.
    //  3) Call OHCD_ProcessDoneTD, which will call OHCD_ProcessFailedTD,
    //     which will:
    //          a) Clean all the TDs associated with the same URB.
    //          b) Complete the URB as USBD_STATUS_CANCELED
    //  4) Clean up the dummy TD when we get to it.     
    //  
    while(!done)
    {
        TDPhysical = READ_HEADP(&Endpoint->HcEndpointDescriptor);
        if(0 == TDPhysical) break;
        TD = OHCD_PoolTDFromPhysicalAddress(TDPhysical);
        if(TDPhysical != Endpoint->HcEndpointDescriptor.TailP)
        {
            ASSERT(OHCD_TD_TYPE_DUMMY != TD->Type);
            TD->HcTransferDescriptor.ConditionCode = OHCI_CC_NOT_ACCESSED2;
            WRITE_HEADP(&Endpoint->HcEndpointDescriptor, TD->HcTransferDescriptor.NextTD);
            OHCD_fProcessDoneTD(DeviceExtension, TD);
        }else
        {
            ASSERT(OHCD_TD_TYPE_DUMMY == TD->Type);
            WRITE_HEADP(&Endpoint->HcEndpointDescriptor, 0);
            Endpoint->HcEndpointDescriptor.TailP = 0;
            OHCD_PoolFreeTD(TD);
            done = TRUE;
        }
    }   
}
