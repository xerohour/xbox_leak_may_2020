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
* Module: GRCXNV04.C                                                        *
*   The graphics engine is managed in this module.  Context switching is    *
*   contained here.                                                         *
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
#include "nvhw.h"
#include "nvhalcpp.h"

RM_STATUS nvHalGrLoadChannelContext_NV04(PHALHWINFO, U032);
RM_STATUS nvHalGrUnloadChannelContext_NV04(PHALHWINFO, U032);

//
// nvHalGrLoadChannelContext
//
RM_STATUS
nvHalGrLoadChannelContext_NV04(PHALHWINFO pHalHwInfo, U032 ChID)
{
    PGRHALINFO_NV04 pGrHalPvtInfo = (PGRHALINFO_NV04)pHalHwInfo->pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV04 pGrChan;
    U032 i;
    U032 misc;
    U032 caches, fifo0, fifo1;

    //
    // Disable FIFO access.
    //
    // KJK Me thinks this shouldn't be necessary
    //
    caches = REG_RD32(NV_PFIFO_CACHES);
    fifo0  = REG_RD32(NV_PFIFO_CACHE0_PULL0);
    fifo1  = REG_RD32(NV_PFIFO_CACHE1_PULL0);
    REG_WR_DRF_DEF(_PFIFO_, CACHES,       _REASSIGN, _DISABLED);
    REG_WR_DRF_DEF(_PFIFO_, CACHE0_PULL0, _ACCESS,   _DISABLED);
    REG_WR_DRF_DEF(_PFIFO_, CACHE1_PULL0, _ACCESS,   _DISABLED);

    //
    // Service any outstanding graphics exceptions.
    //
    if (REG_RD32(NV_PGRAPH_INTR))
        HALRMSERVICEINTR(pHalHwInfo, GR_ENGINE_TAG);
        
    //
    // Save current graphics interface state.
    //
    GR_SAVE_STATE(misc);
    HAL_GR_IDLE(pHalHwInfo);
    
    //
    // Unload current context.
    //   
    if (pGrHalPvtInfo->currentChID != ChID)
        nvHalGrUnloadChannelContext_NV04(pHalHwInfo, pGrHalPvtInfo->currentChID);

    if ((pGrHalPvtInfo->currentChID = ChID) == INVALID_CHID)
    {
        //
        // Set current channel to an invalid one.
        //
        FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, 0x0F);
        //
        // Update current channel.
        //    
        REG_WR32(NV_PGRAPH_CTX_CONTROL, DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
                                      | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _NOT_EXPIRED)
                                      | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
                                      | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                                      | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));
        REG_WR_DRF_DEF(_PGRAPH, _FFINTFC_ST2, _CHID_STATUS, _VALID);

        //
        // Restore graphics interface state.
        //
        misc |= DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _ENABLED);
        GR_RESTORE_STATE(misc); // FIFO reenabled in here
        //
        // Restore FIFO access.
        //
        /*
        REG_WR32(NV_PFIFO_CACHE0_PULL0, fifo0);
        REG_WR32(NV_PFIFO_CACHE1_PULL0, fifo1);
        REG_WR32(NV_PFIFO_CACHES,       caches);
        */
        return (RM_OK);
        //
        // Is the channel ID valid?
        //
    }
    else if (ChID >= NUM_FIFOS_NV04)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Invalid Channel on Graphics Context Switch: 0x%x\n", ChID));
        DBG_BREAKPOINT();
        return (RM_ERR_BAD_ARGUMENT);
    }

    //
    // Get pointer to correct channel context area.
    //
    pGrChan = &pGrHalPvtInfo->grChannels[ChID];

    //
    // Reload std user clip (NV4 dictates that this be restored before ctx switch)
    //
    REG_WR32(NV_PGRAPH_ABS_UCLIP_XMIN,  pGrChan->CurrentPatch.AbsUClipXMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_XMAX,  pGrChan->CurrentPatch.AbsUClipXMax);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_YMIN,  pGrChan->CurrentPatch.AbsUClipYMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_YMAX,  pGrChan->CurrentPatch.AbsUClipYMax);

    //
    // Reload context state
    //
    REG_WR32(NV_PGRAPH_CTX_SWITCH1, pGrChan->ContextSwitch1);
    REG_WR32(NV_PGRAPH_CTX_SWITCH2, pGrChan->ContextSwitch2);
    REG_WR32(NV_PGRAPH_CTX_SWITCH3, pGrChan->ContextSwitch3);
    REG_WR32(NV_PGRAPH_CTX_SWITCH4, pGrChan->ContextSwitch4);

    for (i = 0; i < 8; i++)
    {    
        REG_WR32(NV_PGRAPH_CTX_CACHE1(i), pGrChan->ContextCache1[i]);
        REG_WR32(NV_PGRAPH_CTX_CACHE2(i), pGrChan->ContextCache2[i]);
        REG_WR32(NV_PGRAPH_CTX_CACHE3(i), pGrChan->ContextCache3[i]);
        REG_WR32(NV_PGRAPH_CTX_CACHE4(i), pGrChan->ContextCache4[i]);
    }
    
    REG_WR32(NV_PGRAPH_CTX_USER, pGrChan->ContextUser);
    
    //
    // Reload current dma registers
    //
    REG_WR32(NV_PGRAPH_DMA_START_0, pGrChan->DmaStart0);
    REG_WR32(NV_PGRAPH_DMA_START_1, pGrChan->DmaStart1);
    REG_WR32(NV_PGRAPH_DMA_LENGTH,  pGrChan->DmaLength);
    REG_WR32(NV_PGRAPH_DMA_MISC,    pGrChan->DmaMisc);
    REG_WR32(NV_PGRAPH_DMA_PITCH,   pGrChan->DmaPitch);
            
    //
    // Reload current graphics state
    //
    for (i = 0; i < 6; i++)
    {    
        REG_WR32(NV_PGRAPH_BOFFSET(i),  pGrChan->CurrentPatch.BufferOffset[i]);
        REG_WR32(NV_PGRAPH_BBASE(i),    pGrChan->CurrentPatch.BufferBase[i]);
        REG_WR32(NV_PGRAPH_BLIMIT(i),   pGrChan->CurrentPatch.BufferLimit[i]);
    }
    
    for (i = 0; i < 5; i++)
        REG_WR32(NV_PGRAPH_BPITCH(i),   pGrChan->CurrentPatch.BufferPitch[i]);
        
    REG_WR32(NV_PGRAPH_SURFACE,         pGrChan->CurrentPatch.Surface);        
    REG_WR32(NV_PGRAPH_STATE,           pGrChan->CurrentPatch.State);        
    REG_WR32(NV_PGRAPH_BSWIZZLE2,       pGrChan->CurrentPatch.BufferSwizzle[0]);        
    REG_WR32(NV_PGRAPH_BSWIZZLE5,       pGrChan->CurrentPatch.BufferSwizzle[1]);        
    REG_WR32(NV_PGRAPH_BPIXEL,          pGrChan->CurrentPatch.BufferPixel);
    
    REG_WR32(NV_PGRAPH_NOTIFY,          pGrChan->CurrentPatch.Notify);
    
    REG_WR32(NV_PGRAPH_PATT_COLOR0,     pGrChan->CurrentPatch.PatternColor0);    
    REG_WR32(NV_PGRAPH_PATT_COLOR1,     pGrChan->CurrentPatch.PatternColor1);
    
    FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_3, _RAMREADBACK, _ENABLED);
    for (i = 0; i < 64; i++)
        REG_WR32(NV_PGRAPH_PATT_COLORRAM(i), pGrChan->CurrentPatch.PatternColorRam[i]);
    FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_3, _RAMREADBACK, _DISABLED);
        
    REG_WR32(NV_PGRAPH_PATTERN(0),      pGrChan->CurrentPatch.Pattern[0]);
    REG_WR32(NV_PGRAPH_PATTERN(1),      pGrChan->CurrentPatch.Pattern[1]);
    REG_WR32(NV_PGRAPH_PATTERN_SHAPE,   pGrChan->CurrentPatch.PatternShape);
    
    REG_WR32(NV_PGRAPH_MONO_COLOR0,     pGrChan->CurrentPatch.MonoColor0);
    REG_WR32(NV_PGRAPH_ROP3,            pGrChan->CurrentPatch.Rop3);
    REG_WR32(NV_PGRAPH_CHROMA,          pGrChan->CurrentPatch.Chroma);
    REG_WR32(NV_PGRAPH_BETA_AND,        pGrChan->CurrentPatch.BetaAnd);
    REG_WR32(NV_PGRAPH_BETA_PREMULT,    pGrChan->CurrentPatch.BetaPremult);
    
    REG_WR32(NV_PGRAPH_CONTROL0,        pGrChan->CurrentPatch.Control0);
    REG_WR32(NV_PGRAPH_CONTROL1,        pGrChan->CurrentPatch.Control1);
    REG_WR32(NV_PGRAPH_CONTROL2,        pGrChan->CurrentPatch.Control2);
    
    REG_WR32(NV_PGRAPH_BLEND,           pGrChan->CurrentPatch.Blend);
    
    REG_WR32(NV_PGRAPH_STORED_FMT,      pGrChan->CurrentPatch.StoredFmt);
    REG_WR32(NV_PGRAPH_SOURCE_COLOR,    pGrChan->CurrentPatch.SourceColor);
    
    for (i = 0; i < 32; i++)
    {
        REG_WR32(NV_PGRAPH_ABS_X_RAM(i), pGrChan->CurrentPatch.AbsXRam[i]);
        REG_WR32(NV_PGRAPH_ABS_Y_RAM(i), pGrChan->CurrentPatch.AbsYRam[i]);
    }        
    
    REG_WR32(NV_PGRAPH_ABS_UCLIPA_XMIN, pGrChan->CurrentPatch.AbsUClipAXMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIPA_XMAX, pGrChan->CurrentPatch.AbsUClipAXMax);
    REG_WR32(NV_PGRAPH_ABS_UCLIPA_YMIN, pGrChan->CurrentPatch.AbsUClipAYMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIPA_YMAX, pGrChan->CurrentPatch.AbsUClipAYMax);
    
    REG_WR32(NV_PGRAPH_ABS_ICLIP_XMAX,  pGrChan->CurrentPatch.AbsIClipXMax);
    REG_WR32(NV_PGRAPH_ABS_ICLIP_YMAX,  pGrChan->CurrentPatch.AbsIClipYMax);
    
    REG_WR32(NV_PGRAPH_XY_LOGIC_MISC0,  pGrChan->CurrentPatch.XYLogicMisc0);
    REG_WR32(NV_PGRAPH_XY_LOGIC_MISC1,  pGrChan->CurrentPatch.XYLogicMisc1);
    REG_WR32(NV_PGRAPH_XY_LOGIC_MISC2,  pGrChan->CurrentPatch.XYLogicMisc2);
    REG_WR32(NV_PGRAPH_XY_LOGIC_MISC3,  pGrChan->CurrentPatch.XYLogicMisc3);

    REG_WR32(NV_PGRAPH_CLIPX_0,         pGrChan->CurrentPatch.ClipX0);
    REG_WR32(NV_PGRAPH_CLIPX_1,         pGrChan->CurrentPatch.ClipX1);
    REG_WR32(NV_PGRAPH_CLIPY_0,         pGrChan->CurrentPatch.ClipY0);
    REG_WR32(NV_PGRAPH_CLIPY_1,         pGrChan->CurrentPatch.ClipY1);
    
    for (i = 0; i < 16; i++)
    {
        REG_WR32(NV_PGRAPH_U_RAM(i),    pGrChan->CurrentPatch.URam[i]);
        REG_WR32(NV_PGRAPH_V_RAM(i),    pGrChan->CurrentPatch.VRam[i]);
        REG_WR32(NV_PGRAPH_M_RAM(i),    pGrChan->CurrentPatch.MRam[i]);
    }
    
    REG_WR32(NV_PGRAPH_COMBINE0ALPHA,   pGrChan->CurrentPatch.Combine0Alpha);
    REG_WR32(NV_PGRAPH_COMBINE0COLOR,   pGrChan->CurrentPatch.Combine0Color);
    REG_WR32(NV_PGRAPH_COMBINE1ALPHA,   pGrChan->CurrentPatch.Combine1Alpha);
    REG_WR32(NV_PGRAPH_COMBINE1COLOR,   pGrChan->CurrentPatch.Combine1Color);
    
    REG_WR32(NV_PGRAPH_FORMAT0,         pGrChan->CurrentPatch.Format0);
    REG_WR32(NV_PGRAPH_FORMAT1,         pGrChan->CurrentPatch.Format1);
    REG_WR32(NV_PGRAPH_FILTER0,         pGrChan->CurrentPatch.Filter0);
    REG_WR32(NV_PGRAPH_FILTER1,         pGrChan->CurrentPatch.Filter1);
    
    REG_WR32(NV_PGRAPH_D3D_XY,          pGrChan->CurrentPatch.D3D_XY);
    REG_WR32(NV_PGRAPH_D3D_U0,          pGrChan->CurrentPatch.D3D_U0);
    REG_WR32(NV_PGRAPH_D3D_V0,          pGrChan->CurrentPatch.D3D_V0);
    REG_WR32(NV_PGRAPH_D3D_U1,          pGrChan->CurrentPatch.D3D_U1);
    REG_WR32(NV_PGRAPH_D3D_V1,          pGrChan->CurrentPatch.D3D_V1);
    REG_WR32(NV_PGRAPH_D3D_ZETA,        pGrChan->CurrentPatch.D3D_ZETA);
    REG_WR32(NV_PGRAPH_D3D_RGB,         pGrChan->CurrentPatch.D3D_RGB);
    REG_WR32(NV_PGRAPH_D3D_S,           pGrChan->CurrentPatch.D3D_S);
    REG_WR32(NV_PGRAPH_D3D_M,           pGrChan->CurrentPatch.D3D_M);

    REG_WR32(NV_PGRAPH_PASSTHRU_0,      pGrChan->CurrentPatch.Passthru0);    
    REG_WR32(NV_PGRAPH_PASSTHRU_1,      pGrChan->CurrentPatch.Passthru1);    
    REG_WR32(NV_PGRAPH_PASSTHRU_2,      pGrChan->CurrentPatch.Passthru2);
    REG_WR32(NV_PGRAPH_DVD_COLORFMT,    pGrChan->CurrentPatch.DVDColorFormat);
    REG_WR32(NV_PGRAPH_SCALED_FORMAT,   pGrChan->CurrentPatch.ScaledFormat);
    REG_WR32(NV_PGRAPH_MISC24_0,        pGrChan->CurrentPatch.Misc24_0);
    REG_WR32(NV_PGRAPH_MISC24_1,        pGrChan->CurrentPatch.Misc24_1);
    REG_WR32(NV_PGRAPH_MISC24_2,        pGrChan->CurrentPatch.Misc24_2);
    REG_WR32(NV_PGRAPH_X_MISC,          pGrChan->CurrentPatch.XMisc);
    REG_WR32(NV_PGRAPH_Y_MISC,          pGrChan->CurrentPatch.YMisc);
    REG_WR32(NV_PGRAPH_VALID1,          pGrChan->CurrentPatch.Valid1);
    REG_WR32(NV_PGRAPH_VALID2,          pGrChan->CurrentPatch.Valid2);

    // restore the PGRAPH_DEBUG_3_POSTDITHER_2D setting
    REG_WR32(NV_PGRAPH_DEBUG_3,  pGrChan->Debug3);

    //
    // Update current channel.
    //    
    REG_WR32(NV_PGRAPH_CTX_CONTROL, DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _NOT_EXPIRED)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _VALID)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));
                                  
    FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, ChID);
    
    //
    // Clear the graphics fifo channel context.  This fixes a subtle hardware bug
    // when thrashing channel changes within the graphics fifo.  Ideally the fifo
    // would hold off pending data while a change is active, but it can actually
    // hold multiple changes.  In order to reduce hw confusion, they added a few
    // state bits within one stage fifo (bits 20 and 21).  
    // Clear those bits on channel changes. Don't rely on these to read as zero
    // eventhough these bits are write only.
    //
    // We have to OR in the top bit to fool the Watcom compiler for the Win3.1 version
    // of the resmgr.
    //
    REG_WR32(NV_PGRAPH_FFINTFC_ST2, REG_RD32(NV_PGRAPH_FFINTFC_ST2) & 0xffCfffff);

    // Restore graphics interface state.
    //
    GR_RESTORE_STATE(misc);
    
    //
    // Restore FIFO access.
    //
    REG_WR32(NV_PFIFO_CACHE0_PULL0, fifo0);
    REG_WR32(NV_PFIFO_CACHE1_PULL0, fifo1);
    REG_WR32(NV_PFIFO_CACHES, caches);

    return (RM_OK);
}


RM_STATUS
nvHalGrUnloadChannelContext_NV04(PHALHWINFO pHalHwInfo, U032 ChID)
{
    PGRHALINFO_NV04 pGrHalPvtInfo = (PGRHALINFO_NV04)pHalHwInfo->pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV04 pGrChan;
    U032 i;

    //
    // No need to unload an invalid channel
    //
    if (ChID  == INVALID_CHID)
        return (RM_OK);

    pGrChan = &pGrHalPvtInfo->grChannels[ChID];

    //
    // Unload current context registers.
    //
    pGrChan->ContextSwitch1    = REG_RD32(NV_PGRAPH_CTX_SWITCH1);
    pGrChan->ContextSwitch2    = REG_RD32(NV_PGRAPH_CTX_SWITCH2);
    pGrChan->ContextSwitch3    = REG_RD32(NV_PGRAPH_CTX_SWITCH3);        
    pGrChan->ContextSwitch4    = REG_RD32(NV_PGRAPH_CTX_SWITCH4);

    for (i = 0; i < 8; i++)
    {    
        pGrChan->ContextCache1[i] = REG_RD32(NV_PGRAPH_CTX_CACHE1(i));
        pGrChan->ContextCache2[i] = REG_RD32(NV_PGRAPH_CTX_CACHE2(i));
        pGrChan->ContextCache3[i] = REG_RD32(NV_PGRAPH_CTX_CACHE3(i));
        pGrChan->ContextCache4[i] = REG_RD32(NV_PGRAPH_CTX_CACHE4(i));
    }
    
    pGrChan->ContextUser       = REG_RD32(NV_PGRAPH_CTX_USER);
    
    //
    // Unload current dma registers
    //
    pGrChan->DmaStart0         = REG_RD32(NV_PGRAPH_DMA_START_0);
    pGrChan->DmaStart1         = REG_RD32(NV_PGRAPH_DMA_START_1);
    pGrChan->DmaLength         = REG_RD32(NV_PGRAPH_DMA_LENGTH);
    pGrChan->DmaMisc           = REG_RD32(NV_PGRAPH_DMA_MISC);
    pGrChan->DmaPitch          = REG_RD32(NV_PGRAPH_DMA_PITCH);
            
    //
    // Unload current graphics state
    //
    for (i = 0; i < 6; i++)
    {    
        pGrChan->CurrentPatch.BufferOffset[i] = REG_RD32(NV_PGRAPH_BOFFSET(i));
        pGrChan->CurrentPatch.BufferBase[i]   = REG_RD32(NV_PGRAPH_BBASE(i));
        pGrChan->CurrentPatch.BufferLimit[i]  = REG_RD32(NV_PGRAPH_BLIMIT(i));
    }
    
    for (i = 0; i < 5; i++)
        pGrChan->CurrentPatch.BufferPitch[i]  = REG_RD32(NV_PGRAPH_BPITCH(i));
        
    pGrChan->CurrentPatch.Surface             = REG_RD32(NV_PGRAPH_SURFACE);
    pGrChan->CurrentPatch.State               = REG_RD32(NV_PGRAPH_STATE);
    pGrChan->CurrentPatch.BufferSwizzle[0]    = REG_RD32(NV_PGRAPH_BSWIZZLE2);
    pGrChan->CurrentPatch.BufferSwizzle[1]    = REG_RD32(NV_PGRAPH_BSWIZZLE5);
    pGrChan->CurrentPatch.BufferPixel         = REG_RD32(NV_PGRAPH_BPIXEL);
    pGrChan->CurrentPatch.Notify              = REG_RD32(NV_PGRAPH_NOTIFY);
        pGrChan->CurrentPatch.PatternColor0       = REG_RD32(NV_PGRAPH_PATT_COLOR0);    
    pGrChan->CurrentPatch.PatternColor1       = REG_RD32(NV_PGRAPH_PATT_COLOR1);
    
    FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_3, _RAMREADBACK, _ENABLED);
    for (i = 0; i < 64; i++)
        pGrChan->CurrentPatch.PatternColorRam[i] = REG_RD32(NV_PGRAPH_PATT_COLORRAM(i));
    FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_3, _RAMREADBACK, _DISABLED);
    
    pGrChan->CurrentPatch.Pattern[0]          = REG_RD32(NV_PGRAPH_PATTERN(0));
    pGrChan->CurrentPatch.Pattern[1]          = REG_RD32(NV_PGRAPH_PATTERN(1));
    pGrChan->CurrentPatch.PatternShape        = REG_RD32(NV_PGRAPH_PATTERN_SHAPE);
    
    pGrChan->CurrentPatch.MonoColor0          = REG_RD32(NV_PGRAPH_MONO_COLOR0);
    pGrChan->CurrentPatch.Rop3                = REG_RD32(NV_PGRAPH_ROP3);
    pGrChan->CurrentPatch.Chroma              = REG_RD32(NV_PGRAPH_CHROMA);
    pGrChan->CurrentPatch.BetaAnd             = REG_RD32(NV_PGRAPH_BETA_AND);
    pGrChan->CurrentPatch.BetaPremult         = REG_RD32(NV_PGRAPH_BETA_PREMULT);
    
    pGrChan->CurrentPatch.Control0            = REG_RD32(NV_PGRAPH_CONTROL0);
    pGrChan->CurrentPatch.Control1            = REG_RD32(NV_PGRAPH_CONTROL1);
    pGrChan->CurrentPatch.Control2            = REG_RD32(NV_PGRAPH_CONTROL2);
    
    pGrChan->CurrentPatch.Blend               = REG_RD32(NV_PGRAPH_BLEND);
    
    pGrChan->CurrentPatch.StoredFmt           = REG_RD32(NV_PGRAPH_STORED_FMT);
    pGrChan->CurrentPatch.SourceColor         = REG_RD32(NV_PGRAPH_SOURCE_COLOR);
    
    for (i = 0; i < 32; i++)
    {
        pGrChan->CurrentPatch.AbsXRam[i]      = REG_RD32(NV_PGRAPH_ABS_X_RAM(i));
        pGrChan->CurrentPatch.AbsYRam[i]      = REG_RD32(NV_PGRAPH_ABS_Y_RAM(i));
    }        
    
    pGrChan->CurrentPatch.AbsUClipXMin        = REG_RD32(NV_PGRAPH_ABS_UCLIP_XMIN);
    pGrChan->CurrentPatch.AbsUClipXMax        = REG_RD32(NV_PGRAPH_ABS_UCLIP_XMAX);
    pGrChan->CurrentPatch.AbsUClipYMin        = REG_RD32(NV_PGRAPH_ABS_UCLIP_YMIN);
    pGrChan->CurrentPatch.AbsUClipYMax        = REG_RD32(NV_PGRAPH_ABS_UCLIP_YMAX);
    
    pGrChan->CurrentPatch.AbsUClipAXMin       = REG_RD32(NV_PGRAPH_ABS_UCLIPA_XMIN);
    pGrChan->CurrentPatch.AbsUClipAXMax       = REG_RD32(NV_PGRAPH_ABS_UCLIPA_XMAX);
    pGrChan->CurrentPatch.AbsUClipAYMin       = REG_RD32(NV_PGRAPH_ABS_UCLIPA_YMIN);
    pGrChan->CurrentPatch.AbsUClipAYMax       = REG_RD32(NV_PGRAPH_ABS_UCLIPA_YMAX);
    
    pGrChan->CurrentPatch.AbsIClipXMax        = REG_RD32(NV_PGRAPH_ABS_ICLIP_XMAX);
    pGrChan->CurrentPatch.AbsIClipYMax        = REG_RD32(NV_PGRAPH_ABS_ICLIP_YMAX);
    
    pGrChan->CurrentPatch.XYLogicMisc0        = REG_RD32(NV_PGRAPH_XY_LOGIC_MISC0);
    pGrChan->CurrentPatch.XYLogicMisc1        = REG_RD32(NV_PGRAPH_XY_LOGIC_MISC1);
    pGrChan->CurrentPatch.XYLogicMisc2        = REG_RD32(NV_PGRAPH_XY_LOGIC_MISC2);
    pGrChan->CurrentPatch.XYLogicMisc3        = REG_RD32(NV_PGRAPH_XY_LOGIC_MISC3);

    pGrChan->CurrentPatch.ClipX0              = REG_RD32(NV_PGRAPH_CLIPX_0);
    pGrChan->CurrentPatch.ClipX1              = REG_RD32(NV_PGRAPH_CLIPX_1);
    pGrChan->CurrentPatch.ClipY0              = REG_RD32(NV_PGRAPH_CLIPY_0);
    pGrChan->CurrentPatch.ClipY1              = REG_RD32(NV_PGRAPH_CLIPY_1);
    
    for (i = 0; i < 16; i++)
    {
        pGrChan->CurrentPatch.URam[i]         = REG_RD32(NV_PGRAPH_U_RAM(i));
        pGrChan->CurrentPatch.VRam[i]         = REG_RD32(NV_PGRAPH_V_RAM(i));
        pGrChan->CurrentPatch.MRam[i]         = REG_RD32(NV_PGRAPH_M_RAM(i));
    }
    
    pGrChan->CurrentPatch.Combine0Alpha       = REG_RD32(NV_PGRAPH_COMBINE0ALPHA);
    pGrChan->CurrentPatch.Combine0Color       = REG_RD32(NV_PGRAPH_COMBINE0COLOR);
    pGrChan->CurrentPatch.Combine1Alpha       = REG_RD32(NV_PGRAPH_COMBINE1ALPHA);
    pGrChan->CurrentPatch.Combine1Color       = REG_RD32(NV_PGRAPH_COMBINE1COLOR);
    
    pGrChan->CurrentPatch.Format0             = REG_RD32(NV_PGRAPH_FORMAT0);
    pGrChan->CurrentPatch.Format1             = REG_RD32(NV_PGRAPH_FORMAT1);
    pGrChan->CurrentPatch.Filter0             = REG_RD32(NV_PGRAPH_FILTER0);
    pGrChan->CurrentPatch.Filter1             = REG_RD32(NV_PGRAPH_FILTER1);
    
    pGrChan->CurrentPatch.D3D_XY              = REG_RD32(NV_PGRAPH_D3D_XY);
    pGrChan->CurrentPatch.D3D_U0              = REG_RD32(NV_PGRAPH_D3D_U0);
    pGrChan->CurrentPatch.D3D_V0              = REG_RD32(NV_PGRAPH_D3D_V0);
    pGrChan->CurrentPatch.D3D_U1              = REG_RD32(NV_PGRAPH_D3D_U1);
    pGrChan->CurrentPatch.D3D_V1              = REG_RD32(NV_PGRAPH_D3D_V1);
    pGrChan->CurrentPatch.D3D_ZETA            = REG_RD32(NV_PGRAPH_D3D_ZETA);
    pGrChan->CurrentPatch.D3D_RGB             = REG_RD32(NV_PGRAPH_D3D_RGB);
    pGrChan->CurrentPatch.D3D_S               = REG_RD32(NV_PGRAPH_D3D_S);
    pGrChan->CurrentPatch.D3D_M               = REG_RD32(NV_PGRAPH_D3D_M);

    pGrChan->CurrentPatch.Passthru0           = REG_RD32(NV_PGRAPH_PASSTHRU_0);    
    pGrChan->CurrentPatch.Passthru1           = REG_RD32(NV_PGRAPH_PASSTHRU_1);    
    pGrChan->CurrentPatch.Passthru2           = REG_RD32(NV_PGRAPH_PASSTHRU_2);    
    
    pGrChan->CurrentPatch.DVDColorFormat      = REG_RD32(NV_PGRAPH_DVD_COLORFMT);    
    pGrChan->CurrentPatch.ScaledFormat        = REG_RD32(NV_PGRAPH_SCALED_FORMAT);    
    
    pGrChan->CurrentPatch.Misc24_0            = REG_RD32(NV_PGRAPH_MISC24_0);
    pGrChan->CurrentPatch.Misc24_1            = REG_RD32(NV_PGRAPH_MISC24_1);
    pGrChan->CurrentPatch.Misc24_2            = REG_RD32(NV_PGRAPH_MISC24_2);
    pGrChan->CurrentPatch.XMisc               = REG_RD32(NV_PGRAPH_X_MISC);
    pGrChan->CurrentPatch.YMisc               = REG_RD32(NV_PGRAPH_Y_MISC);
    pGrChan->CurrentPatch.Valid1              = REG_RD32(NV_PGRAPH_VALID1);
    pGrChan->CurrentPatch.Valid2              = REG_RD32(NV_PGRAPH_VALID2);
    
    // save the PGRAPH_DEBUG_3_POSTDITHER_2D setting
    pGrChan->Debug3              = REG_RD32(NV_PGRAPH_DEBUG_3);

    //
    // Invalidate hw's channel ID.
    //
    REG_WR32(NV_PGRAPH_CTX_CONTROL, DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _EXPIRED)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));
    FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, 0x0F);

    return (RM_OK);
}
