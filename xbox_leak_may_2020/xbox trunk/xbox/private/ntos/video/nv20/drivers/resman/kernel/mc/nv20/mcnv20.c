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

/******************************* MC Manager ********************************\
*                                                                           *
* Module: MCNV20.C                                                          *
*   The master control chip dependent HAL routines are kept here.           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <nv20_ref.h>
#include <nv20_hal.h>
#include <heap.h>
#include "nvhw.h"
#include "nvhalcpp.h"

//
// NV20 MC HAL entry points.
//

//
// nvHalMcControl
//
RM_STATUS
nvHalMcControl_NV20(VOID *arg)
{
    PMCCONTROLARG_000 pMcControlArg = (PMCCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pMcControlArg->pHalHwInfo;
    PMCHALINFO pMcHalInfo;
    PMCHALINFO_NV20 pMcHalPvtInfo;
    U032 M, N, O, P;
    U032 crystal, pmc_boot0;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalMcControl_NV20\r\n");

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pMcControlArg->id != MC_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pMcControlArg->pHalHwInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;
    pMcHalPvtInfo = (PMCHALINFO_NV20)pHalHwInfo->pMcHalPvtInfo;

    switch (pMcControlArg->cmd)
    {
        case MC_CONTROL_SETREVISION:
        {
            PPRAMHALINFO pPramHalInfo = pHalHwInfo->pPramHalInfo;

            // set the inst mem offset
            pPramHalInfo->PraminOffset = DEVICE_BASE(NV_PRAMIN);

            //
            // Get chip revision information.
            //
            // We need to be careful here that the values we pull from the
            // hardware line up with the constants defined in mc.h, or
            // else we need to convert them accordingly.
            //
            pmc_boot0 = REG_RD32(NV_PMC_BOOT_0);

            pMcHalInfo->Subrevision      = 0;
            //
            // There's a bug in nv20 where PMC_BOOT_0 (on A02 silicon)
            // isn't accurate.  So we get the maskrev from the pci config
            // register.
            pMcHalInfo->MaskRevision     = REG_RD_DRF(_PBUS, _PCI_NV_2, _REVISION_ID);
            //pMcHalInfo->MaskRevision     = MC_GET_MASKREVISION(pmc_boot0);
            pMcHalInfo->Revision         = MC_GET_REVISION(pmc_boot0);
            pMcHalInfo->Implementation   = MC_GET_IMPLEMENTATION(pmc_boot0);
            pMcHalInfo->Architecture     = MC_GET_ARCHITECTURE(pmc_boot0);
            pMcHalInfo->Manufacturer     = NV_PMC_BOOT_0_MANUFACTURER_NVIDIA;

            break;
        }
        case MC_CONTROL_INIT:
        {
            PDACHALINFO pDacHalInfo = pMcControlArg->pHalHwInfo->pDacHalInfo;
            PFIFOHALINFO pFifoHalInfo = pMcControlArg->pHalHwInfo->pFifoHalInfo;
            PGRHALINFO pGrHalInfo = pHalHwInfo->pGrHalInfo;

            // JJV - Reset toggle the Master Control for the fifo and the Bus Mastering logic.
            // This is a fix for a DELL restart hang.19990409-125432.
            {
                U032 x;

                x=REG_RD32(NV_PMC_ENABLE);
                x&=~(DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED) | DRF_DEF(_PMC, _ENABLE, _PPMI, _ENABLED));
                REG_WR32(NV_PMC_ENABLE,x); 
                x|=(DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED) | DRF_DEF(_PMC, _ENABLE, _PPMI, _ENABLED));
                REG_WR32(NV_PMC_ENABLE,x); 
            }

            //
            // Save original MC registers.
            //
            pMcHalPvtInfo->SavePmc = REG_RD32(NV_PMC_ENABLE);
            if (pMcHalPvtInfo->SavePmc == 0)
            {
                //
                // This is 0 if NV not boot device! Enable everything so we at least work.
                //
                pMcHalPvtInfo->SavePmc = 0xFFFFFFFF;
                //
                // Reset all devices and interrupts.
                //
                REG_WR32(NV_PMC_ENABLE, 0x00000000);
            }
            pMcHalPvtInfo->SaveIntrEn0 = REG_RD32(NV_PMC_INTR_EN_0);
            REG_WR32(NV_PMC_ENABLE, 0xFFFFFFFF);

            //
            // Get the current clock settings.  This, of course, assumes that someone
            // has already started up the PLL's.
            //
            // Determine current strap crystal frequency (in Hz)
            //
            if (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_CRYSTAL) == NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_13500K)
                pMcHalInfo->CrystalFreq = 13500000;
            else if (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_CRYSTAL) == NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_14318180)
                pMcHalInfo->CrystalFreq = 14318180;

            // Get the crystal (Fin) * 1000
            crystal = pMcHalInfo->CrystalFreq;

            M = REG_RD_DRF(_PRAMDAC, _MPLL_COEFF, _MDIV);
            N = REG_RD_DRF(_PRAMDAC, _MPLL_COEFF, _NDIV);
            P = REG_RD_DRF(_PRAMDAC, _MPLL_COEFF, _PDIV);
            O = 1;
    
            pDacHalInfo->MPllM = M;
            pDacHalInfo->MPllN = N;
            pDacHalInfo->MPllO = O;
            pDacHalInfo->MPllP = P;
            pDacHalInfo->MClk  = ( M != 0 ) ? (N * crystal / (1 << P) / M) : 0;

            M = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _MDIV);
            N = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _NDIV);
            P = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _PDIV);
            O = 1;
    
            pDacHalInfo->VPllM = M;
            pDacHalInfo->VPllN = N;
            pDacHalInfo->VPllO = O;
            pDacHalInfo->VPllP = P;
            pDacHalInfo->VClk  = ( M != 0 ) ? (N * crystal / (1 << P) / M) : 0;

            M = REG_RD_DRF(_PRAMDAC, _NVPLL_COEFF, _MDIV);
            N = REG_RD_DRF(_PRAMDAC, _NVPLL_COEFF, _NDIV);
            P = REG_RD_DRF(_PRAMDAC, _NVPLL_COEFF, _PDIV);
            O = 1;
    
            pDacHalInfo->NVPllM = M;
            pDacHalInfo->NVPllN = N;
            pDacHalInfo->NVPllO = O;
            pDacHalInfo->NVPllP = P;
            pDacHalInfo->NVClk  = ( M != 0 ) ? (N * crystal / (1 << P) / M) : 0;

            //
            // Initialize number of crtcs.
            //
            pDacHalInfo->NumCrtcs = 1;

            //
            // Get chip revision information.
            //
            // We need to be careful here that the values we pull from the
            // hardware line up with the constants defined in mc.h, or
            // else we need to convert them accordingly.
            //
            pmc_boot0 = REG_RD32(NV_PMC_BOOT_0);

            pMcHalInfo->Subrevision      = 0;
            //
            // There's a bug in nv20 where PMC_BOOT_0 (on A02 silicon)
            // isn't accurate.  So we get the maskrev from the pci config
            // register.
            pMcHalInfo->MaskRevision     = REG_RD_DRF(_PBUS, _PCI_NV_2, _REVISION_ID);
            //pMcHalInfo->MaskRevision     = MC_GET_MASKREVISION(pmc_boot0);
            pMcHalInfo->Revision         = MC_GET_REVISION(pmc_boot0);
            pMcHalInfo->Implementation   = MC_GET_IMPLEMENTATION(pmc_boot0);
            pMcHalInfo->Architecture     = MC_GET_ARCHITECTURE(pmc_boot0);
            pMcHalInfo->Manufacturer     = NV_PMC_BOOT_0_MANUFACTURER_NVIDIA;

            //Save away total number of BAR addresses for the chip
            //as well as what those addresses are.
            pMcHalInfo->TotalPciBars = 3;
            pMcHalInfo->PciBars[0] = REG_RD32(NV_PBUS_PCI_NV_4);
            pMcHalInfo->PciBars[1] = REG_RD32(NV_PBUS_PCI_NV_5);
            pMcHalInfo->PciBars[2] = REG_RD32(NV_PBUS_PCI_NV_6);

            //
            // Set fifo count/context size here so the values are
            // available for the rest of the STATE_INIT sequence.
            //
            pFifoHalInfo->Count = NUM_FIFOS_NV20;
            pFifoHalInfo->ContextSize = FC1_SIZE_NV20;

            //
            // Determine what graphics capabilities we have.
            //
            // For future chips, we default to basic capabilities which
            // is safest and most reasonable in the compatibility versions
            // of the drivers. For the performance drivers that ship with
            // the chip, we'll add the correct detection in that version.
            //

            //
            // NV20 relies on the PCI ID to match GL functionality
            //
            if ((pMcHalInfo->PCIDeviceID >> 16) == 0x203)
            {
                pGrHalInfo->Capabilities =
                    ( NVHAL_GRAPHICS_CAPS_LOGIC_OPS |
                      NVHAL_GRAPHICS_CAPS_AKILL_USERCLIP |
                      NVHAL_GRAPHICS_CAPS_AA_LINES |
                      NVHAL_GRAPHICS_CAPS_AA_POLYS |
                      NVHAL_GRAPHICS_CAPS_2SIDED_LIGHTING |
                      NVHAL_GRAPHICS_CAPS_QUADRO_GENERIC |
                      NVHAL_GRAPHICS_CAPS_UBB |
                      0x8);     // max clips = 8
            }
            else
            {
                pGrHalInfo->Capabilities =
                    ( NVHAL_GRAPHICS_CAPS_LOGIC_OPS |
                      NVHAL_GRAPHICS_CAPS_AA_LINES |
                      NVHAL_GRAPHICS_CAPS_AA_POLYS |
                      NVHAL_GRAPHICS_CAPS_2SIDED_LIGHTING |
                      0x1);     // max clips = 1
            }
            
            //
            // NV20 Rev A05 is properly identified in NV_PMC_BOOT_0,
            // but in PCI_NV_2 it shows up as A03, so we have to perform
            // this check of pmc_boot0 directly.
            //
            
#define MARKETING_HAS_FINALLY_DECIDED_TO_ALLOW_3DTEX_AND_ANISO          1

            if ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV20) &&
                (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV20) &&
                (pMcHalInfo->MaskRevision == MC_MASK_REVISION_A3) &&
                (MC_GET_MASKREVISION(pmc_boot0) == 0xA5) &&
                MARKETING_HAS_FINALLY_DECIDED_TO_ALLOW_3DTEX_AND_ANISO)
            {
                pGrHalInfo->Capabilities |=
                    ( NVHAL_GRAPHICS_CAPS_3D_TEXTURES |
                      NVHAL_GRAPHICS_CAPS_ANISOTROPIC );
            }
            break;
        }
        case MC_CONTROL_DESTROY:
            REG_WR32(NV_PMC_ENABLE, pMcHalPvtInfo->SavePmc);
            REG_WR32(NV_PMC_INTR_EN_0, pMcHalPvtInfo->SaveIntrEn0);
            break;
        case MC_CONTROL_LOAD:
        case MC_CONTROL_UNLOAD:
        case MC_CONTROL_UPDATE:
            break;
    }
    return (RM_OK);
}


//
// nvHalMcPower
//
extern VOID nvHalFbCalcDrvSlw_NV20(PHALHWINFO);

RM_STATUS
nvHalMcPower_NV20(VOID *arg)
{
    PMCPOWERARG_000 pMcPowerArg = (PMCPOWERARG_000)arg;
    PHALHWINFO pHalHwInfo = pMcPowerArg->pHalHwInfo;
    PMCHALINFO_NV20 pMcHalPvtInfo;
    PMCHALPOWERINFO_NV20 pMcHalPowerInfo;
    PDACHALINFO pDacHalInfo; 
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalMcPower_NV20\r\n");

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pMcPowerArg->id != MC_POWER_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pMcPowerArg->pHalHwInfo;
    pMcHalPvtInfo = (PMCHALINFO_NV20)pHalHwInfo->pMcHalPvtInfo;
    pMcHalPowerInfo = &pMcHalPvtInfo->PowerInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;

    //
    // Handle power management state transition.
    //
    switch (pMcPowerArg->newLevel)
    {
        case MC_POWER_LEVEL_0:
            switch (pMcPowerArg->oldLevel)
            {
                case MC_POWER_LEVEL_1:
                    //
                    // Turn up clock.
                    //
                    FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _ON);
                    break;
                case MC_POWER_LEVEL_2:
                    break;
                case MC_POWER_LEVEL_4:
                    //
                    // Restore fb regs.
                    // XXX Does cstatus really need to be power managed?
                    //     It's an ro register!?!?
                    //
                    REG_WR32(NV_PBUS_DEBUG_1, pMcHalPowerInfo->PbusDebug1);
                    REG_WR32(NV_PFB_TIMING2, pMcHalPowerInfo->PfbTiming2);
                    REG_WR32(NV_PFB_CFG0, pMcHalPowerInfo->PfbCfg0);
                    REG_WR32(NV_PFB_CFG1, pMcHalPowerInfo->PfbCfg1);
                    REG_WR32(NV_PGRAPH_FBCFG0, pMcHalPowerInfo->PfbCfg0);
                    REG_WR32(NV_PGRAPH_FBCFG1, pMcHalPowerInfo->PfbCfg1);
                    REG_WR32(NV_PFB_REMAP, pMcHalPowerInfo->PfbRemap);
                    REG_WR32(NV_PFB_ARB_XFER_SZ, pMcHalPowerInfo->PfbArbXferSz);
                    REG_WR32(NV_PFB_ARB_TIMEOUT, pMcHalPowerInfo->PfbArbTimeout);
                    REG_WR32(NV_PFB_ARB_PREDIVIDER, pMcHalPowerInfo->PfbArbPredivider);
                    REG_WR32(NV_PFB_CSTATUS, pMcHalPowerInfo->PfbCstatus);
                    REG_WR32(NV_PFB_REFCTRL, pMcHalPowerInfo->PfbRefCtrl);
                    REG_WR32(NV_PFB_MRS, pMcHalPowerInfo->PfbMrs);
                    //REG_WR32(NV_PFB_EMRS, pMcHalPowerInfo->PfbEmrs);
                    REG_WR32(NV_PFB_REF, pMcHalPowerInfo->PfbRef);

                    //
                    // Turn up clock.
                    //
                    FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _ON);

                    //
                    // restore CRTC config (may include endian-ness)
                    //
                    for (i=0; i < pDacHalInfo->NumCrtcs; i++)
                    {
                        REG_WR32(NV_PCRTC_CONFIG + pDacHalInfo->CrtcInfo[i].RegOffset, 
                                 pMcHalPowerInfo->SaveCrtcConfig[i]);
                    }                 

                    break;

                case MC_POWER_LEVEL_3:
                case MC_POWER_LEVEL_7:
                {
                    PFIFOHALINFO_NV20 pFifoHalPvtInfo;
                    PFBHALINFO_NV20 pFbHalPvtInfo;
                    NV20_TILE_VALUES *tvp;
                    NV20_ZCOMP_VALUES *zvp;

                    pFifoHalPvtInfo = (PFIFOHALINFO_NV20)pMcPowerArg->pHalHwInfo->pFifoHalPvtInfo;
                    pFbHalPvtInfo = (PFBHALINFO_NV20)pMcPowerArg->pHalHwInfo->pFifoHalPvtInfo;

                    nvHalFbCalcDrvSlw_NV20(pHalHwInfo);

                    //
                    // Restore fb regs.
                    // XXX Does cstatus really need to be power managed?
                    //     It's an ro register!?!?
                    //
                    REG_WR32(NV_PBUS_DEBUG_1, pMcHalPowerInfo->PbusDebug1);
                    REG_WR32(NV_PFB_TIMING2, pMcHalPowerInfo->PfbTiming2);
                    REG_WR32(NV_PFB_CFG0, pMcHalPowerInfo->PfbCfg0);
                    REG_WR32(NV_PFB_CFG1, pMcHalPowerInfo->PfbCfg1);
                    REG_WR32(NV_PGRAPH_FBCFG0, pMcHalPowerInfo->PfbCfg0);
                    REG_WR32(NV_PGRAPH_FBCFG1, pMcHalPowerInfo->PfbCfg1);
                    REG_WR32(NV_PFB_CSTATUS, pMcHalPowerInfo->PfbCstatus);
                    REG_WR32(NV_PFB_REFCTRL, pMcHalPowerInfo->PfbRefCtrl);
                    REG_WR32(NV_PFB_MRS, pMcHalPowerInfo->PfbMrs);
                    REG_WR32(NV_PFB_REF, pMcHalPowerInfo->PfbRef);

                    REG_WR32(NV_PFIFO_RAMHT, pMcHalPowerInfo->PfifoRamHt);
                    REG_WR32(NV_PFIFO_RAMFC, pMcHalPowerInfo->PfifoRamFc);
                    REG_WR32(NV_PFIFO_RAMRO, pMcHalPowerInfo->PfifoRamRo);

                    //
                    // Make sure write protection is disabled.
                    //
                    FLD_WR_DRF_DEF(_PFB, _NVM, _MODE, _DISABLE);
                    
                    // restore video registers - 8/29/2000 - dpearce
                    REG_WR32(NV_PVIDEO_BASE(0),pMcHalPowerInfo->PvideoBase0) ;
                    REG_WR32(NV_PVIDEO_BASE(1),pMcHalPowerInfo->PvideoBase1) ;
                    REG_WR32(NV_PVIDEO_LIMIT(0),pMcHalPowerInfo->PvideoLimit0) ;
                    REG_WR32(NV_PVIDEO_LIMIT(1),pMcHalPowerInfo->PvideoLimit1) ;
                    REG_WR32(NV_PVIDEO_LUMINANCE(0),pMcHalPowerInfo->PvideoLuminance0) ;
                    REG_WR32(NV_PVIDEO_LUMINANCE(1),pMcHalPowerInfo->PvideoLuminance1) ;
                    REG_WR32(NV_PVIDEO_CHROMINANCE(0),pMcHalPowerInfo->PvideoChrominance0) ;
                    REG_WR32(NV_PVIDEO_CHROMINANCE(1),pMcHalPowerInfo->PvideoChrominance1) ;
                    REG_WR32(NV_PVIDEO_OFFSET(0),pMcHalPowerInfo->PvideoOffset0) ;
                    REG_WR32(NV_PVIDEO_OFFSET(1),pMcHalPowerInfo->PvideoOffset1) ;
                    REG_WR32(NV_PVIDEO_SIZE_IN(0),pMcHalPowerInfo->PvideoSizeIn0) ;
                    REG_WR32(NV_PVIDEO_SIZE_IN(1),pMcHalPowerInfo->PvideoSizeIn1) ;
                    REG_WR32(NV_PVIDEO_POINT_IN(0),pMcHalPowerInfo->PvideoPointIn0) ;
                    REG_WR32(NV_PVIDEO_POINT_IN(1),pMcHalPowerInfo->PvideoPointIn1) ;
                    REG_WR32(NV_PVIDEO_DS_DX(0),pMcHalPowerInfo->PvideoDsDx0) ;
                    REG_WR32(NV_PVIDEO_DS_DX(1),pMcHalPowerInfo->PvideoDsDx1) ;
                    REG_WR32(NV_PVIDEO_DT_DY(0),pMcHalPowerInfo->PvideoDtDy0) ;
                    REG_WR32(NV_PVIDEO_DT_DY(1),pMcHalPowerInfo->PvideoDtDy1) ;
                    REG_WR32(NV_PVIDEO_POINT_OUT(0),pMcHalPowerInfo->PvideoPointOut0) ;
                    REG_WR32(NV_PVIDEO_POINT_OUT(1),pMcHalPowerInfo->PvideoPointOut1) ;
                    REG_WR32(NV_PVIDEO_SIZE_OUT(0),pMcHalPowerInfo->PvideoSizeOut0) ;
                    REG_WR32(NV_PVIDEO_SIZE_OUT(1),pMcHalPowerInfo->PvideoSizeOut1) ;
                    REG_WR32(NV_PVIDEO_FORMAT(0),pMcHalPowerInfo->PvideoFormat0) ;
                    REG_WR32(NV_PVIDEO_FORMAT(1),pMcHalPowerInfo->PvideoFormat1) ;
                    REG_WR32(NV_PVIDEO_COLOR_KEY,pMcHalPowerInfo->PvideoColorKey) ;

                    //
                    // Restore tiling registers.
                    //
                    tvp = &pMcHalPowerInfo->TilingValues[0];
                    for (i = 0; i < NV_PFB_TILE__SIZE_1; i++, tvp++)
                    {
                        //
                        // We may have freed a tiled region when we were
                        // unable to write to the hardware...sync up the
                        // hw to our software state now.
                        //
                        if ((pFbHalPvtInfo->usedTiles & MEM_TYPE_TILE_RANGE_SHIFT(i)) == 0)
                            tvp->tile &= ~DRF_DEF(_PFB, _TILE, _REGION, _VALID);
                        REG_WR32(NV_PFB_TLIMIT(i), tvp->tlimit);
                        REG_WR32(NV_PFB_TSIZE(i), tvp->tsize);
                        REG_WR32(NV_PFB_TILE(i), tvp->tile);

                        REG_WR32(NV_PGRAPH_TLIMIT(i), tvp->tlimit);
                        REG_WR32(NV_PGRAPH_TSIZE(i), tvp->tsize);
                        REG_WR32(NV_PGRAPH_TILE(i), tvp->tile);

                        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                                     NV_IGRAPH_TC_MINTFCDEBUG_TLIMIT_ADDRESS(i),
                                     tvp->tlimit);
                        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                                     NV_IGRAPH_TC_MINTFCDEBUG_TPITCH_ADDRESS(i),
                                     tvp->tsize);
                        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                                     NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(i),
                                     tvp->tile);
                    }

                    //
                    // Restore Z compression registers.
                    //
                    zvp = &pMcHalPowerInfo->ZcompValues[0];
                    for (i = 0; i < NV_PFB_ZCOMP__SIZE_1; i++, zvp++)
                    {
                        if ((pFbHalPvtInfo->usedTiles & MEM_TYPE_TILE_RANGE_SHIFT(i)) == 0)
                            zvp->zcomp &= ~DRF_DEF(_PFB, _ZCOMP, _EN, _TRUE);
                        REG_WR32(NV_PFB_ZCOMP(i), zvp->zcomp);
                        REG_WR32(NV_PGRAPH_ZCOMP(i), zvp->zcomp);
                        RDI_REG_WR32(NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                                     NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(i),
                                     zvp->zcomp);
                    }
                    REG_WR32(NV_PFB_ZCOMP_OFFSET, pMcHalPowerInfo->ZcompOffset);
                    REG_WR32(NV_PGRAPH_ZCOMP_OFFSET, pMcHalPowerInfo->ZcompOffset);

                    //
                    // Restore fifo mode register.
                    //
                    REG_WR32(NV_PFIFO_MODE, pFifoHalPvtInfo->Mode);

                    //
                    // Set to unused channel to force context switch
                    // when fifo is accessed.
                    //
                    REG_WR32(NV_PFIFO_CACHE0_PUSH1, NUM_FIFOS_NV20-1);
                    REG_WR32(NV_PFIFO_CACHE1_PUSH1, NUM_FIFOS_NV20-1);

                    //
                    // Turn up clock.
                    //
                    FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _ON);

                    //
                    // Restore CRTC config (may include endian-ness).
                    //
                    //
                    // Restore SetOffsetRange registers.
                    //
                    //
                    for (i=0; i < pDacHalInfo->NumCrtcs; i++)
                    {
                        REG_WR32(NV_PCRTC_CONFIG + pDacHalInfo->CrtcInfo[i].RegOffset, 
                                 pMcHalPowerInfo->SaveCrtcConfig[i]);

                        REG_WR32(NV_PCRTC_RASTER_START + pDacHalInfo->CrtcInfo[i].RegOffset,
                                 pMcHalPowerInfo->PcrtcRasterStart[i]);
                        REG_WR32(NV_PCRTC_RASTER_STOP + pDacHalInfo->CrtcInfo[i].RegOffset,
                                 pMcHalPowerInfo->PcrtcRasterStop[i]);
                    }                 
                }
            }
            break;
        case MC_POWER_LEVEL_1:
            //
            // Turn down clocks.
            //
            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _VPLL);
            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _VPLL2);
            break;
        case MC_POWER_LEVEL_2:
            break;
        case MC_POWER_LEVEL_4:
            // refresh
            pMcHalPowerInfo->PfbTiming2 = REG_RD32(NV_PFB_TIMING2);
            // max refresh
            FLD_WR_DRF_NUM(_PFB, _TIMING2, _REFRESH, 0xFFFFFFFF);

            //
            // Chip has been init'd with the BIOS init tables, unless the BIOS
            // has an incompatible structure and the table was not found, in
            // which case we used a default table, which may not have the
            // correct RAM config, so we save away the current config for
            // restoration above.
            //
            pMcHalPowerInfo->PfbRemap = REG_RD32(NV_PFB_REMAP);
            pMcHalPowerInfo->PfbArbXferSz = REG_RD32(NV_PFB_ARB_XFER_SZ);
            pMcHalPowerInfo->PfbArbTimeout = REG_RD32(NV_PFB_ARB_TIMEOUT);
            pMcHalPowerInfo->PfbArbPredivider = REG_RD32(NV_PFB_ARB_PREDIVIDER);
            pMcHalPowerInfo->PfbCfg0 = REG_RD32(NV_PFB_CFG0);
            pMcHalPowerInfo->PfbCfg1 = REG_RD32(NV_PFB_CFG1);
            pMcHalPowerInfo->PfbCstatus = REG_RD32(NV_PFB_CSTATUS);
            pMcHalPowerInfo->PfbRefCtrl = REG_RD32(NV_PFB_REFCTRL);
            pMcHalPowerInfo->PfbMrs = REG_RD32(NV_PFB_MRS);
            pMcHalPowerInfo->PfbEmrs = REG_RD32(NV_PFB_EMRS);
            pMcHalPowerInfo->PfbRef = REG_RD32(NV_PFB_REF);
            pMcHalPowerInfo->PbusDebug1 = REG_RD32(NV_PBUS_DEBUG_1);

            // CRTC config (may include endian-ness)
            for (i=0; i< pDacHalInfo->NumCrtcs; i++)
            {
                pMcHalPowerInfo->SaveCrtcConfig[i] = REG_RD32(NV_PCRTC_CONFIG + pDacHalInfo->CrtcInfo[i].RegOffset); 
            }
            //
            // Turn down clock.
            //
            //FLD_WR_DRF_DEF(_PRAMDAC, _PLL_SETUP_CONTROL, _PWRDWN, _VPLL);
            //FLD_WR_DRF_DEF(_PRAMDAC, _PLL_SETUP_CONTROL, _PWRDWN, _VPLL2);
            break;
        case MC_POWER_LEVEL_3:
        case MC_POWER_LEVEL_7:
        {
            NV20_TILE_VALUES *tvp;
            NV20_ZCOMP_VALUES *zvp;

            //
            // Save tiling value.
            //
            tvp = &pMcHalPowerInfo->TilingValues[0];
            for (i = 0; i < NV_PFB_TILE__SIZE_1; i++, tvp++)
            {
                tvp->tlimit = REG_RD32(NV_PFB_TLIMIT(i));
                tvp->tsize  = REG_RD32(NV_PFB_TSIZE(i));
                tvp->tile   = REG_RD32(NV_PFB_TILE(i));
            }
            
#ifdef MACOS            
            //because apps do not get notification for power management in MacOS (unless registered, and no one does),
            //video must be absolutely stopped and the notifier posted for that stop before continuing power down.
            if(REG_RD32(NV_PVIDEO_BUFFER))
            {                         
                //notify the chip we would like to stop video overlay buffer activity
                //(we assume resman was already wanting to wait for the scaler to finish current buffer)
                REG_WR32(NV_PVIDEO_STOP,DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _ACTIVE)
                                      |DRF_DEF(_PVIDEO,_STOP,_METHOD,_NORMALLY));
                                      
                //wait for the hardware to finish
                while(REG_RD32(NV_PVIDEO_BUFFER)){}     
            }               
#endif

            //save some video registers
            pMcHalPowerInfo->PvideoBase0=REG_RD32(NV_PVIDEO_BASE(0));
            pMcHalPowerInfo->PvideoBase1=REG_RD32(NV_PVIDEO_BASE(1));
            pMcHalPowerInfo->PvideoLimit0=REG_RD32(NV_PVIDEO_LIMIT(0));
            pMcHalPowerInfo->PvideoLimit1=REG_RD32(NV_PVIDEO_LIMIT(1));
            pMcHalPowerInfo->PvideoLuminance0=REG_RD32(NV_PVIDEO_LUMINANCE(0));
            pMcHalPowerInfo->PvideoLuminance1=REG_RD32(NV_PVIDEO_LUMINANCE(1));
            pMcHalPowerInfo->PvideoChrominance0=REG_RD32(NV_PVIDEO_CHROMINANCE(0));
            pMcHalPowerInfo->PvideoChrominance1=REG_RD32(NV_PVIDEO_CHROMINANCE(1));
            pMcHalPowerInfo->PvideoOffset0=REG_RD32(NV_PVIDEO_OFFSET(0));
            pMcHalPowerInfo->PvideoOffset1=REG_RD32(NV_PVIDEO_OFFSET(1));
            pMcHalPowerInfo->PvideoSizeIn0=REG_RD32(NV_PVIDEO_SIZE_IN(0));
            pMcHalPowerInfo->PvideoSizeIn1=REG_RD32(NV_PVIDEO_SIZE_IN(1));
            pMcHalPowerInfo->PvideoPointIn0=REG_RD32(NV_PVIDEO_POINT_IN(0));
            pMcHalPowerInfo->PvideoPointIn1=REG_RD32(NV_PVIDEO_POINT_IN(1));
            pMcHalPowerInfo->PvideoDsDx0=REG_RD32(NV_PVIDEO_DS_DX(0));
            pMcHalPowerInfo->PvideoDsDx1=REG_RD32(NV_PVIDEO_DS_DX(1));
            pMcHalPowerInfo->PvideoDtDy0=REG_RD32(NV_PVIDEO_DT_DY(0));
            pMcHalPowerInfo->PvideoDtDy1=REG_RD32(NV_PVIDEO_DT_DY(1));
            pMcHalPowerInfo->PvideoPointOut0=REG_RD32(NV_PVIDEO_POINT_OUT(0));
            pMcHalPowerInfo->PvideoPointOut1=REG_RD32(NV_PVIDEO_POINT_OUT(1));
            pMcHalPowerInfo->PvideoSizeOut0=REG_RD32(NV_PVIDEO_SIZE_OUT(0));
            pMcHalPowerInfo->PvideoSizeOut1=REG_RD32(NV_PVIDEO_SIZE_OUT(1));
            pMcHalPowerInfo->PvideoFormat0=REG_RD32(NV_PVIDEO_FORMAT(0));
            pMcHalPowerInfo->PvideoFormat1=REG_RD32(NV_PVIDEO_FORMAT(1));
            pMcHalPowerInfo->PvideoColorKey=REG_RD32(NV_PVIDEO_COLOR_KEY);


            //
            // Save Z compression registers.
            //
            zvp = &pMcHalPowerInfo->ZcompValues[0];
            for (i = 0; i < NV_PFB_ZCOMP__SIZE_1; i++, zvp++)
            {
                zvp->zcomp = REG_RD32(NV_PFB_ZCOMP(i));
            }
            pMcHalPowerInfo->ZcompOffset = REG_RD32(NV_PFB_ZCOMP_OFFSET);

            // refresh
            pMcHalPowerInfo->PfbTiming2 = REG_RD32(NV_PFB_TIMING2);

            // max refresh
            if (pMcPowerArg->newLevel == MC_POWER_LEVEL_3)
                FLD_WR_DRF_NUM(_PFB, _TIMING2, _REFRESH, 0xFFFFFFFF);

            //
            // Chip has been init'd with the BIOS init tables, unless the BIOS
            // has an incompatible structure and the table was not found, in
            // which case we used a default table, which may not have the
            // correct RAM config, so we save away the current config for
            // restoration above.
            //
            pMcHalPowerInfo->PfbCfg0 = REG_RD32(NV_PFB_CFG0);
            pMcHalPowerInfo->PfbCfg1 = REG_RD32(NV_PFB_CFG1);
#if 0
            //
            // On nv20, the PGRAPH_FBCFG* registers do not completely
            // mirror the PFB_CFG* equivalents, so the comparisons below
            // will always fail.
            //
            // Here's the details from the hw dudes:
            //
            // fbcfg0
            //fbregs.nv_pfb_cfg0.part = ToInt(ToU002(NV_PFB_CFG0_PART_2));
            //fbregs.nv_pfb_cfg0.partsense =
            //               ToInt(ToU002(NV_PFB_CFG0_PARTSENSE_00));
            //fbregs.nv_pfb_cfg0.extbank =
            //               ToInt(ToU001(NV_PFB_CFG0_EXTBANK_0));
            //
            // fbcfg1 (we only instantiate a portion of this reg)
            //fbregs.nv_pfb_cfg1.col   = ToInt(ToU004(NV_PFB_CFG1_COL_8));
            //fbregs.nv_pfb_cfg1.rowA  = ToInt(ToU004(NV_PFB_CFG1_ROWA_9));
            //fbregs.nv_pfb_cfg1.rowB  = ToInt(ToU004(NV_PFB_CFG1_ROWB_9));
            //fbregs.nv_pfb_cfg1.bankA = ToInt(ToU001(NV_PFB_CFG1_BANKA_1));
            //fbregs.nv_pfb_cfg1.bankB = ToInt(ToU001(NV_PFB_CFG1_BANKB_1));
            //

            //
            // Assert NV_PGRAPH_CFG0/NV_PGRAPH_CFG1 are the same.
            //
            if (REG_RD32(NV_PFB_CFG0) != REG_RD32(NV_PGRAPH_FBCFG0))
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: pfb_cfg0 != pgraph_cfg0\n");
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, " pfb_cfg0 ", REG_RD32(NV_PFB_CFG0));
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, " pgraph_cfg0 ", REG_RD32(NV_PGRAPH_FBCFG0));
                DBG_BREAKPOINT();
            }
            if (REG_RD32(NV_PFB_CFG1) != REG_RD32(NV_PGRAPH_FBCFG1))
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: pfb_cfg1 != pgraph_cfg1\n");
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, " pfb_cfg1 ", REG_RD32(NV_PFB_CFG1));
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, " pgraph_cfg1 ", REG_RD32(NV_PGRAPH_FBCFG1));
                DBG_BREAKPOINT();
            }
#endif

            pMcHalPowerInfo->PfbRemap = REG_RD32(NV_PFB_REMAP);
            pMcHalPowerInfo->PfbArbXferSz = REG_RD32(NV_PFB_ARB_XFER_SZ);
            pMcHalPowerInfo->PfbArbTimeout = REG_RD32(NV_PFB_ARB_TIMEOUT);
            pMcHalPowerInfo->PfbArbPredivider = REG_RD32(NV_PFB_ARB_PREDIVIDER);
            pMcHalPowerInfo->PfbCstatus = REG_RD32(NV_PFB_CSTATUS);
            pMcHalPowerInfo->PfbRefCtrl = REG_RD32(NV_PFB_REFCTRL);
            pMcHalPowerInfo->PfbMrs = REG_RD32(NV_PFB_MRS);
            pMcHalPowerInfo->PfbEmrs = REG_RD32(NV_PFB_EMRS);
            pMcHalPowerInfo->PfbRef = REG_RD32(NV_PFB_REF);
            pMcHalPowerInfo->PbusDebug1 = REG_RD32(NV_PBUS_DEBUG_1);
            pMcHalPowerInfo->PfifoRamHt = REG_RD32(NV_PFIFO_RAMHT);
            pMcHalPowerInfo->PfifoRamFc = REG_RD32(NV_PFIFO_RAMFC);
            pMcHalPowerInfo->PfifoRamRo = REG_RD32(NV_PFIFO_RAMRO);

            //
            // CRTC config (may include endian-ness)
            //
            // Save SetOffsetRange regs.
            //
            for (i=0; i < pDacHalInfo->NumCrtcs; i++)
            {
                pMcHalPowerInfo->SaveCrtcConfig[i] = REG_RD32(NV_PCRTC_CONFIG + pDacHalInfo->CrtcInfo[i].RegOffset); 
                pMcHalPowerInfo->PcrtcRasterStart[i] = REG_RD32(NV_PCRTC_RASTER_START + pDacHalInfo->CrtcInfo[i].RegOffset);
                pMcHalPowerInfo->PcrtcRasterStop[i] = REG_RD32(NV_PCRTC_RASTER_STOP + pDacHalInfo->CrtcInfo[i].RegOffset); 
            }

/*
When going into hibernation, the NT miniport makes a power state transition from state 0 to state 3.  State 3
implies that ALL clocks have been put in a power-down state, i.e. lowered.  Unfortunately, NT also requires 
the ability to perform VGA accesses *AFTER* the power management call.  Putting PClk into a power-down state 
disables VGA accesses, so for the time being, for NT, we cannot allow PClk to be put into a power-down state.
NOTE:  This is a liability if the system does not truly remove power from the device during hibernation.  In
this case, PClk will still be drawing full current, which could lead to power saving specs' being violated or,
more importantly, reliability problems with the device.
*/
#ifndef NTRM
            //
            // Turn down clock.
            //
            if (pMcPowerArg->newLevel == MC_POWER_LEVEL_3)
            {
                FLD_WR_DRF_DEF(_PRAMDAC, _PLL_SETUP_CONTROL, _PWRDWN, _VPLL);
                FLD_WR_DRF_DEF(_PRAMDAC, _PLL_SETUP_CONTROL, _PWRDWN, _VPLL2);
            }
            break;
#endif
        }
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}

BOOL IsNV20_NV20(PMCHALINFO pMcHalInfo)
{
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV20) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV20));
}

BOOL IsNV20MaskRevA02_NV20(PMCHALINFO pMcHalInfo)
{
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV20) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV20) &&
            (pMcHalInfo->MaskRevision == MC_MASK_REVISION_A2));
}

BOOL IsNV20MaskRevA03_NV20(PMCHALINFO pMcHalInfo)
{
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV20) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV20) &&
            (pMcHalInfo->MaskRevision == MC_MASK_REVISION_A3));
}

