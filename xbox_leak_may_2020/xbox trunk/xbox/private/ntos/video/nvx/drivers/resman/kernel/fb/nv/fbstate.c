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

/************************* Framebuffer Module ******************************\
*                                                                           *
* Module: FBSTATE.C                                                         *
*   The Framebuffer state is changed in this module.                        *
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
#include <os.h>
#include <nvhw.h>
#include <fb.h>
#include <heap.h>
#include <vga.h>


//
// Save registers for framebuffer.
//
// U032 fbSave[2];

//
// New routine to point to location of cursor in instance memory and enable it.
VOID fbEnableCursor(VOID);

// This should already be setup somewhere else
#define RM_GLOBAL_BASE_PATH    "SOFTWARE\\NVIDIA Corporation\\Global\\System"

//
// Here's a definition of how these globals are being used:
//
//        FreeInstSize  - size in bytes of allocatable inst memory.
//                        Based on BIOS_IMAGE_PAD and the allocations for RAMHT, RAMFC and RAMRO.
//
//
//        FreeInstMax   - size in paragraphs of allocatable inst memory.
//                        fbInstBitmap is sized from this value and scanning for free
//                        instance memory uses this as the max available. This value
//                        is FreeInstBase + FreeInstSize in paragraphs.
//
//        FreeInstBase  - base where instance memory allocations begin.
//                        The base has already skipped beyond the BIOS_IMAGE_PAD
//                        and the allocations for RAMHT, RAMFC and RAMRO.
//
//        TotalInstSize - size in bytes of all of instance memory.
//                        This includes:
//                              allocatable instance memory
//                              RAMHT + RAMFC + RAMRO
//                              BIOS_PAD_IMAGE
//

//
// Change Frambuffer hardware state.
//
RM_STATUS stateFb
(
    PHWINFO pDev,
    U032    msg
)
{
    RM_STATUS status;
    S032      size;
    U032      entry;
    int       i;

    status = RM_OK;

    //
    // Deal with state change.
    //
    switch (msg)
    {
        case STATE_UPDATE:
            if (pDev->Framebuffer.UpdateFlags & UPDATE_HWINFO_BUFFER_PARAMS)
            {
                //
                // Validate and calculate new framebuffer memory requirements.
                //
                fbCheckConfig(pDev);
            }
            if (pDev->Framebuffer.UpdateFlags & UPDATE_HWINFO_DISPLAY_PARAMS)
            {
                //
                // Validate and calculate new framebuffer memory requirements.
                //
                fbCheckConfig(pDev);

                //
                // This usage flag will be cleared by the dac state routine
                // after the display is updated
                //
                // pDev->Framebuffer.UpdateFlags &= ~UPDATE_HWINFO_DISPLAY_PARAMS;
            }
            break;
        case STATE_LOAD:

            //
            // Check for any os-specific fb overrides.  These overrides must be known now in case
            // we need to adjust how our framebuffer is aligned or tiled.
            //
			osStateFbEx(pDev);			// check for override on FilterEnable

            //
            // Verify framebuffer configuration.
            //
            fbCheckConfig(pDev);

            //
            // Call into HAL to finish setup of FB state.
            //
            nvHalFbControl(pDev, FB_CONTROL_LOAD);

            // Check if we need to create the heap
            if (pDev->Framebuffer.HeapHandle == NULL)
            {
                //
                // Allocate framebuffer heap.  All memory must be allocated from here to keep the world
                // consistent (N.B. the heap size has been reduced by the amount of instance memory).
                //
                if ((status = heapCreate(pDev,
                                         (U008 *)fbAddr,
                                         pDev->Framebuffer.HalInfo.RamSize - pDev->Pram.HalInfo.TotalInstSize,
                                         pDev->Framebuffer.HalInfo.BankCount,
                                         (PHEAP *)&(pDev->Framebuffer.HeapHandle))) != RM_OK)
                    return (status);
            }

            // Check if we need to allocate the primary surface
            // We don't need to if we are in 4 bpp (VGA.DRV is running).
            if ((pDev->Framebuffer.PrimarySurfaceAlloced == FALSE) && (pDev->Dac.HalInfo.Depth != 4))
            {
                U032 offset;

                // Allocate the primary surface
                nvHalFbGetSurfacePitch(pDev, pDev->Framebuffer.HalInfo.HorizDisplayWidth, pDev->Dac.HalInfo.Depth, &pDev->Dac.DisplayPitch);
                size = pDev->Dac.DisplayPitch * pDev->Framebuffer.HalInfo.VertDisplayWidth;

                // heapAlloc fills in pDev->Framebuffer.HalInfo.Start for MEM_TYPE_PRIMARY
                if ((status = heapAlloc(pDev,
                                        (PHEAP)pDev->Framebuffer.HeapHandle,
                                        'NVRM',
                                        (U032 *) &size,
                                        MEM_TYPE_PRIMARY,
                                        pDev->Framebuffer.HalInfo.VertDisplayWidth,
                                        &pDev->Dac.DisplayPitch,
                                        &offset)) != RM_OK)
                    return (status);
            }
            
            // 
            // For a tiled primary surface, we delay setting the tiled bit in the hardware
            // until after we've modeswitched out of VGA to prevent a garbled splash screen.
            // Just in case we haven't set it yet, do it now.
            //
            for (i=0; i<MAX_CRTCS; i++) {
                if (pDev->Framebuffer.HalInfo.PrimaryFbAllocInfo[i].hwResId != 0) {
                    heapFbSetAllocParameters(pDev, &pDev->Framebuffer.HalInfo.PrimaryFbAllocInfo[i]);
                }
            }

            //
            // Since we're no longer maintaining the cache across mode changes, just
            // wipe the cache clean.
            //
            for (entry=0; entry < MAX_CACHED_CURSORS; entry++)
            {
                pDev->Dac.CursorMaskCRC[entry] = 0;
            }
            pDev->Dac.CursorCached = 0;

            pDev->Framebuffer.UpdateFlags &= ~(UPDATE_HWINFO_BUFFER_PARAMS | UPDATE_HWINFO_DISPLAY_PARAMS);

            break;
        case STATE_UNLOAD:

            //
            // Turn off video.
            //
            // MUST BLANK MONITOR ... setting PFB_GREEN_0_VIDEO_DISABLED doesn't work.
            // The underlying routines (e.g. osSetVideoMode) need to do this before a
            // mode change.
            //
            // REG_WR_DRF_DEF(_PFB, _GREEN_0, _LEVEL, _VIDEO_DISABLED);
#if 0
            //
            // We're not calling heapDestroy as part of an state unload anymore.
            //           

#ifndef NTRM
            // Completely destroy the heap on mode switches. However, we don't want to destroy
            // the heap if we are going into or coming out of a full screen DOS box.
            if (!pDev->Vga.DOSBox)
#endif
            {
                // Free heap.
                heapDestroy(pDev, (PHEAP)pDev->Framebuffer.HeapHandle);
            }
#endif

            break;
        case STATE_INIT:
        {
            U032 data32, fb;

            //
            // Check the registry to see if we're not allowing pitch values to be rounded
            // up to what NV10 tiling requires (pitch must equal width for some old apps).
            // If so, we'll only tile a surface if the pitch is already aligned.
            //
            // We only want to not tile the lower resolutions with this flag, which means
            // just the 6x4 and 8x6 modes. But, we don't have width/height at all times
            // and checking against pitch has overlap with the other modes. So, we'll use
            // what we have at the time; width <= 800 or height <= 600 will be checked.
            //
            if (OS_READ_REGISTRY_DWORD(pDev, RM_GLOBAL_BASE_PATH, "NoAdjustedPitch", &data32) == RM_OK)
                pDev->Framebuffer.NoAdjustedPitch = 0x1;    // surface height check
            else
                pDev->Framebuffer.NoAdjustedPitch = 0x0;

            status = initFb(pDev);
            if (status != RM_OK)
            {
                return (RM_ERROR);
            }

            //
            // Reserve high block of fb mem as instance space.
            //
            for (fb = 0; fb < ((pDev->Pram.HalInfo.TotalInstSize - BIOS_IMAGE_PAD) / 4); fb++)
                REG_WR32(NV_PRAMIN_DATA032(fb) + BIOS_IMAGE_PAD, 0);

            //
            // Setup bitmap.
            //
            status = fbInitializeInstMemBitMap(pDev);
            if (status)
                return (status);

            //
            // Now that the instance bitmap is setup, we can safely
            // claim any instance memory required by the chip's engines.
            //
            status = allocHalEngineInstMem(pDev);
            if (status)
                return (status);
            break;
        }
        case STATE_DESTROY:
            //
            // Free up allocated memory.
            //
            osFreeMem(pDev->DBfbInstBitmap);

            //
            // Restore framebuffer registers.
            //
            nvHalFbControl(pDev, FB_CONTROL_DESTROY);
            break;
    }
    return (status);
}

//
// Verify that we have enough fbmem to support the requested
// video mode.
//
RM_STATUS fbCheckConfig
(
    PHWINFO pDev
)
{
    S032 size;
	U032 normalized_depth;

    //
    // Normalize our framebuffer depth
    //
    switch (pDev->Dac.HalInfo.Depth)
    {
        case 32:
        case 30:
        case 24:
            normalized_depth = 32;
            break;
        case 16:
        case 15:
            normalized_depth = 16;
            break;
        case 8:
        default:
            normalized_depth = 8;
            break;
    }

    //
    // Force video parameters to proper values.
    //
    // Calculate PRAM, Hash Table, and Instance memory sizes.
    //
    size = normalized_depth / 8
         * pDev->Framebuffer.HalInfo.HorizDisplayWidth * pDev->Framebuffer.HalInfo.VertDisplayWidth
         * pDev->Framebuffer.Count;

    //
    // Now calculate the available offscreen memory
    //
    if (size > (S032)pDev->Framebuffer.HalInfo.RamSize)
        size = 0;
    else
        size = pDev->Framebuffer.HalInfo.RamSize - size;

    if (size < (S032)pDev->Pram.HalInfo.TotalInstSize)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Error - not enough PRAM!!!!!\n\r");
        DBG_BREAKPOINT();
        return (RM_ERROR);
    }

    return (RM_OK);
}
