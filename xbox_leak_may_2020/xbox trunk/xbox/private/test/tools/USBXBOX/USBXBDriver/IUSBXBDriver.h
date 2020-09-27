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

#include "..\inclib\usbxbtransfer.h"
//*****************************************************************************
//**  Defines  ****************************************************************
//*****************************************************************************

#ifdef __cplusplus
extern "C" {
#endif
	ULONG DebugPrint(PCHAR Format, ...);	//prints stuff to the debugger computer
#ifdef __cplusplus
}
#endif



#define SLIX_DRIVER_EXTENSION_ID 0
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

typedef struct _DEVICE_EXTENSION
{
    // device class interface to USB core stack
    IUsbDevice                      *Device;
	VOID*							Endpoints[MAX_ENDPOINTS];            //changed by Bing
	UCHAR							EndpointType[MAX_ENDPOINTS];         //changed by Bing
	BOOL							EndpointSupported[MAX_ENDPOINTS];	// added by djk
	DWORD flags;

	BYTE							bPort;
	BYTE							bSlot;
	BYTE							bInterfaceNumber;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


typedef struct SLIX_DRIVER_EXTENSION {				//driver extension, holds all relevant data
//    USB_PNP_CLASS_DRIVER_DESCRIPTION ClassDescriptionList[16];	//basically unused except in DriverInit
    DEVICE_EXTENSION Nodes[MAX_PORTS][MAX_SLOTS][MAX_INTERFACES]; //[port][slot]		//pointers to the deviceObjects of each possible device
	BYTE LastAddress;								//last device referenced in IN, OUT, or SETUP
}  SLIX_DRIVER_EXTENSION, *PSLIX_DRIVER_EXTENSION;



extern SLIX_DRIVER_EXTENSION * gpDriverExtension;	//pointer to the driver extension is global...

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
