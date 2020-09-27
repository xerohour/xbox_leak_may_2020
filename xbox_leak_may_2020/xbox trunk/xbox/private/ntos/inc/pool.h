/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1989-1995  Microsoft Corporation

Module Name:

    pool.h

Abstract:

    Private executive data structures and procedure prototypes for pool
    allocation.


    There are three pool types:
        1. nonpaged,
        2. paged, and
        3. nonpagedmustsucceed.

    There is only one of each the nonpaged and nonpagedmustsucceed pools.

    There can be more than one paged pool.

Author:

    Lou Perazzoli (loup) 23-Feb-1989

Revision History:

--*/

#ifndef _POOL_
#define _POOL_

#if !DBG
#define NO_POOL_CHECKS
#endif

#if !DBG
#define NO_POOL_TAG_TRACKING
#endif

#define POOL_TYPE_MASK (3)

//
// The smallest pool block size must be a multiple of the page size.
//
// Define the block size as 32.
//

#define POOL_BLOCK_SHIFT 5

#define POOL_LIST_HEADS (PAGE_SIZE / (1 << POOL_BLOCK_SHIFT))

#define PAGE_ALIGNED(p) (!(((ULONG_PTR)p) & (PAGE_SIZE - 1)))

//
// Define page end macro.
//

#if defined(_ALPHA_) || defined(_IA64_)
#define PAGE_END(Address) (((ULONG_PTR)(Address) & (PAGE_SIZE - 1)) == (PAGE_SIZE - (1 << POOL_BLOCK_SHIFT)))
#else
#define PAGE_END(Address) (((ULONG_PTR)(Address) & (PAGE_SIZE - 1)) == 0)
#endif

//
// Define pool descriptor structure.
//

typedef struct _POOL_DESCRIPTOR {
    ULONG RunningAllocs;
    ULONG RunningDeAllocs;
    ULONG TotalPages;
    ULONG TotalBigPages;
    LIST_ENTRY ListHeads[POOL_LIST_HEADS];
} POOL_DESCRIPTOR, *PPOOL_DESCRIPTOR;

//
//      Caveat Programmer:
//
//              The pool header must be QWORD (8 byte) aligned in size.  If it
//              is not, the pool allocation code will trash the allocated
//              buffer
//
// The layout of the pool header is:
//
//         31              23         16 15             7            0
//         +----------------------------------------------------------+
//         | Current Size |  PoolType+1 |  Pool Index  |Previous Size |
//         +----------------------------------------------------------+
//         | PoolTag                                                  |
//         +----------------------------------------------------------+
//         | Zero or more longwords of pad such that the pool header  |
//         | is on a cache line boundary and the pool body is also    |
//         | on a cache line boundary.                                |
//         +----------------------------------------------------------+
//
//      PoolBody:
//
//         +----------------------------------------------------------+
//         | Used by allocator, or when free FLINK into sized list    |
//         +----------------------------------------------------------+
//         | Used by allocator, or when free BLINK into sized list    |
//         +----------------------------------------------------------+
//         ... rest of pool block...
//
//
// N.B. The size fields of the pool header are expressed in units of the
//      smallest pool block size.
//

typedef struct _POOL_HEADER {
    union {
        struct {
            UCHAR PreviousSize;
            UCHAR PoolIndex;
            UCHAR PoolType;
            UCHAR BlockSize;
        };
        ULONG Ulong1;                       // used for InterlockedCompareExchange required by Alpha
    };
    ULONG PoolTag;
} POOL_HEADER, *PPOOL_HEADER;

//
// Define size of pool block overhead.
//

#define POOL_OVERHEAD ((LONG)sizeof(POOL_HEADER))

//
// Define size of pool block overhead when the block is on a freelist.
//

#define POOL_FREE_BLOCK_OVERHEAD  (POOL_OVERHEAD + sizeof (LIST_ENTRY))

//
// Define dummy type so computation of pointers is simplified.
//

typedef struct _POOL_BLOCK {
    UCHAR Fill[1 << POOL_BLOCK_SHIFT];
} POOL_BLOCK, *PPOOL_BLOCK;

//
// Define size of smallest pool block.
//

#define POOL_SMALLEST_BLOCK (sizeof(POOL_BLOCK))

#define POOL_BUDDY_MAX  \
   (PAGE_SIZE - (POOL_OVERHEAD + POOL_SMALLEST_BLOCK ))

//++
//SIZE_T
//EX_REAL_POOL_USAGE (
//    IN SIZE_T SizeInBytes
//    );
//
// Routine Description:
//
//    This routine determines the real pool cost of the supplied allocation.
//
// Arguments
//
//    SizeInBytes - Supplies the allocation size in bytes.
//
// Return Value:
//
//    TRUE if unused segment trimming should be initiated, FALSE if not.
//
//--

#define EX_REAL_POOL_USAGE(SizeInBytes)                             \
        (((SizeInBytes) > POOL_BUDDY_MAX) ?                         \
            (ROUND_TO_PAGES(SizeInBytes)) :                         \
            (((SizeInBytes) + POOL_OVERHEAD + (POOL_SMALLEST_BLOCK - 1)) & ~(POOL_SMALLEST_BLOCK - 1)))

typedef struct _POOL_TRACKER_TABLE {
    ULONG Key;
    ULONG NonPagedAllocs;
    ULONG NonPagedFrees;
    SIZE_T NonPagedBytes;
} POOL_TRACKER_TABLE, *PPOOL_TRACKER_TABLE;

//
// N.B. The last entry of the pool tracker table is used for all overflow
//      table entries.
//

extern PPOOL_TRACKER_TABLE PoolTrackTable;

typedef struct _POOL_TRACKER_BIG_PAGES {
    PVOID Va;
    ULONG Key;
    ULONG NumberOfPages;
} POOL_TRACKER_BIG_PAGES, *PPOOL_TRACKER_BIG_PAGES;

//
// Pool specific lookaside list.
//

typedef struct _POOL_LOOKASIDE_LIST {
    SLIST_HEADER ListHead;
    USHORT Depth;
    USHORT Padding;
    ULONG TotalAllocates;
    ULONG AllocateHits;
} POOL_LOOKASIDE_LIST, *PPOOL_LOOKASIDE_LIST;

#endif
