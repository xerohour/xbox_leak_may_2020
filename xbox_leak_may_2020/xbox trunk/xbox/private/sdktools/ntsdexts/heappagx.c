
#define DEBUG_PAGE_HEAP 1

#include "..\..\ntos\rtl\heappagi.h"

__inline
BOOLEAN
CheckInterrupted(
    VOID
    )
    {
    if ( CheckControlC() ) {
        dprintf( "\nInterrupted\n\n" );
        return TRUE;
        }
    return FALSE;
    }


__inline
PVOID
FetchRemotePVOID(
    PVOID Address
    )
    {
    PVOID RemoteValue = NULL;
    ReadMemory( Address, &RemoteValue, sizeof( PVOID ), NULL );
    return RemoteValue;
    }

__inline
ULONG
FetchRemoteULONG(
    PVOID Address
    )
    {
    ULONG RemoteValue = 0;
    ReadMemory( Address, &RemoteValue, sizeof( ULONG ), NULL );
    return RemoteValue;
    }


__inline
BOOL
WriteRemotePVOID(
    PVOID Address,
    PVOID Value
    )
    {
    return WriteMemory( Address, &Value, sizeof( PVOID ), NULL );
    }

__inline
BOOL
WriteRemoteULONG(
    PVOID Address,
    ULONG Value
    )
    {
    return WriteMemory( Address, &Value, sizeof( ULONG ), NULL );
    }



#define FETCH_REMOTE_FIELD_PTR( StructBase, StructType, FieldName ) \
            FetchRemotePVOID((PCHAR)(StructBase) + FIELD_OFFSET( StructType, FieldName ))

#define FETCH_REMOTE_FIELD_INT( StructBase, StructType, FieldName ) \
            FetchRemoteULONG((PCHAR)(StructBase) + FIELD_OFFSET( StructType, FieldName ))

#define WRITE_REMOTE_FIELD_PTR( StructBase, StructType, FieldName, Value ) \
            WriteRemotePVOID((PCHAR)(StructBase) + FIELD_OFFSET( StructType, FieldName ), (Value));

#define WRITE_REMOTE_FIELD_INT( StructBase, StructType, FieldName, Value ) \
            WriteRemoteULONG((PCHAR)(StructBase) + FIELD_OFFSET( StructType, FieldName ), (Value));

#define DUMP_REMOTE_FIELD_INT( DumpName, StructBase, StructType, FieldName ) \
            dprintf( "%s%08X\n", (DumpName), FETCH_REMOTE_FIELD_INT( StructBase, StructType, FieldName ))

#define DUMP_REMOTE_FIELD_PTR( DumpName, StructBase, StructType, FieldName ) \
            dprintf( "%s%p\n", (DumpName), FETCH_REMOTE_FIELD_PTR( StructBase, StructType, FieldName ))

VOID
DebugPageHeapLocateFaultAllocation(
    PVOID RemoteHeap,
    PVOID AddressOfFault
    );

VOID
DebugPageHeapReportAllocation(
    PVOID RemoteHeap,
    PVOID RemoteHeapNode,
    PCHAR NodeType,
    PVOID AddressOfFault
    );

BOOLEAN
DebugPageHeapExtensionShowHeapList(
    VOID
    );

#ifdef DPH_CAPTURE_STACK_TRACE

VOID
DebugPageHeapReportStackTrace(
    ULONG StackTraceLength,
    PVOID StackTraceBuffer[]
    );

ULONG
DebugPageHeapFetchRemoteStackTrace(
    PVOID RemoteStackNode,
    PVOID StackBuffer[]
    );

#endif // DPH_CAPTURE_STACK_TRACE


VOID
DebugPageHeapExtensionFind(
    PCSTR ArgumentString
    )
    {
    PVOID RemoteHeapList;
    PVOID RemoteHeap;
    PVOID RemoteVirtualNode;
    PVOID RemoteVirtualBase;
    ULONG RemoteVirtualSize;
    PVOID AddressOfFault;

    AddressOfFault = (PVOID) strtoul( ArgumentString, NULL, 16 );
    RemoteHeapList = (PVOID) GetExpression( "NTDLL!RtlpDebugPageHeapListHead" );
    RemoteHeap     = FetchRemotePVOID( RemoteHeapList );

    if ( RemoteHeap == NULL ) {
        dprintf( "\nNo debug page heaps active in process (or bad symbols)\n\n" );
        AddressOfFault = NULL;
        }

    if (( AddressOfFault == NULL ) || ( strchr( ArgumentString, '?' ))) {
        dprintf( "\nUsage: !dphfind <address>\n\n"
                 "       where <address> is typically the address of\n"
                 "       a fault attemping to reference heap memory\n"
                 "       from a debug page heap allocation\n\n"
               );
        return;
        }

    //
    //  Find the heap that contains the range of virtual addresses that
    //  contain the AddressOfFault.
    //

    for (;;) {

        //
        //  The heap header contains a linked list of virtual memory
        //  allocations.
        //

        RemoteVirtualNode = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pVirtualStorageListHead );

        while ( RemoteVirtualNode != NULL ) {

            RemoteVirtualBase = FETCH_REMOTE_FIELD_PTR( RemoteVirtualNode, DPH_HEAP_ALLOCATION, pVirtualBlock );
            RemoteVirtualSize = FETCH_REMOTE_FIELD_INT( RemoteVirtualNode, DPH_HEAP_ALLOCATION, nVirtualBlockSize );

            if (( RemoteVirtualBase == NULL ) || ( RemoteVirtualSize == 0 )) {
                dprintf( "\nPAGEHEAP: Heap 0x%p appears to have an invalid\n"
                         "          virtual allocation list\n\n",
                         RemoteHeap
                       );
                }

            if (((PCHAR) AddressOfFault >= ((PCHAR) RemoteVirtualBase )) &&
                ((PCHAR) AddressOfFault <= ((PCHAR) RemoteVirtualBase + RemoteVirtualSize ))) {

                //
                //  The fault appears to have occurred in the range of this
                //  heap, so we'll search the busy and free lists for the
                //  closest match and report it.  Then exit.
                //

                DebugPageHeapLocateFaultAllocation( RemoteHeap, AddressOfFault );
                return;
                }

            if ( CheckInterrupted() ) {
                return;
                }

            RemoteVirtualNode = FETCH_REMOTE_FIELD_PTR( RemoteVirtualNode, DPH_HEAP_ALLOCATION, pNextAlloc );

            }


        //
        //  Not found in this heap.  Continue with next heap or end
        //  of heap list.
        //

        if ( CheckInterrupted() ) {
            return;
            }

        RemoteHeap = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pNextHeapRoot );

        if ( RemoteHeap == NULL ) {
            dprintf( "\nPAGEHEAP: Could not find a debug heap containing\n"
                     "          the virtual address 0x%p\n\n",
                     AddressOfFault
                   );
            return;
            }
        }
    }


VOID
DebugPageHeapLocateFaultAllocation(
    PVOID RemoteHeap,
    PVOID AddressOfFault
    )
    {
    PVOID ClosestHeapNode;
    PVOID ClosestDifference;
    PVOID RemoteHeapNode;
    PVOID RemoteAllocBase;
    ULONG RemoteAllocSize;
    ULONG RemoteFreeListSize;

    ClosestHeapNode = NULL;

    //
    //  First search the busy list for the containing allocation, if any.
    //

    RemoteHeapNode = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pBusyAllocationListHead );

    while ( RemoteHeapNode != NULL ) {

        RemoteAllocBase = FETCH_REMOTE_FIELD_PTR( RemoteHeapNode, DPH_HEAP_ALLOCATION, pVirtualBlock );
        RemoteAllocSize = FETCH_REMOTE_FIELD_INT( RemoteHeapNode, DPH_HEAP_ALLOCATION, nVirtualBlockSize );

        if (((PCHAR) AddressOfFault >= ((PCHAR) RemoteAllocBase )) &&
            ((PCHAR) AddressOfFault <  ((PCHAR) RemoteAllocBase + RemoteAllocSize ))) {

            //
            //  The fault appears to have occurred in this allocation's
            //  memory (which includes the NO_ACCESS page beyond the user
            //  portion of the allocation).
            //

            DebugPageHeapReportAllocation( RemoteHeap, RemoteHeapNode, "allocated", AddressOfFault );
            return;
            }

        if ( CheckInterrupted() ) {
            return;
            }

        RemoteHeapNode = FETCH_REMOTE_FIELD_PTR( RemoteHeapNode, DPH_HEAP_ALLOCATION, pNextAlloc );
        }

    //
    //  Failed to find containing allocation on busy list, so search free.
    //

    RemoteHeapNode = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pFreeAllocationListHead );

    while ( RemoteHeapNode != NULL ) {

        RemoteAllocBase = FETCH_REMOTE_FIELD_PTR( RemoteHeapNode, DPH_HEAP_ALLOCATION, pVirtualBlock );
        RemoteAllocSize = FETCH_REMOTE_FIELD_INT( RemoteHeapNode, DPH_HEAP_ALLOCATION, nVirtualBlockSize );

        if (((PCHAR) AddressOfFault >= ((PCHAR) RemoteAllocBase )) &&
            ((PCHAR) AddressOfFault <  ((PCHAR) RemoteAllocBase + RemoteAllocSize ))) {

            //
            //  The fault appears to have occurred in this freed alloc's
            //  memory.
            //

            DebugPageHeapReportAllocation( RemoteHeap, RemoteHeapNode, "freed", AddressOfFault );
            return;
            }

        if ( CheckInterrupted() ) {
            return;
            }

        RemoteHeapNode = FETCH_REMOTE_FIELD_PTR( RemoteHeapNode, DPH_HEAP_ALLOCATION, pNextAlloc );
        }

    //
    //  Failed to find containing allocation in free list, but we wouldn't
    //  have gotten this far if the debug heap did not contain the virtual
    //  address range of the fault.  So, report it as a wild pointer that
    //  could have been freed memory.
    //

    RemoteFreeListSize = FETCH_REMOTE_FIELD_INT( RemoteHeap, DPH_HEAP_ROOT, nFreeAllocations );

    dprintf( "\nPAGEHEAP: %p references memory contained in the heap %p,\n"
               "          but does not reference an existing allocated or\n"
               "          recently freed heap block.  It is possible that\n"
               "          the memory at %p could previously have been\n"
               "          allocated and freed, but it must have been freed\n"
               "          prior to the most recent %d frees.\n\n",
             AddressOfFault,
             RemoteHeap,
             AddressOfFault,
             RemoteFreeListSize
           );

    }


VOID
DebugPageHeapReportAllocation(
    PVOID RemoteHeap,
    PVOID RemoteHeapNode,
    PCHAR NodeType,
    PVOID AddressOfFault
    )
    {
    PVOID RemoteUserBase;
    ULONG RemoteUserSize;
    PVOID EndOfBlock;
    SIZE_T PastTheBlock;
    SIZE_T BeforeTheBlock;

    RemoteUserBase = FETCH_REMOTE_FIELD_PTR( RemoteHeapNode, DPH_HEAP_ALLOCATION, pUserAllocation );
    RemoteUserSize = FETCH_REMOTE_FIELD_INT( RemoteHeapNode, DPH_HEAP_ALLOCATION, nUserRequestedSize );

    EndOfBlock = (PCHAR) RemoteUserBase + RemoteUserSize - 1;

    if ( AddressOfFault > EndOfBlock ) {

        PastTheBlock = (PCHAR) AddressOfFault - (PCHAR) EndOfBlock;

        dprintf( "\nPAGEHEAP: %p is 0x%x bytes beyond the end of %s heap block at\n"
                   "          %p of 0x%x bytes",
                 AddressOfFault,
                 PastTheBlock,
                 NodeType,
                 RemoteUserBase,
                 RemoteUserSize
               );

        }

    else if ( AddressOfFault >= RemoteUserBase ) {

        dprintf( "\nPAGEHEAP: %p references %s heap block at\n"
                   "          %p of 0x%x bytes",
                 AddressOfFault,
                 NodeType,
                 RemoteUserBase,
                 RemoteUserSize
               );

        }

    else {

        BeforeTheBlock = (PCHAR) RemoteUserBase - (PCHAR) AddressOfFault;

        dprintf( "\nPAGEHEAP: %p is 0x%x bytes before the %s heap block at\n"
                   "          %p of 0x%x bytes",
                 AddressOfFault,
                 BeforeTheBlock,
                 NodeType,
                 RemoteUserBase,
                 RemoteUserSize
               );

        }

#ifdef DPH_CAPTURE_STACK_TRACE

    {
    PVOID RemoteStackTraceBuffer[ DPH_MAX_STACK_LENGTH ];
    ULONG RemoteStackTraceLength;
    PVOID RemoteStackNode;

    RemoteStackNode = FETCH_REMOTE_FIELD_PTR( RemoteHeapNode, DPH_HEAP_ALLOCATION, pStackTrace );

    RemoteStackTraceLength = DebugPageHeapFetchRemoteStackTrace(
                                 RemoteStackNode,
                                 RemoteStackTraceBuffer
                                 );

    if ( RemoteStackTraceLength ) {

        dprintf( ", %s by:\n\n", NodeType );

        DebugPageHeapReportStackTrace(
            RemoteStackTraceLength,
            RemoteStackTraceBuffer
            );

        }

    else {
        dprintf( "(stack trace not available)\n\n" );
        }
    }

#else  // ! DPH_CAPTURE_STACK_TRACE

    dprintf( "\n\n" );

#endif // DPH_CAPTURE_STACK_TRACE

    }


#ifdef DPH_CAPTURE_STACK_TRACE

VOID
DebugPageHeapReportStackTrace(
    ULONG StackTraceLength,
    PVOID StackTraceBuffer[]
    )
    {
    CHAR  SymbolName[ 1024 ];
    ULONG_PTR Displacement;
    ULONG i;

    for ( i = 0; i < StackTraceLength; i++ ) {

        GetSymbol( StackTraceBuffer[ i ], SymbolName, &Displacement );
        GetSymbol( StackTraceBuffer[ i ], SymbolName, &Displacement );

        dprintf( "          %p %s+0x%p\n",
                 StackTraceBuffer[ i ],
                 SymbolName,
                 Displacement
               );

        }

    dprintf( "\n" );

    }


ULONG
DebugPageHeapFetchRemoteStackTrace(
    PVOID RemoteStackNode,
    PVOID StackBuffer[]
    )
    {
    ULONG RemoteStackLength;

    if ( RemoteStackNode == NULL )
        return 0;

    RemoteStackLength = FETCH_REMOTE_FIELD_INT( RemoteStackNode, DPH_STACK_TRACE_NODE, Length );

    if ( RemoteStackLength > 0 ) {

        ZeroMemory( StackBuffer, RemoteStackLength * sizeof( PVOID ));

        ReadMemory(
            (PCHAR) RemoteStackNode + FIELD_OFFSET( DPH_STACK_TRACE_NODE, Address ),
            StackBuffer,
            RemoteStackLength * sizeof( PVOID ),
            NULL
            );

        while (( RemoteStackLength > 0 ) && ( StackBuffer[ RemoteStackLength ] == NULL ))
            --RemoteStackLength;

        }

    return RemoteStackLength;
    }

#endif // DPH_CAPTURE_STACK_TRACE


#define FORMAT_TYPE_BUSY_LIST 0
#define FORMAT_TYPE_FREE_LIST 1
#define FORMAT_TYPE_VIRT_LIST 2

BOOLEAN
DebugPageHeapDumpThisList(
    PVOID RemoteList,
    PCH   ListName,
    ULONG FormatType
    )
    {
    PVOID RemoteNode = RemoteList;
    PVOID RemoteBase;
    ULONG RemoteSize;
    PVOID RemoteUser;
    ULONG RemoteUsiz;
    ULONG RemoteFlag;
    PVOID RemoteValu;

    dprintf( "\n%s:\n", ListName );

    switch ( FormatType ) {
        case FORMAT_TYPE_BUSY_LIST:
            dprintf( "UserAddr  UserSize  VirtAddr  VirtSize  UserFlag  UserValu\n" );
            break;
        case FORMAT_TYPE_FREE_LIST:
            dprintf( "UserAddr  UserSize  VirtAddr  VirtSize\n" );
            break;
        }

    while ( RemoteNode ) {

        RemoteBase = FETCH_REMOTE_FIELD_PTR( RemoteNode, DPH_HEAP_ALLOCATION, pVirtualBlock );
        RemoteSize = FETCH_REMOTE_FIELD_INT( RemoteNode, DPH_HEAP_ALLOCATION, nVirtualBlockSize );
        RemoteUser = FETCH_REMOTE_FIELD_PTR( RemoteNode, DPH_HEAP_ALLOCATION, pUserAllocation );
        RemoteUsiz = FETCH_REMOTE_FIELD_INT( RemoteNode, DPH_HEAP_ALLOCATION, nUserRequestedSize );
        RemoteFlag = FETCH_REMOTE_FIELD_INT( RemoteNode, DPH_HEAP_ALLOCATION, UserFlags );
        RemoteValu = FETCH_REMOTE_FIELD_PTR( RemoteNode, DPH_HEAP_ALLOCATION, UserValue );
        RemoteNode = FETCH_REMOTE_FIELD_PTR( RemoteNode, DPH_HEAP_ALLOCATION, pNextAlloc );

        switch ( FormatType ) {

            case FORMAT_TYPE_BUSY_LIST:

                dprintf(( RemoteFlag || RemoteValu ) ?
                         "%08X  %08X  %08X  %08X  %08X  %08X\n" :
                         "%08X  %08X  %08X  %08X\n",
                         RemoteUser,
                         RemoteUsiz,
                         RemoteBase,
                         RemoteSize,
                         RemoteFlag,
                         RemoteValu
                       );
                break;

            case FORMAT_TYPE_FREE_LIST:

                dprintf( "%p  %08X  %p  %08X\n",
                         RemoteUser,
                         RemoteUsiz,
                         RemoteBase,
                         RemoteSize
                       );
                break;

            case FORMAT_TYPE_VIRT_LIST:

                dprintf( "%p - %p (%08X)\n",
                         RemoteBase,
                         (PCH)RemoteBase + RemoteSize,
                         RemoteSize
                       );
                break;

            }

        if ( CheckInterrupted() ) {
            return FALSE;
            }
        }

    return TRUE;
    }


BOOLEAN
DebugPageHeapDumpThisHeap(
    PVOID RemoteHeap
    )
    {
    PVOID RemoteNode;

    dprintf( "\nDPH Heap at %p:\n\n", RemoteHeap );

    DUMP_REMOTE_FIELD_INT( "Signature:     ", RemoteHeap, DPH_HEAP_ROOT, Signature );
    DUMP_REMOTE_FIELD_INT( "HeapFlags:     ", RemoteHeap, DPH_HEAP_ROOT, HeapFlags );
    DUMP_REMOTE_FIELD_INT( "VirtualRanges: ", RemoteHeap, DPH_HEAP_ROOT, nVirtualStorageRanges );
    DUMP_REMOTE_FIELD_PTR( "VirtualCommit: ", RemoteHeap, DPH_HEAP_ROOT, nVirtualStorageBytes );
    DUMP_REMOTE_FIELD_INT( "BusyAllocs:    ", RemoteHeap, DPH_HEAP_ROOT, nBusyAllocations );
    DUMP_REMOTE_FIELD_PTR( "BusyVirtual:   ", RemoteHeap, DPH_HEAP_ROOT, nBusyAllocationBytesCommitted );
    DUMP_REMOTE_FIELD_PTR( "BusyReadWrite: ", RemoteHeap, DPH_HEAP_ROOT, nBusyAllocationBytesAccessible );
    DUMP_REMOTE_FIELD_INT( "FreeAllocs:    ", RemoteHeap, DPH_HEAP_ROOT, nFreeAllocations );
    DUMP_REMOTE_FIELD_PTR( "FreeVirtual:   ", RemoteHeap, DPH_HEAP_ROOT, nFreeAllocationBytesCommitted );
    DUMP_REMOTE_FIELD_INT( "AvailAllocs:   ", RemoteHeap, DPH_HEAP_ROOT, nAvailableAllocations );
    DUMP_REMOTE_FIELD_PTR( "AvailVirtual:  ", RemoteHeap, DPH_HEAP_ROOT, nAvailableAllocationBytesCommitted );
    DUMP_REMOTE_FIELD_INT( "NodePools:     ", RemoteHeap, DPH_HEAP_ROOT, nNodePools );
    DUMP_REMOTE_FIELD_PTR( "NodeVirtual:   ", RemoteHeap, DPH_HEAP_ROOT, nNodePoolBytes );
    DUMP_REMOTE_FIELD_INT( "AvailNodes:    ", RemoteHeap, DPH_HEAP_ROOT, nUnusedNodes );
    DUMP_REMOTE_FIELD_INT( "ExtraFlags:    ", RemoteHeap, DPH_HEAP_ROOT, ExtraFlags );
    DUMP_REMOTE_FIELD_INT( "NormalHeap:    ", RemoteHeap, DPH_HEAP_ROOT, NormalHeap );
    DUMP_REMOTE_FIELD_INT( "Seed:          ", RemoteHeap, DPH_HEAP_ROOT, Seed );
    DUMP_REMOTE_FIELD_INT( "Counter[0]:    ", RemoteHeap, DPH_HEAP_ROOT, Counter[0] );
    DUMP_REMOTE_FIELD_INT( "Counter[1]:    ", RemoteHeap, DPH_HEAP_ROOT, Counter[1] );
    DUMP_REMOTE_FIELD_INT( "Counter[2]:    ", RemoteHeap, DPH_HEAP_ROOT, Counter[2] );
    DUMP_REMOTE_FIELD_INT( "Counter[3]:    ", RemoteHeap, DPH_HEAP_ROOT, Counter[3] );
    DUMP_REMOTE_FIELD_INT( "Counter[4]:    ", RemoteHeap, DPH_HEAP_ROOT, Counter[4] );

#ifdef DPH_CAPTURE_STACK_TRACE

    {
    PVOID RemoteStackTraceBuffer[ DPH_MAX_STACK_LENGTH ];
    ULONG RemoteStackTraceLength;
    PVOID RemoteStackNode;

    DUMP_REMOTE_FIELD_INT( "StackTraces:   ", RemoteHeap, DPH_HEAP_ROOT, nStackTraceBNodes );
    DUMP_REMOTE_FIELD_INT( "StackBytes:    ", RemoteHeap, DPH_HEAP_ROOT, nStackTraceBytesCommitted );
    DUMP_REMOTE_FIELD_INT( "StackAvail:    ", RemoteHeap, DPH_HEAP_ROOT, nStackTraceStorage );
    DUMP_REMOTE_FIELD_INT( "StackWasted:   ", RemoteHeap, DPH_HEAP_ROOT, nStackTraceBytesWasted );
    DUMP_REMOTE_FIELD_INT( "StackBDepth:   ", RemoteHeap, DPH_HEAP_ROOT, nStackTraceBDepth );
    DUMP_REMOTE_FIELD_INT( "StackBCollide: ", RemoteHeap, DPH_HEAP_ROOT, nStackTraceBHashCollisions );

    RemoteStackNode = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pStackTraceCreator );

    RemoteStackTraceLength = DebugPageHeapFetchRemoteStackTrace(
                                 RemoteStackNode,
                                 RemoteStackTraceBuffer
                                 );

    if ( RemoteStackTraceLength ) {

        dprintf( "\nHeap created by:\n\n" );

        DebugPageHeapReportStackTrace(
            RemoteStackTraceLength,
            RemoteStackTraceBuffer
            );

        }
    }

#endif // DPH_CAPTURE_STACK_TRACE

    if ( ! DebugPageHeapDumpThisList(
                FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pVirtualStorageListHead ),
                "VirtualList",
                FORMAT_TYPE_VIRT_LIST )) {
        return FALSE;
        }

    if ( ! DebugPageHeapDumpThisList(
                FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pNodePoolListHead ),
                "NodePoolList",
                FORMAT_TYPE_VIRT_LIST )) {
        return FALSE;
        }

    if ( ! DebugPageHeapDumpThisList(
                FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pAvailableAllocationListHead ),
                "AvailableList",
                FORMAT_TYPE_VIRT_LIST )) {
        return FALSE;
        }

    if ( ! DebugPageHeapDumpThisList(
                FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pFreeAllocationListHead ),
                "FreeList",
                FORMAT_TYPE_FREE_LIST )) {
        return FALSE;
        }

    if ( ! DebugPageHeapDumpThisList(
                FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pBusyAllocationListHead ),
                "BusyList",
                FORMAT_TYPE_BUSY_LIST )) {
        return FALSE;
        }

    dprintf( "\n" );
    return TRUE;
    }



VOID
DebugPageHeapExtensionDump(
    PCSTR ArgumentString
    )
    {
    PVOID   RemoteHeapList;
    PVOID   RemoteHeap;
    PVOID   RemoteHeapToDump;
    BOOLEAN AnyDumps = FALSE;

    RemoteHeapToDump = (PVOID) strtoul( ArgumentString, NULL, 16 );
    RemoteHeapList   = (PVOID) GetExpression( "NTDLL!RtlpDebugPageHeapListHead" );
    RemoteHeap       = FetchRemotePVOID( RemoteHeapList );

    if (( RemoteHeap       == NULL ) ||
        ( RemoteHeapToDump == NULL ) ||
        ( strchr( ArgumentString, '?' ))) {

        dprintf( "\nUsage: !dphdump <address>\n\n"
                 "       where <address> is the heap base address or heap handle\n\n"
               );

        DebugPageHeapExtensionShowHeapList();
        return;
        }

    while ( RemoteHeap != NULL ) {

        if ((((LONG_PTR)RemoteHeapToDump & 0xFFFF0000 ) == ((LONG_PTR)RemoteHeap & 0xFFFF0000 )) ||
            ((LONG_PTR)RemoteHeapToDump == -1 )) {

            AnyDumps = TRUE;

            if ( ! DebugPageHeapDumpThisHeap( RemoteHeap ))
                return;

            }

        if ( CheckInterrupted() ) {
            return;
            }

        RemoteHeap = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pNextHeapRoot );
        }

    if ( ! AnyDumps ) {
        dprintf( "\nDebug page heap \"0x%p\" not found in process\n\n", RemoteHeapToDump );
        DebugPageHeapExtensionShowHeapList();
        }

    }



BOOLEAN
DebugPageHeapExtensionShowHeapList(
    VOID
    )
    {
    PVOID RemoteHeapList = (PVOID)GetExpression( "NTDLL!RtlpDebugPageHeapListHead" );
    PVOID RemoteHeap     = FetchRemotePVOID( RemoteHeapList );

    if ( RemoteHeap == NULL ) {
        dprintf( "\nNo debug page heaps active in process (or bad symbols)\n" );
        return FALSE;
        }
    else {
        dprintf( "\nDebug page heaps active in process:\n\n" );
        do  {
            dprintf( "0x%p\n", RemoteHeap );
            RemoteHeap = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pNextHeapRoot );
            }
        while ( RemoteHeap );
        dprintf( "\n" );
        return TRUE;
        }
    }


#ifdef DPH_CAPTURE_STACK_TRACE

typedef struct _LOCAL_STACK_NODE LOCAL_STACK_NODE, *PLOCAL_STACK_NODE;

struct _LOCAL_STACK_NODE {

    PLOCAL_STACK_NODE Left;
    PLOCAL_STACK_NODE Right;

    PVOID RemoteStackNode;
    ULONG RemoteBusyCount;
    ULONG RemoteBusyBytes;
    };


BOOLEAN DebugPageHeapSortByCount;


BOOLEAN
DebugPageHeapReportStackNode(
    PLOCAL_STACK_NODE LocalStackNode
    )
    {
    PVOID RemoteStackTraceBuffer[ DPH_MAX_STACK_LENGTH ];
    ULONG RemoteStackTraceLength;

    RemoteStackTraceLength = DebugPageHeapFetchRemoteStackTrace(
                                 LocalStackNode->RemoteStackNode,
                                 RemoteStackTraceBuffer
                                 );

    if ( RemoteStackTraceLength ) {

        ULONG Count = LocalStackNode->RemoteBusyCount;
        ULONG Bytes = LocalStackNode->RemoteBusyBytes;
        ULONG Average;

        if ( Count > 0 ) {

            Average = ( Bytes + ( Count / 2 )) / Count;

            if ( Count > 1 ) {

                dprintf(
                    "%7d bytes in %d allocations (avg %d bytes/alloc):\n\n",
                    Bytes,
                    Count,
                    Average
                    );

                }

            else {

                dprintf(
                    "%7d bytes in 1 allocation:\n\n",
                    Bytes,
                    Average
                    );

                }

            DebugPageHeapReportStackTrace(
                RemoteStackTraceLength,
                RemoteStackTraceBuffer
                );

            dprintf( "\n" );

            }
        }

    else {

        dprintf( "Error retrieving remote stack trace\n\n" );

        }

    return TRUE;
    }


BOOLEAN
DebugPageHeapReportStackTree(
    PLOCAL_STACK_NODE StackTree
    )
    {

    if ( CheckInterrupted() )
        return FALSE;

    if ( StackTree->Left )
        if ( ! DebugPageHeapReportStackTree( StackTree->Left ))
            return FALSE;

    if ( StackTree->RemoteStackNode )
        if ( ! DebugPageHeapReportStackNode( StackTree ))
            return FALSE;

    if ( StackTree->Right )
        if ( ! DebugPageHeapReportStackTree( StackTree->Right ))
            return FALSE;

    return TRUE;
    }


INT
DebugPageHeapCompareStackNodeToValues(
    PLOCAL_STACK_NODE Node,
    PVOID             RemoteStackNode,
    ULONG             RemoteBusyCount,
    ULONG             RemoteBusyBytes
    )
    {
    INT Difference;

    if ( DebugPageHeapSortByCount ) {

        Difference = ( Node->RemoteBusyCount - RemoteBusyCount );

        if ( Difference == 0 )
             Difference = ( Node->RemoteBusyBytes - RemoteBusyBytes );

        }

    else {

        Difference = ( Node->RemoteBusyBytes - RemoteBusyBytes );

        if ( Difference == 0 )
             Difference = ( Node->RemoteBusyCount - RemoteBusyCount );

        }

    if ( Difference == 0 )
         Difference = PtrToInt ((PVOID)((SIZE_T)Node->RemoteStackNode - (SIZE_T)RemoteStackNode ));

    return Difference;
    }


PLOCAL_STACK_NODE
DebugPageHeapFindOrInsertStackNodeInBtree(
    PLOCAL_STACK_NODE Node,
    PVOID             RemoteStackNode,
    ULONG             RemoteBusyCount,
    ULONG             RemoteBusyBytes
    )
    {
    PLOCAL_STACK_NODE NewNode;
    INT Difference;

    for (;;) {

        Difference = DebugPageHeapCompareStackNodeToValues(
                         Node,
                         RemoteStackNode,
                         RemoteBusyCount,
                         RemoteBusyBytes
                         );

        if ( Difference < 0 ) {         // go left
            if ( Node->Left )
                Node = Node->Left;
            else
                break;
            }
        else if ( Difference > 0 ) {    // go right
            if ( Node->Right )
                Node = Node->Right;
            else
                break;
            }
        else {                          // equal
            return Node;
            }
        }

    NewNode = HeapAlloc( GetProcessHeap(), 0, sizeof( LOCAL_STACK_NODE ));

    if ( NewNode != NULL ) {

        NewNode->Left            = NULL;
        NewNode->Right           = NULL;
        NewNode->RemoteStackNode = RemoteStackNode;
        NewNode->RemoteBusyCount = RemoteBusyCount;
        NewNode->RemoteBusyBytes = RemoteBusyBytes;

        if ( Difference < 0 )
            Node->Left  = NewNode;
        else
            Node->Right = NewNode;

        }

    return NewNode;
    }


VOID
DebugPageHeapFreeBtree(
    PLOCAL_STACK_NODE Tree
    )
    {
    if ( Tree->Left )
        DebugPageHeapFreeBtree( Tree->Left );

    if ( Tree->Right )
        DebugPageHeapFreeBtree( Tree->Right );

    HeapFree( GetProcessHeap(), 0, Tree );
    }


BOOLEAN
DebugPageHeapDumpThisHeapHogs(
    PVOID RemoteHeap
    )
    {
    ULONG RemoteBusyCount;
    ULONG RemoteBusyBytes;
    PVOID RemoteStackNode;
    PVOID RemoteBusyNode;
    PLOCAL_STACK_NODE TreeRoot;
    PVOID LocalNode;
    BOOLEAN Success;

    dprintf( "\nDPH Heap at %p, hogs:\n\n", RemoteHeap );

    //
    //  Walk busy list and build sorted list of stack traces
    //  by total busy size, descending.  Note that we expect
    //  multiple references to the same stack trace in the
    //  busy list.  Would be nice to verify that busy counts
    //  in stack trace nodes agree with number found in busy
    //  list.
    //

    TreeRoot = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof( LOCAL_STACK_NODE ));

    if ( TreeRoot == NULL )
        return FALSE;

    RemoteBusyNode = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pBusyAllocationListHead );

    while ( RemoteBusyNode ) {

        RemoteStackNode = FETCH_REMOTE_FIELD_PTR( RemoteBusyNode,  DPH_HEAP_ALLOCATION,  pStackTrace );
        RemoteBusyCount = FETCH_REMOTE_FIELD_INT( RemoteStackNode, DPH_STACK_TRACE_NODE, BusyCount );
        RemoteBusyBytes = FETCH_REMOTE_FIELD_INT( RemoteStackNode, DPH_STACK_TRACE_NODE, BusyBytes );

        LocalNode = DebugPageHeapFindOrInsertStackNodeInBtree(
                        TreeRoot,
                        RemoteStackNode,
                        RemoteBusyCount,
                        RemoteBusyBytes
                        );

        if (( ! LocalNode ) || ( CheckInterrupted() )) {
            DebugPageHeapFreeBtree( TreeRoot );
            return FALSE;
            }

        RemoteBusyNode = FETCH_REMOTE_FIELD_PTR( RemoteBusyNode, DPH_HEAP_ALLOCATION, pNextAlloc );

        }

    //
    //  Now walk btree left to right and dump traces.  Empty tree root
    //  has NULL for RemoteStackNode, so skip that case.
    //

    Success = DebugPageHeapReportStackTree( TreeRoot );

    dprintf( "\n" );
    DebugPageHeapFreeBtree( TreeRoot );
    return Success;
    }


BOOLEAN
DebugPageHeapResetStackNode(
    PVOID RemoteStackNode
    )
    {
    PVOID Left, Right;

    if ( CheckInterrupted() )
        return FALSE;

    WRITE_REMOTE_FIELD_PTR( RemoteStackNode, DPH_STACK_TRACE_NODE, BusyCount, NULL );
    WRITE_REMOTE_FIELD_PTR( RemoteStackNode, DPH_STACK_TRACE_NODE, BusyBytes, NULL );

    Left  = FETCH_REMOTE_FIELD_PTR( RemoteStackNode, DPH_STACK_TRACE_NODE, Left  );
    Right = FETCH_REMOTE_FIELD_PTR( RemoteStackNode, DPH_STACK_TRACE_NODE, Right );

    if ( Left )
        if ( ! DebugPageHeapResetStackNode( Left ))
            return FALSE;

    if ( Right )
        if ( ! DebugPageHeapResetStackNode( Right ))
            return FALSE;

    return TRUE;
    }


BOOLEAN
DebugPageHeapResetHeapHogs(
    PVOID RemoteHeap
    )
    {
    PVOID RemoteStackNode;

    dprintf( "\nDPH Heap at %p, reset hogs to zero...", RemoteHeap );

    //
    //  Walk remote stack trace btree, reset all counts to zero.
    //

    RemoteStackNode = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pStackTraceRoot );

    if ( RemoteStackNode != NULL ) {
        if ( ! DebugPageHeapResetStackNode( RemoteStackNode ))
            return FALSE;
        }

    dprintf( "done\n\n" );
    return TRUE;
    }


BOOLEAN
DebugPageHeapExtensionShowHogs(
    PCSTR ArgumentString
    )
    {
    PVOID   RemoteHeapList;
    PVOID   RemoteHeap;
    PVOID   RemoteHeapToDump;
    BOOLEAN AnyDumps = FALSE;
    BOOLEAN Success;

    RemoteHeapToDump = (PVOID) strtoul( ArgumentString, NULL, 16 );
    RemoteHeapList   = (PVOID) GetExpression( "NTDLL!RtlpDebugPageHeapListHead" );
    RemoteHeap       = FetchRemotePVOID( RemoteHeapList );

    if (( RemoteHeap       == NULL ) ||
        ( RemoteHeapToDump == NULL ) ||
        ( strchr( ArgumentString, '?' ))) {

#ifndef DPH_EXTENSION_BUILT_AS_SEPARATE_PROCESS

        dprintf( "\nUsage: !dphhogs <address> [count] [reset]\n\n"
                 "       where <address> is the heap base address or heap handle,\n"
                 "       [count] changes sort order to count versus bytes, or\n"
                 "       [reset] will force all allocation counts to zero.\n\n"
               );

#endif // DPH_EXTENSION_BUILT_AS_SEPARATE_PROCESS

        return DebugPageHeapExtensionShowHeapList();
        }

    if ( strstr( ArgumentString, "count" ))
        DebugPageHeapSortByCount = TRUE;
    else
        DebugPageHeapSortByCount = FALSE;

    while ( RemoteHeap != NULL ) {

        if ((((SIZE_T)RemoteHeapToDump & 0xFFFF0000 ) == ((SIZE_T)RemoteHeap & 0xFFFF0000 )) ||
            ((SIZE_T)RemoteHeapToDump == 0xFFFFFFFF )) {

            Success = FALSE;

#ifdef DPH_EXTENSION_BUILT_AS_SEPARATE_PROCESS

            if ( DebugPageHeapExtensionLockRemoteHeap( RemoteHeap )) {

#endif // DPH_EXTENSION_BUILT_AS_SEPARATE_PROCESS

                if ( strstr( ArgumentString, "reset" )) {
                    Success = DebugPageHeapResetHeapHogs( RemoteHeap );
                    }
                else {
                    Success = DebugPageHeapDumpThisHeapHogs( RemoteHeap );
                    }

#ifdef DPH_EXTENSION_BUILT_AS_SEPARATE_PROCESS

                DebugPageHeapExtensionUnlockRemoteHeap( RemoteHeap );

                }

#endif // DPH_EXTENSION_BUILT_AS_SEPARATE_PROCESS

            if ( ! Success ) {
                return FALSE;
                }
            else {
                AnyDumps = TRUE;
                }

            }

        if ( CheckInterrupted() ) {
            return FALSE;
            }

        RemoteHeap = FETCH_REMOTE_FIELD_PTR( RemoteHeap, DPH_HEAP_ROOT, pNextHeapRoot );
        }

    if ( ! AnyDumps ) {
        dprintf( "\nDebug page heap \"0x%p\" not found in process\n\n", RemoteHeapToDump );
        DebugPageHeapExtensionShowHeapList();
        return FALSE;
        }

    return TRUE;
    }


#endif // DPH_CAPTURE_STACK_TRACE


VOID
DebugPageHeapExtensionHogs(
    PCSTR ArgumentString
    )
    {
#ifdef DPH_CAPTURE_STACK_TRACE
    DebugPageHeapExtensionShowHogs( ArgumentString );
#else
    dprintf( "\n!dphhogs only available on x86 checked builds\n\n" );
#endif
    }

VOID
DebugPageHeapExtensionFlags(
    PCSTR ArgumentString
    )
{
    PVOID   RemoteHeapList;
    PVOID   RemoteHeap;
    PVOID   RemoteHeapToDump;
    BOOLEAN AnyDumps = FALSE;

    BOOLEAN Success;

    PVOID FlagsAddress;
    ULONG NewFlags, OldFlags;

    FlagsAddress = (PVOID) GetExpression ("NTDLL!RtlpDebugPageHeapGlobalFlags" );
    OldFlags = FetchRemoteULONG (FlagsAddress);
    NewFlags = strtoul (ArgumentString, NULL, 16 );

    if (FlagsAddress == NULL) {
        dprintf ("Cannot read page heap global flags variable. \n"
                 "Make sure you have the right ntdll.dll sysmbols. \n");
        return;
    }

    //
    // Detect a help request.
    //

    if (ArgumentString != NULL
        && strchr (ArgumentString, '?')) {

        dprintf ("\nUsage: !dphfind [FLAGS] \n\n"
                 "Sets the global page heap flags. The value has the following structure:\n\n"
            TEXT("                                                                     \n")
            TEXT("    B7-B0   Bit flags    1 - enable page heap                        \n")
            TEXT("                                                                     \n")
            TEXT("         01 - enable page heap. If zero normal heap is used.         \n")
            TEXT("              In 99%% of the cases you will want this to be set.      \n")
            TEXT("         02 - collect stack traces (default on checked builds)       \n")
            TEXT("         04 - minimize memory impact                                 \n")
            TEXT("         08 - minimize randomly(1)/based on size range(0)            \n")
            TEXT("         10 - catch backward overruns                                \n")
            TEXT("                                                                     \n")
            TEXT("    B15-B8  Percentage of available memory from total memory below   \n")
            TEXT("            which allocations will be made from normal heap. Used    \n")
            TEXT("            in conjuction with bit flag 04.                          \n")
            TEXT("                                                                     \n")
            TEXT("    B31-B24 Probability for page heap allocation. Bit 4 and 8 must   \n")
            TEXT("            be set.                                                  \n")
            TEXT("                                                                     \n")
            TEXT("    B31-B24 Size range start                                         \n")
            TEXT("    B23-B16 Size range end                                           \n")
            TEXT("            Allocations in this size range will be made in page heap.\n")
            TEXT("            Bit 4 must be set and bit 8 must be reset.               \n")
            TEXT("                                                                     \n")
            TEXT(" Examples:                                                           \n")
            TEXT("                                                                     \n")
            TEXT("    !dphflags 0x03                                                   \n")
            TEXT("                                                                     \n")
            TEXT("        Enable stack trace collection on free builds where it is not \n")
            TEXT("        the default.                                                 \n")
            TEXT("                                                                     \n")
            TEXT("    !dphflags 0x13                                                   \n")
            TEXT("                                                                     \n")
            TEXT("        Put the not accessible page at the begining of the allocation\n")
            TEXT("        and enable stack traces.                                     \n")
            TEXT("                                                                     \n")
            TEXT("    !dphflags 0x3000300F                                             \n")
            TEXT("                                                                     \n")
            TEXT("        With 48%% probability allocate in page heap. If memory gets  \n")
            TEXT("        below 48%% then all allocations are done in normal heap.     \n")
            TEXT("                                                                     \n"));

        return;
    }

    dprintf ("Current flags: %08X \n\n", OldFlags);

    if (OldFlags & PAGE_HEAP_ENABLE_PAGE_HEAP) {
        dprintf ("EnablePageHeap \n");
    }

    if (OldFlags & PAGE_HEAP_COLLECT_STACK_TRACES) {
        dprintf ("CollectStackTraces \n");
    }

    if (OldFlags & PAGE_HEAP_MINIMIZE_MEMORY_IMPACT) {

        dprintf ("MinimizeMemoryImpact (criteria %u)\n",
                 ((OldFlags >> 8) & 0xFF));

        if (OldFlags & PAGE_HEAP_VERIFY_RANDOMLY) {
            dprintf ("VerifyRandomly (%u)\n", (OldFlags >> 24));
        }
        else {
            dprintf ("VerifyRange (%u - %u)\n",
                     (OldFlags >> 24), ((OldFlags >> 16) & 0xFF));
        }
    }

    if (OldFlags & PAGE_HEAP_CATCH_BACKWARD_OVERRUNS) {
        dprintf ("CatchBackwardOverruns \n");
    }

    //
    // Figure out if we have to write back something. If the guy really
    // wants to set the value zero then we are hosed. Although this can be
    // workaround by using a value like 0x2 (disables page heap) I need to
    // fix this in the future.
    //

    if (NewFlags != 0) {
        if (! WriteRemoteULONG (FlagsAddress, NewFlags)) {
            dprintf ("Failed to write back the flags value %08X !\n", NewFlags);
        }

        dprintf ("New flags: %08X \n", NewFlags);
    }

    return;
}

