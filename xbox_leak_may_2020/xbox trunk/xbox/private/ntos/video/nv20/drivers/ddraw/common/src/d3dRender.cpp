/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: d3dRender.cpp                                                     *
*   The Direct3D rendering routines.                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*       Ben de Waal (bdw)           11/06/97 - Linear MipMap xxx            *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

/*
 * Dma Pushing loop stats.
 */
DWORD   statDmaSyncLoop      = 0;
DWORD   statDmaSyncOverflow  = 0;
DWORD   statDmaBusyLoop      = 0;
DWORD   statDmaBusyOverflow  = 0;
DWORD   statDmaFlushLoop     = 0;
DWORD   statDmaFlushOverflow = 0;

#ifndef WINNT  // these calls are obsolete on NT (DX7)
/*
 * --------------------------------------------------------------------------
 * nvRenderState
 *
 * LPD3DHAL_RENDERSTATEDATA
 * - Points to the instruction in the execute buffer.
 *
 * We get the count of states to set and then walk the execute buffer
 * handling each one at a time.
 * --------------------------------------------------------------------------
 */
DWORD __stdcall nvRenderState
(
    LPD3DHAL_RENDERSTATEDATA prd
)
{
    DWORD           i;
    LPBYTE          lpData;
    LPD3DSTATE      lpState;
    DWORD override;

    dbgTracePush ("nvRenderState");

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, prd);

    // Get pointer to global driver. data structure.
    nvSetDriverDataPtrFromContext(pContext);

    lpData = (LPBYTE)(((LPDDRAWI_DDRAWSURFACE_INT)prd->lpExeBuf)->lpLcl->lpGbl->fpVidMem);
    for (i = 0, lpState = (LPD3DSTATE)(lpData + prd->dwOffset);
         i < prd->dwCount; i++, lpState++)
    {
        DWORD rtype = (DWORD) lpState->drstRenderStateType;

        if (IS_OVERRIDE(rtype))
        {
            override = GET_OVERRIDE(rtype);
            if (((override - 1) >> DWORD_SHIFT) >= (MAX_STATE >> DWORD_SHIFT))
            {
                DPF("Invalid state override\n");
                dbgD3DError();
                continue;
            }

            if (lpState->dwArg[0])
            {
                DPF_LEVEL (NVDBG_LEVEL_INFO, "nvRenderState - setting override for state %08x", override);
                STATESET_SET(pContext->overrides, override);
            }
            else
            {
                DPF_LEVEL (NVDBG_LEVEL_INFO, "nvRenderState, clearing override for state %08x", override);
                STATESET_CLEAR(pContext->overrides, override);
            }
            DDTICKOFFSET(rtype, lpState->dwArg[0]);

            continue;
        }
        if (STATESET_ISSET(pContext->overrides, rtype))
        {
            DPF_LEVEL (NVDBG_LEVEL_INFO, "nvRenderState, state %08x is overridden, ignoring", rtype);
            DDTICKOFFSET(rtype, lpState->dwArg[0]);
            continue;
        }
        nvSetContextState (pContext, lpState->drstRenderStateType, lpState->dwArg[0], &prd->ddrval);
        if (prd->ddrval != DD_OK)
        {
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

    }

    NvReleaseSemaphore(pDriverData);
    prd->ddrval = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}
#endif  // !WINNT

//---------------------------------------------------------------------------

DWORD __stdcall nvSetRenderTarget
(
    LPD3DHAL_SETRENDERTARGETDATA pRTData
)
{
    LPDDRAWI_DDRAWSURFACE_INT pDDS, pDDSZ;
    LPDDRAWI_DDRAWSURFACE_LCL lpDDSLcl, lpDDSLclZ;
    DWORD                     dwTargetHandle, dwZHandle;
    DWORD                     dwErrorReturnValue;

    dbgTracePush ("nvSetRenderTarget");

    // this now becomes a wrapper for the DX7 version.
    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pRTData);

    // Select the appropriate return value.
    // Normally, DDHAL_DRIVER_HANDLED would be returned from this routine when
    // an error is encountered during processing.  However, DX6 has a bug in which
    // it only returns the pRTData->ddrval error value to the application if
    // DDHAL_DRIVER_NOTHANDLED is returned from SetRenderTarget.
    // Based on the registry setting, return the desired value when an error occurs.
    if (getDC()->nvD3DRegistryData.regValidateZMethod == D3D_REG_VALIDATEZMETHOD_SRTRETURNNOTHANDLED) {
        dwErrorReturnValue = DDHAL_DRIVER_NOTHANDLED;
    }
    else {
        dwErrorReturnValue = DDHAL_DRIVER_HANDLED;
    }

    pDDS = (LPDDRAWI_DDRAWSURFACE_INT)pRTData->lpDDS;
    if (!pDDS) {
        dbgD3DError();
        pRTData->ddrval = DDERR_CURRENTLYNOTAVAIL;
        dbgTracePop();
        return (dwErrorReturnValue);
    }
    lpDDSLcl = pDDS->lpLcl;
    if (!lpDDSLcl) {
        dbgD3DError();
        pRTData->ddrval = DDERR_CURRENTLYNOTAVAIL;
        dbgTracePop();
        return (dwErrorReturnValue);
    }

    dwTargetHandle = (DWORD)lpDDSLcl;

    // get the zbuffer handle if we've got one
    if ((pDDSZ = (LPDDRAWI_DDRAWSURFACE_INT)pRTData->lpDDSZ) != NULL) {
        lpDDSLclZ = pDDSZ->lpLcl;
    }
    else {
        lpDDSLclZ = NULL;
    }
    dwZHandle = (DWORD)lpDDSLclZ;

    // update the associated NvObjects back-pointers since the runtime
    // may have moved nvObject pointers around amongst DDSLcls.
    CNvObject *pNvObj;
    pNvObj = GET_PNVOBJ (lpDDSLcl);
    pNvObj->setHandle ((DWORD)lpDDSLcl);
    if (lpDDSLclZ) {
        pNvObj = GET_PNVOBJ (lpDDSLclZ);
        pNvObj->setHandle ((DWORD)lpDDSLclZ);
    }

    pRTData->ddrval = nvSetRenderTargetDX7 (pContext, dwTargetHandle, dwZHandle);

    if (pRTData->ddrval != DD_OK) {
        dbgTracePop();
        return (dwErrorReturnValue);
    }

    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//---------------------------------------------------------------------------

BOOL nvAdjustZBuffer (CSimpleSurface *pZetaBuffer, LPDDRAWI_DDRAWSURFACE_LCL pDDSLclZ, DWORD dwNewBPP)
{
    if (pZetaBuffer->recreate (dwNewBPP)) {
        // successful allocation. reset some stuff in MS land
        if (pDDSLclZ) {
            pDDSLclZ->lpGbl->fpVidMem                      = pZetaBuffer->getfpVidMem();
            pDDSLclZ->lpGbl->lPitch                        = pZetaBuffer->getPitch();
            pDDSLclZ->lpGbl->lSlicePitch                   = pZetaBuffer->getSlicePitch();
            pDDSLclZ->lpGbl->ddpfSurface.dwZBufferBitDepth = dwNewBPP << 3;
            pDDSLclZ->lpGbl->ddpfSurface.dwZBitMask        = (dwNewBPP==2) ? 0x0000ffff : 0xffffff00;
        }
    }
    else {
        // re-allocation failed. now things are really bad.
        DPF ("nvCheckBufferCompatibility: z-buffer reallocation failed");
        nvAssert(0);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------

// check that we can actually render with this combination of z-buffer and
// render target. if not, try to fix things up for pre-DX7 apps.
// returns TRUE if everything is OK, FALSE otherwise

BOOL nvCheckBufferCompatibility (CSimpleSurface *pRenderTarget, CSimpleSurface *pZetaBuffer,
                                 LPDDRAWI_DDRAWSURFACE_LCL pDDSLclZ, DWORD dwDXAppVersion)
{
    if ((pZetaBuffer == NULL) || (pRenderTarget == NULL)) {
        // non-existent buffers can't conflict
        return TRUE;
    }

    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0097_KELVIN) {
        // with kelvin, all combinations of bit depths are ok
    }

    else if (dwDXAppVersion >= 0x800) { // !kelvin && DX8

        // DX8 apps convert Z if necessary
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_1196_CELSIUS) {
            // NV 11 supports C32/Z16
            if (pZetaBuffer->getBPP() > pRenderTarget->getBPP()) {
                DPF ("nvCheckBufferCompatibility: downgrading NV11 depth buffer for DX8 app");
                if (!nvAdjustZBuffer(pZetaBuffer, pDDSLclZ, pRenderTarget->getBPP())) {
                    return FALSE;
                }
            }
        }
        else if (pZetaBuffer->getBPP() != pRenderTarget->getBPP()) {
            // !NV11
            DPF ("nvCheckBufferCompatibility: forcing congruent bit depths for DX8 app");
            nvAdjustZBuffer(pZetaBuffer, pDDSLclZ, pRenderTarget->getBPP());
        }

    }

    else if (dwDXAppVersion >= 0x700) { // !kelvin && dx7

        // DX7 apps are expected to be compliant
        if ((pZetaBuffer->getBPPRequested() > pRenderTarget->getBPPRequested())
            ||
            ((!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_1196_CELSIUS)) &&
            (pZetaBuffer->getBPP() != pRenderTarget->getBPP())))
        {
            DPF ("nvCheckBufferCompatibility: mixed bit depths are disallowed for dx7");
            nvAssert(0);
            return FALSE;
        }

    }

    else { // !kelvin && DX6 or earlier

        // we try to cater to legacy apps no matter what they requested
        if ((pZetaBuffer->getBPP() > pRenderTarget->getBPP())
            ||
            ((!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_1196_CELSIUS)) &&
            (pZetaBuffer->getBPP() != pRenderTarget->getBPP())))
        {
            DPF ("nvCheckBufferCompatibility: forcing congruent bit depths for legacy app");
            if (!nvAdjustZBuffer(pZetaBuffer, pDDSLclZ, pRenderTarget->getBPP())) {
                return FALSE;
            }
        }

    }

    // check that the z-buffer is big enough
    if ((pRenderTarget->getHeight() > pZetaBuffer->getHeight()) ||
        (pRenderTarget->getWidth() > pZetaBuffer->getWidth())) {
        DPF ("nvCheckBufferCompatibility: z-buffer is insufficiently large");
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------

DWORD nvSetRenderTargetDX7
(
    PNVD3DCONTEXT pContext,
    DWORD dwTargetHandle,
    DWORD dwZBufferHandle
)
{
    CNvObject       *pObj, *pZObj;
    CSimpleSurface  *pSurface, *pZSurface;
    CTexture        *pTexture;
    DWORD            dwClass;
    BOOL             bRTChanged;

    dbgTracePush ("nvSetRenderTargetDX7 (rt = %d, zb = %d)", dwTargetHandle, dwZBufferHandle);

    // Set pointer to global driver data structure.
    nvSetDriverDataPtrFromContext (pContext);

    // get the z surface
    if (dwZBufferHandle) {
        pZObj = nvGetObjectFromHandle (pContext->dwDDLclID, dwZBufferHandle, &global.pNvSurfaceLists);
        pZSurface = pZObj ? pZObj->getSimpleSurface() : NULL;
    }
    else {
        pZSurface = NULL;
    }

    // set the render target ------------------------------------------------

#ifdef WINNT
    pObj = nvGetObjectFromHandle (pContext->dwDDLclID, dwTargetHandle, &global.pNvSurfaceLists);
#else // !WINNT
    // runtime bug in dx7 when running dx5 apps - we will get a handle of 0 here for the primary surface
    // the runtime is supposed to swap the surface handles internally
    pObj = dwTargetHandle ? nvGetObjectFromHandle (pContext->dwDDLclID, dwTargetHandle, &global.pNvSurfaceLists) : (CNvObject*)pDXShare->pNvPrimarySurf;
#endif // !WINNT
    dwClass = pObj ? pObj->getClass() : CNvObject::NVOBJ_UNKNOWN;

    switch (dwClass) {

        case CNvObject::NVOBJ_SIMPLESURFACE :

#if (NVARCH >= 0x010)
            if (pContext->aa.isSuspended()) {
                pContext->aa.modFlags (~AASTATE::FLAG_SUSPENDED,AASTATE::FLAG_ENABLED);
            }
#endif  // NVARCH >= 0x010
#if (NVARCH >= 0x020)
            pContext->kelvinAA.Resume();
#endif
            pSurface = pObj->getSimpleSurface();

#ifdef  STEREO_SUPPORT
            //Make sure the render target has the right eye.
            if (STEREO_ENABLED && !pSurface->isStereo())
            {
                if (!pSurface->createStereo()) {
                    LOG("nvSetRenderTargetDX7: Can't create a stereo surface");
                }
            }
#endif  //STEREO_SUPPORT

            // make sure this is something we can write to
            if (!pSurface->hwCanWrite()) {
                NvReleaseSemaphore(pDriverData);
                dbgD3DError();
                dbgTracePop();
                return (DDERR_CURRENTLYNOTAVAIL);
            }
            break;

        case CNvObject::NVOBJ_TEXTURE :

#if (NVARCH >= 0x010)
            if (pContext->aa.isEnabled())  {
                pContext->aa.modFlags (~AASTATE::FLAG_ENABLED,AASTATE::FLAG_SUSPENDED);
            }
#endif  // NVARCH >= 0x010
#if (NVARCH >= 0x020)
            pContext->kelvinAA.Suspend();
#endif

            pTexture = pObj->getTexture();

            // render to the linear copy if it's newer or if we have dissimilar bit
            // depths on NV11/NV20 which can't handle mixed swizzled rendering
            if ( (pTexture->getLinear()->isUpToDate() && !pTexture->getSwizzled()->isUpToDate())
                 ||
                 (pZSurface && (pZSurface->getBPP() != pTexture->getBPP()))       // and mixed
               )
            {
                pSurface = pTexture->getLinear();
                // make certain we have a video memory linear surface
                if (!pTexture->prepareLinearAsRenderTarget()) {
                    NvReleaseSemaphore(pDriverData);
                    dbgD3DError();
                    dbgTracePop();
                    return (DDERR_OUTOFVIDEOMEMORY);
                }
            }
            else
            {
                // use swizzled rendering in hardware
                pTexture->updateSwizzleSurface(pContext);
                pSurface = pTexture->getSwizzled();
                pSurface->setWrapper (pTexture->getWrapper());
                pSurface->setFormat (pTexture->getFormat());
                pSurface->setBPP (pTexture->getBPP());
                pSurface->setBPPRequested (pTexture->getBPPRequested());
                pSurface->setWidth (pTexture->getWidth());
                pSurface->setHeight (pTexture->getHeight());
                pSurface->setDepth (pTexture->getDepth());
                pSurface->setPitch (pTexture->getPitch());
                pSurface->setSlicePitch (pTexture->getSlicePitch());
                pSurface->tagAsSwizzled();
            }
            break;

        default:
            // no formatted surface available
            DPF("nvSetRenderTarget - NULL target");
            NvReleaseSemaphore(pDriverData);
            dbgD3DError();
            dbgTracePop();
            return (DDERR_CURRENTLYNOTAVAIL);
            break;

    } // switch

    // if required, make certain that the previous flip has completed
    nvPusherSignalWaitForFlip (pSurface->getfpVidMem(), DDSCAPS_PRIMARYSURFACE);

    // mark RT as up to date
    pObj->clearDirty();

    // set the z-buffer -----------------------------------------------------

    if (pZSurface) {
        // enter if AA is not enabled.  Note that AA may have been created but is suspended at the moment
        // due to rendering to a texture or some such.
        // if AA is on and not suspended, then we'll assume that everything works--after all, we created the Z buffer,
        // so we're pretty sure it's valid.
        if (!pContext->kelvinAA.IsEnabled()) {
            if (pContext->kelvinAA.IsCreated()) {
                // if kelvin AA is or was on, we may have destroyed the z-buffer. attempt to re-create it now
                if (!pZSurface->isValid()) {
                    pContext->kelvinAA.ReCreateZBuffer();
                }
            }

            // if the z-surface is still invalid or isn't something the HW can write, bail out now
            if (!pZSurface->isValid() || !pZSurface->hwCanWrite()) {
                NvReleaseSemaphore(pDriverData);
                dbgD3DError();
                dbgTracePop();
                return (DDERR_CURRENTLYNOTAVAIL);
            }
        }

        // disable CT if the z-buffer is being associated with a render target whose size
        // is different from that of the render target with which it was last associated
        DWORD dwLastW = pZObj->getContextualValue0();
        DWORD dwLastH = pZObj->getContextualValue1();
        if (((dwLastW != 0) && (dwLastW != pSurface->getWidth()))
            ||
            ((dwLastH != 0) && (dwLastH != pSurface->getHeight())))
        {
            pZObj->disableCT();
        }
        pZObj->setContextualValue0 (pSurface->getWidth());
        pZObj->setContextualValue1 (pSurface->getHeight());
        // check that the z-buffer is big enough
        if ((pSurface->getHeight() > pZSurface->getHeight()) ||
            (pSurface->getWidth() > pZSurface->getWidth()))
        {
            DPF("z-buffer disabled due to insufficient space");
            pZSurface = NULL;
            dwZBufferHandle = 0;
        }
    }

    else {
        // according to MS, (dwZBufferHandle == 0) => disable z-buffer
        pZSurface = NULL;
    }

    // check that we can render with this combination
    if (!nvCheckBufferCompatibility (pSurface, pZSurface, NULL, pContext->dwDXAppVersion)) {
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDERR_CURRENTLYNOTAVAIL);
    }

    // only now that all sanity checks have passed, update values in the context
    bRTChanged = (pContext->pRenderTarget != pSurface) ? TRUE : FALSE;
    pContext->dwRTHandle    = dwTargetHandle;
    pContext->dwZBHandle    = dwZBufferHandle;
    pContext->pRenderTarget = pSurface;
    pContext->pZetaBuffer   = pZSurface;

    dbgSetFrontRenderTarget (pContext, pSurface);

    // fix up AA if render target changed
    if (bRTChanged) {
        if (getDC()->dwAAContextCount >= 1) {
#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            pContext->kelvinAA.UpdateZBuffer();
            pContext->kelvinAA.UpdateRenderTarget();
        } else
#endif  // NVARCH >= 0x020
#if (NVARCH >= 0x010)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
            // Magnify the new render target into the super buffer
            if (pContext->aa.mAppCompat.bDiscardSRT){
//              pContext->aa.invalidateSRT();
                pContext->aa.makeSuperBufferValid(pContext);
            }
        }
#endif  // NVARCH >= 0x010
        }
    }

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_SURFACE   |
                                       CELSIUS_DIRTY_TRANSFORM |
                                       CELSIUS_DIRTY_SPECFOG_COMBINER);
    pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_SURFACE   |
                                       KELVIN_DIRTY_TRANSFORM |
                                       KELVIN_DIRTY_COMBINERS_SPECFOG);
#endif
    pDriverData->bDirtyRenderTarget = TRUE;

    nvSetD3DSurfaceState (pContext);

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        pContext->kelvinAA.TestCreation(pContext);
    } else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        pContext->aa.Create(pContext);
    }
#endif  // NVARCH >= 0x010

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DD_OK);
}
#endif  // NVARCH >= 0x04

