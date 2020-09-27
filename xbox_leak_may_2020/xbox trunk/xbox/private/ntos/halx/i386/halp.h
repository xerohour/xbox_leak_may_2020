/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1991  Microsoft Corporation

Module Name:

    halp.h

Abstract:

    This header file defines the private Hardware Architecture Layer (HAL)
    interfaces, defines and structures.

--*/

#ifndef _HALP_
#define _HALP_

#include <ntos.h>
#include <hal.h>
#include <pci.h>
#include <smcdef.h>

#pragma intrinsic(_inp,_inpw,_inpd,_outp,_outpw,_outpd)
#pragma intrinsic(_enable,_disable)

#ifdef RtlMoveMemory
#undef RtlMoveMemory
#undef RtlCopyMemory
#undef RtlFillMemory
#undef RtlZeroMemory

#define RtlCopyMemory(Destination,Source,Length) RtlMoveMemory((Destination),(Source),(Length))

VOID
RtlMoveMemory (
   PVOID Destination,
   CONST VOID *Source,
   ULONG Length
   );

VOID
RtlFillMemory (
   PVOID Destination,
   ULONG Length,
   UCHAR Fill
   );

VOID
RtlZeroMemory (
   PVOID Destination,
   ULONG Length
   );

#endif

#include "ix8259.inc"

//
// DBG sensitive DbgPrint wrapper.
//

#if DBG
#define HalDbgPrint(x)                          DbgPrint x
#else
#define HalDbgPrint(x)
#endif

//
// Define function prototypes.
//

VOID
HalpSetupPCIDevices(
    VOID
    );

VOID
HalpProfileInterrupt(
    VOID
    );

VOID
HalpInitializeClock(
    VOID
    );

VOID
HalpInitializeStallExecution(
    VOID
    );

VOID
HalpInitializePICs(
    VOID
    );

VOID
HalpInitializeRealTimeClock(
    VOID
    );

VOID
HalpInitializeSystemControlInterrupt(
    VOID
    );

VOID
HalpSystemControlInterrupt(
    VOID
    );

VOID
HalpSMBusInterrupt(
    VOID
    );

VOID
HalpProcessSMCInitialState(
    VOID
    );

ULONGLONG
FASTCALL
RDMSR (
    IN ULONG MsrAddress
    );

VOID
WRMSR (
    IN ULONG        MsrAddress,
    IN ULONGLONG    MsrValue
    );

//
// Disable the Local APIC on UP (PIC 8259) PentiumPro systems to work around
// spurious interrupt errata.
//
#define APIC_BASE_MSR       0x1B
#define APIC_ENABLED        0x0000000000000800

//
// Definitions provided by NVidia for remapping USB ports
// between host controllers.
//

#define MCP_USB_CFG_20           0x00000050 /* RW-4R */
#define MCP_USB_CFG_20_PORT0     0x00000001 /* RWI-V */
#define MCP_USB_CFG_20_PORT1     0x00000002 /* RWI-V */
#define MCP_USB_CFG_20_PORT2     0x00000004 /* RWI-V */
#define MCP_USB_CFG_20_PORT3     0x00000008 /* RW--V */
#define MCP_USB_CFG_20_PORT4     0x00000010 /* RW--V */
#define MCP_USB_CFG_20_PORT5     0x00000020 /* RW--V */

extern KEVENT HalpTrayEjectCompleteEvent;

#endif // _HALP_
