/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    netpool.c

Abstract:

    Private pool used by the network stack

Notes:

    The private pool is used by the network stack
    when processing asynchronously received packets.
    We don't want to allocate directly out of
    the system pool because that would introduce
    unpredictable memory usage patterns.

Revision History:

    08/03/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Pool allocation block header: We intentionally let this have
// the same size as the system pool header. But we've restructured
// the fields in such a way that if you try to free a memory block
// that was allocated out of our private pool to the system pool,
// you'd get a bugcheck.
//

typedef struct _PoolEntry {
    WORD busy;
        // Whether the block is allocated or free

    WORD tag;
        // We're only using 2 bytes for pool tag,
        // since all of our pool tags have the form NET*.

    WORD blockSize;
    WORD previousSize;
        // Size of this block and the previous block in 32-bit units

    LIST_ENTRY links;
        // A free pool entry has two additional pointer fields
        // for maintaining a doubly-linked list of free blocks.
} PoolEntry;

#define POOL_BLOCK_SHIFT 5
#define POOL_BLOCK_SIZE (1 << POOL_BLOCK_SHIFT)
#define MAX_POOL_SIZE (POOL_BLOCK_SIZE * 0xffff)
#define POOL_HEADER_SIZE offsetof(PoolEntry, links)

#define MarkPoolEntryBusy(_entry) ((_entry)->busy = 0x100)
#define MarkPoolEntryFree(_entry) ((_entry)->busy = 0)
#define IsPoolEntryBusy(_entry) ((_entry)->busy == 0x100)
#define IsPoolEntryFree(_entry) ((_entry)->busy == 0)
#define TagPoolEntry(_entry, _tag) ((_entry)->tag = (WORD) ((_tag) >> 16))

#define GetPoolEntryHeader(_ptr) \
        ((PoolEntry*) ((BYTE*) (_ptr) - POOL_HEADER_SIZE))
#define GetPoolEntryNext(_entry) \
        ((PoolEntry*) ((BYTE*) (_entry) + ((UINT) (_entry)->blockSize << POOL_BLOCK_SHIFT)))
#define GetPoolEntryPrev(_entry) \
        ((PoolEntry*) ((BYTE*) (_entry) - ((UINT) (_entry)->previousSize << POOL_BLOCK_SHIFT)))

//
// Default number of pages used for our private pool (48KB)
//
UINT cfgXnetPoolSize = 48*1024;

//
// Beginning and ending address of the entire pool
//
#define MAX_SMALL_BLOCKS 5
typedef struct _XnetPoolData {
    PoolEntry* start;
    PoolEntry* end;
    LIST_ENTRY freeLists[MAX_SMALL_BLOCKS+1];
        // List of free pool blocks:
        //  we keep a separate list for each type of small blocks
        //  and the last list is for all large blocks
} XnetPoolData;
XnetPoolData _XnetPool;

#define IsPoolEntryValid(pool,_entry) ((_entry) >= (pool)->start && (_entry) < (pool)->end)

#define InsertFreePoolEntry(pool,_entry) { \
            LIST_ENTRY* _head; \
            _head = &(pool)->freeLists[ \
                        (_entry)->blockSize <= MAX_SMALL_BLOCKS ? \
                            (_entry)->blockSize - 1 : \
                            MAX_SMALL_BLOCKS]; \
            InsertHeadList(_head, &(_entry)->links); \
        }

// Set this variable to TRUE to force a breakpoint
// when xnet pool allocation fails.
#if DBG
BOOL XnetBreakWhenPoolEmpty;
#endif


PRIVATE VOID
_XnetPoolInit(
    XnetPoolData* pool,
    VOID* bufstart,
    UINT bufsize
    )

/*++

Routine Description:

    Initialize a private memory pool

Arguments:

    pool - Points to the pool data structure
    bufstart - Starting address of the memory pool buffer
    bufsize - Size of the memory buffer

Return Value:

    NONE

--*/

{
    PoolEntry* entry;
    UINT n;

    pool->start = (PoolEntry*) bufstart;
    pool->end = (PoolEntry*) ((BYTE*) bufstart + bufsize);

    // Initialize the free lists with a single big block

    for (n=0; n <= MAX_SMALL_BLOCKS; n++) {
        InitializeListHead(&pool->freeLists[n]);
    }

    // NOTE: we reserved the very first and the very last
    // pool blocks. This saves us from a couple of extra checks
    // during alloc and free.

    entry = pool->start;
    MarkPoolEntryBusy(entry);
    TagPoolEntry(entry, PTAG_POOL);
    entry->previousSize = 0;
    entry->blockSize = 1;

    entry = GetPoolEntryNext(entry);
    n = (bufsize >> POOL_BLOCK_SHIFT) - 2;
    MarkPoolEntryFree(entry);
    TagPoolEntry(entry, PTAG_POOL);
    entry->previousSize = 1;
    entry->blockSize = (WORD) n;
    InsertFreePoolEntry(pool, entry);

    entry = GetPoolEntryNext(entry);
    MarkPoolEntryBusy(entry);
    TagPoolEntry(entry, PTAG_POOL);
    entry->previousSize = (WORD) n;
    entry->blockSize = 1;
}


#if DBG

PRIVATE VOID
_XnetPoolDump(
    XnetPoolData* pool
    )

/*++

Routine Description:

    Dump out the content of a private pool

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    PoolEntry* entry;
    PoolEntry* prev;
    UINT total = 0;

    DbgPrint("XNET pool %x: %x - %x\n", pool, pool->start, pool->end);

    prev = NULL;
    entry = pool->start;

    while (entry < pool->end) {
        BOOL isbusy = IsPoolEntryBusy(entry);
        if (isbusy) total += entry->blockSize;

        DbgPrint("%c%c%c %08x l%x\n",
            isbusy ? '*' : ' ',
            entry->tag,
            entry->tag >> 8,
            entry,
            entry->blockSize << POOL_BLOCK_SHIFT);

        if (prev) {
            ASSERT(prev->blockSize == entry->previousSize);
            prev = entry;
        }
        entry = GetPoolEntryNext(entry);
    }

    DbgPrint(" Total allocation: %d / %d\n",
        total << POOL_BLOCK_SHIFT,
        (BYTE*) pool->end - (BYTE*) pool->start);
}

#endif // DBG

PRIVATE VOID*
_XnetAlloc(
    XnetPoolData* pool,
    SIZE_T size,
    ULONG tag
    )

/*++

Routine Description:

    Allocate memory from a private pool

Arguments:

    pool - Points to the pool data structure
    size - Specifies the number of bytes needed
    tag - Specifies the memory tag for the caller

Return Value:

    Pointer to the allocated memory buffer
    NULL if we're out of memory

--*/

{
    PoolEntry* entry;
    KIRQL irql;

    ASSERT(size > 0 && size <= MAX_POOL_SIZE - POOL_HEADER_SIZE);
    size = (size + (POOL_HEADER_SIZE + POOL_BLOCK_SIZE - 1)) >> POOL_BLOCK_SHIFT;

    irql = RaiseToDpc();

    // Find a free pool block that's large enough for us.
    // Handle small block allocation with the quick lookup.
    if (size <= MAX_SMALL_BLOCKS) {
        UINT index;

        for (index=size-1; index <= MAX_SMALL_BLOCKS; index++) {
            if (!IsListEmpty(&pool->freeLists[index])) {
                entry = GetPoolEntryHeader(pool->freeLists[index].Flink);
                goto found;
            }
        }
    } else {
        LIST_ENTRY* head;
        LIST_ENTRY* list;

        head = &pool->freeLists[MAX_SMALL_BLOCKS];
        list = head->Flink;
        while (list != head) {
            entry = GetPoolEntryHeader(list);
            if ((WORD) size <= entry->blockSize) goto found;
            list = list->Flink;
        }
    }

    #if DBG

    WARNING_("Xnet%s pool allocation failed: pool %x, %d blocks",
        XnetInsideDbgmon() ? " (XBDM)" : "", pool, size);
    WARNING_("Make sure you're promptly calling recv() to receive incoming data.\n");
    WARNING_("You may also want to consider setting a larger Xnet pool size.\n");

    if (XnetBreakWhenPoolEmpty || XnetInsideDbgmon()) {
        _XnetPoolDump(pool);
        __asm int 3
    }

    #endif

    LowerFromDpc(irql);
    return NULL;

found:
    // Take it out of the free list
    RemoveEntryList(&entry->links);

    // If we didn't use up the entire free block,
    // put the remaining portion back on the free list.
    if (entry->blockSize > size) {
        UINT leftover = entry->blockSize - size;
        PoolEntry* next;

        entry->blockSize = (WORD) size;
        next = GetPoolEntryNext(entry);
        next->blockSize = (WORD) leftover;
        next->previousSize = (WORD) size;
        next->tag = entry->tag;
        MarkPoolEntryFree(next);
        InsertFreePoolEntry(pool, next);

        next = GetPoolEntryNext(next);
        ASSERT(IsPoolEntryValid(pool, next));
        next->previousSize = (WORD) leftover;
    }

    MarkPoolEntryBusy(entry);
    TagPoolEntry(entry, tag);
    LowerFromDpc(irql);

    return (BYTE*) entry + POOL_HEADER_SIZE;
}


PRIVATE VOID
_XnetFree(
    XnetPoolData* pool,
    VOID* ptr
    )

/*++

Routine Description:

    Free the memory buffer to a private pool

Arguments:

    pool - Points to the pool data structure
    ptr - Points to the memory buffer to be freed

Return Value:

    NONE

--*/

{
    PoolEntry* entry;
    PoolEntry* prev;
    PoolEntry* next;
    WORD blockSize;
    KIRQL irql;

    if (!ptr) return;
    entry = GetPoolEntryHeader(ptr);
    ASSERT(IsPoolEntryValid(pool, entry) && IsPoolEntryBusy(entry));

    irql = RaiseToDpc();
    MarkPoolEntryFree(entry);
    blockSize = entry->blockSize;

    //
    // Check to see if the block before this one is free
    // If so, collapse the two free blocks together
    //
    prev = GetPoolEntryPrev(entry);
    ASSERT(IsPoolEntryValid(pool, prev));
    if (IsPoolEntryFree(prev)) {
        RemoveEntryList(&prev->links);
        prev->blockSize = (WORD) (prev->blockSize + blockSize);
        entry = prev;
    }

    //
    // Check if the block after this one is free
    // If so, collapse the two free blocks together
    //
    next = GetPoolEntryNext(entry);
    ASSERT(IsPoolEntryValid(pool, next) && next->previousSize == blockSize);
    if (IsPoolEntryFree(next)) {
        RemoveEntryList(&next->links);
        entry->blockSize = (WORD) (entry->blockSize + next->blockSize);
    }

    //
    // If we performed any collapsing, 
    // update the previousSize field of the next block.
    //
    if (entry->blockSize != blockSize) {
        next = GetPoolEntryNext(entry);
        ASSERT(IsPoolEntryValid(pool, next));
        next->previousSize = entry->blockSize;
    }

    InsertFreePoolEntry(pool, entry);
    LowerFromDpc(irql);
}


//
// Public functions
//

NTSTATUS XnetPoolInit()
{
    VOID* p;

    // Preallocate all the memory we need
    if (cfgXnetPoolSize > MAX_POOL_SIZE)
        cfgXnetPoolSize = MAX_POOL_SIZE;

    p = SysAlloc(cfgXnetPoolSize, PTAG_POOL);
    if (!p) return NETERR_MEMORY;
    _XnetPoolInit(&_XnetPool, p, cfgXnetPoolSize);
    return NETERR_OK;
}

VOID XnetPoolCleanup()
{
    if (_XnetPool.start) {
        SysFree(_XnetPool.start);
        _XnetPool.start = _XnetPool.end = NULL;
    }
}

VOID* XnetAlloc(SIZE_T size, ULONG tag)
{
    return _XnetAlloc(&_XnetPool, size, tag);
}

VOID XnetFree(VOID* p) {
    _XnetFree(&_XnetPool, p);
}


//
// BUGBUG: temporary workaround for nv2a snooping bug
//
#ifdef DVTSNOOPBUG

#define XNET_UNCACHED_POOL_SIZE (256*1024)
XnetPoolData XnetUncachedPool;

PRIVATE VOID* XnetUncachedAlloc(SIZE_T size, ULONG tag)
{
    return _XnetAlloc(&XnetUncachedPool, size, tag);
}

PRIVATE VOID XnetUncachedFree(VOID* p)
{
    _XnetFree(&XnetUncachedPool, p);
}

NTSTATUS XnetUncachedPoolInit()
{
    VOID* p;
    
    p = XnetInsideDbgmon() ?
        MmDbgAllocateMemory(XNET_UNCACHED_POOL_SIZE, PAGE_READWRITE|PAGE_NOCACHE) :
        MmAllocateContiguousMemoryEx(
                XNET_UNCACHED_POOL_SIZE,
                0,
                MAXULONG_PTR,
                0,
                PAGE_READWRITE|PAGE_NOCACHE);
    if (!p) return NETERR_MEMORY;

    __asm wbinvd;
    XnetUncachedAllocProc = XnetUncachedAlloc;
    XnetUncachedFreeProc = XnetUncachedFree;
    
    _XnetPoolInit(&XnetUncachedPool, p, XNET_UNCACHED_POOL_SIZE);
    return NETERR_OK;
}

VOID XnetUncachedPoolCleanup()
{
    if (XnetUncachedPool.start) {
        if (XnetInsideDbgmon()) {
            MmDbgFreeMemory(XnetUncachedPool.start, 0);
        } else {
            MmFreeContiguousMemory(XnetUncachedPool.start);
        }
        XnetUncachedPool.start = XnetUncachedPool.end = NULL;
        XnetUncachedAllocProc = NULL;
        XnetUncachedFreeProc = NULL;
    }
}

#endif // DVTSNOOPBUG

