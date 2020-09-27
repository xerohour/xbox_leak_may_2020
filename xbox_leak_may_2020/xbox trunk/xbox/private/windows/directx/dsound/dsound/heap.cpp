/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       heap.cpp
 *  Content:    Heap implementation objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/24/01    dereks  Created.
 *
 ****************************************************************************/

// #define MCPX_NO_SGE_SHARING

#include "dsoundi.h"

#define DPF_SGE_HEAP DPF_BLAB
#define DPF_MEM_HEAP DPF_BLAB

const DWORD CMcpxSlopMemoryHeap::m_dwUsageThreshold = 64;

CMcpxSlopMemoryHeap *CMcpxSlopMemoryHeap::m_pSlopMemoryHeap = NULL;

DWORD &CMcpxSlopMemoryHeap::m_dwAvailable = g_dwDirectSoundPhysicalMemorySlop;
DWORD &CMcpxSlopMemoryHeap::m_dwUsed = g_dwDirectSoundPhysicalMemoryRecovered;


/****************************************************************************
 *
 *  CMcpxBufferSgeHeap
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::CMcpxBufferSgeHeap"

CMcpxBufferSgeHeap::CMcpxBufferSgeHeap
(
    void
)
:   m_nFreeElementCount(g_dwDirectSoundFreeBufferSGEs)
{
    DPF_ENTER();

    InitializeListHead(&m_lstRuns);
    InitializeListHead(&m_lstMarkers);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CMcpxBufferSgeHeap
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::~CMcpxBufferSgeHeap"

CMcpxBufferSgeHeap::~CMcpxBufferSgeHeap
(
    void
)
{
    DPF_ENTER();

    MEMFREE(m_paMarkers);

    m_nFreeElementCount = 0;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      WORD [in]: count of elements managed by the heap.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::Initialize"

HRESULT
CMcpxBufferSgeHeap::Initialize
(
    WORD                    nLength
)
{
    HRESULT                 hr;
    DWORD                   i;

    m_nFreeElementCount = nLength;

    //
    // Allocate markers, include one extra marker for slop
    //

    hr = HRFROMP(m_paMarkers = MEMALLOC(SGEHEAPRUNMARKER, nLength + 1));

    //
    // All elements are initially free, so add 1 run marker to the run list
    //

    if(SUCCEEDED(hr))
    {
        m_pLargestFreeRunMarker = CreateMarker(&m_paMarkers[0], 0, nLength, &m_lstRuns);
    }

    //
    // The remaining markers are unused
    //

    if(SUCCEEDED(hr))
    {
        for(i = 1; i < (DWORD)nLength + 1; i++)
        {
            InsertTailListUninit(&m_lstMarkers, &m_paMarkers[i].leListEntry);
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Alloc
 *
 *  Description:
 *      Allocates a run of elements.
 *
 *  Arguments:
 *      LPVOID [in]: buffer.
 *      DWORD [in]: buffer size, in bytes.
 *
 *  Returns:  
 *      LPSGEHEAPRUNMARKER: run marker or NULL if the request could not be
 *                          completed.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::Alloc"

LPSGEHEAPRUNMARKER
CMcpxBufferSgeHeap::Alloc
(
    LPVOID                  pvBaseAddress,
    DWORD                   dwBufferSize
)
{
    LPSGEHEAPRUNMARKER      pRunMarker      = NULL;
    PLIST_ENTRY             pleEntry;
    LPSGEHEAPRUNMARKER      pMarkerNode;
    WORD                    nLength;
    CIrql                   irql;
    
    DPF_ENTER();

    //
    // Page-align the buffer address and size
    //

    nLength = (WORD)COMPUTE_PAGES_SPANNED(pvBaseAddress, dwBufferSize);
    pvBaseAddress = (LPVOID)((DWORD)pvBaseAddress & ~(PAGE_SIZE - 1));

    //
    // Check the list for a buffer whose SGEs we can share
    //

    irql.Raise();

#ifndef MCPX_NO_SGE_SHARING

    for(pleEntry = m_lstRuns.Flink; pleEntry != &m_lstRuns; pleEntry = pleEntry->Flink)
    {
        AssertValidEntryList(pleEntry, ASSERT_IN_LIST);

        pMarkerNode = CONTAINING_RECORD(pleEntry, SGEHEAPRUNMARKER, leListEntry);

        if(pMarkerNode->dwRefCount)
        {
            if((DWORD)pvBaseAddress == (DWORD)pMarkerNode->pvBaseAddress)
            {
                if(nLength == pMarkerNode->nLength)
                {
                    pRunMarker = pMarkerNode;
                    break;
                }
            }
        }
    }

    if(pRunMarker)
    {
        //
        // We found one, so just AddRef it
        //

        ASSERT(pRunMarker->dwRefCount < MCPX_HW_MAX_VOICES);

        pRunMarker->dwRefCount++;

        DPF_SGE_HEAP("Sharing SGE run %x (ref count %lu)", pRunMarker->nElement, pRunMarker->dwRefCount);
    }
    else

#endif // MCPX_NO_SGE_SHARING

    {
        //
        // Allocate a new run and map it into SGEs.  Note that we're not 
        // setting the run marker's base address until after it's been
        // mapped.  This will prevent us from ever trying to use an unmapped
        // run if a higher IRQL call comes in.
        //
        
        if(pRunMarker = AllocRun(pvBaseAddress, nLength))
        {
            irql.Lower();

            MapBuffer(pRunMarker->nElement, pvBaseAddress, nLength);
        }
    }

    irql.Lower();

    DPF_LEAVE(pRunMarker);

    return pRunMarker;
}


/****************************************************************************
 *
 *  Free
 *
 *  Description:
 *      Frees a previously allocated run.
 *
 *  Arguments:
 *      LPSGEHEAPRUNMARKER [in]: run marker.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::Free"

void
CMcpxBufferSgeHeap::Free
(
    LPSGEHEAPRUNMARKER      pRunMarker
)
{
    LPVOID                  pvBaseAddress;
    DWORD                   dnLength;
    CIrql                   irql;
    
    DPF_ENTER();

    irql.Raise();
    
    if(!--pRunMarker->dwRefCount)
    {
        pvBaseAddress = pRunMarker->pvBaseAddress;
        dnLength = pRunMarker->nLength;
        
        FreeRun(pRunMarker);

        irql.Lower();

        UnmapBuffer(pvBaseAddress, dnLength);
    }
    
    irql.Lower();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Alloc
 *
 *  Description:
 *      Allocates a run of elements.
 *
 *  Arguments:
 *      LPVOID [in]: buffer.
 *      WORD [in]: element count.
 *
 *  Returns:  
 *      LPSGEHEAPRUNMARKER: run marker or NULL if the request could not be 
 *                          completed.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::AllocRun"

LPSGEHEAPRUNMARKER
CMcpxBufferSgeHeap::AllocRun
(
    LPVOID                  pvBaseAddress,
    WORD                    nLength
)
{
    LPSGEHEAPRUNMARKER      pRunMarker  = NULL;
    LPSGEHEAPRUNMARKER      pMarkerNode;
    PLIST_ENTRY             pleEntry;

    DPF_ENTER();

    //
    // Check the largest available run length to see if we can service
    // the request
    //

    if(m_pLargestFreeRunMarker)
    {
        if(m_pLargestFreeRunMarker->nLength >= nLength)
        {
            //
            // Find a free run that is as close as possible to the requested element
            // count.  This will help to reduce fragmentation.
            //

            for(pleEntry = m_lstRuns.Flink; pleEntry != &m_lstRuns; pleEntry = pleEntry->Flink)
            {
                AssertValidEntryList(pleEntry, ASSERT_IN_LIST);

                pMarkerNode = CONTAINING_RECORD(pleEntry, SGEHEAPRUNMARKER, leListEntry);
            
                if(!pMarkerNode->fAllocated)
                {
                    if(pMarkerNode->nLength >= nLength)
                    {
                        if(!pRunMarker)
                        {
                            pRunMarker = pMarkerNode;
                        }
                        else if(pMarkerNode->nLength < pRunMarker->nLength)
                        {
                            pRunMarker = pMarkerNode;
                        }
                    }
                }
            }

            //
            // If we're using the whole run, just flag it as allocated.  If not,
            // get a free marker from the pool and add it to the list.  We're only
            // using a single list so that runs can be broken and coalesced without
            // having to walk the list.
            //

            if(pRunMarker)
            {
                if(nLength < pRunMarker->nLength)
                {
                    ASSERT(!IsListEmpty(&m_lstMarkers));
            
                    pleEntry = RemoveHeadList(&m_lstMarkers);
                    pMarkerNode = CONTAINING_RECORD(pleEntry, SGEHEAPRUNMARKER, leListEntry);

                    CreateMarker(pMarkerNode, pRunMarker->nElement + nLength, pRunMarker->nLength - nLength, &pRunMarker->leListEntry);

                    pRunMarker->nLength = nLength;

                    DPF_SGE_HEAP("Split SGE run into %x (%lu) and %x (%lu)", pRunMarker->nElement, pRunMarker->nLength, pMarkerNode->nElement, pMarkerNode->nLength);
                }
                else
                {
                    DPF_SGE_HEAP("Allocating SGE run %x (%lu)", pRunMarker->nElement, pRunMarker->nLength);
                }

                pRunMarker->fAllocated = TRUE;
                pRunMarker->pvBaseAddress = pvBaseAddress;
            }

            //
            // Update the free element count and largest run members
            //

            if(pRunMarker)
            {
                ASSERT(m_nFreeElementCount >= pRunMarker->nLength);
                m_nFreeElementCount -= pRunMarker->nLength;

                if(pRunMarker == m_pLargestFreeRunMarker)
                {
                    m_pLargestFreeRunMarker = NULL;
                    
                    for(pleEntry = m_lstRuns.Flink; pleEntry != &m_lstRuns; pleEntry = pleEntry->Flink)
                    {
                        AssertValidEntryList(pleEntry, ASSERT_IN_LIST);

                        pMarkerNode = CONTAINING_RECORD(pleEntry, SGEHEAPRUNMARKER, leListEntry);
            
                        if(!pMarkerNode->fAllocated)
                        {
                            if(!m_pLargestFreeRunMarker)
                            {
                                m_pLargestFreeRunMarker = pMarkerNode;
                            }
                            else if(pMarkerNode->nLength > m_pLargestFreeRunMarker->nLength)
                            {
                                m_pLargestFreeRunMarker = pMarkerNode;
                            }
                        }
                    }
                }                    
            }
        }
    }

    DPF_LEAVE(pRunMarker);

    return pRunMarker;
}


/****************************************************************************
 *
 *  Free
 *
 *  Description:
 *      Frees a previously allocated run.
 *
 *  Arguments:
 *      LPSGEHEAPRUNMARKER [in]: run marker.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::FreeRun"

void
CMcpxBufferSgeHeap::FreeRun
(
    LPSGEHEAPRUNMARKER      pRunMarker
)
{
    PLIST_ENTRY             pleEntry;
    LPSGEHEAPRUNMARKER      pMarkerNode;
    
    DPF_ENTER();

    ASSERT(!pRunMarker->fAllocated);

    DPF_SGE_HEAP("Freeing SGE run %x (%lu)", pRunMarker->nElement, pRunMarker->nLength);

    //
    // Update the free element count
    //

    m_nFreeElementCount += pRunMarker->nLength;

    //
    // Coalesce the run with those around it and add it back to the free 
    // marker pool.
    //

    while(TRUE)
    {
        if((pleEntry = pRunMarker->leListEntry.Flink) == &m_lstRuns)
        {
            break;
        }

        pMarkerNode = CONTAINING_RECORD(pleEntry, SGEHEAPRUNMARKER, leListEntry);

        if(pMarkerNode->fAllocated)
        {
            break;
        }

        pRunMarker = CoalesceRuns(pRunMarker, pMarkerNode);
    }

    while(TRUE)
    {
        if((pleEntry = pRunMarker->leListEntry.Blink) == &m_lstRuns)
        {
            break;
        }

        pMarkerNode = CONTAINING_RECORD(pleEntry, SGEHEAPRUNMARKER, leListEntry);

        if(pMarkerNode->fAllocated)
        {
            break;
        }

        pRunMarker = CoalesceRuns(pMarkerNode, pRunMarker);
    }

    //
    // Update the largest run pointer
    //

    if(m_pLargestFreeRunMarker)
    {
        if(pRunMarker->nLength > m_pLargestFreeRunMarker->nLength)
        {
            m_pLargestFreeRunMarker = pRunMarker;
        }
    }
    else
    {
        m_pLargestFreeRunMarker = pRunMarker;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CoalesceRuns
 *
 *  Description:
 *      Coalesces two adjacent runs.
 *
 *  Arguments:
 *      LPSGEHEAPRUNMARKER [in]: run marker.
 *      LPSGEHEAPRUNMARKER [in]: next run marker in the list (this one will
 *                               be removed).
 *
 *  Returns:  
 *      LPSGEHEAPRUNMARKER: run marker.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::CoalesceRuns"

LPSGEHEAPRUNMARKER
CMcpxBufferSgeHeap::CoalesceRuns
(
    LPSGEHEAPRUNMARKER      pRunMarker,
    LPSGEHEAPRUNMARKER      pNextRunMarker
)
{
    DPF_ENTER();

    ASSERT(pRunMarker->nElement + pRunMarker->nLength == pNextRunMarker->nElement);

    AssertValidEntryList(&pRunMarker->leListEntry, ASSERT_IN_LIST);
    AssertValidEntryList(&pNextRunMarker->leListEntry, ASSERT_IN_LIST);

    DPF_SGE_HEAP("Coalescing SGE run %x (%lu) and %x (%lu)", pRunMarker->nElement, pRunMarker->nLength, pNextRunMarker->nElement, pNextRunMarker->nLength);

    pRunMarker->nLength += pNextRunMarker->nLength;

    MoveEntryTailList(&m_lstMarkers, &pNextRunMarker->leListEntry);

    DPF_LEAVE(pRunMarker);

    return pRunMarker;
}


/****************************************************************************
 *
 *  MapBuffer
 *
 *  Description:
 *      Maps a data buffer into SGEs.
 *
 *  Arguments:
 *      DWORD [in]: base SGE index.
 *      LPVOID [in]: data buffer.  This address must be page-aligned.
 *      DWORD [in]: data buffer size, in pages.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::MapBuffer"

void
CMcpxBufferSgeHeap::MapBuffer
(
    DWORD                   dwSgeIndex,
    LPVOID                  pvBaseAddress,
    DWORD                   dnLength
)
{
    DWORD                   dwAddress;
    CIrql                   irql;
    
    DPF_ENTER();

    ASSERT(dwSgeIndex < MCPX_HW_MAX_BUFFER_PRDS);
    ASSERT(pvBaseAddress);
    ASSERT(!((DWORD)pvBaseAddress & (PAGE_SIZE - 1)));
    ASSERT(dnLength);

    //
    // Lock the buffer data
    //

    MmLockUnlockBufferPages(pvBaseAddress, dnLength << PAGE_SHIFT, FALSE);

    //
    // Start mapping pages into SGEs
    //

    while(dnLength--)
    {
        dwAddress = MmGetPhysicalAddress(pvBaseAddress);
        ASSERT(!(dwAddress & (PAGE_SIZE - 1)));

        irql.Raise();
        
        MCPX_CHECK_VOICE_FIFO(2);

        MCPX_VOICE_WRITE(SetCurrentInBufSGE, dwSgeIndex);
        MCPX_VOICE_WRITE(SetCurrentInBufSGEOffset, dwAddress);

        irql.Lower();

        pvBaseAddress = (LPBYTE)pvBaseAddress + PAGE_SIZE;
        
        dwSgeIndex++;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  UnmapBuffer
 *
 *  Description:
 *      Unmaps a buffer mapped by MapBuffer.
 *
 *  Arguments:
 *      DWORD [in]: data buffer.  This address must be page-aligned.
 *      DWORD [in]: data buffer size, in pages.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBufferSgeHeap::UnmapBuffer"

void
CMcpxBufferSgeHeap::UnmapBuffer
(
    LPVOID                  pvBaseAddress,
    DWORD                   dnLength
)
{
    DPF_ENTER();

    ASSERT(pvBaseAddress);
    ASSERT(!((DWORD)pvBaseAddress & (PAGE_SIZE - 1)));
    ASSERT(dnLength);

    //
    // Unlock the buffer data
    //

    MmLockUnlockBufferPages(pvBaseAddress, dnLength << PAGE_SHIFT, TRUE);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CMcpxSlopMemoryHeap
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxSlopMemoryHeap::CMcpxSlopMemoryHeap"

CMcpxSlopMemoryHeap::CMcpxSlopMemoryHeap
(
    void
)
{
    DPF_ENTER();

    ASSERT(!m_dwAvailable);
    ASSERT(!m_dwUsed);

    ASSERT(!m_pSlopMemoryHeap);
    m_pSlopMemoryHeap = this;

    InitializeListHead(&m_lstEntries);
    InitializeListHead(&m_lstRuns);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CMcpxSlopMemoryHeap
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxSlopMemoryHeap::~CMcpxSlopMemoryHeap"

CMcpxSlopMemoryHeap::~CMcpxSlopMemoryHeap
(
    void
)
{

#ifdef DEBUG

    LPSLOPRUNMARKER         pMarkerNode;

#endif // DEBUG

    PLIST_ENTRY             pleEntry;
    LPSLOPMEMENTRY          pEntryNode;
    LPVOID                  pvBaseAddress;

    DPF_ENTER();
    AutoIrql();

#ifdef DEBUG

    // 
    // Make sure no memory is still being used
    //
    
    for(pleEntry = m_lstRuns.Flink; pleEntry != &m_lstRuns; pleEntry = pleEntry->Flink)
    {
        pMarkerNode = CONTAINING_RECORD(pleEntry, SLOPRUNMARKER, leListEntry);

        ASSERT(!pMarkerNode->fAllocated);
    }

#endif // DEBUG

    //
    // Free the global pointer
    //
    
    ASSERT(this == m_pSlopMemoryHeap);
    m_pSlopMemoryHeap = NULL;

    //
    // Free memory owned by the heap
    //

    while((pleEntry = RemoveHeadList(&m_lstEntries)) != &m_lstEntries)
    {
        pEntryNode = CONTAINING_RECORD(pleEntry, SLOPMEMENTRY, leListEntry);
        pvBaseAddress = pEntryNode->pvBaseAddress;
        
        PHYSFREE(pvBaseAddress);
    }

    //
    // Reset tracking values
    //

    ASSERT(!m_dwUsed);

    m_dwAvailable = 0;
    m_dwUsed = 0;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  AddRun
 *
 *  Description:
 *      Adds a run of slop memory to the heap.
 *
 *  Arguments:
 *      LPVOID [in]: base address.
 *      DWORD [in]: length, in bytes, of the allocation.
 *      DWORD [in]: length, in bytes, of the allocation that's actually being
 *                  used.
 *
 *  Returns:  
 *      BOOL: TRUE if the run was added to the heap.  If the run was added,
 *            the heap now owns the memory, so it should not be freed by the
 *            caller.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxSlopMemoryHeap::AddRun"

BOOL
CMcpxSlopMemoryHeap::AddRun
(
    LPVOID                  pvBaseAddress,
    DWORD                   nLength,
    DWORD                   nUsed
)
{
    DWORD                   nSlopLength;
    LPSLOPRUNMARKER         pRunMarker;
    LPSLOPMEMENTRY          pEntry;
    BOOL                    fOwned;
    
    DPF_ENTER();
    AutoIrql();

    //
    // Make sure the run is large enough to hold the tracking entry, at least
    // one run marker and still have some space left over for data.
    //

    ASSERT(nLength >= nUsed);
    nSlopLength = nLength - nUsed;

    if(fOwned = (nSlopLength > sizeof(*pEntry) + sizeof(*pRunMarker) + m_dwUsageThreshold))
    {
        //
        // Add the allocation to the entry list so we can free it later.  
        // We'll use the first part of the unused memory as the entry marker
        //

        pEntry = (LPSLOPMEMENTRY)((LPBYTE)pvBaseAddress + nUsed);

        pEntry->pvBaseAddress = pvBaseAddress;

        InsertTailListUninit(&m_lstEntries, &pEntry->leListEntry);

        //
        // Initialize a run marker immediately after the tracking entry and
        // add it to the run list.
        //

        pRunMarker = CreateMarker(pEntry + 1, nSlopLength - sizeof(*pEntry), &m_lstRuns);

        if(m_pLargestFreeRunMarker)
        {
            if(pRunMarker->nLength > m_pLargestFreeRunMarker->nLength)
            {
                m_pLargestFreeRunMarker = pRunMarker;
            }
        }
        else
        {
            m_pLargestFreeRunMarker = pRunMarker;
        }

        m_dwAvailable += nLength;

        DPF_MEM_HEAP("Added %x to the slop heap (entry %x, run marker %x, length %lu, %lu bytes used for tracking, %lu bytes used for run marker)", pvBaseAddress, pEntry, pRunMarker, nSlopLength, sizeof(*pEntry), sizeof(*pRunMarker));
    }

    DPF_LEAVE(fOwned);

    return fOwned;
}


/****************************************************************************
 *
 *  Alloc
 *
 *  Description:
 *      Allocates memory from the heap.
 *
 *  Arguments:
 *      DWORD [in]: length, in bytes
 *
 *  Returns:  
 *      LPVOID: buffer base address or NULL.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxSlopMemoryHeap::Alloc"

LPVOID
CMcpxSlopMemoryHeap::Alloc
(
    DWORD                   nLength
)
{
    LPSLOPRUNMARKER         pRunMarker      = NULL;
    LPSLOPRUNMARKER         pMarkerNode;
    PLIST_ENTRY             pleEntry;

    DPF_ENTER();
    AutoIrql();

    //
    // The length must include the space to hold the run marker
    //

    nLength += sizeof(SLOPRUNMARKER);

    //
    // Check the largest available run length to see if we can service
    // the request
    //

    if(m_pLargestFreeRunMarker)
    {
        if(m_pLargestFreeRunMarker->nLength >= nLength)
        {
            //
            // Find a free run that is as close as possible to the requested element
            // count.  This will help to reduce fragmentation.
            //

            for(pleEntry = m_lstRuns.Flink; pleEntry != &m_lstRuns; pleEntry = pleEntry->Flink)
            {
                AssertValidEntryList(pleEntry, ASSERT_IN_LIST);

                pMarkerNode = CONTAINING_RECORD(pleEntry, SLOPRUNMARKER, leListEntry);
            
                if(!pMarkerNode->fAllocated)
                {
                    if(pMarkerNode->nLength >= nLength)
                    {
                        if(!pRunMarker)
                        {
                            pRunMarker = pMarkerNode;
                        }
                        else if(pMarkerNode->nLength < pRunMarker->nLength)
                        {
                            pRunMarker = pMarkerNode;
                        }
                    }
                }
            }

            //
            // If we're using the whole run, just flag it as allocated.  If not,
            // create a new marker from the remaining buffer and add it to the 
            // list.  We're only using a single list so that runs can be broken 
            // and coalesced without having to walk the list.
            //

            if(pRunMarker)
            {
                if(nLength + sizeof(*pRunMarker) + m_dwUsageThreshold < pRunMarker->nLength)
                {
                    pMarkerNode = CreateMarker((LPBYTE)pRunMarker + nLength, pRunMarker->nLength - nLength, &pRunMarker->leListEntry);

                    pRunMarker->nLength = nLength;

                    DPF_MEM_HEAP("Split memory run into %x (%lu) and %x (%lu)", pRunMarker, pRunMarker->nLength, pMarkerNode, pMarkerNode->nLength);
                }
                else
                {
                    DPF_MEM_HEAP("Allocating memory run %x (%lu)", pRunMarker, pRunMarker->nLength);
                }

                pRunMarker->dwSignature = DSOUND_ALLOCATOR_SLOP;
            }

            //
            // Update the largest run member
            //

            if(pRunMarker)
            {
                if(pRunMarker == m_pLargestFreeRunMarker)
                {
                    m_pLargestFreeRunMarker = NULL;
                    
                    for(pleEntry = m_lstRuns.Flink; pleEntry != &m_lstRuns; pleEntry = pleEntry->Flink)
                    {
                        AssertValidEntryList(pleEntry, ASSERT_IN_LIST);

                        pMarkerNode = CONTAINING_RECORD(pleEntry, SLOPRUNMARKER, leListEntry);
            
                        if(!pMarkerNode->fAllocated)
                        {
                            if(!m_pLargestFreeRunMarker)
                            {
                                m_pLargestFreeRunMarker = pMarkerNode;
                            }
                            else if(pMarkerNode->nLength > m_pLargestFreeRunMarker->nLength)
                            {
                                m_pLargestFreeRunMarker = pMarkerNode;
                            }
                        }
                    }
                }                    
            }

            //
            // Update the amount of memory recovered
            //

            if(pRunMarker)
            {
                m_dwUsed += pRunMarker->nLength;
            }

            //
            // AddRef ourselves so we're always around as long as allocated
            // memory is.
            //

            if(pRunMarker)
            {
                AddRef();
            }
        }
    }

    DPF_LEAVE(pRunMarker ? pRunMarker + 1 : NULL);

    return pRunMarker ? pRunMarker + 1 : NULL;
}


/****************************************************************************
 *
 *  Free
 *
 *  Description:
 *      Frees a previously allocated run.
 *
 *  Arguments:
 *      LPVOID [in]: buffer base address.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxSlopMemoryHeap::Free"

void
CMcpxSlopMemoryHeap::Free
(
    LPVOID                  pvBaseAddress
)
{
    LPSLOPRUNMARKER         pRunMarker  = (LPSLOPRUNMARKER)pvBaseAddress - 1;
    PLIST_ENTRY             pleEntry;
    LPSLOPRUNMARKER         pMarkerNode;
    
    DPF_ENTER();
    AutoIrql();

    ASSERT(DSOUND_ALLOCATOR_SLOP == pRunMarker->dwSignature);

    DPF_MEM_HEAP("Freeing memory run %x (%lu)", pRunMarker, pRunMarker->nLength);

    //
    // Update the amount of memory recovered
    //

    ASSERT(m_dwUsed >= pRunMarker->nLength);
    m_dwUsed -= pRunMarker->nLength;

    //
    // Reset the run signature so it's flagged as free
    //

    pRunMarker->dwSignature = 0;

    //
    // Coalesce the run with those around it
    //

    while(TRUE)
    {
        if((pleEntry = pRunMarker->leListEntry.Flink) == &m_lstRuns)
        {
            break;
        }

        pMarkerNode = CONTAINING_RECORD(pleEntry, SLOPRUNMARKER, leListEntry);

        if(pMarkerNode->fAllocated)
        {
            break;
        }

        if((LPBYTE)pRunMarker + pRunMarker->nLength != (LPBYTE)pMarkerNode)
        {
            break;
        }

        pRunMarker = CoalesceRuns(pRunMarker, pMarkerNode);
    }

    while(TRUE)
    {
        if((pleEntry = pRunMarker->leListEntry.Blink) == &m_lstRuns)
        {
            break;
        }

        pMarkerNode = CONTAINING_RECORD(pleEntry, SLOPRUNMARKER, leListEntry);

        if(pMarkerNode->fAllocated)
        {
            break;
        }

        if((LPBYTE)pMarkerNode + pMarkerNode->nLength != (LPBYTE)pRunMarker)
        {
            break;
        }

        pRunMarker = CoalesceRuns(pMarkerNode, pRunMarker);
    }

    //
    // Update the largest run pointer
    //

    if(m_pLargestFreeRunMarker)
    {
        if(pRunMarker->nLength > m_pLargestFreeRunMarker->nLength)
        {
            m_pLargestFreeRunMarker = pRunMarker;
        }
    }
    else
    {
        m_pLargestFreeRunMarker = pRunMarker;
    }

    //
    // Release the reference added in Alloc
    //

    Release();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CoalesceRuns
 *
 *  Description:
 *      Coalesces two adjacent runs.
 *
 *  Arguments:
 *      LPSLOPRUNMARKER [in]: run marker.
 *      LPSLOPRUNMARKER [in]: next run marker in the list (this one will be 
 *                           removed).
 *
 *  Returns:  
 *      LPSLOPRUNMARKER: run marker.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxSlopMemoryHeap::CoalesceRuns"

LPSLOPRUNMARKER
CMcpxSlopMemoryHeap::CoalesceRuns
(
    LPSLOPRUNMARKER         pRunMarker,
    LPSLOPRUNMARKER         pNextRunMarker
)
{
    DPF_ENTER();

    ASSERT((LPBYTE)pRunMarker + pRunMarker->nLength == (LPBYTE)pNextRunMarker);
    ASSERT(!pRunMarker->fAllocated);
    ASSERT(!pNextRunMarker->fAllocated);

    AssertValidEntryList(&pRunMarker->leListEntry, ASSERT_IN_LIST);
    AssertValidEntryList(&pNextRunMarker->leListEntry, ASSERT_IN_LIST);

    DPF_MEM_HEAP("Coalescing memory run %x (%lu) and %x (%lu)", pRunMarker, pRunMarker->nLength, pNextRunMarker, pNextRunMarker->nLength);

    pRunMarker->nLength += pNextRunMarker->nLength;
    
    RemoveEntryList(&pNextRunMarker->leListEntry);

    DPF_LEAVE(pRunMarker);

    return pRunMarker;
}


