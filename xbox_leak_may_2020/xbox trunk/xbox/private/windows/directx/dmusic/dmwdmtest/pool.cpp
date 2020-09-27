/*  Memory pool manager

    05/26/98    Jim Geist           Created this file

*/

#include "stdafx.h"
#include <assert.h>
#include "Pool.h"

// CMemoryPool::CMemoryPool
// 
CMemoryPool::CMemoryPool(ULONG cbObject, ULONG cbAllocationChunk, ULONG cAlignBits)
{
    if (!cbAllocationChunk)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        cbAllocationChunk = si.dwPageSize;
    }

    if (cbObject < sizeof(PoolNode))
    {
        cbObject = sizeof(PoolNode);
    }

    // XXX What is the alignment of memory returned by ExAllocatePoolTag? This stuff is only going
    // to be that good unless we really play games.
    //
    ULONG alignMask = (1 << cAlignBits) - 1;
    cbObject = (cbObject + alignMask) & ~alignMask;

    m_cbObject = cbObject;

    ULONG cbHeader = sizeof(PoolBlock);
    cbHeader = (cbHeader + alignMask) & ~alignMask;
    
    m_cbHeader = cbHeader; 
    
    for (;;)
    {
        m_nodesInBlock = (cbAllocationChunk - cbHeader) / cbObject;
        if (m_nodesInBlock >= 8)
        {
            break;
        }

        cbAllocationChunk *= 2;
    }

    //Trace("cbObject %d  cbAllocationChunk %d", cbObject, cbAllocationChunk);
    //    

    m_poolBlocks        = NULL;
    m_freeList          = NULL;
    m_freeNodes         = 0;
    m_freeLowWaterMark  = m_nodesInBlock / 10;
    if (m_freeLowWaterMark < 4)
    {
        m_freeLowWaterMark = 4;
    }

    Refill();
}

// CMemoryPool::~CMemoryPool
//
CMemoryPool::~CMemoryPool()
{
    while (m_poolBlocks)
    {
        PoolBlock *next = m_poolBlocks->next;

        // XXX ExAllocatePoolTag
        //
        delete[] m_poolBlocks;
        m_poolBlocks = next;
    }    
}

// CMemoryPool::Alloc
//
PVOID CMemoryPool::Alloc()
{
    if (m_freeNodes < m_freeLowWaterMark)
    {
        Refill(); 
    }

    PoolNode *pNew = m_freeList;
    if (pNew)
    {
        m_freeList = pNew->next;
        assert(m_freeNodes);
        m_freeNodes--;
    }

    return (PVOID)pNew;
}

// CMemoryPool::Free
//
void CMemoryPool::Free(PVOID pObject)
{
    PoolNode *pNode = (PoolNode*)pObject;

    pNode->next = m_freeList;
    m_freeList = pNode;
    m_freeNodes++;
}

// CMemoryPool::Refill
//
void CMemoryPool::Refill(void)
{
    // XXX Kernel check at passive level here
    //

    // XXX ExAllocatePoolTag
    //
    LPBYTE pbBlock = new BYTE[m_cbHeader + m_nodesInBlock * m_cbObject];

    if (!pbBlock)
    {
        return; 
    }

    // Link block into block chain so we can free it.
    //    
    PoolBlock *pBlock = (PoolBlock*)pbBlock;
    pBlock->next = m_poolBlocks;
    m_poolBlocks = pBlock;

    // Now walk the block and add nodes to the free list.
    //
    pbBlock += m_cbHeader;
    LPBYTE pbBlockEnd = pbBlock + (m_nodesInBlock - 1) * m_cbObject;

    PoolNode *pChainHead = (PoolNode*)pbBlock;    

    while (pbBlock != pbBlockEnd)
    {
        ((PoolNode*)pbBlock)->next = (PoolNode*)(pbBlock + m_cbObject);

        pbBlock += m_cbObject;
    }

    ((PoolNode*)pbBlock)->next = m_freeList;
    m_freeList = pChainHead;
    m_freeNodes += m_nodesInBlock;                
}
