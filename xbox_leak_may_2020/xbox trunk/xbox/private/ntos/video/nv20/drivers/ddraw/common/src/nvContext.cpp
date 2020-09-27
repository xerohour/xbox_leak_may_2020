/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvContext.cpp                                                     *
*   Hardware specific context management routines.                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/97 - reorganized.                 *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"


#if (NVARCH >= 0x04)

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  CDriverContext  ********************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * CDriverContext::create
 *
 * init NVDD32 local context
 */
void CDriverContext::create
(
    void
)
{
    dbgTracePush ("CDriverContext::create");

    dwGlobalStructSize = sizeof(*this);

    // instantiate some objects
    pRefCount    = new CReferenceCount;
    pBlitTracker = new CReferenceCountMap<5>;

    dbgTracePop();
}

/*****************************************************************************
 * CDriverContext::destroy
 *
 * kill NVDD32 local context
 */
void CDriverContext::destroy (void)
{
    dbgTracePush ("CDriverContext::destroy");

    if (pRefCount)    delete pRefCount;
    if (pBlitTracker) delete pBlitTracker;

    dbgTracePop();
}

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  exports  ***************************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/

// set up the default viewport based on the current render target
// (if there is one). note that the caller is responsible for setting
// celsius drity bits as appropriate

BOOL nvSetSurfaceViewport
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvSetSurfaceViewport");

    pContext->surfaceViewport.clipHorizontal.wX = 0;
    pContext->surfaceViewport.clipVertical.wY   = 0;

    CSimpleSurface *pRenderTarget;

#if (NVARCH >= 0x020)
    pRenderTarget = pContext->kelvinAA.GetCurrentRT(pContext);
#else
    pRenderTarget = pContext->pRenderTarget;
#endif // NVARCH == 0x020

    if (pRenderTarget) {
        pContext->surfaceViewport.clipHorizontal.wWidth = (WORD)pRenderTarget->getWidth();
        pContext->surfaceViewport.clipVertical.wHeight  = (WORD)pRenderTarget->getHeight();
    }

    else {
        pContext->surfaceViewport.clipHorizontal.wWidth = 0;
        pContext->surfaceViewport.clipVertical.wHeight  = 0;
    }

    // initialize near/far values so when applications request a W buffer but
    // don't set a W range we don't kill off the Ws in the transform setup
    pContext->surfaceViewport.dvWNear   = 0.0f; // these default values from MS D3D device interface source
    pContext->surfaceViewport.dvWFar    = 1.0f; // which takes a projection matrix and computes near/far
    pContext->surfaceViewport.dvInvWFar = 1.0f; // values and if there is some error it punts and set to 0.0/1.0
                                                // thus our initial values here

    // now calulate the dvRWFar value
    nvCalcRWFar (pContext);

    dbgTracePop();
    return (TRUE);
}

//--------------------------------------------------------------------------

// used on DX6 to refresh the surface info after a flip has swapped the fpVidMem values

void nvRefreshSurfaceInfo (PNVD3DCONTEXT pContext)
{
    dbgTracePush ("nvRefreshSurfaceInfo");

    nvAssert (global.dwDXRuntimeVersion < 0x0700);

#ifdef DEBUG
    if (dbgFrontRender) {
        pContext->pRenderTarget = &(dbgFrontRenderSurface);
    }
    else
#endif
    {
        LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl = (LPDDRAWI_DDRAWSURFACE_LCL)pContext->pRenderTarget->getWrapper()->getHandle();
        pContext->pRenderTarget->setAddress(pDDSLcl->lpGbl->fpVidMem);
    }

    if (pContext->pZetaBuffer) {
        LPDDRAWI_DDRAWSURFACE_LCL pDDSLclZ =  (LPDDRAWI_DDRAWSURFACE_LCL)pContext->pZetaBuffer->getWrapper()->getHandle();
        pContext->pZetaBuffer->setAddress(pDDSLclZ->lpGbl->fpVidMem);
    }

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_SURFACE;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_SURFACE;
#endif
    pDriverData->bDirtyRenderTarget = TRUE;

    dbgTracePop();
}

//-------------------------------------------------------------------------

BOOL nvSetupContext
(
    PNVD3DCONTEXT pContext
)
{
    DWORD i;

    dbgTracePush ("nvSetupContext");

    // set the viewport based on the current render target
    nvSetSurfaceViewport (pContext);

    // Set the default rendering state for the context.
    // The defaults come from the final DX5 D3D DDK Documentation.
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE]      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS]          = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESS]     = D3DTADDRESS_WRAP;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE] = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_WRAPU]              = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_WRAPV]              = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]           = D3DFILL_SOLID;
    pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE]          = D3DSHADE_GOURAUD;
    pContext->dwRenderState[D3DRENDERSTATE_LINEPATTERN]        = 0;
    pContext->dwRenderState[D3DRENDERSTATE_MONOENABLE]         = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_ROP2]               = R2_COPYPEN;
    pContext->dwRenderState[D3DRENDERSTATE_PLANEMASK]          = 0xFFFFFFFF;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]    = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_LASTPIXEL]          = TRUE;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAG]         = D3DFILTER_NEAREST;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMIN]         = D3DFILTER_NEAREST;
    pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]           = D3DBLEND_ONE;
    pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]          = D3DBLEND_ZERO;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND]    = D3DTBLEND_MODULATE;
    pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]           = D3DCULL_CCW;
    pContext->dwRenderState[D3DRENDERSTATE_ZFUNC]              = D3DCMP_LESSEQUAL;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHAREF]           = 0;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHAFUNC]          = D3DCMP_ALWAYS;
    pContext->dwRenderState[D3DRENDERSTATE_DITHERENABLE]       = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]   = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]          = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]     = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_ZVISIBLE]           = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_SUBPIXEL]           = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_SUBPIXELX]          = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_STIPPLEDALPHA]      = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_FOGCOLOR]           = 0;
    pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE]       = D3DFOG_NONE;
    pContext->dwRenderState[D3DRENDERSTATE_FOGTABLESTART]      = 1;
    pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEEND]        = 100;
    pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEDENSITY]    = 1;
    pContext->dwRenderState[D3DRENDERSTATE_STIPPLEENABLE]      = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_EDGEANTIALIAS]      = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_COLORKEYENABLE]     = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_BORDERCOLOR]        = 0;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSU]    = D3DTADDRESS_WRAP;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSV]    = D3DTADDRESS_WRAP;
    pContext->dwRenderState[D3DRENDERSTATE_MIPMAPLODBIAS]      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]              = 0;
    pContext->dwRenderState[D3DRENDERSTATE_RANGEFOGENABLE]     = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_ANISOTROPY]         = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_FLUSHBATCH]         = FALSE;

    // Enable the Z-Buffer if there is one attached.
    if (pContext->pZetaBuffer) {
        pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]      = TRUE;
        pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] = TRUE;
    }
    else {
        pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]      = FALSE;
        pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] = FALSE;
    }

    pContext->dwRenderState[D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT] = TRUE;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]              = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL]                = D3DSTENCILOP_KEEP;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL]               = D3DSTENCILOP_KEEP;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]                = D3DSTENCILOP_KEEP;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC]                = D3DCMP_ALWAYS;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILREF]                 = 0;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILMASK]                = 0xFFFFFFFF;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK]           = 0xFFFFFFFF;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREFACTOR]              = 0xFFFFFFFF;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP0]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP1]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP2]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP3]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP4]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP5]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP6]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP7]                      = 0;

    // initialize transform and lighting state
    // BUGBUG i don't really know what the defaults are. set to 0 for now
    pContext->dwRenderState[D3DRENDERSTATE_CLIPPING]                   = 0;
    pContext->dwRenderState[D3DRENDERSTATE_LIGHTING]                   = 0;
    pContext->dwRenderState[D3DRENDERSTATE_EXTENTS]                    = 0;
    pContext->dwRenderState[D3DRENDERSTATE_AMBIENT]                    = 0;
    pContext->dwRenderState[D3DRENDERSTATE_FOGVERTEXMODE]              = D3DFOG_NONE;
    pContext->dwRenderState[D3DRENDERSTATE_COLORVERTEX]                = 0;
    pContext->dwRenderState[D3DRENDERSTATE_LOCALVIEWER]                = 0;
    pContext->dwRenderState[D3DRENDERSTATE_NORMALIZENORMALS]           = 0;
    pContext->dwRenderState[D3DRENDERSTATE_COLORKEYBLENDENABLE]        = 0;
    pContext->dwRenderState[D3DRENDERSTATE_DIFFUSEMATERIALSOURCE]      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_SPECULARMATERIALSOURCE]     = 0;
    pContext->dwRenderState[D3DRENDERSTATE_AMBIENTMATERIALSOURCE]      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_EMISSIVEMATERIALSOURCE]     = 0;

    // DX8 additions
    pContext->dwRenderState[D3DRS_POINTSIZE_MIN]                       = FP_ONE_BITS;
    pContext->dwRenderState[D3DRS_POINTSIZE_MAX]                       = FP_ONE_BITS;
    pContext->dwRenderState[D3DRS_POINTSIZE]                           = FP_ONE_BITS;
    pContext->dwRenderState[D3DRS_POINTSCALE_A]                        = FP_ONE_BITS;
    pContext->dwRenderState[D3DRS_POINTSCALE_B]                        = 0;
    pContext->dwRenderState[D3DRS_POINTSCALE_C]                        = 0;
    pContext->dwRenderState[D3DRS_COLORWRITEENABLE]                    = (D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED |
                                                                          D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);

    // Initialize the texture stages.  Stage 1-7 are all defaulted to disabled.
    // Stage 0 is defaulted for no texturing.
    DWORD dwLODBias0 = nvTranslateLODBias (0);

    for (i = 0; i < 8; i++) {
        pContext->tssState[i].dwValue[D3DTSS_TEXTUREMAP]      = 0;
        pContext->tssState[i].dwValue[D3DTSS_ADDRESSU]        = D3DTADDRESS_WRAP;
        pContext->tssState[i].dwValue[D3DTSS_ADDRESSV]        = D3DTADDRESS_WRAP;
        pContext->tssState[i].dwValue[D3DTSS_MAGFILTER]       = D3DTFG_POINT;
        pContext->tssState[i].dwValue[D3DTSS_MINFILTER]       = D3DTFN_POINT;
        pContext->tssState[i].dwValue[D3DTSS_MIPFILTER]       = D3DTFP_POINT;
        pContext->tssState[i].dwValue[D3DTSS_COLOROP]         = D3DTOP_DISABLE;
        pContext->tssState[i].dwValue[D3DTSS_COLORARG1]       = D3DTA_TEXTURE;
        pContext->tssState[i].dwValue[D3DTSS_COLORARG2]       = D3DTA_CURRENT;
        pContext->tssState[i].dwValue[D3DTSS_ALPHAOP]         = D3DTOP_DISABLE;
        pContext->tssState[i].dwValue[D3DTSS_ALPHAARG1]       = D3DTA_TEXTURE;
        pContext->tssState[i].dwValue[D3DTSS_ALPHAARG2]       = D3DTA_CURRENT;
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
    }

    // a couple things that are different for stage 0
    pContext->tssState[0].dwValue[D3DTSS_COLOROP] = D3DTOP_MODULATE;
    pContext->tssState[0].dwValue[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG1;

    pContext->dwStageCount       = 1;
    pContext->bUseDX6Class       = FALSE;
    pContext->bUseTBlendSettings = FALSE;

    pContext->bStencilEnabled = FALSE;
    pContext->dwStencilFill   = 0;

    for (i=0;i<NV_CAPS_MAX_STREAMS;i++) {
        pContext->ppDX8Streams[i]=NULL;
    }
    pContext->dwStreamDMACount = 0;

    // Force an initial state load.
    pContext->bStateChange = TRUE;
    NV_FORCE_TRI_SETUP (pContext);

    dbgTracePop();
    return(TRUE);
}

//-------------------------------------------------------------------------

void nvInitTLData (PNVD3DCONTEXT pContext)
{
    pContext->pLightArray = NULL;
    pContext->dwLightArraySize = 0;

    // initialize the texture transform matrices and flags
    for (DWORD i=0; i<8; i++) {
        nvMemCopy(&pContext->tssState[i].mTexTransformMatrix,
                  &matrixIdent, sizeof(D3DMATRIX));
        pContext->tssState[i].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS] = D3DTTFF_DISABLE;
    }

    // clear all clip planes to 0. MS says this is the default.
    memset (pContext->ppClipPlane, 0, D3DMAXUSERCLIPPLANES * 4 * sizeof(D3DVALUE));
}

//-------------------------------------------------------------------------

#if (NVARCH >= 0x010)

void nvInitCommonData (PNVD3DCONTEXT pContext)
{
    // init common hardware state
    pContext->hwState.dwDirtyFlags                          = CELSIUS_DIRTY_REALLY_FILTHY | KELVIN_DIRTY_REALLY_FILTHY;
    pContext->hwState.dwStateFlags                          = 0;
    pContext->hwState.dwNumActiveCombinerStages             = 0;
    pContext->hwState.dwNextAvailableTextureUnit            = 0;
    pContext->hwState.dwFogSource                           = 0;
    pContext->hwState.dvZScale                              = 1.0f;
    pContext->hwState.dvInvZScale                           = 1.0f;

    pContext->hwState.alphacull_mode                        = 0;

    pContext->hwState.SuperTri.SuperTriInit(pContext);
    pContext->hwState.SuperTri.setStrategy();
}

//-------------------------------------------------------------------------

void nvInitCelsiusData (PNVD3DCONTEXT pContext)
{
    // init common hardware state
    nvInitCommonData (pContext);

    // initialize celsius state
    for (DWORD i = 0; i < CELSIUS_NUM_TEXTURES; i++) {
        pContext->hwState.dwTexUnitToTexStageMapping[i]     = CELSIUS_UNUSED;
        pContext->hwState.dwTextureUnitStatus       [i]     = CELSIUS_TEXUNITSTATUS_IDLE;
        pContext->hwState.pTextureInUse             [i]     = NULL;
    }

    for (i = 0; i < CELSIUS_NUM_COMBINERS; i++) {
        pContext->hwState.dwColorICW[i] = 0;
        pContext->hwState.dwColorOCW[i] = 0;
        pContext->hwState.dwAlphaICW[i] = 0;
        pContext->hwState.dwAlphaOCW[i] = 0;
        pContext->hwState.specfog_cw[i] = 0;
    }

    pContext->hwState.celsius.dwZEnable                     = D3DZB_FALSE;
    pContext->hwState.celsius.pfnVertexCopy                 = NULL;

    // set up some commonly used programs that allow us to pack 2 D3D stages into one combiner stage

    // list of two-stage modes that cannot be compacted:
    // (so you don't have to bother thinking about them again... :)  )
    //      TSS Mask 0      TSS Mask 1      interpretation
    //      ----------      ----------      --------------
    //      0x90020820      0xb0220820      tex*dif,dif                  | a_dif*cur+(1-a_dif)*tex,cur
    //      0x90400400      0x90410400      tex*dif,disable              | tex*cur,disable
    //      0x90400400      0x90411040      tex*dif,disable              | tex*cur,tex*dif
    //      0x90400400      0x94410400
    //      0x90400400      0xb4410400
    //      0x90400840      0x90410400      tex*dif,tex                  | tex*cur,disable
    //      0x90400840      0xb4410c01
    //      0x90401040      0x90411040      tex*dif,tex*dif              | tex*cur,tex*dif
    //      0x90401040      0x94411c41      tex*dif,tex*dif              | 2*tex*cur,tex+cur
    //      0x90401043      0xc0411c41      tex*dif,tex*fac              | a_cur*tex+(1-a_cur)*cur,tex+cur
    //      0x90400c00      0x10410840      tex*dif,dif                  | [tex]*cur,tex
    //      0x90410400      0x90411040      tex*cur,disable              | tex*cur,tex*dif
    //      0x90410840      0x90410400      tex*cur,tex                  | tex*cur,disable
    //      0x90620840      0x90220820      fac*tex,tex                  | cur*tex,cur
    //      0xb8403840      0x9c200c00      a_fac*tex+(1-a_fac)*dif,same | dif+cur,diffuse
    //      0xb8403840      0x9c201c20      a_fac*tex+(1-a_fac)*dif,same | dif+cur,dif+cur
    //      0xe0440840      0x90410400      tex.spec,tex                 | tex*cur,disable

    // list of two-stage modes that COULD be compacted, but are so idiotic
    // that it's not worth wasting the space to store them
    //      TSS Mask 0      TSS Mask 1      interpretation
    //      ----------      ----------      --------------
    //      0x10401040      0x10411041      [tex]*dif,[tex]*dif          | [tex]*cur,[tex]*cur
    //      0x10401040      0x34410400      [tex]*dif,[tex]*dif          | [a_tex]*[tex]+(1-[a_tex])*cur,disable

    // note: in the above, [tex] denotes a reference to a texture that doesn't exist

    // some common 15-bit masks for reference (16th bit is X):
    // 0x0400 = disabled
    // 0x0820 = select current
    // 0x0840 = select texture
    // 0x0c00 = select diffuse
    // 0x0c01 = select current
    // 0x1002 = modulate diffuse / texture
    // 0x1022 = modulate current / texture
    // 0x1040 = modulate texture / diffuse
    // 0x1041 = modulate texture / current
    // 0x1043 = modulate texture / factor
    // 0x1062 = modulate factor / texture
    // 0x1440 = modulate2x texture / diffuse
    // 0x1441 = modulate2x texture / current
    // 0x1c20 = add current / diffuse
    // 0x1c41 = add texture / current
    // 0x2141 = addsigned texture(complement) / current
    // 0x3022 = blend_diffuse_alpha current / texture = a_dif*cur + (1-a_dif)*tex
    // 0x3441 = blend_texture_alpha texture / current
    // 0x3840 = blend_factor_alpha texture / diffuse

    // for relevant D3D definitions, link to D3DTEXTUREOP

    PCELSIUSCOMBINERPROGRAM pProgram;

    // color:  tex0 * tex1
    // alpha:  diffuse
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x88400400;
    pProgram->dwTSSMask1 = 0x90410400;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x08090000;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x14200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  2 * tex0 * tex1
    // alpha:  diffuse
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x88400400;
    pProgram->dwTSSMask1 = 0x94410400;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x08090000;
    pProgram->dwColorOCW[0] = 0x00010c00;
    pProgram->dwAlphaICW[0] = 0x14200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  tex0 + tex1
    // alpha:  diffuse
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x88400400;
    pProgram->dwTSSMask1 = 0x9c410400;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x08200920;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x14200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  alpha_tex1 * tex1 + (1-alpha_tex1) * tex0
    // alpha:  diffuse
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x88400400;
    pProgram->dwTSSMask1 = 0xb4410400;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x19093908;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x14200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  tex0 * tex1
    // alpha:  diffuse
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x88400840;
    pProgram->dwTSSMask1 = 0x90410c00;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x08090000;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x14200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  tex0 * tex1
    // alpha:  tex0
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x88400840;
    pProgram->dwTSSMask1 = 0x90410c01;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x08090000;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x18200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  tex1
    // alpha:  dif*tex0
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x88401002;
    pProgram->dwTSSMask1 = 0x88400820;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x09200000;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x14180000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  tex1 * diffuse
    // alpha:  tex0
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x90400840;
    pProgram->dwTSSMask1 = 0x90400400;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x09040000;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x18200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  (tex0 * diffuse) + tex1
    // alpha:  diffuse
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x90400400;
    pProgram->dwTSSMask1 = 0x9c410400;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x08040920;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x14200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  (tex0 * diffuse) + tex1
    // alpha:  tex0
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x90400840;
    pProgram->dwTSSMask1 = 0x9c410c01;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = 1;
    pProgram->dwColorICW[0] = 0x08040920;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x18200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  tex0 * diffuse
    // alpha:  tex0
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x90410840;
    pProgram->dwTSSMask1 = 0x10410400;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = CELSIUS_UNUSED;
    pProgram->dwColorICW[0] = 0x08040000;
    pProgram->dwColorOCW[0] = 0x00000c00;
    pProgram->dwAlphaICW[0] = 0x18200000;
    pProgram->dwAlphaOCW[0] = 0x00000c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    // color:  2 * tex0 * diffuse
    // alpha:  tex0 + (1-tex1) - 0.5
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));
    nvAssert (pProgram);
    pProgram->dwTSSMask0 = 0x90410840;
    pProgram->dwTSSMask1 = 0x10410400;
    pProgram->dwStateFlags        = 0;
    pProgram->dwNumActiveStages   = 1;
    pProgram->dwTexUnitMapping[0] = 0;
    pProgram->dwTexUnitMapping[1] = CELSIUS_UNUSED;
    pProgram->dwColorICW[0] = 0x08040000;
    pProgram->dwColorOCW[0] = 0x00010c00;
    pProgram->dwAlphaICW[0] = 0x20392018;
    pProgram->dwAlphaOCW[0] = 0x00008c00;
    pProgram->dwColorICW[1] = 0x00000000;
    pProgram->dwColorOCW[1] = 0x10000000;
    pProgram->dwAlphaICW[1] = 0x00000000;
    pProgram->dwAlphaOCW[1] = 0x00000000;
    pProgram->pNext = NULL;
    nvCelsiusCombinersAddProgramToHashTable (pProgram);


}

#endif  // NVARCH >= 0x010

//-------------------------------------------------------------------------

#if (NVARCH >= 0x020)

void nvInitKelvinData (PNVD3DCONTEXT pContext)
{
    // init common hardware state
    nvInitCommonData (pContext);

    for (DWORD i = 0; i < KELVIN_NUM_TEXTURES; i++) {
        pContext->hwState.dwTexUnitToTexStageMapping[i] = KELVIN_UNUSED;
        pContext->hwState.dwTextureUnitStatus       [i] = KELVIN_TEXUNITSTATUS_IDLE;
        pContext->hwState.pTextureInUse             [i] = NULL;
    }

    for (i = 0; i < KELVIN_NUM_COMBINERS; i++) {
        pContext->hwState.dwColorICW[i] = 0;
        pContext->hwState.dwColorOCW[i] = 0;
        pContext->hwState.dwAlphaICW[i] = 0;
        pContext->hwState.dwAlphaOCW[i] = 0;
    }

    pContext->hwState.specfog_cw[0] = 0;
    pContext->hwState.specfog_cw[1] = 0;
}

#endif

//-------------------------------------------------------------------------

// recover D3D after fullscreen DOS box, power saving mode, etc.

void nvD3DRecover (void)
{
    PNVD3DCONTEXT pContext;

    dbgTracePush ("nvD3DRecover");

#if (NVARCH >= 0x010)
    pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    while (pContext) {
        // We need to recreate our AA buffers
        nvCelsiusAADestroy(pContext);
        pContext = pContext->pContextNext;
    }
#endif  // NVARCH >= 0x010

    // Force reset of D3D rendering target and z-buffer.
    pDriverData->bDirtyRenderTarget = TRUE;
    pDriverData->dwCurrentContextHandle  = 0;

    // force reprogram of celsius/kelvin objects
    getDC()->dwLastHWContext = NULL;

    // clear recovery flag
    pDriverData->dwFullScreenDOSStatus &= ~FSDOSSTATUS_RECOVERYNEEDED_D3D;

    dbgTracePop();
}

//-------------------------------------------------------------------------

// recover ddraw after fullscreen DOS box, power saving mode, etc.

void nvDDrawRecover (void)
{
BOOL isOverlayActive;

    dbgTracePush ("nvDDrawRecover");

    NvNotification *pPioFlipOverlayNotifier = pDriverData->pPioFlipOverlayNotifierFlat;
    pPioFlipOverlayNotifier[1].status = 0;
    pPioFlipOverlayNotifier[2].status = 0;

    LPDDHALINFO pHalInfo = GET_HALINFO();
    pHalInfo->ddCaps.dwCurrVisibleOverlays = 0;

    pDriverData->vpp.dwOverlayOwner = 0;
    pDriverData->vpp.overlayRelaxOwnerCheck = 1;

    isOverlayActive = (pDriverData->vpp.dwOverlayFSOvlHead != 0xFFFFFFFF);
    if (isOverlayActive) {
        VppDestroyFSMirror(&(pDriverData->vpp));
        VppDestroyOverlay(&pDriverData->vpp);
    }

    // clear recovery flag
    pDriverData->dwFullScreenDOSStatus &= ~FSDOSSTATUS_RECOVERYNEEDED_DDRAW;

    if (isOverlayActive) {
        VppCreateOverlay(&pDriverData->vpp);
        VppCreateFSMirror(&pDriverData->vpp, pDriverData->vpp.dwOverlayFSSrcWidth, pDriverData->vpp.dwOverlayFSSrcHeight);
    }

    dbgTracePop();
}

#endif  // NVARCH >= 0x04

