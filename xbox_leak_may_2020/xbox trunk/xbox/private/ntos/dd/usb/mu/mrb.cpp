/*++

Copyright (c) 1996-2000 Microsoft Corporation

Module Name:

    mrb.cpp

Abstract:

    This source file contains the state machine for 
    MU requests blocks to the device.  The bulk-only protocol
    specifices three stages for every transfer.

    The only entry point to the MRB state machine is

    MU_fStartMrb

Environment:

    kernel mode

Revision History:

    06-12-2000 : started rewrite : georgioc
    10-20-2000 : major cleanup (separated out the MRB state machine,
                 from the partial write state machine, which is now
                 in disk.cpp : mitchd
    03-07-2001 : generalized the way write commands are handled to aid
                 the partial write state machine (in disk.cpp) support
                 more flexible block sizes : mitchd

--*/


/*++
    Description of MRB state machine changes started on 03/07/01.
    
    Even after the cleanup on 10-20-2000, there was still a special case in
    the state machine to transfer the data stage of an MRB from two buffers:
    the user's buffer, and the MU WriteBuffer used to maintain media block
    integrity.  This code peered into disk.cpp's flags to figure out when this
    was necessary, somewhat of a hack.  The problem with this code is that it
    only supports a media block size that is double the emulated sector size.  
    We now need to lift this restriction in order to support larger media.

    So this change formalizes the hack by adding supporting a new MRB flag,
    MRB_FLAGS_SPLIT_WRITE, and to new fields filled out by disk.cpp: UserStartOffset,
    and UserEndOffset.  When MRB_FLAGS_SPLIT_WRITE the transfer length, must be
    exactly one media block.  The write is performed as follows:

    1) Write UserStartOffset bytes from the beginning of the WriteBuffer.
    2) Write (UserEndOffset-UserStartOffset) bytes from the beginning of the
       user buffer.
    3) Write (MediaBlockSize-UserEndOffset) bytes from the WriteBuffer begining
       UserStartOffset bytes into the WriteBuffer.

    This process allows a write to be submitted from the WriteBuffer inserting an
    arbitrary number of bytes from the user buffer at any point.  In the process
    the MRB state machine becomes more independent of the disk.cpp layer.  The
    new code should be ever scalable to larger and larger media block sizes.

--*/

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************
#include "mu.h"

//*****************************************************************************
// Local Functions
//*****************************************************************************
DEFINE_USB_DEBUG_FUNCTIONS("MU");
VOID
FASTCALL
MU_fCbwTransfer(
    IN PMU_DEVICE_EXTENSION DeviceExtension
    );

VOID
MU_CbwCompletion (
    IN PURB   Urb,
    IN PVOID  Context
    );

VOID
FASTCALL
MU_fDataTransfer (
    PURB Urb,
    PMU_DEVICE_EXTENSION DeviceExtension
    );

VOID
MU_DataCompletion (
    IN PURB  Urb,
    IN PVOID Context
    );

VOID
FASTCALL
MU_fCswTransfer (
    IN PMU_DEVICE_EXTENSION DeviceExtension
    );

VOID
MU_CswCompletion (
    IN PURB            Urb,
    IN PVOID           Context
    );

//
//  MU_MrbTimeout is local, but declared also in mu.h, because
//  mu.cpp sets up the DPC at init time.
//

VOID
MU_MrbTimeout (
    IN PKDPC Dpc,
    IN PVOID Context,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

VOID
FASTCALL
MU_fMrbErrorHandler(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    );


VOID
MU_ResetDeviceCompletionRoutine (
    IN PURB             Urb,
    IN PVOID            Context
    );

#if DBG
VOID
FASTCALL
MU_fValidateMrb(
    IN PMU_DEVICE_EXTENSION DeviceExtension
    );
#define MU_DBG_VALIDATE_MRB(DeviceExtension) MU_fValidateMrb(DeviceExtension)
#else
#define MU_DBG_VALIDATE_MRB(DeviceExtension)
#endif

//*****************************************************************************
// Implementation
//*****************************************************************************

VOID
FASTCALL
MU_fStartMrb(
    IN PMU_DEVICE_EXTENSION DeviceExtension
    )
/*++

    Routine Description:
        Entry point for the MRB state machine.
            
        Does some sanity checks in debug, makes
        sure the device is still connected,
        and calls MU_fCbwTransfer - the first
        phase of any request.

--*/
{
    KIRQL oldIrql;

    USB_DBG_ENTRY_PRINT(("MU_fStartMrb(0x%0.8x)", DeviceExtension));

    //
    //  In Debug perform some simple tests
    //  to make sure that the MRB is well formed.
    //

    MU_DBG_VALIDATE_MRB(DeviceExtension);

    

    //
    //  Raise Irql to synchronize with removal
    //

    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Make sure that the device is still there.
    //

    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_PENDING_REMOVE|DF_REMOVED))
    {
        //
        //  Call the callback with a failure.
        //
        USB_DBG_TRACE_PRINT(("Mrb.CompletionRoutine(0x%0.8x, STATUS_DEVICE_NOT_CONNECTED)", DeviceExtension));
        DeviceExtension->Mrb.CompletionRoutine(DeviceExtension, STATUS_DEVICE_NOT_CONNECTED);
    } else
    {
        //
        //  Send the CBW
        //

        ASSERT(!TEST_FLAG(DeviceExtension->DeviceFlags, DF_ANY_URB_PENDING|DF_RESET_STEPS));
        MU_fCbwTransfer(DeviceExtension);
    }
    KeLowerIrql(oldIrql);
    return;
}

VOID
FASTCALL
MU_fCbwTransfer(
    IN PMU_DEVICE_EXTENSION DeviceExtension
    )
/*++
    Routine Description:
         Builds an URB for the CBW phase and send it.
--*/
{

    PMU_REQUEST_BLOCK       mrb;
    PCBW                    cbw;
    LARGE_INTEGER           deltaTime;

    USB_DBG_ENTRY_PRINT(("MU_CbwTransfer(DeviceExtension=0x%0.8x)", DeviceExtension));

    //
    // Get the mrb
    //

    mrb = &DeviceExtension->Mrb;

    //
    // Initialize the Command Block Wrapper
    //

    cbw = &DeviceExtension->Mrb.Cbw;
    cbw->dCBWSignature = CBW_SIGNATURE;
    cbw->dCBWTag = PtrToUlong(DeviceExtension->PendingIrp);
    cbw->dCBWDataTransferLength = mrb->TransferLength;
    cbw->bCBWFlags = (UCHAR)((mrb->Flags & MRB_FLAGS_DATA_IN) ? CBW_FLAGS_DATA_IN : CBW_FLAGS_DATA_OUT);
    cbw->bCBWLUN = 0; // Xbox supports only signle LUN
    cbw->bCDBLength = 10;  //Xbox supports on 10 byte cdb's

    //
    //  The caller filled out the CDB in the CBW
    //

    //
    // start the request timer
    //
    SET_FLAG(DeviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING);
    deltaTime.QuadPart = MRB_STANDARD_TIMEOUT * MRB_TIMEOUT_UNIT;
    KeSetTimer(&DeviceExtension->Mrb.Timer, deltaTime, &DeviceExtension->Mrb.TimeoutDpcObject);
    
    
    //
    //  Build and submit an URB for the CBW
    //
    SET_FLAG(DeviceExtension->DeviceFlags, DF_PRIMARY_URB_PENDING);
    USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
        &DeviceExtension->Urb.BulkOrInterruptTransfer,
        DeviceExtension->BulkOutEndpointHandle,
        cbw,
        sizeof(CBW),
        USB_TRANSFER_DIRECTION_OUT,
        MU_CbwCompletion,
        DeviceExtension,
        FALSE //short transfer is an error
        );

    //
    //  Don't worry about errors the completion
    //  routine will handle them.
    //
    ASSERT(DeviceExtension->MuInstance);
    DeviceExtension->MuInstance->Device->SubmitRequest(&DeviceExtension->Urb);

    USB_DBG_EXIT_PRINT(("MU_CbwTransfer returning"));

    return;
}

VOID
MU_CbwCompletion (
    IN PURB            Urb,
    IN PVOID           Context
    )
/*
    Routine Description:
        Completion routine for MU_fStartMrb.
        
        If any type of error occured (including a timeout) call MU_HandleBulkError()
        to handle it, and exit.

        On success, if a data phase is required start it, otherwise call MU_fCswTransfer.
*/
{
    PMU_DEVICE_EXTENSION    deviceExtension;
    PMU_REQUEST_BLOCK       mrb;

    USB_DBG_ENTRY_PRINT(("MU_CbwCompletion(Urb=0x%0.8x,Context=0x%0.8x)", Urb, Context));

    deviceExtension = (PMU_DEVICE_EXTENSION) Context;
    ASSERT(Urb == &deviceExtension->Urb);

    //
    //  If the URB failed (which includes being cancelled by the timer DPC) 
    //  call the error handling code.
    //

    if(USBD_ERROR(Urb->Header.Status) || 
       TEST_FLAG(deviceExtension->DeviceFlags, DF_REMOVED|DF_PENDING_REMOVE)
    )
    {
        USB_DBG_WARN_PRINT(("CBW transfer failed, usbdStatus=0x%0.8x", Urb->Header.Status));
        CLEAR_FLAG(deviceExtension->DeviceFlags, DF_PRIMARY_URB_PENDING);
        MU_fMrbErrorHandler(deviceExtension, IUsbDevice::NtStatusFromUsbdStatus(Urb->Header.Status));
        return;
    }
    
    mrb = &deviceExtension->Mrb;
    
    // The CBW Bulk Transfer was successful.  Start the next phase, either
    // the Data Bulk Transfer or CSW Bulk Transfer, and do not complete the
    // request yet.
    //

    if(mrb->DataBuffer)
    {

        ASSERT(mrb->TransferLength != 0);
        mrb->BytesSubmitted = 0;
        
        //
        // Start the first URB
        //
        
        ASSERT(TEST_FLAG(deviceExtension->DeviceFlags, DF_PRIMARY_URB_PENDING));
        MU_fDataTransfer(&deviceExtension->Urb, deviceExtension);
        
        //
        //  If there is more data, start up with the second URB
        //
        
        if(mrb->BytesSubmitted < mrb->TransferLength)
        {
            SET_FLAG(deviceExtension->DeviceFlags, DF_SECONDARY_URB_PENDING);
            MU_fDataTransfer((PURB)&deviceExtension->BulkUrbSecondary, deviceExtension);
        }

    } else
    {
        ASSERT(0 == mrb->TransferLength);
        MU_fCswTransfer(deviceExtension);
    }

    USB_DBG_EXIT_PRINT(("MU_CbwCompletion returning"));
    return;
}

VOID
FASTCALL
MU_fDataTransfer (
    PURB Urb,
    PMU_DEVICE_EXTENSION DeviceExtension
    )
/*
    Routine Description:
        Submit URB for the data stage of the MRB
        stage machine.  It uses the URB passed to it.

        Since USB guarantees that multiple requests
        on the same endpoint will complete in sequence
        we can just keep alternating them until the
        whole transfer buffer is sent.
*/
{
    PMU_REQUEST_BLOCK  mrb;
    PVOID              endpointHandle;
    PVOID              transferBuffer;
    ULONG              transferLength;
    UCHAR              direction;
    
    USB_DBG_ENTRY_PRINT(("MU_DataTransfer(Urb=0x%0.8x,DeviceExtension=0x%0.8x)", Urb, DeviceExtension));

    
    mrb = &DeviceExtension->Mrb;
    
    //
    // Bulk IN or Bulk OUT?
    //

    if ((mrb->Flags & MRB_FLAGS_UNSPECIFIED_DIRECTION) == MRB_FLAGS_DATA_IN)
    {
        endpointHandle = DeviceExtension->BulkInEndpointHandle;
        direction = USB_TRANSFER_DIRECTION_IN;
    } else {
        ASSERT((mrb->Flags & MRB_FLAGS_UNSPECIFIED_DIRECTION) == MRB_FLAGS_DATA_OUT);
        endpointHandle = DeviceExtension->BulkOutEndpointHandle;
        direction = USB_TRANSFER_DIRECTION_OUT;
    }

    //**
    //**  Calculate the transfer buffer for this URB
    //**
    if(TEST_FLAG(mrb->Flags,MRB_FLAGS_SPLIT_WRITE))
    //
    //  MRB_FLAGS_SPLIT_WRITE has three stages, handle
    //  the correct one.
    {
        ASSERT(USB_TRANSFER_DIRECTION_OUT == direction);
        if (mrb->BytesSubmitted < mrb->UserStartOffset)
        //
        //  First stage - from beginning of WriteBuffer
        //
        {
            transferBuffer = MU_DriverExtension.WriteBuffer + mrb->BytesSubmitted;

        } else if(mrb->BytesSubmitted < mrb->UserEndOffset)
        //
        //  Second stage - from User's buffer
        //
        {
            transferBuffer = mrb->DataBuffer + (mrb->BytesSubmitted - mrb->UserStartOffset);

        } else
        //
        //  Third stage - from end of WriteBuffer
        //
        {
            transferBuffer = MU_DriverExtension.WriteBuffer + mrb->UserStartOffset + (mrb->BytesSubmitted - mrb->UserEndOffset);
        }
    } else
    //
    //  The MRB_FLAGS_SPLIT_WRITE is NOT set, so just do the simple thing
    //  from the user buffer.
    {
        transferBuffer = mrb->DataBuffer + mrb->BytesSubmitted;
    }
    
    //
    //  Calculate the transfer length
    //

    transferLength = mrb->TransferLength - mrb->BytesSubmitted;
    if(transferLength > MRB_TRANSFER_SIZE)
    {
        transferLength = MRB_TRANSFER_SIZE;
    }
    mrb->BytesSubmitted += transferLength;

    //
    //  Build and submit an URB for the Transfer
    //

    USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
        &Urb->BulkOrInterruptTransfer,
        endpointHandle,
        transferBuffer,
        transferLength,
        direction,
        MU_DataCompletion,
        DeviceExtension,
        FALSE //short transfer is an error
        );

    //
    //  Extend the timeout.
    //
    LARGE_INTEGER deltaTime;
    deltaTime.QuadPart = MRB_DATA_TIMEOUT * MRB_TIMEOUT_UNIT;
    KeSetTimer(&DeviceExtension->Mrb.Timer, deltaTime, &DeviceExtension->Mrb.TimeoutDpcObject);

    //
    //  Don't worry about errors the completion
    //  routine will handle them.
    //
    ASSERT(DeviceExtension->MuInstance);
    DeviceExtension->MuInstance->Device->SubmitRequest(Urb);

    return;
}

VOID
MU_DataCompletion (
    IN PURB  Urb,
    IN PVOID Context
    )
/*++
    Routine Description:
        This is the completion routine for MU_fDataTransfer.
        
        On an error (from USB), it calls MU_HandleBulkErrors.
        Then it returns.

        If there is more data to submit it loops back to
        MU_fDataTransfer to keep the data moving.

        If there is no more data to submit, it checks to
        see if there is an other URB outstanding.  If there
        are no more URBs outstanding, then it moves on to the
        next stage by calling MU_fTransferCsw.
--*/
{
    PMU_DEVICE_EXTENSION    deviceExtension = (PMU_DEVICE_EXTENSION) Context;
    PMU_REQUEST_BLOCK       mrb;

    mrb = &deviceExtension->Mrb;

    //
    //  Check to see if there is an error pending (from the other URB)
    //

    if(TEST_FLAG(deviceExtension->DeviceFlags,DF_ERROR_PENDING))
    {
        USBD_STATUS usbdStatus;

        
        //
        //  DF_ERROR_PENDING was set only so that we get here,
        //  clear it now.
        //

        CLEAR_FLAG(deviceExtension->DeviceFlags,DF_ERROR_PENDING);

        // Grab the status from the OTHER Urb
        if(Urb == &deviceExtension->Urb)
        {
            usbdStatus = deviceExtension->BulkUrbSecondary.Hdr.Status;
        } else
        {
            usbdStatus = deviceExtension->Urb.Header.Status;
        }
        //
        //  There is definately no URB pending now.
        //  (it is more efficient to clear both flags, than to
        //  figure out which one we need to clear).
        //
        CLEAR_FLAG(deviceExtension->DeviceFlags, DF_PRIMARY_URB_PENDING|DF_SECONDARY_URB_PENDING);
        MU_fMrbErrorHandler(deviceExtension, IUsbDevice::NtStatusFromUsbdStatus(usbdStatus));
        return;
    }

    //
    //  If this URB failed, cancel the other URB, if it is outstanding,
    //  otherwise, start handling the error here.
    //

    if(USBD_ERROR(Urb->Header.Status) ||
       TEST_FLAG(deviceExtension->DeviceFlags, DF_REMOVED|DF_PENDING_REMOVE)
    )
    {
        USB_DBG_WARN_PRINT(("Data transfer failed (USB status)0x%0.8x", Urb->Header.Status));
        
        //
        //  On error, we figure out which URB we are processing and
        //  clear its pending flag.  If the URB is pending we need
        //  to cancel it, and return.
        //
        if(Urb == &deviceExtension->Urb)
        {
            CLEAR_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING);
            if(TEST_FLAG(deviceExtension->DeviceFlags,DF_SECONDARY_URB_PENDING))
            {
               ASSERT(deviceExtension->MuInstance);
               SET_FLAG(deviceExtension->DeviceFlags,DF_ERROR_PENDING);
               deviceExtension->MuInstance->Device->CancelRequest((PURB)&deviceExtension->BulkUrbSecondary);
               return;
            }
        } else
        {
            ASSERT(Urb == (PURB)&deviceExtension->BulkUrbSecondary);
            CLEAR_FLAG(deviceExtension->DeviceFlags,DF_SECONDARY_URB_PENDING);
            if(TEST_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING))
            {
                ASSERT(deviceExtension->MuInstance);
                SET_FLAG(deviceExtension->DeviceFlags,DF_ERROR_PENDING);
                deviceExtension->MuInstance->Device->CancelRequest(&deviceExtension->Urb);
                return;
            }
        }

        //
        //  If we are here there are no outstanding URBs for this device, so
        //  we can start the error handling.
        //

        MU_fMrbErrorHandler(deviceExtension, IUsbDevice::NtStatusFromUsbdStatus(Urb->Header.Status));
        return;
    }

    //
    //  Ugh, that was a lot of error handling code,
    //  but we want to be robust!
    //
    
    //
    //  If there is more data to submit
    //  then call MU_fDataTransfer,
    //  and return.
    //
    if(mrb->BytesSubmitted < mrb->TransferLength)
    {
        MU_fDataTransfer(Urb, deviceExtension);
        return;
    }

    //
    //  We could just move on to the CSW stage now,
    //  even if the other URB is still pending for
    //  a read or write.  This is acceptable, because
    //  of 3.3 of the Bulk-Only specification.  However,
    //  we do not lose measurable efficiency by only
    //  continuing if there are no more data URB
    //  outstanding.  This method reduces the number
    //  of possible error cases we need to check,
    //  and there are already too many!
    //

    if(Urb == &deviceExtension->Urb)
    {
        CLEAR_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING);
        if(TEST_FLAG(deviceExtension->DeviceFlags,DF_SECONDARY_URB_PENDING))
        {
            // Nothing to do, because our twin URB is still pending
            return;
        }
    } else
    {
        ASSERT(Urb == (PURB)&deviceExtension->BulkUrbSecondary);
        CLEAR_FLAG(deviceExtension->DeviceFlags,DF_SECONDARY_URB_PENDING);
        if(TEST_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING))
        {
            // Nothing to do, because our twin URB is still pending
            return;
        }
    }

    //
    //  Reset the primary URB flag as MU_fCswTransfer will use it.
    //

    SET_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING);
    
    //
    //  The data transfer is over, go on to CSW stage
    //
    MU_fCswTransfer(deviceExtension);

    return;
}

VOID
FASTCALL
MU_fCswTransfer (
    IN PMU_DEVICE_EXTENSION DeviceExtension
    )
/*++
    Routine Description:
        Builds and submits an URB for the CswTransfer.  It is broken
        out as a separate routineas it is called from two places
        (MU_DataCompletion and MU_CbwCompletion).
--*/
{
    USB_DBG_ENTRY_PRINT(("MU_DataTransfer(DeviceExtension=0x%0.8x)",DeviceExtension));

    USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
        &DeviceExtension->Urb.BulkOrInterruptTransfer,
        DeviceExtension->BulkInEndpointHandle,
        &DeviceExtension->Mrb.Csw,
        sizeof(CSW),
        USB_TRANSFER_DIRECTION_IN,
        MU_CswCompletion,
        DeviceExtension,
        FALSE //short transfer is an error
        );

    //
    //  Extend the timeout for the CSW.
    //
    LARGE_INTEGER deltaTime;
    deltaTime.QuadPart = ((LONGLONG)DeviceExtension->Mrb.TimeOutValue) * MRB_TIMEOUT_UNIT;
    KeSetTimer(&DeviceExtension->Mrb.Timer, deltaTime, &DeviceExtension->Mrb.TimeoutDpcObject);

    //
    //  Submit the URB, the completion routine will worry about errors.
    //
    ASSERT(DeviceExtension->MuInstance);
    DeviceExtension->MuInstance->Device->SubmitRequest(&DeviceExtension->Urb);
    
    USB_DBG_EXIT_PRINT(("MU_CswTransfer returning"));

    return;
}

VOID
MU_CswCompletion (
    IN PURB            Urb,
    IN PVOID           Context
    )
/*++
    Routine Description:
        Completion routine for the CSW transfer.

        Checks for a number of possible error conditions.
        If any are found it calls MU_HandleBulkError.

        Otherwise calls the MRBs completion routine
        with STATUS_SUCCESS.
--*/
{
    PMU_DEVICE_EXTENSION deviceExtension = (PMU_DEVICE_EXTENSION)Context;
    NTSTATUS status = STATUS_SUCCESS;

    USB_DBG_ENTRY_PRINT(("MU_CswCompletion(Urb=0x%0.8x,Context=0x%0.8x)", Urb, Context));

    //
    //  Cancel the timer
    //
    
    if(TEST_FLAG(deviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING))
    {
        KeCancelTimer(&deviceExtension->Mrb.Timer);
        CLEAR_FLAG(deviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING);
    }
    CLEAR_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING);

    if(USBD_ERROR(Urb->Header.Status))
    {
        // The Data Bulk Transfer was not successful.  Look at how the
        // the transfer failed to figure out how to recover.
        //

        USB_DBG_WARN_PRINT(("CSW transfer failed, usbdStatus=0x%0.8x", Urb->Header.Status));
        status = IUsbDevice::NtStatusFromUsbdStatus(Urb->Header.Status);
    }
    
    //
    //  Check Signature.
    //

    if(deviceExtension->Mrb.Csw.dCSWSignature != CSW_SIGNATURE)
    {
        USB_DBG_WARN_PRINT(("CSW signature incorrect."));
        status = STATUS_UNSUCCESSFUL;
    }

    //
    //  Check Tag
    //
    
    if(deviceExtension->Mrb.Csw.dCSWTag != deviceExtension->Mrb.Cbw.dCBWTag)
    {
        USB_DBG_WARN_PRINT(("CSW tag incorrect, expecting 0x%0.8x, was 0x%0.8x.", 
                                                deviceExtension->Mrb.Cbw.dCBWTag,
                                                deviceExtension->Mrb.Csw.dCSWTag
                                                ));
        status = STATUS_UNSUCCESSFUL;
    }

    //
    //  Check For Phase Error (means missed handshake in the protocol)
    //

    if(deviceExtension->Mrb.Csw.bCSWStatus == CSW_STATUS_PHASE_ERROR)
    {
        USB_DBG_WARN_PRINT(("CSW returned CSW_STATUS_PHASE_ERROR\n"));
        status = STATUS_UNSUCCESSFUL;
    }

    //
    //  Check for failed CSW.  Basically the device had trouble
    //  completing the command.  It could be that we sent an unsupported
    //  command or that we tried to write past the end of the media
    //  or some other error on our part.  OR, THE MEDIA IS BAD.
    //  On a read or write, we ASSUME that this driver did not screw up
    //  and infact a media block has failed!!!  It sucks that we don't
    //  have more specific errors, but we didn't invent the protocol,
    //  we just borrowed it from the USB mass storage committee.
    //

    if(deviceExtension->Mrb.Csw.bCSWStatus == CSW_STATUS_FAILED)
    {
        USB_DBG_WARN_PRINT(("CSW returned CSW_STATUS_FAILED\n"));
        status = STATUS_DATA_ERROR;
    }

    ASSERT(!TEST_FLAG(deviceExtension->DeviceFlags, DF_ANY_URB_PENDING));
     
    //
    //  If any error occurred, call the handle bulk error routine.
    //

    if(NT_ERROR(status))
    {
        MU_fMrbErrorHandler(deviceExtension, status);
        return;
    }
    
    ASSERT(deviceExtension->Mrb.Csw.bCSWStatus == CSW_STATUS_GOOD);

    //
    // Call the MRB completion routine with a success code
    //        
    USB_DBG_TRACE_PRINT(("Mrb.CompletionRoutine(0x%0.8x, STATUS_SUCCESS)", deviceExtension));
    deviceExtension->Mrb.CompletionRoutine(deviceExtension, STATUS_SUCCESS);
    
    USB_DBG_EXIT_PRINT(("MU_CswCompletion returning"));

    return;
}


VOID
MU_MrbTimeout (
    IN PKDPC Dpc,
    IN PVOID Context,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++
    Routine Description:
        
        This is the DPC routine for the MRB timeout timer.  The timer is set
        before the Cbw is transmitted, and canceled when the bulk-only request
        completes by receiving a Csw, or when error occurs aborting the request.

        The timer is intended to protect against devices that NAK forever.
        Device that are just extremely slow will hit this as well.  This code
        does not protect against software bugs in this driver or lower in the
        USB stack.

        All this routine does is cancel any URBs that are outstanding.  This will break
        the deadlock, and the normal error handling in the state machine will kick in.
        
    Timeout Timing:

        The timer is set at CBW, and extended at each data stage and at CSW.
        The length of the timeout for the data stages and CSW is fixed.  The caller
        in the disk layer can set the timeout for the CBW.  This is to accomodate commands
        like VERIFY that may require very long timeouts on the CBW and they don't have
        a data stage.

--*/
{
    PMU_DEVICE_EXTENSION  deviceExtension = (PMU_DEVICE_EXTENSION) Context;

    USB_DBG_WARN_PRINT(("MRB state machine timed out."));

    CLEAR_FLAG(deviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING);

    if(TEST_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING))
    {
        ASSERT(deviceExtension->MuInstance);
        deviceExtension->MuInstance->Device->CancelRequest(&deviceExtension->Urb);
    }

    if(TEST_FLAG(deviceExtension->DeviceFlags, DF_SECONDARY_URB_PENDING))
    {
        ASSERT(deviceExtension->MuInstance);
        deviceExtension->MuInstance->Device->CancelRequest((PURB)&deviceExtension->BulkUrbSecondary);
    }
}

VOID
FASTCALL
MU_fMrbErrorHandler(
    IN PMU_DEVICE_EXTENSION DeviceExtension,
    IN NTSTATUS Status
    )
/*++
    Routine Description:
        This routine is called whenever a stage of the Mrb state machine detects
        an error (USB failure, device removed, CSW problem, or timeout).  There
        should be no outstanding Urbs when this routine is called.  
        
        If the MRB timer is running, cancel it.

        If the device has been removed, this routine cleans up the Mrb state machine
        and calls the Mrb competion routine with STATUS_DEVICE_NOT_CONNECTED.

        If the device is still present, it kicks of the USB bulk-only reset
        sequence.  This a fairly gentle way to get the bulk-only transport
        protocol back in-sync between the device and this driver.
        
        See the MU_ResetDeviceCompletionRoutine description for details on
        the reset sequence.  This routine performs the first two steps,
        then second of which is asynchronous and completes in
        MU_ResetDeviceCompletionRoutine.
--*/
{
    PMU_INSTANCE muInstance = DeviceExtension->MuInstance;

    //
    //  Stop the Mrb Timer, if it is running
    //

    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING))
    {
        KeCancelTimer(&DeviceExtension->Mrb.Timer);
        CLEAR_FLAG(DeviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING);
    }

    //
    //  There should be no URB's pending when this routine is hit.
    //

    ASSERT(!TEST_FLAG(DeviceExtension->DeviceFlags, DF_ANY_URB_PENDING));
    
    //
    //  Check if the device has been removed
    //

    if(TEST_FLAG(DeviceExtension->DeviceFlags, DF_REMOVED|DF_PENDING_REMOVE))
    {
        USB_DBG_TRACE_PRINT(("Mrb.CompletionRoutine(0x%0.8x, STATUS_DEVICE_NOT_CONNECTED)", DeviceExtension));
        DeviceExtension->Mrb.CompletionRoutine(DeviceExtension, STATUS_DEVICE_NOT_CONNECTED);
        return;
    }

    ASSERT(muInstance);

    //**
    //**  Start the reset sequence,
    //**
    
    USB_DBG_TRACE_PRINT(("Initiating Bulk-Only reset sequence."));
    
    SET_FLAG(DeviceExtension->DeviceFlags, DF_RESET_STEP1);

    //
    //  stored the failed status, so that we still have it 
    //  after a reset.  Note this field is a union with 
    //  BytesSubmitted so it is not valid if we resubmit the MRB.
    //
    DeviceExtension->Mrb.FailedStatus = Status;

    //
    //  Clear the data toggle and software halt flag.  This is a
    //  synchronous USB request and should never fail.
    //

    USB_BUILD_SET_ENDPOINT_STATE(
            &DeviceExtension->Urb.GetSetEndpointState,
            DeviceExtension->BulkInEndpointHandle,
            USB_ENDPOINT_STATE_DATA_TOGGLE_RESET | USB_ENDPOINT_STATE_CLEAR_HALT
            );
    if(USBD_ERROR(muInstance->Device->SubmitRequest(&DeviceExtension->Urb)))
    {
        USB_DBG_WARN_PRINT(("MU_ResetDeviceDpc: Bulk-IN SetEndpointStatus failed!\n"));
        ASSERT(FALSE);
    }

    //
    //  Set the MRB timer to time the reset.
    //
    LARGE_INTEGER deltaTime;
    deltaTime.QuadPart = MRB_RESET_TIMEOUT*MRB_TIMEOUT_UNIT;
    SET_FLAG(DeviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING);
    SET_FLAG(DeviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING);
    KeSetTimer(&DeviceExtension->Mrb.Timer, deltaTime, &DeviceExtension->Mrb.TimeoutDpcObject);

    //
    // now send CLEAR_FEATURE(ENDPOINT_STALL) for the IN endpoint
    //

    USB_BUILD_CLEAR_FEATURE(
            &DeviceExtension->Urb.ControlTransfer,
            USB_COMMAND_TO_ENDPOINT,
            USB_FEATURE_ENDPOINT_STALL,
            muInstance->BulkInEndpointAddress,
            (PURB_COMPLETE_PROC)MU_ResetDeviceCompletionRoutine,
            DeviceExtension
            );

    //
    //  The completion routine deal with errors.
    //
    muInstance->Device->SubmitRequest(&DeviceExtension->Urb);
}

VOID
MU_ResetDeviceCompletionRoutine (
    IN PURB             Urb,
    IN PVOID            Context
    )
/*++
    Routine Description:
        
        This routine is the heart of the USB Bulk-Only Mass Storage Reset Sequence.
        The reset sequence consists of:
            1) Clear the STALL on the Bulk-IN endpoint.
            2) Clear the STALL on the Bulk-OUT endpoint.
            3) Send the the BULK_ONLY_MASS_STORAGE_RESET command on the control pipe.

        Clearing a STALL is a two step process on Xbox, first you send a 
        SET_ENDPOINT_STATE Urb to clear the endpoint HALT bit, and to reset the
        data toggle.  Then you send a CLEAR_FEATURE(ENDPOINT_STALL) over the 
        control endpoint to the device, so it clears its STALL bit.

        Step 1) was initiated by MU_fMrbErrorHandler.  So this routine verifies
        that that succeeded.

        Steps 2) and 3) are both asynchronous.  So this routine is a multistage
        state machine.  DeviceFlags are used to keep track of the stage.

        Any failure during the reset sequence, results in reporting the device as
        not responding to the core USB stack, if MU_Remove has not been called in
        the interm.  The core USB stack will drop the device (notifying everyone
        it is gone), and then reenumerate.  This procedure is not gentle!

        At the end of the reset sequence the retry count of the MRB is decremented
        and checked, if there are still retries remaining, then MU_fCbwTransfer is
        called to restart the Mrb.

        Otherwise, the MRB completion routine is called with STATUS_UNSUCCESSFUL.
        Unfortunately, the original error (timeout, or transmission failure)
        has not been propogated.
--*/
{
    PMU_DEVICE_EXTENSION deviceExtension = (PMU_DEVICE_EXTENSION) Context;
    ULONG                resetStage = deviceExtension->DeviceFlags & DF_RESET_STEPS;
    PMU_INSTANCE         muInstance = deviceExtension->MuInstance;

    //
    //  Mark that the primary URB is not outstanding
    //
    CLEAR_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING);

    //
    //  Stop the MRB timer if it is running
    //
    if(TEST_FLAG(deviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING))
    {
        KeCancelTimer(&deviceExtension->Mrb.Timer);
        CLEAR_FLAG(deviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING);
    }

    //
    //  Check for device not connected.
    //
    if(TEST_FLAG(deviceExtension->DeviceFlags, DF_REMOVED|DF_PENDING_REMOVE))
    {
        USB_DBG_TRACE_PRINT(("Mrb.CompletionRoutine(0x%0.8x, STATUS_DEVICE_NOT_CONNECTED)", deviceExtension));
        deviceExtension->Mrb.CompletionRoutine(deviceExtension, STATUS_DEVICE_NOT_CONNECTED);
        return;
    }

    ASSERT(muInstance);

    //
    //  Check for USB errors
    //

    if(USBD_ERROR(Urb->Header.Status))
    {
        
        USB_DBG_WARN_PRINT(("Reset Sequence Failed, reporting device as not responding."));
        
        //
        //  Clear all the reset flags, reset is over, wherever it was
        //  
        CLEAR_FLAG(deviceExtension->DeviceFlags, DF_RESET_STEPS);

        //
        //  Report the device as not responding
        //
        muInstance->Device->DeviceNotResponding();
        
        //
        //  Fail the MRB as STATUS_DEVICE_NOT_CONNECTED
        //
        USB_DBG_TRACE_PRINT(("Mrb.CompletionRoutine(0x%0.8x, STATUS_DEVICE_NOT_CONNECTED)", deviceExtension));
        deviceExtension->Mrb.CompletionRoutine(deviceExtension, STATUS_DEVICE_NOT_CONNECTED);

        return;
    }

    //
    //  Move on to the next stage of the Reset sequence.
    //  (note that we are switching on the resetStage 
    //  of the just completed stage.)
    //

    switch(resetStage)
    {
        case DF_RESET_STEP1:
            
            //
            //  Switch the flag to indicate step2
            //

            CLEAR_FLAG(deviceExtension->DeviceFlags, DF_RESET_STEP1);
            SET_FLAG(deviceExtension->DeviceFlags, DF_RESET_STEP2);

            //
            //  Clear the data toggle and software halt flag for Bulk-OUT
            //  This is a synchronous USB request and should never fail.
            //

            USB_BUILD_SET_ENDPOINT_STATE(
                    &deviceExtension->Urb.GetSetEndpointState,
                    deviceExtension->BulkOutEndpointHandle,
                    USB_ENDPOINT_STATE_DATA_TOGGLE_RESET | USB_ENDPOINT_STATE_CLEAR_HALT
                    );

            if(USBD_ERROR(muInstance->Device->SubmitRequest(&deviceExtension->Urb)))
            {
                USB_DBG_WARN_PRINT(("MU_ResetDeviceDpc: Bulk-OUT SetEndpointStatus failed!"));
                ASSERT(FALSE);
            }

            //
            // Build CLEAR_FEATURE(ENDPOINT_STALL) for the OUT endpoint
            // (it will get sent at the end of this routine.)

            USB_BUILD_CLEAR_FEATURE(
                    &deviceExtension->Urb.ControlTransfer,
                    USB_COMMAND_TO_ENDPOINT,
                    USB_FEATURE_ENDPOINT_STALL,
                    muInstance->BulkOutEndpointAddress,
                    (PURB_COMPLETE_PROC)MU_ResetDeviceCompletionRoutine,
                    deviceExtension
                    );

            break;
        case DF_RESET_STEP2:
            
            //
            //  Switch the flag to indicate step2
            //
            
            CLEAR_FLAG(deviceExtension->DeviceFlags, DF_RESET_STEP2);
            SET_FLAG(deviceExtension->DeviceFlags, DF_RESET_STEP3);

            //
            // Build the BULK_ONLY_MASS_STORAGE_RESET command
            // (it will get sent at the end of this routine.)

            USB_BUILD_CONTROL_TRANSFER(
                &deviceExtension->Urb.ControlTransfer,
                NULL,
                NULL, 
                0,
                USB_TRANSFER_DIRECTION_OUT,
                MU_ResetDeviceCompletionRoutine,
                deviceExtension,
                FALSE,
                (USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE),
                BULK_ONLY_MASS_STORAGE_RESET,
                0,
                muInstance->InterfaceNumber,
                0);

            break;
        case DF_RESET_STEP3:
            
            //
            //  Clear DF_RESET_STEP3, the reset sequence is over.
            //

            CLEAR_FLAG(deviceExtension->DeviceFlags, DF_RESET_STEP3);

            //
            //  Check for retries.
            //
            //  retries = (total tries) - 1, so post decrement
            //  when checking.
            //

            if(deviceExtension->Mrb.Retries--)
            {
               //
               //   If we resubmit the Mrb we need to pet the
               //   watchdog, it wasn't intended to wait through
               //   lots of retries.
               //
               MU_DEBUG_PET_WATCHDOG(deviceExtension);
               //
               //   Resubmit the Mrb
               //
               MU_fCbwTransfer(deviceExtension);
            } else
            {
               //
               //   Fail with status unsuccessful
               //
               USB_DBG_TRACE_PRINT(("Mrb.CompletionRoutine(0x%0.8x, STATUS_UNSUCCESSFUL)", deviceExtension));
               deviceExtension->Mrb.CompletionRoutine(deviceExtension, deviceExtension->Mrb.FailedStatus);
            }

            return;

        default:
            USB_DBG_ERROR_PRINT(("Reaching here indicates an MU driver bug!"));
            return;
    }

    //
    //  Set the MRB timer to time the reset.
    //
    LARGE_INTEGER deltaTime;
    deltaTime.QuadPart = MRB_RESET_TIMEOUT*MRB_TIMEOUT_UNIT;
    SET_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING);
    SET_FLAG(deviceExtension->DeviceFlags, DF_MRB_TIMER_RUNNING);
    KeSetTimer(&deviceExtension->Mrb.Timer, deltaTime, &deviceExtension->Mrb.TimeoutDpcObject);

    //
    //  Submit the URB built in either case DF_RESET_STEP1, 
    //  or case DF_RESET_STEP2.
    //
    SET_FLAG(deviceExtension->DeviceFlags,DF_PRIMARY_URB_PENDING);
    muInstance->Device->SubmitRequest(&deviceExtension->Urb);
    
    return;
}


#if DBG
VOID
FASTCALL
MU_fValidateMrb(
    IN PMU_DEVICE_EXTENSION DeviceExtension
    )
/*++
    Routine Description:
        Debug Only routine that validates the current mrb.  All this
        routine does is spew errors or possible errors, otherwise
        it does not effect the operation of the driver.
      
--*/
{
    //
    //  1) If no direction flag is set, then the DataBuffer
    //     and TransferLength are NULL, and 0.
    //
    if(!TEST_FLAG(DeviceExtension->Mrb.Flags, MRB_FLAGS_UNSPECIFIED_DIRECTION))
    {
        if(DeviceExtension->Mrb.DataBuffer || DeviceExtension->Mrb.TransferLength)
        {
            USB_DBG_ERROR_PRINT(("No direction, but non-NULL DataBuffer or non-zero length"));
        }
    }
        
    //
    //  2) Only one direction flag is set.
    //
    if(TEST_ALL_FLAGS(DeviceExtension->Mrb.Flags, MRB_FLAGS_UNSPECIFIED_DIRECTION))
    {
        USB_DBG_ERROR_PRINT(("More than one direction flag is set in Mrb!"));
    }
    //
    //  3) If a direction flag is that the DataBuffer is non-NULL
    //     and that the size is finite.
    //
    if(TEST_FLAG(DeviceExtension->Mrb.Flags, MRB_FLAGS_UNSPECIFIED_DIRECTION))
    {
        if(NULL == DeviceExtension->Mrb.DataBuffer)
        {
            USB_DBG_ERROR_PRINT(("Mrb with transfer to\from NULL DataBuffer!"));
        }
        if(0 == DeviceExtension->Mrb.TransferLength)
        {
            USB_DBG_ERROR_PRINT(("Mrb with transfer of 0 length!"));
        }
    }

    return;
}

#endif //DBG

