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

// NV20 expects a 1024byte alignment
#define FC_MIN_ALIGN_SHIFT      10

//
// Macro for mirroring PFB reg values into texture cache via RDI.
//
#define RDI_REG_WR32(select, addr, data)    \
    {                                       \
        REG_WR32(NV_PGRAPH_RDI_INDEX,       \
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, select) |    \
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, addr));     \
        REG_WR32(NV_PGRAPH_RDI_DATA, data);                         \
    }

#define RDI_REG_RD32(select, addr, data)    \
    {                                       \
        REG_WR32(NV_PGRAPH_RDI_INDEX,       \
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, select) |    \
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, addr));     \
        data = REG_RD32(NV_PGRAPH_RDI_DATA);                        \
    }

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
    PHWREG nvAddr;
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
    nvAddr = pHalHwInfo->nvBaseAddr;

    switch (pFbControlArg->cmd)
    {
        case FB_CONTROL_INIT:
        {
#if 0
            U032 cfg0, cfg1;
#endif
            U032 tmpAddr;

            //
            // Setup bank count.
            //
            pFbHalInfo->BankCount = REG_RD_DRF(_PFB, _CFG0, _PARTSENSE) + 1;

            //
            // Save off chip-dependent fb state.
            //
            pFbHalPvtInfo->fbSave[0] = REG_RD32(NV_PFB_CFG0);
            pFbHalPvtInfo->fbSave[1] = REG_RD32(NV_PFB_CFG1);

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

    if (pGetSurfacePitchArg->flags & FB_GET_SURFACE_PITCH_NO_ADJUST)
        return RM_OK;

    _nvHalFbRoundTiledPitchSize(&pGetSurfacePitchArg->pitch, NULL, NULL);
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
        
    if (pGetSurfaceDimensionsArg->flags & FB_GET_SURFACE_PITCH_NO_ADJUST)
        return RM_OK;

    _nvHalFbRoundTiledPitchSize(&pGetSurfaceDimensionsArg->pitch,
                                pGetSurfaceDimensionsArg->height,
                                &pGetSurfaceDimensionsArg->size);
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
    U032       *pDmaPush
)
{
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

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
        if (REG_RD32(NV_PMC_INTR_0) & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING))
            HALRMSERVICEINTR(pHalHwInfo, DAC_ENGINE_TAG);
    }

    // Disable the dma pusher
    *pDmaPush = REG_RD32(NV_PFIFO_CACHE1_DMA_PUSH);
    REG_WR_DRF_DEF(_PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _DISABLED);

    // Wait until the DMA pusher is idle
    while (REG_RD_DRF(_PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY)
	    ;
}

static VOID
nvHalTilingUpdateResume_NV20
(
    PHALHWINFO pHalHwInfo,
    U032       DmaPush
)
{
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

    // Restore the dma pusher state
    REG_WR32(NV_PFIFO_CACHE1_DMA_PUSH, DmaPush);
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
        pFbAllocInfo->align = 0;
        return (RM_ERR_BAD_ARGUMENT);
    }

    //
    // A non-zero pitch value indicates the user wants tiled memory
    // (if available).  If the pitch is 0, we're done.
    //
    if (pFbAllocInfo->pitch == 0)
    {
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

    // tiles are 16K aligned.
    pFbAllocInfo->align = 0x4000 - 1;

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
    PHWREG nvAddr;
    U032 i, dmapush;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbFreeArg->id != FB_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbFreeArg->pHalHwInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;
    nvAddr = pFbFreeArg->pHalHwInfo->nvBaseAddr;

    //
    // Bail out now if there's no resource id.
    if (!pFbFreeArg->hwResId)
        return (RM_OK);

    // convert range bits to a TILE index
    for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
        if (pFbFreeArg->hwResId & MEM_TYPE_TILE_RANGE_SHIFT(i))
                break;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Freeing TILE Range:   ", i);

    // idle graphics, so we can update the PGRAPH registers
    nvHalTilingUpdateIdle_NV20(pFbFreeArg->pHalHwInfo, &dmapush);

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
        _nvHalFbFreeZtags(pHalHwInfo, pFbHalPvtInfo->zCompState[i].offset, pFbHalPvtInfo->zCompState[i].size);
        pFbHalPvtInfo->zCompState[i].offset = MAX_ZTAG_BITS;
        pFbHalPvtInfo->zCompState[i].size = 0;
    }

    // resume graphics, now that PFB/PGRAPH are consistent
    nvHalTilingUpdateResume_NV20(pHalHwInfo, dmapush);

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
    PHWREG nvAddr;
    U032 i, dmapush;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbSetAllocParamsArg->id != FB_SET_ALLOC_PARAMS_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbSetAllocParamsArg->pHalHwInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV20)pHalHwInfo->pFbHalPvtInfo;
    pFbAllocInfo = pFbSetAllocParamsArg->pAllocInfo;
    nvAddr = pFbSetAllocParamsArg->pHalHwInfo->nvBaseAddr;

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
    nvHalTilingUpdateIdle_NV20(pFbSetAllocParamsArg->pHalHwInfo, &dmapush);

    //
    // Note: due to a PFB_TLIMIT readback bug, only compare the upper bits
    // when checking for PFB_TLIMIT/PGRAPH_TLIMIT consistency.
    // 
    do {
        // Now, wait until graphics is idle
        HAL_GR_IDLE(pHalHwInfo);

        REG_WR32(NV_PFB_TLIMIT(i), pFbAllocInfo->offset + pFbAllocInfo->size - 1);
        REG_WR32(NV_PFB_TSIZE(i), pFbAllocInfo->pitch);
        REG_WR32(NV_PFB_TILE(i), pFbAllocInfo->offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));

        REG_WR32(NV_PGRAPH_TLIMIT(i), pFbAllocInfo->offset + pFbAllocInfo->size - 1);
        REG_WR32(NV_PGRAPH_TSIZE(i), pFbAllocInfo->pitch);
        REG_WR32(NV_PGRAPH_TILE(i), pFbAllocInfo->offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));

        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TLIMIT_ADDRESS(i),
                     pFbAllocInfo->offset + pFbAllocInfo->size - 1);
        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TPITCH_ADDRESS(i),
                     pFbAllocInfo->pitch);
        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(i),
                     pFbAllocInfo->offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));
    } while ((REG_RD32(NV_PFB_TILE(i)) != REG_RD32(NV_PGRAPH_TILE(i))) ||
             (REG_RD32(NV_PFB_TSIZE(i)) != REG_RD32(NV_PGRAPH_TSIZE(i))) ||
             ((REG_RD32(NV_PFB_TLIMIT(i)) & ~0x3FFF) != (REG_RD32(NV_PGRAPH_TLIMIT(i)) & ~0x3FFF)));

    //
    // Handle Z compression.
    //
    if (pFbAllocInfo->type == MEM_TYPE_DEPTH_COMPR16 || pFbAllocInfo->type == MEM_TYPE_DEPTH_COMPR32)
    {
        U032 offset, size, data;
       
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
            data = DRF_NUM(_PFB, _ZCOMP, _BASE_TAG_ADR, offset) |
                DRF_DEF(_PFB, _ZCOMP, _EN, _TRUE);

            // mode (16 or 32bit)
            if (pFbAllocInfo->type == MEM_TYPE_DEPTH_COMPR32)
                data |= DRF_DEF(_PFB, _ZCOMP, _MODE, _32);

            // enable big endian mode if applicable
            if (pHalHwInfo->pMcHalInfo->EndianControl & MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN)
                data |= DRF_DEF(_PFB, _ZCOMP, _ENDIAN, _BIG);

            //
            // TODO: enable antialiasing...
            //

            do {
                // Now, wait until graphics is idle
                HAL_GR_IDLE(pHalHwInfo);

                REG_WR32(NV_PFB_ZCOMP(i), data);
                REG_WR32(NV_PGRAPH_ZCOMP(i), data);
                RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                             NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(i),
                             data);
            } while (REG_RD32(NV_PFB_ZCOMP(i)) != REG_RD32(NV_PGRAPH_ZCOMP(i)));
        }
    }

    // resume graphics, now that PFB/PGRAPH are consistent
    nvHalTilingUpdateResume_NV20(pHalHwInfo, dmapush);

    return (RM_OK);
}

RM_STATUS
nvHalFbLoadOverride_NV20(VOID *arg)
{
    PFBLOADOVERRIDEARG_000 pFbLoadOverrideArg = (PFBLOADOVERRIDEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PHWREG nvAddr;

    //
    // Verify interface revision.
    //
    if (pFbLoadOverrideArg->id != FB_LOAD_OVERRIDE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbLoadOverrideArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;
    switch (pFbLoadOverrideArg->cmd)
    {
        case FB_LOAD_OVERRIDE_MEMORY:
            REG_WR32(NV_PFB_CFG0, pFbLoadOverrideArg->value);
            break;
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
