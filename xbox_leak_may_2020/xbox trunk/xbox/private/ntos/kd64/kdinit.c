/*++

Copyright (c) 1990-1999  Microsoft Corporation

Module Name:

    kdinit.c

Abstract:

    This module implements the initialization for the portable kernel debgger.

Author:

    David N. Cutler 27-July-1990

Revision History:

--*/

#include "kdp.h"

#define ROM_BASE_ADDRESS            0xFFF00000
#define ROM_VERSION_OFFSET          30 // In DWORDs (0x78 is the absolute offset)
#define ROM_VERSION_KDDELAY_FLAG    0x80000000

VOID
KdInitSystem(
    BOOLEAN InitializingSystem
    )
/*++

Routine Description:

    This routine initializes the portable kernel debugger.

Arguments:

    InitializingSystem - Supplies a boolean value that determines whether we're
        called in the context of system initialization or bugcheck code.

Return Value:

    None.

--*/

{

    ULONG Index;
    BOOLEAN SuperIoInitialized;
    BOOLEAN Initialize;
    STRING NameString;

    //
    // If kernel debugger is already initialized, then return.
    //

    if (KdDebuggerEnabled != FALSE) {
        return;
    }

    KiDebugRoutine = KdpStub;

    //
    // Determine whether or not the debugger should be enabled.
    //
    // Note that if InitializingSystem == FALSE, then KdInitSystem was called
    // from BugCheck code. For this case the debugger is always enabled
    // to report the bugcheck if possible.
    //

    if (InitializingSystem) {

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

        //
        // Initialize the debugger data block list when called at startup time.
        //

        InitializeListHead(&KdpDebuggerDataListHead);

        //
        // Fill in and register the debugger's debugger data block.
        // Most fields are already initialized, some fields will not be
        // filled in until later.
        //

        KdDebuggerDataBlock.KernBase = (ULONG_PTR) PsNtosImageBase;

        KdRegisterDebuggerDataBlock(KDBG_TAG,
                                    &KdDebuggerDataBlock.Header,
                                    sizeof(KdDebuggerDataBlock));

        //
        // Always initialize the kernel debugger if the system has a super I/O
        // controller that we can communicate with.
        //

        HalPulseHardwareMonitorPin();
        Initialize = HalInitializeSuperIo();

    } else {
        Initialize = TRUE;
    }

    if ((KdPortInitialize(&KdDebugParameters, Initialize) == FALSE) ||
        (Initialize == FALSE)) {
        return;
    }

    //
    // If we're going to delay the kernel debugger setup, then mark the debugger
    // as not present for now.
    //

    if (((PULONG)ROM_BASE_ADDRESS)[ROM_VERSION_OFFSET] & ROM_VERSION_KDDELAY_FLAG) {
        KdDebuggerNotPresent = TRUE;
    }

    //
    // Set address of kernel debugger trap routine.
    //

    KiDebugRoutine = KdpTrap;

    if (!KdpDebuggerStructuresInitialized) {

        KdpBreakpointInstruction = KDP_BREAKPOINT_VALUE;
        KdpOweBreakpoint = FALSE;

        //
        // Initialize the breakpoint table.
        //

        for (Index = 0; Index < BREAKPOINT_TABLE_SIZE; Index += 1) {
            KdpBreakpointTable[Index].Flags = 0;
            KdpBreakpointTable[Index].Address = NULL;
        }

        KdpDebuggerStructuresInitialized = TRUE ;
    }

    //
    //  Initialize timer facility - HACKHACK
    //

    KdPerformanceCounterRate = KeQueryPerformanceFrequency();
    KdTimerStart.HighPart = 0L;
    KdTimerStart.LowPart = 0L;

    //
    // Initialize ID for NEXT packet to send and Expect ID of next incoming
    // packet.
    //

    KdpNextPacketIdToSend = INITIAL_PACKET_ID | SYNC_PACKET_ID;
    KdpPacketIdExpected = INITIAL_PACKET_ID;

    //
    // Mark debugger enabled.
    //
    KdDebuggerEnabled = TRUE;

    //
    // Notify the debugger that XBOXKRNL.EXE is loaded.
    //

    if (InitializingSystem) {
        RtlInitAnsiString(&NameString, "xboxkrnl.exe");
        DbgLoadImageSymbols(&NameString, PsNtosImageBase, (ULONG)-1);
    }
}

BOOLEAN
KdRegisterDebuggerDataBlock(
    IN ULONG Tag,
    IN PDBGKD_DEBUG_DATA_HEADER64 DataHeader,
    IN ULONG Size
    )
/*++

Routine Description:

    This routine is called by a component or driver to register a
    debugger data block.  The data block is made accessible to the
    kernel debugger, thus providing a reliable method of exposing
    random data to debugger extensions.

Arguments:

    Tag - Supplies a unique 4 byte tag which is used to identify the
            data block.

    DataHeader - Supplies the address of the debugger data block header.
            The OwnerTag field must contain a unique value, and the Size
            field must contain the size of the data block, including the
            header.  If this block is already present, or there is
            already a block with the same value for OwnerTag, this one
            will not be inserted.  If Size is incorrect, this code will
            not notice, but the usermode side of the debugger might not
            function correctly.

    Size - Supplies the size of the data block, including the header.

Return Value:

    TRUE if the block was added to the list, FALSE if not.

--*/
{
    KIRQL OldIrql;
    PLIST_ENTRY List;
    PDBGKD_DEBUG_DATA_HEADER64 Header;

    OldIrql = KeRaiseIrqlToDpcLevel();

    //
    // Look for a record with the same tag or address
    //

    List = KdpDebuggerDataListHead.Flink;

    while (List != &KdpDebuggerDataListHead) {

        Header = CONTAINING_RECORD(List, DBGKD_DEBUG_DATA_HEADER64, List);

        List = List->Flink;

        if ((Header == DataHeader) || (Header->OwnerTag == Tag)) {
            KeLowerIrql(OldIrql);
            return FALSE;
        }
    }

    //
    // It wasn't already there, so add it.
    //

    DataHeader->OwnerTag = Tag;
    DataHeader->Size = Size;

    InsertTailList(&KdpDebuggerDataListHead, (PLIST_ENTRY)(&DataHeader->List));

    KeLowerIrql(OldIrql);

    return TRUE;
}

VOID
KdLogDbgPrint(
    IN PSTRING String
    )
{
    KIRQL OldIrql;
    ULONG Length;
    ULONG LengthCopied;

    KeRaiseIrql (HIGH_LEVEL, &OldIrql);

    if (KdPrintCircularBuffer) {
        Length = String->Length;
        //
        // truncate ridiculous strings
        //
        if (Length > KDPRINTBUFFERSIZE) {
            Length = KDPRINTBUFFERSIZE;
        }

        if (KdPrintWritePointer + Length <= KdPrintCircularBuffer+KDPRINTBUFFERSIZE) {
            LengthCopied = KdpMoveMemory(KdPrintWritePointer, String->Buffer, Length);
            KdPrintWritePointer += LengthCopied;
            if (KdPrintWritePointer >= KdPrintCircularBuffer+KDPRINTBUFFERSIZE) {
                KdPrintWritePointer = KdPrintCircularBuffer;
                KdPrintRolloverCount++;
            }
        } else {
            ULONG First = (ULONG)(KdPrintCircularBuffer + KDPRINTBUFFERSIZE - KdPrintWritePointer);
            LengthCopied = KdpMoveMemory(KdPrintWritePointer,
                                         String->Buffer,
                                         First);
            if (LengthCopied == First) {
                LengthCopied += KdpMoveMemory(KdPrintCircularBuffer,
                                              String->Buffer + First,
                                              Length - First);
            }
            if (LengthCopied > First) {
                KdPrintWritePointer = KdPrintCircularBuffer + LengthCopied - First;
                KdPrintRolloverCount++;
            } else {
                KdPrintWritePointer += LengthCopied;
                if (KdPrintWritePointer >= KdPrintCircularBuffer+KDPRINTBUFFERSIZE) {
                    KdPrintWritePointer = KdPrintCircularBuffer;
                    KdPrintRolloverCount++;
                }
            }
        }
    }

    KeLowerIrql(OldIrql);
}
