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
*   File: osapi.c
*
*   Description:
*       This file contains the resource manager API for the NT miniport.  It
*   provides services to the miniport that directly access the device.
*
******************************************************************************/

#include <ntddk.h>
#include <nvrm.h>
#include <nvhw.h>
#include <os.h>

BOOL RmLoadState
(
    U032 deviceReference,
    U032 horizWidth,
    U032 vertWidth,
    U032 bitDepth,
    U032 refreshRate,
    BOOL vgaEnabled
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    // allow hi res modes to happen
    // This value should be FALSE after a SetMode has occurred
    // This value should be TRUE when restoring from Hibernation (in VGA mode)
    pDev->Vga.Enabled = vgaEnabled;

    // transfer the arguments from the client into RM global state
    pDev->Dac.HalInfo.Depth             = bitDepth;
    pDev->Framebuffer.HalInfo.RefreshRate        = refreshRate;
    pDev->Framebuffer.HalInfo.HorizDisplayWidth  = horizWidth;
    pDev->Framebuffer.HalInfo.VertDisplayWidth   = vertWidth;

    // reload the state based on the new display mode
    rmStatus = stateNv(pDev, STATE_LOAD);
    
    return (rmStatus == RM_OK) ? TRUE : FALSE;
                        
} // end of RmLoadState()

BOOL RmUnloadState
(
    U032 deviceReference
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    // unload the state
    rmStatus = stateNv(pDev, STATE_UNLOAD);
    
    return (rmStatus == RM_OK) ? TRUE : FALSE;
            
} // end of RmUnloadState()


#if 0
BOOL RmUpdateAGPConfig
(
    U032 deviceReference
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    NvUpdateAGPConfig(pDev);
        
    return (rmStatus == RM_OK) ? TRUE : FALSE;
            
} // end of RmUnloadState()
#endif

BOOL RmSetMode
(
    U032 deviceReference,
    U032 HorizontalVisible,
    U032 HorizontalBlankStart,
    U032 HorizontalRetraceStart,
    U032 HorizontalRetraceEnd,
    U032 HorizontalBlankEnd,
    U032 HorizontalTotal,
    U032 VerticalVisible,
    U032 VerticalBlankStart,
    U032 VerticalRetraceStart,
    U032 VerticalRetraceEnd,
    U032 VerticalBlankEnd,
    U032 VerticalTotal,
    U032 PixelDepth,
    U032 RefreshRate,
    U032 Width,
    U032 Height,
    U032 PixelClock,
    U032 HorizontalSyncPolarity,
    U032 VerticalSyncPolarity,
    U032 DoubleScannedMode     
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    // unload the state
    rmStatus = stateDac(pDev, STATE_UNLOAD);
    if (rmStatus == RM_OK)
    {
        // set the VESA parameters in the device context
        pDev->Dac.HorizontalVisible         = HorizontalVisible;     
        pDev->Dac.HorizontalBlankStart      = HorizontalBlankStart;
        pDev->Dac.HorizontalRetraceStart    = HorizontalRetraceStart;
        pDev->Dac.HorizontalRetraceEnd      = HorizontalRetraceEnd;
        pDev->Dac.HorizontalBlankEnd        = HorizontalBlankEnd;
        pDev->Dac.HorizontalTotal           = HorizontalTotal;
        pDev->Dac.VerticalVisible           = VerticalVisible;
        pDev->Dac.VerticalBlankStart        = VerticalBlankStart;
        pDev->Dac.VerticalRetraceStart      = VerticalRetraceStart;
        pDev->Dac.VerticalRetraceEnd        = VerticalRetraceEnd;
        pDev->Dac.VerticalBlankEnd          = VerticalBlankEnd;
        pDev->Dac.VerticalTotal             = VerticalTotal;
        pDev->Dac.PixelDepth                = PixelDepth;
        pDev->Dac.HalInfo.PixelClock        = PixelClock;
        pDev->Dac.HorizontalSyncPolarity    = HorizontalSyncPolarity;
        pDev->Dac.VerticalSyncPolarity      = VerticalSyncPolarity;
        pDev->Dac.DoubleScannedMode         = DoubleScannedMode;
        pDev->Dac.DisplayPitch              = HorizontalVisible * (PixelDepth >> 3);

        // reload the state -- this will reset the mode
        pDev->Vga.Enabled = FALSE;

        // transfer the arguments from the client into RM global state
        pDev->Dac.HalInfo.Depth             = PixelDepth;
        pDev->Framebuffer.HalInfo.RefreshRate        = RefreshRate;
        pDev->Framebuffer.HalInfo.HorizDisplayWidth  = Width;
        pDev->Framebuffer.HalInfo.VertDisplayWidth   = Height;

        rmStatus = stateDac(pDev, STATE_LOAD);
        if (rmStatus == RM_OK)
            rmStatus = dacSetMode(pDev, 0);
    } 

    return (rmStatus == RM_OK) ? TRUE : FALSE;  
                        
} // end of RmSetMode()

BOOL RmIsr
(
    U032 deviceReference
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    // check for any valid NV interrupts that need to be serviced
    if (pDev)
    {
        rmStatus = osIsr(pDev);
    }

    return (rmStatus == RM_OK);

} // end of RmIsr()

BOOL RmInterruptPending
(
    PHWINFO pDev,
    BOOL *serviced
)
{
    PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;
    V032 pmc;

    // check interrupts only if the RM is initialized
    if (pOsHwInfo->rmInitialized)
    {
        // service interrupts only if they are enabled
        if (REG_RD32(NV_PMC_INTR_EN_0))
        {
            // service interrupts only if they are present
            pmc = REG_RD32(NV_PMC_INTR_0);
            if (pmc)
            {
                // disable interrupts
                RmDisableInterrupts(pDev);

                // only service interrupts locally for NT4
                #if (_WIN32_WINNT < 0x0500)
                // locally service high-priority interrupts immediately
                if (pmc & PENDING_INTERRUPTS)
                {
                    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVGFX: Servicing VBLANK interrupt...\n");
                    VBlank(pDev);
                    *serviced = TRUE;
                }
                #endif // _WIN32_WINNT >= 0x0500

                // recheck to see if any interrupts occured during local servicing
                pmc = REG_RD32(NV_PMC_INTR_0);
                if (pmc)
                {
                    // a non-VBLANK interrupt is pending, so don't enable ints yet
                    return TRUE;
                }
                else
                {
                    // enable interrupts if all were serviced locally
                    RmEnableInterrupts(pDev);
                }
            }
        }
    }

    return FALSE;

} // end of RmInterruptPending()

VOID RmEnableInterrupts
(
    PHWINFO pDev
)
{
    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVGFX: Enabling NV interrupts...\n");
    REG_WR32(NV_PMC_INTR_EN_0, pDev->Chip.IntrEn0);

} // end of RmEnableInterrupts();

VOID RmDisableInterrupts
(
    PHWINFO pDev
)
{
    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVGFX: Disabling NV interrupts...\n");
    REG_WR32(NV_PMC_INTR_EN_0, 0x00000000);

} // end of RmDisableInterrupts();

BOOL RmConfigGetKernel
(
    U032 deviceReference,
    U032 index,
    U032* pValue
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];
    
    rmStatus = stateConfigGet(pDev, index, pValue);

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of RmConfigGetKernel()

BOOL RmConfigGetExKernel
(
    U032 deviceReference,
    U032 index,
    VOID* pParms,
    U032 parmSize
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];
    
    rmStatus = stateConfigGetEx(pDev, index, pParms, parmSize);

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of RmConfigGetExKernel()


BOOL RmConfigSetKernel
(
    U032  deviceReference,
   U032  index,
   U032  newvalue,
   U032* pOldvalue
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];
    
   rmStatus = stateConfigSet(pDev, index, newvalue, pOldvalue);

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of RmConfigSetKernel()

BOOL RmConfigSetExKernel
(
    U032 deviceReference,
    U032 index,
    VOID* pParms,
    U032 parmSize
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];
    
    rmStatus = stateConfigSetEx(pDev, index, pParms, parmSize);

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of RmConfigSetExKernel()

#if 0 // (_WIN32_WINNT >= 0x0500)
VOID RmSetAgpServices
(
    U032 deviceReference,
    VOID *AgpServices
)
{
    PHWINFO pDev = NvDBPtr_Table[deviceReference];
    PRMINFO pRmInfo = (PRMINFO) NvDBPtr_Table[deviceReference]->pRmInfo;
    PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;

    RM_STATUS status;

    pOsHwInfo->AgpServices = AgpServices;

    // setup our AGP base/limit
    if (pOsHwInfo->AgpServices) {
        PHYSICAL_ADDRESS AgpPhysBase;
        VOID *AgpLinearBase;

        // Get the AGP physical base and limit
        status = NvGetAGPBaseLimit(pDev, &pRmInfo->AGP.AGPPhysStart, &pRmInfo->AGP.AGPLimit);
        if (status != RM_OK) {
            // chipset isn't configured correctly
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NvGetAGPBaseLimit call failed\n");
            pOsHwInfo->AgpServices = NULL;        // avoid AGP allocations
            return;
        }

        DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO,   "NVRM: AGP PhysBase:  ", (VOID *)pRmInfo->AGP.AGPPhysStart);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: AGP PhysLimit: ", pRmInfo->AGP.AGPLimit);

        //
        // So we're not dependent on anyone else's mapping to exist, establish our
        // own virtual addr to the AGP physbase. We only need a single page mapped
        // and type of mapping doesn't really matter. This is used only by drivers
        // wanting to setup a context DMA for the whole AGP aperture.
        //
        AgpPhysBase.HighPart = 0x00000000;
        AgpPhysBase.LowPart = (ULONG)pRmInfo->AGP.AGPPhysStart;

        AgpLinearBase = MmMapIoSpace(AgpPhysBase, 0x1000, 0x2);
        if (AgpLinearBase == NULL) {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Failed to establish AGP base mapping\n");
            return;
        }
        pRmInfo->AGP.AGPLinearStart = (VOID_PTR)AgpLinearBase;
    }
}
#endif

BOOL RmPreModeSet
(
    U032 deviceReference,
    U032 head
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];
    int i;

    pDev->Vga.Enabled = FALSE;

    // call the pre-mode set OS service
    osPreModeSetEx(pDev, head);
    
    // 
    // For a tiled primary surface, we delay setting the tiled bit in the hardware
    // until after we've modeswitched out of VGA to prevent a garbled splash screen.
    // Just in case we haven't set it yet, do it now.
    //
    for (i=0; i<MAX_CRTCS; i++) {
        if (pDev->Framebuffer.HalInfo.PrimaryFbAllocInfo[i].hwResId != 0) {
            nvHalFbSetAllocParameters(pDev, &pDev->Framebuffer.HalInfo.PrimaryFbAllocInfo[i]);
        }
    }
    return (rmStatus == RM_OK) ? TRUE : FALSE;

}

BOOL RmPostModeSet
(
    U032 deviceReference,
    U032 head
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    // call the pre-mode set OS service
    osPostModeSetEx(pDev, head);
    
    return (rmStatus == RM_OK) ? TRUE : FALSE;

}

// set a display's state: TRUE = on, FALSE = off
BOOL RmSetDisplayPowerState
(
    U032 deviceReference,
    U032 head,
    BOOL state
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    if (pDev)
    {
        if (state)
        {
            // turn DAC on
            dacEnableDac(pDev, head);
            
            // signify that the sync state has not been set
            pDev->Dac.CrtcInfo[head].SyncStateIsSaved = FALSE;
            
        }
        else
        {
            // store away the sync states only on the first call
            if (!pDev->Dac.CrtcInfo[head].SyncStateIsSaved)
            {
                pDev->Dac.CrtcInfo[head].CurrentVsyncState = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, head);
                pDev->Dac.CrtcInfo[head].CurrentHsyncState = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, head);
                pDev->Dac.CrtcInfo[head].SyncStateIsSaved = TRUE;
            }
            
            // turn DAC off
            dacDisableDac(pDev, head);
        }
    }
    else
    {
        rmStatus = RM_ERROR;
    }    

    return (rmStatus == RM_OK);

} // end of RmSetDisplayPowerState()

// set I2C ownership to the given head
BOOL RmEnableHead
(
    U032 deviceReference,
    U032 head
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    if (pDev)
    {
        EnableHead(pDev, head);
    }
    else
    {
        rmStatus = RM_ERROR;
    }    
    
    return (rmStatus == RM_OK);

} // end of RmEnableHead()

// TO DO:  these defines need to go to a file that is shared between the miniport.c and osapi.c
// power state definitions used by RmSet/GetPowerState() and RmGetCurrentPowerState()
#define NV_POWER_ADAPTER_STATE_0            0x00000001 // full on
#define NV_POWER_ADAPTER_STATE_1            0x00000002 // slowed clocks
#define NV_POWER_ADAPTER_STATE_2            0x00000004 // slowed clocks, mobile style *** not all devices supported ***
#define NV_POWER_ADAPTER_STATE_3            0x00000008 // very slow clocks, state saved (regs & instance memory)
#define NV_POWER_ADAPTER_STATE_4            0x00000010 // future lower adapter power state
#define NV_POWER_ADAPTER_STATE_5            0x00000020 // future lower adapter power state
#define NV_POWER_ADAPTER_STATE_6            0x00000040 // future lower adapter power state
#define NV_POWER_ADAPTER_STATE_7            0x00000080 // future lower adapter power state
#define NV_POWER_ADAPTER_FEATURE_CTXSAVED   0x00000100 // lowest power consumption state saves device context
#define NV_POWER_ADAPTER_FEATURE_UNDEF_1    0x00000200 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_2    0x00000400 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_3    0x00000800 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_4    0x00001000 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_5    0x00002000 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_6    0x00004000 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_7    0x00008000 // future feature
#define NV_POWER_MONITOR_STATE_0            0x00010000 // full on
#define NV_POWER_MONITOR_STATE_1            0x00020000 // no VSYNC nor HSYNC
#define NV_POWER_MONITOR_STATE_2            0x00040000 // future lower monitor power state 
#define NV_POWER_MONITOR_STATE_3            0x00080000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_4            0x00100000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_5            0x00200000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_6            0x00400000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_7            0x00800000 // future lower monitor power state
#define NV_POWER_UNDEF_0                    0x01000000 // future attribute
#define NV_POWER_UNDEF_1                    0x02000000 // future attribute
#define NV_POWER_UNDEF_2                    0x04000000 // future attribute
#define NV_POWER_UNDEF_3                    0x08000000 // future attribute
#define NV_POWER_UNDEF_4                    0x10000000 // future attribute
#define NV_POWER_UNDEF_5                    0x20000000 // future attribute
#define NV_POWER_UNDEF_6                    0x40000000 // future attribute
#define NV_POWER_UNDEF_7                    0x80000000 // future attribute


BOOL RmSetPowerState
(
    U032 deviceReference,
    U032 head,
    U032 state
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    if (pDev)
    {
        switch (state)
        {
            // ADAPTER POWER STATES
            
            //  Level 0 - full on
            case NV_POWER_ADAPTER_STATE_0:
                mcPowerState(pDev, MC_POWER_LEVEL_0);
                break;
                
            //  Level 1 - slowed clocks
            case NV_POWER_ADAPTER_STATE_1:
                mcPowerState(pDev, MC_POWER_LEVEL_1);
                break;
                
            //  Level 2 - slowed clocks, mobile style (not all devices support)
            case NV_POWER_ADAPTER_STATE_2:
                mcPowerState(pDev, MC_POWER_LEVEL_2);
                break;
                
            //  Level 3 - very slow clocks, state saved (regs & instance memory)
            case NV_POWER_ADAPTER_STATE_3:
                mcPowerState(pDev, MC_POWER_LEVEL_3);
                break;
                
                
            // DISPLAY POWER STATES
            
            // display on
            case NV_POWER_MONITOR_STATE_0:
            
                // turn DAC on
                dacEnableDac(pDev, head);
            
                // flag that the sync state has not been set
                pDev->Dac.CrtcInfo[head].SyncStateIsSaved = FALSE;
                
                // set the current monitor power state for this head
                pDev->Dac.CrtcInfo[head].CurrentPowerState = NV_POWER_MONITOR_STATE_0;
                
                break;
            
            // display off
            case NV_POWER_MONITOR_STATE_1:
            
                // store away the sync states only on the first call
                if (!pDev->Dac.CrtcInfo[head].SyncStateIsSaved)
                {
                    pDev->Dac.CrtcInfo[head].CurrentVsyncState = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, head);
                    pDev->Dac.CrtcInfo[head].CurrentHsyncState = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, head);
                    pDev->Dac.CrtcInfo[head].SyncStateIsSaved = TRUE;
                }
            
                // turn DAC off
                dacDisableDac(pDev, head);
                
                // set the current monitor power state for this head
                pDev->Dac.CrtcInfo[head].CurrentPowerState = NV_POWER_MONITOR_STATE_1;
                
                break;
                
            default:
                rmStatus = RM_ERROR;
                break;
        }
    }
    else
    {
        rmStatus = RM_ERROR;
    }    

    return (rmStatus == RM_OK);
    
} // end of RmSetPowerState()

BOOL RmGetCurrentPowerState
(
    U032 deviceReference,
    U032 head, 
    U032* pState
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    if (pDev)
    {
        // load the return state with the current adapter state
        switch (pDev->Power.State)
        {
            case MC_POWER_LEVEL_0:
                *pState = NV_POWER_ADAPTER_STATE_0;
                break;
                
            case MC_POWER_LEVEL_1:
                *pState = NV_POWER_ADAPTER_STATE_1;
                break;
                
            case MC_POWER_LEVEL_2:
                *pState = NV_POWER_ADAPTER_STATE_2;
                break;
                
            case MC_POWER_LEVEL_3:
                *pState = NV_POWER_ADAPTER_STATE_3;
                break;
        }
        
        // OR in the current monitor state
        // TO DO: use HAL to get max head count for this sanity check
        if (head < 2)
        {
            *pState |= pDev->Dac.CrtcInfo[head].CurrentPowerState;
        }
    }
    else
    {
        rmStatus = RM_ERROR;
    }    

    return (rmStatus == RM_OK);
    
} // end of RmGetCurrentPowerState()

BOOL RmGetPowerCaps
(
    U032 deviceReference,
    U032 head, 
    U032* pCaps
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    if (pDev)
    {
        // TO DO: do we need some kind of HAL call for this?
        if (pDev->Power.MobileOperation)
        {
            *pCaps = 
                NV_POWER_ADAPTER_STATE_0            |
                NV_POWER_ADAPTER_STATE_2            |
                NV_POWER_ADAPTER_STATE_3            |
                NV_POWER_ADAPTER_FEATURE_CTXSAVED   |
                NV_POWER_MONITOR_STATE_0            |
                NV_POWER_MONITOR_STATE_1;
        }
        else
        {
            *pCaps = 
                NV_POWER_ADAPTER_STATE_0            |
                NV_POWER_ADAPTER_STATE_1            |
                NV_POWER_ADAPTER_STATE_3            |
                NV_POWER_ADAPTER_FEATURE_CTXSAVED   |
                NV_POWER_MONITOR_STATE_0            |
                NV_POWER_MONITOR_STATE_1;
        }    
    }
    else
    {
        rmStatus = RM_ERROR;
    }    

    return (rmStatus == RM_OK);
    
} // end of RmGetPowerCaps()

BOOL RmSetDriverHotkeyHandling
(
    U032 deviceReference,
    BOOL enabled
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    if (pDev)
    {
        if (enabled)
        {
            EnableMobileHotkeyHandling(pDev);
        }
        else
        {
            DisableMobileHotkeyHandling(pDev);
        }
    }
    else
    {
        rmStatus = RM_ERROR;
    }    

    return (rmStatus == RM_OK);

} // end of RmSetDriverHotkeyHandling()

