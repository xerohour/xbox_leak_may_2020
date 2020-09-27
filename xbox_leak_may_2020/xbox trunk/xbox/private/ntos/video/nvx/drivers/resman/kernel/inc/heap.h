#ifndef _HEAP_H_
#define _HEAP_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995-2000. All rights reserved.
//
/*********************** Heap manager data structures **********************\
*                                                                           *
* Module: HEAP.H                                                            *
*   Heap manager for allocating memory based on usage and                   *
*   memory configuration.                                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    05/19/98 - wrote it.                    *
*                                                                           *
\***************************************************************************/

typedef struct
{
    U032 offset;
    U032 size;
    U032 flags;
} MEMBANK, *PMEMBANK;

typedef struct def_block
{
    U032 owner;
    U032 mhandle;
    U032 begin;
    U032 align;
    U032 end;
    union
    {
        U032              type;
        struct def_block *prevFree;
    } u0;
    union
    {
        U032              hwres;
        struct def_block *nextFree;
    } u1;
    struct def_block *prev;
    struct def_block *next;
} MEMBLOCK, *PMEMBLOCK;

typedef struct
{
    U008     *base;
    U032      total;
    U032      free;
    MEMBLOCK *pBlockList;
    MEMBLOCK *pFreeBlockList;
    U032      numBanks;
    U032      memHandle;
    U032      memTypeFlags[11];
    MEMBANK   Bank[1];
} HEAP, *PHEAP;
//
// Handy heap macros.
//
//#define BLOCK_OFFSET(bp)        ((bp)->offset+(bp)->align)
//#define BLOCK_ADDRESS(hp,bp)    ((hp)->base+(bp)->offset+(bp)->align)
//
// Pre-defined no-owner ID - i.e. free.
//
#define FREE_BLOCK              0xFFFFFFFF
//
// Types of memory to allocate.
//
#ifndef BIT
#define BIT(b)                  (1<<(b))
#endif
#define MEM_GROW_UP             0x00000000
#define MEM_GROW_DOWN           0x80000000
#define MEM_GROW_MASK           0x80000000
#define MEM_TYPE_IMAGE          0
#define MEM_TYPE_DEPTH          1
#define MEM_TYPE_TEXTURE        2
#define MEM_TYPE_OVERLAY        3
#define MEM_TYPE_FONT           4
#define MEM_TYPE_CURSOR         5
#define MEM_TYPE_DMA            6
#define MEM_TYPE_INSTANCE       7
#define MEM_TYPE_PRIMARY        8
#define MEM_TYPE_IMAGE_TILED    9
#define MEM_TYPE_DEPTH_COMPR16  10
#define MEM_TYPE_DEPTH_COMPR32  11
#define MEM_TYPE_MAX            11
#define MEM_TYPE_MASK                       0x0000FFFF
#define MEM_TYPE_IGNORE_BANK_PLACEMENT_FLAG 0x00010000
#define MEM_TYPE_FORCE_MEM_GROWS_UP_FLAG    0x00020000
#define MEM_TYPE_FORCE_MEM_GROWS_DOWN_FLAG  0x00040000
#define MEM_FLAG_IMAGE          BIT(MEM_TYPE_IMAGE)
#define MEM_FLAG_DEPTH          BIT(MEM_TYPE_DEPTH)
#define MEM_FLAG_TEXTURE        BIT(MEM_TYPE_TEXTURE)
#define MEM_FLAG_OVERLAY        BIT(MEM_TYPE_OVERLAY)
#define MEM_FLAG_FONT           BIT(MEM_TYPE_FONT)
#define MEM_FLAG_CURSOR         BIT(MEM_TYPE_CURSOR)
#define MEM_FLAG_DMA            BIT(MEM_TYPE_DMA)
#define MEM_FLAG_INSTANCE       BIT(MEM_TYPE_INSTANCE)
#define MEM_FLAG_PRIMARY        BIT(MEM_TYPE_PRIMARY)
#define MEM_FLAG_IMAGE_TILED    BIT(MEM_TYPE_IMAGE_TILED)
#define MEM_FLAG_DEPTH_COMPR16  BIT(MEM_TYPE_DEPTH_COMPR16)
#define MEM_FLAG_DEPTH_COMPR32  BIT(MEM_TYPE_DEPTH_COMPR32)
#define MEM_TYPE_HEAD_MASK      0x80000000
#define MEM_TYPE_HEAD_SHIFT     31

// bits kept in MEMBLOCK->type indicating if/which HW tile range was allocated
#define MEM_TYPE_TILE_RANGE_SHIFT(i) (1 << (24+(i)))
#define MEM_TYPE_TILE_RANGE_MASK     0xFF000000

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

RM_STATUS heapCreate(PHWINFO, U008 *, U032, U032, PHEAP *);
RM_STATUS heapDestroy(PHWINFO, PHEAP);
RM_STATUS heapAlloc(PHWINFO, PHEAP, U032, U032 *, U032, U032, U032 *, U032 *);
RM_STATUS heapFbSetAllocParameters(PHWINFO, PFBALLOCINFO);
RM_STATUS blockFree(PHWINFO, PHEAP, PMEMBLOCK);
RM_STATUS heapFree(PHWINFO, PHEAP, U032, U032, U032 *);
RM_STATUS heapPurge(PHWINFO, PHEAP, U032);
RM_STATUS heapInfo(PHEAP, U032 *, U032 *, VOID**, U032 *, U032 *);
RM_STATUS heapInfoFreeBlocks(PHEAP, U032 *, U032 *, U032, U032, U032 *);
RM_STATUS heapCompact(PHEAP);
RM_STATUS heapGetSize(PHEAP, U032 *);
RM_STATUS heapGetFree(PHEAP, U032 *);
RM_STATUS heapGetMaxFree(PHEAP, U032 *, U032 *);
RM_STATUS heapGetBase(PHEAP, U008 **);

#endif // _HEAP_H_
