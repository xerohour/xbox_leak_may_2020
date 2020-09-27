/*++

Copyright (c) 2000 Microsoft Corporation

    stubs.cpp

Abstract:
    
    APIs for interfacing with the modem.
    
Revision History:

    06-21-00    vadimg      created

--*/

#include "precomp.h"

#define STUB_BEGIN() \
    BOOL fRet = FALSE;

#define STUB_END() \
    return fRet;
    
#define STUB_ENTER() \
    STUB_BEGIN(); \
    MODEM_ENTER_CRIT();

#define STUB_LEAVE() \
    MODEM_LEAVE_CRIT(); \
    STUB_END();

/***************************************************************************\
* Wait
*
* Helper function to wait on an event while out of the critical section.
\***************************************************************************/

VOID Wait(PKEVENT pevent)
{
    gfUrbInUse = TRUE;

    MODEM_LEAVE_CRIT();

    KeWaitForSingleObject(pevent, Suspended, KernelMode, FALSE, NULL);

    MODEM_ENTER_CRIT();

    gfUrbInUse = FALSE;
}


/***************************************************************************\
* ModemIsConnected
*
\***************************************************************************/

BOOL ModemIsConnected(VOID)
{
    STUB_ENTER();

    fRet = Modem_IsConnected();

    STUB_LEAVE();
}

/***************************************************************************\
* ModemOpen
*
\***************************************************************************/

BOOL ModemOpen(HANDLE hEvent)
{
    NTSTATUS status;
    PKEVENT pevent;

    if (hEvent != NULL) {
        status = ObReferenceObjectByHandle(hEvent, *ExEventObjectType,
            (PVOID*)&pevent);
    } else {
        pevent = NULL;
        status = STATUS_SUCCESS;
    }

    STUB_ENTER();

    if (Modem_IsConnected() && (gRead.pevent == NULL) && NT_SUCCESS(status)) {
        if (Modem_Create()) {
            gRead.pevent = pevent;
            fRet = TRUE;
        }
    }

    MODEM_LEAVE_CRIT();

    if (!fRet && pevent != NULL) {
        ObDereferenceObject(pevent);
    }

    STUB_END();
}

/***************************************************************************\
* ModemClose
*
\***************************************************************************/

BOOL ModemClose(VOID)
{
    STUB_ENTER();

    if (Modem_IsConnected()) {
        fRet = Modem_Close();
    }

    STUB_LEAVE();
}

/***************************************************************************\
* ModemWrite
*
\***************************************************************************/

BOOL ModemWrite(PUCHAR buffer, ULONG *pcb)
{
    STUB_ENTER();

    if (Modem_IsConnected()) {
        if (*pcb > MODEM_BUFFER_SIZE) {
            *pcb = MODEM_BUFFER_SIZE;
        }
        fRet = WriteData(buffer, pcb);
    }

    STUB_LEAVE();
}

/***************************************************************************\
* ModemRead
*
\***************************************************************************/

BOOL ModemRead(PUCHAR buffer, ULONG *pcb)
{
    STUB_ENTER();

    if (Modem_IsConnected()) {
        if (*pcb > MODEM_BUFFER_SIZE) {
            *pcb = MODEM_BUFFER_SIZE;
        }
        fRet = ReadData(buffer, pcb);
    }

    STUB_LEAVE();
}

/***************************************************************************\
* ModemPurge
*
\***************************************************************************/

BOOL ModemPurge(DWORD dwFlags)
{
    STUB_ENTER();

    fRet = Modem_IsConnected();

    STUB_LEAVE();
}

/***************************************************************************\
* ModemGetLineCoding
*
\***************************************************************************/

BOOL ModemGetLineCoding(PULONG pBaudRate, PUCHAR pStopBits, PUCHAR pParity,
        PUCHAR pDataBits)
{
    STUB_ENTER();

    fRet = GetLineCoding(pBaudRate, pStopBits, pParity, pDataBits);

    STUB_LEAVE();
}

/***************************************************************************\
* ModemSetLineCoding
*
\***************************************************************************/

BOOL ModemSetLineCoding(ULONG BaudRate, UCHAR StopBits, UCHAR Parity,
        UCHAR DataBits)
{
    STUB_ENTER();

    fRet = SetLineCoding(BaudRate, StopBits, Parity, DataBits);

    STUB_LEAVE();
}

/***************************************************************************\
* ModemGetDtrRts
*
\***************************************************************************/

BOOL ModemGetDtrRts(USHORT *pwState)
{
    STUB_ENTER();

    fRet = GetDtrRts(pwState);

    STUB_LEAVE();
}

/***************************************************************************\
* ModemSetDtrRts
*
\***************************************************************************/

BOOL ModemSetDtrRts(USHORT wState)
{
    STUB_ENTER();

    fRet = SetDtrRts(wState);

    STUB_LEAVE();
}

