/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    xid.cpp

Abstract:
    
    Basic entry point implementation of the XID driver.
    
    
Environment:

    Designed for XBOX.

Notes:

    
    
Revision History:

    02-21-00 created by Mitchell Dernis (mitchd)

--*/
#define XID_IMPLEMENTATION

//
//  Pull in OS headers
//
#define _XAPI_
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xboxp.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          '_DIX'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("XID");

//
//  Pull in public usb headers
//
#include <usb.h>
//
//  Pull in xid headers
//
#include "xid.h"

//------------------------------------------------------------------------------
//  Declare XID's global variables.
//------------------------------------------------------------------------------
XID_GLOBALS XID_Globals;

//------------------------------------------------------------------------------
//  Forward declaration of locally defined functions
//------------------------------------------------------------------------------
VOID
XID_WatchdogTimerProc(
    PKDPC,
    PVOID,
    PVOID,
    PVOID
    );

VOID
XID_EnumStage1(
    PURB Urb,
    PXID_DEVICE_NODE XidNode
    );

VOID
FASTCALL
XID_fRemoveDeviceComplete(
    IN PXID_DEVICE_NODE XidNode
    );

PXID_DEVICE_NODE
FASTCALL
XID_fFindNode(
    IN PXID_TYPE_INFORMATION TypeInformation,
    IN ULONG  Port
    );

USBD_STATUS
FASTCALL
XID_fOpenEndpoints(
    PXID_OPEN_DEVICE OpenDevice,
    PXINPUT_POLLING_PARAMETERS PollingParameters
    );

VOID
FASTCALL
XID_fCloseEndpoints(
    PXID_OPEN_DEVICE OpenDevice
    );

VOID 
XID_CloseEndpointStateMachine(
    PURB_CLOSE_ENDPOINT CloseUrb,
    PXID_OPEN_DEVICE OpenDevice
    );

VOID
XID_NewInterruptData(
    PURB                Urb,
    PXID_OPEN_DEVICE    OpenDevice
    );

VOID
XID_ClearInputStallComplete(
    PURB                Urb,
    PXID_OPEN_DEVICE    OpenDevice
    );

VOID
XID_OutputComplete(
    PURB                      Urb,
    PXINPUT_FEEDBACK_INTERNAL  OutputReport
    );

VOID
FASTCALL
XID_fOutputComplete1(
    PURB Urb,
    PXINPUT_FEEDBACK_INTERNAL OutputReport
    );

VOID
XID_ClearOutputStallComplete(
    PURB                     Urb,
    PXINPUT_FEEDBACK_INTERNAL OutputReport
    );

VOID XID_EnumLegacy1(PURB Urb, PXID_DEVICE_NODE XidNode);
//VOID XID_EnumLegacy2(PURB Urb, PXID_DEVICE_NODE XidNode);
VOID XID_EnumKeyboard(PURB Urb, PXID_DEVICE_NODE XidNode);
VOID XID_EnumKeyboardComplete(PURB Urb, PXID_DEVICE_NODE XidNode);
PXID_KEYBOARD_SERVICES XID_pKeyboardServices = NULL;

//------------------------------------------------------------------------------
//  Declare XID types and class.
//------------------------------------------------------------------------------

// Normally you would use macros for the device type table.
// However, our types are dynamic, so we just want no types,
// during init, we can enumerate our types and then update the tables.
// USBD won't touch this entry until the first device is AddComplete.
#define XID_Types NULL
USB_CLASS_DRIVER_DECLARATION(XID_, XBOX_DEVICE_CLASS_INPUT_DEVICE, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION_DUPLICATE(XID_, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, 0xFF, 0xFF)
#undef XID_Types

#pragma data_seg(".XPP$ClassXID")
USB_CLASS_DECLARATION_POINTER(XID_)
USB_CLASS_DECLARATION_POINTER_DUPLICATE(XID_, 1)
#pragma data_seg(".XPP$Data")

USB_RESOURCE_REQUIREMENTS XID_gResourceRequirements = 
    {USB_CONNECTOR_TYPE_DIRECT, 0, 0, 1, 0, 2, 10, 0, 0, 0};

#define DEVICE_TYPE_STACK_SIZE 20
#pragma code_seg(".XPPCINIT")
EXTERNUSB VOID XID_Init(IUsbInit *UsbInit)
/*++
    Routine Description:
        Called at boot.  Registers with usbpnp.sys.  Intializes
        globals.
--*/
{
    ULONG handleCount = 0;
    ULONG index;
    
    PXPP_DEVICE_TYPE  devicesTypes[DEVICE_TYPE_STACK_SIZE];
    BOOL fUseDefaultCount;
    PXID_TYPE_INFORMATION *ppTypeInformation;
    DWORD dwTypeIndex = 0;
    
    //
    //  Verify that XOUTPUT_SIZE_OF_INTERNAL_HEADER is defined correctly
    //  in input.h
    //
    C_ASSERT(sizeof(XINPUT_FEEDBACK_HEADER_INTERNAL) == XINPUT_FEEDBACK_HEADER_INTERNAL_SIZE);

    //Sentry to protect against double init.  This happens because we support XID and HID
    //class as the interface class.  This support is here to stay.
    static BOOL fInitialized = FALSE;
    if(fInitialized) return;
    fInitialized = TRUE;

    //
    //  Figure out how many nodes we need
    //
    XID_Globals.DeviceNodeCount = XGetPortCount()*XID_MAX_DEVICE_PER_PORT;

    //
    //  Walk the Device Type Table (and properly register each type)
    //
    fUseDefaultCount = UsbInit->UseDefaultCount();
    ppTypeInformation = (PXID_TYPE_INFORMATION *)((&XID_BeginTypeDescriptionTable)+1);
    while( (ULONG_PTR)ppTypeInformation < (ULONG_PTR)&XID_EndTypeDescriptionTable )
    {
        if(*ppTypeInformation)
        {
            // Recorder the XPP type in the temporary table.
            ASSERT(dwTypeIndex < DEVICE_TYPE_STACK_SIZE);
            devicesTypes[dwTypeIndex++] = (*ppTypeInformation)->XppType;

            // Count how many  handles we need, and record the user's
            // XInitDevices choice for count
            if(!fUseDefaultCount)
            {
                (*ppTypeInformation)->bRemainingHandles = 
                    UsbInit->GetMaxDeviceTypeCount((*ppTypeInformation)->XppType);
                RIP_ON_NOT_TRUE_WITH_MESSAGE(
                    (*ppTypeInformation)->bRemainingHandles <= XGetPortCount(),
                    "XInitDevices: requested more XDEVICE_TYPE_GAMEPAD than available ports.");
            }
            handleCount += (*ppTypeInformation)->bRemainingHandles;
        }
        ppTypeInformation++;
    }
    
    //
    //  In any case, we don't need more handles than we have device nodes.
    //
    if(handleCount > XID_Globals.DeviceNodeCount)
    {
        handleCount = XID_Globals.DeviceNodeCount;
    }

    //
    //  Allocate a block for the device type table, device nodes and handles
    //  
    ULONG allocSize = sizeof(PXPP_DEVICE_TYPE)*dwTypeIndex +
                      sizeof(XID_DEVICE_NODE)*XID_Globals.DeviceNodeCount +
                      sizeof(XID_OPEN_DEVICE)*handleCount;
    ULONG_PTR memory = (ULONG_PTR) RTL_ALLOCATE_HEAP(allocSize);
    ASSERT(memory);

    //
    //  Fix our device type table
    //
    XID_Description.DeviceTypeCount = dwTypeIndex;
    XID_Description.DeviceTypes = (PXPP_DEVICE_TYPE *)memory;
    XID_1Description.DeviceTypeCount = dwTypeIndex;  //The duplicate entry, for HID
    XID_1Description.DeviceTypes = (PXPP_DEVICE_TYPE *)memory;
    memory += (sizeof(PXPP_DEVICE_TYPE)*dwTypeIndex);
    memcpy(XID_Description.DeviceTypes, devicesTypes, sizeof(PXPP_DEVICE_TYPE)*dwTypeIndex);
    
    
    //
    //  Create a free list of handles
    //
    XID_Globals.DeviceHandles = NULL;
    PXID_OPEN_DEVICE openDevices = (PXID_OPEN_DEVICE) memory;
    for(index=0; index < handleCount; index++)
    {
        XID_FreeHandle(openDevices++);
    }

    //
    //  Initialize the open device nodes
    //
    XID_Globals.DeviceNodes = (PXID_DEVICE_NODE)openDevices;
    XID_Globals.DeviceNodeInUseCount = 0;
    for(index = 0; index < XID_Globals.DeviceNodeCount; index++)
    {
        XID_Globals.DeviceNodes[index].InUse = FALSE;
    }

    //
    //  Register our resources
    //
    XID_gResourceRequirements.MaxDevices = handleCount;
    UsbInit->RegisterResources(&XID_gResourceRequirements);

    //
    //  Initialize other globals.
    //
    KeInitializeTimer(&XID_Globals.EnumWatchdogTimer);
	return;
}
#pragma code_seg(".XPPCODE")

__inline void XID_SetEnumWatchdog()
/*++
    Routine Description:
        Called to start a 5 second watchdog timer on transfers
        during device enumeration.
--*/
{
    LARGE_INTEGER wait;
    wait.QuadPart = -50000000;
    KeSetTimer(&XID_Globals.EnumWatchdogTimer,wait,&XID_Globals.EnumWatchdogTimerDpc);
}

__inline void XID_ClearEnumWatchdog()
/*++
    Routine Description:
        Called to cancel a 5 second watchdog timer when a transfer during
        enumeration completes.
--*/
{
    KeCancelTimer(&XID_Globals.EnumWatchdogTimer);
}

VOID XID_WatchdogTimerProc(PKDPC, PVOID pvDevice, PVOID, PVOID)
/*++
    Routine Description:
        DPC routine called if a 5 second watchdog timer expires protecting
        asynchronous transfers during enumeration.  The routine forces completion
        of the URB by canceling it.  This will cause enumeration to fail,
        but avoids hanging USB device enumeration system wide.
--*/
{
    IUsbDevice *device = (IUsbDevice *)pvDevice;
    USB_DBG_ERROR_PRINT(("Enumeration Watchdog Timer has expired.\n"));
    device->CancelRequest(&XID_Globals.EnumUrb);
}

EXTERNUSB VOID
XID_AddDevice(
    IN IUsbDevice *Device
    )
/*++
    Routine Description:
        This routine is registered as the AddDevice routine for new XID devices.
        It performs the following tasks:
            1) Allocates a XidNode from the static pool of nodes.
            2) Begins the XID enumeration provcess.
        Enumeration is continued in XID_EnumStage1.
--*/
{
    UCHAR                          nodeIndex = 0;
    PXID_DEVICE_NODE               xidNode;
    const USB_ENDPOINT_DESCRIPTOR *endpointDescriptor;
	        
    USB_DBG_ENTRY_PRINT(("Entering XID_AddDevice.\n"));
    if(XID_Globals.DeviceNodeInUseCount < XID_Globals.DeviceNodeCount)
    {
        //
        //  Find a free node, and claim it.
        //
        while(XID_Globals.DeviceNodes[nodeIndex].InUse) nodeIndex++;
        XID_Globals.DeviceNodeInUseCount++;
        XID_Globals.EnumNode = nodeIndex;
        xidNode = XID_Globals.DeviceNodes + nodeIndex;
		Device->SetExtension((PVOID)xidNode);
		xidNode->Device = Device;

        //
        //  Initialize what we can about the device.
        //
        xidNode->InUse = TRUE;
        xidNode->Ready = FALSE;
        xidNode->PendingRemove = FALSE;
        xidNode->Opened = FALSE;
        xidNode->InterfaceNumber = Device->GetInterfaceNumber();
        xidNode->OpenDevice = NULL;
        
    	//
		//	Get the interrupt-in endpoint descriptor  (NOT OPTIONAL).
		//
		endpointDescriptor = Device->GetEndpointDescriptor(USB_ENDPOINT_TYPE_INTERRUPT, TRUE, 0);
		ASSERT(endpointDescriptor);
        xidNode->EndpointAddressIn = endpointDescriptor->bEndpointAddress;
        xidNode->MaxPacketIn = (UCHAR)endpointDescriptor->wMaxPacketSize;
		
		//
		//	Get the interrupt-out endpoint (OPTIONAL)
		//
		endpointDescriptor = Device->GetEndpointDescriptor(USB_ENDPOINT_TYPE_INTERRUPT, FALSE, 0);
		if(endpointDescriptor)
		{
			xidNode->EndpointAddressOut = endpointDescriptor->bEndpointAddress;
			xidNode->MaxPacketOut = (UCHAR)endpointDescriptor->wMaxPacketSize;
		} else
		{
            xidNode->EndpointAddressOut = 0;
			xidNode->MaxPacketOut = 0;
		}
        //
        //  Get XID Descriptor
        //
        USB_BUILD_CONTROL_TRANSFER(
            (PURB_CONTROL_TRANSFER)&XID_Globals.EnumUrb,
            NULL, //Default endpoint
            (PVOID)&XID_Globals.EnumDeviceDescriptor,
            sizeof(XID_DESCRIPTOR),
            USB_TRANSFER_DIRECTION_IN,
            (PURB_COMPLETE_PROC)XID_EnumStage1,
            (PVOID)xidNode,
            TRUE,
            (USB_DEVICE_TO_HOST | USB_VENDOR_COMMAND | USB_COMMAND_TO_INTERFACE),
            USB_REQUEST_GET_DESCRIPTOR,
            XID_DESCRIPTOR_TYPE,
            xidNode->InterfaceNumber,
            sizeof(XID_DESCRIPTOR)
            );
        //
        //  Reinitialize the watchdog DPC, so that the context is our device
        //
        KeInitializeDpc(&XID_Globals.EnumWatchdogTimerDpc, XID_WatchdogTimerProc, (PVOID)Device);
        XID_SetEnumWatchdog();
        Device->SubmitRequest(&XID_Globals.EnumUrb);
    }
    else
    {
        //
        //  Out of nodes so we cannot support the device
        //  (allow retry)
        //
        ASSERT(FALSE);
        Device->AddComplete(USBD_STATUS_NO_MEMORY);
    }
    USB_DBG_EXIT_PRINT(("Exiting XID_AddDevice.\n"));
}

VOID
XID_EnumStage1(
    PURB                Urb,
    PXID_DEVICE_NODE    XidNode
    )
/*++
    Routine Description:
        Completion routine for obtaining the XID descriptor started
        in AddDevice.
        
        If retriving the XID_DESCRIPTOR failed,
--*/
{
    XID_ClearEnumWatchdog();
    //
    //  Getting the XID Descriptor may have failed.  This probably
    //  means it is a legacy device.
    //
    if(USBD_ERROR(Urb->Header.Status))
    {
        USB_DBG_EXIT_PRINT(("Couldn't get XID descriptor, could be a HID keyboard.\n"));
        XID_EnumLegacy1(Urb, XidNode);
        return;
    }

    //
    //  Copy the necessary info out of the Xid Descriptor
    //
    XidNode->TypeInformation = GetTypeInformation(XID_Globals.EnumXidDescriptor.bType, &XidNode->TypeIndex);
    XidNode->SubType = XID_Globals.EnumXidDescriptor.bSubType;
    XidNode->bMaxInputReportSize = XID_Globals.EnumXidDescriptor.bMaxInputReportSize;
    XidNode->bMaxOutputReportSize = XID_Globals.EnumXidDescriptor.bMaxOutputReportSize;
    
    //
    //  Check a number of items to determine if we can support the device.
    //      1) The type must be one known at compile time
    //      2) The report size must be at least as large as the XID_REPORT_HEADER.
    //      3) The maximum report size must not be larger than our buffer (XID_MAXIMUM_REPORT_SIZE)
    //         this is a limit on the extensibility of XID.
    //      4) For performance and resource management reasons we enforce that
    //         the entire report fit in a single interrupt-IN packet (there is
    //         no such requirement on the control pipe).
    //      5) Same for output, but only if interrupt OUT is supported.
    //
    if(
        (!XidNode->TypeInformation) ||
        (XID_REPORT_HEADER > XID_Globals.EnumXidDescriptor.bMaxInputReportSize) ||
        (XID_MAXIMUM_REPORT_SIZE < XID_Globals.EnumXidDescriptor.bMaxInputReportSize) ||
        (XidNode->bMaxInputReportSize > XidNode->MaxPacketIn) ||
        (XidNode->EndpointAddressOut && (XidNode->bMaxOutputReportSize > XidNode->MaxPacketOut))
    )
    {
        USB_DBG_WARN_PRINT(("Unsupported XID Device"));
        XidNode->Device->SetExtension(NULL);
        XidNode->Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
    	XidNode->Device=NULL;
        XidNode->InUse = FALSE;
        XID_Globals.DeviceNodeInUseCount--;
        return;
    }

    //
    //  Set the class specific type and call AddComplete.
    //

    XidNode->Device->SetClassSpecificType(XidNode->TypeIndex);
    XidNode->Device->AddComplete(USBD_STATUS_SUCCESS);
    XidNode->Ready = TRUE;
}

EXTERNUSB VOID
XID_RemoveDevice(
    IUsbDevice *Device
    )
/*++
    Routine Description:
        Called by usbd when a device is removed.  It kicks off out
        remove sequence.
--*/
{
    PXID_DEVICE_NODE xidNode = (PXID_DEVICE_NODE)Device->GetExtension();
    ASSERT(NULL != xidNode);
    
	ASSERT_DISPATCH_LEVEL();
    //
    //  Mark the Node remove pending.
    //
    xidNode->PendingRemove = TRUE;
    //
    //  If there is an open handle
    //  kick off a close.
    //
    if(xidNode->OpenDevice)
    {
        //
        // Notify keyboard services of device removal.
        //
        #ifdef DEBUG_KEYBOARD
        if((XDEVICE_TYPE_DEBUG_KEYBOARD==xidNode->TypeInformation->XppType) && XID_pKeyboardServices)
        {
          XID_pKeyboardServices->pfnRemove(xidNode->OpenDevice);
        }
        #endif DEBUG_KEYBOARD
        XID_fCloseEndpoints(xidNode->OpenDevice);
    } else
    //
    //  Otherwise, we can wrap up the remove.
    //
    {
        XID_fRemoveDeviceComplete(xidNode);        
    }
}

VOID
FASTCALL
XID_fRemoveDeviceComplete(
    IN PXID_DEVICE_NODE XidNode
    )
/*++
    Routine Description:
        This routine is called by XID_DereferenceNode, when
        the reference count on a XID node goes to zero.
        At that time, it is certain that all open endpoints
        are closed, and all outstanding I/O is complete.
--*/
{
    ASSERT_DISPATCH_LEVEL();
    //
    //  We are really done
    //
    USB_DBG_TRACE_PRINT(("Freeing node(0x%0.8x) nodeIndex = %d\n", XidNode, XidNode - XID_Globals.DeviceNodes));
    ASSERT(XidNode->PendingRemove);
	XidNode->Device->SetExtension(NULL);
	XidNode->Device->RemoveComplete();
	XidNode->Device=NULL;
    XidNode->InUse = FALSE;
    XID_Globals.DeviceNodeInUseCount--;
    return;
}

PXID_DEVICE_NODE
FASTCALL
XID_fFindNode(
    IN PXID_TYPE_INFORMATION TypeInformation,
    IN ULONG  Port
    )
/*++
    Routine Description:
        Finds a XID node that corresponds to the type and port (port includes port and slot).
        Will only return nodes that are ready to be open.
    Return Value:
        If a suitable node is found, it is returned.  Otherwise, NULL is returned.
    Environement:
        Assumes DISPATCH_LEVEL.
--*/
{
    UCHAR xidNodeIndex;
    PXID_DEVICE_NODE xidNode = NULL;
    //
    //  Brute force search of all the nodes.
    //
    for(xidNodeIndex = 0; xidNodeIndex < XID_Globals.DeviceNodeCount; xidNodeIndex++)
    {
        if(
            XID_Globals.DeviceNodes[xidNodeIndex].InUse &&
            (XID_Globals.DeviceNodes[xidNodeIndex].Device->GetPort() == Port) &&
            (XID_Globals.DeviceNodes[xidNodeIndex].TypeInformation == TypeInformation) &&
            XID_Globals.DeviceNodes[xidNodeIndex].Ready &&
            !XID_Globals.DeviceNodes[xidNodeIndex].PendingRemove
        ){
            xidNode =  XID_Globals.DeviceNodes + xidNodeIndex;
        }
    }
    return xidNode;
}

DWORD
FASTCALL
XID_fOpenDevice(
    PXID_TYPE_INFORMATION TypeInformation,
    ULONG                 Port,
    PXID_OPEN_DEVICE      *OpenDevice,
    PXINPUT_POLLING_PARAMETERS PollingParameters
    )
/*++
    Given a XidTy creates an OpenDevice,
    initializes the OpenDevice and opens
    the endpoints so the device can be used.
--*/
{
    
    USBD_STATUS         status;
    PXID_OPEN_DEVICE    openDevice;
    PXID_DEVICE_NODE    xidNode;
    KIRQL               oldIrql;
    DWORD               errorCode = ERROR_SUCCESS;
    BOOL                partiallyOpen = FALSE;

    //
    // By default we want to return NULL.
    //
    *OpenDevice = NULL;

    //
    //  Must be done at high Irql
    //
    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Find the node
    //
    xidNode = XID_fFindNode(TypeInformation, Port);
    if(!xidNode)
    {
        errorCode = ERROR_DEVICE_NOT_CONNECTED;
        goto exit_open_device;
    }

    //
    //  Make sure that it is not already open
    //
    if(xidNode->OpenDevice)
    {
        errorCode = ERROR_SHARING_VIOLATION;
        goto exit_open_device;
    }

    
    if(0==TypeInformation->bRemainingHandles)
    {
        errorCode = ERROR_OUTOFMEMORY;
        goto exit_open_device;        
    }
    
    //
    //  Allocate memory for the device handle
    //
    
    TypeInformation->bRemainingHandles--;
    openDevice = XID_AllocateHandle();
    
    //
    //  Initialize the basic stuff
    //
    RtlZeroMemory(openDevice, sizeof(XID_OPEN_DEVICE));
    openDevice->XidNode = xidNode;
    openDevice->TypeInformation = TypeInformation;
    openDevice->AutoPoll = PollingParameters->fAutoPoll ? TRUE : FALSE;
    
    //
    //  Tie the xidNode to our handle.
    //
    xidNode->OpenDevice = openDevice;

    //
    //  Any failure after here, requires closing the device.
    //
    partiallyOpen = TRUE;

    //
    //  Open the endpoints
    //

    status = XID_fOpenEndpoints(openDevice, PollingParameters);

    //
    //  If the endpoints were successfully opened
    //  then setup the initial state.
    //
    if(USBD_SUCCESS(status))
    {
        //
        //  Initialize the Report and the Report for URB
        //  with the default values;
        //
        RtlCopyMemory(
            (PVOID)(openDevice->ReportForUrb+XID_REPORT_HEADER),
            TypeInformation->pInputReportInfoList[0].pDefaultValues,
            TypeInformation->pInputReportInfoList[0].bCurrentSize
            );
        RtlCopyMemory(
            (PVOID)openDevice->Report,
            (PVOID)(openDevice->ReportForUrb+XID_REPORT_HEADER),
            sizeof(openDevice->Report)
            );

        //
        //  Initialize an event for synchronization
        //
        KEVENT   event;
        KeInitializeEvent(&event, SynchronizationEvent, FALSE);
    
        //
        //  Build a control transfer
        //
        WORD wReportTypeAndId = 0x0100;
        ULONG ulBufferSize = xidNode->bMaxInputReportSize;
        USB_BUILD_CONTROL_TRANSFER(
            &openDevice->Urb.ControlTransfer,
            NULL, //Default endpoint
            openDevice->ReportForUrb,
            ulBufferSize,
            USB_TRANSFER_DIRECTION_IN,
            (PURB_COMPLETE_PROC)XID_SyncComplete, //do it synchronously
            &event,  //the context is the event.
            TRUE,
            (USB_DEVICE_TO_HOST | USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE),
            XID_COMMAND_GET_REPORT,
            wReportTypeAndId, //Input report 0
            xidNode->InterfaceNumber,
            ulBufferSize
            );

        //
        //  Submit the request
        //
        xidNode->Device->SubmitRequest(&openDevice->Urb);
    
        //
        //  Lower IRQL, and wait for transfer to complete.
        //  Then restore Irql to DPC level
        //
        KeLowerIrql(oldIrql);
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        oldIrql = KeRaiseIrqlToDpcLevel();

        //
        //  During the transfer the device may have been removed,
        //  if so we should check to make sure that the device is
        //  still with us.  There are two cases here: the device is
        //  totally gone, the device is still pending remove.
        //
    
        if(NULL == openDevice->XidNode || xidNode->PendingRemove)
        {   
            errorCode = ERROR_DEVICE_NOT_CONNECTED;
            goto exit_open_device;
        } 

        //
        //  Process the GET_REPORT results, if it succeeded
        //
        if(USBD_SUCCESS(openDevice->Urb.Header.Status))
        {
            TypeInformation->pfnProcessNewData(openDevice);
        } 
#if DBG
        else
        //
        // Issue warning if GET_REPORT failed.
        //
	    { 
           if(USBD_STATUS_STALL_PID == openDevice->Urb.Header.Status)
           {
		       USB_DBG_WARN_PRINT(("Device in port %d (0 based) does not support GET_REPORT\n", Port));
           }
        }
#endif

        //
        // Notify keyboard services of new open device.
        //
        if((XDEVICE_TYPE_DEBUG_KEYBOARD==TypeInformation->XppType) && XID_pKeyboardServices)
        {
          XID_pKeyboardServices->pfnOpen(openDevice);
        }

        //
        //  Prepare the URB for the interrupt-in endpoint.
        //
        USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
            (PURB_BULK_OR_INTERRUPT_TRANSFER)&openDevice->Urb,
            openDevice->InterruptInEndpointHandle,
            openDevice->ReportForUrb,
            xidNode->bMaxInputReportSize,
            USB_TRANSFER_DIRECTION_IN,
            (PURB_COMPLETE_PROC)XID_NewInterruptData,
            (PVOID)openDevice,
            TRUE
            );

        //
        //  If autopoll is set, start polling
        //
        if(openDevice->AutoPoll)
        {
          xidNode->Device->SubmitRequest(&openDevice->Urb);
        }

        
        //
        //  If we are here, the device was successfully opened.
        //  copy the handle, and mark partiallyOpen as FALSE
        //  as we are now fully open.
        //
        *OpenDevice = openDevice;
        partiallyOpen = FALSE;
    }  else
    {
        errorCode = IUsbDevice::Win32FromUsbdStatus(status);
    }

exit_open_device:
    
    //
    //  Done with synchronization
    //
    KeLowerIrql(oldIrql);
    
    //
    //  Partially opened devices
    //  should be closed.
    //
    if(partiallyOpen)
    {
        XID_fCloseDevice(openDevice);
    }

    return errorCode;
}

USBD_STATUS
FASTCALL
XID_fOpenEndpoints(
    IN PXID_OPEN_DEVICE OpenDevice,
    PXINPUT_POLLING_PARAMETERS PollingParameters
    )
/*++
    Routine Description:
        Called from XID_OpenDevice to open the endpoints associated with the
        device.  Opening the endpoints references the XidNode until the endpoints
        are closed.
--*/
{
    
    USBD_STATUS status = USBD_STATUS_SUCCESS;
    PXID_DEVICE_NODE xidNode = OpenDevice->XidNode;

    //
    //  Open the control endpoint
    //
    
    USB_BUILD_OPEN_DEFAULT_ENDPOINT((PURB_OPEN_ENDPOINT)&OpenDevice->Urb);
    status = xidNode->Device->SubmitRequest(&OpenDevice->Urb);
    
    //
    //  If the control endpoint is open,
    //  open the interrupt in endpoint.
    //
    if(USBD_SUCCESS(status))
    {
        OpenDevice->ControlEndpointOpen = TRUE;
        USB_BUILD_OPEN_ENDPOINT(
        (PURB_OPEN_ENDPOINT)&OpenDevice->Urb,
        xidNode->EndpointAddressIn,
        USB_ENDPOINT_TYPE_INTERRUPT,
        (USHORT)XID_MAXIMUM_REPORT_SIZE,
        PollingParameters->bInputInterval
        );
        status = xidNode->Device->SubmitRequest(&OpenDevice->Urb);
    }

    //
    //  If interrupt-IN endpoint opened, save the handle
    //  and try to open the interrupt out, if there is
    //  one.
    //
    if(USBD_SUCCESS(status))
    {
        OpenDevice->InterruptInEndpointHandle = OpenDevice->Urb.OpenEndpoint.EndpointHandle;
        ASSERT(OpenDevice->InterruptInEndpointHandle);
        if(PollingParameters->fInterruptOut && xidNode->EndpointAddressOut)
        {
            USB_BUILD_OPEN_ENDPOINT(
                (PURB_OPEN_ENDPOINT)&OpenDevice->Urb,
                xidNode->EndpointAddressOut,
                USB_ENDPOINT_TYPE_INTERRUPT,
                (USHORT)XID_MAXIMUM_REPORT_SIZE,
                PollingParameters->bOutputInterval
                );
		    status = xidNode->Device->SubmitRequest(&OpenDevice->Urb);
        
            if(USBD_SUCCESS(status))
            {
                OpenDevice->InterruptOutEndpointHandle = 
                        OpenDevice->Urb.OpenEndpoint.EndpointHandle;
                ASSERT(OpenDevice->InterruptOutEndpointHandle);
            }
        }
    }

    //
    //  return the status
    //
    return status;
}


VOID
FASTCALL
XID_fCloseDevice(
    IN PXID_OPEN_DEVICE OpenDevice
    )
/*++
   Routine Description:
        Closes an open or paritally open device.
        Always does the right thing.

        This routine assumes that OpenDevice is valid.
--*/
{
    ASSERT_LESS_THAN_DISPATCH_LEVEL();
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  If we are still attached to 
    //  a xidnode, then we need to close the
    //  endpoints.
    //
    if(OpenDevice->XidNode)
    {
        //
        // Notify keyboard services of device closing.
        //
        #ifdef DEBUG_KEYBOARD
        
        if((XDEVICE_TYPE_DEBUG_KEYBOARD==OpenDevice->TypeInformation->XppType) && XID_pKeyboardServices)
        {
           XID_pKeyboardServices->pfnClose(OpenDevice);
        }
        #endif DEBUG_KEYBOARD

        //
        //  Setup event to synchronize close
        //
        KEVENT event;
        KeInitializeEvent(&event, NotificationEvent, FALSE);
        OpenDevice->CloseEvent = &event;
        
        //
        //  Mark close pending as true
        //
        OpenDevice->ClosePending = TRUE;

        //
        //  Kick off close endpoint state
        //  machine.
        //
        XID_fCloseEndpoints(OpenDevice);

        //
        //  Wait for close endpoint state machine
        //  to do its job.
        //
        KeLowerIrql(oldIrql);
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
    } else
    {
        KeLowerIrql(oldIrql);
    }

    //
    //  Everything is cleaned up, just free the memory.
    //
    OpenDevice->TypeInformation->bRemainingHandles++;
    XID_FreeHandle(OpenDevice);
}


VOID
FASTCALL
XID_fCloseEndpoints(
    PXID_OPEN_DEVICE OpenDevice
    )
/*++
    Routine Description:
        This routine closes the endpoints associated with an OpenDevice handle.
--*/
{
    ASSERT_DISPATCH_LEVEL();

    //
    //  Guard against rentry
    //
    if(OpenDevice->EndpointsPendingClose)
    {
        return; 
    }

    // kill the Alamo DPC timer, if it is running
    #ifdef  ALAMO_RAW_DATA_HACK
    if(OpenDevice->XidNode->AlamoHack) KeCancelTimer(&OpenDevice->AlamoPollTimer);
    #endif  //ALAMO_RAW_DATA_HACK


    OpenDevice->EndpointsPendingClose = TRUE;

    //
    //  Start endpoint state machine.
    //

    XID_CloseEndpointStateMachine(&OpenDevice->CloseUrb, OpenDevice);
}

VOID 
XID_CloseEndpointStateMachine(
    PURB_CLOSE_ENDPOINT CloseUrb,
    PXID_OPEN_DEVICE OpenDevice
    )
{
    
    IUsbDevice *device = OpenDevice->XidNode->Device;

    //
    // If the control endpoint is open
    // build an URB to close it.
    //
    if(OpenDevice->ControlEndpointOpen)
    {
        //
        //  Close the default endpoint
        //
        USB_BUILD_CLOSE_DEFAULT_ENDPOINT(
                        CloseUrb,
                        (PURB_COMPLETE_PROC)XID_CloseEndpointStateMachine,
                        (PVOID)OpenDevice
                        );
        OpenDevice->ControlEndpointOpen = FALSE;
    } else if(OpenDevice->InterruptInEndpointHandle)
    //
    //  If the interrupt in endpoint needs to be closed
    //  build an URB to close it.
    //
    {
        USB_BUILD_CLOSE_ENDPOINT(
            CloseUrb,
            OpenDevice->InterruptInEndpointHandle,
            (PURB_COMPLETE_PROC)XID_CloseEndpointStateMachine,
            (PVOID)OpenDevice
            );
        OpenDevice->InterruptInEndpointHandle = NULL;
    } else if(OpenDevice->InterruptOutEndpointHandle)
    //
    //  If the interrupt in endpoint needs to be closed
    //  build an URB to close it.
    //
    {
        USB_BUILD_CLOSE_ENDPOINT(
            CloseUrb,
            OpenDevice->InterruptOutEndpointHandle,
            (PURB_COMPLETE_PROC)XID_CloseEndpointStateMachine,
            (PVOID)OpenDevice
            );
        OpenDevice->InterruptOutEndpointHandle = NULL;
    } else
    //
    //  Everything is closed, handle the reason
    //  we close the endpoints (either pending close
    //  or pending remove).
    //
    {
        PXID_DEVICE_NODE xidNode = OpenDevice->XidNode;
        //
        //  We should only be here if either a close pending
        //  or if a remove is pending.
        //
        ASSERT(OpenDevice->ClosePending || xidNode->PendingRemove);

        //
        //  In either case, we need to sever the xidNode
        //  and the openDevice.
        //
        xidNode->OpenDevice = NULL;
        OpenDevice->XidNode = NULL;

        //
        //  If there is a pending remove,
        //  we need to call remove complete.
        //
        if(xidNode->PendingRemove)
        {
            XID_fRemoveDeviceComplete(xidNode);
        }

        //
        //  If there is a pending close,
        //  signal the close event.
        //
        if(OpenDevice->ClosePending)
        {
            //
            //  Signal the close event
            //
            ASSERT(OpenDevice->CloseEvent);
            KeSetEvent(OpenDevice->CloseEvent, IO_NO_INCREMENT, FALSE);
        }

        return;
    }
   
    //
    //  submit the next request
    //
    device->SubmitRequest((PURB)CloseUrb);
}
   

VOID
XID_NewInterruptData(
    PURB                Urb,
    PXID_OPEN_DEVICE    OpenDevice
    )
/*++
    Routine Description:
        This is the completion routine for new input data.

        We never need to check xidNode or the OpenDevice
        since this is a completion routine for I/O.
        I/O is always guaranteed to complete for a close.

        So we can safely process the I/O.  However,
        before resubmitting any I/O requests, we
        should verify that a close or remove is not
        pending.
--*/
{
    ASSERT_DISPATCH_LEVEL();
    
    PXID_DEVICE_NODE xidNode = OpenDevice->XidNode;

    //
    //  If the device has been closed or is pending
    //  remove, there is nothing to do.
    //
    if(OpenDevice->ClosePending || xidNode->PendingRemove)
    {
        return;  
    }

    //
    //  If the URB was completed successfully, then
    //  process the data.
    //
    if(USBD_SUCCESS(Urb->Header.Status))
    {
        //
        //  Debug only check that the device is not returning short packets
        //
        #if DBG
        if( Urb->BulkOrInterruptTransfer.TransferBufferLength < xidNode->bMaxInputReportSize)
        {
            USB_DBG_TRACE_PRINT(("An input device returned a short packet(expected %d, received %d).",
                xidNode->bMaxInputReportSize,
                Urb->BulkOrInterruptTransfer.TransferBufferLength
                ));
        }
        if( Urb->BulkOrInterruptTransfer.TransferBufferLength > xidNode->bMaxInputReportSize)
        {
            USB_DBG_ERROR_PRINT(("Packet came back larger than maximum allowed."));
            Urb->BulkOrInterruptTransfer.TransferBufferLength = xidNode->bMaxInputReportSize;
        }
        #endif

        //
        //  Process the new data
        //
        xidNode->TypeInformation->pfnProcessNewData(OpenDevice);
        
        OpenDevice->PacketNumber++;
        OpenDevice->OutstandingPoll = 0;
        
        //
        //  Resubmit the URB if autopoll, reset the transfer
        //  length, in case the last transfer came up short.
        //
        Urb->BulkOrInterruptTransfer.TransferBufferLength = xidNode->bMaxInputReportSize;
        
        //
        //  If this is not an auto-poll device, we are done
        //
        if(!OpenDevice->AutoPoll)
        {
            return;
        }
    } else
    {
        //
        //  Print out the error.
        //
        #if DBG
        if(
            (USBD_STATUS_CANCELED != Urb->Header.Status) &&
            (USBD_ISOCH_STATUS_DEV_NOT_RESPONDING != Urb->Header.Status) //99% of the time indicates device is gone
        )
        {
            USB_DBG_WARN_PRINT(("An input device poll failed. Status = 0x%0.8x\n", Urb->Header.Status));
        }
        #endif
        //
        //  Alter the URB so that it clears the endpoint halt.
        //
        USB_BUILD_CLEAR_FEATURE(
                (PURB_CONTROL_TRANSFER)Urb,
                USB_COMMAND_TO_ENDPOINT,
                USB_FEATURE_ENDPOINT_STALL,
                xidNode->EndpointAddressIn,
                (PURB_COMPLETE_PROC)XID_ClearInputStallComplete,
                OpenDevice
                );
    }
    
    //
    //  Resubmit the URB
    //
    xidNode->Device->SubmitRequest(Urb);
}

VOID
XID_ClearInputStallComplete(
    PURB                Urb,
    PXID_OPEN_DEVICE    OpenDevice
    )
/*++
    Routine Description:
        This is the completion routine for sendinf a
        CLEAR_FEATURE(ENDPOINT_HALT).

        We never need to check xidNode or the OpenDevice
        since this is a completion routine for I/O.
        I/O is always guaranteed to complete for a close.

        We should check to see if a close or remove is
        pending before trying to finish the endpoint
        reset, or submitting an input URB.
--*/
{
    PXID_DEVICE_NODE    xidNode = OpenDevice->XidNode;
    USBD_STATUS         status;
    
    //
    //  Don't do anything if a remove or close is pending.
    //
    if(xidNode->PendingRemove || OpenDevice->ClosePending)
    {
        return;
    }
    //
    //  If the CLEAR_FEATURE succeeded,
    //  reset the endpoint state, and try to resubmit
    //  the I/O.
    //
    if(USBD_SUCCESS(Urb->Header.Status))
    {
	    //
        //  Build URB to reset the endpoint state
        //
        USB_BUILD_SET_ENDPOINT_STATE( 
            (PURB_GET_SET_ENDPOINT_STATE)Urb,
            OpenDevice->InterruptInEndpointHandle,
            USB_ENDPOINT_STATE_CLEAR_HALT | USB_ENDPOINT_STATE_DATA_TOGGLE_RESET
            );
	    status = xidNode->Device->SubmitRequest(Urb);
        ASSERT(USBD_SUCCESS(status));

        USB_DBG_WARN_PRINT(("Reset endpoint, resume polling.\n"));
	    //
        //  Build URB to resume polling
        //
        #ifdef ALAMO_RAW_DATA_HACK
        if(xidNode->AlamoHack) XID_AlamoStartPoll(OpenDevice);
        else {
        #endif

        USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
                    (PURB_BULK_OR_INTERRUPT_TRANSFER)&OpenDevice->Urb,
                    OpenDevice->InterruptInEndpointHandle,
                    OpenDevice->ReportForUrb,
                    xidNode->bMaxInputReportSize,
                    USB_TRANSFER_DIRECTION_IN,
                    (PURB_COMPLETE_PROC)XID_NewInterruptData,
                    (PVOID)OpenDevice,
                    TRUE
                    );
	    xidNode->Device->SubmitRequest(Urb);
        #ifdef ALAMO_RAW_DATA_HACK
        }
        #endif

    }  else
    {
        USB_DBG_WARN_PRINT(("Failed to clear halt. Report device as not responding.  Status = 0x%0.8x\n", Urb->Header.Status));
        xidNode->Device->DeviceNotResponding();
    }
}

void
FASTCALL
XID_ProcessGamepadData(
    PXID_OPEN_DEVICE OpenDevice
    )
/*++
 Routine Description:
   Processing of gamepad data.  Basically, it copies the data
   from the buffer that is ping-ponged to the hardware, to the one
   that is always available for copying from XInputGetState.

   Then it checks to see if anything changes and hits XAutoPowerDownResetTimer
--*/
{
    XINPUT_GAMEPAD *pGamePad = (XINPUT_GAMEPAD *)OpenDevice->Report;
    if(OpenDevice->Urb.CommonTransfer.TransferBufferLength >= XID_REPORT_HEADER)
    {
        RtlCopyMemory(
          (PVOID)pGamePad,
          (PVOID)(OpenDevice->ReportForUrb+XID_REPORT_HEADER),
          OpenDevice->Urb.ControlTransfer.TransferBufferLength-XID_REPORT_HEADER
          );

        //
        //  XAutoPowerDownResetTimer if necessary, too much jitter the thumbsticks
        //  means we check all the other controls to see if any of them are pressed.
        //
        if(
            pGamePad->wButtons& (XINPUT_GAMEPAD_DPAD_UP|XINPUT_GAMEPAD_DPAD_DOWN|XINPUT_GAMEPAD_DPAD_LEFT|
                                 XINPUT_GAMEPAD_DPAD_RIGHT|XINPUT_GAMEPAD_START|XINPUT_GAMEPAD_BACK)
          )
        {
            XAutoPowerDownResetTimer();
        } else
        {
            for(int i = XINPUT_GAMEPAD_A; i <= XINPUT_GAMEPAD_RIGHT_TRIGGER; i++)
            {
                if(pGamePad->bAnalogButtons[i])
                {
                  XAutoPowerDownResetTimer();
                  break;
                }
            }
        }
    }
}

void
FASTCALL
XID_ProcessIRRemoteData(
    PXID_OPEN_DEVICE OpenDevice
    )
/*++
 Routine Description:
   IRREM XID processing of new data.  Copies the data
   from the buffer that is ping-ponged to the hardware, 
   then it hits XAutoPowerDownResetTimer.
--*/
{
    RtlCopyMemory(
        (PVOID)OpenDevice->Report,
        (PVOID)(OpenDevice->ReportForUrb+XID_REPORT_HEADER),
        OpenDevice->Urb.ControlTransfer.TransferBufferLength-XID_REPORT_HEADER
        );
    XAutoPowerDownResetTimer();
}

void
FASTCALL
XID_ProcessNewKeyboardData(
    PXID_OPEN_DEVICE OpenDevice
    )
/*++
 Routine Description:
   Keyboard processing is slightly different.  It doesn't have a XID_REPORT_HEADER
--*/
{
  RtlCopyMemory(OpenDevice->Report, OpenDevice->ReportForUrb, sizeof(XINPUT_KEYBOARD));
  if(XID_pKeyboardServices)
  {
    XID_pKeyboardServices->pfnNewData((PVOID)OpenDevice, (PXINPUT_KEYBOARD)OpenDevice->Report);
    XAutoPowerDownResetTimer();
  }
}

DWORD
FASTCALL
XID_fSendDeviceReport(
        IN      PXID_OPEN_DEVICE           OpenDevice,
        IN      PXINPUT_FEEDBACK_INTERNAL  OutputReport
        )
{
    PVOID               rawOutputBuffer;
    USBD_STATUS         usbStatus;
    PXID_DEVICE_NODE    xidNode = OpenDevice->XidNode;
    KIRQL               oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Check to make sure the device has not been removed
    //
    if(NULL==xidNode || xidNode->PendingRemove)
    {
        OutputReport->dwStatus = ERROR_DEVICE_NOT_CONNECTED;
        goto exit_send_device_report;
    }
    //
    //  If bMaxOutputReportSize is zero, the device doesn't support output.
    //
    if(0 == xidNode->bMaxOutputReportSize)
    {
        OutputReport->dwStatus = ERROR_NOT_SUPPORTED;
        goto exit_send_device_report;
    }

    //
    //  Increase the reference count on the completion event
    //  if there is one.
    //
    if(OutputReport->hEvent)
    {
        NTSTATUS status = ObReferenceObjectByHandle(
                            OutputReport->hEvent,
                            ExEventObjectType,
                            (PHANDLE)&OutputReport->Internal.CompletionEvent
                            );
        //
        //  If the following assertion fails the event handle is invalid.
        //  This does not prevent the I/O, but we cannot set the event.
        //  We assert, and NULL out both the hEvent and the pointer
        //  we would have obtained from ObReferenceObjectByHandle
        //
        //ASSERT(NT_SUCCESS(status));
        if(!NT_SUCCESS(status))
        {
            USB_DBG_ERROR_PRINT(("ERROR: pFeedback->Header.hEvent (0x%0.8x) is invalid.\n"
                                 "hEvent should be NULL or a valid Event handle.\n"
                                 "Event cannot be signaled at completion.\n", OutputReport->hEvent));
            OutputReport->hEvent = NULL;
            OutputReport->Internal.CompletionEvent = NULL;    
        }
    } else
    {
        OutputReport->Internal.CompletionEvent = NULL;
    }

    //
    //  Constrain the size if needed.
    //
    if(xidNode->bMaxOutputReportSize < OutputReport->Internal.bSize)
    {
        OutputReport->Internal.bSize = xidNode->bMaxOutputReportSize;
    }

    //
    //  Legacy devices (notably keyboard) does not have a XID_REPORT_HEADER.
    //
    if(xidNode->TypeInformation->ulFlags&XID_BSF_NO_OUTPUT_HEADER)
    {
        rawOutputBuffer = (PVOID)(&OutputReport->Internal.bReportId + XID_REPORT_HEADER);
    } else
    //
    //  Otherwise rawBuffer starts at the bReportId of the internal header
    //
    {
        rawOutputBuffer = (PVOID)&OutputReport->Internal.bReportId;
    }

    //
    //  Build the URB  (use the interrupt Out pipe if there is
    //  one, otherwise use the default pipe with SET_REPORT).
    //
    if(OpenDevice->InterruptOutEndpointHandle)
    {
        USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
                &OutputReport->Internal.Urb.BulkOrInterruptTransfer,
                OpenDevice->InterruptOutEndpointHandle,
                rawOutputBuffer,
                OutputReport->Internal.bSize,
                USB_TRANSFER_DIRECTION_OUT,
                (PURB_COMPLETE_PROC)XID_OutputComplete,
                (PVOID)OutputReport,
                FALSE
                );
    } else
    {
        USB_BUILD_CONTROL_TRANSFER(
            &OutputReport->Internal.Urb.ControlTransfer,
            NULL,
            rawOutputBuffer,
            OutputReport->Internal.bSize,
            USB_TRANSFER_DIRECTION_OUT,
            (PURB_COMPLETE_PROC)XID_OutputComplete,
            (PVOID)OutputReport,
            FALSE,
            (USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE),
            XID_COMMAND_SET_REPORT,
            0x0200 | OutputReport->Internal.bReportId,
            xidNode->InterfaceNumber,
            (WORD)OutputReport->Internal.bSize
            );
    }

    //
    //  Submit the request
    //
    OutputReport->Internal.OpenDevice = OpenDevice;
    usbStatus = xidNode->Device->SubmitRequest(&OutputReport->Internal.Urb);
    OutputReport->dwStatus = IUsbDevice::Win32FromUsbdStatus(usbStatus);

exit_send_device_report:
    
    KeLowerIrql(oldIrql);
    return OutputReport->dwStatus;
}

VOID
XID_OutputComplete(
    PURB                      Urb,
    PXINPUT_FEEDBACK_INTERNAL  OutputReport
    )
{
    ASSERT_DISPATCH_LEVEL();

    //
    //  Check to see if the device has been removed
    //
    PXID_OPEN_DEVICE openDevice = OutputReport->Internal.OpenDevice;
    PXID_DEVICE_NODE xidNode = openDevice->XidNode;

    //
    //  If the device has been removed or closed
    //  set the error to USBD_STATUS_NO_DEVICE
    //
    if(openDevice->ClosePending || xidNode->PendingRemove)
    {
        Urb->Header.Status = USBD_STATUS_NO_DEVICE;
    }
    
    //
    //  If the interrupt-Out endpoint is stalled we have to clear the stall,
    //  prior to copying the status and signalling an event.
    //
    if(
        (Urb->Header.Status == USBD_STATUS_STALL_PID) &&
        (Urb->Header.Function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER)
        )
    {
        //
        //  Print out the error.
        //
        USB_DBG_WARN_PRINT(("The interrupt-Out endpoint stalled, reseting.\n", Urb->Header.Status));
        //
        //  Alter the URB so that it clears the endpoint halt.
        //
        USB_BUILD_CLEAR_FEATURE(
            (PURB_CONTROL_TRANSFER)Urb,
            USB_COMMAND_TO_ENDPOINT,
            USB_FEATURE_ENDPOINT_STALL,
            xidNode->EndpointAddressOut,
            (PURB_COMPLETE_PROC)XID_ClearOutputStallComplete,
            OutputReport
            );
        //
        //  Submit the request to clean the stall.
        //
        xidNode->Device->SubmitRequest(Urb);
    } else
    //
    //  If the endpoint is not stalled (or it was a control endpoint which does not need clearing)
    //  go ahead and complete the output.
    //
    {
        XID_fOutputComplete1(Urb, OutputReport);
    }
}

VOID
FASTCALL
XID_fOutputComplete1(
    PURB Urb,
    PXINPUT_FEEDBACK_INTERNAL OutputReport
    )
{
    PKEVENT event = OutputReport->Internal.CompletionEvent;

    //
    //  Copy and convert the status.
    //
    OutputReport->dwStatus = IUsbDevice::Win32FromUsbdStatus(Urb->Header.Status);

    //
    //  If the call provided a completion event, set it
    //  and then dereference it.
    //  
    if(event)
    {
        KeSetEvent(event, IO_NO_INCREMENT, FALSE);
        ObDereferenceObject(event);
    }
}


VOID
XID_ClearOutputStallComplete(
    PURB                     Urb,
    PXINPUT_FEEDBACK_INTERNAL OutputReport
    )
/*++
    Routine Description:
        This is the completion routine for sendinf a
        CLEAR_FEATURE(ENDPOINT_HALT).

        We never need to check xidNode or the OpenDevice
        since this is a completion routine for I/O.
        I/O is always guaranteed to complete for a close.

        We should check to see if a close or remove is
        pending before trying to finish the endpoint
        reset, or submitting an input URB.
--*/
{
    PXID_OPEN_DEVICE    openDevice = OutputReport->Internal.OpenDevice;
    PXID_DEVICE_NODE    xidNode = openDevice->XidNode;
    USBD_STATUS         status = Urb->Header.Status;
    
    //
    //  If the device has been removed or closed
    //  set the error to USBD_STATUS_NO_DEVICE
    //
    if(openDevice->ClosePending || xidNode->PendingRemove)
    {
        Urb->Header.Status = USBD_STATUS_NO_DEVICE;
    } else 
    {
        if(USBD_SUCCESS(status))
        {
            //
            //  Build and submit URB to reset the endpoint state
            //
            USB_BUILD_SET_ENDPOINT_STATE( 
                (PURB_GET_SET_ENDPOINT_STATE)Urb,
                openDevice->InterruptOutEndpointHandle,
                USB_ENDPOINT_STATE_CLEAR_HALT | USB_ENDPOINT_STATE_DATA_TOGGLE_RESET
                );
	        xidNode->Device->SubmitRequest(Urb);
        } else
        {
            USB_DBG_ERROR_PRINT(("Attempt to clear endpoint stall result in status: 0x%0.8x.\n", status));
        }
        Urb->Header.Status = USBD_STATUS_STALL_PID;
    }    
   
    XID_fOutputComplete1(Urb, OutputReport);
}

VOID
XID_SyncComplete(
    PURB /*UnreferencedUrb*/,
    PKEVENT SyncEvent
    )
//
//  Used to make an asyncronous call, synchronous.
//
{
    KeSetEvent(SyncEvent, IO_NO_INCREMENT, FALSE);
}


//****
//****  Legacy Hardware Enumeration (right now the 
//****  Microsoft Natrual Keyboard Pro is the only supported
//****  legacy device).  Use of this device is limited for
//****  use as a development aid.  However, this is a policy
//****  decision, not a technical one.  Basically, we
//****  will always enumerate  a Natural Keyboard Pro,
//****  however several required methods for using it
//****  require lining xdbg.lib, which is not allowed for
//****  shipping titles.
//****  

#define VID_MICROSOFT               0x045E
#define PID_NATURAL_KEYBOARD_PRO    0x001D

VOID
XID_EnumLegacy1(
    PURB                Urb,
    PXID_DEVICE_NODE    XidNode
    )
/*++
  Routine Description:
    Gets the full device descriptor.  The gives us access to the
    VID and PID so we can support a particular device.

  Comment:
    The commented out code below allows the support of any known
    protocol device (i.e. any boot keyboard).  This was scratched
    and replaced with VID\PID detection of the Microsoft
    Natural Keyboard Pro just for support reasons.  The other code
    should work on a wide variety of keyboards.
--*/
{
    USB_DBG_ENTRY_PRINT(("Entering XID_EnumLegacy1.\n"));
    
    //
    //  First see if it is a known protocol device. (a.k.a. keyboard or
    //  mouse, but we don't support mouse at this point)
    //
    const USB_INTERFACE_DESCRIPTOR *interfaceDescriptor;
    interfaceDescriptor = XidNode->Device->GetInterfaceDescriptor();
    if(
        (USB_DEVICE_CLASS_HUMAN_INTERFACE == interfaceDescriptor->bInterfaceClass) &&
        (HID_KEYBOARD_PROTOCOL == interfaceDescriptor->bInterfaceProtocol)
    )
    {   
        //
        //  Switch to the boot protcol
        //
        
        USB_BUILD_CONTROL_TRANSFER(
            &XID_Globals.EnumUrb.ControlTransfer,
            NULL,
            NULL,
            0,
            0,
            (PURB_COMPLETE_PROC)XID_EnumKeyboard,
            (PVOID)XidNode,
            TRUE,
            USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE,
            HID_SET_PROTOCOL,
            HID_BOOT_PROTOCOL,
            XidNode->InterfaceNumber,
            0);

    }
#if NEVER
    //
    //  Check for legacy devices that we support via the VID\PID\FW,
    //  currently there are none, so this if #if NEVER.
    else
    {
        //
        //  Get the full device descriptor.
        //  The core driver never bothers to get
        //  more than the first 8 bytes.
        //
        USB_BUILD_GET_DESCRIPTOR(
            &XID_Globals.EnumUrb.ControlTransfer,
            USB_DEVICE_DESCRIPTOR_TYPE,
            0,
            0,
            (PVOID)&XID_Globals.EnumDeviceDescriptor,
            sizeof(USB_DEVICE_DESCRIPTOR),
            (PURB_COMPLETE_PROC)XID_EnumLegacy2,
            (PVOID)XidNode
            );
    }
#else
    else
    {
        //
        //  The device is not supported, free the node.
        //
        XidNode->Device->SetExtension(NULL);
        XidNode->Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        XidNode->Device=NULL;
        XidNode->InUse = FALSE;
        XID_Globals.DeviceNodeInUseCount--;
        return;
    }
#endif

    XID_SetEnumWatchdog();
	XidNode->Device->SubmitRequest(&XID_Globals.EnumUrb);
}

#if NEVER
//
//  This routine process enumeration of devices based on VID\PID, currently
//  we have none, so the routine is #if NEVER
//
VOID
XID_EnumLegacy2(
    PURB                Urb,
    PXID_DEVICE_NODE    XidNode
    )
/*++
  Routine Description:
    Completion routine for XID_EnumLegacy1.  At this point we can check the
    VID PID and decide whether or not to support the device.
--*/
{
    USB_DBG_ENTRY_PRINT(("Entering XID_EnumLegacy2.\n"));
    XID_ClearEnumWatchdog();
    
    //
    //  Check for legacy devices that we support
    //

    //
    //  Natural Keyboard Pro Support (NOT NEEDED, it is a standard boot keyboard)
    //  left as an example of what to do.
    //
    if(
        (VID_MICROSOFT == XID_Globals.EnumDeviceDescriptor.idVendor) &&
        (PID_NATURAL_KEYBOARD_PRO == XID_Globals.EnumDeviceDescriptor.idProduct)
    )
    {
        const USB_INTERFACE_DESCRIPTOR *interfaceDescriptor;
        interfaceDescriptor = XidNode->Device->GetInterfaceDescriptor();
        //
        //  Only the keyboard interface of this is device is supported
        //  (it has more than one interface)
        if(
            (USB_DEVICE_CLASS_HUMAN_INTERFACE == interfaceDescriptor->bInterfaceClass) &&
            (HID_KEYBOARD_PROTOCOL == interfaceDescriptor->bInterfaceProtocol)
        )
        {
            //
            //  Switch to the boot protcol
            //
            USB_BUILD_CONTROL_TRANSFER(
                &Urb->ControlTransfer,
                NULL,
                NULL,
                0,
                0,
                (PURB_COMPLETE_PROC)XID_EnumKeyboard,
                (PVOID)XidNode,
                TRUE,
                USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE,
                HID_SET_PROTOCOL,
                HID_BOOT_PROTOCOL,
                XidNode->InterfaceNumber,
                0);
                XID_SetEnumWatchdog();
	            XidNode->Device->SubmitRequest(&XID_Globals.EnumUrb);
            //
            //  Keyboard has more enum steps
            //
            return;
        }
    }

    //
    //  IF the device is not supported, free the node.
    //
    XidNode->Device->SetExtension(NULL);
    XidNode->Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
    XidNode->Device=NULL;
    XidNode->InUse = FALSE;
    XID_Globals.DeviceNodeInUseCount--;
}
#endif

VOID
XID_EnumKeyboard(
    PURB                Urb,
    PXID_DEVICE_NODE    XidNode
    )
/*++
  Routine Description:
    After a keyboard is found there are still more steps.  In particular,
    we must switch to the boot protocol.  XID_EnumLegacy2 did that, and this
    is the completion routine.

    Now we must set the idle rate to infinite.  Infinite should be the default
    for boot protocol keyboards, but this is for good measure.
--*/
{
    XID_ClearEnumWatchdog();
    //
    //  Switch the idle rate to infinite
    //
    USB_BUILD_CONTROL_TRANSFER(
        &XID_Globals.EnumUrb.ControlTransfer,
        NULL,
        NULL,
        0,
        0,
        (PURB_COMPLETE_PROC)XID_EnumKeyboardComplete,
        (PVOID)XidNode,
        TRUE,
        USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE,
        HID_SET_IDLE,
        HID_IDLE_INFINITE,
        XidNode->InterfaceNumber,
        0);
    XID_SetEnumWatchdog();
	XidNode->Device->SubmitRequest(&XID_Globals.EnumUrb);
}

VOID
XID_EnumKeyboardComplete(
    PURB                Urb,
    PXID_DEVICE_NODE    XidNode
    )
/*++
  Routine Description:
    We are done with enumeration steps for enumerating a keyboard
--*/
{
    USB_DBG_ENTRY_PRINT(("Entering XID_EnumKeyboardComplete.\n"));
    XID_ClearEnumWatchdog();

    // Since keyboards are legacy, we hardcode the type.
    XidNode->TypeInformation = GetTypeInformation(XID_DEVTYPE_KEYBOARD, &XidNode->TypeIndex);

    if(!XidNode->TypeInformation)
    {
        USB_DBG_WARN_PRINT(("No keyboard support in this build."));
        XidNode->Device->SetExtension(NULL);
        XidNode->Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
    	XidNode->Device=NULL;
        XidNode->InUse = FALSE;
        XID_Globals.DeviceNodeInUseCount--;
        return;
    }
    
    XidNode->SubType = XINPUT_DEVSUBTYPE_KBD_STANDARD;
    XidNode->bMaxInputReportSize = sizeof(XINPUT_KEYBOARD);
    XidNode->bMaxOutputReportSize = sizeof(XINPUT_KEYBOARD_LEDS);
    XidNode->Device->SetClassSpecificType(XidNode->TypeIndex);
    XidNode->Device->AddComplete(USBD_STATUS_SUCCESS);
    XidNode->Ready = TRUE;
}