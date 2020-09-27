/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       memmgr.cpp
 *  Content:    Memory manager.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/21/00    dereks  Created.
 *
 ****************************************************************************/

#include "dscommon.h"

// #define MEM_SIZE_CHECK

#ifdef BUILDING_DSOUND

DWORD &CMemoryManager::m_dwPoolMemoryUsage = g_dwDirectSoundPoolMemoryUsage;
DWORD &CMemoryManager::m_dwPhysicalMemoryUsage = g_dwDirectSoundPhysicalMemoryUsage;

#else // BUILDING_DSOUND

DWORD CMemoryManager::m_dwPoolMemoryUsage = 0;
DWORD CMemoryManager::m_dwPhysicalMemoryUsage = 0;

#endif // BUILDING_DSOUND

#ifdef TRACK_MEMORY_USAGE

LIST_ENTRY CMemoryManager::m_lstMemoryTracking = { 0 };

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  PoolAlloc
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
#define DPF_FNAME "CMemoryManager::PoolAlloc"

LPVOID 
CMemoryManager::PoolAlloc
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

#ifdef _XBOX

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

#endif // _MEM_SIZE_CHECK

    if(pvBuffer = ExAllocatePoolWithTag(cbBuffer, nPoolTag))
    {
        cbBuffer = ExQueryPoolBlockSize(pvBuffer);
        m_dwPoolMemoryUsage += cbBuffer;

        if(fZeroInit)
        {
            ZeroMemory(pvBuffer, cbBuffer);
        }
    }

#else // _XBOX

    if(pvBuffer = LocalAlloc(fZeroInit ? LPTR : LMEM_FIXED, cbBuffer))
    {
        cbBuffer = LocalSize(pvBuffer);
        m_dwPoolMemoryUsage += cbBuffer;
    }

#endif // _XBOX

    else
    {
        DPF_RESOURCE("Out of pool memory");
    }

    return pvBuffer;
}


/****************************************************************************
 *
 *  PoolFree
 *
 *  Description:
 *      Frees memory allocated with PoolAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::PoolFree"

void 
CMemoryManager::PoolFree
(
    LPVOID                  pvBuffer
)
{
    DWORD                   cbBuffer;

    ASSERT(pvBuffer);

#ifdef _XBOX

    cbBuffer = ExQueryPoolBlockSize(pvBuffer);

    ASSERT(m_dwPoolMemoryUsage >= cbBuffer);
    m_dwPoolMemoryUsage -= cbBuffer;

    ExFreePool(pvBuffer);

#else // _XBOX

    cbBuffer = LocalSize(pvBuffer);

    ASSERT(m_dwPoolMemoryUsage >= cbBuffer);
    m_dwPoolMemoryUsage -= cbBuffer;

    LocalFree(pvBuffer);

#endif // _XBOX

}


/****************************************************************************
 *
 *  PhysicalAlloc
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
#define DPF_FNAME "CMemoryManager::PhysicalAlloc"

LPVOID 
CMemoryManager::PhysicalAlloc
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
    
#ifdef _XBOX

    if(pvBuffer = MmAllocateContiguousMemoryEx(cbBuffer, 0, 0xFFFFFFFF, cbAlignment, dwFlags))
    {
        cbBuffer = MmQueryAllocationSize(pvBuffer);
        m_dwPhysicalMemoryUsage += cbBuffer;

        if(fZeroInit)
        {
            ZeroMemory(pvBuffer, cbBuffer);
        }
    }

#else // _XBOX

    if(pvBuffer = VirtualAlloc(NULL, cbBuffer, MEM_COMMIT, dwFlags))
    {

#pragma TODO("Not able to query for size of VirtualAlloc'd buffer")

        if(fZeroInit)
        {
            ZeroMemory(pvBuffer, cbBuffer);
        }
    }

#endif // _XBOX

    else
    {
        DPF_RESOURCE("Out of contiguous memory");
    }

    return pvBuffer;
}


/****************************************************************************
 *
 *  PhysicalFree
 *
 *  Description:
 *      Frees memory allocated with PhysicalAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::PhysicalFree"

void 
CMemoryManager::PhysicalFree
(
    LPVOID                  pvBuffer
)
{
    DWORD                   cbBuffer;

    ASSERT(pvBuffer);

#ifdef _XBOX

    cbBuffer = MmQueryAllocationSize(pvBuffer);

    ASSERT(m_dwPhysicalMemoryUsage >= cbBuffer);
    m_dwPhysicalMemoryUsage -= cbBuffer;

    MmFreeContiguousMemory(pvBuffer);

#else // _XBOX

#pragma TODO("Not able to query for size of VirtualAlloc'd buffer")

    VirtualFree(pvBuffer, 0, MEM_RELEASE);

#endif // _XBOX

}


/****************************************************************************
 *
 *  MemAlloc
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
#define DPF_FNAME "CMemoryManager::MemAlloc"

LPVOID 
CMemoryManager::MemAlloc
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

#endif // MEM_SIZE_CHECK

    if(CMcpxSlopMemoryHeap::m_pSlopMemoryHeap)
    {
        if(pvBuffer = CMcpxSlopMemoryHeap::m_pSlopMemoryHeap->Alloc(cbBuffer))
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
        if(pvBuffer = PoolAlloc(nPoolTag, cbBuffer + sizeof(DWORD), fZeroInit))
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
 *  MemFree
 *
 *  Description:
 *      Frees memory allocated with MemAlloc.
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
#define DPF_FNAME "CMemoryManager::MemFree"

void 
CMemoryManager::MemFree
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
        PoolFree(pdwSignature);
    }
    else if(DSOUND_ALLOCATOR_SLOP == *pdwSignature)
    {
        if(CMcpxSlopMemoryHeap::m_pSlopMemoryHeap)
        {
            CMcpxSlopMemoryHeap::m_pSlopMemoryHeap->Free(pvBuffer);
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


/****************************************************************************
 *
 *  TrackAlloc
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

#ifdef TRACK_MEMORY_USAGE

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::TrackAlloc"

LPVOID
CMemoryManager::TrackAlloc
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

#ifdef _XBOX

    //
    // Raise IRQL for synchronization
    //

    AutoIrql();

#endif // _XBOX

    //
    // Add the allocation to the list
    //

    if(!m_lstMemoryTracking.Flink && !m_lstMemoryTracking.Blink)
    {
        InitializeListHead(&m_lstMemoryTracking);
    }
    
    InsertTailListUninit(&m_lstMemoryTracking, &pTracking->leListEntry);

    //
    // Fix up the base address
    //

    return pTracking + 1;
}

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  ForgetAlloc
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

#ifdef TRACK_MEMORY_USAGE

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::ForgetAlloc"

LPVOID
CMemoryManager::ForgetAlloc
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
    
#ifdef _XBOX

    //
    // Raise IRQL for synchronization
    //

    AutoIrql();

#endif // _XBOX

    //
    // Remove the allocation from the list
    //

    RemoveEntryList(&pTracking->leListEntry);

    //
    // Return the proper base address
    //

    return pTracking->pvBaseAddress;
}

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  TrackingPoolAlloc
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

#ifdef TRACK_MEMORY_USAGE

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::TrackingPoolAlloc"

LPVOID 
CMemoryManager::TrackingPoolAlloc
(
    LPCSTR                  pszFile,
    ULONG                   nLine,
    LPCSTR                  pszClass,
    DSOUND_POOL_TAG         nPoolTag,
    ULONG                   cbBuffer,
    BOOL                    fZeroInit
)
{

#ifdef _XBOX

    static const ULONG      cbTracking  = PAGE_SIZE;

#else // _XBOX

    static const ULONG      cbTracking  = sizeof(DSMEMTRACK);

#endif // _XBOX

    LPVOID                  pvBuffer;
    
    //
    // Do the actual allocation
    //
    
    if(pvBuffer = PoolAlloc(nPoolTag, cbBuffer + cbTracking, fZeroInit))
    {
        //
        // Complete the tracking
        //

        pvBuffer = TrackAlloc(pvBuffer, cbTracking, pszFile, nLine, pszClass, DSOUND_ALLOCATOR_POOL, cbBuffer);
    }

    return pvBuffer;
}

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  TrackingPoolFree
 *
 *  Description:
 *      Frees memory allocated with TrackingPoolAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef TRACK_MEMORY_USAGE

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::TrackingPoolFree"

void 
CMemoryManager::TrackingPoolFree
(
    LPVOID                  pvBuffer
)
{
    //
    // Free tracking information
    //

    pvBuffer = ForgetAlloc(pvBuffer, DSOUND_ALLOCATOR_POOL);

    //
    // Free memory
    //

    PoolFree(pvBuffer);
}

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  TrackingPhysicalAlloc
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

#ifdef TRACK_MEMORY_USAGE

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::TrackingPhysicalAlloc"

LPVOID 
CMemoryManager::TrackingPhysicalAlloc
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
    
    if(pvBuffer = PhysicalAlloc(cbBuffer + cbTracking, cbAlignment, dwFlags, fZeroInit))
    {
        //
        // Complete the tracking
        //

        pvBuffer = TrackAlloc(pvBuffer, cbTracking, pszFile, nLine, pszClass, DSOUND_ALLOCATOR_PHYS, cbBuffer);
    }

    return pvBuffer;
}

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  TrackingPhysicalFree
 *
 *  Description:
 *      Frees memory allocated with TrackingPhysicalAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef TRACK_MEMORY_USAGE

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::TrackingPhysicalFree"

void 
CMemoryManager::TrackingPhysicalFree
(
    LPVOID                  pvBuffer
)
{
    //
    // Free tracking information
    //

    pvBuffer = ForgetAlloc(pvBuffer, DSOUND_ALLOCATOR_PHYS);

    //
    // Free memory
    //

    PhysicalFree(pvBuffer);
}

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  TrackingMemAlloc
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

#if defined(TRACK_MEMORY_USAGE) && defined(ENABLE_SLOP_MEMORY_RECOVERY)

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::TrackingMemAlloc"

LPVOID 
CMemoryManager::TrackingMemAlloc
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
    
    if(pvBuffer = MemAlloc(nPoolTag, cbBuffer + cbTracking, fZeroInit))
    {
        //
        // Complete the tracking
        //

        pvBuffer = TrackAlloc(pvBuffer, cbTracking, pszFile, nLine, pszClass, DSOUND_ALLOCATOR_SLOP, cbBuffer);
    }

    return pvBuffer;
}

#endif // defined(TRACK_MEMORY_USAGE) && defined(ENABLE_SLOP_MEMORY_RECOVERY)


/****************************************************************************
 *
 *  TrackingMemFree
 *
 *  Description:
 *      Frees memory allocated with TrackingMemAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#if defined(TRACK_MEMORY_USAGE) && defined(ENABLE_SLOP_MEMORY_RECOVERY)

#undef DPF_FNAME
#define DPF_FNAME "CMemoryManager::TrackingMemFree"

void 
CMemoryManager::TrackingMemFree
(
    LPVOID                  pvBuffer
)
{
    //
    // Free tracking information
    //

    pvBuffer = ForgetAlloc(pvBuffer, DSOUND_ALLOCATOR_SLOP);

    //
    // Free memory
    //

    MemFree(pvBuffer);
}

#endif // defined(TRACK_MEMORY_USAGE) && defined(ENABLE_SLOP_MEMORY_RECOVERY)


/****************************************************************************
 *
 *  DumpMemoryUsage
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
#define DPF_FNAME "CMemoryManager::DumpMemoryUsage"

void
CMemoryManager::DumpMemoryUsage
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

    CDebug::SetContext(DPF_FLAGS_LIBRARY, DPFLVL_ABSOLUTE, NULL, 0, DPF_FNAME, DPF_LIBRARY);
    CDebug::Print(pszBanner);
    CDebug::Print("Begin memory usage dump");
    CDebug::Print(pszBanner);
    
#ifdef TRACK_MEMORY_USAGE

#ifdef _XBOX

    //
    // Raise IRQL for synchronization
    //

    AutoIrql();

#endif // _XBOX
   
    //
    // Show specifically what allocations are remaining
    //
    
    if(m_lstMemoryTracking.Flink)
    {
        if(!IsListEmpty(&m_lstMemoryTracking))
        {
            for(pleEntry = m_lstMemoryTracking.Flink; pleEntry != &m_lstMemoryTracking; pleEntry = pleEntry->Flink)
            {
                AssertValidEntryList(pleEntry, ASSERT_IN_LIST);
            
                pTracking = CONTAINING_RECORD(pleEntry, DSMEMTRACK, leListEntry);

                CDebug::SetContext(DPF_FLAGS_FILELINE, DPFLVL_ABSOLUTE, pTracking->pszFile, pTracking->nLine, DPF_FNAME, DPF_LIBRARY);
                CDebug::Print("%x (%lu bytes, type %s)", pTracking + 1, pTracking->cbSize, pTracking->pszClass);
            }

            CDebug::SetContext(DPF_FLAGS_LIBRARY, DPFLVL_ABSOLUTE, NULL, 0, DPF_FNAME, DPF_LIBRARY);
            CDebug::Print(pszBanner);
        }
    }

#endif // TRACK_MEMORY_USAGE

    CDebug::SetContext(DPF_FLAGS_LIBRARY, DPFLVL_ABSOLUTE, NULL, 0, DPF_FNAME, DPF_LIBRARY);
    CDebug::Print("Pool memory used:  %lu", m_dwPoolMemoryUsage);
    CDebug::Print("Physical memory used:  %lu", m_dwPhysicalMemoryUsage);

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

    CDebug::Print("Physical memory available for recovery:  %lu", CMcpxSlopMemoryHeap::m_dwAvailable);
    CDebug::Print("Physical memory recovered:  %lu", CMcpxSlopMemoryHeap::m_dwUsed);

#endif // ENABLE_SLOP_MEMORY_RECOVERY

    CDebug::Print(pszBanner);
    CDebug::Print("End memory usage dump");
    CDebug::Print(pszBanner);

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

    if(m_dwPoolMemoryUsage || m_dwPhysicalMemoryUsage || CMcpxSlopMemoryHeap::m_dwAvailable || CMcpxSlopMemoryHeap::m_dwUsed)

#else // ENABLE_SLOP_MEMORY_RECOVERY

    if(m_dwPoolMemoryUsage || m_dwPhysicalMemoryUsage)

#endif // ENABLE_SLOP_MEMORY_RECOVERY

    {
        if(fAssertNone)
        {
            __asm int 3
        }

#ifdef TRACK_MEMORY_USAGE

        ASSERT(!IsListEmpty(&m_lstMemoryTracking));

#endif // TRACK_MEMORY_USAGE

    }

#ifdef TRACK_MEMORY_USAGE

    else
    {
        ASSERT(IsListEmpty(&m_lstMemoryTracking));
    }

#endif // TRACK_MEMORY_USAGE

#endif // DEBUG

}


