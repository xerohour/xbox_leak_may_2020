/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    input.c

Abstract:
    
    Implementation of the Input API
    
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    03-07-00 created by Mitchell Dernis (mitchd)
    12-06-00 changed synchronization scheme
--*/

//
//  Pull in OS headers
//
#define _XAPI_
#define _KERNEL32_ //since we are linked to XAPI for now.
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xboxp.h>
#include <xapidrv.h>
#include <xdbg.h>
#include <stdio.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          '_DIX'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("INPUT");

//
//  Pull in public usb headers
//
#include <usb.h>

//
//  Pull in xid headers
//
#include "xid.h"

//--------------------------------------------------------------
//  Handle Verification
//--------------------------------------------------------------
#if DBG //Debug version verifies handles.
#define XID_VERIFY_HANDLES 
#endif
#ifdef XID_VERIFY_HANDLES

PXID_OPEN_DEVICE XID_OpenDeviceList = NULL;
VOID XidInsertHandle(PXID_OPEN_DEVICE OpenDevice);
VOID XidRemoveHandle(PXID_OPEN_DEVICE OpenDevice);
VOID XidCheckHandle(PCHAR ApiName, PXID_OPEN_DEVICE OpenDevice);

#define XID_INSERT_HANDLE(OpenDevice) XidInsertHandle(OpenDevice)
#define XID_REMOVE_HANDLE(OpenDevice) XidRemoveHandle(OpenDevice)
#define XID_CHECK_HANDLE(ApiName, OpenDevice) XidCheckHandle(ApiName, OpenDevice)

#else

#define XID_INSERT_HANDLE(OpenDevice) do{}while(0)
#define XID_REMOVE_HANDLE(OpenDevice) do{}while(0)
#define XID_CHECK_HANDLE(ApiName,OpenDevice) do{}while(0)
    
#endif

//--------------------------------------------------------------
//  Implementation
//--------------------------------------------------------------
XBOXAPI
HANDLE
WINAPI
XInputOpen(
    IN PXPP_DEVICE_TYPE XppDeviceType,
    IN DWORD dwPort,
    IN DWORD dwSlot,
    IN PXINPUT_POLLING_PARAMETERS pPollingParameters OPTIONAL
    )
/*++
    Routine Description:
        Public API to open a handle to an input device.
  
    Arguments:
        XppDeviceType       - device type to open.
        dwPort              - Port of device to open.
        dwSlot              - Slot of device to open.
        pPollingParameters  - [optional] polling parameters to use with device.

    Return Value:
        On success, handle to be used to access device.
        On failure, NULL.  Call GetLastError for specific error code.
        
          ERROR_SHARING_VIOLATION    - attempt to open more than one handle to a single device.
          ERROR_OUTOFMEMORY          - there was not enough memory to open the handle.
          ERROR_INVALID_PARAMETER    - the device type was not known.
          ERROR_DEVICE_NOT_CONNECTED - a device was not found of the given type in the specified
                                       port and slot.
          ERROR_IO_DEVICE            - an error occured talking to the device.

--*/
{
    UCHAR              xidType;
    DWORD              errorCode;
    PXID_OPEN_DEVICE   openDevice = NULL;

    ASSERT_LESS_THAN_DISPATCH_LEVEL();

    RIP_ON_NOT_TRUE_WITH_MESSAGE(XPP_XInitDevicesHasBeenCalled, "XInputOpen: XInitDevices must be called first!");
    
    //
    //  Convert the XppDeviceType to a XID type
    //
    if(XDEVICE_TYPE_GAMEPAD == XppDeviceType)
    {
        xidType = XID_DEVTYPE_GAMECONTROLLER;
    } else
#ifdef DEBUG_KEYBOARD
    if(XDEVICE_TYPE_DEBUG_KEYBOARD == XppDeviceType)
    {
        xidType = XID_DEVTYPE_KEYBOARD;
    } else
#endif //DEBUG_KEYBOARD
    if(XDEVICE_TYPE_IR_REMOTE == XppDeviceType)
    {
        xidType = XID_DEVTYPE_IRREMOTE;
    } else
    {        
        RIP("XInputOpen: XppDeviceType is not a valid type for the XInputXXX API\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    //
    //  RIP port and slot parameters
    //
    RIP_ON_NOT_TRUE(XInputOpen, dwPort<XGetPortCount());

    #if DBG
    switch(xidType)
    {
        
        case XID_DEVTYPE_GAMECONTROLLER:
        case XID_DEVTYPE_KEYBOARD:
        case XID_DEVTYPE_IRREMOTE:
            //
            //  The only valid value for Game Controllers is XDEVICE_NO_SLOT.
            //
            RIP_ON_NOT_TRUE(XInputOpen, (dwSlot == XDEVICE_NO_SLOT));
            break;
        default:
            //
            //  If this is reached it is a bug in XInputOpen.  It means the non-debug only
            //  code above that turns a XppDeviceType into a XidType is broken, or
            //  this switch statement does not contain all of the XidTypes.
            //
            ASSERT(FALSE);
    };
    #endif
       
    //
    //  If the polling parameters are NULL, look up the defaults based on type.
    //
    if(NULL == pPollingParameters)
    {
        pPollingParameters = XID_TypeInformationList[xidType].DefaultPollingParameters;
    }
    
    //
    //  Open the device
    //
    errorCode = XID_fOpenDevice(
                        xidType,
                        (dwSlot != XDEVICE_BOTTOM_SLOT) ? dwPort : dwPort +16,
                        &openDevice,
                        pPollingParameters
                        );

    //
    //  If the open failed, set the error code.
    //
    if(NULL==openDevice)
    {
        SetLastError(errorCode);
    }
    else
    {
        //
        //  Insert the handle into the list of 
        //  valid handles.
        //
        XID_INSERT_HANDLE(openDevice);
    }
    return (HANDLE)openDevice;
}  

XBOXAPI
void
WINAPI
XInputClose(
    IN HANDLE hDevice
    )
/*++

    Routine Description:
        Public API to close a handle to an input device.
  
    Arguments:
        XppDeviceType       - device type to open.
        dwPort              - Port of device to open.
        dwSlot              - Slot of device to open.
        pPollingParameters  - [optional] polling parameters to use with device.

    Return Value:
        None    
    Remarks:
        Close can block for several milliseconds while resources are being cleaned up.
        
        It is the callers responsibility not to use a handle after XInputClose has been
        called.  It is particularly risky with multiple threads.
            * No other call to an XInputXXX API using the handle should be started after
              XInputClose has been called, even if XInputClose has not yet returned.
            * XInputClose must not be called until all other XInputXXX API's using the handle
              have returned.
        In debug builds, an attempt is made to detect these conditions and RIP, however, the
        RIP's are not full-proof, and under some circumstances, may miss such a condition.
        If these rules are violated the result is undefined, in many cases this will lead to
        memory corruption and system crash.

        One should normally wait for asynchronous I/O (started with XInputSetState) to complete
        before calling XInputClose, however, it is not an error to call XInputClose with 
        asynchronous I/O pending.  The result is that pending I/O is guaranteed to complete,
        usually with status set to ERROR_CANCELLED, before XInputClose returns.

--*/
{
    PXID_OPEN_DEVICE openDevice = (PXID_OPEN_DEVICE)hDevice;
    ASSERT_LESS_THAN_DISPATCH_LEVEL();
    XID_REMOVE_HANDLE(openDevice);
    XID_fCloseDevice(openDevice);
    return;
}

XBOXAPI
DWORD
WINAPI
XInputGetCapabilities(
    IN HANDLE hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
    )
/*++
    Routine Description:
        Public API to retrieve capabilities from a device.
    Arguments:
        hDevice - handle to device for which to get capabilities.
        pCapabilities - pointer to buffer to get capabilities.
    Return Value:
        ERROR_SUCCESS - on success.
        ERROR_DEVICE_NOT_CONNECTED - if the device is no longer connected.
        ERROR_IO_DEVICE - problem communicating with the device.
    Remarks:
        This routine blocks while the device is queried.  This can take
        a few milliseconds.
--*/
{
    PCHAR                  pReportBuffer;
    ULONG                  length;
    URB_CONTROL_TRANSFER   urb;
    KEVENT                 event;
    KIRQL                  oldIrql;
    PXID_DEVICE_NODE       xidNode;
    DWORD                  errorCode = ERROR_SUCCESS;
    PXID_OPEN_DEVICE       openDevice = (PXID_OPEN_DEVICE)hDevice;

    ASSERT_LESS_THAN_DISPATCH_LEVEL();

    //
    //  Raise Irql for synchronization
    //
    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Verify handle.
    //
    XID_CHECK_HANDLE("XInputGetCapabilities", openDevice);
 
    //
    //  Make sure device is connected
    //
    xidNode = openDevice->XidNode;
    if( NULL == xidNode || xidNode->PendingRemove)
    {   
        errorCode = ERROR_DEVICE_NOT_CONNECTED;
        goto exit_input_get_caps;
    }

    //
    //  Zero the whole output buffer
    //
    RtlZeroMemory(pCapabilities, sizeof(XINPUT_CAPABILITIES));

    //
    //  Fill in the subtype, that doesn't depend on the report being queried
    //  so even if this fails with invalid parameter, we can check it.
    //
    pCapabilities->SubType = xidNode->SubType;
    
    //
    //  Make sure that the device supports GET_CAPABILITIES.
    //  This check is primarily for legacy devices, in particular
    //  legacy keyboards.
    //
    if(XID_TypeInformationList[xidNode->Type].ulFlags&XID_BSF_NO_CAPABILITIES)
    {
        errorCode = ERROR_ACCESS_DENIED;
    }
    else
    {
        //
        //  Get the output portion
        //
        pReportBuffer = (PCHAR)&pCapabilities->Out;
        length = XID_TypeInformationList[xidNode->Type].pOutputReportInfoList[0].bCurrentSize;

        //
        //  Build the URB
        //
        KeInitializeEvent(&event, NotificationEvent, FALSE);
        USB_BUILD_CONTROL_TRANSFER(
                &urb,
                NULL,
                (PVOID)(pReportBuffer-2),
                length+2,
                USB_TRANSFER_DIRECTION_IN,
                (PURB_COMPLETE_PROC)XID_SyncComplete,
                &event,
                TRUE,
                USB_DEVICE_TO_HOST | USB_VENDOR_COMMAND | USB_COMMAND_TO_INTERFACE,
                XID_COMMAND_GET_CAPABILITIES,
                0x0200,
                xidNode->InterfaceNumber,
                length+2
                );

        //
        //  Submit the request.
        //
        xidNode->Device->SubmitRequest((PURB)&urb);
    
        //
        //  Wait for transfer to complete
        //
        KeLowerIrql(oldIrql);
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        oldIrql = KeRaiseIrqlToDpcLevel();

        //
        //  Recheck the handle (should not have been closed)
        //
        XID_CHECK_HANDLE("XInputGetCapabilities", openDevice);

        //
        //  Make sure the device wasn't removed
        //  in the interm. 
        //  
        if( NULL == openDevice->XidNode || xidNode->PendingRemove)
        {   
            errorCode = ERROR_DEVICE_NOT_CONNECTED;
            goto exit_input_get_caps;
        }

        //
        //  Check for error
        //
        if(USBD_ERROR(urb.Hdr.Status))
        {
            errorCode = IUsbDevice::Win32FromUsbdStatus(urb.Hdr.Status);
            goto exit_input_get_caps;
        }

        //
        //  Now get the input portion
        //
        pReportBuffer = (PCHAR)&pCapabilities->In;
        length = XID_TypeInformationList[xidNode->Type].pInputReportInfoList[0].bCurrentSize;

        //
        //  Build the URB
        //
        USB_BUILD_CONTROL_TRANSFER(
                &urb,
                NULL,
                (PVOID)(pReportBuffer-2),
                length+2,
                USB_TRANSFER_DIRECTION_IN,
                (PURB_COMPLETE_PROC)XID_SyncComplete,
                &event,
                TRUE,
                USB_DEVICE_TO_HOST | USB_VENDOR_COMMAND | USB_COMMAND_TO_INTERFACE,
                XID_COMMAND_GET_CAPABILITIES,
                0x0100,
                xidNode->InterfaceNumber,
                length+2
                );
        KeInitializeEvent(&event, NotificationEvent, FALSE);
        xidNode->Device->SubmitRequest((PURB)&urb);

        //
        //  Wait for transfer to complete
        //
        KeLowerIrql(oldIrql);
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        oldIrql = KeRaiseIrqlToDpcLevel();

        //
        //  Make sure the device wasn't removed
        //  in the interm.
        //
        if( NULL == openDevice->XidNode || xidNode->PendingRemove)
        {   
            errorCode = ERROR_DEVICE_NOT_CONNECTED;
            goto exit_input_get_caps;
        }

        //
        //  Check for error
        //
        if(USBD_ERROR(urb.Hdr.Status))
        {
            errorCode = IUsbDevice::Win32FromUsbdStatus(urb.Hdr.Status);
            goto exit_input_get_caps;
        }
    }

exit_input_get_caps:
    //Bug 2578, make sure nobody tries to falsely divine information
    //from the reserved field.
    pCapabilities->Reserved = 0;
    KeLowerIrql(oldIrql);
    return errorCode;
}

XBOXAPI
DWORD
WINAPI
XInputGetState(
    IN HANDLE hDevice,
    OUT PXINPUT_STATE  pState
    )
/*++
    Routine Description:
        Public API for retrieving the latest known state of the
        device.  This routine does not actually perform I/O it
        just copies the latest known state into the caller's buffer.

        
    Arguments:
        hDevice - handle of device to get state for.
        pState  - buffer to receive state.

    Return Value:
        ERROR_SUCCESS - on success.
        ERROR_DEVICE_NOT_CONNECTED - device no longer connected (buffer is still copied).
--*/
{
    PXID_OPEN_DEVICE    openDevice = (PXID_OPEN_DEVICE) hDevice;
    DWORD               errorCode = ERROR_SUCCESS;
    KIRQL               oldIrql;
    ULONG               length;
    
    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Verify handle.
    //
    XID_CHECK_HANDLE("XInputGetState", openDevice);

    //
    //  Don't allow keyboards to work
    //
#ifdef DEBUG_KEYBOARD
    if(XID_DEVTYPE_KEYBOARD == openDevice->Type)
    {
        KeLowerIrql(oldIrql);
        RIP("XInputGetState does not work for keyboards.");
        return ERROR_INVALID_PARAMETER;
    }
#endif //DEBUG_KEYBOARD

    //
    //  If the device is not connected, mark it, but copy the
    //  data anyway.
    //
    if(NULL == openDevice->XidNode || openDevice->XidNode->PendingRemove)
    {
        errorCode = ERROR_DEVICE_NOT_CONNECTED;
    }
    
    //
    //  Copy packet number
    //
    pState->dwPacketNumber = openDevice->PacketNumber;
    
    //
    //  Copy the size indicated in the capatibility table.
    //
    length = XID_TypeInformationList[openDevice->Type].pInputReportInfoList[0].bCurrentSize;
    
    //
    //  Copy the latest report (We are using GameReport, but we could
    //                          use any member of the union of report types)
    RtlCopyMemory( (PVOID)&pState->Gamepad, (PVOID)openDevice->Report, length);
    
    
    KeLowerIrql(oldIrql);
    return errorCode;
}

XBOXAPI
DWORD
WINAPI
XInputSetState(
    IN HANDLE hDevice,
    OUT PXINPUT_FEEDBACK pFeedback
    )
/*++
    Routine Description:
        Public API for sending data to the device.
        THIS IS ASYNCHRONOUS!!!
    
    Arguments:
        hDevice - handle of device to send data to.
        pFeedback - feedback data to send

    Return Value:
        ERROR_IO_PENDING - on sucess.
        ERROR_DEVICE_NOT_CONNECTED - the device is no longer connected,

    Remarks:
        Calling XInputSetState programs output to the device.  This output could a few to many
        milliseconds to complete depending on the pPollingParameters passed to XInputOpen.
        During this time XAPI owns pFeedback, the caller should consider it read only.

        There are two options for synchronization:
            
        1) If the caller sets pFeedback->Header.hEvent to a valid event handle, it will
           be signaled when the I/O completes, otherwise hEvent should be NULL.
        2) The caller may poll pFeedback->Header.dwStatus.  This will be ERROR_IO_PENDING
           when XInputSetState returns, it will change to ERROR_SUCCESS or another (non-pending)
           error code when the transfer completes, successfully or otherwuse.

        Calling XInputClose before the I/O complete cancels the I/O.  pFeedback->Header.hEvent
        (if a valid event) will be signaled before XInputClose returns,and pFeedback->Header.dwStatus
        will change to ERROR_CANCELLED.

--*/
{
    PXID_OPEN_DEVICE         openDevice = (PXID_OPEN_DEVICE) hDevice;
    PXINPUT_FEEDBACK_INTERNAL feedback = (PXINPUT_FEEDBACK_INTERNAL)pFeedback;
    
    //
    //  Verify handle.
    //
    XID_CHECK_HANDLE("XInputSetState", openDevice);

    //
    //  Don't allow keyboards to work
    //
#ifdef DEBUG_KEYBOARD
    if(XID_DEVTYPE_KEYBOARD == openDevice->Type)
    {
        RIP("XInputSetState does not work for keyboards.");
        return ERROR_INVALID_PARAMETER;
    }
#endif //DEBUG_KEYBOARD

    //
    //  Check Handle the report ID, before
    //  passing down.
    //
    feedback->Internal.bReportId = 0;
    feedback->Internal.bSize = XID_TypeInformationList[openDevice->Type].pOutputReportInfoList[0].bCurrentSize + XID_REPORT_HEADER;
    return XID_fSendDeviceReport(openDevice, feedback);
}


XBOXAPI
DWORD
WINAPI
XInputPoll(
        IN HANDLE hDevice
        )
/*++
    Routine Description:
        Public API for manually polling an input device.
    
    Arguments:
        hDevice - handle of device to poll.
    
    Return Value:
        ERROR_SUCCESS - on success.
        ERROR_DEVICE_NOT_CONNECTED - if the device is no longer connected.

    Remarks:
        This API only needs to be called if XInputOpen was called with
        pPollingParameter->fAutoPoll set to FALSE.  Otherwise, it is
        a no-op.

        XInputGetState should be updated to return the latest state of the device
        no later than (pPollingParameter->bInputInterval + 1) ms, after XInputPoll
        is called.

--*/
{
    DWORD               errorCode = ERROR_SUCCESS;    
    PXID_OPEN_DEVICE    openDevice = (PXID_OPEN_DEVICE)hDevice;
    PXID_DEVICE_NODE    xidNode;
    KIRQL               oldIrql = KeRaiseIrqlToDpcLevel();
    
    //
    //  Verify handle.
    //
    XID_CHECK_HANDLE("XInputPoll", openDevice);

    //
    //  Make sure device is connected
    //
    xidNode = openDevice->XidNode;
    if(NULL == xidNode || xidNode->PendingRemove)
    {   
        errorCode = ERROR_DEVICE_NOT_CONNECTED;
    } else
    {
        if(!openDevice->AutoPoll)
        {
            if(!openDevice->OutstandingPoll)
            {   
                openDevice->OutstandingPoll = 1;
                #ifdef ALAMO_RAW_DATA_HACK
                if(xidNode->AlamoHack) XID_AlamoStartPoll(openDevice);
                else {
                #endif
                openDevice->Urb.BulkOrInterruptTransfer.TransferBufferLength = 
                                            openDevice->XidNode->bMaxInputReportSize;
                openDevice->XidNode->Device->SubmitRequest(&openDevice->Urb);  
                #ifdef ALAMO_RAW_DATA_HACK
                }
                #endif
            }
        }
    }

    KeLowerIrql(oldIrql);
    return errorCode;
}


//---------------------------------------------------------------------
//  Debug routine that keep track of open handles, and generate
//  RIP's when they are not valid.
//---------------------------------------------------------------------
#ifdef XID_VERIFY_HANDLES

VOID XidInsertHandle(PXID_OPEN_DEVICE OpenDevice)
{
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
    OpenDevice->NextOpenDevice = XID_OpenDeviceList;
    XID_OpenDeviceList = OpenDevice;
    KeLowerIrql(oldIrql);
}

VOID XidRemoveHandle(PXID_OPEN_DEVICE OpenDevice)
{
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
    PXID_OPEN_DEVICE previousDevice = NULL;
    PXID_OPEN_DEVICE device = XID_OpenDeviceList;
    BOOL found = FALSE;
    while(device)
    {
        if(device == OpenDevice)
        {
            if(previousDevice)
            {
                previousDevice->NextOpenDevice = device->NextOpenDevice;
            } else
            {
                XID_OpenDeviceList = device->NextOpenDevice;
            }
            found = TRUE;
            break;
        }
        previousDevice = device;
        device = device->NextOpenDevice;
    }
    KeLowerIrql(oldIrql);
    
    if(!found)
    {
        RIP("XInputClose: Invalid Handle\n");
    }

    return;    
}

VOID XidCheckHandle(CHAR *ApiName, PXID_OPEN_DEVICE OpenDevice)
{
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
    PXID_OPEN_DEVICE device = XID_OpenDeviceList;
    BOOL found = FALSE;
    while(device)
    {
        if(device == OpenDevice)
        {
            found = TRUE;
            break;
        }
        device = device->NextOpenDevice;
    }
    
    //
    //  BUG 9292 - cannot RIP at DISPATCH_LEVEL.  This hangs the machine when debugging
    //             with VC.  We were lowering IRQL to oldIrql before the RIP, but since
    //             this routine is usually called at DISPATCH_LEVEL, this is not good enough.
    //             Note that since this routine is NEVER called from a true DPC, it is always
    //             possible to explictly lower IRQL to PASSIVE_LEVEL.  We only want to do this
    //             though when we are going to RIP.  In that case, the system is already hosed
    //             and we do not lose any sleep over completely hosing it.
    //
    if(!found)
    {
        KeLowerIrql(PASSIVE_LEVEL);
        CHAR buffer[255];
        sprintf(buffer, "%s: Invalid Handle(0x%0.8x)\n", ApiName, OpenDevice);
        RIP(buffer);
        KIRQL dummyIrql;
        KeRaiseIrql(oldIrql, &dummyIrql);  // A feeble attempt to keep the system running after the RIP.
    } else
    {
        KeLowerIrql(oldIrql);
    }

    return;
}

#endif //XID_VERIFY_HANDLES