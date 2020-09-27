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
* Module: GRCTXT.C                                                          *
*   The graphics engine is managed in this module.  Context switching is    *
*   contained here.                                                         *
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

// forwards
RM_STATUS nvHalGrLoadChannelContext_NV10(PHALHWINFO, U032);
RM_STATUS nvHalGrUnloadChannelContext_NV10(PHALHWINFO, U032, PGRAPHICSCHANNEL_NV10);
VOID nvHalGrLoadHW(PHALHWINFO, U032, PGRAPHICSCHANNEL_NV10);
RM_STATUS nvHalGrGetState_NV10(PHALHWINFO, PGREXCEPTIONDATA, U032);
RM_STATUS nvHalGrPutState_NV10(PHALHWINFO, PGREXCEPTIONDATA, U032);
RM_STATUS nvHalGrSetMaterialEmission_NV10(PHALHWINFO, PGREXCEPTIONDATA);

// externs
extern RM_STATUS dmaHalInstanceToHalInfo_NV10(U032, U032, PDMAHALOBJINFO *, PHALHWINFO);

static RM_STATUS grDelay_NV10(PHALHWINFO, U032);

// values for enabling passthrough modes
static U032 Vertex_Passthr_Mode[] = {
    0x10000000,
    0x00000000
};
#define VERTEX_COUNT            (sizeof(Vertex_Passthr_Mode) / sizeof(U032))
static U032 ScaleOffset_Passthr_Mode[] = {
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x0,
    0x0,
    0x0,
    0x0
};
#define SCALEOFFSET_COUNT       (sizeof(ScaleOffset_Passthr_Mode) / sizeof(U032))

static U032 Fog_Passthr_Mode[2][3] = {
    { 0x0,        0x0,        0x0 },           // passthrough mode values for A01
    { 0x3F800000, 0x3F800000, 0x3F800000 }     // passthrough mode values for >= A02
};
#define FOG_COUNT               (sizeof(Fog_Passthr_Mode[0]) / sizeof(U032))

static U032 Fog_EyeDist_Passthr_Mode[] = {
    0x0,
    0x0,
    0x0
};
#define FOG_EYEDIST_COUNT        (sizeof(Fog_EyeDist_Passthr_Mode) / sizeof(U032))

//
// Second Inverse ModelViewMatrix location in the XF engine needed for HW
// workaround both for RM related context switching and GetState context
// switching (see notes in grLoadChannelContext for details).
// 
#define INV2_MATRIX_OFFSET    0x653C

//
// Load Pipe Context
//
static RM_STATUS
grLoadPipeContext(
    PHALHWINFO pHalHwInfo,
    U032       Base,
    U032      *SaveAddr,
    U032       Count
)
{
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;
    U032 i;

    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, Base);
    for (i = 0; i < Count; i++) {
	  REG_WR32(NV_PGRAPH_PIPE_DATA, SaveAddr[i]);
    }
    return (RM_OK);
}

//
// Unload Pipe Context
//
static RM_STATUS
grUnloadPipeContext(
    PHALHWINFO pHalHwInfo,
    U032       Base,
    U032      *SaveAddr,
    U032       Count
)
{
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;
    U032 i;

    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, Base);
    for (i = 0; i < Count; i++) {
	  SaveAddr[i] = REG_RD32(NV_PGRAPH_PIPE_DATA);
    }
    return (RM_OK);
}

#define Z16_OVR_MASK (DRF_MASK(NV_PGRAPH_SURFACE_Z16_OVERRIDE) <<    \
                      DRF_SHIFT(NV_PGRAPH_SURFACE_Z16_OVERRIDE))

//
// We're in the midst of a context switch loading in a new 3D channel that
// needs a Celsius method resent to workaround a HW problem. We either need
// to resubmit a Vertex Buffer, or a NOP to setup a correct load of the
// Z16_OVERRIDE bit in PGRAPH_SURFACE.
//
//  1) switch to the Celsius subchannel for the new channel
//  2) save off the method in the stage 2 GR fifo
//  3) load the Celsius method in the stage 2 register
//  4) reenable enough of the GR fifo, so this method/data goes through
//  5) wait for the value to show-up, then disable the GR fifo again
//  6) reload the stage 2 register
//  7) switch back to the correct subchannel before the context switch
//
// Note: Eventhough we're checking for NV11_CELSIUS_PRIMITIVE, we'll never
//       match on that value, since the HW can only hold an 8 bit value and
//       a NV11's class number is 16 bits. But, because 0x1196 will look
//       like 0x96 (NV15's value), the class will match and should be ok.
//
static VOID
grResubmitCelsiusMethod(
    PHALHWINFO pHalHwInfo,
    U032       ChID,            // new ChID
    BOOL       resendDmaVtx
)
{
    PGRHALINFO_NV10 pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV10 pGrChan = &pGrHalPvtInfo->grChannels[ChID];    
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;
    U032 grclass, celsius;
    U032 st2, datalo, datahi, z16_data=0;

    //
    // On the initial load of this channel, we don't have any saved context, so
    // applying one of these workarounds doesn't seem possible. But really, it's
    // unnecessary, since the HW's going to receive the state setting methods
    // anyways.
    //
    if (DRF_VAL(_PGRAPH, _CTX_USER, _CHID, pGrChan->ContextUser) != ChID)
        return;

    // mark this new channel as owning graphics
    REG_WR32(NV_PGRAPH_CTX_USER, pGrChan->ContextUser);

    if (resendDmaVtx) {
        //
        // Determine which is the Celsius subchan and make it current
        //
        for (celsius = 0; celsius < NUM_SUBCHANNELS; celsius++)
        {
            grclass = DRF_VAL(_PGRAPH, _CTX_CACHE1, _GRCLASS, pGrChan->ContextCache1[celsius]);
            if ((grclass == NV10_CELSIUS_PRIMITIVE) ||
                (grclass == NV15_CELSIUS_PRIMITIVE) ||
                (grclass == NV11_CELSIUS_PRIMITIVE))
                    break;
        }
        if (celsius == NUM_SUBCHANNELS)
            return;     // couldn't find the Celsius object loaded

    } else {
        // use the existing subchan (all classes can handle a NOP)
        celsius = REG_RD_DRF(_PGRAPH, _CTX_USER, _SUBCH);
    }

    //
    // Make the Celsius object current by copying the saved subchan
    // state for CTX_CACHE(celsius) into the CTX_SWITCH registers.
    //
    REG_WR32(NV_PGRAPH_CTX_SWITCH1, pGrChan->ContextCache1[celsius]);
    REG_WR32(NV_PGRAPH_CTX_SWITCH2, pGrChan->ContextCache2[celsius]);
    REG_WR32(NV_PGRAPH_CTX_SWITCH3, pGrChan->ContextCache3[celsius]);
    REG_WR32(NV_PGRAPH_CTX_SWITCH4, pGrChan->ContextCache4[celsius]);
    REG_WR32(NV_PGRAPH_CTX_SWITCH5, pGrChan->ContextCache5[celsius]);

    FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _SUBCH, celsius);

    // save the stage2 values, which'll be reloaded after the method's been sent
    st2     = REG_RD32(NV_PGRAPH_FFINTFC_ST2);
    datalo  = REG_RD32(NV_PGRAPH_FFINTFC_ST2_DL);
    datahi  = REG_RD32(NV_PGRAPH_FFINTFC_ST2_DH);

    if (resendDmaVtx) {
        // Set an invalid instance address in GLOBALSTATE1
        FLD_WR_DRF_DEF(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_2, _INVALID);

        // Load the method/data into the stage2 register
        REG_WR32(NV_PGRAPH_FFINTFC_ST2,                         NV056_SET_CONTEXT_DMA_VERTEX    |
                 DRF_NUM(_PGRAPH, _FFINTFC_ST2, _SUBCH,         celsius)                        |
                 DRF_NUM(_PGRAPH, _FFINTFC_ST2, _CHID,          ChID)                           |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _DATAHIGH,      _INVALID)                       |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _STATUS,        _VALID)                         |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _CHID_STATUS,   _VALID)                         |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _CHSWITCH,      _SET)                           |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _FIFOHOLD,      _SET));

        REG_WR32(NV_PGRAPH_FFINTFC_ST2_DL,
                 DRF_VAL(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_2, pGrChan->CurrentPatch.GlobalState1));
    } else {    // resend Z16 override for NV11
        //        
        // Flip all the bits that are in trapped data currently, mask
        // off the bit we want and or in the bit we want to show up in
        // Z16 override
        //
        z16_data  =  (REG_RD32(NV_PGRAPH_TRAPPED_DATA_LOW) ^ 0xFFFFFFFF);
        z16_data  &= ~Z16_OVR_MASK;
        z16_data  |= (Z16_OVR_MASK & pGrChan->CurrentPatch.Surface);

        // Load the method/data into the stage2 register
        REG_WR32(NV_PGRAPH_FFINTFC_ST2,                         NV1196_NO_OPERATION             |
                 DRF_NUM(_PGRAPH, _FFINTFC_ST2, _SUBCH,         celsius)                        |
                 DRF_NUM(_PGRAPH, _FFINTFC_ST2, _CHID,          ChID)                           |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _DATAHIGH,      _INVALID)                       |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _STATUS,        _VALID)                         |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _CHID_STATUS,   _VALID)                         |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _CHSWITCH,      _SET)                           |
                 DRF_DEF(_PGRAPH, _FFINTFC_ST2, _FIFOHOLD,      _SET));

        // Method data is either 0x0 or 0x1, depending on desired value of Z16_OVERRIDE
        REG_WR32(NV_PGRAPH_FFINTFC_ST2_DL, z16_data);
    }

    // Briefly enable PGRAPH_FIFO_ACCESS (and mark the channel valid),
    // so our method can come through
    FLD_WR_DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID, _VALID);
    FLD_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _ENABLED);

    if (resendDmaVtx) {
        // spin until we see our instance address show up in the register
        while (REG_RD_DRF(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_2) !=
               DRF_VAL(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_2, pGrChan->CurrentPatch.GlobalState1))
            ;
    } else {
        // spin until we see the NOP method and data in the trapped registers
        while ((REG_RD_DRF(_PGRAPH, _TRAPPED_ADDR, _MTHD) !=
                DRF_VAL(_PGRAPH, _TRAPPED_ADDR, _MTHD, NV1196_NO_OPERATION)) ||
                (REG_RD32(NV_PGRAPH_TRAPPED_DATA_LOW) != z16_data))
                ;
    }

    // Disable PGRAPH_FIFO_ACCESS again
    FLD_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);

    // Now, restore the stage2 method/data
    REG_WR32(NV_PGRAPH_FFINTFC_ST2,    st2);
    REG_WR32(NV_PGRAPH_FFINTFC_ST2_DL, datalo);
    REG_WR32(NV_PGRAPH_FFINTFC_ST2_DH, datahi);
}

//
// nvHalGrLoadChannelContext
//
RM_STATUS
nvHalGrLoadChannelContext_NV10(PHALHWINFO pHalHwInfo, U032 ChID)
{
    PGRHALINFO_NV10 pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;
    U032 misc;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrLoadChannelContext_NV10\r\n");

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
    if (pGrHalPvtInfo->currentChID != ChID) {
        nvHalGrUnloadChannelContext_NV10(pHalHwInfo,
                                         pGrHalPvtInfo->currentChID,
                                         &pGrHalPvtInfo->grChannels[pGrHalPvtInfo->currentChID]);
    }

    if ((pGrHalPvtInfo->currentChID = ChID) == INVALID_CHID)
    {
        //
        // Set current channel to an invalid one.
        //
        FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, (NUM_FIFOS_NV10 - 1));

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

        return (RM_OK);
        //
        // Is the channel ID valid?
        //
    }
    else if (ChID >= NUM_FIFOS_NV10)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid Channel on Graphics Context Switch:", ChID);
        DBG_BREAKPOINT();
        return (RM_ERROR);
    }

    // Reload the HW for the new channel
    nvHalGrLoadHW(pHalHwInfo,
                  ChID,
                  &pGrHalPvtInfo->grChannels[ChID]);

    // Update current channel.
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
    // state bits within one stage fifo.  Clear those bits on channel changes.
    //
    // Since the bits are write only, they'll read as a zero.
    // Clear those bits on channel changes. Don't rely on these to read as zero
    // eventhough these bits are write only.
    //
    // XXX adding the comments from NV4/NV5 ... actually, we hit this on NV10
    // also (our FIFOHOLD bits were set, so we'll also mask these off on a read)
    // in case this explains the fifo hang we saw.
    //
    // We have to OR in the top bit to fool the Watcom compiler for the Win3.1 version
    // of the resmgr.
    //
    // REG_WR32(NV_PGRAPH_FFINTFC_ST2, REG_RD32(NV_PGRAPH_FFINTFC_ST2) | 0x80000000);
    REG_WR32(NV_PGRAPH_FFINTFC_ST2, REG_RD32(NV_PGRAPH_FFINTFC_ST2) & 0xCFFFFFFF);

    return (RM_OK);
}

U032 reset_delay = 0x0;

VOID
nvHalGrLoadHW(PHALHWINFO pHalHwInfo, U032 ChID, PGRAPHICSCHANNEL_NV10 pGrChan)
{
    PGRHALINFO_NV10 pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;
    U032 i;

    //
    // Because unloading the previous channel may not have reset GR
    // state, after the unload we'll manually reset the engines. In
    // doing this, we'll also need to restore the GR tiling registers,
    // since they get lost on a reset.
    //
    REG_WR_DRF_DEF(_PGRAPH, _DEBUG_0, _STATE, _RESET);

    // delay for at least 10 nvclks
    if (reset_delay == 0) {
        reset_delay = (1000000000 / pHalHwInfo->pDacHalInfo->NVClk);   // nvclk cycle time, in ns
        reset_delay *= 10;
    }
    grDelay_NV10(pHalHwInfo, reset_delay);
    
    // jump back out of reset
    REG_WR_DRF_DEF(_PGRAPH, _DEBUG_0, _STATE, _NORMAL);

    HAL_GR_IDLE(pHalHwInfo);

    // restore PGRAPH_TILE regs after PGRAPH_DEBUG_0_STATE_RESET
    for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
    {
        REG_WR32(NV_PGRAPH_TLIMIT(i), REG_RD32(NV_PFB_TLIMIT(i)));
        REG_WR32(NV_PGRAPH_TSIZE(i),  REG_RD32(NV_PFB_TSIZE(i)));
        REG_WR32(NV_PGRAPH_TILE(i),   REG_RD32(NV_PFB_TILE(i)));
    }

    //
    // Apply context switch related workarounds after we've unloaded the
    // previous channels state, but before loading this new channels state.
    //
    if (pGrHalPvtInfo->currentObjects3d[ChID] != 0)
    {
        //
        // Eventhough we're about to reload GLOBALSTATE1 which holds the inst addr for
        // the vertex buffers context DMA, neither the channel switch nor the priv write
        // invalidates a shadowed copy kept in the hardware. The only thing that does
        // is a SetContextDmaVertex method, so we'll send that before the priv write.
        //
        if (DRF_VAL(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_2, pGrChan->CurrentPatch.GlobalState1))
        {
            grResubmitCelsiusMethod(pHalHwInfo, ChID, TRUE);
        }

        //
        // With NV11, on a priv write to PGRAPH_SURFACE, the Z16_OVERRIDE bit is taken
        // from the last user method not from the RBI bus. To work around this, we'll
        // send down a NOP with the correct data pattern for the bit (0x0 or 0x1), so
        // the correct thing happens on the priv write to PGRAPH_SURFACE.
        // 
        REG_WR32(NV_PGRAPH_SURFACE, pGrChan->CurrentPatch.Surface);        

        if (REG_RD32(NV_PGRAPH_SURFACE) != pGrChan->CurrentPatch.Surface)
        {
            grResubmitCelsiusMethod(pHalHwInfo, ChID, FALSE);

            // wait for the pipe to go idle 
            HAL_GR_IDLE(pHalHwInfo);

            // rewrite PGRAPH_SURFACE to the correct value (done again below).
            REG_WR32(NV_PGRAPH_SURFACE, pGrChan->CurrentPatch.Surface);        

#ifdef DEBUG
            // make sure we've got the expected NV_PGRAPH_SURFACE value
            if (DRF_VAL(_PGRAPH, _CTX_USER, _CHID, pGrChan->ContextUser) == ChID) {
                if (REG_RD32(NV_PGRAPH_SURFACE) != pGrChan->CurrentPatch.Surface) {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Incorrect Z16 override setting\r\n");
                    DBG_BREAKPOINT();
                }
            }
#endif
        }
    }

    //
    // Reload context state
    //
    REG_WR32(NV_PGRAPH_CTX_SWITCH1, pGrChan->ContextSwitch1);
    REG_WR32(NV_PGRAPH_CTX_SWITCH2, pGrChan->ContextSwitch2);
    REG_WR32(NV_PGRAPH_CTX_SWITCH3, pGrChan->ContextSwitch3);
    REG_WR32(NV_PGRAPH_CTX_SWITCH4, pGrChan->ContextSwitch4);
    REG_WR32(NV_PGRAPH_CTX_SWITCH5, pGrChan->ContextSwitch5);

    for (i = 0; i < 8; i++)
    {    
        REG_WR32(NV_PGRAPH_CTX_CACHE1(i), pGrChan->ContextCache1[i]);
        REG_WR32(NV_PGRAPH_CTX_CACHE2(i), pGrChan->ContextCache2[i]);
        REG_WR32(NV_PGRAPH_CTX_CACHE3(i), pGrChan->ContextCache3[i]);
        REG_WR32(NV_PGRAPH_CTX_CACHE4(i), pGrChan->ContextCache4[i]);
        REG_WR32(NV_PGRAPH_CTX_CACHE5(i), pGrChan->ContextCache5[i]);
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
    
    // In NV10, we can access the pattern RAM anytime after the engine's been idled
    for (i = 0; i < 64; i++)
        REG_WR32(NV_PGRAPH_PATT_COLORRAM(i), pGrChan->CurrentPatch.PatternColorRam[i]);
        
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
    REG_WR32(NV_PGRAPH_CONTROL3,        pGrChan->CurrentPatch.Control3);
    
    REG_WR32(NV_PGRAPH_BLEND,           pGrChan->CurrentPatch.Blend);
    REG_WR32(NV_PGRAPH_BLENDCOLOR,      pGrChan->CurrentPatch.BlendColor);
    REG_WR32(NV_PGRAPH_SETUPRASTER,     pGrChan->CurrentPatch.SetupRaster);
    REG_WR32(NV_PGRAPH_FOGCOLOR,        pGrChan->CurrentPatch.FogColor);
    REG_WR32(NV_PGRAPH_COLORKEYCOLOR0,  pGrChan->CurrentPatch.ColorKeyColor0);
    REG_WR32(NV_PGRAPH_COLORKEYCOLOR1,  pGrChan->CurrentPatch.ColorKeyColor1);
    REG_WR32(NV_PGRAPH_POINTSIZE,       pGrChan->CurrentPatch.PointSize);

    REG_WR32(NV_PGRAPH_ZOFFSETFACTOR,   pGrChan->CurrentPatch.ZOffsetFactor);
    REG_WR32(NV_PGRAPH_ZOFFSETBIAS,     pGrChan->CurrentPatch.ZOffsetBias);
    REG_WR32(NV_PGRAPH_ZCLIPMIN,        pGrChan->CurrentPatch.ZClipMin);
    REG_WR32(NV_PGRAPH_ZCLIPMAX,        pGrChan->CurrentPatch.ZClipMax);

    for (i = 0; i < 8; i++)
    {
        REG_WR32(NV_PGRAPH_WINDOWCLIP_HORIZONTAL(i), pGrChan->CurrentPatch.WinClipHorz[i]);
        REG_WR32(NV_PGRAPH_WINDOWCLIP_VERTICAL(i),   pGrChan->CurrentPatch.WinClipVert[i]);
    }

    REG_WR32(NV_PGRAPH_XFMODE0,         pGrChan->CurrentPatch.XFMode[0]);
    REG_WR32(NV_PGRAPH_XFMODE1,         pGrChan->CurrentPatch.XFMode[1]);

    REG_WR32(NV_PGRAPH_GLOBALSTATE0,         pGrChan->CurrentPatch.GlobalState0);
    REG_WR32(NV_PGRAPH_GLOBALSTATE1,         pGrChan->CurrentPatch.GlobalState1);

    REG_WR32(NV_PGRAPH_STORED_FMT,      pGrChan->CurrentPatch.StoredFmt);
    REG_WR32(NV_PGRAPH_SOURCE_COLOR,    pGrChan->CurrentPatch.SourceColor);
    
    for (i = 0; i < 32; i++)
    {
        REG_WR32(NV_PGRAPH_ABS_X_RAM(i), pGrChan->CurrentPatch.AbsXRam[i]);
        REG_WR32(NV_PGRAPH_ABS_Y_RAM(i), pGrChan->CurrentPatch.AbsYRam[i]);
    }        
    
    REG_WR32(NV_PGRAPH_ABS_UCLIP_XMIN,  pGrChan->CurrentPatch.AbsUClipXMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_XMAX,  pGrChan->CurrentPatch.AbsUClipXMax);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_YMIN,  pGrChan->CurrentPatch.AbsUClipYMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_YMAX,  pGrChan->CurrentPatch.AbsUClipYMax);

    REG_WR32(NV_PGRAPH_ABS_UCLIP3D_XMIN, pGrChan->CurrentPatch.AbsUClip3DXMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIP3D_XMAX, pGrChan->CurrentPatch.AbsUClip3DXMax);
    REG_WR32(NV_PGRAPH_ABS_UCLIP3D_YMIN, pGrChan->CurrentPatch.AbsUClip3DYMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIP3D_YMAX, pGrChan->CurrentPatch.AbsUClip3DYMax);

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
    
    REG_WR32(NV_PGRAPH_COMBINE0ALPHAICW,   pGrChan->CurrentPatch.Combine0AlphaICW);
    REG_WR32(NV_PGRAPH_COMBINE0COLORICW,   pGrChan->CurrentPatch.Combine0ColorICW);
    REG_WR32(NV_PGRAPH_COMBINE1ALPHAICW,   pGrChan->CurrentPatch.Combine1AlphaICW);
    REG_WR32(NV_PGRAPH_COMBINE1COLORICW,   pGrChan->CurrentPatch.Combine1ColorICW);
    
    REG_WR32(NV_PGRAPH_COMBINE0FACTOR,     pGrChan->CurrentPatch.Combine0Factor);
    REG_WR32(NV_PGRAPH_COMBINE1FACTOR,     pGrChan->CurrentPatch.Combine1Factor);

    REG_WR32(NV_PGRAPH_COMBINE0ALPHAOCW,   pGrChan->CurrentPatch.Combine0AlphaOCW);
    REG_WR32(NV_PGRAPH_COMBINE0COLOROCW,   pGrChan->CurrentPatch.Combine0ColorOCW);
    REG_WR32(NV_PGRAPH_COMBINE1ALPHAOCW,   pGrChan->CurrentPatch.Combine1AlphaOCW);
    REG_WR32(NV_PGRAPH_COMBINE1COLOROCW,   pGrChan->CurrentPatch.Combine1ColorOCW);

    REG_WR32(NV_PGRAPH_COMBINESPECFOGCW0,   pGrChan->CurrentPatch.CombineSpecFogCW0);
    REG_WR32(NV_PGRAPH_COMBINESPECFOGCW1,   pGrChan->CurrentPatch.CombineSpecFogCW1);

    REG_WR32(NV_PGRAPH_TEXTUREOFFSET0,      pGrChan->CurrentPatch.TextureOffset0);
    REG_WR32(NV_PGRAPH_TEXTUREOFFSET1,      pGrChan->CurrentPatch.TextureOffset1);
    REG_WR32(NV_PGRAPH_TEXTUREPALETTE0,     pGrChan->CurrentPatch.TexturePalette0);
    REG_WR32(NV_PGRAPH_TEXTUREPALETTE1,     pGrChan->CurrentPatch.TexturePalette1);

    REG_WR32(NV_PGRAPH_TEXFORMAT0,         pGrChan->CurrentPatch.TexFormat0);
    REG_WR32(NV_PGRAPH_TEXFORMAT1,         pGrChan->CurrentPatch.TexFormat1);

    REG_WR32(NV_PGRAPH_TEXCONTROL0_0,      pGrChan->CurrentPatch.TexControl0_0);
    REG_WR32(NV_PGRAPH_TEXCONTROL0_1,      pGrChan->CurrentPatch.TexControl0_1);
    REG_WR32(NV_PGRAPH_TEXCONTROL1_0,      pGrChan->CurrentPatch.TexControl1_0);
    REG_WR32(NV_PGRAPH_TEXCONTROL1_1,      pGrChan->CurrentPatch.TexControl1_1);
    REG_WR32(NV_PGRAPH_TEXCONTROL2_0,      pGrChan->CurrentPatch.TexControl2_0);
    REG_WR32(NV_PGRAPH_TEXCONTROL2_1,      pGrChan->CurrentPatch.TexControl2_1);

    REG_WR32(NV_PGRAPH_IMAGERECT0,      pGrChan->CurrentPatch.ImageRect0);
    REG_WR32(NV_PGRAPH_IMAGERECT1,      pGrChan->CurrentPatch.ImageRect1);

    REG_WR32(NV_PGRAPH_FILTER0,         pGrChan->CurrentPatch.Filter0);
    REG_WR32(NV_PGRAPH_FILTER1,         pGrChan->CurrentPatch.Filter1);
    
    REG_WR32(NV_PGRAPH_PASSTHRU_0,      pGrChan->CurrentPatch.Passthru0);    
    REG_WR32(NV_PGRAPH_PASSTHRU_1,      pGrChan->CurrentPatch.Passthru1);    
    REG_WR32(NV_PGRAPH_PASSTHRU_2,      pGrChan->CurrentPatch.Passthru2);    
    
    REG_WR32(NV_PGRAPH_DIMX_TEXTURE,    pGrChan->CurrentPatch.DimxTexture);
    REG_WR32(NV_PGRAPH_WDIMX_TEXTURE,   pGrChan->CurrentPatch.WdimxTexture);

    REG_WR32(NV_PGRAPH_DVD_COLORFMT,    pGrChan->CurrentPatch.DVDColorFormat);    
    REG_WR32(NV_PGRAPH_SCALED_FORMAT,   pGrChan->CurrentPatch.ScaledFormat);    
    
    REG_WR32(NV_PGRAPH_MISC24_0,        pGrChan->CurrentPatch.Misc24_0);
    REG_WR32(NV_PGRAPH_MISC24_1,        pGrChan->CurrentPatch.Misc24_1);
    REG_WR32(NV_PGRAPH_MISC24_2,        pGrChan->CurrentPatch.Misc24_2);
    REG_WR32(NV_PGRAPH_X_MISC,          pGrChan->CurrentPatch.XMisc);
    REG_WR32(NV_PGRAPH_Y_MISC,          pGrChan->CurrentPatch.YMisc);
    REG_WR32(NV_PGRAPH_VALID1,          pGrChan->CurrentPatch.Valid1);
    REG_WR32(NV_PGRAPH_VALID2,          pGrChan->CurrentPatch.Valid2);

    // wait for the pipe to go idle again after loading the pipeline context
    HAL_GR_IDLE(pHalHwInfo);

    // restore pipe context if a 3D object exists in the channel
    if (pGrHalPvtInfo->currentObjects3d[ChID] != 0)
    {
        U032 grclass = INST_RD32(pGrHalPvtInfo->currentObjects3d[ChID],
                                 SF_OFFSET(NV_PRAMIN_CONTEXT_0)) & DRF_MASK(NV_PGRAPH_CTX_SWITCH1_GRCLASS);

        // set vertex passthrough mode (pass data unchanged down the pipe)
        REG_WR32(NV_PGRAPH_XFMODE0,         Vertex_Passthr_Mode[0]);
        REG_WR32(NV_PGRAPH_XFMODE1,         Vertex_Passthr_Mode[1]);

        // set scale and offset constants in ModelViewMatrix1 for passthrough
        grLoadPipeContext(pHalHwInfo, PIPE_CHEOPS_BASE + (3*64), ScaleOffset_Passthr_Mode, SCALEOFFSET_COUNT);

        // set fog constants for passthrough (different between A01 and A02)
        if (IsNV10MaskRevA02orBetter_NV10(pHalHwInfo->pMcHalInfo))
        {
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x2b << 4), Fog_Passthr_Mode[1], FOG_COUNT);
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x28 << 4), Fog_EyeDist_Passthr_Mode, FOG_EYEDIST_COUNT);
        } else
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x2b << 4), Fog_Passthr_Mode[0], FOG_COUNT);


        if ((grclass == NV10_DX5_TEXTURED_TRIANGLE) || (grclass == NV10_DX6_MULTI_TEXTURE_TRIANGLE))
        {
            // set PrimType=NONE (0x0), which puts the following vertices into the vertex file
            i = 0x0;
            grLoadPipeContext(pHalHwInfo, PIPE_PRIM_BASE, &i, 0x1);
            grLoadPipeContext(pHalHwInfo, PIPE_FILE_BASE, pGrChan->CurrentPatch.CurrentPipe.VertexFile, VERTEX_FILE_COUNT);
        }
        else
        {
            // set PrimType=QUAD (0x8), which puts the following vertices into primitive assembly
            i = 0x8;
            grLoadPipeContext(pHalHwInfo, PIPE_PRIM_BASE, &i, 0x1);
            grLoadPipeContext(pHalHwInfo, PIPE_ASSM_BASE, pGrChan->CurrentPatch.CurrentPipe.PrimAssm, PRIM_ASSM_COUNT);
        }

        REG_WR32(NV_PGRAPH_XFMODE0,         pGrChan->CurrentPatch.XFMode[0]);
        REG_WR32(NV_PGRAPH_XFMODE1,         pGrChan->CurrentPatch.XFMode[1]);

        // load transform engine state
        grLoadPipeContext(pHalHwInfo, PIPE_CHEOPS_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_CHEOPS, XF_CHEOPS_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER, XF_ZOSER_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_C0_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C0, XF_ZOSER_C0_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_C1_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C1, XF_ZOSER_C1_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_C2_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C2, XF_ZOSER_C2_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_C3_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C3, XF_ZOSER_C3_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_VAB_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_VAB, XF_VAB_COUNT);

        if (grclass == NV10_CELSIUS_PRIMITIVE)
        {
            U032 format, temp;

            //
            // The 8 FORMAT_SIZE values (bits 7:4) don't get saved correctly during an unload due
            // to a HW bug. To workaround this, the drivers shadow the values in the last DWORD of
            // the last row of the 2nd Inverse ModelViewMatrix (offset = 0x653C).
            // 
            // During a load of this ChID, we'll read the DWORD out of the matrix, unpack the bits
            // and put the correct values in grTable before they're restored into FORMAT_BASE.
            //
            format = pGrChan->CurrentPatch.CurrentPipe.XF_CHEOPS[(INV2_MATRIX_OFFSET - PIPE_CHEOPS_BASE)/4];
        
            for (i = 1; i < INDEX_DMA_FORMAT_COUNT; i += 2, format >>= 4)
            {
                temp = pGrChan->CurrentPatch.CurrentPipe.IndexDmaFormat[i];
                temp = (temp & 0xFFFFFF0F) | ((format & 0xF) << 4);
                pGrChan->CurrentPatch.CurrentPipe.IndexDmaFormat[i] = temp;
            }
        }

        // load index DMA pipe state
        grLoadPipeContext(pHalHwInfo, PIPE_FORMAT_BASE,	pGrChan->CurrentPatch.CurrentPipe.IndexDmaFormat, INDEX_DMA_FORMAT_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_PRIM_BASE, pGrChan->CurrentPatch.CurrentPipe.IndexDmaPrim, INDEX_DMA_PRIM_COUNT);

        // wait for the pipe to go idle 
        HAL_GR_IDLE(pHalHwInfo);

#ifdef NOTDEF
        //
        // Force an invalidate of the vertex cache. Since we force a
        // GR reset of the chip, this shouldn't be necessary anymore.
        //
        i = 0x0;
        grLoadPipeContext(pHalHwInfo, PIPE_PRIM_BASE+0x80, &i, 0x1);
#endif
    }
}

RM_STATUS
nvHalGrUnloadChannelContext_NV10(PHALHWINFO pHalHwInfo, U032 ChID, PGRAPHICSCHANNEL_NV10 pGrChan)
{
    PGRHALINFO_NV10 pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrUnloadChannelContext_NV10\r\n");

    //
    // No need to unload an invalid channel
    //
    if (ChID  == INVALID_CHID)
        return (RM_OK);

    //
    // Unload current context registers.
    //
    pGrChan->ContextSwitch1    = REG_RD32(NV_PGRAPH_CTX_SWITCH1);
    pGrChan->ContextSwitch2    = REG_RD32(NV_PGRAPH_CTX_SWITCH2);
    pGrChan->ContextSwitch3    = REG_RD32(NV_PGRAPH_CTX_SWITCH3);        
    pGrChan->ContextSwitch4    = REG_RD32(NV_PGRAPH_CTX_SWITCH4);
    pGrChan->ContextSwitch5    = REG_RD32(NV_PGRAPH_CTX_SWITCH5);

    for (i = 0; i < 8; i++)
    {    
        pGrChan->ContextCache1[i] = REG_RD32(NV_PGRAPH_CTX_CACHE1(i));
        pGrChan->ContextCache2[i] = REG_RD32(NV_PGRAPH_CTX_CACHE2(i));
        pGrChan->ContextCache3[i] = REG_RD32(NV_PGRAPH_CTX_CACHE3(i));
        pGrChan->ContextCache4[i] = REG_RD32(NV_PGRAPH_CTX_CACHE4(i));
        pGrChan->ContextCache5[i] = REG_RD32(NV_PGRAPH_CTX_CACHE5(i));
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
    
    // In NV10, we can access the pattern RAM anytime after the engine's been idled
    for (i = 0; i < 64; i++)
        pGrChan->CurrentPatch.PatternColorRam[i] = REG_RD32(NV_PGRAPH_PATT_COLORRAM(i));
    
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
    pGrChan->CurrentPatch.Control3            = REG_RD32(NV_PGRAPH_CONTROL3);
    
    pGrChan->CurrentPatch.Blend               = REG_RD32(NV_PGRAPH_BLEND);
    pGrChan->CurrentPatch.BlendColor          = REG_RD32(NV_PGRAPH_BLENDCOLOR);
    pGrChan->CurrentPatch.SetupRaster         = REG_RD32(NV_PGRAPH_SETUPRASTER);
    pGrChan->CurrentPatch.FogColor            = REG_RD32(NV_PGRAPH_FOGCOLOR);
    pGrChan->CurrentPatch.ColorKeyColor0      = REG_RD32(NV_PGRAPH_COLORKEYCOLOR0);
    pGrChan->CurrentPatch.ColorKeyColor1      = REG_RD32(NV_PGRAPH_COLORKEYCOLOR1);
    pGrChan->CurrentPatch.PointSize           = REG_RD32(NV_PGRAPH_POINTSIZE);

    pGrChan->CurrentPatch.ZOffsetFactor       = REG_RD32(NV_PGRAPH_ZOFFSETFACTOR);
    pGrChan->CurrentPatch.ZOffsetBias         = REG_RD32(NV_PGRAPH_ZOFFSETBIAS);
    pGrChan->CurrentPatch.ZClipMin            = REG_RD32(NV_PGRAPH_ZCLIPMIN);
    pGrChan->CurrentPatch.ZClipMax            = REG_RD32(NV_PGRAPH_ZCLIPMAX);
    
    for (i = 0; i < 8; i++)
    {
        pGrChan->CurrentPatch.WinClipHorz[i]    = REG_RD32(NV_PGRAPH_WINDOWCLIP_HORIZONTAL(i));
        pGrChan->CurrentPatch.WinClipVert[i]    = REG_RD32(NV_PGRAPH_WINDOWCLIP_VERTICAL(i));
    }

    pGrChan->CurrentPatch.XFMode[0]           = REG_RD32(NV_PGRAPH_XFMODE0);
    pGrChan->CurrentPatch.XFMode[1]           = REG_RD32(NV_PGRAPH_XFMODE1);

    pGrChan->CurrentPatch.GlobalState0        = REG_RD32(NV_PGRAPH_GLOBALSTATE0);
    pGrChan->CurrentPatch.GlobalState1        = REG_RD32(NV_PGRAPH_GLOBALSTATE1);

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
    
    pGrChan->CurrentPatch.AbsUClip3DXMin      = REG_RD32(NV_PGRAPH_ABS_UCLIP3D_XMIN);
    pGrChan->CurrentPatch.AbsUClip3DXMax      = REG_RD32(NV_PGRAPH_ABS_UCLIP3D_XMAX);
    pGrChan->CurrentPatch.AbsUClip3DYMin      = REG_RD32(NV_PGRAPH_ABS_UCLIP3D_YMIN);
    pGrChan->CurrentPatch.AbsUClip3DYMax      = REG_RD32(NV_PGRAPH_ABS_UCLIP3D_YMAX);
    
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
    
    pGrChan->CurrentPatch.Combine0AlphaICW    = REG_RD32(NV_PGRAPH_COMBINE0ALPHAICW);
    pGrChan->CurrentPatch.Combine1AlphaICW    = REG_RD32(NV_PGRAPH_COMBINE1ALPHAICW);
    pGrChan->CurrentPatch.Combine0ColorICW    = REG_RD32(NV_PGRAPH_COMBINE0COLORICW);
    pGrChan->CurrentPatch.Combine1ColorICW    = REG_RD32(NV_PGRAPH_COMBINE1COLORICW);

    pGrChan->CurrentPatch.Combine0Factor      = REG_RD32(NV_PGRAPH_COMBINE0FACTOR);
    pGrChan->CurrentPatch.Combine1Factor      = REG_RD32(NV_PGRAPH_COMBINE1FACTOR);

    pGrChan->CurrentPatch.Combine0AlphaOCW    = REG_RD32(NV_PGRAPH_COMBINE0ALPHAOCW);
    pGrChan->CurrentPatch.Combine1AlphaOCW    = REG_RD32(NV_PGRAPH_COMBINE1ALPHAOCW);
    pGrChan->CurrentPatch.Combine0ColorOCW    = REG_RD32(NV_PGRAPH_COMBINE0COLOROCW);
    pGrChan->CurrentPatch.Combine1ColorOCW    = REG_RD32(NV_PGRAPH_COMBINE1COLOROCW);

    pGrChan->CurrentPatch.CombineSpecFogCW0   = REG_RD32(NV_PGRAPH_COMBINESPECFOGCW0);
    pGrChan->CurrentPatch.CombineSpecFogCW1   = REG_RD32(NV_PGRAPH_COMBINESPECFOGCW1);

    pGrChan->CurrentPatch.TextureOffset0      = REG_RD32(NV_PGRAPH_TEXTUREOFFSET0);
    pGrChan->CurrentPatch.TextureOffset1      = REG_RD32(NV_PGRAPH_TEXTUREOFFSET1);
    pGrChan->CurrentPatch.TexturePalette0     = REG_RD32(NV_PGRAPH_TEXTUREPALETTE0);
    pGrChan->CurrentPatch.TexturePalette1     = REG_RD32(NV_PGRAPH_TEXTUREPALETTE1);
    pGrChan->CurrentPatch.TexFormat0          = REG_RD32(NV_PGRAPH_TEXFORMAT0);
    pGrChan->CurrentPatch.TexFormat1          = REG_RD32(NV_PGRAPH_TEXFORMAT1);
    pGrChan->CurrentPatch.TexControl0_0       = REG_RD32(NV_PGRAPH_TEXCONTROL0_0);
    pGrChan->CurrentPatch.TexControl0_1       = REG_RD32(NV_PGRAPH_TEXCONTROL0_1);
    pGrChan->CurrentPatch.TexControl1_0       = REG_RD32(NV_PGRAPH_TEXCONTROL1_0);
    pGrChan->CurrentPatch.TexControl1_1       = REG_RD32(NV_PGRAPH_TEXCONTROL1_1);
    pGrChan->CurrentPatch.TexControl2_0       = REG_RD32(NV_PGRAPH_TEXCONTROL2_0);
    pGrChan->CurrentPatch.TexControl2_1       = REG_RD32(NV_PGRAPH_TEXCONTROL2_1);

    pGrChan->CurrentPatch.ImageRect0          = REG_RD32(NV_PGRAPH_IMAGERECT0);
    pGrChan->CurrentPatch.ImageRect1          = REG_RD32(NV_PGRAPH_IMAGERECT1);

    pGrChan->CurrentPatch.Filter0             = REG_RD32(NV_PGRAPH_FILTER0);
    pGrChan->CurrentPatch.Filter1             = REG_RD32(NV_PGRAPH_FILTER1);
    
    pGrChan->CurrentPatch.Passthru0           = REG_RD32(NV_PGRAPH_PASSTHRU_0);    
    pGrChan->CurrentPatch.Passthru1           = REG_RD32(NV_PGRAPH_PASSTHRU_1);    
    pGrChan->CurrentPatch.Passthru2           = REG_RD32(NV_PGRAPH_PASSTHRU_2);    
    
    pGrChan->CurrentPatch.DimxTexture         = REG_RD32(NV_PGRAPH_DIMX_TEXTURE);
    pGrChan->CurrentPatch.WdimxTexture        = REG_RD32(NV_PGRAPH_WDIMX_TEXTURE);

    pGrChan->CurrentPatch.DVDColorFormat      = REG_RD32(NV_PGRAPH_DVD_COLORFMT);    
    pGrChan->CurrentPatch.ScaledFormat        = REG_RD32(NV_PGRAPH_SCALED_FORMAT);    
    
    pGrChan->CurrentPatch.Misc24_0            = REG_RD32(NV_PGRAPH_MISC24_0);
    pGrChan->CurrentPatch.Misc24_1            = REG_RD32(NV_PGRAPH_MISC24_1);
    pGrChan->CurrentPatch.Misc24_2            = REG_RD32(NV_PGRAPH_MISC24_2);
    pGrChan->CurrentPatch.XMisc               = REG_RD32(NV_PGRAPH_X_MISC);
    pGrChan->CurrentPatch.YMisc               = REG_RD32(NV_PGRAPH_Y_MISC);
    pGrChan->CurrentPatch.Valid1              = REG_RD32(NV_PGRAPH_VALID1);
    pGrChan->CurrentPatch.Valid2              = REG_RD32(NV_PGRAPH_VALID2);

    // save pipe context if a 3D object exists in the channel
    if (pGrHalPvtInfo->currentObjects3d[ChID] != 0)
    {
        U032 grclass = INST_RD32(pGrHalPvtInfo->currentObjects3d[ChID],
                                 SF_OFFSET(NV_PRAMIN_CONTEXT_0)) & DRF_MASK(NV_PGRAPH_CTX_SWITCH1_GRCLASS);

        if ((grclass == NV10_DX5_TEXTURED_TRIANGLE) || (grclass == NV10_DX6_MULTI_TEXTURE_TRIANGLE))
        {
            grUnloadPipeContext(pHalHwInfo, PIPE_FILE_BASE, pGrChan->CurrentPatch.CurrentPipe.VertexFile, VERTEX_FILE_COUNT);
            grUnloadPipeContext(pHalHwInfo, PIPE_VAB_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_VAB, XF_VAB_COUNT);
        }
        else
        {
            grUnloadPipeContext(pHalHwInfo, PIPE_VAB_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_VAB, XF_VAB_COUNT);
            grUnloadPipeContext(pHalHwInfo, PIPE_ASSM_BASE,	pGrChan->CurrentPatch.CurrentPipe.PrimAssm, PRIM_ASSM_COUNT);
        }

        // unload transform engine state
        grUnloadPipeContext(pHalHwInfo, PIPE_CHEOPS_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_CHEOPS, XF_CHEOPS_COUNT);
        grUnloadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER, XF_ZOSER_COUNT);
        grUnloadPipeContext(pHalHwInfo, PIPE_ZOSER_C0_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C0, XF_ZOSER_C0_COUNT);
        grUnloadPipeContext(pHalHwInfo, PIPE_ZOSER_C1_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C1, XF_ZOSER_C1_COUNT);
        grUnloadPipeContext(pHalHwInfo, PIPE_ZOSER_C2_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C2, XF_ZOSER_C2_COUNT);
        grUnloadPipeContext(pHalHwInfo, PIPE_ZOSER_C3_BASE, pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C3, XF_ZOSER_C3_COUNT);

        // unload index DMA pipe state
        grUnloadPipeContext(pHalHwInfo, PIPE_PRIM_BASE, pGrChan->CurrentPatch.CurrentPipe.IndexDmaPrim, INDEX_DMA_PRIM_COUNT);

#ifdef CHECK_VTX_COUNT
{
        U032 InitCtxtPtr = pHalHwInfo->pPramHalInfo->FifoContextAddr + (ChID * 32);
        U032 vtxcount = pGrChan->CurrentPatch.CurrentPipe.IndexDmaPrim[0x3] & 0x1F00;

        if (vtxcount) {
            U032 method = REG_RD32((U032)(InitCtxtPtr + SF_OFFSET(NV_RAMFC_DMA_METHOD))) & 0x1FFF;
            if ((method < NV056_INLINE_ARRAY(0)) || (method >= NV056_INLINE_ARRAY(0x200)))
                _asm int 3;
        }
}
#endif
        grUnloadPipeContext(pHalHwInfo, PIPE_FORMAT_BASE, pGrChan->CurrentPatch.CurrentPipe.IndexDmaFormat, INDEX_DMA_FORMAT_COUNT);

        // wait for the pipe to go idle again after unloading the pipeline context
        HAL_GR_IDLE(pHalHwInfo);
    }

    //
    // Invalidate hw's channel ID.
    //
    REG_WR32(NV_PGRAPH_CTX_CONTROL, DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _EXPIRED)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));

    FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, (NUM_FIFOS_NV10 - 1));

    return (RM_OK);
}

RM_STATUS
nvHalGrGetState_NV10(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData, U032 useCtxDma)
{
    PGRHALINFO_NV10 pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV10 pGrChannel;
    U032 ChID;
    U032 access, instance;
    U032 i, format, temp;
    NvGraphicsState *stateptr;
    RM_STATUS status;
    PDMAHALOBJINFO pDmaHalInfo;
    PHWREG nvAddr;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrGetState\r\n");

    pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    pGrChannel = &pGrHalPvtInfo->grChannels[pGrExceptionData->ChID];
    nvAddr = pHalHwInfo->nvBaseAddr;
    ChID = pGrExceptionData->ChID;

#ifdef DEBUG
    // we're from a GR exception, so we should be idled/disable already
    if (REG_RD_DRF(_PGRAPH, _FIFO, _ACCESS) || REG_RD32(NV_PGRAPH_STATUS))
        DBG_BREAKPOINT();
#endif

    access = REG_RD32(NV_PGRAPH_FIFO);
	REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);
    GR_DONE();

    if (useCtxDma)
    {
        // determine where the ContextDmaState buffer resides (kept in GLOBALSTATE1)
        if (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) == ChID)
            instance = REG_RD_DRF(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_3);
        else
            instance = DRF_VAL(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_3, pGrChannel[ChID].CurrentPatch.GlobalState1);

        if (!instance)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NULL ContextDmaState buffer\n\r");
            DBG_BREAKPOINT();
            return RM_ERR_INVALID_STATE;
        }

        // convert instance address to DmaHalInfo structure
        status = dmaHalInstanceToHalInfo_NV10(ChID, instance, &pDmaHalInfo, pHalHwInfo);
        if (status)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ContextDmaState invalid instance\n\r");
            DBG_BREAKPOINT();
            return RM_ERR_INVALID_STATE;
        }

        if (pDmaHalInfo->BufferSize < sizeof (NvGraphicsState))
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ContextDmaState smaller than GetState data\n\r");
            DBG_BREAKPOINT();
            return status;
        }

        stateptr = (NvGraphicsState *)pDmaHalInfo->BufferBase;
    }
    else
    {
        // use the local NvGraphicsState for the Snapshot/Reload interface
        stateptr = &pGrHalPvtInfo->grSnapShots[ChID];
    }

    // copy out the data
    if (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) == ChID)
    {
        // first 4 DWORDS of NvGraphicsState is the PerFragment state
        ((U032 *)stateptr)[0] = REG_RD32(NV_PGRAPH_CONTROL0);
        ((U032 *)stateptr)[1] = REG_RD32(NV_PGRAPH_CONTROL1);
        ((U032 *)stateptr)[2] = REG_RD32(NV_PGRAPH_CONTROL2);
        ((U032 *)stateptr)[3] = REG_RD32(NV_PGRAPH_BLEND);

        // read the
        //      PerVertexState from the VAB,
        //      PrimitiveAssembly from PIPE_ASSM_BASE
        //      PerVertextState.EdgeFlag from PRIM_BASE
        //      VertexArrayState from PIPE_FORMAT
        grUnloadPipeContext(pHalHwInfo, PIPE_VAB_BASE,    (U032 *)(&stateptr->PerVertexState), XF_VAB_COUNT);
        grUnloadPipeContext(pHalHwInfo, PIPE_ASSM_BASE,   (U032 *)(&stateptr->PrimitiveAssmState), PRIM_ASSM_COUNT);
        grUnloadPipeContext(pHalHwInfo, PIPE_PRIM_BASE,   (U032 *)(&stateptr->PerVertexState.pad3), INDEX_DMA_PRIM_COUNT);
        grUnloadPipeContext(pHalHwInfo, PIPE_FORMAT_BASE, (U032 *)(&stateptr->VertexArrayState), INDEX_DMA_FORMAT_COUNT);
    } else {
        // first 4 DWORDS of NvGraphicsState is the PerFragment state
        ((U032 *)stateptr)[0] = pGrChannel->CurrentPatch.Control0;
        ((U032 *)stateptr)[1] = pGrChannel->CurrentPatch.Control1;
        ((U032 *)stateptr)[2] = pGrChannel->CurrentPatch.Control2;
        ((U032 *)stateptr)[3] = pGrChannel->CurrentPatch.Blend;
            
        // read the
        //      PerVertexState from the VAB,
        //      PrimitiveAssembly from PIPE_ASSM_BASE
        //      PerVertextState.EdgeFlag from PRIM_BASE
        //      VertexArrayState from PIPE_FORMAT
        for (i = 0; i < (XF_VAB_COUNT << 2); i++)
            *(((char *)(&stateptr->PerVertexState))+i) = *(((char *)pGrChannel->CurrentPatch.CurrentPipe.XF_VAB)+i);

        for (i = 0; i < (PRIM_ASSM_COUNT << 2); i++)
            *(((char *)(&stateptr->PrimitiveAssmState))+i) = *(((char *)pGrChannel->CurrentPatch.CurrentPipe.PrimAssm)+i);

        for (i = 0; i < (INDEX_DMA_PRIM_COUNT << 2); i++)
            *(((char *)(&stateptr->PerVertexState.pad3))+i) = *(((char *)pGrChannel->CurrentPatch.CurrentPipe.IndexDmaPrim)+i);

        for (i = 0; i < (INDEX_DMA_FORMAT_COUNT << 2); i++)
            *(((char *)(&stateptr->VertexArrayState))+i) = *(((char *)pGrChannel->CurrentPatch.CurrentPipe.IndexDmaFormat)+i);
    }

    // correct FORMAT_SIZE bits 7:4 for the HW context read bug (driver shadowed in INV2_MATRIX_OFFSET)
    grUnloadPipeContext(pHalHwInfo, INV2_MATRIX_OFFSET, &format, 0x1);

    for (i = 1; i < INDEX_DMA_FORMAT_COUNT; i += 2, format >>= 4)
    {
        temp = ((U032 *) &stateptr->VertexArrayState)[i];
        temp = (temp & 0xFFFFFF0F) | ((format & 0xF) << 4);
        ((U032 *) &stateptr->VertexArrayState)[i] = temp;
    }

    GR_DONE();
    REG_WR32(NV_PGRAPH_FIFO, access);

    return RM_OK;
}

RM_STATUS
nvHalGrPutState_NV10(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData, U032 useCtxDma)
{
    PGRHALINFO_NV10 pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV10 pGrChannel;
    U032 access, instance;
    U032 i;
    NvGraphicsState *stateptr;
    RM_STATUS   status;
    PDMAHALOBJINFO pDmaHalInfo;
    PHWREG nvAddr;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrPutState\r\n");

    pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    pGrChannel = &pGrHalPvtInfo->grChannels[pGrExceptionData->ChID];
    nvAddr = pHalHwInfo->nvBaseAddr;

    access = REG_RD32(NV_PGRAPH_FIFO);
    REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);
    GR_DONE();

    if (useCtxDma) {
        // determine where the ContextDmaState buffer resides (kept in GLOBALSTATE1)
        if (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) == pGrExceptionData->ChID)
            instance = REG_RD_DRF(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_3);
        else
            instance = DRF_VAL(_PGRAPH, _GLOBALSTATE1, _DMA_INSTANCE_3, pGrChannel[pGrExceptionData->ChID].CurrentPatch.GlobalState1);

        if (!instance)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NULL ContextDmaState buffer\n\r");
            DBG_BREAKPOINT();
            return RM_ERROR;
        }

        // convert instance address to DmaHalInfo structure
        status = dmaHalInstanceToHalInfo_NV10(pGrExceptionData->ChID, instance, &pDmaHalInfo, pHalHwInfo);
        if (status)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Invalid ContextDmaState buffer\n\r");
            DBG_BREAKPOINT();
            return RM_ERR_INVALID_STATE;
        }

        // ensure DmaState buffer is large enough for the GetState data
        if (pDmaHalInfo->BufferSize < sizeof(NvGraphicsState))
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ContextDmaState smaller than GetState data\n\r");
            DBG_BREAKPOINT();
            return status;
        }

        stateptr = (NvGraphicsState *)pDmaHalInfo->BufferBase;
    } else {
        // use the local NvGraphicsState for the Snapshot/Reload interface
        stateptr = &pGrHalPvtInfo->grSnapShots[pGrExceptionData->ChID];
    }

    // copy data to the HW
    if (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) == pGrExceptionData->ChID)
    {
        U032 xfmode0, xfmode1;
        U032 scale[SCALEOFFSET_COUNT];
        U032 fog[FOG_COUNT];
        U032 fog_eyedist[FOG_EYEDIST_COUNT];

        // first 4 DWORDS of NvGraphicsState is the PerFragment state
        REG_WR32(NV_PGRAPH_CONTROL0, ((U032 *)stateptr)[0]);
        REG_WR32(NV_PGRAPH_CONTROL1, ((U032 *)stateptr)[1]);
        REG_WR32(NV_PGRAPH_CONTROL2, ((U032 *)stateptr)[2]);
        REG_WR32(NV_PGRAPH_BLEND,    ((U032 *)stateptr)[3]);

        // Because we'll need to change bits of the pipe state to put the engines
        // in passthrough mode, we'll need to keep the current values, so we can
        // restore them after we've reloaded PRIM_ASSM.

        // save current xfmode values (modified for passthrough)
        xfmode0 = REG_RD32(NV_PGRAPH_XFMODE0);
        xfmode1 = REG_RD32(NV_PGRAPH_XFMODE1);

        // save current scale/offset constants in ModelViewMatrix1 (modified for passthrough)
        grUnloadPipeContext(pHalHwInfo, PIPE_CHEOPS_BASE + (3*64), scale, SCALEOFFSET_COUNT);

        // save current fog constants (modified for passthrough)
        if (IsNV10MaskRevA02orBetter_NV10(pHalHwInfo->pMcHalInfo))
        {
            grUnloadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x2b << 4), fog, FOG_COUNT);
            grUnloadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x28 << 4), fog_eyedist, FOG_EYEDIST_COUNT);
        } else
            grUnloadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x2b << 4), fog, FOG_COUNT);

        // Now, change the pipe state so that the engines are in passthrough mode.

        // set vertex passthrough mode (pass data unchanged down the pipe)
        REG_WR32(NV_PGRAPH_XFMODE0,         Vertex_Passthr_Mode[0]);
        REG_WR32(NV_PGRAPH_XFMODE1,         Vertex_Passthr_Mode[1]);

        // set scale and offset constants in ModelViewMatrix1 for passthrough
        grLoadPipeContext(pHalHwInfo, PIPE_CHEOPS_BASE + (3*64), ScaleOffset_Passthr_Mode, SCALEOFFSET_COUNT);

        // set fog constants for passthrough (different between A01 and A02)
        if (IsNV10MaskRevA02orBetter_NV10(pHalHwInfo->pMcHalInfo))
        {
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x2b << 4), Fog_Passthr_Mode[1], FOG_COUNT);
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x28 << 4), Fog_EyeDist_Passthr_Mode, FOG_EYEDIST_COUNT);
        } else
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x2b << 4), Fog_Passthr_Mode[0], FOG_COUNT);

        // set PrimType=QUAD (0x8), which puts the following vertices into primitive assembly
        i = 0x8;
        grLoadPipeContext(pHalHwInfo, PIPE_PRIM_BASE, &i, 0x1);

        // restore the PrimitiveAssembly
        grLoadPipeContext(pHalHwInfo, PIPE_ASSM_BASE,   (U032 *)(&stateptr->PrimitiveAssmState), PRIM_ASSM_COUNT);

        // After primitive assembly is restored, reload the original values we
        // modified to put the engines in passthrough mode.

        // restore current xfmode (modified for passthrough)
        REG_WR32(NV_PGRAPH_XFMODE0, xfmode0);
        REG_WR32(NV_PGRAPH_XFMODE1, xfmode1);

        // restore current scale/offset constants in ModelViewMatrix1 (modified for passthrough)
        grLoadPipeContext(pHalHwInfo, PIPE_CHEOPS_BASE + (3*64), scale, SCALEOFFSET_COUNT);
        
        // restore current fog constants (modified for passthrough)
        if (IsNV10MaskRevA02orBetter_NV10(pHalHwInfo->pMcHalInfo))
        {
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x2b << 4), fog, FOG_COUNT);
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x28 << 4), fog_eyedist, FOG_EYEDIST_COUNT);
        } else
            grLoadPipeContext(pHalHwInfo, PIPE_ZOSER_BASE | (0x2b << 4), fog, FOG_COUNT);

        // restore the
        //      PerVertexState to the VAB,
        //      VertexArrayState to PIPE_FORMAT
        //      PerVertextState.EdgeFlag to PRIM_BASE
        grLoadPipeContext(pHalHwInfo, PIPE_VAB_BASE,    (U032 *)(&stateptr->PerVertexState), XF_VAB_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_FORMAT_BASE, (U032 *)(&stateptr->VertexArrayState), INDEX_DMA_FORMAT_COUNT);
        grLoadPipeContext(pHalHwInfo, PIPE_PRIM_BASE,   (U032 *)(&stateptr->PerVertexState.pad3), INDEX_DMA_PRIM_COUNT);

		// force an invalidate of the vertex cache 
        i = 0x0;
        grLoadPipeContext(pHalHwInfo, PIPE_PRIM_BASE+0x80, &i, 0x1);
    } else {
        // first 4 DWORDS of NvGraphicsState is the PerFragment state
        pGrChannel->CurrentPatch.Control0 = ((U032 *)stateptr)[0];
        pGrChannel->CurrentPatch.Control1 = ((U032 *)stateptr)[1];
        pGrChannel->CurrentPatch.Control2 = ((U032 *)stateptr)[2];
        pGrChannel->CurrentPatch.Blend    = ((U032 *)stateptr)[3];

        // restore the
        //      PerVertexState to the VAB,
        //      PrimitiveAssembly to PIPE_ASSM_BASE
        //      PerVertextState.EdgeFlag to PRIM_BASE
        //      VertexArrayState to PIPE_FORMAT
        for (i = 0; i < (XF_VAB_COUNT << 2); i++)
            *(((char *)pGrChannel->CurrentPatch.CurrentPipe.XF_VAB)+i) = *(((char *)(&stateptr->PerVertexState))+i);

        for (i = 0; i < (PRIM_ASSM_COUNT << 2); i++)
            *(((char *)pGrChannel->CurrentPatch.CurrentPipe.PrimAssm)+i) = *(((char *)(&stateptr->PrimitiveAssmState))+i);

        for (i = 0; i < (INDEX_DMA_PRIM_COUNT << 2); i++)
            *(((char *)pGrChannel->CurrentPatch.CurrentPipe.IndexDmaPrim)+i) = *(((char *)(&stateptr->PerVertexState.pad3))+i);

        for (i = 0; i < (INDEX_DMA_FORMAT_COUNT << 2); i++)
            *(((char *)pGrChannel->CurrentPatch.CurrentPipe.IndexDmaFormat)+i) = *(((char *)(&stateptr->VertexArrayState))+i);
    }

    GR_DONE();
    REG_WR32(NV_PGRAPH_FIFO, access);

    return RM_OK;
}

#define ZOSER_MATERIAL_COLOR_BASE   0x6aa0

//
// Workaround a HW problem where a Celsius SetMaterialEmission will be ignored in
// between a Begin/End. This is a SW method OGL calls to have us plugin the value.
// 
RM_STATUS nvHalGrSetMaterialEmission_NV10(
    PHALHWINFO pHalHwInfo,
    PGREXCEPTIONDATA pGrExceptionData
)
{
    PHWREG nvAddr;
    U032    pipeAddr = ZOSER_MATERIAL_COLOR_BASE + (pGrExceptionData->Offset - NV056_SET_MATERIAL_EMISSION_SW(0));

    nvAddr = pHalHwInfo->nvBaseAddr;
    grLoadPipeContext(pHalHwInfo, pipeAddr, &pGrExceptionData->Data, 0x1);
    GR_DONE();

    return (RM_OK);
}

//
// This method gives D3D access to the eye direction state.
//

#define ZOSER_EYE_DIR_BASE      0x6a80

RM_STATUS nvHalGrSetEyeDirection_NV10(
    PHALHWINFO pHalHwInfo,
    PGREXCEPTIONDATA pGrExceptionData
)
{
    PHWREG nvAddr;
    U032    pipeAddr = ZOSER_EYE_DIR_BASE + (pGrExceptionData->Offset - NV056_SET_EYE_DIRECTION_SW(0));

    nvAddr = pHalHwInfo->nvBaseAddr;
    grLoadPipeContext(pHalHwInfo, pipeAddr, &pGrExceptionData->Data, 0x1);
    GR_DONE();

    return (RM_OK);
}

static RM_STATUS grDelay_NV10(
    PHALHWINFO pHalHwInfo,
    U032 nsec
)
{
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;
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
