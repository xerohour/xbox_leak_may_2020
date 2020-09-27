/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    kdlitep.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the Xbox lite kernel debugger.

--*/

#ifndef _KDLITEP_
#define _KDLITEP_

#include <ntos.h>
#include "ki.h"

#pragma intrinsic(_inp,_inpw,_inpd,_outp,_outpw,_outpd)

//
// Define the base address of the serial port.
//

#define KDLITE_DEBUG_BASE_ADDRESS   0x3F8

//
// Define the baud rate for transmission over the serial port.
//

#define KDLITE_DEBUG_BAUD_RATE      19200

//
// Define various constants related to the serial port.
//

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

//
// External symbols.
//

extern PKDEBUG_ROUTINE KiDebugRoutine;

#endif  // KDLITEP
