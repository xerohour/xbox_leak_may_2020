/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    i_slixdriver.h

Abstract:

    internal header for the SliXbox driver

Author:

    Jason Gould (a-jasgou) July 2000

--*/
#ifndef __i_slixdriver_h_
#define __i_slixdriver_h_

#include <xid.h>

//*****************************************************************************
//**  Defines  ****************************************************************
//*****************************************************************************



#define SLIX_DRIVER_EXTENSION_ID 0
#define MAX_PORTS 8					    //max num of ports on the xbox (currently 5)
#define MAX_SLOTS 4						//max num of slots on a duke, plus 1 (currently 3)
#define MAX_INTERFACES 4				// max num of interfaces (currently 2 on Hawk)
#define BACK_PORT 0xf					//the number corresponding to the back-port of the xbox 
#define MAX_ENDPOINTS 32                                                                                        //changed by Bing
#define SLIX_MAX_BULK_PACKET_SIZE    0x40	//max bytes that can be transfered in a packet


// PDEVICE_EXTENSION->DeviceFlags state flags
//
//#define DF_MRB_IN_PROGRESS          0x00000002
//#define DF_PERSISTENT_ERROR         0x00000004
//#define DF_RESET_IN_PROGRESS        0x00000008
//#define DF_DEVICE_DISCONNECTED      0x00000010
#define DF_CONNECTED   0x0001
#define DF_INITIALIZED 0x0002


#define INCREMENT_PENDING_IO_COUNT(deviceExtension) \
    InterlockedIncrement((PLONG)&((deviceExtension)->PendingIoCount))

#define DECREMENT_PENDING_IO_COUNT(deviceExtension) \
    InterlockedDecrement((PLONG)&((deviceExtension)->PendingIoCount))

typedef unsigned char BYTE;	
typedef unsigned long DWORD;

#define		HID_DESCRIPTOR_TYPE		0x21
typedef struct _USB_HID_DESCRIPTOR
{
	UCHAR							Length;
	UCHAR							DescriptorType;
	USHORT							HIDVersion;
	UCHAR							CountryCode;
	UCHAR							NumClassDescriptors;
	UCHAR							ReportDescriptor;
	UCHAR							ReportDescriptorLength;
	UCHAR							ReportDescriptorSubtype;
	UCHAR							ReportDescriptorSubtypeLength;
} USB_HID_DESCRIPTOR, *PUSB_HID_DESCRIPTOR;


typedef struct _DEVICE_EXTENSION
{
    // device class interface to USB core stack
    IUsbDevice                      *Device;
	VOID*							Endpoints[MAX_ENDPOINTS];                                                                 //changed by Bing
	UCHAR							EndpointType[MAX_ENDPOINTS];                                                              //changed by Bing

	DWORD flags;
	USB_INTERFACE_DESCRIPTOR		InterfaceDescriptor; // storage for interface descriptor
	USB_DEVICE_DESCRIPTOR			DeviceDescriptor;
	USB_CONFIGURATION_DESCRIPTOR	ConfigurationDescriptor;
	USB_HID_DESCRIPTOR				HidDescriptor;
	USB_ENDPOINT_DESCRIPTOR			Endpoint1;
	USB_ENDPOINT_DESCRIPTOR			Endpoint2;
	XID_DESCRIPTOR					XidDescriptor;
	URB								Urb;  // used by enumeration for descriptor queries
	BYTE							bConfigData[200];
	BYTE							bNoOpt;
	XINPUT_CAPABILITIES				InputCaps;
	BYTE							bCapsOutBuff[50];
	BYTE							bCapsInBuff[50];
	BOOL							fAvail;
	BOOL							fGotCaps;
	BYTE							bPort;
	BYTE							bSlot;
	BYTE							bInterfaceNumber;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


typedef struct SLIX_DRIVER_EXTENSION {				//driver extension, holds all relevant data
    DEVICE_EXTENSION Nodes[MAX_PORTS][MAX_SLOTS][MAX_INTERFACES]; //[port][slot][Inteface]		//pointers to the deviceObjects of each possible device
	ULONG	ulDeviceInserted;
	ULONG	ulDeviceRemoved;
}  SLIX_DRIVER_EXTENSION, *PSLIX_DRIVER_EXTENSION;

extern SLIX_DRIVER_EXTENSION * gpDriverExtension;	//pointer to the driver extension is global...
extern DEVICE_EXTENSION *	gInsertedDevices[];
extern USHORT				gInsertedCount;


#if DBG || DEBUG_LOG

extern ULONG               DebugFlags;     // DBGF_* Flags
extern ULONG               DebugLevel;     // Level of debug output

#define DBGPRINT(level, _x_) \
    if (level <= DebugLevel) { \
        KdPrint(("SlixD: ")); \
        KdPrint( _x_ ); \
    }


#else

#define DBGPRINT(level, _x_)

#endif



/*****************************************************************************
//
// F U N C T I O N    P R O T O T Y P E S
//
//*****************************************************************************


NTSTATUS SLIX_Create (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS SLIX_Close (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);


NTSTATUS SLIX_DeviceControl (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

VOID SLIX_StartIo (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

//VOID SLIX_TimeoutDpc (IN PKDPC Dpc, IN PVOID Context, IN PVOID SysArg1, IN PVOID SysArg2);



typedef struct _REQUEST {
	BYTE* pData;
	int MaxData;
	
} REQUEST, *PREQUEST;
*/
#endif
