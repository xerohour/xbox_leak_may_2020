/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       heap.h
 *  Content:    Generic heap object.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/24/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __HEAP_H__
#define __HEAP_H__

// 
// Buffer SGE heap run marker
//

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY  leListEntry;            // Linked list entry
    WORD        nElement;               // Run element index
    WORD        nLength;                // Run length, in elements
    union
    {
        DWORD   dwRefCount;             // Run reference count
        BOOL    fAllocated;             // Is the run allocated?
    };
    LPVOID      pvBaseAddress;          // Base address of mapped buffer
END_DEFINE_STRUCT(SGEHEAPRUNMARKER);

//
// Slop heap owned memory entry
//

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY  leListEntry;            // List entry
    LPVOID      pvBaseAddress;          // Base address of allocation
END_DEFINE_STRUCT(SLOPMEMENTRY);

// 
// Unused memory heap run marker
//

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY  leListEntry;            // Linked list entry
    DWORD       nLength;                // Run length, in bytes (includes marker)
    union
    {
        DWORD   dwSignature;            // Allocation validation signature
        BOOL    fAllocated;             // Is the run allocated?
    };
END_DEFINE_STRUCT(SLOPRUNMARKER);

#ifdef __cplusplus

//
// MCP-X buffer SGE heap
//

class CMcpxBufferSgeHeapTest
{
protected:
    LIST_ENTRY              m_lstRuns;                  // Heap element run list
    LIST_ENTRY              m_lstMarkers;               // Unused markers
    LPSGEHEAPRUNMARKER      m_paMarkers;                // Markers
    DWORD                   m_nFreeElementCount;        // Free element count
    LPSGEHEAPRUNMARKER      m_pLargestFreeRunMarker;    // Largest available run

public:
    CMcpxBufferSgeHeapTest(void);
    virtual ~CMcpxBufferSgeHeapTest(void);

public:
    // Initialization
    HRESULT Initialize(WORD nElementCount);

    // Allocation
    LPSGEHEAPRUNMARKER Alloc(LPVOID pvBuffer, DWORD dwBufferSize);
    void Free(LPSGEHEAPRUNMARKER pMarker);

    // Free element count
    DWORD GetFreeElementCount(void);

protected:
    // Allocation
    LPSGEHEAPRUNMARKER AllocRun(LPVOID pvBaseAddress, WORD nElementCount);
    void FreeRun(LPSGEHEAPRUNMARKER pMarker);
    
    // Garbage collection
    LPSGEHEAPRUNMARKER CoalesceRuns(LPSGEHEAPRUNMARKER pMarker, LPSGEHEAPRUNMARKER pNextMarker);

    // Markers
    LPSGEHEAPRUNMARKER CreateMarker(LPSGEHEAPRUNMARKER pMarker, WORD nElement, WORD nLength, PLIST_ENTRY plePrevEntry);

    // MCPX SGE stuff
    static void MapBuffer(DWORD dwSgeIndex, LPVOID pvBaseAddress, DWORD dwLength);
    static void UnmapBuffer(LPVOID pvBaseAddress, DWORD dwLength);
};

__inline DWORD CMcpxBufferSgeHeapTest::GetFreeElementCount(void)
{
    return m_nFreeElementCount;
}

__inline LPSGEHEAPRUNMARKER CMcpxBufferSgeHeapTest::CreateMarker(LPSGEHEAPRUNMARKER pMarker, WORD nElement, WORD nLength, PLIST_ENTRY plePrevEntry)
{
    InsertHeadListUninit(plePrevEntry, &pMarker->leListEntry);

    pMarker->nElement = nElement;
    pMarker->nLength = nLength;
    pMarker->dwRefCount = 0;
    pMarker->pvBaseAddress = NULL;

    return pMarker;
}

//
// MCP-X slop memory heap
//

class CMcpxSlopMemoryHeapTest
    : public CRefCountTest
{
public:
    static const DWORD      m_dwUsageThreshold;         // Minimum amount of memory the heap will track
    static DWORD &          m_dwAvailable;              // Total amount of memory in the heap
    static DWORD &          m_dwUsed;                   // Amount of memory used

protected:
    LIST_ENTRY              m_lstEntries;               // Allocations
    LIST_ENTRY              m_lstRuns;                  // Heap element run list
    LPSLOPRUNMARKER         m_pLargestFreeRunMarker;    // Largest available run

public:
    CMcpxSlopMemoryHeapTest(void);
    virtual ~CMcpxSlopMemoryHeapTest(void);

public:
    // Slop memory
    BOOL AddRun(LPVOID pvBaseAddress, DWORD dwLength, DWORD dwUsed);

    // Allocation
    LPVOID Alloc(DWORD dwLength);
    void Free(LPVOID pvBuffer);

protected:
    // Garbage collection
    LPSLOPRUNMARKER CoalesceRuns(LPSLOPRUNMARKER pMarker, LPSLOPRUNMARKER pNextMarker);

    // Markers
    LPSLOPRUNMARKER CreateMarker(LPVOID pvBaseAddress, DWORD nLength, PLIST_ENTRY plePrevEntry);
};

__inline LPSLOPRUNMARKER CMcpxSlopMemoryHeapTest::CreateMarker(LPVOID pvBaseAddress, DWORD nLength, PLIST_ENTRY plePrevEntry)
{
    LPSLOPRUNMARKER         pRunMarker  = (LPSLOPRUNMARKER)pvBaseAddress;
    
    ASSERT(nLength > sizeof(*pRunMarker));

    InsertHeadListUninit(plePrevEntry, &pRunMarker->leListEntry);

    pRunMarker->nLength = nLength;
    pRunMarker->dwSignature = 0;

    return pRunMarker;
}

//
// The one and only slop memory heap
//

extern CMcpxSlopMemoryHeapTest *g_pDirectSoundTestSlopMemoryHeap;

#endif // __cplusplus

#endif // __HEAP_H__
