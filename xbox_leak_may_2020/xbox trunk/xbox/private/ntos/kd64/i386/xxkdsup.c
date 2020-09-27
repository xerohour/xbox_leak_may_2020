/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    xxkdsup.c

Abstract:

    Com support.  Code to init a com port, store port state, map
    portable procedures to x86 procedures.

Author:

    Bryan M. Willman (bryanwi) 24-Sep-90

Revision History:

    Shielin Tzong (shielint) 10-Apr-91
                Add packet control protocol.

    John Vert (jvert) 11-Jul-1991
        Moved from KD/i386 to HAL

--*/

#include "kdp.h"
#include "ixkdcom.h"

//
// This MUST be initialized to zero so we know not to do anything when
// CpGetByte is called when the kernel debugger is disabled.
//

CPPORT KdPort = {NULL, 0, PORT_DEFAULTRATE };

//
// Remember the debugger port information
//

CPPORT KdPortInformation = {NULL, 0, PORT_DEFAULTRATE};

BOOLEAN
KdPortInitialize(
    PDEBUG_PARAMETERS DebugParameters,
    BOOLEAN Initialize
    )

/*++

Routine Description:

    This procedure checks for which COM port should be used by kernel
    debugger.  If DebugParameter specifies a COM port, we will use it
    even if we can not find it (we trust user).  Otherwise, if COM2
    is present and there is no mouse attaching to it, we use COM2.
    If COM2 is not availabe, we check COM1.  If both COM1 and COM2 are
    not present, we give up and return false.

Arguments:

    DebugParameters - Supplies a pointer a structure which optionally
                      sepcified the debugging port information.

    Initialize - Specifies a boolean value that determines whether the
        debug port is initialized or just the debug port parameters
        are captured.

Returned Value:

    TRUE - If a debug port is found.

--*/

{

    ULONG i;
    ULONG BaudRate = 115200;
    PUCHAR PortAddress = (PUCHAR)COM1_PORT;

    //
    // Check if Port and baudrate have been determined.
    //

    if (KdPortInformation.Address == NULL) {

        //
        // Initialize the port structure.
        //

        KdPortInformation.Address = PortAddress;
        KdPortInformation.Baud = BaudRate;
    }

    if (Initialize == TRUE) {
        CpInitialize(&KdPort,
                     KdPortInformation.Address,
                     KdPortInformation.Baud
                     );
    }
    return(TRUE);
}

ULONG
KdPortGetByte (
    OUT PUCHAR Input
    )

/*++

Routine Description:

    Fetch a byte from the debug port and return it.

    N.B. It is assumed that the IRQL has been raised to the highest level, and
        necessary multiprocessor synchronization has been performed before this
        routine is called.

Arguments:

    Input - Returns the data byte.

Return Value:

    CP_GET_SUCCESS is returned if a byte is successfully read from the
        kernel debugger line.
    CP_GET_ERROR is returned if error encountered during reading.
    CP_GET_NODATA is returned if timeout.

--*/

{
    return CpGetByte(&KdPort, Input, TRUE);
}

ULONG
KdPortPollByte (
    OUT PUCHAR Input
    )

/*++

Routine Description:

    Fetch a byte from the debug port and return it if one is available.

    N.B. It is assumed that the IRQL has been raised to the highest level, and
        necessary multiprocessor synchronization has been performed before this
        routine is called.

Arguments:

    Input - Returns the data byte.

Return Value:

    CP_GET_SUCCESS is returned if a byte is successfully read from the
        kernel debugger line.
    CP_GET_ERROR is returned if error encountered during reading.
    CP_GET_NODATA is returned if timeout.

--*/

{
    return CpGetByte(&KdPort, Input, FALSE);
}

VOID
KdPortPutByte (
    IN UCHAR Output
    )

/*++

Routine Description:

    Write a byte to the debug port.

    N.B. It is assumed that the IRQL has been raised to the highest level, and
        necessary multiprocessor synchronization has been performed before this
        routine is called.

Arguments:

    Output - Supplies the output data byte.

Return Value:

    None.

--*/

{
    CpPutByte(&KdPort, Output);
}
