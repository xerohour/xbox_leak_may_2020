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
* Module: MCNV04.C                                                          *
*   The master control chip dependent HAL routines are kept here.           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <nv4_ref.h>
#include <nv4_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

//
// NV04 MC HAL entry points.
//

//
// nvHalMcControl
//
RM_STATUS
nvHalMcControl_NV04(VOID *arg)
{
    PMCCONTROLARG_000 pMcControlArg = (PMCCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pMcControlArg->pHalHwInfo;
    PMCHALINFO pMcHalInfo;
    PMCHALINFO_NV04 pMcHalPvtInfo;
    U032 M, N, O, P;
    U032 crystal;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalMcControl_NV04\r\n");

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pMcControlArg->id != MC_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pMcHalInfo = pHalHwInfo->pMcHalInfo;
    pMcHalPvtInfo = (PMCHALINFO_NV04)pHalHwInfo->pMcHalPvtInfo;

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
            // hardware line up with the constants defined in chipid.h, or
            // else we need to convert them accordingly.
            //
            // On NV4 and family, they do so we're OK.
            //

            pMcHalInfo->Revision = REG_RD_DRF(_PMC, _BOOT_0, _MAJOR_REVISION);
            pMcHalInfo->Subrevision = REG_RD_DRF(_PMC, _BOOT_0, _MINOR_REVISION);
            pMcHalInfo->Implementation = REG_RD_DRF(_PMC, _BOOT_0, _IMPLEMENTATION);
            pMcHalInfo->Architecture = REG_RD_DRF(_PMC, _BOOT_0, _ARCHITECTURE);
            pMcHalInfo->Manufacturer = REG_RD_DRF(_PMC, _BOOT_0, _MANUFACTURER);
            pMcHalInfo->MaskRevision = REG_RD_DRF(_PMC, _BOOT_0, _MASK_REVISION);

            //
            // Since the Implementation is used by the RM only, we
            // set it to the MASK_REVISION to make out chip id 
            // function implementations cleaner.
            //
            // NOTE: BE CAREFUL HERE!  The MaskRevision must line up
            // with the MC_IMPLEMENTATION macros in <mc.h>.
            //
            pMcHalInfo->Implementation = pMcHalInfo->MaskRevision;
            break;
        }
        case MC_CONTROL_INIT:
        {
            PDACHALINFO pDacHalInfo = pHalHwInfo->pDacHalInfo;
            PFIFOHALINFO pFifoHalInfo = pHalHwInfo->pFifoHalInfo;
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
            if ((!P) && IsNV4_NV04(pMcHalInfo))
                P = 1;      // never really zero on NV4
            pDacHalInfo->MClk  = ( M != 0 ) ? (N * crystal / (1 << P) / M) : 0;
    
            M = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _MDIV);
            N = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _NDIV);
            P = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _PDIV);
            O = 1;
    
            pDacHalInfo->VPllM = M;
            pDacHalInfo->VPllN = N;
            pDacHalInfo->VPllO = O;
            pDacHalInfo->VPllP = P;
            //pDacHalInfo->VClk  = (N * 12096 / (1 << P) / M) * 500;
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
            // hardware line up with the constants defined in chipid.h, or
            // else we need to convert them accordingly.
            //
            // On NV4 and family, they do so we're OK.
            //
            pMcHalInfo->Revision = REG_RD_DRF(_PMC, _BOOT_0, _MAJOR_REVISION);
            pMcHalInfo->Subrevision = REG_RD_DRF(_PMC, _BOOT_0, _MINOR_REVISION);
            pMcHalInfo->Implementation = REG_RD_DRF(_PMC, _BOOT_0, _IMPLEMENTATION);
            pMcHalInfo->Architecture = REG_RD_DRF(_PMC, _BOOT_0, _ARCHITECTURE);
            pMcHalInfo->Manufacturer = REG_RD_DRF(_PMC, _BOOT_0, _MANUFACTURER);
            pMcHalInfo->MaskRevision = REG_RD_DRF(_PMC, _BOOT_0, _MASK_REVISION);

            //
            // Since the Implementation is used by the RM only, we
            // set it to the MASK_REVISION to make out chip id 
            // function implementations cleaner.
            //
            // NOTE: BE CAREFUL HERE!  The MaskRevision must line up
            // with the MC_IMPLEMENTATION macros in <mc.h>.
            //
            pMcHalInfo->Implementation = pMcHalInfo->MaskRevision;

            //Save away total number of BAR addresses for the chip
            //as well as what those addresses are.
            pMcHalInfo->TotalPciBars = 2;
            pMcHalInfo->PciBars[0] = REG_RD32(NV_PBUS_PCI_NV_4);
            pMcHalInfo->PciBars[1] = REG_RD32(NV_PBUS_PCI_NV_5);

            //
            // If this is a TNT2 revB, enable AGP differential mode
            //
            if (IsNV5orBetter_NV04(pMcHalInfo) &&
                (REG_RD_DRF(_PBUS, _PCI_NV_2, _REVISION_ID) & NV_PBUS_PCI_NV_2_REVISION_ID_B01))
            {
                FLD_WR_DRF_DEF(_PBUS, _DEBUG_2, _AGP_AD_STB, _DIFFERENTIAL);
            }

            //
            // Only worry about PMI toggling for NV5 class devices
            //
            if (IsNV5orBetter_NV04(pMcHalInfo))
            {
                U032 savedPCI1, savedPCI19;

                //
                // Toggle PMI off to clear any strobe glitches from Camino that occurred during POST
                //
                // First disable bus mastering
                //
                savedPCI1 = REG_RD32(NV_PBUS_PCI_NV_1);
                REG_WR32(NV_PBUS_PCI_NV_1, savedPCI1 & ~DRF_DEF(_PBUS, _PCI_NV_1, _BUS_MASTER, _ENABLED));
                savedPCI19 = REG_RD32(NV_PBUS_PCI_NV_19);
                REG_WR32(NV_PBUS_PCI_NV_19, savedPCI19 & ~DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_AGP_ENABLE, _ON));

                // Toggle PMI off                     
                REG_WR32(NV_PMC_ENABLE, 0xFFFFFFFF & ~DRF_DEF(_PMC, _ENABLE, _PPMI, _ENABLED));

                // Turn everything back on    
                REG_WR32(NV_PMC_ENABLE, 0xFFFFFFFF);

                //
                // Restore AGP Busmastering
                //
                REG_WR32(NV_PBUS_PCI_NV_1, savedPCI1);
                REG_WR32(NV_PBUS_PCI_NV_19, savedPCI19);
            }

            //
            // Set fifo count/context size here so the values are
            // available for the rest of the STATE_INIT sequence.
            //
            pFifoHalInfo->Count = NUM_FIFOS_NV04;
            pFifoHalInfo->ContextSize = FC_SIZE_NV04;

            //
            // Determine what graphics capabilities we have.
            //
            // For future chips, we default to basic capabilities which is safest and most
            // reasonable in the compatibility versions of the drivers. For the performance
            // drivers that ship with the chip, we'll add the correct detection in that version.
            //
            pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_UBB                // unified backbuffers
                                        | 0x1);                                 // max clips = 1

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
RM_STATUS
nvHalMcPower_NV04(VOID *arg)
{
    PMCPOWERARG_000 pMcPowerArg = (PMCPOWERARG_000)arg;
    PHALHWINFO pHalHwInfo = pMcPowerArg->pHalHwInfo;
    PMCHALINFO_NV04 pMcHalPvtInfo;
    PMCHALPOWERINFO_NV04 pMcHalPowerInfo;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalMcPower_NV04\r\n");

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pMcPowerArg->id != MC_POWER_000)
        return (RM_ERR_VERSION_MISMATCH);

    pMcHalPvtInfo = (PMCHALINFO_NV04)pHalHwInfo->pMcHalPvtInfo;
    pMcHalPowerInfo = &pMcHalPvtInfo->PowerInfo;

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
                    // Power up mode - restore previous refresh value.
                    //
                    REG_WR32(NV_PFB_DEBUG_0, pMcHalPowerInfo->PfbDebug0);
                    REG_WR32(NV_PFB_BOOT_0, pMcHalPowerInfo->PfbBoot0);
                    REG_WR32(NV_PFB_CONFIG_0, pMcHalPowerInfo->PfbConfig0);
                    REG_WR32(NV_PFB_CONFIG_1, pMcHalPowerInfo->PfbConfig1);

                    //
                    // Turn up clock.
                    //
                    FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _ON);
                    break;
                case MC_POWER_LEVEL_3:
                case MC_POWER_LEVEL_7:
                {
                    PFIFOHALINFO_NV04 pFifoHalPvtInfo;

                    pFifoHalPvtInfo = (PFIFOHALINFO_NV04)pHalHwInfo->pFifoHalPvtInfo;
                    
                    //
                    // Power up mode - restore previous refresh value
                    //
                    REG_WR32(NV_PFB_DEBUG_0, pMcHalPowerInfo->PfbDebug0);
                    REG_WR32(NV_PFB_BOOT_0, pMcHalPowerInfo->PfbBoot0);
                    REG_WR32(NV_PFB_CONFIG_0, pMcHalPowerInfo->PfbConfig0);
                    REG_WR32(NV_PFB_CONFIG_1, pMcHalPowerInfo->PfbConfig1);

                    REG_WR32(NV_PFIFO_RAMHT, pMcHalPowerInfo->PfifoRamHt);
                    REG_WR32(NV_PFIFO_RAMFC, pMcHalPowerInfo->PfifoRamFc);
                    REG_WR32(NV_PFIFO_RAMRO, pMcHalPowerInfo->PfifoRamRo);

                    //
                    // Make sure write protection is disabled.
                    //
                    FLD_WR_DRF_DEF(_PFB, _CONFIG_0, _PRAMIN_WR, _INIT);

                    //
                    // Restore fifo mode register.
                    //
                    REG_WR32(NV_PFIFO_MODE, pFifoHalPvtInfo->Mode);

                    //
                    // Set to unused channel to force context switch
                    // when fifo is accessed.
                    //
                    REG_WR32(NV_PFIFO_CACHE0_PUSH1, NUM_FIFOS_NV04-1);
                    REG_WR32(NV_PFIFO_CACHE1_PUSH1, NUM_FIFOS_NV04-1);

                    //
                    // Turn up clock.
                    //
                    FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _ON);
                }
            }
            break;
        case MC_POWER_LEVEL_1:
            //
            // Turn down clock.
            //
            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _VPLL);
            break;
        case MC_POWER_LEVEL_2:
            break;
        case MC_POWER_LEVEL_3:
        case MC_POWER_LEVEL_4:
        case MC_POWER_LEVEL_7:
            // power down mode - max out memory refresh value
            pMcHalPowerInfo->PfbDebug0 = REG_RD32(NV_PFB_DEBUG_0);
            FLD_WR_DRF_NUM(_PFB, _DEBUG_0, _REFRESH_COUNTX64, 0x01);

            // Chip has been init'd with the BIOS init tables, unless the BIOS
            // has an incompatible structure and the table was not found, in which case
            // we used a default table, which may not have the correct RAM config, so
            // we save away the current config for restoration above.
            pMcHalPowerInfo->PfbBoot0 = REG_RD32(NV_PFB_BOOT_0);
            pMcHalPowerInfo->PfbConfig0 = REG_RD32(NV_PFB_CONFIG_0);
            pMcHalPowerInfo->PfbConfig1 = REG_RD32(NV_PFB_CONFIG_1);

            pMcHalPowerInfo->PfifoRamHt = REG_RD32(NV_PFIFO_RAMHT);
            pMcHalPowerInfo->PfifoRamFc = REG_RD32(NV_PFIFO_RAMFC);
            pMcHalPowerInfo->PfifoRamRo = REG_RD32(NV_PFIFO_RAMRO);

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
            //FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _VPLL);
#endif
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}


//
// Chip id functions internal to the HAL.
//
// Note that checking for an Architecture value of MC_ARCHITECTURE_NV04
// should probably be replaced with an ASSERT (i.e. these routines should
// only be used by code operating on an nv4 family device).
//
BOOL
IsNV4_NV04(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV04) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV04));
}

BOOL
IsNV5_NV04(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV04) &&
            (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV05));
}

BOOL
IsNV0A_NV04(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV04) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV0A));
}

BOOL
IsNV5orBetter_NV04(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    return ((pMcHalInfo->Architecture > MC_ARCHITECTURE_NV04) ||
            ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV04) &&
             (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV05)));
}
