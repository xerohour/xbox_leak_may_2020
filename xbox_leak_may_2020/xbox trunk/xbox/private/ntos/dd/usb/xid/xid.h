/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    xid.h

Abstract:

    Structures and functions used through the XID driver.
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    02-22-00 created by Mitchell Dernis (mitchd)

--*/

#ifndef __XID_H__
#define __XID_H__

//-----------------------------------------------
// Byte align all the structures
//-----------------------------------------------
#include <PSHPACK1.H>

//------------------------------------------------------------
// Define pointers to the various types declared in this file.
// This breaks some declaration order dependencies.
//------------------------------------------------------------
typedef struct _XID_TYPE_INFORMATION    *PXID_TYPE_INFORMATION;
typedef struct _XID_DESCRIPTOR          *PXID_DESCRIPTOR;
typedef struct _XID_DEVICE_NODE         *PXID_DEVICE_NODE;
typedef struct _XINPUT_FEEDBACK_HEADER_INTERNAL *PXINPUT_FEEDBACK_HEADER_INTERNAL;
typedef struct _XINPUT_FEEDBACK_INTERNAL *PXINPUT_FEEDBACK_INTERNAL;
typedef struct _XID_OPEN_DEVICE         *PXID_OPEN_DEVICE;
typedef struct _XID_GLOBALS             *PXID_GLOBALS;

//-----------------------------------------------
// Constants for legacy devices - keyboards
//-----------------------------------------------
#define HID_KEYBOARD_PROTOCOL 0x01
#define HID_SET_PROTOCOL      0x0B
#define HID_BOOT_PROTOCOL     0x00
#define HID_SET_IDLE          0x0A
#define HID_IDLE_INFINITE     0x00

//--------------------------------------------------
//  XINPUT_KEYBOARD is not defined in public headers
//  since we ripped out low-level support
//--------------------------------------------------
typedef struct _XINPUT_KEYBOARD
{
    BYTE Modifiers;
    BYTE Reserved;
    BYTE Keys[6];
} XINPUT_KEYBOARD, *PXINPUT_KEYBOARD;
typedef struct _XINPUT_KEYBOARD_LEDS
{
    BYTE LedStates;
} XINPUT_KEYBOARD_LEDS, *PXINPUT_KEYBOARD_LEDS;

//-----------------------------------------------
// Constants defined in the XID specification
//-----------------------------------------------
#define XID_COMMAND_GET_CAPABILITIES    0x01
#define XID_COMMAND_GET_REPORT          0x01
#define XID_COMMAND_SET_REPORT          0x09
#define XID_MAXIMUM_REPORT_SIZE         32
#define XID_DESCRIPTOR_TYPE             0x4200
#define XID_REPORT_TYPE_INPUT           0x01
#define XID_REPORT_TYPE_OUTPUT          0x02
#define XID_IS_INPUT_REPORT(wReportId)  ((wReportId >> 8) == XID_REPORT_TYPE_INPUT)
#define XID_GET_REPORT_ID(wReportId)    (wReportId&0xFF)
#define XID_REPORT_HEADER               0x02 //The Report ID and Size is the header.

//------------------------------------------------
// Private constants related to device types and
// and subtypes.
//------------------------------------------------
#define  XID_DEVTYPE_COUNT                3

//Game Controllers
#define  XID_DEVTYPE_GAMECONTROLLER        0
#define  XID_DEVSUBTYPE_MAX_GAME           1
#define  XID_INPUT_REPORT_ID_MAX_GAME      0
#define  XID_OUTPUT_REPORT_ID_MAX_GAME     0

//Keyboard
#define  XID_DEVTYPE_KEYBOARD              1
#define  XID_DEVSUBTYPE_MAX_KEYBOARD       1
#define  XID_INPUT_REPORT_ID_MAX_KEYBOARD  0
#define  XID_OUTPUT_REPORT_ID_MAX_KEYBOARD 0

//IR Remote
#define  XID_DEVTYPE_IRREMOTE              2
#define  XID_DEVSUBTYPE_MAX_IRREMOTE       1
#define  XID_INPUT_REPORT_ID_MAX_IRREMOTE  0
#define  XID_OUTPUT_REPORT_ID_MAX_IRREMOTE 0

//  Right now we have a device that plugs directly into the box,
//  So it is only physically possible to have two devices per port.
//  If we add a high power device, this should be two.
//  If we add a low power device, this should be three.
//
#define XID_MAX_DEVICE_PER_PORT           1

//-----------------------------------------------------------------------
// Structures for the device information table
//-----------------------------------------------------------------------
typedef void (FASTCALL *PFN_PROCESS_NEWDATA)(PXID_OPEN_DEVICE OpenDevice);

typedef struct _XID_REPORT_TYPE_INFO
{
    BYTE    bCurrentSize;   //Not including bReportId and bSize
    PVOID   pDefaultValues;
} XID_REPORT_TYPE_INFO, *PXID_REPORT_TYPE_INFO;

typedef struct _XID_TYPE_INFORMATION
{
    BYTE                       bRemainingHandles;
    BYTE                       bMaxSubType;
    BYTE                       bMaxInputReportId;
    BYTE                       bMaxOutputReportId;
    PXID_REPORT_TYPE_INFO      pInputReportInfoList;
    PXID_REPORT_TYPE_INFO      pOutputReportInfoList;
    PXINPUT_POLLING_PARAMETERS DefaultPollingParameters;
    PFN_PROCESS_NEWDATA        pfnProcessNewData;
    ULONG                      ulFlags;
} XID_TYPE_INFORMATION;
#define XID_BSF_NO_CAPABILITIES  0x00000001
#define XID_BSF_NO_OUTPUT_HEADER 0x00000002

//----------------------------------------------------------------------
//  Declare the table for the type information.
//----------------------------------------------------------------------
extern XID_TYPE_INFORMATION XID_TypeInformationList[XID_DEVTYPE_COUNT];

//----------------------------------------------------------------------
//  This the XID Descriptor has defined in the XID Specification
//----------------------------------------------------------------------
typedef struct _XID_DESCRIPTOR
{
    UCHAR   bLength;
    UCHAR   bDescriptorType;
    USHORT  bcdXid;
    UCHAR   bType;
    UCHAR   bSubType;
    UCHAR   bMaxInputReportSize;
    UCHAR   bMaxOutputReportSize;
    USHORT  wAlternateProductIds[4];
} XID_DESCRIPTOR; 

//----------------------------------------------------------------------
//  Service Table For Extended Keyboard API
//    In order to separate the extended keyboard API from the core
//    Xbox API, the extended API registers itself with a pointer to
//    this Service Table.
//----------------------------------------------------------------------
typedef void (*PFNKEYBOARD_OPENED)(HANDLE hDevice);
typedef void (*PFNKEYBOARD_CLOSED)(HANDLE hDevice);
typedef void (*PFNKEYBOARD_REMOVED)(HANDLE hDevice);
typedef void (*PFNKEYBOARD_NEWDATA)(HANDLE hDevice, XINPUT_KEYBOARD *pPacket);
typedef struct _XID_KEYBOARD_SERVICES
{
    PFNKEYBOARD_OPENED   pfnOpen;
    PFNKEYBOARD_CLOSED   pfnClose;
    PFNKEYBOARD_REMOVED  pfnRemove;
    PFNKEYBOARD_NEWDATA  pfnNewData;
} XID_KEYBOARD_SERVICES, *PXID_KEYBOARD_SERVICES;
extern PXID_KEYBOARD_SERVICES XID_pKeyboardServices;

//----------------------------------------------------------------------
//  This structure represents a device node.
//----------------------------------------------------------------------
typedef struct _XID_DEVICE_NODE
{
    IUsbDevice              *Device;
       
    UCHAR                   InUse:1;
    UCHAR                   PendingRemove:1;
    UCHAR                   Opened:1;
    UCHAR                   Ready:1;
    UCHAR                   Padding:4;
    UCHAR                   InterfaceNumber;
    UCHAR                   MaxPacketIn;        //MaxPacket is a WORD, but per specification cannot exceed 64.
    UCHAR                   MaxPacketOut;       //MaxPacket is a WORD, but per specification cannot exceed 64.
    UCHAR                   EndpointAddressIn;
    UCHAR                   EndpointAddressOut;
    UCHAR                   Type;
    UCHAR                   SubType;
    UCHAR                   bMaxInputReportSize;
    UCHAR                   bMaxOutputReportSize;
    PXID_OPEN_DEVICE        OpenDevice;
} XID_DEVICE_NODE;
#define XID_INVALID_NODE_INDEX  0xFF

//----------------------------------------------------------------------
//  Structures for output.  These are the internal equivalent
//  of XOUTPUT_HEADER and XOUTPUT_REPORT.
//----------------------------------------------------------------------
typedef struct _XINPUT_FEEDBACK_HEADER_INTERNAL
{
    PXID_OPEN_DEVICE        OpenDevice;
    PKEVENT                 CompletionEvent;        
    URB                     Urb;
    BYTE                    bReportId;
    BYTE                    bSize;
} XINPUT_FEEDBACK_HEADER_INTERNAL;

typedef struct _XINPUT_FEEDBACK_INTERNAL
{
    DWORD                   dwStatus;
    HANDLE                  hEvent;
    XINPUT_FEEDBACK_HEADER_INTERNAL Internal;
} XINPUT_FEEDBACK_INTERNAL;

typedef struct _XID_OPEN_DEVICE
{
    PXID_DEVICE_NODE    XidNode;
    LONG                OutstandingPoll;
    ULONG               PacketNumber;
    PVOID               InterruptInEndpointHandle;
    PVOID               InterruptOutEndpointHandle;
    UCHAR               Report[XID_MAXIMUM_REPORT_SIZE-2];
    UCHAR               ReportForUrb[XID_MAXIMUM_REPORT_SIZE];
    URB                 Urb;
    URB_CLOSE_ENDPOINT  CloseUrb;
    PKEVENT             CloseEvent;
    UCHAR               ClosePending:1;
    UCHAR               ControlEndpointOpen:1;
    UCHAR               EndpointsPendingClose:1;
    UCHAR               AutoPoll:1;
    UCHAR               Pad:4;
    UCHAR               Type;
    PXID_OPEN_DEVICE    NextOpenDevice;
} XID_OPEN_DEVICE;
#include <POPPACK.H>

//------------------------------------------------------------
//  A structure for the global variables.
//  The only exceptions are a few data tables.
//------------------------------------------------------------
typedef struct _XID_GLOBALS
{
    USHORT                  DeviceNodeCount;
    USHORT                  DeviceNodeInUseCount;
    PXID_DEVICE_NODE        DeviceNodes;
    PXID_OPEN_DEVICE        DeviceHandles;
    //
    //  Data used for enumeration
    //
    union
    {
        XID_DESCRIPTOR          EnumXidDescriptor;
        USB_DEVICE_DESCRIPTOR   EnumDeviceDescriptor;
    };
    URB                         EnumUrb;
    UCHAR                       EnumNode;
    KTIMER                      EnumWatchdogTimer;
    KDPC                        EnumWatchdogTimerDpc;
} XID_GLOBALS;
extern XID_GLOBALS XID_Globals;

//--------------------------------------------------------------------------------------
//  Functions called from input.cpp
//--------------------------------------------------------------------------------------
DWORD
FASTCALL
XID_fOpenDevice(
    UCHAR            XidType,
    ULONG            Port,
    PXID_OPEN_DEVICE *OpenDevice,
    PXINPUT_POLLING_PARAMETERS PollingParameters
    );

VOID
FASTCALL
XID_fCloseDevice(
    IN PXID_OPEN_DEVICE OpenDevice
    );


DWORD
FASTCALL
XID_fSendDeviceReport(
        IN      PXID_OPEN_DEVICE           OpenDevice,
        IN      PXINPUT_FEEDBACK_INTERNAL   OutputReport
        );

VOID
XID_SyncComplete(
    PURB /*UnreferencedUrb*/,
    PKEVENT SyncEvent
    );

__inline 
PXID_OPEN_DEVICE 
XID_AllocateHandle()
{
    PXID_OPEN_DEVICE retVal = XID_Globals.DeviceHandles;
    ASSERT(retVal); //if hit we under allocated somehow.
    XID_Globals.DeviceHandles = retVal->NextOpenDevice;
    return retVal;
}

__inline 
VOID
XID_FreeHandle(PXID_OPEN_DEVICE DeviceHandle)
{
    DeviceHandle->NextOpenDevice = XID_Globals.DeviceHandles;
    XID_Globals.DeviceHandles = DeviceHandle;
}

//--------------------------------------------------------------------------------------
//  Type Specific Functions
//--------------------------------------------------------------------------------------
void
FASTCALL
XID_ProcessGamepadData(
    PXID_OPEN_DEVICE OpenDevice
    );

void
FASTCALL
XID_ProcessIRRemoteData(
    PXID_OPEN_DEVICE OpenDevice
    );

void
FASTCALL
XID_ProcessNewKeyboardData(
    PXID_OPEN_DEVICE OpenDevice
    );

#endif  //__XID_H__
