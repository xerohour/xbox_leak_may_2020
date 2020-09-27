/*++

Copyright (c) 2000 Microsoft Corporation

    ioctl.cpp

Abstract:
    
    Serial IO control for the modem.
    
Revision History:

    06-21-00    vadimg      created

--*/

#include "precomp.h"

#define CR_READ   0x00000001
#define CR_WRITE  0x00000002

/***************************************************************************\
* CompleteClassRequest
*
\***************************************************************************/

VOID CompleteClassRequest(PURB purb, PKEVENT pevent)
{
    ASSERT_DISPATCH_LEVEL();

    KeSetEvent(pevent, IO_NO_INCREMENT, FALSE);
}

/***************************************************************************\
* ClassRequest
*
\***************************************************************************/

BOOL ClassRequest(UCHAR uRequest, USHORT wValue, PVOID buffer,
        USHORT cb, ULONG flags)
{
    KEVENT event;
    PURB purb;
    UCHAR bmRequestType;
    UCHAR bTransferDirection;
    PMDL pMdl;

    ASSERT_DISPATCH_LEVEL();

    if (gfUrbInUse) {
        return FALSE;
    }
    purb = &gUrb;

    if (buffer != NULL) {
        MmInitializeMdl(gpMdl, buffer, cb);
        pMdl = gpMdl;
    } else {
        pMdl = NULL;
    }

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    bmRequestType = USB_CLASS_COMMAND | USB_COMMAND_TO_INTERFACE;
    if (flags & CR_READ) {
        bmRequestType |= USB_DEVICE_TO_HOST;
        bTransferDirection = USB_TRANSFER_DIRECTION_IN;
    } else {
        bmRequestType |= USB_HOST_TO_DEVICE;
        bTransferDirection = USB_TRANSFER_DIRECTION_OUT;
    }
    USB_BUILD_CONTROL_TRANSFER(
            (PURB_CONTROL_TRANSFER)purb,
            NULL,
            pMdl,
            cb,
            bTransferDirection,
            (PURB_COMPLETE_PROC)CompleteClassRequest,
            (PVOID)&event,
            TRUE,
            bmRequestType,
            uRequest,
            wValue,
            gDevice.bCommInterface,
            cb);

    gDevice.pDevice->SubmitRequest(purb);

    Wait(&event);

    if (USBD_SUCCESS(purb->Header.Status)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/***************************************************************************\
* GetLineCoding
*
\***************************************************************************/

BOOL GetLineCoding(PULONG pBaudRate, PUCHAR pStopBits, PUCHAR pParity,
        PUCHAR pDataBits)
{
    ASSERT_DISPATCH_LEVEL();

    if (ClassRequest(USB_COMM_GET_LINE_CODING, 0, &gMem.LineCoding,
            sizeof(USB_COMM_LINE_CODING), CR_READ)) {

        *pBaudRate = gMem.LineCoding.DTERate;
        *pStopBits = gMem.LineCoding.CharFormat;
        *pParity = gMem.LineCoding.ParityType;
        *pDataBits = gMem.LineCoding.DataBits;

        return TRUE;
    } else {
        return FALSE;
    }
}

/***************************************************************************\
* SetLineCoding
*
\***************************************************************************/

BOOL SetLineCoding(ULONG BaudRate, UCHAR StopBits, UCHAR Parity,
        UCHAR DataBits)
{
    ASSERT_DISPATCH_LEVEL();

    gMem.LineCoding.DTERate = BaudRate;
    gMem.LineCoding.CharFormat = StopBits;
    gMem.LineCoding.ParityType = Parity;
    gMem.LineCoding.DataBits = DataBits;

    return ClassRequest(USB_COMM_SET_LINE_CODING, 0, &gMem.LineCoding,
            sizeof(USB_COMM_LINE_CODING), CR_WRITE);
}

/***************************************************************************\
* GetDtrRts
*
\***************************************************************************/

BOOL GetDtrRts(USHORT *pwState)
{
    ASSERT_DISPATCH_LEVEL();

    *pwState = gDevice.wDTR_RTS;
    return TRUE;
}

/***************************************************************************\
* SetDtrRts
*
\***************************************************************************/

BOOL SetDtrRts(USHORT wState)
{
    ASSERT_DISPATCH_LEVEL();

    gDevice.wDTR_RTS = wState;

    return ClassRequest(USB_COMM_SET_CONTROL_LINE_STATE, wState, NULL, 0,
            CR_WRITE);
}

