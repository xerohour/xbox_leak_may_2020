/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	MusicHeapFixed.cpp

Abstract:

	User-defined DirectMusic heap object 

Author:

	Dan Haffner(danhaff) 05-Jul-2001

Environment:

	Xbox only

Revision History:

	05-Jul-2001 danhaff
		Initial Version; copied out of dmusic/debug.cpp and modified.

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <xtl.h>
#include "macros.h"
#include "MusicHeap.h"
#include "helpers.h"
#include "assert.h"

using namespace WFVO;

#define VERIFYHEAP VerifyHeap();
#define INITIALIZE_MEMORY 
#define CACHE


namespace WFVO
{

/*
class CMusicHeapFixedCache : public IMusicHeap
{
    //LOCALALLOC_NEWDELETE
public:

    CMusicHeapFixedCache(void);
    HRESULT Initialize(DWORD dwNormalHeapSize);
    ~CMusicHeapFixedCache(void);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    //  IDirectMusicHeap 
    STDMETHOD(Allocate) (DWORD dwSize, PVOID* ppData);
    STDMETHOD(Free)   (PVOID pData);

private:
    // Use standard HeapCreate / HeapAlloc heap for the normal heap.
    HANDLE m_hHeap;
    LONG m_cRef;

};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CMusicHeapFixedCache::CMusicHeapFixedCache(void)
{
m_cRef = 1;
m_hHeap = 0;
}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMusicHeapFixedCache::Initialize(DWORD dwNormalHeapSize)
{
    m_hHeap = HeapCreate(0, dwNormalHeapSize, dwNormalHeapSize);
    if(!m_hHeap){
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CMusicHeapFixedCache::~CMusicHeapFixedCache()
{
    if(m_hHeap) {
        HeapDestroy(m_hHeap);
    }
}

// IUnknown
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    return E_NOTIMPL;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CMusicHeapFixedCache::AddRef(void) 
    
{
    return InterlockedIncrement(&m_cRef);
}


//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CMusicHeapFixedCache::Release(void) 
{
    ULONG cRef;
        
    cRef = InterlockedDecrement(&m_cRef);

    if (cRef == 0)
        delete this;

    return cRef;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//TODO: Add code to track allocations and sizes here.
STDMETHODIMP CMusicHeapFixedCache::Allocate(DWORD dwSize, PVOID* ppData)
{
    PVOID pData = HeapAlloc(m_hHeap, 0, dwSize);
    if(!pData){
        return E_OUTOFMEMORY;
    }
    *ppData = pData;
    return S_OK;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::Free(PVOID pData)
{
    HeapFree(m_hHeap, 0, pData);
    return S_OK;
}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

*/

class CMusicHeapFixedCache : public IMusicHeap
{
//    LOCALALLOC_NEWDELETE
private:
    // Use a traditional first-fit heap for the physical memory heap

    struct BlockHead
    {
        BlockHead* pNext;
        //BlockHead* pPrev;
        unsigned int size;
        BOOL bUsed;
    };

    LONG m_cRef;
    CRITICAL_SECTION m_csPhysicalHeap;
    BlockHead* m_pHead;
    
    #ifdef CACHE
    BlockHead *m_pLastBlock;
    DWORD m_dwCacheHits;
    DWORD m_dwCacheMisses;
    DWORD m_dwCacheMissesNotBigEnough;
    #endif

    BOOL m_bPhysical;
    DWORD m_dwHeapSize;
    DWORD m_dwHeapAllocation;
    DWORD m_dwHeapAllocationPeak;
    DWORD m_dwHeapAllocationBlocks;
    DWORD m_dwHeapAllocationBlocksPeak;

    STDMETHOD(VerifyHeap)(void);

public:
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    //  IMusicHeap
    STDMETHOD(Allocate)   (DWORD dwSize, PVOID* ppData);
    STDMETHOD(Free)   (PVOID pData);
    STDMETHOD(GetSize) (THIS_ PVOID pData, LPDWORD pcbSize);

    // IMusicHeap
    STDMETHOD(GetHeapAllocation          )(DWORD *pdwHeapAllocation);
    STDMETHOD(GetHeapAllocationPeak      )(DWORD *pdwHeapAllocationPeak);
    STDMETHOD(GetHeapAllocationBlocks    )(DWORD *pdwHeapAllocationBlocks);
    STDMETHOD(GetHeapAllocationBlocksPeak)(DWORD *pdwHeapAllocationBlocksPeak);


    CMusicHeapFixedCache(void);
    ~CMusicHeapFixedCache(void);
    HRESULT Initialize(DWORD dwHeapSize, BOOL bPhysical);

};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CMusicHeapFixedCache::CMusicHeapFixedCache()
{
    m_cRef = 1;
    m_pHead = 0;
    
    #ifdef CACHE
    m_pLastBlock = 0;
    #endif

    m_bPhysical = FALSE;
    m_dwHeapSize = 0;
    ZeroMemory(&m_csPhysicalHeap, sizeof(m_csPhysicalHeap));
    m_dwHeapAllocation           = 0;
    m_dwHeapAllocationPeak       = 0;
    m_dwHeapAllocationBlocks     = 0;
    m_dwHeapAllocationBlocksPeak = 0;
    m_dwCacheHits = 0;
    m_dwCacheMisses = 0;
    m_dwCacheMissesNotBigEnough = 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CMusicHeapFixedCache::~CMusicHeapFixedCache()
{
    XPhysicalFree(m_pHead);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMusicHeapFixedCache::Initialize(DWORD dwHeapSize, BOOL bPhysical)
{
    InitializeCriticalSection(&m_csPhysicalHeap);
    EnterCriticalSection(&m_csPhysicalHeap);

    m_dwHeapSize = dwHeapSize;
    if(dwHeapSize < sizeof(BlockHead))
    {
        dwHeapSize = sizeof(BlockHead);
    }
    
    if (!bPhysical)
        m_pHead = (BlockHead *)LocalAlloc(LMEM_FIXED, dwHeapSize);
    else
        m_pHead = (BlockHead*) XPhysicalAlloc(dwHeapSize, MAXULONG_PTR, 0, PAGE_READWRITE);



    if(!m_pHead)
    {
        LeaveCriticalSection(&m_csPhysicalHeap);
        return E_OUTOFMEMORY;
    }
    m_pHead->pNext = 0;
    //m_pHead->pPrev = 0;
    m_pHead->size = dwHeapSize-sizeof(BlockHead);
    m_pHead->bUsed = 0;
    memset(((char *)m_pHead) + sizeof(BlockHead), 0x00, m_pHead->size);
    LeaveCriticalSection(&m_csPhysicalHeap);

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    return E_NOTIMPL;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CMusicHeapFixedCache::AddRef(void)         
{
    return InterlockedIncrement(&m_cRef);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CMusicHeapFixedCache::Release( void)
{
    ULONG cRef;
        
    cRef = InterlockedDecrement(&m_cRef);

    if (cRef == 0)
        delete this;

    return cRef;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::VerifyHeap(void)
{
   DWORD dwCount = 0;
   BlockHead *pB = NULL;
   char *pByte;


   EnterCriticalSection(&m_csPhysicalHeap);

    for(pB = m_pHead; pB; pB = pB->pNext)
    {
        dwCount++;
        pByte = (char *)pB;
        
        //Verify that the first and last byte in this block are 'X'.
        /*
        if (pB->bUsed == 0)
        {
         
            if ((pByte + sizeof(BlockHead))[0] != 'X')
            {
                Log("Heap Corruption at beginning of block");
                assert(FALSE);
            }

            if ((pByte + sizeof(BlockHead))[pB->size - 1] != 'X')
            {
                Log("Heap Corruption at end of block");
                assert(FALSE);
            }
        }
        */

        //Verify that the sizes match.
        if (pB->pNext)
        {
            if ((char *)pB->pNext - (char *)pB != pB->size + sizeof(BlockHead))
            {
                Log("Difference between pB (%08X) and pB->pNext (%08X) is %08X (%d), but the size of pB + sizeof(BlockHead) is %08X (%d)\n",
                     pB,
                     pB->pNext,
                     (char *)pB->pNext - (char *)pB,
                     (char *)pB->pNext - (char *)pB,
                     pB->size,
                     pB->size
                     );
                assert(FALSE);
            }
        }
    }


   LeaveCriticalSection(&m_csPhysicalHeap);
   return S_OK;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::GetSize(PVOID pData, LPDWORD pcbSize)
{
    assert(pData);
    EnterCriticalSection(&m_csPhysicalHeap);
    BlockHead* pB = (BlockHead*)(((char*) pData) - sizeof(BlockHead));
    *pcbSize = pB->size;
    LeaveCriticalSection(&m_csPhysicalHeap);
    return S_OK;
}



//------------------------------------------------------------------------------
//CACHING:

//Simple overview that isn't quite right:
//    When we split a block, we cache the remaining (unallocated) block and check it
//    the next allocation attempt before walking the list again.

//The way it really works.
//    When we free a block, we check if it's address is smaller than the currently cached
//    block.  If so, then we make that the cached block.  

//    When we split a block that was found in the cache, we cache the split block (if there is any space left)
    
//    When we split a block that wasn't found in the cache, and there still is a cached block,
//    we keep the cached block, otherwise we cache the split block.
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::Allocate(DWORD dwSize, PVOID* ppData)
{
    PVOID pData = NULL;
    BOOL bFound = FALSE;
    BOOL bFoundInCache = FALSE;
    BlockHead* pB = NULL;
    BlockHead* pBStartingPoint = NULL;
    DWORD dwStartingPoint = 0;

    VERIFYHEAP

    EnterCriticalSection(&m_csPhysicalHeap);

    //Round up to the nearest DWORD
    dwSize += (4 - (dwSize & 3));

    //We've cached the most recently-created block.  Try it first.    
/*
    #ifdef CACHE
    if (m_pLastBlock)
    {
        //If this one's big enough take it, otherwise walk to the next big enough one.
        if (dwSize <= m_pLastBlock->size)
        {   
            pB = m_pLastBlock;
            m_pLastBlock = NULL;
            bFound = TRUE;
            bFoundInCache = TRUE;
        }
        else
        {
        }
    }
    #endif
*/


    //Try searching and coalescing from the cached block, then from the beginning.
    for (dwStartingPoint = 0; dwStartingPoint < 2; dwStartingPoint++)        
    {
        //Try the cached block first.
        pBStartingPoint = (dwStartingPoint==0) ? m_pLastBlock : m_pHead;
        for(pB = pBStartingPoint; pB; pB = pB->pNext)
        {
            if((!pB->bUsed))
            {
                // Coalesce with next block, if possible
                while(pB->pNext && !pB->pNext->bUsed)
                {
                    //If we're coalescing the cached block, set the cached block to the one that coalesced it.
#ifdef CACHE
                    if (pB->pNext == m_pLastBlock)
                        m_pLastBlock = pB;
#endif
                
                    pB->size += pB->pNext->size + sizeof(BlockHead);
                    pB->pNext = pB->pNext->pNext;
                }
        
                // Is this block big enough to use?
                if(dwSize <= pB->size)
                {
                    bFound = TRUE;
#ifdef CACHE                    
                    //If we've chosen the cached block, then whip it out.
                    if (pB == m_pLastBlock)
                    {
                        m_pLastBlock = NULL;
                        m_dwCacheHits++;
                    }
                    else
                    {
                        m_dwCacheMisses++;
                    }

#endif


                    break;
                }
            }
        }

        
        if (bFound)
            break;
    }

/*
#ifdef CACHE    
    //Keep stats.
    if (bFoundInCache)
        m_dwCacheHits ++;
    else
        m_dwCacheMisses ++;
#endif
*/


    //If we found a big enough block, then attempt to split it.  Then use it.
    if (bFound)
    {
        // Is it big enough to split?
        if(dwSize + sizeof(BlockHead) < pB->size)
        {
            // Split.  
            BlockHead* pNew = (BlockHead*) (((char*) pB) + sizeof(BlockHead) + dwSize);

            pNew->pNext = pB->pNext;
            pNew->size = pB->size - (dwSize + sizeof(BlockHead));
            pNew->bUsed = 0;
            pB->pNext    = pNew;                    
           
            //Cache the most recently created block.
            m_pLastBlock = pNew;

/*
#ifdef CACHE
            //When we split a block that was found in the cache, we cache the split block (if there is any space left)
            if (bFoundInCache)

            //When we split a block that wasn't found in the cache, and there still is a cached block,
            //we keep the cached block, otherwise we cache the split block.
            else
            {
                if (!m_pLastBlock)
                {
                    m_pLastBlock = pNew;
                }
            }

#endif
*/

            pB->size = dwSize;

        }
        
#ifdef CACHE
/*
        //If the block wasn't big enough to split, then walk the list for a new available block.
        else
        {
            m_pLastBlock = pB;
            do
            {
                m_pLastBlock = m_pLastBlock->pNext;                
                if (NULL == m_pLastBlock)
                    break;
            }
            while (m_pLastBlock->bUsed);

        }
*/

        //If we used the cached block, or don't have one, or couldn't split the new block, then walk the list forward from the one we found to
        //  find a new cached block.
/*
        if (!m_pLastBlock)
        {
            m_pLastBlock = pB;
            do
            {
                m_pLastBlock = m_pLastBlock->pNext;                
                if (NULL == m_pLastBlock)
                    break;
            }
            while (m_pLastBlock->bUsed);

        }
*/

#endif

        pB->bUsed = 'z';
        pData = (void*) (((char*) pB) + sizeof(BlockHead));

        
        #ifdef INITIALIZE_MEMORY
        //Initialize the "new data" to something
        memset(pData, 0xAA, dwSize);
        #endif

        
        //Update our internal data.
        m_dwHeapAllocation += (pB->size + sizeof(BlockHead));
        if(m_dwHeapAllocation > m_dwHeapAllocationPeak)
            m_dwHeapAllocationPeak = m_dwHeapAllocation;
        ++m_dwHeapAllocationBlocks;
        if(m_dwHeapAllocationBlocks > m_dwHeapAllocationBlocksPeak)
            m_dwHeapAllocationBlocksPeak = m_dwHeapAllocationBlocks;
    }
    
    LeaveCriticalSection(&m_csPhysicalHeap);

    VERIFYHEAP
    
    if(!pData)
    {
        Log("ERROR: Out of %s memory!\n", m_bPhysical ? "Physical" : "Non-Physical");
        Log("Limit:                  %9u\n", m_dwHeapSize);
        Log("Current:                %9u\n", m_dwHeapAllocation);
        Log("Attempting to allocate: %9u\n", dwSize);
        Log("# of allocations:       %9u\n", m_dwHeapAllocationBlocks);
        Log("Fragmentation (bytes):  %9d\n", m_dwHeapSize - m_dwHeapAllocation - dwSize);
        Log("Fragmentation (%%):       %9.2f\n", FLOAT(100 *(m_dwHeapSize - m_dwHeapAllocation - dwSize)) / FLOAT(m_dwHeapSize));
        assert(FALSE);
        return E_OUTOFMEMORY;
    }

    *ppData = pData;
    assert((DWORD)pData % 4 == 0);
    return S_OK;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::Free(PVOID pData)
{
 
    if (!pData)
    {
        Log("FIXED HEAP ERROR: Someone called Free() on a NULL ptr");
        assert(FALSE);
    }

    VERIFYHEAP
    EnterCriticalSection(&m_csPhysicalHeap);
    BlockHead* pB = (BlockHead*)(((char*) pData) - sizeof(BlockHead));
    if (pB->bUsed != 'z')
    {   
        Log("Bad Block");
        assert(FALSE);
    }

    //Update our internal data.
    m_dwHeapAllocation -= (pB->size + sizeof(BlockHead));
    --m_dwHeapAllocationBlocks;

    //If we don't have a block cached, or this one is less than the cached one,
    //  then cache this one.
    #ifdef CACHE
    if (!m_pLastBlock || (pB < m_pLastBlock))
    {
        m_pLastBlock = pB;
    }
    #endif

    //Set all the memory to a known value for debugging.
    #ifdef INITIALIZE_MEMORY
    memset(((char *)pB) + sizeof(BlockHead), 0xFF, pB->size);
    #endif 

    pB->bUsed = 0;

    LeaveCriticalSection(&m_csPhysicalHeap);
    VERIFYHEAP

    return S_OK;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::GetHeapAllocation          (DWORD *pdwHeapAllocation)
{
    EnterCriticalSection(&m_csPhysicalHeap);        
    *pdwHeapAllocation = m_dwHeapAllocation;
    LeaveCriticalSection(&m_csPhysicalHeap);
    return S_OK;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::GetHeapAllocationPeak      (DWORD *pdwHeapAllocationPeak)
{
    EnterCriticalSection(&m_csPhysicalHeap);        
    *pdwHeapAllocationPeak = m_dwHeapAllocationPeak;
    LeaveCriticalSection(&m_csPhysicalHeap);
    return S_OK;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::GetHeapAllocationBlocks    (DWORD *pdwHeapAllocationBlocks)
{
    EnterCriticalSection(&m_csPhysicalHeap);        
    *pdwHeapAllocationBlocks = m_dwHeapAllocationBlocks;
    LeaveCriticalSection(&m_csPhysicalHeap);
    return S_OK;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CMusicHeapFixedCache::GetHeapAllocationBlocksPeak(DWORD *pdwHeapAllocationBlocksPeak)
{
    EnterCriticalSection(&m_csPhysicalHeap);        
    *pdwHeapAllocationBlocksPeak = m_dwHeapAllocationBlocksPeak;
    LeaveCriticalSection(&m_csPhysicalHeap);
    return S_OK;
};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CreateMusicHeapFixedCache(IMusicHeap** ppHeap, bool bPhysical, DWORD dwSize)
{
    HRESULT hr = S_OK;
    CMusicHeapFixedCache* pHeap = NULL;

    pHeap = new CMusicHeapFixedCache();

    if(!pHeap)
    {
        hr = E_OUTOFMEMORY;
    }
    if(SUCCEEDED(hr))
    {
        hr = pHeap->Initialize(dwSize, bPhysical);
        if(SUCCEEDED(hr))
            *ppHeap = pHeap;
        else
            pHeap->Release();
    }
    return hr;
}



}//end "namespace WFVO"