/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    pcispace.c

Abstract:

    This module implements the routines to interface with PCI configuration
    space.

--*/

#include "halp.h"

//
// Function prototype for PCI configuration space accessors.
//

typedef
ULONG
(*PHAL_PCISPACE_ACCESS_ROUTINE)(
    IN PCI_TYPE1_CFG_BITS CfgBits,
    IN ULONG RegisterNumber,
    OUT PVOID Buffer
    );

ULONG
HalpReadPCISpaceUchar(
    IN PCI_TYPE1_CFG_BITS CfgBits,
    IN ULONG RegisterNumber,
    OUT PVOID Buffer
    )
/*++

Routine Description:

    This routine reads one byte from PCI configuration space.

Arguments:

    CfgBits - Specifies the bus, device, and function number to read from.

    RegisterNumber - Specifies the register number to transfer.

    Buffer - Specifies the location to receive the data read.

Return Value:

    The number of bytes read from PCI configuration space.

--*/
{
    ULONG RegisterByteOffset;

    RegisterByteOffset = RegisterNumber % sizeof(ULONG);
    CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);

    _outpd(PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);
    *((PUCHAR)Buffer) = (UCHAR)_inp(PCI_TYPE1_DATA_PORT + RegisterByteOffset);

    return sizeof(UCHAR);
}

ULONG
HalpReadPCISpaceUshort(
    IN PCI_TYPE1_CFG_BITS CfgBits,
    IN ULONG RegisterNumber,
    OUT PVOID Buffer
    )
/*++

Routine Description:

    This routine reads two bytes from PCI configuration space.

Arguments:

    CfgBits - Specifies the bus, device, and function number to read from.

    RegisterNumber - Specifies the register number to transfer.

    Buffer - Specifies the location to receive the data read.

Return Value:

    The number of bytes read from PCI configuration space.

--*/
{
    ULONG RegisterByteOffset;

    RegisterByteOffset = RegisterNumber % sizeof(ULONG);
    CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);

    _outpd(PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);
    *((PUSHORT)Buffer) = (USHORT)_inpw(PCI_TYPE1_DATA_PORT + RegisterByteOffset);

    return sizeof(USHORT);
}

ULONG
HalpReadPCISpaceUlong(
    IN PCI_TYPE1_CFG_BITS CfgBits,
    IN ULONG RegisterNumber,
    OUT PVOID Buffer
    )
/*++

Routine Description:

    This routine reads four bytes from PCI configuration space.

Arguments:

    CfgBits - Specifies the bus, device, and function number to read from.

    RegisterNumber - Specifies the register number to transfer.

    Buffer - Specifies the location to receive the data read.

Return Value:

    The number of bytes read from PCI configuration space.

--*/
{
    ASSERT((RegisterNumber % sizeof(ULONG)) == 0);

    CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);

    _outpd(PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);
    *((PULONG)Buffer) = _inpd(PCI_TYPE1_DATA_PORT);

    return sizeof(ULONG);
}

ULONG
HalpWritePCISpaceUchar(
    IN PCI_TYPE1_CFG_BITS CfgBits,
    IN ULONG RegisterNumber,
    IN PVOID Buffer
    )
/*++

Routine Description:

    This routine writes one byte to PCI configuration space.

Arguments:

    CfgBits - Specifies the bus, device, and function number to write to.

    RegisterNumber - Specifies the register number to transfer.

    Buffer - Specifies the location that has the data to write out.

Return Value:

    The number of bytes written to PCI configuration space.

--*/
{
    ULONG RegisterByteOffset;

    RegisterByteOffset = RegisterNumber % sizeof(ULONG);
    CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);

    _outpd(PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);
    _outp(PCI_TYPE1_DATA_PORT + RegisterByteOffset, *((PUCHAR)Buffer));

    return sizeof(UCHAR);
}

ULONG
HalpWritePCISpaceUshort(
    IN PCI_TYPE1_CFG_BITS CfgBits,
    IN ULONG RegisterNumber,
    IN PVOID Buffer
    )
/*++

Routine Description:

    This routine writes two bytes to PCI configuration space.

Arguments:

    CfgBits - Specifies the bus, device, and function number to write to.

    RegisterNumber - Specifies the register number to transfer.

    Buffer - Specifies the location that has the data to write out.

Return Value:

    The number of bytes written to PCI configuration space.

--*/
{
    ULONG RegisterByteOffset;

    RegisterByteOffset = RegisterNumber % sizeof(ULONG);
    CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);

    _outpd(PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);
    _outpw(PCI_TYPE1_DATA_PORT + RegisterByteOffset, *((PUSHORT)Buffer));

    return sizeof(USHORT);
}

ULONG
HalpWritePCISpaceUlong(
    IN PCI_TYPE1_CFG_BITS CfgBits,
    IN ULONG RegisterNumber,
    IN PVOID Buffer
    )
/*++

Routine Description:

    This routine writes four bytes to PCI configuration space.

Arguments:

    CfgBits - Specifies the bus, device, and function number to write to.

    RegisterNumber - Specifies the register number to transfer.

    Buffer - Specifies the location that has the data to write out.

Return Value:

    The number of bytes written to PCI configuration space.

--*/
{
    ASSERT((RegisterNumber % sizeof(ULONG)) == 0);

    CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);

    _outpd(PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);
    _outpd(PCI_TYPE1_DATA_PORT, *((PULONG)Buffer));

    return sizeof(ULONG);
}

//
// Array of PCI configuration space read routines.
//
const PHAL_PCISPACE_ACCESS_ROUTINE HalpPCISpaceReaders[] = {
    HalpReadPCISpaceUlong,
    HalpReadPCISpaceUchar,
    HalpReadPCISpaceUshort
};

//
// Array of PCI configuration space write routines.
//
const PHAL_PCISPACE_ACCESS_ROUTINE HalpPCISpaceWriters[] = {
    HalpWritePCISpaceUlong,
    HalpWritePCISpaceUchar,
    HalpWritePCISpaceUshort
};

//
// Matrix of indexes into the above read and write routine arrays in order to
// access a given byte offset and transfer length.
//
const UCHAR HalpPCISpaceAccessMatrix[4][4] = {
    {0, 1, 2, 2},
    {1, 1, 1, 1},
    {2, 1, 2, 2},
    {1, 1, 1, 1}
};

VOID
HalReadWritePCISpace(
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN ULONG RegisterNumber,
    IN PVOID Buffer,
    IN ULONG Length,
    IN BOOLEAN WritePCISpace
    )
/*++

Routine Description:

    This routine reads from or writes to a buffer with PCI configuration space.

Arguments:

    BusNumber - Specifies the desired PCI bus number.

    SlotNumber - Specifies the desired PCI device and function number.

    RegisterNumber - Specifies the desired starting PCI register number.

    Buffer - Specifies the buffer to receive the data or supply the data for the
        transfer.

    Length - Specifies the number of bytes to transfer.

    WritePCISpace - Specifies TRUE if data should be written to PCI
        configuration space, else FALSE if data should be read from PCI
        configuration space.

Return Value:

    None.

--*/
{
    PCI_SLOT_NUMBER PCISlotNumber;
    PCI_TYPE1_CFG_BITS CfgBits;
#if DBG
    USHORT DbgVendorID;
#endif
    const PHAL_PCISPACE_ACCESS_ROUTINE *PCISpaceAccessors;
    ULONG BytesTransferred;
    UCHAR AccessIndex;

    ASSERT(BusNumber <= PCI_MAX_BRIDGE_NUMBER);
    ASSERT(RegisterNumber < sizeof(PCI_COMMON_CONFIG));

    //
    // Initialize the static portion of the configuration bits.
    //

    PCISlotNumber.u.AsULONG = SlotNumber;

    CfgBits.u.AsULONG = 0;
    CfgBits.u.bits.BusNumber = BusNumber;
    CfgBits.u.bits.DeviceNumber = PCISlotNumber.u.bits.DeviceNumber;
    CfgBits.u.bits.FunctionNumber = PCISlotNumber.u.bits.FunctionNumber;
    CfgBits.u.bits.Enable = 1;

    //
    // Synchronize access to PCI configuration space by disabling interrupts.
    //

    _disable();

#if DBG
    //
    // If we're writing out to PCI space, then assert that there's a device
    // connected to the supplied bus/device/function number.
    //

    if (WritePCISpace) {
        HalpReadPCISpaceUshort(CfgBits, 0, &DbgVendorID);
        ASSERT(DbgVendorID != PCI_INVALID_VENDORID);
    }
#endif

    //
    // Determine which set of function pointers to use to access PCI
    // configuration space.
    //

    PCISpaceAccessors = WritePCISpace ? HalpPCISpaceWriters : HalpPCISpaceReaders;

    //
    // Transfer the data to or from PCI configuration space.
    //

    while (Length > 0) {

        AccessIndex = HalpPCISpaceAccessMatrix[RegisterNumber % sizeof(ULONG)][Length % sizeof(ULONG)];

        BytesTransferred = PCISpaceAccessors[AccessIndex](CfgBits,
            RegisterNumber, Buffer);

        RegisterNumber += BytesTransferred;
        Buffer = (PUCHAR)Buffer + BytesTransferred;
        Length -= BytesTransferred;
    }

    //
    // Reenable interrupts now that we're finished accessing PCI configuration
    // space.
    //

    _enable();
}
