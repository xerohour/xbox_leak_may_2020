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
BOOL dacTVConnected(PHWINFO);

BOOL dacMonitorConnectStatus(PHWINFO,U032);
BOOL dacFlatPanelConnectStatus(PHWINFO pDev, U032);

static U032 dacGetDisplayType(PHWINFO, U032);

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
    U032 data, data32;
#ifndef IKOS
    U008 lockA, lockB, cr3BhA, cr3BhB;
    U008 data08;
#endif
    U032 freq = 0;

    nvHalDacControl(pDev, DAC_CONTROL_INIT);

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
    // Initialize flags.
    //
    pDev->Dac.UpdateFlags = 0;

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
    // Set current desktop state to default.
    //
    pDev->Dac.DesktopState = NV_DAC_DESKTOP_STATE_DEFAULT;

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

    // Get TV output format.
    if (dacReadTVOutFromRegistry(pDev, 0, &data32) == RM_OK)
        pDev->Dac.TVoutput = data32;
    else
        pDev->Dac.TVoutput = 0xffffffff;

    // Set TV format to be what the BIOS chose
    CRTC_RD(NV_CIO_CRE_SCRATCH0__INDEX, data08, 0);
    pDev->Dac.TVStandard = data08 & 0x7;

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

    if (pDev->Power.MobileOperation) {

        EnableMobileHotkeyHandling(pDev);

        lockA = ReadCRTCLock(pDev, 0);           //Back up lock state
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);    //Unlock CRTC extended regs
        lockB = ReadCRTCLock(pDev, 1);           //Back up lock state
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 1);    //Unlock CRTC extended regs

        //Read CR3B regs -- used below to set default display types.
        // Write back the registers with the 'enable' bits cleared.
        // They will be set as needed via display driver calls to dacSetModeMulti.
        CRTC_RD(0x3B, cr3BhA, 0);
        CRTC_WR(0x3B, cr3BhA & 0x0F, 0);

        CRTC_RD(0x3B, cr3BhB, 1);
        CRTC_WR(0x3B, cr3BhB & 0x0F, 1);

        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lockA, 0); // Restore lock state
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lockB, 0); // Restore lock state
    }

    if ((pDev->Power.MobileOperation) && ((pDev->Chip.BiosRevision & 0xFF0000FF) >= 0x03000019)) {
    //Set startup display options based on BIOS CR3B settings.
    /*        if (cr3BhA & 0x10) {
            data = DISPLAY_TYPE_FLAT_PANEL; //internal LCD
        } else */
        if (cr3BhA & 0x20) {
            data = DISPLAY_TYPE_MONITOR;
        } else if (cr3BhA & 0x40) {
            data = DISPLAY_TYPE_TV;
        } else if (cr3BhA & 0x80) {
            data = DISPLAY_TYPE_FLAT_PANEL; //external DFP
        } else {
            data = DISPLAY_TYPE_NONE;
        }

        SETDISPLAYTYPE(pDev, 0, data);

        if (cr3BhB & 0x10) {
            data = DISPLAY_TYPE_FLAT_PANEL; //internal LCD
/*        } else if (cr3BhB & 0x20) {
            data = DISPLAY_TYPE_MONITOR;
        } else if (cr3BhB & 0x40) {
            data = DISPLAY_TYPE_TV;
        } else if (cr3BhB & 0x80) {
            data = DISPLAY_TYPE_FLAT_PANEL; //external DFP
            */
        } else {
            data = DISPLAY_TYPE_NONE;
        }

        SETDISPLAYTYPE(pDev, 1, data);

        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR3B/0 register init:", cr3BhA);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR3B/1 register init:", cr3BhB);

    } else {

    //
    // Set default boot display type...first, use what the
    // CR28/CR33 registers tell us via the BIOS settings.
    //
        SETDISPLAYTYPE(pDev, 0, dacGetBiosDisplayType(pDev, 0));
        SETDISPLAYTYPE(pDev, 0, dacGetDisplayType(pDev, 0));
    }


    //Determine which displays the VGA BIOS enabled at boot time, so that on mobile systems
    //we can have the same ones enabled in Windows.  The result is reported to the display
    //driver as a device bit mask rather than the old head-based system.
    data = 0;   //clear boot device bitmask
    //Loop over all heads
    for (j = 0; j < MAX_CRTCS; j++) {   //Head
//        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "*** Head:", j);
        k = GETDISPLAYTYPE(pDev, j);    //Device on this head
        //Loop over all list entries
        for (i = 0; (i < DCB_MAX_NUM_DEVDESCS) && (pDev->Dac.DCBDesc[i].DCBRecType != DCB_DEV_REC_TYPE_EOL); i++)
        {
//            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  *** Checking for: ", pDev->Dac.DCBDesc[i].DevTypeUnit);
            //If list entry is for this head...
            if (pDev->Dac.DCBDesc[i].DCBRecHead == (U032)(j?DCB_DEV_REC_HEAD_B:DCB_DEV_REC_HEAD_A))
            {
                //...and display type matches the BIOS's...
                if (((pDev->Dac.DCBDesc[i].DCBRecType == DCB_DEV_REC_TYPE_CRT) && (k == DISPLAY_TYPE_MONITOR))
                    || ((pDev->Dac.DCBDesc[i].DCBRecType == DCB_DEV_REC_TYPE_DD_SLINK) && (k == DISPLAY_TYPE_FLAT_PANEL))
                    || ((pDev->Dac.DCBDesc[i].DCBRecType == DCB_DEV_REC_TYPE_TV) && (k == DISPLAY_TYPE_TV)))
                {
                    //...OR this entry's mask into the boot device mask
                    data |= pDev->Dac.DCBDesc[i].DevTypeUnit;   //Device bitmask for device on this head.
                }
            }
        }
    }
    pDev->Dac.BootDevicesBitMap = data;
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Boot device(s):", pDev->Dac.BootDevicesBitMap);

    //LPL: This forcing of the display type in the mobile case is a workaround
    // while the display driver still determines which display we booted on (or
    // which display is the primary, in extended mode) using the old method.
    // Once the switch to the new, improved method implemented above (using
    // NV_CFG_GET_BOOT_DEVICES) is complete, the code below should be deleted.
    if (pDev->Power.MobileOperation && (GETDISPLAYTYPE(pDev, 1) == DISPLAY_TYPE_FLAT_PANEL))
        SETDISPLAYTYPE(pDev, 0, DISPLAY_TYPE_FLAT_PANEL);
    //


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
    // Initialize CR44 contents to UNKNOWN 
    //
    pDev->Dac.HalInfo.Head2RegOwner = HEAD2_OWNER_UNKNOWN;

    //
    // Determine which head was used to boot the system
    //
    // NEED TO DO: Get this from the BIOS, but for now if we're
    // a mobile system booting the DFP let's get the drivers to swing
    // over to the secondary head.
    // LPL : preliminary version that reads from BIOS in place above.
    //
    pDev->Dac.HalInfo.InitialBootHead = 0;
#ifdef MACOS
    if (pDev->Dac.HalInfo.NumCrtcs > 1)    // ie: NV11
        if (GETDISPLAYTYPE(pDev, 1) == DISPLAY_TYPE_FLAT_PANEL)
            pDev->Dac.HalInfo.InitialBootHead = 1;
#else
    if (pDev->Power.MobileOperation && (GETDISPLAYTYPE(pDev, 0) == DISPLAY_TYPE_FLAT_PANEL))
        pDev->Dac.HalInfo.InitialBootHead = 1;
#endif

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
    else if (dacVGAConnectStatus(pDev, Head))
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
