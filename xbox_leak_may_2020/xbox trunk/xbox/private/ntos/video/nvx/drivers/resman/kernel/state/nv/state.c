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
 
/***************************** HW State Rotuines ***************************\
*                                                                           *
* Module: STATE.C                                                           *
*       Hardware State is managed in this module.                           *
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
#include <vga.h>
#include <os.h>
#include "nvhw.h"
#include "nvcm.h"
#include "nvcmrsvd.h"
#include "dac.h"
#include "edid.h"
#include "mvision.h"

// this is defined in nvarch.h, but I'm not sure its ok to include that here 
// on all platforms
#ifndef RM_SUCCESS
#define RM_SUCCESS(status)  ((status) == RM_OK)
#endif

#define IsAGPCapable(pdev)  (REG_RD_DRF(_PBUS, _PCI_NV_24, _NEXT_PTR) == NV_PBUS_PCI_NV_24_NEXT_PTR_AGP)

//
// Master state structure.
//
// HWINFO  nvInfo;
//
// State functions. Notice that LOAD/UNLOAD are treated as a pair.
// When an UNLOAD request arrives, the semaphore won't be released
// until a matching LOAD arrives.
//
RM_STATUS stateNv
(
    PHWINFO pDev, // Pointer to an nv device
    U032    msg
)
{
    RM_STATUS status;
//  U032      SemaState;

    status = RM_OK;
    switch (msg)
    {
        case STATE_INIT:
            if (!status) status = stateMc(pDev, msg);
            if (!status) status = stateTmr(pDev, msg);  // init timer before fb so we can do I2C
            if (!status) status = stateFb(pDev, msg);
            if (!status) status = stateDac(pDev, msg);
            if (!status) status = stateVideo(pDev, msg);
            if (!status) status = stateDma(pDev, msg);
            if (!status) status = stateGr(pDev, msg);
            if (!status) status = stateFifo(pDev, msg);
            if (!status) status = stateMp(pDev, msg);
            break;
        case STATE_UPDATE:
            //
            // This is an API routine.  Aquire semaphore.
            //
            //osWaitSemaphore(&SemaState);
        case STATE_LOAD:
            if (!status) status = stateMc(pDev, msg);
            if (!status) status = stateFb(pDev, msg);
            if (!status) status = stateDac(pDev, msg);
            if (!status) status = stateVideo(pDev, msg);
            if (!status) status = stateTmr(pDev, msg);
            if (!status) status = stateDma(pDev, msg);
            if (!status) status = stateGr(pDev, msg);
            if (!status) status = stateFifo(pDev, msg);
            if (!status) status = stateMp(pDev, msg);
            if (msg != STATE_INIT)
            {
                //
                // Don't release semaphore for INIT, wait until
                // LOAD to give up semaphore.
                //
                //osSignalSemaphore(&SemaState);
            }
            break;
        case STATE_UNLOAD:
            //
            // This is an API routine.  Aquire semaphore.
            //
            //osWaitSemaphore(&SemaState);
        case STATE_DESTROY:
            if (!status) status = stateMp(pDev, msg);
            if (!status) status = stateFifo(pDev, msg);
            if (!status) status = stateGr(pDev, msg);
            if (!status) status = stateDma(pDev, msg);
            if (!status) status = stateFb(pDev, msg);
            if (!status) status = stateTmr(pDev, msg);
            if (!status) status = stateVideo(pDev, msg);
            if (!status) status = stateDac(pDev, msg);
            if (!status) status = stateMc(pDev, msg);
            if (msg == STATE_DESTROY)
            {
                //
                // Release semaphore for DESTROY, otherwise wait until
                // LOAD to give up semaphore.
                //
                //osSignalSemaphore(&SemaState);
            }
            break;
    }
    return (status);
}

// 
// This function is used to change from one hires mode to another. It is called 
// by the VIDEO_LUT_CURSOR_DAC classes to set a new mode.
// It is assumed that all the crtc timing values are in the pDev.
//
RM_STATUS stateSetMode_DacClassStyle
(
    PHWINFO pDev
)
{
    // Tell the driver to unload state if necessary.
    osDisplayModeHook(pDev, STATE_UNLOAD);

    stateMc(pDev, STATE_UNLOAD);
    stateFb(pDev, STATE_UNLOAD);
    stateVideo(pDev,STATE_UNLOAD);
    stateDac(pDev,STATE_UNLOAD);
    stateTmr(pDev,STATE_UNLOAD);
    stateDma(pDev,STATE_UNLOAD);
    stateGr(pDev, STATE_UNLOAD);
    stateMp(pDev, STATE_UNLOAD);
    
    stateMp(pDev, STATE_LOAD);
    stateGr(pDev, STATE_LOAD);
    stateDma(pDev,STATE_LOAD);
    stateFb(pDev, STATE_LOAD);
    stateTmr(pDev,STATE_LOAD);
    stateDac(pDev,STATE_LOAD);
    stateVideo(pDev,STATE_LOAD);
    stateMc(pDev, STATE_LOAD);

    osDisplayModeHook(pDev, STATE_UNLOAD);

    return RM_OK;
} // end of stateSetMode

// 
// This function is used to change from one hires mode to another. It is called 
// by the VIDEO_LUT_CURSOR_DAC classes to set a new mode.
// It is assumed that all the crtc timing values are in the pDev.
//
RM_STATUS stateSetMode
(
    PHWINFO pDev
)
{
    // Tell the driver to unload state if necessary.
    osDisplayModeHook(pDev, STATE_UNLOAD);

    // Unload the current state.
    stateNv(pDev, STATE_UNLOAD);

    // Load the new state.
    stateNv(pDev, STATE_LOAD);

    // tell the driver to load state.
    // We should really be calling the callback function with STATE_LOAD at this time. However,
    // the display driver has worked around this bug. So don't change it till the display
    // driver can deal with us calling it with STATE_LOAD.
    osDisplayModeHook(pDev, STATE_UNLOAD);

    return RM_OK;
} // end of stateSetMode

// 
// This function is used to change from one hires mode to another. It is called 
// by the VIDEO_LUT_CURSOR_DAC classes to set a new mode.
// It is assumed that all the crtc timing values are in the pDev.
//
RM_STATUS stateSetModeMultiHead
(
    PHWINFO pDev,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pDac
)
{
    // no state changes are necessary
    dacSetModeMulti(pDev, pDac);

    return RM_OK;
} // end of stateSetMode

//---------------------------------------------------------------------------
//
//  Nv device state information.
//
//---------------------------------------------------------------------------

//
// DDK Entries to get/set system parameters
//
RM_STATUS stateConfigGet(
    PHWINFO pDev,
    U032    index,
    U032*   pValue
)
{
    PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;
    RM_STATUS rmStatus = RM_OK;
    U032 result;
   
    // 
    // Special case the CRTC access since we're using the lower byte of
    // the index to denote the register we want to view
    //
    if ((index & 0xF00) == NV_CFG_CRTC)
    {
        U008 lock, crtc_index;
        
        //
        // Save the current CRTC index
        //
        if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)    // color or mono?
            crtc_index = REG_RD08(NV_PRMCIO_CRX__COLOR);    // save crtc index
        else
            crtc_index = REG_RD08(NV_PRMCIO_CRX__MONO); // save crtc index

        // Unlock CRTC extended regs
        CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, 0);
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);

        //
        // Get the register value.
        //
        CRTC_RD((index & 0xFF), result, 0);
        
        // Relock extended regs
        if (lock == 0)
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, 0);

        // Restore index
        if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)
            REG_WR08(NV_PRMCIO_CRX__COLOR, crtc_index); // restore crtc index
        else
            REG_WR08(NV_PRMCIO_CRX__MONO, crtc_index);  // restore crtc index
    }
    else
    {
        switch (index)
        {
            //
            // These values actually look at hardware, not NvInfo.
            //
            case NV_CFG_VBLANK_STATUS:
                {
                static U032 VblankDummy;
                U032 Value, monitorType, FlatPanelIsDisabled=1, FlatPanelState;
                //
                // Microsoft requested that we twiddle the state of the returned vblank status
                // because if the monitor is off, some DX apps might lockup, waiting for the
                // vblank status to change.  Instead of changing it in the HAL, they asked
                // us to make the driver handle this case.  So if the monitor is off, we
                // twiddle the vblank status bit. Do this ONLY for Win2k.  For any other OS,
                // just behave as if the monitor is always on, since this only seems
                // to be an issue for Win2k.                   
                //

                Value = 0;                                      // Default monitor to always being on
                
#if (_WIN32_WINNT >= 0x0500)
                // Minor Hack - Call back to the miniport to get the 
                // last DPMS state of the monitor
                Value = GetCurrentDPMSState(pDev->DBmpDeviceExtension);
                Value &= 0xC0;
#endif

                // Check for Flat Panel power state
                monitorType = GETMONITORTYPE(pDev, 0);
                if (monitorType==MONITOR_TYPE_FLAT_PANEL)
                {
                    FlatPanelState= REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _DISPEN);     
                    if (FlatPanelState == NV_PRAMDAC_FP_TG_CONTROL_DISPEN_DISABLE)
                        FlatPanelIsDisabled = 1;                // Flat panel is off/disabled
                    else
                        FlatPanelIsDisabled = 0;                // Flat panel is on/active                                           
                }
                
                if ( ((monitorType!=MONITOR_TYPE_FLAT_PANEL) && (Value)) || 
                     ((monitorType==MONITOR_TYPE_FLAT_PANEL) && (FlatPanelIsDisabled)) ) // In power saving mode if either is set
                    Value = (VblankDummy ^= 1);                 // just toggle, so we are sure to get both states
                else
                {
                    Value = REG_RD_DRF(_PCRTC, _RASTER, _POSITION);
                    if (Value == 0) // make sure we don't have false count. NV3 seems to have glitch during display period.
                    {
                        Value = REG_RD_DRF(_PCRTC, _RASTER, _POSITION);
                    }

                    if (Value == 0)
                        Value = 1;                              // In VBLANK
                    else
                        { // raster count is nonzero

                        if (Value >= (pDev->Framebuffer.HalInfo.VertDisplayWidth-1))
                            Value = 1;                          // In VBLANK
                        else    
                            Value = 0;                          // NOT in VBLANK
                        }
                

                    } // if in power saving mode            

                result = Value;                                // Return state of vblank

                }
                
                break;

            case NV_CFG_GE_STATUS:
                result = REG_RD32(NV_PGRAPH_STATUS);
                break;

            case NV_CFG_CURRENT_SCANLINE:
                result = REG_RD_DRF(_PCRTC, _RASTER, _POSITION);
                break;

            case NV_CFG_BIOS_DEFAULT_TV_TYPE:
                {
                  U032 biostvdefault;
                  dacGetBiosDefaultTVType(pDev,&biostvdefault);
                  result = biostvdefault;
                }
                break;

            //
            // Fields in NvInfo.
            //
            case NV_CFG_MANUFACTURER:
                result = pDev->Chip.HalInfo.Manufacturer;
                break;
            case NV_CFG_ARCHITECTURE:
                result = pDev->Chip.HalInfo.Architecture;
                break;
            case NV_CFG_REVISION:
                result = pDev->Chip.HalInfo.MaskRevision;
                break;
            case NV_CFG_BIOS_REVISION:
                result = pDev->Chip.BiosRevision;
                break;
            case NV_CFG_IMPLEMENTATION:
                result = pDev->Chip.HalInfo.Implementation;
                break;
            case NV_CFG_BUS_TYPE:
                result = pDev->Chip.Bus;
                break;
            case NV_CFG_CRYSTAL_FREQ:
                result = pDev->Chip.HalInfo.CrystalFreq;
                break;
            case NV_CFG_BIOS_OEM_REVISION:
                result = pDev->Chip.BiosOEMRevision;
                break;
#ifndef _WIN64
            case NV_CFG_ADDRESS_NVADDR:
                result = (U032) nvAddr;
                break;
#endif
            case NV_CFG_ADDRESS:
                result = pDev->Mapping.PhysAddr;
                break;
            case NV_CFG_IRQ:
                result = pDev->Mapping.IntLine;
                break;
            case NV_CFG_IO_NEEDED:
                result = pDev->Mapping.doIOToFlushCache;
                break;
            case NV_CFG_DEVICE_INSTANCE:
                result = devInstance;
                break;
#if defined(WIN32) && !defined(NTRM)
            case NV_CFG_DEVICE_HANDLE:
                {   
                    PWIN9XHWINFO    pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
                    result = pOsHwInfo->osDeviceHandle;
                }
                break;
#endif
            case NV_CFG_PCI_ID:
                result = pDev->Chip.HalInfo.PCIDeviceID;
                break;            
            case NV_CFG_PCI_SUB_ID:
                result = pDev->Chip.HalInfo.PCISubDeviceID;
                break;            
            case NV_CFG_PROCESSOR_TYPE:
                result = pRmInfo->Processor.Type;
                break;
            case NV_CFG_PROCESSOR_SPEED:
                result = pRmInfo->Processor.Clock;
                break;
            case NV_CFG_GRAPHICS_CAPS:
                result = pDev->Graphics.Capabilities;
                break;
            case NV_CFG_INSTANCE_TOTAL:
                result = pDev->Pram.HalInfo.TotalInstSize;
                break;
            case NV_CFG_INSTANCE_SIZE:
                result = pDev->Pram.FreeInstSize;
                break;
            case NV_CFG_FIFO_COUNT:
                result = pDev->Fifo.HalInfo.Count;
                break;
            case NV_CFG_FIFO_USE_COUNT:
                result = pDev->Fifo.HalInfo.AllocateCount;
                break;
            case NV_CFG_RAM_SIZE_MB:
                result = pDev->Framebuffer.HalInfo.RamSizeMb;
                break;
            case NV_CFG_RAM_SIZE:
                result = pDev->Framebuffer.HalInfo.RamSize;
                break;
            case NV_CFG_RAM_TYPE:
                result = pDev->Framebuffer.HalInfo.RamType;
                break;
            case NV_CFG_AGP_PHYS_BASE:
            {
                if (IsAGPCapable(pDev))        
                    result = pRmInfo->AGP.AGPPhysStart;
                else
                    result = 0;
                break;
            }
#ifndef IA64
            case NV_CFG_AGP_LINEAR_BASE:
            {
                if (IsAGPCapable(pDev))    
                    result = pRmInfo->AGP.AGPLinearStart;
                else
                    result = 0;
                break;
            }
#endif
            case NV_CFG_AGP_LIMIT:
            {
                // Before returning the AGP limit, update our boot time value
                osUpdateAGPLimit(pDev);

                if (IsAGPCapable(pDev))    
                    result = pRmInfo->AGP.AGPLimit;
                else
                    result = 0;
                break;
            }
            case NV_CFG_AGP_HEAP_FREE:
            {
                if (IsAGPCapable(pDev))  
                    result = pRmInfo->AGP.AGPHeapFree;
                else
                    result = 0;
                break;
            }
            case NV_CFG_AGP_FW_ENABLE:
            {
                if (IsAGPCapable(pDev))  
                    result = pRmInfo->AGP.AGPFWEnable;
                else
                    result = 0;
                break;
            }
            case NV_CFG_AGP_FULLSPEED_FW:
            {
                if (IsAGPCapable(pDev))  
                    result = pRmInfo->AGP.AGPFullSpeedFW;
                else
                    result = 0;
                break;
            }
            case NV_CFG_SCREEN_WIDTH:
                result = pDev->Framebuffer.HalInfo.HorizDisplayWidth;
                break;
            case NV_CFG_SCREEN_HEIGHT:
                result = pDev->Framebuffer.HalInfo.VertDisplayWidth;
                break;
            case NV_CFG_PIXEL_DEPTH:
                result = pDev->Dac.HalInfo.Depth;
                break;
            case NV_CFG_PRIMARY_SURFACE_PITCH:
                result = pDev->Dac.DisplayPitch;
                break;
            case NV_CFG_SCREEN_MAX_WIDTH:
                {
                    // This value depends on the display type.
                    // We really should read the EDID of the display device to get the max.
                    // For now hard code the values.
                    switch (GETMONITORTYPE(pDev, 0))
                    {
                        case MONITOR_TYPE_VGA:
                            result = 1920;
                            break;
                        case MONITOR_TYPE_NTSC:
                            result = 800;
                            break;
                        case MONITOR_TYPE_PAL:
                            result = 800;
                            break;
                        case MONITOR_TYPE_FLAT_PANEL:
                            {
                                U032 fpMode, fpMaxX, fpMaxY;
                                BOOL fpConnect;
                                BOOL fpNativeSupported;
                                dacGetFlatPanelInfo(pDev,
                                                    0,
                                                    &fpMode, &fpMaxX, &fpMaxY,
                                                    &fpConnect,
                                                    &fpNativeSupported, 
                                                    FALSE /* not from registry */);
                                result = fpMaxX;
                            }
                            break;
                        default:
                            result = 0;
                            rmStatus = RM_ERROR;
                            break;
                    } // switch on MonitorType
                }
                break;
            case NV_CFG_SCREEN_MAX_HEIGHT:
                {
                    // This value depends on the display type.
                    // We really should read the EDID of the display device to get the max.
                    // For now hard code the values.
                    switch (GETMONITORTYPE(pDev, 0))
                    {
                        case MONITOR_TYPE_VGA:
                            result = 1200;
                            break;
                        case MONITOR_TYPE_NTSC:
                            result = 600;
                            break;
                        case MONITOR_TYPE_PAL:
                            result = 600;
                            break;
                        case MONITOR_TYPE_FLAT_PANEL:
                            {
                                U032 fpMode, fpMaxX, fpMaxY;
                                BOOL fpConnect;
                                BOOL fpNativeSupported;
                                dacGetFlatPanelInfo(pDev,
                                                    0,
                                                    &fpMode, &fpMaxX, &fpMaxY,
                                                    &fpConnect,
                                                    &fpNativeSupported, 
                                                    FALSE /* not from registry */);
                                result = fpMaxY;
                            }
                            break;
                        default:
                            result = 0;
                            rmStatus = RM_ERROR;
                            break;
                    } // switch on MonitorType
                }
                break;
            case NV_CFG_VIDEO_ENCODER_TYPE:
                result = pDev->Dac.EncoderType;
                break;
            case NV_CFG_VIDEO_ENCODER_ID:
                result = pDev->Dac.EncoderID;
                break;
            case NV_CFG_VIDEO_DISPLAY_TYPE:
                result = GETDISPLAYTYPE(pDev, 0);
                break;
            case NV_CFG_VIDEO_MONITOR_TYPE:
                result = GETMONITORTYPE(pDev, 0);
                break;
            case NV_CFG_VIDEO_TV_FORMAT:
                result = pDev->Dac.TVStandard;
                break;
            case NV_CFG_VIDEO_REFRESH_RATE:
                result = pDev->Framebuffer.HalInfo.RefreshRate;
                break;
            case NV_CFG_VIDEO_OVERLAY_ALLOWED:
                result = pDev->Video.OverlayAllowed;
                break;
            case NV_CFG_VIDEO_DOWNSCALE_RATIO_768:
                result = pDev->Video.HalInfo.OverlayMaxDownscale_768;
                break;
            case NV_CFG_VIDEO_DOWNSCALE_RATIO_1280:
                result = pDev->Video.HalInfo.OverlayMaxDownscale_1280;
                break;
            case NV_CFG_VIDEO_DOWNSCALE_RATIO_1920:
                result = pDev->Video.HalInfo.OverlayMaxDownscale_1920;
                break;
            case NV_CFG_FRAMEBUFFER_UNDERSCAN_X:
                result = pDev->Framebuffer.Underscan_x;
                break;
            case NV_CFG_FRAMEBUFFER_UNDERSCAN_Y:
                result = pDev->Framebuffer.Underscan_y;
                break;
            case NV_CFG_VBLANK_TOGGLE:
                // Assume head 0 for now...
                result = pDev->Dac.CrtcInfo[0].VBlankToggle;
                break;
            case NV_CFG_VBLANK_COUNTER:
                // Assume head 0 for now...
                result = pDev->Dac.CrtcInfo[0].VBlankCounter;
                break;
            case NV_CFG_PRAM_CURRENT_SIZE:
                result = pDev->Pram.HalInfo.CurrentSize;
                break;
            case NV_CFG_DAC_INPUT_WIDTH:
                result = pDev->Dac.HalInfo.InputWidth;
                break;
            case NV_CFG_DAC_PIXEL_CLOCK:
                result = pDev->Dac.HalInfo.VClk;
                break;
            case NV_CFG_DAC_MEMORY_CLOCK:
                result = pDev->Dac.HalInfo.MClk;
                break;
            case NV_CFG_DAC_GRAPHICS_CLOCK:
                result = pDev->Dac.HalInfo.NVClk;
                break;
            case NV_CFG_DAC_VPLL_M:
                result = pDev->Dac.HalInfo.VPllM;
                break;
            case NV_CFG_DAC_VPLL_N:
                result = pDev->Dac.HalInfo.VPllN;
                break;
            case NV_CFG_DAC_VPLL_O:
                result = pDev->Dac.HalInfo.VPllO;
                break;
            case NV_CFG_DAC_VPLL_P:
                result = pDev->Dac.HalInfo.VPllP;
                break;
            case NV_CFG_DAC_MPLL_M:
                result = pDev->Dac.HalInfo.MPllM;
                break;
            case NV_CFG_DAC_MPLL_N:
                result = pDev->Dac.HalInfo.MPllN;
                break;
            case NV_CFG_DAC_MPLL_O:
                result = pDev->Dac.HalInfo.MPllO;
                break;
            case NV_CFG_DAC_MPLL_P:
                result = pDev->Dac.HalInfo.MPllP;
                break;
            case NV_CFG_DAC_PCLK_LIMIT_8BPP:
                result = GETCRTCHALINFO(pDev, 0, PCLKLimit8bpp);
                break;            
            case NV_CFG_DAC_PCLK_LIMIT_16BPP:
                result = GETCRTCHALINFO(pDev, 0, PCLKLimit16bpp);
                break;            
            case NV_CFG_DAC_PCLK_LIMIT_32BPP:
                result = GETCRTCHALINFO(pDev, 0, PCLKLimit32bpp);
                break;            
#ifdef _WIN32
            case NV_CFG_MAPPING_BUSDEVICEFUNC:
                result = (pDev->Mapping.nvBusDeviceFunc << 8) | REG_RD32(NV_PBUS_PCI_NV_0);
                break;
#endif
            case NV_CFG_VIDEO_OUTPUT_FORMAT:
            {
                U032 Head = 0;
                rmStatus = dacReadTVOutFromRegistry(pDev, Head, &result);
                break;
            }
            case NV_CFG_NUMBER_OF_HEADS:
                result = pDev->Dac.HalInfo.NumCrtcs;
                break;

            case NV_CFG_NUMBER_OF_ACTIVE_HEADS:
            {
                U032 i;
                for (i = 0, result = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
                    if (pDev->Dac.CrtcInfo[i].pVidLutCurDac)
                        result++;
                break;
            }
            case NV_CFG_INITIAL_BOOT_HEAD:
                result = pDev->Dac.HalInfo.InitialBootHead;
                break;
#ifdef LINUX
            case NV_CFG_INTERRUPTS_ENABLED_STATE:
                *pValue = pDev->Chip.IntrEn0;
                break;
#endif
#ifdef RM_STATS
            case NV_STAT_INTERVAL_NSEC_LO:
                result = pDev->Statistics.UpdateLo;
                break;
            case NV_STAT_INTERVAL_NSEC_HI:
                result = pDev->Statistics.UpdateHi;
                break;
            case NV_STAT_FIFO_EXCEPTIONS:
                result = pDev->Statistics.FifoExceptionCount;
                break;
            case NV_STAT_FRAMEBUFFER_EXCEPTIONS:
                result = pDev->Statistics.FramebufferExceptionCount;
                break;
            case NV_STAT_GRAPHICS_EXCEPTIONS:
                result = pDev->Statistics.GraphicsExceptionCount;
                break;
            case NV_STAT_TIMER_EXCEPTIONS:
                result = pDev->Statistics.TimerExceptionCount;
                break;
            case NV_STAT_CHIP_EXCEPTIONS:
                result = pDev->Statistics.TotalExceptionCount;
                break;
            case NV_STAT_MEM_ALLOCATED:
                result = pDev->Statistics.MemAlloced;
                break;
            case NV_STAT_MEM_LOCKED:
                result = pDev->Statistics.MemLocked;
                break;
            case NV_STAT_EXEC_NSEC_LO:
                result = pDev->Statistics.ExecTimeLo;
                break;
            case NV_STAT_EXEC_NSEC_HI:
                result = pDev->Statistics.ExecTimeHi;
                break;
            case NV_STAT_INT_NSEC_LO:
                result = pDev->Statistics.IntTimeLo;
                break;
            case NV_STAT_INT_NSEC_HI:
                result = pDev->Statistics.IntTimeHi;
                break;
#endif

#if defined(DEBUG) && ( defined(UNIX) || defined(MACOS) )
            // debug only hack to read current debug
            case NV_CFG_DEBUG_LEVEL:
            {
                extern int cur_debuglevel;
                result = cur_debuglevel;
                break;
            }
#endif

            case NV_CFG_GET_ALL_DEVICES:
                result = pDev->Dac.DevicesBitMap;
                break;

            case NV_CFG_GET_BOOT_DEVICES:
                result = pDev->Dac.BootDevicesBitMap;
                break;

            default:
                result = 0;
                rmStatus = RM_ERROR;
                break;
        }
    }

    // save the result
    *pValue = result;

    return rmStatus;
}

//
// Returns the original value
//
RM_STATUS stateConfigSet(
    PHWINFO pDev, // Pointer to an nv device
    U032    index,
    U032    newvalue,
    U032*   pOldvalue
)
{
    PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;
    RM_STATUS rmStatus = RM_OK;
    U032 result = 0;

    // 
    // Special case the CRTC access since we're using the lower byte of
    // the index to denote the register we want to modify
    //
    if ((index & 0xF00) == NV_CFG_CRTC)
    {
        U008 lock, crtc_index;
        
        //
        // Save the current CRTC index
        //
        if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)    // color or mono?
            crtc_index = REG_RD08(NV_PRMCIO_CRX__COLOR);    // save crtc index
        else
            crtc_index = REG_RD08(NV_PRMCIO_CRX__MONO); // save crtc index

        // Unlock CRTC extended regs
        CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, 0);
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);

        //
        // Get the register value.
        //
        CRTC_RD((index & 0xFF), result, 0);

        //
        // Update the new value
        //
        CRTC_WR((index & 0xFF), newvalue, 0);
        
        // Relock extended regs
        if (lock == 0)
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, 0);

        // Restore index
        if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)
            REG_WR08(NV_PRMCIO_CRX__COLOR, crtc_index); // restore crtc index
        else
            REG_WR08(NV_PRMCIO_CRX__MONO, crtc_index);  // restore crtc index
    }
    else
    {
        switch (index)
        {
            //
            // Fields in NvInfo.
            //
            case NV_CFG_AGP_PHYS_BASE:
                result = pRmInfo->AGP.AGPPhysStart;
                pRmInfo->AGP.AGPPhysStart = newvalue;
                break;
#ifndef IA64
            case NV_CFG_AGP_LINEAR_BASE:
                result = pRmInfo->AGP.AGPLinearStart;
                pRmInfo->AGP.AGPLinearStart = newvalue;
                break;
#endif
            case NV_CFG_AGP_LIMIT:
                result = pRmInfo->AGP.AGPLimit;
                pRmInfo->AGP.AGPLimit = newvalue;
                break;
            case NV_CFG_SCREEN_WIDTH:
                result = pDev->Framebuffer.HalInfo.HorizDisplayWidth;
                pDev->Framebuffer.HalInfo.HorizDisplayWidth = newvalue;
                break;
            case NV_CFG_SCREEN_HEIGHT:
                result = pDev->Framebuffer.HalInfo.VertDisplayWidth;
                pDev->Framebuffer.HalInfo.VertDisplayWidth = newvalue;
                break;
            case NV_CFG_PIXEL_DEPTH:
                result = pDev->Dac.HalInfo.Depth;
                pDev->Dac.HalInfo.Depth = newvalue;
                break;
            case NV_CFG_VIDEO_DISPLAY_TYPE:
                result = GETDISPLAYTYPE(pDev, 0);
                SETDISPLAYTYPE(pDev, 0, newvalue);
                break;
            case NV_CFG_VIDEO_MONITOR_TYPE:
                result = GETMONITORTYPE(pDev, 0);
                SETMONITORTYPE(pDev, 0, newvalue);
                break;
            case NV_CFG_VIDEO_REFRESH_RATE:
                result = pDev->Framebuffer.HalInfo.RefreshRate;
                pDev->Framebuffer.HalInfo.RefreshRate = newvalue;
                break;
            case NV_CFG_FRAMEBUFFER_UNDERSCAN_X:
                result = pDev->Framebuffer.Underscan_x;
                pDev->Framebuffer.Underscan_x = newvalue;
                break;
            case NV_CFG_FRAMEBUFFER_UNDERSCAN_Y:
                result = pDev->Framebuffer.Underscan_y;
                pDev->Framebuffer.Underscan_y = newvalue;
                break;
            case NV_CFG_VBLANK_TOGGLE:
                result = pDev->Dac.CrtcInfo[0].VBlankToggle;
                pDev->Dac.CrtcInfo[0].VBlankToggle = newvalue;
                break;
            case NV_CFG_VBLANK_COUNTER:
                result = pDev->Dac.CrtcInfo[0].VBlankCounter;
                pDev->Dac.CrtcInfo[0].VBlankCounter = newvalue;
                break;
            case NV_CFG_DAC_PIXEL_CLOCK:
                result = pDev->Dac.HalInfo.PixelClock;
                pDev->Dac.HalInfo.PixelClock = newvalue;
                break;
            case NV_CFG_DAC_MEMORY_CLOCK:
                result = pDev->Dac.HalInfo.MClk;
                pDev->Dac.HalInfo.MClk = newvalue;
                pDev->Dac.UpdateFlags  |= UPDATE_HWINFO_DAC_MPLL;
                break;
            case NV_CFG_DAC_GRAPHICS_CLOCK:
                result = pDev->Dac.HalInfo.NVClk;
                pDev->Dac.HalInfo.NVClk = newvalue;
                pDev->Dac.UpdateFlags  |= UPDATE_HWINFO_DAC_NVPLL;
                break;
            case NV_CFG_VIDEO_OUTPUT_FORMAT:
            {
                U032 Head = 0;
                dacWriteTVOutToRegistry(pDev, Head, newvalue);
                break;
            }
            case NV_CFG_IRQ:
                result = pDev->Mapping.IntLine;
                pDev->Mapping.IntLine = newvalue; 
                break;
#ifdef  STEREO_SUPPORT
            case NV_CFG_STEREO_CONFIG:
                if (result = (U032)pDev->pStereoParams &&
                    pDev->pStereoParams->FlipOffsets[3][0] != 0xFFFFFFFF)
                {
                    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT vidLutCurDacHalObj;
                    //Stereo was previously activated. We want to make
                    //sure we are flipped to the primary left eye.
                    //pDev->pStereoParams->FlipOffsets[3][0] is supposed
                    //to point us to the primary surface offset.
                    vidLutCurDacHalObj.Head = 0;
                    nvHalDacSetStartAddr(pDev, pDev->pStereoParams->FlipOffsets[3][0], &vidLutCurDacHalObj);
                }
                pDev->pStereoParams = (LPNV_CFG_STEREO_PARAMS)newvalue;
                break;
#endif  //STEREO_SUPPORT

#ifdef LINUX
            case NV_CFG_INTERRUPTS_ENABLED_STATE:
                result = pDev->Chip.IntrEn0;
                pDev->Chip.IntrEn0 = newvalue;
                RmEnableInterrupts(pDev);           // will just slam in IntrEn0
                break;
#endif

#ifdef RM_STATS
            case NV_STAT_INTERVAL_NSEC_LO:
                result = pDev->Statistics.UpdateLo;
                pDev->Statistics.UpdateLo = newvalue;
                break;
            case NV_STAT_INTERVAL_NSEC_HI:
                result = pDev->Statistics.UpdateHi;
                pDev->Statistics.UpdateHi = newvalue;
                break;
            case NV_STAT_FIFO_EXCEPTIONS:
                result = pDev->Statistics.FifoExceptionCount;
                pDev->Statistics.FifoExceptionCount = newvalue;
                break;
            case NV_STAT_FRAMEBUFFER_EXCEPTIONS:
                result = pDev->Statistics.FramebufferExceptionCount;
                pDev->Statistics.FramebufferExceptionCount = newvalue;
                break;
            case NV_STAT_GRAPHICS_EXCEPTIONS:
                result = pDev->Statistics.GraphicsExceptionCount;
                pDev->Statistics.GraphicsExceptionCount = newvalue;
                break;
            case NV_STAT_TIMER_EXCEPTIONS:
                result = pDev->Statistics.TimerExceptionCount;
                pDev->Statistics.TimerExceptionCount = newvalue;
                break;
            case NV_STAT_CHIP_EXCEPTIONS:
                result = pDev->Statistics.TotalExceptionCount;
                pDev->Statistics.TotalExceptionCount = newvalue;
                break;
            case NV_STAT_MEM_ALLOCATED:
                result = pDev->Statistics.MemAlloced;
                pDev->Statistics.MemAlloced = newvalue;
                break;
            case NV_STAT_MEM_LOCKED:
                result = pDev->Statistics.MemLocked;
                pDev->Statistics.MemLocked = newvalue;
                break;
            case NV_STAT_EXEC_NSEC_LO:
                result = pDev->Statistics.ExecTimeLo;
                pDev->Statistics.ExecTimeLo = newvalue;
                break;
            case NV_STAT_EXEC_NSEC_HI:
                result = pDev->Statistics.ExecTimeHi;
                pDev->Statistics.ExecTimeHi = newvalue;
                break;
            case NV_STAT_INT_NSEC_LO:
                result = pDev->Statistics.IntTimeLo;
                pDev->Statistics.IntTimeLo = newvalue;
                break;
            case NV_STAT_INT_NSEC_HI:
                result = pDev->Statistics.IntTimeHi;
                pDev->Statistics.IntTimeHi = newvalue;
                break;
#endif

#if defined(DEBUG) && ( defined(UNIX) || defined(MACOS) )
            // debug only hack to set current debug level
            case NV_CFG_DEBUG_LEVEL:
            {
                extern int cur_debuglevel;
                result = cur_debuglevel;
                cur_debuglevel = newvalue;
                break;
            }
#endif

            default:
                result = 0;
                rmStatus = RM_ERROR;
                break;
        }
    }

    *pOldvalue = result;

    return rmStatus; 
}

RM_STATUS stateConfigGetEx(
    PHWINFO pDev,
    U032    index,
    VOID    *paramStructPtr,
    U032    paramSize
)
{
    PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    RM_STATUS status = RM_OK;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: getEx index ", index);

    switch (index)
    {
        case NV_CFGEX_GET_DESKTOP_POSITION_MONITOR:
        {
            U032 Head = 0;  // for legacy compatibility, there is no parameter passed 
            NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_PARAMS *Params;
            Params = (NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else
            {
                if (Params->GetDefault)
                {
                    dacGetMonitorDefaultPosition(pDev, 
                                                 Head,
                                                 &Params->HBlankStart, &Params->HBlankEnd,
                                                 &Params->VBlankStart, &Params->VBlankEnd,
                                                 &Params->HRetraceStart, &Params->HRetraceEnd,
                                                 &Params->VRetraceStart, &Params->VRetraceEnd);
                    status = RM_OK;
                }
                else
                {
                    U032 cbLen;
                    // Get the H & V Blank start and end. These values do not change after
                    // modeset. The H & V retrace start and end may have been changed.
                    // But get the default values first.
                    dacGetMonitorDefaultPosition(pDev,
                                                 Head,
                                                 &Params->HBlankStart, &Params->HBlankEnd,
                                                 &Params->VBlankStart, &Params->VBlankEnd,
                                                 &Params->HRetraceStart, &Params->HRetraceEnd,
                                                 &Params->VRetraceStart, &Params->VRetraceEnd);
                    // Read the registry to get any updates to the default H & V retrace
                    // start and end.
                    cbLen = 4 * sizeof(U032);
                    status = dacReadDesktopPositionFromRegistry(pDev, Head,
                                                                TRUE,  // isMonitor
                                                                (U008 *) &(Params->HRetraceStart),
                                                                &cbLen);
                    // If there is an error, we don't have to do anything. The default values have already been 
                    // saved in Params.
                }
            }
        }
        break;

        case NV_CFGEX_GET_DESKTOP_POSITION_TV:
        {
            U032 Head = 0;
            NV_CFGEX_DESKTOP_POSITION_TV_PARAMS *Params;
            Params = (NV_CFGEX_DESKTOP_POSITION_TV_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_DESKTOP_POSITION_TV_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else
            {
                U032 cbLen;
                
                // if default, get position from tables, else get from registry
                if (Params->Default)
                {
                    dacGetDefaultTVPosition(pDev, Head, Params->Encoder_ID, Params->Default, (U032 *)&Params->u);
                }
                else
                {
                    NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS MultiParams;

                    //
                    // For dacSetTVPosition to work, we need to standardize on one POSITION_TV
                    // params struct format that lives in the registry (currently, you can have a
                    // MULTI or a non-MULTI version, which is a problem).
                    //
                    // Until we remove the non-MULTI versions of these calls, independent of how
                    // we're called we'll read/write the registry with the MULTI version of struct.
                    // Which means (temporarily), we need to perform a cast in the non-MULTI case.

                    // Read the registry to get any updates to the default H & V offset
                    cbLen = sizeof(NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS);
                    status = dacReadTVDesktopPositionFromRegistry(pDev, Head,
                                                            (U008 *) &MultiParams,
                                                            &cbLen);

                    // Make sure data is valid for the current encoder.
                    if (MultiParams.Encoder_ID != pDev->Dac.EncoderType)
                        status = RM_ERROR;                  
                    if (status) // if reg read failed, return defaults
                        dacGetDefaultTVPosition(pDev, Head, Params->Encoder_ID, Params->Default, (U032 *)&Params->u);
                    else {
                        // succeeded, convert MULTI to non-MULTI version
                        osMemCopy((U008*)Params, (U008*)&MultiParams.Encoder_ID, paramSize);
                    }

                    status = RM_OK;
                }
            }
        }
        break;

        case NV_CFGEX_GET_TV_ENCODER_INFO:
        {
            NV_CFGEX_TV_ENCODER_PARAMS *Params;
            Params = (NV_CFGEX_TV_ENCODER_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_TV_ENCODER_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else
            {
                Params->EncoderType = pDev->Dac.EncoderType;
                Params->EncoderID = pDev->Dac.EncoderID;
                if (dacTVConnectStatus(pDev, 0))
                    Params->TVConnected = TV_CONNECTED;
                else
                    Params->TVConnected = TV_NOT_CONNECTED;
                status = RM_OK;
            }
        }
        break;

        case NV_CFGEX_GET_FLAT_PANEL_INFO:
        {
            NV_CFGEX_GET_FLATPANEL_INFO_PARAMS *Params;

            Params = (NV_CFGEX_GET_FLATPANEL_INFO_PARAMS *)paramStructPtr;
            if(!IsNV5orBetter(pDev))
                    Params->FlatPanelConnected = FALSE;
            else
                if (paramSize < sizeof(NV_CFGEX_GET_FLATPANEL_INFO_PARAMS))
                    // Wrong param size.
                    status = CONFIG_GETEX_BAD_PARAM;
                else
                    status = dacGetFlatPanelInfo(pDev, 0, &Params->FlatPanelMode,
                                             &Params->FlatPanelSizeX, &Params->FlatPanelSizeY,
                                             &Params->FlatPanelConnected,
                                             &Params->FlatPanelNativeSupported,
                                             FALSE /* not from registry */);
        }
        break;
        
        case NV_CFGEX_FLAT_PANEL_BRIGHTNESS:
        {
        	NV_CFGEX_FLATPANEL_BRIGHTNESS_PARAMS * Params;
        	Params=(NV_CFGEX_FLATPANEL_BRIGHTNESS_PARAMS *)paramStructPtr;
        	if(paramSize!=sizeof(NV_CFGEX_FLATPANEL_BRIGHTNESS_PARAMS))
        	{
        		status=CONFIG_GETEX_BAD_PARAM;
        	}
        	else
        	{
        		status = dacGetFlatPanelBrightness(pDev, Params->Head, &(Params->PercentRelativeBrightness),&(Params->PWMControllerEnable));
        	}
        }
        break;

        case NV_CFGEX_GET_MONITOR_INFO:
        {
                NV_CFGEX_GET_MONITOR_INFO_PARAMS *Params;
                Params = (NV_CFGEX_GET_MONITOR_INFO_PARAMS *)paramStructPtr;
                status = dacGetMonitorInfo(pDev, (U032) 0, &Params->MonitorConnected);
        }
        break;

        case NV_CFGEX_GET_EDID:
        {
#ifndef IKOS
            NV_CFGEX_GET_EDID_PARAMS *Params;
            VOID  *edidBuffer = (VOID *) 0;
            U032 bufferLength;
            Params = (NV_CFGEX_GET_EDID_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_EDID_PARAMS)) {
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            }
            else {
                REG_WR08(NV_PRMCIO_CRX__COLOR, 0x571F & 0xFF); 
                REG_WR08(NV_PRMCIO_CRX__COLOR+1, (0x571F >> 8) & 0xFF);
                ////OEMEnableExtensions();          // Unlock the registers before DDC
                status = EDIDRead(pDev,0,Params->displayType);
                REG_WR08(NV_PRMCIO_CRX__COLOR, 0x991F & 0xFF); 
                REG_WR08(NV_PRMCIO_CRX__COLOR+1, (0x991F >> 8) & 0xFF);
                ////OEMDisableExtensions();         // Lock the registers
                if ( status == RM_OK ) {
                    // status = EDIDCopy(pDev, Params->edidBuffer, Params->bufferLength, (U032) 0);
                    if (RM_SUCCESS(status = osCopyIn((U008*) Params->bufferLength, (U008*) &bufferLength, sizeof(U032))) &&
                        RM_SUCCESS(status = osAllocMem((VOID **) &edidBuffer, (U032) bufferLength)) &&
                        RM_SUCCESS(status = EDIDCopy(pDev, edidBuffer, &bufferLength, (U032) 0)) &&
                        RM_SUCCESS(status = osCopyOut((U008*) &bufferLength, (U008*) Params->bufferLength, sizeof(U032))) &&
                        RM_SUCCESS(status = osCopyOut((U008*) edidBuffer, (U008*) Params->edidBuffer, (U032) bufferLength)))
                    {
                        // all done 
                    }
                    if (edidBuffer)
                        osFreeMem(edidBuffer);
                }
                else {
                    status = CONFIG_GETEX_BAD_READ;
                }
            }
#else
            // just for IKOS builds return quickly
            status = CONFIG_GETEX_BAD_READ;
#endif
        }
        break;

        case NV_CFGEX_GET_BIOS:
        {
            NV_CFGEX_GET_BIOS_PARAMS *Params;
           	U008  *biosBuffer = (U008 *)NULL;
			U032 bufferLength;
			Params = (NV_CFGEX_GET_BIOS_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_BIOS_PARAMS)) 
            {
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            }
            else 
            {
                // Get the size we need to recover
                osCopyIn((U008*) Params->bufferLength, (U008*) &bufferLength, sizeof(U032));
                status = osAllocMem((VOID **)&biosBuffer, (U032)bufferLength);
                if (RM_SUCCESS(status))
                {
                    U032 i;
                    // Pull the image byte-by-byte from the ghosted instance copy
                    for (i=0; i<bufferLength;i++)
                        biosBuffer[i] = (U008)BIOS_RD08(i);

                    // Copy the buffers back into the param structures (user buffer)
                    osCopyOut((U008*) &bufferLength, (U008*) Params->bufferLength, sizeof(U032));
                    osCopyOut((U008*) biosBuffer, (U008*) Params->biosBuffer, (U032) bufferLength);
                }

                if (biosBuffer)
                    osFreeMem(biosBuffer);

                if (RM_SUCCESS(status))
                    status = CONFIG_GETEX_OK;
                else
                    status = CONFIG_GETEX_BAD_READ;

            }
        }
        break;

        case NV_CFGEX_GET_SURFACE_PITCH:
        {
            NV_CFGEX_GET_SURFACE_PITCH_PARAMS *Params;
            Params = (NV_CFGEX_GET_SURFACE_PITCH_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_SURFACE_PITCH_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else {
                nvHalFbGetSurfacePitch(pDev, Params->Width, Params->Depth, &Params->Pitch);
                status = CONFIG_GETEX_OK;
            }
        }
        break;

        case NV_CFGEX_GET_SURFACE_DIMENSIONS:
        {
            NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS *Params;
            Params = (NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else {
                nvHalFbGetSurfaceDimensions(pDev, Params->Width, Params->Height, Params->Depth,
                                            &Params->Pitch, &Params->Size);
                status = CONFIG_GETEX_OK;
            }
        }
        break;

        case NV_CFGEX_GET_SUPPORTED_CLASSES:
        {
            NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS *Params;
            Params = (NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else {
                //
                // Return numClasses, if the classBuffer pointer is NULL. Otherwise, fill in
                // the classBuffer if it's big enough to hold all the values.
                //
                if (Params->classBuffer == NULL) {
                    Params->numClasses = pDev->classDB.numClasses;
                    status = CONFIG_GETEX_OK;
                } else {
                    if (Params->numClasses != pDev->classDB.numClasses)
                        status = CONFIG_GETEX_BAD_PARAM;
                    else {
                        U032 i;
                        PCLASS classDB = (PCLASS)pDev->classDB.pClasses;
                        //
                        // Fill in classBuffer with the supported (common/chip specific) classes.
                        //
                        //for (i = 0; i < pDev->numClasses; i++)
                        //    Params->classBuffer[i] = classDB[i].Type;

                        for (i = 0; i < pDev->classDB.numClasses; i++)
                            osCopyOut((U008*) &classDB[i].Type, (U008*) &Params->classBuffer[i], sizeof(U032));

                        status = CONFIG_GETEX_OK;
                    }
                }
            }
        }
        break;

        case NV_CFGEX_GET_DISPLAY_TYPE:
        {
            U032 Head = 0;
            NV_CFGEX_GET_DISPLAY_TYPE_PARAMS *Params;
            Params = (NV_CFGEX_GET_DISPLAY_TYPE_PARAMS *)paramStructPtr;
            
            if (paramSize < sizeof(NV_CFGEX_GET_DISPLAY_TYPE_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else {
                Params->Type = GETDISPLAYTYPE(pDev, Head);
                if (Params->Type == DISPLAY_TYPE_TV)
                    Params->TVType = pDev->Dac.TVStandard;
                status = CONFIG_GETEX_OK;
            }
        }
        break;

        case NV_CFGEX_GET_DISPLAY_TYPE_MULTI:
        {
            NV_CFGEX_GET_DISPLAY_TYPE_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_GET_DISPLAY_TYPE_MULTI_PARAMS *)paramStructPtr;
            
            if (paramSize < sizeof(NV_CFGEX_GET_DISPLAY_TYPE_MULTI_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else {
                Params->Type = GETDISPLAYTYPE(pDev, Params->Head);
                if (Params->Type == DISPLAY_TYPE_TV)
                    Params->TVType = pDev->Dac.TVStandard;
                status = CONFIG_GETEX_OK;
            }
        }
        break;

        case NV_CFGEX_GET_REFRESH_INFO:
        {
            NV_CFGEX_GET_REFRESH_INFO_PARAMS *Params;
            Params = (NV_CFGEX_GET_REFRESH_INFO_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_REFRESH_INFO_PARAMS)) {
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            } else if (Params->Head > pDev->Dac.HalInfo.NumCrtcs) {
                // Wrong param head.
                status = CONFIG_GETEX_BAD_PARAM;
            } else if (pDev->Dac.CrtcInfo[Params->Head].pVidLutCurDac == NULL) {
                Params->ulRefreshRate = 0;
                status = RM_OK;
            } else {
                PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
                pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Params->Head].pVidLutCurDac;
                Params->ulRefreshRate = pVidLutCurDac->RefreshRate;
                status = RM_OK;
            }

            // Params->ulState = pDev->Framebuffer.RefreshRateType;
        }
        break;

        case NV_CFGEX_GET_AGP_OFFSET:
        {
            NV_CFGEX_GET_AGP_OFFSET_PARAMS *Params;
            Params = (NV_CFGEX_GET_AGP_OFFSET_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_AGP_OFFSET_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else {
                Params->offset = 0xFFFFFFFF;        // init to unsigned -1

                status = osVirtualToPhysicalAddr(Params->linaddr, &Params->physaddr);
                if (status == RM_OK)
                {
                    // check if it's in the AGP aperture, if so, return the offset
                    if (
                        Params->physaddr >= (VOID*)pRmInfo->AGP.AGPPhysStart &&
                        (U008*)Params->physaddr <= (U008*)pRmInfo->AGP.AGPPhysStart + pRmInfo->AGP.AGPLimit
                    )
                    {
                        Params->offset = (U032)((U008*)Params->physaddr - (U008*)pRmInfo->AGP.AGPPhysStart);
                        status = RM_OK;
                    }
                    else
                        status = CONFIG_GETEX_BAD_PARAM;        // not an AGP aperture address
                }
            }
        }
        break;

        case NV_CFGEX_AGP_LINEAR_BASE:
        {
            NV_CFGEX_AGP_LINEAR_BASE_PARAMS *Params;
            Params = (NV_CFGEX_AGP_LINEAR_BASE_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_AGP_LINEAR_BASE_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else {
                Params->linaddr = (void *)pRmInfo->AGP.AGPLinearStart;
                status = CONFIG_GETEX_OK;
            }
        }
        break;

        case NV_CFGEX_ADDRESS_NVADDR:
        {
            NV_CFGEX_ADDRESS_NVADDR_PARAMS *Params;
            Params = (NV_CFGEX_ADDRESS_NVADDR_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_ADDRESS_NVADDR_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else {
                Params->nvaddr = nvAddr;
                status = CONFIG_GETEX_OK;
            }
        }
        break;

        case NV_CFGEX_RESERVED:
        {
            // Refer to nvcmrsvd.h for supported properties.
            NV_CFGEX_RESERVED_PROPERTY * pParams = (NV_CFGEX_RESERVED_PROPERTY*)paramStructPtr;
            U032                         In[1]   = {0};
            U032                         Out[1]  = {0};

            // Copy the data in (1 parameter).
            osCopyIn((U008*)pParams->In, (U008*)In, 1*sizeof(U032));

            // Get the data.
            status = CONFIG_GETEX_BAD_PARAM;
            if (paramSize == sizeof(NV_CFGEX_RESERVED_PROPERTY))
            {
                status = RM_OK;
                switch (pParams->Property)
                {
                    case PROPERTY_REG_RD08:
                        Out[0] = REG_RD08(In[0]);
                        break;

                    case PROPERTY_REG_RD16:
                        Out[0] = REG_RD16(In[0]);
                        break;

                    case PROPERTY_REG_RD32:
                        Out[0] = REG_RD32(In[0]);
                        break;

                    case PROPERTY_FB_RD08:
                        Out[0] = FB_RD08(In[0]);
                        break;

                    case PROPERTY_FB_RD16:
                        Out[0] = FB_RD16(In[0]);
                        break;

                    case PROPERTY_FB_RD32:
                        Out[0] = FB_RD32(In[0]);
                        break;

                    default:
                        status = CONFIG_GETEX_BAD_PARAM;
                        break;
                }
            }

            // Copy the data out (1 parameter).
            osCopyOut((U008*)Out, (U008*)pParams->Out, 1*sizeof(U032));
        }
        break;

#if 1 
        case NV_CFGEX_GET_LOGICAL_DEV_EDID:
        {
            U008 lock;        
            NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS *Params;
            U032 Head, edidversion, edidrevision;
            VOID  *edidBuffer = (VOID *) 0;
            //U032 bufferLength, i;
            Params = (NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS)) {
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            }
            else 
            {
                status = dacGetDisplayInfo(pDev, Params->displayMap, &Head, 0, 0, 0);
                if (status != RM_OK)
                {
                    status = CONFIG_GETEX_BAD_READ;
                    break;
                }

                lock = UnlockCRTC(pDev, Head);
                status = edidReadDevEDID(pDev, Params->displayMap, Params->edidBuffer);
                RestoreLock(pDev, Head, lock);

                if (status != RM_OK)
                {
                    status = CONFIG_GETEX_BAD_READ;
                    break;
                }

                status = edidLogicalEDIDIsValid(&Params->edidBuffer, &edidversion, &edidrevision);

                if (status != TRUE)
                {
                    status = CONFIG_GETEX_BAD_READ;
                    break;
                }

                if (edidversion == 1)
                {
                    *Params->bufferLength = EDID_V1_SIZE;
                }
                else if (edidversion == 2)
                {
                    *Params->bufferLength = EDID_V2_SIZE;
                }
            }
            break;
        }

        //
        // Multi-Head Info. These take logical Head number as a parameter
        //
        case NV_CFGEX_GET_EDID_MULTI:
        {
            U008 lock;        
            NV_CFGEX_GET_EDID_MULTI_PARAMS *Params;
            VOID  *edidBuffer = (VOID *) 0;
            U032 bufferLength;
            Params = (NV_CFGEX_GET_EDID_MULTI_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_EDID_MULTI_PARAMS)) {
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            }
            else {
                if (Params->Head > pDev->Dac.HalInfo.NumCrtcs) {    // Bad Head number will cause our nvInfo to get trashed
                    status = CONFIG_GETEX_BAD_PARAM;
                }
                else {
                    lock = UnlockCRTC(pDev, Params->Head);
                    status = EDIDRead(pDev, Params->Head, Params->displayType);
                    RestoreLock(pDev, Params->Head, lock);
                    if ( status == RM_OK ) {
                        //status = EDIDCopy(pDev, Params->edidBuffer, Params->bufferLength, Params->Head);
                        if (RM_SUCCESS(status = osCopyIn((U008*) Params->bufferLength, (U008*) &bufferLength, sizeof(U032))) &&
                            RM_SUCCESS(status = osAllocMem((VOID **) &edidBuffer, (U032) bufferLength)) &&
                            RM_SUCCESS(status = EDIDCopy(pDev, edidBuffer, &bufferLength, (U032) Params->Head)) &&
                            RM_SUCCESS(status = osCopyOut((U008*) &bufferLength, (U008*) Params->bufferLength, sizeof(U032))) &&
                            RM_SUCCESS(status = osCopyOut((U008*) edidBuffer, (U008*) Params->edidBuffer, (U032) bufferLength)))
                        {
                            // all done 
                        }
                        if (edidBuffer)
                            osFreeMem(edidBuffer);
                    }
                    else {
                        status = CONFIG_GETEX_BAD_READ;
                    }
                }
            }
        }
        break;
        
        case NV_CFGEX_VIDEO_ENCODER_TYPE:
        {
            NV_CFGEX_VIDEO_ENCODER_TYPE_PARAMS *Params;
            Params = (NV_CFGEX_VIDEO_ENCODER_TYPE_PARAMS *)paramStructPtr;
            
            Params->EncoderType = pDev->Dac.EncoderType;
        }
        break;
        case NV_CFGEX_VIDEO_ENCODER_ID:
        {
            NV_CFGEX_VIDEO_ENCODER_ID_PARAMS *Params;
            Params = (NV_CFGEX_VIDEO_ENCODER_ID_PARAMS *)paramStructPtr;

            Params->EncoderID = pDev->Dac.EncoderID;
        }
        break;
        case NV_CFGEX_DAC_PCLK_LIMIT_8BPP:
        {
            NV_CFGEX_DAC_PCLK_LIMIT_PARAMS *Params;
            Params = (NV_CFGEX_DAC_PCLK_LIMIT_PARAMS *)paramStructPtr;
            
            if(Params->Head < DACHALINFO(pDev, NumCrtcs))
                Params->pclkLimit = GETCRTCHALINFO(pDev, Params->Head, PCLKLimit8bpp);    
            else    
                status = CONFIG_GETEX_BAD_PARAM;
        }
        break;
        case NV_CFGEX_DAC_PCLK_LIMIT_16BPP:
        {
            NV_CFGEX_DAC_PCLK_LIMIT_PARAMS *Params;
            Params = (NV_CFGEX_DAC_PCLK_LIMIT_PARAMS *)paramStructPtr;
            
            if(Params->Head < DACHALINFO(pDev, NumCrtcs))
                Params->pclkLimit = GETCRTCHALINFO(pDev, Params->Head, PCLKLimit16bpp);   
            else    
                status = CONFIG_GETEX_BAD_PARAM;
        }
        break;
        case NV_CFGEX_DAC_PCLK_LIMIT_32BPP:
        {
            NV_CFGEX_DAC_PCLK_LIMIT_PARAMS *Params;
            Params = (NV_CFGEX_DAC_PCLK_LIMIT_PARAMS *)paramStructPtr;
            
            if(Params->Head < DACHALINFO(pDev, NumCrtcs))
                Params->pclkLimit = GETCRTCHALINFO(pDev, Params->Head, PCLKLimit32bpp);   
            else    
                status = CONFIG_GETEX_BAD_PARAM;
        }
        break;

        case NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_MULTI:
        {
            NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_MULTI_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_MULTI_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else
            {
                if (Params->GetDefault)
                {
                    dacGetMonitorDefaultPosition(pDev, 
                                                 Params->Head,
                                                 &Params->HBlankStart, &Params->HBlankEnd,
                                                 &Params->VBlankStart, &Params->VBlankEnd,
                                                 &Params->HRetraceStart, &Params->HRetraceEnd,
                                                 &Params->VRetraceStart, &Params->VRetraceEnd);
                    status = RM_OK;
                }
                else
                {
                    U032 cbLen;
                    // Get the H & V Blank start and end. These values do not change after
                    // modeset. The H & V retrace start and end may have been changed.
                    // But get the default values first.
                    dacGetMonitorDefaultPosition(pDev,
                                                 Params->Head,
                                                 &Params->HBlankStart, &Params->HBlankEnd,
                                                 &Params->VBlankStart, &Params->VBlankEnd,
                                                 &Params->HRetraceStart, &Params->HRetraceEnd,
                                                 &Params->VRetraceStart, &Params->VRetraceEnd);
                    // Read the registry to get any updates to the default H & V retrace
                    // start and end.
                    cbLen = 4 * sizeof(U032);
                    status = dacReadDesktopPositionFromRegistry(pDev, Params->Head,
                                                                TRUE,  // isMonitor
                                                                (U008 *) &(Params->HRetraceStart),
                                                                &cbLen);
                    // If there is an error, we don't have to do anything. The default values have already been 
                    // saved in Params.
                }
            }
        }
        break;

        case NV_CFGEX_GET_DESKTOP_POSITION_TV_MULTI:
        {
            NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else
            {
                U032 cbLen;
                
                // if default, get position from tables, else get from registry
                if (Params->Default)
                {
                    dacGetDefaultTVPosition(pDev, Params->Head, Params->Encoder_ID, Params->Default, (U032 *)&Params->u);
                }
                else
                {
                    // Read the registry to get any updates to the default H & V offset
                    cbLen = sizeof(NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS);
                    status = dacReadTVDesktopPositionFromRegistry(pDev, Params->Head,
                                                            (U008 *) Params,
                                                            &cbLen);
                    // Make sure data is valid for the current encoder.
                    if (Params->Encoder_ID != pDev->Dac.EncoderType)
                        status = RM_ERROR;                  
                    if (status) // if reg read failed, return defaults
                        dacGetDefaultTVPosition(pDev, Params->Head, Params->Encoder_ID, Params->Default, (U032 *)&Params->u);
                    status = RM_OK;
                }
            }
        }
        break;

        case NV_CFGEX_GET_TV_ENCODER_INFO_MULTI:
        {
            NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS))
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
            else
            {
                Params->EncoderType = pDev->Dac.EncoderType;
                Params->EncoderID = pDev->Dac.EncoderID;
                if (dacTVConnectStatus(pDev, Params->Head))
                    Params->TVConnected = TV_CONNECTED;
                else
                    Params->TVConnected = TV_NOT_CONNECTED;
                status = RM_OK;
            }
        }
        break;

        case NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI:
        {
            NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS *Params;

            Params = (NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS *)paramStructPtr;
            if(!IsNV5orBetter(pDev))
                Params->FlatPanelConnected = FALSE;
            else
                if (paramSize < sizeof(NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS))
                    // Wrong param size.
                    status = CONFIG_GETEX_BAD_PARAM;
                else
                {
                    //
                    // CONTROL PANEL WORKAROUND FOR MOBILE
                    //
                    // On mobile the control panel will be expecting the DFP to be on the
                    // virtual primary head (0), yet in reality the physical head is always 1.
                    // He might call in asking about head0, so route it to head1.
                    //
                    if (pDev->Power.MobileOperation)
                        Params->Head = 1;
                    status = dacGetFlatPanelInfo(pDev, Params->Head, &Params->FlatPanelMode,
                                         &Params->FlatPanelSizeX, &Params->FlatPanelSizeY,
                                         &Params->FlatPanelConnected,
                                         &Params->FlatPanelNativeSupported,
                                         FALSE /* not from registry */);
                }
        }
        break;

        case NV_CFGEX_GET_MONITOR_INFO_MULTI:
        {
            NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS *)paramStructPtr;
            status = dacGetMonitorInfo(pDev, Params->Head, &Params->MonitorConnected);
        }
        break;

        case NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST:
        {
            NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_PARAMS *Params;
            Params = (NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_PARAMS *)paramStructPtr;
            status = dacGetColorSaturationBoost(pDev, Params->Head, &Params->boostValue);
        }
        break;


#endif  // multi-head      

        case NV_CFGEX_CURRENT_SCANLINE_MULTI:
        {
            NV_CFGEX_CURRENT_SCANLINE_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_CURRENT_SCANLINE_MULTI_PARAMS *)paramStructPtr;
            if (Params->Head < DACHALINFO(pDev, NumCrtcs))
                Params->RasterPosition = DAC_REG_RD_DRF(_PCRTC, _RASTER, _POSITION, Params->Head);
            else
                status = CONFIG_GETEX_BAD_PARAM;
        }
        break;

        case NV_CFGEX_SYNC_STATE:
        {
            NV_CFGEX_SYNC_STATE_PARAMS *Params;
            Params = (NV_CFGEX_SYNC_STATE_PARAMS *) paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_SYNC_STATE_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else
            {
                dacGetSyncState(pDev, Params->Head, &Params->Hsync, &Params->Vsync);
                // paper over a bit of ambiguity :-)
                Params->oldHsync = Params->Hsync;
                Params->oldVsync = Params->Vsync;
            }
        }
        break;

        case NV_CFGEX_PRIMARY_INFO:
        {
            NV_CFGEX_PRIMARY_INFO_PARAMS *Params;
            Params = (NV_CFGEX_PRIMARY_INFO_PARAMS *) paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_PRIMARY_INFO_PARAMS))
            {
                // Wrong param size.
                status = CONFIG_GETEX_BAD_PARAM;
                break;
            }

            // TODO is there a 'max heads' #define ?
            if (Params->Head > 1)
            {
                status = CONFIG_GETEX_BAD_PARAM;
                break;
            }
            
            /// TODO 'primarySurfaceAlloced' should probably be per-head
            //  TODO 'Dac.DisplayPitch'         ""
            if ( ! pDev->Framebuffer.PrimarySurfaceAlloced)
            {
                status = CONFIG_GETEX_BAD_READ;
                break;
            }
                
            Params->Pitch = pDev->Dac.DisplayPitch;
            Params->Offset = pDev->Framebuffer.HalInfo.Start[Params->Head];
        }
        break;

        //Interface for control panel to read pending mobile hotkey (Fn+x) events.
        case NV_CFGEX_GET_HOTKEY_EVENT:
        {
            NV_CFGEX_GET_HOTKEY_EVENT_PARAMS *Params;
            Params = (NV_CFGEX_GET_HOTKEY_EVENT_PARAMS *) paramStructPtr;

            if (paramSize < sizeof(NV_CFGEX_GET_HOTKEY_EVENT_PARAMS)) {
                status = CONFIG_GETEX_BAD_PARAM;
                break;
            }
            if (!pDev->Power.MobileOperation) {
                //If there are no hotkeys to support...
                Params->event = NV_HOTKEY_EVENT_NOT_SUPPORTED;
                Params->status = 0;
            } else if (pDev->HotKeys.queueHead == pDev->HotKeys.queueTail) {
                //If no hotkey events are pending...
                Params->event = NV_HOTKEY_EVENT_NONE_PENDING;
                Params->status = 0;
            } else {
                //If there are events in the queue, report the next one...
                Params->event = pDev->HotKeys.Queue[pDev->HotKeys.queueTail].event;
                Params->status = pDev->HotKeys.Queue[pDev->HotKeys.queueTail].status;
                pDev->HotKeys.queueTail = (pDev->HotKeys.queueTail + 1) % NV_HOTKEY_EVENT_QUEUE_SIZE;
            }
        }
        break;

        case NV_CFGEX_GET_DEVICE_MAPPINGS:
        {
            NV_CFGEX_GET_DEVICE_MAPPINGS_PARAMS *Params;

            //
            // This call is for use by the NVDE debugger.
            //
            if (paramSize < sizeof(NV_CFGEX_GET_DEVICE_MAPPINGS_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else
            {
                Params = (NV_CFGEX_GET_DEVICE_MAPPINGS_PARAMS *) paramStructPtr;
                Params->nvVirtAddr = (void *)pDev->DBnvAddr;
                Params->fbVirtAddr = (void *)pDev->DBfbAddr;
                Params->nvPhysAddr = (void *)pDev->Mapping.PhysAddr;
                Params->fbPhysAddr = (void *)pDev->Mapping.PhysFbAddr;

                status = RM_OK;
            }
        }
        break;

        case NV_CFGEX_CHECK_CONNECTED_DEVICES:
        {
            U032        *pDevBitMap = (U032 *)paramStructPtr;

            // make sure it's a subset of all available devices.
            if (!*pDevBitMap ||
                ((*pDevBitMap ^ pDev->Dac.DevicesBitMap) & *pDevBitMap)) {
                status = CONFIG_GETEX_BAD_PARAM;
                break;
            }

            dacDevicesConnectStatus(pDev, pDevBitMap);

            break;
        }

        case NV_CFGEX_GET_DEVICES_CONFIGURATION:
        {
            NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS   *param;

            param = (NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS *)paramStructPtr;

            // make sure it's a subset of all available devices.
            if (!param->DevicesConfig ||
                ((param->DevicesConfig ^ pDev->Dac.DevicesBitMap) & param->DevicesConfig)) {
                status = CONFIG_GETEX_BAD_PARAM;
                break;
            }

            if (dacFindDevicesConfiguration(pDev,
                                            param->DevicesConfig,
                                            &param->DevicesAllocationMap,
                                            param->OldDevicesConfig,
                                            param->OldDevicesAllocationMap) != RM_OK)
                status = CONFIG_GETEX_BAD_PARAM;

            break;
        }

        default:
            status = CONFIG_GETEX_BAD_INDEX;
            break;
    } // switch on index

    return status;

} // end of stateConfigGetEx

RM_STATUS stateConfigSetEx(
    PHWINFO pDev,
    U032    index,
    VOID    *paramStructPtr,
    U032    paramSize
)
{
    PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    RM_STATUS status = RM_OK;
    switch (index)
    {
        case NV_CFGEX_SET_DESKTOP_POSITION_MONITOR:
        {
            U032 Head = 0;
            NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_PARAMS *Params;
            Params = (NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else
            {
                status = dacSetMonitorPosition(pDev, 
                                               Head,
                                               Params->HRetraceStart, Params->HRetraceEnd,
                                               Params->VRetraceStart, Params->VRetraceEnd);
                if (!status && Params->CommitChanges)
                {
                    // Write the the passed values into the registry.
                    status = dacWriteDesktopPositionToRegistry(pDev,
                                                               Head,
                                                               TRUE, // isMonitor
                                                               (U008 *) &(Params->HRetraceStart),
                                                               4 * sizeof(U032));
                }
            }
        }
        break;

        case NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_MULTI:
        {
            NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_MULTI_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_MULTI_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else
            {
                status = dacSetMonitorPosition(pDev, 
                                               Params->Head,
                                               Params->HRetraceStart, Params->HRetraceEnd,
                                               Params->VRetraceStart, Params->VRetraceEnd);
                if (!status && Params->CommitChanges)
                {
                    // Write the the passed values into the registry.
                    status = dacWriteDesktopPositionToRegistry(pDev,
                                                               Params->Head,
                                                               TRUE, // isMonitor
                                                               (U008 *) &(Params->HRetraceStart),
                                                               4 * sizeof(U032));
                }
            }
        }
        break;

        case NV_CFGEX_COMMIT_DESKTOP_POSITION_TV:
        {
            NV_CFGEX_DESKTOP_POSITION_TV_PARAMS *Params;
            U032 Head = 0;

            Params = (NV_CFGEX_DESKTOP_POSITION_TV_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_DESKTOP_POSITION_TV_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else
            {
                NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS MultiParams;

                //
                // For dacSetTVPosition to work, we need to standardize on one POSITION_TV
                // params struct format that lives in the registry (currently, you can have a
                // MULTI or a non-MULTI version, which is a problem).
                //
                // Until we remove the non-MULTI versions of these calls, independent of how
                // we're called we'll read/write the registry with the MULTI version of struct.
                // Which means (temporarily), we need to perform a cast in the non-MULTI case.

                MultiParams.Head = Head;
                osMemCopy((U008*)&MultiParams.Encoder_ID, (U008*)Params, paramSize);

                // Write the passed values into the registry.
                status = dacWriteDesktopPositionToRegistry(pDev,
                                                           Head,
                                                           FALSE, // not Monitor
                                                           (U008 *) &MultiParams,
                                                           sizeof(NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS));
            }
        }
        break;

        case NV_CFGEX_COMMIT_DESKTOP_POSITION_TV_MULTI:
        {
            NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS *Params;

            Params = (NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else
            {
                // Write the the passed values into the registry.
                status = dacWriteDesktopPositionToRegistry(pDev,
                                                           Params->Head,
                                                           FALSE, // not Monitor
                                                           (U008 *) Params,
                                                           sizeof(NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS));
            }
        }
        break;

        case NV_CFGEX_ENABLE_TETRIS_TILING:
        {
            // Automatic tiling will be enabled at the next modeset.
            // The tiling mode will be selected based on the new mode.
            status = nvHalFbControl(pDev, FB_CONTROL_TILING_ENABLE);
        }
        break;

        case NV_CFGEX_DISABLE_TETRIS_TILING:
        {
            // Automatic tiling will be disabled at the next modeset. It will remain disabled till
            // it is enabled by calling ConfigSetEx with NV_CFGEX_ENABLE_TETRIS_TILING.
            status = nvHalFbControl(pDev, FB_CONTROL_TILING_DISABLE);
        }
        break;

        case NV_CFGEX_SET_FLAT_PANEL_INFO:
        {
            NV_CFGEX_SET_FLATPANEL_INFO_PARAMS *Params;
            Params = (NV_CFGEX_SET_FLATPANEL_INFO_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_SET_FLATPANEL_INFO_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else {
                RM_ASSERT(pDev->Dac.CrtcInfo[0].pVidLutCurDac);
                status = dacSetFlatPanelMode(pDev, 0,
                                             (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[0].pVidLutCurDac,
                                             Params->FlatPanelMode, Params->CommitChanges);
            }
        }
        break;

        case NV_CFGEX_SET_FLAT_PANEL_INFO_MULTI:
        {
            NV_CFGEX_SET_FLATPANEL_INFO_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_SET_FLATPANEL_INFO_MULTI_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_SET_FLATPANEL_INFO_MULTI_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else {
                RM_ASSERT(pDev->Dac.CrtcInfo[Params->Head].pVidLutCurDac);
                status = dacSetFlatPanelMode(pDev, Params->Head,
                                             (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Params->Head].pVidLutCurDac,
                                             Params->FlatPanelMode, Params->CommitChanges);
            }
        }
        break;
        
        case NV_CFGEX_FLAT_PANEL_BRIGHTNESS:
        {
        	NV_CFGEX_FLATPANEL_BRIGHTNESS_PARAMS * Params;
        	Params=(NV_CFGEX_FLATPANEL_BRIGHTNESS_PARAMS *)paramStructPtr;
        	
        	if(paramSize<sizeof(NV_CFGEX_FLATPANEL_BRIGHTNESS_PARAMS))
        	{
        		//wrong parameter size
        		status=CONFIG_SETEX_BAD_PARAM;
        	}
        	else
        	{
        		status=dacSetFlatPanelBrightness(pDev, Params->Head,
        									Params->PercentRelativeBrightness,
                							Params->FlatPanelBrightnessControlFrequency,
                							Params->PercentMaximumPWMDutyCycle,
                							Params->PercentMinimumPWMDutyCycle,
                							Params->PWMControllerEnable);
            }
         }
         break;

        case NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST:
        {
            NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_PARAMS *Params;
            Params = (NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_PARAMS *)paramStructPtr;
            status = dacSetColorSaturationBoost(pDev, Params->Head, Params->boostValue);
        }
        break;

        //
        // XXXscottl
        //
        // These SET_DISPLAY_TYPE calls need to be removed.
        // All programming of the display/monitor/tv types
        // should be done via the dac classes from now on.
        //

        case NV_CFGEX_SET_DISPLAY_TYPE:
        {
            U032 Head = 0;

            NV_CFGEX_SET_DISPLAY_TYPE_PARAMS *Params;
            Params = (NV_CFGEX_SET_DISPLAY_TYPE_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_SET_DISPLAY_TYPE_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else {
                SETDISPLAYTYPE(pDev, Head, Params->Type);
                if ((Params->Type == DISPLAY_TYPE_TV) && 
                    (Params->TVType <= KNOWN_TV_FORMATS))
                    pDev->Dac.TVStandard = Params->TVType;
                status = CONFIG_SETEX_OK;
            }
        }
        break;

        case NV_CFGEX_SET_DISPLAY_TYPE_MULTI:
        {
            NV_CFGEX_SET_DISPLAY_TYPE_MULTI_PARAMS *Params;
            Params = (NV_CFGEX_SET_DISPLAY_TYPE_MULTI_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_SET_DISPLAY_TYPE_MULTI_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else {
                SETDISPLAYTYPE(pDev, Params->Head, Params->Type);
                if ((Params->Type == DISPLAY_TYPE_TV) && 
                    (Params->TVType <= KNOWN_TV_FORMATS))
                    pDev->Dac.TVStandard = Params->TVType;
                status = CONFIG_SETEX_OK;
            }
        }
        break;

        case NV_CFGEX_AGP_LINEAR_BASE:
        {
            NV_CFGEX_AGP_LINEAR_BASE_PARAMS *Params;
            VOID_PTR oldValue;

            Params = (NV_CFGEX_AGP_LINEAR_BASE_PARAMS *)paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_AGP_LINEAR_BASE_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else {
                oldValue = pRmInfo->AGP.AGPLinearStart;
                pRmInfo->AGP.AGPLinearStart = (VOID_PTR)Params->linaddr;
                Params->linaddr = (void *)oldValue;
                status = CONFIG_SETEX_OK;
            }
        }
        break;

        case NV_CFGEX_SYNC_STATE:
        {
            NV_CFGEX_SYNC_STATE_PARAMS *Params;
            Params = (NV_CFGEX_SYNC_STATE_PARAMS *) paramStructPtr;
            if (paramSize < sizeof(NV_CFGEX_SYNC_STATE_PARAMS))
                // Wrong param size.
                status = CONFIG_SETEX_BAD_PARAM;
            else
            {
                dacGetSyncState(pDev, Params->Head, &Params->oldHsync, &Params->oldVsync);
                dacSetSyncState(pDev, Params->Head, Params->Hsync, Params->Vsync);
            }
        }
        break;

        case NV_CFGEX_RESERVED:
        {
            // Refer to nvcmrsvd.h for supported properties.
            NV_CFGEX_RESERVED_PROPERTY * pParams = (NV_CFGEX_RESERVED_PROPERTY*)paramStructPtr;
            U032                         In[2]   = {0};

            // Copy the data in (2 parameter).
            osCopyIn((U008*)pParams->In, (U008*)In, 2*sizeof(U032));

            // Set the data.
            status = CONFIG_SETEX_BAD_PARAM;
            if (paramSize == sizeof(NV_CFGEX_RESERVED_PROPERTY))
            {
                status = RM_OK;
                switch (pParams->Property)
                {
                    case PROPERTY_REG_WR08:
                        REG_WR08(In[0], In[1]);
                        break;

                    case PROPERTY_REG_WR16:
                        REG_WR16(In[0], In[1]);
                        break;

                    case PROPERTY_REG_WR32:
                        REG_WR32(In[0], In[1]);
                        break;

                    case PROPERTY_FB_WR08:
                        FB_WR08(In[0], In[1]);
                        break;

                    case PROPERTY_FB_WR16:
                        FB_WR16(In[0], In[1]);
                        break;

                    case PROPERTY_FB_WR32:
                        FB_WR32(In[0], In[1]);
                        break;

                    case PROPERTY_SET_GRAPHICS_CLOCK:
                        pDev->Dac.HalInfo.NVClk = In[0];
                        if (dacProgramNVClk(pDev) != RM_OK)
                            status = CONFIG_SETEX_BAD_WRITE;

                        break;

                    case PROPERTY_SET_MEMORY_CLOCK:
                        pDev->Dac.HalInfo.MClk = In[0];
                        if (dacProgramMClk(pDev) != RM_OK)
                            status = CONFIG_SETEX_BAD_WRITE;

                        break;

                    case PROPERTY_SET_PIXEL_CLOCK:
                    {
                        // dacProgramPClk takes frequency in MHz * 100.
                        U032 Head      = In[0];
                        U032 Frequency = In[1] / 10000;
                        if (dacProgramPClk(pDev, Head, Frequency) != RM_OK)
                            status = CONFIG_SETEX_BAD_WRITE;

                        break;
                    }

                    default:
                        status = CONFIG_SETEX_BAD_PARAM;
                        break;
                }
            }
        }
        break;

        case NV_CFGEX_SET_MACROVISION_MODE:
        {
            NV_CFGEX_SET_MACROVISION_MODE_PARAMS *Params;
            RM_MACROVISION_CONTROL mvCtrl;

            // extract parameters from the command struct
            Params = (NV_CFGEX_SET_MACROVISION_MODE_PARAMS *) paramStructPtr;
            mvCtrl.mode = Params->mode;
            mvCtrl.resx = Params->resx;
            mvCtrl.resy = Params->resy;

            // extract parameters that the RM maintains
            mvCtrl.devAddr = pDev->Dac.EncoderID;
            mvCtrl.encoderType = pDev->Dac.EncoderType;
            mvCtrl.tvStandard = pDev->Dac.TVStandard;
            mvCtrl.port = pDev->Dac.TVOutPortID;

            if ( SetMacrovisionMode(pDev, Params->head, &mvCtrl) ) {
                status = CONFIG_GETEX_BAD_PARAM;
            }
            else {
                status = RM_OK;
            }
        }
        break;

        default:
            status = CONFIG_SETEX_BAD_INDEX;
            break;
    }

    return status;
} // end of stateConfigSetEx
