/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    kdlite.c

Abstract:

    This module implements the Xbox lite kernel debugger.

--*/

#include "kdlitep.h"

//
// Stores whether or not the debugger is enabled.  The debugger is only enabled
// if a super I/O controller is present.
//
DECLSPEC_STICKY BOOLEAN KdDebuggerEnabled;

//
// Hardwired to TRUE, since a real debugger is never really present.
//
BOOLEAN KdDebuggerNotPresent = TRUE;

//
// Local support.
//

VOID
KdpConfigureSerialPort(
    VOID
    );

VOID
KdpInitializeLoadedModuleList(
    VOID
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, KdpConfigureSerialPort)
#endif

VOID
KdpConfigureSerialPort(
    VOID
    )
/*++

Routine Description:

    This routine configures the serial port for the desired baud rate and port
    options.

Arguments:

    None.

Return Value:

    None.

--*/
{
    ULONG DivisorLatch;

    DivisorLatch = CLOCK_RATE / KDLITE_DEBUG_BAUD_RATE;

    _outp(KDLITE_DEBUG_BASE_ADDRESS + COM_LCR, 0x83);
    _outp(KDLITE_DEBUG_BASE_ADDRESS + COM_DLM, (UCHAR)(DivisorLatch >> 8));
    _outp(KDLITE_DEBUG_BASE_ADDRESS + COM_DLL, (UCHAR)(DivisorLatch));
    _outp(KDLITE_DEBUG_BASE_ADDRESS + COM_LCR, 0x03);

    _outp(KDLITE_DEBUG_BASE_ADDRESS + COM_MCR, MC_DTRRTS);
    _outp(KDLITE_DEBUG_BASE_ADDRESS + COM_IEN, 0);
}

VOID
KdpPrintString(
    IN PSTRING String
    )
/*++

Routine Description:

    This routine prints out the supplied string to the serial port.

Arguments:

    String - Specifies the string to be printed to the serial port.

Return Value:

    None.

--*/
{
    BOOLEAN Restore;
    ULONG Length;
    PUCHAR Buffer;

    //
    // If the debugger isn't enabled, don't do anything.
    //

    if (!KdDebuggerEnabled) {
        return;
    }

    //
    // Write out each character to the serial port.  Synchronize by disabling
    // interrupts.
    //

    Restore = KiDisableInterrupts();

    Length = String->Length;
    Buffer = String->Buffer;

    while (Length > 0) {

        while (!(_inp(KDLITE_DEBUG_BASE_ADDRESS + COM_LSR) & COM_OUTRDY));
        _outp(KDLITE_DEBUG_BASE_ADDRESS + COM_DAT, *Buffer);

        Length--;
        Buffer++;
    }

    KiRestoreInterrupts(Restore);
}

BOOLEAN
KdpTrap(
    IN PKTRAP_FRAME TrapFrame,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PCONTEXT ContextRecord,
    IN BOOLEAN SecondChance
    )
/*++

Routine Description:

    This routine is called whenever a exception is dispatched.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame that describes the
        trap.

    ExceptionFrame - Supplies a pointer to a exception frame that describes
        the trap.

    ExceptionRecord - Supplies a pointer to an exception record that
        describes the exception.

    ContextRecord - Supplies the context at the time of the exception.

    SecondChance - Supplies a boolean value that determines whether this is
        the second chance (TRUE) that the exception has been raised.

Return Value:

    A value of TRUE is returned if the exception is handled. Otherwise a
    value of FALSE is returned.

--*/
{
    BOOLEAN Handled = FALSE;

    if ((ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT) &&
        (ExceptionRecord->NumberParameters > 0) &&
        ((ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_LOAD_SYMBOLS)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_UNLOAD_SYMBOLS)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_LOAD_XESECTION)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_UNLOAD_XESECTION)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_KDPRINT)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_PRINT))) {

        if ((ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_KDPRINT)||
            (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_PRINT)) {
            KdpPrintString((PSTRING)ExceptionRecord->ExceptionInformation[1]);
            ContextRecord->Eax = STATUS_SUCCESS;
        }

        ContextRecord->Eip++;
        Handled = TRUE;
    }

    return Handled;
}

VOID
KdInitSystem(
    BOOLEAN InitializingSystem
    )
/*++

Routine Description:

    This routine initializes the lite kernel debugger.

Arguments:

    InitializingSystem - Supplies a boolean value that determines whether we're
        called in the context of system initialization or bugcheck code.

Return Value:

    None.

--*/
{
    if (InitializingSystem) {

        //
        // If the kernel is cold-booting, then determine whether or not a super
        // I/O controller is attached to the system so that we know whether or
        // not to output debug spew through the serial port.
        //

        if (!KeHasQuickBooted) {

            HalPulseHardwareMonitorPin();
            KdDebuggerEnabled = HalInitializeSuperIo();

            if (KdDebuggerEnabled) {
                KdpConfigureSerialPort();
            }
        }

        //
        // Set the debug routine to point at our trap handler.
        //

        KiDebugRoutine = KdpTrap;

#ifdef DEVKIT
        //
        // For DEVKIT builds of this library, we still need to initialize the
        // module list so that the debug monitor will continue to function.
        //

        KdpInitializeLoadedModuleList();
#endif
    }
}

#ifdef DEVKIT

//
// The following code and data are present for DEVKIT builds to enable testing
// of the lite kernel debugger on a DEVKIT platform.  None of this code is used
// by a retail build of the system.
//

//
// Set to TRUE if the are any breakpoints that need to be applied to pages that
// are not inpaged.  Always FALSE since this implementation never applies
// breakpoints.
//
BOOLEAN KdpOweBreakpoint;

//
// Static loader data table entry for XBOXKRNL.EXE.
//
LDR_DATA_TABLE_ENTRY KdpNtosDataTableEntry;

//
// List of modules that have been loaded.
//
INITIALIZED_LIST_ENTRY(KdLoadedModuleList);

VOID
KdpInitializeLoadedModuleList(
    VOID
    )
/*++

Routine Description:

    This routine initializes the static loader data table entry for the kernel
    and attaches it to the loaded module list.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // Initialize the loader data table entry for XBOXKRNL.EXE and attach it
    // to the loaded module list.
    //

    KdpNtosDataTableEntry.DllBase = PsNtosImageBase;
    KdpNtosDataTableEntry.SizeOfImage =
        RtlImageNtHeader(PsNtosImageBase)->OptionalHeader.SizeOfImage;
    KdpNtosDataTableEntry.LoadedImports = (PVOID)MAXULONG_PTR;
    RtlInitUnicodeString(&KdpNtosDataTableEntry.FullDllName, L"xboxkrnl.exe");
    RtlInitUnicodeString(&KdpNtosDataTableEntry.BaseDllName, L"xboxkrnl.exe");

    InsertTailList(&KdLoadedModuleList, &KdpNtosDataTableEntry.InLoadOrderLinks);
}

BOOLEAN
KdPollBreakIn(
    VOID
    )
/*++

Routine Description:

    This routine checks if a breakin packet is pending.

Arguments:

    None.

Return Value:

    Returns FALSE to indicate that no breakin packet is pending.

--*/
{
    return FALSE;
}

VOID
KdSetOwedBreakpoints(
    VOID
    )
/*++

Routine Description:

    This routine is called by the page fault handler to store pending
    breakpoints for pages that may have been just made valid.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // This routine should never be called because this implementation never
    // applies breakpoints and never sets KdpOweBreakpoint to TRUE.
    //

    KeBugCheck(0);
}

VOID
KdDeleteAllBreakpoints(
    VOID
    )
/*++

Routine Description:

    This routine is called to delete all breakpoints from the system.

Arguments:

    None.

Return Value:

    None.

--*/
{
    NOTHING;
}

#endif
