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

// turning on the clocks requires a delay that is easily forgotten,
//   so try to do it all in one place
RM_STATUS dacWriteLinkPLL(
    PHWINFO pDev,
    U032    Head,
    U032    which)
{
    DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, which, Head);     // FP on to access TMDS
    tmrDelay(pDev, 0x400000);
    
    return RM_OK;
}


RM_STATUS dacAdjustCRTCForFlatPanel
(
    PHWINFO pDev, 
    U032 Head,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac
)
{
    U032    Ht, Hre, Vt, Vrs, Vre, Vbs, data32;
    U016    Seq, Vendor;
    U008    lock, Cr03, Cr04, Cr05, Cr07, Cr09, Cr11, Cr25, Cr28, Cr2d, Cr33;
#if 0
    U032 cntl32;
#endif

    dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);   // power on flat panel

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
        {
            dacWriteLinkPLL(pDev, 0, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_TMDS);
        }
    }
#endif
    // Toshiba SBIOS behaves badly: it may lock ext CRTC's on us during SMI.
    lock = UnlockCRTC(pDev, 0);
    if (lock != NV_CIO_SR_UNLOCK_RW_VALUE) 
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ext CRTC's are locked in dacAdjustCRTCForTV\r\n");
    }
    
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
            if (Vendor == 0xf84c)  //SGI
            {
                Cr04 = (U008)Ht; // All modes
                if ( ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 640) &&
                      (pDev->Framebuffer.HalInfo.VertDisplayWidth  == 480))
                     ||
                     ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 800) &&
                      (pDev->Framebuffer.HalInfo.VertDisplayWidth  == 600))
                   )
                {   
                    Cr04 = (U008)Ht+1;
                }
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

    // Also, ensure there's enough bandwidth for video.
    // 
    // With big panels in a scaled low res mode (e.g. 16x12 FP in 8x6), the
    // CRTC slaved off the FP can stall for 1/2 the time, which seems to cause
    // video underflow. To fix this, we lengthen HTOTAL to allow more clocks
    // (see bugid 20010130-110932).
    //
    if (pDev->Power.MobileOperation)
    {
        U032 Hrs, fpHSync, fpHTotal;
 
        // read the horizontal extra bits
        CRTC_RD(NV_CIO_CRE_HEB__INDEX, Cr2d, Head);

        // get the CRTC hsync start
        CRTC_RD(NV_CIO_CR_HRS_INDEX, Hrs, Head);
        if (Cr2d & BIT(3))
            Hrs |= BIT(8);    // add bit8

        // get the FP hsync values
        fpHTotal = DAC_REG_RD32(NV_PRAMDAC_FP_HTOTAL, Head);
        fpHSync = DAC_REG_RD32(NV_PRAMDAC_FP_HCRTC, Head);

        // set the CRTC HTOTAL closer to the FP horiz timings (take off 8
        // chars for the delay between FP_HCRTC and when the CRTC sees it).
        Ht  = (fpHTotal - fpHSync) / 8 + Hrs - 6;
        Ht -= 8;

        // set CR0 and the bit8 overflow, if necessary
        CRTC_WR(NV_CIO_CR_HDT_INDEX, Ht, Head);
        if (Ht & BIT(8))
        {
            Cr2d |= BIT(0);
            CRTC_WR(NV_CIO_CRE_HEB__INDEX, Cr2d, Head);
        }
    }

    // Toshiba SBIOS behaves badly: it may lock ext CRTC's on us during SMI.
    lock = UnlockCRTC(pDev, 0);
    if (lock != NV_CIO_SR_UNLOCK_RW_VALUE) 
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ext CRTC's are locked in dacAdjustCRTCForTV\r\n");
    }
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


RM_STATUS dacSetFlatPanelScaling
(
    PHWINFO pDev,
    U032    Head,
    U032    FpMode,
    BOOL    CommitChange
)
{
    RM_STATUS rmStatus = RM_OK;
    
    //store the fpMode, regardless
    pDev->Dac.HalInfo.CrtcInfo[Head].fpMode = FpMode;
    
    switch (FpMode)
    {
        case NV_CFGEX_GET_FLATPANEL_INFO_SCALED:
        case NV_CFGEX_GET_FLATPANEL_INFO_CENTERED:
        case NV_CFGEX_GET_FLATPANEL_INFO_NATIVE:
        {
            //
            // TEMPORARY WORKAROUND
            // 
            // The current control panel will lose track of what head it is really talking
            // to, so let's make sure this is the flat panel head.
            //
            if (pDev->Power.MobileOperation)
            {
                // Just hit them both in mobile as there is only one dfp
                DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, FpMode, 0);
                DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, FpMode, 1);
            }
            else
            {
                //
                // Change the DFP scaling mode.  For multisync panels, turn of fpclk
                // then delay after the change is being made, in order to ensure the correct
                // setup conditions.  Some multisync panels lose their sync otherwise.
                //

                    if (pDev->Dac.fpNative)
                {
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _BOTH, Head);
                    DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, FpMode, Head);
                    osDelay(10);
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);
                }
                else
                {
                    DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, FpMode, Head);
                }    

                //
                // WORKAROUND CONTROL PANEL BUG   (REMOVE REMOVE REMOVE when possible)
                //
                // The current control panel still asks us to set the wrong head.  We need
                // to fix the panel code, but until then, hit both heads.  This code needs 
                // to be removed when the real fix is made, as it will affect operation
                // of dual DVI systems
                //
                DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE, FpMode, Head ^ 1);
            }
            break;
        }

        case NV_CFGEX_GET_FLATPANEL_INFO_SCALED_8BIT:
        {
            // only implemented on headB
            if ( ! IsNV11(pDev))
                goto failed;
            // require B1 or later
            if ( ! (pDev->halHwInfo.pMcHalInfo->MaskRevision >= MC_MASK_REVISION_B1))
                goto failed;
            if (Head != 1)
                goto failed;
            if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_FLAT_PANEL)
                goto failed;

            // turn on scaling mode
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _SCALE, Head);

            // disable dithering
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _6B_DITHER, _OFF, Head);
            
            // finally, turn on the scaling
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _YINTERP, _TRUNCATE, Head);
            break;
        }

        case NV_CFGEX_GET_FLATPANEL_INFO_CENTERED_DITHER:
        {
            U032 mode;
            
            // this only supported on NV11 B1 or better.
            if ( ! IsNV11(pDev))
                goto failed;
            // require B1 or later
            if ( ! (pDev->halHwInfo.pMcHalInfo->MaskRevision >= MC_MASK_REVISION_B1))
                goto failed;
            if (Head != 1)
                goto failed;
            if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_FLAT_PANEL)
                goto failed;

            mode = DAC_REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _MODE, Head);
            if ((mode != NV_CFGEX_GET_FLATPANEL_INFO_CENTERED) && 
                (mode != NV_CFGEX_GET_FLATPANEL_INFO_NATIVE))
                goto failed;
                
            // turn on centered mode
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _CENTER, Head);
            
            //reset temporal dithering safety counter
            pDev->Dac.CrtcInfo[Head].VBlankTemporalDitherSafetyCounter=0;
            
            // dithering will be enabled in VBlankUpdateDither()

            break;
        }

        case NV_CFGEX_GET_FLATPANEL_INFO_SCALED_ASPECT_CORRECT:
        {
            U032 scaleX, scaleY;
            U032 scaleX1000, scaleY1000;
            U032 visible, unused;
            PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
            
            // this only supported on NV11 and newer (not NV15)
            if ( ! IsNV11orBetter(pDev))
                goto failed;
            if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_FLAT_PANEL)
                goto failed;

            pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
            if ( ! pVidLutCurDac)
                goto failed;
                
            // turn on scaling mode
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _SCALE, Head);

            // disable dithering
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _6B_DITHER, _OFF, Head);

            // disable y truncate mode
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _YINTERP, _BILINEAR, Head);

            // now do the work...
            scaleX = DAC_REG_RD_DRF(_PRAMDAC, _FP_DEBUG_3, _XSTEPSIZE, Head);
            scaleY = DAC_REG_RD_DRF(_PRAMDAC, _FP_DEBUG_3, _YSTEPSIZE, Head);
                
            if (scaleY > scaleX)
            {
                // lock the scaling as a forced value
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_1, _XSCALE_TESTMODE, _ENABLE, Head);
                // just use 12 bits of scale
                scaleY &= 0xFFF;
                DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_DEBUG_1, _XSCALE_VALUE, scaleY, Head);
                
                // now center it in X by setting HVALID start & end
                scaleX1000 = (pDev->Dac.HalInfo.fpVMax * 1000) / pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;
                visible = (pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth * scaleX1000) / 1000;
                unused = pDev->Dac.HalInfo.fpHMax - visible;
                
                // stock away values in the pDev
                pDev->Dac.fpHValidStart=unused / 2;
                pDev->Dac.fpHValidEnd=visible + (unused / 2);
                
                //do the hardware set
                DAC_REG_WR32(NV_PRAMDAC_FP_HVALID_START, unused / 2, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HVALID_END, visible + (unused / 2), Head);
            }
            else
            {
                // lock the scaling as a force value
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_1, _YSCALE_TESTMODE, _ENABLE, Head);
                // just use 12 bits of scale
                scaleX &= 0xFFF;
                DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_DEBUG_1, _YSCALE_VALUE, scaleX, Head);
                
                // now center it in Y by setting VVALID start & end
                scaleY1000 = (pDev->Dac.HalInfo.fpHMax * 1000) / pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
                visible = (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight * scaleY1000) / 1000;
                unused = pDev->Dac.HalInfo.fpVMax - visible;
                
                //store these away in the pDev
                pDev->Dac.fpVValidStart=unused / 2;
                pDev->Dac.fpVValidEnd=visible + (unused / 2);
                
                //do the hardware set
                DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_START, unused / 2, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_END, visible + (unused / 2), Head);
            }

            break;
        }
        
        default:
             return RM_ERROR;
    }
        
    //
    // Do we want to save this setting?
    //
    if (CommitChange)
    {
        // write this mode to the registry
        // fill in the path to the registry key--last character is the display number
       rmStatus = osWriteRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplayNumber, strFpMode, FpMode);
    }

    return rmStatus;
 
failed:
    return RM_ERROR;
}


//Call dacSetFlatPanelScaling, and also commit the change to the system CMOS if
// the system supports that.  Can't be called while at a raised IRQL (e.g.,
// during modesets) because under NT we can't generate an int10h then.
RM_STATUS dacSetFlatPanelScalingCMOS
(
    PHWINFO pDev,
    U032    Head,
    U032    FpMode,
    BOOL    CommitChange
)
{

    RM_STATUS status;
    status = dacSetFlatPanelScaling(pDev, Head, FpMode, CommitChange);

#if !defined(MACOS)  
    if ((pDev->Power.MobileOperation) && (status == RM_OK)) {
        // Call through VGA BIOS to system BIOS to set center/scaling mode CMOS bit.
        // We do this here because under NT we can't make an int10h call during the modeset.
        // (while operating at a raised IRQ level).
//        RM_STATUS status;
        U032 eax, ebx, ecx, edx;
//        U032 FpMode = dacGetFlatPanelConfig(pDev, Head);

        eax = 0x4F14;       // VESA EDID function
//        ebx = (CommitChange?0x8000:0) | 0x0102;       // set center/scale mode
//        ecx = (FpMode == NV_CFGEX_GET_FLATPANEL_INFO_SCALED)?1:0;  // mode
        // XXX; may need to track other NV11 SCALED modes here...
        if (pDev->Power.MobileOperation == 2) {
            ebx = 0x8102;       // set center/scale mode
            ecx = (FpMode == NV_CFGEX_GET_FLATPANEL_INFO_SCALED)?1:0;  // mode
        } else {
            ebx = 0x0102;       // set center/scale mode
            ecx = (FpMode == NV_CFGEX_GET_FLATPANEL_INFO_SCALED)?0:1;  // mode
        }
        edx = 0;

        status = osCallVideoBIOS(pDev, &eax, &ebx, &ecx, &edx, NULL);
//        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: result from updating CMOS:", eax);
    }
#endif

    return status;
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
U032    fpPixClk, fpPolarity=0, M, N, P, pixels, refresh, dt_refresh;
BOOL    ModeValid, RefreshValid = FALSE;
U016    Vendor = 0x0;
U032    spreadEnabled;

    RM_STATUS rmStatus = RM_OK;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"In:dacSetFlatPanelMode\n");
    
    // check to make sure it's not null.  
    // This fixes bug 32021 "BSOD when switching between scaling/centering"
    if(pVidLutCurDac == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,
                         "NVRM: ERROR!!! dacSetFlatPanelMode() is called with pVidLutCurDac==NULL!\n");
        return RM_ERROR;
    }
    // check to make sure this is a DFP as well!
    // This fixes bug 32060: "LCD blooms when switching btw scaling/centering on LCD/TV extended"
    // There is a case on non-mobile platforms where Monitor is hooked up to Head 1
    // and the resman needs to setup as if it was a flat panel.
    if( (pVidLutCurDac->DisplayType != DISPLAY_TYPE_FLAT_PANEL) &&
        !( IsNV11(pDev) && (pVidLutCurDac->DisplayType == DISPLAY_TYPE_MONITOR) && 
            (Head==1) && !pDev->Power.MobileOperation) )
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,
                         "NVRM: ERROR!!! dacSetFlatPanelMode(): pVidLutCurDac != Flat Panel!\n");
        return RM_ERROR;
    }

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


    // *********************************************************
    // Power On Flat Panel TMDS and FPCLK
    // Later in dacAdjustCRTCForFlatPanel() we will power
    // down the internal TMDS if we are using an external
    // one.  -paul
    // *********************************************************

    dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);

    //stall now built into the action of turning the TMDS unit and FPCLK on
   

    // Set the scale factor in FP_DEBUG back to 0 in case the BIOS used this to get a special
    // mode, i.e. Japanese DOS. This is only done on NV10. 
    // In Windows modes, the scaling is always set by HW comparing the values in the FP timing
    // registers and the CRTC registers, so the DEBUG registers are always set to 0. 
    // For DOS 475 lines, this method doesn't work, because the scaling value is not in the HW 
    // tables on NV10, so the BIOS has to work around this by setting these DEBUG registers.
    
    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_1, 0, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_2, 0, Head);
#ifdef IKOS
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

    {
        //Special case for for SGI flat panel.  
        if (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VERSION_NUMBER_INDEX] == 1)
        {
            Vendor = (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VENDOR_ID_INDEX+1])<<8 | pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VENDOR_ID_INDEX];
            if (Vendor == 0xf84c)
            {

                if (pDev->Dac.fpNative)
                {
                     if ( 
                         pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 960
                         && FpMode ==  NV_CFGEX_GET_FLATPANEL_INFO_SCALED
                         && !IsNV11orBetter(pDev)
                         ) 
                     {
                         FpMode = NV_CFGEX_GET_FLATPANEL_INFO_NATIVE;
                     }
                }
                else
                {
                     if ( 
                         pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 960
                         && FpMode ==  NV_CFGEX_GET_FLATPANEL_INFO_SCALED
                         && !IsNV11orBetter(pDev)
                         ) 
                     {
                         FpMode = NV_CFGEX_GET_FLATPANEL_INFO_CENTERED;
                     }
                }
            }
        }
    }
    //
    // Set panel scaling mode
    //
    dacSetFlatPanelScaling(pDev, Head, FpMode, commitChange);

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
                
                    // Got a matching detailed timing, but is it the requested refresh rate?
                    // If not, we have a valid resolution we can use if we don't find right mode in established timing
                    pixels = fpHTotal * fpVTotal;
                    dt_refresh = (fpPixClk*10000) / pixels;
                    if (dt_refresh == refresh) 
                    {
                        RefreshValid = TRUE;
                    }
                }                       
            } // if parseDetailedTimings
        }
#if IKOS // no EDID
        RefreshValid = FALSE;
#endif
        if (!RefreshValid)  // not found in detailed, check established
        {
            est_tim1 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_ESTABLISHED_TIMING_1_INDEX];
            est_tim2 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_ESTABLISHED_TIMING_2_INDEX];
            fpPolarity = 0x3;   // estab timings use positive polarities on syncs

#if IKOS // give them whatever they ask for
            est_tim1 = 0xff;
            est_tim2 = 0xff;
#endif

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
                            RefreshValid = TRUE;
                        }
                        else if ((refresh == 75) && (est_tim1 & EDID_ESTABLISHED_TIMING_640x480_75))
                        {
                            fpHActive =  640;
                            fpHDispEnd = 639;
                            fpHValidStart = 0;
                            fpHValidEnd = 639;
                            fpHSyncStart = 655;
                            fpHSyncEnd =  719;
                            fpHTotal = 839;
                            fpVActive = 480;
                            fpVDispEnd = 479;
                            fpVValidStart = 0;
                            fpVValidEnd =  479;
                            fpVSyncStart = 480;
                            fpVSyncEnd = 483;
                            fpVTotal = 499;
                            fpPixClk = 3150;    // 31.5 MHz
                            fpPolarity = VSYNC_NEG | HSYNC_NEG;
                            RefreshValid = TRUE;
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
                            RefreshValid = TRUE;    // this may not be correct, but we don't check refresh rate in standard timing,
                        }                           // so there is no point in looking further
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
                        RefreshValid = TRUE;
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
                        RefreshValid = TRUE;
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
                        RefreshValid = TRUE; // this may not be correct, but we don't check refresh rate in standard timing
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
                        RefreshValid = TRUE;
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
                        RefreshValid = TRUE;
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
                        RefreshValid = TRUE; // this may not be correct, but we don't check refresh rate in standard timing
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
                        RefreshValid = TRUE;
                    }
                    break;
                default:
                    break;
            } //switch
        } // if not PixClk

        if (!RefreshValid) // If not found in detailed or established, try standard timings
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
                
                // Use scaling to get requested mode
                // NV10 & NV15 will not scale above 1024, so fall back to centered
                if ((IsNV10(pDev) || IsNV15(pDev)) && pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth > 1024) // line > 1024?
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _CENTER, Head);
                else
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _SCALE, Head);   // scaled mode
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
        fpHActive = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
        fpVActive = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;

        // Use the FP scaler in the low resolution modes
        if (fpHActive <= 512 && fpVActive <= 384)
        {
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _SCALE, Head);
            // Use the line/clock doubled equivalent (or the nearest resolution)
            // I only have 60 Hz refresh rate.
            switch (fpHActive) 
            {
            case 320:
                fpHActive =  640;
                fpHDispEnd = 639;
                fpHValidStart = 0;
                fpHValidEnd = 639;
                fpHSyncStart = 655;
                fpHSyncEnd =  655+96;
                fpHTotal = 799;
                fpVActive = 480;    // We could add 400 line timing if 320x200 doesn't scale into 640x480 well
                fpVDispEnd = 479;
                fpVValidStart = 0;
                fpVValidEnd =  479;
                fpVSyncStart = 489;
                fpVSyncEnd = 489+2;
                fpVTotal = 524;
                fpPixClk = 2517;    // 25.17 MHz
                fpPolarity = VSYNC_NEG | HSYNC_NEG;
                break;
            case 400:
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
            case 480:   // We could put 960x720 timing in here, but it should scale up into 512 (1024)
            case 512:
            default:    
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
            }
        }
        else
        {
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _NATIVE, Head);

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
        
        // NV10 & NV15 will not scale above 1024, so fall back to centered
        // except in the case of panels which have a max width of 1600, where we
        // can use scaling intended for other modes.
        if (IsNV10(pDev) || IsNV15(pDev)) 
        {
           if (pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth > 1024      &&
               !((pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth == 1280 || pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth == 1600) && 
                 (pDev->Dac.HalInfo.fpHMax == 1600))
              )
           {
               DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _CENTER, Head);
           }
        }
    }

    // For mobile, see if dithering is needed on the flat panel
    if (pDev->Power.MobileOperation)
    {
        // If this is NV11 B1 or better, on Head 1
        if (IsNV11(pDev) &&
            (pDev->halHwInfo.pMcHalInfo->MaskRevision >= MC_MASK_REVISION_B1) &&
            (Head == 1))
        {
            // 
            // And, we're in a non-scaled (centered/native) FP mode or scaled,
            // but in the native resolution of the FP, and in 32bpp
            //
            if (((FpMode != NV_CFGEX_GET_FLATPANEL_INFO_SCALED) ||
                    ((fpHActive == pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth) &&
                     (fpVActive == pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight))) &&
                    (pVidLutCurDac->HalObject.Dac[0].PixelDepth == 32))
            {
                // enable dithering (kicked off in VBlankUpdateDither)
                pDev->Dac.HalInfo.CrtcInfo[Head].fpMode = NV_CFGEX_GET_FLATPANEL_INFO_CENTERED_DITHER;

                // reset temporal dithering safety counter
                pDev->Dac.CrtcInfo[Head].VBlankTemporalDitherSafetyCounter = 0;
            }
            else 
            {
                // disable dithering
                pDev->Dac.HalInfo.CrtcInfo[Head].fpMode = 0;
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _6B_DITHER, _OFF, Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _DITHER_RB, _NORMAL, Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _DITHER_G, _NORMAL,  Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _DITHER_Y, _NORMAL,  Head);
            }
        }
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

    //
    // NV11 workaround for mobile panels when running low-res modes on hi-res panels.  
    // The panel scaler seems to be causing issues with the LVDS data.
    //
    // Drop the PCLK by 20MHz when running in these configurations.  Only deal with
    // 800 width modes and less on panels greater than 1400 native horizontal and
    // the scaler is enabled.
    //
    // Only happens on HSYNC mode panels (Toshiba does not use these!)
    if ( IsNV11(pDev) && pDev->Power.MobileOperation && (pDev->Power.MobileOperation != 2) )
    {
        if (FpMode == NV_CFGEX_GET_FLATPANEL_INFO_SCALED)
        {
            if ((pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth <= 800) && (fpHActive >= 1400))
            {
                fpPixClk -= 2000;  // drop it by 20MHz (10MHz per LVDS channel)

                // Turn on the flag that says we're dropping the PCLK
                pDev->Dac.DevicesEnabled |= (DAC_LOW_PCLK_ENABLED);
            }
            else
            {
                // Turn off the flag that says we're dropping the PCLK
                pDev->Dac.DevicesEnabled &= ~(DAC_LOW_PCLK_ENABLED);
            }
        }
    }

    spreadEnabled = REG_RD_DRF(_PRAMDAC, _SEL_CLK, _SPREAD_SPECTRUM_VPLL1) || REG_RD_DRF(_PRAMDAC, _SEL_CLK, _SPREAD_SPECTRUM_VPLL2);
    if (pDev->Power.MobileOperation && spreadEnabled)
    {
        //
        // Calculate taking spread into account, so cap M at 6
        //
        // This is currently only being done for mobile systems that use
        // spread spectrum of the fpclk.
        //
        dacCalcMNP_MaxM(pDev, Head, fpPixClk, &M, &N, &P, 6);
    }
    else if (IsNV11(pDev) && (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_MONITOR) && (Head == 1) && pDev->Dac.ExtDacMaxM)
    {
        //
        // Calculate taking external dac noise into account, so cap M at the new value (received from registry)
        //
        dacCalcMNP_MaxM(pDev, Head, fpPixClk, &M, &N, &P, pDev->Dac.ExtDacMaxM);
    }
    else
        dacCalcMNP(pDev, Head, fpPixClk, &M, &N, &P);


    // VPLL doesn't follow the pattern of addressing by adding a fixed offset. 
    // VPLL2 has an address in the same range (680xxx) as the
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
        {
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
    }

    //
    // If an integrated TMDS exists (we can read/write TMDS_CONTROL),
    // set the PLL values there, as well.
    //
    // TMDS access is in the address range of the primary head. (680xxx)
    dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);

    cntl32 = DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _DISABLE) | NV_PRAMDAC_INDIR_TMDS_PLL1;
    DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, cntl32, Head);

    if (IsNV11(pDev) || DAC_REG_RD32(NV_PRAMDAC_FP_TMDS_CONTROL, Head) == cntl32) // IF TMDS present
    {
        U008 tmds_pll0, tmds_pll1, tmds_pll2;

        // make sure PLL is enabled, or we'll hang
        dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE); 

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
                    //
                    // To set TMDS_PLL0 to 0x30, we need to transition through 0x31 first
                    // (setting TMDS_PLL1 both times), with a little delay in between.
                    //
                    dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL0, 0x31);         // set PLL0
                    dacTMDSWrite(pDev, Head, NV_PRAMDAC_INDIR_TMDS_PLL1, tmds_pll1);    // set PLL1
                    tmrDelay(pDev, 1 * 1000 * 1000);  // 1 ms
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
            //DAC_REG_WR32(NV_PRAMDAC_FP_TMDS_CONTROL, 
            //             DRF_DEF(_PRAMDAC, _FP_TMDS_CONTROL, _WRITE, _DISABLE) | NV_PRAMDAC_INDIR_TMDS_MODE,
            //             Head);
            //if ((DAC_REG_RD32(NV_PRAMDAC_FP_TMDS_DATA, Head) & 0x1) || pDev->Power.MobileOperation)
            //
            // From what I've found out, there are no LVDS panels that are not Mobile.
            if (pDev->Power.MobileOperation)
            {
                //
                // OK, we're in LVDS mode.
                //
                // Ideally we use the scripts embedded within the BIOS to perform the panel programming.
                // Let's see if the support is in the BIOS for this particular panel.
                //
                //
                // Note the Mac will use the legacy programming until its fcode is updated accordingly
                //

                // script programming only needs to be run if
                // A. We've changed the PCLK
                // B. Or we're powered off.
                // For mobile panels (which are currenly LVDS only), we never change the PCLK on head 1
                // So, only if we're in an off state, should we run the scripts
                // check if the panel is off
                //if((REG_RD32(NV_PCRTC_GPIO_EXT) & 0x33) == 0x00)
                // The old way was to use the GPIO pins to determine off/on status
                // The new way uses the SW variable, pDev->Dac.DevicesEnabled
                if((pDev->Dac.DevicesEnabled & DAC_PANEL_POWER_ENABLED) != DAC_PANEL_POWER_ENABLED)
                {
#ifndef MACOS
                    U008 panelFlags = 0;
                    U032 timeHI, timeLO;
    
                    // Try to run manufacturers script (i.e. BIOS revision >= 0x14)
                    if(DevinitGetManufacturerInfo(pDev) == RM_OK)
                    {
                      // Test if we need to reprogram the links
                      if(pDev->Dac.ManufacturerProperties.Flags & MANUFACTURER_RESET_REQUIRED)
                      {
                        rmStatus = DevinitRunManufacturerScript(pDev, MSCRIPT_LVDS_RESET_SCRIPT);
                      }
                    }
                    else // BIOS is < revision 0x14
                    {
                        rmStatus = DevinitGetFlatPanelFlags( pDev, pDev->Dac.PanelStrap, &panelFlags );
    
                        // We need to first power down if we're not already powered down.
                        // dacPowerOffMobilePanel(pDev, Head);
                        // We should never have to power down the panel since we can't get here unless we're powered down!
                        
                        
                        //  clear the screen: This is needed since the Old Non-Toshiba Mobile VBIOSes
                        // will turn on the screen with the Uber-script.  So we should make sure that
                        // the screen is clear before we turn on the panel.
                        if(pDev->Power.MobileOperation != 2)
                        {
                            U032 page, size, i;  // for clearing the screen

                            page = pDev->Framebuffer.HalInfo.Start[pDev->Framebuffer.Current] / 4;
                            size = (pDev->Dac.HalInfo.Depth / 8) * pDev->Framebuffer.HalInfo.HorizDisplayWidth * pDev->Framebuffer.HalInfo.VertDisplayWidth / 4;
                            for (i = 0; i < size; i++)
                            {
                                fbAddr->Reg032[page + i] = 0;
                            }
                        }
    
                        // We have to add the smart delay here since the old scripts didn't have the required delay
                        // Now incurr the penalty for stalling between power on and off
                        tmrGetCurrentTime(pDev, &timeHI, &timeLO);
                        if((pDev->Dac.PanelOffTimestampHI > timeHI) ||
                           ((pDev->Dac.PanelOffTimestampHI == timeHI) &&
                            (pDev->Dac.PanelOffTimestampLO > timeLO)) )
                        {
                            // Delay the difference of the times.  Unsigned math shouldn't matter if delay is under 2^31 nanoseconds.
                            tmrDelay(pDev, (pDev->Dac.PanelOffTimestampLO - timeLO));
                        }
        
                        //
                        // Attempt to process the scripts.  Note if this fails the code will fall through
                        // and use the legacy programming.
                        //
                        if ( rmStatus == RM_OK )
                        {
                            if ( ( panelFlags & 0x01 ) == 0 )
                            {
                                // Flag is 0
                                // Run BIOS LVDS single table
                                rmStatus = DevinitProcessBip3InternalTMDS( pDev, fpPixClk, TMDSIT_LVDS_SINGLE_B );
                            }
                            else
                            {
                                // Flag is 1
                                // Run BIOS LVDS dual table
                                rmStatus = DevinitProcessBip3InternalTMDS( pDev, fpPixClk, TMDSIT_LVDS_DUAL );
                            }
                        }

                        // Toshiba's uberscript actually does not turn the power on to the panel and backlight
                        // But Dell and everyone else should.  This is only for Mobile platforms which is checked above.
                        if(pDev->Power.MobileOperation != 2)
                        {
                            pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;
  
                            // We also need to turn off the power pending flag.
                            // If we leave this flag on, then we'll end up not allowing the dacBacklightOff script
                            // to run in dacMobileDisplayNotifyBegin(), which will show corruption during the
                            // modeset - Bad driver, bad, bad driver!
                            pDev->Power.Flags &= ~(DAC_POWER_PENDING);
                        }

                    } // end of BIOS is < revision 0x14
        
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
                            Data = dacTMDSRead(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_MODE );
                            // Or with 0x0C to say get from alternate source
                            // write register back to Link A
                            dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_MODE, (U008)( Data | 0x0C ));
                        }
    
                        //
                        // Until the BIOS contains scripts for enabling/disabling the panel
                        // power and backlight, we will still be forced to use separate
                        // routines (dacPowerOnMobilePanel, dacPowerOffMobilePanel) seen
                        // below.  Since those routines don't know yet the chip revision
                        // or BIOS script they're programming, we need to track if we
                        // programmed up the panel using the BIOS script or by using
                        // our legacy code (below).
                        //
                        // As soon as all the BIOS's are updated and we don't have to worry
                        // about legacy programming (yeah right), then we can take out this
                        // flag.
                        //
                        // This flag will be checked in the PowerOn/PowerOff routines to know
                        // what programming to perform.
                        //
                        pDev->Dac.BIOSPanelProgramming = 1;
                    }
                    else  // Failing side of internal script processing, so go to legacy programming
#endif  // MACOS
                    {
                        //
                        // Legacy programming flag -- to be picked up by PowerOn/PowerOff routines.
                        //
                        // Note this will always be the case for mac until the above is turned back
                        // on again for macos.
                        //
                        pDev->Dac.BIOSPanelProgramming = 0;
    
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
                        dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_PLL1, 0xD4);
                        //
                        //dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x3A);
                        //dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);
                        //
                        dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_IDLY, 0x00);
                        if (Head == 0)
                            dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_MODE, 0x81);
                        else
                            dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_MODE, 0x8D);
                        dacTMDSWrite(pDev, 0x0, NV_PRAMDAC_INDIR_TMDS_LVDS, 0x6E);
    
                        // Setup Link1
                        DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, 0x1101111, 0x1);
                        tmrDelay(pDev, 0x400000);
    
                        dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_PLL0, 0x32);
                        dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_PLL1, 0xD4);
                        //
                        //dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x3A);
                        //dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);
                        //
                        dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_IDLY, 0x00);
                        if (Head == 0)
                            dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_MODE, 0x8D);
                        else
                            dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_MODE, 0x81);
                        dacTMDSWrite(pDev, 0x1, NV_PRAMDAC_INDIR_TMDS_LVDS, 0x2E);
    
                        // Setup both Links to run at 1/2 the fpclk frequency
                        //
                        // Don't do this for Toshiba which is single link.  We need this info
                        // from either the BIOS (when it takes over the programming) or from
                        // the EDID.  For now, watch for Toshiba flags
                        //
                        if (pDev->Power.MobileOperation != 2)
                        {
                            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _FPCLK_RATIO, _DB2, 0);
                            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _FPCLK_RATIO, _DB2, 1);
                        }
                    } // end of legacy programming
                }  // end of if power is off (Don't run scripts if power is already on.)
            }
            else // failing side of if pDev->Power.MobileOperation
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

                if (rmStatus != RM_OK && IsNV11(pDev))
                {
                    // We failed to process the table, so do it the hardcoded way
                    // We only know how to do NV11.

                    // Determine PLL values based on pixclk
                    if (fpPixClk <= 6500) {         // pixclk <= 65MHz
#ifdef MACOS
                        // on the iMac we need to adjust PLL0 timing to reduce
                        //  emissions (Apple's request)
                        //  Test to detect iMac is a bit hokey, but will go away when we switch to
                        //  using the tables anyway...
                        //  NOTE we already know we are NV11 from above.
                        
                        if (pDev->halHwInfo.pMcHalInfo->MaskRevision < MC_MASK_REVISION_B1)
                            tmds_pll0 = 0xB2;
                        else
                        {
                            tmds_pll0 = 0x81;
                            if (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_SUB_VENDOR) == NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_NO_BIOS)
                                tmds_pll0 = 0x01;
                        }
#else
                        tmds_pll0 = 0xB2;
#endif
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


    if (IsNV11(pDev) && (FpMode == NV_CFGEX_GET_FLATPANEL_INFO_SCALED))
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

        //
        // Workaround to get reduced blanking on Dell's QXGA panels
        // REMOVE REMOVE REMOVE as soon as we have a robust alternative worked
        // out.  The EDID is correct in showing a 2048 horizontal total, but we
        // need to drive 2000 (0x7D0).
        //
        // Let me quote the hw engineer:
        //    We could use 0x7D0 for all QXGA panels, so far
        //    there are no issues with this settings for all QXGA timings.
        //
        if ((fpHActive == 0x800) && pDev->Power.MobileOperation)
            fpHActive = 0x7D0;

    }

    data32 = DAC_REG_RD32(NV_PRAMDAC_FP_DEBUG_0, Head);
    DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, (data32 & 0xfff0ffff) | 0x00020000, Head);
    tmrDelay(pDev, 0x400000);
    
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

    // Special case for flat panels with horizontal max 1600, 
    // overrides HW scaling for 1280->1600  in pre-nv11 boards
    if (  pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth == 1280
       && pDev->Dac.HalInfo.fpHMax == 1600
       && FpMode == NV_CFGEX_GET_FLATPANEL_INFO_SCALED
       && !IsNV11orBetter(pDev)
       ) 
    {   
        DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_1,  0x00001ccd, Head);
    }
    // NV10/15 seem to be missing table entries for scaling 720 modes
    // Write the scale factor 
    if (IsNV10(pDev) || IsNV15(pDev)) 
    {
        if (pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth == 720)
        {
            data32 = DAC_REG_RD32(NV_PRAMDAC_FP_DEBUG_1, Head);  // This should be 0
            switch (fpHActive) // res we are scaling into
            {
                case 1024:
                    data32 |= 0x1b3f;
                    break;
                case 1280:
                    data32 |= 0x18ff;
                    break;
                case 1600:
                    data32 |= 0x1732;
                    break;
            }
            // Check vertical 576 (480 will scale properly)
            // (These numbers were gotten by taking the factors in the manual for 512 and 600 and proportioning to 576)
            if (pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight == 576)
            {
                switch (fpVActive) 
                {
                case 768:
                    data32 |= 0x1bfe0000;
                    break;
                case 1024:
                    data32 |= 0x18fe0000;
                    break;
                case 1200:
                    data32 |= 0x17ac0000;
                    break;
                }
            }
            DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_1, data32, Head);
        }
    }

    tmrDelay(pDev, 0x400000);

    // since the flat panel is now setup, turn off the reset flag
    pDev->Dac.DevicesEnabled &=  ~(DAC_PANEL_CLK_RESET);

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
    U032 gpio_ext45;
    U032 timeHI, timeLO;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"dacPowerOnMobilePanel on Head");
    DBG_PRINT_VALUE(DEBUGLEVEL_TRACEINFO,Head);
    
    //
    // If we're already on, don't wait around here with the delays
    //
    // also make sure that this is mobile operation before skipping the good bits
    if(pDev->Power.MobileOperation && (((pDev->Dac.DevicesEnabled & DAC_PANEL_ENABLED) == DAC_PANEL_ENABLED) || (pDev->Power.Flags & DAC_POWER_PENDING)))
        return (RM_OK);

    // Retain GPIO_EXT_4 and GPIO_EXT_5 settings
    gpio_ext45  = REG_RD32(NV_PCRTC_GPIO_EXT);
    gpio_ext45 &= 0xFF00;

    // This delay is for non-Toshiba mobiles.
    // Toshiba flat panels are unaffected from this entire function.
    // We do not have direct control over the panel or backlight power for Toshiba.
    // We can call DPMS Sleep/Wake to turn off the backlight.
    // DeviceControlBegin/End BIOS calls also affect the power to the panel.
    // So this delay is unnecessary for Toshiba.
    if(pDev->Power.MobileOperation != 2)
    {
        // Now incurr the penalty for stalling between power on and off
        tmrGetCurrentTime(pDev, &timeHI, &timeLO);
        if((pDev->Dac.PanelOffTimestampHI > timeHI) ||
           ((pDev->Dac.PanelOffTimestampHI == timeHI) &&
            (pDev->Dac.PanelOffTimestampLO > timeLO)) )
        {
            // Delay the difference of the times.  Unsigned math shouldn't matter.
            tmrDelay(pDev, (pDev->Dac.PanelOffTimestampLO - timeLO));
        }
    }

    // First check to see if we can run the manufacturer scripts
    if(DevinitGetManufacturerInfo(pDev) == RM_OK)
    {
      // Let's power up the Manufacturer way
      DevinitRunManufacturerScript(pDev, MSCRIPT_PANEL_ON);
      
      // 100ms delay? why? it fixes the problem when cycling the hotkeys around to LCD
      // then using TwinView to turn on Clone mode (LCD->LCD+TV).  Without the delay,
      // this switch takes 9-12 seconds, because i2c fails.  With the delay, i2c works!
      if(pDev->Power.MobileOperation == 2)
        tmrDelay(pDev, 100000000);
    }
    //
    // Second check to see if we used the BIOS scripts to program the panel.
    // If so, we should use the BIOS scripts to turn on the panel as well,
    // but until that is complete in the BIOS, just perform the simplest
    // GPIO programming
    //
    else if (pDev->Dac.BIOSPanelProgramming == 1)
    {
        //
        // TOSHIBA doesn't like this new programming.  Until I get a Toshiba to 
        // debug...
        //
        // Remember Toshiba really handles their own power sequencing, so skipping
        // this should not be an issue.
        //
        //LPL: removed due to problems caused during display switching
//        if (pDev->Power.MobileOperation == 2)
//            return (RM_OK);

        // Disable breaker, this turns on the panel PLL
        REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45 | 0x10);

        // Toshiba does not use the GPIO_EXT to enable/disable bl or power.
        if(pDev->Power.MobileOperation != 2)
          pDev->Dac.DevicesEnabled |= (DAC_PANEL_POWER_ENABLED);

        // 220ms delay between power and bl enable
        tmrDelay(pDev, 220000000);

        // Power on the BL so we can see
        REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45 | 0x11);
    }
    else
    {
        if(pDev->Power.MobileOperation)
        {
            //
            // LEGACY PROGRAMMING
            //
            // make sure PLL is enabled, or we'll hang
            dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);   // FP on to access TMDS

            // Enable Head2 GPIO
            lock = UnlockCRTC(pDev, 1);
            CRTC_WR(NV_CIO_GPIO23_INDEX, 0x00, 1);
            RestoreLock(pDev, 1, lock);

            // Disable panel I/O
            dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x52);    // set PLL0
            dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x52);    // set PLL0

            // Wait a bit
            tmrDelay(pDev, 1000000);

            // Enable GPIO3 to a zero, set breaker
            //REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45 | 0x00);
            FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_2_OUTPUT,_0);
            FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_3_OUTPUT,0);
            FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_2_ENABLE,_ENABLE);
            FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_3_ENABLE,_ENABLE);

            // Wait a bit
            tmrDelay(pDev, 1000000);

            // Disable breaker, this turns on the panel PLL
            //REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45 | 0x10);
            FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_3_OUTPUT,1);

            // Wait a lot
            tmrDelay(pDev, 8000000);
        
            // Let DivBy7 go
            dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x38);    // set PLL2
            dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x38);    // set PLL2
        
            // Let DivBy10 go
            dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);    // set PLL1
            dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);    // set PLL1
        
            // Wait a bit
            tmrDelay(pDev, 1000000);
        
            // Reset DivBy10
            dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL1, 0xD4);    // set PLL1
            dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL1, 0xD4);    // set PLL1
        
            // Wait a bit
            tmrDelay(pDev, 1000000);
        
            // Reset DivBy7
            dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x3A);    // set PLL2
            dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x3A);    // set PLL2
        
            // Wait a bit
            tmrDelay(pDev, 1000000);
        
            // Let DivBy7 go again -- this leaves us in LVDS
            dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);    // set PLL1
            dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL1, 0x94);    // set PLL1
        
            // Wait a moderate bit
            tmrDelay(pDev, 2000000);
            
            // Power on the BL so we can see
            //
            //REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45 | 0x11);
            FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_2_ENABLE,_ENABLE);
            FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_2_OUTPUT,1);
        }
        else
        {   
            if(IsNV11(pDev))
            {
                if(osReadRegistryBoolean(pDev,NULL,"backlight-control"))
                {
                    //do P11 specifics
                    FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_2_OUTPUT,1);
                    FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_2_ENABLE,_ENABLE);
                }
                else
                {
                    //do NV11-P40 specifics
                    FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_3_OUTPUT,1);
                    FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_3_ENABLE,_ENABLE);
                }
            }

        
            //do NV20 specifics (on Mac, our flat panel enable is non standard)
            if(IsNV20(pDev))
            {
                REG_WR32(0x600818,REG_RD32(0x600818)|0x11);
            }
            
            //repair GPIO5 according to the inverter needs
            dacSetExternalInverterCurrentMode(pDev);
        }
    }

    // Toshiba does not use the GPIO_EXT to enable/disable bl or power.
    if(pDev->Power.MobileOperation != 2)
      pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;

        return (RM_OK);
}

RM_STATUS dacPowerOffMobilePanel
(
    PHWINFO pDev,
    U032    Head
)
{
 
    U008 lock;
    U032 gpio_ext45;
    U032 PanelOffOnDelay = 500;  // 500 ms is the default
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"dacPowerOffMobilePanel on Head");
    DBG_PRINT_VALUE(DEBUGLEVEL_TRACEINFO,Head);
    
    //
    // If we're already off, don't wait around here with the delays
    //
    //also make sure this is really mobile operation, not a Mac, not PowerEvent before skipping all the good bits
    // if ((REG_RD32(NV_PCRTC_GPIO_EXT) & 0x33) == 0x00)
    if(pDev->Power.MobileOperation && ((pDev->Dac.DevicesEnabled & DAC_PANEL_ENABLED) == DAC_NOT_ENABLED || (pDev->Power.Flags & DAC_POWER_PENDING)))
        return (RM_OK);


    if (MC_POWER_LEVEL_0 != pDev->Power.State) {
        return (RM_OK);
        }
               

    if(DevinitGetManufacturerInfo(pDev) == RM_OK)
    {
      // Let's power down the Manufacturer way
      DevinitRunManufacturerScript(pDev, MSCRIPT_PANEL_OFF);

      PanelOffOnDelay = pDev->Dac.ManufacturerProperties.PanelOffOnDelay;
    }
    //
    // Second, check to see if we used the BIOS scripts to program the panel.
    // If so, we should use the BIOS scripts to turn on the panel as well,
    // but until that is complete in the BIOS, just perform the simplest
    // GPIO programming
    //
    else if (pDev->Dac.BIOSPanelProgramming == 1)
    {
        //
        // TOSHIBA doesn't like this new programming.  Until I get a Toshiba to 
        // debug...
        //
        // Remember Toshiba really handles their own power sequencing, so skipping
        // this should not be an issue.
        //
        //LPL: removed due to problems caused during display switching
//        if (pDev->Power.MobileOperation == 2)
//            return (RM_OK);

        // Read the register for the panel backlight and power
        gpio_ext45  = REG_RD32(NV_PCRTC_GPIO_EXT);

        // Only disable the backlight if it's actually on!
        // Currently we're forcing the SW flag on in mcPowerState 
        // (Returning to 0 from level 2).  If the panel power is
        // actually off, then writing ((gpio_ext45 & 0xFF00) | 0x10)
        // will turn the panel on.
        REG_WR32(NV_PCRTC_GPIO_EXT, (gpio_ext45 & 0xFFF0));

        // Toshiba does not use the GPIO_EXT to enable/disable bl or power.
        if(pDev->Power.MobileOperation != 2)
          pDev->Dac.DevicesEnabled &= ~(DAC_PANEL_BACKLIGHT_ENABLED);
        
        // 20 ms delay between bl off and power off?
        // According to the Panel Timing Specs worksheet, 20 ms is the minimum
        tmrDelay(pDev, 20000000);
    
        // disable panel power    
        REG_WR32(NV_PCRTC_GPIO_EXT, (gpio_ext45 & 0xFF00));
    }
    else
    {
        //
        // LEGACY PROGRAMMING
        //
        if(pDev->Power.MobileOperation)
        {
            // Enable Head2 GPIO
            lock = UnlockCRTC(pDev, 1);
            CRTC_WR(NV_CIO_GPIO23_INDEX, 0x00, 1);
            RestoreLock(pDev, 1, lock);
        
            // make sure PLL is enabled, or we'll hang
            dacWriteLinkPLL(pDev, 0, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);     // FP on to access TMDS
            dacWriteLinkPLL(pDev, 1, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);
        
            // BL disable
            //REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45 | 0x10);
            FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_2_ENABLE,_ENABLE);
            FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_2_OUTPUT,0);
        
            // Disable panel I/O
            dacTMDSWrite(pDev, 0, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x52);    // set PLL0
            dacTMDSWrite(pDev, 1, NV_PRAMDAC_INDIR_TMDS_PLL2, 0x52);    // set PLL0
        
            tmrDelay(pDev, 1000000);
        
            // disable panel power
            //REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45 | 0x00);
            FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_3_ENABLE,_ENABLE);
            FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_3_OUTPUT,0);
        }
        else
        {
            if(IsNV11(pDev))
            {
                if(osReadRegistryBoolean(pDev,NULL,"backlight-control"))
                {
                    //Disable panel backlight only on P11
                    FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_2_ENABLE,_ENABLE);
                    FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_2_OUTPUT,0);
                }
                else
                {
                    //disable GPIO3 which is wired to the panel power control on P40
                    FLD_WR_DRF_DEF(_PCRTC,_GPIO_EXT,_3_ENABLE,_ENABLE);
                    FLD_WR_DRF_NUM(_PCRTC,_GPIO_EXT,_3_OUTPUT,0);
                }
            }
            
            //if nv20 (arg!)
            //do NV20 specifics (on Mac, our flat panel enable is non standard)
            //will go away with virtualized GPIOs
            if(IsNV20(pDev))
            {
                REG_WR32(0x600818,REG_RD32(0x600818)&~0x11);
            }
        }
    }
        
        //satisfy GPIO5 reqs.
    dacSetExternalInverterCurrentMode(pDev);

    // Start smart time delay and turn off SW flag for non-Toshiba mobiles
    // Toshiba flat panels are unaffected from this entire function.
    // We do not have direct control over the panel or backlight power for Toshiba.
    // We can call DPMS Sleep/Wake to turn off the backlight.
    // DeviceControlBegin/End BIOS calls also affect the power to the panel.
    // So this delay is unnecessary for Toshiba.
    if(pDev->Power.MobileOperation != 2)
    {
        //
        // Need to abide by minimum 500ms between panel off and back on again, so
        // enforce it here while the panel is black
        // tmrDelay(pDev, 500000000);
        // 500 ms was the old minimum.  This is now changed by using the PanelOffOnDelay.

        // Store the current time, and then stall on the panel on call
        tmrGetCurrentTime(pDev, 
            &(pDev->Dac.PanelOffTimestampHI), &(pDev->Dac.PanelOffTimestampLO));
            
        // Change the PanelOffOnDelay from ms to ns
        PanelOffOnDelay *= 1000000;
        
        // Go ahead and add the delay to the timestamp
        // Check for carry condition
        if((pDev->Dac.PanelOffTimestampLO + PanelOffOnDelay) < pDev->Dac.PanelOffTimestampLO)
        {
            pDev->Dac.PanelOffTimestampHI++;
        }
        pDev->Dac.PanelOffTimestampLO += PanelOffOnDelay;
    
        // Toshiba does not use the GPIO_EXT to enable/disable bl or power.
        pDev->Dac.DevicesEnabled &= ~(DAC_PANEL_ENABLED);
    }

    return (RM_OK);
}

RM_STATUS dacBacklightOnMobilePanel(PHWINFO pDev)
{
    RM_STATUS status = RM_OK;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"dacBacklightOnMobilePanel");

    // Only do this if Mobile and the backlight is off
    if ((pDev->Power.MobileOperation) && 
        (!(pDev->Dac.DevicesEnabled & DAC_PANEL_BACKLIGHT_ENABLED)))
    {
        // Toshiba doesn't turn off their backlight with Manufacturer scripts or GPIO pins
        if(pDev->Power.MobileOperation != 2)
        {
            // Try manufacturer scripts first
            // Currently the backlight on script is missing an INIT_DONE.
            // and it turns the backlight on then off.  So for now, just
            // do it the old fashioned way.
            if(0)//DevinitGetManufacturerInfo(pDev) == RM_OK)
            {
                // Let's turn the backlight on the Manufacturer way
                status = DevinitRunManufacturerScript(pDev, MSCRIPT_BACKLIGHT_ON);
            }
            else
            {
                U032 gpio_ext45;
    
                // Retain GPIO_EXT_4 and GPIO_EXT_5 settings
                gpio_ext45  = REG_RD32(NV_PCRTC_GPIO_EXT);
                gpio_ext45 &= 0xFFF0;
      
                // Power on the BL so we can see
                REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45 | 0x01);

                status = RM_OK;
            }
        }
        else
        {
            // Turn on the backlight after enabling the DACs
            U032    EAX, EBX, ECX, EDX;
            EAX = 0x4F10;
            EBX = 0x0001; //wake up
            EDX = 0x0000;
            ECX = 0x0000;
            status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
        }
        
        if(status != RM_OK)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"NVRM: ERROR!!! dacBacklightOnMobilePanel failed.  SW State is invalid.\n");
        }

        pDev->Dac.DevicesEnabled |= (DAC_PANEL_BACKLIGHT_ENABLED);
    }
    return status;
}

RM_STATUS dacBacklightOffMobilePanel(PHWINFO pDev)
{
    RM_STATUS status = RM_OK;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"dacBacklightOffMobilePanel");

    // Only do this if Mobile and the backlight is on
    if ((pDev->Power.MobileOperation) && 
        (pDev->Dac.DevicesEnabled & DAC_PANEL_BACKLIGHT_ENABLED))
    { 
      if(pDev->Power.MobileOperation != 2)
      {
          if(DevinitGetManufacturerInfo(pDev) == RM_OK)
          {
              // Let's turn the backlight off the Manufacturer way
              status = DevinitRunManufacturerScript(pDev, MSCRIPT_BACKLIGHT_OFF);
          }
          else
          {
              U032 gpio_ext45;
    
              // Retain GPIO_EXT_4 and GPIO_EXT_5 settings
              gpio_ext45  = REG_RD32(NV_PCRTC_GPIO_EXT);
              gpio_ext45 &= 0xFFF0;
    
              // Power off the Backlight
              REG_WR32(NV_PCRTC_GPIO_EXT, gpio_ext45);

              status = RM_OK;
          }
      }
      else  // Toshiba case
      {
          // Turn off the backlight
          U032    EAX, EBX, ECX, EDX;
          EAX = 0x4F10;
          EBX = 0x0401; //sleep
          EDX = 0x0000;
          ECX = 0x0000;
          status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
      }
        
      if(status != RM_OK)
      {
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"NVRM: ERROR!!! dacBacklightOffMobilePanel failed.  SW State is invalid.\n");
      }
          
      pDev->Dac.DevicesEnabled &= ~(DAC_PANEL_BACKLIGHT_ENABLED);
    }
    return status;
}

void dacZeroPWMController(PHWINFO pDev)
{
        REG_WR32(NV_PBUS_PWM,0);
}

void dacSetPWMController(PHWINFO pDev)
{
        REG_WR32(NV_PBUS_PWM,pDev->Dac.fpPWMRegisterContents);
}

void dacSetExternalInverterCurrentMode(PHWINFO pDev)
{
    U032 nvPcrtcGpioExtVal;
    
    if (pDev->Power.MobileOperation)
        return;    // mobile uses GPIO_EXT_5 for controlling spread spectrum

    nvPcrtcGpioExtVal=REG_RD32(NV_PCRTC_GPIO_EXT);
    
    //uninit'd variable check
    switch(pDev->Dac.fpExternalInverterCurrentMode)
    {
        case 0:
        case 1:
            break;
        default:
            pDev->Dac.fpExternalInverterCurrentMode=0;
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Possible uninitialized fpExternalInverterCurrentMode!.\n");
    }
    
    if(pDev->Dac.fpExternalInverterCurrentMode)
    {       
        //set the bit
        nvPcrtcGpioExtVal|=BIT(12);
        
        //clear the GPIO line disable bit
        nvPcrtcGpioExtVal&=~BIT(13);
    }
    else
    {
        //set the output line to zero
        nvPcrtcGpioExtVal&=~BIT(12);
        
        //clear the GPIO line disable bit
        nvPcrtcGpioExtVal&=~BIT(13);
    }
    
    REG_WR32(NV_PCRTC_GPIO_EXT,nvPcrtcGpioExtVal);
}

void dacGetExternalInverterCurrentMode(PHWINFO pDev)
{
    U032 nvPcrtcGpioExtVal;
    
    if (pDev->Power.MobileOperation)
        return;    // mobile uses GPIO_EXT_5 for controlling spread spectrum

    nvPcrtcGpioExtVal=REG_RD32(NV_PCRTC_GPIO_EXT);
    
    if(BIT(14)&nvPcrtcGpioExtVal)
    {
        pDev->Dac.fpExternalInverterCurrentMode=1;
    }
    else
    {
        pDev->Dac.fpExternalInverterCurrentMode=0;
    }
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

RM_STATUS  dacSetFlatPanelBrightness
(
    PHWINFO pDev,
    U032 Head,
    U032 Brightness,
    U032 Frequency,
    U032 MaxPercent,
    U032 MinPercent,
    BOOL ControllerIsOn,
    BOOL InverterCurrentMode
)
{
    U032 TicksNeededForClock;
    U032 TicksNeededForHigh;
    
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
    
    TicksNeededForClock = GetTicksForFrequency(Frequency);
    TicksNeededForHigh = GetRequiredTickCount(Frequency,MaxPercent,MinPercent,Brightness);

    //program the chip
    FLD_WR_DRF_NUM(_PBUS,_PWM,_HI,TicksNeededForHigh);
    FLD_WR_DRF_NUM(_PBUS,_PWM,_PERIOD,TicksNeededForClock);
    
    //store multiplier for get
    pDev->Dac.fpRelativeBrightness=Brightness;
    
    //set current mode
    pDev->Dac.fpExternalInverterCurrentMode=(U032)InverterCurrentMode;
    dacSetExternalInverterCurrentMode(pDev);
    
    TicksNeededForClock=GetTicksForFrequency(Frequency);
    TicksNeededForHigh=GetRequiredTickCount(Frequency,MaxPercent,MinPercent,Brightness);

    //store multiplier for get
    pDev->Dac.fpRelativeBrightness=Brightness;
        
    //store the raw PWM value in case someone triggers an on without an off
    pDev->Dac.fpPWMRegisterContents = REG_RD32(NV_PBUS_PWM);
                    
    return RM_OK;
}

RM_STATUS  dacGetFlatPanelBrightness(PHWINFO pDev, U032 Head, U032 * Brightness,BOOL * ControllerIsOn, BOOL * InverterCurrentMode)
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
        
        dacGetExternalInverterCurrentMode(pDev);
        *InverterCurrentMode=(BOOL)pDev->Dac.fpExternalInverterCurrentMode;

        return RM_OK;
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

        TicksInClockPeriod = GetTicksForFrequency(Frequency);
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
    if((pDev->Dac.DevicesEnabled & DAC_PANEL_POWER_ENABLED)&&pDev->Power.MobileOperation)
      return TRUE;

    if(REG_RD_DRF(_PCRTC,_GPIO_EXT,_3_INPUT))
    {
        //technically, based on policy, this should be a check for GPIO4 up, but this is only the case on P11
        //this interface is insuffecient to determine that.
        return TRUE;
    }
    
    //this must suffice until the HAL function for GPIO arrives
    //reg 0x600818 is NV_PCRTC_GPIO, BIT(24) is GPIO1 input
    if((IsNV20(pDev))&&(REG_RD32(0x600818)&BIT(24)))
    {
        return TRUE;
    }
    
    return FALSE;
}

