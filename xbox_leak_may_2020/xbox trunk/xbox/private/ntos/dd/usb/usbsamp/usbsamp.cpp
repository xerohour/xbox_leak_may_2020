/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    usbsamp.cpp

Abstract:

    This module implements a sample USB class driver.

--*/

#define _NTOS_
#include <ntddk.h>
#include <xtl.h>
#include <usb.h>
#include "hiddefs.h"
#include "usbsamp.h"

//
// Declare the structure used by the USB enumeration code to internally track
// devices of this type.
//

DECLARE_XPP_TYPE(XDEVICE_TYPE_SAMPLE)

//
// Create the table of device types that this class driver supports.
//

USB_DEVICE_TYPE_TABLE_BEGIN(Sample)
    USB_DEVICE_TYPE_TABLE_ENTRY(XDEVICE_TYPE_SAMPLE)
USB_DEVICE_TYPE_TABLE_END()

//
// Create the structure that binds the USB class, subclass, and protocol codes
// to this class driver.
//

USB_CLASS_DRIVER_DECLARATION(Sample, USB_DEVICE_CLASS_HUMAN_INTERFACE, 0xFF, 0xFF)

//
// Register the class driver with the USB core driver by placing a pointer to
// the above structure in the .XPP$Class section.
//

#pragma data_seg(".XPP$ClassSample")
USB_CLASS_DECLARATION_POINTER(Sample)
#pragma data_seg(".XPP$Data")

//
// During device enumeration, an URB may be required.  Because device
// enumeration is serialized, only one URB is required.  Allocate this as a
// global.
//

URB SampleEnumerationUrb;

//
// Stores all of the per-port instance data related to a device.  Allocate the
// maximum number of devices as a global.
//

typedef struct _SAMPLE_DEVICE_STATE {
    IUsbDevice *Device;
    BOOLEAN DeviceAttached : 1;
    BOOLEAN DeviceReady : 1;
    BOOLEAN DeviceOpened : 1;
    BOOLEAN DeviceRemoved : 1;
    BOOLEAN DefaultEndpointOpened : 1;
    BOOLEAN ClosingEndpoints : 1;
    BOOLEAN RemoveDevicePending : 1;
    BOOLEAN CloseDevicePending : 1;
    UCHAR InterruptEndpointAddress;
    UCHAR InterfaceNumber;
    PVOID InterruptEndpointHandle;
    URB CloseEndpointUrb;
    KEVENT CloseEndpointEvent;
    CRITICAL_SECTION CriticalSection;
} SAMPLE_DEVICE_STATE, *PSAMPLE_DEVICE_STATE;

SAMPLE_DEVICE_STATE SampleDeviceState[XGetPortCount()];

//
// Local support.
//

VOID
SampleSetProtocolComplete(
    PURB Urb,
    PVOID Context
    );

VOID
SampleCloseEndpointsAsync(
    PSAMPLE_DEVICE_STATE DeviceState
    );

VOID
SampleInit(
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
    PSAMPLE_DEVICE_STATE DeviceState;

    DbgPrint("SAMPLE: SampleInit called.\n");

    for (dwPort = 0; dwPort < XGetPortCount(); dwPort++) {

        DeviceState = &SampleDeviceState[dwPort];

        //
        // Initialize the event used to synchronize the closing of endpoints.
        //

        KeInitializeEvent(&DeviceState->CloseEndpointEvent,
            NotificationEvent, FALSE);

        //
        // Initialize the critical section used to synchronize passive level API
        // code.
        //

        InitializeCriticalSection(&DeviceState->CriticalSection);
    }

}

VOID
SampleAddDevice(
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
    PSAMPLE_DEVICE_STATE DeviceState;
    const USB_DEVICE_DESCRIPTOR8 *DeviceDescriptor;
    const USB_CONFIGURATION_DESCRIPTOR *ConfigurationDescriptor;
    const USB_INTERFACE_DESCRIPTOR *InterfaceDescriptor;
    const USB_ENDPOINT_DESCRIPTOR *EndpointDescriptor;

    DbgPrint("SAMPLE: SampleAddDevice called.\n");

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Verify that the device is attached to a legal port number.  Note that if
    // the device were plugged into the bottom slot of a hub, then the port
    // number will exceed XGetPortCount() and we'll ignore the device.
    //

    dwPort = Device->GetPort();

    if (dwPort >= XGetPortCount()) {
        DbgPrint("SAMPLE: Illegal port number.\n");
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    DeviceState = &SampleDeviceState[dwPort];

    //
    // Verify that we haven't already seen a device attached.
    //

    if (DeviceState->DeviceAttached) {
        DbgPrint("SAMPLE: Device already attached.\n");
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    ASSERT(!DeviceState->DeviceReady);

    //
    // Dump the contents of the device descriptor.
    //

    DeviceDescriptor = Device->GetDeviceDescriptor();

    DbgPrint("Device Descriptor\n");
    DbgPrint("\tbLength=%02x\n", DeviceDescriptor->bLength);
    DbgPrint("\tbDescriptorType=%02x\n", DeviceDescriptor->bDescriptorType);
    DbgPrint("\tbcdUSB=%04x\n", DeviceDescriptor->bcdUSB);
    DbgPrint("\tbDeviceClass=%02x\n", DeviceDescriptor->bDeviceClass);
    DbgPrint("\tbDeviceSubClass=%02x\n", DeviceDescriptor->bDeviceSubClass);
    DbgPrint("\tbDeviceProtocol=%02x\n", DeviceDescriptor->bDeviceProtocol);
    DbgPrint("\tbMaxPacketSize0=%02x\n", DeviceDescriptor->bMaxPacketSize0);
    DbgPrint("\n");

    //
    // Dump the contents of the configuration descriptor.
    //

    ConfigurationDescriptor = Device->GetConfigurationDescriptor();

    DbgPrint("Configuration Descriptor\n");
    DbgPrint("\tbLength=%02x\n", ConfigurationDescriptor->bLength);
    DbgPrint("\tbDescriptorType=%02x\n", ConfigurationDescriptor->bDescriptorType);
    DbgPrint("\twTotalLength=%04x\n", ConfigurationDescriptor->wTotalLength);
    DbgPrint("\tbNumInterfaces=%02x\n", ConfigurationDescriptor->bNumInterfaces);
    DbgPrint("\tbConfigurationValue=%02x\n", ConfigurationDescriptor->bConfigurationValue);
    DbgPrint("\tiConfiguration=%02x\n", ConfigurationDescriptor->iConfiguration);
    DbgPrint("\tbmAttributes=%02x\n", ConfigurationDescriptor->bmAttributes);
    DbgPrint("\tMaxPower=%02x\n", ConfigurationDescriptor->MaxPower);
    DbgPrint("\n");

    //
    // Dump the contents of the interface descriptor.
    //

    InterfaceDescriptor = Device->GetInterfaceDescriptor();

    DbgPrint("Interface Descriptor\n");
    DbgPrint("\tbLength=%02x\n", InterfaceDescriptor->bLength);
    DbgPrint("\tbDescriptorType=%02x\n", InterfaceDescriptor->bDescriptorType);
    DbgPrint("\tbInterfaceNumber=%02x\n", InterfaceDescriptor->bInterfaceNumber);
    DbgPrint("\tbAlternateSetting=%02x\n", InterfaceDescriptor->bAlternateSetting);
    DbgPrint("\tbNumEndpoints=%02x\n", InterfaceDescriptor->bNumEndpoints);
    DbgPrint("\tbInterfaceClass=%02x\n", InterfaceDescriptor->bInterfaceClass);
    DbgPrint("\tbInterfaceSubClass=%02x\n", InterfaceDescriptor->bInterfaceSubClass);
    DbgPrint("\tbInterfaceProtocol=%02x\n", InterfaceDescriptor->bInterfaceProtocol);
    DbgPrint("\tiInterface=%02x\n", InterfaceDescriptor->iInterface);
    DbgPrint("\n");

    //
    // Dump the contents of the endpoint descriptor.
    //

    EndpointDescriptor = Device->GetEndpointDescriptor(USB_ENDPOINT_TYPE_INTERRUPT,
        TRUE, 0);

    if (EndpointDescriptor == NULL) {
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    DbgPrint("Endpoint Descriptor\n");
    DbgPrint("\tbLength=%02x\n", EndpointDescriptor->bLength);
    DbgPrint("\tbDescriptorType=%02x\n", EndpointDescriptor->bDescriptorType);
    DbgPrint("\tbEndpointAddress=%02x\n", EndpointDescriptor->bEndpointAddress);
    DbgPrint("\tbmAttributes=%02x\n", EndpointDescriptor->bmAttributes);
    DbgPrint("\twMaxPacketSize=%02x\n", EndpointDescriptor->wMaxPacketSize);
    DbgPrint("\tbInterval=%02x\n", EndpointDescriptor->bInterval);
    DbgPrint("\n");

    //
    // Verify that the device supports the mouse boot protocol.
    //

    if ((InterfaceDescriptor->bInterfaceSubClass != HID_SUBCLASS_BOOT) ||
        (InterfaceDescriptor->bInterfaceProtocol != HID_PROTOCOL_MOUSE)) {
        DbgPrint("SAMPLE: Device is not a mouse.\n");
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
    DeviceState->InterfaceNumber = Device->GetInterfaceNumber();
    DeviceState->InterruptEndpointAddress =
        EndpointDescriptor->bEndpointAddress;

    //
    // The USB enumeration code has the default endpoint open for a class driver
    // to use until the AddComplete routine is invoked.  After AddComplete has
    // been called, then the class driver must open the default endpoint itself.
    //
    // For a mouse device, we need to switch to the boot protocol, so do that
    // now while the default endpoint is open.
    //

    USB_BUILD_CONTROL_TRANSFER(&SampleEnumerationUrb.ControlTransfer,
                               NULL,
                               NULL,
                               0,
                               0,
                               SampleSetProtocolComplete,
                               DeviceState,
                               TRUE,
                               USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE,
                               HID_REQUEST_SET_PROTOCOL,
                               HID_SET_PROTOCOL_BOOT,
                               DeviceState->InterfaceNumber,
                               0);

    Device->SubmitRequest(&SampleEnumerationUrb);

    //
    // The USB enumeration code is blocked until we call AddComplete on our
    // device, but other processing will continue while the above request is in
    // progress.
    //
}

VOID
SampleSetProtocolComplete(
    PURB Urb,
    PVOID Context
    )
/*++

Routine Description:

    This routine is invoked after the URB has completed to set the protocol of
    the HID device.

Arguments:

    Urb - Specifies the pointer to the URB that has completed.

    Context - Specifies the context supplied to URB_BUILD_CONTROL_TRANSFER.

Return Value:

    None.

--*/
{
    PSAMPLE_DEVICE_STATE DeviceState;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    DeviceState = (PSAMPLE_DEVICE_STATE)Context;

    //
    // Check if an error occurred while setting the protocol.  If so, fail to
    // attach the device and forward the error to the USB enumeration code.
    //

    if (USBD_ERROR(Urb->Header.Status)) {
        DeviceState->DeviceAttached = FALSE;
        DeviceState->Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    //
    // Remember that the device is now ready to be used.
    //

    DeviceState->DeviceReady = TRUE;

    //
    // Notify the USB enumeration code that we have successfully added the
    // device.
    //
    // Note that after this call has completed, the default endpoint is no
    // longer open and we must manually open it if required.
    //

    DeviceState->Device->AddComplete(USBD_STATUS_SUCCESS);
}

VOID
SampleRemoveDevice(
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
    PSAMPLE_DEVICE_STATE DeviceState;

    DbgPrint("SAMPLE: SampleRemoveDevice called.\n");

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    dwPort = Device->GetPort();

    ASSERT(dwPort < XGetPortCount());

    DeviceState = &SampleDeviceState[dwPort];

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

    SampleCloseEndpointsAsync(DeviceState);
}

USBD_STATUS
SampleOpenEndpoints(
    PSAMPLE_DEVICE_STATE DeviceState
    )
/*++

Routine Description:

    This routine opens all of the endpoints for the device.

Arguments:

    DeviceState - Specifies the device whose endpoints are to be opened.

Return Value:

    USBD status code.

--*/
{
    URB OpenUrb;
    USBD_STATUS UsbdStatus;

    //
    // Open the default endpoint.
    //

    USB_BUILD_OPEN_DEFAULT_ENDPOINT(&OpenUrb.OpenEndpoint);

    UsbdStatus = DeviceState->Device->SubmitRequest(&OpenUrb);

    if (USBD_SUCCESS(UsbdStatus)) {

        DeviceState->DefaultEndpointOpened = TRUE;

        //
        // Open the "interrupt in" endpoint.
        //

        USB_BUILD_OPEN_ENDPOINT(&OpenUrb.OpenEndpoint,
                                DeviceState->InterruptEndpointAddress,
                                USB_ENDPOINT_TYPE_INTERRUPT,
                                sizeof(HID_BOOT_MOUSE_REPORT),
                                10);

        UsbdStatus = DeviceState->Device->SubmitRequest(&OpenUrb);

        if (USBD_SUCCESS(UsbdStatus)) {
            DeviceState->InterruptEndpointHandle =
                OpenUrb.OpenEndpoint.EndpointHandle;
        }
    }

    return UsbdStatus;
}

VOID
SampleCloseEndpointsComplete(
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
    PSAMPLE_DEVICE_STATE DeviceState;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    DeviceState = (PSAMPLE_DEVICE_STATE)Context;

    if (DeviceState->DefaultEndpointOpened) {

        //
        // Close the default endpoint.
        //

        USB_BUILD_CLOSE_DEFAULT_ENDPOINT(&Urb->CloseEndpoint,
                                         SampleCloseEndpointsComplete,
                                         DeviceState);

        DeviceState->DefaultEndpointOpened = FALSE;

    } else if (DeviceState->InterruptEndpointHandle != NULL) {

        //
        // Close the "interrupt in" endpoint.
        //

        USB_BUILD_CLOSE_ENDPOINT(&Urb->CloseEndpoint,
                                 DeviceState->InterruptEndpointHandle,
                                 SampleCloseEndpointsComplete,
                                 DeviceState);

        DeviceState->InterruptEndpointHandle = NULL;

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
            DeviceState->DeviceReady = FALSE;
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
SampleCloseEndpointsAsync(
    PSAMPLE_DEVICE_STATE DeviceState
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

        SampleCloseEndpointsComplete(&DeviceState->CloseEndpointUrb,
            DeviceState);
    }
}

VOID
SampleCloseEndpoints(
    PSAMPLE_DEVICE_STATE DeviceState
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

    SampleCloseEndpointsAsync(DeviceState);

    KeLowerIrql(OldIrql);

    //
    // Block until the endpoints have been closed.
    //

    KeWaitForSingleObject(&DeviceState->CloseEndpointEvent, Executive,
        KernelMode, FALSE, NULL);
}

DWORD
SampleOpenDevice(
    DWORD dwPort
    )
/*++

Routine Description:

    This routine is invoked by application code to open a device that has been
    successfully enumerated (in particular, XGetDevices reports that the device
    is present).

Arguments:

    dwPort - Specifies the port number of the device to be opened.

Return Value:

    Status of operation.

--*/
{
    PSAMPLE_DEVICE_STATE DeviceState;
    KIRQL OldIrql;
    USBD_STATUS UsbdStatus;

    ASSERT(dwPort < XGetPortCount());

    DeviceState = &SampleDeviceState[dwPort];

    //
    // Synchronize passive level access to this device's state.
    //

    EnterCriticalSection(&DeviceState->CriticalSection);

    //
    // If the device has already been opened, then bail out now.

    if (DeviceState->DeviceOpened) {
        LeaveCriticalSection(&DeviceState->CriticalSection);
        return ERROR_SHARING_VIOLATION;
    }

    //
    // Synchronize access to our globals with the AddDevice and RemoveDevice
    // entrypoints by raising the IRQL to DPC level.  While running at this
    // raised IRQL, thread preemption is disabled.  Only interupt service
    // routines are allowed to run.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    //
    // Reset the flag that indicates that the device has been removed while the
    // device was open or was in the process of opening.
    //

    DeviceState->DeviceRemoved = FALSE;

    //
    // If the device is not ready for use, then bail out now.
    //

    if (!DeviceState->DeviceReady) {
        KeLowerIrql(OldIrql);
        LeaveCriticalSection(&DeviceState->CriticalSection);
        return ERROR_DEVICE_NOT_CONNECTED;
    }

    //
    // Open the device's endpoints.  Note that if this routine fails, some of
    // the endpoints may have been opened, so we need to close these endpoints.
    //

    UsbdStatus = SampleOpenEndpoints(DeviceState);

    if (!USBD_SUCCESS(UsbdStatus)) {
        KeLowerIrql(OldIrql);
        SampleCloseEndpoints(DeviceState);
        LeaveCriticalSection(&DeviceState->CriticalSection);
        return IUsbDevice::Win32FromUsbdStatus(UsbdStatus);
    }

    //
    // Remember that the device is now open.
    //

    DeviceState->DeviceOpened = TRUE;

    KeLowerIrql(OldIrql);

    LeaveCriticalSection(&DeviceState->CriticalSection);

    return ERROR_SUCCESS;
}

VOID
SampleSignalEventComplete(
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
SampleReadButtons(
    DWORD dwPort,
    LPBYTE lpbButtons
    )
/*++

Routine Description:

    This routine is invoked by application code to read the button state.

Arguments:

    dwPort - Specifies the port number of the device to be read from.

    lpbButtons - Specifies the location to receive the button state.

Return Value:

    Status of operation.

--*/
{
    PSAMPLE_DEVICE_STATE DeviceState;
    KIRQL OldIrql;
    KEVENT Event;
    URB Urb;
    HID_BOOT_MOUSE_REPORT BootMouseReport;

    ASSERT(dwPort < XGetPortCount());

    DeviceState = &SampleDeviceState[dwPort];

    OldIrql = KeRaiseIrqlToDpcLevel();

    //
    // If the device hasn't been opened, if the device is in the process of
    // being removed, or if the device has already been removed, then fail the
    // request.
    //

    if (!DeviceState->DeviceOpened || DeviceState->RemoveDevicePending ||
        DeviceState->DeviceRemoved) {
        KeLowerIrql(OldIrql);
        return ERROR_DEVICE_NOT_CONNECTED;
    }

    //
    // Initialize the event used to wait for the below URB to complete.
    //

    KeInitializeEvent(&Event, NotificationEvent, FALSE);

    //
    // Build an URB to do a control transfer from the default endpoint.  Read
    // the input report from the mouse interface.
    //

    USB_BUILD_CONTROL_TRANSFER(&Urb.ControlTransfer,
                               NULL,
                               &BootMouseReport,
                               sizeof(HID_BOOT_MOUSE_REPORT),
                               USB_TRANSFER_DIRECTION_IN,
                               SampleSignalEventComplete,
                               &Event,
                               TRUE,
                               USB_DEVICE_TO_HOST | USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE,
                               HID_REQUEST_GET_REPORT,
                               MAKEWORD(0, 1),
                               DeviceState->InterfaceNumber,
                               sizeof(HID_BOOT_MOUSE_REPORT));

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
        return IUsbDevice::Win32FromUsbdStatus(Urb.Header.Status);
    }

    //
    // The input report was successfullly read from the mouse, so return the
    // button state to the caller.
    //

    *lpbButtons = BootMouseReport.Buttons;

    return ERROR_SUCCESS;
}

VOID
SampleCloseDevice(
    DWORD dwPort
    )
/*++

Routine Description:

    This routine is invoked by application code to close a device that had been
    successfully opened.

Arguments:

    dwPort - Specifies the port number of the device to be closed.

Return Value:

    None.

--*/
{
    PSAMPLE_DEVICE_STATE DeviceState;
    KIRQL OldIrql;

    ASSERT(dwPort < XGetPortCount());

    DeviceState = &SampleDeviceState[dwPort];

    //
    // Synchronize passive level access to this device's state.
    //

    EnterCriticalSection(&DeviceState->CriticalSection);

    //
    // If the device hasn't been opened, then bail out now.
    //

    if (!DeviceState->DeviceOpened) {
        LeaveCriticalSection(&DeviceState->CriticalSection);
        return;
    }

    //
    // Close the device's endpoints.
    //

    SampleCloseEndpoints(DeviceState);

    //
    // Synchronize access to our globals with the AddDevice and RemoveDevice
    // entrypoints by raising the IRQL to DPC level.  While running at this
    // raised IRQL, thread preemption is disabled.  Only interupt service
    // routines are allowed to run.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    //
    // Remember that the device is now closed.
    //

    DeviceState->DeviceOpened = FALSE;

    KeLowerIrql(OldIrql);

    LeaveCriticalSection(&DeviceState->CriticalSection);
}
