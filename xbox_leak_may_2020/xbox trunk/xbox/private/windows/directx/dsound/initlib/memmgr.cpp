/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       memmgr.c
 *  Content:    Pool memory manager.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/21/00    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"

// #define MEM_SIZE_CHECK

DWORD g_dwDirectSoundTestPoolMemoryUsage = 0;
DWORD g_dwDirectSoundTestPhysicalMemoryUsage = 0;
DWORD g_dwDirectSoundTestPhysicalMemorySlop = 0;
DWORD g_dwDirectSoundTestPhysicalMemoryRecovered = 0;

#ifdef TRACK_MEMORY_USAGE

//
// Allocation tracking data
//

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY              leListEntry;
    LPCSTR                  pszFile;
    ULONG                   nLine;
    LPCSTR                  pszClass;
    DSOUND_ALLOCATOR_TAG    nAllocatorTag;
    ULONG                   cbSize;
    LPVOID                  pvBaseAddress;
END_DEFINE_STRUCT(DSMEMTRACK);

LIST_ENTRY g_lstDirectSoundTestMemoryTracking;

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  DirectSoundPoolAlloc
 *
 *  Description:
 *      Allocates pool memory.
 *
 *  Arguments:
 *      ULONG [in]: buffer size, in bytes.
 *      DWORD [in]: pool tag.
 *      BOOL [in]: TRUE to zero-initialize the buffer data.
 *
 *  Returns:  
 *      LPVOID: allocation.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestPoolAlloc"

LPVOID 
DirectSoundTestPoolAlloc
(
    DSOUND_POOL_TAG         nPoolTag,
    ULONG                   cbBuffer,
    BOOL                    fZeroInit
)
{
    static const ULONG      cbLowerThreshold    = 0x20;
    static const DWORD      cbUpperThreshold    = 0xFD8;
    LPVOID                  pvBuffer;

    ASSERT(cbBuffer);

#ifdef MEM_SIZE_CHECK

#pragma TODO("Don't ship with this check turned on")
    
    if((DSOUND_OBJECT_POOL_TAG == nPoolTag) && (cbBuffer >= cbUpperThreshold))
    {
        ASSERTMSG("Allocating an object larger than the pool threshold");
    }
    else if((DSOUND_DATA_POOL_TAG == nPoolTag) && (cbBuffer < cbLowerThreshold))
    {
        ASSERTMSG("Allocating a buffer smaller than 32 bytes");
    }

#endif

    if(pvBuffer = ExAllocatePoolWithTag(cbBuffer, nPoolTag))
    {
        cbBuffer = ExQueryPoolBlockSize(pvBuffer);
        g_dwDirectSoundTestPoolMemoryUsage += cbBuffer;

        if(fZeroInit)
        {
            ZeroMemory(pvBuffer, cbBuffer);
        }
    }
    else
    {
        DPF_RESOURCE("Out of pool memory");
    }

    return pvBuffer;
}


/****************************************************************************
 *
 *  DirectSoundTestPoolFree
 *
 *  Description:
 *      Frees memory allocated with DirectSoundTestPoolAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestPoolFree"

void 
DirectSoundTestPoolFree
(
    LPVOID                  pvBuffer
)
{
    DWORD                   cbBuffer;

    ASSERT(pvBuffer);

    cbBuffer = ExQueryPoolBlockSize(pvBuffer);

    ASSERT(g_dwDirectSoundTestPoolMemoryUsage >= cbBuffer);
    g_dwDirectSoundTestPoolMemoryUsage -= cbBuffer;

    ExFreePool(pvBuffer);
}


/****************************************************************************
 *
 *  DirectSoundTestPhysicalAlloc
 *
 *  Description:
 *      Allocates physically contiguous memory.
 *
 *  Arguments:
 *      ULONG [in]: buffer size, in bytes.
 *      ULONG [in]: buffer alignment, in bytes.
 *      DWORD [in]: allocation flags.
 *      BOOL [in]: TRUE to zero-initialize the buffer data.
 *
 *  Returns:  
 *      LPVOID: buffer pointer.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestPhysicalAlloc"

LPVOID 
DirectSoundTestPhysicalAlloc
(
    ULONG                   cbBuffer, 
    ULONG                   cbAlignment, 
    DWORD                   dwFlags,
    BOOL                    fZeroInit
)
{
    LPVOID                  pvBuffer;

    ASSERT(cbBuffer);
    ASSERT(dwFlags);
    
    if(pvBuffer = MmAllocateContiguousMemoryEx(cbBuffer, 0, 0xFFFFFFFF, cbAlignment, dwFlags))
    {
        cbBuffer = MmQueryAllocationSize(pvBuffer);
        g_dwDirectSoundTestPhysicalMemoryUsage += cbBuffer;

        if(fZeroInit)
        {
            ZeroMemory(pvBuffer, cbBuffer);
        }
    }
    else
    {
        DPF_RESOURCE("Out of contiguous memory");
    }

    return pvBuffer;
}


/****************************************************************************
 *
 *  DirectSoundTestPhysicalFree
 *
 *  Description:
 *      Frees memory allocated with DirectSoundTestPhysicalAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestPhysicalFree"

void 
DirectSoundTestPhysicalFree
(
    LPVOID                  pvBuffer
)
{
    DWORD                   cbBuffer;

    ASSERT(pvBuffer);

    cbBuffer = MmQueryAllocationSize(pvBuffer);

    ASSERT(g_dwDirectSoundTestPhysicalMemoryUsage >= cbBuffer);
    g_dwDirectSoundTestPhysicalMemoryUsage -= cbBuffer;

    MmFreeContiguousMemory(pvBuffer);
}


/****************************************************************************
 *
 *  DirectSoundTestMemAlloc
 *
 *  Description:
 *      Allocates pool memory.
 *
 *  Arguments:
 *      ULONG [in]: buffer size, in bytes.
 *      DWORD [in]: pool tag.
 *      BOOL [in]: TRUE to zero-initialize the buffer data.
 *
 *  Returns:  
 *      LPVOID: allocation.
 *
 ****************************************************************************/

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestMemAlloc"

LPVOID 
DirectSoundTestMemAlloc
(
    DSOUND_POOL_TAG         nPoolTag,
    ULONG                   cbBuffer,
    BOOL                    fZeroInit
)
{
    static const ULONG      cbLowerThreshold    = 0x20;
    static const DWORD      cbUpperThreshold    = 0xFD8;
    LPVOID                  pvBuffer;

    ASSERT(cbBuffer);

#ifdef MEM_SIZE_CHECK

#pragma TODO("Don't ship with this check turned on")
    
    if((DSOUND_OBJECT_POOL_TAG == nPoolTag) && (cbBuffer >= cbUpperThreshold))
    {
        ASSERTMSG("Allocating an object larger than the pool threshold");
    }
    else if((DSOUND_DATA_POOL_TAG == nPoolTag) && (cbBuffer < cbLowerThreshold))
    {
        ASSERTMSG("Allocating a buffer smaller than 32 bytes");
    }

#endif

    if(g_pDirectSoundTestSlopMemoryHeap)
    {
        if(pvBuffer = g_pDirectSoundTestSlopMemoryHeap->Alloc(cbBuffer))
        {
            ASSERT(DSOUND_ALLOCATOR_SLOP == *((LPDWORD)pvBuffer - 1));

            if(fZeroInit)
            {
                ZeroMemory(pvBuffer, cbBuffer);
            }
        }
    }
    else
    {
        pvBuffer = NULL;
    }

    if(!pvBuffer)
    {
        if(pvBuffer = DirectSoundTestPoolAlloc(nPoolTag, cbBuffer + sizeof(DWORD), fZeroInit))
        {
            *(LPDWORD)pvBuffer = DSOUND_ALLOCATOR_POOL;
            pvBuffer = (LPDWORD)pvBuffer + 1;
        }
    }

    return pvBuffer;
}

#endif // ENABLE_SLOP_MEMORY_RECOVERY


/****************************************************************************
 *
 *  DirectSoundTestMemFree
 *
 *  Description:
 *      Frees memory allocated with DirectSoundTestMemAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestMemFree"

void 
DirectSoundTestMemFree
(
    LPVOID                  pvBuffer
)
{
    LPDWORD                 pdwSignature;
    DWORD                   cbBuffer;

    ASSERT(pvBuffer);

    pdwSignature = (LPDWORD)pvBuffer - 1;
    
    if(DSOUND_ALLOCATOR_POOL == *pdwSignature)
    {
        DirectSoundTestPoolFree(pdwSignature);
    }
    else if(DSOUND_ALLOCATOR_SLOP == *pdwSignature)
    {
        if(g_pDirectSoundTestSlopMemoryHeap)
        {
            g_pDirectSoundTestSlopMemoryHeap->Free(pvBuffer);
        }
        else
        {
            ASSERTMSG("Slop memory heap does not exist");
        }
    }
    else
    {
        ASSERTMSG("Invalid memory free");
    }
}

#endif // ENABLE_SLOP_MEMORY_RECOVERY

#ifdef DEBUG

/****************************************************************************
 *
 *  DirectSoundTestTrackAlloc
 *
 *  Description:
 *      Completes a memory tracking operation by adding the allocation to
 *      the list and fixing up the base pointer.
 *
 *  Arguments:
 *      LPVOID [in]: allocation base address.
 *      LPCSTR [in]: file the allocation comes from.
 *      ULONG [in]: line number the allocation comes from.
 *      LPCSTR [in]: class being allocated.
 *      DWORD [in]: allocator tag.
 *      ULONG [in]: size of allocation.
 *
 *  Returns:  
 *      LPVOID: allocation base address.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestTrackAlloc"

LPVOID
DirectSoundTestTrackAlloc
(
    LPVOID                  pvBaseAddress,
    ULONG                   cbTracking,
    LPCSTR                  pszFile,
    ULONG                   nLine,
    LPCSTR                  pszClass,
    DSOUND_ALLOCATOR_TAG    nAllocatorTag,
    ULONG                   cbSize
)
{
    LPDSMEMTRACK            pTracking;
    
    ASSERT(pvBaseAddress);
    ASSERT(cbTracking >= sizeof(*pTracking));
    ASSERT(cbSize);
    
    //
    // Because of alignment issues, the base address may be incremented more
    // than sizeof(DSMEMTRACK).  For this reason, we store the tracking data
    // directly behind the allocation, but allocate more than cbSize +
    // sizeof(DSMEMTRACK).
    //

    pTracking = (LPDSMEMTRACK)((LPBYTE)pvBaseAddress + cbTracking - sizeof(*pTracking));

    //
    // Create the tracking info
    //

    pTracking->pszFile = pszFile;
    pTracking->nLine = nLine;
    pTracking->pszClass = pszClass;
    pTracking->nAllocatorTag = nAllocatorTag;
    pTracking->cbSize = cbSize;
    pTracking->pvBaseAddress = pvBaseAddress;

    //
    // Raise IRQL for synchronization
    //

    AutoIrql();

    //
    // Add the allocation to the list
    //

    if(!g_lstDirectSoundTestMemoryTracking.Flink && !g_lstDirectSoundTestMemoryTracking.Blink)
    {
        InitializeListHead(&g_lstDirectSoundTestMemoryTracking);
    }
    
    InsertTailListUninit(&g_lstDirectSoundTestMemoryTracking, &pTracking->leListEntry);

    //
    // Fix up the base address
    //

    return pTracking + 1;
}


/****************************************************************************
 *
 *  DirectSoundTestForgetAlloc
 *
 *  Description:
 *      Removes a tracked allocation from the list.
 *
 *  Arguments:
 *      LPVOID [in]: allocation base address.
 *      DSOUND_ALLOCATOR_TAG [in]: allocator tag.
 *
 *  Returns:  
 *      LPVOID: allocation base address.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestForgetAlloc"

LPVOID
DirectSoundTestForgetAlloc
(
    LPVOID                  pvBaseAddress,
    DSOUND_ALLOCATOR_TAG    nAllocatorTag
)
{
    LPDSMEMTRACK            pTracking   = (LPDSMEMTRACK)pvBaseAddress - 1;

    ASSERT(pvBaseAddress);

    //
    // Assert that the tag matches what we expect
    //

    ASSERT(nAllocatorTag == pTracking->nAllocatorTag);
    
    //
    // Raise IRQL for synchronization
    //

    AutoIrql();

    //
    // Remove the allocation from the list
    //

    RemoveEntryList(&pTracking->leListEntry);

    //
    // Return the proper base address
    //

    return pTracking->pvBaseAddress;
}


/****************************************************************************
 *
 *  DirectSoundTestTrackingPoolAlloc
 *
 *  Description:
 *      Allocates pool memory.
 *
 *  Arguments:
 *      LPCSTR [in]: file the allocation comes from.
 *      ULONG [in]: line number the allocation comes from.
 *      LPCSTR [in]: class being allocated.
 *      ULONG [in]: buffer size, in bytes.
 *      DWORD [in]: pool tag.
 *      BOOL [in]: TRUE to zero-initialize the buffer data.
 *
 *  Returns:  
 *      LPVOID: allocation.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestTrackingPoolAlloc"

LPVOID 
DirectSoundTestTrackingPoolAlloc
(
    LPCSTR                  pszFile,
    ULONG                   nLine,
    LPCSTR                  pszClass,
    DSOUND_POOL_TAG         nPoolTag,
    ULONG                   cbBuffer,
    BOOL                    fZeroInit
)
{
    static const ULONG      cbTracking  = PAGE_SIZE;
    LPVOID                  pvBuffer;
    
    //
    // Do the actual allocation
    //
    
    if(pvBuffer = DirectSoundTestPoolAlloc(nPoolTag, cbBuffer + cbTracking, fZeroInit))
    {
        //
        // Complete the tracking
        //

        pvBuffer = DirectSoundTestTrackAlloc(pvBuffer, cbTracking, pszFile, nLine, pszClass, DSOUND_ALLOCATOR_POOL, cbBuffer);
    }

    return pvBuffer;
}


/****************************************************************************
 *
 *  DirectSoundTestTrackingPoolFree
 *
 *  Description:
 *      Frees memory allocated with DirectSoundTestTrackingPoolAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestTrackingPoolFree"

void 
DirectSoundTestTrackingPoolFree
(
    LPVOID                  pvBuffer
)
{
    //
    // Free tracking information
    //

    pvBuffer = DirectSoundTestForgetAlloc(pvBuffer, DSOUND_ALLOCATOR_POOL);

    //
    // Free memory
    //

    DirectSoundTestPoolFree(pvBuffer);
}


/****************************************************************************
 *
 *  DirectSoundTestTrackingPhysicalAlloc
 *
 *  Description:
 *      Allocates physically contiguous memory.
 *
 *  Arguments:
 *      ULONG [in]: buffer size, in bytes.
 *      ULONG [in]: buffer alignment, in bytes.
 *      DWORD [in]: allocation flags.
 *      BOOL [in]: TRUE to zero-initialize the buffer data.
 *
 *  Returns:  
 *      LPVOID: buffer pointer.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestTrackingPhysicalAlloc"

LPVOID 
DirectSoundTestTrackingPhysicalAlloc
(
    LPCSTR                  pszFile,
    ULONG                   nLine,
    LPCSTR                  pszClass,
    ULONG                   cbBuffer, 
    ULONG                   cbAlignment, 
    DWORD                   dwFlags,
    BOOL                    fZeroInit
)
{
    ULONG                   cbTracking;
    LPVOID                  pvBuffer;

    //
    // Make sure we allocate enough information for the tracking data
    //

    if(cbAlignment >= sizeof(DSMEMTRACK))
    {
        cbTracking = cbAlignment;
    }
    else
    {
        cbTracking = BLOCKALIGNPAD(sizeof(DSMEMTRACK), cbAlignment);
    }
    
    //
    // Do the actual allocation
    //
    
    if(pvBuffer = DirectSoundTestPhysicalAlloc(cbBuffer + cbTracking, cbAlignment, dwFlags, fZeroInit))
    {
        //
        // Complete the tracking
        //

        pvBuffer = DirectSoundTestTrackAlloc(pvBuffer, cbTracking, pszFile, nLine, pszClass, DSOUND_ALLOCATOR_PHYS, cbBuffer);
    }

    return pvBuffer;
}


/****************************************************************************
 *
 *  DirectSoundTestTrackingPhysicalFree
 *
 *  Description:
 *      Frees memory allocated with DirectSoundTestTrackingPhysicalAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestTrackingPhysicalFree"

void 
DirectSoundTestTrackingPhysicalFree
(
    LPVOID                  pvBuffer
)
{
    //
    // Free tracking information
    //

    pvBuffer = DirectSoundTestForgetAlloc(pvBuffer, DSOUND_ALLOCATOR_PHYS);

    //
    // Free memory
    //

    DirectSoundTestPhysicalFree(pvBuffer);
}


/****************************************************************************
 *
 *  DirectSoundTestTrackingMemAlloc
 *
 *  Description:
 *      Allocates pool memory.
 *
 *  Arguments:
 *      ULONG [in]: buffer size, in bytes.
 *      DWORD [in]: pool tag.
 *      BOOL [in]: TRUE to zero-initialize the buffer data.
 *
 *  Returns:  
 *      LPVOID: allocation.
 *
 ****************************************************************************/

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestTrackingMemAlloc"

LPVOID 
DirectSoundTestTrackingMemAlloc
(
    LPCSTR                  pszFile,
    ULONG                   nLine,
    LPCSTR                  pszClass,
    DSOUND_POOL_TAG         nPoolTag,
    ULONG                   cbBuffer,
    BOOL                    fZeroInit
)
{
    static const ULONG      cbTracking  = sizeof(DSMEMTRACK);
    LPVOID                  pvBuffer;
    
    //
    // Do the actual allocation
    //
    
    if(pvBuffer = DirectSoundTestMemAlloc(nPoolTag, cbBuffer + cbTracking, fZeroInit))
    {
        //
        // Complete the tracking
        //

        pvBuffer = DirectSoundTestTrackAlloc(pvBuffer, cbTracking, pszFile, nLine, pszClass, DSOUND_ALLOCATOR_SLOP, cbBuffer);
    }

    return pvBuffer;
}

#endif // ENABLE_SLOP_MEMORY_RECOVERY


/****************************************************************************
 *
 *  DirectSoundTestTrackingMemFree
 *
 *  Description:
 *      Frees memory allocated with DirectSoundTestTrackingMemAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestTrackingMemFree"

void 
DirectSoundTestTrackingMemFree
(
    LPVOID                  pvBuffer
)
{
    //
    // Free tracking information
    //

    pvBuffer = DirectSoundTestForgetAlloc(pvBuffer, DSOUND_ALLOCATOR_SLOP);

    //
    // Free memory
    //

    DirectSoundTestMemFree(pvBuffer);
}

#endif // ENABLE_SLOP_MEMORY_RECOVERY

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  DirectSoundTestDumpMemoryUsage
 *
 *  Description:
 *      Dumps current memory usage to the debugger.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to assert that there is no memory allocated.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundTestDumpMemoryUsage"

void
DirectSoundTestDumpMemoryUsage
(
    BOOL                    fAssertNone
)
{

#ifdef DEBUG

    static const LPCSTR     pszBanner           = "------------------------------------------------------------------------------";

#ifdef TRACK_MEMORY_USAGE

    PLIST_ENTRY             pleEntry;
    LPDSMEMTRACK            pTracking;

#endif // TRACK_MEMORY_USAGE

    DwDbgSetContext(DPRINTF_LIBRARY, DPFLVL_ABSOLUTE, NULL, 0, DPF_FNAME, DPF_LIBRARY);
    DwDbgPrint(pszBanner);
    DwDbgPrint("Begin memory usage dump");
    DwDbgPrint(pszBanner);
    
#ifdef TRACK_MEMORY_USAGE

    //
    // Raise IRQL for synchronization
    //

    AutoIrql();
   
    //
    // Show specifically what allocations are remaining
    //
    
    if(g_lstDirectSoundTestMemoryTracking.Flink)
    {
        if(!IsListEmpty(&g_lstDirectSoundTestMemoryTracking))
        {
            for(pleEntry = g_lstDirectSoundTestMemoryTracking.Flink; pleEntry != &g_lstDirectSoundTestMemoryTracking; pleEntry = pleEntry->Flink)
            {
                AssertValidEntryList(pleEntry, ASSERT_IN_LIST);
            
                pTracking = CONTAINING_RECORD(pleEntry, DSMEMTRACK, leListEntry);

                DwDbgSetContext(DPRINTF_FILELINE, DPFLVL_ABSOLUTE, pTracking->pszFile, pTracking->nLine, DPF_FNAME, DPF_LIBRARY);
                DwDbgPrint("%x (%lu bytes, type %s)", pTracking + 1, pTracking->cbSize, pTracking->pszClass);
            }

            DwDbgSetContext(DPRINTF_LIBRARY, DPFLVL_ABSOLUTE, NULL, 0, DPF_FNAME, DPF_LIBRARY);
            DwDbgPrint(pszBanner);
        }
    }

#endif // TRACK_MEMORY_USAGE

    DwDbgSetContext(DPRINTF_LIBRARY, DPFLVL_ABSOLUTE, NULL, 0, DPF_FNAME, DPF_LIBRARY);
    DwDbgPrint("Pool memory used:  %lu", g_dwDirectSoundTestPoolMemoryUsage);
    DwDbgPrint("Physical memory used:  %lu", g_dwDirectSoundTestPhysicalMemoryUsage);
    DwDbgPrint("Physical memory available for recovery:  %lu", g_dwDirectSoundTestPhysicalMemorySlop);
    DwDbgPrint("Physical memory recovered:  %lu", g_dwDirectSoundTestPhysicalMemoryRecovered);
    DwDbgPrint(pszBanner);
    DwDbgPrint("End memory usage dump");
    DwDbgPrint(pszBanner);

    if(g_dwDirectSoundTestPoolMemoryUsage || g_dwDirectSoundTestPhysicalMemoryUsage || g_dwDirectSoundTestPhysicalMemorySlop || g_dwDirectSoundTestPhysicalMemoryRecovered)
    {
        if(fAssertNone)
        {
            __asm int 3
        }

#ifdef TRACK_MEMORY_USAGE

        ASSERT(!IsListEmpty(&g_lstDirectSoundTestMemoryTracking));

#endif // TRACK_MEMORY_USAGE

    }

#ifdef TRACK_MEMORY_USAGE

    else
    {
        ASSERT(IsListEmpty(&g_lstDirectSoundTestMemoryTracking));
    }

#endif // TRACK_MEMORY_USAGE

#endif // DEBUG

}


