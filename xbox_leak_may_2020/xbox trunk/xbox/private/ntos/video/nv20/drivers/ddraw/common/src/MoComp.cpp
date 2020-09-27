 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

/*==========================================================================;
 *
 *  Copyright (C) 2000 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       MoComp.cpp
 *  Content:    Windows98 DirectDraw 32 bit driver
 *
 ***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

#include "dxshare.h"

// For DirectX VA definitions.
// Note: Initguid.h causes Guids defined in Dxvadefs.h to be created as global data.
#include "initguid.h"
#include "dxvadefs.h"


extern NVMOCOMPPARAMS           nvMoCompParams;

#ifndef WINNT
    extern DISPDRVDIRECTXCOMMON    *pDXShare;
#endif // WINNT

extern DWORD __stdcall nvInitCelsiusForMoComp(void);
extern DWORD __stdcall nvInitCelsiusForMoCompPrediction(void);
extern DWORD __stdcall nvInitCelsiusForMoCompCorrection();
extern DWORD __stdcall nvInitCelsiusForMoCompConversion(void);
extern DWORD __stdcall nvInitCelsiusForMoCompTemporalFilter(void);
extern DWORD __stdcall nvMoCompProcessCelsiusFrameMacroblocks(LPNVDECODEMACROBLOCK pMacroblocks,
                                                              DWORD numMacroblocks, BYTE formatConversionStatus);
extern DWORD __stdcall nvMoCompProcessCelsiusFieldMacroblocks(LPNVDECODEMACROBLOCK pMacroblocks,
                                                              DWORD numMacroblocks, BYTE formatConversionStatus);
extern DWORD __stdcall nvInitKelvinForMoComp(void);
extern DWORD __stdcall nvInitKelvinForMoCompPrediction(void);
extern DWORD __stdcall nvInitKelvinForMoCompCorrection();
extern DWORD __stdcall nvInitKelvinForMoCompConversion(void);
extern DWORD __stdcall nvInitKelvinForMoCompTemporalFilter(void);
extern DWORD __stdcall nvMoCompProcessKelvinFrameMacroblocks(LPNVDECODEMACROBLOCK pMacroblocks,
                                                              DWORD numMacroblocks, BYTE formatConversionStatus);
extern DWORD __stdcall nvMoCompProcessKelvinFieldMacroblocks(LPNVDECODEMACROBLOCK pMacroblocks,
                                                              DWORD numMacroblocks, BYTE formatConversionStatus);

PNVMCPREDFUNC nvDoBidirectionalFieldPrediction;
PNVMCPREDFUNC nvDoTwoBidirectionalFieldPredictions;
PNVMCPREDFUNC nvDoForwardFieldPrediction;
PNVMCPREDFUNC nvDoTwoForwardFieldPredictions;
PNVMCPREDFUNC nvDoBackwardFieldPrediction;
PNVMCPREDFUNC nvDoTwoBackwardFieldPredictions;

PNVMCPREDFUNC2 nvPredictFrameLumaMacroblock;
PNVMCPREDFUNC2 nvPredictFieldLumaMacroblock;
PNVMCPREDFUNC2 nvPredictFrameChromaMacroblock;
PNVMCPREDFUNC2 nvPredictFieldChromaMacroblock;

PNVMCCORRFUNC2 nvDoAllIntraFieldCorrections;
PNVMCCORRFUNC2 nvDoAllFieldCorrections;
PNVMCCORRFUNC nvDoAllIntraFrameCorrections;
PNVMCCORRFUNC nvDoAllFrameCorrections;
PNVMCCORRFUNC nvDoIntraFieldCorrection;
PNVMCCORRFUNC nvDoOptimizedIntraFieldCorrection;
PNVMCCORRFUNC nvDoFieldCorrection;
PNVMCCORRFUNC nvDoIntraFrameCorrection;
PNVMCCORRFUNC nvDoFrameCorrection;

#ifdef  DEBUG
unsigned long overlaySurfaceBase = 0;
unsigned long moCompDebugLevel = 0;
#endif  // DEBUG

/*
 * nvMoCompCelsiusConvertSurfaceFormat
 *
 * Performs format conversion from our motion comp surface format to an overlay
 * compatible surface format.
 *
 * Note: Destination surface coordinates are referenced in quarter pel units.
 *       Texture coordinates are referenced in pel units.
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompCelsiusConvertSurfaceFormat(DWORD fpVidMem, BOOL filterChroma, DWORD dwHowMuchToConvert)
{
    LPDDRAWI_DDRAWSURFACE_LCL   dstx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   dst = dstx->lpGbl;
    LPNVMCSURFACEFLAGS          lpDstSurfaceFlags;
    unsigned long               surfaceWidth = dst->wWidth;
    unsigned long               surfaceHeight = dst->wHeight;
    unsigned long               surfacePitch = dst->lPitch;
    unsigned long               dwDstSurfaceOffset;
    unsigned long               tmpVal;
    unsigned long               Y0 = 0;
    unsigned long               Y1 = surfaceHeight;
    long                        dstX0;
    long                        dstX1;
    BYTE                        updateReferenceCount = FALSE;
    BYTE                        frameType;

    if (pDriverData->dwMostRecentHWUser != MODULE_ID_DDRAW) {
        if (!(nvInitCelsiusForMoComp()))
            return FALSE;
    }

    // Let the display code know how to display this surface
    lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

    while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpDstSurfaceFlags->dwMCSurfaceBase != fpVidMem))
        lpDstSurfaceFlags++;

    if (!(nvInitCelsiusForMoCompConversion()))
        return FALSE;

    frameType = lpDstSurfaceFlags->bMCPictureStructure;

    tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR << 28) |
             (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR << 24);

    if (pDriverData->bMCOverlaySrcIsSingleField1080i) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_FILTER(0) | 0x40000);
        nvPushData(1, tmpVal);

        nvPusherAdjust(2);
    }

    if (filterChroma) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_FILTER(1) | 0x40000);
        nvPushData(1, tmpVal);

        nvPusherAdjust(2);
    }

    // 420 LUMA portion of this surface starts at fpVidMem
    dwDstSurfaceOffset = VIDMEM_OFFSET(fpVidMem);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
    if (frameType == PICTURE_STRUCTURE_BOTTOM_FIELD)
        nvPushData(1, (dwDstSurfaceOffset + surfacePitch));
    else
        nvPushData(1, dwDstSurfaceOffset);

    // 420 CHROMA portion of this surface starts here
    dwDstSurfaceOffset += (surfacePitch * dst->wHeight);

    if (frameType == PICTURE_STRUCTURE_BOTTOM_FIELD)
        nvPushData(2, (dwDstSurfaceOffset + surfacePitch));
    else
        nvPushData(2, dwDstSurfaceOffset);

    nvPusherAdjust(3);

    // 422 portion of this surface starts here
    dwDstSurfaceOffset = lpDstSurfaceFlags->dwMCSurface422Offset;

    if (frameType == PICTURE_STRUCTURE_FRAME) {

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwDstSurfaceOffset);

        nvPusherAdjust(4);

        if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 7)
            Y0 = (surfaceHeight >> 1) + (surfaceHeight >> 2);
        else if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 3)
            Y0 = (surfaceHeight >> 1);
        else if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 1)
            Y0 = (surfaceHeight >> 2);

        if (dwHowMuchToConvert == 7)
            Y1 = (surfaceHeight >> 1) + (surfaceHeight >> 2);
        else if (dwHowMuchToConvert == 3)
            Y1 = (surfaceHeight >> 1);
        else if (dwHowMuchToConvert == 1)
            Y1 = (surfaceHeight >> 2);

        if (dwHowMuchToConvert == 15)
            updateReferenceCount = TRUE;

    } else {

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
        nvPushData(1, surfacePitch << 17);
        nvPushData(2, surfacePitch << 17);

        surfacePitch = ((surfaceWidth + 3) & ~3);

        // Determine pitch of 422 pixel block
        surfacePitch = ((surfacePitch << 1) + 127) & ~127; // YUY2 destination is twice the surface width

        nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(4, 0);

        nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);

        if (frameType == PICTURE_STRUCTURE_BOTTOM_FIELD)
            nvPushData(6, (dwDstSurfaceOffset + surfacePitch));
        else
            nvPushData(6, dwDstSurfaceOffset);

        surfacePitch |= (surfacePitch << 16);
        surfacePitch <<= 1;

        nvPushData(7, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_PITCH | 0x40000);
        nvPushData(8, surfacePitch);

        nvPusherAdjust(9);

        if (frameType == PICTURE_STRUCTURE_TOP_FIELD) {
            if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted & 1)
                Y0 = (surfaceHeight >> 1);

            if ((dwHowMuchToConvert & 3) != 3)
                Y1 = (surfaceHeight >> 1);
        } else {
            if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted & 4)
                Y0 = (surfaceHeight >> 1);

            if ((dwHowMuchToConvert & 12) != 12)
                Y1 = (surfaceHeight >> 1);
        }

        if ((frameType == PICTURE_STRUCTURE_TOP_FIELD) &&
            (dwHowMuchToConvert == 3))
            updateReferenceCount = TRUE;
        else if ((frameType == PICTURE_STRUCTURE_BOTTOM_FIELD) &&
            (dwHowMuchToConvert == 12))
            updateReferenceCount = TRUE;
    }

    SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    if (pDriverData->bMCHorizontallyDownscale1080i) {
        dstX0 = (0 - SCALED_CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF;
        dstX1 = ((surfaceWidth << 1) - SCALED_CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF;
    } else {
        dstX0 = (0 - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF;
        dstX1 = ((surfaceWidth << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF;
    }

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) | NV056_SET_SURFACE_CLIP_HORIZONTAL | 0x40000);
    nvPushData(1, ((surfacePitch << 17) | 0));
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) | NV056_SET_SURFACE_CLIP_VERTICAL | 0x40000);
    nvPushData(3, ((surfaceHeight << 17) | 0));
    nvPusherAdjust(4);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    if (frameType == PICTURE_STRUCTURE_FRAME) {
        DWORD dstYScale = 18;
        
        if (pDriverData->bMCOverlaySrcIsSingleField1080i)
            dstYScale = 17;

        nvPushData(3, ((Y0 >> 1) << 16) | 0);
        nvPushData(4, (Y0 << 16) | 0);
        nvPushData(5, (Y0 << dstYScale) | dstX0);

        nvPushData(6, ((Y1 >> 1) << 16) | 0); // CHROMA texture is in pel units and is only half the surface height
        nvPushData(7, (Y1 << 16) | 0); // LUMA texture is in pel units
        nvPushData(8, (Y1 << dstYScale) | dstX0); // destination is in quarter pel units

        nvPushData(9, ((Y1 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(10, (Y1 << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(11, (Y1 << dstYScale) | dstX1); // each 32 bit write contains data for 2 422 pixels

        nvPushData(12, ((Y0 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(13, (Y0 << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(14, (Y0 << dstYScale) | dstX1);
    } else {
        nvPushData(3, ((Y0 >> 2) << 16) | 0);
        nvPushData(4, ((Y0 >> 1) << 16) | 0);
        nvPushData(5, (Y0 << 17) | dstX0);

        nvPushData(6, ((Y1 >> 2) << 16) | 0); // CHROMA texture is in pel units and is only half the surface height
        nvPushData(7, ((Y1 >> 1) << 16) | 0); // LUMA texture is in pel units
        nvPushData(8, (Y1 << 17) | dstX0); // destination is in quarter pel units

        nvPushData(9, ((Y1 >> 2) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(10, ((Y1 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(11, (Y1 << 17) | dstX1); // each 32 bit write contains data for 2 422 pixels

        nvPushData(12, ((Y0 >> 2) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(13, ((Y0 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(14, (Y0 << 17) | dstX1);
    }

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

    if (updateReferenceCount) {
        // Update reference count so we know when we can display this surface
        lpDstSurfaceFlags->dwMCFormatCnvReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);
    }

    nvPusherStart(TRUE);

    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted |= dwHowMuchToConvert;

    return TRUE;

} /* nvMoCompCelsiusConvertSurfaceFormat */


/*
 * nvMoCompCelsiusTemporalFilter
 *
 * Performs intra frame temporal filtering between the fields of a post format converted overlay surface
 *
 * Note: Destination surface coordinates are referenced in quarter pel units.
 *       Texture coordinates are referenced in quarter pel units.
 *       The combination factor should be in the range of 0 (0%) - 100 (100%)
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompCelsiusTemporalFilter(DWORD fpVidMem, BYTE combinationFactor)
{
    LPDDRAWI_DDRAWSURFACE_LCL   dstx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   dst = dstx->lpGbl;
    unsigned long               surfaceWidth = dst->wWidth;
    unsigned long               surfaceHeight = dst->wHeight;
    unsigned long               surfacePitch = dst->lPitch;
    unsigned long               dwSrcOffset;
    unsigned long               dwDstOffset;
    unsigned long               pitch422;
    unsigned long               topFieldCombinationFactor;
    unsigned long               bottomFieldCombinationFactor;
    LPNVMCSURFACEFLAGS          lpDstSurfaceFlags;
    long                        dstX0;
    long                        dstX1;

    // We currently don't allocate filtering memory for large surfaces so we don't allow filtering on them
    if (dst->wWidth > 1024)
        return DDERR_GENERIC;

    lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

    while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpDstSurfaceFlags->dwMCSurfaceBase != fpVidMem))
        lpDstSurfaceFlags++;

    // 422 portion of this surface starts here
    dwSrcOffset = lpDstSurfaceFlags->dwMCSurface422Offset;

    // 422 filtering portion of this surface starts here
    pitch422 = (dst->wWidth + 3) & ~3;
    pitch422 = ((pitch422 << 1) + 127) & ~127;
    dwDstOffset = dwSrcOffset + (pitch422 * (DWORD)dst->wHeight);   // 422 filtering portion

    // Make sure surface has been format converted
    if ((lpDstSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) &&
        (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
        nvMoCompConvertSurfaceFormat(fpVidMem, TRUE, 15);
    else {
        if ((lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3)
            nvMoCompConvertSurfaceFormat(fpVidMem, TRUE, 3);
        if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted < 12)
            nvMoCompConvertSurfaceFormat(fpVidMem, TRUE, 12);
    }

    // If this surface has already been filtered then just return
    if (lpDstSurfaceFlags->bMCFrameIsFiltered == TRUE)
        return DD_OK;

    if (pDriverData->dwMostRecentHWUser != MODULE_ID_DDRAW) {
        if (!(nvInitCelsiusForMoComp()))
            return DDERR_GENERIC;
    }

    if (!(nvInitCelsiusForMoCompTemporalFilter()))
        return DDERR_GENERIC;

    if (combinationFactor > 100)
        return DDERR_INVALIDPARAMS;

    topFieldCombinationFactor = (combinationFactor * 256) / 100;
    if (topFieldCombinationFactor > 255)
        topFieldCombinationFactor = 255;
    topFieldCombinationFactor |= topFieldCombinationFactor << 8;
    topFieldCombinationFactor |= topFieldCombinationFactor << 16;

    bottomFieldCombinationFactor = 255 - topFieldCombinationFactor;
    bottomFieldCombinationFactor |= bottomFieldCombinationFactor << 8;
    bottomFieldCombinationFactor |= bottomFieldCombinationFactor << 16;

    // Load combination factors
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    nvPushData(1, topFieldCombinationFactor); // % of top field
    nvPushData(2, bottomFieldCombinationFactor); // % of bottom field

    nvPusherAdjust(3);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(0) | 0x80000);

    nvPushData(1, dwSrcOffset);
    nvPushData(2, (dwSrcOffset + pitch422));

    lpDstSurfaceFlags->bMCFrameIsFiltered = TRUE;

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_NO_OPERATION | 0x40000);
    nvPushData(4, 0);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);

    nvPushData(6, dwDstOffset);

    nvPusherAdjust(7);

    SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    // Filter Top Field

    if (pDriverData->bMCHorizontallyDownscale1080i) {
        dstX0 = (0 - SCALED_CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF;
        dstX1 = ((surfaceWidth << 1) - SCALED_CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF;
    } else {
        dstX0 = (0 - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF;
        dstX1 = ((surfaceWidth << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF;
    }

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushData(3, (0 << 16) | 0); // Odd field
    nvPushData(4, (0 << 16) | 0);
    nvPushData(5, (0 << 16) | dstX0);

    nvPushData(6, (surfaceHeight << 17) | 0);
    nvPushData(7, (surfaceHeight << 17) | 0);
    nvPushData(8, (surfaceHeight << 17) | dstX0);

    nvPushData(9, (surfaceHeight << 17) | (surfaceWidth << 1));
    nvPushData(10, (surfaceHeight << 17) | (surfaceWidth << 1));
    nvPushData(11, (surfaceHeight << 17) | dstX1); // each 32 bit write contains data 2 422 pixels

    nvPushData(12, (0 << 16) | (surfaceWidth << 1));
    nvPushData(13, (0 << 16) | (surfaceWidth << 1));
    nvPushData(14, (0 << 16) | dstX1);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

    // Now reverse combination factors
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    nvPushData(1, bottomFieldCombinationFactor); // % of top field
    nvPushData(2, topFieldCombinationFactor); // % of bottom field

    // Filter Bottom Field

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_NO_OPERATION | 0x40000);
    nvPushData(4, 0);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);

    nvPushData(6, dwDstOffset + pitch422);

    nvPusherAdjust(7);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushData(3, (0 << 16) | 0); // Odd field
    nvPushData(4, (0 << 16) | 0);
    nvPushData(5, (0 << 16) | dstX0);

    nvPushData(6, (surfaceHeight << 17) | 0);
    nvPushData(7, (surfaceHeight << 17) | 0);
    nvPushData(8, (surfaceHeight << 17) | dstX0);

    nvPushData(9, (surfaceHeight << 17) | (surfaceWidth << 1));
    nvPushData(10, (surfaceHeight << 17) | (surfaceWidth << 1));
    nvPushData(11, (surfaceHeight << 17) | dstX1); // each 32 bit write contains data 2 422 pixels

    nvPushData(12, (0 << 16) | (surfaceWidth << 1));
    nvPushData(13, (0 << 16) | (surfaceWidth << 1));
    nvPushData(14, (0 << 16) | dstX1);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

    // Update reference count so we know when we can display this surface
    lpDstSurfaceFlags->dwMCTemporalFilterReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);

    nvPusherStart(TRUE);

    return DD_OK;

} /* nvMoCompCelsiusTemporalFilter */


/*
 * nvMoCompKelvinConvertSurfaceFormat
 *
 * Performs format conversion from our motion comp surface format to an overlay
 * compatible surface format.
 *
 * Note: Destination surface coordinates are referenced in quarter pel units.
 *       Texture coordinates are referenced in pel units.
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompKelvinConvertSurfaceFormat(DWORD fpVidMem, BOOL filterChroma, DWORD dwHowMuchToConvert)
{
    LPDDRAWI_DDRAWSURFACE_LCL   dstx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   dst = dstx->lpGbl;
    LPNVMCSURFACEFLAGS          lpDstSurfaceFlags;
    unsigned long               surfaceWidth = dst->wWidth;
    unsigned long               surfaceHeight = dst->wHeight;
    unsigned long               surfacePitch = dst->lPitch;
    unsigned long               dwDstSurfaceOffset;
    unsigned long               tmpVal;
    unsigned long               Y0 = 0;
    unsigned long               Y1 = surfaceHeight;
    BYTE                        updateReferenceCount = FALSE;
    BYTE                        frameType;

    if (pDriverData->dwMostRecentHWUser != MODULE_ID_DDRAW) {
        if (!(nvInitKelvinForMoComp()))
            return FALSE;
    }

    // Let the display code know how to display this surface
    lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

    while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpDstSurfaceFlags->dwMCSurfaceBase != fpVidMem))
        lpDstSurfaceFlags++;

    if (!(nvInitKelvinForMoCompConversion()))
        return FALSE;

    frameType = lpDstSurfaceFlags->bMCPictureStructure;

    tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
             (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
             (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
             (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
             (NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0 << 24) |
             (NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 << 16) |
             (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

    if (pDriverData->bMCOverlaySrcIsSingleField1080i) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                       NV097_SET_TEXTURE_FILTER(0) | 0x40000);
        nvPushData(1, tmpVal);

        nvPusherAdjust(2);
    }

    if (filterChroma) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                       NV097_SET_TEXTURE_FILTER(1) | 0x40000);
        nvPushData(1, tmpVal);

        nvPusherAdjust(2);
    }

    // 420 LUMA portion of this surface starts at fpVidMem
    dwDstSurfaceOffset = VIDMEM_OFFSET(fpVidMem);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
    if (frameType == PICTURE_STRUCTURE_BOTTOM_FIELD)
        nvPushData(1, (dwDstSurfaceOffset + surfacePitch));
    else
        nvPushData(1, dwDstSurfaceOffset);

    // 420 CHROMA portion of this surface starts here
    dwDstSurfaceOffset += (surfacePitch * dst->wHeight);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
    if (frameType == PICTURE_STRUCTURE_BOTTOM_FIELD)
        nvPushData(3, (dwDstSurfaceOffset + surfacePitch));
    else
        nvPushData(3, dwDstSurfaceOffset);

    nvPusherAdjust(4);

#ifdef  DEBUG
    if ((moCompDebugLevel > 0) && (overlaySurfaceBase != 0)) {
        dwDstSurfaceOffset = VIDMEM_OFFSET(overlaySurfaceBase);
        dwDstSurfaceOffset += (surfacePitch * dst->wHeight);
        lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted = 0;
    }
#endif  // DEBUG

    // 422 portion of this surface starts here
    dwDstSurfaceOffset = lpDstSurfaceFlags->dwMCSurface422Offset;

    if (frameType == PICTURE_STRUCTURE_FRAME) {

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwDstSurfaceOffset);

        nvPusherAdjust(4);

        if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 7)
            Y0 = (surfaceHeight >> 1) + (surfaceHeight >> 2);
        else if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 3)
            Y0 = (surfaceHeight >> 1);
        else if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 1)
            Y0 = (surfaceHeight >> 2);

        if (dwHowMuchToConvert == 7)
            Y1 = (surfaceHeight >> 1) + (surfaceHeight >> 2);
        else if (dwHowMuchToConvert == 3)
            Y1 = (surfaceHeight >> 1);
        else if (dwHowMuchToConvert == 1)
            Y1 = (surfaceHeight >> 2);

        if (dwHowMuchToConvert == 15)
            updateReferenceCount = TRUE;

    } else {

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
        nvPushData(1, surfacePitch << 17);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
        nvPushData(3, surfacePitch << 17);

        surfacePitch = ((surfaceWidth + 3) & ~3);

        // Determine pitch of 422 pixel block
        surfacePitch = ((surfacePitch << 1) + 127) & ~127; // YUY2 destination is twice the surface width

        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_NO_OPERATION | 0x40000);
        nvPushData(5, 0);

        nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);

        if (frameType == PICTURE_STRUCTURE_BOTTOM_FIELD)
            nvPushData(7, (dwDstSurfaceOffset + surfacePitch));
        else
            nvPushData(7, dwDstSurfaceOffset);

        surfacePitch |= (surfacePitch << 16);
        surfacePitch <<= 1;

        nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_PITCH | 0x40000);
        nvPushData(9, surfacePitch);

        nvPusherAdjust(10);

        if (frameType == PICTURE_STRUCTURE_TOP_FIELD) {
            if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted & 1)
                Y0 = (surfaceHeight >> 1);

            if ((dwHowMuchToConvert & 3) != 3)
                Y1 = (surfaceHeight >> 1);
        } else {
            if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted & 4)
                Y0 = (surfaceHeight >> 1);

            if ((dwHowMuchToConvert & 12) != 12)
                Y1 = (surfaceHeight >> 1);
        }

        if ((frameType == PICTURE_STRUCTURE_TOP_FIELD) &&
            (dwHowMuchToConvert == 3))
            updateReferenceCount = TRUE;
        else if ((frameType == PICTURE_STRUCTURE_BOTTOM_FIELD) &&
            (dwHowMuchToConvert == 12))
            updateReferenceCount = TRUE;
    }

    SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40300000);

    if (frameType == PICTURE_STRUCTURE_FRAME) {
        DWORD dstYScale = 18;

        if (pDriverData->bMCOverlaySrcIsSingleField1080i)
            dstYScale = 17;

        nvPushData(3, (Y0 << dstYScale) | 0);
        nvPushData(4, (Y0 << 16) | 0);
        nvPushData(5, ((Y0 >> 1) << 16) | 0);

        nvPushData(6, (Y1 << dstYScale) | 0); // destination is in quarter pel units
        nvPushData(7, (Y1 << 16) | 0); // LUMA texture is in pel units
        nvPushData(8, ((Y1 >> 1) << 16) | 0); // CHROMA texture is in pel units and is only half the surface height

        nvPushData(9, (Y1 << dstYScale) | (surfaceWidth << 1)); // each 32 bit write contains data for 2 422 pixels
        nvPushData(10, (Y1 << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(11, ((Y1 >> 1) << 16) | ((surfaceWidth + 1) >> 1));

        nvPushData(12, (Y0 << dstYScale) | (surfaceWidth << 1));
        nvPushData(13, (Y0 << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(14, ((Y0 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
    } else {
        nvPushData(3, (Y0 << 17) | 0);
        nvPushData(4, ((Y0 >> 1) << 16) | 0);
        nvPushData(5, ((Y0 >> 2) << 16) | 0);

        nvPushData(6, (Y1 << 17) | 0); // destination is in quarter pel units
        nvPushData(7, ((Y1 >> 1) << 16) | 0); // LUMA texture is in pel units
        nvPushData(8, ((Y1 >> 2) << 16) | 0); // CHROMA texture is in pel units and is only half the surface height

        nvPushData(9, (Y1 << 17) | (surfaceWidth << 1)); // each 32 bit write contains data for 2 422 pixels
        nvPushData(10, ((Y1 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(11, ((Y1 >> 2) << 16) | ((surfaceWidth + 1) >> 1));

        nvPushData(12, (Y0 << 17) | (surfaceWidth << 1));
        nvPushData(13, ((Y0 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(14, ((Y0 >> 2) << 16) | ((surfaceWidth + 1) >> 1));
    }

    nvPushData(15, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

    if (updateReferenceCount) {
        // Update reference count so we know when we can display this surface
#ifdef  DEBUG
        if (moCompDebugLevel > 0) {
            lpDstSurfaceFlags->dwMCFormatCnvReferenceCount = getDC()->pRefCount->readShadow();
            getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);
        } else {
            lpDstSurfaceFlags->dwMCFormatCnvReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);
        }
#else   // DEBUG
        lpDstSurfaceFlags->dwMCFormatCnvReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);
#endif  // DEBUG
    }

    nvPusherStart(TRUE);

    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted |= dwHowMuchToConvert;

    return TRUE;

} /* nvMoCompKelvinConvertSurfaceFormat */


/*
 * nvMoCompKelvinTemporalFilter
 *
 * Performs intra frame temporal filtering between the fields of a post format converted overlay surface
 *
 * Note: Destination surface coordinates are referenced in quarter pel units.
 *       Texture coordinates are referenced in quarter pel units.
 *       The combination factor should be in the range of 0 (0%) - 100 (100%)
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompKelvinTemporalFilter(DWORD fpVidMem, BYTE combinationFactor)
{
    LPDDRAWI_DDRAWSURFACE_LCL   dstx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   dst = dstx->lpGbl;
    unsigned long               surfaceWidth = dst->wWidth;
    unsigned long               surfaceHeight = dst->wHeight;
    unsigned long               surfacePitch = dst->lPitch;
    unsigned long               dwSrcOffset;
    unsigned long               dwDstOffset;
    unsigned long               pitch422;
    unsigned long               topFieldCombinationFactor;
    unsigned long               bottomFieldCombinationFactor;
    LPNVMCSURFACEFLAGS          lpDstSurfaceFlags;

    // We currently don't allocate filtering memory for large surfaces so we don't allow filtering on them
    if (dst->wWidth > 1024)
        return DDERR_GENERIC;

#ifdef  DEBUG
    return DD_OK;
#endif  // DEBUG

    lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

    while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpDstSurfaceFlags->dwMCSurfaceBase != fpVidMem))
        lpDstSurfaceFlags++;

    // 422 portion of this surface starts here
    dwSrcOffset = lpDstSurfaceFlags->dwMCSurface422Offset;

    // 422 filtering portion of this surface starts here
    pitch422 = (dst->wWidth + 3) & ~3;
    pitch422 = ((pitch422 << 1) + 127) & ~127;
    dwDstOffset = dwSrcOffset + (pitch422 * (DWORD)dst->wHeight);   // 422 filtering portion

    // Make sure surface has been format converted
    if ((lpDstSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) &&
        (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
        nvMoCompConvertSurfaceFormat(fpVidMem, TRUE, 15);
    else {
        if ((lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3)
            nvMoCompConvertSurfaceFormat(fpVidMem, TRUE, 3);
        if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted < 12)
            nvMoCompConvertSurfaceFormat(fpVidMem, TRUE, 12);
    }

    // If this surface has already been filtered then just return
    if (lpDstSurfaceFlags->bMCFrameIsFiltered == TRUE)
        return DD_OK;

    if (pDriverData->dwMostRecentHWUser != MODULE_ID_DDRAW) {
        if (!(nvInitKelvinForMoComp()))
            return DDERR_GENERIC;
    }

    if (!(nvInitKelvinForMoCompTemporalFilter()))
        return DDERR_GENERIC;

    if (combinationFactor > 100)
        return DDERR_INVALIDPARAMS;

    topFieldCombinationFactor = (combinationFactor * 256) / 100;
    if (topFieldCombinationFactor > 255)
        topFieldCombinationFactor = 255;
    topFieldCombinationFactor |= topFieldCombinationFactor << 8;
    topFieldCombinationFactor |= topFieldCombinationFactor << 16;

    bottomFieldCombinationFactor = 255 - topFieldCombinationFactor;
    bottomFieldCombinationFactor |= bottomFieldCombinationFactor << 8;
    bottomFieldCombinationFactor |= bottomFieldCombinationFactor << 16;

    // Load combination factors
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    nvPushData(1, topFieldCombinationFactor); // % of top field
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    nvPushData(3, bottomFieldCombinationFactor); // % of bottom field

    nvPusherAdjust(4);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
    nvPushData(1, dwSrcOffset);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(3, (dwSrcOffset + pitch422));

    lpDstSurfaceFlags->bMCFrameIsFiltered = TRUE;

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_NO_OPERATION | 0x40000);
    nvPushData(5, 0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
    nvPushData(7, dwDstOffset);

    nvPusherAdjust(8);

    SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    // Filter Top Field

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40300000);

    nvPushData(3, (0 << 16) | 0); // Odd field
    nvPushData(4, (0 << 16) | 0);
    nvPushData(5, (0 << 16) | 0);

    nvPushData(6, (surfaceHeight << 17) | 0);
    nvPushData(7, (surfaceHeight << 17) | 0);
    nvPushData(8, (surfaceHeight << 17) | 0);

    nvPushData(9, (surfaceHeight << 17) | (surfaceWidth << 1));
    nvPushData(10, (surfaceHeight << 17) | (surfaceWidth << 1));
    nvPushData(11, (surfaceHeight << 17) | (surfaceWidth << 1)); // each 32 bit write contains data 2 422 pixels

    nvPushData(12, (0 << 16) | (surfaceWidth << 1));
    nvPushData(13, (0 << 16) | (surfaceWidth << 1));
    nvPushData(14, (0 << 16) | (surfaceWidth << 1));

    nvPushData(15, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

    // Now reverse combination factors
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    nvPushData(1, bottomFieldCombinationFactor); // % of top field
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    nvPushData(3, topFieldCombinationFactor); // % of bottom field

    // Filter Bottom Field

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_NO_OPERATION | 0x40000);
    nvPushData(5, 0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);

    nvPushData(7, dwDstOffset + pitch422);

    nvPusherAdjust(8);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40300000);

    nvPushData(3, (0 << 16) | 0); // Odd field
    nvPushData(4, (0 << 16) | 0);
    nvPushData(5, (0 << 16) | 0);

    nvPushData(6, (surfaceHeight << 17) | 0);
    nvPushData(7, (surfaceHeight << 17) | 0);
    nvPushData(8, (surfaceHeight << 17) | 0);

    nvPushData(9, (surfaceHeight << 17) | (surfaceWidth << 1));
    nvPushData(10, (surfaceHeight << 17) | (surfaceWidth << 1));
    nvPushData(11, (surfaceHeight << 17) | (surfaceWidth << 1)); // each 32 bit write contains data 2 422 pixels

    nvPushData(12, (0 << 16) | (surfaceWidth << 1));
    nvPushData(13, (0 << 16) | (surfaceWidth << 1));
    nvPushData(14, (0 << 16) | (surfaceWidth << 1));

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

    // Update reference count so we know when we can display this surface
    lpDstSurfaceFlags->dwMCTemporalFilterReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);

    nvPusherStart(TRUE);

    return DD_OK;

} /* nvMoCompKelvinTemporalFilter */


/*
 * nvMoCompConvertSurfaceFormat
 *
 * Performs format conversion from our motion comp surface format to an overlay
 * compatible surface format.
 *
 * Note: Destination surface coordinates are referenced in quarter pel units.
 *       Texture coordinates are referenced in pel units.
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompConvertSurfaceFormat(DWORD fpVidMem, BOOL filterChroma, DWORD dwHowMuchToConvert)
{
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        if (!nvEnable32(NULL)) {
            return FALSE;
        }
    }

    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        return nvMoCompKelvinConvertSurfaceFormat(fpVidMem, filterChroma, dwHowMuchToConvert);
    } else {
        return nvMoCompCelsiusConvertSurfaceFormat(fpVidMem, filterChroma, dwHowMuchToConvert);
    }
} /* nvMoCompConvertSurfaceFormat */


/*
 * nvMoCompTemporalFilter
 *
 * Performs intra frame temporal filtering between the fields of a post format converted overlay surface
 *
 * Note: Destination surface coordinates are referenced in quarter pel units.
 *       Texture coordinates are referenced in quarter pel units.
 *       The combination factor should be in the range of 0 (0%) - 100 (100%)
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompTemporalFilter(DWORD fpVidMem, BYTE combinationFactor)
{
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        if (!nvEnable32(NULL)) {
            return DDERR_SURFACELOST;
        }
    }

    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        return nvMoCompKelvinTemporalFilter(fpVidMem, combinationFactor);
    } else {
        return nvMoCompCelsiusTemporalFilter(fpVidMem, combinationFactor);
    }

} /* nvMoCompTemporalFilter */


/*
 * nvMoCompDecompressSubpicture
 *
 * Performs full or partial decompression of a subpicture surface to a DVD_SUBPICTURE compatible format.
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompDecompressSubpicture(LPVOID lpInputData, DWORD dwSurfaceIndex)
{
    LPNVSPDECOMPRESSDATA lpNvSPDecompressData = (LPNVSPDECOMPRESSDATA)lpInputData;
    LPDDRAWI_DDRAWSURFACE_LCL pSurf_lcl;
    LPDDRAWI_DDRAWSURFACE_GBL pSurf_gbl;
    DWORD dwBlockSize;
    unsigned long dwLUTOffset;
    unsigned long dwDstOffset;
    unsigned long ulColor;
    float fRed;
    float fGreen;
    float fBlue;
    float fY;
    float fCb;
    float fCr;
    unsigned long dwCrCbY;
    unsigned char *LUTable;
    long n;
    unsigned long temp;
    long srcScanLength;
    long nxtSrcScan;
    FAST short pxlCount;
    unsigned long dwSrcOffset;
    unsigned char *pSrcInitBits;

    DWORD dwModeBPP = GET_MODE_BPP();
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVSPSurface;
    pSurf_gbl = pSurf_lcl->lpGbl;

    if (pSurf_gbl->ddpfSurface.dwFourCC != FOURCC_NVSP)
        return (DDERR_INVALIDPARAMS);

    if ((lpNvSPDecompressData->wSPHOffset < 0) ||
        (lpNvSPDecompressData->wSPVOffset < 0) ||
        (lpNvSPDecompressData->wSPWidth <= 0) ||
        (lpNvSPDecompressData->wSPHeight <= 0))
        return (DDERR_INVALIDPARAMS);


    dwBlockSize = pSurf_gbl->lPitch * pSurf_gbl->wHeight; // calculate size of 8 bit index per pixel plane
    dwBlockSize <<= 1;  // double size to skip over 8 bit alpha per pixel plane
    dwBlockSize += (dwBlockSize << 1); // increase to skip over fully decompressed 32 bit per pixel plane

    dwLUTOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    // offset to base of look up table in video memory
    dwLUTOffset += dwBlockSize;

    // Update color conversion table if necessary
    if (lpNvSPDecompressData->bSPUpdateLookUpTable) {

        nvPushData (0, dDrawSubchannelOffset(NV_DD_SURFACES) +
            NV042_SET_COLOR_FORMAT | 0x40000);
        nvPushData (1, NV042_SET_COLOR_FORMAT_LE_R5G6B5);
        nvPushData (2, dDrawSubchannelOffset(NV_DD_SURFACES) +
            NV042_SET_PITCH | 0xC0000);
        nvPushData (3, (1024 << 16) | 1024);
        nvPushData (4, 0);
        nvPushData (5, dwLUTOffset);
        nvPusherAdjust (6);
        pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_R5G6B5;
        pDriverData->bltData.dwLastCombinedPitch = (1024 << 16) | 1024;
        pDriverData->bltData.dwLastSrcOffset = 0;
        pDriverData->bltData.dwLastDstOffset = dwLUTOffset;

        nvPushData (0, dDrawSubchannelOffset(NV_DD_ROP) +
            SET_ROP_OFFSET | 0x40000);
        nvPushData (1, SRCCOPYINDEX);
        pDriverData->bltData.dwLastRop = SRCCOPYINDEX;

        nvPushData (2, dDrawSubchannelOffset(NV_DD_IMAGE) +
            NV061_SET_COLOR_FORMAT | 0x40000);
        nvPushData (3, NV061_SET_COLOR_FORMAT_LE_R5G6B5);

        nvPushData (4, dDrawSubchannelOffset(NV_DD_IMAGE) +
            IMAGE_FROM_CPU_POINT_OFFSET | 0xC0000);
        nvPushData (5, 0);

        if (lpNvSPDecompressData->bSPNumberOfActiveTableEntries == 0) {
            n = 256;
            nvPushData (6, asmMergeCoords(512, 1));
            nvPushData (7, asmMergeCoords(512, 1));
        } else {
            n = lpNvSPDecompressData->bSPNumberOfActiveTableEntries;
            nvPushData (6, asmMergeCoords((lpNvSPDecompressData->bSPNumberOfActiveTableEntries << 1), 1));
            nvPushData (7, asmMergeCoords((lpNvSPDecompressData->bSPNumberOfActiveTableEntries << 1), 1));
        }

        nvPusherAdjust (8);

        getDC()->nvPusher.makeSpace (1 + n); // > 128 word xfer
        nvPushData (0, dDrawSubchannelOffset(NV_DD_IMAGE) +
                       IMAGE_FROM_CPU_COLOR_OFFSET | (n << 18));
        getDC()->nvPusher.inc (1);

        LUTable = (unsigned char *)&lpNvSPDecompressData->dwSPLookUpTable[0];

        while (--n >= 0) {

            if (lpNvSPDecompressData->bSPFormat == 1) { // RGB look up table values
                ulColor = LUTable[0];
                fBlue = (float)ulColor;
                ulColor = LUTable[1];
                fGreen = (float)ulColor;
                ulColor = LUTable[2];
                fRed = (float)ulColor;

                // ITU-R BT.601 RGB conversion to YCbCr assuming gamma corrected RGB values ranging from 0 - 255
                //
                // Cr = 0.439R - 0.368G - 0.071B + 128
                // Cb = -0.148R - 0.291G + 0.439B + 128
                // Y = 0.257R + 0.504G + 0.98B + 16

                fCr = (float)((0.439 * fRed) - (0.368 * fGreen) - (0.071 * fBlue) + 128.0);
                if (fCr < 0.0)
                    fCr = 0.0;
                else if (fCr > 255.0)
                    fCr = 255.0;
                dwCrCbY = (unsigned long)fCr;
                dwCrCbY <<= 8;
                fCb = (float)((-0.148 * fRed) - (0.291 * fGreen) + (0.439 * fBlue) + 128.0);
                if (fCb < 0.0)
                    fCb = 0.0;
                else if (fCb > 255.0)
                    fCb = 255.0;
                dwCrCbY |= (unsigned long)fCb;
                dwCrCbY <<= 8;
                fY = (float)((0.257 * fRed) + (0.504 * fGreen) + (0.98 * fBlue) + 16.0);
                if (fY > 255.0)
                    dwCrCbY |= 255;
                else dwCrCbY |= (unsigned long)fY;
            } else { // YUV look up table values
                dwCrCbY = LUTable[0];
                dwCrCbY = (dwCrCbY << 8) | LUTable[1];
                dwCrCbY = (dwCrCbY << 8) | LUTable[2];
            }

            LUTable += 4;

            nvPushData (0, dwCrCbY);

            getDC()->nvPusher.inc (1);
        }

        // Restore IMAGE_FROM_CPU color format
        nvPushData (0, dDrawSubchannelOffset(NV_DD_IMAGE) +
            NV061_SET_OPERATION | 0x40000);
        nvPushData (1, NV061_SET_COLOR_FORMAT);

        if (dwModeBPP == 8)
            nvPushData (2, NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
        else if (dwModeBPP == 16)
            nvPushData (2, NV061_SET_COLOR_FORMAT_LE_R5G6B5);
        else
            nvPushData (2, NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);

        nvPusherAdjust (3);

        nvPusherStart (TRUE);
    }

    // Now decompress index plane to X8Y8U8V8
    dwBlockSize = pSurf_gbl->lPitch * pSurf_gbl->wHeight; // calculate size of 8 bit index per pixel plane
    dwBlockSize <<= 1;  // double size to skip over 8 bit alpha per pixel plane

    dwDstOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    // offset to base of fully decompressed pixel plane in video memory
    dwDstOffset += dwBlockSize;

    nvPushData (0, dDrawSubchannelOffset(NV_DD_SURFACES) +
        NV042_SET_COLOR_FORMAT | 0x40000);
    nvPushData (1, NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
    nvPushData (2, dDrawSubchannelOffset(NV_DD_SURFACES) +
        NV042_SET_PITCH | 0xC0000);
    nvPushData (3, (pSurf_gbl->lPitch << 18) | (pSurf_gbl->lPitch << 2)); // output is 32 bpp
    nvPushData (4, 0);
    nvPushData (5, dwDstOffset);
    nvPusherAdjust (6);
    pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
    pDriverData->bltData.dwLastCombinedPitch = (pSurf_gbl->lPitch << 18) | (pSurf_gbl->lPitch << 2);
    pDriverData->bltData.dwLastSrcOffset = 0;
    pDriverData->bltData.dwLastDstOffset = dwDstOffset;

    srcScanLength = pSurf_gbl->lPitch;

    pSrcInitBits = (unsigned char *) VIDMEM_ADDR(pSurf_gbl->fpVidMem);

    pSrcInitBits += (lpNvSPDecompressData->wSPVOffset * srcScanLength);

    pSrcInitBits += (lpNvSPDecompressData->wSPHOffset & 0xFFFC);

    dwSrcOffset = lpNvSPDecompressData->wSPHOffset & 3;

    temp = ((lpNvSPDecompressData->wSPWidth + dwSrcOffset + 3) & 0xFFFC);

    nxtSrcScan = srcScanLength - temp;

    n = lpNvSPDecompressData->wSPHeight;

    nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData (1, NV_DD_IMAGE_BLACK_RECTANGLE);

    // Let others know that we changed the clip
    pDriverData->dwSharedClipChangeCount++;

    pDriverData->dwDDMostRecentClipChangeCount = pDriverData->dwSharedClipChangeCount;

    // changing canvas clip avoids manually aligning 8-bit source
    nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
        IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000);
    nvPushData (3, 0);
    nvPushData (4, asmMergeCoords(NV_MAX_X_CLIP, NV_MAX_Y_CLIP));

    nvPushData (5, dDrawSubchannelOffset(NV_DD_ROP) +
        SET_ROP_OFFSET | 0x40000);
    nvPushData (6, SRCCOPYINDEX);
    pDriverData->bltData.dwLastRop = SRCCOPYINDEX;

    nvPusherAdjust (7);

    nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData (1, NV_DD_INDEXED_IMAGE_FROM_CPU);

    nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
        NV064_INDEX_FORMAT | 0x40000);
    nvPushData (3, NV064_INDEX_FORMAT_LE_I8);

    nvPushData (4, dDrawSubchannelOffset(NV_DD_SPARE) +
        NV064_LUT_OFFSET | 0x40000);
    nvPushData (5, dwLUTOffset);

    /* compiler screws up this expression when it goes negative in following macro */
    temp = lpNvSPDecompressData->wSPHOffset - dwSrcOffset;

    nvPushData (6, dDrawSubchannelOffset(NV_DD_SPARE) +
        NV064_POINT | 0xC0000);
    nvPushData (7, asmMergeCoords(temp, lpNvSPDecompressData->wSPVOffset));
    nvPushData (8,
        asmMergeCoords(lpNvSPDecompressData->wSPWidth + dwSrcOffset, lpNvSPDecompressData->wSPHeight));

    temp = ((lpNvSPDecompressData->wSPWidth + dwSrcOffset + 3) & 0xFFFC);

    nvPushData (9, asmMergeCoords(temp, lpNvSPDecompressData->wSPHeight));

    nvPusherAdjust (10);

    {
       FAST unsigned char *pSrcBits = pSrcInitBits;
       FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

       while (--n >= 0) {

           pxlCount = (short)temp;

           getDC()->nvPusher.makeSpace (1 + temp / 2); // > 128 word xfer
           nvPushData (0,dDrawSubchannelOffset(NV_DD_SPARE) +
               NV064_INDICES(0) | (temp << 16));
           getDC()->nvPusher.inc (1);

           while (pxlCount >= 32) {
               pxlCount -= 32;

               tmpPxls0 = *(unsigned long *)&pSrcBits[0];
               tmpPxls1 = *(unsigned long *)&pSrcBits[4];
               nvPushData (0,tmpPxls0);
               tmpPxls2 = *(unsigned long *)&pSrcBits[8];
               nvPushData (1,tmpPxls1);
               tmpPxls0 = *(unsigned long *)&pSrcBits[12];
               tmpPxls1 = *(unsigned long *)&pSrcBits[16];
               nvPushData (2,tmpPxls2);
               nvPushData (3,tmpPxls0);
               tmpPxls2 = *(unsigned long *)&pSrcBits[20];
               tmpPxls0 = *(unsigned long *)&pSrcBits[24];
               nvPushData (4,tmpPxls1);
               nvPushData (5,tmpPxls2);
               tmpPxls1 = *(unsigned long *)&pSrcBits[28];
               nvPushData (6,tmpPxls0);
               pSrcBits += 32;
               nvPushData (7,tmpPxls1);

               getDC()->nvPusher.inc (8);
           }

           if (pxlCount > 0) {

               while (pxlCount >= 8) {
                   pxlCount -= 8;
                   tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                   tmpPxls1 = *(unsigned long *)&pSrcBits[4];
                   pSrcBits += 8;
                   nvPushData (0,tmpPxls0);
                   nvPushData (1,tmpPxls1);
                   getDC()->nvPusher.inc (2);
               }

               if (pxlCount > 0) {
                   pxlCount -= 4;
                   tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                   pSrcBits += 4;
                   nvPushData (0,tmpPxls0);

                   getDC()->nvPusher.inc (1);

                   if (pxlCount > 0) {
                       tmpPxls1 = *(unsigned long *)&pSrcBits[0];
                       pSrcBits += 4;
                       nvPushData (0,tmpPxls1);
                       getDC()->nvPusher.inc (1);
                   }
               }
           }
           pSrcBits += nxtSrcScan;

           nvPusherStart (TRUE);
        }
    }

    // Restore surface color format
    nvPushData (0, dDrawSubchannelOffset(NV_DD_SURFACES) +
        NV042_SET_COLOR_FORMAT | 0x40000);

    if (dwModeBPP == 8) {
        nvPushData (1, NV042_SET_COLOR_FORMAT_LE_Y8);
        pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_Y8;
    } else if (dwModeBPP == 16) {
        nvPushData (1, NV042_SET_COLOR_FORMAT_LE_R5G6B5);
        pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_R5G6B5;
    } else {
        nvPushData (1, NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
        pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
    }

    nvPusherAdjust (2);

    // Now copy the corresponding alpha values into the freshly decompressed X8Y8U8V8 pixels
    dwSrcOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    dwSrcOffset += pSurf_gbl->lPitch * pSurf_gbl->wHeight; // offset over the 8 bit index per pixel plane

    dwSrcOffset += (lpNvSPDecompressData->wSPVOffset * srcScanLength);

    dwSrcOffset += lpNvSPDecompressData->wSPHOffset;

    // Destination is 32 bits per pixel
    dwDstOffset += (lpNvSPDecompressData->wSPVOffset * (srcScanLength << 2));

    dwDstOffset += (lpNvSPDecompressData->wSPHOffset << 2);

    dwDstOffset += 3; // first alpha value is 4 byte of first X8Y8U8V8 dword

    n = lpNvSPDecompressData->wSPHeight;

    nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData (1, NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
            MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
    nvPushData (3, dwSrcOffset);
    nvPushData (4, dwDstOffset);
    nvPushData (5, srcScanLength);
    nvPushData (6, (srcScanLength << 2));
    nvPushData (7, (DWORD)lpNvSPDecompressData->wSPWidth);
    nvPushData (8, (DWORD)lpNvSPDecompressData->wSPHeight);
    nvPushData (9, (4 << 8) | 1);
    nvPushData (10, 0);

    nvPusherAdjust (11);

    nvPusherStart (TRUE);

    pDriverData->dDrawSpareSubchannelObject = NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;

    return DD_OK;

} /* nvMoCompDecompressSubpicture */



/*
 * nvMoCompCompositeSubpicture
 *
 * Performs full or partial compositing of a decompressed subpicture surface to the destination overlay
 * surface with scaling.
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompCompositeSubpicture(LPVOID lpInputData, DWORD dwSurfaceIndex)
{
    LPNVSPCOMPOSITEDATA lpNvSPCompositeData = (LPNVSPCOMPOSITEDATA)lpInputData;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
    LPDDRAWI_DDRAWSURFACE_LCL   pSPSurf_lcl;
    LPDDRAWI_DDRAWSURFACE_GBL   pSPSurf_gbl;
    LPNVMCSURFACEFLAGS          lpDstSurfaceFlags;
    DWORD                       dwBlockSize;
    unsigned long               dwSrcOffset;
    unsigned long               dw422Offset;
    unsigned long               dwDstOffset;
    unsigned long               pitch422;
    unsigned long               dwSubPictureSurfaceWidth;
    unsigned long               dwSubPictureSurfaceHeight;
    unsigned long               dwCompositeSurfaceWidth;
    unsigned long               dwCompositeSurfaceHeight;
    BYTE                        revertRegion = FALSE;


    if ((lpNvSPCompositeData->wSPHOffset < 0) ||
        (lpNvSPCompositeData->wSPVOffset < 0) ||
        (lpNvSPCompositeData->wSPWidth < 0) ||
        (lpNvSPCompositeData->wSPHeight < 0))
        return (DDERR_INVALIDPARAMS);

    if ((lpNvSPCompositeData->wMCHOffset < 0) ||
        (lpNvSPCompositeData->wMCVOffset < 0) ||
        (lpNvSPCompositeData->wMCWidth < 0) ||
        (lpNvSPCompositeData->wMCHeight < 0))
        return (DDERR_INVALIDPARAMS);

    // Is this a request to revert to an uncomposited destination ?
    if ((lpNvSPCompositeData->wSPHOffset == 0) &&
        (lpNvSPCompositeData->wSPVOffset == 0) &&
        (lpNvSPCompositeData->wSPWidth == 0) &&
        (lpNvSPCompositeData->wSPHeight == 0))
        revertRegion = TRUE;

    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    pSurf_gbl = pSurf_lcl->lpGbl;

    pitch422 = (pSurf_gbl->wWidth + 3) & ~3;
    pitch422 = ((pitch422 << 1) + 127) & ~127;

    dwCompositeSurfaceWidth = pSurf_gbl->wWidth;
    dwCompositeSurfaceHeight = pSurf_gbl->wHeight;

    pSPSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVSPSurface;
    pSPSurf_gbl = pSPSurf_lcl->lpGbl;
    
    if(!pSPSurf_gbl->fpVidMem)
        return (DDERR_SURFACELOST);

    dwSubPictureSurfaceWidth = pSurf_gbl->wWidth;
    dwSubPictureSurfaceHeight = pSurf_gbl->wHeight;

    if (pSPSurf_gbl->ddpfSurface.dwFourCC != FOURCC_NVSP) {
        return (DDERR_INVALIDPARAMS);
    }

    lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

    while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpDstSurfaceFlags->dwMCSurfaceBase != pSurf_gbl->fpVidMem))
        lpDstSurfaceFlags++;

    // First make sure destination surface has been format converted
    if ((lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME) &&
        (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 15);
    else if ((lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_TOP_FIELD) &&
        ((lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 3);
    else if ((lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_BOTTOM_FIELD) &&
        (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted < 12))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 12);

    if ((revertRegion) || (lpNvSPCompositeData->bSPRestoreImageBeforeCompositing)) {
        unsigned long X0, Y0, X1, Y1;
        unsigned long tmpVal;

        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {

            if (!(nvInitKelvinForMoCompConversion()))
                return DDERR_GENERIC;

            tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
                     (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
                     (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
                     (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
                     (NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0 << 24) |
                     (NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 << 16) |
                     (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_FILTER(1) | 0x40000);
            nvPushData(1, tmpVal);

            nvPusherAdjust(2);

            // 420 LUMA portion of this surface starts at fpVidMem
            dwDstOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
            nvPushData(1, dwDstOffset);

            // 420 CHROMA portion of this surface starts here
            dwDstOffset += (pSurf_gbl->lPitch * pSurf_gbl->wHeight);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(3, dwDstOffset);
            nvPusherAdjust(4);
        } else {

            if (!(nvInitCelsiusForMoCompConversion()))
                return DDERR_GENERIC;

            tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR << 28) |
                     (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR << 24);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_FILTER(1) | 0x40000);
            nvPushData(1, tmpVal);

            nvPusherAdjust(2);

            // 420 LUMA portion of this surface starts at fpVidMem
            dwDstOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
            nvPushData(1, dwDstOffset);

            // 420 CHROMA portion of this surface starts here
            dwDstOffset += (pSurf_gbl->lPitch * pSurf_gbl->wHeight);
            nvPushData(2, dwDstOffset);
            nvPusherAdjust(3);
        }

        if (revertRegion) {
            dw422Offset = lpDstSurfaceFlags->dwMCSurface422Offset;

            // Offset to NV12 surface filtered pixel area if necessary
            if (lpDstSurfaceFlags->bMCFrameIsFiltered) {
                if ((lpDstSurfaceFlags->bMCPictureStructure != PICTURE_STRUCTURE_FRAME) ||
                    (lpNvSPCompositeData->bSPCompositeWith != PICTURE_STRUCTURE_FRAME)) {
                    dw422Offset += (pitch422 * (DWORD)pSurf_gbl->wHeight);  // 422 portion
                }
            }
        } else {
            dw422Offset = VIDMEM_OFFSET(pSPSurf_gbl->fpVidMem);
            dwBlockSize = pSPSurf_gbl->lPitch * pSPSurf_gbl->wHeight; // calculate size of subpicture 8 bit index per pixel plane
            dwBlockSize <<= 1;                  // double size to skip over 8 bit alpha per pixel plane
            dw422Offset += dwBlockSize;         // offset to base of fully decompressed pixel plane in video memory
            dw422Offset += (dwBlockSize << 1);  // increase to skip fully decompressed 32 bit per pixel plane
            dw422Offset += 1024;                // also skip indexed image look up table
        }

        X0 = lpNvSPCompositeData->wMCHOffset;
        X1 = X0 + lpNvSPCompositeData->wMCWidth;
        Y0 = lpNvSPCompositeData->wMCVOffset;
        Y1 = Y0 + lpNvSPCompositeData->wMCHeight;

        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, dw422Offset);

            nvPusherAdjust(4);

            SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_BEGIN_END | 0x40000);
            nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_INLINE_ARRAY | 0x40300000);

            nvPushData(3, (Y0 << 18) | (X0 << 1));
            nvPushData(4, (Y0 << 16) | (X0 >> 1));
            nvPushData(5, (Y0 << 15) | (X0 >> 1));

            nvPushData(6, ((Y1 << 2) << 16) | (X0 << 1)); // destination is in quarter pel units
            nvPushData(7, (Y1 << 16) | (X0 >> 1)); // LUMA texture is in pel units
            if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
                nvPushData(8, ((Y1 >> 1) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height
            else
                nvPushData(8, ((Y1 >> 2) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height

            nvPushData(9, ((Y1 << 2) << 16) | (X1 << 1)); // each 32 bit write contains data for 2 422 pixels
            nvPushData(10, (Y1 << 16) | (X1 >> 1));
            if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
                nvPushData(11, ((Y1 >> 1) << 16) | (X1 >> 1));
            else
                nvPushData(11, ((Y1 >> 2) << 16) | (X1 >> 1));

            nvPushData(12, (Y0 << 18) | (X1 << 1));
            nvPushData(13, (Y0 << 16) | (X1 >> 1));
            nvPushData(14, (Y0 << 15) | (X1 >> 1));

            nvPushData(15, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_BEGIN_END | 0x40000);
            nvPushData(16, NV097_SET_BEGIN_END_OP_END);
            nvPusherAdjust(17);
        } else {

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, dw422Offset);

            nvPusherAdjust(4);

            SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_BEGIN_END | 0x40000);
            nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_INLINE_ARRAY(0) | 0x300000);

            // No HD scaled version because subpicture compositing is not used for HD 
            nvPushData(3, (Y0 << 15) | (X0 >> 1));
            nvPushData(4, (Y0 << 16) | (X0 >> 1));
            nvPushData(5, (Y0 << 18) | (((X0 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

            if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
                nvPushData(6, ((Y1 >> 1) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height
            else
                nvPushData(6, ((Y1 >> 2) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height
            nvPushData(7, (Y1 << 16) | (X0 >> 1)); // LUMA texture is in pel units
            nvPushData(8, ((Y1 << 2) << 16) | (((X0 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // destination is in quarter pel units

            if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
                nvPushData(9, ((Y1 >> 1) << 16) | (X1 >> 1));
            else
                nvPushData(9, ((Y1 >> 2) << 16) | (X1 >> 1));
            nvPushData(10, (Y1 << 16) | (X1 >> 1));
            nvPushData(11, ((Y1 << 2) << 16) | (((X1 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // each 32 bit write contains data for 2 422 pixels

            nvPushData(12, (Y0 << 15) | (X1 >> 1));
            nvPushData(13, (Y0 << 16) | (X1 >> 1));
            nvPushData(14, (Y0 << 18) | (((X1 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

            nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_BEGIN_END | 0x40000);
            nvPushData(16, NV056_SET_BEGIN_END_OP_END);
            nvPusherAdjust(17);
        }

        nvPusherStart(TRUE);
    }

    if (!revertRegion) {
        long subPictureScaleDeltaX;
        long subPictureScaleDeltaY;

        dwDstOffset = lpDstSurfaceFlags->dwMCSurface422Offset;

        if (lpNvSPCompositeData->bSPRestoreImageBeforeCompositing == 0)
            dw422Offset = dwDstOffset;

        if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
            lpDstSurfaceFlags->bMCFrameIsComposited = PICTURE_STRUCTURE_FRAME;
        else if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_TOP_FIELD)
            lpDstSurfaceFlags->bMCFrameIsComposited |= PICTURE_STRUCTURE_TOP_FIELD;
        else
            lpDstSurfaceFlags->bMCFrameIsComposited |= PICTURE_STRUCTURE_BOTTOM_FIELD;

        dwSrcOffset = VIDMEM_OFFSET(pSPSurf_gbl->fpVidMem);

        dwBlockSize = pSPSurf_gbl->lPitch * pSPSurf_gbl->wHeight;   // calculate size of subpicture 8 bit index per pixel plane
        dwBlockSize <<= 1;                                          // double size to skip over 8 bit alpha per pixel plane

        dwSrcOffset += dwBlockSize;                                 // offset to base of fully decompressed pixel plane in video memory

        subPictureScaleDeltaX = lpNvSPCompositeData->wSPWidth;
        subPictureScaleDeltaX = (subPictureScaleDeltaX << 20) / lpNvSPCompositeData->wMCWidth;
        if ((subPictureScaleDeltaX < 0x10000) || (subPictureScaleDeltaX > 0x1000000))
            subPictureScaleDeltaX = 0x100000;

        subPictureScaleDeltaY = lpNvSPCompositeData->wSPHeight;
        subPictureScaleDeltaY = (subPictureScaleDeltaY << 20) / lpNvSPCompositeData->wMCHeight;
        if ((subPictureScaleDeltaY < 0x10000) || (subPictureScaleDeltaY > 0x1000000))
            subPictureScaleDeltaY = 0x100000;

        if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME) {

            nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
            nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
            nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
                    NV088_IMAGE_OUT_POINT | 0x400000);
            nvPushData (3, asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset));
            nvPushData (4, asmMergeCoords(lpNvSPCompositeData->wMCWidth, lpNvSPCompositeData->wMCHeight));
            nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | pitch422);
            nvPushData (6, dwDstOffset);                                            // 422 destination pixel area
            nvPushData (7, 0x100000);
            nvPushData (8, 0x100000);
            nvPushData (9, asmMergeCoords(dwCompositeSurfaceWidth, dwCompositeSurfaceHeight));
            nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | pitch422);

            nvPushData (11, dw422Offset);                                           // 422 source pixel area
            nvPushData (12, ((asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset)) << 4));
            nvPushData (13, subPictureScaleDeltaX);
            nvPushData (14, subPictureScaleDeltaY);
            nvPushData (15, asmMergeCoords(dwSubPictureSurfaceWidth, dwSubPictureSurfaceHeight));
            nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8 << 16) | (pSurf_gbl->lPitch << 2));
            nvPushData (17, dwSrcOffset);
            nvPushData (18, ((asmMergeCoords(lpNvSPCompositeData->wSPHOffset, lpNvSPCompositeData->wSPVOffset)) << 4));

            nvPusherAdjust (19);

            if (lpDstSurfaceFlags->bMCFrameIsFiltered) {
                dwDstOffset += (pitch422 * (DWORD)pSurf_gbl->wHeight);  // 422 filtered portion

                if (lpNvSPCompositeData->bSPRestoreImageBeforeCompositing == 0)
                dw422Offset = dwDstOffset;

                nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
                nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
                nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
                        NV088_IMAGE_OUT_POINT | 0x400000);
                nvPushData (3, asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset));
                nvPushData (4, asmMergeCoords(lpNvSPCompositeData->wMCWidth, lpNvSPCompositeData->wMCHeight));
                nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | pitch422);
                nvPushData (6, dwDstOffset);                                            // 422 destination pixel area
                nvPushData (7, 0x100000);
                nvPushData (8, 0x100000);
                nvPushData (9, asmMergeCoords(dwCompositeSurfaceWidth, dwCompositeSurfaceHeight));
                nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | pitch422);

                nvPushData (11, dw422Offset);                                           // 422 source pixel area
                nvPushData (12, ((asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset)) << 4));
                nvPushData (13, subPictureScaleDeltaX);
                nvPushData (14, subPictureScaleDeltaY);
                nvPushData (15, asmMergeCoords(dwSubPictureSurfaceWidth, dwSubPictureSurfaceHeight));
                nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8 << 16) | (pSurf_gbl->lPitch << 2));
                nvPushData (17, dwSrcOffset);
                nvPushData (18, ((asmMergeCoords(lpNvSPCompositeData->wSPHOffset, lpNvSPCompositeData->wSPVOffset)) << 4));

                nvPusherAdjust (19);
            }
        } else if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_TOP_FIELD) {

            nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
            nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
            nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
                    NV088_IMAGE_OUT_POINT | 0x400000);
            nvPushData (3, asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset));
            nvPushData (4, asmMergeCoords(lpNvSPCompositeData->wMCWidth, lpNvSPCompositeData->wMCHeight));
            nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | (pitch422 << 1));
            nvPushData (6, dwDstOffset);                                            // 422 destination pixel area
            nvPushData (7, 0x100000);
            nvPushData (8, 0x100000);
            nvPushData (9, asmMergeCoords(dwCompositeSurfaceWidth, dwCompositeSurfaceHeight));
            nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | (pitch422 << 1));

            nvPushData (11, dw422Offset);                                           // 422 source pixel area
            nvPushData (12, ((asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset)) << 4));
            nvPushData (13, subPictureScaleDeltaX);
            nvPushData (14, subPictureScaleDeltaY);
            nvPushData (15, asmMergeCoords(dwSubPictureSurfaceWidth, dwSubPictureSurfaceHeight));
            nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8 << 16) | (pSurf_gbl->lPitch << 3));
            nvPushData (17, dwSrcOffset);
            nvPushData (18, ((asmMergeCoords(lpNvSPCompositeData->wSPHOffset, lpNvSPCompositeData->wSPVOffset)) << 4));

            nvPusherAdjust (19);

            if (lpDstSurfaceFlags->bMCFrameIsFiltered) {
                dwDstOffset += (pitch422 * (DWORD)pSurf_gbl->wHeight);  // 422 filtered portion

                if (lpNvSPCompositeData->bSPRestoreImageBeforeCompositing == 0)
                dw422Offset = dwDstOffset;

                nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
                nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
                nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
                        NV088_IMAGE_OUT_POINT | 0x400000);
                nvPushData (3, asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset));
                nvPushData (4, asmMergeCoords(lpNvSPCompositeData->wMCWidth, lpNvSPCompositeData->wMCHeight));
                nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | (pitch422 << 1));
                nvPushData (6, dwDstOffset);                                            // 422 destination pixel area
                nvPushData (7, 0x100000);
                nvPushData (8, 0x100000);
                nvPushData (9, asmMergeCoords(dwCompositeSurfaceWidth, dwCompositeSurfaceHeight));
                nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | (pitch422 << 1));

                nvPushData (11, dw422Offset);                                           // 422 source pixel area
                nvPushData (12, ((asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset)) << 4));
                nvPushData (13, subPictureScaleDeltaX);
                nvPushData (14, subPictureScaleDeltaY);
                nvPushData (15, asmMergeCoords(dwSubPictureSurfaceWidth, dwSubPictureSurfaceHeight));
                nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8 << 16) | (pSurf_gbl->lPitch << 3));
                nvPushData (17, dwSrcOffset);
                nvPushData (18, ((asmMergeCoords(lpNvSPCompositeData->wSPHOffset, lpNvSPCompositeData->wSPVOffset)) << 4));

                nvPusherAdjust (19);
            }
        } else {

            nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
            nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
            nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
                    NV088_IMAGE_OUT_POINT | 0x400000);
            nvPushData (3, asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset));
            nvPushData (4, asmMergeCoords(lpNvSPCompositeData->wMCWidth, lpNvSPCompositeData->wMCHeight));
            nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | (pitch422 << 1));
            nvPushData (6, (dwDstOffset + pitch422));                               // 422 destination pixel area
            nvPushData (7, 0x100000);
            nvPushData (8, 0x100000);
            nvPushData (9, asmMergeCoords(dwCompositeSurfaceWidth, dwCompositeSurfaceHeight));
            nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | (pitch422 << 1));

            nvPushData (11, (dw422Offset + pitch422));                              // 422 source pixel area
            nvPushData (12, ((asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset)) << 4));
            nvPushData (13, subPictureScaleDeltaX);
            nvPushData (14, subPictureScaleDeltaY);
            nvPushData (15, asmMergeCoords(dwSubPictureSurfaceWidth, dwSubPictureSurfaceHeight));
            nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8 << 16) | (pSurf_gbl->lPitch << 3));
            nvPushData (17, (dwSrcOffset + (pSurf_gbl->lPitch << 2)));
            nvPushData (18, ((asmMergeCoords(lpNvSPCompositeData->wSPHOffset, lpNvSPCompositeData->wSPVOffset)) << 4));

            nvPusherAdjust (19);

            if (lpDstSurfaceFlags->bMCFrameIsFiltered) {
                dwDstOffset += (pitch422 * (DWORD)pSurf_gbl->wHeight);  // 422 filtered portion

                if (lpNvSPCompositeData->bSPRestoreImageBeforeCompositing == 0)
                dw422Offset = dwDstOffset;

                nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
                nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
                nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) +
                        NV088_IMAGE_OUT_POINT | 0x400000);
                nvPushData (3, asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset));
                nvPushData (4, asmMergeCoords(lpNvSPCompositeData->wMCWidth, lpNvSPCompositeData->wMCHeight));
                nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | (pitch422 << 1));
                nvPushData (6, (dwDstOffset + pitch422));                               // 422 destination pixel area
                nvPushData (7, 0x100000);
                nvPushData (8, 0x100000);
                nvPushData (9, asmMergeCoords(dwCompositeSurfaceWidth, dwCompositeSurfaceHeight));
                nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | (pitch422 << 1));

                nvPushData (11, (dw422Offset + pitch422));                              // 422 source pixel area
                nvPushData (12, ((asmMergeCoords(lpNvSPCompositeData->wMCHOffset, lpNvSPCompositeData->wMCVOffset)) << 4));
                nvPushData (13, subPictureScaleDeltaX);
                nvPushData (14, subPictureScaleDeltaY);
                nvPushData (15, asmMergeCoords(dwSubPictureSurfaceWidth, dwSubPictureSurfaceHeight));
                nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8 << 16) | (pSurf_gbl->lPitch << 3));
                nvPushData (17, (dwSrcOffset + (pSurf_gbl->lPitch << 2)));
                nvPushData (18, ((asmMergeCoords(lpNvSPCompositeData->wSPHOffset, lpNvSPCompositeData->wSPVOffset)) << 4));

                nvPusherAdjust (19);
            }
        }
    }

    // Update reference count in case we get a status query
    lpDstSurfaceFlags->dwMCCompositeReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);

    nvPusherStart(TRUE);

    pDriverData->dDrawSpareSubchannelObject = NV_VPP_DVD_SUBPICTURE_IID;

    return DD_OK;

} /* nvMoCompCompositeSubpicture */


#if 0

// This is a test function to demonstrate NV12 subpicture compositing using Celsius in NV17.
// It would need some modification to actually be used in the driver.

extern DWORD __stdcall nvInitCelsiusForMoCompNV12LumaSubpictureCompositing(void);
extern DWORD __stdcall nvInitCelsiusForMoCompNV12ChromaSubpictureCompositing(void);

/*
 * nvMoCompCompositeNV12Subpicture
 *
 * Performs full or partial compositing of a decompressed subpicture surface to the destination NV12
 * surface with scaling.  Currently works in frame mode only.
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompCompositeNV12Subpicture(LPVOID lpInputData, DWORD dwSurfaceIndex)
{
    LPNVSPCOMPOSITEDATA lpNvSPCompositeData = (LPNVSPCOMPOSITEDATA)lpInputData;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
    LPDDRAWI_DDRAWSURFACE_LCL   pSPSurf_lcl;
    LPDDRAWI_DDRAWSURFACE_GBL   pSPSurf_gbl;
    LPNVMCSURFACEFLAGS          lpDstSurfaceFlags;
    DWORD                       dwBlockSize;
    unsigned long               dwSrcOffset;
    unsigned long               dw422Offset;
    unsigned long               dwDstOffset;
    unsigned long               pitch422;
    unsigned long               dwSubPictureSurfaceWidth;
    unsigned long               dwSubPictureSurfaceHeight;
    unsigned long               dwCompositeSurfaceWidth;
    unsigned long               dwCompositeSurfaceHeight;
    unsigned long               X0, Y0, X1, Y1;
    unsigned long               SPX0, SPY0, SPX1, SPY1;
    BYTE                        revertRegion = FALSE;

    if ((lpNvSPCompositeData->wSPHOffset < 0) ||
        (lpNvSPCompositeData->wSPVOffset < 0) ||
        (lpNvSPCompositeData->wSPWidth < 0) ||
        (lpNvSPCompositeData->wSPHeight < 0))
        return (DDERR_INVALIDPARAMS);

    if ((lpNvSPCompositeData->wMCHOffset < 0) ||
        (lpNvSPCompositeData->wMCVOffset < 0) ||
        (lpNvSPCompositeData->wMCWidth < 0) ||
        (lpNvSPCompositeData->wMCHeight < 0))
        return (DDERR_INVALIDPARAMS);

    // Is this a request to revert to an uncomposited destination ?
    if ((lpNvSPCompositeData->wSPHOffset == 0) &&
        (lpNvSPCompositeData->wSPVOffset == 0) &&
        (lpNvSPCompositeData->wSPWidth == 0) &&
        (lpNvSPCompositeData->wSPHeight == 0))
        revertRegion = TRUE;

    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    pSurf_gbl = pSurf_lcl->lpGbl;

    pitch422 = (pSurf_gbl->wWidth + 3) & ~3;
    pitch422 = ((pitch422 << 1) + 127) & ~127;

    dwCompositeSurfaceWidth = pSurf_gbl->wWidth;
    dwCompositeSurfaceHeight = pSurf_gbl->wHeight;

    pSPSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVSPSurface;
    pSPSurf_gbl = pSPSurf_lcl->lpGbl;

    dwSubPictureSurfaceWidth = pSurf_gbl->wWidth;
    dwSubPictureSurfaceHeight = pSurf_gbl->wHeight;

    if (pSPSurf_gbl->ddpfSurface.dwFourCC != FOURCC_NVSP) {
        return (DDERR_INVALIDPARAMS);
    }

    lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

    while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpDstSurfaceFlags->dwMCSurfaceBase != pSurf_gbl->fpVidMem))
        lpDstSurfaceFlags++;

    // First make sure destination surface has been format converted
    if ((lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME) &&
        (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 15);
    else if ((lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_TOP_FIELD) &&
        ((lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 3);
    else if ((lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_BOTTOM_FIELD) &&
        (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted < 12))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 12);

    X0 = lpNvSPCompositeData->wMCHOffset;
    X1 = X0 + lpNvSPCompositeData->wMCWidth;
    Y0 = lpNvSPCompositeData->wMCVOffset;
    Y1 = Y0 + lpNvSPCompositeData->wMCHeight;

    SPX0 = lpNvSPCompositeData->wSPHOffset;
    SPX1 = SPX0 + lpNvSPCompositeData->wSPWidth;
    SPY0 = lpNvSPCompositeData->wSPVOffset;
    SPY1 = SPY0 + lpNvSPCompositeData->wSPHeight;

    if ((revertRegion) || (lpNvSPCompositeData->bSPRestoreImageBeforeCompositing)) {
        unsigned long tmpVal;

        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {

            if (!(nvInitKelvinForMoCompConversion()))
                return DDERR_GENERIC;

            tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
                     (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
                     (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
                     (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
                     (NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0 << 24) |
                     (NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 << 16) |
                     (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_FILTER(1) | 0x40000);
            nvPushData(1, tmpVal);

            nvPusherAdjust(2);

            // 420 LUMA portion of this surface starts at fpVidMem
            dwDstOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
            nvPushData(1, dwDstOffset);

            // 420 CHROMA portion of this surface starts here
            dwDstOffset += (pSurf_gbl->lPitch * pSurf_gbl->wHeight);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(3, dwDstOffset);
            nvPusherAdjust(4);
        } else {

            if (!(nvInitCelsiusForMoCompConversion()))
                return DDERR_GENERIC;

            tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR << 28) |
                     (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR << 24);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_FILTER(1) | 0x40000);
            nvPushData(1, tmpVal);

            nvPusherAdjust(2);

            // 420 LUMA portion of this surface starts at fpVidMem
            dwDstOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
            nvPushData(1, dwDstOffset);

            // 420 CHROMA portion of this surface starts here
            dwDstOffset += (pSurf_gbl->lPitch * pSurf_gbl->wHeight);
            nvPushData(2, dwDstOffset);
            nvPusherAdjust(3);
        }

        if (revertRegion) {
            dw422Offset = lpDstSurfaceFlags->dwMCSurface422Offset;

            // Offset to NV12 surface filtered pixel area if necessary
            if (lpDstSurfaceFlags->bMCFrameIsFiltered) {
                if ((lpDstSurfaceFlags->bMCPictureStructure != PICTURE_STRUCTURE_FRAME) ||
                    (lpNvSPCompositeData->bSPCompositeWith != PICTURE_STRUCTURE_FRAME)) {
                    dw422Offset += (pitch422 * (DWORD)pSurf_gbl->wHeight);  // 422 portion
                }
            }
        } else {
            dw422Offset = VIDMEM_OFFSET(pSPSurf_gbl->fpVidMem);
            dwBlockSize = pSPSurf_gbl->lPitch * pSPSurf_gbl->wHeight; // calculate size of subpicture 8 bit index per pixel plane
            dwBlockSize <<= 1;                  // double size to skip over 8 bit alpha per pixel plane
            dw422Offset += dwBlockSize;         // offset to base of fully decompressed pixel plane in video memory
            dw422Offset += (dwBlockSize << 1);  // increase to skip fully decompressed 32 bit per pixel plane
            dw422Offset += 1024;                // also skip indexed image look up table
        }


        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, dw422Offset);

            nvPusherAdjust(4);

            SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_BEGIN_END | 0x40000);
            nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_INLINE_ARRAY | 0x40300000);

            nvPushData(3, (Y0 << 18) | (X0 << 1));
            nvPushData(4, (Y0 << 16) | (X0 >> 1));
            nvPushData(5, (Y0 << 15) | (X0 >> 1));

            nvPushData(6, ((Y1 << 2) << 16) | (X0 << 1)); // destination is in quarter pel units
            nvPushData(7, (Y1 << 16) | (X0 >> 1)); // LUMA texture is in pel units
            if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
                nvPushData(8, ((Y1 >> 1) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height
            else
                nvPushData(8, ((Y1 >> 2) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height

            nvPushData(9, ((Y1 << 2) << 16) | (X1 << 1)); // each 32 bit write contains data for 2 422 pixels
            nvPushData(10, (Y1 << 16) | (X1 >> 1));
            if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
                nvPushData(11, ((Y1 >> 1) << 16) | (X1 >> 1));
            else
                nvPushData(11, ((Y1 >> 2) << 16) | (X1 >> 1));

            nvPushData(12, (Y0 << 18) | (X1 << 1));
            nvPushData(13, (Y0 << 16) | (X1 >> 1));
            nvPushData(14, (Y0 << 15) | (X1 >> 1));

            nvPushData(15, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_BEGIN_END | 0x40000);
            nvPushData(16, NV097_SET_BEGIN_END_OP_END);
            nvPusherAdjust(17);
        } else {

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, dw422Offset);

            nvPusherAdjust(4);

            SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_BEGIN_END | 0x40000);
            nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_INLINE_ARRAY(0) | 0x300000);

            // No HD scaled version because subpicture compositing is not used for HD 
            nvPushData(3, (Y0 << 15) | (X0 >> 1));
            nvPushData(4, (Y0 << 16) | (X0 >> 1));
            nvPushData(5, (Y0 << 18) | (((X0 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

            if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
                nvPushData(6, ((Y1 >> 1) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height
            else
                nvPushData(6, ((Y1 >> 2) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height
            nvPushData(7, (Y1 << 16) | (X0 >> 1)); // LUMA texture is in pel units
            nvPushData(8, ((Y1 << 2) << 16) | (((X0 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // destination is in quarter pel units

            if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
                nvPushData(9, ((Y1 >> 1) << 16) | (X1 >> 1));
            else
                nvPushData(9, ((Y1 >> 2) << 16) | (X1 >> 1));
            nvPushData(10, (Y1 << 16) | (X1 >> 1));
            nvPushData(11, ((Y1 << 2) << 16) | (((X1 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // each 32 bit write contains data for 2 422 pixels

            nvPushData(12, (Y0 << 15) | (X1 >> 1));
            nvPushData(13, (Y0 << 16) | (X1 >> 1));
            nvPushData(14, (Y0 << 18) | (((X1 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

            nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_BEGIN_END | 0x40000);
            nvPushData(16, NV056_SET_BEGIN_END_OP_END);
            nvPusherAdjust(17);
        }

        nvPusherStart(TRUE);
    }

    if (!revertRegion) {
        unsigned long dwSPSrcOffset;
        unsigned long dwSurfacePitch;
        unsigned long surfaceWidth;
        unsigned long tmpVal;
        
        dwSrcOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

        // compositing portion of this surface starts here (overwrites filtered portion)
        dwDstOffset = lpDstSurfaceFlags->dwMCSurface422Offset + (pitch422 * (DWORD)pSurf_gbl->wHeight);   // 420 compositing and 422 filtering portion

        if (lpNvSPCompositeData->bSPRestoreImageBeforeCompositing == 0)
            dw422Offset = dwDstOffset;

        if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_FRAME)
            lpDstSurfaceFlags->bMCFrameIsComposited = PICTURE_STRUCTURE_FRAME;
        else if (lpNvSPCompositeData->bSPCompositeWith == PICTURE_STRUCTURE_TOP_FIELD)
            lpDstSurfaceFlags->bMCFrameIsComposited |= PICTURE_STRUCTURE_TOP_FIELD;
        else
            lpDstSurfaceFlags->bMCFrameIsComposited |= PICTURE_STRUCTURE_BOTTOM_FIELD;

        dwSPSrcOffset = VIDMEM_OFFSET(pSPSurf_gbl->fpVidMem);

        dwBlockSize = pSPSurf_gbl->lPitch * pSPSurf_gbl->wHeight;   // calculate size of subpicture 8 bit index per pixel plane
        dwBlockSize <<= 1;                                          // double size to skip over 8 bit alpha per pixel plane

        dwSPSrcOffset += dwBlockSize;                               // offset to base of fully decompressed pixel plane in video memory

        if (pDriverData->dwMostRecentHWUser != MODULE_ID_DDRAW) {
            if (!(nvInitCelsiusForMoComp()))
                return DDERR_GENERIC;
        }

        // Now blend subpicture rectangle with NV12 luma plane

        if (!(nvInitCelsiusForMoCompNV12LumaSubpictureCompositing()))
            return DDERR_GENERIC;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwDstOffset);

        nvPusherAdjust(4);

        dwSurfacePitch = pSurf_gbl->lPitch;
        dwSurfacePitch |= (dwSurfacePitch << 16);
        
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_PITCH | 0x40000);
        nvPushData(1, dwSurfacePitch);

        nvPusherAdjust(2);
        
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
        nvPushData(1, dwSPSrcOffset);
        nvPushData(2, dwSrcOffset);

        nvPusherAdjust(3);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
        nvPushData(1, pSPSurf_gbl->lPitch << 18); // pitch << 2
        nvPushData(2, dwSurfacePitch << 16);

        nvPusherAdjust(3);

        SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x300000);

        // No HD scaled version because subpicture compositing is not used for HD 
        nvPushData(3, (Y0 << 16) | (X0)); // source NV12 luma in pel units
        nvPushData(4, (SPY0 << 16) | (SPX0)); // AVUY subpicture in pel units
        nvPushData(5, (Y0 << 18) | (((X0 << 2) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // destination NV12 luma in quarter pel units

        nvPushData(6, (Y1 << 16) | (X0));
        nvPushData(7, (SPY1 << 16) | (SPX0));
        nvPushData(8, (Y1 << 18) | (((X0 << 2) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

        nvPushData(9, (Y1 << 16) | (X1));
        nvPushData(10, (SPY1 << 16) | (SPX1));
        nvPushData(11, (Y1 << 18) | (((X1 << 2) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

        nvPushData(12, (Y0 << 16) | (X1));
        nvPushData(13, (SPY0 << 16) | (SPX1));
        nvPushData(14, (Y0 << 18) | (((X1 << 2) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

        nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_BEGIN_END | 0x40000);
        nvPushData(16, NV056_SET_BEGIN_END_OP_END);

        nvPusherAdjust(17);

        // Now blend subpicture rectangle with NV12 chroma plane
        
        if (!(nvInitCelsiusForMoCompNV12ChromaSubpictureCompositing()))
            return DDERR_GENERIC;

        dwDstOffset += pSurf_gbl->lPitch * (DWORD)pSurf_gbl->wHeight; // Change offset to destination chroma plane

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwDstOffset);

        nvPusherAdjust(4);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_PITCH | 0x40000);
        nvPushData(1, dwSurfacePitch);

        nvPusherAdjust(2);
        
        dwSrcOffset += pSurf_gbl->lPitch * (DWORD)pSurf_gbl->wHeight; // Change offset to source chroma plane

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
        nvPushData(1, dwSPSrcOffset);
        nvPushData(2, dwSrcOffset);

        nvPusherAdjust(3);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
        nvPushData(1, pSPSurf_gbl->lPitch << 18); // pitch << 2
        nvPushData(2, dwSurfacePitch << 16);

        nvPusherAdjust(3);

        SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x300000);

        // No HD scaled version because subpicture compositing is not used for HD 
        nvPushData(3, ((Y0 >> 1) << 16) | (X0 >> 1)); // source NV12 chroma in pel units
        nvPushData(4, (SPY0 << 16) | (SPX0)); // AVUY subpicture in pel units
        nvPushData(5, (Y0 << 17) | (((X0 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // destination NV12 chroma in quarter pel units

        nvPushData(6, ((Y1 >> 1) << 16) | (X0 >> 1));
        nvPushData(7, (SPY1 << 16) | (SPX0));
        nvPushData(8, (Y1 << 17) | (((X0 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

        nvPushData(9, ((Y1 >> 1) << 16) | (X1 >> 1));
        nvPushData(10, (SPY1 << 16) | (SPX1));
        nvPushData(11, (Y1 << 17) | (((X1 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

        nvPushData(12, ((Y0 >> 1) << 16) | (X1 >> 1));
        nvPushData(13, (SPY0 << 16) | (SPX1));
        nvPushData(14, (Y0 << 17) | (((X1 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

        nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_BEGIN_END | 0x40000);
        nvPushData(16, NV056_SET_BEGIN_END_OP_END);

        nvPusherAdjust(17);

        // Update reference count in case we get a status query
        lpDstSurfaceFlags->dwMCCompositeReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);

        nvPusherStart(TRUE);

        // Now format convert final NV12 blended rectangle to 422 overwriting current 422 pixels

        if (!(nvInitCelsiusForMoCompConversion()))
            return FALSE;

        tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR << 28) |
                 (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR << 24);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_FILTER(1) | 0x40000);
        nvPushData(1, tmpVal);

        nvPusherAdjust(2);

        // 420 LUMA portion of this surface starts at fpVidMem
        dwDstOffset -= pSurf_gbl->lPitch * (DWORD)pSurf_gbl->wHeight; // Change offset back to destination luma plane
        
        dwSurfacePitch = pSurf_gbl->lPitch;
        
        surfaceWidth = pSurf_gbl->wWidth;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
        nvPushData(1, dwDstOffset);

        // 420 CHROMA portion of this surface starts here
        dwDstOffset += (dwSurfacePitch * pSurf_gbl->wHeight);

        nvPushData(2, dwDstOffset);

        nvPusherAdjust(3);

        // 422 portion of this surface starts here
        dwDstOffset = lpDstSurfaceFlags->dwMCSurface422Offset;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwDstOffset);

        nvPusherAdjust(4);

        // Align surface pitch and set surface format and pitch
        dwSurfacePitch = ((surfaceWidth << 1) + 127) & ~127; // YUY2 destination is twice the surface width
        dwSurfacePitch |= (dwSurfacePitch << 16);
        
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_PITCH | 0x40000);
        nvPushData(1, dwSurfacePitch);

        nvPusherAdjust(2);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
        nvPushData(1, pSurf_gbl->lPitch << 16);
        nvPushData(2, pSurf_gbl->lPitch << 16);

        nvPusherAdjust(3);

        SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x300000);

        // No HD scaled version because subpicture compositing is not used for HD 
        nvPushData(3, ((Y0 >> 1) << 16) | 0);
        nvPushData(4, (Y0 << 16) | 0);
        nvPushData(5, (Y0 << 18) | ((0 - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

        nvPushData(6, ((Y1 >> 1) << 16) | 0); // CHROMA texture is in pel units and is only half the surface height
        nvPushData(7, (Y1 << 16) | 0); // LUMA texture is in pel units
        nvPushData(8, (Y1 << 18) | ((0 - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // destination is in quarter pel units

        nvPushData(9, ((Y1 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(10, (Y1 << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(11, (Y1 << 18) | (((surfaceWidth << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // each 32 bit write contains data for 2 422 pixels

        nvPushData(12, ((Y0 >> 1) << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(13, (Y0 << 16) | ((surfaceWidth + 1) >> 1));
        nvPushData(14, (Y0 << 18) | (((surfaceWidth << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));

        nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_BEGIN_END | 0x40000);
        nvPushData(16, NV056_SET_BEGIN_END_OP_END);

        nvPusherAdjust(17);

        // Update reference count so we know when we can display this surface
        lpDstSurfaceFlags->dwMCFormatCnvReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);

        nvPusherStart(TRUE);

        lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted |= 15;
    }

    nvPusherStart(TRUE);

    return DD_OK;

} /* nvMoCompCompositeNV12Subpicture */

#endif


/*
 * nvMoCompDisplaySurface
 *
 * Displays the current destination FOURCC_NV12 overlay surface
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvMoCompDisplaySurface(LPVOID lpInputData, DWORD dwSurfaceIndex)
{
    LPNVMCDISPLAYOVERLAYDATA    lpNvMCDisplayOverlayData = (LPNVMCDISPLAYOVERLAYDATA)lpInputData;
    NvNotification              *pPioFlipOverlayNotifier;
    HRESULT                     ddrval;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
#ifndef WINNT
    DWORD                       dwSavedNBData;
#endif
    DWORD                       dwVPPFlags;
    LPNVMCSURFACEFLAGS          lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];
    unsigned long               returnVal;

    dbgTracePush ("nvMoCompDisplaySurface");

    DDSTARTTICK(SURF4_FLIP);
    DDFLIPTICKS(FEFLIP);

    pPioFlipOverlayNotifier = pDriverData->pPioFlipOverlayNotifierFlat;

    if (dwSurfaceIndex >= 8) {
        DDENDTICK(SURF4_FLIP);
        dbgTracePop();
        return (DDERR_INVALIDPARAMS);
    }

    pSurf = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNV12Surface[dwSurfaceIndex];

    if (pSurf == 0) {
        DDENDTICK(SURF4_FLIP);
        dbgTracePop();
        return (DDERR_NOTFOUND);
    }

    pDriverData->dwMCDestinationSurface = (DWORD)pSurf;
    pSurf_gbl = pSurf->lpGbl;

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        DDENDTICK(SURF4_FLIP);
        dbgTracePop();
        return (DDERR_SURFACELOST);
    }

    // If UpdateOverlay hasn't been called yet then just return success
    if (GET_HALINFO()->ddCaps.dwCurrVisibleOverlays == 0) {
        DDENDTICK(SURF4_FLIP);
        dbgTracePop();
        return DD_OK;
    }

    if ((pDriverData->bMCOverlaySrcIsSingleField1080i) &&
        (lpNvMCDisplayOverlayData->dwMCFlipFlags & DDFLIP_ODD)) {
        DDENDTICK(SURF4_FLIP);
        dbgTracePop();
        return DD_OK;
    }

    while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpSurfaceFlags->dwMCSurfaceBase != pSurf_gbl->fpVidMem))
        lpSurfaceFlags++;

    // Make sure the surface has been format converted
    if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) &&
        (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 15);
    else if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD) &&
        ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 3);
    else if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) &&
        (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 12))
        nvMoCompConvertSurfaceFormat(pSurf_gbl->fpVidMem, TRUE, 12);

    // Has this surface finished format conversion yet ?
    if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                   lpSurfaceFlags->dwMCFormatCnvReferenceCount,
                                   1)) {
        DDENDTICK(SURF4_FLIP);
        dbgTracePop();
        return (DDERR_WASSTILLDRAWING);
    }

    // If this is a composited surface, is it done compositing yet ?
    if (lpSurfaceFlags->bMCFrameIsComposited) {
        if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                       lpSurfaceFlags->dwMCCompositeReferenceCount,
                                       1)) {
            DDENDTICK(SURF4_FLIP);
            dbgTracePop();
            return (DDERR_WASSTILLDRAWING);
        }
    }

    // If this is a filtered surface, is it done filtering yet ?
    if (lpSurfaceFlags->bMCFrameIsFiltered) {
        if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                       lpSurfaceFlags->dwMCTemporalFilterReferenceCount,
                                       1)) {
            DDENDTICK(SURF4_FLIP);
            dbgTracePop();
            return (DDERR_WASSTILLDRAWING);
        }
    }

    // Check if filtering is disabled on this surface for this display call only
    if (lpSurfaceFlags->bMCFrameType & TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE) {
        lpSurfaceFlags->bMCFrameIsFiltered = FALSE;
        lpSurfaceFlags->bMCFrameType &= ~TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE;
    }

    // Always display non-filtered portion of non-interleaved frames
    if ((lpNvMCDisplayOverlayData->dwMCFlipFlags & (DDFLIP_ODD | DDFLIP_EVEN)) == 0) {
        if (lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME)
            lpSurfaceFlags->bMCFrameIsFiltered = FALSE;
    }

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        DDENDTICK(SURF4_FLIP);
        dbgTracePop();
        return (DDERR_SURFACELOST);
    }

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvDDrawRecover();
        }
        else {
            DDENDTICK(SURF4_FLIP);
            dbgTracePop();
            return (DDERR_SURFACELOST);
        }
    }

#ifndef WINNT
    // check for VIA 82C598 north bridge
    if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
        // turn off write posting
        _asm {
                mov edx, 0cf8h
                mov eax, 80000840h
                out dx, eax
                mov edx, 0cfch
                mov eax, 0
                in  al, dx
                mov dwSavedNBData, eax
                and eax, 0FFFFFF7Fh
                out dx, al
        }
    }
#endif

    /* Limit flips to 1 at a time to work around NV4 RM limitation */
    ddrval = nvMoCompFrameStatus (pSurf_gbl->fpVidMem);

    /* Only reject flip requests when flipping frames (but allow fields to proceed) */
    if ((lpNvMCDisplayOverlayData->dwMCFlipFlags & (DDFLIP_ODD | DDFLIP_EVEN)) == 0) {
        if (ddrval != DD_OK) {
            if (lpNvMCDisplayOverlayData->dwMCFlipFlags & DDFLIP_WAIT) {
                while (ddrval != DD_OK) {
                    ddrval = nvMoCompFrameStatus (pSurf_gbl->fpVidMem);
                    if (ddrval != DD_OK) {
                        NV_SLEEPFLIP;
                    }
                }
            }
            else {
#ifndef WINNT
                // check for VIA 82C598 north bridge
                if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
                    // restore write posting value
                    _asm {
                            mov edx, 0cf8h
                            mov eax, 80000840h
                            out dx, eax
                            mov edx, 0cfch
                            mov eax, dwSavedNBData
                            out dx, al
                    }
                }
#endif
                DDENDTICK(SURF4_FLIP);
                dbgTracePop();
                return (DDERR_WASSTILLDRAWING);
            }
        }
    } else {
        if (ddrval != DD_OK) {
            NV_SLEEPFLIP;
            DDENDTICK(SURF4_FLIP);
            dbgTracePop();
            return (DDERR_WASSTILLDRAWING);
        }
    }

    /*
     * make sure we sync with other channels before writing put
     */
    getDC()->nvPusher.setSyncChannelFlag();

    getDC()->flipOverlayRecord.llPreviousFlipTime = getDC()->flipOverlayRecord.llFlipTime;
    QueryPerformanceCounter((LARGE_INTEGER *) &getDC()->flipOverlayRecord.llFlipTime);
    getDC()->flipOverlayRecord.fpPreviousFlipFrom = getDC()->flipOverlayRecord.fpFlipFrom;
    getDC()->flipOverlayRecord.fpFlipFrom = getDC()->flipOverlayRecord.fpFlipTo;
    getDC()->flipOverlayRecord.fpFlipTo = pSurf_gbl->fpVidMem;

    getDC()->flipOverlayRecord.bFlipFlag = TRUE;

//    dwVPPFlags = VPP_OPTIMIZE | VPP_PRESCALE; // only when rescaling already visible surface
    dwVPPFlags = VPP_MOCOMP;
    if (lpNvMCDisplayOverlayData->dwMCOverlayFlags & DDOVER_INTERLEAVED) {
        dwVPPFlags |= VPP_INTERLEAVED;
    }
    if (lpNvMCDisplayOverlayData->dwMCOverlayFlags & DDOVER_BOB) {
        dwVPPFlags |= VPP_BOB;
    }
    if (lpNvMCDisplayOverlayData->dwMCFlipFlags & DDFLIP_ODD) {
        dwVPPFlags |= VPP_ODD;
    } else if (lpNvMCDisplayOverlayData->dwMCFlipFlags & DDFLIP_EVEN) {
        dwVPPFlags |= VPP_EVEN;
    }

    returnVal = VppDoFlip(&(pDriverData->vpp),
                          VIDMEM_OFFSET(pSurf_gbl->fpVidMem),
                          pSurf_gbl->lPitch,
                          pSurf_gbl->wWidth,
                          pSurf_gbl->wHeight,
                          pSurf_gbl->ddpfSurface.dwFourCC,
                          dwVPPFlags);

#ifndef WINNT
    // check for VIA 82C598 north bridge
    if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
        // restore write posting value
        _asm {
                mov edx, 0cf8h
                mov eax, 80000840h
                out dx, eax
                mov edx, 0cfch
                mov eax, dwSavedNBData
                out dx, al
        }
    }
#endif

    DDENDTICK(SURF4_FLIP);
    dbgTracePop();
    return (returnVal ? DD_OK : DDERR_WASSTILLDRAWING);

} /* nvMoCompDisplaySurface */


/*
 * nvGetMoCompGuids
 *
 * Returns our motion comp GUID
 *
 */
DWORD __stdcall nvGetMoCompGuids( LPDDHAL_GETMOCOMPGUIDSDATA lpGMCGData)
{
    nvSetDriverDataPtrFromDDGbl(lpGMCGData->lpDD->lpGbl);

    if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
        lpGMCGData->ddRVal = DDERR_UNSUPPORTED;
        NvReleaseSemaphore(pDriverData);
        return (DDHAL_DRIVER_HANDLED);
    }

    lpGMCGData->dwNumGuids = 4;

    // Return DX VA Guids for MPEG2 Type A and B restricted profiles.
    if (lpGMCGData->lpGuids != NULL) {
        memcpy(lpGMCGData->lpGuids,     (GUID *)&DXVA_ModeMPEG2_A, sizeof(GUID));
        memcpy(lpGMCGData->lpGuids + 1, (GUID *)&DXVA_ModeMPEG2_B, sizeof(GUID));
        memcpy(lpGMCGData->lpGuids + 2, (GUID *)&DXVA_ModeMPEG2_C, sizeof(GUID));
        memcpy(lpGMCGData->lpGuids + 3, (GUID *)&DXVA_ModeMPEG2_D, sizeof(GUID));
    }

    NvReleaseSemaphore(pDriverData);
    lpGMCGData->ddRVal = DD_OK;

    return (DDHAL_DRIVER_HANDLED);

} /* nvGetMoCompGuids */



/*
 * nvGetMoCompFormats
 *
 * Returns our motion comp uncompressed surface format
 *
 */
DWORD __stdcall nvGetMoCompFormats( LPDDHAL_GETMOCOMPFORMATSDATA lpGMCFData)
{
    nvSetDriverDataPtrFromDDGbl(lpGMCFData->lpDD->lpGbl);

    // Assume we will succeed
    lpGMCFData->ddRVal = DD_OK;

    // If either MPEG2 A, B, C, or D guid is provided, return the
    // uncompressed overlay format we support.
    if (IsEqualIID_C(*(lpGMCFData->lpGuid), DXVA_ModeMPEG2_A) ||
        IsEqualIID_C(*(lpGMCFData->lpGuid), DXVA_ModeMPEG2_B) ||
        IsEqualIID_C(*(lpGMCFData->lpGuid), DXVA_ModeMPEG2_C) ||
        IsEqualIID_C(*(lpGMCFData->lpGuid), DXVA_ModeMPEG2_D)) {
        lpGMCFData->dwNumFormats = 1;

        if (lpGMCFData->lpFormats != NULL) {
            lpGMCFData->lpFormats[0].dwFlags = DDPF_FOURCC;
            lpGMCFData->lpFormats[0].dwFourCC = FOURCC_NV12;
            lpGMCFData->lpFormats[0].dwYUVBitCount = 12;
            lpGMCFData->lpFormats[0].dwYBitMask = 0;
            lpGMCFData->lpFormats[0].dwUBitMask = 0;
            lpGMCFData->lpFormats[0].dwVBitMask = 0;
            lpGMCFData->lpFormats[0].dwYUVAlphaBitMask = 0;
        }

    } else {
        lpGMCFData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
    }

    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);

} /* nvGetMoCompFormats */


/*
 * nvCreateMoComp
 *
 * Returns whether we can support a motion comp uncompressed surface of
 * the requested format and size
 *
 */
DWORD __stdcall nvCreateMoComp( LPDDHAL_CREATEMOCOMPDATA lpCMCData)
{
    BYTE                        bIndex;
    LPDXVA_ConnectMode          lpConnectMode;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPNVMCFRAMEDATA             lpNvMCFrameData;

    nvSetDriverDataPtrFromDDGbl(lpCMCData->lpDD->lpGbl);

    // Assume we will succeed
    lpCMCData->ddRVal = DD_OK;

    if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
        lpCMCData->ddRVal = DDERR_UNSUPPORTED;
    } else {
        if (IsEqualIID_C(*(lpCMCData->lpGuid), DXVA_ModeMPEG2_A) ||
            IsEqualIID_C(*(lpCMCData->lpGuid), DXVA_ModeMPEG2_B) ||
            IsEqualIID_C(*(lpCMCData->lpGuid), DXVA_ModeMPEG2_C) ||
            IsEqualIID_C(*(lpCMCData->lpGuid), DXVA_ModeMPEG2_D)) {

            if (lpCMCData->ddUncompPixelFormat.dwFourCC == FOURCC_NV12) {
                // Maximum NV10 motion comp texture width/height is 2046
                if (lpCMCData->dwUncompWidth > 2046)
                    lpCMCData->ddRVal = DDERR_TOOBIGWIDTH;

                if (lpCMCData->dwUncompHeight > 2046)
                    lpCMCData->ddRVal = DDERR_TOOBIGHEIGHT;
            } else {
                lpCMCData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
            }
        } else {
            lpCMCData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
        }
    }

    // Check connect mode data and make sure it matches what we want.
    lpConnectMode = (LPDXVA_ConnectMode)lpCMCData->lpData;

    if (IsEqualIID_C(*(lpCMCData->lpGuid), DXVA_ModeMPEG2_A)) {
        if ((lpConnectMode->guidMode        != DXVA_ModeMPEG2_A) ||
            (lpConnectMode->wRestrictedMode != DXVA_RESTRICTED_MODE_MPEG2_A))
            lpCMCData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
    }
    else if (IsEqualIID_C(*(lpCMCData->lpGuid), DXVA_ModeMPEG2_B)) {
        if ((lpConnectMode->guidMode        != DXVA_ModeMPEG2_B) ||
            (lpConnectMode->wRestrictedMode != DXVA_RESTRICTED_MODE_MPEG2_B))
            lpCMCData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
    }
    else if (IsEqualIID_C(*(lpCMCData->lpGuid), DXVA_ModeMPEG2_C)) {
        if ((lpConnectMode->guidMode        != DXVA_ModeMPEG2_C) ||
            (lpConnectMode->wRestrictedMode != DXVA_RESTRICTED_MODE_MPEG2_C))
            lpCMCData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
    }
    else if (IsEqualIID_C(*(lpCMCData->lpGuid), DXVA_ModeMPEG2_D)) {
        if ((lpConnectMode->guidMode        != DXVA_ModeMPEG2_D) ||
            (lpConnectMode->wRestrictedMode != DXVA_RESTRICTED_MODE_MPEG2_D))
            lpCMCData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
    }

    // If everything's okay, allocate a system memory buffer for
    // processing Macroblock control headers in Render.
    if (lpCMCData->ddRVal == DD_OK)
        pDriverData->dwDXVAConvertBuffer = (DWORD)GlobalAlloc(GMEM_FIXED, CONVERT_BUFFER_SIZE);

    // Initialize some things in pDriverData.
    pDriverData->dwDXVAFlags |= DXVA_MO_COMP_IN_PROGRESS;

    if (pDriverData->vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_DXVA_BACK_END_ALPHA_PREFERRED) {
        pDriverData->dwDXVAFlags |= DXVA_BACK_END_ALPHA_SELECTED;
    } else {
        pDriverData->dwDXVAFlags &= ~DXVA_BACK_END_ALPHA_SELECTED;
    }

    // Initialize some things in our NVMC surface.
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
    lpNvMCFrameData = (LPNVMCFRAMEDATA)VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);
    lpNvMCFrameData->dwDXVASubpicBuffer = 0;
    lpNvMCFrameData->dwDXVASubpicStride = 0;

    // In case the decoder doesn't call BeginFrame for a surface before using the
    // surface, we try to initialize our surface index arrays to reasonable values.
    // These should be overrwritten by BeginFrame later.
    for (bIndex = 0; bIndex < 8; bIndex++) {
        if (pDriverData->dwMCNV12Surface[bIndex]) {
            pDriverData->bDXVAIndexToNVIndex[bIndex] = bIndex;
            pDriverData->dwDXVAIndexToVidMem[bIndex] =
                ((LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNV12Surface[bIndex])->lpGbl->fpVidMem;
        }
        else {
            pDriverData->bDXVAIndexToNVIndex[bIndex] = 0;
            pDriverData->dwDXVAIndexToVidMem[bIndex] = 0;
        }
    }

    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);

} /* nvCreateMoComp */


/*
 * nvGetMoCompBuffInfo
 *
 * Returns our required motion comp buffer info
 *
 */
DWORD __stdcall nvGetMoCompBuffInfo( LPDDHAL_GETMOCOMPCOMPBUFFDATA lpGMCBData)
{
    nvSetDriverDataPtrFromDDGbl(lpGMCBData->lpDD->lpGbl);

    // Assume we will succeed
    lpGMCBData->ddRVal = DD_OK;

    if (IsEqualIID_C(*(lpGMCBData->lpGuid), DXVA_ModeMPEG2_A) ||
        IsEqualIID_C(*(lpGMCBData->lpGuid), DXVA_ModeMPEG2_B) ||
        IsEqualIID_C(*(lpGMCBData->lpGuid), DXVA_ModeMPEG2_C) ||
        IsEqualIID_C(*(lpGMCBData->lpGuid), DXVA_ModeMPEG2_D)) {
        if (lpGMCBData->ddPixelFormat.dwFourCC == FOURCC_NV12) {
            if (lpGMCBData->lpCompBuffInfo == NULL) {
                lpGMCBData->dwNumTypesCompBuffs = 16;
            }
            else if (lpGMCBData->dwNumTypesCompBuffs < 16) {
                lpGMCBData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
            }
            else {

                DWORD   dwPadWidth, dwPadHeight;
                DWORD   dwFlags, dwRGBBitCount, dwBytesPerPixel;
                DWORD   dwRBitMask, dwGBitMask, dwBBitMask;
                DWORD   dwScreenWidth;

                // We always fill in exactly 16 buffers structures for DXVA.
                lpGMCBData->dwNumTypesCompBuffs = 16;

                // Calculate RGB parameters for the first two types of surfaces.
                dwScreenWidth = GET_MODE_WIDTH();
                dwRGBBitCount = GET_MODE_BPP();
                dwBytesPerPixel = dwRGBBitCount / 8;
                switch (dwRGBBitCount)
                {
                case 8:
                    dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8;
                    dwRBitMask = dwGBitMask = dwBBitMask = 0;
                    break;

                case 16:
                    dwFlags = DDPF_RGB;
                    dwRBitMask = 0xf800;
                    dwGBitMask = 0x07e0;
                    dwBBitMask = 0x001f;
                    break;

                case 32:
                    dwFlags = DDPF_RGB;
                    dwRBitMask = 0x00ff0000;
                    dwGBitMask = 0x0000ff00;
                    dwBBitMask = 0x000000ff;
                    break;
                }

                // Initialize unused CompBuffInfo structure members to zero.
                memset(lpGMCBData->lpCompBuffInfo, 0, 16 * sizeof(DDMCBUFFERINFO));

                // The #0 CompBuffInfo structure isn't used by DXVA.
                // We'll use it to create an NVMC surface to assist our motion comp decoding.
                // The width and height are ignored by our driver.  Creating this surface
                // causes our CreateSurface32 function to initialize certain index arrays
                // related to the NV12 and NVID surfaces.
                lpGMCBData->lpCompBuffInfo[0].dwNumCompBuffers = 1;
                lpGMCBData->lpCompBuffInfo[0].dwWidthToCreate = 16;
                lpGMCBData->lpCompBuffInfo[0].dwHeightToCreate = 16;
                lpGMCBData->lpCompBuffInfo[0].dwBytesToAllocate = 256;
                lpGMCBData->lpCompBuffInfo[0].ddCompCaps.dwCaps = DDSCAPS_VIDEOMEMORY |
                                                                   DDSCAPS_LOCALVIDMEM |
                                                                   DDSCAPS_OVERLAY;
                lpGMCBData->lpCompBuffInfo[0].ddPixelFormat.dwFlags = DDPF_FOURCC;
                lpGMCBData->lpCompBuffInfo[0].ddPixelFormat.dwFourCC = FOURCC_NVMC;
                lpGMCBData->lpCompBuffInfo[0].ddPixelFormat.dwYUVBitCount = 16;

                // The #1 CompBuffInfo structure must be for a picture parameter buffer,
                // large enough to hold a DXVA_PictureParameters structure.  Here we create
                // it as an RGB surface, but the exact format doesn't matter.  We put it in
                // local video memory so Lock32 will wait until all previous decoding is done.
                dwPadWidth = (sizeof(DXVA_PictureParameters) + 7) & 0xFFFFFFF8L;
                dwPadHeight = 1;
                lpGMCBData->lpCompBuffInfo[1].dwNumCompBuffers = 1;
                lpGMCBData->lpCompBuffInfo[1].dwWidthToCreate = dwPadWidth / dwBytesPerPixel;
                lpGMCBData->lpCompBuffInfo[1].dwHeightToCreate = dwPadHeight;
                lpGMCBData->lpCompBuffInfo[1].dwBytesToAllocate = dwPadWidth * dwPadHeight;
                lpGMCBData->lpCompBuffInfo[1].ddCompCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
                                                                  DDSCAPS_SYSTEMMEMORY;
                lpGMCBData->lpCompBuffInfo[1].ddPixelFormat.dwFlags = dwFlags;
                lpGMCBData->lpCompBuffInfo[1].ddPixelFormat.dwFourCC = 0;
                lpGMCBData->lpCompBuffInfo[1].ddPixelFormat.dwRGBBitCount = dwRGBBitCount;
                lpGMCBData->lpCompBuffInfo[1].ddPixelFormat.dwRBitMask = dwRBitMask;
                lpGMCBData->lpCompBuffInfo[1].ddPixelFormat.dwGBitMask = dwGBitMask;
                lpGMCBData->lpCompBuffInfo[1].ddPixelFormat.dwBBitMask = dwBBitMask;
                lpGMCBData->lpCompBuffInfo[1].ddPixelFormat.dwRGBAlphaBitMask = 0;

                // The #2 CompBuffInfo structure must be for a macroblock control command buffer.
                // We want three of these, to match the three NVID buffers requested below.
                // Again we create them as RGB buffers but they're just a plain memory buffers.
                dwPadWidth = 1800;                                      // Enough for PAL DVD frame, plus padding.
                dwPadHeight = sizeof(DXVA_MBctrl_P_HostResidDiff_1);    // Max size of each macroblock structure.
                lpGMCBData->lpCompBuffInfo[2].dwNumCompBuffers = 3;
                lpGMCBData->lpCompBuffInfo[2].dwWidthToCreate = dwPadWidth / dwBytesPerPixel;
                lpGMCBData->lpCompBuffInfo[2].dwHeightToCreate = dwPadHeight;
                lpGMCBData->lpCompBuffInfo[2].dwBytesToAllocate = dwPadWidth * dwPadHeight;
                lpGMCBData->lpCompBuffInfo[2].ddCompCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
                                                                  DDSCAPS_SYSTEMMEMORY;
                lpGMCBData->lpCompBuffInfo[2].ddPixelFormat.dwFlags = dwFlags;
                lpGMCBData->lpCompBuffInfo[2].ddPixelFormat.dwFourCC = 0;
                lpGMCBData->lpCompBuffInfo[2].ddPixelFormat.dwRGBBitCount = dwRGBBitCount;
                lpGMCBData->lpCompBuffInfo[2].ddPixelFormat.dwRBitMask = dwRBitMask;
                lpGMCBData->lpCompBuffInfo[2].ddPixelFormat.dwGBitMask = dwGBitMask;
                lpGMCBData->lpCompBuffInfo[2].ddPixelFormat.dwBBitMask = dwBBitMask;
                lpGMCBData->lpCompBuffInfo[2].ddPixelFormat.dwRGBAlphaBitMask = 0;

                // The #3 CompBuffInfo structure must be for a residual difference buffer,
                // to hold host-based IDCT data.  They're allocated in local video memory.
                // If frame buffer is 32MB or greater, we want 3 buffers 1MB each, unless
                // TwinView is enabled, then only use 2 to leave room for Video Mirror.
                if (pDriverData->TotalVRAM >= 0x2000000) {

                    if ((pDriverData->dwDesktopState == NVTWINVIEW_STATE_DUALVIEW) ||
                        (pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE)) {

                        lpGMCBData->lpCompBuffInfo[3].dwNumCompBuffers = 2;
                        lpGMCBData->lpCompBuffInfo[3].dwWidthToCreate = 1024;
                        lpGMCBData->lpCompBuffInfo[3].dwHeightToCreate = 1024;
                        lpGMCBData->lpCompBuffInfo[3].dwBytesToAllocate = 1024L * 1024L;
                    }
                    else {
                        lpGMCBData->lpCompBuffInfo[3].dwNumCompBuffers = 3;
                        lpGMCBData->lpCompBuffInfo[3].dwWidthToCreate = 1024;
                        lpGMCBData->lpCompBuffInfo[3].dwHeightToCreate = 1024;
                        lpGMCBData->lpCompBuffInfo[3].dwBytesToAllocate = 1024L * 1024L;
                    }
                }

                // Else for 16MB NTSC-size DVD playback...
                else if (lpGMCBData->dwHeight <= 480) {

                    // For the special case of TwinView 1024x768x32, we allocate
                    // the minimum amount to allow Video Mirror to work OK.
                    if (((pDriverData->dwDesktopState == NVTWINVIEW_STATE_DUALVIEW) ||
                        (pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE)) &&
                        (dwScreenWidth == 1024) && (dwRGBBitCount == 32)) {

                        lpGMCBData->lpCompBuffInfo[3].dwNumCompBuffers = 1;
                        lpGMCBData->lpCompBuffInfo[3].dwWidthToCreate = 1024;
                        lpGMCBData->lpCompBuffInfo[3].dwHeightToCreate = 600;
                        lpGMCBData->lpCompBuffInfo[3].dwBytesToAllocate = 1024L * 600L;
                    }
                    else  if (dwScreenWidth == 800){
                        //DCT test mode.  DCT requires at least 6 NV12 surfaces
                        // allocate less memory for NVID surfaces.
                        lpGMCBData->lpCompBuffInfo[3].dwNumCompBuffers = 3;
                        lpGMCBData->lpCompBuffInfo[3].dwWidthToCreate = 1024;
                        lpGMCBData->lpCompBuffInfo[3].dwHeightToCreate = 600;
                        lpGMCBData->lpCompBuffInfo[3].dwBytesToAllocate = 3L*1024L * 600L;

                    }
                    else {
                        DWORD dwTotalMem, dwFreeMem, dwNumBuffer, dwBufferHeight,dwBufferWidth;
                        //FOURCC_NVSP surface 
                        dwTotalMem =(768 * 576 * 2) + (768 * 576 * 4) + 1024 +(768 * 576 * 2);
                        //add  4 NV12 and one extra (size based on Steven's DXVA Memory Layout SpreadSheets)
                        dwTotalMem += 1296640 * 5;
                        //buffer width
                        dwBufferWidth = 512;
                        if(pDriverData->VideoHeapFree >(int)dwTotalMem)
                        {
                            // Find out total free memory
                            dwFreeMem = pDriverData->VideoHeapFree - dwTotalMem;
                            //choose 512X2048 first
                            dwBufferHeight = 2048;
                            dwNumBuffer = dwFreeMem / (dwBufferWidth * dwBufferHeight);
                            if( dwNumBuffer < 2)
                            {
                                 dwNumBuffer = 2;                        //may be no memory to support DXVA
                                 dwBufferHeight = 1200;
                            }
                            else if(dwNumBuffer > 3)
                            {
                                //increas buffer size
                                dwNumBuffer = 3;
                                dwBufferHeight = dwFreeMem / ( dwBufferWidth * 3);
                                if(dwBufferHeight > 6144 )
                                    dwBufferHeight = 6144;
                                else
                                    dwBufferHeight &= ~511;

                            }

                        }
                        else
                        {
                            dwNumBuffer = 2;                        //no memory to support DXVA
                            dwBufferHeight = 1200;
                        }

                        lpGMCBData->lpCompBuffInfo[3].dwNumCompBuffers  = dwNumBuffer;
                        lpGMCBData->lpCompBuffInfo[3].dwWidthToCreate	= dwBufferWidth;
                        lpGMCBData->lpCompBuffInfo[3].dwHeightToCreate	= dwBufferHeight;
                        lpGMCBData->lpCompBuffInfo[3].dwBytesToAllocate = dwBufferWidth * dwBufferHeight * dwNumBuffer;
                    }		
                }
                // Else for 16MB PAL-size DVD playback, do the same thing but with
                // PAL-size buffers.
                else {
                    lpGMCBData->lpCompBuffInfo[3].dwNumCompBuffers = 2;
                    lpGMCBData->lpCompBuffInfo[3].dwWidthToCreate = 1024;
                    lpGMCBData->lpCompBuffInfo[3].dwHeightToCreate = 700;
                    lpGMCBData->lpCompBuffInfo[3].dwBytesToAllocate = 1024L * 700L;
                }

                lpGMCBData->lpCompBuffInfo[3].ddCompCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
                lpGMCBData->lpCompBuffInfo[3].ddPixelFormat.dwFlags = DDPF_FOURCC;
                lpGMCBData->lpCompBuffInfo[3].ddPixelFormat.dwFourCC = FOURCC_NVID;
                lpGMCBData->lpCompBuffInfo[3].ddPixelFormat.dwYUVBitCount = 8;
                lpGMCBData->lpCompBuffInfo[3].ddPixelFormat.dwYBitMask = 0;
                lpGMCBData->lpCompBuffInfo[3].ddPixelFormat.dwUBitMask = 0;
                lpGMCBData->lpCompBuffInfo[3].ddPixelFormat.dwVBitMask = 0;
                lpGMCBData->lpCompBuffInfo[3].ddPixelFormat.dwYUVAlphaBitMask = 0;

                // The #4 CompBuffInfo structure must be for a Deblocking Filter Control buffer.
                // We don't use this one.

                // The #5 CompBuffInfo structure must be for a Inverse Quantization Matrix buffer.
                // We don't use this one.

                // The #6 CompBuffInfo structure must be for a Slice Control buffer.
                // We don't use this one.

                // The #7 CompBuffInfo structure must be for a Bitstream Data buffer.
                // We don't use this one.

                // The #8 CompBuffInfo structure must be for a YUV Palette buffer.
                // We can put this one in system memory.
                dwPadWidth = 64;
                lpGMCBData->lpCompBuffInfo[8].dwNumCompBuffers = 2;
                lpGMCBData->lpCompBuffInfo[8].dwWidthToCreate = dwPadWidth / dwBytesPerPixel;
                lpGMCBData->lpCompBuffInfo[8].dwHeightToCreate = 1;
                lpGMCBData->lpCompBuffInfo[8].dwBytesToAllocate = dwPadWidth;
                lpGMCBData->lpCompBuffInfo[8].ddCompCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
                                                                  DDSCAPS_SYSTEMMEMORY;
                lpGMCBData->lpCompBuffInfo[8].ddPixelFormat.dwFlags = dwFlags;
                lpGMCBData->lpCompBuffInfo[8].ddPixelFormat.dwFourCC = 0;
                lpGMCBData->lpCompBuffInfo[8].ddPixelFormat.dwRGBBitCount = dwRGBBitCount;
                lpGMCBData->lpCompBuffInfo[8].ddPixelFormat.dwRBitMask = dwRBitMask;
                lpGMCBData->lpCompBuffInfo[8].ddPixelFormat.dwGBitMask = dwGBitMask;
                lpGMCBData->lpCompBuffInfo[8].ddPixelFormat.dwBBitMask = dwBBitMask;
                lpGMCBData->lpCompBuffInfo[8].ddPixelFormat.dwRGBAlphaBitMask = 0;

                // The #9 CompBuffInfo structure must be for an IA44 Alpha Blending buffer.
                // We can put this one in system memory.  Always make it 720x576, since
                // subpicture size may be independent of DVD video size.
                dwPadWidth  = 720;
                dwPadHeight = 576;
                lpGMCBData->lpCompBuffInfo[9].dwNumCompBuffers = 2;
                lpGMCBData->lpCompBuffInfo[9].dwWidthToCreate = dwPadWidth / dwBytesPerPixel;
                lpGMCBData->lpCompBuffInfo[9].dwHeightToCreate = dwPadHeight;
                lpGMCBData->lpCompBuffInfo[9].dwBytesToAllocate = dwPadWidth * dwPadHeight;
                lpGMCBData->lpCompBuffInfo[9].ddCompCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
                                                                  DDSCAPS_SYSTEMMEMORY;
                lpGMCBData->lpCompBuffInfo[9].ddPixelFormat.dwFlags = dwFlags;
                lpGMCBData->lpCompBuffInfo[9].ddPixelFormat.dwFourCC = 0;
                lpGMCBData->lpCompBuffInfo[9].ddPixelFormat.dwRGBBitCount = dwRGBBitCount;
                lpGMCBData->lpCompBuffInfo[9].ddPixelFormat.dwRBitMask = dwRBitMask;
                lpGMCBData->lpCompBuffInfo[9].ddPixelFormat.dwGBitMask = dwGBitMask;
                lpGMCBData->lpCompBuffInfo[9].ddPixelFormat.dwBBitMask = dwBBitMask;
                lpGMCBData->lpCompBuffInfo[9].ddPixelFormat.dwRGBAlphaBitMask = 0;

                // The #10 CompBuffInfo structure is for a DPXD Alpha Blending buffer.
                // We don't support DPXD, but we'll use this structure to create a
                // private NVSP surface that we need for subpicture blending.
                // The NVSP surface contains the following buffers:
                //   720x576x2  Compressed subpicture (currently not used for DXVA)
                //   720x576x4  Uncompressed subpicture in YUV32 format
                //   256x4      Lookup table buffer (currently no used for DXVA)
                //   720x576x2  Mixer buffer for subpicture blending
                // Note that the 720 width is padded to a 768 stride for dwBytesToAllocate.
                lpGMCBData->lpCompBuffInfo[10].dwNumCompBuffers = 1;
                lpGMCBData->lpCompBuffInfo[10].dwWidthToCreate = 720;
                lpGMCBData->lpCompBuffInfo[10].dwHeightToCreate = 576;
                lpGMCBData->lpCompBuffInfo[10].dwBytesToAllocate = (768L * 576 * 2) +
                                                                   (768L * 576 * 4) +
                                                                   1024L +
                                                                   (768L * 576 * 2);
                lpGMCBData->lpCompBuffInfo[10].ddCompCaps.dwCaps = DDSCAPS_VIDEOMEMORY |
                                                                   DDSCAPS_LOCALVIDMEM;
                lpGMCBData->lpCompBuffInfo[10].ddPixelFormat.dwFlags = DDPF_FOURCC;
                lpGMCBData->lpCompBuffInfo[10].ddPixelFormat.dwFourCC = FOURCC_NVSP;
                lpGMCBData->lpCompBuffInfo[10].ddPixelFormat.dwRGBBitCount = 8;
                lpGMCBData->lpCompBuffInfo[10].ddPixelFormat.dwRBitMask = 0;
                lpGMCBData->lpCompBuffInfo[10].ddPixelFormat.dwGBitMask = 0;
                lpGMCBData->lpCompBuffInfo[10].ddPixelFormat.dwBBitMask = 0;
                lpGMCBData->lpCompBuffInfo[10].ddPixelFormat.dwRGBAlphaBitMask = 0;

                // The #11 CompBuffInfo structure must be for a Highlight Data buffer.
                // We don't use this one.

                // The #12 CompBuffInfo structure must be for a DCCMD Data buffer.
                // We don't use this one.

                // The #13 CompBuffInfo structure must be for an Alpha Blending Combination buffer.
                // We can put this one in system memory.
                dwPadWidth = (sizeof(DXVA_BlendCombination) + 7) & 0xFFFFFFF8L;
                lpGMCBData->lpCompBuffInfo[13].dwNumCompBuffers = 2;
                lpGMCBData->lpCompBuffInfo[13].dwWidthToCreate = dwPadWidth / dwBytesPerPixel;
                lpGMCBData->lpCompBuffInfo[13].dwHeightToCreate = 1;
                lpGMCBData->lpCompBuffInfo[13].dwBytesToAllocate = dwPadWidth;
                lpGMCBData->lpCompBuffInfo[13].ddCompCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
                                                                  DDSCAPS_SYSTEMMEMORY;
                lpGMCBData->lpCompBuffInfo[13].ddPixelFormat.dwFlags = dwFlags;
                lpGMCBData->lpCompBuffInfo[13].ddPixelFormat.dwFourCC = 0;
                lpGMCBData->lpCompBuffInfo[13].ddPixelFormat.dwRGBBitCount = dwRGBBitCount;
                lpGMCBData->lpCompBuffInfo[13].ddPixelFormat.dwRBitMask = dwRBitMask;
                lpGMCBData->lpCompBuffInfo[13].ddPixelFormat.dwGBitMask = dwGBitMask;
                lpGMCBData->lpCompBuffInfo[13].ddPixelFormat.dwBBitMask = dwBBitMask;
                lpGMCBData->lpCompBuffInfo[13].ddPixelFormat.dwRGBAlphaBitMask = 0;

                // The #14 CompBuffInfo structure must be for a Picture Resampling Control buffer.
                // We don't use this one.

                // The #15 CompBuffInfo structure must be for a Read-Back buffer.
                // We don't use this one.
            }
        } else {
            lpGMCBData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
        }
    } else {
        lpGMCBData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
    }

    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);

} /* nvGetMoCompBuffInfo */


/*
 * nvGetInternalMoCompInfo
 *
 * Returns our internal motion comp video memory requirements
 *
 */
DWORD __stdcall nvGetInternalMoCompInfo( LPDDHAL_GETINTERNALMOCOMPDATA lpGIMCData)
{
    nvSetDriverDataPtrFromDDGbl(lpGIMCData->lpDD->lpGbl);

    // Assume we will succeed
    lpGIMCData->ddRVal = DD_OK;

    if (IsEqualIID_C(*(lpGIMCData->lpGuid), DXVA_ModeMPEG2_A) ||
        IsEqualIID_C(*(lpGIMCData->lpGuid), DXVA_ModeMPEG2_B) ||
        IsEqualIID_C(*(lpGIMCData->lpGuid), DXVA_ModeMPEG2_C) ||
        IsEqualIID_C(*(lpGIMCData->lpGuid), DXVA_ModeMPEG2_D)) {
        if (lpGIMCData->ddPixelFormat.dwFourCC == FOURCC_NV12) {
            lpGIMCData->dwScratchMemAlloc = 0;
        } else {
            lpGIMCData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
        }
    } else {
        lpGIMCData->ddRVal = DDERR_UNSUPPORTEDFORMAT;
    }

    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);

} /* nvGetInternalMoCompInfo */



/*
 * nvBeginMoCompFrame
 *
 * Prepares to perform motion compensation on a new frame
 *
 */
DWORD __stdcall nvBeginMoCompFrame( LPDDHAL_BEGINMOCOMPFRAMEDATA lpBMCFData)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPNVMCFRAMEDATA             lpNvMCFrameData;

    // Get pDriverData pointer.
    nvSetDriverDataPtrFromDDGbl(lpBMCFData->lpDD->lpGbl);

    // If the NVMC or NV12 surface has been destroyed, return Surface Lost error.
    if (!pDriverData->dwMCNVMCSurface    ||
        !pDriverData->dwMCNV12Surface[0] ||
        !lpBMCFData->lpDestSurface)
    {
        NvReleaseSemaphore(pDriverData);
        lpBMCFData->ddRVal = DDERR_SURFACELOST;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Check to see if the destination surface is still being displayed by the overlay.
    // If so, return an error code to force the decoder to try again later.
    lpBMCFData->ddRVal = DD_OK;
    if ( getFlipStatusOverlay(lpBMCFData->lpDestSurface->lpGbl->fpVidMem) != DD_OK) {

        NvReleaseSemaphore(pDriverData);
        lpBMCFData->ddRVal = DDERR_WASSTILLDRAWING;
        return (DDHAL_DRIVER_HANDLED);
    }

    // In the DX-VA spec, decoders need to call BeginFrame at least
    // once for each uncompressed buffer they plan to use, to associate
    // a buffer index with the uncompressed buffer DDraw surface handle.
    // We need to record these assocations to use later in Render.
    if (lpBMCFData->dwInputDataSize >= 2) {
        WORD    wDXVAIndex;

        // Extract index from lpInputData.
        wDXVAIndex = *((LPWORD)(lpBMCFData->lpInputData));

        // Save corresponding fpVidMem in our array.
        pDriverData->dwDXVAIndexToVidMem[wDXVAIndex]
            = (DWORD)lpBMCFData->lpDestSurface->lpGbl->fpVidMem;
    }

    // Calculate a linear pointer to the NVMC surface created by DShow.
    // We use this surface to store the NVMCFRAMEDATA structure.
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
    lpNvMCFrameData = (LPNVMCFRAMEDATA)VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

    // Initialize some things for the new frame.
    lpNvMCFrameData->bMCEndOfPicture = FALSE;
    pDriverData->bMCNewFrame         = TRUE;

    NvReleaseSemaphore(pDriverData);
    lpBMCFData->ddRVal = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
} /* nvBeginMoCompFrame */


/*
 * nvEndMoCompFrame
 *
 * Finishes motion compensation on current frame.
 *
 */
DWORD __stdcall nvEndMoCompFrame( LPDDHAL_ENDMOCOMPFRAMEDATA lpEMCFData)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPNVMCFRAMEDATA             lpNvMCFrameData;
    LPNVMCSURFACEFLAGS          lpSurfaceFlags;

    // Get pDriverData pointer.
    nvSetDriverDataPtrFromDDGbl(lpEMCFData->lpDD->lpGbl);

    // If the NVMC or NV12 surface has been destroyed, return Surface Lost error.
    if (!pDriverData->dwMCNVMCSurface ||
        !pDriverData->dwMCNV12Surface[0])
    {
        NvReleaseSemaphore(pDriverData);
        lpEMCFData->ddRVal = DDERR_SURFACELOST;
        return (DDHAL_DRIVER_HANDLED);
    }

    // If EndFrame is beging called after some macroblocks have been
    // decoded, we need to finish up a few things.  (This might be skipped
    // if BeginFrame/EndFrame have been called around an Alpha Blend
    // operation.)
    if (pDriverData->dwDXVAFlags & DXVA_PICTURE_IN_PROGRESS) {
        // Calculate a linear pointer to the NVMC surface created by DShow.
        // We use this surface to store the NVMCFRAMEDATA structure.
        pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
        lpNvMCFrameData = (LPNVMCFRAMEDATA)VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

        // If End of Picture flag wasn't already set by someone...
        // set End of Picture flag and make a call to Execute with no macroblocks
        // to finish up this frame.
        if (!lpNvMCFrameData->bMCEndOfPicture) {
            lpNvMCFrameData->bMCEndOfPicture = 1;
            nvMoCompExecute(lpNvMCFrameData->dwMCMultiMonID,
                            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION,
                            (LPVOID)(pDriverData->dwDXVAConvertBuffer + MACROBLOCKS_OFFSET),
                            0,
                            0);
        }

        // Clean up.  Make sure surface has been format converted
        lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];
        while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
               (lpSurfaceFlags->dwMCSurfaceBase != pDriverData->dwMCMostRecentlyDecodedSurfaceBase)) {
            lpSurfaceFlags++;
        }
        if (lpSurfaceFlags->dwMCSurfaceBase != 0) {
            if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) &&
                (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
                nvMoCompConvertSurfaceFormat(lpSurfaceFlags->dwMCSurfaceBase, TRUE, 15);
            else if (lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD) {
                if ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3)
                    nvMoCompConvertSurfaceFormat(lpSurfaceFlags->dwMCSurfaceBase, TRUE, 3);
            } else if (lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) {
                if (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 12)
                    nvMoCompConvertSurfaceFormat(lpSurfaceFlags->dwMCSurfaceBase, TRUE, 12);
            }
            if ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted == 15) &&
                (lpSurfaceFlags->bMCFrameIsFiltered == FALSE) &&
                (pDriverData->bMCTemporalFilterDisabled == FALSE))
                nvMoCompTemporalFilter(lpSurfaceFlags->dwMCSurfaceBase, pDriverData->bMCPercentCurrentField);
        }
    }

    // Reset flag to say this picture is done.
    pDriverData->dwDXVAFlags &= ~DXVA_PICTURE_IN_PROGRESS;

    lpEMCFData->ddRVal = DD_OK;
    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);
} /* nvEndMoCompFrame */


/*
 * nvRenderMoComp
 *
 * Performs motion compensation on a portion of a new frame.
 * Several buffers may be passed to this function at one time,
 * of the following types:
 *
 * 1 - Picture Parameter Buffer
 * 2 - Macroblock Control Command Buffer
 * 3 - Residual Difference Block Data Buffer (contains post-IDCT data)
 * 7 - DVD Subpicture Control Buffer
 *
 */
DWORD __stdcall nvRenderMoComp(LPDDHAL_RENDERMOCOMPDATA lpRMCData)
{
    DWORD                       dwRenderIndex;
    DWORD                       dwRetValue;
    LPDDMCBUFFERINFO            lpBufferInfo;
    LPBYTE                      lpBufferLinear;
    LPDXVA_BufferDescription    lpBufferHeader;
    DWORD                       dwMacroblockIndex = 255;
    DWORD                       dwDifferenceIndex = 255;
    DWORD                       dwFunction;
    DWORD                       dwDXVA_Func;
    DWORD                       dwQueryOrReplyFlag;
    DWORD                       dwReturn;

    // Get pDriverData pointer.
    nvSetDriverDataPtrFromDDGbl(lpRMCData->lpDD->lpGbl);

    // If the NVMC, NV12 or NVID surface has been destroyed, return Surface Lost error.
    if (!pDriverData->dwMCNVMCSurface    ||
        !pDriverData->dwMCNV12Surface[0] ||
        !pDriverData->dwMCNVIDSurface[0])
    {
        NvReleaseSemaphore(pDriverData);
        lpRMCData->ddRVal = DDERR_SURFACELOST;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Use the dwFunction value to decide what to do with the buffers
    // passed to us.
    dwFunction = lpRMCData->dwFunction;

    // First check to see if dwFunction contains a QueryOrReply function.
    dwDXVA_Func = readDXVA_QueryOrReplyFuncFunc(&dwFunction);
    dwQueryOrReplyFlag = readDXVA_QueryOrReplyFuncFlag(&dwFunction);
    if ((dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY) ||
        (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_LOCK_QUERY)) {
        switch (dwDXVA_Func) {
        case DXVA_PICTURE_DECODING_FUNCTION:
            dwReturn = nvRenderCheckDecodeConfig(lpRMCData);
            break;

        case DXVA_ALPHA_BLEND_DATA_LOAD_FUNCTION:
            dwReturn = nvRenderCheckAlphaLoadConfig(lpRMCData);
            break;

        case DXVA_ALPHA_BLEND_COMBINATION_FUNCTION:
            dwReturn = nvRenderCheckAlphaCombineConfig(lpRMCData);
            break;
        }

        // Return QueryOrReply response.
        NvReleaseSemaphore(pDriverData);
        return (DDHAL_DRIVER_HANDLED);
    }

    // For other functions, we may need to use our DXVA-To-NV index
    // translation tables.  Make sure they're up to date, since a
    // DDFlip call may swap info between the surface structures.
    nvUpdateDXVAIndexTable(pDriverData);

    // If dwFunction doesn't contain a QueryOrReply function, check it for
    // other functions.  Note that dwFunction can contain up to four 1-byte
    // commands, so they have to be checked one by one, starting with the
    // most significant byte.  (If dwFunction only has one function byte
    // in it, shift it into the top byte so we don't waste too much time.)
    if (dwFunction < 256)
        dwFunction <<= 24;
    while (dwFunction) {
        // Note: if dwFunction starts with FF, we'll fall through to the default
        // case, and it will be treated as a multi-byte command.
        dwDXVA_Func = (dwFunction >> 24);
        dwFunction <<= 8;

        switch (dwDXVA_Func) {

        case 0:     // Unused
            break;

        case DXVA_PICTURE_DECODING_FUNCTION:

            // We need to go through each of the buffers we've been passed and
            // process it according to the buffer type.  For Macroblock and
            // Residual buffers, record their buffer index at this point until
            // we have a match set - we need both to do the motion comp operation.
            lpBufferHeader   = (LPDXVA_BufferDescription)lpRMCData->lpInputData;
            lpBufferInfo = (LPDDMCBUFFERINFO)(lpRMCData->lpBufferInfo);
            for (dwRenderIndex = 0; dwRenderIndex < lpRMCData->dwNumBuffers; dwRenderIndex++) {
                lpBufferLinear   = nvGetLinearAddress(lpBufferInfo->lpCompSurface)
                                   + lpBufferInfo->dwDataOffset;

                switch (lpBufferHeader->dwTypeIndex) {

                case DXVA_PICTURE_DECODE_BUFFER:
                    // Extract the picture parameters for the next frame from
                    // the buffer we were sent.
                    dwRetValue = nvRenderGetPictureParams(lpRMCData, lpBufferInfo);
                    if (dwRetValue != DD_OK) {
                        lpRMCData->ddRVal = dwRetValue;
                        NvReleaseSemaphore(pDriverData);
                        return (DDHAL_DRIVER_HANDLED);
                    }
                    break;

                case DXVA_MACROBLOCK_CONTROL_BUFFER:
                    // Just record the pointer to the Macroblock buffer info for later.
                    dwMacroblockIndex = dwRenderIndex;
                    break;

                case DXVA_RESIDUAL_DIFFERENCE_BUFFER:
                    // Just record the pointer to the Difference buffer info for later.
                    dwDifferenceIndex = dwRenderIndex;
                    break;
                }

                // If we got both a Macroblock and a Difference buffer, we can perform the
                // motion compensation operation for this Render call.
                if ((dwMacroblockIndex != 255) && (dwDifferenceIndex != 255)) {
                    dwRetValue = nvRenderDoMotionComp(lpRMCData,
                                                      dwMacroblockIndex,
                                                      dwDifferenceIndex);
                    dwMacroblockIndex = 255;
                    dwDifferenceIndex = 255;
                }

                lpBufferInfo++;
                lpBufferHeader++;
            }
            break;


        case DXVA_ALPHA_BLEND_DATA_LOAD_FUNCTION:

            // We only check for YUV Palette and IA44 Surface buffers here.
            lpBufferHeader   = (LPDXVA_BufferDescription)lpRMCData->lpInputData;
            lpBufferInfo = (LPDDMCBUFFERINFO)(lpRMCData->lpBufferInfo);
            for (dwRenderIndex = 0; dwRenderIndex < lpRMCData->dwNumBuffers; dwRenderIndex++) {
                lpBufferLinear   = nvGetLinearAddress(lpBufferInfo->lpCompSurface)
                                   + lpBufferInfo->dwDataOffset;

                switch (lpBufferHeader->dwTypeIndex) {

                case DXVA_AYUV_BUFFER:
                    // Process the new YUV palette.
                    nvRenderGetYUVPalette(lpRMCData, dwRenderIndex);
                    break;

                case DXVA_IA44_SURFACE_BUFFER:
                    // Process the new subpicture bitmap.
                    nvRenderGetIA44Surface(lpRMCData, dwRenderIndex);
                    break;
                }

                lpBufferInfo++;
                lpBufferHeader++;
            }
            break;


        case DXVA_ALPHA_BLEND_COMBINATION_FUNCTION:

            // We only check for Alpha Blend Combination buffers here.
            // The Alpha Blend data should have been provided in earlier Render calls.
            lpBufferHeader   = (LPDXVA_BufferDescription)lpRMCData->lpInputData;
            lpBufferInfo = (LPDDMCBUFFERINFO)(lpRMCData->lpBufferInfo);
            for (dwRenderIndex = 0; dwRenderIndex < lpRMCData->dwNumBuffers; dwRenderIndex++) {
                lpBufferLinear   = nvGetLinearAddress(lpBufferInfo->lpCompSurface)
                                   + lpBufferInfo->dwDataOffset;

                switch (lpBufferHeader->dwTypeIndex) {

                case DXVA_ALPHA_BLEND_COMBINATION_BUFFER:
                    // Blend the subpicture bitmap into a video frame.
                    nvRenderAlphaBlendCombination(lpRMCData, dwRenderIndex);
                    break;
                }

                lpBufferInfo++;
                lpBufferHeader++;
            }
            break;
        }
    }

    // For non-Query functions, we are normally supposed to return a status
    // code in the lpPrivateOutputData area as a DWord.  Default to OK.
    if (lpRMCData->lpOutputData && (lpRMCData->dwOutputDataSize >= 4)) {
        *((LPDWORD)lpRMCData->lpOutputData) = 0;
    }

    // Return success code.
    lpRMCData->ddRVal = DD_OK;
    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);
} /* nvRenderMoComp */


/*
 * nvRenderCheckDecodeConfig
 *
 * Private function called by nvRenderMoComp.
 * Checks picture decoding configuration given by the decoder
 * to see if it's a configuration we support.
 *
 */
DWORD __stdcall nvRenderCheckDecodeConfig(LPDDHAL_RENDERMOCOMPDATA lpRMCData)
{
    LPDXVA_ConfigPictureDecode  lpConnectConfigInput;
    LPDXVA_ConfigPictureDecode  lpConnectConfigOutput;
    DXVA_ConfigPictureDecode    NVConnectConfig = {0};
    DWORD                   dwReturn;
    DWORD                   dwFunction;
    DWORD                   dwDXVA_Func;
    DWORD                   dwQueryOrReplyFlag;

    // Get QueryOrReply details.
    dwFunction = lpRMCData->dwFunction;
    dwDXVA_Func = readDXVA_QueryOrReplyFuncFunc(&dwFunction);
    dwQueryOrReplyFlag = readDXVA_QueryOrReplyFuncFlag(&dwFunction);

    // Set up pointers to input and output structures.
    lpConnectConfigInput  = (LPDXVA_ConfigPictureDecode)lpRMCData->lpInputData;
    lpConnectConfigOutput = (LPDXVA_ConfigPictureDecode)lpRMCData->lpOutputData;

    // Check input structure against our preferred structure.
    // Note that we don't care about bConfigMBcontrolRasterOrder
    // and bConfigIntraResidUnsigned, we'll accept either value.
    if ((lpConnectConfigInput->dwFunction                     != dwFunction)     ||
        (lpConnectConfigInput->guidConfigBitstreamEncryption  != DXVA_NoEncrypt) ||
        (lpConnectConfigInput->guidConfigMBcontrolEncryption  != DXVA_NoEncrypt) ||
        (lpConnectConfigInput->guidConfigResidDiffEncryption  != DXVA_NoEncrypt) ||
        (lpConnectConfigInput->bConfigBitstreamRaw            != 0)              ||
        (lpConnectConfigInput->bConfigResidDiffHost           != 1)              ||
        (lpConnectConfigInput->bConfigSpatialResid8           != 1)              ||
        (lpConnectConfigInput->bConfigResid8Subtraction       != 0)              ||
        (lpConnectConfigInput->bConfigSpatialHost8or9Clipping != 0)              ||
        (lpConnectConfigInput->bConfigSpatialResidInterleaved != 1)              ||
        (lpConnectConfigInput->bConfigResidDiffAccelerator    != 0)              ||
        (lpConnectConfigInput->bConfigHostInverseScan         != 0)              ||
        (lpConnectConfigInput->bConfigSpecificIDCT            != 0)              ||
        (lpConnectConfigInput->bConfig4GroupedCoefs           != 0)) {

        // Make the output a duplicate of our preferred structure.
        NVConnectConfig.dwFunction = dwFunction;
        memcpy((GUID *)&NVConnectConfig.guidConfigBitstreamEncryption,
               (GUID *)&DXVA_NoEncrypt,
               sizeof(GUID));
        memcpy((GUID *)&NVConnectConfig.guidConfigMBcontrolEncryption,
               (GUID *)&DXVA_NoEncrypt,
               sizeof(GUID));
        memcpy((GUID *)&NVConnectConfig.guidConfigResidDiffEncryption,
               (GUID *)&DXVA_NoEncrypt,
               sizeof(GUID));
        NVConnectConfig.bConfigResidDiffHost = 1;
        NVConnectConfig.bConfigSpatialResid8 = 1;
        NVConnectConfig.bConfigSpatialResidInterleaved = 1;
        NVConnectConfig.bConfigIntraResidUnsigned = 0;

        memcpy(lpRMCData->lpOutputData,
               (LPVOID)&NVConnectConfig,
               sizeof(DXVA_ConfigPictureDecode));

        lpRMCData->ddRVal = S_FALSE;
        if (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY)
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_PROBE_FALSE_PLUS;
        else
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_LOCK_FALSE_PLUS;
    }
    else {
        // Make the output a duplicate of the input structure.
        memcpy(lpRMCData->lpOutputData,
               lpRMCData->lpInputData,
               sizeof(DXVA_ConfigPictureDecode));

        // Record the setting of the Intra IDCT Unsigned flag.
        if (lpConnectConfigInput->bConfigIntraResidUnsigned)
            pDriverData->dwDXVAFlags |= DXVA_INTRA_IDCT_UNSIGNED;
        else
            pDriverData->dwDXVAFlags &= ~DXVA_INTRA_IDCT_UNSIGNED;

        lpRMCData->ddRVal = S_OK;
        if (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY)
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_PROBE_OK_COPY;
        else
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_LOCK_OK_COPY;
    }

    writeDXVA_QueryOrReplyFunc(&lpConnectConfigOutput->dwFunction, dwReturn, dwDXVA_Func);
    return dwReturn;
}  // nvRenderCheckDecodeConfig


/*
 * nvRenderCheckAlphaLoadConfig
 *
 * Private function called by nvRenderMoComp.
 * Checks alpha blend load configuration given by the decoder
 * to see if it's a configuration we support.
 *
 */
DWORD __stdcall nvRenderCheckAlphaLoadConfig(LPDDHAL_RENDERMOCOMPDATA lpRMCData)
{
    LPDXVA_ConfigAlphaLoad  lpConfigAlphaLoadInput;
    LPDXVA_ConfigAlphaLoad  lpConfigAlphaLoadOutput;
    DXVA_ConfigAlphaLoad    NVConfigAlphaLoad = {0};
    DWORD                   dwFunction;
    DWORD                   dwReturn;
    DWORD                   dwDXVA_Func;
    DWORD                   dwQueryOrReplyFlag;

    // Get QueryOrReply details.
    dwFunction = lpRMCData->dwFunction;
    dwDXVA_Func = readDXVA_QueryOrReplyFuncFunc(&dwFunction);
    dwQueryOrReplyFlag = readDXVA_QueryOrReplyFuncFlag(&dwFunction);

    // Set up pointers to input and output structures.
    lpConfigAlphaLoadInput  = (LPDXVA_ConfigAlphaLoad)lpRMCData->lpInputData;
    lpConfigAlphaLoadOutput = (LPDXVA_ConfigAlphaLoad)lpRMCData->lpOutputData;

    // Check input structure against our preferred structure.
    if ((lpConfigAlphaLoadInput->dwFunction      != dwFunction) ||
        (lpConfigAlphaLoadInput->bConfigDataType != DXVA_CONFIG_DATA_TYPE_IA44)) {

        // Make the output a duplicate of our preferred structure.
        NVConfigAlphaLoad.dwFunction = dwFunction;
        NVConfigAlphaLoad.bConfigDataType = DXVA_CONFIG_DATA_TYPE_IA44;     // AYUV Palette & IA44 Surface.
        memcpy(lpRMCData->lpOutputData,
               (LPVOID)&NVConfigAlphaLoad,
               sizeof(DXVA_ConfigAlphaLoad));

        lpRMCData->ddRVal = S_FALSE;
        if (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY)
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_PROBE_FALSE_PLUS;
        else
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_LOCK_FALSE_PLUS;
    }
    else {
        // Make the output a duplicate the input structure.
        memcpy(lpRMCData->lpOutputData,
               lpRMCData->lpInputData,
               sizeof(DXVA_ConfigAlphaLoad));

        lpRMCData->ddRVal = S_OK;
        if (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY)
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_PROBE_OK_COPY;
        else
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_LOCK_OK_COPY;
    }

    writeDXVA_QueryOrReplyFunc(&lpConfigAlphaLoadOutput->dwFunction, dwReturn, dwDXVA_Func);
    return dwReturn;
}  // nvRenderCheckAlphaLoadConfig


/*
 * nvRenderCheckAlphaCombineConfig
 *
 * Private function called by nvRenderMoComp.
 * Checks alpha blend combination configuration given by the decoder
 * to see if it's a configuration we support.
 *
 */
DWORD __stdcall nvRenderCheckAlphaCombineConfig(LPDDHAL_RENDERMOCOMPDATA lpRMCData)
{
    LPDXVA_ConfigAlphaCombine   lpConfigAlphaCombineInput;
    LPDXVA_ConfigAlphaCombine   lpConfigAlphaCombineOutput;
    DXVA_ConfigAlphaCombine     NVConfigAlphaCombine = {0};
    DWORD                       dwFunction;
    DWORD                       dwReturn;
    DWORD                       dwDXVA_Func;
    DWORD                       dwQueryOrReplyFlag;

    // Get QueryOrReply details.
    dwFunction = lpRMCData->dwFunction;
    dwDXVA_Func = readDXVA_QueryOrReplyFuncFunc(&dwFunction);
    dwQueryOrReplyFlag = readDXVA_QueryOrReplyFuncFlag(&dwFunction);

    // Set up pointers to input and output structures.
    lpConfigAlphaCombineInput  = (LPDXVA_ConfigAlphaCombine)lpRMCData->lpInputData;
    lpConfigAlphaCombineOutput = (LPDXVA_ConfigAlphaCombine)lpRMCData->lpOutputData;

    // Check to see if this is a valid Front End Alpha Blend configuration.
    // We only allow this configuration if video memory size is 32K or above,
    // because front end blending requires extra NV12 surfaces.
    if ((pDriverData->TotalVRAM >= 0x2000000) &&
        (lpConfigAlphaCombineInput->bConfigBlendType == DXVA_CONFIG_BLEND_TYPE_FRONT_BUFFER) &&
        (lpConfigAlphaCombineInput->bConfigOnlyUsePicDestRectArea == 1) &&
        (lpConfigAlphaCombineInput->bConfigWholePlaneAlpha == 0)) {

        // Make the output a duplicate of the input structure.
        memcpy(lpRMCData->lpOutputData,
               lpRMCData->lpInputData,
               sizeof(DXVA_ConfigAlphaCombine));

        // Reset flag to say we're going to use Front End blending.
        pDriverData->dwDXVAFlags &= ~DXVA_BACK_END_ALPHA_SELECTED;
        pDriverData->dwDXVAFlags &= ~DXVA_BACK_END_ALPHA_ENABLED;
        pDriverData->vpp.regOverlayMode3 &= ~NV4_REG_OVL_MODE3_DXVA_BACK_END_ALPHA_PREFERRED;

        lpRMCData->ddRVal = S_OK;
        if (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY)
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_PROBE_OK_COPY;
        else
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_LOCK_OK_COPY;
    }

    // Else check to see if this is a valid Back End Alpha Blend configuration.
    // We allow this configuration for all memory sizes.
    // Check input structure against our preferred structure
    // (we can accept either value of GraphicResizing.)
    else if ((lpConfigAlphaCombineInput->bConfigBlendType == DXVA_CONFIG_BLEND_TYPE_BACK_HARDWARE) &&
        (lpConfigAlphaCombineInput->bConfigPictureResizing        == 0) &&
        (lpConfigAlphaCombineInput->bConfigOnlyUsePicDestRectArea == 1) &&
        (lpConfigAlphaCombineInput->bConfigWholePlaneAlpha        == 0)) {

        // Make the output a duplicate of the input structure.
        memcpy(lpRMCData->lpOutputData,
               lpRMCData->lpInputData,
               sizeof(DXVA_ConfigAlphaCombine));

        // Set flag to say we're going to use Back End blending, initially disabled.
        pDriverData->dwDXVAFlags |= DXVA_BACK_END_ALPHA_SELECTED;
        pDriverData->dwDXVAFlags &= ~DXVA_BACK_END_ALPHA_ENABLED;
        pDriverData->vpp.regOverlayMode3 |= NV4_REG_OVL_MODE3_DXVA_BACK_END_ALPHA_PREFERRED;

        lpRMCData->ddRVal = S_OK;
        if (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY)
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_PROBE_OK_COPY;
        else
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_LOCK_OK_COPY;
    }

    // Otherwise return an error code and preferred configuration based on video
    // memory size.
    else if (pDriverData->TotalVRAM >= 0x2000000) {

        // Make the output a duplicate of our preferred structure for Front End blending.
        NVConfigAlphaCombine.dwFunction = dwFunction;
        NVConfigAlphaCombine.bConfigBlendType = DXVA_CONFIG_BLEND_TYPE_FRONT_BUFFER;
        NVConfigAlphaCombine.bConfigPictureResizing        = 1; // Support picture resize.
        NVConfigAlphaCombine.bConfigOnlyUsePicDestRectArea = 1; // Restrict destination rectangle.
        NVConfigAlphaCombine.bConfigGraphicResizing        = 1; // Support subpicture resizing.
        NVConfigAlphaCombine.bConfigWholePlaneAlpha        = 0; // No alpha override.
        NVConfigAlphaCombine.dwFunction = dwFunction;
        memcpy(lpRMCData->lpOutputData,
               (LPVOID)&NVConfigAlphaCombine,
               sizeof(DXVA_ConfigAlphaCombine));

        lpRMCData->ddRVal = S_FALSE;
        if (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY)
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_PROBE_FALSE_PLUS;
        else
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_LOCK_FALSE_PLUS;
    }

    else {

        // Make the output a duplicate of our preferred structure for Back End blending.
        NVConfigAlphaCombine.dwFunction = dwFunction;
        NVConfigAlphaCombine.bConfigBlendType = DXVA_CONFIG_BLEND_TYPE_BACK_HARDWARE;
        NVConfigAlphaCombine.bConfigPictureResizing        = 0; // Don't support picture resize.
        NVConfigAlphaCombine.bConfigOnlyUsePicDestRectArea = 1; // Restrict destination rectangle.
        NVConfigAlphaCombine.bConfigGraphicResizing        = 1; // Support subpicture resizing.
        NVConfigAlphaCombine.bConfigWholePlaneAlpha        = 0; // No alpha override.
        NVConfigAlphaCombine.dwFunction = dwFunction;
        memcpy(lpRMCData->lpOutputData,
               (LPVOID)&NVConfigAlphaCombine,
               sizeof(DXVA_ConfigAlphaCombine));

        lpRMCData->ddRVal = S_FALSE;
        if (dwQueryOrReplyFlag == DXVA_QUERYORREPLYFUNCFLAG_DECODER_PROBE_QUERY)
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_PROBE_FALSE_PLUS;
        else
            dwReturn = DXVA_QUERYORREPLYFUNCFLAG_ACCEL_LOCK_FALSE_PLUS;
    }

    writeDXVA_QueryOrReplyFunc(&lpConfigAlphaCombineOutput->dwFunction, dwReturn, dwDXVA_Func);
    return dwReturn;
}  // nvRenderCheckAlphaLoadConfig


/*
 * nvRenderGetPictureParams
 *
 * Private function called by nvRenderMoComp.
 * Extracts information for decoding a new frame from a
 * Picture Parameter structure passed to RenderMoComp.
 *
 */
DWORD __stdcall nvRenderGetPictureParams(LPDDHAL_RENDERMOCOMPDATA lpRMCData,
                                         LPDDMCBUFFERINFO lpPictureBuffer)
{
    LPBYTE                      lpBufferLinear;
    LPDXVA_PictureParameters    lpPictureParams;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPNVMCFRAMEDATA             lpNvMCFrameData;

    // Get pDriverData pointer.
    nvSetDriverDataPtrFromDDGbl(lpRMCData->lpDD->lpGbl);

    // Because our Picture Parameter buffer is in System memory,
    // decoder doesn't call our Lock32 before using it.  The
    // following may be required for some DVD decoders.
    getDC()->nvPusher.flush(TRUE, CPushBuffer::FLUSH_WITH_DELAY);

    // Calculate a pointer to the Picture Parameters structure in the buffer.
    lpBufferLinear   = nvGetLinearAddress(lpPictureBuffer->lpCompSurface)
                       + lpPictureBuffer->dwDataOffset;
    lpPictureParams  = (LPDXVA_PictureParameters)lpBufferLinear;

    // Calculate a linear pointer to the NVMC surface created by DShow.
    // We use this surface to store the NVMCFRAMEDATA structure.
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
    lpNvMCFrameData = (LPNVMCFRAMEDATA)VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

    // Make a copy of some Picture Parameters we need later.
    lpNvMCFrameData->wDXVAPicWidthInMB  = lpPictureParams->wPicWidthInMBminus1 + 1;
    lpNvMCFrameData->wDXVAPicHeightInMB = lpPictureParams->wPicHeightInMBminus1 + 1;

    // Extract the Picture Parameter values and use them to fill in our
    // NVMCFRAMEDATA structure, which is used by MoCompExecute.
    lpNvMCFrameData->dwMCNvExecute = 0;

#ifdef WINNT
    // For Win2K, return lpDD as the MultiMon parameter, since pDXShare is NULL.
    lpNvMCFrameData->dwMCMultiMonID = (DWORD)lpRMCData->lpDD;
#else
    // For Win98, return pDXShare as the MultiMon parameter for compatibility
    // with the nVidia proprietary motion comp interface.
    lpNvMCFrameData->dwMCMultiMonID = (DWORD)pDXShare;
#endif

    // Store the "raw" DXVA buffer indexes in the NVMC surface for now.
    // We'll convert them later to NV buffer indexes in RenderDoMotionComp.
    // Note: unused indexes may be set to 0xFFFF according to the DXVA spec.
    lpNvMCFrameData->wDXVADecodedPictureIndex     = min(lpPictureParams->wDecodedPictureIndex, 7);
    lpNvMCFrameData->wDXVAForwardRefPictureIndex  = min(lpPictureParams->wForwardRefPictureIndex, 7);
    lpNvMCFrameData->wDXVABackwardRefPictureIndex = min(lpPictureParams->wBackwardRefPictureIndex, 7);

    lpNvMCFrameData->bMCPictureStructure
        = lpPictureParams->bPicStructure;

    if (lpPictureParams->bPicIntra)
        lpNvMCFrameData->bMCPictureCodingType = PICTURE_TYPE_I;
    else if (lpPictureParams->bPicBackwardPrediction)
        lpNvMCFrameData->bMCPictureCodingType = PICTURE_TYPE_B;
    else
        lpNvMCFrameData->bMCPictureCodingType = PICTURE_TYPE_P;

    lpNvMCFrameData->bMCTopFieldFirst = 1;
    lpNvMCFrameData->bMCSecondField   = lpPictureParams->bSecondField;

    if (lpPictureParams->bPicStructure == PICTURE_STRUCTURE_FRAME)
        lpNvMCFrameData->bMCFrameType = 0;
    else
        lpNvMCFrameData->bMCFrameType = 1;

    lpNvMCFrameData->bMCEndOfPicture  = 0;

    NvReleaseSemaphore(pDriverData);
    return DD_OK;
}  // nvRenderGetPictureParams


/*
 * nvRenderDoMotionComp
 *
 * Private function called by nvRenderMoComp.
 * Does motion comp operation with data passed to Render.
 *
 */

DWORD __stdcall nvRenderDoMotionComp(LPDDHAL_RENDERMOCOMPDATA lpRMCData,
                                     DWORD dwMacroblockIndex,
                                     DWORD dwDifferenceIndex)
{
    LPBYTE                          lpDXVAMacroblockLinear;
    LPBYTE                          lpDXVAMacroblockPtr;
    LPDXVA_MBctrl_P_HostResidDiff_1 lpDXVAType;
    LPNVDECODEMACROBLOCK            lpNVMacroblockPtr;
    DWORD                           dwNumSrcMacroblocks, dwNumDstMacroblocks;
    DWORD                           dwIndex;
    LPDDRAWI_DDRAWSURFACE_LCL       pSurf_lcl;
    LPNVMCFRAMEDATA                 lpNvMCFrameData;
    LPBYTE                          lpIDCTLinear;
    WORD                            wNVIndex;
    DWORD                           dwIDCTOffset;
    DWORD                           dwIDCTIndex;
    WORD                            wDXVAPicWidthInMB;
    WORD                            wDXVAPicHeightInMB;
    BYTE                            bEndOfPicture = 0;
    LPDXVA_BufferDescription        lpDXVAMacroblockHeader;
    LPDXVA_BufferDescription        lpDXVADifferenceHeader;
    LPDXVA_BufferDescription        lpDataHeaders;
    LPDDMCBUFFERINFO                lpMacroblockBuffer;
    LPDDMCBUFFERINFO                lpDifferenceBuffer;
    DWORD                           dwCommand;
    DWORD                           dwStructureSize;

    // Get pDriverData pointer.
    nvSetDriverDataPtrFromDDGbl(lpRMCData->lpDD->lpGbl);

    // Set flag to say we're doing a picture decode.
    pDriverData->dwDXVAFlags |= DXVA_PICTURE_IN_PROGRESS;

    // Get pointers to the Macroblock and Difference buffer headers from lpInputData.
    lpDataHeaders = (LPDXVA_BufferDescription)lpRMCData->lpInputData;
    lpDXVAMacroblockHeader = lpDataHeaders + dwMacroblockIndex;
    lpDXVADifferenceHeader = lpDataHeaders + dwDifferenceIndex;

    // Calculate a pointer to the Macroblock data buffers.
    lpMacroblockBuffer = lpRMCData->lpBufferInfo + dwMacroblockIndex;
    lpDXVAMacroblockLinear = nvGetLinearAddress(lpMacroblockBuffer->lpCompSurface)
                             + lpMacroblockBuffer->dwDataOffset;
    lpDXVAMacroblockPtr    = lpDXVAMacroblockLinear;

    // Calculate the index of the Difference buffer.  This is normally used when we call
    // nvMoCompExecute.
    lpDifferenceBuffer = lpRMCData->lpBufferInfo + dwDifferenceIndex;
    for (wNVIndex = 0; wNVIndex < 4; wNVIndex++) {
        if ((DWORD)lpDifferenceBuffer->lpCompSurface == pDriverData->dwMCNVIDSurface[wNVIndex])
            break;
    }
    if (wNVIndex > 3)
        wNVIndex = 0;


    // Set up a pointer to the IDCT buffer we're going to use.
    lpDifferenceBuffer = lpRMCData->lpBufferInfo + dwDifferenceIndex;
    lpIDCTLinear = (LPBYTE)VIDMEM_ADDR(lpDifferenceBuffer->lpCompSurface->lpGbl->fpVidMem);

    // Calculate an offset to the beginning of the IDCT data in the surface.
    // This is needed for the 0.9.2 version of DXVA, not for 0.9.3.
    dwIDCTOffset = lpDifferenceBuffer->dwDataOffset;

    // Calculate a linear pointer to the NVMC surface created by DShow.
    // We use this surface to store the NVMCFRAMEDATA structure.
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
    lpNvMCFrameData = (LPNVMCFRAMEDATA)VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

    // Convert DXVA indexes we were given earlier into NV surface indexes
    // using our translation table.
    lpNvMCFrameData->bMCDestinationSurfaceIndex
        = pDriverData->bDXVAIndexToNVIndex[lpNvMCFrameData->wDXVADecodedPictureIndex];
    lpNvMCFrameData->bMCForwardSurfaceIndex
        = pDriverData->bDXVAIndexToNVIndex[lpNvMCFrameData->wDXVAForwardRefPictureIndex];
    lpNvMCFrameData->bMCBackwardSurfaceIndex
        = pDriverData->bDXVAIndexToNVIndex[lpNvMCFrameData->wDXVABackwardRefPictureIndex];

    // Set up a pointer to the beginning our Conversion buffer, where we'll
    // create NV-style macroblock headers from the DXVA-style headers we've
    // been given.
    lpNVMacroblockPtr = (LPNVDECODEMACROBLOCK)(pDriverData->dwDXVAConvertBuffer
                                               + MACROBLOCKS_OFFSET);

    // Convert Macroblock Buffer into the nVidia Macroblock format,
    // so we can pass it and the IDCT data surface to nvMoCompExecute.
    wDXVAPicWidthInMB  = lpNvMCFrameData->wDXVAPicWidthInMB;
    wDXVAPicHeightInMB = lpNvMCFrameData->wDXVAPicHeightInMB;

    // Calculate number of input macroblocks based on the data buffer size,
    // since skipped macroblocks may make dwNumMBsInBuffer unreliable.
    if (lpNvMCFrameData->bMCPictureCodingType == PICTURE_TYPE_I)
        dwStructureSize = sizeof(DXVA_MBctrl_I_HostResidDiff_1);
    else
        dwStructureSize = sizeof(DXVA_MBctrl_P_HostResidDiff_1);
    dwNumSrcMacroblocks = lpDXVAMacroblockHeader->dwDataSize / dwStructureSize;
    dwNumDstMacroblocks = 0;

    for (dwIndex = 0; dwIndex < dwNumSrcMacroblocks; dwIndex++) {

        // First take care of fields that are common for Intra and non-Intra
        // macroblocks.
        lpDXVAType = (LPDXVA_MBctrl_P_HostResidDiff_1)lpDXVAMacroblockPtr;

        lpNVMacroblockPtr->hOffset
            = (lpDXVAType->wMBaddress % wDXVAPicWidthInMB) * 16;
        lpNVMacroblockPtr->vOffset
            = (lpDXVAType->wMBaddress / wDXVAPicWidthInMB) * 16;

        // If this is the last macroblock in the frame, set a flag so we
        // know we should set the NV End of Picture byte.
        if (lpDXVAType->wMBaddress == wDXVAPicWidthInMB * wDXVAPicHeightInMB - 1)
            bEndOfPicture = 1;

        dwIDCTIndex = readDXVA_MBdataLocation(lpDXVAType);
        dwIDCTIndex >>= 4;
        lpNVMacroblockPtr->firstIDCTIndex = (WORD)dwIDCTIndex;

        if (readDXVA_FieldResidual(lpDXVAType))
            lpNVMacroblockPtr->DCTType = DCT_FIELD;
        else
            lpNVMacroblockPtr->DCTType = DCT_FRAME;

        lpNVMacroblockPtr->codedBlockPattern
            = lpDXVAType->wPatternCode >> 6;

        lpNVMacroblockPtr->overflowCodedBlockPattern
            = lpDXVAType->wPC_Overflow >> 6;

        // For Intra macroblocks, set motion entries to zero.
        if (readDXVA_IntraMacroblock(lpDXVAType)) {
            // No motion.
            lpNVMacroblockPtr->motionType = 0;

            lpNVMacroblockPtr->macroblockType = MB_INTRA;
        }

        // For Non-Intra macroblocks, set up the motion fields.
        else {

            // Get field selects.
            lpNVMacroblockPtr->fieldSelect[0][0] = readDXVA_MvertFieldSel_0(lpDXVAType);
            lpNVMacroblockPtr->fieldSelect[0][1] = readDXVA_MvertFieldSel_1(lpDXVAType);
            lpNVMacroblockPtr->fieldSelect[1][0] = readDXVA_MvertFieldSel_2(lpDXVAType);
            lpNVMacroblockPtr->fieldSelect[1][1] = readDXVA_MvertFieldSel_3(lpDXVAType);

            // Set up macroblock type.
            lpNVMacroblockPtr->macroblockType = 0;
            if (readDXVA_MotionForward(lpDXVAType))
                lpNVMacroblockPtr->macroblockType |= MB_MOTION_FORWARD;
            if (readDXVA_MotionBackward(lpDXVAType))
                lpNVMacroblockPtr->macroblockType |= MB_MOTION_BACKWARD;

            // Progressive frame cases.
            if (lpNvMCFrameData->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) {
                switch(readDXVA_MotionType(lpDXVAType)) {
                case 0:
                case 1:
                    lpNVMacroblockPtr->motionType = FRAME_STRUCTURE_MOTION_FIELD;
                    if(lpNVMacroblockPtr->macroblockType == MB_MOTION_FORWARD)
                    {
                        memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 4);
                        memcpy(lpNVMacroblockPtr->PMV[1][0], &lpDXVAType->MVector[2], 4);
                    }
                    else if(lpNVMacroblockPtr->macroblockType == MB_MOTION_BACKWARD)
                    {
                        memcpy(lpNVMacroblockPtr->PMV[0][1], &lpDXVAType->MVector[1], 4);
                        memcpy(lpNVMacroblockPtr->PMV[1][1], &lpDXVAType->MVector[3], 4);
                    }
                    else if(lpNVMacroblockPtr->macroblockType)
                    {
                        memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 16);
                    }
                    break;
                case 2:
                    lpNVMacroblockPtr->motionType = FRAME_STRUCTURE_MOTION_FRAME;

                    if(lpNVMacroblockPtr->macroblockType == MB_MOTION_FORWARD)
                    {
                        memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 4);
                    }
                    else if(lpNVMacroblockPtr->macroblockType == MB_MOTION_BACKWARD)
                    {
                        memcpy(lpNVMacroblockPtr->PMV[0][1], &lpDXVAType->MVector[1], 4);
                    }
                    else if(lpNVMacroblockPtr->macroblockType)
                    {
                        memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 8);
                    }

                    break;
                case 3:
                    lpNVMacroblockPtr->motionType = FRAME_STRUCTURE_MOTION_DP;
                    memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 16);

                    // Copy derived motion vectors for dual-prime.
                    lpNVMacroblockPtr->PMV[1][0][0] = lpNVMacroblockPtr->PMV[0][1][0];
                    lpNVMacroblockPtr->PMV[1][0][1] = lpNVMacroblockPtr->PMV[0][1][1];

                    // Zero out unused motion vectors and field selects.
                    lpNVMacroblockPtr->PMV[0][1][0] = 0;
                    lpNVMacroblockPtr->PMV[0][1][1] = 0;
                    memset(lpNVMacroblockPtr->fieldSelect, 0, 4);
                    break;
                }

           }

            // Interlaced frame cases.
            else {
                switch(readDXVA_MotionType(lpDXVAType)) {
                case 0:
                case 1:
                    lpNVMacroblockPtr->motionType = FIELD_STRUCTURE_MOTION_FIELD;
                    if(lpNVMacroblockPtr->macroblockType == MB_MOTION_FORWARD)
                    {
                        memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 4);
                    }
                    else if(lpNVMacroblockPtr->macroblockType == MB_MOTION_BACKWARD)
                    {
                        memcpy(lpNVMacroblockPtr->PMV[0][1], &lpDXVAType->MVector[1], 4);
                    }
                    else if(lpNVMacroblockPtr->macroblockType)
                    {
                        memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 8);
                    }
                    break;
                case 2:
                    lpNVMacroblockPtr->motionType = FIELD_STRUCTURE_MOTION_16X8;
                    if(lpNVMacroblockPtr->macroblockType == MB_MOTION_FORWARD)
                    {
                        memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 4);
                        memcpy(lpNVMacroblockPtr->PMV[1][0], &lpDXVAType->MVector[2], 4);
                    }
                    else if(lpNVMacroblockPtr->macroblockType == MB_MOTION_BACKWARD)
                    {
                        memcpy(lpNVMacroblockPtr->PMV[0][1], &lpDXVAType->MVector[1], 4);
                        memcpy(lpNVMacroblockPtr->PMV[1][1], &lpDXVAType->MVector[3], 4);

                    }
                    else if(lpNVMacroblockPtr->macroblockType)
                    {
                        memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 16);
                    }
                    break;
                case 3:
                    lpNVMacroblockPtr->motionType = FIELD_STRUCTURE_MOTION_DP;

                    memcpy(lpNVMacroblockPtr->PMV, lpDXVAType->MVector, 8);

                    // Copy derived motion vectors for dual-prime.
                    lpNVMacroblockPtr->PMV[1][0][0] = lpNVMacroblockPtr->PMV[0][1][0];
                    lpNVMacroblockPtr->PMV[1][0][1] = lpNVMacroblockPtr->PMV[0][1][1];

                    // Zero out unused motion vectors and field selects.
                    lpNVMacroblockPtr->PMV[0][1][0] = 0;
                    lpNVMacroblockPtr->PMV[0][1][1] = 0;
                    memset(lpNVMacroblockPtr->fieldSelect, 0, 4);
                    break;
                }

            }

            // If this is a No Motion case, convert it into a forward prediction
            // with null motion vectors.
            if (!lpNVMacroblockPtr->macroblockType) {
                lpNVMacroblockPtr->macroblockType = MB_MOTION_FORWARD;
                memset(lpNVMacroblockPtr->PMV, 0, 12);
            }
        }

        // Move to next NV macroblock header position in the Conversion buffer.
        lpNVMacroblockPtr++;
        dwNumDstMacroblocks++;

        // If there were any skipped macroblocks following this block, we
        // need to generate new macroblocks headers for them.  Each of
        // these new headers should have the same type as the real header
        // we were given, but with zero motion vectors and IDCT data.
        // Note: I frames should never have skipped macroblocks.
        if (readDXVA_MBskipsFollowing(lpDXVAType))
        {
            WORD    wSkipIndex;

            for (wSkipIndex = 0; wSkipIndex < readDXVA_MBskipsFollowing(lpDXVAType); wSkipIndex++)
            {
                // Generate each new header from the previous header.
                *lpNVMacroblockPtr = *(lpNVMacroblockPtr - 1);
                lpNVMacroblockPtr->hOffset += 0x10;
                lpNVMacroblockPtr->codedBlockPattern = 0;
                lpNVMacroblockPtr->overflowCodedBlockPattern = 0;
                lpNVMacroblockPtr++;
                dwNumDstMacroblocks++;
            }
        }

        // Move to next DXVA macroblock header.
        lpDXVAMacroblockPtr += dwStructureSize;
    }

    // If this frame contained the last macroblock of the picture, set the
    // End of Picture flag.
    lpNvMCFrameData->bMCEndOfPicture = bEndOfPicture;

    // Send the Conversion buffer to nvMoCompExecute, along with the NVID
    // surface containing the IDCT data.
    if (pDriverData->dwDXVAFlags & DXVA_INTRA_IDCT_UNSIGNED)
        dwCommand = EXECUTE_NON_PREBIASED_MOTION_COMPENSATION;
    else
        dwCommand = EXECUTE_MOTION_COMPENSATION;
    nvMoCompExecute(lpNvMCFrameData->dwMCMultiMonID,
                    dwCommand,
                    (LPVOID)(pDriverData->dwDXVAConvertBuffer + MACROBLOCKS_OFFSET),
                    dwNumDstMacroblocks * sizeof(NVDECODEMACROBLOCK),
                    wNVIndex);

    NvReleaseSemaphore(pDriverData);
    return DD_OK;
}  // nvRenderDoMotionComp


/*
 * nvRenderGetYUVPalette
 *
 * Private function called by nvRenderMoComp.
 * Takes the YUV subpicture palette provided by the decoder and uses it to generate
 * a new subpicture pixel lookup table, then use the lookup table to update the
 * current subpicture bitmap.
 *
 */
DWORD __stdcall nvRenderGetYUVPalette(LPDDHAL_RENDERMOCOMPDATA lpRMCData, DWORD dwRenderIndex)
{
    LPDDMCBUFFERINFO            lpPaletteBuffer;
    LPDXVA_BufferDescription    lpPaletteHeader;
    LPBYTE                      lpPaletteLinear;
    DWORD                       dwIndex, dwPaletteIndex;
    DWORD                       dwX, dwY;
    LPDDRAWI_DDRAWSURFACE_LCL   lpDstSurface;
    LPDWORD                     lpDst;
    DWORD                       dwDstStride;
    DWORD                       dwDstWidth, dwDstHeight;
    DWORD                       dwSrcOffset, dwDstOffset;
    DWORD                       dwAlpha;
    DWORD                       dwYUVArray[16];
    LPBYTE                      lpTemp;
    LPBYTE                      lpSrc;
    LPDWORD                     lpIA44toAYUVLookup;
    LPDXVA_AYUVsample           lpPaletteAYUV;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPNVMCFRAMEDATA             lpNvMCFrameData;
    DWORD                       dwSubpicBuffer, dwSubpicStride;

    // Get pDriverData pointer.
    nvSetDriverDataPtrFromDDGbl(lpRMCData->lpDD->lpGbl);

    // Calculate pointer to decompressed section of our NVSP surface.
    lpDstSurface = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVSPSurface;
    dwDstWidth   = (DWORD)lpDstSurface->lpGbl->wWidth;
    dwDstHeight  = (DWORD)lpDstSurface->lpGbl->wHeight;
    dwDstStride  = (DWORD)lpDstSurface->lpGbl->lPitch;
    lpDst        = (LPDWORD)(nvGetLinearAddress(lpDstSurface)
                             + 2 * dwDstStride * dwDstHeight);

    // Get pointer to the YUV Palette buffer header from lpInputData.
    lpPaletteHeader = (LPDXVA_BufferDescription)lpRMCData->lpInputData;
    lpPaletteHeader += dwRenderIndex;

    // Get pointer to the YUV Palette surface data.
    lpPaletteBuffer = lpRMCData->lpBufferInfo + dwRenderIndex;
    lpPaletteLinear = nvGetLinearAddress(lpPaletteBuffer->lpCompSurface)
                      + lpPaletteBuffer->dwDataOffset;
    lpPaletteAYUV   = (LPDXVA_AYUVsample)lpPaletteLinear;

    // Get pointer to our IA44 to AYUV lookup table.
    lpIA44toAYUVLookup = (LPDWORD)(pDriverData->dwDXVAConvertBuffer + LOOKUP_TABLE_OFFSET);

    // First convert the YUV palette into a YUV array in the correct order for us.
    lpTemp = (LPBYTE)dwYUVArray;
    for (dwIndex = 0; dwIndex < 16; dwIndex++) {
        *lpTemp++ = lpPaletteAYUV->bY_Value;
        *lpTemp++ = lpPaletteAYUV->bCbValue;
        *lpTemp++ = lpPaletteAYUV->bCrValue;
        *lpTemp++ = 0;
        lpPaletteAYUV++;
    }

    // Now use the YUV palette values to fill up the IA44 to AYUV lookup table.
    for (dwIndex = 0; dwIndex < 256; dwIndex++) {
        dwPaletteIndex = readDXVA_IA44index(&dwIndex);
        dwAlpha        = readDXVA_IA44alpha(&dwIndex);

        // Rescale dwAlpha from 0-15 to 0-255 range and shift into right position.
        dwAlpha        = ((dwAlpha * 255) / 15) << 24;
        *(lpIA44toAYUVLookup + dwIndex) = dwYUVArray[dwPaletteIndex] + dwAlpha;
    }

    // Calculate a linear pointer to the NVMC surface created by DShow.
    // We use this surface to store the NVMCFRAMEDATA structure.
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
    lpNvMCFrameData = (LPNVMCFRAMEDATA)VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);
    dwSubpicBuffer = lpNvMCFrameData->dwDXVASubpicBuffer;
    dwSubpicStride = lpNvMCFrameData->dwDXVASubpicStride;

    // Finally, use this new lookup table to re-convert the last IA44 bitmap
    // to AYUV format and copy it into the decompressed section of our NVSP
    // surface.  (Note that dwDstStride is in DWord units, not Byte units.)
    if (dwSubpicBuffer) {
        lpSrc = (LPBYTE)dwSubpicBuffer;
        for (dwY = 0; dwY < dwDstHeight; dwY++) {
            dwSrcOffset = dwY * dwSubpicStride;
            dwDstOffset = dwY * dwDstStride;
            for (dwX = 0; dwX < dwDstWidth; dwX++) {
                *(lpDst + dwDstOffset)
                    = *(lpIA44toAYUVLookup + *(lpSrc + dwSrcOffset));
                dwSrcOffset++;
                dwDstOffset++;
            }
        }
    }

    NvReleaseSemaphore(pDriverData);
    return DD_OK;
}  // nvRenderGetYUVPalette


/*
 * nvRenderGetIA44Surface
 *
 * Private function called by nvRenderMoComp.
 * Takes the IA44 subpicture bitmap provided by the decoder and uses it to generate
 * a new subpicture bitmap in NV AYUV format.
 *
 */
DWORD __stdcall nvRenderGetIA44Surface(LPDDHAL_RENDERMOCOMPDATA lpRMCData, DWORD dwRenderIndex)
{
    LPDXVA_BufferDescription    lpIA44Header;
    LPDDMCBUFFERINFO            lpIA44Buffer;
    LPBYTE                      lpIA44Linear;
    DWORD   dwX, dwY;
    LPDDRAWI_DDRAWSURFACE_LCL   lpDstSurface;
    LPDWORD                     lpDst;
    DWORD                       dwDstStride;
    DWORD                       dwDstWidth, dwDstHeight;
    DWORD                       dwSrcOffset, dwDstOffset;
    LPBYTE                      lpSrc;
    LPDWORD                     lpIA44toAYUVLookup;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPNVMCFRAMEDATA             lpNvMCFrameData;
    DWORD                       dwSubpicBuffer, dwSubpicStride;

    // Get pDriverData pointer.
    nvSetDriverDataPtrFromDDGbl(lpRMCData->lpDD->lpGbl);

    // Calculate pointer to decompressed section of our NVSP surface.
    lpDstSurface = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVSPSurface;
    dwDstWidth   = (DWORD)lpDstSurface->lpGbl->wWidth;
    dwDstHeight  = (DWORD)lpDstSurface->lpGbl->wHeight;
    dwDstStride  = (DWORD)lpDstSurface->lpGbl->lPitch;
    lpDst        = (LPDWORD)(nvGetLinearAddress(lpDstSurface)
                             + 2 * dwDstStride * dwDstHeight);
    memset(lpDst, 0, dwDstHeight * dwDstStride * 4);

    // Get pointer to the IA44 buffer header from lpInputData.
    lpIA44Header = (LPDXVA_BufferDescription)lpRMCData->lpInputData;
    lpIA44Header += dwRenderIndex;

    // Get pointer to the IA44 surface data.
    lpIA44Buffer = lpRMCData->lpBufferInfo + dwRenderIndex;
    lpIA44Linear = (LPBYTE)nvGetLinearAddress(lpIA44Buffer->lpCompSurface)
                       + lpIA44Buffer->dwDataOffset;

    // Get pointer to our IA44 to AYUV lookup table.
    lpIA44toAYUVLookup = (LPDWORD)(pDriverData->dwDXVAConvertBuffer + LOOKUP_TABLE_OFFSET);

    // Calculate a linear pointer to the NVMC surface created by DShow.
    // We use this surface to store the NVMCFRAMEDATA structure.
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
    lpNvMCFrameData = (LPNVMCFRAMEDATA)VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

    // Record subpicture linear address and stride for other functions.
    lpNvMCFrameData->dwDXVASubpicBuffer = (DWORD)lpIA44Linear;
    lpNvMCFrameData->dwDXVASubpicStride = lpIA44Header->dwStride;
    dwSubpicBuffer = lpNvMCFrameData->dwDXVASubpicBuffer;
    dwSubpicStride = lpNvMCFrameData->dwDXVASubpicStride;

    // Finally, use our lookup table to convert this IA44 bitmap to AYUV format
    // and copy it into the decompressed section of our NVSP surface.  (Note
    // that dwDstStride is in DWord units, not Byte units.)
    lpSrc = (LPBYTE)dwSubpicBuffer;
    for (dwY = 0; dwY < dwDstHeight; dwY++) {
        dwSrcOffset = dwY * dwSubpicStride;
        dwDstOffset = dwY * dwDstStride;
        for (dwX = 0; dwX < dwDstWidth; dwX++) {
            *(lpDst + dwDstOffset)
                = *(lpIA44toAYUVLookup + *(lpSrc + dwSrcOffset));
            dwSrcOffset++;
            dwDstOffset++;
        }
    }

    // If back-end alpha blending  enabled, merge this graphic with the frame
    // currently being displayed.
    if (pDriverData->dwDXVAFlags & DXVA_BACK_END_ALPHA_ENABLED)
        nvDXVABackEndAlphaBlend(getDC()->flipOverlayRecord.fpFlipTo);

    NvReleaseSemaphore(pDriverData);
    return DD_OK;
}  // nvRenderGetIA44Surface


/*
 * nvRenderAlphaBlendCombination
 *
 * Private function called by nvRenderMoComp.
 * Alpha blends subpicture bitmap into a decoded video frame.
 *
 */
DWORD __stdcall nvRenderAlphaBlendCombination(LPDDHAL_RENDERMOCOMPDATA lpRMCData, DWORD dwRenderIndex)
{
    LPDXVA_BufferDescription    lpAlphaHeader;
    LPDDMCBUFFERINFO            lpAlphaBuffer;
    LPBYTE                      lpAlphaLinear;
    LPDXVA_BlendCombination     lpDXVABlend;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPNVMCFRAMEDATA             lpNvMCFrameData;

    // Get pDriverData pointer.
    nvSetDriverDataPtrFromDDGbl(lpRMCData->lpDD->lpGbl);

    // Get pointer to NVMC surface with FrameData info.
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
    lpNvMCFrameData = (LPNVMCFRAMEDATA) VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

    // Get pointer to the Alpha Blend Combination buffer header from lpInputData.
    lpAlphaHeader = (LPDXVA_BufferDescription)lpRMCData->lpInputData;
    lpAlphaHeader += dwRenderIndex;

    // Get pointer to the Alpha Blend Combination surface data.
    lpAlphaBuffer = lpRMCData->lpBufferInfo + dwRenderIndex;
    lpAlphaLinear = nvGetLinearAddress(lpAlphaBuffer->lpCompSurface)
                       + lpAlphaBuffer->dwDataOffset;
    lpDXVABlend = (LPDXVA_BlendCombination)lpAlphaLinear;

    // If Back End alpha blending is selected, we just store the information
    // from the DXVABlendCombination structure for later when we decode or Flip.
    if (pDriverData->dwDXVAFlags & DXVA_BACK_END_ALPHA_SELECTED) {

        // Store new back-end blend parameters in our NVMC structure.
        lpNvMCFrameData->rcGraphicSourceRect      = lpDXVABlend->GraphicSourceRect;
        lpNvMCFrameData->rcGraphicDestinationRect = lpDXVABlend->GraphicDestinationRect;
        lpNvMCFrameData->bBlendOn                 = lpDXVABlend->bBlendOn;

        // If bBlendOn, set blend enabled flag and blend graphic into the current frame.
        if (lpDXVABlend->bBlendOn) {
            nvDXVABackEndAlphaBlend(getDC()->flipOverlayRecord.fpFlipTo);
            pDriverData->dwDXVAFlags |= DXVA_BACK_END_ALPHA_ENABLED;
        }
        else {
            pDriverData->dwDXVAFlags &= ~DXVA_BACK_END_ALPHA_ENABLED;
        }
    }

    // Else Front End alpha blending is selected, and we use the Execute
    // function to perform it on the selected frame.
    else {
        // We use the newer EXECUTE_DXVA_ALPHA_BLENDING function to do the
        // blending from the source buffer to the destination.
        nvMoCompExecute(lpNvMCFrameData->dwMCMultiMonID,
                        EXECUTE_DXVA_ALPHA_BLENDING,
                        (LPVOID)lpDXVABlend,
                        sizeof(DDHAL_RENDERMOCOMPDATA),
                        dwRenderIndex);
    }

    NvReleaseSemaphore(pDriverData);
    return DD_OK;
}  // nvRenderAlphaBlendCombination


/*
 * nvMoCompExecute
 *
 * Performs motion compensation on a portion of a new frame
 * (same functionality as nvRenderMoComp() but called directly by decoder as an
 *  interim Win9x interface solution until the HVA interface becomes available on Win9x)
 *
 * Functions supported: 0 = perform motion compensation operation
 *                      1 = temporally filter fields in destination frame
 *                      2 = decompress subpicture surface
 *                      3 = composite decompressed subpicture surface with destination surface
 *                      4 = display overlay surface
 *                      5 = format convert overlay surface
 *                      6 = perform non-prebiased motion compensation operation
 *
 */
DWORD __stdcall nvMoCompExecute(DWORD dwMCMultiMonID, DWORD dwFunction, LPVOID lpInputData, DWORD dwInputDataSize, DWORD dwSurfaceIndex)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
    DWORD                       status, dwIndex;
    DWORD                       dwBlockSize;
    LPNVMCFRAMEDATA             lpNvMCFrameData;
    LPNVMCSURFACEFLAGS          lpDstSurfaceFlags;
    LPNVMCSURFACEFLAGS          lpSurfaceFlags;

    // Set up our multimon environment
#ifdef WINNT
    // For Win2K, get pDriverData from lpDD passed as the MultiMon parameter.
    nvSetDriverDataPtrFromDDGbl(((LPDDRAWI_DIRECTDRAW_LCL)dwMCMultiMonID)->lpGbl);
#else
    pDXShare = (DISPDRVDIRECTXCOMMON *)(dwMCMultiMonID);
    NvAcquireSemaphore((GLOBALDATA *) (pDXShare->pDirectXData));
    pDriverData = (CDriverContext *) (pDXShare->pDirectXData);
#endif

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        if (!nvEnable32(NULL)) {
            NvReleaseSemaphore(pDriverData);
            return (DDERR_SURFACELOST);
        }
        pDriverData->dwMostRecentHWUser = MODULE_ID_NONE;
    }

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    if (pDriverData->dwMostRecentHWUser != MODULE_ID_DDRAW) {
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            if (!(nvInitKelvinForMoComp())) {
                NvReleaseSemaphore(pDriverData);
                return (DDERR_GENERIC);
            }
        }
        else {
            if (!(nvInitCelsiusForMoComp())) {
                NvReleaseSemaphore(pDriverData);
                return (DDERR_GENERIC);
            }
        }
    }

    // Check if this is the first call on a new frame/field
    if (pDriverData->bMCNewFrame) {

        pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
        lpNvMCFrameData = (LPNVMCFRAMEDATA) VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

        // Check if decoder updated it's data in the surface
        if (lpNvMCFrameData->bMCDestinationSurfaceIndex == 255) {
            NvReleaseSemaphore(pDriverData);
            return(DDERR_INVALIDPARAMS);
        }

        pDriverData->bMCNewFrame = FALSE;

        pDriverData->bMCPictureStructure = lpNvMCFrameData->bMCPictureStructure;
        pDriverData->bMCPictureCodingType = lpNvMCFrameData->bMCPictureCodingType;
        pDriverData->bMCTopFieldFirst = lpNvMCFrameData->bMCTopFieldFirst;
        pDriverData->bMCSecondField = lpNvMCFrameData->bMCSecondField;

        if (lpNvMCFrameData->bMCDestinationSurfaceIndex < 8)
            pDriverData->dwMCDestinationSurface = pDriverData->dwMCNV12Surface[lpNvMCFrameData->bMCDestinationSurfaceIndex];
        else
            pDriverData->dwMCDestinationSurface = pDriverData->dwMCNV12Surface[0];

        // Check for mode switches and full screen DOS interruptions
        if (pDriverData->dwMCDestinationSurface == 0) {
            NvReleaseSemaphore(pDriverData);
            return (DDERR_SURFACELOST);
        }

        if (lpNvMCFrameData->bMCForwardSurfaceIndex < 8)
            pDriverData->dwMCForwardSurface = pDriverData->dwMCNV12Surface[lpNvMCFrameData->bMCForwardSurfaceIndex];
        else
            pDriverData->dwMCForwardSurface = pDriverData->dwMCNV12Surface[0];

        if (pDriverData->bMCPictureCodingType == PICTURE_TYPE_B) {
            if (lpNvMCFrameData->bMCBackwardSurfaceIndex < 8)
                pDriverData->dwMCBackwardSurface = pDriverData->dwMCNV12Surface[lpNvMCFrameData->bMCBackwardSurfaceIndex];
            else
                pDriverData->dwMCBackwardSurface = pDriverData->dwMCNV12Surface[0];
            pDriverData->dwMCDualPrimeSurface = pDriverData->dwMCForwardSurface; // necessary for broken HDTV tuner
        } else {
            // For self referencing field structured P fields
            pDriverData->dwMCBackwardSurface = pDriverData->dwMCDestinationSurface;
            pDriverData->dwMCDualPrimeSurface = pDriverData->dwMCForwardSurface;
        }

        // Make sure none of the surface indexes were for surfaces that were never created.
        if (!pDriverData->dwMCDestinationSurface)
            pDriverData->dwMCDestinationSurface  = pDriverData->dwMCNV12Surface[0];
        if (!pDriverData->dwMCForwardSurface)
            pDriverData->dwMCForwardSurface      = pDriverData->dwMCNV12Surface[0];
        if (!pDriverData->dwMCBackwardSurface)
                pDriverData->dwMCBackwardSurface = pDriverData->dwMCNV12Surface[0];
        if (!pDriverData->dwMCDualPrimeSurface)
            pDriverData->dwMCDualPrimeSurface    = pDriverData->dwMCNV12Surface[0];

        pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
        pSurf_gbl = pSurf_lcl->lpGbl;

        pDriverData->dwMCDestinationSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_NO_OPERATION | 0x40000);
            nvPushData(1, 0);
            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, pDriverData->dwMCDestinationSurfaceOffset);
            nvPusherAdjust(4);

            nvPusherStart(TRUE);
        } else {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_NO_OPERATION | 0x40000);
            nvPushData(1, 0);
            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, pDriverData->dwMCDestinationSurfaceOffset);
            nvPusherAdjust(4);

            nvPusherStart(TRUE);
        }

        // Initialize frame specific data if this is indeed a new frame
        if ((dwFunction == EXECUTE_MOTION_COMPENSATION) ||
            (dwFunction == EXECUTE_NON_PREBIASED_MOTION_COMPENSATION)) {

            lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

            while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
                   (lpDstSurfaceFlags->dwMCSurfaceBase != pSurf_gbl->fpVidMem))
                lpDstSurfaceFlags++;

            if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) {
                if (pDriverData->dwMCMostRecentlyDecodedSurfaceBase != pSurf_gbl->fpVidMem)
                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted = 0;
                else if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 15)
                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted = 0;
            } else if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD) {
                if ((pDriverData->dwMCMostRecentlyDecodedSurfaceBase != pSurf_gbl->fpVidMem) ||
                    (lpDstSurfaceFlags->bMCPictureStructure != pDriverData->bMCPictureStructure))
                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted &= ~3;
                else if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 3)
                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted &= ~3;
            } else {
                if ((pDriverData->dwMCMostRecentlyDecodedSurfaceBase != pSurf_gbl->fpVidMem) ||
                    (lpDstSurfaceFlags->bMCPictureStructure != pDriverData->bMCPictureStructure))
                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted &= ~12;
                else if (lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted == 12)
                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted &= ~12;
            }

            // Initialize surface flags in video memory
            lpDstSurfaceFlags->bMCPictureStructure = pDriverData->bMCPictureStructure;
            lpDstSurfaceFlags->bMCPictureCodingType = pDriverData->bMCPictureCodingType;
            lpDstSurfaceFlags->bMCTopFieldFirst = pDriverData->bMCTopFieldFirst;
            lpDstSurfaceFlags->bMCSecondField = pDriverData->bMCSecondField;
            lpDstSurfaceFlags->bMCFrameType = 0;
            lpDstSurfaceFlags->bMCFrameIsFiltered = FALSE;
            lpDstSurfaceFlags->bMCFrameIsComposited = FALSE;

            pDriverData->dwMCMostRecentlyDecodedSurfaceBase = pSurf_gbl->fpVidMem;
            pDriverData->bMCMostRecentlyDecodedPictureStructure = pDriverData->bMCPictureStructure;

            // Make sure this surface is not currently visible
            if (pSurf_lcl->lpGbl->wWidth < 1024) { // If DVD
                while (nvMoCompFrameStatus (pSurf_lcl->lpGbl->fpVidMem) != DD_OK)
                    NV_SLEEP1; // Don't change (bmac)
            } // else { // If HDTV (don't even check, especially on NV11)
//                while (nvMoCompFlipStatus(pSurf_lcl->lpGbl->fpVidMem) != DD_OK)
//                    NV_SLEEP; // Don't change (bmac)
//            }
            
            if (pDriverData->bMCPictureCodingType != PICTURE_TYPE_I) {
                pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCForwardSurface;

                lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

                while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
                       (lpSurfaceFlags->dwMCSurfaceBase != pSurf_lcl->lpGbl->fpVidMem))
                    lpSurfaceFlags++;

                pDriverData->bMCForwardPictureStructure = lpSurfaceFlags->bMCPictureStructure;

                pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface;

                lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

                while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
                       (lpSurfaceFlags->dwMCSurfaceBase != pSurf_lcl->lpGbl->fpVidMem))
                    lpSurfaceFlags++;

                pDriverData->bMCBackwardPictureStructure = lpSurfaceFlags->bMCPictureStructure;
            }
        }

    } else {

        pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
        lpNvMCFrameData = (LPNVMCFRAMEDATA) VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

        // Check if decoder updated it's data in the surface
        if (lpNvMCFrameData->bMCDestinationSurfaceIndex == 255) {
            NvReleaseSemaphore(pDriverData);
            return(DDERR_INVALIDPARAMS);
            }

        pDriverData->bMCPictureStructure = lpNvMCFrameData->bMCPictureStructure;
        pDriverData->bMCPictureCodingType = lpNvMCFrameData->bMCPictureCodingType;
        pDriverData->bMCTopFieldFirst = lpNvMCFrameData->bMCTopFieldFirst;
        pDriverData->bMCSecondField = lpNvMCFrameData->bMCSecondField;

        if (lpNvMCFrameData->bMCDestinationSurfaceIndex < 8)
            pDriverData->dwMCDestinationSurface = pDriverData->dwMCNV12Surface[lpNvMCFrameData->bMCDestinationSurfaceIndex];
        else
            pDriverData->dwMCDestinationSurface = pDriverData->dwMCNV12Surface[0];

        // Check for mode switches and full screen DOS interruptions
        if (pDriverData->dwMCDestinationSurface == 0) {
            NvReleaseSemaphore(pDriverData);
            return (DDERR_SURFACELOST);
        }
    }

    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    pSurf_gbl = pSurf_lcl->lpGbl;

    pDriverData->dwMCDestinationSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

    while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpDstSurfaceFlags->dwMCSurfaceBase != pSurf_gbl->fpVidMem))
        lpDstSurfaceFlags++;

    pDriverData->bMCCurrentExecuteFunction = (BYTE)dwFunction;

    if ((dwFunction == EXECUTE_MOTION_COMPENSATION) ||
        (dwFunction == EXECUTE_NON_PREBIASED_MOTION_COMPENSATION)) {
        LPNVDECODEMACROBLOCK pMacroblocks = (LPNVDECODEMACROBLOCK)lpInputData;
        long dwNumMacroblocks;
        long macroblockIncr = 256;

        // Update reference count on previous IDCT surface to indicate we have switched away from it
        if (pDriverData->dwMCIDCTSurface != 0) {
            for (dwIndex=0; dwIndex<4; dwIndex++) {
                if (pDriverData->dwMCIDCTSurface == pDriverData->dwMCNVIDSurface[dwIndex]) {
                    break;
                }
            }

            if (dwIndex >= 4)
                dwIndex = 0;

            if ((dwIndex < 4) && (dwSurfaceIndex != dwIndex)) {
                pDriverData->dwMCNVIDSurfaceReferenceCount[dwIndex] = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);
                nvPusherStart(TRUE);
            }
        }

        if (dwSurfaceIndex < 4)
            pDriverData->dwMCIDCTSurface = pDriverData->dwMCNVIDSurface[dwSurfaceIndex];
        else
            pDriverData->dwMCIDCTSurface = pDriverData->dwMCNVIDSurface[0];

        pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCForwardSurface;
        pSurf_gbl = pSurf_lcl->lpGbl;

        pDriverData->dwMCForwardSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

        pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface;
        pSurf_gbl = pSurf_lcl->lpGbl;

        pDriverData->dwMCBackwardSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

        pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCIDCTSurface;
        pSurf_gbl = pSurf_lcl->lpGbl;

        if (pSurf_gbl->ddpfSurface.dwFourCC != FOURCC_NVID) {
            NvReleaseSemaphore(pDriverData);
            return (DDERR_INVALIDPARAMS);
        }

        dwBlockSize = pSurf_gbl->lPitch * pSurf_gbl->wHeight;
        dwBlockSize |= 0xFF; // Context DMA must end on 256 byte boundary (enough extra memory was allocated for this correction)


        for (dwIndex=0; dwIndex<4; dwIndex++) {
            if (pSurf_gbl->fpVidMem == pDriverData->dwMCIDCTAGPSurfaceBase[dwIndex]) {
                break;
            }
        }

        if (dwIndex >= 4)
            dwIndex = 0;

        if (pDriverData->dwMCIDCTAGPCtxDmaSize[dwIndex] == 0) {
            pDriverData->dwMCCurrentCelsiusIDCTCtxDma = 0;
            status = NvRmAllocContextDma(pDriverData->dwRootHandle,
                                         NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY+dwIndex,
                                         NV01_CONTEXT_DMA,
                                         DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_ONLY) |
                                         DRF_DEF(OS03, _FLAGS, _LOCKED, _ALWAYS) |
                                         DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                                         (PVOID) VIDMEM_ADDR(pSurf_gbl->fpVidMem),
                                         dwBlockSize);
            if (status != NVOS03_STATUS_SUCCESS) {
                NvReleaseSemaphore(pDriverData);
                return (DDERR_SURFACELOST);
            }

            pDriverData->dwMCIDCTAGPCtxDmaSize[dwIndex] = dwBlockSize;
        }
        // Update IDCT context Dma
        if (pDriverData->dwMCCurrentCelsiusIDCTCtxDma != (NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY+dwIndex)) {
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_SET_CONTEXT_DMA_B | 0x40000);
                nvPushData(1, NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY+dwIndex);
                nvPusherAdjust(2);
                nvPusherStart(TRUE);
            } else {
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_SET_CONTEXT_DMA_B | 0x40000);
                nvPushData(1, NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY+dwIndex);
                nvPusherAdjust(2);
                nvPusherStart(TRUE);
            }
            pDriverData->dwMCCurrentCelsiusIDCTCtxDma = NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY+dwIndex;
        }

        dwNumMacroblocks = dwInputDataSize / sizeof(NVDECODEMACROBLOCK);

        if ((dwNumMacroblocks * sizeof(NVDECODEMACROBLOCK)) != dwInputDataSize) {
            NvReleaseSemaphore(pDriverData);
            return (DDERR_INVALIDPARAMS);
        }

        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) {
                while (dwNumMacroblocks > 0) {
                    if (dwNumMacroblocks > macroblockIncr) {
                        if (!(nvMoCompProcessKelvinFrameMacroblocks(pMacroblocks,
                                                                    macroblockIncr,
                                                                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted))) {
                            NvReleaseSemaphore(pDriverData);
                            return (DDERR_GENERIC);
                        }
                    } else {
                        if (!(nvMoCompProcessKelvinFrameMacroblocks(pMacroblocks,
                                                                    dwNumMacroblocks,
                                                                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted))) {
                            NvReleaseSemaphore(pDriverData);
                            return (DDERR_GENERIC);
                        }
                    }
                    pMacroblocks += macroblockIncr;
                    dwNumMacroblocks -= macroblockIncr;
                }
            } else {
                while (dwNumMacroblocks > 0) {
                    if (dwNumMacroblocks > macroblockIncr) {
                        if (!(nvMoCompProcessKelvinFieldMacroblocks(pMacroblocks,
                                                                    macroblockIncr,
                                                                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted))) {
                            NvReleaseSemaphore(pDriverData);
                            return (DDERR_GENERIC);
                        }
                    } else {
                        if (!(nvMoCompProcessKelvinFieldMacroblocks(pMacroblocks,
                                                                    dwNumMacroblocks,
                                                                    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted))) {
                            NvReleaseSemaphore(pDriverData);
                            return (DDERR_GENERIC);
                        }
                    }
                    pMacroblocks += macroblockIncr;
                    dwNumMacroblocks -= macroblockIncr;
                }
            }
        } else {
            if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) {
                while (dwNumMacroblocks > 0) {
                    if (dwNumMacroblocks > macroblockIncr) {
                        if (!(nvMoCompProcessCelsiusFrameMacroblocks(pMacroblocks,
                                                                     macroblockIncr,
                                                                     lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted))) {
                            NvReleaseSemaphore(pDriverData);
                            return (DDERR_GENERIC);
                        }
                    } else {
                        if (!(nvMoCompProcessCelsiusFrameMacroblocks(pMacroblocks,
                                                                     dwNumMacroblocks,
                                                                     lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted))) {
                            NvReleaseSemaphore(pDriverData);
                            return (DDERR_GENERIC);
                        }
                    }
                    pMacroblocks += macroblockIncr;
                    dwNumMacroblocks -= macroblockIncr;
                }
            } else {
                while (dwNumMacroblocks > 0) {
                    if (dwNumMacroblocks > macroblockIncr) {
                        if (!(nvMoCompProcessCelsiusFieldMacroblocks(pMacroblocks,
                                                                     macroblockIncr,
                                                                     lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted))) {
                            NvReleaseSemaphore(pDriverData);
                            return (DDERR_GENERIC);
                        }
                    } else {
                        if (!(nvMoCompProcessCelsiusFieldMacroblocks(pMacroblocks,
                                                                     dwNumMacroblocks,
                                                                     lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted))) {
                            NvReleaseSemaphore(pDriverData);
                            return (DDERR_GENERIC);
                        }
                    }
                    pMacroblocks += macroblockIncr;
                    dwNumMacroblocks -= macroblockIncr;
                }
            }
        }

        // Update reference count in case we get a status query
        lpDstSurfaceFlags->dwMCMotionCompReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);
        nvPusherStart(TRUE);

    } else if (dwFunction == EXECUTE_TEMPORAL_FILTER) { // function 1 = temporally filter destination surface
        LPNVMCFILTERDATA lpNvMCFilterData = (LPNVMCFILTERDATA)lpInputData;

        // Only allow temporal filter changes if temporal filtering is allowed.
        // Not allowed if bMCTemporalFilterDisabled == 2 (no temporal filter area in NV12 buffers.)
        if (pDriverData->bMCTemporalFilterDisabled < 2) {
            if (lpNvMCFilterData->bMCFilterOptions & TEMPORAL_FILTER_DISABLE_ON_ALL_SURFACES)
                pDriverData->bMCTemporalFilterDisabled = TRUE;
            else
                pDriverData->bMCTemporalFilterDisabled = FALSE;

            if (lpNvMCFilterData->bMCFilterOptions & TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE)
                pDriverData->nvMCSurfaceFlags[dwSurfaceIndex].bMCFrameType |= TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE;

            pDriverData->bMCPercentCurrentField = lpNvMCFilterData->bMCCurrentFieldCombinationFactor;
        }

        NvReleaseSemaphore(pDriverData);
        return (DD_OK);

    } else if (dwFunction == EXECUTE_SUBPICTURE_DECOMPRESSION) { // function 2 = decompress subpicture surface
        DWORD status = nvMoCompDecompressSubpicture(lpInputData, dwSurfaceIndex);
        NvReleaseSemaphore(pDriverData);
        return (status);

    } else if (dwFunction == EXECUTE_SUBPICTURE_COMPOSITING) { // function 3 = composite subpicture

        DWORD status = nvMoCompCompositeSubpicture(lpInputData, dwSurfaceIndex);
        NvReleaseSemaphore(pDriverData);
        return (status);

    } else if (dwFunction == EXECUTE_DISPLAY_OVERLAY_SURFACE) { // function 4 = display overlay surface

        DWORD status = nvMoCompDisplaySurface(lpInputData, dwSurfaceIndex);
        NvReleaseSemaphore(pDriverData);
        return (status);

    } else if (dwFunction == EXECUTE_FORMAT_CONVERT_OVERLAY_SURFACE) {  // function 5 = format convert overlay surface

        pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
        pSurf_gbl = pSurf_lcl->lpGbl;

        lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

        while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
               (lpDstSurfaceFlags->dwMCSurfaceBase != pSurf_gbl->fpVidMem))
            lpDstSurfaceFlags++;

        lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted = 0;
        lpDstSurfaceFlags->bMCPictureStructure = pDriverData->bMCPictureStructure;
        pDriverData->dwMCMostRecentlyDecodedSurfaceBase = pSurf_gbl->fpVidMem;
        pDriverData->bMCMostRecentlyDecodedPictureStructure = pDriverData->bMCPictureStructure;

        nvMoCompConvertSurfaceFormat(pDriverData->dwMCMostRecentlyDecodedSurfaceBase, TRUE, 15);
    }

    else if (dwFunction == EXECUTE_DXVA_ALPHA_BLENDING) {
        DWORD status = nvDXVACompositeSubpicture(lpInputData, dwSurfaceIndex);
        NvReleaseSemaphore(pDriverData);
        return (status);
    }



    NvReleaseSemaphore(pDriverData);
    return (DD_OK);

} /* nvMoCompExecute */


/*
 * nvQueryMoCompStatus
 *
 * Returns current motion comp status.  Does not currently associate status with
 * a particular surface.
 *
 */
DWORD __stdcall nvQueryMoCompStatus( LPDDHAL_QUERYMOCOMPSTATUSDATA lpQMCSData)
{
    LPNVMCSURFACEFLAGS          lpSurfaceFlags;
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;

    nvSetDriverDataPtrFromDDGbl (lpQMCSData->lpDD->lpGbl);

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        if (!nvEnable32(lpQMCSData->lpDD->lpGbl)) {
            NvReleaseSemaphore(pDriverData);
            lpQMCSData->ddRVal = DDERR_SURFACELOST;
            return (DDHAL_DRIVER_HANDLED);
        }
        if (pDriverData->dwMostRecentHWUser != MODULE_ID_DDRAW) {
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                if (!(nvInitKelvinForMoComp())) {
                    NvReleaseSemaphore(pDriverData);
                    lpQMCSData->ddRVal = DDERR_CURRENTLYNOTAVAIL;
                    return(DDHAL_DRIVER_HANDLED);
                }
            }
            else {
                if (!(nvInitCelsiusForMoComp())) {
                    NvReleaseSemaphore(pDriverData);
                    lpQMCSData->ddRVal = DDERR_CURRENTLYNOTAVAIL;
                    return(DDHAL_DRIVER_HANDLED);
                }
            }
        }
    }

    // Assume return state is OK unless we find out otherwise
    lpQMCSData->ddRVal = DD_OK;

    // Do different status checks depending on surface FourCC type.
    switch (lpQMCSData->lpSurface->lpGbl->ddpfSurface.dwFourCC) {

    case FOURCC_NV12:

        // Get flags for this NV12 surface.
        lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];
        while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
               (lpSurfaceFlags->dwMCSurfaceBase != lpQMCSData->lpSurface->lpGbl->fpVidMem)) {
            lpSurfaceFlags++;
        }

        // If this NV12 surface is the last surface decoded into, check to see if the motion comp
        // operation is done before you allow it to be written or read.
        if (lpQMCSData->lpSurface == (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface) {
            if (lpQMCSData->dwFlags & DDMCQUERY_READ) {
                if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                               lpSurfaceFlags->dwMCMotionCompReferenceCount,
                                               0)) {
                    lpQMCSData->ddRVal = DDERR_WASSTILLDRAWING;
                }
            }
            else {
                if ((!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                                lpSurfaceFlags->dwMCMotionCompReferenceCount,
                                                0))
                    ||
                    (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                                lpSurfaceFlags->dwMCFormatCnvReferenceCount,
                                                0))) {
                    lpQMCSData->ddRVal = DDERR_WASSTILLDRAWING;
                }
            }
        }

        // Else if this NV12 surface is being used as a reference frame, check to see if the motion comp
        // operation is done before you allow it to be written to.
        else if ((lpQMCSData->lpSurface == (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCForwardSurface) ||
                 (lpQMCSData->lpSurface == (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface)) {
            if (!(lpQMCSData->dwFlags & DDMCQUERY_READ)) {
                if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                               lpSurfaceFlags->dwMCMotionCompReferenceCount,
                                               0)) {
                    lpQMCSData->ddRVal = DDERR_WASSTILLDRAWING;
                }
            }
        }
        // Else  check the surface is in the middle of compositing sub-picture
        else  if (lpSurfaceFlags->bMCFrameIsComposited) {
            if(!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                lpSurfaceFlags->dwMCCompositeReferenceCount, 0))
            {
           
               lpQMCSData->ddRVal = DDERR_WASSTILLDRAWING;
 

            }

        }
 
        break;

    case FOURCC_NVID:

        // If this NVID surface is the one last used for a motion comp operation, check to see
        // if the motion comp operation is done before you allow it to be reused.
        if ((DWORD)lpQMCSData->lpSurface == pDriverData->dwMCIDCTSurface) {

            // Get flags for the corresponding NV12 surface.
            pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
            lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];
            while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
                   (lpSurfaceFlags->dwMCSurfaceBase != pSurf_lcl->lpGbl->fpVidMem)) {
                lpSurfaceFlags++;
            }

            if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                           lpSurfaceFlags->dwMCMotionCompReferenceCount,
                                           0)) {
                lpQMCSData->ddRVal = DDERR_WASSTILLDRAWING;
            }
        }
        break;
    }

    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);

} /* nvQueryMoCompStatus */


/*
 * nvDestroyMoComp
 *
 * Destroys anything temporarily created for motion comp operations.
 *
 */
DWORD __stdcall nvDestroyMoComp( LPDDHAL_DESTROYMOCOMPDATA lpDMCData)
{
    nvSetDriverDataPtrFromDDGbl(lpDMCData->lpDD->lpGbl);

    // If a conversion buffer was created by nvCreateMoComp, free it now.
    if (pDriverData->dwDXVAConvertBuffer) {
        GlobalFree((HGLOBAL)pDriverData->dwDXVAConvertBuffer);
        pDriverData->dwDXVAConvertBuffer = 0;
    }

    // Reset some variables.
    pDriverData->dwDXVAFlags &= DXVA_BACK_END_ALPHA_ENABLED;    // don't reset this flag, we may be going through a mode switch

    lpDMCData->ddRVal = DD_OK;
    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);

} /* nvDestroyMoComp */


/*
 * nvGetMoCompDriverInfo
 *
 * Initialize Motion Comp Functionality HAL Callbacks
 *
 */
void __stdcall nvGetMoCompDriverInfo(LPDDHAL_GETDRIVERINFODATA lpData)
{
    DDHAL_DDMOTIONCOMPCALLBACKS moCompCB;
    DWORD dwSize = lpData->dwExpectedSize;

    // Check to see whether DXVAEnable was set in the registry.
    // If not, return an error and don't fill in the function table.
    // Note: dwDXVAFlags is set by D3DReadRegistry.
    if (!(pDriverData->dwDXVAFlags & DXVA_ENABLED_IN_REGISTRY)) {
        lpData->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        return;
    }

    global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0600);

    if ( sizeof(moCompCB) < dwSize )
        dwSize = sizeof(moCompCB);

    lpData->dwActualSize = sizeof(moCompCB);
    memset(&moCompCB, 0, dwSize);

    moCompCB.dwSize = dwSize;

    moCompCB.dwFlags = DDHAL_MOCOMP32_GETGUIDS |
                       DDHAL_MOCOMP32_GETFORMATS |
                       DDHAL_MOCOMP32_CREATE |
                       DDHAL_MOCOMP32_GETCOMPBUFFINFO |
                       DDHAL_MOCOMP32_GETINTERNALINFO |
                       DDHAL_MOCOMP32_BEGINFRAME |
                       DDHAL_MOCOMP32_ENDFRAME |
                       DDHAL_MOCOMP32_RENDER |
                       DDHAL_MOCOMP32_QUERYSTATUS |
                       DDHAL_MOCOMP32_DESTROY;

    moCompCB.GetMoCompGuids = nvGetMoCompGuids;
    moCompCB.GetMoCompFormats = nvGetMoCompFormats;
    moCompCB.CreateMoComp = nvCreateMoComp;
    moCompCB.GetMoCompBuffInfo = nvGetMoCompBuffInfo;
    moCompCB.GetInternalMoCompInfo = nvGetInternalMoCompInfo;
    moCompCB.BeginMoCompFrame = nvBeginMoCompFrame;
    moCompCB.EndMoCompFrame = nvEndMoCompFrame;
    moCompCB.RenderMoComp = nvRenderMoComp;
    moCompCB.QueryMoCompStatus = nvQueryMoCompStatus;
    moCompCB.DestroyMoComp = nvDestroyMoComp;

    memcpy(lpData->lpvData, &moCompCB, dwSize );

    lpData->ddRVal = DD_OK;

} /* nvGetMoCompDriverInfo */


/*
 * nvGetLinearAddress
 *
 * Return the linear address of a DDraw surface, depending on whether
 * it's in Local, NonLocal, or System memory.
 *
 */
LPBYTE __stdcall nvGetLinearAddress(LPDDRAWI_DDRAWSURFACE_LCL lpSurf)
{
    if (lpSurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
        if (lpSurf->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM)
            return (LPBYTE)AGPMEM_ADDR(lpSurf->lpGbl->fpVidMem);
        else
            return (LPBYTE)VIDMEM_ADDR(lpSurf->lpGbl->fpVidMem);
    }
    else
        return (LPBYTE)lpSurf->lpGbl->fpVidMem;
}  // nvGetLinearAddress


/*
 * nvDXVACompositeSubpicture
 *
 * Performs full or partial compositing of a decompressed subpicture surface to the destination overlay
 * surface with scaling.  This function does subpicture blending DXVA-style, blending from one NV12
 * surface to another, and allowing for scaling of the video image before the subpicture is blended in.
 *
 * Note that pDriverData must be set before calling this function.
 *
 */
DWORD __stdcall nvDXVACompositeSubpicture(LPVOID lpInputData, DWORD dwIndex)
{
    LPDXVA_BlendCombination     lpDXVABlend;
    LPDDRAWI_DDRAWSURFACE_LCL   pSrcSurf_lcl, pDstSurf_lcl, pSPSurf_lcl;
    LPDDRAWI_DDRAWSURFACE_GBL   pSrcSurf_gbl, pDstSurf_gbl, pSPSurf_gbl;
    LPNVMCSURFACEFLAGS          lpSrcSurfaceFlags, lpDstSurfaceFlags;
    DWORD                       dwBlockSize;
    DWORD                       dwSrcOffset, dwDstOffset, dwSPOffset;
    DWORD                       dwMixOffset;
    DWORD                       dwPitch422;
    DWORD                       dwSPSurfaceWidth, dwSPSurfaceHeight;
    DWORD                       dwDstSurfaceWidth, dwDstSurfaceHeight;
    int                         nPicSrcX, nPicSrcY, nPicSrcW, nPicSrcH;
    int                         nPicDstX, nPicDstY, nPicDstW, nPicDstH;
    int                         nSubSrcX, nSubSrcY, nSubSrcW, nSubSrcH;
    int                         nSubDstX, nSubDstY, nSubDstW, nSubDstH;
    WORD                        wDXVASrcIndex, wDXVADstIndex;
    BYTE                        bNVSrcIndex, bNVDstIndex;
    DDHAL_BLTDATA               BltData;
    RECTL                       SrcRect, DstRect;
    DWORD                       X0, Y0, X1, Y1;
    DWORD                       tmpVal;
    DWORD                       dwLumaOffset, dwChromaOffset;
    DWORD                       dwScaleX, dwScaleY;
    RECT                        rcPictureSourceRect;
    BYTE                        bScaled;

    // Get pointer to DXVA BlendCombination structure from lpInputData.
    lpDXVABlend = (LPDXVA_BlendCombination)lpInputData;

    // Pick up parameters from DXVA Blend structure.
    wDXVASrcIndex = lpDXVABlend->wPictureSourceIndex;
    wDXVADstIndex = lpDXVABlend->wBlendedDestinationIndex;

    rcPictureSourceRect.left   = lpDXVABlend->PictureSourceRect16thPel.left   >> 4;
    rcPictureSourceRect.top    = lpDXVABlend->PictureSourceRect16thPel.top    >> 4;
    rcPictureSourceRect.right  = lpDXVABlend->PictureSourceRect16thPel.right  >> 4;
    rcPictureSourceRect.bottom = lpDXVABlend->PictureSourceRect16thPel.bottom >> 4;

    nPicSrcX = rcPictureSourceRect.left;
    nPicSrcY = rcPictureSourceRect.top;
    nPicSrcW = rcPictureSourceRect.right  - rcPictureSourceRect.left;
    nPicSrcH = rcPictureSourceRect.bottom - rcPictureSourceRect.top;

    nPicDstX = lpDXVABlend->PictureDestinationRect.left;
    nPicDstY = lpDXVABlend->PictureDestinationRect.top;
    nPicDstW = lpDXVABlend->PictureDestinationRect.right
             - lpDXVABlend->PictureDestinationRect.left;
    nPicDstH = lpDXVABlend->PictureDestinationRect.bottom
             - lpDXVABlend->PictureDestinationRect.top;

    nSubSrcX = lpDXVABlend->GraphicSourceRect.left;
    nSubSrcY = lpDXVABlend->GraphicSourceRect.top;
    nSubSrcW = lpDXVABlend->GraphicSourceRect.right
             - lpDXVABlend->GraphicSourceRect.left;
    nSubSrcH = lpDXVABlend->GraphicSourceRect.bottom
             - lpDXVABlend->GraphicSourceRect.top;

    nSubDstX = lpDXVABlend->GraphicDestinationRect.left;
    nSubDstY = lpDXVABlend->GraphicDestinationRect.top;
    nSubDstW = lpDXVABlend->GraphicDestinationRect.right
             - lpDXVABlend->GraphicDestinationRect.left;
    nSubDstH = lpDXVABlend->GraphicDestinationRect.bottom
             - lpDXVABlend->GraphicDestinationRect.top;

    // Check parameters.
    if ((nSubSrcX <  0) ||
        (nSubSrcY <  0) ||
        (nSubSrcW <  0) ||
        (nSubSrcH <  0))
        return (DDERR_INVALIDPARAMS);

    if ((nPicDstX <  0) ||
        (nPicDstY <  0) ||
        (nPicDstW <= 0) ||
        (nPicDstH <= 0))
        return (DDERR_INVALIDPARAMS);

    // Convert DXVA buffer indexes into NV indexes.
    bNVSrcIndex = pDriverData->bDXVAIndexToNVIndex[wDXVASrcIndex];
    bNVDstIndex = pDriverData->bDXVAIndexToNVIndex[wDXVADstIndex];

    // Get pointers to source and dest NV12 surfaces and the subpicture NVSP surface.
    pSrcSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNV12Surface[bNVSrcIndex];
    pSrcSurf_gbl = pSrcSurf_lcl->lpGbl;
    pDstSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNV12Surface[bNVDstIndex];
    pDstSurf_gbl = pDstSurf_lcl->lpGbl;
    pSPSurf_lcl  = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVSPSurface;
    pSPSurf_gbl  = pSPSurf_lcl->lpGbl;

    // Get surface flags for source and dest.
    lpSrcSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];
    while ((lpSrcSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpSrcSurfaceFlags->dwMCSurfaceBase != pSrcSurf_gbl->fpVidMem))
        lpSrcSurfaceFlags++;
    lpDstSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];
    while ((lpDstSurfaceFlags->dwMCSurfaceBase != 0) &&
           (lpDstSurfaceFlags->dwMCSurfaceBase != pDstSurf_gbl->fpVidMem))
        lpDstSurfaceFlags++;

    // Calculate some things for the surfaces.
    dwPitch422 = (pDstSurf_gbl->wWidth + 3) & ~3;
    dwPitch422 = ((dwPitch422 << 1) + 127) & ~127;

    dwDstSurfaceWidth  = pDstSurf_gbl->wWidth;
    dwDstSurfaceHeight = pDstSurf_gbl->wHeight;

    dwSPSurfaceWidth  = pSPSurf_gbl->wWidth;
    dwSPSurfaceHeight = pSPSurf_gbl->wHeight;

    dwSrcOffset = lpSrcSurfaceFlags->dwMCSurface422Offset;
    dwDstOffset = lpDstSurfaceFlags->dwMCSurface422Offset;

    // Check for rectangles too large for their respective surface.
    if (((DWORD)(nPicSrcX + nPicSrcW) > dwDstSurfaceWidth) ||
        ((DWORD)(nPicSrcY + nPicSrcH) > dwDstSurfaceHeight))
        return (DDERR_INVALIDPARAMS);
    
    if (((DWORD)(nPicDstX + nPicDstW) > dwDstSurfaceWidth) ||
        ((DWORD)(nPicDstY + nPicDstH) > dwDstSurfaceHeight))
        return (DDERR_INVALIDPARAMS);
    
    if (((DWORD)(nSubSrcX + nSubSrcW) > dwSPSurfaceWidth) ||
        ((DWORD)(nSubSrcY + nSubSrcH) > dwSPSurfaceHeight))
        return (DDERR_INVALIDPARAMS);
    
    if (((DWORD)(nSubDstX + nSubDstW) > dwDstSurfaceWidth) ||
        ((DWORD)(nSubDstY + nSubDstH) > dwDstSurfaceHeight))
        return (DDERR_INVALIDPARAMS);
    
    // Calculate offsets in the NVSP subpicture surface.
    // dwSPOffset points to the YUV32 subpicture bitmap.
    dwSPOffset  = VIDMEM_OFFSET(pSPSurf_gbl->fpVidMem);
    dwBlockSize = pSPSurf_gbl->lPitch * pSPSurf_gbl->wHeight;
    dwBlockSize <<= 1;
    dwSPOffset += dwBlockSize;

    // dwMixOffset points to the Mixer buffer used temporarily for alpha blending.
    dwMixOffset = VIDMEM_OFFSET(pSPSurf_gbl->fpVidMem);
    dwBlockSize = pSPSurf_gbl->lPitch * pSPSurf_gbl->wHeight;
    dwBlockSize <<= 1;
    dwMixOffset += dwBlockSize;
    dwMixOffset += (dwBlockSize << 1);
    dwMixOffset += 1024;

    // If the picture source and dest buffers are the same (usually only when
    // back-end alpha blending is enabled) we use the Mix buffer for blending.
    // Else if the picture source and dest rects are the same, we don't need to scale
    // or crop, and we can do our subpicture blending in the destination buffer.
    // But if the rectangles don't match, we need to use the Mix buffer to hold
    // the YUY2 image temporarily until we can scale it into the Dst buffer, and
    // then blend the subpicture into the Dst buffer.
    if (bNVSrcIndex == bNVDstIndex) {
        bScaled = 0;
    }
    else if (memcmp(&(rcPictureSourceRect), &(lpDXVABlend->PictureDestinationRect), sizeof(RECT))) {
        bScaled = 1;
    }
    else {
        bScaled = 0;
        dwMixOffset = dwDstOffset;
    }


    // If the DXVA source and destination indexes are different, use DDBlt
    // to copy the image from the NV12 source to the NV12 destination before
    // we blend.  Note that this only copies the NV12-format portion of the
    // surface, not the YUY2 converted portion.
    if (bNVSrcIndex != bNVDstIndex) {
        memset(&BltData, 0, sizeof(BltData));
        BltData.lpDD            = (LPDDRAWI_DIRECTDRAW_GBL)pDriverData->dwDDGlobal;
        BltData.dwFlags         = DDBLT_ROP;
        BltData.lpDDSrcSurface  = pSrcSurf_lcl;
        BltData.lpDDDestSurface = pDstSurf_lcl;
        SrcRect.top             = 0;
        SrcRect.left            = 0;
        SrcRect.bottom          = (LONG)dwDstSurfaceWidth;
        SrcRect.right           = (LONG)dwDstSurfaceHeight;
        BltData.rSrc            = SrcRect;
        DstRect.top             = 0;
        DstRect.left            = 0;
        DstRect.bottom          = (LONG)dwDstSurfaceWidth;
        DstRect.right           = (LONG)dwDstSurfaceHeight;
        BltData.rDest           = DstRect;
        BltData.bltFX.dwSize    = sizeof(DDBLTFX);
        BltData.bltFX.dwROP     = SRCCOPY;
        Blit32(&BltData);
    }

    // We need to generate a fresh YUY2 copy from the NV12 source in case
    // a subpicture was blended in already.  We do this into the Mix buffer
    // hidden in the NVSP surface.
    dwLumaOffset = VIDMEM_OFFSET(pSrcSurf_gbl->fpVidMem);
    dwChromaOffset = dwLumaOffset + (pSrcSurf_gbl->lPitch * pSrcSurf_gbl->wHeight);

    X0 = 0;
    X1 = dwDstSurfaceWidth;
    Y0 = 0;
    Y1 = dwDstSurfaceHeight;

    // If Kelvin class...
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {

        if (!(nvInitKelvinForMoCompConversion()))
            return DDERR_GENERIC;

        tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
                 (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
                 (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
                 (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
                 (NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0 << 24) |
                 (NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 << 16) |
                 (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_SET_TEXTURE_FILTER(1) | 0x40000);
        nvPushData(1, tmpVal);
        nvPusherAdjust(2);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
        nvPushData(1, dwLumaOffset);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(3, dwChromaOffset);
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_NO_OPERATION | 0x40000);
        nvPushData(5, 0);
        nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(7, dwMixOffset);
        nvPusherAdjust(8);

        SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_INLINE_ARRAY | 0x40300000);
        nvPushData(3, (Y0 << 18) | (X0 << 1));
        nvPushData(4, (Y0 << 16) | (X0 >> 1));
        nvPushData(5, (Y0 << 15) | (X0 >> 1));
        nvPushData(6, ((Y1 << 2) << 16) | (X0 << 1)); // destination is in quarter pel units
        nvPushData(7, (Y1 << 16) | (X0 >> 1));        // LUMA texture is in pel units
        nvPushData(8, ((Y1 >> 1) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height
        nvPushData(9, ((Y1 << 2) << 16) | (X1 << 1)); // each 32 bit write contains data for 2 422 pixels
        nvPushData(10, (Y1 << 16) | (X1 >> 1));
        nvPushData(11, ((Y1 >> 1) << 16) | (X1 >> 1));
        nvPushData(12, (Y0 << 18) | (X1 << 1));
        nvPushData(13, (Y0 << 16) | (X1 >> 1));
        nvPushData(14, (Y0 << 15) | (X1 >> 1));
        nvPushData(15, dDrawSubchannelOffset(NV_DD_KELVIN) + NV097_SET_BEGIN_END | 0x40000);
        nvPushData(16, NV097_SET_BEGIN_END_OP_END);
        nvPusherAdjust(17);
        nvPusherStart(TRUE);
    }

    // Else if Celsius class...
    else {

        if (!(nvInitCelsiusForMoCompConversion()))
            return DDERR_GENERIC;

        tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR << 28) |
                 (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR << 24);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_SET_TEXTURE_FILTER(1) | 0x40000);
        nvPushData(1, tmpVal);
        nvPusherAdjust(2);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
        nvPushData(1, dwLumaOffset);
        nvPushData(2, dwChromaOffset);
        nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_NO_OPERATION | 0x40000);
        nvPushData(4, 0);
        nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(6, dwMixOffset);
        nvPusherAdjust(7);

        SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

        // No HD scaled version because subpicture compositing is not used for HD 
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_INLINE_ARRAY(0) | 0x300000);
        nvPushData(3, (Y0 << 15) | (X0 >> 1));
        nvPushData(4, (Y0 << 16) | (X0 >> 1));
        nvPushData(5, (Y0 << 18) | (((X0 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));
        nvPushData(6, ((Y1 >> 1) << 16) | (X0 >> 1)); // CHROMA texture is in pel units and is only half the surface height
        nvPushData(7, (Y1 << 16) | (X0 >> 1));        // LUMA texture is in pel units
        nvPushData(8, ((Y1 << 2) << 16) | (((X0 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // destination is in quarter pel units
        nvPushData(9, ((Y1 >> 1) << 16) | (X1 >> 1));
        nvPushData(10, (Y1 << 16) | (X1 >> 1));
        nvPushData(11, ((Y1 << 2) << 16) | (((X1 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF)); // each 32 bit write contains data for 2 422 pixels
        nvPushData(12, (Y0 << 15) | (X1 >> 1));
        nvPushData(13, (Y0 << 16) | (X1 >> 1));
        nvPushData(14, (Y0 << 18) | (((X1 << 1) - CELSIUS_DESTINATION_VIEWPORT_XOFFSET) & 0x0000FFFF));
        nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV056_SET_BEGIN_END | 0x40000);
        nvPushData(16, NV056_SET_BEGIN_END_OP_END);
        nvPusherAdjust(17);
        nvPusherStart(TRUE);
    }

    // If picture scaling is required, we scale the image from the Mix buffer to
    // the Dst buffer.  No subpicture blending is done at this stage.
    if (bScaled) {
        dwScaleX = (nPicSrcW << 20) / nPicDstW;
        if ((dwScaleX < 0x10000) || (dwScaleX > 0x1000000))
            dwScaleX = 0x100000;
        dwScaleY = (nPicSrcH << 20) / nPicDstH;
        if ((dwScaleY < 0x10000) || (dwScaleY > 0x1000000))
            dwScaleY = 0x100000;

        nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
        nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) + NV088_IMAGE_OUT_POINT | 0x400000);
        nvPushData (3, asmMergeCoords(nPicDstX, nPicDstY));
        nvPushData (4, asmMergeCoords(nPicDstW, nPicDstH));
        nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | dwPitch422);
        nvPushData (6, dwDstOffset);                                            // 422 destination pixel area
        nvPushData (7, dwScaleX);
        nvPushData (8, dwScaleY);
        nvPushData (9, asmMergeCoords(dwDstSurfaceWidth, dwDstSurfaceHeight));
        nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | dwPitch422);
        nvPushData (11, dwMixOffset);                                           // 422 source pixel area
        nvPushData (12, ((asmMergeCoords(nPicSrcX, nPicSrcY)) << 4));
        nvPushData (13, 0x100000);
        nvPushData (14, 0x100000);
        nvPushData (15, asmMergeCoords(dwSPSurfaceWidth, dwSPSurfaceHeight));
        nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT << 16) | (pSPSurf_gbl->lPitch << 2));
        nvPushData (17, dwSPOffset);                                            // AYUV source pixel area
        nvPushData (18, ((asmMergeCoords(nSubSrcX, nSubSrcY)) << 4));
        nvPusherAdjust (19);
        nvPusherStart(TRUE);

        // Further operations should be done in the Dst buffer.
        dwMixOffset = dwDstOffset;
    }

    // Next we use the DVD Subpicture class to blend the subpicture into the Dst buffer.
    if ((nSubSrcW > 0) && (nSubSrcH > 0)) {
        dwScaleX = (nSubSrcW << 20) / nSubDstW;
        if ((dwScaleX < 0x10000) || (dwScaleX > 0x1000000))
            dwScaleX = 0x100000;
        dwScaleY = (nSubSrcH << 20) / nSubDstH;
        if ((dwScaleY < 0x10000) || (dwScaleY > 0x1000000))
            dwScaleY = 0x100000;
        nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
        nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) + NV088_IMAGE_OUT_POINT | 0x400000);
        nvPushData (3, asmMergeCoords(nSubDstX, nSubDstY));
        nvPushData (4, asmMergeCoords(nSubDstW, nSubDstH));
        nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | dwPitch422);
        nvPushData (6, dwMixOffset);                                            // 422 destination pixel area
        nvPushData (7, 0x100000);
        nvPushData (8, 0x100000);
        nvPushData (9, asmMergeCoords(dwDstSurfaceWidth, dwDstSurfaceHeight));
        nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | dwPitch422);
        nvPushData (11, dwMixOffset);                                          // 422 source pixel area
        nvPushData (12, ((asmMergeCoords(nSubDstX, nSubDstY)) << 4));
        nvPushData (13, dwScaleX);
        nvPushData (14, dwScaleY);
        nvPushData (15, asmMergeCoords(dwSPSurfaceWidth, dwSPSurfaceHeight));
        nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8 << 16) | (pSPSurf_gbl->lPitch << 2));
        nvPushData (17, dwSPOffset);                                            // AYUV source pixel area
        nvPushData (18, ((asmMergeCoords(nSubSrcX, nSubSrcY)) << 4));
        nvPusherAdjust (19);
        nvPusherStart(TRUE);
    }

    // If the source buffer and destination buffer are the same (usually only
    // when back-end alpha blending is enabled) we need to copy the blended image
    // to the destination buffer.
    if (bNVSrcIndex == bNVDstIndex) {
        nvPushData (0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData (1, NV_VPP_DVD_SUBPICTURE_IID);
        nvPushData (2, dDrawSubchannelOffset(NV_DD_SPARE) + NV088_IMAGE_OUT_POINT | 0x400000);
        nvPushData (3, asmMergeCoords(0, 0));
        nvPushData (4, asmMergeCoords(dwDstSurfaceWidth, dwDstSurfaceHeight));
        nvPushData (5, (NV088_IMAGE_OUT_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | dwPitch422);
        nvPushData (6, dwDstOffset);                                            // 422 destination pixel area
        nvPushData (7, 0x100000);
        nvPushData (8, 0x100000);
        nvPushData (9, asmMergeCoords(dwDstSurfaceWidth, dwDstSurfaceHeight));
        nvPushData (10, (NV088_IMAGE_IN_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16) | dwPitch422);
        nvPushData (11, dwMixOffset);                                           // 422 source pixel area
        nvPushData (12, ((asmMergeCoords(0, 0)) << 4));
        nvPushData (13, 0x100000);
        nvPushData (14, 0x100000);
        nvPushData (15, asmMergeCoords(dwSPSurfaceWidth, dwSPSurfaceHeight));
        nvPushData (16, (NV088_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT << 16) | (pSPSurf_gbl->lPitch << 2));
        nvPushData (17, dwSPOffset);                                            // AYUV source pixel area
        nvPushData (18, ((asmMergeCoords(0, 0)) << 4));
        nvPusherAdjust (19);
    }

    // See if we need to do temporal filtering on the alpha blended image.
    lpDstSurfaceFlags->bMCFrameIsFiltered = FALSE;
    if (pDriverData->bMCTemporalFilterDisabled == FALSE) {
        nvPusherStart(TRUE);
        nvMoCompTemporalFilter(lpDstSurfaceFlags->dwMCSurfaceBase, pDriverData->bMCPercentCurrentField);
    }

    // Update reference count in case we get a status query
    lpDstSurfaceFlags->dwMCCompositeReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);

    nvPusherStart(TRUE);
    pDriverData->dDrawSpareSubchannelObject = NV_VPP_DVD_SUBPICTURE_IID;

    // Set some surface flags for the destination surface.
    lpDstSurfaceFlags->bMCFrameIsComposited = PICTURE_STRUCTURE_FRAME;
    lpDstSurfaceFlags->bMCFrameAlreadyFormatConverted = 15;
    lpDstSurfaceFlags->bMCPictureStructure  = lpSrcSurfaceFlags->bMCPictureStructure;
    lpDstSurfaceFlags->bMCPictureCodingType = lpSrcSurfaceFlags->bMCPictureCodingType;
    lpDstSurfaceFlags->bMCTopFieldFirst     = lpSrcSurfaceFlags->bMCTopFieldFirst;
    lpDstSurfaceFlags->bMCSecondField       = lpSrcSurfaceFlags->bMCSecondField;
    lpDstSurfaceFlags->bMCFrameType         = lpSrcSurfaceFlags->bMCFrameType;

    return DD_OK;

} /* nvDXVACompositeSubpicture */


/*
 * nvUpdateDXVAIndexTables
 *
 * Recalculates the table pDriverData->bDXVAIndexToNVIndex[8] which is used to convert surface indexes
 * sent by DXVA into our internal NV indexes.  Because the Win/ME DDFlip operation swaps surface
 * pointers, this table may need to be updated if a Flip command came since the last time it was
 * calculated.
 *
 */
 void __stdcall nvUpdateDXVAIndexTable(GLOBALDATA *pDriverData)
 {
    WORD    wDXVAIndex;
    BYTE    bNVIndex;
    DWORD   dwTableVidMem, dwSurfVidMem;

    // Compare the values in the bDXVAIndexToNVIndex and bDXVAIndexToVidMem tables.
    // If they don't match, search through the IndexToVidMem table to see if you can find a match.
    for (wDXVAIndex = 0; wDXVAIndex < 8; wDXVAIndex++) {
        dwTableVidMem = pDriverData->dwDXVAIndexToVidMem[wDXVAIndex];
        if (dwTableVidMem) {
            bNVIndex = pDriverData->bDXVAIndexToNVIndex[wDXVAIndex];
            if (pDriverData->dwMCNV12Surface[bNVIndex]) {
                dwSurfVidMem = ((LPDDRAWI_DDRAWSURFACE_LCL)(pDriverData->dwMCNV12Surface[bNVIndex]))->lpGbl->fpVidMem;
                if (dwTableVidMem != dwSurfVidMem) {
                    for (bNVIndex = 0; bNVIndex < 8; bNVIndex++) {
                        if (pDriverData->dwMCNV12Surface[bNVIndex]) {
                            dwSurfVidMem = ((LPDDRAWI_DDRAWSURFACE_LCL)(pDriverData->dwMCNV12Surface[bNVIndex]))->lpGbl->fpVidMem;
                            if (dwTableVidMem == dwSurfVidMem) {
                                pDriverData->bDXVAIndexToNVIndex[wDXVAIndex] = bNVIndex;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
 }  // nvUpdateDXVAIndexTables


/*
 * nvMoCompFrameStatus
 *
 * This is a variation of getFrameStatusOverlay for motion comp overlay surfaces only.
 * It does additional checking for "previous flip from" surfaces which the other function
 * skips because of WHQL test requirements.
 *
 */
DWORD __stdcall nvMoCompFrameStatus(FLATPTR fpVidMem)
{
    if (getDC()->flipOverlayRecord.bFlipFlag &&
        ((fpVidMem == getDC()->flipOverlayRecord.fpFlipFrom) ||
         (fpVidMem == getDC()->flipOverlayRecord.fpPreviousFlipFrom))) {

        NvNotification *pPioFlipOverlayNotifier = pDriverData->pPioFlipOverlayNotifierFlat;

        if (fpVidMem == getDC()->flipOverlayRecord.fpPreviousFlipFrom) {
            LONGLONG timeNow = 0;
            // only need for one to flush out, no way to really determine this on NV10,
            // but if we wait at least one CRTC refresh since it was issued, we can
            // guarantee it has been flushed through
            if ((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) &&
                (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) {
                QueryPerformanceCounter((LARGE_INTEGER *) &timeNow);
                if ((timeNow - getDC()->flipOverlayRecord.llFlipTime) < (getDC()->flipPrimaryRecord.dwFlipDuration * 2)) {
                    return (DDERR_WASSTILLDRAWING);
                }
            }
        } else {
            if (fpVidMem == getDC()->flipOverlayRecord.fpFlipFrom) {
                // need to wait for both flips to flush out
                if (((volatile)pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) &&
                    ((volatile)pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) {
                    return (DDERR_WASSTILLDRAWING);
                } else {
                    getDC()->flipOverlayRecord.bFlipFlag = FALSE;
                }
            }
        }
        return (DD_OK);
    }
    return (DD_OK);
}  // nvMoCompFrameStatus


/*
 * nvDXVABackEndAlphaBlend
 *
 * This function blends the current subpicture graphic into the selected frame.
 * It can be called from MoComp or Flip functions.
 * Note: pDriverData should be set before this function is called.
 *
 */
DWORD __stdcall nvDXVABackEndAlphaBlend(FLATPTR fpVidMem)
{
    WORD                        wDXVAIndex;
    DXVA_BlendCombination       DXVABlendCombination = {0};
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    LPNVMCFRAMEDATA             lpNvMCFrameData;

    // Look up index of target surface to blend into.
    for (wDXVAIndex = 0; wDXVAIndex < 8; wDXVAIndex++) {
        if (pDriverData->dwDXVAIndexToVidMem[wDXVAIndex] == (DWORD)fpVidMem)
            break;
    }
    if (wDXVAIndex > 7)
        wDXVAIndex = 0;

    // Get pointer to NVMC surface used to store parameters.
    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVMCSurface;
    lpNvMCFrameData = (LPNVMCFRAMEDATA)VIDMEM_ADDR(pSurf_lcl->lpGbl->fpVidMem);

    // Set up DXVA_BlendCombination structure with subpicture parameters.
    DXVABlendCombination.wPictureSourceIndex = wDXVAIndex;
    DXVABlendCombination.wBlendedDestinationIndex = wDXVAIndex;
    DXVABlendCombination.PictureSourceRect16thPel.left   = 0;
    DXVABlendCombination.PictureSourceRect16thPel.top    = 0;
    DXVABlendCombination.PictureSourceRect16thPel.right  = lpNvMCFrameData->wDXVAPicWidthInMB * 16 * 16;
    DXVABlendCombination.PictureSourceRect16thPel.bottom = lpNvMCFrameData->wDXVAPicHeightInMB * 16 * 16;
    DXVABlendCombination.PictureDestinationRect.left   = 0;
    DXVABlendCombination.PictureDestinationRect.top    = 0;
    DXVABlendCombination.PictureDestinationRect.right  = lpNvMCFrameData->wDXVAPicWidthInMB * 16;
    DXVABlendCombination.PictureDestinationRect.bottom = lpNvMCFrameData->wDXVAPicHeightInMB * 16;
    DXVABlendCombination.GraphicSourceRect      = lpNvMCFrameData->rcGraphicSourceRect;
    DXVABlendCombination.GraphicDestinationRect = lpNvMCFrameData->rcGraphicDestinationRect;

    nvMoCompExecute(lpNvMCFrameData->dwMCMultiMonID,
                    EXECUTE_DXVA_ALPHA_BLENDING,
                    (LPVOID)&DXVABlendCombination,
                    sizeof(DXVA_BlendCombination),
                    0);

    return DD_OK;
}


#endif  // NVARCH >= 0x10
