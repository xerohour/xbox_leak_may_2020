 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/***************************** Balanced Tree *******************************\
*                                                                           *
* Module: HEAP.C                                                            *
*   A generic library for heap management is provided.                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    05/19/98 - wrote it.                    *
*                                                                           *
\***************************************************************************/
#include <nvrm.h>
#include <class.h>
#include <heap.h>
#include <os.h>
#include <rmfail.h>
#include <smp.h>

//
// Globals.
//
static U032 memTypeFlags[13] =
{
    MEM_FLAG_IMAGE,
    MEM_FLAG_DEPTH      | MEM_GROW_DOWN,
    MEM_FLAG_TEXTURE,
    MEM_FLAG_OVERLAY,
    MEM_FLAG_FONT,
    MEM_FLAG_CURSOR     | MEM_GROW_DOWN,
    MEM_FLAG_DMA        | MEM_GROW_DOWN,
    MEM_FLAG_INSTANCE   | MEM_GROW_DOWN,
    MEM_FLAG_PRIMARY,
    MEM_FLAG_IMAGE_TILED,
    MEM_FLAG_DEPTH_COMPR16  | MEM_GROW_DOWN,
    MEM_FLAG_DEPTH_COMPR32  | MEM_GROW_DOWN,
    0
};

//
// Statics.
//
static RM_STATUS heapFbFree(PHWINFO, U032);

#ifdef DEBUG
/****************************************************************************/
/*                                                                          */
/*                             DEBUG support!                               */
/*                                                                          */
/****************************************************************************/
U032 dbgDumpHeap = 0;
#define HEAP_VALIDATE(h)    {heapValidate(h);if(dbgDumpHeap)heapDump(h);}

void heapDump
(
    PHEAP heap
)
{
    U032      i;
    U032      free;
    PMEMBLOCK block;

    if (!heap) return;
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"Heap dump.  Size = ", heap->total);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"            Free = ", heap->free);
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"=================================================================\r\n");
    for (i = 0; i < heap->numBanks; i++)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"Bank: ", i);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tType flags = ", heap->Bank[i].flags);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tOffset     = ", heap->Bank[i].offset);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tSize       = ", heap->Bank[i].size);
    }
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"Block List Forward:\r\n");
    block = heap->pBlockList;
    do
    {
        if (block->owner == FREE_BLOCK)
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"\tOwner = FREE\r\n");
        else
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tOwner = ", block->owner);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tType  = ", block->u0.type);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tHwres = ", block->u1.hwres);
        }
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\t\tBegin = ", block->begin);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\t\tEnd   = ", block->end);
        block = block->next;
    } while (block != heap->pBlockList);
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"Block List Reverse:\r\n");
    block = heap->pBlockList;
    do
    {
        block = block->prev;
        if (block->owner == FREE_BLOCK)
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"\tOwner = FREE\r\n");
        else
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tOwner = ", block->owner);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tType  = ", block->u0.type);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tHwres  = ", block->u1.hwres);
        }
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\t\tBegin = ", block->begin);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\t\tEnd   = ", block->end);
    } while (block != heap->pBlockList);
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"FREE Block List Forward:\r\n");
    free  = 0;
    if ((block = heap->pFreeBlockList))
        do
        {
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"\tOwner = FREE\r\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\t\tBegin = ", block->begin);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\t\tEnd   = ", block->end);
            free += block->end - block->begin + 1;
            block = block->u1.nextFree;
        } while (block != heap->pFreeBlockList);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tCalculated free count = ", free);
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"FREE Block List Reverse:\r\n");
    free  = 0;
    if ((block = heap->pFreeBlockList))
        do
        {
            block = block->u0.prevFree;
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"\tOwner = FREE\r\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\t\tBegin = ", block->begin);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\t\tEnd   = ", block->end);
            free += block->end - block->begin + 1;
        } while (block != heap->pFreeBlockList);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"\tCalculated free count = ", free);
}

void heapValidate
(
    PHEAP heap
)
{
    PMEMBLOCK block, blockFree;
    U032      free, used;

    if (!heap) return;
    /*
     * Scan the blocks and check for consistency.
     */
    free      = 0;
    used      = 0;
    block     = heap->pBlockList;
    blockFree = heap->pFreeBlockList;
    do
    {
        if (block->owner == FREE_BLOCK)
        {
            if (!blockFree)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Validate: Invalid free list with free blocks found.\r\n");
                heapDump(heap);
                DBG_BREAKPOINT();
            }
            free += block->end - block->begin + 1;
            if (block != blockFree)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Validate: Free list not consistent with block list.\r\n");
                heapDump(heap);
                DBG_BREAKPOINT();
            }
            blockFree = blockFree->u1.nextFree;
        }
        else
            used += block->end - block->begin + 1;
        if (block->next != heap->pBlockList)
        {
            if (block->end != block->next->begin - 1)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"Validate: Hole between blocks at offset = ", block->end);
                heapDump(heap);
                DBG_BREAKPOINT();
            }
        }
        else
        {
            if (block->end != heap->total - 1)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Validate: Last block doesn't end at top.\r\n");
                heapDump(heap);
                DBG_BREAKPOINT();
            }
            if (block->next->begin != 0)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Validate: First block doesn't start at bottom.\r\n");
                heapDump(heap);
                DBG_BREAKPOINT();
            }
        }
        if (block->end < block->begin)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"Validate: Invalid block begin = ", block->begin);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"                        end   = ", block->end);
            heapDump(heap);
            DBG_BREAKPOINT();
        }
        block = block->next;
    } while (block != heap->pBlockList);
    if (free != heap->free)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Validate: Calculated free count not consistent with heap free count.\r\n");
        heapDump(heap);
        DBG_BREAKPOINT();
    }
    if ((used + free) > heap->total)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Validate: Calculated used count not consistent with heap size.\r\n");
        heapDump(heap);
        DBG_BREAKPOINT();
    }
}
#else
#define HEAP_VALIDATE(h)
#endif // DEBUG

//
// Create a unique memhandle by incrementing the handle, and if it's about
// to overflow into the upper 2 bytes, reset it back to 0.
//
#define UNIQUE_MEMHANDLE()      \
        heap->memHandle = ((heap->memHandle + 1) % 0x10000) ? (heap->memHandle + 1) : 0xcafe0000


/****************************************************************************/
/*                                                                          */
/*                             Heap Manager                                 */
/*                                                                          */
/****************************************************************************/
//
// Create a heap.
//
RM_STATUS heapCreate
(
	PHWINFO pDev,
    U008   *pMem,
    U032   size,
    U032   numBanks,
    PHEAP  *heap
)
{
    PMEMBLOCK block;
    U032      sizeBank, i;

    //
    // Simply create a free heap with the indicated number of banks.
    //
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Heap Manager: HEAP ABOUT TO BE CREATED.\r\n");

    if (!numBanks) numBanks = 1;
    if (osAllocMem((VOID **)heap, sizeof(HEAP) + (numBanks - 1)*sizeof(MEMBANK)) == RM_OK)
    {
        (*heap)->base  = pMem;
        (*heap)->total = size;
        (*heap)->free  = size;
        if (osAllocMem((VOID **)&block, sizeof(MEMBLOCK)) != RM_OK)
        {
            osFreeMem((VOID *)*heap);
            return (RM_ERR_NO_FREE_MEM);
        }
        block->owner    = FREE_BLOCK;
        block->begin    = 0;
        block->align    = 0;
        block->end      = size - 1;
        block->u0.prevFree = block;
        block->u1.nextFree = block;
        block->next     = block;
        block->prev     = block;
        //
        // Fill in the heap bank info.
        //
        (*heap)->pBlockList     = block;
        (*heap)->pFreeBlockList = block;
        (*heap)->numBanks       = numBanks;
        (*heap)->memHandle      = 0xcafe0000;

        //We may modify the memory property table, so we make a per-device copy.
        for (i = 0; i < 11; i++) {
            (*heap)->memTypeFlags[i] = memTypeFlags[i];
        }

        sizeBank                = pDev->Framebuffer.HalInfo.RamSize / numBanks;
        for (i = 0; i < numBanks; i++)
        {
            (*heap)->Bank[i].offset = i * sizeBank;
            (*heap)->Bank[i].size   = sizeBank;
            (*heap)->Bank[i].flags  = 0;
        }

        (*heap)->Bank[0].flags             = (*heap)->memTypeFlags[MEM_TYPE_PRIMARY];
        (*heap)->Bank[numBanks - 1].flags  = (*heap)->memTypeFlags[MEM_TYPE_INSTANCE]
                                          |  (*heap)->memTypeFlags[MEM_TYPE_CURSOR]
                                          |  (*heap)->memTypeFlags[MEM_TYPE_DEPTH]
                                          |  (*heap)->memTypeFlags[MEM_TYPE_DEPTH_COMPR16]
                                          |  (*heap)->memTypeFlags[MEM_TYPE_DEPTH_COMPR32]
                                          |  (*heap)->memTypeFlags[MEM_TYPE_DMA];
        if (numBanks > 1)
        {
            if (numBanks > 2)
            {
                if (IsNV10orBetter(pDev) && (pDev->Framebuffer.HalInfo.RamType == BUFFER_DDRAM))
                {
                    /*
                     * With DDR, there's a penalty for switching external banks, so we'll try to keep
                     * important allocations (IMAGE/DEPTH) in the same external bank. The layout will be
                     *
                     *  bank0:          PRIMARY/IMAGE
                     *  bank1:          DEPTH
                     *  bank2:          IMAGE
                     *  bank3:          IMAGE and everything else
                     *
                     * Note, we'll set MEM_GROW_DOWN on any non-IMAGE/DEPTH buffers, so these start at the
                     * end of memory growing toward the middle leaving as big a block as possible unfragmented
                     * for IMAGE/DEPTH buffer allocs to grow into.
                     *
                     * Also, change IMAGE buffers to grow down and DEPTH buffers grow up. This fixes 2
                     * problems when an IMAGE/DEPTH buffer doesn't fit in their internal bank:
                     *
                     *  . in 10x7x32 triple buffered, when the 3rd IMAGE buffer doesn't fit in bank0,
                     *      we won't put it in with the DEPTH buffer in bank1 ... growing down will cause
                     *      it to be in bank2 or bank3.
                     *
                     *  . in 16x12x32 double buffered, when the DEPTH buffer doesn't fit in bank1, the 2nd
                     *      heapAlloc loop will place it as close as possible to the PRIMARY surface buffer.
                     *      The means it'll mostly fit in the first external bank, so we switch external banks
                     *      only 1/2 the time when flipping. In frontbuffer rendering we'll switch banks only
                     *      for the portion that's above 8MB.
                     */

                    (*heap)->memTypeFlags[MEM_TYPE_TEXTURE]  |= MEM_GROW_DOWN;
                    (*heap)->memTypeFlags[MEM_TYPE_OVERLAY]  |= MEM_GROW_DOWN;
                    (*heap)->memTypeFlags[MEM_TYPE_FONT]     |= MEM_GROW_DOWN;

                    (*heap)->memTypeFlags[MEM_TYPE_IMAGE]        |= MEM_GROW_DOWN;
                    (*heap)->memTypeFlags[MEM_TYPE_IMAGE_TILED]  |= MEM_GROW_DOWN;
                    (*heap)->memTypeFlags[MEM_TYPE_DEPTH]        &= ~MEM_GROW_DOWN;
                    (*heap)->memTypeFlags[MEM_TYPE_DEPTH_COMPR16]   &= ~MEM_GROW_DOWN;
                    (*heap)->memTypeFlags[MEM_TYPE_DEPTH_COMPR32]   &= ~MEM_GROW_DOWN;

                    (*heap)->Bank[0].flags             |= MEM_FLAG_IMAGE
                                                       |  MEM_FLAG_IMAGE_TILED;
                    (*heap)->Bank[1].flags             |= (*heap)->memTypeFlags[MEM_TYPE_DEPTH]
                                                       | (*heap)->memTypeFlags[MEM_TYPE_DEPTH_COMPR16]
                                                       | (*heap)->memTypeFlags[MEM_TYPE_DEPTH_COMPR32];

                    (*heap)->Bank[numBanks - 1].flags  |= (*heap)->memTypeFlags[MEM_TYPE_TEXTURE]
                                                       |  (*heap)->memTypeFlags[MEM_TYPE_OVERLAY]
                                                       |  (*heap)->memTypeFlags[MEM_TYPE_FONT];
                    (*heap)->Bank[numBanks - 1].flags  &= ~(MEM_FLAG_DEPTH
                                                       | MEM_FLAG_DEPTH_COMPR16
                                                       | MEM_FLAG_DEPTH_COMPR32);  // may not be necessary
                }
                else
                {
                    (*heap)->Bank[1].flags            |= (*heap)->memTypeFlags[MEM_TYPE_IMAGE];
                    (*heap)->Bank[numBanks - 2].flags |= (*heap)->memTypeFlags[MEM_TYPE_OVERLAY]
                                                      |  (*heap)->memTypeFlags[MEM_TYPE_TEXTURE];
                }
            }
            else
            {
                (*heap)->Bank[0].flags            |= (*heap)->memTypeFlags[MEM_TYPE_IMAGE]
                                                  |  (*heap)->memTypeFlags[MEM_TYPE_IMAGE_TILED];
                (*heap)->Bank[1].flags            |= (*heap)->memTypeFlags[MEM_TYPE_TEXTURE]
                                                  |  (*heap)->memTypeFlags[MEM_TYPE_OVERLAY]
                                                  |  (*heap)->memTypeFlags[MEM_TYPE_FONT];
            }
        }
        else
        {
            (*heap)->Bank[0].flags             = (*heap)->memTypeFlags[MEM_TYPE_TEXTURE]
                                              |  (*heap)->memTypeFlags[MEM_TYPE_IMAGE]
                                              |  (*heap)->memTypeFlags[MEM_TYPE_IMAGE_TILED]
                                              |  (*heap)->memTypeFlags[MEM_TYPE_OVERLAY]
                                              |  (*heap)->memTypeFlags[MEM_TYPE_FONT];
        }
    }
    return (RM_OK);
}

RM_STATUS heapDestroy
(
	PHWINFO pDev,
    PHEAP heap
)
{
    PMEMBLOCK block, blockFirst, blockNext;
    BOOL headptr_updated;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Heap Manager: HEAP ABOUT TO BE DESTROYED.\r\n");

    if ((!heap)) return (RM_ERROR);


    // we need to figure out what is preventing the w2k display driver from getting a primary surface allocation
    //  at offset zero after a mode switch -  added by C. Moidel
#ifdef DEBUG
    heapDump(heap);
#endif

    //
    // Free all allocated blocks, but preserving non-NVRM primary surfaces.
    // If the head of our list changes, restart the search, since our terminating
    // block pointer may not be in the list anymore.
    //
    do {
        block = blockFirst = heap->pBlockList;
        headptr_updated = FALSE;

        do {
            blockNext = block->next;

            if ((block->owner != FREE_BLOCK) &&
                ((block->u0.type != MEM_TYPE_PRIMARY) || (block->owner == 'NVRM'))) {

                blockFree(pDev, heap, block);

                // restart scanning the list, if the heap->pBlockList changed
                if (blockFirst != heap->pBlockList) {
                    headptr_updated = TRUE;
                    break;
                }
            }
            block = blockNext;

        } while (block != heap->pBlockList);

    } while (headptr_updated);

    //
    // Free the heap structure, if we freed everything
    // (the first block represents the entire free space of the heap).
    //
    if ((heap->pBlockList->begin == 0) &&
        (heap->pBlockList->end == heap->total - 1)) {
#if 0
        //
        // This is now a callable function and so, we can't assume we'll get
        // a chance to recreate it ... so don't free the one free block at the
        // head of the block list or the heap struct itself.
        //

        osFreeMem((VOID *)heap->pBlockList);
        osFreeMem((VOID *)heap);
        pDev->Framebuffer.HeapHandle = NULL;
#endif
    }

    return (RM_OK);
}

RM_STATUS heapAlloc
(
	PHWINFO pDev,
    PHEAP heap,
    U032  owner,
    U032 *size,
    U032  type,
    U032  height,
    U032 *pitch,
    U032 *offset
)
{
    U032      alignment, i, typeFlags, headNum;
    U032      bankLo, bankHi, freeLo, freeHi, allocLo, allocAl, allocHi;
    PMEMBLOCK blockFirstFree, blockFree, blockNew, blockSplit;
    PFBALLOCINFO pFbAllocInfo;
    FBALLOCINFO localFbAllocInfo;
    RM_STATUS status;
    BOOL ignoreBankPlacement = FALSE;

    if ((!heap)) return (RM_ERROR);
    *offset = 0;

    // no-op failure for now
    RM_FAILURE_HEAP(1, offset = offset);

    //
    // Check for valid size.
    //
    if (*size == 0) return (RM_ERROR);

    //
    // If this is a new primary surface alloc (and it's not us), we'll want to
    // free up the primary surface we allocated. Doing a heapPurge should be fine,
    // since we've made no other allocations. Make sure this is done before the
    // free check below.
    //
    if (((type & MEM_TYPE_MASK) == MEM_TYPE_PRIMARY) && (owner != 'NVRM'))
    {
        heapPurge(pDev, heap, 'NVRM');
    }

    //
    // First check for enough memory to satisfy this request.
    //
    if (heap->free < *size) return (RM_ERR_NO_FREE_MEM);

    //
    // Check if IGNORE_BANK_PLACEMENT_FLAG was passed in with the type to
    // ignore placing this allocation in a particular bank. This means we
    // default to the second loop where we choose first fit.
    //
    if (type & MEM_TYPE_IGNORE_BANK_PLACEMENT_FLAG)
        ignoreBankPlacement = TRUE;

    // Set default type flags (includes type and MEM_GROW dir)
    typeFlags = heap->memTypeFlags[type & MEM_TYPE_MASK];

    //
    // Check if FORCE_MEM_GROWS_UP or FORCE_MEM_GROWS_DOWN was passed in
    // to override the MEM_GROWS direction for this allocation.
    //
    if (type & MEM_TYPE_FORCE_MEM_GROWS_UP_FLAG)
        typeFlags = (typeFlags & ~MEM_GROW_MASK) | MEM_GROW_UP;
    if (type & MEM_TYPE_FORCE_MEM_GROWS_DOWN_FLAG)
        typeFlags = (typeFlags & ~MEM_GROW_MASK) | MEM_GROW_DOWN;

    // extract head number from the type argument
    headNum = (type & MEM_TYPE_HEAD_MASK) >> MEM_TYPE_HEAD_SHIFT;

    // Mask off any flags passed in with the type
    type &= MEM_TYPE_MASK;

    //
    // Force type into known values.
    //
    if (type > MEM_TYPE_MAX) type = 0;

    if (type == MEM_TYPE_PRIMARY) {
        pFbAllocInfo = &(pDev->Framebuffer.HalInfo.PrimaryFbAllocInfo[headNum]);
    } else {
        pFbAllocInfo = &localFbAllocInfo;
    }

    //
    // Request any chip-specific resources for memory of this
    // type (e.g. tiles). This call may adjust size, pitch and
    // alignment as necessary.
    //
    pFbAllocInfo->type = type;
    pFbAllocInfo->hwResId = 0;
    pFbAllocInfo->height = height;
    pFbAllocInfo->pitch = (pitch) ? (*pitch) : 0;
    pFbAllocInfo->size = *size;
    pFbAllocInfo->align = 0;

#if 0
    //
    // If we're in VGA mode, set pitch to 0 to ensure the HAL
    // doesn't do anything squirrely (i.e. allocate and use
    // tile resources).
    //
    if (pDev->Vga.Enabled)
        pFbAllocInfo->pitch = 0;
#endif

    //
    // Call into HAL to reserve any hardware resources for
    // the specified memory type.  We'll ignore any
    // insufficient resources errors for now.
    //
    status = nvHalFbAlloc(pDev, pFbAllocInfo);
    if (status != RM_OK && status != RM_ERR_INSUFFICIENT_RESOURCES)
    {
        // probably means we passed in a bogus type
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: nvHalFbAlloc failure!\n");
        return (RM_ERROR);
    }

    //
    // Refresh search parameters.
    //
    *size = pFbAllocInfo->size;
    if (pitch)
        *pitch = pFbAllocInfo->pitch;
    alignment = pFbAllocInfo->align;

    //
    // Find the best bank to start looking in for this type.
    //
    for (i = 0; (i < heap->numBanks) && (ignoreBankPlacement == FALSE); i++)
    {
        if ((heap->Bank[i].flags & typeFlags) & ~MEM_GROW_DOWN)
        {
            bankLo    = heap->Bank[i].offset;
            bankHi    = heap->Bank[i].offset + heap->Bank[i].size - 1;
            if (!(blockFirstFree = heap->pFreeBlockList))
            {
                heapFbFree(pDev, pFbAllocInfo->hwResId);
                return (RM_ERR_NO_FREE_MEM);
            }
            if (heap->Bank[i].flags & MEM_GROW_DOWN)
            {
                //
                // Search top down for the first free block in this range.
                //
                do
                {
                    blockFirstFree = blockFirstFree->u0.prevFree;
                } while ((blockFirstFree != heap->pFreeBlockList) && (blockFirstFree->begin > bankHi));
            }
            else
            {
                //
                // Search bottom up for the first free block in this range.
                //
                while (blockFirstFree->end < bankLo)
                    if ((blockFirstFree = blockFirstFree->u1.nextFree) == heap->pFreeBlockList) break;
            }
            //
            //  Start scanning for a free block.  Continue through all memory until
            // something (hopefully) is found.
            //
            blockFree = blockFirstFree;
            do
            {
                if ((blockFree->begin <= bankHi) && (blockFree->end >= bankLo))
                {
                    //
                    // Found some overlap. Check if it is big enough.
                    //
                    freeLo = (bankLo < blockFree->begin) ? blockFree->begin : bankLo;
                    freeHi = (bankHi > blockFree->end)   ? blockFree->end   : bankHi;
                    if (heap->Bank[i].flags & MEM_GROW_DOWN)
                    {
                        //
                        // Allocate from the top of the memory block.
                        //
                        allocLo = (freeHi - *size + 1) & ~alignment;
                        allocAl = allocLo;
                        allocHi = freeHi;
                    }
                    else
                    {
                        //
                        // Allocate from the bottom of the memory block.
                        //
                        allocLo = freeLo;
                        allocAl = (freeLo + alignment) & ~alignment;
                        allocHi = allocAl + *size - 1;
                    }
                    //
                    // Check for wrap-around.
                    //
                    if (allocLo > allocHi)
                        continue;
                    if ((allocLo == blockFree->begin) && (allocHi == blockFree->end))
                    {
                        //
                        // Wow, exact match so replace free block.
                        // Remove from free list.
                        //
                        blockFree->u1.nextFree->u0.prevFree = blockFree->u0.prevFree;
                        blockFree->u0.prevFree->u1.nextFree = blockFree->u1.nextFree;
                        if (heap->pFreeBlockList == blockFree)
                        {
                            //
                            // This could be the last free block.
                            //
                            if (blockFree->u1.nextFree == blockFree)
                                heap->pFreeBlockList = 0;
                            else
                                heap->pFreeBlockList = blockFree->u1.nextFree;
                        }
                        //
                        // Set owner/type values here.  Don't move because some fields are unions.
                        //
                        blockFree->owner   = owner;
                        blockFree->mhandle = UNIQUE_MEMHANDLE();
                        blockFree->u0.type = type;
                        blockFree->u1.hwres = pFbAllocInfo->hwResId;
                        blockFree->align   = allocAl;
                        //
                        // Reduce free amount by allocated block size.
                        //
                        heap->free -= blockFree->end - blockFree->begin + 1;
                        *offset     = blockFree->align;
                        pFbAllocInfo->offset = *offset;
                        //pFbAllocInfo->size = *size - 1;
                        if (type == MEM_TYPE_PRIMARY)
                        {
                            pDev->Framebuffer.PrimarySurfaceAlloced = 0x1;
                            pDev->Framebuffer.HalInfo.Start[headNum] = *offset;
                        }
                        heapFbSetAllocParameters(pDev, pFbAllocInfo);
                        HEAP_VALIDATE(heap);
                        return (RM_OK);
                    }
                    else if ((allocLo >= freeLo) && (allocHi <= freeHi))
                    {
                        //
                        // Budah-boom, found a match.
                        //
                        if (osAllocMem((VOID **)&blockNew, sizeof(MEMBLOCK)) != RM_OK)
                        {
                            heapFbFree(pDev, pFbAllocInfo->hwResId);
                            return (RM_ERR_NO_FREE_MEM);
                        }
                        blockNew->owner   = owner;
                        blockNew->mhandle = UNIQUE_MEMHANDLE();
                        blockNew->u0.type = type;
                        blockNew->u1.hwres = pFbAllocInfo->hwResId;
                        blockNew->begin   = allocLo;
                        blockNew->align   = allocAl;
                        blockNew->end     = allocHi;
                        if ((blockFree->begin < blockNew->begin) && (blockFree->end > blockNew->end))
                        {
                            //
                            // Split free block in two.
                            //
                            if (osAllocMem((VOID **)&blockSplit, sizeof(MEMBLOCK)) != RM_OK)
                            {
                                heapFbFree(pDev, pFbAllocInfo->hwResId);
                                osFreeMem((VOID *)blockNew);
                                return (RM_ERR_NO_FREE_MEM);
                            }
                            blockSplit->owner = FREE_BLOCK;
                            blockSplit->begin = blockNew->end + 1;
                            blockSplit->align = blockSplit->begin;
                            blockSplit->end   = blockFree->end;
                            blockFree->end    = blockNew->begin - 1;
                            //
                            // Insert free split block into free list.
                            //
                            blockSplit->u1.nextFree = blockFree->u1.nextFree;
                            blockSplit->u0.prevFree = blockFree;
                            blockSplit->u1.nextFree->u0.prevFree = blockSplit;
                            blockFree->u1.nextFree = blockSplit;
                            //
                            //  Insert new and split blocks into block list.
                            //
                            blockNew->next   = blockSplit;
                            blockNew->prev   = blockFree;
                            blockSplit->next = blockFree->next;
                            blockSplit->prev = blockNew;
                            blockFree->next  = blockNew;
                            blockSplit->next->prev = blockSplit;
                        }
                        else if (blockFree->end == blockNew->end)
                        {
                            //
                            // New block inserted after free block.
                            //
                            blockFree->end = blockNew->begin - 1;
                            blockNew->next = blockFree->next;
                            blockNew->prev = blockFree;
                            blockFree->next->prev = blockNew;
                            blockFree->next = blockNew;
                        }
                        else if (blockFree->begin == blockNew->begin)
                        {
                            //
                            // New block inserted before free block.
                            //
                            blockFree->begin = blockNew->end + 1;
                            blockFree->align = blockFree->begin;
                            blockNew->next   = blockFree;
                            blockNew->prev   = blockFree->prev;
                            blockFree->prev->next = blockNew;
                            blockFree->prev = blockNew;
                            if (heap->pBlockList == blockFree)
                                heap->pBlockList  = blockNew;
                        }
                        else
                        {
                            heapFbFree(pDev, pFbAllocInfo->hwResId);
                            osFreeMem((VOID *)blockNew);
                            HEAP_VALIDATE(heap);
                            return (RM_ERR_NO_FREE_MEM);
                        }
                        //
                        // Reduce free amount by allocated block size.
                        //
                        heap->free -= blockNew->end - blockNew->begin + 1;
                        *offset     = blockNew->align;
                        pFbAllocInfo->offset = *offset;
                        //pFbAllocInfo->size = *size - 1;
                        if (type == MEM_TYPE_PRIMARY)
                        {
                            pDev->Framebuffer.PrimarySurfaceAlloced = 0x1;
                            pDev->Framebuffer.HalInfo.Start[headNum] = *offset;
                        }
                        heapFbSetAllocParameters(pDev, pFbAllocInfo);
                        HEAP_VALIDATE(heap);
                        return (RM_OK);
                    }
                }
                blockFree = blockFree->u0.prevFree;
            } while (blockFree != blockFirstFree);
        }
    }

    //
    // Bummer, no nice bank blocks were found.  Try again, only look for anything.
    //
    if (!(blockFirstFree = heap->pFreeBlockList))
    {
        heapFbFree(pDev, pFbAllocInfo->hwResId);
        return (RM_ERR_NO_FREE_MEM);
    }

    //
    // When scanning upwards, start at the bottom - 1 so the following loop looks symetrical.
    //
    if (!(typeFlags & MEM_GROW_DOWN))
        blockFirstFree = blockFirstFree->u0.prevFree;
    blockFree = blockFirstFree;
    do
    {
        if (typeFlags & MEM_GROW_DOWN)
        {
            //
            // Allocate from the top of the memory block.
            //
            blockFree = blockFree->u0.prevFree;
            allocLo   = (blockFree->end - *size + 1) & ~alignment;
            allocAl   = allocLo;
            allocHi   = blockFree->end;
        }
        else
        {
            //
            // Allocate from the bottom of the memory block.
            //
            blockFree = blockFree->u1.nextFree;
            allocLo   = blockFree->begin;
            allocAl   = (blockFree->begin + alignment) & ~alignment;
            allocHi   = allocAl + *size - 1;
        }
        //
        // Check for wrap-around.
        //
        if (allocLo > allocHi)
            continue;
        if ((allocLo == blockFree->begin) && (allocHi == blockFree->end))
        {
            //
            // Wow, exact match so replace free block.
            // Remove from free list.
            //
            blockFree->u1.nextFree->u0.prevFree = blockFree->u0.prevFree;
            blockFree->u0.prevFree->u1.nextFree = blockFree->u1.nextFree;
            if (heap->pFreeBlockList == blockFree)
            {
                //
                // This could be the last free block.
                //
                if (blockFree->u1.nextFree == blockFree)
                    heap->pFreeBlockList = 0;
                else
                    heap->pFreeBlockList = blockFree->u1.nextFree;
            }
            blockFree->owner   = owner;
            blockFree->mhandle = UNIQUE_MEMHANDLE();
            blockFree->u0.type = type;
            blockFree->u1.hwres  = pFbAllocInfo->hwResId;
            blockFree->align   = allocAl;
            //
            // Reduce free amount by allocated block size.
            //
            heap->free -= blockFree->end - blockFree->begin + 1;
            *offset     = blockFree->align;
            pFbAllocInfo->offset = *offset;
            //pFbAllocInfo->size = *size - 1;
            if (type == MEM_TYPE_PRIMARY)
            {
                pDev->Framebuffer.PrimarySurfaceAlloced = 0x1;
                pDev->Framebuffer.HalInfo.Start[headNum] = *offset;
            }
            heapFbSetAllocParameters(pDev, pFbAllocInfo);
            HEAP_VALIDATE(heap);
            return (RM_OK);
        }
        else if ((allocLo >= blockFree->begin) && (allocHi <= blockFree->end))
        {
            //
            // Found a fit.
            //
            if (osAllocMem((VOID **)&blockNew, sizeof(MEMBLOCK)) != RM_OK)
            {
                heapFbFree(pDev, pFbAllocInfo->hwResId);
                return (RM_ERR_NO_FREE_MEM);
            }
            blockNew->owner   = owner;
            blockNew->mhandle = UNIQUE_MEMHANDLE();
            blockNew->u0.type = type;
            blockNew->u1.hwres = pFbAllocInfo->hwResId;
            blockNew->begin   = allocLo;
            blockNew->align   = allocAl;
            blockNew->end     = allocHi;
            if (blockFree->end == blockNew->end)
            {
                //
                // New block inserted after free block.
                //
                blockFree->end = blockNew->begin - 1;
                blockNew->next = blockFree->next;
                blockNew->prev = blockFree;
                blockFree->next->prev = blockNew;
                blockFree->next       = blockNew;
            }
            else if (blockFree->begin == blockNew->begin)
            {
                //
                // New block inserted before free block.
                //
                blockFree->begin = blockNew->end + 1;
                blockFree->align = blockFree->begin;
                blockNew->next   = blockFree;
                blockNew->prev   = blockFree->prev;
                blockFree->prev->next = blockNew;
                blockFree->prev       = blockNew;
                if (heap->pBlockList == blockFree)
                    heap->pBlockList  = blockNew;
            }
            else
            {
                heapFbFree(pDev, pFbAllocInfo->hwResId);
                osFreeMem((VOID *)blockNew);
                HEAP_VALIDATE(heap);
                return (RM_ERR_NO_FREE_MEM);
            }
            //
            // Reduce free amount by allocated block size.
            //
            heap->free -= blockNew->end - blockNew->begin + 1;
            *offset     = blockNew->align;
            pFbAllocInfo->offset = *offset;
            //pFbAllocInfo->size = *size - 1;
            if (type == MEM_TYPE_PRIMARY)
            {
                pDev->Framebuffer.PrimarySurfaceAlloced = 0x1;
                pDev->Framebuffer.HalInfo.Start[headNum] = *offset;
            }
            heapFbSetAllocParameters(pDev, pFbAllocInfo);
            HEAP_VALIDATE(heap);
            return (RM_OK);
        }
    } while (blockFree != blockFirstFree);
    //
    // Sorry Charlie, out of memory.  Could in theory try to compact mem and try again. Later.
    //
    heapFbFree(pDev, pFbAllocInfo->hwResId);
    HEAP_VALIDATE(heap);
    return (RM_ERR_NO_FREE_MEM);
}

RM_STATUS blockFree
(
	PHWINFO   pDev,
    PHEAP     heap,
    PMEMBLOCK block
)
{
    PMEMBLOCK blockTmp;

    //
    // Check for valid owner.
    //
    if (block->owner == FREE_BLOCK) return (RM_ERROR);
    //
    // Update free count.
    //
    heap->free += block->end - block->begin + 1;
    //
    // Release any tiling HW resources that might've been in use
    //
    heapFbFree(pDev, block->u1.hwres);

    // Has the primary surface gone away?
    if (block->u0.type == MEM_TYPE_PRIMARY)
        pDev->Framebuffer.PrimarySurfaceAlloced = 0x0;

    //
    //
    // Can this merge with any surrounding free blocks?
    //
    if ((block->prev->owner == FREE_BLOCK) && (block != heap->pBlockList))
    {
        //
        // Merge with previous block.
        //
        block->prev->next = block->next;
        block->next->prev = block->prev;
        block->prev->end  = block->end;
        blockTmp = block;
        block    = block->prev;
        osFreeMem((VOID *)blockTmp);
    }
    if ((block->next->owner == FREE_BLOCK) && (block->next != heap->pBlockList))
    {
        //
        // Merge with next block.
        //
        block->prev->next    = block->next;
        block->next->prev    = block->prev;
        block->next->begin   = block->begin;
        if (heap->pBlockList == block)
            heap->pBlockList  = block->next;
        if (block->owner == FREE_BLOCK)
        {
            if (heap->pFreeBlockList == block)
                heap->pFreeBlockList  = block->u1.nextFree;
            block->u1.nextFree->u0.prevFree = block->u0.prevFree;
            block->u0.prevFree->u1.nextFree = block->u1.nextFree;
        }
        blockTmp = block;
        block    = block->next;
        osFreeMem((VOID *)blockTmp);
    }
    if (block->owner != FREE_BLOCK)
    {
        //
        // Nothing was merged.  Add to free list.
        //
        if (!(blockTmp = heap->pFreeBlockList))
        {
            heap->pFreeBlockList = block;
            block->u1.nextFree      = block;
            block->u0.prevFree      = block;
        }
        else
        {
            if (blockTmp->begin > block->begin)
                //
                // Insert into beginning of free list.
                //
                heap->pFreeBlockList = block;
            else if (blockTmp->u0.prevFree->begin > block->begin)
                //
                // Insert into free list.
                //
                do
                {
                    blockTmp = blockTmp->u1.nextFree;
                } while (blockTmp->begin < block->begin);
                /*
            else
                 * Insert at end of list.
                 */
            block->u1.nextFree = blockTmp;
            block->u0.prevFree = blockTmp->u0.prevFree;
            block->u0.prevFree->u1.nextFree = block;
            blockTmp->u0.prevFree        = block;
        }
    }
    block->owner   = FREE_BLOCK;
    block->mhandle = 0x0;
    block->align   = block->begin;
    HEAP_VALIDATE(heap);
    return (RM_OK);
}

RM_STATUS heapFree
(
	PHWINFO pDev,
    PHEAP heap,
    U032  owner,
    U032  offsetAlign,
    U032  *memHandle
)
{
    PMEMBLOCK block;

    if ((!heap)) return (RM_ERROR);
    if (owner == FREE_BLOCK) return (RM_ERROR);
    //
    // This requires a search through the heap of this memory block.
    // Try and be somewhat intelligent about it.
    //
    block = heap->pBlockList;
    if (offsetAlign > heap->total / 2)
        //
        // Search backward.
        //
        do
        {
            block = block->prev;
            if ((block->align == offsetAlign) && (block->owner == owner))
            {
                //
                // Found it.  Free it. Return the associated memory handle.
                //
                *memHandle = block->mhandle;
                return (blockFree(pDev, heap, block));
            }
        } while ((block != heap->pBlockList) && (block->align > offsetAlign));
    else
        //
        // Search forward.
        //
        do
        {
            if ((block->align == offsetAlign) && (block->owner == owner))
            {
                //
                // Found it.  Free it. Return the associated memory handle.
                //
                *memHandle = block->mhandle;
                return (blockFree(pDev, heap, block));
            }
            block = block->next;
        } while ((block != heap->pBlockList) && (block->align <= offsetAlign));
    HEAP_VALIDATE(heap);
    return (RM_ERROR);
}

RM_STATUS heapPurge
(
	PHWINFO pDev,
    PHEAP heap,
    U032  owner
)
{
    PMEMBLOCK block, blockNext;

    if ((!heap)) return (RM_ERROR);
    //
    // Scan for all the blocks belonging by this owner.
    //
    block = heap->pBlockList;
    do
    {
        blockNext = block->next;
        if (block->owner == owner)
            blockFree(pDev, heap, block);
        block = blockNext;
    } while (block->next != heap->pBlockList);
    //
    // Check the last one.
    //
    if (block->owner == owner)
        blockFree(pDev, heap, block);
    HEAP_VALIDATE(heap);
    return (RM_OK);
}

RM_STATUS heapCompact
(
    PHEAP heap
)
{
    if ((!heap)) return (RM_ERROR);
    //
    // Not implemented.
    //
    HEAP_VALIDATE(heap);
    return (RM_OK);
}

RM_STATUS heapGetSize
(
    PHEAP heap,
    U032 *size
)
{
    if ((!heap)) return (RM_ERROR);
    *size = heap->total;
    HEAP_VALIDATE(heap);
    return (RM_OK);
}

RM_STATUS heapGetFree
(
    PHEAP heap,
    U032 *free
)
{
    if ((!heap)) return (RM_ERROR);
    *free = heap->free;
    HEAP_VALIDATE(heap);
    return (RM_OK);
}

RM_STATUS heapGetBase
(
    PHEAP  heap,
    U008 **base
)
{
    if ((!heap)) return (RM_ERROR);
    *base = heap->base;
    HEAP_VALIDATE(heap);
    return (RM_OK);
}

RM_STATUS heapGetMaxFree
(
    PHEAP heap,
    U032 *maxOffset,
    U032 *maxFree
)
{
    PMEMBLOCK blockFirstFree, blockFree;
    U032 freeBlockSize;

    *maxFree = 0;
    if ((!heap)) return (RM_ERROR);

    if (!(blockFirstFree = heap->pFreeBlockList))
        // There are no free blocks. Max free is already set to 0
        return (RM_OK);

    // Walk the free block list.
    blockFree = blockFirstFree;
    do {
        freeBlockSize = blockFree->end - blockFree->begin + 1;
        if (freeBlockSize > *maxFree)
        {
            *maxOffset = blockFree->begin;
            *maxFree = freeBlockSize;
        }
        blockFree = blockFree->u1.nextFree;
    } while (blockFree != blockFirstFree);

    return (RM_OK);
}

RM_STATUS heapInfo
(
    PHEAP heap,
    U032 *bytesFree,
    U032 *bytesTotal,
    VOID** base,
    U032 *largestOffset,      // largest free blocks offset
    U032 *largestFree         // largest free blocks size
)
{
    RM_STATUS status;

    if ((!heap)) return (RM_ERROR);

    *bytesFree  = heap->free;
    *bytesTotal = heap->total;
    *base  = heap->base;
    status = heapGetMaxFree(heap, largestOffset, largestFree);
    HEAP_VALIDATE(heap);
    return status;
}

RM_STATUS heapInfoFreeBlocks
(
    PHEAP heap,
    U032 *bytesFree,
    U032 *bytesTotal,
    U032  freeOffset1,
    U032  freeOffset2,
    U032  *largestFree
)
{
    PMEMBLOCK blockArg1, blockArg2, blockArgRM; 
    PMEMBLOCK pLocalFreeBlockList = NULL;
    PMEMBLOCK block = NULL, blockFree;
    PMEMBLOCK mergedBlock = NULL, mergedBlock1 = NULL, mergedBlock2 = NULL;
    U032 freeBlockSize;
    BOOL mergedArg1, mergedArg2, mergedArgRM;
    RM_STATUS status = RM_OK;

    if ((!heap)) return (RM_ERROR);

    RM_ASSERT(heap->pBlockList->begin == 0);

    // Find where the block pointers are for the offset arguments
    block = heap->pBlockList;
    blockArg1 = blockArg2 = blockArgRM = NULL;
    do {
        if (block->align == freeOffset1) {
            blockArg1 = block;
            if (blockArg1->owner == FREE_BLOCK)
                return RM_ERROR;    // can't be an already free block
        } else if (block->align == freeOffset2) {
            blockArg2 = block;
            if (blockArg2->owner == FREE_BLOCK)
                return RM_ERROR;    // can't be an already free block
        } else if (!blockArgRM && block->owner == 'NVRM' &&
                   (block->u0.type = MEM_TYPE_PRIMARY)) {
            blockArgRM = block;     // checking only 1 RM primary surface
        }
        block = block->next;
    } while (block != heap->pBlockList);

    // Check if we found the block for offset1
    if ((freeOffset1 != (U032)-1) && (blockArg1 == NULL))
        return RM_ERROR;

    // Check if we found the block for offset2
    if ((freeOffset2 != (U032)-1) && (blockArg2 == NULL))
        return RM_ERROR;

    //
    // Handle the special case where both freeOffsets map to adjacent blocks.
    // Create a combined blockArg that describes both blocks (clearing the other
    // blockArg pointer). This doesn't disturb the active freelist block ptrs.
    //
    if (blockArg1 && blockArg2) {
        if (((blockArg1->next == blockArg2) && (blockArg2 != heap->pBlockList)) ||
            ((blockArg2->next == blockArg1) && (blockArg1 != heap->pBlockList))) {

            // Create a combined block for both blockArgs
            if (osAllocMem((VOID **)&mergedBlock, sizeof(MEMBLOCK)) != RM_OK) {
                status = RM_ERR_NO_FREE_MEM;
                goto Done;
            }

            // Copy the merged values
            if (blockArg1->next == blockArg2) {
                mergedBlock->begin = blockArg1->begin;
                mergedBlock->prev  = blockArg1->prev;
                mergedBlock->end   = blockArg2->end;
                mergedBlock->next  = blockArg2->next;
            } else {
                mergedBlock->begin = blockArg2->begin;
                mergedBlock->prev  = blockArg2->prev;
                mergedBlock->end   = blockArg1->end;
                mergedBlock->next  = blockArg1->next;
            }

            // Now, we're down to one less block to check
            blockArg1 = mergedBlock;
            blockArg2 = NULL;
        }
    }
    if (blockArg1 && blockArgRM) {
        if (((blockArg1->next == blockArgRM) && (blockArgRM != heap->pBlockList)) ||
            ((blockArgRM->next == blockArg1) && (blockArg1 != heap->pBlockList))) {

            // Create a combined block for both blockArgs
            if (osAllocMem((VOID **)&mergedBlock1, sizeof(MEMBLOCK)) != RM_OK) {
                status = RM_ERR_NO_FREE_MEM;
                goto Done;
            }

            // Copy the merged values
            if (blockArg1->next == blockArgRM) {
                mergedBlock1->begin = blockArg1->begin;
                mergedBlock1->prev  = blockArg1->prev;
                mergedBlock1->end   = blockArgRM->end;
                mergedBlock1->next  = blockArgRM->next;
            } else {
                mergedBlock1->begin = blockArgRM->begin;
                mergedBlock1->prev  = blockArgRM->prev;
                mergedBlock1->end   = blockArg1->end;
                mergedBlock1->next  = blockArg1->next;
            }

            // Now, we're down to one less block to check
            blockArg1 = mergedBlock1;
            blockArgRM = NULL;
        }
    }
    if (blockArgRM && blockArg2) {
        if (((blockArgRM->next == blockArg2) && (blockArg2 != heap->pBlockList)) ||
            ((blockArg2->next == blockArgRM) && (blockArgRM != heap->pBlockList))) {

            // Create a combined block for both blockArgs
            if (osAllocMem((VOID **)&mergedBlock2, sizeof(MEMBLOCK)) != RM_OK) {
                status = RM_ERR_NO_FREE_MEM;
                goto Done;
            }

            // Copy the merged values
            if (blockArgRM->next == blockArg2) {
                mergedBlock2->begin = blockArgRM->begin;
                mergedBlock2->prev  = blockArgRM->prev;
                mergedBlock2->end   = blockArg2->end;
                mergedBlock2->next  = blockArg2->next;
            } else {
                mergedBlock2->begin = blockArg2->begin;
                mergedBlock2->prev  = blockArg2->prev;
                mergedBlock2->end   = blockArgRM->end;
                mergedBlock2->next  = blockArgRM->next;
            }

            // Now, we're down to a single blockArg to check
            blockArgRM = mergedBlock2;
            blockArg2 = NULL;
        }
    }

    mergedArg1 = mergedArg2 = mergedArgRM = FALSE;

    //
    // Setup our freelist by duplicating the real heap->pBlockFreeList. We'll
    // also coalesce the blockArgs with surrounding free blocks as we go.
    //
    if (heap->pFreeBlockList) {
        blockFree = heap->pFreeBlockList;
        do {
            // Create a copy of this free block
            status = osAllocMem((VOID **)&block, sizeof(MEMBLOCK));
            if (status != RM_OK)
                goto Done;    // failed to dup, so cleanup and return

            // Copy the blockFree's initial values
            block->begin    = blockFree->begin;
            block->end      = blockFree->end;

            // 
            // Check if free block precedes blockArg1, blockArg2 or blockArgRM
            //
            // N.B., comparing (blockArg->begin != 0x0) should be equivalent to
            // checking for (blockArg != heap->pBlockList), both are trying to
            // determine if we're at the beginning of the list.
            //
            if (blockArg1 && ((blockArg1->prev == blockFree) && blockArg1->begin)) {
                mergedArg1 = TRUE;
                block->end = blockArg1->end;
            }
            if (blockArg2 && ((blockArg2->prev == blockFree) && blockArg2->begin)) {
                mergedArg2 = TRUE;
                block->end = blockArg2->end;
            }
            if (blockArgRM && ((blockArgRM->prev == blockFree) && blockArgRM->begin)) {
                mergedArgRM = TRUE;
                block->end = blockArgRM->end;        // include a RM primary surface
            }

            // 
            // Check if free block succeeds blockArg1, blockArg2 or blockArgRM
            //
            if (blockArg1 &&
                ((blockArg1->next == blockFree) && (blockFree != heap->pBlockList))) {
                //
                // If a free block also preceded blockArg, then we want to concatenate
                // the two free blocks and can free up the dup block we already allocated
                //
                if (pLocalFreeBlockList &&
                    ((pLocalFreeBlockList->u0.prevFree->end+1) == blockFree->begin)) {
                    pLocalFreeBlockList->u0.prevFree->end = block->end;
                    osFreeMem((VOID *)block);
                    block = NULL;
                } else
                    block->begin = blockArg1->begin;
                mergedArg1 = TRUE;
            }
            if (blockArg2 &&
                ((blockArg2->next == blockFree) && (blockFree != heap->pBlockList))) {
                //
                // If a free block also preceded blockArg, then we want to concatenate
                // the two free blocks and can free up the dup block we already allocated
                //
                if (pLocalFreeBlockList &&
                    ((pLocalFreeBlockList->u0.prevFree->end+1) == blockFree->begin)) {
                    pLocalFreeBlockList->u0.prevFree->end = block->end;
                    osFreeMem((VOID *)block);
                    block = NULL;
                } else
                    block->begin = blockArg2->begin;
                mergedArg2 = TRUE;
            }
            if (blockArgRM &&
                ((blockArgRM->next == blockFree) && (blockFree != heap->pBlockList))) {
                //
                // If a free block also preceded blockArgRM, then we want to concatenate
                // the two free blocks and can free up the dup block we already allocated
                // 
                if (pLocalFreeBlockList &&
                    ((pLocalFreeBlockList->u0.prevFree->end+1) == blockFree->begin)) {
                    pLocalFreeBlockList->u0.prevFree->end = block->end;
                    osFreeMem((VOID *)block);
                    block = NULL;
                } else 
                    block->begin = blockArgRM->begin;
                mergedArgRM = TRUE;
            }

            // Add this block to our list of free blocks
            if (block) {
                if (pLocalFreeBlockList == NULL) {
                    block->u0.prevFree  = block;
                    block->u1.nextFree  = block;
                    pLocalFreeBlockList = block;
                } else {
                    block->u0.prevFree  = pLocalFreeBlockList->u0.prevFree;
                    block->u1.nextFree  = pLocalFreeBlockList;
                    pLocalFreeBlockList->u0.prevFree->u1.nextFree = block;
                    pLocalFreeBlockList->u0.prevFree = block;
                }
            }

            blockFree = blockFree->u1.nextFree;
        } while (blockFree != heap->pFreeBlockList);
    }

    //
    // At this point, we have a built up a pLocalFreeBlockList, that has
    // taken into account any blocks that we can consider freed. Now, find
    // the largest contiguous block that would be availble.
    //
    *bytesFree  = *largestFree = 0;
    *bytesTotal = heap->total;

    blockFree = pLocalFreeBlockList;
    if (blockFree) {
        // Walk the free block list.
        do {
            freeBlockSize = blockFree->end - blockFree->begin + 1;
            *bytesFree += freeBlockSize;
            if (freeBlockSize > *largestFree)
                *largestFree = freeBlockSize;
            blockFree = blockFree->u1.nextFree;
        } while (blockFree != pLocalFreeBlockList);
    }

    //
    // Since the blockArgs weren't added to the pLocalFreeBlockList, if
    // there was no coalescing that occurred, the blockArgs may hold the
    // largest free block (and are added into the total free amount).
    //
    if (blockArg1 && (mergedArg1 == FALSE)) {
        freeBlockSize = blockArg1->end - blockArg1->begin + 1;
        *bytesFree += freeBlockSize;
        if (freeBlockSize > *largestFree)
            *largestFree = freeBlockSize;
    }
    if (blockArg2 && (mergedArg2 == FALSE)) {
        freeBlockSize = blockArg2->end - blockArg2->begin + 1;
        *bytesFree += freeBlockSize;
        if (freeBlockSize > *largestFree)
            *largestFree = freeBlockSize;
    }
    if (blockArgRM && (mergedArgRM == FALSE)) {
        freeBlockSize = blockArgRM->end - blockArgRM->begin + 1;
        *bytesFree += freeBlockSize;
        if (freeBlockSize > *largestFree)
            *largestFree = freeBlockSize;
    }

Done:
    // Free up whatever is on our pLocalFreeBlockList
    blockFree = pLocalFreeBlockList;
    if (blockFree) {
        do {
            block = blockFree->u1.nextFree;
            osFreeMem((VOID *)blockFree);
            blockFree = block;
        } while (blockFree != pLocalFreeBlockList);
    }
    // Free up the mergedBlock, if one was created
    if (mergedBlock)
        osFreeMem((VOID *)mergedBlock);
    if (mergedBlock1)
        osFreeMem((VOID *)mergedBlock1);
    if (mergedBlock2)
        osFreeMem((VOID *)mergedBlock2);

    HEAP_VALIDATE(heap);
    return status;
}

//
// These routines may need to be mutexed if there are hardware
// resources involved.
//
RM_STATUS heapFbSetAllocParameters
(
    PHWINFO pDev,
    PFBALLOCINFO pFbAllocInfo
)
{
    RM_STATUS status;

    //
    // If we're in VGA mode, do not set tiling parameters until after we've
    // done a modeset into an accelerated mode to prevent from corrupting
    // the VGA splash screen on win9x (see fbstate.c for when this
    // happens).
    //
    if (pDev->Vga.Enabled && (pFbAllocInfo->type == MEM_TYPE_PRIMARY) && (pFbAllocInfo->hwResId != 0))
        return (RM_OK);

    if (pFbAllocInfo->hwResId)
        osEnterCriticalCode(pDev);

    status = nvHalFbSetAllocParameters(pDev, pFbAllocInfo);

    if (pFbAllocInfo->hwResId)
        osExitCriticalCode(pDev);

    return status;
}

static RM_STATUS heapFbFree
(
	PHWINFO pDev,
    U032 hwResId
)
{
    U032 i;
    RM_STATUS status;

    if (hwResId)
        osEnterCriticalCode(pDev);

    status = nvHalFbFree(pDev, hwResId);

    // Also, reset PrimaryFbAllocInfo if the hwResID matches
    if (hwResId) {
        for (i = 0; i < MAX_CRTCS; i++) {
            if (pDev->Framebuffer.HalInfo.PrimaryFbAllocInfo[i].hwResId == hwResId)
                pDev->Framebuffer.HalInfo.PrimaryFbAllocInfo[i].hwResId = 0;
        }
    }

    if (hwResId)
        osExitCriticalCode(pDev);

    return status;
}
