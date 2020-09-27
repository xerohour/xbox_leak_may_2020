/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    islixd.cpp

Abstract:

    interface to the slixbox driver

Author:

    Jason Gould (a-jasgou) July 2000

--*/

extern "C" {
#include <nt.h>
#include <ntos.h>
}


#include <scsi.h>
//#include <ntdddisk.h>

#include <xtl.h>

#include <usb.h>
#include <stdio.h>

#include "..\inc\i_slixdriver.h"
#include "..\inc\slixdriver.h"

ULONG               DebugFlags;     // DBGF_* Flags
ULONG               DebugLevel=5;     // Level of debug output
SLIX_DRIVER_EXTENSION gde = {0};	
SLIX_DRIVER_EXTENSION *	gpDriverExtension;
DEVICE_EXTENSION *	gInsertedDevices[20];
USHORT				gInsertedCount;


//the following mess allows slix to grab & talk to all devices
#define SLIX1_Init DriverInit
#define SLIX2_Init DriverInit
#define SLIX3_Init DriverInit
#define SLIX4_Init DriverInit
#define SLIX5_Init DriverInit
#define SLIX6_Init DriverInit
#define SLIX7_Init DriverInit
#define SLIX8_Init DriverInit
#define SLIX9_Init DriverInit
#define SLIX10_Init DriverInit



#define SLIX1_AddDevice SLIX_AddDevice
#define SLIX2_AddDevice SLIX_AddDevice
#define SLIX3_AddDevice SLIX_AddDevice
#define SLIX4_AddDevice SLIX_AddDevice
#define SLIX5_AddDevice SLIX_AddDevice
#define SLIX6_AddDevice SLIX_AddDevice
#define SLIX7_AddDevice SLIX_AddDevice
#define SLIX8_AddDevice SLIX_AddDevice
#define SLIX9_AddDevice SLIX_AddDevice
#define SLIX10_AddDevice SLIX_AddDevice

#define SLIX1_RemoveDevice SLIX_RemoveDevice
#define SLIX2_RemoveDevice SLIX_RemoveDevice
#define SLIX3_RemoveDevice SLIX_RemoveDevice
#define SLIX4_RemoveDevice SLIX_RemoveDevice
#define SLIX5_RemoveDevice SLIX_RemoveDevice
#define SLIX6_RemoveDevice SLIX_RemoveDevice
#define SLIX7_RemoveDevice SLIX_RemoveDevice
#define SLIX8_RemoveDevice SLIX_RemoveDevice
#define SLIX9_RemoveDevice SLIX_RemoveDevice
#define SLIX10_RemoveDevice SLIX_RemoveDevice


#define SLIXDEVICEPORT  (&SLIXDEVICEPORT_TABLE)
#define SLIXDEVICESLOT  (&SLIXDEVICESLOT_TABLE)
DECLARE_XPP_TYPE(SLIXDEVICEPORT)
DECLARE_XPP_TYPE(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX1_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX2_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX3_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX4_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX5_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX6_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX7_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX8_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX9_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
USB_DEVICE_TYPE_TABLE_BEGIN(SLIX10_)
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICEPORT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT),
USB_DEVICE_TYPE_TABLE_ENTRY(SLIXDEVICESLOT)
USB_DEVICE_TYPE_TABLE_END()
#define XBOX_DEVICE_CLASS_INPUT_DEVICE 0x58
#define XBOX_DEVICE_CLASS_XHAWK_DEVICE 0x78
USB_CLASS_DRIVER_DECLARATION(SLIX1_, USB_DEVICE_CLASS_AUDIO, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION_DUPLICATE(SLIX1_, 1, XBOX_DEVICE_CLASS_XHAWK_DEVICE, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX2_, USB_DEVICE_CLASS_COMMUNICATIONS, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX3_, USB_DEVICE_CLASS_HUMAN_INTERFACE, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION_DUPLICATE(SLIX3_, 1, XBOX_DEVICE_CLASS_INPUT_DEVICE, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX4_, USB_DEVICE_CLASS_MONITOR, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX5_, USB_DEVICE_CLASS_PHYSICAL_INTERFACE, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX6_, USB_DEVICE_CLASS_POWER, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX7_, USB_DEVICE_CLASS_PRINTER, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX8_, USB_DEVICE_CLASS_STORAGE, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX9_, USB_DEVICE_CLASS_RESERVED, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION(SLIX10_, USB_DEVICE_CLASS_VENDOR_SPECIFIC, 0xFF, 0xFF)
#pragma data_seg(".XPP$ClassAAASLIX") //ohci should load this before anything else
USB_CLASS_DECLARATION_POINTER(SLIX1_)
USB_CLASS_DECLARATION_POINTER_DUPLICATE(SLIX1_, 1)
USB_CLASS_DECLARATION_POINTER(SLIX2_)
USB_CLASS_DECLARATION_POINTER(SLIX3_)
USB_CLASS_DECLARATION_POINTER_DUPLICATE(SLIX3_, 1)
USB_CLASS_DECLARATION_POINTER(SLIX4_)
USB_CLASS_DECLARATION_POINTER(SLIX5_)
USB_CLASS_DECLARATION_POINTER(SLIX6_)
USB_CLASS_DECLARATION_POINTER(SLIX7_)
USB_CLASS_DECLARATION_POINTER(SLIX8_)
USB_CLASS_DECLARATION_POINTER(SLIX9_)
USB_CLASS_DECLARATION_POINTER(SLIX10_)
#pragma data_seg()


//apis we are replacing by over-writing xid.sys... xapi is still looking for these!
/*
extern "C" void XInputClose(int){}
extern "C" void XInputGetCapabilities(int,int){}
extern "C" void XInputGetState(int,int){}
extern "C" void XInputOpen(int,int,int,int){}
extern "C" void XInputPoll(int){}
extern "C" void XInputSetState(int,int){}
*/

URB CloseUrb[MAX_ENDPOINTS];                                                                                              //changed by Bing from 16 to 32
URB StopUrb[MAX_ENDPOINTS];

void KeepCode()
{
	return;
}

/*************************************************************************************
Function:   DriverInit
Purpose:	the actually useful entrypoint of the slixbox driver. Sets up lots of stuff
Params:     DriverObject --- the object slixboxdriver will use...
			RegistryPath --- irrelevant
Return:     STATUS_SUCCESS, or some error code
*************************************************************************************/
void
DriverInit (IUsbInit *pUsbInit)
{
    DBGPRINT(2, ("enter:  DriverInit\n"));    
    ULONG port, slot,USBInterface;
    UCHAR realMaxPorts = (UCHAR)XGetPortCount();

	gpDriverExtension = &gde;

	for (port = 0;port < MAX_PORTS; port++) 
	{
		for (slot = 0; slot < MAX_SLOTS; slot++) 
		{
			for (USBInterface = 0; USBInterface < MAX_INTERFACES; USBInterface++)
			{
				gpDriverExtension->Nodes[port][slot][USBInterface].Device = NULL;
			}
		}
    }
    


    //
    //  Allocate a whole bunch of resources, we don't
    //  care that much how much memory we take, and
    //  we don't want to run out.
    //
    USB_RESOURCE_REQUIREMENTS resourceRequirements;
    resourceRequirements.ConnectorType = USB_CONNECTOR_TYPE_DIRECT;
    resourceRequirements.MaxDevices = realMaxPorts;
    resourceRequirements.MaxCompositeInterfaces = 3;
    resourceRequirements.MaxControlEndpoints = 2;
    resourceRequirements.MaxBulkEndpoints = 2;
    resourceRequirements.MaxInterruptEndpoints = 2;    
    resourceRequirements.MaxControlTDperTransfer = 40;
    resourceRequirements.MaxBulkTDperTransfer = 40;
    resourceRequirements.MaxIsochEndpoints = 2;
    resourceRequirements.MaxIsochMaxBuffers = 5;
    pUsbInit->RegisterResources(&resourceRequirements);
    resourceRequirements.ConnectorType = USB_CONNECTOR_TYPE_HIGH_POWER;
    pUsbInit->RegisterResources(&resourceRequirements);
    resourceRequirements.ConnectorType = USB_CONNECTOR_TYPE_LOW_POWER;
    resourceRequirements.MaxDevices = realMaxPorts*2;
    pUsbInit->RegisterResources(&resourceRequirements);

    DBGPRINT(2, ("exit:  DriverInit\n"));    

    return;// status;
}

//callback for endpoint closes (below) to allow them to be asynchronous
VOID Arf (PURB urb, PVOID context)
{ }

/*************************************************************************************
Function:   SLIX_RemoveDevice
Purpose:	called by pnp when a device is unplugged. Removes that interface from our array
Params:     Device = the interface to talk to usbd
Return:     none
*************************************************************************************/

VOID SLIX_RemoveDevice (IN IUsbDevice *Device)
{
	HANDLE ourendpoints[MAX_ENDPOINTS];
	int i;

    DBGPRINT(2, ("enter: SLIX_RemoveDevice\n"));
	if(Device == NULL) {
		DBGPRINT(1, ("SLIX_RemoveDevice called with NULL Device!\n"));
		return;
	}

    PDEVICE_EXTENSION   pDevExt = (PDEVICE_EXTENSION) Device->GetExtension();

	if(pDevExt == NULL) {
		DBGPRINT(1, ("SLIX_RemoveDevice called for device that has already been removed!!\n"
			"IUsbDevice passed: %p. Port: %x\n", Device, Device->GetPort()))
	}

	pDevExt->fAvail = FALSE;

	pDevExt->flags = 0;

	pDevExt->Device = 0;


	if(pDevExt->Endpoints[0] == NULL) {	//-1 == closed, other == open
		RtlZeroMemory(&CloseUrb[0],sizeof(URB));
		USB_BUILD_CLOSE_DEFAULT_ENDPOINT((&CloseUrb[0].CloseEndpoint), Arf, 0);
		Device->SubmitRequest(&CloseUrb[0]);
	}
	for(i = 1; i < MAX_ENDPOINTS; i++)	{                                                                            //changed by Bing from 16 to 32
		if(pDevExt->Endpoints[i] != (void*)-1 && pDevExt->Endpoints[i] != (void*)NULL) {
			RtlZeroMemory(&CloseUrb[i],sizeof(URB));
			if(pDevExt->EndpointType[i] == 1) {
				RtlZeroMemory(&StopUrb[i],sizeof(URB));
				USB_BUILD_ISOCH_STOP_TRANSFER((&StopUrb[i].IsochStopTransfer), pDevExt->Endpoints[i]);
				Device->SubmitRequest(&StopUrb[i]);
				USB_BUILD_ISOCH_CLOSE_ENDPOINT((&CloseUrb[i].IsochCloseEndpoint), pDevExt->Endpoints[i], Arf, 0);
			} else {
				USB_BUILD_CLOSE_ENDPOINT((&CloseUrb[i].CloseEndpoint), pDevExt->Endpoints[i], Arf, 0);
			}
			Device->SubmitRequest(&CloseUrb[i]);
		}
	}

	for(i = 0; i < MAX_ENDPOINTS; i++) {
		pDevExt->Endpoints[i] = (void*)-1;
		pDevExt->EndpointType[i] = 0;
	}

	Device->SetExtension(NULL);
	Device->RemoveComplete();


	DBGPRINT(2, ("exit:  SLIX_RemoveDevice\n"));
}


void AddCompletionRoutine(PURB pUrb, PDEVICE_EXTENSION pDevExt)
{
	// wrap up after URB completes

	KIRQL	OldIrqLevel;
	ULONG	ulDevices;
	BYTE *	pTemp = (BYTE *) &pDevExt->XidDescriptor;
	BYTE bByte = * pTemp;
	BYTE bByte2 = * (pTemp +1);

	IUsbDevice * pDevice = pDevExt->Device;
	
	pDevice->AddComplete(USBD_STATUS_SUCCESS);
	pDevExt->bNoOpt = bByte | bByte2;
	pDevExt->fGotCaps = FALSE;
	pDevExt->fAvail = TRUE;

	OldIrqLevel = KeRaiseIrqlToDpcLevel();
	gInsertedDevices[gInsertedCount++] = pDevExt;
	KeLowerIrql(OldIrqLevel);

	DBGPRINT(1,("Submit Request Status = %x\n",pUrb->Header.Status));

}

/*************************************************************************************
Function:   SLIX_AddDevice
Purpose:	the actually useful entrypoint of the slixbox driver. Sets up lots of stuff
Params:     DriverObject --- the object slixboxdriver will use...
			RegistryPath --- irrelevant
Return:     STATUS_SUCCESS, or some error code
*************************************************************************************/
VOID SLIX_AddDevice (IN IUsbDevice *Device)
{
    PSLIX_DRIVER_EXTENSION driverExtension;
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION fdoDeviceExtension;
	ULONG ulPort;
    BYTE bSlot,bUSBInterface;
	URB urb;
	int i;
	UCHAR ucEndpointType;
	
    const USB_INTERFACE_DESCRIPTOR  *pInterfaceDescriptor;
	const USB_DEVICE_DESCRIPTOR8 *	pDeviceDescriptor;
	const USB_CONFIGURATION_DESCRIPTOR * pConfigurationDescriptor;
	driverExtension = gpDriverExtension;

	DBGPRINT(2, ("enter: SLIX_AddDevice\n"));
	ASSERT(Device != NULL /* SLIX_RemoveDevice */);

	// get topology so we know where we are plugged in
	ulPort = Device->GetPort();
    bSlot = 0;
    if(ulPort >= 16)
    {
        bSlot = 1;
        ulPort -= 16;
    }

	
	pInterfaceDescriptor = Device->GetInterfaceDescriptor();
	bUSBInterface = pInterfaceDescriptor->bInterfaceNumber;

	DBGPRINT(3, ("interface Class: %d, interface Subclass: %d , interface number: %d\n", 
		pInterfaceDescriptor->bInterfaceClass,
		pInterfaceDescriptor->bInterfaceSubClass,
		bUSBInterface));

	//if it's an MU or hawk, move it to the next port...
	//(if we didn't do this, it would be impossible ot distinguish top-slot from control pad)
	if((pInterfaceDescriptor->bInterfaceClass != USB_DEVICE_CLASS_HUMAN_INTERFACE) 
		&& (pInterfaceDescriptor->bInterfaceClass != USB_DEVICE_CLASS_PHYSICAL_INTERFACE)
		&& (pInterfaceDescriptor->bInterfaceClass != XBOX_DEVICE_CLASS_INPUT_DEVICE)) {
		bSlot++;
	}	

	fdoDeviceExtension = &driverExtension->Nodes[ulPort][bSlot][bUSBInterface];

	fdoDeviceExtension->bPort = (BYTE) ulPort;
	fdoDeviceExtension->bSlot = bSlot;
	fdoDeviceExtension->bInterfaceNumber = bUSBInterface;
	DBGPRINT(3, ("Topology:  port: %d, slot %d, interface %d\n", ulPort, bSlot, bUSBInterface));
	fdoDeviceExtension = &driverExtension->Nodes[ulPort][bSlot][bUSBInterface];
	fdoDeviceExtension->fAvail = FALSE;
	// save interface, configuration and device descriptors
	memcpy(&fdoDeviceExtension->InterfaceDescriptor,pInterfaceDescriptor,sizeof(USB_INTERFACE_DESCRIPTOR));
	pConfigurationDescriptor = Device->GetConfigurationDescriptor();
	memcpy(&fdoDeviceExtension->ConfigurationDescriptor,pConfigurationDescriptor,sizeof(USB_CONFIGURATION_DESCRIPTOR));
	memcpy(&fdoDeviceExtension->bConfigData,pConfigurationDescriptor,pConfigurationDescriptor->wTotalLength);
	

//	pDeviceDescriptor = Device->GetDeviceDescriptor();
//	memcpy(&fdoDeviceExtension->DeviceDescriptor, pDeviceDescriptor,sizeof(USB_DEVICE_DESCRIPTOR));
	// Set our DeviceExtension in our CUSB class data
	DBGPRINT(3, ("Device->SetExtension\n"));
	Device->SetExtension(fdoDeviceExtension);


	for(i = 0; i < MAX_ENDPOINTS; i++) {
		fdoDeviceExtension->Endpoints[i] = (void*)-1;
		fdoDeviceExtension->EndpointType[i] = 0;
	}

	// save our device class pointer
	fdoDeviceExtension->Device = Device;
	fdoDeviceExtension->flags |= DF_CONNECTED;

	Device->SetClassSpecificType(1);


	USBD_STATUS UsbdStatus = USBD_STATUS_SUCCESS;
		
    //
    //  Get XID Descriptor
    //
    USB_BUILD_CONTROL_TRANSFER(
        (PURB_CONTROL_TRANSFER)&fdoDeviceExtension->Urb,
        NULL, //Default endpoint
        (PVOID)&fdoDeviceExtension->XidDescriptor,
        sizeof(XID_DESCRIPTOR),
        USB_TRANSFER_DIRECTION_IN,
        (PURB_COMPLETE_PROC) AddCompletionRoutine,		
        fdoDeviceExtension,
        TRUE,
        (USB_DEVICE_TO_HOST | USB_VENDOR_COMMAND | USB_COMMAND_TO_INTERFACE),
        USB_REQUEST_GET_DESCRIPTOR,
        XID_DESCRIPTOR_TYPE,
        pInterfaceDescriptor->bInterfaceNumber,
        sizeof(XID_DESCRIPTOR)
        );

	UsbdStatus = Device->SubmitRequest(&fdoDeviceExtension->Urb);
	DBGPRINT(2, ("exit:  SLIX_AddDevice\n"));

}

