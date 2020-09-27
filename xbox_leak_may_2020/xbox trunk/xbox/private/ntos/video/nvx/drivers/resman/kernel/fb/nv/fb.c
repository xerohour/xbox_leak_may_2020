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

/****************************** FB Manager *********************************\
*                                                                           *
* Module: FB.C                                                              *
*   The FB is managed in this module.                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <fifo.h>
#include <dma.h>
#include <gr.h>
#include <fb.h>
#include <os.h>
#include <vga.h>
#include "nvhw.h"


//---------------------------------------------------------------------------
//
//  Instance allocation bitmap.
//
//---------------------------------------------------------------------------

RM_STATUS fbInitializeInstMemBitMap
(
    PHWINFO pDev
)
{
    RM_STATUS status;
    U008      *pbyte;
    U032      size;
    U032      entry;

    //
    // Initialize instance memory usage bitmap.  Each bit equals 1 paragraph (16 bytes)
    // of instance memory.
    //
    pDev->Pram.FreeInstMax = (pDev->Pram.FreeInstSize  / 16) & ~0x07;
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: FB Free Size       = ", pDev->Pram.FreeInstSize);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: FB Free Inst Base  = ", pDev->Pram.FreeInstBase);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: FB Free Inst Max   = ", pDev->Pram.FreeInstMax + pDev->Pram.FreeInstBase);

    size = (pDev->Pram.FreeInstMax / 8 + 4);
    status = osAllocMem((VOID **)&pbyte, size);
    if (status)
        return (status);

    pDev->DBfbInstBitmap = pbyte;

    //
    // Reserve instance 0 as the NULL instance.
    //    
    *pbyte++ = 0xFE;    
    while (--size)
        *pbyte++ = 0xFF;

    //
    // Allocate Instance memory for the cursor image - must be aligned to 2k.
    //
    // KJK This should be removed once the new cursor interface is enabled in the display
    // driver.
    //
    status = fbAllocInstMemAlign(pDev, &pDev->Dac.CursorImageInstance, 0x800 / 16, 0x800 / 16);
    if (status)
    {
        osFreeMem((void *)pDev->DBfbInstBitmap);
        pDev->DBfbInstBitmap = 0;
        return (status);
    }


    //
    // Allocate the cached cursors and clear all current state
    // 
    for (entry=0; entry < MAX_CACHED_CURSORS; entry++)
    {   
        status = fbAllocInstMemAlign(pDev, &pDev->Dac.CursorImageCacheInstance[entry], 0x800 / 16, 0x800 / 16);
        if (status)
        {
            osFreeMem((void *)pDev->DBfbInstBitmap);
            pDev->DBfbInstBitmap = 0;
            return (status);
        }
        //
        // Since we're no longer maintaining the cache across mode changes, just
        // wipe the cache clean.
        //
        pDev->Dac.CursorMaskCRC[entry] = 0;                    
    }                    

    return RM_OK;
} // end of fbInitializeInstMemBitMap 

//---------------------------------------------------------------------------
//
//  PRAM allocation and de-allocation.
//
//---------------------------------------------------------------------------

/*	alloc instance mem with specified alignment
*/
RM_STATUS fbAllocInstMemAlign
(
    PHWINFO pDev,
    U032   *Instance,
    U032    Size,
    U032    Align		
)
{
    U032 i;
    U032 j;
	U032 mask;

    if (pDev->Power.State != 0)
    {
        return (RM_ERR_NO_FREE_MEM);
    }

    DBG_VAL_PTR(pDev->DBfbInstBitmap);
#if DEBUG
    if (Size == 0)    
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Zero size in fbAllocInstMem!\n\r");
        DBG_BREAKPOINT();
    }
#endif
    *Instance = 0;
	i = pDev->Pram.FreeInstBase;		// get base of inst memory
	mask = Align - 1;
	while (mask & i)		 			// align i
		i += 0x1;						// try next page
	i -= pDev->Pram.FreeInstBase;		// offset from base
    for ( ; i < pDev->Pram.FreeInstMax; i += Align)
    {
		U032 k = i;

        if (pDev->DBfbInstBitmap[k >> 3] & BIT(k & 7))		// available?
        {
            //
            // Search for string of 1's the length of the block.
            //
            j = 0;
            do
            {
                if ((++k >= pDev->Pram.FreeInstMax) || (++j >= Size))
                    break;
            }
            while (pDev->DBfbInstBitmap[k >> 3] & BIT(k & 7));
            if (j == Size)
            {
                //
                // Score, a block was found.
                //
                *Instance = k - j + pDev->Pram.FreeInstBase;
                while (j--)
                {
                    //
                    // Mark instance memory as allocated.
                    //
                    k--;
                    pDev->DBfbInstBitmap[k >> 3] &= ~BIT(k & 7);
                }
#if defined(NTRM) && defined(DBG)
				osLogInstMemAlloc(*Instance, Size, Align);
#endif
                return (RM_OK);
            }
        }
    }
    return (RM_ERR_NO_FREE_MEM);
}

RM_STATUS fbAllocInstMem
(
    PHWINFO pDev,
    U032   *Instance,
    U032    Size
)
{
    U032 i;
    U032 j;

    if (pDev->Power.State != 0)
    {
        return (RM_ERR_NO_FREE_MEM);
    }

    DBG_VAL_PTR(pDev->DBfbInstBitmap);
#if DEBUG
    if (Size == 0)    
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Zero size in fbAllocInstMem!\n\r");
        DBG_BREAKPOINT();
    }
#endif
    *Instance = 0;
    for (i = 0; i < pDev->Pram.FreeInstMax; i++)
    {
        if (!pDev->DBfbInstBitmap[i >> 3])
        {
            i += 7;
        }
        else if (pDev->DBfbInstBitmap[i >> 3] & BIT(i & 7))
        {
            //
            // Search for string of 1's the length of the block.
            //
            j = 0;
            do
            {
                if ((++i >= pDev->Pram.FreeInstMax) || (++j >= Size))
                    break;
            }
            while (pDev->DBfbInstBitmap[i >> 3] & BIT(i & 7));
            if (j == Size)
            {
                //
                // Score, a block was found.
                //
                *Instance = i - j + pDev->Pram.FreeInstBase;
                while (j--)
                {
                    //
                    // Mark instance memory as allocated.
                    //
                    i--;
                    pDev->DBfbInstBitmap[i >> 3] &= ~BIT(i & 7);
                }
#if defined(NTRM) && defined(DBG)
				osLogInstMemAlloc(*Instance, Size, 0);
#endif
                return (RM_OK);
            }
        }
    }
    return (RM_ERR_NO_FREE_MEM);
}
RM_STATUS fbFreeInstMem
(
    PHWINFO pDev,
    U032    Instance,
    U032    Size
)
{
#if defined(NTRM) && defined(DBG)
	U032 actualInstance = Instance;
	U032 actualSize = Size;
#endif // DEBUG && NTRM

    //
    // If instance is already set to 0, then it has already been freed.  This can
    // happen in some cases when a mode switch is happening while MIDI is playing
    // using the timer alarm notifies.  Ignoring this case can potentially cause a
    // protection fault, so be careful.
    //
    if (!Instance)
        return (RM_OK);
    Instance -= pDev->Pram.FreeInstBase;
#if DEBUG
    if (Instance > pDev->Pram.FreeInstMax)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ERROR - Freed instance out of range!\n\r");
        DBG_BREAKPOINT();
    }
#endif // DEBUG
    while (Size--)
    {
        //
        // Mark instance memory as free.
        //
        pDev->DBfbInstBitmap[Instance >> 3] |= BIT(Instance & 7);
        Instance++;
    }
#if defined(NTRM) && defined(DBG)
	osUnlogInstMemAlloc(actualInstance, actualSize);
#endif // DEBUG

    return (RM_OK);
}
