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
//  Module: nvCelsiusState.cpp
//      Celsius state management routines.
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        12Feb99         NV10 development
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

#include "nvCelsiusCaps.h"

// all values in this matrix are constant except 3,1 and 4,1 which get correctly set later
// column 1 = vector by which [x,y,z,w] is scaled
// column 2 = vector by which [x,y,z] are biased
static D3DMATRIX mPassthruModelViewMatrix =
{
     1.f,   CELSIUS_BORDER,   0.f,   0.f,
     1.f,   CELSIUS_BORDER,   0.f,   0.f,
     0.f,   0.f,           0.f,   0.f,
     0.f,   0.f,           0.f,   0.f
};

// all elements of this matrix remain zero except the last row and the diagonal
// which get set appropriately in nvSetCelsiusTransform
static D3DMATRIX mViewportMatrix = matrixZero;

//---------------------------------------------------------------------------

// set things that are never changed in D3D. this routine is run once to initialize
// and then only whenever someone else (DDRAW) stomps on our celsius object

HRESULT nvSetCelsiusD3DDefaults (PNVD3DCONTEXT pContext)
{
    DWORD dwWindow;

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_DEFAULTS;

    // context dmas
    pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_NOTIFIES,NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY); // notifier context
    pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_A,       D3D_CONTEXT_DMA_HOST_MEMORY);                          // dma a - system/agp memory
    pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_B,       NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);                // dma b - video memory
    pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_VERTEX,  NV01_NULL_OBJECT);                                     // vertex buffer context (gets set later)
    pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_STATE,   NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);                // state context
    pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_COLOR,   NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);                // frame buffer context
    pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_ZETA,    NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);                // zeta buffer context

    // use the first vertex to determine the color used for flat-shading
    pContext->hwState.celsius.set (NV056_SET_FLAT_SHADE_OP,   NV056_SET_FLAT_SHADE_OP_V_FIRST_VTX);
    pContext->hwState.celsius.prep_wait_for_idle_nv10();
    pContext->hwState.celsius.set (NV056_SET_WINDOW_CLIP_TYPE,NV056_SET_WINDOW_CLIP_TYPE_V_INCLUSIVE);

    // deactivate all but the first clip rectangle
    pContext->hwState.celsius.prep_wait_for_idle_nv10();
    for (dwWindow=1; dwWindow<CELSIUS_MAX_WINDOWS; dwWindow++) {
        pContext->hwState.celsius.set (NV056_SET_WINDOW_CLIP_HORIZONTAL(dwWindow),0);
        pContext->hwState.celsius.set (NV056_SET_WINDOW_CLIP_VERTICAL(dwWindow),0);
    }

    pContext->hwState.celsius.set (NV056_SET_POINT_PARAMS_ENABLE, NV056_SET_POINT_PARAMS_ENABLE_V_FALSE);

    pContext->hwState.celsius.set (NV056_SET_BLEND_EQUATION,      NV056_SET_BLEND_EQUATION_V_FUNC_ADD);
    pContext->hwState.celsius.set (NV056_SET_BLEND_COLOR,         0);

    pContext->hwState.celsius.set (NV056_SET_EDGE_FLAG,  1);

    pContext->hwState.celsius.set4f (NV056_SET_TEXCOORD0_4F(0), 0.0f, 0.0f, 0.0f, 1.0f);
    pContext->hwState.celsius.set4f (NV056_SET_TEXCOORD1_4F(0), 0.0f, 0.0f, 0.0f, 1.0f);

    pContext->hwState.celsius.set3f (NV056_SET_NORMAL3F(0), 0.0f, 0.0f, 1.0f);

    pContext->hwState.celsius.setf (NV056_SET_FOG1F, 0.0f);

    // we never have fog arrays
    pContext->hwState.celsius.set (NV056_SET_FOG_ARRAY_OFFSET, 0);
    pContext->hwState.celsius.set (NV056_SET_FOG_ARRAY_FORMAT,DRF_DEF(056, _SET_FOG_ARRAY_FORMAT, _SIZE, _0) |
                                                           DRF_DEF(056, _SET_FOG_ARRAY_FORMAT, _TYPE, _FLOAT));

    pContext->hwState.celsius.set (NV056_SET_TEXTURE_CONTROL1(0), DRF_NUM(056, _SET_TEXTURE_CONTROL1, _IMAGE_PITCH, 0x0008));
    pContext->hwState.celsius.set (NV056_SET_TEXTURE_CONTROL1(1), DRF_NUM(056, _SET_TEXTURE_CONTROL1, _IMAGE_PITCH, 0x0008));
    pContext->hwState.celsius.set (NV056_SET_TEXTURE_CONTROL2(0),(DRF_NUM(056, _SET_TEXTURE_CONTROL2, _IMAGE_LODF, 0) |
                                                               DRF_NUM(056, _SET_TEXTURE_CONTROL2, _PERTURB_DU, 0) |
                                                               DRF_NUM(056, _SET_TEXTURE_CONTROL2, _PERTURB_DV, 0)));
    pContext->hwState.celsius.set (NV056_SET_TEXTURE_CONTROL2(1),(DRF_NUM(056, _SET_TEXTURE_CONTROL2, _IMAGE_LODF, 0) |
                                                               DRF_NUM(056, _SET_TEXTURE_CONTROL2, _PERTURB_DU, 0) |
                                                               DRF_NUM(056, _SET_TEXTURE_CONTROL2, _PERTURB_DV, 0)));

    // D3D has no texgen (yet)
    pContext->hwState.celsius.set (NV056_SET_TEXGEN_S(0), NV056_SET_TEXGEN_S_V_DISABLE);
    pContext->hwState.celsius.set (NV056_SET_TEXGEN_T(0), NV056_SET_TEXGEN_T_V_DISABLE);
    pContext->hwState.celsius.set (NV056_SET_TEXGEN_R(0), NV056_SET_TEXGEN_Q_V_DISABLE);
    pContext->hwState.celsius.set (NV056_SET_TEXGEN_Q(0), NV056_SET_TEXGEN_R_V_DISABLE);
    pContext->hwState.celsius.set (NV056_SET_TEXGEN_S(1), NV056_SET_TEXGEN_S_V_DISABLE);
    pContext->hwState.celsius.set (NV056_SET_TEXGEN_T(1), NV056_SET_TEXGEN_T_V_DISABLE);
    pContext->hwState.celsius.set (NV056_SET_TEXGEN_R(1), NV056_SET_TEXGEN_Q_V_DISABLE);
    pContext->hwState.celsius.set (NV056_SET_TEXGEN_Q(1), NV056_SET_TEXGEN_R_V_DISABLE);

    // D3D has no smoothing (note smoothing != anti-aliasing!)
    pContext->hwState.celsius.set (NV056_SET_POINT_SMOOTH_ENABLE, NV056_SET_POINT_SMOOTH_ENABLE_V_FALSE);
    pContext->hwState.celsius.set (NV056_SET_LINE_SMOOTH_ENABLE,  NV056_SET_LINE_SMOOTH_ENABLE_V_FALSE);
    pContext->hwState.celsius.set (NV056_SET_POLY_SMOOTH_ENABLE,  NV056_SET_POLY_SMOOTH_ENABLE_V_FALSE);

    // swath width for the boustrophedonic rasterization
    // todo - use montrym's algorithm
    pContext->hwState.celsius.set (NV056_SET_SWATH_WIDTH, NV056_SET_SWATH_WIDTH_V_64);

    pContext->hwState.celsius.set4f (NV056_SET_EYE_POSITION(0), 0.0f, 0.0f, 0.0f, 1.0f);

    // we'll always cull what we call backfacing geometry.
    // this goes along with D3D's definition of culling.
    pContext->hwState.celsius.set (NV056_SET_CULL_FACE, NV056_SET_CULL_FACE_V_BACK);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set surface info: offset, pitch, clip

HRESULT nvSetCelsiusSurfaceInfo (PNVD3DCONTEXT pContext)
{
    DWORD dwClipH, dwClipV;  // horizontal and vertical clip locations

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_SURFACE;

    //
    // get render targets
    //
    CSimpleSurface *pRenderTarget;
    CSimpleSurface *pZetaBuffer;


    if (pContext->aa.isEnabled() && pContext->aa.isRenderTargetValid()) {
        pContext->aa.makeRenderTargetValid(pContext);
        pRenderTarget = pContext->pRenderTarget;
    } else if (!pContext->aa.isEnabled()) {
        pRenderTarget = pContext->pRenderTarget;
    } else {
        pRenderTarget = pContext->aa.pSuperRenderTarget;
    }

    if (pContext->aa.isEnabled() && pContext->aa.isZetaBufferValid()) {
        pContext->aa.makeZetaBufferValid(pContext);
        pZetaBuffer   = pContext->pZetaBuffer;
    } else if (!pContext->aa.isEnabled()) {
        pZetaBuffer   = pContext->pZetaBuffer;
    } else {
        pZetaBuffer   = pContext->aa.pSuperZetaBuffer;
    }


    if ((pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CT_11M) && pZetaBuffer && (pZetaBuffer->getBPP() == 4) && 
        ((pContext->dwEarlyCopyStrategy == 2) || (pContext->dwEarlyCopyStrategy > 3)) &&
        !(pContext->aa.isEnabled() && (pContext->dwEarlyCopyStrategy > 9)))
    {
        pZetaBuffer->recreate(2);
    }


    //
    // program HW
    //
    nvAssert (pRenderTarget);
    DWORD dwFormat  = DRF_DEF(056, _SET_SURFACE_FORMAT, _TYPE, _PITCH);
    if (pRenderTarget->isSwizzled())
    {
        CTexture* pTexture = pRenderTarget->getWrapper()->getTexture();
        nvAssert(pTexture);
        dwFormat  = DRF_DEF(056, _SET_SURFACE_FORMAT, _TYPE, _SWIZZLE);
        dwFormat |= DRF_NUM(056, _SET_SURFACE_FORMAT, _WIDTH, pTexture->getLogWidth());
        dwFormat |= DRF_NUM(056, _SET_SURFACE_FORMAT, _HEIGHT, pTexture->getLogHeight());
    }
    nvAssert(nv056SurfaceFormat[pRenderTarget->getFormat()] != ~0);
    dwFormat |= nv056SurfaceFormat[pRenderTarget->getFormat()];
    if (pZetaBuffer && (pZetaBuffer->getBPP() == 2) && (pRenderTarget->getBPP() == 4))
    {
        // NV11 mixed mode support
        nvAssert(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_1196_CELSIUS);
        dwFormat |= NV1196_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5 ^
                    NV1196_SET_SURFACE_FORMAT_COLOR_LE_Z16_X1R5G5B5_Z1R5G5B5;
    }

    DWORD dwZPitch  = pZetaBuffer ? pZetaBuffer->getPitch()  : CELSIUS_ALIGNMENT_MIN;
#ifdef  STEREO_SUPPORT
    DWORD dwZOffset = pZetaBuffer ? GetStereoOffset(pZetaBuffer) : 0;
#else   //STEREO_SUPPORT==0
    DWORD dwZOffset = pZetaBuffer ? pZetaBuffer->getOffset() : 0;
#endif  //STEREO_SUPPORT
    pContext->hwState.celsius.prep_wait_for_idle_nv10();
    pContext->hwState.celsius.set (NV056_SET_SURFACE_FORMAT,       dwFormat);
    pContext->hwState.celsius.set (NV056_SET_SURFACE_PITCH,        (DRF_NUM(056, _SET_SURFACE_PITCH, _COLOR, pRenderTarget->getPitch()) |
                                                                 DRF_NUM(056, _SET_SURFACE_PITCH, _ZETA,  dwZPitch)));
#ifdef  STEREO_SUPPORT
    pContext->hwState.celsius.set (NV056_SET_SURFACE_COLOR_OFFSET, GetStereoOffset(pRenderTarget));
#else   //STEREO_SUPPORT==0
    pContext->hwState.celsius.set (NV056_SET_SURFACE_COLOR_OFFSET, pRenderTarget->getOffset());
#endif  //STEREO_SUPPORT
    pContext->hwState.celsius.set (NV056_SET_SURFACE_ZETA_OFFSET,  dwZOffset);

    // since we're using the window clip to clip to the viewport,
    // the surface clip can just clip to the whole window.
    dwClipH = DRF_NUM(056, _SET_SURFACE_CLIP_HORIZONTAL, _X, 0) |
              DRF_NUM(056, _SET_SURFACE_CLIP_HORIZONTAL, _WIDTH, ((DWORD)pRenderTarget->getWidth()));
    dwClipV = DRF_NUM(056, _SET_SURFACE_CLIP_VERTICAL, _Y, 0) |
              DRF_NUM(056, _SET_SURFACE_CLIP_VERTICAL, _HEIGHT, ((DWORD)pRenderTarget->getHeight()));
    pContext->hwState.celsius.prep_wait_for_idle_nv10();
    pContext->hwState.celsius.set (NV056_SET_SURFACE_CLIP_HORIZONTAL, dwClipH);
    pContext->hwState.celsius.set (NV056_SET_SURFACE_CLIP_VERTICAL,   dwClipV);

    if (pZetaBuffer) {
        if (pZetaBuffer->getBPP() == 2) {
            pContext->hwState.dvZScale    = CELSIUS_Z_SCALE16;
            pContext->hwState.dvInvZScale = CELSIUS_Z_SCALE16_INV;
        }
        else {
            pContext->hwState.dvZScale    = CELSIUS_Z_SCALE24;
            pContext->hwState.dvInvZScale = CELSIUS_Z_SCALE24_INV;
        }
    }

    return (D3D_OK);
}


//---------------------------------------------------------------------------

// set up the combiners to produce one of the legacy D3D texture blends

HRESULT nvCelsiusSetTextureBlend (PNVD3DCONTEXT pContext)
{
    DWORD dwTBlend;

    if (pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]) {

        // we have a texture. go ahead and do the blend

        dwTBlend = pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND];

        nvAssert (dwTBlend <= D3D_TBLEND_MAX);  // make sure we're not off the end of the table

        pContext->hwState.dwAlphaICW[0] = celsiusTextureBlendSettings[dwTBlend][0];
        pContext->hwState.dwColorICW[0] = celsiusTextureBlendSettings[dwTBlend][1];
        pContext->hwState.dwAlphaOCW[0] = celsiusTextureBlendSettings[dwTBlend][2];
        pContext->hwState.dwColorOCW[0] = celsiusTextureBlendSettings[dwTBlend][3];

        // handle the stupid special case in which we have TBLEND_MODULATE and a
        // texture without alpha; use D3DTBLEND_MODULATEALPHA instead
        if (dwTBlend == D3DTBLEND_MODULATE) {
            CTexture *pTexture = ((CNvObject *)(pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]))->getTexture();
            if (!pTexture->hasAlpha()) {
                pContext->hwState.dwAlphaICW[0] = celsiusTextureBlendSettings[D3DTBLEND_MODULATEALPHA][0];
            }
        }

        pContext->hwState.dwTexUnitToTexStageMapping[0] = 0;

    }

    else {

        // they've enabled a texture blend without a texture. just use diffuse
        pContext->hwState.dwAlphaICW[0] = celsiusTextureBlendDefault[0];
        pContext->hwState.dwColorICW[0] = celsiusTextureBlendDefault[1];
        pContext->hwState.dwAlphaOCW[0] = celsiusTextureBlendDefault[2];
        pContext->hwState.dwColorOCW[0] = celsiusTextureBlendDefault[3];

    }

    pContext->hwState.dwAlphaICW[1] = 0;
    pContext->hwState.dwColorICW[1] = 0;
    pContext->hwState.dwAlphaOCW[1] = 0;
    pContext->hwState.dwColorOCW[1] = DRF_DEF(056, _SET_COMBINER1_COLOR_OCW, _ITERATION_COUNT, _ONE) |
                                          DRF_DEF(056, _SET_COMBINER1_COLOR_OCW, _MUX_SELECT, _LSB);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// define to enable profiling of combiner programs
// #define COMBINER_STATS

// set up the celsius combiners for either the current texture stage state
// or a legacy texture blend

HRESULT nvSetCelsiusColorCombiners (PNVD3DCONTEXT pContext)
{
    BOOL  bStageActive;
    DWORD dwMapping0, dwMapping1, dwStateFlags;
    DWORD dwTSSMask0, dwTSSMask1;

    #ifdef DEBUG
    static DWORD dwActiveStageCounts[2] = {0,0};
    static DWORD dwTotalPrograms = 0;
    static DWORD dwLastMask0, dwLastMask1;
    // masks requiring 2 stages that we already know about and don't want reported again
    // (also modes that could be compacted into one stage but are idiotic and don't deserve it)
    static DWORD dwKnownMasks[19][2] = {
        // uncompactable
        0x90020820, 0xb0220820,
        0x90400400, 0x90410400,
        0x90400400, 0x90411040,
        0x90400400, 0x94410400,
        0x90400400, 0xb4410400,
        0x90400840, 0x90410400,
        0x90400840, 0xb4410c01,
        0x90401040, 0x90411040,
        0x90401040, 0x94411c41,
        0x90401043, 0xc0411c41,
        0x90400c00, 0x10410840,
        0x90410400, 0x90411040,
        0x90410840, 0x90410400,
        0x90620840, 0x90220820,
        0xb8403840, 0x9c200c00,
        0xb8403840, 0x9c201c20,
        0xe0440840, 0x90410400,
        // idiotic
        0x10401040, 0x10411041,
        0x10401040, 0x34410400
    };
    #endif  // DEBUG

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_COMBINERS;

    // cache current values
    dwStateFlags = pContext->hwState.dwStateFlags & CELSIUS_MASK_COMBINERPROGRAMUNIQUENESS;
    dwMapping0   = pContext->hwState.dwTexUnitToTexStageMapping[0];
    dwMapping1   = pContext->hwState.dwTexUnitToTexStageMapping[1];

    // clear state that will be re-determined here
    pContext->hwState.dwNextAvailableTextureUnit = 0;
    pContext->hwState.dwTexUnitToTexStageMapping[0] = CELSIUS_UNUSED;
    pContext->hwState.dwTexUnitToTexStageMapping[1] = CELSIUS_UNUSED;
    pContext->hwState.dwStateFlags &= ~(CELSIUS_FLAG_DOTPRODUCT3(0)        |
                                        CELSIUS_FLAG_DOTPRODUCT3(1)        |
                                        CELSIUS_FLAG_ADDCOMPLEMENTRGB(0)   |
                                        CELSIUS_FLAG_ADDCOMPLEMENTRGB(1)   |
                                        CELSIUS_FLAG_ADDCOMPLEMENTALPHA(0) |
                                        CELSIUS_FLAG_ADDCOMPLEMENTALPHA(1));

    if (pContext->bUseTBlendSettings) {

        nvCelsiusSetTextureBlend (pContext);
        pContext->hwState.dwNumActiveCombinerStages = 1;

    }

    else {

#ifdef CELSIUS_NVCOMBINER
        // count the number of texture stages
        DWORD dwStageCount = 0;
        DWORD dwTexCount = 0;
        while ((dwStageCount < 8) &&
               (pContext->tssState[dwStageCount].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE))
        {
            if(pContext->tssState[dwStageCount].dwValue[D3DTSS_TEXTUREMAP]) dwTexCount++;
            dwStageCount++;
        }
        if (dwStageCount > 2) {
            switch (dwStageCount) {
                case 3:
                    // a very Special case required from developer
                    // disable for now, since not functional
                    /* if (nvCelsiusCheck3StageSpecial (pContext)) {
                        nvCelsiusSet3StageSpecial(pContext);
                    } else
                    */
                    // check for special 3-stage, 2-texture setting
                    if (nvCelsiusCheck3Stage2Textures (pContext)) {
                        nvCelsiusSetTextures3StageCombiners (pContext);
                    } else {
                        pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                        return (D3D_OK);
                    }
                    break;
                case 4:
                    // check for special 4-stage bumpmapping
                    if (nvCheckBumpMapStates (pContext, dwStageCount)) {
                        nvSetCelsius4StageBumpMapCombiners (pContext);
                    } else {
                        pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                        return (D3D_OK);
                    }
                    break;
                case 8:
                    // check for special 8-stage bumpmapping
                    if (((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_DIRECTMAPENABLE_MASK) == D3D_REG_DIRECTMAPENABLE_ENABLE) &&
                        (nvCheckBumpMapStates (pContext,dwStageCount))) {
                        nvSetCelsius8StageBumpMapCombiners (pContext);
                    } else {
                        pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                        return (D3D_OK);
                    }
                    break;
                default:
                    DPF ("unhandled number of combiner stages");
                    pContext->hwState.dwStateFlags |=CELSIUS_FLAG_SETUPFAILURE;
                    return (D3D_OK);
            }
        }

        // see if we can pack two texture stages into one combiner
        else if ((dwStageCount==2) &&
                 (nvCelsiusCheck2Stage1Combiner(pContext)) &&
                 (nvCelsiusSet2Stage1Combiner(pContext) == D3D_OK)) {
            // everything's ok. nothing more to do
        }
        else
#endif
        {
            dwTSSMask0 = nvCelsiusBuildTSSMask (pContext, 0);
            dwTSSMask1 = nvCelsiusBuildTSSMask (pContext, 1);

            if (!nvCelsiusCombinersSetFromHashTable (pContext, dwTSSMask0, dwTSSMask1)) {

                // program the old-fashioned way
                pContext->hwState.dwNumActiveCombinerStages = 1;  // we always have at least one stage

                bStageActive = ConstructColorCombiners (pContext, 0, 0);
                ConstructAlphaCombiners (pContext, 0, 0, bStageActive);

                // if the current unit got assigned to this stage, move on to the next
                if (pContext->hwState.dwTexUnitToTexStageMapping[pContext->hwState.dwNextAvailableTextureUnit] != CELSIUS_UNUSED) {
                    pContext->hwState.dwNextAvailableTextureUnit++;
                }

                if (bStageActive) {

                    // if the first stage was active, go ahead and process the second
                    bStageActive = ConstructColorCombiners (pContext, 1, 1);
                    ConstructAlphaCombiners (pContext, 1, 1, bStageActive);

                    if (bStageActive) {
                        pContext->hwState.dwNumActiveCombinerStages = 2;
                    }

                }  // bStageActive

                else {

                    pContext->hwState.dwColorICW[1] = 0;
                    pContext->hwState.dwColorOCW[1] = 0;
                    pContext->hwState.dwAlphaICW[1] = 0;
                    pContext->hwState.dwAlphaOCW[1] = 0;

                }

                pContext->hwState.dwColorOCW[1] |= DRF_NUM(056, _SET_COMBINER1_COLOR, _OCW_ITERATION_COUNT,
                                                               pContext->hwState.dwNumActiveCombinerStages);

                // add this program to the hash table for future reference
                //only add it if it didn't fail.
                if (!(pContext->hwState.dwStateFlags & CELSIUS_FLAG_SETUPFAILURE))
                {
                    nvCelsiusCombinersAddToHashTable (pContext, dwTSSMask0, dwTSSMask1);
                }
            }  // !nvCelsiusCombinersSetFromHashTable

            #ifdef DEBUG
            dwActiveStageCounts[pContext->hwState.dwNumActiveCombinerStages-1] ++;
            if (pContext->hwState.dwNumActiveCombinerStages == 2) {
                // try to cut down on complaints a bit
                if ((dwTSSMask0 != dwLastMask0) ||
                    (dwTSSMask1 != dwLastMask1)) {
                    dwLastMask0 = dwTSSMask0;
                    dwLastMask1 = dwTSSMask1;
                    BOOL bKnown = FALSE;
                    for (DWORD i=0; i<(sizeof(dwKnownMasks)>>3); i++) {
                        if ((dwKnownMasks[i][0] == dwTSSMask0) &&
                            (dwKnownMasks[i][1] == dwTSSMask1)) {
                            bKnown = TRUE;
                            break;
                        }
                    }
                    if (!bKnown) {
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"using two combiner stages in unknown mode.");
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"please give the following information to craig.");
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   TSS mask 0 = 0x%08x", dwTSSMask0);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   TSS mask 1 = 0x%08x", dwTSSMask1);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   color icw[0] = 0x%08x", pContext->hwState.dwColorICW[0]);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   color ocw[0] = 0x%08x", pContext->hwState.dwColorOCW[0]);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   alpha icw[0] = 0x%08x", pContext->hwState.dwAlphaICW[0]);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   alpha ocw[0] = 0x%08x", pContext->hwState.dwAlphaOCW[0]);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   color icw[1] = 0x%08x", pContext->hwState.dwColorICW[1]);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   color ocw[1] = 0x%08x", pContext->hwState.dwColorOCW[1]);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   alpha icw[1] = 0x%08x", pContext->hwState.dwAlphaICW[1]);
                        DPF_LEVEL(NVDBG_LEVEL_INFO,"   alpha ocw[1] = 0x%08x", pContext->hwState.dwAlphaOCW[1]);
//                        __asm int 3;  comment out so others can use debug builds without hitting int 3
                    }
                }
            }
            #ifdef COMBINER_STATS
            dwTotalPrograms ++;
            if (dwTotalPrograms % 1000 == 0) {
                DPF ("----------------- combiner stats -------------------");
                DPF ("instances of 1 active stages: %d", dwActiveStageCounts[0]);
                DPF ("             2 active stages: %d", dwActiveStageCounts[1]);
                DPF ("total combiner programs: %d", dwTotalPrograms);
            }
            #endif  // COMBINER_STATS
            #endif  // DEBUG
        }
    }

#if 0
    // force combiner settings
    pContext->hwState.dwColorICW[0] = 0x20200000;
    pContext->hwState.dwColorOCW[0] = 0x00000c00;
    pContext->hwState.dwAlphaICW[0] = 0x30300000;
    pContext->hwState.dwAlphaOCW[0] = 0x00000c00;

    pContext->hwState.dwColorICW[1] = 0x00000000;
    pContext->hwState.dwColorOCW[1] = 0x10000000;
    pContext->hwState.dwAlphaICW[1] = 0x00000000;
    pContext->hwState.dwAlphaOCW[1] = 0x00000000;
#endif

    pContext->hwState.celsius.set (NV056_SET_COMBINER_COLOR_ICW(0), pContext->hwState.dwColorICW[0]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER0_COLOR_OCW,   pContext->hwState.dwColorOCW[0]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_ICW(0), pContext->hwState.dwAlphaICW[0]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_OCW(0), pContext->hwState.dwAlphaOCW[0]);

    pContext->hwState.celsius.set (NV056_SET_COMBINER_COLOR_ICW(1), pContext->hwState.dwColorICW[1]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER1_COLOR_OCW,   pContext->hwState.dwColorOCW[1]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_ICW(1), pContext->hwState.dwAlphaICW[1]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_OCW(1), pContext->hwState.dwAlphaOCW[1]);

    // if we've changed the flags or mappings, invalidate a bunch of state
    if ((pContext->hwState.dwStateFlags & CELSIUS_MASK_COMBINERPROGRAMUNIQUENESS) != dwStateFlags) {
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_SPECFOG_COMBINER |
                                          CELSIUS_DIRTY_LIGHTS;
    }

    if ((pContext->hwState.dwTexUnitToTexStageMapping[0] != dwMapping0) ||
        (pContext->hwState.dwTexUnitToTexStageMapping[1] != dwMapping1)) {
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_STATE
                                       |  CELSIUS_DIRTY_TEXTURE_TRANSFORM
                                       |  CELSIUS_DIRTY_TL_MODE
                                       |  CELSIUS_DIRTY_FVF
                                       |  CELSIUS_DIRTY_TRANSFORM;
    }

#ifdef ALPHA_CULL
    if(pContext->dwEarlyCopyStrategy && pContext->hwState.alphacull_mode >= 2)
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_MISC_STATE;
#endif

    return D3D_OK;
}


//---------------------------------------------------------------------------

// set up all the fundamental stuff that can be derived from the texture
// itself (without looking at renderstate, texture stage state, etc.)
// these are values that are handled identically for real user textures,
// the default texture, and the texture used for user clip planes, so we
// consolidate the code here.

__inline void nvCelsiusSetTextureBasics (CTexture *pTexture, DWORD *dwControl0,
                                         DWORD *dwFormatMask, DWORD *dwFilterMask)
{
    *dwControl0    = pTexture->getNV056Control0();
    *dwFormatMask  = pTexture->getNV056Format();
    *dwFilterMask  = 0;
}

//---------------------------------------------------------------------------

// OR in additional default texture stuff. these are values that are common
// to the default white texture and the clip plane texture but NOT real
// user textures. for the latter, they are contingent on other state settings.

__inline void nvCelsiusAddTextureDefaults (CTexture *pTexture,
                                           DWORD *dwControl0, DWORD *dwFormatMask,
                                           DWORD *dwFilterMask, DWORD *dwOffset)
{
    // these are backward from openGL. d3d can only set the maximum mipmap level, by which they mean
    // the openGL minimum level (d3d maximum = largest sized level to use, not largest level number).
    // the d3d minimum level (the openGL / celsius maximum) is always just the end of the chain
    *dwControl0 |= DRF_NUM(056, _SET_TEXTURE_CONTROL0, _MIN_LOD_CLAMP,
                           ((pTexture->getMipMapCount()-1) << 8));  // 4.8 fixed point

    *dwControl0 |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);

    *dwFormatMask |= DRF_DEF(056, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, _FALSE)  |

                     DRF_DEF(056, _SET_TEXTURE_FORMAT, _TEXTUREADDRESSU, _CLAMP) |
                     DRF_DEF(056, _SET_TEXTURE_FORMAT, _TEXTUREADDRESSV, _CLAMP) |

                     DRF_DEF(056, _SET_TEXTURE_FORMAT, _WRAPU, _FALSE)           |
                     DRF_DEF(056, _SET_TEXTURE_FORMAT, _WRAPV, _FALSE);

    *dwFilterMask |= DRF_DEF(056, _SET_TEXTURE_FILTER, _TEXTUREMIN, _NEAREST) |
                     DRF_DEF(056, _SET_TEXTURE_FILTER, _TEXTUREMAG, _NEAREST);

    *dwOffset = pTexture->getSwizzled()->getOffset();
}

//---------------------------------------------------------------------------

__inline void nvCelsiusSendTextureData (PNVD3DCONTEXT pContext,
                                        DWORD dwStage, DWORD dwControl0, DWORD dwOffset,
                                        DWORD dwFormatMask, DWORD dwFilterMask)
{
// #define TEXSTATE_STATS
#if defined(DEBUG) && defined(TEXSTATE_STATS)
    static DWORD dwC0Last = 0;
    static DWORD dwOffsetLast = 0;
    static DWORD dwFormatLast = 0;
    static DWORD dwFilterLast = 0;
    static DWORD dwC0Changes = 0;
    static DWORD dwOffsetChanges = 0;
    static DWORD dwFormatChanges = 0;
    static DWORD dwFilterChanges = 0;
    static DWORD dwTotalTexDataSets = 0;
    static DWORD dwLoopTime = 0;
#endif  // DEBUG && TEXSTATE_STATS

    pContext->hwState.celsius.set (NV056_SET_TEXTURE_CONTROL0(dwStage), dwControl0);
    pContext->hwState.celsius.set (NV056_SET_TEXTURE_OFFSET(dwStage),   dwOffset);
    pContext->hwState.celsius.set (NV056_SET_TEXTURE_FORMAT(dwStage),   dwFormatMask);
    pContext->hwState.celsius.set (NV056_SET_TEXTURE_FILTER(dwStage),   dwFilterMask);

#if defined(DEBUG) && defined(TEXSTATE_STATS)
    dwTotalTexDataSets++;
    if (dwControl0 != dwC0Last) {
        dwC0Last = dwControl0;
        dwC0Changes++;
    }
    if (dwOffset != dwOffsetLast) {
        dwOffsetLast = dwOffset;
        dwOffsetChanges++;
    }
    if (dwFormatMask != dwFormatLast) {
        dwFormatLast = dwFormatMask;
        dwFormatChanges++;
    }
    if (dwFilterMask != dwFilterLast) {
        dwFilterLast = dwFilterMask;
        dwFilterChanges++;
    }
    DWORD dwNow = GetTickCount();
    if ((dwNow - dwLoopTime) > 1000) {
        DPF ("----------------- TEX STATS ----------------- dwNow = %d",dwNow);
        dwLoopTime = dwNow;
        DPF ("control0 changes: %d", dwC0Changes);
        DPF ("  offset changes: %d", dwOffsetChanges);
        DPF ("  format changes: %d", dwFormatChanges);
        DPF ("  filter changes: %d", dwFilterChanges);
    }
#endif  // DEBUG && TEXSTATE_STATS
}

//---------------------------------------------------------------------------

HRESULT nvSetCelsiusTexturePalette (PNVD3DCONTEXT pContext, PNVD3DTEXSTAGESTATE pTSSState, DWORD dwHWStage)
{
    CSimpleSurface     *pPalette;
    CNvObject          *pPalObj;
    DWORD               dwCDMA, dwOffset;
    DWORD               dwSystemPalette = 0;    // count of system memory palettes
    DWORD               dwPaletteOffset = 0;

    // NV10 ISSUE:
    // The run-time DMA error STATUS_ERROR_INVALID_STATE (0x01000000) occurs
    // if the DMA controller requests 32-bit data words from system (PCI or AGP) memory
    // This can occur if the following condition was true when a primitive was launched:
    //  (((SetTextureControl0[0].Enable == TRUE) ||
    //    (SetTextureControl0[1].Enable == TRUE))   &&
    //   ((SetTextureFormat[0].Color == I8_A8R8G8B8) ||
    //    (SetTextureFormat[1].Color == I8_A8R8G8B8))   &&
    //   ((SetTexturePalette[0].PaletteContextDMA != SetTexturePalette[1].PaletteContextDMA) ||
    //    (SetTexturePalette[0].PaletteOffset != SetTexturePalette[1].PaletteOffset))   &&
    //   ((MemoryType(SetContextDmaA.handle) == SYSTEM) ||
    //    (MemoryType(SetContextDmaB.handle) == SYSTEM)))
    //   i.e. you can't fetch BOTH palettes from system/agp memory -- we don't have the bandwith.
    //   we can fetch them from video memory however.

    pPalObj = ((CNvObject *)(pTSSState->dwValue[D3DTSS_TEXTUREMAP]))->getPalette();

    if (pPalObj == NULL) {
        DPF ("stupid app/runtime gave us a palettized texture without a palette");
        dbgD3DError();
        return (DDERR_GENERIC);
    }

    pPalette = pPalObj->getPaletteSurface();
    nvAssert (pPalette);   // should already have bound a palette before using this texture

    dwCDMA   = nv056PaletteContextDma[pPalette->getContextDMA()];
    dwOffset = pPalette->getOffset();

    // only accept a VID MEM Palette
    nvAssert ((dwCDMA == NV056_SET_TEXTURE_PALETTE_CONTEXT_DMA_A) ||
              (dwCDMA == NV056_SET_TEXTURE_PALETTE_CONTEXT_DMA_B));
    nvAssert ((dwOffset & 0xff) == 0);

    if (dwCDMA == NV056_SET_TEXTURE_PALETTE_CONTEXT_DMA_B) {
        if (dwSystemPalette == 1 && dwPaletteOffset != dwOffset) {
            // nvAssert(0); // We can't do two system palettes at the same time
            dwOffset = dwPaletteOffset; // rather than faulting in the driver do this gross thing
                                        // of forcing the second palette to match the first.
        }
        else {
            dwSystemPalette = 1;
            dwPaletteOffset=dwOffset;
        }
    }

    // set the HW state. this perverse method wants dwOffset>>6. ours is not to reason why...
    pContext->hwState.celsius.set (NV056_SET_TEXTURE_PALETTE(dwHWStage),
                                (DRF_NUM (056, _SET_TEXTURE_PALETTE, _CONTEXT_DMA, dwCDMA) |
                                 DRF_NUM (056, _SET_TEXTURE_PALETTE, _PALETTE_OFFSET, (dwOffset>>6))));

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set celsius state having to do with textures, including:
//      - formats
//      - filters
//      - offsets
// note: the combiners must be set up before calling this routine!

HRESULT nvSetCelsiusTextureState (PNVD3DCONTEXT pContext)
{
    DWORD               dwHWStage, dwD3DStage, dwTexUnitStatus;
    DWORD               dwTexGen, dwTCIndex, dwVAIndex, dwMod;
    PNVD3DTEXSTAGESTATE pTSSState;
    DWORD               dwControl0, dwFormatMask, dwFilterMask;
    DWORD               dwOffset, dwTexgenMode;
    DWORD               dwTexgenT, dwZEnable;
    DWORD               dwUserCoordsNeeded = 0;
    DWORD               dwTexCoordIndices = 0;
    DWORD               dwNumTexCoordsNeeded = 0;
    DWORD               dwInvMVNeeded = 0;
    CTexture           *pTexture;
    BOOL                bCubeMap;
    BOOL                bWrapU, bWrapV, bWrapP, bWrapQ;
    DWORD               dwWDivideFlags = 0;
    DWORD               dwWSourceFlags = 0;
    BOOL                bWSourceFound = FALSE;
    D3DVALUE            pClipPlaneEye[4];
    D3DVALUE           *pClipPlane1, *pClipPlane2;
    DWORD               dwClipPlaneEnable, dwCurrentClipPlaneIndex;
    D3DMATRIX           mTmp, mTmp2;

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_TEXTURE_STATE;

    dwZEnable = pContext->pZetaBuffer ? pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] : 0;

    dwClipPlaneEnable = pContext->dwRenderState[D3DRENDERSTATE_CLIPPLANEENABLE];
    dwCurrentClipPlaneIndex = 0;

    // iterate over the HW's two texture units

    for (dwHWStage=0; dwHWStage < 2 ; dwHWStage++) {

        // which D3D stage is handled by this HW stage

        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];

        if (dwD3DStage != CELSIUS_UNUSED) {

#ifdef NV_PROFILE_DP2OPS
            pDriverData->pBenchmark->incActiveTextures();
#endif

            // the combiners assigned this texture unit.
            // we must have a legitimate texture to use

            pTSSState = &(pContext->tssState[dwD3DStage]);
            nvAssert(pTSSState->dwValue[D3DTSS_TEXTUREMAP]);

            if (pTSSState->dwValue[D3DTSS_TEXTUREMAP]) {
                pTexture = ((CNvObject *)(pTSSState->dwValue[D3DTSS_TEXTUREMAP]))->getTexture();
                if(!pTexture){
                    //if failed to get a texture but user requested one -- use the default
                    pTexture = getDC()->pDefaultTexture;
                    pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                }
            } else {
                pTexture = getDC()->pDefaultTexture;
            }

            bCubeMap = pTexture->isCubeMap();
            if (bCubeMap) {
                // cubemaps may only be referenced from their bases
                pTexture = pTexture->getBaseTexture();

                if (dwZEnable == D3DZB_USEW) {
                    DPF("switching from W-buffering to Z-buffering b/c cube maps are enabled");
                    pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                    dwZEnable = D3DZB_TRUE;
                }
            }

            nvAssert (pTexture);
            dbgTextureDisplay (pTexture);

            dwNumTexCoordsNeeded |= KELVIN_SET_NUM_TEX_COORDS(dwHWStage, pTexture->getNumTexCoords());

            dwTexUnitStatus = CELSIUS_TEXUNITSTATUS_USER;
            pContext->hwState.pTextureInUse[dwHWStage] = pTexture;

            if (pTexture->isPalettized()) {
                nvSetCelsiusTexturePalette (pContext, pTSSState, dwHWStage);
            }
            else if (pTexture->hasAutoPalette()) {
                nvAssert((pTexture->getAutoPaletteOffset() & 63) == 0);
                // the header file (nv32.h) appears to be wrong here, texture offset is at bit 0
                // so we can't use the expected DRF_NUM(056, _SET_TEXTURE_PALETTE, _PALETTE_OFFSET, pTexture->getAutoPaletteOffset());
                DWORD dwFormat = DRF_NUM(056, _SET_TEXTURE_PALETTE, _CONTEXT_DMA, nv056PaletteContextDma[pTexture->getSwizzled()->getContextDMA()])
                               | (pTexture->getAutoPaletteOffset());
                pContext->hwState.celsius.set (NV056_SET_TEXTURE_PALETTE(dwHWStage), dwFormat);
            }

            // set up basic texture info
            nvCelsiusSetTextureBasics (pTexture, &dwControl0, &dwFormatMask, &dwFilterMask);

            // set up info specific to real user textures...

            dwTexGen  = (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) ?
                        D3DTSS_TCI_PASSTHRU : pTSSState->dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff0000;
            dwTCIndex = pTSSState->dwValue[D3DTSS_TEXCOORDINDEX] & 0x0000ffff;
            dwVAIndex = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];

            // texgen
            switch (dwTexGen) {
                case D3DTSS_TCI_PASSTHRU:
                    if (pContext->pCurrentVShader->bVAExists(dwVAIndex)) {
                        dwUserCoordsNeeded |= CELSIUS_FLAG_USERTEXCOORDSNEEDED(dwHWStage);
                        dwTexCoordIndices  |= dwTCIndex << (16*dwHWStage);
                    }
                    else {
                        // u,v = 0,0   (says MS) W=1.0 THIS IS IMPORTANT (DOH!)
                            DWORD dwBase = NV056_SET_TEXCOORD0_4F(0) + 0x28 * dwHWStage; // macro not designed well
                            pContext->hwState.celsius.set4f (dwBase,0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    dwTexgenMode = NV056_SET_TEXGEN_S_V_DISABLE;
                    break;
                case D3DTSS_TCI_CAMERASPACENORMAL:
                    dwInvMVNeeded = CELSIUS_FLAG_TEXSTATENEEDSINVMV;
                    dwTexgenMode = NV056_SET_TEXGEN_S_V_NORMAL_MAP;
                    break;
                case D3DTSS_TCI_CAMERASPACEPOSITION:
                    dwMod = 0x40 * dwHWStage; // macro not designed well
                    pContext->hwState.celsius.set4f (dwMod + NV056_SET_TEXGEN_SPLANE0(0), 1.0f, 0.0f, 0.0f, 0.0f);
                    pContext->hwState.celsius.set4f (dwMod + NV056_SET_TEXGEN_TPLANE0(0), 0.0f, 1.0f, 0.0f, 0.0f);
                    pContext->hwState.celsius.set4f (dwMod + NV056_SET_TEXGEN_RPLANE0(0), 0.0f, 0.0f, 1.0f, 0.0f);
                    dwTexgenMode = NV056_SET_TEXGEN_S_V_EYE_LINEAR;
                    break;
                case D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR:
                    dwInvMVNeeded = CELSIUS_FLAG_TEXSTATENEEDSINVMV;
                    dwTexgenMode = NV056_SET_TEXGEN_S_V_REFLECTION_MAP;
                    break;
                default:
                    DPF ("unknown texgen mode in nvSetCelsiusTextureState");
                    dbgD3DError();
                    pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                    break;
            }

            // we always generate three coordinates (i think)
            pContext->hwState.celsius.set (NV056_SET_TEXGEN_S(dwHWStage), dwTexgenMode);
            pContext->hwState.celsius.set (NV056_SET_TEXGEN_T(dwHWStage), dwTexgenMode);
            pContext->hwState.celsius.set (NV056_SET_TEXGEN_R(dwHWStage), dwTexgenMode);
            pContext->hwState.celsius.set (NV056_SET_TEXGEN_Q(dwHWStage), NV056_SET_TEXGEN_Q_V_DISABLE);

            // these are backward from openGL. d3d can only set the maximum mipmap level, by which they mean
            // the openGL minimum level (d3d maximum = largest sized level to use, not largest level number).
            // the d3d minimum level (the openGL / celsius maximum) is always just the end of the chain
            dwControl0 |= DRF_NUM(056, _SET_TEXTURE_CONTROL0, _MIN_LOD_CLAMP,
                                  (pTSSState->dwValue[D3DTSS_MAXMIPLEVEL] << 8));  // 4.8 fixed point

            // colorkey
            if ((pContext->dwRenderState[D3DRENDERSTATE_COLORKEYENABLE]) &&
                (pTexture->hasColorKey())) {
                dwControl0 |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _KILL);  // use this choice since the texture may not have alpha
                DWORD dwKey = pTexture->getColorKey();
#ifdef DEBUG
                DWORD dwFailState = 1;
#endif
                if (pTexture->isPalettized()) {
                    CNvObject *pObj = pTexture->getWrapper();
                    if (!pObj) goto failColorKeyPalette;

                    CNvObject *pPalette = pObj->getPalette();
                    if (!pPalette) goto failColorKeyPalette;

                    CSimpleSurface *pPaletteSurface = pPalette->getPaletteSurface();
                    if (!pPaletteSurface) goto failColorKeyPalette;

                    DWORD *dwPalette = (DWORD *)pPaletteSurface->getAddress();
                    if (!dwPalette) goto failColorKeyPalette;

                    if (dwKey >= 256) goto failColorKeyPalette;

                    dwKey = dwPalette[dwKey];
                }
#ifdef DEBUG
                dwFailState = 0;
#endif
failColorKeyPalette:  // We should put some kind of error code in here eventually
#ifdef DEBUG
                if (dwFailState) {
                    __asm int 3;
                    // Palettized
                }
#endif
                pContext->hwState.celsius.set (NV056_SET_COLOR_KEY_COLOR(dwHWStage), dwKey);
            }
            else {
                dwControl0 |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _FALSE);
            }

            // handle cubemapping
            dwFormatMask |= DRF_NUM(056, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, bCubeMap);

            dwFormatMask |= DRF_NUM(056, _SET_TEXTURE_FORMAT, _TEXTUREADDRESSU,
                                    nv056TextureAddress[pTSSState->dwValue[D3DTSS_ADDRESSU]]);
            dwFormatMask |= DRF_NUM(056, _SET_TEXTURE_FORMAT, _TEXTUREADDRESSV,
                                    nv056TextureAddress[pTSSState->dwValue[D3DTSS_ADDRESSV]]);

            // Check cylindrical wrapping (disallow it if we're cubemapping)
            NV_EVAL_CYLINDRICAL_WRAP (pContext, dwTCIndex, bWrapU, bWrapV, bWrapP, bWrapQ);

            if (bWrapU && !bCubeMap) {
                dwFormatMask |= DRF_DEF(056, _SET_TEXTURE_FORMAT, _WRAPU, _TRUE);
            }
            else {
                dwFormatMask |= DRF_DEF(056, _SET_TEXTURE_FORMAT, _WRAPU, _FALSE);
            }

            if (bWrapV && !bCubeMap) {
                dwFormatMask |= DRF_DEF(056, _SET_TEXTURE_FORMAT, _WRAPV, _TRUE);
            }
            else {
                dwFormatMask |= DRF_DEF(056, _SET_TEXTURE_FORMAT, _WRAPV, _FALSE);
            }

            if (!(dwFormatMask & (DRF_DEF(056, _SET_TEXTURE_FORMAT, _WRAPU, _TRUE) |
                                  DRF_DEF(056, _SET_TEXTURE_FORMAT, _WRAPV, _TRUE)))) {
#ifdef CELSIUS_ENABLE_BAD_PERSPECTIVE
                if (pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE])
#endif
                {
                    dwWDivideFlags |= CELSIUS_FLAG_WDIVIDE (dwHWStage);
                    // we can source w from this stage for the purposes of w-buffering if w-divide is enabled
                    // and the texture is non-projective. we can't source w from a projective texture because
                    // the non-unitary q will be multiplied into the w values.
                    if (!(pTSSState->dwValue[D3DTSS_TEXTURETRANSFORMFLAGS] & D3DTTFF_PROJECTED)) {
                        dwWSourceFlags |= CELSIUS_FLAG_WSOURCE (dwHWStage);
                        bWSourceFound = TRUE;
                    }
                }
            }

            DWORD dwMinFilter = pTSSState->dwValue[D3DTSS_MINFILTER];
            DWORD dwMagFilter = pTSSState->dwValue[D3DTSS_MAGFILTER];
            DWORD dwMipFilter = pTSSState->dwValue[D3DTSS_MIPFILTER];

            if (pContext->dwDXAppVersion >= 0x800) {
                // DX8 changed the filter enumerants.  Remap them so we don't have to change our code.
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
                switch (dwMaxAnisotropy) {
                    case 0:
                        // default back to point
                        dwControl0   |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);
                        dwFilterMask |= DRF_NUM(056, _SET_TEXTURE_FILTER, _TEXTUREMIN, nv056TextureMinFilter[D3DTFN_POINT][dwMipFilter]);
                        dwFilterMask |= DRF_NUM(056, _SET_TEXTURE_FILTER, _TEXTUREMAG, nv056TextureMagFilter[dwMagFilter]);
                        break;
                    case 1:
                        // default back to linear with no aniso
                        dwControl0   |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);
                        dwFilterMask |= DRF_NUM(056, _SET_TEXTURE_FILTER, _TEXTUREMIN, nv056TextureMinFilter[D3DTFN_LINEAR][dwMipFilter]);
                        dwFilterMask |= DRF_NUM(056, _SET_TEXTURE_FILTER, _TEXTUREMAG, nv056TextureMagFilter[dwMagFilter]);
                        break;
                    default:
                        // aniso > 2. we can't really do it, but we'll approximate it as best we can
                        pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                        // fall through
                    case 2:
                        // use real aniso
                        dwControl0   |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _1);
                        dwFilterMask |= DRF_NUM(056, _SET_TEXTURE_FILTER, _TEXTUREMIN, nv056TextureMinFilter[D3DTFN_LINEAR][dwMipFilter]);
                        dwFilterMask |= DRF_NUM(056, _SET_TEXTURE_FILTER, _TEXTUREMAG, nv056TextureMagFilter[D3DTFG_LINEAR]);

                        if(dwMagFilter == D3DTFG_POINT) {
                            // When aniso filtering is on, mag filter will be linear, point filtering
                            // won't happen, so we must fail validate texture stage for DCT450.
                            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                        }
                        break;
                }  // switch
            }

            else {

                dwControl0   |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);
                dwFilterMask |= DRF_NUM(056, _SET_TEXTURE_FILTER, _TEXTUREMIN, nv056TextureMinFilter[dwMinFilter][dwMipFilter]);
                dwFilterMask |= DRF_NUM(056, _SET_TEXTURE_FILTER, _TEXTUREMAG, nv056TextureMagFilter[dwMagFilter]);
            }

            // OR in LOD bias
            DWORD dwLODBias = pTSSState->dwLODBias;
            if ((pContext->bScaledFlatPanel)         &&
                (pContext->dwEarlyCopyStrategy > 2)  &&
                (pContext->dwEarlyCopyStrategy < 10) &&
                (!(pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE))) {
                // sign-extend from 5.8 to 24.8
                int iLODBias = (dwLODBias & 0x00001000) ? (dwLODBias | 0xfffff000) : dwLODBias;
                iLODBias += int (0.5f * 256.0f);   // 5.8 format
                // clamp to -2^12...2^12-1 (5.8 twos-complement fixed-point)
                iLODBias = max (iLODBias, int(-4096));
                iLODBias = min (iLODBias, int(4095));
                // return the last 13 bits
                dwLODBias = iLODBias & 0x00001fff;
            }
            dwFilterMask |= dwLODBias;

            dwOffset = pTexture->getSwizzled()->getOffset();

            nvCelsiusSendTextureData (pContext, dwHWStage, dwControl0, dwOffset, dwFormatMask, dwFilterMask);
        }

        else if (dwClipPlaneEnable) {

            // if user clip planes are enabled, knock off as many as we can.
            // (each texture stage can handle two)

            pTexture = getDC()->pClipPlaneTexture;

            dwTexUnitStatus = CELSIUS_TEXUNITSTATUS_CLIP;
            pContext->hwState.pTextureInUse[dwHWStage] = pTexture;

            dwWDivideFlags |= CELSIUS_FLAG_WDIVIDE (dwHWStage);
            dwWSourceFlags |= CELSIUS_FLAG_WSOURCE (dwHWStage);
            bWSourceFound = TRUE;

            // find the first enabled plane
            while (!(dwClipPlaneEnable & 0x1)) {
                dwClipPlaneEnable >>= 1;
                dwCurrentClipPlaneIndex++;
            }
            pClipPlane1 = pContext->ppClipPlane[dwCurrentClipPlaneIndex];
            dwClipPlaneEnable >>= 1;
            dwCurrentClipPlaneIndex++;

            // find the second enabled plane, if any
            if (dwClipPlaneEnable) {
                while (!(dwClipPlaneEnable & 0x1)) {
                    dwClipPlaneEnable >>= 1;
                    dwCurrentClipPlaneIndex++;
                }
                pClipPlane2 = pContext->ppClipPlane[dwCurrentClipPlaneIndex];
                dwTexgenT = NV056_SET_TEXGEN_T_V_EYE_LINEAR;
                dwClipPlaneEnable >>= 1;
                dwCurrentClipPlaneIndex++;
            }

            else {
                pClipPlane2 = NULL;
                dwTexgenT = NV056_SET_TEXGEN_T_V_DISABLE;
            }

            // set up tex coordinate sources
            // default to [1,1,0,1] and get s and/or t from eye space coords, as needed
            pContext->hwState.celsius.set4f (NV056_SET_TEXCOORD0_4F(0) + 0x28 * dwHWStage, 1.0f, 1.0f, 0.0f, 1.0f);
            pContext->hwState.celsius.set   (NV056_SET_TEXGEN_S(dwHWStage), NV056_SET_TEXGEN_S_V_EYE_LINEAR);
            pContext->hwState.celsius.set   (NV056_SET_TEXGEN_T(dwHWStage), dwTexgenT);
            pContext->hwState.celsius.set   (NV056_SET_TEXGEN_R(dwHWStage), NV056_SET_TEXGEN_R_V_DISABLE);
            pContext->hwState.celsius.set   (NV056_SET_TEXGEN_Q(dwHWStage), NV056_SET_TEXGEN_Q_V_DISABLE);

            // dot the eyespace coords with clip plane normals
            // D3D gives us the normals in world space, so they need to be converted to eye space
            Inverse4x4 (&mTmp, &(pContext->xfmView), TRUE);
            MatrixTranspose4x4 (&mTmp2, &mTmp);

            XformVector4 (pClipPlaneEye, pClipPlane1, &mTmp2);
            pContext->hwState.celsius.set4f (NV056_SET_TEXGEN_SPLANE0(0) + 0x40 * dwHWStage,
                                          pClipPlaneEye[0], pClipPlaneEye[1],
                                          pClipPlaneEye[2], pClipPlaneEye[3]);

            if (pClipPlane2) {
                XformVector4 (pClipPlaneEye, pClipPlane2, &mTmp2);
                pContext->hwState.celsius.set4f (NV056_SET_TEXGEN_TPLANE0(0) + 0x40 * dwHWStage,
                                              pClipPlaneEye[0], pClipPlaneEye[1],
                                              pClipPlaneEye[2], pClipPlaneEye[3]);
            }

            // we now have s<0 behind the plane and s>0 in front of the plane at the input
            // to the texture transform. we need to shift these coords by 0.5 so that clipped
            // stuff falls on the left (or upper) part of the texture and unclipped stuff falls
            // on the right (or lower) part of the texture
            pContext->hwState.celsius.set (NV056_SET_TEXTURE_MATRIX0_ENABLE + dwHWStage * 4, NV056_SET_TEXTURE_MATRIX0_ENABLE_V_TRUE);

            // for user clip planes we need a transform matrix that just adds small
            // quantities to the s and t coords produced by texgen
            DWORD dwBase = dwHWStage ? NV056_SET_TEXTURE_MATRIX1(0) : NV056_SET_TEXTURE_MATRIX0(0);
            pContext->hwState.celsius.set4f (dwBase +  0, 1.0f, 0.0f, 0.0f, 0.5f);
            pContext->hwState.celsius.set4f (dwBase + 16, 0.0f, 1.0f, 0.0f, pClipPlane2 ? 0.5f : 0.0f);
            pContext->hwState.celsius.set4f (dwBase + 32, 0.0f, 0.0f, 1.0f, 0.0f);
            pContext->hwState.celsius.set4f (dwBase + 48, 0.0f, 0.0f, 0.0f, 1.0f);

            nvCelsiusSetTextureBasics (pTexture, &dwControl0, &dwFormatMask, &dwFilterMask);
            nvCelsiusAddTextureDefaults (pTexture, &dwControl0, &dwFormatMask, &dwFilterMask, &dwOffset);

            dwControl0 |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _KILL);
            pContext->hwState.celsius.set (NV056_SET_COLOR_KEY_COLOR(dwHWStage), 0);

            nvCelsiusSendTextureData (pContext, dwHWStage, dwControl0, dwOffset, dwFormatMask, dwFilterMask);
        }

        else if ((dwZEnable==D3DZB_USEW) && (bWSourceFound==FALSE)) {

            // we're using w-buffering, but we haven't yet enabled a texture unit
            // in which we can do the requisite w divide. the w divide is required
            // because the inverted 1/w value (plain w itself) required for w-buffering
            // is only available (with sufficient precision) from the texturing unit.

            // use the default texture (white)
            pTexture = getDC()->pDefaultTexture;

            dwTexUnitStatus = CELSIUS_TEXUNITSTATUS_DEFAULT;
            pContext->hwState.pTextureInUse[dwHWStage] = pTexture;

            dwWDivideFlags |= CELSIUS_FLAG_WDIVIDE (dwHWStage);
            dwWSourceFlags |= CELSIUS_FLAG_WSOURCE (dwHWStage);
            bWSourceFound = TRUE;

            pContext->hwState.celsius.set4 (NV056_SET_TEXGEN_S(dwHWStage),
                                         NV056_SET_TEXGEN_S_V_DISABLE,
                                         NV056_SET_TEXGEN_T_V_DISABLE,
                                         NV056_SET_TEXGEN_R_V_DISABLE,
                                         NV056_SET_TEXGEN_Q_V_DISABLE);

            nvCelsiusSetTextureBasics (pTexture, &dwControl0, &dwFormatMask, &dwFilterMask);
            nvCelsiusAddTextureDefaults (pTexture, &dwControl0, &dwFormatMask, &dwFilterMask, &dwOffset);

            dwControl0 |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _FALSE);

            nvCelsiusSendTextureData (pContext, dwHWStage, dwControl0, dwOffset, dwFormatMask, dwFilterMask);
        }

        else {

            // no user texture, no clip planes, no w-buffering. go ahead and totally disable the stage

            dwTexUnitStatus = CELSIUS_TEXUNITSTATUS_IDLE;
            pContext->hwState.pTextureInUse[dwHWStage] = NULL;

            pContext->hwState.celsius.set4 (NV056_SET_TEXGEN_S(dwHWStage),
                                            NV056_SET_TEXGEN_S_V_DISABLE,
                                            NV056_SET_TEXGEN_T_V_DISABLE,
                                            NV056_SET_TEXGEN_R_V_DISABLE,
                                            NV056_SET_TEXGEN_Q_V_DISABLE);

            // control 0
            // explicitly disable aniso because celsius will not automatically
            // disable it when the texture itself is disabled (can cost performance)
            dwControl0 = DRF_DEF(056, _SET_TEXTURE_CONTROL0, _ENABLE, _FALSE) |
                         DRF_DEF(056, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0);

            pContext->hwState.celsius.set (NV056_SET_TEXTURE_CONTROL0(dwHWStage), dwControl0);
        }

        if (pContext->hwState.dwTextureUnitStatus[dwHWStage] != dwTexUnitStatus) {
            pContext->hwState.dwTextureUnitStatus[dwHWStage] = dwTexUnitStatus;
            pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_TRANSFORM
                                           |  CELSIUS_DIRTY_FVF
                                           |  CELSIUS_DIRTY_TL_MODE;
        }

    }  // for dwHWStage=0...

    // we can't w-buffer if we can't get w from any of the texture units
    if ((dwZEnable==D3DZB_USEW) && (bWSourceFound==FALSE)) {
        DPF("no texture unit available for w divide. w-buffering impossible");
        pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
        dwZEnable = D3DZB_TRUE;
    }

    // set dependent dirty state

    if ((pContext->hwState.dwStateFlags & (CELSIUS_FLAG_USERTEXCOORDSNEEDED(0) | CELSIUS_FLAG_USERTEXCOORDSNEEDED(1))) != dwUserCoordsNeeded) {
        pContext->hwState.dwStateFlags &= ~(CELSIUS_FLAG_USERTEXCOORDSNEEDED(0) | CELSIUS_FLAG_USERTEXCOORDSNEEDED(1));
        pContext->hwState.dwStateFlags |= dwUserCoordsNeeded;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
    }

    if ((pContext->hwState.dwStateFlags & (CELSIUS_FLAG_WDIVIDE(0) | CELSIUS_FLAG_WDIVIDE(1))) != dwWDivideFlags) {
        pContext->hwState.dwStateFlags &= ~(CELSIUS_FLAG_WDIVIDE(0) | CELSIUS_FLAG_WDIVIDE(1));
        pContext->hwState.dwStateFlags |= dwWDivideFlags;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TL_MODE
                                       |  CELSIUS_DIRTY_TEXTURE_TRANSFORM;
    }

    if ((pContext->hwState.dwStateFlags & (CELSIUS_FLAG_WSOURCE(0) | CELSIUS_FLAG_WSOURCE(1))) != dwWSourceFlags) {
        pContext->hwState.dwStateFlags &= ~(CELSIUS_FLAG_WSOURCE(0) | CELSIUS_FLAG_WSOURCE(1));
        pContext->hwState.dwStateFlags |= dwWSourceFlags;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_CONTROL0;
    }

    if ((pContext->hwState.dwStateFlags & CELSIUS_FLAG_TEXSTATENEEDSINVMV) != dwInvMVNeeded) {
        pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_TEXSTATENEEDSINVMV;
        pContext->hwState.dwStateFlags |= dwInvMVNeeded;
        pContext->hwState.dwDirtyFlags |= dwInvMVNeeded ? CELSIUS_DIRTY_TRANSFORM : 0;
    }

    if (pContext->hwState.dwTexCoordIndices != dwTexCoordIndices) {
        pContext->hwState.dwTexCoordIndices = dwTexCoordIndices;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
    }

    if (dwNumTexCoordsNeeded != pContext->hwState.dwNumTexCoordsNeeded) {
        pContext->hwState.dwNumTexCoordsNeeded = dwNumTexCoordsNeeded;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
    }


    if (pContext->hwState.celsius.dwZEnable != dwZEnable) {
        pContext->hwState.celsius.dwZEnable = dwZEnable;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TL_MODE
                                       |  CELSIUS_DIRTY_FVF
                                       |  CELSIUS_DIRTY_TRANSFORM
                                       |  CELSIUS_DIRTY_SPECFOG_COMBINER
                                       |  CELSIUS_DIRTY_CONTROL0
                                       |  CELSIUS_DIRTY_MISC_STATE;
    }

    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_SQUASHW_MASK) == D3D_REG_SQUASHW_ENABLE) {
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
    }

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvCelsiusSetFogMode (PNVD3DCONTEXT pContext, DWORD *dwZFogMask)
{
    float fBias, fScale;

    nvAssert (pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]);

    cacheFogValues(pContext);

    pContext->hwState.celsius.prep_wait_for_idle_nv10();

    switch (pContext->fogData.dwFogTableMode) {
        case D3DFOG_NONE:   // user-supplied fog (neither table nor vertex)
            fBias = 1.0f;
            fScale = 1.0f;
            pContext->hwState.celsius.set (NV056_SET_FOG_MODE, NV056_SET_FOG_MODE_FOG_MODE_LINEAR);
            break;
        case D3DFOG_LINEAR:
            fBias = 1.0f + pContext->fogData.fFogTableEnd * pContext->fogData.fFogTableLinearScale;
            fScale = -pContext->fogData.fFogTableLinearScale;
            pContext->hwState.celsius.set (NV056_SET_FOG_MODE, NV056_SET_FOG_MODE_FOG_MODE_LINEAR);
            break;
        case D3DFOG_EXP:
            fBias = 1.5f;
            fScale = -pContext->fogData.fFogTableDensity * (1.0f / (2.0f * 5.5452f));
            pContext->hwState.celsius.set (NV056_SET_FOG_MODE, NV056_SET_FOG_MODE_FOG_MODE_EXP);
            break;
        case D3DFOG_EXP2:
            fBias = 1.5f;
            fScale = -pContext->fogData.fFogTableDensity * (1.0f / (2.0f * (float)sqrt(5.5452f)));
            pContext->hwState.celsius.set (NV056_SET_FOG_MODE, NV056_SET_FOG_MODE_FOG_MODE_EXP2);
            break;
        default:
            DPF ("uh oh. unknown fog mode");
    }

    *dwZFogMask = 0;

    if (pContext->fogData.dwFogTableMode == D3DFOG_NONE) {
        // both table fog and vertex fog are disabled. we default back to regular user fog
        pContext->hwState.celsius.set (NV056_SET_FOG_GEN_MODE, NV056_SET_FOG_GEN_MODE_FOG_GEN_MODE_USE_INPUT);
        // the hardware needs OGL's infinite view vector for user fog (for reasons that escape me)
        if (getDC()->bD3DViewVector) {
            nvglSetNv10CelsiusNoOp (NV_DD_CELSIUS);
            pContext->hwState.celsius.set3f (NV056_SET_EYE_DIRECTION_SW(0), 0.f,0.f,1.f);
            getDC()->bD3DViewVector = FALSE;
        }
    }

    else {
        // we're doing vertex or table fog. on celsius we simply upgrade the former to the
        // latter and treat them both the same way.
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) {
            // if we're in passthru mode, we have to hack things a bit to still get
            // the use of the HW fog table which is intimately tied to the T&L unit.
            // we use the foggenmode to tell the HW what to use for depth
            nvAssert(pContext->hwState.dwFogSource); // assert that fog source has been set
            if (pContext->hwState.dwFogSource == CHardwareState::FOG_SOURCE_Z) {
                // use z for fog depth
                pContext->hwState.celsius.set (NV056_SET_FOG_GEN_MODE, 0x01);
                *dwZFogMask = CELSIUS_FLAG_ZFOG;
            }
            else {
                // use w for fog depth
                pContext->hwState.celsius.set (NV056_SET_FOG_GEN_MODE, 0x02);
                // if we're w-buffering, 1/w will get scaled before it gets to the fog unit.
                // we need to undo the scale by re-applying the same scale here.
                // (we just re-apply, b/c 1/w has also been subsequently inverted)
                if (pContext->hwState.celsius.dwZEnable == D3DZB_USEW) {
                    nvAssert (pContext->pRenderTarget);
                    fScale *= (pContext->surfaceViewport.dvWFar * pContext->hwState.dvInvZScale);
                }
            }
        }
        else {
            // not in passthru mode. just do things normally
            if (pContext->dwRenderState[D3DRENDERSTATE_RANGEFOGENABLE]) {
                pContext->hwState.celsius.set (NV056_SET_FOG_GEN_MODE, NV056_SET_FOG_GEN_MODE_FOG_GEN_MODE_RADIAL);
            }
            else {
                pContext->hwState.celsius.set (NV056_SET_FOG_GEN_MODE, NV056_SET_FOG_GEN_MODE_FOG_GEN_MODE_PLANAR);
                // Since we're already in eye space, just construct the plane
                pContext->hwState.celsius.set4f (NV056_SET_FOG_PLANE(0), 0.0f, 0.0f, 1.0f, 0.0f);
            }
        }
    }

    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) {
        // in passthru mode, K0, K1, and K2 all get the scale.
        // ambient red gets overloaded with the bias
        pContext->hwState.celsius.set3f (NV056_SET_FOG_PARAMS(0),          fScale, fScale, fScale);
        pContext->hwState.celsius.set3f (NV056_SET_SCENE_AMBIENT_COLOR(0), fBias,  0.0f,   0.0f  );
    }
    else {
        pContext->hwState.celsius.set3f (NV056_SET_FOG_PARAMS(0), fBias, fScale, 0.0f);
    }

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set the fog/specular combiner

HRESULT nvSetCelsiusSpecularFogCombiner (PNVD3DCONTEXT pContext)
{
    DWORD dwControl0, dwControl1;  // HW control words
    DWORD dwZFogMask;

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_SPECFOG_COMBINER;

    dwControl0 = 0;

    if (pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]) {
        pContext->hwState.celsius.set (NV056_SET_FOG_ENABLE, NV056_SET_FOG_ENABLE_V_TRUE);
        pContext->hwState.celsius.set (NV056_SET_FOG_COLOR,  swapRGB(pContext->dwRenderState[D3DRENDERSTATE_FOGCOLOR]));

        nvCelsiusSetFogMode (pContext, &dwZFogMask);

        // A is fog factor in fog alpha
        dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _A_SOURCE, _REG_3);
        // A alpha is used for fog - replicate it
        dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _A_ALPHA, _TRUE);

        // B source is (textured diffuse) or (textured diffuse + specular)
        if (pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]) {
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_SPECLIT);
        }
        else {  // don't add specular to diffuse(textured) color
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_C);
        }

        // if the last stage was using addcomplement as its colorop, we need to invert color here
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTRGB(pContext->hwState.dwNumActiveCombinerStages - 1)) {
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_INVERSE, _TRUE);
        }

        // C is fog color
        dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _C_SOURCE, _REG_3);

        // D isn't used
        pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW0, dwControl0);
    }

    else { // no fog, just output diffuse (+ specular)
        pContext->hwState.celsius.set (NV056_SET_FOG_ENABLE, NV056_SET_FOG_ENABLE_V_FALSE);

        dwZFogMask = 0;

        if (pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]) {
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _D_SOURCE, _REG_SPECLIT);
        }
        else {  // don't add specular to diffuse(textured) color
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _D_SOURCE, _REG_C);
        }

        // if the last stage was using addcomplement as its colorop, we need to invert color here
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTRGB(pContext->hwState.dwNumActiveCombinerStages - 1)) {
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _D_INVERSE, _TRUE);
        }

        pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW0, dwControl0);

        pContext->hwState.celsius.prep_wait_for_idle_nv10();
        pContext->hwState.celsius.set (NV056_SET_FOG_MODE,                  NV056_SET_FOG_MODE_FOG_MODE_LINEAR);
        pContext->hwState.celsius.set (NV056_SET_FOG_GEN_MODE,              NV056_SET_FOG_GEN_MODE_FOG_GEN_MODE_USE_INPUT);
    }

    dwControl1 = 0;

    // the final value for alpha is just taken from G
    dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _G_SOURCE, _REG_C);

    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_DOTPRODUCT3(pContext->hwState.dwNumActiveCombinerStages-1)) {
        // if the last stage produced a dot product, we were supposed to have replicated the result
        // into alpha, but couldn't b/c celsius can't. get it from the color channels now.
        dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _G_ALPHA, _FALSE);
    }
    else {
        // just grab alpha from alpha
        dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _G_ALPHA, _TRUE);
        // if the last stage was using addcomplement as its alphaop, we need to invert alpha here
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTALPHA(pContext->hwState.dwNumActiveCombinerStages-1)) {
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _G_INVERSE, _TRUE);
        }
    }

    dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _SPECULAR_CLAMP, _TRUE);

    pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW1, dwControl1);

    //cachec off spec/fog control words
    pContext->hwState.specfog_cw[0] = dwControl0;
    pContext->hwState.specfog_cw[1] = dwControl1;

#ifdef ALPHA_CULL
    if(pContext->dwEarlyCopyStrategy && pContext->hwState.alphacull_mode >= 2)
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_MISC_STATE;
#endif

    if ((pContext->hwState.dwStateFlags & CELSIUS_FLAG_ZFOG) != dwZFogMask) {
        pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_ZFOG;
        pContext->hwState.dwStateFlags |= dwZFogMask;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
    }

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set celsius TL mode (passthrough stuff)

HRESULT nvSetCelsiusTLMode (PNVD3DCONTEXT pContext)
{
    DWORD dwPassthru, dwHWStage;

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_TL_MODE;

    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) {
        dwPassthru = DRF_DEF(056, _SET_TLMODE, _PASSTHROUGH, _TRUE);
    }
    else {
        dwPassthru = DRF_DEF(056, _SET_TLMODE, _PASSTHROUGH, _FALSE);
    }

    for (dwHWStage=0; dwHWStage<2; dwHWStage++) {
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_WDIVIDE(dwHWStage)) {
            dwPassthru |= (dwHWStage == 0) ?
                          DRF_DEF(056, _SET_TLMODE, _W_DIVIDE_0, _ENABLE) :
                          DRF_DEF(056, _SET_TLMODE, _W_DIVIDE_1, _ENABLE);
        }
        else {
            dwPassthru |= (dwHWStage == 0) ?
                          DRF_DEF(056, _SET_TLMODE, _W_DIVIDE_0, _DISABLE) :
                          DRF_DEF(056, _SET_TLMODE, _W_DIVIDE_1, _DISABLE);
        }
    }

    pContext->hwState.celsius.set (NV056_SET_TLMODE, dwPassthru);

    return (D3D_OK);
}

#if 1

//---------------------------------------------------------------------------

// Check for need / feasibility of texture matrix hack and return values in bNeeded and bFeasible.
// The hack is needed if:
// - we only have a class 056 celsius
// - the broken matrix element is non-negligibly non-zero
// The HW hack if feasible if:
// - this stage uses eyespace coordinates generated by the HW
// - the other texture stage is either disabled or has a user texture with texgen disabled
// - we're not in passthru mode
// The SW workaround is feasible if:
// - texgen is not enabled

void nvTexMatrixHackCheck
(
    PNVD3DCONTEXT pContext,
    DWORD         dwHWStage,
    BOOL          bTexgen,
    D3DVALUE      dvCriticalElement,
    BOOL         *bEnableHWHack,
    BOOL         *bEnableSWHack
)
{
    DWORD dwOtherHWStage, dwOtherTexUnitStatus, dwOtherD3DStage, dwOtherTexgenMode;
    BOOL  bOtherTexgen;

    // assume things are ok unless we find otherwise
    *bEnableHWHack = *bEnableSWHack = FALSE;

    // we only need to worry about this if we have nothing better than a class 056 celsius
    // and the critical element is non-negligible
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_0096_CELSIUS | NVCLASS_1196_CELSIUS))
        &&
        ((dvCriticalElement > CELSIUS_TEX_MATRIX_HACK_THRESHOLD) ||
         (dvCriticalElement < -CELSIUS_TEX_MATRIX_HACK_THRESHOLD)))
    {
        // a texture matrix workaround is required. can we do it?
        dwOtherHWStage       = dwHWStage ^ 0x1;
        dwOtherTexUnitStatus = pContext->hwState.dwTextureUnitStatus[dwOtherHWStage];

        if (dwOtherTexUnitStatus == CELSIUS_TEXUNITSTATUS_USER) {
            dwOtherD3DStage   = pContext->hwState.dwTexUnitToTexStageMapping[dwOtherHWStage];
            dwOtherTexgenMode = (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) ?
                                D3DTSS_TCI_PASSTHRU:pContext->tssState[dwOtherD3DStage].dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff0000;
            bOtherTexgen      = (dwOtherTexgenMode != D3DTSS_TCI_PASSTHRU);
        }

        if ((bTexgen)
            &&
            ((dwOtherTexUnitStatus == CELSIUS_TEXUNITSTATUS_IDLE)
             ||
             (dwOtherTexUnitStatus == CELSIUS_TEXUNITSTATUS_DEFAULT)
             ||
             ((dwOtherTexUnitStatus == CELSIUS_TEXUNITSTATUS_USER) && (!bOtherTexgen)))
            &&
            (!(pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE)))
        {
            // enable the HW-based hack
            *bEnableHWHack = TRUE;
        }
        else if (!bTexgen)
        {
            // enable the SW workaround
            *bEnableSWHack = TRUE;
        }
        else
        {
            // we need it but can't do it
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
        }
    }

}

#else

//---------------------------------------------------------------------------

// check for need / feasibility of texture matrix hack :
// - this stage uses eyespace coordinates generated by the HW
// - the broken matrix element is non-negligibly non-zero
// - the other texture stage is either disabled or has a user texture with texgen disabled
// - we're not in passthru mode

BOOL nvTexMatrixHackEnable (PNVD3DCONTEXT pContext, DWORD dwHWStage, BOOL bTexgen, D3DVALUE dvCriticalElement)
{
    DWORD dwOtherHWStage, dwOtherTexUnitStatus, dwOtherD3DStage, dwOtherTexgenMode;
    BOOL  bOtherTexgen;

    // we only need to worry about this if we have nothing better than a class 056 celsius
    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_0096_CELSIUS | NVCLASS_1196_CELSIUS)) {
        return (FALSE);
    }

    if ((dvCriticalElement > CELSIUS_TEX_MATRIX_HACK_THRESHOLD) ||
        (dvCriticalElement < -CELSIUS_TEX_MATRIX_HACK_THRESHOLD))
    {
        // the texture matrix hack is required. can we actually do it?
        dwOtherHWStage       = dwHWStage ^ 0x1;
        dwOtherTexUnitStatus = pContext->hwState.dwTextureUnitStatus[dwOtherHWStage];

        if (dwOtherTexUnitStatus == CELSIUS_TEXUNITSTATUS_USER) {
            dwOtherD3DStage   = pContext->hwState.dwTexUnitToTexStageMapping[dwOtherHWStage];
            dwOtherTexgenMode = pContext->tssState[dwOtherD3DStage].dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff0000;
            bOtherTexgen      = (dwOtherTexgenMode != D3DTSS_TCI_PASSTHRU);
        }

        if ((bTexgen)
            &&
            ((dwOtherTexUnitStatus == CELSIUS_TEXUNITSTATUS_IDLE)
             ||
             (dwOtherTexUnitStatus == CELSIUS_TEXUNITSTATUS_DEFAULT)
             ||
             ((dwOtherTexUnitStatus == CELSIUS_TEXUNITSTATUS_USER) && (!bOtherTexgen)))
            &&
            (!(pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE)))
        {
            // we need it and can do it
            return (TRUE);
        }
        else
        {
            // we need it but can't do it
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
            return (FALSE);
        }
    }
    else
    {
        // texture matrix hack isn't required
        return (FALSE);
    }
}

#endif

//---------------------------------------------------------------------------

// set texture transformation matrices

HRESULT nvSetCelsiusTextureTransform (PNVD3DCONTEXT pContext)
{
    CTexture    *pTexture;
    BOOL        bEnable, bXForm, bCubeMap, bTexgen;
    DWORD       dwPassthruCubeMappingMask = 0;
    DWORD       dwTextureMatrixHackMask = 0;
#if (NVARCH >= 0x015)
    BOOL        bWDivideEnable;
#endif
    DWORD       dwOp, dwFlags, dwInCount, dwOutCount, dwTexUnitStatus, dwTexgenMode;
    DWORD       dwHWStage, dwD3DStage, dwMatrixEnable, dwMatrixDisable;
    DWORD       dwMatrixName, dwMatrixType;
    LPD3DMATRIX pMatrix;
#ifdef CELSIUS_TEX_MATRIX_HACK
    D3DVALUE    dvCriticalElement;
    BOOL        bHWHack, bSWHack;
#endif  // CELSIUS_TEX_MATRIX_HACK

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_TEXTURE_TRANSFORM;

    for (dwHWStage=0; dwHWStage < 2; dwHWStage++) {

        dwTexUnitStatus = pContext->hwState.dwTextureUnitStatus[dwHWStage];

        if (dwTexUnitStatus == CELSIUS_TEXUNITSTATUS_CLIP) {
            // if we're doing clip planes, we've already set up texture transform
            continue;
        }

        if (dwHWStage==0) {
            dwMatrixEnable  = NV056_SET_TEXTURE_MATRIX0_ENABLE_V_TRUE;
            dwMatrixDisable = NV056_SET_TEXTURE_MATRIX0_ENABLE_V_FALSE;
            dwMatrixName    = NV056_SET_TEXTURE_MATRIX0(0);
        }
        else {
            dwMatrixEnable  = NV056_SET_TEXTURE_MATRIX1_ENABLE_V_TRUE;
            dwMatrixDisable = NV056_SET_TEXTURE_MATRIX1_ENABLE_V_FALSE;
            dwMatrixName    = NV056_SET_TEXTURE_MATRIX1(0);
        }

        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];

        if (dwD3DStage == CELSIUS_UNUSED) {
            bEnable = FALSE;
        }

        else {

            nvAssert (dwTexUnitStatus == CELSIUS_TEXUNITSTATUS_USER);
            dwOp    = pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLOROP];

            // Stage state doesn't matter if pixel shaders are on
            nvAssert (dwOp != D3DTOP_DISABLE || pContext->pCurrentPShader);

            dwFlags = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];

            // the number of coordinates to come out of the transformation, INCLUSIVE
            // of the projective coordinate when the D3DTTFF_PROJECTED flag is set.
            // (== the number of matrix columns with non-zero elements)
            dwOutCount = dwFlags & 0xff; // BUGBUG there should be a #define for this. pester MS
            bXForm = (dwOutCount != D3DTTFF_DISABLE);

            pTexture = pContext->hwState.pTextureInUse[dwHWStage];
            bCubeMap = pTexture && pTexture->isCubeMap();

            dwTexgenMode = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff0000;
            bTexgen = (dwTexgenMode != D3DTSS_TCI_PASSTHRU);

            bWDivideEnable = (pContext->hwState.dwStateFlags & CELSIUS_FLAG_WDIVIDE(dwHWStage)) ? TRUE : FALSE;

            // enable if they requested a transform explicitly or:
            // - cubemapping (requires shift of r into q)
            // - texgen is enabled (accommodate hacky nv15 HW fix for nv10 tex matrix bug)
            // - w divide is disabled in T&L mode (likewise)
            bEnable = bXForm   ||
                      bCubeMap ||
                      bTexgen  ||
                      (!bWDivideEnable && !(pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE));

            if (bEnable && (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE))
            {
                bEnable = FALSE;
                if (bXForm || bTexgen) {
                    // we wanted tex transform for something more than just cubemapping. no can do.
                    DPF ("Hardware can't do texture matrix transform in passthrough mode");
                    pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
                }
                if (bCubeMap) {
                    dwPassthruCubeMappingMask |= CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(dwHWStage);
                }
            }
        }


        if (!bEnable) {

            pContext->hwState.celsius.set (NV056_SET_TEXTURE_MATRIX0_ENABLE + dwHWStage * 4, dwMatrixDisable);

        }

        else {

            pContext->hwState.celsius.set (NV056_SET_TEXTURE_MATRIX0_ENABLE + dwHWStage * 4, dwMatrixEnable);

            pMatrix = &(pContext->tssState[dwD3DStage].mTexTransformMatrix);

            if (bCubeMap) {

                if (!bXForm) {
                    // no transformation desired by user, but celsius still requires a transformation
                    // to shift the r coordinate into the q position where the HW can access it.
                    // (celsius doesn't really handle 3 coordinates and always ignores the r slot)
                    pContext->hwState.celsius.setTextureTransformMatrixCubeMapIdent (dwMatrixName);
                }

                else {
                    // texture transformation and cubemapping are both enabled.

#ifdef CELSIUS_TEX_MATRIX_HACK
                    nvTexMatrixHackCheck (pContext, dwHWStage, bTexgen, pMatrix->_13, &bHWHack, &bSWHack);

                    if (bHWHack) {
                        DPF ("using tex transform hack #1a");
                        dwTextureMatrixHackMask |= CELSIUS_FLAG_TEXMATRIXHACK(dwHWStage);
                        pContext->hwState.celsius.setTextureTransformMatrixCubeMapIdent (dwMatrixName);
                    }
                    else if (bSWHack) {
                        DPF ("using SW workaround for broken texture matrix in stage %d", dwHWStage);
                        dwTextureMatrixHackMask |= CELSIUS_FLAG_TEXMATRIXSWFIX(dwHWStage);
                        pContext->hwState.celsius.setTextureTransformMatrixCubeMapIdent (dwMatrixName);
                    }
                    else
#endif  // CELSIUS_TEX_MATRIX_HACK
                    {
#ifdef DEBUG
                        // some sanity checks:
                        // the transformation should be a 3-coordinate transformation.
                        if (dwOutCount != D3DTTFF_COUNT3) {
                            DPF ("ugh. cubemap texture tranformation matrix is not a 3-D transformation");
                        }
                        // the transform should not include any shifting. translating 3 coordinates
                        // requires four rows in the matrix, but celsius only looks at three rows.
                        if ((pMatrix->_41) || (pMatrix->_42) || (pMatrix->_43)) {
                            DPF ("blurg. cubemap texture tranformation matrix is attempting translation");
                        }
                        // finally, the transform cannot include a projection because projecting
                        // requires q which is unavailable because r will be stored in its place.
                        // projecting cubemap coordinates really just means scaling a vector which
                        // is meaningless anyhow.
                        if (dwFlags & D3DTTFF_PROJECTED) {
                            DPF ("ack. can't do projective cubemapping, but who cares");
                        }
#endif
                        // set the matrix, copying column 3 to column 4, thereby moving r to the q slot.
                        pContext->hwState.celsius.setTextureTransformMatrixCubeMap (dwMatrixName,pMatrix);
                    }

                }

            }  // cubemap

            else if (bXForm) {

                // the number of coordinates incoming from the FVF vertex data or texgen
                // (== the number of matrix rows with non-zero elements)
                // (actually there can be dwInCount+1 rows with non-zero elements b/c D3D assumes
                // that a q==1 is automatially postpended to the coordinates they give you)
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

                // calculate an arbitrary identifier
                dwMatrixType = ((dwFlags & D3DTTFF_PROJECTED) ? 2 : 1) * dwInCount;

#ifdef CELSIUS_TEX_MATRIX_HACK
                switch (dwMatrixType) {
                    case 2:
                    case 3:
                        dvCriticalElement = 0.0f;
                        break;
                    case 4:
                    case 6:
                        dvCriticalElement = pMatrix->_13;
                        break;
                }

                nvTexMatrixHackCheck (pContext, dwHWStage, bTexgen, dvCriticalElement, &bHWHack, &bSWHack);

                if (bHWHack) {
                    DPF ("using tex transform hack #1b");
                    dwTextureMatrixHackMask |= CELSIUS_FLAG_TEXMATRIXHACK(dwHWStage);
                    //swap R/Q columns so we divide by the right thing later.
                    D3DMATRIX mTemp;
                    mTemp._11 = mTemp._22 = mTemp._34 = mTemp._43 = 1;
                    mTemp._12 = mTemp._13 = mTemp._14 = mTemp._21 = mTemp._23 = mTemp._24 = 0;
                    mTemp._31 = mTemp._32 = mTemp._33 = mTemp._41 = mTemp._42 = mTemp._44 = 0;
                    pContext->hwState.celsius.setMatrixTransposed (dwMatrixName, &mTemp);
                    //nvglSetNv10CelsiusTextureMatrixEnable (NV_DD_CELSIUS, dwHWStage, dwMatrixDisable);
                }
                else if (bSWHack) {
                    DPF ("using SW workaround for broken texture matrix in stage %d", dwHWStage);
                    dwTextureMatrixHackMask |= CELSIUS_FLAG_TEXMATRIXSWFIX(dwHWStage);
                    pContext->hwState.celsius.setMatrixIdent (dwMatrixName);
                }
                else
#endif  // CELSIUS_TEX_MATRIX_HACK

                {
#if defined(CELSIUS_TEX_MATRIX_HACK) && defined(DEBUG)
                    if ((!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_0096_CELSIUS | NVCLASS_1196_CELSIUS))) &&
                        (fabs(dvCriticalElement) > CELSIUS_TEX_MATRIX_HACK_THRESHOLD)) {
                        DPF ("nvSetCelsiusTextureTransform: Critical element is non-zero. Undefined result.");
                    }
#endif

                    switch (dwMatrixType) {

                        case 2:

                            // 2 incoming texture coordinates, unprojected
                            // dwOutCount for an unprojected texture should be 2 : (s,t)
                            nvAssert (dwOutCount == 2);
                            // expand the matrix and override the last column with [0 0 0 1]'
                            pContext->hwState.celsius.setTextureTransformMatrixTypeA (dwMatrixName, pMatrix);
#ifdef DEBUG
                            if ((pMatrix->_13 != 0.0f) || (pMatrix->_23 != 0.0f) || (pMatrix->_33 != 0.0f)) {
                                DPF_LEVEL(NVDBG_LEVEL_INFO,"d3d is sketchy. driver overwrote values in last column b/c it was");
                                DPF_LEVEL(NVDBG_LEVEL_INFO,"given a projective texture matrix when projection was disabled");
                            }
#endif
                            break;

                        case 3:

                            // 3 incoming texture coordinates, unprojected
                            // dwOutCount for an unprojected texture should be 2 : (s,t)
                            nvAssert (dwOutCount == 2);
                            // override the last column with [0 0 0 1]'
                            pContext->hwState.celsius.setTextureTransformMatrixTypeB (dwMatrixName, pMatrix);
#ifdef DEBUG
                            if ((pMatrix->_14 != 0.0f) || (pMatrix->_24 != 0.0f) ||
                                (pMatrix->_34 != 0.0f) || (pMatrix->_44 != 0.0f)) {
                                DPF_LEVEL(NVDBG_LEVEL_INFO,"d3d is sketchy. driver overwrote values in last column b/c it was");
                                DPF_LEVEL(NVDBG_LEVEL_INFO,"given a projective texture matrix when projection was disabled");
                            }
#endif
                            break;

                        case 4:

                            // 2 incoming texture coordinates, projected
                            // dwOutCount for a projected texture should be 3 : (s,t,q)
                            nvAssert (dwOutCount == 3);
                            // expand the matrix but don't override the last column
                            pContext->hwState.celsius.setTextureTransformMatrixTypeC (dwMatrixName, pMatrix);
#ifdef DEBUG
                            if ((pMatrix->_13 == 0.0f) && (pMatrix->_23 == 0.0f) && (pMatrix->_33 == 0.0f)) {
                                DPF_LEVEL(NVDBG_LEVEL_INFO,"d3d is REALLY sketchy. got a projective texture transform matrix");
                                DPF_LEVEL(NVDBG_LEVEL_INFO,"with no non-zero values in the last column");
                            }
#endif
                            break;

                        case 6:

                            // 3 incoming texture coordinates, projected
                            // dwOutCount for a projected texture should be 3 : (s,t,q)
                            nvAssert (dwOutCount == 3);
                            // move col 3 to col 4
                            pContext->hwState.celsius.setTextureTransformMatrixTypeD (dwMatrixName, pMatrix);
#ifdef DEBUG
                            if ((pMatrix->_13 == 0.0f) && (pMatrix->_23 == 0.0f) &&
                                (pMatrix->_33 == 0.0f) && (pMatrix->_43 == 0.0f)) {
                                DPF_LEVEL(NVDBG_LEVEL_INFO,"d3d is REALLY sketchy. got a projective texture transform matrix");
                                DPF_LEVEL(NVDBG_LEVEL_INFO,"with zeros in the whole 3rd column");
                            }
#endif
                            break;

                        default:

                            DPF ("unknown combination of projectivism and number of texture coords");
                            dbgD3DError();
                            break;

                    }  // switch

                }  // else ! tex matrix hack

            }  // bXForm

            else {  // !bcubemap && !bXForm

                // we don't have a cubemap and haven't enabled tex xform explicitly, but we're
                // using texgen or we've disabled w divide. due to some hacky HW fixes on nv15,
                // both of these require that we enable the texture transform.
                nvAssert (bTexgen || !bWDivideEnable);
                pContext->hwState.celsius.setMatrixIdent (dwMatrixName);

            }

        }  // else enable

    }  // for dwHWStage=0...

    if ((pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1))) != dwPassthruCubeMappingMask) {
        pContext->hwState.dwStateFlags &= ~(CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1));
        pContext->hwState.dwStateFlags |= dwPassthruCubeMappingMask;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
    }

    // if we now need a tex matrix hack or used to need it and now don't, re-program some other state
    // that has all sorts of hack-specific dependencies for which the dirty bits won't normally get set
    if ((dwTextureMatrixHackMask) || (pContext->hwState.dwStateFlags & CELSIUS_MASK_TEXMATRIXHACKS)) {
        pContext->hwState.dwStateFlags &= ~CELSIUS_MASK_TEXMATRIXHACKS;
        pContext->hwState.dwStateFlags |= dwTextureMatrixHackMask;
        pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TRANSFORM | CELSIUS_DIRTY_FVF);
    }

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set celsius class specular parameters

HRESULT nvCelsiusSetSpecularParams (PNVD3DCONTEXT pContext)
{
    float L, M, N, L1, M1, N1, power;

    nvAssert ((pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]) ||
              (pContext->hwState.dwStateFlags & CELSIUS_FLAG_COMBINERSNEEDSPECULAR));

    power = pContext->Material.power;
    explut (power, &L, &M);
    N = 1.f+L-M;
    explut (0.5f*power, &L1, &M1);
    N1 = 1.f+L1-M1;
    pContext->hwState.celsius.set3f (NV056_SET_SPECULAR_PARAMS(0),L, M, N);
    pContext->hwState.celsius.set3f (NV056_SET_SPECULAR_PARAMS(3),L1,M1,N1);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set diffuse, specular, and ambient colors for a particular light
// input:
//      - dwLightID = light index [0...7]
//      - pLight7 = pointer to light data
//      - dwCmArg   = color material flags

HRESULT nvCelsiusDumpLightColors (PNVD3DCONTEXT pContext,
                                  DWORD dwLightID, LPD3DLIGHT7 pLight7,
                                  DWORD dwCmArg, BOOL bSpecularNeeded)
{
    D3DVALUE dvAmbientR, dvAmbientG, dvAmbientB;
    D3DVALUE dvDiffuseR, dvDiffuseG, dvDiffuseB;
    D3DVALUE dvSpecularR, dvSpecularG, dvSpecularB;

    nvAssert ((dwLightID >=0) && (dwLightID <= 7));

    dvAmbientR = pLight7->dcvAmbient.dvR;
    dvAmbientG = pLight7->dcvAmbient.dvG;
    dvAmbientB = pLight7->dcvAmbient.dvB;
    if (!(dwCmArg & NV056_SET_COLOR_MATERIAL_V_AMBIENT)) {
        // follow material ambient color
        dvAmbientR *= pContext->Material.dcvAmbient.dvR;
        dvAmbientG *= pContext->Material.dcvAmbient.dvG;
        dvAmbientB *= pContext->Material.dcvAmbient.dvB;
    }

    dvDiffuseR = pLight7->dcvDiffuse.dvR;
    dvDiffuseG = pLight7->dcvDiffuse.dvG;
    dvDiffuseB = pLight7->dcvDiffuse.dvB;
    if (!(dwCmArg & NV056_SET_COLOR_MATERIAL_V_DIFFUSE)) {
        // follow material diffuse color
        dvDiffuseR *= pContext->Material.dcvDiffuse.dvR;
        dvDiffuseG *= pContext->Material.dcvDiffuse.dvG;
        dvDiffuseB *= pContext->Material.dcvDiffuse.dvB;
    }

    if (bSpecularNeeded) {
        dvSpecularR = pLight7->dcvSpecular.dvR;
        dvSpecularG = pLight7->dcvSpecular.dvG;
        dvSpecularB = pLight7->dcvSpecular.dvB;
        if (!(dwCmArg & NV056_SET_COLOR_MATERIAL_V_SPECULAR)) {
            // follow material specular color
            dvSpecularR *= pContext->Material.dcvSpecular.dvR;
            dvSpecularG *= pContext->Material.dcvSpecular.dvG;
            dvSpecularB *= pContext->Material.dcvSpecular.dvB;
        }
    }
    else {
        dvSpecularR = dvSpecularG = dvSpecularB = 0.0f;
    }

    pContext->hwState.celsius.set3f (NV056_SET_LIGHT_AMBIENT_COLOR(dwLightID,0), dvAmbientR, dvAmbientG, dvAmbientB);  // ambient
    pContext->hwState.celsius.set3f (NV056_SET_LIGHT_DIFFUSE_COLOR(dwLightID,0), dvDiffuseR, dvDiffuseG, dvDiffuseB);  // diffuse
    pContext->hwState.celsius.set3f (NV056_SET_LIGHT_SPECULAR_COLOR(dwLightID,0),dvSpecularR,dvSpecularG,dvSpecularB); // specular

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set position and attenuation of a particular light
// input:
//      - dwLightID = light index [0...7]
//      - pLight7 = pointer to light data

HRESULT nvCelsiusDumpLocalLightData (PNVD3DCONTEXT pContext, int dwLightID, LPD3DLIGHT7 pLight7)
{
    D3DVECTOR pos0, pos1;

    nvAssert ((dwLightID >=0) && (dwLightID <= 7));

    // We don't normalize the position
    pos0 = pLight7->dvPosition;
    XformBy4x3 (&pos1, &pos0, 1.f, &(pContext->xfmView));

#ifdef CELSIUS_TEX_MATRIX_HACK
    DWORD dwHackMask = pContext->hwState.dwStateFlags & (CELSIUS_FLAG_TEXMATRIXHACK(0) | CELSIUS_FLAG_TEXMATRIXHACK(1));
    // the hack can't be enabled for both stages simultaneously
    nvAssert (dwHackMask != (CELSIUS_FLAG_TEXMATRIXHACK(0) | CELSIUS_FLAG_TEXMATRIXHACK(1)));

    if (dwHackMask) {
        DWORD dwHWStage  = (dwHackMask & CELSIUS_FLAG_TEXMATRIXHACK(0)) ? 0 : 1;
        DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
        LPD3DMATRIX pTexMatrix = &(pContext->tssState[dwD3DStage].mTexTransformMatrix);
        XformBy4x3 (&pos0, &pos1, 1.f, pTexMatrix);
        pos1 = pos0;
    }
#endif  // CELSIUS_TEX_MATRIX_HACK

    pContext->hwState.celsius.set3f (NV056_SET_LIGHT_LOCAL_POSITION(dwLightID,0), pos1.dvX, pos1.dvY, pos1.dvZ);

    if ((*(DWORD *)(&(pLight7->dvAttenuation0)) == 0) &&
        (*(DWORD *)(&(pLight7->dvAttenuation1)) == 0) &&
        (*(DWORD *)(&(pLight7->dvAttenuation2)) == 0)) {
        DPF ("d3d gave us a divide by 0 error b/c all attenuations are 0. nice.");
        pLight7->dvAttenuation0 = 1.f;
    }

    pContext->hwState.celsius.set3f (NV056_SET_LIGHT_LOCAL_ATTENUATION(dwLightID,0),
                                  pLight7->dvAttenuation0,
                                  pLight7->dvAttenuation1,
                                  pLight7->dvAttenuation2);

#ifdef STATE_CULL
    // attenuation stomps on infinite half-vector and vice-versa
    pContext->hwState.celsius.valid [NV056_SET_LIGHT_INFINITE_HALF_VECTOR(dwLightID,0) >> 2] = 0;
#endif

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// D3D allows you to assign either of the two vertex colors (diffuse / specular)
// to ANY of the material colors, eg. you can stuff vertex diffuse into material
// emissive. Celsius can't do all these permutations, so we implement only those
// that we can.

DWORD getCelsiusColorMaterialFlags (PNVD3DCONTEXT pContext)
{
    DWORD dwArgs;

    dwArgs = NV056_SET_COLOR_MATERIAL_V_DISABLED;

    // if COLORVERTEX isn't set just return now
    if (!pContext->dwRenderState[D3DRENDERSTATE_COLORVERTEX]) {
        return (dwArgs);
    }

    switch (pContext->dwRenderState[D3DRENDERSTATE_AMBIENTMATERIALSOURCE]) {
        case D3DMCS_MATERIAL:
            break;
        case D3DMCS_COLOR1:  // diffuse
            // inherit material ambient from vertex diffuse
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
                dwArgs |= NV056_SET_COLOR_MATERIAL_V_AMBIENT;
            }
            break;
        case D3DMCS_COLOR2:  // specular
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                DPF ("Celsius can't map vertex specular into material ambient");
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
            // inherit material diffuse from vertex diffuse
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
                dwArgs |= NV056_SET_COLOR_MATERIAL_V_DIFFUSE;
            }
            break;
        case D3DMCS_COLOR2:  // specular
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                DPF ("Celsius can't map vertex specular into material diffuse");
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
                if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                    // if vertex specular is valid, enabling NV056_SET_COLOR_MATERIAL_SPECULAR
                    // will make the HW pull material specular from the vertex specular,
                    // NOT vertex diffuse as desired.
                    DPF ("Celsius can't map vertex diffuse into material specular when vertex specular is valid");
                }
                else {
                    // in the absence of a vertex specular, NV056_SET_COLOR_MATERIAL_SPECULAR
                    // will make the HW pull material specular from the vertex diffuse
                    dwArgs |= NV056_SET_COLOR_MATERIAL_V_SPECULAR;
                }
            }
            break;
        case D3DMCS_COLOR2:  // specular
            // inherit material specular from vertex specular
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                dwArgs |= NV056_SET_COLOR_MATERIAL_V_SPECULAR;
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
            // inherit material emissive from vertex diffuse
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
                dwArgs |= NV056_SET_COLOR_MATERIAL_V_EMISSION;
            }
            break;
        case D3DMCS_COLOR2:  // specular
            if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                DPF ("Celsius can't map vertex specular into material emissive");
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

HRESULT nvSetCelsiusLights (PNVD3DCONTEXT pContext)
{
    DWORD         dwColorMaterialArgs;
    DWORD         dwControl = 0;
    DWORD         dwLightNum;
    DWORD         dwMask = 0;
    D3DCOLORVALUE dcvAmbient, dcvEmissive;
    D3DVECTOR     eyeDirection;
    D3DVECTOR     dir, hv, tmp;          // light directions
    pnvLight      pLight;
    LPD3DLIGHT7   pLight7;
#ifdef CELSIUS_TEX_MATRIX_HACK
    DWORD         dwHackMask, dwHWStage, dwD3DStage;
    LPD3DMATRIX   pTexMatrix;
#endif  // CELSIUS_TEX_MATRIX_HACK
    BOOL          bSpecularNeeded;
    DWORD         dwInvMVNeeded;

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_LIGHTS;

    // specular enable
    bSpecularNeeded = ((pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]) ||
                       (pContext->hwState.dwStateFlags & CELSIUS_FLAG_COMBINERSNEEDSPECULAR)) ? TRUE : FALSE;
    pContext->hwState.celsius.set (NV056_SET_SPECULAR_ENABLE, DRF_NUM(056, _SET_SPECULAR_ENABLE, _V, bSpecularNeeded));


    // disable hardware lighting if app has disabled it or if we're in passthrough mode
    // (yes, stupid apps will try to enable lighting without giving us normals, etc...)
    if (!GET_LIGHTING_STATE(pContext) || (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE)) {

        // disable lighting
        pContext->hwState.celsius.set (NV056_SET_LIGHTING_ENABLE, NV056_SET_LIGHTING_ENABLE_V_FALSE);

        dwInvMVNeeded = 0;

        // lighting control
        dwControl |= (DRF_DEF(056, _SET_LIGHT_CONTROL, _LOCALEYE, _FALSE) |
                      DRF_DEF(056, _SET_LIGHT_CONTROL, _ATTENUATION_MODE, _INVERT));

        if (bSpecularNeeded) {
            dwControl |= (DRF_DEF(056, _SET_LIGHT_CONTROL, _SEPARATE_SPECULAR_EN, _TRUE) |  // don't force specular to 0
                          DRF_DEF(056, _SET_LIGHT_CONTROL, _SECONDARY_COLOR_EN, _TRUE));    // get specular from user input
        }
        else {
            dwControl |= (DRF_DEF(056, _SET_LIGHT_CONTROL, _SEPARATE_SPECULAR_EN, _FALSE) | // force specular to 0
                          DRF_DEF(056, _SET_LIGHT_CONTROL, _SECONDARY_COLOR_EN, _FALSE));   // doesn't actually matter where we get it from
        }
        pContext->hwState.celsius.set (NV056_SET_LIGHT_CONTROL, dwControl);

    }

    else {

        // we shouldn't be in passthru mode if we're really going to do lighting
        // (moreover we can't be, b/c we won't have calculated xfmInvModelView which
        // is needed for spot lights)
        nvAssert (!(pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE));

        // enable lighting
        pContext->hwState.celsius.set (NV056_SET_LIGHTING_ENABLE, NV056_SET_LIGHTING_ENABLE_V_TRUE);

        dwInvMVNeeded = CELSIUS_FLAG_LIGHTINGNEEDSINVMV;

        // lighting control
        dwControl |= DRF_DEF(056, _SET_LIGHT_CONTROL, _ATTENUATION_MODE, _INVERT);

        // use local viewer only if we really need to, since it's slow
        if ((pContext->dwRenderState[D3DRENDERSTATE_LOCALVIEWER]) &&
            (pContext->lighting.pActiveLights != NULL) &&
            (bSpecularNeeded)) {
            dwControl |= DRF_DEF(056, _SET_LIGHT_CONTROL, _LOCALEYE, _TRUE);
        }
        else {
            dwControl |= DRF_DEF(056, _SET_LIGHT_CONTROL, _LOCALEYE, _FALSE);
            // if the hardware is currently set up to use OGL's infinite
            // view vector, re-program D3D's backward view vector
            if (!getDC()->bD3DViewVector) {
                nvglSetNv10CelsiusNoOp (NV_DD_CELSIUS);
                pContext->hwState.celsius.set3f (NV056_SET_EYE_DIRECTION_SW(0), 0.f,0.f,-1.f);
                getDC()->bD3DViewVector = TRUE;
            }
        }

        // set up specular
        if (bSpecularNeeded) {
            dwControl |= (DRF_DEF(056, _SET_LIGHT_CONTROL, _SEPARATE_SPECULAR_EN, _TRUE) |  // don't force specular to 0
                          DRF_DEF(056, _SET_LIGHT_CONTROL, _SECONDARY_COLOR_EN, _FALSE));   // get specular from T&L
            nvCelsiusSetSpecularParams (pContext);
        }
        else {
            dwControl |= (DRF_DEF(056, _SET_LIGHT_CONTROL, _SEPARATE_SPECULAR_EN, _FALSE) |  // force specular to 0
                          DRF_DEF(056, _SET_LIGHT_CONTROL, _SECONDARY_COLOR_EN, _FALSE));    // doesn't actually matter where we get it from
        }

        pContext->hwState.celsius.set (NV056_SET_LIGHT_CONTROL, dwControl);

        // material properties
        dwColorMaterialArgs = getCelsiusColorMaterialFlags (pContext);

        // We need to handle each of these combinations separately, because the hardware is a bit weird in some
        // ways.  First, Celsius can't do AMBIENT and EMISSION simultaneously--so we factor the emission component
        // into ambient by adding 1.  Second, material emission doesn't seem to work when AMBIENT isn't set, so we
        // again add that component into ambient.  Last, the hardware doesn't have separate controls for material
        // ambient and scene ambient, so we fold both of those into the global ambient when necessary.
        if ((dwColorMaterialArgs & NV056_SET_COLOR_MATERIAL_V_EMISSION_AMBIENT) == NV056_SET_COLOR_MATERIAL_V_EMISSION_AMBIENT) {
            dwColorMaterialArgs &= ~NV056_SET_COLOR_MATERIAL_V_EMISSION_AMBIENT;
            dwColorMaterialArgs |= NV056_SET_COLOR_MATERIAL_V_AMBIENT;
            dcvAmbient.dvR = pContext->lighting.dcvAmbient.dvR + 1.0f;
            dcvAmbient.dvG = pContext->lighting.dcvAmbient.dvG + 1.0f;
            dcvAmbient.dvB = pContext->lighting.dcvAmbient.dvB + 1.0f;
            dcvEmissive.dvR = 0.0f;
            dcvEmissive.dvG = 0.0f;
            dcvEmissive.dvB = 0.0f;
        }
        else if ((dwColorMaterialArgs & NV056_SET_COLOR_MATERIAL_V_AMBIENT) == NV056_SET_COLOR_MATERIAL_V_AMBIENT) {
            dcvAmbient.dvR  = pContext->lighting.dcvAmbient.dvR;
            dcvAmbient.dvG = pContext->lighting.dcvAmbient.dvG;
            dcvAmbient.dvB = pContext->lighting.dcvAmbient.dvB;
            dcvEmissive.dvR = pContext->Material.dcvEmissive.dvR;
            dcvEmissive.dvG = pContext->Material.dcvEmissive.dvG;
            dcvEmissive.dvB = pContext->Material.dcvEmissive.dvB;
        }
        else if ((dwColorMaterialArgs & NV056_SET_COLOR_MATERIAL_V_EMISSION) == NV056_SET_COLOR_MATERIAL_V_EMISSION) {
            dcvAmbient.dvR = pContext->lighting.dcvAmbient.dvR * pContext->Material.dcvAmbient.dvR;
            dcvAmbient.dvG = pContext->lighting.dcvAmbient.dvG * pContext->Material.dcvAmbient.dvG;
            dcvAmbient.dvB = pContext->lighting.dcvAmbient.dvB * pContext->Material.dcvAmbient.dvB;
            dcvEmissive.dvR = 0.0f;
            dcvEmissive.dvG = 0.0f;
            dcvEmissive.dvB = 0.0f;
        }
        else {
            dcvAmbient.dvR = pContext->lighting.dcvAmbient.dvR * pContext->Material.dcvAmbient.dvR + pContext->Material.dcvEmissive.dvR;
            dcvAmbient.dvG = pContext->lighting.dcvAmbient.dvG * pContext->Material.dcvAmbient.dvG + pContext->Material.dcvEmissive.dvG;
            dcvAmbient.dvB = pContext->lighting.dcvAmbient.dvB * pContext->Material.dcvAmbient.dvB + pContext->Material.dcvEmissive.dvB;
            dcvEmissive.dvR = 0.0f;
            dcvEmissive.dvG = 0.0f;
            dcvEmissive.dvB = 0.0f;
        }

        pContext->hwState.celsius.set (NV056_SET_COLOR_MATERIAL, dwColorMaterialArgs);

        pContext->hwState.celsius.set3f (NV056_SET_SCENE_AMBIENT_COLOR(0), dcvAmbient.dvR,  dcvAmbient.dvG,  dcvAmbient.dvB);
        pContext->hwState.celsius.set3f (NV056_SET_MATERIAL_EMISSION(0),   dcvEmissive.dvR, dcvEmissive.dvG, dcvEmissive.dvB);

        pContext->hwState.celsius.setf  (NV056_SET_MATERIAL_ALPHA,         pContext->Material.diffuse.a);

        // Set the eye position and direction
        eyeDirection = vectorZero;
        eyeDirection.dvZ = -1.0f;

    #ifdef CELSIUS_TEX_MATRIX_HACK
        dwHackMask = pContext->hwState.dwStateFlags & (CELSIUS_FLAG_TEXMATRIXHACK(0) | CELSIUS_FLAG_TEXMATRIXHACK(1));
        // the hack can't be enabled for both stages simultaneously
        nvAssert (dwHackMask != (CELSIUS_FLAG_TEXMATRIXHACK(0) | CELSIUS_FLAG_TEXMATRIXHACK(1)));

        if (dwHackMask) {
            dwHWStage  = (dwHackMask & CELSIUS_FLAG_TEXMATRIXHACK(0)) ? 0 : 1;
            dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
            pTexMatrix = &(pContext->tssState[dwD3DStage].mTexTransformMatrix);
            XformBy4x3 (&tmp, &eyeDirection, 0.f, pTexMatrix);
            NormalizeVector3 (&tmp);
            eyeDirection = tmp;
        }
    #endif  // CELSIUS_TEX_MATRIX_HACK

        // Download transform and lighting methods. Pack lights and send them

        dwMask = 0;
        dwLightNum = 0;

        for (pLight=pContext->lighting.pActiveLights; pLight!=NULL; pLight=pLight->pNext) {

            if (dwLightNum == 8) break; // too many

            pLight7 = &(pLight->Light7);
            dbgDisplayLight (pLight7);

            switch (pLight7->dltType) {

                case D3DLIGHT_POINT:
                    dwMask |= NV056_SET_LIGHT_ENABLE_MASK_LIGHT0_LOCAL << (dwLightNum << 1);

                    nvCelsiusDumpLightColors (pContext, dwLightNum, pLight7, dwColorMaterialArgs, bSpecularNeeded);
                    pContext->hwState.celsius.setf (NV056_SET_LIGHT_LOCAL_RANGE(dwLightNum), pLight7->dvRange);
                    nvCelsiusDumpLocalLightData (pContext, dwLightNum, pLight7);

                    break;

                case D3DLIGHT_SPOT:
                    dwMask |= NV056_SET_LIGHT_ENABLE_MASK_LIGHT0_SPOT << (dwLightNum << 1);

                    nvCelsiusDumpLightColors (pContext, dwLightNum, pLight7, dwColorMaterialArgs, bSpecularNeeded);
                    pContext->hwState.celsius.setf (NV056_SET_LIGHT_LOCAL_RANGE(dwLightNum), pLight7->dvRange);
                    nvCelsiusDumpLocalLightData (pContext, dwLightNum, pLight7);

                    // Transform the spotlight direction vector back
                    XformBy4x3 (&dir, &(pLight->direction), 0.f, &(pContext->xfmView));
                    // unless the view matrix has non-uniform scales in it, this is not necessary
                    // For now, we take the easy way out though.
                    NormalizeVector3 (&dir);

    #ifdef CELSIUS_TEX_MATRIX_HACK
                    if (dwHackMask) {
                        XformBy4x3 (&tmp, &dir, 0.f, pTexMatrix);
                        NormalizeVector3 (&tmp);
                        dir = tmp;
                    }
    #endif  // CELSIUS_TEX_MATRIX_HACK

                    ScaleVector3 (&dir, &dir, pLight->dvScale);
                    pContext->hwState.celsius.set3f (NV056_SET_LIGHT_SPOT_FALLOFF(dwLightNum,0),
                                                  pLight->falloffParams.L,
                                                  pLight->falloffParams.M,
                                                  pLight->falloffParams.N);
                    pContext->hwState.celsius.set4f (NV056_SET_LIGHT_SPOT_DIRECTION(dwLightNum,0),
                                                  dir.dvX, dir.dvY, dir.dvZ, pLight->dvW);

                    break;

                case D3DLIGHT_DIRECTIONAL:
                    dwMask |= NV056_SET_LIGHT_ENABLE_MASK_LIGHT0_INFINITE << (dwLightNum << 1);

                    nvCelsiusDumpLightColors (pContext, dwLightNum, pLight7, dwColorMaterialArgs, bSpecularNeeded);
                    pContext->hwState.celsius.setf (NV056_SET_LIGHT_LOCAL_RANGE(dwLightNum), 1e30f);

                    // Transform the spotlight direction vector back
                    XformBy4x3 (&dir, &(pLight->direction), 0.f, &(pContext->xfmView));
                    // unless the view matrix has non-uniform scales in it, this is not necessary
                    // For now, we take the easy way out though.
                    NormalizeVector3 (&dir);

    #ifdef CELSIUS_TEX_MATRIX_HACK
                    if (dwHackMask) {
                        XformBy4x3 (&tmp, &dir, 0.f, pTexMatrix);
                        NormalizeVector3 (&tmp);
                        dir = tmp;
                    }
    #endif  // CELSIUS_TEX_MATRIX_HACK

                    AddVectors3 (&hv, &dir, &eyeDirection);
                    NormalizeVector3 (&hv);
                    pContext->hwState.celsius.set3f (NV056_SET_LIGHT_INFINITE_HALF_VECTOR(dwLightNum,0), hv.dvX, hv.dvY, hv.dvZ);
                    pContext->hwState.celsius.set3f (NV056_SET_LIGHT_INFINITE_DIRECTION(dwLightNum,0),   dir.dvX, dir.dvY, dir.dvZ);

#ifdef STATE_CULL
                    // infinite half-vector stomps on attenuation and vice-versa
                    pContext->hwState.celsius.valid [NV056_SET_LIGHT_LOCAL_ATTENUATION(dwLightNum,0) >> 2] = 0;
#endif

                    break;

                default:
                    DPF ("unknown light type in nvSetCelsiusLights");
                    dbgD3DError();
                    break;

            } // switch

            dwLightNum++;

        } // for all lights

        // Tell HW which lights are active
        pContext->hwState.celsius.set (NV056_SET_LIGHT_ENABLE_MASK, dwMask);

    }

    pContext->hwState.SuperTri.setNumLights(dwLightNum);

    if ((pContext->hwState.dwStateFlags & CELSIUS_FLAG_LIGHTINGNEEDSINVMV) != dwInvMVNeeded) {
        pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_LIGHTINGNEEDSINVMV;
        pContext->hwState.dwStateFlags |= dwInvMVNeeded;
        pContext->hwState.dwDirtyFlags |= dwInvMVNeeded ? CELSIUS_DIRTY_TRANSFORM : 0;
    }

    // disable lights after setting them up (i.e minimum impact)
#ifdef NV_NULL_LIGHTS_DRIVER
    pContext->hwState.celsius.set (NV056_SET_SPECULAR_ENABLE, DRF_NUM(056, _SET_SPECULAR_ENABLE, _V, FALSE));
    pContext->hwState.celsius.set (NV056_SET_LIGHTING_ENABLE, NV056_SET_LIGHTING_ENABLE_V_FALSE);
#endif

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set geometric transformation matrices

HRESULT nvSetCelsiusTransform (PNVD3DCONTEXT pContext)
{
    D3DMATRIX        mMV0, mMV1, mCTM, mInvMV, mTemp;
    DWORD            dwClipH, dwClipV;
    float            fm11, fm22, fm33, fm41, fm42, fm43, fm44;
    float            fxTrans, fyTrans;
    D3DVALUE         dvClip0, dvClip1;
    pSurfaceViewport pViewport;
#ifdef CELSIUS_TEX_MATRIX_HACK
    LPD3DMATRIX      pTexMatrix;
    D3DMATRIX        mTexMatrixInv;
    DWORD            dwHackMask;
#endif

    if (pContext->pZetaBuffer && !pContext->dwEarlyCopyStrategy)
    {
        CNvObject *pNvObj = pContext->pZetaBuffer->getWrapper();
        if (pNvObj->hasCTEnabled())
        {
            if ((pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]      != D3DZB_TRUE)
             || (pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] != TRUE))
            {
                pNvObj->tagCTZE();
            }
        }
    }

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_TRANSFORM;

    pViewport = &(pContext->surfaceViewport);

    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) {

        // set up the model view for passthru

        if ((pContext->hwState.celsius.dwZEnable != D3DZB_TRUE) &&
            (!(pContext->hwState.dwStateFlags & CELSIUS_FLAG_ZFOG))) {
            // we have no need for Z. squash it to zero to eliminates the chance
            // of clipping away bogus z's that we see in apps like 3d flipcube
            mPassthruModelViewMatrix._31 = 0.0;
            mPassthruModelViewMatrix._32 = 0.0f;
        }
        else {
            if (getDC()->nvD3DRegistryData.regfZBias) {
                mPassthruModelViewMatrix._31 = pContext->hwState.dvZScale / (1.f + getDC()->nvD3DRegistryData.regfZBias);
                mPassthruModelViewMatrix._32 = getDC()->nvD3DRegistryData.regfZBias * mPassthruModelViewMatrix._31;
            }
            else {
                mPassthruModelViewMatrix._31 = pContext->hwState.dvZScale;
                mPassthruModelViewMatrix._32 = 0.0f;
            }
        }

        if ( ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_SQUASHW_MASK) == D3D_REG_SQUASHW_ENABLE)
             &&
             ( ((pContext->hwState.dwTextureUnitStatus[0] == CELSIUS_TEXUNITSTATUS_IDLE) &&
                (pContext->hwState.dwTextureUnitStatus[1] == CELSIUS_TEXUNITSTATUS_IDLE))
               ||
               (pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE] == FALSE)
             )
           )
        {
            mPassthruModelViewMatrix._41 = 0.0f;
        }

        else {

            if (pContext->hwState.celsius.dwZEnable == D3DZB_USEW) {
                // we are doing w-buffering
                mPassthruModelViewMatrix._41 = pContext->surfaceViewport.dvWFar * pContext->hwState.dvInvZScale;
            }

            else  {
                mPassthruModelViewMatrix._41 = 1.0f;

                // be sensitive to CT
                if (pContext->pZetaBuffer) {
                    CNvObject *pNvObj = pContext->pZetaBuffer->getWrapper();
                    if (pNvObj->hasCTEnabled()) {
                        if (pNvObj->isCTFront()) {
                            mPassthruModelViewMatrix._32  =  0.0f;
                            mPassthruModelViewMatrix._31 *=  0.5f;
                        }
                        else {
                            mPassthruModelViewMatrix._32  =  mPassthruModelViewMatrix._31;
                            mPassthruModelViewMatrix._31 *= -0.5f;
                        }
                    }  // CT enabled
                }  // pZetaBuffer

            }  // !wbuffering

        }  // ! squashing w

        // apply AA scales
        if (pContext->aa.isEnabled()) {
            mPassthruModelViewMatrix._11 = pContext->aa.fWidthAmplifier;
            mPassthruModelViewMatrix._21 = pContext->aa.fHeightAmplifier;
            // The best sample pattern in AA mode is for the samples to be centered over
            // the original sample.  Without the following correction, the new samples would
            // be down and to the right of the old sample.  Here, we offset the samples by 
            // a small amount such that the new samples are fairly well centered over the old
            // sample
            mPassthruModelViewMatrix._12 = CELSIUS_BORDER + 0.5 - 0.5f/pContext->aa.fWidthAmplifier;
            mPassthruModelViewMatrix._22 = CELSIUS_BORDER + 0.5 - 0.5f/pContext->aa.fHeightAmplifier;
        }
        else {
            mPassthruModelViewMatrix._11 = 1.0f;
            mPassthruModelViewMatrix._21 = 1.0f;
            mPassthruModelViewMatrix._12 = CELSIUS_BORDER;
            mPassthruModelViewMatrix._22 = CELSIUS_BORDER;
        }

#ifdef NV_NULL_FILL_DRIVER
        mPassthruModelViewMatrix._11 = 0.0f;
        mPassthruModelViewMatrix._21 = 0.0f;
#endif

        // this uses the 2nd matrix (normally used for skinning)
        pContext->hwState.celsius.setMatrixTransposed (NV056_SET_MODEL_VIEW_MATRIX1(0), &mPassthruModelViewMatrix);

        // set z clipping extents
        // if we're w-buffering in passthru mode, do the best we can to disable the HW's inevitable
        // z clipping because dumb apps may give us bad z's thinking they're unneeded.
        // otherwise, just clip z to the real range of the z-buffer
        dvClip0 = 0.0f;
        dvClip1 = (pContext->hwState.celsius.dwZEnable == D3DZB_USEW) ?
                  CELSIUS_Z_SCALE24 :
                  pContext->hwState.dvZScale;

    }

    else {  // !FVF_TRANSFORMED

        // apply AA scales
        float fx = 1.0f;
        float fy = 1.0f;
        float fdx = 0.0f;
        float fdy = 0.0f;
        if (pContext->aa.isEnabled()) {
            fx = pContext->aa.fWidthAmplifier;
            fy = pContext->aa.fHeightAmplifier;
            fdx = 0.5 - 0.5f/pContext->aa.fWidthAmplifier;
            fdy = 0.5 - 0.5f/pContext->aa.fHeightAmplifier;
        }

        // process viewport dependencies

        fxTrans = (float)(fx * pViewport->clipHorizontal.wX) + CELSIUS_BORDER + fdx;
        fyTrans = (float)(fy * pViewport->clipVertical.wY) + CELSIUS_BORDER + fdy;

        pContext->hwState.celsius.set4f (NV056_SET_VIEWPORT_OFFSET(0), fxTrans, fyTrans, 0.0f, 0.0f);

        // this is based on clipX = -1.0, clipY = 1.0

        fm11 = 0.5f * fx * (float)(pViewport->clipHorizontal.wWidth);
        fm41 = fm11;

        fm22 = -0.5f * fy * (float)(pViewport->clipVertical.wHeight);
        fm42 = -fm22;

        if (pContext->pZetaBuffer && pContext->pZetaBuffer->getWrapper()->hasCTEnabled()) {
            if (pContext->pZetaBuffer->getWrapper()->isCTFront()) {
                // z=0 => z_min/2    z=1 => z_max/2
                fm33 = 0.5f * (pContext->surfaceViewport.dvMaxZ - pContext->surfaceViewport.dvMinZ);
                fm43 = 0.5f * pContext->surfaceViewport.dvMinZ;
            }
            else {
                // z=0 => 1.0-z_min/2    z=1 => 1.0-z_max/2
                fm33 = -0.5f * (pContext->surfaceViewport.dvMaxZ - pContext->surfaceViewport.dvMinZ);
                fm43 = -0.5f * pContext->surfaceViewport.dvMinZ + 1.0f;
            }
        }
        else {
            // z=0 => z_min    z=1 => z_max
            fm33 = pContext->surfaceViewport.dvMaxZ - pContext->surfaceViewport.dvMinZ;
            fm43 = pContext->surfaceViewport.dvMinZ;
        }

        if( fm33 != 0.f ) {
            fm33 *= pContext->hwState.dvZScale;
            fm43 *= pContext->hwState.dvZScale;

            dvClip0 = fm43;         // 0*fm33 + fm43
            dvClip1 = fm33 + fm43;  // 1*fm33 + fm43
        }
        else {
            // MinZ and MaxZ are equal. It appears that the Z clip planes are not
            // inclusive. If dvClip0 == dvClip1 then primtivies are discarded. We should
            // not have Z clipping anyway (the Z transform has collapsed), so just get
            // the z clip planes out of the way.

            dvClip1 = pContext->hwState.dvZScale;
            dvClip0 = 0.f;

            // The Z clip planes can't always be moved out of the way, so muck with the matrix.
            // BUGBUG - this has been tuned to work with the Quake3 sky (w/ OGL wrapper), but
            // this might not work for all apps.
            float fDeltaZ = 0.0000001f;

            if( fm43 > 1.f - fDeltaZ ) {
                fm43 = pContext->hwState.dvZScale * (1.f - fDeltaZ);
            }
            else if( fm43 < fDeltaZ ) {
                fm43 = pContext->hwState.dvZScale * fDeltaZ;
            }
            else {
                fm43 *= pContext->hwState.dvZScale;
            }
        }

        if (pContext->hwState.celsius.dwZEnable == D3DZB_USEW) {
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

#ifdef CELSIUS_TEX_MATRIX_HACK
        dwHackMask = pContext->hwState.dwStateFlags & (CELSIUS_FLAG_TEXMATRIXHACK(0) | CELSIUS_FLAG_TEXMATRIXHACK(1));
        // the hack can't be enabled for both stages simultaneously
        nvAssert (dwHackMask != (CELSIUS_FLAG_TEXMATRIXHACK(0) | CELSIUS_FLAG_TEXMATRIXHACK(1)));

        if (dwHackMask) {
            // cache off the original modelview matrix and redefine eye space to be
            // "texture space" by incorporating the texture transform here.
            DWORD dwHWStage  = (dwHackMask & CELSIUS_FLAG_TEXMATRIXHACK(0)) ? 0 : 1;
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
            pTexMatrix = &(pContext->tssState[dwD3DStage].mTexTransformMatrix);
            mTemp = mMV0;
            MatrixProduct4x4 (&mMV0, &mTemp, pTexMatrix);
        }
#endif  // CELSIUS_TEX_MATRIX_HACK

        pContext->hwState.celsius.setMatrixTransposed (NV056_SET_MODEL_VIEW_MATRIX0(0), &mMV0);

        pContext->hwState.SuperTri.setEye(&mMV0);

        // Normals are transformed by the transpose of the inverse of the model view matrix.
        // (see Graphics Gems I, p. 541).
        // We only invert the upper 3x3 in order to eliminate the effects of translations.
        // the transposition happens because unlike all other matrices, we DON'T un-transpose
        // this one when we send it to the HW. (this is the one matrix for which D3D's
        // backwardness is convenient)

        if (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_TEXSTATENEEDSINVMV | CELSIUS_FLAG_LIGHTINGNEEDSINVMV)) {
            // there's no reason to waste time normalizing the inverse MV matrix if we're going to have to normalize
            // normals anyway. otherwise, they give us normalized stuff and we need to preserve the normalization
            Inverse4x4 (&mInvMV, &mMV0, (pContext->dwRenderState[D3DRENDERSTATE_NORMALIZENORMALS] == FALSE));
            pContext->hwState.celsius.setInverseModelViewMatrix (NV056_SET_INVERSE_MODEL_VIEW_MATRIX0(0), &mInvMV);
        }

        if (pContext->pCurrentVShader->hasProgram()) {
            // prog pipe
            pContext->hwState.celsius.setMatrixTransposed (NV056_SET_COMPOSITE_MATRIX(0), &mViewportMatrix);
        }

        else if (GET_VERTEXBLEND_STATE(pContext)) {
            // we're skinning
            // compute the second modelview matrix, its inverse, and a distilled composite matrix:
            // cheops composite = projection plus accomodations for the viewport
            MatrixProduct4x4 (&mMV1, &(pContext->xfmWorld[1]), &(pContext->xfmView));
            if (dwHackMask) {
                mTemp = mMV1;
                MatrixProduct4x4 (&mMV1, &mTemp, pTexMatrix);
            }
            pContext->hwState.celsius.setMatrixTransposed (NV056_SET_MODEL_VIEW_MATRIX1(0), &mMV1);
            if (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_TEXSTATENEEDSINVMV | CELSIUS_FLAG_LIGHTINGNEEDSINVMV)) {
                Inverse4x4 (&mInvMV, &mMV1, (pContext->dwRenderState[D3DRENDERSTATE_NORMALIZENORMALS] == FALSE));
                pContext->hwState.celsius.setInverseModelViewMatrix (NV056_SET_INVERSE_MODEL_VIEW_MATRIX1(0), &mInvMV);
            }
            MatrixProduct4x4 (&mCTM, &(pContext->xfmProj), &mViewportMatrix);
            if (dwHackMask) {
                // pre-multiply by the inverted texture matrix to undo the mess
                // we made by introducing it into the modelview matrices above
                Inverse4x4 (&mTexMatrixInv, pTexMatrix, FALSE);
                MatrixProduct4x4 (&mTemp, &mTexMatrixInv, &mCTM);
                mCTM = mTemp;
            }
            pContext->hwState.celsius.setMatrixTransposed (NV056_SET_COMPOSITE_MATRIX(0), &mCTM);
        }

        else {
            // we're not skinning
            // skip the second set of modelview matrices and compute the composite matrix normally:
            // cheops composite = D3D World*View*projection plus accomodations for the viewport
#ifdef CELSIUS_TEX_MATRIX_HACK
            if (dwHackMask) {
                // restore the original MV matrix for the sake of computing the composite matrix
                mMV0 = mTemp;
            }
#endif  // CELSIUS_TEX_MATRIX_HACK
            MatrixProduct4x4 (&mTemp,  &(pContext->xfmProj), &mViewportMatrix);
            MatrixProduct4x4 (&mCTM, &mMV0, &mTemp);
            pContext->hwState.celsius.setMatrixTransposed (NV056_SET_COMPOSITE_MATRIX(0), &mCTM);
            // for supertri we need world*view*proj
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_SUPERTRI) {
                MatrixProduct4x4 (&pContext->hwState.mTransform, &mMV0, &(pContext->xfmProj));
            }
        }

        pContext->hwState.celsius.set (NV056_SET_NORMALIZATION_ENABLE,
                                       DRF_NUM(056, _SET_NORMALIZATION_ENABLE, _V, pContext->dwRenderState[D3DRENDERSTATE_NORMALIZENORMALS]));

    }  // !FVF_TRANSFORMED

    // set z clipping
    // note that clips may be reversed because of CT, so we need the min/max functions
    pContext->hwState.celsius.set2f (NV056_SET_CLIP_MIN, min (dvClip0, dvClip1), max (dvClip0, dvClip1));

    // set window clipping
    // we subtract one from xmax (ymax) because the HW clips to xmin <= x <= xmax
    // ie. it's inclusive of the upper limit. note this makes it impossible to clip to nothing
    DWORD x,y,w,h;
    if (pContext->aa.isEnabled()) {
        float s;
        s = pContext->aa.fWidthAmplifier;
        x = DWORD(0.5f + pViewport->clipHorizontal.wX * s);
        //No one remembers the why of this (below) and we think its bogus
        //but leaving this alternative here commented just in case this is
        //revisited
        //w = DWORD(pViewport->clipHorizontal.wWidth * s + s - 1);
        w = DWORD(pViewport->clipHorizontal.wWidth * s );

        s = pContext->aa.fHeightAmplifier;
        y = DWORD(0.5f + pViewport->clipVertical.wY * s);
        //why??? h = DWORD(pViewport->clipVertical.wHeight * s + s - 1);
        h = DWORD(pViewport->clipVertical.wHeight * s );
    }
    else {
        x = pViewport->clipHorizontal.wX;
        w = pViewport->clipHorizontal.wWidth;
        y = pViewport->clipVertical.wY;
        h = pViewport->clipVertical.wHeight;
    }

    dwClipH = DRF_NUM(056, _SET_WINDOW_CLIP_HORIZONTAL, _XMIN,
                      ((x - CELSIUS_VIEWPORT_BOUND) & 0xfff))  |
              DRF_NUM(056, _SET_WINDOW_CLIP_HORIZONTAL, _XMAX,
                      ((x + w - 1 - CELSIUS_VIEWPORT_BOUND) & 0xfff));
    dwClipV = DRF_NUM(056, _SET_WINDOW_CLIP_VERTICAL, _YMIN,
                      ((y - CELSIUS_VIEWPORT_BOUND) & 0xfff))  |
              DRF_NUM(056, _SET_WINDOW_CLIP_VERTICAL, _YMAX,
                      ((y + h - 1 - CELSIUS_VIEWPORT_BOUND) & 0xfff));

    pContext->hwState.celsius.prep_wait_for_idle_nv10();
    pContext->hwState.celsius.set (NV056_SET_WINDOW_CLIP_HORIZONTAL(0), dwClipH);
    pContext->hwState.celsius.set (NV056_SET_WINDOW_CLIP_VERTICAL(0),   dwClipV);

    // BUBUG
    // This fixes a problem with the Billboard SDK app.  It sends down new vertices, and
    // changes the transform state for each primitive it renders, 4 vertices each
    // It seems likely that we should invalidate the cache here, but it seems just
    // as likely that the hardware should take care of this by itself.  Awaiting more
    // info
    nvglSetNv10CelsiusInvalidateVertexCache (NV_DD_CELSIUS);

    return (D3D_OK);
}


//---------------------------------------------------------------------------

// set everything else not covered by other celsius state routines. this includes:
//      - depth testing
//      - stencil testing
//      - alpha testing
//      - alpha blending

HRESULT nvSetCelsiusMiscState (PNVD3DCONTEXT pContext)
{
    DWORD dwMask, dwZBiasRS;
    DWORD dwSrcFactor, dwDstFactor;
    DWORD dwPointSize;
    float fZBias, fZScale;
    float a, t0, t1, t2, t3, t4, t5;  //point size variables
    float fSizeMin,fSizeMax,fSize;
    float aten1,aten2,aten3,fHeight, fScale;

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_MISC_STATE;

    // z-buffering
    if (pContext->hwState.celsius.dwZEnable != D3DZB_FALSE) {
        pContext->hwState.celsius.set (NV056_SET_DEPTH_TEST_ENABLE, NV056_SET_DEPTH_TEST_ENABLE_V_TRUE);
        pContext->hwState.celsius.set (NV056_SET_DEPTH_MASK,        DRF_NUM(056, _SET_DEPTH_MASK, _V, pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE]));
        pContext->hwState.celsius.set (NV056_SET_DEPTH_FUNC,        nv056DepthFunc[CTFunc(pContext,pContext->dwRenderState[D3DRENDERSTATE_ZFUNC])]);
    }
    else {
        pContext->hwState.celsius.set (NV056_SET_DEPTH_TEST_ENABLE, NV056_SET_DEPTH_TEST_ENABLE_V_FALSE);
    }

    // fill mode
    switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]) {
        case D3DFILL_POINT:
            pContext->hwState.celsius.set (NV056_SET_FRONT_POLYGON_MODE, NV056_SET_FRONT_POLYGON_MODE_V_POINT);
            pContext->hwState.celsius.set (NV056_SET_BACK_POLYGON_MODE,  NV056_SET_BACK_POLYGON_MODE_V_POINT);
            break;
        case D3DFILL_WIREFRAME:
            pContext->hwState.celsius.set (NV056_SET_FRONT_POLYGON_MODE, NV056_SET_FRONT_POLYGON_MODE_V_LINE);
            pContext->hwState.celsius.set (NV056_SET_BACK_POLYGON_MODE,  NV056_SET_BACK_POLYGON_MODE_V_LINE);
            break;
        case D3DFILL_SOLID:
            pContext->hwState.celsius.set (NV056_SET_FRONT_POLYGON_MODE, NV056_SET_FRONT_POLYGON_MODE_V_FILL);
            pContext->hwState.celsius.set (NV056_SET_BACK_POLYGON_MODE,  NV056_SET_BACK_POLYGON_MODE_V_FILL);
            break;
        default:
            DPF ("unknown FILLMODE");
            dbgD3DError();
            break;
    }

    // shading
    pContext->hwState.celsius.set (NV056_SET_SHADE_MODE,(pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE] == D3DSHADE_FLAT)
                                                     ? NV056_SET_SHADE_MODE_V_FLAT
                                                     : NV056_SET_SHADE_MODE_V_SMOOTH);

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

    pContext->hwState.celsius.set (NV056_SET_COLOR_MASK, dwMask);

    // alpha blending
    if (pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]) {
        pContext->hwState.celsius.set (NV056_SET_BLEND_ENABLE, NV056_SET_BLEND_ENABLE_V_TRUE);
        dwSrcFactor = nv056BlendFunc[pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]];
        dwDstFactor = nv056BlendFunc[pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]];
        // for these two cases, we need to go back and override dwDstFactor
        switch (pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]) {
            case D3DBLEND_BOTHSRCALPHA:    // obsolete, but we'll do it for Dx5 and prior
                dwDstFactor = NV056_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_ALPHA;
                break;
            case D3DBLEND_BOTHINVSRCALPHA: // likewise
                dwDstFactor = NV056_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA;
                break;
            default:
                // nada
                break;
        }

        pContext->hwState.celsius.set2 (NV056_SET_BLEND_FUNC_SFACTOR, dwSrcFactor, dwDstFactor);
        pContext->hwState.celsius.set (NV056_SET_BLEND_EQUATION, nv056BlendOp[pContext->dwRenderState[D3DRS_BLENDOP]]);
      }
    else {
        pContext->hwState.celsius.set (NV056_SET_BLEND_ENABLE, NV056_SET_BLEND_ENABLE_V_FALSE);
    }

    // culling
    switch (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]) {
        case D3DCULL_CW:
            pContext->hwState.celsius.set (NV056_SET_CULL_FACE_ENABLE, NV056_SET_CULL_FACE_ENABLE_V_TRUE);
            pContext->hwState.celsius.set (NV056_SET_FRONT_FACE,       NV056_SET_FRONT_FACE_V_CCW);
            break;
        case D3DCULL_CCW:
            pContext->hwState.celsius.set (NV056_SET_CULL_FACE_ENABLE, NV056_SET_CULL_FACE_ENABLE_V_TRUE);
            pContext->hwState.celsius.set (NV056_SET_FRONT_FACE,       NV056_SET_FRONT_FACE_V_CW);
            break;
        default:
            DPF ("unknown cull mode");
        case D3DCULL_NONE:
            pContext->hwState.celsius.set (NV056_SET_CULL_FACE_ENABLE, NV056_SET_CULL_FACE_ENABLE_V_FALSE);
            break;
    }

    if (pContext->dwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]) {
        pContext->hwState.celsius.set (NV056_SET_ALPHA_TEST_ENABLE, NV056_SET_ALPHA_TEST_ENABLE_V_TRUE);
        pContext->hwState.celsius.set (NV056_SET_ALPHA_REF,         pContext->dwRenderState[D3DRENDERSTATE_ALPHAREF]);
        pContext->hwState.celsius.set (NV056_SET_ALPHA_FUNC,        nv056AlphaFunc[pContext->dwRenderState[D3DRENDERSTATE_ALPHAFUNC]]);
    }
    else {
        pContext->hwState.celsius.set (NV056_SET_ALPHA_TEST_ENABLE, NV056_SET_ALPHA_TEST_ENABLE_V_FALSE);
    }
#ifdef ALPHA_CULL
    // alpha test
#if ALPHA_CULL ==0
    pContext->hwState.alphacull_mode = 0;
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
                pContext->hwState.celsius.set (NV056_SET_ALPHA_TEST_ENABLE, NV056_SET_ALPHA_TEST_ENABLE_V_TRUE);
                pContext->hwState.celsius.set (NV056_SET_ALPHA_REF, 0x8);
                pContext->hwState.celsius.set (NV056_SET_ALPHA_FUNC, NV056_SET_ALPHA_FUNC_V_GREATER);
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
            pContext->hwState.celsius.set (NV056_SET_COMBINER_COLOR_ICW(0), 0x08040820);
            pContext->hwState.celsius.set (NV056_SET_COMBINER0_COLOR_OCW,   0x000010CD);
            pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_ICW(0), 0x18200000);
            pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_OCW(0), 0x00000C00);
            pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW0, pContext->hwState.specfog_cw[0]);
            pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW1, 0x0D00 | (pContext->hwState.specfog_cw[1] & ~0x1f00));
            pContext->hwState.celsius.set (NV056_SET_ALPHA_TEST_ENABLE, NV056_SET_ALPHA_TEST_ENABLE_V_TRUE);
            pContext->hwState.celsius.set (NV056_SET_ALPHA_REF, 0x8);
            pContext->hwState.celsius.set (NV056_SET_ALPHA_FUNC, NV056_SET_ALPHA_FUNC_V_GREATER);
        }
#endif
        else if(pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_INVSRCALPHA &&
                pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_SRCALPHA){
                pContext->hwState.celsius.set (NV056_SET_ALPHA_TEST_ENABLE, NV056_SET_ALPHA_TEST_ENABLE_V_TRUE);
                pContext->hwState.celsius.set (NV056_SET_ALPHA_REF, 0xf4);
                pContext->hwState.celsius.set (NV056_SET_ALPHA_FUNC, NV056_SET_ALPHA_FUNC_V_LESS);
        }
        else if(pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_ZERO){
            if(pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_SRCALPHA){
                pContext->hwState.celsius.set (NV056_SET_ALPHA_TEST_ENABLE, NV056_SET_ALPHA_TEST_ENABLE_V_TRUE);
                pContext->hwState.celsius.set (NV056_SET_ALPHA_REF, 0xf4);
                pContext->hwState.celsius.set (NV056_SET_ALPHA_FUNC, NV056_SET_ALPHA_FUNC_V_LESS);
            }
            else if(pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_INVSRCALPHA){
                pContext->hwState.celsius.set (NV056_SET_ALPHA_TEST_ENABLE, NV056_SET_ALPHA_TEST_ENABLE_V_TRUE);
                pContext->hwState.celsius.set (NV056_SET_ALPHA_REF, 0x8);
                pContext->hwState.celsius.set (NV056_SET_ALPHA_FUNC, NV056_SET_ALPHA_FUNC_V_GREATER);
            }
        }
    }
#endif

    //anti aliased edges
    if (getDC()->nvD3DPerfData.dwHaveAALines){
        if (pContext->dwRenderState[D3DRENDERSTATE_EDGEANTIALIAS]){
            pContext->hwState.celsius.set (NV056_SET_BLEND_ENABLE, NV056_SET_LINE_SMOOTH_ENABLE_V_TRUE);
        }
        else {
            pContext->hwState.celsius.set (NV056_SET_LINE_SMOOTH_ENABLE, NV056_SET_LINE_SMOOTH_ENABLE_V_FALSE);
        }
    }

    // dithering
    pContext->hwState.celsius.set (NV056_SET_DITHER_ENABLE, DRF_NUM(056, _SET_DITHER_ENABLE, _V, pContext->dwRenderState[D3DRENDERSTATE_DITHERENABLE]));

    // z bias
    dwZBiasRS = (pContext->dwRenderState[D3DRENDERSTATE_ZBIAS] != 0) ? 1 : 0;
    // switch on an arbitrary identifier
    switch (dwZBiasRS * pContext->hwState.celsius.dwZEnable) {
        case D3DZB_TRUE:
            // renderstate_zbias!=0 and zenable==D3DZB_TRUE.
            // the bias is applied to z values in device coordinates
            // (i.e in the range [0..2^16-1] or [0..2^24-1])
            pContext->hwState.celsius.set (NV056_SET_POLY_OFFSET_POINT_ENABLE, NV056_SET_POLY_OFFSET_POINT_ENABLE_V_TRUE);
            pContext->hwState.celsius.set (NV056_SET_POLY_OFFSET_LINE_ENABLE,  NV056_SET_POLY_OFFSET_LINE_ENABLE_V_TRUE);
            pContext->hwState.celsius.set (NV056_SET_POLY_OFFSET_FILL_ENABLE,  NV056_SET_POLY_OFFSET_FILL_ENABLE_V_TRUE);
            fZBias  = -(float)(pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]);
            fZScale = 0.25f * fZBias;
            pContext->hwState.celsius.set2f (NV056_SET_POLYGON_OFFSET_SCALE_FACTOR, fZScale, fZBias);
            break;
        case D3DZB_USEW:
            // renderstate_zbias!=0 and zenable==D3DZB_USEW
            // D3D would want us to apply the bias to w instead of z, but we can't
            // really do this, so note a setup failure and fall through to disable
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
        case 0:
            // renderstate_zbias==0 and/or zenable==D3DZB_FALSE
            pContext->hwState.celsius.set (NV056_SET_POLY_OFFSET_POINT_ENABLE, NV056_SET_POLY_OFFSET_POINT_ENABLE_V_FALSE);
            pContext->hwState.celsius.set (NV056_SET_POLY_OFFSET_LINE_ENABLE,  NV056_SET_POLY_OFFSET_LINE_ENABLE_V_FALSE);
            pContext->hwState.celsius.set (NV056_SET_POLY_OFFSET_FILL_ENABLE,  NV056_SET_POLY_OFFSET_FILL_ENABLE_V_FALSE);
            break;
        default:
            DPF ("oops. i goofed. got unexpected zbias case");
            dbgD3DError();
            break;
    }

    CNvObject *pNvObj = pContext->pZetaBuffer ? pContext->pZetaBuffer->getWrapper() : NULL;

    if (pNvObj) {
        if (pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]) {
            pNvObj->tagAltStencilBadSemantics();
            pNvObj->tagAltStencilDisabled();
        }
    }

    // stencil
    if (pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]) {
        pContext->bStencilEnabled = TRUE;
        pContext->hwState.celsius.set (NV056_SET_STENCIL_TEST_ENABLE, NV056_SET_STENCIL_TEST_ENABLE_V_TRUE);
        pContext->hwState.celsius.set (NV056_SET_STENCIL_OP_FAIL,     nv056StencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL]]);
        pContext->hwState.celsius.set (NV056_SET_STENCIL_OP_ZFAIL,    nv056StencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL]]);
        pContext->hwState.celsius.set (NV056_SET_STENCIL_OP_ZPASS,    nv056StencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]]);
        pContext->hwState.celsius.set (NV056_SET_STENCIL_FUNC,        nv056StencilFunc[pContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC]]);
        pContext->hwState.celsius.set (NV056_SET_STENCIL_FUNC_REF,    pContext->dwRenderState[D3DRENDERSTATE_STENCILREF]       & 0xff);
        pContext->hwState.celsius.set (NV056_SET_STENCIL_FUNC_MASK,   pContext->dwRenderState[D3DRENDERSTATE_STENCILMASK]      & 0xff);
        pContext->hwState.celsius.set (NV056_SET_STENCIL_MASK,        pContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK] & 0xff);
    }
    else if (pNvObj && pNvObj->isAltStencilEnabled()) {
        // Do nothing; stencil is set elsewhere
    } else {
        pContext->hwState.celsius.set (NV056_SET_STENCIL_TEST_ENABLE, NV056_SET_STENCIL_TEST_ENABLE_V_FALSE);
    }

    // combiner factor
    pContext->hwState.celsius.set (NV056_SET_COMBINE_FACTOR(0), pContext->dwRenderState[D3DRENDERSTATE_TEXTUREFACTOR]);
    pContext->hwState.celsius.set (NV056_SET_COMBINE_FACTOR(1), pContext->dwRenderState[D3DRENDERSTATE_TEXTUREFACTOR]);

    // skinning
    pContext->hwState.celsius.set (NV056_SET_SKIN_ENABLE,(GET_VERTEXBLEND_STATE(pContext) ?
                                                       DRF_DEF(056, _SET_SKIN_ENABLE, _V, _TRUE) :
                                                       DRF_DEF(056, _SET_SKIN_ENABLE, _V, _FALSE)));

    // line and point sizes for AA
    if (pContext->aa.isEnabled()) {
        pContext->hwState.celsius.set (NV056_SET_LINE_WIDTH, pContext->aa.dwLinePointSize);
        pContext->hwState.celsius.set (NV056_SET_POINT_SIZE, pContext->aa.dwLinePointSize);
    }
    else {
        pContext->hwState.celsius.set (NV056_SET_LINE_WIDTH, 0x00000008);  // 6.3 format
        pContext->hwState.celsius.set (NV056_SET_POINT_SIZE, 0x00000008);  // 6.3 format
    }

    // Only do the pt. size renderstate if app is dx8 app.
    if (pContext->dwDXAppVersion >= 0x800)
    {

        fSizeMin = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSIZE_MIN]);
        fSizeMax = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSIZE_MAX]);
        fSize    = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSIZE]);
        fScale = fSize;
        aten1    = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSCALE_A]);
        aten2    = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSCALE_B]);
        aten3    = FLOAT_FROM_DWORD (pContext->dwRenderState[D3DRS_POINTSCALE_C]);
        if (fScale <= 0.0f) fScale = 0.0001f;
        if (fSizeMin <= 0.0f) fSizeMin =  0.0001f;   //watch out for divide by zero....
        if (fSizeMin > 63.869f) fSizeMin = 63.869f;   //watch out for divide by zero....
        if (fSizeMax > 63.870f) fSizeMax = 63.870f;
        if (fSizeMax < 0.0f) fSizeMax = 0.0f;
        //if (pContext->dwRenderState[D3DRS_POINTSCALEENABLE]) fSize *= pContext->surfaceViewport.clipVertical.wHeight;
        if (fSize > fSizeMax ) fSize = fSizeMax;
        if (fSize < fSizeMin ) fSize = fSizeMin;
        t3 = fSizeMax - fSizeMin;
        if (t3) t4 = -fSizeMin / t3;
        t5 = fSizeMin;

        fHeight = 1;
        if ( pContext->surfaceViewport.clipVertical.wHeight &&
             pContext->dwRenderState[D3DRS_POINTSCALEENABLE])
        {
            fHeight = (float)(pContext->surfaceViewport.clipVertical.wHeight);
            //have to do this loop if we want to move height into the attenuation
            //factors because valid point sprite sizes < 0.125 will be expressed
            //as zeros if we don't do something  HMH
            while (fSize < 32.0f && fHeight > 2.0f)
            {
                fSize *= 2;
                fScale *= 2;
                fHeight *= 0.5f;
            }
        }

        a = (t3 / fScale) * (t3 / fScale) * (1.0f / (fHeight * fHeight) );

        t0 = aten1 * a;
        t1 = aten2 * a;
        t2 = aten3 * a;

        dwPointSize = ((DWORD)(fSize * 8.0f)) & 0x1ff;  // convert to 6.3

        pContext->hwState.celsius.set3f (NV056_SET_POINT_PARAMS(0), t0,t1,t2);
        pContext->hwState.celsius.set3f (NV056_SET_POINT_PARAMS(3), t3,t3,t3);
        pContext->hwState.celsius.setf (NV056_SET_POINT_PARAMS(6), t4);
        pContext->hwState.celsius.setf (NV056_SET_POINT_PARAMS(7), t5);
        pContext->hwState.celsius.set (NV056_SET_POINT_SIZE, dwPointSize);  // 6.3 format
        pContext->hwState.celsius.set (NV056_SET_POINT_PARAMS_ENABLE, pContext->dwRenderState[D3DRS_POINTSCALEENABLE] ?
                                                                   NV056_SET_POINT_PARAMS_ENABLE_V_TRUE :
                                                                   NV056_SET_POINT_PARAMS_ENABLE_V_FALSE);
        pContext->hwState.celsius.set (NV056_SET_POINT_SMOOTH_ENABLE, pContext->dwRenderState[D3DRS_POINTSPRITEENABLE] ?
                                                                   NV056_SET_POINT_SMOOTH_ENABLE_V_TRUE:
                                                                   NV056_SET_POINT_SMOOTH_ENABLE_V_FALSE);


    }

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// This handles the SetControl0 method:
//      premultiplied alpha
//      texture perspective
//      z perspective
//      z format
//      alpha kill mode
//      stencil write enable

HRESULT nvSetCelsiusControl0 (PNVD3DCONTEXT pContext)
{
    DWORD dwMask = 0;

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_CONTROL0;

    // SRWNOTE: D3D has the possibility of having pre-multiplied alpha in 1
    // texture stage and not in another stage. How do we handle this?
    // hmm... actually this seems to be specified on a per surface basis
    // hedge on this for now
// BUGBUG
#if 0
    if (statev.premultipliedalpha)
        dwMask |= DRF_DEF(056, _SET_CONTROL0, _PREMULTIPLIEDALPHA, _TRUE);
    else
        dwMask |= DRF_DEF(056, _SET_CONTROL0, _PREMULTIPLIEDALPHA, _FALSE);
#endif

#ifdef CELSIUS_ENABLE_BAD_PERSPECTIVE
    // this method is horribly named. setting it to false forces w to 1.0 in primitive assembly
    // which actually has almost nothing to do with texture perspective, because the w-divide that
    // perspectively corrects textures has already been done in the transform unit.
    // the only things actually affected are those downstream of primitive assembly:
    // - perspectively correct color interpolation
    // - w-clipping
    // this should not ever be disabled in T&L mode because of the latter!
    dwMask |= (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) ?
              DRF_NUM(056, _SET_CONTROL0, _TEXTUREPERSPECTIVE, pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE]) :
              DRF_DEF(056, _SET_CONTROL0, _TEXTUREPERSPECTIVE, _TRUE);
#else
    dwMask |= DRF_DEF(056, _SET_CONTROL0, _TEXTUREPERSPECTIVE, _TRUE);
#endif

    // w or z buffering
    dwMask |= DRF_NUM(056, _SET_CONTROL0, _Z_PERSPECTIVE_ENABLE, (DWORD)(pContext->hwState.celsius.dwZEnable == D3DZB_USEW));

    // D3D has no provision for specification of zbuffer format
    // somewhat arbitrarily choose FIXED. alternately we could select it
    // to their advantage according to whether w-buffering is enabled (?)
    dwMask |= DRF_DEF(056, _SET_CONTROL0, _Z_FORMAT, _FIXED);

    if (pContext->hwState.celsius.dwZEnable == D3DZB_USEW) {
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_WSOURCE(0)) {
            // pull w-buffer values from texture unit 0
            dwMask |= DRF_DEF(056, _SET_CONTROL0, _WBUFFER_SELECT, _0);
        }
        else if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_WSOURCE(1)) {
            // pull w-buffer values from texture unit 1
            dwMask |= DRF_DEF(056, _SET_CONTROL0, _WBUFFER_SELECT, _1);
        }
        else {
            // w-buffering with no available w value
            nvAssert(0);
        }
    }
    else {
        // disable w-buffering
        dwMask |= DRF_DEF(056, _SET_CONTROL0, _WBUFFER_SELECT, _0);
    }

    CNvObject *pNvObj = pContext->pZetaBuffer ? pContext->pZetaBuffer->getWrapper() : NULL;

    dwMask |= (pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE] || (pNvObj && pNvObj->isAltStencilEnabled())) ?
        DRF_DEF(056, _SET_CONTROL0, _STENCIL_WRITE_ENABLE, _TRUE) :
        DRF_DEF(056, _SET_CONTROL0, _STENCIL_WRITE_ENABLE, _FALSE);

    pContext->hwState.celsius.set (NV056_SET_CONTROL0, dwMask);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// returns the size (in 4-byte words) of an inlined vertex

DWORD nvCelsiusInlineVertexStride (PNVD3DCONTEXT pContext)
{
    CVertexShader *pShader;
    DWORD          dwStride;

    pShader = pContext->pCurrentVShader;

    dwStride  = 0;
    dwStride += pShader->bVAExists (defaultInputRegMap[D3DVSDE_BLENDWEIGHT]) ? sizeof(DWORD) : 0;
    dwStride += pShader->getVASize (defaultInputRegMap[D3DVSDE_NORMAL]);

    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1)) {
        DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[1];
        nvAssert (dwD3DStage != CELSIUS_UNUSED);
        DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 16) & 0xffff;
        DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        nvAssert (pShader->bVAExists (dwVAIndex));  // they better have given us coordinates
        DWORD n = (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) | CELSIUS_FLAG_TEXMATRIXSWFIX(1))) ?
                  4*sizeof(DWORD) : pShader->getVASize (dwVAIndex);
        dwStride += n;
    }
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)) {
        DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[0];
        nvAssert (dwD3DStage != CELSIUS_UNUSED);
        DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >>  0) & 0xffff;
        DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        nvAssert (pShader->bVAExists (dwVAIndex));  // they better have given us coordinates
        DWORD n = (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_TEXMATRIXSWFIX(0))) ?
                  4*sizeof(DWORD) : pShader->getVASize (dwVAIndex);
        dwStride += n;
    }

    dwStride += pShader->getVASize (defaultInputRegMap[D3DVSDE_SPECULAR]);
    dwStride += pShader->getVASize (defaultInputRegMap[D3DVSDE_DIFFUSE]);
    dwStride += pShader->getVASize (defaultInputRegMap[D3DVSDE_POSITION]);

    //don't actually add in point size because we we remove it in the inner loop HMH
    //    dwStride += (fvfData.dwPointSizeMask ? 1 : 0);

    return (dwStride >> 2);
}

//---------------------------------------------------------------------------

// set sources and offsets of various vertex components.

HRESULT nvSetCelsiusVertexFormat (PNVD3DCONTEXT pContext)
{
    DWORD   dwFormat[7], dwD3DStage, dwTexCount;
    DWORD   dwTCIndex, dwVAIndex;
    DWORD   dwVertexBufferOffset;
    DWORD   dwFormatShadow = 0;
    DWORD   dwStreamSelector;
    BOOL    bInlineVertices, bTess;
    DWORD   dwInlineStride, dwStride;
    DWORD   dwOffsetAdjustment;

    // clear dirty flag
    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_FVF;

    // calculate a tessellator style FVF format
    bTess = pContext->hwState.dwStateFlags & CELSIUS_FLAG_COMPUTE_TESS_VTX_STRIDE;

    CVertexShader  *pShader   = pContext->pCurrentVShader;
    CVertexBuffer **ppStreams = pContext->ppDX8Streams;

    // shadow the values so we know what we last sent to the HW
    pContext->hwState.dwVertexOffset = pContext->dp2.dwVertexBufferOffset;
    pContext->hwState.pVertexShader  = pContext->pCurrentVShader;
    nvMemCopy (pContext->hwState.ppStreams, pContext->ppDX8Streams, CELSIUS_CAPS_MAX_STREAMS*sizeof(CVertexBuffer *));

    // find the stream containing geometry
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_POSITION]);
    if ((dwStreamSelector == CVertexShader::VA_STREAM_NONE) || (ppStreams[dwStreamSelector] == NULL)) {
        // we don't have vertex data yet (happens if called via validateTSS())
        return (TRUE);
    }

#ifdef STOMP_TEX_COORDS
    pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_STOMP_4TH_COORD_MASK;

    if (!pShader->hasProgram()) {
        for (DWORD dwHWStage=0; dwHWStage < CELSIUS_NUM_TEXTURES; dwHWStage++) {
            if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(dwHWStage)) {
                dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (16*dwHWStage)) & 0xffff;
                if ((pShader->getVASize(defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex]) == 16) &&
                    (CELSIUS_GET_NUM_TEX_COORDS(pContext->hwState.dwNumTexCoordsNeeded, dwHWStage) < 4)) {
                    pContext->hwState.dwStateFlags |= CELSIUS_FLAG_STOMP_4TH_COORD(dwHWStage);
                }
            }
        }
    }

    // This should only happen in WHQL.  If it's happening elsewhere, either the app
    // is really stupid, or we're doing something wrong.
    nvAssert(!(pContext->hwState.dwStateFlags & CELSIUS_FLAG_STOMP_4TH_COORD_MASK));
#endif  // STOMP_TEX_COORDS

    // figure out if we need to inline this vertex data
    bInlineVertices = (NV_NUM_UNIQUE_STREAMS(pContext->dwStreamDMACount) > 1)
                      ||
                      (NV_NUM_SYSTEM_STREAMS(pContext->dwStreamDMACount) > 0)
                      ||
                      (pShader->hasProgram())
                      ||
                      (pShader->bVAExists(defaultInputRegMap[D3DVSDE_PSIZE]))
                      ||
#ifdef STOMP_TEX_COORDS
                      (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) |
                                                         CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) |
                                                         CELSIUS_FLAG_TEXMATRIXSWFIX(0)         |
                                                         CELSIUS_FLAG_TEXMATRIXSWFIX(1)         |
                                                         CELSIUS_FLAG_STOMP_4TH_COORD(0)        |
                                                         CELSIUS_FLAG_STOMP_4TH_COORD(1)));
#else  // !STOMP_TEX_COORDS
                      (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) |
                                                         CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) |
                                                         CELSIUS_FLAG_TEXMATRIXSWFIX(0)         |
                                                         CELSIUS_FLAG_TEXMATRIXSWFIX(1)));
#endif  // !STOMP_TEX_COORDS

    #ifdef FORCE_INLINE_VERTICES
        bInlineVertices = TRUE;
    #endif

    if (bInlineVertices) {
        dwInlineStride = nvCelsiusInlineVertexStride(pContext) << 2;
        pContext->hwState.dwInlineVertexStride = dwInlineStride;
    }
    else {
        pContext->hwState.dwInlineVertexStride = 0;
    }

    // note: dwFormatShadow is a hideous hack. basically, the hardware fails to write out
    // and restore vertex formats (actually just the sizes contained therein) correctly
    // upon context switches, so we store the information redundantly in some dead space
    // (the last element of the fourth row of the second inverse model view matrix).
    // From there, the RM can grab them and reset things properly after a context switch.

    // x, y, z, [w]
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_POSITION]);
    dwStride = bInlineVertices ? dwInlineStride : ppStreams[dwStreamSelector]->getVertexStride();
    if (pShader->getVASize(defaultInputRegMap[D3DVSDE_POSITION]) == 4*sizeof(DWORD)) {
        // w present
        dwFormat[0] = DRF_DEF(056, _SET_VERTEX_ARRAY_FORMAT, _SIZE, _4) |
                      DRF_DEF(056, _SET_VERTEX_ARRAY_FORMAT, _W, _PRESENT) |
                      DRF_DEF(056, _SET_VERTEX_ARRAY_FORMAT, _TYPE, _FLOAT) |
                      DRF_NUM(056, _SET_VERTEX_ARRAY_FORMAT, _STRIDE, dwStride);
        dwFormatShadow |= (NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_4 << 0);
    }
    else {
        // x,y,z only
        dwFormat[0] = DRF_DEF(056, _SET_VERTEX_ARRAY_FORMAT, _SIZE, _3) |
                      DRF_DEF(056, _SET_VERTEX_ARRAY_FORMAT, _W, _NONE) |
                      DRF_DEF(056, _SET_VERTEX_ARRAY_FORMAT, _TYPE, _FLOAT) |
                      DRF_NUM(056, _SET_VERTEX_ARRAY_FORMAT, _STRIDE, dwStride);
        dwFormatShadow |= (NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_3 << 0);
    }

    // diffuse array
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_DIFFUSE]);
    if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
        dwStride = bInlineVertices ? dwInlineStride : ppStreams[dwStreamSelector]->getVertexStride();
        dwFormat[1]  = DRF_DEF(056, _SET_DIFFUSE_ARRAY_FORMAT, _SIZE, _4) |
                       DRF_NUM(056, _SET_DIFFUSE_ARRAY_FORMAT, _STRIDE, dwStride);
        dwFormat[1] |= bTess?
                       DRF_DEF(056, _SET_DIFFUSE_ARRAY_FORMAT, _TYPE, _FLOAT) :
                       DRF_DEF(056, _SET_DIFFUSE_ARRAY_FORMAT, _TYPE, _UNSIGNED_BYTE_BGRA);
        dwFormatShadow |= (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_4 << 4);
    }
    else {
#ifdef STATE_CULL
        pContext->hwState.celsius.valid [NV056_SET_DIFFUSE_COLOR4UB >> 2] = 0;
#endif
        pContext->hwState.celsius.set (NV056_SET_DIFFUSE_COLOR4UB, 0xFFFFFFFF); // default to white
        dwFormat[1] = DRF_DEF(056, _SET_DIFFUSE_ARRAY_FORMAT, _SIZE, _0) |
                      DRF_DEF(056, _SET_DIFFUSE_ARRAY_FORMAT, _TYPE, _UNSIGNED_BYTE_BGRA);
        dwFormatShadow |= (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4);
    }

    // specular array
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_SPECULAR]);
    if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
        dwStride = bInlineVertices ? dwInlineStride : ppStreams[dwStreamSelector]->getVertexStride();
        dwFormat[2]  = DRF_DEF(056, _SET_SPECULAR_ARRAY_FORMAT, _SIZE, _4) |
                       DRF_NUM(056, _SET_SPECULAR_ARRAY_FORMAT, _STRIDE, dwStride);
        dwFormat[2] |= bTess ?
                       DRF_DEF(056, _SET_SPECULAR_ARRAY_FORMAT, _TYPE, _FLOAT) :
                       DRF_DEF(056, _SET_SPECULAR_ARRAY_FORMAT, _TYPE, _UNSIGNED_BYTE_BGRA);
        dwFormatShadow |= (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_4 << 8);
    }
    else {
#ifdef STATE_CULL
        pContext->hwState.celsius.valid [NV056_SET_SPECULAR_COLOR4UB >> 2] = 0;
#endif
        pContext->hwState.celsius.set (NV056_SET_SPECULAR_COLOR4UB, 0); // default to black
        dwFormat[2] = DRF_DEF(056, _SET_SPECULAR_ARRAY_FORMAT, _SIZE, _0) |
                      DRF_DEF(056, _SET_SPECULAR_ARRAY_FORMAT, _TYPE, _UNSIGNED_BYTE_BGRA);
        dwFormatShadow |= (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 8);
    }

    // tex 0 array
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)) {
        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[0];
        nvAssert (dwD3DStage != CELSIUS_UNUSED);
        dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 0) & 0xffff;
        dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        dwStreamSelector = pShader->getVAStream (dwVAIndex);
        nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])); // they better have given us coordinates
        dwTexCount = (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_TEXMATRIXSWFIX(0))) ?
                     4 : ((pShader->getVASize (dwVAIndex)) >> 2);
        dwStride = bInlineVertices ? dwInlineStride : ppStreams[dwStreamSelector]->getVertexStride();
        dwFormat[3] = DRF_NUM(056, _SET_TEX_COORD0_ARRAY_FORMAT, _SIZE, dwTexCount) |
                      DRF_DEF(056, _SET_TEX_COORD0_ARRAY_FORMAT, _TYPE, _FLOAT) |
                      DRF_NUM(056, _SET_TEX_COORD0_ARRAY_FORMAT, _STRIDE, dwStride);
        dwFormatShadow |= (dwTexCount << 12);
    }
    else {
        dwFormat[3] = DRF_DEF(056, _SET_TEX_COORD0_ARRAY_FORMAT, _SIZE, _0) |
                      DRF_DEF(056, _SET_TEX_COORD0_ARRAY_FORMAT, _TYPE, _FLOAT);
        dwFormatShadow |= (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_0 << 12);
    }

    // tex 1 array
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1)) {
        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[1];
        nvAssert (dwD3DStage != CELSIUS_UNUSED);
        dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 16) & 0xffff;
        dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        dwStreamSelector = pShader->getVAStream (dwVAIndex);
        nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])); // they better have given us coordinates
        dwTexCount = (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) | CELSIUS_FLAG_TEXMATRIXSWFIX(1))) ?
                     4 : ((pShader->getVASize (dwVAIndex)) >> 2);
        dwStride = bInlineVertices ? dwInlineStride : ppStreams[dwStreamSelector]->getVertexStride();
        dwFormat[4] = DRF_NUM(056, _SET_TEX_COORD1_ARRAY_FORMAT, _SIZE, dwTexCount) |
                      DRF_DEF(056, _SET_TEX_COORD1_ARRAY_FORMAT, _TYPE, _FLOAT) |
                      DRF_NUM(056, _SET_TEX_COORD1_ARRAY_FORMAT, _STRIDE, dwStride);
        dwFormatShadow |= (dwTexCount << 16);
    }
    else {
        dwFormat[4] = DRF_DEF(056, _SET_TEX_COORD1_ARRAY_FORMAT, _SIZE, _0) |
                      DRF_DEF(056, _SET_TEX_COORD1_ARRAY_FORMAT, _TYPE, _FLOAT);
        dwFormatShadow |= (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_0 << 16);
    }

    // normal array
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_NORMAL]);
    if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
        dwStride = bInlineVertices ? dwInlineStride : ppStreams[dwStreamSelector]->getVertexStride();
        dwFormat[5] = DRF_DEF(056, _SET_NORMAL_ARRAY_FORMAT, _SIZE, _3)      |
                      DRF_DEF(056, _SET_NORMAL_ARRAY_FORMAT, _TYPE, _FLOAT)  |
                      DRF_NUM(056, _SET_NORMAL_ARRAY_FORMAT, _STRIDE, dwStride);
        dwFormatShadow |= (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_3 << 20);
    }
    else {
        dwFormat[5] = DRF_DEF(056, _SET_NORMAL_ARRAY_FORMAT, _SIZE, _0)      |
                      DRF_DEF(056, _SET_NORMAL_ARRAY_FORMAT, _TYPE, _FLOAT);
        dwFormatShadow |= (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 20);
    }

    // weight array
    dwStreamSelector = pShader->getVAStream(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]);
    if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
        dwStride = bInlineVertices ? dwInlineStride : ppStreams[dwStreamSelector]->getVertexStride();
        dwFormat[6] = DRF_DEF(056, _SET_WEIGHT_ARRAY_FORMAT, _SIZE, _1)      |
                      DRF_DEF(056, _SET_WEIGHT_ARRAY_FORMAT, _TYPE, _FLOAT)  |
                      DRF_NUM(056, _SET_WEIGHT_ARRAY_FORMAT, _STRIDE, dwStride);
        dwFormatShadow |= (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_1 << 24);
    }
    else {
        dwFormat[6] = DRF_DEF(056, _SET_WEIGHT_ARRAY_FORMAT, _SIZE, _0)      |
                      DRF_DEF(056, _SET_WEIGHT_ARRAY_FORMAT, _TYPE, _FLOAT);
        dwFormatShadow |= (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 24);
    }

    // fog arrays (non-existent in D3D)
    dwFormatShadow |= (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 28);

    // now go ahead and actually set all the formats, PREFACED by the shadowed copy
    pContext->hwState.celsius.set (NV056_SET_VERTEX_FORMAT_SHADOW,    dwFormatShadow);
    pContext->hwState.celsius.set (NV056_SET_VERTEX_ARRAY_FORMAT,     dwFormat[0]);
    pContext->hwState.celsius.set (NV056_SET_DIFFUSE_ARRAY_FORMAT,    dwFormat[1]);
    pContext->hwState.celsius.set (NV056_SET_SPECULAR_ARRAY_FORMAT,   dwFormat[2]);
    pContext->hwState.celsius.set (NV056_SET_TEX_COORD0_ARRAY_FORMAT, dwFormat[3]);
    pContext->hwState.celsius.set (NV056_SET_TEX_COORD1_ARRAY_FORMAT, dwFormat[4]);
    pContext->hwState.celsius.set (NV056_SET_NORMAL_ARRAY_FORMAT,     dwFormat[5]);
    pContext->hwState.celsius.set (NV056_SET_WEIGHT_ARRAY_FORMAT,     dwFormat[6]);

    // if we're going to fetch vertex data directly, we also need to program up the context dma and offsets
    if (!bInlineVertices) {

        // program the context dma and calculate a celsius-specific offset adjustment:
        // celsius wants an offset relative to a page aligned context dma, something
        // we do not know we have using the built in getOffset() methods.
        dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_POSITION]);

        switch (ppStreams[dwStreamSelector]->getContextDMA()) {

            case NV_CONTEXT_DMA_AGP_OR_PCI:
                pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_VERTEX, D3D_CONTEXT_DMA_HOST_MEMORY);
                pContext->hwState.celsius.set (NV056_INVALIDATE_VERTEX_CACHE_FILE, 0);
                // compensate for ctx dma that is possibly not 4k aligned
                dwOffsetAdjustment = (ppStreams[dwStreamSelector]->getHeapLocation() == CSimpleSurface::HEAP_AGP) ?
                                     (pDriverData->GARTLinearBase & 0xfff) :      // AGP
                                     (getDC()->nvD3DTexHeapData.dwBase & 0xfff);  // PCI
                break;
            case NV_CONTEXT_DMA_VID:
                pContext->hwState.celsius.set (NV056_SET_CONTEXT_DMA_VERTEX, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
                pContext->hwState.celsius.set (NV056_INVALIDATE_VERTEX_CACHE_FILE, 0);
                // compensate for ctx dma that is possibly not 4k aligned
                // NOTE - this is highly unlikely to not be 4k aligned...
                nvAssert ((pDriverData->BaseAddress & 0xfff) == 0);
                dwOffsetAdjustment = 0;
                break;
            default:
                DPF ("unknown context DMA");
                dbgD3DError();
                break;

        }  // switch

        // x, y, z, [w]
        dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_POSITION]);
        nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector]));
        dwVertexBufferOffset = ppStreams[dwStreamSelector]->getOffset() +
                               pContext->hwState.dwVertexOffset +
                               pShader->getVAOffset (defaultInputRegMap[D3DVSDE_POSITION]) +
                               dwOffsetAdjustment;
        pContext->hwState.celsius.set (NV056_SET_VERTEX_ARRAY_OFFSET, dwVertexBufferOffset);

        // diffuse color
        dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_DIFFUSE]);
        if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
            dwVertexBufferOffset = ppStreams[dwStreamSelector]->getOffset() +
                                   pContext->hwState.dwVertexOffset +
                                   pShader->getVAOffset (defaultInputRegMap[D3DVSDE_DIFFUSE]) +
                                   dwOffsetAdjustment;
            pContext->hwState.celsius.set (NV056_SET_DIFFUSE_ARRAY_OFFSET, dwVertexBufferOffset);
        }

        // specular color
        dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_SPECULAR]);
        if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
            dwVertexBufferOffset = ppStreams[dwStreamSelector]->getOffset() +
                                   pContext->hwState.dwVertexOffset +
                                   pShader->getVAOffset (defaultInputRegMap[D3DVSDE_SPECULAR]) +
                                   dwOffsetAdjustment;
            pContext->hwState.celsius.set (NV056_SET_SPECULAR_ARRAY_OFFSET, dwVertexBufferOffset);
        }

        // texture0 coordinates
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)) {
            dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[0];
            nvAssert (dwD3DStage != CELSIUS_UNUSED);
            dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 0) & 0xffff;
            dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            dwStreamSelector = pShader->getVAStream (dwVAIndex);
            nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])); // they better have given us coordinates
            dwVertexBufferOffset = ppStreams[dwStreamSelector]->getOffset() +
                                   pContext->hwState.dwVertexOffset +
                                   pShader->getVAOffset (dwVAIndex) +
                                   dwOffsetAdjustment;
            pContext->hwState.celsius.set (NV056_SET_TEX_COORD0_ARRAY_OFFSET, dwVertexBufferOffset);
        }

        // texture1 coordinates
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1)) {
            dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[1];
            nvAssert (dwD3DStage != CELSIUS_UNUSED);
            dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 16) & 0xffff;
            dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            dwStreamSelector = pShader->getVAStream (dwVAIndex);
            nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])); // they better have given us coordinates
            dwVertexBufferOffset = ppStreams[dwStreamSelector]->getOffset() +
                                   pContext->hwState.dwVertexOffset +
                                   pShader->getVAOffset (dwVAIndex) +
                                   dwOffsetAdjustment;
            pContext->hwState.celsius.set (NV056_SET_TEX_COORD1_ARRAY_OFFSET, dwVertexBufferOffset);
        }

        // normal
        dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_NORMAL]);
        if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
            dwVertexBufferOffset = ppStreams[dwStreamSelector]->getOffset() +
                                   pContext->hwState.dwVertexOffset +
                                   pShader->getVAOffset (defaultInputRegMap[D3DVSDE_NORMAL]) +
                                   dwOffsetAdjustment;
            pContext->hwState.celsius.set (NV056_SET_NORMAL_ARRAY_OFFSET, dwVertexBufferOffset);
        }

        // weights (for skinning)
        dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_BLENDWEIGHT]);
        if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
            dwVertexBufferOffset = ppStreams[dwStreamSelector]->getOffset() +
                                   pContext->hwState.dwVertexOffset +
                                   pShader->getVAOffset (defaultInputRegMap[D3DVSDE_BLENDWEIGHT]) +
                                   dwOffsetAdjustment;
            pContext->hwState.celsius.set (NV056_SET_WEIGHT_ARRAY_OFFSET, dwVertexBufferOffset);
        }

    }  // if (!bInlineVertices)

    // do not remove
    pContext->hwState.celsius.queueTLNOP();

    return (D3D_OK);
}

//---------------------------------------------------------------------------

// set all state of the celsius object
// should be called prior to rendering
HRESULT __stdcall nvSetCelsiusState (PNVD3DCONTEXT pContext)
{

#ifdef NV_PROFILE_DP2OPS
    pDriverData->pBenchmark->beginHWStateChange(pContext->hwState.dwDirtyFlags, pDriverData->nvPusher.m_dwPut);
#endif

#ifdef NV_NULL_BLEND

    if (((pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_MISC_STATE)) || (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_COMBINERS) || (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_TEXTURE_STATE)) {

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

#if (NVARCH >= 0x010)
    // Tell AASTATE that we're starting 3D rendering
    // Note that we call nvSetCelsiusState whenever
    // we're about to do some 3D rendering
    pContext->aa.Rendering(pContext);
#endif  // NVARCH >= 0x010


    // clear the failure flag
    pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_SETUPFAILURE;

    // if ddraw has sullied the celsius object or if we last programmed
    // the celsius object for a different context, start from scratch
    if ((pDriverData->dwMostRecentHWUser != MODULE_ID_D3D) ||
        (getDC()->dwLastHWContext        != (DWORD)pContext))
    {
        pDriverData->dwMostRecentHWUser = MODULE_ID_D3D;
        getDC()->dwLastHWContext        = (DWORD)pContext;
        pContext->hwState.dwDirtyFlags  = CELSIUS_DIRTY_REALLY_FILTHY;
#ifdef STATE_CULL
        for (int i=0; i < CELSIUS_HWSTATE_SIZE; i++) {
            // this case also implies that the object has state inconsistent with our shadowed copy
            // therefore we need to disable HW shadowing for the rest of call
            pContext->hwState.celsius.valid[i] = 0;
        }
#endif
    }

    // apply out-of-context dirty flags
    pContext->hwState.dwDirtyFlags |= getDC()->dwOutOfContextCelsiusDirtyFlags;
    getDC()->dwOutOfContextCelsiusDirtyFlags = 0;

    // this compiles away to nothing in a retail build
    pContext->hwState.dwDirtyFlags |= dbgForceHWRefresh;

    // mask out any non-celsius bits
    pContext->hwState.dwDirtyFlags &= CELSIUS_DIRTY_REALLY_FILTHY;


    // if nothing is dirty, just return
    if (!(pContext->hwState.dwDirtyFlags)) {
        return D3D_OK;
    }

#ifdef NV_PROFILE_CALLSTACK
    NVP_START(NVP_T_SETCELSIUSSTATE);
#endif

    // fundamental D3D stuff that doesn't normally change
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_DEFAULTS) {
        nvSetCelsiusD3DDefaults (pContext);
    }

    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_SURFACE) {
        nvSetCelsiusSurfaceInfo (pContext);
    }

    // program the color combiners early since they determine the mapping
    // between hardware texture units and D3D texture stages, upon
    // which tons of other stuff depends.
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_COMBINERS) {
        nvSetCelsiusColorCombiners (pContext);
    }

    // If a pixel shader is enabled, use it to update the state
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_PIXEL_SHADER) {
        if (pContext->pCurrentPShader) {
            pContext->pCurrentPShader->setCelsiusState(pContext);
            pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_STATE | CELSIUS_DIRTY_FVF;
        }
        pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_PIXEL_SHADER;
    }

    // texture state
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_TEXTURE_STATE) {
        nvSetCelsiusTextureState (pContext);
    }

    // fog / specular combiner
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_SPECFOG_COMBINER) {
        nvSetCelsiusSpecularFogCombiner (pContext);
    }

    // transform / lighting mode
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_TL_MODE) {
        nvSetCelsiusTLMode (pContext);
    }

    // texture transform
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_TEXTURE_TRANSFORM) {
        nvSetCelsiusTextureTransform (pContext);
    }

    // lighting
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_LIGHTS) {
        nvSetCelsiusLights (pContext);
    }

    // transform
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_TRANSFORM) {
        nvSetCelsiusTransform (pContext);
    }

    // remaining random stuff
    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_MISC_STATE) {
        nvSetCelsiusMiscState (pContext);
    }

    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_CONTROL0) {
        nvSetCelsiusControl0 (pContext);
    }

    if (pContext->hwState.dwDirtyFlags & CELSIUS_DIRTY_FVF) {
        nvSetCelsiusVertexFormat (pContext);
    }


#ifdef NV_PROFILE_CALLSTACK
    NVP_STOP(NVP_T_SETCELSIUSSTATE);
    nvpLogTime(NVP_T_SETCELSIUSSTATE,nvpTime[NVP_T_SETCELSIUSSTATE]);
#endif

    // make sure we're not dirty anymore. (this might happen if
    // there are interdepencies among the celsius state routines
    // and the routines get called in the 'wrong' order)
    nvAssert (!(pContext->hwState.dwDirtyFlags));

#ifdef NV_PROFILE_DP2OPS
    pDriverData->pBenchmark->endHWStateChange(pDriverData->nvPusher.m_dwPut);
#endif

    return D3D_OK;
}

#endif // NV_ARCH == 0x010

