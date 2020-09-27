/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvClear.cpp                                                       *
*   NV4 Buffer Clear routines.                                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       04/18/98 - created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//---------------------------------------------------------------------------

void nvSetClearSurface
(
    DWORD   dwDstOffset,
    DWORD   dwDstPitch,
    DWORD   dwDstFormat,
    DWORD   dwRectFormat,
    DWORD   dwFillValue
)
{
    //DWORD size;

    /*
     * Set the destination surface to be cleared.
     */
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT)) {
        //fortunately the NV05E (SOLID_RECTANGLE) surface formats are identical to the
        //NV04A (GDI_RECTANGLE_TEXT) surface formats so they don't need to be remapped here.
        //this is a rather a casual assumption -- if these formats change for NV20 we may
        // need to do modification mapping one to the other.
//        size = sizeSetObject + sizeSet2DSurfacesDestination + sizeSet2DSurfacesColorFormat +
//               sizeSet2DSurfacesPitch + sizeSetNv4GDIRectangleTextColorFormat + sizeNv4GDIRectangleTextColorFormat;
        nvglSet2DSurfacesDestination(NV_DD_SURFACES, dwDstOffset);
        nvglSet2DSurfacesColorFormat(NV_DD_SURFACES, dwDstFormat);
        nvglSet2DSurfacesPitch(NV_DD_SURFACES, dwDstPitch, dwDstPitch);
        nvglSetObject(NV_DD_SPARE, D3D_GDI_RECTANGLE_TEXT);
        nvglDrawRopRectAndTextSetColorFormat(NV_DD_SPARE, dwRectFormat);
        nvglDrawRopRectAndTextColor1A(NV_DD_SPARE, dwFillValue);
        pDriverData->dDrawSpareSubchannelObject = D3D_GDI_RECTANGLE_TEXT;
    }
    else{
//        size = sizeSetObject + sizeSet2DSurfacesDestination + sizeSet2DSurfacesColorFormat +
//               sizeSet2DSurfacesPitch + sizeSetNv4SolidRectangleColorFormat + sizeNv4SolidRectangleColor;
        nvglSet2DSurfacesDestination(NV_DD_SURFACES, dwDstOffset);
        nvglSet2DSurfacesColorFormat(NV_DD_SURFACES, dwDstFormat);
        nvglSet2DSurfacesPitch(NV_DD_SURFACES, dwDstPitch, dwDstPitch);
        nvglSetObject(NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE);
        nvglSetNv4SolidRectangleColorFormat(NV_DD_SPARE, dwRectFormat);
        nvglNv4SolidRectangleColor(NV_DD_SPARE, dwFillValue);
        pDriverData->dDrawSpareSubchannelObject = D3D_RENDER_SOLID_RECTANGLE;
    }

}

//---------------------------------------------------------------------------

void nvReset2DSurfaceFormat
(
    void
)
{
    switch (GET_MODE_BPP()) {
    case 8:
        nvglSet2DSurfacesColorFormat(NV_DD_SURFACES, NV042_SET_COLOR_FORMAT_LE_Y8);
        break;
    case 16:
        nvglSet2DSurfacesColorFormat(NV_DD_SURFACES, NV042_SET_COLOR_FORMAT_LE_R5G6B5);
        break;
    default:
        nvglSet2DSurfacesColorFormat(NV_DD_SURFACES, NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
        break;
    }
    return;
}

//---------------------------------------------------------------------------

void nvClearSurfaceRect
(
    DWORD startx,
    DWORD starty,
    DWORD width,
    DWORD height
)
{
    const DWORD dwTetrisWidth = 128;

    /*
     * use tiled (tetris) clear if width >= 1024 and we run nv5 or earlier
     */
    if (width >= 1024)
    {
        /*
         * extract relevant information
         */
        DWORD endx = startx + width;
        DWORD i = 0;

        while (i < endx)
        {
            /*
             * setup band
             */
            DWORD j     = i + dwTetrisWidth;
            DWORD left  = max(i,startx);
            DWORD right = min(j,endx);

            if (right > left)
            {
                /*
                 * Clear the specified rectangle.
                 */
                if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT)) {
                    nvglDrawRopRectAndTextPointSize(NV_DD_SPARE, left, starty, (right - left), height);
                }
                else {
                    nvglNv4SolidRectanglePointSize (NV_DD_SPARE, left, starty, (right - left), height);
                }
            }

            /*
             * next
             */
            i = j;
        }

        return;
    }

    /*
     * Clear the specified rectangle.
     */
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT)) {
        nvglDrawRopRectAndTextPointSize(NV_DD_SPARE, startx, starty, width, height);
    }
    else {
        nvglNv4SolidRectanglePointSize(NV_DD_SPARE, startx, starty, width, height);
    }
}

//---------------------------------------------------------------------------

// Clear render target and/or zeta-buffer. No explicit support for stencil.

DWORD __stdcall nvClear
(
    LPD3DHAL_CLEARDATA  pcd
)
{
    DWORD dwDstOffset, dwDstPitch;
    DWORD dwRTRectColorFormat, dwRTSurfColorFormat;
    DWORD dwZBRectColorFormat, dwZBSurfColorFormat;

    dbgTracePush ("nvClear");

#ifdef NV_NULL_HW_DRIVER
    pcd->ddrval = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
#endif

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pcd);

    // Get pointer to global driver data structure.
    nvSetDriverDataPtrFromContext (pContext);

    nvSetD3DSurfaceState (pContext);


    if (global.dwDXRuntimeVersion < 0x700) {
        nvPusherSignalWaitForFlip (pContext->pRenderTarget->getfpVidMem(), DDSCAPS_PRIMARYSURFACE);
    }

    if ((!pContext->pRenderTarget) || (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED))
    {
        pcd->ddrval = DD_OK;
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    DDSTARTTICK(DEVICE_CLEARRENDERZ);

    /*
     * Need to make sure that an unfriendly mode switch didn't sneak and not cause
     * us to get re-enabled properly.
     */
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            pcd->ddrval = DD_OK;
            DDENDTICK(DEVICE_CLEARRENDERZ);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

#if (NVARCH >= 0x010)
    // Create AA buffers if needed
    pContext->aa.Create(pContext);
#endif  // NVARCH >= 0x010

    // Get the format of the renter target being cleared
    if (pContext->pRenderTarget->getBPP() == 2) {
        dwRTRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X16R5G6B5;
        dwRTSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y16;
    }
    else {
        // 32-bit
        dwRTRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8;
        dwRTSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y32;
    }

    if (pContext->pZetaBuffer) {
        // Get the format of the zeta buffer being cleared
        if (pContext->pZetaBuffer->getBPP() == 2) {
            dwZBRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X16R5G6B5;
            dwZBSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y16;
        }
        else {
            // 32-bit
            dwZBRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8;
            dwZBSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y32;
        }
    }

    CNvObject *pNvObj = pContext->pZetaBuffer ? pContext->pZetaBuffer->getWrapper() : NULL;
    if (pNvObj)
    {
#ifdef ALTERNATE_STENCIL_MODE
        if ((pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE) &&
            (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_ALT_STENCIL) &&
            (pcd->dwFlags & D3DCLEAR_ZBUFFER)) {

            if (pContext->pZetaBuffer->getBPP() == 2) pNvObj->tagAltStencilBadSemantics();

            if (pNvObj->hasAltStencilBadSemantics()) {
                pNvObj->tagAltStencilDisabled();
            } else {
                pNvObj->disableCT();
                pNvObj->tagAltStencilEnabled();
            }

            DWORD dwOldClearCount = pContext->dwTotalClearCount;

            pContext->dwTotalClearCount = (pContext->dwTotalClearCount+1) % 256;

            pContext->hwState.celsius.set (NV056_SET_STENCIL_TEST_ENABLE, NV056_SET_STENCIL_TEST_ENABLE_V_TRUE);
            pContext->hwState.celsius.set (NV056_SET_STENCIL_FUNC,        NV056_SET_STENCIL_FUNC_V_NOTEQUAL);
            pContext->hwState.celsius.set (NV056_SET_STENCIL_FUNC_MASK,   0xff);
            pContext->hwState.celsius.set (NV056_SET_STENCIL_MASK,        0xff);
            pContext->hwState.celsius.set (NV056_SET_STENCIL_OP_FAIL,     NV056_SET_STENCIL_OP_FAIL_V_KEEP);
            pContext->hwState.celsius.set (NV056_SET_STENCIL_OP_ZFAIL,    NV056_SET_STENCIL_OP_FAIL_V_REPLACE);
            pContext->hwState.celsius.set (NV056_SET_STENCIL_OP_ZPASS,    NV056_SET_STENCIL_OP_FAIL_V_REPLACE);
            pContext->hwState.celsius.set (NV056_SET_STENCIL_FUNC_REF,    dwOldClearCount);

            *(volatile int *)(pDriverData->NvBaseFlat+0x400088) |= 0x40000000;

            if (dwOldClearCount == 0) {
                pcd->dwFlags |= D3DCLEAR_STENCIL;
                pcd->dwFillDepth &= 0xffffff00;
            } else {
                pcd->dwFlags &= ~D3DCLEAR_ZBUFFER;
            }
        } else
#endif
        if ((pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE)
             && (((pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CONTROL_TRAFFIC_16) && (dwZBSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16))
             || ((pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CONTROL_TRAFFIC_32) && (dwZBSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y32))))
        {
            BOOL bOK = TRUE;
            if (pContext->dwDXAppVersion >= 0x0700) {
                bOK = (dwZBSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y32)
                   || ((pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CT_11M) && (pContext->dwEarlyCopyStrategy > 3))
                   || ((pContext->pRenderTarget->getBPP() == 4) && (dwZBSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16) && (pContext->dwEarlyCopyStrategy > 3));
            }
            else
            {
                // DX6 or earlier apps - disable CT when using 16 bit z
                bOK = ((dwZBSurfColorFormat != NV042_SET_COLOR_FORMAT_LE_Y16)
                        || ((!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)) && (pContext->dwEarlyCopyStrategy > 0x20)));
            }

            if (((pcd->dwFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET)) == D3DCLEAR_ZBUFFER)
                && (((dwZBSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16) && ((pcd->dwFillDepth & 0xffff) == 0xffff)) ||
                    ((dwZBSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y32) && ((pcd->dwFillDepth & 0xffffff00) == 0xffffff00)))
                && bOK
                )
            {
                BOOL x = FALSE;

                if (pNvObj->hasCTEnabled())
                {
                    pContext->dwClearCount++;
                    if (pContext->dwClearCount > 1)
                    {
                        pNvObj->tagDblClear();
                    }
                    if (pNvObj->hasBadCTSem())
                    {
                        pNvObj->disableCT();
                    }
                    else
                    {
                        if (pNvObj->isCTDrawPrim())
                        {
                            pNvObj->toggleFrontDrawPrim();
                        }
                        x = TRUE;
                    }
                }
                else
                {
                    if (pNvObj->hasBadCTSem())
                    {
                        pNvObj->disableCT();
                    }
                    else
                    {
                        if (dwZBSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16)
                        {
                            pNvObj->enableCT();
                            if (pcd->dwFillDepth & 0x8000) pNvObj->setCTFront();
                        }
                        else
                        {
                            pNvObj->enableCT();
                            if (pcd->dwFillDepth & 0x80000000) pNvObj->setCTFront();
                        }
                        pNvObj->clearCTDrawPrim();
                        { static int i = 1; if (i) { DPF ("***********************"); i = 0; } }
                    }
                }

#if (NVARCH >= 0x010)
                if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_KELVIN | NVCLASS_FAMILY_CELSIUS)) {
                    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM  | KELVIN_DIRTY_MISC_STATE;
                    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM | CELSIUS_DIRTY_MISC_STATE;
                }
                else
#endif  // NVARCH >= 0x010
                {
                    NV_FORCE_TRI_SETUP (pContext);
                }

                if (x)
                {
                    pcd->ddrval = DD_OK;
                    DDENDTICK(DEVICE_CLEARRENDERZ);
                    NvReleaseSemaphore(pDriverData);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
            }
            else
            {
                if (pNvObj->hasCTEnabled())
                {
#if (NVARCH >= 0x010)
                    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_KELVIN | NVCLASS_FAMILY_CELSIUS)) {
                        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM  | KELVIN_DIRTY_MISC_STATE;
                        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM | CELSIUS_DIRTY_MISC_STATE;
                    }
                    else
#endif  // NVARCH >= 0x010
                    {
                        NV_FORCE_TRI_SETUP (pContext);
                    }
                    pNvObj->disableCT();
                }
            }
        }
        else
        {
            pNvObj->disableCT();
            // tri setup forced implicitly
        }
    }



#if (NVARCH >= 0x010)
    if (pContext->aa.isEnabled()) {

        // select large buffer (without magnify)
        nvCelsiusAASelectSuperBuffers (pContext);


        // clear z buffer
        if ((pcd->dwFlags & D3DCLEAR_ZBUFFER) && (pContext->aa.pSuperZetaBuffer))
        {
            DWORD       dwNumRects  = pcd->dwNumRects;
            LPD3DRECT   lpRects     = pcd->lpRects;

            pContext->aa.makeSuperZBValid(pContext);

            DWORD dwFillDepth = pcd->dwFillDepth;
#ifndef STEREO_SUPPORT
            dwDstOffset = pContext->aa.pSuperZetaBuffer->getOffset();
#else   //STEREO_SUPPORT==1
            dwDstOffset = GetStereoOffset(pContext->aa.pSuperZetaBuffer);
#endif  //STEREO_SUPPORT
            dwDstPitch  = pContext->aa.pSuperZetaBuffer->getPitch();

            if (dwNumRects)
            {
                nvSetClearSurface  (dwDstOffset, dwDstPitch, dwZBSurfColorFormat, dwZBRectColorFormat, dwFillDepth);
                while (dwNumRects)
                {
                    DWORD startx, starty;
                    DWORD width, height;

                    /*
                     * Get the next rectangle to clear.
                     */
                    startx = DWORD(0.5f + lpRects->x1 * pContext->aa.fWidthAmplifier);
                    width  = DWORD((lpRects->x2 - lpRects->x1) * pContext->aa.fWidthAmplifier);

                    starty = DWORD(0.5f + lpRects->y1 * pContext->aa.fHeightAmplifier);
                    height = DWORD((lpRects->y2 - lpRects->y1) * pContext->aa.fHeightAmplifier);

                    /*
                     * Use the hardware to do the clear.
                     */
                    nvClearSurfaceRect(startx, starty, width, height);

                    /*
                     * Move to the next rectangle.
                     */
                    lpRects++;
                    dwNumRects--;
                }
            }
        }

        // clear frame buffer
        if ((pcd->dwFlags & D3DCLEAR_TARGET) && (pContext->aa.pSuperRenderTarget))
        {
            DWORD       dwNumRects  = pcd->dwNumRects;
            LPD3DRECT   lpRects     = pcd->lpRects;

            DWORD       dwFillColor;

            pContext->aa.makeSuperBufferValid(pContext);

            if (dwRTSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16)
                /*
                 * Get the fill color (24bit RGB) and convert it to 16bpp (565) format.
                 */
                dwFillColor = ((pcd->dwFillColor & 0x00F80000) >> 8)
                            | ((pcd->dwFillColor & 0x0000FC00) >> 5)
                            | ((pcd->dwFillColor & 0x000000F8) >> 3);
            else
                dwFillColor = pcd->dwFillColor;

#ifndef STEREO_SUPPORT
            dwDstOffset = pContext->aa.pSuperRenderTarget->getOffset();
#else   //STEREO_SUPPORT==1
            dwDstOffset = GetStereoOffset(pContext->aa.pSuperRenderTarget);
#endif  //STEREO_SUPPORT
            dwDstPitch  = pContext->aa.pSuperRenderTarget->getPitch();

            if (dwNumRects)
            {
                nvSetClearSurface  (dwDstOffset, dwDstPitch, dwRTSurfColorFormat, dwRTRectColorFormat, dwFillColor);

                while (dwNumRects)
                {
                    DWORD startx, starty;
                    DWORD width, height;

                    /*
                     * Get the next rectangle to clear.
                     */
                    startx = DWORD(0.5f + lpRects->x1 * pContext->aa.fWidthAmplifier);
                    width  = DWORD((lpRects->x2 - lpRects->x1) * pContext->aa.fWidthAmplifier);

                    starty = DWORD(0.5f + lpRects->y1 * pContext->aa.fHeightAmplifier);
                    height = DWORD((lpRects->y2 - lpRects->y1) * pContext->aa.fHeightAmplifier);


                    /*
                     * Use the hardware to do the clear.
                     */
                    nvClearSurfaceRect(startx, starty, width, height);

                    /*
                     * Move to the next rectangle.
                     */
                    lpRects++;
                    dwNumRects--;
                }
            }
        }
    }

    else // !pContext->aa.isEnabled()

#endif  // NVARCH >= 0x010

    {
#if (NVARCH >= 0x020)
        CSimpleSurface *pRenderTarget = pContext->kelvinAA.GetCurrentRT(pContext);
        CSimpleSurface *pZetaBuffer = pContext->kelvinAA.GetCurrentZB(pContext);
#else
        CSimpleSurface *pRenderTarget = pContext->pRenderTarget;
        CSimpleSurface *pZetaBuffer = pContext->pZetaBuffer;
#endif


        // conditionally clear the z-buffer
        if ((pcd->dwFlags & D3DCLEAR_ZBUFFER) && pZetaBuffer)
        {

            DWORD dwFillDepth = pcd->dwFillDepth;

            // Normal (not AA)
            {
                DWORD       dwNumRects  = pcd->dwNumRects;
                LPD3DRECT   lpRects     = pcd->lpRects;

#ifndef STEREO_SUPPORT
                dwDstOffset = pZetaBuffer->getOffset();
#else   //STEREO_SUPPORT==1
                dwDstOffset = GetStereoOffset(pZetaBuffer);
#endif  //STEREO_SUPPORT
                dwDstPitch  = pRenderTarget->isSwizzled() ?
                                pRenderTarget->getPitch() : pZetaBuffer->getPitch();
                //syncrhonization marker in case DDRAW wants to do a lock later
                pZetaBuffer->hwLock(CSimpleSurface::LOCK_NORMAL);
                if (dwNumRects)
                {
                    /*
                     * Set the destination surface to be cleared.
                     */
                    nvSetClearSurface(dwDstOffset, dwDstPitch, dwZBSurfColorFormat, dwZBRectColorFormat, dwFillDepth);

                    while (dwNumRects)
                    {
                        DWORD startx, starty;
                        DWORD width, height;

                        /*
                         * Get the next rectangle to clear.
                         */
                        startx = lpRects->x1;
                        starty = lpRects->y1;
                        width  = lpRects->x2 - startx;
                        height = lpRects->y2 - starty;

                        /*
                         * Use the hardware to do the clear.
                         */
                        nvClearSurfaceRect(startx, starty, width, height);

                        /*
                         * Move to the next rectangle.
                         */
                        lpRects++;
                        dwNumRects--;
                    }
                }
                //syncrhonization marker in case DDRAW wants to do a lock later
                pZetaBuffer->hwUnlock();

            }
        }

        // clear the render surface
        if ((pcd->dwFlags & D3DCLEAR_TARGET) && pRenderTarget)
        {
            DWORD       dwFillColor;
            DWORD       dwNumRects  = pcd->dwNumRects;
            LPD3DRECT   lpRects     = pcd->lpRects;

            CNvObject *pNvObj = pContext->pRenderTarget->getWrapper();

            if (!dbgFrontRender) {
                pNvObj->checkClearRT(pContext->dwEarlyCopyStrategy, &pContext->xfmView._41, &pContext->xfmView._42, &pContext->xfmView._43);
                if (pNvObj->doClearRTNone()) goto skip_rt_clear;
            }

            if (dwRTSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16)
                /*
                 * Get the fill color (24bit RGB) and convert it to 16bpp (565) format.
                 */
                dwFillColor = ((pcd->dwFillColor & 0x00F80000) >> 8)
                            | ((pcd->dwFillColor & 0x0000FC00) >> 5)
                            | ((pcd->dwFillColor & 0x000000F8) >> 3);
            else
                dwFillColor = pcd->dwFillColor;

#ifndef STEREO_SUPPORT
            dwDstOffset = pRenderTarget->getOffset();
#else   //STEREO_SUPPORT==1
            dwDstOffset = GetStereoOffset(pRenderTarget);
#endif  //STEREO_SUPPORT
            dwDstPitch  = pRenderTarget->getPitch();
            //syncrhonization marker in case DDRAW wants to do a lock later
            pRenderTarget->hwLock(CSimpleSurface::LOCK_NORMAL);

            /*
             * Clear the rendering target.
             * Send the correct default objects for all the subchannels
             * just to be safe.
             */
            if (dwNumRects)
            {
                /*
                 * Set the destination surface to be cleared.
                 */
                nvSetClearSurface(dwDstOffset, dwDstPitch, dwRTSurfColorFormat, dwRTRectColorFormat, dwFillColor);
                while (dwNumRects)
                {
                    DWORD startx, starty;
                    DWORD width, height;

                    /*
                     * Get the next rectangle to clear.
                     */
                    startx = lpRects->x1;
                    starty = lpRects->y1;
                    width  = lpRects->x2 - startx;
                    height = lpRects->y2 - starty;

                    if (!dbgFrontRender) {
                        pNvObj->adjustRTCBounds(starty, height);
                    }

                    /*
                     * Use the hardware to do the clear.
                     */
                    nvClearSurfaceRect(startx, starty, width, height);

                    /*
                     * Move to the next rectangle.
                     */
                    lpRects++;
                    dwNumRects--;
                }
            }
            //syncrhonization marker in case DDRAW wants to do a lock later
            pRenderTarget->hwUnlock();
        }
skip_rt_clear:;
    }

    nvReset2DSurfaceFormat();
    pDriverData->TwoDRenderingOccurred   = TRUE;
    pDriverData->ThreeDRenderingOccurred = 0;
    nvPusherStart (FALSE);

    dbgFlushType (NVDBG_FLUSH_2D);

    pcd->ddrval = DD_OK;
    DDENDTICK(DEVICE_CLEARRENDERZ);
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//---------------------------------------------------------------------------

// clear a surface using a raster operation.

void nvClearSurfaceWithRop (PNVD3DCONTEXT pContext,
                            DWORD dwOffset, DWORD dwPitch,         // offset and pitch of surface to clear
                            LPD3DRECT lpRects, DWORD dwNumRects,   // rectangles to clear
                            DWORD dwAddressMode,                   // 8-bit granularity or 32?
                            DWORD dwROP,                           // raster operation
                            DWORD dwColor0, DWORD dwColor1,        // colors selected by pattern
                            DWORD dwPattern,                       // p=0 selects color0, p=1 selects color1
                            DWORD dwFillValue)                     // source value (fill value)
{
    DWORD dwSurfaceFormat, dwMultiplier;
    DWORD dwX1, dwY1, dwWidth, dwHeight;


    // the real surface depth must be 32. we fake things accordingly
    nvAssert (pContext->pRenderTarget);
    nvAssert (pContext->pRenderTarget->getBPP() == 4);
    nvAssert ((dwAddressMode==8) || (dwAddressMode==32));

    // set up the objects on which D3D_RENDER_SOLID_RECTANGLE depends:
    //      - NV_DD_SURFACES_2D (the target surface)
    //      - NV_DD_CONTEXT_PATTERN
    //      - NV_DD_CONTEXT_ROP
    // BUGBUG most of these are probably already set this way. try pruning this list down

    dwSurfaceFormat = (dwAddressMode==8) ? NV042_SET_COLOR_FORMAT_LE_Y8 : NV042_SET_COLOR_FORMAT_LE_Y32;
    dwMultiplier    = (dwAddressMode==8) ? 2 : 0;  // if we're addressing things by bytes, we multiply coordinates by 4

    nvglSet2DSurfacesColorFormat (NV_DD_SURFACES, dwSurfaceFormat);
    nvglSet2DSurfacesPitch (NV_DD_SURFACES, dwPitch, dwPitch);
    nvglSet2DSurfacesDestination (NV_DD_SURFACES, dwOffset);

    nvglSetObject (NV_DD_SPARE, NV_DD_CONTEXT_PATTERN);
    nvglSetNv4ContextPatternMonochromeFormat (NV_DD_SPARE, NV044_SET_MONOCHROME_FORMAT_LE_M1);
    nvglSetNv4ContextPatternMonochromeShape (NV_DD_SPARE, NV044_SET_MONOCHROME_SHAPE_8X_8Y);
    nvglSetNv4ContextPatternPatternSelect (NV_DD_SPARE, NV044_SET_PATTERN_SELECT_MONOCHROME);
    nvglSetNv4ContextPatternMonochromeColors (NV_DD_SPARE, dwColor0, dwColor1);
    nvglSetNv4ContextPatternMonochromePatterns (NV_DD_SPARE, dwPattern, dwPattern);

    nvglSetRop5 (NV_DD_ROP, dwROP);

    // set up the D3D_RENDER_SOLID_RECTANGLE object
    // BUGBUG most of these are probably already set this way. try pruning this list down

    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT)) {
        nvglSetObject (NV_DD_SPARE, D3D_GDI_RECTANGLE_TEXT);
        nvglDrawPatternRectAndTextSetPattern(NV_DD_SPARE, NV_DD_CONTEXT_PATTERN);
        nvglDrawRopRectAndTextSetROP(NV_DD_SPARE, NV_DD_CONTEXT_ROP);
        nvglDrawRopRectAndTextSetSurface(NV_DD_SPARE, NV_DD_SURFACES_2D);
        nvglDrawRopRectAndTextSetOperation(NV_DD_SPARE, NV04A_SET_OPERATION_ROP_AND);
        nvglDrawRopRectAndTextColor1A(NV_DD_SPARE, dwFillValue);
    }
    else{
        nvglSetObject (NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE);
        nvglSetNv4SolidRectanglePattern(NV_DD_SPARE, NV_DD_CONTEXT_PATTERN);
        nvglSetNv4SolidRectangleRop(NV_DD_SPARE, NV_DD_CONTEXT_ROP);
        nvglSetNv4SolidRectangleSurface (NV_DD_SPARE, NV_DD_SURFACES_2D);
        nvglSetNv4SolidRectangleOperation (NV_DD_SPARE, NV05E_SET_OPERATION_ROP_AND);
        nvglNv4SolidRectangleColor (NV_DD_SPARE, dwFillValue);
    }

    while (dwNumRects) {

        // get the window coordinates and size
        dwX1 = lpRects->x1;
        dwY1 = lpRects->y1;
        dwWidth  = lpRects->x2 - dwX1;
        dwHeight = lpRects->y2 - dwY1;

#if (NVARCH >= 0x010)
        if (pContext->aa.isEnabled()) {
            if (pContext->aa.isRenderTargetValid()) {
                // Make sure that it's the only one that's valid
                pContext->aa.makeRenderTargetValid(pContext);
            } else {
                float s;
                s        = pContext->aa.fWidthAmplifier;
                dwX1     = DWORD(0.5f + dwX1 * s);
                dwWidth  = DWORD(dwWidth * s + s - 1);
                s        = pContext->aa.fHeightAmplifier;
                dwY1     = DWORD(0.5f + dwY1 * s);
                dwHeight = DWORD(dwHeight * s + s - 1);
            }
        }
#endif  // NVARCH >= 0x010

        // scale x coords by 4 if we've lied to the HW about the surface format
        dwX1    <<= dwMultiplier;
        dwWidth <<= dwMultiplier;

        if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT)) {
            nvglDrawRopRectAndTextPointSize (NV_DD_SPARE, dwX1, dwY1, dwWidth, dwHeight);
        }
        else {
            nvglNv4SolidRectanglePointSize (NV_DD_SPARE, dwX1, dwY1, dwWidth, dwHeight);
        }

        lpRects++;
        dwNumRects--;

    }

    // put some key stuff back the way it was
    pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_PATTERN;
    nvglSetObject (NV_DD_SPARE, NV_DD_CONTEXT_PATTERN);
    nvglSetNv4ContextPatternMonochromeColors (NV_DD_SPARE, 0xffffffff, 0xffffffff);

    nvReset2DSurfaceFormat();

    pDriverData->TwoDRenderingOccurred   = TRUE;
    pDriverData->ThreeDRenderingOccurred = FALSE;

}

//---------------------------------------------------------------------------

void nvValidateClearRects(LPD3DRECT pRects, DWORD dwNumRects)
{
    const maxSize = 4096;

    for(DWORD dwRect = 0; dwRect < dwNumRects; dwRect++) {
        if (pRects[dwRect].x1 > maxSize) pRects[dwRect].x1 = maxSize;
        if (pRects[dwRect].x1 < 0 )      pRects[dwRect].x1 = 0;
        if (pRects[dwRect].x2 > maxSize) pRects[dwRect].x2 = maxSize;
        if (pRects[dwRect].x2 < 0)       pRects[dwRect].x2 = 0;
        if (pRects[dwRect].y1 > maxSize) pRects[dwRect].y1 = maxSize;
        if (pRects[dwRect].y1 < 0)       pRects[dwRect].y1 = 0;
        if (pRects[dwRect].y2 > maxSize) pRects[dwRect].y2 = maxSize;
        if (pRects[dwRect].y2 < 0)       pRects[dwRect].y2 = 0;
    }
}

//---------------------------------------------------------------------------

// DX6 callback for clearing render target, z-buffer and stencil buffer.

DWORD __stdcall nvClear2
(
    LPD3DHAL_CLEAR2DATA pc2d
)
{
    PNVD3DCONTEXT   pContext;
    BOOL            bResetSwizzleTag = FALSE;
    BOOL            bHaveStencil;
    DWORD           dwClear2Flags;
    DWORD           dwFastClearFlags;
    DWORD           dwFillColor = 0;
    DWORD           dwFillDepth = 0;
    DWORD           dwFillStencil = 0;
    DWORD           dwFastFillDepth;
    __int64         i64FillDepth;
    LPD3DRECT       pRects;
    DWORD           dwNumRects;
    CSimpleSurface *pRenderTarget;
    CSimpleSurface *pZetaBuffer;

    dbgTracePush ("nvClear2");

#ifdef NV_NULL_HW_DRIVER
    pc2d->ddrval = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
#endif

    // get the context
    NV_SET_CONTEXT (pContext, pc2d);
    nvSetDriverDataPtrFromContext (pContext);

    nvValidateClearRects(pc2d->lpRects, pc2d->dwNumRects);

    // force AA enabled if registry said so
#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        pContext->kelvinAA.TestCreation(pContext);
    } else
#endif
#if (NVARCH >= 0x010)
     if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASENABLE_MASK) // aa enabled
        && ((pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS] != D3DANTIALIAS_NONE)
        || (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASFORCEENABLE_MASK))) // aa forced
        {
            // create aa buffers
            if (nvCelsiusAACreate(pContext)) {
                // enable AA
                pContext->aa.setFlags (AASTATE::FLAG_ENABLED);
            }

        }
    }
#endif  // NVARCH >= 0x010

#if (NVARCH >= 0x020)
    pRenderTarget = pContext->kelvinAA.GetCurrentRT(pContext);
    pZetaBuffer = pContext->kelvinAA.GetCurrentZB(pContext);

    CKelvinAAState::AccessType RTAccess, ZBAccess;
 
    if (pc2d->dwNumRects == 1) {
        D3DRECT r = pc2d->lpRects[0];
 
        RTAccess =
            ((pc2d->dwFlags & D3DCLEAR_TARGET) && 
            pContext->pRenderTarget && 
            ((r.x2-r.x1) == (int)pContext->pRenderTarget->getWidth()) &&
            ((r.y2-r.y1) == (int)pContext->pRenderTarget->getHeight())) ?
            CKelvinAAState::ACCESS_WRITE_DISCARD :
            CKelvinAAState::ACCESS_WRITE;
 
        ZBAccess =
            ((pc2d->dwFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) && 
            pContext->pZetaBuffer && 
            ((r.x2-r.x1) == (int)pContext->pZetaBuffer->getWidth()) &&
            ((r.y2-r.y1) == (int)pContext->pZetaBuffer->getHeight())) ?
            CKelvinAAState::ACCESS_WRITE_DISCARD :
            CKelvinAAState::ACCESS_WRITE;
    } else {
        RTAccess = CKelvinAAState::ACCESS_WRITE;
        ZBAccess = CKelvinAAState::ACCESS_WRITE;
    }
 
    if ((pRenderTarget != pContext->pRenderTarget) && (pc2d->dwFlags & D3DCLEAR_TARGET)) {
        pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_SRT, RTAccess);
    }
    if ((pZetaBuffer != pContext->pZetaBuffer) && (pc2d->dwFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL))) {
        pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_SZB, ZBAccess);
    }
#else
    pRenderTarget = pContext->pRenderTarget;
    pZetaBuffer = pContext->pZetaBuffer;
#endif

    if ((!pRenderTarget) || getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        pc2d->ddrval = DD_OK;
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return(DDHAL_DRIVER_HANDLED);
    }

    nvSetD3DSurfaceState (pContext);

    if (global.dwDXRuntimeVersion < 0x700) {
        nvPusherSignalWaitForFlip (pRenderTarget->getfpVidMem(), DDSCAPS_PRIMARYSURFACE);
    }

    // assume things are OK unless we find out otherwise
    pc2d->ddrval = DD_OK;

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    DDSTARTTICK(DEVICE_CLEARALL);

    // Need to make sure that an unfriendly mode switch didn't sneak and not cause
    // us to get re-enabled properly.

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            DDENDTICK(DEVICE_CLEARALL);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    bHaveStencil = nvStencilBufferExists (pContext);

    // sanity check the flags
    dwClear2Flags  = pc2d->dwFlags & D3DCLEAR_TARGET;
    dwClear2Flags |= pZetaBuffer  ? (pc2d->dwFlags & D3DCLEAR_ZBUFFER) : 0;
    dwClear2Flags |= bHaveStencil ? (pc2d->dwFlags & D3DCLEAR_STENCIL) : 0;

    if (pContext->dwRTHandle)
    {
        CNvObject *pRTObj = nvGetObjectFromHandle(pContext->dwDDLclID, pContext->dwRTHandle, &global.pNvSurfaceLists);

        if (pRTObj) {

            // check if render target is a texture
            CTexture *pTexture = pTexture = pRTObj->getTexture();

            if (pTexture) {
                // if we're using the swizzled surface, we could have problems
                if (pContext->pRenderTarget != pTexture->getLinear()) {
                    // check if there are any sub-rects on the list
                    for (DWORD i = 0; i < pc2d->dwNumRects; i++) {
                        if ((pc2d->lpRects[i].x1 | pc2d->lpRects[i].y1)
                         || (pc2d->lpRects[i].y2 != (LONG)pTexture->getHeight())
                         || (pc2d->lpRects[i].x2 != (LONG)pTexture->getWidth())) {
                            break;
                        }
                    }
                    // if we have partial clears or if we're trying to clear z
                    // with mis-matched buffer sizes, resort to linear
                    if ( (i < pc2d->dwNumRects)
                         ||
                         ( (dwClear2Flags & D3DCLEAR_ZBUFFER)
                           &&
                           ((pZetaBuffer->getWidth() > pTexture->getWidth()) ||
                            (pZetaBuffer->getPitch() > (pTexture->getWidth() * pTexture->getBPP())))
                         )
                       )
                    {
                        if (!pTexture->prepareLinearAsRenderTarget()) {
                            // could not create linear surface
                            nvAssert(0);
                        }
                        // set linear surface as new render target
                        pDriverData->bDirtyRenderTarget = TRUE;
                        pContext->pRenderTarget = pTexture->getLinear();
                        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_SURFACE;
                    }
                }
                // update flags
                if (pContext->pRenderTarget == pTexture->getLinear()) {
                    pTexture->getLinear()->tagUpToDate();    // mark linear surface we are about to clear as touched
                    pTexture->getSwizzled()->tagOutOfDate(); // mark linear surface as out of date
                }
                else {
                    pTexture->getSwizzled()->tagUpToDate();  // mark swizzled surface we are about to clear as touched
                    pTexture->getLinear()->tagOutOfDate();   // mark linear surface as out of date
                    if (getDC()->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                        // surface is actually swizzled, tag as linear to prevent kelvin invalid state error
                        pContext->pRenderTarget->tagAsLinear();
                        // set flag so that we can reset the true value later
                        bResetSwizzleTag = TRUE;
                        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_SURFACE;
                    }
                }
            }

            else {
                // not a texture
                // store the fill color for use by the logo clear function
                if (dwClear2Flags & D3DCLEAR_TARGET) {
                    pContext->dwLastFillColor = pc2d->dwFillColor;
                    if ((pc2d->lpRects[0].x1 | pc2d->lpRects[0].y1)
                     || (pc2d->lpRects[0].y2 != (LONG)pContext->pRenderTarget->getHeight())
                     || (pc2d->lpRects[0].x2 != (LONG)pContext->pRenderTarget->getWidth())) {
                        pContext->bClearLogo = TRUE;
                    }
                }
            }

        } // if (pRTObj)

    } // if (pContext->dwRTHandle)

    // convert the color to the right format
    if (dwClear2Flags & D3DCLEAR_TARGET) {
        switch (pRenderTarget->getFormat()) {
            case NV_SURFACE_FORMAT_X8R8G8B8:
            case NV_SURFACE_FORMAT_A8R8G8B8:
                dwFillColor = pc2d->dwFillColor;
                break;
            case NV_SURFACE_FORMAT_R5G6B5:
                dwFillColor = ((pc2d->dwFillColor & 0x00F80000) >> 8)
                            | ((pc2d->dwFillColor & 0x0000FC00) >> 5)
                            | ((pc2d->dwFillColor & 0x000000F8) >> 3);
                break;
            case NV_SURFACE_FORMAT_X1R5G5B5:
                dwFillColor = ((pc2d->dwFillColor & 0x00F80000) >> 9)
                            | ((pc2d->dwFillColor & 0x0000F800) >> 6)
                            | ((pc2d->dwFillColor & 0x000000F8) >> 3);
                break;
            default:
                DPF ("unhandled surface format in clear2");
                dbgD3DError();
                dwFillColor = pc2d->dwFillColor;
                break;
        } // switch
    }

    // convert the depth fill to an appropriate fixed-point value
    if (dwClear2Flags & D3DCLEAR_ZBUFFER) {
        if (pZetaBuffer->getBPP() == 2) {
            i64FillDepth = (__int64)((double)pc2d->dvFillDepth * (double)65535.0f);   // convert to 16-bit number
            i64FillDepth = (i64FillDepth > 0xffff) ? 0xffff : i64FillDepth;
            i64FillDepth = (i64FillDepth < 0)      ? 0      : i64FillDepth;
            dwFillDepth  = (DWORD)i64FillDepth;
        }
        else {
            assert (pZetaBuffer->getBPP() == 4);
            i64FillDepth = (__int64)((double)pc2d->dvFillDepth * (double)16777215.0); // convert to 24-bit number
            i64FillDepth = (i64FillDepth > 0xffffff) ? 0xffffff : i64FillDepth;
            i64FillDepth = (i64FillDepth < 0)        ? 0        : i64FillDepth;
            dwFillDepth  = (DWORD)i64FillDepth << 8;  // the z-buffer occupies the upper 3 bytes.
        }
    }

    // if we're clearing stencil, get the fill value and cache it off
    if (dwClear2Flags & D3DCLEAR_STENCIL) {
        dwFillStencil = pc2d->dwFillStencil & 0xff;
        pContext->dwStencilFill = dwFillStencil;
        // i think we could potentially also do something like this if it would help any apps...
        // pContext->bStencilEnabled = FALSE; (rename Enabled => Valid)
        // pContext->hwState.celsius.dwDirtyBits |= CELSIUS_DIRTY_MISC; || NV_FORCE_TRI_SETUP
    }

#if (NVARCH >= 0x020) && defined (KELVIN_CLEAR)

    if (getDC()->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {

        DWORD   dwEnable;
        DWORD   dwLeft, dwBandLeft;
        DWORD   dwRight, dwBandRight;
        DWORD   dwWidth,dwHeight;
        D3DRECT d3dRect;

        // if no stencil exists, clear the stencil buffer for NV20
        // it's actually faster to clear the stencil buffer every time, than to only clear it one time.
        if (!bHaveStencil && (dwClear2Flags & D3DCLEAR_ZBUFFER)) {
            if (pZetaBuffer->getBPP() == 4) {
                dwClear2Flags |= D3DCLEAR_STENCIL;
                dwFillStencil = 0;
                pContext->dwStencilFill = dwFillStencil;
            }
        }

        nvSetKelvinClearState (pContext);

        if (bResetSwizzleTag) {
            // reset swizzled render target flag
            pContext->pRenderTarget->tagAsSwizzled();
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_SURFACE;
            // temporarily disable z-compression
            pContext->hwState.kelvin.set1 (NV097_SET_COMPRESS_ZBUFFER_EN, NV097_SET_COMPRESS_ZBUFFER_EN_V_DISABLE);
        }

        // set the clear values
        pContext->hwState.kelvin.set2 (NV097_SET_ZSTENCIL_CLEAR_VALUE,
                                       dwFillDepth | dwFillStencil,
                                       dwFillColor);

        // figure out which surfaces to clear
        dwEnable = (dwClear2Flags & D3DCLEAR_TARGET) ?
                   (DRF_DEF (097, _CLEAR_SURFACE, _R, _ENABLE) |
                    DRF_DEF (097, _CLEAR_SURFACE, _G, _ENABLE) |
                    DRF_DEF (097, _CLEAR_SURFACE, _B, _ENABLE) |
                    DRF_DEF (097, _CLEAR_SURFACE, _A, _ENABLE)) :
                   (DRF_DEF (097, _CLEAR_SURFACE, _R, _DISABLE) |
                    DRF_DEF (097, _CLEAR_SURFACE, _G, _DISABLE) |
                    DRF_DEF (097, _CLEAR_SURFACE, _B, _DISABLE) |
                    DRF_DEF (097, _CLEAR_SURFACE, _A, _DISABLE));

        dwEnable |= DRF_NUM (097, _CLEAR_SURFACE, _Z,       ((dwClear2Flags & D3DCLEAR_ZBUFFER) ? TRUE : FALSE));
        dwEnable |= DRF_NUM (097, _CLEAR_SURFACE, _STENCIL, ((dwClear2Flags & D3DCLEAR_STENCIL) ? TRUE : FALSE));

        // lock surfaces
        if (dwClear2Flags & D3DCLEAR_TARGET) {
            pRenderTarget->hwLock (CSimpleSurface::LOCK_NORMAL);
        }
        if (dwClear2Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) {
            nvAssert (pZetaBuffer);
            pZetaBuffer->hwLock (CSimpleSurface::LOCK_NORMAL);
        }

        // iterate through the rectangles
        pRects     = pc2d->lpRects;
        dwNumRects = pc2d->dwNumRects;

        while (dwNumRects) {

            d3dRect.x1 = (DWORD)(0.5 + pRects->x1*pContext->aa.fWidthAmplifier);
            d3dRect.y1 = (DWORD)(0.5 + pRects->y1*pContext->aa.fHeightAmplifier);
            d3dRect.x2 = (DWORD)(0.5 + pRects->x2*pContext->aa.fWidthAmplifier);
            d3dRect.y2 = (DWORD)(0.5 + pRects->y2*pContext->aa.fHeightAmplifier);

            dwWidth = d3dRect.x2 - d3dRect.x1;
            dwHeight = d3dRect.y2 - d3dRect.y1;
            if (dwWidth && dwHeight) //if there's work to do
            {
            // clear in <=256-byte aligned vertical bands
            if (dwWidth >= KELVIN_TETRIS_THRESHOLD) {

                dwBandLeft = 0;
                while (dwBandLeft < (DWORD)(d3dRect.x2)) {
                    // setup band
                    dwBandRight = dwBandLeft + KELVIN_TETRIS_WIDTH;
                    dwLeft      = max (dwBandLeft,  (DWORD)d3dRect.x1);
                    dwRight     = min (dwBandRight, (DWORD)d3dRect.x2);
                    if (dwRight > dwLeft) {
                        // clear the current sub-rectangle. note that our rects are [a,b) whereas kelvin's are [a,b]
                        pContext->hwState.kelvin.set2 (NV097_SET_CLEAR_RECT_HORIZONTAL,
                                                       (DRF_NUM (097, _SET_CLEAR_RECT_HORIZONTAL, _XMIN, dwLeft) |
                                                        DRF_NUM (097, _SET_CLEAR_RECT_HORIZONTAL, _XMAX, dwRight-1)),
                                                       (DRF_NUM (097, _SET_CLEAR_RECT_VERTICAL,   _YMIN, d3dRect.y1) |
                                                        DRF_NUM (097, _SET_CLEAR_RECT_VERTICAL,   _YMAX, d3dRect.y2-1)));
                        pContext->hwState.kelvin.set1 (NV097_CLEAR_SURFACE, dwEnable);
                    }
                    dwBandLeft = dwBandRight;
                }
            }

            else {
                // clear the whole rectangle at once. note that MS's rects are [a,b) whereas kelvin's are [a,b]
                pContext->hwState.kelvin.set2 (NV097_SET_CLEAR_RECT_HORIZONTAL,
                                               (DRF_NUM (097, _SET_CLEAR_RECT_HORIZONTAL, _XMIN, d3dRect.x1) |
                                                DRF_NUM (097, _SET_CLEAR_RECT_HORIZONTAL, _XMAX, d3dRect.x2-1)),
                                               (DRF_NUM (097, _SET_CLEAR_RECT_VERTICAL,   _YMIN, d3dRect.y1) |
                                                DRF_NUM (097, _SET_CLEAR_RECT_VERTICAL,   _YMAX, d3dRect.y2-1)));
                pContext->hwState.kelvin.set1 (NV097_CLEAR_SURFACE, dwEnable);
            }
            }
            pRects++;
            dwNumRects--;

        }

        // unlock
        if (dwClear2Flags & D3DCLEAR_TARGET) {
            pRenderTarget->hwUnlock();
        }
        if (dwClear2Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) {
            nvAssert (pZetaBuffer);
            pZetaBuffer->hwUnlock();
        }

        if (bResetSwizzleTag) {
#ifndef DISABLE_Z_COMPR
            // enable z-compression again (unless disabled in registry)
            if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ZCOMPRESSENABLE_MASK) == D3D_REG_ZCOMPRESSENABLE_ENABLE) {
                pContext->hwState.kelvin.set1 (NV097_SET_COMPRESS_ZBUFFER_EN, NV097_SET_COMPRESS_ZBUFFER_EN_V_ENABLE);
            }
#endif
        }

        DDENDTICK(DEVICE_CLEARALL);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);

    }

#endif  // (NVARCH >= 0x020) && defined (KELVIN_CLEAR)

    // figure out what we can fast clear...
    dwFastClearFlags = dwClear2Flags & D3DCLEAR_TARGET;  // we can always fast clear the target

    if (!bHaveStencil) {
        // if there's no stencil buffer, then we can fast clear the z-buffer
        dwFastClearFlags |= dwClear2Flags & D3DCLEAR_ZBUFFER;
        dwFastFillDepth   = dwFillDepth;
    }
    else {
        // if there is a stencil buffer, we can fast clear it and the
        // z-buffer together, but not independantly. we can clear them both if:
        // a. the user has requested both -or-
        // b. the user has requested only z, but the stencil buffer is unused (or clear)
        if ((dwClear2Flags & D3DCLEAR_ZBUFFER) && (dwClear2Flags & D3DCLEAR_STENCIL)) {
            dwFastClearFlags |= D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
            dwFastFillDepth   = dwFillDepth | dwFillStencil;
        }
        else if ((dwClear2Flags & D3DCLEAR_ZBUFFER) && (!pContext->bStencilEnabled)) {
            dwFastClearFlags |= D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
            dwFastFillDepth   = dwFillDepth | pContext->dwStencilFill;
        }
    }

    // if there's anything to fast clear, go ahead and do it
    if (dwFastClearFlags) {

        D3DHAL_CLEARDATA cd;

        cd.dwhContext  = pc2d->dwhContext;
        cd.dwFlags     = dwFastClearFlags;
        cd.dwFillColor = pc2d->dwFillColor;
        cd.dwFillDepth = dwFastFillDepth;
        cd.lpRects     = pc2d->lpRects;
        cd.dwNumRects  = pc2d->dwNumRects;

        nvClear (&cd);

        pc2d->ddrval = cd.ddrval;

    }

    // see if there's anything left to do. if not, just return.
    dwClear2Flags &= ~dwFastClearFlags;

    if (!dwClear2Flags) {
        DDENDTICK(DEVICE_CLEARALL);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    pRects     = pc2d->lpRects;
    dwNumRects = pc2d->dwNumRects;

#if (NVARCH >= 0x010)
    // handle AA clear
    if (pContext->aa.isEnabled()) {
        // select large buffer (without magnify)
        nvCelsiusAASelectSuperBuffers (pContext);
    }
#endif  // NVARCH >= 0x010

    // at this point, we must have a stencil buffer and be wanting to clear
    // only it OR the z-buffer, but not both.
    nvAssert (bHaveStencil);
    nvAssert (((dwClear2Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) == D3DCLEAR_ZBUFFER) ||
              ((dwClear2Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) == D3DCLEAR_STENCIL));

    // get active zeta buffer
#if (NVARCH >= 0x010)
    if (pContext->aa.isEnabled()) {
        pZetaBuffer = pContext->aa.pSuperZetaBuffer;
    } else
#endif  // NVARCH < 0x010
    {
        pZetaBuffer = pContext->pZetaBuffer;
    }

    DWORD dwPitch  = pRenderTarget->isSwizzled() ?
                     pRenderTarget->getPitch() : pZetaBuffer->getPitch();

    //More D3D Synchronization with clears
    pZetaBuffer->hwLock(CSimpleSurface::LOCK_NORMAL);

    if (dwClear2Flags & D3DCLEAR_ZBUFFER) {
        // we need to clear the z-buffer without stomping on the stencil buffer
        DWORD dwFillDepth0, dwFillDepth1, dwFillDepth2;

        dwFillDepth0 = (dwFillDepth >>  8) & 0xff;
        dwFillDepth1 = (dwFillDepth >> 16) & 0xff;
        dwFillDepth2 = (dwFillDepth >> 24) & 0xff;

        if ((dwFillDepth0 == dwFillDepth1) && (dwFillDepth1 == dwFillDepth2)) {
            // if all three bytes to be written are the same, we can do this with byte masks.
            nvClearSurfaceWithRop (pContext,
#ifndef STEREO_SUPPORT
                                   pZetaBuffer->getOffset(),
#else   //STEREO_SUPPORT==1
                                   GetStereoOffset(pZetaBuffer),
#endif  //STEREO_SUPPORT
                                   dwPitch,
                                   pRects, dwNumRects,
                                   8,                       // clear byte by byte
                                   0x000000cc,              // select src. write enable contingent on pattern's alpha
                                   0x00000000, 0x0000ff00,  // X16A8Y8. a=00 to kill the write, a=ff to enable it
                                   0xeeeeeeee,              // write,write,write,kill repeat
                                   dwFillDepth0);           // X24Y8
        }
        else {
            // we have to do a read-modify-write
            nvClearSurfaceWithRop (pContext,
#ifndef STEREO_SUPPORT
                                   pZetaBuffer->getOffset(),
#else   //STEREO_SUPPORT==1
                                   GetStereoOffset(pZetaBuffer),
#endif  //STEREO_SUPPORT
                                   dwPitch,
                                   pRects, dwNumRects,
                                   32,                      // clear by DWORDs
                                   0x000000ca,              // pattern ? src : dest
                                   0xffffff00, 0x00000000,  // Y32. color0 => write 24 bits, skip 8
                                   0x00000000,              // we always just select the first pattern (color0)
                                   dwFillDepth);            // Y32
        }
    }

    else {
        // we need to clear the stencil buffer without stomping on the z-buffer.
        assert (dwClear2Flags & D3DCLEAR_STENCIL);
        nvClearSurfaceWithRop (pContext,
#ifndef STEREO_SUPPORT
                               pZetaBuffer->getOffset(),
#else   //STEREO_SUPPORT==1
                               GetStereoOffset(pZetaBuffer),
#endif  //STEREO_SUPPORT
                               dwPitch,
                               pRects, dwNumRects,
                               8,                       // clear byte by byte
                               0x000000cc,              // select src. write enable contingent on pattern's alpha
                               0x00000000, 0x0000ff00,  // X16A8Y8. a=00 to kill the write, a=ff to enable it
                               0x11111111,              // kill,kill,kill,write repeat
                               dwFillStencil);          // X24Y8
    }

    pZetaBuffer->hwUnlock();

    DDENDTICK(DEVICE_CLEARALL);
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

}  // nvClear2

//---------------------------------------------------------------------------

/*
 * nvCTDecodeBuffer
 *
 * decodes an optimized buffer
 */
void nvCTDecodeBuffer
(
    CNvObject *pNvObj,
    DWORD      dwAddr,
    DWORD      dwBPP,
    DWORD      dwPitch,
    DWORD      dwWidth,
    DWORD      dwHeight
)
{
    /*
     * setup
     */
    extern BYTE nvTexelScratchBuffer[8192]; // reuse nvswiz.c scratch buffer

    DWORD  dwWidthBytes = dwWidth * dwBPP;
    BYTE  *pMem;
    BOOL   bMem;
    DWORD  y;

    if (dwWidthBytes > sizeof(nvTexelScratchBuffer))
    {
        pMem = (unsigned char*)AllocIPM(dwWidthBytes);
        bMem = TRUE;
    }
    else
    {
        pMem = nvTexelScratchBuffer;
        bMem = FALSE;
    }

    /*
     * for all scan lines
     */
    for (y = 0; y < dwHeight; y++)
    {
        DWORD x;

        /*
         * read scan line
         */
        nvMemCopy ((DWORD)pMem,dwAddr,dwWidthBytes);

        /*
         * process line
         */
        if (pNvObj->isCTFront())
        {
            switch (dwBPP)
            {
                case 2:
                {
                    for (x = 0; x < dwWidth; x++)
                    {
                        ((WORD*)pMem)[x] = ((WORD*)pMem)[x] << 1;
                    }
                    break;
                }
                case 4:
                {
                    for (x = 0; x < dwWidth; x++)
                    {
                        ((DWORD*)pMem)[x] = ((DWORD*)pMem)[x] << 1;
                    }
                    break;
                }
            }
        }
        else
        {
            switch (dwBPP)
            {
                case 2:
                {
                    for (x = 0; x < dwWidth; x++)
                    {
                        ((WORD*)pMem)[x] = (~((WORD*)pMem)[x]) << 1;
                    }
                    break;
                }
                case 4:
                {
                    for (x = 0; x < dwWidth; x++)
                    {
                        ((DWORD*)pMem)[x] = (~((DWORD*)pMem)[x]) << 1;
                    }
                    break;
                }
            }
        }

        /*
         * write scan line
         */
        nvMemCopy (dwAddr,(DWORD)pMem,dwWidthBytes);

        /*
         * next
         */
        dwAddr += dwPitch;
    }

    /*
     * done
     */
    if (bMem)
    {
        FreeIPM (pMem);
    }
}

#endif  // NVARCH >= 0x04

