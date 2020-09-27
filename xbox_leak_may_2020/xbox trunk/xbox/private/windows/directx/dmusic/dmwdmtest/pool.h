/*  Memory pool manager

    05/26/98    Jim Geist           Created this file

*/

#ifndef __Pool_H__
#define __Pool_H__

class CMemoryPool
{
public:
    CMemoryPool(ULONG cbObject, ULONG cbAllocationChunk = 0, ULONG cAlignBits = 0);
    ~CMemoryPool();

    PVOID Alloc();
    void  Free(PVOID pObject);

private:
    
    struct PoolNode
    {
        PoolNode        *next;
    };

    struct PoolBlock
    {
        PoolBlock       *next;
    };

    ULONG               m_cbObject;
    ULONG               m_cbHeader;
    ULONG               m_nodesInBlock;

    PoolBlock          *m_poolBlocks;
    PoolNode           *m_freeList;
    ULONG               m_freeNodes;
    ULONG               m_freeLowWaterMark;

private:
    void Refill();
};

#endif // __Pool_H__