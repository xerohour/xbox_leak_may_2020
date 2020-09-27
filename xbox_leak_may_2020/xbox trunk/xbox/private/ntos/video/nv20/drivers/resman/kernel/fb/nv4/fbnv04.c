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
* Module: fbnv04.c                                                          *
*   The NV4 specific HAL FB routines reside in this file.                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv4_ref.h>
#include <nvrm.h>
#include <nv4_hal.h>
#include <heap.h>
#include "nvhw.h"
#include "os.h"
#include "nvhalcpp.h"

// forwards
RM_STATUS nvHalFbControl_NV04(VOID *);
RM_STATUS nvHalFbSetAllocParameters_NV04(VOID *);
RM_STATUS nvHalFbAlloc_NV04(VOID *);
RM_STATUS nvHalFbFree_NV04(VOID *);
RM_STATUS nvHalFbGetSurfacePitch_NV04(VOID *);
RM_STATUS nvHalFbGetSurfaceDimensions_NV04(VOID *);
RM_STATUS nvHalFbLoadOverride_NV04(VOID *);
static U032 nvHalFbRealMemSize(U032, PHWREG);
static U032 calcChkSum(PHWREG, U032);
static void restoreFbSave(U032 *, PHWREG, U032);

// NV4 expects a 512byte alignment
#define FC_MIN_ALIGN_SHIFT      9

/*****************************************************************************
 * tetris tiling
 *****************************************************************************/
char tetris_tile_table_signature[] = "_TETRIS_TILE_TABLE_";
static U032 NvChipTetrisTileTable[] =
{
// width  depth input_width  shift                               mode
//   |    |     |            |                                   |
     320, 16 | (128 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/0, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,
     640, 16 | (128 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/1, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,
     800, 16 | (128 << 16),  0, 0,
    1024, 16 | (128 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/1, /*NV_PFB_CONFIG_0_TETRIS_MODE*/3,
    1280, 16 | (128 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/2, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,

     320, 32 | (128 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/1, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,
     640, 32 | (128 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/2, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,
     800, 32 | (128 << 16),  0, 0,
    1024, 32 | (128 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/2, /*NV_PFB_CONFIG_0_TETRIS_MODE*/3,
    1280, 32 | (128 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/2, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,

     320, 16 | ( 64 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/0, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,
     640, 16 | ( 64 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/1, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,
     800, 16 | ( 64 << 16),  0, 0,
    1024, 16 | ( 64 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/1, /*NV_PFB_CONFIG_0_TETRIS_MODE*/2,
    1280, 16 | ( 64 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/2, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,

     320, 32 | ( 64 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/1, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,
     640, 32 | ( 64 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/2, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,
     800, 32 | ( 64 << 16),  0, 0,
    1024, 32 | ( 64 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/2, /*NV_PFB_CONFIG_0_TETRIS_MODE*/2,
    1280, 32 | ( 64 << 16),  /*NV_PFB_CONFIG_0_TETRIS_SHIFT_*/2, /*NV_PFB_CONFIG_0_TETRIS_MODE*/6,

    /* all others are disabled */
};

static RM_STATUS
nvHalTetrisTileWrite_NV04(PHALHWINFO pHalHwInfo, U032 NewConfig0Value)
{
    PFBHALINFO pFbHalInfo;
    PFBHALINFO_NV04 pFbHalPvtInfo;
    PPRAMHALINFO pPramHalInfo;
    U032 OldConfig0Value;

    pFbHalInfo = pHalHwInfo->pFbHalInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV04)pHalHwInfo->pFbHalPvtInfo;
    pPramHalInfo = pHalHwInfo->pPramHalInfo;

    /*
     * check if we need to reprogram tiling
     */
    OldConfig0Value = REG_RD32(NV_PFB_CONFIG_0);
    NewConfig0Value = (NewConfig0Value & 0x000fffff) | (OldConfig0Value & 0xfff00000);
    if (NewConfig0Value != OldConfig0Value)
    {
        U032      reg0140;
        U032      reg2500;
        U032      reg3200;
        U032      reg3250;
        U032      size    = pPramHalInfo->TotalInstSize;
        VOID*     fbBase  = pHalHwInfo->fbBaseAddr;
        U032      fbSize  = pFbHalInfo->RamSize;
        U032      i;
        U008      *src, *dest;
        U032      sizeMinusU032;
        PMCHALINFO          pMcHalInfo;
        pMcHalInfo = pHalHwInfo->pMcHalInfo;
        /*
         * save instance and driver memory
         */
        src = (U008 *)fbBase + fbSize - size;
        dest = (U008 *)pFbHalPvtInfo->bufferBase;

        DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalTetrisTileWrite_NV04: src ", src);
        DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalTetrisTileWrite_NV04: dest ", dest);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalTetrisTileWrite_NV04: count ", size);
        sizeMinusU032 = size - sizeof(U032);
        for (i = 0; i < sizeMinusU032; i++)
        {
            dest[i] = src[i];
        }

        if (IsNV0A_NV04(pMcHalInfo))
        {
            dest[i]   = 0x77;
            dest[i+1] = 0xcc;
            dest[i+2] = 0x56;
            dest[i+3] = 0x49;
        }
        else
        {
            for(i=sizeMinusU032; i<size; i++)
            {
                dest[i] = src[i]; 
            }
        }
        /*
         * disable chip
         */
        HAL_GR_IDLE(pHalHwInfo);
        WriteIndexed(NV_PRMVIO_SRX, 0x0000);  // _outp (0x3c4,0); _outp (0x3c5,0);
        reg2500 = REG_RD32(NV_PFIFO_CACHES);       REG_WR32(NV_PFIFO_CACHES, 0);
        reg3200 = REG_RD32(NV_PFIFO_CACHE1_PUSH0); REG_WR32(NV_PFIFO_CACHE1_PUSH0, 0);
        reg3250 = REG_RD32(NV_PFIFO_CACHE1_PULL0); REG_WR32(NV_PFIFO_CACHE1_PULL0, 0);
        reg0140 = REG_RD32(NV_PMC_INTR_EN_0);      REG_WR32(NV_PMC_INTR_EN_0, 0);

        /*
         * program tiling
         */
        REG_WR32(NV_PFB_CONFIG_0, NewConfig0Value);

        /*
         * restore instance and driver memory
         */
        for (i = 0; i < size; i++)
        {
            src[i] = dest[i];
        }

        /*
         * re-enable chip
         */
        REG_WR32(NV_PMC_INTR_EN_0, reg0140);
        REG_WR32(NV_PFIFO_CACHE1_PULL0, reg3250);
        REG_WR32(NV_PFIFO_CACHE1_PUSH0, reg3200);
        REG_WR32(NV_PFIFO_CACHES, reg2500);
        WriteIndexed(NV_PRMVIO_SRX, 0x0300); // _outp (0x3c4,0); _outp (0x3c5,3);
    }

    /*
     * done
     */
    return RM_OK;
}


static RM_STATUS
nvHalTetrisTile_NV04(PHALHWINFO pHalHwInfo)
{   
    PFBHALINFO pFbHalInfo;
    PFBHALINFO_NV04 pFbHalPvtInfo;
    PDACHALINFO pDacHalInfo;
    U032 width, depth;
    U032 i;

    pFbHalInfo = pHalHwInfo->pFbHalInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV04)pHalHwInfo->pFbHalPvtInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalTetrisTile_NV04: tiling mode ", pFbHalPvtInfo->tilingDisabled);

    if (!pFbHalPvtInfo->tilingDisabled)
    {
        /*
         * determine if we should setup special tiling for this mode
         */
        width = pFbHalInfo->HorizDisplayWidth;
        depth = pDacHalInfo->Depth | (pDacHalInfo->InputWidth << 16);

#if 0
        /* DO NOT REMOVE - used for tiling tests */
        *((U032*)0xb0050) = (U032)NvChipTetrisTileTable;
#endif

        for (i = 0; i < sizeof(NvChipTetrisTileTable) / sizeof(U032); i += 4)
        {
            if ((NvChipTetrisTileTable[i + 0] == width)
             && (NvChipTetrisTileTable[i + 1] == depth)
             &&  NvChipTetrisTileTable[i + 2]
             &&  NvChipTetrisTileTable[i + 3])
            {
                U032 NewConfig0Value;

                /*
                 * calculate new tiling info
                 */
                NewConfig0Value = NV_PFB_CONFIG_0_TYPE_TETRIS
                                | (NvChipTetrisTileTable[i + 2] << 18)
                                | (NvChipTetrisTileTable[i + 3] << 15);

                /*
                 * program the tiling
                 */
                nvHalTetrisTileWrite_NV04(pHalHwInfo, NewConfig0Value);

                /*
                 * done
                 */
                return RM_OK;
            }
        }
    }  // Tiling is not disabled

    /*
     * disable tiling
     */
    nvHalTetrisTileWrite_NV04(pHalHwInfo, NV_PFB_CONFIG_0_TYPE_NOTILING);

    /*
     * done
     */
    return RM_OK;
}

//
// nvHalFbControl
//
//
RM_STATUS
nvHalFbControl_NV04(VOID *arg)
{
    PFBCONTROLARG_000 pFbControlArg = (PFBCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PFBHALINFO pFbHalInfo;
    PPRAMHALINFO pPramHalInfo;
    PFBHALINFO_NV04 pFbHalPvtInfo;
    U032 test_mem;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbControlArg->id != FB_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pFbControlArg->pHalHwInfo;
    pFbHalInfo = pHalHwInfo->pFbHalInfo;
    pPramHalInfo = pHalHwInfo->pPramHalInfo;
    pFbHalPvtInfo = (PFBHALINFO_NV04)pHalHwInfo->pFbHalPvtInfo;

    switch (pFbControlArg->cmd)
    {
        case FB_CONTROL_INIT:
            //
            // Save off chip-dependent fb state.
            //
            pFbHalPvtInfo->fbSave[0] = REG_RD32(NV_PFB_CONFIG_0);
            pFbHalPvtInfo->fbSave[1] = REG_RD32(NV_PFB_CONFIG_1);

            //
            // Since this is only called from initFb(), we'll do this here
            // KJK Verify a valid RTL setting
            //
            REG_WR32(NV_PFB_RTL, 0);

            //
            // Setup pointer to our tiling save area.  It's immediately
            // after the FBHALINFO_NV04 structure in our engine
            // private data area setup by the RM.
            //
            pFbHalPvtInfo->bufferBase = (U008*)pFbHalPvtInfo + sizeof (FBHALINFO_NV04);

            //
            //  Set RamType
            //
            switch (REG_RD_DRF(_PFB, _BOOT_0, _RAM_TYPE)) {
                //case NV_PFB_BOOT_0_RAM_TYPE_1024K_2BANK:
                case NV_PFB_BOOT_0_RAM_TYPE_SDRAM_16MBIT:
                case NV_PFB_BOOT_0_RAM_TYPE_SDRAM_64MBIT:
                case NV_PFB_BOOT_0_RAM_TYPE_SDRAM_64MBITX16:
                    pFbHalInfo->RamType = BUFFER_SDRAM;
                    break;
                default:
                    // Everything else is SGRAM
                    pFbHalInfo->RamType = BUFFER_SGRAM;
                    break;
            }

            if (REG_RD_DRF(_PFB, _BOOT_0, _UMA)){
                pFbHalInfo->RamType = BUFFER_CRUSH_UMA;
            }

            //
            // Set RamSize (in bytes)
            // 
            // Note: we should have a better way of determining the number of
            // banks (should check the encoded value in the _RAM_TYPE field).
            // In order to avoid possible fragmentation, especially with the
            // smaller mem configs, we'll only use a BankCount of 2.
            //
            switch (REG_RD_DRF(_PFB, _BOOT_0, _RAM_AMOUNT)) {
                case NV_PFB_BOOT_0_RAM_AMOUNT_16MB:
                    pFbHalInfo->RamSizeMb = 16;
                    pFbHalInfo->RamSize   = 0x01000000;
                    pFbHalInfo->BankCount = 2;
                    break;
                case NV_PFB_BOOT_0_RAM_AMOUNT_8MB:
                    pFbHalInfo->RamSizeMb = 8;
                    pFbHalInfo->RamSize   = 0x00800000;
                    pFbHalInfo->BankCount = 2;
                    break;
                case NV_PFB_BOOT_0_RAM_AMOUNT_4MB:
                    pFbHalInfo->RamSizeMb = 4;
                    pFbHalInfo->RamSize   = 0x00400000;
                    pFbHalInfo->BankCount = 2;
                    break;
                case NV_PFB_BOOT_0_RAM_AMOUNT_2MB:
                    //
                    // This is overloaded in NV4.5 to denote 32meg.  We'll never see a 2meg NV4.
                    //
                    pFbHalInfo->RamSizeMb = 32;
                    pFbHalInfo->RamSize = 0x02000000;
                    pFbHalInfo->BankCount = 2;
                    break;
                default:
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Illegal RAM size in NV_PFB_BOOT_0\n\r");
                    DBG_BREAKPOINT();
                    pFbHalInfo->RamSizeMb = 8;
                    pFbHalInfo->RamSize = 0x00800000;
                    pFbHalInfo->BankCount = 2;
                    break;
            }

            //
            // Load config 0 value.
            //
            if (pFbHalInfo->RamSize == 0x00200000)
                FLD_WR_DRF_DEF(_PFB, _CONFIG_0, _BANK_SWAP, _2M);
            else
                FLD_WR_DRF_DEF(_PFB, _CONFIG_0, _BANK_SWAP, _4M);
            
            //
            // Check for UMA usage
            //
            // Note that this is only valid for NV0A, but the other NV4 variants hardwire these
            // bit fields to zero.
            //
            if (REG_RD_DRF(_PFB, _BOOT_0, _UMA) == NV_PFB_BOOT_0_UMA_ENABLE)
                switch (REG_RD_DRF(_PFB, _BOOT_0, _UMA_SIZE)) {
                    case NV_PFB_BOOT_0_UMA_SIZE_2M:
                        pFbHalInfo->RamSizeMb = 2;
                        pFbHalInfo->RamSize   = 0x00200000;
                        break;                 
                    case NV_PFB_BOOT_0_UMA_SIZE_4M:
                        pFbHalInfo->RamSizeMb = 4;
                        pFbHalInfo->RamSize   = 0x00400000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_6M:
                        pFbHalInfo->RamSizeMb = 6;
                        pFbHalInfo->RamSize   = 0x00600000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_8M:
                        pFbHalInfo->RamSizeMb = 8;
                        pFbHalInfo->RamSize   = 0x00800000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_10M:
                        pFbHalInfo->RamSizeMb = 10;
                        pFbHalInfo->RamSize   = 0x00A00000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_12M:
                        pFbHalInfo->RamSizeMb = 12;
                        pFbHalInfo->RamSize   = 0x00C00000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_14M:
                        pFbHalInfo->RamSizeMb = 14;
                        pFbHalInfo->RamSize   = 0x00E00000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_16M:
                        pFbHalInfo->RamSizeMb = 16;
                        pFbHalInfo->RamSize   = 0x01000000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_18M:
                        pFbHalInfo->RamSizeMb = 18;
                        pFbHalInfo->RamSize   = 0x01200000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_20M:
                        pFbHalInfo->RamSizeMb = 20;
                        pFbHalInfo->RamSize   = 0x01400000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_22M:
                        pFbHalInfo->RamSizeMb = 22;
                        pFbHalInfo->RamSize   = 0x01600000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_24M:
                        pFbHalInfo->RamSizeMb = 24;
                        pFbHalInfo->RamSize   = 0x01800000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_26M:
                        pFbHalInfo->RamSizeMb = 26;
                        pFbHalInfo->RamSize   = 0x01A00000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_28M:
                        pFbHalInfo->RamSizeMb = 28;
                        pFbHalInfo->RamSize   = 0x01C00000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_30M:
                        pFbHalInfo->RamSizeMb = 30;
                        pFbHalInfo->RamSize   = 0x01E00000;
                        break;
                    case NV_PFB_BOOT_0_UMA_SIZE_32M:
                        pFbHalInfo->RamSizeMb = 32;
                        pFbHalInfo->RamSize   = 0x02000000;
                        break;
                    default:
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Illegal UMA size in NV_PFB_BOOT_0\n\r");
                        DBG_BREAKPOINT();
                        pFbHalInfo->RamSizeMb = 8;
                        pFbHalInfo->RamSize   = 0x00800000;
                        break;
                }

            //CAH:Actually step through memory to verify we have as much there as the BIOS is 
            //telling us. On some "gray" market boards they may be strapped to report 
            //double the actual physical memory.This test will step through the framebuffer
            //to determine if it's actually there.
            test_mem = nvHalFbRealMemSize(pFbHalInfo->RamSizeMb, pHalHwInfo->fbBaseAddr);

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
            // For NV4, this is 128K allocatable.
            //
            pPramHalInfo->TotalInstSize = 0x20000 + pPramHalInfo->InstPadSize;

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
            pFbHalPvtInfo->fifoContextSize = NUM_FIFOS_NV04 * FC_SIZE_NV04;
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
        case FB_CONTROL_LOAD:
            break;
        case FB_CONTROL_UNLOAD:
            break;
        case FB_CONTROL_UPDATE:
            break;
        case FB_CONTROL_DESTROY:
            // We now do everything tiled (mode switches and fullscreen DOS boxes), so reverting
            // to a boot time non-tiled PFB_CONFIG_0 will corrupt the shutdown splash screen since
            // it was written to a tiled FB. If we're tiled at this point, we'll just stay tiled.
            if (pFbHalPvtInfo->tilingDisabled == TRUE) {
                REG_WR32(NV_PFB_CONFIG_0, pFbHalPvtInfo->fbSave[0]);
                REG_WR32(NV_PFB_CONFIG_1, pFbHalPvtInfo->fbSave[1]);
            }            
            break;
        case FB_CONTROL_INSTWR_DISABLE:
#if 0
            // We have a bug in NV4 hardware which makes the write protection also a read protection.
            // We'll depend on the save above to protect instance memory. If we turn on write protect, it may
            // cause problems on a restart, because either the BIOS image won't be found or can't be copied.
            // 
            //
            // Disable access to instance memory while a full-screen DOS VGA session is active, as they
            // may accidently (intentionally?) clear the entire framebuffer through VGA.
            //
            FLD_WR_DRF_DEF(_PFB, _CONFIG_0, _PRAMIN_WR, _DISABLED);
            FLD_WR_DRF_NUM(_PFB, _CONFIG_0, _PRAMIN_WR_MASK, 0x0001);
#endif
            break;
        case FB_CONTROL_INSTWR_ENABLE:
            FLD_WR_DRF_DEF(_PFB, _CONFIG_0, _PRAMIN_WR, _INIT);
            break;
        case FB_CONTROL_TILING_DISABLE:
            pFbHalPvtInfo->tilingDisabled = TRUE;
            break;
        case FB_CONTROL_TILING_ENABLE:
            pFbHalPvtInfo->tilingDisabled = FALSE;
            break;
        case FB_CONTROL_TILING_CONFIGURE:
            nvHalTetrisTile_NV04(pFbControlArg->pHalHwInfo);
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

// I don't like this #ifdef ... this is just for the nightly build
#ifdef NTRM
#define NV4_PITCHALIGN  0x1F
#else
#define NV4_PITCHALIGN  0x3F
#endif

RM_STATUS
nvHalFbGetSurfacePitch_NV04(VOID *arg)
{
    PFBGETSURFACEPITCHARG_000 pFbGetSurfacePitchArg = (PFBGETSURFACEPITCHARG_000)arg;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbGetSurfacePitchArg->id != FB_GET_SURFACE_PITCH_000)
        return (RM_ERR_VERSION_MISMATCH);

    // surfaces are (depending on who you talk to) 32byte/64byte aligned
    // Watch out for bpp==15: add 1 here to force 15 to 16 for the shift
    pFbGetSurfacePitchArg->pitch = ((pFbGetSurfacePitchArg->width * ((pFbGetSurfacePitchArg->bpp+1) >> 3)) + NV4_PITCHALIGN) & ~NV4_PITCHALIGN;

    return RM_OK;
}

//
// nvHalFbGetSurfaceDimensions
//
// Given a width, height and bits per pixel, return the surface pitch and size
//
RM_STATUS
nvHalFbGetSurfaceDimensions_NV04(VOID *arg)
{
    PFBGETSURFACEDIMENSIONSARG_000 pFbGetSurfaceDimensionsArg = (PFBGETSURFACEDIMENSIONSARG_000)arg;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbGetSurfaceDimensionsArg->id != FB_GET_SURFACE_DIMENSIONS_000)
        return (RM_ERR_VERSION_MISMATCH);

    // surfaces are (depending on who you talk to) 32byte/64byte aligned
    // Watch out for bpp==15: add 1 here to force 15 to 16 for the shift
    pFbGetSurfaceDimensionsArg->pitch = ((pFbGetSurfaceDimensionsArg->width * ((pFbGetSurfaceDimensionsArg->bpp+1) >> 3)) + NV4_PITCHALIGN) & ~NV4_PITCHALIGN;
    pFbGetSurfaceDimensionsArg->size  = pFbGetSurfaceDimensionsArg->pitch * pFbGetSurfaceDimensionsArg->height;

    return RM_OK;
}

//
// nvHalFbAlloc
//
// Reserve a tile if one is available.
//
RM_STATUS
nvHalFbSetAllocParameters_NV04(VOID *arg)
{
    PFBSETALLOCPARAMSARG_000 pFbSetAllocParamsArg = (PFBSETALLOCPARAMSARG_000)arg;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbSetAllocParamsArg->id != FB_SET_ALLOC_PARAMS_000)
        return (RM_ERR_VERSION_MISMATCH);

    return (RM_OK);
}

//
// nvHalFbAlloc
//
RM_STATUS
nvHalFbAlloc_NV04(VOID *arg)
{
    PFBALLOCARG_000 pFbAllocArg = (PFBALLOCARG_000)arg;
    PFBALLOCINFO pFbAllocInfo;

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pFbAllocArg->id != FB_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    pFbAllocInfo = pFbAllocArg->pAllocInfo;

    //
    // Set default alignment based on type.
    //
    switch (pFbAllocInfo->type) {
    case MEM_TYPE_IMAGE:
    case MEM_TYPE_OVERLAY:      
        pFbAllocInfo->align = 0x3f;     // 64 byte alignment
        break;
    case MEM_TYPE_TEXTURE:
        pFbAllocInfo->align = 0xff;     // 256 byte alignment
        break;
    case MEM_TYPE_FONT:
        pFbAllocInfo->align = 0x1f;     // 32 byte alignment
        break;
    case MEM_TYPE_CURSOR:
        pFbAllocInfo->align = 0x7ff;    // 2048 byte alignment
        break;
    case MEM_TYPE_DMA:
        pFbAllocInfo->align = 0x3;      // 4 byte alignment
        break;
    case MEM_TYPE_INSTANCE:
        pFbAllocInfo->align = 0xf;      // 16 byte alignment
        break;
    case MEM_TYPE_DEPTH:
    case MEM_TYPE_DEPTH_COMPR16:
    case MEM_TYPE_DEPTH_COMPR32:
    case MEM_TYPE_PRIMARY:
    case MEM_TYPE_IMAGE_TILED:
        pFbAllocInfo->align = 0xfff;    // 4096 byte alignment
        break;
    default:
        pFbAllocInfo->align = pFbAllocInfo->alignPerf = 0;
        return (RM_ERR_BAD_ARGUMENT);
    }

    pFbAllocInfo->alignPerf = pFbAllocInfo->align;

    return (RM_OK);
}

//
// nvHalFbFree
//
RM_STATUS
nvHalFbFree_NV04(VOID *arg)
{
    PFBFREEARG_000 pFbFreeArg = (PFBFREEARG_000)arg;

    //
    // Verify interface revision.
    //
    if (pFbFreeArg->id != FB_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);

    return (RM_OK);
}

RM_STATUS
nvHalFbLoadOverride_NV04(VOID *arg)
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
            REG_WR32(NV_PFB_CONFIG_1, pFbLoadOverrideArg->value);
            break;
        case FB_LOAD_OVERRIDE_RTL:
            REG_WR32(NV_PFB_RTL, pFbLoadOverrideArg->value);
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

    //Write 0,1,2,3,...N on each 1mb boundry in the FB
    for (i = 0, wp = 0; i < RamSizeMb; i++, wp += 0x00100000) 
    {
        fb_save[i] = base->Reg032[wp/4];
        base->Reg032[wp/4] = i;
        checksum1 += i;
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
