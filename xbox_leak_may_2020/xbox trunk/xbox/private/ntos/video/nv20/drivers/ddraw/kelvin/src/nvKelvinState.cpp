// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvKelvinState.cpp
//      Kelvin state management routines.
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        12Mar00         NV20 development
//
// **************************************************************************

#include "nvprecomp.h"

#if (NVARCH >= 0x20)

// all elements of this matrix remain zero except the last row and the diagonal
// which get set appropriately in nvSetKelvinTransform
static D3DMATRIX mViewportMatrix = matrixZero;

#define PRECOMPILED_PASSTHROUGH
#ifdef PRECOMPILED_PASSTHROUGH

static BYTE kmpPassthruProgramSpecFog[] = {
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0x10, 0x1F,
    0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x01, 0x00, 0x02, 0x18, 0xF8, 0x71, 0x90, 0xF1, 0x2F, 0xD8, 0x20,
    0xB0, 0x61, 0x00, 0x42, 0x20, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x81, 0x00, 0x02,
    0x28, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0x20, 0xF0, 0x8F, 0x00, 0x02, 0xF8, 0x0F, 0x10, 0x1F,
    0xB0, 0x61, 0xD8, 0x10, 0xB1, 0x01, 0x74, 0x04, 0x30, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0x20,
    0x00, 0x20, 0x00, 0x02, 0x00, 0xF8, 0x70, 0x30, 0xB0, 0x21, 0xD8, 0x90, 0xB1, 0x01, 0x76, 0x06,
    0x48, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0xA0, 0xB0, 0x21, 0x01, 0x02, 0x50, 0xF8, 0x70, 0x90,
    0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x41, 0x01, 0x02, 0x58, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0xA0,
    0xB0, 0x61, 0x01, 0x02, 0x61, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0xA0, 0xB0, 0x81, 0x01, 0x02,
};

static BYTE kmpPassthruProgramZFog[] = {
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0x10, 0x1F,
    0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x01, 0x00, 0x02, 0x18, 0xF8, 0x71, 0x90, 0xF1, 0x2F, 0xD8, 0x20,
    0xB0, 0x61, 0x00, 0x42, 0x20, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x81, 0x00, 0x02,
    0x28, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0x10, 0xA1, 0x0A, 0x00, 0x02, 0xF8, 0x0F, 0x10, 0x1F,
    0xB0, 0x61, 0xD8, 0x10, 0xB1, 0x01, 0x74, 0x04, 0x30, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0x20,
    0x00, 0x20, 0x00, 0x02, 0x00, 0xF8, 0x70, 0x30, 0xB0, 0x21, 0xD8, 0x90, 0xB1, 0x01, 0x76, 0x06,
    0x48, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0xA0, 0xB0, 0x21, 0x01, 0x02, 0x50, 0xF8, 0x70, 0x90,
    0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x41, 0x01, 0x02, 0x58, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0xA0,
    0xB0, 0x61, 0x01, 0x02, 0x61, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0xA0, 0xB0, 0x81, 0x01, 0x02,
};

static BYTE kmpPassthruProgramWFog[] = {
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0x10, 0x1F,
    0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x01, 0x00, 0x02, 0x18, 0xF8, 0x71, 0x90, 0xF1, 0x2F, 0xD8, 0x20,
    0xB0, 0x61, 0x00, 0x42, 0x20, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x81, 0x00, 0x02,
    0x28, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0x10, 0xF1, 0x0F, 0x00, 0x02, 0xF8, 0x0F, 0x10, 0x1F,
    0xB0, 0x61, 0xD8, 0x10, 0xB1, 0x01, 0x74, 0x04, 0x30, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0x20,
    0x00, 0x20, 0x00, 0x02, 0x00, 0xF8, 0x70, 0x30, 0xB0, 0x21, 0xD8, 0x90, 0xB1, 0x01, 0x76, 0x06,
    0x48, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0xA0, 0xB0, 0x21, 0x01, 0x02, 0x50, 0xF8, 0x70, 0x90,
    0xB0, 0x21, 0xD8, 0x20, 0xB0, 0x41, 0x01, 0x02, 0x58, 0xF8, 0x70, 0x10, 0xB0, 0x21, 0xD8, 0xA0,
    0xB0, 0x61, 0x01, 0x02, 0x61, 0xF8, 0x70, 0x90, 0xB0, 0x21, 0xD8, 0xA0, 0xB0, 0x81, 0x01, 0x02,
};

#else  // !PRECOMPILED_PASSTHROUGH

static char szPassthruProgramSpecFog[] = \
   "%!VP1.0                              \
    MOV R1,v[0];                         \
    MOV o[COL0],v[3] : RCP R1.w,R1.w;    \
    MOV o[COL1],v[4];                    \
    MOV o[FOGC],v[4].w;                  \
    MUL R1,R1,c[58];                     \
    MOV o[PSIZ],v[1].x;                  \
    ADD o[HPOS],R1,c[59];                \
    MOV o[TEX0],v[9];                    \
    MOV o[TEX1],v[10];                   \
    MOV o[TEX2],v[11];                   \
    MOV o[TEX3],v[12];";

static char szPassthruProgramZFog[] =    \
   "%!VP1.0                              \
    MOV R1,v[0];                         \
    MOV o[COL0],v[3] : RCP R1.w,R1.w;    \
    MOV o[COL1],v[4];                    \
    MOV o[FOGC],R1.z;                    \
    MUL R1,R1,c[58];                     \
    MOV o[PSIZ],v[1].x;                  \
    ADD o[HPOS],R1,c[59];                \
    MOV o[TEX0],v[9];                    \
    MOV o[TEX1],v[10];                   \
    MOV o[TEX2],v[11];                   \
    MOV o[TEX3],v[12];";

static char szPassthruProgramWFog[] =    \
   "%!VP1.0                              \
    MOV R1,v[0];                         \
    MOV o[COL0],v[3] : RCP R1.w,R1.w;    \
    MOV o[COL1],v[4];                    \
    MOV o[FOGC],R1.w;                    \
    MUL R1,R1,c[58];                     \
    MOV o[PSIZ],v[1].x;                  \
    ADD o[HPOS],R1,c[59];                \
    MOV o[TEX0],v[9];                    \
    MOV o[TEX1],v[10];                   \
    MOV o[TEX2],v[11];                   \
    MOV o[TEX3],v[12];";

#endif  // !PRECOMPILED_PASSTHROUGH

//---------------------------------------------------------------------------

// set things that are never changed in D3D. this routine is run once to initialize
// and then only whenever someone else (DDRAW) stomps on our kelvin object

HRESULT nvSetKelvinD3DDefaults (PNVD3DCONTEXT pContext)
{
    // clear the dirty bit
    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_DEFAULTS;

    // context dmas
    pContext->hwState.kelvin.set3 (NV097_SET_CONTEXT_DMA_NOTIFIES,
                                   NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY,   // notifier context
                                   D3D_CONTEXT_DMA_HOST_MEMORY,                            // texture context a - system/agp memory
                                   NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);                 // texture context b - video memory

#ifdef KELVIN_SEMAPHORES
    pContext->hwState.kelvin.set7 (NV097_SET_CONTEXT_DMA_STATE,
                                   NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY,                  // state context
                                   NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,                  // frame buffer context
                                   NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,                  // zeta buffer context
                                   D3D_CONTEXT_DMA_HOST_MEMORY,                            // vertex buffer context a
                                   NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY,                // vertex buffer context b
                                   CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_IN_MEMORY,     // semaphore context
                                   NV01_NULL_OBJECT);                                      // report context
#else
    pContext->hwState.kelvin.set7 (NV097_SET_CONTEXT_DMA_STATE,
                                   NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY,                  // state context
                                   NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,                  // frame buffer context
                                   NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,                  // zeta buffer context
                                   D3D_CONTEXT_DMA_HOST_MEMORY,                            // vertex buffer context a
                                   NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY,                // vertex buffer context b
                                   NV01_NULL_OBJECT,                                       // semaphore context
                                   NV01_NULL_OBJECT);                                      // report context
#endif

    // use the first vertex to determine the color used for flat-shading
    pContext->hwState.kelvin.set1 (NV097_SET_FLAT_SHADE_OP, NV097_SET_FLAT_SHADE_OP_V_FIRST_VTX);

    // clipping is inclusive of boundaries
    pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_TYPE, NV097_SET_WINDOW_CLIP_TYPE_V_INCLUSIVE);

    // deactivate all but the first clip rectangle
    for (DWORD dwWindow=1; dwWindow < KELVIN_NUM_WINDOWS; dwWindow++) {
        pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_HORIZONTAL(dwWindow), 0);
        pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_VERTICAL(dwWindow), 0);
    }

    // no point params until DX8
    if (global.dwDXRuntimeVersion < 0x0800) {
        pContext->hwState.kelvin.set1 (NV097_SET_POINT_PARAMS_ENABLE, NV097_SET_POINT_PARAMS_ENABLE_V_FALSE);
    }

    // D3D has no smoothing (note smoothing != anti-aliasing!)
    pContext->hwState.kelvin.set3 (NV097_SET_POINT_SMOOTH_ENABLE,
                                   NV097_SET_POINT_SMOOTH_ENABLE_V_FALSE,
                                   NV097_SET_LINE_SMOOTH_ENABLE_V_FALSE,
                                   NV097_SET_POLY_SMOOTH_ENABLE_V_FALSE);

    // blend color not available in d3d
    pContext->hwState.kelvin.set1 (NV097_SET_BLEND_COLOR, 0);

    // line and point sizes are just one pixel (in DX7. this changes in DX8)
    pContext->hwState.kelvin.set1 (NV097_SET_LINE_WIDTH, 0x00000008);  // 6.3 format
    pContext->hwState.kelvin.set1 (NV097_SET_POINT_SIZE, 0x00000008);  // 6.3 format

    pContext->hwState.kelvin.set1 (NV097_SET_EDGE_FLAG, NV097_SET_EDGE_FLAG_V_TRUE);

    // initialize immediate mode data
    pContext->hwState.kelvin.set3f (NV097_SET_NORMAL3F(0), 0.0f, 0.0f, 1.0f);

    pContext->hwState.kelvin.set4f (NV097_SET_TEXCOORD0_4F(0), 0.0f, 0.0f, 0.0f, 1.0f);
    pContext->hwState.kelvin.set4f (NV097_SET_TEXCOORD1_4F(0), 0.0f, 0.0f, 0.0f, 1.0f);
    pContext->hwState.kelvin.set4f (NV097_SET_TEXCOORD2_4F(0), 0.0f, 0.0f, 0.0f, 1.0f);
    pContext->hwState.kelvin.set4f (NV097_SET_TEXCOORD3_4F(0), 0.0f, 0.0f, 0.0f, 1.0f);

    pContext->hwState.kelvin.set1f (NV097_SET_FOG1F, 0.0f);

    // set image (unswizzled) texture pitch to an allowable, innocuous value
    for (DWORD dwTex=0; dwTex < KELVIN_NUM_TEXTURES; dwTex++) {
        pContext->hwState.kelvin.set1 (NV097_SET_TEXTURE_CONTROL1(dwTex),
                                       DRF_NUM (097, _SET_TEXTURE_CONTROL1, _IMAGE_PITCH, 0x0008));
    }

    // swath width for the boustrophedonic rasterization
    // todo - use montrym's algorithm
    pContext->hwState.kelvin.set1 (NV097_SET_SWATH_WIDTH, NV097_SET_SWATH_WIDTH_V_128);

    pContext->hwState.kelvin.set4f (NV097_SET_EYE_POSITION(0),  0.0f, 0.0f, 0.0f, 1.0f);
    pContext->hwState.kelvin.set3f (NV097_SET_EYE_DIRECTION(0), 0.0f, 0.0f,-1.0f);

    // we'll always cull what we call backfacing geometry.
    // this goes along with D3D's definition of culling.
    pContext->hwState.kelvin.set1 (NV097_SET_CULL_FACE, NV097_SET_CULL_FACE_V_BACK);

    // always clip <0
    pContext->hwState.kelvin.set1 (NV097_SET_SHADER_CLIP_PLANE_MODE, 0);

    // fog plane
    pContext->hwState.kelvin.set4f (NV097_SET_FOG_PLANE(0), 0.0f, 0.0f, 1.0f, 0.0f);

#ifndef DISABLE_Z_COMPR
    // compressed z
    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ZCOMPRESSENABLE_MASK) == D3D_REG_ZCOMPRESSENABLE_ENABLE) {
        pContext->hwState.kelvin.set1 (NV097_SET_COMPRESS_ZBUFFER_EN, NV097_SET_COMPRESS_ZBUFFER_EN_V_ENABLE);
    }
#endif

    // no two-sided lighting in d3d (maybe in dx8?)
    pContext->hwState.kelvin.set1 (NV097_SET_TWO_SIDE_LIGHT_EN, NV097_SET_TWO_SIDE_LIGHT_EN_V_FALSE);

    // set up clipping in z and w. for now default to strict nv10-like behavior
    pContext->hwState.kelvin.set1 (NV097_SET_ZMIN_MAX_CONTROL,
                                   (DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _CULL_NEAR_FAR_EN, _TRUE) |
                                    DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _ZCLAMP_EN, _CULL)        |
                                    DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _CULL_IGNORE_W, _FALSE)));

    // for DX7, dependent texture lookups always run between stages i and i+1
    pContext->hwState.kelvin.set1 (NV097_SET_SHADER_OTHER_STAGE_INPUT,
                                   (DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE1, _INSTAGE_0) |
                                    DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE2, _INSTAGE_1) |
                                    DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE3, _INSTAGE_2)));

    dbgFlushType (NVDBG_FLUSH_STATE);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set surface info: offset, pitch, clip

HRESULT nvSetKelvinSurfaceInfo (PNVD3DCONTEXT pContext)
{
    DWORD dwFormat;
    DWORD dwOffset, dwPitch;
    DWORD dwZOffset, dwZPitch;
    DWORD dwClipH, dwClipV;  // horizontal and vertical clip locations
    DWORD dwOcclude;

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_SURFACE;

    // get  render targets
#if (NVARCH >= 0x020)
    CSimpleSurface *pRenderTarget = pContext->kelvinAA.GetCurrentRT(pContext);
    CSimpleSurface *pZetaBuffer = pContext->kelvinAA.GetCurrentZB(pContext);
#else
    CSimpleSurface *pRenderTarget = pContext->pRenderTarget;
    CSimpleSurface *pZetaBuffer = pContext->pZetaBuffer;
#endif

    nvAssert (pRenderTarget);

    // surface formats
    dwFormat = DRF_NUM (097, _SET_SURFACE_FORMAT, _COLOR, nv097SurfaceFormat[pRenderTarget->getFormat()]);

    if (pRenderTarget->isSwizzled()) {
        CTexture* pTexture = pRenderTarget->getWrapper()->getTexture();
        nvAssert(pTexture);
        dwFormat |= DRF_DEF (097, _SET_SURFACE_FORMAT, _TYPE, _SWIZZLE);
        dwFormat |= DRF_NUM (097, _SET_SURFACE_FORMAT, _WIDTH,  pTexture->getLogWidth());
        dwFormat |= DRF_NUM (097, _SET_SURFACE_FORMAT, _HEIGHT, pTexture->getLogHeight());
    }
    else {
        dwFormat |= DRF_DEF (097, _SET_SURFACE_FORMAT, _TYPE, _PITCH);
    }

    dwFormat |= pZetaBuffer ?
                DRF_NUM (097, _SET_SURFACE_FORMAT, _ZETA, nv097SurfaceFormat[pZetaBuffer->getFormat()]) :
                // no zbuffer - set the format according to the render target bit depth to prevent an RM exception
                DRF_NUM (097, _SET_SURFACE_FORMAT, _ZETA, (pRenderTarget->getBPP() == 2) ?
                         NV097_SET_SURFACE_FORMAT_ZETA_Z16 : NV097_SET_SURFACE_FORMAT_ZETA_Z24S8);

    dwFormat |= pContext->kelvinAA.GetAAFormat();

    // render target pitch and offset
#ifdef STEREO_SUPPORT
    dwOffset = GetStereoOffset(pRenderTarget);
#else  //STEREO_SUPPORT==0
    dwOffset = pRenderTarget->getOffset();
#endif //STEREO_SUPPORT
    dwPitch  = pRenderTarget->getPitch();

    // zeta buffer pitch and offset
    if (pZetaBuffer) {
#ifdef  STEREO_SUPPORT
        dwZOffset = GetStereoOffset(pZetaBuffer);
#else //STEREO_SUPPORT==0
        dwZOffset = pZetaBuffer->getOffset();
#endif//STEREO_SUPPORT
        dwZPitch  = pZetaBuffer->getPitch();
    }
    else {
        dwZOffset = 0;
        dwZPitch  = KELVIN_ALIGNMENT_MIN;
    }

    // send everything to the HW
    pContext->hwState.kelvin.set3 (NV097_SET_SURFACE_PITCH,
                                   (DRF_NUM (097, _SET_SURFACE_PITCH, _COLOR, dwPitch) |
                                    DRF_NUM (097, _SET_SURFACE_PITCH, _ZETA,  dwZPitch)),
                                   dwOffset,
                                   dwZOffset);
    pContext->hwState.kelvin.set1 (NV097_SET_SURFACE_FORMAT, dwFormat);

    // since we're using the window clip to clip to the viewport,
    // the surface clip can just clip to the whole window.
    dwClipH = DRF_NUM (097, _SET_SURFACE_CLIP_HORIZONTAL, _X, 0) |
              DRF_NUM (097, _SET_SURFACE_CLIP_HORIZONTAL, _WIDTH, ((DWORD)pRenderTarget->getWidth()));
    dwClipV = DRF_NUM (097, _SET_SURFACE_CLIP_VERTICAL, _Y, 0) |
              DRF_NUM (097, _SET_SURFACE_CLIP_VERTICAL, _HEIGHT, ((DWORD)pRenderTarget->getHeight()));

    pContext->hwState.kelvin.set2 (NV097_SET_SURFACE_CLIP_HORIZONTAL, dwClipH, dwClipV);

    // set up occlusion culling. this doesn't logically belong here, but ends up here
    // because of an unfortunate HW bug that yields a dependancy on z-buffer format

    // BUGBUG: BSK 1/5/00 -- Disable Z CULL in AA modes -- it's broken and yet slow. Remove this for NV25.
    if (pZetaBuffer && !pContext->kelvinAA.IsEnabled() && ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ZCULLENABLE_MASK) == D3D_REG_ZCULLENABLE_ENABLE)) {
        dwOcclude  = DRF_DEF (097, _SET_OCCLUDE_ZSTENCIL_EN, _OCCLUDE_STENCIL_EN, _ENABLE);
        dwOcclude |= (pZetaBuffer->getBPP() == 2) ?
                     DRF_DEF (097, _SET_OCCLUDE_ZSTENCIL_EN, _OCCLUDE_ZEN, _DISABLE) :
                     DRF_DEF (097, _SET_OCCLUDE_ZSTENCIL_EN, _OCCLUDE_ZEN, _ENABLE);
    }
    else {
        dwOcclude  = DRF_DEF (097, _SET_OCCLUDE_ZSTENCIL_EN, _OCCLUDE_STENCIL_EN, _DISABLE) |
                     DRF_DEF (097, _SET_OCCLUDE_ZSTENCIL_EN, _OCCLUDE_ZEN, _DISABLE);
    }
    pContext->hwState.kelvin.set1 (NV097_SET_OCCLUDE_ZSTENCIL_EN, dwOcclude);

    // cache the z scales
    if (pZetaBuffer) {
        if (pZetaBuffer->getBPP() == 2) {
            pContext->hwState.dvZScale    = KELVIN_Z_SCALE16;
            pContext->hwState.dvInvZScale = KELVIN_Z_SCALE16_INV;
        }
        else {
            pContext->hwState.dvZScale    = KELVIN_Z_SCALE24;
            pContext->hwState.dvInvZScale = KELVIN_Z_SCALE24_INV;
        }
    }

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set up the combiners to produce one of the legacy D3D texture blends

HRESULT nvKelvinSetTextureBlend (PNVD3DCONTEXT pContext)
{
    DWORD dwTBlend;

    if (pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]) {

        // we have a texture. go ahead and do the blend

        dwTBlend = pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND];

        nvAssert (dwTBlend <= D3D_TBLEND_MAX);  // make sure we're not off the end of the table

        pContext->hwState.dwAlphaICW[0] = kelvinTextureBlendSettings[dwTBlend][0];
        pContext->hwState.dwColorICW[0] = kelvinTextureBlendSettings[dwTBlend][1];
        pContext->hwState.dwAlphaOCW[0] = kelvinTextureBlendSettings[dwTBlend][2];
        pContext->hwState.dwColorOCW[0] = kelvinTextureBlendSettings[dwTBlend][3];

        // handle the stupid special case in which we have TBLEND_MODULATE and a
        // texture without alpha; use D3DTBLEND_MODULATEALPHA instead
        if (dwTBlend == D3DTBLEND_MODULATE) {
            CTexture *pTexture = ((CNvObject *)(pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]))->getTexture();
            if (!pTexture->hasAlpha()) {
                pContext->hwState.dwAlphaICW[0] = kelvinTextureBlendSettings[D3DTBLEND_MODULATEALPHA][0];
            }
        }

        pContext->hwState.dwTexUnitToTexStageMapping[0] = 0;

    }

    else {

        // they've enabled a texture blend without a texture. just use diffuse
        pContext->hwState.dwAlphaICW[0] = kelvinTextureBlendDefault[0];
        pContext->hwState.dwColorICW[0] = kelvinTextureBlendDefault[1];
        pContext->hwState.dwAlphaOCW[0] = kelvinTextureBlendDefault[2];
        pContext->hwState.dwColorOCW[0] = kelvinTextureBlendDefault[3];

    }

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set up the kelvin combiners for either the current texture stage state
// or a legacy texture blend

HRESULT nvSetKelvinColorCombiners     (PNVD3DCONTEXT pContext)
{
    DWORD dwStage;
    BOOL  bStageActive;
    DWORD dwMapping[KELVIN_NUM_TEXTURES];
    DWORD dwControl;

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_COMBINERS_COLOR;

    // save off the last prim type for which combiners were set up (affects point sprites)
    pContext->hwState.dwCombinerDP2Prim = pContext->dp2.dwDP2Prim;

    // cache current values
    for (dwStage = 0; dwStage < KELVIN_NUM_TEXTURES; dwStage++) {
        dwMapping[dwStage] = pContext->hwState.dwTexUnitToTexStageMapping[dwStage];
    }

    // clear state that will be re-determined here
    pContext->hwState.dwNumActiveCombinerStages = 0;
    for (dwStage=0; dwStage < KELVIN_NUM_TEXTURES; dwStage++) {
        pContext->hwState.dwTexUnitToTexStageMapping[dwStage] = KELVIN_UNUSED;
    }

    pContext->hwState.dwNextAvailableTextureUnit = ((pContext->dwRenderState[D3DRS_POINTSPRITEENABLE]) &&
                                                    (pContext->dp2.dwDP2Prim == D3DDP2OP_POINTS)) ? 3 : 0;

    if (pContext->bUseTBlendSettings) {

        nvKelvinSetTextureBlend (pContext);
        pContext->hwState.dwNumActiveCombinerStages = 1;

    }

    else {

        // process all active texture stages
        do {

            bStageActive = nvConstructKelvinColorCombiners (pContext,
                                                            pContext->hwState.dwNumActiveCombinerStages,
                                                            pContext->hwState.dwNumActiveCombinerStages);
            nvConstructKelvinAlphaCombiners (pContext,
                                             pContext->hwState.dwNumActiveCombinerStages,
                                             pContext->hwState.dwNumActiveCombinerStages,
                                             bStageActive);

            // if the current unit got assigned to this stage, move on to the next
            if ((pContext->hwState.dwNextAvailableTextureUnit < KELVIN_NUM_TEXTURES) &&
                (pContext->hwState.dwTexUnitToTexStageMapping[pContext->hwState.dwNextAvailableTextureUnit] != KELVIN_UNUSED)) {
                pContext->hwState.dwNextAvailableTextureUnit++;
            }

            if ((bStageActive) || (pContext->hwState.dwNumActiveCombinerStages == 0)) {
                pContext->hwState.dwNumActiveCombinerStages++;
            }

        } while (bStageActive && pContext->hwState.dwNumActiveCombinerStages < KELVIN_NUM_COMBINERS);

    }

    // clear all the unused stages
    for (dwStage=pContext->hwState.dwNumActiveCombinerStages; dwStage<KELVIN_NUM_COMBINERS; dwStage++) {
        pContext->hwState.dwColorICW[dwStage] = 0;
        pContext->hwState.dwColorOCW[dwStage] = 0;
        pContext->hwState.dwAlphaICW[dwStage] = 0;
        pContext->hwState.dwAlphaOCW[dwStage] = 0;
    }

    // set control bits
    dwControl = DRF_NUM (097, _SET_COMBINER_CONTROL, _ITERATION_COUNT,
                         pContext->hwState.dwNumActiveCombinerStages);
    dwControl |= (DRF_DEF (097, _SET_COMBINER_CONTROL, _MUX_SELECT, _LSB)           |
                  DRF_DEF (097, _SET_COMBINER_CONTROL, _FACTOR0, _SAME_FACTOR_ALL)  |
                  DRF_DEF (097, _SET_COMBINER_CONTROL, _FACTOR1, _SAME_FACTOR_ALL));

#if STATE_OPTIMIZE
    // send the active stages to the hardware
    pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_CONTROL, dwControl);
    pContext->hwState.kelvin.seta (NV097_SET_COMBINER_COLOR_ICW(0), pContext->hwState.dwNumActiveCombinerStages, pContext->hwState.dwColorICW);
    pContext->hwState.kelvin.seta (NV097_SET_COMBINER_COLOR_OCW(0), pContext->hwState.dwNumActiveCombinerStages, pContext->hwState.dwColorOCW);
    pContext->hwState.kelvin.seta (NV097_SET_COMBINER_ALPHA_ICW(0), pContext->hwState.dwNumActiveCombinerStages, pContext->hwState.dwAlphaICW);
    pContext->hwState.kelvin.seta (NV097_SET_COMBINER_ALPHA_OCW(0), pContext->hwState.dwNumActiveCombinerStages, pContext->hwState.dwAlphaOCW);

#else
    // send everything to the hardware
    pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_CONTROL, dwControl);
    pContext->hwState.kelvin.set8 (NV097_SET_COMBINER_COLOR_ICW(0),
                                   pContext->hwState.dwColorICW[0],
                                   pContext->hwState.dwColorICW[1],
                                   pContext->hwState.dwColorICW[2],
                                   pContext->hwState.dwColorICW[3],
                                   pContext->hwState.dwColorICW[4],
                                   pContext->hwState.dwColorICW[5],
                                   pContext->hwState.dwColorICW[6],
                                   pContext->hwState.dwColorICW[7]);
    pContext->hwState.kelvin.set8 (NV097_SET_COMBINER_COLOR_OCW(0),
                                   pContext->hwState.dwColorOCW[0],
                                   pContext->hwState.dwColorOCW[1],
                                   pContext->hwState.dwColorOCW[2],
                                   pContext->hwState.dwColorOCW[3],
                                   pContext->hwState.dwColorOCW[4],
                                   pContext->hwState.dwColorOCW[5],
                                   pContext->hwState.dwColorOCW[6],
                                   pContext->hwState.dwColorOCW[7]);
    pContext->hwState.kelvin.set8 (NV097_SET_COMBINER_ALPHA_ICW(0),
                                   pContext->hwState.dwAlphaICW[0],
                                   pContext->hwState.dwAlphaICW[1],
                                   pContext->hwState.dwAlphaICW[2],
                                   pContext->hwState.dwAlphaICW[3],
                                   pContext->hwState.dwAlphaICW[4],
                                   pContext->hwState.dwAlphaICW[5],
                                   pContext->hwState.dwAlphaICW[6],
                                   pContext->hwState.dwAlphaICW[7]);
    pContext->hwState.kelvin.set8 (NV097_SET_COMBINER_ALPHA_OCW(0),
                                   pContext->hwState.dwAlphaOCW[0],
                                   pContext->hwState.dwAlphaOCW[1],
                                   pContext->hwState.dwAlphaOCW[2],
                                   pContext->hwState.dwAlphaOCW[3],
                                   pContext->hwState.dwAlphaOCW[4],
                                   pContext->hwState.dwAlphaOCW[5],
                                   pContext->hwState.dwAlphaOCW[6],
                                   pContext->hwState.dwAlphaOCW[7]);
#endif

    // if we've changed the flags or mappings, invalidate a bunch of state

    //if ((pContext->hwState.dwStateFlags & KELVIN_MASK_COMBINERPROGRAMUNIQUENESS) != dwStateFlags) {
    //    pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_SPECFOG_COMBINER |
    //                                            KELVIN_DIRTY_LIGHTS);
    //}

#ifdef TESTCALLANDRETURN

    // this should render a pink banana in bend
    getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);

    DWORD dwPBBase = getDC()->nvPusher.getBase();
    DWORD dwPBEnd = dwPBBase + getDC()->nvPusher.getSize();
    DWORD dwFuncBase = dwPBEnd - (14<<2);

    getDC()->nvPusher.push (0, NVPUSHER_CALL(dwFuncBase-dwPBBase));
    getDC()->nvPusher.adjust(1);

    dwControl = DRF_NUM (097, _SET_COMBINER_CONTROL, _ITERATION_COUNT, 1);
    dwControl |= (DRF_DEF (097, _SET_COMBINER_CONTROL, _MUX_SELECT, _LSB)           |
                  DRF_DEF (097, _SET_COMBINER_CONTROL, _FACTOR0, _SAME_FACTOR_ALL)  |
                  DRF_DEF (097, _SET_COMBINER_CONTROL, _FACTOR1, _SAME_FACTOR_ALL));

    DWORD dwCICW = 0x20010000;
    DWORD dwCOCW = 0x00000c00;
    DWORD dwAICW = 0x30300000;
    DWORD dwAOCW = 0x00000c00;

    ((DWORD*)dwFuncBase)[0]  = (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_COMBINER_FACTOR0(0));
    ((DWORD*)dwFuncBase)[1]  = 0x00ff00ff;
    ((DWORD*)dwFuncBase)[2]  = (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_COMBINER_CONTROL);
    ((DWORD*)dwFuncBase)[3]  = dwControl;
    ((DWORD*)dwFuncBase)[4]  = (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_COMBINER_COLOR_ICW(0));
    ((DWORD*)dwFuncBase)[5]  = dwCICW;
    ((DWORD*)dwFuncBase)[6]  = (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_COMBINER_COLOR_OCW(0));
    ((DWORD*)dwFuncBase)[7]  = dwCOCW;
    ((DWORD*)dwFuncBase)[8]  = (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_COMBINER_ALPHA_ICW(0));
    ((DWORD*)dwFuncBase)[9]  = dwAICW;
    ((DWORD*)dwFuncBase)[10] = (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_COMBINER_ALPHA_OCW(0));
    ((DWORD*)dwFuncBase)[11] = dwAOCW;
    ((DWORD*)dwFuncBase)[12] = NVPUSHER_RETURN();

    getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);

#endif

    for (dwStage=0; dwStage<KELVIN_NUM_TEXTURES; dwStage++) {
        if (pContext->hwState.dwTexUnitToTexStageMapping[dwStage] != dwMapping[dwStage]) {
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_STATE
                                           |  KELVIN_DIRTY_TEXTURE_TRANSFORM
                                           |  KELVIN_DIRTY_FVF
                                           |  KELVIN_DIRTY_TRANSFORM;
            break;
        }
    }

#ifdef ALPHA_CULL
    if(pContext->dwEarlyCopyStrategy && pContext->hwState.alphacull_mode >= 2)
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_MISC_STATE;
#endif

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvSetKelvinPixelShader (PNVD3DCONTEXT pContext)
{
    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_PIXEL_SHADER;

    if (pContext->pCurrentPShader) {
        pContext->pCurrentPShader->setKelvinState(pContext);
        pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_TEXTURE_STATE     |
                                           KELVIN_DIRTY_FVF               |
                                           KELVIN_DIRTY_TEXTURE_TRANSFORM |
                                           KELVIN_DIRTY_TRANSFORM);
    }

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// find next clip plane, if any, given clip plane enable bits and the
// current clip plane index. The former is expected to already have been
// shifted right by the latter.

__inline D3DVALUE *nvKelvinGetClipPlane
(
    PNVD3DCONTEXT pContext,
    DWORD *dwClipPlaneEnable,
    DWORD *dwCurrentClipPlaneIndex
)
{
    D3DVALUE *pClipPlane;

    if (*dwClipPlaneEnable) {
        // skip over disabled planes
        while (!((*dwClipPlaneEnable) & 0x1)) {
            (*dwClipPlaneEnable) >>= 1;
            (*dwCurrentClipPlaneIndex) ++;
        }
        // pick off the enabled plane
        pClipPlane = pContext->ppClipPlane[*dwCurrentClipPlaneIndex];
        (*dwClipPlaneEnable) >>= 1;
        (*dwCurrentClipPlaneIndex) ++;
    }
    else {
        pClipPlane = NULL;
    }

    return (pClipPlane);
}

//---------------------------------------------------------------------------

// set kelvin texture state, including:
//      - formats
//      - filters
//      - offsets
// note: the combiners must be set up before calling this routine!

HRESULT nvSetKelvinTextureState (PNVD3DCONTEXT pContext)
{
    DWORD               dwHWStage, dwD3DStage;
    PNVD3DTEXSTAGESTATE pTSSState;
    CTexture           *pTexture;
    CNvObject          *pTexObj;
    CNvObject          *pPalObj;
    CSimpleSurface     *pPalette;
    DWORD               dwOffset, dwFormat, dwAddress;
    DWORD               dwControl0, dwControl1;
    DWORD               dwFilter, dwRect, dwPalette, dwKey;
    DWORD               dwTexUnitStatus, dwLastTexUnitStatus;
    DWORD               dwTexGen, dwTCIndex, dwTexgenMode;
    DWORD               dwUserCoordsNeeded = 0;
    DWORD               dwTexCoordIndices = 0;
    DWORD               dwNumTexCoordsNeeded = 0;
    DWORD               dwInvMVNeeded = 0;
    BOOL                bCubeMap, bBumpMap, bSignedHILO, bUnsignedHILO, bLuminanceMode, bTextureHasLuminance;
    BOOL                bWrapU, bWrapV, bWrapP, bWrapQ;
    DWORD               dwClipPlaneEnable, dwCurrentClipPlaneIndex, dwClipStagesAllowed;
    DWORD               dwPlane, dwMethodOffset;
    D3DVALUE            dvScale, dvOffset;
    D3DVALUE            pClipPlaneEye[4];
    D3DVALUE           *pClipPlane[KELVIN_NUM_TEXTURES];
    D3DMATRIX           mTmp, mTmp2;
    DWORD               dwShaderMode[KELVIN_NUM_TEXTURES];
    DWORD               dwRGBMapping[KELVIN_NUM_TEXTURES];

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_TEXTURE_STATE;

    dwClipStagesAllowed     = KELVIN_CAPS_MAX_USER_CLIP_PLANES_ALLOWED >> 2;
    dwClipPlaneEnable       = pContext->dwRenderState[D3DRENDERSTATE_CLIPPLANEENABLE];
    dwCurrentClipPlaneIndex = 0;

    dwLastTexUnitStatus = KELVIN_TEXUNITSTATUS_IDLE;

    // iterate over the HW's texture units

    for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {

        // which D3D stage is handled by this HW stage
        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];

        if (dwD3DStage != KELVIN_UNUSED) {

#ifdef NV_PROFILE_DP2OPS
            pDriverData->pBenchmark->incActiveTextures();
#endif

            // the combiners assigned this texture unit.
            // we must have a legitimate texture to use

            pTSSState = &(pContext->tssState[dwD3DStage]);
            pTexObj = (CNvObject *)(pTSSState->dwValue[D3DTSS_TEXTUREMAP]);
            pTexture = pTexObj ? pTexObj->getTexture() : 0;

            if(!pTexture){
                //we should ALWAYS have a texture by this point -- substitute the default texture
                //this should be considered a setup error and will propogate out to 
                //nvValidateTextureStageState
                pTexture = getDC()->pDefaultTexture;
                pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
            }

            if (pTexture) {

                dbgTextureDisplay (pTexture);

                bCubeMap = pTexture->isCubeMap();
                if (bCubeMap) {
                    // cubemaps may only be referenced from their bases
                    pTexture = pTexture->getBaseTexture();
                }

                bBumpMap = (pContext->pCurrentPShader) ?
                        pContext->pCurrentPShader->isBEM(dwHWStage) :
                        (pTSSState->dwValue[D3DTSS_COLOROP] == D3DTOP_BUMPENVMAP) || (pTSSState->dwValue[D3DTSS_COLOROP] == D3DTOP_BUMPENVMAPLUMINANCE);

                if ((bBumpMap ^ pTexture->isBumpMap()) && !pContext->pCurrentPShader) {
                    DPF ("stupid app (WHQL) using bumpmapping w/o a bumpmap texture or vice-versa");
                    dbgD3DError();
                    pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                }

                if (bBumpMap) {
                    // some kind of bumpmapping is desired. the exact flavor and how we treat it
                    // depends on the DX version, the blend mode, and the texture format. ugh.
                    bLuminanceMode = (pContext->pCurrentPShader) ?
                                    pContext->pCurrentPShader->isBEML(dwHWStage) :
                                    (pTSSState->dwValue[D3DTSS_COLOROP] == D3DTOP_BUMPENVMAPLUMINANCE);
                    bTextureHasLuminance = ((pTexture->getFormat() == NV_SURFACE_FORMAT_L6DV5DU5) ||
                                            (pTexture->getFormat() == NV_SURFACE_FORMAT_X8L8DV8DU8)) ? TRUE : FALSE;
                    dwTexUnitStatus = (!bLuminanceMode && !bTextureHasLuminance) ?
                                        KELVIN_TEXUNITSTATUS_BUMPMAPA :
                                        (!bLuminanceMode && bTextureHasLuminance) ?
                                            KELVIN_TEXUNITSTATUS_BUMPMAPB :
                                            (bLuminanceMode && !bTextureHasLuminance) ?
                                                KELVIN_TEXUNITSTATUS_BUMPMAPC :
                                                KELVIN_TEXUNITSTATUS_BUMPMAPD;
                }
                else {
                    dwTexUnitStatus = KELVIN_TEXUNITSTATUS_USER;
                }

                pContext->hwState.pTextureInUse[dwHWStage] = pTexture;

            }  // pTexture

            // we need this data later, for stomping out the projective coordinate when apps are stupid
            // and send down 4 coordinates, even though they don't need them
            if (pContext->pCurrentPShader) {
                dwNumTexCoordsNeeded |= KELVIN_SET_NUM_TEX_COORDS(dwHWStage, pContext->pCurrentPShader->getNumTexCoords(pContext, dwHWStage));
            } else if (pTexture) {
                dwNumTexCoordsNeeded |= KELVIN_SET_NUM_TEX_COORDS(dwHWStage, pTexture->getNumTexCoords());
            }

            // texgen -------------------------------------------------------
            if (pContext->pCurrentVShader->hasProgram()) {
                dwTexgenMode = NV097_SET_TEXGEN_S_V_DISABLE;  // vertex shader subsumes texgen
                dwTCIndex    = dwD3DStage;                    // mapping between vertex shader outputs and texture stages is identity
            }
            else {
                dwTexGen  = (pContext->hwState.dwStateFlags & KELVIN_FLAG_PASSTHROUGHMODE) ?
                            D3DTSS_TCI_PASSTHRU :
                            pTSSState->dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff0000;
                dwTCIndex = pTSSState->dwValue[D3DTSS_TEXCOORDINDEX] & 0x0000ffff;
                switch (dwTexGen) {
                    case D3DTSS_TCI_PASSTHRU:
                        if (pContext->pCurrentVShader->bVAExists (defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex])) {
                            dwUserCoordsNeeded |= KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage);
                            dwTexCoordIndices  |= dwTCIndex << (8*dwHWStage);
                        }
                        else if (!pContext->dwRenderState[D3DRS_POINTSPRITEENABLE]) {
                            // the architects are on crack. the NV097_SET_TEXCOORDx_4F methods are not evenly spaced.
                            DWORD dwBase = (dwHWStage == 3) ?
                                           NV097_SET_TEXCOORD3_4F(0) :
                                           NV097_SET_TEXCOORD0_4F(0) + 0x28*dwHWStage;
                            // u,v = 0,0  (says MS)W=1.0 THIS IS IMPORTANT (DOH!)
                            pContext->hwState.kelvin.set4f (dwBase, 0.0f, 0.0f, 0.0f, 1.0f);
                        }
                        dwTexgenMode = NV097_SET_TEXGEN_S_V_DISABLE;
                        break;
                    case D3DTSS_TCI_CAMERASPACENORMAL:
                        dwInvMVNeeded = KELVIN_FLAG_TEXSTATENEEDSINVMV;
                        dwTexgenMode = NV097_SET_TEXGEN_S_V_NORMAL_MAP;
                        break;
                    case D3DTSS_TCI_CAMERASPACEPOSITION:
                        pContext->hwState.kelvin.set4f (NV097_SET_TEXGEN_PLANE_S(dwHWStage,0), 1.0f, 0.0f, 0.0f, 0.0f);
                        pContext->hwState.kelvin.set4f (NV097_SET_TEXGEN_PLANE_T(dwHWStage,0), 0.0f, 1.0f, 0.0f, 0.0f);
                        pContext->hwState.kelvin.set4f (NV097_SET_TEXGEN_PLANE_R(dwHWStage,0), 0.0f, 0.0f, 1.0f, 0.0f);
                        dwTexgenMode = NV097_SET_TEXGEN_S_V_EYE_LINEAR;
                        break;
                    case D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR:
                        dwInvMVNeeded = KELVIN_FLAG_TEXSTATENEEDSINVMV;
                        dwTexgenMode = NV097_SET_TEXGEN_S_V_REFLECTION_MAP;
                        break;
                    default:
                        DPF ("unknown texgen mode in nvSetKelvinTextureState");
                        dbgD3DError();
                        pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                        break;
                }
            }

            // we always generate three coordinates (i think)
            pContext->hwState.kelvin.set4 (NV097_SET_TEXGEN_S(dwHWStage),
                                           dwTexgenMode, dwTexgenMode,
                                           dwTexgenMode, NV097_SET_TEXGEN_Q_V_DISABLE);
            pContext->hwState.kelvin.set1 (NV097_SET_TEXGEN_VIEW_MODEL,
                                           pContext->dwRenderState[D3DRENDERSTATE_LOCALVIEWER] ?
                                           NV097_SET_TEXGEN_VIEW_MODEL_V_LOCAL_VIEWER :
                                           NV097_SET_TEXGEN_VIEW_MODEL_V_INFINITE_VIEWER);

            if (pTexture) {

                // offset -------------------------------------------------------
                dwOffset = pTexture->getSwizzled()->getOffset();

                // format -------------------------------------------------------
                dwFormat  = DRF_NUM (097, _SET_TEXTURE_FORMAT, _CONTEXT_DMA, nv097TextureContextDma[pTexture->getSwizzled()->getContextDMA()]);
                dwFormat |= DRF_NUM (097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, bCubeMap);

                dwFormat |= (pTexture->isVolumeMap()) ?
                            DRF_DEF (097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, _THREE) :
                            DRF_DEF (097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, _TWO);

                if ((pTSSState->dwValue[D3DTSS_ADDRESSU] == D3DTADDRESS_BORDER) ||
                    (pTSSState->dwValue[D3DTSS_ADDRESSV] == D3DTADDRESS_BORDER) ||
                    (pTSSState->dwValue[D3DTSS_ADDRESSW] == D3DTADDRESS_BORDER)) {
                    pContext->hwState.kelvin.set1 (NV097_SET_TEXTURE_BORDER_COLOR(dwHWStage), pTSSState->dwValue[D3DTSS_BORDERCOLOR]);
                }

                dwFormat |= DRF_DEF (097, _SET_TEXTURE_FORMAT, _BORDER_SOURCE, _COLOR);
                dwFormat |= DRF_NUM (097, _SET_TEXTURE_FORMAT, _COLOR, nv097TextureFormat[pTexture->getFormat()]);

                dwFormat |= DRF_NUM (097, _SET_TEXTURE_FORMAT, _MIPMAP_LEVELS, pTexture->getMipMapCount());
                dwFormat |= DRF_NUM (097, _SET_TEXTURE_FORMAT, _BASE_SIZE_U,   pTexture->getLogicalLogWidth());
                dwFormat |= DRF_NUM (097, _SET_TEXTURE_FORMAT, _BASE_SIZE_V,   pTexture->getLogicalLogHeight());
                dwFormat |= DRF_NUM (097, _SET_TEXTURE_FORMAT, _BASE_SIZE_P,   pTexture->getLogicalLogDepth());

                // addressing ---------------------------------------------------

                // force clamp to edge if we're cubemapping
                dwAddress  = DRF_NUM (097, _SET_TEXTURE_ADDRESS, _U, pTexture->isCubeMap() ? nv097TextureAddress[3] : nv097TextureAddress[pTSSState->dwValue[D3DTSS_ADDRESSU]]);
                dwAddress |= DRF_NUM (097, _SET_TEXTURE_ADDRESS, _V, pTexture->isCubeMap() ? nv097TextureAddress[3] : nv097TextureAddress[pTSSState->dwValue[D3DTSS_ADDRESSV]]);
                dwAddress |= pTexture->isVolumeMap() ?    // D3DTSS_ADDRESSW doesn't seem to be valid if we don't have a volume map
                             DRF_NUM (097, _SET_TEXTURE_ADDRESS, _P, nv097TextureAddress[pTSSState->dwValue[D3DTSS_ADDRESSW]]) :
                             DRF_NUM (097, _SET_TEXTURE_ADDRESS, _P, nv097TextureAddress[1]);


                // check cylindrical wrapping (disallow it if we're cubemapping)
                NV_EVAL_CYLINDRICAL_WRAP (pContext, dwTCIndex, bWrapU, bWrapV, bWrapP, bWrapQ);
                dwAddress |= DRF_NUM (097, _SET_TEXTURE_ADDRESS, _CYLWRAP_U, bWrapU);
                dwAddress |= DRF_NUM (097, _SET_TEXTURE_ADDRESS, _CYLWRAP_V, bWrapV);
                dwAddress |= DRF_NUM (097, _SET_TEXTURE_ADDRESS, _CYLWRAP_P, bWrapP);
                dwAddress |= DRF_NUM (097, _SET_TEXTURE_ADDRESS, _CYLWRAP_Q, bWrapQ);

                // control0 -----------------------------------------------------
                dwControl0  = DRF_DEF (097, _SET_TEXTURE_CONTROL0, _ENABLE, _TRUE);

                // these are backward from openGL. d3d can only set the maximum mipmap level, by which they mean
                // the openGL minimum level (d3d maximum = largest sized level to use, not largest level number).
                // the d3d minimum level (the openGL / kelvin maximum) is always just the end of the chain
                dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MIN_LOD_CLAMP, ((pTSSState->dwValue[D3DTSS_MAXMIPLEVEL]) << 8));  // 4.8 fixed point
                // If we are not mip mapped, we need to set the MAX_LOD_CLAMP to 1 in order to
                // get min filtering (if set to 0 we only get mag filtering on non-mipmapped textures).
                if( pTexture->getMipMapCount() == 1 ) {
                    dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MAX_LOD_CLAMP, (1 << 8));  // 4.8 fixed point
                }
                else {
                    dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MAX_LOD_CLAMP, ((pTexture->getMipMapCount()-1) << 8));  // 4.8 fixed point
                }

                dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _ALPHA_KILL_ENABLE, _FALSE);
                dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _IMAGE_FIELD_ENABLE, _FALSE);  // BUGBUG what's this???

                // colorkey
                if ((pContext->dwRenderState[D3DRENDERSTATE_COLORKEYENABLE]) && (pTexture->hasColorKey())) {
                    dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _KILL);  // use this choice since the texture may not have alpha
                    dwKey = pTexture->getColorKey();
                    if (pTexture->isPalettized()) {
                        if ((pTexture->getWrapper()) &&
                            (pTexture->getWrapper()->getPalette()) &&
                            (pTexture->getWrapper()->getPalette()->getPaletteSurface()) &&
                            (dwKey < 256)) {
                            dwKey = ((DWORD *)(pTexture->getWrapper()->getPalette()->getPaletteSurface()->getAddress()))[dwKey];
                        }
                        else {
                            DPF ("unable to extract color key from palettized texture");
                            dbgD3DError();
                        }
                    }
                    pContext->hwState.kelvin.set1 (NV097_SET_COLOR_KEY_COLOR(dwHWStage), dwKey);
                }
                else {
                    dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _FALSE);
                }

                // control1 -----------------------------------------------------
                dwControl1 = DRF_NUM (097, _SET_TEXTURE_CONTROL1, _IMAGE_PITCH, 0x0008);

                // filter -------------------------------------------------------
                dwFilter  = DRF_DEF (097, _SET_TEXTURE_FILTER, _CONVOLUTION_KERNEL, _GAUSSIAN_3);

                bSignedHILO   = (pTexture->getFormat() == NV_SURFACE_FORMAT_HILO_HEMI_V16U16);
                bUnsignedHILO = (pTexture->getFormat() == NV_SURFACE_FORMAT_HILO_1_V16U16);

                if (pContext->pCurrentPShader && !pContext->pCurrentPShader->stageUsesTexture(dwHWStage)) {
                    dwFilter |= DRF_DEF (097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_DISABLED);
                } else if (bBumpMap) {
                    dwFilter |= DRF_DEF (097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_ENABLED)  |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_ENABLED);
                } else if (bSignedHILO) {
                    dwFilter |= DRF_DEF (097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_ENABLED)  |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_ENABLED)  |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_DISABLED);
                } else if (pTexture->getFourCC() == D3DFMT_Q8W8V8U8) {
                    dwFilter |= DRF_DEF (097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_ENABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_ENABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_ENABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_ENABLED);
                } else {
                    dwFilter |= DRF_DEF (097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_DISABLED) |
                                DRF_DEF (097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_DISABLED);
                }

                DWORD dwMinFilter = pTSSState->dwValue[D3DTSS_MINFILTER];
                DWORD dwMagFilter = pTSSState->dwValue[D3DTSS_MAGFILTER];
                DWORD dwMipFilter = pTSSState->dwValue[D3DTSS_MIPFILTER];

                if (pContext->dwDXAppVersion >= 0x800) {
                    // DX8 changed the filter enumerants. Remap them so we don't have to change our code.
                    dwMinFilter = dx8MinFilterMapping[dwMinFilter];
                    dwMagFilter = dx8MagFilterMapping[dwMagFilter];
                    dwMipFilter = dx8MipFilterMapping[dwMipFilter];
                }

                // Mag filter aniso is bilinear in HW, and this passes WHQL.
                // As a result trigger anisotropic off of Min filter only
                if ((dwMinFilter == D3DTFN_ANISOTROPIC) ||
                    (getDC()->nvD3DRegistryData.regAnisotropicLevel != D3D_REG_ANISOTROPICLEVEL_PASSIVE))
                {
                    DWORD dwMaxAnisotropy = (getDC()->nvD3DRegistryData.regAnisotropicLevel == D3D_REG_ANISOTROPICLEVEL_PASSIVE) ?
                                            pTSSState->dwValue[D3DTSS_MAXANISOTROPY] :
                                            getDC()->nvD3DRegistryData.regAnisotropicLevel;
                    // We can only handle anisotropy in powers-of-2.  Non powers-of-2 are
                    // promoted to higher levels of anisotropy.
                    switch (dwMaxAnisotropy) {
                        case 0:
                            // default back to point
                            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MIN, nv097TextureMinFilter[D3DTFN_POINT][dwMipFilter]);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MAG, nv097TextureMagFilter[dwMagFilter]);
                            break;
                        case 1:
                            // default back to linear with no aniso
                            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MIN, nv097TextureMinFilter[D3DTFN_LINEAR][dwMipFilter]);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MAG, nv097TextureMagFilter[dwMagFilter]);
                            break;
                        case 2:
                            // use real aniso
                            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _1);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MIN, nv097TextureMinFilter[D3DTFN_LINEAR][dwMipFilter]);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MAG, nv097TextureMagFilter[D3DTFG_LINEAR]);
                            if(dwMagFilter == D3DTFG_POINT) {
                                // When aniso filtering is on, mag filter will be linear, point filtering
                                // won't happen, so we must fail validate texture stage for DCT450.
                                pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                            }
                            break;
                        case 3:
                        case 4:
                            // use real aniso
                            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _2);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MIN, nv097TextureMinFilter[D3DTFN_LINEAR][dwMipFilter]);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MAG, nv097TextureMagFilter[D3DTFG_LINEAR]);
                            if(dwMagFilter == D3DTFG_POINT) {
                                // When aniso filtering is on, mag filter will be linear, point filtering
                                // won't happen, so we must fail validate texture stage for DCT450.
                                pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                            }
                            break;
                        default:
                            // aniso > 8. we can't really do it, but we'll approximate it as best we can
                            pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                            // fall through
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                            // use real aniso
                            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _3);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MIN, nv097TextureMinFilter[D3DTFN_LINEAR][dwMipFilter]);
                            dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MAG, nv097TextureMagFilter[D3DTFG_LINEAR]);
                            if(dwMagFilter == D3DTFG_POINT) {
                                // When aniso filtering is on, mag filter will be linear, point filtering
                                // won't happen, so we must fail validate texture stage for DCT450.
                                pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                            }
                            break;
                    }  // switch
                }

                else {  // anisotropic filtering should not be enabled
                    // Mag aniso is bilinear in HW, and this passes WHQL.
                    if (dwMagFilter == D3DTFG_ANISOTROPIC) {
                        dwMagFilter = D3DTFG_LINEAR;
                    }

                    dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);
                    dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MIN, nv097TextureMinFilter[dwMinFilter][dwMipFilter]);
                    dwFilter   |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MAG, nv097TextureMagFilter[dwMagFilter]);
                }

                // LOD bias
                dwFilter |= DRF_NUM (097, _SET_TEXTURE_FILTER, _MIPMAP_LOD_BIAS, pTSSState->dwLODBias);

                // image rect ---------------------------------------------------
                dwRect  = DRF_NUM (097, _SET_TEXTURE_IMAGE_RECT, _WIDTH, 8);  // this is ignored, just set something valid
                dwRect |= DRF_NUM (097, _SET_TEXTURE_IMAGE_RECT, _HEIGHT, 8);

                // palette ------------------------------------------------------
                dwPalette = 0;
                if (pTexture->isPalettized()) {
                    pPalObj = ((CNvObject *)(pTSSState->dwValue[D3DTSS_TEXTUREMAP]))->getPalette();
                    if (pPalObj) {
                        pPalette = pPalObj->getPaletteSurface();
                        nvAssert (pPalette);   // should already have bound a palette before using this texture
                        dwPalette |= DRF_NUM (097, _SET_TEXTURE_PALETTE, _CONTEXT_DMA, nv097PaletteContextDma[pPalette->getContextDMA()]);
                        dwPalette |= DRF_DEF (097, _SET_TEXTURE_PALETTE, _LENGTH, _256);
                        dwPalette |= DRF_NUM (097, _SET_TEXTURE_PALETTE, _OFFSET, (pPalette->getOffset() >> 6));  // offset measured is 64-byte units
                    } else {
                        DPF ("stupid app/runtime gave us a palettized texture without a palette");
                        DPF ("the app likely has no idea about alt-tabbing");
                        dbgD3DError();
                    }
                }
                else if (pTexture->hasAutoPalette()) {
                    nvAssert ((pTexture->getAutoPaletteOffset() & 63) == 0);
                    dwPalette |= DRF_NUM (097, _SET_TEXTURE_PALETTE, _CONTEXT_DMA, nv097PaletteContextDma[pTexture->getSwizzled()->getContextDMA()]);
                    dwPalette |= DRF_DEF (097, _SET_TEXTURE_PALETTE, _LENGTH, _256);
                    dwPalette |= DRF_NUM (097, _SET_TEXTURE_PALETTE, _OFFSET, (pTexture->getAutoPaletteOffset() >> 6));  // offset measured is 64-byte units
                }

                // send everything ----------------------------------------------
                pContext->hwState.kelvin.set6 (NV097_SET_TEXTURE_OFFSET(dwHWStage),
                                               dwOffset, dwFormat, dwAddress, dwControl0, dwControl1, dwFilter);
                pContext->hwState.kelvin.set2 (NV097_SET_TEXTURE_IMAGE_RECT(dwHWStage),
                                               dwRect, dwPalette);

                // bumpmapping and shader mode ----------------------------------
                // the latter is saved off to be sent to the HW when we're done with all textures
                if (dwLastTexUnitStatus & (KELVIN_TEXUNITSTATUS_BUMPMAPA |
                                           KELVIN_TEXUNITSTATUS_BUMPMAPB |
                                           KELVIN_TEXUNITSTATUS_BUMPMAPC |
                                           KELVIN_TEXUNITSTATUS_BUMPMAPD)) {

                    if (pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS] & D3DTTFF_PROJECTED) {
                        DPF ("kelvin097 cannot do projective texturing with bumpmapping");
                        pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                    }

                    DWORD dwBumpMapStage = (pContext->pCurrentPShader) ? dwD3DStage : (dwD3DStage-1);

                    pContext->hwState.kelvin.set4 (NV097_SET_TEXTURE_SET_BUMP_ENV_MAT00(dwHWStage),
                                                   pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVMAT00],
                                                   pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVMAT01],
                                                   pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVMAT11],
                                                   pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVMAT10]);

                    if (pContext->dwDXAppVersion < 0x0700) {
                        // DX6 or below
                        switch (dwLastTexUnitStatus) {
                            case KELVIN_TEXUNITSTATUS_BUMPMAPA:
                            case KELVIN_TEXUNITSTATUS_BUMPMAPB:
                            case KELVIN_TEXUNITSTATUS_BUMPMAPC:
                                dvScale  = 0.f;
                                dvOffset = FLOAT_FROM_DWORD(pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVLSCALE]) +
                                           FLOAT_FROM_DWORD(pContext->tssState[dwD3DStage-1].dwValue[D3DTSS_BUMPENVLOFFSET]);
                                dwShaderMode[dwHWStage] = NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE;
                                break;
                            case KELVIN_TEXUNITSTATUS_BUMPMAPD:
                                dvScale  = FLOAT_FROM_DWORD(pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVLSCALE]);
                                dvOffset = FLOAT_FROM_DWORD(pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVLOFFSET]);
                                dwShaderMode[dwHWStage] = NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE;
                                break;
                        }
                    }
                    else {
                        // DX7 or higher
                        switch (dwLastTexUnitStatus) {
                            case KELVIN_TEXUNITSTATUS_BUMPMAPA:
                            case KELVIN_TEXUNITSTATUS_BUMPMAPB:
                                dvScale  = 0.f;  // actually n/a
                                dvOffset = 0.f;  // actually n/a
                                dwShaderMode[dwHWStage] = NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP;
                                break;
                            case KELVIN_TEXUNITSTATUS_BUMPMAPC:
                                dvScale  = 0.f;
                                dvOffset = FLOAT_FROM_DWORD(pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVLSCALE]) +
                                           FLOAT_FROM_DWORD(pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVLOFFSET]);
                                dwShaderMode[dwHWStage] = NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE;
                                break;
                            case KELVIN_TEXUNITSTATUS_BUMPMAPD:
                                dvScale  = FLOAT_FROM_DWORD(pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVLSCALE]);
                                dvOffset = FLOAT_FROM_DWORD(pContext->tssState[dwBumpMapStage].dwValue[D3DTSS_BUMPENVLOFFSET]);
                                dwShaderMode[dwHWStage] = NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE;
                                break;
                        }
                    }
                    pContext->hwState.kelvin.set2f (NV097_SET_TEXTURE_SET_BUMP_ENV_SCALE(dwHWStage), dvScale, dvOffset);

                }

                else {
                    dwShaderMode[dwHWStage] = bCubeMap ?
                                                  NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CUBE_MAP :
                                                  pTexture->isVolumeMap() ?
                                                      NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_3D_PROJECTIVE :
                                                      NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_2D_PROJECTIVE;
                }

                // really only relevant for stages >= 1, but innocuous for stage 0
                dwRGBMapping[dwHWStage] = bSignedHILO ?
                                              NV097_SET_DOT_RGBMAPPING_STAGE1_HILO_HEMISPHERE_MS :
                                              (bUnsignedHILO ?
                                                  NV097_SET_DOT_RGBMAPPING_STAGE1_HILO_1 :
                                                  NV097_SET_DOT_RGBMAPPING_STAGE1_ZERO_TO_1);

            }

            else {

                dwTexUnitStatus = KELVIN_TEXUNITSTATUS_USER;

                // control 0
                dwControl0  = DRF_DEF (097, _SET_TEXTURE_CONTROL0, _ENABLE, _FALSE);
                dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MIN_LOD_CLAMP, 0);
                dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MAX_LOD_CLAMP, 0);
                dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _ALPHA_KILL_ENABLE, _FALSE);
                dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _IMAGE_FIELD_ENABLE, _FALSE);
                dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _FALSE);
                dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);

                pContext->hwState.kelvin.set1 (NV097_SET_TEXTURE_CONTROL0(dwHWStage), dwControl0);

            }

        }

        else if (dwClipPlaneEnable && dwClipStagesAllowed) {

            // if user clip planes are enabled, knock off as many as we can.
            // (each texture stage can handle four)

            dwTexUnitStatus = KELVIN_TEXUNITSTATUS_CLIP;
            pContext->hwState.pTextureInUse[dwHWStage] = NULL;

            dwClipStagesAllowed --;

            // fetch up to four planes
            for (dwPlane = 0; dwPlane < 4; dwPlane++) {
                pClipPlane[dwPlane] = nvKelvinGetClipPlane (pContext, &dwClipPlaneEnable, &dwCurrentClipPlaneIndex);
            }

            // we better have found at least one
            nvAssert(pClipPlane[0]);

            // D3D gives us the normals in world space, so they need to be converted to eye space
            // calculate the required matrix
            Inverse4x4 (&mTmp, &(pContext->xfmView), TRUE);
            MatrixTranspose4x4 (&mTmp2, &mTmp);

            dwMethodOffset = NV097_SET_TEXGEN_PLANE_S (dwHWStage,0);

            // transform and send as many planes as we have
            for (dwPlane = 0; dwPlane < 4; dwPlane++) {
                if (pClipPlane[dwPlane]) {
                    XformVector4 (pClipPlaneEye, pClipPlane[dwPlane], &mTmp2);
                }
                // send the last plane repeatedly to fill the remaining slots
                pContext->hwState.kelvin.set4f (dwMethodOffset,
                                                pClipPlaneEye[0], pClipPlaneEye[1],
                                                pClipPlaneEye[2], pClipPlaneEye[3]);
                dwMethodOffset += 0x10;
            }

            // generate texture coords from eye space coordinates
            pContext->hwState.kelvin.set4 (NV097_SET_TEXGEN_S(dwHWStage),
                                           NV097_SET_TEXGEN_S_V_EYE_LINEAR,
                                           NV097_SET_TEXGEN_T_V_EYE_LINEAR,
                                           NV097_SET_TEXGEN_R_V_EYE_LINEAR,
                                           NV097_SET_TEXGEN_Q_V_EYE_LINEAR);

            // according to Harold, the only thing we care about in this case is control0
            dwControl0  = DRF_DEF (097, _SET_TEXTURE_CONTROL0, _ENABLE, _TRUE);
            dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MIN_LOD_CLAMP, 0);
            dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MAX_LOD_CLAMP, 0);
            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _ALPHA_KILL_ENABLE, _FALSE);
            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _IMAGE_FIELD_ENABLE, _FALSE);
            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _FALSE);
            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);

            pContext->hwState.kelvin.set1 (NV097_SET_TEXTURE_CONTROL0(dwHWStage), dwControl0);

            // set the shader program to clip mode (pixel kill contingent on sign of eye
            // coords dotted with clip plane normal)
            dwShaderMode[dwHWStage] = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE;
            dwRGBMapping[dwHWStage] = NV097_SET_DOT_RGBMAPPING_STAGE1_ZERO_TO_1;

        }

        else if (!pContext->pCurrentPShader) {

            // no user texture, no clip planes. go ahead and totally disable the stage

            dwTexUnitStatus = KELVIN_TEXUNITSTATUS_IDLE;
            pContext->hwState.pTextureInUse[dwHWStage] = NULL;

            pContext->hwState.kelvin.set4 (NV097_SET_TEXGEN_S(dwHWStage),
                                           NV097_SET_TEXGEN_S_V_DISABLE,
                                           NV097_SET_TEXGEN_T_V_DISABLE,
                                           NV097_SET_TEXGEN_R_V_DISABLE,
                                           NV097_SET_TEXGEN_Q_V_DISABLE);

            // control 0
            dwControl0  = DRF_DEF (097, _SET_TEXTURE_CONTROL0, _ENABLE, _FALSE);
            dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MIN_LOD_CLAMP, 0);
            dwControl0 |= DRF_NUM (097, _SET_TEXTURE_CONTROL0, _MAX_LOD_CLAMP, 0);
            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _ALPHA_KILL_ENABLE, _FALSE);
            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _IMAGE_FIELD_ENABLE, _FALSE);
            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _FALSE);
            dwControl0 |= DRF_DEF (097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);

            pContext->hwState.kelvin.set1 (NV097_SET_TEXTURE_CONTROL0(dwHWStage), dwControl0);

            dwShaderMode[dwHWStage] = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE;
            dwRGBMapping[dwHWStage] = NV097_SET_DOT_RGBMAPPING_STAGE1_ZERO_TO_1;

        }

        dwLastTexUnitStatus = dwTexUnitStatus;

        if (pContext->hwState.dwTextureUnitStatus[dwHWStage] != dwTexUnitStatus) {
            pContext->hwState.dwTextureUnitStatus[dwHWStage] = dwTexUnitStatus;
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_TRANSFORM | KELVIN_DIRTY_FVF;
        }

    }  // for dwHWStage=0...

    if (!pContext->pCurrentPShader) {
        // send the shader RGB mapping state to the hardware
        pContext->hwState.kelvin.set1 (NV097_SET_DOT_RGBMAPPING, // No stage 0 remapping
                                       (DRF_NUM (097, _SET_DOT_RGBMAPPING, _STAGE1, dwRGBMapping[1]) |
                                        DRF_NUM (097, _SET_DOT_RGBMAPPING, _STAGE2, dwRGBMapping[2]) |
                                        DRF_NUM (097, _SET_DOT_RGBMAPPING, _STAGE3, dwRGBMapping[3])));

        // send the collective shader programs to the HW
        pContext->hwState.kelvin.set1 (NV097_SET_SHADER_STAGE_PROGRAM,
                                       (DRF_NUM (097, _SET_SHADER_STAGE_PROGRAM, _STAGE0, dwShaderMode[0]) |
                                        DRF_NUM (097, _SET_SHADER_STAGE_PROGRAM, _STAGE1, dwShaderMode[1]) |
                                        DRF_NUM (097, _SET_SHADER_STAGE_PROGRAM, _STAGE2, dwShaderMode[2]) |
                                        DRF_NUM (097, _SET_SHADER_STAGE_PROGRAM, _STAGE3, dwShaderMode[3])));

        // for DX7, dependent texture lookups always run between stages i and i+1
        pContext->hwState.kelvin.set1 (NV097_SET_SHADER_OTHER_STAGE_INPUT,
                                      (DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE1, _INSTAGE_0) |
                                       DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE2, _INSTAGE_1) |
                                       DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE3, _INSTAGE_2)));
    }

    // set dependent dirty state

    if ((pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED_MASK) != dwUserCoordsNeeded) {
        pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_USERTEXCOORDSNEEDED_MASK;
        pContext->hwState.dwStateFlags |= dwUserCoordsNeeded;
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    }

    if (dwNumTexCoordsNeeded != pContext->hwState.dwNumTexCoordsNeeded) {
        pContext->hwState.dwNumTexCoordsNeeded = dwNumTexCoordsNeeded;
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    }

    if ((pContext->hwState.dwStateFlags & KELVIN_FLAG_TEXSTATENEEDSINVMV) != dwInvMVNeeded) {
        pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_TEXSTATENEEDSINVMV;
        pContext->hwState.dwStateFlags |= dwInvMVNeeded;
        pContext->hwState.dwDirtyFlags |= dwInvMVNeeded ? KELVIN_DIRTY_TRANSFORM : 0;
    }

    if (pContext->hwState.dwTexCoordIndices != dwTexCoordIndices) {
        pContext->hwState.dwTexCoordIndices = dwTexCoordIndices;
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    }

    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_SQUASHW_MASK) == D3D_REG_SQUASHW_ENABLE) {
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
    }

    if (dwClipPlaneEnable) {
        DPF ("nvSetKelvinTextureState: exiting with unhandled clip planes");
        nvAssert (0);
        pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
    }

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvKelvinSetFogMode (PNVD3DCONTEXT pContext)
{
    float fBias, fScale;

    nvAssert (pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]);

    cacheFogValues (pContext);

    switch (pContext->fogData.dwFogTableMode) {
        case D3DFOG_NONE:   // user-supplied fog (neither table nor vertex)
            fBias = 1.0f;
            fScale = 1.0f;
            pContext->hwState.kelvin.set1 (NV097_SET_FOG_MODE, NV097_SET_FOG_MODE_V_LINEAR);
            break;
        case D3DFOG_LINEAR:
            fBias = 1.0f + pContext->fogData.fFogTableEnd * pContext->fogData.fFogTableLinearScale;
            fScale = -pContext->fogData.fFogTableLinearScale;
            pContext->hwState.kelvin.set1 (NV097_SET_FOG_MODE, NV097_SET_FOG_MODE_V_LINEAR);
            break;
        case D3DFOG_EXP:
            fBias = 1.5f;
            fScale = -pContext->fogData.fFogTableDensity * (1.0f / (2.0f * 5.5452f));
            pContext->hwState.kelvin.set1 (NV097_SET_FOG_MODE, NV097_SET_FOG_MODE_V_EXP);
            break;
        case D3DFOG_EXP2:
            fBias = 1.5f;
            fScale = -pContext->fogData.fFogTableDensity * (1.0f / (2.0f * (float)sqrt(5.5452f)));
            pContext->hwState.kelvin.set1 (NV097_SET_FOG_MODE, NV097_SET_FOG_MODE_V_EXP2);
            break;
        default:
            DPF ("uh oh. unknown fog mode");
    }

    // set fog_gen mode. note this is actually only relevant in T&L fixed mode, but we'll just set it regardless
    if (pContext->fogData.dwFogTableMode == D3DFOG_NONE) {
        // both table fog and vertex fog are disabled. we default back to regular user fog from specular alpha
        pContext->hwState.kelvin.set1 (NV097_SET_FOG_GEN_MODE, NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA);
    }
    else {
        pContext->hwState.kelvin.set1 (NV097_SET_FOG_GEN_MODE,
                                       pContext->dwRenderState[D3DRENDERSTATE_RANGEFOGENABLE] ?
                                           NV097_SET_FOG_GEN_MODE_V_RADIAL :
                                           NV097_SET_FOG_GEN_MODE_V_PLANAR);
    }

    pContext->hwState.kelvin.set3f (NV097_SET_FOG_PARAMS(0), fBias, fScale, 0.0f);

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set the specular/fog combiner

HRESULT nvSetKelvinSpecularFogCombiner (PNVD3DCONTEXT pContext)
{
    DWORD dwControl0, dwControl1;  // HW control words

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_COMBINERS_SPECFOG;

    dwControl0 = 0;

    if (pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]) {

        pContext->hwState.kelvin.set2 (NV097_SET_FOG_ENABLE,
                                       NV097_SET_FOG_ENABLE_V_TRUE,
                                       swapRGB(pContext->dwRenderState[D3DRENDERSTATE_FOGCOLOR]));

        nvKelvinSetFogMode (pContext);

        // A is fog factor, obtained from fog alpha
        dwControl0 |= (DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW0, _A_SOURCE, _REG_3) |
                       DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW0, _A_ALPHA, _TRUE));

        // B source is (textured diffuse) or (textured diffuse + specular)
        if (pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]) {
            dwControl0 |= DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_SPECLIT);
        }
        else {  // don't add specular to diffuse(textured) color
            dwControl0 |= DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_C);
        }

        // C is fog color
        dwControl0 |= DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW0, _C_SOURCE, _REG_3);

        // D isn't used

        pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_SPECULAR_FOG_CW0, dwControl0);
    }

    else { // no fog, just output diffuse (+ specular)

        pContext->hwState.kelvin.set1 (NV097_SET_FOG_ENABLE, NV097_SET_FOG_ENABLE_V_FALSE);

        if (pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]) {
            dwControl0 |= DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW0, _D_SOURCE, _REG_SPECLIT);
        }
        else {  // don't add specular to diffuse(textured) color
            dwControl0 |= DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW0, _D_SOURCE, _REG_C);
        }

        pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_SPECULAR_FOG_CW0, dwControl0);
    }

    dwControl1 = 0;

    // the final value for alpha is just taken from G
    dwControl1 |= (DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW1, _G_SOURCE, _REG_C) |
                   DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW1, _G_ALPHA, _TRUE));

    dwControl1 |= DRF_DEF (097, _SET_COMBINER_SPECULAR_FOG_CW1, _SPECULAR_CLAMP, _TRUE);

    pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_SPECULAR_FOG_CW1, dwControl1);

#ifdef ALPHA_CULL
    //cachec off spec/fog control words
    pContext->hwState.specfog_cw[0] = dwControl0;
    pContext->hwState.specfog_cw[1] = dwControl1;

    if(pContext->dwEarlyCopyStrategy && pContext->hwState.alphacull_mode >= 2)
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_MISC_STATE;
#endif

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set kelvin vertex shaders

HRESULT nvSetKelvinVertexShader (PNVD3DCONTEXT pContext)
{
#ifdef PRECOMPILED_PASSTHROUGH
    KELVIN_MICRO_PROGRAM *pProgram;
#else
    KELVIN_MICRO_PROGRAM program;
    char *pProgram;
#endif

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_VERTEX_SHADER;

    nvAssert(pContext->pCurrentVShader);

    if ((pContext->dwDXAppVersion >= 0x800) && (pContext->pCurrentVShader->hasProgram())) {

        // load user's program
        CVertexShader* pVertexShader = pContext->pCurrentVShader;

        if (pVertexShader->m_ProgramOutput.residentNumInstructions) {

#ifdef KPFS
            pDriverData->nvKelvinPM.download(pContext,pVertexShader);
#else

            pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_EXECUTION_MODE,
                                           (DRF_DEF (097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |
                                            DRF_DEF (097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _USER)));
            pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN,
                                           DRF_DEF (097, _SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, _V, _READ_ONLY));

            pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_START, 0);
            nvKelvinDownloadProgram (pContext, &pVertexShader->m_ProgramOutput, 0);
            nvKelvinDownloadConstants (pContext);
#endif

        }

    }

    else if (pContext->hwState.dwStateFlags & KELVIN_FLAG_PASSTHROUGHMODE) {
        // choose the appropriate passthru program
        if (pContext->fogData.dwFogTableMode == D3DFOG_NONE) {
            // fog is disabled or both table fog and vertex fog are disabled.
            // we default back to regular user fog from specular alpha
#ifdef PRECOMPILED_PASSTHROUGH
            pProgram = (KELVIN_MICRO_PROGRAM*)(kmpPassthruProgramSpecFog);
#else
            pProgram = szPassthruProgramSpecFog;
#endif
        }
        else {
            // we're doing vertex or table fog
            nvAssert(pContext->hwState.dwFogSource); // assert that fog source has been set
            if (pContext->hwState.dwFogSource == CHardwareState::FOG_SOURCE_Z) {
                // use z for fog depth
#ifdef PRECOMPILED_PASSTHROUGH
                pProgram = (KELVIN_MICRO_PROGRAM*)(kmpPassthruProgramZFog);
#else
                pProgram = szPassthruProgramZFog;
#endif
            }
            else {
                // use w for fog depth
#ifdef PRECOMPILED_PASSTHROUGH
                pProgram = (KELVIN_MICRO_PROGRAM*)(kmpPassthruProgramWFog);
#else
                pProgram = szPassthruProgramWFog;
#endif
            }
        }
        DWORD temp = sizeof(KELVIN_MICRO_PROGRAM);
        // passthrough program
        pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_EXECUTION_MODE,
                                       (DRF_DEF (097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |
                                        DRF_DEF (097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV)));
        pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN,
                                       DRF_DEF (097, _SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, _V, _READ_ONLY));
#ifdef PRECOMPILED_PASSTHROUGH

    #ifdef KPFS
        pDriverData->nvKelvinPM.download_special(pContext,pProgram,0xFFFFFFFF);
    #else
        nvKelvinDownloadProgram (pContext, pProgram,0);
    #endif

#else
        nvKelvinInitProgram (&program);
        nvKelvinParseProgram (&program, pProgram);

    #ifdef KPFS
        pDriverData->nvKelvinPM.download_special(pContext,pProgram,0xFFFFFFFF);
    #else
        nvKelvinDownloadProgram (pContext, &program,0);
    #endif

#endif

#ifndef KPFS
        // the starting address is just 0
        pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_START, 0);
#endif
    }

    else {
        // just use fixed mode
        pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_EXECUTION_MODE,
                                       (DRF_DEF (097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _FIXED) |
                                        DRF_DEF (097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV)));
#ifdef KPFS
        pDriverData->nvKelvinPM.dirty();
#endif

    }

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set texture transformation matrices

HRESULT nvSetKelvinTextureTransform (PNVD3DCONTEXT pContext)
{
    BOOL       bEnable;
    DWORD      dwHWStage, dwD3DStage;
    DWORD      dwInCount, dwOutCount;
    DWORD      dwFlags, dwTexUnitStatus;
    DWORD      dwMatrixName, dwMatrixType;
    D3DMATRIX *pMatrix;

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_TEXTURE_TRANSFORM;

    for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {

        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
        dwTexUnitStatus = pContext->hwState.dwTextureUnitStatus[dwHWStage];

        if ((dwD3DStage == KELVIN_UNUSED) || (dwTexUnitStatus == KELVIN_TEXUNITSTATUS_CLIP)) {

            bEnable = FALSE;

        }

        else {

            nvAssert (dwTexUnitStatus & (KELVIN_TEXUNITSTATUS_USER     |
                                         KELVIN_TEXUNITSTATUS_BUMPMAPA |
                                         KELVIN_TEXUNITSTATUS_BUMPMAPB |
                                         KELVIN_TEXUNITSTATUS_BUMPMAPC |
                                         KELVIN_TEXUNITSTATUS_BUMPMAPD));
            nvAssert (pContext->pCurrentPShader || (pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE));

            dwFlags = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];

            // the number of coordinates to come out of the transformation, including the projective coordinate
            // (== the number of d3d-style matrix columns with non-zero elements)
            dwOutCount = dwFlags & 0xff; // BUGBUG there should be a #define for this. pester MS
            bEnable = (dwOutCount != D3DTTFF_DISABLE);

        }

        if (!bEnable) {

            pContext->hwState.kelvin.set1 (NV097_SET_TEXTURE_MATRIX_ENABLE(dwHWStage),
                                           DRF_DEF (097, _SET_TEXTURE_MATRIX_ENABLE, _V, _FALSE));

        }

        else {

            pContext->hwState.kelvin.set1 (NV097_SET_TEXTURE_MATRIX_ENABLE(dwHWStage),
                                           DRF_DEF (097, _SET_TEXTURE_MATRIX_ENABLE, _V, _TRUE));

            pMatrix = &(pContext->tssState[dwD3DStage].mTexTransformMatrix);

            // the number of coordinates incoming from the vertex streams or texgen (not counting q).
            // this is typically the number of d3d-style matrix rows with non-zero elements, though
            // there may be dwInCount+1 rows, the last of which references the q=1.0 that d3d assumes
            // to be automatically postpended to the incoming coordinates.
            DWORD dwTexGen  = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff0000;
            DWORD dwTCIndex = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXCOORDINDEX] & 0x0000ffff;
            DWORD dwVAIndex = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            if (dwTexGen) {
                dwInCount = 3;
            }
            else {
                // if index invalid, just use 0,0
                dwInCount = (pContext->pCurrentVShader->bVAExists(dwVAIndex)) ?
                            (pContext->pCurrentVShader->getVASize(dwVAIndex)) :
                            2*sizeof(DWORD);
                dwInCount >>= 2;
            }

            // nv097 texture matrix macros are less than ideal
            dwMatrixName = NV097_SET_TEXTURE_MATRIX0(0) + 0x40*dwHWStage;

            // calculate an arbitrary identifier
            dwMatrixType = (dwInCount << 8) | (dwOutCount << 4) | ((dwFlags & D3DTTFF_PROJECTED) ? 1 : 0);

            // the way we emit the matrix varies widely because d3d is dumb.
            // A. when there are two incoming coordinates, we must move the 3rd matrix row to the fourth, because
            //    MS expects the appended q to sit in the third slot, whereas in the HW, it sits in the 4th.
            // B. when the matrix is projective and there are only 3 outgoing coordinates (including q)
            //    we must shift the value in the third outgoing slot (where MS puts q) to the 4th slot (where
            //    the HW wants it) by moving the 3rd column to the 4th
            // C. when the matrix is non-projective, it becomes our job to override the relevant row of the
            //    matrix, forcing the outgoing q to 1.0
            // finally, when all is said and done, the matrix must be transposed to change it from backward
            // d3d style to real opengl style.
            switch (dwMatrixType) {

                case 0x220:
                    // (s,t,1.0) in, (s,t,1.0) out
                    pContext->hwState.kelvin.setTextureTransformMatrixType220 (dwMatrixName, pMatrix);
                    break;
                case 0x230:
                    // (s,t,1.0) in, (s,t,r,1.0) out
                    pContext->hwState.kelvin.setTextureTransformMatrixType230 (dwMatrixName, pMatrix);
                    break;
                case 0x231:
                    // (s,t,1.0) in, (s,t,q) out
                    pContext->hwState.kelvin.setTextureTransformMatrixType231 (dwMatrixName, pMatrix);
                    break;
                case 0x241:
                    // (s,t,1.0) in, (s,t,r,q) out
                    pContext->hwState.kelvin.setTextureTransformMatrixType241 (dwMatrixName, pMatrix);
                    break;
                case 0x320:
                    // (s,t,r,1.0) in, (s,t,1.0) out
                    pContext->hwState.kelvin.setTextureTransformMatrixType320 (dwMatrixName, pMatrix);
                    break;
                case 0x330:
                    // (s,t,r,1.0) in, (s,t,r,1.0) out
                    pContext->hwState.kelvin.setTextureTransformMatrixType330 (dwMatrixName, pMatrix);
                    break;
                case 0x331:
                    // (s,t,r,1.0) in, (s,t,q) out
                    pContext->hwState.kelvin.setTextureTransformMatrixType331 (dwMatrixName, pMatrix);
                    break;
                case 0x341:
                    // (s,t,r,1.0) in, (s,t,r,q) out
                    pContext->hwState.kelvin.setTextureTransformMatrixType341 (dwMatrixName, pMatrix);
                    break;
                default:
                    DPF ("unhandled texture transform = 0x%04x", dwMatrixType);
                    dbgD3DError();
                    break;

            }  // switch

        }  // else enable

    }  // for dwHWStage=0...

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set kelvin class specular parameters

HRESULT nvKelvinSetSpecularParams (PNVD3DCONTEXT pContext)
{
    float L, M, N, L1, M1, N1, power;

    nvAssert ((pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]) ||
              (pContext->hwState.dwStateFlags & KELVIN_FLAG_COMBINERSNEEDSPECULAR));

    power = pContext->Material.dvPower;
    explut (power, &L, &M);
    N = 1.f+L-M;
    explut (0.5f*power, &L1, &M1);
    N1 = 1.f+L1-M1;
    pContext->hwState.kelvin.set3f (NV097_SET_SPECULAR_PARAMS(0),L, M, N);
    pContext->hwState.kelvin.set3f (NV097_SET_SPECULAR_PARAMS(3),L1,M1,N1);
    //pContext->hwState.kelvin.set3f (NV097_SET_BACK_SPECULAR_PARAMS(0),L, M, N);
    //pContext->hwState.kelvin.set3f (NV097_SET_BACK_SPECULAR_PARAMS(3),L1,M1,N1);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set diffuse, specular, and ambient colors for a particular light
// input:
//      - dwLightID = light index [0...7]
//      - pLight7 = pointer to light data
//      - dwCmArg   = color material flags

HRESULT nvKelvinDumpLightColors (PNVD3DCONTEXT pContext, DWORD dwLightID, LPD3DLIGHT7 pLight7, DWORD dwCmArg)
{
    D3DVALUE dvAmbientR, dvAmbientG, dvAmbientB;
    D3DVALUE dvDiffuseR, dvDiffuseG, dvDiffuseB;
    D3DVALUE dvSpecularR, dvSpecularG, dvSpecularB;

    nvAssert ((dwLightID >=0) && (dwLightID <= 7));

    dvAmbientR = pLight7->dcvAmbient.dvR;
    dvAmbientG = pLight7->dcvAmbient.dvG;
    dvAmbientB = pLight7->dcvAmbient.dvB;
    if (DRF_VAL (097, _SET_COLOR_MATERIAL, _AMBIENT_MATERIAL, dwCmArg) == NV097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL_DISABLE) {
        // use material ambient color
        dvAmbientR *= pContext->Material.dcvAmbient.dvR;
        dvAmbientG *= pContext->Material.dcvAmbient.dvG;
        dvAmbientB *= pContext->Material.dcvAmbient.dvB;
    }

    dvDiffuseR = pLight7->dcvDiffuse.dvR;
    dvDiffuseG = pLight7->dcvDiffuse.dvG;
    dvDiffuseB = pLight7->dcvDiffuse.dvB;
    if (DRF_VAL (097, _SET_COLOR_MATERIAL, _DIFF_MATERIAL, dwCmArg) == NV097_SET_COLOR_MATERIAL_DIFF_MATERIAL_DISABLE) {
        // use material diffuse color
        dvDiffuseR *= pContext->Material.dcvDiffuse.dvR;
        dvDiffuseG *= pContext->Material.dcvDiffuse.dvG;
        dvDiffuseB *= pContext->Material.dcvDiffuse.dvB;
    }

    dvSpecularR = pLight7->dcvSpecular.dvR;
    dvSpecularG = pLight7->dcvSpecular.dvG;
    dvSpecularB = pLight7->dcvSpecular.dvB;
    if (DRF_VAL (097, _SET_COLOR_MATERIAL, _SPECULAR_MATERIAL, dwCmArg) == NV097_SET_COLOR_MATERIAL_SPECULAR_MATERIAL_DISABLE) {
        // follow material specular color
        dvSpecularR *= pContext->Material.dcvSpecular.dvR;
        dvSpecularG *= pContext->Material.dcvSpecular.dvG;
        dvSpecularB *= pContext->Material.dcvSpecular.dvB;
    }

    pContext->hwState.kelvin.set3f (NV097_SET_LIGHT_AMBIENT_COLOR(dwLightID,0), dvAmbientR, dvAmbientG, dvAmbientB);  // ambient
    pContext->hwState.kelvin.set3f (NV097_SET_LIGHT_DIFFUSE_COLOR(dwLightID,0), dvDiffuseR, dvDiffuseG, dvDiffuseB);  // diffuse
    pContext->hwState.kelvin.set3f (NV097_SET_LIGHT_SPECULAR_COLOR(dwLightID,0),dvSpecularR,dvSpecularG,dvSpecularB); // specular

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set position and attenuation of a particular light
// input:
//      - dwLightID = light index [0...7]
//      - pLight7 = pointer to light data

HRESULT nvKelvinDumpLocalLightData (PNVD3DCONTEXT pContext, int dwLightID, LPD3DLIGHT7 pLight7)
{
    D3DVECTOR pos1;

    nvAssert ((dwLightID >=0) && (dwLightID <= 7));

    // We don't normalize the position
    XformBy4x3 (&pos1, &(pLight7->dvPosition), 1.f, &(pContext->xfmView));

    pContext->hwState.kelvin.set3f (NV097_SET_LIGHT_LOCAL_POSITION(dwLightID,0), pos1.dvX, pos1.dvY, pos1.dvZ);

    nvAssert ((DWORD_FROM_FLOAT(pLight7->dvAttenuation0) != 0) ||
              (DWORD_FROM_FLOAT(pLight7->dvAttenuation1) != 0) ||
              (DWORD_FROM_FLOAT(pLight7->dvAttenuation2) != 0));

    pContext->hwState.kelvin.set3f (NV097_SET_LIGHT_LOCAL_ATTENUATION(dwLightID,0),
                                    pLight7->dvAttenuation0,
                                    pLight7->dvAttenuation1,
                                    pLight7->dvAttenuation2);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

DWORD getKelvinColorMaterialFlags (PNVD3DCONTEXT pContext)
{
    DWORD dwArgs;

    // start out with everything disabled
    dwArgs = 0;

    // if COLORVERTEX isn't set just return now
    if (!pContext->dwRenderState[D3DRENDERSTATE_COLORVERTEX]) {
        return (dwArgs);
    }

    switch (pContext->dwRenderState[D3DRENDERSTATE_AMBIENTMATERIALSOURCE]) {
        case D3DMCS_MATERIAL:
            break;
        case D3DMCS_COLOR1:  // diffuse
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
                dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _AMBIENT_MATERIAL, _DIFFUSE_VTX_COLOR);
                //dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _BACK_AMBIENT_MATERIAL, _DIFF_VTX_COLOR);
            }
            break;
        case D3DMCS_COLOR2:  // specular
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _AMBIENT_MATERIAL, _SPECULAR_VTX_COLOR);
                //dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _BACK_AMBIENT_MATERIAL, _SPEC_VTX_COLOR);
            }
            break;
        default:
            DPF ("unknown AMBIENTMATERIALSOURCE");
            break;
    }

    switch (pContext->dwRenderState[D3DRENDERSTATE_DIFFUSEMATERIALSOURCE]) {
        case D3DMCS_MATERIAL:
            break;
        case D3DMCS_COLOR1:  // diffuse
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
                dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _DIFF_MATERIAL, _DIFFUSE_VTX_COLOR);
                //dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _BACK_DIFF_MATERIAL, _DIFF_VTX_COLOR);
            }
            break;
        case D3DMCS_COLOR2:  // specular
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _DIFF_MATERIAL, _SPECULAR_VTX_COLOR);
                //dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _BACK_DIFF_MATERIAL, _SPEC_VTX_COLOR);
            }
            break;
        default:
            DPF ("unknown DIFFUSEMATERIALSOURCE");
            break;
    }

    switch (pContext->dwRenderState[D3DRENDERSTATE_SPECULARMATERIALSOURCE]) {
        case D3DMCS_MATERIAL:
            break;
        case D3DMCS_COLOR1:  // diffuse
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
                dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _SPECULAR_MATERIAL, _DIFFUSE_VTX_COLOR);
                //dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _BACK_SPECULAR_MATERIAL, _DIFF_VTX_COLOR);
            }
            break;
        case D3DMCS_COLOR2:  // specular
            // inherit material specular from vertex specular
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _SPECULAR_MATERIAL, _SPECULAR_VTX_COLOR);
                //dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _BACK_SPECULAR_MATERIAL, _SPEC_VTX_COLOR);
            }
            break;
        default:
            DPF ("unknown SPECULARMATERIALSOURCE");
            break;
    }

    switch (pContext->dwRenderState[D3DRENDERSTATE_EMISSIVEMATERIALSOURCE]) {
        case D3DMCS_MATERIAL:
            break;
        case D3DMCS_COLOR1:  // diffuse
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
                dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _EMISSIVE_MATERIAL, _DIFFUSE_VTX_COLOR);
                //dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _BACK_EMISSIVE_MATERIAL, _DIFF_VTX_COLOR);
            }
            break;
        case D3DMCS_COLOR2:  // specular
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _EMISSIVE_MATERIAL, _SPECULAR_VTX_COLOR);
                //dwArgs |= DRF_DEF (097, _SET_COLOR_MATERIAL, _BACK_EMISSIVE_MATERIAL, _SPEC_VTX_COLOR);
            }
            break;
        default:
            DPF ("unknown EMISSIVEMATERIALSOURCE");
            break;
    }

    return (dwArgs);
}

//---------------------------------------------------------------------------

// set lighting state

HRESULT nvSetKelvinLights (PNVD3DCONTEXT pContext)
{
    DWORD         dwColorMaterialArgs;
    DWORD         dwControl = 0;
    DWORD         dwLightNum;
    DWORD         dwMask = 0;
    D3DCOLORVALUE dcvAmbient, dcvEmissive;
    D3DVECTOR     eyeDirection;
    D3DVECTOR     dir, hv;          // light directions
    pnvLight      pLight;
    LPD3DLIGHT7   pLight7;
    BOOL          bSpecularNeeded;
    DWORD         dwInvMVNeeded;

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_LIGHTS;

    // specular enable
    bSpecularNeeded = ((pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]) ||
                       (pContext->hwState.dwStateFlags & KELVIN_FLAG_COMBINERSNEEDSPECULAR)) ? TRUE : FALSE;

    // disable hardware lighting if app has disabled it or if we're in passthrough mode
    // (yes, stupid apps will try to enable lighting without giving us normals, etc...)
    if (!GET_LIGHTING_STATE(pContext) || (pContext->hwState.dwStateFlags & KELVIN_FLAG_PASSTHROUGHMODE)) {

        // disable lighting
        pContext->hwState.kelvin.set1 (NV097_SET_LIGHTING_ENABLE, NV097_SET_LIGHTING_ENABLE_V_FALSE);

        dwInvMVNeeded = 0;

        pContext->hwState.kelvin.set1 (NV097_SET_SPECULAR_ENABLE, DRF_NUM (097, _SET_SPECULAR_ENABLE, _V, bSpecularNeeded));

        dwControl |= (DRF_DEF (097, _SET_LIGHT_CONTROL, _LOCALEYE, _FALSE) |
                      DRF_DEF (097, _SET_LIGHT_CONTROL, _SOUT, _ZERO_OUT)  |            // set alpha component to 1.0 (method is horribly misnamed)
                      DRF_DEF (097, _SET_LIGHT_CONTROL, _SEPARATE_SPECULAR_EN, _TRUE)); // this is a don't-care

        pContext->hwState.kelvin.set1 (NV097_SET_LIGHT_CONTROL, dwControl);

        // set default diffuse and specular
        pContext->hwState.kelvin.set1 (NV097_SET_DIFFUSE_COLOR4UB,  0xFFFFFFFF);  // white
        pContext->hwState.kelvin.set1 (NV097_SET_SPECULAR_COLOR4UB, 0);           // black

    }

    else {

        // we shouldn't be in passthru mode if we're really going to do lighting
        // (moreover we can't be, b/c we won't have calculated xfmInvModelView which
        // is needed for spot lights)
        nvAssert (!(pContext->hwState.dwStateFlags & KELVIN_FLAG_PASSTHROUGHMODE));

        // enable lighting
        pContext->hwState.kelvin.set1 (NV097_SET_LIGHTING_ENABLE, NV097_SET_LIGHTING_ENABLE_V_TRUE);

        dwInvMVNeeded = KELVIN_FLAG_LIGHTINGNEEDSINVMV;

        // this should always be TRUE in T&L mode. see table of kelvin's perverse specular behavior
        pContext->hwState.kelvin.set1 (NV097_SET_SPECULAR_ENABLE, DRF_DEF (097, _SET_SPECULAR_ENABLE, _V, _TRUE));

        // set specular parameters if needed
        if (bSpecularNeeded) {
            nvKelvinSetSpecularParams (pContext);
        }

        // use local viewer only if we really need to, since it's slow
        if ((pContext->dwRenderState[D3DRENDERSTATE_LOCALVIEWER]) &&
            (pContext->lighting.pActiveLights != NULL) &&
            (bSpecularNeeded)) {
            dwControl |= DRF_DEF (097, _SET_LIGHT_CONTROL, _LOCALEYE, _TRUE);
        }
        else {
            dwControl |= DRF_DEF (097, _SET_LIGHT_CONTROL, _LOCALEYE, _FALSE);
        }

        dwControl |= (DRF_DEF (097, _SET_LIGHT_CONTROL, _SOUT, _ZERO_OUT) |             // set alpha component to 1.0 (method is horribly misnamed)
                      DRF_DEF (097, _SET_LIGHT_CONTROL, _SEPARATE_SPECULAR_EN, _TRUE)); // d3d never lumps specular in with diffuse

        pContext->hwState.kelvin.set1 (NV097_SET_LIGHT_CONTROL, dwControl);

        // material properties
        dwColorMaterialArgs = getKelvinColorMaterialFlags (pContext);
        pContext->hwState.kelvin.set1 (NV097_SET_COLOR_MATERIAL, dwColorMaterialArgs);

        // in kelvin, the color is initialized to SCENE_AMBIENT, C0, or C1. this is followed by an
        // optional accumulation of C0 or C1 times MATERIAL_EMISSION. both the initialization and
        // accumulation are controlled by the color material bits, so these determine the (sometimes
        // counter-intuitive) values to which we initialize SCENE_AMBIENT and MATERIAL_EMISSION.

        if (dwColorMaterialArgs & (DRF_DEF (097, _SET_COLOR_MATERIAL, _AMBIENT_MATERIAL, _DIFFUSE_VTX_COLOR) |
                                   DRF_DEF (097, _SET_COLOR_MATERIAL, _AMBIENT_MATERIAL, _SPECULAR_VTX_COLOR))) {
            // ambient is being pulled from the vertex
            dcvAmbient.dvR  = pContext->Material.dcvEmissive.dvR;
            dcvAmbient.dvG  = pContext->Material.dcvEmissive.dvG;
            dcvAmbient.dvB  = pContext->Material.dcvEmissive.dvB;
            dcvEmissive.dvR = pContext->lighting.dcvAmbient.dvR;
            dcvEmissive.dvG = pContext->lighting.dcvAmbient.dvG;
            dcvEmissive.dvB = pContext->lighting.dcvAmbient.dvB;
        }
        else if (dwColorMaterialArgs & (DRF_DEF (097, _SET_COLOR_MATERIAL, _EMISSIVE_MATERIAL, _DIFFUSE_VTX_COLOR) |
                                        DRF_DEF (097, _SET_COLOR_MATERIAL, _EMISSIVE_MATERIAL, _SPECULAR_VTX_COLOR))) {
            // emissive is being pulled from the vertex, and ambient is not
            dcvAmbient.dvR  = pContext->lighting.dcvAmbient.dvR * pContext->Material.dcvAmbient.dvR;
            dcvAmbient.dvG  = pContext->lighting.dcvAmbient.dvG * pContext->Material.dcvAmbient.dvG;
            dcvAmbient.dvB  = pContext->lighting.dcvAmbient.dvB * pContext->Material.dcvAmbient.dvB;
            dcvEmissive.dvR = 1.0;
            dcvEmissive.dvG = 1.0;
            dcvEmissive.dvB = 1.0;
        }
        else {
            // neither ambient nor emissive is being pulled from the vertex
            dcvAmbient.dvR  = (pContext->lighting.dcvAmbient.dvR * pContext->Material.dcvAmbient.dvR) + pContext->Material.dcvEmissive.dvR;
            dcvAmbient.dvG  = (pContext->lighting.dcvAmbient.dvG * pContext->Material.dcvAmbient.dvG) + pContext->Material.dcvEmissive.dvG;
            dcvAmbient.dvB  = (pContext->lighting.dcvAmbient.dvB * pContext->Material.dcvAmbient.dvB) + pContext->Material.dcvEmissive.dvB;
            dcvEmissive.dvR = 0.0;
            dcvEmissive.dvG = 0.0;
            dcvEmissive.dvB = 0.0;
        }

        pContext->hwState.kelvin.set3f (NV097_SET_SCENE_AMBIENT_COLOR(0), dcvAmbient.dvR,  dcvAmbient.dvG,  dcvAmbient.dvB);
        pContext->hwState.kelvin.set3f (NV097_SET_MATERIAL_EMISSION(0),   dcvEmissive.dvR, dcvEmissive.dvG, dcvEmissive.dvB);

        // pull material alpha from the diffuse component
        pContext->hwState.kelvin.set1f (NV097_SET_MATERIAL_ALPHA, pContext->Material.dcvDiffuse.dvA);
        //pContext->hwState.kelvin.set1f (NV097_SET_BACK_MATERIAL_ALPHA, pContext->Material.dcvDiffuse.dvA);

        // Set the eye position and direction
        eyeDirection = vectorZero;
        eyeDirection.dvZ = -1.0f;

        // Download transform and lighting methods. Pack lights and send them

        dwMask = 0;
        dwLightNum = 0;

        for (pLight=pContext->lighting.pActiveLights; pLight!=NULL; pLight=pLight->pNext) {

            if (dwLightNum == 8) break; // too many

            pLight7 = &(pLight->Light7);
            dbgDisplayLight (pLight7);

            switch (pLight7->dltType) {

                case D3DLIGHT_POINT:

                    dwMask |= NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_LOCAL << (dwLightNum << 1);

                    nvKelvinDumpLightColors (pContext, dwLightNum, pLight7, dwColorMaterialArgs);
                    pContext->hwState.kelvin.set1f (NV097_SET_LIGHT_LOCAL_RANGE(dwLightNum), pLight7->dvRange);
                    nvKelvinDumpLocalLightData (pContext, dwLightNum, pLight7);

                    break;

                case D3DLIGHT_SPOT:

                    dwMask |= NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_SPOT << (dwLightNum << 1);

                    nvKelvinDumpLightColors (pContext, dwLightNum, pLight7, dwColorMaterialArgs);
                    pContext->hwState.kelvin.set1f (NV097_SET_LIGHT_LOCAL_RANGE(dwLightNum), pLight7->dvRange);
                    nvKelvinDumpLocalLightData (pContext, dwLightNum, pLight7);

                    // Transform the spotlight direction vector back
                    XformBy4x3 (&dir, &(pLight->direction), 0.f, &(pContext->xfmView));
                    // unless the view matrix has non-uniform scales in it, this is not necessary
                    // For now, we take the easy way out though.
                    NormalizeVector3 (&dir);

                    ScaleVector3 (&dir, &dir, pLight->dvScale);
                    pContext->hwState.kelvin.set3f (NV097_SET_LIGHT_SPOT_FALLOFF(dwLightNum,0),
                                                    pLight->falloffParams.L,
                                                    pLight->falloffParams.M,
                                                    pLight->falloffParams.N);
                    pContext->hwState.kelvin.set4f (NV097_SET_LIGHT_SPOT_DIRECTION(dwLightNum,0),
                                                    dir.dvX, dir.dvY, dir.dvZ, pLight->dvW);

                    break;

                case D3DLIGHT_DIRECTIONAL:

                    dwMask |= NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_INFINITE << (dwLightNum << 1);

                    nvKelvinDumpLightColors (pContext, dwLightNum, pLight7, dwColorMaterialArgs);
                    pContext->hwState.kelvin.set1f (NV097_SET_LIGHT_LOCAL_RANGE(dwLightNum), 1e30f);

                    // Transform the spotlight direction vector back
                    XformBy4x3 (&dir, &(pLight->direction), 0.f, &(pContext->xfmView));
                    // unless the view matrix has non-uniform scales in it, this is not necessary
                    // For now, we take the easy way out though.
                    NormalizeVector3 (&dir);

                    AddVectors3 (&hv, &dir, &eyeDirection);
                    NormalizeVector3 (&hv);
                    pContext->hwState.kelvin.set3f (NV097_SET_LIGHT_INFINITE_HALF_VECTOR(dwLightNum,0),
                                                    hv.dvX, hv.dvY, hv.dvZ);
                    pContext->hwState.kelvin.set3f (NV097_SET_LIGHT_INFINITE_DIRECTION(dwLightNum,0),
                                                    dir.dvX, dir.dvY, dir.dvZ);

                    break;

                default:

                    DPF ("unknown light type in nvSetKelvinLights");
                    dbgD3DError();
                    break;

            } // switch

            dwLightNum++;

        } // for all lights

        // Tell HW which lights are active
        pContext->hwState.kelvin.set1 (NV097_SET_LIGHT_ENABLE_MASK, dwMask);

    }

    if ((pContext->hwState.dwStateFlags & KELVIN_FLAG_LIGHTINGNEEDSINVMV) != dwInvMVNeeded) {
        pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_LIGHTINGNEEDSINVMV;
        pContext->hwState.dwStateFlags |= dwInvMVNeeded;
        pContext->hwState.dwDirtyFlags |= dwInvMVNeeded ? KELVIN_DIRTY_TRANSFORM : 0;
    }

#ifdef KELVIN_ILC
    pContext->hwState.SuperTri.setNumLights(dwLightNum);
#endif

    // disable lights after setting them up (i.e minimum impact)
#ifdef NV_NULL_LIGHTS_DRIVER
    pContext->hwState.kelvin.set (NV097_SET_SPECULAR_ENABLE, DRF_NUM (097, _SET_SPECULAR_ENABLE, _V, FALSE));
    pContext->hwState.kelvin.set (NV097_SET_LIGHTING_ENABLE, NV097_SET_LIGHTING_ENABLE_V_FALSE);
#endif

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set geometric transformation matrices

HRESULT nvSetKelvinTransform (PNVD3DCONTEXT pContext)
{
    D3DMATRIX        mMV0, mMVBlend, mCTM, mInvMV, mTemp;
    DWORD            dwClipH, dwClipV;
    float            fm11, fm22, fm33, fm41, fm42, fm43, fm44;
    float            fxTrans, fyTrans;
    SurfaceViewport  viewport;

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_TRANSFORM;

    viewport = pContext->surfaceViewport;

    if (pContext->kelvinAA.IsEnabled()) {
        viewport.clipHorizontal.wX      = (DWORD)(0.5f + viewport.clipHorizontal.wX     * pContext->aa.fWidthAmplifier);
        viewport.clipHorizontal.wWidth  = (DWORD)(0.5f + viewport.clipHorizontal.wWidth * pContext->aa.fWidthAmplifier);
        viewport.clipVertical.wY        = (DWORD)(0.5f + viewport.clipVertical.wY       * pContext->aa.fHeightAmplifier);
        viewport.clipVertical.wHeight   = (DWORD)(0.5f + viewport.clipVertical.wHeight  * pContext->aa.fHeightAmplifier);
    }

    if (pContext->pCurrentVShader->hasProgram()) {

        fxTrans = (float)(viewport.clipHorizontal.wX) + KELVIN_BORDER;
        fyTrans = (float)(viewport.clipVertical.wY) + KELVIN_BORDER;

        fm11 = 0.5f * (float)(viewport.clipHorizontal.wWidth);
        fm41 = fm11;

        fm22 = -0.5f * (float)(viewport.clipVertical.wHeight);
        fm42 = -fm22;

        fm33 = pContext->hwState.dvZScale * (pContext->surfaceViewport.dvMaxZ - pContext->surfaceViewport.dvMinZ);
        fm43 = pContext->hwState.dvZScale * pContext->surfaceViewport.dvMinZ;

        // these end up in privileged registers c[58] and c[59], from which they are read by the
        // viewport transformation code automatically tacked onto the end of every user program
        pContext->hwState.kelvin.set4f (NV097_SET_VIEWPORT_SCALE(0), fm11, fm22, fm33, 0.0f);
        pContext->hwState.kelvin.set4f (NV097_SET_VIEWPORT_OFFSET(0), fm41 + fxTrans, fm42 + fyTrans, fm43, 0.0f);

        // set depth clipping
        pContext->hwState.kelvin.set2f (NV097_SET_CLIP_MIN, 0.0f, pContext->hwState.dvZScale);

    }

    else if (pContext->hwState.dwStateFlags & KELVIN_FLAG_PASSTHROUGHMODE) {

        // calculate scales and biases for passthru mode

        D3DVECTOR4 vScale = {1.0f, 1.0f, 1.0f, 1.0f};
        D3DVECTOR4 vBias  = {KELVIN_BORDER, KELVIN_BORDER, 0.0f, 0.0f};

        if (getDC()->nvD3DRegistryData.regfZBias) {
            vScale.dvZ = pContext->hwState.dvZScale / (1.f + getDC()->nvD3DRegistryData.regfZBias);
            vBias.dvZ  = getDC()->nvD3DRegistryData.regfZBias * vScale.dvZ;
        }
        else {
            vScale.dvZ = pContext->hwState.dvZScale;
        }

        // Because NV20 uses a weird sampling pattern, we offset by approximately a pixel here, then clamp on
        // the right edge of the superbuffer.  This fixes apps that clip their non-T&L geometry
        // to the border of the *aliased* samples on screen (not the antialiased subsamples).
        if (pContext->kelvinAA.IsMultisampling()) {
            vBias.dvX += pContext->kelvinAA.GetXOffset();
            vBias.dvY += pContext->kelvinAA.GetYOffset();
        }

        if ( ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_SQUASHW_MASK) == D3D_REG_SQUASHW_ENABLE)
             &&
             ( ((pContext->hwState.dwTextureUnitStatus[0] == KELVIN_TEXUNITSTATUS_IDLE) &&
                (pContext->hwState.dwTextureUnitStatus[1] == KELVIN_TEXUNITSTATUS_IDLE))
               ||
               (pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE] == FALSE)
             )
           )
        {
            vScale.dvW = 0.0f;
        }
        else  {
            vScale.dvW = (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW) ?
                         pContext->surfaceViewport.dvInvWFar * pContext->hwState.dvZScale :
                         1.0f;
        }

        vScale.dvX *= pContext->aa.fWidthAmplifier;
        vScale.dvY *= pContext->aa.fHeightAmplifier;

        // these end up in privileged registers c[58] and c[59], from which they are read by the passthru-mode programs
        pContext->hwState.kelvin.set4f (NV097_SET_VIEWPORT_SCALE(0), vScale.dvX, vScale.dvY, vScale.dvZ, vScale.dvW);
        pContext->hwState.kelvin.set4f (NV097_SET_VIEWPORT_OFFSET(0), vBias.dvX, vBias.dvY, vBias.dvZ, vBias.dvW);

        // set depth clipping
        // if we're w-buffering in passthru mode, do the best we can to disable the HW's inevitable
        // z clipping because dumb apps may give us bad z's thinking they're unneeded.
        // otherwise, just clip z to the real range of the z-buffer
        // BUGBUG we can explicitly ignore z on nv20. no need for this hack
        pContext->hwState.kelvin.set2f (NV097_SET_CLIP_MIN, 0.0f, (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW) ?
                                                                  KELVIN_Z_SCALE24 :
                                                                  pContext->hwState.dvZScale);

    }

    else {

        float fMinZ = pContext->surfaceViewport.dvMinZ;
        float fMaxZ = pContext->surfaceViewport.dvMaxZ;

        if (fMinZ == fMaxZ) {
            // the whole universe has been packed down to a single plane.
            // this is an LSB nightmare, so back it off a bit.
            fMinZ = max (0.0f, fMinZ-EPSILON);
            fMaxZ = min (1.0f, fMaxZ+EPSILON);
        }

        // disable CT
        if (pContext->pZetaBuffer) {
            pContext->pZetaBuffer->getWrapper()->tagCTZE();
            pContext->pZetaBuffer->getWrapper()->disableCT();
        }

        // process viewport dependencies
        fxTrans = (float)(viewport.clipHorizontal.wX) + KELVIN_BORDER;
        fyTrans = (float)(viewport.clipVertical.wY) + KELVIN_BORDER;

        pContext->hwState.kelvin.set4f (NV097_SET_VIEWPORT_OFFSET(0), fxTrans, fyTrans, 0.0f, 0.0f);

        // this is based on clipX = -1.0, clipY = 1.0
        fm11 = 0.5f * (float)(viewport.clipHorizontal.wWidth);
        fm41 = fm11;

        fm22 = -0.5f * (float)(viewport.clipVertical.wHeight);
        fm42 = -fm22;

        fm33 = pContext->hwState.dvZScale * (fMaxZ - fMinZ);
        fm43 = pContext->hwState.dvZScale * fMinZ;

        if (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW) {
            // we are doing w-buffering  - scale w to optimize use of buffer
            fm44 = pContext->hwState.dvZScale * pContext->surfaceViewport.dvInvWFar;
            // scale x, y, and z similarly to negate the effect of dividing through by a scaled w
            fm11 *= fm44;  fm22 *= fm44;  fm33 *= fm44;
            fm41 *= fm44;  fm42 *= fm44;  fm43 *= fm44;
        }
        else {
            fm44 = 1.0f;
        }

#ifdef NV_NULL_FILL_DRIVER
        fm11 = 0.0f;
        fm22 = 0.0f;
#endif

        mViewportMatrix._11 = fm11;
        mViewportMatrix._22 = fm22;
        mViewportMatrix._33 = fm33;
        mViewportMatrix._41 = fm41;
        mViewportMatrix._42 = fm42;
        mViewportMatrix._43 = fm43;
        mViewportMatrix._44 = fm44;

        // set the 3 Cheops matrices

        // Cheops modelview = D3D world * D3D view
        MatrixProduct4x4 (&mMV0, &(pContext->xfmWorld[0]), &(pContext->xfmView));

        pContext->hwState.kelvin.setMatrixTransposed (NV097_SET_MODEL_VIEW_MATRIX0(0), &mMV0);

#ifdef KELVIN_ILC
        pContext->hwState.SuperTri.setEye(&mMV0);
#endif
        // Normals are transformed by the transpose of the inverse of the model view matrix.
        // (see Graphics Gems I, p. 541).
        // We only invert the upper 3x3 in order to eliminate the effects of translations.
        // the transposition happens because unlike all other matrices, we DON'T un-transpose
        // this one when we send it to the HW. (this is the one matrix for which D3D's
        // backwardness is convenient)

        if (pContext->hwState.dwStateFlags & (KELVIN_FLAG_TEXSTATENEEDSINVMV | KELVIN_FLAG_LIGHTINGNEEDSINVMV)) {
            // there's no reason to waste time normalizing the inverse MV matrix if we're going to have to normalize
            // normals anyway. otherwise, they give us normalized stuff and we need to preserve the normalization
            Inverse4x4 (&mInvMV, &mMV0, (pContext->dwRenderState[D3DRENDERSTATE_NORMALIZENORMALS] == FALSE));
            pContext->hwState.kelvin.setInverseModelViewMatrix (NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0), &mInvMV);
        }

        if (GET_VERTEXBLEND_STATE(pContext) == FALSE) {
            // we're not skinning.
            // compute the composite matrix normally and skip the second set of modelview matrices.
            // cheops composite = D3D World*View*projection plus accomodations for the viewport
            MatrixProduct4x4 (&mTemp, &(pContext->xfmProj), &mViewportMatrix);
            MatrixProduct4x4 (&mCTM, &mMV0, &mTemp);
            pContext->hwState.kelvin.setMatrixTransposed (NV097_SET_COMPOSITE_MATRIX(0), &mCTM);

            // for supertri we need world*view*proj
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_SUPERTRI) {
                MatrixProduct4x4 (&pContext->hwState.mTransform, &mMV0, &(pContext->xfmProj));
            }
        }

        else {
            // when skinning, the composite matrix does NOT include the modelview matrix.
            // cheops composite = projection plus accomodations for the viewport
            MatrixProduct4x4 (&mCTM, &(pContext->xfmProj), &mViewportMatrix);
            pContext->hwState.kelvin.setMatrixTransposed (NV097_SET_COMPOSITE_MATRIX(0), &mCTM);

            // we also need to compute and send down the second set of modelview matrices
            for (DWORD i=1; i<KELVIN_NUM_MATRICES; i++) {
                MatrixProduct4x4 (&mMVBlend, &(pContext->xfmWorld[i]), &(pContext->xfmView));
                pContext->hwState.kelvin.setMatrixTransposed (NV097_SET_MODEL_VIEW_MATRIX0(0) + 0x40*i, &mMVBlend);
                if (pContext->hwState.dwStateFlags & (KELVIN_FLAG_TEXSTATENEEDSINVMV | KELVIN_FLAG_LIGHTINGNEEDSINVMV)) {
                    Inverse4x4 (&mInvMV, &mMVBlend, (pContext->dwRenderState[D3DRENDERSTATE_NORMALIZENORMALS] == FALSE));
                    pContext->hwState.kelvin.setInverseModelViewMatrix (NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0) + 0x40*i, &mInvMV);
                }
            }
        }

        pContext->hwState.kelvin.set1 (NV097_SET_NORMALIZATION_ENABLE,
                                       DRF_NUM(097, _SET_NORMALIZATION_ENABLE, _V, pContext->dwRenderState[D3DRENDERSTATE_NORMALIZENORMALS]));

        // set depth clipping
        float fClipNear, fClipFar;
        if (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW) {
            fClipNear = pContext->hwState.dvZScale * pContext->surfaceViewport.dvWNear * pContext->surfaceViewport.dvInvWFar;
            fClipFar  = pContext->hwState.dvZScale;
        }
        else {
            fClipNear = pContext->hwState.dvZScale * fMinZ;
            fClipFar  = pContext->hwState.dvZScale * fMaxZ;
        }
        pContext->hwState.kelvin.set2f (NV097_SET_CLIP_MIN, fClipNear, fClipFar);

    }  // !FVF_TRANSFORMED

    // set window clipping
    // we subtract one from xmax (ymax) because the HW clips to xmin <= x <= xmax
    // ie. it's inclusive of both limits. note this makes it impossible to clip to nothing
    DWORD x,y,w,h;
    x = viewport.clipHorizontal.wX;
    w = viewport.clipHorizontal.wWidth;
    y = viewport.clipVertical.wY;
    h = viewport.clipVertical.wHeight;

    dwClipH = DRF_NUM (097, _SET_WINDOW_CLIP_HORIZONTAL, _XMIN, ((x - KELVIN_VIEWPORT_BOUND) & 0xfff))  |
              DRF_NUM (097, _SET_WINDOW_CLIP_HORIZONTAL, _XMAX, ((x + w - 1 - KELVIN_VIEWPORT_BOUND) & 0xfff));
    dwClipV = DRF_NUM (097, _SET_WINDOW_CLIP_VERTICAL, _YMIN, ((y - KELVIN_VIEWPORT_BOUND) & 0xfff))  |
              DRF_NUM (097, _SET_WINDOW_CLIP_VERTICAL, _YMAX, ((y + h - 1 - KELVIN_VIEWPORT_BOUND) & 0xfff));

    pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_HORIZONTAL(0), dwClipH);
    pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_VERTICAL(0),   dwClipV);

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// This handles the SetControl0 method:
//      premultiplied alpha
//      texture perspective
//      z perspective
//      z format
//      stencil write enable

HRESULT nvSetKelvinControl0 (PNVD3DCONTEXT pContext)
{
    DWORD dwControl0;

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_CONTROL0;

    dwControl0 = DRF_DEF (097, _SET_CONTROL0, _COLOR_SPACE_CONVERT, _PASS);

    // SRWNOTE: D3D has the possibility of having pre-multiplied alpha in 1
    // texture stage and not in another stage. How do we handle this?
    // hmm... actually this seems to be specified on a per surface basis
    // hedge on this for now
// BUGBUG
#if 0
    if (statev.premultipliedalpha)
        dwControl0 |= DRF_DEF (097, _SET_CONTROL0, _PREMULTIPLIEDALPHA, _TRUE);
    else
        dwControl0 |= DRF_DEF (097, _SET_CONTROL0, _PREMULTIPLIEDALPHA, _FALSE);
#endif

    // disabling texture perspective kills color perspective as well. since
    // the latter is assumed by D3D, we must always enable the former.
    dwControl0 |= DRF_DEF (097, _SET_CONTROL0, _TEXTUREPERSPECTIVE, _TRUE);

    // w or z buffering
    dwControl0 |= DRF_NUM (097, _SET_CONTROL0, _Z_PERSPECTIVE_ENABLE,
                           (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW) ? 1:0);

    // D3D has no provision for specification of zbuffer format
    // somewhat arbitrarily choose FIXED. alternately we could select it
    // to their advantage according to whether w-buffering is enabled (?)
    dwControl0 |= DRF_DEF (097, _SET_CONTROL0, _Z_FORMAT, _FIXED);


    dwControl0 |= DRF_NUM (097, _SET_CONTROL0, _STENCIL_WRITE_ENABLE,
                           pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]);

    pContext->hwState.kelvin.set1 (NV097_SET_CONTROL0, dwControl0);

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set everything else not covered by other kelvin state routines. this includes:
//      - depth testing
//      - stencil testing
//      - alpha testing
//      - alpha blending

HRESULT nvSetKelvinMiscState (PNVD3DCONTEXT pContext)
{
    DWORD dwHWStage, dwMask;
    DWORD dwSrcFactor, dwDstFactor;
    DWORD dwPointSize;
    DWORD dwZEnable;
    float fDepthBias, fDepthScale;
    float fAtten, fPP0, fPP1, fPP2, fPP3, fPP4, fPP5;  // point size variables
    float fSizeMin, fSizeMax, fSize, fHeight, fScale;

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_MISC_STATE;

    // z-buffering
    dwZEnable = pContext->pZetaBuffer ? pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] : D3DZB_FALSE;
    if (dwZEnable) {
        pContext->hwState.kelvin.set1 (NV097_SET_DEPTH_TEST_ENABLE, NV097_SET_DEPTH_TEST_ENABLE_V_TRUE);
        pContext->hwState.kelvin.set1 (NV097_SET_DEPTH_MASK,        DRF_NUM (097, _SET_DEPTH_MASK, _V, pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE]));
        pContext->hwState.kelvin.set1 (NV097_SET_DEPTH_FUNC,        nv097DepthFunc[CTFunc(pContext,pContext->dwRenderState[D3DRENDERSTATE_ZFUNC])]);
    }
    else {
        pContext->hwState.kelvin.set1 (NV097_SET_DEPTH_TEST_ENABLE, NV097_SET_DEPTH_TEST_ENABLE_V_FALSE);
    }

    //if no zbuffer (that's true that enable is set to false...) and in passthrough mode, disable min/max z culling
    if ((dwZEnable == D3DZB_FALSE) && (pContext->hwState.dwStateFlags & KELVIN_FLAG_PASSTHROUGHMODE)) {
        pContext->hwState.kelvin.set1 (NV097_SET_ZMIN_MAX_CONTROL, (DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _CULL_NEAR_FAR_EN, _FALSE)
                                                                  | DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _ZCLAMP_EN, _CLAMP)
                                                                  | DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _CULL_IGNORE_W, _TRUE)));
    }
    else if (dwZEnable == D3DZB_USEW) {
        pContext->hwState.kelvin.set1 (NV097_SET_ZMIN_MAX_CONTROL, (DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _CULL_NEAR_FAR_EN, _FALSE)
                                                                  | DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _ZCLAMP_EN, _CULL)
                                                                  | DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _CULL_IGNORE_W, _FALSE)));
    }
    else {
        // if z-buffering and/or not passthrough
        pContext->hwState.kelvin.set1 (NV097_SET_ZMIN_MAX_CONTROL, (DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _CULL_NEAR_FAR_EN, _TRUE)
                                                                  | DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _ZCLAMP_EN, _CULL)
                                                                  | DRF_DEF (097, _SET_ZMIN_MAX_CONTROL, _CULL_IGNORE_W, _FALSE)));
    }

    // fill mode
    switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]) {
        case D3DFILL_POINT:
            pContext->hwState.kelvin.set2 (NV097_SET_FRONT_POLYGON_MODE,
                                           NV097_SET_FRONT_POLYGON_MODE_V_POINT,
                                           NV097_SET_BACK_POLYGON_MODE_V_POINT);
            break;
        case D3DFILL_WIREFRAME:
            pContext->hwState.kelvin.set2 (NV097_SET_FRONT_POLYGON_MODE,
                                           NV097_SET_FRONT_POLYGON_MODE_V_LINE,
                                           NV097_SET_BACK_POLYGON_MODE_V_LINE);
            break;
        case D3DFILL_SOLID:
            pContext->hwState.kelvin.set2 (NV097_SET_FRONT_POLYGON_MODE,
                                           NV097_SET_FRONT_POLYGON_MODE_V_FILL,
                                           NV097_SET_BACK_POLYGON_MODE_V_FILL);
            break;
        default:
            DPF ("unknown FILLMODE");
            dbgD3DError();
            break;
    }

    // shading
    pContext->hwState.kelvin.set1 (NV097_SET_SHADE_MODE, (pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE] == D3DSHADE_FLAT)
                                                         ? NV097_SET_SHADE_MODE_V_FLAT
                                                         : NV097_SET_SHADE_MODE_V_SMOOTH);

    // color write enable
    DWORD dwPlaneMask = pContext->dwRenderState[D3DRS_COLORWRITEENABLE];
    if (dwPlaneMask == (D3DCOLORWRITEENABLE_ALPHA |
                        D3DCOLORWRITEENABLE_RED   |
                        D3DCOLORWRITEENABLE_GREEN |
                        D3DCOLORWRITEENABLE_BLUE)) {
        // the common case
        dwMask = 0x01010101;
    }
    else {
        dwMask = 0;
        if (dwPlaneMask & D3DCOLORWRITEENABLE_ALPHA) dwMask |= (0x1 << 24);
        if (dwPlaneMask & D3DCOLORWRITEENABLE_RED)   dwMask |= (0x1 << 16);
        if (dwPlaneMask & D3DCOLORWRITEENABLE_GREEN) dwMask |= (0x1 << 8);
        if (dwPlaneMask & D3DCOLORWRITEENABLE_BLUE)  dwMask |= (0x1 << 0);
    }

    pContext->hwState.kelvin.set1 (NV097_SET_COLOR_MASK, dwMask);

    // alpha blending
    if (pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]) {
        pContext->hwState.kelvin.set1 (NV097_SET_BLEND_ENABLE, NV097_SET_BLEND_ENABLE_V_TRUE);
        dwSrcFactor = nv097BlendFunc[pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]];
        dwDstFactor = nv097BlendFunc[pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]];
        // for these two cases, we need to go back and override dwDstFactor
        switch (pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]) {
            case D3DBLEND_BOTHSRCALPHA:    // obsolete, but we'll do it for Dx5 and prior
                dwDstFactor = NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_ALPHA;
                break;
            case D3DBLEND_BOTHINVSRCALPHA: // likewise
                dwDstFactor = NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA;
                break;
            default:
                // nada
                break;
        }
        pContext->hwState.kelvin.set2 (NV097_SET_BLEND_FUNC_SFACTOR, dwSrcFactor, dwDstFactor);
        pContext->hwState.kelvin.set1 (NV097_SET_BLEND_EQUATION,
                                       (pContext->dwDXAppVersion >= 0x0800) ?
                                       nv097BlendOp[pContext->dwRenderState[D3DRS_BLENDOP]] :
                                       DRF_DEF (097, _SET_BLEND_EQUATION, _V, _FUNC_ADD));
    }
    else {
        pContext->hwState.kelvin.set1 (NV097_SET_BLEND_ENABLE, NV097_SET_BLEND_ENABLE_V_FALSE);
    }

    //anti aliased edges
    if (getDC()->nvD3DPerfData.dwHaveAALines){
        if (pContext->dwRenderState[D3DRENDERSTATE_EDGEANTIALIAS]){
            pContext->hwState.kelvin.set1(NV097_SET_LINE_SMOOTH_ENABLE, NV097_SET_LINE_SMOOTH_ENABLE_V_TRUE);
        }
        else {
            pContext->hwState.kelvin.set1(NV097_SET_LINE_SMOOTH_ENABLE, NV097_SET_LINE_SMOOTH_ENABLE_V_FALSE);
        }
    }

    // culling
    switch (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]) {
        case D3DCULL_CW:
            pContext->hwState.kelvin.set1 (NV097_SET_CULL_FACE_ENABLE, NV097_SET_CULL_FACE_ENABLE_V_TRUE);
            pContext->hwState.kelvin.set1 (NV097_SET_FRONT_FACE,       NV097_SET_FRONT_FACE_V_CCW);
            break;
        case D3DCULL_CCW:
            pContext->hwState.kelvin.set1 (NV097_SET_CULL_FACE_ENABLE, NV097_SET_CULL_FACE_ENABLE_V_TRUE);
            pContext->hwState.kelvin.set1 (NV097_SET_FRONT_FACE,       NV097_SET_FRONT_FACE_V_CW);
            break;
        default:
            DPF ("unknown cull mode");
        case D3DCULL_NONE:
            pContext->hwState.kelvin.set1 (NV097_SET_CULL_FACE_ENABLE, NV097_SET_CULL_FACE_ENABLE_V_FALSE);
            break;
    }

    // alpha test
    if (pContext->dwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]) {
        pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_TEST_ENABLE, NV097_SET_ALPHA_TEST_ENABLE_V_TRUE);
        pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_REF,         pContext->dwRenderState[D3DRENDERSTATE_ALPHAREF]);
        pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_FUNC,        nv097AlphaFunc[pContext->dwRenderState[D3DRENDERSTATE_ALPHAFUNC]]);
    }
    else {
        pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_TEST_ENABLE, NV097_SET_ALPHA_TEST_ENABLE_V_FALSE);
    }
#ifdef ALPHA_CULL
    // alpha test
#if ALPHA_CULL ==0
    pContext->alphacull_mode = 0;
    if (!pContext->dwRenderState[D3DRENDERSTATE_ALPHATESTENABLE] && pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE] &&
        !pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] && !pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE] &&
    pContext->dwEarlyCopyStrategy
       )
    {
#else
    pContext->hwState.alphacull_mode = 0;
    if (!pContext->dwRenderState[D3DRENDERSTATE_ALPHATESTENABLE] && pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE] &&
        /*!pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] &&*/ !pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]  &&
    pContext->dwEarlyCopyStrategy
       )
#endif
    {
        pContext->hwState.alphacull_mode = 1;
        if(pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_SRCALPHA &&
           pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_INVSRCALPHA){
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_TEST_ENABLE, NV097_SET_ALPHA_TEST_ENABLE_V_TRUE);
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_REF, 0x8);
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_FUNC, NV097_SET_ALPHA_FUNC_V_GREATER);
        }
#if ALPHA_CULL >= 2
        /*else if(pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_ZERO &&
           pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_SRCCOLOR &&
           pContext->hwState.dwAlphaICW[0] == 0x18200000 && pContext->hwState.dwAlphaOCW[0] == 0xc00 &&
           pContext->hwState.dwColorICW[0] == 0x08040000 && pContext->hwState.dwColorOCW[0] == 0xc00)
        {
            pContext->hwState.celsius.alphacull_mode = 2;
            pContext->hwState.celsius.set (NV056_SET_COMBINER_COLOR_ICW(0), 0x08042820);
            pContext->hwState.celsius.set (NV056_SET_COMBINER0_COLOR_OCW,   0x000010CD);
            pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_ICW(0), 0x18200000);
            pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_OCW(0), 0x00000C00);
            pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW0, pContext->hwState.specfog_cw[0]);
            pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW1, 0x0D00 | (pContext->hwState.specfog_cw[1] & ~0x1f00));
            pContext->hwState.celsius.set (NV056_SET_ALPHA_TEST_ENABLE, NV056_SET_ALPHA_TEST_ENABLE_V_TRUE);
            pContext->hwState.celsius.set (NV056_SET_ALPHA_REF, 0x8);
            pContext->hwState.celsius.set (NV056_SET_ALPHA_FUNC, NV056_SET_ALPHA_FUNC_V_GREATER);
        }*/
#endif
#if ALPHA_CULL >= 3
        else if(pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_ONE &&
           pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_ONE &&
           pContext->hwState.dwColorICW[0] == 0x08040000 && pContext->hwState.dwColorOCW[0] == 0xc00 &&
           pContext->hwState.dwAlphaICW[0] == 0x18200000 && pContext->hwState.dwAlphaOCW[0] == 0xc00 &&
           pContext->dwEarlyCopyStrategy == 3)
        {
            pContext->hwState.alphacull_mode = 3;
            pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_COLOR_ICW(0), 0x08040820);
            pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_COLOR_OCW(0),   0x000010CD);
            pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_ALPHA_ICW(0), 0x18200000);
            pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_ALPHA_OCW(0), 0x00000C00);
            pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_SPECULAR_FOG_CW0, pContext->hwState.specfog_cw[0]);
            pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_SPECULAR_FOG_CW1, 0x0D00 | (pContext->hwState.specfog_cw[1] & ~0x1f00));
            pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_TEST_ENABLE, NV097_SET_ALPHA_TEST_ENABLE_V_TRUE);
            pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_REF, 0x8);
            pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_FUNC, NV097_SET_ALPHA_FUNC_V_GREATER);
        }
#endif
        else if(pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_INVSRCALPHA &&
                pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_SRCALPHA){
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_TEST_ENABLE, NV097_SET_ALPHA_TEST_ENABLE_V_TRUE);
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_REF, 0xf4);
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_FUNC, NV097_SET_ALPHA_FUNC_V_LESS);
        }
        else if(pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_ZERO){
            if(pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_SRCALPHA){
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_TEST_ENABLE, NV097_SET_ALPHA_TEST_ENABLE_V_TRUE);
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_REF, 0xf4);
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_FUNC, NV097_SET_ALPHA_FUNC_V_LESS);
            }
            else if(pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_INVSRCALPHA){
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_TEST_ENABLE, NV097_SET_ALPHA_TEST_ENABLE_V_TRUE);
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_REF, 0x8);
                pContext->hwState.kelvin.set1 (NV097_SET_ALPHA_FUNC, NV097_SET_ALPHA_FUNC_V_GREATER);
            }
        }
    }
#endif

    // dithering
    pContext->hwState.kelvin.set1 (NV097_SET_DITHER_ENABLE,
                                DRF_NUM (097, _SET_DITHER_ENABLE, _V, pContext->dwRenderState[D3DRENDERSTATE_DITHERENABLE]));

    // Multisample mask
    pContext->kelvinAA.SetSampleMask();

    // depth bias
    if ((dwZEnable) &&
        (pContext->dwRenderState[D3DRENDERSTATE_ZBIAS])) {
        // the bias is applied to depth values in device coordinates
        // (i.e in the range [0..2^16-1] or [0..2^24-1])
        pContext->hwState.kelvin.set3 (NV097_SET_POLY_OFFSET_POINT_ENABLE,
                                    NV097_SET_POLY_OFFSET_POINT_ENABLE_V_TRUE,
                                    NV097_SET_POLY_OFFSET_LINE_ENABLE_V_TRUE,
                                    NV097_SET_POLY_OFFSET_FILL_ENABLE_V_TRUE);
        fDepthBias  = -(float)(pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]);
        fDepthScale = 0.25f * fDepthBias;
        pContext->hwState.kelvin.set2f (NV097_SET_POLYGON_OFFSET_SCALE_FACTOR, fDepthScale, fDepthBias);
    }
    else {
        // renderstate_zbias==0 and/or zenable==D3DZB_FALSE
        pContext->hwState.kelvin.set3 (NV097_SET_POLY_OFFSET_POINT_ENABLE,
                                       NV097_SET_POLY_OFFSET_POINT_ENABLE_V_FALSE,
                                       NV097_SET_POLY_OFFSET_LINE_ENABLE_V_FALSE,
                                       NV097_SET_POLY_OFFSET_FILL_ENABLE_V_FALSE);
    }

    // stencil
    if (pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]) {
        pContext->bStencilEnabled = TRUE;
        pContext->hwState.kelvin.set1 (NV097_SET_STENCIL_TEST_ENABLE, NV097_SET_STENCIL_TEST_ENABLE_V_TRUE);
        pContext->hwState.kelvin.set7 (NV097_SET_STENCIL_MASK,
                                       pContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK] & 0xff,
                                       nv097StencilFunc[pContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC]],
                                       pContext->dwRenderState[D3DRENDERSTATE_STENCILREF]       & 0xff,
                                       pContext->dwRenderState[D3DRENDERSTATE_STENCILMASK]      & 0xff,
                                       nv097StencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL]],
                                       nv097StencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL]],
                                       nv097StencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]]);
    }
    else {
        pContext->hwState.kelvin.set1 (NV097_SET_STENCIL_TEST_ENABLE, NV097_SET_STENCIL_TEST_ENABLE_V_FALSE);
    }

    // combiner factor
    if (!pContext->pCurrentPShader) {
        for (dwHWStage=0; dwHWStage<KELVIN_NUM_COMBINERS; dwHWStage++) {
            pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_FACTOR0(dwHWStage), pContext->dwRenderState[D3DRENDERSTATE_TEXTUREFACTOR]);
            pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_FACTOR1(dwHWStage), pContext->dwRenderState[D3DRENDERSTATE_TEXTUREFACTOR]);
        }
    }

    // skinning
    switch (GET_VERTEXBLEND_STATE(pContext)) {
        case D3DVBF_DISABLE:
            pContext->hwState.kelvin.set1 (NV097_SET_SKIN_MODE, NV097_SET_SKIN_MODE_V_OFF);
            break;
        case D3DVBF_1WEIGHTS:
            pContext->hwState.kelvin.set1 (NV097_SET_SKIN_MODE, NV097_SET_SKIN_MODE_V_2G);
            break;
        case D3DVBF_2WEIGHTS:
            pContext->hwState.kelvin.set1 (NV097_SET_SKIN_MODE, NV097_SET_SKIN_MODE_V_3G);
            break;
        case D3DVBF_3WEIGHTS:
            pContext->hwState.kelvin.set1 (NV097_SET_SKIN_MODE, NV097_SET_SKIN_MODE_V_4G);
            break;
        default:
            DPF ("illegal vertex blend mode");
            dbgD3DError();
            break;
    }  // switch

    // point params
    if (pContext->dwDXAppVersion >= 0x800) {

        // setting POINT_PARAMS_ENABLE to true tells the HW that a screen-space point size is being
        // calculated or needs to be calculated per vertex. in fixed-pipe mode, the HW will perform
        // the calculation itself, as a function of POINT_SIZE and POINT_PARAMS. in program mode, the
        // calculation is handled by some other entity (vertex shader / SW T&L) but POINT_PARAMS_ENABLE
        // is still set true so that the HW knows to fetch the pre-calculated point size from oPts.x.
        // when POINT_PARAMS_ENABLE is false, the HW simple takes the static value from POINT_SIZE
        // and performs no additional calculations.

        DWORD dwMode = pContext->pCurrentVShader->hasProgram() ?
                           1 :
                           ((pContext->hwState.dwStateFlags & KELVIN_FLAG_PASSTHROUGHMODE) ?
                               2 :
                               3);

        BOOL bHavePSize = pContext->pCurrentVShader->bVAExists (defaultInputRegMap[D3DVSDE_PSIZE]);
        BOOL bPScaleEn  = pContext->dwRenderState[D3DRS_POINTSCALEENABLE];

        // check for incompatibility with broken HW
        if ((dwMode == 3) && (bHavePSize)) {
            DPF ("uh oh. app wants to provide a point size per vertex in fixed-pipe mode.");
            dbgD3DError();
            pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
        }

        // enable point params if...
        BOOL bPPEnable = (dwMode == 1)                  ||  // ...we have a vertex shader
                         ((dwMode == 2) && bHavePSize)  ||  // ...we're in passthrough mode and have a per-vertex point size
                         ((dwMode == 3) && bPScaleEn);      // ...we're in fixed-pipe mode and D3DRS_POINTSCALEENABLE is true

        pContext->hwState.kelvin.set2 (NV097_SET_POINT_PARAMS_ENABLE,
                                       bPPEnable ?
                                       NV097_SET_POINT_PARAMS_ENABLE_V_TRUE :
                                       NV097_SET_POINT_PARAMS_ENABLE_V_FALSE,
                                       pContext->dwRenderState[D3DRS_POINTSPRITEENABLE] ?
                                       NV097_SET_POINT_SMOOTH_ENABLE_V_TRUE :
                                       NV097_SET_POINT_SMOOTH_ENABLE_V_FALSE);

        // we need to set POINT_SIZE if...
        if (((dwMode == 2) && (!bHavePSize)) ||  // ...we're in passthrough mode without a per-vertex point size
            (dwMode == 3))                       // ...we're in fixed-pipe mode
        {
            fSizeMin = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSIZE_MIN]);
            fSizeMax = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSIZE_MAX]);
            fSize    = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSIZE]);

            // workaround for NV20 bug where point sprites < 0.125 get bumped to 1.0
            if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) == NVCLASS_0097_KELVIN) {
                // clamp POINTSIZE_MIN to >= 0.125
                fSizeMin = max (fSizeMin, 0.125f);
            }

            fScale   = max (fSize, 0.0001f);

            fSize    = max (fSize, fSizeMin);
            fSize    = min (fSize, fSizeMax);

            if ((pContext->surfaceViewport.clipVertical.wHeight) && (pContext->dwRenderState[D3DRS_POINTSCALEENABLE])) {
                fHeight = (float)(pContext->surfaceViewport.clipVertical.wHeight);
                // have to do this loop if we want to move height into the attenuation
                // factors because valid point sprite sizes < 0.125 will be expressed
                // as zeros if we don't do something  HMH
                while ((fSize < 32.0f) && (fHeight > 2.0f)) {
                    fSize   *= 2.0f;
                    fScale  *= 2.0f;
                    fHeight *= 0.5f;
                }
            }

            dwPointSize = ((DWORD)(fSize * 8.0f)) & 0x1ff;  // convert to 6.3

            pContext->hwState.kelvin.set1 (NV097_SET_POINT_SIZE, dwPointSize);

            // finally, we must also set POINT_PARAMS if we're in fixed pipe mode with POINTSCALEENABLE set true
            if ((dwMode == 3) && bPScaleEn) {

                fPP3 = fSizeMax - fSizeMin;
                fPP4 = fPP3 ? (-fSizeMin / fPP3) : 0.0f;
                fPP5 = fSizeMin;

                fAtten  = fPP3 / (fScale * fHeight);
                fAtten *= fAtten;

                fPP0 = fAtten * FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSCALE_A]);
                fPP1 = fAtten * FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSCALE_B]);
                fPP2 = fAtten * FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSCALE_C]);

                pContext->hwState.kelvin.set8f (NV097_SET_POINT_PARAMS(0), fPP0,fPP1,fPP2,fPP3,fPP3,fPP3,fPP4,fPP5);

            }
        }
    }

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

__inline DWORD nvKelvinVertexFormat
(
    CVertexShader  *pShader,
    CVertexBuffer **ppStreams,
    DWORD           dwReg,
    BOOL            bInlineVertices,
    DWORD           dwInlineStride
)
{
    DWORD dwFormat, dwStreamSelector, dwType;

    dwStreamSelector = pShader->getVAStream (dwReg);

    if ((dwStreamSelector == CVertexShader::VA_STREAM_NONE) || (ppStreams[dwStreamSelector] == NULL)) {
        dwFormat = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                   DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);
    }
    else {
        dwType   = pShader->getVAType (dwReg);
        dwFormat = (DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, kelvinVertexSize[dwType]) |
                    DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, kelvinVertexType[dwType]) |
                    DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _STRIDE, bInlineVertices ?
                                                                          dwInlineStride  :
                                                                          ppStreams[dwStreamSelector]->getVertexStride()));
        if (dwType == D3DVSDT_UBYTE4) {
            DPF ("app is using unsupported D3DVSDT_UBYTE4 despite D3DVTXPCAPS_NO_VSDT_UBYTE4");
            nvAssert(0);
        }
    }

    return (dwFormat);
}

//---------------------------------------------------------------------------

__inline DWORD nvKelvinVertexOffset
(
    CVertexShader  *pShader,
    CVertexBuffer **ppStreams,
    DWORD           dwReg,
    DWORD           dwBaseVertexOffset
)
{
    DWORD dwOffset;
    DWORD dwStreamSelector;
    DWORD dwOffsetAdjustment;
    DWORD dwDMAMask, dwVBOffset;

    dwStreamSelector = pShader->getVAStream (dwReg);

    if ((dwStreamSelector == CVertexShader::VA_STREAM_NONE) || (ppStreams[dwStreamSelector] == NULL)) {

        dwOffset = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A) |
                   DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, 0);

    }

    else {

        // program the context dma and calculate a kelvin-specific offset adjustment:
        // kelvin wants an offset relative to a page aligned context dma, something
        // we do not know we have using the built in getOffset() methods.
        switch (ppStreams[dwStreamSelector]->getContextDMA()) {
            case NV_CONTEXT_DMA_AGP_OR_PCI:
                dwDMAMask = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A);
                // compensate for ctx dma that is possibly not 4k aligned
                dwOffsetAdjustment = (ppStreams[dwStreamSelector]->getHeapLocation() == CSimpleSurface::HEAP_AGP) ?
                                     (pDriverData->GARTLinearBase & 0xfff) :      // AGP
                                     (getDC()->nvD3DTexHeapData.dwBase & 0xfff);  // PCI
                break;
            case NV_CONTEXT_DMA_VID:
                dwDMAMask = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_B);
                // NOTE - this is highly unlikely to not be 4k aligned...
                nvAssert ((pDriverData->BaseAddress & 0xfff) == 0);
                dwOffsetAdjustment = 0;
                break;
            default:
                DPF ("unknown context DMA");
                dbgD3DError();
                break;
        }  // switch

        dwVBOffset = ppStreams[dwStreamSelector]->getOffset() +
                     dwBaseVertexOffset +
                     pShader->getVAOffset (dwReg) +
                     dwOffsetAdjustment;

        dwOffset = dwDMAMask | DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, dwVBOffset);

    }

    return (dwOffset);
}

//---------------------------------------------------------------------------

// tell the hardware where to find vertex coordinates, normals, etc...
// the order of these is important!

HRESULT nvSetKelvinVertexFormat (PNVD3DCONTEXT pContext)
{
    DWORD  dwFormat[16], dwOffset[16];
    DWORD  dwHWStage, dwD3DStage, dwTCIndex, dwVAIndex;
    DWORD  dwInlineStride;
    BOOL   bInlineVertices;

    pContext->hwState.dwDirtyFlags &= ~KELVIN_DIRTY_FVF;

    CVertexShader  *pShader   = pContext->pCurrentVShader;
    CVertexBuffer **ppStreams = pContext->ppDX8Streams;

    // shadow the values so we know what we last sent to the HW
    pContext->hwState.dwVertexOffset = pContext->dp2.dwVertexBufferOffset;
    pContext->hwState.pVertexShader  = pContext->pCurrentVShader;
    nvMemCopy (pContext->hwState.ppStreams, pContext->ppDX8Streams, KELVIN_CAPS_MAX_STREAMS*sizeof(CVertexBuffer *));

#ifdef STOMP_TEX_COORDS
    pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_STOMP_4TH_COORD_MASK;

    if (!pShader->hasProgram()) {
        for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {
            if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage)) {
                dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;
                if ((pShader->getVASize(defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex]) == 16) &&
                    (KELVIN_GET_NUM_TEX_COORDS(pContext->hwState.dwNumTexCoordsNeeded, dwHWStage) < 4)) {
                    pContext->hwState.dwStateFlags |= KELVIN_FLAG_STOMP_4TH_COORD(dwHWStage);
                }
            }
        }
    }

    // This should only happen in WHQL.  If it's happening elsewhere, either the app
    // is really stupid, or we're doing something wrong.
    nvAssert(!(pContext->hwState.dwStateFlags & KELVIN_FLAG_STOMP_4TH_COORD_MASK));
#endif  // STOMP_TEX_COORDS

    // figure out if we can dma this vertex data
#ifdef FORCE_INLINE_VERTICES
    bInlineVertices = TRUE;
#else
#ifdef STOMP_TEX_COORDS
    bInlineVertices = ((NV_NUM_UNIQUE_STREAMS(pContext->dwStreamDMACount) > 2) ||
                       (NV_NUM_SYSTEM_STREAMS(pContext->dwStreamDMACount) > 0) ||
                       (pContext->hwState.dwStateFlags & KELVIN_FLAG_STOMP_4TH_COORD_MASK));
#else  // !STOMP_TEX_COORDS
    bInlineVertices = ((NV_NUM_UNIQUE_STREAMS(pContext->dwStreamDMACount) > 2) ||
                       (NV_NUM_SYSTEM_STREAMS(pContext->dwStreamDMACount) > 0));
#endif  // !STOMP_TEX_COORDS
#endif

    if (bInlineVertices) {
        dwInlineStride = pShader->hasProgram() ?
                         pContext->pCurrentVShader->getStride() :
                         nvKelvinInlineVertexStride (pContext);
        nvAssert ( !(dwInlineStride == 4 && !pShader->hasProgram()) );
    }
    else {
        dwInlineStride = 0;
    }
    // set context inline stride
    pContext->hwState.dwInlineVertexStride = dwInlineStride; //don't want to lose the actual stride
    //workaround for kelvin bug
    if (dwInlineStride == 4) dwInlineStride = 8; //fake out vertex setup for HW failure case

    // formats --------------------------------------------------------------

    if (pShader->hasProgram()) {

        for (DWORD dwReg = 0; dwReg < 16; dwReg++) {
            dwFormat[dwReg] = nvKelvinVertexFormat (pShader, ppStreams, dwReg, bInlineVertices, dwInlineStride);
        }
    }

    else {

        // x, y, z, [w]
        dwFormat[0] = nvKelvinVertexFormat (pShader, ppStreams, defaultInputRegMap[D3DVSDE_POSITION], bInlineVertices, dwInlineStride);

        // weights (for skinning)
        dwFormat[1] = nvKelvinVertexFormat (pShader, ppStreams, defaultInputRegMap[D3DVSDE_BLENDWEIGHT], bInlineVertices, dwInlineStride);

        // normal
        dwFormat[2] = nvKelvinVertexFormat (pShader, ppStreams, defaultInputRegMap[D3DVSDE_NORMAL], bInlineVertices, dwInlineStride);

        // diffuse
        dwFormat[3] = nvKelvinVertexFormat (pShader, ppStreams, defaultInputRegMap[D3DVSDE_DIFFUSE], bInlineVertices, dwInlineStride);
        if (dwFormat[3] == (DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                            DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F))) {
            // no vertex diffuse. default to white
            pContext->hwState.kelvin.set1 (NV097_SET_DIFFUSE_COLOR4UB, 0xFFFFFFFF);
        }

        // specular
        dwFormat[4] = nvKelvinVertexFormat (pShader, ppStreams, defaultInputRegMap[D3DVSDE_SPECULAR], bInlineVertices, dwInlineStride);
        if (dwFormat[4] == (DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                            DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F))) {
            // no vertex specular. default to black
            pContext->hwState.kelvin.set1 (NV097_SET_SPECULAR_COLOR4UB, 0);
        }

        // fog distance (never exists in d3d)
        dwFormat[5] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                      DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

        // point size
        dwFormat[6] = nvKelvinVertexFormat (pShader, ppStreams, defaultInputRegMap[D3DVSDE_PSIZE], bInlineVertices, dwInlineStride);

        // back diffuse (DX8 was going to have this, but axed it)
        dwFormat[7] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                      DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

        // back specular (DX8 was going to have this, but axed it)
        dwFormat[8] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                      DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

        // texture 0-3
        for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {
            if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage)) {
                // the app needs texture N and provided us with one
                dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
                nvAssert (dwD3DStage != KELVIN_UNUSED);
                dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;
                dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
                nvAssert (pShader->bVAExists(dwVAIndex));  // they better have given us coordinates
                dwFormat[9+dwHWStage] = nvKelvinVertexFormat (pShader, ppStreams, dwVAIndex, bInlineVertices, dwInlineStride);
            }
            else {
                dwFormat[9+dwHWStage] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                                        DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);
            }
        }  // for dwHWStage

        // remainder are unused
        dwFormat[13] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                       DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);
        dwFormat[14] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                       DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);
        dwFormat[15] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) |
                       DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

    }

    //"fix" the format if it's only one DWORD and headed towards inline.
    //workaround for kelvin bug
    if (bInlineVertices && pContext->hwState.dwInlineVertexStride == 4)
    {
        if (dwFormat[0] == 2 ) //the one actual DWORD is somewhere else (not likely)
        {
            dwFormat[0]=0x825; //hardcode to 2 DWORD stride/ 2 Short value.
        }
        else //use register 1 for bogus info
        {
            dwFormat[1]=0x825; //hardcode to 2 DWORD stride/ 2 Short value.
        }
    }
    // send the formats to the HW
    pContext->hwState.kelvin.set8 (NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0),
                                   dwFormat[0],  dwFormat[1],  dwFormat[2],  dwFormat[3],
                                   dwFormat[4],  dwFormat[5],  dwFormat[6],  dwFormat[7]);
    pContext->hwState.kelvin.set8 (NV097_SET_VERTEX_DATA_ARRAY_FORMAT(8),
                                   dwFormat[8],  dwFormat[9],  dwFormat[10], dwFormat[11],
                                   dwFormat[12], dwFormat[13], dwFormat[14], dwFormat[15]);

    // offsets --------------------------------------------------------------

    // we only need to send offsets if we're dma'ing vertex data
    if (!bInlineVertices) {

        // invalidate the vertex cache since we're messing with offsets
        pContext->hwState.kelvin.set1 (NV097_INVALIDATE_VERTEX_CACHE_FILE, 0);

        if (pShader->hasProgram()) {

            for (DWORD dwReg = 0; dwReg < 16; dwReg++) {
                dwOffset[dwReg] = nvKelvinVertexOffset (pShader, ppStreams, dwReg, pContext->hwState.dwVertexOffset);
            }

        }

        else {

            // x,y,z,[w]
            dwOffset[0] = nvKelvinVertexOffset (pShader, ppStreams, defaultInputRegMap[D3DVSDE_POSITION], pContext->hwState.dwVertexOffset);

            // weights
            dwOffset[1] = nvKelvinVertexOffset (pShader, ppStreams, defaultInputRegMap[D3DVSDE_BLENDWEIGHT], pContext->hwState.dwVertexOffset);

            // normal
            dwOffset[2] = nvKelvinVertexOffset (pShader, ppStreams, defaultInputRegMap[D3DVSDE_NORMAL], pContext->hwState.dwVertexOffset);

            // diffuse
            dwOffset[3] = nvKelvinVertexOffset (pShader, ppStreams, defaultInputRegMap[D3DVSDE_DIFFUSE], pContext->hwState.dwVertexOffset);

            // specular
            dwOffset[4] = nvKelvinVertexOffset (pShader, ppStreams, defaultInputRegMap[D3DVSDE_SPECULAR], pContext->hwState.dwVertexOffset);

            // fog distance
            dwOffset[5] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A) |
                          DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, 0);

            // point size
            dwOffset[6] = nvKelvinVertexOffset (pShader, ppStreams, defaultInputRegMap[D3DVSDE_PSIZE], pContext->hwState.dwVertexOffset);

            // back diffuse
            dwOffset[7] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A) |
                          DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, 0);

            // back specular
            dwOffset[8] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A) |
                          DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, 0);

            // texture 0-3
            for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {
                if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage)) {
                    // the app needs texture N and provided us with one
                    dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
                    nvAssert (dwD3DStage != KELVIN_UNUSED);
                    dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;
                    dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
                    nvAssert (pShader->bVAExists(dwVAIndex));
                    dwOffset[9+dwHWStage] = nvKelvinVertexOffset (pShader, ppStreams, dwVAIndex,
                                                                  pContext->hwState.dwVertexOffset);
                }
                else {
                    dwOffset[9+dwHWStage] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A) |
                                            DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, 0);
                }
            }  // for dwHWStage

            // remainder are unused
            dwOffset[13] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A) |
                           DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, 0);
            dwOffset[14] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A) |
                           DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, 0);
            dwOffset[15] = DRF_DEF (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _CONTEXT_DMA, _VERTEX_A) |
                           DRF_NUM (097, _SET_VERTEX_DATA_ARRAY_OFFSET, _OFFSET, 0);

        }

        // send the offsets to the HW
        pContext->hwState.kelvin.set8 (NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0),
                                       dwOffset[0],  dwOffset[1],  dwOffset[2],  dwOffset[3],
                                       dwOffset[4],  dwOffset[5],  dwOffset[6],  dwOffset[7]);
        pContext->hwState.kelvin.set8 (NV097_SET_VERTEX_DATA_ARRAY_OFFSET(8),
                                       dwOffset[8],  dwOffset[9],  dwOffset[10], dwOffset[11],
                                       dwOffset[12], dwOffset[13], dwOffset[14], dwOffset[15]);

    }  // !bInlineVertices

    dbgFlushType (NVDBG_FLUSH_STATE);
    return (D3D_OK);
}


#ifdef PROFILE_STATE_CHANGES
// Profile number of times a given state is changed. This is quick and dirty and will be improved.
static DWORD state_count[14] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

//---------------------------------------------------------------------------

// set all state of the kelvin object

HRESULT __stdcall nvSetKelvinState (PNVD3DCONTEXT pContext)
{


#ifdef PROFILE_STATE_CHANGES
    state_count[13]++;
#endif

#ifdef NV_PROFILE_DP2OPS
    pDriverData->pBenchmark->beginHWStateChange(pContext->hwState.dwDirtyFlags, pDriverData->nvPusher.m_dwPut);
#endif

//    These calls have been moved/deferred from nvDrawPrimitives2 to nvSetKelvinState in order to avoid spurious upsampling in AA modes
//    where we have a simple Flip-SRT-Lock-UnLock-Flip loop.  The SRT as a consequence of the Flip was causing us
//    to do an unneeded upsample when no 3D rendering actually occurred.  Defer to SetKelvinState because it is only
//    called JUST before 3D rendering happens.
    pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_SRT, CKelvinAAState::ACCESS_WRITE);
    pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_SZB, CKelvinAAState::ACCESS_WRITE);

#ifdef NV_NULL_BLEND

    if (((pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_MISC_STATE)) || (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_COMBINERS_COLOR) || (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_TEXTURE_STATE)) {

    pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]           = D3DBLEND_ONE;
    pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]          = D3DBLEND_ZERO;
    DWORD dwLODBias0 = nvTranslateLODBias (0);
    pContext->dwRenderState[D3DRENDERSTATE_ALPHAFUNC]          = D3DCMP_ALWAYS;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]   = FALSE;
    for (int i = 0; i < 8; i++) {
#ifdef NV_NULL_TEXTURES
        pContext->tssState[i].dwValue[D3DTSS_TEXTUREMAP]      = 0;
        pContext->tssState[i].dwValue[D3DTSS_ADDRESSU]        = D3DTADDRESS_WRAP;
        pContext->tssState[i].dwValue[D3DTSS_ADDRESSV]        = D3DTADDRESS_WRAP;
        pContext->tssState[i].dwValue[D3DTSS_MAGFILTER]       = D3DTFG_POINT;
        pContext->tssState[i].dwValue[D3DTSS_MINFILTER]       = D3DTFN_POINT;
        pContext->tssState[i].dwValue[D3DTSS_MIPFILTER]       = D3DTFP_POINT;
        pContext->tssState[i].dwValue[D3DTSS_COLOROP]         = D3DTOP_DISABLE;
        pContext->tssState[i].dwValue[D3DTSS_COLORARG1]       = D3DTA_TEXTURE;
        pContext->tssState[i].dwValue[D3DTSS_COLORARG2]       = D3DTA_CURRENT;
#endif
        pContext->tssState[i].dwValue[D3DTSS_ALPHAOP]         = D3DTOP_DISABLE;
        pContext->tssState[i].dwValue[D3DTSS_ALPHAARG1]       = D3DTA_TEXTURE;
        pContext->tssState[i].dwValue[D3DTSS_ALPHAARG2]       = D3DTA_CURRENT;
#ifdef NV_NULL_TEXTURES
        pContext->tssState[i].dwValue[D3DTSS_BUMPENVMAT00]    = 0;
        pContext->tssState[i].dwValue[D3DTSS_BUMPENVMAT01]    = 0;
        pContext->tssState[i].dwValue[D3DTSS_BUMPENVMAT10]    = 0;
        pContext->tssState[i].dwValue[D3DTSS_BUMPENVMAT11]    = 0;
        pContext->tssState[i].dwValue[D3DTSS_TEXCOORDINDEX]   = 0;
        pContext->tssState[i].dwValue[D3DTSS_BORDERCOLOR]     = 0x00000000;
        pContext->tssState[i].dwValue[D3DTSS_MIPMAPLODBIAS]   = 0;
        pContext->tssState[i].dwValue[D3DTSS_MAXMIPLEVEL]     = 0;
        pContext->tssState[i].dwValue[D3DTSS_MAXANISOTROPY]   = 1;
        pContext->tssState[i].dwValue[D3DTSS_BUMPENVLSCALE]   = 0;
        pContext->tssState[i].dwValue[D3DTSS_BUMPENVLOFFSET]  = 0;
        pContext->tssState[i].dwHandle                        = 0;
        pContext->tssState[i].dwLODBias                       = dwLODBias0;
#endif
    }
#ifdef NV_NULL_TEXTURES
    pContext->tssState[0].dwValue[D3DTSS_COLOROP] = D3DTOP_MODULATE;
    pContext->tssState[0].dwValue[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG1;
    pContext->dwStageCount       = 1;
#endif
    }

#endif

    // clear the failure flag
    pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_SETUPFAILURE;

    // if ddraw has sullied the kelvin object or if we last programmed
    // the kelvin object for a different context, start from scratch
    if ((pDriverData->dwMostRecentHWUser != MODULE_ID_D3D) ||
        (getDC()->dwLastHWContext        != (DWORD)pContext))
    {
        pDriverData->dwMostRecentHWUser = MODULE_ID_D3D;
        getDC()->dwLastHWContext        = (DWORD)pContext;
        pContext->hwState.dwDirtyFlags  = KELVIN_DIRTY_REALLY_FILTHY;
    }

    // apply out-of-context dirty flags
    // this was here because of aliasing to the celsius object.
    // lord willing we won't have the same problem with kelvin, but...
    //pContext->hwState.dwDirtyFlags |= pDriverData->dwOutOfContextKelvinDirtyFlags;
    //pDriverData->dwOutOfContextKelvinDirtyFlags = 0;

    pContext->hwState.dwDirtyFlags |= dbgForceHWRefresh;

    // mask out any non-kelvin bits
    pContext->hwState.dwDirtyFlags &= KELVIN_DIRTY_REALLY_FILTHY;

    // if nothing is dirty, just return
    if (!(pContext->hwState.dwDirtyFlags)) {
        return D3D_OK;
    }

#ifdef NV_PROFILE_CALLSTACK
    NVP_START (NVP_T_SETKELVINSTATE);
#endif

    // fundamental D3D stuff that doesn't normally change
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_DEFAULTS) {
#ifdef PROFILE_STATE_CHANGES
        state_count[0]++;
#endif
        nvSetKelvinD3DDefaults (pContext);
    }

    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_SURFACE) {
#ifdef PROFILE_STATE_CHANGES
        state_count[1]++;
#endif
        nvSetKelvinSurfaceInfo (pContext);
    }

    // program the color combiners early since they determine the mapping
    // between hardware texture units and D3D texture stages, upon
    // which tons of other stuff depends.
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_COMBINERS_COLOR) {
#ifdef PROFILE_STATE_CHANGES
        state_count[2]++;
#endif
        nvSetKelvinColorCombiners (pContext);
    }

    // If a pixel shader is enabled, use it to update the state
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_PIXEL_SHADER) {
#ifdef PROFILE_STATE_CHANGES
        state_count[3]++;
#endif
        nvSetKelvinPixelShader (pContext);
    }

    // texture state
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_TEXTURE_STATE) {
#ifdef PROFILE_STATE_CHANGES
        state_count[4]++;
#endif
        nvSetKelvinTextureState (pContext);
    }

    // fog / specular combiner
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_COMBINERS_SPECFOG) {
#ifdef PROFILE_STATE_CHANGES
        state_count[5]++;
#endif
        nvSetKelvinSpecularFogCombiner (pContext);
    }

    // vertex shader
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_VERTEX_SHADER) {
#ifdef PROFILE_STATE_CHANGES
        state_count[6]++;
#endif
        nvSetKelvinVertexShader (pContext);
    }

    // texture transform
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_TEXTURE_TRANSFORM) {
#ifdef PROFILE_STATE_CHANGES
        state_count[7]++;
#endif
        nvSetKelvinTextureTransform (pContext);
    }

    // lighting
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_LIGHTS) {
#ifdef PROFILE_STATE_CHANGES
        state_count[8]++;
#endif
        nvSetKelvinLights (pContext);
    }

    // transform
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_TRANSFORM) {
#ifdef PROFILE_STATE_CHANGES
        state_count[9]++;
#endif
        nvSetKelvinTransform (pContext);
    }

    // control0
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_CONTROL0) {
#ifdef PROFILE_STATE_CHANGES
        state_count[10]++;
#endif
        nvSetKelvinControl0 (pContext);
    }

    // remaining random stuff
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_MISC_STATE) {
#ifdef PROFILE_STATE_CHANGES
        state_count[11]++;
#endif
        nvSetKelvinMiscState (pContext);
    }

    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_FVF) {
#ifdef PROFILE_STATE_CHANGES
        state_count[12]++;
#endif
        nvSetKelvinVertexFormat (pContext);
    }

#ifdef NV_PROFILE_CALLSTACK
    NVP_STOP(NVP_T_SETKELVINSTATE);
    nvpLogTime(NVP_T_SETKELVINSTATE,nvpTime[NVP_T_SETKELVINSTATE]);
#endif

    // make sure we're not dirty anymore. (this might happen if
    // there are interdepencies among the kelvin state routines
    // and the routines get called in the 'wrong' order)
    nvAssert (!(pContext->hwState.dwDirtyFlags));

#ifdef NV_PROFILE_DP2OPS
    pDriverData->pBenchmark->endHWStateChange(pDriverData->nvPusher.m_dwPut);
#endif

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set only the kelvin state required to clear

HRESULT __stdcall nvSetKelvinClearState (PNVD3DCONTEXT pContext)
{
    // if ddraw has sullied the kelvin object or if we last programmed
    // the kelvin object for a different context, start from scratch
    if ((pDriverData->dwMostRecentHWUser != MODULE_ID_D3D) ||
        (getDC()->dwLastHWContext        != (DWORD)pContext))
    {
        pDriverData->dwMostRecentHWUser = MODULE_ID_D3D;
        getDC()->dwLastHWContext        = (DWORD)pContext;
        pContext->hwState.dwDirtyFlags  = KELVIN_DIRTY_REALLY_FILTHY;
    }

    pContext->hwState.dwDirtyFlags |= dbgForceHWRefresh;

    // mask out any non-kelvin bits
    pContext->hwState.dwDirtyFlags &= KELVIN_DIRTY_REALLY_FILTHY;

#ifdef NV_PROFILE_CALLSTACK
    NVP_START (NVP_T_SETKELVINSTATE);
#endif

    // fundamental D3D stuff that doesn't normally change
    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_DEFAULTS) {
        nvSetKelvinD3DDefaults (pContext);
    }

    if (pContext->hwState.dwDirtyFlags & KELVIN_DIRTY_SURFACE) {
        nvSetKelvinSurfaceInfo (pContext);
    }

#ifdef NV_PROFILE_CALLSTACK
    NVP_STOP(NVP_T_SETKELVINSTATE);
    nvpLogTime(NVP_T_SETKELVINSTATE,nvpTime[NVP_T_SETKELVINSTATE]);
#endif

    return (D3D_OK);
}

#endif // (NVARCH >= 0x20)

