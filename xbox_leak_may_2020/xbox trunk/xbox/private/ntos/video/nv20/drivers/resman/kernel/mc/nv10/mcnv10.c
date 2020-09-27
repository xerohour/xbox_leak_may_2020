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
* Module: MCNV10.C                                                          *
*   The master control chip dependent HAL routines are kept here.           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <nv10_ref.h>
#include <nv10_hal.h>
#include <heap.h>
#include "nvhw.h"
#include "nvhalcpp.h"
#include "cr11help.h"

//
// NV10 MC HAL entry points.
//

//
// nvHalMcControl
//
RM_STATUS
nvHalMcControl_NV10(VOID *arg)
{
    PMCCONTROLARG_000 pMcControlArg = (PMCCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pMcControlArg->pHalHwInfo;
    PMCHALINFO pMcHalInfo;
    PMCHALINFO_NV10 pMcHalPvtInfo;
    U032 M, N, O, P;
    U032 crystal, pmc_boot0;
    U032 IsPllSlowed = 0;  // some compilers think its uninitialized otherwise

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalMcControl_NV10\r\n");

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pMcControlArg->id != MC_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pMcControlArg->pHalHwInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;
    pMcHalPvtInfo = (PMCHALINFO_NV10)pHalHwInfo->pMcHalPvtInfo;

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
            // On NV10 and family, they do so we're OK.
            //
            pmc_boot0 = REG_RD32(NV_PMC_BOOT_0);

            pMcHalInfo->Subrevision      = 0;
            pMcHalInfo->MaskRevision     = MC_GET_MASKREVISION(pmc_boot0);
            pMcHalInfo->Revision         = MC_GET_REVISION(pmc_boot0);
            pMcHalInfo->Implementation   = MC_GET_IMPLEMENTATION(pmc_boot0);
            pMcHalInfo->Architecture     = MC_GET_ARCHITECTURE(pmc_boot0);
            pMcHalInfo->Manufacturer     = NV_PMC_BOOT_0_MANUFACTURER_NVIDIA;
            break;
        }
        case MC_CONTROL_INIT:
        {
            U032 PCI_NV1;
            U032 PCI_NV19;
            PDACHALINFO pDacHalInfo = pHalHwInfo->pDacHalInfo;
            PFIFOHALINFO pFifoHalInfo = pHalHwInfo->pFifoHalInfo;
            PGRHALINFO pGrHalInfo = pHalHwInfo->pGrHalInfo;

            //
            // Disable bus mastering before manipulating the NV_PMC_ENABLE register;
            // the dev_master.ref spec indicates that bus mastering must be disabled
            // before clearing PPMI, but experience has show that touching NV_PMC_ENABLE
            // AT ALL with bus mastering on can be dangerous.  This manifests itself on
            // linux with various systems (most notably Dell laptops with GeForce2Go)
            // where fiddling with the NV_PMC_ENABLE register causes the
            // NV_PFIFO_CACHE1_DMA_PUSH's busy bit to get set which in turn causes
            // system hangs when we mistakenly wait for the fifo to go idle.
            // Fixes bug 34247. - aritger
            // Also fixes bug 34388
            //
            PCI_NV1 = REG_RD32(NV_PBUS_PCI_NV_1);
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _BUS_MASTER, _DISABLED);
	    
            // NV11 Manual says disable both PCI and AGP Bus Mastering
            // This is not needed for bug 34388 fix but may prevent problems in the future
            PCI_NV19 = REG_RD32(NV_PBUS_PCI_NV_19);
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_AGP_ENABLE, _OFF);

            // JJV - Reset toggle the Master Control for the fifo and the Bus Mastering logic.
            // This is a fix for a DELL restart hang.19990409-125432.

           {
                U032 x;

                x=REG_RD32(NV_PMC_ENABLE);
                x&=~(DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED) | DRF_DEF(_PMC, _ENABLE, _PPMI, _ENABLED));
                REG_WR32(NV_PMC_ENABLE,x); 
                // This is 100 millisecond delay since this is causing a problem
                osDelay(100);
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
            // restore the previous value of NV_PBUS_PCI_NV_19/NV_PBUS_PCI_NV_1; this will
            // re-enable bus mastering if it was enabled above
            //
            REG_WR32(NV_PBUS_PCI_NV_19, PCI_NV19);
            REG_WR32(NV_PBUS_PCI_NV_1, PCI_NV1);

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

            // Make sure to disable the clock slowdown circuit
            if (IsNV11_NV10(pMcHalInfo))
            {
                IsPllSlowed = REG_RD_DRF(_PBUS, _DEBUG_1, _CORE_SLOWDWN);
                FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, 0);
            }

            M = REG_RD_DRF(_PRAMDAC, _NVPLL_COEFF, _MDIV);
            N = REG_RD_DRF(_PRAMDAC, _NVPLL_COEFF, _NDIV);
            P = REG_RD_DRF(_PRAMDAC, _NVPLL_COEFF, _PDIV);
            O = 1;
    
            // Restore the clock slowdown circuit
            if (IsNV11_NV10(pMcHalInfo))
                FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, IsPllSlowed);

            pDacHalInfo->NVPllM = M;
            pDacHalInfo->NVPllN = N;
            pDacHalInfo->NVPllO = O;
            pDacHalInfo->NVPllP = P;
            pDacHalInfo->NVClk  = ( M != 0 ) ? (N * crystal / (1 << P) / M) : 0;

            //
            // Get chip revision information.
            //
            // We need to be careful here that the values we pull from the
            // hardware line up with the constants defined in chipid.h, or
            // else we need to convert them accordingly.
            //
            // On NV10 and family, they do so we're OK.
            //
            pmc_boot0 = REG_RD32(NV_PMC_BOOT_0);

            pMcHalInfo->Subrevision      = 0;
            pMcHalInfo->MaskRevision     =  pmc_boot0 &        0xFF;
            pMcHalInfo->Revision         = (pmc_boot0 &     0xF0000) >> 16; 
            pMcHalInfo->Implementation   = (pmc_boot0 &    0xF00000) >> 20;
            pMcHalInfo->Architecture     = (pmc_boot0 &  0xFFF00000) >> 20;
            pMcHalInfo->Manufacturer     = NV_PMC_BOOT_0_MANUFACTURER_NVIDIA;

            // take the Implementation out of the Architecture field
            pMcHalInfo->Architecture     ^= pMcHalInfo->Implementation;

            //
            // Initialize number of crtcs.
            //
            // NOTE: This code needs to be flushed out further to consider the board
            // layout and the number of physical display connectors.  An NV11 device on
            // a board with only a VGA connector should only report one CRTC available.
            //
            if (IsNV11_NV10(pMcHalInfo))
                pDacHalInfo->NumCrtcs = 2;
            else
                pDacHalInfo->NumCrtcs = 1;

            //Save away total number of BAR addresses for the chip
            //as well as what those addresses are.
            pMcHalInfo->TotalPciBars = 2;
            pMcHalInfo->PciBars[0] = REG_RD32(NV_PBUS_PCI_NV_4);
            pMcHalInfo->PciBars[1] = REG_RD32(NV_PBUS_PCI_NV_5);

            //
            // Because of a HW bug in NV10/NV15 (fixed in NV11) regarding Celsius SetContextDmaVertex when
            // passed the NULL object, we need to guarantee the first DWORD of the BIOS image looks like a
            // valid context DMA.
            // 
            // Lucky for us normally what lives at instance addr 0 is the first part of the BIOS VGA
            // signature (0xAA55), which appears to have the NV_DMA_PAGE_ENTRY_LINEAR set. But on IKOS and
            // possibly non-IBM Compatible BIOS's, we may not be so lucky. So, we'll check what's there
            // and if necessary, OR-in the NV_DMA_PAGE_TABLE_PRESENT bit (gulp).
            //
            if (IsNV10_NV10(pMcHalInfo) || IsNV15_NV10(pMcHalInfo))
            {
                U032 inst_addr0 = REG_RD32(DEVICE_BASE(NV_PRAMIN));

                if ((DRF_VAL(_DMA, _PAGE, _TABLE, inst_addr0) == NV_DMA_PAGE_TABLE_NOT_PRESENT) &&
                    (DRF_VAL(_DMA, _PAGE, _ENTRY, inst_addr0) == NV_DMA_PAGE_ENTRY_NOT_LINEAR))
                {
#if !defined(IKOS) && !defined(MACOS)
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Non-VGA BIOS image? Setting the PAGE_TABLE_PRESENT bit");
                    DBG_BREAKPOINT();
#endif
                    REG_WR32(DEVICE_BASE(NV_PRAMIN), (inst_addr0 | DRF_DEF(_DMA, _PAGE, _TABLE, _PRESENT)));
                }
            }

            //
            // Set fifo count/context size here so the values are
            // available for the rest of the STATE_INIT sequence.
            //
            pFifoHalInfo->Count = NUM_FIFOS_NV10;
            pFifoHalInfo->ContextSize = FC_SIZE_NV10;

            //
            // Determine what graphics capabilities we have.
            //
            // For future chips, we default to basic capabilities which is safest and most
            // reasonable in the compatibility versions of the drivers. For the performance
            // drivers that ship with the chip, we'll add the correct detection in that version.
            //
            if (IsNV10_NV10(pMcHalInfo))
            {
                //
                // NV10 relies on the PCI ID to match GL functionality
                //
                if ((pMcHalInfo->PCIDeviceID >> 16) == 0x103)
                    pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_AKILL_USERCLIP     // fast user clip planes
                                                | NVHAL_GRAPHICS_CAPS_AA_LINES          // AA lines
                                                | NVHAL_GRAPHICS_CAPS_AA_POLYS          // AA polys
                                                | NVHAL_GRAPHICS_CAPS_2SIDED_LIGHTING   // two-sided lighting
                                                | NVHAL_GRAPHICS_CAPS_QUADRO_GENERIC    // anything not yet covered
                                                | NVHAL_GRAPHICS_CAPS_UBB               // unified backbuffers
                                                | NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                | 0x8);                                 // max clips = 8
                //
                // also need to check for pseudo-Quadro boards
                //
                //  NV10 Elsa Quadro 0x0C481048
                //  Generic NV vendor overrides where SubsystemDeviceID[15] = 1
                //
                else if ((pMcHalInfo->PCISubDeviceID == 0x0C481048) ||
                         ((pMcHalInfo->PCISubDeviceID & 0x8000FFFF) == 0x800010DE))

                          pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_UBB               // unified backbuffers
                                                      | NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                      | 0x1);                                 // max clips = 1

                else 
                    pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                | 0x1);                                 // max clips = 1
            }
            else if (IsNV15_NV10(pMcHalInfo))
            {
                //
                // NV15 relies on the PCI ID to match GL functionality
                //
                if ((pMcHalInfo->PCIDeviceID >> 16) == 0x153)
                    pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_LOGIC_OPS          // logic ops
                                                | NVHAL_GRAPHICS_CAPS_AKILL_USERCLIP    // fast user clip planes
                                                | NVHAL_GRAPHICS_CAPS_AA_LINES          // AA lines
                                                | NVHAL_GRAPHICS_CAPS_AA_POLYS          // AA polys
                                                | NVHAL_GRAPHICS_CAPS_2SIDED_LIGHTING   // two-sided lighting
                                                | NVHAL_GRAPHICS_CAPS_QUADRO_GENERIC    // anything not yet covered
                                                | NVHAL_GRAPHICS_CAPS_UBB               // unified backbuffers
                                                | NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                | 0x8);                                 // max clips = 8
                
                //
                // also need to check for pseudo-Quadro boards
                //
                //  NV15 Dell 0x002E10DE
                //  Generic NV vendor overrides where SubsystemDeviceID[15] = 1
                //
                else if ((pMcHalInfo->PCISubDeviceID == 0x002E10DE) ||
                         ((pMcHalInfo->PCISubDeviceID & 0x8000FFFF) == 0x800010DE))
                
                    pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_UBB               // unified backbuffers
                                                 | NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                 | 0x1);                                 // max clips = 1

                else 
                    pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                | 0x1);                                 // max clips = 1
                
                // needs NV15 aliased line fix
                if (IsNV15MaskRevA03_NV10(pMcHalInfo) || IsNV15MaskRevA02_NV10(pMcHalInfo))
                {
                    pGrHalInfo->Capabilities |= NVHAL_GRAPHICS_CAPS_NV15_ALINES;       // aliased line fix
                }
            }
            else if (IsNV1A_NV10(pMcHalInfo) || IsNV11_NV10(pMcHalInfo))
            {
                //
                // NV11 relies on the PCI ID to match GL functionality
                // (note: no AA_LINES or AA_POLYS for the NV11 GL part)
                // (note2: no UBB for NV11-mobile part (0x0112)
                //
                if ((pMcHalInfo->PCIDeviceID >> 16) == 0x113)
                    pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_LOGIC_OPS          // logic ops
                                                | NVHAL_GRAPHICS_CAPS_AKILL_USERCLIP    // fast user clip planes
                                                | NVHAL_GRAPHICS_CAPS_2SIDED_LIGHTING   // two-sided lighting
                                                | NVHAL_GRAPHICS_CAPS_QUADRO_GENERIC    // anything not yet covered
                                                | NVHAL_GRAPHICS_CAPS_UBB               // unified backbuffers
                                                | NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                | 0x8);                                 // max clips = 8

                //
                // also need to check for pseudo-Quadro boards
                //
                //  NV11 IBM 0x004810DE
                //  Generic NV vendor overrides where SubsystemDeviceID[15] = 1
                //
                else if ((pMcHalInfo->PCISubDeviceID == 0x004810DE) ||
                         ((pMcHalInfo->PCISubDeviceID & 0x8000FFFF) == 0x800010DE))

                    pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_UBB               // unified backbuffers
                                                | NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                | 0x1);                                 // max clips = 1

                else if ((pMcHalInfo->PCIDeviceID >> 16) == 0x112)
                    pGrHalInfo->Capabilities = ( 0x1 );                                 // max clips = 1
                else 
                    pGrHalInfo->Capabilities = ( NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE // needs bug workaround
                                                | 0x1);                                 // max clips = 1
            }
            else
            {
                //
                // Should never get here, but just in case
                //
                DBG_BREAKPOINT();
                pGrHalInfo->Capabilities = 0x1;     // no AA lines/polys, max clips = 1
            }
            
            //
            // All nv1x parts support a 2:1 anisotropic filtering
            //
            pGrHalInfo->Capabilities |= NVHAL_GRAPHICS_CAPS_ANISOTROPIC;
	    
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
nvHalMcPower_NV10(VOID *arg)
{
    PMCPOWERARG_000 pMcPowerArg = (PMCPOWERARG_000)arg;
    PHALHWINFO pHalHwInfo = pMcPowerArg->pHalHwInfo;
    PMCHALINFO_NV10 pMcHalPvtInfo;
    PMCHALPOWERINFO_NV10 pMcHalPowerInfo;
    PDACHALINFO pDacHalInfo; 
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalMcPower_NV10\r\n");

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pMcPowerArg->id != MC_POWER_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pMcPowerArg->pHalHwInfo;
    pMcHalPvtInfo = (PMCHALINFO_NV10)pHalHwInfo->pMcHalPvtInfo;
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
                {
                    U032 data32;
                    PFIFOHALINFO_NV10 pFifoHalPvtInfo;
                    PFBHALINFO_NV10 pFbHalPvtInfo;

                    pFifoHalPvtInfo = (PFIFOHALINFO_NV10)pMcPowerArg->pHalHwInfo->pFifoHalPvtInfo;
                    pFbHalPvtInfo = (PFBHALINFO_NV10)pMcPowerArg->pHalHwInfo->pFbHalPvtInfo;

                    // Reset the AGP and bus mastering logic (else we get dma pusher parse errors when DMA'ing across AGP)
                    data32 = REG_RD32(NV_PBUS_PCI_NV_19);
                    REG_WR32(NV_PBUS_PCI_NV_19, 0);         // Clear all of AGP Command. It's important to reset DATA_RATE bits.
                    REG_WR32(NV_PBUS_PCI_NV_19, data32);
                    
                    FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _BUS_MASTER, _DISABLED);
                    FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _BUS_MASTER, _ENABLED);

                    //
                    // This section is for mobile only!
                    //
                    // We don't expose the MobileOperation switch to the HAL
                    // at the moment, so we rely on the RM to never use this
                    // state (MC_POWER_LEVEL_2) except when MobileOperation
                    // is defined.
                    //
                    // Refer to mcstate.c for the MobileOperation check.
                    //
                    //
                    // Restore fifo mode register.
                    //
                    REG_WR32(NV_PFIFO_MODE, pFifoHalPvtInfo->Mode);
                    
                    // Set to unused channel to force context switch
                    // when fifo is accessed.
                    //
                    REG_WR32(NV_PFIFO_CACHE0_PUSH1, NUM_FIFOS_NV10-1);
                    REG_WR32(NV_PFIFO_CACHE1_PUSH1, NUM_FIFOS_NV10-1);
                    
                    REG_WR32(NV_PFIFO_RAMHT, pMcHalPowerInfo->PfifoRamHt);
                    REG_WR32(NV_PFIFO_RAMFC, pMcHalPowerInfo->PfifoRamFc);
                    REG_WR32(NV_PFIFO_RAMRO, pMcHalPowerInfo->PfifoRamRo);
                    
                    data32 = REG_RD32(NV_PFB_SELF_REF);
                    data32 &= ~(SF_DEF(_PFB, _SELF_REF_CMD, _ENABLED));
                    REG_WR32(NV_PFB_SELF_REF, data32);

                    //
                    // We may have freed a tiled region when we were
                    // unable to write to the hardware...sync up our
                    // software state with the hardware now.
                    //
                    for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
                    {
                        if ((pFbHalPvtInfo->usedTiles & MEM_TYPE_TILE_RANGE_SHIFT(i)) == 0)
                        {
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
                                    HAL_GR_IDLE(pHalHwInfo);

                                    // invalidate the range
                                    REG_WR32(NV_PFB_TILE(i), NV_PFB_TILE_REGION_INVALID);
                                    REG_WR32(NV_PGRAPH_TILE(i), NV_PFB_TILE_REGION_INVALID);    

                                } while (REG_RD32(NV_PFB_TILE(i)) != REG_RD32(NV_PGRAPH_TILE(i)));
                            }
                        }
                    }
                    break;
                }
                case MC_POWER_LEVEL_4:
                    // must set FBI differential
                    REG_WR32(NV_PBUS_DEBUG_1, pMcHalPowerInfo->PbusDebug1);
                    // refresh
                    REG_WR32(NV_PFB_TIMING2, pMcHalPowerInfo->PfbTiming2);
                    REG_WR32(NV_PFB_CFG, pMcHalPowerInfo->PfbBoot0);
                    REG_WR32(NV_PFB_CSTATUS, pMcHalPowerInfo->PfbConfig0);
                    REG_WR32(NV_PFB_REFCTRL, pMcHalPowerInfo->PfbConfig1);
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
                    // 
                    // These two states are more or less identical (at least
                    // as far as power up is concerned).
                    //
                    PFIFOHALINFO_NV10 pFifoHalPvtInfo;
                    NV10_TILE_VALUES *tvp;

                    pFifoHalPvtInfo = (PFIFOHALINFO_NV10)pMcPowerArg->pHalHwInfo->pFifoHalPvtInfo;

                    //
                    // Restore fb regs.
                    //

                    // must set FBI differential
                    REG_WR32(NV_PBUS_DEBUG_1, pMcHalPowerInfo->PbusDebug1);
                    // refresh
                    REG_WR32(NV_PFB_TIMING2, pMcHalPowerInfo->PfbTiming2);
                    REG_WR32(NV_PFB_CFG, pMcHalPowerInfo->PfbBoot0);
                    REG_WR32(NV_PFB_CSTATUS, pMcHalPowerInfo->PfbConfig0);
                    REG_WR32(NV_PFB_REFCTRL, pMcHalPowerInfo->PfbConfig1);
                    REG_WR32(NV_PFB_MRS, pMcHalPowerInfo->PfbMrs);
                    //REG_WR32(NV_PFB_EMRS, pMcHalPowerInfo->PfbEmrs);
                    REG_WR32(NV_PFB_REF, pMcHalPowerInfo->PfbRef);

                    REG_WR32(NV_PFIFO_RAMHT, pMcHalPowerInfo->PfifoRamHt);
                    REG_WR32(NV_PFIFO_RAMFC, pMcHalPowerInfo->PfifoRamFc);
                    REG_WR32(NV_PFIFO_RAMRO, pMcHalPowerInfo->PfifoRamRo);

                    //
                    // Make sure write protection is disabled.
                    //
                    FLD_WR_DRF_DEF(_PFB, _NVM, _MODE, _DISABLE);

                    //
                    // Reset the MRS (Memory PLL)
                    //
                    FLD_WR_DRF_DEF(_PFB, _MRS, _DLL, _RESET);
                    FLD_WR_DRF_DEF(_PFB, _MRS, _DLL, _NORMAL);
                    
                    //
                    // Restore tiling registers.
                    //
                    tvp = (NV10_TILE_VALUES *)&pMcHalPowerInfo->TilingValues[0];

                    //
                    // Check for integrated graphics -- we need to setup tiling through the north bridge
                    //
                    if( IsNV1A_NV10(pHalHwInfo->pMcHalInfo) )
                    {
                        //North bridge integrated graphics has a different tile setup
                        U016	wNBVendorID;
                        U016	wNBDeviceID;
                        VOID    *uNBHandle = 0;
                        U032    uOffset;
                        U032    uSize;
                        U032    uPitch;
                        U032    uValid;

                        uNBHandle = osPciInitHandle(0, 0, 2, &wNBVendorID, &wNBDeviceID);
                        if( !uNBHandle )
                        {
	                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unable to touch integrated graphics tiling\n");
	                    DBG_BREAKPOINT();
	                    return (RM_ERROR);
                        }

                        for (i = 0; i < NV_PFB_TILE__SIZE_1; i++, tvp++)
                        {
                            uOffset = tvp->tile & DRF_NUM(_PFB,_TILE,_ADR,0xffffffff);
                            uPitch = tvp->tsize;
                            uSize = tvp->tlimit - uOffset + 1;
                            uValid = DRF_VAL(_PGRAPH, _TILE, _REGION, tvp->tile);

          	            osPciWriteDword(uNBHandle, CR_XL_TBASE(i)  & 0xFF, WriteNBTBASE(uPitch, uOffset, uSize, uValid));
	                    osPciWriteDword(uNBHandle, CR_XL_TLIMIT(i) & 0xFF, WriteNBTLIMIT(uPitch, uOffset, uSize, uValid));

                            REG_WR32(NV_PGRAPH_TLIMIT(i),   tvp->tlimit);
                            REG_WR32(NV_PGRAPH_TSIZE(i),    tvp->tsize);
                            REG_WR32(NV_PGRAPH_TILE(i),     tvp->tile);
                        }
                    }
                    else
                    {
                        for (i = 0; i < NV_PFB_TILE__SIZE_1; i++, tvp++)
                        {
                            REG_WR32(NV_PFB_TLIMIT(i),      tvp->tlimit);
                            REG_WR32(NV_PFB_TSIZE(i),       tvp->tsize);
                            REG_WR32(NV_PFB_TILE(i),        tvp->tile);

                            REG_WR32(NV_PGRAPH_TLIMIT(i),   tvp->tlimit);
                            REG_WR32(NV_PGRAPH_TSIZE(i),    tvp->tsize);
                            REG_WR32(NV_PGRAPH_TILE(i),     tvp->tile);
                        }
                    }
                    
                    //
                    // Restore fifo mode register.
                    //
                    REG_WR32(NV_PFIFO_MODE, pFifoHalPvtInfo->Mode);

                    //
                    // Set to unused channel to force context switch
                    // when fifo is accessed.
                    //
                    REG_WR32(NV_PFIFO_CACHE0_PUSH1, NUM_FIFOS_NV10-1);
                    REG_WR32(NV_PFIFO_CACHE1_PUSH1, NUM_FIFOS_NV10-1);

                    //
                    // Turn up clock.
                    //
                    FLD_WR_DRF_DEF(_PRAMDAC, _PLL_SETUP_CONTROL, _PWRDWN, _ON);

                    //
                    // restore CRTC config for each head (may include endian-ness)
                    // Also restore range regs for each head.
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
                    
                    //REG_WR32(NV_PVIDEO_STOP,pMcHalPowerInfo->PvideoStop);
                    REG_WR32(NV_PVIDEO_BASE(0),pMcHalPowerInfo->PvideoBase0);
                    REG_WR32(NV_PVIDEO_BASE(1),pMcHalPowerInfo->PvideoBase1);
                    REG_WR32(NV_PVIDEO_LIMIT(0),pMcHalPowerInfo->PvideoLimit0);
                    REG_WR32(NV_PVIDEO_LIMIT(1),pMcHalPowerInfo->PvideoLimit1);
                    REG_WR32(NV_PVIDEO_LUMINANCE(0),pMcHalPowerInfo->PvideoLuminance0);
                    REG_WR32(NV_PVIDEO_LUMINANCE(1),pMcHalPowerInfo->PvideoLuminance1);
                    REG_WR32(NV_PVIDEO_CHROMINANCE(0),pMcHalPowerInfo->PvideoChrominance0);
                    REG_WR32(NV_PVIDEO_CHROMINANCE(1),pMcHalPowerInfo->PvideoChrominance1);
                    REG_WR32(NV_PVIDEO_OFFSET(0),pMcHalPowerInfo->PvideoOffset0);
                    REG_WR32(NV_PVIDEO_OFFSET(1),pMcHalPowerInfo->PvideoOffset1);
                    REG_WR32(NV_PVIDEO_SIZE_IN(0),pMcHalPowerInfo->PvideoSizeIn0);
                    REG_WR32(NV_PVIDEO_SIZE_IN(1),pMcHalPowerInfo->PvideoSizeIn1);
                    REG_WR32(NV_PVIDEO_POINT_IN(0),pMcHalPowerInfo->PvideoPointIn0);
                    REG_WR32(NV_PVIDEO_POINT_IN(1),pMcHalPowerInfo->PvideoPointIn1);
                    REG_WR32(NV_PVIDEO_DS_DX(0),pMcHalPowerInfo->PvideoDsDx0);
                    REG_WR32(NV_PVIDEO_DS_DX(1),pMcHalPowerInfo->PvideoDsDx1);
                    REG_WR32(NV_PVIDEO_DT_DY(0),pMcHalPowerInfo->PvideoDtDy0);
                    REG_WR32(NV_PVIDEO_DT_DY(1),pMcHalPowerInfo->PvideoDtDy1);
                    REG_WR32(NV_PVIDEO_POINT_OUT(0),pMcHalPowerInfo->PvideoPointOut0);
                    REG_WR32(NV_PVIDEO_POINT_OUT(1),pMcHalPowerInfo->PvideoPointOut1);
                    REG_WR32(NV_PVIDEO_SIZE_OUT(0),pMcHalPowerInfo->PvideoSizeOut0);
                    REG_WR32(NV_PVIDEO_SIZE_OUT(1),pMcHalPowerInfo->PvideoSizeOut1);
                    REG_WR32(NV_PVIDEO_FORMAT(0),pMcHalPowerInfo->PvideoFormat0);
                    REG_WR32(NV_PVIDEO_FORMAT(1),pMcHalPowerInfo->PvideoFormat1);
                    REG_WR32(NV_PVIDEO_COLOR_KEY,pMcHalPowerInfo->PvideoColorKey);
                    REG_WR32(NV_PVIDEO_INTR_EN,pMcHalPowerInfo->PvideoIntrEn);

                    //
                    // Restore SetOffsetRange registers.
                    //
                    //REG_WR32(NV_PCRTC_RASTER_START, pMcHalPowerInfo->PcrtcRasterStart);
                    //REG_WR32(NV_PCRTC_RASTER_STOP, pMcHalPowerInfo->PcrtcRasterStop);
                    
                    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO," ");
                    
                    for(i=0;i<pDacHalInfo->NumCrtcs;i++)
                    {
                        REG_WR32(NV_PCRTC_RASTER_START+i*0x2000,pMcHalPowerInfo->PcrtcRasterStart[i]);
                        REG_WR32(NV_PCRTC_RASTER_STOP+i*0x2000,pMcHalPowerInfo->PcrtcRasterStop[i]);
                        
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"Raster on head - ",i);
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"Raster Start: ",pMcHalPowerInfo->PcrtcRasterStart[i]);
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"Raster Stop : ",pMcHalPowerInfo->PcrtcRasterStop[i]);
                    }
                    
                    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO," ");
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
            //
            // This section is for mobile only!
            //
            // We don't expose the MobileOperation switch to the HAL
            // at the moment, so we rely on the RM to never use this
            // state (MC_POWER_LEVEL_2) except when MobileOperation
            // is defined.
            //
            // Refer to mcstate.c for the MobileOperation check.
            //
            pMcHalPowerInfo->PfifoRamHt = REG_RD32(NV_PFIFO_RAMHT);
            pMcHalPowerInfo->PfifoRamFc = REG_RD32(NV_PFIFO_RAMFC);
            pMcHalPowerInfo->PfifoRamRo = REG_RD32(NV_PFIFO_RAMRO);
            
            FLD_WR_DRF_DEF(_PFB, _SELF_REF, _CMD, _ENABLED);
            break;
        case MC_POWER_LEVEL_4:
            pMcHalPowerInfo->PfbTiming2 = REG_RD32(NV_PFB_TIMING2);     // refresh
            FLD_WR_DRF_NUM(_PFB, _TIMING2, _REFRESH, 0xFFFFFFFF);       // max refresh
            // Chip has been init'd with the BIOS init tables, unless the BIOS
            // has an incompatible structure and the table was not found, in which case
            // we used a default table, which may not have the correct RAM config, so
            // we save away the current config for restoration above.
            pMcHalPowerInfo->PfbBoot0 = REG_RD32(NV_PFB_CFG);
            pMcHalPowerInfo->PfbConfig0 = REG_RD32(NV_PFB_CSTATUS);
            pMcHalPowerInfo->PfbConfig1 = REG_RD32(NV_PFB_REFCTRL);
            pMcHalPowerInfo->PfbMrs = REG_RD32(NV_PFB_MRS);
            pMcHalPowerInfo->PfbEmrs = REG_RD32(NV_PFB_EMRS);
            pMcHalPowerInfo->PfbRef     = REG_RD32(NV_PFB_REF);
            // RAM config - FBI differential
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
            break;
        case MC_POWER_LEVEL_3:
        case MC_POWER_LEVEL_7:
        {
            NV10_TILE_VALUES *tvp;

            //
            // Save tiling value.
            //
            tvp = (NV10_TILE_VALUES *)&pMcHalPowerInfo->TilingValues[0];

            //
            // Check for integrated graphics -- we need to setup tiling through the north bridge
            //
            if( IsNV1A_NV10(pHalHwInfo->pMcHalInfo) )
            {
                //North bridge integrated graphics has a different tile setup
                for (i = 0; i < NV_PFB_TILE__SIZE_1; i++, tvp++)
                {
                    tvp->tlimit = REG_RD32(NV_PGRAPH_TLIMIT(i));
                    tvp->tsize  = REG_RD32(NV_PGRAPH_TSIZE(i));
                    tvp->tile   = REG_RD32(NV_PGRAPH_TILE(i));
                }
            }
            else
            {
                for (i = 0; i < NV_PFB_TILE__SIZE_1; i++, tvp++)
                {
                    tvp->tlimit = REG_RD32(NV_PFB_TLIMIT(i));
                    tvp->tsize  = REG_RD32(NV_PFB_TSIZE(i));
                    tvp->tile   = REG_RD32(NV_PFB_TILE(i));
                }
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
           
            for(i=0;i<pDacHalInfo->NumCrtcs;i++)
            {
                pMcHalPowerInfo->PcrtcRasterStart[i]=REG_RD32(NV_PCRTC_RASTER_START+i*0x2000);
                pMcHalPowerInfo->PcrtcRasterStop[i]=REG_RD32(NV_PCRTC_RASTER_STOP+i*0x2000);
            }
                        
            //save interrupt allowance state for later restoration
            pMcHalPowerInfo->PvideoIntrEn=REG_RD32(NV_PVIDEO_INTR_EN);
                        
            //save stateful video registers
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
			
            pMcHalPowerInfo->PfbTiming2 = REG_RD32(NV_PFB_TIMING2);     // refresh

            if (pMcPowerArg->newLevel == MC_POWER_LEVEL_3)
                FLD_WR_DRF_NUM(_PFB, _TIMING2, _REFRESH, 0xFFFFFFFF);       // max refresh

            // Chip has been init'd with the BIOS init tables, unless the BIOS
            // has an incompatible structure and the table was not found, in which case
            // we used a default table, which may not have the correct RAM config, so
            // we save away the current config for restoration above.
            pMcHalPowerInfo->PfbBoot0 = REG_RD32(NV_PFB_CFG);
            pMcHalPowerInfo->PfbConfig0 = REG_RD32(NV_PFB_CSTATUS);
            pMcHalPowerInfo->PfbConfig1 = REG_RD32(NV_PFB_REFCTRL);
            pMcHalPowerInfo->PfbMrs = REG_RD32(NV_PFB_MRS);
            pMcHalPowerInfo->PfbEmrs = REG_RD32(NV_PFB_EMRS);
            pMcHalPowerInfo->PfbRef     = REG_RD32(NV_PFB_REF);
            pMcHalPowerInfo->PfifoRamHt = REG_RD32(NV_PFIFO_RAMHT);
            pMcHalPowerInfo->PfifoRamFc = REG_RD32(NV_PFIFO_RAMFC);
            pMcHalPowerInfo->PfifoRamRo = REG_RD32(NV_PFIFO_RAMRO);
            // RAM config - FBI differential
            pMcHalPowerInfo->PbusDebug1 = REG_RD32(NV_PBUS_DEBUG_1);
            //
            // CRTC config (may include endian-ness)
            //
            // Also, save SetOffsetRange regs.
            //
            for (i=0; i< pDacHalInfo->NumCrtcs; i++)
            {
                pMcHalPowerInfo->SaveCrtcConfig[i] = REG_RD32(NV_PCRTC_CONFIG + pDacHalInfo->CrtcInfo[i].RegOffset); 

                pMcHalPowerInfo->PcrtcRasterStart[i] = REG_RD32(NV_PCRTC_RASTER_START + pDacHalInfo->CrtcInfo[i].RegOffset);
                pMcHalPowerInfo->PcrtcRasterStop[i] = REG_RD32(NV_PCRTC_RASTER_STOP + pDacHalInfo->CrtcInfo[i].RegOffset);
            }

            //
            // Reset the MRS (Memory PLL)
            //
            FLD_WR_DRF_DEF(_PFB, _MRS, _DLL, _RESET);
            FLD_WR_DRF_DEF(_PFB, _MRS, _DLL, _NORMAL);

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
                FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V1SOURCE, _DEFAULT);
                FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V2SOURCE, _DEFAULT);
            }
                //FLD_WR_DRF_DEF(_PRAMDAC,_PLL_SETUP_CONTROL, _PWRDWN, _VPLL);
#endif
            break;
        }
        default:
            return RM_ERR_BAD_ARGUMENT;
    }

    return RM_OK;
}

BOOL IsNV10_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV10));
}

BOOL IsNV10MaskRevA02orBetter_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    // Is this NV10 A02 or better.  Note that "better" also includes all 
    // NV1X parts above NV10.
    return ((pMcHalInfo->MaskRevision >= MC_MASK_REVISION_A2) ||
            (pMcHalInfo->Implementation > MC_IMPLEMENTATION_NV10));
}

BOOL IsNV10MaskRevA03orBetter_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    // Is this NV10 A03 or better.  Note that "better" also includes all 
    // NV1X parts above NV10.
    return ((pMcHalInfo->MaskRevision >= MC_MASK_REVISION_A3) ||
            (pMcHalInfo->Implementation > MC_IMPLEMENTATION_NV10));
}

BOOL IsNV15_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    // Is this NV15?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV15));
}

#ifdef _WIN32
#pragma warning(disable: 4296)
#endif

BOOL IsNV10orBetter_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    // Is this NV10 or better?
    return ((pMcHalInfo->Architecture >= MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV10));
}

#ifdef _WIN32
#pragma warning(default: 4296)
#endif

BOOL IsNV15orBetter_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    // Is this NV15 or better?
    return ((pMcHalInfo->Architecture >= MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV11));
}

BOOL IsNV11orBetter_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    // Is this NV11 or better?
    return (((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
             (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV11) &&
             (pMcHalInfo->Implementation != MC_IMPLEMENTATION_NV15)) ||
             (pMcHalInfo->Architecture >= MC_ARCHITECTURE_NV20));
}

BOOL IsNV11_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    // Is this NV11?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV11));
}

BOOL IsNV1A_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif
    // Is this NV1A?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV1A));
}

BOOL IsNV15MaskRevA02_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif

    // Is this NV15 Mask Revision A02?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV15) &&
            (pMcHalInfo->MaskRevision == MC_MASK_REVISION_A2));
}

BOOL IsNV15MaskRevA03_NV10(PMCHALINFO pMcHalInfo)
{
#ifdef DEBUG
    if (!pMcHalInfo->Architecture)
        return (RM_ERROR);
        //DBG_BREAKPOINT();   // mask revision hasn't been set yet
#endif

    // Is this NV15 Mask Revision A03?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV15) &&
            (pMcHalInfo->MaskRevision == MC_MASK_REVISION_A3));
}

