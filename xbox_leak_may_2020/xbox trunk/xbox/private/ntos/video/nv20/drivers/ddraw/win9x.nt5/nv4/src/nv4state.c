#ifdef  NV4
/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4STATE.C                                                        *
*   NV4 state management routines.                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman               04/18/98 - NV4 development.             *
*                                                                           *
\***************************************************************************/
#include <math.h>
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3d.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv4dblnd.h"
#include "nv3ddbg.h"
#include "nv4vxmac.h"
#include "nv4dreg.h"

#ifdef NV_CONTROLTRAFFIC
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

__inline DWORD CTFunc
(
    DWORD dw
)
{
    if ((pCurrentContext->dwCTFlags & (NV_CT_ENABLED | NV_CT_FRONT)) == NV_CT_ENABLED)
    {
        return dwCTTable[dw];
    }
    else
    {
        return dw;
    }
}

#endif //NV_CONTROLTRAFFIC

/*
 * Send suface setup to the hardware.
 */
void nvSetD3DSurfaceState
(
    PNVD3DCONTEXT   pContext
)
{
    /*
     * Only need to update the surface if it has changed since the last render call
     * or the global clip state has changed.
     */
    if ((pDriverData->lpLast3DSurfaceRendered != pContext->lpLcl->lpGbl->fpVidMem)
     || (pDriverData->TwoDRenderingOccurred))
    {
        /*
         * Make sure the context has the correct suface information in it.
         */
        if (dbgFrontRender)
        {
            pContext->dwSurfaceAddr            = pDriverData->CurrentVisibleSurfaceAddress;
            pContext->dwSurfaceOffset          = VIDMEM_OFFSET(pDriverData->CurrentVisibleSurfaceAddress);
            pContext->surfacePitch.wColorPitch = (WORD)dbgFrontRenderPitch;
        }
        else
        {
            pContext->dwSurfaceAddr            = VIDMEM_ADDR(pContext->lpLcl->lpGbl->fpVidMem);
            pContext->dwSurfaceOffset          = VIDMEM_OFFSET(pContext->lpLcl->lpGbl->fpVidMem);
            pContext->surfacePitch.wColorPitch = (WORD)pContext->lpLcl->lpGbl->lPitch;
        }
        if (pContext->lpLclZ)
        {
            /*
             * I've seen it happen where the location of the z-buffer will actually
             * change without without the context being destroyed or the set render
             * target call happening. So it's neccessary to make sure that we have the
             * correct z-buffer address and offset here.
             */
            pContext->ZBufferAddr             = VIDMEM_ADDR(pContext->lpLclZ->lpGbl->fpVidMem);
            pContext->ZBufferOffset           = VIDMEM_OFFSET(pContext->lpLclZ->lpGbl->fpVidMem);
            pContext->surfacePitch.wZetaPitch = (WORD)pContext->lpLclZ->lpGbl->lPitch;
        }
        /*
         * Send to hardware
         */
        while (nvFreeCount < (sizeSetObject + sizeSetRenderTargetContexts))
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, (sizeSetObject + sizeSetRenderTargetContexts));
        nvglSetObject(nvFifo, nvFreeCount, NV_DD_SURFACES, D3D_CONTEXT_SURFACES_ARGB_ZS);
        nvglSetRenderTargetContexts(nvFifo, nvFreeCount, NV_DD_SURFACES,
                                    NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

        /*
         * AA overrides (AA is disabled when dwAntiAliasFlags == 0)
         */
        if (pContext->dwAntiAliasFlags & AA_MODE_SINGLECHUNK)
        {
            /*
             * single chunk super sampled aa
             */
            PNVAACONTEXT pCtx = pContext->pAAContext;
            DWORD clip  = (pCtx->dwSuperHeight << 16)
                        |  pCtx->dwSuperWidth;
            DWORD pitch = (pCtx->dwSuperZPitch << 16)
                        |  pCtx->dwSuperPitch;
            while (nvFreeCount < sizeSetRenderTarget2)
                nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, sizeSetRenderTarget2);
            nvglSetRenderTarget2(nvFifo, nvFreeCount, NV_DD_SURFACES,
                                 pContext->dwSurfaceFormat, clip,
                                 pitch,  pCtx->dwSuperOffset,
                                 pCtx->dwSuperZOffset);
        }
        else
        if (pContext->dwAntiAliasFlags & AA_RENDER)
        {
            /*
             * multi chunk super sampled aa
             */
            PNVAACONTEXT pCtx = pContext->pAAContext;
            DWORD clip  = (pCtx->dwSuperHeight << 16)
                        |  pCtx->dwSuperWidth;
            DWORD pitch = (pContext->surfacePitch.wZetaPitch << 16)
                        |  pCtx->dwSuperPitch;
            while (nvFreeCount < sizeSetRenderTarget2)
                nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, sizeSetRenderTarget2);
            nvglSetRenderTarget2(nvFifo, nvFreeCount, NV_DD_SURFACES,
                                 pContext->dwSurfaceFormat, clip,
                                 pitch,  pCtx->dwSuperOffset,
                                 pContext->ZBufferOffset);
        }
        else
        /*
         * Normal case
         */
        {
            if (!(pContext->surfaceViewport.clipHorizontal.wX)
             && !(pContext->surfaceViewport.clipVertical.wY))
            {
                DWORD clip = (((DWORD)pContext->surfaceViewport.clipVertical.wHeight) << 16)
                           | pContext->surfaceViewport.clipHorizontal.wWidth;
                while (nvFreeCount < sizeSetRenderTarget2)
                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, sizeSetRenderTarget2);
                nvglSetRenderTarget2(nvFifo, nvFreeCount, NV_DD_SURFACES,
                                     pContext->dwSurfaceFormat, clip,
                                     pContext->dwSurfacePitch,  pContext->dwSurfaceOffset,
                                     pContext->ZBufferOffset);
            }
            else
            {
#if 0
                /*
                 * If this didn't cause an exception to the RM, this is how I'd program this.
                 */
                while (nvFreeCount < sizeSetRenderTarget)
                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, sizeSetRenderTarget);
                nvglSetRenderTarget(nvFifo, nvFreeCount, NV_DD_SURFACES,
                                    pContext->surfaceViewport.dwClipHorizontal, pContext->surfaceViewport.dwClipVertical,
                                    pContext->dwSurfaceFormat,
                                    pContext->dwSurfacePitch,  pContext->dwSurfaceOffset,
                                    pContext->ZBufferOffset);
#else
                /*
                 * Until programming the clipped viewport doesn't cause an RM exception,
                 * program it this way.
                 */
                DWORD clip = (((DWORD)pContext->surfaceViewport.clipVertical.wHeight) << 16)
                           | pContext->surfaceViewport.clipHorizontal.wWidth;
                while (nvFreeCount < (sizeSetRenderTarget2 + sizeSetObject + sizeSetClip))
                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, (sizeSetRenderTarget2 + sizeSetObject + sizeSetClip));
                nvglSetRenderTarget2(nvFifo, nvFreeCount, NV_DD_SURFACES,
                                     pContext->dwSurfaceFormat, clip,
                                     pContext->dwSurfacePitch,  pContext->dwSurfaceOffset,
                                     pContext->ZBufferOffset);

                /*
                 * Set the image black rectangle to the clip region.
                 */
                nvglSetObject(nvFifo, nvFreeCount, NV_DD_SURFACES, NV_DD_IMAGE_BLACK_RECTANGLE);
                nvglSetClip(nvFifo, nvFreeCount, NV_DD_SURFACES,
                            pContext->surfaceViewport.clipHorizontal.wX, pContext->surfaceViewport.clipVertical.wY,
                            pContext->surfaceViewport.clipHorizontal.wWidth, pContext->surfaceViewport.clipVertical.wHeight);
#endif
            }
        }
        while (nvFreeCount < sizeSetObject)
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, sizeSetObject);
        nvglSetObject(nvFifo, nvFreeCount, NV_DD_SURFACES, NV_DD_SURFACES_2D);
        pDriverData->TwoDRenderingOccurred = 0;
        pDriverData->ThreeDRenderingOccurred = TRUE;
        pDriverData->dDrawSpareSubchannelObject = 0; // force a reload.
    }
    pDriverData->lpLast3DSurfaceRendered = pContext->lpLcl->lpGbl->fpVidMem;
    pDriverData->ddClipUpdate = TRUE;
    pDriverData->dwSharedClipChangeCount++;
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}
void nvSetD3DSurfaceViewport
(
    PNVD3DCONTEXT   pContext
)
{
    /*
     * AA does not need to tell the HW about the viewport at this point
     *  early exit
     */
    if (pContext->dwAntiAliasFlags & (AA_MODE_SUPER | AA_MODE_SINGLECHUNK))
    {
        return;
    }

    /*
     * Send to hardware
     */
#if 0
    /*
     * If this didn't cause an exception to the RM, this is how I'd program this.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetRenderTargetViewport + sizeSetObject))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, (sizeSetObject + sizeSetRenderTargetViewport + sizeSetObject));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SURFACES, D3D_CONTEXT_SURFACES_ARGB_ZS);
    nvglSetRenderTargetViewport(nvFifo, nvFreeCount, NV_DD_SURFACES,
                                pContext->surfaceViewport.dwClipHorizontal, pContext->surfaceViewport.dwClipVertical);
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SURFACES, NV_DD_SURFACES_2D);
#else
    /*
     * Until programming the clipped viewport doesn't cause an RM exception,
     * program it this way.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetClip + sizeSetObject))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, (sizeSetObject + sizeSetClip + sizeSetObject));

    /*
     * Set the image black rectangle to the clip region.
     */
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SURFACES, NV_DD_IMAGE_BLACK_RECTANGLE);
    nvglSetClip(nvFifo, nvFreeCount, NV_DD_SURFACES,
                pContext->surfaceViewport.clipHorizontal.wX, pContext->surfaceViewport.clipVertical.wY,
                pContext->surfaceViewport.clipHorizontal.wWidth, pContext->surfaceViewport.clipVertical.wHeight);

    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SURFACES, NV_DD_SURFACES_2D);
#endif
    pDriverData->TwoDRenderingOccurred      = 0;
    pDriverData->ThreeDRenderingOccurred    = TRUE;
    pDriverData->dDrawSpareSubchannelObject = 0; // force a reload.
    pDriverData->lpLast3DSurfaceRendered    = pContext->lpLcl->lpGbl->fpVidMem;
    pDriverData->ddClipUpdate = TRUE;
    pDriverData->dwSharedClipChangeCount++;
    NV_FORCE_TRI_SETUP(pContext);
}

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

    /*
     * Validate the inner loop hardware control registers.
     */
    dbgValidateControlRegisters(pCurrentContext);

    /*
     * AA semantics check - I hate these
     */
    if (pContext->dwAntiAliasFlags & AA_ENABLED_MASK)
        pContext->dwAntiAliasFlags |= AA_MIX_DP_NOAA;
    else
        pContext->dwAntiAliasFlags |= AA_MIX_DP_AA;

    /*
     * we do not send state when we capture geometry for super sampled AA
     */
    if (pCurrentContext->dwAntiAliasFlags & AA_MODE_SUPER)
    {
        if (!(pCurrentContext->dwAntiAliasFlags & AA_CAPTURE_PUSHBUFFER))
        {
            return;
        }
    }

#if 0
    /*
     * Send down the triangle state.
     */
    while (nvFreeCount < (sizeSetObject + sizeDx5TriangleState))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeDx5TriangleState));
    pTriangleState = (DWORD *)&pContext->ctxInnerLoop;
    /*
     * Only write channel if it is not selected - avoids texture cache invalidates
     */
    if (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE)
    {
        nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);
    }
    nvglDx5TriangleState(nvFifo, nvFreeCount, NV_DD_SPARE,
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
        while (nvFreeCount < sizeSetObject)
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetObject);
        nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);
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
        while (nvFreeCount < (sizeSetData * 2 + sizeSetStartMethod * 1))
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetData * 2 + sizeSetStartMethod * 1));
        nvglSetStartMethod (nvFifo,nvFreeCount,NV_DD_SPARE,NV054_OFFSET,2);
        nvglSetData        (nvFifo,nvFreeCount,            pTriangleState[1]);
        pTriangleStateShadow[1] = pTriangleState[1];
        nvglSetData        (nvFifo,nvFreeCount,            pTriangleState[2]);
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
        while (nvFreeCount < (sizeSetData * 5 + sizeSetStartMethod * 2))
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetData * 5 + sizeSetStartMethod * 2));
        nvglSetStartMethod (nvFifo,nvFreeCount,NV_DD_SPARE,NV054_COLOR_KEY,1);
        nvglSetData        (nvFifo,nvFreeCount,            pTriangleState[0]);
        pTriangleStateShadow[0] = pTriangleState[0];
        nvglSetStartMethod (nvFifo,nvFreeCount,NV_DD_SPARE,NV054_FILTER,4);
        nvglSetData        (nvFifo,nvFreeCount,            pTriangleState[3]);
        pTriangleStateShadow[3] = pTriangleState[3];
#ifdef  SPEC_HACK
        /*
         * Send the current blend state. This is an attempt to maximize the ability to
         * get 2 pixels per clock out of the DX5 class when specular highlighting is
         * enabled.
         */
        nvglSetData        (nvFifo,nvFreeCount,            pTriangleState[8]);
#else   // SPEC_HACK
        nvglSetData        (nvFifo,nvFreeCount,            pTriangleState[4]);
#endif
        pTriangleStateShadow[4] = pTriangleState[4];
        nvglSetData        (nvFifo,nvFreeCount,            pTriangleState[5]);
        pTriangleStateShadow[5] = pTriangleState[5];
        nvglSetData        (nvFifo,nvFreeCount,            pTriangleState[6]);
        pTriangleStateShadow[6] = pTriangleState[6];
    }
#endif

    return;
}

/*
 * Context and hardware state set routines.
 */
DWORD nvSetContextState
(
    DWORD           dwStateType,
    DWORD           dwStateValue,
    HRESULT        *pddrval
)
{
    /*
     * Make sure the state being set is within the range the driver is handling.
     */
    if (dwStateType <= D3D_RENDERSTATE_MAX)
    {
        /*
         * Only update the state if it's different.
         */
        if (pCurrentContext->dwRenderState[dwStateType] != dwStateValue)
        {
            if (dbgShowState & NVDBG_SHOW_RENDER_STATE)
            {
                DPF("nvSetContextState:Change Render State");
                DPF("State = %08ld", dwStateType);
                DPF("Data  = 0x%08lx", dwStateValue);
            }
            pCurrentContext->dwRenderState[dwStateType] = dwStateValue;
            pCurrentContext->dwStateChange = TRUE;

            /*
             * Handle some special case render states now.
             */
            switch (dwStateType)
            {
                case D3DRENDERSTATE_TEXTUREHANDLE:
                    if (dwStateValue)
                    {
                        pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE] |= 0x80000000;
#ifdef  NVD3D_DX6
                        pCurrentContext->tssState[0].dwTextureMap = pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE];
#endif  // NVD3D_DX6
                    }
                    break;
                case D3DRENDERSTATE_TEXTUREADDRESS:
                    pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSU] = dwStateValue;
                    pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSV] = dwStateValue;
                    break;
                case D3DRENDERSTATE_MIPMAPLODBIAS:
                    /*
                     * Convert LODBIAS value to NV hardware value.
                     * Here is the true meaning of this value as implemented by the reference rasterizer:
                     *    Floating-point D3DVALUE value used to change the level of detail (LOD) bias.
                     *    This value offsets the value of the mipmap level that is computed by trilinear
                     *    texturing. It is usually in the range  -1.0 to 1.0; the default value is 0.0.
                     *    Each unit bias (+/-1.0) biases the selection by exactly one mipmap level.
                     *    A negative bias will cause the use of larger mipmap levels, resulting in a
                     *    sharper but more aliased image.
                     *    A positive bias will cause the use of smaller mipmap levels, resulting in a
                     *    blurrier image.
                     *    Applying a positive bias also results in the referencing of a smaller amount
                     *    of texture data, which can boost performance on some systems.
                     */
                    if (!pCurrentContext->dwRenderState[D3DRENDERSTATE_MIPMAPLODBIAS])
                        pCurrentContext->dwMipMapLODBias = pDriverData->regLODBiasAdjust;
                    else
                    {
                        D3DVALUE dvLODBias;

                        dvLODBias = *((D3DVALUE *)&(pCurrentContext->dwRenderState[D3DRENDERSTATE_MIPMAPLODBIAS]));
                        if (dvLODBias > 15.0f)
                            dvLODBias = 15.0f;
                        else if (dvLODBias < -15.0f)
                            dvLODBias = -15.0f;
                        /*
                         * Convert the floating point value from D3D into a 5.3 NV value.
                         * 0x08 == 1.0f
                         */
                        dvLODBias *= 8.0;
                        pCurrentContext->dwMipMapLODBias = (DWORD)((long)dvLODBias & 0x000000FF);
                    }
                    break;

                /*
                 * Stupid DX will occasionally send a WrapU/WrapV other than TRUE(1) or FALSE(0)
                 * which can cause problems when the value is or'd directly into the hareware
                 * register.  Make sure that our value is only 0 or 1.
                 */
                case D3DRENDERSTATE_WRAPU:
                    if (dwStateValue)
                        pCurrentContext->dwRenderState[D3DRENDERSTATE_WRAPU] = TRUE;
                    else
                        pCurrentContext->dwRenderState[D3DRENDERSTATE_WRAPU] = FALSE;
                    break;
                case D3DRENDERSTATE_WRAPV:
                    if (dwStateValue)
                        pCurrentContext->dwRenderState[D3DRENDERSTATE_WRAPV] = TRUE;
                    else
                        pCurrentContext->dwRenderState[D3DRENDERSTATE_WRAPV] = FALSE;
                    break;
                case D3DRENDERSTATE_FILLMODE:
                    if (!(dwStateValue)
                     || (dwStateValue > D3DFILL_SOLID))
                    {
                        pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE] = D3DFILL_SOLID;
                    }
                    break;
#ifdef  NVD3D_DX6
                case D3DRENDERSTATE_TEXTUREMIN:
                    switch (dwStateValue)
                    {
                        case D3DFILTER_NEAREST:
                            pCurrentContext->tssState[0].dwMinFilter = D3DTFN_POINT;
                            pCurrentContext->tssState[0].dwMipFilter = D3DTFP_NONE;
                            break;
                        case D3DFILTER_LINEAR:
                            pCurrentContext->tssState[0].dwMinFilter = D3DTFN_LINEAR;
                            pCurrentContext->tssState[0].dwMipFilter = D3DTFP_NONE;
                            break;
                        case D3DFILTER_MIPNEAREST:
                            pCurrentContext->tssState[0].dwMinFilter = D3DTFN_POINT;
                            pCurrentContext->tssState[0].dwMipFilter = D3DTFP_POINT;
                            break;
                        case D3DFILTER_MIPLINEAR:
                            pCurrentContext->tssState[0].dwMinFilter = D3DTFN_LINEAR;
                            pCurrentContext->tssState[0].dwMipFilter = D3DTFP_POINT;
                            break;
                        case D3DFILTER_LINEARMIPNEAREST:
                            pCurrentContext->tssState[0].dwMinFilter = D3DTFN_POINT;
                            pCurrentContext->tssState[0].dwMipFilter = D3DTFP_LINEAR;
                            break;
                        case D3DFILTER_LINEARMIPLINEAR:
                            pCurrentContext->tssState[0].dwMinFilter = D3DTFN_LINEAR;
                            pCurrentContext->tssState[0].dwMipFilter = D3DTFP_LINEAR;
                            break;
                        default:
                            pCurrentContext->tssState[0].dwMinFilter = D3DTFN_POINT;
                            pCurrentContext->tssState[0].dwMipFilter = D3DTFP_NONE;
                            break;
                    }
                    break;
                case D3DRENDERSTATE_TEXTUREMAG:
                    switch (dwStateValue)
                    {
                        case D3DFILTER_NEAREST:
                            pCurrentContext->tssState[0].dwMagFilter = D3DTFG_POINT;
                            break;
                        case D3DFILTER_LINEAR:
                            pCurrentContext->tssState[0].dwMagFilter = D3DTFG_LINEAR;
                            break;
                        default:
                            pCurrentContext->tssState[0].dwMagFilter = D3DTFG_POINT;
                            break;
                    }
                    break;
#if 0
                case D3DRENDERSTATE_TEXTUREMAPBLEND:
                {
                    DWORD   i;
                    /*
                     * When we get a texturemapblend renderstate, we will also modify the texture stage
                     * state to reflect this change. An application should not be using both TEXTUREMAPBLEND
                     * as well as texture stages so this should be a safe thing to do.  But, as the docs say:
                     *
                     *      Legacy Blending Modes and Texture Stages
                     *
                     *      Although Direct3D still supports the texture blending render state,
                     *      D3DRENDERSTATE_TEXTUREMAPBLEND, the blending modes it offers should not be
                     *      used in combination with texture stage based texture blending, as the
                     *      results can be unpredictable.
                     */
                    switch (dwStateValue)
                    {
                        case D3DTBLEND_DECAL:
                        case D3DTBLEND_COPY:
                        case D3DTBLEND_DECALMASK:
                            pCurrentContext->tssState[0].dwColorOp   = D3DTOP_SELECTARG1;
                            pCurrentContext->tssState[0].dwColorArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwColorArg2 = D3DTA_DIFFUSE;
                            pCurrentContext->tssState[0].dwAlphaOp   = D3DTOP_SELECTARG1;
                            pCurrentContext->tssState[0].dwAlphaArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwAlphaArg2 = D3DTA_DIFFUSE;
                            break;
                        case D3DTBLEND_MODULATE:
                        case D3DTBLEND_MODULATEMASK:
                            pCurrentContext->tssState[0].dwColorOp   = D3DTOP_MODULATE;
                            pCurrentContext->tssState[0].dwColorArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwColorArg2 = D3DTA_DIFFUSE;
                            pCurrentContext->tssState[0].dwAlphaOp   = D3DTOP_SELECTARG1;
                            pCurrentContext->tssState[0].dwAlphaArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwAlphaArg2 = D3DTA_DIFFUSE;
                            break;
                        case D3DTBLEND_DECALALPHA:
                            pCurrentContext->tssState[0].dwColorOp   = D3DTOP_SELECTARG1;
                            pCurrentContext->tssState[0].dwColorArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwColorArg2 = D3DTA_DIFFUSE;
                            pCurrentContext->tssState[0].dwAlphaOp   = D3DTOP_SELECTARG1;
                            pCurrentContext->tssState[0].dwAlphaArg1 = D3DTA_DIFFUSE;
                            pCurrentContext->tssState[0].dwAlphaArg2 = D3DTA_DIFFUSE;
                            break;
                        case D3DTBLEND_MODULATEALPHA:
                            pCurrentContext->tssState[0].dwColorOp   = D3DTOP_MODULATE;
                            pCurrentContext->tssState[0].dwColorArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwColorArg2 = D3DTA_DIFFUSE;
                            pCurrentContext->tssState[0].dwAlphaOp   = D3DTOP_MODULATE;
                            pCurrentContext->tssState[0].dwAlphaArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwAlphaArg2 = D3DTA_DIFFUSE;
                            break;
                        case D3DTBLEND_ADD:
                            pCurrentContext->tssState[0].dwColorOp   = D3DTOP_ADD;
                            pCurrentContext->tssState[0].dwColorArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwColorArg2 = D3DTA_DIFFUSE;
                            pCurrentContext->tssState[0].dwAlphaOp   = D3DTOP_SELECTARG1;
                            pCurrentContext->tssState[0].dwAlphaArg1 = D3DTA_DIFFUSE;
                            pCurrentContext->tssState[0].dwAlphaArg2 = D3DTA_DIFFUSE;
                            break;
                        default:
                            pCurrentContext->tssState[0].dwColorOp   = D3DTOP_MODULATE;
                            pCurrentContext->tssState[0].dwColorArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwColorArg2 = D3DTA_DIFFUSE;
                            pCurrentContext->tssState[0].dwAlphaOp   = D3DTOP_MODULATE;
                            pCurrentContext->tssState[0].dwAlphaArg1 = D3DTA_TEXTURE;
                            pCurrentContext->tssState[0].dwAlphaArg2 = D3DTA_DIFFUSE;
                            break;
                    }
                    /*
                     * Disable all but stage 0.
                     */
                    for (i = 1; i < 8; i++)
                    {
                        pCurrentContext->tssState[i].dwColorOp = D3DTOP_DISABLE;
                        pCurrentContext->tssState[i].dwAlphaOp = D3DTOP_DISABLE;
                    }
                    break;
                }
#endif
                case D3DRENDERSTATE_WRAP0:
                    pCurrentContext->dwRenderState[D3DRENDERSTATE_WRAPU] =
                    pCurrentContext->tssState[0].bWrapU                  = ((dwStateValue & D3DWRAP_U) == D3DWRAP_U);
                    pCurrentContext->dwRenderState[D3DRENDERSTATE_WRAPV] =
                    pCurrentContext->tssState[0].bWrapV                  = ((dwStateValue & D3DWRAP_V) == D3DWRAP_V);
                    break;
                case D3DRENDERSTATE_WRAP1:
                case D3DRENDERSTATE_WRAP2:
                case D3DRENDERSTATE_WRAP3:
                case D3DRENDERSTATE_WRAP4:
                case D3DRENDERSTATE_WRAP5:
                case D3DRENDERSTATE_WRAP6:
                case D3DRENDERSTATE_WRAP7:
                    pCurrentContext->tssState[dwStateType - D3DRENDERSTATE_WRAP0].bWrapU = ((dwStateValue & D3DWRAP_U) == D3DWRAP_U);
                    pCurrentContext->tssState[dwStateType - D3DRENDERSTATE_WRAP0].bWrapV = ((dwStateValue & D3DWRAP_V) == D3DWRAP_V);
                    break;
#endif  // NVD3D_DX6
                case D3DRENDERSTATE_ANTIALIAS:
                    {
                        BOOL bWasEnabled;
                        BOOL bWillEnable;

                        /*
                         * prep for toggle counts
                         */
                        bWasEnabled = (pCurrentContext->dwAntiAliasFlags & AA_ENABLED_MASK) ? 1 : 0;
                        bWillEnable =  dwStateValue != D3DANTIALIAS_NONE;

                        /*
                         * assume disabled
                         */
                        pCurrentContext->dwAntiAliasFlags &= ~(AA_ENABLED_MASK | AA_2D_COUNT_MASK | AA_MIX_MASK);

                        /*
                         * check master switch
                         */
                        if (pDriverData->regD3DEnableBits1 & REG_BIT1_ANTI_ALIAS_ENABLE)
                        {
                            /*
                             * enable aa only if app request aa and is behaving well
                             */
                            if ((dwStateValue != D3DANTIALIAS_NONE)
                             && !(pCurrentContext->dwAntiAliasFlags & AA_INVALID_SEMANTICS))
                            {
                                /*
                                 * decide which AA strategy to use depending on registry
                                 */
                                // aatodo - check aamethod in registry
                                if (1)
                                {
                                    /*
                                     * enable super sampled AA
                                     */
                                    if (pCurrentContext->dwAntiAliasFlags & AA_BUFALLOC_MASK)
                                    {
                                        /*
                                         * enable correct aa mode depending on buffers that where allocated
                                         */
                                        if (pCurrentContext->dwAntiAliasFlags & AA_BUFALLOC_SUPER)
                                            pCurrentContext->dwAntiAliasFlags |= AA_MODE_SUPER;
                                        if (pCurrentContext->dwAntiAliasFlags & AA_BUFALLOC_SINGLECHUNK)
                                            pCurrentContext->dwAntiAliasFlags |= AA_MODE_SINGLECHUNK;
                                        if (pCurrentContext->dwAntiAliasFlags & AA_BUFALLOC_EDGE)
                                            pCurrentContext->dwAntiAliasFlags |= AA_MODE_EDGE;
                                    }
                                    else
                                    {
                                        pCurrentContext->dwAntiAliasFlags |= AA_MODE_SUPER;
                                    }
                                }
                                else
                                {
                                    /*
                                     * enable edge AA
                                     */
                                    pCurrentContext->dwAntiAliasFlags |= AA_MODE_EDGE;
                                }

                                /*
                                 * allocate proper buffers if needed
                                 */
                                if (!(pCurrentContext->dwAntiAliasFlags & AA_BUFALLOC_MASK))
                                {
                                    if (!nvAACreateBuffers())
                                    {
                                        /*
                                         * fail to create buffers - punt
                                         */
                                        pCurrentContext->dwAntiAliasFlags &= ~(AA_ENABLED_MASK | AA_BUFALLOC_MASK);
                                        *pddrval = DDERR_OUTOFVIDEOMEMORY;
                                        return (DDHAL_DRIVER_HANDLED);
                                    }
                                }
                            }
                        }
                    }
                    break;
            }
        }
    }

    /*
     * Force a reload of the global context state.
     */
    pDriverData->dwCurrentContextHandle = 0;
    *pddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
/*
 * Calculate the hardware state register based on the D3D context state settings.
 */
void nvCalculateHardwareStateDX5
(
    DWORD   dwTBlendState,
    DWORD   dwMinFilter,
    DWORD   dwMagFilter,
    BOOL    bDx6ToDx5Conversion
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
    PNVD3DTEXTURE   pTexture;

    pdwRenderState = (DWORD *)pCurrentContext->dwRenderState;

    // txtodo - validate swizzled texture copy


    dwZOHMode     = ((pDriverData->regTexelAlignment & REG_TA_ZOH_MASK) == REG_TA_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
    dwFOHMode     = ((pDriverData->regTexelAlignment & REG_TA_FOH_MASK) == REG_TA_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;
    dwTexelOrigin = ((pDriverData->regTexelAlignment & REG_TA_TEXEL_MASK) == REG_TA_TEXEL_CENTER)
                  ? NV054_CONTROL_ORIGIN_CENTER : NV054_CONTROL_ORIGIN_CORNER;

    /*
     * Maximize 2 pixels per clock.
     */
    dwMMDE = ((pdwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_ONE) && (pdwRenderState[D3DRENDERSTATE_DESTBLEND] == D3DBLEND_ONE)) || ((pDriverData->regD3DEnableBits1 & REG_BIT1_MIPMAP_DITHER_ENABLE) == REG_BIT1_MIPMAP_DITHER_ENABLE);

    /*
     * Set up the texture format register.
     */
    if (pTexture = (PNVD3DTEXTURE)pdwRenderState[D3DRENDERSTATE_TEXTUREHANDLE])
        NV_LOCK_TEXTURE_SURFACE(pTexture);
    if ((pTexture)
     && (pTexture->lpLcl))
    {
        /*
         * DEBUG only.
         * Display the new texture to the current visible buffer.
         */
        dbgDisplayTexture(pCurrentContext, pTexture);

#ifdef NV_TEX2
        /*
         * setup current texture(s)
         */
        pCurrentContext->pTexture0 = pTexture;
        pCurrentContext->pTexture1 = NULL;
#endif

        /*
         * Grab the color key from the texture.
         */
        dwTextureColorFormat = pTexture->dwTextureColorFormat;
        if (pTexture->lpLcl->dwFlags & DDRAWISURF_HASCKEYSRCBLT)
        {
#ifndef NV_TEX2
            if (pDriverData->regD3DEnableBits1 & REG_BIT1_RESTRICT_AUTO_MIPMAPS)
                pTexture->dwTextureFlags &= ~NV4_TEXTURE_CAN_AUTO_MIPMAP;
#endif
            switch (dwTextureColorFormat)
            {
                case NV054_FORMAT_COLOR_LE_Y8:
                    break;
                /*
                 * Must assume the application does the right thing here and passes in the
                 * exact color key value as is in the texture map.  If the alpha bits are masked
                 * here, WHQL will fail.
                 */
                case NV054_FORMAT_COLOR_LE_A1R5G5B5:
                case NV054_FORMAT_COLOR_LE_A4R4G4B4:
//                case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                case NV054_FORMAT_COLOR_LE_R5G6B5:
                    dwColorKey = (pTexture->lpLcl->ddckCKSrcBlt.dwColorSpaceHighValue & 0x0000FFFF);
                    break;
                case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                    dwColorKey = (pTexture->lpLcl->ddckCKSrcBlt.dwColorSpaceHighValue & 0x00007FFF);
                    break;
                case NV054_FORMAT_COLOR_LE_A8R8G8B8:
                case NV054_FORMAT_COLOR_LE_X8R8G8B8:
                    dwColorKey = pTexture->lpLcl->ddckCKSrcBlt.dwColorSpaceHighValue;
                    break;
            }
            dwFormat |= DRF_NUM(054, _FORMAT, _COLORKEYENABLE,  pdwRenderState[D3DRENDERSTATE_COLORKEYENABLE]);
        }
        else
            dwFormat |= DRF_DEF(054, _FORMAT, _COLORKEYENABLE,  _FALSE);

        /*
         * Get the texture format and offset.
         */
#ifdef NV_TEX2
        dwOffset  = TEX_SWIZZLE_OFFSET(pTexture,pTexture->dwTextureFlags);
#else
        dwOffset  = pTexture->dwTextureOffset;
#endif
        dwFormat |= DRF_NUM(054, _FORMAT, _CONTEXT_DMA,      pTexture->dwTextureContextDma)
                 |  DRF_NUM(054, _FORMAT, _COLOR,            dwTextureColorFormat)
                 |  DRF_NUM(054, _FORMAT, _BASE_SIZE_U,      pTexture->dwMipMapBaseU)
                 |  DRF_NUM(054, _FORMAT, _BASE_SIZE_V,      pTexture->dwMipMapBaseV)
                 |  DRF_NUM(054, _FORMAT, _TEXTUREADDRESSU,  pdwRenderState[D3DRENDERSTATE_TEXTUREADDRESSU])
                 |  DRF_NUM(054, _FORMAT, _WRAPU,            pdwRenderState[D3DRENDERSTATE_WRAPU])
                 |  DRF_NUM(054, _FORMAT, _TEXTUREADDRESSV,  pdwRenderState[D3DRENDERSTATE_TEXTUREADDRESSV])
                 |  DRF_NUM(054, _FORMAT, _WRAPV,            pdwRenderState[D3DRENDERSTATE_WRAPV]);
        dwFormat |= DRF_NUM(054, _FORMAT, _ORIGIN_ZOH,       dwZOHMode)
                 |  DRF_NUM(054, _FORMAT, _ORIGIN_FOH,       dwFOHMode);

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

        if ((pTexture->lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
         || (pTexture->dwMipMapLevels == 1))
        {
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
            if ((dwMinFilter == D3DFILTER_NEAREST)
             || (dwMinFilter == D3DFILTER_LINEAR))
                dwFormat |= DRF_NUM(054, _FORMAT, _MIPMAP_LEVELS, 1);
            else
                dwFormat |= DRF_NUM(054, _FORMAT, _MIPMAP_LEVELS, pTexture->dwMipMapLevels);
        }
        else
        {
#ifdef NV_TEX2
            if (pTexture->dwTextureFlags & NV4_TEXFLAG_AUTOMIPMAP)
#else
            if (pTexture->dwTextureFlags & NV4_TEXTURE_CAN_AUTO_MIPMAP)
#endif
            {
                /*
                 * This is the auto-generated mipmap case. In order to get the some sort of mipmapping
                 * to happen, the MIN/MAG filters must be modified if they are NEAREST or LINEAR.
                 */
                dwFilter |= DRF_NUM(054, _FILTER, _MIPMAP_DITHER_ENABLE, dwMMDE);
                dwFormat |= DRF_NUM(054, _FORMAT, _MIPMAP_LEVELS,  pTexture->dwMipMapLevels);

                /*
                 * Set the MINIFICATION filter.
                 */
                if (pDriverData->regAutoMipMapMethod == REG_AMMM_BILINEAR)
                {
                    if (dwMinFilter == D3DFILTER_NEAREST)
                        dwFilter |= DRF_DEF(054, _FILTER, _TEXTUREMIN, _MIPNEAREST);
                    else if (dwMinFilter == D3DFILTER_LINEAR)
                        dwFilter |= DRF_DEF(054, _FILTER, _TEXTUREMIN, _MIPLINEAR);
                    else
                        dwFilter |= DRF_NUM(054, _FILTER, _TEXTUREMIN, dwMinFilter);
                }
                else
                {
                    if (dwMinFilter == D3DFILTER_NEAREST)
                        dwFilter |= DRF_DEF(054, _FILTER, _TEXTUREMIN, _LINEARMIPNEAREST);
                    else if (dwMinFilter == D3DFILTER_LINEAR)
                        dwFilter |= DRF_DEF(054, _FILTER, _TEXTUREMIN, _LINEARMIPLINEAR);
                    else
                        dwFilter |= DRF_NUM(054, _FILTER, _TEXTUREMIN, dwMinFilter);
                }

                /*
                 * Set the MAGNIFICATION filter.
                 * Can only validly be set to NEAREST or LINEAR
                 */
                dwFilter |= DRF_NUM(054, _FILTER, _TEXTUREMAG,    dwMagFilter);
            }
            else
            {
                /*
                 * This texture is flagged as not a auto-generated mipmapable texture.
                 */
                dwFilter |= DRF_NUM(054, _FILTER, _MIPMAP_DITHER_ENABLE, dwMMDE)
                         |  DRF_NUM(054, _FILTER, _TEXTUREMIN,    dwMinFilter)
                         |  DRF_NUM(054, _FILTER, _TEXTUREMAG,    dwMagFilter);
                dwFormat |= DRF_NUM(054, _FORMAT, _MIPMAP_LEVELS, 1);
            }
        }

        /*
         * When texturing, always select the proper TexturePerspective render state value.
         */
        dwTexturePerspective = pdwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE];
        NV_UNLOCK_TEXTURE_SURFACE(pTexture);
    }
    else
    {
        /*
         * There is no texture set the default texture format and offset.
         */
        dwTextureColorFormat = pCurrentContext->dwDefaultTextureColorFormat;
        dwOffset  = pCurrentContext->dwDefaultTextureOffset;
        dwFormat |= DRF_NUM(054, _FORMAT, _CONTEXT_DMA,      pCurrentContext->dwDefaultTextureContextDma)
                 |  DRF_DEF(054, _FORMAT, _COLORKEYENABLE,  _FALSE)
                 |  DRF_NUM(054, _FORMAT, _COLOR,            dwTextureColorFormat)
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
         * When there is no texture, always set texture map blend to be MODULATEALPHA
         */
        if (pdwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE])
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
    }

    /*
     * Set the Mipmap LOD BIAS based on registery setting and render state setting.
     */
    dwFilter |= DRF_NUM(054, _FILTER, _MIPMAPLODBIAS, pCurrentContext->dwMipMapLODBias);

    /*
     * D3DRENDERSTATE_ANISOTROPY defines 1 as no anisotropy.
     * The following if statement is based on the reference rasterizer.
     */
    if (pdwRenderState[D3DRENDERSTATE_ANISOTROPY] > 1)
    {
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
#ifdef NV_CONTROLTRAFFIC
              |  DRF_NUM(054, _CONTROL, _ZFUNC,                CTFunc(pdwRenderState[D3DRENDERSTATE_ZFUNC]))
#else
              |  DRF_NUM(054, _CONTROL, _ZFUNC,                pdwRenderState[D3DRENDERSTATE_ZFUNC])
#endif
#ifdef NV_FASTLOOPS
              |  DRF_NUM(054, _CONTROL, _CULLMODE,             D3DCULL_NONE)
#else
              |  DRF_NUM(054, _CONTROL, _CULLMODE,             pdwRenderState[D3DRENDERSTATE_CULLMODE])
#endif
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
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    ColorKey Color     = %08lx", dwColorKey);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    AlphaTestEnable    = %08lx", pdwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    AlphaRef           = %08lx", pdwRenderState[D3DRENDERSTATE_ALPHAREF]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    AlphaFunc          = %08lx", pdwRenderState[D3DRENDERSTATE_ALPHAFUNC]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    AlphaBlendEnable   = %08lx", pdwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    SrcBlend           = %08lx", pdwRenderState[D3DRENDERSTATE_SRCBLEND]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    DstBlend           = %08lx", pdwRenderState[D3DRENDERSTATE_DESTBLEND]);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    TextureMapBlend    = %08lx", dwTBlendState);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    TextureColorFormat = %08lx", dwTextureColorFormat);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    MagFliter          = %08lx", dwMagFilter);
        DPF_LEVEL(NVDBG_LEVEL_COLORKEY_INFO, "    MinFliter          = %08lx", dwMinFilter);

        bAlphaTexture = ((dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A1R5G5B5)
                      || (dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A4R4G4B4)
                      || (dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A8R8G8B8));

        if (!(pDriverData->regD3DEnableBits1 & REG_BIT1_CKCOMPATABILITY_ENABLE))
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

            /*
             * Special case for non-alpha textures when blending is enabled and the source blend
             * is set to D3DBLEND_ONE and the texturemap blend is set to modulate alpha.  Switch
             * the TBLEND to be modulate since the vertex alpha doesn't matter.
             * But one other requirement is that the SRCALPHA cannot be involved in the destblend either.
             * This will prevent vertices with alphas set to all zero to not get keyed away.
             */
            if ((dwAlphaBlendEnable) //&& (!bAlphaTexture)
             && (dwTBlendState == D3DTBLEND_MODULATEALPHA)
             && (dwSrcBlend == D3DBLEND_ONE)
             && ((dwDstBlend != D3DBLEND_SRCALPHA)
              && (dwDstBlend != D3DBLEND_INVSRCALPHA)
              && (dwDstBlend != D3DBLEND_SRCALPHASAT)
              && (dwDstBlend != D3DBLEND_BOTHSRCALPHA)
              && (dwDstBlend != D3DBLEND_BOTHINVSRCALPHA)))
            {
                dwBlend &= ~DRF_MASK(NV054_BLEND_TEXTUREMAPBLEND);
                dwBlend |= DRF_DEF(054, _BLEND, _TEXTUREMAPBLEND, _MODULATE);
            }

            /*
             * Set the colorkey alphatest mode.
             */
            dwAlphaRef = pDriverData->regColorkeyRef;
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
            |  DRF_NUM(054, _BLEND, _SHADEMODE,          pdwRenderState[D3DRENDERSTATE_SHADEMODE])
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

    if ((pCurrentContext->lpLclZ)
     && (pdwRenderState[D3DRENDERSTATE_ZENABLE]))
    {
        if (pdwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE)
        {
            /*
             * Plain Z-Buffer.
             */
            dwControl |= DRF_DEF(054, _CONTROL, _ZENABLE,              _TRUE)
                      |  DRF_NUM(054, _CONTROL, _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                      |  DRF_NUM(054, _CONTROL, _Z_PERSPECTIVE_ENABLE, pDriverData->regZPerspectiveEnable)
                      |  DRF_NUM(054, _CONTROL, _Z_FORMAT,             pDriverData->regZFormat);
        }
        else
        {
            /*
             * W-Buffer.
             */
            // Fixed point w-buffer reqires scaling of RHW but gives more precision in 16bit w-buffers.
            dwControl |= DRF_DEF(054, _CONTROL,  _ZENABLE,              _TRUE)
                      |  DRF_NUM(054, _CONTROL,  _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                      |  DRF_DEF(054, _CONTROL,  _Z_PERSPECTIVE_ENABLE, _TRUE)
                      |  DRF_DEF(054, _CONTROL,  _Z_FORMAT,             _FIXED);
        }
    }
    else
    {
        dwControl |= DRF_DEF(054, _CONTROL, _ZENABLE,      _FALSE)
                  |  DRF_DEF(054, _CONTROL, _ZWRITEENABLE, _FALSE)
                  |  DRF_NUM(054, _CONTROL, _Z_PERSPECTIVE_ENABLE, pDriverData->regZPerspectiveEnable)
                  |  DRF_NUM(054, _CONTROL, _Z_FORMAT,             pDriverData->regZFormat);

    }

    /*
     * Set the fog color.
     */
    dwFogColor = pdwRenderState[D3DRENDERSTATE_FOGCOLOR];

    /*
     * Store fog table values as floats in the context structure.
     */
    pCurrentContext->dwFogTableMode   = pdwRenderState[D3DRENDERSTATE_FOGTABLEMODE];
    pCurrentContext->fFogTableStart   = *(float *)&pdwRenderState[D3DRENDERSTATE_FOGTABLESTART];
    pCurrentContext->fFogTableEnd     = *(float *)&pdwRenderState[D3DRENDERSTATE_FOGTABLEEND];
    pCurrentContext->fFogTableDensity = *(float *)&pdwRenderState[D3DRENDERSTATE_FOGTABLEDENSITY];
    if (pCurrentContext->dwFogTableMode == D3DFOG_LINEAR)
    {
        if (pCurrentContext->fFogTableEnd != pCurrentContext->fFogTableStart)
            pCurrentContext->fFogTableLinearScale = 1.0f / (pCurrentContext->fFogTableEnd - pCurrentContext->fFogTableStart);
        else
            pCurrentContext->fFogTableLinearScale = 0.0f;
    }

    /*
     * Store the register values.
     */
    pCurrentContext->ctxInnerLoop.dwColorKey      = dwColorKey;
    pCurrentContext->ctxInnerLoop.dwTextureOffset = dwOffset;
    pCurrentContext->ctxInnerLoop.dwFormat        = dwFormat;
    pCurrentContext->ctxInnerLoop.dwFilter        = dwFilter;
    pCurrentContext->ctxInnerLoop.dwBlend         = dwBlend;
    pCurrentContext->ctxInnerLoop.dwControl       = dwControl;
    pCurrentContext->ctxInnerLoop.dwFogColor      = dwFogColor;
    pCurrentContext->dwStateChange                = FALSE;

#ifdef  SPEC_HACK
    /*
     * Update the current blend mode but set the specular enable bit the way it was left.
     * If specular is disabled in the render state, then it should be disabled in the current state.
     * Otherwise just use the last know state of specular highlighting.  It will get enabled when it is
     * needed if it was previously disabled.
     */
    if (!pdwRenderState[D3DRENDERSTATE_SPECULARENABLE])
        pCurrentContext->ctxInnerLoop.dwCurrentSpecularState = FALSE;

    pCurrentContext->ctxInnerLoop.dwCurrentBlend = (dwBlend & 0xFFFF0FFF)
                                                 |  DRF_NUM(054, _BLEND, _SPECULARENABLE, pCurrentContext->ctxInnerLoop.dwCurrentSpecularState);
    pCurrentContext->ctxInnerLoop.dwNoSpecularTriangleCount = 0;
#endif  // SPEC_HACK

    return;
}
BOOL nvSetHardwareState
(
    void
)
{
    DWORD   dwTBlend;
    DWORD   dwMinFilter, dwMagFilter;

#ifdef NV_STATS
    {
        extern DWORD dwHWPCount;
        dwHWPCount ++;
    }
#endif

    /*
     * This code is only enabled in the DEBUG build.
     * Display the DX5 render state to be calculated.
     */
    dbgDisplayContextState(NVDBG_SHOW_RENDER_STATE, pCurrentContext);

    /*
     * Get state to be passed to the hardware setup routine.
     */
    dwTBlend = pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND];
    dwMinFilter = pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREMIN];
    dwMagFilter = pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAG];

    /*
     * Build up the DX5 control state.
     */
    nvCalculateHardwareStateDX5(dwTBlend, dwMinFilter, dwMagFilter, FALSE);

    /*
     * This code is only enabled in the DEBUG build.
     * Validate the inner loop hardware control registers.
     */
    dbgValidateControlRegisters(pCurrentContext);

    /*
     * This code is only enabled in the DEBUG build.
     * Display the hardware registers that were calculated.
     */
    dbgDisplayContextState(NVDBG_SHOW_DX5_CLASS_STATE, pCurrentContext);
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

//-------------------------------------------------------------------------------------

BOOL nvConvertTextureStageToDX5Class
(
)
{
    BOOL                    bAlphaTexture;
    DWORD                   tssAlphaBlend, tssColorBlend;
    DWORD                   dwMinFilter, dwMagFilter;
    DWORD                   dwTBlend;
    DWORD                   dwTexture;
    PNVD3DTEXTURE           pTexture;
    PNVD3DTEXSTAGESTATE     ptssState0;
    PNVD3DMULTITEXTURESTATE pmtsState;


    ptssState0 = &pCurrentContext->tssState[0];
    pmtsState  = &pCurrentContext->mtsState;

    tssAlphaBlend = ((ptssState0->dwAlphaArg1 & 0x000000FF) << 0)
                  | ((ptssState0->dwAlphaArg2 & 0x000000FF) << 8)
                  | ((ptssState0->dwAlphaOp & 0x000000FF) << 16);
    tssColorBlend = ((ptssState0->dwColorArg1 & 0x000000FF) << 0)
                  | ((ptssState0->dwColorArg2 & 0x000000FF) << 8)
                  | ((ptssState0->dwColorOp & 0x000000FF) << 16);

    /*
     * Save off the current texture handle.  This may be changed under some circumstances.
     * MUST BE RESTORED BEFORE EXITING ROUTINE.
     */
    dwTexture = pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE];

    /*
     * Determine if the single texture stage can be converted to a DX5 TEXTUREMAPBLEND state.
     */
    if (ptssState0->dwColorOp != D3DTOP_DISABLE)
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
            bAlphaTexture = FALSE;
            if (pTexture = (PNVD3DTEXTURE)dwTexture)
            {
                bAlphaTexture = ((pTexture->dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A1R5G5B5)
                              || (pTexture->dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A4R4G4B4)
                              || (pTexture->dwTextureColorFormat == NV054_FORMAT_COLOR_LE_A8R8G8B8));
            }
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
                 * TEXTUREMAPBLEND equqivalent is MODULATE and the texture has an alpha component,
                 * so program the DX5 class with MODULATE.
                 * However, if there is no texture selected, the DX5 class should be programmed as
                 * MODULATEALPHA to use the vertex alpha values.
                 */
                if (!pTexture)
                    dwTBlend = D3DTBLEND_MODULATEALPHA;
                else
                    dwTBlend = D3DTBLEND_MODULATE;
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
            else
                return (FALSE);
        }
        else
            return (FALSE);
    }
    else
    {
        /*
         * When stage 0 is disabled, force DX5 setup with no texture.
         */
        pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE] = 0;
        nvCalculateHardwareStateDX5(D3DTBLEND_MODULATEALPHA, D3DFILTER_NEAREST, D3DFILTER_NEAREST, TRUE);
        pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE] = dwTexture;
        return (TRUE);
    }

    /*
     * Calculate the MIN and MAG filters from the texture stage state.
     */
    /*
     * Need to calculate the MIN and MAG filters.
     */
    switch (ptssState0->dwMagFilter)
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
    if (ptssState0->dwMipFilter == D3DTFP_NONE)
    {
        switch (ptssState0->dwMinFilter)
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
    else if (ptssState0->dwMipFilter == D3DTFP_POINT)
    {
        switch (ptssState0->dwMinFilter)
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
        switch (ptssState0->dwMinFilter)
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
    if (pTexture = (PNVD3DTEXTURE)dwTexture)
        NV_LOCK_TEXTURE_SURFACE(pTexture);

    if ((pTexture) && (pTexture->lpLcl)
     && (pTexture->lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
     && (ptssState0->dwMaxMipLevel)
     && (ptssState0->dwMaxMipLevel < pTexture->dwMipMapLevels))
    {
        DWORD   i;

        for (i = 0; i < ptssState0->dwMaxMipLevel; i++)
            pTexture = (PNVD3DTEXTURE)pTexture->lpLcl->lpAttachList->lpAttached->dwReserved1;

        /*
         * Update the render state handle for the starting mipmap level.
         */
        pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE] = (DWORD)pTexture;
        NV_UNLOCK_TEXTURE_SURFACE(pTexture);
    }

    /*
     * All other state comes directly from the renderstate.
     * Build up the DX5 control state.
     */
    nvCalculateHardwareStateDX5(dwTBlend, dwMinFilter, dwMagFilter, TRUE);

    /*
     * Restore the original texture handle.
     */
    pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE] = dwTexture;

    /*
     * Validate the inner loop hardware control registers.
     */
    dbgValidateControlRegisters(pCurrentContext);

    /*
     * This code is only enabled in the DEBUG build.
     * Display the hardware registers that were calculated.
     */
    dbgDisplayContextState(NVDBG_SHOW_DX5_CLASS_STATE, pCurrentContext);

    /*
     * Determine the UV coordinate offset for the first stage.
     */
    pmtsState->dwUVOffset[0] = (2 * sizeof(D3DVALUE)) * ptssState0->dwTexCoordIndex;

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

    /*
     * AA semantics check - I hate these
     */
    if (pContext->dwAntiAliasFlags & AA_ENABLED_MASK)
        pContext->dwAntiAliasFlags |= AA_MIX_DP_NOAA;
    else
        pContext->dwAntiAliasFlags |= AA_MIX_DP_AA;

    /*
     * we do not send state when we capture geometry for super sampled AA
     */
    if (pCurrentContext->dwAntiAliasFlags & AA_MODE_SUPER)
    {
        if (!(pCurrentContext->dwAntiAliasFlags & AA_CAPTURE_PUSHBUFFER))
        {
            return;
        }
    }

    while (nvFreeCount < (sizeSetObject + sizeDX6TriangleState))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeDX6TriangleState));

    /*
     * setup locals
     */
    pState       = &pCurrentContext->mtsState;
    pShadowState = &pCurrentContext->mtsShadowState;

    /*
     * Only write channel if it is not selected - avoids texture cache invalidates
     */
    if (pDriverData->dDrawSpareSubchannelObject != D3D_DX6_MULTI_TEXTURE_TRIANGLE)
    {
        /*
         * select object
         */
        nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX6_MULTI_TEXTURE_TRIANGLE);

        /*
         * program everything
         */
        nvglDx6TriangleOffset(nvFifo, nvFreeCount, NV_DD_SPARE,
                              pState->dwTextureOffset[0], 0);
        nvglDx6TriangleOffset(nvFifo, nvFreeCount, NV_DD_SPARE,
                              pState->dwTextureOffset[1], 1);
        nvglDx6TriangleFormat(nvFifo, nvFreeCount, NV_DD_SPARE,
                              pState->dwTextureFormat[0], 0);
        nvglDx6TriangleFormat(nvFifo, nvFreeCount, NV_DD_SPARE,
                              pState->dwTextureFormat[1], 1);
        nvglDx6TriangleFilter(nvFifo, nvFreeCount, NV_DD_SPARE,
                              pState->dwTextureFilter[0], 0);
        nvglDx6TriangleFilter(nvFifo, nvFreeCount, NV_DD_SPARE,
                              pState->dwTextureFilter[1], 1);
        nvglDx6TriangleStageState(nvFifo, nvFreeCount, NV_DD_SPARE,
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
            nvglDx6TriangleOffset(nvFifo, nvFreeCount, NV_DD_SPARE,
                                  pState->dwTextureOffset[0], 0);

            pShadowState->dwTextureOffset[0] = pState->dwTextureOffset[0];
        }

        if (pShadowState->dwTextureOffset[1] != pState->dwTextureOffset[1])
        {
            nvglDx6TriangleOffset(nvFifo, nvFreeCount, NV_DD_SPARE,
                                  pState->dwTextureOffset[1], 1);

            pShadowState->dwTextureOffset[1] = pState->dwTextureOffset[1];
        }

        if (pShadowState->dwTextureFormat[0] != pState->dwTextureFormat[0])
        {
            nvglDx6TriangleFormat(nvFifo, nvFreeCount, NV_DD_SPARE,
                                  pState->dwTextureFormat[0], 0);

            pShadowState->dwTextureFormat[0] = pState->dwTextureFormat[0];
        }

        if (pShadowState->dwTextureFormat[1] != pState->dwTextureFormat[1])
        {
            nvglDx6TriangleFormat(nvFifo, nvFreeCount, NV_DD_SPARE,
                                  pState->dwTextureFormat[1], 1);

            pShadowState->dwTextureFormat[1] = pState->dwTextureFormat[1];
        }

        if (pShadowState->dwTextureFilter[0] != pState->dwTextureFilter[0])
        {
            nvglDx6TriangleFilter(nvFifo, nvFreeCount, NV_DD_SPARE,
                                  pState->dwTextureFilter[0], 0);

            pShadowState->dwTextureFilter[0] = pState->dwTextureFilter[0];
        }

        if (pShadowState->dwTextureFilter[1] != pState->dwTextureFilter[1])
        {
            nvglDx6TriangleFilter(nvFifo, nvFreeCount, NV_DD_SPARE,
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
            nvglDx6TriangleStageState(nvFifo, nvFreeCount, NV_DD_SPARE,
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

    return;
}

//-------------------------------------------------------------------------------------

/*
 * Validate the multi-texture rendering state.
 */
DWORD FAR PASCAL nvValidateTextureStageState
(
    LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA  pvtssd
)
{
    DWORD   i;

    pCurrentContext = (PNVD3DCONTEXT)pvtssd->dwhContext;
    if (!pCurrentContext)
    {
        return (DDHAL_DRIVER_HANDLED);
    }
    nvSetMultiTextureHardwareState();
    pvtssd->ddrval = pCurrentContext->mtsState.ddrval;

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
        for (i = 0; i < pCurrentContext->dwStageCount; i++)
        {
            if ((pCurrentContext->tssState[i].dwColorOp != D3DTOP_DISABLE)
             && ((pCurrentContext->tssState[i].dwAddressU == D3DTADDRESS_BORDER)
              || (pCurrentContext->tssState[i].dwAddressV == D3DTADDRESS_BORDER)))
            {
                pvtssd->ddrval      = D3DERR_CONFLICTINGRENDERSTATE;
                pvtssd->dwNumPasses = 0xFFFFFFFF;
                return (DDHAL_DRIVER_HANDLED);
            }
#ifdef DX7
            if ((pCurrentContext->tssState[i].dwColorArg1 == D3DTA_SPECULAR) ||
                (pCurrentContext->tssState[i].dwColorArg2 == D3DTA_SPECULAR))
            {
                pvtssd->ddrval      = D3DERR_UNSUPPORTEDCOLORARG;
                pvtssd->dwNumPasses = 0xFFFFFFFF;
                return (DDHAL_DRIVER_HANDLED);
            }

            if ((pCurrentContext->tssState[i].dwAlphaArg1 == D3DTA_SPECULAR) ||
                (pCurrentContext->tssState[i].dwAlphaArg2 == D3DTA_SPECULAR))
            {
                pvtssd->ddrval      = D3DERR_UNSUPPORTEDALPHAARG;
                pvtssd->dwNumPasses = 0xFFFFFFFF;
                return (DDHAL_DRIVER_HANDLED);
            }
#endif
        }
        /*
         * If the DX6 class is being used and colorkey is enabled, return an error.
         */
        if ((pCurrentContext->bUseDX6Class)
         && (pCurrentContext->dwRenderState[D3DRENDERSTATE_COLORKEYENABLE]))
        {
            pvtssd->ddrval      = D3DERR_CONFLICTINGRENDERSTATE;
            pvtssd->dwNumPasses = 0xFFFFFFFF;
            return (DDHAL_DRIVER_HANDLED);
        }
        pvtssd->dwNumPasses = 1;
    }
    else
        pvtssd->dwNumPasses = 0xFFFFFFFF;
    return (DDHAL_DRIVER_HANDLED);
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

BOOL nvCheckStencilBuffer
(
    void
)
{
    /*
     * If the rendering surface is 16bpp, then there can't be a stencil buffer.
     */
    if ((pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) |
                                              DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1R5G5B5_Z1R5G5B5)))
     || (pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) |
                                              DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_R5G6B5))))
        return (FALSE);

    /*
     * If there is no Z-Buffer, then there can't be a stencil buffer.
     */
    if (!pCurrentContext->lpLclZ)
        return (FALSE);

    /*
     * Make sure Surface pixel format is specified.  If it's not, there can't
     * be a stencil buffer.
     */
    if (!(pCurrentContext->lpLclZ->dwFlags & DDRAWISURF_HASPIXELFORMAT))
        return (FALSE);

    /*
     * If the z-buffer pixel format does not specify a stencil buffer,
     * then there is no stencil buffer.
     */
    if (!(pCurrentContext->lpLclZ->lpGbl->ddpfSurface.dwFlags & DDPF_STENCILBUFFER))
        return (FALSE);

    /*
     * There is a stencil buffer and the rendering surface is 32bpp,
     * return whether the stencil buffer is enabled or not.
     */
    return (pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]);
}

//-------------------------------------------------------------------------------------

void nvSetDX6TextureState
(
    DWORD                   dwTextureNum,
    PNVD3DTEXSTAGESTATE     ptssState,
    PNVD3DMULTITEXTURESTATE pmtsState
)
{
    DWORD           dwMagFilter;
    DWORD           dwMinFilter;
    DWORD           dwZOHMode, dwFOHMode;
    PNVD3DTEXTURE   pTexture;

    /*
     * setup state
     */
    dwZOHMode     = ((pDriverData->regTexelAlignment & REG_TA_ZOH_MASK) == REG_TA_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
    dwFOHMode     = ((pDriverData->regTexelAlignment & REG_TA_FOH_MASK) == REG_TA_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;

    if (pTexture = (PNVD3DTEXTURE)ptssState->dwTextureMap)
        NV_LOCK_TEXTURE_SURFACE(pTexture);
    if ((pTexture)
     && (pTexture->lpLcl))
    {
        if ((ptssState->dwMaxMipLevel)
         && (pTexture->lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP))
        {
            if (ptssState->dwMaxMipLevel < pTexture->dwMipMapLevels)
            {
                DWORD   i;
                /*
                 * Need to track down the mipmap level that the application wants to start from.
                 */
                for (i = 0; i < ptssState->dwMaxMipLevel; i++)
                    pTexture = (PNVD3DTEXTURE)pTexture->lpLcl->lpAttachList->lpAttached->dwReserved1;
            }
        }
#ifdef NV_TEX2
        pmtsState->dwTextureOffset[dwTextureNum] = TEX_SWIZZLE_OFFSET(pTexture,pTexture->dwTextureFlags);
#else
        pmtsState->dwTextureOffset[dwTextureNum] = pTexture->dwTextureOffset;
#endif
        pmtsState->dwTextureFormat[dwTextureNum] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     pTexture->dwTextureContextDma)
                                                 | DRF_NUM(055, _FORMAT, _COLOR,           pTexture->dwTextureColorFormat)
                                                 | DRF_NUM(055, _FORMAT, _BASE_SIZE_U,     pTexture->dwMipMapBaseU)
                                                 | DRF_NUM(055, _FORMAT, _BASE_SIZE_V,     pTexture->dwMipMapBaseV)
                                                 | DRF_NUM(055, _FORMAT, _TEXTUREADDRESSU, ptssState->dwAddressU)
                                                 | DRF_NUM(055, _FORMAT, _WRAPU,           (DWORD)ptssState->bWrapU)
                                                 | DRF_NUM(055, _FORMAT, _TEXTUREADDRESSV, ptssState->dwAddressV)
                                                 | DRF_NUM(055, _FORMAT, _WRAPV,           (DWORD)ptssState->bWrapU);
        pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _ORIGIN_ZOH,     dwZOHMode)
                                                 |  DRF_NUM(055, _FORMAT, _ORIGIN_FOH,     dwFOHMode);

        /*
         * Need to calculate the MIN and MAG filters.
         */
        switch (ptssState->dwMagFilter)
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
        if (ptssState->dwMipFilter == D3DTFP_NONE)
        {
            switch (ptssState->dwMinFilter)
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
        }
        else if (ptssState->dwMipFilter == D3DTFP_POINT)
        {
            switch (ptssState->dwMinFilter)
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
        }
        else
        {
            switch (ptssState->dwMinFilter)
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
        }

        if ((pTexture->lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
         || (pTexture->dwMipMapLevels == 1))
        {
            /*
             * When this is a user supplied mipmap chain
             * OR the number of mipmap levels is 1 (i.e. no mipmaps, only base texture)
             * set the MIN/MAG filters as specified by the render states.
             */
            pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE);
            pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _TEXTUREMIN, dwMinFilter)
                                                     |  DRF_NUM(055, _FILTER, _TEXTUREMAG, dwMagFilter);

            /*
             * The only way to guarantee that mipmapping is disabled is to set MIPMAP_LEVELS to 1.
             * In cases where there are the user has supplied mipmaps but the minification filter is
             * set to D3DFILTER_NEAREST or D3DFILTER_LINEAR, force the mipmap levels field to be a 1.
             */
            if ((dwMinFilter == D3DFILTER_NEAREST)
             || (dwMinFilter == D3DFILTER_LINEAR))
                pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS, 1);
            else
                pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS, pTexture->dwMipMapLevels);
        }
        else
        {
#ifdef NV_TEX2
            if (pTexture->dwTextureFlags & NV4_TEXFLAG_AUTOMIPMAP)
#else
            if (pTexture->dwTextureFlags & NV4_TEXTURE_CAN_AUTO_MIPMAP)
#endif
            {
                /*
                 * This is the auto-generated mipmap case. In order to get the some sort of mipmapping
                 * to happen, the MIN/MAG filters must be modified if they are NEAREST or LINEAR.
                 */
                pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE);
                pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS, pTexture->dwMipMapLevels);

                /*
                 * Set the MINIFICATION filter.
                 */
                if (pDriverData->regAutoMipMapMethod == REG_AMMM_BILINEAR)
                {
                    if (dwMinFilter == D3DFILTER_NEAREST)
                        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _TEXTUREMIN, _MIPNEAREST);
                    else if (dwMinFilter == D3DFILTER_LINEAR)
                        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _TEXTUREMIN, _MIPLINEAR);
                    else
                        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _TEXTUREMIN, dwMinFilter);
                }
                else
                {
                    if (dwMinFilter == D3DFILTER_NEAREST)
                        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _TEXTUREMIN, _LINEARMIPNEAREST);
                    else if (dwMinFilter == D3DFILTER_LINEAR)
                        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _TEXTUREMIN, _LINEARMIPLINEAR);
                    else
                        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _TEXTUREMIN, dwMinFilter);
                }

                /*
                 * Set the MAGNIFICATION filter.
                 * Can only validly be set to NEAREST or LINEAR
                 */
                pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _TEXTUREMAG, dwMagFilter);
            }
            else
            {
                /*
                 * This texture is flagged as not a auto-generated mipmapable texture.
                 */
                pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE)
                                                         |  DRF_NUM(055, _FILTER, _TEXTUREMIN,    dwMinFilter)
                                                         |  DRF_NUM(055, _FILTER, _TEXTUREMAG,    dwMagFilter);
                pmtsState->dwTextureFormat[dwTextureNum] |= DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS, 1);
            }
        }
        dbgDisplayTexture(pCurrentContext, pTexture);
        NV_LOCK_TEXTURE_SURFACE(pTexture);
    }
    else
    {
        /*
         * There is no texture set the default texture format and offset.
         */
        pmtsState->dwTextureOffset[dwTextureNum] = pCurrentContext->dwDefaultTextureOffset;
        pmtsState->dwTextureFormat[dwTextureNum] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     pCurrentContext->dwDefaultTextureContextDma)
                                                 | DRF_NUM(055, _FORMAT, _COLOR,           pCurrentContext->dwDefaultTextureColorFormat)
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
        pmtsState->dwTextureFilter[dwTextureNum] |= DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _FALSE)
                                                 |  DRF_DEF(055, _FILTER, _TEXTUREMIN,           _NEAREST)
                                                 |  DRF_DEF(055, _FILTER, _TEXTUREMAG,           _NEAREST);
    }

    /*
     * Setup stage texture filter
     */
    pmtsState->dwTextureFilter[dwTextureNum] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, (DWORD)ptssState->bLODBias);

    /*
     * Determine the UV coordinate offset for this texture stage.
     */
    pmtsState->dwUVOffset[dwTextureNum] = (2 * sizeof(D3DVALUE)) * ptssState->dwTexCoordIndex;
    return;
}

//-------------------------------------------------------------------------------------

void nvSetDX6State
(
    DWORD                   *pdwRenderState,
    PNVD3DMULTITEXTURESTATE  pmtsState
)
{
    DWORD   dwAlphaRef;
    DWORD   dwTexelOrigin;

    dwTexelOrigin = ((pDriverData->regTexelAlignment & REG_TA_TEXEL_MASK) == REG_TA_TEXEL_CENTER)
                  ? NV054_CONTROL_ORIGIN_CENTER : NV054_CONTROL_ORIGIN_CORNER;

    /*
     * Set the Combine Factor.
     */
    pmtsState->dwCombineFactor = pdwRenderState[D3DRENDERSTATE_TEXTUREFACTOR];

    /*
     * Setup Blend control.
     */
    pmtsState->dwBlend = DRF_DEF(055, _BLEND, _MASK_BIT,           _MSB)
                       | DRF_NUM(055, _BLEND, _SHADEMODE,          pdwRenderState[D3DRENDERSTATE_SHADEMODE])
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
#ifdef NV_CONTROLTRAFFIC
                          | DRF_NUM(055, _CONTROL0, _ZFUNC,                CTFunc(pdwRenderState[D3DRENDERSTATE_ZFUNC]))
#else
                          | DRF_NUM(055, _CONTROL0, _ZFUNC,                pdwRenderState[D3DRENDERSTATE_ZFUNC])
#endif
#ifdef NV_FASTLOOPS
                          | DRF_NUM(055, _CONTROL0, _CULLMODE,             D3DCULL_NONE)
#else
                          | DRF_NUM(055, _CONTROL0, _CULLMODE,             pdwRenderState[D3DRENDERSTATE_CULLMODE])
#endif
                          | DRF_NUM(055, _CONTROL0, _DITHERENABLE,         pdwRenderState[D3DRENDERSTATE_DITHERENABLE]);

    /*
     * Set Control0 z-buffer enable bits.
     */
    if ((pCurrentContext->lpLclZ)
     && (pdwRenderState[D3DRENDERSTATE_ZENABLE]))
    {
        if (pdwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE)
        {
            /*
             * Plain Z-Buffer.
             */
            pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _TRUE)
                                  |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                  |  DRF_NUM(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, pDriverData->regZPerspectiveEnable)
                                  |  DRF_NUM(055, _CONTROL0, _Z_FORMAT,             pDriverData->regZFormat);
        }
        else
        {
            /*
             * W-Buffer.
             */
            pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _TRUE)
                                  |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pdwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                  |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _TRUE)
                                  |  DRF_DEF(055, _CONTROL0, _Z_FORMAT,             _FIXED);
        }
    }
    else
    {
        pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _FALSE)
                              |  DRF_DEF(055, _CONTROL0, _ZWRITEENABLE,         _FALSE)
                              |  DRF_NUM(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, pDriverData->regZPerspectiveEnable)
                              |  DRF_NUM(055, _CONTROL0, _Z_FORMAT,             pDriverData->regZFormat);
    }
    /*
     * Set Control0 stencil buffer enable bits.
     */
    if (nvCheckStencilBuffer())
    {
        pmtsState->dwControl0 |= DRF_NUM(055, _CONTROL0, _STENCIL_WRITE_ENABLE, pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]);
        /*
         * Setup Control1.
         * Stencil buffer control bits.
         */
        pmtsState->dwControl1 = DRF_NUM(055, _CONTROL1, _STENCIL_TEST_ENABLE, pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_FUNC,        pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_REF,         pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILREF])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_READ,   pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILMASK])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_WRITE,  pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK]);

        /*
         * Setup Control2.
         * More stencil buffer control bits.
         */
        pmtsState->dwControl2 = DRF_NUM(055, _CONTROL2, _STENCIL_OP_FAIL,  pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL])
                              | DRF_NUM(055, _CONTROL2, _STENCIL_OP_ZFAIL, pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL])
                              | DRF_NUM(055, _CONTROL2, _STENCIL_OP_ZPASS, pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]);
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

    /*
     * Store fog table values as floats in the context structure.
     */
    pCurrentContext->dwFogTableMode   = pdwRenderState[D3DRENDERSTATE_FOGTABLEMODE];
    pCurrentContext->fFogTableStart   = *(float *)&pdwRenderState[D3DRENDERSTATE_FOGTABLESTART];
    pCurrentContext->fFogTableEnd     = *(float *)&pdwRenderState[D3DRENDERSTATE_FOGTABLEEND];
    pCurrentContext->fFogTableDensity = *(float *)&pdwRenderState[D3DRENDERSTATE_FOGTABLEDENSITY];
    if (pCurrentContext->dwFogTableMode == D3DFOG_LINEAR)
    {
        if (pCurrentContext->fFogTableEnd != pCurrentContext->fFogTableStart)
            pCurrentContext->fFogTableLinearScale = 1.0f / (pCurrentContext->fFogTableEnd - pCurrentContext->fFogTableStart);
        else
            pCurrentContext->fFogTableLinearScale = 0.0f;
    }
    return;
}

//-------------------------------------------------------------------------------------

void nvSetDefaultMultiTextureHardwareState
(
)
{
    DWORD                   dwZOHMode, dwFOHMode, dwTexelOrigin;
    DWORD                   dwTextureOffset;
    DWORD                   dwTextureContextDma;
    DWORD                   dwTextureColorFormat;
    DWORD                   dwMipMapLevels;
    DWORD                   dwMipMapBaseU;
    DWORD                   dwMipMapBaseV;
    PNVD3DTEXTURE           pTexture0;
    PNVD3DMULTITEXTURESTATE pmtsState;

    // txtodo - check texture valid for HW program

    dwZOHMode     = ((pDriverData->regTexelAlignment & REG_TA_ZOH_MASK) == REG_TA_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
    dwFOHMode     = ((pDriverData->regTexelAlignment & REG_TA_FOH_MASK) == REG_TA_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;
    dwTexelOrigin = ((pDriverData->regTexelAlignment & REG_TA_TEXEL_MASK) == REG_TA_TEXEL_CENTER)
                  ? NV054_CONTROL_ORIGIN_CENTER : NV054_CONTROL_ORIGIN_CORNER;

    /*
     * Grab texture from the render state array.  This is guaranteed to always have the
     * correct texture no matter if it was set using the render state method or the texture
     * stage 0 method.
     */
    pTexture0 = (PNVD3DTEXTURE)pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE];
    if (pTexture0)
    {
#ifdef NV_TEX2
        dwTextureOffset      = TEX_SWIZZLE_OFFSET(pTexture0,pTexture0->dwTextureFlags);
#else
        dwTextureOffset      = pTexture0->dwTextureOffset;
#endif
        dwTextureContextDma  = pTexture0->dwTextureContextDma;
        dwTextureColorFormat = pTexture0->dwTextureColorFormat;
        dwMipMapBaseU        = pTexture0->dwMipMapBaseU;
        dwMipMapBaseV        = pTexture0->dwMipMapBaseV;
        dwMipMapLevels       = pTexture0->dwMipMapLevels;
    }
    else
    {
        /*
         * No texture, setup default.
         */
        dwTextureOffset      = pCurrentContext->dwDefaultTextureOffset;
        dwTextureContextDma  = pCurrentContext->dwDefaultTextureContextDma;
        dwTextureColorFormat = pCurrentContext->dwDefaultTextureColorFormat;
        dwMipMapLevels       = 1;
        dwMipMapBaseU        = NV055_FORMAT_BASE_SIZE_U_1;
        dwMipMapBaseV        = NV055_FORMAT_BASE_SIZE_V_1;
    }

    pmtsState  = &pCurrentContext->mtsState;
    pmtsState->dwTextureOffset[0] =  dwTextureOffset;
    pmtsState->dwTextureFormat[0] =  DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     dwTextureContextDma)
                                  |  DRF_NUM(055, _FORMAT, _COLOR,           dwTextureColorFormat)
                                  |  DRF_NUM(055, _FORMAT, _MIPMAP_LEVELS,   dwMipMapLevels)
                                  |  DRF_NUM(055, _FORMAT, _BASE_SIZE_U,     dwMipMapBaseU)
                                  |  DRF_NUM(055, _FORMAT, _BASE_SIZE_V,     dwMipMapBaseV)
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

    pmtsState->dwTextureFilter[0] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, pDriverData->regLODBiasAdjust);

    pmtsState->dwTextureOffset[1] =  pCurrentContext->dwDefaultTextureOffset;
    pmtsState->dwTextureFormat[1] =  DRF_NUM(055, _FORMAT, _CONTEXT_DMA,      pCurrentContext->dwDefaultTextureContextDma)
                                  |  DRF_NUM(055, _FORMAT, _COLOR,            pCurrentContext->dwDefaultTextureColorFormat)
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
                                  |  DRF_DEF(055, _FILTER, _MIPMAP_DITHER_ENABLE, _TRUE)
                                  |  DRF_DEF(055, _FILTER, _TEXTUREMIN,    _NEAREST)
                                  |  DRF_DEF(055, _FILTER, _ANISOTROPIC_MINIFY_ENABLE, _FALSE)
                                  |  DRF_DEF(055, _FILTER, _TEXTUREMAG,    _NEAREST)
                                  |  DRF_DEF(055, _FILTER, _ANISOTROPIC_MAGNIFY_ENABLE, _FALSE);

    pmtsState->dwTextureFilter[1] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, pDriverData->regLODBiasAdjust);

    pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_DEFAULT;
    pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_DEFAULT;
    pmtsState->dwCombine1Alpha = DX6TRI_COMBINE0ALPHA_DEFAULT;;
    pmtsState->dwCombine1Color = DX6TRI_COMBINE0COLOR_DEFAULT;;

    pmtsState->dwCombineFactor = 0;
    pmtsState->dwBlend         = DRF_DEF(055, _BLEND, _MASK_BIT,           _MSB)
                               | DRF_NUM(055, _BLEND, _SHADEMODE,          pCurrentContext->dwRenderState[D3DRENDERSTATE_SHADEMODE])
                               | DRF_NUM(055, _BLEND, _TEXTUREPERSPECTIVE, pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE])
                               | DRF_NUM(055, _BLEND, _SPECULARENABLE,     pCurrentContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE])
                               | DRF_NUM(055, _BLEND, _FOGENABLE,          pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGENABLE])
                               | DRF_NUM(055, _BLEND, _ALPHABLENDENABLE,   pCurrentContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]);

    if (pCurrentContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_BOTHSRCALPHA)
        pmtsState->dwBlend |= DRF_DEF(055, _BLEND, _SRCBLEND,  _SRCALPHA)
                           |  DRF_DEF(055, _BLEND, _DESTBLEND, _INVSRCALPHA);
    else if (pCurrentContext->dwRenderState[D3DRENDERSTATE_SRCBLEND] == D3DBLEND_BOTHINVSRCALPHA)
        pmtsState->dwBlend |= DRF_DEF(055, _BLEND, _SRCBLEND,  _INVSRCALPHA)
                           |  DRF_DEF(055, _BLEND, _DESTBLEND, _SRCALPHA);
    else
        pmtsState->dwBlend |= DRF_NUM(055, _BLEND, _SRCBLEND,  pCurrentContext->dwRenderState[D3DRENDERSTATE_SRCBLEND])
                           |  DRF_NUM(055, _BLEND, _DESTBLEND, pCurrentContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]);

    pmtsState->dwControl0      = DRF_NUM(055, _CONTROL0, _ALPHAREF,             0)
                               | DRF_DEF(055, _CONTROL0, _ALPHAFUNC,            _ALWAYS)
                               | DRF_DEF(055, _CONTROL0, _ALPHATESTENABLE,      _FALSE)
                               | DRF_NUM(055, _CONTROL0, _ORIGIN,               dwTexelOrigin)
#ifdef NV_CONTROLTRAFFIC
                               | DRF_NUM(055, _CONTROL0, _ZFUNC,                CTFunc(pCurrentContext->dwRenderState[D3DRENDERSTATE_ZFUNC]))
#else
                               | DRF_NUM(055, _CONTROL0, _ZFUNC,                pCurrentContext->dwRenderState[D3DRENDERSTATE_ZFUNC])
#endif
#ifdef NV_FASTLOOPS
                               | DRF_NUM(055, _CONTROL0, _CULLMODE,             D3DCULL_NONE)
#else
                               | DRF_NUM(055, _CONTROL0, _CULLMODE,             pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE])
#endif
                               | DRF_NUM(055, _CONTROL0, _DITHERENABLE,         pCurrentContext->dwRenderState[D3DRENDERSTATE_DITHERENABLE]);
    if ((pCurrentContext->lpLclZ)
     && (pCurrentContext->dwRenderState[D3DRENDERSTATE_ZENABLE]))
    {
        if (pCurrentContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE)
        {
            /*
             * Plain Z-Buffer.
             */
            pmtsState->dwControl0 |= DRF_NUM(055, _CONTROL0, _ZENABLE,              pCurrentContext->dwRenderState[D3DRENDERSTATE_ZENABLE])
                                  |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pCurrentContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                  |  DRF_NUM(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, pDriverData->regZPerspectiveEnable)
                                  |  DRF_NUM(055, _CONTROL0, _Z_FORMAT,             pDriverData->regZFormat);
        }
        else
        {
            /*
             * W-Buffer.
             */
            // Fixed point w-buffer reqires scaling of RHW but gives more precision in 16bit w-buffers.
            pmtsState->dwControl0 |= DRF_DEF(055, _CONTROL0, _ZENABLE,              _TRUE)
                                  |  DRF_NUM(055, _CONTROL0, _ZWRITEENABLE,         pCurrentContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE])
                                  |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _TRUE)
                                  |  DRF_DEF(055, _CONTROL0, _Z_FORMAT,             _FIXED);
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
    if (nvCheckStencilBuffer())
    {
        pmtsState->dwControl0 |= DRF_NUM(055, _CONTROL0, _STENCIL_WRITE_ENABLE, pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]);
        /*
         * Setup Control1.
         * Stencil buffer control bits.
         */
        pmtsState->dwControl1 = DRF_NUM(055, _CONTROL1, _STENCIL_TEST_ENABLE, pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_FUNC,        pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_REF,         pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILREF])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_READ,   pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILMASK])
                              | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_WRITE,  pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK]);

        /*
         * Setup Control2.
         * More stencil buffer control bits.
         */
        pmtsState->dwControl2 = DRF_NUM(055, _CONTROL2, _STENCIL_OP_FAIL,  pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL])
                              | DRF_NUM(055, _CONTROL2, _STENCIL_OP_ZFAIL, pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL])
                              | DRF_NUM(055, _CONTROL2, _STENCIL_OP_ZPASS, pCurrentContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]);
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
    pmtsState->dwFogColor = pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGCOLOR];

    /*
     * Store fog table values as floats in the context structure.
     */
    pCurrentContext->dwFogTableMode   = pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE];
    pCurrentContext->fFogTableStart   = *(float *)&pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGTABLESTART];
    pCurrentContext->fFogTableEnd     = *(float *)&pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGTABLEEND];
    pCurrentContext->fFogTableDensity = *(float *)&pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGTABLEDENSITY];
    if (pCurrentContext->dwFogTableMode == D3DFOG_LINEAR)
    {
        if (pCurrentContext->fFogTableEnd != pCurrentContext->fFogTableStart)
            pCurrentContext->fFogTableLinearScale = 1.0f / (pCurrentContext->fFogTableEnd - pCurrentContext->fFogTableStart);
        else
            pCurrentContext->fFogTableLinearScale = 0.0f;
    }
    return;
}

//-------------------------------------------------------------------------------------

void nvSetMultiTextureHardwareState
(
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
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1, ptssState2, ptssState3;
    PNVD3DMULTITEXTURESTATE pmtsState;
    DWORD                  *pdwRenderState = 0;

#ifdef NV_STATS
    {
        extern DWORD dwHWPCount;
        dwHWPCount ++;
    }
#endif

    /*
     * This code is only enabled in the DEBUG build.
     * Display the DX5/DX6 render state to be calculated.
     */
    dbgDisplayContextState((NVDBG_SHOW_RENDER_STATE | NVDBG_SHOW_MTS_STATE), pCurrentContext);

    pdwRenderState = (DWORD *)pCurrentContext->dwRenderState;
    dwStageCount   = 0;
    ptssState0     = &pCurrentContext->tssState[0];
    ptssState1     = &pCurrentContext->tssState[1];
    pmtsState      = &pCurrentContext->mtsState;

    /*
     * Our bump mapping algorithm is done in a single pass with four texture
     * stages being specified.
     * This is the only four stage operation that we are supporting. Look
     * for it now and reject any other 4 stage operations.
     */
    if ((pCurrentContext->tssState[0].dwColorOp != D3DTOP_DISABLE)
     && (pCurrentContext->tssState[1].dwColorOp != D3DTOP_DISABLE)
     && (pCurrentContext->tssState[2].dwColorOp != D3DTOP_DISABLE)
     && (pCurrentContext->tssState[3].dwColorOp != D3DTOP_DISABLE)
     && (pCurrentContext->tssState[4].dwColorOp == D3DTOP_DISABLE))
    {
        ptssState2     = &pCurrentContext->tssState[2];
        ptssState3     = &pCurrentContext->tssState[3];

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
        if ((ptssState0->dwAlphaOp != D3DTOP_SELECTARG1)
         || (ptssState0->dwAlphaArg1 != (D3DTA_TEXTURE | D3DTA_COMPLEMENT))
         || (ptssState0->dwColorOp == D3DTOP_DISABLE)
         || (ptssState0->dwTexCoordIndex != 0)
         || (ptssState1->dwAlphaOp != D3DTOP_ADDSIGNED)
         || (!((ptssState1->dwAlphaArg1 == D3DTA_TEXTURE)
            && (ptssState1->dwAlphaArg2 == D3DTA_CURRENT))
          && !((ptssState1->dwAlphaArg1 == D3DTA_CURRENT)
            && (ptssState1->dwAlphaArg2 == D3DTA_TEXTURE)))
         || (ptssState1->dwColorOp == D3DTOP_DISABLE)
         || (ptssState1->dwTexCoordIndex != 1)
         || (ptssState2->dwAlphaOp != D3DTOP_DISABLE)
         || (ptssState2->dwColorOp != D3DTOP_ADDSIGNED2X)
         || (ptssState2->dwColorArg1 != (D3DTA_CURRENT | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE))
         || (ptssState2->dwColorArg2 != D3DTA_DIFFUSE)
         || (ptssState3->dwAlphaOp != D3DTOP_DISABLE)
         || (ptssState3->dwColorOp != D3DTOP_MODULATE)
         || (ptssState3->dwColorArg1 != D3DTA_TEXTURE)
         || (ptssState3->dwTexCoordIndex != 0))
        {
            nvSetDefaultMultiTextureHardwareState();
            pCurrentContext->mtsState.bTSSValid = FALSE;
            pCurrentContext->mtsState.ddrval    = D3DERR_TOOMANYOPERATIONS;
            pCurrentContext->dwStageCount       = 1;
            pCurrentContext->bUseDX6Class       = FALSE;
            /*
             * Note that the state has changed and needs to be sent to the hardware.
             */
            NV_FORCE_TRI_SETUP(pCurrentContext);
            return;
        }
        /*
         * Set up Bump Mapping State.
         *
         * Texture0 and Texture1 comes from stage 0, 1 or 3 (always same texture).
         * Texture0 is used for stages with TEXCOORDINDEX = 0
         * Texture1 is used for stages with TEXCOORDINDEX = 1
         */
        nvSetDX6TextureState(0, ptssState0, pmtsState);
        nvSetDX6TextureState(1, ptssState1, pmtsState);
        pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_BUMP;
        pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_BUMP;
        pmtsState->dwCombine1Alpha = DX6TRI_COMBINE1ALPHA_BUMP;
        pmtsState->dwCombine1Color = DX6TRI_COMBINE1COLOR_BUMP;
        nvSetDX6State(pdwRenderState, pmtsState);
        /*
         *
         */
        pCurrentContext->mtsState.bTSSValid = TRUE;
        pCurrentContext->mtsState.ddrval    = DD_OK;
        /*
         * Enable use of multi-texture triangle class if there is more than one texture
         * stage or if the stencil buffer is enabled.
         */
        pCurrentContext->dwStageCount  = 4;
        pCurrentContext->bUseDX6Class  = TRUE;
        pCurrentContext->dwStateChange = FALSE;

        /*
         * This code is only enabled in the DEBUG build.
         * Display the hardware registers that were calculated.
         */
        dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);
        return;
    }

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
    if (pCurrentContext->bUseTBlendSettings)
    {
        if (!nvCheckStencilBuffer())
        {
            /*
             * When using TEXTUREMAPBLEND settings, as long as there is no stencil buffer we
             * can use the faster DX5 class.
             */
            nvSetHardwareState();
            pCurrentContext->bUseDX6Class       = FALSE;
            pCurrentContext->mtsState.bTSSValid = FALSE;
            pCurrentContext->dwStateChange      = FALSE;
        }
        else
        {
            DWORD   dwZOHMode, dwFOHMode;
            dwZOHMode = ((pDriverData->regTexelAlignment & REG_TA_ZOH_MASK) == REG_TA_ZOH_CENTER)
                      ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
            dwFOHMode = ((pDriverData->regTexelAlignment & REG_TA_FOH_MASK) == REG_TA_FOH_CENTER)
                      ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;

            /*
             * Setup stage 0 texture offset
             * Setup stage 0 texture format
             */
            nvSetDX6TextureState(0, ptssState0, pmtsState);

            /*
             * Setup combine0 based on the TEXTUREMAPBLEND setting.
             */
            switch (pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND])
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
            if (!pCurrentContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE])
            {
                pmtsState->dwCombine0Alpha = DX6TRI_COMBINE0ALPHA_TBLEND_MODULATEALPHA;
                pmtsState->dwCombine0Color = DX6TRI_COMBINE0COLOR_TBLEND_MODULATEALPHA;
            }

            /*
             * Set the default texture format and offset.
             */
            pmtsState->dwTextureOffset[1] = pCurrentContext->dwDefaultTextureOffset;
            pmtsState->dwTextureFormat[1] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     pCurrentContext->dwDefaultTextureContextDma)
                                          | DRF_NUM(055, _FORMAT, _COLOR,           pCurrentContext->dwDefaultTextureColorFormat)
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

            pmtsState->dwTextureFilter[1] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, pDriverData->regLODBiasAdjust);

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
            nvSetDX6State(pdwRenderState, pmtsState);

            /*
             * Need to use the DX6 triangle class here.
             */
            pCurrentContext->bUseDX6Class       = TRUE;
            pCurrentContext->mtsState.bTSSValid = TRUE;
            pCurrentContext->dwStateChange      = FALSE;

            /*
             * This code is only enabled in the DEBUG build.
             * Display the hardware registers that were calculated.
             */
            dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);
        }

        pCurrentContext->mtsState.ddrval    = DD_OK;
        pCurrentContext->dwStageCount       = 1;
        /*
         * Note that the state has changed and needs to be sent to the hardware.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);
        return;
    }


    /*
     * Compile the hardware state for each texture stage.
     * Any errors during compile will result in disabling multi-textureing.
     *
     * Stage 0.
     */
    if (ptssState0->dwColorOp == D3DTOP_DISABLE)
    {
        /*
         * No texturing at all!
         */
        if (!nvCheckStencilBuffer())
        {
            nvConvertTextureStageToDX5Class();
            pCurrentContext->bUseDX6Class       = FALSE;
            pCurrentContext->mtsState.bTSSValid = FALSE;
            pCurrentContext->dwStateChange      = FALSE;
        }
        else
        {
            nvSetDefaultMultiTextureHardwareState();
            pCurrentContext->bUseDX6Class       = TRUE;
            pCurrentContext->mtsState.bTSSValid = TRUE;
        }
        pCurrentContext->mtsState.ddrval    = DD_OK;
        pCurrentContext->dwStageCount       = 1;

        /*
         * This code is only enabled in the DEBUG build.
         * Display the hardware registers that were calculated.
         */
        dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);

        /*
         * Note that the state has changed and needs to be sent to the hardware.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);
        return;
    }

    /*
     * Check for a texture stage state that can be handled by the DX5 class.
     */
    if ((ptssState1->dwColorOp == D3DTOP_DISABLE)
     && (!nvCheckStencilBuffer()))
    {
        /*
         * If there's only one texture stage, and no stencil buffer.
         * There's an opportunity to use the DX5 class for this
         * operation and get 2 Pixels per clock.
         */
        if (nvConvertTextureStageToDX5Class())
        {
            /*
             * Single texture stage was converted to DX5 class.
             * Use it instead.
             */
            pCurrentContext->bUseDX6Class  = FALSE;
            pCurrentContext->dwStateChange = FALSE;

            /*
             * Show that the DX6 class state is not calculated.
             */
            pCurrentContext->mtsState.bTSSValid = FALSE;
            pCurrentContext->mtsState.ddrval    = DD_OK;
            return;
        }
        if (ptssState0->dwAlphaOp != D3DTOP_DISABLE)
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "D3D:nvSetMultiTextureHardwareState - Single Texture Stage Using DX6 Class!!!");
    }

    /*
     * Setup stage 0 texture offset
     * Setup stage 0 texture format
     */
    nvSetDX6TextureState(0, ptssState0, pmtsState);

#ifdef NV_TEX2
    /*
     * setup current texture
     */
    pCurrentContext->pTexture0 = (PNVD3DTEXTURE)ptssState0->dwTextureMap;
#endif

    /*
     * Setup stage 0 Alpha Combiner.
     */
    dwStageOp = ptssState0->dwAlphaOp;
    switch (ptssState0->dwAlphaArg1 & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssState0->dwTextureMap)
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE0;
            else
            {
                /*
                 * Disable Arg1 if there is no texture handle selected.
                 */
                arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
                dwStageOp  = D3DTOP_SELECTARG1;
            }
            break;
        case D3DTA_TFACTOR:
            arg1Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_FACTOR;
            break;
    }
    arg1Invert = !((ptssState0->dwAlphaArg1 & D3DTA_COMPLEMENT) == 0);
    switch (ptssState0->dwAlphaArg2 & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            /*
             * D3DTA_TEXTURE isn't really a valid argument for Arg2.
             */
            if (ptssState0->dwTextureMap)
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE0;
            else
                arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            arg2Select = NV055_COMBINE_0_ALPHA_ARGUMENT_0_FACTOR;
            break;
    }
    arg2Invert = !((ptssState0->dwAlphaArg2 & D3DTA_COMPLEMENT) == 0);
#ifdef  DEBUG
    if ((arg1Select == NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE0)
     && (ptssState0->dwAlphaOp != D3DTOP_DISABLE)
     && (ptssState0->dwTextureMap))
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
            if (ptssState1->dwColorOp != D3DTOP_DISABLE)
            {
                DWORD   arg1InvertStage1;
                DWORD   arg1SelectStage1;

#if 0
                /*
                 * When there is a second texture stage, stage 0 for PREMODULATE is defined as:
                 * Stage 0 arg1 * Stage 1 arg1
                 */
                switch (ptssState1->dwAlphaArg1 & D3DTA_SELECTMASK)
                {
                    case D3DTA_DIFFUSE:
                        arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
                        break;
                    case D3DTA_CURRENT:
                        arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_INPUT;
                        break;
                    case D3DTA_TEXTURE:
                        if (ptssState1->dwTextureMap)
                            arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE1;
                        else
                            arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
                        break;
                    case D3DTA_TFACTOR:
                        arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_FACTOR;
                        break;
                }
                arg1InvertStage1 = !((ptssState1->dwAlphaArg1 & D3DTA_COMPLEMENT) == 0);
#else
                if (ptssState1->dwTextureMap)
                    arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE1;
                else
                    arg1SelectStage1 = NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE;
                arg1InvertStage1 = NV055_COMBINE_0_ALPHA_INVERSE_1_NORMAL;
#endif
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
            nvSetDefaultMultiTextureHardwareState();
            pCurrentContext->mtsState.bTSSValid = FALSE;
            pCurrentContext->mtsState.ddrval    = D3DERR_UNSUPPORTEDALPHAOPERATION;
            pCurrentContext->dwStageCount       = 1;
            if (!nvCheckStencilBuffer())
                pCurrentContext->bUseDX6Class       = FALSE;
            else
                pCurrentContext->bUseDX6Class       = TRUE;

            /*
             * This code is only enabled in the DEBUG build.
             * Display the hardware registers that were calculated.
             */
            dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);

            /*
             * Note that the state has changed and needs to be sent to the hardware.
             */
            NV_FORCE_TRI_SETUP(pCurrentContext);
            return;
    }


    /*
     * Setup stage 0 Color Combiner.
     */
    dwStageOp = ptssState0->dwColorOp;
    switch (ptssState0->dwColorArg1 & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            if (ptssState0->dwTextureMap)
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE0;
            else
            {
                /*
                 * Disable Arg1 if there is no texture handle selected.
                 */
                arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
                dwStageOp  = D3DTOP_SELECTARG1;
            }
            break;
        case D3DTA_TFACTOR:
            arg1Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_FACTOR;
            break;
    }
    arg1Invert   = !((ptssState0->dwColorArg1 & D3DTA_COMPLEMENT) == 0);
    arg1AlphaRep = !((ptssState0->dwColorArg1 & D3DTA_ALPHAREPLICATE) == 0);
    switch (ptssState0->dwColorArg2 & D3DTA_SELECTMASK)
    {
        case D3DTA_DIFFUSE:
            arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_CURRENT:
            arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TEXTURE:
            /*
             * D3DTA_TEXTURE isn't really a valid argument for Arg2.
             */
            if (ptssState0->dwTextureMap)
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE0;
            else
                arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
            break;
        case D3DTA_TFACTOR:
            arg2Select = NV055_COMBINE_0_COLOR_ARGUMENT_0_FACTOR;
            break;
    }
    arg2Invert   = !((ptssState0->dwColorArg2 & D3DTA_COMPLEMENT) == 0);
    arg2AlphaRep = !((ptssState0->dwColorArg2 & D3DTA_ALPHAREPLICATE) == 0);
#ifdef  DEBUG
    if ((arg1Select == NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE0)
     && (ptssState0->dwColorOp != D3DTOP_DISABLE)
     && (ptssState0->dwTextureMap))
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
            if (ptssState1->dwColorOp != D3DTOP_DISABLE)
            {
                DWORD   arg1InvertStage1;
                DWORD   arg1AlphaRepStage1;
                DWORD   arg1SelectStage1;

#if 0
                /*
                 * When there is a second texture stage, stage 0 for PREMODULATE is defined as:
                 * Stage 0 arg1 * Stage 1 arg1
                 */
                switch (ptssState1->dwColorArg1 & D3DTA_SELECTMASK)
                {
                    case D3DTA_DIFFUSE:
                        arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
                        break;
                    case D3DTA_CURRENT:
                        arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
                        break;
                    case D3DTA_TEXTURE:
                        if (ptssState1->dwTextureMap)
                            arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE1;
                        else
                            arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
                        break;
                    case D3DTA_TFACTOR:
                        arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_FACTOR;
                        break;
                }
                arg1InvertStage1   = !((ptssState1->dwColorArg1 & D3DTA_COMPLEMENT) == 0);
                arg1AlphaRepStage1 = !((ptssState1->dwColorArg1 & D3DTA_ALPHAREPLICATE) == 0);
#else
                if (ptssState1->dwTextureMap)
                    arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE1;
                else
                    arg1SelectStage1 = NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE;
                arg1InvertStage1   = NV055_COMBINE_0_COLOR_INVERSE_1_NORMAL;
                arg1AlphaRepStage1 = NV055_COMBINE_0_COLOR_ALPHA_1_COLOR;
#endif
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
            nvSetDefaultMultiTextureHardwareState();
            pCurrentContext->mtsState.bTSSValid = FALSE;
            pCurrentContext->mtsState.ddrval    = D3DERR_UNSUPPORTEDCOLOROPERATION;
            pCurrentContext->dwStageCount       = 1;
            if (!nvCheckStencilBuffer())
                pCurrentContext->bUseDX6Class       = FALSE;
            else
                pCurrentContext->bUseDX6Class       = TRUE;

            /*
             * This code is only enabled in the DEBUG build.
             * Display the hardware registers that were calculated.
             */
            dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);

            /*
             * Note that the state has changed and needs to be sent to the hardware.
             */
            NV_FORCE_TRI_SETUP(pCurrentContext);
            return;
    }
    dwStageCount++;

    /*
     * Stage 1.
     */
    if (ptssState1->dwColorOp != D3DTOP_DISABLE)
    {
        nvSetDX6TextureState(1, ptssState1, pmtsState);

#ifdef NV_TEX2
        /*
         * setup current texture
         */
        pCurrentContext->pTexture1 = (PNVD3DTEXTURE)ptssState1->dwTextureMap;
#endif
        /*
         * Setup stage 1 Alpha Combiner.
         */
        dwStageOp = ptssState1->dwAlphaOp;
        switch (ptssState1->dwAlphaArg1 & D3DTA_SELECTMASK)
        {
            case D3DTA_DIFFUSE:
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_DIFFUSE;
                break;
            case D3DTA_CURRENT:
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
                break;
            case D3DTA_TEXTURE:
                if (ptssState1->dwTextureMap)
                    arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1;
                else
                {
                    /*
                     * Disable Arg1 if there is no texture handle selected.
                     */
                    arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
//                    dwStageOp  = D3DTOP_SELECTARG1;
                }
                break;
            case D3DTA_TFACTOR:
                arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_FACTOR;
                break;
        }
        arg1Invert = !((ptssState1->dwAlphaArg1 & D3DTA_COMPLEMENT) == 0);
        switch (ptssState1->dwAlphaArg2 & D3DTA_SELECTMASK)
        {
            case D3DTA_DIFFUSE:
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_DIFFUSE;
                break;
            case D3DTA_CURRENT:
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
                break;
            case D3DTA_TEXTURE:
                /*
                 * D3DTA_TEXTURE isn't really a valid argument for Arg2.
                 */
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1;
                break;
            case D3DTA_TFACTOR:
                arg2Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_FACTOR;
                break;
        }
        arg2Invert = !((ptssState1->dwAlphaArg2 & D3DTA_COMPLEMENT) == 0);
#ifdef  DEBUG
        if ((arg1Select == NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1)
         && (ptssState1->dwAlphaOp != D3DTOP_DISABLE)
         && (ptssState1->dwTextureMap))
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
                nvSetDefaultMultiTextureHardwareState();
                pCurrentContext->mtsState.bTSSValid = FALSE;
                pCurrentContext->mtsState.ddrval    = D3DERR_UNSUPPORTEDALPHAOPERATION;
                pCurrentContext->dwStageCount       = 1;
                if (!nvCheckStencilBuffer())
                    pCurrentContext->bUseDX6Class       = FALSE;
                else
                    pCurrentContext->bUseDX6Class       = TRUE;

                /*
                 * This code is only enabled in the DEBUG build.
                 * Display the hardware registers that were calculated.
                 */
                dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);

                /*
                 * Note that the state has changed and needs to be sent to the hardware.
                 */
                NV_FORCE_TRI_SETUP(pCurrentContext);
                return;
        }

        /*
         * Setup stage 1 Color Combiner.
         */
        dwStageOp = ptssState1->dwColorOp;
        switch (ptssState1->dwColorArg1 & D3DTA_SELECTMASK)
        {
            case D3DTA_DIFFUSE:
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_DIFFUSE;
                break;
            case D3DTA_CURRENT:
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_INPUT;
                break;
            case D3DTA_TEXTURE:
                if (ptssState1->dwTextureMap)
                    arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1;
                else
                {
                    /*
                     * Disable Arg1 if there is no texture handle selected.
                     */
                    arg1Select = NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT;
//                    dwStageOp  = D3DTOP_SELECTARG1;
                }
                break;
            case D3DTA_TFACTOR:
                arg1Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_FACTOR;
                break;
        }
        arg1Invert   = !((ptssState1->dwColorArg1 & D3DTA_COMPLEMENT) == 0);
        arg1AlphaRep = !((ptssState1->dwColorArg1 & D3DTA_ALPHAREPLICATE) == 0);
        switch (ptssState1->dwColorArg2 & D3DTA_SELECTMASK)
        {
            case D3DTA_DIFFUSE:
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_DIFFUSE;
                break;
            case D3DTA_CURRENT:
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_INPUT;
                break;
            case D3DTA_TEXTURE:
                /*
                 * D3DTA_TEXTURE isn't really a valid argument for Arg2.
                 */
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1;
                break;
            case D3DTA_TFACTOR:
                arg2Select = NV055_COMBINE_1_COLOR_ARGUMENT_0_FACTOR;
                break;
        }
        arg2Invert   = !((ptssState1->dwColorArg2 & D3DTA_COMPLEMENT) == 0);
        arg2AlphaRep = !((ptssState1->dwColorArg2 & D3DTA_ALPHAREPLICATE) == 0);
#ifdef  DEBUG
        if ((arg1Select == NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1)
         && (ptssState1->dwColorOp != D3DTOP_DISABLE)
         && (ptssState1->dwTextureMap))
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
                nvSetDefaultMultiTextureHardwareState();
                pCurrentContext->mtsState.bTSSValid = FALSE;
                pCurrentContext->mtsState.ddrval    = D3DERR_UNSUPPORTEDCOLOROPERATION;
                pCurrentContext->dwStageCount       = 1;
                if (!nvCheckStencilBuffer())
                    pCurrentContext->bUseDX6Class       = FALSE;
                else
                    pCurrentContext->bUseDX6Class       = TRUE;

                /*
                 * This code is only enabled in the DEBUG build.
                 * Display the hardware registers that were calculated.
                 */
                dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);

                /*
                 * Note that the state has changed and needs to be sent to the hardware.
                 */
                NV_FORCE_TRI_SETUP(pCurrentContext);
                return;
        }
        dwStageCount++;
#ifdef  DEBUG
        if (bStage0UsesTexture && bStage1UsesTexture)
        {
            PNVD3DTEXTURE   pTexture0, pTexture1;
            pTexture0 = (PNVD3DTEXTURE)(ptssState0->dwTextureMap);
            pTexture1 = (PNVD3DTEXTURE)(ptssState1->dwTextureMap);
            if (pTexture0->dwTextureContextDma != pTexture1->dwTextureContextDma)
            {
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "D3D:nvSetMultiTextureHardwareState - Multi-Textures Memory Space Error!!");
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture0 = %08lx,  Context = %08lx", ptssState0->dwTextureMap, pTexture0->dwTextureContextDma);
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture1 = %08lx,  Context = %08lx", ptssState1->dwTextureMap, pTexture1->dwTextureContextDma);
                dbgD3DError();
            }
            else
            {
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "D3D:nvSetMultiTextureHardwareState - YEAH! Multi-Textures Memory Space OK!!");
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture0 = %08lx,  Context = %08lx", ptssState0->dwTextureMap, pTexture0->dwTextureContextDma);
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "        pTexture1 = %08lx,  Context = %08lx", ptssState1->dwTextureMap, pTexture1->dwTextureContextDma);
            }
        }
#endif  // DEBUG
    }
    else
    {
        DWORD   dwZOHMode, dwFOHMode;
        dwZOHMode = ((pDriverData->regTexelAlignment & REG_TA_ZOH_MASK) == REG_TA_ZOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_ZOH_CENTER : NV054_FORMAT_ORIGIN_ZOH_CORNER;
        dwFOHMode = ((pDriverData->regTexelAlignment & REG_TA_FOH_MASK) == REG_TA_FOH_CENTER)
                  ? NV054_FORMAT_ORIGIN_FOH_CENTER : NV054_FORMAT_ORIGIN_FOH_CORNER;

        /*
         * Set the default texture format and offset.
         */
        pmtsState->dwTextureOffset[1] = pCurrentContext->dwDefaultTextureOffset;
        pmtsState->dwTextureFormat[1] = DRF_NUM(055, _FORMAT, _CONTEXT_DMA,     pCurrentContext->dwDefaultTextureContextDma)
                                      | DRF_NUM(055, _FORMAT, _COLOR,           pCurrentContext->dwDefaultTextureColorFormat)
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

        pmtsState->dwTextureFilter[1] |= DRF_NUM(055, _FILTER, _MIPMAPLODBIAS, pDriverData->regLODBiasAdjust);

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

#ifdef NV_TEX2
        /*
         * setup current texture
         */
        pCurrentContext->pTexture0 = NULL;
#endif
    }
    nvSetDX6State(pdwRenderState, pmtsState);

    /*
     * If stage 3 is not disabled, flag the error but the hardware will still
     * use the first two stages of setup.
     */
    if (pCurrentContext->tssState[2].dwColorOp != D3DTOP_DISABLE)
    {
        pCurrentContext->mtsState.bTSSValid = FALSE;
        pCurrentContext->mtsState.ddrval    = D3DERR_TOOMANYOPERATIONS;
        pCurrentContext->bUseDX6Class       = TRUE;

        /*
         * This code is only enabled in the DEBUG build.
         * Display the hardware registers that were calculated.
         */
        dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);

        return;
    }

    /*
     *
     */
    pCurrentContext->mtsState.bTSSValid = TRUE;
    pCurrentContext->mtsState.ddrval    = DD_OK;

    /*
     * Assume that the DX6 class is going to be used for rendering.
     * stage or if the stencil buffer is enabled.
     */
    pCurrentContext->bUseDX6Class  = TRUE;
    pCurrentContext->dwStateChange = FALSE;

    /*
     * This code is only enabled in the DEBUG build.
     * Display the hardware registers that were calculated.
     */
    dbgDisplayContextState(NVDBG_SHOW_DX6_CLASS_STATE, pCurrentContext);
    return;
}
#endif  // NV4
