/*
 *
 * dmpool.c
 *
 * copied from ex\pool.c
 *
 */

#include "dmp.h"
#include "pool.h"

//
// FREE_CHECK_KTIMER - If enabled causes each free pool to verify no
// active KTIMERs are in the pool block being freed.
//

#if 0

#define FREE_CHECK_KTIMER(Va, NumberOfBytes) \
            KeCheckForTimer(Va, NumberOfBytes)

#else

#define FREE_CHECK_KTIMER(Va, NumberOfBytes)

#endif


//
// We redefine the LIST_ENTRY macros to have each pointer biased
// by one so any rogue code using these pointers will access
// violate.  See \nt\public\sdk\inc\ntrtl.h for the original
// definition of these macros.
//
// This is turned off in the shipping product.
//

#ifndef NO_POOL_CHECKS

ULONG ExpPoolBugCheckLine;

#define DecodeLink(Link) ((PLIST_ENTRY)((ULONG_PTR)(Link) & ~1))
#define EncodeLink(Link) ((PLIST_ENTRY)((ULONG_PTR)(Link) |  1))

#define PrivateInitializeListHead(ListHead) (                     \
    (ListHead)->Flink = (ListHead)->Blink = EncodeLink(ListHead))

#define PrivateIsListEmpty(ListHead)              \
    (DecodeLink((ListHead)->Flink) == (ListHead))

#define PrivateRemoveHeadList(ListHead)                     \
    DecodeLink((ListHead)->Flink);                          \
    {PrivateRemoveEntryList(DecodeLink((ListHead)->Flink))}

#define PrivateRemoveTailList(ListHead)                     \
    DecodeLink((ListHead)->Blink);                          \
    {PrivateRemoveEntryList(DecodeLink((ListHead)->Blink))}

#define PrivateRemoveEntryList(Entry) {       \
    PLIST_ENTRY _EX_Blink;                    \
    PLIST_ENTRY _EX_Flink;                    \
    _EX_Flink = DecodeLink((Entry)->Flink);   \
    _EX_Blink = DecodeLink((Entry)->Blink);   \
    _EX_Blink->Flink = EncodeLink(_EX_Flink); \
    _EX_Flink->Blink = EncodeLink(_EX_Blink); \
    }

#define PrivateInsertTailList(ListHead,Entry) {  \
    PLIST_ENTRY _EX_Blink;                       \
    PLIST_ENTRY _EX_ListHead;                    \
    _EX_ListHead = (ListHead);                   \
    _EX_Blink = DecodeLink(_EX_ListHead->Blink); \
    (Entry)->Flink = EncodeLink(_EX_ListHead);   \
    (Entry)->Blink = EncodeLink(_EX_Blink);      \
    _EX_Blink->Flink = EncodeLink(Entry);        \
    _EX_ListHead->Blink = EncodeLink(Entry);     \
    }

#define PrivateInsertHeadList(ListHead,Entry) {  \
    PLIST_ENTRY _EX_Flink;                       \
    PLIST_ENTRY _EX_ListHead;                    \
    _EX_ListHead = (ListHead);                   \
    _EX_Flink = DecodeLink(_EX_ListHead->Flink); \
    (Entry)->Flink = EncodeLink(_EX_Flink);      \
    (Entry)->Blink = EncodeLink(_EX_ListHead);   \
    _EX_Flink->Blink = EncodeLink(Entry);        \
    _EX_ListHead->Flink = EncodeLink(Entry);     \
    }

#define CHECK_LIST(LINE,LIST,ENTRY)                                         \
    if ((DecodeLink(DecodeLink((LIST)->Flink)->Blink) != (LIST)) ||         \
        (DecodeLink(DecodeLink((LIST)->Blink)->Flink) != (LIST))) {         \
            ExpPoolBugCheckLine = LINE;                                     \
            KeBugCheckEx (BAD_POOL_HEADER,                                  \
                          3,                                                \
                          (ULONG_PTR)LIST,                                  \
                          (ULONG_PTR)DecodeLink(DecodeLink((LIST)->Flink)->Blink),     \
                          (ULONG_PTR)DecodeLink(DecodeLink((LIST)->Blink)->Flink));    \
    }

#define CHECK_POOL_HEADER(LINE,ENTRY) {                                                 \
    PPOOL_HEADER PreviousEntry;                                                         \
    PPOOL_HEADER NextEntry;                                                             \
    if ((ENTRY)->PreviousSize != 0) {                                                   \
        PreviousEntry = (PPOOL_HEADER)((PPOOL_BLOCK)(ENTRY) - (ENTRY)->PreviousSize);   \
        if (PreviousEntry->BlockSize != (ENTRY)->PreviousSize) {                        \
            ExpPoolBugCheckLine = LINE;                                     \
            KeBugCheckEx(BAD_POOL_HEADER, 5, (ULONG_PTR)PreviousEntry, LINE, (ULONG_PTR)ENTRY); \
        }                                                                               \
    }                                                                                   \
    NextEntry = (PPOOL_HEADER)((PPOOL_BLOCK)(ENTRY) + (ENTRY)->BlockSize);              \
    if (!PAGE_END(NextEntry)) {                                                         \
        if (NextEntry->PreviousSize != (ENTRY)->BlockSize) {                            \
            ExpPoolBugCheckLine = LINE;                                     \
            KeBugCheckEx(BAD_POOL_HEADER, 5, (ULONG_PTR)NextEntry, LINE, (ULONG_PTR)ENTRY);     \
        }                                                                               \
    }                                                                                   \
}

#define ASSERT_ALLOCATE_IRQL(_NumberOfBytes)                            \
    if (KeGetCurrentIrql() > DISPATCH_LEVEL) {                          \
        KeBugCheckEx (BAD_POOL_CALLER, 8, KeGetCurrentIrql(), 0, _NumberOfBytes);                                                            \
    }

#define ASSERT_FREE_IRQL(_P)                                            \
    if (KeGetCurrentIrql() > DISPATCH_LEVEL) {                          \
        KeBugCheckEx (BAD_POOL_CALLER, 9, KeGetCurrentIrql(), 0, (ULONG_PTR)P);                                                              \
    }

#define ASSERT_POOL_NOT_FREE(_Entry)                                    \
    if ((_Entry->PoolType & POOL_TYPE_MASK) == 0) {                     \
        KeBugCheckEx (BAD_POOL_CALLER, 6, __LINE__, (ULONG_PTR)_Entry, _Entry->Ulong1);                                                                 \
    }

#define ASSERT_POOL_TYPE_NOT_ZERO(_Entry)                               \
    if (_Entry->PoolType == 0) {                                        \
        KeBugCheckEx(BAD_POOL_CALLER, 1, (ULONG_PTR)_Entry, (ULONG_PTR)(*(PULONG)_Entry), 0);                                                           \
    }

#define CHECK_LOOKASIDE_LIST(LINE,LIST,ENTRY) {NOTHING;}

#else

#define DecodeLink(Link) ((PLIST_ENTRY)((ULONG_PTR)(Link)))
#define EncodeLink(Link) ((PLIST_ENTRY)((ULONG_PTR)(Link)))
#define PrivateInitializeListHead InitializeListHead
#define PrivateIsListEmpty        IsListEmpty
#define PrivateRemoveHeadList     RemoveHeadList
#define PrivateRemoveTailList     RemoveTailList
#define PrivateRemoveEntryList    RemoveEntryList
#define PrivateInsertTailList     InsertTailList
#define PrivateInsertHeadList     InsertHeadList

#define ASSERT_ALLOCATE_IRQL(_P)            {NOTHING;}
#define ASSERT_FREE_IRQL(_P)                {NOTHING;}
#define ASSERT_POOL_NOT_FREE(_Entry)        {NOTHING;}
#define ASSERT_POOL_TYPE_NOT_ZERO(_Entry)   {NOTHING;}

//
// The check list macros come in two flavors - there is one in the checked
// and free build that will bugcheck the system if a list is ill-formed, and
// there is one for the final shipping version that has all the checked
// disabled.
//
// The check lookaside list macros also comes in two flavors and is used to
// verify that the look aside lists are well formed.
//
// The check pool header macro (two flavors) verifies that the specified
// pool header matches the preceeding and succeeding pool headers.
//

#define CHECK_LIST(LINE,LIST,ENTRY)         {NOTHING;}
#define CHECK_POOL_HEADER(LINE,ENTRY)       {NOTHING;}

#define CHECK_LOOKASIDE_LIST(LINE,LIST,ENTRY) {NOTHING;}

#define CHECK_POOL_PAGE(PAGE) \
    {                                                                         \
        PPOOL_HEADER P = (PPOOL_HEADER)(((ULONG_PTR)(PAGE)) & ~(PAGE_SIZE-1));    \
        ULONG SIZE, LSIZE;                                                    \
        LOGICAL FOUND=FALSE;                                                  \
        LSIZE = 0;                                                            \
        SIZE = 0;                                                             \
        do {                                                                  \
            if (P == (PPOOL_HEADER)PAGE) {                                    \
                FOUND = TRUE;                                                 \
            }                                                                 \
            if (P->PreviousSize != LSIZE) {                                   \
                DbgPrint("DMPOOL: Inconsistent size: ( %lx ) - %lx->%u != %u\n",\
                         PAGE, P, P->PreviousSize, LSIZE);                    \
                DbgBreakPoint();                                              \
            }                                                                 \
            LSIZE = P->BlockSize;                                             \
            SIZE += LSIZE;                                                    \
            P = (PPOOL_HEADER)((PPOOL_BLOCK)P + LSIZE);                       \
        } while ((SIZE < (PAGE_SIZE / POOL_SMALLEST_BLOCK)) &&                \
                 (PAGE_END(P) == FALSE));                                     \
        if ((PAGE_END(P) == FALSE) || (FOUND == FALSE)) {                     \
            DbgPrint("DMPOOL: Inconsistent page: %lx\n",P);                     \
            DbgBreakPoint();                                                  \
        }                                                                     \
    }

#endif


#define MAX_TRACKER_TABLE   1025
#define MAX_BIGPAGE_TABLE   4096
// #define MAX_TRACKER_TABLE   5
// #define MAX_BIGPAGE_TABLE   4

ULONG FirstPrint;

PPOOL_TRACKER_TABLE PoolTrackTable;
SIZE_T PoolTrackTableSize;
SIZE_T PoolTrackTableMask;

PPOOL_TRACKER_BIG_PAGES PoolBigPageTable;
SIZE_T PoolBigPageTableSize;
SIZE_T PoolBigPageTableHash;

FORCEINLINE BOOL FIsDmPool(PVOID P)
{
    return ((ULONG)P & 0xF0000000) == 0xB0000000;
}

VOID
ExpInsertPoolTracker (
    IN ULONG Key,
    IN SIZE_T Size
    );

VOID
ExpRemovePoolTracker (
    IN ULONG Key,
    IN ULONG Size
    );

LOGICAL
ExpAddTagForBigPages (
    IN PVOID Va,
    IN ULONG Key,
    IN ULONG NumberOfPages
    );

ULONG
ExpFindAndRemoveTagBigPages (
    IN PVOID Va
    );

//
// Define macros to pack and unpack a pool index.
//

#define MARK_POOL_HEADER_ALLOCATED(POOLHEADER)      {(POOLHEADER)->PoolIndex = 0x80;}
#define MARK_POOL_HEADER_FREED(POOLHEADER)          {(POOLHEADER)->PoolIndex = 0;}
#define IS_POOL_HEADER_MARKED_ALLOCATED(POOLHEADER) ((POOLHEADER)->PoolIndex == 0x80)

//
// Pool descriptors for nonpaged pool and nonpaged pool must succeed are
// static.
//

POOL_DESCRIPTOR DmPoolDescriptor;

KSPIN_LOCK ExpTaggedPoolLock;

//
// Define paged and nonpaged pool lookaside descriptors.
//

POOL_LOOKASIDE_LIST ExpSmallDmPoolLookasideLists[POOL_SMALL_LISTS];


//
// LOCK_POOL and UNLOCK_POOL are only used within this module.
//

#define LOCK_POOL(LockHandle) {                                                \
    LockHandle = KeRaiseIrqlToDpcLevel();                                      \
}

#define UNLOCK_POOL(LockHandle) {                                              \
    KeLowerIrql(LockHandle);                                                   \
}

typedef struct _DMPP {
    struct _DMPP *pdmppNext;
    ULONG cpg;
    struct _DMPP *pdmppSucc;
} DMPP, *PDMPP;

static PDMPP pdmppHead;
static ULONG cpgDmppList;
static const ULONG cpgDmppMax = ((64 * 1024) >> PAGE_SHIFT);

void QueueDmpp(PDMPP pdmpp)
{
    PDMPP *ppdmpp;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    /* First see whether we can coalesce this block with another */
    pdmpp->pdmppSucc = (PDMPP)((ULONG_PTR)pdmpp + (pdmpp->cpg << PAGE_SHIFT));
    ppdmpp = &pdmppHead;
    while(*ppdmpp) {
        if(pdmpp->pdmppSucc == *ppdmpp) {
            /* We precede another block */
            pdmpp->pdmppSucc = (*ppdmpp)->pdmppSucc;
            cpgDmppList -= (*ppdmpp)->cpg;
            pdmpp->cpg += (*ppdmpp)->cpg;
            *ppdmpp = (*ppdmpp)->pdmppNext;
        } else if((*ppdmpp)->pdmppSucc == pdmpp) {
            /* We follow another block */
            (*ppdmpp)->pdmppSucc = pdmpp->pdmppSucc;
            cpgDmppList -= (*ppdmpp)->cpg;
            (*ppdmpp)->cpg += pdmpp->cpg;
            pdmpp = *ppdmpp;
            *ppdmpp = (*ppdmpp)->pdmppNext;
        } else
            ppdmpp = &(*ppdmpp)->pdmppNext;
    }

    /* Now insert our possibly coalesced block back into the list */
    ppdmpp = &pdmppHead;
    while(*ppdmpp && (*ppdmpp)->cpg < pdmpp->cpg)
        ppdmpp = &(*ppdmpp)->pdmppNext;
    pdmpp->pdmppNext = *ppdmpp;
    *ppdmpp = pdmpp;
    cpgDmppList += pdmpp->cpg;
}

ULONG
DmpFreePoolMemory(
    IN PVOID pvFree,
    IN SIZE_T cb
    )
{
    KIRQL irqlSav;
    int cpgNeed;
    int cpg;
    PMMPTE pmmpte;
    PDMPP pdmpp;

    irqlSav = KeRaiseIrqlToDpcLevel();

    /* If we need to retain some of this memory in the free page pool, figure
     * out how much */
    if(cpgDmppList < cpgDmppMax) {
        cpgNeed = cpgDmppMax - cpgDmppList;
        if(cb != 0)
            cpg = (cb + PAGE_SIZE - 1) >> PAGE_SHIFT;
        else {
            pmmpte = MiGetPteAddress(pvFree);
            for(cpg = 1; !pmmpte->Hard.GuardOrEndOfAllocation; ++cpg, ++pmmpte);
        }
        if(cpg < cpgNeed)
            cpgNeed = cpg;

        /* The front end of the block is going to go into the free list */
        pdmpp = (PDMPP)pvFree;
        pdmpp->cpg = cpgNeed;
        QueueDmpp(pdmpp);

        /* The tail end of the block, if any, will be sent back to the memory
         * manager */
        if(cpg != cpgNeed) 
            DmpFreePoolMemory((PVOID)((ULONG_PTR)pvFree + (cpgNeed << PAGE_SHIFT)),
                (cpg - cpgNeed) << PAGE_SHIFT);
    } else
        /* The whole block can be sent back */
        cpg = MmDbgFreeMemory(pvFree, cb);

    KeLowerIrql(irqlSav);
    
    return cpg;
}

PVOID DmpAllocatePoolMemory(
    IN SIZE_T cb
    )
{
    KIRQL irqlSav;
    ULONG cpgNeed;
    ULONG cpg;
    PMMPTE pmmpte;
    PDMPP pdmpp, pdmppTail, *ppdmpp;
    PVOID pv;

    /* We go to the mm first */
    pv = MmDbgAllocateMemory(cb, PAGE_READWRITE);
    if(pv)
        return pv;

    /* No luck; we're going to have to pull off our pool */
    irqlSav = KeRaiseIrqlToDpcLevel();

    /* See if we can find a sufficiently large free block */
    cpgNeed = (cb + PAGE_SIZE - 1) >> PAGE_SHIFT;
    ppdmpp = &pdmppHead;
    while(*ppdmpp && (*ppdmpp)->cpg < cpgNeed)
        ppdmpp = &(*ppdmpp)->pdmppNext;

    if(*ppdmpp) {
        /* Got one.  Remove it from the list */
        pdmpp = *ppdmpp;
        *ppdmpp = pdmpp->pdmppNext;
        cpgDmppList -= pdmpp->cpg;

        /* If we have a tail end, put it back on the free list */
        if(pdmpp->cpg > cpgNeed) {
            pdmppTail = (PDMPP)((ULONG_PTR)pdmpp + (cpgNeed << PAGE_SHIFT));
            pdmppTail->cpg = pdmpp->cpg - cpgNeed;
            QueueDmpp(pdmppTail);
        }

        /* Walk the PTEs and mark the allocation boundary */
        pmmpte = MiGetPteAddress(pdmpp);
        while(--cpgNeed)
            (pmmpte++)->Hard.GuardOrEndOfAllocation = 0;
        pmmpte->Hard.GuardOrEndOfAllocation = 1;

        pv = pdmpp;
    } else if (cpgNeed <= cpgDmppList) {
        /* We couldn't find a sufficiently large block, but we have enough
         * pages to reclaim.  Reclaim as many as necessary and then try
         * our luck */
        while(cpgNeed && pdmppHead) {
            pdmpp = pdmppHead;
            pdmppHead = pdmpp->pdmppNext;
            cpgDmppList -= pdmpp->cpg;

            /* If this is more than we need to free, then put the tail back
             * on the list */
            if(pdmpp->cpg > cpgNeed) {
                pdmppTail = (PDMPP)((ULONG_PTR)pdmpp + (cpgNeed << PAGE_SHIFT));
                pdmppTail->cpg = pdmpp->cpg - cpgNeed;
                QueueDmpp(pdmppTail);
                pdmpp->cpg = cpgNeed;
            }

            MmDbgFreeMemory(pdmpp, pdmpp->cpg << PAGE_SHIFT);
        }

        /* Now see whether our reclaimed pages can be reallocated */
        pv = MmDbgAllocateMemory(cb, PAGE_READWRITE);
    } else
        pv = NULL;

    KeLowerIrql(irqlSav);

    return pv;
}

PVOID
DmpAllocatePoolPages(
    IN SIZE_T NumberOfBytes
    )
{

    PVOID p = DmpAllocatePoolMemory(NumberOfBytes);
    if(p) {
        PBYTE pT = p;
        SIZE_T cb;

        while(NumberOfBytes) {
            cb = NumberOfBytes > PAGE_SIZE ? PAGE_SIZE : NumberOfBytes;
            RtlFillMemoryUlong(pT, cb, 'looP');
            NumberOfBytes -= cb;
            pT += cb;
        }
    }
    return p;
}

VOID
InitPool(
    VOID
    )

/*++

Routine Description:

    This procedure initializes a pool descriptor for the specified pool
    type.  Once initialized, the pool may be used for allocation and
    deallocation.

    This function should be called once for each base pool type during
    system initialization.

    Each pool descriptor contains an array of list heads for free
    blocks.  Each list head holds blocks which are a multiple of
    the POOL_BLOCK_SIZE.  The first element on the list [0] links
    together free entries of size POOL_BLOCK_SIZE, the second element
    [1] links together entries of POOL_BLOCK_SIZE * 2, the third
    POOL_BLOCK_SIZE * 3, etc, up to the number of blocks which fit
    into a page.

Arguments:

    PoolType - Supplies the type of pool being initialized (e.g.
               nonpaged pool, paged pool...).

Return Value:

    None.

--*/

{
    ULONG Index;
    PDMPP pdmpp;
    KIRQL irqlSav;

    //
    // Initialize nonpaged pools.
    //

#if !DBG
    if (NtGlobalFlag & FLG_POOL_ENABLE_TAGGING) {
#endif  //!DBG
        PoolTrackTableSize = MAX_TRACKER_TABLE;
        PoolTrackTableMask = PoolTrackTableSize - 2;
        PoolTrackTable = DmpAllocatePoolMemory(PoolTrackTableSize *
            sizeof(POOL_TRACKER_TABLE));

        RtlZeroMemory(PoolTrackTable, PoolTrackTableSize * sizeof(POOL_TRACKER_TABLE));

        PoolBigPageTableSize = MAX_BIGPAGE_TABLE;
        PoolBigPageTableHash = PoolBigPageTableSize - 1;
        PoolBigPageTable = DmpAllocatePoolMemory(PoolBigPageTableSize *
            sizeof(POOL_TRACKER_BIG_PAGES));

        RtlZeroMemory(PoolBigPageTable, PoolBigPageTableSize * sizeof(POOL_TRACKER_BIG_PAGES));
#if !DBG
    }
#endif  //!DBG

    //
    // Initialize the spinlocks for nonpaged pool.
    //

    KeInitializeSpinLock (&ExpTaggedPoolLock);

    //
    // Initialize the nonpaged pool descriptor.
    //

    DmPoolDescriptor.RunningAllocs = 0;
    DmPoolDescriptor.RunningDeAllocs = 0;
    DmPoolDescriptor.TotalPages = 0;
    DmPoolDescriptor.TotalBigPages = 0;

    //
    // Initialize the allocation listheads.
    //

    for (Index = 0; Index < POOL_LIST_HEADS; Index += 1) {
        PrivateInitializeListHead(&DmPoolDescriptor.ListHeads[Index]);
    }

    if (PoolTrackTable) {
        ExpInsertPoolTracker('looP',
                              (ULONG) ROUND_TO_PAGES(PoolTrackTableSize * sizeof(POOL_TRACKER_TABLE)));

        ExpInsertPoolTracker('looP',
                              (ULONG) ROUND_TO_PAGES(PoolBigPageTableSize * sizeof(POOL_TRACKER_BIG_PAGES)));
    }

    /* Allocate the minimum free list */
    pdmpp = MmDbgAllocateMemory(cpgDmppMax << PAGE_SHIFT, PAGE_READWRITE);
    if(pdmpp) {
        irqlSav = KeRaiseIrqlToDpcLevel();
        pdmpp->cpg = cpgDmppMax;
        QueueDmpp(pdmpp);
        KeLowerIrql(irqlSav);
    }
}

PVOID
DmAllocatePool(
    IN SIZE_T NumberOfBytes
    )

/*++

Routine Description:

    This function allocates a block of pool of the specified type and
    returns a pointer to the allocated block.  This function is used to
    access both the page-aligned pools, and the list head entries (less than
    a page) pools.

    If the number of bytes specifies a size that is too large to be
    satisfied by the appropriate list, then the page-aligned
    pool allocator is used.  The allocated block will be page-aligned
    and a page-sized multiple.

    Otherwise, the appropriate pool list entry is used.  The allocated
    block will be 64-bit aligned, but will not be page aligned.  The
    pool allocator calculates the smallest number of POOL_BLOCK_SIZE
    that can be used to satisfy the request.  If there are no blocks
    available of this size, then a block of the next larger block size
    is allocated and split.  One piece is placed back into the pool, and
    the other piece is used to satisfy the request.  If the allocator
    reaches the paged-sized block list, and nothing is there, the
    page-aligned pool allocator is called.  The page is split and added
    to the pool...

Arguments:

    PoolType - ignored

    NumberOfBytes - Supplies the number of bytes to allocate.

Return Value:

    NULL - The PoolType is not one of the "MustSucceed" pool types, and
        not enough pool exists to satisfy the request.

    NON-NULL - Returns a pointer to the allocated pool.

--*/

{
    return DmAllocatePoolWithTag (NumberOfBytes, 'enoN');
}

PVOID
DmAllocatePoolWithTag(
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    )

/*++

Routine Description:

    This function allocates a block of pool of the specified type and
    returns a pointer to the allocated block. This function is used to
    access both the page-aligned pools and the list head entries (less
    than a page) pools.

    If the number of bytes specifies a size that is too large to be
    satisfied by the appropriate list, then the page-aligned pool
    allocator is used. The allocated block will be page-aligned and a
    page-sized multiple.

    Otherwise, the appropriate pool list entry is used. The allocated
    block will be 64-bit aligned, but will not be page aligned. The
    pool allocator calculates the smallest number of POOL_BLOCK_SIZE
    that can be used to satisfy the request. If there are no blocks
    available of this size, then a block of the next larger block size
    is allocated and split. One piece is placed back into the pool, and
    the other piece is used to satisfy the request. If the allocator
    reaches the paged-sized block list, and nothing is there, the
    page-aligned pool allocator is called. The page is split and added
    to the pool.

Arguments:

    NumberOfBytes - Supplies the number of bytes to allocate.

    Tag - Supplies the caller's identifying tag.

Return Value:

    NULL - The PoolType is not one of the "MustSucceed" pool types, and
        not enough pool exists to satisfy the request.

    NON-NULL - Returns a pointer to the allocated pool.

--*/

{
    PVOID Block;
    PPOOL_HEADER Entry;
    PPOOL_LOOKASIDE_LIST LookasideList;
    PPOOL_HEADER NextEntry;
    PPOOL_HEADER SplitEntry;
    KIRQL LockHandle;
    PPOOL_DESCRIPTOR PoolDesc = &DmPoolDescriptor;
    ULONG Index;
    ULONG ListNumber;
    ULONG NeededSize;
    PLIST_ENTRY ListHead;
    ULONG NumberOfPages;
    PVOID CallingAddress;
    PVOID CallersCaller;

    ASSERT(NumberOfBytes != 0);
    ASSERT_ALLOCATE_IRQL(NumberOfBytes);

    //
    // Check to determine if the requested block can be allocated from one
    // of the pool lists or must be directly allocated from virtual memory.
    //

    if (NumberOfBytes > POOL_BUDDY_MAX) {

        //
        // The requested size is greater than the largest block maintained
        // by allocation lists.
        //

        LOCK_POOL(LockHandle);

        PoolDesc->RunningAllocs += 1;

        Entry = (PPOOL_HEADER) DmpAllocatePoolPages(NumberOfBytes);

        if (Entry != NULL) {

            NumberOfPages = BYTES_TO_PAGES(NumberOfBytes);
            PoolDesc->TotalBigPages += NumberOfPages;

            UNLOCK_POOL(LockHandle);

            if (PoolBigPageTable != NULL) {

                if (ExpAddTagForBigPages((PVOID)Entry,
                                         Tag,
                                         NumberOfPages) == FALSE) {
                    Tag = ' GIB';
                }

                ExpInsertPoolTracker (Tag,
                                      (ULONG) ROUND_TO_PAGES(NumberOfBytes));
            }

        } else {

            UNLOCK_POOL(LockHandle);

            KdPrint(("EX: DmAllocatePool (%p) returning NULL\n", NumberOfBytes));
        }

        return Entry;
    }

    //
    // The requested size is less than or equal to the size of the
    // maximum block maintained by the allocation lists.
    //

    //
    // Compute the Index of the listhead for blocks of the requested
    // size.
    //

    ListNumber = (ULONG)((NumberOfBytes + POOL_OVERHEAD + (POOL_SMALLEST_BLOCK - 1)) >> POOL_BLOCK_SHIFT);

    NeededSize = ListNumber;

    //
    // If the requested pool block is a small block, then attempt to
    // allocate the requested pool from the per processor lookaside
    // list. If the attempt fails, then attempt to allocate from the
    // system lookaside list. If the attempt fails, then select a
    // pool to allocate from and allocate the block normally.
    //

    if (NeededSize <= POOL_SMALL_LISTS) {
        LookasideList = &ExpSmallDmPoolLookasideLists[NeededSize - 1];
        LookasideList->TotalAllocates += 1;

        CHECK_LOOKASIDE_LIST(__LINE__, LookasideList, 0);

        Entry = (PPOOL_HEADER)InterlockedPopEntrySList (&LookasideList->ListHead);

        if (Entry != NULL) {

            CHECK_LOOKASIDE_LIST(__LINE__, LookasideList, Entry);

            Entry -= 1;
            LookasideList->AllocateHits += 1;

            Entry->PoolType = 1;
            MARK_POOL_HEADER_ALLOCATED(Entry);

            Entry->PoolTag = Tag;

            if (PoolTrackTable != NULL) {

                ExpInsertPoolTracker (Tag, Entry->BlockSize << POOL_BLOCK_SHIFT);
            }

            //
            // Zero out any back pointer to our internal structures
            // to stop someone from corrupting us via an
            // uninitialized pointer.
            //

            ((PULONG)((PCHAR)Entry + POOL_OVERHEAD))[0] = 0;

            return (PUCHAR)Entry + POOL_OVERHEAD;
        }
    }

    LOCK_POOL(LockHandle);

    //
    // The following code has an outer loop and an inner loop.
    //
    // The outer loop is utilized to repeat a nonpaged must succeed
    // allocation if necessary.
    //
    // The inner loop is used to repeat an allocation attempt if there
    // are no entries in any of the pool lists.
    //

    PoolDesc->RunningAllocs += 1;
    ListHead = &PoolDesc->ListHeads[ListNumber];

    do {

        //
        // Attempt to allocate the requested block from the current free
        // blocks.
        //

        do {

            //
            // If the list is not empty, then allocate a block from the
            // selected list.
            //

            if (PrivateIsListEmpty(ListHead) == FALSE) {

                CHECK_LIST( __LINE__, ListHead, 0 );
                Block = PrivateRemoveHeadList(ListHead);
                CHECK_LIST( __LINE__, ListHead, 0 );
                Entry = (PPOOL_HEADER)((PCHAR)Block - POOL_OVERHEAD);

                ASSERT(Entry->BlockSize >= NeededSize);

                ASSERT(Entry->PoolType == 0);

                if (Entry->BlockSize != NeededSize) {

                    //
                    // The selected block is larger than the allocation
                    // request. Split the block and insert the remaining
                    // fragment in the appropriate list.
                    //
                    // If the entry is at the start of a page, then take
                    // the allocation from the front of the block so as
                    // to minimize fragmentation. Otherwise, take the
                    // allocation from the end of the block which may
                    // also reduce fragmentation if the block is at the
                    // end of a page.
                    //

                    if (Entry->PreviousSize == 0) {

                        //
                        // The entry is at the start of a page.
                        //

                        SplitEntry = (PPOOL_HEADER)((PPOOL_BLOCK)Entry + NeededSize);
                        SplitEntry->BlockSize = (UCHAR)(Entry->BlockSize - (UCHAR)NeededSize);
                        SplitEntry->PreviousSize = (UCHAR)NeededSize;

                        //
                        // If the allocated block is not at the end of a
                        // page, then adjust the size of the next block.
                        //

                        NextEntry = (PPOOL_HEADER)((PPOOL_BLOCK)SplitEntry + SplitEntry->BlockSize);
                        if (PAGE_END(NextEntry) == FALSE) {
                            NextEntry->PreviousSize = SplitEntry->BlockSize;
                        }

                    } else {

                        //
                        // The entry is not at the start of a page.
                        //

                        SplitEntry = Entry;
                        Entry->BlockSize -= (UCHAR)NeededSize;
                        Entry = (PPOOL_HEADER)((PPOOL_BLOCK)Entry + Entry->BlockSize);
                        Entry->PreviousSize = SplitEntry->BlockSize;

                        //
                        // If the allocated block is not at the end of a
                        // page, then adjust the size of the next block.
                        //

                        NextEntry = (PPOOL_HEADER)((PPOOL_BLOCK)Entry + NeededSize);
                        if (PAGE_END(NextEntry) == FALSE) {
                            NextEntry->PreviousSize = (UCHAR)NeededSize;
                        }
                    }

                    //
                    // Set the size of the allocated entry, clear the pool
                    // type of the split entry, set the index of the split
                    // entry, and insert the split entry in the appropriate
                    // free list.
                    //

                    Entry->BlockSize = (UCHAR)NeededSize;
                    SplitEntry->PoolType = 0;
                    Index = SplitEntry->BlockSize;

                    CHECK_LIST(__LINE__, &PoolDesc->ListHeads[Index - 1], 0);
                    PrivateInsertTailList(&PoolDesc->ListHeads[Index - 1], ((PLIST_ENTRY)((PCHAR)SplitEntry + POOL_OVERHEAD)));
                    CHECK_LIST(__LINE__, &PoolDesc->ListHeads[Index - 1], 0);
                    CHECK_LIST(__LINE__, ((PLIST_ENTRY)((PCHAR)SplitEntry + POOL_OVERHEAD)), 0);
                }

                Entry->PoolType = 1;

                MARK_POOL_HEADER_ALLOCATED(Entry);

                CHECK_POOL_HEADER(__LINE__, Entry);

                UNLOCK_POOL(LockHandle);

                Entry->PoolTag = Tag;

                if (PoolTrackTable != NULL) {

                    ExpInsertPoolTracker (Tag,
                                          Entry->BlockSize << POOL_BLOCK_SHIFT);
                }

                //
                // Zero out any back pointer to our internal structures
                // to stop someone from corrupting us via an
                // uninitialized pointer.
                //

                ((PULONGLONG)((PCHAR)Entry + POOL_OVERHEAD))[0] = 0;

                return (PCHAR)Entry + POOL_OVERHEAD;
            }
            ListHead += 1;

        } while (ListHead != &PoolDesc->ListHeads[POOL_LIST_HEADS]);

        //
        // A block of the desired size does not exist and there are
        // no large blocks that can be split to satisfy the allocation.
        // Attempt to expand the pool by allocating another page to be
        // added to the pool.
        //

        Entry = (PPOOL_HEADER)DmpAllocatePoolPages(PAGE_SIZE);

        if (Entry == NULL) {

            //
            // No more pool of the specified type is available.
            //

            KdPrint(("EX: DmAllocatePool (%p) returning NULL\n",
                NumberOfBytes));

            UNLOCK_POOL(LockHandle);

            return NULL;
        }

        //
        // Insert the allocated page in the last allocation list.
        //

        PoolDesc->TotalPages += 1;
        Entry->PoolType = 0;

        //
        // N.B. A byte is used to store the block size in units of the
        //      smallest block size. Therefore, if the number of small
        //      blocks in the page is greater than 255, the block size
        //      is set to 255.
        //

        if ((PAGE_SIZE / POOL_SMALLEST_BLOCK) > 255) {
            Entry->BlockSize = 255;

        } else {
            Entry->BlockSize = (UCHAR)(PAGE_SIZE / POOL_SMALLEST_BLOCK);
        }

        Entry->PreviousSize = 0;
        ListHead = &PoolDesc->ListHeads[POOL_LIST_HEADS - 1];

        CHECK_LIST(__LINE__, ListHead, 0);
        PrivateInsertHeadList(ListHead, ((PLIST_ENTRY)((PCHAR)Entry + POOL_OVERHEAD)));
        CHECK_LIST(__LINE__, ListHead, 0);
        CHECK_LIST(__LINE__, ((PLIST_ENTRY)((PCHAR)Entry + POOL_OVERHEAD)), 0);

    } while (TRUE);
}

VOID
ExpInsertPoolTracker (
    IN ULONG Key,
    IN SIZE_T Size
    )

/*++

Routine Description:

    This function inserts a pool tag in the tag table and increments the
    number of allocates and updates the total allocation size.

Arguments:

    Key - Supplies the key value used to locate a matching entry in the
          tag table.

    Size - Supplies the allocation size.

    PoolType - Supplies the pool type.

Return Value:

    None.

Environment:

    No pool locks held so pool may be freely allocated here as needed.

--*/

{
    ULONG Hash;
    ULONG OriginalKey;
    ULONG OriginalHash;
    ULONG Index;
    KIRQL OldIrql;
    ULONG BigPages;
    LOGICAL HashedIt;
    SIZE_T NewSize;
    SIZE_T SizeInBytes;
    SIZE_T NewSizeInBytes;
    SIZE_T NewSizeMask;
    PPOOL_TRACKER_TABLE OldTable;
    PPOOL_TRACKER_TABLE NewTable;

retry:

    //
    // Compute hash index and search for pool tag.
    //

    ExAcquireSpinLock(&ExpTaggedPoolLock, &OldIrql);

    Hash = ((40543*((((((((PUCHAR)&Key)[0]<<2)^((PUCHAR)&Key)[1])<<2)^((PUCHAR)&Key)[2])<<2)^((PUCHAR)&Key)[3]))>>2) & (ULONG)PoolTrackTableMask;
    Index = Hash;

    do {
        if (PoolTrackTable[Hash].Key == Key) {
            PoolTrackTable[Hash].Key = Key;
            goto EntryFound;
        }

        if (PoolTrackTable[Hash].Key == 0 && Hash != PoolTrackTableSize - 1) {
            PoolTrackTable[Hash].Key = Key;
            goto EntryFound;
        }

        Hash = (Hash + 1) & (ULONG)PoolTrackTableMask;
    } while (Hash != Index);

    //
    // No matching entry and no free entry was found.
    // If the overflow bucket has been used then expansion of the tracker table
    // is not allowed because a subsequent free of a tag can go negative as the
    // original allocation is in overflow and a newer allocation may be
    // distinct.
    //

    NewSize = ((PoolTrackTableSize - 1) << 1) + 1;
    NewSizeInBytes = NewSize * sizeof(POOL_TRACKER_TABLE);

    SizeInBytes = PoolTrackTableSize * sizeof(POOL_TRACKER_TABLE);

    if ((NewSizeInBytes > SizeInBytes) &&
        (PoolTrackTable[PoolTrackTableSize - 1].Key == 0)) {

        NewTable = DmpAllocatePoolMemory (NewSizeInBytes);

        if (NewTable != NULL) {

            OldTable = (PVOID)PoolTrackTable;

            KdPrint(("DMPOOL:grew track table (%p, %p, %p)\n",
                OldTable,
                PoolTrackTableSize,
                NewTable));

            RtlZeroMemory ((PVOID)NewTable, NewSizeInBytes);

            //
            // Rehash all the entries into the new table.
            //

            NewSizeMask = NewSize - 2;

            for (OriginalHash = 0; OriginalHash < PoolTrackTableSize; OriginalHash += 1) {
                OriginalKey = PoolTrackTable[OriginalHash].Key;

                if (OriginalKey == 0) {
                    continue;
                }

                Hash = (ULONG)((40543*((((((((PUCHAR)&OriginalKey)[0]<<2)^((PUCHAR)&OriginalKey)[1])<<2)^((PUCHAR)&OriginalKey)[2])<<2)^((PUCHAR)&OriginalKey)[3]))>>2) & (ULONG)NewSizeMask;
                Index = Hash;

                HashedIt = FALSE;
                do {
                    if (NewTable[Hash].Key == 0 && Hash != NewSize - 1) {
                        RtlCopyMemory ((PVOID)&NewTable[Hash],
                                       (PVOID)&PoolTrackTable[OriginalHash],
                                       sizeof(POOL_TRACKER_TABLE));
                        HashedIt = TRUE;
                        break;
                    }

                    Hash = (Hash + 1) & (ULONG)NewSizeMask;
                } while (Hash != Index);

                //
                // No matching entry and no free entry was found, have to bail.
                //

                if (HashedIt == FALSE) {
                    KdPrint(("DMPOOL:rehash of track table failed (%p, %p, %p %p)\n",
                        OldTable,
                        PoolTrackTableSize,
                        NewTable,
                        OriginalKey));

                    DmpFreePoolMemory (NewTable, 0);
                    goto overflow;
                }
            }

            PoolTrackTable = NewTable;
            PoolTrackTableSize = NewSize;
            PoolTrackTableMask = NewSizeMask;

            ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);

            BigPages = DmpFreePoolMemory (OldTable, 0);

            ExpRemovePoolTracker ('looP', BigPages * PAGE_SIZE);

            ExpInsertPoolTracker ('looP', (ULONG) ROUND_TO_PAGES(NewSizeInBytes));

            goto retry;
        }
    }

overflow:

    //
    // Use the very last entry as a bit bucket for overflows.
    //

    Hash = (ULONG)PoolTrackTableSize - 1;

    PoolTrackTable[Hash].Key = 'lfvO';

    //
    // Update pool tracker table entry.
    //

EntryFound:

    PoolTrackTable[Hash].NonPagedAllocs += 1;
    PoolTrackTable[Hash].NonPagedBytes += Size;

    ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);

    return;
}

VOID
ExpRemovePoolTracker (
    IN ULONG Key,
    IN ULONG Size
    )

/*++

Routine Description:

    This function increments the number of frees and updates the total
    allocation size.

Arguments:

    Key - Supplies the key value used to locate a matching entry in the
          tag table.

    Size - Supplies the allocation size.

Return Value:

    None.

--*/

{
    ULONG Hash;
    ULONG Index;
    KIRQL OldIrql;

    //
    // Compute hash index and search for pool tag.
    //

    ExAcquireSpinLock(&ExpTaggedPoolLock, &OldIrql);

    Hash = ((40543*((((((((PUCHAR)&Key)[0]<<2)^((PUCHAR)&Key)[1])<<2)^((PUCHAR)&Key)[2])<<2)^((PUCHAR)&Key)[3]))>>2) & (ULONG)PoolTrackTableMask;
    Index = Hash;

    do {
        if (PoolTrackTable[Hash].Key == Key) {
            goto EntryFound;
        }

        if (PoolTrackTable[Hash].Key == 0 && Hash != PoolTrackTableSize - 1) {
            KdPrint(("DMPOOL: Unable to find tracker %lx, table corrupted\n", Key));
            ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);
            return;
        }

        Hash = (Hash + 1) & (ULONG)PoolTrackTableMask;
    } while (Hash != Index);

    //
    // No matching entry and no free entry was found.
    //

    Hash = (ULONG)PoolTrackTableSize - 1;

    //
    // Update pool tracker table entry.
    //

EntryFound:

    PoolTrackTable[Hash].NonPagedBytes -= Size;
    PoolTrackTable[Hash].NonPagedFrees += 1;

    ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);

    return;
}

LOGICAL
ExpAddTagForBigPages (
    IN PVOID Va,
    IN ULONG Key,
    IN ULONG NumberOfPages
    )
/*++

Routine Description:

    This function inserts a pool tag in the big page tag table.

Arguments:

    Va - Supplies the allocated virtual address.

    Key - Supplies the key value used to locate a matching entry in the
        tag table.

    NumberOfPages - Supplies the number of pages that were allocated.

Return Value:

    TRUE if an entry was allocated, FALSE if not.

Environment:

    No pool locks held so the table may be freely expanded here as needed.

--*/
{
    ULONG Hash;
    ULONG BigPages;
    PVOID OldTable;
    LOGICAL Inserted;
    KIRQL OldIrql;
    SIZE_T SizeInBytes;
    SIZE_T NewSizeInBytes;
    PPOOL_TRACKER_BIG_PAGES NewTable;
    PPOOL_TRACKER_BIG_PAGES p;

retry:

    Inserted = TRUE;
    Hash = (ULONG)(((ULONG_PTR)Va >> PAGE_SHIFT) & PoolBigPageTableHash);
    ExAcquireSpinLock(&ExpTaggedPoolLock, &OldIrql);
    while ((LONG_PTR)PoolBigPageTable[Hash].Va < 0) {
        Hash += 1;
        if (Hash >= PoolBigPageTableSize) {
            if (!Inserted) {

                //
                // Try to expand the tracker table.
                //

                SizeInBytes = PoolBigPageTableSize * sizeof(POOL_TRACKER_BIG_PAGES);
                NewSizeInBytes = (SizeInBytes << 1);

                if (NewSizeInBytes > SizeInBytes) {
                    NewTable = DmpAllocatePoolMemory (NewSizeInBytes);

                    if (NewTable != NULL) {
    
                        OldTable = (PVOID)PoolBigPageTable;

                        KdPrint(("DMPOOL:grew big table (%p, %p, %p)\n",
                            OldTable,
                            PoolBigPageTableSize,
                            NewTable));

                        RtlCopyMemory ((PVOID)NewTable,
                                       OldTable,
                                       SizeInBytes);

                        RtlZeroMemory ((PVOID)(NewTable + PoolBigPageTableSize),
                                       NewSizeInBytes - SizeInBytes);

                        PoolBigPageTable = NewTable;
                        PoolBigPageTableSize <<= 1;
                        PoolBigPageTableHash = PoolBigPageTableSize - 1;

                        ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);

                        BigPages = DmpFreePoolMemory (OldTable, 0);

                        ExpRemovePoolTracker ('looP',
                                              BigPages * PAGE_SIZE);

                        ExpInsertPoolTracker ('looP',
                                              (ULONG) ROUND_TO_PAGES(NewSizeInBytes));

                        goto retry;
                    }
                }

                if (!FirstPrint) {
                    KdPrint(("DMPOOL:unable to insert big page slot %lx\n",Key));
                    FirstPrint = TRUE;
                }

                ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);
                return FALSE;
            }

            Hash = 0;
            Inserted = FALSE;
        }
    }

    p = &PoolBigPageTable[Hash];

    ASSERT ((LONG_PTR)Va < 0);

    p->Va = Va;
    p->Key = Key;
    p->NumberOfPages = NumberOfPages;

    ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);

    return TRUE;
}

ULONG
ExpFindAndRemoveTagBigPages (
    IN PVOID Va
    )
{
    ULONG Hash;
    LOGICAL Inserted;
    KIRQL OldIrql;
    ULONG ReturnKey;

    Inserted = TRUE;
    Hash = (ULONG)(((ULONG_PTR)Va >> PAGE_SHIFT) & PoolBigPageTableHash);
    ExAcquireSpinLock(&ExpTaggedPoolLock, &OldIrql);
    while (PoolBigPageTable[Hash].Va != Va) {
        Hash += 1;
        if (Hash >= PoolBigPageTableSize) {
            if (!Inserted) {
                if (!FirstPrint) {
                    KdPrint(("DMPOOL:unable to find big page slot %lx\n",Va));
                    FirstPrint = TRUE;
                }

                ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);
                return ' GIB';
            }

            Hash = 0;
            Inserted = FALSE;
        }
    }

    ASSERT ((LONG_PTR)Va < 0);
    (ULONG_PTR)PoolBigPageTable[Hash].Va &= MAXLONG_PTR;

    ReturnKey = PoolBigPageTable[Hash].Key;
    ExReleaseSpinLock(&ExpTaggedPoolLock, OldIrql);
    return ReturnKey;
}

VOID
DmFreePool(
    IN PVOID P
    )
/*++

Routine Description:

    This function deallocates a block of pool. This function is used to
    deallocate to both the page aligned pools and the buddy (less than
    a page) pools.

    If the address of the block being deallocated is page-aligned, then
    the page-aligned pool deallocator is used.

    Otherwise, the binary buddy pool deallocator is used.  Deallocation
    looks at the allocated block's pool header to determine the pool
    type and block size being deallocated.  If the pool was allocated
    using ExAllocatePoolWithQuota, then after the deallocation is
    complete, the appropriate process's pool quota is adjusted to reflect
    the deallocation, and the process object is dereferenced.

Arguments:

    P - Supplies the address of the block of pool being deallocated.

Return Value:

    None.

--*/
{
    PPOOL_HEADER Entry;
    ULONG Index;
    KIRQL LockHandle;
    PPOOL_LOOKASIDE_LIST LookasideList;
    PPOOL_HEADER NextEntry;
    PPOOL_DESCRIPTOR PoolDesc = &DmPoolDescriptor;
    LOGICAL Combined;
    ULONG BigPages;
    ULONG Tag;

    if(!FIsDmPool(P)) {
        DbgPrint("DmFreePool called for %08x\n", P);
        DmExFreePool(P);
        return;
    }

    //
    // If entry is page aligned, then call free block to the page aligned
    // pool. Otherwise, free the block to the allocation lists.
    //

    if (PAGE_ALIGNED(P)) {

        ASSERT_FREE_IRQL(P);

        if (PoolTrackTable != NULL) {
            Tag = ExpFindAndRemoveTagBigPages(P);
        }

        LOCK_POOL(LockHandle);

        PoolDesc->RunningDeAllocs += 1;

        BigPages = DmpFreePoolMemory(P, 0);

        if (PoolTrackTable != NULL) {
            ExpRemovePoolTracker(Tag, BigPages * PAGE_SIZE);
        }

        //
        // Check if a KTIMER is currently active in this memory block
        //

        FREE_CHECK_KTIMER(P, BigPages << PAGE_SHIFT);

        PoolDesc->TotalBigPages -= BigPages;

        UNLOCK_POOL(LockHandle);

        return;
    }

    //
    // Align the entry address to a pool allocation boundary.
    //

    Entry = (PPOOL_HEADER)((PCHAR)P - POOL_OVERHEAD);

    ASSERT_POOL_NOT_FREE(Entry);

    ASSERT_FREE_IRQL(P);

    if (!IS_POOL_HEADER_MARKED_ALLOCATED(Entry)) {
        KeBugCheckEx (BAD_POOL_CALLER, 7, __LINE__, (ULONG_PTR)Entry, (ULONG_PTR)P);
    }

    MARK_POOL_HEADER_FREED(Entry);

    ASSERT_POOL_TYPE_NOT_ZERO(Entry);

    //
    // Check if a KTIMER is currently active in this memory block.
    //

    FREE_CHECK_KTIMER(Entry, (ULONG)(Entry->BlockSize << POOL_BLOCK_SHIFT));

    //
    // If pool tagging is enabled, then update the pool tracking database.
    // Otherwise, check to determine if quota was charged when the pool
    // block was allocated.
    //

    if (PoolTrackTable != NULL) {
        Tag = Entry->PoolTag;
        ExpRemovePoolTracker(Tag, Entry->BlockSize << POOL_BLOCK_SHIFT);
    }

    //
    // If the pool block is a small block, then attempt to free the block
    // to the single entry lookaside list. If the free attempt fails, then
    // free the block by merging it back into the pool data structures.
    //

    Index = Entry->BlockSize;

    if (Index <= POOL_SMALL_LISTS) {

        //
        // Attempt to free the small block to a per processor lookaside
        // list.
        //

        LookasideList = &ExpSmallDmPoolLookasideLists[Index - 1];

        CHECK_LOOKASIDE_LIST(__LINE__, LookasideList, P);

        if (ExQueryDepthSList(&LookasideList->ListHead) < LookasideList->Depth) {
            Entry += 1;
            InterlockedPushEntrySList(&LookasideList->ListHead,
                                      (PSINGLE_LIST_ENTRY)Entry);

            CHECK_LOOKASIDE_LIST(__LINE__, LookasideList, P);

            return;
        }
    }

    LOCK_POOL(LockHandle);

    CHECK_POOL_HEADER(__LINE__, Entry);

    PoolDesc->RunningDeAllocs += 1;

    //
    // Free the specified pool block.
    //
    // Check to see if the next entry is free.
    //

    Combined = FALSE;
    NextEntry = (PPOOL_HEADER)((PPOOL_BLOCK)Entry + Entry->BlockSize);
    if (PAGE_END(NextEntry) == FALSE) {

        if (NextEntry->PoolType == 0) {

            //
            // This block is free, combine with the released block.
            //

            Combined = TRUE;

            CHECK_LIST(__LINE__, ((PLIST_ENTRY)((PCHAR)NextEntry + POOL_OVERHEAD)), P);
            PrivateRemoveEntryList(((PLIST_ENTRY)((PCHAR)NextEntry + POOL_OVERHEAD)));
            CHECK_LIST(__LINE__, DecodeLink(((PLIST_ENTRY)((PCHAR)NextEntry + POOL_OVERHEAD))->Flink), P);
            CHECK_LIST(__LINE__, DecodeLink(((PLIST_ENTRY)((PCHAR)NextEntry + POOL_OVERHEAD))->Blink), P);

            Entry->BlockSize += NextEntry->BlockSize;
        }
    }

    //
    // Check to see if the previous entry is free.
    //

    if (Entry->PreviousSize != 0) {
        NextEntry = (PPOOL_HEADER)((PPOOL_BLOCK)Entry - Entry->PreviousSize);
        if (NextEntry->PoolType == 0) {

            //
            // This block is free, combine with the released block.
            //

            Combined = TRUE;

            CHECK_LIST(__LINE__, ((PLIST_ENTRY)((PCHAR)NextEntry + POOL_OVERHEAD)), P);
            PrivateRemoveEntryList(((PLIST_ENTRY)((PCHAR)NextEntry + POOL_OVERHEAD)));
            CHECK_LIST(__LINE__, DecodeLink(((PLIST_ENTRY)((PCHAR)NextEntry + POOL_OVERHEAD))->Flink), P);
            CHECK_LIST(__LINE__, DecodeLink(((PLIST_ENTRY)((PCHAR)NextEntry + POOL_OVERHEAD))->Blink), P);

            NextEntry->BlockSize += Entry->BlockSize;
            Entry = NextEntry;
        }
    }

    //
    // If the block being freed has been combined into a full page,
    // then return the free page to memory management.
    //

    if (PAGE_ALIGNED(Entry) &&
        (PAGE_END((PPOOL_BLOCK)Entry + Entry->BlockSize) != FALSE)) {

        DmpFreePoolMemory(Entry, 0);

        PoolDesc->TotalPages -= 1;

    } else {

        //
        // Insert this element into the list.
        //

        Entry->PoolType = 0;
        Index = Entry->BlockSize;

        //
        // If the freed block was combined with any other block, then
        // adjust the size of the next block if necessary.
        //

        if (Combined != FALSE) {

            //
            // The size of this entry has changed, if this entry is
            // not the last one in the page, update the pool block
            // after this block to have a new previous allocation size.
            //

            NextEntry = (PPOOL_HEADER)((PPOOL_BLOCK)Entry + Entry->BlockSize);
            if (PAGE_END(NextEntry) == FALSE) {
                NextEntry->PreviousSize = Entry->BlockSize;
            }

            //
            // Reduce fragmentation and insert at the tail in hopes
            // neighbors for this will be freed before this is reallocated.
            //

            CHECK_LIST(__LINE__, &PoolDesc->ListHeads[Index - 1], P);
            PrivateInsertTailList(&PoolDesc->ListHeads[Index - 1], ((PLIST_ENTRY)((PCHAR)Entry + POOL_OVERHEAD)));
            CHECK_LIST(__LINE__, &PoolDesc->ListHeads[Index - 1], P);
            CHECK_LIST(__LINE__, ((PLIST_ENTRY)((PCHAR)Entry + POOL_OVERHEAD)), P);

        } else {

            CHECK_LIST(__LINE__, &PoolDesc->ListHeads[Index - 1], P);
            PrivateInsertHeadList(&PoolDesc->ListHeads[Index - 1], ((PLIST_ENTRY)((PCHAR)Entry + POOL_OVERHEAD)));
            CHECK_LIST(__LINE__, &PoolDesc->ListHeads[Index - 1], P);
            CHECK_LIST(__LINE__, ((PLIST_ENTRY)((PCHAR)Entry + POOL_OVERHEAD)), P);
        }
    }

    UNLOCK_POOL(LockHandle);
}

ULONG
DmQueryPoolBlockSize (
    IN PVOID PoolBlock
    )

/*++

Routine Description:

    This function returns the size of the pool block.

Arguments:

    PoolBlock - Supplies the address of the block of pool.

Return Value:

    Size of pool block.

--*/

{
    PPOOL_HEADER Entry;
    ULONG size;

    if (PAGE_ALIGNED(PoolBlock)) {
        return MmQueryAllocationSize(PoolBlock);
    }

    //
    // Align entry on pool allocation boundary.
    //

    Entry = (PPOOL_HEADER)((PCHAR)PoolBlock - POOL_OVERHEAD);
    size = (ULONG)((Entry->BlockSize << POOL_BLOCK_SHIFT) - POOL_OVERHEAD);

    return size;
}
