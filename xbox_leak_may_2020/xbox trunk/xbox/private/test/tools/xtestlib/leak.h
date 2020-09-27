/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    leak.h

Abstract:

    Leak debugging routines

Author:

    Sakphong Chanbai (schanbai) 22-May-2000

Environment:

    Xbox

Revision History:

--*/


#ifndef __LEAK_H__
#define __LEAK_H__


#ifdef __cplusplus
extern "C" {
#endif


//
// Allocation history for various memory allocation APIs
// (malloc, calloc, free, LocalAlloc, LocalFree, etc.)
//

typedef enum {

    AllocationType_malloc = 0,
    AllocationType_calloc,
    AllocationType_realloc,
    AllocationType_LocalAlloc,
    AllocationType_GlobalAlloc,
    AllocationType_HeapAlloc,
    AllocationType_operator_new,
    AllocationType_operator_new_bracket,
    AllocationType_VirtualAlloc,
    AllocationType_CreateEvent,
    AllocationType_OpenEvent,
    AllocationType_CreateMutex,
    AllocationType_OpenMutex,
    AllocationType_CreateSemaphore,
    AllocationType_OpenSemaphore,
    AllocationType_CreateWaitableTimer,
    AllocationType_OpenWaitableTimer,
    AllocationType_FindFirstFileA,
    AllocationType_FindFirstFileW,
    AllocationType_Max

} AllocationType;


#define AllocationHistorySignature  '@meM'

#define ASSERT_SIGNATURE( p )  ASSERT( (p)->Signature == AllocationHistorySignature )

typedef struct _ALLOCATION_HISTORY {

    LIST_ENTRY      List;
    ULONG_PTR       Caller;       // Caller address (actually it's return address)
    PVOID           MemBlock;
    ULONG           Size;
    AllocationType  AllocType;
    PVOID          *StackBackTrace;

#if DBG
    ULONG           Signature;
#endif

} ALLOCATION_HISTORY, *PALLOCATION_HISTORY;


#define ENABLE_LEAK_TRACKING        0x00000001
#define CAPTURE_STACK_BACK_TRACE    0x00000002
#define DETECT_INVALID_FREE         0x80000000
#define MAX_STACK_DEPTH             32


#define RET_ADDR( c ) *((PULONG)&(c) - 1)


BOOL
InitializeHeapAllocationHistory(
    VOID
    );


VOID
UninitializeHeapAllocationHistory(
    VOID
    );


#ifdef __cplusplus
}
#endif


#endif // __LEAK_H__
