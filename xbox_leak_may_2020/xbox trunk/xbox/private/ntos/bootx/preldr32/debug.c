/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    debug.c

Abstract:

    This module implements routines to assist debugging the boot loader.

--*/

#include <ntos.h>
#include <stdio.h>
#include <dos.h>

VOID
DbgInitializeSuperIo(
    VOID
    );

#define COM_DAT     0x00
#define COM_IEN     0x01            // interrupt enable register
#define COM_LCR     0x03            // line control registers
#define COM_MCR     0x04            // modem control reg
#define COM_LSR     0x05            // line status register
#define COM_MSR     0x06            // modem status register
#define COM_DLL     0x00            // divisor latch least sig
#define COM_DLM     0x01            // divisor latch most sig

#define COM_BI      0x10
#define COM_FE      0x08
#define COM_PE      0x04
#define COM_OE      0x02

#define LC_DLAB     0x80            // divisor latch access bit

#define CLOCK_RATE  0x1C200         // USART clock rate

#define MC_DTRRTS   0x03            // Control bits to assert DTR and RTS
#define MS_DSRCTSCD 0xB0            // Status bits for DSR, CTS and CD
#define MS_CD       0x80

#define COM_OUTRDY  0x20
#define COM_DATRDY  0x01

BOOLEAN DbgInitialized = FALSE;
USHORT DbgBaseAddress = 0x3F8;
ULONG DbgBaudRate = 19200;

VOID
DbgInitialize(
    VOID
    )
{
    ULONG DivisorLatch;

    DbgInitializeSuperIo();

    DivisorLatch = CLOCK_RATE / DbgBaudRate;

    _outp(DbgBaseAddress + COM_LCR, 0x83);
    _outp(DbgBaseAddress + COM_DLM, (UCHAR)(DivisorLatch >> 8));
    _outp(DbgBaseAddress + COM_DLL, (UCHAR)(DivisorLatch));
    _outp(DbgBaseAddress + COM_LCR, 0x03);

    _outp(DbgBaseAddress + COM_MCR, MC_DTRRTS);
    _outp(DbgBaseAddress + COM_IEN, 0);

    DbgInitialized = TRUE;
}

VOID
DbgPrintCharacter(
    IN UCHAR Character
    )
{
    while (!(_inp(DbgBaseAddress + COM_LSR) & COM_OUTRDY));

    _outp(DbgBaseAddress + COM_DAT, Character);
}

ULONG
DbgPrint(
    PCHAR Format,
    ...
    )
{
    va_list arglist;
    UCHAR Buffer[512];
    int cb;
    int current;

    if (!DbgInitialized) {
        DbgInitialize();
    }

    va_start(arglist, Format);

    cb = _vsnprintf(Buffer, sizeof(Buffer), Format, arglist);

    va_end(arglist);

    for (current = 0; current < cb; current++) {
        DbgPrintCharacter(Buffer[current]);
    }
    DbgPrintCharacter('\r');
    DbgPrintCharacter('\n');

    return STATUS_SUCCESS;
}
