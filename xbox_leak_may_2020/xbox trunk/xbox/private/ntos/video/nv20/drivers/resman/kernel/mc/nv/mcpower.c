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
#include <nvagp.h>
#include <dac.h>

#define KERNEL_PM_STATE_CHANGE // Do in kernel state changes which used to be done in OS-specific section

// this define is in the chip-specific headers; it seems like it ought to go in nv_ref.h.
#define NV_PRMIO_RMA_PTR                                 0x00007104 /* RW-4R */

// For APM, slow down AGP data to slowest possible 
#define NV_PBUS_DEBUG_3_AGP_4X_NVCLK_SLOW  0x00000001   // NV_PBUS_DEBUG_3_AGP_4X_NVCLK data throttling for AGP


//
// Find our NV device on the PCI bus and enable MEM space accesses.
//
RM_STATUS
mcPowerRestorePCIIoMemSpace
(
    PHWINFO pDev
)
{
    VOID *handle;
    U008 bus, device;
    U016 vendorID, deviceID;
    U016 command;

    // do we already know our bus/device?
    if (pDev->Mapping.BusDevice == 0)
    {
        // we're checking all the device/funcs for the first 5 buses
        for (bus = 0; bus < 5; bus++)
        {
            for (device = 0; device < PCI_MAX_DEVICES; device++)
            {
                // read at bus, device (we're always function 0)
                handle = osPciInitHandle(bus, device, 0, &vendorID, &deviceID);
                if (!handle)
                    continue;

                if (vendorID != NV_PBUS_PCI_NV_0_VENDOR_ID_NVIDIA)
                    continue;

                // make sure we're a VGA device class
                if (osPciReadByte(handle, 0xB) != PCI_CLASS_DISPLAY_DEV)
                    continue;

                // if the BAR0 matches our PhysAddr, it's the correct device
                if (osPciReadDword(handle, PCI_BASE_ADDRESS_0) != pDev->Mapping.PhysAddr)
                    continue;

                // save our Bus and Device location
                pDev->Mapping.BusDevice = (bus << BUS_DEVICE_BUS_SHIFT) | device;
                RM_ASSERT(pDev->Mapping.BusDevice);    // expect this to be non-zero
                break;
            }
        }
    }

    if (pDev->Mapping.BusDevice == 0)
    {
        DBG_BREAKPOINT();
        return RM_ERROR;    // couldn't find it
    }

    bus =    (U008)(pDev->Mapping.BusDevice >> BUS_DEVICE_BUS_SHIFT);
    device = (U008)(pDev->Mapping.BusDevice & BUS_DEVICE_BUS_MASK);

    handle = osPciInitHandle(bus, device, 0, &vendorID, &deviceID);
    if (!handle)
    {
        DBG_BREAKPOINT();
        return RM_ERROR;    // couldn't create the handle
    }

    // restore IO/MEM space enables in our PCI config space
    command = osPciReadWord(handle, PCI_COMMAND);

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "NVRM: Enabling IO/MEM Space via PCI Command:\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Bus: ", bus);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Device: ", device);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      current  command: ", command);

    command &= ~0x3;        // clear existing bits
    command |= (U016)pDev->Power.PciIoMemSpace;
    osPciWriteWord(handle, PCI_COMMAND, command);

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      restored command: ", command);

    // verify we've been successful
    if (REG_RD_DRF(_PBUS, _PCI_NV_0, _VENDOR_ID) != NV_PBUS_PCI_NV_0_VENDOR_ID_NVIDIA)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unable to Enable MEM Space Access\n");
        DBG_BREAKPOINT();
        return RM_ERROR;
    }

    return RM_OK;
}

//  Transition between power states
//  States are:
//  Level 0 - full on
//  Level 1 - slowed clocks
//  Level 2 - slowed clocks, mobile style (not all devices support)
//  Level 3 - very slow clocks, state saved (regs & instance memory)
RM_STATUS mcPowerState
(
    PHWINFO pDev, 
    U032    state,
    U032    Head
)
{
    U032 i, data, data32, status;
    U016 data16;
    U008 lock, retry = 0;
    U032 *fb_sv;
    BOOL   biosUsedToInit;
    U032    EAX, EBX, ECX, EDX;
    U032 /*M,N,P,*/ NVClkSlow, fpMode, NVClktemp;
    U008 lockA, lockB, cr3BhA, cr3BhB, cr2BhA, cr2BhB, data08;

    //
    // We have potential preemption issues in this code on MP systems
    // when accessing any of the indexed registers.  It's possible to take
    // an interrupt on another processor while we're in here and end up
    // with corrupted state.
    //
    // To avoid these races, we use the os{Enter,Exit}CriticalCode
    // routines to serialize these critical sections with other parts of
    // the RM that operate at interrupt level.
    //
    switch (state) 
    {
        case MC_POWER_LEVEL_0:          // power up
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 0");

            //In Toshiba case, we resume to the system BIOS's choice of one
            // display, even if we were in clone mode when we shut down.  Yeesh.
            // In spanning mode, we use more convoluted logic.
//            pDev->Dac.DeviceProperties.InferAuxDev = FALSE;
            pDev->Dac.DeviceProperties.LastSwitchEvent = SWITCH_FROM_WINDOWS;

            switch (pDev->Power.State)  // procedure depends on what state we are transitioning from
            {
                case MC_POWER_LEVEL_1:
                    // Disable I/O broadcast mode for now to work around a
                    // a hardware bug that prevents us from reaching
                    // HeadB through privSpace with this enabled
                    FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _DISABLE);

                    // turn clocks up
                    FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _ON);
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);

                    //Gradually step up nvclk
                    if (dacProgramNVClk(pDev) != RM_OK)
                    {
                        U032 M, N, P;

                        // Start with 50MHz
                        dacCalcMNP(pDev, 0, 5000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));

                        // Move to 100MHz
                        dacCalcMNP(pDev, 0, 10000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));

                        // Program final
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                    }

                    REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
                    
                    nvHalMcPower(pDev, MC_POWER_LEVEL_0, pDev->Power.State);

                    //Re-detect devices on return from standby for non-Toshiba mobiles.
                    if (pDev->Power.MobileOperation && !(pDev->Dac.DeviceProperties.OverrideSBIOSDevs)) {
                        osEnterCriticalCode(pDev);
                        dacDetectDevices(pDev, FALSE, TRUE);
                        osExitCriticalCode(pDev);
                    }

                    //turn on backlight (if either head is the flat panel)
                    if ((pDev->Power.MobileOperation == 2) && ((GETDISPLAYTYPE(pDev, 0) == DISPLAY_TYPE_FLAT_PANEL) ||
                                                               (GETDISPLAYTYPE(pDev, 1) == DISPLAY_TYPE_FLAT_PANEL)))
                    {
                        status = dacBacklightOnMobilePanel(pDev);
                        if (status)
                            break;
                        pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;
                    }
                    
                    // Turn on sequencer. We cannot rely on display driver doing a config_VGA to set the mode.
                    osEnterCriticalCode(pDev);
                    WriteIndexed(NV_PRMVIO_SRX, 0x0101);
                    osExitCriticalCode(pDev);

                    pDev->Power.State = MC_POWER_LEVEL_0;
                    pDev->DeviceDisabled = FALSE;     // let ISR handle interrupts
                    break;
                
                case MC_POWER_LEVEL_2:
                    // Power level for standby on mobile systems

                    // Turn back on our PCI IO/MEM space
                    mcPowerRestorePCIIoMemSpace(pDev);

                    // Disable I/O broadcast mode for now to work around a
                    // a hardware bug that prevents us from reaching
                    // HeadB through privSpace with this enabled
                    FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _DISABLE);

                    data32 = (pDev->Power.PbusDebug1 & ~(SF_DEF(_PBUS, _DEBUG_1_IDDQ, _ONE)));
                    REG_WR32(NV_PBUS_DEBUG_1, data32);

                    osDelay(2); // wait 2 ms

                    data32 &= ~(SF_DEF(_PBUS, _DEBUG_1_PLL_STOPCLK, _ENABLE));
                    REG_WR32(NV_PBUS_DEBUG_1, data32);

                    //It is possible that the TMDS link programming could 
                    //get messed up. This happens rather frequently when
                    //resuming from Hibernation on Win98 (Compal namely).
                    //We should power off the panel so it can be powered
                    //on later. This ensure the TMDS links get programmed
                    //correctly.
                    if (pDev->Power.MobileOperation)
                    {
                        osEnterCriticalCode(pDev);
                        lockB = UnlockCRTC(pDev, 1);
                        CRTC_RD(0x3B, cr3BhB, 1);
                        RestoreLock(pDev, 1, lockB);
                        osExitCriticalCode(pDev);

                        if (cr3BhB & 0x10)
                        {
                            U032 tempPowerFlags = pDev->Power.Flags & DAC_POWER_PENDING;

                            pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;
                            pDev->Power.Flags &= ~DAC_POWER_PENDING;
                            dacPowerOffMobilePanel(pDev, Head);
                            //restore original seetings for power flags.
                            pDev->Power.Flags |= tempPowerFlags;
                        }
                    }

                    // restore the fifo context in case the VBIOS was POSTed.
                    nvHalFbControl(pDev, FB_CONTROL_INSTWR_ENABLE);
                    nvHalMcPower(pDev, MC_POWER_LEVEL_0, pDev->Power.State);

                    // Make sure fb_save is not NULL
                    if (pDev->fb_save != 0)
                    {
                        // copy from save area to framebuffer instance memory
                        fb_sv = pDev->fb_save;

                        for(i=0; i < pDev->Pram.HalInfo.TotalInstSize / 4; i++)
                        {
                            data = *fb_sv++;
                            REG_WR32(NV_PRAMIN_DATA032(i), data);
                        }            
                        osFreeMem((VOID *)pDev->fb_save);
                        pDev->fb_save = 0;
                    }
                    
                    pDev->Power.State = MC_POWER_LEVEL_0;
                    
                    // now we are powered up
                    
                    // leave the real mode access window pointing to a safe place.
                    osEnterCriticalCode(pDev);
                    REG_WR32(NV_PRMIO_RMA_PTR, NV_CONFIG_PCI_NV_0); // point to revision reg
                    EnableHead(pDev, Head);                         // ensure I/O is directed to head 0
                    lock = UnlockCRTC(pDev, Head);
                    CRTC_WR(NV_CIO_CRE_RMA__INDEX, 0, Head);        // set index mode to be safe
                    RestoreLock(pDev, Head, lock);
                    osExitCriticalCode(pDev);
                    
                    //Re-detect devices on return from standby for non-Toshiba mobiles.
                    if (pDev->Power.MobileOperation && !(pDev->Dac.DeviceProperties.OverrideSBIOSDevs)) {
                        osEnterCriticalCode(pDev);
                        dacDetectDevices(pDev, FALSE, TRUE);
                        osExitCriticalCode(pDev);
                    }

                    // Here, we assume that all devices are turned off on non-Toshiba Mobiles
                    if((pDev->Power.MobileOperation) && (pDev->Power.MobileOperation != 2))
                        pDev->Dac.DevicesEnabled &= ~(DAC_ALL_DEVICES_ENABLED);

                    //Turn off DFP backlight
                    // This should be before the reset. The reset will turn off the 
                    // pixel clocks and start the LCD blooming.
                    if (pDev->Power.MobileOperation)
                    {
                        status = dacBacklightOffMobilePanel(pDev);
                        if (status)
                            break;
                    }

                    // now we are powered up
                    break;
                case MC_POWER_LEVEL_3:
                case MC_POWER_LEVEL_7:
                    // turn off bus master before resetting master enable
                    // else we may get spurious bus master cycles 
                        data32 = REG_RD32(NV_PBUS_PCI_NV_1);
                        FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _BUS_MASTER, _DISABLED);

                        if (pDev->Power.MobileOperation) {
                            //Is it safe to make this call this early?  Probably not.
                            EAX = 0x4F14;       // VESA EDID function
                            EBX = 0x8002;       // set center/scale mode
                            ECX = 0;
                            EDX = 0;
                            status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
                            if (status == RM_OK) {
                                if ((ECX & 0x0000FFFF ) == 1)
                                    fpMode = NV_PRAMDAC_FP_TG_CONTROL_MODE_CENTER;
                                else
                                    fpMode = NV_PRAMDAC_FP_TG_CONTROL_MODE_SCALE;
                            }
                            else {
                                fpMode = NV_PRAMDAC_FP_TG_CONTROL_MODE_SCALE;
                            }
                        }

                    

                    //Turn off DFP backlight
                    // This should be before the reset. The reset will turn off the 
                    // pixel clocks and start the LCD blooming.
                    if (pDev->Power.MobileOperation)
                    {
                        dacBacklightOffMobilePanel(pDev);
                    }


                    //Program PLL_COEFF_SELECT to 0 to get NVCLK and MCLK back to default
                    //values. On i850 chipset with NV5 the programmed slowed down values
                    //can result in sufficient delays on register reads to cause the host to
                    //hang the bus.
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, 0);

                    REG_WR32(NV_PTIMER_NUMERATOR, pDev->Timer.Numerator);     
                    REG_WR32(NV_PTIMER_DENOMINATOR, pDev->Timer.Denominator);

                    // Wait 10 ms to allow PLL to lock.
                    // HW guy says 1 ms should be enough.
                    // In fact, very small delay seems to do it.
                    tmrDelay(pDev, 10000000); 

                    if (pDev->Power.MobileOperation != 1) //if not Dell (causes hang in ACPI)
                    {
                        // Reset the chip, in case the HW didn't.
                        REG_WR32(NV_PMC_ENABLE, 0);
                        REG_RD32(NV_PMC_ENABLE);
                        REG_RD32(NV_PMC_ENABLE);

                        REG_WR32(NV_PMC_ENABLE, -1);
                        REG_RD32(NV_PMC_ENABLE);
                        REG_RD32(NV_PMC_ENABLE);

                        // Make sure that we know we've reset the device
                        // If we try to power on the flat panel without running the reset
                        // scripts, the flat panel will bloom.
                        pDev->Dac.DevicesEnabled |= DAC_PANEL_CLK_RESET;
                    }

                    // restore bus master enable
                    REG_WR32(NV_PBUS_PCI_NV_1, data32);

                    // Disable I/O broadcast mode for now to work around a
                    // a hardware bug that prevents us from reaching
                    // HeadB through privSpace with this enabled
                    FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _DISABLE);

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

                    //A hack, part A: read the CR3B registers before we re-POST the BIOS,
                    // because that overwrites the scratch registers.  Note that we can't
                    // just cache these registers on shutdown, because at power-up the
                    // system BIOS may have a different opinion about what displays are
                    // available and should be enabled.

                    //A hack, partA.1: it appears the CR2B also gets stomped on durring
                    //re-POST. CR2B[7:4] indicate which vga modes are valid. This is used
                    //for Toshiba when displaying a vga graphic when going into Hibernation
                    //on Win98.
                    if (pDev->Power.MobileOperation) {
                        osEnterCriticalCode(pDev);
                        lockA = UnlockCRTC(pDev, 0);
                        lockB = UnlockCRTC(pDev, 1);
                        CRTC_RD(0x3B, cr3BhA, 0);
                        CRTC_RD(0x3B, cr3BhB, 1);
                        CRTC_RD(0x2B, cr2BhA, 0);
                        CRTC_RD(0x2B, cr2BhB, 1);
                        RestoreLock(pDev, 0, lockA);
                        RestoreLock(pDev, 1, lockB);
                        osExitCriticalCode(pDev);
                    }

                    //Dac.HalInfo.NVClk will be reinitialized durring DevinitInitializeDevice
                    //save this value away so we keep the current clock setting. This only
                    //becomes an issue if the user has changed the default NVCLK via
                    //overclocking Control Panel controls.
                    NVClktemp = pDev->Dac.HalInfo.NVClk;

                    // Make sure fb_save is not NULL
                    if (pDev->fb_save != 0)
                    {
                        // POST the device using BIOS init tables
                        DevinitInitializeDevice(pDev, (PHWREG)pDev->fb_save, &biosUsedToInit);
                    }

                    pDev->Dac.HalInfo.NVClk = NVClktemp;

                    //A hack, part B: write the CR3B registers back post-POST.
                    // We read them immediately before the init.
                    if (pDev->Power.MobileOperation) {
                        //Stomp back in the CR3B values:
                        osEnterCriticalCode(pDev);
                        lockA = UnlockCRTC(pDev, 0);
                        lockB = UnlockCRTC(pDev, 1);
                        CRTC_WR(0x3B, cr3BhA, 0);
                        CRTC_WR(0x3B, cr3BhB, 1);
                        CRTC_WR(0x2B, cr2BhA, 0);
                        CRTC_WR(0x2B, cr2BhB, 1);
                        RestoreLock(pDev, 0, lockA);
                        RestoreLock(pDev, 1, lockB);
                        osExitCriticalCode(pDev);
                    }

                    // turn clocks up
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);

                    //Gradually step up nvclk
                    if (dacProgramNVClk(pDev) != RM_OK)
                    {
                        U032 M, N, P;
                        
                        // Start with 50MHz
                        dacCalcMNP(pDev, 0, 5000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));
                        
                        // Move to 100MHz
                        dacCalcMNP(pDev, 0, 10000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));

                        // Program final
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                    }

                    //
                    // For NV20, we let the HAL restore our mclk setting.
                    //
                    // XXXscottl - we should really let the  HAL do this for
                    // all chips.
                    //
                    if (!IsNV20orBetter(pDev))
                    {
                        U032 M, N, P;
                        
                        // Start with 50MHz
                        dacCalcMNP(pDev, 0, 5000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_MPLL_COEFF, DRF_NUM(_PRAMDAC, _MPLL_COEFF, _MDIV, M) |
                                                        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _NDIV, N) |
                                                        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _PDIV, P));
                        
                        // Move to 100MHz
                        dacCalcMNP(pDev, 0, 10000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_MPLL_COEFF, DRF_NUM(_PRAMDAC, _MPLL_COEFF, _MDIV, M) |
                                                        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _NDIV, N) |
                                                        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _PDIV, P));
                        
                        // Program final
                        REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
                    }

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
                    // allow any further PLL changes to settle
                    tmrDelay(pDev, 2000000);    // 2ms

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

                    // Make sure fb_save is not NULL
                    if (pDev->fb_save != 0)
                    {
                        // copy from save area to framebuffer instance memory
                        fb_sv = pDev->fb_save;

                        for(i=0; i < pDev->Pram.HalInfo.TotalInstSize / 4; i++)
                        {
                            data = *fb_sv++;
                            REG_WR32(NV_PRAMIN_DATA032(i), data);
                        }            
                        osFreeMem((VOID *)pDev->fb_save);
                        pDev->fb_save = 0;
                    }

                    pDev->DeviceDisabled = FALSE;     // let ISR handle interrupts

                    if (pDev->Power.MobileOperation) {
                        //Restore centered/scaled state.  We set both heads, as in
                        //dacSetFlatPanelScaling.
                        DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, fpMode, 0);
                        DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, fpMode, 1);
                    }

//                    pDev->Dac.DeviceProperties.InferAuxDev = FALSE;
                    pDev->Dac.DeviceProperties.LastSwitchEvent = SWITCH_FROM_COLD_START;
//                    dacDetectDevices(pDev, FALSE, pDev->Dac.DeviceProperties.OverrideSBIOSDevs);
                    dacDetectDevices(pDev, FALSE, TRUE);

                    // Here, we assume that all devices are turned off
                    pDev->Dac.DevicesEnabled &= ~(DAC_ALL_DEVICES_ENABLED);
 
                    //Restore Spread Spectrum settings
                    if (pDev->Power.MobileOperation) 
                    {
                        // only return the portion of GIPO that relates to spread spectrum
                        // leave the rest as is so that the panel doesn't get inadvertantly turned
                        // on before it is meant to.
                        // Look at all them bit masks!  Unfortunately, there isn't a single definition
                        // for all of GPIO_EXT_5, so I have to use the definitions for each bit field.
                        data32 = REG_RD32(NV_PCRTC_GPIO_EXT);
                        data32 &= ~((DRF_MASK(NV_PCRTC_GPIO_EXT_5_OUTPUT) << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_OUTPUT))
                               |    (DRF_MASK(NV_PCRTC_GPIO_EXT_5_ENABLE) << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_ENABLE))
                               |    (DRF_MASK(NV_PCRTC_GPIO_EXT_5_INPUT)  << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_INPUT)));

                        data32 |= (pDev->Power.GPIOExt & ((DRF_MASK(NV_PCRTC_GPIO_EXT_5_OUTPUT) << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_OUTPUT)) 
                                                       |  (DRF_MASK(NV_PCRTC_GPIO_EXT_5_ENABLE) << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_ENABLE)) 
                                                       |  (DRF_MASK(NV_PCRTC_GPIO_EXT_5_INPUT)  << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_INPUT))));

                        REG_WR32(NV_PCRTC_GPIO_EXT, data32);

                        osEnterCriticalCode(pDev);
                        lockA = UnlockCRTC(pDev, 0);
                        lockB = UnlockCRTC(pDev, 1);
                        CRTC_WR(0x51, pDev->Power.cr51hA, 0);
                        CRTC_WR(0x51, pDev->Power.cr51hB, 1);
                        RestoreLock(pDev, 0, lockA);
                        RestoreLock(pDev, 1, lockB);
                        osExitCriticalCode(pDev);
                    }

                    //
                    // Restore any neccessary agp chipset registers when coming out of hibernation.
                    //
                    if (pDev->Power.State == MC_POWER_LEVEL_7)
                        NvSaveRestoreAGPChipset(pDev, MC_POWER_LEVEL_0);

                    break;

            case MC_POWER_LEVEL_4:
                if (pDev->Power.MobileOperation) 
                {
                    // turn off bus master before resetting master enable
                    // else we may get spurious bus master cycles 

                        /*if (pDev->Power.MobileOperation) {
                            //Is it safe to make this call this early?  Probably not.
                            EAX = 0x4F14;       // VESA EDID function
                            EBX = 0x8002;       // set center/scale mode
                            ECX = 0;
                            EDX = 0;
                            status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
                            if (status == RM_OK) {
                                if ((ECX & 0x0000FFFF ) == 1)
                                    fpMode = NV_PRAMDAC_FP_TG_CONTROL_MODE_CENTER;
                                else
                                    fpMode = NV_PRAMDAC_FP_TG_CONTROL_MODE_SCALE;
                            }
                            else {
                                fpMode = NV_PRAMDAC_FP_TG_CONTROL_MODE_SCALE;
                            }
                        } */

                    

                    //Turn off DFP backlight
                    // This should be before the reset. The reset will turn off the 
                    // pixel clocks and start the LCD blooming.
                    if (pDev->Power.MobileOperation)
                    {
                        dacBacklightOffMobilePanel(pDev);
                    }


                    //Program PLL_COEFF_SELECT to 0 to get NVCLK and MCLK back to default
                    //values. On i850 chipset with NV5 the programmed slowed down values
                    //can result in sufficient delays on register reads to cause the host to
                    //hang the bus.
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, 0);

                    REG_WR32(NV_PTIMER_NUMERATOR, pDev->Timer.Numerator);     
                    REG_WR32(NV_PTIMER_DENOMINATOR, pDev->Timer.Denominator);

                    // Wait 10 ms to allow PLL to lock.
                    // HW guy says 1 ms should be enough.
                    // In fact, very small delay seems to do it.
                    tmrDelay(pDev, 10000000); 

                    {
                        /*U032 PCI_NV19;

                        data32 = REG_RD32(NV_PBUS_PCI_NV_1);
                        FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _BUS_MASTER, _DISABLED);
                        PCI_NV19 = REG_RD32(NV_PBUS_PCI_NV_19);
                        FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_AGP_ENABLE, _OFF);
						*/

                            // Reset the chip, in case the HW didn't.
                            REG_WR32(NV_PMC_ENABLE, 0);
                            REG_RD32(NV_PMC_ENABLE);
                            REG_RD32(NV_PMC_ENABLE);

                            REG_WR32(NV_PMC_ENABLE, -1);
                            REG_RD32(NV_PMC_ENABLE);
                            REG_RD32(NV_PMC_ENABLE);

                            // Make sure that we know we've reset the device
                            // If we try to power on the flat panel without running the reset
                            // scripts, the flat panel will bloom.
                            pDev->Dac.DevicesEnabled |= DAC_PANEL_CLK_RESET;

                        // restore bus master enable
                        //REG_WR32(NV_PBUS_PCI_NV_1, data32);
                       // REG_WR32(NV_PBUS_PCI_NV_19, PCI_NV19);
                    }
                    // Disable I/O broadcast mode for now to work around a
                    // a hardware bug that prevents us from reaching
                    // HeadB through privSpace with this enabled
                    FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _DISABLE);

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

                    //A hack, part A: read the CR3B registers before we re-POST the BIOS,
                    // because that overwrites the scratch registers.  Note that we can't
                    // just cache these registers on shutdown, because at power-up the
                    // system BIOS may have a different opinion about what displays are
                    // available and should be enabled.

                    //A hack, partA.1: it appears the CR2B also gets stomped on durring
                    //re-POST. CR2B[7:4] indicate which vga modes are valid. This is used
                    //for Toshiba when displaying a vga graphic when going into Hibernation
                    //on Win98.
                    if (pDev->Power.MobileOperation) {
                        osEnterCriticalCode(pDev);
                        lockA = UnlockCRTC(pDev, 0);
                        lockB = UnlockCRTC(pDev, 1);
                        CRTC_RD(0x3B, cr3BhA, 0);
                        CRTC_RD(0x3B, cr3BhB, 1);
                        CRTC_RD(0x2B, cr2BhA, 0);
                        CRTC_RD(0x2B, cr2BhB, 1);
                        RestoreLock(pDev, 0, lockA);
                        RestoreLock(pDev, 1, lockB);
                        osExitCriticalCode(pDev);
                    }

                    //Dac.HalInfo.NVClk will be reinitialized durring DevinitInitializeDevice
                    //save this value away so we keep the current clock setting. This only
                    //becomes an issue if the user has changed the default NVCLK via
                    //overclocking Control Panel controls.
                    NVClktemp = pDev->Dac.HalInfo.NVClk;

                    // Make sure fb_save is not NULL
                    if (pDev->fb_save != 0)
                    {
                        // POST the device using BIOS init tables
                        DevinitInitializeDevice(pDev, (PHWREG)pDev->fb_save, &biosUsedToInit);
                    }

                    pDev->Dac.HalInfo.NVClk = NVClktemp;

                    //A hack, part B: write the CR3B registers back post-POST.
                    // We read them immediately before the init.
                    if (pDev->Power.MobileOperation) {
                        //Stomp back in the CR3B values:
                        osEnterCriticalCode(pDev);
                        lockA = UnlockCRTC(pDev, 0);
                        lockB = UnlockCRTC(pDev, 1);
                        CRTC_WR(0x3B, cr3BhA, 0);
                        CRTC_WR(0x3B, cr3BhB, 1);
                        CRTC_WR(0x2B, cr2BhA, 0);
                        CRTC_WR(0x2B, cr2BhB, 1);
                        RestoreLock(pDev, 0, lockA);
                        RestoreLock(pDev, 1, lockB);
                        osExitCriticalCode(pDev);
                    }

                    // turn clocks up
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);

                    //Gradually step up nvclk
                    if (dacProgramNVClk(pDev) != RM_OK)
                    {
                        U032 M, N, P;
                        
                        // Start with 50MHz
                        dacCalcMNP(pDev, 0, 5000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));
                        
                        // Move to 100MHz
                        dacCalcMNP(pDev, 0, 10000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));

                        // Program final
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                    }

                    //
                    // For NV20, we let the HAL restore our mclk setting.
                    //
                    // XXXscottl - we should really let the  HAL do this for
                    // all chips.
                    //
                    if (!IsNV20orBetter(pDev))
                    {
                        U032 M, N, P;
                        
                        // Start with 50MHz
                        dacCalcMNP(pDev, 0, 5000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_MPLL_COEFF, DRF_NUM(_PRAMDAC, _MPLL_COEFF, _MDIV, M) |
                                                        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _NDIV, N) |
                                                        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _PDIV, P));
                        
                        // Move to 100MHz
                        dacCalcMNP(pDev, 0, 10000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_MPLL_COEFF, DRF_NUM(_PRAMDAC, _MPLL_COEFF, _MDIV, M) |
                                                        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _NDIV, N) |
                                                        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _PDIV, P));
                        
                        // Program final
                        REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
                    }

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
                    // come out of what LOOKS like Hal power level 3 (Mobile 4) into 0
                    nvHalMcPower(pDev, MC_POWER_LEVEL_0, MC_POWER_LEVEL_3);
                    // allow any further PLL changes to settle
                    tmrDelay(pDev, 2000000);    // 2ms

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

                    // Make sure fb_save is not NULL
                    if (pDev->fb_save != 0)
                    {
                        // copy from save area to framebuffer instance memory
                        fb_sv = pDev->fb_save;

                        for(i=0; i < pDev->Pram.HalInfo.TotalInstSize / 4; i++)
                        {
                            data = *fb_sv++;
                            REG_WR32(NV_PRAMIN_DATA032(i), data);
                        }            
                        osFreeMem((VOID *)pDev->fb_save);
                        pDev->fb_save = 0;
                    }

                    pDev->DeviceDisabled = FALSE;     // let ISR handle interrupts

#if 0
                    if (pDev->Power.MobileOperation) {
                        //Restore centered/scaled state.  We set both heads, as in
                        //dacSetFlatPanelScaling.
                        DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, fpMode, 0);
                        DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, fpMode, 1);
                    }
#endif

//                    pDev->Dac.DeviceProperties.InferAuxDev = FALSE;
                    pDev->Dac.DeviceProperties.LastSwitchEvent = SWITCH_FROM_COLD_START;
//                    dacDetectDevices(pDev, FALSE, pDev->Dac.DeviceProperties.OverrideSBIOSDevs);
                    dacDetectDevices(pDev, FALSE, TRUE);

                    // Here, we assume that all devices are turned off
                    pDev->Dac.DevicesEnabled &= ~(DAC_ALL_DEVICES_ENABLED);
 
                    //Restore Spread Spectrum settings
                    if (pDev->Power.MobileOperation) 
                    {
                        // only return the portion of GIPO that relates to spread spectrum
                        // leave the rest as is so that the panel doesn't get inadvertantly turned
                        // on before it is meant to.
                        // Look at all them bit masks!  Unfortunately, there isn't a single definition
                        // for all of GPIO_EXT_5, so I have to use the definitions for each bit field.
                        data32 = REG_RD32(NV_PCRTC_GPIO_EXT);
                        data32 &= ~((DRF_MASK(NV_PCRTC_GPIO_EXT_5_OUTPUT) << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_OUTPUT))
                               |    (DRF_MASK(NV_PCRTC_GPIO_EXT_5_ENABLE) << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_ENABLE))
                               |    (DRF_MASK(NV_PCRTC_GPIO_EXT_5_INPUT)  << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_INPUT)));

                        data32 |= (pDev->Power.GPIOExt & ((DRF_MASK(NV_PCRTC_GPIO_EXT_5_OUTPUT) << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_OUTPUT)) 
                                                       |  (DRF_MASK(NV_PCRTC_GPIO_EXT_5_ENABLE) << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_ENABLE)) 
                                                       |  (DRF_MASK(NV_PCRTC_GPIO_EXT_5_INPUT)  << DRF_SHIFT(NV_PCRTC_GPIO_EXT_5_INPUT))));

                        REG_WR32(NV_PCRTC_GPIO_EXT, data32);

                        osEnterCriticalCode(pDev);
                        lockA = UnlockCRTC(pDev, 0);
                        lockB = UnlockCRTC(pDev, 1);
                        CRTC_WR(0x51, pDev->Power.cr51hA, 0);
                        CRTC_WR(0x51, pDev->Power.cr51hB, 1);
                        RestoreLock(pDev, 0, lockA);
                        RestoreLock(pDev, 1, lockB);
                        osExitCriticalCode(pDev);
                    }
                }
                else
                {
                    FLD_WR_DRF_DEF(_PCRTC, _INTR_EN_0, _VBLANK, _ENABLED);
                    FLD_WR_DRF_DEF(_PTIMER, _INTR_EN_0, _ALARM, _ENABLED);
                    data16 = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A);
                    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016) (data16 & ~0x8000));     // set Hsync to active

                    // turn clocks up
                    // restore PCI and AGP throttle 
                    FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _ON);
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);

                    //Gradually step up nvclk
                    {
                        U032 M, N, P;

                        // Start with 50MHz
                        dacCalcMNP(pDev, 0, 5000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));

                        // Move to 100MHz
                        dacCalcMNP(pDev, 0, 10000, &M, &N, &P);
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
                                                         DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));

                        // Program final
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                    }

//                        REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
                    REG_WR32(NV_PBUS_DEBUG_3, pDev->Power.PbusDebug3);           // pbus debug 3                            
                    FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _PCIM_THROTTLE, _DISABLED);      // pbus debug 1
                    nvHalMcPower(pDev, MC_POWER_LEVEL_0, pDev->Power.State);
                    // Turn on sequencer. We cannot rely on display driver doing a config_VGA to set the mode.
                    WriteIndexed(NV_PRMVIO_SRX, 0x0101);
                }         
                    pDev->Power.State = MC_POWER_LEVEL_0;

                    break;
            } // switch (pDev->Power.State)
#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            if(pDev->Power.State == MC_POWER_LEVEL_7)
            {
                U032    count;
        
                /*
                 * When coming back from hibernation under Win2k (MC_POWER_LEVEL_7), need to clear the display
                 * before unloading the VGA device state so that the display won't get filled with garbage
                 * text characters when the HIRES device state is loaded and the tiling bits are set.
                 * This happens because the display mode is still currently a VGA display mode and the hi-res
                 * mode set hasn't happened yet.
                 */
                for (count = 0; count < (64*1024); count += 4)
                    FB_WR32(count, 0);

            }
            // Unload VGA device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = FALSE;
            // Load HIRES device state
            stateNv(pDev, STATE_LOAD);

#endif

            osEnterCriticalCode(pDev);
            data16 = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A);
            WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016) (data16 & 0x3fff));     // enable syncs
            REG_WR32(NV_PMC_INTR_EN_0, 1);                 // re-enable interrupts.
            dacSetFan(pDev, 1);   // turn on fan
            osExitCriticalCode(pDev);


            // Set power state in PCI config reg so BIOS can read
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D0);
            // Make sure BUS MASTERING is turned on (bug in system BIOS?)
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _BUS_MASTER, _ENABLED);
            // Make sure MEMORY is turned on (VGA BIOS may disable to prevent hang if something accesses framebuffer during LEVEL 2)
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _MEMORY_SPACE, _ENABLED);
            // Make sure IO is turned on
            FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _IO_SPACE, _ENABLED);
            
            pDev->Power.State = MC_POWER_LEVEL_0;
            break;
            // end case MC_POWER_LEVEL_0
        
        //
        // Power Down
        //
        case MC_POWER_LEVEL_1:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 1");

            //Turn off DFP backlight
            if (pDev->Power.MobileOperation == 2)
            {
                dacBacklightOffMobilePanel(pDev);
            }

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

            osEnterCriticalCode(pDev);
            // stop fetching video mem
            data16 = ReadIndexed(NV_PRMVIO_SRX, 0x01);
            data16 |= 0x2000;   // screen off
            WriteIndexed(NV_PRMVIO_SRX, data16);
            osExitCriticalCode(pDev);
            
            // save current clock coefficients
            if (pDev->Power.State == 0) //Have we already slowed down the clocks?
            {

                U032 IsPllSlowed;

                //
                // Check to see if NVCLK is currently being dynamically slowed
                // If so, temporarily disable so we can get a true time delay
                //
                // KJK: I wonder if this should be halified?
                //
                if (IsNV11(pDev))
                {
                    IsPllSlowed = REG_RD_DRF(_PBUS, _DEBUG_1, _CORE_SLOWDWN);
                    FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, 0);
                }
                //
                // If the Power.DisplayState is currently MC_POWER_LEVEL_6 (off)
                // then there's no need to save this register since it's already
                // been saved. Doing so again could result in an incorrect value
                // being read since the dac has already been disabled.
                // This should only ever be the case on Win2K since other os's don't
                // use the MC_POWER_LEVEL_6 state. (i.e. all other os's should be saving
                // the PLL_COEFF_SELECT register here all the time.
                //
                if ((pDev->Power.DisplayState[0] != MC_POWER_LEVEL_6)  
                 && (pDev->Power.DisplayState[1] != MC_POWER_LEVEL_6))
                    pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
                pDev->Power.nvpll = REG_RD32(NV_PRAMDAC_NVPLL_COEFF);   // nvclk
                pDev->Power.vpll = REG_RD32(NV_PRAMDAC_VPLL_COEFF);   // vclk
                pDev->Power.mpll = REG_RD32(NV_PRAMDAC_MPLL_COEFF);     // mclk

                //
                // Restore PLL slowdown setting
                //
                if (IsNV11(pDev))
                    FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, IsPllSlowed);

            }

            // turn clocks down to save power
            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_SOURCE,_PROG_ALL);   // program all clocks

            // NV5 and NV10 NV clocks are tied to MCLK
            if (IsNV15orBetter(pDev)) 
            {
                NVClkSlow = 6600;   // slow down to PCI clk speed
            }
            else
            {
                NVClkSlow = (pDev->Dac.HalInfo.MClk * 2)/30000;   // NV5/NV10 are limited to MCLK/1.5. Number is MHz * 100
            }

            //XXX: Don't do the slowdown for highspeed mems (DDR)
            if (pDev->Framebuffer.HalInfo.RamType != BUFFER_DDRAM)
            {
                NVClktemp = pDev->Dac.HalInfo.NVClk;
                pDev->Dac.HalInfo.NVClk = NVClkSlow * 10000;
                if (dacProgramNVClk(pDev) != RM_OK)
                {
                    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                }
                pDev->Dac.HalInfo.NVClk = NVClktemp;
                FLD_WR_DRF_NUM(_PRAMDAC,_MPLL_COEFF, _PDIV, 5);                 // div by 32
                
                // Shut off fan only if NVCLK is slowed
                osEnterCriticalCode(pDev);
                dacSetFan(pDev, 0);   // turn off fan
                osExitCriticalCode(pDev);
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

            //Turn off DFP backlight
            if (pDev->Power.MobileOperation == 2)
            {
                dacBacklightOffMobilePanel(pDev);
            }

            // Blank both heads
            osEnterCriticalCode(pDev);
            dacDisableDac(pDev, 0);
            dacDisableDac(pDev, 1);
            osExitCriticalCode(pDev);

            // Power management for mobile chips:
            // Set RAM self-refresh mode (HAL)
            // Set flag to tell VGA BIOS to handle
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 2");

#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            // Unload HIRES device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = TRUE;
            // Load VGA device state
            stateNv(pDev, STATE_LOAD);
#endif            
            // Save the fifo context (inc instance memory) in case the VBIOS gets POSTed (Dell)

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
            
            nvHalMcPower(pDev, MC_POWER_LEVEL_2, pDev->Power.State);
            
            // leave the real mode access window pointing to PBUS_DEBUG_1 so that the BIOS can wake us up if called.
            osEnterCriticalCode(pDev);
            REG_WR32(NV_PRMIO_RMA_PTR, NV_PBUS_DEBUG_1);    // point to PBUS_DEBUG_1 address
            EnableHead(pDev, Head);                         // ensure I/O is directed to head 0
            lock = UnlockCRTC(pDev, Head);
            CRTC_WR(NV_CIO_CRE_RMA__INDEX, 5, Head);        // set data mode
            CRTC_RD(0x2E, data08, Head);                    // leave pointing at innocuous location
            osExitCriticalCode(pDev);

            data32 = REG_RD32(NV_PBUS_DEBUG_1);
            pDev->Power.PbusDebug1 = data32;                // save this for resume function
            
            // save the current IO/MEM values
            pDev->Power.PciIoMemSpace = REG_RD32(NV_PBUS_PCI_NV_1) & 0x3;
            
            // For Dell, do not set NV_25_POWER_STATE. BIOS will handle.
            if (pDev->Power.MobileOperation == 1)        // if Dell, don't set standby
            {
                // Leave dac powered because BIOS does not turn this on!
                dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);
                
                // Set power state in PCI config reg so BIOS can read
                FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D0);   // Let BIOS do standby
            }
            else // Not Dell
            {
                // Set power state in PCI config reg so BIOS can read
                FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D3_HOT);  
                
                // If memory or I/O is accessed while asleep, we will hang, so don't allow it.
                // Disable in the PCI config space 
                FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _PLL_STOPCLK, _ENABLE);

                osDelay(2); // wait 2 ms

                FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _IDDQ, _ONE);
                
                //XXX: Don't do this right now. Causes hang in Standby for Toshiba Win2K.
                // disable I/O and memory
                //FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _IO_SPACE, _DISABLED);     // disable I/O while we are asleep
                //FLD_WR_DRF_DEF(_PBUS, _PCI_NV_1, _MEMORY_SPACE, _DISABLED); // disable memory while we are asleep
            }

            pDev->Power.State = MC_POWER_LEVEL_2;
            break;
        
        case MC_POWER_LEVEL_3:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 3");
            //Turn off DFP backlight
            if (pDev->Power.MobileOperation == 2)
            {
                dacBacklightOffMobilePanel(pDev);
            }


#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            // Unload HIRES device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = TRUE;
            // Load VGA device state
            stateNv(pDev, STATE_LOAD);
#endif

            REG_WR32(NV_PMC_INTR_EN_0, 0);                      // disable all interrupts. We're going to be shut down!
            REG_WR32(NV_PMC_INTR_0, REG_RD32(NV_PMC_INTR_0));   // clear any pending ints
            pDev->DeviceDisabled = TRUE;                      // signal ISR to ignore
            
            pDev->Power.strapInfo = REG_RD32(NV_PEXTDEV_BOOT_0);    // READ STRAPS
            //
            // On some architectures it's not always the case that we will get
            // to D3 having come through D1. On the Mac we may get here from
            // D0. If that is the case we should save the power_coeff here.
            //
            // Also do NOT save the COEFF_SELECT here if Power.DisplayState is currently
            // MC_POWER_LEVEL_6 (off) because it's already been saved.
            // been saved. Doing so again could result in an incorrect value
            // being read since the dac has already been disabled.
            // This should only ever be the case on Win2K since other os's don't
            // use the MC_POWER_LEVEL_6 state. (i.e. all other os's should be saving
            // the PLL_COEFF_SELECT register here all the time.
            //
            if ((pDev->Power.State == MC_POWER_LEVEL_0)
             && ((pDev->Power.DisplayState[0] != MC_POWER_LEVEL_6)  
             &&  (pDev->Power.DisplayState[1] != MC_POWER_LEVEL_6)))
                pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
            
            // Before we set Hal power level, make sure that the current level is 0, else we will restore the wrong state
            // when we resume. The power level could be non-zero if we processed an APM suspend.
            if (pDev->Power.State != MC_POWER_LEVEL_0) 
            {
                nvHalMcPower(pDev, MC_POWER_LEVEL_0, pDev->Power.State); // restore full ON state
            }
            nvHalMcPower(pDev, MC_POWER_LEVEL_3, pDev->Power.State);    // save ON state, and set to power saving state
            // allow any further PLL changes to settle
            tmrDelay(pDev, 2000000);    // 2ms

            // We will always transition thru D0, so this SHOULDN'T ever be true.
            if (pDev->Power.State == MC_POWER_LEVEL_1) // if we slowed down mclk, restore it
            {
                REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
            }else if (pDev->Power.State == MC_POWER_LEVEL_4) //Have we gone through APM suspend?
            {
                // turn clocks up to avoid mem corruption when saving instance memory.
                REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);

                if (pDev->Power.MobileOperation)
                {
                    NVClktemp = pDev->Dac.HalInfo.NVClk;
                    pDev->Dac.HalInfo.NVClk = pDev->Power.PostNVCLK;
                }

                if (dacProgramNVClk(pDev) != RM_OK)
                {
                    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                }

                if (pDev->Power.MobileOperation)
                {
                    pDev->Dac.HalInfo.NVClk = NVClktemp;
                }

                REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
            }
            
            //Save away Spread Spectrum settings
            if (pDev->Power.MobileOperation) 
            {
                pDev->Power.GPIOExt = REG_RD32(NV_PCRTC_GPIO_EXT);

                osEnterCriticalCode(pDev);
                lockA = UnlockCRTC(pDev, 0);
                lockB = UnlockCRTC(pDev, 1);
                CRTC_RD(0x51, pDev->Power.cr51hA, 0);
                CRTC_RD(0x51, pDev->Power.cr51hB, 1);
                RestoreLock(pDev, 0, lockA);
                RestoreLock(pDev, 1, lockB);
                osExitCriticalCode(pDev);
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

                U032 IsPllSlowed;

                //
                // Check to see if NVCLK is currently being dynamically slowed
                // If so, temporarily disable so we can get a true time delay
                //
                // KJK: I wonder if this should be halified?
                //
                if (IsNV11(pDev))
                {
                    IsPllSlowed = REG_RD_DRF(_PBUS, _DEBUG_1, _CORE_SLOWDWN);
                    FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, 0);
                }

                pDev->Power.nvpll = REG_RD32(NV_PRAMDAC_NVPLL_COEFF);   // nvclk
                pDev->Power.vpll = REG_RD32(NV_PRAMDAC_VPLL_COEFF);   // vclk
                pDev->Power.mpll = REG_RD32(NV_PRAMDAC_MPLL_COEFF);     // mclk

                //
                // Restore PLL slowdown setting
                //
                if (IsNV11(pDev))
                    FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, IsPllSlowed);
            }

            // Dell mobile uses level 3 for hibernation only. Power goes off.
            if (pDev->Power.MobileOperation != 1)        // if not Dell
            {
                // turn clocks down to save power
                FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_SOURCE,_PROG_ALL);   // program all clocks

                if (IsNV5(pDev)) // NV5 is robust, we can really crank it down
                {
                    FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_SOURCE,_PROG_ALL);   // program all clocks
                    FLD_WR_DRF_NUM(_PRAMDAC,_NVPLL_COEFF, _PDIV, 4);    // max pwr saving is not max divisor
                    FLD_WR_DRF_NUM(_PRAMDAC,_NVPLL_COEFF, _NDIV, 4);    // pll lower limit around 4-10
                    FLD_WR_DRF_NUM(_PRAMDAC,_MPLL_COEFF, _PDIV, 5);                 // div by 32
                }
                else
                {
                    // NV10 NV clock is tied to MCLK
                    if (IsNV10(pDev)) 
                    {
                        NVClkSlow = (pDev->Dac.HalInfo.MClk * 2)/30000;   // NV10 is limited to MCLK/1.5. Number is MHz * 100
                    }
                        else
                    {
                            NVClkSlow = 6600;   // slow down to PCI clk speed
                    }

                    //XXX: Don't do the slowdown for highspeed mems (DDR)
                    if (pDev->Framebuffer.HalInfo.RamType != BUFFER_DDRAM)
                    {
                        NVClktemp = pDev->Dac.HalInfo.NVClk;
                        pDev->Dac.HalInfo.NVClk = NVClkSlow * 10000;
                        if (dacProgramNVClk(pDev) != RM_OK)
                        {
                            REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                        }
                        pDev->Dac.HalInfo.NVClk = NVClktemp;
                        FLD_WR_DRF_NUM(_PRAMDAC,_MPLL_COEFF, _PDIV, 5);                 // div by 32
                        
                        // Shut off fan only if NVCLK is slowed
                        osEnterCriticalCode(pDev);
                        dacSetFan(pDev, 0);   // turn off fan
                        osExitCriticalCode(pDev);
                    }
                }
            }            

            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_VPLL_SOURCE,_XTAL);  // use crystal

            // Power down DAC
            FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF);

            // Save a few critical registers that are going to be lost
            pDev->Master.Enable = REG_RD32(NV_PMC_ENABLE);    // gr enable gets blown away
            pDev->Master.PciNv19 = REG_RD32(NV_PBUS_PCI_NV_19);     // read AGP COMMAND

            dacDisableDac(pDev, 1);         // blank display on second head
            if (!IsNV5(pDev))               // if NV5, NVCLK is so slow, we shouldn't access CRTC
            {
                osEnterCriticalCode(pDev);
                dacDisableDac(pDev, 0);         // blank display on first head--and unslave to TV encoder
                osExitCriticalCode(pDev);
            }
            
                // Set power state in PCI config reg so BIOS can read
                FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D3_HOT);

                if (pDev->Power.MobileOperation == 1)
                {
                    // Reset the chip, in case this is WHQL doing hibernate w/o HW reset.
                    // We see occasional hangs early in VBIOS POST on NV11 mobile. This seems to prevent this.
                    REG_WR32(NV_PMC_ENABLE, 0);
                    REG_RD32(NV_PMC_ENABLE);
                    REG_RD32(NV_PMC_ENABLE);

                    REG_WR32(NV_PMC_ENABLE, -1);
                    REG_RD32(NV_PMC_ENABLE);
                    REG_RD32(NV_PMC_ENABLE);
                }
            pDev->Power.State = MC_POWER_LEVEL_3;
            break; // case power level 3
        //This case handles APM suspend
    case  MC_POWER_LEVEL_4:
        if (pDev->Power.MobileOperation) 
        {
            //Turn off DFP backlight
            if (pDev->Power.MobileOperation == 2)
            {
                dacBacklightOffMobilePanel(pDev);
            }


#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            // Unload HIRES device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = TRUE;
            // Load VGA device state
            stateNv(pDev, STATE_LOAD);
#endif

            if ((!IsNV10(pDev)) && (!IsNV15(pDev))) // NV10 runs too hot in standby to shut off the fan
            {
                osEnterCriticalCode(pDev);
                dacSetFan(pDev, 0);   // turn off fan
                osExitCriticalCode(pDev);
            }

            REG_WR32(NV_PMC_INTR_EN_0, 0);                      // disable all interrupts. We're going to be shut down!
            REG_WR32(NV_PMC_INTR_0, REG_RD32(NV_PMC_INTR_0));   // clear any pending ints
            pDev->DeviceDisabled = TRUE;                      // signal ISR to ignore
            
            pDev->Power.strapInfo = REG_RD32(NV_PEXTDEV_BOOT_0);    // READ STRAPS
            //
            // On some architectures it's not always the case that we will get
            // to D3 having come through D1. On the Mac we may get here from
            // D0. If that is the case we should save the power_coeff here.
            //
            // Also do NOT save the COEFF_SELECT here if Power.DisplayState is currently
            // MC_POWER_LEVEL_6 (off) because it's already been saved.
            // been saved. Doing so again could result in an incorrect value
            // being read since the dac has already been disabled.
            // This should only ever be the case on Win2K since other os's don't
            // use the MC_POWER_LEVEL_6 state. (i.e. all other os's should be saving
            // the PLL_COEFF_SELECT register here all the time.
            //
            if ((pDev->Power.State == MC_POWER_LEVEL_0)
             && ((pDev->Power.DisplayState[0] != MC_POWER_LEVEL_6)  
             &&  (pDev->Power.DisplayState[1] != MC_POWER_LEVEL_6)))
                pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
            
            // Before we set Hal power level, make sure that the current level is 0, else we will restore the wrong state
            // when we resume. The power level could be non-zero if we processed an APM suspend.
            if (pDev->Power.State != MC_POWER_LEVEL_0) 
            {
                nvHalMcPower(pDev, MC_POWER_LEVEL_0, pDev->Power.State); // restore full ON state
            }
            nvHalMcPower(pDev, MC_POWER_LEVEL_3, pDev->Power.State);    // save ON state, and set to power saving state
            // allow any further PLL changes to settle
            tmrDelay(pDev, 2000000);    // 2ms

            // We will always transition thru D0, so this SHOULDN'T ever be true.
            if (pDev->Power.State == MC_POWER_LEVEL_1) // if we slowed down mclk, restore it
            {
                REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
            }else if (pDev->Power.State == MC_POWER_LEVEL_4) //Have we gone through APM suspend?
            {
                // turn clocks up to avoid mem corruption when saving instance memory.
                REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);

                if (pDev->Power.MobileOperation)
                {
                    NVClktemp = pDev->Dac.HalInfo.NVClk;
                    pDev->Dac.HalInfo.NVClk = pDev->Power.PostNVCLK;
                }

                if (dacProgramNVClk(pDev) != RM_OK)
                {
                    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                }

                if (pDev->Power.MobileOperation)
                {
                    pDev->Dac.HalInfo.NVClk = NVClktemp;
                }

                REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
            }
            
            //Save away Spread Spectrum settings
            if (pDev->Power.MobileOperation) 
            {
                pDev->Power.GPIOExt = REG_RD32(NV_PCRTC_GPIO_EXT);

                osEnterCriticalCode(pDev);
                lockA = UnlockCRTC(pDev, 0);
                lockB = UnlockCRTC(pDev, 1);
                CRTC_RD(0x51, pDev->Power.cr51hA, 0);
                CRTC_RD(0x51, pDev->Power.cr51hB, 1);
                RestoreLock(pDev, 0, lockA);
                RestoreLock(pDev, 1, lockB);
                osExitCriticalCode(pDev);
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

                U032 IsPllSlowed;

                //
                // Check to see if NVCLK is currently being dynamically slowed
                // If so, temporarily disable so we can get a true time delay
                //
                // KJK: I wonder if this should be halified?
                //
                if (IsNV11(pDev))
                {
                    IsPllSlowed = REG_RD_DRF(_PBUS, _DEBUG_1, _CORE_SLOWDWN);
                    FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, 0);
                }

                pDev->Power.nvpll = REG_RD32(NV_PRAMDAC_NVPLL_COEFF);   // nvclk
                pDev->Power.vpll = REG_RD32(NV_PRAMDAC_VPLL_COEFF);   // vclk
                pDev->Power.mpll = REG_RD32(NV_PRAMDAC_MPLL_COEFF);     // mclk

                //
                // Restore PLL slowdown setting
                //
                if (IsNV11(pDev))
                    FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, IsPllSlowed);
            }

            // Dell mobile uses level 3 for hibernation only. Power goes off.
            if (pDev->Power.MobileOperation != 1)        // if not Dell
            {
                // turn clocks down to save power
                FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_SOURCE,_PROG_ALL);   // program all clocks

                if (IsNV5(pDev)) // NV5 is robust, we can really crank it down
                {
                    FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_SOURCE,_PROG_ALL);   // program all clocks
                    FLD_WR_DRF_NUM(_PRAMDAC,_NVPLL_COEFF, _PDIV, 4);    // max pwr saving is not max divisor
                    FLD_WR_DRF_NUM(_PRAMDAC,_NVPLL_COEFF, _NDIV, 4);    // pll lower limit around 4-10
                    FLD_WR_DRF_NUM(_PRAMDAC,_MPLL_COEFF, _PDIV, 5);                 // div by 32
                }
                else
                {
                    // NV10 NV clock is tied to MCLK
                    if (IsNV10(pDev)) 
                    {
                        NVClkSlow = (pDev->Dac.HalInfo.MClk * 2)/30000;   // NV10 is limited to MCLK/1.5. Number is MHz * 100
                    }
                        else
                    {
                            NVClkSlow = 6600;   // slow down to PCI clk speed
                    }

                    //XXX: Don't do the slowdown for highspeed mems (DDR)
                    if (pDev->Framebuffer.HalInfo.RamType != BUFFER_DDRAM)
                    {
                        NVClktemp = pDev->Dac.HalInfo.NVClk;
                        pDev->Dac.HalInfo.NVClk = NVClkSlow * 10000;
                        if (dacProgramNVClk(pDev) != RM_OK)
                        {
                            REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                        }
                        pDev->Dac.HalInfo.NVClk = NVClktemp;
                        FLD_WR_DRF_NUM(_PRAMDAC,_MPLL_COEFF, _PDIV, 5);                 // div by 32
                    }
                }
            }            

            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_VPLL_SOURCE,_XTAL);  // use crystal

            // Power down DAC
            FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF);

            // Save a few critical registers that are going to be lost
            pDev->Master.Enable = REG_RD32(NV_PMC_ENABLE);    // gr enable gets blown away
            pDev->Master.PciNv19 = REG_RD32(NV_PBUS_PCI_NV_19);     // read AGP COMMAND

            dacDisableDac(pDev, 1);         // blank display on second head
            if (!IsNV5(pDev))               // if NV5, NVCLK is so slow, we shouldn't access CRTC
            {
                osEnterCriticalCode(pDev);
                dacDisableDac(pDev, 0);         // blank display on first head--and unslave to TV encoder
                osExitCriticalCode(pDev);
            }
            
            /*if (!is_apm) 
            {
                // Set power state in PCI config reg so BIOS can read
                FLD_WR_DRF_DEF(_PBUS, _PCI_NV_25, _POWER_STATE, _D3_HOT);

                if (pDev->Power.MobileOperation == 1)
                {
                    // Reset the chip, in case this is WHQL doing hibernate w/o HW reset.
                    // We see occasional hangs early in VBIOS POST on NV11 mobile. This seems to prevent this.
                    REG_WR32(NV_PMC_ENABLE, 0);
                    REG_RD32(NV_PMC_ENABLE);
                    REG_RD32(NV_PMC_ENABLE);

                    REG_WR32(NV_PMC_ENABLE, -1);
                    REG_RD32(NV_PMC_ENABLE);
                    REG_RD32(NV_PMC_ENABLE);
                }
            } */
        }
        else
        {
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 4");
            // some power management systems (NEC) will resume when they see our interrupt, so disable timer and vblank
            FLD_WR_DRF_DEF(_PCRTC, _INTR_EN_0, _VBLANK, _DISABLED);
            FLD_WR_DRF_DEF(_PTIMER, _INTR_EN_0, _ALARM, _DISABLED);
            // some may not turn off the monitor
            data16 = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A);
            WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016) (data16 | 0x8000));     // set Hsync to inactive

            // add PM for APM

            // It appears we can get both ACPI and APM calls, so only save the
            // PLL settings and lengthen the memory refresh, if we're not already
            // in a power down state.

            // turn clocks down to save power, set PCLK to use the crystal, increase mem refresh, shut off DAC
            pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
            pDev->Power.nvpll = REG_RD32(NV_PRAMDAC_NVPLL_COEFF);   // nvclk
            pDev->Power.vpll = REG_RD32(NV_PRAMDAC_VPLL_COEFF);     // vclk
            pDev->Power.mpll = REG_RD32(NV_PRAMDAC_MPLL_COEFF);     // mclk

            nvHalMcPower(pDev, MC_POWER_LEVEL_4, pDev->Power.State);

            // Stop fetching video mem
            // On NV10, this is not just to save power; refresh does not have priority,
            // so when we turn the clock down, the memory will get starved because refresh
            // is held off because scanout is using up the bandwidth.
            data16 = ReadIndexed(NV_PRMVIO_SRX, 0x01);
            data16 |= 0x2000;   // screen off
            WriteIndexed(NV_PRMVIO_SRX, data16);

            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_SOURCE,_PROG_ALL);   // program all clocks

            // We can lower NVCLK to 66 MHz. When we do this, we also should slow down data going
            // across PCI and AGP. 

            // We won't even attempt to lower MCLK. It causes memory corruption on NV10.

            // When lowering NVCLK, also throttle down PCI and AGP
            FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _PCIM_THROTTLE, _ENABLED);        // nvclk < PCI
            FLD_WR_DRF_DEF(_PBUS, _DEBUG_3, _AGP_4X_NVCLK, _SLOW);            // nvclk < AGP

            // NV5 and NV10 NV clocks are tied to MCLK
            if (IsNV15orBetter(pDev)) 
                NVClkSlow = 6600;   // slow down to PCI clk speed
            else
                NVClkSlow = (pDev->Dac.HalInfo.MClk * 2)/30000;   // NV5/NV10 are limited to MCLK/1.5. Number is MHz * 100

            //XXX: Don't do the slowdown for highspeed mems on NV10 (DDR and Quadro)
            if (pDev->Framebuffer.HalInfo.RamType != BUFFER_DDRAM)
            {
                NVClktemp = pDev->Dac.HalInfo.NVClk;
                pDev->Dac.HalInfo.NVClk = NVClkSlow * 10000;
                if (dacProgramNVClk(pDev) != RM_OK)
                {
                    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                }
                pDev->Dac.HalInfo.NVClk = NVClktemp;
            }

            FLD_WR_DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_VPLL_SOURCE,_XTAL);  // use crystal
            FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF);
        }
            pDev->Power.State = MC_POWER_LEVEL_4;
            break;
/*
 * MC_POWER_LEVEL_5 AND MC_POWER_LEVEL_6 HAVE MOVED TO THE NEW ROUTINE mcPowerDisplayState BELOW
 * I HAVE LEFT THEM HERE ONLY OUT OF PARANOIA OF BREAKING A DIFFERENT OS, HOWEVER I BELIEVE ONLY WIN2K
 * USES THEM.  HOPEFULLY THEY CAN BE REMOVED AT A LATER DATE.
 */
        case MC_POWER_LEVEL_5:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 5");

            // turn DAC on
            dacEnableDac(pDev, Head);

            if (pDev->Power.MobileOperation == 2)
            {
                dacBacklightOnMobilePanel(pDev);
                pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;

                osEnterCriticalCode(pDev);
                if ((GETDISPLAYTYPE(pDev, 0) == DISPLAY_TYPE_FLAT_PANEL) ||
                    (GETDISPLAYTYPE(pDev, 1) == DISPLAY_TYPE_FLAT_PANEL))
                {
                    U032 prevHead;
                    //
                    // Tosh issues:
                    //
                    // Make sure CR17[1:0] and SR1[5] are restored correctly.
                    // They may have been munged when we disabled the backlight.
                    //
                    CRTC_RD(NV_CIO_CR_MODE_INDEX, data, Head);
                    data |= 0x83;
                    CRTC_WR(NV_CIO_CR_MODE_INDEX, data, Head);

                    prevHead = (pDev->Dac.HalInfo.Head2RegOwner & 0x1);
                    EnableHead(pDev, Head);        // sequencer updates require setting CR44 

                    data16 = ReadIndexed(NV_PRMVIO_SRX, 0x01);
                    data16 &= ~0x2000;
                    WriteIndexed(NV_PRMVIO_SRX, data16);

                    EnableHead(pDev, prevHead);    // restore previous CR44 value
                }
                else if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
                {
                    U032 prevHead;
                    //
                    // Tosh issues:
                    //
                    // For TV, we're also seeing where SR1[5] is not being restored
                    // correctly, so we'll fix this by hand also.
                    //
                    prevHead = (pDev->Dac.HalInfo.Head2RegOwner & 0x1);
                    EnableHead(pDev, Head);        // sequencer updates require setting CR44 

                    data16 = ReadIndexed(NV_PRMVIO_SRX, 0x01);
                    data16 &= ~0x2000;
                    WriteIndexed(NV_PRMVIO_SRX, data16);

                    EnableHead(pDev, prevHead);    // restore previous CR44 value
                }
                osExitCriticalCode(pDev);
            }

            // 
            // This should be done in dacEnableTV to complement dacDisableTV.
            // 
            // But, dacEnableTV is called early in the modeset code possibly before the
            // encoder is fully setup and able slave the CRTC off of it.
            //
            // Here, we only need to offset the little bit of dacDisableTV when resuming
            // from POWER_LEVEL_6. We'll take the safer approach and include it here. In
            // the future, we might create DAC routines specific to standby/resume.
            //
            if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
            {
                // wait for previous dacEnableTV to get the encoder powered up
                tmrDelay(pDev, 200000000);   // delay 200 ms

                // move the clocking input source back to the TV
                REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);

                osEnterCriticalCode(pDev);

                // reslave the CRTC back to the TV
                lock = UnlockCRTC(pDev, Head);

                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data08, Head);
                data08 |= 0x80;
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data08, Head);

                RestoreLock(pDev, Head, lock);

                osExitCriticalCode(pDev);
            }

            // flag that the sync state has not been set
            pDev->Dac.CrtcInfo[Head].SyncStateIsSaved = FALSE;

            pDev->Power.State = MC_POWER_LEVEL_0;    // back to LEVEL_0, not LEVEL_5
            break;

        case MC_POWER_LEVEL_6:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 6");

            // store away the sync states only on the first call
            if (!pDev->Dac.CrtcInfo[Head].SyncStateIsSaved)
            {
                pDev->Dac.CrtcInfo[Head].CurrentVsyncState = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, Head);
                pDev->Dac.CrtcInfo[Head].CurrentHsyncState = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, Head);
                pDev->Dac.CrtcInfo[Head].SyncStateIsSaved = TRUE;
            }

            if (pDev->Power.State == MC_POWER_LEVEL_0) 
            {
                // save PLL_COEFF_SELECT (cleared by dacDisableDac/dacDisableTV)
                pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
            }

            //Turn off DFP backlight
            if (pDev->Power.MobileOperation == 2)
            {
                dacBacklightOffMobilePanel(pDev);

                //
                // Tosh issues:
                //
                // After this call to turn off the backlight, SR1 and
                // CR17 have been cleared!  We'll refresh the values
                // in POWER_LEVEL_5 when we turn the backlight back on. 
                //
            }

            // turn DAC off
            osEnterCriticalCode(pDev);
            dacDisableDac(pDev, Head);
            osExitCriticalCode(pDev);

            pDev->Power.State = MC_POWER_LEVEL_6;
            break;

        case MC_POWER_LEVEL_7:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 7");
            
            //
            // This is win2k hibernate mode.
            //
            // Msoft dictates that power to the chip (including
            // the dac) remain on but the state of the chip should
            // be saved/restored.
            // 
#ifdef KERNEL_PM_STATE_CHANGE   // Do in kernel state changes which used to be done in OS-specific section
            // Unload HIRES device state
            stateNv(pDev, STATE_UNLOAD);
            pDev->Vga.Enabled = TRUE;
            // Load VGA device state
            stateNv(pDev, STATE_LOAD);
#endif
            //
            // Save off any neccessary agp chipset registers
            //
            NvSaveRestoreAGPChipset(pDev, MC_POWER_LEVEL_7);

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
                
                if (pDev->Power.MobileOperation)
                {
                    NVClktemp = pDev->Dac.HalInfo.NVClk;
                    pDev->Dac.HalInfo.NVClk = pDev->Power.PostNVCLK;
                }

                if (dacProgramNVClk(pDev) != RM_OK)
                {
                    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                }

                if (pDev->Power.MobileOperation)
                {
                    pDev->Dac.HalInfo.NVClk = NVClktemp;
                }

                REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
            }
            
            //Save away Spread Spectrum settings
            if (pDev->Power.MobileOperation) 
            {
                pDev->Power.GPIOExt = REG_RD32(NV_PCRTC_GPIO_EXT);

                osEnterCriticalCode(pDev);
                lockA = UnlockCRTC(pDev, 0);
                lockB = UnlockCRTC(pDev, 1);
                CRTC_RD(0x51, pDev->Power.cr51hA, 0);
                CRTC_RD(0x51, pDev->Power.cr51hB, 1);
                RestoreLock(pDev, 0, lockA);
                RestoreLock(pDev, 1, lockB);
                osExitCriticalCode(pDev);
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
                U032 IsPllSlowed;

                //
                // Check to see if NVCLK is currently being dynamically slowed
                // If so, temporarily disable so we can get a true time delay
                //
                // KJK: I wonder if this should be halified?
                //
                if (IsNV11(pDev))
                {
                    IsPllSlowed = REG_RD_DRF(_PBUS, _DEBUG_1, _CORE_SLOWDWN);
                    FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, 0);
                }

                pDev->Power.nvpll = REG_RD32(NV_PRAMDAC_NVPLL_COEFF);   // nvclk
                pDev->Power.vpll = REG_RD32(NV_PRAMDAC_VPLL_COEFF);   // vclk
                pDev->Power.mpll = REG_RD32(NV_PRAMDAC_MPLL_COEFF);     // mclk

                //
                // Restore PLL slowdown setting
                //
                if (IsNV11(pDev))
                    FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, IsPllSlowed);
            }

            // On some architectures it's not always the case that we will get
            // to D3 having come through D1. On the Mac we may get here from
            // D0. If that is the case we should save the power_coeff here.
            //
            // Also do NOT save the COEFF_SELECT here if Power.DisplayState is currently
            // MC_POWER_LEVEL_6 (off) because it's already been saved.
            // been saved. Doing so again could result in an incorrect value
            // being read since the dac has already been disabled.
            // This should only ever be the case on Win2K since other os's don't
            // use the MC_POWER_LEVEL_6 state. (i.e. all other os's should be saving
            // the PLL_COEFF_SELECT register here all the time.
            //
            if ((pDev->Power.State == MC_POWER_LEVEL_0) 
             && ((pDev->Power.DisplayState[0] != MC_POWER_LEVEL_6)  
             &&  (pDev->Power.DisplayState[1] != MC_POWER_LEVEL_6)))
                pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);

            // Save a few critical registers that are going to be lost
            pDev->Master.Enable = REG_RD32(NV_PMC_ENABLE);    // gr enable gets blown away
            pDev->Master.PciNv19 = REG_RD32(NV_PBUS_PCI_NV_19);     // read AGP COMMAND

            pDev->Power.State = MC_POWER_LEVEL_7;
            break; // case power level 7
    }
    return RM_OK;
}
//  Transition between display power states
//  States are:
//  Level 5 - monitor on
//  Level 6 - monitor off
RM_STATUS mcPowerDisplayState
(
    PHWINFO pDev,
    U032    state,
    U032    Head
)
{
    U008                        lock, data08;
    U016                        data16;
    U032                        data;
    U032                        DisplayType;
    BOOL                        overrideDacClass = FALSE;
    VIDEO_LUT_CURSOR_DAC_OBJECT *pTempDacClass = NULL;

    //
    // This routine relies on knowing what type of device should be enabled
    // for this head.  However, on occassion (such as Standby->Hibernation
    // transitions on win2k) the DisplayType stored is 'none'.  In this case
    // determine what the current connected display type is and use that instead.
    //
    DisplayType = GETDISPLAYTYPE(pDev, Head);
    if (DisplayType == DISPLAY_TYPE_NONE)
    {
        switch (GETMONITORTYPE(pDev, Head))
        {
            case MONITOR_TYPE_VGA:
                DisplayType = DISPLAY_TYPE_MONITOR;
                break;
            case MONITOR_TYPE_NTSC:
            case MONITOR_TYPE_PAL:
                DisplayType = DISPLAY_TYPE_TV;
                break;
            case MONITOR_TYPE_FLAT_PANEL:
                DisplayType = DISPLAY_TYPE_FLAT_PANEL;
                break;
            default:
                DisplayType = DISPLAY_TYPE_MONITOR;
                break;

        }
    }
    /*
     * Need some special hackery for the case when transitioning from Standby->Hibernation.
     * In this situation, the driver doesn't not ever set a hi-res mode, therefore there are
     * no current VideoLutCursorDac class objects in use.  This is a problem for the lower
     * level routines in the RM for enabling and disabling the dac because they rely on a
     * valid VideoLutCursorDac class object to determine what the display type is. This can
     * result in either the hibernation screen not showing up or internal flat panels blooming.
     * To get around this, whenever a call comes through where there isn't a valid VideoLutCursorDac
     * class object, we'll build up a fake one that has enough information in it that the rm can
     * get the job done "correctly".
     * This fake DacClass has to be Nonpaged and zeroed out.  Interrupt could happen (and very 
     * likely to happen) inside this routine.
     */
    if (!pDev->Dac.CrtcInfo[Head].pVidLutCurDac)
    {
        if (osAllocMem((VOID **)&pTempDacClass, sizeof(VIDEO_LUT_CURSOR_DAC_OBJECT)) == 0)
        {
            U032    count;

            osMemSet(pTempDacClass, 0, sizeof(VIDEO_LUT_CURSOR_DAC_OBJECT));
            pTempDacClass->DisplayType = DisplayType;
            overrideDacClass = TRUE;
            pDev->Dac.CrtcInfo[Head].pVidLutCurDac = (VOID_PTR)pTempDacClass;
            /*
             * In the case where we are transitioning from Standby->Hibernation, the driver
             * doesn't ever set a hi-res mode. Win2k will only set a vga mode for displaying
             * the hibernation screen.  However, if we just turn the display on without first
             * clearing the first 64K of memory, there may be garbage left in memory that will
             * be displayed momentarily before the hibernation screen is drawn. To prevent this
             * clear out the first 64K of video memory now.
             */
            for (count = 0; count < (64*1024); count += 4)
                FB_WR32(count, 0);
        }
    }

    switch (state)
    {
        // Monitor On
        case MC_POWER_LEVEL_5:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 5");

//            if (pDev->Power.DisplayState[Head] != MC_POWER_LEVEL_5)
            {
                // turn DAC on
                dacEnableDac(pDev, Head);

                if (pDev->Power.MobileOperation == 2)
                {
                    U032 prevHead;

                    switch (DisplayType)
                    {
                        case DISPLAY_TYPE_MONITOR:
                            break;
                        case DISPLAY_TYPE_FLAT_PANEL:
                            if (!overrideDacClass)
                            {
                                osEnterCriticalCode(pDev);
                                //
                                // Tosh issues:
                                //
                                // Make sure CR17[1:0] and SR1[5] are restored correctly.
                                // They may have been munged when we disabled the backlight.
                                //
                                CRTC_RD(NV_CIO_CR_MODE_INDEX, data, Head);
                                data |= 0x83;
                                CRTC_WR(NV_CIO_CR_MODE_INDEX, data, Head);
                                //
                                // sequencer updates require setting CR44
                                // Save off current owner for restore later.
                                prevHead = (pDev->Dac.HalInfo.Head2RegOwner & 0x1);
                                EnableHead(pDev, Head);

                                data16 = ReadIndexed(NV_PRMVIO_SRX, 0x01);
                                data16 &= ~0x2000;
                                WriteIndexed(NV_PRMVIO_SRX, data16);

                                // restore previous CR44 value
                                EnableHead(pDev, prevHead);

                                osExitCriticalCode(pDev);
                            }
                            break;
                        case DISPLAY_TYPE_TV:
                            osEnterCriticalCode(pDev);
                            //
                            // Tosh issues:
                            //
                            // For TV, we're also seeing where SR1[5] is not being restored
                            // correctly, so we'll fix this by hand also.
                            //
                            prevHead = (pDev->Dac.HalInfo.Head2RegOwner & 0x1);
                            EnableHead(pDev, Head);        // sequencer updates require setting CR44

                            data16 = ReadIndexed(NV_PRMVIO_SRX, 0x01);
                            data16 &= ~0x2000;
                            WriteIndexed(NV_PRMVIO_SRX, data16);

                            EnableHead(pDev, prevHead);    // restore previous CR44 value

                            osExitCriticalCode(pDev);
                            break;
                    }
                }
                if (pDev->Power.MobileOperation == 2)
                {
                    dacBacklightOnMobilePanel(pDev);
                }
                //
                // This should be done in dacEnableTV to complement dacDisableTV.
                //
                // But, dacEnableTV is called early in the modeset code possibly before the
                // encoder is fully setup and able slave the CRTC off of it.
                //
                // Here, we only need to offset the little bit of dacDisableTV when resuming
                // from POWER_LEVEL_6. We'll take the safer approach and include it here. In
                // the future, we might create DAC routines specific to standby/resume.
                //
                if (DisplayType == DISPLAY_TYPE_TV)
                {
                    // wait for previous dacEnableTV to get the encoder powered up
                    tmrDelay(pDev, 200000000);   // delay 200 ms
                    //
                    // move the clocking input source back to the TV
                    // but only restore if saved value is non-zero, it's possible that we
                    // got here without actually having ever saved this state off.
                    // For instance if you start win2k on the tv and shut down or restart
                    // without ever having gone into a power save mode, pdev->Power.coeff_select
                    // will not have been initialized.  It would probably be better if this state
                    // was updated any time it was changed. But for now just assume that if the
                    // value is zero, then the current state is valid and what we want.
                    //
                    if(pDev->Power.coeff_select)
                        REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);

                    osEnterCriticalCode(pDev);

                    // reslave the CRTC back to the TV
                    lock = UnlockCRTC(pDev, Head);

                    CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data08, Head);
                    data08 |= 0x80;
                    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data08, Head);

                    RestoreLock(pDev, Head, lock);


                    osExitCriticalCode(pDev);
                }

                // flag that the sync state has not been set
                pDev->Dac.CrtcInfo[Head].SyncStateIsSaved = FALSE;

                // Set current monitor power state for this head.
                pDev->Power.DisplayState[Head] = MC_POWER_LEVEL_5;
            }
            break;

        // Monitor Off
        case MC_POWER_LEVEL_6:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Power Level 6");

//            if (pDev->Power.DisplayState[Head] != MC_POWER_LEVEL_6)
            {
                // store away the sync states only on the first call
                if (!pDev->Dac.CrtcInfo[Head].SyncStateIsSaved)
                {
                    pDev->Dac.CrtcInfo[Head].CurrentVsyncState = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, Head);
                    pDev->Dac.CrtcInfo[Head].CurrentHsyncState = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, Head);
                    pDev->Dac.CrtcInfo[Head].SyncStateIsSaved = TRUE;
                }
                //
                // Save the PLL_COEFF_SELECT only if adapter is at MC_POWER_LEVEL_0
                // and we haven't already been through the MC_POWER_LEVEL_6 code already.
                // Don't want to end up saving a bogus value if we've already been through
                // this code once.
                //
                if ((pDev->Power.State == MC_POWER_LEVEL_0)
                 && ((pDev->Power.DisplayState[0] != MC_POWER_LEVEL_6)
                 &&  (pDev->Power.DisplayState[1] != MC_POWER_LEVEL_6)))
                {
                    // save PLL_COEFF_SELECT (cleared by dacDisableDac/dacDisableTV)
                    pDev->Power.coeff_select = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
                }
                if (pDev->Power.MobileOperation == 2)
                {
                    switch (DisplayType)
                    {
                        case DISPLAY_TYPE_MONITOR:
                            break;
                        case DISPLAY_TYPE_FLAT_PANEL:
                            dacBacklightOffMobilePanel(pDev);
                            break;
                        case DISPLAY_TYPE_TV:
                            break;
                    }
                    //
                    // Tosh issues:
                    //
                    // After this call to turn off the backlight, SR1 and
                    // CR17 have been cleared!  We'll refresh the values
                    // in POWER_LEVEL_5 when we turn the backlight back on.
                    //
                }

                // turn DAC off
                osEnterCriticalCode(pDev);
                dacDisableDac(pDev, Head);
                osExitCriticalCode(pDev);

                pDev->Power.DisplayState[Head] = MC_POWER_LEVEL_6;
            }
            break;
    }
    if (overrideDacClass)
    {
        pDev->Dac.CrtcInfo[Head].pVidLutCurDac = (VOID_PTR)NULL;
        osFreeMem(pTempDacClass);
    }
    return RM_OK;
}

