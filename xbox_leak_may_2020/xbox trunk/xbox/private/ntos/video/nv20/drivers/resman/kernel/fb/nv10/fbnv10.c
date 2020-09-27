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

/************************ Chip Specific HAL FB Routines ********************\
*                                                                           *
* Module: FBNV10.c                                                          *
*   The NV10 specific HAL FB routines reside in this file.                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv10_ref.h>
#include <nvrm.h>
#include <nv10_hal.h>
#include <heap.h>
#include "nvhw.h"
#include "nvhalcpp.h"
#include <bios.h>
#include "cr11help.h"

// forwards
RM_STATUS nvHalFbControl_NV10(VOID *);
RM_STATUS nvHalFbAlloc_NV10(VOID *);
RM_STATUS nvHalFbFree_NV10(VOID *);
RM_STATUS nvHalFbSetAllocParameters_NV10(VOID *);
RM_STATUS nvHalFbGetSurfacePitch_NV10(VOID *);
RM_STATUS nvHalFbGetSurfaceDimensions_NV10(VOID *);
RM_STATUS nvHalFbLoadOverride_NV10(VOID *);

// statics
static VOID nvHalFbRoundTiledPitchSize_NV10(U032 *, U032, U032 *);
static U032 nvHalFbRealMemSize(U032, PHWREG);
static U032 calcChkSum(PHWREG, U032);
static void restoreFbSave(U032 *, PHWREG, U032);

// NV10 expects a 1024 byte alignment
#define FC_MIN_ALIGN_SHIFT      10

//
// nvHalFbControl
//
RM_STATUS
nvHalFbControl_NV10(VOID *arg)
{
    PFBCONTROLARG_000 pFbControlArg = (PFBCONTROLARG_000)arg;
    PMCHALINFO pMcHalInfo;
    PHALHWINFO pHalHwInfo;
    PFBHALINFO pFbHalInfo;
    PPRAMHALINFO pPramHalInfo;
    PFBHALINFO_NV10 pFbHalPvtInfo;
    U032 i;
    U032 test_mem;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbControlArg->id != FB_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbControlArg->pHalHwInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;
    pFbHalInfo = pHalHwInfo->pFbHalInfo;
    pPramHalInfo = pHalHwInfo->pPramHalInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV10)pHalHwInfo->pFbHalPvtInfo;

    switch (pFbControlArg->cmd)
    {
        case FB_CONTROL_INIT:
        {
            U032 extbank, intbank;

            // set ram size
            pFbHalInfo->RamSize = REG_RD32(NV_PFB_CSTATUS);
            pFbHalInfo->RamSizeMb = pFbHalInfo->RamSize / 0x100000;

            //Setup integrated graphics frame buffer
            if ( IsNV1A_NV10(pHalHwInfo->pMcHalInfo) )
            {
                U032    uLastTLBEntry;
                U032    uLastTag;
                U032    uLastAddr;

                U016    wNBVendorID;
                U016    wNBDeviceID;
                VOID    *uNBHandle;
                PHWREG  fbTestFBBase;
                U032    uTestFBRamSize;
                U032    uDIMMConfig;
                U032    uMinDIMMColumnBits;

                uNBHandle = osPciInitHandle(0, 0, 2, &wNBVendorID, &wNBDeviceID);
                if( !uNBHandle )
                {
                    DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_ERRORS, "NVRM: Unable to touch integrated graphics\n"));
                    DBG_BREAKPOINT();
                    return (RM_ERROR);
                }

                DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "NVRM: North Bridge Vendor=%04x\n", wNBVendorID));
                DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "      North Bridge Device=%04x\n", wNBDeviceID));

                //Determine the size of the current frame buffer pre-allocated by the system BIOS
                //Crush11 is a little bit odd in that the instance memory is hard wired to the 31st meg,
                //regardless of the actual frame buffer size.

                //Walk the frame buffer in 1M hops, looking for non-existant memory.
                fbTestFBBase = pHalHwInfo->fbBaseAddr;
                uTestFBRamSize = 32;                    //Look through the whole 32M region
                pFbHalInfo->RamSizeMb = 0;              //The first contiguous chunk is the frame buffer

                while(uTestFBRamSize)
                {
                    test_mem = nvHalFbRealMemSize(uTestFBRamSize, fbTestFBBase);
                    if( test_mem )
                    {
                        DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "NVRM: Crush11 FB block@%08x (%dM)\n", fbTestFBBase, test_mem));
                        uTestFBRamSize -= test_mem;
                        if( pFbHalInfo->RamSizeMb==0 )
                        {
                            pFbHalInfo->RamSizeMb = test_mem;
                        }
                        fbTestFBBase += test_mem*1024*1024/sizeof(HWREG);
                    }
                    else
                    {
                        //Bump to the next boundary and try again -- we may be sparsely allocated
                        uTestFBRamSize -= 1;
                        fbTestFBBase += 1*1024*1024/sizeof(HWREG);
                    }
                }

                //Now we know the size of the current frame buffer
                //Try reading CR_XL_FB_TLB_AD to find the last value written.  This should tell us the how many TLB
                //entries have been loaded as well as give us a hint of the starting physical address.
                //Caveat: This only works if no one has been mucking around with the page translation.
                uLastTLBEntry = osPciReadDword(uNBHandle, CR_XL_FB_TLB_AD & 0xFF);
                uLastTag  = CR_DRF_VAL( _XL, _FB_TLB_AD, _TAG, uLastTLBEntry);
                uLastAddr = CR_DRF_VAL( _XL, _FB_TLB_AD, _ADDR, uLastTLBEntry);

                DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "NVRM: CR_XL_FB_TLB_AD  =%08x\n", uLastTLBEntry));
                DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "                  _TAG =%03x\n", uLastTag));
                DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "                  _ADDR=%05x\n", uLastTLBEntry));

                pFbHalInfo->RamSize = pFbHalInfo->RamSizeMb*1024*1024;
                pFbHalInfo->ExtBankCount = 1;
                pFbHalInfo->BankCount = 1;
                pFbHalInfo->RamType = BUFFER_SDRAM;

                // set minimum alignment for tiled surfaces

                // For integrated graphics, we need to check the DIMM configuration to calculate the 
                // tile alignment restriction based on the number of column bits
                uMinDIMMColumnBits = 0xffffffff;
                for( i=0; i<CR_XL_DIMM_CFG__SIZE_1; i++ )
                {
                    uDIMMConfig = osPciReadDword(uNBHandle, CR_XL_DIMM_CFG(i) & 0xFF);
                    DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "NVRM: _DIMM_CFG(%d)=%08x\n", i, uDIMMConfig));

                    // Check if the DIMM is present
                    if( uDIMMConfig & CR_DRF_DEF( _XL, _DIMM_CFG, _PRESENT, _TRUE ) )
                    {
                        DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "      _PRESENT     =%x\n", CR_DRF_VAL( _XL, _DIMM_CFG, _PRESENT, uDIMMConfig)));
                        DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "      _SIZE        =%dM\n", 16*CR_DRF_VAL( _XL, _DIMM_CFG, _SIZE, uDIMMConfig)));
                        DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "      _NUM_ROW_BITS=%d\n", 11+CR_DRF_VAL( _XL, _DIMM_CFG, _NUM_ROW_BITS, uDIMMConfig)));
                        DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "      _NUM_COL_BITS=%d\n", 8+CR_DRF_VAL( _XL, _DIMM_CFG, _NUM_COL_BITS, uDIMMConfig)));
                        DBG_PRINTF(( DBG_MODULE_GLOBAL, DEBUGLEVEL_TRACEINFO, "      _NUM_EXT_BANKS=%d\n", 1+CR_DRF_VAL( _XL, _DIMM_CFG, _NUM_EXT_BANKS, uDIMMConfig)));

                        //Find the smallest column configuration -- that's the case we need to account for
                        uMinDIMMColumnBits = min( CR_DRF_VAL(  _XL, _DIMM_CFG, _NUM_COL_BITS, uDIMMConfig ), uMinDIMMColumnBits );
                    }
                }

                // Now figure out the alignment.  The tiling registers have a minimum granularity of 16K.
                // But the column bits might require 32K (10 bits) or 64K (11 bits).
                pFbHalPvtInfo->uTileAlign = max( DRF_NUM( _PGRAPH, _TILE, _ADR,1 ), 32 << (8+uMinDIMMColumnBits))-1;

            }
            else
            {

                // set bank count
                extbank = REG_RD_DRF(_PFB, _CFG, _EXTBANK);
                intbank = REG_RD_DRF(_PFB, _CFG, _BANK);

                // record number of external banks
                if (extbank == NV_PFB_CFG_EXTBANK_1)
                    pFbHalInfo->ExtBankCount = 2;
                else
                    pFbHalInfo->ExtBankCount = 1;

                // set heap manager bank count
                if (extbank && intbank)
                    pFbHalInfo->BankCount = 4;
                else if ((extbank && !intbank) || (!extbank && intbank))
                    pFbHalInfo->BankCount = 2;
                else
                    pFbHalInfo->BankCount = 1;

                // save off chip-dependent fb state
                pFbHalPvtInfo->fbSave[0] = REG_RD32(NV_PFB_CFG);

                // set ram type
                if (REG_RD_DRF(_PFB, _CFG, _TYPE) == NV_PFB_CFG_TYPE_SDR)
                    pFbHalInfo->RamType = BUFFER_SDRAM;
                else
                    pFbHalInfo->RamType = BUFFER_DDRAM;

                // set minimum alignment for tiled surfaces
                pFbHalPvtInfo->uTileAlign = DRF_NUM( _PGRAPH, _TILE, _ADR,1 )-1;
            }


            //CAH:Actually step through memory to verify we have as much there as the BIOS is 
            //telling us. On some "gray" market boards they may be strapped to report 
            //double the actual physical memory.This test will step through the framebuffer
            //to determine if it's actually there.
            test_mem = nvHalFbRealMemSize(pFbHalInfo->RamSizeMb, pHalHwInfo->fbBaseAddr);

            //
            // Make sure the NV11 and NV15 are not specifying more than 64MB of memory
            //
            if ( IsNV11_NV10(pMcHalInfo) || IsNV15_NV10(pMcHalInfo) )
            {
                if (test_mem > 64) 
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NV11 or NV15 with more than 64MB detected\n\r");
                    return (RM_ERROR);
                }
            }

            if (pFbHalInfo->RamSizeMb != test_mem) 
            {

                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: RamSize incorrectly read from BIOS\n\r");
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Resetting RamSize from Memory Test\n\r");
                return (RM_ERROR);
            }

            //
            // Set instance space size.  Add in whatever os-dependent
            // (i.e. BIOS) space is needed as well.
            //
            // For NV10, this is 128K allocatable.
            //
#ifdef KDA_BUFFER_ENABLED
            // LPL: test purposes, bump to 256K (back to 128 now)
            pPramHalInfo->TotalInstSize = 0x20000 + pPramHalInfo->InstPadSize;
#else
            pPramHalInfo->TotalInstSize = 0x20000 + pPramHalInfo->InstPadSize;
#endif
            //
            // Configure hash table.
            //
            pFbHalPvtInfo->hashTableSize = 0x4000;
            pPramHalInfo->HashDepth = pFbHalPvtInfo->hashTableSize / sizeof (HASH_TABLE_ENTRY);
            pPramHalInfo->HashSearch = 128;
            pFbHalPvtInfo->hashTableAddr = DEVICE_BASE(NV_PRAMIN) + pPramHalInfo->InstPadSize;
            REG_WR32(NV_PFIFO_RAMHT, (DRF_DEF(_PFIFO, _RAMHT, _SIZE, _16K) |
                                      DRF_NUM(_PFIFO, _RAMHT, _BASE_ADDRESS, (pFbHalPvtInfo->hashTableAddr - DEVICE_BASE(NV_PRAMIN)) >> 12) |
                                      DRF_DEF(_PFIFO, _RAMHT, _SEARCH, _128)));

            //
            // Configure fifo context area.
            //
            pFbHalPvtInfo->fifoContextSize = NUM_FIFOS_NV10 * FC_SIZE_NV10;
            pFbHalPvtInfo->fifoContextAddr = DEVICE_BASE(NV_PRAMIN) + pPramHalInfo->InstPadSize + pFbHalPvtInfo->hashTableSize;
            REG_WR_DRF_NUM(_PFIFO, _RAMFC, _BASE_ADDRESS, ((pFbHalPvtInfo->fifoContextAddr - DEVICE_BASE(NV_PRAMIN)) >> FC_MIN_ALIGN_SHIFT));

            //
            // Configure runout area.
            //
            pFbHalPvtInfo->fifoRunoutSize = 512;
            pFbHalPvtInfo->fifoRunoutAddr = DEVICE_BASE(NV_PRAMIN) + pPramHalInfo->InstPadSize +
                pFbHalPvtInfo->hashTableSize + pFbHalPvtInfo->fifoContextSize;
            pFbHalPvtInfo->fifoRunoutMask = pFbHalPvtInfo->fifoRunoutSize - 1;
            REG_WR32(NV_PFIFO_RAMRO, (DRF_DEF(_PFIFO, _RAMRO, _SIZE, _512) |
                                      DRF_NUM(_PFIFO, _RAMRO, _BASE_ADDRESS, (pFbHalPvtInfo->fifoRunoutAddr - DEVICE_BASE(NV_PRAMIN)) >> 9)));

            //
            // Sum of all allocations plus a little slop
            //
            pPramHalInfo->CurrentSize  = pFbHalPvtInfo->hashTableSize + pFbHalPvtInfo->fifoContextSize + pFbHalPvtInfo->fifoRunoutSize + 512;

            break;
        }
        case FB_CONTROL_LOAD:
            break;
        case FB_CONTROL_UNLOAD:
            break;
        case FB_CONTROL_UPDATE:
            break;
        case FB_CONTROL_DESTROY:
            REG_WR32(NV_PFB_CFG, pFbHalPvtInfo->fbSave[0]);
            break;
        case FB_CONTROL_INSTWR_DISABLE:
            // disable writes by enabling  protection
            switch (pPramHalInfo->TotalInstSize)
            {
                case 0x30000:
                    FLD_WR_DRF_DEF(_PFB, _NVM, _LIMIT, _192K);
                    break;
                case 0x20000:
                default:
                    FLD_WR_DRF_DEF(_PFB, _NVM, _LIMIT, _128K);
                    break;
            }
            FLD_WR_DRF_DEF(_PFB, _NVM, _MODE, _ENABLE);
            break;
        case FB_CONTROL_INSTWR_ENABLE:
            // enable writes by disabling write protection
            FLD_WR_DRF_DEF(_PFB, _NVM, _MODE, _DISABLE);
            break;
        case FB_CONTROL_TILING_DISABLE:
            //
            // Check for integrated graphics -- we need to setup tiling through the north bridge
            //
            if( IsNV1A_NV10(pHalHwInfo->pMcHalInfo) )
            {
                Nv1AInvalidateTile( pHalHwInfo, 0, 8 );
            }
            else
            {
                HAL_GR_IDLE(pHalHwInfo);
                for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
                {
                    REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
                    REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);
                }
            }
            break;
        case FB_CONTROL_TILING_ENABLE:
            // this is treated as an error on nv10 (???)
            return (RM_ERR_BAD_ARGUMENT);
        case FB_CONTROL_TILING_CONFIGURE:
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}

//
// nvHalFbGetSurfacePitch
//
// Given a width and bits per pixel, return the surface pitch
//
RM_STATUS
nvHalFbGetSurfacePitch_NV10(VOID *arg)
{
    PFBGETSURFACEPITCHARG_000 pGetSurfacePitchArg = (PFBGETSURFACEPITCHARG_000)arg;
    
    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pGetSurfacePitchArg->id != FB_GET_SURFACE_PITCH_000)
        return (RM_ERR_VERSION_MISMATCH);

    // surfaces are (at least) 64byte aligned
    // Watch out for bpp==15: add 1 here to force 15 to 16 for the shift
    pGetSurfacePitchArg->pitch = ((pGetSurfacePitchArg->width * ((pGetSurfacePitchArg->bpp+1) >> 3)) + 0x3F) & ~0x3F;

    if (pGetSurfacePitchArg->flags & FB_GET_SURFACE_PITCH_NO_ADJUST)
        return RM_OK;

    if (!(pGetSurfacePitchArg->flags & FB_GET_SURFACE_PITCH_NO_ADJUST))
    {
        nvHalFbRoundTiledPitchSize_NV10(&pGetSurfacePitchArg->pitch, NULL, NULL);
    }
    return RM_OK;
}

//
// nvHalFbGetSurfaceDimensions
//
// Given a width/height and bits per pixel, return the surface pitch and size
//
RM_STATUS
nvHalFbGetSurfaceDimensions_NV10(VOID *arg)
{
    PFBGETSURFACEDIMENSIONSARG_000 pGetSurfaceDimensionsArg = (PFBGETSURFACEDIMENSIONSARG_000)arg;
    
    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pGetSurfaceDimensionsArg->id != FB_GET_SURFACE_DIMENSIONS_000)
        return (RM_ERR_VERSION_MISMATCH);

    // surfaces are (at least) 64byte aligned
    // Watch out for bpp==15: add 1 here to force 15 to 16 for the shift
    pGetSurfaceDimensionsArg->pitch = ((pGetSurfaceDimensionsArg->width * ((pGetSurfaceDimensionsArg->bpp+1) >> 3)) + 0x3F) & ~0x3F;
    pGetSurfaceDimensionsArg->size  = pGetSurfaceDimensionsArg->pitch * pGetSurfaceDimensionsArg->height;
        
    if (!(pGetSurfaceDimensionsArg->flags & FB_GET_SURFACE_PITCH_NO_ADJUST))
    {
        nvHalFbRoundTiledPitchSize_NV10(&pGetSurfaceDimensionsArg->pitch,
                                        pGetSurfaceDimensionsArg->height,
                                        &pGetSurfaceDimensionsArg->size);
    }
    return RM_OK;
}

static VOID
nvHalFbRoundTiledPitchSize_NV10(U032 *pitch, U032 height, U032 *size)
{
    // adjust the pitch, if necessary
    if (*pitch <= 0x200)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _0200);
    else if (*pitch <= 0x400)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _0400);
    else if (*pitch <= 0x600)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _0600);
    else if (*pitch <= 0x800)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _0800);
    else if (*pitch <= 0xA00)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _0A00);
    else if (*pitch <= 0xC00)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _0C00);
    else if (*pitch <= 0xE00)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _0E00);
    else if (*pitch <= 0x1000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _1000);
    else if (*pitch <= 0x1400)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _1400);
    else if (*pitch <= 0x1800)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _1800);
    else if (*pitch <= 0x1C00)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _1C00);
    else if (*pitch <= 0x2000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _2000);
    else if (*pitch <= 0x2800)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _2800);
    else if (*pitch <= 0x3000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _3000);
    else if (*pitch <= 0x3800)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _3800);
    else if (*pitch <= 0x4000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _4000);
    else if (*pitch <= 0x5000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _5000);
    else if (*pitch <= 0x6000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _6000);
    else if (*pitch <= 0x7000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _7000);
    else if (*pitch <= 0x8000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _8000);
    else if (*pitch <= 0xA000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _A000);
    else if (*pitch <= 0xC000)
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _C000);
    else
        *pitch = DRF_DEF(_PFB, _TSIZE, _PITCH, _E000);

    if (size) {
        U032 hzmult;

        // set size given possibly adjusted pitch
        *size = *pitch * height;

        //
        // Round size to an even multiple of horizontal tiles. Remapping occurs
        // within addresses of length (pitch * tile_height * 2) in the vertical
        // direction (where the max TILE_HEIGHT is 16 lines).
        //
        hzmult = *pitch * (16 * 2);
        *size = ((*size + (hzmult - 1)) / hzmult) * hzmult;
    }
}

//
// Before updating the tiling registers (in particular, the PGRAPH version),
// idle graphics as best as possible.
//
// If we're not GR_IDLE during these writes to the PGRAPH registers, these
// writes can be dropped leaving the PFB and PGRAPH regs inconsistent. The
// idle/resume routines are called during FreeTiledRange/ProgramTiledRange.
//
static VOID
nvHalTilingUpdateIdle_NV10
(
    PHALHWINFO pHalHwInfo,
    U032       *pDmaCaches,
    U032       *pDmaPush
)
{
    *pDmaCaches = REG_RD32(NV_PFIFO_CACHES);
    FLD_WR_DRF_DEF(_PFIFO, _CACHES, _REASSIGN, _DISABLED);
    
    // Empty FIFO and runout.
    while ((REG_RD_DRF(_PFIFO, _CACHE1_STATUS, _LOW_MARK) != NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY)
        || (REG_RD_DRF(_PFIFO, _RUNOUT_STATUS, _LOW_MARK) != NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY)
        || (REG_RD_DRF(_PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY))
    {
        // Make sure we process any fifo interrupts
        HALRMSERVICEINTR(pHalHwInfo, FIFO_ENGINE_TAG);

        // Check if GE needs servicing.
        if (REG_RD32(NV_PGRAPH_INTR))
            HALRMSERVICEINTR(pHalHwInfo, GR_ENGINE_TAG);

        // Check if vblank needs servicing.
        if (REG_RD32(NV_PMC_INTR_0) & (DRF_DEF(_PMC, _INTR_0, _PCRTC,  _PENDING) |
                                       DRF_DEF(_PMC, _INTR_0, _PCRTC2, _PENDING)))
            HALRMSERVICEINTR(pHalHwInfo, DAC_ENGINE_TAG);
    }

    // Disable the dma pusher
    *pDmaPush = REG_RD32(NV_PFIFO_CACHE1_DMA_PUSH);
    FLD_WR_DRF_DEF(_PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _DISABLED);

    // Wait until the DMA pusher is idle
    while (REG_RD_DRF(_PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY)
        ;
}

static VOID
nvHalTilingUpdateResume_NV10
(
    PHALHWINFO pHalHwInfo,
    U032       DmaCaches,
    U032       DmaPush
)
{
    // Restore the dma pusher state
    REG_WR32(NV_PFIFO_CACHE1_DMA_PUSH, DmaPush);

    // Restore the reassign state
    REG_WR32(NV_PFIFO_CACHES, DmaCaches);
}

//
// nvHalFbAlloc
//
// Update user alloc request parameter according to memory
// type and (possibly) reserve hw resources.
//
RM_STATUS
nvHalFbAlloc_NV10(VOID *arg)
{
    PFBALLOCARG_000 pFbAllocArg = (PFBALLOCARG_000)arg;
    PFBHALINFO_NV10 pFbHalPvtInfo;
    PFBALLOCINFO pFbAllocInfo;
    U032 i, pitch, size;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbAllocArg->id != FB_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    pFbHalPvtInfo = (PFBHALINFO_NV10)pFbAllocArg->pHalHwInfo->pFbHalPvtInfo;
    pFbAllocInfo = pFbAllocArg->pAllocInfo;
    
    // Set default alignment based on type.
    switch (pFbAllocInfo->type) {
    case MEM_TYPE_IMAGE:
    case MEM_TYPE_OVERLAY:
        pFbAllocInfo->align = 0x3f;         // 64 byte alignment
        break;
    case MEM_TYPE_TEXTURE:
        pFbAllocInfo->align = 0xff;         // 256 byte alignment
        break;
    case MEM_TYPE_FONT:
        pFbAllocInfo->align = 0x1f;         // 32 byte alignment
        break;
    case MEM_TYPE_CURSOR:
        pFbAllocInfo->align = 0x7ff;        // 2048 byte alignment
        break;
    case MEM_TYPE_DMA:
        pFbAllocInfo->align = 0x3;          // 4 byte alignment
        break;
    case MEM_TYPE_INSTANCE:
        pFbAllocInfo->align = 0xf;          // 16 byte alignment
        break;
    case MEM_TYPE_DEPTH:
    case MEM_TYPE_DEPTH_COMPR16:
    case MEM_TYPE_DEPTH_COMPR32:
    case MEM_TYPE_PRIMARY:
    case MEM_TYPE_IMAGE_TILED:
        pFbAllocInfo->align = 0xfff;        // 4096 byte alignment
        break;
    default:
        // should this be 0xffffffff
        pFbAllocInfo->align = pFbAllocInfo->alignPerf = 0;
        return (RM_ERR_BAD_ARGUMENT);
    }


    //
    // A non-zero pitch value indicates the user wants tiled memory
    // (if available).  If the pitch is 0, we're done.
    //
    if (pFbAllocInfo->pitch == 0)
    {
        pFbAllocInfo->alignPerf = pFbAllocInfo->align;
        return (RM_OK);
    }
        
    // Now allocate a TILE_RANGE
    for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
    {
        if ((pFbHalPvtInfo->usedTiles & MEM_TYPE_TILE_RANGE_SHIFT(i)) == 0)
        {
            pFbHalPvtInfo->usedTiles |= MEM_TYPE_TILE_RANGE_SHIFT(i);
            break;
        }
    }
    if (i == NV_PFB_TILE__SIZE_1)
    {
        return (RM_ERR_INSUFFICIENT_RESOURCES);
    }

    pitch = pFbAllocInfo->pitch;
    nvHalFbRoundTiledPitchSize_NV10(&pitch,
                                    pFbAllocInfo->height,
                                    &size);

    //
    // If we're not allowing the pitch to be adjusted, we can only tile the surface
    // if the pitch is already HW aligned.
    // 
    if ((pFbAllocArg->flags & FB_ALLOC_NO_PITCH_ADJUST) && (pitch != pFbAllocInfo->pitch))
    {
        // requires rounding, so give back the range and fail the alloc
        pFbHalPvtInfo->usedTiles &= ~MEM_TYPE_TILE_RANGE_SHIFT(i);
        pFbAllocInfo->hwResId = 0;
        return (RM_OK);
    }

    // tiles are _typically_ 16K aligned.
    pFbAllocInfo->align = pFbAllocInfo->alignPerf = pFbHalPvtInfo->uTileAlign;

    // return adjusted pitch and size
    pFbAllocInfo->pitch = pitch;
    pFbAllocInfo->size  = size;

    pFbAllocInfo->hwResId = MEM_TYPE_TILE_RANGE_SHIFT(i);
    return (RM_OK);
}

//
// nvHalFbFree
//
// Release tile back to the free pool.
//
RM_STATUS
nvHalFbFree_NV10(VOID *arg)
{
    PFBFREEARG_000 pFbFreeArg = (PFBFREEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PFBHALINFO_NV10 pFbHalPvtInfo;
    U032 i, dmapush, dmacaches;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbFreeArg->id != FB_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbFreeArg->pHalHwInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV10)pFbFreeArg->pHalHwInfo->pFbHalPvtInfo;

    //
    // Bail out now if there's no resource id.
    if (!pFbFreeArg->hwResId)
        return (RM_OK);

    // convert range bits to a TILE index
    for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
        if (pFbFreeArg->hwResId & MEM_TYPE_TILE_RANGE_SHIFT(i))
                break;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Freeing TILE Range:   ", i);

    //
    // It's possible for us to be freeing a tiled region after we've
    // disable PCI accesses to our chip (i.e. we get here after
    // we've gone through MC_POWER_LEVEL_2).
    // 
    // Before touching the hw, make sure the device is enabled.
    //
    if (REG_RD32(NV_PMC_BOOT_0) == 0xffffffff)
    {
        // mark it free
        pFbHalPvtInfo->usedTiles &= ~pFbFreeArg->hwResId;
        return (RM_OK);
    }

    // idle graphics, so we can update the PGRAPH registers
    nvHalTilingUpdateIdle_NV10(pFbFreeArg->pHalHwInfo, &dmacaches, &dmapush);

    //
    // Check for integrated graphics -- we need to setup tiling through the north bridge
    //
    if( IsNV1A_NV10(pHalHwInfo->pMcHalInfo) )
    {
        Nv1AInvalidateTile( pHalHwInfo, i, 1 );
    }
    else
    {
        do {
            // Now, wait until graphics is idle
            HAL_GR_IDLE(pFbFreeArg->pHalHwInfo);

            // invalidate the range
            REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
            REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);    

        } while (REG_RD32(NV_PFB_TILE(i)) != REG_RD32(NV_PGRAPH_TILE(i)));
    }

    // resume graphics, now that PFB/PGRAPH are consistent
    nvHalTilingUpdateResume_NV10(pHalHwInfo, dmacaches, dmapush);

    // mark it free
    pFbHalPvtInfo->usedTiles &= ~pFbFreeArg->hwResId;

    return (RM_OK);
}

//
// nvHalFbSetAllocParameters
//
// Associate memory block (base/limit) with hardware resources.
//
RM_STATUS
nvHalFbSetAllocParameters_NV10(VOID *arg)
{
    PFBSETALLOCPARAMSARG_000 pFbSetAllocParamsArg = (PFBSETALLOCPARAMSARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PFBHALINFO_NV10 pFbHalPvtInfo;
    PFBALLOCINFO pFbAllocInfo;
    U032 i, dmapush, dmacaches;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbSetAllocParamsArg->id != FB_SET_ALLOC_PARAMS_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbSetAllocParamsArg->pHalHwInfo;
    pFbAllocInfo = pFbSetAllocParamsArg->pAllocInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV10)pFbSetAllocParamsArg->pHalHwInfo->pFbHalPvtInfo;

    //
    // Bail out if this allocation didn't reserve any tiling regs.
    //
    if (pFbAllocInfo->hwResId == 0)
        return (RM_OK);

    //
    // Convert range bits to a TILE index.
    //
    for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
        if (pFbAllocInfo->hwResId == (U032)MEM_TYPE_TILE_RANGE_SHIFT(i))
            break;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Programming TILE Range:   ", i);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:          Base =   ", pFbAllocInfo->offset);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:         Limit =   ", pFbAllocInfo->offset + (pFbAllocInfo->size - 1));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:         Pitch =   ", pFbAllocInfo->pitch);

    if (pFbAllocInfo->offset & 0x3fff)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Error: unaligned Base\n");
        // mark it free
        pFbHalPvtInfo->usedTiles &= ~pFbAllocInfo->hwResId;
        // invalidate the range
        //
        // Check for integrated graphics -- we need to setup tiling through the north bridge
        //
        if( IsNV1A_NV10(pHalHwInfo->pMcHalInfo) )
        {
            Nv1AInvalidateTile( pHalHwInfo, i, 1 );
        }
        else
        {
            REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
            REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);    
        }
        return (RM_OK);
    }
    if (pFbAllocInfo->size & 0x3fff)
    {
        // mark it free
        pFbHalPvtInfo->usedTiles &= ~pFbAllocInfo->hwResId;
        // invalidate the range
        //
        // Check for integrated graphics -- we need to setup tiling through the north bridge
        //
        if( IsNV1A_NV10(pHalHwInfo->pMcHalInfo) )
        {
            Nv1AInvalidateTile( pHalHwInfo, i, 1 );
        }
        else
        {
            REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
            REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);    
        }
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Error: unaligned Limit\n");
        return (RM_OK);
    }

    // idle graphics, so we can update the PGRAPH registers
    nvHalTilingUpdateIdle_NV10(pFbSetAllocParamsArg->pHalHwInfo, &dmacaches, &dmapush);

    //
    // Note: due to a PFB_TLIMIT readback bug, only compare the upper bits
    // when checking for PFB_TLIMIT/PGRAPH_TLIMIT consistency.
    // 

    //
    // Check for integrated graphics -- we need to setup tiling through the north bridge
    //
    if( IsNV1A_NV10(pHalHwInfo->pMcHalInfo) )
    {
        //North bridge integrated graphics has a different tile setup
        U016    wNBVendorID;
        U016    wNBDeviceID;
        VOID    *uNBHandle = 0;
        U032    uNBFakeTLIMIT;
        U032    uNBFakeTSIZE;
        U032    uNBFakeTILE;

        uNBHandle = osPciInitHandle(0, 0, 2, &wNBVendorID, &wNBDeviceID);
        if( !uNBHandle )
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unable to touch integrated graphics tiling\n");
            DBG_BREAKPOINT();
            return (RM_ERROR);
        }

        do {
            U032    uNBTBASE  = WriteNBTBASE(pFbAllocInfo->pitch, pFbAllocInfo->offset, pFbAllocInfo->size, 1);
            U032    uNBTLIMIT = WriteNBTLIMIT(pFbAllocInfo->pitch, pFbAllocInfo->offset, pFbAllocInfo->size, 1);

            // Now, wait until graphics is idle
            HAL_GR_IDLE(pHalHwInfo);

            osPciWriteDword(uNBHandle, CR_XL_TBASE(i)  & 0xFF, uNBTBASE );
            osPciWriteDword(uNBHandle, CR_XL_TLIMIT(i) & 0xFF, uNBTLIMIT );

            REG_WR32(NV_PGRAPH_TLIMIT(i), pFbAllocInfo->offset + pFbAllocInfo->size - 1);
            REG_WR32(NV_PGRAPH_TSIZE(i), pFbAllocInfo->pitch);
            REG_WR32(NV_PGRAPH_TILE(i), pFbAllocInfo->offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));

            uNBTBASE  = osPciReadDword(uNBHandle, CR_XL_TBASE(i)  & 0xFF );
            uNBTLIMIT = osPciReadDword(uNBHandle, CR_XL_TLIMIT(i) & 0xFF );

            uNBFakeTLIMIT = FormatPFBTLimit( ReadNBPitch( uNBTBASE, uNBTLIMIT ), ReadNBOffset( uNBTBASE, uNBTLIMIT ), ReadNBSize( uNBTBASE, uNBTLIMIT ), ReadNBValid( uNBTBASE, uNBTLIMIT ) );
            uNBFakeTSIZE  = FormatPFBTSize ( ReadNBPitch( uNBTBASE, uNBTLIMIT ), ReadNBOffset( uNBTBASE, uNBTLIMIT ), ReadNBSize( uNBTBASE, uNBTLIMIT ), ReadNBValid( uNBTBASE, uNBTLIMIT ) );
            uNBFakeTILE   = FormatPFBTile  ( ReadNBPitch( uNBTBASE, uNBTLIMIT ), ReadNBOffset( uNBTBASE, uNBTLIMIT ), ReadNBSize( uNBTBASE, uNBTLIMIT ), ReadNBValid( uNBTBASE, uNBTLIMIT ) );

        } while ((uNBFakeTILE != REG_RD32(NV_PGRAPH_TILE(i))) ||
                 (uNBFakeTSIZE != REG_RD32(NV_PGRAPH_TSIZE(i))) ||
                 ((uNBFakeTLIMIT & ~0x3FFF) != (REG_RD32(NV_PGRAPH_TLIMIT(i)) & ~0x3FFF)));
    }
    else
    {
        do {
            // Now, wait until graphics is idle
            HAL_GR_IDLE(pHalHwInfo);

            REG_WR32(NV_PFB_TLIMIT(i), pFbAllocInfo->offset + pFbAllocInfo->size - 1);
            REG_WR32(NV_PFB_TSIZE(i), pFbAllocInfo->pitch);
            REG_WR32(NV_PFB_TILE(i), pFbAllocInfo->offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));

            REG_WR32(NV_PGRAPH_TLIMIT(i), pFbAllocInfo->offset + pFbAllocInfo->size - 1);
            REG_WR32(NV_PGRAPH_TSIZE(i), pFbAllocInfo->pitch);
            REG_WR32(NV_PGRAPH_TILE(i), pFbAllocInfo->offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));

        } while ((REG_RD32(NV_PFB_TILE(i)) != REG_RD32(NV_PGRAPH_TILE(i))) ||
                 (REG_RD32(NV_PFB_TSIZE(i)) != REG_RD32(NV_PGRAPH_TSIZE(i))) ||
                 ((REG_RD32(NV_PFB_TLIMIT(i)) & ~0x3FFF) != (REG_RD32(NV_PGRAPH_TLIMIT(i)) & ~0x3FFF)));
    }

    // resume graphics, now that PFB/PGRAPH are consistent
    nvHalTilingUpdateResume_NV10(pHalHwInfo, dmacaches, dmapush);

    return (RM_OK);
}

RM_STATUS
nvHalFbLoadOverride_NV10(VOID *arg)
{
    PFBLOADOVERRIDEARG_000 pFbLoadOverrideArg = (PFBLOADOVERRIDEARG_000)arg;
    PHALHWINFO pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pFbLoadOverrideArg->id != FB_LOAD_OVERRIDE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbLoadOverrideArg->pHalHwInfo;

    switch (pFbLoadOverrideArg->cmd)
    {
        case FB_LOAD_OVERRIDE_MEMORY:
            REG_WR32(NV_PFB_CFG, pFbLoadOverrideArg->value);
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}


//CAH: Setup to detect possible alliasing of memory to tell us we have double
//the memory that we actually do. This algorithm will detect the case where
//memory addresses lay on top of eachother so as to make it appear that there
//is twice the memory available than is physically present. 
static U032 nvHalFbRealMemSize(U032 RamSizeMb, PHWREG base)
{
    U032 i, wp = 0;
    U032 checksum1 = 0, checksum2 = 0;
    U032 fb_save[128]; 
    U032 uTag;

    //Write 0,1,2,3,...N on each 1mb boundry in the FB
    for (i = 0, wp = 0; i < RamSizeMb; i++, wp += 0x00100000) 
    {
        fb_save[i] = base->Reg032[wp/4];
        uTag = 0x01010101*(i+1);  //Write a more interesting pattern - 0 is too easy to mistake
        base->Reg032[wp/4] = uTag;
        checksum1 += uTag;
        checksum2 = calcChkSum(base, i);
        if (checksum1 != checksum2)
        {
            restoreFbSave(fb_save, base, i);
            return i; 
        }
    }

    restoreFbSave(fb_save, base, --i);
    return RamSizeMb; //This means we made it all the way through memory.
}

static void restoreFbSave(U032 *save, PHWREG base, U032 index)
{
    U032 i, wp;

    for (i = 0, wp = 0; i <= index; i++, wp += 0x00100000)
    {
        base->Reg032[wp/4] = save[i];
    }
}

static U032 calcChkSum(PHWREG base, U032 index)
{
    U032 i, wp, checksum = 0;

    for (i = 0, wp = 0; i <= index; i++, wp += 0x00100000)
    {
        checksum += base->Reg032[wp/4];
    }

    return checksum;
}

//
// For North bridge integrated graphics, part of the tiling control is in the north bridge
// We need to clear it via PCI config space
//

VOID Nv1AInvalidateTile( PHALHWINFO pHalHwInfo, U032 uStart, U032 uCount )

{
    //North bridge integrated graphics has a different tile setup
    U016    wNBVendorID;
    U016    wNBDeviceID;
    VOID    *uNBHandle = 0;
    U032    i;

    uNBHandle = osPciInitHandle(0, 0, 2, &wNBVendorID, &wNBDeviceID);
    if( !uNBHandle )
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unable to touch integrated graphics tiling\n");
        DBG_BREAKPOINT();
        return;
    }

    for( i=uStart; uCount>0; uCount--, i++ )
    {
        do {
            // Now, wait until graphics is idle
            HAL_GR_IDLE(pHalHwInfo);

            REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);    
            osPciWriteDword(uNBHandle, CR_XL_TBASE(i) & 0xFF, CR_XL_TBASE_REGION_INVALID );

        } while (osPciReadDword(uNBHandle, CR_XL_TBASE(i)  & 0xFF ) != REG_RD32(NV_PGRAPH_TILE(i)));
    }
}
