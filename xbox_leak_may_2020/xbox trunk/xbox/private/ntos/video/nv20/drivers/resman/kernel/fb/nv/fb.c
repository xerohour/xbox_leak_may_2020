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
#include <bios.h>
#include <fifo.h>
#include <dma.h>
#include <gr.h>
#include <fb.h>
#include <os.h>
#include <vga.h>
#include "nvhw.h"



//KDA buffer summary
// Synopsys: The KDA buffer is a data area in instance memory shared between
// the driver and the VBIOS.  First created March, 2001.
// The VBIOS is already mirrored in the lower 64K The buffer is set up by the VBIOS


//---------------------------------------------------------------------------
//
//  Instance allocation bitmap.
//
//---------------------------------------------------------------------------


//Attempt to take control of the entire KDA buffer via a signalling bit.
// Returns success if there is no buffer present, or if we successfully
// set our bit (which first entails checking that the VBIOS has not currently
// set its own access bit).
//We should not hold this semaphore for long -- there are separate cell-specific
// semaphores that can be taken and held indefinitely.
RM_STATUS fbKDATakeBufferSemaphore
(
    PHWINFO pDev,
    U032    timeout
)
{
#if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    U032 biosFlags, driverFlags, curLoc;

    if (!pDev->DBfbKDASharedMem) return RM_OK;

    curLoc = (INST_RD32(0, KDA_HARDWIRED_PTR_LOC)) >> 4;
    if (!curLoc) return RM_OK;

    do {
        biosFlags = INST_RD32(curLoc, KDA_HEADER_VBIOS_FLAGS_LOC);
        if (biosFlags & KDA_BUFFER_BUSY_MASK) {
            U032 stepdelay = (timeout < 100000)?timeout:100000;
            tmrDelay(pDev, stepdelay);
            timeout -= stepdelay;
        } else {
            driverFlags = INST_RD32(curLoc, KDA_HEADER_DRIVER_FLAGS_LOC);
            INST_WR32(curLoc, KDA_HEADER_DRIVER_FLAGS_LOC, driverFlags | KDA_BUFFER_BUSY_MASK);
            //Verify the VBIOS didn't sneak in during the window between our
            // last check and the set above.
            biosFlags = INST_RD32(curLoc, KDA_HEADER_VBIOS_FLAGS_LOC);
            if (biosFlags & KDA_BUFFER_BUSY_MASK)   //If it did, drop our own attempt to take the semaphore.
                INST_WR32(curLoc, KDA_HEADER_DRIVER_FLAGS_LOC, driverFlags);
            else                            //Otherwise we're all set, so clear the remaining time.
                timeout = 0;
        }
    } while (timeout);

    if (!((INST_RD32(curLoc, KDA_HEADER_DRIVER_FLAGS_LOC)) & KDA_BUFFER_BUSY_MASK))
        return RM_ERROR;
#endif  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    return RM_OK;
}

//Release our lock on the KDA buffer
RM_STATUS fbKDAReleaseBufferSemaphore
(
    PHWINFO pDev
)
{
#if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    U032 driverFlags, curLoc;

    if (!pDev->DBfbKDASharedMem) return RM_OK;

    curLoc = (INST_RD32(0, KDA_HARDWIRED_PTR_LOC)) >> 4;
    if (!curLoc) return RM_OK;

    driverFlags = INST_RD32(curLoc, KDA_HEADER_DRIVER_FLAGS_LOC);
    INST_WR32(curLoc, KDA_HEADER_DRIVER_FLAGS_LOC, driverFlags & ~KDA_BUFFER_BUSY_MASK);
#endif  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    return RM_OK;
}

//Look-up function to search for the next occurence of a specified cell type
// within the KDA buffer.
//The prevMatch parameter is set to 0 to find the first occurence,
// and to the offset of the last found cell to find the next.
#if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
RM_STATUS fbKDAFindNextCellOffset
(
    PHWINFO pDev,
    U032 cellType,
    U032 *prevMatch
)
{
    U032 curLoc, bufferSize, nextCellLoc;
    curLoc = (INST_RD32(0, KDA_HARDWIRED_PTR_LOC)) >> 4;
    bufferSize = INST_RD32(curLoc, KDA_HEADER_BUFFER_SIZE_LOC) >> 4;

    nextCellLoc = KDA_BUFFER_CELL_START;
    while (INST_RD32(curLoc, nextCellLoc + KDA_CELL_SIZE_LOC) && (nextCellLoc < bufferSize)) {
        if (nextCellLoc > *prevMatch) {
            if (INST_RD32(curLoc, nextCellLoc + KDA_CELL_TYPE_LOC) == cellType) {
                *prevMatch = nextCellLoc;
                return RM_OK;
            }
        }
        nextCellLoc += INST_RD32(curLoc, nextCellLoc + KDA_CELL_SIZE_LOC);
    }

    return RM_ERROR;
}

//Find the next occurence within the KDA buffer of a cursor cell.
//The prevMatch parameter is set to 0 to find the first occurence,
// and to the offset of the last found cell to find the next.
RM_STATUS fbKDAFindCursorCellOffset
(
    PHWINFO pDev,
    U032 head,
    U032 *prevMatch
)
{
    RM_STATUS rmStatus;
    U032 curLoc;

    curLoc = (INST_RD32(0, KDA_HARDWIRED_PTR_LOC)) >> 4;

    //Find the cell we are interested in.
    while ((rmStatus = fbKDAFindNextCellOffset(pDev, KDA_CELL_TYPE_HW_ICON, prevMatch)) == RM_OK) {
        if (INST_RD32(curLoc, *prevMatch + KDA_CELL_HWICON_HEAD_LOC) == head)
            break;
    }

    if (rmStatus == RM_ERROR) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "    NVRM (fbKDAFindCursorCellOffset): Unable to find cell match.\r\n");
    }

    return rmStatus;
}
#endif  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)


//Take the semaphore for a cursor cell
// Returns success if we successfully set our bit (which first entails
// checking that the VBIOS has not currently set its own access bit).
// During HW icon emulation on NV11, the VBIOS may hold this semaphore
// indefinitely.
RM_STATUS fbKDATakeCursorCellSemaphore
(
    PHWINFO pDev,
    U032    cellOffset,
    U032    timeout
)
{
#if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    U032 curLoc;
    U032 biosFlags, driverFlags;

    curLoc = (INST_RD32(0, KDA_HARDWIRED_PTR_LOC)) >> 4;

    //Take the semaphore for this cell
    do {
        biosFlags = INST_RD32(curLoc, cellOffset + KDA_CELL_HWICON_VBIOS_FLAGS_LOC);  //LPL: need to adjust by cell type!
        if (biosFlags & KDA_CELL_HWICON_BUSY_MASK) {
            U032 stepdelay = (timeout < 100000)?timeout:100000;
            tmrDelay(pDev, stepdelay);
            timeout -= stepdelay;
        } else {
            driverFlags = INST_RD32(curLoc, cellOffset + KDA_CELL_HWICON_DRIVER_FLAGS_LOC);
            INST_WR32(curLoc, cellOffset + KDA_CELL_HWICON_DRIVER_FLAGS_LOC, driverFlags | KDA_CELL_HWICON_BUSY_MASK);
            //Verify the VBIOS didn't sneak in during the window between our
            // last check and the set above.
            biosFlags = INST_RD32(curLoc, cellOffset + KDA_CELL_HWICON_VBIOS_FLAGS_LOC);
            if (biosFlags & KDA_CELL_HWICON_BUSY_MASK)   //If it did, drop our own attempt to take the semaphore.
                INST_WR32(curLoc, cellOffset + KDA_CELL_HWICON_DRIVER_FLAGS_LOC, driverFlags);
            else                            //Otherwise we're all set, so clear the remaining time.
                timeout = 0;
        }
    } while (timeout);

    if (!((INST_RD32(curLoc, KDA_CELL_HWICON_DRIVER_FLAGS_LOC)) & KDA_CELL_HWICON_BUSY_MASK)) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "    NVRM: Unable to take cell semaphore.\r\n");
        return RM_ERROR;
    }
#endif  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    return RM_OK;
}


//Release the semaphore for a cursor cell
RM_STATUS fbKDAReleaseCursorCellSemaphore
(
    PHWINFO pDev,
    U032    cellOffset
)
{
#if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    U032 curLoc;
    U032 driverFlags;

    curLoc = (INST_RD32(0, KDA_HARDWIRED_PTR_LOC)) >> 4;

    driverFlags = INST_RD32(curLoc, cellOffset + KDA_CELL_HWICON_DRIVER_FLAGS_LOC);
    INST_WR32(curLoc, cellOffset + KDA_CELL_HWICON_DRIVER_FLAGS_LOC, driverFlags & KDA_CELL_HWICON_BUSY_MASK);

    return RM_OK;
#else  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    return RM_OK;
#endif  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
}

RM_STATUS fbInitializeInstMemBitMap
(
    PHWINFO pDev
)
{
    RM_STATUS status;
    U008      *pbyte;
    U032      size;
#if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    U032      rEAX, rEBX, rECX, rEDX;
    U032      KDASizeBytes;
#else  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    U032      entry;
#endif  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)

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

#if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    //Allocate space for the KDA buffer and copy the buffer the VBIOS was using
    // to the new location.
    //LPL to add: make sure the new buffer is not > old but overlapping -- the copy
    // couldn't be done non-destructively if so...
    //Verify that BIOS supports use of an instance memory buffer.
    rEAX = 0x4F14;
    rEBX = 0x0006;
    rECX = 0;
    rEDX = 0;
    status = osCallVideoBIOS(pDev, &rEAX, &rEBX, &rECX, &rEDX, NULL);

    pDev->DBfbKDASharedMem = 0; //Special 'it's not there' value.
    pDev->DBfbKDACursorOverride = FALSE;

    if ((status == RM_OK) && (INST_RD32(0, KDA_HARDWIRED_PTR_LOC) >> 4)) {   //If the BIOS supports this
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: BIOS supports hardware icons in instance memory.\r\n");
        //Allocate space for the new buffer
        status = fbAllocInstMemAlign(pDev, &pDev->DBfbKDASharedMem, KDA_MEM_SIZE >> 4, 0x800 >> 4);
        if (status == RM_OK) {
            //Take ownership of the old buffer
            status = fbKDATakeBufferSemaphore(pDev, 10000000);
            if (status == RM_OK) {
                U032 origBufLoc, temp;
                S032 i;
                origBufLoc = (INST_RD32(0, KDA_HARDWIRED_PTR_LOC)) >> 4;
                KDASizeBytes = INST_RD32(origBufLoc, KDA_HEADER_BUFFER_SIZE_LOC);
                if (KDASizeBytes == 0) {
                    return RM_ERROR;
                }

                // Copy the whole buffer.  Since there is no guarantee the buffer the BIOS
                // was using originally and the one we allocate won't overlap, we have to
                // be careful not to accidentally overwrite information during the copy.
                // We copy up or down as needed, and the copy is broken up into two parts:
                // up to and including the location holding the semaphore flag (after which
                // the buffer pointer is overwritten to point to the new location) and
                // everything else.
                if (origBufLoc > pDev->DBfbKDASharedMem) {
                    for (i = 0; i <= KDA_HEADER_DRIVER_FLAGS_LOC; i+=sizeof(U032)) {
                        temp = INST_RD32(origBufLoc, i);
                        INST_WR32(pDev->DBfbKDASharedMem, i, temp);
                    }
                    //Reassign buffer pointer to point to newly allocated region (stored as offset in bytes)
                    INST_WR32(0, KDA_HARDWIRED_PTR_LOC, pDev->DBfbKDASharedMem<<4);
                    for (i = KDA_HEADER_DRIVER_FLAGS_LOC; i < (S032) KDASizeBytes; i+=sizeof(U032)) {
                        temp = INST_RD32(origBufLoc, i);
                        INST_WR32(pDev->DBfbKDASharedMem, i, temp);
                    }
                } else {
                    for (i = (S032) KDASizeBytes - sizeof(U032); i >= KDA_HEADER_DRIVER_FLAGS_LOC; i-=sizeof(U032)) {
                        temp = INST_RD32(origBufLoc, i);
                        INST_WR32(pDev->DBfbKDASharedMem, i, temp);
                    }
                    //Reassign buffer pointer to point to newly allocated region (stored as offset in bytes)
                    INST_WR32(0, KDA_HARDWIRED_PTR_LOC, pDev->DBfbKDASharedMem<<4);
                    for (i = KDA_HEADER_DRIVER_FLAGS_LOC; i >= 0; i-=sizeof(U032)) {
                        temp = INST_RD32(origBufLoc, i);
                        INST_WR32(pDev->DBfbKDASharedMem, i, temp);
                    }
                }

                /*
                //1. Copy over the driver flags so the new location also indicates we
                // own the semaphore.
                temp = INST_RD32(origBufLoc, KDA_HEADER_DRIVER_FLAGS_LOC);
                INST_WR32(pDev->DBfbKDASharedMem, KDA_HEADER_DRIVER_FLAGS_LOC, temp);
                //2. Overwrite buffer pointer to point to newly allocated region (stored as offset in bytes)
                INST_WR32(0, KDA_HARDWIRED_PTR_LOC, pDev->DBfbKDASharedMem<<4);
                //3. Copy the whole buffer.
                for (i = 0; i < KDA_MEM_SIZE; i+=sizeof(U032)) {
                    temp = INST_RD32(origBufLoc, i);
                     INST_WR32(pDev->DBfbKDASharedMem, i, temp);
                }
                */
                fbKDAReleaseBufferSemaphore(pDev);
            } else {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "    NVRM: FB KDA allocation error!\r\n");
                fbFreeInstMem(pDev, pDev->DBfbKDASharedMem, KDA_MEM_SIZE>>4);
                INST_WR32(0, KDA_HARDWIRED_PTR_LOC, 0); //Note we only write this if the BIOS supports the hw icon feature
                pDev->DBfbKDASharedMem = 0; //Special 'it's not there' value.
            }
        }
    } else {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: BIOS does not support hardware icons in instance memory.\r\n");
    }
#else  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)
    //LPL: Legacy allocs, not used anymore.
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
#endif  //if defined(WIN32) && defined (KDA_BUFFER_ENABLED)

    return RM_OK;
} // end of fbInitializeInstMemBitMap 

RM_STATUS fbClearInstMem
(
    PHWINFO pDev
)
{
    U032 fb;
#if defined(KDA_BUFFER_ENABLED)

    if (pDev->DBfbKDASharedMem)
    {
        U032 KDAStart, KDASizeBytes, KDAEnd;
        // If a KDA exists, make sure not to clear it.

        KDAStart     = pDev->DBfbKDASharedMem << 4;
        KDASizeBytes = INST_RD32(pDev->DBfbKDASharedMem, KDA_HEADER_BUFFER_SIZE_LOC);
        KDAEnd       = KDAStart + KDASizeBytes;

        if (KDASizeBytes == 0)
        {
            return RM_ERROR;
        }

        for (fb = 0; fb < ((KDAStart - BIOS_IMAGE_PAD) / 4); fb++)
        {
            REG_WR32(NV_PRAMIN_DATA032(fb) + BIOS_IMAGE_PAD, 0);
        }

        for (fb = ((KDAEnd - BIOS_IMAGE_PAD) / 4); fb < ((pDev->Pram.HalInfo.TotalInstSize - BIOS_IMAGE_PAD) / 4); fb++)
        {
            REG_WR32(NV_PRAMIN_DATA032(fb) + BIOS_IMAGE_PAD, 0);
        }
    }
    else
#endif // defined(KDA_BUFFER_ENABLED)
    {
        for (fb = 0; fb < ((pDev->Pram.HalInfo.TotalInstSize - BIOS_IMAGE_PAD) / 4); fb++)
            REG_WR32(NV_PRAMIN_DATA032(fb) + BIOS_IMAGE_PAD, 0);
    }

    return RM_OK;
} // end of fbClearInstMem

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

    osEnterCriticalCode(pDev);

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
                osExitCriticalCode(pDev);
#if defined(NTRM) && defined(DBG)
                osLogInstMemAlloc(*Instance, Size, Align);
#endif
                return (RM_OK);
            }
        }
    }
    osExitCriticalCode(pDev);
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

    switch(pDev->Power.State)
    {
        case MC_POWER_LEVEL_0:
        case MC_POWER_LEVEL_5:
        case MC_POWER_LEVEL_6:
            break;
        default:
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

    osEnterCriticalCode(pDev);

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
                osExitCriticalCode(pDev);
#if defined(NTRM) && defined(DBG)
                osLogInstMemAlloc(*Instance, Size, 0);
#endif
                return (RM_OK);
            }
        }
    }
    osExitCriticalCode(pDev);
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
    if (!pDev->DBfbInstBitmap)
        return (RM_OK);

    osEnterCriticalCode(pDev);

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
    osExitCriticalCode(pDev);

#if defined(NTRM) && defined(DBG)
    osUnlogInstMemAlloc(actualInstance, actualSize);
#endif // DEBUG

    return (RM_OK);
}
