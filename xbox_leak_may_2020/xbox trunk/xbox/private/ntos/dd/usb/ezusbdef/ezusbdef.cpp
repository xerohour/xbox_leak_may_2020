/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    ezusbdef.cpp

Abstract:

    This module implements a USB class driver for the EZ-USB default device.

--*/

#define _NTOS_
#include <ntddk.h>
#include <xtl.h>
#include <usb.h>
#include "ezusbdef.h"

//
// Declare the structure used by the USB enumeration code to internally track
// devices of this type.
//

DECLARE_XPP_TYPE(XDEVICE_TYPE_EZUSBDEF)

//
// Create the table of device types that this class driver supports.
//

USB_DEVICE_TYPE_TABLE_BEGIN(Ezusb)
    USB_DEVICE_TYPE_TABLE_ENTRY(XDEVICE_TYPE_EZUSBDEF)
USB_DEVICE_TYPE_TABLE_END()

//
// Create the structure that binds the USB class, subclass, and protocol codes
// to this class driver.
//

USB_CLASS_DRIVER_DECLARATION_DEVICE_LEVEL(Ezusb, USB_DEVICE_CLASS_VENDOR_SPECIFIC, 0xFF, 0xFF)

//
// Register the class driver with the USB core driver by placing a pointer to
// the above structure in the .XPP$Class section.
//

#pragma data_seg(".XPP$ClassEzusb")
USB_CLASS_DECLARATION_POINTER(Ezusb)
#pragma data_seg(".XPP$Data")

//
// Stores all of the per-port instance data related to a device.  Allocate the
// maximum number of devices as a global.
//

typedef struct _EZUSB_DEVICE_STATE {
    IUsbDevice *Device;
    BOOLEAN DeviceAttached : 1;
    BOOLEAN DeviceRemoved : 1;
    BOOLEAN DefaultEndpointOpened : 1;
    BOOLEAN ClosingEndpoints : 1;
    BOOLEAN RemoveDevicePending : 1;
    URB CloseEndpointUrb;
    KEVENT CloseEndpointEvent;
} EZUSB_DEVICE_STATE, *PEZUSB_DEVICE_STATE;

EZUSB_DEVICE_STATE EzusbDeviceState[XGetPortCount()];

//
// Track the device insertions and removal bitmasks here.  The application
// cannot use XGetDeviceChanges because the USB core driver only reports changes
// to XTL for devices without USB_DEVICE_CLASS_VENDOR_SPECIFIC as a class code.
//

DWORD EzusbDeviceInsertions;
DWORD EzusbDeviceRemovals;

//
// Define the EZ-USB vendor specific request codes.
//

#define EZUSB_REQUEST_FIRMWARE_LOAD                     0xA0

//
// Local support.
//

VOID
EzusbCloseEndpointsAsync(
    PEZUSB_DEVICE_STATE DeviceState
    );

VOID
EzusbInit(
    IUsbInit *UsbInit
    )
/*++

Routine Description:

    This routine is invoked by the core USB driver to initialize the class
    driver.  This routine can allocate resources for the expected number of
    devices (either statically known or dynamically determined from the values
    from XInitDevices) and register resource requirements with the core USB
    driver.

Arguments:

    UsbInit - Specifies a virtual table of functions that can be used to control
        the behavior of this class driver.

Return Value:

    None.

--*/
{
    DWORD dwPort;
    PEZUSB_DEVICE_STATE DeviceState;

    for (dwPort = 0; dwPort < XGetPortCount(); dwPort++) {

        DeviceState = &EzusbDeviceState[dwPort];

        //
        // Initialize the event used to synchronize the closing of endpoints.
        //

        KeInitializeEvent(&DeviceState->CloseEndpointEvent,
            NotificationEvent, FALSE);
    }
}

VOID
EzusbAddDevice(
    IUsbDevice *Device
    )
/*++

Routine Description:

    This routine is invoked by the core USB driver during enumeration time when
    a device has been added that is supported by this class driver.

Arguments:

    Device - Specifies a virtual table of functions that can be used to control
        the behavior of the device.

Return Value:

    None.

--*/
{
    DWORD dwPort;
    PEZUSB_DEVICE_STATE DeviceState;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Verify that the device is attached to a legal port number.  Note that if
    // the device were plugged into the bottom slot of a hub, then the port
    // number will exceed XGetPortCount() and we'll ignore the device.
    //

    dwPort = Device->GetPort();

    if (dwPort >= XGetPortCount()) {
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    DeviceState = &EzusbDeviceState[dwPort];

    //
    // Verify that we haven't already seen a device attached.
    //

    if (DeviceState->DeviceAttached) {
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    //
    // Remember that a device is physically attached.
    //

    DeviceState->DeviceAttached = TRUE;

    //
    // Store information about the device in our globals.
    //

    DeviceState->Device = Device;

    //
    // Notify the USB enumeration code that we have successfully added the
    // device.
    //

    DeviceState->Device->AddComplete(USBD_STATUS_SUCCESS);

    //
    // Remember that this device has been inserted in the global device bitmap.
    //

    EzusbDeviceInsertions |= (1 << dwPort);
}

VOID
EzusbRemoveDevice(
    IUsbDevice *Device
    )
/*++

Routine Description:

    This routine is invoked by the core USB driver during enumeration time when
    a device has been removed that had successfully been added before.

Arguments:

    Device - Specifies a virtual table of functions that can be used to control
        the behavior of the device.

Return Value:

    None.

--*/
{
    DWORD dwPort;
    PEZUSB_DEVICE_STATE DeviceState;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    dwPort = Device->GetPort();

    ASSERT(dwPort < XGetPortCount());

    DeviceState = &EzusbDeviceState[dwPort];

    ASSERT(DeviceState->DeviceAttached);
    ASSERT(DeviceState->Device == Device);

    //
    // Remember that we're in the middle of a device removal.
    //

    DeviceState->RemoveDevicePending = TRUE;

    //
    // Close all of the open endpoints.  When this operation completes, the
    // remove device process will be completed.
    //

    EzusbCloseEndpointsAsync(DeviceState);

    //
    // Remember that this device has been removed in the global device bitmap.
    //

    EzusbDeviceRemovals |= (1 << dwPort);
}

VOID
EzusbCloseEndpointsComplete(
    PURB Urb,
    PVOID Context
    )
/*++

Routine Description:

    This routine is invoked after the URB has completed to close an endpoint.

Arguments:

    Urb - Specifies the pointer to the URB that has completed.

    Context - Specifies the context supplied to URB_BUILD_CONTROL_TRANSFER.

Return Value:

    None.

--*/
{
    PEZUSB_DEVICE_STATE DeviceState;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    DeviceState = (PEZUSB_DEVICE_STATE)Context;

    if (DeviceState->DefaultEndpointOpened) {

        //
        // Close the default endpoint.
        //

        USB_BUILD_CLOSE_DEFAULT_ENDPOINT(&Urb->CloseEndpoint,
                                         EzusbCloseEndpointsComplete,
                                         DeviceState);

        DeviceState->DefaultEndpointOpened = FALSE;

    } else {

        //
        // All endpoints are closed.  Signal the close endpoint event in case a
        // thread is waiting for the close endpoint to complete.
        //

        KeSetEvent(&DeviceState->CloseEndpointEvent, IO_NO_INCREMENT,
            FALSE);
        DeviceState->ClosingEndpoints = FALSE;

        //
        // If the device has been removed, then complete the device removal
        // process by notifying the USB enumeration code.
        //

        if (DeviceState->RemoveDevicePending) {

            DeviceState->RemoveDevicePending = FALSE;
            DeviceState->DeviceAttached = FALSE;
            DeviceState->DeviceRemoved = TRUE;

            DeviceState->Device->RemoveComplete();
        }

        return;
    }

    //
    // There's at least one endpoint still open.  If an endpoint is open, then
    // we must still think the device is logically attached and connected to
    // this device extension.
    //

    ASSERT(DeviceState->DeviceAttached);

    //
    // Submit the close request and wait for the USB driver to close the
    // endpoint.
    //

    DeviceState->Device->SubmitRequest(Urb);
}

VOID
EzusbCloseEndpointsAsync(
    PEZUSB_DEVICE_STATE DeviceState
    )
/*++

Routine Description:

    This routine asynchronously closes all of the open endpoints.

Arguments:

    DeviceState - Specifies the device whose endpoints are to be closed.

Return Value:

    None.

--*/
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // If we haven't already started closing endpoints, then kick off the
    // process.
    //

    if (!DeviceState->ClosingEndpoints) {

        //
        // Clear the close endpoint event used for synchronous close operations.
        //

        KeClearEvent(&DeviceState->CloseEndpointEvent);
        DeviceState->ClosingEndpoints = TRUE;

        //
        // Enter the close endpoints state machine.
        //

        EzusbCloseEndpointsComplete(&DeviceState->CloseEndpointUrb,
            DeviceState);
    }
}

VOID
EzusbCloseEndpoints(
    PEZUSB_DEVICE_STATE DeviceState
    )
/*++

Routine Description:

    This routine synchronously closes all of the open endpoints.

Arguments:

    DeviceState - Specifies the device whose endpoints are to be opened.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    // Asynchronously close all of the open endpoints.  When all of the
    // endpoints are closed, the close endpoint event is signaled.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    EzusbCloseEndpointsAsync(DeviceState);

    KeLowerIrql(OldIrql);

    //
    // Block until the endpoints have been closed.
    //

    KeWaitForSingleObject(&DeviceState->CloseEndpointEvent, Executive,
        KernelMode, FALSE, NULL);
}

VOID
EzusbGetDeviceChanges(
    LPDWORD lpdwInsertions,
    LPDWORD lpdwRemovals
    )
/*++

Routine Description:

    This routine is the equivalent of XGetDeviceChanges for the EZ-USB device.

Arguments:

    lpdwInsertions - Specifies the buffer to receive the bitmask of devices that
        have been inserted.

    lpdwRemovals - Specifies the buffer to receive the bitmask of devices that
        have been removed.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;

    OldIrql = KeRaiseIrqlToDpcLevel();

    *lpdwInsertions = EzusbDeviceInsertions;
    *lpdwRemovals = EzusbDeviceRemovals;

    EzusbDeviceInsertions = 0;
    EzusbDeviceRemovals = 0;

    KeLowerIrql(OldIrql);
}

VOID
EzusbSignalEventComplete(
    PURB Urb,
    PVOID Context
    )
/*++

Routine Description:

    This routine is invoked after a generic URB has completed.

Arguments:

    Urb - Specifies the pointer to the URB that has completed.

    Context - Specifies the context supplied to the URB builder macro.

Return Value:

    None.

--*/
{
    //
    // Wake up the thread waiting for the URB to complete.
    //

    KeSetEvent((PKEVENT)Context, EVENT_INCREMENT, FALSE);
}

DWORD
EzusbFirmwareLoad(
    DWORD dwPort,
    WORD wStartingAddress,
    LPVOID lpvFirmware,
    WORD wNumberOfBytes,
    BOOL fUpload
    )
/*++

Routine Description:

    This routine performs a firmware upload or download for the EZ-USB default
    device.

Arguments:

    dwPort - Specifies the port number of the device to be accessed.

    wStartingAddress - Specifies the starting address to begin the transfer.

    lpvFirmware - Specifies the buffer that contains the data to be uploaded or
        the buffer to receive the data to be downloaded.

    wNumberOfBytes - Specifies the number of bytes to transfer.

    fUpload - Specifies TRUE if data is to be uploaded to the EZ-USB device,
        else FALSE if data is to be downloaded from the EZ-USB device.

Return Value:

    Status of operation.

--*/
{
    PEZUSB_DEVICE_STATE DeviceState;
    KIRQL OldIrql;
    DWORD UsbdStatus;
    KEVENT Event;
    URB Urb;

    ASSERT(dwPort < XGetPortCount());

    DeviceState = &EzusbDeviceState[dwPort];

    OldIrql = KeRaiseIrqlToDpcLevel();

    //
    // Verify that the device is attached.
    //

    if (!DeviceState->DeviceAttached) {
        KeLowerIrql(OldIrql);
        return ERROR_DEVICE_NOT_CONNECTED;
    }

    //
    // Open the default control endpoint, if we haven't already opened it.
    //

    if (!DeviceState->DefaultEndpointOpened) {

        USB_BUILD_OPEN_DEFAULT_ENDPOINT(&Urb.OpenEndpoint);

        UsbdStatus = DeviceState->Device->SubmitRequest(&Urb);

        if (!USBD_SUCCESS(UsbdStatus)) {
            KeLowerIrql(OldIrql);
            return IUsbDevice::Win32FromUsbdStatus(Urb.Header.Status);
        }

        DeviceState->DefaultEndpointOpened = TRUE;
    }

    //
    // Initialize the event used to wait URBs to complete.
    //

    KeInitializeEvent(&Event, NotificationEvent, FALSE);

    //
    // Build and submit the vendor specific command to transfer the buffer.
    //

    USB_BUILD_CONTROL_TRANSFER(&Urb.ControlTransfer,
                               NULL,
                               lpvFirmware,
                               wNumberOfBytes,
                               USB_TRANSFER_DIRECTION_IN,
                               EzusbSignalEventComplete,
                               &Event,
                               TRUE,
                               USB_DEVICE_TO_HOST | USB_VENDOR_COMMAND | USB_COMMAND_TO_DEVICE,
                               EZUSB_REQUEST_FIRMWARE_LOAD,
                               wStartingAddress,
                               0,
                               wNumberOfBytes);

    if (fUpload) {
        Urb.ControlTransfer.TransferDirection = USB_TRANSFER_DIRECTION_OUT;
        Urb.ControlTransfer.SetupPacket.bmRequestType =
            USB_HOST_TO_DEVICE | USB_VENDOR_COMMAND | USB_COMMAND_TO_DEVICE;
    }

    DeviceState->Device->SubmitRequest(&Urb);

    //
    // Wait for the URB to complete.  This must be done at lowered IRQL.
    //

    KeLowerIrql(OldIrql);
    KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);

    //
    // Check the URB for any errors.
    //

    if (USBD_ERROR(Urb.Header.Status)) {
        DbgPrint("EzusbFirmwareLoad failed %08x\n", Urb.Header.Status);
        return IUsbDevice::Win32FromUsbdStatus(Urb.Header.Status);
    }

    return ERROR_SUCCESS;
}
