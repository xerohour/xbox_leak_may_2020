
 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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
 *  Copyright (C) 1999 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       MoInit.cpp
 *  Content:    Windows98 DirectDraw 32 bit driver
 *
 ***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

NVMOCOMPPARAMS nvMoCompParams;

/*
 * nvInitCelsiusForMoComp
 *
 * Performs basic motion comp initialization of the celcius class object
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvInitCelsiusForMoComp(void)
{
    LPDDRAWI_DDRAWSURFACE_LCL   dstx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   dst;
    unsigned long               surfaceWidth;
    unsigned long               surfaceHeight;
    unsigned long               surfacePitch;
    unsigned long               tmpVal;
    float                       fBuffer[16];
    unsigned long               *ftol = (unsigned long *)&fBuffer[0];
    float                       xTrans, yTrans, zTrans;

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        if (!nvEnable32(NULL)) {
            return FALSE;
        }
    }

	if(dstx == NULL)		//It happnes for WinDVD
		return FALSE;

    dst = dstx->lpGbl;
    surfaceWidth = dst->wWidth;
    surfaceHeight = dst->wHeight;
    surfacePitch = dst->lPitch;

    pDriverData->dwMostRecentHWUser = MODULE_ID_DDRAW;

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    // Clear current IDCT context Dma
    pDriverData->dwMCCurrentCelsiusIDCTCtxDma = 0;

    // Reset current vertex mode
    nvMoCompParams.vertexMode = 0;

    // Reset current data format
    nvMoCompParams.dataFormat = 0;

    // Reset the celsius context Dmas to use the default video context Dma
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CONTEXT_DMA_A | 0x40000);
    nvPushData(1, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CONTEXT_DMA_B | 0x40000);
    nvPushData(3, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CONTEXT_DMA_VERTEX | 0x40000);
    nvPushData(5, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CONTEXT_DMA_STATE | 0x40000);
    nvPushData(7, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CONTEXT_DMA_COLOR | 0x40000);
    nvPushData(9, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_CONTEXT_DMA_ZETA | 0x40000);
    nvPushData(11, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPusherAdjust(12);

    // Set up one window clip rectangle to be the rendered area
    // All the other rectangles are deactivated
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_WINDOW_CLIP_HORIZONTAL(0) | 0x200000);
    nvPushData(1, ((2047 << 16) | (-2048 & 0xfff)));
    nvPushData(2, 0);
    nvPushData(3, 0);
    nvPushData(4, 0);
    nvPushData(5, 0);
    nvPushData(6, 0);
    nvPushData(7, 0);
    nvPushData(8, 0);

    nvPushData(9, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_WINDOW_CLIP_VERTICAL(0) | 0x200000);
    nvPushData(10, ((2047 << 16) | (-2048 & 0xfff)));
    nvPushData(11, 0);
    nvPushData(12, 0);
    nvPushData(13, 0);
    nvPushData(14, 0);
    nvPushData(15, 0);
    nvPushData(16, 0);
    nvPushData(17, 0);

    nvPushData(18, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_WINDOW_CLIP_TYPE | 0x40000);
    nvPushData(19, NV056_SET_WINDOW_CLIP_TYPE_V_INCLUSIVE);

    nvPushData(20, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_SURFACE_CLIP_HORIZONTAL | 0x40000);
    // pitch << 1
    nvPushData(21, ((surfacePitch << 17) | 0));

    nvPushData(22, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_SURFACE_CLIP_VERTICAL | 0x40000);
    // height << 1 which is at least enough for luma height + chroma height
    nvPushData(23, ((surfaceHeight << 17) | 0));

    nvPusherAdjust(24);

    // Enable transforms
    tmpVal = (NV056_SET_TLMODE_W_DIVIDE_1_ENABLE << 2) |
             (NV056_SET_TLMODE_W_DIVIDE_0_ENABLE << 1) |
              NV056_SET_TLMODE_PASSTHROUGH_FALSE;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TLMODE | 0x40000);
    nvPushData(1, tmpVal);

    nvPusherAdjust(2);

    // Set Control0 defaults
    tmpVal = (NV056_SET_CONTROL0_PREMULTIPLIEDALPHA_FALSE << 24) |
             (NV056_SET_CONTROL0_TEXTUREPERSPECTIVE_FALSE << 20) |
             (NV056_SET_CONTROL0_Z_PERSPECTIVE_ENABLE_FALSE << 16) |
             (NV056_SET_CONTROL0_Z_FORMAT_FIXED << 12) |
             (NV056_SET_CONTROL0_WBUFFER_SELECT_0 << 8) |
             (NV056_SET_CONTROL0_STENCIL_WRITE_ENABLE_TRUE);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CONTROL0 | 0x40000);
    nvPushData(1, tmpVal);

    nvPusherAdjust(2);

    // Load ModelView matrix and inverse ModelView matrix with identity
    fBuffer[0]  = 1.0; fBuffer[1]  = 0.0; fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
    fBuffer[4]  = 0.0; fBuffer[5]  = 1.0; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
    fBuffer[8]  = 0.0; fBuffer[9]  = 0.0; fBuffer[10] = 1.0; fBuffer[11] = 0.0;
    fBuffer[12] = 0.0; fBuffer[13] = 0.0; fBuffer[14] = 0.0; fBuffer[15] = 1.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_MODEL_VIEW_MATRIX0(0) | 0x400000);
    nvPushData(1, ftol[0]);
    nvPushData(2, ftol[4]);
    nvPushData(3, ftol[8]);
    nvPushData(4, ftol[12]);
    nvPushData(5, ftol[1]);
    nvPushData(6, ftol[5]);
    nvPushData(7, ftol[9]);
    nvPushData(8, ftol[13]);
    nvPushData(9, ftol[2]);
    nvPushData(10, ftol[6]);
    nvPushData(11, ftol[10]);
    nvPushData(12, ftol[14]);
    nvPushData(13, ftol[3]);
    nvPushData(14, ftol[7]);
    nvPushData(15, ftol[11]);
    nvPushData(16, ftol[15]);

    nvPushData(17, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_INVERSE_MODEL_VIEW_MATRIX0(0) | 0x400000);
    nvPushData(18, ftol[0]);
    nvPushData(19, ftol[4]);
    nvPushData(20, ftol[8]);
    nvPushData(21, ftol[12]);
    nvPushData(22, ftol[1]);
    nvPushData(23, ftol[5]);
    nvPushData(24, ftol[9]);
    nvPushData(25, ftol[13]);
    nvPushData(26, ftol[2]);
    nvPushData(27, ftol[6]);
    nvPushData(28, ftol[10]);
    nvPushData(29, ftol[14]);
    nvPushData(30, ftol[3]);
    nvPushData(31, ftol[7]);
    nvPushData(32, ftol[11]);
    nvPushData(33, ftol[15]);

    nvPusherAdjust(34);

    // Align surface pitch and set surface format and pitch
    surfacePitch |= (surfacePitch << 16);
    surfacePitch <<= 1; // Surface is always referenced by hardware in field mode

    tmpVal = (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
              NV056_SET_SURFACE_FORMAT_COLOR_LE_B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(1, tmpVal);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_PITCH | 0x40000);
    nvPushData(3, surfacePitch);

    nvPusherAdjust(4);

    // Set up viewport
//    xTrans = (float)(-2048 - (1.0 / 32.0)); // hardware epsilon
    // Changed to allow destination X coordinate range of -2046 - 2046 for optimal 1080i performance
    xTrans = (float)(-2 - (1.0 / 32.0)); // hardware epsilon
    yTrans = (float)(-2048 - (1.0 / 32.0));
    zTrans = 0.0;

    // Load composite matrix assuming quarter pel scaling of surface coordinates
    if (pDriverData->bMCHorizontallyDownscale1080i)
        fBuffer[0]  = 0.1875; // quarter pel xScale
    else
        fBuffer[0]  = 0.25; // quarter pel xScale
    fBuffer[1]  = 0.0;
    fBuffer[2]  = 0.0;
    fBuffer[3]  = 0.0;
    fBuffer[4]  = 0.0;
    fBuffer[5]  = 0.25; // quarter pel yScale
    fBuffer[6]  = 0.0;
    fBuffer[7]  = 0.0;
    fBuffer[8]  = 0.0;
    fBuffer[9]  = 0.0;
    fBuffer[10] = 1.0;
    fBuffer[11] = 0.0;
    fBuffer[12] = 0.0;
    fBuffer[13] = 0.0;
    fBuffer[14] = 0.0;
    fBuffer[15] = 1.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMPOSITE_MATRIX(0) | 0x400000);
    nvPushData(1, ftol[0]);
    nvPushData(2, ftol[4]);
    nvPushData(3, ftol[8]);
    nvPushData(4, ftol[12]);
    nvPushData(5, ftol[1]);
    nvPushData(6, ftol[5]);
    nvPushData(7, ftol[9]);
    nvPushData(8, ftol[13]);
    nvPushData(9, ftol[2]);
    nvPushData(10, ftol[6]);
    nvPushData(11, ftol[10]);
    nvPushData(12, ftol[14]);
    nvPushData(13, ftol[3]);
    nvPushData(14, ftol[7]);
    nvPushData(15, ftol[11]);
    nvPushData(16, ftol[15]);

    nvPusherAdjust(17);

    // Set viewport offset separately
    fBuffer[0] = xTrans;
    fBuffer[1] = yTrans;
    fBuffer[2] = zTrans;
    fBuffer[3] = 0.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_VIEWPORT_OFFSET(0) | 0x100000);
    nvPushData(1, ftol[0]);
    nvPushData(2, ftol[1]);
    nvPushData(3, ftol[2]);
    nvPushData(4, ftol[3]);

    nvPusherAdjust(5);

    // Set front and back clipping
    fBuffer[0] = 0.0;
    fBuffer[1] = 1.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CLIP_MIN | 0x80000);
    nvPushData(1, ftol[0]);
    nvPushData(2, ftol[1]);

    nvPusherAdjust(3);

    // Disable almost everything

    // Disable alpha test
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_ALPHA_TEST_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_ALPHA_TEST_ENABLE_V_FALSE);

    // Disable blending
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BLEND_ENABLE | 0x40000);
    nvPushData(3, NV056_SET_BLEND_ENABLE_V_FALSE);

    // Disable culling
    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CULL_FACE_ENABLE | 0x40000);
    nvPushData(5, NV056_SET_CULL_FACE_ENABLE_V_FALSE);

    // Disable depth test
    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_DEPTH_TEST_ENABLE | 0x40000);
    nvPushData(7, NV056_SET_DEPTH_TEST_ENABLE_V_FALSE);

    // Disable dither
    nvPushData(8, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_DITHER_ENABLE | 0x40000);
    nvPushData(9, NV056_SET_DITHER_ENABLE_V_FALSE);

    // Disable lighting
    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_LIGHTING_ENABLE | 0x40000);
    nvPushData(11, NV056_SET_LIGHTING_ENABLE_V_FALSE);

    // Disable point parameters computation
    nvPushData(12, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_POINT_PARAMS_ENABLE | 0x40000);
    nvPushData(13, NV056_SET_POINT_PARAMS_ENABLE_V_FALSE);

    // Disable point smoothing
    nvPushData(14, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_POINT_SMOOTH_ENABLE | 0x40000);
    nvPushData(15, NV056_SET_LINE_SMOOTH_ENABLE_V_FALSE);

    // Disable line smoothing
    nvPushData(16, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_LINE_SMOOTH_ENABLE | 0x40000);
    nvPushData(17, NV056_SET_LINE_SMOOTH_ENABLE_V_FALSE);

    // Disable polygon smoothing
    nvPushData(18, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_POLY_SMOOTH_ENABLE | 0x40000);
    nvPushData(19, NV056_SET_POLY_SMOOTH_ENABLE_V_FALSE);

    // Disable skinning
    nvPushData(20, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_SKIN_ENABLE | 0x40000);
    nvPushData(21, NV056_SET_SKIN_ENABLE_V_FALSE);

    // Disable stencil test
    nvPushData(22, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_STENCIL_TEST_ENABLE | 0x40000);
    nvPushData(23, NV056_SET_STENCIL_TEST_ENABLE_V_FALSE);

    // Disable polygon offset point
    nvPushData(24, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_POLY_OFFSET_POINT_ENABLE | 0x40000);
    nvPushData(25, NV056_SET_POLY_OFFSET_POINT_ENABLE_V_FALSE);

    // Disable polygon offset line
    nvPushData(26, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_POLY_OFFSET_LINE_ENABLE | 0x40000);
    nvPushData(27, NV056_SET_POLY_OFFSET_LINE_ENABLE_V_FALSE);

    // Disable polygon offset fill
    nvPushData(28, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_POLY_OFFSET_FILL_ENABLE | 0x40000);
    nvPushData(29, NV056_SET_POLY_OFFSET_FILL_ENABLE_V_FALSE);

    nvPusherAdjust(30);

    // Set light control
    tmpVal = (NV056_SET_LIGHT_CONTROL_SEPARATE_SPECULAR_EN_FALSE << 1) |
              NV056_SET_LIGHT_CONTROL_SECONDARY_COLOR_EN_FALSE;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_LIGHT_CONTROL | 0x40000);
    nvPushData(1, tmpVal);

    // Disable color material
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COLOR_MATERIAL | 0x40000);
    nvPushData(3, NV056_SET_COLOR_MATERIAL_V_DISABLED);

    // Set material emission
    fBuffer[0] = 0.0;

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_MATERIAL_EMISSION(0) | 0xC0000);
    nvPushData(5, ftol[0]);
    nvPushData(6, ftol[0]);
    nvPushData(7, ftol[0]);

    // Disable specular
    nvPushData(8, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SPECULAR_ENABLE | 0x40000);
    nvPushData(9, NV056_SET_SPECULAR_ENABLE_V_FALSE);

    // Disable all lights
    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_LIGHT_ENABLE_MASK | 0x40000);
    nvPushData(11, 0);

    nvPusherAdjust(12);

    // Disable texgen modes for now (OpenGL default is EYE_LINEAR)

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXGEN_S(0) | 0x40000);
    nvPushData(1, NV056_SET_TEXGEN_S_V_DISABLE);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXGEN_T(0) | 0x40000);
    nvPushData(3, NV056_SET_TEXGEN_T_V_DISABLE);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXGEN_R(0) | 0x40000);
    nvPushData(5, NV056_SET_TEXGEN_R_V_DISABLE);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXGEN_Q(0) | 0x40000);
    nvPushData(7, NV056_SET_TEXGEN_Q_V_DISABLE);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXGEN_S(1) | 0x40000);
    nvPushData(9, NV056_SET_TEXGEN_S_V_DISABLE);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_T(1) | 0x40000);
    nvPushData(11, NV056_SET_TEXGEN_T_V_DISABLE);

    nvPushData(12, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_R(1) | 0x40000);
    nvPushData(13, NV056_SET_TEXGEN_R_V_DISABLE);

    nvPushData(14, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_Q(1) | 0x40000);
    nvPushData(15, NV056_SET_TEXGEN_Q_V_DISABLE);

    nvPusherAdjust(16);

    // Set default texgen planes
    fBuffer[0] = 0.0;
    fBuffer[1] = 1.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXGEN_SPLANE0(0) | 0x100000);
    nvPushData(1, ftol[1]);
    nvPushData(2, ftol[0]);
    nvPushData(3, ftol[0]);
    nvPushData(4, ftol[0]);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXGEN_TPLANE0(0) | 0x100000);
    nvPushData(6, ftol[0]);
    nvPushData(7, ftol[1]);
    nvPushData(8, ftol[0]);
    nvPushData(9, ftol[0]);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_RPLANE0(0) | 0x100000);
    nvPushData(11, ftol[0]);
    nvPushData(12, ftol[0]);
    nvPushData(13, ftol[0]);
    nvPushData(14, ftol[0]);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_QPLANE0(0) | 0x100000);
    nvPushData(16, ftol[0]);
    nvPushData(17, ftol[0]);
    nvPushData(18, ftol[0]);
    nvPushData(19, ftol[0]);

    nvPushData(20, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_SPLANE1(0) | 0x100000);
    nvPushData(21, ftol[1]);
    nvPushData(22, ftol[0]);
    nvPushData(23, ftol[0]);
    nvPushData(24, ftol[0]);

    nvPushData(25, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_TPLANE1(0) | 0x100000);
    nvPushData(26, ftol[0]);
    nvPushData(27, ftol[1]);
    nvPushData(28, ftol[0]);
    nvPushData(29, ftol[0]);

    nvPushData(30, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_RPLANE1(0) | 0x100000);
    nvPushData(31, ftol[0]);
    nvPushData(32, ftol[0]);
    nvPushData(33, ftol[0]);
    nvPushData(34, ftol[0]);

    nvPushData(35, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXGEN_QPLANE1(0) | 0x100000);
    nvPushData(36, ftol[0]);
    nvPushData(37, ftol[0]);
    nvPushData(38, ftol[0]);
    nvPushData(39, ftol[0]);

    nvPusherAdjust(40);

    // Set default fog mode
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_MODE | 0x40000);
    nvPushData(1, NV056_SET_FOG_MODE_FOG_MODE_LINEAR);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_GEN_MODE | 0x40000);
    nvPushData(3, NV056_SET_FOG_GEN_MODE_FOG_GEN_MODE_USE_INPUT);

    // Disable fog
    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_ENABLE | 0x40000);
    nvPushData(5, NV056_SET_FOG_ENABLE_V_FALSE);

    // Set default fog parameters
    fBuffer[0] = 1.0;                           // K0
    fBuffer[1] = 1.0;                           // K1
    fBuffer[2] = 0.0;                           // K2

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_PARAMS(0) | 0xC0000);
    nvPushData(7, ftol[0]);
    nvPushData(8, ftol[1]);
    nvPushData(9, ftol[2]);

    // Set default fog color
    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_FOG_COLOR | 0x40000);
    nvPushData(11, 0xFFFFFFFF);

    nvPusherAdjust(12);

    // Set default alpha function
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_ALPHA_FUNC | 0x40000);
    nvPushData(1, NV056_SET_ALPHA_FUNC_V_ALWAYS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_ALPHA_REF | 0x40000);
    nvPushData(3, 0);

    // Set default blend equation
    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BLEND_EQUATION | 0x40000);
    nvPushData(5, NV056_SET_BLEND_EQUATION_V_FUNC_ADD);

    // Set default blend color
    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BLEND_COLOR | 0x40000);
    nvPushData(7, 0);

    // Set default blend func source factor
    nvPushData(8, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BLEND_FUNC_SFACTOR | 0x40000);
    nvPushData(9, NV056_SET_BLEND_FUNC_SFACTOR_V_ONE);

    // Set default blend func destination factor
    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BLEND_FUNC_DFACTOR | 0x40000);
    nvPushData(11, NV056_SET_BLEND_FUNC_DFACTOR_V_ZERO);

    nvPusherAdjust(12);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FRONT_FACE | 0x40000);
    nvPushData(1, NV056_SET_FRONT_FACE_V_CCW);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_CULL_FACE | 0x40000);
    nvPushData(3, NV056_SET_CULL_FACE_V_BACK);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SHADE_MODE | 0x40000);
    nvPushData(5, NV056_SET_SHADE_MODE_V_SMOOTH);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FRONT_POLYGON_MODE | 0x40000);
    nvPushData(7, NV056_SET_FRONT_POLYGON_MODE_V_FILL);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BACK_POLYGON_MODE | 0x40000);
    nvPushData(9, NV056_SET_BACK_POLYGON_MODE_V_FILL);

    fBuffer[0] = 0.0;

    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_POLYGON_OFFSET_SCALE_FACTOR | 0x40000);
    nvPushData(11, ftol[0]);

    nvPushData(12, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_POLYGON_OFFSET_BIAS | 0x40000);
    nvPushData(13, ftol[0]);

    nvPushData(14, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_EDGE_FLAG | 0x40000);
    nvPushData(15, 1);

    nvPusherAdjust(16);

    // Initialize some vertex attributes
    fBuffer[0] = 0.0;
    fBuffer[1] = 1.0;

    // Set color for MODULATE blend mode
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_DIFFUSE_COLOR4F(0) | 0x100000);
    nvPushData(1, ftol[1]);
    nvPushData(2, ftol[1]);
    nvPushData(3, ftol[1]);
    nvPushData(4, ftol[1]);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SPECULAR_COLOR3F(0) | 0xC0000);
    nvPushData(6, ftol[0]);
    nvPushData(7, ftol[0]);
    nvPushData(8, ftol[0]);

    nvPushData(9, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXCOORD0_4F(0) | 0x100000);
    nvPushData(10, ftol[0]);
    nvPushData(11, ftol[0]);
    nvPushData(12, ftol[0]);
    nvPushData(13, ftol[1]);

    nvPushData(14, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXCOORD1_4F(0) | 0x100000);
    nvPushData(15, ftol[0]);
    nvPushData(16, ftol[0]);
    nvPushData(17, ftol[0]);
    nvPushData(18, ftol[1]);

    nvPushData(19, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_NORMAL3F(0) | 0xC0000);
    nvPushData(20, ftol[0]);
    nvPushData(21, ftol[0]);
    nvPushData(22, ftol[1]);

    nvPushData(23, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_FOG1F | 0x40000);
    nvPushData(24, ftol[0]);

    nvPushData(25, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_WEIGHT1F | 0x40000);
    nvPushData(26, ftol[1]);

    nvPusherAdjust(27);

    // Set depth function
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_DEPTH_FUNC | 0x40000);
    nvPushData(1, NV056_SET_DEPTH_FUNC_V_ALWAYS);

    // Set color mask
    tmpVal = (NV056_SET_COLOR_MASK_ALPHA_WRITE_ENABLE_TRUE << 24) |
             (NV056_SET_COLOR_MASK_RED_WRITE_ENABLE_TRUE << 16) |
             (NV056_SET_COLOR_MASK_GREEN_WRITE_ENABLE_TRUE << 8) |
              NV056_SET_COLOR_MASK_BLUE_WRITE_ENABLE_TRUE;

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COLOR_MASK | 0x40000);
    nvPushData(3, tmpVal);

    // Set depth mask
    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_DEPTH_MASK | 0x40000);
    nvPushData(5, NV056_SET_DEPTH_MASK_V_FALSE);

    nvPusherAdjust(6);

    // Initialize stencil state (test already disabled above)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_STENCIL_FUNC | 0x40000);
    nvPushData(1, NV056_SET_STENCIL_FUNC_V_ALWAYS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_STENCIL_FUNC_REF | 0x40000);
    nvPushData(3, 0);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_STENCIL_FUNC_MASK | 0x40000);
    nvPushData(5, 0x000000FF);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_STENCIL_OP_FAIL | 0x40000);
    nvPushData(7, NV056_SET_STENCIL_OP_FAIL_V_KEEP);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_STENCIL_OP_ZFAIL | 0x40000);
    nvPushData(9, NV056_SET_STENCIL_OP_ZFAIL_V_KEEP);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_STENCIL_OP_ZPASS | 0x40000);
    nvPushData(11, NV056_SET_STENCIL_OP_ZPASS_V_KEEP);

    nvPushData(12, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_STENCIL_MASK | 0x40000);
    nvPushData(13, 0x000000FF);

    nvPusherAdjust(14);

    // This is set once and forgotten. It puts the class in OGL flat shade mode.
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FLAT_SHADE_OP | 0x40000);
    nvPushData(1, NV056_SET_FLAT_SHADE_OP_V_LAST_VTX);

    nvPusherAdjust(2);

    // Load texture matrices assuming quarter pel scaling of texture coordinates
    if (pDriverData->bMCHorizontallyDownscale1080i)
        fBuffer[0]  = 0.1875;
    else
        fBuffer[0]  = 0.25;

                        fBuffer[1]  = 0.0;  fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
    fBuffer[4]  = 0.0;  fBuffer[5]  = 0.25; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
    fBuffer[8]  = 0.0;  fBuffer[9]  = 0.0;  fBuffer[10] = 1.0; fBuffer[11] = 0.0;
    fBuffer[12] = 0.0;  fBuffer[13] = 0.0;  fBuffer[14] = 0.0; fBuffer[15] = 1.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX0(0) | 0x400000);
    nvPushData(1, ftol[0]);
    nvPushData(2, ftol[4]);
    nvPushData(3, ftol[8]);
    nvPushData(4, ftol[12]);
    nvPushData(5, ftol[1]);
    nvPushData(6, ftol[5]);
    nvPushData(7, ftol[9]);
    nvPushData(8, ftol[13]);
    nvPushData(9, ftol[2]);
    nvPushData(10, ftol[6]);
    nvPushData(11, ftol[10]);
    nvPushData(12, ftol[14]);
    nvPushData(13, ftol[3]);
    nvPushData(14, ftol[7]);
    nvPushData(15, ftol[11]);
    nvPushData(16, ftol[15]);

    nvPushData(17, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_MATRIX1(0) | 0x400000);
    nvPushData(18, ftol[0]);
    nvPushData(19, ftol[4]);
    nvPushData(20, ftol[8]);
    nvPushData(21, ftol[12]);
    nvPushData(22, ftol[1]);
    nvPushData(23, ftol[5]);
    nvPushData(24, ftol[9]);
    nvPushData(25, ftol[13]);
    nvPushData(26, ftol[2]);
    nvPushData(27, ftol[6]);
    nvPushData(28, ftol[10]);
    nvPushData(29, ftol[14]);
    nvPushData(30, ftol[3]);
    nvPushData(31, ftol[7]);
    nvPushData(32, ftol[11]);
    nvPushData(33, ftol[15]);

    nvPusherAdjust(34);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitCelsiusForMoComp */



/*
 * nvInitCelsiusForMoCompPrediction
 *
 * Performs final celsius object motion comp initialization for
 * the prediction portion of the algorithm.
 *
 * For prediction, texture 0 is used for forward prediction, texture 1
 * is used for backward prediction.  These textures are just passed through
 * the register combiners to the final combiner.  The final combiner is
 * configured so that the fog coordinate is used to control the contribution
 * of each texture in the final output.  A fog value of 0.0 selects forward
 * prediction, 0.5 selects bidirectional prediction, and 1.0 selects backward
 * prediction.
 *
 * In this mode the register combiners are set to:
 *
 *      INPUT: A = (1.0)constantColor0, B = texture0, C = (1.0)constantColor0, D = texture1
 *      OUTPUT: spare0 = A * B, spare1 = C * D
 *
 * And the final combiner is set to:
 *
 *      INPUT: A.rgb = Fog.a, B = spare1, C = spare0, D = 0
 *      OUTPUT: B * A + C * (1.0 - A)
 *
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvInitCelsiusForMoCompPrediction(void)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl = pSurf_lcl->lpGbl;
    unsigned long               surfaceWidth = pSurf_gbl->wWidth;
    unsigned long               surfaceHeight = pSurf_gbl->wHeight;
    unsigned long               surfacePitch = pSurf_gbl->lPitch;
    unsigned long               dwSurfaceOffset;
    unsigned long               tmpVal;
    float                       fBuffer[16];
    unsigned long               *ftol = (unsigned long *)&fBuffer[0];
    unsigned long               moCompICW;
    unsigned long               moCompOCW;
    unsigned long               moCompFinalCW0;
    unsigned long               moCompFinalCW1;

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    // Initialize surface offsets
    dwSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
    nvPushData(3, dwSurfaceOffset);

    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCForwardSurface;
    pSurf_gbl = pSurf_lcl->lpGbl;

    dwSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(0) | 0x40000);
    nvPushData(5, dwSurfaceOffset);

    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface;
    pSurf_gbl = pSurf_lcl->lpGbl;

    dwSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(7, dwSurfaceOffset);

    nvPusherAdjust(8);

    // Align surface pitch and set surface format and pitch
    surfacePitch |= (surfacePitch << 16);
    surfacePitch <<= 1; // Surface is always referenced by hardware in field mode

    tmpVal = (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
              NV056_SET_SURFACE_FORMAT_COLOR_LE_B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(1, tmpVal);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_PITCH | 0x40000);
    nvPushData(3, surfacePitch);

    nvPusherAdjust(4);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX0_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_TEXTURE_MATRIX0_ENABLE_V_TRUE);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX1_ENABLE | 0x40000);
    nvPushData(3, NV056_SET_TEXTURE_MATRIX1_ENABLE_V_TRUE);

    nvMoCompParams.dataFormat = DATA_FORMAT_LUMA;

    tmpVal = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
             (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
             (1 << 12) | // 1 MIPMAP level
             (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8 << 7) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
             (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_FORMAT(0) | 0x80000);
    nvPushData(5, tmpVal);
    nvPushData(6, tmpVal);

    pDriverData->dwMCTex0Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(7, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL0(0) | 0x80000);
    nvPushData(8, pDriverData->dwMCTex0Control0);
    nvPushData(9, pDriverData->dwMCTex1Control0);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    nvPushData(11, surfacePitch << 16);
    nvPushData(12, surfacePitch << 16);

    nvPushData(13, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_CONTROL2(0) | 0x80000);
    nvPushData(14, 0);
    nvPushData(15, 0);

//    tmpVal = (2044 << 16) | (surfaceHeight << 1); // Must be even and <= 2046
    tmpVal = (2044 << 16) | 2046; // Must be even and <= 2046

    nvPushData(16, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_IMAGE_RECT(0) | 0x80000);
    nvPushData(17, tmpVal);
    nvPushData(18, tmpVal);

    tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR << 28) |
             (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR << 24);

    nvPushData(19, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_FILTER(0) | 0x80000);
    nvPushData(20, tmpVal);
    nvPushData(21, tmpVal);

    nvPusherAdjust(22);

    if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) {

        // Load composite and texture matrices assuming quarter pel horizontal scaling and eigth pel vertical
        // scaling of texture coordinates
        if (pDriverData->bMCHorizontallyDownscale1080i)
            fBuffer[0]  = 0.1875;
        else
            fBuffer[0]  = 0.25;

                            fBuffer[1]  = 0.0;   fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
        fBuffer[4]  = 0.0;  fBuffer[5]  = 0.125; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
        fBuffer[8]  = 0.0;  fBuffer[9]  = 0.0;   fBuffer[10] = 1.0; fBuffer[11] = 0.0;
        fBuffer[12] = 0.0;  fBuffer[13] = 0.0;   fBuffer[14] = 0.0; fBuffer[15] = 1.0;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_COMPOSITE_MATRIX(0) | 0x400000);
        nvPushData(1, ftol[0]);
        nvPushData(2, ftol[4]);
        nvPushData(3, ftol[8]);
        nvPushData(4, ftol[12]);
        nvPushData(5, ftol[1]);
        nvPushData(6, ftol[5]);
        nvPushData(7, ftol[9]);
        nvPushData(8, ftol[13]);
        nvPushData(9, ftol[2]);
        nvPushData(10, ftol[6]);
        nvPushData(11, ftol[10]);
        nvPushData(12, ftol[14]);
        nvPushData(13, ftol[3]);
        nvPushData(14, ftol[7]);
        nvPushData(15, ftol[11]);
        nvPushData(16, ftol[15]);

        nvPusherAdjust(17);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_MATRIX0(0) | 0x400000);
        nvPushData(1, ftol[0]);
        nvPushData(2, ftol[4]);
        nvPushData(3, ftol[8]);
        nvPushData(4, ftol[12]);
        nvPushData(5, ftol[1]);
        nvPushData(6, ftol[5]);
        nvPushData(7, ftol[9]);
        nvPushData(8, ftol[13]);
        nvPushData(9, ftol[2]);
        nvPushData(10, ftol[6]);
        nvPushData(11, ftol[10]);
        nvPushData(12, ftol[14]);
        nvPushData(13, ftol[3]);
        nvPushData(14, ftol[7]);
        nvPushData(15, ftol[11]);
        nvPushData(16, ftol[15]);

        nvPushData(17, dDrawSubchannelOffset(NV_DD_CELSIUS) +
            NV056_SET_TEXTURE_MATRIX1(0) | 0x400000);
        nvPushData(18, ftol[0]);
        nvPushData(19, ftol[4]);
        nvPushData(20, ftol[8]);
        nvPushData(21, ftol[12]);
        nvPushData(22, ftol[1]);
        nvPushData(23, ftol[5]);
        nvPushData(24, ftol[9]);
        nvPushData(25, ftol[13]);
        nvPushData(26, ftol[2]);
        nvPushData(27, ftol[6]);
        nvPushData(28, ftol[10]);
        nvPushData(29, ftol[14]);
        nvPushData(30, ftol[3]);
        nvPushData(31, ftol[7]);
        nvPushData(32, ftol[11]);
        nvPushData(33, ftol[15]);

        nvPusherAdjust(34);
    }

    // Enable fog (mocomp fog modes set during default mocomp initialization)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_FOG_ENABLE_V_TRUE);

    nvPusherAdjust(2);

    // Now setup combiners

    // Default register combiner settings are:
    // A = primary color
    // B = one
    // C = zero
    // D = zero

    // Our prediction moComp values
    // combiner0, RGB portion:   A = 1
    // combiner0, RGB portion:   B = texture0
    // combiner0, RGB portion:   C = 1
    // combiner0, RGB portion:   D = texture1

    moCompICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_INVERT << 29) |
                (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_0 << 24) |

                (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT << 13) |
                (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |

                (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                 NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_ICW(0) | 0x80000);
    nvPushData(1, DEFAULT_ALPHA_ICW);
    nvPushData(2, DEFAULT_ALPHA_ICW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_COLOR_ICW(0) | 0x80000);
    nvPushData(4, moCompICW);
    nvPushData(5, DEFAULT_COLOR_ICW);

    // Load combination factors
    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    nvPushData(7, 0xFFFFFFFF); // 100%
    nvPushData(8, 0xFFFFFFFF); // 100%

    nvPusherAdjust(9);

    // combiner0, RGB portion:   spare0 = texture0, spare1 = texture1
    moCompOCW = (NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT << 15) |
                (NV056_SET_COMBINER0_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                (NV056_SET_COMBINER0_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                (NV056_SET_COMBINER0_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                (NV056_SET_COMBINER0_COLOR_OCW_SUM_DST_REG_0 << 8) |
                (NV056_SET_COMBINER0_COLOR_OCW_AB_DST_REG_C << 4) |
                 NV056_SET_COMBINER0_COLOR_OCW_CD_DST_REG_D;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_OCW(0) | 0x80000);
    nvPushData(1, DEFAULT_ALPHA_OCW);
    nvPushData(2, DEFAULT_ALPHA_OCW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER0_COLOR_OCW | 0x40000);
    nvPushData(4, moCompOCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER1_COLOR_OCW | 0x40000);
    nvPushData(6, DEFAULT_C1_COLOR_OCW);

    nvPusherAdjust(7);

    // Reconfigure fog unit to interpolate
    // fog factor in alpha
    // fog = 0.0: all texture0
    // fog = 0.5: equal parts of texture0 and texture1
    // fog = 1.0: all texture1

    moCompFinalCW0 = (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE_FALSE << 29) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA_TRUE << 28) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_3 << 24) |   // fog

                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE_FALSE << 21) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA_FALSE << 20) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_D << 16) |   // spare1

                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE_FALSE << 13) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA_FALSE << 12) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_C << 8) |    // spare0

                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE_FALSE << 5) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA_FALSE << 4) |
                      NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_0;           // 0

    moCompFinalCW1 = (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE_FALSE << 29) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA_FALSE << 28) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_0 << 24) |   // 0

                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE_FALSE << 21) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA_FALSE << 20) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_0 << 16) |   // 0

                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE_TRUE << 13) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA_TRUE << 12) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_0 << 8) |    // 0xFF

                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP_TRUE << 7) |
                     (NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5_FALSE << 6) |
                      NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12_FALSE;

    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, moCompFinalCW0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, moCompFinalCW1);

    nvPusherAdjust(4);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitCelsiusForMoCompPrediction */


/*
 * nvInitCelsiusForMoCompCorrection
 *
 * Performs final celcius object motion comp initialization for
 * the correction portion of the algorithm.
 *
 * For correction, texture 0 is used for the error terms, texture 1
 * is mapped over the source/destination.  The error terms are signed
 * and the textures are unsigned so the register combiners need to be
 * setup appropriately.
 *
 * In this mode the register combiners are set to:
 *
 *      INPUT: A = (1.0)constantColor0, B = (signed)texture0, C = (1.0)constantColor0, D = (unsigned)texture1
 *      OUTPUT: spare0 = A * B + C * D
 *
 * And the final combiner is set to:
 *
 *      INPUT: C = spare0
 *      OUTPUT: C = spare0
 *
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvInitCelsiusForMoCompCorrection(void)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl = pSurf_lcl->lpGbl;
    unsigned long               surfaceWidth = pSurf_gbl->wWidth;
    unsigned long               surfaceHeight = pSurf_gbl->wHeight;
    unsigned long               surfacePitch = pSurf_gbl->lPitch;
    unsigned long               dwSurfaceOffset;
    unsigned long               texturePitch0;
    unsigned long               texturePitch1 = pSurf_gbl->lPitch;
    unsigned long               tmpVal;
    unsigned long               textureFormat0;
    unsigned long               textureFormat1;
    float                       fBuffer[16];
    unsigned long               *ftol = (unsigned long *)&fBuffer[0];
    unsigned long               moCompICW;
    unsigned long               moCompOCW;

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    // Initialize surface offsets
    dwSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
    nvPushData(3, dwSurfaceOffset);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
    nvPushData(5, 0);
    nvPushData(6, dwSurfaceOffset);

    nvPusherAdjust(7);

    // Align surface pitch and set surface format and pitch
    surfacePitch |= (surfacePitch << 16);

    if (pDriverData->bMCPictureStructure != PICTURE_STRUCTURE_FRAME)
        surfacePitch <<= 1; // Field surface is always referenced by hardware using double pitch

    tmpVal = (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
              NV056_SET_SURFACE_FORMAT_COLOR_LE_B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(1, tmpVal);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_PITCH | 0x40000);
    nvPushData(3, surfacePitch);

    nvPusherAdjust(4);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX0_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_TEXTURE_MATRIX0_ENABLE_V_TRUE);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX1_ENABLE | 0x40000);
    nvPushData(3, NV056_SET_TEXTURE_MATRIX1_ENABLE_V_TRUE);

    nvMoCompParams.dataFormat = DATA_FORMAT_CORRECTION;

    // Signed Y8
    textureFormat0 = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
                     (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
                     (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
                     (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
                     (1 << 12) | // 1 MIPMAP level
                     (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_SY8 << 7) |
                     (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
                     (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
                     (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
                      NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_B;
    // Unsigned Y8
    textureFormat1 = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
                     (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
                     (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
                     (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
                     (1 << 12) | // 1 MIPMAP level
                     (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8 << 7) |
                     (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
                     (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
                     (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
                      NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_FORMAT(0) | 0x80000);
    nvPushData(5, textureFormat0);
    nvPushData(6, textureFormat1);

    pDriverData->dwMCTex0Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(7, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL0(0) | 0x80000);
    nvPushData(8, pDriverData->dwMCTex0Control0);
    nvPushData(9, pDriverData->dwMCTex1Control0);

    nvPusherAdjust(10);

    // Correction textures ALWAYS have 8 byte frame pitches or 16 byte field pitches
    if (pDriverData->bMCPictureStructure != PICTURE_STRUCTURE_FRAME)
        texturePitch0 = 16;
    else {
        texturePitch0 = 8;

        // Load composite and texture matrices assuming quarter pel scaling of texture coordinates
        if (pDriverData->bMCHorizontallyDownscale1080i)
            fBuffer[0]  = 0.1875;
        else
            fBuffer[0]  = 0.25;

                            fBuffer[1]  = 0.0;  fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
        fBuffer[4]  = 0.0;  fBuffer[5]  = 0.25; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
        fBuffer[8]  = 0.0;  fBuffer[9]  = 0.0;  fBuffer[10] = 1.0; fBuffer[11] = 0.0;
        fBuffer[12] = 0.0;  fBuffer[13] = 0.0;  fBuffer[14] = 0.0; fBuffer[15] = 1.0;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_COMPOSITE_MATRIX(0) | 0x400000);
        nvPushData(1, ftol[0]);
        nvPushData(2, ftol[4]);
        nvPushData(3, ftol[8]);
        nvPushData(4, ftol[12]);
        nvPushData(5, ftol[1]);
        nvPushData(6, ftol[5]);
        nvPushData(7, ftol[9]);
        nvPushData(8, ftol[13]);
        nvPushData(9, ftol[2]);
        nvPushData(10, ftol[6]);
        nvPushData(11, ftol[10]);
        nvPushData(12, ftol[14]);
        nvPushData(13, ftol[3]);
        nvPushData(14, ftol[7]);
        nvPushData(15, ftol[11]);
        nvPushData(16, ftol[15]);

        nvPusherAdjust(17);

        if (pDriverData->bMCHorizontallyDownscale1080i)
            fBuffer[0]  = 0.25;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_MATRIX0(0) | 0x400000);
        nvPushData(1, ftol[0]);
        nvPushData(2, ftol[4]);
        nvPushData(3, ftol[8]);
        nvPushData(4, ftol[12]);
        nvPushData(5, ftol[1]);
        nvPushData(6, ftol[5]);
        nvPushData(7, ftol[9]);
        nvPushData(8, ftol[13]);
        nvPushData(9, ftol[2]);
        nvPushData(10, ftol[6]);
        nvPushData(11, ftol[10]);
        nvPushData(12, ftol[14]);
        nvPushData(13, ftol[3]);
        nvPushData(14, ftol[7]);
        nvPushData(15, ftol[11]);
        nvPushData(16, ftol[15]);

        if (pDriverData->bMCHorizontallyDownscale1080i)
            fBuffer[0]  = 0.1875;

        nvPushData(17, dDrawSubchannelOffset(NV_DD_CELSIUS) +
            NV056_SET_TEXTURE_MATRIX1(0) | 0x400000);
        nvPushData(18, ftol[0]);
        nvPushData(19, ftol[4]);
        nvPushData(20, ftol[8]);
        nvPushData(21, ftol[12]);
        nvPushData(22, ftol[1]);
        nvPushData(23, ftol[5]);
        nvPushData(24, ftol[9]);
        nvPushData(25, ftol[13]);
        nvPushData(26, ftol[2]);
        nvPushData(27, ftol[6]);
        nvPushData(28, ftol[10]);
        nvPushData(29, ftol[14]);
        nvPushData(30, ftol[3]);
        nvPushData(31, ftol[7]);
        nvPushData(32, ftol[11]);
        nvPushData(33, ftol[15]);

        nvPusherAdjust(34);
    }

    if (pDriverData->bMCPictureStructure != PICTURE_STRUCTURE_FRAME)
        texturePitch1 <<= 1; // Field textures are always referenced by hardware using double pitch

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    nvPushData(1, texturePitch0 << 16);
    nvPushData(2, texturePitch1 << 16);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL2(0) | 0x80000);
    nvPushData(4, 0);
    nvPushData(5, 0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_IMAGE_RECT(0) | 0x80000);
    nvPushData(7, (16 << 16) | 2044); // Must be even and <= 2046
//    nvPushData(8, ((2044 << 16) | (surfaceHeight << 1))); // Must be even and <= 2046
    nvPushData(8, ((2044 << 16) | 2046)); // Must be even and <= 2046

    tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_NEAREST << 28) |
             (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_NEAREST << 24);

    nvPushData(9, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_FILTER(0) | 0x80000);
    nvPushData(10, tmpVal);
    nvPushData(11, tmpVal);

    nvPusherAdjust(12);

    // Disable fog
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_FOG_ENABLE_V_FALSE);

    nvPusherAdjust(2);

    // Now setup combiners

    // Default register combiner settings are:
    // A = primary color
    // B = one
    // C = zero
    // D = zero

    // Our correction moComp values
    // combiner0, RGB portion:   A = 1
    // combiner0, RGB portion:   B = (signed)texture0
    // combiner0, RGB portion:   C = 1
    // combiner0, RGB portion:   D = (unsigned)texture1

    moCompICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                (NV056_SET_COMBINER_COLOR_ICW_B_MAP_HALFBIAS_NORMAL << 21) |
                (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1 << 8) |

                (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                 NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_ICW(0) | 0x80000);
    nvPushData(1, DEFAULT_ALPHA_ICW);
    nvPushData(2, DEFAULT_ALPHA_ICW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_COLOR_ICW(0) | 0x80000);
    nvPushData(4, moCompICW);
    nvPushData(5, DEFAULT_COLOR_ICW);

    // Load combination factors
    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    nvPushData(7, 0xFFFFFFFF); // 100%
    nvPushData(8, 0); // 0%

    nvPusherAdjust(9);

    // combiner0, RGB portion:   texture0 + texture1
    // spare0 = A * B + C * D
    moCompOCW = (NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT << 15) |
                (NV056_SET_COMBINER0_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                (NV056_SET_COMBINER0_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                (NV056_SET_COMBINER0_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                (NV056_SET_COMBINER0_COLOR_OCW_SUM_DST_REG_C << 8) |
                (NV056_SET_COMBINER0_COLOR_OCW_AB_DST_REG_0 << 4) |
                 NV056_SET_COMBINER0_COLOR_OCW_CD_DST_REG_0;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_OCW(0) | 0x80000);
    nvPushData(1, DEFAULT_ALPHA_OCW);
    nvPushData(2, DEFAULT_ALPHA_OCW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER0_COLOR_OCW | 0x40000);
    nvPushData(4, moCompOCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER1_COLOR_OCW | 0x40000);
    nvPushData(6, DEFAULT_C1_COLOR_OCW);

    nvPusherAdjust(7);

    // Load final stages with default values
    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, DEFAULT_FINAL_CW0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, DEFAULT_FINAL_CW1);

    nvPusherAdjust(4);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitCelsiusForMoCompCorrection */

#if 0 // Until NV17

/*
 * nvInitCelsiusForMoCompNV12LumaSubpictureCompositing
 *
 * Performs final celsius object motion comp initialization for
 * A8V8U8Y8 subpicture compositing with an NV12 surface luma portion resulting in an
 * output surface also in NV12 format.
 *
 * For luma subpicture compositing, texture0 is the subpicture in A8V8U8Y8 format from
 * which we extract the A8 and Y8 components and alpha blend with the Y8 components of
 * the NV12 source surface in texture1 to produce alpha blended Y8 components in NV12
 * format in the destination surface.
 * The output texture is just passed through the register combiners to the final combiner.
 *
 * In this mode the register combiners are set to:
 *
 *      INPUT: A = texture0 alpha, B = texture0 color B(Y), C = !texture0 alpha, D = texture1 color B(Y)
 *      OUTPUT: spare0 = A * B + C * D
 *
 * And the final combiner is set to:
 *
 *      INPUT: C = spare0
 *      OUTPUT: C = spare0
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvInitCelsiusForMoCompNV12LumaSubpictureCompositing(void)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl = pSurf_lcl->lpGbl;
    unsigned long               surfaceWidth = pSurf_gbl->wWidth;
    unsigned long               surfaceHeight = pSurf_gbl->wHeight;
    unsigned long               surfacePitch = pSurf_gbl->lPitch;
    unsigned long               texturePitch = pSurf_gbl->lPitch;
    unsigned long               tmpVal;
    unsigned long               moCompColorICW;
    unsigned long               moCompAlphaOCW;
    unsigned long               moCompColorOCW;
    float                       fBuffer[16];
    unsigned long               *ftol = (unsigned long *)&fBuffer[0];

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    // Surface offsets are initialized in the subpicture compositing function

    // Align surface pitch and set surface format and pitch
    surfacePitch |= (surfacePitch << 16);

    tmpVal = (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
              NV056_SET_SURFACE_FORMAT_COLOR_LE_B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(3, tmpVal);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_PITCH | 0x40000);
    nvPushData(5, surfacePitch);

    nvPusherAdjust(6);

    // Just disable both texture matrices since these texture coordinates are unscaled

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX0_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_TEXTURE_MATRIX0_ENABLE_V_FALSE);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX1_ENABLE | 0x40000);
    nvPushData(3, NV056_SET_TEXTURE_MATRIX1_ENABLE_V_FALSE);

    nvPusherAdjust(4);

    nvMoCompParams.dataFormat = DATA_FORMAT_SUBPICTURE_LUMA_BLEND;

    tmpVal = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
             (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
             (1 << 12) | // 1 MIPMAP level
             (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_A8R8G8B8 << 7) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
             (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_FORMAT(0) | 0x80000);
    nvPushData(1, tmpVal);
    
    tmpVal = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
             (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
             (1 << 12) | // 1 MIPMAP level
             (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8 << 7) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
             (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(2, tmpVal);

    pDriverData->dwMCTex0Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL0(0) | 0x80000);
    nvPushData(4, pDriverData->dwMCTex0Control0);
    nvPushData(5, pDriverData->dwMCTex1Control0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    nvPushData(7, texturePitch << 16);
    nvPushData(8, texturePitch << 16);

    nvPushData(9, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL2(0) | 0x80000);
    nvPushData(10, 0);
    nvPushData(11, 0);

//    tmpVal = (2044 << 16) | (surfaceHeight << 1); // Must be even and <= 2046
    tmpVal = (2044 << 16) | 2046; // Must be even and <= 2046

    nvPushData(12, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_IMAGE_RECT(0) | 0x80000);
    nvPushData(13, tmpVal);
    nvPushData(14, tmpVal);

    tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_NEAREST << 28) |
             (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_NEAREST << 24);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_FILTER(0) | 0x80000);
    nvPushData(16, tmpVal);
    nvPushData(17, tmpVal);

    nvPusherAdjust(18);

    // Disable fog
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_FOG_ENABLE_V_FALSE);

    nvPusherAdjust(2);

    // Now setup combiners

    // Default register combiner settings are:
    // A = primary color
    // B = one
    // C = zero
    // D = zero

    // Our subpictue luma alpha blending moComp values
    // combiner0, RGB portion:   A = texture0(Alpha)
    // combiner0, RGB portion:   B = texture0(RGB(VUY))
    // combiner0, RGB portion:   C = texture0(!Alpha)
    // combiner0, RGB portion:   D = texture1(BBB(YYY))

    moCompColorICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_TRUE << 28) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_8 << 24) |

                     (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                     (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT << 13) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_TRUE << 12) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_8 << 8) |

                     (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                      NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_ICW(0) | 0x80000);
    nvPushData(1, DEFAULT_ALPHA_ICW);
    nvPushData(2, DEFAULT_ALPHA_ICW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_COLOR_ICW(0) | 0x80000);
    nvPushData(4, moCompColorICW);
    nvPushData(5, DEFAULT_COLOR_ICW);

    // Load combination factors
    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    nvPushData(7, 0xFF0000FF); // 100%
    nvPushData(8, 0xFF0000FF); // 100%

    nvPusherAdjust(9);

    // combiner0, RGB portion:      spare0 = A * B + C * D = texture0 alpha * texture0 B(Y) + !texture0 alpha * texture1 B(Y)
    moCompColorOCW = (NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT << 15) |
                     (NV056_SET_COMBINER0_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV056_SET_COMBINER0_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                     (NV056_SET_COMBINER0_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                     (NV056_SET_COMBINER0_COLOR_OCW_SUM_DST_REG_C << 8) |
                     (NV056_SET_COMBINER0_COLOR_OCW_AB_DST_REG_0 << 4) |
                      NV056_SET_COMBINER0_COLOR_OCW_CD_DST_REG_0;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_OCW(0) | 0x80000);
    nvPushData(1, DEFAULT_ALPHA_OCW);
    nvPushData(2, DEFAULT_ALPHA_OCW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER0_COLOR_OCW | 0x40000);
    nvPushData(4, moCompColorOCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER1_COLOR_OCW | 0x40000);
    nvPushData(6, DEFAULT_C1_COLOR_OCW);

    nvPusherAdjust(7);

    // Load final stages with default values
    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, DEFAULT_FINAL_CW0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, DEFAULT_FINAL_CW1);

    nvPusherAdjust(4);


    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitCelsiusForMoCompNV12LumaSubpictureCompositing */


/*
 * nvInitCelsiusForMoCompNV12ChromaSubpictureCompositing
 *
 * Performs final celsius object motion comp initialization for
 * A8V8U8Y8 subpicture compositing with an NV12 surface chroma portion resulting in an
 * output surface also in NV12 format.
 *
 * For chroma subpicture compositing, texture0 is the subpicture in A8V8U8Y8 format from
 * which we extract the A8 and V8U8 components and alpha blend with the V8U8 components of
 * the NV12 source surface in texture1 to produce alpha blended V8U8 components in NV12
 * format in the destination surface.
 *
 * In this mode the register combiners are set to:
 *
 * COMBINER0:
 *      INPUT: A = texture0 alpha, B = texture0 color RGB(VUY), C = !texture0 alpha, D = texture1 color RBB(VUU)
 *      OUTPUT: spare0 = A * B + C * D
 * COMBINER1:
 *      INPUT: A = constant0 (0,1,0)(U), B = spare0 color (VUU), C = constant1(1,0,0)(V), D = spare0 color (VUU)
 *      OUTPUT: OCW C = A . B = UUU, OCW D = C . D = VVV
 *
 * And the final combiner is set to:
 *
 *      INPUT: C = UUU, D = VVV
 *      OUTPUT: C = VU
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvInitCelsiusForMoCompNV12ChromaSubpictureCompositing(void)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl = pSurf_lcl->lpGbl;
    unsigned long               surfaceWidth = pSurf_gbl->wWidth;
    unsigned long               surfaceHeight = pSurf_gbl->wHeight;
    unsigned long               surfacePitch = pSurf_gbl->lPitch;
    unsigned long               texturePitch = pSurf_gbl->lPitch;
    unsigned long               tmpVal;
    unsigned long               moCompColorICW;
    unsigned long               moCompColorOCW;
    unsigned long               moCompFinalCW;
    float                       fBuffer[16];
    unsigned long               *ftol = (unsigned long *)&fBuffer[0];

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    // Surface offsets are initialized in the subpicture compositing function

    // Align surface pitch and set surface format and pitch
    surfacePitch |= (surfacePitch << 16);

    tmpVal = (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
              NV056_SET_SURFACE_FORMAT_COLOR_LE_G8B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(3, tmpVal);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_PITCH | 0x40000);
    nvPushData(5, surfacePitch);

    nvPusherAdjust(6);

    // Just disable both texture matrices since these texture coordinates are unscaled

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX0_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_TEXTURE_MATRIX0_ENABLE_V_FALSE);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX1_ENABLE | 0x40000);
    nvPushData(3, NV056_SET_TEXTURE_MATRIX1_ENABLE_V_FALSE);

    nvPusherAdjust(4);

    nvMoCompParams.dataFormat = DATA_FORMAT_SUBPICTURE_CHROMA_BLEND;

    tmpVal = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
             (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
             (1 << 12) | // 1 MIPMAP level
             (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_A8R8G8B8 << 7) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
             (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_FORMAT(0) | 0x80000);
    nvPushData(1, tmpVal);
    
    tmpVal = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
             (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
             (1 << 12) | // 1 MIPMAP level
             (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_R8B8 << 7) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
             (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(2, tmpVal);

    pDriverData->dwMCTex0Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL0(0) | 0x80000);
    nvPushData(4, pDriverData->dwMCTex0Control0);
    nvPushData(5, pDriverData->dwMCTex1Control0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    nvPushData(7, texturePitch << 16);
    nvPushData(8, texturePitch << 16);

    nvPushData(9, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL2(0) | 0x80000);
    nvPushData(10, 0);
    nvPushData(11, 0);

//    tmpVal = (2044 << 16) | (surfaceHeight << 1); // Must be even and <= 2046
    tmpVal = (2044 << 16) | 2046; // Must be even and <= 2046

    nvPushData(12, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_IMAGE_RECT(0) | 0x80000);
    nvPushData(13, tmpVal);
    nvPushData(14, tmpVal);

    tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_NEAREST << 28) |
             (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_NEAREST << 24);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_FILTER(0) | 0x80000);
    nvPushData(16, tmpVal);
    nvPushData(17, tmpVal);

    nvPusherAdjust(18);

    // Disable fog
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_FOG_ENABLE_V_FALSE);

    nvPusherAdjust(2);

    // Now setup combiners

    // Default register combiner settings are:
    // A = primary color
    // B = one
    // C = zero
    // D = zero

    // Our subpicture chroma alpha blending moComp values for combiner0
    // combiner0, RGB portion:   A = texture0(Alpha)
    // combiner0, RGB portion:   B = texture0(RGB(VUY))
    // combiner0, RGB portion:   C = texture0(!Alpha)
    // combiner0, RGB portion:   D = texture1(RBB(VUU))

    moCompColorICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_TRUE << 28) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_8 << 24) |

                     (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                     (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT << 13) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_TRUE << 12) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_8 << 8) |

                     (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                      NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_ICW(0) | 0x80000);
    nvPushData(1, DEFAULT_ALPHA_ICW);
    nvPushData(2, DEFAULT_ALPHA_ICW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_COLOR_ICW(0) | 0x80000);
    nvPushData(4, moCompColorICW);

    // Our subpicture chroma alpha blending moComp values for combiner1
    // combiner1, RGB portion:   A = constant0(0,1,0)(U)
    // combiner1, RGB portion:   B = combiner0 color (final VUU)
    // combiner1, RGB portion:   C = constant1(1,0,0)(V)
    // combiner1, RGB portion:   D = combiner0 color (final VUU)

    moCompColorICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                     (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_C << 16) |

                     (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_2 << 8) |

                     (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                      NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_C;

    nvPushData(5, moCompColorICW);

    // Load combination factors
    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    nvPushData(7, 0x0000FF00); // 100%
    nvPushData(8, 0x00FF0000); // 100%

    nvPusherAdjust(9);

    // combiner0, RGB portion:      spare0 = A * B + C * D = texture0 alpha * texture0 RGB(VUY) + !texture0 alpha * texture1 RBB(VUU)
    moCompColorOCW = (NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT << 15) |
                     (NV056_SET_COMBINER0_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV056_SET_COMBINER0_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                     (NV056_SET_COMBINER0_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                     (NV056_SET_COMBINER0_COLOR_OCW_SUM_DST_REG_C << 8) |
                     (NV056_SET_COMBINER0_COLOR_OCW_AB_DST_REG_0 << 4) |
                      NV056_SET_COMBINER0_COLOR_OCW_CD_DST_REG_0;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_OCW(0) | 0x80000);
    nvPushData(1, DEFAULT_ALPHA_OCW);
    nvPushData(2, DEFAULT_ALPHA_OCW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER0_COLOR_OCW | 0x40000);
    nvPushData(4, moCompColorOCW);

    // combiner1, RGB portion:      OCW C = A . B = constant0 (0,1,0) * final U, OCW D = C . D = constant1(1,0,0) * final V
    moCompColorOCW = (NV056_SET_COMBINER1_COLOR_OCW_ITERATION_COUNT_TWO << 28) |
                     (NV056_SET_COMBINER1_COLOR_OCW_MUX_SELECT_MSB << 27) |
                     (NV056_SET_COMBINER1_COLOR_OCW_OPERATION_NOSHIFT << 15) |
                     (NV056_SET_COMBINER1_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV056_SET_COMBINER1_COLOR_OCW_AB_DOT_ENABLE_TRUE << 13) |
                     (NV056_SET_COMBINER1_COLOR_OCW_CD_DOT_ENABLE_TRUE << 12) |
                     (NV056_SET_COMBINER1_COLOR_OCW_SUM_DST_REG_0 << 8) |
                     (NV056_SET_COMBINER1_COLOR_OCW_AB_DST_REG_C << 4) |
                      NV056_SET_COMBINER1_COLOR_OCW_CD_DST_REG_D;

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER1_COLOR_OCW | 0x40000);
    nvPushData(6, moCompColorOCW);

    nvPusherAdjust(7);

    // Load final stages: final output (VU) = constant0(0,1,0) * OCW D (VVV) + (1 - constant0)(1,0,1) * OCW C (UUU) + 0
    // Initialize combiner final CW methods

    moCompFinalCW = (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE_FALSE << 29) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA_FALSE << 28) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_1 << 24) |
                                                                                          
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE_FALSE << 21) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA_FALSE << 20) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_D << 16) |
                                                                                           
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE_FALSE << 13) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA_FALSE << 12) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_C << 8) |
                                                                                         
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE_FALSE << 5) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA_FALSE << 4) |
                     NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, moCompFinalCW);
    
    moCompFinalCW = (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE_FALSE << 29) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA_FALSE << 28) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_0 << 24) |
                                                                                          
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE_FALSE << 21) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA_FALSE << 20) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_0 << 16) |
                                                                                      
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE_FALSE << 13) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA_FALSE << 12) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_0 << 8) |
                                                                                          
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP_TRUE << 7) |
                    (NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5_FALSE << 6) |
                     NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12_FALSE;
    
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, moCompFinalCW);

    nvPusherAdjust(4);


    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitCelsiusForMoCompNV12ChromaSubpictureCompositing */

#endif // Until NV17

/*
 * nvInitCelsiusForMoCompConversion
 *
 * Performs final celsius object motion comp initialization for
 * the format conversion portion of the algorithm.
 *
 * For conversion, texture0 is used to convert the luma values to 0x00Y100Y0 and
 * texture1 is used to convert the chroma values to 0xV000U000 which are then added together
 * thus allowing the conversion to take place in one pass.  The textures are modulated
 * with constant colors to select which component comes from which texture.
 * The output texture is just passed through the register combiners to the final combiner.
 *
 * In this mode the register combiners are set to:
 *
 *      INPUT: A = constantColor0, B = texture0, C = constantColor1, D = texture1
 *      OUTPUT: spare0 = A * B + C * D
 *
 * And the final combiner is set to:
 *
 *      INPUT: C = spare0
 *      OUTPUT: C = spare0
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvInitCelsiusForMoCompConversion(void)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl = pSurf_lcl->lpGbl;
    unsigned long               surfaceWidth = pSurf_gbl->wWidth;
    unsigned long               surfaceHeight = pSurf_gbl->wHeight;
    unsigned long               surfacePitch = pSurf_gbl->lPitch;
    unsigned long               texturePitch = pSurf_gbl->lPitch;
    unsigned long               tmpVal;
    unsigned long               moCompAlphaICW;
    unsigned long               moCompColorICW;
    unsigned long               moCompAlphaOCW;
    unsigned long               moCompColorOCW;
    float                       fBuffer[16];
    unsigned long               *ftol = (unsigned long *)&fBuffer[0];

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    if (pDriverData->bMCHorizontallyDownscale1080i) {
        // Restore default composite and texture matrices assuming quarter pel scaling of texture coordinates
        fBuffer[0]  = 0.1875; fBuffer[1]  = 0.0;  fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
        fBuffer[4]  = 0.0;    fBuffer[5]  = 0.25; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
        fBuffer[8]  = 0.0;    fBuffer[9]  = 0.0;  fBuffer[10] = 1.0; fBuffer[11] = 0.0;
        fBuffer[12] = 0.0;    fBuffer[13] = 0.0;  fBuffer[14] = 0.0; fBuffer[15] = 1.0;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_COMPOSITE_MATRIX(0) | 0x400000);
        nvPushData(1, ftol[0]);
        nvPushData(2, ftol[4]);
        nvPushData(3, ftol[8]);
        nvPushData(4, ftol[12]);
        nvPushData(5, ftol[1]);
        nvPushData(6, ftol[5]);
        nvPushData(7, ftol[9]);
        nvPushData(8, ftol[13]);
        nvPushData(9, ftol[2]);
        nvPushData(10, ftol[6]);
        nvPushData(11, ftol[10]);
        nvPushData(12, ftol[14]);
        nvPushData(13, ftol[3]);
        nvPushData(14, ftol[7]);
        nvPushData(15, ftol[11]);
        nvPushData(16, ftol[15]);

        nvPusherAdjust(17);
    }

    // Surface offsets are initialized in the format conversion function

    // Align surface pitch and set surface format and pitch
    surfacePitch = ((surfaceWidth << 1) + 127) & ~127; // YUY2 destination is twice the surface width
    surfacePitch |= (surfacePitch << 16);

    tmpVal = (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
              NV056_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(3, tmpVal);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_PITCH | 0x40000);
    nvPushData(5, surfacePitch);

    nvPusherAdjust(6);

    // Just disable both texture matrices since these texture coordinates are unscaled

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX0_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_TEXTURE_MATRIX0_ENABLE_V_FALSE);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX1_ENABLE | 0x40000);
    nvPushData(3, NV056_SET_TEXTURE_MATRIX1_ENABLE_V_FALSE);

    nvPusherAdjust(4);

    nvMoCompParams.dataFormat = DATA_FORMAT_FOURCC_CONVERSION;

    tmpVal = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
             (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
             (1 << 12) | // 1 MIPMAP level
             (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_R8B8 << 7) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
             (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_FORMAT(0) | 0x80000);
    nvPushData(1, tmpVal);
    nvPushData(2, tmpVal);

    pDriverData->dwMCTex0Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL0(0) | 0x80000);
    nvPushData(4, pDriverData->dwMCTex0Control0);
    nvPushData(5, pDriverData->dwMCTex1Control0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    nvPushData(7, texturePitch << 16);
    nvPushData(8, texturePitch << 16);

    nvPushData(9, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL2(0) | 0x80000);
    nvPushData(10, 0);
    nvPushData(11, 0);

//    tmpVal = (2044 << 16) | (surfaceHeight << 1); // Must be even and <= 2046
    tmpVal = (2044 << 16) | 2046; // Must be even and <= 2046

    nvPushData(12, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_IMAGE_RECT(0) | 0x80000);
    nvPushData(13, tmpVal);
    nvPushData(14, tmpVal);

    tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_NEAREST << 28) |
             (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_NEAREST << 24);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_FILTER(0) | 0x80000);
    nvPushData(16, tmpVal);
    nvPushData(17, tmpVal);

    nvPusherAdjust(18);

    // Disable fog
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_FOG_ENABLE_V_FALSE);

    nvPusherAdjust(2);

    // Now setup combiners

    // Default register combiner settings are:
    // A = primary color
    // B = one
    // C = zero
    // D = zero

    // Our format conversion moComp values
    // combiner0, Alpha portion:   A = 0
    // combiner0, Alpha portion:   B = 0
    // combiner0, Alpha portion:   C = constantColor1 (0xFF00FF00)
    // combiner0, Alpha portion:   D = texture1
    // combiner0, RGB portion:   A = constantColor0 (0x00FF00FF)
    // combiner0, RGB portion:   B = texture0
    // combiner0, RGB portion:   C = constantColor1 (0xFF00FF00)
    // combiner0, RGB portion:   D = texture1

    moCompAlphaICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_TRUE << 28) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_0 << 24) |

                     (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_TRUE << 20) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_0 << 16) |

                     (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_TRUE << 12) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_2 << 8) |

                     (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_TRUE << 4) |
                      NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    moCompColorICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                     (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                     (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_2 << 8) |

                     (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                      NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_ICW(0) | 0x80000);
    nvPushData(1, moCompAlphaICW);
    nvPushData(2, DEFAULT_ALPHA_ICW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_COLOR_ICW(0) | 0x80000);
    nvPushData(4, moCompColorICW);
    nvPushData(5, DEFAULT_COLOR_ICW);

    // Load combination factors
    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    nvPushData(7, 0x00FF00FF); // luma
    nvPushData(8, 0xFF00FF00); // chroma

    nvPusherAdjust(9);

    // combiner0, Alpha portion:    spare0 = A * B + C * D = texture1
    moCompAlphaOCW = (NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT << 15) |
                     (NV056_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV056_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C << 8) |
                     (NV056_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0 << 4) |
                      NV056_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0;

    // combiner0, RGB portion:      spare0 = A * B + C * D = texture0 + texture1
    moCompColorOCW = (NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT << 15) |
                     (NV056_SET_COMBINER0_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV056_SET_COMBINER0_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                     (NV056_SET_COMBINER0_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                     (NV056_SET_COMBINER0_COLOR_OCW_SUM_DST_REG_C << 8) |
                     (NV056_SET_COMBINER0_COLOR_OCW_AB_DST_REG_0 << 4) |
                      NV056_SET_COMBINER0_COLOR_OCW_CD_DST_REG_0;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_OCW(0) | 0x80000);
    nvPushData(1, moCompAlphaOCW);
    nvPushData(2, DEFAULT_ALPHA_OCW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER0_COLOR_OCW | 0x40000);
    nvPushData(4, moCompColorOCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER1_COLOR_OCW | 0x40000);
    nvPushData(6, DEFAULT_C1_COLOR_OCW);

    nvPusherAdjust(7);

    // Load final stages with default values
    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, DEFAULT_FINAL_CW0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, DEFAULT_FINAL_CW1);

    nvPusherAdjust(4);


    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitCelsiusForMoCompConversion */


/*
 * nvInitCelsiusForMoCompTemporalFilter
 *
 * Performs final celsius object motion comp initialization for
 * the intra frame field temporal filtering portion of the algorithm.
 *
 * For temporal filtering, texture0 is used to reference the top field and
 * texture1 is used to reference the bottom field which are then added together
 * thus allowing the filter to complete in one pass.  The textures are modulated
 * with constant colors to select the fraction of coverage which comes from which texture.
 * The output texture is just passed through the register combiners to the final combiner.
 *
 * In this mode the register combiners are set to:
 *
 *      INPUT: A = constantColor0, B = texture0, C = constantColor1, D = texture1
 *      OUTPUT: spare0 = A * B + C * D
 *
 * And the final combiner is set to:
 *
 *      INPUT: C = spare0
 *      OUTPUT: C = spare0
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvInitCelsiusForMoCompTemporalFilter(void)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl = pSurf_lcl->lpGbl;
    unsigned long               surfaceWidth = pSurf_gbl->wWidth;
    unsigned long               surfaceHeight = pSurf_gbl->wHeight;
    unsigned long               surfacePitch;
    unsigned long               tmpVal;
    unsigned long               moCompAlphaICW;
    unsigned long               moCompColorICW;
    unsigned long               moCompAlphaOCW;
    unsigned long               moCompColorOCW;

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    // Surface offsets are initialized in the temporal filter function

    // Align surface pitch and set surface format and pitch
    surfacePitch = (pSurf_gbl->wWidth + 3) & ~3;
    surfacePitch = ((surfacePitch << 1) + 127) & ~127; // YUY2 destination is twice the surface width
    surfacePitch |= (surfacePitch << 16);
    surfacePitch <<= 1;

    tmpVal = (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
              NV056_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(3, tmpVal);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_PITCH | 0x40000);
    nvPushData(5, surfacePitch);

    nvPusherAdjust(6);

    // Enable both texture matrices since these texture coordinates are in quarter pel
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX0_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_TEXTURE_MATRIX0_ENABLE_V_TRUE);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_MATRIX1_ENABLE | 0x40000);
    nvPushData(3, NV056_SET_TEXTURE_MATRIX1_ENABLE_V_TRUE);

    nvPusherAdjust(4);

    nvMoCompParams.dataFormat = DATA_FORMAT_TEMPORAL_FILTER;

    tmpVal = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
             (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
             (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
             (1 << 12) | // 1 MIPMAP level
             (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_A8R8G8B8 << 7) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
             (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
             (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_FORMAT(0) | 0x80000);
    nvPushData(1, tmpVal);
    nvPushData(2, tmpVal);

    pDriverData->dwMCTex0Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                                    (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL0(0) | 0x80000);
    nvPushData(4, pDriverData->dwMCTex0Control0);
    nvPushData(5, pDriverData->dwMCTex1Control0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    nvPushData(7, (surfacePitch << 16));
    nvPushData(8, (surfacePitch << 16));

    nvPushData(9, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL2(0) | 0x80000);
    nvPushData(10, 0);
    nvPushData(11, 0);

//    tmpVal = (2044 << 16) | (surfaceHeight << 1); // Must be even and <= 2046
    tmpVal = (2044 << 16) | 2046; // Must be even and <= 2046

    nvPushData(12, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_IMAGE_RECT(0) | 0x80000);
    nvPushData(13, tmpVal);
    nvPushData(14, tmpVal);

    tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR << 28) |
             (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR << 24);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_FILTER(0) | 0x80000);
    nvPushData(16, tmpVal);
    nvPushData(17, tmpVal);

    nvPusherAdjust(18);

    // Disable fog
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV056_SET_FOG_ENABLE_V_FALSE);

    nvPusherAdjust(2);

    // Now setup combiners

    // Default register combiner settings are:
    // A = primary color
    // B = one
    // C = zero
    // D = zero

    // Our temporal filter moComp values
    // combiner0, RGB portion:   A = constantColor0
    // combiner0, RGB portion:   B = texture0
    // combiner0, RGB portion:   C = constantColor1
    // combiner0, RGB portion:   D = texture1

    moCompAlphaICW = (NV056_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV056_SET_COMBINER_ALPHA_ICW_A_ALPHA_TRUE << 28) |
                     (NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1 << 24) |

                     (NV056_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV056_SET_COMBINER_ALPHA_ICW_B_ALPHA_TRUE << 20) |
                     (NV056_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_8 << 16) |

                     (NV056_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV056_SET_COMBINER_ALPHA_ICW_C_ALPHA_TRUE << 12) |
                     (NV056_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_2 << 8) |

                     (NV056_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV056_SET_COMBINER_ALPHA_ICW_D_ALPHA_TRUE << 4) |
                      NV056_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_9;

    moCompColorICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                     (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                     (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                     (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                     (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                     (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_2 << 8) |

                     (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                      NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_ICW(0) | 0x80000);
    nvPushData(1, moCompAlphaICW);
    nvPushData(2, DEFAULT_ALPHA_ICW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_COLOR_ICW(0) | 0x80000);
    nvPushData(4, moCompColorICW);
    nvPushData(5, DEFAULT_COLOR_ICW);

    // Load combination factors
    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    nvPushData(7, 0x80808080); // % of top field
    nvPushData(8, 0x80808080); // % of bottom field

    nvPusherAdjust(9);

    // combiner0, RGB portion:      spare0 = A * B + C * D = texture0 + texture1
    moCompAlphaOCW = (NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT << 15) |
                     (NV056_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV056_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C << 8) |
                     (NV056_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0 << 4) |
                      NV056_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0;

    moCompColorOCW = (NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT << 15) |
                     (NV056_SET_COMBINER0_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV056_SET_COMBINER0_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                     (NV056_SET_COMBINER0_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                     (NV056_SET_COMBINER0_COLOR_OCW_SUM_DST_REG_C << 8) |
                     (NV056_SET_COMBINER0_COLOR_OCW_AB_DST_REG_0 << 4) |
                      NV056_SET_COMBINER0_COLOR_OCW_CD_DST_REG_0;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_ALPHA_OCW(0) | 0x80000);
    nvPushData(1, moCompAlphaOCW);
    nvPushData(2, DEFAULT_ALPHA_OCW);

    nvPushData(3, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER0_COLOR_OCW | 0x40000);
    nvPushData(4, moCompColorOCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER1_COLOR_OCW | 0x40000);
    nvPushData(6, DEFAULT_C1_COLOR_OCW);

    nvPusherAdjust(7);

    // Load final stages with default values
    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, DEFAULT_FINAL_CW0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, DEFAULT_FINAL_CW1);

    nvPusherAdjust(4);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitCelsiusForMoCompTemporalFilter */



#endif  // NVARCH >= 0x10

