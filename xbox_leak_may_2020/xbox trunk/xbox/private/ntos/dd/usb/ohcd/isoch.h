/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    isoch.h

Abstract:

    This file contains Macros and Declaration need
    for Isochronous support.  Since USBD can be compiled
    with or without isoch support there are two
    versions of every macro.

Environment:

    Designed for XBOX.

Notes:

Revision History:

    06-29-00 created by Mitchell Dernis (mitchd)

--*/
#ifndef __ISOCH_H__
#define __ISOCH_H__


#ifdef  OHCD_ISOCHRONOUS_SUPPORTED

//
//  Forward declaration of pointer types.
//
typedef struct _OHCD_ISOCH_ENDPOINT             *POHCD_ISOCH_ENDPOINT;
typedef struct _OHCD_ISOCH_TRANSFER_DESCRIPTOR  *POHCD_ISOCH_TRANSFER_DESCRIPTOR;
typedef struct _OHCD_ISOCH_TRANSFER_DESCRIPTOR
{
    OHCI_ISOCHRONOUS_TRANSFER_DESCRIPTOR  IsochTransferDescriptor;
    POHCD_ISOCH_ENDPOINT                  Endpoint;
    PFNUSBD_ISOCH_TRANSFER_COMPLETE       TransferComplete;
    PVOID                                 Context;
    UCHAR                                 TdIndex;
    UCHAR                                 TdIndexPrev;
    USHORT                                Pad;
    USHORT                                Offsets[8];
} OHCD_ISOCH_TRANSFER_DESCRIPTOR;

typedef struct _OHCD_ISOCH_ENDPOINT
{
    //
    //  OHCI mandated portion of endpoint.  This structure
    //  must start on a paragraph(16-byte) boundary.
    //
    OHCI_ENDPOINT_DESCRIPTOR    HcEndpointDescriptor;
    //
    //  Fields for managing the schedule (especially for interrupt)
    //
    UCHAR                       Flags;                  //Endpoint flags
    UCHAR                       EndpointType;           //Type of endpoint
    UCHAR                       ScheduleIndex;          //Index in schedule - see definitions above
    UCHAR                       PollingInterval;        //Maximum polling interval (in ms)
    
    ULONG                       PhysicalAddress;        //Physical address of this structure.
    POHCD_ENDPOINT              Next;                   //Next endpoint in schedule

    ULONG                       PauseFrame;             //Used to verify that we have waited at least

    UCHAR                       PendingPauseCount;      //Number of reasons that we are paused.
    UCHAR                       Padding;                //Padding to keep up DWORD alignement.
    USHORT                      Bandwidth;              //Bandwidth required by this endpoint (interrupt and isoch only)
    
    //
    //  Above here must be identical to OHCD_ENDPOINT so that the schedule routines work,
    //  and part of the close routines.
    //
    UCHAR                       MaxAttachedBuffers;
    UCHAR                       AttachedBuffers;

    UCHAR                       NextFreeTD;
    UCHAR                       Alignment;

    ULONG                           NextUnusedFrame;
    POHCD_ISOCH_TRANSFER_DESCRIPTOR TransferDescriptors;
} OHCD_ISOCH_ENDPOINT;

#define OHCD_ISOCH_EDFL_CIRCULAR_DMA 0x01
#define OHCD_ISOCH_EDFL_TRANSFERING  0x02
#define OHCD_ISOCH_EDFL_STOPPING     0x04
#define OHCD_ISOCH_EDFL_PAUSING      OHCD_ENDPOINT_FLAG_PAUSING

//
//  Isochronous function definitions
//
USBD_STATUS
FASTCALL
OHCD_fIsochOpenEndpoint(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    );

USBD_STATUS
FASTCALL
OHCD_fIsochCloseEndpoint(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    );

USBD_STATUS
FASTCALL
OHCD_fIsochAttachBuffer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    );

USBD_STATUS
FASTCALL
OHCD_fIsochStartTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    );

USBD_STATUS
FASTCALL
OHCD_fIsochStopTransfer(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN PURB                     Urb
    );

VOID
FASTCALL
OHCD_fIsochProcessTD(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN POHCD_TRANSFER_DESCRIPTOR TransferDescriptor
    );

VOID
FASTCALL
OHCD_fIsochCompleteCloseEndpoint
    (
    IN POHCD_DEVICE_EXTENSION       DeviceExtension,
    IN PURB_ISOCH_CLOSE_ENDPOINT    Urb
    );

//
//  Macros used outside of isoch.c that require stub definitions.
//
#define OHCD_ISOCH_OPEN_ENDPOINT(DeviceExtension, Urb)\
           OHCD_fIsochOpenEndpoint(DeviceExtension, Urb)

#define OHCD_ISOCH_CLOSE_ENDPOINT(DeviceExtension, Urb)\
           OHCD_fIsochCloseEndpoint(DeviceExtension, Urb)

#define OHCD_ISOCH_ATTACH_BUFFERS(DeviceExtension, Urb)\
           OHCD_fIsochAttachBuffer(DeviceExtension, Urb)

#define OHCD_ISOCH_START_TRANSFER(DeviceExtension, Urb)\
           OHCD_fIsochStartTransfer(DeviceExtension, Urb)

#define OHCD_ISOCH_STOP_TRANSFER(DeviceExtension, Urb)\
           OHCD_fIsochStopTransfer(DeviceExtension, Urb)

//
//  The format bit should be clear for non-isoch transfers.
//  In non-isoch builds it doesn't really matter.  However,
//  the non-isoch checked build does some testing to make sure
//  that it is always cleared when it should be.
//
#define OHCD_CLEAR_TD_ISOCH_FORMAT_BIT(TD)\
          TD->HcTransferDescriptor.Format = 0;
#define OHCD_IS_ISOCH_TD(TD)\
           (TD->HcTransferDescriptor.Format)
#define OHCD_PROCESS_ISOCHRONOUS_TD(DeviceExtension, TD)\
           OHCD_fIsochProcessTD(DeviceExtension, TD)
#define OHCD_IS_ISOCH_CLOSE(urb)\
           (URB_FUNCTION_ISOCH_CLOSE_ENDPOINT == urb->Hdr.Function)
#define OHCD_ISOCH_COMPLETE_CLOSE_ENDPOINT(DeviceExtension, Urb)\
            OHCD_fIsochCompleteCloseEndpoint(DeviceExtension, Urb)
#define HC_CONTROL_ISOCH_ENABLE_STATE   0x00000008

#define OHCD_ISOCH_ENDPOINT_SIZE(MaxAttachedBuffers)\
    (sizeof(OHCD_ISOCH_ENDPOINT) +\
     MaxAttachedBuffers*sizeof(OHCD_ISOCH_TRANSFER_DESCRIPTOR))
#define OHCD_ISOCH_POOL ULONG_PTR IsochFreeEndpoints; ULONG IsochMaxBuffers;

#else

/*
**  DEFINITIONS WHEN COMPILED WITHOUT ISOCHRONOUS
**  AND CHECKED OR FREE DOESN'T MATTER
*/
#define HC_CONTROL_ISOCH_ENABLE_STATE   0
#define OHCD_IS_ISOCH_CLOSE(urb) (FALSE)
#define OHCD_ISOCH_COMPLETE_CLOSE_ENDPOINT(DeviceExtension, Urb)
#define OHCD_ISOCH_ENDPOINT_SIZE(MaxAttachedBuffers) 0
#define OHCD_ISOCH_FREE_ENDPOINTS

#if DBG
/*
**  DEFINITIONS WHEN COMPILED WITHOUT
**  ISOCHRONOUS SUPPORT AND CHECKED BUILD
**  In the checked build, some macros spew debug information
**  and/or break into the debugger if they are hit.
*/  

//
//  Stub for Isochronous Support Error.
//
__inline USBD_STATUS OHCD_ErrorNoIsochSupport()
{
    DbgPrint("ERROR: THIS VERSION OF USBD.SYS WAS BUILT WITHOUT SUPPORT FOR USING ISOCHRONOUS DEVICES.\n");
    DbgPrint("ERROR: THIS ERROR SHOULD NOT BE HIT MERELY BECAUSE AN ISOCHRONOUS DEVICE IS PLUGGED IN,\n");
    DbgPrint("ERROR: BUT ONLY IF A CLASS DRIVER FOR THE DEVICE IS PRESENT AND IF AN OPEN ON THE DEVICE\n");
    DbgPrint("ERROR: IS ATTEMPTED.\n");
    DbgBreakPoint();
    return USBD_STATUS_ISOCH_NOT_SUPPORTED;
}

//
//  Macros definitons
//  

#define OHCD_ISOCH_OPEN_ENDPOINT(DeviceExtension, Urb)\
                                OHCD_ErrorNoIsochSupport()

#define OHCD_ISOCH_CLOSE_ENDPOINT(DeviceExtension, Urb)\
                                OHCD_ErrorNoIsochSupport()

#define OHCD_ISOCH_ATTACH_BUFFERS(DeviceExtension, Urb)\
                                OHCD_ErrorNoIsochSupport()

#define OHCD_ISOCH_START_TRANSFER(DeviceExtension, Urb)\
                                OHCD_ErrorNoIsochSupport()

#define OHCD_ISOCH_STOP_TRANSFER(DeviceExtension, Urb)\
                                OHCD_ErrorNoIsochSupport()

//
//  Even though isochronous support is off, we keep track of the Format bit like in
//  an isoch build.  If OHCD_PROCESS_ISOCHRONOUS_TD is ever hit as a result,
//  it errors out and then calls OHCD_ProcessDoneTD, like it should anyway.
//  This should help find problems with how the format bit is being used, and may
//  reveal problems that would be hard to find or solve in the builds with isoch support.
//
#define OHCD_CLEAR_TD_ISOCH_FORMAT_BIT(TD) TD->HcTransferDescriptor.Format = 0;
#define OHCD_IS_ISOCH_TD(TD) (TD->HcTransferDescriptor.Format)
#define OHCD_PROCESS_ISOCHRONOUS_TD(deviceExtension, tempTD)\
{\
    DbgPrint("ERROR: Found TD with Format bit set in a build that does not support Isochronous transfers.\n");\
    DbgPrint("ERROR: This indicates that the format bit is not being cleared properly for non-isoch transfers.\n");\
    DbgPrint("ERROR: Hitting this suggests that builds which do support isochronous transfers may have undetected errors.\n");\
    DbgPrint("ERROR: If you hit 'g' in the debugger, it will handle this properly as a non-isoch TD.\n");\
    DbgBreakPoint();\
    OHCD_fProcessDoneTD(deviceExtension, tempTD);\
}

/*
**  DEFINITIONS WHEN COMPILED WITHOUT
**  ISOCHRONOUS SUPPORT AND FREE BUILD
**
**  In the free build, we do not spew debug information.  Macros that shouldn't
**  be hit simply return an ERROR.
*/
#else //DBG!=1

#define OHCD_ISOCH_OPEN_ENDPOINT(DeviceExtension, Urb) USBD_STATUS_ISOCH_NOT_SUPPORTED

#define OHCD_ISOCH_CLOSE_ENDPOINT(DeviceExtension, Urb) USBD_STATUS_ISOCH_NOT_SUPPORTED

#define OHCD_ISOCH_ATTACH_BUFFERS(DeviceExtension, Urb) USBD_STATUS_ISOCH_NOT_SUPPORTED

#define OHCD_ISOCH_START_TRANSFER(DeviceExtension, Urb) USBD_STATUS_ISOCH_NOT_SUPPORTED

#define OHCD_ISOCH_STOP_TRANSFER(DeviceExtension, Urb) USBD_STATUS_ISOCH_NOT_SUPPORTED


#define OHCD_CLEAR_TD_ISOCH_FORMAT_BIT(TD) //NOP, not necessary to clear the bit, if isoch not supported.
#define OHCD_IS_ISOCH_TD(TD) (FALSE)       //We don't care.
#define OHCD_PROCESS_ISOCHRONOUS_TD(deviceExtension, tempTD) //Should never be hit

#endif //end of else DBG!=1
#endif //end of else clause for ifdef OHCD_ISOCHRONOUS_SUPPORTED
#endif //__ISOCH_H__
