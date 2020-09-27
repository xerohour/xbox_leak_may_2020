/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    leak.c

Abstract:

    Leak debugging routines

Author:

    Sakphong Chanbai (schanbai) 22-May-2000

Environment:

    Xbox

Revision History:

--*/


#include "precomp.h"
#include "prochook.h"
#include "leak.h"
#include "misc.h"


CRITICAL_SECTION HistoryDatabaseLock;
LIST_ENTRY HistoryDatabase;
ULONG HistoryEntriesCount;
LONG AllocationTrackingEnabled = 0;


BOOL
InitializeHeapAllocationHistory(
    VOID
    )

/*++

Routine Description:
              
    Initialize heap and critical section for heap allocation history

Arguments:

    None

Return:
    
    TRUE if success, FALSE otherwise

--*/

{
    InitializeListHead( &HistoryDatabase );
    InitializeCriticalSection( &HistoryDatabaseLock );
    HistoryEntriesCount = 0;
    return TRUE;
}


VOID
UninitializeHeapAllocationHistory(
    VOID
    )
{
    PLIST_ENTRY Entry;
    PALLOCATION_HISTORY p;

    EnterCriticalSection( &HistoryDatabaseLock );

    Entry = HistoryDatabase.Flink;
    while ( Entry != &HistoryDatabase ) {
        p = CONTAINING_RECORD( Entry, ALLOCATION_HISTORY, List );
        ASSERT( p );
        ASSERT_SIGNATURE( p );
        ASSERT( p->AllocType < AllocationType_Max );
        Entry = Entry->Flink;
        FreeKernelPool( p );
    }

    HistoryEntriesCount = 0;
    InitializeListHead( &HistoryDatabase );
    LeaveCriticalSection( &HistoryDatabaseLock );

    DeleteCriticalSection( &HistoryDatabaseLock );
}


VOID
InsertAllocationHistory(
    ULONG Caller,
    PVOID MemBlock,
    ULONG Size,
    AllocationType AllocType
    )
{
    PLIST_ENTRY Entry;
    PALLOCATION_HISTORY p;
    ULONG AllocSize = sizeof(ALLOCATION_HISTORY);
    ULONG Hash;

    if ( !MemBlock || !AllocationTrackingEnabled ) {
        return;
    }

    EnterCriticalSection( &HistoryDatabaseLock );

    // Find insertion point or that the list is sorted by caller
    Entry = HistoryDatabase.Flink;
    while ( Entry != &HistoryDatabase ) {
        p = CONTAINING_RECORD( Entry, ALLOCATION_HISTORY, List );
        ASSERT( p );
        ASSERT_SIGNATURE( p );
        ASSERT( p->AllocType < AllocationType_Max );
        if ( p->Caller == Caller ) {
            break;
        }
        Entry = Entry->Flink;
    }

    if ( AllocationTrackingEnabled & CAPTURE_STACK_BACK_TRACE ) {
        AllocSize += ( sizeof(PULONG) * MAX_STACK_DEPTH );
    }

    p = (PALLOCATION_HISTORY)AllocateKernelPool( AllocSize );

    if ( p ) {
        p->Caller = Caller;
        p->MemBlock = MemBlock;
        p->AllocType = AllocType;
        p->Size = Size;

        if ( AllocationTrackingEnabled & CAPTURE_STACK_BACK_TRACE ) {
            p->StackBackTrace = (PVOID*)( (PBYTE)p + sizeof(ALLOCATION_HISTORY) );
            RtlCaptureStackBackTrace( 2, MAX_STACK_DEPTH, p->StackBackTrace, &Hash );
        } else {
            p->StackBackTrace = NULL;
        }

#if DBG
        p->Signature = AllocationHistorySignature;
#endif

        InsertHeadList( Entry, &p->List );
        HistoryEntriesCount++;
    } else {
        DbgPrint( "XTESTLIB: unable to allocate allocation history\n" );
    }
    
    LeaveCriticalSection( &HistoryDatabaseLock );
}


BOOL
RemoveAllocationHistory(
    PVOID MemBlock
    )
{
    PLIST_ENTRY Entry;
    PALLOCATION_HISTORY p;
    BOOL Found = FALSE;

    if ( !AllocationTrackingEnabled  ) {
        return TRUE;
    }

    EnterCriticalSection( &HistoryDatabaseLock );

    Entry = HistoryDatabase.Flink;
    while ( Entry != &HistoryDatabase ) {
        p = CONTAINING_RECORD( Entry, ALLOCATION_HISTORY, List );
        ASSERT( p );
        ASSERT_SIGNATURE( p );
        ASSERT( p->AllocType < AllocationType_Max );
        if ( p->MemBlock == MemBlock ) {
            Found = TRUE;
            RemoveEntryList( &p->List );
            HistoryEntriesCount--;
            FreeKernelPool( p );
            break;
        }
        Entry = Entry->Flink;
    }
    
    if ( !Found ) {
        if ( AllocationTrackingEnabled & DETECT_INVALID_FREE ) {
            DbgPrint( "\nXTESTLIB: invalid de-allocation, couldn't find %p in database\n", MemBlock );
            __asm int 3;
        } else {
            Found = TRUE;
        }
    }

    LeaveCriticalSection( &HistoryDatabaseLock );
    return Found;
}


extern "C"
ULONG
WINAPI
SetAllocationTrackingOptions(
    ULONG Flags
    )
{
    return InterlockedExchange( &AllocationTrackingEnabled, Flags );
}


extern "C"
void *
__cdecl
xxx_malloc(
    size_t size
    )
{
    void * ptr = LocalAlloc( LPTR, size );
    InsertAllocationHistory( RET_ADDR(size), ptr, size, AllocationType_malloc );
    return ptr;
}


extern "C"
void *
__cdecl
xxx_calloc(
    size_t num,
    size_t size
    )
{
    size_t total = num*size;
    void * ptr = LocalAlloc( LPTR, total );
    InsertAllocationHistory( RET_ADDR(num), ptr, total, AllocationType_calloc );
    return ptr;
}


extern "C"
void
__cdecl
xxx_free(
    void * memblock
    )
{
    if ( memblock && RemoveAllocationHistory(memblock) ) {
        LocalFree( memblock );
    }
}


extern "C"
void *
__cdecl
xxx_realloc(
    void * memblock,
    size_t size
    )
{
    void * ptr;

    if ( !memblock ) {
        ptr = LocalAlloc( LPTR, size );
        InsertAllocationHistory( RET_ADDR(memblock), ptr, size, AllocationType_realloc );
    } else if ( size == 0 ) {
        ptr = NULL;
        xxx_free( memblock );
    } else {
        ptr = LocalReAlloc( memblock, size, LPTR );
        if ( ptr ) {
            RemoveAllocationHistory( memblock );
            InsertAllocationHistory( RET_ADDR(memblock), ptr, size, AllocationType_realloc );
        } else {
            ptr = NULL;
        }
    }

    return ptr;
}


HLOCAL
WINAPI
xxx_LocalAlloc(
    UINT   uFlags,
    SIZE_T uBytes
    )
{
    HLOCAL h = LocalAlloc( uFlags, uBytes );
    InsertAllocationHistory( RET_ADDR(uFlags), h, uBytes, AllocationType_LocalAlloc );
    return h;
}


extern "C"
HGLOBAL
WINAPI
xxx_GlobalAlloc(
    UINT   uFlags,
    SIZE_T uBytes
    )
{
    HGLOBAL h = GlobalAlloc( uFlags, uBytes );
    InsertAllocationHistory( RET_ADDR(uFlags), h, uBytes, AllocationType_GlobalAlloc );
    return h;
}


HLOCAL
WINAPI
xxx_LocalFree(
    HLOCAL hMem
    )
{
    if ( hMem && !RemoveAllocationHistory(hMem) ) {
        return hMem;
    }

    return LocalFree( hMem );
}


HGLOBAL
WINAPI
xxx_GlobalFree(
    HGLOBAL hMem
    )
{
    if ( hMem && !RemoveAllocationHistory(hMem) ) {
        return hMem;
    }

    return GlobalFree( hMem );
}


LPVOID
WINAPI
xxx_HeapAlloc(
    HANDLE hHeap,
    DWORD  dwFlags,
    SIZE_T dwBytes
    )
{
    LPVOID MemBlock = HeapAlloc( hHeap, dwFlags, dwBytes );
    InsertAllocationHistory( RET_ADDR(hHeap), MemBlock, dwBytes, AllocationType_HeapAlloc );
    return MemBlock;
}


BOOL
WINAPI
xxx_HeapFree(
    HANDLE hHeap,
    DWORD  dwFlags,
    LPVOID lpMem
    )
{
    if ( lpMem && !RemoveAllocationHistory(lpMem) ) {
        return FALSE;
    }

    return HeapFree( hHeap, dwFlags, lpMem );
}


void *
__cdecl
operator new(
    unsigned int size
    )
{
    void * p = LocalAlloc( LPTR, size );
    InsertAllocationHistory( RET_ADDR(size), p, size, AllocationType_operator_new );
    return p;
}


void
__cdecl
operator delete(
    void * lpMem
    )
{
    if ( lpMem && !RemoveAllocationHistory(lpMem) ) {
        return;
    }

    LocalFree( lpMem );
}


void *
__cdecl
operator new[](
    unsigned int size
    )
{
    void * p = LocalAlloc( LPTR, size );
    InsertAllocationHistory( RET_ADDR(size), p, size, AllocationType_operator_new_bracket );
    return p;
}


void
__cdecl
operator delete[](
    void * lpMem
    )
{
    if ( lpMem && !RemoveAllocationHistory(lpMem) ) {
        return;
    }

    LocalFree( lpMem );
}


LPVOID
WINAPI
xxx_VirtualAlloc(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flAllocationType,
    DWORD  flProtect
    )
{
    LPVOID pv = VirtualAlloc( lpAddress, dwSize, flAllocationType, flProtect );

    // Insert allocation tracking only MEM_COMMIT
    if ( flAllocationType & MEM_COMMIT ) {
        InsertAllocationHistory( RET_ADDR(lpAddress), pv, dwSize, AllocationType_VirtualAlloc );
    }

    return pv;
}


BOOL
WINAPI
xxx_VirtualFree(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  dwFreeType
    )
{
    RemoveAllocationHistory( lpAddress );
    return VirtualFree( lpAddress, dwSize, dwFreeType );
}


struct {
    LPCSTR ModuleName;
    LPCSTR ProcName;
    PROC   NewProcAddress;
} ProcToBeHooked[] = {
    "xapi.dll", "LocalAlloc",   (PROC)xxx_LocalAlloc,
    "xapi.dll", "LocalFree",    (PROC)xxx_LocalFree,
    "xapi.dll", "GlobalAlloc",  (PROC)xxx_GlobalAlloc,
    "xapi.dll", "GlobalFree",   (PROC)xxx_GlobalFree,
    "xapi.dll", "HeapAlloc",    (PROC)xxx_HeapAlloc,
    "xapi.dll", "HeapFree",     (PROC)xxx_HeapFree,
    "xapi.dll", "VirtualAlloc", (PROC)xxx_VirtualAlloc,
    "xapi.dll", "VirtualFree",  (PROC)xxx_VirtualFree,
};


extern "C"
BOOL
WINAPI
EnableAllocationTracking(
    HMODULE XeModuleHandle
    )
{
    SIZE_T i, ArraySize = sizeof(ProcToBeHooked)/sizeof(ProcToBeHooked[0]);

    for ( i=0; i<ArraySize; i++ ) {
        HookImportedFunction(
            XeModuleHandle,
            ProcToBeHooked[i].ModuleName,
            ProcToBeHooked[i].ProcName,
            ProcToBeHooked[i].NewProcAddress
            );
    }

    return TRUE;
}
