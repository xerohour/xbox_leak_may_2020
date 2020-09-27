/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    islixd.cpp

Abstract:

    interface to the slixbox driver

Author:

    Jason Gould (a-jasgou) July 2000

--*/

extern "C" 
{
#include <nt.h>
#include <ntos.h>
}


#include <scsi.h>
//#include <ntdddisk.h>

#include <xtl.h>

#include <usb.h>
#include <stdio.h>
#include "..\inclib\usbxbtransfer.h"

#include "IUSBXBdriver.h"



ULONG               DebugFlags;     // DBGF_* Flags
ULONG               DebugLevel=5;     // Level of debug output
SLIX_DRIVER_EXTENSION gde = {{0},0};
PSLIX_DRIVER_EXTENSION gpDriverExtension = &gde;

URB CloseUrb[MAX_ENDPOINTS];                                                                                              //changed by Bing from 16 to 32
URB StopUrb[MAX_ENDPOINTS];

// we have two stacks and one linked list in order to track and record changes in device states
// one stack for insertion events, one stack for removal events and one doubly-linked list of active devices

HANDLE ghEventActivation;
CXBoxTopology gPluggedStack[32];
HANDLE ghInsertionLock;
int	giPluggedCount = 0;
CXBoxTopology gUnpluggedStack[32];
int giUnpluggedCount = 0;
HANDLE ghUnplugLock;


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
#define XBOX_DEVICE_CLASS_XHAWK_DEVICE 0x78
#define XBOX_DEVICE_CLASS_INPUT_DEVICE 0x58
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
    
	ghEventActivation = CreateEvent(NULL,FALSE,FALSE,NULL);
	ghInsertionLock = CreateMutex(NULL,FALSE,NULL);
	ghUnplugLock = CreateMutex(NULL,FALSE,NULL);
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

	CXBoxTopology Topo;
	Topo.m_bInterfaceNumber = pDevExt->bInterfaceNumber;
	Topo.m_bPort = pDevExt->bPort;
	Topo.m_bSlot = pDevExt->bSlot;
	gUnpluggedStack[giUnpluggedCount++] = Topo;
	SetEvent(ghEventActivation);
	DBGPRINT(2, ("exit:  SLIX_RemoveDevice\n"));
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
    PDEVICE_EXTENSION pDevExt;
	ULONG ulPort;
    BYTE bSlot,bUSBInterface;
	URB urb;
	int i;
	
    const USB_INTERFACE_DESCRIPTOR    *pInterfaceDescriptor;
	const USB_CONFIGURATION_DESCRIPTOR * pConfigurationDescriptor;
	USB_ENDPOINT_DESCRIPTOR * pEndpoint;
	DBGPRINT(2, ("enter: SLIX_AddDevice\n"));
	ASSERT(Device != NULL /* SLIX_RemoveDevice */);

	driverExtension = gpDriverExtension;

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

	pDevExt = &driverExtension->Nodes[ulPort][bSlot][bUSBInterface];

	pDevExt->bPort = (BYTE) ulPort;
	pDevExt->bSlot = bSlot;
	pDevExt->bInterfaceNumber = bUSBInterface;


	// Set our DeviceExtension in our CUSB class data
	DBGPRINT(3, ("Device->SetExtension\n"));
	Device->SetExtension(pDevExt);

	for(i = 0; i < MAX_ENDPOINTS; i++)
	{
		pDevExt->Endpoints[i] = (void*)-1;
		pDevExt->EndpointType[i] = 0;
		pDevExt->EndpointSupported[i] = FALSE;
	}

	pConfigurationDescriptor = Device->GetConfigurationDescriptor();

	// fill in endpoint types
	int iTotalLen = pConfigurationDescriptor->wTotalLength;
	BYTE * pTemp = (BYTE *) pConfigurationDescriptor;
	BYTE * pEnd = pTemp + iTotalLen;

	while(pTemp < pEnd)
	{
		pTemp += pTemp[0];
		// now must start examining type, may be multiple interface descriptors
		if(pTemp[1] == 0x05) // check if endpoint
		{
			BYTE bEndpointNumber;
			pEndpoint = (USB_ENDPOINT_DESCRIPTOR *) pTemp;
			bEndpointNumber = pEndpoint->bEndpointAddress & 0x0f;
			pDevExt->EndpointType[bEndpointNumber] = pEndpoint->bmAttributes & 0x03;
			DebugPrint("Endpoint Supported address = %x\n",&pDevExt->EndpointSupported[bEndpointNumber]);
			
			pDevExt->EndpointSupported[bEndpointNumber] = TRUE;
		}
	}

	// save our device class pointer
	pDevExt->Device = Device;
	pDevExt->flags |= DF_CONNECTED;


	// update current topology
	CXBoxTopology Topo;
	Topo.m_bInterfaceNumber = pDevExt->bInterfaceNumber;
	Topo.m_bPort = pDevExt->bPort;
	Topo.m_bSlot = pDevExt->bSlot;

	Device->SetClassSpecificType(1);
	Device->AddComplete(USBD_STATUS_SUCCESS);
	// insert onto notification stack

	gPluggedStack[giPluggedCount++] = Topo;
	SetEvent(ghEventActivation);
	DBGPRINT(2, ("exit:  SLIX_AddDevice\n"));

}

