/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    ldrx.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the Xbox executable image loader.

--*/

#ifndef _LDRX_
#define _LDRX_

#include <ntos.h>
#include <ani.h>
#include <ldr.h>
#include <dm.h>
#include <ntddcdvd.h>
#include <xbeimage.h>
#include <smcdef.h>
#include <limits.h>
#include <wtypes.h>
#include <xbox.h>
#include <xboxp.h>
#include <xconfig.h>
#include <xlaunch.h>
#include <pshpack4.h>

//
// DBG sensitive DbgPrint wrapper.
//

#if DBG
#define LdrxDbgPrint(x)                         DbgPrint x
#define LdrxDbgBreakPoint()                     DbgBreakPoint()
#else
#define LdrxDbgPrint(x)
#define LdrxDbgBreakPoint()
#endif

//
// Bit flag macros.
//

#define LdrxIsFlagSet(flagset, flag)            (((flagset) & (flag)) != 0)
#define LdrxIsFlagClear(flagset, flag)          (((flagset) & (flag)) == 0)

//
// Define the title identifier of the dashboard.
//

#define LDR_DASHBOARD_TITLE_ID                  0xFFFE0000

//
// External symbols.
//

VOID
DebugLoadImageSymbols(
    IN PSTRING FileName,
    IN PKD_SYMBOLS_INFO SymbolInfo
    );

NTSTATUS
DebugService(
    ULONG ServiceClass,
    PVOID Arg1,
    PVOID Arg2
    );

DECLSPEC_NORETURN
VOID
IdexDiskFatalError(
    IN ULONG ErrorCode
    );

extern PDEVICE_OBJECT IdexCdRomDeviceObject;
extern BOOLEAN IdexCdRomDVDX2Authenticated;
extern BOOLEAN IdexDiskSecurityUnlocked;

#include <poppack.h>

#endif  // LDRX
