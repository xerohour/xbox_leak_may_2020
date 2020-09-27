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
* Module: MCSTATE.C                                                         *
*   The Master Control state is changed in this module.                     *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <mc.h>
#include <os.h>
#include "nvhw.h"
#include "nvcm.h"
#include <bios.h>   //For hotkey defs.  Can remove once those are put in nv_ref.h

// Registry access
extern char strDevNodeRM[];
extern VOID dbgDumpMTRRs(VOID);

//
// Change Master Control hardware state.
//
RM_STATUS stateMc
(
    PHWINFO pDev, // handle to an nv device
    U032 msg
)
{
    RM_STATUS status;
    U032      data,temp;
    
    switch (msg)
    {
        case STATE_UPDATE:
            break;
        case STATE_LOAD:
            //
            // Enable all devices and interrupts.
            //
            // We must toggle the AGP COMMAND register to make sure that if we went thru a power standby,
            // AGP will be reset and send all command packets, not just those it thinks are not cached.
            // We only need to transition the SBA/AGP enable bits to reset NV for this case.
            //
            status = RM_OK;
            data = REG_RD32(NV_PBUS_PCI_NV_19);     // read AGP COMMAND
            REG_WR32(NV_PBUS_PCI_NV_19,
                     (data & ~(DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, _ON) |
                               DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_AGP_ENABLE, _ON))));
            temp = REG_RD32(NV_PRAMIN_DATA032(0));  // read inst mem to delay
            REG_WR32(NV_PBUS_PCI_NV_19, data);      // restore

            REG_WR32(NV_PMC_ENABLE, 0xFFFFFFFF);    // enable all engines
            REG_WR32(NV_PMC_INTR_EN_0, pDev->Chip.IntrEn0);    // setup in mcinit

#if defined(DEBUG) && !defined(IA64)
            // At times, it may be useful to verify the processors MTRRs
            dbgDumpMTRRs();
#endif
            if (status != RM_OK)
            {
                return (status);
            }

            break;
        case STATE_UNLOAD:
            //
            // Clear interrupts.
            //
            REG_WR32(NV_PMC_INTR_EN_0, 0x00000000);
            break;
        case STATE_INIT:
            //
            // We must determine if there is any customer-specific code
            // we need to enable for this device.  Start the customer
            // coding at generic (reference drivers), and then override
            // it with information from the registry  
            //
            pDev->Video.HalInfo.CustomizationCode = CUSTOMER_GENERIC;
            if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "CustomizationCode", &data) == RM_OK)
                pDev->Video.HalInfo.CustomizationCode = data;

            //
            // Make sure we can find NV before partying on it.
            //
            status = initMapping(pDev);
            if (status)
                return (status);

            //
            // One time initialization.
            //
            status = initMc(pDev);
            if (status)
                return (status);
            break;
        case STATE_DESTROY:
            nvHalMcControl(pDev, MC_CONTROL_DESTROY);
            break;
    }    
    return (RM_OK);
}

RM_STATUS initMc
(
    PHWINFO pDev
)
{
    PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;
    U032 data;
    U008 lock;

    //
    // Set the PCI Device ID's
    //
    pDev->Chip.HalInfo.PCIDeviceID = REG_RD32(NV_PBUS_PCI_NV_0);
    pDev->Chip.HalInfo.PCISubDeviceID = REG_RD32(NV_PBUS_PCI_NV_11);

    //
    // Call into HAL to chip-specific initialization.
    //
    nvHalMcControl(pDev, MC_CONTROL_INIT);

    //
    // Enable the interrupt mapping within the chip
    //
    switch(pDev->Mapping.IntPin)
    {
        case 0x01:
        case 0x0A:
            pDev->Chip.IntrEn0 = DRF_DEF(_PMC, _INTR_EN_0, _INTA, _HARDWARE);
            break;
        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Bad initialization table!");
            DBG_BREAKPOINT();
            return(RM_ERROR);
    }

    //
    // Determine the current bus
    //
#ifdef IKOS
    //For IKOS just set bus type to PCI.
    pDev->Chip.Bus = BUS_PCI;
#else

    //
    // Instead of reading the strap, walk the PCI config space.  See if there is an AGP cap
    // after the power mgmt cap.
    //
    if (REG_RD_DRF(_PBUS, _PCI_NV_24, _NEXT_PTR) == NV_PBUS_PCI_NV_24_NEXT_PTR_AGP)
        pDev->Chip.Bus = BUS_AGP;
    else 
        pDev->Chip.Bus = BUS_PCI;
#endif // IKOS

    //
    // Set our flag indicating if AGP fast-writes are enabled for both NV10
    // and the chipset ... only A03 silicon or better has working FW support.
    //
    pRmInfo->AGP.AGPFWEnable = (IsNV10MaskRevA03orBetter(pDev) &&
                                REG_RD_DRF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE));

    //
    // Set our flag indicating if AGP fast-writes are at full speed or half speed.
    // Unfortunately, NV15 needs to run in NV10 crippled mode due to a compatibility
    // issue with Camino (and, unfortunately, it's not a selectable NV15 setting).
    // For NV11, if PBUS_DEBUG_1_AGPFW_ADIS is set, then we're not using NV10 compat
    // and do have fullspeed fastwrites (currently only settable through the registry.
    //
    pRmInfo->AGP.AGPFullSpeedFW = (pRmInfo->AGP.AGPFWEnable &&
                                   IsNV11orBetter(pDev) && 
                                   REG_RD_DRF(_PBUS, _DEBUG_1, _AGPFW_ADIS));

    //
    // convert HAL graphics caps, set in MC_CONTROL_INIT, to API values
    //
    pDev->Graphics.Capabilities = 0;
    
    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_UNKNOWN)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_UNKNOWN;
  
    if ((pDev->Graphics.HalInfo.Capabilities >> NVHAL_GRAPHICS_CAPS_MAXCLIPS_SHIFT)
                                                & NVHAL_GRAPHICS_CAPS_MAXCLIPS_MASK)
    {
        U032 numClips = (pDev->Graphics.HalInfo.Capabilities
                            >> NVHAL_GRAPHICS_CAPS_MAXCLIPS_SHIFT)
                            & NVHAL_GRAPHICS_CAPS_MAXCLIPS_MASK;

        pDev->Graphics.Capabilities |= ((numClips & NV_CFG_GRAPHICS_CAPS_MAXCLIPS_MASK)
                                        << NV_CFG_GRAPHICS_CAPS_MAXCLIPS_SHIFT);
    } 
    
    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_AA_LINES)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_AA_LINES;
   
    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_AA_POLYS)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_AA_POLYS;
   
    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_AKILL_USERCLIP)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_AKILL_USERCLIP;
   
    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_LOGIC_OPS)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_LOGIC_OPS;
   
    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_NV15_ALINES)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_NV15_ALINES;

    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_2SIDED_LIGHTING)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_2SIDED_LIGHTING;

    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_QUADRO_GENERIC)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC;

    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE;

    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_UBB)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_UBB;

    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_3D_TEXTURES)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_3D_TEXTURES;

    if (pDev->Graphics.HalInfo.Capabilities & NVHAL_GRAPHICS_CAPS_ANISOTROPIC)
        pDev->Graphics.Capabilities |= NV_CFG_GRAPHICS_CAPS_ANISOTROPIC;

    if (pDev->Graphics.Capabilities == 0)
        pDev->Graphics.Capabilities = 0x1;

    //
    // Set the BIOS revision and check the RM DWORD for driver compatibility
    //
    if (mcSetBiosRevision(pDev) != RM_OK)
        return (RM_ERROR);

    //
    // Temporary workaround to disable twinview functionality via a registry key
    //
    // Note the original count was determined down in the HAL routine
    //
    // AS SOON AS THE BIOS->RESMAN DISPLAY TABLE IS IN PLACE, REMOVE THIS OVERRIDE
    //
    if (IsNV11(pDev))
        if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "TwinViewDisabled", &data) == RM_OK)
            pDev->Dac.HalInfo.NumCrtcs = 1;


    //
    // Determine if we're running this device in a mobile environment.  This
    // will enable various power saving and device functionality that is not
    // available with desktop devices.
    //
    {
        pDev->Power.MobileOperation = 0;
        pDev->Power.PostNVCLK = 0;
        pDev->Power.PostMCLK = 0;

        pDev->Power.PmNVCLK[0] = 0;         // Full NVClk
        pDev->Power.PmMCLK[0] = 0;          // Full MClk 

        pDev->Power.PmNVCLK[1] = 102000000; // 102 MHz
        pDev->Power.PmMCLK[1]  = 0;         // Full MClk

        pDev->Power.PmNVCLK[2] = 102000000; // 51 MHz
        pDev->Power.PmNVCLK[3] =  51000000; // 51 MHz
        pDev->Power.PmNVCLK[4] =  51000000; // 51 MHz

        if (pDev->Framebuffer.HalInfo.RamType == NV_CFG_RAM_TYPE_DDRRAM)
        {
            pDev->Power.PmMCLK[2]  = 166000000; // 166 MHz
            pDev->Power.PmMCLK[3]  = 166000000; // 166 MHz
            pDev->Power.PmMCLK[4]  = 120000000; // 120 MHz
        }
        else
        {
            // MClk scaling disabled on anything but DDR since it has
            // not been tested and since there are almost no DDR mobile
            // parts in the market.
            pDev->Power.PmMCLK[2]  = 0; // Full MClk
            pDev->Power.PmMCLK[3]  = 0; // Full MClk
            pDev->Power.PmMCLK[4]  = 0; // Full MClk
        }

        //Read the BIOS bit that indicates whether this is a laptop or not.
        lock = UnlockCRTC(pDev, 0);
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 0);
        RestoreLock(pDev, 0, lock);
    
        if (IsNV11(pDev) && (data & NV_IS_MOBILE_BIOS))
            pDev->Power.MobileOperation = 1;

        //
        // Just in case, check the registry for an override enable
        //
        if ((IsNV11(pDev)) &&
            (RM_OK == OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "Mobile", &data)))
            pDev->Power.MobileOperation = data;

        // initialize access to the video bios (Linux only)
        
#ifdef LINUX
        if (pDev->Power.MobileOperation)
            osInitVideoBIOS(pDev->DBmpDeviceExtension);
#endif // LINUX
        
        //
        // Check for a registry override for the Idle NVClock setting
        //
        if (IsNV11(pDev))
        {
            if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "PMNVCLOCK", &data) == RM_OK)
            {
                pDev->Power.PmNVCLK[1] = ((data >>  0) & 0xFF) * 1000000;
                pDev->Power.PmNVCLK[2] = ((data >>  8) & 0xFF) * 1000000;
                pDev->Power.PmNVCLK[3] = ((data >> 16) & 0xFF) * 1000000;
                pDev->Power.PmNVCLK[4] = ((data >> 24) & 0xFF) * 1000000;
            }
        }

        //
        // Check for a registry override for the Idle MClock setting
        //
        if (IsNV11(pDev))
        {
            U032 multiplier;

            if (pDev->Framebuffer.HalInfo.RamType == NV_CFG_RAM_TYPE_DDRRAM)
            {
                // For DDR, MCLK is in terms of pre-multiplied clock.
                multiplier = 2000000;
            }
            else
            {
                multiplier = 1000000;
            }
            if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "PMMCLOCK", &data) == RM_OK)
            {
                pDev->Power.PmMCLK[1] = ((data >>  0) & 0xFF) * multiplier;
                pDev->Power.PmMCLK[2] = ((data >>  8) & 0xFF) * multiplier;
                pDev->Power.PmMCLK[3] = ((data >> 16) & 0xFF) * multiplier;
                pDev->Power.PmMCLK[4] = ((data >> 24) & 0xFF) * multiplier;
            }
        }

        //
        // Initialize Power Mizer
        //
        pDev->Power.PowerMizerDefault = POWER_MIZER_DEFAULT;
        if (IsNV11(pDev))
            if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, NV_REG_POWER_MIZER_DEFAULT, &data) == RM_OK)
                pDev->Power.PowerMizerDefault = data;

        pDev->Power.PowerMizerLevel = pDev->Power.PowerMizerDefault;
        if (IsNV11(pDev))
            if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, NV_REG_POWER_MIZER_LEVEL, &data) == RM_OK)
                pDev->Power.PowerMizerLevel = data;

        // 
        // Just in case this is not a real mobile BIOS, make sure hw power 
        // features are enabled
        //
        //if (pDev->Power.MobileOperation)
        //{
        //  FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, 3);
        //  FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _PCIM_THROTTLE, _ENABLED);
        //}
    }

    return (RM_OK);
}

RM_STATUS mcSetPowerMizerLevel(PHWINFO pDev, U032 PMLevel)
{
    pDev->Power.PowerMizerLevel = PMLevel;
    osWriteRegistryDword(pDev, strDevNodeRM, NV_REG_POWER_MIZER_LEVEL, PMLevel);

    return (RM_OK);
}
