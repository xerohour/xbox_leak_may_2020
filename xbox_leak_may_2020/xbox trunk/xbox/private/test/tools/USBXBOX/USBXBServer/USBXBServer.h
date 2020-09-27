/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    USBXBServer.h

Abstract:

    main header file for usb xbox server

Author:

    Jason Gould (a-jasgou) June 2000

--*/
#ifndef __slixbox_h_
#define __slixbox_h_

#define NODSOUND //prevent some weird build break
#include <xtl.h>

#include <winsockx.h>
#include <stdio.h>
#include <time.h>			
#include "USBXBsock.h"

#include "..\inclib\USBXBtransfer.h"



class CListNode
{
public:
	CListNode *		pNext;
	CListNode *		pPrev;
	CXBoxTopology   m_Topology;
};


//#include "usbd.h"

#ifdef __cplusplus
extern "C" {
#endif
	ULONG DebugPrint(PCHAR Format, ...);	//prints stuff to the debugger computer
	int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hprev, LPSTR lpcmd, int nShow);
#ifdef __cplusplus
}
#endif

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





#endif
