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

/******************************* DAC Flat panel Control ********************\
*                                                                           *
* Module: DACFP.C                                                           *
*   Mode set for flat panel is done here.                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <dac.h>
#include <os.h>
#include <nvcm.h>
#include <edid.h>
#include <devinit.h>
#include "nvhw.h"


#if defined(NTRM) && !defined(WriteIndexed)
// For NT these are in modehw.h which is in the miniport.
VOID WriteIndexed(U032 addr, U016 datum);
U016 ReadIndexed(U032 addr, U008 reg);
#endif //NTRM

// Const strings for accessing the registry
extern char strFpMode[];

//*************************** Exported functions *****************************************

RM_STATUS dacAdjustCRTCForFlatPanel
(
    PHWINFO pDev, 
    U032 Head,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac
)
{
    U032    Ht, Hre, Vt, Vrs, Vre, Vbs, data32;
    U016    Seq, Vendor;
    U008    Cr03, Cr04, Cr05, Cr07, Cr09, Cr11, Cr25, Cr28, Cr2d, Cr33;
#if 0
    U032 cntl32;
#endif

    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);   // power on flat panel

#if 0 // If TMDS not powered up, it will hang
    //
    // First, check if the internal TMDS exists, by trying to read/write
    // the TMDS_CONTROL register. If it exists, only power up the internal
    // TMDS, if we're not using an external one.
    //
    cntl32 = DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _DISABLE) | NV_PRAMDAC_INDIR_TMDS_PLL1;
    REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, cntl32);
    if (REG_RD32(NV_PRAMDAC_FP_TMDS_CONTROL) == cntl32)
    {
        // The internal TMDS exists, see if we're using the external TMDS
        if (REG_RD_DRF(_PRAMDAC, _FP_TEST_CONTROL, _TMDS) == NV_PRAMDAC_FP_TEST_CONTROL_TMDS_EXTERNAL)
            FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _TMDS);
    }
#endif
    // check for pixel doubled modes
    if ((GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_MONITOR) &&
        (pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth < 640))
    {
        Seq = ReadIndexed(NV_PRMVIO_SRX, 1);
        Seq |= 0x800;   // divide pixel clock to double size
        WriteIndexed(NV_PRMVIO_SRX, Seq);
    }        
    // Make sure attr is set correctly
    Cr07 = REG_RD08(NV_PRMCIO_INP0__COLOR); // read 3DA to toggle (data = don't care)
    REG_WR08(NV_PRMCIO_ARX, 0x30);          // select reg 0x10, bit 5 set to access reg (not palette)
    REG_WR08(NV_PRMCIO_ARX, 0x01);          // clear bit 5

    // New rules
    // VRS = VT - 3
    // VRE = VT - 2
    // HRS = HT - 3
    // HRE = HT - 2
    // HBE = HT + 4  This is required for the overlay to sync (videos will be offset).
    CRTC_RD(NV_CIO_CRE_HEB__INDEX, Cr2d, Head);
    CRTC_RD(NV_CIO_CR_HDT_INDEX, Ht, Head);
    if (Cr2d & BIT(0))
        Ht |= BIT(8);
    Cr04 = (U008)Ht - 3; // everyone else except SGI
    // SGI is "special". We have to check for their panel and adjust HRS.
    if (pDev->Dac.CrtcInfo[Head].EDID != 0)
    {
    // EDID Version 1 version field should never be 1 on a version 2 (it's an ascii field in V2).
        if (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VERSION_NUMBER_INDEX] == 1)
        {
            Vendor = (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VENDOR_ID_INDEX+1])<<8 | pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VENDOR_ID_INDEX];
            if (Vendor == 0xf84c)
            {
                Cr04 = (U008)Ht; // All modes
//                if ( (pDev->Framebuffer.HalInfo.HorizDisplayWidth == 640) &&
//                     (pDev->Framebuffer.HalInfo.VertDisplayWidth == 480))
//                {   
//                    Cr04 = (U008)Ht+2;
//                }
            }                              
        }
    }
    CRTC_WR(NV_CIO_CR_HRS_INDEX, Cr04, Head);        // HRS
    if (Ht & BIT(8))
        Cr2d |= 8;
    CRTC_WR(NV_CIO_CRE_HEB__INDEX, Cr2d, Head);   // HRS msb
    
    CRTC_RD(NV_CIO_CR_HRE_INDEX, Hre, Head);    // read CR05 and mask 4:0
    Hre &= 0xe0;
    CRTC_WR(NV_CIO_CR_HRE_INDEX, (Ht-2) | Hre, Head);   // HRE = Ht - 2, CR04[4:0]
    
    CRTC_RD(NV_CIO_CR_OVL_INDEX, Cr07, Head);
    CRTC_RD(NV_CIO_CRE_LSR_INDEX, Cr25, Head);
    CRTC_RD(NV_CIO_CR_HBE_INDEX, Cr03, Head);
    // HBE = HT + 4. HBE[6] = CR25[4], HBE[5] = CR5[7], HBE[4:0] = CR3[4:0]
    Cr03 &= 0xE0;   // lose 4:0, the HBE bits
    Cr03 |= (Ht + 4) & 0x1f;
    CRTC_WR(NV_CIO_CR_HBE_INDEX, Cr03, Head);     // HBE[4:0]
    CRTC_RD(NV_CIO_CR_HRE_INDEX, Cr05, Head);
    Cr05 &= 0x7f;
    if ((Ht + 4) & BIT(5))
        Cr05 |= BIT(7);
    CRTC_WR(NV_CIO_CR_HRE_INDEX, Cr05, Head);     // HBE[5]
    Cr25 &= 0xEF;
    if ((Ht + 4) & BIT(6))
        Cr25 |= BIT(4);
    CRTC_WR(NV_CIO_CRE_LSR_INDEX, Cr25, Head);    // HBE[6]

    // gather VT bits
    CRTC_RD(NV_CIO_CR_VDT_INDEX, Vt, Head);
    if (Cr07 & BIT(0))  // bit 8 of VT
        Vt |= BIT(8);
    if (Cr07 & BIT(5))  // bit 9 of VT
        Vt |= BIT(9);
    if (Cr25 & BIT(0))  // bit 10 of VT
        Vt |= BIT(10);

    // VRS: CR10,7,25
    Vrs = Vt - 3;
    CRTC_WR(NV_CIO_CR_VRS_INDEX, Vrs, Head);
    Cr07 &= ~(BIT(7) | BIT(2)); // clear bits 7 and 2
    if (Vrs & BIT(8)) Cr07 |= BIT(2);
    if (Vrs & BIT(9)) Cr07 |= BIT(7);
    CRTC_WR(NV_CIO_CR_OVL_INDEX, Cr07, Head);
    if (Vrs & BIT(10)) Cr25 |= BIT(2);
    CRTC_WR(NV_CIO_CRE_LSR_INDEX, Cr25, Head);
    
    // VRE: CR11[3:0]
    Vre = (Vt - 2) & 0x0f;
    CRTC_RD(NV_CIO_CR_VRE_INDEX, Cr11, Head);
    Vre |= (Cr11 & 0xf0);
    CRTC_WR(NV_CIO_CR_VRE_INDEX, Vre, Head);
    
    // VBS = VRS
    CRTC_RD(NV_CIO_CR_CELL_HT_INDEX, Cr09, Head);
    Vbs = Vrs;
                
    // write VBS            
    Cr07 &= ~BIT(3); // clear bit 3
    if (Vbs & BIT(8)) Cr07 |= BIT(3);
    Cr09 &= ~BIT(5); // clear bit 5
    if (Vbs & BIT(9)) Cr09 |= BIT(5);
    Cr25 &= ~BIT(3); // clear bit 3
    if (Vbs & BIT(10)) Cr25 |= BIT(3);
    CRTC_WR(NV_CIO_CR_VBS_INDEX, Vbs, Head);
    CRTC_WR(NV_CIO_CR_OVL_INDEX, Cr07, Head);
    CRTC_WR(NV_CIO_CR_CELL_HT_INDEX, Cr09, Head);
    CRTC_WR(NV_CIO_CRE_LSR_INDEX, Cr25, Head);
            
    data32 = DAC_REG_RD32(NV_PRAMDAC_FP_DEBUG_0, Head);    // usual macro won't help us here
    data32 &= ~(NV_PRAMDAC_FP_DEBUG_0_TEST_BOTH << 16);    // clear VCNTR bits
    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, data32, Head);

    // Slave the CRTC to timing generator
    // Setup the crtc to work in slave mode (cr28).
    //
    CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);
    Cr28 |= BIT(7);
    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head); 
    CRTC_RD(0x33, Cr33, Head);   // NV_CIO_CRE_LCD__INDEX
    Cr33 |= BIT(0);     // bit 0 is HW
    Cr33 |= BIT(1);     // bit 1 is BIOS flag
    CRTC_WR(0x33, Cr33, Head);   //NV_CIO_CRE_LCD__INDEX
                          
    return RM_OK;
}

VOID dacTMDSWrite
(
    PHWINFO pDev,
    U032    Head,
    U008    Addr,
    U008    Data
)
{
    // Due to a bug in NV15, the procedure is more complicated than intended:
    // Write the index of the register with WRITE DISABLE set.
    // Write the data.
    // Write the index with WRITE DISABLED cleared.
    // Write the index with WRITE DISABLED set.
    DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _DISABLE) | Addr, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_DATA, Data, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _ENABLE) | Addr, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _DISABLE) | Addr, Head);
}


U008 dacTMDSRead
(
    PHWINFO pDev,
    U032    Head,
    U008    Addr
)
{
    U008 Data;

    // Write the index of the register with WRITE DISABLE set.
    DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _DISABLE) | Addr, Head);
    // Read the data.
    Data = (U008) ( DAC_REG_RD32(NV_PRAMDAC_FP_TMDS_DATA, Head) & 0xFF );
    return Data;
}


RM_STATUS dacSetFlatPanelMode
(
    PHWINFO pDev,
    U032    Head,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac,
    U032    FpMode,
    BOOL    commitChange
)
{
U032    data32, cntl32;
U008    est_tim1, est_tim2;
U032    fpHActive=0, fpHDispEnd=0, fpHValidStart=0, fpHValidEnd=0, fpHSyncStart=0, fpHSyncEnd=0, fpHTotal=0;
U032    fpVActive=0, fpVDispEnd=0, fpVValidStart=0, fpVValidEnd=0, fpVSyncStart=0, fpVSyncEnd=0, fpVTotal=0;
U032    fpPixClk, fpPolarity=0, M, N, P, pixels, refresh;
BOOL    ModeValid;

    RM_STATUS rmStatus = RM_OK;

    // *********************************************************
    // Power On Flat Panel TMDS and FPCLK
    // Later in dacAdjustCRTCForFlatPanel() we will power
    // down the internal TMDS if we are using an external
    // one.  -paul
    // *********************************************************
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);
    
    // *********************************************************
    // We need to stall after powering on the flat panel TMDS
    // and FPCLK and before we start programming the setup 
    // registers since there is a bit of instability immediately
    // after powerup. On win9x, we will get noise on the FP 
    // without this stall. By trial and error, we found that a 
    // 0x400000 nanosecond delay is sufficient. -paul
    // *********************************************************
    tmrDelay(pDev, 0x400000);
    // Set the scale factor in FP_DEBUG back to 0 in case the BIOS used this to get a special
    // mode, i.e. Japanese DOS. This is only done on NV10. 
    // In Windows modes, the scaling is always set by HW comparing the values in the FP timing
    // registers and the CRTC registers, so the DEBUG registers are always set to 0. 
    // For DOS 475 lines, this method doesn't work, because the scaling value is not in the HW 
    // tables on NV10, so the BIOS has to work around this by setting these DEBUG registers.
    
    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_1, 0, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_2, 0, Head);
#ifndef IKOS
    if (FpMode > NV_CFGEX_GET_FLATPANEL_INFO_NATIVE)
    {
        return RM_ERROR;
    }
#else
    FpMode = NV_CFGEX_GET_FLATPANEL_INFO_NATIVE;
#endif
    // read strapping bit to set proper control width
    if (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_FP_IFACE) == NV_PEXTDEV_BOOT_0_STRAP_FP_IFACE_24BIT)
    {
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _WIDTH, _24, Head);
    }
    else
    {
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _WIDTH, _12, Head);
    }
    // set scaling mode
    DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, FpMode, Head);

    // If we are going native, we must reprogram the dac with new timing
    // We saved the EDID in pDev->Dac.CrtcInfo[Head].EDID[].
    if ((FpMode == NV_CFGEX_GET_FLATPANEL_INFO_NATIVE) &&
        (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL))
    {
        // Check detailed timings first
        DACFPTIMING fpTim;
        U008 block;
        
        fpPixClk = 0;
        for (block=0; block<NUM_DET_TIM; block++)           
        {
            if (edidParseDetailedTimingBlock(pDev, block, &fpTim, Head))
            {
                // Check whether this timing block is for the mode requested
                //  or the doubled mode
                ModeValid = FALSE;
                switch  (pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth)
                {
                    case 1600:
                        if (fpTim.HActive == 1600)
                        {
                            if ((fpTim.VActive == 1200) && (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 1200)) 
                                ModeValid = TRUE;
                            if ((fpTim.VActive == 1024) && (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 1024))
                                ModeValid = TRUE;
                        }
                        break;
                    case 1280:
                        if (fpTim.HActive == 1280) 
                        {
                            if ((fpTim.VActive == 1024) && (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 1024))
                                ModeValid = TRUE;
                            if ((fpTim.VActive == 960) && (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 960))
                                ModeValid = TRUE;
                        }
                        break;
                    case 1024:
                    case 512:
                        if (fpTim.HActive == 1024)
                            if (fpTim.VActive == 768)
                                ModeValid = TRUE;
                        break;
                    case 800:
                    case 400:
                        if (fpTim.HActive == 800)
                            if (fpTim.VActive == 600)
                                ModeValid = TRUE;
                        break;
                    case 640:
                        if (fpTim.HActive == 640) // can be 640x480 or 640x400
                        {
                            if ((fpTim.VActive == 480) && (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 480))
                                ModeValid = TRUE;
                            if ((fpTim.VActive == 400) && (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 400))
                                ModeValid = TRUE;
                        }
                        break;
                    case 320:
                        if (fpTim.HActive == 640) // can be 640x480 or 640x400
                        {
                            if ((fpTim.VActive == 480) && (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 240))
                                ModeValid = TRUE;
                            if ((fpTim.VActive == 400) && (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 200))
                                ModeValid = TRUE;
                        }
                        break;
                }
                if (ModeValid) // Use detailed timing
                {
                    fpPixClk = fpTim.PixClk;
                    fpHActive = fpTim.HActive;
                    fpHDispEnd = fpTim.HDispEnd;
                    fpHValidEnd = fpTim.HValidEnd;
                    fpHSyncStart = fpTim.HSyncStart;
                    fpHSyncEnd = fpTim.HSyncEnd;
                    fpHValidStart = fpTim.HValidStart;
                    fpHTotal = fpTim.HTotal - 1;
                    
                    fpVActive = fpTim.VActive;
                    fpVDispEnd = fpTim.VDispEnd;
                    fpVValidEnd = fpTim.VValidEnd;
                    fpVSyncStart = fpTim.VSyncStart;
                    fpVSyncEnd = fpTim.VSyncEnd;
                    fpVValidStart = fpTim.VValidStart;
                    fpVTotal = fpTim.VTotal - 1;
                    fpPolarity = (fpTim.Flags); // assume 4&3=1 (digital), then 2=vsync, 1=hsync
                }                       
            } // if parseDetailedTimings
        }
#if IKOS // no EDID
        fpPixClk = 0;
#endif
        if (!fpPixClk)  // not found in detailed, check established
        {
            est_tim1 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_ESTABLISHED_TIMING_1_INDEX];
            est_tim2 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_ESTABLISHED_TIMING_2_INDEX];
            fpPolarity = 0x3;   // estab timings use positive polarities on syncs

#if IKOS // give them whatever they ask for
            est_tim1 = 0xff;
            est_tim2 = 0xff;
#endif

            // Find requested refresh rate
            pixels = pVidLutCurDac->HalObject.Dac[0].TotalWidth * pVidLutCurDac->HalObject.Dac[0].TotalHeight;
            refresh = (pVidLutCurDac->HalObject.Dac[0].PixelClock*10000) / pixels;

            // if it didn't come out exactly, set to nearest choice
            // established timings are limited to 60, 70, 72, and 75
            if (refresh < 62)
                refresh = 60;
            else 
                if (refresh < 72)
                    refresh = 70;
                else 
                    if (refresh < 74)
                        refresh = 72;
                    else
                        refresh = 75;

            switch (pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth)
            {
                case 320:
                case 640:   // 640x400 is not an established timing, but we can do 640x480 panel timing and send 640x400 data within this.
                    if ((pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight==480) 
                        || (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight==240)
                        || (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight==400)
                        || (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight==200))
                    {
                        if ((refresh == 72) && (est_tim1 & EDID_ESTABLISHED_TIMING_640x480_72))
                        {
                            fpHActive =  640;
                            fpHDispEnd = 639;
                            fpHValidStart = 0;
                            fpHValidEnd = 639;
                            fpHSyncStart = 663;
                            fpHSyncEnd =  663+40;
                            fpHTotal = 831;
                            fpVActive = 480;
                            fpVDispEnd = 479;
                            fpVValidStart = 0;
                            fpVValidEnd =  479;
                            fpVSyncStart = 487;
                            fpVSyncEnd = 487+3;
                            fpVTotal = 519;
                            fpPixClk = 3150;    // 31.5 MHz
                            fpPolarity = VSYNC_NEG | HSYNC_NEG;
                        }
                        else if ((refresh == 75) && (est_tim1 & EDID_ESTABLISHED_TIMING_640x480_75))
                        {
                            fpHActive =  640;
                            fpHDispEnd = 639;
                            fpHValidStart = 0;
                            fpHValidEnd = 639;
                            fpHSyncStart = 655;
                            fpHSyncEnd =  655+64;
                            fpHTotal = 839;
                            fpVActive = 480;
                            fpVDispEnd = 479;
                            fpVValidStart = 0;
                            fpVValidEnd =  479;
                            fpVSyncStart = 480;
                            fpVSyncEnd = 480+20;
                            fpVTotal = 480+20;
                            fpPixClk = 3150;    // 31.5 MHz
                            fpPolarity = VSYNC_NEG | HSYNC_NEG;
                        }
                        // if none of the above, use 60 Hz
                        else if (est_tim1 & EDID_ESTABLISHED_TIMING_640x480_ANY)
                        {
                            fpHActive =  640;
                            fpHDispEnd = 639;
                            fpHValidStart = 0;
                            fpHValidEnd = 639;
                            fpHSyncStart = 655;
                            fpHSyncEnd =  655+96;
                            fpHTotal = 799;
                            fpVActive = 480;
                            fpVDispEnd = 479;
                            fpVValidStart = 0;
                            fpVValidEnd =  479;
                            fpVSyncStart = 489;
                            fpVSyncEnd = 489+2;
                            fpVTotal = 524;
                            fpPixClk = 2517;    // 25.17 MHz
                            fpPolarity = VSYNC_NEG | HSYNC_NEG;
                        }
                    }
                    break;
                case 400:
                case 800:
                    if ((refresh == 72) && (est_tim2 & EDID_ESTABLISHED_TIMING_800x600_72))
                    {
                        fpHActive =  800;
                        fpHDispEnd = 799;
                        fpHValidStart = 0;
                        fpHValidEnd = 799;
                        fpHSyncStart = 855;
                        fpHSyncEnd =  855+120;
                        fpHTotal = 1040;
                        fpVActive = 600;
                        fpVDispEnd = 599;
                        fpVValidStart = 0;
                        fpVValidEnd =  599;
                        fpVSyncStart = 636;
                        fpVSyncEnd = 636+6;
                        fpVTotal = 666;
                        fpPixClk = 5000;    // 50 MHz
                        fpPolarity = VSYNC_POS | HSYNC_POS;
                    }
                    else if ((refresh == 75) && (est_tim2 & EDID_ESTABLISHED_TIMING_800x600_75))
                    {
                        fpHActive =  800;
                        fpHDispEnd = 799;
                        fpHValidStart = 0;
                        fpHValidEnd = 799;
                        fpHSyncStart = 815;
                        fpHSyncEnd =  815+80;
                        fpHTotal = 1055;
                        fpVActive = 600;
                        fpVDispEnd = 599;
                        fpVValidStart = 0;
                        fpVValidEnd =  599;
                        fpVSyncStart = 600;
                        fpVSyncEnd = 600+3;
                        fpVTotal = 624;
                        fpPixClk = 4950;    // 49.5 MHz
                        fpPolarity = VSYNC_POS | HSYNC_POS;
                    }
                    // if none of the above, use 60 Hz
                    else if ((est_tim1 & EDID_ESTABLISHED_TIMING_800x600_60) 
                        || (est_tim2 & EDID_ESTABLISHED_TIMING_800x600_ANY))
                    {
                        fpHActive =  800;
                        fpHDispEnd = 799;
                        fpHValidStart = 0;
                        fpHValidEnd = 799;
                        fpHSyncStart = 839;
                        fpHSyncEnd =  839+128;
                        fpHTotal = 1055;
                        fpVActive = 600;
                        fpVDispEnd = 599;
                        fpVValidStart = 0;
                        fpVValidEnd =  599;
                        fpVSyncStart = 600;
                        fpVSyncEnd = 600+4;
                        fpVTotal = 627;
                        fpPixClk = 4000;    // 40 MHz
                        fpPolarity = VSYNC_POS | HSYNC_POS;
                    }
                    break;
                case 512:
                case 1024:
                    if ((refresh == 70) && (est_tim2 & EDID_ESTABLISHED_TIMING_1024x768_70))
                    {
                        fpHActive = 1024;
                        fpHDispEnd = 1023;
                        fpHValidStart = 0;
                        fpHValidEnd = 1023;
                        fpHSyncStart = 1047;
                        fpHSyncEnd = 1047+136;
                        fpHTotal = 1327;
                        fpVActive = 768;
                        fpVDispEnd = 767;
                        fpVValidStart = 0;
                        fpVValidEnd = 767;
                        fpVSyncStart = 770;
                        fpVSyncEnd = 770+6;
                        fpVTotal = 805;
                        fpPixClk = 7500;    // 75 MHz
                        fpPolarity = VSYNC_NEG | HSYNC_NEG;
                    }
                    else if ((refresh == 75) && (est_tim2 & EDID_ESTABLISHED_TIMING_1024x768_75))
                    {
                        fpHActive = 1024;
                        fpHDispEnd = 1023;
                        fpHValidStart = 0;
                        fpHValidEnd = 1023;
                        fpHSyncStart = 1039;
                        fpHSyncEnd = 1039+96;
                        fpHTotal = 1311;
                        fpVActive = 768;
                        fpVDispEnd = 767;
                        fpVValidStart = 0;
                        fpVValidEnd = 767;
                        fpVSyncStart = 769;
                        fpVSyncEnd = 769+3;
                        fpVTotal = 799;
                        fpPixClk = 7875;    // 78.75 MHz
                        fpPolarity = VSYNC_POS | HSYNC_POS;
                    }
                    else if (est_tim2 & EDID_ESTABLISHED_TIMING_1024x768_ANY)
                    {
                        fpHActive = 1024;
                        fpHDispEnd = 1023;
                        fpHValidStart = 0;
                        fpHValidEnd = 1023;
                        fpHSyncStart = 1047;
                        fpHSyncEnd = 1047+136;
                        fpHTotal = 1343;
                        fpVActive = 768;
                        fpVDispEnd = 767;
                        fpVValidStart = 0;
                        fpVValidEnd = 767;
                        fpVSyncStart = 770;
                        fpVSyncEnd = 770+6;
                        fpVTotal = 805;
                        fpPixClk = 6500;    // 65 MHz
                        fpPolarity = VSYNC_NEG | HSYNC_NEG;
                    }
                    break;
                case 1280:
                    if ((refresh == 75) && (est_tim2 & EDID_ESTABLISHED_TIMING_1280x1024_75))
                    {
                        fpHActive = 1280;
                        fpHDispEnd = 1279;
                        fpHValidStart = 0;
                        fpHValidEnd = 1279;
                        fpHSyncStart = 1295;
                        fpHSyncEnd = 1295+144;
                        fpHTotal = 1687;
                        fpVActive = 1024;
                        fpVDispEnd = 1023;
                        fpVValidStart = 0;
                        fpVValidEnd = 1024;
                        fpVSyncStart = 1024;
                        fpVSyncEnd = 1024+3;
                        fpVTotal = 1065;
                        fpPixClk = 13500;   // 135 MHz
                        fpPolarity = VSYNC_POS | HSYNC_POS;
                    }
                    break;
                default:
                    break;
            } //switch
        } // if not PixClk

        if (!fpPixClk) // If not found in detailed or established, try standard timings
        {
            U008 stdTiming1, stdTiming2, i;
            U016 hRes, vRes;

            for (i=0; i<EDID_V1_NUM_STANDARD_TIMINGS; i++) 
            {
                // Translate coded value into H + V resolution
                stdTiming1 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_STANDARD_TIMING_1_INDEX + i*2];
                stdTiming2 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_STANDARD_TIMING_1_INDEX + i*2 +1];
                hRes = (stdTiming1 + 31) * 8;
                switch (stdTiming2>>6)
                {
                    case 0:
                        vRes = (hRes / 16) * 10;
                        break;
                    case 1:
                        vRes = (hRes / 4) * 3;
                        break;
                    case 2:
                        vRes = (hRes / 5) * 4;
                        break;
                    case 3:
                        vRes = (hRes / 16) * 9;
                        break;
                }
                // Standard timing could also specify any refresh rate from 60-123 Hz. This only handles 60 Hz.
                if (pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth == hRes) 
                {
                    switch (hRes) 
                    {
                    case 640:
                        fpHActive =  640;
                        fpHDispEnd = 639;
                        fpHValidStart = 0;
                        fpHValidEnd = 639;
                        fpHSyncStart = 655;
                        fpHSyncEnd =  655+96;
                        fpHTotal = 799;
                        fpVActive = 480;
                        fpVDispEnd = 479;
                        fpVValidStart = 0;
                        fpVValidEnd =  479;
                        fpVSyncStart = 489;
                        fpVSyncEnd = 489+2;
                        fpVTotal = 524;
                        fpPixClk = 2517;    // 25.17 MHz
                        fpPolarity = VSYNC_NEG | HSYNC_NEG;
                        break;
                    case 800:
                        fpHActive =  800;
                        fpHDispEnd = 799;
                        fpHValidStart = 0;
                        fpHValidEnd = 799;
                        fpHSyncStart = 839;
                        fpHSyncEnd =  839+128;
                        fpHTotal = 1055;
                        fpVActive = 600;
                        fpVDispEnd = 599;
                        fpVValidStart = 0;
                        fpVValidEnd =  599;
                        fpVSyncStart = 600;
                        fpVSyncEnd = 600+4;
                        fpVTotal = 627;
                        fpPixClk = 4000;    // 40 MHz
                        fpPolarity = VSYNC_POS | HSYNC_POS;
                        break;
                    case 1024:
                        fpHActive = 1024;
                        fpHDispEnd = 1023;
                        fpHValidStart = 0;
                        fpHValidEnd = 1023;
                        fpHSyncStart = 1047;
                        fpHSyncEnd = 1047+136;
                        fpHTotal = 1343;
                        fpVActive = 768;
                        fpVDispEnd = 767;
                        fpVValidStart = 0;
                        fpVValidEnd = 767;
                        fpVSyncStart = 770;
                        fpVSyncEnd = 770+6;
                        fpVTotal = 805;
                        fpPixClk = 6500;    // 65 MHz
                        fpPolarity = VSYNC_NEG | HSYNC_NEG;
                        break;
                    case 1280:
                        if (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 960) 
                        {   // GTF
                            fpHActive = 1280;
                            fpHDispEnd = 1279;
                            fpHValidStart = 0;
                            fpHValidEnd = 1279;
                            fpHSyncStart = 1279+80;
                            fpHSyncEnd = 1279+80+136;
                            fpHTotal = 1711;
                            fpVActive = 960;
                            fpVDispEnd = 959;
                            fpVValidStart = 0;
                            fpVValidEnd = 959;
                            fpVSyncStart = 960;
                            fpVSyncEnd = 963;
                            fpVTotal = 993;
                            fpPixClk = 10200;    // 102 MHz
                            fpPolarity = VSYNC_POS | HSYNC_POS;
                        }
                        if (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 1024) 
                        {   // DMT
                            fpHActive = 1280;
                            fpHDispEnd = 1279;
                            fpHValidStart = 0;
                            fpHValidEnd = 1279;
                            fpHSyncStart = 1327;
                            fpHSyncEnd = 1327+112;
                            fpHTotal = 1687;
                            fpVActive = 1024;
                            fpVDispEnd = 1023;
                            fpVValidStart = 0;
                            fpVValidEnd = 1023;
                            fpVSyncStart = 1024;
                            fpVSyncEnd = 1024+3;
                            fpVTotal = 1065;
                            fpPixClk = 10800;    // 108 MHz
                            fpPolarity = VSYNC_POS | HSYNC_POS;
                        }
                        break;
                    case 1600:
                        fpHActive = 1600;
                        fpHDispEnd = 1599;
                        fpHValidStart = 0;
                        fpHValidEnd = 1599;
                        fpHSyncStart = 1599+104;
                        fpHSyncEnd = 1599+104+176;
                        fpHTotal = 2159;
                        fpVActive = 1024;
                        fpVDispEnd = 1023;
                        fpVValidStart = 0;
                        fpVValidEnd = 1023;
                        fpVSyncStart = 1024;
                        fpVSyncEnd = 1024+3;
                        fpVTotal = 1060;
                        fpPixClk = 16000;    // 160 MHz
                        fpPolarity = VSYNC_NEG | HSYNC_NEG;
                        break;
                    }
                }
                if (fpPixClk) // found? break for loop
                    break;
            } // for loop

        }
        if (!fpPixClk) // Did we find a detailed, established, or standard timings for the requested mode?
        {
            // No, did we find any valid timings in the EDID? Then fall back to adapter scaling.
            if (pDev->Dac.fpPixClk) 
            {
                // These values were gotten when we read the Edid. It should be the preferred mode.
                fpHActive = pDev->Dac.fpHActive;
                fpHDispEnd = pDev->Dac.fpHDispEnd;
                fpHValidStart = pDev->Dac.fpHValidStart;
                fpHValidEnd = pDev->Dac.fpHValidEnd;
                fpHSyncStart = pDev->Dac.fpHSyncStart;
                fpHSyncEnd = pDev->Dac.fpHSyncEnd;
                fpHTotal = pDev->Dac.fpHTotal - 1;
                fpVActive = pDev->Dac.fpVActive;
                fpVDispEnd = pDev->Dac.fpVDispEnd;
                fpVValidStart = pDev->Dac.fpVValidStart;
                fpVValidEnd = pDev->Dac.fpVValidEnd;
                fpVSyncStart = pDev->Dac.fpVSyncStart;
                fpVSyncEnd = pDev->Dac.fpVSyncEnd;
                fpVTotal = pDev->Dac.fpVTotal - 1;
                fpPixClk = pDev->Dac.fpPixClk;
                fpPolarity = (pDev->Dac.fpFlags); // assume 4&3=1 (digital), then 2=vsync, 1=hsync
                DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, 0, Head);   // scaled mode
            }
            else
            {
                // No EDID info: set 640x480. We need valid timing, especially the clock, so we will generate interrupts
                // and notifiers which depend on vblank interrupt will get written.
                fpHActive =  640;
                fpHDispEnd = 639;
                fpHValidStart = 0;
                fpHValidEnd = 639;
                fpHSyncStart = 655;
                fpHSyncEnd =  655+96;
                fpHTotal = 799;
                fpVActive = 480;
                fpVDispEnd = 479;
                fpVValidStart = 0;
                fpVValidEnd =  479;
                fpVSyncStart = 489;
                fpVSyncEnd = 489+2;
                fpVTotal = 524;
                fpPixClk = 2517;    // 25.17 MHz
                fpPolarity = VSYNC_NEG | HSYNC_NEG;
                return RM_ERROR;
            }
        }
    }
    else if ((GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_MONITOR))
    {
        // These values are gotten from the LUT_CURSOR_DAC class
        fpHActive = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
        fpHDispEnd = fpHActive - 1;
        fpHValidStart = 0;
        fpHValidEnd = fpHDispEnd;
        fpHSyncStart = pVidLutCurDac->HalObject.Dac[0].HorizontalSyncStart - 1;
        fpHSyncEnd = fpHSyncStart + pVidLutCurDac->HalObject.Dac[0].HorizontalSyncWidth;
        fpHTotal = pVidLutCurDac->HalObject.Dac[0].TotalWidth - 1;

        fpVActive = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;
        // check for scanline doubled modes
        if (pVidLutCurDac->HalObject.Dac[0].Format.DoubleScanMode)
            fpVActive *= 2;

        fpVDispEnd = fpVActive - 1;
        fpVValidStart = 0;
        fpVValidEnd = fpVDispEnd;
        fpVSyncStart = pVidLutCurDac->HalObject.Dac[0].VerticalSyncStart - 1;
        fpVSyncEnd = fpVSyncStart + pVidLutCurDac->HalObject.Dac[0].VerticalSyncHeight;
        fpVTotal = pVidLutCurDac->HalObject.Dac[0].TotalHeight - 1;

        fpPixClk = pVidLutCurDac->HalObject.Dac[0].PixelClock;
        // make sure we're not exceeding our max pixclk for this head
        RM_ASSERT((fpPixClk * 10000) <= GETCRTCHALINFO(pDev, Head, PCLKLimit32bpp));

        fpPolarity = ((!pVidLutCurDac->HalObject.Dac[0].Format.VerticalSyncPolarity) << 2) | 
                     ((!pVidLutCurDac->HalObject.Dac[0].Format.HorizontalSyncPolarity) << 1);
    }
    else    // FP not native mode, restore full resolution
    {
        // These values were gotten when we read the Edid.
        fpHActive = pDev->Dac.fpHActive;
        fpHDispEnd = pDev->Dac.fpHDispEnd;
        fpHValidStart = pDev->Dac.fpHValidStart;
        fpHValidEnd = pDev->Dac.fpHValidEnd;
        fpHSyncStart = pDev->Dac.fpHSyncStart;
        fpHSyncEnd = pDev->Dac.fpHSyncEnd;
        fpHTotal = pDev->Dac.fpHTotal - 1;
        fpVActive = pDev->Dac.fpVActive;
        fpVDispEnd = pDev->Dac.fpVDispEnd;
        fpVValidStart = pDev->Dac.fpVValidStart;
        fpVValidEnd = pDev->Dac.fpVValidEnd;
        fpVSyncStart = pDev->Dac.fpVSyncStart;
        fpVSyncEnd = pDev->Dac.fpVSyncEnd;
        fpVTotal = pDev->Dac.fpVTotal - 1;
        fpPixClk = pDev->Dac.fpPixClk;
        fpPolarity = (pDev->Dac.fpFlags); // assume 4&3=1 (digital), then 2=vsync, 1=hsync
    }
    // Work-around for HW bug: can't set HT lsb's to 00 in 1280 mode.
    if (fpHActive == 1280)
        if (!(fpHTotal & 0x03))
            fpHTotal += 1;  // if x...x00, make it 01
    // We are switching modes, so program the DAC with new timings.
    // If switching between scaled and centered, this isn't necessary, but it won't hurt.
    if (fpPolarity & VSYNC_POS)
    {
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, _POS, Head);
    }
    else
    {
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, _NEG, Head);
    }
    if (fpPolarity & HSYNC_POS)
    {
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, _POS, Head);
    }
    else
    {
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, _NEG, Head);
    }
    // Pixel Clock
#ifdef NV11_PCLK_HACK
    if (fpHActive == 1024)
    {
        fpPixClk = 5000;    // lower pixclk on NV11 for demo. 
    }
#endif
    dacCalcMNP(pDev, Head, fpPixClk, &M, &N, &P);
    // VPLL doesn't follow the pattern of addressing by adding a fixed offset. VPLL2 has an address in the same range (680xxx) as the
    // primary head. This makes the DAC_REG_WR32 macros useless.
    if (Head == 0) 
    {
        REG_WR32(NV_PRAMDAC_VPLL_COEFF, DRF_NUM(_PRAMDAC, _VPLL_COEFF, _MDIV, M) |
                                        DRF_NUM(_PRAMDAC, _VPLL_COEFF, _NDIV, N) |
                                        DRF_NUM(_PRAMDAC, _VPLL_COEFF, _PDIV, P));
    }
    else
    {
        // Tell DAC to use programmed M,N,P values, then give it M,N,P.
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _MSOURCE, _PROG); 
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V1SOURCE, _PROG); 
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _NVSOURCE, _PROG); 
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V2SOURCE, _PROG); 
        REG_WR32(NV_PRAMDAC_VPLL2_COEFF, DRF_NUM(_PRAMDAC, _VPLL_COEFF, _MDIV, M) |
                                        DRF_NUM(_PRAMDAC, _VPLL_COEFF, _NDIV, N) |
                                        DRF_NUM(_PRAMDAC, _VPLL_COEFF, _PDIV, P));
    }

    //
    // Set Link depending on which head we want to drive fp.
    //
    if (IsNV11(pDev))
    {
        // Some boards have only one link, and that should be connected to the second head
        if (Head == 0)
            FLD_WR_DRF_DEF(_PRAMDAC, _SEL_CLK, _IFPCLK1, _SEL_FPCLK1);
        else
            FLD_WR_DRF_DEF(_PRAMDAC, _SEL_CLK, _IFPCLK2, _SEL_FPCLK2);

        // Mobile is dual link LVDS, so both links are used
        if (pDev->Power.MobileOperation)
            if (Head == 0)
            {
                // If using Head==0, both links use primary clock
                FLD_WR_DRF_DEF(_PRAMDAC, _SEL_CLK, _IFPCLK1, _SEL_FPCLK1);
                FLD_WR_DRF_DEF(_PRAMDAC, _SEL_CLK, _IFPCLK2, _SEL_FPCLK1);
            }
            else
            {
                // If using Head==1, both links use secondary clock
                FLD_WR_DRF_DEF(_PRAMDAC, _SEL_CLK, _IFPCLK1, _SEL_FPCLK2);
                FLD_WR_DRF_DEF(_PRAMDAC, _SEL_CLK, _IFPCLK2, _SEL_FPCLK2);
            }
    }

    //
    // If an integrated TMDS exists (we can read/write TMDS_CONTROL),
    // set the PLL values there, as well.
    //
    // TMDS access is in the address range of the primary head. (680xxx)
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);     // FP on to access TMDS

    cntl32 = DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _DISABLE) | NV_PRAMDAC_INDIR_TMDS_PLL1;
    DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, cntl32, Head);

    if (DAC_REG_RD32(NV_PRAMDAC_FP_TMDS_CONTROL, Head) == cntl32) // IF TMDS present
    {
        U008 tmds_pll0, tmds_pll1, tmds_pll2;

        // make sure PLL is enabled, or we'll hang
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head); 

        if (IsNV15(pDev))
        {
            // TMDS settings

            // Run BIOS TMDS single, link A or link B table
            if ( Head == 0 )
            {
                rmStatus = DevinitProcessBip3InternalTMDS( pDev, fpPixClk, TMDSIT_TMDS_SINGLE_A );
            }
            else
            {
                rmStatus = DevinitProcessBip3InternalTMDS( pDev, fpPixClk, TMDSIT_TMDS_SINGLE_B );
            }

            if ( rmStatus != RM_OK )
            {
                BOOL isMode4 = FALSE;

                // clear error
                rmStatus = RM_OK;

                // Determine PLL values based on pixclk
                if (fpPixClk <= 6400)
                {         // pixclk <= 64MHz
                    tmds_pll0 = 0x31;
                    tmds_pll1 = 0x5F;
                }
                else if (fpPixClk == 6500)
                {  // pixclk == 65MHz
                    tmds_pll0 = 0x31;
                    tmds_pll1 = 0x49;
                }
                else if (fpPixClk <= 8000)
                {  // pixclk <= 80Mhz
                    tmds_pll0 = 0x29;
                    tmds_pll1 = 0x4E;
                }
                else if (fpPixClk <= 10000)
                {  // pixclk <= 100Mhz
                    tmds_pll0 = 0x29;
                    tmds_pll1 = 0x49;
                }
                else
                {                        // pixclk > 100MHz
                    tmds_pll0 = 0x30;
                    tmds_pll1 = 0x4F;
                    isMode4 = TRUE;
                }

                if (isMode4)
                {
                    static U032 mode4_delay = 1000000;  // delay 1 ms between programming

                    //
                    // To set TMDS_PLL0 to 0x30, we need to transition through 0x31 first
                    // (setting TMDS_PLL1 both times), with a little delay in between.
                    //
                    dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL0, 0x31);         // set PLL0
                    dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL1, tmds_pll1);    // set PLL1
                    tmrDelay(pDev, mode4_delay);
                }

                dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL0, tmds_pll0);    // set PLL0
                dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL1, tmds_pll1);    // set PLL1
            }
        }
        else
        {
            // First, check if we're in TMDS or LVDS mode
            // Currently looking at one of the registers
            // TODO: should look at the BIOS tables instead?
            DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, 
                         DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _DISABLE) | NV_PRAMDAC_INDIR_TMDS_MODE,
                         Head);
            if ((DAC_REG_RD32(NV_PRAMDAC_FP_TMDS_DATA, Head) & 0x1) || pDev->Power.MobileOperation)
            {
                // LVDS mode

                // Run BIOS LVDS dual table
                rmStatus = DevinitProcessBip3InternalTMDS( pDev, fpPixClk, TMDSIT_LVDS_DUAL );

                if ( rmStatus == RM_OK )
                {
                    // Head selection does not occur in BIOS, so we do this manually
                    // after the other stuff is done.
                    if (Head == 0)
                    {
                        U008 Data;
                        // read register NV_PRAMDAC_INDIR_TMDS_MODE in LINK B
                        Data = dacTMDSRead(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_MODE );
                        // Or with 0x0C to say get from alternate source
                        // write register back to Link B
                        dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_MODE, (U008)( Data | 0x0C ));
                    }
                    else
                    {
                        U008 Data;
                        // read register NV_PRAMDAC_INDIR_TMDS_MODE in LINK A
                        Data = dacTMDSRead(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_MODE );
                        // Or with 0x0C to say get from alternate source
                        // write register back to Link A
                        dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_MODE, (U008)( Data | 0x0C ));
                    }
                }
                else
                {
                    // We failed to process the table, so do it the hardcoded way

                    if (!IsNV11(pDev))
                    {
                        // We only know how to do NV11
                        return RM_ERROR;
                    }

                    // Clear the error
                    rmStatus = RM_OK;

                    // Setup Link0
                    dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_PLL0, 0x32);
                    dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_PLL1, 0xF7);
                    dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x3A);
                    dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);
                    dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_IDLY, 0x00);
                    if (Head == 0)
                        dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_MODE, 0x81);
                    else
                        dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_MODE, 0x8D);
                    dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_LVDS, 0x6E);

                    // Setup Link1
                    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, 0x1101111, 0x1);

                    dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_PLL0, 0x32);
                    dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_PLL1, 0xF7);
                    dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x3A);
                    dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);
                    dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_IDLY, 0x00);
                    if (Head == 0)
                        dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_MODE, 0x8D);
                    else
                        dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_MODE, 0x81);
                    dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_LVDS, 0x2E);

                    // Setup both Links to run at 1/2 the fpclk frequency
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _FPCLK_RATIO, _DB2, 0);
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _FPCLK_RATIO, _DB2, 1);
                    tmrDelay(pDev, 2000000);    // delay to let PLL's stabilize
                }
            }
            else
            {
                // TMDS settings

                // Run BIOS TMDS single, link A or link B table
                if ( Head == 0 )
                {
                    rmStatus = DevinitProcessBip3InternalTMDS( pDev, fpPixClk, TMDSIT_TMDS_SINGLE_A );
                }
                else
                {
                    rmStatus = DevinitProcessBip3InternalTMDS( pDev, fpPixClk, TMDSIT_TMDS_SINGLE_B );
                }

                if ( rmStatus != RM_OK )
                {
                    // We failed to process the table, so do it the hardcoded way

                    if (!IsNV11(pDev))
                    {
                        // We only know how to do NV11
                        return RM_ERROR;
                    }

                    // Clear the error
                    rmStatus = RM_OK;

                    // Determine PLL values based on pixclk
                    if (fpPixClk <= 6500) {         // pixclk <= 65MHz
                        tmds_pll0 = 0xB2;
                        tmds_pll1 = 0xF7;
                        tmds_pll2 = 0x00;
                    } else {                        // pixclk > 65MHz
                        tmds_pll0 = 0x32;
                        tmds_pll1 = 0x6F;
                        tmds_pll2 = 0x20;
                    }

                    dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL0, tmds_pll0);    // set PLL0
                    dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL1, tmds_pll1);    // set PLL1
                    dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL2, tmds_pll2);    // set PLL2

                    // Link0 gets data from CRTC0; Link1 gets data from CRTC1, so set "normal" for DINSEL
                    dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_MODE, 0x80);    // get data from CRTCx

                }
            }
        }
    }


    if (IsNV11(pDev))
    {
        U032    HSyncOffset;
        
        // For NV11, there is a clock synchronization problem when scaling:
        // At H display end, the CRTC clock must be held off until sync start, but if the period between end of active display
        // and sync start is too small, there is not enough time to synchronize, so we will reduce HCRTC, which is the synchronization
        // point.
        HSyncOffset = fpHSyncStart - fpHActive;    // find period between display end and sync start
        if (HSyncOffset < 28) 
        {
            fpHActive = fpHActive - (28 - HSyncOffset); // reduce HCRTC by an appropriate amount
        }
    }

    data32 = DAC_REG_RD32(NV_PRAMDAC_FP_DEBUG_0, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, (data32 & 0xfff0ffff) | 0x00020000, Head);
    
    DAC_REG_WR32(NV_PRAMDAC_FP_HCRTC, fpHActive, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_HDISPLAY_END, fpHDispEnd, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_HVALID_START, fpHValidStart, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_HVALID_END, fpHValidEnd, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_HSYNC_START, fpHSyncStart, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_HSYNC_END, fpHSyncEnd, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_HTOTAL, fpHTotal, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_VCRTC, fpVActive, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_VDISPLAY_END, fpVDispEnd, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_START, fpVValidStart, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_END, fpVValidEnd, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_VSYNC_START, fpVSyncStart, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_VSYNC_END, fpVSyncEnd, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_VTOTAL, fpVTotal, Head);
    
    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, data32, Head);

    if (commitChange)
    {
        // write this mode to the registry
        // fill in the path to the registry key--last character is the display number
       return osWriteRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplayNumber, strFpMode, FpMode);
    }

    return RM_OK;
}

// TODOMK: What is this for?  Can it be replaced with BIOS table usage?
RM_STATUS dacPowerOnMobilePanel
(
    PHWINFO pDev,
    U032    Head
)
{

    U008 lock;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"dacPowerOnMobilePanel on Head");
    DBG_PRINT_VALUE(DEBUGLEVEL_TRACEINFO,Head);
    
    // make sure PLL is enabled, or we'll hang
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);     // FP on to access TMDS

    // make sure PLL is enabled, or we'll hang
    FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE); 

	// Enable Head2 GPIO
    lock = UnlockCRTC(pDev, 1);
    CRTC_WR(NV_CIO_GPIO23_INDEX, 0x00, 1);
    RestoreLock(pDev, 1, lock);

#ifdef MACOS
	//MACOS specific panel init
	
	// Disable panel I/O
    dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x62);    // set PLL0
    dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x62);    // set PLL0

	// Enable GPIO3 to a zero, set breaker
    REG_WR32(NV_PCRTC_GPIO_EXT, 0x2200);

	// Enable panel I/O
    dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x3A);    // set PLL0
    dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x3A);    // set PLL0
#endif
   
#ifndef MACOS
	// Disable panel I/O
    dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x52);    // set PLL0
    dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x52);    // set PLL0
    
    // Wait a bit
    tmrDelay(pDev, 1000000);

    dacTMDSWrite(pDev, 0, 0xF7, 1);

    tmrDelay(pDev, 1000000);

    dacTMDSWrite(pDev, 0, 0x94, 1);

    // Wait a bit
    tmrDelay(pDev, 100000);

    // Let DivBy7 go again -- this leaves us in LVDS
    dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);    // set PLL1
    dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);    // set PLL1

    // Wait a moderate bit
    tmrDelay(pDev, 2000000);
#endif

    //
    // Power on the BL so we can see
    //
    REG_WR32(NV_PCRTC_GPIO_EXT, 0x2211);

	return (RM_OK);
}

RM_STATUS dacPowerOffMobilePanel
(
    PHWINFO pDev,
    U032    Head
)
{
    
    U008 lock;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"dacPowerOffMobilePanel on Head");
    DBG_PRINT_VALUE(DEBUGLEVEL_TRACEINFO,Head);
    
	// Enable Head2 GPIO
    lock = UnlockCRTC(pDev, 1);
    CRTC_WR(NV_CIO_GPIO23_INDEX, 0x00, 1);
    RestoreLock(pDev, 1, lock);

#ifndef MACOS
	// make sure PLL is enabled, or we'll hang
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, 0);     // FP on to access TMDS
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, 1);     // FP on to access TMDS

    // BL disable
	REG_WR32(NV_PCRTC_GPIO_EXT, 0x2210);

	// Disable panel I/O
    dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x52);    // set PLL0
    dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x52);    // set PLL0

    tmrDelay(pDev, 1000000);
#endif

	REG_WR32(NV_PCRTC_GPIO_EXT, 0x2200);

	return (RM_OK);
}

void dacZeroPWMController(PHWINFO pDev)
{
	REG_WR32(NV_PBUS_PWM,0);
}

void dacSetPWMController(PHWINFO pDev)
{
	REG_WR32(NV_PBUS_PWM,pDev->Dac.fpPWMRegisterContents);
}

void dacGetPWMController(PHWINFO pDev)
{
	U032 NvPbusPwmHighValue,ControllerRegisterValue,FpDynamicRange;
    U032 NvPbusPwmPeriodValue;
    U032 MinPeriodValue,RelativePeriod;
	
	//we need to init the variable for the flat panel brightness
    if(!pDev->Dac.fpRelativeBrightness)
    {
    	ControllerRegisterValue=REG_RD32(NV_PBUS_PWM);
    	NvPbusPwmHighValue=(ControllerRegisterValue>>16)&0x7fff;
    	NvPbusPwmPeriodValue=ControllerRegisterValue&0x7fff;
    	
    	//a kludge - 40% to 100%
    	MinPeriodValue=((NvPbusPwmPeriodValue*40)+50)/100;
    	
    	FpDynamicRange=NvPbusPwmPeriodValue-MinPeriodValue;
    	
    	RelativePeriod=NvPbusPwmHighValue-MinPeriodValue;
    	
    	pDev->Dac.fpRelativeBrightness=(((RelativePeriod*10000)/(FpDynamicRange*10000/255))*10+5)/10;
    	
    	//store this raw PWM value also into the DAC.fpPWMRegisterContents save/restore var.
    	pDev->Dac.fpPWMRegisterContents=ControllerRegisterValue;
    }
}

RM_STATUS  dacSetFlatPanelBrightness(PHWINFO pDev, U032 Head, U032 Brightness, U032 Frequency, U032 MaxPercent, U032 MinPercent,BOOL ControllerIsOn)
{

	
	U032 TicksNeededForClock;
	U032 TicksNeededForHigh;
	
	U032 ControlValue=0;
	
	//clip if necessary
	if(Brightness>255)
	{
		Brightness=255;
	}
	
	//enable PWM output
	if(ControllerIsOn)
	{
		FLD_WR_DRF_DEF(_PBUS,_PWM,_EN,_ENABLED);
	}
	else
	{
		FLD_WR_DRF_DEF(_PBUS,_PWM,_EN,_DISABLED);
	}
	
	TicksNeededForClock=GetTicksForFrequency(Frequency);
	TicksNeededForHigh=GetRequiredTickCount(Frequency,MaxPercent,MinPercent,Brightness);
	
	//program the chip
	FLD_WR_DRF_NUM(_PBUS,_PWM,_HI,TicksNeededForHigh);
	FLD_WR_DRF_NUM(_PBUS,_PWM,_PERIOD,TicksNeededForClock);
	
	//store multiplier for get
	pDev->Dac.fpRelativeBrightness=Brightness;
	
	//store the raw PWM value in case someone triggers an on without an off
	pDev->Dac.fpPWMRegisterContents = REG_RD32(NV_PBUS_PWM);
			
	return RM_OK;
}

RM_STATUS  dacGetFlatPanelBrightness(PHWINFO pDev, U032 Head, U032 * Brightness,BOOL * ControllerIsOn)
{
	U032 NvPbusPwmEnable;
	
	//read register
	NvPbusPwmEnable=REG_RD_DRF(_PBUS,_PWM,_EN);
	
	//retrieve last brightness setting
	*Brightness=pDev->Dac.fpRelativeBrightness;
	
	//check the status of the control bit
	if(NvPbusPwmEnable)
	{
		*ControllerIsOn=TRUE;
	}
	else
	{
		*ControllerIsOn=FALSE;
	}
	
	return RM_OK;
}

U032 GetAbsoluteDifference(U032 value_a, U032 value_b)
{
	if(value_a>value_b)
	{
		return value_a-value_b;
	}
	else
	{
		return value_b-value_a;
	}
}

U032 GetTicksForFrequency(U032 Frequency)
{
	//this replaced alot of code - overcomplication on my part
	return 33333333/Frequency;
}

U032 GetPortionOfTotalSizeBasedOnPercentage(U032 TotalSize,U032 Percentage)
{

	//do calculation, round up
	return (((TotalSize*Percentage)+50)/100);
}

U032 DetermineFrequencyFromTickCount(U032 NumTicks)
{
	return ((333333330/NumTicks)+5)/10;
}

U032 GetRequiredTickCount(U032 Frequency,U032 MaxPercent,U032 MinPercent,U032 BaseMultiplier)
{
	U032 TicksInClockPeriod;
	U032 MaxTicks;
	U032 MinTicks;
	U032 DynamicRange;
	U032 PreliminaryTickCount;

	TicksInClockPeriod=GetTicksForFrequency(Frequency);
	MaxTicks=GetPortionOfTotalSizeBasedOnPercentage(TicksInClockPeriod,MaxPercent);
	MinTicks=GetPortionOfTotalSizeBasedOnPercentage(TicksInClockPeriod,MinPercent);

	DynamicRange=MaxTicks-MinTicks;

	PreliminaryTickCount=MinTicks+(((((DynamicRange*10)/255)+5)/10)*BaseMultiplier);
	
	if (PreliminaryTickCount>MaxTicks)
	{
		//if rounding produces a number too large, clip it
		PreliminaryTickCount=MaxTicks;
	}
	
	return PreliminaryTickCount;
}

BOOL dacIsFlatPanelOn(PHWINFO pDev,U032 Head)
{
    U008 lock;
    
	// Enable Head2 GPIO
    lock = UnlockCRTC(pDev, 1);
    CRTC_WR(NV_CIO_GPIO23_INDEX, 0x00, 1);
    RestoreLock(pDev, 1, lock);

    if((BIT(4)|BIT(6))&REG_RD32(NV_PCRTC_GPIO_EXT))
    {
        return TRUE;
    }
    
    return FALSE;
}

/*
//we are going to memorize the setting that we spec'd before, because we cannot reliably reproduce the same setting as
//was spec'd for. The specing is more like a hash than an exact formula. And hashes cannot be reversed easily....
U032 DetermineMultiplierForCurrentPeriod(PHWINFO pDev, U032 ClockTicksTotal,U032 DutyCycle)
{
	U032 DynamicRange,MaxTicks,MinTicks,PreliminaryMultiplier;

	MaxTicks=GetPortionOfTotalSizeBasedOnPercentage(ClockTicksTotal,pDev->Dac.fpMaxPercentDutyCycle);
	MinTicks=GetPortionOfTotalSizeBasedOnPercentage(ClockTicksTotal,pDev->Dac.fpMinPercentDutyCycle);
	DynamicRange=MaxTicks-MinTicks;
	
	PreliminaryMultiplier=((((DutyCycle-MinTicks)*10000)/(DynamicRange*10000/255))*10+5)/10;
	
	if(PreliminaryMultiplier>255)
	{
		PreliminaryMultiplier=255;
	}
	
		if(DutyCycle==MaxTicks)
		{
			PreliminaryMultiplier=255;
		}
		
		if(DutyCycle==MinTicks)
		{
			PreliminaryMultiplier=0;
		}
	
	
	return PreliminaryMultiplier;
}
*/
