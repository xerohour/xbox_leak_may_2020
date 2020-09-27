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

/**************************** Graphics Manager *****************************\
*                                                                           *
* Module: GRNV20.C                                                          *
*   The graphics chip dependent routines are kept here.                     *
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
RM_STATUS nvHalGrControl_NV20(VOID *);
RM_STATUS nvHalGrAlloc_NV20(VOID *);
RM_STATUS nvHalGrFree_NV20(VOID *);
RM_STATUS nvHalGrGetExceptionData_NV20(VOID *);
RM_STATUS nvHalGrService_NV20(VOID *);
RM_STATUS nvHalGrGetNotifyData_NV20(VOID *);
RM_STATUS nvHalGrSetObjectContext_NV20(VOID *);

// statics
static RM_STATUS _nvHalGrInitObjectContext_NV20(PHALHWINFO, U032, U032);
static RM_STATUS _nvHalGrInit3d_NV20(PHALHWINFO, U032);
static RM_STATUS _nvHalGrDestroy3d_NV20(PHALHWINFO, U032);

// externs
extern RM_STATUS nvHalGrClassSoftwareMethod_NV20(PHALHWINFO, PGREXCEPTIONDATA);

extern RM_STATUS nvHalGrLoadChannelContext_NV20(PHALHWINFO, U032);
extern RM_STATUS nvHalGrUnloadChannelContext_NV20(PHALHWINFO, U032);

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

//
// nvHalGrControl
//
RM_STATUS
nvHalGrControl_NV20(VOID *arg)
{
    PGRCONTROLARG_000 pGrControlArg = (PGRCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrControlArg->pHalHwInfo;
    PGRHALINFO pGrHalInfo;
    PGRHALINFO_NV20 pGrHalPvtInfo;
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrControl_NV20\r\n");

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pGrControlArg->id != GR_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrControlArg->pHalHwInfo;
    pGrHalInfo = pHalHwInfo->pGrHalInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;
    
    switch (pGrControlArg->cmd)
    {
        case GR_CONTROL_INIT:
            //
            // Initialize graphics debug values.
            //

            // all DEBUG0 bits are RESET bits in NV20
            pGrHalPvtInfo->Debug0 = 0x0;

            pGrHalPvtInfo->Debug1 = (U032)
                DRF_DEF(_PGRAPH, _DEBUG_1, _VOLATILE_RESET,   _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_PTE,          _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_CACHE,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_FILE,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_1, _DRAWDIR_AUTO,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_1, _DRAWDIR_Y,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_1, _INSTANCE,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_1, _CTX,              _INIT);

            pGrHalPvtInfo->Debug2 = (U032)
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_2D, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_BLT, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_CELS, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_3D, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_LIMIT_CHECK,    _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_OVRFLW_CHECK,   _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_MEMSIZE_CHECK,  _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_SWIZZLE_CHECK,  _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_TILEVIOL,       _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_ENDIAN_CHECK,   _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_LIMIT_INT,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_OVRFLW_INT,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PASS_VIOL,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCHWRAP,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_3D,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_2D,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_SWALLOW_REQS,   _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_SBFILTER,       _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_FASTCLEAR,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_FLUSH_HOLDOFF,  _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_TILEBIT_UPDATE, _INIT);

            pGrHalPvtInfo->Debug3 = (U032)
                DRF_DEF(_PGRAPH, _DEBUG_3, _FLUSHING,              _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _HW_CONTEXT_SWITCH,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _FD_CHECK,              _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_DATA_STRTCH,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_3D_SHADOW_DATA,   _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _CHECK_64BYTE_ALIGN,    _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _SINGLE_STEP,           _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _RDI_IDLE_WAIT,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _IDLE_FILTER,           _ENABLED) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _SYNCHRONIZE,           _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _OBJECT_RELOAD,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _PM_TRIGGER,            _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _ALTARCH,               _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _SINGLE_CYCLE_LOAD,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _BILINEAR_3D,           _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _VOLATILE_RESET,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _DATA_CHECK,            _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _DATA_CHECK_FAIL,       _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _FORMAT_CHECK,          _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _DMA_CHECK,             _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _STATE_CHECK,           _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _KELVIN_HWFLIP,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_3D_RESTORE,       _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _STATE3D_CHECK,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _XFMODE_COALESCE,       _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _CTX_METHODS,           _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _OP_METHODS,            _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_3, _IGNORE_PATCHVALID,     _INIT);

            // all DEBUG_[4,5] bits are SPARES currently
            pGrHalPvtInfo->Debug4 = 0x0;
            pGrHalPvtInfo->Debug5 =
                DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_REQ_FULL_CVG,    _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_PUNT_FB_BUSY,    _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_PUNT_S_ON_Z,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_RETURN_COMP,     _INIT);

            pGrHalPvtInfo->Debug6 = (U032)
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_SEP_ZC_READS,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_DITHER_3D,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_DITHER_2D,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_EARLY_ZABORT,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_PASS_ROPFLUSH,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_CBUF_DRAIN,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_CBUF_HIGHWATER,    _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_CBUF_HIGHWATER2,   _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_COALESCE_3D,       _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_COALESCE_2D,       _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_FIXED_ADRS,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_FAST_KEEP_DST,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_FORCE_CREAD,       _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ALLOW_3D_SKIP_READ, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZCMP_ALWAYS_READ,  _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZCOMPRESS_EN,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_BURST_CREADS,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_BURST_CWRITES,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_AUTO_INIT,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_HASH_TEST1,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_INTERLOCK,         _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZREAD,             _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZCULL_DATA,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_CROP_SWALLOW,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_6, _SPARE_BIT31,           _INIT);

            pGrHalPvtInfo->Debug7 = (U032)
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_INIT_ZRD_TMSTAMP,  _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZCULL_NO_STALE,    _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZROP_INTERLOCK,    _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_FORCE_VISIBLE, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_PURGE_PER_PKT, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_FORCE_NO_16BYTE_WR, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_FORCE_FSTCLR_EXPAND, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_FORCE_NO_RMW_DISABLE, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_INHIBIT_COMPRESS, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_CMPR_FULL_BE_ONLY, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_ALWAYS_WR_STENCIL, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_INHIBIT_SKIP_WR_EQ, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_SELECTPM,      _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_EXTRA1,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_EXTRA2,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_EXTRA3,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_CROP_SELECTPM,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_EXTRA4,        _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZRD_HASH_TAGS,     _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_CROP_BLIT_HASH,    _INIT);

            pGrHalPvtInfo->Debug8 = (U032)
                DRF_DEF(_PGRAPH, _DEBUG_8, _SHADER_FIFO_WATERMARK, _INIT) |
                DRF_DEF(_PGRAPH, _DEBUG_8, _COMBINER_NEG_TEXTURE,  _INIT);

            pGrHalPvtInfo->Debug9 = (U032)
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_PATCH0_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_PATCH1_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_PATCH2_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_PATCH3_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _END_PATCH_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_END_SWATCH_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_END_CURVE_CHECK, _DISABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _CURVE_COEFF_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_TRANS0_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_TRANS1_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_TRANS2_CHECK, _ENABLE) | 
                DRF_DEF(_PGRAPH, _DEBUG_9, _END_TRANSITION_CHECK, _ENABLE);

            //
            // Have to use hybrid context switch mode to work around
            // FD bug...we'll do this here so it's easier to control
            // on future chips where we'll hopefully be able to
            // employ full hw context switching.
            //
            pGrHalPvtInfo->Debug3 &= ~(U032)DRF_DEF(_PGRAPH, _DEBUG_3, _HW_CONTEXT_SWITCH, _ENABLED);

            //
            // Separate color and Z reads on SDR memory configurations
            // (i.e. same dram bank).
            //
            if (REG_RD_DRF(_PBUS, _FBIO_RAM, _TYPE) == NV_PBUS_FBIO_RAM_TYPE_SDR)
                pGrHalPvtInfo->Debug6 |= DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_SEP_ZC_READS, _ENABLED);
            else
                pGrHalPvtInfo->Debug6 |= DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_SEP_ZC_READS, _DISABLED);

            //
            // Initialize graphics channel context table register.
            //
            pGrHalPvtInfo->CtxTableBase = pHalHwInfo->grInstMem;
            REG_WR_DRF_NUM(_PGRAPH, _CHANNEL_CTX_TABLE, _INST, pHalHwInfo->grInstMem);
            for (i = 0; i < NUM_FIFOS_NV20; i++)
                INST_WR32(pGrHalPvtInfo->CtxTableBase, i*4, 0);

            //
            // Init channel id to invalid.
            //
            pGrHalPvtInfo->currentChID = NUM_FIFOS_NV20;

            //
            // Per Luke Chang...
            //
            // Fix 2D im_class (scaled class) zpipe overflow
            // bug by shortening the rbuffer in tcache by 9 entries
            //
            RDI_REG_WR32(NV_IGRAPH_TC_TPA_INDEX_SELECT,
                         NV_IGRAPH_TC_TPA_RBFRDEPTH_INDEX_ADDRESS,
                         0x30);
            RDI_REG_WR32(NV_IGRAPH_TC_TPB_INDEX_SELECT,
                         NV_IGRAPH_TC_TPA_RBFRDEPTH_INDEX_ADDRESS,
                         0x30);
            break;
        case GR_CONTROL_LOAD:
        {
            U032 misc;

            //
            // To reset graphics on nv20, we can't use
            // DEBUG_0, because the _CACHE_STATE bit will
            // cause a hang in the tcache.
            //
            // Instead, we use the PMC register.
            //
            FLD_WR_DRF_DEF(_PMC, _ENABLE, _PGRAPH, _DISABLED);
            FLD_WR_DRF_DEF(_PMC, _ENABLE, _PGRAPH, _ENABLED);

            //
            // Restore DEBUG registers.
            //
            REG_WR32(NV_PGRAPH_DEBUG_0, pGrHalPvtInfo->Debug0);
            REG_WR32(NV_PGRAPH_DEBUG_1, pGrHalPvtInfo->Debug1);
            REG_WR32(NV_PGRAPH_DEBUG_2, pGrHalPvtInfo->Debug2);
            REG_WR32(NV_PGRAPH_DEBUG_3, pGrHalPvtInfo->Debug3);
            REG_WR32(NV_PGRAPH_DEBUG_4, pGrHalPvtInfo->Debug4);
            REG_WR32(NV_PGRAPH_DEBUG_5, pGrHalPvtInfo->Debug5);
            REG_WR32(NV_PGRAPH_DEBUG_6, pGrHalPvtInfo->Debug6);
            REG_WR32(NV_PGRAPH_DEBUG_7, pGrHalPvtInfo->Debug7);
            REG_WR32(NV_PGRAPH_DEBUG_8, pGrHalPvtInfo->Debug8);
            REG_WR32(NV_PGRAPH_DEBUG_9, pGrHalPvtInfo->Debug9);

            //
            // Per Luke Chang...
            //
            // Fix 2D im_class (scaled class) zpipe overflow
            // bug by shortening the rbuffer in tcache by 9 entries
            //
            RDI_REG_WR32(NV_IGRAPH_TC_TPA_INDEX_SELECT,
                         NV_IGRAPH_TC_TPA_RBFRDEPTH_INDEX_ADDRESS,
                         0x30);
            RDI_REG_WR32(NV_IGRAPH_TC_TPB_INDEX_SELECT,
                         NV_IGRAPH_TC_TPA_RBFRDEPTH_INDEX_ADDRESS,
                         0x30);

            //
            // Restore context buffer base address.
            //
            REG_WR_DRF_NUM(_PGRAPH, _CHANNEL_CTX_TABLE, _INST, pHalHwInfo->grInstMem);

            // 
            // PGRAPH_TILE values will be reset after a PGRAPH_DEBUG_0_STATE_RESET.
            // Unfortunately, it may have had values we wanted (e.g. from stateFb),
            // so after a DEBUG_0_STATE_RESET, restore the PGRAPH values from PFB.
            //
            HAL_GR_IDLE(pGrControlArg->pHalHwInfo);

            for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
            {
                misc = REG_RD32(NV_PFB_TLIMIT(i));
                REG_WR32(NV_PGRAPH_TLIMIT(i), misc);
                RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                             NV_IGRAPH_TC_MINTFCDEBUG_TLIMIT_ADDRESS(i),
                             misc);
                
                misc = REG_RD32(NV_PFB_TSIZE(i));
                REG_WR32(NV_PGRAPH_TSIZE(i), misc);
                RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                             NV_IGRAPH_TC_MINTFCDEBUG_TPITCH_ADDRESS(i),
                             misc);

                misc = REG_RD32(NV_PFB_TILE(i));
                REG_WR32(NV_PGRAPH_TILE(i), REG_RD32(NV_PFB_TILE(i)));
                RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                             NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(i),
                             misc);

#ifdef DEBUG
                // these registers need to agree
                if (REG_RD32(NV_PFB_TILE(i)) != REG_RD32(NV_PGRAPH_TILE(i)))
                    DBG_BREAKPOINT();
                if (REG_RD32(NV_PFB_TLIMIT(i)) != REG_RD32(NV_PGRAPH_TLIMIT(i)))
                    DBG_BREAKPOINT();
                if (REG_RD32(NV_PFB_TSIZE(i)) != REG_RD32(NV_PGRAPH_TSIZE(i)))
                    DBG_BREAKPOINT();
#endif
            }

            //
            // Restore zcompression table from fb engine.
            //
            for (i = 0; i < NV_PFB_ZCOMP__SIZE_1; i++)
            {
                misc = REG_RD32(NV_PFB_ZCOMP(i));
                REG_WR32(NV_PGRAPH_ZCOMP(i), misc);
                RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                             NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(i),
                             misc);
            }
            misc = REG_RD32(NV_PFB_ZCOMP_OFFSET);
            REG_WR32(NV_PGRAPH_ZCOMP_OFFSET, misc);
            RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                         NV_IGRAPH_TC_MINTFCDEBUG_ZOFFSET_ADDRESS,
                         misc);

            //
            // Restore framebuffer config registers from fb engine.
            //
            misc = REG_RD32(NV_PFB_CFG0);
            REG_WR32(NV_PGRAPH_FBCFG0, misc);
            RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                         NV_IGRAPH_TC_MINTFCDEBUG_CFG0_ADDRESS,
                         misc);

            misc = REG_RD32(NV_PFB_CFG1);
            REG_WR32(NV_PGRAPH_FBCFG1, misc);
            RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                         NV_IGRAPH_TC_MINTFCDEBUG_CFG1_ADDRESS,
                         misc);

            REG_WR32(NV_PGRAPH_CTX_SWITCH1,  0x00000000);
            REG_WR32(NV_PGRAPH_CTX_SWITCH2,  0x00000000);
            REG_WR32(NV_PGRAPH_CTX_SWITCH3,  0x00000000);
            REG_WR32(NV_PGRAPH_CTX_SWITCH4,  0x00000000);
            REG_WR32(NV_PGRAPH_CTX_CONTROL,
                     DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _EXPIRED)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));
            REG_WR_DRF_DEF(_PGRAPH, _FFINTFC_ST2, _CHID_STATUS, _VALID);

            //
            // Reload current graphics context.
            //
            (void) nvHalGrLoadChannelContext_NV20(pHalHwInfo, pGrHalPvtInfo->currentChID);
            break;
        }
        case GR_CONTROL_UNLOAD:
        {
            //
            // Force an unload of the current channel.
            //
            (void) nvHalGrLoadChannelContext_NV20(pHalHwInfo, NUM_FIFOS_NV20);
            break;
        }
        case GR_CONTROL_DESTROY:
        case GR_CONTROL_UPDATE:
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}

//
// nvHalGrAlloc
//
RM_STATUS
nvHalGrAlloc_NV20(VOID *arg)
{
    PGRALLOCARG_000 pGrAllocArg = (PGRALLOCARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrAllocArg->pHalHwInfo;
    PGRHALINFO pGrHalInfo;
    PGRHALINFO_NV20 pGrHalPvtInfo;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrAlloc_NV20\r\n");

    //
    // Verify interface revision.
    //
    if (pGrAllocArg->id != GR_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrAllocArg->pHalHwInfo;
    pGrHalInfo = pHalHwInfo->pGrHalInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;

    //
    // If we've got a 3d object, keep note of it.
    //
    if ((pGrAllocArg->classNum == NV10_CELSIUS_PRIMITIVE) ||
        (pGrAllocArg->classNum == NV15_CELSIUS_PRIMITIVE) ||
        (pGrAllocArg->classNum == NV11_CELSIUS_PRIMITIVE) ||
        (pGrAllocArg->classNum == NV20_KELVIN_PRIMITIVE))
    {
        //
        // Check that a current 3D object hasn't already been created.
        //
        if (pGrHalPvtInfo->currentObjects3d[pGrAllocArg->chid] != 0) {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: 3d state in use by instance ", pGrHalPvtInfo->currentObjects3d[pGrAllocArg->chid]);
            return (RM_ERR_ILLEGAL_OBJECT);
        }
        // ASSERT(pGrChannel->CurrentObject3d == 0)

        pGrHalPvtInfo->currentObjects3d[pGrAllocArg->chid] = pGrAllocArg->instance;

        pGrHalInfo->has3dObject |= (0x1 << pGrAllocArg->chid);

        _nvHalGrInit3d_NV20(pHalHwInfo, pGrAllocArg->chid);
    }

    //
    // Initialize generic object context.
    //
    _nvHalGrInitObjectContext_NV20(pHalHwInfo, pGrAllocArg->instance, pGrAllocArg->classNum);

    return (RM_OK);
}

//
// nvHalGrFree
//
RM_STATUS
nvHalGrFree_NV20(VOID *arg)
{
    PGRFREEARG_000 pGrFreeArg = (PGRFREEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRHALINFO pGrHalInfo;
    PGRHALINFO_NV20 pGrHalPvtInfo;
    U032 i;

    //
    // Verify interface revision.
    //
    if (pGrFreeArg->id != GR_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrFreeArg->pHalHwInfo;
    pGrHalInfo = pHalHwInfo->pGrHalInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;

    //
    // Clear 3d current object pointer if necessary.
    //
    if (pGrFreeArg->instance == pGrHalPvtInfo->currentObjects3d[pGrFreeArg->chid])
    {
        // we could ASSERT that this is a 3d object here as well
        pGrHalPvtInfo->currentObjects3d[pGrFreeArg->chid] = 0;

        // clear the Celsius bit (either never had one or it was just freed)
        pGrHalInfo->has3dObject &= ~(0x1 << pGrFreeArg->chid);
    }

    //
    // Zero out the object context for good measure.
    //
    for (i = 0; i < 4; i++)
    {
        INST_WR32(pGrFreeArg->instance, i, 0);
    }

    return (RM_OK);
}

//
// nvHalGrGetExceptionData
//
// Retrieve graphics engine exception data.
//
RM_STATUS
nvHalGrGetExceptionData_NV20(VOID *arg)
{
    PGRGETEXCEPTIONDATAARG_000 pGrGetExceptionDataArg = (PGRGETEXCEPTIONDATAARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrGetExceptionDataArg->pHalHwInfo;
    PGREXCEPTIONDATA pGrExceptionData;

    //
    // Verify interface revision.
    //
    if (pGrGetExceptionDataArg->id != GR_GET_EXCEPTION_DATA_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrGetExceptionDataArg->pHalHwInfo;
    pGrExceptionData = pGrGetExceptionDataArg->pExceptionData;

    //
    // Fill in exception data.
    //
    pGrExceptionData->Offset = REG_RD32(NV_PGRAPH_TRAPPED_ADDR);
    pGrExceptionData->ChID = DRF_VAL(_PGRAPH, _TRAPPED_ADDR, _CHID, pGrExceptionData->Offset);
    pGrExceptionData->NotifyInstance = REG_RD_DRF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE);
    pGrExceptionData->classNum = REG_RD_DRF(_PGRAPH, _CTX_SWITCH1, _GRCLASS);
    pGrExceptionData->Data = REG_RD32(NV_PGRAPH_TRAPPED_DATA_LOW); 
    pGrExceptionData->Instance = REG_RD32(NV_PGRAPH_CTX_SWITCH4);
    pGrExceptionData->Offset &= (DRF_MASK(NV_PGRAPH_TRAPPED_ADDR_MTHD) << DRF_SHIFT(NV_PGRAPH_TRAPPED_ADDR_MTHD));
    pGrExceptionData->Nsource = REG_RD32(NV_PGRAPH_NSOURCE);
    pGrExceptionData->MethodStatus = RM_OK;

    return (RM_OK);
}

//
// nvHalGrService
//
// Handle selected graphics object exceptions.
//
RM_STATUS
nvHalGrService_NV20(VOID *arg)
{
    PGRSERVICEARG_000 pGrServiceArg = (PGRSERVICEARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrServiceArg->pHalHwInfo;
    PGRHALINFO_NV20 pGrHalPvtInfo;
    PGREXCEPTIONDATA pGrExceptionData;

    //
    // Verify interface revision.
    //
    if (pGrServiceArg->id != GR_SERVICE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrServiceArg->pHalHwInfo;
    pGrExceptionData = pGrServiceArg->pExceptionData;
    pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;

    //
    // Make sure object is valid.
    //
    //if (pGrExceptionData->instance == 0)
    //    return (NV_HAL_ERROR_INVALID_STATE);

    //
    // Handle any exceptions...
    //
    if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _PENDING))
    {
        //
        // Reset condition.
        //
        REG_WR_DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _RESET);

        //
        // Wait for idle
        //
        GR_DONE();
    
        //
        // Graphics channel switch.
        //
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Graphics context switch exception.\n\r");
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Previous graphics channel:",pGrHalPvtInfo->currentChID);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: New graphics channel:", pGrExceptionData->ChID);

        (void) nvHalGrLoadChannelContext_NV20(pHalHwInfo, pGrExceptionData->ChID);
    }

    //
    // Handle methods in software.
    //
    if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _PENDING))
    {
        if (nvHalGrClassSoftwareMethod_NV20(pHalHwInfo, pGrExceptionData) == RM_OK)
        {
            // Reset condition.
            REG_WR_DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _RESET);
        }
    }

    if ((pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING)) ||
        (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _ERROR, _PENDING)))
    {
        if (pGrExceptionData->Nsource & DRF_DEF(_PGRAPH, _NSOURCE, _ILLEGAL_MTHD, _PENDING))
        {
            if (nvHalGrClassSoftwareMethod_NV20(pHalHwInfo, pGrExceptionData) == RM_OK)
            {
                // Reset condition.
                if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING))
                    REG_WR_DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _RESET);
                else
                    REG_WR_DRF_DEF(_PGRAPH, _INTR, _ERROR, _RESET);
            }
        }
    }

    return (RM_OK);
}

//
// nvHalGrNotifyData
//
// Fill in notify data (trigger status and action).
//
RM_STATUS
nvHalGrGetNotifyData_NV20(VOID *arg)
{
    PGRGETNOTIFYDATAARG_000 pGrGetNotifyDataArg = (PGRGETNOTIFYDATAARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrGetNotifyDataArg->pHalHwInfo;
    PGRNOTIFYDATA pGrNotifyData;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrGetNotifyData_NV20\r\n");

    //
    // Verify interface revision.
    //
    if (pGrGetNotifyDataArg->id != GR_GET_NOTIFY_DATA_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrGetNotifyDataArg->pHalHwInfo;
    pGrNotifyData = pGrGetNotifyDataArg->pGrNotifyData;

    if (REG_RD32(NV_PGRAPH_NOTIFY) &
        (DRF_DEF(_PGRAPH, _NOTIFY, _REQ, _PENDING) |
         DRF_DEF(_PGRAPH, _NOTIFY, _STYLE, _WRITE_THEN_AWAKEN)))
    {
        pGrNotifyData->trigger = TRUE;
        pGrNotifyData->action =  REG_RD_DRF(_PGRAPH, _NOTIFY, _STYLE);
        // clear the notify condition
        FLD_WR_DRF_DEF(_PGRAPH, _NOTIFY, _REQ, _NOT_PENDING);
    } else {
        pGrNotifyData->trigger = FALSE;
        pGrNotifyData->action = 0;
    }

    return (RM_OK);
}

//
// nvHalGrSetObjectContext
//
// Update various graphics object context bits.
// On NV20, these bits are managed by hardware so this
// routine does some error checking and returns.
//
RM_STATUS
nvHalGrSetObjectContext_NV20(VOID *arg)
{
    PGRSETOBJECTCONTEXTARG_000 pGrSetObjectContextArg = (PGRSETOBJECTCONTEXTARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrSetObjectContextArg->pHalHwInfo;
    U032 context, instance;
    U032 fifo;
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrSetObjectContext_NV20\r\n");

    //
    // Verify interface revision.
    //
    if (pGrSetObjectContextArg->id != GR_SET_OBJECT_CONTEXT_000)
        return (RM_ERR_VERSION_MISMATCH);

    //
    // Verify chid.
    //
    if (pGrSetObjectContextArg->chid > NUM_FIFOS_NV20)
        return (RM_ERR_BAD_ARGUMENT);

    //
    // Verify instance (what else can we do here without
    // keeping track of all objects that pass through the
    // nvHalGrAlloc interface down in the HAL?).
    //
    if (pGrSetObjectContextArg->instance == 0)
        return (RM_ERR_BAD_ARGUMENT);

    pHalHwInfo = pGrSetObjectContextArg->pHalHwInfo;
    instance = pGrSetObjectContextArg->instance;
    
    //
    // Fetch current context.
    //
    context = INST_RD32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0));

    //
    // Make necessary adjustments.
    //
    context &= pGrSetObjectContextArg->andMask;
    context |= pGrSetObjectContextArg->orMask;

    //
    // Write it back out                   
    //
    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0), context);
        
    //
    // Now we need to update the hardware context if appropriate.
    //
    if (pGrSetObjectContextArg->chid == REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID))
    {
        //
        // First disable the graphics fifo so we can get to the context rams
        //
        fifo = REG_RD32(NV_PGRAPH_FIFO);
        REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);
            
        //
        // Walk to subchannels and see if we find a match; update when we do
        //
        for (i = 0 ; i < 8 ; i++)
        {            
            if (REG_RD32(NV_PGRAPH_CTX_CACHE4(i)) == instance)
            {   
                //
                // Get the current settings
                //
                context = REG_RD32(NV_PGRAPH_CTX_CACHE1(i));

                //
                // Make necessary adjustments.
                //
                context &= pGrSetObjectContextArg->andMask;
                context |= pGrSetObjectContextArg->orMask;

                //
                // Write it back out                   
                //
                REG_WR32(NV_PGRAPH_CTX_CACHE1(i), context);
            }
        }                                    
                             
        //
        // See if the currently active subchannel also needs updating
        //
        if (REG_RD_DRF(_PGRAPH, _CTX_SWITCH4, _USER_INSTANCE) == instance)
        {   
            //
            // Get the current settings
            //
            context = REG_RD32(NV_PGRAPH_CTX_SWITCH1);

            //
            // Make necessary adjustments.
            //
            context &= pGrSetObjectContextArg->andMask;
            context |= pGrSetObjectContextArg->orMask;

            //
            // Write it back out                   
            //
            REG_WR32(NV_PGRAPH_CTX_SWITCH1, context);
        }
            
        //
        // Restore the fifo state
        //
        REG_WR32(NV_PGRAPH_FIFO, fifo);
        
    }    
    return (RM_OK);
}

static RM_STATUS
_nvHalGrInitObjectContext_NV20(
    PHALHWINFO  pHalHwInfo,
    U032        instance,
    U032        classNum
)
{
    U032 ctx1, ctx2, ctx3;

    // default _CTX_SWITCH1 value
    ctx1 = DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, classNum)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY_AND)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _INVALID);    

    // turn on big endian for this class if supported and we're in big endian mode
    if (pHalHwInfo->pMcHalInfo->EndianControl & MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN)
        ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _ENDIAN_MODE, _BIG);

    // default _CTX_SWITCH2 value
    ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);

    // default _CTX_SWITCH3 value
    ctx3 = DRF_DEF(_PGRAPH, _CTX_SWITCH3, _DMA_INSTANCE_0, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH3, _DMA_INSTANCE_1, _INVALID);

    switch (classNum)
    {
        case NV01_CONTEXT_BETA:
        case NV01_CONTEXT_CLIP_RECTANGLE:
        case NV01_IMAGE_BLIT:
        case NV03_CONTEXT_ROP:
        case NV04_CONTEXT_SURFACES_2D:
        case NV03_DX3_TEXTURED_TRIANGLE:
        case NV04_CONTEXT_SURFACE_SWIZZLED:
        case NV04_CONTEXT_SURFACES_3D:
        case NV03_CONTEXT_SURFACE_2D_DESTINATION:
        case NV03_CONTEXT_SURFACE_2D_SOURCE:
        case NV03_CONTEXT_SURFACE_3D_COLOR:
        case NV03_CONTEXT_SURFACE_3D_DEPTH:
        case NV04_IMAGE_BLIT:
        case NV04_CONTEXT_BETA:
        case NV10_CONTEXT_SURFACES_2D:
        case NV10_CONTEXT_SURFACES_3D:
        case NV15_CONTEXT_SURFACE_SWIZZLED:
        case NV15_IMAGE_BLIT:
            // defaults are fine
            break;
        case NV20_IMAGE_BLIT:
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CLASS_TYPE, _PERFORMANCE);
            break;
        case NV01_CONTEXT_COLOR_KEY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16A8Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_CONTEXT_PATTERN:
        case NV04_CONTEXT_PATTERN:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16A8Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_RENDER_SOLID_LIN:
        case NV01_RENDER_SOLID_TRIANGLE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X24Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_IMAGE_FROM_CPU:
        case NV03_STRETCHED_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV1_NULL:
            //
            // The NULL object is really a pseudo-dma object, so it needs a valid
            // limit value and should also have the PAGE_TABLE_PRESENT bit set.
            //
            ctx1 |= SF_DEF(_DMA, _PAGE_TABLE, _PRESENT);
            ctx2 = 0xffffffff;
            //INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_1), 0xFFFFFFFF);
            //FLD_WR_ISF_DEF(instance, _DMA, _PAGE_TABLE, _PRESENT);
            break;
        case NV03_SCALED_IMAGE_FROM_MEMORY:
        case NV04_SCALED_IMAGE_FROM_MEMORY:
        case NV10_SCALED_IMAGE_FROM_MEMORY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV11_SCALED_IMAGE_FROM_MEMORY:
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CLASS_TYPE, _PERFORMANCE);
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_DVD_SUBPICTURE:
        case NV10_DVD_SUBPICTURE:
            //
            // DVD class is patchless, so it is always considered valid.  HW doesn't
            // use this bit, but let's set it anyway to be consistent with other
            // objects
            //
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV03_MEMORY_TO_MEMORY_FORMAT:
            // Note that this object is always considered patch valid
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV04_GDI_RECTANGLE_TEXT:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV03_GDI_RECTANGLE_TEXT:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X24Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_CONTEXT_COLOR_KEY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_RENDER_SOLID_LIN:
        case NV04_RENDER_SOLID_TRIANGLE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_INDEXED_IMAGE_FROM_CPU:
        case NV04_IMAGE_FROM_CPU:
        case NV04_STRETCHED_IMAGE_FROM_CPU:
        case NV10_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_SCALED_IMAGE_FROM_MEMORY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_INDEXED_IMAGE_FROM_CPU:
        case NV05_IMAGE_FROM_CPU:
        case NV05_STRETCHED_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV11_CELSIUS_PRIMITIVE:
            //
            // The above DRF_NUM macro on GRCLASS has already masked this class
            // number to an 8bit value, we just need to set the PERFORMANCE bit.
            //
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CLASS_TYPE, _PERFORMANCE);
        case NV10_CELSIUS_PRIMITIVE:
        case NV15_CELSIUS_PRIMITIVE:
        case NV20_KELVIN_PRIMITIVE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV10_TEXTURE_FROM_CPU:
            // TEXTURE_FROM_CPU is similar to the IMAGE_FROM_CPU classes, but with
            // only a subset of the methods, so we'll need to make sure things are
            // initialized in a way that makes sense for this class.
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _DITHER_MODE, _TRUNCATE);    
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);

            break;
        default:
            // assume defaults are cool
            break;
    }


    //
    // Write out the values.
    //
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: instance data for classNum ", classNum);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "      instance offset ", instance);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "      ctx1 ", ctx1);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "      ctx2 ", ctx2);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "      ctx3 ", ctx3);

    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0), ctx1);
    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_1), ctx2);
    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_2), ctx3);
    //
    // Be sure to clear this dword clear to keep the graphics method
    // trapped disabled.
    //
    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_3), 0);

    return (RM_OK);
}

#if 0
//
// Given a pipe address and ChID return the corresponding pipe context save
// area in the channel context in instance memory.
//
static U032 *
grInitPipeContextAddr(
    PGRAPHICSCHANNEL_NV20 pGrChan,
    U032 base
)
{
    U032 index;

    if (base >= XL_XFREG_BASE && base < (XL_XFREG_BASE + (XL_XFREG_COUNT*4)))
    {
        index = (base - XL_XFREG_BASE) / sizeof (U032);
        return &pGrChan->grCtxPipe.XlXfReg[index];
    }
    if (base >= XL_XFCTX_BASE && base < (XL_XFCTX_BASE + (XL_XFCTX_COUNT*4)))
    {
        index = (base - XL_XFCTX_BASE) / sizeof (U032);
        return &pGrChan->grCtxPipe.XlXfCtx[index];
    }
    if (base >= XL_LTCTX_BASE && base < (XL_LTCTX_BASE + (XL_LTCTX_COUNT*4)))
    {
        index = (base - XL_LTCTX_BASE) / sizeof (U032);
        return &pGrChan->grCtxPipe.XlLtCtx[index];
    }
    if (base >= XL_LTC0_BASE && base < (XL_LTC0_BASE + (XL_LTC0_COUNT*4)))
    {
        index = (base - XL_LTC0_BASE) / sizeof (U032);
        return &pGrChan->grCtxPipe.XlLtc[index];
    }
    if (base >= XL_LTC1_BASE && base < (XL_LTC1_BASE + (XL_LTC1_COUNT*4)))
    {
        index = (base - XL_LTC1_BASE) / sizeof (U032);
        return &pGrChan->grCtxPipe.XlLtc[index + XL_LTC0_COUNT];
    }
    if (base >= XL_LTC2_BASE && base < (XL_LTC2_BASE + (XL_LTC2_COUNT*4)))
    {
        index = (base - XL_LTC2_BASE) / sizeof (U032);
        return &pGrChan->grCtxPipe.XlLtc[index + XL_LTC0_COUNT + XL_LTC1_COUNT];
    }
    if (base >= XL_LTC3_BASE && base < (XL_LTC3_BASE + (XL_LTC3_COUNT*4)))
    {
        index = (base - XL_LTC3_BASE) / sizeof (U032);
        return &pGrChan->grCtxPipe.XlLtc[index + XL_LTC0_COUNT + XL_LTC1_COUNT + XL_LTC2_COUNT];
    }

    return NULL;        // error
}

#define grInitPipeContextData(data)     *pipectx++ = data;
#endif

//
// Initialize 3d state in the specified channel.
// Note that both Celsius/Kelvin classes are initialized in the same
// manner.
//
// If we transition to supporting multiple 3d objects per channel,
// then this will need to be reworked.  We'll have to allocate the
// 3d context buffers with the 3d object and not with the channel.
//
// XXXscottl - Should we take steps to ensure we don't context switch
//             to this channel while we're in here?
//             Also, what if the specified channel is the current chid?
//
// See /hw/nv20/diag/common/commonIniLib.c for more info.
//
static RM_STATUS
_nvHalGrInit3d_NV20(
    PHALHWINFO  pHalHwInfo,
    U032        ChID
)
{
    PGRHALINFO_NV20 pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;
    U032 grCtxBuffer;
    U032 offset;
    U032 ctxUser;
    U032 cheopsCtxOffset, zoserCtxOffset;
    U032 zoserC0Offset, zoserC1Offset;
    U032 i, dmaPush;

    grCtxBuffer = pGrHalPvtInfo->CtxTable[ChID];

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: initializing 3d state: chid ", ChID);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      buffer offset ", grCtxBuffer);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      buffer size ", sizeof (GRAPHICSCHANNEL_NV20));
   
    //
    // If this is the current channel, then we need to flush the
    // current context out to instance memory where the following
    // initialization will occur.
    //
    if (pGrHalPvtInfo->currentChID == ChID)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: unload current chid ", ChID);

        // Empty FIFO and runout.
        while ((REG_RD_DRF(_PFIFO, _CACHE1_STATUS, _LOW_MARK) != NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY) ||
               (REG_RD_DRF(_PFIFO, _RUNOUT_STATUS, _LOW_MARK) != NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY) ||
               (REG_RD_DRF(_PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY))
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
        dmaPush = REG_RD32(NV_PFIFO_CACHE1_DMA_PUSH);
        REG_WR_DRF_DEF(_PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _DISABLED);

        // Wait until the DMA pusher is idle
        while (REG_RD_DRF(_PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY)
            ;
    
        // Now, wait until graphics is idle
        HAL_GR_IDLE(pHalHwInfo);

        (void) nvHalGrUnloadChannelContext_NV20(pHalHwInfo, ChID);
    }

    //
    // Initialize 3d channel state.
    // Start by setting 3D present bit in CTX_USER
    // (offset 0 in ctx buffer).
    //
    ctxUser = INST_RD32(grCtxBuffer, 0);
    ctxUser |= DRF_DEF(_PGRAPH, _CTX_USER, _CHANNEL_3D, _TRUE);
    INST_WR32(grCtxBuffer, 0, ctxUser);
    
    // Offset starts after state common to 2d and 3d.
    offset = sizeof (GRCTXCMN_NV20)/4;

    INST_WR32(grCtxBuffer, offset++<<2, 0xffff0000);   //Antialiasing
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Blend
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BlendColor
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BorderColor0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BorderColor1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BorderColor2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BorderColor3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat00_1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat00_2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat00_3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat01_1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat01_2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat01_3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat10_1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat10_2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat10_3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat11_1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat11_2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpMat11_3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpOffset1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpOffset2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpOffset3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpScale1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpScale2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BumpScale3
    INST_WR32(grCtxBuffer, offset++<<2, 0x0fff0000);   //ClearRectX
    INST_WR32(grCtxBuffer, offset++<<2, 0x0fff0000);   //ClearRectY
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ColorClearValue
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ColorKeyColor0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ColorKeyColor1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ColorKeyColor2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ColorKeyColor3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor0_0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor0_1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor0_2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor0_3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor0_4
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor0_5
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor0_6
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor0_7
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor1_0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor1_1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor1_2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor1_3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor1_4
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor1_5
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor1_6
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineFactor1_7
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaI0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaI1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaI2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaI3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaI4
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaI5
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaI6
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaI7
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaO0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaO1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaO2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaO3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaO4
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaO5
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaO6
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineAlphaO7
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorI0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorI1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorI2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorI3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorI4
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorI5
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorI6
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorI7
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorO0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorO1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorO2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorO3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorO4
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorO5
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorO6
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineColorO7
    INST_WR32(grCtxBuffer, offset++<<2, 0x101);        //CombineCtl
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineSpecFog0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CombineSpecFog1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Control0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Control1
    INST_WR32(grCtxBuffer, offset++<<2, 0x111);        //Control2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Control3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //FogColor
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //FogParam0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //FogParam1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //PointSize
    INST_WR32(grCtxBuffer, offset++<<2, 0x44400000);   //SetupRaster
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ShaderClipMode
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ShaderCtl
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ShaderProg
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //SemaphoreOffset
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ShadowCtl
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ShadowZslopeThreshold
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //SpecFogFactor0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //SpecFogFactor1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //SurfaceClipX
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //SurfaceClipY
    INST_WR32(grCtxBuffer, offset++<<2, 0x30303);      //TexAddress0
    INST_WR32(grCtxBuffer, offset++<<2, 0x30303);      //TexAddress1
    INST_WR32(grCtxBuffer, offset++<<2, 0x30303);      //TexAddress2
    INST_WR32(grCtxBuffer, offset++<<2, 0x30303);      //TexAddress3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexControl0_0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexControl0_1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexControl0_2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexControl0_3
    INST_WR32(grCtxBuffer, offset++<<2, 0x80000);      //TexControl1_0
    INST_WR32(grCtxBuffer, offset++<<2, 0x80000);      //TexControl1_1
    INST_WR32(grCtxBuffer, offset++<<2, 0x80000);      //TexControl1_2
    INST_WR32(grCtxBuffer, offset++<<2, 0x80000);      //TexControl1_3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexControl2_0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexControl2_1
    INST_WR32(grCtxBuffer, offset++<<2, 0x01012000);   //TexFilter0
    INST_WR32(grCtxBuffer, offset++<<2, 0x01012000);   //TexFilter1
    INST_WR32(grCtxBuffer, offset++<<2, 0x01012000);   //TexFilter2
    INST_WR32(grCtxBuffer, offset++<<2, 0x01012000);   //TexFilter3
    INST_WR32(grCtxBuffer, offset++<<2, 0x105b8);      //TexFormat0
    INST_WR32(grCtxBuffer, offset++<<2, 0x105b8);      //TexFormat1
    INST_WR32(grCtxBuffer, offset++<<2, 0x105b8);      //TexFormat2
    INST_WR32(grCtxBuffer, offset++<<2, 0x105b8);      //TexFormat3
    INST_WR32(grCtxBuffer, offset++<<2, 0x00080008);   //TexImageRect0
    INST_WR32(grCtxBuffer, offset++<<2, 0x00080008);   //TexImageRect1
    INST_WR32(grCtxBuffer, offset++<<2, 0x00080008);   //TexImageRect2
    INST_WR32(grCtxBuffer, offset++<<2, 0x00080008);   //TexImageRect3

    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexOffset0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexOffset1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexOffset2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexOffset3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexPalette0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexPalette1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexPalette2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //TexPalette3

    for (i = 0; i < 8; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0x07ff0000);    //WindowClipX

    for (i = 0; i < 8; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0x07ff0000);    //WindowClipY

    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ZCompressOcclude
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ZStencilClearValue
    INST_WR32(grCtxBuffer, offset++<<2, 0x4b7fffff);   //ZClipMax
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ZClipMin
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ContextDmaA
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ContextDmaB
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ContextDmaVtxA
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ContextDmaVtxB
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ZOffsetBias
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ZOffsetFactor
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Eyevec0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Eyevec1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Eyevec2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Shadow
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //FdData
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //FdSwatch
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //FdExtras
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Emission0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Emission1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Emission2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //SceneAmb0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //SceneAmb1
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //SceneAmb2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //ContextDmaGetState
    INST_WR32(grCtxBuffer, offset++<<2, 0x1);          //BeginPatch0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BeginPatch1
    INST_WR32(grCtxBuffer, offset++<<2, 0x4000);       //BeginPatch2
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BeginPatch3
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BeginCurve
    INST_WR32(grCtxBuffer, offset++<<2, 0x1);          //BeginTrans0
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //BeginTrans1
    INST_WR32(grCtxBuffer, offset++<<2, 0x00040000);   //Csv0_D
    INST_WR32(grCtxBuffer, offset++<<2, 0x00010000);   //Csv0_C
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Csv1_B
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //Csv1_A
    INST_WR32(grCtxBuffer, offset++<<2, 0);            //CheopsOffset

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      pipe offset ", offset);

    for (i = 0; i < IDX_FMT_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //IDX_FMT
    for (i = 0; i < CAS0_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //CAS0
    for (i = 0; i < CAS1_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //CAS1
    for (i = 0; i < CAS2_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //CAS2
    for (i = 0; i < ASSM_STATE_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //ASSM_STATE
    for (i = 0; i < IDX_CACHE0_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //IDX_CACHE0
    for (i = 0; i < IDX_CACHE1_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //IDX_CACHE1
    for (i = 0; i < IDX_CACHE2_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //IDX_CACHE2
    for (i = 0; i < IDX_CACHE3_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //IDX_CACHE3
    for (i = 0; i < VTX_FILE0_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //VTX_FILE0
    for (i = 0; i < VTX_FILE1_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //VTX_FILE1
    for (i = 0; i < XL_P_COUNT; i+=4)
    {
        INST_WR32(grCtxBuffer, offset++<<2, 0x10700ff9); //XL_P
        INST_WR32(grCtxBuffer, offset++<<2, 0x0436086c);
        INST_WR32(grCtxBuffer, offset++<<2, 0x000c001b);
        INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    }
    cheopsCtxOffset = offset;
    for (i = 0; i < XL_XFCTX_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //XL_XFCTX
    zoserCtxOffset = offset;
    for (i = 0; i < XL_LTCTX_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //XL_LTCTX
    zoserC0Offset = offset;
    for (i = 0; i < XL_LTC0_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //XL_LTC0
    zoserC1Offset = offset;
    for (i = 0; i < XL_LTC1_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //XL_LTC1
    for (i = 0; i < XL_LTC2_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //XL_LTC2
    for (i = 0; i < XL_LTC3_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //XL_LTC3
    for (i = 0; i < XL_VAB_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //XL_VAB
    for (i = 0; i < CRSTR_STIPP_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //CRSTR_STIPP
    for (i = 0; i < FD_CTRL_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //FD_CTRL
    for (i = 0; i < ROP_REGS_COUNT; i++)
        INST_WR32(grCtxBuffer, offset++<<2, 0);         //ROP_REGS

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      end pipe offset ", offset);

    // CHEOP_CTX, eye pos
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_EYEP<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x3f800000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // CHEOP_CTX_CONST0
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_CONS0<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x40000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x3f800000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x3f000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // CHEOP_CTX_CONST1
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_CONS1<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x40000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x3f800000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0xbf800000);

    // CHEOP_CTX_CONST2
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_CONS2<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0xbf800000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // CHEOP_CTX, fog plane
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_FOG<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x3f800000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // CHEOP_CTX_CONST3
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_CONS3<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_CTX_ZERO
    offset = zoserCtxOffset + (NV_IGRAPH_XF_LTCTX_ZERO<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_EYE_DIR (22bit words) -- see Simon Moy about this
    offset = zoserCtxOffset + (NV_IGRAPH_XF_LTCTX_EYED<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x3f800000>>10);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_FRONT_AMBIENT1
    offset = zoserCtxOffset + (NV_IGRAPH_XF_LTCTX_FR_AMB<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_FRONT_AMBIENT2
    offset = zoserCtxOffset + (NV_IGRAPH_XF_LTCTX_CM_COL<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_EYE_DIR (22bit words) -- see Simon Moy about this
    offset = zoserCtxOffset + (NV_IGRAPH_XF_LTCTX_FOGLIN<<2);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x3f800000>>20);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_C0 (22bit word) -- see Simon Moy about this
    offset = zoserC0Offset + (NV_IGRAPH_XF_LTC0_MONE);
    INST_WR32(grCtxBuffer, offset<<2, 0xbf800000>>10);

    // LIGHT0 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r0);
    INST_WR32(grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT1 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r1);
    INST_WR32(grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT2 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r2);
    INST_WR32(grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT3 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r3);
    INST_WR32(grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT4 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r4);
    INST_WR32(grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT5 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r5);
    INST_WR32(grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT6 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r6);
    INST_WR32(grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT7 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r7);
    INST_WR32(grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      last offset ", offset);

#ifdef NOTYET
    //
    // Enable the texture cache performance counters.
    // Should this be on by default?
    //
    FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_1, _CACHE_COUNTERS, _ENABLED);
#endif

    if (pGrHalPvtInfo->currentChID == ChID)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: reload current chid ", ChID);

        (void) nvHalGrLoadChannelContext_NV20(pHalHwInfo, ChID);

        // Restore the dma pusher state
        REG_WR32(NV_PFIFO_CACHE1_DMA_PUSH, dmaPush);
    }

    return (RM_OK);
}


//
// Destroy 3d state in specified channel.
//
static RM_STATUS
_nvHalGrDestroy3d_NV20
(
    PHALHWINFO  pHalHwInfo,
    U032        ChID
)
{
    PGRHALINFO_NV20 pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;
    U032 grCtxBuffer;
    U032 ctxUser, fifo;

    grCtxBuffer = pGrHalPvtInfo->CtxTable[ChID];

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: initializing 3d state: chid ", ChID);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      buffer offset ", grCtxBuffer);

    //
    // Clear 3d present bit in channel.
    //
    if (ChID == REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID))
    {
        //
        // First disable the graphics fifo so we can get to the context rams
        //
        fifo = REG_RD32(NV_PGRAPH_FIFO);
        REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);

        FLD_WR_DRF_DEF(_PGRAPH, _CTX_USER, _CHANNEL_3D, _FALSE);

        //
        // Restore the fifo state
        //
        REG_WR32(NV_PGRAPH_FIFO, fifo);
    }
    else
    {
        ctxUser = INST_RD32(grCtxBuffer, 0);
        ctxUser &= ~DRF_DEF(_PGRAPH, _CTX_USER, _CHANNEL_3D, _TRUE);
        INST_WR32(grCtxBuffer, 0, ctxUser);    
    }

    return (RM_OK);
}
