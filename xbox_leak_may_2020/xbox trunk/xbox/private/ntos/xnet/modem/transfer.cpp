/*++

Copyright (c) 2000 Microsoft Corporation

    transfer.cpp

Abstract:
    
    Handle transfer of data to and from the modem.
    
Revision History:

    06-21-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* CompleteRead
*
\***************************************************************************/

VOID CompleteRead(PURB purb, PVOID)
{
    ASSERT_DISPATCH_LEVEL();

    gRead.fReadPending = FALSE;
    gRead.iCurrent = 0;

    if (USBD_SUCCESS(purb->Header.Status)) {
        gRead.cb = purb->BulkOrInterruptTransfer.TransferBufferLength;

        if (gRead.pevent != NULL) {
            KeSetEvent(gRead.pevent, IO_NO_INCREMENT, FALSE);
        }
    } else {
        gRead.cb = 0;
    }
}

/***************************************************************************\
* ReadWorker
*
\***************************************************************************/

VOID ReadWorker(VOID)
{
    PURB purb;

    ASSERT_DISPATCH_LEVEL();

    if (gRead.fReadPending) {
        return;
    }

    purb = &gUrbRead;

    USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
            (PURB_BULK_OR_INTERRUPT_TRANSFER)purb,
            gDevice.pBulkInEndpointHandle,
            gpMdlRead,
            MODEM_BUFFER_SIZE,
            USB_TRANSFER_DIRECTION_IN,
            (PURB_COMPLETE_PROC)CompleteRead,
            NULL,
            TRUE);

    if (USBD_SUCCESS(gDevice.pDevice->SubmitRequest(purb))) {
        gRead.fReadPending = TRUE;
    }
}

/***************************************************************************\
* ReadData
*
\***************************************************************************/

BOOL ReadData(PUCHAR buffer, ULONG *pcb)
{
    ULONG cbCopy;

    ASSERT_DISPATCH_LEVEL();

    if (gRead.cb != 0) {
        cbCopy = min(gRead.cb, *pcb);

        RtlCopyMemory(buffer, &gRead.buffer[gRead.iCurrent], cbCopy);

        gRead.iCurrent += cbCopy;
        gRead.cb -= cbCopy;
        *pcb = cbCopy;
    } else {
        *pcb = 0;
    }

    if (gRead.cb == 0) {
        ReadWorker();
    }

    return TRUE;
}

/***************************************************************************\
* CompleteWrite
*
\***************************************************************************/

VOID CompleteWrite(PURB purb, PKEVENT pevent)
{
    ASSERT_DISPATCH_LEVEL();

    KeSetEvent(pevent, IO_NO_INCREMENT, FALSE);
}

/***************************************************************************\
* WriteData
*
\***************************************************************************/

BOOL WriteData(PUCHAR buffer, ULONG *pcb)
{
    KEVENT event;    
    PURB purb;
    BOOL fRet;

    ASSERT_DISPATCH_LEVEL();

    if (gfUrbInUse) {
        return FALSE;
    }
    purb = &gUrb;

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    MmInitializeMdl(gpMdl, buffer, *pcb);

    USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(
            (PURB_BULK_OR_INTERRUPT_TRANSFER)purb,
            gDevice.pBulkOutEndpointHandle,
            gpMdl,
            *pcb,
            USB_TRANSFER_DIRECTION_OUT,
            (PURB_COMPLETE_PROC)CompleteWrite,
            (PVOID)&event,
            TRUE);

    gDevice.pDevice->SubmitRequest(purb);

    Wait(&event);

    if (USBD_SUCCESS(purb->Header.Status)) {
        *pcb = purb->BulkOrInterruptTransfer.TransferBufferLength;
        fRet = TRUE;
    } else {
        *pcb = 0;
        fRet = FALSE;
    }

    return fRet;
}

