/*++

Copyright (c) 1996-2000 Microsoft Corporation

Module Name:

    MU.CPP

Abstract:

    This source file implements the major entry points to the MU driver.

        Entries required by the USB core stack:
            MU_Init
            MU_AddDevice
            MU_RemoveDevice
        Entries used by XAPI for making MU's accessible.



Environment:

    Xbox USB class driver.

Revision History:

    George Chrysanthakopoulos (georgioc) wrote the original several months ago, loosely based
    on the existing Win2k USB mass storage driver.

    10-18-00 : Mitchell Dernis(mitchd) - systematic code and comment cleanup, now that all
               the major architectural issues are resolved.
    11-08-00 : Mitchell Dernis(mitchd) - change when DeviceObject are allocated

--*/

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************
#include "mu.h"
#include <stdio.h>

//*****************************************************************************
// Global Declarations
//*****************************************************************************
MU_DRIVER_EXTENSION MU_DriverExtension;

DRIVER_OBJECT MU_DriverObject = {
    MU_StartIo,                         // DriverStartIo
    NULL,                               // DriverDeleteDevice
    NULL,                               // DriverDismountVolume
    {
        IoInvalidDeviceRequest,         // IRP_MJ_CREATE
        IoInvalidDeviceRequest,         // IRP_MJ_CLOSE
        MU_InternalIo,                  // IRP_MJ_READ
        MU_InternalIo,                  // IRP_MJ_WRITE
        IoInvalidDeviceRequest,         // IRP_MJ_QUERY_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_SET_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_FLUSH_BUFFERS
        IoInvalidDeviceRequest,         // IRP_MJ_QUERY_VOLUME_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_DIRECTORY_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_FILE_SYSTEM_CONTROL
        MU_InternalIo,                  // IRP_MJ_DEVICE_CONTROL
        MU_InternalIo,                  // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_SHUTDOWN
        IoInvalidDeviceRequest,         // IRP_MJ_CLEANUP
    }
};

//This is exposed for XAPI so that XMountMU can check if the user tries to exceed it.
#if DBG
extern "C" ULONG MU_MaxUserDevices = MU_DEFAULT_MAX_MOUNTED;
#endif 

//*****************************************************************************
// Local Function Declarations
//*****************************************************************************
DEFINE_USB_DEBUG_FUNCTIONS("MU");

VOID
FASTCALL
MU_fCloseEndpoints (
    IN PMU_DEVICE_EXTENSION   DeviceExtension
    );

VOID
MU_CloseEndpointCompletion (
    IN PURB Urb,
    IN PVOID Context
    );


//*****************************************************************************
//  Declare XID types and class.
//*****************************************************************************
DECLARE_XPP_TYPE(XDEVICE_TYPE_MEMORY_UNIT)
USB_DEVICE_TYPE_TABLE_BEGIN(MU_)
USB_DEVICE_TYPE_TABLE_ENTRY(XDEVICE_TYPE_MEMORY_UNIT)
USB_DEVICE_TYPE_TABLE_END()
USB_CLASS_DRIVER_DECLARATION(MU_, USB_DEVICE_CLASS_STORAGE, 0xFF, 0xFF)
#pragma data_seg(".XPP$ClassMU")
USB_CLASS_DECLARATION_POINTER(MU_)
#pragma data_seg(".XPP$Data")

USB_RESOURCE_REQUIREMENTS MU_gResourceRequirements = 
    {USB_CONNECTOR_TYPE_LOW_POWER, 0, 0, 1, 2, 0, 0, MU_BULK_MIN_TD_QUOTA, 0, 0};

//*****************************************************************************
// Implementation of USB Required Entry Points:
//  MU_Init,
//  MU_AddDevice,
//  MU_RemoveDevice
//*****************************************************************************
#pragma code_seg(".XPPCINIT")
EXTERNUSB VOID MU_Init (IUsbInit *pUsbInit)
/*++
    Routine Description:

        Called by the USB core stack if the MU driver's description table is
        linked in to the application.
--*/
{
    ULONG portCount;
    ULONG maxDeviceCount;
    ULONG maxOpenCount;
    ULONG allocSize;
    ULONG index;
    OBJECT_STRING deviceObjectName;
    PMU_DEVICE_EXTENSION deviceExtension;
    PDEVICE_OBJECT deviceObject;

    USB_DBG_ENTRY_PRINT(("entering MU_Init\n"));

    //
    //  The "MU_DriverExtension" is just a global.  Initialize it.
    //

    RtlZeroMemory(&MU_DriverExtension,sizeof(MU_DRIVER_EXTENSION));
    InitializeListHead(&MU_DriverExtension.PartialRequestQueue);

    //
    //  Calculate maxDeviceCount and allocate the array of devices.
    //

    portCount = XGetPortCount();
    maxDeviceCount =  portCount * MU_CONTROLLERS_PER_PORT;

    //
    // Allocate an array of pointers to hold the nodes (MU_INSTANCES)
    //

    allocSize = sizeof(MU_INSTANCE)*maxDeviceCount;
    MU_DriverExtension.Nodes = (PMU_INSTANCE) RTL_ALLOCATE_HEAP(allocSize);
    ASSERT(MU_DriverExtension.Nodes);  //This allocation happens at boot, it better not fail.
    RtlZeroMemory(MU_DriverExtension.Nodes, allocSize);
    
    //
    //  Register the user selected devices
    //
    if(pUsbInit->UseDefaultCount())
    {
        maxOpenCount = MU_DEFAULT_MAX_MOUNTED;
    } else
    {
        maxOpenCount = pUsbInit->GetMaxDeviceTypeCount(XDEVICE_TYPE_MEMORY_UNIT);
        RIP_ON_NOT_TRUE_WITH_MESSAGE(maxOpenCount <= maxDeviceCount, "XInitDevices: requested more XDEVICE_TYPE_MEMORY_UNIT than available ports.");
        
        // Save the number of devices that the user requested so we can RIP in debug versions of XMountMU if the user tries to exceed this.
        #if DBG
        MU_MaxUserDevices = maxOpenCount;
        #endif 
        
        // Bump up the maximum opened count. This is because API's such as XGetMUNameFromPortAndSlot,
        // and XReadMUMetaData may need to mount an MU behind the game's back, the game has not already mounted it.
        // These API's are smart enough never to mount more than one at a time beyond what the game has mounted.
        maxOpenCount++; 
        // The maximum count never needs to exceed the maximum physical devices.
        if(maxOpenCount > maxDeviceCount) maxOpenCount = maxDeviceCount;
    }

    //
    //  Allocate all the device objects and place them on a free list
    //
    RtlInitObjectString(&deviceObjectName, OTEXT("\\Device\\MU_0"));
    for(index = 0; index < maxOpenCount; index++)
    {
        NTSTATUS status;
        deviceObjectName.Buffer[sizeof("\\Device\\MU")] = (index > 9) ? (index + OTEXT('A')) : (index + OTEXT('0'));
        status = IoCreateDevice(
                    &MU_DriverObject,
                    sizeof(MU_DEVICE_EXTENSION),
                    &deviceObjectName,
                    FILE_DEVICE_MEMORY_UNIT,
                    FALSE,
                    &deviceObject
                    );
        if(NT_SUCCESS(status))
        {
            
            deviceExtension = (PMU_DEVICE_EXTENSION)deviceObject->DeviceExtension;
            RtlZeroMemory(deviceExtension, sizeof(MU_DEVICE_EXTENSION));    
            deviceExtension->DeviceObject = deviceObject;
            deviceExtension->DeviceObjectNumber = index;
            deviceExtension->DeviceFlags = DF_REMOVED;
            deviceObject->StackSize = 1;
            deviceObject->Flags |=  DO_DIRECT_IO;
            deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
            MU_ReleaseDeviceObject(deviceExtension);
        } else break;
    }

#if DBG 
    if(index < maxOpenCount)
    {
        RIP("XInitDevices: Memory Unit failed to allocate memory all requested instances.");
    }
#endif 

    //
    //  Register the resources for the support device count
    //
    MU_gResourceRequirements.MaxDevices = maxOpenCount;
    if(maxOpenCount > 1) MU_gResourceRequirements.MaxBulkTDperTransfer *= 2;
    pUsbInit->RegisterResources(&MU_gResourceRequirements);

    //
    //  Xbox has no concept of drivers failing load
    //  so there is no point in returning an error code
    //  other than STATUS_SUCCESS
    //
    
    USB_DBG_EXIT_PRINT(("exiting MU_Init\n"));

    return;
}
#pragma code_seg(".XPPCODE")

EXTERNUSB VOID MU_AddDevice(IN IUsbDevice *Device)
/*++
    Routine Description:
        Standard USB class driver AddDevice routine.

        Verifies that MU is supported.

        Claims a preallocated MU_INSTANCE, and caches
        enumtime information needed later.
        
        Connects the IUsbDevice to the MU_INSTANCE.

    Arguments:
        Device - Interface to USB core stack for device.

    Notes:
        AddDevice routines are always called at DPC level.
--*/
{
    PMU_INSTANCE      muInstance;
    ULONG             port;
    ULONG             slot;
	const USB_ENDPOINT_DESCRIPTOR *endpointDescriptor;

    USB_DBG_ENTRY_PRINT(("MU_AddDevice(Device=0x%0.8x)", Device));

    //
    //  Get the device port and slot
    //

    port = Device->GetPort();
    if(port >= 16)
    {
        port -= 16;
        slot = 1;
    } else
    {
        slot = 0;
    }
    USB_DBG_TRACE_PRINT(("Memory Unit Added: Port = %d, Slot = %d", port, slot));

    //
    //  Choose the correct MU instance object
    //

    muInstance = &MU_DriverExtension.Nodes[NODE_INDEX_FROM_PORT_AND_SLOT(port,slot)];
    ASSERT(1==++muInstance->AddRemoveCount);
    //
    //	get the endpoint descriptor for bulk-IN
    //

    endpointDescriptor = Device->GetEndpointDescriptor(USB_ENDPOINT_TYPE_BULK, TRUE, 0);
    if(endpointDescriptor)
    {

        muInstance->BulkInEndpointAddress = endpointDescriptor->bEndpointAddress;
        
        //
        //  Verify wMaxPacketSize, for Xbox performance reasons, we require
        //  MU_MAX_BULK_PACKET_SIZE (64 bytes).  Furthermore, assumptions are
        //  made regarding max packet size based on 64 bytes.  So this really
        //  is a requirement.
        //

        if (endpointDescriptor->wMaxPacketSize != MU_MAX_BULK_PACKET_SIZE)
        {
            USB_DBG_WARN_PRINT(("unsupported device: wMaxPacketSize is incorrect for the bulk-IN endpoint."));
            Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
            return;
        }

    } else
    {
        USB_DBG_WARN_PRINT(("unsupported device: endpoint descriptor not found for bulk-IN."));
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    //
    //	get the endpoint descriptor for bulk-OUT
    //
    
    endpointDescriptor = Device->GetEndpointDescriptor(USB_ENDPOINT_TYPE_BULK, FALSE, 0);
    if (endpointDescriptor)
    {
        muInstance->BulkOutEndpointAddress =  endpointDescriptor->bEndpointAddress;

        //
        //  Verify wMaxPacketSize, see comments a few lines above regarding wMaxPacketSize
        //  for the bulk-IN endpoint.
        //

        if (endpointDescriptor->wMaxPacketSize != MU_MAX_BULK_PACKET_SIZE)
        {
            USB_DBG_WARN_PRINT(("unsupported device: wMaxPacketSize is incorrect for the bulk-OUT endpoint."));
            Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
            return;
        }

    } else {
        USB_DBG_WARN_PRINT(("unsupported device: endpoint descriptor not found for bulk-OUT."));
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    //
    // Set our muInstance in our IUsbDevice
    //

    Device->SetExtension(muInstance);

    //
    // save our IUsbDevice pointer
    //

    muInstance->Device = Device;
    muInstance->InterfaceNumber = Device->GetInterfaceNumber();

    //
    // set the class specific type so that it gets enumerated to applications.
    //

    Device->SetClassSpecificType(0);
    Device->AddComplete(USBD_STATUS_SUCCESS);
}

EXTERNUSB VOID
MU_RemoveDevice (
    IN IUsbDevice *Device
    )
/*++
    Routine Description:
        Standard USB class driver RemoveDevice routine.
        
        There are two possibilities: the device is in use (has a 
        DeviceObject attached and the endpoints are opened), or
        the device is not in use.  In the former case, we mark
        the device as DF_PENDING_REMOVE, and kick off the state
        machine to close the endpoints.  The last step of that state
        machine will see DF_PENDING_REMOVE and call Device->RemoveComplete()
        for us.  In the latter case, we can just call Device->RemoveComplete().

    Arguments:
        Device - Interface to USB core stack for device that is to be
                 removed.
    Notes:
        RemoveDevice routines are always called at DPC level.
--*/
{
    USB_DBG_ENTRY_PRINT(("MU_RemoveDevice(Device=0x%0.8x)", Device));

    PMU_INSTANCE           muInstance = (PMU_INSTANCE) Device->GetExtension();
    PMU_DEVICE_EXTENSION   deviceExtension = muInstance->DeviceExtension;
    ASSERT(0==--muInstance->AddRemoveCount);

    //
    //  If the MU_INSTANCE is currently attached to a device object
    //  for the current muInstance, then the endpoints are open and need to
    //  be closed before we can complete the remove.  However, it is possible
    //  that the deviceExtension is for a previous incarnation of this
    //  muInstance (a.k.a. the MU was removed, added, and now is being removed 
    //  again, without the device object ever having been closed).  In this
    //  bizarre circumstance, we can just complete the remove.
    //
    if(deviceExtension && !TEST_FLAG(deviceExtension->DeviceFlags, DF_REMOVED))
    {
        //
        //  Mark the device remove pending and let the close
        //  endpoint state machine take care of the rest.
        //
        SET_FLAG(deviceExtension->DeviceFlags, DF_PENDING_REMOVE);
        MU_fCloseEndpoints(deviceExtension);
    } else
    //
    //  Otherwise, the device is not in use, we can just complete the
    //  remove right here.
    //
    {
        muInstance->Device->RemoveComplete();
        muInstance->Device = NULL;
    }
    
    USB_DBG_EXIT_PRINT(("exiting MU_RemoveDevice"));

}

//*****************************************************************************
// Implementation of XAPI Entry Points:
//  MU_CreateDeviceObject,
//  MU_CloseDeviceObject
//*****************************************************************************

NTSTATUS
MU_CreateDeviceObject(
    IN  ULONG            Port,
    IN  ULONG            Slot,
    IN  POBJECT_STRING   DeviceName
    )
/*++
    Routine Description:

        This routine is called to expose an MU as a device object.  It is called by XMountMU,
        and it also must be called to open a device with the RAW filesystem, which is used for
        formatting and some low-level tests.

        All endpoint handles are opened at them same time.

    Arguments:

        Port         - port of MU to open
        Slot         - slot of MU to open
        DeviceName   - name to give device object

    Notes:
        
        MU_CloseDeviceObject reverses MU_CreateDeviceObject
        freeing all memory.

        MU_CreateDeviceObject should only be called once on a particular port and slot.
--*/
{
    PMU_INSTANCE         muInstance;
    PMU_DEVICE_EXTENSION deviceExtension;
    NTSTATUS             status = STATUS_SUCCESS;
    USBD_STATUS          usbdStatus;
    KIRQL                oldIrql;

    USB_DBG_ENTRY_PRINT(("MU_CreateDeviceObject(Port=0x%0.8x, Slot=0x%0.8x)", Port, Slot));

    //
    //  ASSERT arguments.
    //

    ASSERT(Port < XGetPortCount());
    ASSERT((Slot == XDEVICE_TOP_SLOT) || (Slot == XDEVICE_BOTTOM_SLOT));

    //
    //  Choose the correct MU instance object
    //

    muInstance = &MU_DriverExtension.Nodes[NODE_INDEX_FROM_PORT_AND_SLOT(Port,Slot)];

    oldIrql = KeRaiseIrqlToDpcLevel();

    deviceExtension = MU_AcquireDeviceObject();

    if(NULL == deviceExtension)
    {
        KeLowerIrql(oldIrql);
        RIP("XMountMu() - exceeded number of devices requested with XInitDevices");
        return STATUS_NO_MEMORY;
    }

    //
    //  Verify that the device is connected.
    //
    if(!muInstance->Device)
    {
        MU_ReleaseDeviceObject(deviceExtension);
        KeLowerIrql(oldIrql);
        return STATUS_DEVICE_NOT_CONNECTED;
    }

    //
    //  Point the MU_INSTANCE to the device extension
    //  and vice versa.  Now the failure processing
    //  can survive the normal close processing, even
    //  if the reset of open fails.
    //
    ASSERT(NULL == muInstance->DeviceExtension);
    muInstance->DeviceExtension = deviceExtension;
    deviceExtension->MuInstance = muInstance;

    //
    //  Initialize the DPC object and mrb timeouts
    //

    KeInitializeDpc(&deviceExtension->Mrb.TimeoutDpcObject, MU_MrbTimeout, deviceExtension);
    KeInitializeTimer(&deviceExtension->Mrb.Timer);

    //
    //  Initialize the debug watchdog parameters
    //
    MU_DEBUG_INIT_WATCHDOG_PARAMETERS(deviceExtension);

    //
    //  Initialize the close event.
    //

    KeInitializeEvent(&deviceExtension->CloseEvent, NotificationEvent, FALSE);

    //
    // Open the default endpoint
    //
    
    USB_BUILD_OPEN_DEFAULT_ENDPOINT(&deviceExtension->Urb.OpenEndpoint);
    usbdStatus = muInstance->Device->SubmitRequest(&deviceExtension->Urb);
    if(USBD_SUCCESS(usbdStatus))
    {
        SET_FLAG(deviceExtension->DeviceFlags,DF_DEFAULT_ENDPOINT_OPEN);
    } else
    {
        status = IUsbDevice::NtStatusFromUsbdStatus(usbdStatus);
        USB_DBG_WARN_PRINT(("Failed to open the default endpoint for an MU: usbdStatus = 0x%0.8x", usbdStatus));
        goto SkipRemainingEndpointOpens;
    }

    //
    // Open the Bulk-IN endpoint
    //
    USB_BUILD_OPEN_ENDPOINT(
        &deviceExtension->Urb.OpenEndpoint,
        muInstance->BulkInEndpointAddress,
        USB_ENDPOINT_TYPE_BULK,
        MU_MAX_BULK_PACKET_SIZE,
        0 //interval unimportant for bulk endpoint 
        );
    usbdStatus = muInstance->Device->SubmitRequest(&deviceExtension->Urb);
    if(USBD_SUCCESS(usbdStatus))
    {
        SET_FLAG(deviceExtension->DeviceFlags,DF_BULK_IN_ENDPOINT_OPEN);
        deviceExtension->BulkInEndpointHandle = deviceExtension->Urb.OpenEndpoint.EndpointHandle;
    } else 
    {
        status = IUsbDevice::NtStatusFromUsbdStatus(usbdStatus);
        USB_DBG_WARN_PRINT(("Failed to open the bulk-IN endpoint for an MU: usbdStatus = 0x%0.8x", usbdStatus));
        goto SkipRemainingEndpointOpens;
    }

    //
    // Open the Bulk-OUT endpoint (if needed)
    //

    USB_BUILD_OPEN_ENDPOINT(
       &deviceExtension->Urb.OpenEndpoint,
       muInstance->BulkOutEndpointAddress,
       USB_ENDPOINT_TYPE_BULK,
       MU_MAX_BULK_PACKET_SIZE,
       0
       );
    usbdStatus = muInstance->Device->SubmitRequest(&deviceExtension->Urb);
    if(USBD_SUCCESS(usbdStatus))
    {
        SET_FLAG(deviceExtension->DeviceFlags,DF_BULK_OUT_ENDPOINT_OPEN);
        deviceExtension->BulkOutEndpointHandle =  deviceExtension->Urb.OpenEndpoint.EndpointHandle;
    } else
    {
        status = IUsbDevice::NtStatusFromUsbdStatus(usbdStatus);
        USB_DBG_WARN_PRINT(("Failed to open the bulk-OUT endpoint for an MU: usbdStatus = 0x%0.8x", usbdStatus));
    }

SkipRemainingEndpointOpens:

    KeLowerIrql(oldIrql);

    //
    //  If an error occured, the device may be "half" opened.
    //  MU_CloseDeviceObject is smart enough to close a partially
    //  opened device, so we will just call it.
    //
    if(NT_ERROR(status))
    {
        MU_CloseDeviceObject(Port, Slot);
    }  else
    {
        //generate the device object name in the caller's buffer
        DeviceName->Length = (sizeof(OTEXT("\\Device\\MU_0"))/sizeof(OCHAR))-1;
        ASSERT(DeviceName->MaximumLength-1 >= DeviceName->Length);
        ASSERT(deviceExtension->DeviceObjectNumber < 16);
        soprintf(DeviceName->Buffer, OTEXT("\\Device\\MU_%x"), deviceExtension->DeviceObjectNumber);
    }

    USB_DBG_EXIT_PRINT(("MU_CreateDeviceObject returning 0x%0.8x" status));
    return status;
}

VOID
MU_CloseDeviceObject(
    IN  ULONG  Port,
    IN  ULONG  Slot
    )
/*++
    Routine Description:
        Reverse MU_CreateDeviceObject.

    Argument:
        DeviceObject - MU DeviceObject to close.
--*/
{
    PMU_DEVICE_EXTENSION deviceExtension;
    PMU_INSTANCE         muInstance;
    KIRQL                oldIrql;

    USB_DBG_ENTRY_PRINT(("MU_CloseDeviceObject(Port=%d,Slot=%d)", Port, Slot));

    ASSERT_LESS_THAN_DISPATCH_LEVEL();

    //
    //  ASSERT arguments.
    //

    ASSERT(Port < XGetPortCount());
    ASSERT((Slot == XDEVICE_TOP_SLOT) || (Slot == XDEVICE_BOTTOM_SLOT));

    //
    //  Choose the correct MU instance object
    //

    muInstance = &MU_DriverExtension.Nodes[NODE_INDEX_FROM_PORT_AND_SLOT(Port,Slot)];

    //
    //  Raise Irql to synchronize
    //

    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Get the deviceExtension from the muInstance.
    //

    deviceExtension = muInstance->DeviceExtension;

    //
    //  Hitting assert probably indicates that MU_CloseDeviceObject was called
    //  for an MU that was already closed.  Since this is not a
    //  public API, we just ASSERT.  Fix the calling code!
    //

    ASSERT(deviceExtension);

    //
    //  If the device extension points back to an active MU_INSTANCE behind this device object,
    //  close the endpoints on it first.
    //

    if(deviceExtension->MuInstance)
    {
        //
        //  Mark the device as pending close
        //
        SET_FLAG(deviceExtension->DeviceFlags, DF_PENDING_CLOSE);

        //
        //  Let the close endpoint state machine finish up the work of closing.
        //
        MU_fCloseEndpoints(deviceExtension);

        //
        //  Lower irql for synchronization purposes.
        //
        KeLowerIrql(oldIrql);

        //
        //  Wait for the close endpoint state machine to finish
        //
        KeWaitForSingleObject(&deviceExtension->CloseEvent, Executive, KernelMode, FALSE, NULL);
        
    } else
    {
        muInstance->DeviceExtension = NULL;
        KeLowerIrql(oldIrql);
    }
    //
    //  The MU instance can now forget about the device object.
    //
    MU_ReleaseDeviceObject(deviceExtension);

    USB_DBG_EXIT_PRINT(("MU_CloseDeviceObject returning."));
    return;
};

PDEVICE_OBJECT
MU_GetExistingDeviceObject(
    IN  ULONG  Port,
    IN  ULONG  Slot
    )
{
    PMU_INSTANCE         muInstance;

    USB_DBG_ENTRY_PRINT(("MU_GetExistingDeviceObject(Port=%d,Slot=%d)", Port, Slot));

    //
    //  ASSERT arguments.
    //

    ASSERT(Port < XGetPortCount());
    ASSERT((Slot == XDEVICE_TOP_SLOT) || (Slot == XDEVICE_BOTTOM_SLOT));

    //
    //  Choose the correct MU instance object
    //

    muInstance = &MU_DriverExtension.Nodes[NODE_INDEX_FROM_PORT_AND_SLOT(Port,Slot)];

    //
    //  This routine should only be invoked for a MU object that has been
    //  created.  The caller is responsible for synchronizing calls to create
    //  and close MUs with this routine.
    //

    ASSERT(muInstance->DeviceExtension);

    return muInstance->DeviceExtension->DeviceObject;
}

//*****************************************************************************
//  Implementation of local functions:
//      MU_fCloseEndpoints
//      MU_CloseEndpointCompletion
//*****************************************************************************
VOID
FASTCALL
MU_fCloseEndpoints (
    IN PMU_DEVICE_EXTENSION   DeviceExtension
    )
/*++
    Routine Description:
        Enters the close endpoint state machine.

        It is protected against reentrancy, by checking
        a flag.

        If an MU_fOpenEndpoints is in progress, it signals
        that MU_fOpenEndpoints should call it when it
        syncs up its new status.
--*/
{
    ASSERT_DISPATCH_LEVEL();

    //
    // If a close is not already in progress, start one
    //

    if(!TEST_FLAG(DeviceExtension->DeviceFlags, DF_CLOSING_ENDPOINTS))
    {
       SET_FLAG(DeviceExtension->DeviceFlags,DF_CLOSING_ENDPOINTS);
       MU_CloseEndpointCompletion(NULL, DeviceExtension);
    }

    return;
}

VOID
MU_CloseEndpointCompletion (
    PURB Urb,
    PVOID Context
    )
/*++
    Routine Description:

        This is the heart of the close endpoint state machine.
        Each time it is called, it looks for an endpoint that
        needs to be closed, and builds an URB to close it and
        return back to this routine when it is closed.  At the
        end of the routine, whichever URB was built is submitted.
        
        When all the endpoints are closed, it will get to the
        last else clause.  It will complete a remove if one
        was pending, and exit the close endpoint state machine,
        before the URB is submitted (since it build one).
    
--*/
{
    PMU_DEVICE_EXTENSION deviceExtension = (PMU_DEVICE_EXTENSION) Context;
    PMU_INSTANCE muInstance = deviceExtension->MuInstance;
    
    ASSERT_DISPATCH_LEVEL();

    if(TEST_FLAG(deviceExtension->DeviceFlags, DF_BULK_OUT_ENDPOINT_OPEN))
    //
    //  The Bulk-OUT endpoint needs to be closed.
    //
    {
        //
        //  Build the URB.
        //

        USB_BUILD_CLOSE_ENDPOINT(
          &deviceExtension->CloseEndpointUrb,
          deviceExtension->BulkOutEndpointHandle,
          MU_CloseEndpointCompletion,
          deviceExtension
          );

        //
        //  Mark the endpoint closed
        //

        deviceExtension->BulkOutEndpointHandle = 0;
        CLEAR_FLAG(deviceExtension->DeviceFlags,DF_BULK_OUT_ENDPOINT_OPEN);

    } else if(TEST_FLAG(deviceExtension->DeviceFlags, DF_BULK_IN_ENDPOINT_OPEN))
    //
    //  The Bulk-IN endpoint needs to be closed.
    //
    {
        //
        //  Build the URB.
        //

        USB_BUILD_CLOSE_ENDPOINT(
          &deviceExtension->CloseEndpointUrb,
          deviceExtension->BulkInEndpointHandle,
          MU_CloseEndpointCompletion,
          deviceExtension
          );

        //
        //  Mark the endpoint closed
        //

        deviceExtension->BulkInEndpointHandle = 0;
        CLEAR_FLAG(deviceExtension->DeviceFlags,DF_BULK_IN_ENDPOINT_OPEN);

    } else if(TEST_FLAG(deviceExtension->DeviceFlags, DF_DEFAULT_ENDPOINT_OPEN))
    //
    //  The default endpoint needs to be closed.
    //
    {
        //
        //  Build the URB.
        //

        USB_BUILD_CLOSE_DEFAULT_ENDPOINT(
          &deviceExtension->CloseEndpointUrb,
          MU_CloseEndpointCompletion,
          deviceExtension
          );

        //
        //  Mark the endpoint closed
        //

        CLEAR_FLAG(deviceExtension->DeviceFlags, DF_DEFAULT_ENDPOINT_OPEN);

    } else
    //
    // Our state machine is done closing endpoints
    //
    {

        //
        //  We are no longer closing endpoints.
        //
        CLEAR_FLAG(deviceExtension->DeviceFlags, DF_CLOSING_ENDPOINTS);

        //
        //  This state machine has been invoked because
        //  a remove and\or close is pending.  In either
        //  case, the device object needs to forget about
        //  the MU_INSTANCE, but NOT vice versa.  The MuInstance
        //  only forgets about the device object on close.
        //
        deviceExtension->MuInstance = NULL;

        //
        //  If a close is pending, signal the close event
        //  so the close caller that started the state machine
        //  can finish up its processing.
        //
        if(TEST_FLAG(deviceExtension->DeviceFlags, DF_PENDING_CLOSE))
        {
            //
            //  We can forget about the device object now
            //  as it will be deleted by MU_CloseDeviceObject
            //  when we signal the close event.
            //
            muInstance->DeviceExtension = NULL;
            KeSetEvent(&deviceExtension->CloseEvent, IO_NO_INCREMENT, FALSE);
            CLEAR_FLAG(deviceExtension->DeviceFlags, DF_PENDING_CLOSE);
        }

        //
        //  If a remove is pending, then process it.
        //

        if(TEST_FLAG(deviceExtension->DeviceFlags, DF_PENDING_REMOVE))
        {
            //
            //  if remove was pending, call remove complete
            //

            muInstance->Device->RemoveComplete();
            muInstance->Device = NULL;

            //
            //  it is now removed, not remove pending.
            //

            CLEAR_FLAG(deviceExtension->DeviceFlags, DF_PENDING_REMOVE);
            SET_FLAG(deviceExtension->DeviceFlags, DF_REMOVED);
        }
        
        //
        //  We are done, so return before submitting the URB
        //

        return;
    }

    //
    //  If we are here, we have built an URB, submit it.
    //

    muInstance->Device->SubmitRequest((PURB)&deviceExtension->CloseEndpointUrb);
    
    return;
}
