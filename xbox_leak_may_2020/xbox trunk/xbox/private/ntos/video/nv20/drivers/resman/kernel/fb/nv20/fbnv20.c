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
* Module: FBNV20.c                                                          *
*   The NV20 specific HAL FB routines reside in this file.                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv20_ref.h>
#include <nvrm.h>
#include <nv20_hal.h>
#include <heap.h>
#include "nvhw.h"
#include "nvhalcpp.h"
#include "bios.h"

// forwards
RM_STATUS nvHalFbControl_NV20(VOID *);
RM_STATUS nvHalFbAlloc_NV20(VOID *);
RM_STATUS nvHalFbFree_NV20(VOID *);
RM_STATUS nvHalFbSetAllocParameters_NV20(VOID *);
RM_STATUS nvHalFbGetSurfacePitch_NV20(VOID *);
RM_STATUS nvHalFbGetSurfaceDimensions_NV20(VOID *);
RM_STATUS nvHalFbLoadOverride_NV20(VOID *);

// statics
static VOID _nvHalFbRoundTiledPitchSize(U032 *, U032, U032 *);
static RM_STATUS _nvHalFbAllocZtags(PHALHWINFO, U032 *, U032 *);
static RM_STATUS _nvHalFbFreeZtags(PHALHWINFO, U032, U032);
static VOID _nvHalFbInitDrvSlwParms(PHALHWINFO);
VOID nvHalFbCalcDrvSlw_NV20(PHALHWINFO);

// extern -- remove this when we no longer have to update mclk ourselves
extern RM_STATUS nvHalDacProgramMClk_NV20(VOID *);

// NV20 expects a 1024byte alignment
#define FC_MIN_ALIGN_SHIFT      10

//
// nvHalFbControl
//
RM_STATUS
nvHalFbControl_NV20(VOID *arg)
{
    PFBCONTROLARG_000 pFbControlArg = (PFBCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PFBHALINFO pFbHalInfo;
    PPRAMHALINFO pPramHalInfo;
    PFBHALINFO_NV20 pFbHalPvtInfo;
    U032 i;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbControlArg->id != FB_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbControlArg->pHalHwInfo;
    pFbHalInfo = pHalHwInfo->pFbHalInfo;
    pPramHalInfo = pHalHwInfo->pPramHalInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;

    switch (pFbControlArg->cmd)
    {
        case FB_CONTROL_INIT:
        {
#if 0
            U032 cfg0, cfg1;
#endif
            U032 tmpAddr;

            if (pFbHalInfo->AdjustDriveSlew)
            {
                //
                // Adjust drive/slew values if the timer is running.
                //
                if (REG_RD32(NV_PTIMER_NUMERATOR) != 0 && REG_RD32(NV_PTIMER_DENOMINATOR) != 0)
                {
                    //
                    // Init params structure.   
                    //
                    _nvHalFbInitDrvSlwParms(pHalHwInfo);
    
                    //
                    // Update the hardware.
                    //
                    nvHalFbCalcDrvSlw_NV20(pHalHwInfo);
                }
            }

            //
            // Save off chip-dependent fb state.
            //
            pFbHalPvtInfo->fbSave[0] = REG_RD32(NV_PFB_CFG0);
            pFbHalPvtInfo->fbSave[1] = REG_RD32(NV_PFB_CFG1);

            //
            // Setup "bank" count.
            //
            if (DRF_VAL(_PFB, _CFG0, _EXTBANK, pFbHalPvtInfo->fbSave[0]) == NV_PFB_CFG0_EXTBANK_1)
                pFbHalInfo->BankCount = 4;
            else
                pFbHalInfo->BankCount = 2;

            //
            // Set lines-per-tile.
            //
            switch (DRF_VAL(_PFB, _CFG1, _COL, pFbHalPvtInfo->fbSave[1]))
            {
                case NV_PFB_CFG1_COL_9:
                    pFbHalPvtInfo->linesPerTileShift = 5;   // 32
                    break;
                case NV_PFB_CFG1_COL_10:
                    pFbHalPvtInfo->linesPerTileShift = 6;   // 64
                    break;
                case NV_PFB_CFG1_COL_7:
                case NV_PFB_CFG1_COL_8:
                default:
                    pFbHalPvtInfo->linesPerTileShift = 4;   // 16
                    break;
            }

            //
            // Set ram type.
            //
            if (REG_RD_DRF(_PBUS, _FBIO_RAM, _TYPE) == NV_PBUS_FBIO_RAM_TYPE_SDR)
                pFbHalInfo->RamType = BUFFER_SDRAM;
            else
                pFbHalInfo->RamType = BUFFER_DDRAM;

            //
            // Set ram size.  Beginning with nv20, we need to
            // support external banks of possibly differing sizes.
            // Start by calculating size of primary bank and then
            // adding size of secondary bank if it exists.
            //
            // The formula (from James Van Dyke):
            //
            //  mem_in_bytes = (1 << (R + B + C + P)) * 4bytes, P=0,1,2
            //      R = row bits
            //      B = bank bits
            //      C = column bits
            //      P = partition count
            //

#if 0
            //
            // Set external bank count.
            //
            cfg0 = REG_RD32(NV_PFB_CFG0);
            cfg1 = REG_RD32(NV_PFB_CFG1);

            if (DRF_VAL(_PFB, _CFG0, _EXTBANK, cfg0) == NV_PFB_CFG0_EXTBANK_1)
                pFbHalInfo->ExtBankCount = 2;
            else
                pFbHalInfo->ExtBankCount = 1;

            //
            // Calculate primary external bank size.
            //
            size = (1 << (DRF_VAL(_PFB, _CFG1, _COL, cfg0) +
                          DRF_VAL(_PFB, _CFG1, _ROWA, cfg0) +
                          DRF_VAL(_PFB, _CFG1, _BANKA, cfg0) +
                          DRF_VAL(_PFB, _CFG0, _PART, cfg0))) * 4;
            pFbHalInfo->ExtBanks[0].size = size;
            pFbHalInfo->ExtBanks[0].intBanks = DRF_VAL(_PFB, _CFG1, _BANKA, cfg0) + 1;

            //
            // Check for secondary external bank.
            //
            if (DRF_VAL(_PFB, _CFG0, _EXTBANK, cfg0) == NV_PFB_CFG0_EXTBANK_1)
            {
                size = (1 << (DRF_DEF(_PFB, _CFG1, _COL, cfg1) +
                              DRF_DEF(_PFB, _CFG1, _ROWB, cfg1) +
                              DRF_DEF(_PFB, _CFG1, _BANKB, cfg1) +
                              DRF_DEF(_PFB, _CFG0, _PART, cfg0))) * 4;
                pFbHalInfo->ExtBanks[1].size = size;
                pFbHalInfo->ExtBanks[1].intBanks = DRF_VAL(_PFB, _CFG1, _BANKB, cfg0) + 1;
            }
#ifdef DEBUG
            //
            // Debug calculated size vs hard-wired value.
            //
            if (size != REG_RD_DRF(_PFB, _CSTATUS, _RAMAMOUNT_LS))
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: framebuffer size miscalculation ", size);
                DBG_BREAKPOINT();
            }

            //
            // Generate a debug message if the MS bit is actually set.
            //
            if (REG_RD_DRF(_PFB, _CSTATUS, _RAMAMOUNT_MS))
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: > 4G of framebuffer indicated!!");
                DBG_BREAKPOINT();
            }
#endif
#endif

            pFbHalInfo->RamSizeMb = REG_RD_DRF(_PFB, _CSTATUS, _RAMAMOUNT_LS);
            pFbHalInfo->RamSize = pFbHalInfo->RamSizeMb * 0x100000;

            //
            // Initialize ZTAG bitmap (0=allocated, 1=free).
            //
            for (i = 0; i < MAX_ZTAG_BITS/8; i++)
                pFbHalPvtInfo->zTagPool[i] = 0xff;

            for (i = 0; i < NV_PFB_ZCOMP__SIZE_1; i++)
            {
                pFbHalPvtInfo->zCompState[i].offset = MAX_ZTAG_BITS;
                pFbHalPvtInfo->zCompState[i].size = 0;
            }

            //
            // Fb arbitration settings from /hw/nv20/doc/perf_settings.doc.
            //
            // The ARB_XFER_SZ value we use needs to workaround a zcull/zcomp
            // bug in the hw.
            //
            //REG_WR32(NV_PFB_ARB_XFER_SZ, 0x84848880);
            REG_WR32(NV_PFB_ARB_XFER_SZ, 0x84f48880);
            REG_WR32(NV_PFB_ARB_TIMEOUT, 0xffffcff0);
            REG_WR32(NV_PFB_ARB_PREDIVIDER, 0x1);

            //
            // Set partition count.
            // 0=1partition, 1=2partitions, 3=4partitions (default).
            //
            pFbHalPvtInfo->partitions = REG_RD_DRF(_PFB, _CFG0, _PART) + 1;

            //
            // Set instance space size.  Add in whatever os-dependent
            // (i.e. BIOS) space is needed as well.
            //
            // For NV20, this is 576K allocatable.
            //
            pPramHalInfo->TotalInstSize = 0x90000 + pPramHalInfo->InstPadSize;

            //
            // The start of all of these space is after the BIOS
            // (denoted by InstPadSize).
            //
            tmpAddr = DEVICE_BASE(NV_PRAMIN) + pPramHalInfo->InstPadSize;

            //
            // Configure hash table.
            //
            pFbHalPvtInfo->hashTableSize = 0x4000;
            pPramHalInfo->HashDepth = pFbHalPvtInfo->hashTableSize / sizeof (HASH_TABLE_ENTRY);
            pPramHalInfo->HashSearch = 128;

            pFbHalPvtInfo->hashTableAddr = tmpAddr;
            tmpAddr += pFbHalPvtInfo->hashTableSize;

            REG_WR32(NV_PFIFO_RAMHT, (DRF_DEF(_PFIFO, _RAMHT, _SIZE, _16K) |
                                      DRF_NUM(_PFIFO, _RAMHT, _BASE_ADDRESS, (pFbHalPvtInfo->hashTableAddr - DEVICE_BASE(NV_PRAMIN)) >> 12) |
                                      DRF_DEF(_PFIFO, _RAMHT, _SEARCH, _128)));
            
            //
            // Configure fifo context areas.
            // NV20 has two distinct context areas.
            //
            pFbHalPvtInfo->fifoContextSize1 = NUM_FIFOS_NV20 * FC1_SIZE_NV20;
            pFbHalPvtInfo->fifoContextSize2 = NUM_FIFOS_NV20 * FC2_SIZE_NV20;

            pFbHalPvtInfo->fifoContextAddr1 = tmpAddr;
            tmpAddr += pFbHalPvtInfo->fifoContextSize1;
            pFbHalPvtInfo->fifoContextAddr2 = tmpAddr;
            tmpAddr += pFbHalPvtInfo->fifoContextSize2;

            REG_WR32(NV_PFIFO_RAMFC,
                     (DRF_NUM(_PFIFO, _RAMFC, _BASE_ADDRESS, ((pFbHalPvtInfo->fifoContextAddr1 - DEVICE_BASE(NV_PRAMIN)) >> FC_MIN_ALIGN_SHIFT))) |
                     DRF_DEF(_PFIFO, _RAMFC, _SIZE, _2K) |
                     (DRF_NUM(_PFIFO, _RAMFC, _BASE_ADDRESS2, ((pFbHalPvtInfo->fifoContextAddr2 - DEVICE_BASE(NV_PRAMIN)) >> FC_MIN_ALIGN_SHIFT))));

            //
            // Configure runout area.
            //
            pFbHalPvtInfo->fifoRunoutSize = 512;
            pFbHalPvtInfo->fifoRunoutAddr = tmpAddr;
            tmpAddr += pFbHalPvtInfo->fifoRunoutSize;
            pFbHalPvtInfo->fifoRunoutMask = pFbHalPvtInfo->fifoRunoutSize - 1;
            REG_WR32(NV_PFIFO_RAMRO, (DRF_DEF(_PFIFO, _RAMRO, _SIZE, _512) |
                                      DRF_NUM(_PFIFO, _RAMRO, _BASE_ADDRESS, (pFbHalPvtInfo->fifoRunoutAddr - DEVICE_BASE(NV_PRAMIN)) >> 9)));

            //
            // Sum of all allocations plus a little slop
            //
            pPramHalInfo->CurrentSize  = (tmpAddr + 512) - (DEVICE_BASE(NV_PRAMIN) + pPramHalInfo->InstPadSize);

            break;
        }
        case FB_CONTROL_LOAD:
            break;
        case FB_CONTROL_UNLOAD:
            break;
        case FB_CONTROL_UPDATE:
            break;
        case FB_CONTROL_DESTROY:
            REG_WR32(NV_PFB_CFG0, pFbHalPvtInfo->fbSave[0]);
            REG_WR32(NV_PFB_CFG1, pFbHalPvtInfo->fbSave[1]);
            break;
        case FB_CONTROL_INSTWR_DISABLE:
            // disable writes by enabling  protection
            switch (pPramHalInfo->TotalInstSize)
            {
                case 0xA0000:
                    FLD_WR_DRF_DEF(_PFB, _NVM, _LIMIT, _640K);
                    break;
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
            HAL_GR_IDLE(pHalHwInfo);

            // TILE region registers
            for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
            {
                REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
                REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);
                RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                             NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(i),
                             NV_PFB_TILE_REGION_INVALID);
            }

            // ZCOMP_OFFSET register
            REG_WR32(NV_PGRAPH_ZCOMP_OFFSET, NV_PFB_ZCOMP_OFFSET_EN_FALSE);
            RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                         NV_IGRAPH_TC_MINTFCDEBUG_ZOFFSET_ADDRESS,
                         NV_PFB_ZCOMP_OFFSET_EN_FALSE);

            // ZCOMP region registers
            for (i = 0; i < NV_PFB_ZCOMP__SIZE_1; i++)
            {
                REG_WR32(NV_PFB_ZCOMP(i), NV_PFB_ZCOMP_EN_FALSE);
                REG_WR32(NV_PGRAPH_ZCOMP(i), NV_PFB_ZCOMP_EN_FALSE);
                RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                             NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(i),
                             NV_PFB_ZCOMP_EN_FALSE);
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
nvHalFbGetSurfacePitch_NV20(VOID *arg)
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

    if (!(pGetSurfacePitchArg->flags & FB_GET_SURFACE_PITCH_NO_ADJUST))
    {
        _nvHalFbRoundTiledPitchSize(&pGetSurfacePitchArg->pitch, NULL, NULL);
    }
    return RM_OK;
}

//
// nvHalFbGetSurfaceDimensions
//
// Given a width/height and bits per pixel, return the surface pitch and size
//
RM_STATUS
nvHalFbGetSurfaceDimensions_NV20(VOID *arg)
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
        _nvHalFbRoundTiledPitchSize(&pGetSurfaceDimensionsArg->pitch,
                                        pGetSurfaceDimensionsArg->height,
                                        &pGetSurfaceDimensionsArg->size);
    }

    return RM_OK;
}

static VOID
_nvHalFbRoundTiledPitchSize(U032 *pitch, U032 height, U032 *size)
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
nvHalTilingUpdateIdle_NV20
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
nvHalTilingUpdateResume_NV20
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
nvHalFbAlloc_NV20(VOID *arg)
{
    PFBALLOCARG_000 pFbAllocArg = (PFBALLOCARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PFBHALINFO_NV20 pFbHalPvtInfo;
    PFBALLOCINFO pFbAllocInfo;
    U032 i, pitch, size;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbAllocArg->id != FB_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbAllocArg->pHalHwInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;
    pFbAllocInfo = pFbAllocArg->pAllocInfo;
    
    // Set default alignment based on type.
    switch (pFbAllocInfo->type) {
    case MEM_TYPE_IMAGE:
    case MEM_TYPE_OVERLAY:
        //
        // These are used for texturing by mocomp, so we're forced
        // to align them above the 64byte requirement the overlay
        // hw requires.
        //
        pFbAllocInfo->align = 0xff;         // 256 byte alignment
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

    _nvHalFbRoundTiledPitchSize(&pitch, pFbAllocInfo->height, &size);

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

    //
    // Both the required and optimal aligment are 16K.
    //
    pFbAllocInfo->align = pFbAllocInfo->alignPerf = 0x3fff;

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
nvHalFbFree_NV20(VOID *arg)
{
    PFBFREEARG_000 pFbFreeArg = (PFBFREEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PFBHALINFO_NV20 pFbHalPvtInfo;
    U032 i, dmapush, dmacaches;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbFreeArg->id != FB_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbFreeArg->pHalHwInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;

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
    // disable PCI accesses to our chip while handling certain
    // power mgmt calls.  We do this explicitly in some power levels
    // but the os can also do this (e.g. win9x).
    // 
    // Before touching the hw, make sure the device is enabled.
    // If it isn't, don't bother modifying the hw, but clear the
    // sw enable bit so we know that this region is to be marked invalid
    // when we resume.
    //
    if (REG_RD32(NV_PMC_BOOT_0) == 0xffffffff)
    {
        // mark it free
        pFbHalPvtInfo->usedTiles &= ~pFbFreeArg->hwResId;
        return (RM_OK);
    }

    // idle graphics, so we can update the PGRAPH registers
    nvHalTilingUpdateIdle_NV20(pFbFreeArg->pHalHwInfo, &dmacaches, &dmapush);

    do {
        // Now, wait until graphics is idle
        HAL_GR_IDLE(pFbFreeArg->pHalHwInfo);

        // invalidate the range
        REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
        REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);    
        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(i),
                     NV_PFB_TILE_REGION_INVALID);
    } while (REG_RD32(NV_PFB_TILE(i)) != REG_RD32(NV_PGRAPH_TILE(i)));

    //
    // Free any z compression resources.
    //
    if (pFbHalPvtInfo->zCompState[i].offset != MAX_ZTAG_BITS && pFbHalPvtInfo->zCompState[i].size != 0)
    {
        do {

            // Now, wait until graphics is idle
            HAL_GR_IDLE(pFbFreeArg->pHalHwInfo);

            REG_WR32(NV_PFB_ZCOMP(i), NV_PFB_ZCOMP_EN_FALSE);
            REG_WR32(NV_PGRAPH_ZCOMP(i), NV_PFB_ZCOMP_EN_FALSE);
            RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                         NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(i),
                         NV_PFB_ZCOMP_EN_FALSE);
        } while (REG_RD32(NV_PFB_ZCOMP(i)) != REG_RD32(NV_PGRAPH_ZCOMP(i)));

        _nvHalFbFreeZtags(pHalHwInfo, pFbHalPvtInfo->zCompState[i].offset, pFbHalPvtInfo->zCompState[i].size);
        pFbHalPvtInfo->zCompState[i].offset = MAX_ZTAG_BITS;
        pFbHalPvtInfo->zCompState[i].size = 0;
    }

    // resume graphics, now that PFB/PGRAPH are consistent
    nvHalTilingUpdateResume_NV20(pHalHwInfo, dmacaches, dmapush);

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
nvHalFbSetAllocParameters_NV20(VOID *arg)
{
    PFBSETALLOCPARAMSARG_000 pFbSetAllocParamsArg = (PFBSETALLOCPARAMSARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PFBHALINFO_NV20 pFbHalPvtInfo;
    PFBALLOCINFO pFbAllocInfo;
    U032 i, dmapush, dmacaches, data32;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbSetAllocParamsArg->id != FB_SET_ALLOC_PARAMS_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbSetAllocParamsArg->pHalHwInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;
    pFbAllocInfo = pFbSetAllocParamsArg->pAllocInfo;

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
        REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
        REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);    
        return (RM_OK);
    }
    if (pFbAllocInfo->size & 0x3fff)
    {
        // mark it free
        pFbHalPvtInfo->usedTiles &= ~pFbAllocInfo->hwResId;
        // invalidate the range
        REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
        REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);    
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Error: unaligned Limit\n");
        return (RM_OK);
    }

    // idle graphics, so we can update the PGRAPH registers
    nvHalTilingUpdateIdle_NV20(pFbSetAllocParamsArg->pHalHwInfo, &dmacaches, &dmapush);

    //
    // Note: due to a PFB_TLIMIT readback bug, only compare the upper bits
    // when checking for PFB_TLIMIT/PGRAPH_TLIMIT consistency.
    // 
    do {
        // Now, wait until graphics is idle
        HAL_GR_IDLE(pHalHwInfo);

        REG_WR32(NV_PFB_TLIMIT(i), pFbAllocInfo->offset + pFbAllocInfo->size - 1);
        REG_WR32(NV_PFB_TSIZE(i), pFbAllocInfo->pitch);
        REG_WR32(NV_PGRAPH_TLIMIT(i), pFbAllocInfo->offset + pFbAllocInfo->size - 1);
        REG_WR32(NV_PGRAPH_TSIZE(i), pFbAllocInfo->pitch);

        //
        // Flip BANK0_SENSE on for depth buffers, but leave it off
        // for color and anything else.
        //
        data32 = pFbAllocInfo->offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID);
        if (pFbAllocInfo->type == MEM_TYPE_DEPTH ||
            pFbAllocInfo->type == MEM_TYPE_DEPTH_COMPR16 ||
            pFbAllocInfo->type == MEM_TYPE_DEPTH_COMPR32)
        {
            data32 |= DRF_DEF(_PFB, _TILE, _BANK0_SENSE, _1);
        }

        REG_WR32(NV_PGRAPH_TILE(i), data32);
        REG_WR32(NV_PFB_TILE(i), data32);
        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TLIMIT_ADDRESS(i),
                     pFbAllocInfo->offset + pFbAllocInfo->size - 1);
        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TPITCH_ADDRESS(i),
                     pFbAllocInfo->pitch);
        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(i),
                     data32);

    } while ((REG_RD32(NV_PFB_TILE(i)) != REG_RD32(NV_PGRAPH_TILE(i))) ||
             (REG_RD32(NV_PFB_TSIZE(i)) != REG_RD32(NV_PGRAPH_TSIZE(i))) ||
             ((REG_RD32(NV_PFB_TLIMIT(i)) & ~0x3FFF) != (REG_RD32(NV_PGRAPH_TLIMIT(i)) & ~0x3FFF)));

    //
    // Handle Z compression.
    //
    if (pFbAllocInfo->type == MEM_TYPE_DEPTH_COMPR16 || pFbAllocInfo->type == MEM_TYPE_DEPTH_COMPR32)
    {
        U032 offset, size;
       
        //
        // Find offset into ZTag table of range we'll associate with
        // this block of memory.
        //
        // Each Ztag maps to a single 64byte tile per partition.
        //
        size = pFbAllocInfo->size / (pFbHalPvtInfo->partitions * 64);
        if (_nvHalFbAllocZtags(pHalHwInfo, &size, &offset) == RM_OK)
        {
            // save off ztag region state
            pFbHalPvtInfo->zCompState[i].offset = offset;
            pFbHalPvtInfo->zCompState[i].size = size;

            // initialize offset into ztag pool
            data32 = offset | DRF_DEF(_PFB, _ZCOMP, _EN, _TRUE);

            // mode (16 or 32bit)
            if (pFbAllocInfo->type == MEM_TYPE_DEPTH_COMPR32)
                data32 |= DRF_DEF(_PFB, _ZCOMP, _MODE, _32);

            // enable big endian mode if applicable
            if (pHalHwInfo->pMcHalInfo->EndianControl & MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN)
                data32 |= DRF_DEF(_PFB, _ZCOMP, _ENDIAN, _BIG);

            //
            // TODO: enable antialiasing...
            //

            do {
                // Now, wait until graphics is idle
                HAL_GR_IDLE(pHalHwInfo);

                REG_WR32(NV_PFB_ZCOMP(i), data32);
                REG_WR32(NV_PGRAPH_ZCOMP(i), data32);
                RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                             NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(i),
                             data32);
            } while (REG_RD32(NV_PFB_ZCOMP(i)) != REG_RD32(NV_PGRAPH_ZCOMP(i)));
        }
    }
    else
    {
        //
        // Set ENDIAN-ness in zcomp register; required even if
        // compression isn't enabled.  If we don't set this correctly,
        // we can incur TILEVIOL exceptions.
        //
        data32 = REG_RD32(NV_PFB_ZCOMP(i));
        data32 &= ~(DRF_DEF(_PFB, _ZCOMP, _ENDIAN, _BIG));
        if (pHalHwInfo->pMcHalInfo->EndianControl & MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN)
            data32 |= DRF_DEF(_PFB, _ZCOMP, _ENDIAN, _BIG);
        REG_WR32(NV_PFB_ZCOMP(i), data32);
        REG_WR32(NV_PGRAPH_ZCOMP(i), data32);
        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(i),
                     data32);
    }


    // resume graphics, now that PFB/PGRAPH are consistent
    nvHalTilingUpdateResume_NV20(pHalHwInfo, dmacaches, dmapush);

    return (RM_OK);
}

RM_STATUS
nvHalFbLoadOverride_NV20(VOID *arg)
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
            REG_WR32(NV_PFB_CFG0, pFbLoadOverrideArg->value);
            break;
        case FB_LOAD_OVERRIDE_REMAP:
        {
            U032 i, *fb_sv;

            //
            // We're going to be restting the remapper register.
            // Before doing that, we need to save off the bios
            // image.
            //
            // We're ASSUMING that nothing else in the fb needs
            // to be saved here.
            //
            fb_sv = (U032 *)pHalHwInfo->pFbMallocBuffer;
            for(i=0; i < 0xA0000/4; i++)
                *fb_sv++ = REG_RD32(NV_PRAMIN_DATA032(i));

            REG_WR32(NV_PFB_REMAP, pFbLoadOverrideArg->value);

            fb_sv = (U032 *)pHalHwInfo->pFbMallocBuffer;
            for(i=0; i < 0xA0000/4; i++)
                REG_WR32(NV_PRAMIN_DATA032(i), *fb_sv++);

            break;
        }   
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}

//
// Z Compression management.
//
// Here's some details from the _HOW_TO_MAP_Z_COMPRESS_TAG_BITS_
// document in the NV20 Architecture database:
//
// (1) Z tag bits only live in compressed regions.
// (2) Only tiled regions can be compressed.
// (3) There is one Z tag bit per (64byte) packet.
// (4) One packet maps to one partition.
// (5) A ZCOMP_BASE_TAG_ADR exists for each region.  They define the tag
//     bit offset into each partition's tag RAM.  All RAMs in each partition
//     get the same address.
// (6) A region uses RAM tag bits starting at the ZCOMP_BASE_TAG_ADR.
//     RAM tag bits are allocated until the end of the memory region, or
//     until they run out, whichever is first.
// (7) ZCOMP_MAX_TAG_ADR specifies how many bits exist in each
//     partition tag RAM.
//
// There are two new memory types that the drivers can use to request
// compressed depth buffers:
//
//  MEM_TYPE_DEPTH_COMPR16 - 16bit compression
//  MEM_TYPE_DEPTH_COMPR32 - 32bit compression
//
// When the tiling parameters are programmed up for one of these
// memory types, we scan the Z tag pool (a bitmap) for any free
// bits.  Because of (6), we can partially compress a region, which
// we will do as a last resort.
//
static RM_STATUS
_nvHalFbAllocZtags(PHALHWINFO pHalHwInfo, U032 *size, U032 *offset)
{
    PFBHALINFO_NV20 pFbHalPvtInfo;
    U032 bestSize, bestOffset;
    U032 i, j;

    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;

    //
    // Error check incoming size parameter.
    //
    if (*size == 0)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: zero size to ztag allocator\n");
        *offset = MAX_ZTAG_BITS;                // out of range value
        DBG_BREAKPOINT();
    }

    bestSize = 0;
    bestOffset = MAX_ZTAG_BITS;
    for (i = 0; i < MAX_ZTAG_BITS; i++)
    {
        if (!pFbHalPvtInfo->zTagPool[i >> 3])
        {
            i += 7;
        }
        else if (pFbHalPvtInfo->zTagPool[i >> 3] & BIT(i & 7))
        {
            //
            // Search for string of 1's the length of the block.
            //
            j = 0;

            do
            {
                if ((++i >= MAX_ZTAG_BITS) || (++j >= *size))
                    break;
            }
            while (pFbHalPvtInfo->zTagPool[i >> 3] & BIT(i & 7));


            if (j == *size)
            {
                *offset = i - j;
                while (j--)
                {
                    i--;
                    pFbHalPvtInfo->zTagPool[i >> 3] &= ~BIT(i & 7);
                }
                return (RM_OK);
            }

            //
            // If this is the best range so far, save it off.
            //
            if (j != 0 && j > bestSize)
            {
                bestSize = j;
                bestOffset = i - j;
            }
        }
    }

    //
    // If we found _any_ ranges, use the biggest one...
    //
    if (bestSize != 0)
    {
        *offset = bestOffset;
        *size = bestSize;
        return (RM_OK);
    }

    *offset = MAX_ZTAG_BITS;
    *size = 0;

    return (RM_ERR_NO_FREE_MEM);
}


static RM_STATUS
_nvHalFbFreeZtags(PHALHWINFO pHalHwInfo, U032 offset, U032 size)
{
    PFBHALINFO_NV20 pFbHalPvtInfo;

    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;

    //
    // Validate range.
    //
    if (offset + size > MAX_ZTAG_BITS)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: bad ZTag pool parameters: offset ", offset);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: size ", size);
        DBG_BREAKPOINT();
        return (RM_ERROR);
    }

    //
    // Mark range free.
    //
    while (size--)
    {
        pFbHalPvtInfo->zTagPool[offset >> 3] |= BIT(offset & 7);
        offset++;
    }

    return (RM_OK);
}


static RM_STATUS fbDelay_NV20(
    PHALHWINFO pHalHwInfo,
    U032 nsec
)
{
    U032 TimeHi;
    U032 TimeLo;
    U032 TimeCheck;
    
    //
    // For small delays which can only wrap PTIMER_TIME_0 once, doing 2's
    // comp math on TIME_0 is enough and means we don't have to sync TIME_1
    //
#define MAX_SMALLNS_TMRDELAY (0xFFFFFFFF >> 2)

    if (nsec < MAX_SMALLNS_TMRDELAY)
    {
        U032 TimeStart = REG_RD32(NV_PTIMER_TIME_0);
        while (nsec > ((volatile U032)REG_RD32(NV_PTIMER_TIME_0) - TimeStart))
		    ;
        return (RM_OK);
    }

    //
    // Get current time.
    //
    do
    {
        TimeCheck = REG_RD32(NV_PTIMER_TIME_1);
        TimeLo    = REG_RD32(NV_PTIMER_TIME_0);
        TimeHi    = REG_RD32(NV_PTIMER_TIME_1);
    } while (TimeCheck != TimeHi);

    //
    // Add nanosecond delay.
    //
    TimeLo += nsec;
    if (TimeLo < nsec)
        TimeHi++;
    //
    // Wait until time catches up.
    //
    while (TimeHi > (U032)((volatile U032)REG_RD32(NV_PTIMER_TIME_1)))
        ;
    //
    // Try to avoid infinite delay.
    //
    while ((TimeLo > (U032)((volatile U032)REG_RD32(NV_PTIMER_TIME_0))) &&
           (TimeHi == (U032)((volatile U032)REG_RD32(NV_PTIMER_TIME_1))))
        ;

    return (RM_OK);
}

//
// The following is from Luke Chang.
//

// we use 12.20 fixed point for these calculations
// the -from- function also rounds and masks to 4 bits
#define TOFIXEDPOINT(d)         ((d) << 20)
#define FROMFIXEDPOINT(d)       ((((d) + (0x1 << 19)) >> 20) & 0xf)

//
// Initialize drive/slew parameters.
//
static VOID
_nvHalFbInitDrvSlwParms(PHALHWINFO pHalHwInfo)
{
    PBIOSDRVSLWPARMS pBiosDrvSlwParms;
    PFBHALINFO_NV20 pFbHalPvtInfo;
    PDRVSLWPARMS pDrvSlwParms;

    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;
    pDrvSlwParms = &pFbHalPvtInfo->drvSlwParms;

    //
    // Initialize static (extreme) values first.
    //
    pDrvSlwParms->SlowCountExt = SLOW_COUNT_EXT;
    pDrvSlwParms->FastCountExt = FAST_COUNT_EXT;

    pDrvSlwParms->SlowCountBExt = SLOW_COUNTB_EXT;
    pDrvSlwParms->FastCountBExt = FAST_COUNTB_EXT;

    // ********** EXTREME FAST Case Values
    pDrvSlwParms->AdrDrvFallFastExt = ADR_DRV_FALL_FAST_EXT;
    pDrvSlwParms->AdrDrvRiseFastExt = ADR_DRV_RISE_FAST_EXT;
    pDrvSlwParms->AdrSlwFallFastExt = ADR_SLW_FALL_FAST_EXT;
    pDrvSlwParms->AdrSlwRiseFastExt = ADR_SLW_RISE_FAST_EXT;

    pDrvSlwParms->ClkDrvFallFastExt = CLK_DRV_FALL_FAST_EXT;
    pDrvSlwParms->ClkDrvRiseFastExt = CLK_DRV_RISE_FAST_EXT;
    pDrvSlwParms->ClkSlwFallFastExt = CLK_SLW_FALL_FAST_EXT;
    pDrvSlwParms->ClkSlwRiseFastExt = CLK_SLW_RISE_FAST_EXT;

    pDrvSlwParms->DatDrvFallFastExt = DAT_DRV_FALL_FAST_EXT;
    pDrvSlwParms->DatDrvRiseFastExt = DAT_DRV_RISE_FAST_EXT;
    pDrvSlwParms->DatSlwFallFastExt = DAT_SLW_FALL_FAST_EXT;
    pDrvSlwParms->DatSlwRiseFastExt = DAT_SLW_RISE_FAST_EXT;

    pDrvSlwParms->DqsDrvFallFastExt = DQS_DRV_FALL_FAST_EXT;
    pDrvSlwParms->DqsDrvRiseFastExt = DQS_DRV_RISE_FAST_EXT;
    pDrvSlwParms->DqsSlwFallFastExt = DQS_SLW_FALL_FAST_EXT;
    pDrvSlwParms->DqsSlwRiseFastExt = DQS_SLW_RISE_FAST_EXT;

    pDrvSlwParms->DqsInbDelyFastExt = DQS_INB_DELY_FAST_EXT;

    // ********** EXTREME SLOW Case Values
    pDrvSlwParms->AdrDrvFallSlowExt = ADR_DRV_FALL_SLOW_EXT;
    pDrvSlwParms->AdrDrvRiseSlowExt = ADR_DRV_RISE_SLOW_EXT;
    pDrvSlwParms->AdrSlwFallSlowExt = ADR_SLW_FALL_SLOW_EXT;
    pDrvSlwParms->AdrSlwRiseSlowExt = ADR_SLW_RISE_SLOW_EXT;

    pDrvSlwParms->ClkDrvFallSlowExt = CLK_DRV_FALL_SLOW_EXT;
    pDrvSlwParms->ClkDrvRiseSlowExt = CLK_DRV_RISE_SLOW_EXT;
    pDrvSlwParms->ClkSlwFallSlowExt = CLK_SLW_FALL_SLOW_EXT;
    pDrvSlwParms->ClkSlwRiseSlowExt = CLK_SLW_RISE_SLOW_EXT;

    pDrvSlwParms->DatDrvFallSlowExt = DAT_DRV_FALL_SLOW_EXT;
    pDrvSlwParms->DatDrvRiseSlowExt = DAT_DRV_RISE_SLOW_EXT;
    pDrvSlwParms->DatSlwFallSlowExt = DAT_SLW_FALL_SLOW_EXT;
    pDrvSlwParms->DatSlwRiseSlowExt = DAT_SLW_RISE_SLOW_EXT;

    pDrvSlwParms->DqsDrvFallSlowExt = DQS_DRV_FALL_SLOW_EXT;
    pDrvSlwParms->DqsDrvRiseSlowExt = DQS_DRV_RISE_SLOW_EXT;
    pDrvSlwParms->DqsSlwFallSlowExt = DQS_SLW_FALL_SLOW_EXT;
    pDrvSlwParms->DqsSlwRiseSlowExt = DQS_SLW_RISE_SLOW_EXT;

    pDrvSlwParms->DqsInbDelySlowExt = DQS_INB_DELY_SLOW_EXT;

    //
    // Now initialize average and typical values.
    //
    // If we've got BIOS bip3 data that we're familiar with, use it.
    // Otherwise, fall back to static data.
    //
    // We also check the silicon rev (at least temporarily) to make
    // sure we don't use the drvslwparms table on an A02.  This could
    // happen if a newer bios was mis-flashed on the part.
    //
    pBiosDrvSlwParms = (PBIOSDRVSLWPARMS)pHalHwInfo->pFbHalInfo->pBiosDrvSlwParms;
    if (!IsNV20MaskRevA02_NV20(pHalHwInfo->pMcHalInfo) &&
        (pBiosDrvSlwParms != NULL && pBiosDrvSlwParms->Version >= 0x11))
    {
        //
        // Cool...pull it from the bios.
        //
        pDrvSlwParms->MaxMClk = (U016)pBiosDrvSlwParms->MaxMClk;
        pDrvSlwParms->SlowCountAvg = (U008)pBiosDrvSlwParms->SlowCountAvg;
        pDrvSlwParms->SlowCountBAvg = (U008)pBiosDrvSlwParms->SlowCountBAvg;
        pDrvSlwParms->TypiCount = (U008)pBiosDrvSlwParms->TypiCount;
        pDrvSlwParms->TypiCountB = (U008)pBiosDrvSlwParms->TypiCountB;
        pDrvSlwParms->FastCountAvg = (U008)pBiosDrvSlwParms->FastCountAvg;
        pDrvSlwParms->FastCountBAvg = (U008)pBiosDrvSlwParms->FastCountBAvg;
        pDrvSlwParms->FbioInterpMode = (U008)pBiosDrvSlwParms->FbioInterpMode;

        // ********** Average FAST Case Values
        pDrvSlwParms->AdrDrvFallFastAvg = (U008)pBiosDrvSlwParms->AdrDrvFallFastAvg;
        pDrvSlwParms->AdrDrvRiseFastAvg = (U008)pBiosDrvSlwParms->AdrDrvRiseFastAvg;
        pDrvSlwParms->AdrSlwFallFastAvg = (U008)pBiosDrvSlwParms->AdrSlwFallFastAvg;
        pDrvSlwParms->AdrSlwRiseFastAvg = (U008)pBiosDrvSlwParms->AdrSlwRiseFastAvg;

        pDrvSlwParms->ClkDrvFallFastAvg = (U008)pBiosDrvSlwParms->ClkDrvFallFastAvg;
        pDrvSlwParms->ClkDrvRiseFastAvg = (U008)pBiosDrvSlwParms->ClkDrvRiseFastAvg;
        pDrvSlwParms->ClkSlwFallFastAvg = (U008)pBiosDrvSlwParms->ClkSlwFallFastAvg;
        pDrvSlwParms->ClkSlwRiseFastAvg = (U008)pBiosDrvSlwParms->ClkSlwRiseFastAvg;

        pDrvSlwParms->DatDrvFallFastAvg = (U008)pBiosDrvSlwParms->DatDrvFallFastAvg;
        pDrvSlwParms->DatDrvRiseFastAvg = (U008)pBiosDrvSlwParms->DatDrvRiseFastAvg;
        pDrvSlwParms->DatSlwFallFastAvg = (U008)pBiosDrvSlwParms->DatSlwFallFastAvg;
        pDrvSlwParms->DatSlwRiseFastAvg = (U008)pBiosDrvSlwParms->DatSlwRiseFastAvg;

        pDrvSlwParms->DqsDrvFallFastAvg = (U008)pBiosDrvSlwParms->DqsDrvFallFastAvg;
        pDrvSlwParms->DqsDrvRiseFastAvg = (U008)pBiosDrvSlwParms->DqsDrvRiseFastAvg;
        pDrvSlwParms->DqsSlwFallFastAvg = (U008)pBiosDrvSlwParms->DqsSlwFallFastAvg;
        pDrvSlwParms->DqsSlwRiseFastAvg = (U008)pBiosDrvSlwParms->DqsSlwRiseFastAvg;

        pDrvSlwParms->DqsInbDelyFastAvg = (U008)pBiosDrvSlwParms->DqsInbDelyFastAvg;

        // ********** TYPICAL Case Values
        pDrvSlwParms->AdrDrvFallTypi = (U008)pBiosDrvSlwParms->AdrDrvFallTypi;
        pDrvSlwParms->AdrDrvRiseTypi = (U008)pBiosDrvSlwParms->AdrDrvRiseTypi;
        pDrvSlwParms->AdrSlwFallTypi = (U008)pBiosDrvSlwParms->AdrSlwFallTypi;
        pDrvSlwParms->AdrSlwRiseTypi = (U008)pBiosDrvSlwParms->AdrSlwRiseTypi;

        pDrvSlwParms->ClkDrvFallTypi = (U008)pBiosDrvSlwParms->ClkDrvFallTypi;
        pDrvSlwParms->ClkDrvRiseTypi = (U008)pBiosDrvSlwParms->ClkDrvRiseTypi;
        pDrvSlwParms->ClkSlwFallTypi = (U008)pBiosDrvSlwParms->ClkSlwFallTypi;
        pDrvSlwParms->ClkSlwRiseTypi = (U008)pBiosDrvSlwParms->ClkSlwRiseTypi;

        pDrvSlwParms->DatDrvFallTypi = (U008)pBiosDrvSlwParms->DatDrvFallTypi;
        pDrvSlwParms->DatDrvRiseTypi = (U008)pBiosDrvSlwParms->DatDrvRiseTypi;
        pDrvSlwParms->DatSlwFallTypi = (U008)pBiosDrvSlwParms->DatSlwFallTypi;
        pDrvSlwParms->DatSlwRiseTypi = (U008)pBiosDrvSlwParms->DatSlwRiseTypi;

        pDrvSlwParms->DqsDrvFallTypi = (U008)pBiosDrvSlwParms->DqsDrvFallTypi;
        pDrvSlwParms->DqsDrvRiseTypi = (U008)pBiosDrvSlwParms->DqsDrvRiseTypi;
        pDrvSlwParms->DqsSlwFallTypi = (U008)pBiosDrvSlwParms->DqsSlwFallTypi;
        pDrvSlwParms->DqsSlwRiseTypi = (U008)pBiosDrvSlwParms->DqsSlwRiseTypi;

        pDrvSlwParms->DqsInbDelyTypi = (U008)pBiosDrvSlwParms->DqsInbDelyTypi;

        // ********** Average SLOW Case Values
        pDrvSlwParms->AdrDrvFallSlowAvg = (U008)pBiosDrvSlwParms->AdrDrvFallSlowAvg;
        pDrvSlwParms->AdrDrvRiseSlowAvg = (U008)pBiosDrvSlwParms->AdrDrvRiseSlowAvg;
        pDrvSlwParms->AdrSlwFallSlowAvg = (U008)pBiosDrvSlwParms->AdrSlwFallSlowAvg;
        pDrvSlwParms->AdrSlwRiseSlowAvg = (U008)pBiosDrvSlwParms->AdrSlwRiseSlowAvg;

        pDrvSlwParms->ClkDrvFallSlowAvg = (U008)pBiosDrvSlwParms->ClkDrvFallSlowAvg;
        pDrvSlwParms->ClkDrvRiseSlowAvg = (U008)pBiosDrvSlwParms->ClkDrvRiseSlowAvg;
        pDrvSlwParms->ClkSlwFallSlowAvg = (U008)pBiosDrvSlwParms->ClkSlwFallSlowAvg;
        pDrvSlwParms->ClkSlwRiseSlowAvg = (U008)pBiosDrvSlwParms->ClkSlwRiseSlowAvg;

        pDrvSlwParms->DatDrvFallSlowAvg = (U008)pBiosDrvSlwParms->DatDrvFallSlowAvg;
        pDrvSlwParms->DatDrvRiseSlowAvg = (U008)pBiosDrvSlwParms->DatDrvRiseSlowAvg;
        pDrvSlwParms->DatSlwFallSlowAvg = (U008)pBiosDrvSlwParms->DatSlwFallSlowAvg;
        pDrvSlwParms->DatSlwRiseSlowAvg = (U008)pBiosDrvSlwParms->DatSlwRiseSlowAvg;

        pDrvSlwParms->DqsDrvFallSlowAvg = (U008)pBiosDrvSlwParms->DqsDrvFallSlowAvg;
        pDrvSlwParms->DqsDrvRiseSlowAvg = (U008)pBiosDrvSlwParms->DqsDrvRiseSlowAvg;
        pDrvSlwParms->DqsSlwFallSlowAvg = (U008)pBiosDrvSlwParms->DqsSlwFallSlowAvg;
        pDrvSlwParms->DqsSlwRiseSlowAvg = (U008)pBiosDrvSlwParms->DqsSlwRiseSlowAvg;

        pDrvSlwParms->DqsInbDelySlowAvg = (U008)pBiosDrvSlwParms->DqsInbDelySlowAvg;
    }
    else
    {
        //
        // Go with static table data.
        //
        pDrvSlwParms->MaxMClk = 20000;

        pDrvSlwParms->SlowCountAvg = SLOW_COUNT_AVG;
        pDrvSlwParms->SlowCountBAvg = SLOW_COUNTB_AVG;
        pDrvSlwParms->TypiCount = TYPI_COUNT;
        pDrvSlwParms->TypiCountB = TYPI_COUNTB;
        pDrvSlwParms->FastCountAvg = FAST_COUNT_AVG;
        pDrvSlwParms->FastCountBAvg = FAST_COUNTB_AVG;

        pDrvSlwParms->FbioInterpMode = FBIO_INTERP_MODE;

        // ********** Average FAST Case Values
        pDrvSlwParms->AdrDrvFallFastAvg = ADR_DRV_FALL_FAST_AVG;
        pDrvSlwParms->AdrDrvRiseFastAvg = ADR_DRV_RISE_FAST_AVG;
        pDrvSlwParms->AdrSlwFallFastAvg = ADR_SLW_FALL_FAST_AVG;
        pDrvSlwParms->AdrSlwRiseFastAvg = ADR_SLW_RISE_FAST_AVG;

        pDrvSlwParms->ClkDrvFallFastAvg = CLK_DRV_FALL_FAST_AVG;
        pDrvSlwParms->ClkDrvRiseFastAvg = CLK_DRV_RISE_FAST_AVG;
        pDrvSlwParms->ClkSlwFallFastAvg = CLK_SLW_FALL_FAST_AVG;
        pDrvSlwParms->ClkSlwRiseFastAvg = CLK_SLW_RISE_FAST_AVG;

        pDrvSlwParms->DatDrvFallFastAvg = DAT_DRV_FALL_FAST_AVG;
        pDrvSlwParms->DatDrvRiseFastAvg = DAT_DRV_RISE_FAST_AVG;
        pDrvSlwParms->DatSlwFallFastAvg = DAT_SLW_FALL_FAST_AVG;
        pDrvSlwParms->DatSlwRiseFastAvg = DAT_SLW_RISE_FAST_AVG;

        pDrvSlwParms->DqsDrvFallFastAvg = DQS_DRV_FALL_FAST_AVG;
        pDrvSlwParms->DqsDrvRiseFastAvg = DQS_DRV_RISE_FAST_AVG;
        pDrvSlwParms->DqsSlwFallFastAvg = DQS_SLW_FALL_FAST_AVG;
        pDrvSlwParms->DqsSlwRiseFastAvg = DQS_SLW_RISE_FAST_AVG;

        pDrvSlwParms->DqsInbDelyFastAvg = DQS_INB_DELY_FAST_AVG;

        // ********** TYPICAL Case Values
        pDrvSlwParms->AdrDrvFallTypi = ADR_DRV_FALL_TYPI;
        pDrvSlwParms->AdrDrvRiseTypi = ADR_DRV_RISE_TYPI;
        pDrvSlwParms->AdrSlwFallTypi = ADR_SLW_FALL_TYPI;
        pDrvSlwParms->AdrSlwRiseTypi = ADR_SLW_RISE_TYPI;

        pDrvSlwParms->ClkDrvFallTypi = CLK_DRV_FALL_TYPI;
        pDrvSlwParms->ClkDrvRiseTypi = CLK_DRV_RISE_TYPI;
        pDrvSlwParms->ClkSlwFallTypi = CLK_SLW_FALL_TYPI;
        pDrvSlwParms->ClkSlwRiseTypi = CLK_SLW_RISE_TYPI;

        pDrvSlwParms->DatDrvFallTypi = DAT_DRV_FALL_TYPI;
        pDrvSlwParms->DatDrvRiseTypi = DAT_DRV_RISE_TYPI;
        pDrvSlwParms->DatSlwFallTypi = DAT_SLW_FALL_TYPI;
        pDrvSlwParms->DatSlwRiseTypi = DAT_SLW_RISE_TYPI;

        pDrvSlwParms->DqsDrvFallTypi = DQS_DRV_FALL_TYPI;
        pDrvSlwParms->DqsDrvRiseTypi = DQS_DRV_RISE_TYPI;
        pDrvSlwParms->DqsSlwFallTypi = DQS_SLW_FALL_TYPI;
        pDrvSlwParms->DqsSlwRiseTypi = DQS_SLW_RISE_TYPI;

        pDrvSlwParms->DqsInbDelyTypi = DQS_INB_DELY_TYPI;

        // ********** Average SLOW Case Values
        pDrvSlwParms->AdrDrvFallSlowAvg = ADR_DRV_FALL_SLOW_AVG;
        pDrvSlwParms->AdrDrvRiseSlowAvg = ADR_DRV_RISE_SLOW_AVG;
        pDrvSlwParms->AdrSlwFallSlowAvg = ADR_SLW_FALL_SLOW_AVG;
        pDrvSlwParms->AdrSlwRiseSlowAvg = ADR_SLW_RISE_SLOW_AVG;

        pDrvSlwParms->ClkDrvFallSlowAvg = CLK_DRV_FALL_SLOW_AVG;
        pDrvSlwParms->ClkDrvRiseSlowAvg = CLK_DRV_RISE_SLOW_AVG;
        pDrvSlwParms->ClkSlwFallSlowAvg = CLK_SLW_FALL_SLOW_AVG;
        pDrvSlwParms->ClkSlwRiseSlowAvg = CLK_SLW_RISE_SLOW_AVG;

        pDrvSlwParms->DatDrvFallSlowAvg = DAT_DRV_FALL_SLOW_AVG;
        pDrvSlwParms->DatDrvRiseSlowAvg = DAT_DRV_RISE_SLOW_AVG;
        pDrvSlwParms->DatSlwFallSlowAvg = DAT_SLW_FALL_SLOW_AVG;
        pDrvSlwParms->DatSlwRiseSlowAvg = DAT_SLW_RISE_SLOW_AVG;

        pDrvSlwParms->DqsDrvFallSlowAvg = DQS_DRV_FALL_SLOW_AVG;
        pDrvSlwParms->DqsDrvRiseSlowAvg = DQS_DRV_RISE_SLOW_AVG;
        pDrvSlwParms->DqsSlwFallSlowAvg = DQS_SLW_FALL_SLOW_AVG;
        pDrvSlwParms->DqsSlwRiseSlowAvg = DQS_SLW_RISE_SLOW_AVG;

        pDrvSlwParms->DqsInbDelySlowAvg = DQS_INB_DELY_SLOW_AVG;
    }
}

// **** Setup optimal drive and slew based on operating condition
// **** For oeprating conditions outside slow and fast border, just use the
// **** slow and fast settings. For values in between, use linear 
// **** interpolation between the settings.
//
VOID
nvHalFbCalcDrvSlw_NV20(PHALHWINFO pHalHwInfo)
{
    PFBHALINFO_NV20 pFbHalPvtInfo;
    PDRVSLWPARMS pDrvSlwParms;
    U032 count0, count1, count2, count3, acount, bcount;
    U032 adr_drv_fall_lo=0, adr_drv_rise_lo=0,
        clk_drv_fall_lo=0, clk_drv_rise_lo=0,
        dat_drv_fall_lo=0, dat_drv_rise_lo=0,
        dqs_drv_fall_lo=0, dqs_drv_rise_lo=0;

    U032 adr_drv_fall_hi=0, adr_drv_rise_hi=0,
        clk_drv_fall_hi=0, clk_drv_rise_hi=0,
        dat_drv_fall_hi=0, dat_drv_rise_hi=0,
        dqs_drv_fall_hi=0, dqs_drv_rise_hi=0;
 
    U032 adr_drv_fall=0, adr_drv_rise=0,
        clk_drv_fall=0, clk_drv_rise=0,
        dat_drv_fall=0, dat_drv_rise=0,
        dqs_drv_fall=0, dqs_drv_rise=0;

    U032 adr_drv, clk_drv, dat_drv, dqs_drv;

    U032 adr_slw_fall_lo=0, adr_slw_rise_lo=0,
        clk_slw_fall_lo=0, clk_slw_rise_lo=0,
        dat_slw_fall_lo=0, dat_slw_rise_lo=0,
        dqs_slw_fall_lo=0, dqs_slw_rise_lo=0;

    U032 adr_slw_fall_hi=0, adr_slw_rise_hi=0,
        clk_slw_fall_hi=0, clk_slw_rise_hi=0,
        dat_slw_fall_hi=0, dat_slw_rise_hi=0,
        dqs_slw_fall_hi=0, dqs_slw_rise_hi=0;

    U032 adr_slw_fall=0, adr_slw_rise=0,
        clk_slw_fall=0, clk_slw_rise=0,
        dat_slw_fall=0, dat_slw_rise=0,
        dqs_slw_fall=0, dqs_slw_rise=0;

    U032 adr_slw, clk_slw, dat_slw, dqs_slw;

    U032 dqs_ib_del_lo=0, dqs_ib_del_hi=0, dqs_ib_del=0;
    U032 dqs_ib;

    U032 ratio_a, ratio_b;
    U032 ratio2_a, ratio2_b;

    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;
    pDrvSlwParms = &pFbHalPvtInfo->drvSlwParms;

#define NV_PBUS_FBIO_CALEN_VAL          0x000f0f0f
#define NV_PBUS_FBIO_CALSEL_VAL         0x00040004
#define NV_PBUS_FBIO_CALSELB_VAL        0x00040000

    //******* Figure out which operating condition we are in  
    REG_WR32(NV_PBUS_FBIO_CALEN, NV_PBUS_FBIO_CALEN_VAL);

    REG_WR32(NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VAL);
    fbDelay_NV20(pHalHwInfo, 10*1000);
    count0 = REG_RD32(NV_PBUS_FBIO_CALCNT);

    REG_WR32(NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VAL+1);
    // ****** need some delay here (~10 us ms) here
    fbDelay_NV20(pHalHwInfo, 10*1000);
    count1 = REG_RD32(NV_PBUS_FBIO_CALCNT);

    REG_WR32(NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VAL+2);
    // ****** need some delay here (~10 us ms)
    fbDelay_NV20(pHalHwInfo, 10*1000);
    count2 = REG_RD32(NV_PBUS_FBIO_CALCNT);

    REG_WR32(NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VAL+3);
    // ****** need some delay here (~10 us ms)
    fbDelay_NV20(pHalHwInfo, 10*1000);
    count3 = REG_RD32(NV_PBUS_FBIO_CALCNT);

    acount = (count0 + count1 + count2 + count3) / 4;
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "drvslw_nv20: acount ", acount);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "             count0 ", count0);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "             count1 ", count1);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "             count2 ", count2);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "             count3 ", count3);

    // ******* New Code - 2/16/01 **********
    // ******* Get VDD speedometers for FBIO_DLY calibration
    REG_WR32(NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSELB_VAL);
    fbDelay_NV20(pHalHwInfo, 10*1000);
    count0 = REG_RD32(NV_PBUS_FBIO_CALCNT);

    REG_WR32(NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSELB_VAL+1);
    fbDelay_NV20(pHalHwInfo, 10*1000);
    count1 = REG_RD32(NV_PBUS_FBIO_CALCNT);

    REG_WR32(NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSELB_VAL+2);
    fbDelay_NV20(pHalHwInfo, 10*1000);
    count2 = REG_RD32(NV_PBUS_FBIO_CALCNT);

    REG_WR32(NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSELB_VAL+3);
    fbDelay_NV20(pHalHwInfo, 10*1000);
    count3 = REG_RD32(NV_PBUS_FBIO_CALCNT);

    bcount = (count0 + count1 + count2 + count3) / 4;
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "drvslw_nv20: bcount ", bcount);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "             count0 ", count0);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "             count1 ", count1);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "             count2 ", count2);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "             count3 ", count3);

    //******** Shut off oscillators to reduce power and noise
    REG_WR32(NV_PBUS_FBIO_CALEN, 0);

    //
    // If these remain 0, then we'll assume that no interpolation
    // is required.
    //
    ratio_a = ratio_b = 0;
    ratio2_a = ratio2_b = 0;

    if ((acount <= pDrvSlwParms->SlowCountAvg) && (pDrvSlwParms->FbioInterpMode == 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " slow case - clamp to extremes\n");

        adr_drv_fall = pDrvSlwParms->AdrDrvFallSlowExt;			
        adr_drv_rise = pDrvSlwParms->AdrDrvRiseSlowExt;
        adr_slw_fall = pDrvSlwParms->AdrSlwFallSlowExt;			
        adr_slw_rise = pDrvSlwParms->AdrSlwRiseSlowExt;

        clk_drv_fall = pDrvSlwParms->ClkDrvFallSlowExt;			
        clk_drv_rise = pDrvSlwParms->ClkDrvRiseSlowExt;
        clk_slw_fall = pDrvSlwParms->ClkSlwFallSlowExt;			
        clk_slw_rise = pDrvSlwParms->ClkSlwRiseSlowExt;
				
        dat_drv_fall = pDrvSlwParms->DatDrvFallSlowExt;			
        dat_drv_rise = pDrvSlwParms->DatDrvRiseSlowExt;
        dat_slw_fall = pDrvSlwParms->DatSlwFallSlowExt;			
        dat_slw_rise = pDrvSlwParms->DatSlwRiseSlowExt;

        dqs_drv_fall = pDrvSlwParms->DqsDrvFallSlowExt;			
        dqs_drv_rise = pDrvSlwParms->DqsDrvRiseSlowExt;
        dqs_slw_fall = pDrvSlwParms->DqsSlwFallSlowExt;			
        dqs_slw_rise = pDrvSlwParms->DqsSlwRiseSlowExt;
    }
    else if ((acount <= pDrvSlwParms->SlowCountAvg) && (pDrvSlwParms->FbioInterpMode == 1))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " slow case - clamp to averages\n");
        adr_drv_fall = pDrvSlwParms->AdrDrvFallSlowAvg;			
        adr_drv_rise = pDrvSlwParms->AdrDrvRiseSlowAvg;
        adr_slw_fall = pDrvSlwParms->AdrSlwFallSlowAvg;			
        adr_slw_rise = pDrvSlwParms->AdrSlwRiseSlowAvg;

        clk_drv_fall = pDrvSlwParms->ClkDrvFallSlowAvg;			
        clk_drv_rise = pDrvSlwParms->ClkDrvRiseSlowAvg;
        clk_slw_fall = pDrvSlwParms->ClkSlwFallSlowAvg;			
        clk_slw_rise = pDrvSlwParms->ClkSlwRiseSlowAvg;
				
        dat_drv_fall = pDrvSlwParms->DatDrvFallSlowAvg;			
        dat_drv_rise = pDrvSlwParms->DatDrvRiseSlowAvg;
        dat_slw_fall = pDrvSlwParms->DatSlwFallSlowAvg;			
        dat_slw_rise = pDrvSlwParms->DatSlwRiseSlowAvg;

        dqs_drv_fall = pDrvSlwParms->DqsDrvFallSlowAvg;			
        dqs_drv_rise = pDrvSlwParms->DqsDrvRiseSlowAvg;
        dqs_slw_fall = pDrvSlwParms->DqsSlwFallSlowAvg;			
        dqs_slw_rise = pDrvSlwParms->DqsSlwRiseSlowAvg;
    }
    else if ((acount <= pDrvSlwParms->SlowCountAvg) && (pDrvSlwParms->FbioInterpMode == 2))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " slow case - interp avg and extreme\n");

        adr_drv_fall_lo = pDrvSlwParms->AdrDrvFallSlowExt;			
        adr_drv_rise_lo = pDrvSlwParms->AdrDrvRiseSlowExt;
        adr_slw_fall_lo = pDrvSlwParms->AdrSlwFallSlowExt;			
        adr_slw_rise_lo = pDrvSlwParms->AdrSlwRiseSlowExt;

        clk_drv_fall_lo = pDrvSlwParms->ClkDrvFallSlowExt;			
        clk_drv_rise_lo = pDrvSlwParms->ClkDrvRiseSlowExt;
        clk_slw_fall_lo = pDrvSlwParms->ClkSlwFallSlowExt;			
        clk_slw_rise_lo = pDrvSlwParms->ClkSlwRiseSlowExt;
				
        dat_drv_fall_lo = pDrvSlwParms->DatDrvFallSlowExt;			
        dat_drv_rise_lo = pDrvSlwParms->DatDrvRiseSlowExt;
        dat_slw_fall_lo = pDrvSlwParms->DatSlwFallSlowExt;			
        dat_slw_rise_lo = pDrvSlwParms->DatSlwRiseSlowExt;

        dqs_drv_fall_lo = pDrvSlwParms->DqsDrvFallSlowExt;			
        dqs_drv_rise_lo = pDrvSlwParms->DqsDrvRiseSlowExt;
        dqs_slw_fall_lo = pDrvSlwParms->DqsSlwFallSlowExt;			
        dqs_slw_rise_lo = pDrvSlwParms->DqsSlwRiseSlowExt;

        adr_drv_fall_hi = pDrvSlwParms->AdrDrvFallSlowAvg;
        adr_drv_rise_hi = pDrvSlwParms->AdrDrvRiseSlowAvg;
        adr_slw_fall_hi = pDrvSlwParms->AdrSlwFallSlowAvg;
        adr_slw_rise_hi = pDrvSlwParms->AdrSlwRiseSlowAvg;

        clk_drv_fall_hi = pDrvSlwParms->ClkDrvFallSlowAvg;
        clk_drv_rise_hi = pDrvSlwParms->ClkDrvRiseSlowAvg;
        clk_slw_fall_hi = pDrvSlwParms->ClkSlwFallSlowAvg;
        clk_slw_rise_hi = pDrvSlwParms->ClkSlwRiseSlowAvg;
				
        dat_drv_fall_hi = pDrvSlwParms->DatDrvFallSlowAvg;
        dat_drv_rise_hi = pDrvSlwParms->DatDrvRiseSlowAvg;
        dat_slw_fall_hi = pDrvSlwParms->DatSlwFallSlowAvg;
        dat_slw_rise_hi = pDrvSlwParms->DatSlwRiseSlowAvg;

        dqs_drv_fall_hi = pDrvSlwParms->DqsDrvFallSlowAvg;
        dqs_drv_rise_hi = pDrvSlwParms->DqsDrvRiseSlowAvg;
        dqs_slw_fall_hi = pDrvSlwParms->DqsSlwFallSlowAvg;			
        dqs_slw_rise_hi = pDrvSlwParms->DqsSlwRiseSlowAvg;

        //ratio_a = (acount - SLOW_COUNT_EXT) / (SLOW_COUNT_AVG - SLOW_COUNT_EXT);
        //if (ratio_a < 0)
        //    ratio_a = 0;
        //ratio_b = 1.0 - ratio_a;
        ratio_a = TOFIXEDPOINT(acount - pDrvSlwParms->SlowCountExt) / (pDrvSlwParms->SlowCountAvg - pDrvSlwParms->SlowCountExt);
        if (ratio_a & 0x80000000)       // good test for < 0?!?
            ratio_a = 0;
        ratio_b = TOFIXEDPOINT(1) - ratio_a;
    }
    else if ((acount >= pDrvSlwParms->FastCountAvg) && (pDrvSlwParms->FbioInterpMode == 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " fast case - clamp to extremes\n");

        adr_drv_fall = pDrvSlwParms->AdrDrvFallFastExt;
        adr_drv_rise = pDrvSlwParms->AdrDrvRiseFastExt;
        adr_slw_fall = pDrvSlwParms->AdrSlwFallFastExt;
        adr_slw_rise = pDrvSlwParms->AdrSlwRiseFastExt;

        clk_drv_fall = pDrvSlwParms->ClkDrvFallFastExt;
        clk_drv_rise = pDrvSlwParms->ClkDrvRiseFastExt;
        clk_slw_fall = pDrvSlwParms->ClkSlwFallFastExt;
        clk_slw_rise = pDrvSlwParms->ClkSlwRiseFastExt;
				
        dat_drv_fall = pDrvSlwParms->DatDrvFallFastExt;
        dat_drv_rise = pDrvSlwParms->DatDrvRiseFastExt;
        dat_slw_fall = pDrvSlwParms->DatSlwFallFastExt;
        dat_slw_rise = pDrvSlwParms->DatSlwRiseFastExt;

        dqs_drv_fall = pDrvSlwParms->DqsDrvFallFastExt;
        dqs_drv_rise = pDrvSlwParms->DqsDrvRiseFastExt;
        dqs_slw_fall = pDrvSlwParms->DqsSlwFallFastExt;
        dqs_slw_rise = pDrvSlwParms->DqsSlwRiseFastExt;
    }
    else if ((acount >= pDrvSlwParms->FastCountAvg) && (pDrvSlwParms->FbioInterpMode == 1))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " fast case - clamp to averages\n");

        adr_drv_fall = pDrvSlwParms->AdrDrvFallFastAvg;
        adr_drv_rise = pDrvSlwParms->AdrDrvRiseFastAvg;
        adr_slw_fall = pDrvSlwParms->AdrSlwFallFastAvg;
        adr_slw_rise = pDrvSlwParms->AdrSlwRiseFastAvg;

        clk_drv_fall = pDrvSlwParms->ClkDrvFallFastAvg;
        clk_drv_rise = pDrvSlwParms->ClkDrvRiseFastAvg;
        clk_slw_fall = pDrvSlwParms->ClkSlwFallFastAvg;
        clk_slw_rise = pDrvSlwParms->ClkSlwRiseFastAvg;
				
        dat_drv_fall = pDrvSlwParms->DatDrvFallFastAvg;
        dat_drv_rise = pDrvSlwParms->DatDrvRiseFastAvg;
        dat_slw_fall = pDrvSlwParms->DatSlwFallFastAvg;
        dat_slw_rise = pDrvSlwParms->DatSlwRiseFastAvg;

        dqs_drv_fall = pDrvSlwParms->DqsDrvFallFastAvg;
        dqs_drv_rise = pDrvSlwParms->DqsDrvRiseFastAvg;
        dqs_slw_fall = pDrvSlwParms->DqsSlwFallFastAvg;
        dqs_slw_rise = pDrvSlwParms->DqsSlwRiseFastAvg;
    }
    else if ((acount >= pDrvSlwParms->FastCountAvg) && (pDrvSlwParms->FbioInterpMode == 2))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " fast case - interp avg and extreme\n");

        adr_drv_fall_lo = pDrvSlwParms->AdrDrvFallFastAvg;			
        adr_drv_rise_lo = pDrvSlwParms->AdrDrvRiseFastAvg;
        adr_slw_fall_lo = pDrvSlwParms->AdrSlwFallFastAvg;			
        adr_slw_rise_lo = pDrvSlwParms->AdrSlwRiseFastAvg;

        clk_drv_fall_lo = pDrvSlwParms->ClkDrvFallFastAvg;			
        clk_drv_rise_lo = pDrvSlwParms->ClkDrvRiseFastAvg;
        clk_slw_fall_lo = pDrvSlwParms->ClkSlwFallFastAvg;			
        clk_slw_rise_lo = pDrvSlwParms->ClkSlwRiseFastAvg;
				
        dat_drv_fall_lo = pDrvSlwParms->DatDrvFallFastAvg;			
        dat_drv_rise_lo = pDrvSlwParms->DatDrvRiseFastAvg;
        dat_slw_fall_lo = pDrvSlwParms->DatSlwFallFastAvg;			
        dat_slw_rise_lo = pDrvSlwParms->DatSlwRiseFastAvg;

        dqs_drv_fall_lo = pDrvSlwParms->DqsDrvFallFastAvg;			
        dqs_drv_rise_lo = pDrvSlwParms->DqsDrvRiseFastAvg;
        dqs_slw_fall_lo = pDrvSlwParms->DqsSlwFallFastAvg;			
        dqs_slw_rise_lo = pDrvSlwParms->DqsSlwRiseFastAvg;

        adr_drv_fall_hi = pDrvSlwParms->AdrDrvFallFastExt;
        adr_drv_rise_hi = pDrvSlwParms->AdrDrvRiseFastExt;
        adr_slw_fall_hi = pDrvSlwParms->AdrSlwFallFastExt;
        adr_slw_rise_hi = pDrvSlwParms->AdrSlwRiseFastExt;

        clk_drv_fall_hi = pDrvSlwParms->ClkDrvFallFastExt;
        clk_drv_rise_hi = pDrvSlwParms->ClkDrvRiseFastExt;
        clk_slw_fall_hi = pDrvSlwParms->ClkSlwFallFastExt;
        clk_slw_rise_hi = pDrvSlwParms->ClkSlwRiseFastExt;
				
        dat_drv_fall_hi = pDrvSlwParms->DatDrvFallFastExt;
        dat_drv_rise_hi = pDrvSlwParms->DatDrvRiseFastExt;
        dat_slw_fall_hi = pDrvSlwParms->DatSlwFallFastExt;
        dat_slw_rise_hi = pDrvSlwParms->DatSlwRiseFastExt;

        dqs_drv_fall_hi = pDrvSlwParms->DqsDrvFallFastExt;
        dqs_drv_rise_hi = pDrvSlwParms->DqsDrvRiseFastExt;
        dqs_slw_fall_hi = pDrvSlwParms->DqsSlwFallFastExt;			
        dqs_slw_rise_hi = pDrvSlwParms->DqsSlwRiseFastExt;

        //ratio_a = (acount - SLOW_COUNT_EXT) / (SLOW_COUNT_AVG - SLOW_COUNT_EXT);
        //if (ratio_a < 0)
        //    ratio_a = 0;
        //ratio_b = 1.0 - ratio_a;
        ratio_a = TOFIXEDPOINT(acount - pDrvSlwParms->FastCountAvg) / (pDrvSlwParms->FastCountExt - pDrvSlwParms->FastCountAvg);
        if (ratio_a > TOFIXEDPOINT(1))
            ratio_a = TOFIXEDPOINT(1);
        ratio_b = TOFIXEDPOINT(1) - ratio_a;
    }
    else if (acount < pDrvSlwParms->TypiCount)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " between slow and typi cases\n");

        adr_drv_fall_lo = pDrvSlwParms->AdrDrvFallSlowAvg;
        adr_drv_rise_lo = pDrvSlwParms->AdrDrvRiseSlowAvg;
        adr_slw_fall_lo = pDrvSlwParms->AdrSlwFallSlowAvg;
        adr_slw_rise_lo = pDrvSlwParms->AdrSlwRiseSlowAvg;

        clk_drv_fall_lo = pDrvSlwParms->ClkDrvFallSlowAvg;
        clk_drv_rise_lo = pDrvSlwParms->ClkDrvRiseSlowAvg;
        clk_slw_fall_lo = pDrvSlwParms->ClkSlwFallSlowAvg;
        clk_slw_rise_lo = pDrvSlwParms->ClkSlwRiseSlowAvg;
				
        dat_drv_fall_lo = pDrvSlwParms->DatDrvFallSlowAvg;
        dat_drv_rise_lo = pDrvSlwParms->DatDrvRiseSlowAvg;
        dat_slw_fall_lo = pDrvSlwParms->DatSlwFallSlowAvg;
        dat_slw_rise_lo = pDrvSlwParms->DatSlwRiseSlowAvg;

        dqs_drv_fall_lo = pDrvSlwParms->DqsDrvFallSlowAvg;
        dqs_drv_rise_lo = pDrvSlwParms->DqsDrvRiseSlowAvg;
        dqs_slw_fall_lo = pDrvSlwParms->DqsSlwFallSlowAvg;
        dqs_slw_rise_lo = pDrvSlwParms->DqsSlwRiseSlowAvg;

        adr_drv_fall_hi = pDrvSlwParms->AdrDrvFallTypi;
        adr_drv_rise_hi = pDrvSlwParms->AdrDrvRiseTypi;
        adr_slw_fall_hi = pDrvSlwParms->AdrSlwFallTypi;
        adr_slw_rise_hi = pDrvSlwParms->AdrSlwRiseTypi;

        clk_drv_fall_hi = pDrvSlwParms->ClkDrvFallTypi;
        clk_drv_rise_hi = pDrvSlwParms->ClkDrvRiseTypi;
        clk_slw_fall_hi = pDrvSlwParms->ClkSlwFallTypi;
        clk_slw_rise_hi = pDrvSlwParms->ClkSlwRiseTypi;
				
        dat_drv_fall_hi = pDrvSlwParms->DatDrvFallTypi;
        dat_drv_rise_hi = pDrvSlwParms->DatDrvRiseTypi;
        dat_slw_fall_hi = pDrvSlwParms->DatSlwFallTypi;
        dat_slw_rise_hi = pDrvSlwParms->DatSlwRiseTypi;

        dqs_drv_fall_hi = pDrvSlwParms->DqsDrvFallTypi;
        dqs_drv_rise_hi = pDrvSlwParms->DqsDrvRiseTypi;
        dqs_slw_fall_hi = pDrvSlwParms->DqsSlwFallTypi;
        dqs_slw_rise_hi = pDrvSlwParms->DqsSlwRiseTypi;

        //ratio_a = (acount - SLOW_COUNT_AVG) / (TYPI_COUNT - SLOW_COUNT_AVG);
        //ratio_b = 1.0 - ratio_a;
        ratio_a = TOFIXEDPOINT(acount - pDrvSlwParms->SlowCountAvg) / (pDrvSlwParms->TypiCount - pDrvSlwParms->SlowCountAvg);
        ratio_b = TOFIXEDPOINT(1) - ratio_a;
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " between typi and fast cases\n");

        adr_drv_fall_lo = pDrvSlwParms->AdrDrvFallTypi;
        adr_drv_rise_lo = pDrvSlwParms->AdrDrvRiseTypi;
        adr_slw_fall_lo = pDrvSlwParms->AdrSlwFallTypi;
        adr_slw_rise_lo = pDrvSlwParms->AdrSlwRiseTypi;

        clk_drv_fall_lo = pDrvSlwParms->ClkDrvFallTypi;
        clk_drv_rise_lo = pDrvSlwParms->ClkDrvRiseTypi;
        clk_slw_fall_lo = pDrvSlwParms->ClkSlwFallTypi;
        clk_slw_rise_lo = pDrvSlwParms->ClkSlwRiseTypi;
				
        dat_drv_fall_lo = pDrvSlwParms->DatDrvFallTypi;
        dat_drv_rise_lo = pDrvSlwParms->DatDrvRiseTypi;
        dat_slw_fall_lo = pDrvSlwParms->DatSlwFallTypi;
        dat_slw_rise_lo = pDrvSlwParms->DatSlwRiseTypi;

        dqs_drv_fall_lo = pDrvSlwParms->DqsDrvFallTypi;
        dqs_drv_rise_lo = pDrvSlwParms->DqsDrvRiseTypi;
        dqs_slw_fall_lo = pDrvSlwParms->DqsSlwFallTypi;
        dqs_slw_rise_lo = pDrvSlwParms->DqsSlwRiseTypi;

        adr_drv_fall_hi = pDrvSlwParms->AdrDrvFallFastAvg;
        adr_drv_rise_hi = pDrvSlwParms->AdrDrvRiseFastAvg;
        adr_slw_fall_hi = pDrvSlwParms->AdrSlwFallFastAvg;
        adr_slw_rise_hi = pDrvSlwParms->AdrSlwRiseFastAvg;

        clk_drv_fall_hi = pDrvSlwParms->ClkDrvFallFastAvg;
        clk_drv_rise_hi = pDrvSlwParms->ClkDrvRiseFastAvg;
        clk_slw_fall_hi = pDrvSlwParms->ClkSlwFallFastAvg;
        clk_slw_rise_hi = pDrvSlwParms->ClkSlwRiseFastAvg;
				
        dat_drv_fall_hi = pDrvSlwParms->DatDrvFallFastAvg;
        dat_drv_rise_hi = pDrvSlwParms->DatDrvRiseFastAvg;
        dat_slw_fall_hi = pDrvSlwParms->DatSlwFallFastAvg;
        dat_slw_rise_hi = pDrvSlwParms->DatSlwRiseFastAvg;

        dqs_drv_fall_hi = pDrvSlwParms->DqsDrvFallFastAvg;
        dqs_drv_rise_hi = pDrvSlwParms->DqsDrvRiseFastAvg;
        dqs_slw_fall_hi = pDrvSlwParms->DqsSlwFallFastAvg;
        dqs_slw_rise_hi = pDrvSlwParms->DqsSlwRiseFastAvg;

        //ratio_a = (acount - TYPI_COUNT) / (FAST_COUNT_AVG - TYPI_COUNT);
        //ratio_b = 1.0 - ratio_a;
        ratio_a = TOFIXEDPOINT(acount - pDrvSlwParms->TypiCount) / (pDrvSlwParms->FastCountAvg - pDrvSlwParms->TypiCount);
        ratio_b = TOFIXEDPOINT(1) - ratio_a;
    }

    // ******* New Code - 2/16/01 **********
    //********** Set DQS_IB delay based on VDD speedometer
    if ((bcount <= pDrvSlwParms->SlowCountBAvg) && (pDrvSlwParms->FbioInterpMode == 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Slow case - Clamp to extremes\n");
        dqs_ib_del = pDrvSlwParms->DqsInbDelySlowExt;
    }
    else if ((bcount <= pDrvSlwParms->SlowCountBAvg) && (pDrvSlwParms->FbioInterpMode == 1))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Slow case - Clamp to Average\n");
        dqs_ib_del = pDrvSlwParms->DqsInbDelySlowAvg;
    }
    else if ((bcount <= pDrvSlwParms->SlowCountBAvg) && (pDrvSlwParms->FbioInterpMode == 2))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " slow case - interp avg and extreme\n");

        dqs_ib_del_lo = pDrvSlwParms->DqsInbDelySlowExt;
        dqs_ib_del_hi = pDrvSlwParms->DqsInbDelySlowAvg;

        //ratio_a = (acount - SLOW_COUNT_EXT) / (SLOW_COUNT_AVG - SLOW_COUNT_EXT);
        //if (ratio_a < 0)
        //    ratio_a = 0;
        //ratio_b = 1.0 - ratio_a;
        ratio2_a = TOFIXEDPOINT(bcount - pDrvSlwParms->SlowCountBExt) / (pDrvSlwParms->SlowCountBAvg - pDrvSlwParms->SlowCountBExt);
        if (ratio2_a & 0x80000000)       // good test for < 0?!?
            ratio2_a = 0;
        ratio2_b = TOFIXEDPOINT(1) - ratio2_a;
    }
    else if ((bcount >= pDrvSlwParms->FastCountBAvg) && (pDrvSlwParms->FbioInterpMode == 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Fast case - Clamp to Extremes\n");
        dqs_ib_del = pDrvSlwParms->DqsInbDelyFastExt;
    }
    else if ((bcount >= pDrvSlwParms->FastCountBAvg) && (pDrvSlwParms->FbioInterpMode == 1))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Fast case - Clamp to Averages\n");
        dqs_ib_del = pDrvSlwParms->DqsInbDelyFastAvg;
    }
    else if ((bcount >= pDrvSlwParms->FastCountBAvg) && (pDrvSlwParms->FbioInterpMode == 2))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " fast case - interp avg and extreme\n");

        dqs_ib_del_lo = pDrvSlwParms->DqsInbDelyFastAvg;
        dqs_ib_del_hi = pDrvSlwParms->DqsInbDelyFastExt;

        //ratio_a = (ucount - SLOW_COUNT_EXT) / (SLOW_COUNT_AVG - SLOW_COUNT_EXT);
        //if (ratio_a < 0)
        //    ratio_a = 0;
        //ratio_b = 1.0 - ratio_a;
        ratio2_a = TOFIXEDPOINT(bcount - pDrvSlwParms->FastCountBAvg) / (pDrvSlwParms->FastCountBExt - pDrvSlwParms->FastCountBAvg);
        if (ratio2_a > TOFIXEDPOINT(1))
            ratio2_a = TOFIXEDPOINT(1);
        ratio2_b = TOFIXEDPOINT(1) - ratio2_a;
    }
    else if (bcount < pDrvSlwParms->TypiCountB)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " between slow and typi cases\n");

        dqs_ib_del_lo = pDrvSlwParms->DqsInbDelySlowAvg;
        dqs_ib_del_hi = pDrvSlwParms->DqsInbDelyTypi;

        //ratio_a = (acount - SLOW_COUNT_AVG) / (TYPI_COUNT - SLOW_COUNT_AVG);
        //ratio_b = 1.0 - ratio_a;
        ratio2_a = TOFIXEDPOINT(bcount - pDrvSlwParms->SlowCountBAvg) / (pDrvSlwParms->TypiCountB - pDrvSlwParms->SlowCountBAvg);
        ratio2_b = TOFIXEDPOINT(1) - ratio2_a;
    }
    else 
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, " between typi and fast cases\n");

        dqs_ib_del_lo = pDrvSlwParms->DqsInbDelyTypi;
        dqs_ib_del_hi = pDrvSlwParms->DqsInbDelyFastAvg;

        //ratio_a = (acount - TYPI_COUNT) / (FAST_COUNT_AVG - TYPI_COUNT);
        //ratio_b = 1.0 - ratio_a;
        ratio2_a = TOFIXEDPOINT(bcount - pDrvSlwParms->TypiCountB) / (pDrvSlwParms->FastCountBAvg - pDrvSlwParms->TypiCountB);
        ratio2_b = TOFIXEDPOINT(1) - ratio2_a;
    }

    //
    // Interpolation required...
    //
    if (ratio_a != 0 || ratio_b != 0)
    {
        //adr_drv_fall = Math.round((adr_drv_fall_lo * ratio_b) +
        //                          (adr_drv_fall_hi * ratio_a));
        adr_drv_fall = FROMFIXEDPOINT((adr_drv_fall_lo * ratio_b) +
                                      (adr_drv_fall_hi * ratio_a)); 
        adr_drv_rise = FROMFIXEDPOINT((adr_drv_rise_lo * ratio_b) +
                                      (adr_drv_rise_hi * ratio_a));
        adr_slw_fall = FROMFIXEDPOINT((adr_slw_fall_lo * ratio_b) +
                                      (adr_slw_fall_hi * ratio_a));
        adr_slw_rise = FROMFIXEDPOINT((adr_slw_rise_lo * ratio_b) +
                                      (adr_slw_rise_hi * ratio_a));

        clk_drv_fall = FROMFIXEDPOINT((clk_drv_fall_lo * ratio_b) +
                                      (clk_drv_fall_hi * ratio_a));
        clk_drv_rise = FROMFIXEDPOINT((clk_drv_rise_lo * ratio_b) +
                                      (clk_drv_rise_hi * ratio_a));
        clk_slw_fall = FROMFIXEDPOINT((clk_slw_fall_lo * ratio_b) +
                                      (clk_slw_fall_hi * ratio_a));
        clk_slw_rise = FROMFIXEDPOINT((clk_slw_rise_lo * ratio_b) +
                                      (clk_slw_rise_hi * ratio_a));

        dat_drv_fall = FROMFIXEDPOINT((dat_drv_fall_lo * ratio_b) +
                                      (dat_drv_fall_hi * ratio_a));
        dat_drv_rise = FROMFIXEDPOINT((dat_drv_rise_lo * ratio_b) +
                                      (dat_drv_rise_hi * ratio_a));
        dat_slw_fall = FROMFIXEDPOINT((dat_slw_fall_lo * ratio_b) +
                                      (dat_slw_fall_hi * ratio_a));
        dat_slw_rise = FROMFIXEDPOINT((dat_slw_rise_lo * ratio_b) +
                                      (dat_slw_rise_hi * ratio_a));

        dqs_drv_fall = FROMFIXEDPOINT((dqs_drv_fall_lo * ratio_b) +
                                      (dqs_drv_fall_hi * ratio_a));
        dqs_drv_rise = FROMFIXEDPOINT((dqs_drv_rise_lo * ratio_b) +
                                      (dqs_drv_rise_hi * ratio_a));
        dqs_slw_fall = FROMFIXEDPOINT((dqs_slw_fall_lo * ratio_b) +
                                      (dqs_slw_fall_hi * ratio_a));
        dqs_slw_rise = FROMFIXEDPOINT((dqs_slw_rise_lo * ratio_b) +
                                      (dqs_slw_rise_hi * ratio_a));
    }
    //
    // Interpolation required...
    //
    if (ratio2_a != 0 || ratio2_b != 0)
    {
        dqs_ib_del = FROMFIXEDPOINT((dqs_ib_del_lo * ratio2_b) +
                                    (dqs_ib_del_hi * ratio2_a));
    }

    //
    // Program up the results...
    //
#define SLW_VALUE(r,f) \
    ((r << 28) | (f << 24) | (r << 20) | (f << 16) | \
     (r << 12) | (f << 8)  | (r <<  4) |  f)

    // ADR
    adr_drv = SLW_VALUE(adr_drv_rise, adr_drv_fall);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  adr_drv: ", adr_drv);
    REG_WR32(NV_PBUS_FBIO_ADRDRV, adr_drv);  
    
    adr_slw = SLW_VALUE(adr_slw_rise, adr_slw_fall);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  adr_slw: ", adr_slw);
    REG_WR32(NV_PBUS_FBIO_ADRSLW, adr_slw);  

    // CLK
    clk_drv = SLW_VALUE(clk_drv_rise, clk_drv_fall);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  clk_drv: ", clk_drv);
    REG_WR32(NV_PBUS_FBIO_CLKDRV, clk_drv);  
    
    clk_slw = SLW_VALUE(clk_slw_rise, clk_slw_fall);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  clk_slw: ", clk_slw);
    REG_WR32(NV_PBUS_FBIO_CLKSLW, clk_slw);  

    // DAT
    dat_drv = SLW_VALUE(dat_drv_rise, dat_drv_fall);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  dat_drv: ", dat_drv);
    REG_WR32(NV_PBUS_FBIO_DATDRV, dat_drv);  
    
    dat_slw = SLW_VALUE(dat_slw_rise, dat_slw_fall);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  dat_slw: ", dat_slw);
    REG_WR32(NV_PBUS_FBIO_DATSLW, dat_slw);  

    // DQS
    dqs_drv = SLW_VALUE(dqs_drv_rise, dqs_drv_fall);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  dqs_drv: ", dqs_drv);
    REG_WR32(NV_PBUS_FBIO_DQSDRV, dqs_drv);  
    
    dqs_slw = SLW_VALUE(dqs_slw_rise, dqs_slw_fall);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  dqs_slw: ", dqs_slw);
    REG_WR32(NV_PBUS_FBIO_DQSSLW, dqs_slw);  

    dqs_ib = (dqs_ib_del << 29) |(dqs_ib_del << 21) | (dqs_ib_del << 13) | (dqs_ib_del << 5) | 0x18181818;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  dqs_ib: ", dqs_ib);
    REG_WR32(NV_PBUS_FBIO_DLY, dqs_ib);

    //
    // For A02/A03 chips (XBOX), we're going to up mclk.
    // Note that the we store the clock doubled value in the pHalDev.
    //
    if (IsNV20MaskRevA02_NV20(pHalHwInfo->pMcHalInfo))
    {
        // pDrvSlwParms are in units of MHz/10000
        pHalHwInfo->pDacHalInfo->MClk = 2*pDrvSlwParms->MaxMClk*10000;
        pHalHwInfo->pDacHalInfo->UpdateFlags |= NV_HAL_DAC_UPDATE_MPLL;
    }
    if (IsNV20MaskRevA03_NV20(pHalHwInfo->pMcHalInfo))
    {
        // pDrvSlwParms are in units of MHz/10000
        pHalHwInfo->pDacHalInfo->MClk = 2*pDrvSlwParms->MaxMClk*10000;
        pHalHwInfo->pDacHalInfo->UpdateFlags |= NV_HAL_DAC_UPDATE_MPLL;
    }

}
