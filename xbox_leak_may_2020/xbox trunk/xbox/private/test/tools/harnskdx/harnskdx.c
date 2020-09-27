/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    harnskdx.c

Abstract:

    This module is kernel debugger extension for harness. It contains useful
    commands for debugging Xbox test harness

Author:

    Sakphong Chanbai (schanbai) 5-Jul-2000

Environment:

    XBox

Revision History:

--*/


#include "precomp.h"
#include "loader.h"
#include "leak.h"
#include "xtestlib.h"
#include <stdio.h>
#include <xboxverp.h>
#include <ldr.h>
#include <heap.h>

WINDBG_EXTENSION_APIS ExtensionApis;
EXT_API_VERSION ApiVersion = { 5, 0, EXT_API_VERSION_NUMBER, 0 };

CONST CHAR * AllocationTypeString[] = {
    "malloc",
    "calloc",
    "realloc",
    "LocalAlloc",
    "GlobalAlloc",
    "HeapAlloc",
    "C++ new",
    "C++ new[]",
    "VirtualAlloc",
    "CreateEvent",
    "OpenMutex",
    "CreateSemaphore",
    "OpenSemaphore",
    "CreateWaitableTimer",
    "OpenWaitableTimer",
    "FindFirstFileA",
    "FindFirstFileW",
};

USHORT SavedMajorVersion;
USHORT SavedMinorVersion;
BOOL   ChkTarget;            // is debuggee a CHK build?

BOOL
GetData(
    LPVOID ptr,
    PVOID  dwAddress,
    LONG   size,
    PCSTR  type
    )
/*++

Routine Description:

    Get 'size' bytes from the debuggee program at 'dwAddress' and place it
    in our address space at 'ptr'.  Use 'type' in an error printout if necessary

Arguments:

Return Value:

--*/
{
    BOOL b;
    LONG BytesRead;
    LONG count = size;
    ULONG_PTR dwAddr = (ULONG_PTR)dwAddress;

    while( size > 0 ) {

        if (count >= 3000) {
            count = 3000;
        }

        b = ReadMemory(dwAddr, ptr, count<size ? count : size, &BytesRead );

        if (!b || BytesRead != (count<size ? count : size)) {
            dprintf( "Unable to read %u bytes at %X, for %s\n", count<size ? count : size, dwAddress, type );
            return FALSE;
        }

        dwAddr += count;
        size -= count;
        ptr = (LPVOID)((ULONG_PTR)ptr + count);
    }

    return TRUE;
}

BOOL
GetString(
    IN  LPSTR dwAddress,
    OUT LPSTR buf,
    IN  ULONG MaxChars
    )
{
    do {
        if( !GetData( buf, dwAddress, sizeof( *buf ), "Character" ) )
            return FALSE;

        dwAddress += sizeof( *buf );

    } while( (--MaxChars) != 0 && *buf++ != '\0' );

    return TRUE;
}

BOOL
GetStringFromLPSTR(
    LPVOID dwAddress,
    LPSTR buf,
    ULONG MaxChars
    )
{
    LPSTR lpAddress;
    *buf = 0;

    if ( !GetData( &lpAddress, dwAddress, sizeof(LPVOID), "LPVOID" ) ) {
        return FALSE;
    }

    return GetString( lpAddress, buf, MaxChars );
}

DECLARE_API( help )
{
    dprintf( "Harness kernel debugger extensions:\n" );
    dprintf( "\n" );
    dprintf( "  testlist    - dumps list of test modules\n" );
    dprintf( "  leakdump    - dumps allocation tracking\n" );
    dprintf( "  heaplist    - dumps all private heap created by test modules\n" );
    dprintf( "  heap        - dumps memory allocated from heap handle\n" );
    dprintf( "\n" );
}

DECLARE_API( testlist )
{
    PVOID HarnessTestListHead;
    LIST_ENTRY ListEntry;
    PLIST_ENTRY NextEntry;
    HARNESS_TEST_LIST_ENTRY ModuleEntry;
    PHARNESS_TEST_LIST_ENTRY p;
    ULONG ModuleCount = 0;
    ULONG Flags = 0;
    CHAR ListEntrySymbol[256];

    HarnessTestListHead = (PVOID)GetExpression( "harness!HarnessTestListHead" );

    sscanf( args, "%s %d", ListEntrySymbol, &Flags );

    if ( !HarnessTestListHead ) {
        HarnessTestListHead = (PVOID)GetExpression( ListEntrySymbol );
    }

    if ( !HarnessTestListHead ) {
        dprintf( "unable to resolve symbol \"HarnessTestListHead\"\n" );
        return;
    }

    if ( !GetData(&ListEntry, HarnessTestListHead, sizeof(LIST_ENTRY), "LIST_ENTRY") ) {
        return;
    }

    NextEntry = ListEntry.Flink;

    while ( NextEntry != HarnessTestListHead ) {

        if ( CheckControlC() ) {
            break;
        }

        p = CONTAINING_RECORD( NextEntry, HARNESS_TEST_LIST_ENTRY, List );

        if ( !GetData(&ModuleEntry, p, sizeof(HARNESS_TEST_LIST_ENTRY), "HARNESS_TEST_LIST_ENTRY") ) {
            break;
        }

        ModuleCount++;

        p = &ModuleEntry;
        dprintf( "Entry #%lu (%p): %s\n", ModuleCount, NextEntry, p->FullTestPath );

        if ( Flags & 1 ) {
            dprintf( "  FullTestPath           = %s\n",  p->FullTestPath );
            dprintf( "  ModuleHandle           = %p\n",  p->ModuleHandle );
            dprintf( "  FullLogPath            = %s\n",  p->FullLogPath );
            dprintf( "  LogLevel               = %d\n",  p->LogLevel );
            dprintf( "  LogOptions             = %d\n",  p->LogOptions );
            dprintf( "  LogHandle              = %p\n",  p->LogHandle );
            dprintf( "  Iterations             = %d\n",  p->Iterations );
            dprintf( "  NumberOfThreads        = %d\n",  p->NumberOfThreads );
            dprintf( "  ThreadStackSize        = %d\n",  p->ThreadStackSize );
            dprintf( "  SleepBetweenRun        = %d\n",  p->SleepBetweenRun );
            dprintf( "  StartTestEventHandle   = %p\n",  p->StartTestEventHandle );
            dprintf( "  StartTest address      = %p\n",  p->pStartupProc );
            dprintf( "  EndTestTest address    = %p\n",  p->pCleanupProc );
            dprintf( "  ShutdownTest address   = %p\n",  p->pShutdownProc );
            dprintf( "\n" );
        }

        NextEntry = p->List.Flink;
    }

    dprintf( "%lu module(s) found in the list\n\n", ModuleCount );
}

VOID
DumpOutstandingAllocation(
    AllocationType AllocType,
    ULONG Caller,
    ULONG Occurrences,
    ULONG TotalSize,
    PVOID *StackBackTrace OPTIONAL
    )
{
    CHAR CallerSymbol[256];
    ULONG Stacks[MAX_STACK_DEPTH];
    ULONG i, Displacement;
    BOOL b;

    GetSymbol( (PVOID)Caller, CallerSymbol, &Displacement );
    if ( !CallerSymbol[0] ) {
        sprintf( CallerSymbol, "0x%p", Caller );
    }

    dprintf(
        "\r%s allocated by %s, stack back trace %p, %lu occurence(s), total %lu bytes\n",
        AllocationTypeString[AllocType],
        CallerSymbol,
        StackBackTrace,
        Occurrences,
        TotalSize
        );

    // Display stack back trace
    b = GetData( Stacks, StackBackTrace, MAX_STACK_DEPTH*sizeof(ULONG_PTR), "ULONG_PTR" );

    for ( i=0; b && i<MAX_STACK_DEPTH && Stacks[i]; i++ ) {
        GetSymbol( (PVOID)Stacks[i], CallerSymbol, &Displacement );
        if ( CallerSymbol[0] ) {
            dprintf( "\t%s+0x%x\n", CallerSymbol, Displacement );
        } else {
            dprintf( "\t0x%x\n", Stacks[i] );
        }
    }

    dprintf( "\n" );
}

DECLARE_API( leakdump )
{
    PVOID HistoryDatabase;
    PVOID HistoryEntriesCountPtr;
    ULONG HistoryEntriesCount;
    LIST_ENTRY ListEntry;
    PLIST_ENTRY NextEntry;
    ALLOCATION_HISTORY LeakEntry;
    PALLOCATION_HISTORY p;
    ULONG StartEntry;
    ULONG Occurrences = 0;
    ULONG LastCaller = 0;
    ULONG LastSize = 0;
    AllocationType LastAllocType = 0;
    PVOID *LastBackTrace = NULL;

    sscanf( args, "%ld", &StartEntry );

    HistoryDatabase = (PVOID)GetExpression( "xtestlib!HistoryDatabase" );
    if ( !HistoryDatabase ) {
        dprintf( "unable to resolve symbol \"xtestlib!HistoryDatabase\"\n" );
        return;
    }

    HistoryEntriesCountPtr = (PVOID)GetExpression( "xtestlib!HistoryEntriesCount" );
    if ( !HistoryEntriesCountPtr ) {
        dprintf( "unable to resolve symbol \"xtestlib!HistoryEntriesCount\"\n" );
        return;
    }

    if ( !GetData(&ListEntry, HistoryDatabase, sizeof(LIST_ENTRY), "LIST_ENTRY") ) {
        return;
    }

    if ( !GetData(&HistoryEntriesCount, HistoryEntriesCountPtr, sizeof(ULONG), "ULONG") ) {
        return;
    }

    dprintf( "%lu allocation(s) found in the list\n\n", HistoryEntriesCount );

    if ( !HistoryEntriesCount ) {
        return;
    }

    NextEntry = ListEntry.Flink;
    dprintf( "Searching..." );

    while ( NextEntry != HistoryDatabase ) {

        if ( CheckControlC() ) {
            break;
        }

        p = CONTAINING_RECORD( NextEntry, ALLOCATION_HISTORY, List );

        if ( !GetData(&LeakEntry, p, sizeof(ALLOCATION_HISTORY), "ALLOCATION_HISTORY") ) {
            break;
        }

        p = &LeakEntry;

        if ( !LastCaller ) {
            Occurrences = 0;
            LastCaller = p->Caller;
            LastAllocType = p->AllocType;
            LastSize = p->Size;
            LastBackTrace = p->StackBackTrace;
        }

        if ( LastCaller != p->Caller ) {
            DumpOutstandingAllocation( LastAllocType, LastCaller, Occurrences, LastSize, LastBackTrace );
            LastAllocType = p->AllocType;
            LastCaller = p->Caller;
            Occurrences = 1;
            LastSize = p->Size;
            LastBackTrace = p->StackBackTrace;
            dprintf( "Searching..." );
        } else {
            Occurrences++;
            LastSize += p->Size;
        }

        NextEntry = p->List.Flink;
    }

    // Dump the last one
    DumpOutstandingAllocation( LastAllocType, LastCaller, Occurrences, LastSize, LastBackTrace );
}

VOID
DisplayVirtualAllocLists(
    PHEAP HeapPtr,
    SIZE_T *TotalCommittedSize
    )
{
    LIST_ENTRY ListEntry;
    PLIST_ENTRY NextEntry;
    HEAP_VIRTUAL_ALLOC_ENTRY HeapVirtualAllocEntry;
    PHEAP_VIRTUAL_ALLOC_ENTRY p;
    SIZE_T CommittedSize;

    if ( !GetData(&ListEntry, &HeapPtr->VirtualAllocdBlocks, sizeof(LIST_ENTRY), "LIST_ENTRY") ) {
        return;
    }

    NextEntry = ListEntry.Flink;
    while ( NextEntry != &HeapPtr->VirtualAllocdBlocks ) {
        if ( CheckControlC() ) {
            break;
        }

        p = CONTAINING_RECORD( NextEntry, HEAP_VIRTUAL_ALLOC_ENTRY, Entry );

        if ( !GetData(&HeapVirtualAllocEntry, p, sizeof(HEAP_VIRTUAL_ALLOC_ENTRY), "HEAP_VIRTUAL_ALLOC_ENTRY") ) {
            break;
        }

        p = &HeapVirtualAllocEntry;
        CommittedSize = p->CommitSize - p->BusyBlock.Size;
        *TotalCommittedSize += CommittedSize;

        dprintf(
            "  Virtual Alloc List: %p, bytes committed: %lu\n",
            NextEntry,
            CommittedSize
            );

        NextEntry = p->Entry.Flink;
    }
}


VOID
DisplayHeapSegments(
    PHEAP HeapPtr,
    SIZE_T *TotalBytesCommitted
    )
{
    ULONG i;
    HEAP Heap;
    HEAP_SEGMENT Segments[ HEAP_MAXIMUM_SEGMENTS ];
    SIZE_T SizeCommitted;

    if ( !GetData(&Heap, HeapPtr, sizeof(HEAP), "HEAP") ) {
        return;
    }

    for ( i=0; i<HEAP_MAXIMUM_SEGMENTS; i++ ) {

        if ( Heap.Segments[ i ] != NULL ) {

            if ( !GetData(&Segments[i], Heap.Segments[i], sizeof(HEAP_SEGMENT), "HEAP_SEGMENT") ) {
                continue;
            }

            SizeCommitted = (DWORD_PTR)(Segments[ i ].LastValidEntry) - \
                            (DWORD_PTR)(i == 0 ? (DWORD_PTR)HeapPtr : (DWORD_PTR)Heap.Segments[ i ]) - \
                            (Segments[ i ].NumberOfUnCommittedPages * PAGE_SIZE);

            dprintf(
                "  Segment at %p to %p, bytes committed: %lu\n",
                i == 0 ? (PVOID)HeapPtr : (PVOID)Heap.Segments[ i ],
                Segments[ i ].LastValidEntry,
                SizeCommitted
                );

            *TotalBytesCommitted += SizeCommitted;
        }
    }
}

DECLARE_API( heap )
{
    PHEAP HeapPtr;
    SIZE_T TotalBytesCommitted = 0;

    HeapPtr = (PHEAP)GetExpression( args );
    DisplayHeapSegments( HeapPtr, &TotalBytesCommitted );
    DisplayVirtualAllocLists( HeapPtr, &TotalBytesCommitted );
    dprintf( "\nTotal bytes committed: %ld\n", TotalBytesCommitted );
}

DECLARE_API( heaplist )
{
    PVOID HeapHandleDatabasePtr, HeapHandleDatabase;
    PRIVATE_HEAP_RECORD HeapRecord[MAX_HEAP_HANDLE_TRACKING];
    PPRIVATE_HEAP_RECORD HeapRecordPtr;
    LDR_CURRENT_IMAGE XeLdrCurrentImage;
    PLDR_CURRENT_IMAGE XeLdrCurrentImagePtr;
    XIMAGE_HEADER ImageHeader;
    ULONG i, NumberOfModules;
    PXIMAGE_MODULE_HEADER ModuleHeader;
    PHEAP HeapPtr;
    SIZE_T TotalBytesCommitted = 0;
    CHAR ModuleName[MAX_PATH];
    PVOID MiscData;

    HeapHandleDatabasePtr = (PVOID)GetExpression( "xtestlib!HeapHandleDatabase" );
    if ( !HeapHandleDatabasePtr ) {
        dprintf( "unable to resolve symbol \"xtestlib!HeapHandleDatabase\"\n" );
        return;
    }

    if ( !GetData(&HeapHandleDatabase, HeapHandleDatabasePtr, sizeof(PVOID), "PVOID") ) {
        return;
    }

    XeLdrCurrentImagePtr = (PLDR_CURRENT_IMAGE)GetExpression( "nt!XeLdrCurrentImage" );
    if ( !XeLdrCurrentImagePtr ) {
        dprintf( "unable to resolve symbol \"nt!XeLdrCurrentImage\"\n" );
        return;
    }

    if ( !GetData(&XeLdrCurrentImage, XeLdrCurrentImagePtr, sizeof(XeLdrCurrentImage), "LDR_CURRENT_IMAGE") ) {
        return;
    }

    if ( !GetData(&ImageHeader, XeLdrCurrentImage.ImageHeader, sizeof(ImageHeader), "XIMAGE_HEADER") ) {
        return;
    }

    if ( ImageHeader.Signature != XIMAGE_XBOX_SIGNATURE ) {
        dprintf( "Invalid XE image header\n" );
    }

    NumberOfModules = ImageHeader.OptionalHeader.NumberOfModules;
    ModuleHeader = (PXIMAGE_MODULE_HEADER)_alloca(NumberOfModules * sizeof(XIMAGE_MODULE_HEADER));
    if ( !ModuleHeader ) {
        dprintf( "unable to allocate memory for XIMAGE_MODULE_HEADER\n" );
    }

    if ( !GetData(
            ModuleHeader,
            XeLdrCurrentImage.FirstModuleHeader,
            NumberOfModules * sizeof(XIMAGE_MODULE_HEADER),
            "XIMAGE_MODULE_HEADER"
            ) ) {
        return;
    }

    if ( !GetData(
                HeapRecord,
                HeapHandleDatabase,
                sizeof(HeapRecord),
                "PRIVATE_HEAP_RECORD"
                ) ) {
        return;
    }

    MiscData = XIMAGE_MISCDATA( &ImageHeader, XeLdrCurrentImage.VarImageHeader );

    for ( i=0, HeapRecordPtr=HeapRecord; i<MAX_HEAP_HANDLE_TRACKING; i++, HeapRecordPtr++ ) {
        if ( HeapRecordPtr->HeapHandle ) {
            HeapPtr = (PHEAP)HeapRecordPtr->HeapHandle;

            ModuleName[0] = 0;
            GetString(
                (PCHAR)MiscData + ModuleHeader[HeapRecordPtr->OwnerModuleIndex].NameOfs,
                ModuleName,
                sizeof(ModuleName)
                );

            dprintf(
                "\nHeap handle: 0x%p (%s)\n",
                HeapRecordPtr->HeapHandle,
                ModuleName
                );
            DisplayHeapSegments( HeapRecordPtr->HeapHandle, &TotalBytesCommitted );
            DisplayVirtualAllocLists( HeapRecordPtr->HeapHandle, &TotalBytesCommitted );
        }
    }

    dprintf( "\nTotal bytes committed: %ld\n", TotalBytesCommitted );
}

VOID
WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT MajorVersion,
    USHORT MinorVersion
    )
{
    ExtensionApis = *lpExtensionApis;
    SavedMajorVersion = MajorVersion;
    SavedMinorVersion = MinorVersion;
    ChkTarget = SavedMajorVersion == 0x0c ? TRUE : FALSE;
}

DECLARE_API( version )
{
#if DBG
    PCSTR kind = "Checked";
#else
    PCSTR kind = "Free";
#endif

    dprintf(
        "%s harness extension dll for build %d debugging %s kernel for build %d\n",
        kind,
        VER_PRODUCTBUILD,
        SavedMajorVersion == 0x0c ? "checked" : "free",
        SavedMinorVersion
    );
}

VOID
CheckVersion(
    VOID
    )
{
#if DBG
    if ((SavedMajorVersion != 0x0c) || (SavedMinorVersion != VER_PRODUCTBUILD)) {
        dprintf("\r\n*** Extension DLL(%d Checked) does not match target system(%d %s)\r\n\r\n",
                VER_PRODUCTBUILD, SavedMinorVersion, (SavedMajorVersion==0x0f) ? "Free" : "Checked" );
    }
#else
    if ((SavedMajorVersion != 0x0f) || (SavedMinorVersion != VER_PRODUCTBUILD)) {
        dprintf("\r\n*** Extension DLL(%d Free) does not match target system(%d %s)\r\n\r\n",
                VER_PRODUCTBUILD, SavedMinorVersion, (SavedMajorVersion==0x0f) ? "Free" : "Checked" );
    }
#endif
}

LPEXT_API_VERSION
ExtensionApiVersion(
    VOID
    )
{
    return &ApiVersion;
}
