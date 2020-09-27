 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

/*==========================================================================;
 *
 *  Copyright (C) 1995, 1999 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       nvBlit.cpp
 *  Content:    Windows95 DirectDraw 32 bit driver
 *
 ***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

// Convert pusher threshold from word to safe dword value
const int g_iPusherThreshold = (NVPUSHER_THRESHOLD_SIZE >> 1) - 1;

//---------------------------------------------------------------------------
// GetBltStatus32
//      DX callback
DWORD __stdcall GetBltStatus32 (LPDDHAL_GETBLTSTATUSDATA lpGetBltStatus)
{
    dbgTracePush ("GetBltStatus32");

    DDSTARTTICK(SURF4_GETBLTSTATUS);
    nvSetDriverDataPtrFromDDGbl (lpGetBltStatus->lpDD);

    if ( lpGetBltStatus->dwFlags == DDGBS_CANBLT ) {

        // CANBLT: can we add a blt?

        // make sure that we've flipped away from the destination surface
        lpGetBltStatus->ddRVal = getFlipStatus(lpGetBltStatus->lpDDSurface->lpGbl->fpVidMem,
                                               lpGetBltStatus->lpDDSurface->ddsCaps.dwCaps);

        if ( lpGetBltStatus->ddRVal == DD_OK ) {
            if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
                if (!nvEnable32(lpGetBltStatus->lpDD)) {
                    NvReleaseSemaphore(pDriverData);
                    lpGetBltStatus->ddRVal = DDERR_SURFACELOST;
                    DDENDTICK(SURF4_GETBLTSTATUS);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
            }
            // so there was no flip going on, is there room in the fifo to add a blt?
            getDC()->nvPusher.setSyncChannelFlag();
            lpGetBltStatus->ddRVal = DD_OK;
        }
    }

    else {

        // DONEBLT: is a blt in progress?

        // Some apps call GetBltStatus many times after calling Blit even after they were told that it was done
        if (pDriverData->blitCalled == FALSE) {
            NvReleaseSemaphore(pDriverData);
            lpGetBltStatus->ddRVal = DD_OK;
            DDENDTICK(SURF4_GETBLTSTATUS);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
            if (!nvEnable32(lpGetBltStatus->lpDD)) {
                NvReleaseSemaphore(pDriverData);
                lpGetBltStatus->ddRVal = DDERR_SURFACELOST;
                DDENDTICK(SURF4_GETBLTSTATUS);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }
        }

        /* If a texture surface then just wait for notifier */
        if (lpGetBltStatus->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_TEXTURE) {

            /*
             * Only check hardware status if we actually use HW to do texture blts.
             */
/*
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_USEHW)
            {
                BOOL bBusy;
                CNvObject *pObj = GET_PNVOBJ(lpGetBltStatus->lpDDSurface);
                DWORD   dwClass = pObj ? pObj->getClass() : CNvObject::NVOBJ_UNKNOWN;

                switch (dwClass) {
                case CNvObject::NVOBJ_SIMPLESURFACE:
                    bBusy = lpGetBltStatus->ddRVal = pObj->getSimpleSurface()->isBusy();
                    break;
                case CNvObject::NVOBJ_TEXTURE:
                    bBusy = lpGetBltStatus->ddRVal = pObj->getTexture()->getSwizzled()->isBusy();
                    break;
                default:
                    bBusy = WaitForIdle(FALSE, FALSE);
                    break;
                } // switch

                if (bBusy) {
                    nvPusherStart(TRUE);
                    lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
                } else {
                    lpGetBltStatus->ddRVal = DD_OK;
                }
            }
            else
*/
            {
                lpGetBltStatus->ddRVal = DD_OK;
            }

            NvReleaseSemaphore(pDriverData);
            DDENDTICK(SURF4_GETBLTSTATUS);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        /* Is there a DMA push blit synchronization in progress ? */
        if (pDriverData->syncDmaRecord.bSyncFlag) { /* if so then check status */

            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                               pDriverData->bltData.dwStatusReferenceCount,
                                               0)) {
                    lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
                }
                else {
                    lpGetBltStatus->ddRVal = DD_OK;
                    pDriverData->syncDmaRecord.bSyncFlag = FALSE;
                    pDriverData->blitCalled = FALSE;
                }
            }
            else {
                if (getDC()->nvPusher.isIdle(TRUE)) {
                    pDriverData->syncDmaRecord.bSyncFlag = FALSE;
                    pDriverData->blitCalled = FALSE;
                    lpGetBltStatus->ddRVal = DD_OK;
                } else {
                    lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
                    // always make sure everything has been pushed off
                    nvPusherStart(TRUE);
                }
            }
        }
        else { /* start a blit synchronization sequence */
            pDriverData->syncDmaRecord.bSyncFlag = TRUE;

            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                pDriverData->bltData.dwStatusReferenceCount = getDC()->pRefCount->inc (CReferenceCount::INC_LAZY);
                nvPusherStart (TRUE);
                lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
            }
            else {
                if (getDC()->nvPusher.isIdle(TRUE)) {
                    pDriverData->syncDmaRecord.bSyncFlag = FALSE;
                    pDriverData->blitCalled = FALSE;
                    lpGetBltStatus->ddRVal = DD_OK;
                } else {
                    lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
                    // always make sure everything has been pushed off
                    nvPusherStart(TRUE);
                }
            }
        }
    }

    NvReleaseSemaphore(pDriverData);
    DDENDTICK(SURF4_GETBLTSTATUS);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

} // GetBltStatus32

//---------------------------------------------------------------------------

void nvCheckQueuedBlits (void)
{
    // max queued frames is regPreRenderLimit (defaults to 3 - PC99 spec)
    DWORD dwMaxQueuedBlits = getDC()->nvD3DRegistryData.regPreRenderLimit;

    // read HW blit #
    DWORD dwCompletedBlit = getDC()->pBlitTracker->get();

    // have we progressed too far?
    while ((getDC()->dwCurrentBlit - dwCompletedBlit) > dwMaxQueuedBlits)
    {
        // kick off buffer
        nvPusherStart (TRUE);
        // wait for HW to catch up
        nvDelay();
        dwCompletedBlit = getDC()->pBlitTracker->get();
    }
}

//---------------------------------------------------------------------------

void nvUpdateBlitTracker (void)
{
    // update frame counters
    getDC()->dwCurrentBlit ++;
    getDC()->pBlitTracker->put (getDC()->dwCurrentBlit);
    // kick off buffer
    nvPusherStart (TRUE);
}

//---------------------------------------------------------------------------
// bltInit
//      Initialize blit globals
void __stdcall bltInit(void)
{
    pDriverData->bltData.dwSystemSurfaceContextDMAIndex = 0;
    pDriverData->bltSysMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    pDriverData->bltAGPMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    pDriverData->bltVidMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    pDriverData->bltData.dwLastRop = 0xFFFFFFFF;
    pDriverData->bltData.dwLastColourKey = 0xFFFFFFF0;
    pDriverData->bltData.dwLastColourFormat = 0;
    pDriverData->bltData.dwLastCombinedPitch = 0;
    pDriverData->bltData.dwLastSrcOffset = 0xFFFFFFFF;
    pDriverData->bltData.dwLastDstOffset = 0xFFFFFFFF;
    pDriverData->bltData.dwSystemBltFallback = FALSE;
    pDriverData->bltData.dwLetItThrash = FALSE;
}

//---------------------------------------------------------------------------
// Blit32
//      DX callback.  Uber-function for calling all the blit styles
//
// Hints and rules of thumb
//      - bltSimpleCopy and blt4CCto4CC do not required Surfaces2D to be
//        initialized, all other blit functions do
//      - bltSimpleCopy can handle all combinations of sources and destinations
//        except for system memory to system memory copies
//      - bltSimpleVtoVBlt must have both source and destination is local
//        video memory
//      - Only bltSimpleVtoVBlt can handle colour keys and non-trivial ROPs
//      - bltStrBlt may return NOT_HANDLED
//      - bltStrBlt sources can be in local or system memory
//      - bltStrBlt destination must be in local video memory
//      - Error checks are required for workspace allocation
//      - When using an intermediate workspace, remember to set Surfaces2D
//      - Be careful about using workspaces for multiple things that can't
//        be done in place.  Color keyed S->V and 8 bpp strblts already use
//        local workspaces

#ifdef  STEREO_SUPPORT
#ifdef  PER_PRIMITIVE_SYNC
#define SyncToHw                                \
        StereoSync();               
#else   //PER_PRIMITIVE_SYNC==0
#define SyncToHw
#endif  //PER_PRIMITIVE_SYNC

#define CheckStereoSecondPass                   \
        if (--dwNPasses)                        \
            goto StereoSecondPass;
#else   //STEREO_SUPPORT==0
#define CheckStereoSecondPass
#define SyncToHw
#endif  //STEREO_SUPPORT

#define BLT_ABNORMAL_EXIT                       \
        {                                       \
            SyncToHw;                           \
            if (dst.bDefaultBlitChanged) {                                  \
                nvPushData(0, dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000); \
                nvPushData(1, NV_DD_IMAGE_BLIT);                            \
                nvPusherAdjust(2);                                          \
            }                                           \
        if (bRewriteSurfData) {                                   \
            dst.pGbl->fpVidMem = fpOldVidMem;                           \
            dst.pGbl->lPitch = lOldPitch;                               \
            dst.pGbl->wHeight = wOldHeight;                             \
                dst.pGbl->wWidth = wOldWidth;                               \
            }                                                             \
            CheckStereoSecondPass;              \
            DDENDTICK(SURF4_BLT);               \
            NvReleaseSemaphore(pDriverData);    \
            dbgTracePop();                      \
            return retVal;                      \
        }

#define BLT_EXIT                                                            \
    {                                                                       \
      SyncToHw;                                                             \
      if (dst.bDefaultBlitChanged) {                                        \
            nvPushData(0, dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000);     \
            nvPushData(1, NV_DD_IMAGE_BLIT);                                \
            nvPusherAdjust(2);                                              \
            nvPusherStart(TRUE);                                            \
        }                                                                   \
        if (bRewriteSurfData) {                             \
        dst.pGbl->fpVidMem = fpOldVidMem;                               \
        dst.pGbl->lPitch = lOldPitch;                                   \
        dst.pGbl->wHeight = wOldHeight;                                 \
            dst.pGbl->wWidth = wOldWidth;                                   \
        }                                                                   \
        if (dst.bIsLocked)                                                  \
        {                                                                   \
            if (dst.pObj->getClass()==CNvObject::NVOBJ_TEXTURE)             \
            {                                                               \
                if( dst.bUseSwizzle ) {                                     \
                    dst.pObj->getTexture()->getSwizzled()->hwUnlock();      \
                    dst.pObj->getTexture()->getSwizzled()->tagUpToDate();   \
                    dst.pObj->getTexture()->getLinear()->tagOutOfDate();    \
                } else {                                                    \
                    dst.pObj->getTexture()->getLinear()->hwUnlock();        \
                    dst.pObj->getTexture()->getLinear()->tagUpToDate();     \
                    dst.pObj->getTexture()->getSwizzled()->tagOutOfDate();  \
                }                                                           \
            } else                                                          \
                dst.pObj->getSimpleSurface()->hwUnlock();                   \
            nvPusherStart(TRUE);                                            \
            dst.bIsLocked = FALSE;                                          \
        }                                                                   \
        if (src.bIsLocked)                                                  \
        {                                                                   \
            if (src.pObj->getClass()==CNvObject::NVOBJ_TEXTURE)             \
            {                                                               \
                if( src.bUseSwizzle ) {                                     \
                    src.pObj->getTexture()->getSwizzled()->hwUnlock();      \
                    src.pObj->getTexture()->getSwizzled()->tagUpToDate();   \
                    src.pObj->getTexture()->getLinear()->tagOutOfDate();    \
                } else {                                                    \
                    src.pObj->getTexture()->getLinear()->hwUnlock();        \
                    src.pObj->getTexture()->getLinear()->tagUpToDate();     \
                    src.pObj->getTexture()->getSwizzled()->tagOutOfDate();  \
                }                                                           \
            } else                                                          \
                src.pObj->getSimpleSurface()->hwUnlock();                   \
            nvPusherStart(TRUE);                                            \
            src.bIsLocked = FALSE;                                          \
        }                                                                   \
        if (retVal == DDHAL_DRIVER_HANDLED) {                               \
            CheckStereoSecondPass;                                          \
            if (doFlushOnExit) {                                            \
                getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY); \
            }                                                               \
            DDENDTICK(SURF4_BLT);                                           \
            NvReleaseSemaphore(pDriverData);                                \
            dbgTracePop();                                                  \
            return retVal;                                                  \
        }                                                                   \
    }

//---------------------------------------------------------------------------

DWORD __stdcall Blit32( LPDDHAL_BLTDATA pbd )
{
    SURFINFO src, dst, ws;
    DWORD dwPbdFlags;
    DWORD retVal = DDHAL_DRIVER_HANDLED;
    DWORD dwBytesPerPixel;
    BOOL doStretchX, doStretchY, doStretch;
    BOOL doMirrorX, doMirrorY, doMirror;
    BOOL doFlushOnExit;
    BOOL bDoEarlyBlitFallback = FALSE;
    BOOL bStretchLinearFilter = TRUE;
    BOOL bRewriteSurfData = FALSE;
    BOOL bSrcBPPSpecified = FALSE;

    FLATPTR fpOldVidMem = NULL;
    LONG lOldPitch = 0;

#ifdef WINNT
    DWORD wOldHeight = 0;
    DWORD wOldWidth = 0;
#else
    WORD wOldHeight = 0;
    WORD wOldWidth = 0;
#endif

#ifdef  STEREO_SUPPORT
    STEREOBLTPASSINFO StereoBltPass;
    StereoBltPass.dwPass = 0;
    DWORD   dwNPasses = 1;
#endif  //STEREO_SUPPORT

    dbgTracePush ("Blit32");

    DDSTARTTICK(SURF4_BLT);

    nvSetDriverDataPtrFromDDGbl (pbd->lpDD);
    DWORD dwModeBPP = GET_MODE_BPP();

    dwPbdFlags = pbd->dwFlags;

    dwBytesPerPixel = (dwModeBPP + 1) >> 3;

    // force the blit wait flag if requested in the registry
    if (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_FORCEBLITWAITFLAGENABLE_ENABLE) {
        dwPbdFlags |= DDBLT_WAIT;
    }

    // force flush on exit if DDBLT_WAIT flag is set & registry key is enabled
    doFlushOnExit = (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_FLUSHAFTERBLITENABLE_ENABLE) &&
                    (dwPbdFlags & DDBLT_WAIT);

    // get commonly used surface data
    src.pLcl = pbd->lpDDSrcSurface;
    dst.pLcl = pbd->lpDDDestSurface;
    src.dwFourCC = dst.dwFourCC = 0;
    src.dwCaps = dst.dwCaps = 0;
    src.pObj = dst.pObj = NULL;
    src.pGbl = dst.pGbl = NULL;
    src.dwPitch = dst.dwPitch = 0;
    src.dwWidth = dst.dwWidth = 0;
    src.dwHeight = dst.dwHeight = 0;
    src.dwOffset = dst.dwOffset = 0;
    src.fpVidMem = dst.fpVidMem = 0;
    src.isPotentialFault = dst.isPotentialFault = FALSE;
    src.dwBytesPerPixel = dst.dwBytesPerPixel = dwBytesPerPixel;
    src.bUseSwizzle = dst.bUseSwizzle = FALSE;
    src.bIsLocked = dst.bIsLocked = FALSE;
    src.bDefaultBlitChanged = dst.bDefaultBlitChanged = FALSE;

    if(dst.pLcl){
        //have to set this BEFORE we do the
        //becuase it is used to determing the
        //ContextDMA optimization while
        //seting up the src object.
        dst.dwCaps = dst.pLcl->ddsCaps.dwCaps;
    }

    // source info
    if (src.pLcl) {
        src.pObj = GET_PNVOBJ(src.pLcl);
        src.dwCaps = src.pLcl->ddsCaps.dwCaps;
        src.pGbl = src.pLcl->lpGbl;
        src.dwSubRectX = pbd->rSrc.left;
        src.dwSubRectY = pbd->rSrc.top;
        src.dwSubRectWidth = pbd->rSrc.right - src.dwSubRectX;
        src.dwSubRectHeight = pbd->rSrc.bottom - src.dwSubRectY;
        if (src.pGbl) {
            src.fpVidMem = src.pGbl->fpVidMem;
            src.dwPitch = src.pGbl->lPitch;
            src.dwWidth = src.pGbl->wWidth;
            src.dwHeight = src.pGbl->wHeight;
            if (src.pLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT) {
                bSrcBPPSpecified = TRUE;
                if (src.pGbl->ddpfSurface.dwFlags & DDPF_FOURCC) {
                    src.dwFourCC = src.pGbl->ddpfSurface.dwFourCC;
                    src.dwBytesPerPixel = 2;
                } else {
                    src.dwBytesPerPixel = (src.pGbl->ddpfSurface.dwRGBBitCount + 1) >> 3;
                }
            }
            if (src.dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) {
                if ((src.dwCaps & DDSCAPS_TEXTURE) || (dst.dwCaps & DDSCAPS_TEXTURE)) {
                    // system or AGP memory and not a texture (D3D will handle this)
                    src.dwOffset = 0;
                    src.dwContextDma = 0;
                } else if (pDriverData->bltData.dwSystemBltFallback) {
                    src.dwContextDma = 0;
                    bDoEarlyBlitFallback = TRUE;
                } else {
#ifndef WINNT
                    // don't do this for WINNT because they can sometimes hand us an aliased linear address
                    // to an AGP surface which is nowhere near the GARTLinearBase that we know about
                    if ((src.dwCaps & DDSCAPS_NONLOCALVIDMEM) && pDriverData->GARTLinearBase) {
                        // AGP memory
                        src.dwContextDma = D3D_CONTEXT_DMA_HOST_MEMORY;
                        src.dwOffset = AGPMEM_OFFSET(src.pGbl->fpVidMem);
                    } else {
#endif
                        // system memory
                        src.dwOffset = 0;
                        retVal = bltCreateContextDMA(pbd, &src);
                        if (retVal != DDHAL_DRIVER_HANDLED) {
                            src.dwContextDma = 0;
                            bDoEarlyBlitFallback = TRUE;
                        }
#ifndef WINNT
                    }
#endif
                }
            } else {
                // video memory
                src.dwOffset = VIDMEM_OFFSET(src.pGbl->fpVidMem);
                src.dwContextDma = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
            }
        }
        // do this last, need the invalid one to create a context DMA
        src.pObj = IS_VALID_PNVOBJ(src.pObj) ? src.pObj : NULL;
    }

    // destination info
    if (dst.pLcl) {
        dst.pObj = GET_PNVOBJ(dst.pLcl);
        dst.dwCaps = dst.pLcl->ddsCaps.dwCaps;
        dst.pGbl = dst.pLcl->lpGbl;
        dst.dwSubRectX = pbd->rDest.left;
        dst.dwSubRectY = pbd->rDest.top;
        dst.dwSubRectWidth = pbd->rDest.right - dst.dwSubRectX;
        dst.dwSubRectHeight = pbd->rDest.bottom - dst.dwSubRectY;
        if (dst.pGbl) {
            dst.fpVidMem = dst.pGbl->fpVidMem;
            dst.dwPitch = dst.pGbl->lPitch;
            dst.dwWidth = dst.pGbl->wWidth;
            dst.dwHeight = dst.pGbl->wHeight;
            if (dst.pLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT) {
                if (dst.pGbl->ddpfSurface.dwFlags & DDPF_FOURCC) {
                    dst.dwFourCC = dst.pGbl->ddpfSurface.dwFourCC;
                    dst.dwBytesPerPixel = 2;
                } else {
                    dst.dwBytesPerPixel = (dst.pGbl->ddpfSurface.dwRGBBitCount + 1) >> 3;
                }
                if (!bSrcBPPSpecified) {
                    src.dwBytesPerPixel = dst.dwBytesPerPixel;
                }
            }
            if (dst.dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM) 
                && (dst.dwFourCC!=FOURCC_DXT1) && (dst.dwFourCC!=FOURCC_DXT2)  
                && (dst.dwFourCC!=FOURCC_DXT3) && (dst.dwFourCC!=FOURCC_DXT4) 
                && (dst.dwFourCC!=FOURCC_DXT5)) {
                // system or AGP memory
                if (pDriverData->bltData.dwSystemBltFallback) {
                    dst.dwContextDma = 0;
                    bDoEarlyBlitFallback = TRUE;
                } else {
#ifndef WINNT
                    // don't do this for WINNT because they can sometimes hand us an aliased linear address
                    // to an AGP surface which is nowhere near the GARTLinearBase that we know about
                    if ((dst.dwCaps & DDSCAPS_NONLOCALVIDMEM) && pDriverData->GARTLinearBase) {
                        // AGP memory
                        dst.dwContextDma = D3D_CONTEXT_DMA_HOST_MEMORY;
                        dst.dwOffset = AGPMEM_OFFSET(dst.pGbl->fpVidMem);
                    } else {
#endif
                        // system memory
                        dst.dwOffset = 0;
                        retVal = bltCreateContextDMA(pbd, &dst);
                        if (retVal != DDHAL_DRIVER_HANDLED) {
                            dst.dwContextDma = 0;
                            bDoEarlyBlitFallback = TRUE;
                        }
#ifndef WINNT
                    }
#endif
                }
            } else {
                // video memory
                dst.dwOffset = VIDMEM_OFFSET(dst.pGbl->fpVidMem);
                dst.dwContextDma = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
            }
        }
        // do this last, need the invalid one to create a context DMA
        dst.pObj = IS_VALID_PNVOBJ(dst.pObj) ? dst.pObj : NULL;
    }

    // update frame tracker if appropriate
    if ((dwPbdFlags & DDBLT_LAST_PRESENTATION)
        ||
        // DX7 - if registry key is set, allow at most "regPreRenderLimit" blits to primary with the
        // DDBLT_WAIT flag to queue up. this prevents horrible lag if we queue too many.
        ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_LIMITQUEUEDFBBLITSENABLE_ENABLE) &&
         (dst.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE))))
    {
        nvCheckQueuedBlits();
        nvUpdateBlitTracker();
    }

#ifdef  STEREO_SUPPORT
StereoSecondPass:
        dwNPasses = StereoBltAdjustParameters(src, dst, StereoBltPass);
#endif  //STEREO_SUPPORT

#if (NVARCH >= 0x010)

    // Handle AA
    PNVD3DCONTEXT pContext = NULL;

    if (src.pObj) {
        pContext = nvCelsiusAAFindContext(src.pObj->getSimpleSurface());
    }

    if (!pContext && dst.pObj) {
        pContext = nvCelsiusAAFindContext(dst.pObj->getSimpleSurface());
    }

    if (pContext) {
        CSimpleSurface *pSrc = 0;
        CSimpleSurface *pDst = 0;
        bool bRewriteDst = false;
        DWORD dwWidthScale, dwHeightScale;

        if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) && pContext->kelvinAA.IsEnabled()) {
            if (src.pObj && (src.pObj->getSimpleSurface() == pContext->pRenderTarget)) {
                pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_RT, CKelvinAAState::ACCESS_READ);
            }

            if (dst.pObj && (dst.pObj->getSimpleSurface() == pContext->pRenderTarget)) {
                if (pContext->kelvinAA.IsValid(CKelvinAAState::BUFFER_SRT) && 
                    !bDoEarlyBlitFallback &&
                    !pContext->kelvinAA.CompatibilityMode(CKelvinAAState::APPFLAGS_DOWNSAMPLE_ON_BLIT)) {
                    // Early blit fallback seems to be incompatible with destination rewriting.  This 
                    // should be looked at...

                    // Grant exclusive access
                    pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_SRT, CKelvinAAState::ACCESS_WRITE);
                    pDst = pContext->kelvinAA.GetSRT();
                    bRewriteDst = true;
                } else {
                    pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_RT, CKelvinAAState::ACCESS_WRITE);
                }
            }

            if (src.pObj && (src.pObj->getSimpleSurface() == pContext->pZetaBuffer)) {
                pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_ZB, CKelvinAAState::ACCESS_READ);
            }

            if (dst.pObj && (dst.pObj->getSimpleSurface() == pContext->pZetaBuffer)) {
                if (dwPbdFlags & DDBLT_DEPTHFILL) {
                    // grant exclusive access
                    pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_SZB, CKelvinAAState::ACCESS_WRITE);
                    pDst = pContext->kelvinAA.GetSZB();
                    bRewriteDst = true;
                } else {
                    pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_ZB, CKelvinAAState::ACCESS_WRITE);
                }
            }

            dwWidthScale = pContext->kelvinAA.GetWidthScale();
            dwHeightScale = pContext->kelvinAA.GetHeightScale();
        } else if (pContext->aa.isInit() && (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)) {

            // force AA enabled if registry said so

            if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASENABLE_MASK) // aa enabled
                && ((pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS] != D3DANTIALIAS_NONE)
                || (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASFORCEENABLE_MASK))) // aa forced
            {
                // create aa buffers
                if (nvCelsiusAACreate(pContext))
                {
                    // enable AA
                    pContext->aa.setFlags (AASTATE::FLAG_ENABLED);
                }
            }

            if (getDC()->dwAAContextCount >= 1)
            {
                // We don't support parameter rewriting when the source is our render target
                // or Z buffer, we must minify (if necessary)
                if (!pContext->aa.mAppCompat.bBlitSync) {
                    if (src.pObj && src.pObj->getSimpleSurface() == pContext->pRenderTarget) {
                        pContext->aa.Minify(pContext);
                    } else if (src.pObj && src.pObj->getSimpleSurface() == pContext->pZetaBuffer) {
                        pContext->aa.MinifyZ(pContext);
                    }
                }

                // If our destination is our render target or Z buffer, we want to
                // see which buffer is valid, and blit to that.  That way we prevent
                // unnecessary minifications/manifications.
                if (!pContext->aa.mAppCompat.bBlitSync) {
                    if (dst.pObj && dst.pObj->getSimpleSurface() == pContext->pRenderTarget) {
                        pDst = pContext->pRenderTarget;

                        if (!pContext->aa.isRenderTargetValid()) {
                            bRewriteDst = true;
                            pDst = pContext->aa.pSuperRenderTarget;
                        }
                    } else if (dst.pObj && dst.pObj->getSimpleSurface() == pContext->pZetaBuffer) {
                        pDst = pContext->pZetaBuffer;
                        if (!pContext->aa.isZetaBufferValid()) {
                            bRewriteDst = true;
                            pDst = pContext->aa.pSuperZetaBuffer;
                        }
                    }
                } else {
                    // If configuration file says so, we want to force blits to to go to the
                    // render target, not the super render target
                    if (dst.pObj && dst.pObj->getSimpleSurface() == pContext->pRenderTarget) {
                        pContext->aa.Minify(pContext);
                    } else if (dst.pObj && dst.pObj->getSimpleSurface() == pContext->pZetaBuffer) {
                        pContext->aa.MinifyZ(pContext);
                    }

                }

            }

            dwWidthScale = int(pContext->aa.fWidthAmplifier + 0.5f);
            dwHeightScale = int(pContext->aa.fHeightAmplifier + 0.5f);
        }

        if (bRewriteDst && pDst) {
            if ((src.dwSubRectWidth == dst.dwSubRectWidth) &&
                (src.dwSubRectHeight == dst.dwSubRectHeight)) {
                bStretchLinearFilter = FALSE;
            }
            
            dst.dwSubRectX      *= dwWidthScale;
            dst.dwSubRectY      *= dwHeightScale;
            dst.dwSubRectWidth  *= dwWidthScale;
            dst.dwSubRectHeight *= dwHeightScale;
            
            // Make a backup
            bRewriteSurfData = TRUE;
            
            fpOldVidMem = dst.pGbl->fpVidMem;
            lOldPitch = dst.pGbl->lPitch;
            wOldHeight = dst.pGbl->wHeight;
            wOldWidth = dst.pGbl->wWidth;
            
            dst.pGbl->fpVidMem = pDst->getfpVidMem();
            dst.pGbl->lPitch = pDst->getPitch();
            dst.pGbl->wHeight = (WORD)pDst->getHeight();
            dst.pGbl->wWidth = (WORD)pDst->getWidth();
            
            pbd->lpDDDestSurface = dst.pLcl;
            pbd->rDest.left   = dst.dwSubRectX;
            pbd->rDest.top    = dst.dwSubRectY;
            pbd->rDest.right  = dst.dwSubRectX + dst.dwSubRectWidth;
            pbd->rDest.bottom = dst.dwSubRectY + dst.dwSubRectHeight;
            
            dst.pObj = pDst->getWrapper();
            dst.fpVidMem = pDst->getfpVidMem();
            dst.dwOffset = pDst->getOffset();
            dst.dwPitch = pDst->getPitch();
            dst.dwWidth = pDst->getWidth();
            dst.dwHeight = pDst->getHeight();
        }
    }
#endif  // NVARCH >= 0x010

    // set up capability booleans
    doStretchX = (src.dwSubRectWidth != dst.dwSubRectWidth)   ? TRUE : FALSE;
    doStretchY = (src.dwSubRectHeight != dst.dwSubRectHeight) ? TRUE : FALSE;
    doStretch  = doStretchX || doStretchY;


    doMirrorX = (pbd->bltFX.dwDDFX & DDBLTFX_MIRRORLEFTRIGHT) ? TRUE : FALSE;
    doMirrorY = (pbd->bltFX.dwDDFX & DDBLTFX_MIRRORUPDOWN)    ? TRUE : FALSE;
    doMirror  = doMirrorX || doMirrorY;

    // trivial rejection
    retVal = bltEarlyErrorCheck(pbd, &src, &dst);
    if (retVal != DDHAL_DRIVER_HANDLED || pbd->ddRVal != DD_OK) {
        BLT_ABNORMAL_EXIT;
    }

    // make sure we get access to all of video memory
    bltUpdateClip(pDriverData);

    // texture manager wants full control before all the sync points below
    // it will check for these in time to not break
    if ((src.dwCaps | dst.dwCaps) & DDSCAPS_TEXTURE) {
        nvAssert((src.pObj && (src.pObj->getClass() == CNvObject::NVOBJ_TEXTURE))
              || (dst.pObj && (dst.pObj->getClass() == CNvObject::NVOBJ_TEXTURE))
              || (src.pObj && (src.pObj->getClass() == CNvObject::NVOBJ_SIMPLESURFACE))
              || (dst.pObj && (dst.pObj->getClass() == CNvObject::NVOBJ_SIMPLESURFACE)));

        retVal = bltTexture (pbd, &src, &dst, doStretch);
        assert(retVal == DDHAL_DRIVER_HANDLED || retVal == DDHAL_DRIVER_NOTHANDLED || retVal == DDERR_UNSUPPORTED);
        BLT_EXIT;

        // the only texture blit cases which should fall through to here are
        // FOURCC to texture blits, video to system, and system to video texture blits
        if ((src.dwCaps & DDSCAPS_TEXTURE) && (src.dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM))) {
            if (pDriverData->bltData.dwSystemBltFallback) {
                src.dwContextDma = 0;
                bDoEarlyBlitFallback = TRUE;
            } else {
                // because we didn't create a context DMA before
#ifndef WINNT
                // don't do this for WINNT because they can sometimes hand us an aliased linear address
                // to an AGP surface which is nowhere near the GARTLinearBase that we know about
                if ((src.dwCaps & DDSCAPS_NONLOCALVIDMEM) && pDriverData->GARTLinearBase) {
                    // AGP memory
                    src.dwContextDma = D3D_CONTEXT_DMA_HOST_MEMORY;
                    src.dwOffset = AGPMEM_OFFSET(src.pGbl->fpVidMem);
                } else {
#endif
                    src.dwOffset = 0;
                    src.pObj = GET_PNVOBJ(src.pLcl);        // need this, invalid or not for creating context DMAs
                    retVal = bltCreateContextDMA(pbd, &src);
                    if (retVal != DDHAL_DRIVER_HANDLED) {
                        src.dwContextDma = 0;
                        bDoEarlyBlitFallback = TRUE;
                    }
                    src.pObj = IS_VALID_PNVOBJ(src.pObj) ? src.pObj : NULL;
#ifndef WINNT
                }
#endif
            }
        }
    }

    // see if the last flip from this surface has completed
    DWORD ddrval = getFlipStatus (dst.fpVidMem, dst.pLcl->ddsCaps.dwCaps);

    // the way we handle failure depends on whether or not they're willing to wait...
    if (dwPbdFlags & DDBLT_WAIT) {
        while (ddrval != DD_OK) {
            NV_SLEEP;
            ddrval = getFlipStatus (dst.fpVidMem, dst.pLcl->ddsCaps.dwCaps);
        }
    }
    else {
        if (ddrval != DD_OK) {
            pbd->ddRVal = DDERR_WASSTILLDRAWING;
            BLT_ABNORMAL_EXIT;
        }
    }

    pDriverData->blitCalled = TRUE;
    pDriverData->TwoDRenderingOccurred = 1;

    // make sure we sync with other channels before writing put
    getDC()->nvPusher.setSyncChannelFlag();

#ifdef NVSTATDRIVER
    if (dst.dwCaps & DDSCAPS_PRIMARYSURFACE)
    {
        DDFLIPTICKS(FEBLIT);
    }
#endif

    // set blt state: ROP, pattern, colour key
    bltSetBltState(pbd, &src, &dst);

    // D3D stuff, not sure what this does
    bltControlTraffic(&src, &dst);

#ifndef WINNT
    // On win9x we need to flush system blits only on DX8 or above
    if (global.dwDXRuntimeVersion >= 0x0800)
#endif
    {
        // WIN2K has no system surface synchronization, so flush all system blits
        // from this point onward.  System textures (above this point) can be properly flushed in
        // CreateSurfaceEx, but this does not apply to any other type of system surface
        if ((src.dwCaps | dst.dwCaps) & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) {
            doFlushOnExit = TRUE;
        }
    }
    // All non-textured video to system blits must be flushed because of DX7/DX8's lack
    // of synchronization on accesses to system surfaces.
    if ((global.dwDXRuntimeVersion >= 0x0700) &&
        (src.dwCaps & (DDSCAPS_LOCALVIDMEM | DDSCAPS_NONLOCALVIDMEM)) &&
        (dst.dwCaps & DDSCAPS_SYSTEMMEMORY)) {
        doFlushOnExit = TRUE;
    }

    if (bDoEarlyBlitFallback) {
        goto bltFallback;
    }

    // colorfill (outside the unary check, needs to be fast)
    if (dwPbdFlags & DDBLT_COLORFILL) {
        DWORD dwFillColour = pbd->bltFX.dwFillColor & pDriverData->physicalColorMask;
        retVal = bltColourFill(pbd, &dst, dwFillColour);
        BLT_EXIT;
    }

    // unary blit operations (avoid all those if..thens)
    if (src.pLcl == NULL) {
        // zbuffer fill
        if (dwPbdFlags & DDBLT_DEPTHFILL) {
            //We don't need a special case for Z buffer clear
            //Regular color fill would be just fine. The only thing
            //we have to take care of is to scale dwFillDepth properly (Andrei Osnovich).
            if (!(dst.dwCaps & DDSCAPS_ZBUFFER)) {
                pbd->ddRVal = DDERR_INVALIDSURFACETYPE;
                BLT_ABNORMAL_EXIT;
            }
            retVal = bltColourFill(pbd, &dst, ScaleZFillDepth(pbd, &dst));
            BLT_EXIT;
        }

        if (dwPbdFlags & DDBLT_ROP) {
            DWORD dwRop = pbd->bltFX.dwROP >> 16;
            if (dwRop == BLACKNESSINDEX) {
                retVal = bltColourFill(pbd, &dst, 0x00000000);
                BLT_EXIT;
            }
            if (dwRop == WHITENESSINDEX) {
                retVal = bltColourFill(pbd, &dst, 0xFFFFFFFF);
                BLT_EXIT;
            }
            if (dwRop == PATCOPYINDEX) {
                DWORD dwFillColor = pbd->bltFX.dwFillColor & pDriverData->physicalColorMask;
                retVal = bltColourFill(pbd, &dst, dwFillColor);
                BLT_EXIT;
            }
            if (dwRop == PATINVERTINDEX) {
                DWORD dwFillColor = pbd->bltFX.dwFillColor & pDriverData->physicalColorMask;
                retVal = bltColourFill(pbd, &dst, ~dwFillColor);
                BLT_EXIT;
            }
        }
    }

#if (NVARCH <= 3)
    // HWBUG: in cases where we might run into a DMA read fault, copy everything except the last
    //        line to local memory, and manual copy the last line, then do the op from vidmem to vidmem
    if (src.isPotentialFault) {
        if (pbd->dwFlags & DDBLT_KEYSRCOVERRIDE) goto bltFallback;  // need another ws to handle these cases
        if (!doStretch && !doMirror && ((pbd->bltFX.dwROP >> 16) == SRCCOPYINDEX)) {
            // single pass, directly into the destination
            retVal = bltPotentialFaultHandler(pbd, &src, &dst);
            BLT_EXIT;
        } else {
            // copy into video memory workspace first and then do the op
            ws = src;
            if (bltAllocWorkSpace(pbd, &ws, BLTWS_REQUIRED_VIDEO, pbd->lpDD) == DDHAL_DRIVER_HANDLED) {
                bltPotentialFaultHandler(pbd, &src, &ws);
                src = ws;
                src.isPotentialFault = FALSE;
                // fall through to do second pass
            } else {
                goto bltFallback;
            }
        }
    }
#endif  // NVARCH <= 3

    if ((src.dwCaps & DDSCAPS_SYSTEMMEMORY) && ((src.dwWidth & 0x1) != 0)){
        //NV4/NV10 fix for scaled image -- we overrun context dma for system memory surfaces
        //because we are fetching texels in pairs. -- confirm with Gopal.
        goto bltFallback;
    }

    if (src.dwFourCC) {
        // FOURCC blits
        if (src.dwFourCC == dst.dwFourCC) {
            // FOURCC to FOURCC blits
            if (!doMirror) {
                retVal = blt4CCto4CC(pbd, &src, &dst);
                BLT_EXIT;
            }
        } else {
            if (dst.dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) {
                retVal = DDERR_UNSUPPORTED;
                BLT_ABNORMAL_EXIT;
            }
            retVal = bltStrBlt(pbd, &src, &dst, bStretchLinearFilter);
            BLT_EXIT;
        }
    } else {
        // non-FOURCC blits

        if (dst.dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) {
            if (src.bUseSwizzle) {

                // KThompson's ruminations on why we probably never reach this code...
                // 
                // How can we possibly get here?  I think we can't.  To get here, the source
                // surface has to be a swizzled surface not handled by bltTexture() further
                // above.  I can see two possible causes.  First, 'src.bUseSwizzle' is set 
                // true if all of the following are true:
                //
                //     src.pObj != NULL
                //     src.pObj->getClass()==CNvObject::NVOBJ_TEXTURE
                //     src.bIsLocked
                //     (pTex=src.pObj->getTexture()) != NULL
                //     pTex->getLinear()
                //     pTex->getLinear()->isValid()
                //
                // We could get to this point in the code if all of the above are true
                // (which causes 'src.bUseSwizzle' to be true) and "src.dwCaps&DDSCAPS_TEXTURE"
                // is false (so earlier code bypasses bltTexture()).  This seems unlikely,
                // but not provably impossible.
                //
                // The other way we could get here is if bltTexture() doesn't return
                // DDHAL_DRIVER_HANDLED, which in turn means that nvTextureBltDX7() returns
                // an error condition.  Some oddball situations to look into...
                //
                //     * Can a surface be neither texture nor simple?
                //     * Do "unsupported" blits get here: zero size, unequal src/dest depth,
                //       different src/dest DXT types, unknown FOURCC types etc?
                //
                // ...but keep in mind we still won't reach here if 'bDoEarlyBlitFallback' is
                // true, or other conditions that cause a jump to the 'bltFallback' label.

                // swizzled to unswizzled copy
                DWORD dwLSBIndex, dwMSBIndex, dwLogWidth, dwLogHeight;

                // find log width
                dwLSBIndex = dwMSBIndex = src.dwPitch;
                ASM_LOG2(dwLSBIndex);
                ASM_LOG2_R(dwMSBIndex);
                if (dwMSBIndex != dwLSBIndex) { dwMSBIndex++; } // this wasn't an even power of 2. round up.
                if (dwMSBIndex < 5) { dwMSBIndex = 5; }         // lowest allowed pitch is 32
                dwLogWidth = dwMSBIndex;

                // find log height
                dwLSBIndex = dwMSBIndex = src.dwHeight;
                ASM_LOG2(dwLSBIndex);
                ASM_LOG2_R(dwMSBIndex);
                if (dwMSBIndex != dwLSBIndex) { dwMSBIndex++; } // this wasn't an even power of 2. round up.
                dwLogHeight = dwMSBIndex;

                nvSwizBlt_cpu_swz2lin(src.fpVidMem, src.dwSubRectX, src.dwSubRectY, dwLogWidth, dwLogHeight,
                                      dst.fpVidMem, dst.dwPitch, dst.dwSubRectX, dst.dwSubRectY,
                                      dst.dwSubRectWidth, dst.dwSubRectHeight, dst.dwBytesPerPixel);
                retVal = DD_OK;
            } else {
                // simple video to system blts
                retVal = bltSimpleCopy(pbd, &src, &dst);
            }
            BLT_EXIT;
        }

        if (doStretch) {
            // strblt cases

            // NV4 has a potential single bit error in the LSBit of each colour component with SCALED_IMAGE,
            //      and will not pass WHQL.  Go to old code in 16 bpp modes
            if (pDriverData->NvDeviceVersion <= NV_DEVICE_VERSION_4 && dst.dwBytesPerPixel == 2) {
                goto bltFallback;
            }

            if (src.fpVidMem == dst.fpVidMem &&
                src.dwContextDma == dst.dwContextDma &&
              !(pbd->dwFlags & DDBLT_KEYSRCOVERRIDE) &&
                dst.dwBytesPerPixel != 1) {
                // overlapping strblt exceptions
                ws = src;
                if (bltAllocWorkSpace(pbd, &ws, BLTWS_PREFERRED_VIDEO, pbd->lpDD) == DDHAL_DRIVER_HANDLED) {
                    bltSimpleCopy(pbd, &src, &ws);
                    retVal = bltStrBlt(pbd, &ws, &dst, bStretchLinearFilter);
                    BLT_EXIT;
                }
            } else {

                // non-overlapping strblts
                if (pbd->dwFlags & DDBLT_KEYSRCOVERRIDE) {
                    // color keyed strblts
                    // do this in 2 stages, first strblt it to video mem workspace, then do a simple V2V blit
                    ws = dst;
                    if (bltAllocWorkSpace(pbd, &ws, BLTWS_REQUIRED_VIDEO, pbd->lpDD) == DDHAL_DRIVER_HANDLED) {
                        if (bltStrBlt(pbd, &src, &ws, bStretchLinearFilter) == DDHAL_DRIVER_HANDLED) {
                            bltSetBltState(pbd, &ws, &dst);
                            retVal = bltSimpleVtoVBlt(pbd, &ws, &dst);
                            BLT_EXIT;
                        }
                    } else {

                        // error allocating workspace, fall through to old code
                    }

                } else {
                    // unkeyed strblts
                    if ((src.dwSubRectWidth  > (dst.dwSubRectWidth << 1) ||
                         src.dwSubRectHeight > (dst.dwSubRectHeight << 1)) &&
                       ((pbd->bltFX.dwROP >> 16) == SRCCOPYINDEX) &&
                        (dst.dwBytesPerPixel != 1)) {
                        // VIDEO2000 quality optimization: if downscale factor is greater than 2,
                        // then downscale in multiple passes to get more effective taps
                        ws = src;
                        if (src.dwSubRectWidth  > (dst.dwSubRectWidth << 1)) {
                            ws.dwPitch >>= 1;
                            ws.dwWidth = (ws.dwWidth + 1) >> 1;
                            ws.dwSubRectX >>= 1;
                            ws.dwSubRectWidth = (ws.dwSubRectWidth + 1) >> 1;
                        }
                        if (src.dwSubRectHeight > (dst.dwSubRectHeight << 1)) {
                            ws.dwHeight = (ws.dwHeight + 1) >> 1;
                            ws.dwSubRectY >>= 1;
                            ws.dwSubRectHeight = (ws.dwSubRectHeight + 1) >> 1;
                        }
                        ws.dwSubRectHeight <<= 1;  // in case we need to do 3 passes
                        if (bltAllocWorkSpace(pbd, &ws, BLTWS_REQUIRED_VIDEO, pbd->lpDD) == DDHAL_DRIVER_HANDLED) {
                            ws.dwSubRectHeight >>= 1;  // restore height
                            bltStrBlt(pbd, &src, &ws, bStretchLinearFilter);
                            if ((ws.dwSubRectWidth  > (dst.dwSubRectWidth << 1)) ||
                                (ws.dwSubRectHeight > (dst.dwSubRectHeight << 1))) {
                                // more than 4x downscale, lets do this again
                                SURFINFO ws2 = ws;
                                ws.dwOffset += ws.dwSubRectHeight * ws.dwPitch;
                                if (ws2.dwSubRectWidth  > (dst.dwSubRectWidth << 1)) {
                                    ws.dwSubRectX >>= 1;
                                    ws.dwSubRectWidth = (ws.dwSubRectWidth + 1) >> 1;
                                }
                                if (ws2.dwSubRectHeight > (dst.dwSubRectHeight << 1)) {
                                    ws.dwSubRectY >>= 1;
                                    ws.dwSubRectHeight = (ws.dwSubRectHeight + 1) >> 1;
                                }
                                bltStrBlt(pbd, &ws2, &ws, bStretchLinearFilter);
                            }
                            retVal = bltStrBlt(pbd, &ws, &dst, bStretchLinearFilter);
                            BLT_EXIT;
                        }
                    }
                    // simple case
                    retVal = bltStrBlt(pbd, &src, &dst, bStretchLinearFilter);
                    BLT_EXIT;
                }

            }
        } else {    // !doStretch
            // simple blit cases
            if (src.dwCaps & DDSCAPS_LOCALVIDMEM) {
                // video to video
                if (!doMirror) {
                    retVal = bltSimpleVtoVBlt(pbd, &src, &dst);
                    BLT_EXIT;
                }
            } else {
                // system to video
                if (pbd->dwFlags & DDBLT_KEYSRCOVERRIDE) {
                    // colour keyed system to video
                    if (!doMirror) {
                        // do this in 2 stages, first copy it to video mem workspace, then do a simple V2V blit
                        ws = src;
                        if (bltAllocWorkSpace(pbd, &ws, BLTWS_REQUIRED_VIDEO, pbd->lpDD) == DDHAL_DRIVER_HANDLED) {
                            bltSimpleCopy(pbd, &src, &ws);
                            retVal = bltSimpleVtoVBlt(pbd, &ws, &dst);
                            BLT_EXIT;
                        } else {
                            // error allocating workspace, fall through to old code
                        }
                    }
                } else {
                    // unkeyed system to video

                    // NV4 has a potential single bit error in the LSBit of each colour component with SCALED_IMAGE,
                    //      and will not pass WHQL.  Go to old code in 16 bpp modes
                    if (pDriverData->NvDeviceVersion <= NV_DEVICE_VERSION_4 && dst.dwBytesPerPixel == 2) {
                        goto bltFallback;
                    }

                    if ((pbd->bltFX.dwROP >> 16) == SRCCOPYINDEX && !doMirror) {
                        // no stretch, no colour key, no rop, no mirror
                        retVal = bltSimpleCopy(pbd, &src, &dst);
                        BLT_EXIT;
                    } else {
                        retVal = bltStrBlt(pbd, &src, &dst, bStretchLinearFilter);
                        BLT_EXIT;
                    }
                }
            }
        }   // doStretch
    }

bltFallback:
    // all other blt cases, fall back code

    // need to restore these things if we changed them
    if (dst.bDefaultBlitChanged) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000);
        nvPushData(1, NV_DD_IMAGE_BLIT);
        nvPusherAdjust(2);
    }

    if (dst.dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) {
        // slow video to system case
        retVal = bltSlowVtoSBlt(pbd, &src, &dst);
        BLT_EXIT;
    } else {
        // slow system to video, or video to video case
        if (src.dwContextDma == dst.dwContextDma && src.dwOffset == dst.dwOffset) {
            // deal with overlapping case
            ws = src;
            if (bltAllocWorkSpace(pbd, &ws, BLTWS_PREFERRED_SYSTEM, pbd->lpDD) == DDHAL_DRIVER_HANDLED) {
                bltSimpleCopy(pbd, &src, &ws);
                src = ws;
                WaitForIdle(TRUE, TRUE);
            } else {
                retVal = bltSlowStoVBlt(pbd, &src, &dst);
                BLT_EXIT;
            }
        }
        retVal = bltSlowStoVBlt(pbd, &src, &dst);
        BLT_EXIT;
    }

    // should never get to this point, if you do, see slum or drohrer
    DPF("Blit end run");
    nvAssert(FALSE);

    BLT_ABNORMAL_EXIT;

}  // Blit32

// -----------------------------------------------------------------------------
// bltEarlyErrorCheck
//      Does cursory error checking
DWORD __stdcall bltEarlyErrorCheck (LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
    dbgTracePush ("bltEarlyErrorCheck");

    pbd->ddRVal = DD_OK;

    /*
     * This needs to be here for cases where the channel has closed due to
     * a mode switch or some other reason when there is a pending notification
     * which gets lost due to the channel closure. nvEnable32 will deal
     * with clearing the pending notifier.
     */
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        if (!nvEnable32(pbd->lpDD)) {
            DPF("BLTERROR: DMA channel lost");
            pbd->ddRVal = DDERR_SURFACELOST;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    // no destination color key
    if (pbd->dwFlags & (DDBLT_KEYDEST | DDBLT_KEYDESTOVERRIDE)) {
        DPF("BLTERROR: destination colour key not supported");
        pbd->ddRVal = DDERR_NOCOLORKEYHW;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    // trivial reject of zero dimensioned blt
    if (pbd->rDest.right == pbd->rDest.left || pbd->rDest.bottom == pbd->rDest.top) {
        DPF("BLTERROR: trivial reject of zero dimensioned blt");
        dbgTracePop();
        return DDHAL_DRIVER_NOTHANDLED;
    }

    // can't do system to system blits
    if ((pSrc->dwCaps & DDSCAPS_SYSTEMMEMORY) && (pDst->dwCaps & DDSCAPS_SYSTEMMEMORY)) {
        DPF("BLTERROR: can't do system to system blits");
        dbgTracePop();
        return DDHAL_DRIVER_NOTHANDLED;
    }


    //// h.azar: can't do YUV surface to RGB texture blit for now... this is part of 
    // the VMR support which is implemented in rel20
    // ** this condition will prevent a hang in the VMR DCT450 test
    if (((pSrc->dwFourCC == FOURCC_YUY2) || (pSrc->dwFourCC == FOURCC_UYVY) || (pSrc->dwFourCC == FOURCC_NV12)) && (pDst->dwCaps & DDSCAPS_TEXTURE))
    {
        DPF("BLTERROR: can't do yuv surface to texture blits");
        dbgTracePop();
        return DDHAL_DRIVER_NOTHANDLED;
    }


    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltUpdateClip
//      Updates the NV clipper state if necessary
DWORD __stdcall bltUpdateClip(GLOBALDATA *pDriverData)
{
    dbgTracePush ("bltUpdateClip");
    if (pDriverData->ddClipUpdate ||
        pDriverData->dwSharedClipChangeCount != pDriverData->dwDDMostRecentClipChangeCount) {

        getDC()->nvPusher.push (0,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
        getDC()->nvPusher.push (1,NV_DD_IMAGE_BLACK_RECTANGLE);
        getDC()->nvPusher.push (2,dDrawSubchannelOffset(NV_DD_SURFACES) +
                                IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000);
        getDC()->nvPusher.push (3,0);
        getDC()->nvPusher.push (4,asmMergeCoords(NV_MAX_X_CLIP,NV_MAX_Y_CLIP));
        getDC()->nvPusher.push (5,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
        getDC()->nvPusher.push (6,NV_DD_SURFACES_2D);

        getDC()->nvPusher.adjust (7);
        getDC()->nvPusher.start  (TRUE);

        pDriverData->dwSharedClipChangeCount++;
        pDriverData->dwDDMostRecentClipChangeCount = pDriverData->dwSharedClipChangeCount;
        pDriverData->ddClipUpdate = FALSE;
    }
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltDestroyAllContextDMAs
//      Destroys all context DMAs created by bltCreateContextDMA and walks
//      through to entire surface list to adjust the NVObjects
DWORD __stdcall bltDestroyAllContextDMAs(LPDDRAWI_DIRECTDRAW_GBL lpDD)
{
    dbgTracePush ("bltDestroyAllContextDMAs");

    // walk the entire surface list
#ifdef WINNT
    // FIXME: figure out what how to clean up surfaces that do not have a CNvObject
    CNvObject *pObj = global.pNvObjectHead;
    while (pObj) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, pObj->getContextDma());
        pObj->setContextDma(0);
        if (pObj->getSimpleSurface() ) pObj->getSimpleSurface()->tagNotAsDynSys();
        pObj = pObj->getNext();
    }
#else
    LPDDRAWI_DDRAWSURFACE_INT pSurfInt;

    for (pSurfInt = lpDD->dsList; pSurfInt != NULL; pSurfInt = pSurfInt->lpLink) {
        LPDDRAWI_DDRAWSURFACE_LCL pSurfLcl = pSurfInt->lpLcl;
        if (pSurfLcl->ddsCaps.dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) {
            CNvObject *pObj = GET_PNVOBJ(pSurfLcl);
            if ((((DWORD)pObj) & MASK_FLOATING_CONTEXT_DMA_ID) == FLOATING_CONTEXT_DMA_ID) {
                NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, (DWORD) pObj);
                SET_PNVOBJ(pSurfLcl, 0);
            } else if (IS_VALID_PNVOBJ(pObj)) {
                NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, pObj->getContextDma());
                pObj->setContextDma(0);
                if (pObj->getSimpleSurface() ) pObj->getSimpleSurface()->tagNotAsDynSys();
            }
        }
    }
#endif

    // really make sure
    DWORD dwContextDma;
    for (DWORD i=0; i < pDriverData->bltData.dwSystemSurfaceContextDMAIndex; i+=2) {
        dwContextDma = NV_DD_FLOATING_CONTEXT_DMA_BASE + i;
        // HWFIX: deal with hw hash table bug, swap bits 1 and 2 with 11 and 22 (which are part of the hash function)
        dwContextDma = ((dwContextDma & 0x00000002) << 10) | ((dwContextDma & 0x00000800) >> 10) |
                       ((dwContextDma & 0x00000004) << 20) | ((dwContextDma & 0x00400000) >> 20) |
                       (dwContextDma & ~0x00400806);
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, dwContextDma);
    }

    // seem to cause RM errors on next allocation, even though we have previously destroyed them
//    pDriverData->bltData.dwSystemSurfaceContextDMAIndex = 0;

    // free system workspace
    if (pDriverData->bltSysMemInfo.dwLocale == BLTWS_SYSTEM) {
        FreeIPM((void*)pDriverData->bltSysMemInfo.fpVidMem);
        pDriverData->bltSysMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }

    // free AGP workspace
    if (pDriverData->bltAGPMemInfo.dwLocale == BLTWS_AGP) {
        nvAGPFree((void*)pDriverData->bltAGPMemInfo.fpVidMem);
        pDriverData->bltAGPMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }

    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltCreateContextDMA
//      Creates a context DMA for system memory surfaces if it doesn't already
//      exist.  Fills in the instance handle in pContextDmaInstance
DWORD __stdcall bltCreateContextDMA(LPDDHAL_BLTDATA pbd, LPSURFINFO pSurf)
{
#ifdef WINNT
    // WINNT faults when attempting to free a system surface (out of our control, driver never called) which
    // has a context DMA attached to it.  Creation and destruction of the context DMA within the blit call
    // is way too slow, so just fail all dynamic context DMA allocation and use the fallback code
    return DDHAL_DRIVER_NOTHANDLED;

#else   // win9x

DWORD dwSurfSize;

    dbgTracePush ("bltCreateContextDMA");

    pSurf->dwContextDma = 0;
    if ((DWORD) pSurf->pObj != 1) {
        if ((((DWORD)pSurf->pObj) & MASK_FLOATING_CONTEXT_DMA_ID) == FLOATING_CONTEXT_DMA_ID) {
            pSurf->dwContextDma = (DWORD)pSurf->pObj;
        } else if (pSurf->pObj) {
            pSurf->dwContextDma = pSurf->pObj->getContextDma();
        }
    }

    dwSurfSize = pSurf->dwPitch * pSurf->dwHeight;
    switch (pSurf->dwFourCC) {
    case FOURCC_YV12:
    case FOURCC_420i:
        dwSurfSize += (pSurf->dwPitch >> 1) * (pSurf->dwHeight >> 1) * 2;
        break;

    case FOURCC_YVU9:
    case FOURCC_IF09:
    case FOURCC_IV32:
    case FOURCC_IV31:
        dwSurfSize += (pSurf->dwPitch >> 2) * (pSurf->dwHeight >> 2) * 2;
        break;

    default:
        break;
    }

#if (NVARCH <= 3)
    // HWBUG: If a DMA read is attempted near the end of a system surface, HW will try to
    //        prefetch and may fault past the end of the surface
    if (((pSurf->dwSubRectY + pSurf->dwSubRectHeight) >= (pSurf->dwHeight - (NV10_BLT_READ_AHEAD_BUFFER_SIZE / pSurf->dwPitch))) &&
          IsBadReadPtr((LPVOID)(pSurf->fpVidMem + dwSurfSize + (NV10_BLT_READ_AHEAD_BUFFER_SIZE - 1)), 1)) {
        pSurf->isPotentialFault = TRUE;
    }
#endif  // NVARCH <= 3

#ifdef BLT_VERYSAFE
    // touch each page of the system memory surface to ensure it is paged in
    LPBYTE ptr = (LPBYTE) pSurf->fpVidMem;
    for (DWORD i=0; i<((dwSurfSize+4093)/4096); i++) {
        volatile BYTE bDontOptimizeMe = *ptr;
        ptr += 4096;
    }
#endif

    if (!pSurf->dwContextDma) {

        if ((pDriverData->bltData.dwSystemSurfaceContextDMAIndex >> 1) >= MAX_FLOATING_CONTEXT_DMA) {
            bltDestroyAllContextDMAs(pbd->lpDD);
            pDriverData->bltData.dwSystemSurfaceContextDMAIndex = 0;
#ifdef  SYSTEMBLTFALLBACK
            //Verified that context DMA allocation case still works 3.3 times faster than the
            //SystemBltFallback case. It is up to Sandy to finally decide on this. Andrei O.
            if (!pDriverData->bltData.dwLetItThrash) {
                pDriverData->bltData.dwSystemBltFallback = TRUE;
            }
#endif  //SYSTEMBLTFALLBACK
        }

        pSurf->dwContextDma = NV_DD_FLOATING_CONTEXT_DMA_BASE + pDriverData->bltData.dwSystemSurfaceContextDMAIndex;
        // HWFIX: deal with hw hash table bug, swap bits 1 and 2 with 11 and 22 (which are part of the hash function)
        pSurf->dwContextDma = ((pSurf->dwContextDma & 0x00000002) << 10) | ((pSurf->dwContextDma & 0x00000800) >> 10) |
                              ((pSurf->dwContextDma & 0x00000004) << 20) | ((pSurf->dwContextDma & 0x00400000) >> 20) |
                              (pSurf->dwContextDma & ~0x00400806);

        // careful, DestroySurface is not called for non-texture system surfaces on DX7
        // this means we must rely on DestroyDriver to get rid of these
        if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                                pSurf->dwContextDma,
                                NV01_CONTEXT_DMA,
                                DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                DRF_DEF(OS03, _FLAGS, _PHYSICALITY, _NONCONTIGUOUS) |
                                DRF_DEF(OS03, _FLAGS, _LOCKED, _IN_TRANSIT) |
                                DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                               (PVOID)pSurf->fpVidMem,
                                dwSurfSize - 1)) {
            // ran out of context DMA's, delete all and try again
            bltDestroyAllContextDMAs(pbd->lpDD);

            // try again
            if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                                    pSurf->dwContextDma,
                                    NV01_CONTEXT_DMA,
                                    DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                    DRF_DEF(OS03, _FLAGS, _PHYSICALITY, _NONCONTIGUOUS) |
                                    DRF_DEF(OS03, _FLAGS, _LOCKED, _IN_TRANSIT) |
                                    DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                                   (PVOID)pSurf->fpVidMem,
                                    dwSurfSize - 1)) {
                pSurf->isPotentialFault = TRUE;
                pSurf->dwContextDma = 0;
                dbgTracePop();
                return DDHAL_DRIVER_NOTHANDLED;
            }
        }
        if ((DWORD) pSurf->pObj != 1) {
            if (IS_VALID_PNVOBJ(pSurf->pObj)) {
                pSurf->pObj->setContextDma(pSurf->dwContextDma);
                if (pSurf->pObj->getSimpleSurface() ) pSurf->pObj->getSimpleSurface()->tagAsDynSys();
                //textures are a seperate path
                //vertex buffers can't be blited
                //if this changes, we need to set the correct bits for each type of
                //object since everything doesn't inherit HMH
            } else {
                SET_PNVOBJ(pSurf->pLcl, pSurf->dwContextDma);
            }
        }
        pDriverData->bltData.dwSystemSurfaceContextDMAIndex += 2;
    }

    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
#endif  // WINNT
}

// -----------------------------------------------------------------------------
// bltAllocVidMem
//      Video memory allocator
DWORD __stdcall bltAllocVidMem(LPSURFINFO pSurf, DWORD dwSize)
{
    dbgTracePush ("bltAllocVidMem");

    // check for enough space
    if (pDriverData->bltVidMemInfo.dwWSSize < dwSize &&
        pDriverData->bltVidMemInfo.dwLocale == BLTWS_VIDEO) {
        // not enough space, deallocate current video workspace
        NVHEAP_FREE(pDriverData->bltVidMemInfo.fpVidMem);
        pDriverData->bltVidMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }

    // allocate it
    if (pDriverData->bltVidMemInfo.dwLocale == BLTWS_UNINITIALIZED) {
        DWORD dwStatus;
        NVHEAP_ALLOC(dwStatus, pDriverData->bltVidMemInfo.fpVidMem, dwSize, TYPE_IMAGE);
        if (dwStatus==0) {
            // video mem allocation succeeded
            pDriverData->bltVidMemInfo.dwLocale = BLTWS_VIDEO;
            pDriverData->bltVidMemInfo.dwWSSize = dwSize;
            pDriverData->bltVidMemInfo.dwContextDma = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
            pDriverData->bltVidMemInfo.dwOffset = VIDMEM_OFFSET(pDriverData->bltVidMemInfo.fpVidMem);
        } else {
            // failed to allocate in video memory
            dbgTracePop();
            return DDHAL_DRIVER_NOTHANDLED;
        }
    }

    pSurf->dwCaps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
    pSurf->dwCaps &= ~(DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM);
    pSurf->dwContextDma = pDriverData->bltVidMemInfo.dwContextDma;
    pSurf->dwOffset = pDriverData->bltVidMemInfo.dwOffset;
    pSurf->fpVidMem = pDriverData->bltVidMemInfo.fpVidMem;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltAllocSysMem
//      System memory allocator
DWORD __stdcall bltAllocSysMem(LPDDHAL_BLTDATA pbd, LPSURFINFO pSurf, DWORD dwSize)
{
    dbgTracePush ("bltAllocSysMem");

    // check for enough space
    if (pDriverData->bltSysMemInfo.dwWSSize < dwSize &&
        pDriverData->bltSysMemInfo.dwLocale == BLTWS_SYSTEM) {
        // not enough space, deallocate current system workspace
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, pDriverData->bltSysMemInfo.dwContextDma);
        FreeIPM((void*)pDriverData->bltSysMemInfo.fpVidMem);
        pDriverData->bltSysMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }

    if (pDriverData->bltSysMemInfo.dwLocale == BLTWS_UNINITIALIZED) {
        pDriverData->bltSysMemInfo.fpVidMem = (DWORD) AllocIPM(dwSize + 4096);
        if (pDriverData->bltSysMemInfo.fpVidMem) {
            // sys mem allocation succeeded
            pDriverData->bltSysMemInfo.dwLocale = BLTWS_SYSTEM;
            pDriverData->bltSysMemInfo.dwWSSize = dwSize;
            pDriverData->bltSysMemInfo.dwOffset = 0;
            pSurf->dwOffset = 0;
            pSurf->fpVidMem = pDriverData->bltSysMemInfo.fpVidMem;
            pSurf->pObj = (CNvObject *) 1;
            if (bltCreateContextDMA(pbd, pSurf) != DDHAL_DRIVER_HANDLED) {
                FreeIPM((void*)pDriverData->bltSysMemInfo.fpVidMem);
                pDriverData->bltSysMemInfo.dwLocale = BLTWS_UNINITIALIZED;
                dbgTracePop();
                return DDHAL_DRIVER_NOTHANDLED;
            }
            pDriverData->bltSysMemInfo.dwContextDma = pSurf->dwContextDma;
            pSurf->isPotentialFault = FALSE;    // we already added 4k to ensure HWBUG does not happen
        } else {
            // failed to allocate in system memory
            dbgTracePop();
            return DDHAL_DRIVER_NOTHANDLED;
        }
    }

    pSurf->dwCaps |= DDSCAPS_SYSTEMMEMORY;
    pSurf->dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM | DDSCAPS_LOCALVIDMEM);
    pSurf->fpVidMem = pDriverData->bltSysMemInfo.fpVidMem;
    pSurf->dwOffset = pDriverData->bltSysMemInfo.dwOffset;
    pSurf->dwContextDma = pDriverData->bltSysMemInfo.dwContextDma;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltAllocAGPMem
//      Allocate and AGP workspace
DWORD __stdcall bltAllocAGPMem(LPDDHAL_BLTDATA pbd, LPSURFINFO pSurf, DWORD dwSize, LPDDRAWI_DIRECTDRAW_GBL lpDD)
{
    dbgTracePush ("bltAllocAGPMem");

    // check for enough space
    if (pDriverData->bltAGPMemInfo.dwWSSize < dwSize &&
        pDriverData->bltAGPMemInfo.dwLocale == BLTWS_SYSTEM) {
        // not enough space, deallocate current system workspace
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, pDriverData->bltAGPMemInfo.dwContextDma);
        nvAGPFree((void*)pDriverData->bltAGPMemInfo.fpVidMem);
        pDriverData->bltAGPMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }

    if (pDriverData->bltAGPMemInfo.dwLocale == BLTWS_UNINITIALIZED) {
        pDriverData->bltAGPMemInfo.fpVidMem = (DWORD)nvAGPAlloc(dwSize + 4096);
        if (pDriverData->bltAGPMemInfo.fpVidMem != ERR_DXALLOC_FAILED) {
            // sys mem allocation succeeded
            pDriverData->bltAGPMemInfo.dwLocale = BLTWS_SYSTEM;
            pDriverData->bltAGPMemInfo.dwWSSize = dwSize;
            pDriverData->bltAGPMemInfo.dwOffset = 0;
            pSurf->dwOffset = 0;
            pSurf->fpVidMem = pDriverData->bltAGPMemInfo.fpVidMem;
            pSurf->pObj = (CNvObject *) 1;
            if (bltCreateContextDMA(pbd, pSurf) != DDHAL_DRIVER_HANDLED) {
                nvAGPFree((void*)pDriverData->bltAGPMemInfo.fpVidMem);
                pDriverData->bltSysMemInfo.dwLocale = BLTWS_UNINITIALIZED;
                dbgTracePop();
                return DDHAL_DRIVER_NOTHANDLED;
            }
            pDriverData->bltAGPMemInfo.dwContextDma = pSurf->dwContextDma;
            pSurf->isPotentialFault = FALSE;    // we already added 4k to ensure HWBUG does not happen
        } else {
            // failed to allocate in system memory
            dbgTracePop();
            return DDHAL_DRIVER_NOTHANDLED;
        }
    }

    pSurf->dwCaps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM;
    pSurf->dwCaps &= ~(DDSCAPS_SYSTEMMEMORY | DDSCAPS_LOCALVIDMEM);
    pSurf->dwContextDma = pDriverData->bltAGPMemInfo.dwContextDma;
    pSurf->dwOffset = pDriverData->bltAGPMemInfo.dwOffset;
    pSurf->fpVidMem = pDriverData->bltAGPMemInfo.fpVidMem;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltAllocWorkspace
//      Allocates workspace in video, AGP, or system memory for use by blts.
//      Returns the dwContextDMA and dwOffset members of the SURFINFO structure.
DWORD __stdcall bltAllocWorkSpace(LPDDHAL_BLTDATA pbd, LPSURFINFO pSurf, DWORD dwFlags, LPDDRAWI_DIRECTDRAW_GBL lpDD)
{
DWORD dwSize;

    dbgTracePush ("bltAllocWorkSpace");

    // only allocate what is required for the subrectangle
    pSurf->dwWidth = pSurf->dwSubRectWidth;
    pSurf->dwHeight = pSurf->dwSubRectHeight;
    pSurf->dwSubRectX = 0;
    pSurf->dwSubRectY = 0;

    // surface size calculation
    // pitch alignment only required for vidmem surfaces, but do it for all to simplify logic
    switch (pSurf->dwFourCC) {
    case FOURCC_YV12:
    case FOURCC_420i:
        pSurf->dwPitch = (pSurf->dwWidth + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        dwSize = pSurf->dwPitch * pSurf->dwHeight * 3 / 2 + pSurf->dwPitch;
        break;

    case FOURCC_YVU9:
    case FOURCC_IF09:
    case FOURCC_IV32:
    case FOURCC_IV31:
        pSurf->dwPitch = (pSurf->dwWidth + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        dwSize = pSurf->dwPitch * pSurf->dwHeight * 9 / 8 + pSurf->dwPitch;
        break;

    case FOURCC_YUY2:
    case FOURCC_UYVY:
    case FOURCC_UYNV:
    case FOURCC_YUNV:
    default:
        pSurf->dwPitch = (pSurf->dwWidth * pSurf->dwBytesPerPixel + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        dwSize = pSurf->dwPitch * pSurf->dwHeight;
        break;
    }

    if (dwFlags & (BLTWS_PREFERRED_VIDEO | BLTWS_REQUIRED_VIDEO)) {
        if (bltAllocVidMem(pSurf, dwSize) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
        // failed to allocate in video memory
        if (dwFlags & BLTWS_REQUIRED_VIDEO) {
            // fatal error
            dbgTracePop();
            return DDHAL_DRIVER_NOTHANDLED;
        }
        // try system memory
        if (bltAllocSysMem(pbd, pSurf, dwSize) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
        // try AGP memory
        if (bltAllocAGPMem(pbd, pSurf, dwSize, lpDD) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    } else if (dwFlags & (BLTWS_PREFERRED_AGP | BLTWS_REQUIRED_AGP)) {
        // AGP allocation
        if (bltAllocAGPMem(pbd, pSurf, dwSize, lpDD) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
        // failed to allocate in AGP memory
        if (dwFlags & BLTWS_REQUIRED_AGP) {
            // fatal error
            dbgTracePop();
            return DDHAL_DRIVER_NOTHANDLED;
        }
        // try system memory
        if (bltAllocSysMem(pbd, pSurf, dwSize) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
        // try video memory
        if (bltAllocVidMem(pSurf, dwSize) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    } else if (dwFlags & (BLTWS_PREFERRED_SYSTEM | BLTWS_REQUIRED_SYSTEM)) {
        // system memory allocation
        if (bltAllocSysMem(pbd, pSurf, dwSize) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
        // failed to allocate in system memory
        if (dwFlags & BLTWS_REQUIRED_SYSTEM) {
            // fatal error
            dbgTracePop();
            return DDHAL_DRIVER_NOTHANDLED;
        }
        // try AGP memory
        if (bltAllocAGPMem(pbd, pSurf, dwSize, lpDD) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
        // try video memory
        if (bltAllocVidMem(pSurf, dwSize) == DDHAL_DRIVER_HANDLED) {
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    dbgTracePop();
    return DDHAL_DRIVER_NOTHANDLED;
}

// -----------------------------------------------------------------------------
// bltSetSurfaces2D
//      Sets source and destination surface context
DWORD __stdcall bltSetSurfaces2D(LPSURFINFO pSrc, LPSURFINFO pDst)
{
DWORD dwDstPitch, dwSrcPitch, dwCombinedPitch;
DWORD dwDstOffset, dwSrcOffset, dwColourFormat;

    dbgTracePush("bltSetSurfaces2D");

    dwDstOffset = pDst->dwOffset;
    dwSrcOffset = pSrc->dwOffset;

    // only set surfaces2D if we are going to use it
    if (!pDst->bUseSwizzle && !pSrc->bUseSwizzle) {

        // If dest is in system memory, then we won't actually use this part
        // of surfaces2D and we just need to set it to something that RM won't throw up on
        // If we actually need to fix surface pitch alignment, do it at surface creation
        dwDstPitch = (pDst->dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) ?
                     (pDst->dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad :
                     pDst->dwPitch;

        dwSrcPitch = (pSrc->dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) ?
                     (pSrc->dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad :
                     pSrc->dwPitch;

        nvAssert(dwDstPitch != 0);

        if ((pSrc->pGbl == NULL) || (dwSrcPitch == 0) ||
            (pSrc->dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM))) {
            dwSrcOffset = 0;
            dwSrcPitch = dwDstPitch;
        }
        if (pDst->dwCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM)) {
            dwDstOffset = 0;
            dwDstPitch = dwSrcPitch;
        }
        dwCombinedPitch = (dwDstPitch << 16) | dwSrcPitch;

        if (pDst->dwCaps & DDSCAPS_ZBUFFER) {
            switch (pDst->dwBytesPerPixel) {
            case 3:
            case 4:  dwColourFormat = NV062_SET_COLOR_FORMAT_LE_Y32;    break;
            default: dwColourFormat = NV062_SET_COLOR_FORMAT_LE_Y16;    break;
            }
            nvPushData(0, dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000);
            nvPushData(1, NV_DD_SRCCOPY_IMAGE_BLIT);
            nvPusherAdjust(2);
            pDst->bDefaultBlitChanged = TRUE;
        } else {
            switch (pDst->dwBytesPerPixel) {
            case 1:  dwColourFormat = NV062_SET_COLOR_FORMAT_LE_Y8;                 break;
            case 4:  dwColourFormat = NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;  break;
            default: dwColourFormat = NV062_SET_COLOR_FORMAT_LE_R5G6B5;             break;
            }
        }

        // if copy alpha...
        if (!pSrc->dwFourCC &&
            pSrc->dwContextDma == NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY &&
            pDst->dwContextDma == NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY &&
            pDriverData->bltData.dwLastColourKey == 0xFFFFFFFF &&
            pDst->dwBytesPerPixel == 4 &&
            pDriverData->bltData.dwLastRop == SRCCOPYINDEX &&
            pDst->dwSubRectWidth == pSrc->dwSubRectWidth &&
            pDst->dwSubRectHeight == pSrc->dwSubRectHeight &&
         !((pDst->dwCaps | pSrc->dwCaps) & DDSCAPS_TEXTURE)) {
            dwColourFormat = NV062_SET_COLOR_FORMAT_LE_Y32;
        }

        if (pDriverData->bltData.dwLastColourFormat != dwColourFormat ||
            pDriverData->bltData.dwLastCombinedPitch != dwCombinedPitch ||
            pDriverData->bltData.dwLastSrcOffset != dwSrcOffset ||
            pDriverData->bltData.dwLastDstOffset != dwDstOffset) {

            nvPushData(0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
            nvPushData(1, dwColourFormat);                  // SetColorFormat
            nvPushData(2, dwCombinedPitch);                 // SetPitch
            nvPushData(3, dwSrcOffset);                     // SetSrcOffset
            nvPushData(4, dwDstOffset);                     // SetDstOffset
            nvPusherAdjust(5);

            pDriverData->bltData.dwLastColourFormat = dwColourFormat;
            pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;
            pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
            pDriverData->bltData.dwLastDstOffset = dwDstOffset;

            // also set mono pattern mask when pixel depth changes
            // scaled image and gdi_rect will silently fail unless we set the pattern (?)
            getDC()->nvPusher.push(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
            getDC()->nvPusher.push(1, NV_DD_CONTEXT_PATTERN);
            getDC()->nvPusher.push(2, dDrawSubchannelOffset(NV_DD_SPARE) + NV044_SET_PATTERN_SELECT | 0xC0000);
            getDC()->nvPusher.push(3, NV044_SET_PATTERN_SELECT_MONOCHROME);
            switch (pDst->dwBytesPerPixel) {
            case 1:
                getDC()->nvPusher.push(4,NV_ALPHA_1_008);
                getDC()->nvPusher.push(5,NV_ALPHA_1_008);
                break;
            case 4:
                getDC()->nvPusher.push(4,NV_ALPHA_1_032);
                getDC()->nvPusher.push(5,NV_ALPHA_1_032);
                break;
            default:
                getDC()->nvPusher.push(4,NV_ALPHA_1_016);
                getDC()->nvPusher.push(5,NV_ALPHA_1_016);
                break;
            }
            getDC()->nvPusher.adjust(6);
        }

    } else {

        DWORD dwLSBIndex, dwMSBIndex, dwLogWidth, dwLogHeight;

        // find log width
        dwLSBIndex = dwMSBIndex = pDst->dwWidth;
        ASM_LOG2(dwLSBIndex);
        ASM_LOG2_R(dwMSBIndex);
        if (dwMSBIndex != dwLSBIndex) { dwMSBIndex++; } // this wasn't an even power of 2. round up.
        dwLogWidth = dwMSBIndex;

        // find log height
        dwLSBIndex = dwMSBIndex = pDst->dwHeight;
        ASM_LOG2(dwLSBIndex);
        ASM_LOG2_R(dwMSBIndex);
        if (dwMSBIndex != dwLSBIndex) { dwMSBIndex++; } // this wasn't an even power of 2. round up.
        dwLogHeight = dwMSBIndex;

        // find surface format
        switch (pDst->dwBytesPerPixel) {
            case 2:  dwColourFormat = NV052_SET_FORMAT_COLOR_LE_R5G6B5;    break;
            case 4:  dwColourFormat = NV052_SET_FORMAT_COLOR_LE_A8R8G8B8;  break;
            default: DPF ("illegal format"); dbgD3DError();                break;
        }

        // set up swizzled surface
        // for now, assume destination is always in video memory
        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData(1, D3D_CONTEXT_SURFACE_SWIZZLED);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | NV052_SET_FORMAT | 0x80000);
        nvPushData(3, dwColourFormat     |                  // SetFormat
                      (dwLogWidth << 16) |
                      (dwLogHeight << 24));
        nvPushData(4, dwDstOffset);                         // SetOffset
        nvPusherAdjust(5);
    }

    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltSetBltState
//      Sets surfaces context, ROP, and colour key state

DWORD __stdcall bltSetBltState(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
DWORD dwRop;

    dbgTracePush("bltSetBltState");

    dwRop = pbd->bltFX.dwROP >> 16;
    if (dwRop != pDriverData->bltData.dwLastRop) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_ROP) | SET_ROP_OFFSET | 0x40000);
        nvPushData(1, dwRop);
        nvPusherAdjust(2);
        pDriverData->bltData.dwLastRop = dwRop;
    }

    if (pbd->dwFlags & DDBLT_KEYSRCOVERRIDE) {
        // source key is enabled
        const static DWORD alphaBit[5] = {0, NV_ALPHA_1_008, NV_ALPHA_1_016, NV_ALPHA_1_032, NV_ALPHA_1_032};
        DWORD dwColorKey = pbd->bltFX.ddckSrcColorkey.dwColorSpaceLowValue;

        if (dwColorKey != pDriverData->bltData.dwLastColourKey) {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
            nvPushData(1, NV_DD_CONTEXT_COLOR_KEY);
            nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | SET_TRANSCOLOR_OFFSET | 0x40000);
            nvPushData(3, (dwColorKey & pDriverData->physicalColorMask) | alphaBit[pDst->dwBytesPerPixel]);
            nvPusherAdjust(4);
            pDriverData->bltData.dwLastColourKey = dwColorKey;
            pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_COLOR_KEY;
        }
    } else {
        // source key is disabled
        if (pDriverData->bltData.dwLastColourKey != 0xFFFFFFFF) {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
            nvPushData(1, NV_DD_CONTEXT_COLOR_KEY);
            nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | SET_TRANSCOLOR_OFFSET | 0x40000);
            nvPushData(3, 0);
            nvPusherAdjust(4);
            pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;
            pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_COLOR_KEY;
        }

        // in 32 bpp modes, if no ROP and no colour key and no stretch and not a texture, then copy alpha too
        // WARNING:
        //  - We still won't be able to handle alpha with ROP or colour key.  Luckily this
        //    behaviour is ill-defined and is unlikely that anyone will use it.
        //  - On older NV architectures, alpha does stuff like bypass the gamma table for that
        //    pixel.  So far WHQL tests alpha copying in offscreen memory.  If they try
        //    this in the primary, it could look funny.
        if (!pSrc->dwFourCC &&
            pSrc->dwContextDma == NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY &&
            pDst->dwContextDma == NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY &&
            pDst->dwBytesPerPixel == 4 && dwRop == SRCCOPYINDEX &&
            pDst->dwSubRectWidth == pSrc->dwSubRectWidth &&
            pDst->dwSubRectHeight == pSrc->dwSubRectHeight &&
         !((pDst->dwCaps | pSrc->dwCaps) & DDSCAPS_TEXTURE)) {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000);
            nvPushData(1, NV_DD_SRCCOPY_IMAGE_BLIT);
            nvPushData(2, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000);
            nvPushData(3, NV062_SET_COLOR_FORMAT_LE_Y32);   // SetColorFormat
            nvPusherAdjust(4);
            pDst->bDefaultBlitChanged = TRUE;
            pDriverData->bltData.dwLastColourFormat = NV062_SET_COLOR_FORMAT_LE_Y32;
        }
    }

    // lock destination texture
    if (pDst->pObj && pDst->pObj->getClass()==CNvObject::NVOBJ_TEXTURE && !pDst->bIsLocked) {
        CTexture *pTex;
        pTex = pDst->pObj->getTexture();
        if (pTex && pTex->getSwizzled() && pTex->getSwizzled()->isValid()) {
            pDst->fpVidMem = pTex->getSwizzled()->getAddress();
            pTex->getSwizzled()->hwLock(CSimpleSurface::LOCK_NORMAL);
            pDst->bUseSwizzle = TRUE;
        } else if (pTex && pTex->getLinear() && pTex->getLinear()->isValid()) {
            pDst->fpVidMem = pTex->getLinear()->getAddress();
            pTex->getLinear()->hwLock(CSimpleSurface::LOCK_NORMAL);
            pDst->bUseSwizzle = FALSE;
        } else {
            DPF("Unsupported: Texture Blt Failed --- Destination not textured");
            assert(FALSE);
        }
        pDst->bIsLocked = TRUE;
    }

    // lock source texture
    if (pSrc->pObj && pSrc->pObj->getClass()==CNvObject::NVOBJ_TEXTURE && !pSrc->bIsLocked) {
        CTexture *pTex;
        pTex = pSrc->pObj->getTexture();
        if (pTex && pTex->getSwizzled() && pTex->getSwizzled()->isValid()) {
            pSrc->fpVidMem = pTex->getSwizzled()->getAddress();
            pTex->getSwizzled()->hwLock(CSimpleSurface::LOCK_NORMAL);
            pSrc->bUseSwizzle = TRUE;
        } else if (pTex && pTex->getLinear() && pTex->getLinear()->isValid()) {
            pSrc->fpVidMem = pTex->getLinear()->getAddress();
            pTex->getLinear()->hwLock(CSimpleSurface::LOCK_NORMAL);
            pSrc->bUseSwizzle = FALSE;
        } else {
            DPF("Unsupported: Texture Blt Failed --- Source not textured");
            assert(FALSE);
        }
        pSrc->bIsLocked = TRUE;
    }

    // lock destination system memory surface
    if (pDst->dwCaps & DDSCAPS_SYSTEMMEMORY && pDst->pObj &&
        pDst->pObj->getClass()==CNvObject::NVOBJ_SIMPLESURFACE &&
        !pDst->bIsLocked)
    {
        pDst->pObj->getSimpleSurface()->hwLock(CSimpleSurface::LOCK_NORMAL);
        pDst->bIsLocked = TRUE;
    }

    // lock source system memory surface
    if (pSrc->dwCaps & DDSCAPS_SYSTEMMEMORY && pSrc->pObj &&
        pSrc->pObj->getClass()==CNvObject::NVOBJ_SIMPLESURFACE &&
        !pSrc->bIsLocked)
    {
        pSrc->pObj->getSimpleSurface()->hwLock(CSimpleSurface::LOCK_NORMAL);
        pSrc->bIsLocked = TRUE;
    }

    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltControlTraffic
//      D3D function

DWORD __stdcall bltControlTraffic (LPSURFINFO pSrc, LPSURFINFO pDst)
{
    dbgTracePush ("bltControlTraffic");

    if ((getDC()->dwD3DContextCount > 0)
     && (pDriverData->nvD3DPerfData.dwPerformanceStrategy & (PS_CONTROL_TRAFFIC_16 | PS_CONTROL_TRAFFIC_32)))
    {
        CNvObject    *pNvObj;
        PNVD3DCONTEXT pContext;

        // check to see if the source is a z-buffer

        if (pSrc->pLcl)
        {
            if (pSrc->dwCaps & DDSCAPS_ZBUFFER)
            {
                pNvObj = GET_PNVOBJ(pSrc->pLcl);
                if (IS_VALID_PNVOBJ(pNvObj) && pNvObj->hasCTEnabled())
                {
                    pNvObj->tagZBlt();
                    if (pSrc->pGbl)
                    {
                        pNvObj->disableCT();
                        nvCTDecodeBuffer (pNvObj,
                                          VIDMEM_ADDR(pSrc->fpVidMem),
                                          pSrc->pGbl->ddpfSurface.dwRGBBitCount >> 3,
                                          pSrc->pGbl->lPitch,
                                          pSrc->pGbl->wWidth,
                                          pSrc->pGbl->wHeight);
                    }
                }
            }
        }  // if (pSrc->pLcl)...

        // check to see if the dest is either a z-buffer or render target

        if (pDst->pLcl)
        {
            if (pDst->dwCaps & DDSCAPS_ZBUFFER)
            {
                pNvObj = GET_PNVOBJ(pDst->pLcl);
                if (IS_VALID_PNVOBJ(pNvObj) && pNvObj->hasCTEnabled())
                {
                    pNvObj->tagZBlt();
                    if (pDst->pGbl)
                    {
                        pNvObj->disableCT();
                        nvCTDecodeBuffer (pNvObj,
                                          VIDMEM_ADDR(pDst->fpVidMem),
                                          pDst->pGbl->ddpfSurface.dwRGBBitCount >> 3,
                                          pDst->pGbl->lPitch,
                                          pDst->pGbl->wWidth,
                                          pDst->pGbl->wHeight);
                    }
                }
            }
            else
            {
                pNvObj = GET_PNVOBJ(pDst->pLcl);
                pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
                while (pContext)
                {
                    //this used to do checks against pContext->pRenderTarget
                    //however after mode switches the cached pointer to pRenderTarget may be
                    //invalid -- look up the RTHandle's pNvObj instead for this context and see if THEY match
                    //this CAN return NULL and should be OK if it does.
                    CNvObject *pRTObj = nvGetObjectFromHandle(pContext->dwDDLclID, pContext->dwRTHandle, &global.pNvSurfaceLists);
                    if ((pRTObj == pNvObj)){
                        if (pContext->pZetaBuffer)
                        {
                            nvAssert (pContext->pZetaBuffer->getWrapper());
                            if ((pDst->dwSubRectX == 0) && (pDst->dwSubRectY == 0) &&
                                (pDst->dwSubRectHeight == pDst->dwHeight) &&
                                (pDst->dwSubRectWidth == pDst->dwWidth)
                                )
                            {
                                    // Only tag this if we are doing a full blit
                                    // to the renderTarget
                                    pContext->pZetaBuffer->getWrapper()->tagFBlt();
                            }
                        }
                        break;
                    }
                    pContext = pContext->pContextNext;
                }
            }
        }  // if (pDst->pLcl)...

    }

    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltPotentialFaultHandler
//      Avoids the read ahead fault that may occur if HW attempts to prefetch
//      beyond memory which is mapped
DWORD __stdcall bltPotentialFaultHandler(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
DWORD numBottomLines = min((NV10_BLT_READ_AHEAD_BUFFER_SIZE / pSrc->dwPitch) + 1, pDst->dwSubRectHeight);
DWORD dwLastLine = pSrc->dwSubRectY + pSrc->dwSubRectHeight - numBottomLines;
LPDWORD fpSrcLastLine = (LPDWORD)(pSrc->fpVidMem + dwLastLine * pSrc->dwPitch + pSrc->dwSubRectX * pSrc->dwBytesPerPixel);
LPDWORD fpDstLastLine = (LPDWORD)(VIDMEM_ADDR(pDst->fpVidMem) + dwLastLine * pDst->dwPitch + pDst->dwSubRectX * pDst->dwBytesPerPixel);
DWORD dwLineWidth = pSrc->dwSubRectWidth * pDst->dwBytesPerPixel;

    dbgTracePush ("bltPotentialFaultHandler");

    // copy as much as we can with HW
    pDst->dwSubRectHeight -= numBottomLines;
    if (pDst->dwSubRectHeight) {
        bltSimpleCopy(pbd, pSrc, pDst);
    }
    pDst->dwSubRectHeight += numBottomLines;

    // copy the last few lines with CPU pushing
    for (DWORD j = 0; j<numBottomLines; j++) {
        for (DWORD i = 0; i<(dwLineWidth / 4); i++) {
            *(fpDstLastLine + i) = *(fpSrcLastLine + i);
        }
        switch (dwLineWidth & 0x3) {
        case 1:
            *((LPBYTE)(fpDstLastLine + dwLineWidth / 4)) = *((LPBYTE)(fpSrcLastLine + dwLineWidth / 4));
            break;
        case 2:
            *((LPWORD)(fpDstLastLine + dwLineWidth / 4)) = *((LPWORD)(fpSrcLastLine + dwLineWidth / 4));
            break;
        case 3:
            *((LPWORD)(fpDstLastLine + dwLineWidth / 4)) = *((LPWORD)(fpSrcLastLine + dwLineWidth / 4));
            *((LPBYTE)(fpDstLastLine + dwLineWidth / 4) + 2) = *((LPBYTE)(fpSrcLastLine + dwLineWidth / 4) + 2);
            break;
        case 0:
        default:
            break;
        }
        fpDstLastLine += pDst->dwPitch / 4;
        fpSrcLastLine += pSrc->dwPitch / 4;
    }
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltTexture
//      Texture blits

DWORD __stdcall bltTexture (LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst, BOOL bStretch)
{
    BOOL  bTempSrcObj = FALSE;         // use for system memory source cobj
    BOOL  bTempDstObj = FALSE;         // use for system memory destintation object

    DWORD dwRVal;
    dbgTracePush ("bltTexture");

    // check if this is a system memory surface we've never seen before
    if (pSrc->pLcl && !IS_VALID_PNVOBJ(pSrc->pObj))
    {
        // create temporary nv object for source we haven't seen before
        nvAssert (pSrc->dwCaps & DDSCAPS_SYSTEMMEMORY);
        dwRVal = nvCreateSystemMemorySurface(pSrc->pLcl);
        if (dwRVal != DD_OK)
        {
            pbd->ddRVal = dwRVal;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
        pSrc->pObj = GET_PNVOBJ (pSrc->pLcl);
        bTempSrcObj = TRUE;
    }

    // check if this is a system memory surface we've never seen before
    if (pDst->pLcl && !IS_VALID_PNVOBJ(pDst->pObj))
    {
        // create temporary nv object for destination we haven't seen before
        nvAssert (pDst->dwCaps & DDSCAPS_SYSTEMMEMORY);
        dwRVal = nvCreateSystemMemorySurface(pDst->pLcl);
        if (dwRVal != DD_OK)
        {
            pbd->ddRVal = dwRVal;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
        pDst->pObj = GET_PNVOBJ (pDst->pLcl);
        bTempDstObj = TRUE;
    }

    #ifdef DEBUG
    // make sure that we and MS still agree about where this thing is
    if (IS_VALID_PNVOBJ(pSrc->pObj)) {
        if (pSrc->pObj->getClass() == CNvObject::NVOBJ_SIMPLESURFACE) {
            CSimpleSurface *pSurf = pSrc->pObj->getSimpleSurface();
            if (pSurf->getAddress() != pSrc->fpVidMem) {
                DPF ("Surface addresses differ. NV:0x%08x, MS:0x%08x, handle:0x%08x",
                     pSurf->getAddress(), pSrc->fpVidMem, pSrc->pObj->getHandle());
                nvAssert (0);
            }
        }
    }
    #endif  // DEBUG

    if (pbd->dwFlags & DDBLT_COLORFILL) {
        DWORD dwFillColor = pbd->bltFX.dwFillColor & pDriverData->physicalColorMask;
        pbd->ddRVal = nvTextureColorFill (pDst->pObj,
                                          pDst->dwSubRectX, pDst->dwSubRectY,
                                          pDst->dwSubRectWidth, pDst->dwSubRectHeight,
                                          dwFillColor);
    }

    else if (bStretch) {
        if (IS_VALID_PNVOBJ(pSrc->pObj) && (pSrc->pObj->getClass() == CNvObject::NVOBJ_TEXTURE)) {
            pbd->ddRVal = nvTextureStretchBlit (pSrc, pDst);
        } else {
            pbd->ddRVal = DDERR_UNSUPPORTED;
        }
    }

    else {
        D3DBOX srcBox;

        srcBox.Left = pbd->rSrc.left;
        srcBox.Right = pbd->rSrc.right;
        srcBox.Top = pbd->rSrc.top;
        srcBox.Bottom = pbd->rSrc.bottom;
        srcBox.Front = 0;
        srcBox.Back = 1;

        pbd->ddRVal = nvTextureBltDX7 (NULL, pSrc->pObj, pDst->pObj, &srcBox, pbd->rDest.left, pbd->rDest.top, 0, pbd->dwFlags);
    }

    //clean up the dummy src/surface objects
    if (bTempSrcObj) {
        nvDeleteSystemMemorySurface(pSrc->pLcl);
    }

    if (bTempDstObj) {
        nvDeleteSystemMemorySurface(pDst->pLcl);
    }

    dbgTracePop();
    return ((pbd->ddRVal == DD_OK) ? DDHAL_DRIVER_HANDLED : pbd->ddRVal);
}

// -----------------------------------------------------------------------------
// bltColourFill
//      Fill with a solid colour
DWORD __stdcall bltColourFill(LPDDHAL_BLTDATA pbd, LPSURFINFO pDst, DWORD dwFillColour)
{
    dbgTracePush ("bltColourFill");

    bltSetSurfaces2D(pDst, pDst);

    if (pDriverData->bltData.dwLastRop != SRCCOPYINDEX) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_ROP) | SET_ROP_OFFSET | 0x40000);
        nvPushData(1, SRCCOPYINDEX);
        nvPusherAdjust(2);
        pDriverData->bltData.dwLastRop = SRCCOPYINDEX;
    }

    nvPushData (0, dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000);
    nvPushData (1, dwFillColour);
    nvPushData (2, asmMergeCoords(pDst->dwSubRectY, pDst->dwSubRectX));
    nvPushData (3, asmMergeCoords(pDst->dwSubRectHeight, pDst->dwSubRectWidth));
    nvPusherAdjust (4);
    nvPusherStart(TRUE);

    pbd->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// ScaleZFillDepth
//      Scales and returns Z FillDepth value.
DWORD __stdcall ScaleZFillDepth(LPDDHAL_BLTDATA pbd, LPSURFINFO pDst)
{
    __int64 i64FillDepth;

    dbgTracePush ("ScaleZFillDepth");

    nvAssert(pDst);
    nvAssert(pDst->pObj);
    nvAssert(pDst->pObj->getSimpleSurface());

    DWORD dwBPPRequested=pDst->pObj->getSimpleSurface()->getBPPRequested();
    DWORD dwBPP=pDst->pObj->getSimpleSurface()->getBPP();
    DWORD dwFill = pbd->bltFX.dwFillDepth;


    if((dwBPPRequested==2)&&(dwBPP==4))
    {
        // convert 16 bit to 24-bit number
        i64FillDepth = (__int64)((float)dwFill * CELSIUS_Z_SCALE16_INV * CELSIUS_Z_SCALE24);
        i64FillDepth = (i64FillDepth > 0xffffff) ? 0xffffff : i64FillDepth;
        i64FillDepth = (i64FillDepth < 0)        ? 0        : i64FillDepth;
        // the z-buffer occupies the upper 3 bytes.
        dwFill  = (DWORD)i64FillDepth << 8;
    }
    dbgTracePop();
    return dwFill;
}

// -----------------------------------------------------------------------------
// blt4CCto4CC
//      FOURCC to FOURCC blit
//
// Assumptions:
//      - Source and destination have identical FOURCC formats
//      - Source and destination have the same dimensions
//      - The entire surface is being moved, no subrectangles
DWORD __stdcall blt4CCto4CC(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
DWORD dwSrcOffset, dwDstOffset;
DWORD dwSrcPitch, dwDstPitch;
DWORD dwSubSample;

    dbgTracePush ("blt4CCto4CC");

    if (!(pbd->dwFlags & DDBLT_ROP) ||
         (pbd->bltFX.dwROP >> 16) != SRCCOPYINDEX) {
        dbgTracePop();
        return DDHAL_DRIVER_NOTHANDLED;
    }

    switch (pDst->dwFourCC) {
    case FOURCC_YV12:
    case FOURCC_420i:
    case FOURCC_NV12:
        dwSubSample = 1;
        break;

    case FOURCC_YVU9:
    case FOURCC_IF09:
    case FOURCC_IV32:
    case FOURCC_IV31:
        dwSubSample = 2;
        break;

    case FOURCC_YUY2:
    case FOURCC_UYVY:
    case FOURCC_UYNV:
    case FOURCC_YUNV:
        // everything already moved
        dwSubSample = 0;
        break;

    default:
        pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    dwSrcOffset = pSrc->dwOffset;
    dwSrcPitch  = pSrc->dwPitch;
    dwDstOffset = pDst->dwOffset;
    dwDstPitch  = pDst->dwPitch;
    pDriverData->bltData.dwMTMFIndex ^= 1;

    // move luma
    nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    if (pDriverData->bltData.dwMTMFIndex & 0x1) {
        nvPushData(1, NV_DD_GENERIC_MTMF_1);
        pDriverData->dDrawSpareSubchannelObject = NV_DD_GENERIC_MTMF_1;
    } else {
        nvPushData(1, NV_DD_GENERIC_MTMF_2);
        pDriverData->dDrawSpareSubchannelObject = NV_DD_GENERIC_MTMF_2;
    }
    nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x80000);
    nvPushData(3, pSrc->dwContextDma);                      // SetContextDmaBufferIn
    nvPushData(4, pDst->dwContextDma);                      // SetContextDmaBufferOut
    nvPushData(5, dDrawSubchannelOffset(NV_DD_SPARE) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
    nvPushData(6, dwSrcOffset);                             // OffsetIn
    nvPushData(7, dwDstOffset);                             // OffsetOut
    nvPushData(8, dwSrcPitch);                              // PitchIn
    nvPushData(9, dwDstPitch);                              // PitchOut
    nvPushData(10, dwSrcPitch);                             // LineLengthIn
    nvPushData(11, pDst->dwHeight);                         // LineCount
    nvPushData(12, 0x101);                                  // Format
    nvPushData(13, NV039_BUFFER_NOTIFY_WRITE_ONLY);         // BufferNotify
    nvPusherAdjust(14);

    // move chroma
    if (dwSubSample > 0) {
        pDriverData->bltData.dwMTMFIndex ^= 1;
        dwSrcOffset += dwSrcPitch * pDst->dwHeight;
        dwDstOffset += dwDstPitch * pDst->dwHeight;
        dwSrcPitch >>= dwSubSample;
        dwDstPitch >>= dwSubSample;

        // move both V and U planes in one shot
        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        if (pDriverData->bltData.dwMTMFIndex & 0x1) {
            nvPushData(1, NV_DD_GENERIC_MTMF_1);
            pDriverData->dDrawSpareSubchannelObject = NV_DD_GENERIC_MTMF_1;
        } else {
            nvPushData(1, NV_DD_GENERIC_MTMF_2);
            pDriverData->dDrawSpareSubchannelObject = NV_DD_GENERIC_MTMF_2;
        }
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x80000);
        nvPushData(3, pSrc->dwContextDma);                      // SetContextDmaBufferIn
        nvPushData(4, pDst->dwContextDma);                      // SetContextDmaBufferOut
        nvPushData(5, dDrawSubchannelOffset(NV_DD_SPARE) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        nvPushData(6, dwSrcOffset);                             // OffsetIn
        nvPushData(7, dwDstOffset);                             // OffsetOut
        nvPushData(8, dwSrcPitch);                              // PitchIn
        nvPushData(9, dwDstPitch);                              // PitchOut
        nvPushData(10, dwSrcPitch);                             // LineLengthIn
        nvPushData(11, pDst->dwHeight);                         // LineCount
        nvPushData(12, 0x101);                                  // Format
        nvPushData(13, NV039_BUFFER_NOTIFY_WRITE_ONLY);         // BufferNotify
        nvPusherAdjust(14);
    }
    nvPusherStart(TRUE);

    pbd->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltSlowVtoSBlt
//      Handles the fallback case when context DMA's are thrashing.  Simple copy,
//      no stretching, no ROPs, no colour key, no FOURCC to RGB
DWORD __stdcall bltSlowVtoSBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
DWORD i, j;
FLATPTR fpSrc, fpDst;
DWORD dwWidth, dwWidth1, dwWidth2;

    dbgTracePush ("bltSlowVtoSBlt");

    getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);

    // calculate source and destination offsets
    fpSrc  = pSrc->fpVidMem + pSrc->dwSubRectY * pSrc->dwPitch;
    fpSrc += (pSrc->dwSubRectX  * pSrc->dwBytesPerPixel);

    fpDst  = pDst->fpVidMem + pDst->dwSubRectY * pDst->dwPitch;
    fpDst += (pDst->dwSubRectX  * pDst->dwBytesPerPixel);

#ifdef WINNT
    if (pSrc->dwCaps & DDSCAPS_LOCALVIDMEM) {
        fpSrc += (DWORD) ppdev->pjFrameBufbase;
    }
    if (pDst->dwCaps & DDSCAPS_LOCALVIDMEM) {
        fpDst += (DWORD) ppdev->pjFrameBufbase;
    }
#endif

    // calculate widths
    // dwWidth1 is the width in DWORDs
    // dwWidth2 is the width in bytes of the rightmost pixel to the nearest 32 bit alignment on the left
    dwWidth = pSrc->dwSubRectWidth * pSrc->dwBytesPerPixel;
    dwWidth1 = dwWidth >> 2;
    dwWidth2 = dwWidth - (dwWidth1 << 2);

    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_MMX) {
        DWORD dwSrcPitch = pSrc->dwPitch;
        DWORD dwDstPitch = pDst->dwPitch;
        for (j = 0; j < pDst->dwSubRectHeight; j++) {
            _asm {
                    mov     edi, fpDst
                    mov     esi, fpSrc
                    mov     eax, dwWidth
                    mov     ecx, dwWidth1
                    and     eax, 7
                    shr     ecx, 1
                    jz      checkd

            qloop:  movq    mm0, [esi]
                    add     esi, 8
                    movq    [edi], mm0
                    add     edi, 8
                    dec     ecx
                    jnz     qloop

            checkd: cmp     eax, 4
                    jb      checkw
                    mov     edx, [esi]
                    add     esi, 4
                    sub     eax, 4
                    mov     [edi], edx
                    add     edi, 4
            checkw: cmp     eax, 2
                    jb      checkb
                    mov     dx, [esi]
                    add     esi, 2
                    sub     eax, 2
                    mov     [edi], dx
                    add     edi, 2
            checkb: test    eax, eax
                    jz      endofline
                    mov     dl, [esi]
                    mov     [edi], dl

            endofline:
                    mov     eax, fpSrc
                    mov     ebx, fpDst
                    add     eax, dwSrcPitch
                    add     ebx, dwDstPitch
                    mov     fpSrc, eax
                    mov     fpDst, ebx
            }
        }
        _asm emms;
    } else {
        for (j = 0; j < pDst->dwSubRectHeight; j++) {
            for (i = 0; i < dwWidth1; i++) {
                *((LPDWORD)fpDst + i) = *((LPDWORD)fpSrc + i);
            }
            switch (dwWidth2) {
            case 1: *((LPBYTE)fpDst + (dwWidth1 << 2)) = *((LPBYTE)fpSrc + (dwWidth1 << 2)); break;
            case 2: *((LPWORD)fpDst + (dwWidth1 << 1)) = *((LPWORD)fpSrc + (dwWidth1 << 1)); break;
            case 3: *((LPWORD)fpDst + (dwWidth1 << 1)) = *((LPWORD)fpSrc + (dwWidth1 << 1));
                    *((LPBYTE)fpDst + (dwWidth1 << 2) + 2) = *((LPBYTE)fpSrc + (dwWidth1 << 2) + 2); break;
            default: break;
            }
            fpDst += pDst->dwPitch;
            fpSrc += pSrc->dwPitch;
        }
    }

    pbd->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltSlowStoVBlt
//      Handles the fallback case when context DMA's are thrashing.  Use stretched_image
//      instead of context DMA's
DWORD __stdcall bltSlowStoVBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
DWORD dwSrcFormat;
DWORD dwSizeIn, dwSizeOut, dwClipOut, dwPointOut;
DWORD dwDeltaX, dwDeltaY;
DWORD dwSrcLineSize;
DWORD i, j;
LPDWORD lpSrcPtr;
DWORD dwAlignAdjust;
DWORD dwDstX, dwDstY;

    dbgTracePush ("bltSlowStoVBlt");

    bltSetSurfaces2D(pSrc, pDst);

    switch (pSrc->dwBytesPerPixel) {
    case 1: dwSrcFormat = NV066_SET_COLOR_FORMAT_LE_X8R8G8B8; break;
    case 2: dwSrcFormat = NV066_SET_COLOR_FORMAT_LE_R5G6B5;   break;
    case 4: dwSrcFormat = NV066_SET_COLOR_FORMAT_LE_X8R8G8B8; break;
    default:
        dbgTracePop();
        return DDHAL_DRIVER_NOTHANDLED;
    }

    dwSizeIn  = pSrc->dwSubRectWidth | (pSrc->dwSubRectHeight << 16);
    dwClipOut = pDst->dwSubRectX     | (pDst->dwSubRectY << 16);
    dwSizeOut = pDst->dwSubRectWidth | (pDst->dwSubRectHeight << 16);
    dwDstX    = pDst->dwSubRectX;
    dwDstY    = pDst->dwSubRectY;

    dwDeltaX = (pDst->dwSubRectWidth << 20)  / pSrc->dwSubRectWidth + 1;
    dwDeltaY = (pDst->dwSubRectHeight << 20) / pSrc->dwSubRectHeight + 1;

    if (pbd->bltFX.dwDDFX & DDBLTFX_MIRRORLEFTRIGHT) {
        dwDeltaX = -(int)dwDeltaX;
        pbd->bltFX.dwDDFX &= ~DDBLTFX_MIRRORLEFTRIGHT;
        dwDstX += pDst->dwSubRectWidth + 1;
    }
    if (pbd->bltFX.dwDDFX & DDBLTFX_MIRRORUPDOWN) {
        dwDeltaY = -(int)dwDeltaY;
        pbd->bltFX.dwDDFX &= ~DDBLTFX_MIRRORUPDOWN;
        dwDstY += pDst->dwSubRectHeight + 1;
    }
    dwPointOut = (dwDstY << 20) | (dwDstX << 4);

    if (pbd->dwFlags & DDBLT_KEYSRCOVERRIDE) {
        DWORD dwColorKey  = pbd->bltFX.ddckSrcColorkey.dwColorSpaceLowValue;
        DWORD dwAlphaMask[5] = { 0xDEADBEAF, NV_ALPHA_1_008, NV_ALPHA_1_016, 0xDEADBEAF, NV_ALPHA_1_032 };
        nvAssert((pDst->dwBytesPerPixel >= 1) && (pDst->dwBytesPerPixel <= 4));

        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData(1, NV_DD_CONTEXT_COLOR_KEY);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) + SET_TRANSCOLOR_OFFSET | 0x40000);
        nvPushData(3, (dwColorKey & pDriverData->physicalColorMask) | dwAlphaMask[pDst->dwBytesPerPixel]);
        nvPusherAdjust(4);
    }

    nvPushData(0, dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData(1, NV_DD_GENERIC_STRETCHED_IMAGE);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_STRETCH) | STRETCHED_IMAGE_COLOR_FORMAT_OFFSET | 0x1C0000);
    nvPushData(3, dwSrcFormat);             // SetColorFormat
    nvPushData(4, dwSizeIn);                // SizeIn
    nvPushData(5, dwDeltaX);                // DxDs
    nvPushData(6, dwDeltaY);                // DyDt
    nvPushData(7, dwClipOut);               // ClipPoint
    nvPushData(8, dwSizeOut);               // ClipSize
    nvPushData(9, dwPointOut);              // Point12d4
    nvPusherAdjust(10);

    dwSrcLineSize = pSrc->dwSubRectWidth * pSrc->dwBytesPerPixel;
    lpSrcPtr = (LPDWORD)(pSrc->fpVidMem + pSrc->dwSubRectY * pSrc->dwPitch + pSrc->dwSubRectX * pSrc->dwBytesPerPixel);
#ifdef WINNT
    if (pSrc->dwCaps & DDSCAPS_LOCALVIDMEM) {
        lpSrcPtr = (LPDWORD)((DWORD)lpSrcPtr + (DWORD)ppdev->pjFrameBufbase);
    }
#endif

    for (j = 0, dwAlignAdjust = 0; j < pSrc->dwSubRectHeight; j++) {
        int nBytesRemaining;
        LPDWORD lpData;

        nBytesRemaining = dwSrcLineSize - dwAlignAdjust;
        lpData = (LPDWORD)((DWORD)lpSrcPtr + dwAlignAdjust);
        while (nBytesRemaining > 3)
        {
            DWORD dwChunk, dwIndex;
            // dwChunk is measured in DWORDS
            dwChunk = min(nBytesRemaining >> 2, g_iPusherThreshold); // pusher can't accept more than 63 dwords
            nvPushData(0, dDrawSubchannelOffset(NV_DD_STRETCH) | STRETCHED_IMAGE_COLOR_OFFSET | (dwChunk << 18));
            for (i = 0, dwIndex = 1; i < dwChunk; i++) {
                // shovel data into push buffer
                nvPushData(dwIndex++, *lpData++);
            }
            nvPusherAdjust(dwIndex);
            nBytesRemaining -= dwChunk << 2;
        }
        if (nBytesRemaining) {
            // deal with unaligned lengths
            DWORD dwLastBytes;
            switch (nBytesRemaining) {
            case 1: dwLastBytes = *((LPBYTE)lpData); break;
            case 2: dwLastBytes = *((LPWORD)lpData); break;
            case 3: dwLastBytes = *((LPWORD)lpData) | (*((LPBYTE)lpData + 2) << 16); break;
            default: break;
            }
            if ((j + 1) < pSrc->dwSubRectHeight) {
                // need to pack pixels from next line
                LPDWORD lpNextLine;
                lpNextLine = lpSrcPtr + (pSrc->dwPitch >> 2);
                switch (nBytesRemaining) {
                case 1:
                    dwLastBytes |= ((DWORD)*((LPDWORD)lpNextLine) << 8) | ((DWORD)*((LPBYTE)lpNextLine + 2) << 24);
                    dwAlignAdjust = 3;
                    break;
                case 2:
                    dwLastBytes |= (DWORD)*((LPWORD)lpNextLine) << 16;
                    dwAlignAdjust = 2;
                    break;
                case 3:
                    dwLastBytes |= (DWORD)*((LPBYTE)lpNextLine) << 24;
                    dwAlignAdjust = 1;
                    break;
                default: break;
                }
                if (dwAlignAdjust > dwSrcLineSize) {
                    DWORD dwBytesNeededToPackTheDword = dwAlignAdjust - dwSrcLineSize;
                    lpNextLine += (pSrc->dwPitch >> 2);
                    // very small width line (one pixel), need to pack in pixels from next line(s) too
                    if ((j + 2) < pSrc->dwSubRectHeight) {
                        switch (dwBytesNeededToPackTheDword) {
                        case 1:
                            dwLastBytes &= 0x00FFFFFF;
                            dwLastBytes |= (DWORD)*((LPBYTE)lpNextLine) << 24;
                            dwAlignAdjust = 1;
                            break;
                        case 2:
                            dwLastBytes &= 0x0000FFFF;
                            dwLastBytes |= (DWORD)*((LPWORD)lpNextLine) << 16;
                            dwAlignAdjust = 2;
                            break;
                        }
                    }
                    if (dwAlignAdjust > dwSrcLineSize) {
                        lpNextLine += (pSrc->dwPitch >> 2);
                        if ((j + 3) < pSrc->dwSubRectHeight) {
                            dwLastBytes &= 0x00FFFFFF;
                            dwLastBytes |= (DWORD)*((LPBYTE)lpNextLine) << 24;
                            dwAlignAdjust = 1;
                        }
                        // we skipped two lines
                        lpSrcPtr += (pSrc->dwPitch >> 1);
                        j += 2;
                    } else {
                        // we skipped a line
                        lpSrcPtr += (pSrc->dwPitch >> 2);
                        j++;
                    }
                }
            }
            nvPushData(0, dDrawSubchannelOffset(NV_DD_STRETCH) | STRETCHED_IMAGE_COLOR_OFFSET | 0x40000);
            nvPushData(1, dwLastBytes);
            nvPusherAdjust(2);
        } else if (nBytesRemaining == 0) {
            dwAlignAdjust = 0;
        }
        lpSrcPtr += (pSrc->dwPitch >> 2);
    }
    nvPusherStart(TRUE);

    pbd->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltSimpleVtoVBlt
//      Simple video to video, no stretch, no mirror blit
DWORD __stdcall bltSimpleVtoVBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
    dbgTracePush ("bltSimpleVtoVBlt");

    bltSetSurfaces2D(pSrc, pDst);

    nvPushData (0, dDrawSubchannelOffset(NV_DD_BLIT) | BLIT_POINT_IN_OFFSET | 0xC0000);
    nvPushData (1, (pSrc->dwSubRectY << 16) | pSrc->dwSubRectX);          // ControlPointIn
    nvPushData (2, (pDst->dwSubRectY << 16) | pDst->dwSubRectX);          // ControlPointOut
    nvPushData (3, (pDst->dwSubRectHeight << 16) | pDst->dwSubRectWidth); // Size
    nvPusherAdjust(4);
    nvPusherStart(TRUE);

    pbd->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltSimpleCopy
//      Simple rectangular copy.  No rop, no colour key, no bit depth dependency.
//      Will not do system to system copies.
DWORD __stdcall bltSimpleCopy(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
DWORD dwSrcOffset, dwDstOffset;

    dbgTracePush ("bltSimpleCopy");

    dwSrcOffset = pSrc->dwOffset + pSrc->dwSubRectY * pSrc->dwPitch + pSrc->dwSubRectX * pSrc->dwBytesPerPixel;
    dwDstOffset = pDst->dwOffset + pDst->dwSubRectY * pDst->dwPitch + pDst->dwSubRectX * pDst->dwBytesPerPixel;
    pDriverData->bltData.dwMTMFIndex ^= 1;   // alternate odd and even

    nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    if (pDriverData->bltData.dwMTMFIndex & 0x1) {
        nvPushData(1, NV_DD_GENERIC_MTMF_1);
        pDriverData->dDrawSpareSubchannelObject = NV_DD_GENERIC_MTMF_1;
    } else {
        nvPushData(1, NV_DD_GENERIC_MTMF_2);
        pDriverData->dDrawSpareSubchannelObject = NV_DD_GENERIC_MTMF_2;
    }
    nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x80000);
    nvPushData(3, pSrc->dwContextDma);                      // SetContextDmaBufferIn
    nvPushData(4, pDst->dwContextDma);                      // SetContextDmaBufferOut
    nvPushData(5, dDrawSubchannelOffset(NV_DD_SPARE) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
    nvPushData(6, dwSrcOffset);                             // OffsetIn
    nvPushData(7, dwDstOffset);                             // OffsetOut
    nvPushData(8, pSrc->dwPitch);                           // PitchIn
    nvPushData(9, pDst->dwPitch);                           // PitchOut
    nvPushData(10, pDst->dwSubRectWidth * pDst->dwBytesPerPixel); // LineLengthIn
    nvPushData(11, pDst->dwSubRectHeight);                  // LineCount
    nvPushData(12, 0x101);                                  // Format
    nvPushData(13, NV039_BUFFER_NOTIFY_WRITE_ONLY);         // BufferNotify
    nvPusherAdjust(14);
    nvPusherStart(TRUE);

    pbd->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// blt8bppStrBlt
//      Special case for 8 bpp strblts.  Pixel replication only, no interpolation
DWORD __stdcall blt8bppStrBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst)
{
DWORD xAcc, yAcc;
DWORD xInc, yInc;
DWORD i, j;
SURFINFO src, ws;
DWORD ex, why, size;
unsigned iPrecisionFactor = 20;

    dbgTracePush ("blt8bppStrBlt");

    xAcc = yAcc = 0;
    xInc = (pSrc->dwSubRectWidth << iPrecisionFactor)  / pDst->dwSubRectWidth;
    yInc = (pSrc->dwSubRectHeight << iPrecisionFactor) / pDst->dwSubRectHeight;

    // create intermediate workspace
    ws = *pSrc;
    ws.dwSubRectX = 0;
    ws.dwSubRectY = 0;
    ws.dwSubRectWidth  = max(pSrc->dwSubRectWidth, pDst->dwSubRectWidth);
    ws.dwSubRectHeight = max(pSrc->dwSubRectHeight, pDst->dwSubRectHeight);
    ws.dwPitch = (ws.dwWidth + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

    if (pSrc->dwCaps & DDSCAPS_LOCALVIDMEM) {
        // video to video
        ws.dwSubRectHeight *= 2;  // we need 2x as much work space
        if (bltAllocWorkSpace(pbd, &ws, BLTWS_REQUIRED_VIDEO, pbd->lpDD) != DDHAL_DRIVER_HANDLED) {
            return DDHAL_DRIVER_NOTHANDLED;
        }
        ws.dwSubRectHeight /= 2;  // restore proper height
        ws.dwOffset += ws.dwPitch * ws.dwSubRectHeight;     // leave top half undisturbed for use by blit32
        bltSetSurfaces2D(pSrc, &ws);
    } else {
        // system to video
        ws.dwSubRectHeight *= 3;  // we need 3x as much work space
        if (bltAllocWorkSpace(pbd, &ws, BLTWS_REQUIRED_VIDEO, pbd->lpDD) != DDHAL_DRIVER_HANDLED) {
            return DDHAL_DRIVER_NOTHANDLED;
        }
        ws.dwSubRectHeight /= 3;  // restore proper height

        // first copy it to video memory
        ws.dwOffset += ws.dwPitch * ws.dwSubRectHeight;     // leave top third undisturbed for use by blit32
        ws.dwSubRectWidth = pSrc->dwSubRectWidth;
        ws.dwSubRectHeight = pSrc->dwSubRectHeight;
        bltSimpleCopy(pbd, pSrc, &ws);
        src = ws;
        pSrc = &src;                                        // mid third of ws is our new source
        ws.dwOffset += ws.dwPitch * ws.dwSubRectHeight;     // bottom third is our ws
        bltSetSurfaces2D(&src, &ws);
    }

    // disable color key
    if (pDriverData->bltData.dwLastColourKey != 0xFFFFFFFF) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData(1, NV_DD_CONTEXT_COLOR_KEY);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | SET_TRANSCOLOR_OFFSET | 0x40000);
        nvPushData(3, 0);
        nvPusherAdjust(4);
        pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;
        pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_COLOR_KEY;
    }

    // first do the horizontal stretch one strip at a time
    // (better for memory bw when upscaling)
    why = pSrc->dwSubRectY << 16;
    size = (pSrc->dwSubRectHeight << 16) | 1;
    for (i=0, j=0; i<pDst->dwSubRectWidth; i++) {
        nvPushData (j++, dDrawSubchannelOffset(NV_DD_BLIT) | BLIT_POINT_IN_OFFSET | 0xC0000);
        nvPushData (j++, why | (pSrc->dwSubRectX + (xAcc >> iPrecisionFactor)));// ControlPointIn
        nvPushData (j++, 0 | i);                                      // ControlPointOut
        nvPushData (j++, size);                                       // Size
        xAcc += xInc;
        if (j >= g_iPusherThreshold) {
            nvPusherAdjust(j);
            j = 0;
        }
    }
    if (j) nvPusherAdjust(j);
    nvPusherStart(TRUE);

    // do the vertical stretching
    bltSetSurfaces2D(&ws, pDst);
    ex = pDst->dwSubRectX;
    size = (1 << 16) | pDst->dwSubRectWidth;
    for (i=0, j=0; i<pDst->dwSubRectHeight; i++) {
        nvPushData (j++, dDrawSubchannelOffset(NV_DD_BLIT) | BLIT_POINT_IN_OFFSET | 0xC0000);
        nvPushData (j++, ((yAcc >> iPrecisionFactor) << 16) | 0);   // ControlPointIn
        nvPushData (j++, ((pDst->dwSubRectY + i) << 16) | ex);      // ControlPointOut
        nvPushData (j++, size);                                     // Size
        yAcc += yInc;
        if (j >= g_iPusherThreshold) {
            nvPusherAdjust(j);
            j = 0;
        }
    }
    if (j) nvPusherAdjust(j);
    nvPusherStart(TRUE);

    pbd->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// bltStrBlt
//      Handles stretchblits, system to video blits, and FOURCC to RGB blits.
//      Does not handle stretched 8 bpp sources, nor color keyed blits.
//      Assumes this is non-overlapping.
DWORD __stdcall bltStrBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst, BOOL bFilter)
{
DWORD dwSrcFormat;
DWORD dwDeltaX, dwDeltaY;
DWORD dwDstPoint, dwDstSize, dwClipPoint;
DWORD dwSrcPoint, dwSrcWidth, dwSrcSize;
DWORD dwRetVal;

    dbgTracePush ("bltStrBlt");

    switch (pSrc->dwFourCC) {
    case 0:
        switch (pSrc->dwBytesPerPixel) {
        case 1:
            if ((pbd->bltFX.dwROP >> 16) != SRCCOPYINDEX ||
                (pbd->bltFX.dwDDFX & DDBLTFX_MIRRORLEFTRIGHT) ||
                (pbd->bltFX.dwDDFX & DDBLTFX_MIRRORUPDOWN)) {
                return DDHAL_DRIVER_NOTHANDLED;
            }
            dwRetVal = blt8bppStrBlt(pbd, pSrc, pDst);
            dbgTracePop();
            return dwRetVal;

        case 2:
            dwSrcFormat = NV063_SET_COLOR_FORMAT_LE_R5G6B5;
            break;

        case 4:
            dwSrcFormat = NV063_SET_COLOR_FORMAT_LE_X8R8G8B8;
            break;

        default:
            dbgTracePop();
            return DDHAL_DRIVER_NOTHANDLED;
        }
        break;

    case FOURCC_YUY2:
    case FOURCC_YUNV:
    case FOURCC_NV12:
        dwSrcFormat = NV089_SET_COLOR_FORMAT_LE_CR8YB8CB8YA8;
        if (pSrc->dwFourCC == FOURCC_NV12) { // Offset to YUY2 portion of surface, convert pitch to YUY2 pitch
            // TODO: make sure surface FOURCC format conversion is up to date before blitting
            pSrc->dwOffset += (pSrc->dwPitch * pSrc->dwHeight);
            pSrc->dwOffset += (pSrc->dwPitch * (pSrc->dwHeight >> 1));
            pSrc->dwOffset += (pSrc->dwPitch * 8) + 256;
            pSrc->dwPitch = (pSrc->dwWidth + 3) & ~3;
            pSrc->dwPitch = ((pSrc->dwPitch << 1) + 127) & ~127;
            // TODO: modify surface height when source is single field 1080i 
        }
        break;

    case FOURCC_UYVY:
    case FOURCC_UYNV:
        dwSrcFormat = NV089_SET_COLOR_FORMAT_LE_YB8CR8YA8CB8;
        break;

    case FOURCC_YV12:
    case FOURCC_420i:
        // TODO: convert YUV12 to YUV422
    case FOURCC_IF09:
    case FOURCC_YVU9:
    case FOURCC_IV32:
    case FOURCC_IV31:
        // TODO: convert YUV9 to YUV422
    default:
        dbgTracePop();
        return DDHAL_DRIVER_NOTHANDLED;
    }

    // scaled image doesn't like big subrectangle offsets, adjust
    // NV20 offsets must be aligned to pitch*4
    pSrc->dwOffset += (pSrc->dwSubRectY & 0xFFFFFFFC) * pSrc->dwPitch;
    pSrc->dwSubRectY &= 0x3;
    pSrc->dwOffset += (pSrc->dwSubRectX  * pSrc->dwBytesPerPixel) & ~NV_BYTE_ALIGNMENT_PAD;
    pSrc->dwSubRectX &= (NV_BYTE_ALIGNMENT / pSrc->dwBytesPerPixel) - 1;

    pDst->dwOffset += (pDst->dwSubRectY & 0xFFFFFFFC) * pDst->dwPitch;
    pDst->dwSubRectY &= 0x3;
    pDst->dwOffset += (pDst->dwSubRectX  * pDst->dwBytesPerPixel) & ~NV_BYTE_ALIGNMENT_PAD;
    pDst->dwSubRectX &= (NV_BYTE_ALIGNMENT / pDst->dwBytesPerPixel) - 1;

    bltSetSurfaces2D(pSrc, pDst);

    if (bFilter) {
        if (pDst->dwSubRectWidth <= 1) {
            dwDeltaX = 1 << 20;
        } else {
            dwDeltaX = ((pSrc->dwSubRectWidth - 1) << 20)  / (pDst->dwSubRectWidth - 1);    // 12.20 fixed point
        }
        if (pDst->dwSubRectHeight <= 1) {
            dwDeltaY = 1 << 20;
        } else {
            dwDeltaY = ((pSrc->dwSubRectHeight - 1) << 20) / (pDst->dwSubRectHeight - 1);   // 12.20 fixed point
        }
    } else {
        dwDeltaX = (pSrc->dwSubRectWidth  << 20) / pDst->dwSubRectWidth;    // 12.20 fixed point
        dwDeltaY = (pSrc->dwSubRectHeight << 20) / pDst->dwSubRectHeight;   // 12.20 fixed point
    }
    dwDstPoint = dwClipPoint = (pDst->dwSubRectY << 16) | pDst->dwSubRectX;
    dwDstSize  = (pDst->dwSubRectHeight << 16) | pDst->dwSubRectWidth;
    dwSrcPoint = ((pSrc->dwSubRectY << 16)     | pSrc->dwSubRectX) << 4;
    dwSrcWidth = pSrc->dwSubRectWidth;
    if ((dwSrcWidth + pSrc->dwSubRectX) > 2046) dwSrcWidth = 2046 - pSrc->dwSubRectX; // hardware limit
    dwSrcSize  = ((pSrc->dwSubRectHeight + pSrc->dwSubRectY) << 16) | ((dwSrcWidth + pSrc->dwSubRectX + 1) & ~1);
    // add pixel biases if we are scaling, except when we are using colour key (WHQL issue)
    if (bFilter && !(pbd->dwFlags & DDBLT_KEYSRCOVERRIDE)) {
        if (dwDeltaX != 0x100000) {
            dwSrcPoint = (dwSrcPoint & 0xFFFF0000) | ((dwSrcPoint & 0xFFFF) - 4);
        }
        if (dwDeltaY != 0x100000) {
            dwSrcPoint = ((dwSrcPoint & 0xFFFF0000) - 0x00080000) | (dwSrcPoint & 0xFFFF);
        }
    } else {
        dwSrcPoint += (dwDeltaX >> 17);
        dwSrcPoint += (dwDeltaY >> 17) << 16;
    }

    if (pbd->bltFX.dwDDFX & DDBLTFX_MIRRORLEFTRIGHT) {
        dwDeltaX = -(int)dwDeltaX;
        pbd->bltFX.dwDDFX &= ~DDBLTFX_MIRRORLEFTRIGHT;
        dwSrcPoint = ((dwSrcPoint & 0x0000FFFF) + ((pSrc->dwSubRectWidth + 1) << 4)) | (dwSrcPoint & 0xFFFF0000);
    }
    if (pbd->bltFX.dwDDFX & DDBLTFX_MIRRORUPDOWN) {
        dwDeltaY = -(int)dwDeltaY;
        pbd->bltFX.dwDDFX &= ~DDBLTFX_MIRRORUPDOWN;
        dwSrcPoint = ((dwSrcPoint & 0xFFFF0000) + ((pSrc->dwSubRectHeight + 1) << 20)) | (dwSrcPoint & 0x0000FFFF);
    }

    nvPushData(0, dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData(1, NV_DD_SCALED_IMAGE_IID);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000);
    nvPushData(3, pSrc->dwContextDma);      // SetContextDmaImage
    nvPushData(4, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CONTEXT_SURFACE_OFFSET | 0x40000);
    if (pDst->bUseSwizzle) {
        nvPushData(5, D3D_CONTEXT_SURFACE_SWIZZLED);    // SetContextSurface
    } else {
        nvPushData(5, NV_DD_SURFACES_2D);               // SetContextSurface
    }
    nvPushData(6, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_SET_COLOR_FORMAT | 0x40000);
    nvPushData(7, dwSrcFormat);             // SetColorFormat
    nvPushData(8, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CLIPPOINT_OFFSET | 0x180000);
    nvPushData(9, dwClipPoint);             // ClipPoint
    nvPushData(10, dwDstSize);              // ClipSize
    nvPushData(11, dwDstPoint);             // ImageOutPoint
    nvPushData(12, dwDstSize);              // ImageOutSize
    nvPushData(13, dwDeltaX);               // DsDx
    nvPushData(14, dwDeltaY);               // DtDy

    nvPushData(15, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0x100000);
    nvPushData(16, dwSrcSize);              // ImageInSize
    if (bFilter) {
        nvPushData(17, (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |     // ImageInFormat
                       (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                       pSrc->dwPitch);
    } else {
        nvPushData(17, (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24) |     // ImageInFormat
                       (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16) |
                       pSrc->dwPitch);
    }
    nvPushData(18, pSrc->dwOffset);         // ImageInOffset
    nvPushData(19, dwSrcPoint);             // ImageInPoint
    nvPusherAdjust(20);

    nvPusherStart(TRUE);

    pbd->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}


#endif  // NVARCH >= 0x04
