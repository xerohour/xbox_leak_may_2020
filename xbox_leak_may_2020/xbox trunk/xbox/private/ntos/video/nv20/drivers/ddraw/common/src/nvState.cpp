/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvState.cpp                                                       *
*   NV4 state management routines.                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman               04/18/98 - NV4 development.             *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#include "nvdblnd.h"

inline void nvFailMultiTextureSetup( PNVD3DCONTEXT pContext, HRESULT failureCode);

DWORD dwCTTable[] =
{
    0,
    D3DCMP_NEVER,
    D3DCMP_GREATER,
    D3DCMP_EQUAL,
    D3DCMP_GREATEREQUAL,
    D3DCMP_LESS,
    D3DCMP_NOTEQUAL,
    D3DCMP_LESSEQUAL,
    D3DCMP_ALWAYS,
};

DWORD CTFunc
(
    PNVD3DCONTEXT pContext,
    DWORD         dw
)
{
    CNvObject *pNvObj = pContext->pZetaBuffer ? pContext->pZetaBuffer->getWrapper() : NULL;
    if (pNvObj
     && pNvObj->hasCTEnabledNotFront())
    {
        return dwCTTable[dw];
    }
    else
    {
        return dw;
    }
}

//-------------------------------------------------------------------------------------

// convert a D3D LOD bias into the value used by the hardware.

DWORD nvTranslateLODBias (DWORD dwValue)
{
    D3DVALUE dvLODBias;
    DWORD    dwLODBias;
    int      iLODBias;

    dbgTracePush ("nvTranslateLODBias");

    // add biases from the user and registry
    dvLODBias = FLOAT_FROM_DWORD (dwValue) +
                (float (*(int*)&(getDC()->nvD3DRegistryData.regLODBiasAdjust)) / 8.0f);  // registry contributes in 8ths

    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)) {
        // convert to 24.8 twos-complement fixed-point
        iLODBias = int (256.0f*dvLODBias + ((dvLODBias>0) ? 0.5f:-0.5f));
        // clamp to -2^12...2^12-1 (5.8 twos-complement fixed-point)
        iLODBias = max (iLODBias, int(-4096));
        iLODBias = min (iLODBias, int(4095));
        // return the last 13 bits
        dwLODBias = iLODBias & 0x00001fff;
    }

    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_DXTRI) {
        // convert to 29.3 twos-complement fixed-point
        iLODBias = int (8.0f*dvLODBias + ((dvLODBias>0) ? 0.5f:-0.5f));
        // clamp to -2^7...2^7-1 (5.3 twos-complement fixed-point)
        iLODBias = max (iLODBias, int(-128));
        iLODBias = min (iLODBias, int(127));
        // return the last 8 bits
        dwLODBias = iLODBias & 0x000000ff;
    }

    else {
        DPF ("unknown hardware type");
        dbgD3DError();
        dwLODBias = 0;
    }

    dbgTracePop();
    return (dwLODBias);
}

//-------------------------------------------------------------------------------------

/*
 * Send suface setup to the hardware.
 */
void nvSetD3DSurfaceState
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvSetD3DSurfaceState");

    if (pContext->dwRTHandle)
    {
        CNvObject *pRTObj = nvGetObjectFromHandle(pContext->dwDDLclID, pContext->dwRTHandle, &global.pNvSurfaceLists);
        if (pRTObj && pRTObj->isDirty())
        {
            // reset the render target after a mode switch
            nvSetRenderTargetDX7(pContext, pContext->dwRTHandle, pContext->dwZBHandle);
#if (NVARCH >= 0x010)
            // make sure the AA info is also updated
            if (pContext->aa.isEnabled()) nvCelsiusAADestroy(pContext);
#endif  // NVARCH >= 0x010

#if (NVARCH >= 0x020)
            pContext->kelvinAA.Refresh();
#endif  // NVARCH >= 0x020
        }
    }

    /*
     * Only need to update the surface if it has changed since the last render call
     * or the global clip state has changed or zeta buffer changed
     */

    // on DX6, they may have moved pointers around without explicitly telling us.
    // make sure our copies are up-to-date.
    if (global.dwDXRuntimeVersion < 0x700) {
        nvRefreshSurfaceInfo (pContext);
    }

    nvAssert (pContext->pRenderTarget);

    CSimpleSurface *pRenderTarget = pContext->pRenderTarget;
    CSimpleSurface *pZetaBuffer   = pContext->pZetaBuffer;

        if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN))) {

            // if ddraw has sullied the celsius object or if we last programmed
            // the celsius object for a different context, start from scratch
            if ((getDC()->dwLastHWContext != (DWORD)pContext))
            {
                getDC()->dwLastHWContext    = (DWORD)pContext;
                getDC()->bDirtyRenderTarget = TRUE;
            }

            if (pDriverData->bDirtyRenderTarget || pDriverData->TwoDRenderingOccurred) {

            // Send to hardware
            nvglSetObject(NV_DD_SURFACES, D3D_CONTEXT_SURFACES_ARGB_ZS);
            nvglSetRenderTargetContexts(NV_DD_SURFACES,
                                        NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

            // Normal case
            DWORD dwFormat = nv053SurfaceFormat[pRenderTarget->getFormat()];
            if (pRenderTarget->isSwizzled())
            {
                CTexture* pTexture = pRenderTarget->getWrapper()->getTexture();
                nvAssert(pTexture);
                dwFormat |= DRF_DEF(053, _SET_FORMAT, _TYPE, _SWIZZLE);
                dwFormat |= DRF_NUM(053, _SET_FORMAT, _WIDTH, pTexture->getLogWidth());
                dwFormat |= DRF_NUM(053, _SET_FORMAT, _HEIGHT, pTexture->getLogHeight());
            }
            else
            {
                dwFormat |= DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH);
            }

#ifndef STEREO_SUPPORT
            DWORD dwZOffset = pZetaBuffer ? pZetaBuffer->getOffset() : 0;
            DWORD dwTargetOffset = pRenderTarget->getOffset();
#else   //STEREO_SUPPORT==1
            DWORD dwZOffset = pZetaBuffer ? GetStereoOffset(pZetaBuffer) : 0;
            DWORD dwTargetOffset = GetStereoOffset(pRenderTarget);
#endif  //STEREO_SUPPORT
            if (!(pContext->surfaceViewport.clipHorizontal.wX)
             && !(pContext->surfaceViewport.clipVertical.wY))
            {
                DWORD dwClip = (((DWORD)pContext->surfaceViewport.clipVertical.wHeight) << 16) |
                               (pContext->surfaceViewport.clipHorizontal.wWidth);
                DWORD dwZPitch  = pZetaBuffer ? pZetaBuffer->getPitch()  : 32;
                DWORD dwPitch   = (DRF_NUM(053, _SET_PITCH, _COLOR, pRenderTarget->getPitch())) |
                                  (DRF_NUM(053, _SET_PITCH, _ZETA,  dwZPitch));
                nvglSetRenderTarget2(NV_DD_SURFACES,
                                     dwFormat,
                                     dwClip,
                                     dwPitch,
                                     dwTargetOffset,
                                     dwZOffset);
            }
            else
            {
                DWORD dwClip = (((DWORD)pContext->surfaceViewport.clipVertical.wHeight) << 16) |
                               (pContext->surfaceViewport.clipHorizontal.wWidth);
                DWORD dwZPitch  = pZetaBuffer ? pZetaBuffer->getPitch()  : 32;
                DWORD dwPitch   = (DRF_NUM(053, _SET_PITCH, _COLOR, pRenderTarget->getPitch())) |
                                  (DRF_NUM(053, _SET_PITCH, _ZETA,  dwZPitch));
                nvglSetRenderTarget2(NV_DD_SURFACES,
                                     dwFormat,
                                     dwClip,
                                     dwPitch,
                                     dwTargetOffset,
                                     dwZOffset);

                // Set the image black rectangle to the clip region.
                nvglSetObject(NV_DD_SURFACES, NV_DD_IMAGE_BLACK_RECTANGLE);
                nvglSetClip(NV_DD_SURFACES,
                            pContext->surfaceViewport.clipHorizontal.wX, pContext->surfaceViewport.clipVertical.wY,
                            pContext->surfaceViewport.clipHorizontal.wWidth, pContext->surfaceViewport.clipVertical.wHeight);
            }

            nvglSetObject(NV_DD_SURFACES, NV_DD_SURFACES_2D);
            pDriverData->TwoDRenderingOccurred = 0;
            pDriverData->ThreeDRenderingOccurred = TRUE;
            pDriverData->dDrawSpareSubchannelObject = 0; // force a reload.
            pDriverData->bDirtyRenderTarget = FALSE;
            pDriverData->ddClipUpdate = TRUE;
            pDriverData->dwSharedClipChangeCount++;

            NV_FORCE_TRI_SETUP(pContext);

        }  // ! (CELSIUS | KELVIN)
    }

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

void nvSetD3DSurfaceViewport
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvSetD3DSurfaceViewport");

    // Set the image black rectangle to the clip region.
    nvglSetObject(NV_DD_SURFACES, NV_DD_IMAGE_BLACK_RECTANGLE);
    nvglSetClip(NV_DD_SURFACES,
                pContext->surfaceViewport.clipHorizontal.wX, pContext->surfaceViewport.clipVertical.wY,
                pContext->surfaceViewport.clipHorizontal.wWidth, pContext->surfaceViewport.clipVertical.wHeight);

    nvglSetObject(NV_DD_SURFACES, NV_DD_SURFACES_2D);

    pDriverData->TwoDRenderingOccurred      = 0;
    pDriverData->ThreeDRenderingOccurred    = TRUE;
    pDriverData->dDrawSpareSubchannelObject = 0; // force a reload.
    pDriverData->ddClipUpdate = TRUE;
    pDriverData->dwSharedClipChangeCount++;
    NV_FORCE_TRI_SETUP(pContext);

    dbgTracePop();
}

//-------------------------------------------------------------------------------------

/*
 * Send DX5 Triangle class state to the hardware.
 */
void nvSetDx5TriangleState
(
    PNVD3DCONTEXT   pContext
)
{
    DWORD  *pTriangleState;
    DWORD  *pTriangleStateShadow;

    dbgTracePush ("nvSetDx5TriangleState");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    /*
     * Validate the inner loop hardware control registers.
     */
    dbgValidateControlRegisters(pContext);

#if 0
    /*
     * Send down the triangle state.
     */
    pTriangleState = (DWORD *)&pContext->ctxInnerLoop;
    /*
     * Only write channel if it is not selected - avoids texture cache invalidates
     */
    if (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE)
    {
        nvglSetObject(NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);
    }
    nvglDx5TriangleState(NV_DD_SPARE,
                         *(pTriangleState++), *(pTriangleState++), *(pTriangleState++),
                         *(pTriangleState++), *(pTriangleState++), *(pTriangleState++),
                         *(pTriangleState));
    pDriverData->dDrawSpareSubchannelObject = D3D_DX5_TEXTURED_TRIANGLE;
#else
    /*
     * prep variables
     */
    pTriangleState       = (DWORD*)&pContext->ctxInnerLoop;
    PREFETCH (pTriangleState);
    pTriangleStateShadow = (DWORD*)&pContext->ctxInnerLoopShadow;
    PREFETCH (pTriangleStateShadow);

    /*
     * set object if needed
     */
    if (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE)
    {
        nvglSetObject(NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);
        pDriverData->dDrawSpareSubchannelObject = D3D_DX5_TEXTURED_TRIANGLE;

        /*
         * force state load
         */
        pTriangleStateShadow[0] = ~pTriangleState[0];
        pTriangleStateShadow[1] = ~pTriangleState[1];
    }

    /*
     * wait-for-fe-idle methods
     */
    if ((pTriangleState[1] != pTriangleStateShadow[1])  // offset
     || (pTriangleState[2] != pTriangleStateShadow[2])) // format
    {
        nvglSetStartMethod (0,NV_DD_SPARE,NV054_OFFSET,2);
        nvglSetData        (1,            pTriangleState[1]);
        nvglSetData        (2,            pTriangleState[2]);
        nvglAdjust         (3);
        pTriangleStateShadow[1] = pTriangleState[1];
        pTriangleStateShadow[2] = pTriangleState[2];
    }

    /*
     * wait-for-idle methods
     */
    if ((pTriangleState[0] != pTriangleStateShadow[0])  // colorKey
     || (pTriangleState[3] != pTriangleStateShadow[3])  // filter
     || (pTriangleState[4] != pTriangleStateShadow[4])  // blend
     || (pTriangleState[5] != pTriangleStateShadow[5])  // control
     || (pTriangleState[6] != pTriangleStateShadow[6])) // fogColor
    {
        nvglSetStartMethod (0,NV_DD_SPARE,NV054_COLOR_KEY,1);
        nvglSetData        (1,            pTriangleState[0]);
        nvglSetStartMethod (2,NV_DD_SPARE,NV054_FILTER,4);
        nvglSetData        (3,            pTriangleState[3]);
        nvglSetData        (4,            pTriangleState[4]);
        nvglSetData        (5,            pTriangleState[5]);
        nvglSetData        (6,            pTriangleState[6]);
        nvglAdjust         (7);
        pTriangleStateShadow[0] = pTriangleState[0];
        pTriangleStateShadow[3] = pTriangleState[3];
        pTriangleStateShadow[4] = pTriangleState[4];
        pTriangleStateShadow[5] = pTriangleState[5];
        pTriangleStateShadow[6] = pTriangleState[6];
    }
#endif

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

/*
 * Context and hardware state set routines.
 */

DWORD nvSetContextState
(
    PNVD3DCONTEXT pContext,
    DWORD         dwStateType,
    DWORD         dwStateValue,
    HRESULT      *pddrval
)
{
    DWORD dwTextureHandle = 0;

    dbgTracePush ("nvSetContextState");

    /*
     * Make sure the state being set is within the range the driver is handling.
     */
    if (dwStateType <= D3D_RENDERSTATE_MAX)
    {

        // if we're getting a texture handle, translate it to the
        // form we store before checking for a change in the value
        if ((dwStateType == D3DRENDERSTATE_TEXTUREHANDLE) && (dwStateValue)) {
            if (global.dwDXRuntimeVersion >= 0x0700) {
                dwTextureHandle = dwStateValue;
                dwStateValue = (DWORD) nvGetObjectFromHandle (pContext->dwDDLclID, dwStateValue, &global.pNvSurfaceLists);
            }
            else {
                dwStateValue |= 0x80000000;
            }
        }

#ifdef TEX_MANAGE
        //Don't treat this like a normal renderstate we need to honor it EVERY time its called
        //not just when its value changes -- i.e. ms should have defined this as a dp2 not a rs.
        //case D3DRENDERSTATE_EVICTMANAGEDTEXTURES: // 61
        if(dwStateType == D3DRENDERSTATE_EVICTMANAGEDTEXTURES){
                DPF ("warning: hideously expensive call. under what circumstances do they do this??");
                nvTexManageEvictAll (pContext);
        }
#endif
        /*
         * Only update the state if it's different.
         */
        if (pContext->dwRenderState[dwStateType] != dwStateValue)
        {

            dbgShowRenderStateChange (TRUE, dwStateType, dwStateValue);

            // force boolean states to 0 or 1

            switch (dwStateType) {
                case D3DRENDERSTATE_WRAPU:             // 5
                case D3DRENDERSTATE_WRAPV:             // 6
                case D3DRENDERSTATE_ZWRITEENABLE:      // 14
                case D3DRENDERSTATE_ALPHATESTENABLE:   // 15
                case D3DRENDERSTATE_LASTPIXEL:         // 16
                case D3DRENDERSTATE_DITHERENABLE:      // 26
                case D3DRENDERSTATE_ALPHABLENDENABLE:  // 27
                case D3DRENDERSTATE_FOGENABLE:         // 28
                case D3DRENDERSTATE_SPECULARENABLE:    // 29
                case D3DRENDERSTATE_ZVISIBLE:          // 30
                case D3DRENDERSTATE_STIPPLEDALPHA:     // 33
                case D3DRENDERSTATE_EDGEANTIALIAS:     // 40
                case D3DRENDERSTATE_COLORKEYENABLE:    // 41
                case D3DRENDERSTATE_STENCILENABLE:     // 52
                case D3DRS_POINTSPRITEENABLE:
                case D3DRS_POINTSCALEENABLE:
                    dwStateValue = dwStateValue != 0;
                    break;
            }

            // handle some special cases and validate parameters

            switch (dwStateType) {

                case D3DRENDERSTATE_TEXTUREHANDLE: // 1
                    pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = dwStateValue;
                    pContext->tssState[0].dwHandle = dwTextureHandle;
                    break;

                case D3DRENDERSTATE_ANTIALIAS: // 2
                    // app must be DX7 or later and we must be running with a celsius or better
#if (NVARCH >= 0x010)
                    if (pContext->dwDXAppVersion >= 0x0700) {
#if (NVARCH >= 0x020)
                        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                            // we'll check it later
                        } else
#endif
                        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                            switch (dwStateValue)
                            {
                                case D3DANTIALIAS_SORTINDEPENDENT:
                                case D3DANTIALIAS_SORTDEPENDENT:
                                {
                                    // We used to create AA buffers here, but
                                    // now we do it later on, in flip, blit, and some other places.
                                    break;
                                }
                                default:
                                {
                                    dwStateValue = D3DANTIALIAS_NONE;
                                    nvCelsiusAADestroy (pContext);
                                    break;
                                }
                            }
                        }
                    } else
#endif  // NVARCH >= 0x010
                    {
                        // setting ignored
                        dwStateValue = D3DANTIALIAS_NONE;
                    }
                    break;

                case D3DRENDERSTATE_TEXTUREADDRESS: // 3
                    pContext->tssState[0].dwValue[D3DTSS_ADDRESSU] = dwStateValue;
                    pContext->tssState[0].dwValue[D3DTSS_ADDRESSV] = dwStateValue;
                    break;

#ifdef WINNT  // validate this parameter since we later use it to index a table
                case D3DRENDERSTATE_FILLMODE: // 8
                    if (dwStateValue > D3DFILL_SOLID) {
                        dwStateValue = D3DFILL_SOLID;
                    }
                    break;
#endif  // WINNT

                case D3DRENDERSTATE_TEXTUREMAG: // 17
                    switch (dwStateValue)
                    {
                        case D3DFILTER_NEAREST:
                            pContext->tssState[0].dwValue[D3DTSS_MAGFILTER] = D3DTFG_POINT;
                            break;
                        case D3DFILTER_LINEAR:
                            pContext->tssState[0].dwValue[D3DTSS_MAGFILTER] = D3DTFG_LINEAR;
                            break;
                        default:
                            DPF ("stupid app (ahem. turok) sent a bogus TEXTUREMAG. defaulting to linear");
                            pContext->tssState[0].dwValue[D3DTSS_MAGFILTER] = D3DTFG_LINEAR;
                            break;
                    }
                    break;

                case D3DRENDERSTATE_TEXTUREMIN: // 18
                    switch (dwStateValue)
                    {
                        case D3DFILTER_NEAREST:
                            pContext->tssState[0].dwValue[D3DTSS_MINFILTER] = D3DTFN_POINT;
                            pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] = D3DTFP_NONE;
                            break;
                        case D3DFILTER_LINEAR:
                            pContext->tssState[0].dwValue[D3DTSS_MINFILTER] = D3DTFN_LINEAR;
                            pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] = D3DTFP_NONE;
                            break;
                        case D3DFILTER_MIPNEAREST:
                            pContext->tssState[0].dwValue[D3DTSS_MINFILTER] = D3DTFN_POINT;
                            pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] = D3DTFP_POINT;
                            break;
                        case D3DFILTER_MIPLINEAR:
                            pContext->tssState[0].dwValue[D3DTSS_MINFILTER] = D3DTFN_LINEAR;
                            pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] = D3DTFP_POINT;
                            break;
                        case D3DFILTER_LINEARMIPNEAREST:
                            pContext->tssState[0].dwValue[D3DTSS_MINFILTER] = D3DTFN_POINT;
                            pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] = D3DTFP_LINEAR;
                            break;
                        case D3DFILTER_LINEARMIPLINEAR:
                            pContext->tssState[0].dwValue[D3DTSS_MINFILTER] = D3DTFN_LINEAR;
                            pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] = D3DTFP_LINEAR;
                            break;
                        default:
                            pContext->tssState[0].dwValue[D3DTSS_MINFILTER] = D3DTFN_POINT;
                            pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] = D3DTFP_NONE;
                            break;
                    }
                    break;

                case D3DRENDERSTATE_FOGTABLEMODE: // 35
                    // if fog table is disabled in the registry, don't allow it to be set
                    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_FOGTABLEENABLE_MASK) == D3D_REG_FOGTABLEENABLE_DISABLE) {
                        dwStateValue = D3DFOG_NONE;
                    }
                    break;

                case D3DRENDERSTATE_BORDERCOLOR: // 43
                    pContext->tssState[0].dwValue[D3DTSS_BORDERCOLOR] = dwStateValue;
                    break;

                case D3DRENDERSTATE_TEXTUREADDRESSU: // 44
                    pContext->tssState[0].dwValue[D3DTSS_ADDRESSU] = dwStateValue;
                    break;

                case D3DRENDERSTATE_TEXTUREADDRESSV: // 45
                    pContext->tssState[0].dwValue[D3DTSS_ADDRESSV] = dwStateValue;
                    break;

                case D3DRENDERSTATE_MIPMAPLODBIAS: // 46
                    pContext->tssState[0].dwValue[D3DTSS_MIPMAPLODBIAS] = dwStateValue;
                    pContext->tssState[0].dwLODBias = nvTranslateLODBias (dwStateValue);
                    break;

                case D3DRENDERSTATE_ANISOTROPY:
                    pContext->tssState[0].dwValue[D3DTSS_MAXANISOTROPY] = dwStateValue;
                    break;

                case D3DRENDERSTATE_SCENECAPTURE: // 62
                {
                    D3DHAL_SCENECAPTUREDATA scd;
                    scd.dwhContext = (DWORD) pContext;
                    if (dwStateValue)
                        scd.dwFlag = D3DHAL_SCENE_CAPTURE_START;
                    else
                        scd.dwFlag = D3DHAL_SCENE_CAPTURE_END;
                    nvSceneCapture (&scd);
                    break;
                }

                case D3DRENDERSTATE_AMBIENT: // 139
                    pContext->lighting.dcvAmbient.dvR = D3DVAL(RGBA_GETRED(dwStateValue))   / D3DVAL(255);
                    pContext->lighting.dcvAmbient.dvG = D3DVAL(RGBA_GETGREEN(dwStateValue)) / D3DVAL(255);
                    pContext->lighting.dcvAmbient.dvB = D3DVAL(RGBA_GETBLUE(dwStateValue))  / D3DVAL(255);
                    break;

                case D3DRS_POINTSIZE_MIN: // 155
                {
                    float fSizeMin = FLOAT_FROM_DWORD (dwStateValue);
                    fSizeMin = max (fSizeMin, 0.0001f);  // watch out for divide by zero....
                    fSizeMin = min (fSizeMin, 63.869f);
                    dwStateValue = DWORD_FROM_FLOAT (fSizeMin);
                    break;
                }

                case D3DRS_POINTSIZE_MAX: // 166
                {
                    float fSizeMax = FLOAT_FROM_DWORD (dwStateValue);
                    fSizeMax = max (fSizeMax, 0.0f);
                    fSizeMax = min (fSizeMax, 63.870f);
                    dwStateValue = DWORD_FROM_FLOAT (fSizeMax);
                    break;
                }

                case D3DRS_DELETERTPATCH: // 169
                {
                    CNvObject *pObj = NULL;
                    pObj = nvGetObjectFromHandle(pContext->dwDDLclID, dwStateValue, &global.pNvPatchLists);
                    if(pObj){
                        nvClearObjectListEntry(pObj, &global.pNvPatchLists);
                        pObj->release();
                        DPF_LEVEL(NVDBG_LEVEL_HOS_INFO,"Deleting patch #%d", dwStateValue);
                    }
#ifdef DEBUG
                    else{
                        DPF_LEVEL(NVDBG_LEVEL_ERROR,"Unable to delete patch #%d", dwStateValue);
                    }
#endif
                    break;
                }

                default:
                    // nada
                    break;

            }  // switch (dwstatetype)

            // now that we've sanitized things, store the value
            pContext->dwRenderState[dwStateType] = dwStateValue;
            pContext->bStateChange = TRUE;

            nvAssert (dwStateType <= D3D_RENDERSTATE_MAX);  // make sure we're in the table
#if (NVARCH >= 0x010)
            pContext->hwState.dwDirtyFlags |= celsiusDirtyBitsFromRenderState[dwStateType];

#ifdef ALPHA_CULL
            if((dwStateType == D3DRENDERSTATE_SRCBLEND || dwStateType == D3DRENDERSTATE_DESTBLEND) &&
                pContext->hwState.alphacull_mode >= 2){
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0097_KELVIN)
                    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_COMBINERS_COLOR | KELVIN_DIRTY_COMBINERS_SPECFOG;
                else
                    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_COMBINERS | CELSIUS_DIRTY_SPECFOG_COMBINER;
            }
#endif

#endif  // NVARCH >= 0x010
#if (NVARCH >= 0x020)
            pContext->hwState.dwDirtyFlags |= kelvinDirtyBitsFromRenderState[dwStateType];
#endif  // NVARCH >= 0x020

        }

        else {
            // state value !changed
            dbgShowRenderStateChange (FALSE, dwStateType, dwStateValue);
        }

    }

    pContext->kelvinAA.TestCreation(pContext);

    /*
     * Force a reload of the global context state.
     */
    pDriverData->dwCurrentContextHandle = 0;
    *pddrval = DD_OK;
    dbgTracePop();
    DDTICKOFFSET(dwStateType, dwStateValue);
    return (DDHAL_DRIVER_HANDLED);
}

//-------------------------------------------------------------------------------------

void cacheFogValues
(
    PNVD3DCONTEXT pContext
)
{
    DWORD *pdwRenderState = (DWORD *)pContext->dwRenderState;

    dbgTracePush ("cacheFogValues");

    // Store fog table values as floats in the context structure
    // and pre-calculate the linear scale
    pContext->fogData.dwFogTableMode   = NV_FOG_MODE;
    pContext->fogData.fFogTableStart   = FLOAT_FROM_DWORD(pdwRenderState[D3DRENDERSTATE_FOGTABLESTART]);
    pContext->fogData.fFogTableEnd     = FLOAT_FROM_DWORD(pdwRenderState[D3DRENDERSTATE_FOGTABLEEND]);
    pContext->fogData.fFogTableDensity = FLOAT_FROM_DWORD(pdwRenderState[D3DRENDERSTATE_FOGTABLEDENSITY]);

    if (pContext->fogData.dwFogTableMode == D3DFOG_LINEAR) {
        if (pContext->fogData.fFogTableEnd != pContext->fogData.fFogTableStart) {
            pContext->fogData.fFogTableLinearScale = 1.0f / (pContext->fogData.fFogTableEnd - pContext->fogData.fFogTableStart);
        }
        else {
#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                pContext->fogData.fFogTableLinearScale = KELVIN_MAX_FOG_SCALE;
            }
            else
#endif // NVARCH >= 0x020
#if (NVARCH >= 0x010)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                pContext->fogData.fFogTableLinearScale = CELSIUS_MAX_FOG_SCALE;
            }
            else
#endif // NVARCH >= 0x010
            {
                pContext->fogData.fFogTableLinearScale = 0.0f;
            }
        }
    }

    dbgTracePop();
}

//-------------------------------------------------------------------------------------

/*
 * Calculate the hardware state register based on the D3D context state settings.
 */
void nvCalculateHardwareStateDX5
(
    PNVD3DCONTEXT pContext,
    DWORD         dwTBlendState,
    DWORD         dwMinFilter,
    DWORD         dwMagFilter,
    BOOL          bDx6ToDx5Conversion
)
{
    BOOL            bAlphaTexture = FALSE;
    DWORD           dwMMDE;
    DWORD           dwAlphaRef;
    DWORD           dwAlphaFunc       = 0;  // Will get initialized before being used.
    DWORD           dwTextureColorFormat;
    DWORD           dwZOHMode, dwFOHMode, dwTexelOrigin;
    DWORD           dwTexturePerspective;
    DWORD           dwSrcBlend;
    DWORD           dwDstBlend;
    DWORD           dwAlphaBlendEnable;
    DWORD           dwColorKey        = 0;
    DWORD           dwOffset          = 0;
    DWORD           dwFormat          = 0;
    DWORD           dwFilter          = 0;
    DWORD           dwBlend           = 0;
    DWORD           dwControl         = 0;
    DWORD           dwFogColor        = 0;
    DWORD          *pdwRenderState    = 0;
    CNvObject      *pNvObj;
    CTexture       *pTexture;

    dbgTracePush ("nvCalculateHardwareStateDX5");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    pdwRenderState = (DWORD *)pContext->dwRenderState;

    dwZOHMode     = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_ZOH_MASK) == D3D_REG_TEXELALIGNMENT_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
    dwFOHMode     = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_FOH_MASK) == D3D_REG_TEXELALIGNMENT_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;
    dwTexelOrigin = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_TEXEL_MASK) == D3D_REG_TEXELALIGNMENT_TEXEL_CENTER)
                  ? NV054_CONTROL_ORIGIN_CENTER : NV054_CONTROL_ORIGIN_CORNER;

    /*
     * Maximize 2 pixels per clock.
     */
    switch (getDC()->nvD3DRegistryData.regMipMapDitherMode)
    {
        case D3D_REG_MIPMAPDITHERMODE_DISABLE:
        {
            //turn on trilinear dither when src and destination blends are one?
            dwMMDE = (pdwRenderState[D3DRENDERSTATE_SRCBLEND]  == D3DBLEND_ONE)
                  && (pdwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_ONE);
            break;
        }
        case D3D_REG_MIPMAPDITHERMODE_ENABLE:
        {
            if((dwMinFilter == D3DFILTER_LINEARMIPLINEAR) || (dwMinFilter == D3DFILTER_LINEARMIPNEAREST))
                dwMMDE = TRUE;
            else
                dwMMDE = FALSE;
            break;
        }
        case D3D_REG_MIPMAPDITHERMODE_SMART:
        {
            if((dwMinFilter == D3DFILTER_LINEARMIPLINEAR) || (dwMinFilter == D3DFILTER_LINEARMIPNEAREST))
                //performance adjustment -- if we are at a screen resolution bigger than 1024 don't use
                //true trilinear but approximate with dithered trilinear.
                dwMMDE = GET_MODE_WIDTH() >= 1024;
            else
                dwMMDE = FALSE;
            break;
        }
    }

    // binding of texture handles set with D3DRENDERSTATE_TEXTUREHANDLE
    DWORD dwTextureHandle = pContext->tssState[0].dwHandle;
    if (dwTextureHandle) {
        pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = (DWORD) nvGetObjectFromHandle (pContext->dwDDLclID, dwTextureHandle, &global.pNvSurfaceLists);
        pContext->tssState[0].dwHandle = 0;
    }

    /*
     * Set up the texture format register.
     */
    pNvObj   = (CNvObject *)(pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]);
    pTexture = pNvObj ? pNvObj->getTexture() : NULL;

    if (pTexture) {
        /*
         * Display the new texture to the current visible buffer.
         */
        dbgTextureDisplay (pTexture);

        /*
         * Grab the color key from the texture.
         */
        dwTextureColorFormat = nv054TextureFormat[pTexture->getFormat()];
        if (pTexture->hasColorKey()) {
            dwColorKey = pTexture->getColorKey();
            dwFormat |= DRF_NUM(054, _FORMAT, _COLORKEYENABLE, pdwRenderState[D3DRENDERSTATE_COLORKEYENABLE]);
        }
        else {
            dwFormat |= DRF_DEF(054, _FORMAT, _COLORKEYENABLE, _FALSE);
        }

        /*
         * Get the texture format and offset.
         */
        DWORD dwTCIndex = pContext->tssState[0].dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff;
        BOOL  bWrapU, bWrapV, bWrapP, bWrapQ;
        NV_EVAL_CYLINDRICAL_WRAP (pContext, dwTCIndex, bWrapU, bWrapV, bWrapP, bWrapQ);

        dwOffset  = pTexture->getSwizzled()->getOffset();
        dwFormat |= DRF_NUM(054, _FORMAT, _CONTEXT_DMA,     nv054TextureContextDma[pTexture->getSwizzled()->getContextDMA()])
                 |  DRF_NUM(054, _FORMAT, _COLOR,           dwTextureColorFormat)
                 |  DRF_NUM(054, _FORMAT, _BASE_SIZE_U,     pTexture->getLogWidth())
                 |  DRF_NUM(054, _FORMAT, _BASE_SIZE_V,     pTexture->getLogHeight())
                 |  DRF_NUM(054, _FORMAT, _TEXTUREADDRESSU, pContext->tssState[0].dwValue[D3DTSS_ADDRESSU])
                 |  DRF_NUM(054, _FORMAT, _WRAPU,           bWrapU)
                 |  DRF_NUM(054, _FORMAT, _TEXTUREADDRESSV, pContext->tssState[0].dwValue[D3DTSS_ADDRESSV])
                 |  DRF_NUM(054, _FORMAT, _WRAPV,           bWrapV);
        dwFormat |= DRF_NUM(054, _FORMAT, _ORIGIN_ZOH,      dwZOHMode)
                 |  DRF_NUM(054, _FORMAT, _ORIGIN_FOH,      dwFOHMode);

        /*
         * When there is a texture, use the texture map blend set by the user.
         * One exception.  For textures that do not have an alpha component, set MODULATEALPHA
         * instead of MODULATE.
         *
         * NOTE: For some reason, this does not work when SRCBLEND == DESTBLEND == D3DBLEND_ONE
         *       WHY? (Incoming)
         *
         * NOTE: I'm going to use the fact that the hardware doesn't implement MODULATE exactly
         *       as specified in order to get two pixels per clock out of the following DX6
         *       single texture stage 0 setup: COLOROP = MODULATE, COLORARG1 = TEXTURE,
         *       COLORARG2 = CURRENT/DIFFUSE ALPHAOP = SELECTARG1, ALPHAARG1 = TEXTURE.
         *       When useing TBLEND=MODULATE and a non-alpha texture, the alpha should come from the
         *       Diffuse color component, however when using texture stages, we should not do this.
         *       On entry, if we are converting from a texture stage setup, the bUseDX6Class flag will
         *       still be set, and under this condition, we should not modify the passed in TBLEND value.
         */
        if ((!bDx6ToDx5Conversion)
         && (dwTBlendState == D3DTBLEND_MODULATE)
         && (pdwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE])
         && !((pdwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_ONE)
          &&  (pdwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_ONE))
         && ((dwTextureColorFormat == NV054_FORMAT_COLOR_LE_X1R5G5B5)
          || (dwTextureColorFormat == NV054_FORMAT_COLOR_LE_R5G6B5)
          || (dwTextureColorFormat == NV054_FORMAT_COLOR_LE_X8R8G8B8)))
        {
            dwTBlendState = D3DTBLEND_MODULATEALPHA;
            dwBlend  |= DRF_DEF(054, _BLEND, _TEXTUREMAPBLEND, _MODULATEALPHA);
        }
        else
            dwBlend  |= DRF_NUM(054, _BLEND, _TEXTUREMAPBLEND, dwTBlendState);

        /*
         * When this is a user supplied mipmap chain
         * OR the number of mipmap levels is 1 (i.e. no mipmaps, only base texture)
         * set the MIN/MAG filters as specified by the render states.
         */
        dwFilter |= DRF_NUM(054, _FILTER, _MIPMAP_DITHER_ENABLE, dwMMDE)
                 |  DRF_NUM(054, _FILTER, _TEXTUREMIN,    dwMinFilter)
                 |  DRF_NUM(054, _FILTER, _TEXTUREMAG,    dwMagFilter);

        /*
         * The only way to guarantee that mipmapping is disabled is to set MIPMAP_LEVELS to 1.
         * In cases where there are the user has supplied mipmaps but the minification filter is
         * set to D3DFILTER_NEAREST or D3DFILTER_LINEAR, force the mipmap levels field to be a 1.
         */
        if ((dwMinFilter == D3DFILTER_NEAREST) || (dwMinFilter == D3DFILTER_LINEAR)) {
            dwFormat |= DRF_NUM(054, _FORMAT, _MIPMAP_LEVELS, 1);
        }
        else {
            dwFormat |= DRF_NUM(054, _FORMAT, _MIPMAP_LEVELS, pTexture->getMipMapCount());
        }

         /*
         * Set the Mipmap LOD BIAS based on registery setting and render state setting.
         */
        if ((dwMinFilter == D3DFILTER_MIPNEAREST) || (dwMinFilter == D3DFILTER_MIPLINEAR))
            dwFilter |= DRF_NUM(054, _FILTER, _MIPMAPLODBIAS, pContext->tssState[0].dwLODBias + 0x04);
        else
            dwFilter |= DRF_NUM(054, _FILTER, _MIPMAPLODBIAS, pContext->tssState[0].dwLODBias);

        /*
         * When texturing, always select the proper TexturePerspective render state value.
         */
        dwTexturePerspective = pdwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE];
    }
    else    //no texture
    {
        /*
         * There is no texture set the default texture format and offset.
         */
        dwTextureColorFormat = getDC()->pDefaultTexture->getFormat();
        dwOffset  = getDC()->pDefaultTexture->getSwizzled()->getOffset();
        dwFormat |= DRF_NUM(054, _FORMAT, _CONTEXT_DMA,     nv054TextureContextDma[getDC()->pDefaultTexture->getSwizzled()->getContextDMA()])
                 |  DRF_DEF(054, _FORMAT, _COLORKEYENABLE,  _FALSE)
                 |  DRF_NUM(054, _FORMAT, _COLOR,           dwTextureColorFormat)
                 |  DRF_NUM(054, _FORMAT, _MIPMAP_LEVELS,    1)
                 |  DRF_DEF(054, _FORMAT, _BASE_SIZE_U,     _1)
                 |  DRF_DEF(054, _FORMAT, _BASE_SIZE_V,     _1)
                 |  DRF_DEF(054, _FORMAT, _TEXTUREADDRESSU, _CLAMP)
                 |  DRF_DEF(054, _FORMAT, _WRAPU,           _FALSE)
                 |  DRF_DEF(054, _FORMAT, _TEXTUREADDRESSV, _CLAMP)
                 |  DRF_DEF(054, _FORMAT, _WRAPV,           _FALSE);
        dwFormat |= DRF_NUM(054, _FORMAT, _ORIGIN_ZOH,      dwZOHMode)
                 |  DRF_NUM(054, _FORMAT, _ORIGIN_FOH,      dwFOHMode);

        /*
         * When there is no texture,
         *  - always set texture map blend to be MODULATEALPHA when
         *    a) alphablend is enabled or
         *    b) alphatest is enabled
         *  - otherwise just go for modulate. (bdw: why do we want this ever?)
         */
        if (pdwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]
         || pdwRenderState[D3DRENDERSTATE_ALPHATESTENABLE])
            dwBlend  |= DRF_DEF(054, _BLEND, _TEXTUREMAPBLEND, _MODULATEALPHA);
        else
            dwBlend  |= DRF_DEF(054, _BLEND, _TEXTUREMAPBLEND, _MODULATE);

        /*
         * When there's no texture, always set for nearest (point sample) filtering.
         */
        dwFilter |= DRF_DEF(054, _FILTER, _MIPMAP_DITHER_ENABLE, _FALSE)
                 |  DRF_DEF(054, _FILTER, _TEXTUREMIN,    _NEAREST)
                 |  DRF_DEF(054, _FILTER, _TEXTUREMAG,    _NEAREST);

        /*
         * When there is no texture, set texture perspective to false to insure that no 0 RHW values
         * screw up the hardware.
         * But this isn't a good hack.  So it's gone.
         */
//        dwTexturePerspective = FALSE;
        dwTexturePerspective = pdwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE];

        /*
         * Set the Mipmap LOD BIAS based on registery setting and render state setting.
         */
        dwFilter |= DRF_NUM(054, _FILTER, _MIPMAPLODBIAS, 0);

    }


    /*
     * dwMaxAnisotropy defines 1 as no anisotropy.
     * The following if statement is based on the reference rasterizer.
     */

    if((pContext->tssState[0].dwValue[D3DTSS_MAGFILTER] == D3DTFG_ANISOTROPIC ||
        pContext->tssState[0].dwValue[D3DTSS_MINFILTER] == D3DTFN_ANISOTROPIC) &&
        pContext->tssState[0].dwValue[D3DTSS_MAXANISOTROPY] > 1){
            dwFilter |= DRF_NUM(054, _FILTER, _KERNEL_SIZE_X,              0x1F)
                     |  DRF_NUM(054, _FILTER, _KERNEL_SIZE_Y,              0x1F)
                     |  DRF_DEF(054, _FILTER, _ANISOTROPIC_MINIFY_ENABLE,  _TRUE)
                     |  DRF_DEF(054, _FILTER, _ANISOTROPIC_MAGNIFY_ENABLE, _TRUE);
    }
    else
    {
        dwFilter |= DRF_DEF(054, _FILTER, _ANISOTROPIC_MINIFY_ENABLE,  _FALSE)
                 |  DRF_DEF(054, _FILTER, _ANISOTROPIC_MAGNIFY_ENABLE, _FALSE);
    }

    /*
     * Set up the control register.
     */
    dwControl |= DRF_NUM(054, _CONTROL, _ORIGIN,               dwTexelOrigin)
              |  DRF_NUM(054, _CONTROL, _ZFUNC,                CTFunc(pContext,pdwRenderState[D3DRENDERSTATE_ZFUNC]))
              |  DRF_NUM(054, _CONTROL, _CULLMODE,             pdwRenderState[D3DRENDERSTATE_CULLMODE])
              |  DRF_NUM(054, _CONTROL, _DITHERENABLE,         pdwRenderState[D3DRENDERSTATE_DITHERENABLE]);

    /*
     * Get the Src/Dest blending state
     */
    dwAlphaBlendEnable = pdwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE];
    dwSrcBlend         = pdwRenderState[D3DRENDERSTATE_SRCBLEND];
    dwDstBlend         = pdwRenderState[D3DRENDERSTATE_DESTBLEND];

    /*
     * Colorkey setup.
     */
    if ((pdwRenderState[D3DRENDERSTATE_COLORKEYENABLE])
     && ((dwFormat >> DRF_SHIFT(NV054_FORMAT_COLORKEYENABLE)) & DRF_MASK(NV054_FORMAT_COLORKEYENABLE)))
    {
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "D3D:nvCalculateHardwareStateDX5 - ColorKey Enabled");
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    ColorKey Color     = %08x", dwColorKey);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    AlphaTestEnable    = %08x", pdwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    AlphaRef           = %08x", pdwRenderState[D3DRENDERSTATE_ALPHAREF]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    AlphaFunc          = %08x", pdwRenderState[D3DRENDERSTATE_ALPHAFUNC]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    AlphaBlendEnable   = %08x", pdwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    SrcBlend           = %08x", pdwRenderState[D3DRENDERSTATE_SRCBLEND]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    DstBlend           = %08x", pdwRenderState[D3DRENDERSTATE_DESTBLEND]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    TextureMapBlend    = %08x", dwTBlendState);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    TextureColorFormat = %08x", dwTextureColorFormat);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    MagFliter          = %08x", dwMagFilter);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    MinFliter          = %08x", dwMinFilter);

        bAlphaTexture = ((dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A1R5G5B5)
                      || (dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A4R4G4B4)
                      || (dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A8R8G8B8));

        if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_CKCOMPATABILITYENABLE_MASK) == D3D_REG_CKCOMPATABILITYENABLE_DISABLE)
        {
            DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "ColorKey Compatability Disabled");
            goto    labelColorKeySetupNoAlphaTest;
        }

        /*
         * Check the texture format and alpha blending operation.
         * for 8888 and 1555 textures, if (alpha blending is disabled) OR
         * (alpha blending is enabled and neither srcblend and destblend use the source
         * alpha) then change the format sent to hardware to be the complimentery non-alpha
         * texture format.
         */
        if ((dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A1R5G5B5)
         || (dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A8R8G8B8))
        {
            DWORD dwNewFormat;
            dwNewFormat = (dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A1R5G5B5) ? NV054_FORMAT_COLOR_LE_X1R5G5B5 : NV054_FORMAT_COLOR_LE_X8R8G8B8;

            if (!dwAlphaBlendEnable)
            {
                /*
                 * When alpha blending is disabled, force 1555 and 8888 textures to be x555 and x888
                 * repectively so that non-colorkey 0 alpha texels don't get alpha tested away.
                 */
                dwFormat &= ~(DRF_MASK(NV054_FORMAT_COLOR) << DRF_SHIFT(NV054_FORMAT_COLOR));
                dwFormat |= DRF_NUM(054, _FORMAT, _COLOR, dwNewFormat);
            }
            else
            {
                if ((dwSrcBlend != D3DBLEND_SRCALPHA)
                 && (dwSrcBlend != D3DBLEND_INVSRCALPHA)
                 && (dwSrcBlend != D3DBLEND_SRCALPHASAT)
                 && (dwSrcBlend != D3DBLEND_BOTHSRCALPHA)
                 && (dwSrcBlend != D3DBLEND_BOTHINVSRCALPHA)
                 && (dwDstBlend != D3DBLEND_SRCALPHA)
                 && (dwDstBlend != D3DBLEND_INVSRCALPHA)
                 && (dwDstBlend != D3DBLEND_SRCALPHASAT)
                 && (dwDstBlend != D3DBLEND_BOTHSRCALPHA)
                 && (dwDstBlend != D3DBLEND_BOTHINVSRCALPHA))
                {
                    /*
                     * When alpha blending is enabled but the srcalpha is not being used, force 1555
                     * and 8888 textures to be x555 and x888 repectively so that non-colorkey 0 alpha
                     * texels don't get alpha tested away.
                     */
                    dwFormat &= ~(DRF_MASK(NV054_FORMAT_COLOR) << DRF_SHIFT(NV054_FORMAT_COLOR));
                    dwFormat |= DRF_NUM(054, _FORMAT, _COLOR,            dwNewFormat);
                }
            }
        }

        /*
         * Change color key implementation to always use alpha test
         */
        if (!pdwRenderState[D3DRENDERSTATE_ALPHATESTENABLE])
        {
            DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "ColorKey with registry alphatest");

            // if the blend uses the diffuse color and the diffuse color happens to have alpha=0,
            // the resultant pixel will also have alpha=0 and will always get alpha tested away.
            // we need to prevent this when possible by changing the blend mode to something
            // that will NOT squash all alphas to 0. of course we can only do this if we don't
            // care about the value of alpha for other reasons...

            BOOL bTBlendUsesDiffuse = (dwTBlendState == D3DTBLEND_MODULATEALPHA) ||
                                      (dwTBlendState == D3DTBLEND_DECALALPHA)    ||
                                      (dwTBlendState == D3DTBLEND_DECALMASK)     ||
                                      (dwTBlendState == D3DTBLEND_MODULATEMASK);

            if (bTBlendUsesDiffuse) {

                if ((!dwAlphaBlendEnable)
                    ||
                    ((dwSrcBlend == D3DBLEND_ONE)          &&
                     (dwDstBlend != D3DBLEND_SRCALPHA)     &&
                     (dwDstBlend != D3DBLEND_INVSRCALPHA)  &&
                     (dwDstBlend != D3DBLEND_SRCALPHASAT)  &&
                     (dwDstBlend != D3DBLEND_BOTHSRCALPHA) &&
                     (dwDstBlend != D3DBLEND_BOTHINVSRCALPHA)
                    )
                   )
                {
                    dwBlend &= ~DRF_MASK(NV054_BLEND_TEXTUREMAPBLEND);
                    dwBlend |= DRF_DEF(054, _BLEND, _TEXTUREMAPBLEND, _MODULATE);
                }

            }

            /*
             * Set the colorkey alphatest mode.
             */
            dwAlphaRef = getDC()->nvD3DRegistryData.regColorkeyRef;
            dwControl |= DRF_NUM(054, _CONTROL, _ALPHAREF,        dwAlphaRef)
                      |  DRF_DEF(054, _CONTROL, _ALPHAFUNC,       _GREATER)
                      |  DRF_DEF(054, _CONTROL, _ALPHATESTENABLE, _TRUE);
        }
        else
        {
            DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "ColorKey with app alphatest");

            /*
             * Calculate the hardware alpha test reference value.
             */
            dwAlphaRef = pdwRenderState[D3DRENDERSTATE_ALPHAREF];
            if (dwAlphaRef == 0x00010000)
                dwAlphaRef = 0xFF;
            else if (dwAlphaRef & 0xFF00)
                dwAlphaRef >>= 8;

            /*
             * Get the application specified alpha test comparison function.
             */
            dwAlphaFunc = pdwRenderState[D3DRENDERSTATE_ALPHAFUNC];

            /*
             * There is a very specialized case where we want to actually modify the alphatest setup
             * from what is passed in by the application.  This is for Motoracer2 which does something
             * very strange...but not all the time.  This is most likely an application bug, but I'm
             * working around it here...yuck.
             *
             * The situation is this, occasionally, Motoracer2 will set the following alphatest state:
             * AlphaTestEnable = 1
             * AlphaRef = 0x00
             * AlphaFunc = 7 (GREATEREQUAL)
             * AlphaBlendEnable = 1
             * SrcBlend = 5 (SRCALPHA)
             * DstBlend = 2 (ONE)
             * TextureMapBlend = 4 (MODULATEALPHA)
             * TextureColorFormat = 5 (R5G6B5)
             * However, in this situation, with the alpharef of 0 and the alphafunc set to GREATEREQUAL,
             * the colorkey (which has it's alpha forced to 0) will still show up since the alphatest
             * comparison will always be TRUE.
             * So, I'll make a general case where if the I see this combination when colorkeying is enabled,
             * I'll just change the alphafunc to be GREATER.
             */
            if ((dwAlphaRef == 0x00)
             && (dwAlphaFunc == D3DCMP_GREATEREQUAL ||
                 dwAlphaFunc == D3DCMP_ALWAYS))
            {
                DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "Modify application alphafunc");
                dwAlphaFunc = D3DCMP_GREATER;
            }

            /*
             * Set the hardware alpha test state.
             */
            dwControl |= DRF_NUM(054, _CONTROL, _ALPHAREF,        dwAlphaRef)
                      |  DRF_NUM(054, _CONTROL, _ALPHAFUNC,       dwAlphaFunc)
                      |  DRF_NUM(054, _CONTROL, _ALPHATESTENABLE, pdwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]);
        }
    }
    else
    {
#if 0   // VR POWERBOAT RACING HACK - NEED SOMETHING EXTREMELY SPECIFIC TO VR POWERBOAT RACING HERE.
        if ((!pdwRenderState[D3DRENDERSTATE_ALPHATESTENABLE])
         && (pdwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE])
         && (dwSrcBlend != D3DBLEND_ONE))
        {
            /*
             * Alpha test away zero alpha pixels to repress z-writes when alpha test
             * is not enabled and alpha blending is enabled.  This is a HACK to work
             * around an application bug in VR Powerboat Racing.
             */
            dwControl |= DRF_NUM(054, _CONTROL, _ALPHAREF,        0x00)
                      |  DRF_NUM(054, _CONTROL, _ALPHAFUNC,       D3DCMP_GREATER)
                      |  DRF_NUM(054, _CONTROL, _ALPHATESTENABLE, TRUE);
        }
        else
#endif  // VR POWERBOAT RACING HACK
        {
labelColorKeySetupNoAlphaTest:
            dwAlphaRef = pdwRenderState[D3DRENDERSTATE_ALPHAREF];
            if (dwAlphaRef == 0x00010000)
                dwAlphaRef = 0xFF;
            else if (dwAlphaRef & 0xFF00)
                dwAlphaRef >>= 8;
            dwControl |= DRF_NUM(054, _CONTROL, _ALPHAREF,        dwAlphaRef)
                      |  DRF_NUM(054, _CONTROL, _ALPHAFUNC,       pdwRenderState[D3DRENDERSTATE_ALPHAFUNC])
                      |  DRF_NUM(054, _CONTROL, _ALPHATESTENABLE, pdwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]);
        }
    }

    /*
     * Set up the texture map blending control register.
     * Not sure about the MASK_BIT.
     */
    dwBlend |= DRF_DEF(054, _BLEND, _MASK_BIT,           _MSB)
            |  DRF_NUM(054, _BLEND, _SHADEMODE,          D3DSHADE_GOURAUD/*pdwRenderState[D3DRENDERSTATE_SHADEMODE]*/) // inner loop takes care of flat shading
            |  DRF_NUM(054, _BLEND, _TEXTUREPERSPECTIVE, dwTexturePerspective)
            |  DRF_NUM(054, _BLEND, _SPECULARENABLE,     pdwRenderState[D3DRENDERSTATE_SPECULARENABLE])
            |  DRF_NUM(054, _BLEND, _FOGENABLE,          pdwRenderState[D3DRENDERSTATE_FOGENABLE])
            |  DRF_NUM(054, _BLEND, _ALPHABLENDENABLE,   dwAlphaBlendEnable);
    if ((pdwRenderState[D3DRENDERSTATE_SRCBLEND]) == D3DBLEND_BOTHSRCALPHA)
        dwBlend |= DRF_DEF(054, _BLEND, _SRCBLEND,  _SRCALPHA)
                |  DRF_DEF(054, _BLEND, _DESTBLEND, _INVSRCALPHA);
    else if ((pdwRenderState[D3DRENDERSTATE_SRCBLEND]) == D3DBLEND_BOTHINVSRCALPHA)
        dwBlend |= DRF_DEF(054, _BLEND, _SRCBLEND,  _INVSRCALPHA)
                |  DRF_DEF(054, _BLEND, _DESTBLEND, _SRCALPHA);
    else if ((pdwRenderState[D3DRENDERSTATE_DESTBLEND]) == D3DBLEND_BOTHSRCALPHA)
        dwBlend |= DRF_DEF(054, _BLEND, _SRCBLEND,  _SRCALPHA)
                |  DRF_DEF(054, _BLEND, _DESTBLEND, _INVSRCALPHA);
    else if ((pdwRenderState[D3DRENDERSTATE_DESTBLEND]) == D3DBLEND_BOTHINVSRCALPHA)
        dwBlend |= DRF_DEF(054, _BLEND, _SRCBLEND,  _INVSRCALPHA)
                |  DRF_DEF(054, _BLEND, _DESTBLEND, _SRCALPHA);
    else
        dwBlend |= DRF_NUM(054, _BLEND, _SRCBLEND,  dwSrcBlend)
                |  DRF_NUM(054, _BLEND, _DESTBLEND, dwDstBlend);

    if ((pContext->pZetaBuffer)
     && (pdwRenderState[D3DRENDERSTATE_ZENABLE]))
    {
        if ((pdwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE)
         || ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_WBUFFERENABLE_MASK) == D3D_REG_WBUFFERENABLE_DISABLE))
        {
            /*
             * Plain Z-Buffer.
             */
            dwControl |= DRF_DEF(054, _CONTROL, _ZENABLE,              _TRUE)
                      |  DRF_NUM(054, _CONTROL, _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                      |  DRF_DEF(054, _CONTROL, _Z_PERSPECTIVE_ENABLE, _FALSE)
                      |  DRF_DEF(054, _CONTROL, _Z_FORMAT,             _FIXED);
        }
        else
        {
            /*
             * W-Buffer.
             */
            nvAssert (pContext->pRenderTarget);
            if (pContext->pRenderTarget->getBPP() == 2)
            {
                // Fixed point w-buffer requires scaling of RHW but gives more precision in 16bit w-buffers.
                dwControl |= DRF_DEF(054, _CONTROL,  _ZENABLE,              _TRUE)
                          |  DRF_NUM(054, _CONTROL,  _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                          |  DRF_DEF(054, _CONTROL,  _Z_PERSPECTIVE_ENABLE, _TRUE)
                          |  DRF_NUM(054, _CONTROL,  _Z_FORMAT,             getDC()->nvD3DRegistryData.regW16Format);
            }
            else
            {
                // Floating point w-buffer gives more precision for 32-bit (also removes a HW rounding bug with fixed)
                dwControl |= DRF_DEF(054, _CONTROL,  _ZENABLE,              _TRUE)
                          |  DRF_NUM(054, _CONTROL,  _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                          |  DRF_DEF(054, _CONTROL,  _Z_PERSPECTIVE_ENABLE, _TRUE)
                          |  DRF_NUM(054, _CONTROL,  _Z_FORMAT,             getDC()->nvD3DRegistryData.regW32Format);
            }
        }
    }
    else
    {
        dwControl |= DRF_DEF(054, _CONTROL, _ZENABLE,              _FALSE)
                  |  DRF_DEF(054, _CONTROL, _ZWRITEENABLE,         _FALSE)
                  |  DRF_DEF(054, _CONTROL, _Z_PERSPECTIVE_ENABLE, _FALSE)
                  |  DRF_DEF(054, _CONTROL, _Z_FORMAT,             _FIXED);

    }

    /*
     * Set the fog color.
     */
    dwFogColor = pdwRenderState[D3DRENDERSTATE_FOGCOLOR];

    cacheFogValues (pContext);

    /*
     * handle textures that are unaligned
     *
     * this happens when a mipmap chain is created and the app selects one
     *  of the unaligned lower level mipmaps as the base texture
     * aka HW alignment limitation
     */
    if (dwOffset & 0xff)
    {
        DWORD         dwSize;
        DWORD         dwAddr;

        /*
         * get source address
         */
        dwAddr = pTexture->getSwizzled()->getAddress();

        /*
         * determine how many bytes to move
         */
        dwSize = pTexture->getBaseTexture()->getSwizzled()->getSize()
               - (dwOffset - pTexture->getBaseTexture()->getSwizzled()->getOffset());

        /*
         * move them to spare texture 0
         */
        nvMemCopy (getDC()->dwSpareTextureOffset[0] + VIDMEM_ADDR(pDriverData->BaseAddress),dwAddr,dwSize);

        /*
         * modify texture source
         */
        dwOffset  = getDC()->dwSpareTextureOffset[0];
        dwFormat &= ~0x3;
        dwFormat |= nv054TextureContextDma[NV_CONTEXT_DMA_VID];
    }

    /*
     * Store the register values.
     */
    pContext->ctxInnerLoop.dwColorKey      = dwColorKey;
    pContext->ctxInnerLoop.dwTextureOffset = dwOffset;
    pContext->ctxInnerLoop.dwFormat        = dwFormat;
    pContext->ctxInnerLoop.dwFilter        = dwFilter;
    pContext->ctxInnerLoop.dwBlend         = dwBlend;
    pContext->ctxInnerLoop.dwControl       = dwControl;
    pContext->ctxInnerLoop.dwFogColor      = dwFogColor;
    pContext->bStateChange                 = FALSE;

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

BOOL nvSetHardwareState
(
    PNVD3DCONTEXT pContext
)
{
    DWORD   dwTBlend;
    DWORD   dwMinFilter, dwMagFilter;

    dbgTracePush ("nvSetHardwareState");

    // This code is only enabled in the DEBUG build.
    // Display the DX5 render state to be calculated.
    dbgDisplayContextState (NVDBG_SHOW_RENDER_STATE, pContext);

    // Get state to be passed to the hardware setup routine.
    dwTBlend = pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND];

    // Calculate equivalent TEXTUREMIN and TEXTUREMAG bits from
    // MINFILTER, MAGFILTER and MIPFILTER.
    switch (pContext->tssState[0].dwValue[D3DTSS_MAGFILTER]) {
        case D3DTFG_POINT:
            dwMagFilter = D3DFILTER_NEAREST;
            break;
        case D3DTFG_LINEAR:
            dwMagFilter = D3DFILTER_LINEAR;
            break;
        default:
            dwMagFilter = D3DFILTER_LINEAR;
            break;
    }

    if (pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] == D3DTFP_NONE) {
        switch (pContext->tssState[0].dwValue[D3DTSS_MINFILTER]) {
            case D3DTFN_POINT:
                dwMinFilter = D3DFILTER_NEAREST;
                break;
            case D3DTFN_LINEAR:
                dwMinFilter = D3DFILTER_LINEAR;
                break;
            default:
                dwMinFilter = D3DFILTER_LINEAR;
                break;
        }
    }
    else if (pContext->tssState[0].dwValue[D3DTSS_MIPFILTER] == D3DTFP_POINT) {
        switch (pContext->tssState[0].dwValue[D3DTSS_MINFILTER]) {
            case D3DTFN_POINT:
                dwMinFilter = D3DFILTER_MIPNEAREST;
                break;
            case D3DTFN_LINEAR:
                dwMinFilter = D3DFILTER_MIPLINEAR;
                break;
            default:
                dwMinFilter = D3DFILTER_MIPLINEAR;
                break;
        }
    }
    else {
        switch (pContext->tssState[0].dwValue[D3DTSS_MINFILTER]) {
            case D3DTFN_POINT:
                dwMinFilter = D3DFILTER_LINEARMIPNEAREST;
                break;
            case D3DTFN_LINEAR:
                dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                break;
            default:
                dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                break;
        }
    }

    // Build up the DX5 control state.
    nvCalculateHardwareStateDX5 (pContext, dwTBlend, dwMinFilter, dwMagFilter, FALSE);

    // This code is only enabled in the DEBUG build.
    // Validate the inner loop hardware control registers.
    dbgValidateControlRegisters (pContext);

    // This code is only enabled in the DEBUG build.
    // Display the hardware registers that were calculated.
    dbgDisplayContextState (NVDBG_SHOW_DX5_CLASS_STATE, pContext);

    dbgTracePop();
    return (TRUE);
}

//-------------------------------------------------------------------------------------

/*
 * Format of the cobine data is:
 * Bits 31-24  23-16  15-8  7 - 0
 *       00     op    arg2  arg1
 */

/*
 * DX5 D3DTBLEND_DECAL equivalent texture stage state.
 */
#define DX5_TSSCOLOR_DECAL_MASK_1      0x00FF00FF
#define DX5_TSSCOLOR_DECAL_1           ((D3DTA_TEXTURE << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSCOLOR_DECAL_MASK_2      0x00FFFF00
#define DX5_TSSCOLOR_DECAL_2           ((D3DTA_TEXTURE << 8) | (D3DTOP_SELECTARG2 << 16))

#define DX5_TSSALPHA_DECAL_MASK_1      0x00FF00FF
#define DX5_TSSALPHA_DECAL_1           ((D3DTA_TEXTURE << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSALPHA_DECAL_MASK_2      0x00FFFF00
#define DX5_TSSALPHA_DECAL_2           ((D3DTA_TEXTURE << 8) | (D3DTOP_SELECTARG2 << 16))

/*
 * DX5 D3DTBLEND_DECALALPHA equivalent texture stage state.
 */
#define DX5_TSSCOLOR_DECALALPHA_MASK    0x00FFFFFF
#define DX5_TSSCOLOR_DECALALPHA_1       ((D3DTA_TEXTURE << 0) | (D3DTA_DIFFUSE << 8) | (D3DTOP_BLENDTEXTUREALPHA << 16))
#define DX5_TSSCOLOR_DECALALPHA_2       ((D3DTA_TEXTURE << 0) | (D3DTA_CURRENT << 8) | (D3DTOP_BLENDTEXTUREALPHA << 16))
#define DX5_TSSCOLOR_DECALALPHA_3       ((D3DTA_DIFFUSE << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_BLENDTEXTUREALPHA << 16))
#define DX5_TSSCOLOR_DECALALPHA_4       ((D3DTA_CURRENT << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_BLENDTEXTUREALPHA << 16))

#define DX5_TSSALPHA_DECALALPHA_MASK_12 0x00FF00FF
#define DX5_TSSALPHA_DECALALPHA_1       ((D3DTA_DIFFUSE << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSALPHA_DECALALPHA_2       ((D3DTA_CURRENT << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSALPHA_DECALALPHA_MASK_34 0x00FFFF00
#define DX5_TSSALPHA_DECALALPHA_3       ((D3DTA_DIFFUSE << 8) | (D3DTOP_SELECTARG2 << 16))
#define DX5_TSSALPHA_DECALALPHA_4       ((D3DTA_CURRENT << 8) | (D3DTOP_SELECTARG2 << 16))

/*
 * DX5 D3DTBLEND_MODULATE equivalent texture stage state.
 */
#define DX5_TSSCOLOR_MODULATE_MASK      0x00FFFFFF
#define DX5_TSSCOLOR_MODULATE_1         ((D3DTA_TEXTURE << 0) | (D3DTA_DIFFUSE << 8) | (D3DTOP_MODULATE << 16))
#define DX5_TSSCOLOR_MODULATE_2         ((D3DTA_TEXTURE << 0) | (D3DTA_CURRENT << 8) | (D3DTOP_MODULATE << 16))
#define DX5_TSSCOLOR_MODULATE_3         ((D3DTA_DIFFUSE << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_MODULATE << 16))
#define DX5_TSSCOLOR_MODULATE_4         ((D3DTA_CURRENT << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_MODULATE << 16))


#define DX5_TSSALPHA_MODULATE_MASK_1    0x00FF00FF
#define DX5_TSSALPHA_MODULATE_1         ((D3DTA_TEXTURE << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSALPHA_MODULATE_MASK_2    0x00FFFF00
#define DX5_TSSALPHA_MODULATE_2         ((D3DTA_TEXTURE << 8) | (D3DTOP_SELECTARG2 << 16))

#define DX5_TSSALPHA_MODULATE_MASK_34   0x00FFFF00
#define DX5_TSSALPHA_MODULATE_3         ((D3DTA_DIFFUSE << 8) | (D3DTOP_SELECTARG2 << 16))
#define DX5_TSSALPHA_MODULATE_4         ((D3DTA_CURRENT << 8) | (D3DTOP_SELECTARG2 << 16))

#define DX5_TSSALPHA_MODULATE_MASK_56   0x00FF00FF
#define DX5_TSSALPHA_MODULATE_5         ((D3DTA_DIFFUSE << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSALPHA_MODULATE_6         ((D3DTA_CURRENT << 0) | (D3DTOP_SELECTARG1 << 16))

/*
 * DX5 D3DTBLEND_MODULATEALPHA equivalent texture stage state.
 */
#define DX5_TSSALPHA_MODULATEALPHA_MASK 0x00FFFFFF
#define DX5_TSSALPHA_MODULATEALPHA_1    ((D3DTA_TEXTURE << 0) | (D3DTA_DIFFUSE << 8) | (D3DTOP_MODULATE << 16))
#define DX5_TSSALPHA_MODULATEALPHA_2    ((D3DTA_TEXTURE << 0) | (D3DTA_CURRENT << 8) | (D3DTOP_MODULATE << 16))
#define DX5_TSSALPHA_MODULATEALPHA_3    ((D3DTA_DIFFUSE << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_MODULATE << 16))
#define DX5_TSSALPHA_MODULATEALPHA_4    ((D3DTA_CURRENT << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_MODULATE << 16))

/*
 * DX5 D3DTBLEND_ADD equivalent texture stage state.
 */
#define DX5_TSSCOLOR_ADD_MASK           0x00FFFFFF
#define DX5_TSSCOLOR_ADD_1              ((D3DTA_TEXTURE << 0) | (D3DTA_DIFFUSE << 8) | (D3DTOP_ADD << 16))
#define DX5_TSSCOLOR_ADD_2              ((D3DTA_TEXTURE << 0) | (D3DTA_CURRENT << 8) | (D3DTOP_ADD << 16))
#define DX5_TSSCOLOR_ADD_3              ((D3DTA_DIFFUSE << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_ADD << 16))
#define DX5_TSSCOLOR_ADD_4              ((D3DTA_CURRENT << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_ADD << 16))

#define DX5_TSSALPHA_ADD_MASK_12        0x00FF00FF
#define DX5_TSSALPHA_ADD_1              ((D3DTA_DIFFUSE << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSALPHA_ADD_2              ((D3DTA_CURRENT << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSALPHA_ADD_MASK_34        0x00FFFF00
#define DX5_TSSALPHA_ADD_3              ((D3DTA_DIFFUSE << 8) | (D3DTOP_SELECTARG2 << 16))
#define DX5_TSSALPHA_ADD_4              ((D3DTA_CURRENT << 8) | (D3DTOP_SELECTARG2 << 16))

/*
 * DX5 D3DTBLEND_MODULATE equivalent texture stage state
 *  because AlphaOp is DISABLED (works only for textures without alpha)
 */
#define DX5_TSSCOLOR_0AOP1_MASK_1234    0x00FFFFFF
#define DX5_TSSCOLOR_0AOP1_1            ((D3DTA_DIFFUSE << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_MODULATE << 16))
#define DX5_TSSCOLOR_0AOP1_2            ((D3DTA_CURRENT << 0) | (D3DTA_TEXTURE << 8) | (D3DTOP_MODULATE << 16))
#define DX5_TSSCOLOR_0AOP1_3            ((D3DTA_DIFFUSE << 8) | (D3DTA_TEXTURE << 0) | (D3DTOP_MODULATE << 16))
#define DX5_TSSCOLOR_0AOP1_4            ((D3DTA_CURRENT << 8) | (D3DTA_TEXTURE << 0) | (D3DTOP_MODULATE << 16))

#define DX5_TSSALPHA_0AOP1_MASK_1       0x00FF0000
#define DX5_TSSALPHA_0AOP1_1            (D3DTOP_DISABLE << 16)

/*
 * DX5 D3DTBLEND_COPY equivalent texture stage state
 *  because AlphaOp is DISABLED (works only for textures without alpha)
 */
#define DX5_TSSCOLOR_0AOP2_MASK_1       0x00FF00FF
#define DX5_TSSCOLOR_0AOP2_1            ((D3DTA_TEXTURE << 0) | (D3DTOP_SELECTARG1 << 16))
#define DX5_TSSCOLOR_0AOP2_MASK_2       0x00FFFF00
#define DX5_TSSCOLOR_0AOP2_2            ((D3DTA_TEXTURE << 8) | (D3DTOP_SELECTARG2 << 16))

#define DX5_TSSALPHA_0AOP2_MASK_1        0x00FF0000
#define DX5_TSSALPHA_0AOP2_1             (D3DTOP_DISABLE << 16)

//-------------------------------------------------------------------------------------

BOOL nvConvertTextureStageToDX5Class
(
    PNVD3DCONTEXT pContext
)
{
    BOOL                    bAlphaTexture;
    DWORD                   tssAlphaBlend, tssColorBlend;
    DWORD                   dwMinFilter, dwMagFilter;
    DWORD                   dwTBlend;
    DWORD                   dwTexture;
    CTexture               *pTexture;
    PNVD3DTEXSTAGESTATE     ptssState0;
    PNVD3DMULTITEXTURESTATE pmtsState;

    dbgTracePush ("nvConvertTextureStageToDX5Class");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    ptssState0 = &pContext->tssState[0];
    pmtsState  = &pContext->mtsState;

    tssAlphaBlend = ((ptssState0->dwValue[D3DTSS_ALPHAARG1] & 0x000000FF) << 0)
                  | ((ptssState0->dwValue[D3DTSS_ALPHAARG2] & 0x000000FF) << 8)
                  | ((ptssState0->dwValue[D3DTSS_ALPHAOP]   & 0x000000FF) << 16);
    tssColorBlend = ((ptssState0->dwValue[D3DTSS_COLORARG1] & 0x000000FF) << 0)
                  | ((ptssState0->dwValue[D3DTSS_COLORARG2] & 0x000000FF) << 8)
                  | ((ptssState0->dwValue[D3DTSS_COLOROP]   & 0x000000FF) << 16);

    /*
     * Save off the current texture handle.  This may be changed under some circumstances.
     * MUST BE RESTORED BEFORE EXITING ROUTINE.
     */
    dwTexture = pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP];
    pTexture  = dwTexture ? ((CNvObject *)dwTexture)->getTexture() : NULL;
    if (pTexture) {
        bAlphaTexture = pTexture->hasAlpha();
    }
    else {
        if(dwTexture){ //the pnvObj is valid but the texture deref isn't -- probably a sysmem texture
            //this is HACKY -- I'm borrowing a routine from elsewhere dcr
            nvFailMultiTextureSetup(pContext, D3DERR_CONFLICTINGRENDERSTATE);
            return TRUE;
        }
        bAlphaTexture = FALSE;
    }

    /*
     * Determine if the single texture stage can be converted to a DX5 TEXTUREMAPBLEND state.
     */
    if (ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
    {
        if ((((tssColorBlend & DX5_TSSCOLOR_DECAL_MASK_1) == DX5_TSSCOLOR_DECAL_1) ||
             ((tssColorBlend & DX5_TSSCOLOR_DECAL_MASK_2) == DX5_TSSCOLOR_DECAL_2))
         && (((tssAlphaBlend & DX5_TSSALPHA_DECAL_MASK_1) == DX5_TSSALPHA_DECAL_1) ||
             ((tssAlphaBlend & DX5_TSSALPHA_DECAL_MASK_2) == DX5_TSSALPHA_DECAL_2)))
        {
            /*
             * TEXTUREMAPBLEND equivalent is DECAL.
             */
            dwTBlend = D3DTBLEND_DECAL;
        }
        else if ((((tssColorBlend & DX5_TSSCOLOR_DECALALPHA_MASK) == DX5_TSSCOLOR_DECALALPHA_1) ||
                  ((tssColorBlend & DX5_TSSCOLOR_DECALALPHA_MASK) == DX5_TSSCOLOR_DECALALPHA_2) ||
                  ((tssColorBlend & DX5_TSSCOLOR_DECALALPHA_MASK) == DX5_TSSCOLOR_DECALALPHA_3) ||
                  ((tssColorBlend & DX5_TSSCOLOR_DECALALPHA_MASK) == DX5_TSSCOLOR_DECALALPHA_4))
              && (((tssAlphaBlend & DX5_TSSALPHA_DECALALPHA_MASK_12) == DX5_TSSALPHA_DECALALPHA_1) ||
                  ((tssAlphaBlend & DX5_TSSALPHA_DECALALPHA_MASK_12) == DX5_TSSALPHA_DECALALPHA_2) ||
                  ((tssAlphaBlend & DX5_TSSALPHA_DECALALPHA_MASK_34) == DX5_TSSALPHA_DECALALPHA_3) ||
                  ((tssAlphaBlend & DX5_TSSALPHA_DECALALPHA_MASK_34) == DX5_TSSALPHA_DECALALPHA_4)))
        {
            /*
             * TEXTUREMAPBLEND equivalent is DECALALPHA.
             */
            dwTBlend = D3DTBLEND_DECALALPHA;
        }
        else if ((((tssColorBlend & DX5_TSSCOLOR_ADD_MASK) == DX5_TSSCOLOR_ADD_1) ||
                  ((tssColorBlend & DX5_TSSCOLOR_ADD_MASK) == DX5_TSSCOLOR_ADD_2) ||
                  ((tssColorBlend & DX5_TSSCOLOR_ADD_MASK) == DX5_TSSCOLOR_ADD_3) ||
                  ((tssColorBlend & DX5_TSSCOLOR_ADD_MASK) == DX5_TSSCOLOR_ADD_4))
              && (((tssAlphaBlend & DX5_TSSALPHA_ADD_MASK_12) == DX5_TSSALPHA_ADD_1) ||
                  ((tssAlphaBlend & DX5_TSSALPHA_ADD_MASK_12) == DX5_TSSALPHA_ADD_2) ||
                  ((tssAlphaBlend & DX5_TSSALPHA_ADD_MASK_34) == DX5_TSSALPHA_ADD_3) ||
                  ((tssAlphaBlend & DX5_TSSALPHA_ADD_MASK_34) == DX5_TSSALPHA_ADD_4)))
        {
            /*
             * TEXTUREMAPBLEND equivalent is ADD.
             */
            dwTBlend = D3DTBLEND_ADD;
        }
        else if (((tssColorBlend & DX5_TSSCOLOR_MODULATE_MASK) == DX5_TSSCOLOR_MODULATE_1) ||
                 ((tssColorBlend & DX5_TSSCOLOR_MODULATE_MASK) == DX5_TSSCOLOR_MODULATE_2) ||
                 ((tssColorBlend & DX5_TSSCOLOR_MODULATE_MASK) == DX5_TSSCOLOR_MODULATE_3) ||
                 ((tssColorBlend & DX5_TSSCOLOR_MODULATE_MASK) == DX5_TSSCOLOR_MODULATE_4))
        {
            if (((tssAlphaBlend & DX5_TSSALPHA_MODULATEALPHA_MASK) == DX5_TSSALPHA_MODULATEALPHA_1) ||
                ((tssAlphaBlend & DX5_TSSALPHA_MODULATEALPHA_MASK) == DX5_TSSALPHA_MODULATEALPHA_2) ||
                ((tssAlphaBlend & DX5_TSSALPHA_MODULATEALPHA_MASK) == DX5_TSSALPHA_MODULATEALPHA_3) ||
                ((tssAlphaBlend & DX5_TSSALPHA_MODULATEALPHA_MASK) == DX5_TSSALPHA_MODULATEALPHA_4))
            {
                /*
                 * TEXTUREMAPBLEND equivalent is MODULATEALPHA.
                 */
                dwTBlend = D3DTBLEND_MODULATEALPHA;
            }
            else if (((tssAlphaBlend & DX5_TSSALPHA_MODULATE_MASK_1) == DX5_TSSALPHA_MODULATE_1) ||
                     ((tssAlphaBlend & DX5_TSSALPHA_MODULATE_MASK_2) == DX5_TSSALPHA_MODULATE_2))
            {
                /*
                 * TEXTUREMAPBLEND is equivalent to the HW's MODULATE (not exactly the same as D3D's MODULATE)
                 * unless there is no texture in which case we want to pull the alpha component from diffuse.
                 */
                if (pTexture) {
                    dwTBlend = D3DTBLEND_MODULATE;
                }
                else {
                    dwTBlend = D3DTBLEND_MODULATEALPHA;
                }
            }
            else if (((!bAlphaTexture) || (!pTexture))
                  && (((tssAlphaBlend & DX5_TSSALPHA_MODULATE_MASK_34) == DX5_TSSALPHA_MODULATE_3) ||
                      ((tssAlphaBlend & DX5_TSSALPHA_MODULATE_MASK_34) == DX5_TSSALPHA_MODULATE_4) ||
                      ((tssAlphaBlend & DX5_TSSALPHA_MODULATE_MASK_56) == DX5_TSSALPHA_MODULATE_5) ||
                      ((tssAlphaBlend & DX5_TSSALPHA_MODULATE_MASK_56) == DX5_TSSALPHA_MODULATE_6)))
            {
                /*
                 * When the TEXTUREMAPBLEND equivalent is MODULATE but the texture format does
                 * not have an alpha component, need to program the DX5 class as MODULATEALPHA.
                 */
                dwTBlend = D3DTBLEND_MODULATEALPHA;
            }
            else if (pTexture && !bAlphaTexture
                  && ((tssAlphaBlend & DX5_TSSALPHA_0AOP1_MASK_1) == DX5_TSSALPHA_0AOP1_1)
                  && (((tssColorBlend & DX5_TSSCOLOR_0AOP1_MASK_1234) == DX5_TSSCOLOR_0AOP1_1)
                   || ((tssColorBlend & DX5_TSSCOLOR_0AOP1_MASK_1234) == DX5_TSSCOLOR_0AOP1_2)
                   || ((tssColorBlend & DX5_TSSCOLOR_0AOP1_MASK_1234) == DX5_TSSCOLOR_0AOP1_3)
                   || ((tssColorBlend & DX5_TSSCOLOR_0AOP1_MASK_1234) == DX5_TSSCOLOR_0AOP1_4)))
            {
                /*
                 * when we have a non-alpha texture, alpha op is disabled and colorop is modulate
                 */
                dwTBlend = D3DTBLEND_MODULATE;
            }
            else {
                dbgTracePop();
                return (FALSE);
            }
        }
        else if (pTexture && !bAlphaTexture
              && ((tssAlphaBlend & DX5_TSSALPHA_0AOP2_MASK_1) == DX5_TSSALPHA_0AOP2_1)
              && (((tssColorBlend & DX5_TSSCOLOR_0AOP2_MASK_1) == DX5_TSSCOLOR_0AOP2_1)
               || ((tssColorBlend & DX5_TSSCOLOR_0AOP2_MASK_2) == DX5_TSSCOLOR_0AOP2_2)))
        {
            /*
             * when we have a non-alpha texture, alpha op is disabled and colorop is only texture
             */
            dwTBlend = D3DTBLEND_COPY;
        }
        else {
            dbgTracePop();
            return (FALSE);
        }
    }
    else
    {
        /*
         * When stage 0 is disabled, force DX5 setup with no texture.
         */
        pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = 0;
        nvCalculateHardwareStateDX5 (pContext, D3DTBLEND_MODULATEALPHA, D3DFILTER_NEAREST, D3DFILTER_NEAREST, TRUE);
        pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = dwTexture;
        dbgTracePop();
        return (TRUE);
    }

    /*
     * Calculate the MIN and MAG filters from the texture stage state.
     */
    /*
     * Need to calculate the MIN and MAG filters.
     */
    switch (ptssState0->dwValue[D3DTSS_MAGFILTER])
    {
        case D3DTFG_POINT:
            dwMagFilter = D3DFILTER_NEAREST;
            break;
        case D3DTFG_LINEAR:
            dwMagFilter = D3DFILTER_LINEAR;
            break;
        default:
            dwMagFilter = D3DFILTER_LINEAR;
            break;
    }
    if (ptssState0->dwValue[D3DTSS_MIPFILTER] == D3DTFP_NONE)
    {
        switch (ptssState0->dwValue[D3DTSS_MINFILTER])
        {
            case D3DTFN_POINT:
                dwMinFilter = D3DFILTER_NEAREST;
                break;
            case D3DTFN_LINEAR:
                dwMinFilter = D3DFILTER_LINEAR;
                break;
            default:
                dwMinFilter = D3DFILTER_LINEAR;
                break;
        }
    }
    else if (ptssState0->dwValue[D3DTSS_MIPFILTER] == D3DTFP_POINT)
    {
        switch (ptssState0->dwValue[D3DTSS_MINFILTER])
        {
            case D3DTFN_POINT:
                dwMinFilter = D3DFILTER_MIPNEAREST;
                break;
            case D3DTFN_LINEAR:
                dwMinFilter = D3DFILTER_MIPLINEAR;
                break;
            default:
                dwMinFilter = D3DFILTER_MIPLINEAR;
                break;
        }
    }
    else
    {
        switch (ptssState0->dwValue[D3DTSS_MINFILTER])
        {
            case D3DTFN_POINT:
                dwMinFilter = D3DFILTER_LINEARMIPNEAREST;
                break;
            case D3DTFN_LINEAR:
                dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                break;
            default:
                dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                break;
        }
    }

    /*
     * Need to track down the mipmap level that the application wants to start from.
     */
    if ((pTexture)
     && (pTexture->getMipMapCount() > 1)
     && (ptssState0->dwValue[D3DTSS_MAXMIPLEVEL]))
    {
        DWORD dwMaxLevel;

        // Mipmap levels are numbered from 0 through (pTexture->MipMapLevels - 1).
        // If the application is asking us to start at a level beyond this range,
        // we must limit it to be within this range.
        dwMaxLevel = ptssState0->dwValue[D3DTSS_MAXMIPLEVEL];
        if (dwMaxLevel >= pTexture->getMipMapCount()) {
            dwMaxLevel = pTexture->getMipMapCount() - 1;
        }

        for (DWORD i=0; i<dwMaxLevel; i++) {
            pTexture = pTexture->getWrapper()->getAttachedA()->getTexture();
        }

        // Update the render state handle for the starting mipmap level.
        pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = (DWORD)(pTexture->getWrapper());
    }

    /*
     * All other state comes directly from the renderstate.
     * Build up the DX5 control state.
     */
    nvCalculateHardwareStateDX5 (pContext, dwTBlend, dwMinFilter, dwMagFilter, TRUE);

    /*
     * Restore the original texture handle.
     */
    pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = dwTexture;

    /*
     * Validate the inner loop hardware control registers.
     */
    dbgValidateControlRegisters(pContext);

    /*
     * This code is only enabled in the DEBUG build.
     * Display the hardware registers that were calculated.
     */
    dbgDisplayContextState(NVDBG_SHOW_DX5_CLASS_STATE, pContext);

    /*
     * Determine the UV coordinate offset for the first stage.
     */
    pmtsState->dwUVOffset[0] = (2 * sizeof(D3DVALUE)) * (ptssState0->dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff);

    dbgTracePop();
    return (TRUE);
}

//-------------------------------------------------------------------------------------

void nvSetDx6MultiTextureState
(
    PNVD3DCONTEXT   pContext
)
{
    NVD3DMULTITEXTURESTATE *pState;
    NVD3DMULTITEXTURESTATE *pShadowState;

    dbgTracePush ("nvSetDx6MultiTextureState");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    /*
     * setup locals
     */
    pState       = &pContext->mtsState;
    pShadowState = &pContext->mtsShadowState;

    /*
     * Only write channel if it is not selected - avoids texture cache invalidates
     */
    if (pDriverData->dDrawSpareSubchannelObject != D3D_DX6_MULTI_TEXTURE_TRIANGLE)
    {
        /*
         * select object
         */
        nvglSetObject(NV_DD_SPARE, D3D_DX6_MULTI_TEXTURE_TRIANGLE);

        /*
         * program everything
         */
        nvglDx6TriangleOffset(NV_DD_SPARE,
                              pState->dwTextureOffset[0], 0);
        nvglDx6TriangleOffset(NV_DD_SPARE,
                              pState->dwTextureOffset[1], 1);
        nvglDx6TriangleFormat(NV_DD_SPARE,
                              pState->dwTextureFormat[0], 0);
        nvglDx6TriangleFormat(NV_DD_SPARE,
                              pState->dwTextureFormat[1], 1);
        nvglDx6TriangleFilter(NV_DD_SPARE,
                              pState->dwTextureFilter[0], 0);
        nvglDx6TriangleFilter(NV_DD_SPARE,
                              pState->dwTextureFilter[1], 1);
        nvglDx6TriangleStageState(NV_DD_SPARE,
                                  pState->dwCombine0Alpha, pState->dwCombine0Color,
                                  pState->dwCombine1Alpha, pState->dwCombine1Color,
                                  pState->dwCombineFactor, pState->dwBlend,
                                  pState->dwControl0,      pState->dwControl1,
                                  pState->dwControl2,      pState->dwFogColor);

        pDriverData->dDrawSpareSubchannelObject = D3D_DX6_MULTI_TEXTURE_TRIANGLE;

        /*
         * cache everything
         */
        *pShadowState = *pState;
    }
    else
    {
        if (pShadowState->dwTextureOffset[0] != pState->dwTextureOffset[0])
        {
            nvglDx6TriangleOffset(NV_DD_SPARE,
                                  pState->dwTextureOffset[0], 0);

            pShadowState->dwTextureOffset[0] = pState->dwTextureOffset[0];
        }

        if (pShadowState->dwTextureOffset[1] != pState->dwTextureOffset[1])
        {
            nvglDx6TriangleOffset(NV_DD_SPARE,
                                  pState->dwTextureOffset[1], 1);

            pShadowState->dwTextureOffset[1] = pState->dwTextureOffset[1];
        }

        if (pShadowState->dwTextureFormat[0] != pState->dwTextureFormat[0])
        {
            nvglDx6TriangleFormat(NV_DD_SPARE,
                                  pState->dwTextureFormat[0], 0);

            pShadowState->dwTextureFormat[0] = pState->dwTextureFormat[0];
        }

        if (pShadowState->dwTextureFormat[1] != pState->dwTextureFormat[1])
        {
            nvglDx6TriangleFormat(NV_DD_SPARE,
                                  pState->dwTextureFormat[1], 1);

            pShadowState->dwTextureFormat[1] = pState->dwTextureFormat[1];
        }

        if (pShadowState->dwTextureFilter[0] != pState->dwTextureFilter[0])
        {
            nvglDx6TriangleFilter(NV_DD_SPARE,
                                  pState->dwTextureFilter[0], 0);

            pShadowState->dwTextureFilter[0] = pState->dwTextureFilter[0];
        }

        if (pShadowState->dwTextureFilter[1] != pState->dwTextureFilter[1])
        {
            nvglDx6TriangleFilter(NV_DD_SPARE,
                                  pState->dwTextureFilter[1], 1);

            pShadowState->dwTextureFilter[1] = pState->dwTextureFilter[1];
        }

        /*
         * must be evaluated last
         */
        if ((pShadowState->dwCombine0Alpha != pState->dwCombine0Alpha)
         || (pShadowState->dwCombine0Color != pState->dwCombine0Color)
         || (pShadowState->dwCombine1Alpha != pState->dwCombine1Alpha)
         || (pShadowState->dwCombine1Color != pState->dwCombine1Color)
         || (pShadowState->dwCombineFactor != pState->dwCombineFactor)
         || (pShadowState->dwBlend         != pState->dwBlend        )
         || (pShadowState->dwControl0      != pState->dwControl0     )
         || (pShadowState->dwControl1      != pState->dwControl1     )
         || (pShadowState->dwControl2      != pState->dwControl2     )
         || (pShadowState->dwFogColor      != pState->dwFogColor     ))
        {
            nvglDx6TriangleStageState(NV_DD_SPARE,
                                      pState->dwCombine0Alpha, pState->dwCombine0Color,
                                      pState->dwCombine1Alpha, pState->dwCombine1Color,
                                      pState->dwCombineFactor, pState->dwBlend,
                                      pState->dwControl0,      pState->dwControl1,
                                      pState->dwControl2,      pState->dwFogColor);

            /*
             * cache everything - this is why this part is last
             */
            *pShadowState = *pState;
        }
    }

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

DWORD nv4ValidateTextureStageState
(
    LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA pvtssd
)
{
    dbgTracePush ("nv4ValidateTextureStageState");

   // get the context
   PNVD3DCONTEXT pContext;
   NV_SET_CONTEXT (pContext, pvtssd);

   //initialize this to DD_OK so I know if a subroutine failed when it doesn't match
   pContext->mtsState.ddrval = DD_OK;

   nvSetMultiTextureHardwareState (pContext);
   pvtssd->ddrval = pContext->mtsState.ddrval;

   DWORD dwStageCount = 0;
   PNVD3DTEXSTAGESTATE     ptssState0= &pContext->tssState[dwStageCount];
    while ((dwStageCount < 8)
        && (ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE))
    {
        dwStageCount++;
        if (dwStageCount < 8)
            ptssState0 = &pContext->tssState[dwStageCount];
    }

    /*
     * If returning DD_OK, then we can do this operation in 1 pass.
     * If returning an error, then who the heck knows.  Return an obnoxious number.
     */
    if (pvtssd->ddrval == DD_OK)
    {
        /*
         * Hardware setup didn't catch any problems, but there may be some things that we don't support that
         * hardware setup doesn't catch.  Check for them now.
         */
        for (DWORD i=0; i<dwStageCount; i++)
        {
            if ((pContext->tssState[i].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
             && ((pContext->tssState[i].dwValue[D3DTSS_ADDRESSU] == D3DTADDRESS_BORDER)
              || (pContext->tssState[i].dwValue[D3DTSS_ADDRESSV] == D3DTADDRESS_BORDER)))
            {
                pvtssd->ddrval      = D3DERR_CONFLICTINGRENDERSTATE;
                pvtssd->dwNumPasses = 0xFFFFFFFF;
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }
        }
        /*
         * If the DX6 class is being used and colorkey is enabled, return an error.
         */
        if ((pContext->bUseDX6Class)
         && (pContext->dwRenderState[D3DRENDERSTATE_COLORKEYENABLE]))
        {
            pvtssd->ddrval      = D3DERR_CONFLICTINGRENDERSTATE;
            pvtssd->dwNumPasses = 0xFFFFFFFF;
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }


        if ((pContext->bUseDX6Class) && (pContext->dwDXAppVersion >= 0x0700) &&
            (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ENFORCESTRICTTRILINEAR_MASK)
            &&(dwStageCount>1))
        {
            // if we are multitexturing and trilinear filtering we've chosen to use a dither
            // approximation for performance reasons.  Invalidate this texture state operation
            // on dx7 apps so winbench2k doesn't complain.
            DWORD j,textureUsed;
            BOOL triLinear;
            triLinear=FALSE;

            for (j=0, textureUsed=0; j<(dwStageCount); j++){
                if (pContext->tssState[j].dwValue[D3DTSS_MIPFILTER] == D3DTFP_LINEAR)
                {
                    triLinear=TRUE;
                }
                if((((pContext->tssState[j].dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_TEXTURE)
                    &&(pContext->tssState[j].dwValue[D3DTSS_COLOROP]!=D3DTOP_SELECTARG2))
                    ||(((pContext->tssState[j].dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_TEXTURE)
                    &&(pContext->tssState[j].dwValue[D3DTSS_COLOROP]!=D3DTOP_SELECTARG1)))
                {
                    textureUsed++;
                }
            }
            if((textureUsed>1)&&triLinear)
            {
                pvtssd->ddrval      = D3DERR_UNSUPPORTEDTEXTUREFILTER;
                pvtssd->dwNumPasses = 0xFFFFFFFF;
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }
        }

        pvtssd->dwNumPasses = 1;
    }

    else {
        pvtssd->dwNumPasses = 0xFFFFFFFF;
    }

    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//-------------------------------------------------------------------------------------

/*
 * Validate the multi-texture rendering state.
 */
DWORD FAR PASCAL nvValidateTextureStageState
(
    LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA pvtssd
)
{
    DWORD dwRetVal;

    dbgTracePush ("nvValidateTextureStageState");

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pvtssd);
    nvSetDriverDataPtrFromContext (pContext);

    // lock textures since the validate code programs the HW with them
    // and we need to make sure they're in HW-readable memory
    nvHWLockTextures (pContext);

    CNvObject *pRTObj = nvGetObjectFromHandle(pContext->dwDDLclID, pContext->dwRTHandle, &global.pNvSurfaceLists);
    if (pRTObj && pRTObj->isDirty())
    {
        // reset the render target
        nvSetRenderTargetDX7(pContext, pContext->dwRTHandle, pContext->dwZBHandle);
        // make sure the AA info is also updated
#if (NVARCH >= 0x010)
        if (pContext->aa.isEnabled()) nvCelsiusAADestroy(pContext);
#endif  // NVARCH >= 0x010

#if (NVARCH >= 0x020)
        pContext->kelvinAA.Refresh();
#endif  // NVARCH >= 0x020

    }

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        dwRetVal = nvKelvinValidateTextureStageState (pvtssd);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        dwRetVal = nvCelsiusValidateTextureStageState (pvtssd);
    }
    else
#endif
    {
        dwRetVal = nv4ValidateTextureStageState (pvtssd);
    }

    nvHWUnlockTextures (pContext);


    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (dwRetVal);
}

/**************************** DX6 Multi Texture Setup************************\
*
* In all cases we can use one combiner for each TSS (when they map).
* So we can advertise that we handle 2 textures and 2 stages.
*
* At some point we may want to consider setup of the hardware to support
* 3 stages and sometimes even 4 stages into our 2 combiners.
*
* D3DTA_FACTOR    factor
* D3DTA_DIFFUSE   diffuse
* D3DTA_SPECULAR  ** error not supported **
* D3DTA_CURRENT   diffuse
* D3DTA_TEXTURE   Texture 0
*
* DX6 arg in stage 1 & ~(D3DTA_ALPHA | D3DTA_INV) NV4 arg for combiner 1
*
* D3DTA_FACTOR    factor
* D3DTA_DIFFUSE   diffuse
* D3DTA_SPECULAR  ** error not supported **
* D3DTA_CURRENT   input
* D3DTA_TEXTURE   Texture 1
*
* If (DX6arg & D3DTA_ALPHA) NV4arg |= alpha
* If (DX6arg & D3DTA_INV) NV4arg |= inv
*
* Map the DX6 Arguments and operations as follows:
*
* DX6op                     | NV4op         | NV4arg1            | NV4arg2          | NV4arg3      | NV4arg4
* ==========================+===============+====================+==================+==============+===============
* Selectarg1                | Add           | Arg1               | Inv | zero       | Zero         | Zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Selectarg2                | Add           | Arg2               | Inv | zero       | Zero         | Zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Modulate                  | Add           | Arg1               | Arg2             | Zero         | Zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Modulate2x                | Add           | Arg1               | Arg2             | Arg1         | Arg2
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Modulate4x                | Add2          | Arg1               | Arg2             | Arg1         | Arg2
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Add                       | Add           | Arg1               | Inv | zero       | Arg2         | Inv | zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Addsigned                 | Addsigned     | Arg1               | Inv | zero       | Arg2         | Inv | zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Addsigned2x               | Addsigned2    | Arg1               | Inv | zero       | Arg2         | Inv | zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Addsmoth                  | Add           | Arg1               | Inv | zero       | Inv | Arg1   | Arg2
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Subtract                  | addcomplement | Arg1 | inv         | Inv | zero       | Arg2         | Inv | zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* blenddiffusealpha         | Add           | Arg1               | Alpha | diffuse  | Arg2         | Inv | alpha |
*                           |               |                    |                  |              | diffuse
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Blendtexturealpha         | Add           | Arg1               | Alpha | tex<0|1> | Arg2         | Inv | alpha |
*                           |               |                    |                  |              | tex<0|1>
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Blendfactoralpha          | Add           | Arg1               | Alpha | factor   | Arg2         | Inv | alpha |
*                           |               |                    |                  |              | factor
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Blendtexturealphapm       | Add           | Arg1               | Inv | zero       | Arg2         | Inv | alpha |
*                           |               |                    |                  |              | texture
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* Blendcurrentalpha         | Add           | Arg1               | Alpha | input    | Arg2         | Inv | alpha |
*                           |               |                    |                  |              | diffuse
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* premodulate               | ** error **   |                    |                  |              |
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* MODULATEALPHA_ADDCOLOR    | Add           | Arg1 | alpha       | Arg2             | Arg1         | Inv | zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* MODULATECOLOR_ADDALPHA    | Add           | Arg1               | Arg2             | Arg1 | alpha | Inv | zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* MODULATEINVALPHA_ADDCOLOR | Add           | Arg1 | alpha | inv | Arg2             | Arg1         | Inv | zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* MODULATEINVCOLOR_ADDALPHA | Add           | Arg1 | inv         | Arg2             | Arg1 | alpha | Inv | zero
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* BUMPENVMAP                | ** error **   |                    |                  |              |
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* BUMPENVMAPLUMINANCE       | ** error **   |                    |                  |              |
* --------------------------+---------------+--------------------+------------------+--------------+---------------
* BUMPMAPLIGHT              | ** error **   |                    |                  |              |
* ==========================+===============+====================+==================+==============+===============
*
\****************************************************************************/

BOOL nvStencilBufferExists
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvStencilBufferExists");

    // on pre-kelvin HW, there can't be a stencil buffer if the render target is only 16bpp
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)) {
        nvAssert (pContext->pRenderTarget);
        if (pContext->pRenderTarget->getBPP() == 2) {
            dbgTracePop();
            return (FALSE);
        }
    }

    // if there is no Z-Buffer, then there can't be a stencil buffer.
    if (!pContext->pZetaBuffer) {
        dbgTracePop();
        return (FALSE);
    }

    BOOL bRV = pContext->pZetaBuffer->hasStencil();
    dbgTracePop();
    return (bRV);
}

//-------------------------------------------------------------------------------------

// returns true is a stencil buffer exists and is enabled

BOOL nvStencilBufferInUse (PNVD3DCONTEXT pContext)
{
    dbgTracePush ("nvStencilBufferInUse");
    BOOL bRV = (pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE] &&
                nvStencilBufferExists(pContext));
    dbgTracePop();
    return (bRV);
}

//-------------------------------------------------------------------------------------

void nvSetDX6TextureState
(
    PNVD3DCONTEXT           pContext,
    DWORD                   dwTextureNum,
    PNVD3DTEXSTAGESTATE     ptssState,
    PNVD3DMULTITEXTURESTATE pmtsState
)
{
    DWORD           dwMagFilter;
    DWORD           dwMinFilter;
    DWORD           dwZOHMode, dwFOHMode;
    CNvObject      *pNvObj;
    CTexture       *pTexture;

    dbgTracePush ("nvSetDX6TextureState");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    /*
     * setup state
     */
    dwZOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_ZOH_MASK) == D3D_REG_TEXELALIGNMENT_ZOH_CENTER)
              ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
    dwFOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_FOH_MASK) == D3D_REG_TEXELALIGNMENT_FOH_CENTER)
              ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;

    pNvObj    = (CNvObject *)(ptssState->dwValue[D3DTSS_TEXTUREMAP]);
    pTexture  = pNvObj ? pNvObj->getTexture() : NULL;

    if (!pTexture){
        //if the texture is undefined -- its probably a sysmem texture bail
        nvFailMultiTextureSetup(pContext, D3DERR_CONFLICTINGRENDERSTATE);
        return;
    }

    if (pTexture)
    {
        if ((pTexture->getMipMapCount() > 1) && (ptssState->dwValue[D3DTSS_MAXMIPLEVEL]))
        {
            DWORD dwMaxLevel;

            // Mipmap levels are numbered from 0 through (pTexture->MipMapLevels - 1).
            // If the application is asking us to start at a level beyond this range,
            // we must limit it to be within this range.
            dwMaxLevel = ptssState->dwValue[D3DTSS_MAXMIPLEVEL];
            if (dwMaxLevel >= pTexture->getMipMapCount()) {
                dwMaxLevel = pTexture->getMipMapCount() - 1;
            }

            for (DWORD i=0; i<dwMaxLevel; i++) {
                pTexture = pTexture->getWrapper()->getAttachedA()->getTexture();
            }

            // Update the render state handle for the starting mipmap level.
            ptssState->dwValue[D3DTSS_TEXTUREMAP] = (DWORD)(pTexture->getWrapper());
        }

        DWORD dwTCIndex = ptssState->dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff;
        BOOL  bWrapU, bWrapV, bWrapP, bWrapQ;
        NV_EVAL_CYLINDRICAL_WRAP (pContext, dwTCIndex, bWrapU, bWrapV, bWrapP, bWrapQ);

        pmtsState->dwTextureOffset[dwTextureNum] = pTexture->getSwizzled()->getOffset();
        pmtsState->dwTextureFormat[dwTextureNum] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     nv055TextureContextDma[pTexture->getSwizzled()->getContextDMA()])
                                                 | DRF_NUM(055, _FORMAT, _COLOR,           nv055TextureFormat[pTexture->getFormat()])
                                                 | DRF_NUM(055, _FORMAT, _BASE_SIZE_U,     pTexture->getLogWidth())
                                                 | DRF_NUM(055, _FORMAT, _BASE_SIZE_V,     pTexture->getLogHeight())
                                                 | DRF_NUM(055, _FORMAT, _TEXTUREADDRESSU, ptssState->dwValue[D3DTSS_ADDRESSU])
                                                 | DRF_NUM(055, _FORMAT, _WRAPU,           bWrapU)
                                                 | DRF_NUM(055, _FORMAT, _TEXTUREADDRESSV, ptssState->dwValue[D3DTSS_ADDRESSV])
                                                 | DRF_NUM(055, _FORMAT, _WRAPV,           bWrapV);
        pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,     dwZOHMode)
                                                 |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,     dwFOHMode);

        /*
         * Need to calculate the MIN and MAG filters.
         */
        pmtsState->dwTextureFilter[dwTextureNum] = 0;  // initialize

        switch (ptssState->dwValue[D3DTSS_MAGFILTER])
        {
            case D3DTFG_POINT:
                dwMagFilter = D3DFILTER_NEAREST;
                break;
            case D3DTFG_LINEAR:
                dwMagFilter = D3DFILTER_LINEAR;
                break;
            case D3DTFG_ANISOTROPIC:
                dwMagFilter = D3DFILTER_LINEAR;
                pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(054, _FILTER, _KERNEL_SIZE_X,              0x1F)
                                                         |  DRF_NUM(054, _FILTER, _KERNEL_SIZE_Y,              0x1F)
                                                         |  DRF_DEF(054, _FILTER, _ANISOTROPIC_MAGNIFY_ENABLE, _TRUE);
                break;
            default:
                dwMagFilter = D3DFILTER_LINEAR;
                break;
        }

        if (ptssState->dwValue[D3DTSS_MIPFILTER] == D3DTFP_NONE)
        {
            switch (ptssState->dwValue[D3DTSS_MINFILTER])
            {
                case D3DTFN_POINT:
                    dwMinFilter = D3DFILTER_NEAREST;
                    break;
                case D3DTFN_LINEAR:
                    dwMinFilter = D3DFILTER_LINEAR;
                    break;
                case D3DTFN_ANISOTROPIC:
                    dwMinFilter = D3DFILTER_LINEAR;
                    pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(054, _FILTER, _KERNEL_SIZE_X,              0x1F)
                                                             |  DRF_NUM(054, _FILTER, _KERNEL_SIZE_Y,              0x1F)
                                                             |  DRF_DEF(054, _FILTER, _ANISOTROPIC_MINIFY_ENABLE,  _TRUE);
                    break;
                default:
                    dwMinFilter = D3DFILTER_LINEAR;
                    break;
            }
            pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, (DWORD)ptssState->dwLODBias);
        }
        else if (ptssState->dwValue[D3DTSS_MIPFILTER] == D3DTFP_POINT)
        {
            switch (ptssState->dwValue[D3DTSS_MINFILTER])
            {
                case D3DTFN_POINT:
                    dwMinFilter = D3DFILTER_MIPNEAREST;
                    break;
                case D3DTFN_LINEAR:
                    dwMinFilter = D3DFILTER_MIPLINEAR;
                    break;
                case D3DTFN_ANISOTROPIC:
                    dwMinFilter = D3DFILTER_MIPLINEAR;
                    pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(054, _FILTER, _KERNEL_SIZE_X,              0x1F)
                                                             |  DRF_NUM(054, _FILTER, _KERNEL_SIZE_Y,              0x1F)
                                                             |  DRF_DEF(054, _FILTER, _ANISOTROPIC_MINIFY_ENABLE,  _TRUE);
                    break;
                default:
                    dwMinFilter = D3DFILTER_MIPLINEAR;
                    break;
            }
            pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, (DWORD)ptssState->dwLODBias + 0x04);
        }
        else
        {
            switch (ptssState->dwValue[D3DTSS_MINFILTER])
            {
                case D3DTFN_POINT:
                    dwMinFilter = D3DFILTER_LINEARMIPNEAREST;
                    break;
                case D3DTFN_LINEAR:
                    dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                    break;
                case D3DTFN_ANISOTROPIC:
                    dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                    pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(054, _FILTER, _KERNEL_SIZE_X,              0x1F)
                                                             |  DRF_NUM(054, _FILTER, _KERNEL_SIZE_Y,              0x1F)
                                                             |  DRF_DEF(054, _FILTER, _ANISOTROPIC_MINIFY_ENABLE,  _TRUE);
                    break;
                default:
                    dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                    break;
            }
            pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, (DWORD)ptssState->dwLODBias + 0x04);
        }

        /*
         * When this is a user supplied mipmap chain
         * OR the number of mipmap levels is 1 (i.e. no mipmaps, only base texture)
         * set the MIN/MAG filters as specified by the render states.
         */

        if((dwMinFilter == D3DFILTER_LINEARMIPLINEAR) || (dwMinFilter == D3DFILTER_LINEARMIPNEAREST))
            pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE);
        else
            pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _FALSE);

        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _TEXTUREMIN, dwMinFilter)
                                                 |  DRF_NUM(055, _FILTER, _TEXTUREMAG, dwMagFilter);

        /*
         * The only way to guarantee that mipmapping is disabled is to set MIPMAP_LEVELS to 1.
         * In cases where there are the user has supplied mipmaps but the minification filter is
         * set to D3DFILTER_NEAREST or D3DFILTER_LINEAR, force the mipmap levels field to be a 1.
         */
        if ((dwMinFilter == D3DFILTER_NEAREST) || (dwMinFilter == D3DFILTER_LINEAR)) {
            pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS, 1);
        }
        else {
            pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS, pTexture->getMipMapCount());
        }

        dbgTextureDisplay (pTexture);
    }
    else
    {
        /*
         * There is no texture set the default texture format and offset.
         */
        pmtsState->dwTextureOffset[dwTextureNum] = getDC()->pDefaultTexture->getSwizzled()->getOffset();
        pmtsState->dwTextureFormat[dwTextureNum] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     nv055TextureContextDma[getDC()->pDefaultTexture->getSwizzled()->getContextDMA()])
                                                 | DRF_NUM(055, _FORMAT, _COLOR,           nv055TextureFormat[getDC()->pDefaultTexture->getFormat()])
                                                 | DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS,   1)
                                                 | DRF_DEF(055, _FORMAT, _BASE_SIZE_U,     _1)
                                                 | DRF_DEF(055, _FORMAT, _BASE_SIZE_V,     _1)
                                                 | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSU, _CLAMP)
                                                 | DRF_DEF(055, _FORMAT, _WRAPU,           _FALSE)
                                                 | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSV, _CLAMP)
                                                 | DRF_DEF(055, _FORMAT, _WRAPV,           _FALSE);
        pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,     dwZOHMode)
                                                 |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,     dwFOHMode);



        /*
         * When there's no texture, always set for nearest (point sample) filtering.
         */
        pmtsState->dwTextureFilter[dwTextureNum] = DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _FALSE)
                                                 | DRF_DEF(055, _FILTER, _TEXTUREMIN,           _NEAREST)
                                                 | DRF_DEF(055, _FILTER, _TEXTUREMAG,           _NEAREST);

        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, (DWORD)0);

    }


    /*
     * Determine the UV coordinate offset for this texture stage.
     */
    pmtsState->dwUVOffset[dwTextureNum] = (2 * sizeof(D3DVALUE)) * (ptssState->dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

void nvSetDX6State
(
    PNVD3DCONTEXT            pContext,
    DWORD                   *pdwRenderState,
    PNVD3DMULTITEXTURESTATE  pmtsState
)
{
    DWORD   dwAlphaRef;
    DWORD   dwTexelOrigin;

    dbgTracePush ("nvSetDX6State");

    dwTexelOrigin = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_TEXEL_MASK) == D3D_REG_TEXELALIGNMENT_TEXEL_CENTER)
                  ? NV054_CONTROL_ORIGIN_CENTER : NV054_CONTROL_ORIGIN_CORNER;

    /*
     * Set the Combine Factor.
     */
    pmtsState->dwCombineFactor = pdwRenderState[D3DRENDERSTATE_TEXTUREFACTOR];

    /*
     * Setup Blend control.
     */
    pmtsState->dwBlend = DRF_DEF(055, _BLEND, _MASK_BIT,           _MSB)
                       | DRF_NUM(055, _BLEND, _SHADEMODE,          D3DSHADE_GOURAUD/*pdwRenderState[D3DRENDERSTATE_SHADEMODE]*/) // inner loop takes care of flat shading
                       | DRF_NUM(055, _BLEND, _TEXTUREPERSPECTIVE, pdwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE])
                       | DRF_NUM(055, _BLEND, _SPECULARENABLE,     pdwRenderState[D3DRENDERSTATE_SPECULARENABLE])
                       | DRF_NUM(055, _BLEND, _FOGENABLE,          pdwRenderState[D3DRENDERSTATE_FOGENABLE])
                       | DRF_NUM(055, _BLEND, _ALPHABLENDENABLE,   pdwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]);

    if ((pdwRenderState[D3DRENDERSTATE_SRCBLEND]) == D3DBLEND_BOTHSRCALPHA)
        pmtsState->dwBlend |= DRF_DEF(055, _BLEND, _SRCBLEND,  _SRCALPHA)
                           |  DRF_DEF(055, _BLEND, _DESTBLEND, _INVSRCALPHA);
    else if ((pdwRenderState[D3DRENDERSTATE_SRCBLEND]) == D3DBLEND_BOTHINVSRCALPHA)
        pmtsState->dwBlend |= DRF_DEF(055, _BLEND, _SRCBLEND,  _INVSRCALPHA)
                           |  DRF_DEF(055, _BLEND, _DESTBLEND, _SRCALPHA);
    else
        pmtsState->dwBlend |= DRF_NUM(055, _BLEND, _SRCBLEND,  pdwRenderState[D3DRENDERSTATE_SRCBLEND])
                           |  DRF_NUM(055, _BLEND, _DESTBLEND, pdwRenderState[D3DRENDERSTATE_DESTBLEND]);

    /*
     * Setup Control0.
     */
    if (pdwRenderState[D3DRENDERSTATE_ALPHAREF] == 0x00010000)
        dwAlphaRef = 0xFF;
    else
        dwAlphaRef = (pdwRenderState[D3DRENDERSTATE_ALPHAREF] & 0x0000FF00)
                   ? (pdwRenderState[D3DRENDERSTATE_ALPHAREF] >> 8)
                   : (pdwRenderState[D3DRENDERSTATE_ALPHAREF] & 0x000000FF);
    pmtsState->dwControl0 = DRF_NUM(055, _CONTROL0, _ALPHAREF,             dwAlphaRef)
                          | DRF_NUM(055, _CONTROL0, _ALPHAFUNC,            pdwRenderState[D3DRENDERSTATE_ALPHAFUNC])
                          | DRF_NUM(055, _CONTROL0, _ALPHATESTENABLE,      pdwRenderState[D3DRENDERSTATE_ALPHATESTENABLE])
                          | DRF_NUM(055, _CONTROL0, _ORIGIN,               dwTexelOrigin)
                          | DRF_NUM(055, _CONTROL0, _ZFUNC,                CTFunc(pContext,pdwRenderState[D3DRENDERSTATE_ZFUNC]))
                          | DRF_NUM(055, _CONTROL0, _CULLMODE,             pdwRenderState[D3DRENDERSTATE_CULLMODE])
                          | DRF_NUM(055, _CONTROL0, _DITHERENABLE,         pdwRenderState[D3DRENDERSTATE_DITHERENABLE]);

    /*
     * Set Control0 z-buffer enable bits.
     */
    if ((pContext->pZetaBuffer) && (pdwRenderState[D3DRENDERSTATE_ZENABLE]))
    {
        if ((pdwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE)
         || ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_WBUFFERENABLE_MASK) == D3D_REG_WBUFFERENABLE_DISABLE))
        {
            /*
             * Plain Z-Buffer.
             */
            pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _TRUE)
                                  |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                  |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _FALSE)
                                  |  DRF_DEF(055, _CONTROL0, _Z_FORMAT,             _FIXED);
        }
        else
        {
            /*
             * W-Buffer.
             */
            nvAssert (pContext->pRenderTarget);
            if (pContext->pRenderTarget->getBPP() == 2)
            {
                // Fixed point w-buffer reqires scaling of RHW but gives more precision in 16bit w-buffers.
                pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _TRUE)
                                      |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                      |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _TRUE)
                                      |  DRF_NUM(055, _CONTROL0, _Z_FORMAT,             getDC()->nvD3DRegistryData.regW16Format);
            }
            else
            {
                // Floating point w-buffer gives more precision for 32-bit (also removes a HW rounding bug with fixed)
                pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _TRUE)
                                      |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                      |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _TRUE)
                                      |  DRF_NUM(055, _CONTROL0, _Z_FORMAT,             getDC()->nvD3DRegistryData.regW32Format);
            }
        }
    }
    else
    {
        pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _FALSE)
                              |  DRF_DEF(055, _CONTROL0, _ZWRITEENABLE,         _FALSE)
                              |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _FALSE)
                              |  DRF_DEF(055, _CONTROL0, _Z_FORMAT,             _FIXED);
    }
    /*
     * Set Control0 stencil buffer enable bits.
     */
    if (nvStencilBufferInUse(pContext))
    {
        pContext->bStencilEnabled = TRUE;
        pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _STENCIL_WRITE_ENABLE, _TRUE);
        /*
         * Setup Control1.
         * Stencil buffer control bits.
         */
        pmtsState->dwControl1 = DRF_NUM(055, _CONTROL1, _STENCIL_TEST_ENABLE, pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_FUNC,        pContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_REF,         pContext->dwRenderState[D3DRENDERSTATE_STENCILREF])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_READ,   pContext->dwRenderState[D3DRENDERSTATE_STENCILMASK])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_WRITE,  pContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK]);

        /*
         * Setup Control2.
         * More stencil buffer control bits.
         */
        pmtsState->dwControl2 = DRF_NUM(055, _CONTROL2, _STENCIL_OP_FAIL,  pContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL])
                              | DRF_NUM(055, _CONTROL2, _STENCIL_OP_ZFAIL, pContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL])
                              | DRF_NUM(055, _CONTROL2, _STENCIL_OP_ZPASS, pContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]);
    }
    else
    {
        pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _STENCIL_WRITE_ENABLE, _FALSE);
        /*
         * Setup Control1.
         * Stencil buffer control bits.
         */
        pmtsState->dwControl1 = DRF_DEF(055, _CONTROL1, _STENCIL_TEST_ENABLE, _FALSE)
                              | DRF_DEF(055, _CONTROL1, _STENCIL_FUNC,        _NEVER)
                              | DRF_NUM(055, _CONTROL1, _STENCIL_REF,         0)
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_READ,   0)
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_WRITE,  0);

        /*
         * Setup Control2.
         * More stencil buffer control bits.
         */
        pmtsState->dwControl2 = DRF_DEF(055, _CONTROL2, _STENCIL_OP_FAIL,  _KEEP)
                              | DRF_DEF(055, _CONTROL2, _STENCIL_OP_ZFAIL, _KEEP)
                              | DRF_DEF(055, _CONTROL2, _STENCIL_OP_ZPASS, _KEEP);
    }
    /*
     * Set ARGB write enbles.
     * Always enable RGB only enable Alpha for alpha frame buffers.
     */
//    if
//    else
    {
        pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ALPHA_WRITE_ENABLE, _FALSE);
    }
    pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _RED_WRITE_ENABLE,   _TRUE);
    pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _GREEN_WRITE_ENABLE, _TRUE);
    pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _BLUE_WRITE_ENABLE,  _TRUE);

    /*
     * Set the Fog color.
     */
    pmtsState->dwFogColor = pdwRenderState[D3DRENDERSTATE_FOGCOLOR];

    cacheFogValues(pContext);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

void nvSetDefaultMultiTextureHardwareState
(
    PNVD3DCONTEXT pContext
)
{
    DWORD                   dwZOHMode, dwFOHMode, dwTexelOrigin;
    DWORD                   dwTextureOffset;
    DWORD                   dwTextureContextDma;
    DWORD                   dwTextureColorFormat;
    DWORD                   dwMipMapLevels;
    DWORD                   dwLogWidth;
    DWORD                   dwLogHeight;
    CNvObject              *pNvObj;
    CTexture               *pTexture0;
    PNVD3DMULTITEXTURESTATE pmtsState;

    dbgTracePush ("nvSetDefaultMultiTextureHardwareState");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    dwZOHMode     = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_ZOH_MASK) == D3D_REG_TEXELALIGNMENT_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
    dwFOHMode     = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_FOH_MASK) == D3D_REG_TEXELALIGNMENT_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;
    dwTexelOrigin = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_TEXEL_MASK) == D3D_REG_TEXELALIGNMENT_TEXEL_CENTER)
                  ? NV054_CONTROL_ORIGIN_CENTER : NV054_CONTROL_ORIGIN_CORNER;

    /*
     * Grab texture from the render state array. This is guaranteed to always have the
     * correct texture no matter if it was set using the render state method or the texture
     * stage 0 method.
     */
    pNvObj    = (CNvObject *)(pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]);
    pTexture0 = pNvObj ? pNvObj->getTexture() : getDC()->pDefaultTexture; // substitute default texture if none selected
    if(!pTexture0) pTexture0 = getDC()->pDefaultTexture;

    dwTextureOffset      = pTexture0->getSwizzled()->getOffset();
    dwTextureContextDma  = pTexture0->getSwizzled()->getContextDMA();
    dwTextureColorFormat = pTexture0->getFormat();
    dwLogWidth           = pTexture0->getLogWidth();
    dwLogHeight          = pTexture0->getLogHeight();
    dwMipMapLevels       = pTexture0->getMipMapCount();

    pmtsState  = &pContext->mtsState;
    pmtsState->dwTextureOffset[0] =  dwTextureOffset;
    pmtsState->dwTextureFormat[0] =  DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     nv055TextureContextDma[dwTextureContextDma])
                                  |  DRF_NUM(055, _FORMAT, _COLOR,           nv055TextureFormat[dwTextureColorFormat])
                                  |  DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS,   dwMipMapLevels)
                                  |  DRF_NUM(055, _FORMAT, _BASE_SIZE_U,     dwLogWidth)
                                  |  DRF_NUM(055, _FORMAT, _BASE_SIZE_V,     dwLogHeight)
                                  |  DRF_DEF(055, _FORMAT, _TEXTUREADDRESSU, _CLAMP)
                                  |  DRF_DEF(055, _FORMAT, _WRAPU,           _FALSE)
                                  |  DRF_DEF(055, _FORMAT, _TEXTUREADDRESSV, _CLAMP)
                                  |  DRF_DEF(055, _FORMAT, _WRAPV,           _FALSE);
    pmtsState->dwTextureFormat[0] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,      dwZOHMode)
                                  |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,      dwFOHMode);
    pmtsState->dwTextureFilter[0] =  DRF_NUM(055, _FILTER, _KERNEL_SIZE_X, 0)
                                  |  DRF_NUM(055, _FILTER, _KERNEL_SIZE_Y, 0)
                                  |  DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _FALSE)
                                  |  DRF_DEF(055, _FILTER, _TEXTUREMIN,    _NEAREST)
                                  |  DRF_DEF(055, _FILTER, _ANISOTROPIC_MINIFY_ENABLE, _FALSE)
                                  |  DRF_DEF(055, _FILTER, _TEXTUREMAG,    _NEAREST)
                                  |  DRF_DEF(055, _FILTER, _ANISOTROPIC_MAGNIFY_ENABLE, _FALSE);

    pmtsState->dwTextureFilter[0] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, getDC()->nvD3DRegistryData.regLODBiasAdjust);

    pmtsState->dwTextureOffset[1] =  getDC()->pDefaultTexture->getSwizzled()->getOffset();
    pmtsState->dwTextureFormat[1] =  DRF_NUM(055, _FORMAT, _CONTEXT_DMA,      nv055TextureContextDma[getDC()->pDefaultTexture->getSwizzled()->getContextDMA()])
                                  |  DRF_NUM(055, _FORMAT, _COLOR,            nv055TextureFormat[getDC()->pDefaultTexture->getFormat()])
                                  |  DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS,    1)
                                  |  DRF_DEF(055, _FORMAT, _BASE_SIZE_U,     _1)
                                  |  DRF_DEF(055, _FORMAT, _BASE_SIZE_V,     _1)
                                  |  DRF_DEF(055, _FORMAT, _TEXTUREADDRESSU, _CLAMP)
                                  |  DRF_DEF(055, _FORMAT, _WRAPU,           _FALSE)
                                  |  DRF_DEF(055, _FORMAT, _TEXTUREADDRESSV, _CLAMP)
                                  |  DRF_DEF(055, _FORMAT, _WRAPV,           _FALSE);
    pmtsState->dwTextureFormat[1] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,      dwZOHMode)
                                  |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,      dwFOHMode);
    pmtsState->dwTextureFilter[1] =  DRF_NUM(055, _FILTER, _KERNEL_SIZE_X, 0)
                                  |  DRF_NUM(055, _FILTER, _KERNEL_SIZE_Y, 0)
//                                  |  DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE)
                                  |  DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _FALSE)
                                  |  DRF_DEF(055, _FILTER, _TEXTUREMIN,    _NEAREST)
                                  |  DRF_DEF(055, _FILTER, _ANISOTROPIC_MINIFY_ENABLE, _FALSE)
                                  |  DRF_DEF(055, _FILTER, _TEXTUREMAG,    _NEAREST)
                                  |  DRF_DEF(055, _FILTER, _ANISOTROPIC_MAGNIFY_ENABLE, _FALSE);

    pmtsState->dwTextureFilter[1] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, getDC()->nvD3DRegistryData.regLODBiasAdjust);

    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_DEFAULT;
    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_DEFAULT;
    pmtsState->dwCombine1Alpha = DX6TRI_COMBINE0ALPHA_DEFAULT;;
    pmtsState->dwCombine1Color = DX6TRI_COMBINE0COLOR_DEFAULT;;

    pmtsState->dwCombineFactor = 0;
    pmtsState->dwBlend         = DRF_DEF(055, _BLEND, _MASK_BIT,           _MSB)
                               | DRF_NUM(055, _BLEND, _SHADEMODE,          D3DSHADE_GOURAUD/*pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE]*/) // inner loop takes care of flat shading
                               | DRF_NUM(055, _BLEND, _TEXTUREPERSPECTIVE, pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE])
                               | DRF_NUM(055, _BLEND, _SPECULARENABLE,     pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE])
                               | DRF_NUM(055, _BLEND, _FOGENABLE,          pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE])
                               | DRF_NUM(055, _BLEND, _ALPHABLENDENABLE,   pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]);

    if (pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_BOTHSRCALPHA)
        pmtsState->dwBlend |= DRF_DEF(055, _BLEND, _SRCBLEND,  _SRCALPHA)
                           |  DRF_DEF(055, _BLEND, _DESTBLEND, _INVSRCALPHA);
    else if (pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_BOTHINVSRCALPHA)
        pmtsState->dwBlend |= DRF_DEF(055, _BLEND, _SRCBLEND,  _INVSRCALPHA)
                           |  DRF_DEF(055, _BLEND, _DESTBLEND, _SRCALPHA);
    else
        pmtsState->dwBlend |= DRF_NUM(055, _BLEND, _SRCBLEND,  pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND])
                           |  DRF_NUM(055, _BLEND, _DESTBLEND, pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]);

    pmtsState->dwControl0      = DRF_NUM(055, _CONTROL0, _ALPHAREF,             0)
                               | DRF_DEF(055, _CONTROL0, _ALPHAFUNC,            _ALWAYS)
                               | DRF_DEF(055, _CONTROL0, _ALPHATESTENABLE,      _FALSE)
                               | DRF_NUM(055, _CONTROL0, _ORIGIN,               dwTexelOrigin)
                               | DRF_NUM(055, _CONTROL0, _ZFUNC,                CTFunc(pContext,pContext->dwRenderState[D3DRENDERSTATE_ZFUNC]))
                               | DRF_NUM(055, _CONTROL0, _CULLMODE,             pContext->dwRenderState[D3DRENDERSTATE_CULLMODE])
                               | DRF_NUM(055, _CONTROL0, _DITHERENABLE,         pContext->dwRenderState[D3DRENDERSTATE_DITHERENABLE]);

    if ((pContext->pZetaBuffer) && (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]))
    {
        if ((pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE)
         || ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_WBUFFERENABLE_MASK) == D3D_REG_WBUFFERENABLE_DISABLE))
        {
            /*
             * Plain Z-Buffer.
             */
            pmtsState->dwControl0 |= DRF_NUM(055, _CONTROL0, _ZENABLE,              pContext->dwRenderState[D3DRENDERSTATE_ZENABLE])
                                  |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                  |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _FALSE)
                                  |  DRF_DEF(055, _CONTROL0, _Z_FORMAT,             _FIXED);
        }
        else
        {
            /*
             * W-Buffer.
             */
            nvAssert (pContext->pRenderTarget);
            if (pContext->pRenderTarget->getBPP() == 2)
            {
                // Fixed point w-buffer reqires scaling of RHW but gives more precision in 16bit w-buffers.
                pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _TRUE)
                                      |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                      |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _TRUE)
                                      |  DRF_NUM(055, _CONTROL0, _Z_FORMAT,             getDC()->nvD3DRegistryData.regW16Format);
            }
            else
            {
                // Floating point w-buffer gives more precision for 32-bit (also removes a HW rounding bug with fixed)
                pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _TRUE)
                                      |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                      |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _TRUE)
                                      |  DRF_NUM(055, _CONTROL0, _Z_FORMAT,             getDC()->nvD3DRegistryData.regW32Format);
            }
        }
    }
    else
    {
        pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,      _FALSE)
                              |  DRF_DEF(055, _CONTROL0, _ZWRITEENABLE, _FALSE);
    }
    pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ALPHA_WRITE_ENABLE,   _FALSE);
    pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _RED_WRITE_ENABLE,     _TRUE);
    pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _GREEN_WRITE_ENABLE,   _TRUE);
    pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _BLUE_WRITE_ENABLE,    _TRUE);
    if (nvStencilBufferInUse(pContext))
    {
        pContext->bStencilEnabled = TRUE;
        pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _STENCIL_WRITE_ENABLE, _TRUE);
        /*
         * Setup Control1.
         * Stencil buffer control bits.
         */
        pmtsState->dwControl1 = DRF_NUM(055, _CONTROL1, _STENCIL_TEST_ENABLE, pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_FUNC,        pContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_REF,         pContext->dwRenderState[D3DRENDERSTATE_STENCILREF])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_READ,   pContext->dwRenderState[D3DRENDERSTATE_STENCILMASK])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_WRITE,  pContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK]);

        /*
         * Setup Control2.
         * More stencil buffer control bits.
         */
        pmtsState->dwControl2 = DRF_NUM(055, _CONTROL2, _STENCIL_OP_FAIL,  pContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL])
                              | DRF_NUM(055, _CONTROL2, _STENCIL_OP_ZFAIL, pContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL])
                              | DRF_NUM(055, _CONTROL2, _STENCIL_OP_ZPASS, pContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]);
    }
    else
    {
        pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _STENCIL_WRITE_ENABLE, _FALSE);

        /*
         * Setup Control1.
         * Stencil buffer control bits.
         */
        pmtsState->dwControl1 = DRF_DEF(055, _CONTROL1, _STENCIL_TEST_ENABLE, _FALSE)
                              | DRF_DEF(055, _CONTROL1, _STENCIL_FUNC,        _NEVER)
                              | DRF_NUM(055, _CONTROL1, _STENCIL_REF,         0)
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_READ,   0)
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_WRITE,  0);

        /*
         * Setup Control2.
         * More stencil buffer control bits.
         */
        pmtsState->dwControl2 = DRF_DEF(055, _CONTROL2, _STENCIL_OP_FAIL,  _KEEP)
                              | DRF_DEF(055, _CONTROL2, _STENCIL_OP_ZFAIL, _KEEP)
                              | DRF_DEF(055, _CONTROL2, _STENCIL_OP_ZPASS, _KEEP);
    }
    pmtsState->dwFogColor = pContext->dwRenderState[D3DRENDERSTATE_FOGCOLOR];

    cacheFogValues(pContext);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

/*
 * The NVIDIA bump mapping algorithm is done in a single pass with four texture stages being specified.
 * This is the only four stage operation that we are supporting. Look for it now and reject any
 * other 4 stage operations.
 */
void nvCalculateBumpMapState
(
    PNVD3DCONTEXT pContext
)
{
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1, ptssState2, ptssState3;
    PNVD3DMULTITEXTURESTATE pmtsState;

    dbgTracePush ("nvCalculateBumpMapState");

    /*
     * Set up some pointers to the individual texture stages.
     */
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];
    ptssState3 = &pContext->tssState[3];
    pmtsState  = &pContext->mtsState;

    /*
     * Make specific check for our version of bump mapping.
     *
     * THIS IS A VERY SPECIFIC CHECK FOR OUR BUMP MAPPING ALGORITHM.  ALL CONDITIONS MUST BE
     * TRUE OR WE WILL FAIL VALIDATION AND NOT RENDER ANYTHING PREDICTABLE.
     *
     * Even though D3D specifies that Arg2 cannot be a texture, since we used to do this for
     * our bump mapping algorithm, I'm going to continue to allow it so that we don't break
     * anything.
     */
    if ((ptssState0->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)
     || (ptssState0->dwValue[D3DTSS_ALPHAARG1] != (D3DTA_TEXTURE | D3DTA_COMPLEMENT))
     || (ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_DISABLE)
     || (ptssState0->dwValue[D3DTSS_TEXCOORDINDEX] != 0)
     || (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED)
     || (!((ptssState1->dwValue[D3DTSS_ALPHAARG1] == D3DTA_TEXTURE)
        && (ptssState1->dwValue[D3DTSS_ALPHAARG2] == D3DTA_CURRENT))
      && !((ptssState1->dwValue[D3DTSS_ALPHAARG1] == D3DTA_CURRENT)
        && (ptssState1->dwValue[D3DTSS_ALPHAARG2] == D3DTA_TEXTURE)))
     || (ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_DISABLE)
     || (ptssState1->dwValue[D3DTSS_TEXCOORDINDEX] != 1)
     || (ptssState2->dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
     || (ptssState2->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED2X)
     || (ptssState2->dwValue[D3DTSS_COLORARG1] != (D3DTA_CURRENT | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE))
     || (ptssState2->dwValue[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
     || (ptssState3->dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
     || (ptssState3->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE)
     || (ptssState3->dwValue[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
     || (ptssState3->dwValue[D3DTSS_TEXCOORDINDEX] != 0))
    {
        nvSetDefaultMultiTextureHardwareState (pContext);
        pContext->mtsState.bTSSValid = FALSE;
        pContext->mtsState.ddrval    = D3DERR_TOOMANYOPERATIONS;
        pContext->dwStageCount       = 1;
        pContext->bUseDX6Class       = FALSE;
        /*
         * Note that the state has changed and needs to be sent to the hardware.
         */
        NV_FORCE_TRI_SETUP(pContext);
        dbgTracePop();
        return;
    }
    /*
     * Set up Bump Mapping State.
     *
     * Texture0 and Texture1 comes from stage 0, 1 or 3 (always same texture).
     * Texture0 is used for stages with TEXCOORDINDEX = 0
     * Texture1 is used for stages with TEXCOORDINDEX = 1
     */
    nvSetDX6TextureState (pContext, 0, ptssState0, pmtsState);
    nvSetDX6TextureState (pContext, 1, ptssState1, pmtsState);
    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_BUMP;
    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_BUMP;
    pmtsState->dwCombine1Alpha = DX6TRI_COMBINE1ALPHA_BUMP;
    pmtsState->dwCombine1Color = DX6TRI_COMBINE1COLOR_BUMP;
    nvSetDX6State (pContext, (DWORD *)pContext->dwRenderState, pmtsState);
    /*
     *
     */
    pContext->mtsState.bTSSValid = TRUE;
//    pContext->mtsState.ddrval    = DD_OK; this was set on entry of validate

    /*
     * Enable use of multi-texture triangle class if there is more than one texture
     * stage or if the stencil buffer is enabled.
     */
    pContext->dwStageCount  = 4;
    pContext->bUseDX6Class  = TRUE;
    pContext->bStateChange  = FALSE;

    /*
     * This code is only enabled in the DEBUG build.
     * Display the hardware registers that were calculated.
     */
    dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pContext);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

void nvCalculateDirectHardwareState
(
    PNVD3DCONTEXT pContext
)
{
    DWORD                   opSelect;
    DWORD                   arg0Select, arg0Invert, arg0AlphaRep;
    DWORD                   arg1Select, arg1Invert, arg1AlphaRep;
    DWORD                   arg2Select, arg2Invert, arg2AlphaRep;
    DWORD                   arg3Select, arg3Invert, arg3AlphaRep;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1, ptssState2, ptssState3, ptssState4, ptssState5, ptssState6, ptssState7;
    PNVD3DTEXSTAGESTATE     ptssTexture0, ptssTexture1;
    PNVD3DMULTITEXTURESTATE pmtsState;

    dbgTracePush ("nvCalculateDirectHardwareState");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    /*
     * Set up some pointers to the individual texture stages.
     */
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];
    ptssState3 = &pContext->tssState[3];
    ptssState4 = &pContext->tssState[4];
    ptssState5 = &pContext->tssState[5];
    ptssState6 = &pContext->tssState[6];
    ptssState7 = &pContext->tssState[7];
    pmtsState  = &pContext->mtsState;

    /*
     * Check the alphaops and colorops.
     *
     * Stages 0, 2, 4, and 6 should all be set as D3DTOP_MODULATE.
     */
    if ((ptssState0->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
     || (ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE)
     || (ptssState2->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
     || (ptssState2->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE)
     || (ptssState4->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
     || (ptssState4->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE)
     || (ptssState6->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
     || (ptssState6->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE))
        goto invalidDirectHardwareState;

    /*
     * Stages 3 and and 7 should all be set as D3DTOP_SELECTARG1
     */
    if ((ptssState3->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)
     || (ptssState3->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
     || (ptssState7->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)
     || (ptssState7->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG1))
        goto invalidDirectHardwareState;

    /*
     * Stages 1 and 5 should be one of the following:
     * D3DTOP_ADD, D3DTOP_ADDSIGNED, D3DTOP_ADDSIGNED2X or D3DTOP_SUBTRACT.
     * We also allow D3DTOP_MODULATE2X and D3DTOP_MODULATE4X to be specified
     * but these operations are translated to ADD2 and ADD4 respectively.
     */
    if ((ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADD)
     && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED)
     && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED2X)
     && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SUBTRACT)
     && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE2X)
     && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE4X))
        goto invalidDirectHardwareState;

    if ((ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_ADD)
     && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED)
     && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED2X)
     && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_SUBTRACT)
     && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE2X)
     && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE4X))
        goto invalidDirectHardwareState;

    if ((ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADD)
     && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED)
     && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED2X)
     && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SUBTRACT)
     && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE2X)
     && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE4X))
        goto invalidDirectHardwareState;

    if ((ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_ADD)
     && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED)
     && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED2X)
     && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_SUBTRACT)
     && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE2X)
     && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE4X))
        goto invalidDirectHardwareState;

    /*
     * Next track down all the textures being used.
     * The method for selecting the textures is simple:
     *   Map Stage 0 Texture/TextureCoordIndex to TEXTURE0.
     *   Map Stage 1 Texture/TextureCoordIndex to TEXTURE1.
     */
    ptssTexture0 = ptssTexture1 = NULL;
    if (pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP])
        ptssTexture0 = &pContext->tssState[0];
    if (pContext->tssState[1].dwValue[D3DTSS_TEXTUREMAP])
        ptssTexture1 = &pContext->tssState[1];

    /*
     * Set up the textures being used.
     */
    if (ptssTexture0) {
        nvSetDX6TextureState (pContext, 0, ptssTexture0, pmtsState);
    }
    else
    {
        DWORD   dwZOHMode, dwFOHMode;
        dwZOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_ZOH_MASK) == D3D_REG_TEXELALIGNMENT_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
        dwFOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_FOH_MASK) == D3D_REG_TEXELALIGNMENT_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;

        /*
         * Set the default texture format and offset.
         */
        pmtsState->dwTextureOffset[0] = getDC()->pDefaultTexture->getSwizzled()->getOffset();
        pmtsState->dwTextureFormat[0] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     nv055TextureContextDma[getDC()->pDefaultTexture->getSwizzled()->getContextDMA()])
                                      | DRF_NUM(055, _FORMAT, _COLOR,           nv055TextureFormat[getDC()->pDefaultTexture->getFormat()])
                                      | DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS,   1)
                                      | DRF_DEF(055, _FORMAT, _BASE_SIZE_U,     _1)
                                      | DRF_DEF(055, _FORMAT, _BASE_SIZE_V,     _1)
                                      | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSU, _CLAMP)
                                      | DRF_DEF(055, _FORMAT, _WRAPU,           _FALSE)
                                      | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSV, _CLAMP)
                                      | DRF_DEF(055, _FORMAT, _WRAPV,           _FALSE);
        pmtsState->dwTextureFormat[0] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,     dwZOHMode)
                                      |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,     dwFOHMode);

        /*
         * Setup stage texture filter
         */
        pmtsState->dwTextureFilter[0] = DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE)
                                      | DRF_NUM(055, _FILTER, _TEXTUREMIN, D3DFILTER_NEAREST)
                                      | DRF_NUM(055, _FILTER, _TEXTUREMAG, D3DFILTER_NEAREST);

        pmtsState->dwTextureFilter[0] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, getDC()->nvD3DRegistryData.regLODBiasAdjust);

        /*
         * Use the first set of texture coordinates when disabled.
         */
        pmtsState->dwUVOffset[0] = 0;
    }
    if (ptssTexture1) {
        nvSetDX6TextureState (pContext, 1, ptssTexture1, pmtsState);
    }
    else
    {
        DWORD   dwZOHMode, dwFOHMode;
        dwZOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_ZOH_MASK) == D3D_REG_TEXELALIGNMENT_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
        dwFOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_FOH_MASK) == D3D_REG_TEXELALIGNMENT_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;

        /*
         * Set the default texture format and offset.
         */
        pmtsState->dwTextureOffset[1] = getDC()->pDefaultTexture->getSwizzled()->getOffset();
        pmtsState->dwTextureFormat[1] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     nv055TextureContextDma[getDC()->pDefaultTexture->getSwizzled()->getContextDMA()])
                                      | DRF_NUM(055, _FORMAT, _COLOR,           nv055TextureFormat[getDC()->pDefaultTexture->getFormat()])
                                      | DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS,   1)
                                      | DRF_DEF(055, _FORMAT, _BASE_SIZE_U,     _1)
                                      | DRF_DEF(055, _FORMAT, _BASE_SIZE_V,     _1)
                                      | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSU, _CLAMP)
                                      | DRF_DEF(055, _FORMAT, _WRAPU,           _FALSE)
                                      | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSV, _CLAMP)
                                      | DRF_DEF(055, _FORMAT, _WRAPV,           _FALSE);
        pmtsState->dwTextureFormat[1] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,     dwZOHMode)
                                      |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,     dwFOHMode);

        /*
         * Setup stage texture filter
         */
        pmtsState->dwTextureFilter[1] = DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE)
                                      | DRF_NUM(055, _FILTER, _TEXTUREMIN, D3DFILTER_NEAREST)
                                      | DRF_NUM(055, _FILTER, _TEXTUREMAG, D3DFILTER_NEAREST);

        pmtsState->dwTextureFilter[1] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, getDC()->nvD3DRegistryData.regLODBiasAdjust);

        /*
         * Use the first set of texture coordinates when disabled.
         */
        pmtsState->dwUVOffset[1] = 0;
    }

    /*
     * Setup hardware texture combiner 0 alpha components based on texture stages 0-3.
     * Operation comes from AlphaOp of stage 1.
     * Arguments come from AlphaArg1 of stages 0-3.
     */
    switch (ptssState1->dwValue[D3DTSS_ALPHAOP])
    {
        case D3DTOP_ADD:
            opSelect = NV055_COMBINE_0_ALPHA_OPERATION_ADD;
            break;
        case D3DTOP_ADDSIGNED:
            opSelect = NV055_COMBINE_0_ALPHA_OPERATION_ADDSIGNED;
            break;
        case D3DTOP_ADDSIGNED2X:
            opSelect = NV055_COMBINE_0_ALPHA_OPERATION_ADDSIGNED2;
            break;
        case D3DTOP_SUBTRACT:
            opSelect = NV055_COMBINE_0_ALPHA_OPERATION_ADDCOMPLEMENT;
            break;
        case D3DTOP_MODULATE2X:
            opSelect = NV055_COMBINE_0_ALPHA_OPERATION_ADD2;
            break;
        case D3DTOP_MODULATE4X:
            opSelect = NV055_COMBINE_0_ALPHA_OPERATION_ADD4;
            break;
    }

    /*
     * Select Alpha Argument 0 Combiner 0.
     */
    switch (ptssState0->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg0Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg0Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg0Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE0;
            else if (ptssTexture1 && ((ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg0Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE1;
            else
                arg0Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg0Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_FACTOR;
            else
                arg0Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_ZERO;
            break;
    }
    arg0Invert = !((ptssState0->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Select Alpha Argument 1 Combiner 0.
     */
    switch (ptssState1->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_1_TEXTURE0;
            else if (ptssTexture1 && ((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_1_TEXTURE1;
            else
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_1_FACTOR;
            else
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_1_ZERO;
            break;
    }
    arg1Invert = !((ptssState1->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Select Alpha Argument 2 Combiner 0.
     */
    switch (ptssState2->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState2->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_2_TEXTURE0;
            else if (ptssTexture1 && ((ptssState2->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_2_TEXTURE1;
            else
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState2->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_2_FACTOR;
            else
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_2_ZERO;
            break;
    }
    arg2Invert = !((ptssState2->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Select Alpha Argument 3 Combiner 0.
     */
    switch (ptssState3->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg3Select = NV055_COMBINE_0_ALPHA_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg3Select = NV055_COMBINE_0_ALPHA_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState3->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg3Select = NV055_COMBINE_0_ALPHA_ARGUMENT_3_TEXTURE0;
            else if (ptssTexture1 && ((ptssState3->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg3Select = NV055_COMBINE_0_ALPHA_ARGUMENT_3_TEXTURE1;
            else
                arg3Select = NV055_COMBINE_0_ALPHA_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState3->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg3Select = NV055_COMBINE_0_ALPHA_ARGUMENT_3_FACTOR;
            else
                arg3Select = NV055_COMBINE_0_ALPHA_ARGUMENT_3_ZERO;
            break;
    }
    arg3Invert = !((ptssState3->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Program Texture Combine 0 Alpha Stage.
     */
    pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg0Invert)
                               |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg0Select)
                               |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_1,  arg1Invert)
                               |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_1, arg1Select)
                               |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                               |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                               |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_3,  arg3Invert)
                               |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_3, arg3Select)
                               |  DRF_NUM(055, _COMBINE_0_ALPHA, _OPERATION,  opSelect);

    /*
     * Setup hardware texture combiner 0 color components based on texture stages 0-3.
     * Operation comes from ColorOp of stage 1.
     * Arguments come from ColorArg1 of stages 0-3.
     */
    switch (ptssState1->dwValue[D3DTSS_COLOROP])
    {
        case D3DTOP_ADD:
            opSelect = NV055_COMBINE_0_COLOR_OPERATION_ADD;
            break;
        case D3DTOP_ADDSIGNED:
            opSelect = NV055_COMBINE_0_COLOR_OPERATION_ADDSIGNED;
            break;
        case D3DTOP_ADDSIGNED2X:
            opSelect = NV055_COMBINE_0_COLOR_OPERATION_ADDSIGNED2;
            break;
        case D3DTOP_SUBTRACT:
            opSelect = NV055_COMBINE_0_COLOR_OPERATION_ADDCOMPLEMENT;
            break;
        case D3DTOP_MODULATE2X:
            opSelect = NV055_COMBINE_0_COLOR_OPERATION_ADD2;
            break;
        case D3DTOP_MODULATE4X:
            opSelect = NV055_COMBINE_0_COLOR_OPERATION_ADD4;
            break;
    }

    /*
     * Select Color Argument 0 Combiner 0.
     */
    switch (ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg0Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg0Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg0Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE0;
            else if (ptssTexture1 && ((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg0Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE1;
            else
                arg0Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg0Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_FACTOR;
            else
                arg0Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_ZERO;
            break;
    }
    arg0Invert = !((ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    arg0AlphaRep = !((ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument 1 Combiner 0.
     */
    switch (ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_1_TEXTURE0;
            else if (ptssTexture1 && ((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_1_TEXTURE1;
            else
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_1_FACTOR;
            else
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_1_ZERO;
            break;
    }
    arg1Invert   = !((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    arg1AlphaRep = !((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument 2 Combiner 0.
     */
    switch (ptssState2->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState2->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_2_TEXTURE0;
            else if (ptssTexture1 && ((ptssState2->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_2_TEXTURE1;
            else
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState2->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_2_FACTOR;
            else
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_2_ZERO;
            break;
    }
    arg2Invert   = !((ptssState2->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    arg2AlphaRep = !((ptssState2->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument 3 Combiner 0.
     */
    switch (ptssState3->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg3Select = NV055_COMBINE_0_COLOR_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg3Select = NV055_COMBINE_0_COLOR_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState3->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg3Select = NV055_COMBINE_0_COLOR_ARGUMENT_3_TEXTURE0;
            else if (ptssTexture1 && ((ptssState3->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg3Select = NV055_COMBINE_0_COLOR_ARGUMENT_3_TEXTURE1;
            else
                arg3Select = NV055_COMBINE_0_COLOR_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState3->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg3Select = NV055_COMBINE_0_COLOR_ARGUMENT_3_FACTOR;
            else
                arg3Select = NV055_COMBINE_0_COLOR_ARGUMENT_3_ZERO;
            break;
    }
    arg3Invert   = !((ptssState3->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    arg3AlphaRep = !((ptssState3->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Program Texture Combine 0 Color Stage.
     */
    pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg0Invert)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg0AlphaRep)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg0Select)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg1Invert)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg1AlphaRep)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg1Select)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_3,  arg3Invert)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_3,    arg3AlphaRep)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_3, arg3Select)
                               |  DRF_NUM(055, _COMBINE_0_COLOR, _OPERATION,  opSelect);

    /*
     * Setup hardware texture combiner 1 alpha components based on texture stages 4-7.
     * Operation comes from AlphaOp of stage 5.
     * Arguments come from AlphaArg1 of stages 4-7.
     */
    switch (ptssState5->dwValue[D3DTSS_ALPHAOP])
    {
        case D3DTOP_ADD:
            opSelect = NV055_COMBINE_1_ALPHA_OPERATION_ADD;
            break;
        case D3DTOP_ADDSIGNED:
            opSelect = NV055_COMBINE_1_ALPHA_OPERATION_ADDSIGNED;
            break;
        case D3DTOP_ADDSIGNED2X:
            opSelect = NV055_COMBINE_1_ALPHA_OPERATION_ADDSIGNED2;
            break;
        case D3DTOP_SUBTRACT:
            opSelect = NV055_COMBINE_1_ALPHA_OPERATION_ADDCOMPLEMENT;
            break;
        case D3DTOP_MODULATE2X:
            opSelect = NV055_COMBINE_1_ALPHA_OPERATION_ADD2;
            break;
        case D3DTOP_MODULATE4X:
            opSelect = NV055_COMBINE_1_ALPHA_OPERATION_ADD4;
            break;
    }

    /*
     * Select Alpha Argument 0 Combiner 1.
     */
    switch (ptssState4->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg0Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg0Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState4->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg0Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE0;
            else if (ptssTexture1 && ((ptssState4->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg0Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1;
            else
                arg0Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState4->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg0Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_FACTOR;
            else
                arg0Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_ZERO;
            break;
    }
    arg0Invert = !((ptssState4->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Select Alpha Argument 1 Combiner 1.
     */
    switch (ptssState5->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_1_INPUT;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState5->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_1_TEXTURE0;
            else if (ptssTexture1 && ((ptssState5->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_1_TEXTURE1;
            else
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState5->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_1_FACTOR;
            else
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_1_ZERO;
            break;
    }
    arg1Invert = !((ptssState5->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Select Alpha Argument 2 Combiner 1.
     */
    switch (ptssState6->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_2_INPUT;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState6->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_2_TEXTURE0;
            else if (ptssTexture1 && ((ptssState6->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_2_TEXTURE1;
            else
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState6->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_2_FACTOR;
            else
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_2_ZERO;
            break;
    }
    arg2Invert = !((ptssState6->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Select Alpha Argument 3 Combiner 1.
     */
    switch (ptssState7->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg3Select = NV055_COMBINE_1_ALPHA_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg3Select = NV055_COMBINE_1_ALPHA_ARGUMENT_3_INPUT;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState7->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg3Select = NV055_COMBINE_1_ALPHA_ARGUMENT_3_TEXTURE0;
            else if (ptssTexture1 && ((ptssState7->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg3Select = NV055_COMBINE_1_ALPHA_ARGUMENT_3_TEXTURE1;
            else
                arg3Select = NV055_COMBINE_1_ALPHA_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState7->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg3Select = NV055_COMBINE_1_ALPHA_ARGUMENT_3_FACTOR;
            else
                arg3Select = NV055_COMBINE_1_ALPHA_ARGUMENT_3_ZERO;
            break;
    }
    arg3Invert = !((ptssState7->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Program Texture Combine 1 Alpha Stage.
     */
    pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg0Invert)
                               |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg0Select)
                               |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_1,  arg1Invert)
                               |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_1, arg1Select)
                               |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                               |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                               |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_3,  arg3Invert)
                               |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_3, arg3Select)
                               |  DRF_NUM(055, _COMBINE_1_ALPHA, _OPERATION,  opSelect);

    /*
     * Setup hardware texture combiner 1 color components based on texture stages 4-7.
     * Operation comes from ColorOp of stage 5.
     * Arguments come from ColorArg1 of stages 4-7.
     */
    switch (ptssState5->dwValue[D3DTSS_COLOROP])
    {
        case D3DTOP_ADD:
            opSelect = NV055_COMBINE_1_COLOR_OPERATION_ADD;
            break;
        case D3DTOP_ADDSIGNED:
            opSelect = NV055_COMBINE_1_COLOR_OPERATION_ADDSIGNED;
            break;
        case D3DTOP_ADDSIGNED2X:
            opSelect = NV055_COMBINE_1_COLOR_OPERATION_ADDSIGNED2;
            break;
        case D3DTOP_SUBTRACT:
            opSelect = NV055_COMBINE_1_COLOR_OPERATION_ADDCOMPLEMENT;
            break;
        case D3DTOP_MODULATE2X:
            opSelect = NV055_COMBINE_1_COLOR_OPERATION_ADD2;
            break;
        case D3DTOP_MODULATE4X:
            opSelect = NV055_COMBINE_1_COLOR_OPERATION_ADD4;
            break;
    }

    /*
     * Select Color Argument 0 Combiner 1.
     */
    switch (ptssState4->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg0Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg0Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_INPUT;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState4->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg0Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE0;
            else if (ptssTexture1 && ((ptssState4->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg0Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1;
            else
                arg0Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState4->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg0Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_FACTOR;
            else
                arg0Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_ZERO;
            break;
    }
    arg0Invert = !((ptssState4->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    arg0AlphaRep = !((ptssState4->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument 1 Combiner 1.
     */
    switch (ptssState5->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_1_INPUT;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState5->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_1_TEXTURE0;
            else if (ptssTexture1 && ((ptssState5->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_1_TEXTURE1;
            else
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_1_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState5->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_1_FACTOR;
            else
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_1_ZERO;
            break;
    }
    arg1Invert   = !((ptssState5->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    arg1AlphaRep = !((ptssState5->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument 2 Combiner 1.
     */
    switch (ptssState6->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_2_INPUT;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState6->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_2_TEXTURE0;
            else if (ptssTexture1 && ((ptssState6->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_2_TEXTURE1;
            else
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_2_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState6->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_2_FACTOR;
            else
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_2_ZERO;
            break;
    }
    arg2Invert   = !((ptssState6->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    arg2AlphaRep = !((ptssState6->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument 3 Combiner 1.
     */
    switch (ptssState7->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg3Select = NV055_COMBINE_1_COLOR_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg3Select = NV055_COMBINE_1_COLOR_ARGUMENT_3_INPUT;
            break;
        case D3DTA_TEXTURE:
            if (ptssTexture0 && ((ptssState7->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
                arg3Select = NV055_COMBINE_1_COLOR_ARGUMENT_3_TEXTURE0;
            else if (ptssTexture1 && ((ptssState7->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
                arg3Select = NV055_COMBINE_1_COLOR_ARGUMENT_3_TEXTURE1;
            else
                arg3Select = NV055_COMBINE_1_COLOR_ARGUMENT_3_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            if ((ptssState7->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
                arg3Select = NV055_COMBINE_1_COLOR_ARGUMENT_3_FACTOR;
            else
                arg3Select = NV055_COMBINE_1_COLOR_ARGUMENT_3_ZERO;
            break;
    }
    arg3Invert   = !((ptssState7->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    arg3AlphaRep = !((ptssState7->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Program Texture Combine 1 Color Stage.
     */
    pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg0Invert)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg0AlphaRep)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg0Select)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_1,  arg1Invert)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_1,    arg1AlphaRep)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_1, arg1Select)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_3,  arg3Invert)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_3,    arg3AlphaRep)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_3, arg3Select)
                               |  DRF_NUM(055, _COMBINE_1_COLOR, _OPERATION,  opSelect);

    nvSetDX6State (pContext, (DWORD *)pContext->dwRenderState, pmtsState);

    /*
     * Show that the texture stage state is valid.
     */
    pContext->mtsState.bTSSValid = TRUE;
    // pContext->mtsState.ddrval    = DD_OK;

    /*
     * Enable use of multi-texture triangle class if there is more than one texture
     * stage or if the stencil buffer is enabled.
     */
    pContext->dwStageCount  = 8;
    pContext->bUseDX6Class  = TRUE;
    pContext->bStateChange  = FALSE;

    dbgTracePop();
    return;

    /*
     * The texture stage state does not meet the NVIDIA Direct Hardware Mapping requirements.
     * Set the default hardware state and return.
     */
invalidDirectHardwareState:
    nvSetDefaultMultiTextureHardwareState (pContext);
    pContext->mtsState.bTSSValid = FALSE;
    pContext->mtsState.ddrval    = D3DERR_TOOMANYOPERATIONS;
    pContext->dwStageCount       = 1;
    pContext->bUseDX6Class       = FALSE;
    /*
     * Note that the state has changed and needs to be sent to the hardware.
     */
    NV_FORCE_TRI_SETUP(pContext);
    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------------------

inline void nvFailMultiTextureSetup
(
    PNVD3DCONTEXT pContext,
    HRESULT       failureCode
)
{
    nvSetDefaultMultiTextureHardwareState (pContext);
    pContext->mtsState.bTSSValid = FALSE;
    pContext->mtsState.ddrval    = failureCode;
    pContext->dwStageCount       = 1;
    if (!nvStencilBufferInUse (pContext)) {
        pContext->bUseDX6Class = FALSE;
    }
    else {
        pContext->bUseDX6Class = TRUE;
    }

    // Display the hardware registers that were calculated.
    dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pContext);

    // Note that the state has changed and needs to be sent to the hardware.
    NV_FORCE_TRI_SETUP(pContext);
}

//-------------------------------------------------------------------------------------

void nvSetMultiTextureHardwareState
(
    PNVD3DCONTEXT pContext
)
{
#ifdef  DEBUG
    BOOL                    bStage0UsesTexture = FALSE;
    BOOL                    bStage1UsesTexture = FALSE;
#endif  // DEBUG
    DWORD                   dwStageCount;
    DWORD                   dwStageOp;
    DWORD                   arg1Select, arg1Invert, arg1AlphaRep;
    DWORD                   arg2Select, arg2Invert, arg2AlphaRep;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1;
    PNVD3DMULTITEXTURESTATE pmtsState;
    DWORD                  *pdwRenderState = 0;

    dbgTracePush ("nvSetMultiTextureHardwareState");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    /*
     * This code is only enabled in the DEBUG build.
     * Display the DX5/DX6 render state to be calculated.
     */
    dbgDisplayContextState ((NVDBG_SHOW_RENDER_STATE | NVDBG_SHOW_MTS_STATE), pContext);

    /*
     * Do a quick count of the number of enabled texture stages to be used to determine if
     * special handling is required.
     */
    dwStageCount = 0;
    ptssState0   = &pContext->tssState[dwStageCount];
    while ((dwStageCount < 8)
        && (ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE))
    {
        dwStageCount++;
        if (dwStageCount < 8)
            ptssState0 = &pContext->tssState[dwStageCount];
    }

    /*
     * The hardware cannot handle more than two texture stages being enabled.  However,
     * there are a couple of special cases for 4 and 8 enabled texture stages.  Check if
     * special handling is required now.
     *
     * NVIDIA BUMP MAPPING HANDLING (4 Stages Enabled)
     * If there are four texture stages enabled, this could be the NVIDIA style of bump mapping.
     * Determine if it is.  If it's not, the default hardware state will be set.
     */
    if (dwStageCount == 4)
    {
        nvCalculateBumpMapState (pContext);
        dbgTracePop();
        return;
    }
    /*
     * NVIDIA DIRECT HARDWARE PROGRAMMING
     * When eight texture stages are enabled, this can signify an NVIDIA specific method of programming
     * the texture combiners directly.
     * Determine if this is the special case and program it accordingly.  If it's not, the default hardware
     * state will be set.
     */
    if (((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_DIRECTMAPENABLE_MASK) == D3D_REG_DIRECTMAPENABLE_ENABLE)
     && (dwStageCount == 8))
    {
        nvCalculateDirectHardwareState (pContext);
        dbgTracePop();
        return;
    }


    /*
     * Perform normal processing of the texture stage state setup.
     */
    pdwRenderState = (DWORD *)pContext->dwRenderState;
    pContext->dwStageCount   = 0;
    ptssState0     = &pContext->tssState[0];
    ptssState1     = &pContext->tssState[1];
    pmtsState      = &pContext->mtsState;

    /*
     * Clear out the current state.
     */
    pmtsState->dwTextureOffset[0] = 0;
    pmtsState->dwTextureOffset[1] = 0;
    pmtsState->dwTextureFormat[0] = 0;
    pmtsState->dwTextureFormat[1] = 0;
    pmtsState->dwTextureFilter[0] = 0;
    pmtsState->dwTextureFilter[1] = 0;
    pmtsState->dwCombine0Alpha    = 0;
    pmtsState->dwCombine0Color    = 0;
    pmtsState->dwCombine1Alpha    = 0;
    pmtsState->dwCombine1Color    = 0;
    pmtsState->dwBlend            = 0;
    pmtsState->dwControl0         = 0;
    pmtsState->dwControl1         = 0;
    pmtsState->dwControl2         = 0;
    pmtsState->dwFogColor         = 0;

    /*
     * If D3DRENDERSTATE_TEXTUREMAPBLEND was invoked more recently than TEXTURESTAGESTATE,
     * then use the current TEXTUREMAPBLEND setting.
     */
    if (pContext->bUseTBlendSettings)
    {
        if (!nvStencilBufferInUse (pContext))
        {
            /*
             * When using TEXTUREMAPBLEND settings, as long as there is no stencil buffer we
             * can use the faster DX5 class.
             */
            nvSetHardwareState (pContext);
            pContext->bUseDX6Class       = FALSE;
            pContext->mtsState.bTSSValid = FALSE;
            pContext->bStateChange       = FALSE;
        }
        else
        {
            DWORD   dwZOHMode, dwFOHMode;
            dwZOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_ZOH_MASK) == D3D_REG_TEXELALIGNMENT_ZOH_CENTER)
                      ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
            dwFOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_FOH_MASK) == D3D_REG_TEXELALIGNMENT_FOH_CENTER)
                      ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;

            /*
             * Setup stage 0 texture offset
             * Setup stage 0 texture format
             */
            nvSetDX6TextureState (pContext, 0, ptssState0, pmtsState);

            /*
             * Setup combine0 based on the TEXTUREMAPBLEND setting.
             */
            switch (pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND])
            {
                case D3DTBLEND_DECAL:
                    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_DECAL;
                    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_DECAL;
                    break;

                case D3DTBLEND_MODULATE:
                    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_MODULATE;
                    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_MODULATE;
                    break;

                case D3DTBLEND_DECALALPHA:
                    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_DECALALPHA;
                    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_DECALALPHA;
                    break;

                case D3DTBLEND_MODULATEALPHA:
                    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_MODULATEALPHA;
                    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_MODULATEALPHA;
                    break;

                case D3DTBLEND_DECALMASK:
                    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_DECALMASK;
                    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_DECALMASK;
                    break;

                case D3DTBLEND_MODULATEMASK:
                    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_MODULATEMASK;
                    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_MODULATEMASK;
                    break;

                case D3DTBLEND_COPY:
                    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_COPY;
                    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_COPY;
                    break;

                case D3DTBLEND_ADD:
                    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_ADD;
                    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_ADD;
                    break;
            }

            /*
             * If there is no texture specified, then set to modulatealpha.
             */
            if (!pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP])
            {
                pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_MODULATEALPHA;
                pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_MODULATEALPHA;
            }

            /*
             * Set the default texture format and offset.
             */
            pmtsState->dwTextureOffset[1] = getDC()->pDefaultTexture->getSwizzled()->getOffset();
            pmtsState->dwTextureFormat[1] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     nv055TextureContextDma[getDC()->pDefaultTexture->getSwizzled()->getContextDMA()])
                                          | DRF_NUM(055, _FORMAT, _COLOR,           nv055TextureFormat[getDC()->pDefaultTexture->getFormat()])
                                          | DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS,   1)
                                          | DRF_DEF(055, _FORMAT, _BASE_SIZE_U,     _1)
                                          | DRF_DEF(055, _FORMAT, _BASE_SIZE_V,     _1)
                                          | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSU, _CLAMP)
                                          | DRF_DEF(055, _FORMAT, _WRAPU,           _FALSE)
                                          | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSV, _CLAMP)
                                          | DRF_DEF(055, _FORMAT, _WRAPV,           _FALSE);
            pmtsState->dwTextureFormat[1] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,     dwZOHMode)
                                          |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,     dwFOHMode);

            /*
             * Setup stage texture filter
             */
            pmtsState->dwTextureFilter[1] = DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE)
                                          | DRF_NUM(055, _FILTER, _TEXTUREMIN, D3DFILTER_NEAREST)
                                          | DRF_NUM(055, _FILTER, _TEXTUREMAG, D3DFILTER_NEAREST);

            pmtsState->dwTextureFilter[1] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, getDC()->nvD3DRegistryData.regLODBiasAdjust);

            /*
             * Always set combine1 to pass through.
             */
            pmtsState->dwCombine1Alpha = DX6TRI_COMBINE1ALPHA_PASS_THRU;
            pmtsState->dwCombine1Color = DX6TRI_COMBINE1COLOR_PASS_THRU;

            /*
             * Use the first set of texture coordinates when disabled.
             */
            pmtsState->dwUVOffset[1] = 0;

            /*
             * Setup the rest of the DX6 triangle state.
             */
            nvSetDX6State (pContext, pdwRenderState, pmtsState);

            /*
             * Need to use the DX6 triangle class here.
             */
            pContext->bUseDX6Class       = TRUE;
            pContext->mtsState.bTSSValid = TRUE;
            pContext->bStateChange       = FALSE;

            /*
             * This code is only enabled in the DEBUG build.
             * Display the hardware registers that were calculated.
             */
            dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pContext);
        }

        // pContext->mtsState.ddrval    = DD_OK;
        pContext->dwStageCount       = 1;
        /*
         * Note that the state has changed and needs to be sent to the hardware.
         */
        NV_FORCE_TRI_SETUP(pContext);
        dbgTracePop();
        return;
    }


    /*
     * Compile the hardware state for each texture stage.
     * Any errors during compile will result in disabling multi-textureing.
     *
     * Stage 0.
     */
    if (ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_DISABLE)
    {
        /*
         * No texturing at all!
         */
        if (!nvStencilBufferInUse (pContext))
        {
            nvConvertTextureStageToDX5Class (pContext);
            pContext->bUseDX6Class       = FALSE;
            pContext->mtsState.bTSSValid = FALSE;
            pContext->bStateChange       = FALSE;
        }
        else
        {
            nvSetDefaultMultiTextureHardwareState (pContext);
            pContext->bUseDX6Class       = TRUE;
            pContext->mtsState.bTSSValid = TRUE;
        }
        // pContext->mtsState.ddrval    = DD_OK;
        pContext->dwStageCount       = 1;

        /*
         * This code is only enabled in the DEBUG build.
         * Display the hardware registers that were calculated.
         */
        dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pContext);

        /*
         * Note that the state has changed and needs to be sent to the hardware.
         */
        NV_FORCE_TRI_SETUP(pContext);
        dbgTracePop();
        return;
    }

    /*
     * Check for a texture stage state that can be handled by the DX5 class.
     */
    if ((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_DISABLE)
     && (!nvStencilBufferInUse (pContext)))
    {
        /*
         * If there's only one texture stage, and no stencil buffer.
         * There's an opportunity to use the DX5 class for this
         * operation and get 2 Pixels per clock.
         */
        if (nvConvertTextureStageToDX5Class (pContext))
        {
            /*
             * Single texture stage was converted to DX5 class.
             * Use it instead.
             */
            pContext->bUseDX6Class  = FALSE;
            pContext->bStateChange  = FALSE;

            /*
             * Show that the DX6 class state is not calculated.
             */
            pContext->mtsState.bTSSValid = FALSE;
            // pContext->mtsState.ddrval    = DD_OK;
            dbgTracePop();
            return;
        }
        if (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "D3D:nvSetMultiTextureHardwareState - Single Texture Stage Using DX6 Class!!!");
    }

    /*
     * Setup stage 0 texture offset
     * Setup stage 0 texture format
     */
    nvSetDX6TextureState (pContext, 0, ptssState0, pmtsState);

    /*
     * Setup stage 0 Alpha Combiner.
     */
    dwStageOp = ptssState0->dwValue[D3DTSS_ALPHAOP];
    if((dwStageOp!=D3DTOP_SELECTARG2)&&(dwStageOp!=D3DTOP_DISABLE))
    {

        switch (ptssState0->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
        {
        case D3DTA_DIFFUSE:
            arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssState0->dwValue[D3DTSS_TEXTUREMAP]) {
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE0;
            }
            else {
                // Disable Arg1 if there is no texture handle selected.
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
                dwStageOp  = D3DTOP_SELECTARG1;
            }
            break;
        case D3DTA_TFACTOR:
            arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_FACTOR;
            break;

        default:   // (including D3DTA_SPECULAR in DX7)
            nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDALPHAARG);
            dbgTracePop();
            return;
        }
        arg1Invert = !((ptssState0->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);
    }

    if((dwStageOp!=D3DTOP_SELECTARG1)&&(dwStageOp!=D3DTOP_DISABLE))
    {

        switch (ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK)
        {
        case D3DTA_DIFFUSE:
            arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            // D3DTA_TEXTURE isn't really a valid argument for Arg2.
            if (ptssState0->dwValue[D3DTSS_TEXTUREMAP])
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE0;
            else
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_FACTOR;
            break;

        default:   // (including D3DTA_SPECULAR in DX7)
            nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDALPHAARG);
            dbgTracePop();
            return;
        }
        arg2Invert = !((ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_COMPLEMENT) == 0);
    }

#ifdef  DEBUG
    if ((arg1Select == NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE0)
     && (ptssState0->dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
     && (ptssState0->dwValue[D3DTSS_TEXTUREMAP]))
        bStage0UsesTexture = TRUE;
#endif  // DEBUG

    switch (dwStageOp)
    {
        case D3DTOP_DISABLE:
            pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_DISABLE;
            break;
        case D3DTOP_SELECTARG1:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_2,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_2, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_SELECTARG2:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_2,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_2, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATE:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_1, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_2,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_2, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATE2X:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_1, arg2Select)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_3,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_3, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATE4X:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_1, arg2Select)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_3,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_3, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD2);
            break;
        case D3DTOP_ADD:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_ADDSIGNED:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADDSIGNED);
            break;
        case D3DTOP_ADDSIGNED2X:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADDSIGNED2);
            break;
        case D3DTOP_SUBTRACT:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  (arg1Invert ^ NV055_COMBINE_0_COLOR_INVERSE_0_INVERSE))
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADDCOMPLEMENT);
            break;
        case D3DTOP_ADDSMOOTH:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  (arg1Invert ^ NV055_COMBINE_0_ALPHA_INVERSE_2_INVERSE))
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_3,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_3, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDDIFFUSEALPHA:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _DIFFUSE)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _DIFFUSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDTEXTUREALPHA:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _TEXTURE0)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _TEXTURE0)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDFACTORALPHA:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _FACTOR)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _FACTOR)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDTEXTUREALPHAPM:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _TEXTURE0)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDCURRENTALPHA:
            pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _DIFFUSE)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _DIFFUSE)
                                       |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            break;
        case D3DTOP_PREMODULATE:
            if (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
            {
                DWORD   arg1InvertStage1;
                DWORD   arg1SelectStage1;

                if (ptssState1->dwValue[D3DTSS_TEXTUREMAP])
                    arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE1;
                else
                    arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
                arg1InvertStage1 = NV055_COMBINE_0_ALPHA_INVERSE_1_NORMAL;

                pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_1,  arg1InvertStage1)
                                           |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_1, arg1SelectStage1)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            }
            else
            {
                /*
                 * When second texture stage is disabled, this is the same as SELECTARG1.
                 */
                pmtsState->dwCombine0Alpha =  DRF_NUM(055, _COMBINE_0_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_0_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,  _ADD);
            }
            break;
        default:
            nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDALPHAOPERATION);
            dbgTracePop();
            return;
    }

    /*
     * Setup stage 0 Color Combiner.
     */
    dwStageOp = ptssState0->dwValue[D3DTSS_COLOROP];

    if((dwStageOp!=D3DTOP_SELECTARG2)&&(dwStageOp!=D3DTOP_DISABLE))
    {
        switch (ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
        {
        case D3DTA_DIFFUSE:
            arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssState0->dwValue[D3DTSS_TEXTUREMAP]) {
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE0;
            }
            else {
                // Disable Arg1 if there is no texture handle selected.
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
                dwStageOp  = D3DTOP_SELECTARG1;
            }
            break;
        case D3DTA_TFACTOR:
            arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_FACTOR;
            break;

        default:   // (including D3DTA_SPECULAR in DX7)
            nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDCOLORARG);
            dbgTracePop();
            return;
        }
        arg1Invert   = !((ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
        arg1AlphaRep = !((ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);
    }

    if((dwStageOp!=D3DTOP_SELECTARG1)&&(dwStageOp!=D3DTOP_DISABLE))
    {
        switch (ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK)
        {
        case D3DTA_DIFFUSE:
            arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            // D3DTA_TEXTURE isn't really a valid argument for Arg2.
            if (ptssState0->dwValue[D3DTSS_TEXTUREMAP])
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE0;
            else
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_FACTOR;
            break;

        default:   // (including D3DTA_SPECULAR in DX7)
            nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDCOLORARG);
            dbgTracePop();
            return;
        }
        arg2Invert   = !((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_COMPLEMENT) == 0);
        arg2AlphaRep = !((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_ALPHAREPLICATE) == 0);
    }

#ifdef  DEBUG
    if ((arg1Select == NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE0)
     && (ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
     && (ptssState0->dwValue[D3DTSS_TEXTUREMAP]))
        bStage0UsesTexture = TRUE;
#endif  // DEBUG

    switch (dwStageOp)
    {
        case D3DTOP_DISABLE:
            pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_DISABLE;
            break;
        case D3DTOP_SELECTARG1:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_2,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_2,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_2, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_SELECTARG2:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_2,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_2,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_2, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATE:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_2,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_2,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_2, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATE2X:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg2Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_3,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_3,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_3, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATE4X:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg2Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_3,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_3,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_3, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD2);
            break;
        case D3DTOP_ADD:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_ADDSIGNED:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADDSIGNED);
            break;
        case D3DTOP_ADDSIGNED2X:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADDSIGNED2);
            break;
        case D3DTOP_SUBTRACT:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  (arg1Invert ^ NV055_COMBINE_0_COLOR_INVERSE_0_INVERSE))
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADDCOMPLEMENT);
            break;
        case D3DTOP_ADDSMOOTH:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  (arg1Invert ^ NV055_COMBINE_0_COLOR_INVERSE_2_INVERSE))
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_3,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_3,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_3, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDDIFFUSEALPHA:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _DIFFUSE)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _DIFFUSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDTEXTUREALPHA:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _TEXTURE0)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _TEXTURE0)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDFACTORALPHA:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _FACTOR)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _FACTOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDTEXTUREALPHAPM:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _TEXTURE0)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_BLENDCURRENTALPHA:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _NORMAL)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _DIFFUSE)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg2Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _ALPHA)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _DIFFUSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_PREMODULATE:
            if (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
            {
                DWORD   arg1InvertStage1;
                DWORD   arg1AlphaRepStage1;
                DWORD   arg1SelectStage1;

                if (ptssState1->dwValue[D3DTSS_TEXTUREMAP])
                    arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE1;
                else
                    arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
                arg1InvertStage1   = NV055_COMBINE_0_COLOR_INVERSE_1_NORMAL;
                arg1AlphaRepStage1 = NV055_COMBINE_0_COLOR_ALPHA_1_COLOR;

                pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg1InvertStage1)
                                           |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg1AlphaRepStage1)
                                           |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg1SelectStage1)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_2,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            }
            else
            {
                /*
                 * When second texture stage is disabled, this is the same as SELECTARG1.
                 */
                pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_2,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            }
            break;

        case D3DTOP_MODULATEALPHA_ADDCOLOR:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    _ALPHA)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg2Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATECOLOR_ADDALPHA:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg2Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    _ALPHA)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  (arg1Invert ^ NV055_COMBINE_0_COLOR_INVERSE_0_INVERSE))
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    _ALPHA)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg2Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
            pmtsState->dwCombine0Color =  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_0,  (arg1Invert ^ NV055_COMBINE_0_COLOR_INVERSE_0_INVERSE))
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_0,    arg1AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_0, arg1Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_1,  arg2Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_1,    arg2AlphaRep)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_1, arg2Select)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _INVERSE_2,  arg1Invert)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ALPHA_2,    _ALPHA)
                                       |  DRF_NUM(055, _COMBINE_0_COLOR, _ARGUMENT_2, arg1Select)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,  _INVERSE)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,    _COLOR)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3, _ZERO)
                                       |  DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,  _ADD);
            break;
        default:
            nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDCOLOROPERATION);
            dbgTracePop();
            return;
    }
    pContext->dwStageCount++;

    /*
     * Stage 1.
     */
    if (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
    {
        nvSetDX6TextureState (pContext, 1, ptssState1, pmtsState);

        /*
         * Setup stage 1 Alpha Combiner.
         */
        dwStageOp = ptssState1->dwValue[D3DTSS_ALPHAOP];

        if((dwStageOp!=D3DTOP_SELECTARG2)&&(dwStageOp!=D3DTOP_DISABLE))
        {
            switch (ptssState1->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
            {
            case D3DTA_DIFFUSE:
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_DIFFUSE;
                break;
            case D3DTA_CURRENT:
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
                break;
            case D3DTA_TEXTURE:
                if (ptssState1->dwValue[D3DTSS_TEXTUREMAP]) {
                    arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1;
                }
                else {
                    // Disable Arg1 if there is no texture handle selected.
                    arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
                    // dwStageOp  = D3DTOP_SELECTARG1;
                }
                break;
            case D3DTA_TFACTOR:
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_FACTOR;
                break;
            default:
                nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDALPHAARG);
                dbgTracePop();
                return;
            }
            arg1Invert = !((ptssState1->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);
        }

        if((dwStageOp!=D3DTOP_SELECTARG1)&&(dwStageOp!=D3DTOP_DISABLE))
        {
            switch (ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK)
            {
            case D3DTA_DIFFUSE:
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_DIFFUSE;
                break;
            case D3DTA_CURRENT:
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
                break;
            case D3DTA_TEXTURE:
                // D3DTA_TEXTURE isn't really a valid argument for Arg2.
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1;
                break;
            case D3DTA_TFACTOR:
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_FACTOR;
                break;
            default:
                nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDALPHAARG);
                dbgTracePop();
                return;
            }
            arg2Invert = !((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_COMPLEMENT) == 0);
        }

#ifdef  DEBUG
        if ((arg1Select == NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1)
         && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
         && (ptssState1->dwValue[D3DTSS_TEXTUREMAP]))
            bStage1UsesTexture = TRUE;
#endif  // DEBUG

        switch (dwStageOp)
        {
            case D3DTOP_DISABLE:
                pmtsState->dwCombine1Alpha = DX6TRI_COMBINE1ALPHA_PASS_THRU;//DX6TRI_COMBINE1ALPHA_DISABLE;
                break;
            case D3DTOP_SELECTARG1:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_SELECTARG2:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATE:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_1, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATE2X:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_1, arg2Select)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_3,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_3, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATE4X:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_1, arg2Select)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_3,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_3, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD2);
                break;
            case D3DTOP_ADD:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_ADDSIGNED:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADDSIGNED);
                break;
            case D3DTOP_ADDSIGNED2X:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADDSIGNED2);
                break;
            case D3DTOP_SUBTRACT:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  (arg1Invert ^ NV055_COMBINE_0_COLOR_INVERSE_0_INVERSE))
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADDCOMPLEMENT);
                break;
            case D3DTOP_ADDSMOOTH:

                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  (arg1Invert ^ NV055_COMBINE_1_ALPHA_INVERSE_2_INVERSE))
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_3,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_3, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDDIFFUSEALPHA:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _DIFFUSE)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _DIFFUSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDTEXTUREALPHA:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _TEXTURE1)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _TEXTURE1)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDFACTORALPHA:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _FACTOR)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _FACTOR)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDTEXTUREALPHAPM:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _TEXTURE1)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDCURRENTALPHA:
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _INPUT)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _INPUT)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            case D3DTOP_PREMODULATE:
                /*
                 * When premodulate is set on stage 1, this is the same as just a selctarg1 since we
                 * don't support more than 2 texture stages.
                 * It would seem rather silly to do this on stage 1 anyway because you lose whatever was
                 * set on stage 0.
                 */
                pmtsState->dwCombine1Alpha =  DRF_NUM(055, _COMBINE_1_ALPHA, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_ALPHA, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,  _ADD);
                break;
            default:
                nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDALPHAOPERATION);
                dbgTracePop();
                return;
        }

        /*
         * Setup stage 1 Color Combiner.
         */
        dwStageOp = ptssState1->dwValue[D3DTSS_COLOROP];

        if((dwStageOp!=D3DTOP_SELECTARG2)&&(dwStageOp!=D3DTOP_DISABLE))
        {
        switch (ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
        {
            case D3DTA_DIFFUSE:
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_DIFFUSE;
                break;
            case D3DTA_CURRENT:
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_INPUT;
                break;
            case D3DTA_TEXTURE:
                if (ptssState1->dwValue[D3DTSS_TEXTUREMAP]) {
                    arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1;
                }
                else {
                    // Disable Arg1 if there is no texture handle selected.
                    arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
                    // dwStageOp  = D3DTOP_SELECTARG1;
                }
                break;
            case D3DTA_TFACTOR:
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_FACTOR;
                break;

            default:
                nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDCOLORARG);
                dbgTracePop();
                return;
        }
        arg1Invert   = !((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
        arg1AlphaRep = !((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);
        }

        if((dwStageOp!=D3DTOP_SELECTARG1)&&(dwStageOp!=D3DTOP_DISABLE))
        {
            switch (ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK)
            {
            case D3DTA_DIFFUSE:
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_DIFFUSE;
                break;
            case D3DTA_CURRENT:
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_INPUT;
                break;
            case D3DTA_TEXTURE:
                // D3DTA_TEXTURE isn't really a valid argument for Arg2.
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1;
                break;
            case D3DTA_TFACTOR:
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_FACTOR;
                break;

            default:
                nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDCOLORARG);
                dbgTracePop();
                return;
            }
            arg2Invert   = !((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_COMPLEMENT) == 0);
            arg2AlphaRep = !((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_ALPHAREPLICATE) == 0);
        }

#ifdef  DEBUG
        if ((arg1Select == NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1)
         && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
         && (ptssState1->dwValue[D3DTSS_TEXTUREMAP]))
            bStage1UsesTexture = TRUE;
#endif  // DEBUG

        switch (dwStageOp)
        {
            case D3DTOP_DISABLE:
                pmtsState->dwCombine1Color = DX6TRI_COMBINE1COLOR_DISABLE;
                break;
            case D3DTOP_SELECTARG1:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_2,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_SELECTARG2:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_2,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATE:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_1,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_1, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_2,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATE2X:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_1,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_1, arg2Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_3,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_3,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_3, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATE4X:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_1,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_1, arg2Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_3,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_3,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_3, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD2);
                break;
            case D3DTOP_ADD:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_ADDSIGNED:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADDSIGNED);
                break;
            case D3DTOP_ADDSIGNED2X:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADDSIGNED2);
                break;
            case D3DTOP_SUBTRACT:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  (arg1Invert ^ NV055_COMBINE_0_COLOR_INVERSE_0_INVERSE))
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADDCOMPLEMENT);
                break;
            case D3DTOP_ADDSMOOTH:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  (arg1Invert ^ NV055_COMBINE_1_COLOR_INVERSE_2_INVERSE))
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_3,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_3,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_3, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDDIFFUSEALPHA:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _DIFFUSE)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _DIFFUSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDTEXTUREALPHA:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _TEXTURE1)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _TEXTURE1)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDFACTORALPHA:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _FACTOR)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _FACTOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDTEXTUREALPHAPM:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _TEXTURE1)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_BLENDCURRENTALPHA:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _INPUT)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg2Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _ALPHA)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _INPUT)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_PREMODULATE:
                /*
                 * When premodulate is set on stage 1, this is the same as just a selctarg1 since we
                 * don't support more than 2 texture stages.
                 * It would seem rather silly to do this on stage 1 anyway because you lose whatever was
                 * set on stage 0.
                 */
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_2,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_2,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_2, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _NORMAL)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATEALPHA_ADDCOLOR:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    _ALPHA)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_1,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_1, arg2Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATECOLOR_ADDALPHA:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_1,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_1, arg2Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    _ALPHA)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  (arg1Invert ^ NV055_COMBINE_1_COLOR_INVERSE_0_INVERSE))
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    _ALPHA)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_1,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_1, arg2Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
                pmtsState->dwCombine1Color =  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_0,  (arg1Invert ^ NV055_COMBINE_1_COLOR_INVERSE_0_INVERSE))
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_0,    arg1AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_0, arg1Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_1,  arg2Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_1,    arg2AlphaRep)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_1, arg2Select)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _INVERSE_2,  arg1Invert)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ALPHA_2,    _ALPHA)
                                           |  DRF_NUM(055, _COMBINE_1_COLOR, _ARGUMENT_2, arg1Select)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,  _INVERSE)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,    _COLOR)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3, _ZERO)
                                           |  DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,  _ADD);
                break;
            default:
                nvFailMultiTextureSetup (pContext, D3DERR_UNSUPPORTEDCOLOROPERATION);
                dbgTracePop();
                return;
        }

        pContext->dwStageCount++;

#ifdef  DEBUG
        if (bStage0UsesTexture && bStage1UsesTexture)
        {
            CTexture *pTexture0, *pTexture1;
            pTexture0 = ((CNvObject *)(ptssState0->dwValue[D3DTSS_TEXTUREMAP]))->getTexture();
            pTexture1 = ((CNvObject *)(ptssState1->dwValue[D3DTSS_TEXTUREMAP]))->getTexture();
            
            if(!pTexture0 || !pTexture1){
                DPF("D3D:nvSetMultiTextureHardwareState - Multi-Textures Memory Space Error!!");
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture0Obj = %08x,  Texture = %08x", ptssState0->dwValue[D3DTSS_TEXTUREMAP], pTexture0);
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture1Obj = %08x,  Texture = %08x", ptssState1->dwValue[D3DTSS_TEXTUREMAP], pTexture1);
            }
            else {

                if (pTexture0->getSwizzled()->getContextDMA() != pTexture1->getSwizzled()->getContextDMA())
                {
                    DPF("D3D:nvSetMultiTextureHardwareState - Multi-Textures Memory Space Error!!");
                    DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture0 = %08x,  Context = %08x", ptssState0->dwValue[D3DTSS_TEXTUREMAP], pTexture0->getSwizzled()->getContextDMA());
                    DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture1 = %08x,  Context = %08x", ptssState1->dwValue[D3DTSS_TEXTUREMAP], pTexture1->getSwizzled()->getContextDMA());
                }
                else
                {
                    DPF_LEVEL(NVDBG_LEVEL_ERROR, "D3D:nvSetMultiTextureHardwareState - YEAH! Multi-Textures Memory Space OK!!");
                    DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture0 = %08x,  Context = %08x", ptssState0->dwValue[D3DTSS_TEXTUREMAP], pTexture0->getSwizzled()->getContextDMA());
                    DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture1 = %08x,  Context = %08x", ptssState1->dwValue[D3DTSS_TEXTUREMAP], pTexture1->getSwizzled()->getContextDMA());
                }
            }
        }
#endif  // DEBUG

    }

    else
    {
        DWORD   dwZOHMode, dwFOHMode;
        dwZOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_ZOH_MASK) == D3D_REG_TEXELALIGNMENT_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
        dwFOHMode = ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_FOH_MASK) == D3D_REG_TEXELALIGNMENT_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;

        /*
         * Set the default texture format and offset.
         */
        pmtsState->dwTextureOffset[1] = getDC()->pDefaultTexture->getSwizzled()->getOffset();
        pmtsState->dwTextureFormat[1] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     nv055TextureContextDma[getDC()->pDefaultTexture->getSwizzled()->getContextDMA()])
                                      | DRF_NUM(055, _FORMAT, _COLOR,           nv055TextureFormat[getDC()->pDefaultTexture->getFormat()])
                                      | DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS,   1)
                                      | DRF_DEF(055, _FORMAT, _BASE_SIZE_U,     _1)
                                      | DRF_DEF(055, _FORMAT, _BASE_SIZE_V,     _1)
                                      | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSU, _CLAMP)
                                      | DRF_DEF(055, _FORMAT, _WRAPU,           _FALSE)
                                      | DRF_DEF(055, _FORMAT, _TEXTUREADDRESSV, _CLAMP)
                                      | DRF_DEF(055, _FORMAT, _WRAPV,           _FALSE);
        pmtsState->dwTextureFormat[1] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,     dwZOHMode)
                                      |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,     dwFOHMode);

        /*
         * Setup stage texture filter
         */
        pmtsState->dwTextureFilter[1] = DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE)
                                      | DRF_NUM(055, _FILTER, _TEXTUREMIN, D3DFILTER_NEAREST)
                                      | DRF_NUM(055, _FILTER, _TEXTUREMAG, D3DFILTER_NEAREST);

        pmtsState->dwTextureFilter[1] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, getDC()->nvD3DRegistryData.regLODBiasAdjust);

        /*
         * No stage 1 texture blending.
         * Set the stage 1 combiners to something sane.
         */
        pmtsState->dwCombine1Alpha = DX6TRI_COMBINE1ALPHA_PASS_THRU;
        pmtsState->dwCombine1Color = DX6TRI_COMBINE1COLOR_PASS_THRU;

        /*
         * Use the first set of texture coordinates when disabled.
         */
        pmtsState->dwUVOffset[1] = 0;

    }

    /*
     * handle textures that are unaligned
     *
     * this happens when a mipmap chain is created and the app selects one
     *  of the unaligned lower level mipmaps as the base texture
     * aka HW alignment limitation
     */
    if (pmtsState->dwTextureOffset[0] & 0xff)
    {
        CTexture *pTexture;
        DWORD     dwSize;
        DWORD     dwAddr;

        /*
         * get the texture that is involved
         */
        pTexture = ((CNvObject *)(ptssState0->dwValue[D3DTSS_TEXTUREMAP]))->getTexture();
        if (!pTexture){
            pTexture = getDC()->pDefaultTexture;
        }

        /*
         * get source address
         */
        dwAddr = pTexture->getSwizzled()->getAddress();

        /*
         * determine how many bytes to move
         */
        dwSize = pTexture->getBaseTexture()->getSwizzled()->getSize()
               - (pTexture->getSwizzled()->getOffset() - pTexture->getBaseTexture()->getSwizzled()->getOffset());

        /*
         * move them to spare texture 0
         */
        nvMemCopy (getDC()->dwSpareTextureOffset[0] + VIDMEM_ADDR(pDriverData->BaseAddress),dwAddr,dwSize);

        /*
         * modify texture source
         */
        pmtsState->dwTextureOffset[0]  = getDC()->dwSpareTextureOffset[0];
        pmtsState->dwTextureFormat[0] &= ~0xf;
        pmtsState->dwTextureFormat[0] |= nv055TextureContextDma[NV_CONTEXT_DMA_VID];
    }

    if (pmtsState->dwTextureOffset[1] & 0xff)
    {
        CTexture *pTexture;
        DWORD         dwSize;
        DWORD         dwAddr;

        /*
         * get the texture that is involved
         */
        pTexture = ((CNvObject *)(ptssState1->dwValue[D3DTSS_TEXTUREMAP]))->getTexture();
        if (!pTexture){
            pTexture = getDC()->pDefaultTexture;
        }

        /*
         * get source address
         */
        dwAddr = pTexture->getSwizzled()->getAddress();

        /*
         * determine how many bytes to move
         */
        dwSize = pTexture->getBaseTexture()->getSwizzled()->getSize()
               - (pTexture->getSwizzled()->getOffset() - pTexture->getBaseTexture()->getSwizzled()->getOffset());

        /*
         * move them to spare texture 1
         */
        nvMemCopy (getDC()->dwSpareTextureOffset[1] + VIDMEM_ADDR(pDriverData->BaseAddress),dwAddr,dwSize);

        /*
         * modify texture source
         */
        pmtsState->dwTextureOffset[1]  = getDC()->dwSpareTextureOffset[1];
        pmtsState->dwTextureFormat[1] &= ~0xf;
        pmtsState->dwTextureFormat[1] |= nv055TextureContextDma[NV_CONTEXT_DMA_VID];
    }

    nvSetDX6State (pContext, pdwRenderState, pmtsState);

    /*
     * At this point, only stages 0 and 1 should be enabled at most.  If both are
     * enabled and stage 2 is enabled also, then this is an error.  Flag it but the
     * hardware will still use the first two stages of setup.
     */
    if ((pContext->tssState[0].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
     && (pContext->tssState[1].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)
     && (pContext->tssState[2].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE))
    {
        pContext->mtsState.bTSSValid = FALSE;
        pContext->mtsState.ddrval    = D3DERR_TOOMANYOPERATIONS;
        pContext->bUseDX6Class       = TRUE;

        /*
         * This code is only enabled in the DEBUG build.
         * Display the hardware registers that were calculated.
         */
        dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pContext);

        dbgTracePop();
        return;
    }

    pContext->mtsState.bTSSValid = TRUE;
//    pContext->mtsState.ddrval    = DD_OK;

    /*
     * Assume that the DX6 class is going to be used for rendering.
     * stage or if the stencil buffer is enabled.
     */
    pContext->bUseDX6Class = TRUE;
    pContext->bStateChange = FALSE;

    /*
     * This code is only enabled in the DEBUG build.
     * Display the hardware registers that were calculated.
     */
    dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pContext);
    dbgTracePop();
    return;
}

#endif  // NVARCH >= 0x04

