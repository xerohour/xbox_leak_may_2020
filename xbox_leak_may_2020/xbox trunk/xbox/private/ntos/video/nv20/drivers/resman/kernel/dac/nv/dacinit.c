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

/******************************** DAC Manager ******************************\
*                                                                           *
* Module: DACINIT.C                                                         *
*   The DAC is initialized in this module.                                  *
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
#include <dma.h>
#include <dac.h>
#include <os.h>
#include <i2c.h>
#include <edid.h>
#include "nvhw.h"
#include "nvcm.h"
#include <devinit.h>

//
// DAC names.
//
char dac16Name[] = "SGS 16-bit DAC                ";
char dac32Name[] = "SGS 32-bit DAC (Van Gogh)     ";
char dac64Name[] = "SGS 64-bit DAC (Van Dyke)     ";
char dac128Name[] = "NV 128-bit DAC (Picasso)    ";

//
// Externals.
//

extern char strDevNodeRM[];
char strEnable256Burst[] = "Enable256Burst";
char strTVOutOnPrimary[] = "TVOutOnPrimary";
char strLastExtDevice[] = "LastExtDevice";
char strCurWinDevices[] = "strCurWinDevices";

BOOL dacTVConnected(PHWINFO);

BOOL dacMonitorConnectStatus(PHWINFO,U032);
BOOL dacFlatPanelConnectStatus(PHWINFO pDev, U032);

static U032 dacGetDisplayType(PHWINFO, U032);

static RM_STATUS dacInitCurrentDevices(PHWINFO);
static RM_STATUS dacInitGetMobileEDIDFromBios(PHWINFO);
static VOID dacInitFPTableToEDID(PHWINFO, PBIOSFPTable, U008 *);

char digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
#define DIGIT(i,n) digits[((n >> (i * 4)) & 0xF)]
#define COMPRESSEDASCIIVAL(i) ((i - 'A') + 1)
//
// Initialize the DAC.
//
RM_STATUS initDac
(
    PHWINFO pDev
)
{
    U032 i;
    U032 j;
    U032 k;
    U032 data, data32, type;
#ifndef IKOS
    U008 data08, lockA;
#endif
    U032 freq = 0;

    //
    // Initialize flags.
    //
    pDev->Dac.UpdateFlags = 0;
    pDev->Dac.DisplayChangeInProgress = 0;

    //
    // Disable I/O broadcast mode for now until we can figure
    // out why we can't reach HeadB through privSpace with
    // this enabled
    //
    FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _DISABLE);

    nvHalDacControl(pDev, DAC_CONTROL_INIT);

    //
    // Initialize CR44 shadow contents to UNKNOWN and determine/set the
    // current boot head (call EnableHead to make things consistent).
    // 
    pDev->Dac.HalInfo.Head2RegOwner = HEAD2_OWNER_UNKNOWN;
    EnableHead(pDev, dacGetBiosDisplayHead(pDev));

    if ( BiosGetMaxExternalPCLKFreq( pDev, 1, &freq ) == RM_OK )
    {
        if ( freq != 0 )
        {
            // Convert freq from freq(Mhz)*100 to plain Hz
            freq = freq * 10000;
            pDev->halHwInfo.pDacHalInfo->CrtcInfo[1].PCLKLimit8bpp  = freq;
            pDev->halHwInfo.pDacHalInfo->CrtcInfo[1].PCLKLimit16bpp = freq;
            pDev->halHwInfo.pDacHalInfo->CrtcInfo[1].PCLKLimit32bpp = freq;
        }
    }
    //
    // Determine how to handle syncs on CRT.  This key, if it exists, tells us
    // to leave HSync ON during power mgmt of the display (CRT)
    //
    if (dacReadMonitorSyncsFromRegistry(pDev, &data32) == RM_OK)
        pDev->Dac.HsyncOff = 0;
    else
        pDev->Dac.HsyncOff = 1;     // default is to leave it off like normal

    // Determine I2C setup from BIOS
    dacReadBIOSI2CSettings(pDev);

    // Read/Parse BIOS Display Config Block
    dacParseDCBFromBIOS(pDev);

    //
    // Find out where to send TV Out commands (overrides dacReadBIOSI2CSettings)
    //
    // NV_I2C_PORT_SECONDARY is the default (unless the BIOS overrode it), but
    // we can override everything here with the registry
    if ( OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, strTVOutOnPrimary, &data32) == RM_OK)
    {
        pDev->Dac.TVOutPortID = NV_I2C_PORT_PRIMARY;
    }

    //
    // Initialize cursor.
    //
    for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
    {
        pDev->Dac.CrtcInfo[i].CursorType      = DAC_CURSOR_TWO_COLOR_XOR;
        pDev->Dac.CrtcInfo[i].UpdateFlags = 0;
    }

    //
    // Start the cursor cache at 0
    //
    pDev->Dac.CursorCacheCount = 0;
    pDev->Dac.CursorCacheEntry = 0;
    pDev->Dac.CursorCacheEnable = 1;
    
    //
    // Clear the initial cursor cache data
    //
    for (k = 0; k < MAX_CACHED_CURSORS; k++)
    {
        pDev->Dac.CursorMaskCRC[k] = 0;    
        for (i = 0; i < 32; i++)    
            for (j = 0; j < 32; j++)
                pDev->Dac.CursorImageCache[i][j][k] = 0;
    }                
            
    //
    // Initialize palette to grey scale ramp.
    //
    for (i = 0; i < 256; i++)
        pDev->Dac.Palette[i] = (i << 16) | (i << 8) | i;
    //
    // Initialize gamma to unity translation.
    //
    for (i = 0; i < 256; i++)
        pDev->Dac.Gamma[i] = (i << 16) | (i << 8) | i;
    //
    // Initialize power management state.
    //
    pDev->Dac.DpmLevel = 0;

    //Initialize device properties bits, used to interpret scratch register settings.
    if (osReadRegistryDword(pDev, strDevNodeRM, "RMMaintainDevs", &data32) == RM_OK)
        pDev->Dac.DeviceProperties.RMMaintainDevs = data32;
    else
        pDev->Dac.DeviceProperties.RMMaintainDevs = 0;

    pDev->Dac.DeviceProperties.OverrideSBIOSDevs = ((pDev->Power.MobileOperation == 2) || (pDev->Power.MobileOperation == 4))?TRUE:FALSE;
    // For Toshiba, we'd like to set the infer property only in the case of spanning mode.
    // Unfortunately, at this stage the display driver hasn't told us if we're in spanning
    // mode or not.  I've seen that, at least in Win9x, the display driver enables the
    // second display itself once it is started.  But I'd rather not have the logic in
    // two locations like this.
//    pDev->Dac.DeviceProperties.InferAuxDev = FALSE;
    pDev->Dac.DeviceProperties.Spanning = FALSE;
    pDev->Dac.DeviceProperties.LastSwitchEvent = SWITCH_FROM_COLD_START;

    //
    // Check for a registry override to temporarily enable the 256Burst setting
    // within the DAC.
    //    
    if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, strEnable256Burst, &data) == RM_OK)
        pDev->Dac.HalInfo.Enable256Burst = data;

    //
    // Set the min/max VCO values from the BIOS. This call may fail if we're
    // running an older BIOS, so in this case we'll use the RM defaults.
    //
    if (DevinitGetMinMaxVCOValues(pDev,
                                  &pDev->Dac.FminVco,
                                  &pDev->Dac.FmaxVco) != RM_OK)
    {
        // Use the RM default values for these chips
        // NOTE: These defaults are duplicated in dacCalcMNP() which may
        //       be called before initDac()
        if (IsNV15orBetter(pDev)) {
            if (IsNV15MaskRevA01(pDev))
                pDev->Dac.FminVco = 200000;
            else
                pDev->Dac.FminVco = 250000;

            pDev->Dac.FmaxVco = pDev->Dac.FminVco * 2; 
        } else {
            pDev->Dac.FminVco = 128000;
            pDev->Dac.FmaxVco = 256000;
        }
    }

    //
    // Look for a registry override for min/max VCO (in MHz).
    //
    if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "FminVCO", &data32) == RM_OK)
        pDev->Dac.FminVco = data32 * 1000;

    if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "FmaxVCO", &data32) == RM_OK)
        pDev->Dac.FmaxVco = data32 * 1000;

    //
    // Look for a registry override to set the maximum M value for the PLL equation
    // when driving the external DAC on NV11.  Value must be greater than or equal
    // to 1.  A value of 0 is not allowed, and therefore means the limit feature
    // is disabled.
    //
    // This value is used in dacfp.c
    //
    pDev->Dac.ExtDacMaxM = 0;   // default to off
    if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "EXTERNALDACMAXM", &data32) == RM_OK)
        if ((data32 > 0) && (data32 <= 0xFF))
            pDev->Dac.ExtDacMaxM = data32;

    //
    // Attempt to detect the presense of an external TV chip.  Attempt to also
    // identify the part so we know how to program it up when a modeset comes
    // through a bit later.
    //    

    //Skip TV encoder detection for IKOS.
#ifndef IKOS
    //
    // If Canopus, skip the detection
    //
    if (pDev->Video.HalInfo.CustomizationCode & CUSTOMER_CANOPUS)
        pDev->Dac.EncoderType = NV_ENCODER_NONE;
    else
    {        
        // Use head 0 for decoder detection.
        dacDetectEncoder(pDev, 0);
    } // !Canopus

    // Get TV output format from the registry.
    if (dacReadTVOutFromRegistry(pDev, 0, &data32) == RM_OK)
        pDev->Dac.TVoutput = data32;
    else
        pDev->Dac.TVoutput = 0xffffffff;

    //
    // Get TV standard format from what the BIOS chose
    //
    lockA = ReadCRTCLock(pDev, 0);           //Back up lock state
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);    //Unlock CRTC extended regs
    CRTC_RD(NV_CIO_CRE_SCRATCH0__INDEX, data08, 0);  //scratch value on HeadA only!!
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lockA, 0); // Restore lock state

    pDev->Dac.TVStandard = data08 & 0x7;
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "DacInit - read TV standard:", pDev->Dac.TVStandard);
    
    if (!pDev->Power.MobileOperation)
    {
        // Check for User override of TV standard
        if (dacReadTVStandardFromRegistry(pDev, &type) == RM_OK)
        {
            pDev->Dac.TVStandard = type;
        }
    }
    //
    // By default on Chrontel encoders, we need to invert the encoder clock
    // out in order to lengthen our time to sample HSYNC.  Some systems require
    // this feature to be disabled, so look for a registry key to override
    // this functionality
    //
    // Default is enabled, override means disabled
    //
    if (osReadRegistryDword(pDev, strDevNodeRM, "DisableTVClockInversion", &data32) == RM_OK)
        pDev->Dac.TVClockInversionDisabled = 1;
    else
        pDev->Dac.TVClockInversionDisabled = 0;

    if (osReadRegistryDword(pDev, strDevNodeRM, "DisableTVOverscan", &data32) == RM_OK)
        pDev->Dac.TVOverscanDisabled = data32;
    else
        pDev->Dac.TVOverscanDisabled = 0;

    if (RM_OK != osReadRegistryDword(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, "TVMasterMode", &pDev->Dac.TVMasterMode))
        pDev->Dac.TVMasterMode = 0;

    //
    // Default Macrovision mode (OFF)
    //
    pDev->Dac.TVLastMacrovision.devAddr         = 0;
    pDev->Dac.TVLastMacrovision.mode            = 0;
    pDev->Dac.TVLastMacrovision.port            = 0;
    pDev->Dac.TVLastMacrovision.encoderType     = 0;
    pDev->Dac.TVLastMacrovision.tvStandard      = 0;
    pDev->Dac.TVLastMacrovision.resx            = 0;
    pDev->Dac.TVLastMacrovision.resy            = 0;

    //
    // We're about to setup head 0's default display type.
    // Before we do that, invalidate any other heads.
    //
    for (i = 1; i < pDev->Dac.HalInfo.NumCrtcs; i++)
    {
        SETDISPLAYTYPE(pDev, i, 0xffffffff);
        SETMONITORTYPE(pDev, i, 0xffffffff);
    }


    
    //Mobile hotkey support initialization.

    pDev->HotKeys.queueHead = 0;
    pDev->HotKeys.queueTail = 0;
    pDev->HotKeys.enabled = FALSE;

    if (pDev->Power.MobileOperation)
        EnableMobileHotkeyHandling(pDev);

    //
    //Set boot display types.
    //
    dacDetectDevices(pDev, TRUE, FALSE);

    // Assume BIOS panel programming initally for all Mobile platforms
    if(pDev->Power.MobileOperation)
      pDev->Dac.BIOSPanelProgramming = 1;

    dacInitCurrentDevices(pDev);


    // We need to setup the DevicesEnabled SW flag.
    // First turn off all devices
    pDev->Dac.DevicesEnabled &= ~(DAC_ALL_DEVICES_ENABLED);
    // loop on all CRTCs
    for (i = 0; i < MAX_CRTCS; i++) 
    {
        // make sure to switch on the CurrentDisplayType
        switch(pDev->Dac.HalInfo.CrtcInfo[i].CurrentDisplayType)
        {
            case DISPLAY_TYPE_MONITOR:
                // pDev->Dac.DevicesEnabled |= DAC_CRT_ENABLED;  // CRT not currently used
                break;
            case DISPLAY_TYPE_FLAT_PANEL:
                pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;
                break;
            case DISPLAY_TYPE_TV:
                pDev->Dac.DevicesEnabled |= DAC_TV_ENABLED;
                break;
            default:
                break;
        }
    }

    //Use what we know about the boot devices to set up a mapping of logical->physical devices
    // for the display driver's benefit.  This is a recommendation of which head should drive
    // the primary device. Logical device 0 is the primary, logical device 1 is the secondary.
    // By default, we set head 0 to the primary.
    // This initialization should only be of interest to mobile systems that don't do explicit
    // reenumerations when the display driver changes devices (read 'Toshiba'), on desktops it's
    // all run via registry keys.  Later changes to the logical device mapping are made via
    // rmUsrAPI calls to NVRM_API_DISPLAY_SET_PHYS_MAPPING.
    pDev->Dac.LogicalDeviceMap[0] = 0;
    pDev->Dac.LogicalDeviceMap[1] = 1;
    if (pDev->Power.MobileOperation) {
        //We use a simple rule set:
        // 1) if there is only one device, leave it alone.  Except for Toshiba, which wants both
        //   devices regardless.
        // 2) if there are two, one is the internal LCD.  That's the primary.
        if ((pDev->Power.MobileOperation == 2)
            || ((pDev->Dac.BootDevicesBitMap - 1) & pDev->Dac.BootDevicesBitMap)) {
            pDev->Dac.LogicalDeviceMap[0] = 1;
            pDev->Dac.LogicalDeviceMap[1] = 0;
        }
    }

    //
    // Set the MonitorType for head 0 *only*
    //
    switch (GETDISPLAYTYPE(pDev, 0))
    {
        case DISPLAY_TYPE_MONITOR:
            SETMONITORTYPE(pDev, 0, MONITOR_TYPE_VGA);
            break;
        case DISPLAY_TYPE_TV:
            switch (pDev->Dac.TVStandard)
            {
                case NTSC_M:
                case NTSC_J:
                    SETMONITORTYPE(pDev, 0, MONITOR_TYPE_NTSC);
                    break;
                case PAL_A:
                case PAL_M:
                case PAL_N:
                case PAL_NC:
                    SETMONITORTYPE(pDev, 0, MONITOR_TYPE_PAL);
                    break;
            }
            break;
        case DISPLAY_TYPE_FLAT_PANEL:        
            SETMONITORTYPE(pDev, 0, MONITOR_TYPE_FLAT_PANEL);
            break;
        default:
            SETDISPLAYTYPE(pDev, 0, DISPLAY_TYPE_MONITOR);
    }

#endif // not IKOS

    //
    // initialize per-crtc state
    //
    for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
    {
        pDev->Dac.CrtcInfo[i].RefCount = 0;
        pDev->Dac.CrtcInfo[i].StateFlags = 0;
        pDev->Dac.CrtcInfo[i].VBlankCounter = 0;
        pDev->Dac.CrtcInfo[i].VBlankToggle = 0;
        pDev->Dac.CrtcInfo[i].VBlankCallbackList = NULL;
        pDev->Dac.CrtcInfo[i].CurrentPanOffset = 0;
        pDev->Dac.CrtcInfo[i].CurrentImageOffset = 0;
    }

    //
    // Perform any os initialization
    //
    initDacEx(pDev);
    
    //
    // initialize I2C lock flags
    //
    pDev->I2CAcquired = 0;

    //
    // Initialize mclk divide.  Default is 1 which is no
    // divide
    //
    pDev->Dac.HalInfo.MClkDivide = 1;

    //
    // Determine which head was used to boot the system
    //
    // NEED TO DO: Get this from the BIOS, but for now if we're
    // a mobile system booting the DFP let's get the drivers to swing
    // over to the secondary head.
    // LPL : preliminary version that reads from BIOS in place above.
    //
    pDev->Dac.HalInfo.InitialBootHead = 0;
    if (pDev->Power.MobileOperation && (GETDISPLAYTYPE(pDev, 0) == DISPLAY_TYPE_FLAT_PANEL))
        pDev->Dac.HalInfo.InitialBootHead = 1;

    //
    // WORKAROUND - REMOVE QUICKLY BEFORE SCOTT LONG CATCHES ME
    //
    // The current PCLK limits programmed by the HAL are head driven.
    // They really need to be device driven, as the head can drive any
    // one of many devices.  The current HeadB limits were dictated by
    // an external CRT (150MHz max) and were fine while the possible
    // DFP's and TV's stayed below that ceiling.  Now mobile is appearing
    // with huge DFP's, and therefore the 150 limit is insufficient.
    //
    // Since mobile will never use an external DAC, override in this one
    // case to 170MHz in order to handle 16x12 panels.
    //
    // The real limits will actually come from the BIOS via the BIP/3
    // parser.
    //
    if (pDev->Power.MobileOperation)
    {
        pDev->Dac.HalInfo.CrtcInfo[1].PCLKLimit8bpp = 170000000;
        pDev->Dac.HalInfo.CrtcInfo[1].PCLKLimit16bpp = 170000000;
        pDev->Dac.HalInfo.CrtcInfo[1].PCLKLimit32bpp = 170000000;
    }

    //
    // WORKAROUND FOR INCORRECT BOARD STRAPPING ON TOSHIBA SYSTEMS
    //
    // Toshiba mobile is incorrectly strapped for 24bit DFP operation,
    // which precludes the use of the TV encoder.  Force back to the
    // correct 12bit until the boards are reworked.
    //
    if (pDev->Power.MobileOperation == 2)
    {
        FLD_WR_DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_OVERWRITE, _ENABLED);
        FLD_WR_DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_FP_IFACE, _12BIT);
    }

    // Make sure we initialize the panel strap and Mobile EDID
    dacPreInitEDID(pDev);
    
    // determine broadcast support
    // Read Head A CRTC regs
    {
      U008 Cr3C, lock;

      lock = UnlockCRTC(pDev, 0);
      CRTC_RD(NV_CIO_CRE_SCRATCH4__INDEX, Cr3C, 0);     // find out if BIOS supports broadcast mode
      RestoreLock(pDev, 0, lock);
      if((pDev->Power.MobileOperation) && // only for mobiles right now.
         (Cr3C & 0x02)  && // Bit 1 shows BIOS support for broadcast mode. 0-no support 1-supported
         (0)) //(pDev->Chip.BiosRevision > 0x3110134))  Don't support broadcast mode until I get this fixed.
        pDev->Dac.BroadcastSupport = 1;
      else
        pDev->Dac.BroadcastSupport = 0;
    }

    return (RM_OK);
}

// WinNT / Win2k Notes:
//
//        Before RmInitDevice() is called in the NT miniport, we need
//        to make sure that any non-primary monitor is 'alive',  so that
//        dacMonitorConnectStatus() works correctly.  We do this by using
//        the 'WakeUpCard' function in the miniport.
//  
//        dacMonitorConnectStatus() sometimes returns FALSE POSITIVE 
//        when on a FLAT PANEL panel or TV.  So to make NT initialization
//        as safe as possible, we need to use combinations of these functions.
//
//        1) Check for CRT first
//        2) Check for Flat Panel
//        3) Check for TV
//        4) Fall back to CRT
//

#ifdef MACOS        /* macos9 or macosX */

// Mac algorithm for monitor selection is different from windows:
//
//   if flat panel avail
//      use it
//   else if CRT attached
//      use it
//   else
//      no device, error.
//

static U032 dacGetDisplayType(
    PHWINFO pDev,
    U032 Head
)
{
    U032   BootDisplayDevice;
    U008   lock;
    RM_STATUS status;

    //
    // Use our current setting as the desired display type.
    //
    BootDisplayDevice = GETDISPLAYTYPE(pDev, Head);

    if (dacFlatPanelConnectStatus(pDev, Head))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DacInit - Digital Flat Panel\n");

        BootDisplayDevice = DISPLAY_TYPE_FLAT_PANEL;
        
        dacGetPWMController(pDev);          
    }
    else if (dacMonitorConnectStatus(pDev, Head))
    {
        lock = UnlockCRTC(pDev, Head);
        status = EDIDRead(pDev, Head, DISPLAY_TYPE_MONITOR);
        RestoreLock(pDev, Head, lock);
        
        if ((status == RM_OK) && (edidIsAnalogDisplay(pDev, Head)))
            BootDisplayDevice = DISPLAY_TYPE_MONITOR;
    }

    return BootDisplayDevice;
}

#else

static U032 dacGetDisplayType(
    PHWINFO pDev,
    U032 Head
)
{
    U032 BootDisplayDevice;
    BOOL MonitorAvail, FlatPanelAvail, TVAvail, RequestedDisplayDeviceAvail;
    U032 RequestedDisplayDevice;

    //
    // Use our current setting as the desired display type.
    //
    RequestedDisplayDevice = GETDISPLAYTYPE(pDev, Head);

    // @ Detect avail display devices
    // - Check the MONITOR status
    if(dacMonitorConnectStatus(pDev, Head)) {
        MonitorAvail = TRUE;
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DacInit - Monitor connected\n\r");
    }
    else {
        MonitorAvail = FALSE;
    }

    // - Check the FLATPANEL status
    if(IsNV5orBetter(pDev) && dacFlatPanelConnectStatus(pDev, Head)) {
        FlatPanelAvail = TRUE;
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DacInit - Digital Flat Panel connected\n\r");
    }
    else {
        FlatPanelAvail = FALSE;
    }

    // - Check the TV status
    if(dacTVConnectStatus(pDev, Head)) {
        TVAvail = TRUE;
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DacInit - TV connected\n\r");
    }
    else {
        TVAvail = FALSE;
    }

    // - Assume the requested display device is not available
    RequestedDisplayDeviceAvail = FALSE;

    switch(RequestedDisplayDevice) {
        case DISPLAY_TYPE_MONITOR:
            if(MonitorAvail)
                RequestedDisplayDeviceAvail = TRUE;
            break;

        case DISPLAY_TYPE_FLAT_PANEL:
            if(FlatPanelAvail)
                RequestedDisplayDeviceAvail = TRUE;
            break;

        case DISPLAY_TYPE_TV:
            if(TVAvail)
                RequestedDisplayDeviceAvail = TRUE;
            break;

        default:
            break;
    }

    // - If the requested device is unavail, determine the next best thing
    if(!RequestedDisplayDeviceAvail)
    {
        if(MonitorAvail)
        {
            BootDisplayDevice = DISPLAY_TYPE_MONITOR;
        }
        else
        {
            if(FlatPanelAvail)
            {
                BootDisplayDevice = DISPLAY_TYPE_FLAT_PANEL;
            }
            else
            {
                if(TVAvail)
                    BootDisplayDevice = DISPLAY_TYPE_TV;
                else
                    BootDisplayDevice = dacGetBiosDisplayType(pDev, 0);
            }
        }
    }

    // - If the requested device is present, use it
    if(RequestedDisplayDeviceAvail)
        BootDisplayDevice = RequestedDisplayDevice;

    // Display the resulting display device
    switch(BootDisplayDevice) {

        case DISPLAY_TYPE_MONITOR:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DacInit - Monitor selected as display device\n\r");
            break;

        case DISPLAY_TYPE_FLAT_PANEL:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DacInit - Digital Flat Panel selected as display device\n\r");
            break;

        case DISPLAY_TYPE_TV:
               DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DacInit - TV selected as display device\n\r");
               break;

        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DacInit - Display device is unknown\n\r");
            break;
    }

    return BootDisplayDevice;
}
#endif   /* ! MACOS */


//Note this function is only intended for mobile system lookup (assumes internal LCD on head 1)
U032 dispTypeToHotkeyLookup(U032 Head, U032 dispType)
{
    if (dispType == DISPLAY_TYPE_MONITOR)
        return NV_HOTKEY_STATUS_DISPLAY_ENABLE_CRT;
    if (dispType == DISPLAY_TYPE_TV)
        return NV_HOTKEY_STATUS_DISPLAY_ENABLE_TV;
    if (dispType == DISPLAY_TYPE_FLAT_PANEL) {
        if (Head)
            return NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD;
        else
            return NV_HOTKEY_STATUS_DISPLAY_ENABLE_DFP;
    }

    return 0;
}

RM_STATUS dacDetectDevices
(
    PHWINFO pDev,
    BOOL    bStartup,
    BOOL    spoofKey
)
{
    U008 lockA, lockB, cr3BhA, cr3BhB;
    U032 i, j, curDispType[MAX_CRTCS], origDispType[MAX_CRTCS], devmask;
    U032 noCheckDevices = 0;

    // Disable I/O broadcast mode for now to work around a
    // a hardware bug that prevents us from reaching
    // HeadB through privSpace with this enabled
    FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _DISABLE);

    for (i = 0; i < MAX_CRTCS; i++) {
        curDispType[i] = GETDISPLAYTYPE(pDev, i);
        //I'm seeing the display type set to a valid value when the display isn't in use.  So...
        if (!pDev->Dac.CrtcInfo[i].pVidLutCurDac) curDispType[i] = DISPLAY_TYPE_NONE;
        origDispType[i] = curDispType[i];
    }

    if (pDev->Power.MobileOperation) {
        if (pDev->Dac.DeviceProperties.RMMaintainDevs) { //Ignore SBIOS/VBIOS and apply our own selection scheme
            //Could make it so we only need to read the reg key at startup and cache the changes
            // the rest of the time.  This would avoid the problem of being unable to access
            // the registry under NT while at raised IRQL.  But I think we're safe whenever this
            // fn is called.
            if(osReadRegistryDword(pDev, strDevNodeRM, strCurWinDevices, &i) == RM_OK) {
                //Seems unfortunate to break down the bitmask into devices, only to reconstruct
                // a bitmask later, but since part of that later process is detection, and it
                // alreasy works, we'll stick with this format for now.
                curDispType[0] = DISPLAY_TYPE_NONE;
                curDispType[1] = DISPLAY_TYPE_NONE;
                if (i & DAC_DD_MASK_IN_MAP) curDispType[1] = DISPLAY_TYPE_FLAT_PANEL;
                if (i & DAC_TV_MASK_IN_MAP) curDispType[0] = DISPLAY_TYPE_TV;
                if (i & DAC_CRT_MASK_IN_MAP) curDispType[0] = DISPLAY_TYPE_MONITOR;
            } else {    //No reg key? Fallback rules apply.
                curDispType[0] = DISPLAY_TYPE_MONITOR;
                curDispType[1] = DISPLAY_TYPE_FLAT_PANEL;
            }
        } else {    //Check and use current VBIOS settings
            // Unlock both heads
            lockA = UnlockCRTC(pDev, 0);
            lockB = UnlockCRTC(pDev, 1);

            //Read CR3B regs -- used below to set default display types.
            // Write back the registers with the 'enable' bits cleared.
            // They will be set as needed via display driver calls to dacSetModeMulti.
            CRTC_RD(0x3B, cr3BhA, 0);
            CRTC_RD(0x3B, cr3BhB, 1);

            //Removed because calls to detect devices after boot but before any modesets
            //will detect no devices enabled.  If there is a modeset to each head at startup
            //this is safe.  If there were only one, and the other head had been enabled
            //in DOS but not in Windows, I think invalid CR3B settings may slip through.
        //    if (bStartup) { //May not be needed anymore -- explicit clear of what's enabled
        //                    // at startup.  Modesets will then enable whatever is needed.
        //        CRTC_WR(0x3B, cr3BhA & 0x0F, 0);
        //        CRTC_WR(0x3B, cr3BhB & 0x0F, 1);
        //    }

            RestoreLock(pDev, 0, lockA);
            RestoreLock(pDev, 1, lockB);

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR3B/0 register:", cr3BhA);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR3B/1 register:", cr3BhB);

            //Set startup display options based on BIOS CR3B settings.
            if (cr3BhA & 0x20) {
                curDispType[0] = DISPLAY_TYPE_MONITOR;
            } else if (cr3BhA & 0x40) {
                curDispType[0] = DISPLAY_TYPE_TV;
            } else if (cr3BhA & 0x80) {
                curDispType[0] = DISPLAY_TYPE_FLAT_PANEL; //external DFP
            } else {
                curDispType[0] = DISPLAY_TYPE_NONE;
            }
            if (cr3BhB & 0x10) {
                curDispType[1] = DISPLAY_TYPE_FLAT_PANEL; //internal LCD
            } else {
                curDispType[1] = DISPLAY_TYPE_NONE;
            }

            // Since the VGA modes do not support twinview modes, we may need a
            // way to infer that in Windows a second display should be enabled
            // (Since it won't be reflected in the CR3B scratch register settings).
            // We currently only apply this for Toshiba.  Dell's system BIOS
            // largely sets the values correctly, and they have agreed they'd
            // rather keep control of it themselves than have us add in logic
            // here.  God bless 'em.
            if (pDev->Dac.DeviceProperties.OverrideSBIOSDevs) {
                noCheckDevices = pDev->Dac.DevicesNoCheck;

                if(osReadRegistryDword(pDev, strDevNodeRM, strLastExtDevice, &i) == RM_OK) {
    //                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: read ext dev:", i);
                } else {
                    i = DISPLAY_TYPE_NONE;
                }
                switch (pDev->Dac.DeviceProperties.LastSwitchEvent) {
                case SWITCH_FROM_FS_DOS:
                    //If we were in spanning mode in Windows, restore to same devices.
                    // Otherwise switch to one device, with preference for LCD.
                    if (pDev->Dac.DeviceProperties.Spanning) {
                        curDispType[0] = i;
                        curDispType[1] = DISPLAY_TYPE_FLAT_PANEL; //internal LCD
                    } else {
                        //This should already be set properly by SBIOS coming from
                        // hibernation or cold boot, but for Win2K may not be set
                        // correctly returning from FS-DOS.
                        if (curDispType[1] != DISPLAY_TYPE_NONE)
                            curDispType[0] = DISPLAY_TYPE_NONE;
                    }
                    break;
                case SWITCH_FROM_COLD_START:
    #if 1
                    if (i != DISPLAY_TYPE_NONE) {
                        curDispType[0] = i;
                        curDispType[1] = DISPLAY_TYPE_FLAT_PANEL; //internal LCD
                    
                        // If we hibernate in LCD+CRT Clone mode, Toshiba wants to return 
                        // to LCD+CRT whether or not the CRT is actually present.  
                        // To fix this, we use pDev->Dac.DevicesNoCheck so that when 
                        // we call dacDevicesConnectStatus() later in this function,
                        // it will return TRUE always for that particular device.
                        // This fixes bug 32780.
      //                  noCheckDevices = pDev->Dac.DevicesNoCheck;
                    }
    #else
                    if (pDev->Dac.DeviceProperties.Spanning) {
                        curDispType[0] = DISPLAY_TYPE_MONITOR;
                        curDispType[1] = DISPLAY_TYPE_FLAT_PANEL; //internal LCD
                    } else {
                        //This should already be set properly by SBIOS coming from
                        // hibernation or cold boot, but for Win2K may not be set
                        // correctly returning from FS-DOS.
                        if (curDispType[1] != DISPLAY_TYPE_NONE)
                            curDispType[0] = DISPLAY_TYPE_NONE;
                    }
    #endif
                    break;
                case SWITCH_FROM_WINDOWS:
                default:
                    break;
                }
            }

            //
            // BUG WORKAROUND
            //
            // We used to do a device scan on every boot, but now with the new DEVICES_CONNECTED
            // API in place, this is only done on demand.
            //
            // Now with the Chrontel encoders Windows sometimes boots up all wavy.
            //
            // There appears to be a bug in programming the initial mode on the Chrontel 7007
            // TV encoder where some values are incorrect.  For whatever reason, we didn't
            // see this before because of the detection sequence that occurred before the
            // modeset.
            // 
            // In any event, asking the encoder to detect the presence of the TV clears 
            // everything up.
            //
            // Until we can find the underlying cause, force a detect even though we
            // don't need it.  This will act as a workaround until I can get someone
            // to debug the TV programming.
            //
            // PLEASE remove this as soon as possible.
            //
            // Only do this at boot time.
            if(bStartup)
                i = dacGetDisplayType(pDev, 0);  // don't really care what we get, just scan
        }

    } else {    //Desktop, not mobile
    //
    // Set default boot display type...first, use what the
    // CR28/CR33 registers tell us via the BIOS settings.
    //
        if (bStartup) {
            SETDISPLAYTYPE(pDev, 0, dacGetBiosDisplayType(pDev, 0));
            SETDISPLAYTYPE(pDev, 0, dacGetDisplayType(pDev, 0));
            curDispType[0] = GETDISPLAYTYPE(pDev, 0);
        }
    }


    //Determine which displays the VGA BIOS enabled at boot time, so that on mobile systems
    //we can have the same ones enabled in Windows.  The result is reported to the display
    //driver as a device bit mask rather than the old head-based system.
    // (We now do this re-detect at several other times, including resume from hibernation
    // and whenever the boot mask is requested).
    devmask = 0;   //clear boot device bitmask
    //Loop over all heads
    for (j = 0; j < MAX_CRTCS; j++) {   //Head
        //Loop over all list entries
        for (i = 0; (i < DCB_MAX_NUM_DEVDESCS) && (pDev->Dac.DCBDesc[i].DCBRecType != DCB_DEV_REC_TYPE_EOL); i++)
        {
//            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  *** Checking for: ", pDev->Dac.DCBDesc[i].DevTypeUnit);
            //If list entry is for this head...
            if (pDev->Dac.DCBDesc[i].DCBRecHead == (U032)(j?DCB_DEV_REC_HEAD_B:DCB_DEV_REC_HEAD_A))
            {
                //...and display type matches the BIOS's...
                U032 typematch = 0;
                switch (pDev->Dac.DCBDesc[i].DCBRecType) {
                case DCB_DEV_REC_TYPE_CRT:
                    if (curDispType[j] == DISPLAY_TYPE_MONITOR) typematch = 1;
                    break;
                case DCB_DEV_REC_TYPE_TV:
                    if (curDispType[j] == DISPLAY_TYPE_TV) typematch = 1;
                    break;
                case DCB_DEV_REC_TYPE_DD_SLINK:
                case DCB_DEV_REC_TYPE_DD_DLINK:
                case DCB_DEV_REC_TYPE_DD:
                    if (curDispType[j] == DISPLAY_TYPE_FLAT_PANEL) typematch = 1;
                    break;
                default:
                    break;
                }

                if (typematch) {
                    //...OR this entry's mask into the boot device mask
                    U032 temp = pDev->Dac.DCBDesc[i].DevTypeUnit;
//                    if (pDev->Power.MobileOperation != 2) //Toshiba doesn't want non-existence to prevent enabling a device
                        dacDevicesConnectStatus(pDev, &temp, noCheckDevices);   //Test for presence of this device
                    if (!temp) {
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Discarding as boot dev:", pDev->Dac.DCBDesc[i].DevTypeUnit);
                        curDispType[j] = DISPLAY_TYPE_NONE;
                    }
                    devmask |= temp;   //Device bitmask for device on this head.
                }
            }
        }

        if (bStartup && pDev->Power.MobileOperation)
            SETDISPLAYTYPE(pDev, j, curDispType[j]);
    }

    //What if nothing turned up?
    if (!devmask) {
        if (pDev->Power.MobileOperation) { 
            devmask = 0x1 << DAC_DD_OFFSET_IN_MAP;  // Fall back to the internal LCD
            curDispType[1] = DISPLAY_TYPE_FLAT_PANEL;
        } else {
            devmask = 0x1 << DAC_CRT_OFFSET_IN_MAP; // Fall back to a CRT
            curDispType[0] = DISPLAY_TYPE_MONITOR;
        }
        
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No boot device determined.  Using default.\r\n");
    }
    
    // Only assign the devmask to BootDevicesBitMap if we are mobile or doing
    // startup; desktop systems calling dacDetectDevices during non-startup
    // may build an incorrect devmask (and since the BootDevicesBitMap is
    // static on desktops, we don't want to reassign it anyway)
    
    if (pDev->Power.MobileOperation || bStartup)
        pDev->Dac.BootDevicesBitMap = devmask;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Boot device(s):", pDev->Dac.BootDevicesBitMap);

#ifndef NTRM
    // There may have been a change in which devices are connected during power-down.
    // When we resume, on mobile systems we emulate the state the system BIOS powered
    // up in.  Under Win2K, this is handled with ACPI events.  Under Win9x, we may generate
    // a fake hotkey event here which will switch the displays if needed.  Currently
    // only used with Toshiba.
    if (spoofKey && pDev->Power.MobileOperation) {
        if ((curDispType[0] != origDispType[0]) || (curDispType[1] != origDispType[1])) {
            U032 induceSwitch;
            induceSwitch = dispTypeToHotkeyLookup(0, curDispType[0]) | dispTypeToHotkeyLookup(1, curDispType[1]);
            hotkeyAddEvent(pDev, NV_HOTKEY_EVENT_DISPLAY_ENABLE, induceSwitch);
        }
    }
#endif

    return RM_OK;
}

static RM_STATUS dacInitCurrentDevices
(
    PHWINFO pDev
)
{
    U008 lockA, lockB, cr3BhA, cr3BhB;

    if (pDev->Power.MobileOperation) {

        // Unlock both heads
        lockA = UnlockCRTC(pDev, 0);
        lockB = UnlockCRTC(pDev, 1);
    
        //Read CR3B regs -- used below to set default display types.
        CRTC_RD(0x3B, cr3BhA, 0);
        CRTC_RD(0x3B, cr3BhB, 1);
        RestoreLock(pDev, 0, lockA);
        RestoreLock(pDev, 1, lockB);

        //Set startup display options based on BIOS CR3B settings.
        if (cr3BhA & 0x20) {
            pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = DISPLAY_TYPE_MONITOR;
        } else if (cr3BhA & 0x40) {
            pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = DISPLAY_TYPE_TV;
        } else if (cr3BhA & 0x80) {
            pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = DISPLAY_TYPE_FLAT_PANEL; //external DFP
        } else {
            pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = DISPLAY_TYPE_NONE;
        }
        
        if (cr3BhB & 0x10) {
            pDev->Dac.HalInfo.CrtcInfo[1].CurrentDisplayType = DISPLAY_TYPE_FLAT_PANEL; //internal LCD
        } else {
            pDev->Dac.HalInfo.CrtcInfo[1].CurrentDisplayType = DISPLAY_TYPE_NONE;
        }
    } else { // Non-mobile case already detected in dacDetectDevices.
        pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = pDev->Dac.HalInfo.CrtcInfo[0].DisplayType;
    }
    return RM_OK;
}

static RM_STATUS dacInitGetMobileEDIDFromBios(PHWINFO pDev)
{
    U032 rmStatus = RM_ERROR;
    U032 offset;
    BMP_Control_Block bmpCtrlBlk;
    BIOSFPTable biosFPTable;
    U008 fpTableIndex;
    U008 fpXlateTable[SIZE_FP_XLATE_TABLE];
    U032 useDynEDIDs;

    //The sequence of events here is
    //A.) Find the control block
    //B.) If correct ver/subver grab the BMP Table
    //C.) Get strappings from vbios call
    //D.) Read a translate table from the bios
    //E.) Use the straps to index into the translate table
    //F.) Take the entry at that index in the xlate talbe
    //    and that is the index of the FP_TABLE you want to
    //    use.
    //G.) Mult the xlate table entry by # bytes in an FP_TABLE
    //    then add this to the FP_TABLE ptr in the control blk.
    //    This gets you to the FP_TABLE you need.
    //H.) Read in the FP_TABLE and extract EDID DTD.

    //Check to see if this feature is desired, based on registry key setting:
    pDev->Dac.MobileEDID[0] = 1;    //Assure the EDID is invalid unless we get all the way through.
                                    // (checked in edidConstructMobileInfo())
    rmStatus = osReadRegistryDword(pDev, strDevNodeRM, "SoftEDIDs", &useDynEDIDs);
    if ((rmStatus != RM_OK) || !useDynEDIDs) {
        rmStatus = RM_ERROR;
    } else {
        rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);
    }

    if (rmStatus == RM_OK)
    {
        //According to the bios guys only Rev5 Subver14 BMP Tables
        //can be guaranteed to have this structure. We cannot 
        //necessarily assume this will stay the same in later subvers.
        if (bmpCtrlBlk.CTL_Version == BMP_INIT_TABLE_VER_5 &&
            bmpCtrlBlk.CTL_Flags >= BMP_INIT_TABLE_SUBVER_0x14)
        {
            BMP_Struc_Rev5_0x14 bmpInitTableInfo;

            rmStatus = BiosReadStructure(pDev, 
                                        (U008*)&bmpInitTableInfo,
                                        offset,
                                        (U032 *)0,
                                        BMP_INIT5_0x14_FMT);
            if (rmStatus == RM_OK)
            {
                //Read the 16 byte XLate table
                rmStatus = BiosReadBytes(pDev,
                                         fpXlateTable,
                                         bmpInitTableInfo.BMP_FPXlateTablePtr,
                                         sizeof( fpXlateTable )); 
                if (rmStatus == RM_OK)
                {
                    //Index into the XLate table to determine
                    //which FPTable we want.
                    fpTableIndex = fpXlateTable[pDev->Dac.PanelStrap];

                    //Multiply the index of the desired FPTable
                    //by the size of the FPTable and add that
                    //to the base pointer found in the BMP
                    //table. This should get us to exactly the
                    //FPTable we want.
                    rmStatus = BiosReadStructure(pDev,
                                                 &biosFPTable,
                                                 bmpInitTableInfo.BMP_FPTablePtr + 
                                                 (fpTableIndex * BMP_FP_TABLE_SIZE),
                                                 (U032 *)0, BMP_FP_TABLE_FMT);
                    if (rmStatus == RM_OK)
                    {
                        //Now that we have the FPTable, extract
                        //out everything we need to make an EDID.
                        dacInitFPTableToEDID(pDev, &biosFPTable, pDev->Dac.MobileEDID);
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: MobileEDID ptr:", (U032)(NV_UINTPTR_T)(pDev->Dac.MobileEDID));
                    }
                }
            }
        }
        else
        {
            rmStatus = RM_ERROR;
        }
    }

    return rmStatus;
}


//Looks up numerical value of a character interpreted as a hex digit,
// and shifts it in to the low nibble of an 8-bit result.
// Non-hex characters have no effect.  This automatically handles
// most leading/trailing characters.  Returns TRUE is a hex char was found.
BOOL convNibbleFromHex(U008 textchar, U008 *cumulative)
{
    if ((textchar >= '0') && (textchar <= '9'))
        *cumulative = (*cumulative << 4) | (textchar - '0');
    else if ((textchar >= 'A') && (textchar <= 'F'))
        *cumulative = (*cumulative << 4) | (textchar - 'A' + 10);
    else if ((textchar >= 'a') && (textchar <= 'f'))
        *cumulative = (*cumulative << 4) | (textchar - 'a' + 10);
    else
        return FALSE;

    return TRUE;
}


// Read Straps from bios and create Mobile EDID
VOID dacPreInitEDID(PHWINFO pDev)
{
    // Check to see if we've initialized the panel strap
    if(!(pDev->Dac.DevicesEnabled & DAC_PRE_EDID_INITIALIZED))
    {
        // Create Mobile EDID
        if (pDev->Power.MobileOperation)
        {
            // Get Panel Strap
            U032 eax, ebx, ecx, edx;
            // FP Straps come back in CL.
            eax = 0x4F14;
            ebx = 0x0186;
            ecx = edx = 0;
            
            // Call BIOS to get strap
            osCallVideoBIOS(pDev, &eax, &ebx, &ecx, &edx, NULL);
            
            // If the BIOS call fails, it will return 0x014f
            if (eax == 0x014F)
                pDev->Dac.PanelStrap = 0;  // BIOS call didn't support it
            else if (eax == 0x004F)
                pDev->Dac.PanelStrap = ecx & 0xF; //We only care about the lower nibble.
            else
            {
                // In WinXP, this call can occur before the int10 handler has been
                // hooked, so that osCallVideoBIOS doesn't get to our vbios. In this
                // case, don't set the DAC_PRE_EDID_INITIALIZED flag, so we can try
                // again on a subsequent call.
                pDev->Dac.PanelStrap = 0;
                return;
            }
        
            dacInitGetMobileEDIDFromBios(pDev);
        }
        else
            pDev->Dac.PanelStrap = 0;  // Panel straps only for Mobile BIOSes
    
        // Set flag so we don't have to run this function again.
        pDev->Dac.DevicesEnabled |= DAC_PRE_EDID_INITIALIZED;
    }
}


//Calculate and assign the EDID checksum, defined as the value that causes the
// sum of all 128 bytes, mod 256, to total 0.
static VOID dacInitCalculateEDIDChkSum(PHWINFO pDev, U008 *pEdid)
{
    U032 i;
    U008 chksum = 0;

    for (i = 0; i < 127; i++)
    {
        chksum += pEdid[i];
    }

    pEdid[0x7F] = (~chksum) + 1;
}


#define DD_BLOCK_LENGTH 18
#define DD_BLOCK_DESC_LENGTH 5
#define MAX_MON_STRING_LENGTH ((DD_BLOCK_LENGTH - DD_BLOCK_DESC_LENGTH) * 3)

static VOID dacInitFPTableToEDID(PHWINFO pDev, PBIOSFPTable pFPTable, U008 *pEdid)
{
    U032 i, j, RegHImageSize, RegVImageSize;
    U008 HActive, HBlank, HAcHBlnk;
    U008 HSyncOffset, HSyncPW, HImageSize, HBorder;
    U008 VActive, VBlank, VAcVBlnk;
    U008 VSyncOffset, VSyncPW, VImageSize, VBorder;
    U008 VSyncOffPW, HVSyncOffPW, HVImageSize;
    U008 Flags;
    U032 panelStrap = pDev->Dac.PanelStrap;
    U032 numbytes;
    U032 HexManufacturerName = 0, blknum = 0, curblk;
    char PanelXX[16];// = "Panel";
    char EDIDPanelXX[16];// = "EDIDPanel";
    char *manufacturername = "NVD";
    char panelname[MAX_MON_STRING_LENGTH];// = "Nvidia Default Flat Panel";
    char featuresupport = (U008)0xEA;
    char PanelRegKeyVal[800];   //Needs to be big enough to hold a string describing
                                //a full EDID (128 * x chars per entry, e.g. "3, a, 4" or "0x23, 0x50")

    /* XXX  temporary hack to work around gcc curiosity on linux: gcc was
       implicity inserting a memset into this code to clear the bytes of
       PanelXX[] from after "Panel" through the end of the array (and
       likewise for EDIDPanelXX[] and panelname[] -- any statically
       sized character array that was assigned a string smaller than
       the array size).  This results in an unresolved symbol when insmod'ing
       NVdriver into a 2.2.x kernel.  The quick 'n dirty solution is to
       explicitly clear the memory and strcpy the default string values into
       the arrays, though I don't quite understand what the difference is
       between gcc's inserted memset, and the memset called by osMemSet.
       My guess is that the preprocessor does something with the memset
       called by osMemSet to change it into a symbol defined by the kernel,
       while the memset inserted by gcc gets inserted after preprocessing,
       and thus does not get translated into a symbol defined by the
       kernel... that's only a guess, though.
       
       remove this hack as soon as we find a better solution. */

    osMemSet (PanelXX, 0, 16);
    osStringCopy (PanelXX, "Panel");

    osMemSet (EDIDPanelXX, 0, 16);
    osStringCopy (EDIDPanelXX, "EDIDPanel");

    osMemSet (panelname, 0, MAX_MON_STRING_LENGTH);
    osStringCopy (panelname, "Nvidia Default Flat Panel");
    
    //Just in case
    for (i = 0; i < 128; i++)
    {
        pEdid[i] = 0x00;
    }

    //Construct registry key for individual parameter overrides.
    // Format is 'Panel'+ a 2-digit strap index.
    i = osStringLength(PanelXX);
    PanelXX[i++] = DIGIT(1, panelStrap);
    PanelXX[i++] = DIGIT(0, panelStrap);
    PanelXX[i] = NULL;

    //XXX: Not really needed here. Can be moved to edidConstructMobileInfo
    //Construct registry key for full-EDID override.
    // Format is 'EDIDPanel'+ a 2-digit strap index.
    i = osStringLength(EDIDPanelXX);
    EDIDPanelXX[i++] = DIGIT(1, panelStrap);
    EDIDPanelXX[i++] = DIGIT(0, panelStrap);
    EDIDPanelXX[i] = NULL;

    //Now read out field-based overrides.
    // The fields are all packed into one comma-separated string, with most commonly
    // overriden elements first.  In order to skip a field in the string without
    // overriding it, follow the preceding comma immediately with another comma.
    // Field order: Manufacturer descriptor string, manufacturer ID (3 chars), Feature Support
    numbytes = sizeof(PanelRegKeyVal);
    if (osReadRegistryBinary(pDev, strDevNodeRM, PanelXX, (U008 *)PanelRegKeyVal, &numbytes) == RM_OK) 
    {
        i = j = 0;

        while ((PanelRegKeyVal[j] != ',') && (PanelRegKeyVal[j] != '\0'))
        {
            panelname[i++] = PanelRegKeyVal[j++];
        }
        if (i) {  //if this field had at least one char
            for (;i < MAX_MON_STRING_LENGTH; i++) panelname[i] = '\0';  //pad the rest of the field with NULLs
        }

        if (PanelRegKeyVal[j] != '\0') {
            j++;
            i = 0;

            //Extract manufacturer code
            while ((PanelRegKeyVal[j] != ',') && (PanelRegKeyVal[j] != '\0'))
            {
                manufacturername[i++] = PanelRegKeyVal[j++];
            }
//            if (i) manufacturername[i] = '\0';  //if this field had at least one char

            if (PanelRegKeyVal[j] != '\0') {
                U008 entry = 0;
                j++;
                i = 0;

                while ((PanelRegKeyVal[j] != ',') && (PanelRegKeyVal[j] != '\0'))
                {
                    if (convNibbleFromHex(PanelRegKeyVal[j++], &entry)) i++;
                }
                if (i) featuresupport = entry;  //if this field had at least one digit
            }
        }
    }

    //Convert to 5 bit compressed ASCII to encode in 2 bytes of the EDID.
    HexManufacturerName = (COMPRESSEDASCIIVAL(manufacturername[0]) & 0x1F) << 10;
    HexManufacturerName |= (COMPRESSEDASCIIVAL(manufacturername[1]) & 0x1F) << 5;
    HexManufacturerName |= COMPRESSEDASCIIVAL(manufacturername[2]) & 0x1F;


    // Extract out all the relevant bits from the BIOS FP_TABLE
    // We need to fill in a Detailed Timing Descriptor (DTD) in an EDID.
    // The following is from the VESA EDID spec (sec 3.16) and descibes 
    // why we have to do all the shifting and masking below.

    /****************************************************************/
    /* Bytes|   Detailed Timing Desc.   |           Format          */
    /* ------------------------------------------------------------ */
    /*  2   | Pixel Clock/10,000        | Stored LSB First          */
    /* ------------------------------------------------------------ */
    /*  1   | Horizontal Active         | Pixels, lower 8bits       */
    /* ------------------------------------------------------------ */
    /*  1   | Horizontal Blanking       | Pixels, lower 8bits       */
    /* ------------------------------------------------------------ */
    /*  1   | Horizontal Active:        | Upper nibble: Upper 4 bits*/
    /*      | Horizontal Blanking       | of Horizontal Active.     */
    /*      |                           | Lower nibble: Upper 4 bits*/
    /*      |                           | of Horzinontal Blanking   */
    /* ------------------------------------------------------------ */
    /*  1   | Vertical Active           | Lines, lower 8bits        */
    /* ------------------------------------------------------------ */
    /*  1   | Vertical Blanking         | Lines, lower 8bits        */
    /* ------------------------------------------------------------ */
    /*  1   | Vertical Active:          | Upper nibble: Upper 4 bits*/
    /*      | Vertical Blanking         | of Vertical Active.       */
    /*      |                           | Lower nibble: Upper 4 bits*/
    /*      |                           | of Vertical Blanking      */
    /* ------------------------------------------------------------ */
    /*  1   | Horizontal Sync Offset    | Pixelss, from blanking    */
    /*      |                           | starts. Lower 8 bits      */
    /* ------------------------------------------------------------ */
    /*  1   | Horiz Sync Pulse Width    | Pixels, lower 8 bits      */
    /* ------------------------------------------------------------ */
    /*  1   | Vertical Sync Offset:     | Upper nibble: lines, lower*/
    /*      | Vertical Sync Pulse Width | 4 bits of VSync Offset.   */
    /*      |                           | Lower nibble: lines, lower*/
    /*      |                           | 4 bits of VSync PW        */
    /* ------------------------------------------------------------ */
    /*  1   | Horizontal Sync Offset    | bits 7,6: upper 2bits HSO */
    /*      | HSync Pulse Width         | bits 5,4: upper 2bits HSPW*/
    /*      | Vertical Sync Offset      | bits 3,2: upper 2bits VSO */
    /*      | VSync Pulse Width         | bits 0,1: upper 2bits VSPW*/
    /* ------------------------------------------------------------ */
    /*  1   | Horizontal Image Size     | mm, lower 8 bits          */
    /* ------------------------------------------------------------ */
    /*  1   | Vertical Image Size       | mm, lower 8 bits          */
    /* ------------------------------------------------------------ */
    /*  1   | Horizontal & Vertical     | Upper nibble: upper 4bits */
    /*      | Image Size                | of Horiz Image Size.      */
    /*      |                           | Lower nibble: upper 4bits */
    /*      |                           | of Vert Image Size.       */
    /* ------------------------------------------------------------ */
    /*  1   | Horizontal Border         | Pixels, see vesa spec 3.12*/
    /* ------------------------------------------------------------ */
    /*  1   | Vertical Border           | Lines, see vesa spec 3.12 */
    /* ------------------------------------------------------------ */
    /*  1   | Flags                     | See Vesa Spec 3.16        */
    /* ------------------------------------------------------------ */

    HActive     = (U008)(pFPTable->HActive & 0xFF);
    HBlank      = (U008)(((pFPTable->HTotal - pFPTable->HActive) + 0x01) & 0xFF);
    HAcHBlnk    = (U008)(((pFPTable->HActive & 0x0F00) >> 4) | 
                        (((pFPTable->HTotal - pFPTable->HActive) >> 8) & 0x0F));
    VActive     = (U008)(pFPTable->VActive & 0xFF);
    VBlank      = (U008)(((pFPTable->VTotal - pFPTable->VActive) + 0x01) & 0xFF);
    VAcVBlnk    = (U008)(((pFPTable->VActive & 0x0F00) >> 4) | 
                  (((pFPTable->VTotal - pFPTable->VActive) >> 8) & 0x0F));
    HSyncOffset = (U008)(((pFPTable->HSyncStart - pFPTable->HActive) + 0x01) & 0xFF);
    HSyncPW     = (U008)((pFPTable->HSyncEnd - pFPTable->HSyncStart) & 0xFF);
    VSyncOffset = (U008)(((pFPTable->VSyncStart - pFPTable->VActive) + 0x01) & 0xFF);
    VSyncPW     = (U008)((pFPTable->VSyncEnd - pFPTable->VSyncStart) & 0xFF);
    VSyncOffPW  = (U008)(((VSyncOffset & 0x0F) << 4) | (VSyncPW & 0x0F));
    HVSyncOffPW = (U008)(((pFPTable->HSyncStart - pFPTable->HActive) & 0xC000)  |
                  (((pFPTable->HSyncEnd - pFPTable->HSyncStart) & 0xC000) >> 2) |
                  (((pFPTable->VSyncStart - pFPTable->VActive) & 0xC000) >> 4)  |
                  (((pFPTable->VSyncEnd - pFPTable->VSyncStart) & 0xC000) >> 6));
    HBorder     = (U008)(((pFPTable->HValidEnd - pFPTable->HDispEnd) & 0xFF));
    VBorder     = (U008)(((pFPTable->VValidEnd - pFPTable->VDispEnd) & 0xFF));
    //OR 0x18 into flags. This means:Non-Interlaced, No Stereo, Digital Separate.
    //The lower 2 bits of pFPTable->Flags tells us the sync polarities.
//    Flags       = (U008)(0x18 | (pFPTable->Flags & 0x03)); 
    //Meaning of bits 1 & 2 vary with setting, but for Digital Separate they are
    //Vsync polarity (bit 0 in BMP entry) and Hsync polarity (bit 4 in BMP entry)
    Flags       = (U008)(0x18 | ((pFPTable->Flags << 2) & 0x04) | ((pFPTable->Flags >> 3) & 0x02)); 

    //11 inches x 7 inches
    RegHImageSize = 0x011E;
    RegVImageSize = 0x00D7;

    HImageSize  = (U008)(RegHImageSize & 0xFF);
    VImageSize  = (U008)(RegVImageSize & 0xFF);
    HVImageSize = (U008)(((RegHImageSize & 0x0F00) >> 4) | ((RegVImageSize & 0x0F00) >> 8));

    //Construct a valid version 1 EDID header {00, FF, FF, FF, FF, FF, FF, 00}
    (*(U032*)pEdid)      = 0xFFFFFF00;
    (*(U032*)(pEdid +4)) = 0x00FFFFFF;

    pEdid[0x8] = (U008)((HexManufacturerName & 0xFF00) >> 8);
    pEdid[0x9] = (U008)(HexManufacturerName & 0x00FF);

    pEdid[0xB] = (U008)panelStrap; //Encode strapping number as a monitor ID digit.
                                   //This makes each EDID unique from Windows' perspective.

    //Fill in a version of 0x01
    //Fill in a revision of 0x03
    pEdid[0x12] = 0x01;
    pEdid[0x13] = 0x03;

    //Basic Display parameters
    pEdid[0x14] = 0x80; //Digital Signal level
    pEdid[0x15] = 0x1D; //Max Horiz Image Size (cm)
    pEdid[0x16] = 0x16; //Max Vert Image Size (cm)

    //Set Feature Support to indicate that we support
    //Standby, Suspend, Non-RGB MultiColor display,
    //and our prefered timing is in the first DTD block
    //(defined below).
    pEdid[0x18] = featuresupport; //0xCA;

    //Fill standard timings with 0x01 (means 'unused' in EDID parlance)
    *(U032*)(pEdid + 0x026) = 0x01010101;
    *(U032*)(pEdid + 0x02A) = 0x01010101;
    *(U032*)(pEdid + 0x02E) = 0x01010101;
    *(U032*)(pEdid + 0x032) = 0x01010101;

    //Begin Detailed Timing Description
    pEdid[0x36] = (U008)(pFPTable->PixClk & 0x00FF);
    pEdid[0x37] = (U008)((pFPTable->PixClk & 0xFF00) >> 8);
    pEdid[0x38] = HActive;
    pEdid[0x39] = HBlank;
    pEdid[0x3A] = HAcHBlnk;
    pEdid[0x3B] = VActive;
    pEdid[0x3C] = VBlank;
    pEdid[0x3D] = VAcVBlnk;
    pEdid[0x3E] = HSyncOffset;
    pEdid[0x3F] = HSyncPW;
    pEdid[0x40] = VSyncOffPW;
    pEdid[0x41] = HVSyncOffPW;
    pEdid[0x42] = HImageSize;
    pEdid[0x43] = VImageSize;
    pEdid[0x44] = HVImageSize;
    pEdid[0x45] = HBorder;
    pEdid[0x46] = VBorder;
    pEdid[0x47] = Flags;

    //Fill in panel name.  LPL: should we enforce a null terminator as last char?
    i = 0;
    while (i < MAX_MON_STRING_LENGTH)
    {
        curblk = 0x48 + (blknum * DD_BLOCK_LENGTH);
        pEdid[curblk+3] = 0xFC; //header is
        for (j = DD_BLOCK_DESC_LENGTH; j < DD_BLOCK_LENGTH; j++, i++)
        {
            if (panelname[i] != NULL)
            {
                pEdid[0x48 + (blknum * DD_BLOCK_LENGTH) + j] = panelname[i];
            }
        }
        blknum++;
        if (panelname[i] == NULL) break;
    }


    //Check if there is a full-EDID override key.  Parsing assumes a hexadecimal, comma-separated list.
    //Should we enforce a correct checksum, ignore it, or calculate it for them?  Currently we calc
    // it for them.
    if (osReadRegistryBinary(pDev, strDevNodeRM, EDIDPanelXX, (U008 *)PanelRegKeyVal, &numbytes) == RM_OK) 
    {
        U008 entry;
        i = 0;
        //Parsing presumes a comma-separate list, but is otherwise robust enough for
        // most hex digit formats. (padded to 2 digits or not, leading or trailing $, h, 0x, or none, etc.)
        for (j = 0; j <= numbytes; j++) {   //include terminating null char
            if ((PanelRegKeyVal[j] == ',') || (PanelRegKeyVal[j] == '\0')) {
                pEdid[i++] = entry;
                entry = 0;
            } else {
                convNibbleFromHex(PanelRegKeyVal[j], &entry);
            }
        }
    }


    //Make sure Num 128byte blocks to follow is 0
    pEdid[0x7E] = 0x00;
    //Make sure Checksum makes the edid add up to 0 
    dacInitCalculateEDIDChkSum(pDev, pEdid);
}


