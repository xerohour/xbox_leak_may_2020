/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       lazy.cpp
 *  Content:    Whacks the hardware for state changes that we handle
 *              lazily
 *
 ***************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

const float FLOAT_64 = 64.0f;

//------------------------------------------------------------------------------
// LazySetPointParams
//
// DOC: For point sprites, Xbox takes everything from texture stage 3,
//      not 0

VOID LazySetPointParams(
    CDevice* pDevice)
{
    COUNT_PERF(PERF_SETSTATE_POINTPARAMS);

    PPUSH pPush = pDevice->StartPush();

    FLOAT min = Floatify(D3D__RenderState[D3DRS_POINTSIZE_MIN]);
    FLOAT max = Floatify(D3D__RenderState[D3DRS_POINTSIZE_MAX]);
    FLOAT size = Floatify(D3D__RenderState[D3DRS_POINTSIZE]);

    if (DBG_CHECK(min >= max))
    {
        DPF_ERR("D3DRS_POINTSIZE_MIN must be less than D3DRS_POINTSIZE_MAX");

        max = min + 0.001f; // This simply avoids a divide-by-zero 
    }

    BOOL pointScaleEnable = D3D__RenderState[D3DRS_POINTSCALEENABLE];

    if (!pointScaleEnable)
    {
        // Note that we don't have to adjust the point-size with scale-
        // enable, since that is already relative to the viewport size.

        size *= pDevice->m_SuperSampleScale;

        if (size < min)
            size = min;
        if (size > max) 
            size = max;
        if (size > FLOAT_64)
            size = FLOAT_64;
    }
    else
    {
        // !!! We need something analagous to the Hadden hack-around for
        //     scale-enable with a small size factor, due to the fact that
        //     we have to round a *world-space* value to 6.3
        //
        //     The ideal range for the size is between 32 and 64, and
        //     we can adjust the floating point exponents of 'size' and
        //     'height' directly to do this

        FLOAT height = (FLOAT) pDevice->m_Viewport.Height;
    
        FLOAT a = Floatify(D3D__RenderState[D3DRS_POINTSCALE_A]);
        FLOAT b = Floatify(D3D__RenderState[D3DRS_POINTSCALE_B]);
        FLOAT c = Floatify(D3D__RenderState[D3DRS_POINTSCALE_C]);
    
        // Shaun Ho on what we're doing with this delta/factor stuff:
        // The T&L hw will do (according to the dx spec):
        //
        //      MIN <= PointSize/sqrt(k0 + k1d + k2d*d) <= MAX
        //
        // Eventually this will end up with:
        //
        //      0 <= ((PointSize/(MAX-MIN))/sqrt(k0 + k1d + k2d*d)) 
        //           - MIN/(MAX-MIN) <= 1
        //
        // The PointParameter methods pre-calculate some terms in the 
        // equation and pass them down to the T&L hw.
    
        FLOAT delta = max - min;
        FLOAT factor = delta / (size * height);
        factor *= factor;
    
        // Shaun Ho on why delta is dup'd 3 times:
        //  delta is used to calculate the result. Strictly speaking, we do 
        //  not have to duplicate delta. In general, T&L generates a triple 
        //  (e.g., rgb) for downstream consumption. Since size is only one 
        //  component, we only have to send one delta.  Erik chose to 
        //  duplicate the result of the calculation to each component of a 
        //  triple. The downstream hw can thus pick up the size value from 
        //  any of the three components.
    
        PushCount(pPush, NV097_SET_POINT_PARAMS(0), 8);
    
        FLOAT* pParam = (FLOAT*) pPush + 1;
        *(pParam + 0) = a * factor;
        *(pParam + 1) = b * factor;
        *(pParam + 2) = c * factor;
        *(pParam + 3) = delta;
        *(pParam + 4) = delta;
        *(pParam + 5) = delta;
        *(pParam + 6) = -min / delta;
        *(pParam + 7) = min;

        pPush += 9;
    }

    Push2(pPush, 
          NV097_SET_POINT_PARAMS_ENABLE, 

          // NV097_SET_POINT_PARAMS_ENABLE:
        
          pointScaleEnable,

          // NV097_SET_POINT_SMOOTH_ENABLE:

          D3D__RenderState[D3DRS_POINTSPRITEENABLE]);

    DWORD fixedSize = Round(8.0f * size);

    // Clamp 64.0 and greater to 63.875.  We used to assert on this case,
    // but it seems like a value of 64.0 is reasonable and should be clamped
    // to the hardware's true maximum of 63.875.

    if (fixedSize > 0x1ff)
        fixedSize = 0x1ff;

    Push1(pPush + 3, NV097_SET_POINT_SIZE, fixedSize);
    
    pDevice->EndPush(pPush + 5);
}

//------------------------------------------------------------------------------
// LazySetShaderStageProgram

VOID LazySetShaderStageProgram(
    CDevice* pDevice)
{
    INT stage;
    DWORD shaderStageProgram = 0;

    if (pDevice->m_pPixelShader == NULL)
    {
        for (stage = 3; stage >= 0; stage--)
        {
            DWORD shaderMode 
                = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_2D_PROJECTIVE;

            D3DBaseTexture* pTexture = pDevice->m_Textures[stage];
    
            if (pTexture == NULL)
            {
                shaderMode = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE;
            }
            else if ((stage != 0) &&
                     (D3D__TextureState[stage - 1][D3DTSS_COLOROP]
                      == D3DTOP_BUMPENVMAP))
            {
                shaderMode = NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP;
            }
            else if ((stage != 0) &&
                     (D3D__TextureState[stage - 1][D3DTSS_COLOROP]
                      == D3DTOP_BUMPENVMAPLUMINANCE))
            {
                shaderMode =
                    NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE;
            }
            else if (pTexture->Format &
                 DRF_DEF(097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, _TRUE))
            {
                shaderMode = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CUBE_MAP;
            }
            else if ((pTexture->Format & DRF_NUM(097, _SET_TEXTURE_FORMAT, 
                                                 _DIMENSIONALITY, ~0)) ==
                  DRF_DEF(097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, _THREE))
            {
                shaderMode = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_3D_PROJECTIVE;
            }
    
            shaderStageProgram = (shaderStageProgram << 5) | shaderMode;
        }
    }
    else
    {
        DWORD PSShaderStageProgram = pDevice->m_PSShaderStageProgram;

        if(pDevice->m_ShaderAdjustsTexMode == 0)
        {
            // don't adjust shader stage program

            shaderStageProgram = PSShaderStageProgram;
        }
        else
        {
            // adjust shader stage program according to set textures

            for (stage = 3; stage >= 0; stage--)
            {
                // Assume by default we'll keep the specified mode for this stage:
    
                DWORD shaderMode = (PSShaderStageProgram >> (stage * 5)) & 0x1f;
    
                // PERF: Disable texture unit for _NONE case?
    
                D3DBaseTexture* pTexture = pDevice->m_Textures[stage];
    
                if((pTexture == NULL) &&
                   (shaderMode != PS_TEXTUREMODES_PASSTHRU) &&
                   (shaderMode != PS_TEXTUREMODES_CLIPPLANE) &&
                   (shaderMode != PS_TEXTUREMODES_DOT_ZW) &&
                   (shaderMode != PS_TEXTUREMODES_DOTPRODUCT))
                {
                    // no texture set but texture mode requires one

                    shaderMode = PS_TEXTUREMODES_NONE;
                }
                else if ((shaderMode >= PS_TEXTUREMODES_PROJECT2D) &&
                         (shaderMode <= PS_TEXTUREMODES_CUBEMAP))
                {
                    DWORD colorFormat = pTexture->Format 
                        & DRF_NUM(097, _SET_TEXTURE_FORMAT, _COLOR, ~0);
    
                    if (pTexture->Format & 
                        DRF_DEF(097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, _TRUE))
                    {
                        shaderMode = PS_TEXTUREMODES_CUBEMAP;
                    }
                    else if (((pTexture->Format & DRF_NUM(097, _SET_TEXTURE_FORMAT, 
                                                          _DIMENSIONALITY, ~0)) 
                                == DRF_DEF(097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, 
                                           _THREE)) ||
    
                             ((colorFormat >= DRF_DEF(097, _SET_TEXTURE_FORMAT, 
                                                      _COLOR, _SZ_DEPTH_X8_Y24_FIXED)) &&
                              (colorFormat <= DRF_DEF(097, _SET_TEXTURE_FORMAT, 
                                                      _COLOR, _LU_IMAGE_DEPTH_Y16_FLOAT))))
                    {
                        shaderMode = PS_TEXTUREMODES_PROJECT3D;
                    }
                    else
                    {
                        shaderMode = PS_TEXTUREMODES_PROJECT2D;
                    }
                }
                else if ((shaderMode == PS_TEXTUREMODES_DOT_STR_3D) ||
                         (shaderMode == PS_TEXTUREMODES_DOT_STR_CUBE))
                {
                    if (pTexture->Format & DRF_DEF(097, _SET_TEXTURE_FORMAT, 
                                                  _CUBEMAP_ENABLE, _TRUE))
                    {
                        shaderMode = PS_TEXTUREMODES_DOT_STR_CUBE;
                    }
                    else
                    {
                        shaderMode = PS_TEXTUREMODES_DOT_STR_3D;
                    }
                }
        
                shaderStageProgram = (shaderStageProgram << 5) | shaderMode;
            }
        }
    }

#if DBG
    ValidTextureModes(shaderStageProgram, "LazySetShaderStageProgram");
#endif

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_SHADER_STAGE_PROGRAM, shaderStageProgram);

    pDevice->EndPush(pPush + 2);
}

//------------------------------------------------------------------------------
// LazySetTextureState - corresponds to nvSetKelvinTextureState

VOID LazySetTextureState(
    CDevice* pDevice)
{
    COUNT_PERF(PERF_SETSTATE_TEXTURESTATE);

    DWORD dirtyFlags = D3D__DirtyFlags;

    for (DWORD stage = 0; stage < D3DTSS_MAXSTAGES; stage++, dirtyFlags >>= 1)
    {
        if (dirtyFlags & D3DDIRTYFLAG_TEXTURE_STATE_0)
        {
            DWORD* pTextureStates = D3D__TextureState[stage];
    
            // NOTE: We CAN'T check for a NULL texture in this stage, and only
            //       do the following when it's non-NULL.  That's because we
            //       NEVER set D3DDIRTYFLAG_TEXTURE_STATE in SetTexture!
    
            // EXT: Expose Format.BorderSource functionality for having border
            //      come from the texture or the border color.  Can't simply
            //      be a D3DTADDRESS type because it can't be applied separately
            //      to U, V, and P.
    
            DWORD texCoordIndex
                = pTextureStates[D3DTSS_TEXCOORDINDEX] & 0xffff;
    
            DWORD address 
                = DRF_NUMFAST(097, _SET_TEXTURE_ADDRESS, _U, 
                              pTextureStates[D3DTSS_ADDRESSU])
                | DRF_NUMFAST(097, _SET_TEXTURE_ADDRESS, _V,
                              pTextureStates[D3DTSS_ADDRESSV])
                | DRF_NUMFAST(097, _SET_TEXTURE_ADDRESS, _P,
                              pTextureStates[D3DTSS_ADDRESSW])
                | D3D__RenderState[D3DRS_WRAP0 + texCoordIndex];
    
            // These are backward from OpenGL.  D3D can only set the maximum
            // mipmap level, by which they mean the OpenGL minimum level
            // (D3D maximum is the largest sized level to use, not the largest
            // level number).
            //
            // The D3D minimum level (the OpenGL/hardware maximum) is always
            // just the end of the chain.
            //
            // EXT: Expose MAX_LOD_CLAMP?
            //
            // The Nvidia driver sets MAX_LOD_CLAMP to getMipMapCount() - 1,
            // which I'd rather avoid because it would mean updating this
            // register at SetTexture time.  I'm assuming the hardware actually
            // takes the min of Format.MipmapLevels and Control0.MaxLodClamp.
    
            DWORD control0
                = DRF_NUMFAST(097, _SET_TEXTURE_CONTROL0, _MIN_LOD_CLAMP,
                              pTextureStates[D3DTSS_MAXMIPLEVEL] << 8)
    
                | DRF_NUM(097, _SET_TEXTURE_CONTROL0, _MAX_LOD_CLAMP, ~0)
    
                | pTextureStates[D3DTSS_ALPHAKILL]
    
                | DRF_NUMFAST(097, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, 
                              pTextureStates[D3DTSS_COLORKEYOP]);
    
            if (pDevice->m_Textures[stage] != NULL)
            {
                control0 |= DRF_DEF(097, _SET_TEXTURE_CONTROL0, _ENABLE, _TRUE);
            }
    
            DWORD minFilter = pTextureStates[D3DTSS_MINFILTER];
            DWORD magFilter = pTextureStates[D3DTSS_MAGFILTER];
    
            DWORD convolutionKernel 
                = DRF_DEF(097, _SET_TEXTURE_FILTER, _CONVOLUTION_KERNEL, _QUINCUNX);
    
            if ((minFilter >= D3DTEXF_ANISOTROPIC) || 
                (magFilter >= D3DTEXF_ANISOTROPIC))
            {
                if ((minFilter > D3DTEXF_ANISOTROPIC) ||
                    (magFilter > D3DTEXF_ANISOTROPIC))
                {
                    // CONVOLUTION_2D_LOD0 has to be set in both the Min and Mag
                    // fields in order for a convolution filter to be used.  So
                    // convolution always wins if it's mixed with something else.
                    //
                    // Between Quincunx and Gaussian, Gaussian always wins.
    
                    if ((minFilter == D3DTEXF_GAUSSIANCUBIC) ||
                        (magFilter == D3DTEXF_GAUSSIANCUBIC))
                    {
                        convolutionKernel = DRF_DEF(097, _SET_TEXTURE_FILTER, 
                                                    _CONVOLUTION_KERNEL, _GAUSSIAN_3);
                    }
    
                    // CONVOLUTION_2D_LOD0 has a value of '7', so it doesn't 
                    // matter what the MinFilter() ORs into the 'Min' field a
                    // few lines down, since it will OR in values less than 7...
    
                    convolutionKernel |= DRF_DEF(097, _SET_TEXTURE_FILTER, 
                                                 _MIN, _CONVOLUTION_2D_LOD0);
    
                    minFilter = D3DTEXF_LINEAR;
    
                    magFilter = NV097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0;
                }
                else
                {
                    // Handle anisotropic filters.  If either min or mag filter 
                    // are anisotropic, we use anisotropic:
    
                    DWORD maxAnisotropy = pTextureStates[D3DTSS_MAXANISOTROPY];
                    if (maxAnisotropy == 0)
                    {
                        minFilter = D3DTEXF_POINT;
                        magFilter = D3DTEXF_POINT;
                    }
                    else
                    {
                        // DOC: We max out at a D3DTSS_MAXANISOTROPY of 4:
    
                        ASSERT(maxAnisotropy <= 4);
        
                        maxAnisotropy--;
                        minFilter = D3DTEXF_LINEAR;
                        magFilter = D3DTEXF_LINEAR;
                    }
        
                    control0 |= DRF_NUMFAST(097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO,
                                            maxAnisotropy);
                }
            }
    
            ASSERT((D3DTEXF_POINT == NV097_SET_TEXTURE_FILTER_MAG_BOX_LOD0) &&
                   (D3DTEXF_LINEAR == NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0));
    
            pDevice->m_TextureControl0Enabled[stage] = control0 
                | DRF_DEF(097, _SET_TEXTURE_CONTROL0, _ENABLE, _TRUE);
    
            INT lodBias = Round(256.0f * (Floatify(pTextureStates[D3DTSS_MIPMAPLODBIAS])
                                          + pDevice->m_SuperSampleLODBias));
    
            if (lodBias < -0x1000)
                lodBias = -0x1000;
            if (lodBias > 0xfff)
                lodBias = 0xfff;
    
            // We're about to assume that the bump-env color-ops come at the end
            // of the list so that we can use a single inequality comparison:
    
            ASSERT(((D3DTOP_BUMPENVMAP + 1) == D3DTOP_BUMPENVMAPLUMINANCE) &&
                   ((D3DTOP_BUMPENVMAP + 2) == D3DTOP_MAX));
    
            // When doing bump mapping, we always override the color-sign texture
            // stage state:
    
            DWORD colorSign = (pTextureStates[D3DTSS_COLOROP] >= D3DTOP_BUMPENVMAP)
                ? (D3DTSIGN_GSIGNED | D3DTSIGN_BSIGNED)
                : pTextureStates[D3DTSS_COLORSIGN];
    
            DWORD filter 
                = DRF_NUM(097, _SET_TEXTURE_FILTER, _MIPMAP_LOD_BIAS, lodBias)
    
                | convolutionKernel
    
                | MinFilter(minFilter, pTextureStates[D3DTSS_MIPFILTER])
    
                | DRF_NUMFAST(097, _SET_TEXTURE_FILTER, _MAG, magFilter)
    
                | colorSign;
    
            PPUSH pPush = pDevice->StartPush();
    
            // Hit NV097_SET_TEXTURE_ADDRESS and NV097_SET_TEXTURE_CONTROL0:
    
            Push2(pPush, NV097_SET_TEXTURE_ADDRESS(stage), address, control0);
            Push1(pPush + 3, NV097_SET_TEXTURE_FILTER(stage), filter);
    
            pDevice->EndPush(pPush + 5);
        }
    }

    // !!! Gotta set TEXGEN_VIEW_MODEL somewhere!
}

//------------------------------------------------------------------------------
// LazySetSpecFogCombiner - corresponds to nvSetKelvinSpecularFogCombiner

VOID LazySetSpecFogCombiner(
    CDevice* pDevice)
{
    PPUSH pPush;
    FLOAT bias;
    FLOAT scale;
    DWORD fogMode;
    DWORD control0;
    DWORD control1;

    COUNT_PERF(PERF_SETSTATE_SPECFOGCOMBINER);

    // The current passthrough program is dependent on the fog type
    // (usually a passthrough program is not selected so this will be
    // a quick early-out):

    CommonSetPassthroughProgram(pDevice);

    // The final alpha is just taken from G:

    control1 =
           DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW1, _G_SOURCE, _REG_C) 
         | DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW1, _G_ALPHA, _TRUE)
         | DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW1, _SPECULAR_CLAMP, _TRUE);

    if (D3D__RenderState[D3DRS_FOGENABLE])
    {
        // Note that unlike with the Nvidia driver, we do not do the
        // hack of supporting D3DRS_FOGVERTEXMODE, since the chip doesn't
        // actually do vertex fog.

        DWORD fogTableMode = D3D__RenderState[D3DRS_FOGTABLEMODE];

        FLOAT fogTableStart = Floatify(D3D__RenderState[D3DRS_FOGSTART]);

        FLOAT fogTableEnd = Floatify(D3D__RenderState[D3DRS_FOGEND]);

        FLOAT fogTableDensity = Floatify(D3D__RenderState[D3DRS_FOGDENSITY]);

        // EXT: Expose additional fog gen modes?

        DWORD fogGenMode = D3D__RenderState[D3DRS_RANGEFOGENABLE]
                         ? NV097_SET_FOG_GEN_MODE_V_RADIAL
                         : NV097_SET_FOG_GEN_MODE_V_PLANAR;

        if (fogTableMode == D3DFOG_NONE)
        {
            // User-supplied fog (neither table nor vertex)

            bias = 1.0f;
            scale = 1.0f;
            fogMode = NV097_SET_FOG_MODE_V_LINEAR;
            fogGenMode = NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA;
        }
        else if (fogTableMode == D3DFOG_LINEAR)
        {
            FLOAT fogTableLinearScale;

            if (fogTableEnd != fogTableStart)
            {
                fogTableLinearScale = 1.0f / (fogTableEnd - fogTableStart);
            }
            else
            {
                fogTableLinearScale = MAX_FOG_SCALE;
            }

            bias = 1.0f + fogTableEnd * fogTableLinearScale;
            scale = -fogTableLinearScale;
            fogMode = NV097_SET_FOG_MODE_V_LINEAR;
        }
        else if (fogTableMode == D3DFOG_EXP)
        {
            bias = 1.5f;
            scale = -fogTableDensity * (1.0f / (2.0f * 5.5452f));
            fogMode = NV097_SET_FOG_MODE_V_EXP;
        }
        else
        {
            ASSERT(fogTableMode == D3DFOG_EXP2);

            bias = 1.5f;

            // This is -fogTableDensity * (1 / (2 * sqrt(5.5452))):

            scale = -fogTableDensity * (1.0f / (2.0f * 2.354824834249885f));
            fogMode = NV097_SET_FOG_MODE_V_EXP2;
        }

        pPush = pDevice->StartPush();

        // Hit NV097_SET_FOG_GEN_MODE and NV097_SET_FOG_ENABLE:

        Push2(pPush, NV097_SET_FOG_GEN_MODE, fogGenMode, TRUE); // 0x2a0

        Push1(pPush + 3, NV097_SET_FOG_MODE, fogMode);          // 0x29c

        Push3f(pPush + 5, NV097_SET_FOG_PARAMS(0), bias, scale, 0.0f); 
                                                                // 0x9c0

        // Only update final combiner if there is no active pixel shader

        if ((pDevice->m_pPixelShader == NULL) ||
            ((pDevice->m_pPixelShader != NULL) && (pDevice->m_ShaderUsesSpecFog == 0)))
        {
            // A is fog factor, obtained from fog alpha, C is fog color,
            // D isn't used

            control0 
             = DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW0, _A_SOURCE, _REG_3)
             | DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW0, _A_ALPHA, _TRUE)
             | DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW0, _C_SOURCE, _REG_3);
    
            // B source is (textured diffuse) or (textured diffuse + specular)

            if (D3D__RenderState[D3DRS_SPECULARENABLE])
            {
                control0 |= DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW0, 
                                    _B_SOURCE, _REG_SPECLIT);
            }
            else 
            {  
                // Don't add specular to diffuse (textured) color:

                control0 |= DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW0, 
                                    _B_SOURCE, _REG_C);
            }
    
            // Hit NV097_SET_COMBINER_SPECULAR_FOG_CW0 and
            // NV097_SET_COMBINER_SPECULAR_FOG_CW1:

            Push2(pPush + 9, NV097_SET_COMBINER_SPECULAR_FOG_CW0,   // 0x288
                  control0, control1);
    
            pDevice->EndPush(pPush + 12);
        }
        else
        {
            // pixel shader that uses final combiner is active, finish
            // pushbuffer sequence without control0 and control1

            pDevice->EndPush(pPush + 9);
        }
    }
    else
    {
        // No fog, just output diffuse (+ specular)

        pPush = pDevice->StartPush();

        Push1(pPush, NV097_SET_FOG_ENABLE, FALSE);              // 0x2a4

        // Only update final combiner if there is no active pixel shader

        if ((pDevice->m_pPixelShader == NULL) ||
            ((pDevice->m_pPixelShader != NULL) && (pDevice->m_ShaderUsesSpecFog == 0)))
        {
            if (D3D__RenderState[D3DRS_SPECULARENABLE])
            {
                control0 = DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW0, 
                                   _D_SOURCE, _REG_SPECLIT);
            }
            else 
            {  
                // Don't add specular to diffuse (textured) color

                control0 = DRF_DEF(097, _SET_COMBINER_SPECULAR_FOG_CW0, 
                                   _D_SOURCE, _REG_C);
            }
    
            // Hit NV097_SET_COMBINER_SPECULAR_FOG_CW0 and
            // NV097_SET_COMBINER_SPECULAR_FOG_CW1:

            Push2(pPush + 2, NV097_SET_COMBINER_SPECULAR_FOG_CW0,   // 0x288
                  control0, control1);
    
            pDevice->EndPush(pPush + 5);
        }
        else
        {
            // pixel shader that uses final combiner is active, finish
            // pushbuffer sequence without control0 and control1

            pDevice->EndPush(pPush + 2);
        }
    }
}

//------------------------------------------------------------------------------
// LazySetTextureTransform helper functions

FORCEINLINE VOID Set4f(
    FLOAT* p,
    FLOAT a, 
    FLOAT b,
    FLOAT c, 
    FLOAT d)
{
    p[0] = a;
    p[1] = b;
    p[2] = c;
    p[3] = d;
}

// various routines to set texture transform matrices. 
// (s,t,1.0) in, (s,t,1.0) out

FORCEINLINE VOID SetTextureTransformMatrixType220(
    D3DMATRIX *pDst, 
    CONST D3DMATRIX* pMatrix)
{
    Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
    Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
    Set4f(&pDst->m[2][0], 0.0f,         0.0f,         0.0f,         0.0f        );
    Set4f(&pDst->m[3][0], 0.0f,         0.0f,         0.0f,         1.0f        );
}

// (s,t,1.0) in, (s,t,r,1.0) out

FORCEINLINE VOID SetTextureTransformMatrixType230(
    D3DMATRIX *pDst, 
    CONST D3DMATRIX* pMatrix)
{
    Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
    Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
    Set4f(&pDst->m[2][0], pMatrix->_13, pMatrix->_23, 0.0f,         pMatrix->_33);
    Set4f(&pDst->m[3][0], 0.0f,         0.0f,         0.0f,         1.0f        );
}

// (s,t,1.0) in, (s,t,q) out

FORCEINLINE VOID SetTextureTransformMatrixType231(
    D3DMATRIX *pDst, 
    CONST D3DMATRIX* pMatrix)
{
    Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
    Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
    Set4f(&pDst->m[2][0], 0.0f,         0.0f,         0.0f,         0.0f        );
    Set4f(&pDst->m[3][0], pMatrix->_13, pMatrix->_23, 0.0f,         pMatrix->_33);
}

// (s,t,1.0) in, (s,t,r,q) out

FORCEINLINE VOID SetTextureTransformMatrixType241(
    D3DMATRIX *pDst, 
    CONST D3DMATRIX* pMatrix)
{
    Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
    Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
    Set4f(&pDst->m[2][0], pMatrix->_13, pMatrix->_23, 0.0f,         pMatrix->_33);
    Set4f(&pDst->m[3][0], pMatrix->_14, pMatrix->_24, 0.0f,         pMatrix->_34);
}

// (s,t,r,1.0) in, (s,t,1.0) out

FORCEINLINE VOID SetTextureTransformMatrixType320(
    D3DMATRIX *pDst, 
    CONST D3DMATRIX* pMatrix)
{
    Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
    Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
    Set4f(&pDst->m[2][0], 0.0f,         0.0f,         0.0f,         0.0f        );
    Set4f(&pDst->m[3][0], 0.0f,         0.0f,         0.0f,         1.0f        );
}

// (s,t,r,1.0) in, (s,t,r,1.0) out

FORCEINLINE VOID SetTextureTransformMatrixType330(
    D3DMATRIX *pDst, 
    CONST D3DMATRIX* pMatrix)
{
    Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
    Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
    Set4f(&pDst->m[2][0], pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
    Set4f(&pDst->m[3][0], 0.0f,         0.0f,         0.0f,         1.0f        );
}

// (s,t,r,1.0) in, (s,t,q) out

FORCEINLINE VOID SetTextureTransformMatrixType331(
    D3DMATRIX *pDst, 
    CONST D3DMATRIX* pMatrix)
{
    Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
    Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
    Set4f(&pDst->m[2][0], 0.0f,         0.0f,         0.0f,         0.0f        );
    Set4f(&pDst->m[3][0], pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
}

// (s,t,r,1.0) in, (s,t,r,q) out

FORCEINLINE VOID SetTextureTransformMatrixType341(
    D3DMATRIX *pDst, 
    CONST D3DMATRIX* pMatrix)
{
    Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
    Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
    Set4f(&pDst->m[2][0], pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
    Set4f(&pDst->m[3][0], pMatrix->_14, pMatrix->_24, pMatrix->_34, pMatrix->_44);
}

//------------------------------------------------------------------------------
// LazySetTextureTransform - corresponds to nvSetKelvinTextureTransform

VOID LazySetTextureTransform(
    CDevice* pDevice)
{
    PPUSH pPush;
    DWORD stage;
    D3DMATRIX matrix;
    DWORD inCount;
    DWORD width;
    DWORD height;
    DWORD depth;
    DWORD pitch;

    COUNT_PERF(PERF_SETSTATE_TEXTRANSFORM);

    // Make sure we don't whack the constant registers if we're running
    // a vertex program or if we're in 192 constants mode

    if ((!(pDevice->m_pVertexShader->Flags & (VERTEXSHADER_PROGRAM |
                                              VERTEXSHADER_PASSTHROUGH))) &&
        ((pDevice->m_ConstantMode & ~D3DSCM_NORESERVEDCONSTANTS) != 
                                                     D3DSCM_192CONSTANTS))
    {
        for (stage = 0; stage < D3DTSS_MAXSTAGES; stage++)
        {
            DWORD transformFlags = D3D__TextureState[stage]
                                                [D3DTSS_TEXTURETRANSFORMFLAGS];
    
            pPush = pDevice->StartPush();

            // Note from Shaun about whether the driver should disable the 
            // texture transformation if a texture unit is disabled:
            //
            //     Erik did some optimization there. If a texture is disabled, 
            //     the matrix transformation for that texcoord will be skipped.
    
            if (transformFlags == D3DTTFF_DISABLE)
            {
                // Disable the transform:
    
                Push1(pPush, NV097_SET_TEXTURE_MATRIX_ENABLE(stage), FALSE);
                pDevice->EndPush(pPush + 2);
            }
            else
            {
                // Enable the transform:

                DWORD texCoord = D3D__TextureState[stage][D3DTSS_TEXCOORDINDEX];
                DWORD texgen = texCoord & 0xffff0000;
    
                if (texgen)
                {
                    inCount = 3;
                }
                else
                {
                    DWORD index = texCoord & 0xffff;
    
                    // If the index is invalid, just use (0, 0):

                    inCount = (pDevice->m_pVertexShader->Dimensionality 
                                >> (8 * index)) & 0xff;

                    if (inCount == 0)
                        inCount = 2;
                }
    
                DWORD outCount = transformFlags & 0xff;
    
                DWORD matrixType = (inCount << 8) | 
                                   (outCount << 4) | 
                                   ((transformFlags & D3DTTFF_PROJECTED) != 0);

                D3DMATRIX* pMatrix 
                    = &pDevice->m_Transform[D3DTS_TEXTURE0 + stage];

                Push1(pPush, NV097_SET_TEXTURE_MATRIX_ENABLE(stage), TRUE);

                PushCount(pPush + 2, 
                          NV097_SET_TEXTURE_MATRIX0(0) + stage*0x40,
                          16);

                D3DMATRIX* pDst = (D3DMATRIX*) (pPush + 3);

                pPush += 19;

                // The way we emit the matrix varies widely because of D3D.
                //
                // A. When there are two incoming coordinates, we must move 
                //    the 3rd matrix row to the fourth, because D3D expects 
                //    the appended q to sit in the third slot, whereas in 
                //    the hardware, it sits in the 4th.
                // B. When the matrix is projective and there are only 3 
                //    outgoing coordinates (including q) we must shift the 
                //    value in the third outgoing slot (where D3D puts q) to 
                //    the 4th slot (where the hardware wants it) by moving the 
                //    3rd column to the 4th
                // C. When the matrix is non-projective, it becomes our job to 
                //    override the relevant row of the matrix, forcing the 
                //    outgoing q to 1.0
                //
                // Finally, when all is said and done, the matrix must be 
                // transposed to change it from D3D style to hardware style.
    
                switch (matrixType)
                {
                case 0x220:
                    // (s,t,1.0) in, (s,t,1.0) out
                    SetTextureTransformMatrixType220(pDst, pMatrix);
                    break;
                case 0x230:
                    // (s,t,1.0) in, (s,t,r,1.0) out
                    SetTextureTransformMatrixType230(pDst, pMatrix);
                    break;
                case 0x231:
                    // (s,t,1.0) in, (s,t,q) out
                    SetTextureTransformMatrixType231(pDst, pMatrix);
                    break;
                case 0x241:
                    // (s,t,1.0) in, (s,t,r,q) out
                    SetTextureTransformMatrixType241(pDst, pMatrix);
                    break;
                case 0x320:
                    // (s,t,r,1.0) in, (s,t,1.0) out
                    SetTextureTransformMatrixType320(pDst, pMatrix);
                    break;
                case 0x330:
                    // (s,t,r,1.0) in, (s,t,r,1.0) out
                    SetTextureTransformMatrixType330(pDst, pMatrix);
                    break;
                case 0x331:
                    // (s,t,r,1.0) in, (s,t,q) out
                    SetTextureTransformMatrixType331(pDst, pMatrix);
                    break;
                case 0x341:
                    // (s,t,r,1.0) in, (s,t,r,q) out
                    SetTextureTransformMatrixType341(pDst, pMatrix);
                    break;
                default:
                    NODEFAULT("Unhandled texture transform type\n");
                }

                pDevice->EndPush(pPush);
            }
        }
    }
}

//------------------------------------------------------------------------------
// g_EyeDirection
//
// Used by LazySetLights

D3DCONST D3DVECTOR g_EyeDirection = { 0.0f, 0.0f, -1.0f };

//------------------------------------------------------------------------------
// GetColorMaterial
//
// Constructs the NV097_SET_COLOR_MATERIAL register.  Analagous to 
// getKelvinColorMaterialFlags.  

DWORD GetColorMaterial()
{
    DWORD colorMaterial = 0;

    if (D3D__RenderState[D3DRS_COLORVERTEX])
    {
        ASSERT((D3DRS_BACKSPECULARMATERIALSOURCE + 1 
                                        == D3DRS_BACKDIFFUSEMATERIALSOURCE) &&
               (D3DRS_BACKSPECULARMATERIALSOURCE + 2 
                                        == D3DRS_BACKAMBIENTMATERIALSOURCE) &&
               (D3DRS_BACKSPECULARMATERIALSOURCE + 3 
                                        == D3DRS_BACKEMISSIVEMATERIALSOURCE) &&
               (D3DRS_BACKSPECULARMATERIALSOURCE + 4 
                                        == D3DRS_SPECULARMATERIALSOURCE) &&
               (D3DRS_BACKSPECULARMATERIALSOURCE + 5 
                                        == D3DRS_DIFFUSEMATERIALSOURCE) &&
               (D3DRS_BACKSPECULARMATERIALSOURCE + 6 
                                        == D3DRS_AMBIENTMATERIALSOURCE) &&
               (D3DRS_BACKSPECULARMATERIALSOURCE + 7 
                                        == D3DRS_EMISSIVEMATERIALSOURCE));

        for (DWORD i = 0; i < 8; i++)
        {
            colorMaterial <<= 2;
            colorMaterial 
                |= D3D__RenderState[D3DRS_BACKSPECULARMATERIALSOURCE + i];
        }

        CDevice* pDevice = g_pDevice;

        // We can't use specular or diffuse if we don't have them:
    
        if (!(pDevice->m_pVertexShader->Flags & VERTEXSHADER_HASDIFFUSE))
            colorMaterial &= ~0x0055;

        if (!(pDevice->m_pVertexShader->Flags & VERTEXSHADER_HASSPECULAR))
            colorMaterial &= ~0x00aa;

        if (!(pDevice->m_pVertexShader->Flags & VERTEXSHADER_HASBACKDIFFUSE))
            colorMaterial &= ~0x5500;

        if (!(pDevice->m_pVertexShader->Flags & VERTEXSHADER_HASBACKSPECULAR))
            colorMaterial &= ~0xaa00;
    }

    return colorMaterial;
}

//------------------------------------------------------------------------------
// SetLightColors

PPUSH SetLightColors(
    PPUSH pPush,
    CONST Light* pLight,
    DWORD lightNum,
    DWORD colorMaterial)
{
    ASSERT(D3D__RenderState[D3DRS_TWOSIDEDLIGHTING] <= 1);

    CDevice* pDevice = g_pDevice;

    DWORD i = D3D__RenderState[D3DRS_TWOSIDEDLIGHTING];
    D3DMATERIAL8* pMaterial = &pDevice->m_Material;

    PushCount(pPush, NV097_SET_LIGHT_AMBIENT_COLOR(lightNum, 0), 9);

    while (TRUE)
    {
        FLOAT* f = (FLOAT*) pPush;

        if (!(colorMaterial & DRF_NUM(097, _SET_COLOR_MATERIAL, 
                                      _AMBIENT_MATERIAL, ~0)))
        {
            *(f + 1) = pMaterial->Ambient.r * pLight->Light8.Ambient.r;
            *(f + 2) = pMaterial->Ambient.g * pLight->Light8.Ambient.g;
            *(f + 3) = pMaterial->Ambient.b * pLight->Light8.Ambient.b;
        }
        else
        {
            *(f + 1) = pLight->Light8.Ambient.r;
            *(f + 2) = pLight->Light8.Ambient.g;
            *(f + 3) = pLight->Light8.Ambient.b;
        }
    
        if (!(colorMaterial & DRF_NUM(097, _SET_COLOR_MATERIAL, 
                                      _DIFF_MATERIAL, ~0)))
        {
            *(f + 4) = pMaterial->Diffuse.r * pLight->Light8.Diffuse.r;
            *(f + 5) = pMaterial->Diffuse.g * pLight->Light8.Diffuse.g;
            *(f + 6) = pMaterial->Diffuse.b * pLight->Light8.Diffuse.b;
        }
        else
        {
            *(f + 4) = pLight->Light8.Diffuse.r;
            *(f + 5) = pLight->Light8.Diffuse.g;
            *(f + 6) = pLight->Light8.Diffuse.b;
        }
    
        if (!(colorMaterial & DRF_NUM(097, _SET_COLOR_MATERIAL, 
                                      _SPECULAR_MATERIAL, ~0)))
        {
            *(f + 7) = pMaterial->Specular.r * pLight->Light8.Specular.r;
            *(f + 8) = pMaterial->Specular.g * pLight->Light8.Specular.g;
            *(f + 9) = pMaterial->Specular.b * pLight->Light8.Specular.b;
        }
        else
        {
            *(f + 7) = pLight->Light8.Specular.r;
            *(f + 8) = pLight->Light8.Specular.g;
            *(f + 9) = pLight->Light8.Specular.b;
        }

        pPush += 10;

        if (i-- == 0)
            return pPush;

        PushCount(pPush, NV097_SET_BACK_LIGHT_AMBIENT_COLOR(lightNum, 0), 9);

        colorMaterial >>= 8;
        pMaterial = &pDevice->m_BackMaterial;
    }
}

//------------------------------------------------------------------------------
// SetSceneAmbientAndMaterialEmission

PPUSH SetSceneAmbientAndMaterialEmission(
    PPUSH pPush,
    DWORD colorMaterial)
{
    FLOAT emissiveR;
    FLOAT emissiveG;
    FLOAT emissiveB;

    CDevice* pDevice = g_pDevice;

    DWORD i = D3D__RenderState[D3DRS_TWOSIDEDLIGHTING];
    DWORD ambient = D3D__RenderState[D3DRS_AMBIENT];
    D3DMATERIAL8* pMaterial = &pDevice->m_Material;

    PushCount(pPush, NV097_SET_SCENE_AMBIENT_COLOR(0), 3);
    PushCount(pPush + 4, NV097_SET_MATERIAL_EMISSION(0), 3);
    PushCount(pPush + 8, NV097_SET_MATERIAL_ALPHA, 1);

    while (TRUE)
    {
        FLOAT ambientR = D3DVAL(RGBA_GETRED(ambient)) / 255.0f;
        FLOAT ambientG = D3DVAL(RGBA_GETGREEN(ambient)) / 255.0f;
        FLOAT ambientB = D3DVAL(RGBA_GETBLUE(ambient)) / 255.0f;
    
        // In kelvin, the color is initialized to SCENE_AMBIENT, C0, or C1. 
        // This is followed by an optional accumulation of C0 or C1 times 
        // MATERIAL_EMISSION.  Both the initialization and accumulation are 
        // controlled by the color material bits, so these determine the 
        // (sometimes counter-intuitive) values to which we initialize 
        // SCENE_AMBIENT and MATERIAL_EMISSION.
    
        if (colorMaterial & 
                    DRF_NUM(097, _SET_COLOR_MATERIAL, _AMBIENT_MATERIAL, ~0))
        {
            // Ambient is being pulled from the vertex:
    
            emissiveR = ambientR;
            emissiveG = ambientG;
            emissiveB = ambientB;
    
            ambientR = pMaterial->Emissive.r;
            ambientG = pMaterial->Emissive.g;
            ambientB = pMaterial->Emissive.b;
        }
        else if (colorMaterial & 
                    DRF_NUM(097, _SET_COLOR_MATERIAL, _EMISSIVE_MATERIAL, ~0))
        {
            // Emissive is being pulled from the vertex, and ambient is not:
    
            ambientR = ambientR * pMaterial->Ambient.r;
            ambientG = ambientG * pMaterial->Ambient.g;
            ambientB = ambientB * pMaterial->Ambient.b;
    
            emissiveR = 1.0f;
            emissiveG = 1.0f;
            emissiveB = 1.0f;
        }
        else 
        {
            // Neither ambient nor emissive is being pulled from the vertex:
    
            ambientR = (ambientR * pMaterial->Ambient.r) + pMaterial->Emissive.r;
            ambientG = (ambientG * pMaterial->Ambient.g) + pMaterial->Emissive.g;
            ambientB = (ambientB * pMaterial->Ambient.b) + pMaterial->Emissive.b;
    
            emissiveR = 0.0f;
            emissiveG = 0.0f;
            emissiveB = 0.0f;
        }

        *((FLOAT*) pPush + 1) = ambientR;
        *((FLOAT*) pPush + 2) = ambientG;
        *((FLOAT*) pPush + 3) = ambientB;

        *((FLOAT*) pPush + 5) = emissiveR;
        *((FLOAT*) pPush + 6) = emissiveG;
        *((FLOAT*) pPush + 7) = emissiveB;

        *((FLOAT*) pPush + 9) = pMaterial->Diffuse.a;

        pPush += 10;
        if (i-- == 0)
            return pPush;

        // Set up for the second pass, this time handling the back components:

        colorMaterial >>= 8;

        ambient = D3D__RenderState[D3DRS_BACKAMBIENT];
        pMaterial = &pDevice->m_BackMaterial;

        PushCount(pPush, NV097_SET_BACK_SCENE_AMBIENT_COLOR(0), 3);
        PushCount(pPush + 4, NV097_SET_BACK_MATERIAL_EMISSION(0), 3);
        PushCount(pPush + 8, NV097_SET_BACK_MATERIAL_ALPHA, 1);
    }
}
//------------------------------------------------------------------------------
// SetSpecularParameters

PPUSH SetSpecularParameters(
    PPUSH pPush)
{
    FLOAT L, M, N, L1, M1, N1;

    CDevice* pDevice = g_pDevice;

    DWORD i = D3D__RenderState[D3DRS_TWOSIDEDLIGHTING];
    FLOAT power = pDevice->m_Material.Power;

    PushCount(pPush, NV097_SET_SPECULAR_PARAMS(0), 6);

    while (TRUE)
    {
        Explut(power, &L, &M);
        N = 1.0f + L - M;
        Explut(0.5f * power, &L1, &M1);
        N1 = 1.0f + L1 - M1;
        *((FLOAT*) pPush + 1) = L;
        *((FLOAT*) pPush + 2) = M;
        *((FLOAT*) pPush + 3) = N;
        *((FLOAT*) pPush + 4) = L1;
        *((FLOAT*) pPush + 5) = M1;
        *((FLOAT*) pPush + 6) = N1;
    
        pPush += 7;
        if (i-- == 0)
            return pPush;

        power = pDevice->m_BackMaterial.Power;

        PushCount(pPush, NV097_SET_BACK_SPECULAR_PARAMS(0), 6);
    }
}

//------------------------------------------------------------------------------
// LazySetLights - corresponds to nvSetKelvinLights

VOID LazySetLights(
    CDevice* pDevice)
{
    PPUSH pPush;
    D3DVECTOR pos;
    D3DVECTOR dir;
    D3DVECTOR hv;
    FLOAT ambientR, ambientG, ambientB;
    FLOAT emissiveR, emissiveG, emissiveB;

    COUNT_PERF(PERF_SETSTATE_LIGHTS);

    BOOL isSpecularNeeded = (D3D__RenderState[D3DRS_SPECULARENABLE] ||
            (pDevice->m_StateFlags & STATE_COMBINERNEEDSSPECULAR));

    // Disable hardware lighting if the application has disabled it, or
    // we're running a vertex shader program, or we're in passthrough mode, 
    // or 192 constants mode is enabled

    if ((pDevice->m_pVertexShader->Flags & (VERTEXSHADER_PROGRAM | 
                                            VERTEXSHADER_PASSTHROUGH)) ||
        !(D3D__RenderState[D3DRS_LIGHTING]) ||
        ((pDevice->m_ConstantMode & ~D3DSCM_NORESERVEDCONSTANTS) == 
                                                  D3DSCM_192CONSTANTS))
    {
        // hardware lighting disabled
        pPush = pDevice->StartPush();

        Push1(pPush, NV097_SET_LIGHTING_ENABLE, FALSE);
        Push1(pPush + 2, NV097_SET_SPECULAR_ENABLE, isSpecularNeeded);

        // ZERO_OUT sets alpha component to 1.0 (method is horribly named).
        // SEPARATE_SPECULAR_EN is a don't-care:

        Push1(pPush + 4, 
              NV097_SET_LIGHT_CONTROL,
              (DRF_DEF(097, _SET_LIGHT_CONTROL, _LOCALEYE, _FALSE) |
               DRF_DEF(097, _SET_LIGHT_CONTROL, _SOUT, _PASSTHROUGH) |          
               DRF_DEF(097, _SET_LIGHT_CONTROL, _SEPARATE_SPECULAR_EN, _TRUE)));

        BOOL twoSided = D3D__RenderState[D3DRS_TWOSIDEDLIGHTING];
        Push1(pPush + 6, NV097_SET_TWO_SIDE_LIGHT_EN, twoSided);    // 0x17c4

        pDevice->EndPush(pPush + 8);
    }
    else
    {
        // hardware lighting is enabled
        pPush = pDevice->StartPush();

        // SOUT is misnamed.  And D3D never lumps specular in with diffuse:

        DWORD control = 
            (DRF_DEF(097, _SET_LIGHT_CONTROL, _SOUT, _ZERO_OUT) |
             DRF_DEF(097, _SET_LIGHT_CONTROL, _SEPARATE_SPECULAR_EN, _TRUE));

        if (isSpecularNeeded)
        {
            // Use local viewer only if we really need to, since it's slow:

            if ((D3D__RenderState[D3DRS_LOCALVIEWER]) &&
                (pDevice->m_pActiveLights != NULL))
            {
                control |= DRF_DEF(097, _SET_LIGHT_CONTROL, _LOCALEYE, _TRUE);
            }

            pPush = SetSpecularParameters(pPush);
        }

        Push1(pPush, NV097_SET_LIGHT_CONTROL, control);             // 0x294

        Push1(pPush + 2, NV097_SET_LIGHTING_ENABLE, TRUE);          // 0x314

        BOOL twoSided = D3D__RenderState[D3DRS_TWOSIDEDLIGHTING];

        Push1(pPush + 4, NV097_SET_TWO_SIDE_LIGHT_EN, twoSided);    // 0x17c4

        // Always set specular-enable to TRUE in T&L mode, due to Kelvin's
        // perverse specular behavior:

        Push1(pPush + 6, NV097_SET_SPECULAR_ENABLE, TRUE);          // 0x3b8

        DWORD colorMaterial = GetColorMaterial();

        Push1(pPush + 8, NV097_SET_COLOR_MATERIAL, colorMaterial);  // 0x298

        pPush = SetSceneAmbientAndMaterialEmission(pPush + 10, colorMaterial);

        // Download transform and lighting methods. Pack lights and send them:

        DWORD enableMask = 0;
        DWORD lightNum = 0;
        Light* pLight;

        for (pLight = pDevice->m_pActiveLights; 
             pLight != NULL; 
             pLight = pLight->pNext)
        {
            pDevice->EndPush(pPush);
            pPush = pDevice->StartPush();

            pPush = SetLightColors(pPush, pLight, lightNum, colorMaterial);

            if (pLight->Light8.Type == D3DLIGHT_DIRECTIONAL)
            {
                enableMask |= 
                (NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_INFINITE << (lightNum << 1));

                // Transform the spotlight direction vector back:

                XformBy4x3(&dir, &pLight->Direction, 0.f, 
                           &pDevice->m_Transform[D3DTS_VIEW]);

                // Unless the view matrix has non-uniform scales in it, this is
                // not necessary.  For now, we take the easy way out though.

                NormalizeVector3(&dir);

                AddVectors3(&hv, &dir, &g_EyeDirection);

                NormalizeVector3(&hv);

                Push1f(pPush, NV097_SET_LIGHT_LOCAL_RANGE(lightNum), 1e30f);
                                                                    // 0x1024
                PushCount(pPush + 2,
                          NV097_SET_LIGHT_INFINITE_HALF_VECTOR(lightNum, 0),
                          6);                                       // 0x1028

                *((FLOAT*) pPush + 3) = hv.x;
                *((FLOAT*) pPush + 4) = hv.y;
                *((FLOAT*) pPush + 5) = hv.z;

                // NV097_SET_LIGHT_INFINITE_DIRECTION:              // 0x1034

                *((FLOAT*) pPush + 6) = dir.x;
                *((FLOAT*) pPush + 7) = dir.y;
                *((FLOAT*) pPush + 8) = dir.z;

                pPush += 9;
            }
            else // Handle D3DLIGHT_POINT and D3DLIGHT_SPOT lights
            {
                Push1f(pPush, 
                       NV097_SET_LIGHT_LOCAL_RANGE(lightNum), 
                       pLight->Light8.Range);

                // We don't normalize the position:

                XformBy4x3(&pos, &pLight->Light8.Position, 1.0f, 
                           &pDevice->m_Transform[D3DTS_VIEW]);

                PushCount(pPush + 2, 
                          NV097_SET_LIGHT_LOCAL_POSITION(lightNum,0), 6);
                                                                    // 0x105c

                *((FLOAT*) pPush + 3) = pos.x;
                *((FLOAT*) pPush + 4) = pos.y;
                *((FLOAT*) pPush + 5) = pos.z;

                // NV097_SET_LIGHT_LOCAL_ATTENUATION:               // 0x1068

                *((FLOAT*) pPush + 6) = pLight->Light8.Attenuation0;
                *((FLOAT*) pPush + 7) = pLight->Light8.Attenuation1;
                *((FLOAT*) pPush + 8) = pLight->Light8.Attenuation2;

                pPush += 9;

                if (pLight->Light8.Type == D3DLIGHT_POINT)
                {
                    enableMask |= 
                    (NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_LOCAL << (lightNum << 1));
                }
                else
                {
                    ASSERT(pLight->Light8.Type == D3DLIGHT_SPOT);

                    enableMask |= 
                    (NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_SPOT << (lightNum << 1));

                    // Transform the spotlight direction vector back

                    XformBy4x3(&dir, &pLight->Direction, 0.0f, 
                               &pDevice->m_Transform[D3DTS_VIEW]);

                    // unless the view matrix has non-uniform scales in it,
                    // this is not necessary.  For now, we take the easy way
                    // out though.

                    NormalizeVector3(&dir);

                    ScaleVector3(&dir, &dir, pLight->Scale);

                    PushCount(pPush, 
                              NV097_SET_LIGHT_SPOT_FALLOFF(lightNum, 0), 7);
                                                                    // 0x1040

                    *((FLOAT*) pPush + 1) = pLight->Falloff_L;
                    *((FLOAT*) pPush + 2) = pLight->Falloff_M;
                    *((FLOAT*) pPush + 3) = pLight->Falloff_N;

                    // NV097_SET_LIGHT_SPOT_DIRECTION:              // 0x104c

                    *((FLOAT*) pPush + 4) = dir.x;
                    *((FLOAT*) pPush + 5) = dir.y;
                    *((FLOAT*) pPush + 6) = dir.z;
                    *((FLOAT*) pPush + 7) = pLight->W;

                    pPush += 8;
                }
            }

            // The hardware supports but 8 lights, so cap it there:

            if (++lightNum == 8)
                break;
        }

        // Tell hardware which lights are active:

        Push1(pPush, NV097_SET_LIGHT_ENABLE_MASK, enableMask);      // 0x3bc
        pDevice->EndPush(pPush + 2);
    }
}

//------------------------------------------------------------------------------
// LazySetTransform - corresponds to nvSetKelvinTransform

VOID LazySetTransform(
    CDevice* pDevice)
{
    D3DALIGN D3DMATRIX modelView;
    D3DMATRIX inverseModelView;
    D3DALIGN D3DMATRIX modelViewBlend;
    D3DMATRIX inverseModelViewBlend;
    D3DALIGN D3DMATRIX composite;
    DWORD stride;
    DWORD i;

    COUNT_PERF(PERF_SETSTATE_TRANSFORM);

    // If SetModelView was last called, we're already done:

    if (D3D__DirtyFlags & D3DDIRTYFLAG_DIRECT_MODELVIEW)
        return;

    // Make sure we don't whack the constant registers if we're running
    // a vertex program or if we're in 192 constants mode

    if ((!(pDevice->m_pVertexShader->Flags & (VERTEXSHADER_PROGRAM |
                                              VERTEXSHADER_PASSTHROUGH))) &&
        ((pDevice->m_ConstantMode & ~D3DSCM_NORESERVEDCONSTANTS) != 
                                                     D3DSCM_192CONSTANTS))
    {   
        PPUSH pPush = pDevice->StartPush();
    
        // Cheops modelview = D3D world * D3D view:
    
        MatrixProduct4x4(&modelView,
                         &pDevice->m_Transform[D3DTS_WORLD],
                         &pDevice->m_Transform[D3DTS_VIEW]);

        PushMatrixTransposed(pPush,
                             NV097_SET_MODEL_VIEW_MATRIX0(0),
                             &modelView);
        pPush += 17;

        // Compute the inverse modelview matrix only if lighting or 
        // texgen requires it:

        if ((pDevice->m_TexGenInverseNeeded) ||
            (D3D__RenderState[D3DRS_LIGHTING]))
        {
            Inverse4x4(&inverseModelView,
                       &modelView,
                       !D3D__RenderState[D3DRS_NORMALIZENORMALS]);
    
            PushInverseModelViewMatrix(pPush,
                                       NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0),
                                       &inverseModelView);
            pPush += 13;
        }
    
        if (!D3D__RenderState[D3DRS_VERTEXBLEND])
        {
            // We're not skinning.
            //
            // Compute the composite matrix normally and skip the second set
            // of modelview matrices.
            //
            // cheops composite = D3D World * View * projection plus
            // accomodations for the viewport
    
            MatrixProduct4x4(&composite,
                             &modelView,
                             &pDevice->m_ProjectionViewportTransform);
    
            PushMatrixTransposed(pPush,
                                 NV097_SET_COMPOSITE_MATRIX(0),
                                 &composite);
            pPush += 17;
        }
        else
        {
            // When skinning, the composite matrix does NOT include the
            // modelview matrix.
            //
            // cheops composite = projection plus accomodations for the viewport.
    
            PushMatrixTransposed(pPush,
                                 NV097_SET_COMPOSITE_MATRIX(0),
                                 &pDevice->m_ProjectionViewportTransform);
            pPush += 17;
    
            pDevice->EndPush(pPush);
            pPush = pDevice->StartPush();
    
            // We also need to compute and send down the second set of 
            // modelview matrices:
    
            for (i = 1; i < NUM_BLEND_MATRICES; i++)
            {
                MatrixProduct4x4(&modelViewBlend,
                                 &pDevice->m_Transform[D3DTS_WORLDMATRIX(i)],
                                 &pDevice->m_Transform[D3DTS_VIEW]);
    
                stride = NV097_SET_MODEL_VIEW_MATRIX1(0)
                       - NV097_SET_MODEL_VIEW_MATRIX0(0);
    
                PushMatrixTransposed(pPush,
                                     NV097_SET_MODEL_VIEW_MATRIX0(0) + i * stride,
                                     &modelViewBlend);
                pPush += 17;

                // Compute the inverse modelview matrix only if lighting or 
                // texgen requires it:

                if ((pDevice->m_TexGenInverseNeeded) ||
                    (D3D__RenderState[D3DRS_LIGHTING]))
                {
                    Inverse4x4(&inverseModelViewBlend,
                               &modelViewBlend,
                               !D3D__RenderState[D3DRS_NORMALIZENORMALS]);
        
                    stride = NV097_SET_INVERSE_MODEL_VIEW_MATRIX1(0)
                           - NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0);
        
                    PushInverseModelViewMatrix(
                                    pPush,
                                    NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0) 
                                        + i * stride,
                                    &inverseModelViewBlend);
                    pPush += 13;
                }
            }
        }
    
        pDevice->EndPush(pPush);
    }
}

//------------------------------------------------------------------------------
// SET_STATE_FLAGS
//
// These are the D3D__DirtyFlags flags handled by SetState:

#define SET_STATE_FLAGS ~(D3DDIRTYFLAG_VERTEXFORMAT_VB |         \
                          D3DDIRTYFLAG_VERTEXFORMAT_UP |         \
                          D3DDIRTYFLAG_VERTEXFORMAT_OFFSETS |    \
                          D3DDIRTYFLAG_DIRECT_INPUT |            \
                          D3DDIRTYFLAG_DIRECT_MODELVIEW)

//------------------------------------------------------------------------------
// SetState - corresponds to nvSetkelvinState
//
// Flushes any lazily pending render-states/texture-stage-states.
//
// This should be called before any rendering.

VOID SetState()
{
    CDevice* pDevice = g_pDevice;

    DWORD dirtyFlags = D3D__DirtyFlags;

    if (dirtyFlags & D3DDIRTYFLAG_POINTPARAMS)
        LazySetPointParams(pDevice);

    if (dirtyFlags & D3DDIRTYFLAG_SHADER_STAGE_PROGRAM)
        LazySetShaderStageProgram(pDevice);

    if (dirtyFlags & D3DDIRTYFLAG_COMBINERS)
        LazySetCombiners(pDevice);

    if (dirtyFlags & D3DDIRTYFLAG_TEXTURE_STATE)
        LazySetTextureState(pDevice);

    if (dirtyFlags & D3DDIRTYFLAG_SPECFOG_COMBINER)
        LazySetSpecFogCombiner(pDevice);

    if (dirtyFlags & D3DDIRTYFLAG_TEXTURE_TRANSFORM)
        LazySetTextureTransform(pDevice);

    if (dirtyFlags & D3DDIRTYFLAG_LIGHTS)
        LazySetLights(pDevice);

    if (dirtyFlags & D3DDIRTYFLAG_TRANSFORM)
        LazySetTransform(pDevice);

    // Clear the dirty flags for any state we just set:

    D3D__DirtyFlags = dirtyFlags & ~SET_STATE_FLAGS;
}

//------------------------------------------------------------------------------
// SetStateVB
//
// Do the required setup for rendering calls that use vertex buffers.

VOID CDevice::SetStateVB(
    DWORD IndexBase)    // 0 when called from DrawPrimitive, pDevice->m_IndexBase
                        //   when called from DrawIndexedPrimitive
{
    DWORD i;
    PPUSH pPush;
    BYTE* pSlotMapping;
    VertexShader* pVertexShader;

    CDevice* pDevice = this;

    // Set the common state:

    DWORD dirtyFlags = D3D__DirtyFlags;

    // Clear the dirty flags for any state we're about to set in this routine:

    D3D__DirtyFlags = dirtyFlags & ~(D3DDIRTYFLAG_VERTEXFORMAT_VB |
                                     D3DDIRTYFLAG_VERTEXFORMAT_OFFSETS);

    // Set any common state:

    if (dirtyFlags & SET_STATE_FLAGS)
        SetState();

    // If SetVertexShaderInput was last called, we're already done.
    //
    // PERF: We could get rid of m_CachedIndexBase by using a different
    //       dirty flag distinguishing between DrawPrimitive and
    //       DrawIndexedPrimitive:

    if (!(dirtyFlags & D3DDIRTYFLAG_DIRECT_INPUT) &&
         ((dirtyFlags & D3DDIRTYFLAG_VERTEXFORMAT_OFFSETS) ||
          (pDevice->m_CachedIndexBase != IndexBase)))
    {
        pDevice->m_CachedIndexBase = IndexBase;

        pVertexShader = pDevice->m_pVertexShader;

        ASSERT(VERTEXSHADER_PROGRAM == 16);
        pSlotMapping 
            = &g_SlotMapping[pVertexShader->Flags & VERTEXSHADER_PROGRAM];

        pPush = pDevice->StartPush();

        if (dirtyFlags & D3DDIRTYFLAG_VERTEXFORMAT_VB)
        {
            PushCount(pPush++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0), 16);

            for (i = 0; i < 16; i++)
            {
                DWORD slot = (DWORD) pSlotMapping[i];
                *pPush++ = pVertexShader->Slot[slot].SizeAndType
                    + (g_Stream[pVertexShader->Slot[slot].StreamIndex].Stride << 8);
            }
        }

        if (IndexBase == 0)
        {
            for (i = 0; i < 16; i++)
            {
                DWORD slot = pSlotMapping[i];
                if (pVertexShader->Slot[slot].SizeAndType != SIZEANDTYPE_DISABLED)
                {
                    Stream* pStream = &g_Stream[pVertexShader->Slot[slot].StreamIndex];

                    // NOTE: This non-NULL check isn't strictly required, 
                    // except for the BeginPushBuffer/DrawPrimUP/EndPushBuffer
                    // case:

                    if (pStream->pVertexBuffer != NULL)
                    {
                        DWORD offset = GetGPUDataFromResource(pStream->pVertexBuffer)
                                     + pVertexShader->Slot[slot].Offset
                                     + pStream->Offset;
    
                        Push1(pPush,
                              NV097_SET_VERTEX_DATA_ARRAY_OFFSET(i),
                              offset);
    
                        pPush += 2;
                    }
                }
            }
        }
        else
        {
            for (i = 0; i < 16; i++)
            {
                DWORD slot = pSlotMapping[i];
                if (pVertexShader->Slot[slot].SizeAndType != SIZEANDTYPE_DISABLED)
                {
                    Stream* pStream = &g_Stream[pVertexShader->Slot[slot].StreamIndex];

                    if (pStream->pVertexBuffer != NULL)
                    {
                        DWORD offset = GetGPUDataFromResource(pStream->pVertexBuffer)
                                     + pVertexShader->Slot[slot].Offset
                                     + pStream->Offset
                                     + IndexBase * pStream->Stride;
    
                        Push1(pPush,
                              NV097_SET_VERTEX_DATA_ARRAY_OFFSET(i),
                              offset);
    
                        pPush += 2;
                    }
                }
            }
        }

        pDevice->EndPush(pPush);
    }

    // On debug builds, make sure the hardware won't generate a state error:

    PRIMITIVE_LAUNCH_STATE_TEST();
    PRIMITIVE_LAUNCH_STATE_TEST2();
}

//------------------------------------------------------------------------------
// SetStateUP
//
// Do the required setup for UP rendering calls.
//
// We've built the vertex-shader structure for best use by VB calls
// (since those will be by far the most time critical).  So for UP calls
// we do some extra processing now to get things into a form for inline
// processing, for use by 'DumpInlineNonIndexedVertices' and
// 'DumpInlineIndexedVertices'.

VOID CDevice::SetStateUP()
{
    DWORD i;
    PPUSH pPush;

    CDevice* pDevice = this;

    // Set the common state:

    if (D3D__DirtyFlags & SET_STATE_FLAGS)
        SetState();

    if (D3D__DirtyFlags & D3DDIRTYFLAG_VERTEXFORMAT_UP)
    {
        // Because this routine stomps the strides to zero, we'll have to
        // reprogram the registers if we need to set up for a VB:

        D3D__DirtyFlags |= D3DDIRTYFLAG_VERTEXFORMAT;
        D3D__DirtyFlags &= ~D3DDIRTYFLAG_VERTEXFORMAT_UP;

        VertexShader* pVertexShader = pDevice->m_pVertexShader;

        ASSERT(VERTEXSHADER_PROGRAM == 16);
        BYTE* pSlotMapping 
            = &g_SlotMapping[pVertexShader->Flags & VERTEXSHADER_PROGRAM];

        DWORD totalDwords = 0;
        DWORD attributeCount = 0;
        DWORD endOffset = 0;
        DWORD startOffset;

        pPush = pDevice->StartPush();

        PushCount(pPush++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0), 16);

        // Now go through the slots in order (because that's the order in 
        // which we have to transmit them when sending them inline), and 
        // compute the dword size and delta for each:

        for (DWORD i = 0; i < 16; i++)
        {
            DWORD slot = pSlotMapping[i];
            DWORD sizeAndType = pVertexShader->Slot[slot].SizeAndType;

            *pPush++ = sizeAndType;

            if (sizeAndType != SIZEANDTYPE_DISABLED)
            {
                DWORD bytes = g_UnitsOfElement[(sizeAndType & 0xf0) >> 4]
                            * g_BytesPerUnit[sizeAndType & 0xf];

                ASSERT(bytes != 0);

                // The NV2A requires that every attribute be padded to a
                // DWORD when sent inline:

                DWORD dwords = (bytes + 3) >> 2;

                DWORD offset = pVertexShader->Slot[slot].Offset;
                if (attributeCount == 0)
                {
                    startOffset = offset;

                    pDevice->m_InlineAttributeData[0].UP_Count = dwords;

                    attributeCount++;
                }
                else
                {
                    DWORD delta = offset - endOffset;
                    if (delta == 0)
                    {
                        // The new attribute begins at the end of the old
                        // attribute, so there's really no need to create a new 
                        // attribute.  We simply append to the old one:
    
                        pDevice->m_InlineAttributeData[attributeCount - 1].UP_Count 
                            += dwords;
                    }
                    else
                    {
                        // Create a new attribute:

                        pDevice->m_InlineAttributeData[attributeCount - 1].UP_Delta 
                            = delta;
        
                        pDevice->m_InlineAttributeData[attributeCount].UP_Count 
                            = dwords;
        
                        attributeCount++;
                    }
                }

                totalDwords += dwords;
                endOffset = offset + 4*dwords;
            }
        }

        pDevice->EndPush(pPush);

        ASSERT(attributeCount > 0);

        if (DBG_CHECK(totalDwords <= 1))
        {
            DPF_ERR("Inline vertices of size 1 DWORD don't work on the NV2A.\n"
                    "The workaround is to pad the vertex to fill 2 DWORDs (e.g.,\n"
                    "append an unused attribute)\n");
        }

        pDevice->m_InlineAttributeCount = attributeCount;
        pDevice->m_InlineVertexDwords = totalDwords;
        pDevice->m_InlineStartOffset = startOffset;

        // Now convert the 'delta' fields to the byte offset to jump from the
        // end of the current source attribute to the start of the next source
        // attribute:

        pDevice->m_InlineDelta = startOffset - endOffset;
    }

    // On debug builds, make sure the hardware won't generate a state error:

    PRIMITIVE_LAUNCH_STATE_TEST();
    PRIMITIVE_LAUNCH_STATE_TEST2();
}

} // end of namespace

