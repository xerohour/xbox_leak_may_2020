/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    hcdi.h

    Generated from usb.x

Abstract:

    Header File for  host controller driver

Environment:

    Xbox

--*/

#ifndef __USB_X__
#define __USB_X__



//
//	USB drivers, and XAPI code modules that rely on USB all go into 
//	the XPP section.
//
#pragma code_seg(".XPPCODE")
#pragma data_seg(".XPP$Data")
#pragma const_seg(".XPPRDATA")

#include <usb100.h>

//
// XBOX platform USB device classes
//

#define XBOX_DEVICE_CLASS_INPUT_DEVICE              0x58
#define XBOX_DEVICE_CLASS_XDCS                      0x59
#define XBOX_DEVICE_CLASS_AUDIO_DEVICE              0x78

//
// USBD status codes
//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+---------------------------+-------------------------------+
//  | S |               Status Code                                 |
//  +---+---------------------------+-------------------------------+
//
//  where
//
//      S - is the state code
//
//          00 - completed with success
//          01 - request is pending
//          10 - completed with error, endpoint not stalled
//          11 - completed with error, endpoint stalled
//
//
//      Code - is the status code
//

typedef LONG USBD_STATUS;

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define USBD_SUCCESS(Status) ((USBD_STATUS)(Status) >= 0)

//
// Generic test for pending status value.
//

#define USBD_PENDING(Status) ((ULONG)(Status) >> 30 == 1)

//
// Generic test for error on any status value.
//

#define USBD_ERROR(Status) ((USBD_STATUS)(Status) < 0)

//
//  Macro to ensure that error bit is set.
//
#define SET_USBD_ERROR(err)  ((err) | USBD_STATUS_ERROR)


//
// Generic test for stall on any status value.
//

#define USBD_HALTED(Status) ((ULONG)(Status) >> 30 == 3)

#define USBD_STATUS_SUCCESS                  ((USBD_STATUS)0x00000000L)
#define USBD_STATUS_PENDING                  ((USBD_STATUS)0x40000000L)
#define USBD_STATUS_HALTED                   ((USBD_STATUS)0xC0000000L)
#define USBD_STATUS_ERROR                    ((USBD_STATUS)0x80000000L)

//
// HC status codes
// Note: these status codes have both the error and the stall bit set.
//
#define USBD_STATUS_CRC                      ((USBD_STATUS)0xC0000001L)
#define USBD_STATUS_BTSTUFF                  ((USBD_STATUS)0xC0000002L)
#define USBD_STATUS_DATA_TOGGLE_MISMATCH     ((USBD_STATUS)0xC0000003L)
#define USBD_STATUS_STALL_PID                ((USBD_STATUS)0xC0000004L)
#define USBD_STATUS_DEV_NOT_RESPONDING       ((USBD_STATUS)0xC0000005L)
#define USBD_STATUS_PID_CHECK_FAILURE        ((USBD_STATUS)0xC0000006L)
#define USBD_STATUS_UNEXPECTED_PID           ((USBD_STATUS)0xC0000007L)
#define USBD_STATUS_DATA_OVERRUN             ((USBD_STATUS)0xC0000008L)
#define USBD_STATUS_DATA_UNDERRUN            ((USBD_STATUS)0xC0000009L)
#define USBD_STATUS_RESERVED1                ((USBD_STATUS)0xC000000AL)
#define USBD_STATUS_RESERVED2                ((USBD_STATUS)0xC000000BL)
#define USBD_STATUS_BUFFER_OVERRUN           ((USBD_STATUS)0xC000000CL)
#define USBD_STATUS_BUFFER_UNDERRUN          ((USBD_STATUS)0xC000000DL)
#define USBD_STATUS_NOT_ACCESSED             ((USBD_STATUS)0xC000000EL)
#define USBD_STATUS_FIFO                     ((USBD_STATUS)0xC0000010L)

//
// HC status codes
// Note: these status codes have the error bit and not the stall bit set.
//
#define USBD_ISOCH_STATUS_CRC                      (1)
#define USBD_ISOCH_STATUS_BTSTUFF                  (2)
#define USBD_ISOCH_STATUS_DATA_TOGGLE_MISMATCH     (3)
#define USBD_ISOCH_STATUS_STALL_PID                (4)
#define USBD_ISOCH_STATUS_DEV_NOT_RESPONDING       (5)
#define USBD_ISOCH_STATUS_PID_CHECK_FAILURE        (6)
#define USBD_ISOCH_STATUS_UNEXPECTED_PID           (7)
#define USBD_ISOCH_STATUS_DATA_OVERRUN             (8)
#define USBD_ISOCH_STATUS_DATA_UNDERRUN            (9)
#define USBD_ISOCH_STATUS_RESERVED1                (A)
#define USBD_ISOCH_STATUS_RESERVED2                (B)
#define USBD_ISOCH_STATUS_BUFFER_OVERRUN           (C)
#define USBD_ISOCH_STATUS_BUFFER_UNDERRUN          (D)
#define USBD_ISOCH_STATUS_NOT_ACCESSED             (E)

//
// returned by HCD if a transfer is submitted to an endpoint that is 
// stalled
//
#define USBD_STATUS_ENDPOINT_HALTED         ((USBD_STATUS)0xC0000030L)

//
// Software status codes
// Note: the following status codes have only the error bit set
//
#define USBD_STATUS_NO_MEMORY                ((USBD_STATUS)0x80000100L)
#define USBD_STATUS_INVALID_URB_FUNCTION     ((USBD_STATUS)0x80000200L)
#define USBD_STATUS_INVALID_PARAMETER        ((USBD_STATUS)0x80000300L)
#define USBD_STATUS_UNSUPPORTED_DEVICE       ((USBD_STATUS)0x80000400L)
#define USBD_STATUS_TRANSFER_TOO_LONG        ((USBD_STATUS)0x80000500L)

//
// returned by USBD if it cannot complete a URB request, typically this 
// will be returned in the URB status field when the Irp is completed
// with a more specific NT error code in the irp.status field.
//
#define USBD_STATUS_REQUEST_FAILED           ((USBD_STATUS)0x80000600L)

#define USBD_STATUS_NO_DEVICE                ((USBD_STATUS)0x80000700L)

// returned when there is not enough bandwidth avialable
// to open a requested endpoint
#define USBD_STATUS_NO_BANDWIDTH             ((USBD_STATUS)0x80000800L)
//
// generic HC error
// 
#define USBD_STATUS_INTERNAL_HC_ERROR        ((USBD_STATUS)0x80000900L)
//
// returned when a short packet terminates the transfer
// ie USBD_SHORT_TRANSFER_OK bit not set
// 
#define USBD_STATUS_ERROR_SHORT_TRANSFER     ((USBD_STATUS)0x80000A00L)


// 
// returned if the requested start frame is not within
// USBD_ISOCH_START_FRAME_RANGE of the current USB frame, 
// note that the stall bit is set
// 
#define USBD_STATUS_BAD_START_FRAME          ((USBD_STATUS)0xC0000B00L)
//
// returned by HCD if all packets in an iso transfer complete with an error 
//
#define USBD_STATUS_ISOCH_REQUEST_FAILED     ((USBD_STATUS)0xC0000C00L)
//
// returned by HCD if an attempt is made to attach more isoch
// buffers to an endpoint than specified as the maximum when opening
// the endpoint.
//
#define USBD_STATUS_ISOCH_TOO_MANY_BUFFERS   ((USBD_STATUS)0xC0000D00L)
//
// returned by HCD if an attempt is made to start an endpoint which is
// already started.
//
#define USBD_STATUS_ISOCH_ALREADY_STARTED    ((USBD_STATUS)0xC0000E00L)
//
// returned by HCD if an attempt is made to stop an endpoint which is
// not already started.
//
#define USBD_STATUS_ISOCH_NOT_STARTED        ((USBD_STATUS)0xC0000F00L)
//
// returned by HCD if an attempt is made to start an endpoint setup for
// circular DMA with fewer than MaxAttachedBuffers,
//
#define USBD_STATUS_ISOCH_ATTACH_MORE_BUFFERS ((USBD_STATUS)0xC0001000L)
//
// This build of the usb driver does not support isochronous requests.
//
#define USBD_STATUS_ISOCH_NOT_SUPPORTED      ((USBD_STATUS)0xC0002000L)
//
// set when a transfers is completed due to an AbortPipe request from
// the client driver
//
#define USBD_STATUS_CANCELED                 ((USBD_STATUS)0xC000000FL)
#define USBD_STATUS_CANCELING                ((USBD_STATUS)0x40020000L)

//
//	Status returned by hub, when a reset completes successfully and the
//	device is low-speed.
//
#define USBD_STATUS_LOWSPEED				 ((USBD_STATUS)0x01000000L)


//------------------------------------------------------------------------------------
// URB Function Codes   - High bit indicates that USBD (pre-)processes URB
//------------------------------------------------------------------------------------
//-- special bit in URB_FUNCTION codes -----------------
#define URB_FUNCTION_USBD_PROCESSED                 0x80    
#define URB_FUNCTION_ASYNCHRONOUS                   0x40
//------------------------------------------------------
#define URB_FUNCTION_CONTROL_TRANSFER               (0x00 | URB_FUNCTION_ASYNCHRONOUS)
#define URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER     (0x01 | URB_FUNCTION_ASYNCHRONOUS)
#define URB_FUNCTION_OPEN_ENDPOINT                  0x02
#define URB_FUNCTION_CLOSE_ENDPOINT                 (0x03 | URB_FUNCTION_ASYNCHRONOUS)
#define URB_FUNCTION_GET_ENDPOINT_STATE             0x04
#define URB_FUNCTION_SET_ENDPOINT_STATE             0x05
#define URB_FUNCTION_ABORT_ENDPOINT                 (0x06 | URB_FUNCTION_ASYNCHRONOUS)
#define URB_FUNCTION_GET_FRAME_NUMBER               0x07
#define URB_FUNCTION_OPEN_DEFAULT_ENDPOINT          (URB_FUNCTION_USBD_PROCESSED | URB_FUNCTION_OPEN_ENDPOINT)
#define URB_FUNCTION_CLOSE_DEFAULT_ENDPOINT         (URB_FUNCTION_USBD_PROCESSED | URB_FUNCTION_CLOSE_ENDPOINT)
#define URB_FUNCTION_RESET_PORT                     (URB_FUNCTION_USBD_PROCESSED | 0x08)
#define URB_FUNCTION_ISOCH_OPEN_ENDPOINT            0x09
#define URB_FUNCTION_ISOCH_CLOSE_ENDPOINT           (0x0A | URB_FUNCTION_ASYNCHRONOUS)
#define URB_FUNCTION_ISOCH_ATTACH_BUFFER            0x0B
#define URB_FUNCTION_ISOCH_START_TRANSFER           0x0C
#define URB_FUNCTION_ISOCH_STOP_TRANSFER            0x0D

//------------------------------------------------------------------------------------
//  Values for the transfer directions
//------------------------------------------------------------------------------------
#define USB_TRANSFER_DIRECTION_OUT              0x01
#define USB_TRANSFER_DIRECTION_IN               0x02

//----------------------------------------------------------------------------------------------------------------------
// Flags for URB_FUNCTION_GET_ENDPOINT_STATE
//----------------------------------------------------------------------------------------------------------------------
#define USB_ENDPOINT_STATE_HALTED              	0x01    //Set on return if endpoint is halted.
#define USB_ENDPOINT_STATE_TRANSFERS_QUEUED     0x02    //Set on return if one or more transfers are queued to endpoint.

//----------------------------------------------------------------------------------------------------------------------
// Flags for URB_FUNCTION_SET_ENDPOINT_STATE
//----------------------------------------------------------------------------------------------------------------------
#define USB_ENDPOINT_STATE_CLEAR_HALT		    0x00    //Clear endpoint halt.
#define USB_ENDPOINT_STATE_KEEP_HALT		    0x01    //Do not clear enddpoint halt
#define USB_ENDPOINT_STATE_DATA_TOGGLE_RESET    0x04    //Reset data toggle. (i.e. DATA0)
#define USB_ENDPOINT_STATE_DATA_TOGGLE_SET      0x08    //Set data toggle. (i.e. DATA1), provided for testing.
//There is no USB_ENDPOINT_STATE_SET_HALT, since only the hardware may set it.

//----------------------------------------------------------------------------------------------------------------------
// Macros for the InterruptDelay variable in transfer and attach buffer URBs - this is really an OpenHCI thing,
// but gives more control to class drivers.  These are the same as OHCI_TD_DELAY_INTERRUPT_XXX
// DUE TO BUG 9512 IT IS ONLY SAFE TO USE USBD_DELAY_INTERRUPT_0_MS.  OTHERWISE, YOU MAY EXPERIENCE PROBLEMS
// WHEN CLOSING THE ENDPOINT.  SEE BUG 9512 FOR MORE DETAILS.
//----------------------------------------------------------------------------------------------------------------------
#define USBD_DELAY_INTERRUPT_0_MS        0   // Interrupt at end of frame TD is completed
#define USBD_DELAY_INTERRUPT_1_MS        1   // Interrupt within 1 frame of TD compeletion
#define USBD_DELAY_INTERRUPT_2_MS        2   // Interrupt within 2 frames of TD compeletion
#define USBD_DELAY_INTERRUPT_3_MS        3   // Interrupt within 3 frame of TD compeletion
#define USBD_DELAY_INTERRUPT_4_MS        4   // Interrupt within 4 frame of TD compeletion
#define USBD_DELAY_INTERRUPT_5_MS        5   // Interrupt within 5 frame of TD compeletion
#define USBD_DELAY_INTERRUPT_6_MS        6   // Interrupt within  frame of TD compeletion
#define USBD_DELAY_INTERRUPT_NONE        7   // Do not Interrupt upon completion of TD

//------------------------------------------------------------------------------------
// USBD Structures for class drivers
//------------------------------------------------------------------------------------
typedef union _URB *PURB;
typedef VOID (*PURB_COMPLETE_PROC)(PURB Urb, PVOID Context);

//
//  See approptate HCD header file for
//  for flags.
//
struct _URB_HCD_AREA
{
    union
    {
        USHORT  HcdTDCount;
        USHORT  HcdOriginalLength;  //While programed contains the original length
    
	};
    USHORT  HcdUrbFlags;
	PURB    HcdUrbLink;     //Used while the URB is pending
};

struct _URB_HEADER
{
    UCHAR               Length;
    UCHAR               Function;
    USBD_STATUS         Status;
    PURB_COMPLETE_PROC  CompleteProc;       // A completetion routine if the URB is not associated with an Irp
    PVOID               CompleteContext;    // Context to pass into the completion routine.
};

typedef struct _USB_CONTROL_SETUP_PACKET
{
    UCHAR   bmRequestType;
    UCHAR   bRequest;
    USHORT  wValue;
    USHORT  wIndex;
    USHORT  wLength;
} USB_CONTROL_SETUP_PACKET;

typedef struct _URB_CONTROL_TRANSFER
{
    struct _URB_HEADER          Hdr;                    
    PVOID                       EndpointHandle;
    ULONG                       TransferBufferLength;
    PVOID                       TransferBuffer;
    UCHAR                       TransferDirection;
    BOOLEAN                     ShortTransferOK;
    UCHAR                       InterruptDelay;
    UCHAR                       Padding;
    struct _URB_HCD_AREA        Hca;
    USB_CONTROL_SETUP_PACKET    SetupPacket;
} URB_CONTROL_TRANSFER, *PURB_CONTROL_TRANSFER;

typedef struct _URB_BULK_OR_INTERRUPT_TRANSFER
{
    struct _URB_HEADER      Hdr;
    PVOID                   EndpointHandle;
    ULONG                   TransferBufferLength;
    PVOID                   TransferBuffer;
    UCHAR                   TransferDirection;
    BOOLEAN                 ShortTransferOK;
    UCHAR                   InterruptDelay;
    UCHAR                   Padding;
    struct _URB_HCD_AREA    Hca;   // fields for HCD use
} URB_BULK_OR_INTERRUPT_TRANSFER, *PURB_BULK_OR_INTERRUPT_TRANSFER;


typedef struct _URB_OPEN_ENDPOINT
{
    struct _URB_HEADER          Hdr;
    PVOID                       EndpointHandle;
    UCHAR                       FunctionAddress;
    UCHAR                       EndpointAddress;
    UCHAR                       EndpointType;
    UCHAR                       Interval;
    PULONG                      DataToggleBits; // Filled out by USBD
    USHORT                      MaxPacketSize;
    BOOLEAN                     LowSpeed;   //Temporary - filled out by USBD
} URB_OPEN_ENDPOINT, *PURB_OPEN_ENDPOINT;

typedef struct _URB_CLOSE_ENDPOINT
{
    struct _URB_HEADER     Hdr;
    PVOID                  EndpointHandle;
    PURB                   HcdNextClose;   // An extra pointer which Hcd can use to link pending closes.
    PULONG                 DataToggleBits; // Filled out by USBD
} URB_CLOSE_ENDPOINT, *PURB_CLOSE_ENDPOINT;

typedef struct _URB_GET_SET_ENDPOINT_STATE
{
    struct _URB_HEADER      Hdr;
    PVOID                   EndpointHandle;
    ULONG                   EndpointState;
} URB_GET_SET_ENDPOINT_STATE, *PURB_GET_SET_ENDPOINT_STATE;

typedef struct _URB_ABORT_ENDPOINT
{
    struct _URB_HEADER      Hdr;
    PVOID                   EndpointHandle;
    PURB                    HcdNextAbort;   // An extra pointer which Hcd can use to link pending aborts.
} URB_ABORT_ENDPOINT, *PURB_ABORT_ENDPOINT;

typedef struct _URB_RESET_PORT
{
    struct _URB_HEADER  Hdr;
    UCHAR               DeviceNode;
    UCHAR               PortNumber;
} URB_RESET_PORT, *PURB_RESET_PORT;

typedef struct _URB_GET_FRAME_NUMBER
{
    struct _URB_HEADER  Hdr;
    UCHAR               DeviceNode;
    ULONG               FrameNumber;
} URB_GET_FRAME_NUMBER, *PURB_GET_FRAME_NUMBER;


//------------------------------------------------
// Isoch related URBs and related structures
//------------------------------------------------
typedef struct _USBD_ISOCH_PACKET_STATUS_WORD
{
    USHORT BytesRead:12;
    USHORT ConditionCode:4;
} USBD_ISOCH_PACKET_STATUS_WORD, *PUSBD_ISOCH_PACKET_STATUS_WORD;

typedef struct _USBD_ISOCH_TRANSFER_STATUS
{
    USBD_STATUS                     Status;
    ULONG                           FrameCount;
    USBD_ISOCH_PACKET_STATUS_WORD   PacketStatus[8];
} USBD_ISOCH_TRANSFER_STATUS, *PUSBD_ISOCH_TRANSFER_STATUS;

typedef VOID (*PFNUSBD_ISOCH_TRANSFER_COMPLETE)(PUSBD_ISOCH_TRANSFER_STATUS Status, PVOID Context);

typedef struct _USBD_ISOCH_BUFFER_DESCRIPTOR
{
    ULONG                               FrameCount;
    PVOID                               TransferBuffer;
    USHORT                              Pattern[8];
    PFNUSBD_ISOCH_TRANSFER_COMPLETE     TransferComplete;
    PVOID                               Context;
} USBD_ISOCH_BUFFER_DESCRIPTOR, *PUSBD_ISOCH_BUFFER_DESCRIPTOR;

typedef struct _URB_ISOCH_ATTACH_BUFFER
{
    struct _URB_HEADER            Hdr;
    PVOID                         EndpointHandle;
    UCHAR                         InterruptDelay;
    PUSBD_ISOCH_BUFFER_DESCRIPTOR BufferDescriptor;
} URB_ISOCH_ATTACH_BUFFER, *PURB_ISOCH_ATTACH_BUFFER;

typedef struct _URB_ISOCH_START_TRANSFER
{
    struct _URB_HEADER     Hdr;
    PVOID                  EndpointHandle;
    ULONG                  FrameNumber;
    ULONG                  Flags;
} URB_ISOCH_START_TRANSFER, *PURB_ISOCH_START_TRANSFER;
#define URB_FLAG_ISOCH_START_ASAP 0x0001
#define USBD_ISOCH_START_FRAME_RANGE 1024

typedef struct _URB_ISOCH_STOP_TRANSFER
{
    struct _URB_HEADER     Hdr;
    PVOID                  EndpointHandle;
} URB_ISOCH_STOP_TRANSFER, *PURB_ISOCH_STOP_TRANSFER;

typedef struct _URB_ISOCH_OPEN_ENDPOINT
{
    struct _URB_HEADER              Hdr;
    PVOID                           EndpointHandle;
    UCHAR                           FunctionAddress;
    UCHAR                           EndpointAddress;
    USHORT                          MaxPacketSize;
    USHORT                          Flags;
    USHORT                          Pad;
} URB_ISOCH_OPEN_ENDPOINT, *PURB_ISOCH_OPEN_ENDPOINT;
#define URB_FLAG_ISOCH_CIRCULAR_DMA 0x0001

typedef struct _URB_CLOSE_ENDPOINT URB_ISOCH_CLOSE_ENDPOINT, *PURB_ISOCH_CLOSE_ENDPOINT;

//------------------------------------------------
// Union of all URBs
//------------------------------------------------
typedef union _URB
{
    struct _URB_HEADER              Header;
    URB_CONTROL_TRANSFER            ControlTransfer;
    URB_BULK_OR_INTERRUPT_TRANSFER  BulkOrInterruptTransfer;
    URB_BULK_OR_INTERRUPT_TRANSFER  CommonTransfer;
    URB_OPEN_ENDPOINT               OpenEndpoint;
    URB_CLOSE_ENDPOINT              CloseEndpoint;
    URB_GET_SET_ENDPOINT_STATE      GetSetEndpointState;
    URB_ABORT_ENDPOINT              AbortEndpoint;
    URB_RESET_PORT                  ResetPort;
    URB_GET_FRAME_NUMBER            GetFrame;
    URB_ISOCH_ATTACH_BUFFER         IsochAttachBuffer;
    URB_ISOCH_START_TRANSFER        IsochStartTransfer;
    URB_ISOCH_STOP_TRANSFER         IsochStopTransfer;
    URB_ISOCH_OPEN_ENDPOINT         IsochOpenEndpoint;
    URB_ISOCH_CLOSE_ENDPOINT        IsochCloseEndpoint;
} URB;

//------------------------------------------------------------------------------------
// Macros to build USB Request Blocks
//------------------------------------------------------------------------------------
#define USB_BUILD_CONTROL_TRANSFER(\
                        _UrbControlTransfer_,\
                        _EndpointHandle_,\
                        _TransferBuffer_,\
                        _TransferBufferLength_,\
                        _TransferDirection_,\
                        _CompleteProc_,\
                        _CompleteContext_,\
                        _ShortTransferOK_,\
                        _bmRequestType_,\
                        _bRequest_,\
                        _wValue_,\
                        _wIndex_,\
                        _wLength_\
    )\
    {\
        (_UrbControlTransfer_)->Hdr.Length                  =   sizeof(URB_CONTROL_TRANSFER);\
        (_UrbControlTransfer_)->Hdr.Function                =   URB_FUNCTION_CONTROL_TRANSFER;\
        (_UrbControlTransfer_)->Hdr.CompleteProc            =   (_CompleteProc_);\
        (_UrbControlTransfer_)->Hdr.CompleteContext         =   (_CompleteContext_);\
        (_UrbControlTransfer_)->EndpointHandle              =   (_EndpointHandle_);\
        (_UrbControlTransfer_)->TransferBuffer              =   (_TransferBuffer_);\
        (_UrbControlTransfer_)->TransferBufferLength        =   (_TransferBufferLength_);\
        (_UrbControlTransfer_)->TransferDirection           =   (_TransferDirection_);\
        (_UrbControlTransfer_)->ShortTransferOK             =   (_ShortTransferOK_);\
        (_UrbControlTransfer_)->InterruptDelay              =   USBD_DELAY_INTERRUPT_0_MS;\
        (_UrbControlTransfer_)->SetupPacket.bmRequestType   =   (_bmRequestType_);\
        (_UrbControlTransfer_)->SetupPacket.bRequest        =   (_bRequest_);\
        (_UrbControlTransfer_)->SetupPacket.wValue          =   (_wValue_);\
        (_UrbControlTransfer_)->SetupPacket.wIndex          =   (_wIndex_);\
        (_UrbControlTransfer_)->SetupPacket.wLength         =   (_wLength_);\
    }


#define USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(\
                        _UrbBulkOrInterruptTransfer_,\
                        _EndpointHandle_,\
                        _TransferBuffer_,\
                        _TransferBufferLength_,\
                        _TransferDirection_,\
                        _CompleteProc_,\
                        _CompleteContext_,\
                        _ShortTransferOK_\
    )\
    {\
        (_UrbBulkOrInterruptTransfer_)->Hdr.Length          =   sizeof(URB_BULK_OR_INTERRUPT_TRANSFER);\
        (_UrbBulkOrInterruptTransfer_)->Hdr.Function        =   URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER;\
        (_UrbBulkOrInterruptTransfer_)->Hdr.CompleteProc    =   (_CompleteProc_);\
        (_UrbBulkOrInterruptTransfer_)->Hdr.CompleteContext =   (_CompleteContext_);\
        (_UrbBulkOrInterruptTransfer_)->EndpointHandle      =   (_EndpointHandle_);\
        (_UrbBulkOrInterruptTransfer_)->TransferBuffer      =   (_TransferBuffer_);\
        (_UrbBulkOrInterruptTransfer_)->TransferBufferLength=   (_TransferBufferLength_);\
        (_UrbBulkOrInterruptTransfer_)->TransferDirection   =   (_TransferDirection_);\
        (_UrbBulkOrInterruptTransfer_)->ShortTransferOK     =   (_ShortTransferOK_);\
        (_UrbBulkOrInterruptTransfer_)->InterruptDelay      =   USBD_DELAY_INTERRUPT_0_MS;\
    }

//
// Isoch requests
//

#define USB_BUILD_ISOCH_OPEN_ENDPOINT(\
                        _UrbIsochOpenEndpoint_,\
                        _EndpointAddress_,\
                        _MaxPacketSize_,\
                        _Flags_\
    )\
    {\
        (_UrbIsochOpenEndpoint_)->Hdr.Length         =   sizeof(URB_ISOCH_OPEN_ENDPOINT);\
        (_UrbIsochOpenEndpoint_)->Hdr.Function       =   URB_FUNCTION_ISOCH_OPEN_ENDPOINT;\
        (_UrbIsochOpenEndpoint_)->Hdr.CompleteProc   =   NULL;\
        (_UrbIsochOpenEndpoint_)->Hdr.CompleteContext=   NULL;\
        (_UrbIsochOpenEndpoint_)->EndpointAddress    =   (_EndpointAddress_);\
        (_UrbIsochOpenEndpoint_)->MaxPacketSize      =   (_MaxPacketSize_);\
        (_UrbIsochOpenEndpoint_)->Flags              =   (_Flags_);\
    }

#define USB_BUILD_ISOCH_CLOSE_ENDPOINT(\
                        _UrbIsochCloseEndpoint_,\
                        _EndpointHandle_,\
						_CompleteProc_,\
						_CompleteContext_\
    )\
    {\
        (_UrbIsochCloseEndpoint_)->Hdr.Length           =   sizeof(URB_ISOCH_CLOSE_ENDPOINT);\
        (_UrbIsochCloseEndpoint_)->Hdr.Function         =   URB_FUNCTION_ISOCH_CLOSE_ENDPOINT;\
        (_UrbIsochCloseEndpoint_)->Hdr.CompleteProc     =   (_CompleteProc_);\
        (_UrbIsochCloseEndpoint_)->Hdr.CompleteContext  =   (_CompleteContext_);\
        (_UrbIsochCloseEndpoint_)->EndpointHandle       =   (_EndpointHandle_);\
    }

#define USB_BUILD_ISOCH_START_TRANSFER(\
                        _UrbIsochStartTransfer_,\
                        _EndpointHandle_,\
                        _FrameNumber_,\
                        _Flags_\
    )\
    {\
        (_UrbIsochStartTransfer_)->Hdr.Length           =   sizeof(URB_ISOCH_START_TRANSFER);\
        (_UrbIsochStartTransfer_)->Hdr.Function         =   URB_FUNCTION_ISOCH_START_TRANSFER;\
        (_UrbIsochStartTransfer_)->Hdr.CompleteProc     =   NULL;\
        (_UrbIsochStartTransfer_)->Hdr.CompleteContext  =   NULL;\
        (_UrbIsochStartTransfer_)->EndpointHandle       =   (_EndpointHandle_);\
        (_UrbIsochStartTransfer_)->FrameNumber          =   (_FrameNumber_);\
        (_UrbIsochStartTransfer_)->Flags                =   (_Flags_);\
    }

#define USB_BUILD_ISOCH_STOP_TRANSFER(\
                        _UrbIsochStopTransfer_,\
                        _EndpointHandle_\
    )\
    {\
        (_UrbIsochStopTransfer_)->Hdr.Length           =   sizeof(URB_ISOCH_STOP_TRANSFER);\
        (_UrbIsochStopTransfer_)->Hdr.Function         =   URB_FUNCTION_ISOCH_STOP_TRANSFER;\
        (_UrbIsochStopTransfer_)->Hdr.CompleteProc     =   NULL;\
        (_UrbIsochStopTransfer_)->Hdr.CompleteContext  =   NULL;\
        (_UrbIsochStopTransfer_)->EndpointHandle       =   (_EndpointHandle_);\
    }


#define USB_BUILD_ISOCH_ATTACH_BUFFER(\
                        _UrbIsochAttachBuffer_,\
                        _EndpointHandle_,\
                        _InterruptDelay_,\
                        _BufferDescriptor_\
    )\
    {\
        (_UrbIsochAttachBuffer_)->Hdr.Length            =   sizeof(URB_ISOCH_ATTACH_BUFFER);\
        (_UrbIsochAttachBuffer_)->Hdr.Function          =   URB_FUNCTION_ISOCH_ATTACH_BUFFER;\
        (_UrbIsochAttachBuffer_)->Hdr.CompleteProc      =   NULL;\
        (_UrbIsochAttachBuffer_)->Hdr.CompleteContext   =   NULL;\
        (_UrbIsochAttachBuffer_)->EndpointHandle       =   (_EndpointHandle_);\
        (_UrbIsochAttachBuffer_)->InterruptDelay       =   (_InterruptDelay_);\
        (_UrbIsochAttachBuffer_)->BufferDescriptor     =   (_BufferDescriptor_);\
    }

//
// Bulk or Interrupt requests
//

#define USB_BUILD_OPEN_ENDPOINT(\
                        _UrbOpenEndpoint_,\
                        _EndpointAddress_,\
                        _EndpointType_,\
                        _MaxPacketSize_,\
                        _Interval_\
    )\
    {\
        (_UrbOpenEndpoint_)->Hdr.Length         =   sizeof(URB_OPEN_ENDPOINT);\
        (_UrbOpenEndpoint_)->Hdr.Function       =   URB_FUNCTION_OPEN_ENDPOINT;\
        (_UrbOpenEndpoint_)->Hdr.CompleteProc   =   NULL;\
        (_UrbOpenEndpoint_)->EndpointAddress    =   (_EndpointAddress_);\
        (_UrbOpenEndpoint_)->EndpointType       =   (_EndpointType_);\
        (_UrbOpenEndpoint_)->Interval           =   (_Interval_);\
        (_UrbOpenEndpoint_)->MaxPacketSize      =   (_MaxPacketSize_);\
    }

#define USB_BUILD_CLOSE_ENDPOINT(\
                        _UrbCloseEndpoint_,\
                        _EndpointHandle_,\
                        _CompleteProc_,\
                        _CompleteContext_\
    )\
    {\
        (_UrbCloseEndpoint_)->Hdr.Length        =   sizeof(URB_CLOSE_ENDPOINT);\
        (_UrbCloseEndpoint_)->Hdr.Function      =   URB_FUNCTION_CLOSE_ENDPOINT;\
        (_UrbCloseEndpoint_)->Hdr.CompleteProc  =   (_CompleteProc_);\
        (_UrbCloseEndpoint_)->Hdr.CompleteContext = (_CompleteContext_);\
        (_UrbCloseEndpoint_)->EndpointHandle    =   (_EndpointHandle_);\
    }

#define USB_BUILD_OPEN_DEFAULT_ENDPOINT(\
                            _UrbOpenEndpoint_\
    )\
    {\
        (_UrbOpenEndpoint_)->Hdr.Length         =   sizeof(URB_OPEN_ENDPOINT);\
        (_UrbOpenEndpoint_)->Hdr.Function       =   URB_FUNCTION_OPEN_DEFAULT_ENDPOINT;\
        (_UrbOpenEndpoint_)->Hdr.CompleteProc   =   NULL;\
    }

#define USB_BUILD_CLOSE_DEFAULT_ENDPOINT(_UrbCloseEndpoint_, _CompleteProc_, _CompleteContext_)\
    {\
        (_UrbCloseEndpoint_)->Hdr.Length        =   sizeof(URB_CLOSE_ENDPOINT);\
        (_UrbCloseEndpoint_)->Hdr.Function      =   URB_FUNCTION_CLOSE_DEFAULT_ENDPOINT;\
        (_UrbCloseEndpoint_)->Hdr.CompleteProc  =   (_CompleteProc_);\
        (_UrbCloseEndpoint_)->Hdr.CompleteContext = (_CompleteContext_);\
    }

#define USB_BUILD_GET_ENDPOINT_STATE(_UrbGetSetEndpointState_, _EndpointHandle_)\
{\
    (_UrbGetSetEndpointState_)->Hdr.Length          =   sizeof(URB_GET_SET_ENDPOINT_STATE);\
    (_UrbGetSetEndpointState_)->Hdr.Function        =   URB_FUNCTION_GET_ENDPOINT_STATE;\
    (_UrbGetSetEndpointState_)->Hdr.CompleteProc    =   NULL;\
    (_UrbGetSetEndpointState_)->EndpointHandle      =   (_EndpointHandle_);\
}

#define USB_BUILD_SET_ENDPOINT_STATE(_UrbGetSetEndpointState_, _EndpointHandle_, _StateFlags_)\
{\
    (_UrbGetSetEndpointState_)->Hdr.Length          =   sizeof(URB_GET_SET_ENDPOINT_STATE);\
    (_UrbGetSetEndpointState_)->Hdr.Function        =   URB_FUNCTION_SET_ENDPOINT_STATE;\
    (_UrbGetSetEndpointState_)->Hdr.CompleteProc    =   NULL;\
    (_UrbGetSetEndpointState_)->EndpointHandle      =   (_EndpointHandle_);\
    (_UrbGetSetEndpointState_)->EndpointState       =   (_StateFlags_);\
}

#define USB_BUILD_ABORT_ENDPOINT(_UrbAbortEndpoint_, _EndpointHandle_, _CompleteProc_, _CompleteContext_)\
{\
    (_UrbAbortEndpoint_)->Hdr.Length          =   sizeof(URB_ABORT_ENDPOINT);\
    (_UrbAbortEndpoint_)->Hdr.Function        =   URB_FUNCTION_ABORT_ENDPOINT;\
    (_UrbAbortEndpoint_)->Hdr.CompleteProc    =   (_CompleteProc_);\
	(_UrbAbortEndpoint_)->Hdr.CompleteContext =   (_CompleteContext_);\
    (_UrbAbortEndpoint_)->EndpointHandle      =   (_EndpointHandle_);\
}

#define USB_BUILD_RESET_PORT(_UrbResetPort_)\
{\
    (_UrbResetPort_)->Hdr.Length        =   sizeof(URB_RESET_PORT);\
    (_UrbResetPort_)->Hdr.Function      =   URB_FUNCTION_RESET_PORT;\
    (_UrbResetPort_)->Hdr.CompleteProc  =   NULL;\
}

#define USB_BUILD_GET_FRAME(_UrbGetFrame_)\
{\
    (_UrbResetPort_)->Hdr.Length        =   sizeof(URB_GET_FRAME);\
    (_UrbResetPort_)->Hdr.Function      =   URB_FUNCTION_GET_FRAME;\
    (_UrbResetPort_)->Hdr.CompleteProc  =   NULL;\
}

//------------------------------------------------------------------------------------
// Macros to build standard USB Command Request Blocks
//------------------------------------------------------------------------------------
#define USB_BUILD_CLEAR_FEATURE(\
                _UrbControlTransfer_,\
                _Recipient_,\
                _FeatureSelect_,\
                _Index_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = NULL;\
    (_UrbControlTransfer_)->TransferBufferLength        = 0;\
    (_UrbControlTransfer_)->TransferDirection           = 0;\
    (_UrbControlTransfer_)->ShortTransferOK             = 0;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = (_Recipient_);\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_CLEAR_FEATURE;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = (_FeatureSelect_);\
    (_UrbControlTransfer_)->SetupPacket.wIndex          = (_Index_);\
    (_UrbControlTransfer_)->SetupPacket.wLength         = 0;\
}

#define USB_BUILD_SET_FEATURE(\
                _UrbControlTransfer_,\
                _Recipient_,\
                _FeatureSelect_,\
                _Index_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = NULL;\
    (_UrbControlTransfer_)->TransferBufferLength        = 0;\
    (_UrbControlTransfer_)->TransferDirection           = 0;\
    (_UrbControlTransfer_)->ShortTransferOK             = 0;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = (_Recipient_);\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_SET_FEATURE;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = (_FeatureSelect_);\
    (_UrbControlTransfer_)->SetupPacket.wIndex          = (_Index_);\
    (_UrbControlTransfer_)->SetupPacket.wLength         = 0;\
}

#define USB_BUILD_GET_CONFIGURATION(\
                _UrbControlTransfer_,\
                _Configuration_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = (_Configuration_);\
    (_UrbControlTransfer_)->TransferBufferLength        = sizeof(UCHAR);\
    (_UrbControlTransfer_)->TransferDirection           = USB_TRANSFER_DIRECTION_IN;\
    (_UrbControlTransfer_)->ShortTransferOK             = 0;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = USB_DEVICE_TO_HOST;\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_GET_CONFIGURATION;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = 0;\
    (_UrbControlTransfer_)->SetupPacket.wIndex          = 0;\
    (_UrbControlTransfer_)->SetupPacket.wLength         = 1;\
}

#define USB_BUILD_SET_CONFIGURATION(\
                _UrbControlTransfer_,\
                _Configuration_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = 0;\
    (_UrbControlTransfer_)->TransferBufferLength        = 0;\
    (_UrbControlTransfer_)->TransferDirection           = 0;\
    (_UrbControlTransfer_)->ShortTransferOK             = 0;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = USB_HOST_TO_DEVICE;\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_SET_CONFIGURATION;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = (_Configuration_);\
    (_UrbControlTransfer_)->SetupPacket.wIndex          = 0;\
    (_UrbControlTransfer_)->SetupPacket.wLength         = 0;\
}


#define USB_BUILD_GET_DESCRIPTOR(\
                _UrbControlTransfer_,\
                _DescriptorType_,\
                _DescriptorIndex_,\
                _LanguageID_,\
                _DescriptorBuffer_,\
                _DescriptorBufferLength_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = (_DescriptorBuffer_);\
    (_UrbControlTransfer_)->TransferBufferLength        = (_DescriptorBufferLength_);\
    (_UrbControlTransfer_)->TransferDirection           = USB_TRANSFER_DIRECTION_IN;\
    (_UrbControlTransfer_)->ShortTransferOK             = TRUE;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = USB_DEVICE_TO_HOST;\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_GET_DESCRIPTOR;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = ((_DescriptorType_)<<8)|(_DescriptorIndex_);\
    (_UrbControlTransfer_)->SetupPacket.wIndex          = (_LanguageID_);\
    (_UrbControlTransfer_)->SetupPacket.wLength         = (_DescriptorBufferLength_);\
}

#define USB_BUILD_SET_DESCRIPTOR(\
                _UrbControlTransfer_,\
                _DescriptorType_,\
                _DescriptorIndex_,\
                _LanguageID_,\
                _DescriptorBuffer_,\
                _DescriptorBufferLength_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = (_DescriptorBuffer_);\
    (_UrbControlTransfer_)->TransferBufferLength        = (_DescriptorBufferLength_);\
    (_UrbControlTransfer_)->TransferDirection           = USB_TRANSFER_DIRECTION_OUT;\
    (_UrbControlTransfer_)->ShortTransferOK             = FALSE;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = USB_HOST_TO_DEVICE;\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_SET_DESCRIPTOR;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = ((_DescriptorType_)<<8)|(_DescriptorIndex_);\
    (_UrbControlTransfer_)->SetupPacket.wIndex          = (_LanguageID_);\
    (_UrbControlTransfer_)->SetupPacket.wLength         = (_DescriptorBufferLength_);\
}


#define USB_BUILD_GET_INTERFACE(\
                _UrbControlTransfer_,\
                _Interface_,\
                _AlternateSetting_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = (_AlternateSetting_);\
    (_UrbControlTransfer_)->TransferBufferLength        = sizeof(UCHAR);\
    (_UrbControlTransfer_)->TransferDirection           = USB_TRANSFER_DIRECTION_IN;\
    (_UrbControlTransfer_)->ShortTransferOK             = 0;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = USB_DEVICE_TO_HOST;\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_GET_INTERFACE;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = 0\
    (_UrbControlTransfer_)->SetupPacket.wIndex          = (_Interface_);\
    (_UrbControlTransfer_)->SetupPacket.wLength         = 1;\
}

#define USB_BUILD_SET_INTERFACE(\
                _UrbControlTransfer_,\
                _Interface_,\
                _AlternateSetting_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = 0;\
    (_UrbControlTransfer_)->TransferBufferLength        = 0;\
    (_UrbControlTransfer_)->TransferDirection           = 0;\
    (_UrbControlTransfer_)->ShortTransferOK             = 0;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = USB_HOST_TO_DEVICE;\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_SET_INTERFACE;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = (_AlternateSetting_);\
    (_UrbControlTransfer_)->SetupPacket.wIndex          = (_Interface_);\
    (_UrbControlTransfer_)->SetupPacket.wLength         = 0;\
}


#define USB_BUILD_GET_STATUS(\
                _UrbControlTransfer_,\
                _Recipient_,\
                _Index_,\
                _Status_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = (_Status_);\
    (_UrbControlTransfer_)->TransferBufferLength        = size(WORD);\
    (_UrbControlTransfer_)->TransferDirection           = USB_TRANSFER_DIRECTION_IN;\
    (_UrbControlTransfer_)->ShortTransferOK             = 0;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = (USB_DEVICE_TO_HOST|(_Recipient_));\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_GET_STATUS;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = 0;\
    (_UrbControlTransfer_)->SetupPacket.wIndex          =(_Index_);\
    (_UrbControlTransfer_)->SetupPacket.wLength         = 2;\
}

#define USB_BUILD_SYNC_FRAME(\
                _UrbControlTransfer_,\
                _Endpoint_,\
                _Frame_,\
                _CompleteProc_,\
                _CompleteContext_\
                )\
{\
    (_UrbControlTransfer_)->Hdr.Length                  = sizeof(URB_CONTROL_TRANSFER);\
    (_UrbControlTransfer_)->Hdr.Function                = URB_FUNCTION_CONTROL_TRANSFER;\
    (_UrbControlTransfer_)->Hdr.CompleteProc            = (_CompleteProc_);\
    (_UrbControlTransfer_)->Hdr.CompleteContext         = (_CompleteContext_);\
    (_UrbControlTransfer_)->EndpointHandle              = NULL;\
    (_UrbControlTransfer_)->TransferBuffer              = (_Frame_);\
    (_UrbControlTransfer_)->TransferBufferLength        = size(WORD);\
    (_UrbControlTransfer_)->TransferDirection           = USB_TRANSFER_DIRECTION_IN;\
    (_UrbControlTransfer_)->ShortTransferOK             = 0;\
    (_UrbControlTransfer_)->InterruptDelay              = USBD_DELAY_INTERRUPT_0_MS;\
    (_UrbControlTransfer_)->SetupPacket.bmRequestType   = (USB_DEVICE_TO_HOST|USB_COMMAND_TO_ENDPOINT);\
    (_UrbControlTransfer_)->SetupPacket.bRequest        = USB_REQUEST_SYNC_FRAME;\
    (_UrbControlTransfer_)->SetupPacket.wValue          = 0;\
    (_UrbControlTransfer_)->SetupPacket.wIndex          =(_Endpoint_);\
    (_UrbControlTransfer_)->SetupPacket.wLength         = 2;\
}

//-------------------------------------------------------------------------------------------------------------
// Get the USB status code
//-------------------------------------------------------------------------------------------------------------
#define URB_STATUS(urb) ((urb)->Header.Status)
//------------------------------------------------------------------------------------
// bmRequestType values for commands over control pipes, belongs in usb100.h
//------------------------------------------------------------------------------------

//  Data Direction
#define USB_HOST_TO_DEVICE              0x00    
#define USB_DEVICE_TO_HOST              0x80

//  Type
#define USB_STANDARD_COMMAND            0x00
#define USB_CLASS_COMMAND               0x20
#define USB_VENDOR_COMMAND              0x40

//  Recipient
#define USB_COMMAND_TO_DEVICE           0x00
#define USB_COMMAND_TO_INTERFACE        0x01
#define USB_COMMAND_TO_ENDPOINT         0x02
#define USB_COMMAND_TO_OTHER            0x03

//------------------------------------------------------------------------------------
// The enumerator only provides the first eight bytes of the device descriptor
//------------------------------------------------------------------------------------
typedef struct _USB_DEVICE_DESCRIPTOR8 {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
} USB_DEVICE_DESCRIPTOR8, *PUSB_DEVICE_DESCRIPTOR8;
//---------------------------------------------------------------------------------------------------------------
// USB_HOST_CONTROLLER - Host Controller Configuration Selection
//---------------------------------------------------------------------------------------------------------------
#define USB_SINGLE_HOST_CONTROLLER 1
#define USB_DUAL_HOST_CONTROLLER_XDK 2
//  Select which configuragtion to use.

#ifndef USB_HOST_CONTROLLER_CONFIGURATION
#ifdef SILVER
#define USB_HOST_CONTROLLER_CONFIGURATION USB_DUAL_HOST_CONTROLLER_XDK
#else  //!SILVER
#define USB_HOST_CONTROLLER_CONFIGURATION USB_SINGLE_HOST_CONTROLLER
#endif ////!SILVER
#endif  USB_HOST_CONTROLLER_CONFIGURATION


#

//--------------------------------------------------------------------------------
//  Resource structure shared between USBD and OHCD
//--------------------------------------------------------------------------------
#define HCD_INTERRUPT_TD_QUOTA 3
typedef struct _HCD_RESOURCE_REQUIREMENTS
{
    ULONG EndpointCount;
    ULONG IsochEndpointCount;
    ULONG TDCount;
    UCHAR ControlTDQuota;
    UCHAR BulkTDQuota;
    UCHAR IsochMaxBuffers;
} HCD_RESOURCE_REQUIREMENTS, *PHCD_RESOURCE_REQUIREMENTS;


//--------------------------------------------------------------------------------
//  Interface between the host controller driver and usbd
//--------------------------------------------------------------------------------

//
// HCD specific URBs
//
#define USBD_EP_FLAG_LOWSPEED                0x0001
#define USBD_EP_FLAG_NEVERHALT               0x0002

//
// Common transfer request definition, all transfer
// requests passed to the HCD will be mapped to this
// format.  The HCD will can use this structure to
// reference fields that are common to all transfers
// as well as fields specific to isochronous and
// control transfers.

//
// bandwidth related definitions
//

// overhead in bytes/ms

#define USB_ISO_OVERHEAD_BYTES              9
#define USB_INTERRUPT_OVERHEAD_BYTES        13

  
//-------------------------------------------------------------
// Utility procedures supported by USB System Driver
//-------------------------------------------------------------
VOID
USBD_CompleteRequest(
    IN  PURB Urb
    );

USHORT
USBD_CalculateUsbBandwidth(
    IN USHORT   MaxPacketSize,
    IN UCHAR    EndpointType,
    IN BOOLEAN  LowSpeed
    );

VOID
USBD_DeviceConnected(
    IN  PVOID    HcdExtension,
    IN  UCHAR    PortNumber
    );

VOID
USBD_DeviceDisconnected(
    IN  PVOID HcdExtension,
    IN  UCHAR PortNumber
    );

VOID
USBD_NewHostController(
    IN PPCI_DEVICE_DESCRIPTOR PciDevice,
	IN ULONG HcdDeviceExtensionSize
    );

//-------------------------------------------------------------
// Procedures which must be exported by Host Controller Driver
//-------------------------------------------------------------
VOID
HCD_DriverEntry(
    PHCD_RESOURCE_REQUIREMENTS ResourceRequirements
    );

VOID
HCD_EnumHardware();

NTSTATUS
HCD_NewHostController(
    IN PVOID  HcdExtension,
    IN UCHAR  HostControllerNumber,
    IN PPCI_DEVICE_DESCRIPTOR PciDevice
    );

USBD_STATUS
HCD_SubmitRequest(
    IN PVOID HcdExtension,
    IN PURB Urb
    );

USBD_STATUS
HCD_CancelRequest(
    IN PVOID HcdExtension,
    IN PURB  Urb
    );

typedef
VOID
(*PFNHCD_RESET_COMPLETE)(
    IN USBD_STATUS  Status,
    IN PVOID        Context
    );

VOID
HCD_ResetRootHubPort(
    IN PVOID                    HcdExtension,
    IN ULONG                    PortNumber,
    IN PFNHCD_RESET_COMPLETE    ResetCompleteProc,
    IN PVOID                    CompleteContext
    );

VOID
HCD_DisableRootHubPort(
    IN PVOID                    HcdExtension,
    IN ULONG                    PortNumber
    );


#endif //__USB_X__


