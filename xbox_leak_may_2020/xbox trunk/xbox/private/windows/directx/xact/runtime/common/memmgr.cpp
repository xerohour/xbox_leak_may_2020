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

#include "common.h"


#ifdef TRACK_MEMORY_USAGE

//
// Allocation tracking data
//

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY              leListEntry;
    LPCSTR                  pszFile;
    ULONG                   nLine;
    LPCSTR                  pszClass;
    ULONG                   cbSize;
    LPVOID                  pvBaseAddress;
END_DEFINE_STRUCT(MEMTRACK);

LIST_ENTRY g_lstMemoryTracking;

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  MemAlloc
 *
 *  Description:
 *      Allocates memory.
 *
 *  Arguments:
 *      ULONG [in]: buffer size, in bytes.
 *      BOOL [in]: TRUE to zero-initialize the buffer data.
 *
 *  Returns:  
 *      LPVOID: allocation.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "XactMemAlloc"

LPVOID 
XactMemAlloc
(
    ULONG                   cbBuffer,
    BOOL                    fZeroInit
)
{
    LPVOID                  pvBuffer;

    using namespace XACT;
    ASSERT(cbBuffer);

    pvBuffer = ExAllocatePoolWithTag(cbBuffer,'tcax');
    memset(pvBuffer,0,cbBuffer);

    return pvBuffer;
}

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

#undef DPF_FNAME
#define DPF_FNAME "XactMemFree"

void 
XactMemFree
(
    LPVOID                  pvBuffer
)
{
    using namespace XACT;
    ASSERT(pvBuffer);	
	ExFreePool(pvBuffer);
}

/****************************************************************************
 *
 *  TrackMemAlloc
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
#define DPF_FNAME "TrackMemAlloc"

LPVOID
XactTrackMemAlloc
(
    LPVOID                  pvBaseAddress,
    ULONG                   cbTracking,
    LPCSTR                  pszFile,
    ULONG                   nLine,
    LPCSTR                  pszClass,
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
    pTracking->cbSize = cbSize;
    pTracking->pvBaseAddress = pvBaseAddress;

    //
    // Raise IRQL for synchronization
    //

    AutoIrql();

    //
    // Add the allocation to the list
    //

    if(!g_lstMemoryTracking.Flink && !g_lstMemoryTracking.Blink)
    {
        InitializeListHead(&g_lstMemoryTracking);
    }
    
    InsertTailListUninit(&g_lstMemoryTracking, &pTracking->leListEntry);

    //
    // Fix up the base address
    //

    return pTracking + 1;
}

#endif // TRACK_MEMORY_USAGE


/****************************************************************************
 *
 *  DirectSoundForgetAlloc
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
#define DPF_FNAME "ForgetAlloc"

LPVOID
XactForgetAlloc
(
    LPVOID                  pvBaseAddress,
)
{
    LPDSMEMTRACK            pTracking   = (LPDSMEMTRACK)pvBaseAddress - 1;

    ASSERT(pvBaseAddress);

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

#endif // TRACK_MEMORY_USAGE




/****************************************************************************
 *
 *  TrackMemFree
 *
 *  Description:
 *      Frees memory allocated with TrackMemAlloc.
 *
 *  Arguments:
 *      LPVOID [in]: buffer pointer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#if defined(TRACK_MEMORY_USAGE)

#undef DPF_FNAME
#define DPF_FNAME "TrackMemFree"

void 
XactTrackMemFree
(
    LPVOID                  pvBuffer
)
{
    //
    // Free tracking information
    //

    pvBuffer = ForgetAlloc(pvBuffer);

    //
    // Free memory
    //

    MemFree(pvBuffer);
}

#endif // defined(TRACK_MEMORY_USAGE)


/****************************************************************************
 *
 *  DirectSoundDumpMemoryUsage
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
#define DPF_FNAME "DumpMemoryUsage"

void
DumpMemoryUsage
(
    BOOL                    fAssertNone
)
{

    using namespace XACT;
#ifdef DEBUG

    static const LPCSTR     pszBanner           = "------------------------------------------------------------------------------";

#ifdef TRACK_MEMORY_USAGE

    PLIST_ENTRY             pleEntry;
    LPDSMEMTRACK            pTracking;

#endif // TRACK_MEMORY_USAGE

    DwDbgSetContext(DPF_FLAGS_LIBRARY, DPFLVL_ABSOLUTE, NULL, 0, DPF_FNAME, DPF_LIBRARY);
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
    
    if(g_lstMemoryTracking.Flink)
    {
        if(!IsListEmpty(&g_lstMemoryTracking))
        {
            for(pleEntry = g_lstMemoryTracking.Flink; pleEntry != &g_lstMemoryTracking; pleEntry = pleEntry->Flink)
            {
                AssertValidEntryList(pleEntry, ASSERT_IN_LIST);
            
                pTracking = CONTAINING_RECORD(pleEntry, DSMEMTRACK, leListEntry);

                DwDbgSetContext(DPF_FLAGS_FILELINE, DPFLVL_ABSOLUTE, pTracking->pszFile, pTracking->nLine, DPF_FNAME, DPF_LIBRARY);
                DwDbgPrint("%x (%lu bytes, type %s)", pTracking + 1, pTracking->cbSize, pTracking->pszClass);
            }

            DwDbgSetContext(DPF_FLAGS_LIBRARY, DPFLVL_ABSOLUTE, NULL, 0, DPF_FNAME, DPF_LIBRARY);
            DwDbgPrint(pszBanner);
        }
    }

#endif // TRACK_MEMORY_USAGE


    DwDbgPrint(pszBanner);
    DwDbgPrint("End memory usage dump");
    DwDbgPrint(pszBanner);

#endif // DEBUG

}


