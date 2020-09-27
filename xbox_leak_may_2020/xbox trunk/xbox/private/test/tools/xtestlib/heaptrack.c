/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    heaptrack.c

Abstract:

    Routines to tracking private heap created by HeapCreate/HeapDestroy.
    The basic idea is that IATs (of HeapCreate/HeapDestroy) of implicit
    modules will be intercepted during DLL_PROCESS_ATTACH of XTESTLIB.DLL.
    IATs of dynamic, explicit modules will be intercepted during image
    load notification callback from NTOSKRNL

    For implicit module, the tricky part is the way harness.exe get built.
    Makes sure that XTESTLIB.LIB is the very first library to link with so
    that XTESTLIB will get DLL_PROCESS_ATTACH before other modules and
    be able to intercept IATs. Otherwise we will miss HeapCreate called
    during DLL_PROCESS_ATTACH

Author:

    Sakphong Chanbai (schanbai) 11-Jul-2000

Environment:

    Xbox

Revision History:

--*/


#include "precomp.h"
#include "misc.h"
#include "xeutils.h"
#include "prochook.h"
#include "heaptrack.h"


#define RET_ADDR( c ) *((PULONG)&(c) - 1)


typedef HANDLE (WINAPI *HEAPCREATEPROC)(DWORD, SIZE_T, SIZE_T);
typedef BOOL (WINAPI *HEAPDESTROYPROC)(HANDLE);

CRITICAL_SECTION HeapHandleDatabaseLock;
PPRIVATE_HEAP_RECORD HeapHandleDatabase;

PXIMAGE_MODULE_HEADER XapiModuleHeader;
HEAPCREATEPROC HeapCreateProc;
HEAPDESTROYPROC HeapDestroyProc;


BOOL
InsertHeapHandleToDatabase(
    HANDLE HeapHandle,
    ULONG  OwnerModuleIndex
    )

/*++

Routine Description:

    This is a routine to insert heap handle from database

Arguments:

    HeapHandle - Heap handle returned by HeapCreate or GetProcessHeap
    OwnerModuleIndex - XE module index of owner who created the heap

Return Value:

    TRUE if success, FALSE otherwise

--*/

{
    ULONG i, ModuleIndex;
    PPRIVATE_HEAP_RECORD p = HeapHandleDatabase;

    for ( i=0; i<MAX_HEAP_HANDLE_TRACKING; i++, p++ ) {
        if ( !p->HeapHandle ) {
            ASSERT( p->OwnerModuleIndex == ~0UL );
            p->HeapHandle = HeapHandle;
            p->OwnerModuleIndex = OwnerModuleIndex;
            break;
        }
    }

    return i < MAX_HEAP_HANDLE_TRACKING;
}


BOOL
RemoveHeapHandleFromDatabase(
    HANDLE HeapHandle,
    ULONG  OwnerModuleIndex
    )

/*++

Routine Description:

    This is a routine to remove heap handle from database

Arguments:

    HeapHandle - Heap handle returned by HeapCreate or GetProcessHeap

Return Value:

    TRUE if success, FALSE otherwise

--*/

{
    ULONG i, ModuleIndex;
    PPRIVATE_HEAP_RECORD p = HeapHandleDatabase;

    for ( i=0; i<MAX_HEAP_HANDLE_TRACKING; i++, p++ ) {
        if ( p->HeapHandle == HeapHandle ) {
            ASSERT( p->OwnerModuleIndex == OwnerModuleIndex );
            p->HeapHandle = NULL;
            p->OwnerModuleIndex = ~0UL;
            break;
        }
    }

    return i < MAX_HEAP_HANDLE_TRACKING;
}


HANDLE
WINAPI
xxx_HeapCreate(
    DWORD  flOptions,
    SIZE_T dwInitialSize,
    SIZE_T MaximumSize
    )

/*++

Routine Description:

    The HeapCreate function creates a heap object that can be used by the calling
    process. The function reserves space in the virtual address space of the
    process and allocates physical storage for a specified initial portion of
    this block

Arguments:

    flOptions - Specifies optional attributes for the new heap. These options
        affect subsequent access to the new heap through calls to the heap functions
        See MSDN for more details

    dwInitialSize - Specifies the initial size, in bytes, of the heap. This value
        determines the initial amount of physical storage that is allocated for
        the heap. The value is rounded up to the next page boundary

    dwMaximumSize - If dwMaximumSize is a nonzero value, it specifies the maximum
        size, in bytes, of the heap. The HeapCreate function rounds dwMaximumSize
        up to the next page boundary, and then reserves a block of that size in
        the process's virtual address space for the heap. See MSDN for more details

Return Value:

    Handle to newly created heap or NULL if the function fails

--*/

{
    HANDLE HeapHandle = HeapCreateProc( flOptions, dwInitialSize, MaximumSize );

    if ( HeapHandle && HeapHandleDatabase ) {
        try {
            EnterCriticalSection( &HeapHandleDatabaseLock );
            InsertHeapHandleToDatabase( HeapHandle, XeModuleIndexFromAddress(RET_ADDR(flOptions)) );
        } finally {
            LeaveCriticalSection( &HeapHandleDatabaseLock );
        }
    }

    return HeapHandle;
}


BOOL
WINAPI
xxx_HeapDestroy(
    HANDLE HeapHandle
    )

/*++

Routine Description:

    The HeapDestroy function destroys the specified heap object. HeapDestroy
    decommits and releases all the pages of a private heap object, and it
    invalidates the handle to the heap

Arguments:

    HeapHandle - Specifies the heap to be destroyed. This parameter should be
    a heap handle returned by the HeapCreate function. Do not use the handle to
    the process heap returned by the GetProcessHeap function

Return Value:

    TRUE if succeeds, FALSE otherwise

--*/

{
    BOOL b = HeapDestroyProc( HeapHandle );

    if ( HeapHandle && b && HeapHandleDatabase ) {
        try {
            EnterCriticalSection( &HeapHandleDatabaseLock );
            RemoveHeapHandleFromDatabase( HeapHandle, XeModuleIndexFromAddress(RET_ADDR(HeapHandle)) );
        } finally {
            LeaveCriticalSection( &HeapHandleDatabaseLock );
        }
    }

    return b;
}


VOID
XTestLibLoadImageNotifyRoutine(
    PUNICODE_STRING FullImageName,
    PVOID   ImageHandle,
    PVOID   ImageBaseAddress,
    BOOLEAN Unloading
    )

/*++

Routine Description:

    This is a callback routine notified when there is a module about to load
    or unload. We will hook xapi!HeapCreate and xapi!HeapDestroy here.

Arguments:

    None

Return Value:

    TRUE if success, FALSE otherwise

--*/

{
    if ( Unloading || !XapiModuleHeader || !HeapCreateProc || !HeapDestroyProc ) {
        return;
    }

    // Hijack xapi!HeapCreate to xxx_HeapCreate
    HookPEImportedFunctionEx(
        (ULONG_PTR)ImageBaseAddress,
        "xapi.dll",
        (PROC)HeapCreateProc,
        (PROC)xxx_HeapCreate
        );

    // Hijack xapi!HeapDestroy to xxx_HeapDestroy
    HookPEImportedFunctionEx(
        (ULONG_PTR)ImageBaseAddress,
        "xapi.dll",
        (PROC)HeapDestroyProc,
        (PROC)xxx_HeapDestroy
        );
}


BOOL
InitializeHeapHandleTracking(
    VOID
    )

/*++

Routine Description:

    This routine initializes critical section, allocates buffer to track
    private heap created by HeapCreate/HeapDestroy, setup load image notification
    callback and intercepts HeapCreate/HeapDestroy from implicit module

Arguments:

    None

Return Value:

    TRUE if success, FALSE otherwise

--*/

{
    NTSTATUS Status;
    PXIMAGE_HEADER ImageHeader;
    PXIMAGE_MODULE_HEADER ModuleHeader;
    PVOID VarImageHeader;
    PPRIVATE_HEAP_RECORD p;
    ULONG i;

    InitializeCriticalSection( &HeapHandleDatabaseLock );

    ImageHeader = XTestLibLocateXImageHeader();
    if ( !ImageHeader ) {
        DbgPrint( "XTESTLIB: unable to get XE image header\n" );
        DebugBreak();
        return FALSE;
    }

    XapiModuleHeader = XeModuleHeaderFromModuleName( "xapi.dll" );
    if ( !XapiModuleHeader ) {
        DbgPrint( "XTESTLIB: unable to get module header of xapi.dll\n" );
        DebugBreak();
        return FALSE;
    }

    HeapCreateProc = (HEAPCREATEPROC)GetProcAddress( (HMODULE)XapiModuleHeader, "HeapCreate" );
    HeapDestroyProc = (HEAPDESTROYPROC)GetProcAddress( (HMODULE)XapiModuleHeader, "HeapDestroy" );

    if ( !HeapCreateProc || !HeapDestroyProc ) {
        DbgPrint( "XTESTLIB: unable to get proc address of HeapCreate/HeapDestroy\n" );
        DebugBreak();
        return FALSE;
    }

    HeapHandleDatabase = AllocateKernelPool( sizeof(PRIVATE_HEAP_RECORD) * MAX_HEAP_HANDLE_TRACKING );
    if ( !HeapHandleDatabase ) {
        DbgPrint( "XTESTLIB: unable to allocate memory for private heap tracking\n" );
        DebugBreak();
        return FALSE;
    }

    for ( p=HeapHandleDatabase, i=0; i<MAX_HEAP_HANDLE_TRACKING; p++, i++ ) {
        p->OwnerModuleIndex = ~0UL;
    }

    ASSERT( GetProcessHeap() != NULL );

    if ( GetProcessHeap() ) {
        InsertHeapHandleToDatabase( GetProcessHeap(), 0 );
    }

    VarImageHeader = (PVOID)(ImageHeader->HeadersBase + XIMAGE_FIXED_HEADER_SIZE(ImageHeader));
    ModuleHeader = XIMAGE_FIRST_MODULE(ImageHeader, VarImageHeader); 

    for ( i=0; i<ImageHeader->OptionalHeader.NumberOfModules; i++, ModuleHeader++ ) {

        //
        // Only do this for implicit module, load image callback will take care
        // of explicit module later once it is being loaded
        //
        if ( ModuleHeader->ModuleFlags & XIMAGE_MODFLAG_IMPLICIT ) {
            HookImportedFunction( (HMODULE)ModuleHeader, "xapi.dll", "HeapCreate", (PROC)xxx_HeapCreate );
            HookImportedFunction( (HMODULE)ModuleHeader, "xapi.dll", "HeapDestroy", (PROC)xxx_HeapDestroy );
        }
    }

    Status = PsSetLoadImageNotifyRoutine( XTestLibLoadImageNotifyRoutine );
    if ( !NT_SUCCESS(Status) ) {
        DbgPrint( "XTESTLIB: PsSetLoadImageNotifyRoutine failed 0x%x\n", Status );
        DebugBreak();
        return FALSE;
    }

    return TRUE;
}


VOID
UninitializeHeapHandleTracking(
    VOID
    )

/*++

Routine Description:

    This routine frees memory allocate by InitializeHeapHandleTracking

Arguments:

    None

Return Value:

    None

--*/

{
    DeleteCriticalSection( &HeapHandleDatabaseLock );

    if ( HeapHandleDatabase ) {
        FreeKernelPool( HeapHandleDatabase );
    }
}


BOOL
WINAPI
GetHeapTrackingDatabase(
    PPRIVATE_HEAP_RECORD HeapRecordPtr,
    SIZE_T BufferSize
    )

/*++

Routine Description:

    This routine copies raw data from heap handle tracking database to specified
    buffer.

Arguments:

    HeapRecordPtr - Buffer to receive heap handle tracking data
    BufferSize - Size of buffer in byte

Return Value:

    TRUE if succeeds, FALSE otherwise. To get extended error information, call GetLastError

--*/

{
#if 0
    DWORD ErrorCode = ERROR_NOACCESS;
    BOOL Success = FALSE;

    if ( BufferSize < (sizeof(PRIVATE_HEAP_RECORD) * MAX_HEAP_HANDLE_TRACKING) ) {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        return FALSE;
    }

    try {
        EnterCriticalSection( &HeapHandleDatabaseLock );
        RtlCopyMemory( HeapRecordPtr, HeapHandleDatabase, sizeof(PRIVATE_HEAP_RECORD) * MAX_HEAP_HANDLE_TRACKING );
        ErrorCode = ERROR_SUCCESS;
        Success = TRUE;
    } finally {
        LeaveCriticalSection( &HeapHandleDatabaseLock );
    }

    SetLastError( ErrorCode );
    return Success;
#endif

    SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
    return FALSE;
}
