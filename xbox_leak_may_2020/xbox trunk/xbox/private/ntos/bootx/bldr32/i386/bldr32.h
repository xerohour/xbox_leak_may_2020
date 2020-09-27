/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    bldr32.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the boot loader.

--*/

#ifndef _BOOT32_
#define _BOOT32_

#include <ntos.h>

//
// To enable spew, define BLDRSPEW below
//

#undef BLDRSPEW

//
// To enable a memory test before and after calibration, define BLDRMEMTEST below
// This should only be used for debug purposes
//

#undef BLDRMEMTEST

//
// PCI space access
//

#define PCI_TYPE1_ADDR_PORT         ((PULONG) 0xCF8)
#define PCI_TYPE1_DATA_PORT         0xCFC

#pragma intrinsic(_inp,_inpw,_inpd,_outp,_outpw,_outpd)


typedef unsigned long DWORD;

VOID
BldrEncDec(
    IN  PUCHAR  MsgData,
    IN  ULONG   MsgDataLen,
    IN  const UCHAR * Key,
    IN  ULONG   KeyBytes
    );

VOID
BldrCopyROMToRAM(
    IN const UCHAR * DecryptionKey
    );

VOID
BldrShutdownSystem(
    VOID
    );

VOID
__fastcall
BldrLoadMediaROM(
    ULONG_PTR MediaROMBaseAddress
    );

VOID 
BldrInitTimer(
    PVOID RegisterBase
    );

VOID
BldrSetDrvSlwFromROMData(
    PVOID RegisterBase
    );

VOID
BldrSetupLDTBus(
    VOID
    );

//
// Smbus access routines
//
    
NTSTATUS
BldrReadSMBusValue(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN ReadWordValue,
    OUT ULONG *DataValue
    );

#define BldrReadSMBusByte(SlaveAddress, CommandCode, DataValue) \
    BldrReadSMBusValue(SlaveAddress, CommandCode, FALSE, DataValue)

#define BldrReadSMBusWord(SlaveAddress, CommandCode, DataValue) \
    BldrReadSMBusValue(SlaveAddress, CommandCode, TRUE, DataValue)

NTSTATUS
BldrWriteSMBusValue(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN WriteWordValue,
    IN ULONG DataValue
    );

#define BldrWriteSMBusByte(SlaveAddress, CommandCode, DataValue) \
    BldrWriteSMBusValue(SlaveAddress, CommandCode, FALSE, DataValue)

#define BldrWriteSMBusWord(SlaveAddress, CommandCode, DataValue) \
    BldrWriteSMBusValue(SlaveAddress, CommandCode, TRUE, DataValue)

#define SMC_SLAVE_ADDRESS                           0x20

VOID
DbgPrintHex(
    PUCHAR Buffer,
    ULONG  BufferSize
    );


#ifdef BLDRSPEW
#define BldrPrint(x)                DbgPrint x
#else
#define BldrPrint(x)
#endif

//
// Following structure exists at BLDR_CODE location.  BLDRBLD reserves spaces
// for it and fills in the Bldr32EntryPoint.  The rest of the fields are filled by
// DOSBOOT when booting from DOS.  When booting from ROM, these fields contain 0
//
typedef struct _BOOTLDRPARAM {
    ULONG Bldr32EntryPoint;
    CHAR  CommandLine[64];
} BOOTLDRPARAM, *PBOOTLDRPARAM;

#endif // BOOT32
