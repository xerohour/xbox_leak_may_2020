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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/******************************************************************************
*
*   File: mcpower.c
*
*   Description:
*       This file contains the resource manager power management API.
*
******************************************************************************/

#include <nvrm.h>
#include <devinit.h>
#include "nvhw.h"

#define KERNEL_PM_STATE_CHANGE // Do in kernel state changes which used to be done in OS-specific section

// this define is in the chip-specific headers; it seems like it ought to go in nv_ref.h.
#define NV_PRMIO_RMA_PTR                                 0x00007104 /* RW-4R */

//  Transition between power states
//  States are:
//  Level 0 - full on
//  Level 1 - slowed clocks
//  Level 2 - slowed clocks, mobile style (not all devices support)
//  Level 3 - very slow clocks, state saved (regs & instance memory)
RM_STATUS mcPowerState
(
    PHWINFO pDev, 
    U032    state
)
{
    U032 i, data, data32, ChipID, status;
    U016 data16;
    U008 lock, retry = 0;
    U032 *fb_sv, Head=0;
    BOOL   biosUsedToInit;

    switch (state) 
    {
        case MC_POWER_LEVEL_0:          // power up
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 0");
            switch (pDev->Power.State)  // procedure depends on what state we are transitioning from
            {
                case MC_POWER_LEVEL_1:
                    // turn clocks up
                    FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _ON);
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);
                    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                    REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
                    
                    nvHalMcPower(pDev, MC_POWER_LEVEL_0, pDev->Power.State);
                    
                    pDev->DeviceDisabled = FALSE;     // let ISR handle interrupts
                    break;
                
                case MC_POWER_LEVEL_2:
                    
                    data32 = (pDev->Power.PbusDebug1 & ~(SF_DEF(_PBUS, _DEBUG_1_IDDQ, _ONE)));
                    REG_WR32(NV_PBUS_DEBUG_1, data32);

                    osDelay(2); // wait 2 ms

                    data32 &= ~(SF_DEF(_PBUS, _DEBUG_1_PLL_STOPCLK, _ENABLE));
                    REG_WR32(NV_PBUS_DEBUG_1, data32);

                    nvHalMcPower(pDev, MC_POWER_LEVEL_0, pDev->Power.State);
                    
                    // now we are powered up
                    
#if 0
                    // Check for active displays and enable them
                    data32 = ((PVIDEO_LUT_CURSOR_DAC_OBJECT)(pDev->Dac.CrtcInfo[0].pVidLutCurDac))->DisplayType;
                    if ((data32 == DISPLAY_TYPE_MONITOR) || (data32 == DISPLAY_TYPE_FLAT_PANEL) || (data32 == DISPLAY_TYPE_TV))
                    {
                        dacEnableDac(pDev, 0);
                    }
                    data32 = ((PVIDEO_LUT_CURSOR_DAC_OBJECT)(pDev->Dac.CrtcInfo[1].pVidLutCurDac))->DisplayType;
                    if ((data32 == DISPLAY_TYPE_MONITOR) || (data32 == DISPLAY_TYPE_FLAT_PANEL) || (data32 == DISPLAY_TYPE_TV))
                    {
                        dacEnableDac(pDev, 1);
                    }
#endif                    
                    // now we are powered up
                    break;
                case MC_POWER_LEVEL_3:
                    // turn off bus master before resetting master enable
                    // else we may get spurious bus master cycles 
                    data32 = REG_RD32(NV_PBUS_PCI_NV_1);
                    FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _BUS_MASTER, _DISABLED);
                    // Reset the chip, in case the HW didn't.
                    REG_WR32(NV_PMC_ENABLE, 0);
                    REG_RD32(NV_PMC_ENABLE);
                    REG_RD32(NV_PMC_ENABLE);

                    REG_WR32(NV_PMC_ENABLE, -1);
                    REG_RD32(NV_PMC_ENABLE);
                    REG_RD32(NV_PMC_ENABLE);
                    // restore bus master enable
                    REG_WR32(NV_PBUS_PCI_NV_1, data32);

                    // turn DAC on
                    REG_WR32(NV_PRAMDAC_TEST_CONTROL, 0); // make sure test modes are off!
                    FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF); // leave output off

                    // Delay for a while before initializing chip; verify strap register is reading correctly.
                    // Note: it's not just incorrect strap values which cause the problem; without the delay,
                    // the problem still occurs, even if correct strap values are used to init.
                    // (else memory is not correctly init'd. Occurs on Dell Magellan (i820 chipset). Does not occur on rel4.)
               
                    while (REG_RD32(NV_PEXTDEV_BOOT_0) != pDev->Power.strapInfo) 
                    {
                        for (i=0; i<2000; i++) 
                        {
                            REG_RD32(NV_PMC_ENABLE);
                        }
                        REG_WR32(NV_PEXTDEV_BOOT_0, pDev->Power.strapInfo);    // WRITE STRAP REG
                        retry++;
                        if (retry == D0_DELAY_RETRIES) 
                            break;
                    }
                    // POST the device using BIOS init tables
                    DevinitInitializeDevice(pDev, (PHWREG)pDev->fb_save, &biosUsedToInit);

                    // turn clocks up
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);
                    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                    REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk

                    // restore timer before attempting delay
                    REG_WR32(NV_PTIMER_NUMERATOR, pDev->Timer.Numerator);     
                    REG_WR32(NV_PTIMER_DENOMINATOR, pDev->Timer.Denominator);     
                    // Wait 10 ms to allow PLL to lock.
                    // HW guy says 1 ms should be enough.
                    // In fact, very small delay seems to do it.
                    tmrDelay(pDev, 10000000); 
                                                
                    if (pDev == NvDBPtr_Table[0]) // if primary
                        // enable VGA I/O
                        REG_WR08(NV_PRMVIO_VSE2, 0x01);
                    // the chip may come up with instance memory write protected! Make sure its not.
                    nvHalFbControl(pDev, FB_CONTROL_INSTWR_ENABLE);
                    nvHalMcPower(pDev, MC_POWER_LEVEL_0, pDev->Power.State);

                    // restore critical registers lost during power down
                    REG_WR32(NV_PMC_ENABLE, pDev->Master.Enable);
                    //REG_WR32(NV_PBUS_PCI_NV_19, pDev->Master.PciNv19);    // not needed, now
                    // that we call nvUpdateAGPConfig (and dangerous, since FW could get re-enabled
                    // without re-enabling it in the motherboard chipset)

                    // next the display driver will get the foreground switch message, which calls rmEnableHires
                    // this starts us up again

                    //LPL: the 440BX-specific hack above to restore AGP registers
                    //can almost be removed.  The call below takes care of all the
                    //details (for more chipsets) except the register $50 issue.
                    NvUpdateAGPConfig(pDev);

                    // copy from save area to framebuffer instance memory
                    fb_sv = pDev->fb_save;

                    for(i=0; i < pDev->Pram.HalInfo.TotalInstSize / 4; i++)
                    {
                         data = *fb_sv++;
                         REG_WR32(NV_PRAMIN_DATA032(i), data);
                    }            
                    osFreeMem((VOID *)pDev->fb_save);
                    pDev->fb_save = 0;
                    pDev->DeviceDisabled = FALSE;     // let ISR handle interrupts
                    break;
            } // switch (pDev->Power.State)
#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            // Unload VGA device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = FALSE;
            // Load HIRES device state
            stateNv(pDev, STATE_LOAD);
#endif            
            data16 = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A);
            WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016) (data16 & 0x3fff));     // enable syncs
            REG_WR32(NV_PMC_INTR_EN_0, 1);                 // re-enable interrupts.
            dacSetFan(pDev, 1);   // turn on fan
            
            // Set power state in PCI config reg so BIOS can read
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D0);
            
            pDev->Power.State = MC_POWER_LEVEL_0;
            break;
        // end case MC_POWER_LEVEL_0
        
        //
        // Power Down
        //
        case MC_POWER_LEVEL_1:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 1");
            
#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            // Unload HIRES device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = TRUE;
            // Load VGA device state
            stateNv(pDev, STATE_LOAD);
#endif            
            REG_WR32(NV_PMC_INTR_EN_0, 0);                      // disable all interrupts. 
            REG_WR32(NV_PMC_INTR_0, REG_RD32(NV_PMC_INTR_0));   // clear any pending ints
            pDev->DeviceDisabled = TRUE;                      // signal ISR to ignore
            dacSetFan(pDev, 0);   // turn off fan
            
            // stop fetching video mem
            data16 = ReadIndexed(NV_PRMVIO_SRX, 0x01);
            data16 |= 0x2000;   // screen off
            WriteIndexed(NV_PRMVIO_SRX, data16);
            
            // save current clock coefficients
            pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
            pDev->Power.nvpll = REG_RD32(NV_PRAMDAC_NVPLL_COEFF);   // nvclk
            pDev->Power.vpll = REG_RD32(NV_PRAMDAC_VPLL_COEFF);   // vclk
            pDev->Power.mpll = REG_RD32(NV_PRAMDAC_MPLL_COEFF);     // mclk

            // turn clocks down to save power
            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_SOURCE,_PROG_ALL);   // program all clocks
            FLD_WR_DRF_NUM(_PRAMDAC,_NVPLL_COEFF, _PDIV, 4);    // max pwr saving is not max divisor
            FLD_WR_DRF_NUM(_PRAMDAC,_NVPLL_COEFF, _NDIV, 4);    // pll lower limit around 4-10
            
            // High speed memories don't work with the following clock slow-down
            // Don't do it on NV15 Ultra or Quadro 2
            ChipID = REG_RD_DRF(_CONFIG_PCI, _NV_0, _DEVICE_ID);
            if ((ChipID != NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID2) && (ChipID != NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID3))
            {
                FLD_WR_DRF_NUM(_PRAMDAC,_MPLL_COEFF, _PDIV, 5);                 // div by 32
            }
            // Set PCLK to use the crystal
            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_VPLL_SOURCE,_XTAL);  

            nvHalMcPower(pDev, MC_POWER_LEVEL_1, pDev->Power.State);

            // power down the DAC
            FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF);
            
            // Set power state in PCI config reg so BIOS can read
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D3_HOT);
            
            pDev->Power.State = MC_POWER_LEVEL_1;
            break;
        
        case MC_POWER_LEVEL_2:
            
            // Power management for mobile chips:
            // Set RAM self-refresh mode (HAL)
            // Set flag to tell VGA BIOS to handle
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 2");

            // Blank both heads
            dacDisableDac(pDev, 0);
            dacDisableDac(pDev, 1);

            // Set power state in PCI config reg so BIOS can read
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D3_HOT);

#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            // Unload HIRES device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = TRUE;
            // Load VGA device state
            stateNv(pDev, STATE_LOAD);
#endif            
            nvHalMcPower(pDev, MC_POWER_LEVEL_2, pDev->Power.State);

            // leave the real mode access window pointing to PBUS_DEBUG_1 so that the BIOS can wake us up if called.
            REG_WR32(NV_PRMIO_RMA_PTR, NV_PBUS_DEBUG_1);    // point to PBUS_DEBUG_1 address
            lock = UnlockCRTC(pDev, Head);
            CRTC_WR(NV_CIO_CRE_RMA__INDEX, 5, Head);        // set data mode
            RestoreLock(pDev, Head, lock);

            data32 = REG_RD32(NV_PBUS_DEBUG_1);
            pDev->Power.PbusDebug1 = data32;                // save this for resume function
            FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _PLL_STOPCLK, _ENABLE);
    
            osDelay(2); // wait 2 ms

            FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _IDDQ, _ONE);
            
            pDev->Power.State = MC_POWER_LEVEL_2;
            break;
        
        case MC_POWER_LEVEL_3:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 3");
#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            // Unload HIRES device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = TRUE;
            // Load VGA device state
            stateNv(pDev, STATE_LOAD);
#endif            
            dacSetFan(pDev, 0);   // turn off fan
            REG_WR32(NV_PMC_INTR_EN_0, 0);                      // disable all interrupts. We're going to be shut down!
            REG_WR32(NV_PMC_INTR_0, REG_RD32(NV_PMC_INTR_0));   // clear any pending ints
            pDev->DeviceDisabled = TRUE;                      // signal ISR to ignore
            
            pDev->Power.strapInfo = REG_RD32(NV_PEXTDEV_BOOT_0);    // READ STRAPS

            nvHalMcPower(pDev, MC_POWER_LEVEL_3, pDev->Power.State);

            // We will always transition thru D0, so this SHOULDN'T ever be true.
            if (pDev->Power.State == MC_POWER_LEVEL_1) // if we slowed down mclk, restore it
            {
                REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
            }else if (pDev->Power.State == MC_POWER_LEVEL_4) //Have we gone through APM suspend?
            {
                // turn clocks up to avoid mem corruption when saving instance memory.
                REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);
                REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
            }
            
            // copy from framebuffer instance memory to save area.
            status = osAllocMem((VOID **)&pDev->fb_save, pDev->Pram.HalInfo.TotalInstSize);
            if (!status)
            {
                fb_sv = pDev->fb_save;
                for(i=0; i < pDev->Pram.HalInfo.TotalInstSize / 4; i++)
                {
                    *fb_sv++ = REG_RD32(NV_PRAMIN_DATA032(i));      // from the top--64k BIOS + instance memory
                                                                    // this actually decrements from the top
                }            
            }
            // Save clock coefficients
            if (pDev->Power.State == MC_POWER_LEVEL_0) // already slowed down clocks?
            {
                pDev->Power.nvpll = REG_RD32(NV_PRAMDAC_NVPLL_COEFF);   // nvclk
                pDev->Power.vpll = REG_RD32(NV_PRAMDAC_VPLL_COEFF);   // vclk
                pDev->Power.mpll = REG_RD32(NV_PRAMDAC_MPLL_COEFF);     // mclk
            }
            // turn clocks down to save power
            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_SOURCE,_PROG_ALL);   // program all clocks
            FLD_WR_DRF_NUM(_PRAMDAC,_NVPLL_COEFF, _PDIV, 4);    // max pwr saving is not max divisor
            FLD_WR_DRF_NUM(_PRAMDAC,_NVPLL_COEFF, _NDIV, 4);    // pll lower limit around 4-10
            FLD_WR_DRF_NUM(_PRAMDAC,_MPLL_COEFF, _PDIV, 5);                 // div by 32
            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_VPLL_SOURCE,_XTAL);  // use crystal
            
            // Power down DAC
            FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF);

            // On some architectures it's not always the case that we will get
            // to D3 having come through D1. On the Mac we may get here from
            // D0. If that is the case we should save the power_coeff here.
            if (pDev->Power.State == MC_POWER_LEVEL_0) 
            {
                pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
            }

            // Save a few critical registers that are going to be lost
            pDev->Master.Enable = REG_RD32(NV_PMC_ENABLE);    // gr enable gets blown away
            pDev->Master.PciNv19 = REG_RD32(NV_PBUS_PCI_NV_19);     // read AGP COMMAND

            dacDisableDac(pDev, 1);         // blank display on second head
            
            // Set power state in PCI config reg so BIOS can read
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D3_HOT);
            
            pDev->Power.State = MC_POWER_LEVEL_3;
            break; // case power level 3
    }
    return RM_OK;
}

           


