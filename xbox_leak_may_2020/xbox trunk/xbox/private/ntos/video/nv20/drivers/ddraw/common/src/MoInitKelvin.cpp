
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
 *  File:       MoInitKelvin.cpp
 *  Content:    Windows98 DirectDraw 32 bit driver
 *
 ***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

extern NVMOCOMPPARAMS nvMoCompParams;

/*
 * nvInitKelvinForMoComp
 *
 * Performs basic motion comp initialization of the kelvin class object
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvInitKelvinForMoComp(void)
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

	if(dstx == NULL)	//it happens with WinDVD
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

    // Reset the kelvin context Dmas to use the default video context Dma
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    nvPushData(1, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CONTEXT_DMA_A | 0x40000);
    nvPushData(3, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CONTEXT_DMA_B | 0x40000);
    nvPushData(5, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CONTEXT_DMA_STATE | 0x40000);
    nvPushData(7, NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CONTEXT_DMA_COLOR | 0x40000);
    nvPushData(9, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CONTEXT_DMA_ZETA | 0x40000);
    nvPushData(11, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_CONTEXT_DMA_VERTEX_A | 0x40000);
    nvPushData(13, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_CONTEXT_DMA_VERTEX_B | 0x40000);
    nvPushData(15, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    nvPushData(16, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_CONTEXT_DMA_SEMAPHORE | 0x40000);
    nvPushData(17, NV01_NULL_OBJECT);

    nvPushData(18, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_CONTEXT_DMA_REPORT | 0x40000);
    nvPushData(19, NV01_NULL_OBJECT);

    nvPusherAdjust(20);

    // Set surface clip rectangle
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_CLIP_HORIZONTAL | 0x40000);
    // pitch << 1
    nvPushData(1, ((surfacePitch << 17) | 0));

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_CLIP_VERTICAL | 0x40000);
    // height << 1 which is at least enough for luma height + chroma height
    nvPushData(3, ((surfaceHeight << 17) | 0));

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_WINDOW_CLIP_TYPE | 0x40000);
    nvPushData(5, NV097_SET_WINDOW_CLIP_TYPE_V_INCLUSIVE);

    // Set up one window clip rectangle to be the rendered area
    // All the other rectangles are deactivated
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_WINDOW_CLIP_HORIZONTAL(0) | 0x200000);
    nvPushData(7, ((4095 << 16) | 0));
    nvPushData(8, 0);
    nvPushData(9, 0);
    nvPushData(10, 0);
    nvPushData(11, 0);
    nvPushData(12, 0);
    nvPushData(13, 0);
    nvPushData(14, 0);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_WINDOW_CLIP_VERTICAL(0) | 0x200000);
    nvPushData(16, ((4095 << 16) | 0));
    nvPushData(17, 0);
    nvPushData(18, 0);
    nvPushData(19, 0);
    nvPushData(20, 0);
    nvPushData(21, 0);
    nvPushData(22, 0);
    nvPushData(23, 0);

    nvPusherAdjust(24);

    // Set Control0 defaults
    tmpVal = (NV097_SET_CONTROL0_COLOR_SPACE_CONVERT_PASS << 28) |
             (NV097_SET_CONTROL0_PREMULTIPLIEDALPHA_FALSE << 24) |
             (NV097_SET_CONTROL0_TEXTUREPERSPECTIVE_FALSE << 20) |
             (NV097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE_FALSE << 16) |
             (NV097_SET_CONTROL0_Z_FORMAT_FIXED << 12) |
             (NV097_SET_CONTROL0_STENCIL_WRITE_ENABLE_TRUE);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CONTROL0 | 0x40000);
    nvPushData(1, tmpVal);

    nvPusherAdjust(2);

    // Make sure to disable any transform program which may be running in D3D
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TRANSFORM_EXECUTION_MODE | 0x40000);
    nvPushData(1, ((NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV << 2) |
                    NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED));

    nvPusherAdjust(2);

    // Load Projection, ModelView, and inverse ModelView matrix with identity
    fBuffer[0]  = 1.0; fBuffer[1]  = 0.0; fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
    fBuffer[4]  = 0.0; fBuffer[5]  = 1.0; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
    fBuffer[8]  = 0.0; fBuffer[9]  = 0.0; fBuffer[10] = 1.0; fBuffer[11] = 0.0;
    fBuffer[12] = 0.0; fBuffer[13] = 0.0; fBuffer[14] = 0.0; fBuffer[15] = 1.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_PROJECTION_MATRIX(0) | 0x400000);
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

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_MODEL_VIEW_MATRIX0(0) | 0x400000);
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

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0) | 0x400000);
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

    // Align surface pitch and set surface format and pitch
    surfacePitch |= (surfacePitch << 16);
    surfacePitch <<= 1; // Surface is always referenced by hardware in field mode

    tmpVal = (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) |
             (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
             (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) |
              NV097_SET_SURFACE_FORMAT_COLOR_LE_B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(1, tmpVal);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_PITCH | 0x40000);
    nvPushData(3, surfacePitch);

    nvPusherAdjust(4);

    // Set up viewport
    xTrans = (float)(1.0 / 32.0); // hardware epsilon
    yTrans = (float)(1.0 / 32.0);
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

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMPOSITE_MATRIX(0) | 0x400000);
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

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_VIEWPORT_OFFSET(0) | 0x100000);
    nvPushData(1, ftol[0]);
    nvPushData(2, ftol[1]);
    nvPushData(3, ftol[2]);
    nvPushData(4, ftol[3]);

    nvPusherAdjust(5);

#if (NVARCH >= 0x20)
    // Set front and back clipping
    fBuffer[0] = 0.0;
    fBuffer[1] = KELVIN_Z_SCALE24;
#endif

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CLIP_MIN | 0x80000);
    nvPushData(1, ftol[0]);
    nvPushData(2, ftol[1]);

    nvPusherAdjust(3);

    // Set light control
    tmpVal = (NV097_SET_LIGHT_CONTROL_SOUT_ZERO_OUT << 17) |
             (NV097_SET_LIGHT_CONTROL_LOCALEYE_FALSE << 16) |
              NV097_SET_LIGHT_CONTROL_SEPARATE_SPECULAR_EN_FALSE;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_LIGHT_CONTROL | 0x40000);
    nvPushData(1, tmpVal);

    // Disable color material
    tmpVal = (NV097_SET_COLOR_MATERIAL_BACK_SPECULAR_MATERIAL_DISABLE << 14) |
             (NV097_SET_COLOR_MATERIAL_BACK_DIFF_MATERIAL_DISABLE << 12) |
             (NV097_SET_COLOR_MATERIAL_BACK_AMBIENT_MATERIAL_DISABLE << 10) |
             (NV097_SET_COLOR_MATERIAL_BACK_EMISSIVE_MATERIAL_DISABLE << 8) |
             (NV097_SET_COLOR_MATERIAL_SPECULAR_MATERIAL_DISABLE << 6) |
             (NV097_SET_COLOR_MATERIAL_DIFF_MATERIAL_DISABLE << 4) |
             (NV097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL_DISABLE << 2) |
              NV097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL_DISABLE;

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COLOR_MATERIAL | 0x40000);
    nvPushData(3, tmpVal);

    nvPusherAdjust(4);

    // Set default fog mode
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_MODE | 0x40000);
    nvPushData(1, NV097_SET_FOG_MODE_V_LINEAR);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_GEN_MODE | 0x40000);
//    nvPushData(3, NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA);
    nvPushData(3, NV097_SET_FOG_GEN_MODE_V_FOG_X);

    // Disable fog
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_ENABLE | 0x40000);
    nvPushData(5, NV097_SET_FOG_ENABLE_V_FALSE);

    // Set default fog color
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_COLOR | 0x40000);
    nvPushData(7, 0xFFFFFFFF);

    nvPusherAdjust(8);

    // Disable almost everything

    // Disable alpha test
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_ALPHA_TEST_ENABLE | 0x40000);
    nvPushData(1, NV097_SET_ALPHA_TEST_ENABLE_V_FALSE);

    // Disable blending
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BLEND_ENABLE | 0x40000);
    nvPushData(3, NV097_SET_BLEND_ENABLE_V_FALSE);

    // Disable culling
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_CULL_FACE_ENABLE | 0x40000);
    nvPushData(5, NV097_SET_CULL_FACE_ENABLE_V_FALSE);

    // Disable depth test
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_DEPTH_TEST_ENABLE | 0x40000);
    nvPushData(7, NV097_SET_DEPTH_TEST_ENABLE_V_FALSE);

    // Disable dither
    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_DITHER_ENABLE | 0x40000);
    nvPushData(9, NV097_SET_DITHER_ENABLE_V_FALSE);

    // Disable lighting
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_LIGHTING_ENABLE | 0x40000);
    nvPushData(11, NV097_SET_LIGHTING_ENABLE_V_FALSE);

    // Disable point parameters computation
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_POINT_PARAMS_ENABLE | 0x40000);
    nvPushData(13, NV097_SET_POINT_PARAMS_ENABLE_V_FALSE);

    // Disable point smoothing
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_POINT_SMOOTH_ENABLE | 0x40000);
    nvPushData(15, NV097_SET_LINE_SMOOTH_ENABLE_V_FALSE);

    // Disable line smoothing
    nvPushData(16, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_LINE_SMOOTH_ENABLE | 0x40000);
    nvPushData(17, NV097_SET_LINE_SMOOTH_ENABLE_V_FALSE);

    // Disable polygon smoothing
    nvPushData(18, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_POLY_SMOOTH_ENABLE | 0x40000);
    nvPushData(19, NV097_SET_POLY_SMOOTH_ENABLE_V_FALSE);

    // Disable stippling
    nvPushData(20, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_STIPPLE_CONTROL | 0x40000);
    nvPushData(21, NV097_SET_STIPPLE_CONTROL_V_OFF);

    // Disable skinning
    nvPushData(22, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_SKIN_MODE | 0x40000);
    nvPushData(23, NV097_SET_SKIN_MODE_V_OFF);

    // Disable stencil test
    nvPushData(24, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_STENCIL_TEST_ENABLE | 0x40000);
    nvPushData(25, NV097_SET_STENCIL_TEST_ENABLE_V_FALSE);

    // Disable polygon offset point
    nvPushData(26, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_POLY_OFFSET_POINT_ENABLE | 0x40000);
    nvPushData(27, NV097_SET_POLY_OFFSET_POINT_ENABLE_V_FALSE);

    // Disable polygon offset line
    nvPushData(28, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_POLY_OFFSET_LINE_ENABLE | 0x40000);
    nvPushData(29, NV097_SET_POLY_OFFSET_LINE_ENABLE_V_FALSE);

    // Disable polygon offset fill
    nvPushData(30, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_POLY_OFFSET_FILL_ENABLE | 0x40000);
    nvPushData(31, NV097_SET_POLY_OFFSET_FILL_ENABLE_V_FALSE);

    nvPusherAdjust(32);

    // Set default alpha function
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_ALPHA_FUNC | 0x40000);
    nvPushData(1, NV097_SET_ALPHA_FUNC_V_ALWAYS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_ALPHA_REF | 0x40000);
    nvPushData(3, 0);

    // Initialize blend factors for later use
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BLEND_FUNC_SFACTOR | 0x40000);
    nvPushData(5, NV097_SET_BLEND_FUNC_SFACTOR_V_ONE);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BLEND_FUNC_DFACTOR | 0x40000);
    nvPushData(7, NV097_SET_BLEND_FUNC_DFACTOR_V_ONE);

    // Set default blend color
    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BLEND_COLOR | 0x40000);
    nvPushData(9, 0);

    // Set default blend equation
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BLEND_EQUATION | 0x40000);
    nvPushData(11, NV097_SET_BLEND_EQUATION_V_FUNC_ADD_SIGNED);

    nvPusherAdjust(12);

    // Set depth function
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_DEPTH_FUNC | 0x40000);
    nvPushData(1, NV097_SET_DEPTH_FUNC_V_ALWAYS);

    // Set color mask
    tmpVal = (NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE_TRUE << 24) |
             (NV097_SET_COLOR_MASK_RED_WRITE_ENABLE_TRUE << 16) |
             (NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE_TRUE << 8) |
              NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE_TRUE;

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COLOR_MASK | 0x40000);
    nvPushData(3, tmpVal);

    // Set depth mask
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_DEPTH_MASK | 0x40000);
    nvPushData(5, NV097_SET_DEPTH_MASK_V_FALSE);

    nvPusherAdjust(6);

    // Initialize stencil state (test already disabled above)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_STENCIL_MASK | 0x40000);
    nvPushData(1, 0x000000FF);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_STENCIL_FUNC | 0x40000);
    nvPushData(3, NV097_SET_STENCIL_FUNC_V_ALWAYS);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_STENCIL_FUNC_REF | 0x40000);
    nvPushData(5, 0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_STENCIL_FUNC_MASK | 0x40000);
    nvPushData(7, 0x000000FF);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_STENCIL_OP_FAIL | 0x40000);
    nvPushData(9, NV097_SET_STENCIL_OP_FAIL_V_KEEP);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_STENCIL_OP_ZFAIL | 0x40000);
    nvPushData(11, NV097_SET_STENCIL_OP_ZFAIL_V_KEEP);

    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_STENCIL_OP_ZPASS | 0x40000);
    nvPushData(13, NV097_SET_STENCIL_OP_ZPASS_V_KEEP);

    nvPusherAdjust(14);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SHADE_MODE | 0x40000);
    nvPushData(1, NV097_SET_SHADE_MODE_V_FLAT);

    fBuffer[0] = 0.0;

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_POLYGON_OFFSET_SCALE_FACTOR | 0x40000);
    nvPushData(3, ftol[0]);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_POLYGON_OFFSET_BIAS | 0x40000);
    nvPushData(5, ftol[0]);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FRONT_POLYGON_MODE | 0x40000);
    nvPushData(7, NV097_SET_FRONT_POLYGON_MODE_V_FILL);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BACK_POLYGON_MODE | 0x40000);
    nvPushData(9, NV097_SET_BACK_POLYGON_MODE_V_FILL);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_CULL_FACE | 0x40000);
    nvPushData(11, NV097_SET_CULL_FACE_V_BACK);

    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_FRONT_FACE | 0x40000);
    nvPushData(13, NV097_SET_FRONT_FACE_V_CCW);

    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_NORMALIZATION_ENABLE | 0x40000);
    nvPushData(15, NV097_SET_NORMALIZATION_ENABLE_V_FALSE);

    nvPushData(16, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_EDGE_FLAG | 0x40000);
    nvPushData(17, NV097_SET_EDGE_FLAG_V_TRUE);

    nvPusherAdjust(18);

    // Set material emission
    fBuffer[0] = 0.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_MATERIAL_EMISSION(0) | 0xC0000);
    nvPushData(1, ftol[0]);
    nvPushData(2, ftol[0]);
    nvPushData(3, ftol[0]);

    // Disable specular
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SPECULAR_ENABLE | 0x40000);
    nvPushData(5, NV097_SET_SPECULAR_ENABLE_V_FALSE);

    // Disable all lights
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_LIGHT_ENABLE_MASK | 0x40000);
    nvPushData(7, 0);

    nvPusherAdjust(8);

    // Disable texgen modes for now (OpenGL default is EYE_LINEAR)

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXGEN_S(0) | 0x40000);
    nvPushData(1, NV097_SET_TEXGEN_S_V_DISABLE);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXGEN_T(0) | 0x40000);
    nvPushData(3, NV097_SET_TEXGEN_T_V_DISABLE);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXGEN_R(0) | 0x40000);
    nvPushData(5, NV097_SET_TEXGEN_R_V_DISABLE);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXGEN_Q(0) | 0x40000);
    nvPushData(7, NV097_SET_TEXGEN_Q_V_DISABLE);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXGEN_S(1) | 0x40000);
    nvPushData(9, NV097_SET_TEXGEN_S_V_DISABLE);

    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXGEN_T(1) | 0x40000);
    nvPushData(11, NV097_SET_TEXGEN_T_V_DISABLE);

    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXGEN_R(1) | 0x40000);
    nvPushData(13, NV097_SET_TEXGEN_R_V_DISABLE);

    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXGEN_Q(1) | 0x40000);
    nvPushData(15, NV097_SET_TEXGEN_Q_V_DISABLE);

    nvPusherAdjust(16);

    // We don't use texgen so we skip setting texgen planes

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SWATH_WIDTH | 0x40000);
    nvPushData(1, NV097_SET_SWATH_WIDTH_V_OFF);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SHADER_STAGE_PROGRAM | 0x40000);
    nvPushData(3, ((NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PROGRAM_NONE << 15) |
                   (NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PROGRAM_NONE << 10) |
                   (NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_2D_PROJECTIVE << 5) |
                    NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_2D_PROJECTIVE));

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SHADER_CLIP_PLANE_MODE | 0x40000);
    nvPushData(5, 0);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SHADER_OTHER_STAGE_INPUT | 0x40000);
    nvPushData(7, 0);

    // This is set once and forgotten. It puts the class in OGL flat shade mode.
    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FLAT_SHADE_OP | 0x40000);
    nvPushData(9, NV097_SET_FLAT_SHADE_OP_V_LAST_VTX);

    nvPusherAdjust(10);

    fBuffer[0] = 0.0;
    fBuffer[1] = 1.0;

    // Set default fog parameters
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_PARAMS(0) | 0xC0000);
    nvPushData(1, ftol[1]);                     // K0
    nvPushData(2, ftol[1]);                     // K1
    nvPushData(3, ftol[0]);                     // K2

    // Set fog plane
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_PLANE(0) | 0x100000);
    nvPushData(5, ftol[0]);
    nvPushData(6, ftol[0]);
    nvPushData(7, ftol[1]);
    nvPushData(8, ftol[0]);

    nvPusherAdjust(9);

    // Initialize some vertex attributes
    fBuffer[0] = 0.0;
    fBuffer[1] = 1.0;

    // Set color for MODULATE blend mode
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_DIFFUSE_COLOR4F(0) | 0x100000);
    nvPushData(1, ftol[1]);
    nvPushData(2, ftol[1]);
    nvPushData(3, ftol[1]);
    nvPushData(4, ftol[1]);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SPECULAR_COLOR3F(0) | 0xC0000);
    nvPushData(6, ftol[0]);
    nvPushData(7, ftol[0]);
    nvPushData(8, ftol[0]);

    nvPushData(9, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXCOORD0_4F(0) | 0x100000);
    nvPushData(10, ftol[0]);
    nvPushData(11, ftol[0]);
    nvPushData(12, ftol[0]);
    nvPushData(13, ftol[1]);

    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXCOORD1_4F(0) | 0x100000);
    nvPushData(15, ftol[0]);
    nvPushData(16, ftol[0]);
    nvPushData(17, ftol[0]);
    nvPushData(18, ftol[1]);

    nvPushData(19, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_NORMAL3F(0) | 0xC0000);
    nvPushData(20, ftol[0]);
    nvPushData(21, ftol[0]);
    nvPushData(22, ftol[1]);

    nvPushData(23, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_FOG1F | 0x40000);
    nvPushData(24, ftol[0]);

    nvPushData(25, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_WEIGHT1F | 0x40000);
    nvPushData(26, ftol[1]);

    nvPusherAdjust(27);

    // Load texture matrices assuming quarter pel scaling of texture coordinates
    if (pDriverData->bMCHorizontallyDownscale1080i)
        fBuffer[0]  = 0.1875;
    else
        fBuffer[0]  = 0.25;

                        fBuffer[1]  = 0.0;  fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
    fBuffer[4]  = 0.0;  fBuffer[5]  = 0.25; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
    fBuffer[8]  = 0.0;  fBuffer[9]  = 0.0;  fBuffer[10] = 1.0; fBuffer[11] = 0.0;
    fBuffer[12] = 0.0;  fBuffer[13] = 0.0;  fBuffer[14] = 0.0; fBuffer[15] = 1.0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_MATRIX0(0) | 0x400000);
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

    nvPushData(17, dDrawSubchannelOffset(NV_DD_KELVIN) +
        NV097_SET_TEXTURE_MATRIX1(0) | 0x400000);
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

    // Disable logic ops
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_LOGIC_OP_ENABLE | 0x40000);
    nvPushData(1, NV097_SET_LOGIC_OP_ENABLE_V_FALSE);

    // Disable Z min/max culling
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_ZMIN_MAX_CONTROL | 0x40000);
    nvPushData(3, ((NV097_SET_ZMIN_MAX_CONTROL_CULL_IGNORE_W_TRUE << 8) |
                   (NV097_SET_ZMIN_MAX_CONTROL_ZCLAMP_EN_CLAMP << 4) |
                    NV097_SET_ZMIN_MAX_CONTROL_CULL_NEAR_FAR_EN_FALSE));

    // MUST set antialiasing control sample mask even though antialiasing is disabled
    // Disable antialiasing
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_ANTI_ALIASING_CONTROL | 0x40000);
    nvPushData(5, ((0xFFFF0000) |
                    NV097_SET_ANTI_ALIASING_CONTROL_ENABLE_FALSE));

    // Disable z buffer compression
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMPRESS_ZBUFFER_EN | 0x40000);
    nvPushData(7, NV097_SET_COMPRESS_ZBUFFER_EN_V_DISABLE);

    // Disable z stencil occlusion
    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_OCCLUDE_ZSTENCIL_EN | 0x40000);
    nvPushData(9, NV097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_ZEN_DISABLE);

    // Disable two sided lighting
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TWO_SIDE_LIGHT_EN | 0x40000);
    nvPushData(11, NV097_SET_TWO_SIDE_LIGHT_EN_V_FALSE);

    // Disable z pass pixel counting
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_ZPASS_PIXEL_COUNT_ENABLE | 0x40000);
    nvPushData(13, NV097_SET_ZPASS_PIXEL_COUNT_ENABLE_V_FALSE);

    nvPusherAdjust(14);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitKelvinForMoComp */



/*
 * nvInitKelvinForMoCompPrediction
 *
 * Performs final kelvin object motion comp initialization for
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
DWORD __stdcall nvInitKelvinForMoCompPrediction(void)
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

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
    nvPushData(3, dwSurfaceOffset);

    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCForwardSurface;
    pSurf_gbl = pSurf_lcl->lpGbl;

    dwSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
    nvPushData(5, dwSurfaceOffset);

    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface;
    pSurf_gbl = pSurf_lcl->lpGbl;

    dwSurfaceOffset = VIDMEM_OFFSET(pSurf_gbl->fpVidMem);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(7, dwSurfaceOffset);

    nvPusherAdjust(8);

    // Align surface pitch and set surface format and pitch
    surfacePitch |= (surfacePitch << 16);
    surfacePitch <<= 1; // Surface is always referenced by hardware in field mode

    tmpVal = (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) |
             (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
             (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) |
              NV097_SET_SURFACE_FORMAT_COLOR_LE_B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(1, tmpVal);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_PITCH | 0x40000);
    nvPushData(3, surfacePitch);

    nvPusherAdjust(4);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_MATRIX_ENABLE(0) | 0x100000);
    nvPushData(1, NV097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE);
    nvPushData(2, NV097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE);
    nvPushData(3, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);
    nvPushData(4, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);

    nvPusherAdjust(5);

    nvMoCompParams.dataFormat = DATA_FORMAT_LUMA;

    tmpVal = (1 << 16) | // 1 MIPMAP level
             (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8 << 8) |
             (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) |
             (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) |
             (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(0) | 0x40000);
    nvPushData(1, tmpVal);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(1) | 0x40000);
    nvPushData(3, tmpVal);
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(2) | 0x40000);
    nvPushData(5, tmpVal);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(3) | 0x40000);
    nvPushData(7, tmpVal);

    tmpVal = (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_Q_FALSE << 24) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_P_FALSE << 20) |
             (NV097_SET_TEXTURE_ADDRESS_P_CLAMP_TO_EDGE << 16) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_V_FALSE << 12) |
             (NV097_SET_TEXTURE_ADDRESS_V_CLAMP_TO_EDGE << 8) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_U_FALSE << 4) |
              NV097_SET_TEXTURE_ADDRESS_U_CLAMP_TO_EDGE;

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_ADDRESS(0) | 0x40000);
    nvPushData(9, tmpVal);
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(1) | 0x40000);
    nvPushData(11, tmpVal);
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(2) | 0x40000);
    nvPushData(13, tmpVal);
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(3) | 0x40000);
    nvPushData(15, tmpVal);

    nvPusherAdjust(16);

    pDriverData->dwMCTex0Control0 = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 18) | // MIN_LOD_CLAMP == 0.9
                                    (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
                                    (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 18) | // MIN_LOD_CLAMP == 0.9
                                    (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
                                    (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    tmpVal = (NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE << 30) |
             (15 << 18) | // MIN_LOD_CLAMP == 0.9
             (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
             (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
             (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
             (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(0) | 0x40000);
    nvPushData(1, pDriverData->dwMCTex0Control0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(1) | 0x40000);
    nvPushData(3, pDriverData->dwMCTex1Control0);
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(2) | 0x40000);
    nvPushData(5, tmpVal);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(3) | 0x40000);
    nvPushData(7, tmpVal);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
    nvPushData(9, surfacePitch << 16);
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(11, surfacePitch << 16);
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(2) | 0x40000);
    nvPushData(13, surfacePitch << 16);
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(3) | 0x40000);
    nvPushData(15, surfacePitch << 16);

    tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
             (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
             (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
             (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
             (NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0 << 24) |
             (NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 << 16) |
             (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

    nvPushData(16, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(0) | 0x40000);
    nvPushData(17, tmpVal);
    nvPushData(18, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(1) | 0x40000);
    nvPushData(19, tmpVal);
    nvPushData(20, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(2) | 0x40000);
    nvPushData(21, tmpVal);
    nvPushData(22, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(3) | 0x40000);
    nvPushData(23, tmpVal);

    tmpVal = (4094 << 16) | 2046; // Must be even and <= 4094

    nvPushData(24, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(0) | 0x40000);
    nvPushData(25, tmpVal);
    nvPushData(26, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(1) | 0x40000);
    nvPushData(27, tmpVal);
    nvPushData(28, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(2) | 0x40000);
    nvPushData(29, tmpVal);
    nvPushData(30, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(3) | 0x40000);
    nvPushData(31, tmpVal);

    nvPusherAdjust(32);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TRANSFORM_EXECUTION_MODE | 0x40000);
    nvPushData(1, ((NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV << 2) |
                    NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED));

    nvPusherAdjust(2);


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

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_COMPOSITE_MATRIX(0) | 0x400000);
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

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_MATRIX0(0) | 0x400000);
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

        nvPushData(17, dDrawSubchannelOffset(NV_DD_KELVIN) +
            NV097_SET_TEXTURE_MATRIX1(0) | 0x400000);
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
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV097_SET_FOG_ENABLE_V_TRUE);

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

    moCompICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_INVERT << 29) |
                (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_0 << 24) |

                (NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT << 13) |
                (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |

                (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                 NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_ALPHA_ICW(0) | 0x100000);
    nvPushData(1, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(2, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(3, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(4, DEFAULT_KELVIN_ALPHA_ICW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_COLOR_ICW(0) | 0x100000);
    nvPushData(6, moCompICW);
    nvPushData(7, DEFAULT_KELVIN_COLOR_ICW);
    nvPushData(8, DEFAULT_KELVIN_COLOR_ICW);
    nvPushData(9, DEFAULT_KELVIN_COLOR_ICW);

    // Load combination factors
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    nvPushData(11, 0xFFFFFFFF); // 100%

    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    nvPushData(13, 0xFFFFFFFF); // 100%

    nvPusherAdjust(14);

    // combiner0, RGB portion:   spare0 = texture0, spare1 = texture1
    moCompOCW = (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_DISABLE << 19) |
                (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE << 18) |
                (NV097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT << 15) |
                (NV097_SET_COMBINER_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                (NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                (NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                (NV097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_0 << 8) |
                (NV097_SET_COMBINER_COLOR_OCW_AB_DST_REG_C << 4) |
                 NV097_SET_COMBINER_COLOR_OCW_CD_DST_REG_D;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_ALPHA_OCW(0) | 0x100000);
    nvPushData(1, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(2, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(3, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(4, DEFAULT_KELVIN_ALPHA_OCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_COLOR_OCW(0) | 0x100000);
    nvPushData(6, moCompOCW);
    nvPushData(7, DEFAULT_KELVIN_COLOR_OCW);
    nvPushData(8, DEFAULT_KELVIN_COLOR_OCW);
    nvPushData(9, DEFAULT_KELVIN_COLOR_OCW);

    tmpVal = (NV097_SET_COMBINER_CONTROL_FACTOR1_SAME_FACTOR_ALL << 16) |
             (NV097_SET_COMBINER_CONTROL_FACTOR0_SAME_FACTOR_ALL << 12) |
             (NV097_SET_COMBINER_CONTROL_MUX_SELECT_MSB << 8) |
              NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_ONE;

    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_CONTROL | 0x40000);
    nvPushData(11, tmpVal);

    nvPusherAdjust(12);

    // Reconfigure fog unit to interpolate
    // fog factor in alpha
    // fog = 0.0: all texture0
    // fog = 0.5: equal parts of texture0 and texture1
    // fog = 1.0: all texture1

    moCompFinalCW0 = (NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE_FALSE << 29) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA_TRUE << 28) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_3 << 24) |   // fog

                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE_FALSE << 21) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA_FALSE << 20) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_D << 16) |   // spare1

                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE_FALSE << 13) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA_FALSE << 12) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_C << 8) |    // spare0

                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE_FALSE << 5) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA_FALSE << 4) |
                      NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_0;           // 0

    moCompFinalCW1 = (NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE_FALSE << 29) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA_FALSE << 28) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_0 << 24) |   // 0

                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE_FALSE << 21) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA_FALSE << 20) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_0 << 16) |   // 0

                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE_TRUE << 13) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA_TRUE << 12) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_0 << 8) |    // 0xFF

                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP_TRUE << 7) |
                     (NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5_FALSE << 6) |
                      NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12_FALSE;

    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, moCompFinalCW0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, moCompFinalCW1);

    nvPusherAdjust(4);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitKelvinForMoCompPrediction */


/*
 * nvInitKelvinForMoCompCorrection
 *
 * Performs final kelvin object motion comp initialization for
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
DWORD __stdcall nvInitKelvinForMoCompCorrection(void)
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

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
    nvPushData(3, dwSurfaceOffset);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
    nvPushData(5, 0);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(7, dwSurfaceOffset);

    nvPusherAdjust(8);

    // Align surface pitch and set surface format and pitch
    surfacePitch |= (surfacePitch << 16);

    if (pDriverData->bMCPictureStructure != PICTURE_STRUCTURE_FRAME)
        surfacePitch <<= 1; // Field surface is always referenced by hardware using double pitch

    tmpVal = (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) |
             (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
             (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) |
              NV097_SET_SURFACE_FORMAT_COLOR_LE_B8;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(1, tmpVal);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_PITCH | 0x40000);
    nvPushData(3, surfacePitch);

    nvPusherAdjust(4);

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_MATRIX_ENABLE(0) | 0x100000);
    nvPushData(1, NV097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE);
    nvPushData(2, NV097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE);
    nvPushData(3, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);
    nvPushData(4, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);

    nvPusherAdjust(5);

    nvMoCompParams.dataFormat = DATA_FORMAT_CORRECTION;

    // Signed Y8
    textureFormat0 = (1 << 16) | // 1 MIPMAP level
                     (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8 << 8) |
                     (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) |
                     (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) |
                     (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
                      NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B;

    // Unsigned Y8
    textureFormat1 = (1 << 16) | // 1 MIPMAP level
                     (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8 << 8) |
                     (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) |
                     (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) |
                     (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
                      NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(0) | 0x40000);
    nvPushData(1, textureFormat0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(1) | 0x40000);
    nvPushData(3, textureFormat1);
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(2) | 0x40000);
    nvPushData(5, textureFormat1);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(3) | 0x40000);
    nvPushData(7, textureFormat1);

    tmpVal = (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_Q_FALSE << 24) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_P_FALSE << 20) |
             (NV097_SET_TEXTURE_ADDRESS_P_CLAMP_TO_EDGE << 16) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_V_FALSE << 12) |
             (NV097_SET_TEXTURE_ADDRESS_V_CLAMP_TO_EDGE << 8) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_U_FALSE << 4) |
              NV097_SET_TEXTURE_ADDRESS_U_CLAMP_TO_EDGE;

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_ADDRESS(0) | 0x40000);
    nvPushData(9, tmpVal);
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(1) | 0x40000);
    nvPushData(11, tmpVal);
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(2) | 0x40000);
    nvPushData(13, tmpVal);
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(3) | 0x40000);
    nvPushData(15, tmpVal);

    nvPusherAdjust(16);

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

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_COMPOSITE_MATRIX(0) | 0x400000);
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

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_MATRIX0(0) | 0x400000);
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

        nvPushData(17, dDrawSubchannelOffset(NV_DD_KELVIN) +
            NV097_SET_TEXTURE_MATRIX1(0) | 0x400000);
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

    pDriverData->dwMCTex0Control0 = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 18) | // MIN_LOD_CLAMP == 0.9
                                    (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
                                    (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 18) | // MIN_LOD_CLAMP == 0.9
                                    (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
                                    (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    tmpVal = (NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE << 30) |
             (15 << 18) | // MIN_LOD_CLAMP == 0.9
             (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
             (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
             (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
             (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(0) | 0x40000);
    nvPushData(1, pDriverData->dwMCTex0Control0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(1) | 0x40000);
    nvPushData(3, pDriverData->dwMCTex1Control0);
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(2) | 0x40000);
    nvPushData(5, tmpVal);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(3) | 0x40000);
    nvPushData(7, tmpVal);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
    nvPushData(9, texturePitch0 << 16);
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(11, texturePitch1 << 16);
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(2) | 0x40000);
    nvPushData(13, texturePitch1 << 16);
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(3) | 0x40000);
    nvPushData(15, texturePitch1 << 16);

    tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
             (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
             (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
             (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
             (NV097_SET_TEXTURE_FILTER_MAG_BOX_LOD0 << 24) |
             (NV097_SET_TEXTURE_FILTER_MIN_BOX_LOD0 << 16) |
             (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

    nvPushData(16, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(0) | 0x40000);
    nvPushData(17, tmpVal);
    nvPushData(18, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(1) | 0x40000);
    nvPushData(19, tmpVal);
    nvPushData(20, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(2) | 0x40000);
    nvPushData(21, tmpVal);
    nvPushData(22, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(3) | 0x40000);
    nvPushData(23, tmpVal);

    tmpVal = (4094 << 16) | 2046; // Must be even and <= 4094

    nvPushData(24, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(0) | 0x40000);
    nvPushData(25, (16 << 16) | 2044); // Must be even and <= 2046
    nvPushData(26, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(1) | 0x40000);
    nvPushData(27, tmpVal);
    nvPushData(28, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(2) | 0x40000);
    nvPushData(29, tmpVal);
    nvPushData(30, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(3) | 0x40000);
    nvPushData(31, tmpVal);

    nvPusherAdjust(32);

    // Disable fog
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV097_SET_FOG_ENABLE_V_FALSE);

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

    moCompICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                (NV097_SET_COMBINER_COLOR_ICW_B_MAP_HALFBIAS_NORMAL << 21) |
                (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1 << 8) |

                (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                 NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_ALPHA_ICW(0) | 0x100000);
    nvPushData(1, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(2, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(3, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(4, DEFAULT_KELVIN_ALPHA_ICW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_COLOR_ICW(0) | 0x100000);
    nvPushData(6, moCompICW);
    nvPushData(7, DEFAULT_KELVIN_COLOR_ICW);
    nvPushData(8, DEFAULT_KELVIN_COLOR_ICW);
    nvPushData(9, DEFAULT_KELVIN_COLOR_ICW);

    // Load combination factors
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    nvPushData(11, 0xFFFFFFFF); // 100%

    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    nvPushData(13, 0x0); // 0%

    nvPusherAdjust(14);

    // combiner0, RGB portion:   texture0 + texture1
    // spare0 = A * B + C * D
    moCompOCW = (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_DISABLE << 19) |
                (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE << 18) |
                (NV097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT << 15) |
                (NV097_SET_COMBINER_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                (NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                (NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                (NV097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_C << 8) |
                (NV097_SET_COMBINER_COLOR_OCW_AB_DST_REG_0 << 4) |
                 NV097_SET_COMBINER_COLOR_OCW_CD_DST_REG_0;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_ALPHA_OCW(0) | 0x100000);
    nvPushData(1, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(2, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(3, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(4, DEFAULT_KELVIN_ALPHA_OCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_COLOR_OCW(0) | 0x100000);
    nvPushData(6, moCompOCW);
    nvPushData(7, DEFAULT_KELVIN_COLOR_OCW);
    nvPushData(8, DEFAULT_KELVIN_COLOR_OCW);
    nvPushData(9, DEFAULT_KELVIN_COLOR_OCW);

    tmpVal = (NV097_SET_COMBINER_CONTROL_FACTOR1_SAME_FACTOR_ALL << 16) |
             (NV097_SET_COMBINER_CONTROL_FACTOR0_SAME_FACTOR_ALL << 12) |
             (NV097_SET_COMBINER_CONTROL_MUX_SELECT_MSB << 8) |
              NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_ONE;

    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_CONTROL | 0x40000);
    nvPushData(11, tmpVal);

    nvPusherAdjust(12);

    // Load final stages with default values
    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, DEFAULT_KELVIN_FINAL_CW0);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, DEFAULT_KELVIN_FINAL_CW1);

    nvPusherAdjust(4);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitKelvinForMoCompCorrection */



/*
 * nvInitKelvinForMoCompConversion
 *
 * Performs final kelvin object motion comp initialization for
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
DWORD __stdcall nvInitKelvinForMoCompConversion(void)
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
        fBuffer[0]  = (float)0.1875; fBuffer[1]  = 0.0;  fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
        fBuffer[4]  = 0.0;           fBuffer[5]  = 0.25; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
        fBuffer[8]  = 0.0;           fBuffer[9]  = 0.0;  fBuffer[10] = 1.0; fBuffer[11] = 0.0;
        fBuffer[12] = 0.0;           fBuffer[13] = 0.0;  fBuffer[14] = 0.0; fBuffer[15] = 1.0;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_COMPOSITE_MATRIX(0) | 0x400000);
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

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    tmpVal = (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) |
             (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
             (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) |
              NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8;

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(3, tmpVal);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_PITCH | 0x40000);
    nvPushData(5, surfacePitch);

    nvPusherAdjust(6);

    // Just disable both texture matrices since these texture coordinates are unscaled
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_MATRIX_ENABLE(0) | 0x100000);
    nvPushData(1, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);
    nvPushData(2, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);
    nvPushData(3, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);
    nvPushData(4, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);

    nvPusherAdjust(5);

    nvMoCompParams.dataFormat = DATA_FORMAT_FOURCC_CONVERSION;

    tmpVal = (1 << 16) | // 1 MIPMAP level
             (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8 << 8) |
             (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) |
             (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) |
             (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(0) | 0x40000);
    nvPushData(1, tmpVal);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(1) | 0x40000);
    nvPushData(3, tmpVal);
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(2) | 0x40000);
    nvPushData(5, tmpVal);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(3) | 0x40000);
    nvPushData(7, tmpVal);

    tmpVal = (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_Q_FALSE << 24) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_P_FALSE << 20) |
             (NV097_SET_TEXTURE_ADDRESS_P_CLAMP_TO_EDGE << 16) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_V_FALSE << 12) |
             (NV097_SET_TEXTURE_ADDRESS_V_CLAMP_TO_EDGE << 8) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_U_FALSE << 4) |
              NV097_SET_TEXTURE_ADDRESS_U_CLAMP_TO_EDGE;

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_ADDRESS(0) | 0x40000);
    nvPushData(9, tmpVal);
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(1) | 0x40000);
    nvPushData(11, tmpVal);
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(2) | 0x40000);
    nvPushData(13, tmpVal);
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(3) | 0x40000);
    nvPushData(15, tmpVal);

    nvPusherAdjust(16);

    pDriverData->dwMCTex0Control0 = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 18) | // MIN_LOD_CLAMP == 0.9
                                    (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
                                    (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 18) | // MIN_LOD_CLAMP == 0.9
                                    (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
                                    (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    tmpVal = (NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE << 30) |
             (15 << 18) | // MIN_LOD_CLAMP == 0.9
             (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
             (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
             (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
             (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(0) | 0x40000);
    nvPushData(1, pDriverData->dwMCTex0Control0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(1) | 0x40000);
    nvPushData(3, pDriverData->dwMCTex1Control0);
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(2) | 0x40000);
    nvPushData(5, tmpVal);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(3) | 0x40000);
    nvPushData(7, tmpVal);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
    nvPushData(9, texturePitch << 16);
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(11, texturePitch << 16);
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(2) | 0x40000);
    nvPushData(13, texturePitch << 16);
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(3) | 0x40000);
    nvPushData(15, texturePitch << 16);

    tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
             (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
             (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
             (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
             (NV097_SET_TEXTURE_FILTER_MAG_BOX_LOD0 << 24) |
             (NV097_SET_TEXTURE_FILTER_MIN_BOX_LOD0 << 16) |
             (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

    nvPushData(16, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(0) | 0x40000);
    nvPushData(17, tmpVal);
    nvPushData(18, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(1) | 0x40000);
    nvPushData(19, tmpVal);
    nvPushData(20, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(2) | 0x40000);
    nvPushData(21, tmpVal);
    nvPushData(22, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(3) | 0x40000);
    nvPushData(23, tmpVal);

    tmpVal = (4094 << 16) | 2046; // Must be even and <= 4094

    nvPushData(24, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(0) | 0x40000);
    nvPushData(25, tmpVal);
    nvPushData(26, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(1) | 0x40000);
    nvPushData(27, tmpVal);
    nvPushData(28, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(2) | 0x40000);
    nvPushData(29, tmpVal);
    nvPushData(30, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(3) | 0x40000);
    nvPushData(31, tmpVal);

    nvPusherAdjust(32);

    // Disable fog
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV097_SET_FOG_ENABLE_V_FALSE);

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

    moCompAlphaICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_TRUE << 28) |
                     (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_0 << 24) |

                     (NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_TRUE << 20) |
                     (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_0 << 16) |

                     (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_TRUE << 12) |
                     (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_2 << 8) |

                     (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_TRUE << 4) |
                      NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    moCompColorICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                     (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                     (NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                     (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                     (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                     (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_2 << 8) |

                     (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                      NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_ALPHA_ICW(0) | 0x100000);
    nvPushData(1, moCompAlphaICW);
    nvPushData(2, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(3, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(4, DEFAULT_KELVIN_ALPHA_ICW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_COLOR_ICW(0) | 0x100000);
    nvPushData(6, moCompColorICW);
    nvPushData(7, DEFAULT_KELVIN_COLOR_ICW);
    nvPushData(8, DEFAULT_KELVIN_COLOR_ICW);
    nvPushData(9, DEFAULT_KELVIN_COLOR_ICW);

    // Load combination factors
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    nvPushData(11, 0x00FF00FF); // luma

    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    nvPushData(13, 0xFF00FF00); // chroma

    nvPusherAdjust(14);

    // combiner0, Alpha portion:    spare0 = A * B + C * D = texture1
    moCompAlphaOCW = (NV097_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT << 15) |
                     (NV097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C << 8) |
                     (NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0 << 4) |
                      NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0;

    // combiner0, RGB portion:      spare0 = A * B + C * D = texture0 + texture1
    moCompColorOCW = (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_DISABLE << 19) |
                     (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE << 18) |
                     (NV097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT << 15) |
                     (NV097_SET_COMBINER_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                     (NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                     (NV097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_C << 8) |
                     (NV097_SET_COMBINER_COLOR_OCW_AB_DST_REG_0 << 4) |
                      NV097_SET_COMBINER_COLOR_OCW_CD_DST_REG_0;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_ALPHA_OCW(0) | 0x100000);
    nvPushData(1, moCompAlphaOCW);
    nvPushData(2, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(3, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(4, DEFAULT_KELVIN_ALPHA_OCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_COLOR_OCW(0) | 0x100000);
    nvPushData(6, moCompColorOCW);
    nvPushData(7, DEFAULT_KELVIN_COLOR_OCW);
    nvPushData(8, DEFAULT_KELVIN_COLOR_OCW);
    nvPushData(9, DEFAULT_KELVIN_COLOR_OCW);

    tmpVal = (NV097_SET_COMBINER_CONTROL_FACTOR1_SAME_FACTOR_ALL << 16) |
             (NV097_SET_COMBINER_CONTROL_FACTOR0_SAME_FACTOR_ALL << 12) |
             (NV097_SET_COMBINER_CONTROL_MUX_SELECT_MSB << 8) |
              NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_ONE;

    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_CONTROL | 0x40000);
    nvPushData(11, tmpVal);

    nvPusherAdjust(12);

    // Load final stages with default values
    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, DEFAULT_KELVIN_FINAL_CW0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, DEFAULT_KELVIN_FINAL_CW1);

    nvPusherAdjust(4);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitKelvinForMoCompConversion */


/*
 * nvInitKelvinForMoCompTemporalFilter
 *
 * Performs final kelvin object motion comp initialization for
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
DWORD __stdcall nvInitKelvinForMoCompTemporalFilter(void)
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

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_NO_OPERATION | 0x40000);
    nvPushData(1, 0);

    tmpVal = (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) |
             (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
             (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) |
              NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8;

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_FORMAT | 0x40000);
    nvPushData(3, tmpVal);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_SURFACE_PITCH | 0x40000);
    nvPushData(5, surfacePitch);

    nvPusherAdjust(6);

    // Enable both texture matrices since these texture coordinates are in quarter pel
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_MATRIX_ENABLE(0) | 0x100000);
    nvPushData(1, NV097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE);
    nvPushData(2, NV097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE);
    nvPushData(3, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);
    nvPushData(4, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);

    nvPusherAdjust(5);

    nvMoCompParams.dataFormat = DATA_FORMAT_TEMPORAL_FILTER;

    tmpVal = (1 << 16) | // 1 MIPMAP level
             (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8 << 8) |
             (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) |
             (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) |
             (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(0) | 0x40000);
    nvPushData(1, tmpVal);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(1) | 0x40000);
    nvPushData(3, tmpVal);
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(2) | 0x40000);
    nvPushData(5, tmpVal);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_FORMAT(3) | 0x40000);
    nvPushData(7, tmpVal);

    tmpVal = (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_Q_FALSE << 24) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_P_FALSE << 20) |
             (NV097_SET_TEXTURE_ADDRESS_P_CLAMP_TO_EDGE << 16) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_V_FALSE << 12) |
             (NV097_SET_TEXTURE_ADDRESS_V_CLAMP_TO_EDGE << 8) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_U_FALSE << 4) |
              NV097_SET_TEXTURE_ADDRESS_U_CLAMP_TO_EDGE;

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_ADDRESS(0) | 0x40000);
    nvPushData(9, tmpVal);
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(1) | 0x40000);
    nvPushData(11, tmpVal);
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(2) | 0x40000);
    nvPushData(13, tmpVal);
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_ADDRESS(3) | 0x40000);
    nvPushData(15, tmpVal);

    nvPusherAdjust(16);

    pDriverData->dwMCTex0Control0 = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 18) | // MIN_LOD_CLAMP == 0.9
                                    (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
                                    (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    pDriverData->dwMCTex1Control0 = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                                    (15 << 18) | // MIN_LOD_CLAMP == 0.9
                                    (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
                                    (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                                    (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                                    (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                                     NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    tmpVal = (NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE << 30) |
             (15 << 18) | // MIN_LOD_CLAMP == 0.9
             (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
             (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
             (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
             (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(0) | 0x40000);
    nvPushData(1, pDriverData->dwMCTex0Control0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(1) | 0x40000);
    nvPushData(3, pDriverData->dwMCTex1Control0);
    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(2) | 0x40000);
    nvPushData(5, tmpVal);
    nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_TEXTURE_CONTROL0(3) | 0x40000);
    nvPushData(7, tmpVal);

    nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
    nvPushData(9, surfacePitch << 16);
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(11, surfacePitch << 16);
    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(2) | 0x40000);
    nvPushData(13, surfacePitch << 16);
    nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_CONTROL1(3) | 0x40000);
    nvPushData(15, surfacePitch << 16);

    tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
             (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
             (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
             (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
             (NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0 << 24) |
             (NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 << 16) |
             (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

    nvPushData(16, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(0) | 0x40000);
    nvPushData(17, tmpVal);
    nvPushData(18, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(1) | 0x40000);
    nvPushData(19, tmpVal);
    nvPushData(20, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(2) | 0x40000);
    nvPushData(21, tmpVal);
    nvPushData(22, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_FILTER(3) | 0x40000);
    nvPushData(23, tmpVal);

    tmpVal = (4094 << 16) | 2046; // Must be even and <= 4094

    nvPushData(24, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(0) | 0x40000);
    nvPushData(25, tmpVal);
    nvPushData(26, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(1) | 0x40000);
    nvPushData(27, tmpVal);
    nvPushData(28, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(2) | 0x40000);
    nvPushData(29, tmpVal);
    nvPushData(30, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_TEXTURE_IMAGE_RECT(3) | 0x40000);
    nvPushData(31, tmpVal);

    nvPusherAdjust(32);

    // Disable fog
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG_ENABLE | 0x40000);
    nvPushData(1, NV097_SET_FOG_ENABLE_V_FALSE);

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

    moCompAlphaICW = (NV097_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV097_SET_COMBINER_ALPHA_ICW_A_ALPHA_TRUE << 28) |
                     (NV097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1 << 24) |

                     (NV097_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV097_SET_COMBINER_ALPHA_ICW_B_ALPHA_TRUE << 20) |
                     (NV097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_8 << 16) |

                     (NV097_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV097_SET_COMBINER_ALPHA_ICW_C_ALPHA_TRUE << 12) |
                     (NV097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_2 << 8) |

                     (NV097_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV097_SET_COMBINER_ALPHA_ICW_D_ALPHA_TRUE << 4) |
                      NV097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_9;

    moCompColorICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                     (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                     (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                     (NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                     (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                     (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                     (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                     (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                     (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_2 << 8) |

                     (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                     (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                      NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Initialize combiner ICW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_ALPHA_ICW(0) | 0x100000);
    nvPushData(1, moCompAlphaICW);
    nvPushData(2, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(3, DEFAULT_KELVIN_ALPHA_ICW);
    nvPushData(4, DEFAULT_KELVIN_ALPHA_ICW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_COLOR_ICW(0) | 0x100000);
    nvPushData(6, moCompColorICW);
    nvPushData(7, DEFAULT_KELVIN_COLOR_ICW);
    nvPushData(8, DEFAULT_KELVIN_COLOR_ICW);
    nvPushData(9, DEFAULT_KELVIN_COLOR_ICW);

    // Load combination factors
    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    nvPushData(11, 0x80808080); // % of top field

    nvPushData(12, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    nvPushData(13, 0x80808080); // % of bottom field

    nvPusherAdjust(14);

    // combiner0, RGB portion:      spare0 = A * B + C * D = texture0 + texture1
    moCompAlphaOCW = (NV097_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT << 15) |
                     (NV097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C << 8) |
                     (NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0 << 4) |
                      NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0;

    moCompColorOCW = (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_DISABLE << 19) |
                     (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE << 18) |
                     (NV097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT << 15) |
                     (NV097_SET_COMBINER_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                     (NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                     (NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                     (NV097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_C << 8) |
                     (NV097_SET_COMBINER_COLOR_OCW_AB_DST_REG_0 << 4) |
                      NV097_SET_COMBINER_COLOR_OCW_CD_DST_REG_0;

    // Initialize combiner OCW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_ALPHA_OCW(0) | 0x100000);
    nvPushData(1, moCompAlphaOCW);
    nvPushData(2, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(3, DEFAULT_KELVIN_ALPHA_OCW);
    nvPushData(4, DEFAULT_KELVIN_ALPHA_OCW);

    nvPushData(5, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_COLOR_OCW(0) | 0x100000);
    nvPushData(6, moCompColorOCW);
    nvPushData(7, DEFAULT_KELVIN_COLOR_OCW);
    nvPushData(8, DEFAULT_KELVIN_COLOR_OCW);
    nvPushData(9, DEFAULT_KELVIN_COLOR_OCW);

    tmpVal = (NV097_SET_COMBINER_CONTROL_FACTOR1_SAME_FACTOR_ALL << 16) |
             (NV097_SET_COMBINER_CONTROL_FACTOR0_SAME_FACTOR_ALL << 12) |
             (NV097_SET_COMBINER_CONTROL_MUX_SELECT_MSB << 8) |
              NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_ONE;

    nvPushData(10, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_CONTROL | 0x40000);
    nvPushData(11, tmpVal);

    nvPusherAdjust(12);

    // Load final stages with default values
    // Initialize combiner final CW methods
    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    nvPushData(1, DEFAULT_KELVIN_FINAL_CW0);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    nvPushData(3, DEFAULT_KELVIN_FINAL_CW1);

    nvPusherAdjust(4);

    nvPusherStart(TRUE);

    return TRUE;

} /* nvInitKelvinForMoCompTemporalFilter */



#endif  // NVARCH >= 0x10

