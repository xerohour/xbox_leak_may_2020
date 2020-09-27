/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       state.cpp
 *  Content:    Handles any state-changing APIs, either by whacking the
 *              hardware immediately or marking them as dirty for
 *              later lazy setting.
 *
 ***************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

const float F_MINUS_8 = -8.0f;

//------------------------------------------------------------------------------
// Table for handling TEXCOORDINDEX re-mapping
//
// Because of D3DRS_TEXCOORDINDEX, texture coordinates for the fixed-function
// pipeline may be mapped arbitrarily to the hardware texture stages.  This
// array handles that remapping (the D3DRS_TEXCOORDINDEX render state can
// modify the entries for SLOT_TEXTURE0 through SLOT_TEXTURE3).
//
// The first 16 bytes are for the fixed-function pipeline, and the second 16
// bytes are for vertex shaders (which don't respect D3DRS_TEXCOORDINDEX and
// so always get an identity mapping).

BYTE g_SlotMapping[] = 
{ 
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
};

//------------------------------------------------------------------------------
// g_ComplexRenderStateFunctionTable

typedef VOID (*SetComplexRenderStateFunction)(DWORD Value);

D3DCONST SetComplexRenderStateFunction g_ComplexRenderStateFunctionTable[] = 
{
    D3DDevice_SetRenderState_PSTextureModes,
    D3DDevice_SetRenderState_VertexBlend,
    D3DDevice_SetRenderState_FogColor,
    D3DDevice_SetRenderState_FillMode,
    D3DDevice_SetRenderState_BackFillMode,
    D3DDevice_SetRenderState_TwoSidedLighting,
    D3DDevice_SetRenderState_NormalizeNormals,
    D3DDevice_SetRenderState_ZEnable,
    D3DDevice_SetRenderState_StencilEnable,
    D3DDevice_SetRenderState_StencilFail,
    D3DDevice_SetRenderState_FrontFace,
    D3DDevice_SetRenderState_CullMode,
    D3DDevice_SetRenderState_TextureFactor,
    D3DDevice_SetRenderState_ZBias,
    D3DDevice_SetRenderState_LogicOp,
    D3DDevice_SetRenderState_EdgeAntiAlias,
    D3DDevice_SetRenderState_MultiSampleAntiAlias,
    D3DDevice_SetRenderState_MultiSampleMask,
    D3DDevice_SetRenderState_MultiSampleMode,
    D3DDevice_SetRenderState_MultiSampleRenderTargetMode,
    D3DDevice_SetRenderState_ShadowFunc,
    D3DDevice_SetRenderState_LineWidth,
    D3DDevice_SetRenderState_Dxt1NoiseEnable,
    D3DDevice_SetRenderState_YuvEnable,
    D3DDevice_SetRenderState_OcclusionCullEnable,
    D3DDevice_SetRenderState_StencilCullEnable,
    D3DDevice_SetRenderState_RopZCmpAlwaysRead,
    D3DDevice_SetRenderState_RopZRead,
    D3DDevice_SetRenderState_DoNotCullUncompressed,
};

#if DBG

// Debug-only variable for tracking if stencil was ever enabled:

BOOL g_StencilEverEnabled;

// Debug-only variable for tracking if caller was ever warned about Z-clears
// being faster if Stencil is cleared at the same time:

BOOL g_WarnedAboutStencil;

// Debug-only variable for tracking if caller was ever warned about 
// rops / clipping rects with swizzled textures.

BOOL g_WarnAboutCopyRectStateSwizzle;

//------------------------------------------------------------------------------
// g_RenderStateEncodings
//
// This array is used on debug exclusively to verify that the inline
// D3DSIMPLERENDERSTATEENCODE array in d3d8.x contains the right values,
// and to re-generate the hex values for that table (do an "nmake state.cod")

#define E(Method) PUSHER_METHOD(SUBCH_3D, (Method), 1)

D3DCONST DWORD g_RenderStateEncodings[] =
{
    // NOTE: Any unused slots should be assigned NV097_NO_OPERATION because
    // state blocks will try to restore their values...

    E(NV097_SET_COMBINER_ALPHA_ICW(0)),        D3DRS_PSALPHAINPUTS0,
    E(NV097_SET_COMBINER_ALPHA_ICW(1)),        D3DRS_PSALPHAINPUTS1,
    E(NV097_SET_COMBINER_ALPHA_ICW(2)),        D3DRS_PSALPHAINPUTS2,
    E(NV097_SET_COMBINER_ALPHA_ICW(3)),        D3DRS_PSALPHAINPUTS3,
    E(NV097_SET_COMBINER_ALPHA_ICW(4)),        D3DRS_PSALPHAINPUTS4,
    E(NV097_SET_COMBINER_ALPHA_ICW(5)),        D3DRS_PSALPHAINPUTS5,
    E(NV097_SET_COMBINER_ALPHA_ICW(6)),        D3DRS_PSALPHAINPUTS6,
    E(NV097_SET_COMBINER_ALPHA_ICW(7)),        D3DRS_PSALPHAINPUTS7,
    E(NV097_SET_COMBINER_SPECULAR_FOG_CW0),    D3DRS_PSFINALCOMBINERINPUTSABCD,
    E(NV097_SET_COMBINER_SPECULAR_FOG_CW1),    D3DRS_PSFINALCOMBINERINPUTSEFG,
    E(NV097_SET_COMBINER_FACTOR0(0)),          D3DRS_PSCONSTANT0_0,
    E(NV097_SET_COMBINER_FACTOR0(1)),          D3DRS_PSCONSTANT0_1,
    E(NV097_SET_COMBINER_FACTOR0(2)),          D3DRS_PSCONSTANT0_2,
    E(NV097_SET_COMBINER_FACTOR0(3)),          D3DRS_PSCONSTANT0_3,
    E(NV097_SET_COMBINER_FACTOR0(4)),          D3DRS_PSCONSTANT0_4,
    E(NV097_SET_COMBINER_FACTOR0(5)),          D3DRS_PSCONSTANT0_5,
    E(NV097_SET_COMBINER_FACTOR0(6)),          D3DRS_PSCONSTANT0_6,
    E(NV097_SET_COMBINER_FACTOR0(7)),          D3DRS_PSCONSTANT0_7,
    E(NV097_SET_COMBINER_FACTOR1(0)),          D3DRS_PSCONSTANT1_0,
    E(NV097_SET_COMBINER_FACTOR1(1)),          D3DRS_PSCONSTANT1_1,
    E(NV097_SET_COMBINER_FACTOR1(2)),          D3DRS_PSCONSTANT1_2,
    E(NV097_SET_COMBINER_FACTOR1(3)),          D3DRS_PSCONSTANT1_3,
    E(NV097_SET_COMBINER_FACTOR1(4)),          D3DRS_PSCONSTANT1_4,
    E(NV097_SET_COMBINER_FACTOR1(5)),          D3DRS_PSCONSTANT1_5,
    E(NV097_SET_COMBINER_FACTOR1(6)),          D3DRS_PSCONSTANT1_6,
    E(NV097_SET_COMBINER_FACTOR1(7)),          D3DRS_PSCONSTANT1_7,
    E(NV097_SET_COMBINER_ALPHA_OCW(0)),        D3DRS_PSALPHAOUTPUTS0,
    E(NV097_SET_COMBINER_ALPHA_OCW(1)),        D3DRS_PSALPHAOUTPUTS1,
    E(NV097_SET_COMBINER_ALPHA_OCW(2)),        D3DRS_PSALPHAOUTPUTS2,
    E(NV097_SET_COMBINER_ALPHA_OCW(3)),        D3DRS_PSALPHAOUTPUTS3,
    E(NV097_SET_COMBINER_ALPHA_OCW(4)),        D3DRS_PSALPHAOUTPUTS4,
    E(NV097_SET_COMBINER_ALPHA_OCW(5)),        D3DRS_PSALPHAOUTPUTS5,
    E(NV097_SET_COMBINER_ALPHA_OCW(6)),        D3DRS_PSALPHAOUTPUTS6,
    E(NV097_SET_COMBINER_ALPHA_OCW(7)),        D3DRS_PSALPHAOUTPUTS7,
    E(NV097_SET_COMBINER_COLOR_ICW(0)),        D3DRS_PSRGBINPUTS0,
    E(NV097_SET_COMBINER_COLOR_ICW(1)),        D3DRS_PSRGBINPUTS1,
    E(NV097_SET_COMBINER_COLOR_ICW(2)),        D3DRS_PSRGBINPUTS2,
    E(NV097_SET_COMBINER_COLOR_ICW(3)),        D3DRS_PSRGBINPUTS3,
    E(NV097_SET_COMBINER_COLOR_ICW(4)),        D3DRS_PSRGBINPUTS4,
    E(NV097_SET_COMBINER_COLOR_ICW(5)),        D3DRS_PSRGBINPUTS5,
    E(NV097_SET_COMBINER_COLOR_ICW(6)),        D3DRS_PSRGBINPUTS6,
    E(NV097_SET_COMBINER_COLOR_ICW(7)),        D3DRS_PSRGBINPUTS7,
    E(NV097_SET_SHADER_CLIP_PLANE_MODE),       D3DRS_PSCOMPAREMODE,
    E(NV097_SET_SPECULAR_FOG_FACTOR(0)),       D3DRS_PSFINALCOMBINERCONSTANT0,
    E(NV097_SET_SPECULAR_FOG_FACTOR(1)),       D3DRS_PSFINALCOMBINERCONSTANT1,
    E(NV097_SET_COMBINER_COLOR_OCW(0)),        D3DRS_PSRGBOUTPUTS0,
    E(NV097_SET_COMBINER_COLOR_OCW(1)),        D3DRS_PSRGBOUTPUTS1,
    E(NV097_SET_COMBINER_COLOR_OCW(2)),        D3DRS_PSRGBOUTPUTS2,
    E(NV097_SET_COMBINER_COLOR_OCW(3)),        D3DRS_PSRGBOUTPUTS3,
    E(NV097_SET_COMBINER_COLOR_OCW(4)),        D3DRS_PSRGBOUTPUTS4,
    E(NV097_SET_COMBINER_COLOR_OCW(5)),        D3DRS_PSRGBOUTPUTS5,
    E(NV097_SET_COMBINER_COLOR_OCW(6)),        D3DRS_PSRGBOUTPUTS6,
    E(NV097_SET_COMBINER_COLOR_OCW(7)),        D3DRS_PSRGBOUTPUTS7,
    E(NV097_SET_COMBINER_CONTROL),             D3DRS_PSCOMBINERCOUNT,
    E(NV097_SET_COLOR_CLEAR_VALUE),            54, // Slot reserved for PSTEXTUREMODES
                                                   // (so we use a parameterized NOP)
    E(NV097_SET_DOT_RGBMAPPING),               D3DRS_PSDOTMAPPING,
    E(NV097_SET_SHADER_OTHER_STAGE_INPUT),     D3DRS_PSINPUTTEXTURE,
                                                                            
    E(NV097_SET_DEPTH_FUNC),                   D3DRS_ZFUNC,
    E(NV097_SET_ALPHA_FUNC),                   D3DRS_ALPHAFUNC,
    E(NV097_SET_BLEND_ENABLE),                 D3DRS_ALPHABLENDENABLE,
    E(NV097_SET_ALPHA_TEST_ENABLE),            D3DRS_ALPHATESTENABLE,
    E(NV097_SET_ALPHA_REF),                    D3DRS_ALPHAREF,
    E(NV097_SET_BLEND_FUNC_SFACTOR),           D3DRS_SRCBLEND,
    E(NV097_SET_BLEND_FUNC_DFACTOR),           D3DRS_DESTBLEND,
    E(NV097_SET_DEPTH_MASK),                   D3DRS_ZWRITEENABLE,
    E(NV097_SET_DITHER_ENABLE),                D3DRS_DITHERENABLE,
    E(NV097_SET_SHADE_MODE),                   D3DRS_SHADEMODE,
    E(NV097_SET_COLOR_MASK),                   D3DRS_COLORWRITEENABLE,
    E(NV097_SET_STENCIL_OP_ZFAIL),             D3DRS_STENCILZFAIL,
    E(NV097_SET_STENCIL_OP_ZPASS),             D3DRS_STENCILPASS,
    E(NV097_SET_STENCIL_FUNC),                 D3DRS_STENCILFUNC,
    E(NV097_SET_STENCIL_FUNC_REF),             D3DRS_STENCILREF,
    E(NV097_SET_STENCIL_FUNC_MASK),            D3DRS_STENCILMASK,
    E(NV097_SET_STENCIL_MASK),                 D3DRS_STENCILWRITEMASK,
    E(NV097_SET_BLEND_EQUATION),               D3DRS_BLENDOP,
    E(NV097_SET_BLEND_COLOR),                  D3DRS_BLENDCOLOR,
    E(NV097_SET_SWATH_WIDTH),                  D3DRS_SWATHWIDTH,
    E(NV097_SET_POLYGON_OFFSET_SCALE_FACTOR),  D3DRS_POLYGONOFFSETZSLOPESCALE,
    E(NV097_SET_POLYGON_OFFSET_BIAS),          D3DRS_POLYGONOFFSETZOFFSET,
    E(NV097_SET_POLY_OFFSET_POINT_ENABLE),     D3DRS_POINTOFFSETENABLE,
    E(NV097_SET_POLY_OFFSET_LINE_ENABLE),      D3DRS_WIREFRAMEOFFSETENABLE,
    E(NV097_SET_POLY_OFFSET_FILL_ENABLE),      D3DRS_SOLIDOFFSETENABLE,
};

//------------------------------------------------------------------------------
// VerifyHeaderFileEncodings
//
// Verify that the header file render and texture states are consistent
// with all the various tables we have sprinkled around everywhere.

VOID VerifyHeaderFileEncodings()
{
    DWORD i;

    // If you hit an assert in here, it's most likely because one of the
    // state tables is out-of-whack.

    ASSERT(sizeof(g_RenderStateEncodings) / (2 * sizeof(DWORD)) 
           == D3DRS_SIMPLE_MAX);
    ASSERT(sizeof(D3DSIMPLERENDERSTATEENCODE) / (sizeof(DWORD)) 
           == D3DRS_SIMPLE_MAX);

    for (i = 0; i < D3DRS_SIMPLE_MAX; i++)
    {
        ASSERT(g_RenderStateEncodings[2*i + 1] == i);
        ASSERT(g_RenderStateEncodings[2*i] == D3DSIMPLERENDERSTATEENCODE[i]);
    }

    ASSERT(sizeof(D3DDIRTYFROMRENDERSTATE) / sizeof(D3DDIRTYFROMRENDERSTATE[0])
           == (D3DRS_DEFERRED_MAX - D3DRS_SIMPLE_MAX));

    ASSERT(sizeof(g_ComplexRenderStateFunctionTable) 
         / sizeof(g_ComplexRenderStateFunctionTable[0])
           == (D3DRS_MAX - D3DRS_DEFERRED_MAX));

    ASSERT(sizeof(D3DDIRTYFROMTEXTURESTATE) / sizeof(D3DDIRTYFROMTEXTURESTATE[0])
           == (D3DTSS_DEFERRED_MAX));
}

#endif

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_EdgeAntiAlias

extern "C"
VOID WINAPI D3DDevice_SetRenderState_EdgeAntiAlias(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_EDGEANTIALIAS);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    // Note that we don't do PointSmoothEnable because that's truly
    // PointSpritesEnable, which is done via D3DRS_POINTSPRITEENABLE.

    Push2(pPush, NV097_SET_LINE_SMOOTH_ENABLE, 

          // NV097_SET_LINE_SMOOTH_ENABLE:

          Value,

          // NV097_SET_POLY_SMOOTH_ENABLE:

          Value);

    pDevice->EndPush(pPush + 3);

    D3D__RenderState[D3DRS_EDGEANTIALIAS] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_ShadowFunc

extern "C"
VOID WINAPI D3DDevice_SetRenderState_ShadowFunc(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_SHADOWFUNC);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    // Oddly enough, the hardware takes 0-based values for 
    // NV097_SET_SHADOW_DEPTH_FUNC:

    Push1(pPush, NV097_SET_SHADOW_DEPTH_FUNC, Value - D3DCMP_NEVER);

    pDevice->EndPush(pPush + 2);

    D3D__RenderState[D3DRS_SHADOWFUNC] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_FogColor

extern "C"
VOID WINAPI D3DDevice_SetRenderState_FogColor(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_FOGCOLOR);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    // Note that this requires a 'SwapRgb' whereas no other color register
    // does (why I don't know)...

    Push1(pPush, NV097_SET_FOG_COLOR, SwapRgb(Value));

    pDevice->EndPush(pPush + 2);

    D3D__RenderState[D3DRS_FOGCOLOR] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_CullMode

extern "C"
VOID WINAPI D3DDevice_SetRenderState_CullMode(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_CULLMODE);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    if (Value == D3DCULL_NONE)
    {
        Push1(pPush, NV097_SET_CULL_FACE_ENABLE, FALSE);
        pDevice->EndPush(pPush + 2);
    }
    else
    {
        Push1(pPush, NV097_SET_CULL_FACE_ENABLE, TRUE);

        DWORD cullFace = (Value == D3D__RenderState[D3DRS_FRONTFACE]) 
                       ? NV097_SET_CULL_FACE_V_FRONT
                       : NV097_SET_CULL_FACE_V_BACK;

        Push1(pPush + 2, NV097_SET_CULL_FACE, cullFace);

        pDevice->EndPush(pPush + 4);
    }

    D3D__RenderState[D3DRS_CULLMODE] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_FrontFace

extern "C"
VOID WINAPI D3DDevice_SetRenderState_FrontFace(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_FRONTFACE);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_FRONT_FACE, Value);

    pDevice->EndPush(pPush + 2);

    D3D__RenderState[D3DRS_FRONTFACE] = Value;

    // Because the hardware sets the cull-mode based on 'front' or 'back',
    // whereas our API does it based on 'clockwise' or 'counterclockwise',
    // we have to reprogram the cull-mode when our definition of 'front'
    // changes:

    D3DDevice_SetRenderState_CullMode(D3D__RenderState[D3DRS_CULLMODE]);
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_NormalizeNormals

extern "C"
VOID WINAPI D3DDevice_SetRenderState_NormalizeNormals(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_NORMALIZENORMALS);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_NORMALIZATION_ENABLE, Value);

    pDevice->EndPush(pPush + 2);

    // Transforms need to know:

    D3D__DirtyFlags |= D3DDIRTYFLAG_TRANSFORM;

    D3D__RenderState[D3DRS_NORMALIZENORMALS] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_ZEnable

extern "C"
VOID WINAPI D3DDevice_SetRenderState_ZEnable(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_ZENABLE);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, 
          NV097_SET_DEPTH_TEST_ENABLE, 
          ((Value != D3DZB_FALSE) && (pDevice->m_pZBuffer != NULL)));

    if (Value == D3DZB_USEW)
    {
        Push1(pPush + 2,
              NV097_SET_ZMIN_MAX_CONTROL, 
              DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _CULL_NEAR_FAR_EN, _FALSE)
            | DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _ZCLAMP_EN, _CULL)
            | DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _CULL_IGNORE_W, _FALSE)); 
    }
    else
    {
        Push1(pPush + 2,
              NV097_SET_ZMIN_MAX_CONTROL, 
              DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _CULL_NEAR_FAR_EN, _TRUE)
            | DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _ZCLAMP_EN, _CULL)
            | DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _CULL_IGNORE_W, _FALSE)); 
    }

    pDevice->EndPush(pPush + 4);

    DWORD oldValue = D3D__RenderState[D3DRS_ZENABLE];

    D3D__RenderState[D3DRS_ZENABLE] = Value;

    // When transitioning to or from w-buffering mode, we have to do
    // a bunch of extra junk:

    if ((oldValue == D3DZB_USEW) || (Value == D3DZB_USEW))
    {
        UpdateProjectionViewportTransform();

        CommonSetPassthroughProgram(pDevice);

        PPUSH pPush = pDevice->StartPush();

        pPush = CommonSetControl0(pDevice, pPush);

        pPush = CommonSetViewport(pDevice, pPush);

        pDevice->EndPush(pPush);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_StencilEnable

extern "C"
VOID WINAPI D3DDevice_SetRenderState_StencilEnable(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_STENCILENABLE);

    CDevice* pDevice = g_pDevice;

#if DBG

    if (DBG_CHECK(Value))
    {
        g_StencilEverEnabled = TRUE;
    }

#endif

    PPUSH pPush = pDevice->StartPush();

    pPush = CommonSetOccludeZStencilEn(pDevice, pPush);

    Push1(pPush, 
          NV097_SET_STENCIL_TEST_ENABLE, 
          (Value && (pDevice->m_pZBuffer != NULL)));

    pDevice->EndPush(pPush + 2);

    D3D__RenderState[D3DRS_STENCILENABLE] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_StencilFail

extern "C"
VOID WINAPI D3DDevice_SetRenderState_StencilFail(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_STENCILFAIL);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    pPush = CommonSetOccludeZStencilEn(pDevice, pPush);

    Push1(pPush, NV097_SET_STENCIL_OP_FAIL, Value);

    pDevice->EndPush(pPush + 2);

    D3D__RenderState[D3DRS_STENCILFAIL] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_TextureFactor

extern "C"
VOID WINAPI D3DDevice_SetRenderState_TextureFactor(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_TEXTUREFACTOR);

    CDevice* pDevice = g_pDevice;

    // Only whack the combiner-factor registers if a pixel shader is not
    // active:

    if (pDevice->m_pPixelShader == NULL)
    {
        PPUSH pPush = pDevice->StartPush();
    
        // Hit NV097_SET_COMBINER_FACTOR0 and NV097_SET_COMBINER_FACTOR1 for
        // all the stages:
    
        PushCount(pPush++, NV097_SET_COMBINER_FACTOR0(0), 2 * NUM_COMBINERS);
    
        for (DWORD i = 0; i < 2 * NUM_COMBINERS; i++)
            *pPush++ = Value;
    
        pDevice->EndPush(pPush);
    }

    D3D__RenderState[D3DRS_TEXTUREFACTOR] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_LineWidth

extern "C"
VOID WINAPI D3DDevice_SetRenderState_LineWidth(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_LINEWIDTH);

    CDevice* pDevice = g_pDevice;

    // Convert to 6.3:

    DWORD width = Round(Floatify(Value) * 8.0f * pDevice->m_SuperSampleScale);

    // Clamp to 63.875:

    if (width > 0x1ff)
    {
        width = 0x1ff;
    }

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_LINE_WIDTH, width);

    pDevice->EndPush(pPush + 2);

    D3D__RenderState[D3DRS_LINEWIDTH] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_Dxt1NoiseEnable

extern "C"
VOID WINAPI D3DDevice_SetRenderState_Dxt1NoiseEnable(
    DWORD Value)
{
    CDevice* pDevice = g_pDevice;

    // We ignore the noise-enable render state when the render target is
    // 16bpp:

    BOOL noiseEnable = FALSE;
    if (PixelJar::GetBitsPerPixel(pDevice->m_pRenderTarget) == 32)
    {
        noiseEnable = Value;
    }

    // Changing the noise-enable status is extremely expensive, so in a rare
    // departure we actually check to see if the new value is different (in
    // part because this is called for every SetRenderTarget call).

    ASSERT((STATE_DXTNOISEENABLE == TRUE) && ((noiseEnable & ~1) == 0));

    if (noiseEnable != (pDevice->m_StateFlags & STATE_DXTNOISEENABLE))
    {
        // Since this is always called from SetRenderTarget, count the 
        // API call only if the state changes:

        COUNT_API(API_D3DDEVICE_SETRENDERSTATE_DXT1NOISEENABLE);

        pDevice->m_StateFlags ^= STATE_DXTNOISEENABLE;

        PPUSH pPush = pDevice->StartPush();
    
        Push1(pPush, NV097_WAIT_FOR_IDLE, 0);

        // NVX_DXT1_NOISE_ENABLE Data stored in NV097_SET_ZSTENCIL_CLEAR_VALUE

        Push1(pPush + 2, NV097_SET_ZSTENCIL_CLEAR_VALUE, 
              noiseEnable);
    
        Push1(pPush + 4,
              NV097_NO_OPERATION,
              NVX_DXT1_NOISE_ENABLE);
    
        pDevice->EndPush(pPush + 6);
    }

    D3D__RenderState[D3DRS_DXT1NOISEENABLE] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_YuvEnable

extern "C"
VOID WINAPI D3DDevice_SetRenderState_YuvEnable(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_YUVENABLE);

    CDevice* pDevice = g_pDevice;

    D3D__RenderState[D3DRS_YUVENABLE] = Value;

    PPUSH pPush = pDevice->StartPush();

    pPush = CommonSetControl0(pDevice, pPush);

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_OcclusionCullEnable

extern "C"
VOID WINAPI D3DDevice_SetRenderState_OcclusionCullEnable(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_OCCLUSIONCULLENABLE);

    CDevice* pDevice = g_pDevice;

    D3D__RenderState[D3DRS_OCCLUSIONCULLENABLE] = Value;

    PPUSH pPush = pDevice->StartPush();

    pPush = CommonSetOccludeZStencilEn(pDevice, pPush);

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_StencilCullEnable

extern "C"
VOID WINAPI D3DDevice_SetRenderState_StencilCullEnable(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_STENCILCULLENABLE);

    CDevice* pDevice = g_pDevice;

    D3D__RenderState[D3DRS_STENCILCULLENABLE] = Value;

    PPUSH pPush = pDevice->StartPush();

    pPush = CommonSetOccludeZStencilEn(pDevice, pPush);

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_RopZCmpAlwaysRead

extern "C"
VOID WINAPI D3DDevice_SetRenderState_RopZCmpAlwaysRead(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_ROPZCMPALWAYSREAD);

    D3D__RenderState[D3DRS_ROPZCMPALWAYSREAD] = Value;

    CommonSetDebugRegisters();
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_RopZRead

extern "C"
VOID WINAPI D3DDevice_SetRenderState_RopZRead(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_ROPZREAD);

    D3D__RenderState[D3DRS_ROPZREAD] = Value;

    CommonSetDebugRegisters();
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_DoNotCullUncompressed

extern "C"
VOID WINAPI D3DDevice_SetRenderState_DoNotCullUncompressed(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_DONOTCULLUNCOMPRESSED);

    D3D__RenderState[D3DRS_DONOTCULLUNCOMPRESSED] = Value;

    CommonSetDebugRegisters();
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_ZBias
//
// DOC: This overwrites current D3DRS_POLYGONOFFSETZSLOPESCALE, 
//      D3DRS_POLYGONOFFSETZOFFSET, D3DRS_POINTOFFSETENABLE, 
//      D3DRS_WIREFRAMEOFFSETENABLE, D3DRS_SOLIDOFFSETENABLE values.

extern "C"
VOID WINAPI D3DDevice_SetRenderState_ZBias(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_ZBIAS);

    CDevice* pDevice = g_pDevice;

    // State blocks restore stuff in reverse order, so make sure ZBias won't
    // nuke any individually set parameters:

    ASSERT((D3DRS_ZBIAS > D3DRS_POLYGONOFFSETZSLOPESCALE) &&
           (D3DRS_ZBIAS > D3DRS_POLYGONOFFSETZOFFSET) &&
           (D3DRS_ZBIAS > D3DRS_POINTOFFSETENABLE) &&
           (D3DRS_ZBIAS > D3DRS_WIREFRAMEOFFSETENABLE) &&
           (D3DRS_ZBIAS > D3DRS_SOLIDOFFSETENABLE));

    BOOL enable = (Value != 0);
    FLOAT bias = -(FLOAT)Value;
    FLOAT scale = 0.25f * bias;

    pDevice->SetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE, Dwordify(scale));
    pDevice->SetRenderState(D3DRS_POLYGONOFFSETZOFFSET, Dwordify(bias));
    pDevice->SetRenderState(D3DRS_POINTOFFSETENABLE, enable);
    pDevice->SetRenderState(D3DRS_WIREFRAMEOFFSETENABLE, enable);
    pDevice->SetRenderState(D3DRS_SOLIDOFFSETENABLE, enable);

    D3D__RenderState[D3DRS_ZBIAS] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_MultiSampleMode

extern "C"
VOID WINAPI D3DDevice_SetRenderState_MultiSampleMode(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_MULTISAMPLEMODE);

    CDevice* pDevice = g_pDevice;

    D3D__RenderState[D3DRS_MULTISAMPLEMODE] = Value;

    D3DSurface* pRenderTarget = pDevice->m_pRenderTarget;
    if (pRenderTarget == pDevice->m_pFrameBuffer[0])
    {
        D3DDevice_SetRenderTarget(pRenderTarget, pDevice->m_pZBuffer); 
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_MultiSampleRenderTargetMode

extern "C"
VOID WINAPI D3DDevice_SetRenderState_MultiSampleRenderTargetMode(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_MULTISAMPLERENDERTARGETMODE);

    CDevice* pDevice = g_pDevice;

    D3D__RenderState[D3DRS_MULTISAMPLERENDERTARGETMODE] = Value;

    D3DSurface* pRenderTarget = pDevice->m_pRenderTarget;
    if (pRenderTarget != pDevice->m_pFrameBuffer[0])
    {
        D3DDevice_SetRenderTarget(pRenderTarget, pDevice->m_pZBuffer); 
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_MultiSampleAntiAlias

extern "C"
VOID WINAPI D3DDevice_SetRenderState_MultiSampleAntiAlias(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_MULTISAMPLEANTIALIAS);

    CDevice* pDevice = g_pDevice;

    D3D__RenderState[D3DRS_MULTISAMPLEANTIALIAS] = Value;

    CommonSetAntiAliasingControl(pDevice);
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_MultiSampleMask

extern "C"
VOID WINAPI D3DDevice_SetRenderState_MultiSampleMask(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_MULTISAMPLEMASK);

    CDevice* pDevice = g_pDevice;

    D3D__RenderState[D3DRS_MULTISAMPLEMASK] = Value;

    CommonSetAntiAliasingControl(pDevice);
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_LogicOp

extern "C"
VOID WINAPI D3DDevice_SetRenderState_LogicOp(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_LOGICOP);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    if (Value == D3DLOGICOP_NONE)
    {
        Push1(pPush, NV097_SET_LOGIC_OP_ENABLE, FALSE);

        pPush += 2;
    }
    else
    {
        Push2(pPush, NV097_SET_LOGIC_OP_ENABLE, 
    
              // NV097_SET_LOGIC_OP_ENABLE:
    
              TRUE,
    
              // NV097_SET_LOGIC_OP:
    
              Value);

        pPush += 3;
    }

    pDevice->EndPush(pPush);

    D3D__RenderState[D3DRS_LOGICOP] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_FillMode

extern "C"
VOID WINAPI D3DDevice_SetRenderState_FillMode(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_FILLMODE);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    DWORD backFillMode = D3D__RenderState[D3DRS_TWOSIDEDLIGHTING]
                       ? D3D__RenderState[D3DRS_BACKFILLMODE]
                       : Value;

    // Set both NV097_SET_FRONT_POLYGON_MODE and NV097_SET_BACK_POLYGON_MODE:

    Push2(pPush, NV097_SET_FRONT_POLYGON_MODE, Value, backFillMode);

    pDevice->EndPush(pPush + 3);

    D3D__RenderState[D3DRS_FILLMODE] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_BackFillMode

extern "C"
VOID WINAPI D3DDevice_SetRenderState_BackFillMode(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_BACKFILLMODE);

    D3D__RenderState[D3DRS_BACKFILLMODE] = Value;

    D3DDevice_SetRenderState_FillMode(D3D__RenderState[D3DRS_FILLMODE]);
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_TwoSidedLighting

extern "C"
VOID WINAPI D3DDevice_SetRenderState_TwoSidedLighting(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_TWOSIDEDLIGHTING);

    CDevice* pDevice = g_pDevice;

    // We could D3DDIRTYFLAG_LIGHTS only for the disable case if we actually
    // hit the two-sided enable register here...

    D3D__DirtyFlags |= D3DDIRTYFLAG_LIGHTS;

    D3D__RenderState[D3DRS_TWOSIDEDLIGHTING] = Value;

    // When two-sided lighting is disabled, FILLMODE applies to both the
    // back-sides and front-sides.  So if it changes, we have to update:

    D3DDevice_SetRenderState_FillMode(D3D__RenderState[D3DRS_FILLMODE]);
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_VertexBlend

extern "C"
VOID WINAPI D3DDevice_SetRenderState_VertexBlend(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_VERTEXBLEND);

    CDevice* pDevice = g_pDevice;

    D3D__DirtyFlags |= D3DDIRTYFLAG_TRANSFORM;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_SKIN_MODE, Value);

    pDevice->EndPush(pPush + 2);

    D3D__RenderState[D3DRS_VERTEXBLEND] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_PSTextureModes

extern "C"
VOID WINAPI D3DDevice_SetRenderState_PSTextureModes(
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_PSTEXTUREMODES);

    CDevice* pDevice = g_pDevice;

    // Make adjustments to texture modes based on current textures

    pDevice->m_PSShaderStageProgram = Value;

    D3D__DirtyFlags |= D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;

    D3D__RenderState[D3DRS_PSTEXTUREMODES] = Value;
}

#if DBG

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_ParameterCheck

extern "C"
HRESULT WINAPI D3DDevice_SetRenderState_ParameterCheck(
    D3DRENDERSTATETYPE State,
    DWORD Value) 
{ 
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_PARAMETERCHECK);

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (State >= D3DRS_MAX) 
        {
            D3D_ERR("Invalid render state type");
            return D3DERR_INVALIDCALL;
        }

        switch(State)
        {
        case D3DRS_ALPHATESTENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_ALPHAREF:
            if (Value & 0xffffff00)
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_ALPHAFUNC:
            if ((Value < D3DCMP_NEVER) || (Value > D3DCMP_ALWAYS))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_ZWRITEENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_ALPHABLENDENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_SRCBLEND:
            if (((Value > D3DBLEND_ONE) && (Value < D3DBLEND_SRCCOLOR)) ||
                ((Value > D3DBLEND_SRCALPHASAT) && (Value < D3DBLEND_CONSTANTCOLOR)) ||
                (Value > D3DBLEND_INVCONSTANTALPHA))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_DESTBLEND:
            if (((Value > D3DBLEND_ONE) && (Value < D3DBLEND_SRCCOLOR)) ||
                ((Value > D3DBLEND_SRCALPHASAT) && (Value < D3DBLEND_CONSTANTCOLOR)) ||
                (Value > D3DBLEND_INVCONSTANTALPHA))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_DITHERENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_FILLMODE:
        case D3DRS_BACKFILLMODE:
            if ((Value < D3DFILL_POINT) || (Value > D3DFILL_SOLID))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_SHADEMODE:
            if ((Value < D3DSHADE_FLAT) || (Value > D3DSHADE_GOURAUD))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_CULLMODE:
            if ((Value != D3DCULL_NONE) && 
                (Value != D3DCULL_CW) && 
                (Value != D3DCULL_CCW))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_NORMALIZENORMALS:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_SWATHWIDTH:
            if (((Value > D3DSWATH_128) && (Value < D3DSWATH_OFF)) ||
                (Value > D3DSWATH_OFF))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_ZENABLE:
            if (Value > D3DZB_USEW)
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_ZFUNC:
            if ((Value < D3DCMP_NEVER) || (Value > D3DCMP_ALWAYS))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_COLORWRITEENABLE:
            if (Value & ~(D3DCOLORWRITEENABLE_ALPHA |
                          D3DCOLORWRITEENABLE_RED |
                          D3DCOLORWRITEENABLE_GREEN |
                          D3DCOLORWRITEENABLE_BLUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_STENCILENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_STENCILFAIL:
            if (!((Value == D3DSTENCILOP_ZERO) ||
                  (Value >= D3DSTENCILOP_KEEP && Value <= D3DSTENCILOP_DECRSAT) ||
                  (Value == D3DSTENCILOP_INVERT) ||
                  (Value >= D3DSTENCILOP_INCR && Value <= D3DSTENCILOP_DECR)))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_STENCILZFAIL:
            if (!((Value == D3DSTENCILOP_ZERO) ||
                  (Value >= D3DSTENCILOP_KEEP && Value <= D3DSTENCILOP_DECRSAT) ||
                  (Value == D3DSTENCILOP_INVERT) ||
                  (Value >= D3DSTENCILOP_INCR && Value <= D3DSTENCILOP_DECR)))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_STENCILPASS:
            if (!((Value == D3DSTENCILOP_ZERO) ||
                  (Value >= D3DSTENCILOP_KEEP && Value <= D3DSTENCILOP_DECRSAT) ||
                  (Value == D3DSTENCILOP_INVERT) ||
                  (Value >= D3DSTENCILOP_INCR && Value <= D3DSTENCILOP_DECR)))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_STENCILFUNC:
            if ((Value < D3DCMP_NEVER) || (Value > D3DCMP_ALWAYS))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_STENCILREF:
            if (Value > 0xff)
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_VERTEXBLEND:
            if (Value >= D3DVBF_MAX)
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_LINEWIDTH:
            if (Floatify(Value) < 0.0f)
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_DXT1NOISEENABLE:
        case D3DRS_YUVENABLE:
        case D3DRS_OCCLUSIONCULLENABLE:
        case D3DRS_STENCILCULLENABLE:
        case D3DRS_ROPZCMPALWAYSREAD:
        case D3DRS_ROPZREAD:
        case D3DRS_DONOTCULLUNCOMPRESSED:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_POINTSIZE:
        case D3DRS_POINTSIZE_MIN:
        case D3DRS_POINTSIZE_MAX:
            if (Floatify(Value) < 0.0f)
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_POINTSCALEENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_POINTSPRITEENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_POINTOFFSETENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_WIREFRAMEOFFSETENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_SOLIDOFFSETENABLE:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_SHADOWFUNC:
            if ((Value < D3DCMP_NEVER) || (Value > D3DCMP_ALWAYS))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_ZBIAS:
            if (Value > 16)
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_MULTISAMPLEANTIALIAS:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_MULTISAMPLEMODE:
        case D3DRS_MULTISAMPLERENDERTARGETMODE:
            if ((Value != D3DMULTISAMPLEMODE_1X) &&
                (Value != D3DMULTISAMPLEMODE_2X) &&
                (Value != D3DMULTISAMPLEMODE_4X))
            {
                DPF_ERR("Invalid D3DMULTISAMPLEMODE value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_SWAPFILTER:
            if ((Value < D3DTEXF_POINT) || (Value > D3DTEXF_GAUSSIANCUBIC))
            {
                D3D_ERR("Invalid value.");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_LOGICOP:
            if (((Value < D3DLOGICOP_CLEAR) || (Value > D3DLOGICOP_SET)) &&
                (Value != D3DLOGICOP_NONE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_EDGEANTIALIAS:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_BLENDOP:
            if ((Value < D3DBLENDOP_ADD) ||
                ((Value > D3DBLENDOP_MAX) && (Value < D3DBLENDOP_SUBTRACT)) ||
                ((Value > D3DBLENDOP_REVSUBTRACT) && 
                        (Value < D3DBLENDOP_REVSUBTRACTSIGNED)) ||
                (Value > D3DBLENDOP_ADDSIGNED))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_WRAP0:
        case D3DRS_WRAP1:
        case D3DRS_WRAP2:
        case D3DRS_WRAP3:
            if (Value & ~(DRF_DEF(097, _SET_TEXTURE_ADDRESS, _CYLWRAP_U, _TRUE) 
                        | DRF_DEF(097, _SET_TEXTURE_ADDRESS, _CYLWRAP_V, _TRUE) 
                        | DRF_DEF(097, _SET_TEXTURE_ADDRESS, _CYLWRAP_P, _TRUE) 
                        | DRF_DEF(097, _SET_TEXTURE_ADDRESS, _CYLWRAP_Q, _TRUE)))
            {
                D3D_ERR("Invalid WRAP value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_LIGHTING:
        case D3DRS_TWOSIDEDLIGHTING:
        case D3DRS_SPECULARENABLE:
        case D3DRS_LOCALVIEWER:
            if ((Value != FALSE) && (Value != TRUE))
            {
                DPF_ERR("Invalid value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_BACKSPECULARMATERIALSOURCE:
        case D3DRS_BACKDIFFUSEMATERIALSOURCE:
        case D3DRS_BACKAMBIENTMATERIALSOURCE:
        case D3DRS_BACKEMISSIVEMATERIALSOURCE:
        case D3DRS_SPECULARMATERIALSOURCE:
        case D3DRS_DIFFUSEMATERIALSOURCE:
        case D3DRS_AMBIENTMATERIALSOURCE:
        case D3DRS_EMISSIVEMATERIALSOURCE:
            if ((Value != D3DMCS_COLOR1) &&
                (Value != D3DMCS_COLOR2) &&
                (Value != D3DMCS_MATERIAL))
            {
                D3D_ERR("Invalid D3DMCS value");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DRS_PSALPHAINPUTS0:
            if(!ValidCombinerInputs(Value, "PSAlphaInputs0"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAINPUTS1:
            if(!ValidCombinerInputs(Value, "PSAlphaInputs1"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAINPUTS2:
            if(!ValidCombinerInputs(Value, "PSAlphaInputs2"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAINPUTS3:
            if(!ValidCombinerInputs(Value, "PSAlphaInputs3"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAINPUTS4:
            if(!ValidCombinerInputs(Value, "PSAlphaInputs4"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAINPUTS5:
            if(!ValidCombinerInputs(Value, "PSAlphaInputs5"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAINPUTS6:
            if(!ValidCombinerInputs(Value, "PSAlphaInputs6"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAINPUTS7:
            if(!ValidCombinerInputs(Value, "PSAlphaInputs7"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBINPUTS0:
            if(!ValidCombinerInputs(Value, "PSRGBInputs0"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBINPUTS1:
            if(!ValidCombinerInputs(Value, "PSRGBInputs1"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBINPUTS2:
            if(!ValidCombinerInputs(Value, "PSRGBInputs2"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBINPUTS3:
            if(!ValidCombinerInputs(Value, "PSRGBInputs3"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBINPUTS4:
            if(!ValidCombinerInputs(Value, "PSRGBInputs4"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBINPUTS5:
            if(!ValidCombinerInputs(Value, "PSRGBInputs5"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBINPUTS6:
            if(!ValidCombinerInputs(Value, "PSRGBInputs6"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBINPUTS7:
            if(!ValidCombinerInputs(Value, "PSRGBInputs7"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAOUTPUTS0:
            if(!ValidAlphaOutputs(Value, "PSAlphaOutputs0"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAOUTPUTS1:
            if(!ValidAlphaOutputs(Value, "PSAlphaOutputs1"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAOUTPUTS2:
            if(!ValidAlphaOutputs(Value, "PSAlphaOutputs2"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAOUTPUTS3:
            if(!ValidAlphaOutputs(Value, "PSAlphaOutputs3"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAOUTPUTS4:
            if(!ValidAlphaOutputs(Value, "PSAlphaOutputs4"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAOUTPUTS5:
            if(!ValidAlphaOutputs(Value, "PSAlphaOutputs5"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAOUTPUTS6:
            if(!ValidAlphaOutputs(Value, "PSAlphaOutputs6"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSALPHAOUTPUTS7:
            if(!ValidAlphaOutputs(Value, "PSAlphaOutputs7"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBOUTPUTS0:
            if(!ValidRGBOutputs(Value, "PSRGBOutputs0"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBOUTPUTS1:
            if(!ValidRGBOutputs(Value, "PSRGBOutputs1"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBOUTPUTS2:
            if(!ValidRGBOutputs(Value, "PSRGBOutputs2"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBOUTPUTS3:
            if(!ValidRGBOutputs(Value, "PSRGBOutputs3"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBOUTPUTS4:
            if(!ValidRGBOutputs(Value, "PSRGBOutputs4"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBOUTPUTS5:
            if(!ValidRGBOutputs(Value, "PSRGBOutputs5"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBOUTPUTS6:
            if(!ValidRGBOutputs(Value, "PSRGBOutputs6"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSRGBOUTPUTS7:
            if(!ValidRGBOutputs(Value, "PSRGBOutputs7"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSFINALCOMBINERINPUTSABCD:
            if(!ValidFinalCombinerABCD(Value, "PSFinalCombinerInputsABCD"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSFINALCOMBINERINPUTSEFG:
            if(!ValidFinalCombinerEFG(Value, "PSFinalCombinerInputsEFG"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSTEXTUREMODES:
            if(!ValidTextureModes(Value, "PSTextureModes"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSINPUTTEXTURE:
            if(!ValidInputTexture(Value, "PSInputTexture"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSCOMBINERCOUNT:
            if(!ValidCombinerCount(Value, "PSCombinerCount"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSCOMPAREMODE:
            if(!ValidCompareMode(Value, "PSCompareMode"))
                return D3DERR_INVALIDCALL;
            break;
        case D3DRS_PSDOTMAPPING:
            if(!ValidDotMapping(Value, "PSDotMapping"))
                return D3DERR_INVALIDCALL;
            break;
        }
    }

    return S_OK;
}

#endif

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_Simple[_Fast]

extern "C"
#if DBG
_declspec(naked) void D3DFASTCALL D3DDevice_SetRenderState_Simple_Fast(
#else
_declspec(naked) void D3DFASTCALL D3DDevice_SetRenderState_Simple(
#endif
    DWORD Method, // Already encoded with one-dword count
    DWORD Value)
{ 
    _asm
    {
        ; ecx = Method
        ; edx = Value

    Simple_Start:
        mov     eax, g_Device.m_Pusher.m_pPut
        add     eax, 8
        cmp     eax, g_Device.m_Pusher.m_pThreshold
        jae     Simple_MakeSpace
        mov     [g_Device.m_Pusher.m_pPut], eax
        mov     [eax-8], ecx
        mov     [eax-4], edx
        ret     

    Simple_MakeSpace:
        push    edx
        push    ecx
        call    MakeSpace
        pop     ecx
        pop     edx
        jmp     Simple_Start
    }
}

#if DBG

//------------------------------------------------------------------------------
// D3DDevice_SetRenderState_Simple (Debug only)

extern "C"
VOID D3DFASTCALL D3DDevice_SetRenderState_Simple(
    DWORD Method,
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETRENDERSTATE_SIMPLE);

    if (DBG_CHECK(TRUE))
    {
        if (((Method & 3) != 0) ||
            ((Method & PUSHER_METHOD(-1, 0, 0)) !=  
                       PUSHER_METHOD(SUBCH_3D, 0, 1)))
        {
            DPF_ERR("Invalid encoding");
        }
    }

    D3DDevice_SetRenderState_Simple_Fast(Method, Value);
}

#endif DBG

//------------------------------------------------------------------------------
// D3DDevice_SetRenderStateNotInline

extern "C"
VOID WINAPI D3DDevice_SetRenderStateNotInline(
    D3DRENDERSTATETYPE State,
    DWORD Value) 
{ 
    CDevice* pDevice = g_pDevice;

#if PROFILE

    // If we're supposed to override this renderstate's value then do it.

    if(g_PerfCounters.m_RenderStateOverrides[State].Override)
        Value = g_PerfCounters.m_RenderStateOverrides[State].Value;

#endif

    if (DBG_CHECK(D3DDevice_SetRenderState_ParameterCheck(State, Value) != S_OK))
        return;

    COUNT_API(API_D3DDEVICE_SETRENDERSTATENOTINLINE);
    COUNT_RENDERSTATE(State);
    if (Value == D3D__RenderState[State])
    {
        COUNT_PERF(PERF_REDUNDANT_SETRENDERSTATE);
    }

    if (State < D3DRS_SIMPLE_MAX)
    {
        D3DDevice_SetRenderState_Simple(D3DSIMPLERENDERSTATEENCODE[State], Value);

        D3D__RenderState[State] = Value;
    }
    else if (State < D3DRS_DEFERRED_MAX)
    {
        D3D__DirtyFlags |= D3DDIRTYFROMRENDERSTATE[State - D3DRS_SIMPLE_MAX];
        D3D__RenderState[State] = Value;
    }
    else
    {
        g_ComplexRenderStateFunctionTable[State - D3DRS_DEFERRED_MAX](Value);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetTextureState_TexCoordIndex

extern "C"
VOID WINAPI D3DDevice_SetTextureState_TexCoordIndex(
    DWORD Stage, 
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETTEXTURESTATE_TEXCOORDINDEX);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK((Value & 0xffff) >= D3DDP_MAXTEXCOORD))
    {
        DPF_ERR("Index can't be D3DDP_MAXTEXCOORD (4) or more");
    }

    D3D__TextureState[Stage][D3DTSS_TEXCOORDINDEX] = Value;

    // Initialize assuming D3DTSS_TCI_PASSTHRU:

    DWORD needsInverseModelViewState = FALSE;
    DWORD mapToStage = Value;
    DWORD texgenMode = NV097_SET_TEXGEN_S_V_DISABLE;

    PPUSH pPush = pDevice->StartPush();

    // Now specifically handle texgens:

    DWORD texGen = Value & 0xffff0000;
    if (texGen != D3DTSS_TCI_PASSTHRU)
    {
        // Since texgen is enabled, we obviously don't need to read texture 
        // coordinates from the vertex buffer for this stage.  Rather than 
        // add special logic in the vertex buffer setup code, we simply make
        // sure we map to this stage, and work under the assumption that
        // the caller won't specify a texture coordinate for this stage in
        // their FVF.

        mapToStage = Stage;

        // Stomp the texture coordinate with (0, 0, 0, 1).  We do this to  
        // get a '1' into 'W' because we leave Q disabled.

        Push1(pPush, NV097_SET_VERTEX_DATA4UB(SLOT_TEXTURE0 + Stage), 0xff000000);

        pPush += 2;

        switch (texGen)
        {
        case D3DTSS_TCI_CAMERASPACENORMAL:
            needsInverseModelViewState = TRUE;
            texgenMode = NV097_SET_TEXGEN_S_V_NORMAL_MAP;
            break;
    
        case D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR:
            needsInverseModelViewState = TRUE;
            texgenMode = NV097_SET_TEXGEN_S_V_REFLECTION_MAP;
            break;
    
        case D3DTSS_TCI_CAMERASPACEPOSITION:
            // Note, we already initialized to identity the texgen plane 
            // (NV097_SET_TEXGEN_SPLANE0, etc.) in our initialization code.
    
            texgenMode = NV097_SET_TEXGEN_S_V_EYE_LINEAR;
            break;
    
        case D3DTSS_TCI_OBJECT:
            texgenMode = NV097_SET_TEXGEN_S_V_OBJECT_LINEAR;
            break;
    
        case D3DTSS_TCI_SPHERE:
            needsInverseModelViewState = TRUE;
            texgenMode = NV097_SET_TEXGEN_S_V_SPHERE_MAP;
            break;
    
        default:
            NODEFAULT("Unexpected texgen\n");
        }
    }

    // Handle the re-mapping of a stage:

    g_SlotMapping[SLOT_TEXTURE0 + Stage] = SLOT_TEXTURE0 + (BYTE) mapToStage;

    // Set S, T, and R all to the same mode.  We leave Q disabled:

    Push3(pPush, NV097_SET_TEXGEN_S(Stage), texgenMode, texgenMode, texgenMode);

    pDevice->EndPush(pPush + 4);

    // If we suddenly start needing the inverse modelview matrix, make sure
    // that LazySetTransform computes it on the very next call:

    if (!(pDevice->m_TexGenInverseNeeded) && (needsInverseModelViewState))
    {
        D3D__DirtyFlags |= D3DDIRTYFLAG_TRANSFORM;
    }
    
    pDevice->m_TexGenInverseNeeded &= ~(1 << Stage);
    pDevice->m_TexGenInverseNeeded |= (needsInverseModelViewState << Stage);


    D3D__DirtyFlags |= (D3DDIRTYFLAG_TEXTURE_TRANSFORM | 
                        D3DDIRTYFLAG_TEXTURE_STATE | 
                        D3DDIRTYFLAG_VERTEXFORMAT);
}

//------------------------------------------------------------------------------
// D3DDevice_SetTextureState_BumpEnv

extern "C"
VOID WINAPI D3DDevice_SetTextureState_BumpEnv(
    DWORD Stage, 
    D3DTEXTURESTAGESTATETYPE Type,
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETTEXTURESTATE_BUMPENV);

    CDevice* pDevice = g_pDevice;

    ASSERT((Type >= D3DTSS_BUMPENVMAT00) && (Type <= D3DTSS_BUMPENVLOFFSET));
    ASSERT((D3DTSS_BUMPENVMAT00 + 1 == D3DTSS_BUMPENVMAT01) &&
           (D3DTSS_BUMPENVMAT00 + 2 == D3DTSS_BUMPENVMAT11) &&
           (D3DTSS_BUMPENVMAT00 + 3 == D3DTSS_BUMPENVMAT10) &&
           (D3DTSS_BUMPENVMAT00 + 4 == D3DTSS_BUMPENVLSCALE) &&
           (D3DTSS_BUMPENVMAT00 + 5 == D3DTSS_BUMPENVLOFFSET));

    // D3D uses the i'th texture stage to express the bump map data, where
    // the environment is in the next stage.  NV20 uses the i+1'th stage,
    // so have to add one to the D3D stage when programming the hardware.
    //
    // But when a pixel shader is active, it maps directly.

    DWORD textureUnit = (pDevice->m_pPixelShader != NULL) ? Stage : Stage + 1;

    // Only texture units 1, 2, and 3 can be programmed:

    if (textureUnit & 3)
    {
        PPUSH pPush = pDevice->StartPush();

        // Also hit NV097_SET_TEXTURE_SET_BUMP_ENV_OFFSET and
        // NV097_SET_TEXTURE_SET_BUMP_ENV_SCALE here:
    
        Push1(pPush, 
              NV097_SET_TEXTURE_SET_BUMP_ENV_MAT00(textureUnit) 
                    + 4 * (Type - D3DTSS_BUMPENVMAT00),
              Value);
    
        pDevice->EndPush(pPush + 2);
    }

    D3D__TextureState[Stage][Type] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetTextureState_BorderColor

extern "C"
VOID WINAPI D3DDevice_SetTextureState_BorderColor(
    DWORD Stage, 
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETTEXTURESTATE_BORDERCOLOR);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_TEXTURE_BORDER_COLOR(Stage), Value);

    pDevice->EndPush(pPush + 2);

    D3D__TextureState[Stage][D3DTSS_BORDERCOLOR] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetTextureState_ColorKeyColor

extern "C"
VOID WINAPI D3DDevice_SetTextureState_ColorKeyColor(
    DWORD Stage, 
    DWORD Value)
{
    COUNT_API(API_D3DDEVICE_SETTEXTURESTATE_COLORKEYCOLOR);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_COLOR_KEY_COLOR(Stage), Value);

    pDevice->EndPush(pPush + 2);

    D3D__TextureState[Stage][D3DTSS_COLORKEYCOLOR] = Value;
}

//------------------------------------------------------------------------------
// D3DDevice_SetTextureState_ParameterCheck

extern "C"
HRESULT WINAPI D3DDevice_SetTextureState_ParameterCheck(
    DWORD Stage,
    D3DTEXTURESTAGESTATETYPE Type,
    DWORD Value) 
{ 
    COUNT_API(API_D3DDEVICE_SETTEXTURESTATE_PARAMETERCHECK);

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if ((Stage >= D3DTSS_MAXSTAGES) || (Type >= D3DTSS_MAX))
        {
            D3D_ERR("Invalid texture stage or state index");
            return D3DERR_INVALIDCALL;
        }

        switch(Type)
        {
        case D3DTSS_COLOROP:
        case D3DTSS_ALPHAOP:
            /* D3DTEXTUREOP - per-stage blending controls for color channels */

            if (!(Value) || (Value >= D3DTOP_MAX))
            {
                D3D_ERR("Invalid value for D3DTSS_COLOROP/ALPHAOP");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_COLORARG0:
        case D3DTSS_ALPHAARG0:
        case D3DTSS_COLORARG1:
        case D3DTSS_COLORARG2:
        case D3DTSS_ALPHAARG1:
        case D3DTSS_ALPHAARG2:
        case D3DTSS_RESULTARG:
            /* D3DTA_* (texture arg) */

            if ((Value & D3DTA_SELECTMASK) >= D3DTA_SELECTMAX)
            {
                D3D_ERR("Invalid value selector for D3DTSS_COLORARGx/D3DTSS_ALPHAARGx.");
                return D3DERR_INVALIDCALL;
            }

            if ((Value & ~D3DTA_SELECTMASK) >= D3DTA_MODIFIERMAX)
            {
                D3D_ERR("Invalid modifier for D3DTSS_COLORARGx/D3DTSS_ALPHAARGx.");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_ADDRESSU:
        case D3DTSS_ADDRESSV:
        case D3DTSS_ADDRESSW:
            /* D3DTEXTUREADDRESS for U/V/W coordinate */

            if ((Value == 0) || (Value >= D3DTADDRESS_MAX))
            {
                D3D_ERR("Invalid value for D3DTSS_ADRESSx.");
                return D3DERR_INVALIDCALL;
            }
            break;  

        case D3DTSS_MAGFILTER:
        case D3DTSS_MINFILTER:
            if ((Value < D3DTEXF_POINT) || (Value > D3DTEXF_GAUSSIANCUBIC))
            {
                D3D_ERR("Invalid D3DTSS_MAGFILTER/D3DTSS_MINFILTER value.");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_MIPFILTER:
            if (Value > D3DTEXF_LINEAR)
            {
                D3D_ERR("Invalid D3DTSS_MIPFILTER value.");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_MIPMAPLODBIAS:
            /* float Mipmap LOD bias */
            break;

        case D3DTSS_MAXMIPLEVEL:
            /* DWORD 0..(n-1) LOD index of largest map to use (0 == largest) */
            break;

        case D3DTSS_MAXANISOTROPY:
            /* DWORD maximum anisotropy */
            if (Value > 4)
            {
                D3D_ERR("Maximum MAXANISOTROPY is 4");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_BUMPENVMAT00:
        case D3DTSS_BUMPENVMAT01:
        case D3DTSS_BUMPENVMAT10:
        case D3DTSS_BUMPENVMAT11:
            if ((Floatify(Value) < F_MINUS_8) || (Floatify(Value) > 8.0f))
            {
                D3D_ERR("Must be in the range [-8.0, 8.0]");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_BUMPENVLSCALE:
            /* float scale for bump map luminance */
            break;

        case D3DTSS_BUMPENVLOFFSET:
            /* float offset for bump map luminance */
            break;

        case D3DTSS_TEXTURETRANSFORMFLAGS:
            /* D3DTEXTURETRANSFORMFLAGS controls texture transform */

            if ((Value & 0xFF) >= 5)
            {
                D3D_ERR("Invalid coordinate count for D3DTSS_TEXTURETRANSFORMFLAGS.");
                return D3DERR_INVALIDCALL;
            }

            if (Value & ~0x1FF)
            {
                D3D_ERR("Invalid flags for D3DTSS_TEXTURETRANSFORMFLAGS.");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_COLORKEYOP:
            // DOC: This new texture stage state

            if (Value >= D3DTCOLORKEYOP_MAX)
            {
                D3D_ERR("Invalid D3DTSS_COLORKEYOP value.");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_COLORSIGN:
            // DOC: This new texture stage state

            if (Value & ~(DRF_DEF(097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_ENABLED)
                        | DRF_DEF(097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_ENABLED)
                        | DRF_DEF(097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_ENABLED)
                        | DRF_DEF(097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_ENABLED)))
            {
                D3D_ERR("Invalid D3DTSS_COLORSIGN value.");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_ALPHAKILL:
            // DOC: This new texture stage state

            if ((Value != D3DTALPHAKILL_ENABLE) && (Value != D3DTALPHAKILL_DISABLE))
            {
                D3D_ERR("Invalid D3DTSS_ALPHAKILL value.");
                return D3DERR_INVALIDCALL;
            }
            break;

        case D3DTSS_TEXCOORDINDEX:
            if ((Value & 0xffff) > D3DTSS_TCI_TEXCOORDINDEX_MAX)
            {
                D3D_ERR("Texture coordinate index is too big");
                return D3DERR_INVALIDCALL;
            }

            if ((Value & 0xffff0000) > D3DTSS_TCI_TEXGEN_MAX)
            {
                D3D_ERR("Invalid texgen");
                return D3DERR_INVALIDCALL;
            }
            break;
        }
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_SetTextureStageStateNotInline

extern "C"
void WINAPI D3DDevice_SetTextureStageStateNotInline(
    DWORD Stage,
    D3DTEXTURESTAGESTATETYPE Type,
    DWORD Value) 
{ 
    COUNT_API(API_D3DDEVICE_SETTEXTURESTAGESTATENOTINLINE);

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(D3DDevice_SetTextureState_ParameterCheck(Stage, Type, Value) 
                  != S_OK))
        return;

    COUNT_TEXTURESTATE(Type);
    if (Value == D3D__TextureState[Stage][Type])
    {
        COUNT_PERF(PERF_REDUNDANT_SETTEXTURESTATE);
    }

    if (Type < D3DTSS_DEFERRED_TEXTURE_STATE_MAX)
    {
        D3D__DirtyFlags |= (D3DDIRTYFLAG_TEXTURE_STATE_0 << Stage);
        D3D__TextureState[Stage][Type] = Value;
    }
    else if (Type == D3DTSS_COLOROP)
    {
        // Only D3DTOP_BUMPENVMAP and D3DTOP_BUMPENVMAPLUMINANCE require the
        // sign flags and shader program to be recomputed:

        D3D__DirtyFlags |= (Value < D3DTOP_BUMPENVMAP)
                         ? (D3DDIRTYFLAG_COMBINERS)
                         : (D3DDIRTYFLAG_COMBINERS | 
                            D3DDIRTYFLAG_SHADER_STAGE_PROGRAM | 
                            D3DDIRTYFLAG_TEXTURE_STATE);

        ASSERT((Value < D3DTOP_BUMPENVMAP) || 
               (Value == D3DTOP_BUMPENVMAP) || 
               (Value == D3DTOP_BUMPENVMAPLUMINANCE));

        D3D__TextureState[Stage][Type] = Value;
    }
    else if (Type < D3DTSS_DEFERRED_MAX)
    {
        D3D__DirtyFlags |= D3DDIRTYFROMTEXTURESTATE[Type];
        D3D__TextureState[Stage][Type] = Value;
    }
    else if (Type == D3DTSS_TEXCOORDINDEX)
    {
        D3DDevice_SetTextureState_TexCoordIndex(Stage, Value);
    }
    else if (Type == D3DTSS_BORDERCOLOR)
    {
        D3DDevice_SetTextureState_BorderColor(Stage, Value);
    }
    else if (Type == D3DTSS_COLORKEYCOLOR)
    {
        D3DDevice_SetTextureState_ColorKeyColor(Stage, Value);
    }
    else if ((Type >= D3DTSS_BUMPENVMAT00) && (Type <= D3DTSS_BUMPENVLOFFSET))
    {
        D3DDevice_SetTextureState_BumpEnv(Stage, Type, Value);
    }
}

//------------------------------------------------------------------------------
// CommonSetTextureBumpEnv
//
// For the bump-env registers, what stages map to what texture units changes
// depending on whether a pixel shader is active or not.
//
// Rather than do extra work in LazySetTextureState or other places, we 
// just always reprogram all the bump-env registers when switching in or
// out of a pixel shader.

PPUSH FASTCALL CommonSetTextureBumpEnv(
    CDevice* pDevice,
    PPUSH pPush)
{
    // D3D uses the i'th texture stage to express the bump map data, where
    // the environment is in the next stage.  NV20 uses the i+1'th stage,
    // so have to add one to the D3D stage when programming the hardware.
    //
    // But when a pixel shader is active, it maps directly.

    DWORD stage = (pDevice->m_pPixelShader == NULL) ? 0 : 1;

    for (DWORD unit = 1; unit <= 3; unit++)
    {
        PushCount(pPush, NV097_SET_TEXTURE_SET_BUMP_ENV_MAT00(unit), 6);

        // Also hit NV097_SET_TEXTURE_SET_BUMP_ENV_OFFSET and
        // NV097_SET_TEXTURE_SET_BUMP_ENV_SCALE here:

        memcpy((void*) (pPush + 1),
               &D3D__TextureState[stage][D3DTSS_BUMPENVMAT00], 
               6 * sizeof(DWORD));

        pPush += 7;
        stage++;
    }

    return pPush;
}

//------------------------------------------------------------------------------
// CommonSetViewport
//
// Sets the NV097_SET_VIEWPORT_OFFSET, NV097_SET_VIEWPORT_SCALE, and 
// NV097_SET_CLIP_MIN registers as appropriate for the current state.

PPUSH FASTCALL CommonSetViewport(
    CDevice* pDevice,
    PPUSH pPush)
{
    FLOAT clipNear;
    FLOAT clipFar;

    FLOAT xViewport = pDevice->m_Viewport.X * pDevice->m_SuperSampleScaleX
                    + pDevice->m_ScreenSpaceOffsetX;
    FLOAT yViewport = pDevice->m_Viewport.Y * pDevice->m_SuperSampleScaleY
                    + pDevice->m_ScreenSpaceOffsetY;

    // Adjust for the implicit 0.5 offset that the hardware multisampling 
    // applies:

    if ((pDevice->m_StateFlags & STATE_MULTISAMPLING) &&
        (D3D__RenderState[D3DRS_MULTISAMPLEANTIALIAS]))
    {
        xViewport -= 0.5f;
        yViewport -= 0.5f;
    }
    
    if (pDevice->m_pVertexShader->Flags & (VERTEXSHADER_PASSTHROUGH |
                                           VERTEXSHADER_PROGRAM))
    {
        // Programmable vertex shader case.  Set the magic '-38' and 
        // '-37' constant registers:

        if (!(pDevice->m_StateFlags & STATE_NORESERVEDCONSTANTS))
        {
            FLOAT fm11 = 0.5f * pDevice->m_Viewport.Width 
                              * pDevice->m_SuperSampleScaleX;
            FLOAT fm41 = fm11;
        
            FLOAT fm22 = -0.5f * pDevice->m_Viewport.Height 
                               * pDevice->m_SuperSampleScaleY;
            FLOAT fm42 = -fm22;
        
            FLOAT fm33 = pDevice->m_ZScale * (pDevice->m_Viewport.MaxZ - 
                                              pDevice->m_Viewport.MinZ);
            FLOAT fm43 = pDevice->m_ZScale * (pDevice->m_Viewport.MinZ);
        
            Push4f(pPush, 
                   NV097_SET_VIEWPORT_OFFSET(0), 
                   fm41 + xViewport, 
                   fm42 + yViewport, 
                   fm43, 
                   0.0f);
        
            Push4f(pPush + 5,
                   NV097_SET_VIEWPORT_SCALE(0), 
                   fm11, 
                   fm22, 
                   fm33, 
                   0.0f);

            pPush += 10;
        }

        clipNear = 0.0f;
        clipFar = pDevice->m_ZScale;
    }
    else
    {
        // Fixed-function-pipeline case:

        Push4f(pPush, 
               NV097_SET_VIEWPORT_OFFSET(0), 
               xViewport,
               yViewport,
               0.0f,
               0.0f);
        
        pPush += 5;

        if (D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
        {
            clipNear = pDevice->m_ZScale * pDevice->m_WNear * pDevice->m_InverseWFar;
            clipFar = pDevice->m_ZScale;
        }
        else
        {
            clipNear = pDevice->m_ZScale * pDevice->m_Viewport.MinZ;
            clipFar = pDevice->m_ZScale * pDevice->m_Viewport.MaxZ;
        }
    }

    Push2f(pPush, NV097_SET_CLIP_MIN, clipNear, clipFar);

    return pPush + 3;
}

//------------------------------------------------------------------------------
// CommonSetControl0
//
// Sets the NV097_SETCONTROL0 register.
//
// Dependencies:
//
//      D3DRS_ZENABLE
//      D3DRS_YUVENABLE

PPUSH FASTCALL CommonSetControl0(
    CDevice* pDevice,
    PPUSH pPush)
{
    DWORD control0 = DRF_DEF(097, _SET_CONTROL0, _TEXTUREPERSPECTIVE, _TRUE)
                   | DRF_DEF(097, _SET_CONTROL0, _STENCIL_WRITE_ENABLE, _TRUE);

    if (D3D__RenderState[D3DRS_YUVENABLE])
    {
        control0 |= DRF_DEF(097, _SET_CONTROL0, _COLOR_SPACE_CONVERT, _CRYCB_TO_RGB);
    }

    if (D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
    {
        control0 |= DRF_DEF(097, _SET_CONTROL0, _Z_PERSPECTIVE_ENABLE, _TRUE);
    }

    if (pDevice->m_pZBuffer)
    {
        D3DFORMAT Format = PixelJar::GetFormat(pDevice->m_pZBuffer);

        if (Format == D3DFMT_F16
            || Format == D3DFMT_F24S8
            || Format == D3DFMT_LIN_F16
            || Format == D3DFMT_LIN_F24S8)
        {
            control0 |= DRF_DEF(097, _SET_CONTROL0, _Z_FORMAT, _FLOAT);
        }
    }

    Push1(pPush, NV097_SET_CONTROL0, control0);

    return pPush + 2;
}

//------------------------------------------------------------------------------
// CommonSetAntiAliasingControl

VOID FASTCALL CommonSetAntiAliasingControl(
    CDevice* pDevice)
{
    // The passthrough program and viewport are dependent upon the
    // D3DRS_MULTISAMPLEANTIALIASING state...

    CommonSetPassthroughProgram(pDevice);

    PPUSH pPush = pDevice->StartPush();

    pPush = CommonSetViewport(pDevice, pPush);

    Push1(pPush, 
          NV097_SET_ANTI_ALIASING_CONTROL, 
          DRF_NUM(097, _SET_ANTI_ALIASING_CONTROL, _ENABLE, 
                  D3D__RenderState[D3DRS_MULTISAMPLEANTIALIAS])
        | DRF_NUM(097, _SET_ANTI_ALIASING_CONTROL, _SAMPLE_MASK, 
                  D3D__RenderState[D3DRS_MULTISAMPLEMASK]));

    pDevice->EndPush(pPush + 2);
}

//------------------------------------------------------------------------------
// CommonSetOccludeZStencilEn                                                           
//                                                                             
// Sets the NV097_SET_OCCLUDE_ZSTENCIL_EN register.                                        
//                                                                             
// Dependencies:                                                               
//                                                                             
//      D3DRS_STENCILFAIL
//      D3DRS_STENCILENABLE
//      D3DRS_OCCLUSIONCULLENABLE
//      D3DRS_STENCILCULLENABLE

PPUSH FASTCALL CommonSetOccludeZStencilEn(
    CDevice* pDevice,
    PPUSH pPush)
{
    DWORD occlude = 0;

    if (D3D__RenderState[D3DRS_STENCILCULLENABLE])
    {
        occlude |= DRF_DEF(097, _SET_OCCLUDE_ZSTENCIL_EN, 
                           _OCCLUDE_STENCIL_EN, _ENABLE);
    }

    if (D3D__RenderState[D3DRS_OCCLUSIONCULLENABLE])
    {
        // Both the NV20 and the NV2A have bugs with Z culling when stencil is
        // on and stencil fails but the mode is not KEEP - they will incorrectly
        // cull out the pixels.
    
        if ((!D3D__RenderState[D3DRS_STENCILENABLE]) || 
            (D3D__RenderState[D3DRS_STENCILFAIL] == D3DSTENCILOP_KEEP))
        {
            occlude |= DRF_DEF(097, _SET_OCCLUDE_ZSTENCIL_EN, _OCCLUDE_ZEN, 
                               _ENABLE);
        }
    }

    Push1(pPush, NV097_SET_OCCLUDE_ZSTENCIL_EN, occlude);

    return pPush + 2;
}

//------------------------------------------------------------------------------
// CommonSetDebugRegisters

VOID FASTCALL CommonSetDebugRegisters()
{
    CDevice* pDevice = g_pDevice;

    // Debug register 5:

    pDevice->m_Miniport.m_DebugRegister[5] 
        &= ~(DRF_NUM(_PGRAPH, _DEBUG_5, _ZCULL_RETURN_COMP, ~0));

    if (D3D__RenderState[D3DRS_DONOTCULLUNCOMPRESSED])
    {
        pDevice->m_Miniport.m_DebugRegister[5]
            |= DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_RETURN_COMP, _ENABLED);
    }

    // Debug register 6:

    pDevice->m_Miniport.m_DebugRegister[6] 
        &= ~(DRF_NUM(_PGRAPH, _DEBUG_6, _ROP_ZCMP_ALWAYS_READ, ~0)
           | DRF_NUM(_PGRAPH, _DEBUG_6, _ROP_ZREAD, ~0));

    if (D3D__RenderState[D3DRS_ROPZCMPALWAYSREAD])
    {
        pDevice->m_Miniport.m_DebugRegister[6] 
            |= DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZCMP_ALWAYS_READ, _ENABLED);
    }
    if (D3D__RenderState[D3DRS_ROPZREAD])
    {
        pDevice->m_Miniport.m_DebugRegister[6] 
            |= DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZREAD, _FORCE_ZREAD);
    }

    // Set the state, synchronized to the push-buffer:

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_WAIT_FOR_IDLE, 0);

    Push2(pPush + 2, NV097_SET_ZSTENCIL_CLEAR_VALUE, 
          NV_PGRAPH_DEBUG_5,
          pDevice->m_Miniport.m_DebugRegister[5]);

    Push1(pPush + 5, NV097_NO_OPERATION,
        NVX_WRITE_REGISTER_VALUE);

    Push2(pPush + 7, NV097_SET_ZSTENCIL_CLEAR_VALUE, 
          NV_PGRAPH_DEBUG_6,
          pDevice->m_Miniport.m_DebugRegister[6]);

    Push1(pPush + 10, NV097_NO_OPERATION,
        NVX_WRITE_REGISTER_VALUE);

    pDevice->EndPush(pPush + 12);
}

//------------------------------------------------------------------------------
// UpdateProjectionViewportTransform
//
// Depedencies:
//
//      D3DRS_ZENABLE
//      m_Viewport
//      D3DTS_PROJECTION
//      antialising

VOID UpdateProjectionViewportTransform()
{
    D3DALIGN D3DMATRIX viewportTransform;
    float fm11, fm22, fm33, fm41, fm42, fm43, fm44;

    CDevice* pDevice = g_pDevice;

    // This is based on clipX = -1.0, clipY = 1.0

    fm11 = 0.5f * pDevice->m_SuperSampleScaleX * pDevice->m_Viewport.Width;
    fm41 = fm11;

    fm22 = -0.5f * pDevice->m_SuperSampleScaleY * pDevice->m_Viewport.Height;
    fm42 = -fm22;

    fm33 = pDevice->m_ZScale * (pDevice->m_Viewport.MaxZ - 
                                pDevice->m_Viewport.MinZ);
    fm43 = pDevice->m_ZScale * pDevice->m_Viewport.MinZ;

    if (D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
    {
        // We are doing w-buffering - scale w to optimize use of buffer:

        fm44 = pDevice->m_ZScale * pDevice->m_InverseWFar;

        // Scale x, y, and z similarly to negate the effect of dividing
        // through by a scaled w:

        fm11 *= fm44;  fm22 *= fm44;  fm33 *= fm44;
        fm41 *= fm44;  fm42 *= fm44;  fm43 *= fm44;
    }
    else
    {
        fm44 = 1.0f;
    }

    ZeroMemory(&viewportTransform, sizeof(viewportTransform));

    viewportTransform._11 = fm11;
    viewportTransform._22 = fm22;
    viewportTransform._33 = fm33;
    viewportTransform._41 = fm41;
    viewportTransform._42 = fm42;
    viewportTransform._43 = fm43;
    viewportTransform._44 = fm44;

    // Update our cached transform:

    MatrixProduct4x4(&pDevice->m_ProjectionViewportTransform,
                     &pDevice->m_Transform[D3DTS_PROJECTION],
                     &viewportTransform);

    // The transform stuff is naturally dependent upon our cached transform:

    D3D__DirtyFlags |= D3DDIRTYFLAG_TRANSFORM;
}

} // end of namespace
