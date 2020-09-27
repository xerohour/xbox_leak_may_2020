/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: D3DDBG.C                                                          *
*   Debug build stuff for D3D.                                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       11/18/97 - re-wrote and cleaned up.     *
*                                                                           *
\***************************************************************************/
#include <math.h>
#include <windows.h>
#include "nvd3ddrv.h"

#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nv3ddbg.h"

/*
 * Define global data valid only when DEBUG is defined.
 */
#ifdef  DEBUG
BOOL    dbgShowState    = 0;
BOOL    dbgFlush        = 0x00000000;   // Flush Nothing
DWORD   dbgLevel        = 0x00000000;   // No Debug output.
#endif  // DEBUG

/*
 * Define global data valid only when DBGFRONTRENDER is defined.
 */
#ifdef  DBGFRONTRENDER
BOOL    dbgFrontRender      = 0;
DWORD   dbgFrontRenderPitch = 1024 * 2;
#endif  // DBGFRONTRENDER

/*
 * Define global data valid only when DBGPREVIEWTEX is defined.
 */
#ifdef  DBGPREVIEWTEX
BOOL    dbgPreviewTextureUseBase = 0;
BOOL    dbgPreviewTexture        = 0;
BOOL    dbgPreviewLinearTexture  = 0;
BOOL    dbgMaskAlpha0            = 0;
DWORD   dbgPreviewTexturePitch   = 1024 * 2;
DWORD   dbgSurfaceDepth          = 16;
BOOL    dbgPreviewTextureClearFirst = TRUE;
DWORD   dbgPreviewTextureMagX = 1;
DWORD   dbgPreviewTextureMagY = 1;
#endif  // DBGPREVIEWTEX

/*
 * Define global data valid only when DMAVALIDATE is defined.
 */
#ifdef  DMAVALIDATE
BOOL    dbgValidatePushBuffer       = 0;
DWORD   dbgLastValidatedPushBuffer  = 0;
#endif  // DMAVALIDATE

#ifdef  DEBUG
/*
 *
 */
char szD3DBoolean[][32] =
{
    "FALSE",
    "TRUE",
};
char szD3DShadeMode[][32] =
{
    "NULL",
    "D3DSHADE_FLAT",
    "D3DSHADE_GOURAUD",
    "D3DSHADE_PHONG"
};
char szD3DFillMode[][32] =
{
    "NULL",
    "D3DFILL_POINT",
    "D3DFILL_WIREFRAME",
    "D3DFILL_SOLID"
};
char szD3DTextureFilter[][32] =
{
    "NULL",
    "D3DFILTER_NEAREST",
    "D3DFILTER_LINEAR",
    "D3DFILTER_MIPNEAREST",
    "D3DFILTER_MIPLINEAR",
    "D3DFILTER_LINEARMIPNEAREST",
    "D3DFILTER_LINEARMIPLINEAR"
};
char szD3DBlend[][32] =
{
    "NULL",
    "D3DBLEND_ZERO",
    "D3DBLEND_ONE",
    "D3DBLEND_SRCCOLOR",
    "D3DBLEND_INVSRCCOLOR",
    "D3DBLEND_SRCALPHA",
    "D3DBLEND_INVSRCALPHA",
    "D3DBLEND_DESTALPHA",
    "D3DBLEND_INVDESTALPHA",
    "D3DBLEND_DESTCOLOR",
    "D3DBLEND_INVDESTCOLOR",
    "D3DBLEND_SRCALPHASAT",
    "D3DBLEND_BOTHSRCALPHA",
    "D3DBLEND_BOTHINVSRCALPHA"
};
char szD3DTextureBlend[][32] =
{
    "NULL",
    "D3DTBLEND_DECAL",
    "D3DTBLEND_MODULATE",
    "D3DTBLEND_DECALALPHA",
    "D3DTBLEND_MODULATEALPHA",
    "D3DTBLEND_DECALMASK",
    "D3DTBLEND_MODULATEMASK",
    "D3DTBLEND_COPY",
    "D3DTBLEND_ADD"
};
char szD3DTextureAddress[][32] =
{
    "NULL",
    "D3DTADDRESS_WRAP",
    "D3DTADDRESS_MIRROR",
    "D3DTADDRESS_CLAMP",
    "D3DTADDRESS_BORDER"
};
char szD3DCull[][32] =
{
    "NULL",
    "D3DCULL_NONE",
    "D3DCULL_CW",
    "D3DCULL_CCW"
};
char szD3DCmpFunc[][32] =
{
    "NULL",
    "D3DCMP_NEVER",
    "D3DCMP_LESS",
    "D3DCMP_EQUAL",
    "D3DCMP_LESSEQUAL",
    "D3DCMP_GREATER",
    "D3DCMP_NOTEQUAL",
    "D3DCMP_GREATEREQUAL",
    "D3DCMP_ALWAYS"
};
char szD3DStencilOp[][32] =
{
    "NULL",
    "D3DSTENCILOP_KEEP",
    "D3DSTENCILOP_ZERO",
    "D3DSTENCILOP_REPLACE",
    "D3DSTENCILOP_INCRSAT",
    "D3DSTENCILOP_DECRSAT",
    "D3DSTENCILOP_INVERT",
    "D3DSTENCILOP_INCR",
    "D3DSTENCILOP_DECR"
};
char szD3DFogMode[][32] =
{
    "NULL",
    "D3DFOG_NONE",
    "D3DFOG_EXP",
    "D3DFOG_EXP2",
    "D3DFOG_LINEAR"
};
char szD3DZBufferType[][32] =
{
    "NULL",
    "D3DZB_FALSE",
    "D3DZB_TRUE",
    "D3DZB_USEW"
};
char szD3DAntiAliasMode[][32] =
{
    "NULL",
    "D3DANTIALIAS_NONE",
    "D3DANTIALIAS_SORTDEPENDENT",
    "D3DANTIALIAS_SORTINDEPENDENT"
};
char szD3DTextureOp[][36] =
{
    "NULL",
    "D3DTOP_DISABLE",
    "D3DTOP_SELECTARG1",
    "D3DTOP_SELECTARG2",
    "D3DTOP_MODULATE",
    "D3DTOP_MODULATE2X",
    "D3DTOP_MODULATE4X",
    "D3DTOP_ADD",
    "D3DTOP_ADDSIGNED",
    "D3DTOP_ADDSIGNED2X",
    "D3DTOP_SUBTRACT",
    "D3DTOP_ADDSMOOTH",
    "D3DTOP_BLENDDIFFUSEALPHA",
    "D3DTOP_BLENDTEXTUREALPHA",
    "D3DTOP_BLENDFACTORALPHA",
    "D3DTOP_BLENDTEXTUREALPHAPM",
    "D3DTOP_BLENDCURRENTALPHA",
    "D3DTOP_PREMODULATE",
    "D3DTOP_MODULATEALPHA_ADDCOLOR",
    "D3DTOP_MODULATECOLOR_ADDALPHA",
    "D3DTOP_MODULATEINVALPHA_ADDCOLOR",
    "D3DTOP_MODULATEINVCOLOR_ADDALPHA",
    "D3DTOP_BUMPENVMAP",
    "D3DTOP_BUMPENVMAPLUMINANCE",
    "D3DTOP_DOTPRODUCT3"
};
char szD3DArg[][64] =
{
    "D3DTA_DIFFUSE",
    "D3DTA_CURRENT",
    "D3DTA_TEXTURE",
    "D3DTA_TFACTOR",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "D3DTA_DIFFUSE | D3DTA_COMPLEMENT",
    "D3DTA_CURRENT | D3DTA_COMPLEMENT",
    "D3DTA_TEXTURE | D3DTA_COMPLEMENT",
    "D3DTA_TFACTOR | D3DTA_COMPLEMENT",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "D3DTA_DIFFUSE | D3DTA_ALPHAREPLICATE",
    "D3DTA_CURRENT | D3DTA_ALPHAREPLICATE",
    "D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE",
    "D3DTA_TFACTOR | D3DTA_ALPHAREPLICATE",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "D3DTA_DIFFUSE | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE",
    "D3DTA_CURRENT | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE",
    "D3DTA_TEXTURE | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE",
    "D3DTA_TFACTOR | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID"
};
char szD3DArgModifier[][32] =
{
    "D3DTA_NORMAL",
    "D3DTA_COMPLEMENT",
    "D3DTA_ALPHAREPLICATE"
};
char szD3DTextureMagFilter[][32] =
{
    "NULL",
    "D3DTFG_POINT",
    "D3DTFG_LINEAR",
    "D3DTFG_FLATCUBIC",
    "D3DTFG_GAUSSIANCUBIC",
    "D3DTFG_ANISOTROPIC"
};
char szD3DTextureMinFilter[][32] =
{
    "NULL",
    "D3DTFN_POINT",
    "D3DTFN_LINEAR",
    "D3DTFN_ANISOTROPIC"
};
char szD3DTextureMipFilter[][32] =
{
    "NULL",
    "D3DTFP_POINT",
    "D3DTFP_LINEAR"
};
#endif  // DEBUG


#ifdef  DEBUG
#define START_STR       "NVD3D: "
#define END_STR         "\r\n"
#ifndef WINNT
void __cdecl DPF_LEVEL(DWORD dwLevel, LPSTR szFormat, ...)
{
    char str[256];

    if (dbgLevel & dwLevel)
    {
        lstrcpy(str, START_STR);
        wvsprintf(str+lstrlen(str), szFormat, (LPVOID)(&szFormat+1));
        lstrcat(str, END_STR);
        OutputDebugString(str);
    }
}
#else
void __cdecl DPF_LEVEL(DWORD dwLevel, LPSTR szFormat, ...)
{
    va_list ap;

    if (dbgLevel & dwLevel)
    {
        va_start(ap, szFormat);
        EngDebugPrint(START_STR, szFormat, ap);
        EngDebugPrint("", "\n", ap);
        va_end(ap);
    }
}
#endif // #ifdef WINNT

#endif  // DEBUG


/*
 * Debug routines that are valid only when DEBUG is defined.
 */
#ifdef  DEBUG
void dbgD3DError
(
    void
)
{
    DPF("NV D3D ERROR!!!");
    return;
}

void dbgDisplayContextState
(
    DWORD           dwFlags,
    PNVD3DCONTEXT   pContext
)
{
    if (!dbgShowState)
        return;

    if ((dbgShowState & NVDBG_SHOW_RENDER_STATE)
     && (dwFlags & NVDBG_SHOW_RENDER_STATE))
    {
        DPF("D3D:Context Render State:");
        DPF("    TextureHandle      = %08lx", pContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE]);
        DPF("    AntiAlias          = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS]]);
        DPF("    TextureAddress     = %s",    szD3DTextureAddress[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESS]]);
        DPF("    TexturePerspective = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE]]);
        DPF("    WrapU              = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_WRAPU]]);
        DPF("    WrapV              = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_WRAPV]]);
        DPF("    ZEnable            = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]]);
        DPF("    FillMode           = %s",    szD3DFillMode[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]]);
        DPF("    ShadeMode          = %s",    szD3DShadeMode[pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE]]);
        DPF("    LinePattern        = %08lx", pContext->dwRenderState[D3DRENDERSTATE_LINEPATTERN]);
        DPF("    MonoEnable         = %08lx", pContext->dwRenderState[D3DRENDERSTATE_MONOENABLE]);
        DPF("    Rop2               = %08lx", pContext->dwRenderState[D3DRENDERSTATE_ROP2]);
        DPF("    PlaneMask          = %08lx", pContext->dwRenderState[D3DRENDERSTATE_PLANEMASK]);
        DPF("    ZWriteEnable       = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE]]);
        DPF("    AlphaTestEnable    = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]]);
        DPF("    LastPixelEnable    = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_LASTPIXEL]]);
        DPF("    TextureMag         = %s",    szD3DTextureFilter[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAG]]);
        DPF("    TextureMin         = %s",    szD3DTextureFilter[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMIN]]);
        DPF("    SrcBlend           = %s",    szD3DBlend[pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]]);
        DPF("    DstBlend           = %s",    szD3DBlend[pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]]);
        DPF("    TextureMapBlend    = %s",    szD3DTextureBlend[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND]]);
        DPF("    CullMode           = %s",    szD3DCull[pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]]);
        DPF("    ZFunc              = %s",    szD3DCmpFunc[pContext->dwRenderState[D3DRENDERSTATE_ZFUNC]]);
        DPF("    AlphaRef           = %08lx", pContext->dwRenderState[D3DRENDERSTATE_ALPHAREF]);
        DPF("    AlphaFunc          = %s",    szD3DCmpFunc[pContext->dwRenderState[D3DRENDERSTATE_ALPHAFUNC]]);
        DPF("    DitherEnable       = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_DITHERENABLE]]);
        DPF("    AlphaBlendEnable   = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]]);
        DPF("    FogEnable          = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]]);
        DPF("    SpecularEnable     = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]]);
        DPF("    ZVisible           = %08lx", pContext->dwRenderState[D3DRENDERSTATE_ZVISIBLE]);
        DPF("    SubPixel           = %08lx", pContext->dwRenderState[D3DRENDERSTATE_SUBPIXEL]);
        DPF("    SubPixelX          = %08lx", pContext->dwRenderState[D3DRENDERSTATE_SUBPIXELX]);
        DPF("    StippledAlpha      = %08lx", pContext->dwRenderState[D3DRENDERSTATE_STIPPLEDALPHA]);
        DPF("    FogColor           = %08lx", pContext->dwRenderState[D3DRENDERSTATE_FOGCOLOR]);
        DPF("    FogTableMode       = %s",    szD3DFogMode[pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE]]);
        DPF("    FogTableStart      = %08lx", pContext->dwRenderState[D3DRENDERSTATE_FOGTABLESTART]);
        DPF("    FogTableEnd        = %08lx", pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEEND]);
        DPF("    FogTableDensity    = %08lx", pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEDENSITY]);
        DPF("    StippledEnable     = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_STIPPLEENABLE]]);
        DPF("    EdgeAntiAlias      = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_EDGEANTIALIAS]]);
        DPF("    ColorKeyEnable     = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_COLORKEYENABLE]]);
        DPF("    BorderColor        = %08lx", pContext->dwRenderState[D3DRENDERSTATE_BORDERCOLOR]);
        DPF("    TextureAddressU    = %s",    szD3DTextureAddress[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSU]]);
        DPF("    TextureAddressV    = %s",    szD3DTextureAddress[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSV]]);
        DPF("    MipMapLODBias      = %08lx", pContext->dwRenderState[D3DRENDERSTATE_MIPMAPLODBIAS]);
        DPF("    ZBias              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]);
        DPF("    RangeFogEnable     = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_RANGEFOGENABLE]]);
        DPF("    Anisotropy         = %08lx", pContext->dwRenderState[D3DRENDERSTATE_ANISOTROPY]);
        DPF("    FlushBatch         = %08lx", pContext->dwRenderState[D3DRENDERSTATE_FLUSHBATCH]);
#ifdef  NVD3D_DX6
        DPF("    TranslucentSortInd = %08lx", pContext->dwRenderState[D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT]);
        DPF("    StencilEnable      = %s",    szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]]);
        DPF("    StencilFail        = %s",    szD3DStencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL]]);
        DPF("    StencilZFail       = %s",    szD3DStencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL]]);
        DPF("    StencilPass        = %s",    szD3DStencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]]);
        DPF("    StencilFunc        = %s",    szD3DCmpFunc[pContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC]]);
        DPF("    StencilRef         = %08lx", pContext->dwRenderState[D3DRENDERSTATE_STENCILREF]);
        DPF("    StencilMask        = %08lx", pContext->dwRenderState[D3DRENDERSTATE_STENCILMASK]);
        DPF("    StencilWriteMask   = %08lx", pContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK]);
        DPF("    TextureFactor      = %08lx", pContext->dwRenderState[D3DRENDERSTATE_TEXTUREFACTOR]);
        DPF("    Wrap0              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_WRAP0]);
        DPF("    Wrap1              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_WRAP1]);
        DPF("    Wrap2              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_WRAP2]);
        DPF("    Wrap3              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_WRAP3]);
        DPF("    Wrap4              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_WRAP4]);
        DPF("    Wrap5              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_WRAP5]);
        DPF("    Wrap6              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_WRAP6]);
        DPF("    Wrap7              = %08lx", pContext->dwRenderState[D3DRENDERSTATE_WRAP7]);
#endif  // NVD3D_DX6
        DPF("\r\n");
    }
#ifdef  NVD3D_DX6
    if ((dbgShowState & NVDBG_SHOW_MTS_STATE)
     && (dwFlags & NVDBG_SHOW_MTS_STATE))
    {
        DWORD   dwNumStages;
        DWORD   dwStage;

        dwNumStages = (dbgShowState & NVDBG_SHOW_MTS_STATE) >> NVDBG_SHOW_MTS_STATE_SHIFT;
        dwNumStages = (dwNumStages <= NVDBG_SHOW_MTS_STATE_MAX) ? dwNumStages : NVDBG_SHOW_MTS_STATE_MAX;
        for (dwStage = 0; dwStage < dwNumStages; dwStage++)
        {
            DPF("D3D:Texture Stage %d State:", dwStage);
            DPF("    TextureMap     = %08lx", pContext->tssState[dwStage].dwTextureMap);
            DPF("    AddressU       = %s",    szD3DTextureAddress[pContext->tssState[dwStage].dwAddressU]);
            DPF("    AddressV       = %s",    szD3DTextureAddress[pContext->tssState[dwStage].dwAddressV]);
            DPF("    MagFilter      = %s",    szD3DTextureMagFilter[pContext->tssState[dwStage].dwMagFilter]);
            DPF("    MinFilter      = %s",    szD3DTextureMinFilter[pContext->tssState[dwStage].dwMinFilter]);
            DPF("    MipFilter      = %s",    szD3DTextureMipFilter[pContext->tssState[dwStage].dwMipFilter]);
            DPF("    ColorOp        = %s",    szD3DTextureOp[pContext->tssState[dwStage].dwColorOp]);
            DPF("    ColorArg1      = %s",    szD3DArg[pContext->tssState[dwStage].dwColorArg1]);
            DPF("    ColorArg2      = %s",    szD3DArg[pContext->tssState[dwStage].dwColorArg2]);
            DPF("    AlphaOp        = %s",    szD3DTextureOp[pContext->tssState[dwStage].dwAlphaOp]);
            DPF("    AlphaArg1      = %s",    szD3DArg[pContext->tssState[dwStage].dwAlphaArg1]);
            DPF("    AlphaArg2      = %s",    szD3DArg[pContext->tssState[dwStage].dwAlphaArg2]);
            DPF("    BumpEnvMat00   = %08lx", pContext->tssState[dwStage].dwBumpEnvMat00);
            DPF("    BumpEnvMat01   = %08lx", pContext->tssState[dwStage].dwBumpEnvMat01);
            DPF("    BumpEnvMat10   = %08lx", pContext->tssState[dwStage].dwBumpEnvMat10);
            DPF("    BumpEnvMat11   = %08lx", pContext->tssState[dwStage].dwBumpEnvMat11);
            DPF("    TexCoordIndex  = %08lx", pContext->tssState[dwStage].dwTexCoordIndex);
            DPF("    BorderColor    = %08lx", pContext->tssState[dwStage].dwBorderColor);
            DPF("    MipMapLODBias  = %08lx", pContext->tssState[dwStage].dwMipMapLODBias);
            DPF("    MaxMipMapLevel = %08lx", pContext->tssState[dwStage].dwMaxMipLevel);
            DPF("    MaxAnisotropy  = %08lx", pContext->tssState[dwStage].dwMaxAnisotropy);
            DPF("    BumpEnvlScale  = %08lx", pContext->tssState[dwStage].dwBumpEnvlScale);
            DPF("    BumpEnvlOffset = %08lx", pContext->tssState[dwStage].dwBumpEnvlOffset);
            DPF("\r\n");
        }
    }
#endif  // NVD3D_DX6
    if ((dbgShowState & NVDBG_SHOW_DX5_CLASS_STATE)
     && (dwFlags & NVDBG_SHOW_DX5_CLASS_STATE))
    {
        DPF("D3D:NV4 DX5 Class Context State:");
        DPF("    ColorKey           = %08lx", pContext->ctxInnerLoop.dwColorKey);
        DPF("    TextureOffset      = %08lx", pContext->ctxInnerLoop.dwTextureOffset);
        DPF("    Format             = %08lx", pContext->ctxInnerLoop.dwFormat);
        DPF("    Filter             = %08lx", pContext->ctxInnerLoop.dwFilter);
        DPF("    Blend              = %08lx", pContext->ctxInnerLoop.dwBlend);
        DPF("    Control            = %08lx", pContext->ctxInnerLoop.dwControl);
        DPF("    FogColor           = %08lx", pContext->ctxInnerLoop.dwFogColor);
        DPF("\r\n");
    }
#ifdef  NVD3D_DX6
    if ((dbgShowState & NVDBG_SHOW_DX6_CLASS_STATE)
     && (dwFlags & NVDBG_SHOW_DX6_CLASS_STATE))
    {
        DPF("D3D:NV4 DX6 Class Context State:");
        DPF("    TextureOffset[0]   = %08lx", pContext->mtsState.dwTextureOffset[0]);
        DPF("    TextureOffset[1]   = %08lx", pContext->mtsState.dwTextureOffset[1]);
        DPF("    TextureFormat[0]   = %08lx", pContext->mtsState.dwTextureFormat[0]);
        DPF("    TextureFormat[1]   = %08lx", pContext->mtsState.dwTextureFormat[1]);
        DPF("    TextureFilter[0]   = %08lx", pContext->mtsState.dwTextureFilter[0]);
        DPF("    TextureFilter[1]   = %08lx", pContext->mtsState.dwTextureFilter[1]);
        DPF("    Combine0Alpha      = %08lx", pContext->mtsState.dwCombine0Alpha);
        DPF("    Combine0Color      = %08lx", pContext->mtsState.dwCombine0Color);
        DPF("    Combine1Alpha      = %08lx", pContext->mtsState.dwCombine1Alpha);
        DPF("    Combine1Color      = %08lx", pContext->mtsState.dwCombine1Color);
        DPF("    CombineFactor      = %08lx", pContext->mtsState.dwCombineFactor);
        DPF("    Blend              = %08lx", pContext->mtsState.dwBlend);
        DPF("    Control0           = %08lx", pContext->mtsState.dwControl0);
        DPF("    Control1           = %08lx", pContext->mtsState.dwControl1);
        DPF("    Control2           = %08lx", pContext->mtsState.dwControl2);
        DPF("    FogColor           = %08lx", pContext->mtsState.dwFogColor);
        DPF("\r\n");
    }
#endif  // NVD3D_DX6
    if ((dbgShowState & NVDBG_SHOW_SURFACE_STATE)
     && (dwFlags & NVDBG_SHOW_SURFACE_STATE))
    {
        DPF("D3D:NV4 Surface State:");
        DPF("    ClipStart (x, y)   = %04ld, %04ld", pContext->surfaceViewport.clipHorizontal.wX, pContext->surfaceViewport.clipVertical.wY);
        DPF("    ClipSize (w, h)    = %04ld, %04ld", pContext->surfaceViewport.clipHorizontal.wWidth, pContext->surfaceViewport.clipVertical.wHeight);
        DPF("    SurfaceAddr        = %08lx", pContext->dwSurfaceAddr);
        DPF("    SurfaceOffset      = %08lx", pContext->dwSurfaceOffset);
        DPF("    SurfaceOffset      = %04lx", pContext->surfacePitch.wColorPitch);
        DPF("    SurfaceFormat      = %08lx", pContext->dwSurfaceFormat);
        DPF("    Z-BufferAddr       = %08lx", pContext->ZBufferAddr);
        DPF("    Z-BufferOffset     = %08lx", pContext->ZBufferOffset);
        DPF("    Z-BufferPitch      = %04lx", pContext->surfacePitch.wZetaPitch);
        DPF("\r\n");
    }
    return;
}

void dbgDisplayDrawPrimitives2Info
(
    BYTE bCommand,
    WORD wCommandCount
)
{
    if(!(dbgShowState & NVDBG_SHOW_DRAWPRIM2_INFO))
    {
        return;
    }

    switch(bCommand)
    {

    case D3DDP2OP_POINTS:
        DPF("D3D:D3DDP2OP_POINTS : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_INDEXEDLINELIST:
        DPF("D3DDP2OP_INDEXEDLINELIST : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_INDEXEDTRIANGLELIST:
        DPF("D3DDP2OP_INDEXEDTRIANGLELIST : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_RENDERSTATE:
        DPF("D3DDP2OP_RENDERSTATE : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_LINELIST:
        DPF("D3DDP2OP_LINELIST : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_LINESTRIP:
        DPF("D3DDP2OP_LINESTRIP : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_INDEXEDLINESTRIP:
        DPF("D3DDP2OP_INDEXEDLINESTRIP : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_TRIANGLELIST:
        DPF("D3DDP2OP_TRIANGLELIST : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_TRIANGLESTRIP:
        DPF("D3DDP2OP_TRIANGLESTRIP : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_INDEXEDTRIANGLESTRIP:
        DPF("D3DDP2OP_INDEXEDTRIANGLESTRIP : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_TRIANGLEFAN:
        DPF("D3DDP2OP_TRIANGLEFAN : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_INDEXEDTRIANGLEFAN:
        DPF("D3DDP2OP_INDEXEDTRIANGLEFAN : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_TRIANGLEFAN_IMM:
        DPF("D3DDP2OP_TRIANGLEFAN_IMM : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_LINELIST_IMM:
        DPF("D3DDP2OP_LINELIST_IMM : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_TEXTURESTAGESTATE:
        DPF("D3DDP2OP_TEXTURESTAGESTATE : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_INDEXEDTRIANGLELIST2:
        DPF("D3DDP2OP_INDEXEDTRIANGLELIST2 : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_INDEXEDLINELIST2:
        DPF("D3DDP2OP_INDEXEDLINELIST2 : %08lx", (DWORD)wCommandCount);
        break;
    case D3DDP2OP_VIEWPORTINFO:
        DPF("D3DDP2OP_VIEWPORTINFO : %08lx", (DWORD)wCommandCount);
        break;
    default:
        DPF("D3DDP2OP_UNKNOWN : Command = %08lx", (DWORD)bCommand);
        break;
    }

/*
    switch(bCommand)
    {
        BOOL didIPrintAnything = 0;

    case D3DDP2OP_POINTS:
    case D3DDP2OP_INDEXEDLINELIST:
    case D3DDP2OP_INDEXEDTRIANGLELIST:
    case D3DDP2OP_LINELIST:
    case D3DDP2OP_LINESTRIP:
    case D3DDP2OP_INDEXEDLINESTRIP:
    case D3DDP2OP_TRIANGLELIST:
    case D3DDP2OP_TRIANGLESTRIP:
    case D3DDP2OP_INDEXEDTRIANGLESTRIP:
    case D3DDP2OP_TRIANGLEFAN:
    case D3DDP2OP_INDEXEDTRIANGLEFAN:
    case D3DDP2OP_TRIANGLEFAN_IMM:
    case D3DDP2OP_LINELIST_IMM:
    case D3DDP2OP_INDEXEDTRIANGLELIST2:
    case D3DDP2OP_INDEXEDLINELIST2:

        // profiling -paul
        if(pCurrentContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE] == 1 &&
           pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGENABLE] == 1)
        {
            DPF("*** ALPHA FOG ***");
            didIPrintAnything = 1;
        }
        if(pCurrentContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE] == 1)
        {
            DPF("*** SPECULAR ***");
            didIPrintAnything = 1;
        }
        if(pCurrentContext->tssState[0].dwMipFilter == D3DTFP_LINEAR)
        {
            DPF("*** TRILINEAR MIPMAPPING ***");
            didIPrintAnything = 1;
        }
        if(pCurrentContext->tssState[1].dwColorOp != D3DTOP_DISABLE ||
           pCurrentContext->tssState[1].dwAlphaOp != D3DTOP_DISABLE)
        {

        }
        if(didIPrintAnything)
        {
            DPF(" "); DPF(" ");
        }

        break;
    default:
        break;
    }
*/
}

void dbgDisplayVertexData
(
    DWORD   sx,
    DWORD   sy,
    DWORD   sz,
    DWORD   rhw,
    DWORD   color,
    DWORD   specular,
    DWORD   tu,
    DWORD   tv
)
{
    if (dbgShowState & NVDBG_SHOW_VERTICES)
    {
        float f;
        int i, frac;
        DPF("D3D:NV4 Vertex Data:");
        f = *(float *)&sx;    i = (int)f;      frac = (int)(100.0*(f-i));
        DPF("    X        = %08lx ~= %d.%02d", sx, i, frac);
        f = *(float *)&sy;    i = (int)f;      frac = (int)(100.0*(f-i));
        DPF("    Y        = %08lx ~= %d.%02d", sy, i, frac);
        f = *(float *)&sz;    i = (int)f;      frac = (int)(100.0*(f-i));
        DPF("    Z        = %08lx ~= %d.%02d", sz, i, frac);
        f = *(float *)&rhw;   i = (int)f;      frac = (int)(100.0*(f-i));
        DPF("    RHW      = %08lx ~= %d.%02d", rhw, i, frac);
        DPF("    Color    = %08lx", color);
        DPF("    Specular = %08lx", specular);
        f = *(float *)&tu;    i = (int)f;      frac = (int)(100.0*(f-i));
        DPF("    U        = %08lx ~= %d.%02d", tu, i, frac);
        f = *(float *)&tv;    i = (int)f;      frac = (int)(100.0*(f-i));
        DPF("    V        = %08lx ~= %d.%02d", tv, i, frac);
        DPF("\r\n");
    }
    return;
}

/*
 * Routine to test the push buffers ability to function.
 */
BOOL dbgTestPushBuffer
(
    PNVD3DCONTEXT   pContext
)
{
    return (TRUE);
}
BOOL dbgValidateControlRegisters
(
    PNVD3DCONTEXT   pContext
)
{
    DWORD   drfValue;
    /*
     * Validate the Format register.
     */
    drfValue = DRF_VAL(054, _FORMAT, _CONTEXT_DMA, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_CONTEXT_DMA_A)
     && (drfValue != NV054_FORMAT_CONTEXT_DMA_B))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_CONTEXT_DMA field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _COLORKEYENABLE, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_COLORKEYENABLE_FALSE)
     && (drfValue != NV054_FORMAT_COLORKEYENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_COLORKEYENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _ORIGIN_ZOH, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_ORIGIN_ZOH_CENTER)
     && (drfValue != NV054_FORMAT_ORIGIN_ZOH_CORNER))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT__ORIGIN_ZOH field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _ORIGIN_FOH, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_ORIGIN_FOH_CENTER)
     && (drfValue != NV054_FORMAT_ORIGIN_FOH_CORNER))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT__ORIGIN_FOH field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _COLOR, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_COLOR_LE_Y8)
     && (drfValue != NV054_FORMAT_COLOR_LE_A1R5G5B5)
     && (drfValue != NV054_FORMAT_COLOR_LE_X1R5G5B5)
     && (drfValue != NV054_FORMAT_COLOR_LE_A4R4G4B4)
     && (drfValue != NV054_FORMAT_COLOR_LE_R5G6B5)
     && (drfValue != NV054_FORMAT_COLOR_LE_A8R8G8B8)
     && (drfValue != NV054_FORMAT_COLOR_LE_X8R8G8B8))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_COLOR field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
//    drfValue = DRF_VAL(054, _FORMAT, _MIPMAP_LEVELS, pContext->ctxInnerLoop.dwFormat);
//    if (drfValue )
//    {
//        DPF("dbgValidateControlRegisters: NV054_FORMAT_MIPMAP_LEVELS field invalid 0x%08lx", drfValue);
//        dbgD3DError();
//    }
    drfValue = DRF_VAL(054, _FORMAT, _BASE_SIZE_U, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_BASE_SIZE_U_1)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_2)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_4)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_8)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_16)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_32)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_64)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_128)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_256)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_512)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_1024)
     && (drfValue != NV054_FORMAT_BASE_SIZE_U_2048))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_BASE_SIZE_U field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _BASE_SIZE_V, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_BASE_SIZE_V_1)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_2)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_4)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_8)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_16)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_32)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_64)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_128)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_256)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_512)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_1024)
     && (drfValue != NV054_FORMAT_BASE_SIZE_V_2048))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_BASE_SIZE_V field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _TEXTUREADDRESSU, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_TEXTUREADDRESSU_WRAP)
     && (drfValue != NV054_FORMAT_TEXTUREADDRESSU_MIRROR)
     && (drfValue != NV054_FORMAT_TEXTUREADDRESSU_CLAMP)
     && (drfValue != NV054_FORMAT_TEXTUREADDRESSU_BORDER))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_TEXTUREADDRESSU field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _WRAPU, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_WRAPU_FALSE)
     && (drfValue != NV054_FORMAT_WRAPU_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_WRAPU field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _TEXTUREADDRESSV, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_TEXTUREADDRESSV_WRAP)
     && (drfValue != NV054_FORMAT_TEXTUREADDRESSV_MIRROR)
     && (drfValue != NV054_FORMAT_TEXTUREADDRESSV_CLAMP)
     && (drfValue != NV054_FORMAT_TEXTUREADDRESSV_BORDER))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_TEXTUREADDRESSV field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _WRAPV, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_WRAPV_FALSE)
     && (drfValue != NV054_FORMAT_WRAPV_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_WRAPV field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }

    /*
     * Validate the Filter register.
     */
    drfValue = DRF_VAL(054, _FILTER, _MIPMAP_DITHER_ENABLE, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_MIPMAP_DITHER_ENABLE_FALSE)
     && (drfValue != NV054_FILTER_MIPMAP_DITHER_ENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_FILTER_MIPMAP_DITHER_ENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FILTER, _TEXTUREMIN, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_TEXTUREMIN_NEAREST)
     && (drfValue != NV054_FILTER_TEXTUREMIN_LINEAR)
     && (drfValue != NV054_FILTER_TEXTUREMIN_MIPNEAREST)
     && (drfValue != NV054_FILTER_TEXTUREMIN_MIPLINEAR)
     && (drfValue != NV054_FILTER_TEXTUREMIN_LINEARMIPNEAREST)
     && (drfValue != NV054_FILTER_TEXTUREMIN_LINEARMIPLINEAR))
    {
        DPF("dbgValidateControlRegisters: NV054_FILTER_TEXTUREMIN field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FILTER, _ANISOTROPIC_MINIFY_ENABLE, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE_FALSE)
     && (drfValue != NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FILTER, _TEXTUREMAG, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_TEXTUREMAG_NEAREST)
     && (drfValue != NV054_FILTER_TEXTUREMAG_LINEAR)
     && (drfValue != NV054_FILTER_TEXTUREMAG_MIPNEAREST)
     && (drfValue != NV054_FILTER_TEXTUREMAG_MIPLINEAR)
     && (drfValue != NV054_FILTER_TEXTUREMAG_LINEARMIPNEAREST)
     && (drfValue != NV054_FILTER_TEXTUREMAG_LINEARMIPLINEAR))
    {
        DPF("dbgValidateControlRegisters: NV054_FILTER_TEXTUREMAG field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FILTER, _ANISOTROPIC_MAGNIFY_ENABLE, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_FALSE)
     && (drfValue != NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }

    /*
     * Validate the Blend register.
     */
    drfValue = DRF_VAL(054, _BLEND, _TEXTUREMAPBLEND, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_TEXTUREMAPBLEND_DECAL)
     && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_MODULATE)
     && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_DECALALPHA)
     && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_MODULATEALPHA)
     && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_DECALMASK)
     && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_MODULATEMASK)
     && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_COPY)
     && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_ADD))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_TEXTUREMAPBLEND field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _MASK_BIT, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_MASK_BIT_LSB)
     && (drfValue != NV054_BLEND_MASK_BIT_MSB))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_MASK_BIT field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _SHADEMODE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_SHADEMODE_FLAT)
     && (drfValue != NV054_BLEND_SHADEMODE_GOURAUD)
     && (drfValue != NV054_BLEND_SHADEMODE_PHONG))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_SHADEMODE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _TEXTUREPERSPECTIVE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_TEXTUREPERSPECTIVE_FALSE)
     && (drfValue != NV054_BLEND_TEXTUREPERSPECTIVE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_TEXTUREPERSPECTIVE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _SPECULARENABLE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_SPECULARENABLE_FALSE)
     && (drfValue != NV054_BLEND_SPECULARENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_SPECULARENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _FOGENABLE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_FOGENABLE_FALSE)
     && (drfValue != NV054_BLEND_FOGENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_FOGENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _ALPHABLENDENABLE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_ALPHABLENDENABLE_FALSE)
     && (drfValue != NV054_BLEND_ALPHABLENDENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_ALPHABLENDENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _SRCBLEND, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_SRCBLEND_ZERO)
     && (drfValue != NV054_BLEND_SRCBLEND_ONE)
     && (drfValue != NV054_BLEND_SRCBLEND_SRCCOLOR)
     && (drfValue != NV054_BLEND_SRCBLEND_INVSRCCOLOR)
     && (drfValue != NV054_BLEND_SRCBLEND_SRCALPHA)
     && (drfValue != NV054_BLEND_SRCBLEND_INVSRCALPHA)
     && (drfValue != NV054_BLEND_SRCBLEND_DESTALPHA)
     && (drfValue != NV054_BLEND_SRCBLEND_INVDESTALPHA)
     && (drfValue != NV054_BLEND_SRCBLEND_DESTCOLOR)
     && (drfValue != NV054_BLEND_SRCBLEND_INVDESTCOLOR)
     && (drfValue != NV054_BLEND_SRCBLEND_SRCALPHASAT))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_SRCBLEND field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _DESTBLEND, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_DESTBLEND_ZERO)
     && (drfValue != NV054_BLEND_DESTBLEND_ONE)
     && (drfValue != NV054_BLEND_DESTBLEND_SRCCOLOR)
     && (drfValue != NV054_BLEND_DESTBLEND_INVSRCCOLOR)
     && (drfValue != NV054_BLEND_DESTBLEND_SRCALPHA)
     && (drfValue != NV054_BLEND_DESTBLEND_INVSRCALPHA)
     && (drfValue != NV054_BLEND_DESTBLEND_DESTALPHA)
     && (drfValue != NV054_BLEND_DESTBLEND_INVDESTALPHA)
     && (drfValue != NV054_BLEND_DESTBLEND_DESTCOLOR)
     && (drfValue != NV054_BLEND_DESTBLEND_INVDESTCOLOR)
     && (drfValue != NV054_BLEND_DESTBLEND_SRCALPHASAT))
    {
        DPF("dbgValidateControlRegisters: NV054_BLEND_DESTBLEND field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }

    /*
     * Validate the Control register.
     */
    drfValue = DRF_VAL(054, _CONTROL, _ALPHAFUNC, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ALPHAFUNC_NEVER)
     && (drfValue != NV054_CONTROL_ALPHAFUNC_LESS)
     && (drfValue != NV054_CONTROL_ALPHAFUNC_EQUAL)
     && (drfValue != NV054_CONTROL_ALPHAFUNC_LESSEQUAL)
     && (drfValue != NV054_CONTROL_ALPHAFUNC_GREATER)
     && (drfValue != NV054_CONTROL_ALPHAFUNC_NOTEQUAL)
     && (drfValue != NV054_CONTROL_ALPHAFUNC_GREATEREQUAL)
     && (drfValue != NV054_CONTROL_ALPHAFUNC_ALWAYS))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ALPHAFUNC field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _ALPHATESTENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ALPHATESTENABLE_FALSE)
     && (drfValue != NV054_CONTROL_ALPHATESTENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ALPHATESTENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _ZENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ZENABLE_FALSE)
     && (drfValue != NV054_CONTROL_ZENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ZENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _ZFUNC, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ZFUNC_NEVER)
     && (drfValue != NV054_CONTROL_ZFUNC_LESS)
     && (drfValue != NV054_CONTROL_ZFUNC_EQUAL)
     && (drfValue != NV054_CONTROL_ZFUNC_LESSEQUAL)
     && (drfValue != NV054_CONTROL_ZFUNC_GREATER)
     && (drfValue != NV054_CONTROL_ZFUNC_NOTEQUAL)
     && (drfValue != NV054_CONTROL_ZFUNC_GREATEREQUAL)
     && (drfValue != NV054_CONTROL_ZFUNC_ALWAYS))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ZFUNC field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _CULLMODE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_CULLMODE_NONE)
     && (drfValue != NV054_CONTROL_CULLMODE_CW)
     && (drfValue != NV054_CONTROL_CULLMODE_CCW))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_CULLMODE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _DITHERENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_DITHERENABLE_FALSE)
     && (drfValue != NV054_CONTROL_DITHERENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_DITHERENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _Z_PERSPECTIVE_ENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_Z_PERSPECTIVE_ENABLE_FALSE)
     && (drfValue != NV054_CONTROL_Z_PERSPECTIVE_ENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_Z_PERSPECTIVE_ENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _ZWRITEENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ZWRITEENABLE_FALSE)
     && (drfValue != NV054_CONTROL_ZWRITEENABLE_TRUE))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ZWRITEENABLE field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _Z_FORMAT, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_Z_FORMAT_FIXED)
     && (drfValue != NV054_CONTROL_Z_FORMAT_FLOAT))
    {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_Z_FORMAT field invalid 0x%08lx", drfValue);
        dbgD3DError();
    }
    return (TRUE);
}
#endif  // DEBUG

#ifdef  DBGPREVIEWTEX
void dbgDisplayTexture
(
    PNVD3DCONTEXT   pContext,
    PNVD3DTEXTURE   pTexture
)
{
    // txtodo - support this
    if (dbgPreviewTexture && pTexture)
    {
        LPDDRAWI_DDRAWSURFACE_LCL   lclDDS   = pTexture->lpLcl;
        LPDDRAWI_DDRAWSURFACE_GBL   gblDDS   = pTexture->lpLcl->lpGbl;
        long                        u, v, u0, v0, uu, vv;
        long                        texWidth  = gblDDS->wWidth;
        long                        texHeight = gblDDS->wHeight;
        long                        texPitch  = gblDDS->lPitch;
        long                        fbPitch;
        long                        magX, magY;
        DWORD                       dwNonZeroAlphaCount = 0;

        if (pContext)
        {
            switch (pContext->dwSurfaceFormat & 0x0000000F)
            {
                case NV053_SET_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5:
                case NV053_SET_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5:
                case NV053_SET_FORMAT_COLOR_LE_R5G6B5:
                    dbgSurfaceDepth = 16;
                    break;
                case NV053_SET_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8:
                case NV053_SET_FORMAT_COLOR_LE_X8R8G8B8_O8R8G8B8:
                case NV053_SET_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8:
                case NV053_SET_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8:
                case NV053_SET_FORMAT_COLOR_LE_A8R8G8B8:
                    dbgSurfaceDepth = 32;
                    break;
            }
        }

        NV_D3D_GLOBAL_SAVE();
        nvFlushDmaBuffers();
        NV_D3D_GLOBAL_SETUP();
        if (dbgSurfaceDepth == 16)
        {
            short                       texelSwizzle;
            unsigned short             *texMapSwizzle;
            unsigned short             *fb, *fb0;

#ifdef NV_TEX2
            if (pTexture->dwTextureFlags & NV4_TEXFLAG_USER_MIP_LEVEL)
                texMapSwizzle = (unsigned short *)TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags);
#else
            if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL)
                texMapSwizzle = (unsigned short *)pTexture->dwTexturePointer;
#endif
            else
            {
#ifdef NV_TEX2
                texMapSwizzle = (unsigned short *)TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags);
#else // !NV_TEX2
                if (pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
                    texMapSwizzle = (unsigned short *)(VIDMEM_ADDR(pDriverData->BaseAddress + pTexture->dwTextureOffset));
                else
                {
                    if (pDriverData->GARTLinearBase > 0)
                        texMapSwizzle = (unsigned short *)pTexture->dwTexturePointer;
                    else
                        texMapSwizzle = (unsigned short *)GetPointerTextureHeap(pTexture->dwTextureOffset);
                }
#endif // !NV_TEX2
            }
            fb = fb0 = (dbgPreviewTextureUseBase) ? (unsigned short *)VIDMEM_ADDR(pDriverData->BaseAddress)
                                                  : (unsigned short *)VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress);
            fbPitch = dbgPreviewTexturePitch;

            // if we can fit it in half the screen width or less,
            // then magnify as requested. otherwise don't
            magX = magY = 1;
            if ((int)(dbgPreviewTextureMagX * texWidth) < (fbPitch >> 2))
                magX = dbgPreviewTextureMagX;
            if ((int)(dbgPreviewTextureMagY * texHeight) < (fbPitch >> 2))
                magY = dbgPreviewTextureMagY;
            if ((magX > 1) || (magY > 1))
                DPF("D3D: DbgDisplayTexture: Texture Preview magnified by %d,%d (x,y)", magX, magY);

            if (dbgPreviewTextureClearFirst)
            {
                short clrColor;
                clrColor = (short)(0xffff * ((double)rand() / RAND_MAX));
                for (v = 0; v < texHeight*magY + 32; v++)
                {
                    for (u = 0; u < texWidth*magX + 32; u++) fb[u] = clrColor;
                    fb += (fbPitch >> 1);
                }
                fb = fb0;
            }

            for (v = 0; v < texHeight; v++)
            {
                V_INTERLEAVE(v0, v, texWidth, texHeight);

                for (vv = 0; vv < magY; vv++)
                {
                    for (u = 0; u < texWidth; u++)
                    {
                        U_INTERLEAVE(u0, u, texWidth, texHeight);
                        texelSwizzle = texMapSwizzle[u0|v0];

                        switch (pTexture->dwTextureColorFormat)
                        {

                        case NV054_FORMAT_COLOR_LE_A1R5G5B5:
                        {
                            if (texelSwizzle & 0x8000)
                                dwNonZeroAlphaCount++;
                            if ((!dbgMaskAlpha0)
                             || (texelSwizzle & 0x8000))
                            {
                                short r, g;
                                r = (texelSwizzle & 0x7C00) << 1;
                                g = (texelSwizzle & 0x03E0) << 1;
                                texelSwizzle = r | g | (texelSwizzle & 0x001F);
                                for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            }
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                        {
                            short r, g;
                            r = (texelSwizzle & 0x7C00) << 1;
                            g = (texelSwizzle & 0x03E0) << 1;
                            texelSwizzle = r | g | (texelSwizzle & 0x001F);
                            for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_A4R4G4B4:
                        {
                            short a, r, g;
                            if (a = texelSwizzle & 0xF000)
                                dwNonZeroAlphaCount++;

                            r = (texelSwizzle & 0x0F00) << 3;
                            g = (texelSwizzle & 0x00F0) << 2;
                            texelSwizzle = ((texelSwizzle & 0x000F) << 1) | r | g;

                            if ((!dbgMaskAlpha0) || (a))
                            {
                                for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            }
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_R5G6B5:
                        {
                            for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_A8R8G8B8:
                        {
                            DWORD   dwTexel;
                            DWORD   a;
                            SHORT   r, g, b;
                            dwTexel = ((DWORD *)texMapSwizzle)[u0|v0];
                            if (a = (dwTexel & 0xFF000000))
                                dwNonZeroAlphaCount++;

                            r = (SHORT)((dwTexel & 0x00FF0000) >> 8) & 0xF800;
                            g = (SHORT)((dwTexel & 0x0000FF00) >> 5) & 0x07E0;
                            b = (SHORT)((dwTexel & 0x000000FF) >> 3) & 0x001F;
                            texelSwizzle = r | g | b;
                            if ((!dbgMaskAlpha0) || (a))
                            {
                                for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            }
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_X8R8G8B8:
                        {
                            DWORD   dwTexel;
                            SHORT   r, g, b;
                            dwTexel = ((DWORD *)texMapSwizzle)[u0|v0];
                            r = (SHORT)((dwTexel & 0x00FF0000) >> 8) & 0xF800;
                            g = (SHORT)((dwTexel & 0x0000FF00) >> 5) & 0x07E0;
                            b = (SHORT)((dwTexel & 0x000000FF) >> 3) & 0x001F;
                            texelSwizzle = r | g | b;
                            for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            break;
                        }

                        default:
                        {
                            for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            break;
                        }

                        }  // switch

                    }  // loop on u

                    fb += (fbPitch >> 1);

                }  // loop on vv
            }  // loop on v
        }
        else
        {
            DWORD   texelSwizzle;
            DWORD  *texMapSwizzle;
            DWORD  *fb, *fb0;

#ifdef NV_TEX2
            if (pTexture->dwTextureFlags & NV4_TEXFLAG_USER_MIP_LEVEL)
                texMapSwizzle = (unsigned long *)TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags);
#else
            if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL)
                texMapSwizzle = (unsigned long *)pTexture->dwTexturePointer;
#endif
            else
            {
#ifdef NV_TEX2
                texMapSwizzle = (unsigned long *)TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags);
#else // !NV_TEX2
                if (pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
                    texMapSwizzle = (unsigned long *)(VIDMEM_ADDR(pDriverData->BaseAddress + pTexture->dwTextureOffset));
                else
                {
                    if (pDriverData->GARTLinearBase > 0)
                        texMapSwizzle = (unsigned long *)pTexture->dwTexturePointer;
                    else
                        texMapSwizzle = (unsigned long *)GetPointerTextureHeap(pTexture->dwTextureOffset);
                }
#endif // !NV_TEX2
            }
            fb = fb0 = (dbgPreviewTextureUseBase) ? (DWORD *)VIDMEM_ADDR(pDriverData->BaseAddress)
                                                  : (DWORD *)VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress);
            fbPitch = dbgPreviewTexturePitch;

            // if we can fit it in half the screen width or less,
            // then magnify as requested. otherwise don't
            magX = magY = 1;
            if ((int)(dbgPreviewTextureMagX * texWidth) < (fbPitch >> 3))
                magX = dbgPreviewTextureMagX;
            if ((int)(dbgPreviewTextureMagY * texHeight) < (fbPitch >> 3))
                magY = dbgPreviewTextureMagY;
            if ((magX > 1) || (magY > 1))
                DPF("D3D: DbgDisplayTexture: Texture Preview magnified by %d,%d (x,y)", magX, magY);

            if (dbgPreviewTextureClearFirst)
            {
                DWORD clrColor;
                clrColor = (DWORD)(0xffffffff * ((float)rand() / RAND_MAX));
                for (v = 0; v < texHeight*magY + 32; v++)
                {
                    for (u = 0; u < texWidth*magX + 32; u++) fb[u] = clrColor;
                    fb += (fbPitch >> 2);
                }
                fb = fb0;
            }

            for (v = 0; v < texHeight; v++)
            {
                V_INTERLEAVE(v0, v, texWidth, texHeight);

                for (vv = 0; vv < magY; vv++)
                {

                    for (u = 0; u < texWidth; u++)
                    {

                        U_INTERLEAVE(u0, u, texWidth, texHeight);

                        switch (pTexture->dwTextureColorFormat)
                        {

                        case NV054_FORMAT_COLOR_LE_A1R5G5B5:
                        {
                            WORD    wTexel;
                            wTexel = ((WORD *)texMapSwizzle)[u0|v0];

                            if (wTexel & 0x8000)
                                dwNonZeroAlphaCount++;
                            if ((!dbgMaskAlpha0)
                             || (wTexel & 0x8000))
                            {
                                texelSwizzle = ((wTexel & 0x7C00) << 9)
                                             | ((wTexel & 0x03E0) << 6)
                                             | ((wTexel & 0x001F) << 3);
                                for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            }
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                        {
                            WORD    wTexel;
                            wTexel = ((WORD *)texMapSwizzle)[u0|v0];
                            texelSwizzle = ((wTexel & 0x7C00) << 9)
                                         | ((wTexel & 0x03E0) << 6)
                                         | ((wTexel & 0x001F) << 3);
                            for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_A4R4G4B4:
                        {
                            short a;
                            WORD    wTexel;
                            wTexel = ((WORD *)texMapSwizzle)[u0|v0];

                            if (a = wTexel & 0xF000)
                                dwNonZeroAlphaCount++;

                            if ((!dbgMaskAlpha0)
                             || (a))
                            {
                                texelSwizzle = ((wTexel & 0x0F00) << 12)
                                             | ((wTexel & 0x00F0) << 8)
                                             | ((wTexel & 0x000F) << 4);
                                for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            }
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_R5G6B5:
                        {
                            WORD    wTexel;
                            wTexel = ((WORD *)texMapSwizzle)[u0|v0];

                            texelSwizzle = ((wTexel & 0xF800) << 8)
                                         | ((wTexel & 0x07E0) << 5)
                                         | ((wTexel & 0x001F) << 3);
                            for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_A8R8G8B8:
                        {
                            DWORD   a;
                            texelSwizzle = texMapSwizzle[u0|v0];
                            if (a = (texelSwizzle & 0xFF000000))
                                dwNonZeroAlphaCount++;

                            if ((!dbgMaskAlpha0)
                             || (a))
                            {
                                for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            }
                            break;
                        }

                        case NV054_FORMAT_COLOR_LE_X8R8G8B8:
                        {
                            texelSwizzle = texMapSwizzle[u0|v0];
                            for (uu=0; uu<magX; uu++) fb[magX*u+uu] = texelSwizzle;
                            break;
                        }

                        }  // switch

                    }  // loop on u

                    fb += (fbPitch >> 2);

                }  // loop on vv
            }  // loop on v

        }  // else (32-bit)

        if ((pTexture->dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A1R5G5B5)
         || (pTexture->dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A4R4G4B4)
         || (pTexture->dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A8R8G8B8))
        {
            if (dwNonZeroAlphaCount)
                DPF("D3D:dbgDisplayTexture - Non Zero Alpha Count = %08lx", dwNonZeroAlphaCount);
            else
                DPF("D3D:dbgDisplayTexture - ALPHA TEXTURE HAS NO ALPHA!!!!");
        }
    }

    return;
}

void dbgDisplayLinearTexture
(
    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl,
    LPDDRAWI_DDRAWSURFACE_LCL   lpLclDst,
    DWORD                       dwTextureFormat
)
{
    if (dbgPreviewLinearTexture && lpLcl)
    {
        LPDDRAWI_DDRAWSURFACE_GBL   lpGbl = lpLcl->lpGbl;
        short                       texelLinear;
        unsigned short             *texMapLinear;
        unsigned short             *fb;
        long                        u, v;
        long                        texWidth  = lpGbl->wWidth;
        long                        texHeight = lpGbl->wHeight;
        long                        texPitch  = lpGbl->lPitch;
        long                        fbPitch;
        DWORD                       dwNonZeroAlphaCount = 0;
        DWORD                       dwSrcBitCount;

        texMapLinear = (unsigned short *)lpGbl->fpVidMem;
        fb           = (dbgPreviewTextureUseBase) ? (unsigned short *)VIDMEM_ADDR(pDriverData->BaseAddress) : (unsigned short *)VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress);
        fbPitch      = dbgPreviewTexturePitch;
        if (lpGbl->ddpfSurface.dwFlags & DDPF_RGB)
            dwSrcBitCount = lpGbl->ddpfSurface.dwRGBBitCount;
        else
            dwSrcBitCount = pDriverData->bi.biBitCount;
#ifndef WINNT // BUGBUG NT5 doesn't appear to support palettized textures
        if (dwSrcBitCount == 8)
        {
            BYTE            bIndex;
            WORD            wColor;
            LPPALETTEENTRY  pPalette;
            unsigned char  *srcTexture;
            if ((!lpLclDst->lpDDPalette)
             || (!lpLclDst->lpDDPalette->lpLcl)
             || (!lpLclDst->lpDDPalette->lpLcl->lpGbl)
             || (!lpLclDst->lpDDPalette->lpLcl->lpGbl->lpColorTable))
            {
                return;
            }
            pPalette = lpLclDst->lpDDPalette->lpLcl->lpGbl->lpColorTable;
            srcTexture = (unsigned char *)texMapLinear;
            for (v = 0; v < texHeight; v++)
            {
                for (u = 0; u < texWidth; u++)
                {
                    bIndex = srcTexture[u];
                    wColor = ((WORD)(pPalette[bIndex].peRed & 0xF8) << 8)
                           | ((WORD)(pPalette[bIndex].peGreen & 0xFC) << 3)
                           | ((WORD)(pPalette[bIndex].peBlue & 0xF8) >> 3);
                    fb[u+264] = wColor;
                }
                srcTexture += texPitch;
                fb         += (fbPitch >> 1);
            }
            return;
        }
#endif // BUGBUG #ifndef WINNT

        for (v = 0; v < texHeight; v++)
        {
            for (u = 0; u < texWidth; u++)
            {
                texelLinear = texMapLinear[u];
                switch (dwTextureFormat)
                {
                    case NV054_FORMAT_COLOR_LE_A1R5G5B5:
                    {
                        if (texelLinear & 0x8000)
                            dwNonZeroAlphaCount++;
                        if ((!dbgMaskAlpha0)
                         || (texelLinear & 0x8000))
                        {
                            short r, g;
                            r = (texelLinear & 0x7C00) << 1;
                            g = (texelLinear & 0x03D0) << 1;
                            texelLinear = r | g | (texelLinear & 0x001F);
                            fb[u+264] = texelLinear;
                        }
                        break;
                    }
                    case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                    {
                        short r, g;
                        r = (texelLinear & 0x7C00) << 1;
                        g = (texelLinear & 0x03D0) << 1;
                        texelLinear = r | g | (texelLinear & 0x001F);
                        fb[u+264] = texelLinear;
                        break;
                    }
                    case NV054_FORMAT_COLOR_LE_A4R4G4B4:
                    {
                        short a, r, g;
                        if (a = texelLinear & 0xF000)
                            dwNonZeroAlphaCount++;

                        r = (texelLinear & 0x0F00) << 3;
                        g = (texelLinear & 0x00F0) << 2;
                        texelLinear = ((texelLinear & 0x000F) << 1) | r | g;

                        if ((!dbgMaskAlpha0)
                         || (a))
                        {
                            fb[u+264] = texelLinear;
                        }
                        break;
                    }
                    case NV054_FORMAT_COLOR_LE_R5G6B5:
                    {
                        fb[u+264] = texelLinear;
                        break;
                    }
                    case NV054_FORMAT_COLOR_LE_A8R8G8B8:
                    {
                        DWORD   dwTexel;
                        DWORD   a;
                        SHORT   r, g, b;
                        dwTexel = ((DWORD *)texMapLinear)[u];
                        if (a = (dwTexel & 0xFF000000))
                            dwNonZeroAlphaCount++;

                        r = (SHORT)((dwTexel & 0x00FF0000) >> 8) & 0xF800;
                        g = (SHORT)((dwTexel & 0x0000FF00) >> 5) & 0x07E0;
                        b = (SHORT)((dwTexel & 0x000000FF) >> 3) & 0x001F;
                        texelLinear = r | g | b;
                        if ((!dbgMaskAlpha0)
                         || (a))
                        {
                            fb[u+264] = texelLinear;
                        }
                        break;
                    }
                    case NV054_FORMAT_COLOR_LE_X8R8G8B8:
                    {
                        DWORD   dwTexel;
                        SHORT   r, g, b;
                        dwTexel = ((DWORD *)texMapLinear)[u];
                        r = (SHORT)((dwTexel & 0x00FF0000) >> 8) & 0xF800;
                        g = (SHORT)((dwTexel & 0x0000FF00) >> 5) & 0x07E0;
                        b = (SHORT)((dwTexel & 0x000000FF) >> 3) & 0x001F;
                        texelLinear = r | g | b;
                        fb[u+264] = texelLinear;
                        break;
                    }
                    default:
                        fb[u+264] = texelLinear;
                        break;

                }
            }
            texMapLinear += (texPitch >> 1);
            fb           += (fbPitch >> 1);
        }
        if ((dwTextureFormat == NV054_FORMAT_COLOR_LE_A1R5G5B5)
         || (dwTextureFormat == NV054_FORMAT_COLOR_LE_A4R4G4B4)
         || (dwTextureFormat == NV054_FORMAT_COLOR_LE_A8R8G8B8))
        {
            if (dwNonZeroAlphaCount)
                DPF("D3D:dbgDisplayTexture - Non Zero Alpha Count = %08lx", dwNonZeroAlphaCount);
            else
                DPF("D3D:dbgDisplayTexture - ALPHA TEXTURE HAS NO ALPHA!!!!");
        }
    }
    return;
}
#endif  // DBGPREVIEWTEX

/*
 * Routines that are valid when DMAVALIDATE is defined.
 */
#ifdef  DMAVALIDATE
void dbgValidatePushBufferError
(
    void
)
{
    DPF("D3D:dbgValidatePushBufferError - Push Buffer Validation Error!!!");
    return;
}
void dbgValidatePushBufferData
(
    void
)
{
    return;
}
#endif  // DMAVALIDATE

