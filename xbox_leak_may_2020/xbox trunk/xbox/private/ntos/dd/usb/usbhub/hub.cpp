/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    hub.cpp

Abstract:
	
	Basic entry point implementation of the USB HUB driver.
    
	
Environment:

	Designed for XBOX.

Notes:

Revision History:

    03-02-00 created by Mitchell Dernis (mitchd)

--*/


//
//	Pull in OS headers
//
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <usb.h>
#include <xdbg.h>

//
//	Setup the debug information for this file (see ..\inc\debug.h)
//
//#define PROMOTE_TRACE_TO_WARN //define this to get more spew in this module only

#define	MODULE_POOL_TAG			 'HBSU'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("USBHUB");

//
//	Pull in public usb headers
//
#include <usb.h>
//
//	Pull in hub headers
//
#include "hub.h"


//------------------------------------------------------------------------------
//  Declare USBHUB types and class.
//------------------------------------------------------------------------------
USB_DEVICE_TYPE_TABLE_BEGIN(USBHUB_)
USB_DEVICE_TYPE_TABLE_ENTRY(NULL)
USB_DEVICE_TYPE_TABLE_END()
USB_CLASS_DRIVER_DECLARATION_DEVICE_LEVEL(USBHUB_, USB_DEVICE_CLASS_HUB, 0, 0)
#pragma data_seg(".XPP$ClassHub")
USB_CLASS_DECLARATION_POINTER(USBHUB_)
#pragma data_seg(".XPP$Data")

USBHUB_INFO						 GLOBAL_HubInfo;
USBHUB_ENUM_BLOCK				 GLOBAL_HubEnum;

#pragma code_seg(".XPPCINIT")
EXTERNUSB VOID USBHUB_Init(IUsbInit *)
{
	
	//
	//	Initialize global structures
	//
	GLOBAL_HubInfo.NodeCount = USBHUB_MAXIMUM_HUB_COUNT;
	GLOBAL_HubInfo.NodesInUse = 0;
	KeInitializeTimer(&GLOBAL_HubEnum.WatchdogTimer);
}
#pragma code_seg(".XPPCODE")

VOID
USBHUB_WatchdogTimerProc(PKDPC Dpc, PVOID pvHubDevice, PVOID, PVOID)
{
    USB_DBG_WARN_PRINT(("Watchdog timer proc hit."));
    IUsbDevice *hubDevice = (IUsbDevice *)pvHubDevice;
    PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)hubDevice->GetExtension();
    switch(GLOBAL_HubEnum.WatchdogReason)
    {
        case USBHUB_WATCHDOG_REASON_ENUM:
            hubDevice->CancelRequest(&hubNode->Urb);
            break;
        case USBHUB_WATCHDOG_REASON_RESET_PORT:
            hubDevice->ResetComplete(USBD_STATUS_REQUEST_FAILED, GLOBAL_HubEnum.ResetContext);
            break;
        case USBHUB_WATCHDOG_REASON_DISABLE_PORT:
            hubDevice->DisableComplete(USBD_STATUS_REQUEST_FAILED, GLOBAL_HubEnum.ResetContext);
            break;
    }
    
}
__inline void USBHUB_SetEnumWatchdog(DWORD dwReason)
{
    LARGE_INTEGER wait;
    if(USBHUB_WATCHDOG_REASON_ENUM == dwReason)
    {
        wait.QuadPart = -5000*10000; //5 seconds for most commands
    } else
    {
        wait.QuadPart = -500*10000; //500 ms for disabling or reseting a port 
    }
    GLOBAL_HubEnum.WatchdogReason = dwReason;
    KeSetTimer(&GLOBAL_HubEnum.WatchdogTimer, wait, &GLOBAL_HubEnum.WatchdogTimerDpc);
}
__inline void USBHUB_ClearEnumWatchdog()
{
    KeCancelTimer(&GLOBAL_HubEnum.WatchdogTimer);
}

EXTERNUSB VOID
USBHUB_AddDevice(
	IN IUsbDevice *HubDevice
	)
{
	ULONG						nodeIndex = 0;
	PUSBHUB_DEVICE_NODE			hubNode;
	if(GLOBAL_HubInfo.NodesInUse < GLOBAL_HubInfo.NodeCount)
	{
		//
		//	Find a free node, and claim it.
		//
		while(GLOBAL_HubInfo.Nodes[nodeIndex].InUse) nodeIndex++;
		GLOBAL_HubInfo.NodesInUse++;
		hubNode = &GLOBAL_HubInfo.Nodes[nodeIndex];
		//
		//	Attach the node to our hubDevice
		//
		HubDevice->SetExtension((PVOID)hubNode);

		//
		//	Initialize what we can about the device.
		//
		hubNode->InUse = TRUE;
        hubNode->NotResponding = FALSE;
		hubNode->RemovePending = FALSE;
		hubNode->PortConnectedBitmap = 0;
        hubNode->ErrorCount = 0;

		//
		//	Open the default endpoint, this cannot fail, because we
		//	are at enum time.
		//
		USB_BUILD_OPEN_DEFAULT_ENDPOINT((PURB_OPEN_ENDPOINT)&hubNode->Urb);
		HubDevice->SubmitRequest(&hubNode->Urb);
				
		//
		//	Now get the configuration descriptor. We are guaranteed to get 
		//	only one enumeration request at a time, so we can just assume
		//	that all of our shared enumeration resources are available.
		//
		USB_BUILD_GET_DESCRIPTOR(
				(PURB_CONTROL_TRANSFER)&hubNode->Urb,
				USB_CONFIGURATION_DESCRIPTOR_TYPE,
				0,
				0,
				GLOBAL_HubEnum.EnumBuffer,
				sizeof(GLOBAL_HubEnum.EnumBuffer),
				(PURB_COMPLETE_PROC)USBHUB_EnumHubStage1,
				(PVOID)HubDevice
				);
        KeInitializeDpc(&GLOBAL_HubEnum.WatchdogTimerDpc, USBHUB_WatchdogTimerProc, HubDevice);

        USBHUB_SetEnumWatchdog(USBHUB_WATCHDOG_REASON_ENUM);
  		HubDevice->SubmitRequest(&hubNode->Urb);
	}
	else
	{
        ASSERT(FALSE);
		//
		//	Out of nodes so we cannot support the device
		//  (allow a retry though)
        //
		HubDevice->AddComplete(USBD_STATUS_NO_MEMORY);
	}
}

VOID
USBHUB_EnumHubStage1(
			PURB	Urb,
			IUsbDevice	*HubDevice
	)
{
	USBD_STATUS status;
	PUSBHUB_DEVICE_NODE	 hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	USBHUB_ClearEnumWatchdog();
	//
	//	It is possible that get configuration descriptor failed.
	//
	if(USBD_ERROR(Urb->Header.Status))
	{
		USBHUB_RemoveHubStage2(Urb, HubDevice);
        return;
	}
	//
	//	We need the following descriptors from the configuration:
	//		Endpoint Descriptor for Status Change Endpoint:
	//
	//
	PUSB_COMMON_DESCRIPTOR		commonDescriptor = (PUSB_COMMON_DESCRIPTOR)(GLOBAL_HubEnum.EnumBuffer);
	PUSB_ENDPOINT_DESCRIPTOR	endpointDescriptor;
	ULONG						enumBufferOffset = 0;
	do
	{
		enumBufferOffset += commonDescriptor->bLength;
		ASSERT(enumBufferOffset < sizeof(GLOBAL_HubEnum.EnumBuffer));
		commonDescriptor = (PUSB_COMMON_DESCRIPTOR)(GLOBAL_HubEnum.EnumBuffer + enumBufferOffset);
	}while(commonDescriptor->bDescriptorType != USB_ENDPOINT_DESCRIPTOR_TYPE);
	endpointDescriptor = (PUSB_ENDPOINT_DESCRIPTOR)commonDescriptor;
	
	if(endpointDescriptor->wMaxPacketSize <= 4)
	{
		hubNode->InterruptEndpointMaxPacket = (UCHAR) endpointDescriptor->wMaxPacketSize;
	} else
	{
		hubNode->InterruptEndpointMaxPacket = 4;
	}
    
    hubNode->InterruptEndpointAddress = endpointDescriptor->bEndpointAddress;
	
    //
	//	Now try to open the interrupt endpoint
	//	(this could fail due to limited bandwidth)
	//
	USB_BUILD_OPEN_ENDPOINT(
		(PURB_OPEN_ENDPOINT)Urb,
		endpointDescriptor->bEndpointAddress,
		endpointDescriptor->bmAttributes&USB_ENDPOINT_TYPE_MASK,
		hubNode->InterruptEndpointMaxPacket,
		//endpointDescriptor->bInterval
		(UCHAR)16
		);
	status = HubDevice->SubmitRequest(Urb);
	//
	//	If the open failed abort the enumeration, we cannot support the hub.
	//
	if(USBD_ERROR(status))
	{
		USBHUB_RemoveHubStage2(Urb, HubDevice);
	}
	else
	{
		//
		//	Save the interrupt handle
		//
		hubNode->InterruptEndpointHandle = Urb->OpenEndpoint.EndpointHandle;
        ASSERT(hubNode->InterruptEndpointHandle);
		//
		//	Configure the device.
		//
		USB_BUILD_SET_CONFIGURATION(
				(PURB_CONTROL_TRANSFER)Urb,
				1,
				(PURB_COMPLETE_PROC)USBHUB_EnumHubStage2,
				(PVOID)HubDevice
				);

        USBHUB_SetEnumWatchdog(USBHUB_WATCHDOG_REASON_ENUM);
		HubDevice->SubmitRequest(Urb);
	}
	return;
}

VOID
USBHUB_EnumHubStage2(
	PURB    Urb,
	IUsbDevice	*HubDevice
	)
{
	USBHUB_ClearEnumWatchdog();
    if(USBD_ERROR(Urb->Header.Status))
	{
		USBHUB_RemoveHubStage1(HubDevice);
        return;
	}
	
	//
	//	Get the Hub descriptor
	//
	USB_BUILD_CONTROL_TRANSFER(
		(PURB_CONTROL_TRANSFER)Urb,
		NULL,
		GLOBAL_HubEnum.EnumBuffer,				//The middle was initialized in stage 1
		sizeof(USB_HUB_DESCRIPTOR),
		USB_TRANSFER_DIRECTION_IN,
		(PURB_COMPLETE_PROC)USBHUB_EnumHubComplete,
		(PVOID)HubDevice,
		TRUE,
		USB_DEVICE_TO_HOST | USB_CLASS_COMMAND,
		USB_REQUEST_GET_DESCRIPTOR,
		USB_HUB_DESCRIPTOR_TYPE << 8,
		0,
		sizeof(USB_HUB_DESCRIPTOR)
		);

    USBHUB_SetEnumWatchdog(USBHUB_WATCHDOG_REASON_ENUM);

	HubDevice->SubmitRequest(Urb);
}

VOID
USBHUB_EnumHubComplete(
			PURB	Urb,
			IUsbDevice	*HubDevice
	)
{
	PUSBHUB_DEVICE_NODE	 hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	PUSB_HUB_DESCRIPTOR  hubDescriptor;

    USBHUB_ClearEnumWatchdog();
	if(USBD_ERROR(Urb->Header.Status))
	{
		USBHUB_RemoveHubStage1(HubDevice);
        return;
	}
	//
	//	Strip the info we need out of the hub descriptor.
	//
	hubDescriptor = (PUSB_HUB_DESCRIPTOR)GLOBAL_HubEnum.EnumBuffer;
	hubNode->PortCount = hubDescriptor->bNumberOfPorts;
	//
	//	Assert that the hub does not have too many ports.
	//
	ASSERT(7 >= hubNode->PortCount);
	//
	//	We support only 7 ports on a hub!
	//
	if(hubNode->PortCount > 7) hubNode->PortCount = 7;
	USB_DBG_TRACE_PRINT(("Detected Hub with %d ports", hubNode->PortCount));
	//
	//	AddComplete.  What does this mean?
	//	1) That we are done with EnumArgs passed into us at USBHUB_AddHub.
	//	2) That we are done with our global enumeration resources, and thus
	//	   are ready to accept enumeration requests for additional hubs.
	//	3) We are ready to accept remove requests for this device.
	//
	HubDevice->AddComplete(USBD_STATUS_SUCCESS);

	//
	//	However, we still need to power on the ports
	//	and detect any devices that may be attached.
	//	Our normal port status change code will handle this.
	//	Basically we just indicate that the status of all the
	//	ports have changed and go out and enumerate.
	//
	hubNode->PortProcessIndex = 1;
	USB_DBG_TRACE_PRINT(("Powering port 1"));
	USB_BUILD_CONTROL_TRANSFER(
				(PURB_CONTROL_TRANSFER)&hubNode->Urb,
				NULL,
				NULL,
				0,
				0,
				(PURB_COMPLETE_PROC)USBHUB_PoweringPorts,
				(PVOID)HubDevice,
				FALSE,
				USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_OTHER,
				USB_REQUEST_SET_FEATURE,
				USBHUB_FEATURE_PORT_POWER,
				hubNode->PortProcessIndex,
				0
				);
	HubDevice->SubmitRequest(&hubNode->Urb);
}	

VOID
USBHUB_PoweringPorts(
			PURB	Urb,
			IUsbDevice	*HubDevice
	)
{
	PUSBHUB_DEVICE_NODE	 hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	
	if(USBD_ERROR(Urb->Header.Status))
	{
		USB_DBG_WARN_PRINT(("Error Powering Port."));
		//
		//	Move on to next port.
		//
	}
	
	if(hubNode->PortProcessIndex == hubNode->PortCount)
	{
		USB_DBG_TRACE_PRINT(("Done powering ports, submit change notification request."));
		//
		//	All ports are powered, submit a change notification
		//	Urb, that should kick everything off.
		//
		hubNode->PortProcessIndex = 0;
		USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
				(PURB_BULK_OR_INTERRUPT_TRANSFER)&hubNode->Urb,
				hubNode->InterruptEndpointHandle,
				hubNode->RequestBuffer,
				hubNode->InterruptEndpointMaxPacket,
				USB_TRANSFER_DIRECTION_IN,
				(PURB_COMPLETE_PROC)USBHUB_ChangeNotifyComplete,
				(PVOID)HubDevice,
				TRUE
				);
	} else
	//
	//	Update the port number and re-submit the URB to power the next port
	//
	{
		USB_DBG_TRACE_PRINT(("Powering port %d", hubNode->PortProcessIndex+1));
		Urb->ControlTransfer.SetupPacket.wIndex = ++hubNode->PortProcessIndex;
	}
	HubDevice->SubmitRequest(&hubNode->Urb);
}

VOID
USBHUB_ChangeNotifyComplete(
			PURB	Urb,
			IUsbDevice	*HubDevice
	)
/*++
Routine Description:
	This is the completion routine for transfers over the
	interrupt endpoint.  USB Hubs uses the interrupt endpoint
	to notify the driver that either the status of the hub or
	one of its hosts has changed.  The number of valid bits
	depends on the number of ports.  See Figure 11-14 in USB
	Specification Revision 1.1 for details.

	Here we copy the valid bits (we only support up to 7 ports)
	into PortStatusChangeBitmap before starting the kicking
	off the processing of changes.
Parameters:
	Urb			- URB that was just completed.
	HubNode		- HubNode of hub we are handling.
--*/
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	//
	//	Check for pending removal
	//
	if(hubNode->RemovePending)
	{
		//
		//	The removal was waiting for this URB to complete 
		//	so start the removal process.
		//
		USBHUB_RemoveHubStage1(HubDevice);
		return;
	}
	//
	//	If the URB is successful we need to start
	//	processing whatever happened.
	//
	if(USBD_SUCCESS(Urb->Header.Status))
	{
		hubNode->ErrorCount = 0;
        UCHAR Mask = (1 << (hubNode->PortCount+1))-1;
		hubNode->PortStatusChangeBitmap = hubNode->RequestBuffer[0]&Mask;
		USBHUB_PortStatusChange1(HubDevice);
		return;
    }

	//
	//	Increment the error count.
	//  With three failures in a row
    //  we report the device as not resonding.
    //
    if(3 < ++hubNode->ErrorCount)
    {
        hubNode->NotResponding = TRUE;
        HubDevice->DeviceNotResponding();
        return;
    }

    //
    //  Clear the endpoint halt
    // 
    USB_BUILD_CLEAR_FEATURE(
        (PURB_CONTROL_TRANSFER)Urb,
        USB_COMMAND_TO_ENDPOINT,
        USB_FEATURE_ENDPOINT_STALL,
        hubNode->InterruptEndpointAddress,
        (PURB_COMPLETE_PROC)USBHUB_ClearInterruptStallComplete,
        HubDevice
        );
    HubDevice->SubmitRequest(Urb);
}

VOID
USBHUB_ClearInterruptStallComplete(
			PURB	Urb,
			IUsbDevice	*HubDevice
	)
/*++
    After a notification fails on the interrupt endpoint (via a stall)
    the stall must be cleared before it can be retried.  The first
    step sending a (CLEAR_FEATURE(STALL)) is an asynchronous wire
    transfer this is the completion routine.

    We then instruct the USB core stack to clear the halt, and
    resubmitt the interrupt request.
--*/
{
    PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	//
	//	Check for pending removal
	//
	if(hubNode->RemovePending)
	{
		//
		//	The removal was waiting for this URB to complete 
		//	so start the removal process.
		//
		USBHUB_RemoveHubStage1(HubDevice);
		return;
	}
 
    //
    //  If we cannot clear the halt, then just
    //  report the device as not responding.
    //
    if(USBD_ERROR(Urb->Header.Status))
    {
        hubNode->NotResponding = TRUE;
        HubDevice->DeviceNotResponding();
        return;
    }
	
    //
    //  Build and submit URB to reset the endpoint state
    //  (This synchronous and cannot fail with valid parameters).
    //
    USB_BUILD_SET_ENDPOINT_STATE( 
        (PURB_GET_SET_ENDPOINT_STATE)Urb,
        hubNode->InterruptEndpointHandle,
        USB_ENDPOINT_STATE_CLEAR_HALT | USB_ENDPOINT_STATE_DATA_TOGGLE_RESET
        );
    HubDevice->SubmitRequest(Urb);
    
    //
    //  Resubmit the interrupt request
    //

    USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
		&Urb->BulkOrInterruptTransfer,
		hubNode->InterruptEndpointHandle,
		hubNode->RequestBuffer,
		hubNode->InterruptEndpointMaxPacket,
		USB_TRANSFER_DIRECTION_IN,
		(PURB_COMPLETE_PROC)USBHUB_ChangeNotifyComplete,
		(PVOID)HubDevice,
		TRUE
		);
    HubDevice->SubmitRequest(Urb);
}

VOID
USBHUB_PortStatusChange1(
		IUsbDevice	*HubDevice
	)
/*++

Routine Description:
	This is the first in a series of routines that process
	status changes to hub or its ports.  The processing is a 
	loop (we can only handle one port at a time).  In this stage,
	we isolate one of the changed elements (either the hub itself,
	or a port on the hub) and start the processing.  After
	we are done processing that change we come back here to get
	status for the next one.
Arguments:
	HubNode - The node for the hub we need to process.

--*/
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	UCHAR mask = 1;
	//
	//	Find the the first bit set in 
	//	PortStatusChangeBitmap, note bit 0 means the hub status
	//	has changed.  This is why there are portcount + 1, valid bits.
	//
	USB_DBG_TRACE_PRINT(("Looking for ports with changed status"));
	for(
		hubNode->PortProcessIndex=0;
		hubNode->PortProcessIndex <= hubNode->PortCount;
		hubNode->PortProcessIndex++, mask <<= 1
	)
	{
		if(hubNode->PortStatusChangeBitmap & mask)
		{
			USB_DBG_TRACE_PRINT(("Port %d has changed status", hubNode->PortProcessIndex));
			//
			//	Clear the bit and break.
			//
			hubNode->PortStatusChangeBitmap &= ~mask;
			break;
		}
	}

	//*
	//*	Three cases follow:
	//*		1) PortProcessIndex is greater than port count: all changes
	//*			are processed, so we build the URB as a request for
	//*			change notification on the interrupt-in endpoint.
	//*
	//*		2) PortProcessIndex is 0:  the status of the hub itself
	//*			has changed.  We build a request to get the
	//*			status of the hub.
	//*
	//*		3) PortProcessIndex is between 1 and the port count, inclusive. (Basically,
	//*			not case 1 or 2.)  We build a request to get the status of the
	//*			port.
	//*
	//*		In all three cases we have build an URB.  We then submit it, and return.

	//
	//	1) No more changes
	//
	if(hubNode->PortProcessIndex > hubNode->PortCount)
	{
		USB_DBG_TRACE_PRINT(("All port changes are processed, submitting URB on change notification endpoint"));
		USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
				(PURB_BULK_OR_INTERRUPT_TRANSFER)&hubNode->Urb,
				hubNode->InterruptEndpointHandle,
				hubNode->RequestBuffer,
				hubNode->InterruptEndpointMaxPacket,
				USB_TRANSFER_DIRECTION_IN,
				(PURB_COMPLETE_PROC)USBHUB_ChangeNotifyComplete,
				(PVOID)HubDevice,
				TRUE
				);
	
	}
	//
	//	2) Hub status changed
	//
	else if(0==hubNode->PortProcessIndex)
	{
		USB_DBG_TRACE_PRINT(("Hub status has changed.  Querying for hub status."));
		USB_BUILD_CONTROL_TRANSFER(
			(PURB_CONTROL_TRANSFER)&hubNode->Urb,
			NULL,
			hubNode->RequestBuffer,
			4,
			USB_TRANSFER_DIRECTION_IN,
			(PURB_COMPLETE_PROC)USBHUB_HubStatusChange,
			(PVOID)HubDevice,
			FALSE,
			USB_DEVICE_TO_HOST | USB_CLASS_COMMAND | USB_COMMAND_TO_DEVICE,
			USB_REQUEST_GET_STATUS,
			0,
			0,
			4
			);	
		
	} 
	//
	//	2) Port status changed
	//
	else
	{
		USB_DBG_TRACE_PRINT(("Port %d status has changed.  Querying for port status.", hubNode->PortProcessIndex));
		USB_BUILD_CONTROL_TRANSFER(
			(PURB_CONTROL_TRANSFER)&hubNode->Urb,
			NULL,
			hubNode->RequestBuffer,
			4,
			USB_TRANSFER_DIRECTION_IN,
			(PURB_COMPLETE_PROC)USBHUB_PortStatusChange2,
			(PVOID)HubDevice,
			FALSE,
			USB_DEVICE_TO_HOST | USB_CLASS_COMMAND | USB_COMMAND_TO_OTHER,
			USB_REQUEST_GET_STATUS,
			0,
			hubNode->PortProcessIndex,
			4
			);
	}
	
	//
	//	Submit the request that we have built
	//
	HubDevice->SubmitRequest(&hubNode->Urb);
}

VOID
USBHUB_PortStatusChange2(
			PURB	Urb,
			IUsbDevice	*HubDevice
	)
/*++

Routine Description:
	Stage two of processing a change to a hub.  The Urb to get
	the status of the port should now have completed.
Arguments:
	Urb		- URB that was submitted to get status.
	HubNode - The node for the hub we need to process.
--*/
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	//
	//	Check for pending removal
	//
	if(hubNode->RemovePending)
	{
		//
		//	The removal was waiting for this URB to complete 
		//	so start the removal process.
		//
		USBHUB_RemoveHubStage1(HubDevice);
		return;
	}
	//
	//	If the URB is successful we need to start
	//	processing whatever happened.
	//
	if(USBD_SUCCESS(Urb->Header.Status))
	{
        hubNode->ErrorCount = 0;
		//
		//	One or more flags are set
		//
		USBHUB_PortProcessChange(HubDevice);
	}else
	{
		//
		//	Deal with the error case.
		//
        if(3 < ++hubNode->ErrorCount)
        {
            hubNode->NotResponding = TRUE;
            HubDevice->DeviceNotResponding();
        } else
        {
            //
            //  Resubmit the request.
            //
            Urb->ControlTransfer.TransferBufferLength = 4;
            HubDevice->SubmitRequest(Urb);
        }
	}
}

VOID
USBHUB_HubStatusChange(
			PURB	Urb,
			IUsbDevice	*HubDevice
	)
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	USHORT FeatureToClear;
	
	//
	//	Check for pending removal
	//
	if(hubNode->RemovePending)
	{
		//
		//	The removal was waiting for this URB to complete 
		//	so start the removal process.
		//
		USBHUB_RemoveHubStage1(HubDevice);
		return;
	}

    //
    //	Deal with the error case.
	//
    if(USBD_ERROR(Urb->Header.Status))
    {
        if(3 < ++hubNode->ErrorCount)
        {
            hubNode->NotResponding = TRUE;
            HubDevice->DeviceNotResponding();
    
        } else
        {
            //
            //  Resubmit the request.
            //
            Urb->ControlTransfer.TransferBufferLength = 4;
            HubDevice->SubmitRequest(Urb);
        }
        return;
    } 
    
    hubNode->ErrorCount = 0;

	//
	//	The status of the whole hub changed
	//
	if(hubNode->PortStatus.StatusChange&USBHUB_C_HUB_STATUS_LOCAL_POWER)
	{
		FeatureToClear = USBHUB_FEATURE_C_HUB_LOCAL_POWER;
		hubNode->PortStatus.StatusChange &= ~USBHUB_C_HUB_STATUS_LOCAL_POWER;
	} else if(hubNode->PortStatus.StatusChange&USBHUB_C_HUB_STATUS_OVER_CURRENT)
	{
		FeatureToClear = USBHUB_FEATURE_C_HUB_OVER_CURRENT;
		hubNode->PortStatus.StatusChange &= ~USBHUB_C_HUB_STATUS_OVER_CURRENT;
	} else
	{
		USB_DBG_WARN_PRINT(("HubStatusChange called when there was none."));
		USBHUB_PortStatusChange1(HubDevice);
		return;
	}
	//
	//	Build the clear feature request, and send it
	//
	USB_BUILD_CONTROL_TRANSFER(
		(PURB_CONTROL_TRANSFER)&hubNode->Urb,
		NULL,
		NULL,
		0,
		0,
		(PURB_COMPLETE_PROC)USBHUB_ClearHubFeatureComplete,
		(PVOID)HubDevice,
		FALSE,
		USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_DEVICE,
		USB_REQUEST_CLEAR_FEATURE,
		FeatureToClear,
		0,
		0
		);
	HubDevice->SubmitRequest(&hubNode->Urb);
}

VOID
USBHUB_ClearHubFeatureComplete(
			PURB	Urb,
			IUsbDevice	*HubDevice
	)
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	//
	//	Check for pending removal
	//
	if(hubNode->RemovePending)
	{
		//
		//	The removal was waiting for this URB to complete 
		//	so start the removal process.
		//
		USBHUB_RemoveHubStage1(HubDevice);
		return;
	}
    
    //
    //	Deal with the error case.
	//
    if(USBD_ERROR(Urb->Header.Status))
    {
        if(3 < ++hubNode->ErrorCount)
        {
            hubNode->NotResponding = TRUE;
            HubDevice->DeviceNotResponding();
    
        } else
        {
            //
            //  Resubmit the request.
            //
            Urb->ControlTransfer.TransferBufferLength = 4;
            HubDevice->SubmitRequest(Urb);
        }
        return;
    } 
    hubNode->ErrorCount = 0;

	USBHUB_PortStatusChange1(HubDevice);
}

VOID
USBHUB_PortProcessChange(
	IUsbDevice	*HubDevice
	)
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	USHORT FeatureToClear;
	//
	//	Check for a reset port that completed.
	//
	if(hubNode->PortStatus.StatusChange&USBHUB_C_PORT_STATUS_RESET)
	{
		USB_DBG_TRACE_PRINT(("Port reset completed"));
		//
		//	We can signal that the reset is completed
		//	successfully.
		//
        if(
            (USBHUB_WATCHDOG_REASON_RESET_PORT == GLOBAL_HubEnum.WatchdogReason) &&
            (GLOBAL_HubEnum.ResetContext)
        )
        {
            USBD_STATUS status = USBD_STATUS_SUCCESS;
            USBHUB_ClearEnumWatchdog();
            //
            //  Under exterme hot-plug conditions, the
            //  port can be disabled even before we
            //  realise that the reset was complete.
            //  Just say that the reset failed.
            //
            if(
                !(hubNode->PortStatus.Status & USBHUB_PORT_STATUS_ENABLE) ||
                (hubNode->PortStatus.Status & USBHUB_PORT_STATUS_RESET)
            )
            {
				USB_DBG_WARN_PRINT(("Reset Completed, but port not enabled, failing port reset!"));
                status = USBD_STATUS_REQUEST_FAILED;
            } else
            {
                //
                //  Check for low-speed
                //
                if(hubNode->PortStatus.Status&USBHUB_PORT_STATUS_LOW_SPEED)
                {
                    status = USBD_STATUS_LOWSPEED;
                }
            }
            PVOID Context = GLOBAL_HubEnum.ResetContext;
            GLOBAL_HubEnum.ResetContext = NULL;
            HubDevice->ResetComplete(status, Context);
            USB_DBG_TRACE_PRINT(("Returned from notifying port reset"));
        }
		//
		//	Clear the change status
		//
		hubNode->PortStatus.StatusChange &= ~USBHUB_C_PORT_STATUS_RESET;
		FeatureToClear = USBHUB_FEATURE_C_PORT_RESET;
	}
	//
	//	Check for a change in the connection status
	//
	else if(hubNode->PortStatus.StatusChange&USBHUB_C_PORT_STATUS_CONNECTION)
	{
		UCHAR PortNumber = hubNode->PortProcessIndex;
		UCHAR PortMask = 1 << (PortNumber-1);
		//
		//	Is it a connect or a disconnect?
		//
		if(hubNode->PortStatus.Status&USBHUB_PORT_STATUS_CONNECTION)
		{
            
            if(hubNode->PortConnectedBitmap&PortMask)
		    {
			   USB_DBG_WARN_PRINT(("Remove and Add in One Notification."));
               HubDevice->DeviceDisconnected(PortNumber);
		    }
            //
            //  We used to just report DeviceConnected here.
            //  Now do this RelatchPortSpeed thing which works
            //  around a bug in the TI chipset.  It keeps the
            //  state machine going, so we just return afterwards.
            //
            USBHUB_RelatchPortSpeed(HubDevice);
            return;
		}else
		{
			if(hubNode->PortConnectedBitmap&PortMask)
			{
				hubNode->PortConnectedBitmap &= ~PortMask;
    			HubDevice->DeviceDisconnected(PortNumber);
			}
		}
		//
		//	Clear the change status
		//
		hubNode->PortStatus.StatusChange &= ~USBHUB_C_PORT_STATUS_CONNECTION;
		FeatureToClear = USBHUB_FEATURE_C_PORT_CONNECTION;
	}
	//
	//	Check for a change in the enable status
	//
	else if(hubNode->PortStatus.StatusChange&USBHUB_C_PORT_STATUS_ENABLE)
	{
        //
        //  Nothing to do.
        //
		
        //
		//	Clear the change status
		//
		hubNode->PortStatus.StatusChange &= ~USBHUB_C_PORT_STATUS_ENABLE;
		FeatureToClear = USBHUB_FEATURE_C_PORT_ENABLE;
	}
	//
	//	Check for a change in the suspend status
	//
	else if(hubNode->PortStatus.StatusChange&USBHUB_C_PORT_STATUS_SUSPEND)
	{
		//
		//	Clear the change status
		//
		hubNode->PortStatus.StatusChange &= ~USBHUB_C_PORT_STATUS_SUSPEND;
		FeatureToClear = USBHUB_FEATURE_C_PORT_SUSPEND;
		USB_DBG_WARN_PRINT(("Unexpected changed in suspend status, Xbox does not suspend devices."));
	}
	else if(hubNode->PortStatus.StatusChange&USBHUB_C_PORT_STATUS_OVER_CURRENT)
	{
		//
		//	Clear the change status
		//
		hubNode->PortStatus.StatusChange &= ~USBHUB_C_PORT_STATUS_OVER_CURRENT;
		FeatureToClear = USBHUB_FEATURE_C_PORT_OVER_CURRENT;
		USB_DBG_WARN_PRINT(("Overcurrent detected on port %d", hubNode->PortProcessIndex));
	} else
	{
		USB_DBG_WARN_PRINT(("Port change bitmask not recognized (0x%0.8x)  Ignoring it.", hubNode->PortStatus.StatusChange));
		hubNode->PortStatus.StatusChange = 0;  //NULL this out so we don't get called back.
		USBHUB_ClearPortFeatureComplete(&hubNode->Urb, HubDevice);
		return;
	}

	//
	//	Build the clear feature request
	//
	
	USB_DBG_TRACE_PRINT(("Clearing the feature %d.", FeatureToClear));
	USB_BUILD_CONTROL_TRANSFER(
		(PURB_CONTROL_TRANSFER)&hubNode->Urb,
		NULL,
		NULL,
		0,
		0,
		(PURB_COMPLETE_PROC)USBHUB_ClearPortFeatureComplete,
		(PVOID)HubDevice,
		FALSE,
		USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_OTHER,
		USB_REQUEST_CLEAR_FEATURE,
		FeatureToClear,
		hubNode->PortProcessIndex,
		0
		);
	HubDevice->SubmitRequest(&hubNode->Urb);
}

VOID
USBHUB_ClearPortFeatureComplete(
	PURB	Urb,
	IUsbDevice	*HubDevice
	)
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	//
	//	Check for pending removal
	//
	if(hubNode->RemovePending)
	{
		//
		//	The removal was waiting for this URB to complete 
		//	so start the removal process.
		//
		USBHUB_RemoveHubStage1(HubDevice);
		return;
	}
	//
	//	Other changes to process on this port?
	//
	if(hubNode->PortStatus.StatusChange)
	{
		USB_DBG_TRACE_PRINT(("Other status bits on this port changed."));
		USBHUB_PortProcessChange(HubDevice);
	} else
	//
	//	Loop back and process other ports that may have changed.
	//
	{
		USB_DBG_TRACE_PRINT(("Done with this port, check others."));
		USBHUB_PortStatusChange1(HubDevice);
	}
}

EXTERNUSB VOID
USBHUB_RemoveDevice(
	IN IUsbDevice *HubDevice
	)
{
	ULONG				portBit;
	UCHAR				portIndex;
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	
	ASSERT_DISPATCH_LEVEL();
	
	
	ASSERT(hubNode);
		
	//
	//	Mark the removed flag.
	//
	hubNode->RemovePending = TRUE;
	
	//
	//	Report any devices that were connected
	//	to the hub, as disconnected.
	//
	portBit = 1;
	portIndex = 1;
	do
	{
		if(hubNode->PortConnectedBitmap & portBit)
		{
			HubDevice->DeviceDisconnected(portIndex);
			hubNode->PortConnectedBitmap &= ~portBit;
		}
		portBit <<= 1;
	}while( ++portIndex <= hubNode->PortCount );

    //
    //  If the hub was reported as not responding, then
    //  we there is no outstanding I/O.  We can just start the
    //  close procedure.
    //
    if(hubNode->NotResponding)
    {
        USBHUB_RemoveHubStage1(HubDevice);
    }
}


VOID
USBHUB_RemoveHubStage1(
	IN IUsbDevice *HubDevice
	)
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	USB_DBG_ENTRY_PRINT(("Entering USBHUB_RemoveHubStage1"));
	ASSERT_DISPATCH_LEVEL();
	//
	//	Close the interrupt endpoint
	//
	USB_BUILD_CLOSE_ENDPOINT(
					(PURB_CLOSE_ENDPOINT)&hubNode->Urb,
					hubNode->InterruptEndpointHandle,
					(PURB_COMPLETE_PROC)USBHUB_RemoveHubStage2,
					(PVOID)HubDevice
					);
	HubDevice->SubmitRequest(&hubNode->Urb);
}

VOID
USBHUB_RemoveHubStage2(
	PURB	Urb,
	IUsbDevice	*HubDevice
	)
{
	USB_DBG_ENTRY_PRINT(("Entering USBHUB_RemoveHubStage2"));
	ASSERT_DISPATCH_LEVEL();
	//
	//	Close the default endpoint
	//
	USB_BUILD_CLOSE_DEFAULT_ENDPOINT(
							(PURB_CLOSE_ENDPOINT)Urb,
							(PURB_COMPLETE_PROC)USBHUB_RemoveHubComplete,
							(PVOID)HubDevice
							);
	HubDevice->SubmitRequest(Urb);
}

VOID
USBHUB_RemoveHubComplete(
	PURB	Urb,
	IUsbDevice	*HubDevice
	)
{
	PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
	ASSERT_DISPATCH_LEVEL();
	USB_DBG_ENTRY_PRINT(("Entering USBHUB_RemoveHubComplete"));
	//
	//	Free the hubNode
	//
	HubDevice->SetExtension(NULL);
	hubNode->InUse = FALSE;
	GLOBAL_HubInfo.NodesInUse--;
	//
	//	Notify our bus driver that we are done processing
	//	remove, or that we have aborted an Add.
	//
	if(hubNode->RemovePending)
	{
		HubDevice->RemoveComplete();
	}else
	{
        //
        //  Fail, but allow retry. (Remove ASSERT)
        //
        USB_DBG_WARN_PRINT(("Abort Enumeration, allow retry."));
        HubDevice->AddComplete(USBD_STATUS_NO_MEMORY);
	}
}

	
VOID
USBHUB_DisableResetPort(
	IN IUsbDevice *HubDevice,
	IN UCHAR	PortNumber,
	IN PVOID	Context,
    IN BOOLEAN  Disable
	)
{
	PUSBHUB_DEVICE_NODE hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
    UCHAR bRequest = USB_REQUEST_SET_FEATURE;
    USHORT wValue = USBHUB_FEATURE_PORT_RESET;
    DWORD dwReason = USBHUB_WATCHDOG_REASON_RESET_PORT;
    PURB_COMPLETE_PROC CompleteProc = (PURB_COMPLETE_PROC)USBHUB_ResetComplete;
	
	//
	//	If we didn't find the hub, or if the port number is invalid
	//	call the complete routine with an error, then return.
	//
	
	if( !hubNode || (hubNode->PortCount < PortNumber) )
	{
		USB_DBG_ERROR_PRINT(("Reset failed due to invalid parameter.", hubNode));
		HubDevice->ResetComplete(USBD_STATUS_INVALID_PARAMETER, Context);
		return;
	}

	//
	//	Save of the completion information.
	// (We can only handle one reset or disable at a time)
	GLOBAL_HubEnum.ResetContext = Context;

	//
	//	Build and send an URB for reseting a port.
	//
    if(Disable)
    {
        bRequest = USB_REQUEST_CLEAR_FEATURE;
        wValue = USBHUB_FEATURE_PORT_ENABLE;
        CompleteProc = (PURB_COMPLETE_PROC)USBHUB_DisableComplete;
        dwReason = USBHUB_WATCHDOG_REASON_DISABLE_PORT;
        USB_DBG_TRACE_PRINT(("Disabling Port %d.", PortNumber));
    }   
    else
    {
        USB_DBG_TRACE_PRINT(("Reseting Port %d.", PortNumber));
    }
	USB_BUILD_CONTROL_TRANSFER(
		(PURB_CONTROL_TRANSFER)&GLOBAL_HubEnum.ResetUrb,
		NULL,
		NULL,
		0,
		0,
		CompleteProc,
		(PVOID)HubDevice,
		FALSE,
		USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_OTHER,
		bRequest,
		wValue,
		PortNumber,
		0
		);
    
    //
    //  Set the watchdog so that disable and\or reset don't
    //  hang.  This could happen if the hub is removed.
    //
    USBHUB_SetEnumWatchdog(dwReason);
	HubDevice->SubmitRequest(&GLOBAL_HubEnum.ResetUrb);
}

VOID
USBHUB_ResetComplete(
	IN PURB    Urb,
	IN IUsbDevice *HubDevice
	)
{
	//
	//	If the set feature reset transmission failed, notify
	//	completion procedure.
	//
	if(USBD_ERROR(Urb->Header.Status))
	{
		USBHUB_ClearEnumWatchdog();
        USB_DBG_TRACE_PRINT(("Port %d Reset failed.", Urb->ControlTransfer.SetupPacket.wIndex));
		HubDevice->ResetComplete(Urb->Header.Status, GLOBAL_HubEnum.ResetContext);
	}
	//
	//	Otherwise we are waiting for the port status to change.
	//	Nothing to do.
}

VOID
USBHUB_DisableComplete(
	IN PURB    Urb,
	IN IUsbDevice *HubDevice
	)
{
    //
    //  Pass the status to disable
    //
    USBHUB_ClearEnumWatchdog();
	HubDevice->DisableComplete(Urb->Header.Status, GLOBAL_HubEnum.ResetContext);
}


//
// HACK: For TI hub, but we apply it to all hubs, since it does no harm.
//
VOID
USBHUB_RelatchPortSpeed(
    IUsbDevice *HubDevice
    )
/*++
 Routine Description:
   Sends a CLEAR_PORT_FEATURE(FEATURE_PORT_ENABLE) to "disable" the hub when a
   new device is connected.  This is a very strange thing to do, but works
   around a bug in the TI hub chip.  In short, the TI hub debounces the full-speed
   \low-speed of a newly inserted device poorly.  It will (often enough) latch in
   low-speed when a full-speed device is connected.  TI claims that sending a disable
   command will cause the speed bit to relatch, and that fixes the problem.

   This is better than the previous work-around at it only introduces an extra
   millisecond delay (as opposed to a 120 ms retry) and should prevent any
   wierd errors.  We don't know what 3rd party devices will do if someone speaks
   low-speed to them.
--*/
{
    PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
    USB_BUILD_CONTROL_TRANSFER(
	    (PURB_CONTROL_TRANSFER)&hubNode->Urb,
		NULL,
		NULL,
		0,
		0,
		(PURB_COMPLETE_PROC)USBHUB_RelatchPortSpeedComplete,
		(PVOID)HubDevice,
		FALSE,
		USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_OTHER,
		USB_REQUEST_CLEAR_FEATURE,
		USBHUB_FEATURE_PORT_ENABLE,
		hubNode->PortProcessIndex,
		0
		);
	HubDevice->SubmitRequest(&hubNode->Urb);
}


VOID
USBHUB_RelatchPortSpeedComplete(
    IN PURB    Urb,
    IUsbDevice *HubDevice
    )
/*++
  Routine Description:
   Completion routine for USBHUB_RelatchPortSpeed.

   Actually, tells USBD about the new device.
--*/
{

    PUSBHUB_DEVICE_NODE	hubNode = (PUSBHUB_DEVICE_NODE)HubDevice->GetExtension();
    hubNode->PortStatus.StatusChange &= ~USBHUB_C_PORT_STATUS_CONNECTION;
    UCHAR ucPortMask = 1 << (hubNode->PortProcessIndex-1);

    HubDevice->DeviceConnected(hubNode->PortProcessIndex, 5);
    hubNode->PortConnectedBitmap |= ucPortMask;
    
    USB_DBG_TRACE_PRINT(("Clearing the feature %d.", FeatureToClear));
	USB_BUILD_CONTROL_TRANSFER(
		(PURB_CONTROL_TRANSFER)&hubNode->Urb,
		NULL,
		NULL,
		0,
		0,
		(PURB_COMPLETE_PROC)USBHUB_ClearPortFeatureComplete,
		(PVOID)HubDevice,
		FALSE,
		USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_OTHER,
		USB_REQUEST_CLEAR_FEATURE,
		USBHUB_FEATURE_C_PORT_CONNECTION,
		hubNode->PortProcessIndex,
		0
		);
	HubDevice->SubmitRequest(&hubNode->Urb);
}