/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    ixkdcom.c

Abstract:

    This module contains a very simply package to do com I/O on machines
    with standard AT com-ports.  It is C code derived from the debugger's
    com code.  Likely does not work on a PS/2.  (Only rewrote the thing
    into C so we wouldn't have to deal with random debugger macros.)

    Procedures to init a com object, set and query baud rate, output
    character, input character.

Author:

    Bryan M. Willman (bryanwi) 24-Sep-1990

Revision History:

    John Vert (jvert) 12-Jun-1991
        Added ability to check for com-port's existence and hook onto the
        highest com-port available.

    John Vert (jvert) 19-Jul-1991
        Moved into HAL

--*/

#include    "kdp.h"
#include    "ixkdcom.h"
// timeout was 1024 * 500, but at 115200 baud, this should be enough
#define     TIMEOUT_COUNT  1024 * 100

UCHAR CpReadLsr (PCPPORT, UCHAR);

BOOLEAN KdDbgPortsPresent = TRUE;
UCHAR   KdDbgErrorCount;

#define DBG_ACCEPTABLE_ERRORS   25

VOID
CpInitialize (
    PCPPORT Port,
    PUCHAR  Address,
    ULONG  Rate
    )

/*++

    Routine Description:

        Fill in the com port port object, set the initial baud rate,
        turn on the hardware.

    Arguments:

        Port - address of port object

        Address - port address of the com port
                    (CP_COM1_PORT, CP_COM2_PORT)

        Rate - baud rate  (CP_BD_150 ... CP_BD_19200)

--*/

{
    PUCHAR hwport;
    UCHAR   mcr, ier;

    Port->Address = Address;
    Port->Baud = 0;

    CpSetBaud(Port, Rate);

    //
    // Assert DTR, RTS.
    //

    hwport = Port->Address;
    hwport += COM_MCR;

    mcr = MC_DTRRTS;
    _outp(hwport, mcr);

    hwport = Port->Address;
    hwport += COM_IEN;

    ier = 0;
    _outp(hwport, ier);
}

VOID
CpSetBaud (
    PCPPORT  Port,
    ULONG   Rate
    )

/*++

    Routine Description:

        Set the baud rate for the port and record it in the port object.

    Arguments:

        Port - address of port object

        Rate - baud rate  (CP_BD_150 ... CP_BD_56000)

--*/

{
    ULONG   divisorlatch;
    PUCHAR  hwport;
    UCHAR   lcr;

    //
    // compute the divsor
    //

    divisorlatch = CLOCK_RATE / Rate;

    //
    // set the divisor latch access bit (DLAB) in the line control reg
    //

    hwport = Port->Address;
    hwport += COM_LCR;                  // hwport = LCR register

    lcr = (UCHAR)_inp(hwport);

    lcr |= LC_DLAB;
    _outp(hwport, lcr);

    //
    // set the divisor latch value.
    //

    hwport = Port->Address;
    hwport += COM_DLM;                  // divisor latch msb
    _outp(hwport, (UCHAR)((divisorlatch >> 8) & 0xff));

    hwport--;                           // divisor latch lsb
    _outp(hwport, (UCHAR)(divisorlatch & 0xff));


    //
    // Set LCR to 3.  (3 is a magic number in the original assembler)
    //

    hwport = Port->Address;
    hwport += COM_LCR;
    _outp(hwport, 3);


    //
    // Remember the baud rate
    //

    Port->Baud = Rate;
}

UCHAR
CpReadLsr (
    PCPPORT Port,
    UCHAR   waiting
    )

/*++

    Routine Description:

        Read LSR byte from specified port.  If HAL owns port & display
        it will also cause a debug status to be kept up to date.

        Handles entering & exiting modem control mode for debugger.

    Arguments:

        Port - Address of CPPORT

    Returns:

        Byte read from port

--*/
{
    UCHAR   lsr;

    lsr = (UCHAR)_inp(Port->Address + COM_LSR);

    //
    // Check to see if the port still exists.
    //
    if (lsr == SERIAL_LSR_NOT_PRESENT) {

        KdDbgErrorCount++;

        if (KdDbgErrorCount >= DBG_ACCEPTABLE_ERRORS) {
            KdDbgPortsPresent = FALSE;
            KdDbgErrorCount = 0;
        }
        return SERIAL_LSR_NOT_PRESENT;
    }

    if (lsr & waiting) {
        return lsr;
    }

    return lsr;
}

VOID
CpPutByte (
    PCPPORT  Port,
    UCHAR   Byte
    )

/*++

    Routine Description:

        Write a byte out to the specified com port.

    Arguments:

        Port - Address of CPPORT object

        Byte - data to emit

--*/

{
    if (KdDbgPortsPresent == FALSE) {
        return;
    }

    //
    //  Wait for port to not be busy
    //

    while (!(CpReadLsr(Port, COM_OUTRDY) & COM_OUTRDY)) ;

    //
    // Send the byte
    //

    _outp(Port->Address + COM_DAT, Byte);
}

USHORT
CpGetByte (
    PCPPORT  Port,
    PUCHAR Byte,
    BOOLEAN WaitForByte
    )

/*++

    Routine Description:

        Fetch a byte and return it.

    Arguments:

        Port - address of port object that describes hw port

        Byte - address of variable to hold the result

        WaitForByte - flag indicates wait for byte or not.

    Return Value:

        CP_GET_SUCCESS if data returned.

        CP_GET_NODATA if no data available, but no error.

        CP_GET_ERROR if error (overrun, parity, etc.)

--*/

{
    UCHAR   lsr;
    UCHAR   value;
    ULONG   limitcount;

    //
    // Check to make sure the CPPORT we were passed has been initialized.
    // (The only time it won't be initialized is when the kernel debugger
    // is disabled, in which case we just return.)
    //

    if (Port->Address == NULL) {
        return(CP_GET_NODATA);
    }

    if (KdDbgPortsPresent == FALSE) {

        if (CpReadLsr(Port, COM_DATRDY) == SERIAL_LSR_NOT_PRESENT) {

            return(CP_GET_NODATA);
        } else {

            CpSetBaud(Port, Port->Baud);
            KdDbgPortsPresent = TRUE;
        }
    }

    limitcount = WaitForByte ? TIMEOUT_COUNT : 1;
    while (limitcount != 0) {
        limitcount--;

        lsr = CpReadLsr(Port, COM_DATRDY);

        if (lsr == SERIAL_LSR_NOT_PRESENT) {
            return(CP_GET_NODATA);
        }

        if ((lsr & COM_DATRDY) == COM_DATRDY) {

            //
            // Check for errors
            //
            if (lsr & (COM_FE | COM_PE | COM_OE)) {
                *Byte = 0;
                return(CP_GET_ERROR);
            }

            //
            // fetch the byte
            //

            value = (UCHAR)_inp(Port->Address + COM_DAT);

            *Byte = value & (UCHAR)0xff;
            return CP_GET_SUCCESS;
        }
    }

    CpReadLsr (Port, 0);
    return CP_GET_NODATA;
}
