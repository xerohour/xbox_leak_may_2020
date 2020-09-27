/***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
/********************************* Direct 3D *******************************\
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/27/99 - created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

NVOS11_PARAMETERS HeapParams;

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 *** CHeap *******************************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/

//
// switches
//

//
// globals
//
BOOL CInterProcessHeap::m_bInitialized = FALSE;
CInterProcessHeap g_nvIPHeap;

/*****************************************************************************
 * CInterProcessHeap::dbgTestIntegrity
 *
 * check that the heap is in good shape
 */
#ifdef CHEAP_SANITY_CHECK
void CInterProcessHeap::dbgTestIntegrity
(
    void
)
{
    HEADER *pFastHeader, *pSlowHeader;
    DWORD dwCount = 0;

    // check linked list
    for (pSlowHeader = pFastHeader = m_pBase; pFastHeader;)
    {
        if (pFastHeader->pNext)
        {
            if (pFastHeader->pNext->pPrev != pFastHeader)
            {
                DPF ("CInterProcessHeap::dbgTestIntegrity: list doesn't point to its parent");
                __asm int 3;
            }
        }

        pFastHeader = pFastHeader->pNext;

        if (dwCount & 0x01) pSlowHeader = pSlowHeader->pNext;
        dwCount ++;
        if (pSlowHeader == pFastHeader)
        {
            DPF ("CInterProcessHeap::dbgTestIntegrity: list points to itself!");
            __asm int 3;
            return;
        }
    }

    // check free list
    dwCount = 0;
    for (pSlowHeader = pFastHeader = m_pBaseFree; pFastHeader;)
    {
        pFastHeader = pFastHeader->pNextFree;
        if (dwCount & 0x01) pSlowHeader = pSlowHeader->pNextFree;
        dwCount++;
        if (pSlowHeader == pFastHeader)
        {
            DPF ("CInterProcessHeap::dbgTestIntegrity: free list points to itself!");
            __asm int 3;
            return;
        }
    }
}
#endif

//
// display use of IPM heap
//
void CInterProcessHeap::checkHeapUsage
(
   void
)
{

    // defragmenting here prevents the un-allocated heap from growing without bound
    combineFreeBlocks();

    // release any blocks that are completely unused
    releaseFreeBlocks();


#ifdef DEBUG
    if (m_pBase)
    {
        DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "*** IPM heap usage ***");
        DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "pHeader  end      (start)  (size)   pPrev    pCaller");
        DWORD dwBlocksInUse = 0, dwBlocksFree = 0;
        DWORD dwBytesInUse  = 0, dwBytesFree  = 0;
        HEADER *pHeader = m_pBase;
        while(pHeader != NULL)
        {

            if(pHeader->pPrev)
                DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "%08x %08x %08x %08x %08x %08x %c%c", pHeader,
                pHeader + pHeader->dwRealSize, pHeader->dwUserStart, pHeader->dwUserSize,
                pHeader->pPrev, pHeader->pCaller, isBoundary(pHeader) ? 'B' : ' ', isAlloc(pHeader) ? '*' : ' ');
            else
                DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "%08x %08x %08x %08x **NULL** %08x %c%c", pHeader,
                pHeader + pHeader->dwRealSize, pHeader->dwUserStart, pHeader->dwUserSize, pHeader->pCaller,
                isBoundary(pHeader) ? 'B' : ' ', isAlloc(pHeader) ? '*' : ' ');

            if (isAlloc(pHeader))
            {
                dwBlocksInUse ++;
                dwBytesInUse += pHeader->dwRealSize;
            }
            else
            {
                dwBlocksFree ++;
                dwBytesFree += pHeader->dwRealSize;
            }

            pHeader = pHeader->pNext;
        }
        DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "Blocks in use: %8d", dwBlocksInUse);
        DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "Blocks free  : %8d", dwBlocksFree);
        DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "Bytes in use : %08x", dwBytesInUse);
        DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "Bytes free   : %08x", dwBytesFree);
        DPF_LEVEL(NVDBG_LEVEL_HEAP_USAGE, "**********************");
    }
#endif
}

/*****************************************************************************
 * CInterProcessHeap::init
 *
 * initialize the IP heap
 */
void CInterProcessHeap::init
(
    void
)
{
    // make sure we only do this once since some apps may load the DLL twice
    if (!m_bInitialized) {
        m_pBase        = NULL;
        m_pBaseFree    = NULL;
        m_dwRMID       = IPM_MEMORY_OBJECT_LO;
#ifndef WINNT
        char devName[128];
        DWORD res;
        res = NvRmAllocRoot(&m_dwRootHandle);
        nvAssert(res == NVOS01_STATUS_SUCCESS);
        m_dwDeviceHandle = IPM_DEVICE_HANDLE;
        res = NvRmAllocDevice (m_dwRootHandle, m_dwDeviceHandle, NV01_DEVICE_0, (BYTE*)devName);
        nvAssert(res == NVOS06_STATUS_SUCCESS);
#endif
        m_bInitialized = TRUE;
    }
}

/*****************************************************************************
 * CInterProcessHeap::create
 *
 * alloc IP memory directly from RM to create a new (or bigger) heap
 */
CInterProcessHeap::HEADER* CInterProcessHeap::create
(
    DWORD dwSize
)
{
    DPF_LEVEL(NVDBG_LEVEL_HEAP_ACTIVITY, "---IPM---Create: %08x bytes", dwSize);

    // sanity check
    nvAssert (!(dwSize & 0xfff)); // must be multiple of 4k

    // allocate the memory
    DWORD adwMem[2], dwRes = FALSE, dwExtraFlags = 0;

    dwSize--; // allocator wants a limit, not a size

#ifdef WINNT
    adwMem[0] = (DWORD) EngAllocMem(FL_ZERO_MEMORY, dwSize + 1, 'x_VN');
#else  // !WINNT
    DWORD dwFlags = (NVOS02_FLAGS_PHYSICALITY_NONCONTIGUOUS << 4)
                  | (NVOS02_FLAGS_LOCATION_PCI << 8)
                  | (NVOS02_FLAGS_COHERENCY_CACHED << 12);
    if (global.b16BitCode)
    {
        DRVSERVICE_ALLOC_MEMORY dam;
        dam.dwObjectID   = m_dwRMID;
        dam.dwAllocFlags = dwFlags;
        dam.dwNumBytes   = dwSize + 1;
        dwRes = !DrvCallbackFunc(NV_DRVSERVICE_ALLOC_MEMORY, &dam, adwMem);
        dwExtraFlags = FLAG_16BIT;
    }
    else
    {
        dwRes = NvRmAllocMemory(m_dwRootHandle, m_dwDeviceHandle,
            m_dwRMID,
            NV01_MEMORY_SYSTEM,
            dwFlags,
            (PVOID*)adwMem,
            (unsigned long*)&dwSize);
    }
#endif  // !WINNT

    if (dwRes || (adwMem[0] == NULL))
    {
        DPF ("CInterProcessHeap::create: Memory allocation failed");
        dbgD3DError();
        return  FALSE;
    }


    // populate header
    HEADER *pHeader     = (HEADER*)adwMem[0];
#ifdef DEBUG
    memset (pHeader,0xcc,dwSize);
#endif
    pHeader->dwFlags    = FLAG_BOUNDARY | dwExtraFlags;
    pHeader->dwRealSize = dwSize + 1;
    pHeader->dwRMID     = m_dwRMID;

    // prepare for next
    nvAssert(m_dwRMID < IPM_MEMORY_OBJECT_HI);
    m_dwRMID ++;

    // add new header to list
    AddToList(pHeader);

    // add new header to free list
    AddToFreeList(pHeader);

    return pHeader;
}

__forceinline void CInterProcessHeap::freeBlock(HEADER *pHeader)
{
#ifdef WINNT
    EngFreeMem((PVOID) pHeader);
#else
    nvAssert(!global.b16BitCode);
    if (pHeader->dwFlags & FLAG_16BIT)
    {
        // this block was allocated from 16 bit land, so we need to use an ESC call to free it
        MyExtEscape(pDXShare->dwHeadNumber, NV_ESC_FREE_MEMORY, sizeof(DWORD), (LPCSTR)&pHeader->dwRMID, 0, NULL);
    }
    else
    {
        NvRmFree (m_dwRootHandle, m_dwDeviceHandle, pHeader->dwRMID);
    }
#endif
}

/*****************************************************************************
 * CInterProcessHeap::destroy
 *
 * destroys the IP heap
 */

void CInterProcessHeap::destroy
(
    void
)
{
    dbgTestIntegrity();

    // release all the blocks in the heap
    for (HEADER *pHeader = m_pBase; pHeader;)
    {
        HEADER *pHeader2 = pHeader->pNext;

        if (pHeader2 && !isBoundary(pHeader2))
        {
            // skip towards end of block
            pHeader->pNext = pHeader2->pNext;
        }
        else
        {
            // free the block
            nvAssert(isBoundary(pHeader));
            freeBlock(pHeader);
            pHeader = pHeader2;
        }
    }

#ifndef WINNT
    DWORD res = NvRmFree (m_dwRootHandle, NV01_NULL_OBJECT, m_dwRootHandle);
    nvAssert(res == NVOS00_STATUS_SUCCESS);
#endif

    // reset
    m_pBase     = NULL;
    m_pBaseFree = NULL;
    m_bInitialized = FALSE;
}

/*****************************************************************************
 * CInterProcessHeap::combineFreeBlocks
 *
 * combine adjacent free blocks so we have larger available space in a
 *  once fragmented heap
 */
void CInterProcessHeap::combineFreeBlocks
(
    void
)
{
    // for all blocks
    for (HEADER *pHeader = m_pBase; pHeader;)
    {
        HEADER *pHeader2 = pHeader->pNext;

        // combinable?
        if (pHeader2
         && isFree(pHeader)
         && !isBoundary(pHeader2)
         && isFree(pHeader2))
        {
            // combine
            pHeader->dwRealSize += pHeader2->dwRealSize;
            pHeader->pNext       = pHeader2->pNext;
            if (pHeader->pNext) pHeader->pNext->pPrev = pHeader;

            RemoveFromFreeList(pHeader2);

            // test
            dbgTestIntegrity();
        }
        else
        {
            pHeader = pHeader2;
        }
    }
}

/*****************************************************************************
 * CInterProcessHeap::releaseFreeBlocks
 *
 * releases any heap blocks that are no longer in use
 * note that the heap must be defragmented first with combineFreeBlocks for this to work properly
 */
void CInterProcessHeap::releaseFreeBlocks
(
    void
)
{
    HEADER *pHeader = m_pBase;

    while (pHeader)
    {
        HEADER *pNext = pHeader->pNext;

        if (pNext)
        {
            // release the block only if all parts are free
            if (isBoundary(pHeader)
             && isFree(pHeader)
             && isBoundary(pNext))
            {
                RemoveFromList(pHeader);
                RemoveFromFreeList(pHeader);
                freeBlock(pHeader);
            }
        }
        else
        {
            // this is the last block
            if (isFree(pHeader)
             && isBoundary(pHeader))
            {
                RemoveFromList(pHeader);
                RemoveFromFreeList(pHeader);
                freeBlock(pHeader);
            }
        }

        // get next header
        pHeader = pNext;
    }
}

/*****************************************************************************

 * CInterProcessHeap::alloc
 *
 * allocates IP memory
 */
void* CInterProcessHeap::alloc
(
#ifdef DEBUG
    DWORD dwSize,
    void *pCaller // = 0
#else
    DWORD dwSize
#endif
)
{
    dbgTestIntegrity();

#ifdef DEBUG
    if (!pCaller)
    {
        _asm mov eax, [EBP]
        _asm mov eax, [eax + 4]
        _asm mov [pCaller], eax
    }
    DPF_LEVEL(NVDBG_LEVEL_HEAP_ACTIVITY, "---IPM---Alloc: %08x bytes, pCaller = %08x", dwSize, pCaller);
#endif // DEBUG

    // find a free block to use. we make three passes:
    //  1) look for a block
    //  2) apply defragmentation, then look again
    //  3) allocate a block we know must be big enough, looking for it cannot fail
    DWORD dwRealSize     = dwSize + sizeof(HEADER) + ALIGN; // size to fit
    DWORD dwRequiredSize = dwRealSize + sizeof(HEADER);     // size for a split

    for (DWORD dwTry = 0; ; dwTry++)
    {
        // look for free space
        for (HEADER *pHeader = m_pBaseFree; pHeader; pHeader = pHeader->pNextFree)
        {
            // will it fit in a block without splitting up?
            if ((pHeader->dwRealSize >= dwRealSize)             // at least big enough
                && (pHeader->dwRealSize <= (dwRealSize + 128))) // not too big
            {
                // found a block
                pHeader->dwFlags    |= FLAG_ALLOCATED;
                pHeader->dwUserSize  = dwSize;
                pHeader->dwUserStart = (DWORD(pHeader) + sizeof(HEADER) + ALIGN - 1) & ~(ALIGN - 1);
                *(DWORD*)(pHeader->dwUserStart - 4) = DWORD(pHeader);
#ifdef DEBUG
                pHeader->pCaller = pCaller;
#endif

                RemoveFromFreeList(pHeader);

                // sanity check
                dbgTestIntegrity();

                // done
                DPF_LEVEL(NVDBG_LEVEL_HEAP_ACTIVITY, "              : ptr = %08x", pHeader->dwUserStart);
                return (void*)pHeader->dwUserStart;
            }
            else
                // will it fit in a large block that we split up?
                if (pHeader->dwRealSize >= dwRequiredSize)
                {
                    // found a block, split it up
                    HEADER *pHeader2     = (HEADER*)(DWORD(pHeader) + dwRealSize);
                    pHeader2->dwFlags    = 0;
                    pHeader2->dwRealSize = pHeader->dwRealSize - dwRealSize;
                    pHeader2->pPrev      = pHeader;
                    pHeader2->pNext      = pHeader->pNext;

                    if (pHeader->pNext) pHeader->pNext->pPrev = pHeader2;

                    AddToFreeList(pHeader2);

                    pHeader->pNext       = pHeader2;
                    pHeader->dwRealSize  = dwRealSize;
                    pHeader->dwFlags    |= FLAG_ALLOCATED;
                    pHeader->dwUserSize  = dwSize;
                    pHeader->dwUserStart = (DWORD(pHeader) + sizeof(HEADER) + ALIGN - 1) & ~(ALIGN - 1);
                    *(DWORD*)(pHeader->dwUserStart - 4) = DWORD(pHeader);
#ifdef DEBUG
                    pHeader->pCaller = pCaller;
#endif

                    RemoveFromFreeList(pHeader);

                    // sanity check
                    assert (pHeader2->dwRealSize >= sizeof(HEADER));
                    assert ((pHeader->dwUserStart + pHeader->dwUserSize) <= DWORD(pHeader2));
                    dbgTestIntegrity();

                    // done
                    DPF_LEVEL(NVDBG_LEVEL_HEAP_ACTIVITY, "              : ptr = %08x", pHeader->dwUserStart);
                    return (void*)pHeader->dwUserStart;
                }
        }

        // do something before we try again
        switch (dwTry)
        {
            case 0:
            {
                // combine adjacent free blocks
                combineFreeBlocks();
                break;
            }
            default:
            {
                // alloc a large enough block
                DWORD dwBlockSize = max(dwRequiredSize,65536);
                      dwBlockSize = (dwBlockSize + 4095) & ~4095;
                if (!create(dwBlockSize))
                {
                    return NULL;
                }
                break;
            }
        }
    }
}

/*****************************************************************************
 * CInterProcessHeap::realloc
 *
 * resize IP memory
 */
BOOL CInterProcessHeap::realloc
(
    void  *pOldMem,
    DWORD  dwNewSize,
    void  **ppNewMem
)
{
    HEADER *pHeader;

#ifdef DEBUG
    void* pCaller;
    _asm mov eax, [EBP]
    _asm mov eax, [eax + 4]
    _asm mov [pCaller], eax
#endif // DEBUG

    DPF_LEVEL(NVDBG_LEVEL_HEAP_ACTIVITY, "---IPM---ReAlloc: ptr = %08x, size = %d", pOldMem, dwNewSize);

    nvAssert(dwNewSize);

    // defragment the heap
    combineFreeBlocks();

    // realloc by resizing the old block if possible
    if (pOldMem)
    {
        pHeader = (HEADER*)*(DWORD*)(DWORD(pOldMem) - 4);
        HEADER *pHeader2 = pHeader->pNext;

        if (pHeader2
         && isFree(pHeader2)
         && !isBoundary(pHeader2))
        {
            DWORD dwCombinedSize = pHeader->dwRealSize + pHeader2->dwRealSize; // total size of combined blocks
            DWORD dwRequiredSize = dwNewSize + sizeof(HEADER) + ALIGN;         // size required

            if (dwCombinedSize >= dwRequiredSize + sizeof(HEADER) + 128) // size required for split with some spare change
            {
                // adjust the free block (pHeader2)
                HEADER *pNewHeader     = (HEADER*)(DWORD(pHeader) + dwRequiredSize);
                pNewHeader->pPrevFree  = pHeader2->pPrevFree; // must set these four first, in this order
                pNewHeader->pPrev      = pHeader2->pPrev;     // in case of overlap between pNewHeader and pHeader2
                pNewHeader->pNextFree  = pHeader2->pNextFree;
                pNewHeader->pNext      = pHeader2->pNext;
                pNewHeader->dwFlags    = 0;                   // mark block as free
                pNewHeader->dwRealSize = dwCombinedSize - dwRequiredSize;

                // update linked references to the adjusted free block
                if (pNewHeader->pPrev)     pNewHeader->pPrev->pNext     = pNewHeader;
                if (pNewHeader->pNext)     pNewHeader->pNext->pPrev     = pNewHeader;
                nvAssert(m_pBase != pHeader2);
                if (pNewHeader->pPrevFree)   pNewHeader->pPrevFree->pNextFree = pNewHeader;
                if (pNewHeader->pNextFree)   pNewHeader->pNextFree->pPrevFree = pNewHeader;
                if (m_pBaseFree == pHeader2) m_pBaseFree = pNewHeader;

                // enlarge the allocated block (pHeader)
                nvAssert(isAlloc(pHeader));
                pHeader->dwRealSize  = dwRequiredSize;
                pHeader->dwUserSize  = dwNewSize;

                dbgTestIntegrity();

                *ppNewMem = pOldMem;
                return TRUE;
            }
            else if (dwCombinedSize >= dwRequiredSize)
            {
                // combine
                pHeader->dwRealSize += pHeader2->dwRealSize;
                pHeader->dwUserSize  = dwNewSize;
                pHeader->pNext       = pHeader2->pNext;
                if (pHeader->pNext) pHeader->pNext->pPrev = pHeader;

                RemoveFromFreeList(pHeader2);

                dbgTestIntegrity();

                *ppNewMem = pOldMem;
                return TRUE;
            }
        }
    }

    // realloc by copying to new block
#ifdef DEBUG
    *ppNewMem = alloc(dwNewSize, pCaller);
#else // !DEBUG
    *ppNewMem = alloc(dwNewSize);
#endif // !DEBUG
    if (!*ppNewMem) return FALSE;

    if (pOldMem)
    {
        DPF_LEVEL(NVDBG_LEVEL_HEAP_ACTIVITY, "                : memcpy (%08x, %08x, %d)", *ppNewMem, pOldMem, pHeader->dwUserSize);
        memcpy (*ppNewMem, pOldMem, pHeader->dwUserSize);
        free (pOldMem);
    }

    return TRUE;
}

/*****************************************************************************
 * CInterProcessHeap::free
 *
 * free IP memory
 */
void CInterProcessHeap::free
(
    void *pMemory
)
{
    dbgTestIntegrity();

    DPF_LEVEL(NVDBG_LEVEL_HEAP_ACTIVITY, "---IPM---Free: ptr = %08x", pMemory);

    // get header
    HEADER *pHeader = (HEADER*)*(DWORD*)(DWORD(pMemory) - 4);

    // this will assert if we try to free the same memory twice
    nvAssert(pHeader->dwFlags & FLAG_ALLOCATED);

    // free it
    if (pHeader->dwFlags & FLAG_ALLOCATED)
    {
        pHeader->dwFlags &= ~FLAG_ALLOCATED;
        AddToFreeList(pHeader);
    }

    // done
    dbgTestIntegrity();
}

#endif  // NVARCH >= 0x04
