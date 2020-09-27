/*++

Copyright (c) 2000 Microsoft Corporation

    modem.cpp

Abstract:
    
    Driver entry points for the XBox USB modem driver.
    
Revision History:

    06-21-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* Modem_IsConnected
*
\***************************************************************************/

BOOL Modem_IsConnected(VOID)
{
    ASSERT_DISPATCH_LEVEL();

    return (gDevice.pDevice != NULL);
}

/***************************************************************************\
* Modem_Create
*
\***************************************************************************/

BOOL Modem_Create(VOID)
{
    ASSERT_DISPATCH_LEVEL();

    if (!OpenEndpoints()) {
        CloseEndpoints();
        return FALSE;
    }

    ReadWorker();

    return TRUE;
}

/***************************************************************************\
* Modem_Close
*
\***************************************************************************/

BOOL Modem_Close(VOID)
{
    ASSERT_DISPATCH_LEVEL();

    if (gRead.pevent != NULL) {
        ObDereferenceObject(gRead.pevent);
        gRead.pevent = NULL;
    }

    CloseEndpoints();
    return TRUE;
}

/***************************************************************************\
* Modem_AddDevice
*
\***************************************************************************/

VOID Modem_AddDevice(IUsbDevice *pDevice, PVOID)
{
    ASSERT_DISPATCH_LEVEL();

    if (gDevice.pDevice == NULL) {
        ConfigureDevice(pDevice);
    } else {
        pDevice->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE, NULL);
    }
}

/***************************************************************************\
* CloseDefaultEndpoint
*
\***************************************************************************/

VOID CloseDefaultEndpoint(PURB purb, IUsbDevice *pDevice)
{
    ASSERT_DISPATCH_LEVEL();

    Modem_Close();
}

/***************************************************************************\
* Modem_RemoveDevice
*
\***************************************************************************/

VOID Modem_RemoveDevice(IUsbDevice *pDevice)
{
    PURB purb;

    ASSERT_DISPATCH_LEVEL();

    gfRemoved = TRUE;

    purb = &gUrb;

    USB_BUILD_CLOSE_DEFAULT_ENDPOINT(
            (PURB_CLOSE_ENDPOINT)purb,
            (PURB_COMPLETE_PROC)CloseDefaultEndpoint,
            (PVOID)pDevice);

    pDevice->SubmitRequest(purb);
}

/***************************************************************************\
* DriverEntry
*
\***************************************************************************/

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT, IN PUNICODE_STRING)
{
    gpMdl = IoAllocateMdl(&gMem, sizeof(gMem), FALSE, FALSE, NULL);
    if (gpMdl == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    gpMdlRead = IoAllocateMdl(&gRead.buffer, MODEM_BUFFER_SIZE, FALSE, FALSE, NULL);
    if (gpMdlRead == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    gDeviceDescription.ConnectorType = USBPNP_CONNECTOR_TYPE_DIRECT;
    gDeviceDescription.ControlEndpointCount = 1;
    gDeviceDescription.BulkEndpointCount = 2;
    gDeviceDescription.InterruptEndpointCount = 1;
    gDeviceDescription.IsochEndpointCount = 0;
    gDeviceDescription.MaxBulkTDperTransfer = 2;
    gDeviceDescription.MaxControlTDperTransfer = 30;
    gDeviceDescription.NumNodes = 1;
    gClassDescription.AddDevice = (PFNADD_USB_DEVICE)Modem_AddDevice;
    gClassDescription.RemoveDevice = (PFNREMOVE_USB_DEVICE)Modem_RemoveDevice;
    gClassDescription.ClassId.AsLong = USB_DEVICE_TYPE_MODEM;
    gClassDescription.DeviceTypeCount = 1;
    gClassDescription.DeviceTypes = &gDeviceDescription;
    
    USBPNP_RegisterClassDriver(1, &gClassDescription);
    
    return STATUS_SUCCESS;
}

