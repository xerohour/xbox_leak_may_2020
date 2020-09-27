/*++

Copyright (c) 1990-1999  Microsoft Corporation

Module Name:

    heap.c

Abstract:

    Maps the Win32 heap APIs to the Rtl heap functions

--*/

#include "basedll.h"
#pragma hdrstop

//
// Handle of the default process heap.
//
HANDLE XapiProcessHeap;

HANDLE
WINAPI
HeapCreate(
    DWORD flOptions,
    SIZE_T dwInitialSize,
    SIZE_T dwMaximumSize
    )
/*++

Routine Description:

    Creates a new heap

Arguments:

    flOptions - either HEAP_GENERATE_EXCEPTIONS or HEAP_NO_SERIALIZE
    dwInitialSize - initial heap size
    dwMaximumSize - maximum heap size

Return Value:

    Returns the new heap handle

--*/
{
    HANDLE hHeap;
    ULONG GrowthThreshold;
    ULONG Flags;


    Flags = (flOptions & (HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE)) | HEAP_CLASS_1;
    GrowthThreshold = 0;

    if (dwMaximumSize < PAGE_SIZE) {

        if (dwMaximumSize == 0) {

            GrowthThreshold = PAGE_SIZE * 16;
            Flags |= HEAP_GROWABLE;
            }
        else {
            dwMaximumSize = PAGE_SIZE;
            }
        }

    if (GrowthThreshold == 0 && dwInitialSize > dwMaximumSize) {
        dwMaximumSize = dwInitialSize;
        }

    hHeap = (HANDLE)RtlCreateHeap( Flags,
                                   NULL,
                                   dwMaximumSize,
                                   dwInitialSize,
                                   0,
                                   NULL
                                 );
    if (hHeap == NULL) {
        SetLastError( ERROR_NOT_ENOUGH_MEMORY );
        }

    return( hHeap );
}


BOOL
WINAPI
HeapDestroy(
    HANDLE hHeap
    )
{
    if (RtlDestroyHeap( (PVOID)hHeap ) == NULL ) {
        return( TRUE );
        }
    else {
        SetLastError( ERROR_INVALID_HANDLE );
        return( FALSE );
        }
}


BOOL
WINAPI
HeapFree(
    HANDLE hHeap,
    DWORD  dwFlags,
    LPVOID lpMem
    )
{
    return (BOOL) RtlFreeHeap( (PVOID)hHeap, dwFlags, lpMem );
}

#if DBG

BOOL
WINAPI
HeapValidate(
    HANDLE hHeap,
    DWORD dwFlags,
    LPVOID lpMem
    )
{
    return RtlValidateHeap( hHeap, dwFlags, lpMem );
}

#endif // DBG

HANDLE
WINAPI
GetProcessHeap( VOID )
/*++

Routine Description:

    Returns the handle for the process default heap (used with Local/GlobalXxxx functions)

Arguments:

Return Value:

    Process heap handle (can be used with HeapAlloc/HeapFree/HeapSize functions)

--*/
{
    return XapiProcessHeap;
}

#if DBG

WINBASEAPI
SIZE_T
WINAPI
HeapCompact(
    HANDLE hHeap,
    DWORD dwFlags
    )
{
    return RtlCompactHeap( hHeap, dwFlags );
}


WINBASEAPI
BOOL
WINAPI
HeapLock(
    HANDLE hHeap
    )
{
    return RtlLockHeap( hHeap );
}


WINBASEAPI
BOOL
WINAPI
HeapUnlock(
    HANDLE hHeap
    )
{
    return RtlUnlockHeap( hHeap );
}

WINBASEAPI
BOOL
WINAPI
HeapWalk(
    HANDLE hHeap,
    LPPROCESS_HEAP_ENTRY lpEntry
    )
{
    RTL_HEAP_WALK_ENTRY Entry;
    NTSTATUS Status;

    if (lpEntry->lpData == NULL) {
        Entry.DataAddress = NULL;
        Status = RtlWalkHeap( hHeap, &Entry );
        }
    else {
        Entry.DataAddress = lpEntry->lpData;
        Entry.SegmentIndex = lpEntry->iRegionIndex;
        if (lpEntry->wFlags & PROCESS_HEAP_REGION) {
            Entry.Flags = RTL_HEAP_SEGMENT;
            }
        else
        if (lpEntry->wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE) {
            Entry.Flags = RTL_HEAP_UNCOMMITTED_RANGE;
            Entry.DataSize = lpEntry->cbData;
            }
        else
        if (lpEntry->wFlags & PROCESS_HEAP_ENTRY_BUSY) {
            Entry.Flags = RTL_HEAP_BUSY;
            }
        else {
            Entry.Flags = 0;
            }

        Status = RtlWalkHeap( hHeap, &Entry );
        }

    if (NT_SUCCESS( Status )) {
        lpEntry->lpData = Entry.DataAddress;
        lpEntry->cbData = (DWORD)Entry.DataSize;
        lpEntry->cbOverhead = Entry.OverheadBytes;
        lpEntry->iRegionIndex = Entry.SegmentIndex;
        if (Entry.Flags & RTL_HEAP_BUSY) {
            lpEntry->wFlags = PROCESS_HEAP_ENTRY_BUSY;

            //
            // DDESHARE and MOVEABLE are not meaningful on Xbox
            //
#if 0
            if (Entry.Flags & BASE_HEAP_FLAG_DDESHARE) {
                lpEntry->wFlags |= PROCESS_HEAP_ENTRY_DDESHARE;
                }

            if (Entry.Flags & BASE_HEAP_FLAG_MOVEABLE) {
                lpEntry->wFlags |= PROCESS_HEAP_ENTRY_MOVEABLE;
                lpEntry->Block.hMem = (HLOCAL)Entry.Block.Settable;
                }
#endif // 0

            memset( lpEntry->Block.dwReserved, 0, sizeof( lpEntry->Block.dwReserved ) );
            }
        else
        if (Entry.Flags & RTL_HEAP_SEGMENT) {
            lpEntry->wFlags = PROCESS_HEAP_REGION;
            lpEntry->Region.dwCommittedSize = Entry.Segment.CommittedSize;
            lpEntry->Region.dwUnCommittedSize = Entry.Segment.UnCommittedSize;
            lpEntry->Region.lpFirstBlock = Entry.Segment.FirstEntry;
            lpEntry->Region.lpLastBlock = Entry.Segment.LastEntry;
            }
        else
        if (Entry.Flags & RTL_HEAP_UNCOMMITTED_RANGE) {
            lpEntry->wFlags = PROCESS_HEAP_UNCOMMITTED_RANGE;
            memset( &lpEntry->Region, 0, sizeof( lpEntry->Region ) );
            }
        else {
            lpEntry->wFlags = 0;
            }

        return TRUE;
        }
    else {
        XapiSetLastNTError( Status );
        return FALSE;
        }
}

#endif // DBG

HLOCAL
WINAPI
LocalAlloc(
    UINT uFlags,
    SIZE_T uBytes
    )
/*++

Routine Description:

    The LocalAlloc function allocates the specified number of bytes from the heap.
    In the linear Win32 API environment, there is no difference between the
    local heap and the global heap.

    NOTE: Moveable memory is not implemented. LMEM_FIXED is always on.
Arguments:

    UINT uFlags - allocation attributes
    UINT uBytes - number of bytes to allocate

Return Value:

    Returns pointer to newly allocated memory or NULL on failure.

--*/
{
#if DBG
    // Validate flags
    if (uFlags & (LMEM_MOVEABLE | LMEM_DISCARDABLE))
    {
        RIP("LocalAlloc() invalid parameter (uFlags)");
    }
#endif // DBG

    return (HLOCAL)RtlAllocateHeap(XapiProcessHeap,
        (uFlags & LMEM_ZEROINIT) ? HEAP_ZERO_MEMORY : 0, (DWORD)uBytes);
}

HLOCAL
WINAPI
LocalReAlloc(
    HLOCAL hMem,
    SIZE_T uBytes,
    UINT uFlags
    )
/*++

Routine Description:

    The LocalReAlloc function changes the size of a specified 
    local memory object. The size can increase or decrease.

    NOTE: Since moveable memory isn't implemented, passing LMEM_MOVEABLE allows the handle
    (pointer) value to change on return. If LMEM_MOVEABLE is not passed in and there is
    no space following the memory block, the call may fail to grow the block size. Note
    that this is the opposite sense of the HEAP_REALLOC_IN_PLACE_ONLY flag.

Arguments:

    hMem - pointer to memory object to resize
    uBytes - new size
    uFlags - options for reallocation (same semantics as HeapReAlloc)

Return Value:

    returns the new handle (may have changed) or NULL on failure.

--*/
{
#if DBG
    // Validate flags
    if (uFlags & (LMEM_DISCARDABLE | LMEM_MODIFY))
    {
        RIP("LocalReAlloc() invalid parameter (uFlags)");
    }
#endif // DBG

    return (HLOCAL)RtlReAllocateHeap(XapiProcessHeap,
        (uFlags & LMEM_ZEROINIT) ? HEAP_ZERO_MEMORY : 0 +
        (uFlags & LMEM_MOVEABLE) ? 0 : HEAP_REALLOC_IN_PLACE_ONLY,
        (PVOID)hMem, (DWORD)uBytes);
}

SIZE_T
WINAPI
LocalSize(
    HLOCAL hMem
    )
/*++

Routine Description:

    Returns the size of an allocation in bytes. Also implements GlobalSize()

Arguments:

    hMem - pointer to memory

Return Value:

    size or zero on error.

--*/
{
    return (SIZE_T)RtlSizeHeap(XapiProcessHeap, 0, (LPVOID)hMem);
}

HLOCAL
WINAPI
LocalFree(
    HLOCAL hMem
    )
/*++

Routine Description:

    The LocalFree function frees the specified local memory object and
    invalidates its handle. Also implements GlobalFree().

Arguments:

    hMem - handle of local memory object

Return Value:

    If the function succeeds, the return value is NULL. If the function fails,
    the return value is equal to the handle of the local memory object.

--*/
{
    if (RtlFreeHeap(XapiProcessHeap, 0, (LPVOID)hMem))
        return NULL;
    else
        return hMem;
}

WINBASEAPI
LPVOID
WINAPI
LocalLock(
    IN HLOCAL hMem
    )
/*++

Routine Description:

    Maps a handle to a pointer. Since only fixed memory is supported,
    the input value is returned. This function is unneccesary overhead
    for applications to call. Note that no reference count is maintained.

Arguments:

    hMem - memory handle

Return Value:

    returns hMem cast to a pointer.

--*/
{
    return (LPVOID)hMem;
}

WINBASEAPI
HLOCAL
WINAPI
LocalHandle(
    IN LPCVOID pMem
    )
/*++

Routine Description:

    Maps a pointer to a handle. Since only fixed memory is supported,
    the input value is returned. It is safe to assume that handles == pointers
    for LocalXxxx functions.

Arguments:

    pMem - memory pointer

Return Value:

    returns pMem cast as a handle

--*/
{
    return (HLOCAL)pMem;
}

WINBASEAPI
BOOL
WINAPI
LocalUnlock(
    IN HLOCAL hMem
    )
/*++

Routine Description:

    Does nothing at all since only fixed memory is supported
    
Arguments:

    hMem - memory handle

Return Value:

    Always returns true

--*/
{
    return TRUE;
}

HGLOBAL
WINAPI
GlobalAlloc(
    UINT uFlags,
    SIZE_T dwBytes
    )
/*++

Routine Description:

    The GlobalAlloc function allocates the specified number of bytes from the heap.
    In the linear Win32 API environment, there is no difference between the 
    local heap and the global heap. 

    NOTE: Moveable memory is not implemented. GMEM_FIXED is always on.

Arguments:

    UINT uFlags - allocation attributes
    UINT uBytes - number of bytes to allocate  

Return Value:

    Returns pointer to newly allocated memory or NULL on failure.

--*/
{
#if DBG
    // Validate flags (don't allow GMEM_MOVEABLE or GMEM_DISCARDABLE
    // ignore all other flags--they should be completely benign)
    if (uFlags & (GMEM_MOVEABLE | GMEM_DISCARDABLE))
    {
        RIP("GlobalAlloc() invalid parameter (uFlags)");
    }
#endif // DBG

    return (HLOCAL)RtlAllocateHeap(XapiProcessHeap,
        (uFlags & GMEM_ZEROINIT) ? HEAP_ZERO_MEMORY : 0, (DWORD)dwBytes);
}

HGLOBAL
WINAPI
GlobalReAlloc(
    HANDLE hMem,
    SIZE_T uBytes,
    UINT uFlags
    )
/*++

Routine Description:

    The GlobalReAlloc function changes the size of a specified 
    local memory object. The size can increase or decrease.

    NOTE: Since moveable memory isn't implemented, passing GMEM_MOVEABLE allows the handle
    (pointer) value to change on return. If GMEM_MOVEABLE is not passed in and there is
    no space following the memory block, the call may fail to grow the block size. Note
    that this is the opposite sense of the HEAP_REALLOC_IN_PLACE_ONLY flag.

Arguments:

    hMem - pointer to memory object to resize
    uBytes - new size
    uFlags - options for reallocation (same semantics as HeapReAlloc)
    
Return Value:

    returns the new handle (may have changed) or NULL on failure.
    
--*/
{
#if DBG
    // Validate flags (don't allow GMEM_DISCARDABLE or GMEM_MODIFY, ignore others
    // as they should be completely benign)
    if (uFlags & (GMEM_DISCARDABLE | GMEM_MODIFY))
    {
        RIP("GlobalReAlloc() invalid parameter (uFlags)");
    }
#endif // DBG

    return (HLOCAL)RtlReAllocateHeap(XapiProcessHeap,
        (uFlags & GMEM_ZEROINIT) ? HEAP_ZERO_MEMORY : 0 +
        (uFlags & GMEM_MOVEABLE) ? 0 : HEAP_REALLOC_IN_PLACE_ONLY,
        (PVOID)hMem, (DWORD)uBytes);
}
