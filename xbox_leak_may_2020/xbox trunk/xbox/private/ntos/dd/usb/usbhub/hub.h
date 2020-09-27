/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    hub.h

Abstract:

    Structures and functions used by the usbhub driver
	
Environment:

	Designed for XBOX.

Notes:

Revision History:

    02-22-00 created by Mitchell Dernis (mitchd)

--*/

#ifndef __HUB_H__
#define __HUB_H__


#define USB_HUB_DESCRIPTOR_TYPE		0x29

//
//	USB HUB SPECIFIC FEATURES
//
#define	USBHUB_FEATURE_C_HUB_LOCAL_POWER		0
#define	USBHUB_FEATURE_C_HUB_OVER_CURRENT		1
#define	USBHUB_FEATURE_PORT_CONNECTION			0
#define	USBHUB_FEATURE_PORT_ENABLE				1
#define	USBHUB_FEATURE_PORT_SUSPEND				2
#define	USBHUB_FEATURE_PORT_OVER_CURRENT		3
#define	USBHUB_FEATURE_PORT_RESET				4
#define	USBHUB_FEATURE_PORT_POWER				8
#define	USBHUB_FEATURE_C_PORT_CONNECTION		16
#define	USBHUB_FEATURE_C_PORT_ENABLE			17
#define	USBHUB_FEATURE_C_PORT_SUSPEND			18
#define	USBHUB_FEATURE_C_PORT_OVER_CURRENT		19
#define	USBHUB_FEATURE_C_PORT_RESET				20

//
//	Bit mask for checking the hub status
//
#define USBHUB_HUB_STATUS_LOCAL_POWER		0x0001
#define USBHUB_HUB_STATUS_OVER_CURRENT		0x0002

//
//	Bit mask for checking the hub status change
//
#define USBHUB_C_HUB_STATUS_LOCAL_POWER		0x0001
#define USBHUB_C_HUB_STATUS_OVER_CURRENT	0x0002

//
//	Bit mask for checking the port status
//	
#define USBHUB_PORT_STATUS_CONNECTION		0x0001
#define USBHUB_PORT_STATUS_ENABLE			0x0002
#define USBHUB_PORT_STATUS_SUSPEND			0x0004
#define USBHUB_PORT_STATUS_OVER_CURRENT		0x0008
#define USBHUB_PORT_STATUS_RESET			0x0010
#define USBHUB_PORT_STATUS_POWER			0x0100
#define USBHUB_PORT_STATUS_LOW_SPEED		0x0200


//
//	Bit mask for checking the port status change
//	
#define USBHUB_C_PORT_STATUS_CONNECTION		0x0001
#define USBHUB_C_PORT_STATUS_ENABLE			0x0002
#define USBHUB_C_PORT_STATUS_SUSPEND		0x0004
#define USBHUB_C_PORT_STATUS_OVER_CURRENT	0x0008
#define USBHUB_C_PORT_STATUS_RESET			0x0010

//
//	Device Node for a hub
//
typedef struct _USBHUB_DEVICE_NODE
{
	//
	//	First byte is various status bits.
	//
	UCHAR			InUse:1;
	UCHAR			RemovePending:1;
	UCHAR			PoweringPorts:1;
    UCHAR			NotResponding:1;
	UCHAR			Padding:4;
	UCHAR			InterruptEndpointAddress;
	UCHAR			PortCount;
	UCHAR			PortProcessIndex;		//Port currently being processed.
	UCHAR			PortStatusChangeBitmap; //Bitmap of ports whose status has changed
	UCHAR			PortConnectedBitmap;	//Bitmap of ports that are connected
    UCHAR           ErrorCount;
	UCHAR			InterruptEndpointMaxPacket;
	
	URB				Urb;
	
	//
	//	Memory buffer for the URB.  We use a union to format it.
	//
	union
	{
		UCHAR			RequestBuffer[4];
		struct {
			USHORT		Status;
			USHORT		StatusChange;
		}				PortStatus;
	};
	PVOID			InterruptEndpointHandle;
} USBHUB_DEVICE_NODE, *PUSBHUB_DEVICE_NODE;

#define USBHUB_MAXIMUM_HUB_COUNT 6 //Need an extra slop node, in case a reset is done

typedef	struct _USBHUB_INFO
{
	USHORT				NodeCount;
	USHORT				NodesInUse;
	USBHUB_DEVICE_NODE	Nodes[USBHUB_MAXIMUM_HUB_COUNT];
	PUCHAR				EndpointMemory;
} USBHUB_INFO, *PUSBHUB_INFO;

typedef struct _USBHUB_ENUM_BLOCK
{
	UCHAR						EnumBuffer[48];
	//
	//	Resets are not done by the enumeration routine in particular, but USBD guarantees
	//	the serialization of resets across all host controllers.  We are therefore safe
	//	sticking these variables in a global context.
	URB							ResetUrb;
	PVOID						ResetContext;
    KTIMER                      WatchdogTimer;
    KDPC                        WatchdogTimerDpc;
    ULONG                       WatchdogReason;
} USBHUB_ENUM_BLOCK, *PUSBHUB_ENUM_BLOCK;

#define USBHUB_WATCHDOG_REASON_ENUM         0
#define USBHUB_WATCHDOG_REASON_RESET_PORT   1
#define USBHUB_WATCHDOG_REASON_DISABLE_PORT 2

extern USBHUB_INFO			GLOBAL_HubInfo;
extern USBHUB_ENUM_BLOCK	GLOBAL_HubEnum;


//----------------------------------------------------------
//	Entry points from external modules
//----------------------------------------------------------
VOID
USBHUB_AddHub(
	IN IUsbDevice *HubDevice,
	IN PVOID Context
	);

VOID
USBHUB_RemoveHub(
	IN IUsbDevice *HubDevice
	);

VOID
USBHUB_DisableResetPort(
	IN IUsbDevice *HubDevice,
	IN UCHAR	PortNumber,
	IN PVOID	Context,
    IN BOOLEAN  Disable
	);



//----------------------------------------------------------
//	Procedures implemented in hub.c
//----------------------------------------------------------

VOID
USBHUB_EnumHubStage1(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_EnumHubStage2(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_EnumHubComplete(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_PoweringPorts(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_ChangeNotifyComplete(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_ClearInterruptStallComplete(
    PURB	Urb,
	IUsbDevice	*HubDevice
	);

VOID
USBHUB_PortStatusChange1(
	IN IUsbDevice *HubDevice
	);

VOID
USBHUB_PortStatusChange2(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_PortProcessChange(
	IN IUsbDevice *HubDevice
	);

VOID
USBHUB_HubStatusChange(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_ClearHubFeatureComplete(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_ClearPortFeatureComplete(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_RemoveHubStage1(
	IN IUsbDevice *HubDevice
	);

VOID
USBHUB_RemoveHubStage2(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_RemoveHubComplete(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_ResetComplete(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_DisableComplete(
	PURB	Urb,
	IUsbDevice *HubDevice
	);

VOID
USBHUB_RelatchPortSpeed(
    IUsbDevice *HubDevice
    );

VOID
USBHUB_RelatchPortSpeedComplete(
    IN PURB    Urb,
    IUsbDevice *HubDevice
    );

#endif	//__HUB_H__
