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

/**************************** Interrupt Routine ****************************\
*                                                                           *
* Module: VBLANK.C                                                          *
*   Vertibal blank interrupt servicing is handled here.  This is a true     *
*   interrupt time call so no os services are available.                    *
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
#include <vblank.h>
#include <gr.h>
#include <dac.h>
#include <os.h>
#include <vga.h>
#include "nvhw.h"
#include <bios.h>   //For hotkey defs.  Can remove once those are put in nv_ref.h
#include <nv10_ref.h>    // for PCRTC_CURSOR_CONFIG def and mobile FP dithering 



// Const strings for accessing the registry
extern char strFpMode[];

// external interfaces
VOID VBlankAddCallback(PHWINFO, U032, PVBLANKCALLBACK);
VOID VBlankDeleteCallback(PHWINFO, U032, PVBLANKCALLBACK);
U032 VBlankPending(PHWINFO);
VOID VBlankUpdateDither(PHWINFO,U032 Head);


RM_STATUS mthdColormapDirtyNotify(PHWINFO, RM_STATUS);

static VOID VBlankProcessCallbacks
(
    PHWINFO pDev,
    U032 Head
)
{
    PVBLANKCALLBACK Callback;
    PVBLANKCALLBACK NextCallback;
    PVBLANKCALLBACK *PrevCallback;
    PDACCRTCINFO pDacCrtcInfo;


    pDacCrtcInfo = &pDev->Dac.CrtcInfo[Head];
    Callback     = (PVBLANKCALLBACK)pDacCrtcInfo->VBlankCallbackList;
    PrevCallback = (PVBLANKCALLBACK *)&pDacCrtcInfo->VBlankCallbackList;

    while (Callback) {
        NextCallback   = Callback->Next;
        Callback->Next = NULL;

        // Execute this on a particular VBlankCount?
        if ((Callback->Flags & CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT) &&
            (Callback->VBlankCount != pDacCrtcInfo->VBlankCounter)) {
            PrevCallback = &Callback->Next;     // skip it
        } else {
            // Call the function now
            if (Callback->Proc) {
                Callback->Proc(pDev,
                               Callback->Object,
                               Callback->Param1,
                               Callback->Param2,
                               Callback->Status);
            }
            *PrevCallback = NextCallback;       // unlink it
        }
        Callback = NextCallback;
    }
}


VOID VBlankAddCallback
(
    PHWINFO         pDev, 
    U032            Head,
    PVBLANKCALLBACK Callback
)
{
    RM_ASSERT(Head <= MAX_CRTCS);

    //
    // There are two conditions we need to guard against here:
    //
    //   [1] This head doesn't have a valid mode.  This can happen
    //       if we get here after dacDestroyObj.
    //   [2] Vblank interrupts have been disabled.  This can happen
    //       if we get here after dacDisableDac via some power mgmt.
    //
    // In both of these cases, we need to handle the callback immediately
    // rather than queue it up since there won't be a vblank intr.
    //
    // To test for [1], we check for an active dac object.  This will
    // tell us if this head has seen a modeset.
    //
    // To test for [2], we check the VBLANK_ENABLED flag.
    //
    // Neither is the cleanest thing, but should be OK for now.
    //
    if (pDev->Dac.CrtcInfo[Head].pVidLutCurDac &&
        (pDev->Dac.CrtcInfo[Head].StateFlags & DAC_CRTCINFO_VBLANK_ENABLED))
    {
        // queue it
        Callback->Next = (PVBLANKCALLBACK)pDev->Dac.CrtcInfo[Head].VBlankCallbackList;
        pDev->Dac.CrtcInfo[Head].VBlankCallbackList = (VOID_PTR)Callback;
    }
    else
    {
        // call it now
        if (Callback->Proc) {
            // Force it to appear to be on the correct VBlankCount
            Callback->VBlankCount = pDev->Dac.CrtcInfo[Head].VBlankCounter;
            Callback->Proc(pDev,
                           Callback->Object,
                           Callback->Param1,
                           Callback->Param2,
                           Callback->Status);
        }
    }
}


//
// This routine can be called from an API entry point.
//
// For winnt4/win2k the necessary locking is already preventing a conflict
// with the VBlank handler running concurrently. For win9x, make sure the
// interrupt is disable before pulling callbacks off the list.
//
VOID VBlankDeleteCallback
(
    PHWINFO         pDev,
    U032            Head,
    PVBLANKCALLBACK Callback
)
{
    PDACCRTCINFO pDacCrtcInfo;
    U032 enable;

    RM_ASSERT(Head <= MAX_CRTCS);
    pDacCrtcInfo = &pDev->Dac.CrtcInfo[Head];

    // Disable VBlank while we scan/process the callback list

    enable = DAC_REG_RD_DRF(_PCRTC, _INTR_EN_0, _VBLANK, Head);
    DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_EN_0, _VBLANK, _DISABLED, Head);

    // Search the list and remove this Callback entry
    if (pDacCrtcInfo->VBlankCallbackList == (VOID_PTR)Callback) {
        //
        // Found it.
        // Should the callback be executed as part of the object destroy
        // (safe to do, since we already hold the necessary lock).
        //
        if (Callback->Proc && 
            (Callback->Flags & CALLBACK_FLAG_COMPLETE_ON_OBJECT_CLEANUP)) {
            // Force it to appear to be on the correct VBlankCount
            Callback->VBlankCount = pDacCrtcInfo->VBlankCounter;
            Callback->Proc(pDev,
                           Callback->Object,
                           Callback->Param1,
                           Callback->Param2,
                           Callback->Status);
        }
        pDacCrtcInfo->VBlankCallbackList = (VOID_PTR)Callback->Next;
    } else {
        PVBLANKCALLBACK PrevCallback = (PVBLANKCALLBACK)pDacCrtcInfo->VBlankCallbackList;

        while (PrevCallback) {
            if (PrevCallback->Next == Callback) {
                //
                // Found it.
                // Should the callback be executed as part of the object destroy
                // (safe to do, since we already hold the necessary lock).
                //
                if (Callback->Proc && 
                    (Callback->Flags & CALLBACK_FLAG_COMPLETE_ON_OBJECT_CLEANUP)) {
                    // Force it to appear to be on the correct VBlankCount
                    Callback->VBlankCount = pDacCrtcInfo->VBlankCounter;
                    Callback->Proc(pDev,
                                   Callback->Object,
                                   Callback->Param1,
                                   Callback->Param2,
                                   Callback->Status);
                }
                PrevCallback->Next = Callback->Next;
                break;
            }
            PrevCallback = PrevCallback->Next;
        }
    }
    Callback->Next = NULL;

    // Restore VBlank enable
    DAC_FLD_WR_DRF_NUM(_PCRTC, _INTR_EN_0, _VBLANK, enable, Head);
}

//
// VBlankUpdateFlip
//
static U032 VBlankUpdateFlip
(
    PHWINFO         pDev,
    U032            Head
)
{
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT vidLutCurDacHalObj;
    U008 Cr07, Cr25;
    U008 lock;
    U016 DisplayEnd, Scanline;
    U032 rmFinish = FALSE;
#ifdef NTRM
    U008 crtc_color, crtc_index;
#endif

    //
    // Initialize logical head so that the HAL will know which
    // crtc to program up.
    //
    vidLutCurDacHalObj.Head = Head;

#ifdef NTRM
    // XXX temporarily NT only (this needs to be made common)
    crtc_color = (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01);    // color or mono?
    if (crtc_color)
        crtc_index = REG_RD08(NV_PRMCIO_CRX__COLOR);
    else
        crtc_index = REG_RD08(NV_PRMCIO_CRX__MONO);
#endif

    // Unlock CRTC extended regs
    lock = UnlockCRTC(pDev, Head);

    //
    // Update framebuffer address?
    //
    if (pDev->Framebuffer.UpdateFlags & UPDATE_HWINFO_BUFFER_FLIP)
    {
        pDev->Framebuffer.FlipFrom = pDev->Framebuffer.Current;
        pDev->Framebuffer.Current  = pDev->Framebuffer.FlipTo;
        if (!pDev->Vga.Enabled)
        {
            //
            // Make sure we're really in blank
            //
            if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_FLAT_PANEL)
                while (REG_RD_DRF(_PCRTC, _RASTER, _VERT_BLANK) != NV_PCRTC_RASTER_VERT_BLANK_ACTIVE)
                    ;
            else // !? Easy way isn't working with flat panel. Use scanline counter instead.
            {
                // Compare scanline count to display end
                CRTC_RD(NV_CIO_CR_VDE_INDEX, DisplayEnd, Head);
                CRTC_RD(NV_CIO_CR_OVL_INDEX, Cr07, Head);
                CRTC_RD(NV_CIO_CRE_LSR_INDEX, Cr25, Head);
                if (Cr07 & 2) DisplayEnd |= 0x100;
                if (Cr07 & 0x40) DisplayEnd |= 0x200;
                if (Cr25 & 2) DisplayEnd |= 0x400;                    

                nvHalDacGetRasterPosition(pDev, &Scanline, (VOID*) &vidLutCurDacHalObj);
                while (Scanline < DisplayEnd)
                {
                    nvHalDacGetRasterPosition(pDev, &Scanline, (VOID*) &vidLutCurDacHalObj);
                }
            }

            //
            // Program the new start address; this will latch at the end of blank
            //
            if (!pDev->pStereoParams || !(pDev->pStereoParams->Flags & STEREOCFG_STEREOACTIVATED))
                nvHalDacSetStartAddr(pDev, pDev->Framebuffer.HalInfo.Start[pDev->Framebuffer.Current], &vidLutCurDacHalObj);
            else
                RM_ASSERT(FALSE);

        }
        pDev->Framebuffer.UpdateFlags &= ~UPDATE_HWINFO_BUFFER_FLIP;       
        pDev->Framebuffer.FinishFlags |=  FINISH_HWINFO_BUFFER_FLIP;
        rmFinish = TRUE;
    }

    if (pDev->pStereoParams &&
       (pDev->pStereoParams->Flags & (STEREOCFG_STEREOACTIVATED|STEREOCFG_INTERLACED)) == STEREOCFG_STEREOACTIVATED &&
        pDev->pStereoParams->FlipOffsets[3][0] != 0xFFFFFFFF)
    {
        unsigned long *pBufferOffset;
        if (pDev->pStereoParams->FlipOffsets[3][0] == pDev->pStereoParams->FlipOffsets[0][0])
            pBufferOffset = &pDev->pStereoParams->FlipOffsets[0][0];
        else
        if (pDev->pStereoParams->FlipOffsets[3][0] == pDev->pStereoParams->FlipOffsets[1][0])
            pBufferOffset = &pDev->pStereoParams->FlipOffsets[1][0];
        else
        if (pDev->pStereoParams->FlipOffsets[3][0] == pDev->pStereoParams->FlipOffsets[2][0])
            pBufferOffset = &pDev->pStereoParams->FlipOffsets[2][0];
        else
        {
            pDev->pStereoParams->FlipOffsets[3][1] = pDev->pStereoParams->FlipOffsets[3][0];
            pBufferOffset = &pDev->pStereoParams->FlipOffsets[3][0];
            //The only explanation that the last flip offset doesn't match one of ours is
            //that it happened upon someone else request. This is most likely a video mode
            //restore taking place when an application ends. The safest strategy here would
            //be to force stereo off.
            pDev->pStereoParams->Flags = 0;
        }
        pDev->pStereoParams->EyeDisplayed ^= 1;
        nvHalDacSetStartAddr(pDev, pBufferOffset[pDev->pStereoParams->EyeDisplayed], &vidLutCurDacHalObj);
        if (pDev->pStereoParams->Flags & STEREOCFG_ELSADDC)
        {
            U008 DDCVal;
            U008 DDCOwner = 0;         // avoid unused compile warning
            if (IsNV11(pDev))
            {
                //Make sure we own the DDC.
                DDCOwner = REG_RD08(NV_CIO_CRE_ENGINE_CTRL);
                AssocDDC(pDev, Head);
            }
            DDCVal = pDev->pStereoParams->EyeDisplayed ? 0x1 : 0x11;
            CRTC_WR(NV_CIO_CRE_DDC_WR__INDEX, DDCVal, Head);
            if (IsNV11(pDev))
            {
                REG_WR08(NV_CIO_CRE_ENGINE_CTRL, DDCOwner);
            }
        }
    }

    // Relock CRTC extended regs
    RestoreLock(pDev, Head, lock);

#ifdef NTRM
    //
    // Restore crtc index register
    //
    if (crtc_color)
        REG_WR08(NV_PRMCIO_CRX__COLOR, crtc_index);
    else
        REG_WR08(NV_PRMCIO_CRX__MONO, crtc_index);
#endif

    return rmFinish;
}

//
// VBlankUpdatePalette
//
static VOID VBlankUpdatePalette
(
    PHWINFO         pDev,
    U032            Head
)
{
    U032 i, j, k;
    U016 Width, Height;
    U016  color;
    U008  test, mask;
    U032  color32;
    U008  color8;
    U008  *lpColorMask;

#define   COLOR_OFFSET    32*4    // offset of XOR mask from start of buffer (= size of and mask)

    if (pDev->Dac.UpdateFlags & UPDATE_HWINFO_DAC_COLORMAP)
    {
        if (!pDev->Vga.Enabled)
        {
            if (pDev->Dac.HalInfo.Depth == 8)
                dacProgramLUT(pDev, Head, &pDev->Dac.Palette[0], 256);
            else
                dacProgramLUT(pDev, Head, &pDev->Dac.Gamma[0], 256);

            // JJV- Update the cursor to the new DAC values.
            // Read the saved DAC indices from nvinfo ... the cursor
            // indices are saved after each update.
            // Generate a cursor image update to force a restore on
            // the cursor palette. 
            if ( (pDev->Dac.HalInfo.Depth == 8)  && 
                 (pDev->Dac.CursorBitDepth == 8) &&
                 (pDev->Dac.CrtcInfo[Head].CursorType == DAC_CURSOR_FULL_COLOR_XOR) )
            {
                lpColorMask = pDev->Dac.SavedCursor + COLOR_OFFSET;
                Width=Height=32;
                for (i = 0; i < Height; i++)
                {
                    for (j = 0; j < (U032)Width/8; j++)
                    {
                        mask = pDev->Dac.SavedCursor[i * sizeof(U032) + j]; // get 8 AND bits                                
                        test = 0x80;
                        for (k = 0; k < 8; k++, test >>=1)  // get 8 pixels and write 8 words
                        {
                            color8 = lpColorMask[i*32+j*8+k];   // get 8 bits of color
                            color32 = pDev->Dac.Palette[color8];     // look up color (24 bits)
                            // pack 8-8-8 into 5-5-5, using most sig bits 
                            color =  (U016)((color32 >> 9) & 0x7c00);   // red
                            color |= (color32 >> 6) & 0x03e0;           // green
                            color |= (color32 >> 3) & 0x001f;           // blue
                            if (!(test & mask))
                                color |= 0x8000;                            
                            pDev->Dac.CursorColorImage[i][j*8+k] = color;
                        }
                    }
                }    
                pDev->Dac.UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_IMAGE;
            }
                
            // We have just updated the colormap. Notify the object.
            mthdColormapDirtyNotify(pDev, RM_OK);
            pDev->Dac.UpdateFlags &= ~UPDATE_HWINFO_DAC_COLORMAP;
        }
    }
}

//
// VBlankUpdateVideoScaler
//
static U032 VBlankUpdateVideoScaler
(
    PHWINFO         pDev,
    U032            Head
)
{
    U032 rmFinish = FALSE;

    //
    // Update the video scaler settings
    //
    if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_DUALSURFACE)   // if we are using the scalar for dual surface, don't change it
    {
        if (pDev->Video.HalInfo.UpdateFlags & UPDATE_HWINFO_VIDEO_ENABLE)
        {
            if (pDev->Video.HalInfo.Enabled)
                nvHalVideoControl(pDev, VIDEO_CONTROL_ENABLE);
            else
                nvHalVideoControl(pDev, VIDEO_CONTROL_DISABLE);
            pDev->Video.HalInfo.UpdateFlags &= ~UPDATE_HWINFO_VIDEO_ENABLE;
            pDev->Video.HalInfo.FinishFlags |=  FINISH_HWINFO_VIDEO_ENABLE;
            rmFinish = TRUE;
        }
    } // dualsurface    
    else
    {
        if (pDev->Video.HalInfo.UpdateFlags & UPDATE_HWINFO_VIDEO_ENABLE)   // if dual surf, set TV for YUV
        {
            if (pDev->Video.HalInfo.Enabled)
                FLD_WR_DRF_DEF(_PRAMDAC, _TV_SETUP, _VS_PIXFMT, _YUV);
            else
                FLD_WR_DRF_DEF(_PRAMDAC, _TV_SETUP, _VS_PIXFMT, _565);
            pDev->Video.HalInfo.UpdateFlags &= ~UPDATE_HWINFO_VIDEO_ENABLE;
            pDev->Video.HalInfo.FinishFlags |=  FINISH_HWINFO_VIDEO_ENABLE;
            rmFinish = TRUE;
        }                
    }

    return rmFinish;
}

//
// VBlankUpdateCursor
//
// Handle win9x cursor updates.
//
static VOID VBlankUpdateCursor
(
    PHWINFO         pDev,
    U032            Head
)
{
    U008 crtc_color, crtc_index;
    PDACCRTCINFO pDacCrtcInfo;
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT VidLutCurDac;

    VidLutCurDac.Head = Head;

    //
    // Save away the current CRTC index values.
    //
    // This should be in VBlank common to all callbacks, but placing it there
    // caused problems at boot time (which are still a mystery). For now, make
    // the save/restore local to the cursor update callback.
    //
    crtc_color = (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01);    // color or mono?
    if (crtc_color)
        crtc_index = REG_RD08(NV_PRMCIO_CRX__COLOR);
    else
        crtc_index = REG_RD08(NV_PRMCIO_CRX__MONO);

    pDacCrtcInfo = &pDev->Dac.CrtcInfo[Head];

    // position cursor?
    if (pDacCrtcInfo->UpdateFlags & UPDATE_HWINFO_DAC_CURSOR_POS)
    {
        dacProgramCursorPosition(pDev, Head,
                                 pDacCrtcInfo->CursorPosX,
                                 pDacCrtcInfo->CursorPosY);
        pDacCrtcInfo->UpdateFlags &= ~UPDATE_HWINFO_DAC_CURSOR_POS;
    }

#ifdef WIN31    // This is the old way of updating the cursor image

    // update glyph?
    if (pDacCrtcInfo->UpdateFlags & UPDATE_HWINFO_DAC_CURSOR_IMAGE)
    {
        U032 i, j;

        // The image was copied into the pDev->structure in the API call.
        // We need to copy into the Instance memory, where the DAC can find it.

        // The 2k image has been formatted by the OS module NVSYS.C, 
        // The buffer is a 32x32 array of words. Each word represents a pixel.
        for (i = 0; i < 32; i++)    
            for (j = 0; j < 32; j+=2)
            {
                U032 data;
                // grab two words at a time and stuff them in instance mem
                data = pDacCrtcInfo->CursorColorImage[i][j+1];
                data = (data << 16) | pDacCrtcInfo->CursorColorImage[i][j];
                INST_WR32(pDacCrtcInfo->CursorImageInstance, i*64+j*2, data);
            }
    }
#else
    // The display driver massages the image and hands us a pointer (to image in framebuffer).
    // This is the new way of manipulating the cursor image. 
    if (pDacCrtcInfo->UpdateFlags & UPDATE_HWINFO_DAC_CURSOR_IMAGE_NEW)
    {
        nvHalDacProgramCursorImage(pDev,
                              pDacCrtcInfo->CursorOffset, 
                              DAC_ASI_FRAMEBUFFER, 
                              pDacCrtcInfo->CursorWidth, pDacCrtcInfo->CursorHeight,
                              pDacCrtcInfo->CursorColorFormat,
                              (VOID*) &VidLutCurDac);
        pDacCrtcInfo->UpdateFlags &= ~UPDATE_HWINFO_DAC_CURSOR_IMAGE_NEW;
    }
#endif
    if (pDacCrtcInfo->UpdateFlags & UPDATE_HWINFO_DAC_CURSOR_ENABLE)
    {
        // Enable the cursor.
        dacEnableCursor(pDev, Head);
        pDacCrtcInfo->UpdateFlags &= ~UPDATE_HWINFO_DAC_CURSOR_ENABLE;
    }

    if (pDacCrtcInfo->UpdateFlags & UPDATE_HWINFO_DAC_CURSOR_DISABLE)
    {
        // Disable the cursor.
        dacDisableCursor(pDev, Head);
        pDacCrtcInfo->UpdateFlags &= ~UPDATE_HWINFO_DAC_CURSOR_DISABLE;
    }

    //
    // Restore crtc index register
    //
    if (crtc_color)
        REG_WR08(NV_PRMCIO_CRX__COLOR, crtc_index);
    else
        REG_WR08(NV_PRMCIO_CRX__MONO, crtc_index);
}

//
// VBlankUpdateImageOffsetFormat
//
// Handle win9x panning start address updates. This could be made to be
// a generalized backdoor taking method offsets and data.
//
static VOID VBlankUpdateImageOffsetFormat
(
    PHWINFO         pDev,
    U032            Head
)
{
    PDACCRTCINFO pDacCrtcInfo;
    RM_STATUS status;

    pDacCrtcInfo = &pDev->Dac.CrtcInfo[Head];
    if (pDacCrtcInfo->UpdateFlags & UPDATE_HWINFO_DAC_IMAGE_OFFSET_FORMAT)
    {
        POBJECT Object;

        // Given the object handle, find the POBJECT pointer
        status = fifoSearchObject(pDev,
                                  pDacCrtcInfo->ObjectHandle,
                                  pDacCrtcInfo->ChID,
                                  &Object);
        if (status != RM_OK) {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,
                                   "NVRM: VBlankUpdateImageOffsetFormat: bad object handle ",
                                    pDacCrtcInfo->ObjectHandle);
            DBG_BREAKPOINT();
            pDacCrtcInfo->UpdateFlags &= ~UPDATE_HWINFO_DAC_IMAGE_OFFSET_FORMAT;
            return;
        }

        // Verify the method offsets are the same for these VIDEO_LUT_CURSOR_DAC classes.
        RM_ASSERT((NV046_SET_PAN_OFFSET == NV049_SET_PAN_OFFSET) &&
                  (NV049_SET_PAN_OFFSET == NV067_SET_PAN_OFFSET) &&
                  (NV067_SET_PAN_OFFSET == NV07C_SET_PAN_OFFSET));

        // Call the SET_IMAGE_OFFSET method
        status = classSoftwareMethod(pDev, Object,
                                     NV07C_SET_PAN_OFFSET, 
                                     pDacCrtcInfo->ImageOffset);
        if (status != RM_OK) {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: VBlankUpdateImageOffsetFormat: method failed");
            DBG_BREAKPOINT();
            pDacCrtcInfo->UpdateFlags &= ~UPDATE_HWINFO_DAC_IMAGE_OFFSET_FORMAT;
            return;
        }

        // Verify the method offsets are the same for these VIDEO_LUT_CURSOR_DAC classes.
        RM_ASSERT((NV046_SET_IMAGE_FORMAT(0) == NV049_SET_IMAGE_FORMAT(0)) &&
                  (NV049_SET_IMAGE_FORMAT(0) == NV067_SET_IMAGE_FORMAT(0)) &&
                  (NV067_SET_IMAGE_FORMAT(0) == NV07C_SET_IMAGE_FORMAT(0)));

        // Call the SET_IMAGE_FORMAT method
        status = classSoftwareMethod(pDev, Object,
                                     NV046_SET_IMAGE_FORMAT(0),    
                                     pDacCrtcInfo->ImageFormat);
        if (status != RM_OK) {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: VBlankUpdateImageOffsetFormat: method failed");
            DBG_BREAKPOINT();
            pDacCrtcInfo->UpdateFlags &= ~UPDATE_HWINFO_DAC_IMAGE_OFFSET_FORMAT;
            return;
        }

        pDacCrtcInfo->UpdateFlags &= ~UPDATE_HWINFO_DAC_IMAGE_OFFSET_FORMAT;
    }
}


//Check for special reasons we may wish to suppress calls to hotkeyAddEvent.
// Returns FALSE if the event should be discarded.
// Intention is for a passed event or status value of '0' to potentially be special,
// meaning that info is not available or not applicable.
BOOL hotkeyValidateEvent(PHWINFO pDev, U032 event, U032 status)
{

    switch (event) {
    case NV_HOTKEY_EVENT_DISPLAY_ENABLE:
        if (pDev->Dac.DeviceProperties.Spanning) {
            if (pDev->Power.MobileOperation && (pDev->Power.MobileOperation != 1)) { //For any laptop OEM other than Dell
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: hotkeyAddEvent: Suppressing event due to extended mode.\n\r");
                return FALSE;
            }
        }
//Fall through to default case.
    default:
        return TRUE;
        break;
    }
}


//Called to place an event into the queue polled by QuickTweak/NVSVC.  These events
// currently include only requests for display changes and notifications of BIOS
// changes to the flat panel center/scaling state.  This display change mechanism,
// first put in place for hotkeys, is also used to spoof a hotkey-like transition
// when a low level event requires a device change (e.g., a laptop set to use the
// CRT resumes from hibernation without the CRT connected).
VOID hotkeyAddEvent(PHWINFO pDev, U032 event, U032 status)
{
    U032 temp;

    // Don't add another event if we're still in a modeset.
    // Added to fix bug 32596.
    if (pDev->Dac.DisplayChangeInProgress)
        return;

    //Reformat the status parameter for a display switch to reflect
    // which is primary.  The current rule, specified by Toshiba and
    // hopefully accepted by Dell, is that if the LCD is among the
    // devices, it's always the primary.
    //The format is byte0 = primary device, byte1 = secondary, etc., up to 4 devices.
    if (event == NV_HOTKEY_EVENT_DISPLAY_ENABLE) {
        U032 oldstat, i;
        U032 prioritylist[] = {
            NV_HOTKEY_STATUS_DISPLAY_ENABLE_TV,    //lowest priority to be primary
            NV_HOTKEY_STATUS_DISPLAY_ENABLE_DFP,
            NV_HOTKEY_STATUS_DISPLAY_ENABLE_CRT,
            NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD    //highest priority to be primary
        };

        oldstat = status;
        status = 0;
        for (i = 0; i < 4; i++) {
            if (oldstat & prioritylist[i]) {
                status <<= 8;
                status |= prioritylist[i];
            }
        }
    }

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: hotkeyAddEvent: Adding event:", event);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: hotkeyAddEvent:       status:", status);

    if (!hotkeyValidateEvent(pDev, event, status)) return;

    temp = (pDev->HotKeys.queueHead + 1) % NV_HOTKEY_EVENT_QUEUE_SIZE;

    //Don't wrap past tail if there's a backlog.  It may actually
    // be better to keep the newer events rather than the older ones,
    // but that raises queue access coherency issues.  This is a
    // safe way to handle an event that should never happen anyway.
    if (temp == pDev->HotKeys.queueTail) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: hotkeyAddEvent: Queue is full.\n\r");
        return;
    }

    pDev->HotKeys.Queue[pDev->HotKeys.queueHead].event = event;
    pDev->HotKeys.Queue[pDev->HotKeys.queueHead].status = status;
    pDev->HotKeys.queueHead = temp;

#if (defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x500) && defined(NTRM)) || defined(LINUX)
    //Send a notification that an event was added.  Currently only implemented for NT4
    //and linux.
    RmSignalHotKeyEvent();
#endif
}


//Mobile system feature: look for bits in our CR registers that indicate
// a keyboard Fn + key event has occured.  If so, we queue up the event
// so it can be serviced later.
VOID VBlankCheckHotKeys(PHWINFO pDev)
{
    U008 temp, newEvents, stateReg; //, stateChanges;
    U008 lock, head;
    U032 centering;

    //Workaround: loop over both heads.
    for (head = 0; head <= 1; head++) 
    {

    lock = UnlockCRTC(pDev, head);
    
    CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, temp, head);
    newEvents = temp & (NV_MOBILE_FLAG_SCALE_MASK | NV_MOBILE_FLAG_DISPLAY_MASK);    //Only handle events we know about.

    CRTC_RD(NV_CIO_CRE_SCRATCH7__INDEX, stateReg, head);

    //DEBUG
    if (newEvents) {

        //Leave this printf for current debug purposes -- hotkeys are young yet.
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Head:", head);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR4A register is now:", temp);
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, temp, head);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR4B register is now:", temp);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR4C register is now:", stateReg);
        CRTC_RD(0x3B, temp, head);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR3B register is now:", temp);
//        CRTC_RD(0x3B, temp, head^1);
//        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Other CR3B register is now:", temp);

//        CRTC_RD(0x3B, temp, 1);
//        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: CR3B/1 register is now:", temp);
    }
    //DEBUG

    //Check if anything has changed:
    //Pressing a key may generate more than one event (e.g., disabling one display and enabling another)

    if (newEvents & NV_MOBILE_FLAG_DISPLAY_MASK) {

        temp = 0;
        if (stateReg & NV_MOBILE_DISPLAY_LCD_MASK) {
            temp |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD;
        }
        if (stateReg & NV_MOBILE_DISPLAY_CRT_MASK) {
            temp |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_CRT;
        }
        if (stateReg & NV_MOBILE_DISPLAY_TV_MASK) {
            temp |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_TV;
        }
        if (stateReg & NV_MOBILE_DISPLAY_DFP_MASK) {
            temp |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_DFP;
        }

        //LPL: I'm seeing some problems with the screen never unblanking
        // with the code below enabled.  There are probably other causes,
        // but in general I'm not comfortable with the idea of blanking the
        // display here because we don't know whether Quicktweak will actually
        // choose to perform the requested action.  If it doesn't, we'll
        // be stuck with the display off.  Unfortunately, not blanking here
        // does allow us to see the backlight turn on and off during the
        // display switch.
#if 0//!defined(MACOS) && !defined(NTRM) 
        // 
        // If we're transitioning to the LCD, turnoff the backlight. This is
        // not included for NT, since these VideoPort calls can't be made from
        // an ISR.
        //
        if (temp & NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD)
        {
            dacBacklightOffMobilePanel(pDev);
        }
#endif

//            if (stateChanges & (NV_MOBILE_DISPLAY_LCD_MASK | NV_MOBILE_DISPLAY_CRT_MASK
//                | NV_MOBILE_DISPLAY_TV_MASK | NV_MOBILE_DISPLAY_DFP_MASK )) {
            hotkeyAddEvent(pDev, NV_HOTKEY_EVENT_DISPLAY_ENABLE, temp);
//            }
    }

    if (newEvents & NV_MOBILE_FLAG_SCALE_MASK) {
//            if (stateChanges & NV_MOBILE_DISPLAY_SCALE_MASK) {
        centering = REG_RD_DRF(_PRAMDAC, _FP_TG_CONTROL, _MODE);
        if (centering == NV_PRAMDAC_FP_TG_CONTROL_MODE_SCALE) temp = NV_HOTKEY_STATUS_DISPLAY_SCALED;
        else if (centering == NV_PRAMDAC_FP_TG_CONTROL_MODE_CENTER) temp = NV_HOTKEY_STATUS_DISPLAY_CENTERED;
        else temp = NV_HOTKEY_STATUS_DISPLAY_NATIVE;
//                hotkeyAddEvent(pDev, NV_HOTKEY_EVENT_DISPLAY_CENTERING, (stateReg & NV_MOBILE_DISPLAY_SCALE_MASK)?1:0);
        hotkeyAddEvent(pDev, NV_HOTKEY_EVENT_DISPLAY_CENTERING, temp);
//            }

        if (temp == NV_HOTKEY_STATUS_DISPLAY_SCALED) temp = NV_CFGEX_GET_FLATPANEL_INFO_SCALED;
        else if (temp == NV_HOTKEY_STATUS_DISPLAY_CENTERED) temp = NV_CFGEX_GET_FLATPANEL_INFO_CENTERED;
        else if (temp == NV_HOTKEY_STATUS_DISPLAY_NATIVE) temp = NV_CFGEX_GET_FLATPANEL_INFO_NATIVE;
        //
        // XXXscottl:
        // The following is a no-no on win2k (can't use the underlying
        // RtlWriteRegistryValue at raised irql).  The current feeling is
        // that we really don't need to update the registry for this any
        // longer anyway, so we'll comment this out.
        //
        //osWriteRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplayNumber, strFpMode, temp);
    }

        //Now reset the event flags we've handled.  We do this after we've actually handled
        // the events (not explicitly required at this point, but seems cleanest) and in the
        // smallest possible amount of time between re-reading the register and writing it.
    if (newEvents) {
//            pDev->HotKeys.reg4CShadow = (U032)stateReg;            //Update our shadow register

        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, temp, head);
        temp &= ~newEvents;    //Reset the events we will handle.
            // LPL: possible race here -- another SSF bit gets set during our read-mod-write window,
            // and is cleared by us unintentionally (that's why this window is made intentionally
            // as small as possible by re-reading the register)
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, temp, head);
    }

    // Restore lock state
    RestoreLock(pDev, head, lock);

    }   //workaround: loop over both heads

}

//
// Service vertical blank interrupt.
//
VOID VBlank
(
    PHWINFO pDev
)
{
    PDACCRTCINFO pDacCrtcInfo;
    U032 pmc, pending;
    U032 head;
    U032 rmFinish;
    U008 CrtcIndex, CrtcIndex2 = 0;
    U032 MirrorEnabled = 0;

#ifdef RM_STATS
    pDev->Framebuffer.ServiceCount++;
#endif
    rmFinish = FALSE;

    NVRM_TRACE('VBLK');

    //
    // Save the CRTC index reg, since we may have interrupted someone doing
    // an access (CR44 could be pointing at either head, so save both heads).
    //
    CrtcIndex  = DAC_REG_RD08(NV_PRMCIO_CRX__COLOR, 0);
    CrtcIndex2 = 0;
    if (pDev->Dac.HalInfo.NumCrtcs == 2)
        CrtcIndex2 = DAC_REG_RD08(NV_PRMCIO_CRX__COLOR, 1);

    //
    // Disable I/O broadcast mode.  There are bugs in NV11's use of broadcast
    // mode where we can't reach HeadB through privSpace with this enabled.
    // It's possible that we've gotten an Int10 interrupt that enables
    // Broadcast mode before we've gotten the rmEnableVGA call.  We need
    // to disable this when servicing the interrupts, because these functions
    // need to access both Head A and Head B through privSpace.
    // We disable this also in mcService, but since VBlank gets called outside
    // of mcService as well, I decided to correct this one individually as well.
    //
    // Chips after NV11 have this fixed.
    //
    if (IsNV11(pDev) && !pDev->Vga.Enabled)
    {
        // store broadcast support
        MirrorEnabled = REG_RD_DRF(_PBUS, _DEBUG_1, _DISP_MIRROR);

        // disable broadcast support
        FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _DISABLE);
    }

    //
    // We're here because at least one of the PCRTC bits is pending.
    // Read the NV_PMC_INTR_0 register to figure out which one(s).
    //
    pending = 0;
    pmc = REG_RD32(NV_PMC_INTR_0);
    if (pmc & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING))
        pending |= 0x1;
    if (pmc & DRF_DEF(_PMC, _INTR_0, _PCRTC2, _PENDING))
        pending |= 0x2;

    for (head = 0; head < MAX_CRTCS; head++)
    {
        // Move on if this crtc's interrupt isn't pending...
        if ((pending & (1 <<  head)) == 0)
            continue;

        // Scanout address updates...
        //
        // Since class07c's SET_IMAGE_FORMAT queues a VBlank callback (which we'll
        // handle "immediately" in VBlankProcessCallbacks below) and has an ASSERT
        // on the VBlankCounter value, we'll handle this update before the counter
        // is incremented.
        //
        VBlankUpdateImageOffsetFormat(pDev, head);

        // This head needs servicing...
        pDacCrtcInfo = &pDev->Dac.CrtcInfo[head];

        pDacCrtcInfo->VBlankToggle = 0;
        pDacCrtcInfo->VBlankCounter++;
        pDacCrtcInfo->IsVBlank = TRUE;

        //
        // Head 0 still gets unique treatment for things driven
        // by the older NV_VIDE0_FROM_MEMORY class and family.
        //
        if (head == 0)
        {
            rmFinish |= VBlankUpdateFlip(pDev, head);
            class63VBlank(pDev);
            VBlankUpdatePalette(pDev, head);
            rmFinish |= VBlankUpdateVideoScaler(pDev, head);
        }
#ifdef KDA_BUFFER_ENABLED
    //LPL: narrow this to just around cursor, if possible (was: bail on whole vblank routine)
    //Check if the VBIOS is overriding the HW cursor.  If not,
    // try to take the buffer semaphore to lock out the VBIOS
    // while we perform cursor updates.  Note that the flag being
    // checked here is first the general KDA semaphore, then the one specific
    // to the cursor area.  See fb.c for more detail on the KDA buffer.
        if ((fbKDATakeBufferSemaphore(pDev, 0) == RM_OK) && (pDev->DBfbKDASharedMem)) {
            U032 cellOffset = 0;
            if (fbKDAFindCursorCellOffset(pDev, head, &cellOffset) == RM_OK) {
                if (fbKDATakeCursorCellSemaphore(pDev, cellOffset, 0) == RM_OK) {
                // Cursor updates...
//                    if (pDev->DBfbKDACursorOverride) {
//                        SignalIconEndEvent();
//                        pDev->DBfbKDACursorOverride = FALSE;
//                    }
                    VBlankUpdateCursor(pDev, head);
                    fbKDAReleaseCursorCellSemaphore(pDev, cellOffset);
                } else {
//                    if (!pDev->DBfbKDACursorOverride) {
//                        SignalIconBeginEvent();
//                        pDev->DBfbKDACursorOverride = TRUE;
//                    }
                }
            } else {
                VBlankUpdateCursor(pDev, head);
            }
            fbKDAReleaseBufferSemaphore(pDev);
        } else {
            VBlankUpdateCursor(pDev, head);
        }
#else   //#ifdef KDA_BUFFER_ENABLED
        // Cursor updates...
        VBlankUpdateCursor(pDev, head);
#endif  //#ifdef KDA_BUFFER_ENABLED... else

        // Callback list...
        VBlankProcessCallbacks(pDev, head);

        pDacCrtcInfo->IsVBlank = FALSE;
        
        //
        // update the dither pattern if needed
        //
        if (pDev->Dac.HalInfo.CrtcInfo[head].fpMode == NV_CFGEX_GET_FLATPANEL_INFO_CENTERED_DITHER)
        {    
            VBlankUpdateDither(pDev,head);
        }
    }

    if (pDev->Power.MobileOperation && pDev->HotKeys.enabled)
        VBlankCheckHotKeys(pDev);

    //
    // Update service count if no RM processing required (RM will increment this in mcService).
    //
    if (!rmFinish)
    {
#ifdef RM_STATS                    
        pDev->Chip.ServiceCount++;
#endif
        // reset the VBlank intrs we've handled
        if (pending & 0x1)
            DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_0, _VBLANK, _RESET, 0);

        if (pending & 0x2)
            DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_0, _VBLANK, _RESET, 1);
    }

    // Restore Broadcast support if needed
    if (IsNV11(pDev) && !(pDev->Vga.Enabled) && MirrorEnabled)
    {
        FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _ENABLE);
    }
    
    // restore crtc index for both heads
    DAC_REG_WR08(NV_PRMCIO_CRX__COLOR, CrtcIndex, 0);
    // Don't restore Head B if broadcast is enabled, since this doesn't work
    if ((pDev->Dac.HalInfo.NumCrtcs == 2) && (!MirrorEnabled))
        DAC_REG_WR08(NV_PRMCIO_CRX__COLOR, CrtcIndex2, 1);

    
    NVRM_TRACE('vblk');
}

//
// Return bitmask representing heads with pending
// vblank interrupts.
//
U032
VBlankPending
(
    PHWINFO pDev
)
{
    U032 pending = 0;

    //
    // TODO: call into HAL to get these.
    //
    if (REG_RD32(NV_PMC_INTR_0) & (DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING)))
        pending |= 0x1;
    if (REG_RD32(NV_PMC_INTR_0) & (DRF_DEF(_PMC, _INTR_0, _PCRTC2, _PENDING)))
        pending |= 0x2;

    return pending;
}

void VBlankUpdateDither
(
    PHWINFO pDev,
    U032 Head
)
{
    U032 CurrentDitherPattern;
    U032 CurrentPLLCompatReg;
       
    // need to enable the dithering?
    // We wait 60 ticks before enabling to reduce change of weird visual
    // effects during startup.

    pDev->Dac.CrtcInfo[Head].VBlankTemporalDitherSafetyCounter++;

    if (pDev->Dac.CrtcInfo[Head].VBlankTemporalDitherSafetyCounter == 60)
    {
        // make sure we're in the correct HW cursor state for dithering
        DAC_FLD_WR_DRF_DEF(_PCRTC, _CURSOR_CONFIG, _CUR_BPP,   _32,   Head);
        DAC_FLD_WR_DRF_DEF(_PCRTC, _CURSOR_CONFIG, _CUR_BLEND, _ROP,  Head);
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIPE, _LONG, Head);

        //In the case where we are using a sw cursor the CrtcInfo structure
        //will not have been initialized. Thus things like cursor color 
        //format and cursor width/height will still be zero'd out. 
        //Incorrect color format can cause screen color corruption when
        //dithering is turned on.
        pDev->Dac.CrtcInfo[Head].CursorColorFormat = NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8;

        // enable dithering
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _6B_DITHER, _ON,     Head);
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _DITHER_RB, _NORMAL, Head);
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _DITHER_G, _NORMAL,  Head);
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COMPAT, _DITHER_Y, _NORMAL,  Head);
        
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Enabled dithering at vblank.\n");
    }
    else if (pDev->Dac.CrtcInfo[Head].VBlankTemporalDitherSafetyCounter > 60)
    {
#ifdef MACOS
        // we have next pattern, need to save it (quicky value lookup courtesy tb :-)
        CurrentDitherPattern = "\001\002\005\006"[(pDev->Dac.CrtcInfo[Head].VBlankCounter)&3];
#else
        // we have next pattern, need to save it (quicky value lookup courtesy tb :-)
        CurrentDitherPattern = "\002\002\002\002"[(pDev->Dac.CrtcInfo[Head].VBlankCounter)&3];
#endif
        
        // get current dither pattern.
        CurrentPLLCompatReg = DAC_REG_RD32(NV_PRAMDAC_PLL_COMPAT, Head);

        //clear the current bit pattern in PLL reg value
        // DITHER_RB bit is the beginning of 3 bit dither pattern
        CurrentPLLCompatReg = (~(0x7 << (0?NV_PRAMDAC_PLL_COMPAT_DITHER_RB))) &
                                 CurrentPLLCompatReg;

        //OR in the pattern
        CurrentPLLCompatReg = CurrentPLLCompatReg |
                                 (CurrentDitherPattern << (0?NV_PRAMDAC_PLL_COMPAT_DITHER_RB));

        //write pattern
        DAC_REG_WR32(NV_PRAMDAC_PLL_COMPAT, CurrentPLLCompatReg, Head);
    }
}
