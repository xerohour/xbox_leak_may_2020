/*++

Copyright (c) 2000 Microsoft Corporation

    xmodem.h

Abstract:

    Microsoft XBox specific modem APIs.

--*/

#ifndef __XMODEM_H__
#define __XMODEM_H__

#define MODEM_BUFFER_SIZE 64

BOOL ModemIsConnected(VOID);
BOOL ModemOpen(HANDLE hEvent);
BOOL ModemClose(VOID);

BOOL ModemWrite(PUCHAR buffer, ULONG *pcb);
BOOL ModemRead(PUCHAR buffer, ULONG *pcb);
BOOL ModemPurge(DWORD dwFlags);

BOOL ModemGetLineCoding(PULONG pBaudRate, PUCHAR pStopBits, PUCHAR pParity,
        PUCHAR pDataBits);
BOOL ModemSetLineCoding(ULONG BaudRate, UCHAR StopBits, UCHAR Parity,
        UCHAR DataBits);
BOOL ModemGetDtrRts(USHORT *pwState);
BOOL ModemSetDtrRts(USHORT wState);

#endif // __XMODEM_H__

