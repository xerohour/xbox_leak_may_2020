/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    ixsysbus.c

Abstract:

--*/

#include "halp.h"

ULONG
HalGetInterruptVector(
    IN ULONG BusInterruptLevel,
    OUT PKIRQL Irql
    )

/*++

Routine Description:

Arguments:

    BusInterruptLevel - Supplies the bus specific interrupt level.

    Irql - Returns the system request priority.

Return Value:

    Returns the system interrupt vector corresponding to the specified device.

--*/
{
    ULONG SystemVector;

    SystemVector = BusInterruptLevel + PRIMARY_VECTOR_BASE;

    if ((SystemVector < PRIMARY_VECTOR_BASE) ||
        (SystemVector > PRIMARY_VECTOR_BASE + HIGHEST_LEVEL_FOR_8259) ) {

        //
        // This is an illegal BusInterruptLevel and cannot be connected.
        //

        return(0);
    }

    *Irql = (KIRQL)(HIGHEST_LEVEL_FOR_8259 + PRIMARY_VECTOR_BASE - SystemVector);

    return SystemVector;
}
